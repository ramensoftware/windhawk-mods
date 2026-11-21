// ==WindhawkMod==
// @id              dynamic-taskbar-live-alerts
// @name            Modern Taskbar for Windows 11 (Dynamic Taskbar)
// @name:vi-VN      Modern Taskbar for Windows 11 (Dynamic Taskbar)
// @description     Native XAML Optical Liquid Glass Split Taskbars, Media Album Art + Waveform, Task Progress %, <10ms Lock/Clipboard/Alt+F4/Battery Milestones & Interactive Sliders.
// @description:vi-VN Mặt kính Liquid Glass gắn trực tiếp cây XAML Taskbar, Phát nhạc (Ảnh bìa + Sóng nhạc), Tiến trình %, Phím khóa/Clipboard/Alt+F4/Mốc Pin tức thì <10ms & Kéo chuột Âm lượng/Độ sáng.
// @version         8.1.0
// @author          caotu2005
// @github          https://github.com/caotu2005
// @include         explorer.exe
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -ldwmapi -lgdi32 -luser32 -lshell32 -lshlwapi -ladvapi32 -lole32 -loleaut32 -lwindowscodecs -luuid -lruntimeobject -lsetupapi -lpsapi -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Taskbar for Windows 11 (Dynamic Taskbar)

> ℹ️ **Compatibility Note (Smart App Control) / Lưu ý tương thích:**
> - **🌐 English:** Like all Windhawk mods, this mod is compiled locally into an unsigned `.dll` file and loaded into `explorer.exe`. If Windows **Smart App Control** is active on your PC, Windows will block the compiled `.dll` and the mod will not load. See [Windhawk's documentation and guidance on Smart App Control](https://github.com/ramensoftware/windhawk/issues/696) for details.
> - **🇻🇳 Tiếng Việt:** Giống như mọi bản mod trên Windhawk, mod này được biên dịch cục bộ thành tệp `.dll` không có chữ ký số để nạp vào `explorer.exe`. Nếu máy đang bật **Smart App Control (Kiểm soát Ứng dụng Thông minh)**, Windows sẽ chặn tệp `.dll` này khiến mod không thể hoạt động. Xem thêm [hướng dẫn và thảo luận chính thức của Windhawk về Smart App Control](https://github.com/ramensoftware/windhawk/issues/696).

---

## 🌐 Main Features (English)

### 1. Native XAML Liquid Glass Split Taskbar (All Edges)
- Splits the Windows 11 taskbar into optical **Liquid Glass** pills (**Apps Pill** + **System Tray Pill**) injected directly into the native `Shell_TrayWnd` XAML tree for zero flicker and accurate Z-order.
- Supports **Horizontal** (Bottom / Top) and **Vertical** (Left / Right) taskbars with real-time 60fps `LayoutUpdated` resizing, circular task buttons, pill-styled system tray buttons, and automatic light/dark contrast adaptation (including Windows 11 25H2+ battery icons).
- Automatically shifts or hides the default Windows Widgets button when space is needed for the corner Live Activity pill.

### 2. Dynamic Live Alerts on Taskbar
- **Flexible Positioning & Non-Blocking UX:** Morphs inline on the taskbar (hiding only the Apps Pill while keeping the System Tray visible) or floats adjacent to the taskbar (`popupPosition`) with click-through for read-only alerts. Includes **Auto-Dismiss on Hover** (`dismissOnHover`) and shorter durations (`1.3s` default) for quick actions so taskbar clicks are never obstructed.
- **Interactive Hardware Sliders:** Drag or scroll directly on the **Volume** (click icon to mute) and **Screen Brightness** Live Alert sliders, with optional suppression of the default Windows hardware OSD.
- **Instant Keyboard & Clipboard Feedback (`<10ms`):** `Caps Lock`, `Num Lock`, `Scroll Lock`, `Insert`, `Copy` / `Cut` / `Paste` (full character count, images, and files), and `Alt + F4` application closed alerts.
- **System, Power & Device Alerts:**
  - **Battery & Power:** Charging milestones (`80%`, `90%`, `100%`), Low Battery milestones (`50%`, `20%`, `10%`, `5%`), AC/Battery switch, and Battery Saver mode.
  - **Connectivity & Peripherals:** Wi-Fi / Wired LAN, VPN connect/disconnect, Bluetooth devices (with battery % when available), and USB plug/unplug.
  - **System & Security:** File deleted / moved to Recycle Bin, elevated Administrator (UAC) app launch, High RAM (`80%` / `90%` / `95%`) & Low Disk (`<10%` free) warnings, and Do Not Disturb / Focus Mode.

### 3. Interactive Corner Live Activity Pill (`< >` Switchable)
- **Media Playback (`GSMTC`):** Displays decoded album cover art + animated equalizer waveform (click to focus player or toggle play/pause).
- **Task Progress (`ITaskbarList3`):** Shows an app-specific icon and live percentage (e.g., `77%`) for browser downloads, File Explorer copies/extractions, archive tools, and installers.
- **VPN, Mobile Hotspot, Clock & Privacy:** Quick-access pills for active VPN, Mobile Hotspot (with connected device count), Windows Clock Timer & Stopwatch, and Microphone / Camera / Location indicators (automatically hides duplicate system tray privacy icons).
- **True Fullscreen-Aware:** Automatically hides only when a true borderless or exclusive D3D fullscreen app is active on that monitor, while remaining visible for normal maximized windows with titlebars.

---

## 🇻🇳 Tính năng chính (Tiếng Việt)

### 1. Taskbar Tách Đôi Liquid Glass gắn trực tiếp cây XAML (Mọi cạnh)
- Tách thanh Taskbar Windows 11 thành các viên thuốc kính **Liquid Glass** (**Thanh ứng dụng** + **Khay hệ thống**) gắn trực tiếp vào cây XAML gốc của `Shell_TrayWnd`, không nhấp nháy và không lệch lớp hiển thị.
- Hỗ trợ đầy đủ cả **Taskbar Ngang** (Dưới / Trên) và **Taskbar Dọc** (Trái / Phải), tự động co dãn 60fps theo `LayoutUpdated`, bo tròn nút ứng dụng, bo dạng viên thuốc cho Khay hệ thống và tự động đảo màu tương phản sáng/tối (bao gồm biểu tượng pin Windows 11 25H2+).
- Tự động dịch chuyển hoặc ẩn nút Widget mặc định của Windows khi cần nhường chỗ cho Live Activity ở góc.

### 2. Thông báo Live Alerts trực tiếp trên Taskbar
- **Linh hoạt vị trí & không cản thao tác:** Biến hình ngay tại vị trí Thanh ứng dụng (giữ nguyên Khay trạng thái) hoặc chọn nổi sát mép Taskbar (`popupPosition`, xuyên chuột cho thông báo chỉ đọc). Hỗ trợ **tự thu gọn khi rê chuột vào** (`dismissOnHover`) và thời lượng riêng ngắn gọn (`1.3s`) cho thao tác nhanh để không vướng khi bấm chuyển ứng dụng.
- **Thanh trượt tương tác bằng chuột:** Kéo hoặc lăn chuột trực tiếp để chỉnh **Âm lượng** (bấm biểu tượng loa để tắt tiếng) và **Độ sáng màn hình**, đồng thời ẩn thanh OSD mặc định của Windows.
- **Phản hồi phím & Bảng tạm tức thì (`<10ms`):** Thông báo `Caps Lock`, `Num Lock`, `Scroll Lock`, `Insert`, `Copy` / `Cut` / `Paste` (đếm đủ ký tự văn bản, hình ảnh, tệp tin) và thông báo thoát ứng dụng bằng `Alt + F4`.
- **Thông báo Hệ thống, Pin & Thiết bị:**
  - **Pin & Nguồn điện:** Cắm/rút sạc, các mốc sạc (`80%`, `90%`, `100%`), mốc pin yếu (`50%`, `20%`, `10%`, `5%`) và chế độ Tiết kiệm pin.
  - **Kết nối & Ngoại vi:** Mạng Wi-Fi / LAN có dây, kết nối/ngắt VPN, Bluetooth (kèm % pin thiết bị nếu có) và cắm/rút USB.
  - **Hệ thống & Bảo mật:** Xóa tệp / chuyển vào Thùng rác, mở ứng dụng quyền Quản trị (Administrator / UAC), cảnh báo RAM cao (`80%` / `90%` / `95%`), ổ đĩa sắp đầy (`<10%` trống) và Không làm phiền / Tập trung.

### 3. Viên thuốc Live Activity góc trái/trên (Chuyển đổi `< >` & Bấm mở nhanh)
- **Phát nhạc (`GSMTC`):** Hiển thị ảnh bìa bài hát kèm sóng nhạc chuyển động mượt (bấm mở trình phát hoặc phát/tạm dừng).
- **Tiến trình tác vụ (`ITaskbarList3`):** Hiển thị biểu tượng kèm tỷ lệ `%` trực tiếp (ví dụ `77%`) khi tải tệp, sao chép/giải nén trên File Explorer, 7-Zip/WinRAR hoặc cài đặt ứng dụng.
- **VPN, Điểm phát sóng, Đồng hồ & Quyền riêng tư:** Ghim trạng thái VPN, Điểm phát sóng di động (kèm số thiết bị kết nối), Hẹn giờ / Bấm giờ từ ứng dụng Đồng hồ Windows và biểu tượng Micro / Camera / Vị trí (tự động ẩn icon trùng lặp trên Khay hệ thống).
- **Nhận diện Toàn màn hình thông minh:** Chỉ tự ẩn khi có ứng dụng/game/video vào toàn màn hình thật (không có thanh tiêu đề titlebar), không ẩn nhầm với cửa sổ phóng to thông thường.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarGroup:
  - enableSplitTaskbar: true
    $name: Enable Liquid Glass Split Taskbar (Horizontal & Vertical)
    $name:vi-VN: Bật Taskbar Tách Đôi nền kính Liquid Glass (Ngang & Dọc)
  - popupPosition: inline_taskbar
    $name: Live Alert Popup Position
    $name:vi-VN: Vị trí hiển thị Thanh thông báo Live Alert
    $options:vi-VN:
    - inline_taskbar: Biến hình ngay trên Taskbar (Mặc định)
    - adjacent_taskbar: Nổi sát mép Taskbar (Giữ nguyên Thanh ứng dụng)
    $options:
    - inline_taskbar: Morph Inline on Taskbar (Default)
    - adjacent_taskbar: Float Adjacent to Taskbar (Keep Apps Pill Visible)
  - popupSizeMode: fixed
    $name: Center Alert Popup Size Mode
    $name:vi-VN: Chế độ kích thước Thanh thông báo ở giữa
    $options:vi-VN:
    - fixed: Kích thước cố định (Mặc định 600px)
    - sync_apps: Đồng bộ kích thước theo Thanh ứng dụng
    $options:
    - fixed: Fixed Size (Default 600px)
    - sync_apps: Sync Size with Apps Pill
  - fixedPopupWidthDip: 600
    $name: Fixed Popup Width in DIP/px (280 to 1000, Default 600)
    $name:vi-VN: Chiều dài cố định của Thanh thông báo ngang (280 đến 1000px, Mặc định 600)
  - fixedPopupVertHeightDip: 280
    $name: Fixed Popup Height in DIP (180 to 560, for Vertical Taskbar)
    $name:vi-VN: Chiều cao cố định của Thanh thông báo dọc (180 đến 560px)
  - replaceAppsPillOnAlert: true
    $name: Hide Only Apps Pill While Popup Is Shown (Keep System Tray Visible)
    $name:vi-VN: Ẩn sạch Thanh ứng dụng khi hiện Popup (Giữ nguyên Khay trạng thái)
  - iconPaddingDip: 8
    $name: Apps Pill Side Padding Around Icons in DIP (4 to 24)
    $name:vi-VN: Khoảng đệm ôm quanh cụm biểu tượng Ứng dụng (4 đến 24 DIP)
  - pillCornerRadiusDip: 20
    $name: Pill Corner Radius in DIP (8 to 24)
    $name:vi-VN: Độ bo tròn góc viên thuốc (8 đến 24 DIP)
  - pillVerticalMarginDip: 3
    $name: Pill Outer Margin in DIP (0 to 6)
    $name:vi-VN: Khoảng cách lề ngoài của viên thuốc (0 đến 6 DIP)
  $name: 1. Taskbar & Popup Layout
  $name:vi-VN: 1. Bố cục Taskbar & Kích thước Popup (600px)

- appearanceGroup:
  - glassClarity: -85
    $name: Liquid Glass Clarity (-100 = Crystal Clear, 0 = Balanced, 80 = Tinted)
    $name:vi-VN: Độ trong suốt kính Liquid Glass (-100 = Trong vắt, 0 = Cân bằng, 80 = Kính đậm)
  - colorMode: auto
    $name: Contrast Theme (Auto / Dark / Light)
    $name:vi-VN: Tương phản chữ & viền kính (Tự động / Tối / Sáng)
    $options:vi-VN:
    - auto: Tự động theo độ sáng nền kính
    - dark: Kính tối (chữ sáng)
    - light: Kính sáng (chữ tối)
    $options:
    - auto: Auto Backdrop Contrast
    - dark: Dark Glass
    - light: Light Glass
  - fontScalePercent: 100
    $name: Font Scale (90% to 125%)
    $name:vi-VN: Tỷ lệ cỡ chữ (90% đến 125%)
  - language: auto
    $name: Display Language
    $name:vi-VN: Ngôn ngữ hiển thị
    $options:vi-VN:
    - auto: Tự động theo hệ thống
    - vi: Tiếng Việt
    - en: English
    $options:
    - auto: System Default
    - vi: Tiếng Việt
    - en: English
  $name: 2. Synchronized Eye-Comfort Liquid Glass
  $name:vi-VN: 2. Giao diện Kính Liquid Glass Đồng bộ Dịu mắt

- persistentGroup:
  - enableMediaBadge: true
    $name: Media Playback (Album Cover Art + Animated Waveform)
    $name:vi-VN: Phát nhạc (Ảnh bìa bài hát + Sóng nhạc động ở góc trái & Popup 3s)
  - enableTaskProgressBadge: true
    $name: Task Progress (Icon + Percentage e.g. 77% for Downloads & File Ops)
    $name:vi-VN: Tiến trình tác vụ (Biểu tượng + Tỷ lệ % ví dụ 77% ở góc trái & Popup 3s)
  - enableVpnBadge: true
    $name: VPN Connection 3s Alert & Corner Live Activity (Click for Settings)
    $name:vi-VN: Kết nối VPN (Popup 3s & Live Activity góc trái, bấm mở Cài đặt)
  - enableHotspotBadge: true
    $name: Mobile Hotspot Corner Icon (+ Connected Count, Click for Settings)
    $name:vi-VN: Biểu tượng Điểm phát sóng góc trái (+ số thiết bị, bấm mở Cài đặt)
  - enablePrivacyBadges: true
    $name: Mic, Camera & Location Alerts + Corner Icon (Hides Default Tray Icon)
    $name:vi-VN: Thông báo & Icon góc Micro, Camera, Vị trí (Ẩn icon mặc định hệ thống)
  - enableClockBadges: true
    $name: Windows Clock Timer & Stopwatch 3s Alert + Live Time at Corner
    $name:vi-VN: Hẹn giờ & Bấm giờ (Hiện thông báo 3s + biểu tượng & thời gian góc trái)
  $name: 3. Interactive Corner Status Pill (< > Switchable)
  $name:vi-VN: 3. Viên thuốc Trạng thái Góc trái (Có nút < > & Bấm mở nhanh)

- alertsGroup:
  - displayDurationMs: 3000
    $name: Standard Live Alert Duration (ms) — Sliders, Devices & Warnings
    $name:vi-VN: Thời gian hiển thị thông báo chi tiết & Thanh trượt (ms)
  - quickAlertDurationMs: 1300
    $name: Quick Action Alert Duration (ms) — Lock Keys & Clipboard (Default 1300ms)
    $name:vi-VN: Thời gian hiển thị thao tác nhanh — Phím khóa & Copy/Paste (ms, Mặc định 1300)
  - dismissOnHover: true
    $name: Auto-Dismiss Read-Only Alerts on Mouse Hover (Unblock Taskbar Clicks)
    $name:vi-VN: Tự thu gọn thông báo khi rê chuột vào (Nhường chỗ bấm Taskbar ngay)
  - hideDefaultOsd: true
    $name: Hide Default Windows Volume & Brightness OSD
    $name:vi-VN: Ẩn triệt để thanh Âm lượng & Độ sáng mặc định của Windows
  - enableVolumeAlert: true
    $name: Interactive Volume & Mute Live Alert (Mouse Drag & Wheel)
    $name:vi-VN: Thông báo Âm lượng & Tắt tiếng (Chỉnh được bằng chuột)
  - enableBrightnessAlert: true
    $name: Interactive Screen Brightness Live Alert (Mouse Drag & Wheel)
    $name:vi-VN: Thông báo Độ sáng màn hình (Chỉnh được bằng chuột)
  - enableClipboardAlert: true
    $name: Copy, Cut & Paste Live Alert (Full Char Count, Images & Files)
    $name:vi-VN: Thông báo Copy, Cut & Dán (Đếm đủ ký tự văn bản, Hình ảnh, Tệp tin)
  - enableFileDeleteAlert: true
    $name: File Deleted & Moved to Recycle Bin Live Alert
    $name:vi-VN: Thông báo Đã xóa tệp & Đã chuyển vào Thùng rác
  - enableKeyLockAlert: true
    $name: Instant Caps Lock, Num Lock, Scroll Lock & Insert Alert (<10ms)
    $name:vi-VN: Thông báo tức thì Caps Lock, Num Lock, Scroll Lock & Insert (<10ms)
  - enableAdminAppAlert: true
    $name: Elevated App Opened Alert (Administrator / UAC Privileges)
    $name:vi-VN: Thông báo khi mở ứng dụng có quyền Quản trị (Administrator)
  - enableAppExitAlert: true
    $name: Application Closed Alert (When Pressing Alt + F4)
    $name:vi-VN: Thông báo khi thoát ứng dụng (Khi nhấn tổ hợp Alt + F4)
  - enableChargingAlert: true
    $name: Battery Charge (80/90/100%) & Low Battery (50/20/10/5%) Milestones
    $name:vi-VN: Thông báo Sạc pin (80/90/100%) & Pin còn (50/20/10/5%)
  - enableBatterySaverAlert: true
    $name: Battery Saver Mode Live Alert
    $name:vi-VN: Thông báo Chế độ Tiết kiệm pin
  - enableNetworkAlert: true
    $name: Network Connected (Wired LAN / Wi-Fi) Live Alert
    $name:vi-VN: Thông báo Đã kết nối mạng (Mạng dây LAN / Wi-Fi)
  - enableBluetoothAlert: true
    $name: Bluetooth Connected / Disconnected Live Alert
    $name:vi-VN: Thông báo Kết nối / Ngắt kết nối Bluetooth
  - enableUsbAlert: true
    $name: USB Connected / Disconnected Live Alert
    $name:vi-VN: Thông báo Đã kết nối / Ngắt kết nối USB
  - enableRamRomAlert: true
    $name: High RAM (80/90/95%) & Low Disk/ROM (<10% Free) Warnings
    $name:vi-VN: Cảnh báo RAM cao (80/90/95%) & Ổ đĩa ROM sắp đầy (<10% trống)
  - enableDndFocusAlert: true
    $name: Do Not Disturb & Focus Mode Live Alert
    $name:vi-VN: Thông báo Chế độ Không làm phiền & Tập trung
  $name: 4. 3-Second Instant Popup Live Alerts
  $name:vi-VN: 4. Danh sách Popup Live Alerts Tức thì (3 giây)
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WINVER
#define WINVER 0x0A00
#endif
#define PSAPI_VERSION 2
#define NOMINMAX

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-W#warnings"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma clang diagnostic pop
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <sddl.h>
#include <dwmapi.h>
#include <dbt.h>
#include <winioctl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <setupapi.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mfapi.h>
#include <mfidl.h>
#include <uiautomationclient.h>

#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Networking.NetworkOperators.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Shell.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

namespace wf   = winrt::Windows::Foundation;
namespace wux  = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

extern "C" IMAGE_DOS_HEADER __ImageBase;
inline HINSTANCE GetModInstanceHandle() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

// Truy cập con trỏ byte trực tiếp của WriteableBitmap.PixelBuffer() trong C++/WinRT
struct IWhBufferByteAccess : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};
__CRT_UUID_DECL(IWhBufferByteAccess, 0x905a0fef, 0xbc53, 0x11df, 0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda);

// IOCTL độ sáng màn hình tích hợp chuẩn Windows (ntddvdeo.h)
#ifndef IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS
#define IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x126, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif
#ifndef IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS
#define IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x127, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

#pragma pack(push, 1)
struct WhDisplayBrightnessPacket {
    UCHAR ucDisplayPolicy; // 1 = AC, 2 = DC, 3 = BOTH
    UCHAR ucACBrightness;
    UCHAR ucDCBrightness;
};
#pragma pack(pop)

// Giao diện Media Foundation theo dõi hoạt động Camera thụ động
#ifndef __IMFSensorActivityMonitor_INTERFACE_DEFINED__
struct IMFSensorProcessActivity : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetProcessId(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamingState(BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamingMode(int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetReportTime(FILETIME*) = 0;
};
struct IMFSensorActivityReport : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetFriendlyName(LPWSTR, ULONG, ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSymbolicLink(LPWSTR, ULONG, ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessCount(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessActivity(ULONG, IMFSensorProcessActivity**) = 0;
};
struct IMFSensorActivitiesReport : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetActivityReport(ULONG, IMFSensorActivityReport**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetActivityReportByDeviceName(LPCWSTR, IMFSensorActivityReport**) = 0;
};
struct IMFSensorActivitiesReportCallback : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE OnActivitiesReport(IMFSensorActivitiesReport*) = 0;
};
__CRT_UUID_DECL(IMFSensorActivitiesReportCallback, 0xde5072ee, 0xdbe3, 0x46dc, 0x8a, 0x87, 0xb6, 0xf6, 0x31, 0x19, 0x47, 0x51);
struct IMFSensorActivityMonitor : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Start() = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop() = 0;
};
#endif

// Tương thích FocusSessionManager an toàn trên mọi SDK
struct WhNoFocusSessionManagerTag {
    WhNoFocusSessionManagerTag() = default;
    WhNoFocusSessionManagerTag(std::nullptr_t) noexcept {}
    WhNoFocusSessionManagerTag& operator=(std::nullptr_t) noexcept { return *this; }
    explicit operator bool() const noexcept { return false; }
};
namespace winrt::Windows::UI {
    using FocusSessionManager = WhNoFocusSessionManagerTag;
}
namespace winrt::Windows::UI::Shell {
    using DetectedFocusSessionManager = FocusSessionManager;
}

// ---------------------------------------------------------------------------
// Các GUID & WNF hệ thống của nhân Windows
// ---------------------------------------------------------------------------
static constexpr GUID kGuidAcDcPowerSource = {
    0x5d3e9a59, 0xe9d5, 0x4b00, { 0xa6, 0xbd, 0xff, 0x34, 0xff, 0x51, 0x65, 0x48 }
};
static constexpr GUID kGuidBatteryPercentage = {
    0xa7ad8041, 0xb45a, 0x4cae, { 0x87, 0xa3, 0xee, 0xcb, 0xb4, 0x68, 0xa9, 0xe1 }
};
static constexpr GUID kGuidVideoBrightness = {
    0xaded5e82, 0xb909, 0x4619, { 0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb }
};
static constexpr GUID kGuidPowerSavingStatus = {
    0xe00958c0, 0xc213, 0x4ace, { 0xac, 0x77, 0xfe, 0xcc, 0xed, 0x2e, 0xee, 0xa5 }
};
static constexpr GUID kGuidDevInterfaceUsbDevice = {
    0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed }
};
static constexpr GUID kModVolumeSetContextGuid = {
    0x7c8e94f2, 0x3d1a, 0x4b92, { 0x9a, 0x61, 0x54, 0x22, 0x8e, 0xc1, 0x08, 0x00 }
};

struct WnfStateName {
    ULONG Data[2];
};
static constexpr WnfStateName kWnfQuietHoursStateName = { 0xa3bf1c75UL, 0x0d83063eUL };
static constexpr WnfStateName kWnfLocationStateName   = { 0xa3bc0875UL, 0x41950821UL };

// ---------------------------------------------------------------------------
// 1. Cấu hình & Trạng thái toàn cục
// ---------------------------------------------------------------------------
enum class ColorMode { Auto, Dark, Light };
enum class UiLang { Auto, Vi, En };
enum class PopupSizeMode { Fixed, SyncApps };
enum class PopupPosition { InlineTaskbar, AdjacentTaskbar };

struct ModSettings {
    bool enableSplitTaskbar = true;
    PopupPosition popupPosition = PopupPosition::InlineTaskbar;
    PopupSizeMode popupSizeMode = PopupSizeMode::Fixed;
    int fixedPopupWidthDip = 600;
    int fixedPopupVertHeightDip = 280;
    bool replaceAppsPillOnAlert = true;
    int iconPaddingDip = 8;
    int pillCornerRadiusDip = 20;
    int pillVerticalMarginDip = 3;

    int glassClarity = -85;
    ColorMode colorMode = ColorMode::Auto;
    int fontScalePercent = 100;
    UiLang lang = UiLang::Auto;

    bool enableMediaBadge = true;
    bool enableTaskProgressBadge = true;
    bool enableVpnBadge = true;
    bool enableHotspotBadge = true;
    bool enablePrivacyBadges = true;
    bool enableClockBadges = true;

    int displayDurationMs = 3000;
    int quickAlertDurationMs = 1300;
    bool dismissOnHover = true;
    bool hideDefaultOsd = true;
    bool enableVolumeAlert = true;
    bool enableBrightnessAlert = true;
    bool enableClipboardAlert = true;
    bool enableFileDeleteAlert = true;
    bool enableKeyLockAlert = true;
    bool enableAdminAppAlert = true;
    bool enableAppExitAlert = true;
    bool enableChargingAlert = true;
    bool enableBatterySaverAlert = true;
    bool enableNetworkAlert = true;
    bool enableBluetoothAlert = true;
    bool enableUsbAlert = true;
    bool enableRamRomAlert = true;
    bool enableDndFocusAlert = true;
};

static ModSettings g_uiSettings;
static std::atomic<bool> g_running{false};
static std::atomic<bool> g_unloading{false};

static std::atomic<bool> g_enableSplitTaskbarAtomic{true};
static std::atomic<bool> g_hideDefaultOsdAtomic{true};
static std::atomic<bool> g_enableVolumeAlertAtomic{true};
static std::atomic<bool> g_enableBrightnessAlertAtomic{true};
static std::atomic<bool> g_enableKeyLockAlertAtomic{true};
static std::atomic<bool> g_enableClipboardAlertAtomic{true};
static std::atomic<bool> g_enableAdminAppAlertAtomic{true};
static std::atomic<bool> g_enableAppExitAlertAtomic{true};
static std::atomic<bool> g_enableNetworkAlertAtomic{true};
static std::atomic<bool> g_enableBluetoothAlertAtomic{true};
static std::atomic<bool> g_enableRamRomAlertAtomic{true};
static std::atomic<bool> g_enableDndFocusAlertAtomic{true};
static std::atomic<bool> g_enableMediaBadgeAtomic{true};
static std::atomic<bool> g_enableTaskProgressBadgeAtomic{true};
static std::atomic<bool> g_enableVpnBadgeAtomic{true};
static std::atomic<bool> g_enableHotspotBadgeAtomic{true};
static std::atomic<bool> g_enablePrivacyBadgesAtomic{true};
static std::atomic<bool> g_enableClockBadgesAtomic{true};
static std::atomic<bool> g_useViAtomic{true};
static std::atomic<int>  g_alertDurationMsAtomic{3000};
static std::atomic<int>  g_quickAlertDurationMsAtomic{1300};
static std::atomic<int>  g_iconPaddingDipAtomic{8};
static std::atomic<int>  g_pillCornerRadiusDipAtomic{20};
static std::atomic<int>  g_pillVerticalMarginDipAtomic{3};

static bool g_cachedSystemLightTheme = false;
static bool g_cachedTaskbarCentered = true;

static HANDLE g_singletonMutex = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_watcherWakeEvent = nullptr;
static HANDLE g_watcherThread = nullptr;
static HANDLE g_clockThread = nullptr;
static HANDLE g_kbdHookThread = nullptr;
static DWORD  g_kbdHookThreadId = 0;
static HANDLE g_uiThread = nullptr;
static DWORD  g_uiThreadId = 0;

static HWND   g_alertHwnd = nullptr;        // Popup Live Alert 3s ở giữa (kéo thanh trượt chuột, sóng nhạc, ảnh bìa)
static HWND   g_cornerWidgetHwnd = nullptr; // Viên thuốc Góc Trái/Trên tương tác chuột (< > chuyển đổi & bấm mở nhanh)

// Trạng thái phím tức thì từ WH_KEYBOARD_LL (chống 100% nhầm Alt+Tab sang Độ sáng!)
static std::atomic<bool>      g_llCtrlDown{false};
static std::atomic<bool>      g_llShiftDown{false};
static std::atomic<bool>      g_llAltDown{false};
static std::atomic<bool>      g_llWinDown{false};
static std::atomic<ULONGLONG> g_lastAltOrWinKeyTick{0};
static std::atomic<ULONGLONG> g_lastCutShortcutTick{0};
static std::atomic<ULONGLONG> g_lastVolKeyPressTick{0};

#define WM_APP_SHOW_ALERT             (WM_APP + 101)
#define WM_APP_RELOAD_SETTINGS        (WM_APP + 102)
#define WM_APP_SHNOTIFY               (WM_APP + 103)
#define WM_APP_XAML_BOUNDS_CHANGED    (WM_APP + 104)
#define WM_APP_INDICATORS_CHANGED     (WM_APP + 105)
#define WM_APP_OSD_INTERCEPTED        (WM_APP + 106)
#define WM_APP_AUDIO_ENDPOINT_CHANGED (WM_APP + 107)
#define WM_APP_WNF_QUIETHOURS         (WM_APP + 108)
#define WM_APP_KEYLOCK_EVENT          (WM_APP + 109)
#define WM_APP_VOLKEY_EVENT           (WM_APP + 110)
#define WM_APP_PASTE_EVENT            (WM_APP + 111)
#define WM_APP_ALTF4_PRESSED          (WM_APP + 112)
#define WM_APP_TASK_PROGRESS_EVENT    (WM_APP + 113)

#define IDT_ANIM_FRAME                1001
#define IDT_ALERT_EXPIRE              1002
#define IDT_SHELLHOOK_DEBOUNCE        1003
#define IDT_CLIPBOARD_DEBOUNCE        1004
#define IDT_FILEOP_COALESCE           1007
#define IDT_USB_DEBOUNCE              1008
#define IDT_TB_BACKDROP_WATCH         1009
#define IDT_VOLKEY_FALLBACK           1011
#define IDT_CORNER_CLOCK_TICK         1012
#define IDT_DND_FALLBACK_POLL         1013
#define IDT_CORNER_WAVE_TICK          1014
#define IDT_ALTF4_VERIFY              1015
#define IDT_TASK_PROGRESS_CLEANUP     1016

static constexpr UINT_PTR kTaskbarOwnerSyncTimerId   = 0x57484C49;
static constexpr UINT_PTR kTaskbarFastAnimTimerId    = 0x57484C4A;
static constexpr UINT_PTR kTaskbarMenuInjectTimerId  = 0x57484C4B;

static constexpr wchar_t kSettingsHostClassName[]   = L"WindhawkModernTaskbarSettingsHost_v8";
static constexpr wchar_t kWin11SettingsClassName[]  = L"WindhawkModernTaskbarSettingsWnd_v8";
static constexpr wchar_t kComboFlyoutClassName[]    = L"WindhawkModernTaskbarComboFlyout_v8";

static UINT g_msgOpenSettings   = 0;
static UINT g_msgReloadSettings = 0;

static std::atomic<bool>   g_isExplorerProcess{false};
static bool                g_isToolModProcessLauncher = false;
static HANDLE              g_toolModProcessMutex = nullptr;
static std::atomic<HWND>   g_hSettingsHostWnd{nullptr};
static std::atomic<HWND>   g_hWin11SettingsWnd{nullptr};
static HANDLE              g_hSettingsHostThread = nullptr;
static DWORD               g_settingsHostThreadId = 0;

void OpenModernTaskbarSettingsApp();

std::wstring ToLowerStr(std::wstring s) {
    for (wchar_t& ch : s) ch = static_cast<wchar_t>(std::towlower(ch));
    return s;
}

std::wstring FormatUintWithSeparators(uint64_t value, bool vi) {
    std::wstring raw = std::to_wstring(value);
    wchar_t sep = vi ? L'.' : L',';
    std::wstring out;
    out.reserve(raw.size() + raw.size() / 3);
    for (size_t i = 0; i < raw.size(); ++i) {
        if (i > 0 && ((raw.size() - i) % 3 == 0)) {
            out.push_back(sep);
        }
        out.push_back(raw[i]);
    }
    return out;
}

void RefreshSystemThemeAndAlignmentCache() {
    DWORD val = 0;
    DWORD cb = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme",
                     RRF_RT_REG_DWORD, nullptr, &val, &cb) == ERROR_SUCCESS) {
        g_cachedSystemLightTheme = (val != 0);
    } else {
        g_cachedSystemLightTheme = false;
    }

    val = 1;
    cb = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"TaskbarAl",
                     RRF_RT_REG_DWORD, nullptr, &val, &cb) == ERROR_SUCCESS) {
        g_cachedTaskbarCentered = (val != 0);
    } else {
        g_cachedTaskbarCentered = true;
    }
}

bool ComputeUseVietnamese(UiLang lang) {
    if (lang == UiLang::Vi) return true;
    if (lang == UiLang::En) return false;
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_VIETNAMESE;
}

inline const wchar_t* PickText(bool vi, const wchar_t* viText, const wchar_t* enText) {
    return vi ? viText : enText;
}

inline int DpiScale(int value, UINT dpi, int scalePct = 100) {
    return MulDiv(value, static_cast<int>(dpi) * std::clamp(scalePct, 85, 135), 96 * 100);
}

inline float DpiScaleF(float value, UINT dpi, int scalePct = 100) {
    return value * (static_cast<float>(dpi) / 96.0f) * (std::clamp(scalePct, 85, 135) / 100.0f);
}

// ---------------------------------------------------------------------------
// 1b. Per-User In-App Settings Storage (InAppCfg_<key>_<SID>) + Windhawk Snapshot
// ---------------------------------------------------------------------------
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

std::wstring MakeInAppCfgKey(const wchar_t* keyName) {
    return std::wstring(L"InAppCfg_") + keyName + L"_" + GetCachedUserSidString();
}

void SaveInAppIntOverride(const wchar_t* keyName, int val) {
    Wh_SetIntValue(MakeInAppCfgKey(keyName).c_str(), val);
}

void SaveInAppStringOverride(const wchar_t* keyName, const std::wstring& val) {
    Wh_SetStringValue(MakeInAppCfgKey(keyName).c_str(), val.c_str());
}

void ClearInAppOverride(const wchar_t* keyName) {
    Wh_DeleteValue(MakeInAppCfgKey(keyName).c_str());
}

bool TryLoadInAppIntOverride(const wchar_t* keyName, int& outVal) {
    constexpr int kSentinel = -2147483647;
    int v = Wh_GetIntValue(MakeInAppCfgKey(keyName).c_str(), kSentinel);
    if (v == kSentinel) return false;
    outVal = v;
    return true;
}

bool TryLoadInAppStringOverride(const wchar_t* keyName, std::wstring& outVal) {
    wchar_t buf[256] = {};
    size_t len = Wh_GetStringValue(MakeInAppCfgKey(keyName).c_str(), buf, ARRAYSIZE(buf));
    if (len == 0 || buf[0] == L'\0') return false;
    outVal.assign(buf);
    return true;
}

struct WindhawkRawSnapshot {
    bool initialized = false;
    int enableSplitTaskbar = 1;
    std::wstring popupPosition;
    std::wstring popupSizeMode;
    int fixedPopupWidthDip = 600;
    int fixedPopupVertHeightDip = 280;
    int replaceAppsPillOnAlert = 1;
    int iconPaddingDip = 8;
    int pillCornerRadiusDip = 20;
    int pillVerticalMarginDip = 3;

    int glassClarity = -85;
    std::wstring colorMode;
    int fontScalePercent = 100;
    std::wstring language;

    int enableMediaBadge = 1;
    int enableTaskProgressBadge = 1;
    int enableVpnBadge = 1;
    int enableHotspotBadge = 1;
    int enablePrivacyBadges = 1;
    int enableClockBadges = 1;

    int displayDurationMs = 3000;
    int quickAlertDurationMs = 1300;
    int dismissOnHover = 1;
    int hideDefaultOsd = 1;
    int enableVolumeAlert = 1;
    int enableBrightnessAlert = 1;
    int enableClipboardAlert = 1;
    int enableFileDeleteAlert = 1;
    int enableKeyLockAlert = 1;
    int enableAdminAppAlert = 1;
    int enableAppExitAlert = 1;
    int enableChargingAlert = 1;
    int enableBatterySaverAlert = 1;
    int enableNetworkAlert = 1;
    int enableBluetoothAlert = 1;
    int enableUsbAlert = 1;
    int enableRamRomAlert = 1;
    int enableDndFocusAlert = 1;
};

static WindhawkRawSnapshot g_lastWhSnapshot;
static std::mutex g_whSnapshotMutex;

void ClearAllInAppOverrides() {
    static constexpr const wchar_t* kAllKeys[] = {
        L"enableSplitTaskbar", L"popupPosition", L"popupSizeMode",
        L"fixedPopupWidthDip", L"fixedPopupVertHeightDip", L"replaceAppsPillOnAlert",
        L"iconPaddingDip", L"pillCornerRadiusDip", L"pillVerticalMarginDip",
        L"glassClarity", L"colorMode", L"fontScalePercent", L"language",
        L"enableMediaBadge", L"enableTaskProgressBadge", L"enableVpnBadge",
        L"enableHotspotBadge", L"enablePrivacyBadges", L"enableClockBadges",
        L"displayDurationMs", L"quickAlertDurationMs", L"dismissOnHover",
        L"hideDefaultOsd", L"enableVolumeAlert", L"enableBrightnessAlert",
        L"enableClipboardAlert", L"enableFileDeleteAlert", L"enableKeyLockAlert",
        L"enableAdminAppAlert", L"enableAppExitAlert", L"enableChargingAlert",
        L"enableBatterySaverAlert", L"enableNetworkAlert", L"enableBluetoothAlert",
        L"enableUsbAlert", L"enableRamRomAlert", L"enableDndFocusAlert",
    };
    for (const wchar_t* k : kAllKeys) {
        ClearInAppOverride(k);
    }
}

ModSettings ReadSettingsFromWindhawk(bool fromSettingsChanged = false) {
    auto readWhStr = [](const wchar_t* settingName, const wchar_t* fallback) -> std::wstring {
        auto val = WindhawkUtils::StringSetting::make(settingName);
        return (val.get() && val.get()[0]) ? std::wstring(val.get()) : std::wstring(fallback);
    };

    WindhawkRawSnapshot raw;
    raw.initialized             = true;
    raw.enableSplitTaskbar      = Wh_GetIntSetting(L"taskbarGroup.enableSplitTaskbar") != 0 ? 1 : 0;
    raw.popupPosition           = readWhStr(L"taskbarGroup.popupPosition", L"inline_taskbar");
    raw.popupSizeMode           = readWhStr(L"taskbarGroup.popupSizeMode", L"fixed");
    raw.fixedPopupWidthDip      = Wh_GetIntSetting(L"taskbarGroup.fixedPopupWidthDip");
    raw.fixedPopupVertHeightDip = Wh_GetIntSetting(L"taskbarGroup.fixedPopupVertHeightDip");
    raw.replaceAppsPillOnAlert  = Wh_GetIntSetting(L"taskbarGroup.replaceAppsPillOnAlert") != 0 ? 1 : 0;
    raw.iconPaddingDip          = Wh_GetIntSetting(L"taskbarGroup.iconPaddingDip");
    raw.pillCornerRadiusDip     = Wh_GetIntSetting(L"taskbarGroup.pillCornerRadiusDip");
    raw.pillVerticalMarginDip   = Wh_GetIntSetting(L"taskbarGroup.pillVerticalMarginDip");

    raw.glassClarity            = Wh_GetIntSetting(L"appearanceGroup.glassClarity");
    raw.colorMode               = readWhStr(L"appearanceGroup.colorMode", L"auto");
    raw.fontScalePercent        = Wh_GetIntSetting(L"appearanceGroup.fontScalePercent");
    raw.language                = readWhStr(L"appearanceGroup.language", L"auto");

    raw.enableMediaBadge        = Wh_GetIntSetting(L"persistentGroup.enableMediaBadge") != 0 ? 1 : 0;
    raw.enableTaskProgressBadge = Wh_GetIntSetting(L"persistentGroup.enableTaskProgressBadge") != 0 ? 1 : 0;
    raw.enableVpnBadge          = Wh_GetIntSetting(L"persistentGroup.enableVpnBadge") != 0 ? 1 : 0;
    raw.enableHotspotBadge      = Wh_GetIntSetting(L"persistentGroup.enableHotspotBadge") != 0 ? 1 : 0;
    raw.enablePrivacyBadges     = Wh_GetIntSetting(L"persistentGroup.enablePrivacyBadges") != 0 ? 1 : 0;
    raw.enableClockBadges       = Wh_GetIntSetting(L"persistentGroup.enableClockBadges") != 0 ? 1 : 0;

    raw.displayDurationMs       = Wh_GetIntSetting(L"alertsGroup.displayDurationMs");
    raw.quickAlertDurationMs    = Wh_GetIntSetting(L"alertsGroup.quickAlertDurationMs");
    raw.dismissOnHover          = Wh_GetIntSetting(L"alertsGroup.dismissOnHover") != 0 ? 1 : 0;
    raw.hideDefaultOsd          = Wh_GetIntSetting(L"alertsGroup.hideDefaultOsd") != 0 ? 1 : 0;
    raw.enableVolumeAlert       = Wh_GetIntSetting(L"alertsGroup.enableVolumeAlert") != 0 ? 1 : 0;
    raw.enableBrightnessAlert   = Wh_GetIntSetting(L"alertsGroup.enableBrightnessAlert") != 0 ? 1 : 0;
    raw.enableClipboardAlert    = Wh_GetIntSetting(L"alertsGroup.enableClipboardAlert") != 0 ? 1 : 0;
    raw.enableFileDeleteAlert   = Wh_GetIntSetting(L"alertsGroup.enableFileDeleteAlert") != 0 ? 1 : 0;
    raw.enableKeyLockAlert      = Wh_GetIntSetting(L"alertsGroup.enableKeyLockAlert") != 0 ? 1 : 0;
    raw.enableAdminAppAlert     = Wh_GetIntSetting(L"alertsGroup.enableAdminAppAlert") != 0 ? 1 : 0;
    raw.enableAppExitAlert      = Wh_GetIntSetting(L"alertsGroup.enableAppExitAlert") != 0 ? 1 : 0;
    raw.enableChargingAlert     = Wh_GetIntSetting(L"alertsGroup.enableChargingAlert") != 0 ? 1 : 0;
    raw.enableBatterySaverAlert = Wh_GetIntSetting(L"alertsGroup.enableBatterySaverAlert") != 0 ? 1 : 0;
    raw.enableNetworkAlert      = Wh_GetIntSetting(L"alertsGroup.enableNetworkAlert") != 0 ? 1 : 0;
    raw.enableBluetoothAlert    = Wh_GetIntSetting(L"alertsGroup.enableBluetoothAlert") != 0 ? 1 : 0;
    raw.enableUsbAlert          = Wh_GetIntSetting(L"alertsGroup.enableUsbAlert") != 0 ? 1 : 0;
    raw.enableRamRomAlert       = Wh_GetIntSetting(L"alertsGroup.enableRamRomAlert") != 0 ? 1 : 0;
    raw.enableDndFocusAlert     = Wh_GetIntSetting(L"alertsGroup.enableDndFocusAlert") != 0 ? 1 : 0;

    {
        std::lock_guard<std::mutex> lock(g_whSnapshotMutex);
        if (fromSettingsChanged && g_lastWhSnapshot.initialized) {
            auto clearIfChangedInt = [](const wchar_t* k, int oldV, int newV) {
                if (oldV != newV) ClearInAppOverride(k);
            };
            auto clearIfChangedStr = [](const wchar_t* k, const std::wstring& oldV, const std::wstring& newV) {
                if (oldV != newV) ClearInAppOverride(k);
            };
            clearIfChangedInt(L"enableSplitTaskbar", g_lastWhSnapshot.enableSplitTaskbar, raw.enableSplitTaskbar);
            clearIfChangedStr(L"popupPosition", g_lastWhSnapshot.popupPosition, raw.popupPosition);
            clearIfChangedStr(L"popupSizeMode", g_lastWhSnapshot.popupSizeMode, raw.popupSizeMode);
            clearIfChangedInt(L"fixedPopupWidthDip", g_lastWhSnapshot.fixedPopupWidthDip, raw.fixedPopupWidthDip);
            clearIfChangedInt(L"fixedPopupVertHeightDip", g_lastWhSnapshot.fixedPopupVertHeightDip, raw.fixedPopupVertHeightDip);
            clearIfChangedInt(L"replaceAppsPillOnAlert", g_lastWhSnapshot.replaceAppsPillOnAlert, raw.replaceAppsPillOnAlert);
            clearIfChangedInt(L"iconPaddingDip", g_lastWhSnapshot.iconPaddingDip, raw.iconPaddingDip);
            clearIfChangedInt(L"pillCornerRadiusDip", g_lastWhSnapshot.pillCornerRadiusDip, raw.pillCornerRadiusDip);
            clearIfChangedInt(L"pillVerticalMarginDip", g_lastWhSnapshot.pillVerticalMarginDip, raw.pillVerticalMarginDip);

            clearIfChangedInt(L"glassClarity", g_lastWhSnapshot.glassClarity, raw.glassClarity);
            clearIfChangedStr(L"colorMode", g_lastWhSnapshot.colorMode, raw.colorMode);
            clearIfChangedInt(L"fontScalePercent", g_lastWhSnapshot.fontScalePercent, raw.fontScalePercent);
            clearIfChangedStr(L"language", g_lastWhSnapshot.language, raw.language);

            clearIfChangedInt(L"enableMediaBadge", g_lastWhSnapshot.enableMediaBadge, raw.enableMediaBadge);
            clearIfChangedInt(L"enableTaskProgressBadge", g_lastWhSnapshot.enableTaskProgressBadge, raw.enableTaskProgressBadge);
            clearIfChangedInt(L"enableVpnBadge", g_lastWhSnapshot.enableVpnBadge, raw.enableVpnBadge);
            clearIfChangedInt(L"enableHotspotBadge", g_lastWhSnapshot.enableHotspotBadge, raw.enableHotspotBadge);
            clearIfChangedInt(L"enablePrivacyBadges", g_lastWhSnapshot.enablePrivacyBadges, raw.enablePrivacyBadges);
            clearIfChangedInt(L"enableClockBadges", g_lastWhSnapshot.enableClockBadges, raw.enableClockBadges);

            clearIfChangedInt(L"displayDurationMs", g_lastWhSnapshot.displayDurationMs, raw.displayDurationMs);
            clearIfChangedInt(L"quickAlertDurationMs", g_lastWhSnapshot.quickAlertDurationMs, raw.quickAlertDurationMs);
            clearIfChangedInt(L"dismissOnHover", g_lastWhSnapshot.dismissOnHover, raw.dismissOnHover);
            clearIfChangedInt(L"hideDefaultOsd", g_lastWhSnapshot.hideDefaultOsd, raw.hideDefaultOsd);
            clearIfChangedInt(L"enableVolumeAlert", g_lastWhSnapshot.enableVolumeAlert, raw.enableVolumeAlert);
            clearIfChangedInt(L"enableBrightnessAlert", g_lastWhSnapshot.enableBrightnessAlert, raw.enableBrightnessAlert);
            clearIfChangedInt(L"enableClipboardAlert", g_lastWhSnapshot.enableClipboardAlert, raw.enableClipboardAlert);
            clearIfChangedInt(L"enableFileDeleteAlert", g_lastWhSnapshot.enableFileDeleteAlert, raw.enableFileDeleteAlert);
            clearIfChangedInt(L"enableKeyLockAlert", g_lastWhSnapshot.enableKeyLockAlert, raw.enableKeyLockAlert);
            clearIfChangedInt(L"enableAdminAppAlert", g_lastWhSnapshot.enableAdminAppAlert, raw.enableAdminAppAlert);
            clearIfChangedInt(L"enableAppExitAlert", g_lastWhSnapshot.enableAppExitAlert, raw.enableAppExitAlert);
            clearIfChangedInt(L"enableChargingAlert", g_lastWhSnapshot.enableChargingAlert, raw.enableChargingAlert);
            clearIfChangedInt(L"enableBatterySaverAlert", g_lastWhSnapshot.enableBatterySaverAlert, raw.enableBatterySaverAlert);
            clearIfChangedInt(L"enableNetworkAlert", g_lastWhSnapshot.enableNetworkAlert, raw.enableNetworkAlert);
            clearIfChangedInt(L"enableBluetoothAlert", g_lastWhSnapshot.enableBluetoothAlert, raw.enableBluetoothAlert);
            clearIfChangedInt(L"enableUsbAlert", g_lastWhSnapshot.enableUsbAlert, raw.enableUsbAlert);
            clearIfChangedInt(L"enableRamRomAlert", g_lastWhSnapshot.enableRamRomAlert, raw.enableRamRomAlert);
            clearIfChangedInt(L"enableDndFocusAlert", g_lastWhSnapshot.enableDndFocusAlert, raw.enableDndFocusAlert);
        }
        g_lastWhSnapshot = raw;
    }

    auto resolveInt = [](const wchar_t* k, int rawVal) -> int {
        int ov = 0;
        return TryLoadInAppIntOverride(k, ov) ? ov : rawVal;
    };
    auto resolveBool = [&](const wchar_t* k, int rawVal) -> bool {
        return resolveInt(k, rawVal) != 0;
    };
    auto resolveStr = [](const wchar_t* k, const std::wstring& rawVal) -> std::wstring {
        std::wstring ov;
        return TryLoadInAppStringOverride(k, ov) ? ov : rawVal;
    };

    ModSettings s;
    s.enableSplitTaskbar = resolveBool(L"enableSplitTaskbar", raw.enableSplitTaskbar);

    std::wstring posModeStr = resolveStr(L"popupPosition", raw.popupPosition);
    s.popupPosition = (posModeStr == L"adjacent_taskbar") ? PopupPosition::AdjacentTaskbar : PopupPosition::InlineTaskbar;

    std::wstring szModeStr = resolveStr(L"popupSizeMode", raw.popupSizeMode);
    s.popupSizeMode = (szModeStr == L"sync_apps") ? PopupSizeMode::SyncApps : PopupSizeMode::Fixed;

    int fw = resolveInt(L"fixedPopupWidthDip", raw.fixedPopupWidthDip);
    s.fixedPopupWidthDip = (fw > 0) ? std::clamp(fw, 280, 1000) : 600;

    int fvh = resolveInt(L"fixedPopupVertHeightDip", raw.fixedPopupVertHeightDip);
    s.fixedPopupVertHeightDip = (fvh > 0) ? std::clamp(fvh, 180, 560) : 280;

    s.replaceAppsPillOnAlert = resolveBool(L"replaceAppsPillOnAlert", raw.replaceAppsPillOnAlert);

    int padDip = resolveInt(L"iconPaddingDip", raw.iconPaddingDip);
    s.iconPaddingDip = (padDip > 0) ? std::clamp(padDip, 4, 24) : 8;

    int rad = resolveInt(L"pillCornerRadiusDip", raw.pillCornerRadiusDip);
    s.pillCornerRadiusDip = (rad > 0) ? std::clamp(rad, 8, 24) : 20;

    int vMargin = resolveInt(L"pillVerticalMarginDip", raw.pillVerticalMarginDip);
    s.pillVerticalMarginDip = std::clamp(vMargin, 0, 6);

    s.glassClarity = std::clamp(resolveInt(L"glassClarity", raw.glassClarity), -100, 100);

    std::wstring colStr = resolveStr(L"colorMode", raw.colorMode);
    if (colStr == L"dark") s.colorMode = ColorMode::Dark;
    else if (colStr == L"light") s.colorMode = ColorMode::Light;
    else s.colorMode = ColorMode::Auto;

    int fScale = resolveInt(L"fontScalePercent", raw.fontScalePercent);
    s.fontScalePercent = (fScale > 0) ? std::clamp(fScale, 90, 125) : 100;

    std::wstring langStr = resolveStr(L"language", raw.language);
    if (langStr == L"vi") s.lang = UiLang::Vi;
    else if (langStr == L"en") s.lang = UiLang::En;
    else s.lang = UiLang::Auto;

    s.enableMediaBadge        = resolveBool(L"enableMediaBadge", raw.enableMediaBadge);
    s.enableTaskProgressBadge = resolveBool(L"enableTaskProgressBadge", raw.enableTaskProgressBadge);
    s.enableVpnBadge          = resolveBool(L"enableVpnBadge", raw.enableVpnBadge);
    s.enableHotspotBadge      = resolveBool(L"enableHotspotBadge", raw.enableHotspotBadge);
    s.enablePrivacyBadges     = resolveBool(L"enablePrivacyBadges", raw.enablePrivacyBadges);
    s.enableClockBadges       = resolveBool(L"enableClockBadges", raw.enableClockBadges);

    int dur = resolveInt(L"displayDurationMs", raw.displayDurationMs);
    s.displayDurationMs = (dur >= 1000) ? std::clamp(dur, 1000, 15000) : 3000;

    int qDur = resolveInt(L"quickAlertDurationMs", raw.quickAlertDurationMs);
    s.quickAlertDurationMs = (qDur >= 500) ? std::clamp(qDur, 500, 5000) : 1300;

    s.dismissOnHover          = resolveBool(L"dismissOnHover", raw.dismissOnHover);
    s.hideDefaultOsd          = resolveBool(L"hideDefaultOsd", raw.hideDefaultOsd);
    s.enableVolumeAlert       = resolveBool(L"enableVolumeAlert", raw.enableVolumeAlert);
    s.enableBrightnessAlert   = resolveBool(L"enableBrightnessAlert", raw.enableBrightnessAlert);
    s.enableClipboardAlert    = resolveBool(L"enableClipboardAlert", raw.enableClipboardAlert);
    s.enableFileDeleteAlert   = resolveBool(L"enableFileDeleteAlert", raw.enableFileDeleteAlert);
    s.enableKeyLockAlert      = resolveBool(L"enableKeyLockAlert", raw.enableKeyLockAlert);
    s.enableAdminAppAlert     = resolveBool(L"enableAdminAppAlert", raw.enableAdminAppAlert);
    s.enableAppExitAlert      = resolveBool(L"enableAppExitAlert", raw.enableAppExitAlert);
    s.enableChargingAlert     = resolveBool(L"enableChargingAlert", raw.enableChargingAlert);
    s.enableBatterySaverAlert = resolveBool(L"enableBatterySaverAlert", raw.enableBatterySaverAlert);
    s.enableNetworkAlert      = resolveBool(L"enableNetworkAlert", raw.enableNetworkAlert);
    s.enableBluetoothAlert    = resolveBool(L"enableBluetoothAlert", raw.enableBluetoothAlert);
    s.enableUsbAlert          = resolveBool(L"enableUsbAlert", raw.enableUsbAlert);
    s.enableRamRomAlert       = resolveBool(L"enableRamRomAlert", raw.enableRamRomAlert);
    s.enableDndFocusAlert     = resolveBool(L"enableDndFocusAlert", raw.enableDndFocusAlert);

    g_enableSplitTaskbarAtomic.store(s.enableSplitTaskbar, std::memory_order_relaxed);
    g_hideDefaultOsdAtomic.store(s.hideDefaultOsd, std::memory_order_relaxed);
    g_enableVolumeAlertAtomic.store(s.enableVolumeAlert, std::memory_order_relaxed);
    g_enableBrightnessAlertAtomic.store(s.enableBrightnessAlert, std::memory_order_relaxed);
    g_enableKeyLockAlertAtomic.store(s.enableKeyLockAlert, std::memory_order_relaxed);
    g_enableClipboardAlertAtomic.store(s.enableClipboardAlert, std::memory_order_relaxed);
    g_enableAdminAppAlertAtomic.store(s.enableAdminAppAlert, std::memory_order_relaxed);
    g_enableAppExitAlertAtomic.store(s.enableAppExitAlert, std::memory_order_relaxed);
    g_enableNetworkAlertAtomic.store(s.enableNetworkAlert, std::memory_order_relaxed);
    g_enableBluetoothAlertAtomic.store(s.enableBluetoothAlert, std::memory_order_relaxed);
    g_enableRamRomAlertAtomic.store(s.enableRamRomAlert, std::memory_order_relaxed);
    g_enableDndFocusAlertAtomic.store(s.enableDndFocusAlert, std::memory_order_relaxed);
    g_enableMediaBadgeAtomic.store(s.enableMediaBadge, std::memory_order_relaxed);
    g_enableTaskProgressBadgeAtomic.store(s.enableTaskProgressBadge, std::memory_order_relaxed);
    g_enableVpnBadgeAtomic.store(s.enableVpnBadge, std::memory_order_relaxed);
    g_enableHotspotBadgeAtomic.store(s.enableHotspotBadge, std::memory_order_relaxed);
    g_enablePrivacyBadgesAtomic.store(s.enablePrivacyBadges, std::memory_order_relaxed);
    g_enableClockBadgesAtomic.store(s.enableClockBadges, std::memory_order_relaxed);
    g_useViAtomic.store(ComputeUseVietnamese(s.lang), std::memory_order_relaxed);
    g_alertDurationMsAtomic.store(s.displayDurationMs, std::memory_order_relaxed);
    g_quickAlertDurationMsAtomic.store(s.quickAlertDurationMs, std::memory_order_relaxed);
    g_iconPaddingDipAtomic.store(s.iconPaddingDip, std::memory_order_relaxed);
    g_pillCornerRadiusDipAtomic.store(s.pillCornerRadiusDip, std::memory_order_relaxed);
    g_pillVerticalMarginDipAtomic.store(s.pillVerticalMarginDip, std::memory_order_relaxed);

    RefreshSystemThemeAndAlignmentCache();
    return s;
}

// ---------------------------------------------------------------------------
// 2. Hook Hệ thống Nhẹ trong explorer.exe:
//    (A) Bộ lọc OSD Âm lượng/Độ sáng siêu chặt (Không bao giờ bắt nhầm Alt+Tab hay Win+Tab!)
//    (B) Xóa nền DWM mờ đục gốc của Shell_TrayWnd
// ---------------------------------------------------------------------------
struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    DWORD GradientColor;
    int AnimationId;
};
struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
static SetWindowCompositionAttribute_t pOrigSetWindowCompositionAttribute = nullptr;

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t pOrigShowWindow = nullptr;

using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t pOrigSetWindowPos = nullptr;

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t pOrigLoadLibraryExW = nullptr;

static HWND g_cachedTaskbarHwnd = nullptr;
static std::atomic<bool> g_xamlBgCollapsed{false};
static std::atomic<bool> g_xamlGlassInjected{false};
static std::atomic<bool> g_hideAppsPillAtomic{false};
static std::atomic<ULONGLONG> g_lastOsdInterceptTick{0};
static std::atomic<ULONGLONG> g_lastVolOrBriAlertTick{0};
static std::atomic<ULONGLONG> g_lastVolumeAlertTick{0};

HWND FindCurrentProcessTaskbarWnd() {
    if (g_cachedTaskbarHwnd && IsWindow(g_cachedTaskbarHwnd)) {
        return g_cachedTaskbarHwnd;
    }
    HWND found = nullptr;
    struct FindTrayCb {
        static BOOL CALLBACK Proc(HWND hWnd, LPARAM lParam) {
            DWORD pid = 0;
            wchar_t cls[32] = {};
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
                _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        }
    };
    EnumWindows(FindTrayCb::Proc, reinterpret_cast<LPARAM>(&found));
    g_cachedTaskbarHwnd = found;
    return found;
}

bool IsMultitaskingViewThread(DWORD threadId) {
    using GetThreadDescriptionFn = HRESULT(WINAPI*)(HANDLE, PWSTR*);
    static auto fnGetDesc = reinterpret_cast<GetThreadDescriptionFn>(
        GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "GetThreadDescription"));
    if (!fnGetDesc || threadId == 0) return false;

    HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
    if (!hThread) return false;
    PWSTR desc = nullptr;
    bool isMtView = false;
    if (SUCCEEDED(fnGetDesc(hThread, &desc)) && desc) {
        if (StrStrIW(desc, L"MultitaskingView") != nullptr) {
            isMtView = true;
        }
        LocalFree(desc);
    }
    CloseHandle(hThread);
    return isMtView;
}

// Kiểm tra chính xác tuyệt đối cửa sổ OSD Âm lượng / Độ sáng nhỏ ở giữa đáy màn hình
// (Loại trừ 100% Alt+Tab, Win+Tab, Win+Space, Snap Assist và mọi flyout hệ thống!)
bool IsWindowsHardwareOsdWindow(HWND hWnd, int xCand, int yCand, int cxCand, int cyCand, UINT swpFlags, bool fromSwp) {
    if (!hWnd || !g_running.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }
    if (!g_hideDefaultOsdAtomic.load(std::memory_order_relaxed)) {
        return false;
    }

    // Chặn ngay lập tức nếu người dùng đang giữ hoặc vừa nhấn Alt / Win (Alt+Tab, Win+Tab, Alt+Esc, Win+G...)
    if (g_llAltDown.load(std::memory_order_relaxed) || g_llWinDown.load(std::memory_order_relaxed)) {
        return false;
    }
    if (GetTickCount64() - g_lastAltOrWinKeyTick.load(std::memory_order_relaxed) < 650) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) return false;

    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) return false;

    wchar_t cls[64] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    if (_wcsicmp(cls, L"XamlExplorerHostIslandWindow") != 0) return false;

    using GetWindowBandFn = BOOL(WINAPI*)(HWND, PDWORD);
    static auto fnGetBand = reinterpret_cast<GetWindowBandFn>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetWindowBand"));
    DWORD band = 0;
    if (fnGetBand && fnGetBand(hWnd, &band)) {
        if (band == 7 /* ZBID_IMMERSIVE_MOGO */) return false;
        // OSD phần cứng của Windows 11 luôn thuộc ZBID_ABOVELOCK_UX (16), nhưng KHÔNG return true sớm trước khi đo kích thước!
        if (band != 16 && IsMultitaskingViewThread(tid)) return false;
    } else if (IsMultitaskingViewThread(tid)) {
        return false;
    }

    UINT dpi = GetDpiForWindow(hWnd);
    if (dpi == 0) dpi = 96;

    RECT rc{};
    GetWindowRect(hWnd, &rc);
    int x = (fromSwp && !(swpFlags & SWP_NOMOVE)) ? xCand : rc.left;
    int y = (fromSwp && !(swpFlags & SWP_NOMOVE)) ? yCand : rc.top;
    int w = (fromSwp && !(swpFlags & SWP_NOSIZE) && cxCand > 0) ? cxCand : (rc.right - rc.left);
    int h = (fromSwp && !(swpFlags & SWP_NOSIZE) && cyCand > 0) ? cyCand : (rc.bottom - rc.top);

    // Kích thước chuẩn của thanh OSD Âm lượng/Độ sáng Windows 11 (~200-260 x 48-76 DIP); Alt+Tab lớn hơn rất nhiều!
    if (w < DpiScale(130, dpi) || w > DpiScale(350, dpi) ||
        h < DpiScale(32, dpi)  || h > DpiScale(104, dpi)) {
        return false;
    }

    // Nếu tọa độ đang nằm trên màn hình (không phải -32000), kiểm tra đúng vị trí căn giữa sát đáy màn hình
    if (x > -10000 && y > -10000) {
        POINT ptCenter = { x + w / 2, y + h / 2 };
        HMONITOR hMon = MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        if (hMon && GetMonitorInfoW(hMon, &mi)) {
            int monMidX = (mi.rcMonitor.left + mi.rcMonitor.right) / 2;
            int distFromCenter = std::abs((x + w / 2) - monMidX);
            int distFromBottom = mi.rcMonitor.bottom - (y + h);
            if (distFromCenter > DpiScale(60, dpi) ||
                distFromBottom < -DpiScale(8, dpi) ||
                distFromBottom > DpiScale(185, dpi)) {
                return false;
            }
        }
    }
    return true;
}

void NotifyHardwareOsdShowAttempt() {
    ULONGLONG now = GetTickCount64();
    if (g_llAltDown.load(std::memory_order_relaxed) ||
        g_llWinDown.load(std::memory_order_relaxed) ||
        (now - g_lastAltOrWinKeyTick.load(std::memory_order_relaxed) < 700)) {
        return;
    }
    ULONGLONG prev = g_lastOsdInterceptTick.load(std::memory_order_relaxed);
    if (now - prev >= 90) {
        g_lastOsdInterceptTick.store(now, std::memory_order_relaxed);
        if (g_alertHwnd && IsWindow(g_alertHwnd)) {
            PostMessageW(g_alertHwnd, WM_APP_OSD_INTERCEPTED, 0, 0);
        }
    }
}

BOOL WINAPI HookedShowWindow(HWND hWnd, int nCmdShow) {
    if (nCmdShow != SW_HIDE && IsWindowsHardwareOsdWindow(hWnd, 0, 0, 0, 0, 0, false)) {
        NotifyHardwareOsdShowAttempt();
        return pOrigShowWindow ? pOrigShowWindow(hWnd, SW_HIDE) : TRUE;
    }
    return pOrigShowWindow ? pOrigShowWindow(hWnd, nCmdShow) : FALSE;
}

BOOL WINAPI HookedSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (IsWindowsHardwareOsdWindow(hWnd, X, Y, cx, cy, uFlags, true)) {
        if ((uFlags & SWP_SHOWWINDOW) || (X > -10000 && Y > -10000 && !(uFlags & SWP_HIDEWINDOW))) {
            NotifyHardwareOsdShowAttempt();
        }
        UINT cleanFlags = (uFlags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW | SWP_NOACTIVATE;
        cleanFlags &= ~SWP_NOMOVE;
        BOOL ok = pOrigSetWindowPos
            ? pOrigSetWindowPos(hWnd, hWndInsertAfter, -32000, -32000, cx, cy, cleanFlags)
            : TRUE;
        BOOL cloak = TRUE;
        DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
        return ok;
    }
    return pOrigSetWindowPos
        ? pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags)
        : FALSE;
}

void SweepExistingHardwareOsdWindows() {
    if (!g_hideDefaultOsdAtomic.load(std::memory_order_relaxed)) return;
    HWND hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, L"XamlExplorerHostIslandWindow", nullptr)) != nullptr) {
        if (IsWindowsHardwareOsdWindow(hWnd, 0, 0, 0, 0, 0, false)) {
            BOOL cloak = TRUE;
            DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
            if (pOrigSetWindowPos) {
                pOrigSetWindowPos(hWnd, nullptr, -32000, -32000, 0, 0,
                                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
            } else if (pOrigShowWindow) {
                pOrigShowWindow(hWnd, SW_HIDE);
            }
        }
    }
}

BOOL WINAPI HookedSetWindowCompositionAttribute(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA* pData) {
    if (pData && pData->Attrib == 19 /* WCA_ACCENT_POLICY */ && pData->pvData &&
        pData->cbData >= sizeof(ACCENT_POLICY) &&
        g_running.load(std::memory_order_relaxed) &&
        !g_unloading.load(std::memory_order_relaxed) &&
        g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) &&
        g_xamlBgCollapsed.load(std::memory_order_relaxed)) {
        wchar_t cls[32] = {};
        if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            ACCENT_POLICY clearPolicy{};
            clearPolicy.AccentState = 2; // ACCENT_ENABLE_TRANSPARENTGRADIENT
            clearPolicy.AccentFlags = 2;
            clearPolicy.GradientColor = 0x00000000;
            WINDOWCOMPOSITIONATTRIBDATA customData = { 19, &clearPolicy, sizeof(clearPolicy) };
            return pOrigSetWindowCompositionAttribute(hWnd, &customData);
        }
    }
    return pOrigSetWindowCompositionAttribute ? pOrigSetWindowCompositionAttribute(hWnd, pData) : FALSE;
}

void ApplyTaskbarClearComposition(HWND hTaskbar, bool clear) {
    if (!hTaskbar || !IsWindow(hTaskbar) || !pOrigSetWindowCompositionAttribute) return;
    ACCENT_POLICY policy{};
    if (clear) {
        policy.AccentState = 2;
        policy.AccentFlags = 2;
        policy.GradientColor = 0x00000000;
    } else {
        policy.AccentState = 4;
        policy.AccentFlags = 2;
        policy.GradientColor = 0xCC181820;
    }
    WINDOWCOMPOSITIONATTRIBDATA data = { 19, &policy, sizeof(policy) };
    pOrigSetWindowCompositionAttribute(hTaskbar, &data);
}

// ---------------------------------------------------------------------------
// 3. Luồng Bàn Phím WH_KEYBOARD_LL Độc Lập Siêu Nhẹ (< 1us callback, 0% CPU khi nghỉ)
//    - Bắt tức thì Caps Lock, Num Lock, Scroll Lock, Insert (<10ms)
//    - Bắt tức thì Ctrl+X / Shift+Del, Ctrl+V / Shift+Ins, Phím Âm lượng & Alt+F4
// ---------------------------------------------------------------------------
static HHOOK g_hLowLevelKbdHook = nullptr;

LRESULT CALLBACK LowLevelKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static bool s_keyDown[256] = {};
    static bool s_insToggleState = false;

    if (nCode == HC_ACTION && lParam != 0 &&
        g_running.load(std::memory_order_relaxed) &&
        !g_unloading.load(std::memory_order_relaxed)) {
        const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        DWORD vk = kb->vkCode;
        bool isUp = (kb->flags & LLKHF_UP) != 0 || (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
        ULONGLONG now = GetTickCount64();

        if (vk < 256) {
            bool wasDown = s_keyDown[vk];
            s_keyDown[vk] = !isUp;

            // Cập nhật trạng thái phím bổ trợ (Modifier) chính xác theo phần cứng
            if (vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_CONTROL) {
                g_llCtrlDown.store(s_keyDown[VK_LCONTROL] || s_keyDown[VK_RCONTROL] || s_keyDown[VK_CONTROL], std::memory_order_relaxed);
            } else if (vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT) {
                g_llShiftDown.store(s_keyDown[VK_LSHIFT] || s_keyDown[VK_RSHIFT] || s_keyDown[VK_SHIFT], std::memory_order_relaxed);
            } else if (vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU) {
                bool altNow = s_keyDown[VK_LMENU] || s_keyDown[VK_RMENU] || s_keyDown[VK_MENU];
                g_llAltDown.store(altNow, std::memory_order_relaxed);
                g_lastAltOrWinKeyTick.store(now, std::memory_order_relaxed);
            } else if (vk == VK_LWIN || vk == VK_RWIN) {
                bool winNow = s_keyDown[VK_LWIN] || s_keyDown[VK_RWIN];
                g_llWinDown.store(winNow, std::memory_order_relaxed);
                g_lastAltOrWinKeyTick.store(now, std::memory_order_relaxed);
            }

            if (!isUp && (g_llAltDown.load(std::memory_order_relaxed) || g_llWinDown.load(std::memory_order_relaxed) || vk == VK_TAB)) {
                if (g_llAltDown.load(std::memory_order_relaxed) || g_llWinDown.load(std::memory_order_relaxed)) {
                    g_lastAltOrWinKeyTick.store(now, std::memory_order_relaxed);
                }
            }

            if (vk == VK_VOLUME_UP || vk == VK_VOLUME_DOWN || vk == VK_VOLUME_MUTE) {
                g_lastVolKeyPressTick.store(now, std::memory_order_relaxed);
            }

            // Chỉ xử lý cạnh xuống đầu tiên của phím (bỏ qua auto-repeat khi giữ đè phím)
            if (!isUp && !wasDown) {
                HWND hAlert = g_alertHwnd;
                bool ctrl  = g_llCtrlDown.load(std::memory_order_relaxed);
                bool shift = g_llShiftDown.load(std::memory_order_relaxed);
                bool alt   = g_llAltDown.load(std::memory_order_relaxed);
                bool win   = g_llWinDown.load(std::memory_order_relaxed);

                // 1. Caps Lock, Num Lock, Scroll Lock, Insert -> Phản hồi tức khắc <10ms!
                if ((vk == VK_CAPITAL || vk == VK_NUMLOCK || vk == VK_SCROLL) && !ctrl && !alt && !win) {
                    if (g_enableKeyLockAlertAtomic.load(std::memory_order_relaxed) && hAlert) {
                        // Trong WH_KEYBOARD_LL, GetKeyState(vk) & 1 phản ánh trạng thái TRƯỚC khi phím vừa nhấn đổi chiều!
                        bool nextOn = !((GetKeyState(static_cast<int>(vk)) & 1) != 0);
                        PostMessageW(hAlert, WM_APP_KEYLOCK_EVENT, static_cast<WPARAM>(vk), nextOn ? 1 : 0);
                    }
                } else if (vk == VK_INSERT && !ctrl && !shift && !alt && !win) {
                    if (g_enableKeyLockAlertAtomic.load(std::memory_order_relaxed) && hAlert) {
                        s_insToggleState = !s_insToggleState;
                        PostMessageW(hAlert, WM_APP_KEYLOCK_EVENT, static_cast<WPARAM>(VK_INSERT), s_insToggleState ? 1 : 0);
                    }
                }
                // 2. Phím Âm lượng (đảm bảo hiện ngay cả khi đang ở 0% hoặc 100%!)
                else if (vk == VK_VOLUME_UP || vk == VK_VOLUME_DOWN || vk == VK_VOLUME_MUTE) {
                    g_lastVolKeyPressTick.store(now, std::memory_order_relaxed);
                    if (g_enableVolumeAlertAtomic.load(std::memory_order_relaxed) && hAlert) {
                        PostMessageW(hAlert, WM_APP_VOLKEY_EVENT, static_cast<WPARAM>(vk), 0);
                    }
                }
                // 3. Tổ hợp Alt + F4 đóng ứng dụng!
                else if (vk == VK_F4 && alt && !ctrl && !win) {
                    if (g_enableAppExitAlertAtomic.load(std::memory_order_relaxed) && hAlert) {
                        PostMessageW(hAlert, WM_APP_ALTF4_PRESSED, 0, 0);
                    }
                }
                // 4. Tổ hợp Cắt (Ctrl+X / Shift+Delete) & Dán (Ctrl+V / Shift+Insert)
                else if (g_enableClipboardAlertAtomic.load(std::memory_order_relaxed)) {
                    if ((ctrl && !alt && !win && vk == 'X') || (shift && !ctrl && !alt && !win && vk == VK_DELETE)) {
                        g_lastCutShortcutTick.store(now, std::memory_order_relaxed);
                    } else if ((ctrl && !alt && !win && vk == 'V') || (shift && !ctrl && !alt && !win && vk == VK_INSERT)) {
                        if (hAlert) {
                            PostMessageW(hAlert, WM_APP_PASTE_EVENT, 0, 0);
                        }
                    }
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI KbdHookThreadProc(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    // Khởi tạo hàng đợi thông điệp luồng trước khi đăng ký WH_KEYBOARD_LL
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hLowLevelKbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardHookProc, GetModInstanceHandle(), 0);

    while (g_running.load(std::memory_order_relaxed) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hLowLevelKbdHook) {
        UnhookWindowsHookEx(g_hLowLevelKbdHook);
        g_hLowLevelKbdHook = nullptr;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// 4. Tích hợp Trực tiếp Cây XAML Taskbar.dll + Theo dõi Tiến trình TaskbandHWND:
//    - Gắn trực tiếp 2 viên thuốc Liquid Glass (Thanh ứng dụng & Khay trạng thái) vào XAML gốc
//      ở lớp đáy ZIndex = -1000 -> KHÔNG BAO GIỜ mất nền (Hình 2,3,5) hay đè lên icon (Hình 4)!
//    - Khi hiện Popup 3s: ẩn sạch riêng Thanh ứng dụng + icon ứng dụng qua XAML Opacity,
//      giữ nguyên 100% Khay trạng thái (Hình 1)!
//    - Lắng nghe thụ động TaskbandHWND (WM_USER + 64 / 65) để lấy tỷ lệ tiến trình (vd: 77%)!
// ---------------------------------------------------------------------------
struct MeasuredXamlTaskbarBounds {
    bool hasApps = false;
    bool hasTray = false;
    RECT rcAppsLocal{};
    RECT rcTrayLocal{};
};

static std::mutex g_xamlBoundsMutex;
static MeasuredXamlTaskbarBounds g_measuredBounds;

// Dữ liệu mặt kính khúc xạ chia sẻ từ luồng UI sang luồng XAML của Taskbar
struct SharedTaskbarXamlGlassPixels {
    uint64_t revision = 0;
    bool isLight = false;
    int appsW = 0, appsH = 0;
    std::vector<DWORD> appsPixels;
    int trayW = 0, trayH = 0;
    std::vector<DWORD> trayPixels;
};
static std::mutex g_xamlGlassPixelsMutex;
static SharedTaskbarXamlGlassPixels g_sharedXamlGlassPixels;

// Các đối tượng XAML chỉ được tạo/sửa/giải phóng trên chính luồng UI của Shell_TrayWnd!
[[clang::no_destroy]] static wuxc::Panel  g_xamlHostPanel{ nullptr };
[[clang::no_destroy]] static wuxc::Canvas g_xamlGlassCanvas{ nullptr };
[[clang::no_destroy]] static wuxc::Border g_xamlAppsPillBorder{ nullptr };
wux::FrameworkElement g_xamlTrayFrameFe{ nullptr };
wuxc::Panel g_xamlBgParentPanel{ nullptr };
[[clang::no_destroy]] static wuxc::Border g_xamlTrayPillBorder{ nullptr };
[[clang::no_destroy]] static wuxm::Imaging::WriteableBitmap g_xamlAppsBitmap{ nullptr };
[[clang::no_destroy]] static wuxm::Imaging::WriteableBitmap g_xamlTrayBitmap{ nullptr };
static std::vector<winrt::weak_ref<wux::UIElement>> g_hiddenAppUiElements;
static uint64_t g_appliedXamlGlassRevision = 0;
static int g_appliedAppsBmpW = 0, g_appliedAppsBmpH = 0;
static int g_appliedTrayBmpW = 0, g_appliedTrayBmpH = 0;
static int g_appliedRimStyle = -1;

// Theo dõi LayoutUpdated thời gian thực (~60fps) + dịch chuyển Widget mặc định của Windows sang 1 bên
[[clang::no_destroy]] static wux::FrameworkElement g_xamlLayoutWatchTarget{ nullptr };
[[clang::no_destroy]] static wux::FrameworkElement::LayoutUpdated_revoker g_xamlLayoutRevoker;
static bool g_inXamlLayoutSync = false;
static ULONGLONG g_lastLayoutSyncTick = 0;

[[clang::no_destroy]] static wux::FrameworkElement g_xamlWidgetsButton{ nullptr };
static double g_appliedWidgetsShiftX = 0.0;
static double g_appliedWidgetsShiftY = 0.0;
static bool   g_appliedWidgetsHidden = false;
static double g_cachedWidgetsNatX = -1.0;
static double g_cachedWidgetsNatY = -1.0;
static std::atomic<bool> g_cornerPillVisibleAtomic{false};
static std::atomic<int>  g_cornerPillEndDipAtomic{0};

static bool      g_fastAnimTimerActive = false;
static ULONGLONG g_fastAnimStopTick = 0;

// Trạng thái hình học đã áp dụng cho 2 viên thuốc XAML (chỉ cập nhật khi thay đổi thực sự để không chặn animation gốc của TaskbarFrameRepeater!)
static double    g_appliedAppsRad = -1.0;
static double    g_appliedTrayRad = -1.0;

// Hook luồng của Shell_TrayWnd (vừa nhận thông điệp đồng bộ XAML <0.3ms, vừa đọc TaskbandHWND 77%!)
static HHOOK g_taskbarThreadCallHook = nullptr;
static HHOOK g_taskbarThreadGetHook  = nullptr;
static HWND  g_observedTaskbandHwnd  = nullptr;
static UINT  g_wmTaskbarFastSync     = 0;

// Dữ liệu tiến trình tác vụ từ TaskbandHWND (ITaskbarList3: WM_USER + 64 / 65)
struct RawTaskbandProgressEntry {
    uintptr_t hwndKey = 0;
    int percent = -1;
    unsigned state = 0; // 1=Indeterminate, 2=Normal, 4=Error, 8=Paused
    ULONGLONG startedTick = 0;
    ULONGLONG lastUpdateTick = 0;
    ULONGLONG reached100Tick = 0;
    uint64_t epoch = 0;
    bool startNotified = false;
    bool completedNotified = false;
};
static std::mutex g_taskbandMutex;
static std::map<uintptr_t, RawTaskbandProgressEntry> g_taskbandEntries;
static uint64_t g_taskbandEpochCounter = 0;

int ComputeTaskbandPercent(LPARAM lParam) {
    int64_t val = static_cast<int64_t>(lParam);
    if (val < 0 || val > 65534) return -1;
    return static_cast<int>((val * 100 + 32767) / 65534);
}

void ObserveTaskbandProgressMessage(HWND destHwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    if (!g_running.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) return;
    if (!g_enableTaskProgressBadgeAtomic.load(std::memory_order_relaxed)) return;
    if (!g_observedTaskbandHwnd || destHwnd != g_observedTaskbandHwnd) return;
    if (msg != (WM_USER + 64) && msg != (WM_USER + 65)) return;

    HWND hTarget = reinterpret_cast<HWND>(wParam);
    if (!hTarget || !IsWindow(hTarget)) return;

    if (msg == (WM_USER + 64) && (lParam < 0 || lParam > 65534)) return;
    if (msg == (WM_USER + 65) && lParam != 0 && lParam != 1 && lParam != 2 && lParam != 4 && lParam != 8) return;

    bool notifyUi = false;
    WPARAM completedWndParam = 0;
    try {
        ULONGLONG now = GetTickCount64();
        std::lock_guard<std::mutex> lock(g_taskbandMutex);
        uintptr_t key = static_cast<uintptr_t>(wParam);
        auto it = g_taskbandEntries.find(key);
        if (it == g_taskbandEntries.end()) {
            if (lParam == 0 || g_taskbandEntries.size() >= 96) return;
            it = g_taskbandEntries.emplace(key, RawTaskbandProgressEntry{ key, -1, 0, now, now, 0, ++g_taskbandEpochCounter, false, false }).first;
        }
        auto prev = it->second;
        if (msg == (WM_USER + 64)) {
            if (lParam == 0 && (prev.state == 0 || prev.percent >= 95)) {
                if (prev.startNotified && !prev.completedNotified && prev.percent >= 95) {
                    completedWndParam = wParam;
                }
                g_taskbandEntries.erase(it);
                notifyUi = true;
            } else {
                int pct = ComputeTaskbandPercent(lParam);
                it->second.percent = pct;
                it->second.lastUpdateTick = now;
                if (lParam > 0 && it->second.state == 0) {
                    it->second.state = 2;
                    it->second.startedTick = now;
                }
                if (pct >= 100) {
                    if (it->second.reached100Tick == 0) {
                        it->second.reached100Tick = now;
                    }
                    if (prev.percent >= 0 && prev.percent < 100 && prev.startNotified && !it->second.completedNotified) {
                        it->second.completedNotified = true;
                        completedWndParam = wParam;
                    }
                } else {
                    it->second.reached100Tick = 0;
                    if (pct < 90) it->second.completedNotified = false;
                }
                if (prev.percent != it->second.percent || prev.state != it->second.state) {
                    notifyUi = true;
                }
            }
        } else {
            unsigned nextState = static_cast<unsigned>(lParam);
            if (nextState == 0) {
                if (prev.startNotified && !prev.completedNotified && prev.percent >= 85) {
                    completedWndParam = wParam;
                }
                g_taskbandEntries.erase(it);
                notifyUi = true;
            } else {
                if (prev.state == 0) {
                    it->second.startedTick = now;
                    it->second.epoch = ++g_taskbandEpochCounter;
                }
                it->second.lastUpdateTick = now;
                it->second.state = nextState;
                if (nextState == 1) {
                    it->second.percent = -1;
                    it->second.reached100Tick = 0;
                }
                if (prev.state != nextState) notifyUi = true;
            }
        }
    } catch (...) {}

    if ((notifyUi || completedWndParam != 0) && g_alertHwnd && IsWindow(g_alertHwnd)) {
        PostMessageW(g_alertHwnd, WM_APP_TASK_PROGRESS_EVENT, completedWndParam, 0);
    }
}

static void* g_CTaskBand_ITaskListWndSite_vftable = nullptr;
using CTaskBand_GetTaskbarHost_t = void**(WINAPI*)(void* pThis, void** result);
static CTaskBand_GetTaskbarHost_t g_CTaskBand_GetTaskbarHost_Original = nullptr;
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
static TaskbarHost_FrameHeight_t g_TaskbarHost_FrameHeight_Original = nullptr;
using Std_Ref_Decref_t = void(WINAPI*)(void* pThis);
static Std_Ref_Decref_t g_Std_Ref_Decref_Original = nullptr;
using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
static TrayUI_StartTaskbar_t g_TrayUI_StartTaskbar_Original = nullptr;

static std::atomic<bool> g_taskbarSymbolsAttempted{false};
static std::atomic<bool> g_taskbarSymbolsReady{false};
static bool g_ownerTimerInstalled = false;

void SyncTaskbarXamlOnOwnerThread(HWND hTaskbar, bool restoreDefault);
void StartTaskbarFastAnimBurst(HWND hTaskbar, ULONGLONG durationMs = 460);
void StartTaskbarContextMenuInjectBurst(HWND hTaskbar);
void ApplySharedGlassPixelsOnOwnerThread();
void ApplyAppsVisibilityOnOwnerThread(HWND hTaskbar);

LRESULT CALLBACK TaskbarOwnerCallWndHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && lParam != 0) {
        const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        ObserveTaskbandProgressMessage(cwp->hwnd, cwp->message, cwp->wParam, cwp->lParam);
        if (cwp->message == WM_CONTEXTMENU || cwp->message == WM_RBUTTONUP ||
            cwp->message == WM_NCRBUTTONUP || cwp->message == WM_POINTERUP) {
            if (HWND hTb = FindCurrentProcessTaskbarWnd()) {
                StartTaskbarContextMenuInjectBurst(hTb);
            }
        }
        if (g_wmTaskbarFastSync != 0 && cwp->message == g_wmTaskbarFastSync) {
            if (cwp->wParam == 1) {
                ApplyAppsVisibilityOnOwnerThread(cwp->hwnd);
            } else if (cwp->wParam == 2) {
                ApplySharedGlassPixelsOnOwnerThread();
            } else if (cwp->wParam == 3) {
                bool needSync = g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
                                g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
                SyncTaskbarXamlOnOwnerThread(cwp->hwnd, !needSync);
                if (needSync) StartTaskbarFastAnimBurst(cwp->hwnd, 460);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK TaskbarOwnerGetMsgHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == PM_REMOVE && lParam != 0) {
        const auto* msg = reinterpret_cast<const MSG*>(lParam);
        ObserveTaskbandProgressMessage(msg->hwnd, msg->message, msg->wParam, msg->lParam);
        if (msg->message == WM_CONTEXTMENU || msg->message == WM_RBUTTONUP ||
            msg->message == WM_NCRBUTTONUP || msg->message == WM_POINTERUP) {
            if (HWND hTb = FindCurrentProcessTaskbarWnd()) {
                StartTaskbarContextMenuInjectBurst(hTb);
            }
        }
        if (g_wmTaskbarFastSync != 0 && msg->message == g_wmTaskbarFastSync) {
            if (msg->wParam == 1) {
                ApplyAppsVisibilityOnOwnerThread(msg->hwnd);
            } else if (msg->wParam == 2) {
                ApplySharedGlassPixelsOnOwnerThread();
            } else if (msg->wParam == 3) {
                bool needSync = g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
                                g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
                SyncTaskbarXamlOnOwnerThread(msg->hwnd, !needSync);
                if (needSync) StartTaskbarFastAnimBurst(msg->hwnd, 460);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void EnsureTaskbarOwnerThreadHooks(HWND hTaskbar, bool enable) {
    if (!enable) {
        if (g_taskbarThreadCallHook) { UnhookWindowsHookEx(g_taskbarThreadCallHook); g_taskbarThreadCallHook = nullptr; }
        if (g_taskbarThreadGetHook)  { UnhookWindowsHookEx(g_taskbarThreadGetHook);  g_taskbarThreadGetHook  = nullptr; }
        g_observedTaskbandHwnd = nullptr;
        return;
    }
    if (!hTaskbar || GetWindowThreadProcessId(hTaskbar, nullptr) != GetCurrentThreadId()) return;
    if (g_wmTaskbarFastSync == 0) {
        g_wmTaskbarFastSync = RegisterWindowMessageW(L"Windhawk_FastXamlSync_DynamicTaskbarLiveAlertsV8");
    }
    g_observedTaskbandHwnd = reinterpret_cast<HWND>(GetPropW(hTaskbar, L"TaskbandHWND"));
    if (!g_taskbarThreadCallHook) {
        g_taskbarThreadCallHook = SetWindowsHookExW(WH_CALLWNDPROC, TaskbarOwnerCallWndHookProc, nullptr, GetCurrentThreadId());
    }
    if (!g_taskbarThreadGetHook) {
        g_taskbarThreadGetHook = SetWindowsHookExW(WH_GETMESSAGE, TaskbarOwnerGetMsgHookProc, nullptr, GetCurrentThreadId());
    }
}

void PostFastSyncToTaskbar(WPARAM action) {
    HWND hTb = FindCurrentProcessTaskbarWnd();
    if (hTb && IsWindow(hTb) && g_wmTaskbarFastSync != 0 && g_taskbarThreadGetHook != nullptr) {
        PostMessageW(hTb, g_wmTaskbarFastSync, action, 0);
    }
}

using WindowThreadProc_t = void(*)(void* param);
bool RunOnWindowThread(HWND hWnd, WindowThreadProc_t proc, void* param) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    static const UINT s_msg = RegisterWindowMessageW(L"Windhawk_RunOnThread_DynamicTaskbarLiveAlertsV8");
    struct CallData {
        WindowThreadProc_t proc;
        void* param;
    };

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (tid == 0) return false;
    if (tid == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    struct HookCb {
        static LRESULT CALLBACK Proc(int nCode, WPARAM wParam, LPARAM lParam) {
            if (nCode == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp && cwp->message == s_msg && cwp->lParam) {
                    auto* cd = reinterpret_cast<CallData*>(cwp->lParam);
                    cd->proc(cd->param);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }
    };
    HHOOK hHook = SetWindowsHookExW(WH_CALLWNDPROC, HookCb::Proc, nullptr, tid);
    if (!hHook) return false;

    CallData cd{ proc, param };
    SendMessageW(hWnd, s_msg, 0, reinterpret_cast<LPARAM>(&cd));
    UnhookWindowsHookEx(hHook);
    return true;
}

wux::XamlRoot ExtractTaskbarXamlRoot(HWND hTaskbar) {
    if (!hTaskbar || !g_taskbarSymbolsReady.load(std::memory_order_relaxed) ||
        !g_CTaskBand_ITaskListWndSite_vftable ||
        !g_CTaskBand_GetTaskbarHost_Original ||
        !g_Std_Ref_Decref_Original) {
        return nullptr;
    }

    HWND hTaskSw = reinterpret_cast<HWND>(GetPropW(hTaskbar, L"TaskbandHWND"));
    if (!hTaskSw) return nullptr;

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSw, 0));
    if (!taskBand) return nullptr;

    void* sitePtr = taskBand;
    for (int i = 0; i < 20; ++i) {
        if (*reinterpret_cast<void**>(sitePtr) == g_CTaskBand_ITaskListWndSite_vftable) {
            break;
        }
        sitePtr = reinterpret_cast<void**>(sitePtr) + 1;
        if (i == 19) return nullptr;
    }

    void* hostShared[2] = {};
    g_CTaskBand_GetTaskbarHost_Original(sitePtr, hostShared);
    struct RefGuard {
        void* ctrl;
        ~RefGuard() { if (ctrl && g_Std_Ref_Decref_Original) g_Std_Ref_Decref_Original(ctrl); }
    } guard{ hostShared[1] };

    if (!hostShared[0]) return nullptr;

    size_t elemOffset = 0x48;
    if (g_TaskbarHost_FrameHeight_Original) {
        const BYTE* b = reinterpret_cast<const BYTE*>(g_TaskbarHost_FrameHeight_Original);
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC &&
            b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            elemOffset = b[7];
        }
    }

    auto* candidateSlot = reinterpret_cast<IUnknown**>(reinterpret_cast<BYTE*>(hostShared[0]) + elemOffset);
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(candidateSlot, &mbi, sizeof(mbi)) == 0 || mbi.State != MEM_COMMIT) {
        return nullptr;
    }

    IUnknown* elemUnk = *candidateSlot;
    if (!elemUnk) return nullptr;

    wux::FrameworkElement fe{ nullptr };
    if (FAILED(elemUnk->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(fe))) || !fe) {
        return nullptr;
    }
    return fe.XamlRoot();
}

wuxm::LinearGradientBrush BuildXamlGlassRimBrush(bool isLight) {
    wuxm::LinearGradientBrush grad;
    grad.StartPoint({ 0.0, 0.0 });
    grad.EndPoint({ 1.0, 1.0 });

    auto addStop = [&](double offset, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
        wuxm::GradientStop st;
        st.Offset(offset);
        st.Color(winrt::Windows::UI::Color{ a, r, g, b });
        grad.GradientStops().Append(st);
    };

    if (isLight) {
        addStop(0.00, 155, 255, 255, 255);
        addStop(0.30,  82, 185, 228, 255);
        addStop(0.52,  46,  40,  50,  70);
        addStop(0.76,  76, 255, 210, 240);
        addStop(1.00, 128, 255, 255, 255);
    } else {
        addStop(0.00, 168, 255, 255, 255);
        addStop(0.30,  88, 185, 228, 255);
        addStop(0.52,  42, 255, 255, 255);
        addStop(0.76,  80, 255, 210, 240);
        addStop(1.00, 132, 255, 255, 255);
    }
    return grad;
}

bool UpdateWriteableBitmapOnXamlThread(wuxm::Imaging::WriteableBitmap& ioBmp, int& ioW, int& ioH,
                                       const std::vector<DWORD>& srcPixels, int w, int h) {
    if (w <= 4 || h <= 4 || srcPixels.size() != static_cast<size_t>(w) * h) return false;
    try {
        if (!ioBmp || ioW != w || ioH != h) {
            ioBmp = wuxm::Imaging::WriteableBitmap(w, h);
            ioW = w;
            ioH = h;
        }
        BYTE* dst = nullptr;
        if (SUCCEEDED(ioBmp.PixelBuffer().as<IWhBufferByteAccess>()->Buffer(&dst)) && dst) {
            std::memcpy(dst, srcPixels.data(), srcPixels.size() * sizeof(DWORD));
            ioBmp.Invalidate();
            return true;
        }
    } catch (...) {}
    return false;
}

static std::atomic<bool> g_adaptiveBackdropIsLight{false};
static bool ComputeTaskbarGlassBackdropIsLight(bool curIsLight);
static void ApplyContrastToSystemTrayBattery(wux::FrameworkElement const& trayRoot, wux::ElementTheme wantTheme);

void ApplySharedGlassPixelsOnOwnerThread() {
    if (!g_xamlAppsPillBorder || !g_xamlTrayPillBorder) return;

    SharedTaskbarXamlGlassPixels snap;
    {
        std::lock_guard<std::mutex> lock(g_xamlGlassPixelsMutex);
        if (g_sharedXamlGlassPixels.revision == 0 ||
            g_sharedXamlGlassPixels.revision == g_appliedXamlGlassRevision) {
            return;
        }
        snap = g_sharedXamlGlassPixels;
    }

    try {
        int rimStyle = snap.isLight ? 1 : 0;
        if (rimStyle != g_appliedRimStyle) {
            g_xamlAppsPillBorder.BorderBrush(BuildXamlGlassRimBrush(snap.isLight));
            g_xamlTrayPillBorder.BorderBrush(BuildXamlGlassRimBrush(snap.isLight));
            g_appliedRimStyle = rimStyle;
        }

        if (UpdateWriteableBitmapOnXamlThread(g_xamlAppsBitmap, g_appliedAppsBmpW, g_appliedAppsBmpH,
                                              snap.appsPixels, snap.appsW, snap.appsH)) {
            wuxm::ImageBrush br;
            br.ImageSource(g_xamlAppsBitmap);
            br.Stretch(wuxm::Stretch::Fill);
            g_xamlAppsPillBorder.Background(br);
        }

        if (UpdateWriteableBitmapOnXamlThread(g_xamlTrayBitmap, g_appliedTrayBmpW, g_appliedTrayBmpH,
                                              snap.trayPixels, snap.trayW, snap.trayH)) {
            wuxm::ImageBrush br;
            br.ImageSource(g_xamlTrayBitmap);
            br.Stretch(wuxm::Stretch::Fill);
            g_xamlTrayPillBorder.Background(br);
        }

        g_appliedXamlGlassRevision = snap.revision;

        // Cập nhật màu chữ/icon tương phản ngay lập tức cùng khung hình với nền kính mới (0ms delay!)
        if (g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed)) {
            bool backdropIsLight = ComputeTaskbarGlassBackdropIsLight(g_adaptiveBackdropIsLight.load(std::memory_order_relaxed));
            wux::ElementTheme wantTheme = backdropIsLight ? wux::ElementTheme::Light : wux::ElementTheme::Dark;
            if (g_xamlTrayFrameFe) {
                if (g_xamlTrayFrameFe.RequestedTheme() != wantTheme) g_xamlTrayFrameFe.RequestedTheme(wantTheme);
                ApplyContrastToSystemTrayBattery(g_xamlTrayFrameFe, wantTheme);
            }
            if (g_xamlBgParentPanel) {
                if (g_xamlBgParentPanel.RequestedTheme() != wantTheme) g_xamlBgParentPanel.RequestedTheme(wantTheme);
            }
        }
    } catch (...) {}
}

void ResetWidgetsButtonShiftOnOwnerThread() {
    if (g_xamlWidgetsButton) {
        try {
            if (g_appliedWidgetsHidden) {
                g_xamlWidgetsButton.Opacity(1.0);
                g_xamlWidgetsButton.IsHitTestVisible(true);
            }
            if (g_appliedWidgetsShiftX != 0.0 || g_appliedWidgetsShiftY != 0.0) {
                wuxm::TranslateTransform tt;
                tt.X(0.0);
                tt.Y(0.0);
                g_xamlWidgetsButton.RenderTransform(tt);
            }
        } catch (...) {}
        g_xamlWidgetsButton = nullptr;
    }
    g_appliedWidgetsShiftX = 0.0;
    g_appliedWidgetsShiftY = 0.0;
    g_appliedWidgetsHidden = false;
    g_cachedWidgetsNatX = -1.0;
    g_cachedWidgetsNatY = -1.0;
}

void RemoveInjectedTaskbarGlassOnOwnerThread() {
    g_xamlLayoutRevoker.revoke();
    g_xamlLayoutWatchTarget = nullptr;
    ResetWidgetsButtonShiftOnOwnerThread();
    for (const auto& wk : g_hiddenAppUiElements) {
        if (auto el = wk.get()) {
            try {
                el.Opacity(1.0);
                el.IsHitTestVisible(true);
            } catch (...) {}
        }
    }
    g_hiddenAppUiElements.clear();

    if (g_xamlGlassCanvas && g_xamlHostPanel) {
        try {
            uint32_t idx = 0;
            if (g_xamlHostPanel.Children().IndexOf(g_xamlGlassCanvas, idx)) {
                g_xamlHostPanel.Children().RemoveAt(idx);
            }
        } catch (...) {}
    }
    g_xamlAppsBitmap = nullptr;
    g_xamlTrayBitmap = nullptr;
    g_xamlAppsPillBorder = nullptr;
    g_xamlTrayFrameFe = nullptr;
    g_xamlBgParentPanel = nullptr;
    g_xamlTrayPillBorder = nullptr;
    g_xamlGlassCanvas = nullptr;
    g_xamlHostPanel = nullptr;
    g_appliedXamlGlassRevision = 0;
    g_appliedAppsBmpW = g_appliedAppsBmpH = 0;
    g_appliedTrayBmpW = g_appliedTrayBmpH = 0;
    g_appliedRimStyle = -1;
    g_appliedAppsRad = -1.0;
    g_appliedTrayRad = -1.0;
    g_xamlGlassInjected.store(false, std::memory_order_relaxed);
}

void ApplyAppsVisibilityOnOwnerThread(HWND hTaskbar) {
    bool hideApps = g_hideAppsPillAtomic.load(std::memory_order_relaxed) &&
                    g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed);
    try {
        if (g_xamlAppsPillBorder) {
            g_xamlAppsPillBorder.Opacity(hideApps ? 0.0 : 1.0);
        }
        if (!hideApps) {
            for (const auto& wk : g_hiddenAppUiElements) {
                if (auto el = wk.get()) {
                    try {
                        el.Opacity(1.0);
                        el.IsHitTestVisible(true);
                    } catch (...) {}
                }
            }
            g_hiddenAppUiElements.clear();
        } else {
            bool anyAlive = false;
            for (const auto& wk : g_hiddenAppUiElements) {
                if (auto el = wk.get()) {
                    try {
                        el.Opacity(0.0);
                        el.IsHitTestVisible(false);
                        anyAlive = true;
                    } catch (...) {}
                }
            }
            if (!anyAlive && hTaskbar) {
                SyncTaskbarXamlOnOwnerThread(hTaskbar, false);
            }
        }
    } catch (...) {}
}

bool TransformElementBoundsToRootDip(wux::FrameworkElement const& elem, wf::Rect& outDipRect) {
    try {
        double aw = elem.ActualWidth();
        double ah = elem.ActualHeight();
        if (aw <= 1.0 || ah <= 1.0) return false;

        auto xform = elem.TransformToVisual(nullptr);
        float fw = static_cast<float>(aw);
        float fh = static_cast<float>(ah);

        wf::Point corners[4] = {
            xform.TransformPoint({ 0.0f, 0.0f }),
            xform.TransformPoint({ fw,   0.0f }),
            xform.TransformPoint({ 0.0f, fh   }),
            xform.TransformPoint({ fw,   fh   })
        };

        float minX = corners[0].X, maxX = corners[0].X;
        float minY = corners[0].Y, maxY = corners[0].Y;
        for (int i = 1; i < 4; ++i) {
            minX = std::min(minX, corners[i].X);
            maxX = std::max(maxX, corners[i].X);
            minY = std::min(minY, corners[i].Y);
            maxY = std::max(maxY, corners[i].Y);
        }
        if (maxX <= minX + 1.5f || maxY <= minY + 1.5f) return false;
        outDipRect = { minX, minY, maxX - minX, maxY - minY };
        return true;
    } catch (...) {
        return false;
    }
}

RECT DipRectToTaskbarLocalPx(const wf::Rect& rDip, UINT dpi, int bridgeOffX, int bridgeOffY) {
    float scale = static_cast<float>(dpi) / 96.0f;
    return {
        bridgeOffX + static_cast<LONG>(std::lround(rDip.X * scale)),
        bridgeOffY + static_cast<LONG>(std::lround(rDip.Y * scale)),
        bridgeOffX + static_cast<LONG>(std::lround((rDip.X + rDip.Width) * scale)),
        bridgeOffY + static_cast<LONG>(std::lround((rDip.Y + rDip.Height) * scale))
    };
}

void CALLBACK TaskbarFastAnimTimerProc(HWND hWnd, UINT, UINT_PTR, DWORD) {
    if (!g_running.load(std::memory_order_relaxed) ||
        g_unloading.load(std::memory_order_relaxed) ||
        !g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
        GetTickCount64() >= g_fastAnimStopTick) {
        KillTimer(hWnd, kTaskbarFastAnimTimerId);
        g_fastAnimTimerActive = false;
        return;
    }
    SyncTaskbarXamlOnOwnerThread(hWnd, false);
}

void StartTaskbarFastAnimBurst(HWND hTaskbar, ULONGLONG durationMs) {
    if (!hTaskbar || !IsWindow(hTaskbar) ||
        !g_running.load(std::memory_order_relaxed) ||
        g_unloading.load(std::memory_order_relaxed) ||
        !g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed)) {
        return;
    }
    g_fastAnimStopTick = GetTickCount64() + std::min<ULONGLONG>(durationMs, 360);
    if (!g_fastAnimTimerActive) {
        // Nhịp 90ms nhẹ nhàng đủ bắt điểm đầu & điểm cuối animation mà tuyệt đối không chặn layout pass của Windows!
        g_fastAnimTimerActive = (SetTimer(hTaskbar, kTaskbarFastAnimTimerId, 90, TaskbarFastAnimTimerProc) != 0);
    }
}

static int g_menuInjectRemainingTicks = 0;

static bool StringContainsTaskbarMenuHint(const std::wstring& s) {
    if (s.empty()) return false;
    std::wstring low = ToLowerStr(s);
    return low.find(L"taskbar") != std::wstring::npos ||
           low.find(L"thanh tác vụ") != std::wstring::npos ||
           low.find(L"task manager") != std::wstring::npos ||
           low.find(L"taskmanager") != std::wstring::npos ||
           low.find(L"trình quản lý tác vụ") != std::wstring::npos ||
           low.find(L"task-manager") != std::wstring::npos ||
           low.find(L"barre des tâches") != std::wstring::npos ||
           low.find(L"barra de tareas") != std::wstring::npos ||
           low.find(L"タスク") != std::wstring::npos ||
           low.find(L"任务栏") != std::wstring::npos ||
           low.find(L"작업 표시줄") != std::wstring::npos;
}

bool TryInjectTaskbarContextMenuSettingsItem(HWND hTaskbar) {
    if (!hTaskbar || !IsWindow(hTaskbar) || g_unloading.load(std::memory_order_relaxed)) return false;
    try {
        wux::XamlRoot xamlRoot = ExtractTaskbarXamlRoot(hTaskbar);
        if (!xamlRoot) return false;
        auto popups = wuxm::VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot);
        if (!popups || popups.Size() == 0) return false;

        for (uint32_t pi = 0; pi < popups.Size(); ++pi) {
            auto popup = popups.GetAt(pi);
            if (!popup || !popup.IsOpen()) continue;
            auto rootChild = popup.Child();
            if (!rootChild) continue;

            // BFS inside open popup to find MenuFlyoutPresenter and check all text hints
            std::vector<wux::DependencyObject> q;
            q.push_back(rootChild);
            wuxc::MenuFlyoutPresenter presenter = nullptr;
            bool popupHasTaskbarHint = false;
            bool alreadyInjected = false;

            for (size_t qi = 0; qi < q.size() && qi < 180; ++qi) {
                auto cur = q[qi];
                if (!presenter) {
                    if (auto p = cur.try_as<wuxc::MenuFlyoutPresenter>()) {
                        presenter = p;
                    }
                }
                if (auto fe = cur.try_as<wux::FrameworkElement>()) {
                    std::wstring nm = fe.Name().c_str();
                    if (nm == L"WhModernTaskbarSettingsMenuItem") {
                        alreadyInjected = true;
                        break;
                    }
                    if (!popupHasTaskbarHint && StringContainsTaskbarMenuHint(nm)) {
                        popupHasTaskbarHint = true;
                    }
                }
                if (!popupHasTaskbarHint) {
                    if (auto mfi = cur.try_as<wuxc::MenuFlyoutItem>()) {
                        if (StringContainsTaskbarMenuHint(mfi.Text().c_str())) {
                            popupHasTaskbarHint = true;
                        }
                    } else if (auto tb = cur.try_as<wuxc::TextBlock>()) {
                        if (StringContainsTaskbarMenuHint(tb.Text().c_str())) {
                            popupHasTaskbarHint = true;
                        }
                    }
                }
                int cnt = wuxm::VisualTreeHelper::GetChildrenCount(cur);
                for (int ci = 0; ci < cnt; ++ci) {
                    if (auto ch = wuxm::VisualTreeHelper::GetChild(cur, ci)) {
                        q.push_back(ch);
                    }
                }
            }
            if (alreadyInjected) return true;
            if (!presenter) continue;

            auto items = presenter.Items();
            if (!popupHasTaskbarHint && items) {
                for (uint32_t ii = 0; ii < items.Size(); ++ii) {
                    auto obj = items.GetAt(ii);
                    if (auto mfi = obj.try_as<wuxc::MenuFlyoutItem>()) {
                        std::wstring nm = mfi.Name().c_str();
                        if (nm == L"WhModernTaskbarSettingsMenuItem") {
                            alreadyInjected = true;
                            break;
                        }
                        if (StringContainsTaskbarMenuHint(mfi.Text().c_str()) ||
                            StringContainsTaskbarMenuHint(nm)) {
                            popupHasTaskbarHint = true;
                        }
                    }
                }
            }
            if (alreadyInjected) return true;
            if (!popupHasTaskbarHint) continue;

            bool vi = g_useViAtomic.load(std::memory_order_relaxed);
            wuxc::MenuFlyoutSeparator sep;
            sep.Name(L"WhModernTaskbarSettingsMenuSep");

            wuxc::MenuFlyoutItem settingsItem;
            settingsItem.Name(L"WhModernTaskbarSettingsMenuItem");
            settingsItem.Text(vi ? L"Cài đặt Modern Taskbar" : L"Modern Taskbar Settings");
            try {
                wuxc::FontIcon icon;
                icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
                icon.Glyph(L"\uE790");
                icon.FontSize(16.0);
                settingsItem.Icon(icon);
            } catch (...) {}
            settingsItem.Click([](const winrt::Windows::Foundation::IInspectable&, const wux::RoutedEventArgs&) {
                OpenModernTaskbarSettingsApp();
            });

            bool appended = false;
            if (items && !presenter.ItemsSource()) {
                try {
                    items.Append(sep);
                    items.Append(settingsItem);
                    appended = true;
                } catch (...) {}
            }
            if (!appended) {
                try {
                    if (auto panel = presenter.ItemsPanelRoot().try_as<wuxc::Panel>()) {
                        panel.Children().Append(sep);
                        panel.Children().Append(settingsItem);
                        appended = true;
                    }
                } catch (...) {}
            }
            if (appended) {
                Wh_Log(L"Injected Modern Taskbar Settings item into Windows 11 Taskbar context menu");
                return true;
            }
        }
    } catch (...) {}
    return false;
}

void CALLBACK TaskbarContextMenuInjectTimerProc(HWND hWnd, UINT, UINT_PTR, DWORD) {
    if (!g_running.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) {
        KillTimer(hWnd, kTaskbarMenuInjectTimerId);
        g_menuInjectRemainingTicks = 0;
        return;
    }
    if (TryInjectTaskbarContextMenuSettingsItem(hWnd) || --g_menuInjectRemainingTicks <= 0) {
        KillTimer(hWnd, kTaskbarMenuInjectTimerId);
        g_menuInjectRemainingTicks = 0;
    }
}

void StartTaskbarContextMenuInjectBurst(HWND hTaskbar) {
    if (!hTaskbar || !IsWindow(hTaskbar) ||
        !g_running.load(std::memory_order_relaxed) ||
        g_unloading.load(std::memory_order_relaxed)) {
        return;
    }
    g_menuInjectRemainingTicks = 16;
    SetTimer(hTaskbar, kTaskbarMenuInjectTimerId, 30, TaskbarContextMenuInjectTimerProc);
}

void CALLBACK TaskbarOwnerTimerProc(HWND hWnd, UINT, UINT_PTR, DWORD) {
    if (!g_running.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) {
        KillTimer(hWnd, kTaskbarOwnerSyncTimerId);
        g_ownerTimerInstalled = false;
        if (g_fastAnimTimerActive) {
            KillTimer(hWnd, kTaskbarFastAnimTimerId);
            g_fastAnimTimerActive = false;
        }
        KillTimer(hWnd, kTaskbarMenuInjectTimerId);
        g_menuInjectRemainingTicks = 0;
        return;
    }
    bool needXamlSync = g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
                        g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
    SyncTaskbarXamlOnOwnerThread(hWnd, !needXamlSync);
    TryInjectTaskbarContextMenuSettingsItem(hWnd);
}


// Đo độ sáng thực tế (BT.709) CHỈ trên chính lớp nền kính của thanh Taskbar (snap.trayPixels / appsPixels),
// hoàn toàn không bị ảnh hưởng bởi cửa sổ ứng dụng trắng/đen nằm bên cạnh (như Notepad++, Chrome...)!
static bool ComputeTaskbarGlassBackdropIsLight(bool curIsLight) {
    std::vector<DWORD> pixels;
    int pw = 0, ph = 0;
    {
        std::lock_guard<std::mutex> lock(g_xamlGlassPixelsMutex);
        if (!g_sharedXamlGlassPixels.trayPixels.empty() && g_sharedXamlGlassPixels.trayW > 0 && g_sharedXamlGlassPixels.trayH > 0) {
            pixels = g_sharedXamlGlassPixels.trayPixels;
            pw = g_sharedXamlGlassPixels.trayW;
            ph = g_sharedXamlGlassPixels.trayH;
        } else if (!g_sharedXamlGlassPixels.appsPixels.empty() && g_sharedXamlGlassPixels.appsW > 0 && g_sharedXamlGlassPixels.appsH > 0) {
            pixels = g_sharedXamlGlassPixels.appsPixels;
            pw = g_sharedXamlGlassPixels.appsW;
            ph = g_sharedXamlGlassPixels.appsH;
        }
    }
    if (pixels.size() < static_cast<size_t>(pw * ph) || pw <= 0 || ph <= 0) {
        return curIsLight;
    }

    int stepX = std::max(1, pw / 16);
    int stepY = std::max(1, ph / 16);
    double sumLuma = 0.0;
    int count = 0;
    for (int y = 0; y < ph; y += stepY) {
        for (int x = 0; x < pw; x += stepX) {
            DWORD px = pixels[static_cast<size_t>(y * pw + x)];
            uint8_t b = static_cast<uint8_t>(px & 0xFF);
            uint8_t g = static_cast<uint8_t>((px >> 8) & 0xFF);
            uint8_t r = static_cast<uint8_t>((px >> 16) & 0xFF);
            uint8_t a = static_cast<uint8_t>((px >> 24) & 0xFF);
            if (a < 24) continue;
            // Un-premultiply để ra màu hiển thị chuẩn
            double invA = 255.0 / static_cast<double>(a);
            double rf = std::min(1.0, (static_cast<double>(r) * invA) / 255.0);
            double gf = std::min(1.0, (static_cast<double>(g) * invA) / 255.0);
            double bf = std::min(1.0, (static_cast<double>(b) * invA) / 255.0);
            sumLuma += 0.2126 * rf + 0.7152 * gf + 0.0722 * bf;
            ++count;
        }
    }
    if (count == 0) return curIsLight;

    double avgLuma = sumLuma / static_cast<double>(count);
    // Hysteresis ổn định (nền tối -> chữ trắng, nền sáng -> chữ đen)
    bool nextIsLight = curIsLight ? (avgLuma >= 0.46) : (avgLuma > 0.56);
    g_adaptiveBackdropIsLight.store(nextIsLight, std::memory_order_relaxed);
    return nextIsLight;
}

// Dọn sạch mọi vòng kính thừa từ phiên bản trước và CHỈ bo tròn gọn 1 lớp nền BackgroundFill vừa vặn bên trong thanh Taskbar
static void ApplyCircularTaskButtonStyle(wux::FrameworkElement const& btnFe, bool enableCircular) {
    if (!btnFe) return;
    auto walk = [&](auto&& self, wux::DependencyObject const& node, int depth) -> void {
        if (!node || depth > 6) return;
        try {
            if (auto fe = node.try_as<wux::FrameworkElement>()) {
                std::wstring name = fe.Name().c_str();
                if (name == L"RunningIndicator" || name.find(L"Badge") != std::wstring::npos) {
                    return;
                }

                // 1. Dọn sạch mọi LinearGradientBrush còn sót lại trên cây XAML từ bản trước (không cần restart Explorer!)
                if (auto brd = node.try_as<wuxc::Border>()) {
                    if (brd.BorderBrush() && brd.BorderBrush().try_as<wuxm::LinearGradientBrush>()) {
                        brd.ClearValue(wuxc::Border::BorderBrushProperty());
                        brd.ClearValue(wuxc::Border::BorderThicknessProperty());
                    }
                    if (brd.Background() && brd.Background().try_as<wuxm::LinearGradientBrush>()) {
                        brd.ClearValue(wuxc::Border::BackgroundProperty());
                    }
                } else if (auto rect = node.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>()) {
                    if (rect.Stroke() && rect.Stroke().try_as<wuxm::LinearGradientBrush>()) {
                        rect.ClearValue(winrt::Windows::UI::Xaml::Shapes::Shape::StrokeProperty());
                        rect.ClearValue(winrt::Windows::UI::Xaml::Shapes::Shape::StrokeThicknessProperty());
                    }
                    if (rect.Fill() && rect.Fill().try_as<wuxm::LinearGradientBrush>()) {
                        rect.ClearValue(winrt::Windows::UI::Xaml::Shapes::Shape::FillProperty());
                    }
                } else if (auto grid = node.try_as<wuxc::Grid>()) {
                    if (grid.BorderBrush() && grid.BorderBrush().try_as<wuxm::LinearGradientBrush>()) {
                        grid.ClearValue(wuxc::Grid::BorderBrushProperty());
                        grid.ClearValue(wuxc::Grid::BorderThicknessProperty());
                    }
                    if (grid.Background() && grid.Background().try_as<wuxm::LinearGradientBrush>()) {
                        grid.ClearValue(wuxc::Panel::BackgroundProperty());
                    }
                }

                // 2. Ẩn dải thẻ chồng (MultiWindowElement / BackPlate) ở mép phải khi bo tròn để không bị thừa vệt đỏ/xám lệch ngoài lề
                if (name == L"MultiWindowElement" || name.find(L"MultiWindow") != std::wstring::npos || name.find(L"BackPlate") != std::wstring::npos) {
                    if (enableCircular) {
                        if (fe.Opacity() > 0.01) fe.Opacity(0.0);
                    } else {
                        fe.ClearValue(wux::UIElement::OpacityProperty());
                    }
                    return;
                }

                // 3. Bo tròn mọi lớp nền kích thước nút (>= 22x22 DIP) theo đúng bán kính thật min(w,h)/2 để không bị lỗi mép khi nhấp nháy/thông báo
                double aw = fe.ActualWidth();
                double ah = fe.ActualHeight();
                bool isBgNamed = (name == L"BackgroundFill" || name == L"BackgroundStroke" || name == L"Background" ||
                                  name.find(L"Flash") != std::wstring::npos || name.find(L"Background") != std::wstring::npos);
                if ((aw >= 22.0 && ah >= 22.0) || isBgNamed) {
                    // Cân bằng lề đối xứng và thu gọn nhẹ vòng tròn (~3.5 DIP mỗi cạnh) để nằm gọn bên trong thanh Taskbar ngang lẫn dọc
                    if (enableCircular && (node.try_as<wuxc::Border>() || node.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>())) {
                        auto m = fe.Margin();
                        double hSym = std::max(3.5, std::min(m.Left, m.Right));
                        double vSym = std::max(3.5, std::min(m.Top, m.Bottom));
                        if (std::abs(m.Left - hSym) > 0.4 || std::abs(m.Right - hSym) > 0.4 ||
                            std::abs(m.Top - vSym) > 0.4 || std::abs(m.Bottom - vSym) > 0.4) {
                            fe.Margin({ hSym, vSym, hSym, vSym });
                        }
                    }
                    double targetRad = enableCircular ? ((aw >= 16.0 && ah >= 16.0) ? (std::min(aw, ah) * 0.5) : 20.0) : 4.0;
                    wux::CornerRadius targetCr{ targetRad, targetRad, targetRad, targetRad };
                    if (auto brd = node.try_as<wuxc::Border>()) {
                        if (std::abs(brd.CornerRadius().TopLeft - targetRad) > 0.5) brd.CornerRadius(targetCr);
                    } else if (auto rect = node.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>()) {
                        if (std::abs(rect.RadiusX() - targetRad) > 0.5 || std::abs(rect.RadiusY() - targetRad) > 0.5) {
                            rect.RadiusX(targetRad);
                            rect.RadiusY(targetRad);
                        }
                    } else if (auto grid = node.try_as<wuxc::Grid>()) {
                        if (grid.Background() != nullptr && std::abs(grid.CornerRadius().TopLeft - targetRad) > 0.5) {
                            grid.CornerRadius(targetCr);
                        }
                    }
                }
            }
        } catch (...) {}

        int count = 0;
        try { count = wuxm::VisualTreeHelper::GetChildrenCount(node); } catch (...) { return; }
        for (int i = 0; i < count; ++i) {
            wux::DependencyObject ch{ nullptr };
            try { ch = wuxm::VisualTreeHelper::GetChild(node, i); } catch (...) { continue; }
            if (ch) self(self, ch, depth + 1);
        }
    };
    walk(walk, btnFe, 0);
}

// Bo dạng viên thuốc (pill) hoặc hình tròn cho mọi nút trong Khay hệ thống (Quick Settings, Đồng hồ, Mũi tên ^, Chuông thông báo...)
// Đồng bộ màu tương phản cho toàn bộ icon/lõi pin trong Khay hệ thống Windows 11 25H2+
// Không phụ thuộc tên class "Battery": quét trực tiếp mọi Shape / IconElement (PathIcon, FontIcon) / Border nhỏ có màu đơn sắc đặc (A >= 180)
// và giữ nguyên 100% các màu trạng thái (xanh lá sạc, vàng tiết kiệm pin, đỏ pin yếu, huy hiệu thông báo)!
static void ApplyContrastToSystemTrayBattery(wux::FrameworkElement const& trayRoot, wux::ElementTheme wantTheme) {
    if (!trayRoot) return;
    static wuxm::SolidColorBrush s_whiteBrush{ nullptr };
    static wuxm::SolidColorBrush s_blackBrush{ nullptr };
    if (wantTheme != wux::ElementTheme::Default) {
        try {
            if (!s_whiteBrush) s_whiteBrush = wuxm::SolidColorBrush(winrt::Windows::UI::Color{ 255, 255, 255, 255 });
            if (!s_blackBrush) s_blackBrush = wuxm::SolidColorBrush(winrt::Windows::UI::Color{ 255, 24, 24, 24 });
        } catch (...) {}
    }

    auto shouldInvertMono = [&](winrt::Windows::UI::Color c, bool& toWhite) -> bool {
        // Chỉ chạm vào nét/lõi màu đặc (A >= 180), tuyệt đối bỏ qua nền nút hover bán trong suốt
        if (c.A < 180) return false;
        int maxC = std::max({ (int)c.R, (int)c.G, (int)c.B });
        int minC = std::min({ (int)c.R, (int)c.G, (int)c.B });
        // Chỉ đảo màu đơn sắc (tránh đụng màu xanh lá sạc, vàng tiết kiệm pin, đỏ pin yếu, badge thông báo)
        if ((maxC - minC) > 24) return false;
        if (wantTheme == wux::ElementTheme::Dark && maxC < 120) {
            toWhite = true;
            return true;
        }
        if (wantTheme == wux::ElementTheme::Light && minC > 170) {
            toWhite = false;
            return true;
        }
        return false;
    };

    auto walk = [&](auto&& self, wux::DependencyObject const& node, int depth) -> void {
        if (!node || depth > 16) return;
        try {
            if (auto fe = node.try_as<wux::FrameworkElement>()) {
                std::wstring name = fe.Name().c_str();
                if (name.find(L"Badge") != std::wstring::npos) {
                    return;
                }
                double aw = fe.ActualWidth();
                double ah = fe.ActualHeight();

                // Đặt RequestedTheme trên các cụm icon con để ép WinUI cập nhật tài nguyên màu
                if (depth > 0 && depth <= 8 && fe.RequestedTheme() != wantTheme) {
                    std::wstring cls = winrt::get_class_name(node).c_str();
                    if (cls.find(L"Icon") != std::wstring::npos ||
                        cls.find(L"Battery") != std::wstring::npos ||
                        cls.find(L"OmniButton") != std::wstring::npos) {
                        fe.RequestedTheme(wantTheme);
                    }
                }

                if (wantTheme != wux::ElementTheme::Default && s_whiteBrush && s_blackBrush) {
                    if (auto shp = node.try_as<winrt::Windows::UI::Xaml::Shapes::Shape>()) {
                        if (auto scb = shp.Fill().try_as<wuxm::SolidColorBrush>()) {
                            bool toWhite = false;
                            if (shouldInvertMono(scb.Color(), toWhite)) {
                                shp.Fill(toWhite ? s_whiteBrush : s_blackBrush);
                            }
                        }
                        if (auto scb = shp.Stroke().try_as<wuxm::SolidColorBrush>()) {
                            bool toWhite = false;
                            if (shouldInvertMono(scb.Color(), toWhite)) {
                                shp.Stroke(toWhite ? s_whiteBrush : s_blackBrush);
                            }
                        }
                    } else if (auto ic = node.try_as<wuxc::IconElement>()) {
                        if (auto scb = ic.Foreground().try_as<wuxm::SolidColorBrush>()) {
                            bool toWhite = false;
                            if (shouldInvertMono(scb.Color(), toWhite)) {
                                ic.Foreground(toWhite ? s_whiteBrush : s_blackBrush);
                            }
                        }
                    } else if (auto tb = node.try_as<wuxc::TextBlock>()) {
                        if (auto scb = tb.Foreground().try_as<wuxm::SolidColorBrush>()) {
                            bool toWhite = false;
                            if (shouldInvertMono(scb.Color(), toWhite)) {
                                tb.Foreground(toWhite ? s_whiteBrush : s_blackBrush);
                            }
                        }
                    } else if (aw > 0.5 && aw <= 34.0 && ah > 0.5 && ah <= 26.0) {
                        // Lõi pin dạng Border/Grid nhỏ bên trong biểu tượng pin (không bao giờ chạm vào nền nút lớn)
                        if (auto brd = node.try_as<wuxc::Border>()) {
                            if (auto scb = brd.Background().try_as<wuxm::SolidColorBrush>()) {
                                bool toWhite = false;
                                if (shouldInvertMono(scb.Color(), toWhite)) {
                                    brd.Background(toWhite ? s_whiteBrush : s_blackBrush);
                                }
                            }
                            if (auto scb = brd.BorderBrush().try_as<wuxm::SolidColorBrush>()) {
                                bool toWhite = false;
                                if (shouldInvertMono(scb.Color(), toWhite)) {
                                    brd.BorderBrush(toWhite ? s_whiteBrush : s_blackBrush);
                                }
                            }
                        } else if (auto grid = node.try_as<wuxc::Grid>()) {
                            if (auto scb = grid.Background().try_as<wuxm::SolidColorBrush>()) {
                                bool toWhite = false;
                                if (shouldInvertMono(scb.Color(), toWhite)) {
                                    grid.Background(toWhite ? s_whiteBrush : s_blackBrush);
                                }
                            }
                        }
                    }
                }
            }
        } catch (...) {}

        int count = 0;
        try { count = wuxm::VisualTreeHelper::GetChildrenCount(node); } catch (...) { return; }
        for (int i = 0; i < count; ++i) {
            wux::DependencyObject ch{ nullptr };
            try { ch = wuxm::VisualTreeHelper::GetChild(node, i); } catch (...) { continue; }
            if (ch) self(self, ch, depth + 1);
        }
    };
    walk(walk, trayRoot, 0);
}

// Bo dạng viên thuốc (pill) hoặc hình tròn cho mọi nút trong Khay hệ thống (Quick Settings, Đồng hồ, Mũi tên ^, Chuông thông báo...)

static void ApplyPillStyleToSystemTray(wux::FrameworkElement const& trayRoot, bool enablePill) {
    if (!trayRoot) return;
    auto walk = [&](auto&& self, wux::DependencyObject const& node, int depth) -> void {
        if (!node || depth > 10) return;
        try {
            if (auto fe = node.try_as<wux::FrameworkElement>()) {
                std::wstring name = fe.Name().c_str();
                if (name.find(L"Badge") != std::wstring::npos) {
                    return;
                }
                double aw = fe.ActualWidth();
                double ah = fe.ActualHeight();
                // Chỉ áp dụng cho lớp nền nút trong SystemTray (kích thước nút >= 18px, tránh chính SystemTrayFrame tổng)
                bool isTrayRoot = (depth == 0 || name == L"SystemTrayFrame");
                bool isBgNamed = (name == L"BackgroundFill" || name == L"BackgroundStroke" ||
                                  name == L"Background" || name == L"ContentGrid" || name == L"ContainerGrid");
                if (!isTrayRoot && ((aw >= 18.0 && ah >= 18.0) || isBgNamed)) {
                    double targetRad = enablePill ? ((aw >= 16.0 && ah >= 16.0) ? (std::min(aw, ah) * 0.5) : 18.0) : 4.0;
                    wux::CornerRadius targetCr{ targetRad, targetRad, targetRad, targetRad };
                    if (auto brd = node.try_as<wuxc::Border>()) {
                        if (std::abs(brd.CornerRadius().TopLeft - targetRad) > 0.5) brd.CornerRadius(targetCr);
                    } else if (auto rect = node.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>()) {
                        if (std::abs(rect.RadiusX() - targetRad) > 0.5 || std::abs(rect.RadiusY() - targetRad) > 0.5) {
                            rect.RadiusX(targetRad);
                            rect.RadiusY(targetRad);
                        }
                    } else if (auto grid = node.try_as<wuxc::Grid>()) {
                        if (grid.Background() != nullptr && std::abs(grid.CornerRadius().TopLeft - targetRad) > 0.5) {
                            grid.CornerRadius(targetCr);
                        }
                    }
                }
            }
        } catch (...) {}

        int count = 0;
        try { count = wuxm::VisualTreeHelper::GetChildrenCount(node); } catch (...) { return; }
        for (int i = 0; i < count; ++i) {
            wux::DependencyObject ch{ nullptr };
            try { ch = wuxm::VisualTreeHelper::GetChild(node, i); } catch (...) { continue; }
            if (ch) self(self, ch, depth + 1);
        }
    };
    walk(walk, trayRoot, 0);
}

void SyncTaskbarXamlOnOwnerThread(HWND hTaskbar, bool restoreDefault) {
    if (!hTaskbar || !IsWindow(hTaskbar)) return;
    if (g_inXamlLayoutSync) return;
    g_inXamlLayoutSync = true;
    struct SyncReentrancyGuard {
        ~SyncReentrancyGuard() { g_inXamlLayoutSync = false; }
    } syncGuard;

    bool splitEnabled = !restoreDefault && g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed);
    bool hideSysPrivacyIcon = !restoreDefault && g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
    bool wantThreadHooks = !restoreDefault &&
                           (splitEnabled || hideSysPrivacyIcon || g_enableTaskProgressBadgeAtomic.load(std::memory_order_relaxed));

    EnsureTaskbarOwnerThreadHooks(hTaskbar, wantThreadHooks);

    if (wantThreadHooks && !g_ownerTimerInstalled && g_running.load(std::memory_order_relaxed)) {
        g_ownerTimerInstalled = (SetTimer(hTaskbar, kTaskbarOwnerSyncTimerId, 240, TaskbarOwnerTimerProc) != 0);
    } else if (restoreDefault) {
        if (g_ownerTimerInstalled) {
            KillTimer(hTaskbar, kTaskbarOwnerSyncTimerId);
            g_ownerTimerInstalled = false;
        }
        if (g_fastAnimTimerActive) {
            KillTimer(hTaskbar, kTaskbarFastAnimTimerId);
            g_fastAnimTimerActive = false;
        }
    }

    if (restoreDefault) {
        RemoveInjectedTaskbarGlassOnOwnerThread();
    }

    try {
        wux::XamlRoot xamlRoot = ExtractTaskbarXamlRoot(hTaskbar);
        if (!xamlRoot) {
            if (!splitEnabled) {
                g_xamlBgCollapsed.store(false, std::memory_order_relaxed);
                ApplyTaskbarClearComposition(hTaskbar, false);
            }
            return;
        }

        auto rootFe = xamlRoot.Content().try_as<wux::FrameworkElement>();
        if (!rootFe) return;

        RECT rcTb{};
        GetWindowRect(hTaskbar, &rcTb);
        int tbW = rcTb.right - rcTb.left;
        int tbH = rcTb.bottom - rcTb.top;
        if (tbW <= 20 || tbH <= 20) return;
        bool isVert = (tbH > tbW);

        UINT dpi = GetDpiForWindow(hTaskbar);
        if (dpi == 0) dpi = 96;
        float dpiScale = static_cast<float>(dpi) / 96.0f;

        int bridgeOffX = 0, bridgeOffY = 0;
        HWND hBridge = FindWindowExW(hTaskbar, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        RECT rcBridge{};
        if (hBridge && GetWindowRect(hBridge, &rcBridge)) {
            bridgeOffX = rcBridge.left - rcTb.left;
            bridgeOffY = rcBridge.top - rcTb.top;
        }

        // Gắn LayoutUpdated trực tiếp trên rootFe để nền Thanh ứng dụng co dãn theo thời gian thực (~60fps)
        if (splitEnabled && g_xamlLayoutWatchTarget != rootFe) {
            g_xamlLayoutRevoker.revoke();
            g_xamlLayoutWatchTarget = rootFe;
            HWND hTbCapture = hTaskbar;
            g_xamlLayoutRevoker = rootFe.LayoutUpdated(winrt::auto_revoke,
                [hTbCapture](wf::IInspectable const&, wf::IInspectable const&) {
                    if (!g_running.load(std::memory_order_relaxed) ||
                        g_unloading.load(std::memory_order_relaxed) ||
                        g_inXamlLayoutSync ||
                        !g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed)) {
                        return;
                    }
                    ULONGLONG now = GetTickCount64();
                    if (now - g_lastLayoutSyncTick < 16) return;
                    g_lastLayoutSyncTick = now;
                    SyncTaskbarXamlOnOwnerThread(hTbCapture, false);
                });
        }

        bool foundBg = false;
        wuxc::Panel bgParentPanel{ nullptr };
        std::vector<wf::Rect> iconDipRects;
        std::vector<wux::UIElement> appHideTargets;
        wux::FrameworkElement trayFrameFe{ nullptr };
        wux::FrameworkElement widgetsButtonFe{ nullptr };
        bool mainStackChanged = false;
        bool hideAppsNow = splitEnabled && g_hideAppsPillAtomic.load(std::memory_order_relaxed);

        auto walk = [&](auto&& self, wux::DependencyObject const& node, int depth, bool insideTray) -> void {
            if (!node || depth > 18) return;
            std::wstring clsName;
            std::wstring elName;
            wux::FrameworkElement fe{ nullptr };
            try {
                clsName = winrt::get_class_name(node).c_str();
                fe = node.try_as<wux::FrameworkElement>();
                if (fe) {
                    elName = fe.Name().c_str();
                    if (elName == L"WhTaskbarLiquidGlassCanvas") return;

                    bool isSpecialHiddenTarget =
                        (clsName == L"Taskbar.TaskbarBackground" ||
                         elName == L"BackgroundControl" ||
                         elName == L"BackgroundStroke" ||
                         (clsName == L"SystemTray.Stack" && elName == L"MainStack"));
                    if (!restoreDefault && !isSpecialHiddenTarget && fe.Visibility() != wux::Visibility::Visible) {
                        return;
                    }
                }
            } catch (...) { return; }

            if (fe) {
                // 1. Ẩn nền mờ đục gốc của Windows 11 Taskbar và lấy Panel cha bao trọn toàn Taskbar
                if (clsName == L"Taskbar.TaskbarBackground" || elName == L"BackgroundControl" || elName == L"BackgroundStroke") {
                    if (!bgParentPanel && (clsName == L"Taskbar.TaskbarBackground" || elName == L"BackgroundControl")) {
                        if (auto par = wuxm::VisualTreeHelper::GetParent(fe)) {
                            bgParentPanel = par.try_as<wuxc::Panel>();
                        }
                    }
                    fe.Visibility(splitEnabled ? wux::Visibility::Collapsed : wux::Visibility::Visible);
                    if (splitEnabled) foundBg = true;
                    return;
                }

                // 2. Ẩn biểu tượng Micro / Camera / Vị trí mặc định bên Khay hệ thống (SystemTray.Stack#MainStack)
                if (clsName == L"SystemTray.Stack" && elName == L"MainStack") {
                    auto wantVis = hideSysPrivacyIcon ? wux::Visibility::Collapsed : wux::Visibility::Visible;
                    if (fe.Visibility() != wantVis) {
                        fe.Visibility(wantVis);
                        mainStackChanged = true;
                    }
                    return;
                }

                if (clsName == L"SystemTray.SystemTrayFrame" || elName == L"SystemTrayFrame") {
                    trayFrameFe = fe;
                    insideTray = true;
                }

                // 2b. Tách riêng nút Widget mặc định của Windows để dịch sang bên cạnh, không đè lên Live Activity!
                if (!insideTray &&
                    (clsName == L"Taskbar.AugmentedEntryPointButton" || elName == L"WidgetsButton")) {
                    widgetsButtonFe = fe;
                    return;
                }

                // 3. Đo chính xác toàn bộ nút ĐANG HIỂN THỊ trên Thanh ứng dụng (loại sạch nút ảo/recycled của ItemsRepeater!)
                if (!insideTray &&
                    (clsName == L"Taskbar.TaskListButton" ||
                     clsName == L"Taskbar.TaskListLabeledButton" ||
                     clsName == L"Taskbar.ExperienceToggleButton" ||
                     clsName == L"Taskbar.OverflowButton" ||
                     clsName == L"Taskbar.SearchBoxButton" ||
                     clsName == L"SearchUx.SearchUI.SearchButtonControl")) {
                    bool isToggleBtn = (clsName == L"Taskbar.ExperienceToggleButton");
                    bool isAppVisible = hideAppsNow || (fe.Opacity() > 0.15 && (isToggleBtn || fe.IsHitTestVisible()));
                    if (isAppVisible && fe.ActualWidth() >= 14.0 && fe.ActualHeight() >= 14.0) {
                        // Chỉ kiểm tra nút rác recycled đối với TaskListButton (tuyệt đối không loại nhầm nút Start / ExperienceToggleButton!)
                        bool innerVisible = true;
                        if (clsName == L"Taskbar.TaskListButton" || clsName == L"Taskbar.TaskListLabeledButton") {
                            int nSub = 0;
                            try { nSub = wuxm::VisualTreeHelper::GetChildrenCount(fe); } catch (...) { nSub = 0; }
                            if (nSub == 0) {
                                innerVisible = false;
                            } else if (nSub == 1) {
                                try {
                                    if (auto subFe = wuxm::VisualTreeHelper::GetChild(fe, 0).try_as<wux::FrameworkElement>()) {
                                        if (subFe.Visibility() != wux::Visibility::Visible || (!hideAppsNow && subFe.Opacity() <= 0.10)) {
                                            innerVisible = false;
                                        }
                                    }
                                } catch (...) {}
                            }
                        }
                        wf::Rect rDip{};
                        if (innerVisible && TransformElementBoundsToRootDip(fe, rDip)) {
                            float maxW = static_cast<float>(tbW - bridgeOffX) / dpiScale;
                            float maxH = static_cast<float>(tbH - bridgeOffY) / dpiScale;
                            bool inTaskbar = (rDip.X >= -2.0f && rDip.Y >= -2.0f &&
                                              rDip.X + rDip.Width <= maxW + 2.0f &&
                                              rDip.Y + rDip.Height <= maxH + 2.0f);
                            if (inTaskbar) {
                                iconDipRects.push_back(rDip);
                                appHideTargets.push_back(fe);
                                if (clsName == L"Taskbar.TaskListButton" ||
                                    clsName == L"Taskbar.TaskListLabeledButton" ||
                                    clsName == L"Taskbar.ExperienceToggleButton" ||
                                    clsName == L"Taskbar.OverflowButton") {
                                    ApplyCircularTaskButtonStyle(fe, splitEnabled && !restoreDefault);
                                }
                            }
                        }
                    }
                    return;
                }

                if (!insideTray && elName == L"TaskbarFrameRepeater") {
                    appHideTargets.push_back(fe);
                }
            }

            int childCount = 0;
            try { childCount = wuxm::VisualTreeHelper::GetChildrenCount(node); } catch (...) { return; }
            for (int i = 0; i < childCount; ++i) {
                wux::DependencyObject ch{ nullptr };
                try { ch = wuxm::VisualTreeHelper::GetChild(node, i); } catch (...) { continue; }
                if (ch) self(self, ch, depth + 1, insideTray);
            }
        };

        walk(walk, rootFe, 0, false);

        if (mainStackChanged && trayFrameFe) {
            try { trayFrameFe.UpdateLayout(); } catch (...) {}
        }

        if (!splitEnabled) {
            RemoveInjectedTaskbarGlassOnOwnerThread();
            bool wasCollapsed = g_xamlBgCollapsed.exchange(false, std::memory_order_relaxed);
            if (wasCollapsed) {
                ApplyTaskbarClearComposition(hTaskbar, false);
            }
            return;
        }

        if (foundBg) {
            bool prev = g_xamlBgCollapsed.exchange(true, std::memory_order_relaxed);
            if (!prev) {
                ApplyTaskbarClearComposition(hTaskbar, true);
            }
        }

        // Áp dụng ẩn/hiện riêng cụm icon Thanh ứng dụng khi đang hiện Popup 3s (KHÔNG chạm Khay trạng thái!)
        if (hideAppsNow) {
            g_hiddenAppUiElements.clear();
            for (const auto& el : appHideTargets) {
                try {
                    el.Opacity(0.0);
                    el.IsHitTestVisible(false);
                    g_hiddenAppUiElements.push_back(winrt::make_weak(el));
                } catch (...) {}
            }
        } else if (!g_hiddenAppUiElements.empty()) {
            for (const auto& wk : g_hiddenAppUiElements) {
                if (auto el = wk.get()) {
                    try {
                        el.Opacity(1.0);
                        el.IsHitTestVisible(true);
                    } catch (...) {}
                }
            }
            g_hiddenAppUiElements.clear();
        }

        wf::Rect trayDipRect{};
        bool hasTrayDip = (trayFrameFe && TransformElementBoundsToRootDip(trayFrameFe, trayDipRect));

        float tbDipW = static_cast<float>(tbW - bridgeOffX) / dpiScale;
        float tbDipH = static_cast<float>(tbH - bridgeOffY) / dpiScale;
        float padOuterDip = static_cast<float>(g_pillVerticalMarginDipAtomic.load(std::memory_order_relaxed));
        float padIconDip  = static_cast<float>(g_iconPaddingDipAtomic.load(std::memory_order_relaxed));
        float gapDip      = 8.0f;
        float radiusDip   = static_cast<float>(g_pillCornerRadiusDipAtomic.load(std::memory_order_relaxed));

        wf::Rect unionAppsDip{};
        bool hasAppsDip = false;
        if (!iconDipRects.empty()) {
            std::sort(iconDipRects.begin(), iconDipRects.end(), [isVert](const wf::Rect& a, const wf::Rect& b) {
                return isVert ? (a.Y < b.Y) : (a.X < b.X);
            });

            size_t bestStart = 0, bestEnd = 0, curStart = 0;
            for (size_t i = 1; i < iconDipRects.size(); ++i) {
                float gap = isVert
                    ? (iconDipRects[i].Y - (iconDipRects[i - 1].Y + iconDipRects[i - 1].Height))
                    : (iconDipRects[i].X - (iconDipRects[i - 1].X + iconDipRects[i - 1].Width));
                if (gap > 140.0f) {
                    if ((i - 1 - curStart) > (bestEnd - bestStart)) {
                        bestStart = curStart;
                        bestEnd = i - 1;
                    }
                    curStart = i;
                }
            }
            if ((iconDipRects.size() - 1 - curStart) >= (bestEnd - bestStart)) {
                bestStart = curStart;
                bestEnd = iconDipRects.size() - 1;
            }

            float minX = iconDipRects[bestStart].X;
            float minY = iconDipRects[bestStart].Y;
            float maxX = iconDipRects[bestStart].X + iconDipRects[bestStart].Width;
            float maxY = iconDipRects[bestStart].Y + iconDipRects[bestStart].Height;
            for (size_t i = bestStart + 1; i <= bestEnd; ++i) {
                minX = std::min(minX, iconDipRects[i].X);
                minY = std::min(minY, iconDipRects[i].Y);
                maxX = std::max(maxX, iconDipRects[i].X + iconDipRects[i].Width);
                maxY = std::max(maxY, iconDipRects[i].Y + iconDipRects[i].Height);
            }
            unionAppsDip = { minX, minY, maxX - minX, maxY - minY };
            hasAppsDip = true;
        }

        // Tính đúng 2 viên thuốc XAML đồng đều nhau 100% (cùng Top/Bottom cho ngang, cùng Left/Right cho dọc!)
        wf::Rect pillAppsDip{};
        wf::Rect pillTrayDip{};
        bool showTrayPill = false;
        bool showAppsPill = false;

        if (!isVert) {
            float pTop = padOuterDip;
            float pH   = std::max(16.0f, tbDipH - padOuterDip * 2.0f);
            if (hasTrayDip) {
                float tL = std::clamp(trayDipRect.X - 6.0f, 120.0f, std::max(122.0f, tbDipW - 48.0f));
                float tR = std::max(tL + 36.0f, tbDipW - 2.0f);
                pillTrayDip = { tL, pTop, tR - tL, pH };
                showTrayPill = true;
            }
            float maxAppsR = showTrayPill ? (pillTrayDip.X - gapDip) : (tbDipW - 8.0f);
            if (hasAppsDip) {
                float aL = std::max(6.0f, unionAppsDip.X - padIconDip);
                float aR = std::min(maxAppsR, unionAppsDip.X + unionAppsDip.Width + padIconDip);
                if (aR > aL + 32.0f) {
                    pillAppsDip = { aL, pTop, aR - aL, pH };
                    showAppsPill = true;
                }
            }
        } else {
            float pLeft = padOuterDip;
            float pW    = std::max(16.0f, tbDipW - padOuterDip * 2.0f);
            if (hasTrayDip) {
                float tT = std::clamp(trayDipRect.Y - 6.0f, 120.0f, std::max(122.0f, tbDipH - 60.0f));
                float tB = std::max(tT + 36.0f, tbDipH - 2.0f);
                pillTrayDip = { pLeft, tT, pW, tB - tT };
                showTrayPill = true;
            }
            float maxAppsB = showTrayPill ? (pillTrayDip.Y - gapDip) : (tbDipH - 8.0f);
            if (hasAppsDip) {
                float aT = std::max(6.0f, unionAppsDip.Y - padIconDip);
                float aB = std::min(maxAppsB, unionAppsDip.Y + unionAppsDip.Height + padIconDip);
                if (aB > aT + 32.0f) {
                    pillAppsDip = { pLeft, aT, pW, aB - aT };
                    showAppsPill = true;
                }
            }
        }

        // Gắn Canvas nền kính Liquid Glass trực tiếp vào vị trí 0 của Panel gốc Taskbar (nằm dưới 100% biểu tượng!)
        if (!bgParentPanel) {
            bgParentPanel = rootFe.try_as<wuxc::Panel>();
        }
        if (bgParentPanel) {
            bool needInsert = (!g_xamlGlassCanvas || g_xamlHostPanel != bgParentPanel);
            if (!needInsert) {
                uint32_t existingIdx = 0;
                if (!bgParentPanel.Children().IndexOf(g_xamlGlassCanvas, existingIdx)) {
                    needInsert = true;
                }
            }

            if (needInsert) {
                // Dọn sạch nếu từng có bản cũ còn sót trên cây XAML
                int count = static_cast<int>(bgParentPanel.Children().Size());
                for (int i = count - 1; i >= 0; --i) {
                    auto ch = bgParentPanel.Children().GetAt(i).try_as<wux::FrameworkElement>();
                    if (ch && ch.Name() == L"WhTaskbarLiquidGlassCanvas") {
                        bgParentPanel.Children().RemoveAt(i);
                    }
                }

                wuxc::Canvas canvas;
                canvas.Name(L"WhTaskbarLiquidGlassCanvas");
                canvas.IsHitTestVisible(false);
                canvas.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
                canvas.VerticalAlignment(wux::VerticalAlignment::Stretch);
                wuxc::Grid::SetColumn(canvas, 0);
                wuxc::Grid::SetColumnSpan(canvas, 32);
                wuxc::Grid::SetRow(canvas, 0);
                wuxc::Grid::SetRowSpan(canvas, 32);
                wuxc::Canvas::SetZIndex(canvas, -1000);

                bool initLight = g_cachedSystemLightTheme;
                winrt::Windows::UI::Color fallbackFill = initLight
                    ? winrt::Windows::UI::Color{ 205, 244, 246, 252 }
                    : winrt::Windows::UI::Color{ 205, 22, 24, 34 };

                wuxc::Border appsBorder;
                appsBorder.IsHitTestVisible(false);
                appsBorder.BorderThickness({ 1.0, 1.0, 1.0, 1.0 });
                appsBorder.BorderBrush(BuildXamlGlassRimBrush(initLight));
                appsBorder.Background(wuxm::SolidColorBrush(fallbackFill));

                wuxc::Border trayBorder;
                trayBorder.IsHitTestVisible(false);
                trayBorder.BorderThickness({ 1.0, 1.0, 1.0, 1.0 });
                trayBorder.BorderBrush(BuildXamlGlassRimBrush(initLight));
                trayBorder.Background(wuxm::SolidColorBrush(fallbackFill));

                canvas.Children().Append(appsBorder);
                canvas.Children().Append(trayBorder);
                bgParentPanel.Children().InsertAt(0, canvas);

                g_xamlHostPanel      = bgParentPanel;
                g_xamlGlassCanvas    = canvas;
                g_xamlAppsPillBorder = appsBorder;
                g_xamlTrayPillBorder = trayBorder;
                g_appliedXamlGlassRevision = 0;
                g_appliedRimStyle = -1;
            }

            // Chuyển đổi qua ma trận nghịch đảo của Panel (tương thích 100% cả khi Taskbar Dọc xoay ma trận XAML!)
            auto toRoot = bgParentPanel.TransformToVisual(nullptr);
            auto fromRoot = toRoot.Inverse();

            auto layoutPillBorder = [&](wuxc::Border const& border, double& appliedRad, const wf::Rect& rRootDip, bool visible, double opacity) {
                if (!border) return;
                if (!visible || rRootDip.Width <= 4.0f || rRootDip.Height <= 4.0f) {
                    if (border.Visibility() != wux::Visibility::Collapsed) {
                        border.Visibility(wux::Visibility::Collapsed);
                    }
                    return;
                }
                wf::Rect rLocal = fromRoot ? fromRoot.TransformBounds(rRootDip) : rRootDip;
                double rad = std::min({ static_cast<double>(radiusDip), rLocal.Width * 0.5, rLocal.Height * 0.5 });
                double curL = wuxc::Canvas::GetLeft(border);
                double curT = wuxc::Canvas::GetTop(border);
                double curW = border.Width();
                double curH = border.Height();
                if (std::isnan(curL) || std::abs(curL - rLocal.X) >= 0.5) wuxc::Canvas::SetLeft(border, rLocal.X);
                if (std::isnan(curT) || std::abs(curT - rLocal.Y) >= 0.5) wuxc::Canvas::SetTop(border, rLocal.Y);
                if (std::isnan(curW) || std::abs(curW - rLocal.Width) >= 0.5) border.Width(rLocal.Width);
                if (std::isnan(curH) || std::abs(curH - rLocal.Height) >= 0.5) border.Height(rLocal.Height);
                if (std::abs(appliedRad - rad) >= 0.4) {
                    border.CornerRadius({ rad, rad, rad, rad });
                    appliedRad = rad;
                }
                if (std::abs(border.Opacity() - opacity) >= 0.01) border.Opacity(opacity);
                if (border.Visibility() != wux::Visibility::Visible) border.Visibility(wux::Visibility::Visible);
            };

            layoutPillBorder(g_xamlAppsPillBorder, g_appliedAppsRad, pillAppsDip, showAppsPill, hideAppsNow ? 0.0 : 1.0);
            layoutPillBorder(g_xamlTrayPillBorder, g_appliedTrayRad, pillTrayDip, showTrayPill, 1.0);

            g_xamlGlassInjected.store(true, std::memory_order_relaxed);
            ApplySharedGlassPixelsOnOwnerThread();
        }

        // Tự động đổi màu chữ & biểu tượng Taskbar theo độ tương phản nền thực tế (nền tối -> chữ trắng, nền sáng -> chữ đen)
        {
            wux::ElementTheme wantTheme = wux::ElementTheme::Default;
            if (splitEnabled && !restoreDefault) {
                bool curIsLight = g_adaptiveBackdropIsLight.load(std::memory_order_relaxed);
                bool backdropIsLight = ComputeTaskbarGlassBackdropIsLight(curIsLight);
                wantTheme = backdropIsLight ? wux::ElementTheme::Light : wux::ElementTheme::Dark;
            }
            if (trayFrameFe) {
                g_xamlTrayFrameFe = trayFrameFe;
                ApplyPillStyleToSystemTray(trayFrameFe, splitEnabled && !restoreDefault);
                try {
                    if (trayFrameFe.RequestedTheme() != wantTheme) {
                        trayFrameFe.RequestedTheme(wantTheme);
                    }
                    ApplyContrastToSystemTrayBattery(trayFrameFe, wantTheme);
                } catch (...) {}
            }
            if (bgParentPanel) {
                g_xamlBgParentPanel = bgParentPanel;
                try {
                    if (bgParentPanel.RequestedTheme() != wantTheme) {
                        bgParentPanel.RequestedTheme(wantTheme);
                    }
                } catch (...) {}
            }
        }

        // Xử lý nút Widget mặc định của Windows: ẩn sạch nếu không đủ chỗ (gần như bị lấp đầy), chỉ dịch 1 lần ổn định nếu đủ khoảng trống!
        if (widgetsButtonFe) {
            if (g_xamlWidgetsButton && g_xamlWidgetsButton != widgetsButtonFe) {
                ResetWidgetsButtonShiftOnOwnerThread();
            }
            g_xamlWidgetsButton = widgetsButtonFe;

            bool cornerVis = g_cornerPillVisibleAtomic.load(std::memory_order_relaxed);
            int cornerEndDip = g_cornerPillEndDipAtomic.load(std::memory_order_relaxed);

            bool shouldHideWidget = false;
            double targetShiftX = 0.0;
            double targetShiftY = 0.0;

            wf::Rect wCurDip{};
            if (TransformElementBoundsToRootDip(widgetsButtonFe, wCurDip)) {
                // Ghi nhớ tọa độ gốc ban đầu 1 lần khi chưa dịch để tuyệt đối không bị trôi/rung liên tục!
                if (g_cachedWidgetsNatX < 0.0 || (g_appliedWidgetsShiftX == 0.0 && g_appliedWidgetsShiftY == 0.0)) {
                    g_cachedWidgetsNatX = static_cast<double>(wCurDip.X);
                    g_cachedWidgetsNatY = static_cast<double>(wCurDip.Y);
                }

                if (cornerVis && cornerEndDip > 0) {
                    double desiredStart = static_cast<double>(cornerEndDip) + gapDip;
                    if (!isVert) {
                        double widgetW = std::max(28.0, static_cast<double>(wCurDip.Width));
                        double appsStart = showAppsPill ? static_cast<double>(pillAppsDip.X) : static_cast<double>(tbDipW);
                        double freeSpace = (appsStart - gapDip) - desiredStart;
                        if (g_cachedWidgetsNatX < desiredStart) {
                            if (freeSpace < widgetW + 6.0) {
                                // Gần như bị lấp đầy / không đủ chỗ đặt Widget bên cạnh -> ẩn sạch Widget mặc định
                                shouldHideWidget = true;
                            } else {
                                targetShiftX = std::max(0.0, desiredStart - g_cachedWidgetsNatX);
                            }
                        }
                    } else {
                        double widgetH = std::max(28.0, static_cast<double>(wCurDip.Height));
                        double appsStart = showAppsPill ? static_cast<double>(pillAppsDip.Y) : static_cast<double>(tbDipH);
                        double freeSpace = (appsStart - gapDip) - desiredStart;
                        if (g_cachedWidgetsNatY < desiredStart) {
                            if (freeSpace < widgetH + 6.0) {
                                // Thanh dọc không đủ chỗ giữa Live Activity và Thanh ứng dụng -> ẩn sạch Widget mặc định
                                shouldHideWidget = true;
                            } else {
                                targetShiftY = std::max(0.0, desiredStart - g_cachedWidgetsNatY);
                            }
                        }
                    }
                }
            }

            try {
                if (shouldHideWidget) {
                    if (!g_appliedWidgetsHidden) {
                        widgetsButtonFe.Opacity(0.0);
                        widgetsButtonFe.IsHitTestVisible(false);
                        g_appliedWidgetsHidden = true;
                    }
                    if (g_appliedWidgetsShiftX != 0.0 || g_appliedWidgetsShiftY != 0.0) {
                        wuxm::TranslateTransform tt;
                        tt.X(0.0);
                        tt.Y(0.0);
                        widgetsButtonFe.RenderTransform(tt);
                        g_appliedWidgetsShiftX = 0.0;
                        g_appliedWidgetsShiftY = 0.0;
                    }
                } else {
                    if (g_appliedWidgetsHidden) {
                        widgetsButtonFe.Opacity(1.0);
                        widgetsButtonFe.IsHitTestVisible(true);
                        g_appliedWidgetsHidden = false;
                    }
                    // Deadzone 3 DIP tránh rung/dịch chuyển liên tục khi kích thước dao động nhỏ
                    if (std::abs(targetShiftX - g_appliedWidgetsShiftX) >= 3.0 ||
                        std::abs(targetShiftY - g_appliedWidgetsShiftY) >= 3.0 ||
                        (targetShiftX == 0.0 && g_appliedWidgetsShiftX != 0.0) ||
                        (targetShiftY == 0.0 && g_appliedWidgetsShiftY != 0.0)) {
                        wuxm::TranslateTransform tt;
                        tt.X(targetShiftX);
                        tt.Y(targetShiftY);
                        widgetsButtonFe.RenderTransform(tt);
                        g_appliedWidgetsShiftX = targetShiftX;
                        g_appliedWidgetsShiftY = targetShiftY;
                    }
                }
            } catch (...) {}
        } else if (g_xamlWidgetsButton) {
            ResetWidgetsButtonShiftOnOwnerThread();
        }

        // Lưu tọa độ pixel chính xác để đồng bộ tạo bitmap khúc xạ & định vị Popup/Viên thuốc góc
        RECT appsPx = showAppsPill ? DipRectToTaskbarLocalPx(pillAppsDip, dpi, bridgeOffX, bridgeOffY) : RECT{};
        RECT trayPx = showTrayPill ? DipRectToTaskbarLocalPx(pillTrayDip, dpi, bridgeOffX, bridgeOffY) : RECT{};

        bool changed = false;
        {
            std::lock_guard<std::mutex> lock(g_xamlBoundsMutex);
            auto diffRect = [](const RECT& a, const RECT& b) {
                return std::abs(a.left - b.left) >= 2 || std::abs(a.top - b.top) >= 2 ||
                       std::abs(a.right - b.right) >= 2 || std::abs(a.bottom - b.bottom) >= 2;
            };
            if (g_measuredBounds.hasApps != showAppsPill ||
                g_measuredBounds.hasTray != showTrayPill ||
                (showAppsPill && diffRect(g_measuredBounds.rcAppsLocal, appsPx)) ||
                (showTrayPill && diffRect(g_measuredBounds.rcTrayLocal, trayPx))) {
                g_measuredBounds.hasApps = showAppsPill;
                g_measuredBounds.hasTray = showTrayPill;
                g_measuredBounds.rcAppsLocal = appsPx;
                g_measuredBounds.rcTrayLocal = trayPx;
                changed = true;
            }
        }

        if (changed && g_alertHwnd && IsWindow(g_alertHwnd)) {
            PostMessageW(g_alertHwnd, WM_APP_XAML_BOUNDS_CHANGED, 0, 0);
        }
    } catch (...) {}
}

void WINAPI HookedTrayUI_StartTaskbar(void* pThis) {
    if (g_TrayUI_StartTaskbar_Original) {
        g_TrayUI_StartTaskbar_Original(pThis);
    }
    if (g_running.load(std::memory_order_relaxed) && !g_unloading.load(std::memory_order_relaxed)) {
        if (HWND hTb = FindCurrentProcessTaskbarWnd()) {
            bool needSync = g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
                            g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
            SyncTaskbarXamlOnOwnerThread(hTb, !needSync);
        }
    }
}

void TryHookTaskbarDllSymbols(HMODULE hMod, bool applyHooks) {
    if (!hMod || g_unloading.load(std::memory_order_relaxed) || g_taskbarSymbolsAttempted.exchange(true)) {
        return;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            { LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})" },
            &g_CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const)",
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void) const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void) const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
            },
            &g_CTaskBand_GetTaskbarHost_Original,
        },
        {
            {
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const)",
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void) const)",
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )",
            },
            &g_TaskbarHost_FrameHeight_Original,
            nullptr,
            true,
        },
        {
            { LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))" },
            &g_Std_Ref_Decref_Original,
        },
        {
            { LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))" },
            &g_TrayUI_StartTaskbar_Original,
            HookedTrayUI_StartTaskbar,
            true,
        },
    };

    if (WindhawkUtils::HookSymbols(hMod, hooks, ARRAYSIZE(hooks))) {
        g_taskbarSymbolsReady.store(true, std::memory_order_relaxed);
        if (applyHooks) {
            Wh_ApplyHookOperations();
        }
    } else {
        Wh_Log(L"Taskbar.dll symbols unavailable; waiting for standard taskbar state.");
    }
}

HMODULE WINAPI HookedLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hLoaded = pOrigLoadLibraryExW ? pOrigLoadLibraryExW(lpLibFileName, hFile, dwFlags) : nullptr;
    if (hLoaded && lpLibFileName && !g_taskbarSymbolsAttempted.load(std::memory_order_relaxed)) {
        const wchar_t* base = PathFindFileNameW(lpLibFileName);
        if (base && _wcsicmp(base, L"Taskbar.dll") == 0) {
            TryHookTaskbarDllSymbols(hLoaded, true);
        }
    }
    return hLoaded;
}

// ---------------------------------------------------------------------------
// 5. Giải mã Hình Ảnh WIC (Dùng chung cho Ảnh Bìa Bài Hát & Ảnh Nền Desktop Không Tự Nhiễm)
// ---------------------------------------------------------------------------
struct DecodedBgraImage {
    UINT width = 0;
    UINT height = 0;
    std::vector<DWORD> pixels; // 32bppPBGRA
};

std::shared_ptr<DecodedBgraImage> DecodeMemoryImageWic(const BYTE* data, DWORD sizeBytes, UINT targetMaxEdge) {
    if (!data || sizeBytes == 0 || sizeBytes > 8 * 1024 * 1024) return nullptr;
    try {
        winrt::com_ptr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.put())))) {
            return nullptr;
        }
        winrt::com_ptr<IWICStream> stream;
        if (FAILED(factory->CreateStream(stream.put())) ||
            FAILED(stream->InitializeFromMemory(const_cast<BYTE*>(data), sizeBytes))) {
            return nullptr;
        }
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        if (FAILED(factory->CreateDecoderFromStream(stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.put()))) {
            return nullptr;
        }
        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        if (FAILED(decoder->GetFrame(0, frame.put()))) return nullptr;

        UINT srcW = 0, srcH = 0;
        if (FAILED(frame->GetSize(&srcW, &srcH)) || srcW == 0 || srcH == 0 || srcW > 16384 || srcH > 16384) {
            return nullptr;
        }
        double scale = std::min(1.0, static_cast<double>(targetMaxEdge) / std::max(srcW, srcH));
        UINT dstW = std::max(1u, static_cast<UINT>(std::lround(srcW * scale)));
        UINT dstH = std::max(1u, static_cast<UINT>(std::lround(srcH * scale)));

        winrt::com_ptr<IWICBitmapScaler> scaler;
        if (FAILED(factory->CreateBitmapScaler(scaler.put())) ||
            FAILED(scaler->Initialize(frame.get(), dstW, dstH, WICBitmapInterpolationModeFant))) {
            return nullptr;
        }
        winrt::com_ptr<IWICFormatConverter> conv;
        if (FAILED(factory->CreateFormatConverter(conv.put())) ||
            FAILED(conv->Initialize(scaler.get(), GUID_WICPixelFormat32bppPBGRA,
                                    WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
            return nullptr;
        }

        auto out = std::make_shared<DecodedBgraImage>();
        out->width = dstW;
        out->height = dstH;
        out->pixels.resize(static_cast<size_t>(dstW) * dstH);
        if (FAILED(conv->CopyPixels(nullptr, dstW * 4, static_cast<UINT>(out->pixels.size() * 4),
                                    reinterpret_cast<BYTE*>(out->pixels.data())))) {
            return nullptr;
        }
        return out;
    } catch (...) {
        return nullptr;
    }
}

bool DecodeWallpaperToMonitorBuffer(const wchar_t* filePath, int monW, int monH, std::vector<DWORD>& outMonitorPixels) {
    if (!filePath || !filePath[0] || monW <= 16 || monH <= 16) return false;
    try {
        winrt::com_ptr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.put())))) {
            return false;
        }
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        if (FAILED(factory->CreateDecoderFromFilename(filePath, nullptr, GENERIC_READ,
                                                      WICDecodeMetadataCacheOnDemand, decoder.put()))) {
            return false;
        }
        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        if (FAILED(decoder->GetFrame(0, frame.put()))) return false;

        UINT srcW = 0, srcH = 0;
        if (FAILED(frame->GetSize(&srcW, &srcH)) || srcW == 0 || srcH == 0) return false;

        // Phóng tỷ lệ Fill (UniformToFill) giống mặc định của Windows 11 Desktop Wallpaper
        double scale = std::max(static_cast<double>(monW) / srcW, static_cast<double>(monH) / srcH);
        UINT scaledW = std::max(static_cast<UINT>(monW), static_cast<UINT>(std::ceil(srcW * scale)));
        UINT scaledH = std::max(static_cast<UINT>(monH), static_cast<UINT>(std::ceil(srcH * scale)));

        winrt::com_ptr<IWICBitmapScaler> scaler;
        if (FAILED(factory->CreateBitmapScaler(scaler.put())) ||
            FAILED(scaler->Initialize(frame.get(), scaledW, scaledH, WICBitmapInterpolationModeLinear))) {
            return false;
        }
        winrt::com_ptr<IWICFormatConverter> conv;
        if (FAILED(factory->CreateFormatConverter(conv.put())) ||
            FAILED(conv->Initialize(scaler.get(), GUID_WICPixelFormat32bppBGR,
                                    WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
            return false;
        }

        WICRect cropRect = {
            static_cast<INT>((scaledW - static_cast<UINT>(monW)) / 2),
            static_cast<INT>((scaledH - static_cast<UINT>(monH)) / 2),
            monW,
            monH
        };
        outMonitorPixels.resize(static_cast<size_t>(monW) * monH);
        if (FAILED(conv->CopyPixels(&cropRect, static_cast<UINT>(monW * 4),
                                    static_cast<UINT>(outMonitorPixels.size() * 4),
                                    reinterpret_cast<BYTE*>(outMonitorPixels.data())))) {
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

// ---------------------------------------------------------------------------
// 6. Mô hình Dữ liệu Viên thuốc Trạng thái Góc Trái/Trên (< > chuyển đổi + Bấm mở nhanh)
//    Hỗ trợ: Phát nhạc (Ảnh bìa + Sóng nhạc), Tiến trình (Icon + 77%), Hẹn giờ/Bấm giờ, Hotspot, Micro/Cam/Vị trí
// ---------------------------------------------------------------------------
enum class CornerItemKind {
    None = 0,
    Media,
    TaskProgress,
    Vpn,
    Timer,
    Stopwatch,
    Hotspot,
    Microphone,
    Camera,
    Location
};

struct ActiveMediaDisplayInfo {
    bool available = false;
    bool playing = false;
    std::wstring title;
    std::wstring artist;
    std::wstring sourceAppId;
    uint64_t artworkId = 0;
    std::shared_ptr<DecodedBgraImage> artwork;
};

struct ActiveTaskDisplayInfo {
    bool active = false;
    int percent = -1; // -1 = indeterminate, 0..100 = tỷ lệ %
    uintptr_t targetWindow = 0;
    std::wstring appName;
};

struct ActiveClockDisplayInfo {
    bool timerRunning = false;
    int64_t timerBaseMs = 0;
    ULONGLONG timerSampleTick = 0;
    std::wstring timerLabel;
    HWND clockTopHwnd = nullptr;

    bool stopwatchRunning = false;
    int64_t stopwatchBaseMs = 0;
    ULONGLONG stopwatchSampleTick = 0;

    int64_t CurrentTimerSeconds(ULONGLONG nowTick) const {
        if (!timerRunning) return 0;
        uint64_t dt = (nowTick >= timerSampleTick) ? (nowTick - timerSampleTick) : 0;
        int64_t remMs = std::max<int64_t>(0, timerBaseMs - static_cast<int64_t>(dt));
        return (remMs + 999) / 1000;
    }

    int64_t CurrentStopwatchSeconds(ULONGLONG nowTick) const {
        if (!stopwatchRunning) return 0;
        uint64_t dt = (nowTick >= stopwatchSampleTick) ? (nowTick - stopwatchSampleTick) : 0;
        int64_t elMs = std::max<int64_t>(0, stopwatchBaseMs + static_cast<int64_t>(dt));
        return elMs / 1000;
    }
};

struct PersistentIndicators {
    ActiveMediaDisplayInfo media;
    ActiveTaskDisplayInfo  task;
    ActiveClockDisplayInfo clock;
    bool vpnConnected = false;
    std::wstring vpnName;
    bool hotspotOn = false;
    int  hotspotClients = 0;
    bool micActive = false;
    bool camActive = false;
    bool locActive = false;
};

static std::mutex g_persistMutex;
static PersistentIndicators g_persistIndicators;
static CornerItemKind g_cornerSelectedKind = CornerItemKind::None;
static int g_cornerHoverZone = 0; // 0 = none, 1 = prev (<), 2 = center action, 3 = next (>)
static std::atomic<bool> g_requestMediaTogglePlayPause{false};

std::wstring FormatClockCompactTime(int64_t totalSeconds) {
    totalSeconds = std::max<int64_t>(0, totalSeconds);
    int64_t hrs = totalSeconds / 3600;
    int64_t mins = (totalSeconds % 3600) / 60;
    int64_t secs = totalSeconds % 60;
    wchar_t buf[32] = {};
    if (hrs > 0) {
        swprintf_s(buf, L"%lld:%02lld:%02lld", static_cast<long long>(hrs),
                   static_cast<long long>(mins), static_cast<long long>(secs));
    } else {
        swprintf_s(buf, L"%02lld:%02lld", static_cast<long long>(mins), static_cast<long long>(secs));
    }
    return buf;
}

struct CornerResolvedItem {
    CornerItemKind kind = CornerItemKind::None;
    const wchar_t* glyph = L"";
    std::wstring badgeText;
    HWND targetHwnd = nullptr;
    bool mediaPlaying = false;
    uint64_t artworkId = 0;
    std::shared_ptr<DecodedBgraImage> artwork;
    std::wstring mediaSourceApp;
    std::wstring mediaTitle;
    std::wstring mediaArtist;
};

static const wchar_t* PickTaskProgressGlyph(const std::wstring& appName, HWND targetHwnd) {
    std::wstring low = appName;
    for (wchar_t& ch : low) ch = static_cast<wchar_t>(towlower(ch));

    // 1. File Explorer operations (Copy / Move / Delete / Extract)
    if (low.find(L"file explorer") != std::wstring::npos || low.find(L"explorer") != std::wstring::npos) {
        if (targetHwnd && IsWindow(targetHwnd)) {
            wchar_t titleBuf[256] = {};
            if (GetWindowTextW(targetHwnd, titleBuf, ARRAYSIZE(titleBuf)) > 0) {
                std::wstring titleLow(titleBuf);
                for (wchar_t& ch : titleLow) ch = static_cast<wchar_t>(towlower(ch));
                if (titleLow.find(L"delete") != std::wstring::npos ||
                    titleLow.find(L"xóa") != std::wstring::npos ||
                    titleLow.find(L"recycle") != std::wstring::npos) {
                    return L"\uE74D"; // Delete / Trash
                }
                if (titleLow.find(L"extract") != std::wstring::npos ||
                    titleLow.find(L"giải nén") != std::wstring::npos ||
                    titleLow.find(L"compress") != std::wstring::npos ||
                    titleLow.find(L"nén") != std::wstring::npos) {
                    return L"\uF012"; // ZipFolder / Archive
                }
                if (titleLow.find(L"copy") != std::wstring::npos ||
                    titleLow.find(L"sao chép") != std::wstring::npos ||
                    titleLow.find(L"move") != std::wstring::npos ||
                    titleLow.find(L"di chuyển") != std::wstring::npos) {
                    return L"\uE8C8"; // Copy / File Transfer
                }
            }
        }
        return L"\uE8B7"; // Folder (File Explorer default)
    }

    // 2. Archive / compression tools
    if (low.find(L"winrar") != std::wstring::npos ||
        low.find(L"7-zip") != std::wstring::npos ||
        low.find(L"7z") != std::wstring::npos ||
        low.find(L"bandizip") != std::wstring::npos ||
        low.find(L"nanazip") != std::wstring::npos ||
        low.find(L"peazip") != std::wstring::npos) {
        return L"\uF012"; // ZipFolder
    }

    // 3. Browsers, download managers & game stores
    if (low.find(L"chrome") != std::wstring::npos ||
        low.find(L"edge") != std::wstring::npos ||
        low.find(L"firefox") != std::wstring::npos ||
        low.find(L"brave") != std::wstring::npos ||
        low.find(L"opera") != std::wstring::npos ||
        low.find(L"vivaldi") != std::wstring::npos ||
        low.find(L"cốc cốc") != std::wstring::npos ||
        low.find(L"coccoc") != std::wstring::npos ||
        low.find(L"idm") != std::wstring::npos ||
        low.find(L"download") != std::wstring::npos ||
        low.find(L"fdm") != std::wstring::npos ||
        low.find(L"torrent") != std::wstring::npos ||
        low.find(L"motrix") != std::wstring::npos ||
        low.find(L"steam") != std::wstring::npos ||
        low.find(L"epic") != std::wstring::npos ||
        low.find(L"store") != std::wstring::npos) {
        return L"\uE896"; // Download
    }

    // 4. General app / installer / system progress
    return L"\uE895"; // Sync / Task Progress
}

std::vector<CornerResolvedItem> BuildActiveCornerItems(const PersistentIndicators& ind, ULONGLONG nowTick) {
    std::vector<CornerResolvedItem> items;
    items.reserve(8);

    if (ind.media.available) {
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Media;
        it.glyph = L"\uEC4F";
        it.mediaPlaying = ind.media.playing;
        it.artworkId = ind.media.artworkId;
        it.artwork = ind.media.artwork;
        it.mediaSourceApp = ind.media.sourceAppId;
        it.mediaTitle = ind.media.title;
        it.mediaArtist = ind.media.artist;
        items.push_back(std::move(it));
    }
    if (ind.task.active) {
        CornerResolvedItem it{};
        it.kind = CornerItemKind::TaskProgress;
        HWND taskHwnd = reinterpret_cast<HWND>(ind.task.targetWindow);
        it.glyph = PickTaskProgressGlyph(ind.task.appName, taskHwnd);
        it.badgeText = (ind.task.percent >= 0) ? (std::to_wstring(ind.task.percent) + L"%") : L"…";
        it.targetHwnd = taskHwnd;
        items.push_back(std::move(it));
    }
    if (ind.vpnConnected) {
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Vpn;
        it.glyph = L"\uE705";
        it.badgeText = L"VPN";
        items.push_back(std::move(it));
    }
    if (ind.clock.timerRunning) {
        int64_t remSec = ind.clock.CurrentTimerSeconds(nowTick);
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Timer;
        it.glyph = L"\uE916";
        it.badgeText = FormatClockCompactTime(remSec);
        it.targetHwnd = ind.clock.clockTopHwnd;
        items.push_back(std::move(it));
    }
    if (ind.clock.stopwatchRunning) {
        int64_t elSec = ind.clock.CurrentStopwatchSeconds(nowTick);
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Stopwatch;
        it.glyph = L"\uE916";
        it.badgeText = FormatClockCompactTime(elSec);
        it.targetHwnd = ind.clock.clockTopHwnd;
        items.push_back(std::move(it));
    }
    if (ind.hotspotOn) {
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Hotspot;
        it.glyph = L"\uEC15";
        it.badgeText = (ind.hotspotClients > 0) ? std::to_wstring(ind.hotspotClients) : L"";
        items.push_back(std::move(it));
    }
    if (ind.micActive) {
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Microphone;
        it.glyph = L"\uE720";
        items.push_back(std::move(it));
    }
    if (ind.camActive) {
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Camera;
        it.glyph = L"\uE714";
        items.push_back(std::move(it));
    }
    if (ind.locActive) {
        CornerResolvedItem it{};
        it.kind = CornerItemKind::Location;
        it.glyph = L"\uE707";
        items.push_back(std::move(it));
    }
    return items;
}

PersistentIndicators GetPersistentIndicatorsSnapshot() {
    std::lock_guard<std::mutex> lock(g_persistMutex);
    return g_persistIndicators;
}

int ResolveSelectedCornerIndex(const std::vector<CornerResolvedItem>& items) {
    if (items.empty()) {
        g_cornerSelectedKind = CornerItemKind::None;
        return -1;
    }
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].kind == g_cornerSelectedKind) {
            return static_cast<int>(i);
        }
    }
    g_cornerSelectedKind = items[0].kind;
    return 0;
}

static bool IsKnownBrowserExeName(const std::wstring& exeLow) {
    static constexpr PCWSTR kBrowsers[] = {
        L"chrome.exe", L"msedge.exe", L"firefox.exe", L"brave.exe",
        L"opera.exe", L"vivaldi.exe", L"browser.exe", L"coccoc.exe",
        L"zen.exe", L"floorp.exe", L"arc.exe", L"waterfox.exe", L"librewolf.exe"
    };
    for (PCWSTR b : kBrowsers) {
        if (exeLow == b) return true;
    }
    return false;
}

static std::wstring NormalizeMediaMatchText(std::wstring s) {
    s = ToLowerStr(std::move(s));
    // Bỏ tiền tố số thông báo dạng "(18) " ở đầu tiêu đề tab YouTube/trình duyệt
    size_t p = 0;
    while (p < s.size() && iswspace(s[p])) ++p;
    if (p < s.size() && s[p] == L'(') {
        size_t closeP = s.find(L')', p + 1);
        if (closeP != std::wstring::npos && closeP - p <= 7) {
            bool allDigits = (closeP > p + 1);
            for (size_t k = p + 1; k < closeP; ++k) {
                if (!iswdigit(s[k]) && s[k] != L'+') { allDigits = false; break; }
            }
            if (allDigits) {
                p = closeP + 1;
                while (p < s.size() && iswspace(s[p])) ++p;
                s = s.substr(p);
            }
        }
    }
    while (!s.empty() && iswspace(s.back())) s.pop_back();
    return s;
}

static int ScoreBrowserTabForMedia(const std::wstring& rawTabName, const std::wstring& normTitle, const std::wstring& normArtist) {
    if (rawTabName.empty()) return 0;
    std::wstring normTab = NormalizeMediaMatchText(rawTabName);
    if (normTab.empty()) return 0;

    int score = 0;
    if (!normTitle.empty()) {
        if (normTab.find(normTitle) != std::wstring::npos) {
            score += 1000;
        } else if (normTab.size() >= 6 && normTitle.find(normTab) != std::wstring::npos) {
            score += 850;
        } else {
            // So khớp theo tỷ lệ từ khóa cho trường hợp tiêu đề tab bị rút gọn
            int totalWords = 0, matchedWords = 0;
            size_t i = 0;
            while (i < normTitle.size()) {
                while (i < normTitle.size() && !iswalnum(normTitle[i])) ++i;
                size_t st = i;
                while (i < normTitle.size() && iswalnum(normTitle[i])) ++i;
                if (i - st >= 3) {
                    ++totalWords;
                    if (normTab.find(normTitle.substr(st, i - st)) != std::wstring::npos) {
                        ++matchedWords;
                    }
                }
            }
            if (totalWords >= 2 && matchedWords * 100 >= totalWords * 60) {
                score += 550 + matchedWords * 20;
            }
        }
    }
    if (normArtist.size() >= 3 && normTab.find(normArtist) != std::wstring::npos) {
        score += 250;
    }
    if (normTab.find(L"audio playing") != std::wstring::npos ||
        normTab.find(L"playing audio") != std::wstring::npos ||
        normTab.find(L"đang phát âm thanh") != std::wstring::npos ||
        normTab.find(L"âm thanh đang phát") != std::wstring::npos) {
        score += 350;
    }
    return score;
}

static std::vector<HWND> CollectMediaPlayerWindowsByAppId(const std::wstring& appId, bool& outIsBrowser) {
    outIsBrowser = false;
    std::wstring lowId = ToLowerStr(appId);
    struct SearchCtx {
        std::wstring needle;
        std::vector<HWND> matched;
        std::vector<HWND> allBrowsers;
        bool matchedBrowser = false;
    } ctx;

    if (lowId.find(L"chrome") != std::wstring::npos) ctx.needle = L"chrome.exe";
    else if (lowId.find(L"msedge") != std::wstring::npos) ctx.needle = L"msedge.exe";
    else if (lowId.find(L"firefox") != std::wstring::npos) ctx.needle = L"firefox.exe";
    else if (lowId.find(L"brave") != std::wstring::npos) ctx.needle = L"brave.exe";
    else if (lowId.find(L"opera") != std::wstring::npos) ctx.needle = L"opera.exe";
    else if (lowId.find(L"vivaldi") != std::wstring::npos) ctx.needle = L"vivaldi.exe";
    else if (lowId.find(L"coccoc") != std::wstring::npos) ctx.needle = L"browser.exe";
    else if (lowId.find(L"spotify") != std::wstring::npos) ctx.needle = L"spotify.exe";
    else if (lowId.find(L"vlc") != std::wstring::npos) ctx.needle = L"vlc.exe";
    else if (lowId.find(L"zunemusic") != std::wstring::npos || lowId.find(L"mediaplayer") != std::wstring::npos) ctx.needle = L"microsoft.media.player.exe";
    else {
        size_t sl = lowId.find_last_of(L"\\/");
        ctx.needle = (sl != std::wstring::npos) ? lowId.substr(sl + 1) : lowId;
    }

    struct MediaEnumCb {
        static BOOL CALLBACK Proc(HWND w, LPARAM lp) {
            auto& c = *reinterpret_cast<SearchCtx*>(lp);
            if (!IsWindowVisible(w) || GetWindowTextLengthW(w) == 0) return TRUE;
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid == 0 || pid == GetCurrentProcessId()) return TRUE;
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProc) return TRUE;
        wchar_t path[1024] = {};
        DWORD cch = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(hProc, 0, path, &cch) && path[0]) {
            std::wstring base = ToLowerStr(PathFindFileNameW(path));
            bool isBr = IsKnownBrowserExeName(base);
            if (isBr) c.allBrowsers.push_back(w);
            if (!c.needle.empty() && (base == c.needle || base.find(c.needle) != std::wstring::npos)) {
                c.matched.push_back(w);
                if (isBr) c.matchedBrowser = true;
            }
        }
            CloseHandle(hProc);
            return TRUE;
        }
    };
    EnumWindows(MediaEnumCb::Proc, reinterpret_cast<LPARAM>(&ctx));

    if (!ctx.matched.empty()) {
        outIsBrowser = ctx.matchedBrowser;
        return ctx.matched;
    }
    if (!ctx.allBrowsers.empty()) {
        outIsBrowser = true;
        return ctx.allBrowsers;
    }
    return {};
}

static void FocusWindowClean(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    if (IsIconic(hWnd)) ShowWindowAsync(hWnd, SW_RESTORE);
    SetForegroundWindow(hWnd);
}

static bool TrySwitchToBrowserMediaTab(const std::vector<HWND>& browserWindows,
                                       const std::wstring& mediaTitle,
                                       const std::wstring& mediaArtist) {
    if (browserWindows.empty()) return false;
    std::wstring normTitle  = NormalizeMediaMatchText(mediaTitle);
    std::wstring normArtist = NormalizeMediaMatchText(mediaArtist);

    // 1. Kiểm tra nhanh: nếu tab đang phát đã là tab hiện hành của một cửa sổ trình duyệt nào đó
    if (!normTitle.empty()) {
        for (HWND w : browserWindows) {
            if (!IsWindow(w)) continue;
            wchar_t wt[512] = {};
            if (GetWindowTextW(w, wt, ARRAYSIZE(wt)) > 0) {
                if (NormalizeMediaMatchText(wt).find(normTitle) != std::wstring::npos) {
                    FocusWindowClean(w);
                    return true;
                }
            }
        }
    }

    // 2. Quét siêu nhanh thanh Tab của trình duyệt bằng UI Automation (bỏ qua toàn bộ DOM Document -> <15ms!)
    winrt::com_ptr<IUIAutomation> uia;
    {
        winrt::com_ptr<IUIAutomation2> uia2;
        if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
                                       __uuidof(IUIAutomation2), uia2.put_void())) && uia2) {
            uia2->put_ConnectionTimeout(180);
            uia2->put_TransactionTimeout(180);
            uia2->put_AutoSetFocus(FALSE);
            uia = uia2;
        } else {
            CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
                             __uuidof(IUIAutomation), uia.put_void());
        }
    }
    if (!uia) return false;

    winrt::com_ptr<IUIAutomationTreeWalker> walker;
    if (FAILED(uia->get_ControlViewWalker(walker.put())) || !walker) return false;

    HWND bestHwnd = nullptr;
    winrt::com_ptr<IUIAutomationElement> bestTab;
    int bestScore = 0;

    ULONGLONG deadline = GetTickCount64() + 500;
    for (HWND w : browserWindows) {
        if (!IsWindow(w) || GetTickCount64() >= deadline) continue;
        winrt::com_ptr<IUIAutomationElement> root;
        if (FAILED(uia->ElementFromHandle(w, root.put())) || !root) continue;

        struct QNode { winrt::com_ptr<IUIAutomationElement> el; unsigned depth; };
        std::vector<QNode> q;
        q.reserve(160);
        q.push_back({ std::move(root), 0 });

        for (size_t i = 0; i < q.size() && i < 320 && GetTickCount64() < deadline; ++i) {
            auto cur = q[i];
            CONTROLTYPEID ctype = 0;
            cur.el->get_CurrentControlType(&ctype);

            // Bỏ qua hoàn toàn nội dung trang web (Document) để tránh duyệt DOM nặng
            if (ctype == UIA_DocumentControlTypeId) continue;

            if (ctype == UIA_TabItemControlTypeId) {
                BSTR bName = nullptr;
                if (SUCCEEDED(cur.el->get_CurrentName(&bName)) && bName) {
                    std::wstring tabName(bName, SysStringLen(bName));
                    SysFreeString(bName);
                    int sc = ScoreBrowserTabForMedia(tabName, normTitle, normArtist);
                    if (sc > bestScore) {
                        bestScore = sc;
                        bestHwnd = w;
                        bestTab = cur.el;
                    }
                }
                continue;
            }

            if (cur.depth >= 10) continue;
            winrt::com_ptr<IUIAutomationElement> ch;
            if (FAILED(walker->GetFirstChildElement(cur.el.get(), ch.put()))) continue;
            while (ch && q.size() < 320 && GetTickCount64() < deadline) {
                q.push_back({ ch, cur.depth + 1 });
                winrt::com_ptr<IUIAutomationElement> nxt;
                if (FAILED(walker->GetNextSiblingElement(ch.get(), nxt.put()))) break;
                ch = std::move(nxt);
            }
        }
    }

    if (bestTab && bestHwnd && bestScore >= 300) {
        FocusWindowClean(bestHwnd);
        bool activated = false;
        winrt::com_ptr<IUIAutomationSelectionItemPattern> selPat;
        if (SUCCEEDED(bestTab->GetCurrentPatternAs(UIA_SelectionItemPatternId, __uuidof(IUIAutomationSelectionItemPattern), selPat.put_void())) && selPat) {
            activated = SUCCEEDED(selPat->Select());
        }
        if (!activated) {
            winrt::com_ptr<IUIAutomationInvokePattern> invPat;
            if (SUCCEEDED(bestTab->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern), invPat.put_void())) && invPat) {
                activated = SUCCEEDED(invPat->Invoke());
            }
        }
        if (!activated) {
            bestTab->SetFocus();
        }
        return true;
    }
    return false;
}

static std::atomic<bool> g_mediaActivateInFlight{false};

struct MediaActivateJob {
    std::wstring sourceApp;
    std::wstring title;
    std::wstring artist;
};

static DWORD WINAPI MediaActivateWorkerProc(LPVOID lpParam) {
    std::unique_ptr<MediaActivateJob> job(static_cast<MediaActivateJob*>(lpParam));
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    bool isBrowser = false;
    auto windows = CollectMediaPlayerWindowsByAppId(job->sourceApp, isBrowser);
    bool handled = false;
    if (isBrowser && !windows.empty()) {
        handled = TrySwitchToBrowserMediaTab(windows, job->title, job->artist);
    }
    if (!handled) {
        if (!windows.empty()) {
            FocusWindowClean(windows.front());
        } else {
            g_requestMediaTogglePlayPause.store(true, std::memory_order_relaxed);
            if (g_watcherWakeEvent) SetEvent(g_watcherWakeEvent);
        }
    }

    if (SUCCEEDED(hrCom)) CoUninitialize();
    g_mediaActivateInFlight.store(false, std::memory_order_relaxed);
    return 0;
}

void ExecuteCornerItemAction(const CornerResolvedItem& item) {
    switch (item.kind) {
        case CornerItemKind::Media: {
            if (!g_mediaActivateInFlight.exchange(true, std::memory_order_relaxed)) {
                AllowSetForegroundWindow(ASFW_ANY);
                auto* job = new MediaActivateJob{ item.mediaSourceApp, item.mediaTitle, item.mediaArtist };
                HANDLE hTh = CreateThread(nullptr, 0, MediaActivateWorkerProc, job, 0, nullptr);
                if (hTh) {
                    CloseHandle(hTh);
                } else {
                    delete job;
                    g_mediaActivateInFlight.store(false, std::memory_order_relaxed);
                }
            }
            break;
        }
        case CornerItemKind::TaskProgress:
            if (item.targetHwnd && IsWindow(item.targetHwnd)) {
                if (IsIconic(item.targetHwnd)) ShowWindowAsync(item.targetHwnd, SW_RESTORE);
                SetForegroundWindow(item.targetHwnd);
            }
            break;
        case CornerItemKind::Vpn:
            ShellExecuteW(nullptr, L"open", L"ms-settings:network-vpn", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case CornerItemKind::Hotspot:
            ShellExecuteW(nullptr, L"open", L"ms-settings:network-mobilehotspot", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case CornerItemKind::Microphone:
            ShellExecuteW(nullptr, L"open", L"ms-settings:privacy-microphone", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case CornerItemKind::Camera:
            ShellExecuteW(nullptr, L"open", L"ms-settings:privacy-webcam", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case CornerItemKind::Location:
            ShellExecuteW(nullptr, L"open", L"ms-settings:privacy-location", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case CornerItemKind::Timer:
        case CornerItemKind::Stopwatch:
            if (item.targetHwnd && IsWindow(item.targetHwnd)) {
                if (IsIconic(item.targetHwnd)) ShowWindowAsync(item.targetHwnd, SW_RESTORE);
                SetForegroundWindow(item.targetHwnd);
            } else {
                ShellExecuteW(nullptr, L"open", L"ms-clock:", nullptr, nullptr, SW_SHOWNORMAL);
            }
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// 7. Hình học Taskbar Ngang & Dọc (Kích thước Popup mặc định 600px + Viên thuốc Góc < >)
// ---------------------------------------------------------------------------
struct TaskbarPillsGeometry {
    HWND hTaskbar = nullptr;
    RECT rcTaskbarScreen{};
    RECT rcAppsLocal{};
    RECT rcTrayLocal{};
    RECT rcPersistLocal{};
    RECT rcPersistScreen{};
    RECT rcPopupSlotScreen{};
    int  cornerDiamPx = 36;
    UINT dpi = 96;
    bool isVertical = false;
    bool hasApps = false;
    bool hasTray = false;
    bool hasPersist = false;
    bool nativeAutoHide = false;
    bool valid = false;
};

static int  g_cachedOpenAppWindows = 4;
static bool g_hideAppsPillForPopup = false;

static BOOL CALLBACK CountTaskbarEligibleWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindow(hwnd, GW_OWNER) != nullptr && !(ex & WS_EX_APPWINDOW)) return TRUE;
    if (GetWindowTextLengthW(hwnd) == 0) return TRUE;
    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0) {
        return TRUE;
    }
    (*reinterpret_cast<int*>(lParam))++;
    return TRUE;
}

void RefreshOpenAppWindowCountOnce() {
    int count = 0;
    EnumWindows(CountTaskbarEligibleWindowsProc, reinterpret_cast<LPARAM>(&count));
    g_cachedOpenAppWindows = std::clamp(count, 1, 24);
}

bool QueryTaskbarPillsGeometry(TaskbarPillsGeometry& out) {
    out = {};
    HWND hTaskbar = FindCurrentProcessTaskbarWnd();
    if (!hTaskbar || !IsWindow(hTaskbar)) return false;

    RECT rcTb{};
    if (!GetWindowRect(hTaskbar, &rcTb)) return false;
    int tbW = rcTb.right - rcTb.left;
    int tbH = rcTb.bottom - rcTb.top;
    if (tbW <= 20 || tbH <= 20) return false;

    bool isVert = (tbH > tbW);
    UINT dpi = GetDpiForWindow(hTaskbar);
    if (dpi == 0) dpi = 96;

    APPBARDATA abd = { sizeof(abd), hTaskbar };
    UINT_PTR abState = SHAppBarMessage(ABM_GETSTATE, &abd);
    bool nativeAutoHide = (abState & ABS_AUTOHIDE) != 0;

    int padOuter = DpiScale(g_uiSettings.pillVerticalMarginDip, dpi);
    int gapPills = DpiScale(8, dpi);
    int crossThickness = (isVert ? tbW : tbH) - padOuter * 2;
    int cornerDiam = std::clamp(std::min(crossThickness, DpiScale(g_uiSettings.pillCornerRadiusDip * 2, dpi)), 10, 64);

    MeasuredXamlTaskbarBounds xamlSnap;
    {
        std::lock_guard<std::mutex> lock(g_xamlBoundsMutex);
        xamlSnap = g_measuredBounds;
    }

    PersistentIndicators persist = GetPersistentIndicatorsSnapshot();
    ULONGLONG nowTick = GetTickCount64();
    auto activeCornerItems = BuildActiveCornerItems(persist, nowTick);
    int selCornerIdx = ResolveSelectedCornerIndex(activeCornerItems);

    RECT rcAppsLocal{};
    RECT rcTrayLocal{};
    RECT rcPersistLocal{};
    bool hasApps = false;
    bool hasTray = false;
    bool hasPersist = (selCornerIdx >= 0);

    HWND hTray = FindWindowExW(hTaskbar, nullptr, L"TrayNotifyWnd", nullptr);
    RECT rcTrayWin{};
    bool hasTrayWin = (hTray && GetWindowRect(hTray, &rcTrayWin) &&
                       (rcTrayWin.right > rcTrayWin.left) && (rcTrayWin.bottom > rcTrayWin.top));

    if (!isVert) {
        // ==================== BỐ CỤC NGANG (DƯỚI / TRÊN) ====================
        int pillTop = padOuter;
        int pillBot = tbH - padOuter;

        if (xamlSnap.hasTray) {
            rcTrayLocal = xamlSnap.rcTrayLocal;
            rcTrayLocal.top = pillTop;
            rcTrayLocal.bottom = pillBot;
            hasTray = true;
        } else if (hasTrayWin) {
            int rawTrayL = static_cast<int>(rcTrayWin.left - rcTb.left);
            int trayL = std::clamp(rawTrayL - DpiScale(6, dpi), DpiScale(120, dpi), tbW - DpiScale(48, dpi));
            rcTrayLocal = { trayL, pillTop, tbW - DpiScale(2, dpi), pillBot };
            hasTray = true;
        }

        int maxAppsRight = hasTray ? (rcTrayLocal.left - gapPills) : (tbW - DpiScale(8, dpi));

        if (xamlSnap.hasApps && xamlSnap.rcAppsLocal.right > xamlSnap.rcAppsLocal.left) {
            rcAppsLocal = xamlSnap.rcAppsLocal;
            rcAppsLocal.top = pillTop;
            rcAppsLocal.bottom = pillBot;
            rcAppsLocal.right = std::min<LONG>(rcAppsLocal.right, maxAppsRight);
            hasApps = (rcAppsLocal.right > rcAppsLocal.left + DpiScale(32, dpi));
        }
        if (!hasApps) {
            int estDipW = std::clamp((g_cachedOpenAppWindows + 3) * 44 + 16, 180, 1100);
            int appW = DpiScale(estDipW, dpi);
            int appL = g_cachedTaskbarCentered
                ? std::max(DpiScale(8, dpi), (tbW - appW) / 2)
                : DpiScale(8, dpi);
            int appR = std::min(maxAppsRight, appL + appW);
            rcAppsLocal = { appL, pillTop, std::max(appL + DpiScale(80, dpi), appR), pillBot };
            hasApps = true;
        }

        if (hasPersist) {
            const auto& curItem = activeCornerItems[selCornerIdx];
            bool multi = (activeCornerItems.size() >= 2);
            int centerContentW = 0;
            if (curItem.kind == CornerItemKind::Media) {
                // Ảnh bìa 24 DIP + khoảng cách 6 DIP + Sóng nhạc 4 cột 18 DIP + đệm 10 DIP
                centerContentW = DpiScale(58, dpi);
            } else {
                int textChars = static_cast<int>(curItem.badgeText.size());
                centerContentW = DpiScale(28, dpi) + (textChars > 0 ? (textChars * DpiScale(7, dpi) + DpiScale(8, dpi)) : 0);
            }
            int arrowsW = multi ? DpiScale(38, dpi) : 0;
            int badgeW = std::max(pillBot - pillTop, centerContentW + arrowsW + DpiScale(8, dpi));
            int badgeL = DpiScale(8, dpi);
            if (badgeL + badgeW + gapPills > rcAppsLocal.left) {
                badgeL = rcAppsLocal.right + gapPills;
            }
            if (badgeL + badgeW <= maxAppsRight) {
                rcPersistLocal = { badgeL, pillTop, badgeL + badgeW, pillBot };
            } else {
                hasPersist = false;
            }
        }
    } else {
        // ==================== BỐ CỤC DỌC (TRÁI / PHẢI) ====================
        int pillLeft  = padOuter;
        int pillRight = tbW - padOuter;

        if (xamlSnap.hasTray) {
            rcTrayLocal = xamlSnap.rcTrayLocal;
            rcTrayLocal.left = pillLeft;
            rcTrayLocal.right = pillRight;
            hasTray = true;
        } else if (hasTrayWin) {
            int rawTrayT = static_cast<int>(rcTrayWin.top - rcTb.top);
            int trayT = std::clamp(rawTrayT - DpiScale(6, dpi), DpiScale(120, dpi), tbH - DpiScale(60, dpi));
            rcTrayLocal = { pillLeft, trayT, pillRight, tbH - DpiScale(2, dpi) };
            hasTray = true;
        }

        int maxAppsBottom = hasTray ? (rcTrayLocal.top - gapPills) : (tbH - DpiScale(8, dpi));

        if (xamlSnap.hasApps && xamlSnap.rcAppsLocal.bottom > xamlSnap.rcAppsLocal.top) {
            rcAppsLocal = xamlSnap.rcAppsLocal;
            rcAppsLocal.left = pillLeft;
            rcAppsLocal.right = pillRight;
            rcAppsLocal.bottom = std::min<LONG>(rcAppsLocal.bottom, maxAppsBottom);
            hasApps = (rcAppsLocal.bottom > rcAppsLocal.top + DpiScale(32, dpi));
        }
        if (!hasApps) {
            int estH = DpiScale(std::clamp((g_cachedOpenAppWindows + 3) * 44 + 16, 180, 720), dpi);
            int availTop = DpiScale(8, dpi);
            int spanH = std::min(estH, std::max(DpiScale(80, dpi), maxAppsBottom - availTop));
            int appT = g_cachedTaskbarCentered
                ? (availTop + (maxAppsBottom - availTop - spanH) / 2)
                : availTop;
            rcAppsLocal = { pillLeft, appT, pillRight, appT + spanH };
            hasApps = true;
        }

        if (hasPersist) {
            const auto& curItem = activeCornerItems[selCornerIdx];
            bool multi = (activeCornerItems.size() >= 2);
            int baseH = DpiScale(34, dpi);
            if (curItem.kind == CornerItemKind::Media) {
                baseH = DpiScale(48, dpi);
            } else if (curItem.badgeText.size() >= 3) {
                baseH = DpiScale(44, dpi);
            }
            int arrowsH = multi ? DpiScale(18, dpi) : 0;
            int badgeH = std::max(pillRight - pillLeft, baseH + arrowsH);
            int badgeT = DpiScale(8, dpi);
            if (badgeT + badgeH + gapPills > rcAppsLocal.top) {
                badgeT = rcAppsLocal.bottom + gapPills;
            }
            if (badgeT + badgeH <= maxAppsBottom) {
                rcPersistLocal = { pillLeft, badgeT, pillRight, badgeT + badgeH };
            } else {
                hasPersist = false;
            }
        }
    }

    // Tọa độ Popup 3 giây ở giữa (Mặc định rộng 600 DIP / 600px, căn thẳng hàng tâm Thanh ứng dụng!)
    RECT rcSlot{};
    if (!isVert) {
        int slotTop = rcTb.top + rcAppsLocal.top;
        int slotBot = rcTb.top + rcAppsLocal.bottom;
        int minLeftScreen  = rcTb.left + (hasPersist && rcPersistLocal.right <= rcAppsLocal.left
                                            ? (rcPersistLocal.right + gapPills) : DpiScale(8, dpi));
        int maxRightScreen = rcTb.left + (hasTray ? (rcTrayLocal.left - gapPills) : (tbW - DpiScale(8, dpi)));
        int availW = std::max(DpiScale(220, dpi), maxRightScreen - minLeftScreen);

        int targetW = 0;
        if (g_uiSettings.popupSizeMode == PopupSizeMode::Fixed) {
            targetW = std::min(availW, DpiScale(g_uiSettings.fixedPopupWidthDip, dpi));
        } else {
            int appsW = rcAppsLocal.right - rcAppsLocal.left;
            targetW = std::clamp(appsW, std::min(availW, DpiScale(420, dpi)), availW);
        }

        int centerScreenX = g_uiSettings.replaceAppsPillOnAlert
            ? (rcTb.left + (rcAppsLocal.left + rcAppsLocal.right) / 2)
            : (rcTb.left + tbW / 2);
        int slotL = std::clamp(centerScreenX - targetW / 2, minLeftScreen, std::max(minLeftScreen, maxRightScreen - targetW));
        rcSlot = { slotL, slotTop, slotL + targetW, slotBot };
    } else {
        int slotLeft  = rcTb.left + rcAppsLocal.left;
        int slotRight = rcTb.left + rcAppsLocal.right;
        int minTopScreen = rcTb.top + (hasPersist && rcPersistLocal.bottom <= rcAppsLocal.top
                                            ? (rcPersistLocal.bottom + gapPills) : DpiScale(8, dpi));
        int maxBotScreen = rcTb.top + (hasTray ? (rcTrayLocal.top - gapPills) : (tbH - DpiScale(8, dpi)));
        int availH = std::max(DpiScale(160, dpi), maxBotScreen - minTopScreen);

        int targetH = 0;
        if (g_uiSettings.popupSizeMode == PopupSizeMode::Fixed) {
            targetH = std::min(availH, DpiScale(g_uiSettings.fixedPopupVertHeightDip, dpi));
        } else {
            int appsH = rcAppsLocal.bottom - rcAppsLocal.top;
            targetH = std::clamp(appsH, std::min(availH, DpiScale(240, dpi)), availH);
        }

        int centerScreenY = g_uiSettings.replaceAppsPillOnAlert
            ? (rcTb.top + (rcAppsLocal.top + rcAppsLocal.bottom) / 2)
            : (rcTb.top + tbH / 2);
        int slotT = std::clamp(centerScreenY - targetH / 2, minTopScreen, std::max(minTopScreen, maxBotScreen - targetH));
        rcSlot = { slotLeft, slotT, slotRight, slotT + targetH };
    }

    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        if (!isVert) {
            int slotH = rcSlot.bottom - rcSlot.top;
            if (rcSlot.bottom > mi.rcMonitor.bottom + 2) {
                rcSlot.bottom = mi.rcMonitor.bottom - std::max(padOuter, DpiScale(4, dpi));
                rcSlot.top = rcSlot.bottom - slotH;
            } else if (rcSlot.top < mi.rcMonitor.top - 2) {
                rcSlot.top = mi.rcMonitor.top + std::max(padOuter, DpiScale(4, dpi));
                rcSlot.bottom = rcSlot.top + slotH;
            }
        } else {
            int slotW = rcSlot.right - rcSlot.left;
            if (rcSlot.left < mi.rcMonitor.left - 2) {
                rcSlot.left = mi.rcMonitor.left + std::max(padOuter, DpiScale(4, dpi));
                rcSlot.right = rcSlot.left + slotW;
            } else if (rcSlot.right > mi.rcMonitor.right + 2) {
                rcSlot.right = mi.rcMonitor.right - std::max(padOuter, DpiScale(4, dpi));
                rcSlot.left = rcSlot.right - slotW;
            }
        }
    }

    out.hTaskbar = hTaskbar;
    out.rcTaskbarScreen = rcTb;
    out.rcAppsLocal = rcAppsLocal;
    out.rcTrayLocal = rcTrayLocal;
    out.rcPersistLocal = rcPersistLocal;
    out.rcPersistScreen = {
        rcTb.left + rcPersistLocal.left,
        rcTb.top  + rcPersistLocal.top,
        rcTb.left + rcPersistLocal.right,
        rcTb.top  + rcPersistLocal.bottom
    };
    out.rcPopupSlotScreen = rcSlot;
    out.cornerDiamPx = cornerDiam;
    out.dpi = dpi;
    out.isVertical = isVert;
    out.hasApps = hasApps;
    out.hasTray = hasTray;
    out.hasPersist = hasPersist;
    out.nativeAutoHide = nativeAutoHide;
    out.valid = true;
    return true;
}

void SetAppsPillHiddenForPopup(bool hide) {
    bool effectiveHide = hide && g_uiSettings.replaceAppsPillOnAlert && g_uiSettings.enableSplitTaskbar;
    if (g_hideAppsPillForPopup == effectiveHide) return;
    g_hideAppsPillForPopup = effectiveHide;
    g_hideAppsPillAtomic.store(effectiveHide, std::memory_order_relaxed);
    PostFastSyncToTaskbar(1);
}

void RestoreDefaultTaskbarEverything() {
    g_hideAppsPillForPopup = false;
    g_hideAppsPillAtomic.store(false, std::memory_order_relaxed);
    HWND hTaskbar = FindCurrentProcessTaskbarWnd();
    if (hTaskbar && IsWindow(hTaskbar)) {
        RunOnWindowThread(hTaskbar, [](void* p) {
            SyncTaskbarXamlOnOwnerThread(static_cast<HWND>(p), true);
        }, hTaskbar);
        SetWindowRgn(hTaskbar, nullptr, TRUE);
        ApplyTaskbarClearComposition(hTaskbar, false);
        if (!IsWindowVisible(hTaskbar)) {
            if (pOrigShowWindow) pOrigShowWindow(hTaskbar, SW_SHOWNA);
            else ShowWindowAsync(hTaskbar, SW_SHOWNA);
        }
    }
}

// ---------------------------------------------------------------------------
// 8. Engine Optical Liquid Glass Sạch Tuyệt Đối (Wallpaper WIC + PrintWindow Cửa Sổ Nền)
//    - Không bao giờ tự chụp nhầm biểu tượng Shell_TrayWnd vào kính!
//    - Không gọi DwmFlush() hay SetWindowDisplayAffinity() -> 0ms lag!
// ---------------------------------------------------------------------------
void BlurBufferSeparableRGB(std::vector<DWORD>& buf, std::vector<DWORD>& tmp, int w, int h, int radius) {
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
                sumR += (p >> 16) & 0xFF;
                sumG += (p >> 8) & 0xFF;
                sumB += p & 0xFF;
            }
            for (int x = 0; x < w; ++x) {
                rowOut[x] = (((sumR * invDiam) >> 24) << 16) |
                            (((sumG * invDiam) >> 24) << 8)  |
                             ((sumB * invDiam) >> 24);
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
                sumR += (p >> 16) & 0xFF;
                sumG += (p >> 8) & 0xFF;
                sumB += p & 0xFF;
            }
            for (int y = 0; y < h; ++y) {
                buf[static_cast<size_t>(y) * w + x] =
                    (((sumR * invDiam) >> 24) << 16) |
                    (((sumG * invDiam) >> 24) << 8)  |
                     ((sumB * invDiam) >> 24);
                DWORD pOut = tmp[static_cast<size_t>(std::clamp(y - radius, 0, h - 1)) * w + x];
                DWORD pIn  = tmp[static_cast<size_t>(std::clamp(y + radius + 1, 0, h - 1)) * w + x];
                sumR += ((pIn >> 16) & 0xFF) - ((pOut >> 16) & 0xFF);
                sumG += ((pIn >> 8) & 0xFF)  - ((pOut >> 8) & 0xFF);
                sumB += (pIn & 0xFF)         - (pOut & 0xFF);
            }
        }
    }
}

struct DibCanvas {
    HDC hdcMem = nullptr;
    HBITMAP hbm = nullptr;
    HGDIOBJ oldBmp = nullptr;
    void* bits = nullptr;
    int capW = 0;
    int capH = 0;

    bool Ensure(int w, int h) {
        if (w <= capW && h <= capH && hdcMem && hbm) return true;
        int nw = std::max({ w, capW, 64 });
        int nh = std::max({ h, capH, 64 });
        Release();

        HDC hdcScr = GetDC(nullptr);
        hdcMem = CreateCompatibleDC(hdcScr);
        ReleaseDC(nullptr, hdcScr);
        if (!hdcMem) return false;

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = nw;
        bmi.bmiHeader.biHeight = -nh;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
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

struct CachedMonitorWallpaper {
    RECT rcMonitor{};
    uint64_t lastWriteFt = 0;
    ULONGLONG lastStatTick = 0;
    std::wstring pathUsed;
    std::vector<DWORD> pixels;
    bool valid = false;
};
static CachedMonitorWallpaper g_wallpaperCache;
static DibCanvas g_printWndCanvas;

void EnsureMonitorWallpaperLoaded(const RECT& rcMonitor) {
    int monW = rcMonitor.right - rcMonitor.left;
    int monH = rcMonitor.bottom - rcMonitor.top;
    if (monW <= 32 || monH <= 32) return;

    ULONGLONG now = GetTickCount64();
    bool sameMon = EqualRect(&rcMonitor, &g_wallpaperCache.rcMonitor);
    if (g_wallpaperCache.valid && sameMon && (now - g_wallpaperCache.lastStatTick < 3000)) {
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

bool IsRealAppBackdropWindow(HWND w, DWORD selfPid) {
    if (!w || w == g_cornerWidgetHwnd || w == g_alertHwnd || w == g_cachedTaskbarHwnd) return false;
    if (!IsWindowVisible(w) || IsIconic(w)) return false;
    LONG_PTR ex = GetWindowLongPtrW(w, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return false;

    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0) {
        return false;
    }

    wchar_t cls[64] = {};
    if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) return false;
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(cls, L"Progman") == 0 ||
        _wcsicmp(cls, L"WorkerW") == 0 ||
        _wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
        _wcsicmp(cls, L"TaskListThumbnailWnd") == 0 ||
        _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (pid == selfPid && _wcsicmp(cls, L"CabinetWClass") != 0 && _wcsicmp(cls, L"ExploreWClass") != 0) {
        return false;
    }
    return true;
}

struct PreblurredRegion {
    int capX = 0, capY = 0, capW = 0, capH = 0;
    int hw = 0, hh = 0;
    bool isLight = false;
    std::vector<DWORD> raw;
    std::vector<DWORD> halfSoft;
    std::vector<DWORD> halfCrisp;
    std::vector<DWORD> blurTmp;
};

// Thu thập nền sạch phía sau Taskbar (Wallpaper + Cửa sổ ứng dụng giao cắt nếu có), KHÔNG BAO GIỜ chụp dính icon Taskbar!
bool CaptureCleanBackdropBehindTaskbar(const RECT& rcTarget, int padPx, UINT dpi, const ModSettings& s, PreblurredRegion& out) {
    int tw = rcTarget.right - rcTarget.left;
    int th = rcTarget.bottom - rcTarget.top;
    if (tw <= 8 || th <= 8) return false;

    out.capX = rcTarget.left - padPx;
    out.capY = rcTarget.top - padPx;
    out.capW = tw + padPx * 2;
    out.capH = th + padPx * 2;
    out.raw.resize(static_cast<size_t>(out.capW) * out.capH);

    HMONITOR hMon = MonitorFromRect(&rcTarget, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) return false;

    // 1. Lớp nền Hình nền Desktop gốc (sạch 100%, 0ms không gọi BitBlt!)
    EnsureMonitorWallpaperLoaded(mi.rcMonitor);
    int monW = mi.rcMonitor.right - mi.rcMonitor.left;
    int monH = mi.rcMonitor.bottom - mi.rcMonitor.top;
    if (g_wallpaperCache.valid && static_cast<int>(g_wallpaperCache.pixels.size()) == monW * monH) {
        for (int y = 0; y < out.capH; ++y) {
            int my = std::clamp((out.capY + y) - static_cast<int>(mi.rcMonitor.top), 0, monH - 1);
            const DWORD* srcRow = &g_wallpaperCache.pixels[static_cast<size_t>(my) * monW];
            DWORD* dstRow = &out.raw[static_cast<size_t>(y) * out.capW];
            for (int x = 0; x < out.capW; ++x) {
                int mx = std::clamp((out.capX + x) - static_cast<int>(mi.rcMonitor.left), 0, monW - 1);
                dstRow[x] = srcRow[mx];
            }
        }
    }

    // 2. Nếu có cửa sổ ứng dụng thực sự nằm đè phía dưới vùng Taskbar (ví dụ khi bật Tự động ẩn Taskbar trên Chrome, Hình 2!)
    RECT rcCap = { out.capX, out.capY, out.capX + out.capW, out.capY + out.capH };
    DWORD selfPid = GetCurrentProcessId();
    std::vector<std::pair<HWND, RECT>> overlappingWindows;
    for (HWND w = GetTopWindow(nullptr); w != nullptr; w = GetWindow(w, GW_HWNDNEXT)) {
        if (!IsRealAppBackdropWindow(w, selfPid)) continue;
        RECT rcW{};
        if (!GetWindowRect(w, &rcW) || rcW.right <= rcW.left || rcW.bottom <= rcW.top) continue;
        RECT rcInterTarget{};
        if (!IntersectRect(&rcInterTarget, &rcW, &rcTarget)) continue;
        overlappingWindows.push_back({ w, rcW });
        // Nếu cửa sổ đã che kín hoàn toàn toàn bộ dải rcCap thì không cần xét cửa sổ bên dưới nó nữa
        if (rcW.left <= rcCap.left && rcW.top <= rcCap.top &&
            rcW.right >= rcCap.right && rcW.bottom >= rcCap.bottom) {
            break;
        }
        if (overlappingWindows.size() >= 3) break;
    }

    // Vẽ đè theo thứ tự từ dưới lên trên (back-to-front)
    for (int idx = static_cast<int>(overlappingWindows.size()) - 1; idx >= 0; --idx) {
        HWND w = overlappingWindows[idx].first;
        const RECT& rcW = overlappingWindows[idx].second;
        int ww = rcW.right - rcW.left;
        int wh = rcW.bottom - rcW.top;
        if (ww <= 16 || wh <= 16 || ww > 7680 || wh > 4320) continue;

        bool printedOk = false;
        if (g_printWndCanvas.Ensure(ww, wh)) {
            DWORD* wndBits = static_cast<DWORD*>(g_printWndCanvas.bits);
            RECT rcInter{};
            if (IntersectRect(&rcInter, &rcW, &rcCap)) {
                for (int sy = rcInter.top; sy < rcInter.bottom; ++sy) {
                    std::fill_n(&wndBits[static_cast<size_t>(sy - rcW.top) * g_printWndCanvas.capW + (rcInter.left - rcW.left)],
                                rcInter.right - rcInter.left, 0u);
                }
                if (PrintWindow(w, g_printWndCanvas.hdcMem, PW_RENDERFULLCONTENT)) {
                    GdiFlush();
                    uint64_t nonZero = 0;
                    for (int sy = rcInter.top; sy < rcInter.bottom; sy += 4) {
                        const DWORD* rPtr = &wndBits[static_cast<size_t>(sy - rcW.top) * g_printWndCanvas.capW];
                        for (int sx = rcInter.left; sx < rcInter.right; sx += 8) {
                            if ((rPtr[sx - rcW.left] & 0x00FFFFFFu) != 0) ++nonZero;
                        }
                    }
                    if (nonZero > 0) {
                        printedOk = true;
                        for (int sy = rcInter.top; sy < rcInter.bottom; ++sy) {
                            const DWORD* srcRow = &wndBits[static_cast<size_t>(sy - rcW.top) * g_printWndCanvas.capW];
                            DWORD* dstRow = &out.raw[static_cast<size_t>(sy - out.capY) * out.capW];
                            for (int sx = rcInter.left; sx < rcInter.right; ++sx) {
                                dstRow[sx - out.capX] = srcRow[sx - rcW.left] & 0x00FFFFFFu;
                            }
                        }
                    }
                }
            }
        }

        // Dự phòng siêu nhanh nếu cửa sổ chặn PrintWindow: lấy dải pixel thực sát mép ngoài Taskbar
        if (!printedOk) {
            bool isVert = (th > tw);
            HDC hScr = GetDC(nullptr);
            if (hScr) {
                HDC hMem = CreateCompatibleDC(hScr);
                int stripX = !isVert ? out.capX : ((rcTarget.left <= mi.rcMonitor.left + 20) ? (rcTarget.right + 1) : (rcTarget.left - 17));
                int stripY = !isVert ? ((rcTarget.top <= mi.rcMonitor.top + 20) ? (rcTarget.bottom + 1) : (rcTarget.top - 17)) : out.capY;
                int stripW = !isVert ? out.capW : 16;
                int stripH = !isVert ? 16 : out.capH;
                if (g_printWndCanvas.Ensure(stripW, stripH) &&
                    BitBlt(g_printWndCanvas.hdcMem, 0, 0, stripW, stripH, hScr, stripX, stripY, SRCCOPY)) {
                    GdiFlush();
                    const DWORD* sBits = static_cast<const DWORD*>(g_printWndCanvas.bits);
                    RECT rcInter{};
                    if (IntersectRect(&rcInter, &rcW, &rcCap)) {
                        for (int sy = rcInter.top; sy < rcInter.bottom; ++sy) {
                            DWORD* dstRow = &out.raw[static_cast<size_t>(sy - out.capY) * out.capW];
                            for (int sx = rcInter.left; sx < rcInter.right; ++sx) {
                                int lx = !isVert ? (sx - out.capX) : ((sy + sx) & 15);
                                int ly = !isVert ? ((sy + sx) & 15) : (sy - out.capY);
                                dstRow[sx - out.capX] = sBits[static_cast<size_t>(std::clamp(ly, 0, stripH - 1)) * g_printWndCanvas.capW +
                                                              std::clamp(lx, 0, stripW - 1)] & 0x00FFFFFFu;
                            }
                        }
                    }
                }
                DeleteDC(hMem);
                ReleaseDC(nullptr, hScr);
            }
        }
    }

    // 3. Đo tương phản Sáng / Tối tự động
    bool isLight = g_cachedSystemLightTheme;
    if (s.colorMode == ColorMode::Light) {
        isLight = true;
    } else if (s.colorMode == ColorMode::Dark) {
        isLight = false;
    } else {
        uint64_t lumaSum = 0;
        uint32_t cnt = 0;
        int stepX = std::max(2, tw / 24);
        int stepY = std::max(2, th / 10);
        for (int sy = padPx + 2; sy < padPx + th - 2; sy += stepY) {
            const DWORD* row = &out.raw[static_cast<size_t>(sy) * out.capW];
            for (int sx = padPx + 2; sx < padPx + tw - 2; sx += stepX) {
                DWORD px = row[sx];
                lumaSum += (77u * ((px >> 16) & 0xFF) + 150u * ((px >> 8) & 0xFF) + 29u * (px & 0xFF)) >> 8;
                ++cnt;
            }
        }
        if (cnt > 0) isLight = ((lumaSum / cnt) >= 165);
    }
    out.isLight = isLight;

    // 4. Tạo 2 tầng mờ quang học (Soft & Crisp) + tăng độ trong dịu mắt
    out.hw = std::max(8, out.capW / 2);
    out.hh = std::max(8, out.capH / 2);
    out.halfSoft.resize(static_cast<size_t>(out.hw) * out.hh);
    out.blurTmp.resize(static_cast<size_t>(out.hw) * out.hh);

    for (int hy = 0; hy < out.hh; ++hy) {
        int sy0 = std::clamp(hy * 2,     0, out.capH - 1);
        int sy1 = std::clamp(hy * 2 + 1, 0, out.capH - 1);
        const DWORD* r0 = &out.raw[static_cast<size_t>(sy0) * out.capW];
        const DWORD* r1 = &out.raw[static_cast<size_t>(sy1) * out.capW];
        DWORD* dst = &out.halfSoft[static_cast<size_t>(hy) * out.hw];
        for (int hx = 0; hx < out.hw; ++hx) {
            int sx0 = std::clamp(hx * 2,     0, out.capW - 1);
            int sx1 = std::clamp(hx * 2 + 1, 0, out.capW - 1);
            DWORD p0 = r0[sx0], p1 = r0[sx1], p2 = r1[sx0], p3 = r1[sx1];
            uint32_t r = (((p0 >> 16) & 0xFF) + ((p1 >> 16) & 0xFF) + ((p2 >> 16) & 0xFF) + ((p3 >> 16) & 0xFF)) >> 2;
            uint32_t g = (((p0 >> 8) & 0xFF)  + ((p1 >> 8) & 0xFF)  + ((p2 >> 8) & 0xFF)  + ((p3 >> 8) & 0xFF))  >> 2;
            uint32_t b = ((p0 & 0xFF)         + (p1 & 0xFF)         + (p2 & 0xFF)         + (p3 & 0xFF))         >> 2;
            dst[hx] = (r << 16) | (g << 8) | b;
        }
    }

    out.halfCrisp = out.halfSoft;
    int glassPct = std::clamp(s.glassClarity, -100, 100);
    float negF = (glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
    float posF = (glassPct > 0) ? (std::clamp(glassPct, 0, 100) / 100.0f) : 0.0f;

    int softRad  = std::clamp(static_cast<int>(std::lround((7.5f + 6.0f * posF) * (dpi / 96.0f))), 5, 22);
    int crispRad = std::clamp(softRad / 2, 3, 12);
    BlurBufferSeparableRGB(out.halfSoft,  out.blurTmp, out.hw, out.hh, softRad);
    BlurBufferSeparableRGB(out.halfCrisp, out.blurTmp, out.hw, out.hh, crispRad);

    const float satBoost = 1.14f + 0.08f * negF - 0.08f * posF;
    const float tintWeight = (glassPct < 0)
        ? (0.12f + 0.08f * (1.0f - negF))
        : (0.20f + 0.42f * posF);
    const int satScale = static_cast<int>(std::lround(satBoost * 256.0f));
    const int tintW = static_cast<int>(std::lround(tintWeight * 256.0f));
    const int invTintW = 256 - tintW;
    const int tintR = (isLight ? 244 : 14) * tintW;
    const int tintG = (isLight ? 246 : 16) * tintW;
    const int tintB = (isLight ? 252 : 24) * tintW;

    auto applyVibrancy = [&](std::vector<DWORD>& buf) {
        for (DWORD& px : buf) {
            int r = (px >> 16) & 0xFF;
            int g = (px >> 8) & 0xFF;
            int b = px & 0xFF;
            int luma = (77 * r + 150 * g + 29 * b) >> 8;
            r = std::clamp(luma + (((r - luma) * satScale) >> 8), 0, 255);
            g = std::clamp(luma + (((g - luma) * satScale) >> 8), 0, 255);
            b = std::clamp(luma + (((b - luma) * satScale) >> 8), 0, 255);
            r = std::clamp((r * invTintW + tintR) >> 8, 0, 255);
            g = std::clamp((g * invTintW + tintG) >> 8, 0, 255);
            b = std::clamp((b * invTintW + tintB) >> 8, 0, 255);
            px = (static_cast<DWORD>(r) << 16) | (static_cast<DWORD>(g) << 8) | static_cast<DWORD>(b);
        }
    };
    applyVibrancy(out.halfSoft);
    applyVibrancy(out.halfCrisp);
    return true;
}

// Khúc xạ mặt kính Liquid Glass tinh khiết (KHÔNG tạo quầng sương trắng chói mắt)
void RenderRefractedPillToBuffer(const PreblurredRegion& reg, int canvasScreenX, int canvasScreenY,
                                 DWORD* dstCanvas, int canvasW, int canvasH,
                                 const RECT& rcPillInCanvas, float cornerRadiusPx, UINT dpi, int glassPct, bool premulOuterAA) {
    int pw = rcPillInCanvas.right - rcPillInCanvas.left;
    int ph = rcPillInCanvas.bottom - rcPillInCanvas.top;
    if (pw <= 4 || ph <= 4 || reg.halfSoft.empty()) return;

    float negF = (glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
    const float zoom = 1.018f + 0.022f * negF;
    const float invZoomDelta = 1.0f - (1.0f / zoom);
    const float minDim = static_cast<float>(std::min(pw, ph));
    const float cornerR = std::clamp(cornerRadiusPx, 6.0f, minDim * 0.5f);
    const float bevelW  = std::min((dpi / 96.0f) * (9.0f + 2.5f * negF), minDim * 0.24f);
    const float maxDisp = (dpi / 96.0f) * (11.0f + 11.0f * negF);

    const float cx = pw * 0.5f;
    const float cy = ph * 0.5f;
    const float hxBox = std::max(0.0f, cx - cornerR);
    const float hyBox = std::max(0.0f, cy - cornerR);

    auto sampleBilinear = [&](const std::vector<DWORD>& src, float fx, float fy) -> DWORD {
        fx = std::clamp(fx, 0.0f, static_cast<float>(reg.hw - 1));
        fy = std::clamp(fy, 0.0f, static_cast<float>(reg.hh - 1));
        int x0 = static_cast<int>(fx);
        int y0 = static_cast<int>(fy);
        int x1 = std::min(x0 + 1, reg.hw - 1);
        int y1 = std::min(y0 + 1, reg.hh - 1);
        uint32_t wx = static_cast<uint32_t>((fx - x0) * 256.0f);
        uint32_t wy = static_cast<uint32_t>((fy - y0) * 256.0f);
        uint32_t iwx = 256u - wx;
        uint32_t iwy = 256u - wy;

        DWORD p00 = src[static_cast<size_t>(y0) * reg.hw + x0];
        DWORD p10 = src[static_cast<size_t>(y0) * reg.hw + x1];
        DWORD p01 = src[static_cast<size_t>(y1) * reg.hw + x0];
        DWORD p11 = src[static_cast<size_t>(y1) * reg.hw + x1];

        auto ch = [&](int shift) -> uint32_t {
            uint32_t c0 = (((p00 >> shift) & 0xFF) * iwx + ((p10 >> shift) & 0xFF) * wx) >> 8;
            uint32_t c1 = (((p01 >> shift) & 0xFF) * iwx + ((p11 >> shift) & 0xFF) * wx) >> 8;
            return (c0 * iwy + c1 * wy) >> 8;
        };
        return (ch(16) << 16) | (ch(8) << 8) | ch(0);
    };

    const int baseOffX = (canvasScreenX + rcPillInCanvas.left) - reg.capX;
    const int baseOffY = (canvasScreenY + rcPillInCanvas.top)  - reg.capY;

    for (int py = 0; py < ph; ++py) {
        int cyDst = rcPillInCanvas.top + py;
        if (cyDst < 0 || cyDst >= canvasH) continue;
        DWORD* dstRow = &dstCanvas[static_cast<size_t>(cyDst) * canvasW];

        float dy = (py + 0.5f) - cy;
        float qy = std::fabs(dy) - hyBox;
        float oy = std::max(qy, 0.0f);
        float baseShiftY = -dy * invZoomDelta;

        for (int px = 0; px < pw; ++px) {
            int cxDst = rcPillInCanvas.left + px;
            if (cxDst < 0 || cxDst >= canvasW) continue;

            float dx = (px + 0.5f) - cx;
            float qx = std::fabs(dx) - hxBox;
            float ox = std::max(qx, 0.0f);

            float outDist = std::sqrt(ox * ox + oy * oy);
            float inDist  = std::min(std::max(qx, qy), 0.0f);
            float sdf     = outDist + inDist - cornerR;

            if (premulOuterAA && sdf >= 0.65f) continue;

            float edgeAA = premulOuterAA ? std::clamp(0.65f - sdf, 0.0f, 1.0f) : 1.0f;
            float distFromEdge = std::max(0.0f, -sdf);
            float tBevel = (sdf < 0.0f && distFromEdge < bevelW && bevelW > 0.5f)
                ? (1.0f - distFromEdge / bevelW)
                : (sdf >= 0.0f ? 1.0f : 0.0f);
            float baseShiftX = -dx * invZoomDelta;

            if (tBevel <= 0.001f && edgeAA >= 0.999f) {
                float sx = (baseOffX + px + baseShiftX) * 0.5f;
                float sy = (baseOffY + py + baseShiftY) * 0.5f;
                dstRow[cxDst] = 0xFF000000u | sampleBilinear(reg.halfSoft, sx, sy);
                continue;
            }

            float nx = 0.0f, ny = 0.0f;
            if (outDist > 1e-4f) {
                nx = (dx >= 0.0f ? 1.0f : -1.0f) * (ox / outDist);
                ny = (dy >= 0.0f ? 1.0f : -1.0f) * (oy / outDist);
            } else {
                float maxQ = std::max(qx, qy);
                float wx = std::exp(std::clamp((qx - maxQ) * 0.28f, -20.0f, 0.0f));
                float wy = std::exp(std::clamp((qy - maxQ) * 0.28f, -20.0f, 0.0f));
                nx = (dx >= 0.0f ? 1.0f : -1.0f) * wx;
                ny = (dy >= 0.0f ? 1.0f : -1.0f) * wy;
                float nLen = std::sqrt(nx * nx + ny * ny);
                if (nLen > 1e-4f) { nx /= nLen; ny /= nLen; }
            }

            float lensCurve = tBevel * tBevel * (0.38f + 0.62f * tBevel);
            float dispX = -nx * lensCurve * maxDisp;
            float dispY = -ny * lensCurve * maxDisp;

            float sxR = (baseOffX + px + baseShiftX + dispX * 0.86f) * 0.5f;
            float syR = (baseOffY + py + baseShiftY + dispY * 0.86f) * 0.5f;
            float sxG = (baseOffX + px + baseShiftX + dispX * 1.00f) * 0.5f;
            float syG = (baseOffY + py + baseShiftY + dispY * 1.00f) * 0.5f;
            float sxB = (baseOffX + px + baseShiftX + dispX * 1.14f) * 0.5f;
            float syB = (baseOffY + py + baseShiftY + dispY * 1.14f) * 0.5f;

            uint32_t r = (sampleBilinear(reg.halfSoft, sxR, syR) >> 16) & 0xFF;
            uint32_t g = (sampleBilinear(reg.halfSoft, sxG, syG) >> 8)  & 0xFF;
            uint32_t b =  sampleBilinear(reg.halfSoft, sxB, syB)        & 0xFF;

            if (tBevel > 0.50f) {
                float cmF = std::clamp((tBevel - 0.50f) / 0.50f, 0.0f, 1.0f);
                uint32_t cm = static_cast<uint32_t>(std::lround(cmF * cmF * 0.28f * 256.0f));
                uint32_t invCm = 256u - cm;
                uint32_t rC = (sampleBilinear(reg.halfCrisp, sxR, syR) >> 16) & 0xFF;
                uint32_t gC = (sampleBilinear(reg.halfCrisp, sxG, syG) >> 8)  & 0xFF;
                uint32_t bC =  sampleBilinear(reg.halfCrisp, sxB, syB)        & 0xFF;
                r = (r * invCm + rC * cm) >> 8;
                g = (g * invCm + gC * cm) >> 8;
                b = (b * invCm + bC * cm) >> 8;
            }

            float lightTL = std::max(0.0f, -0.68f * nx - 0.73f * ny);
            float lightBR = std::max(0.0f,  0.68f * nx + 0.73f * ny);
            float fresnel = std::clamp(std::pow(tBevel, 3.8f) * (0.04f + 0.11f * lightTL + 0.08f * lightBR), 0.0f, 0.16f);

            uint32_t frU8 = static_cast<uint32_t>(std::lround(fresnel * 256.0f));
            uint32_t invFr = 256u - frU8;

            r = std::min(255u, (r * invFr + 242u * frU8) >> 8);
            g = std::min(255u, (g * invFr + 246u * frU8) >> 8);
            b = std::min(255u, (b * invFr + 255u * frU8) >> 8);

            uint32_t aByte = static_cast<uint32_t>(std::clamp(static_cast<int>(std::lround(edgeAA * 255.0f)), 1, 255));
            uint32_t rPremul = (r * aByte) >> 8;
            uint32_t gPremul = (g * aByte) >> 8;
            uint32_t bPremul = (b * aByte) >> 8;
            dstRow[cxDst] = (aByte << 24) | (rPremul << 16) | (gPremul << 8) | bPremul;
        }
    }
}

uint64_t ComputeTaskbarBackdropSignature(const RECT& rcTb) {
    DWORD selfPid = GetCurrentProcessId();
    uint64_t hash = 14695981039346656037ULL;
    auto mix = [&](uint64_t v) { hash ^= v; hash *= 1099511628211ULL; };

    mix(g_wallpaperCache.lastWriteFt);

    for (HWND w = GetTopWindow(nullptr); w != nullptr; w = GetWindow(w, GW_HWNDNEXT)) {
        if (!IsRealAppBackdropWindow(w, selfPid)) continue;
        RECT rcW{};
        if (!GetWindowRect(w, &rcW) || rcW.right <= rcW.left || rcW.bottom <= rcW.top) continue;
        RECT rcInter{};
        if (!IntersectRect(&rcInter, &rcW, &rcTb)) continue;

        mix(reinterpret_cast<uintptr_t>(w));
        mix(static_cast<uint64_t>(rcInter.left));
        mix(static_cast<uint64_t>(rcInter.top));
        mix(static_cast<uint64_t>(rcInter.right));
        mix(static_cast<uint64_t>(rcInter.bottom));
    }
    return hash;
}

// ---------------------------------------------------------------------------
// 9. Tài nguyên Direct2D, Đo Động Độ Rộng Chữ, Vẽ Ảnh Bìa & Sóng Nhạc Động
// ---------------------------------------------------------------------------
static DibCanvas g_alertCanvas;
static DibCanvas g_cornerCanvas;

static ID2D1Factory* g_pD2DFactory = nullptr;
static ID2D1DCRenderTarget* g_pDCRT = nullptr;
static ID2D1SolidColorBrush* g_pBrush = nullptr;
static ID2D1LinearGradientBrush* g_pRimGradBrushLight = nullptr;
static ID2D1LinearGradientBrush* g_pRimGradBrushDark = nullptr;
static IDWriteFactory* g_pDWrite = nullptr;

// Cache ảnh bìa bài hát trên GPU Direct2D
static uint64_t g_cachedD2DArtworkId = 0;
static ID2D1Bitmap* g_pCachedD2DArtworkBmp = nullptr;
static ID2D1BitmapBrush* g_pCachedD2DArtworkBrush = nullptr;

void ReleaseArtworkGpuCache() {
    if (g_pCachedD2DArtworkBrush) { g_pCachedD2DArtworkBrush->Release(); g_pCachedD2DArtworkBrush = nullptr; }
    if (g_pCachedD2DArtworkBmp)   { g_pCachedD2DArtworkBmp->Release();   g_pCachedD2DArtworkBmp   = nullptr; }
    g_cachedD2DArtworkId = 0;
}

bool EnsureD2DArtworkBrush(const std::shared_ptr<DecodedBgraImage>& art, uint64_t artId) {
    if (!art || art->width == 0 || art->height == 0 || art->pixels.empty() || !g_pDCRT) return false;
    if (g_pCachedD2DArtworkBrush && g_cachedD2DArtworkId == artId && artId != 0) return true;

    ReleaseArtworkGpuCache();
    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
    if (SUCCEEDED(g_pDCRT->CreateBitmap(
            D2D1::SizeU(art->width, art->height),
            art->pixels.data(), art->width * 4, &props, &g_pCachedD2DArtworkBmp)) && g_pCachedD2DArtworkBmp) {
        D2D1_BITMAP_BRUSH_PROPERTIES bp = D2D1::BitmapBrushProperties(
            D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        if (SUCCEEDED(g_pDCRT->CreateBitmapBrush(g_pCachedD2DArtworkBmp, bp, &g_pCachedD2DArtworkBrush))) {
            g_cachedD2DArtworkId = artId;
            return true;
        }
    }
    return false;
}

void DrawRoundedArtworkBox(const D2D1_RECT_F& rBox, float cornerRad,
                           const std::shared_ptr<DecodedBgraImage>& art, uint64_t artId, bool isLight) {
    if (!EnsureD2DArtworkBrush(art, artId) || !g_pCachedD2DArtworkBrush || !art) return;
    float bw = rBox.right - rBox.left;
    float bh = rBox.bottom - rBox.top;
    if (bw <= 2.0f || bh <= 2.0f) return;

    float sx = bw / static_cast<float>(art->width);
    float sy = bh / static_cast<float>(art->height);
    float sc = std::max(sx, sy);
    float offX = rBox.left + (bw - art->width * sc) * 0.5f;
    float offY = rBox.top  + (bh - art->height * sc) * 0.5f;

    g_pCachedD2DArtworkBrush->SetTransform(
        D2D1::Matrix3x2F::Scale(sc, sc) * D2D1::Matrix3x2F::Translation(offX, offY));
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rBox, cornerRad, cornerRad);
    g_pDCRT->FillRoundedRectangle(rr, g_pCachedD2DArtworkBrush);

    g_pBrush->SetColor(isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.16f) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.24f));
    g_pDCRT->DrawRoundedRectangle(rr, g_pBrush, 0.9f);
}

// Vẽ hiệu ứng Sóng nhạc (Equalizer Waveform) mượt mà, sống động
void DrawMusicWaveformBars(const D2D1_RECT_F& rWave, int numBars, bool playing, ULONGLONG nowTick,
                           const D2D1_COLOR_F& colWave, UINT dpi) {
    float totalW = rWave.right - rWave.left;
    float maxH   = rWave.bottom - rWave.top;
    if (totalW <= 4.0f || maxH <= 4.0f || numBars < 3) return;

    float gap = DpiScaleF(2.2f, dpi);
    float barW = std::clamp((totalW - gap * (numBars - 1)) / numBars, DpiScaleF(2.2f, dpi), DpiScaleF(3.6f, dpi));
    float usedW = barW * numBars + gap * (numBars - 1);
    float startX = rWave.left + (totalW - usedW) * 0.5f;
    float midY   = (rWave.top + rWave.bottom) * 0.5f;

    double t = static_cast<double>(nowTick) * 0.001;
    static constexpr double kSpeeds[5] = { 7.4, 9.8, 6.3, 11.1, 8.2 };
    static constexpr double kPhases[5] = { 0.0, 1.3, 2.7, 0.8, 3.6 };

    g_pBrush->SetColor(colWave);
    for (int i = 0; i < numBars; ++i) {
        float ratio = 0.26f;
        if (playing) {
            double s1 = 0.5 + 0.5 * std::sin(t * kSpeeds[i % 5] + kPhases[i % 5]);
            double s2 = 0.5 + 0.5 * std::cos(t * (kSpeeds[(i + 2) % 5] * 0.67) + i * 1.1);
            ratio = static_cast<float>(0.22 + 0.78 * (0.65 * s1 + 0.35 * s2));
        }
        float bh = std::max(barW, maxH * ratio);
        float x0 = startX + i * (barW + gap);
        D2D1_RECT_F rb = D2D1::RectF(x0, midY - bh * 0.5f, x0 + barW, midY + bh * 0.5f);
        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rb, barW * 0.5f, barW * 0.5f), g_pBrush);
    }
}

struct AlertFonts {
    UINT dpi = 0;
    int scalePct = 0;
    IDWriteTextFormat* pIcon = nullptr;
    IDWriteTextFormat* pBadgeIcon = nullptr;
    IDWriteTextFormat* pChevronIcon = nullptr;
    IDWriteTextFormat* pBadgeCount = nullptr;
    IDWriteTextFormat* pBadgeVertTime = nullptr;
    IDWriteTextFormat* pTitle = nullptr;
    IDWriteTextFormat* pSub = nullptr;
    IDWriteTextFormat* pVal = nullptr;
    IDWriteTextFormat* pValCenter = nullptr;

    void Release() {
        if (pIcon)          { pIcon->Release();          pIcon = nullptr; }
        if (pBadgeIcon)     { pBadgeIcon->Release();     pBadgeIcon = nullptr; }
        if (pChevronIcon)   { pChevronIcon->Release();   pChevronIcon = nullptr; }
        if (pBadgeCount)    { pBadgeCount->Release();    pBadgeCount = nullptr; }
        if (pBadgeVertTime) { pBadgeVertTime->Release(); pBadgeVertTime = nullptr; }
        if (pTitle)         { pTitle->Release();         pTitle = nullptr; }
        if (pSub)           { pSub->Release();           pSub = nullptr; }
        if (pVal)           { pVal->Release();           pVal = nullptr; }
        if (pValCenter)     { pValCenter->Release();     pValCenter = nullptr; }
        dpi = 0;
        scalePct = 0;
    }
};
static AlertFonts g_fonts;

// Đo chính xác bề rộng pixel của chuỗi DirectWrite (đảm bảo tiêu đề "Đã sao chép văn bản" KHÔNG BAO GIỜ bị cắt chữ!)
float MeasureTextExactWidthPx(IDWriteTextFormat* pFmt, const std::wstring& text) {
    if (!g_pDWrite || !pFmt || text.empty()) return 0.0f;
    IDWriteTextLayout* pLayout = nullptr;
    float width = 0.0f;
    if (SUCCEEDED(g_pDWrite->CreateTextLayout(
            text.c_str(), static_cast<UINT32>(text.size()), pFmt, 4096.0f, 256.0f, &pLayout)) && pLayout) {
        DWRITE_TEXT_METRICS metrics{};
        if (SUCCEEDED(pLayout->GetMetrics(&metrics))) {
            width = std::ceil(metrics.widthIncludingTrailingWhitespace);
        }
        pLayout->Release();
    }
    return width;
}

struct GlassBitmapSlotCache {
    int w = 0, h = 0;
    bool isLight = false;
    bool valid = false;
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

static GlassBitmapSlotCache g_popupGlassCache;
static GlassBitmapSlotCache g_cornerGlassCache;

bool EnsureGlassBrushFromSlotCache(GlassBitmapSlotCache& cache, ID2D1RenderTarget* pRT) {
    if (!cache.valid || cache.pixels.empty() || !pRT) return false;
    if (cache.pBrush && cache.pOwner == pRT) return true;

    cache.ReleaseGpu();
    D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
    if (SUCCEEDED(pRT->CreateBitmap(
            D2D1::SizeU(static_cast<UINT32>(cache.w), static_cast<UINT32>(cache.h)),
            cache.pixels.data(),
            static_cast<UINT32>(cache.w * 4),
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

bool EnsureRenderResources(UINT dpi, int scalePct) {
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
    if (!g_pDCRT) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f);
        if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&props, &g_pDCRT)) && g_pDCRT) {
            g_pDCRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            g_pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &g_pBrush);

            auto makeRimBrush = [&](bool light, ID2D1LinearGradientBrush** ppOut) {
                D2D1_GRADIENT_STOP stops[5] = {
                    { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, light ? 0.56f : 0.64f) },
                    { 0.30f, D2D1::ColorF(185.0f / 255.0f, 228.0f / 255.0f, 1.00f, light ? 0.28f : 0.32f) },
                    { 0.52f, D2D1::ColorF(1.00f, 1.00f, 1.00f, light ? 0.14f : 0.16f) },
                    { 0.76f, D2D1::ColorF(1.00f, 210.0f / 255.0f, 240.0f / 255.0f, light ? 0.26f : 0.30f) },
                    { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, light ? 0.44f : 0.48f) }
                };
                ID2D1GradientStopCollection* pStops = nullptr;
                if (SUCCEEDED(g_pDCRT->CreateGradientStopCollection(stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
                    g_pDCRT->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(100, 40)),
                        pStops, ppOut);
                    pStops->Release();
                }
            };
            makeRimBrush(true, &g_pRimGradBrushLight);
            makeRimBrush(false, &g_pRimGradBrushDark);
        }
    }

    if (g_pDWrite && (g_fonts.dpi != dpi || g_fonts.scalePct != scalePct || !g_fonts.pTitle)) {
        g_fonts.Release();
        g_fonts.dpi = dpi;
        g_fonts.scalePct = scalePct;

        g_pDWrite->CreateTextFormat(
            L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(16.0f, dpi, scalePct), L"", &g_fonts.pIcon);
        g_pDWrite->CreateTextFormat(
            L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(14.5f, dpi, scalePct), L"", &g_fonts.pBadgeIcon);
        g_pDWrite->CreateTextFormat(
            L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(9.5f, dpi, scalePct), L"", &g_fonts.pChevronIcon);
        g_pDWrite->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi, scalePct), L"", &g_fonts.pBadgeCount);
        g_pDWrite->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(9.5f, dpi, scalePct), L"", &g_fonts.pBadgeVertTime);
        g_pDWrite->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi, scalePct), L"", &g_fonts.pTitle);
        g_pDWrite->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.5f, dpi, scalePct), L"", &g_fonts.pSub);
        g_pDWrite->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi, scalePct), L"", &g_fonts.pVal);
        g_pDWrite->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(11.5f, dpi, scalePct), L"", &g_fonts.pValCenter);

        for (auto* f : { g_fonts.pIcon, g_fonts.pBadgeIcon, g_fonts.pChevronIcon,
                         g_fonts.pBadgeCount, g_fonts.pBadgeVertTime, g_fonts.pValCenter }) {
            if (f) {
                f->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            }
        }
        if (g_fonts.pTitle) {
            g_fonts.pTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            g_fonts.pTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_fonts.pTitle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (g_fonts.pSub) {
            g_fonts.pSub->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_fonts.pSub->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_fonts.pSub->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            IDWriteInlineObject* pEllipsis = nullptr;
            if (SUCCEEDED(g_pDWrite->CreateEllipsisTrimmingSign(g_fonts.pSub, &pEllipsis)) && pEllipsis) {
                DWRITE_TRIMMING trim = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
                g_fonts.pSub->SetTrimming(&trim, pEllipsis);
                pEllipsis->Release();
            }
        }
        if (g_fonts.pVal) {
            g_fonts.pVal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            g_fonts.pVal->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_fonts.pVal->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
    }
    return g_pDCRT != nullptr && g_pBrush != nullptr;
}

void ReleaseRenderResources() {
    ReleaseArtworkGpuCache();
    g_popupGlassCache.ClearAll();
    g_cornerGlassCache.ClearAll();
    g_fonts.Release();
    if (g_pRimGradBrushLight) { g_pRimGradBrushLight->Release(); g_pRimGradBrushLight = nullptr; }
    if (g_pRimGradBrushDark)  { g_pRimGradBrushDark->Release();  g_pRimGradBrushDark = nullptr; }
    if (g_pBrush)             { g_pBrush->Release();             g_pBrush = nullptr; }
    if (g_pDCRT)              { g_pDCRT->Release();              g_pDCRT = nullptr; }
    if (g_pDWrite)            { g_pDWrite->Release();            g_pDWrite = nullptr; }
    if (g_pD2DFactory)        { g_pD2DFactory->Release();        g_pD2DFactory = nullptr; }
    g_alertCanvas.Release();
    g_cornerCanvas.Release();
    g_printWndCanvas.Release();
}

void DrawCleanGlassRim(const RECT& rLocal, float radiusPx, UINT dpi, bool isLight) {
    float l = static_cast<float>(rLocal.left) + 0.6f;
    float t = static_cast<float>(rLocal.top) + 0.6f;
    float r = static_cast<float>(rLocal.right) - 0.6f;
    float b = static_cast<float>(rLocal.bottom) - 0.6f;
    if (r <= l + 2.0f || b <= t + 2.0f) return;

    float rad = std::min({ radiusPx, (r - l) * 0.5f, (b - t) * 0.5f });
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(l, t, r, b), rad, rad);

    ID2D1LinearGradientBrush* pRim = isLight ? g_pRimGradBrushLight : g_pRimGradBrushDark;
    if (pRim) {
        pRim->SetStartPoint(D2D1::Point2F(l, t));
        pRim->SetEndPoint(D2D1::Point2F(r, b));
        g_pDCRT->DrawRoundedRectangle(rr, pRim, DpiScaleF(1.05f, dpi));
    }
    if (isLight) {
        g_pBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.14f));
        g_pDCRT->DrawRoundedRectangle(rr, g_pBrush, DpiScaleF(0.75f, dpi));
    }
}

static PreblurredRegion g_tbCachedRegion;
static uint64_t g_lastTbBackdropHash = 0;
static RECT     g_lastTbScreenRect = {};
static RECT     g_lastAppsRenderedLocal = {};
static RECT     g_lastTrayRenderedLocal = {};
static int      g_lastTbClarity = -999;
static ColorMode g_lastTbColorMode = ColorMode::Auto;
static RECT     g_lastCornerScreenRect = {};
static bool     g_lastCornerIsVert = false;
static UINT     g_lastCornerDpi = 96;
static float    g_lastCornerRadiusPx = 18.0f;

D2D1_COLOR_F ColorForCornerItemKind(CornerItemKind kind, bool isLight) {
    switch (kind) {
        case CornerItemKind::Media:
            return isLight ? D2D1::ColorF(0.68f, 0.14f, 0.66f, 1.0f) : D2D1::ColorF(0.94f, 0.56f, 1.00f, 1.0f);
        case CornerItemKind::TaskProgress:
            return isLight ? D2D1::ColorF(0.04f, 0.46f, 0.84f, 1.0f) : D2D1::ColorF(0.36f, 0.86f, 1.00f, 1.0f);
        case CornerItemKind::Vpn:
            return isLight ? D2D1::ColorF(0.00f, 0.48f, 0.56f, 1.0f) : D2D1::ColorF(0.24f, 0.94f, 0.88f, 1.0f);
        case CornerItemKind::Hotspot:
            return isLight ? D2D1::ColorF(0.04f, 0.50f, 0.28f, 1.0f) : D2D1::ColorF(0.26f, 0.94f, 0.60f, 1.0f);
        case CornerItemKind::Microphone:
            return isLight ? D2D1::ColorF(0.80f, 0.38f, 0.00f, 1.0f) : D2D1::ColorF(1.00f, 0.74f, 0.22f, 1.0f);
        case CornerItemKind::Camera:
            return isLight ? D2D1::ColorF(0.78f, 0.14f, 0.16f, 1.0f) : D2D1::ColorF(1.00f, 0.44f, 0.44f, 1.0f);
        case CornerItemKind::Location:
            return isLight ? D2D1::ColorF(0.02f, 0.44f, 0.80f, 1.0f) : D2D1::ColorF(0.40f, 0.82f, 1.00f, 1.0f);
        case CornerItemKind::Timer:
        case CornerItemKind::Stopwatch:
        default:
            return isLight ? D2D1::ColorF(0.06f, 0.42f, 0.82f, 1.0f) : D2D1::ColorF(0.46f, 0.86f, 1.00f, 1.0f);
    }
}

// Vẽ Viên thuốc Góc Trái/Trên (Phát nhạc Ảnh bìa + Sóng nhạc, Tiến trình 77%, Hẹn giờ/Bấm giờ, Hotspot, Quyền riêng tư + nút < >)
// Kiểm tra xem có ứng dụng/trình duyệt/game nào đang mở Toàn màn hình thực sự (Borderless Fullscreen / F11 / D3D, KHÔNG có thanh tiêu đề titlebar) trên màn hình chứa Taskbar hay không
static bool IsFullscreenWindowOnMonitor(HMONITOR hMon, const RECT& rcMon) {
    if (!hMon) return false;
    HWND hFg = GetForegroundWindow();

    // 1. Direct3D Exclusive Fullscreen thực thụ (bỏ QUNS_BUSY để không bắt nhầm cửa sổ phóng to/UltraViewer/trình phát có titlebar)
    QUERY_USER_NOTIFICATION_STATE quns = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns)) && quns == QUNS_RUNNING_D3D_FULL_SCREEN) {
        if (!hFg || MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST) == hMon) {
            return true;
        }
    }

    if (!hFg || !IsWindow(hFg) || !IsWindowVisible(hFg) || IsIconic(hFg) ||
        hFg == g_alertHwnd || hFg == g_cornerWidgetHwnd) {
        return false;
    }

    wchar_t cls[64] = {};
    GetClassNameW(hFg, cls, ARRAYSIZE(cls));
    bool isShellDesktop =
        _wcsicmp(cls, L"Progman") == 0 ||
        _wcsicmp(cls, L"WorkerW") == 0 ||
        _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(cls, L"SecondaryTrayWnd") == 0 ||
        _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
    if (isShellDesktop) return false;

    DWORD cloaked = 0;
    DwmGetWindowAttribute(hFg, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked != 0 || MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST) != hMon) {
        return false;
    }

    // 2. Loại trừ mọi cửa sổ còn thanh tiêu đề (WS_CAPTION) hoặc viền khung cửa sổ (WS_THICKFRAME) — ví dụ cửa sổ Maximized / UltraViewer chưa full view
    LONG_PTR style = GetWindowLongPtrW(hFg, GWL_STYLE);
    bool hasTitlebarStyle = ((style & WS_CAPTION) == WS_CAPTION);
    bool hasResizeFrame   = ((style & WS_THICKFRAME) != 0);
    bool isMaximized      = (IsZoomed(hFg) != FALSE);
    bool isUwpFrame       = (_wcsicmp(cls, L"ApplicationFrameWindow") == 0);

    if (isUwpFrame) {
        // Ứng dụng UWP khi phóng to bình thường (có titlebar) luôn có IsZoomed == TRUE; chỉ khi vào Fullscreen thật mới hết Zoomed
        if (isMaximized) return false;
    } else {
        // Mọi ứng dụng Win32/Electron/trình duyệt chưa vào Fullscreen thật đều giữ WS_CAPTION hoặc WS_THICKFRAME
        if (hasTitlebarStyle || hasResizeFrame) return false;
    }

    // 3. Kiểm tra khung hiển thị thực (DWMWA_EXTENDED_FRAME_BOUNDS) và vùng nội dung (ClientRect) phải phủ kín 100% toàn màn hình (không có titlebar)
    RECT rcFrame{};
    if (FAILED(DwmGetWindowAttribute(hFg, DWMWA_EXTENDED_FRAME_BOUNDS, &rcFrame, sizeof(rcFrame)))) {
        if (!GetWindowRect(hFg, &rcFrame)) return false;
    }
    bool frameCoversMon =
        (rcFrame.left <= rcMon.left + 2 && rcFrame.top <= rcMon.top + 2 &&
         rcFrame.right >= rcMon.right - 2 && rcFrame.bottom >= rcMon.bottom - 2);
    if (!frameCoversMon) return false;

    RECT rcClient{};
    if (!GetClientRect(hFg, &rcClient)) return false;
    POINT ptTL{ rcClient.left, rcClient.top };
    POINT ptBR{ rcClient.right, rcClient.bottom };
    if (!ClientToScreen(hFg, &ptTL) || !ClientToScreen(hFg, &ptBR)) return false;

    bool clientCoversMon =
        (ptTL.x <= rcMon.left + 2 && ptTL.y <= rcMon.top + 2 &&
         ptBR.x >= rcMon.right - 2 && ptBR.y >= rcMon.bottom - 2);
    return clientCoversMon;
}

void RenderCornerWidgetSurface(bool rebuildGlass) {
    if (!g_cornerWidgetHwnd || !IsWindow(g_cornerWidgetHwnd)) return;

    PersistentIndicators ind = GetPersistentIndicatorsSnapshot();
    ULONGLONG nowTick = GetTickCount64();
    auto items = BuildActiveCornerItems(ind, nowTick);
    int selIdx = ResolveSelectedCornerIndex(items);

    // Tự động quản lý các bộ đếm thông minh (0.0% CPU khi không có đồng hồ hoặc không phát nhạc!)
    if (g_alertHwnd && IsWindow(g_alertHwnd)) {
        static bool s_clockTickActive = false;
        bool wantClockTick = (selIdx >= 0 && (items[selIdx].kind == CornerItemKind::Timer || items[selIdx].kind == CornerItemKind::Stopwatch)) ||
                             ind.clock.timerRunning || ind.clock.stopwatchRunning;
        if (wantClockTick) {
            if (!s_clockTickActive) {
                SetTimer(g_alertHwnd, IDT_CORNER_CLOCK_TICK, 250, nullptr);
                s_clockTickActive = true;
            }
        } else if (s_clockTickActive) {
            KillTimer(g_alertHwnd, IDT_CORNER_CLOCK_TICK);
            s_clockTickActive = false;
        }

        bool needWaveAnim = (selIdx >= 0 && items[selIdx].kind == CornerItemKind::Media && items[selIdx].mediaPlaying &&
                             g_uiSettings.enableSplitTaskbar);
        if (needWaveAnim) {
            SetTimer(g_alertHwnd, IDT_CORNER_WAVE_TICK, 55, nullptr);
        } else {
            KillTimer(g_alertHwnd, IDT_CORNER_WAVE_TICK);
        }
    }

    auto syncCornerShiftToXaml = [](bool visible, int endDip) {
        bool prevVis = g_cornerPillVisibleAtomic.exchange(visible, std::memory_order_relaxed);
        int prevEnd = g_cornerPillEndDipAtomic.exchange(visible ? endDip : 0, std::memory_order_relaxed);
        if (prevVis != visible || (visible && std::abs(prevEnd - endDip) >= 2)) {
            PostFastSyncToTaskbar(3);
        }
    };

    if (!g_uiSettings.enableSplitTaskbar || selIdx < 0) {
        if (IsWindowVisible(g_cornerWidgetHwnd)) {
            if (pOrigShowWindow) pOrigShowWindow(g_cornerWidgetHwnd, SW_HIDE);
        }
        syncCornerShiftToXaml(false, 0);
        return;
    }

    TaskbarPillsGeometry geo{};
    if (!QueryTaskbarPillsGeometry(geo) || !geo.hasPersist) {
        if (IsWindowVisible(g_cornerWidgetHwnd)) {
            if (pOrigShowWindow) pOrigShowWindow(g_cornerWidgetHwnd, SW_HIDE);
        }
        syncCornerShiftToXaml(false, 0);
        return;
    }

    // Nếu người dùng bật Tự động ẩn Taskbar trong Windows Settings và Taskbar đang trượt ẩn khỏi màn hình -> ẩn luôn góc trái!
    HMONITOR hMon = MonitorFromWindow(geo.hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        RECT rcVis{};
        bool taskbarHidden = !IntersectRect(&rcVis, &geo.rcTaskbarScreen, &mi.rcMonitor) ||
                             (rcVis.right - rcVis.left) < 10 || (rcVis.bottom - rcVis.top) < 10;
        bool isFullscreen = IsFullscreenWindowOnMonitor(hMon, mi.rcMonitor);
        if (taskbarHidden || isFullscreen) {
            if (g_alertHwnd && IsWindow(g_alertHwnd)) {
                KillTimer(g_alertHwnd, IDT_CORNER_WAVE_TICK);
            }
            if (IsWindowVisible(g_cornerWidgetHwnd) && pOrigShowWindow) {
                pOrigShowWindow(g_cornerWidgetHwnd, SW_HIDE);
            }
            return;
        }
    }

    int cw = geo.rcPersistScreen.right - geo.rcPersistScreen.left;
    int ch = geo.rcPersistScreen.bottom - geo.rcPersistScreen.top;
    if (cw <= 12 || ch <= 12) {
        syncCornerShiftToXaml(false, 0);
        return;
    }
    int endPx = geo.isVertical ? static_cast<int>(geo.rcPersistLocal.bottom) : static_cast<int>(geo.rcPersistLocal.right);
    int endDip = static_cast<int>(std::ceil(static_cast<float>(endPx) * 96.0f / static_cast<float>(geo.dpi ? geo.dpi : 96)));
    syncCornerShiftToXaml(true, endDip);

    if (!EnsureRenderResources(geo.dpi, g_uiSettings.fontScalePercent)) return;
    if (!g_cornerCanvas.Ensure(cw, ch)) return;

    if (rebuildGlass || !g_cornerGlassCache.valid ||
        g_cornerGlassCache.w != cw || g_cornerGlassCache.h != ch ||
        !EqualRect(&geo.rcPersistScreen, &g_lastCornerScreenRect)) {
        g_lastCornerScreenRect = geo.rcPersistScreen;
        g_lastCornerIsVert = geo.isVertical;
        g_lastCornerDpi = geo.dpi;
        g_lastCornerRadiusPx = geo.cornerDiamPx * 0.5f;

        if (!g_tbCachedRegion.halfSoft.empty()) {
            g_cornerGlassCache.ReleaseGpu();
            g_cornerGlassCache.w = cw;
            g_cornerGlassCache.h = ch;
            g_cornerGlassCache.isLight = g_tbCachedRegion.isLight;
            g_cornerGlassCache.pixels.assign(static_cast<size_t>(cw) * ch, 0u);
            RECT rcFull = { 0, 0, cw, ch };
            RenderRefractedPillToBuffer(g_tbCachedRegion, geo.rcPersistScreen.left, geo.rcPersistScreen.top,
                                        g_cornerGlassCache.pixels.data(), cw, ch, rcFull,
                                        g_lastCornerRadiusPx, geo.dpi, g_uiSettings.glassClarity, true);
            g_cornerGlassCache.valid = true;
        }
    }

    bool isLight = g_cornerGlassCache.valid
        ? g_cornerGlassCache.isLight
        : (g_uiSettings.colorMode == ColorMode::Light ? true
            : (g_uiSettings.colorMode == ColorMode::Dark ? false : g_cachedSystemLightTheme));

    const auto& curItem = items[selIdx];
    bool multi = (items.size() >= 2);
    float rad = std::min({ g_lastCornerRadiusPx, cw * 0.5f, ch * 0.5f });

    RECT rcBind = { 0, 0, cw, ch };
    if (SUCCEEDED(g_pDCRT->BindDC(g_cornerCanvas.hdcMem, &rcBind))) {
        g_pDCRT->BeginDraw();
        g_pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        g_pDCRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(0.5f, 0.5f, cw - 0.5f, ch - 0.5f), rad, rad);
        if (EnsureGlassBrushFromSlotCache(g_cornerGlassCache, g_pDCRT) && g_cornerGlassCache.pBrush) {
            g_pDCRT->FillRoundedRectangle(rr, g_cornerGlassCache.pBrush);
        } else {
            g_pBrush->SetColor(isLight
                ? D2D1::ColorF(0.95f, 0.96f, 0.99f, 0.88f)
                : D2D1::ColorF(0.10f, 0.11f, 0.15f, 0.88f));
            g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
        }

        RECT rLocal = { 0, 0, cw, ch };
        DrawCleanGlassRim(rLocal, rad, geo.dpi, isLight);

        D2D1_COLOR_F colItem = ColorForCornerItemKind(curItem.kind, isLight);
        D2D1_COLOR_F colArrow = isLight
            ? D2D1::ColorF(0.10f, 0.12f, 0.18f, 0.82f)
            : D2D1::ColorF(0.94f, 0.96f, 1.00f, 0.84f);
        D2D1_COLOR_F colHoverBg = isLight
            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.09f)
            : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.13f);

        if (!geo.isVertical) {
            float btnW = multi ? DpiScaleF(20.0f, geo.dpi) : 0.0f;
            float midL = btnW;
            float midR = cw - btnW;

            if (g_cornerHoverZone == 2) {
                g_pBrush->SetColor(colHoverBg);
                D2D1_RECT_F rHov = D2D1::RectF(midL + 2.0f, 3.0f, midR - 2.0f, ch - 3.0f);
                g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rHov, DpiScaleF(8.0f, geo.dpi), DpiScaleF(8.0f, geo.dpi)), g_pBrush);
            }

            if (multi && g_fonts.pChevronIcon) {
                if (g_cornerHoverZone == 1) {
                    g_pBrush->SetColor(colHoverBg);
                    g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(2.5f, 4.0f, btnW, ch - 4.0f), 8.0f, 8.0f), g_pBrush);
                }
                if (g_cornerHoverZone == 3) {
                    g_pBrush->SetColor(colHoverBg);
                    g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cw - btnW, 4.0f, cw - 2.5f, ch - 4.0f), 8.0f, 8.0f), g_pBrush);
                }
                g_pBrush->SetColor(colArrow);
                g_pDCRT->DrawText(L"\uE76B", 1, g_fonts.pChevronIcon, D2D1::RectF(1.0f, 0.0f, btnW + 1.0f, static_cast<float>(ch)), g_pBrush);
                g_pDCRT->DrawText(L"\uE76C", 1, g_fonts.pChevronIcon, D2D1::RectF(cw - btnW - 1.0f, 0.0f, static_cast<float>(cw) - 1.0f, static_cast<float>(ch)), g_pBrush);
            }

            if (curItem.kind == CornerItemKind::Media) {
                // Phát nhạc: Ảnh bìa bài hát bo góc + Sóng nhạc 4 cột động!
                float artEdge = std::min(static_cast<float>(ch) - DpiScaleF(12.0f, geo.dpi), DpiScaleF(24.0f, geo.dpi));
                float waveW   = DpiScaleF(18.0f, geo.dpi);
                float gapIn   = DpiScaleF(6.0f, geo.dpi);
                float groupW  = artEdge + gapIn + waveW;
                float startX  = midL + ((midR - midL) - groupW) * 0.5f;
                float artTop  = (ch - artEdge) * 0.5f;

                D2D1_RECT_F rArt = D2D1::RectF(startX, artTop, startX + artEdge, artTop + artEdge);
                if (curItem.artwork && !curItem.artwork->pixels.empty()) {
                    DrawRoundedArtworkBox(rArt, DpiScaleF(5.5f, geo.dpi), curItem.artwork, curItem.artworkId, isLight);
                } else if (g_fonts.pBadgeIcon) {
                    g_pBrush->SetColor(colItem);
                    g_pDCRT->DrawText(L"\uEC4F", 1, g_fonts.pBadgeIcon, rArt, g_pBrush);
                }

                float waveMaxH = DpiScaleF(16.0f, geo.dpi);
                D2D1_RECT_F rWave = D2D1::RectF(rArt.right + gapIn, (ch - waveMaxH) * 0.5f,
                                                rArt.right + gapIn + waveW, (ch + waveMaxH) * 0.5f);
                DrawMusicWaveformBars(rWave, 4, curItem.mediaPlaying, nowTick, colItem, geo.dpi);
            } else {
                g_pBrush->SetColor(colItem);
                if (!curItem.badgeText.empty() && g_fonts.pBadgeIcon && g_fonts.pBadgeCount) {
                    float iconSlotW = DpiScaleF(20.0f, geo.dpi);
                    float totalContentW = (midR - midL);
                    float padIn = DpiScaleF(4.0f, geo.dpi);
                    D2D1_RECT_F rIc = D2D1::RectF(midL + padIn, 0.0f, midL + padIn + iconSlotW, static_cast<float>(ch));
                    g_pDCRT->DrawText(curItem.glyph, 1, g_fonts.pBadgeIcon, rIc, g_pBrush);

                    D2D1_RECT_F rTxt = D2D1::RectF(midL + padIn + iconSlotW - DpiScaleF(1.0f, geo.dpi), 0.0f,
                                                   midL + totalContentW - padIn, static_cast<float>(ch));
                    g_pDCRT->DrawText(curItem.badgeText.c_str(), static_cast<UINT32>(curItem.badgeText.size()),
                                      g_fonts.pBadgeCount, rTxt, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                } else if (g_fonts.pBadgeIcon) {
                    D2D1_RECT_F rIc = D2D1::RectF(midL, 0.0f, midR, static_cast<float>(ch));
                    g_pDCRT->DrawText(curItem.glyph, 1, g_fonts.pBadgeIcon, rIc, g_pBrush);
                }
            }
        } else {
            // Bố cục dọc cho Taskbar Trái / Phải
            float arrowsH = multi ? DpiScaleF(18.0f, geo.dpi) : 0.0f;
            float contentBot = ch - arrowsH;

            if (g_cornerHoverZone == 2) {
                g_pBrush->SetColor(colHoverBg);
                g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(3.0f, 3.0f, cw - 3.0f, contentBot - 2.0f), 8.0f, 8.0f), g_pBrush);
            }

            if (curItem.kind == CornerItemKind::Media) {
                float artEdge = std::min(static_cast<float>(cw) - DpiScaleF(12.0f, geo.dpi), DpiScaleF(22.0f, geo.dpi));
                float artLeft = (cw - artEdge) * 0.5f;
                float artTop  = DpiScaleF(5.0f, geo.dpi);
                D2D1_RECT_F rArt = D2D1::RectF(artLeft, artTop, artLeft + artEdge, artTop + artEdge);
                if (curItem.artwork && !curItem.artwork->pixels.empty()) {
                    DrawRoundedArtworkBox(rArt, DpiScaleF(5.0f, geo.dpi), curItem.artwork, curItem.artworkId, isLight);
                } else if (g_fonts.pBadgeIcon) {
                    g_pBrush->SetColor(colItem);
                    g_pDCRT->DrawText(L"\uEC4F", 1, g_fonts.pBadgeIcon, rArt, g_pBrush);
                }
                D2D1_RECT_F rWave = D2D1::RectF(DpiScaleF(6.0f, geo.dpi), rArt.bottom + DpiScaleF(3.0f, geo.dpi),
                                                cw - DpiScaleF(6.0f, geo.dpi), contentBot - DpiScaleF(3.0f, geo.dpi));
                DrawMusicWaveformBars(rWave, 4, curItem.mediaPlaying, nowTick, colItem, geo.dpi);
            } else {
                g_pBrush->SetColor(colItem);
                if (curItem.badgeText.size() >= 3 && g_fonts.pBadgeIcon && g_fonts.pBadgeVertTime) {
                    float splitY = contentBot * 0.54f;
                    g_pDCRT->DrawText(curItem.glyph, 1, g_fonts.pBadgeIcon,
                                      D2D1::RectF(0.0f, DpiScaleF(2.0f, geo.dpi), static_cast<float>(cw), splitY), g_pBrush);
                    g_pDCRT->DrawText(curItem.badgeText.c_str(), static_cast<UINT32>(curItem.badgeText.size()),
                                      g_fonts.pBadgeVertTime,
                                      D2D1::RectF(1.0f, splitY - DpiScaleF(2.0f, geo.dpi), static_cast<float>(cw) - 1.0f, contentBot),
                                      g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                } else if (!curItem.badgeText.empty() && g_fonts.pBadgeIcon && g_fonts.pBadgeCount) {
                    float midX = cw * 0.52f;
                    g_pDCRT->DrawText(curItem.glyph, 1, g_fonts.pBadgeIcon,
                                      D2D1::RectF(1.0f, 0.0f, midX + 2.0f, contentBot), g_pBrush);
                    g_pDCRT->DrawText(curItem.badgeText.c_str(), static_cast<UINT32>(curItem.badgeText.size()),
                                      g_fonts.pBadgeCount, D2D1::RectF(midX, 0.0f, static_cast<float>(cw) - 2.0f, contentBot), g_pBrush);
                } else if (g_fonts.pBadgeIcon) {
                    g_pDCRT->DrawText(curItem.glyph, 1, g_fonts.pBadgeIcon,
                                      D2D1::RectF(0.0f, 0.0f, static_cast<float>(cw), contentBot), g_pBrush);
                }
            }

            if (multi && g_fonts.pChevronIcon) {
                float halfW = cw * 0.5f;
                if (g_cornerHoverZone == 1) {
                    g_pBrush->SetColor(colHoverBg);
                    g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(2.0f, contentBot, halfW, ch - 2.0f), 6.0f, 6.0f), g_pBrush);
                }
                if (g_cornerHoverZone == 3) {
                    g_pBrush->SetColor(colHoverBg);
                    g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(halfW, contentBot, cw - 2.0f, ch - 2.0f), 6.0f, 6.0f), g_pBrush);
                }
                g_pBrush->SetColor(colArrow);
                g_pDCRT->DrawText(L"\uE76B", 1, g_fonts.pChevronIcon, D2D1::RectF(0.0f, contentBot, halfW, static_cast<float>(ch)), g_pBrush);
                g_pDCRT->DrawText(L"\uE76C", 1, g_fonts.pChevronIcon, D2D1::RectF(halfW, contentBot, static_cast<float>(cw), static_cast<float>(ch)), g_pBrush);
            }
        }

        HRESULT hr = g_pDCRT->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            ReleaseRenderResources();
            return;
        }
        if (SUCCEEDED(hr)) {
            HDC hdcScr = GetDC(nullptr);
            POINT ptSrc = { 0, 0 };
            POINT ptDst = { geo.rcPersistScreen.left, geo.rcPersistScreen.top };
            SIZE  szDst = { cw, ch };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            UpdateLayeredWindow(g_cornerWidgetHwnd, hdcScr, &ptDst, &szDst, g_cornerCanvas.hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
            ReleaseDC(nullptr, hdcScr);

            if (pOrigSetWindowPos) {
                pOrigSetWindowPos(g_cornerWidgetHwnd, HWND_TOPMOST,
                                  geo.rcPersistScreen.left, geo.rcPersistScreen.top, cw, ch,
                                  SWP_NOACTIVATE | SWP_SHOWWINDOW);
            }
        }
    }
}

// Tạo các mặt kính khúc xạ và đẩy trực tiếp vào cây XAML của Shell_TrayWnd (Không dùng cửa sổ nền ngoài!)
void RefreshTaskbarGlassSurfaces(bool forceBackdropRecapture = false) {
    if (!g_uiSettings.enableSplitTaskbar) {
        if (g_cornerWidgetHwnd && IsWindowVisible(g_cornerWidgetHwnd) && pOrigShowWindow) {
            pOrigShowWindow(g_cornerWidgetHwnd, SW_HIDE);
        }
        return;
    }

    TaskbarPillsGeometry geo{};
    if (!QueryTaskbarPillsGeometry(geo)) return;

    int tbW = geo.rcTaskbarScreen.right - geo.rcTaskbarScreen.left;
    int tbH = geo.rcTaskbarScreen.bottom - geo.rcTaskbarScreen.top;
    if (tbW <= 20 || tbH <= 20) return;

    if (!EnsureRenderResources(geo.dpi, g_uiSettings.fontScalePercent)) return;

    HMONITOR hMon = MonitorFromWindow(geo.hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        EnsureMonitorWallpaperLoaded(mi.rcMonitor);
    }

    uint64_t curHash = ComputeTaskbarBackdropSignature(geo.rcTaskbarScreen);
    bool needRecapture = forceBackdropRecapture ||
                         g_tbCachedRegion.halfSoft.empty() ||
                         curHash != g_lastTbBackdropHash ||
                         !EqualRect(&geo.rcTaskbarScreen, &g_lastTbScreenRect) ||
                         g_uiSettings.glassClarity != g_lastTbClarity ||
                         g_uiSettings.colorMode != g_lastTbColorMode;

    if (needRecapture) {
        int pad = std::max(20, DpiScale(24, geo.dpi));
        if (CaptureCleanBackdropBehindTaskbar(geo.rcTaskbarScreen, pad, geo.dpi, g_uiSettings, g_tbCachedRegion)) {
            g_lastTbBackdropHash = curHash;
            g_lastTbScreenRect = geo.rcTaskbarScreen;
            g_lastTbClarity = g_uiSettings.glassClarity;
            g_lastTbColorMode = g_uiSettings.colorMode;
        }
    }

    bool boundsChanged = !EqualRect(&geo.rcAppsLocal, &g_lastAppsRenderedLocal) ||
                         !EqualRect(&geo.rcTrayLocal, &g_lastTrayRenderedLocal);

    if ((needRecapture || boundsChanged || g_sharedXamlGlassPixels.revision == 0) &&
        !g_tbCachedRegion.halfSoft.empty()) {
        g_lastAppsRenderedLocal = geo.rcAppsLocal;
        g_lastTrayRenderedLocal = geo.rcTrayLocal;

        float cornerR = geo.cornerDiamPx * 0.5f;
        int aw = std::max<int>(8, geo.rcAppsLocal.right - geo.rcAppsLocal.left);
        int ah = std::max<int>(8, geo.rcAppsLocal.bottom - geo.rcAppsLocal.top);
        std::vector<DWORD> appsPx(static_cast<size_t>(aw) * ah, 0u);
        RECT rcAppsZero = { 0, 0, aw, ah };
        RenderRefractedPillToBuffer(g_tbCachedRegion,
                                    geo.rcTaskbarScreen.left + geo.rcAppsLocal.left,
                                    geo.rcTaskbarScreen.top  + geo.rcAppsLocal.top,
                                    appsPx.data(), aw, ah, rcAppsZero, cornerR,
                                    geo.dpi, g_uiSettings.glassClarity, false);

        int tw = std::max<int>(8, geo.hasTray ? (geo.rcTrayLocal.right - geo.rcTrayLocal.left) : 8);
        int th = std::max<int>(8, geo.hasTray ? (geo.rcTrayLocal.bottom - geo.rcTrayLocal.top) : 8);
        std::vector<DWORD> trayPx(static_cast<size_t>(tw) * th, 0u);
        if (geo.hasTray) {
            RECT rcTrayZero = { 0, 0, tw, th };
            RenderRefractedPillToBuffer(g_tbCachedRegion,
                                        geo.rcTaskbarScreen.left + geo.rcTrayLocal.left,
                                        geo.rcTaskbarScreen.top  + geo.rcTrayLocal.top,
                                        trayPx.data(), tw, th, rcTrayZero, cornerR,
                                        geo.dpi, g_uiSettings.glassClarity, false);
        }

        {
            std::lock_guard<std::mutex> lock(g_xamlGlassPixelsMutex);
            ++g_sharedXamlGlassPixels.revision;
            g_sharedXamlGlassPixels.isLight = g_tbCachedRegion.isLight;
            g_sharedXamlGlassPixels.appsW = aw;
            g_sharedXamlGlassPixels.appsH = ah;
            g_sharedXamlGlassPixels.appsPixels = std::move(appsPx);
            g_sharedXamlGlassPixels.trayW = tw;
            g_sharedXamlGlassPixels.trayH = th;
            g_sharedXamlGlassPixels.trayPixels = std::move(trayPx);
        }

        if (g_xamlGlassInjected.load(std::memory_order_relaxed)) {
            PostFastSyncToTaskbar(2);
        } else {
            PostFastSyncToTaskbar(3);
        }
    }

    // Cập nhật Viên thuốc Góc Trái/Trên tương tác chuột
    RenderCornerWidgetSurface(needRecapture);
}

// ---------------------------------------------------------------------------
// 10. Điều khiển Phần cứng Âm lượng & Độ sáng Trực tiếp Bằng Chuột (Click / Kéo / Lăn chuột)
// ---------------------------------------------------------------------------
static IMMDeviceEnumerator* g_pAudioEnum = nullptr;
static IMMDevice* g_pAudioDev = nullptr;
static IAudioEndpointVolume* g_pEndVol = nullptr;

bool QuerySystemBrightnessPercent(int& outPct) {
    HANDLE hLcd = CreateFileW(L"\\\\.\\LCD", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (hLcd != INVALID_HANDLE_VALUE) {
        WhDisplayBrightnessPacket pkt{};
        DWORD bytesRet = 0;
        BOOL ok = DeviceIoControl(hLcd, IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS,
                                  nullptr, 0, &pkt, sizeof(pkt), &bytesRet, nullptr);
        CloseHandle(hLcd);
        if (ok && bytesRet >= sizeof(pkt)) {
            int val = (pkt.ucDisplayPolicy == 2) ? pkt.ucDCBrightness : pkt.ucACBrightness;
            outPct = std::clamp(val, 0, 100);
            return true;
        }
    }
    return false;
}

bool SetSystemBrightnessPercent(int pct) {
    pct = std::clamp(pct, 0, 100);
    HANDLE hLcd = CreateFileW(L"\\\\.\\LCD", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (hLcd != INVALID_HANDLE_VALUE) {
        WhDisplayBrightnessPacket pkt{};
        pkt.ucDisplayPolicy = 3;
        pkt.ucACBrightness = static_cast<UCHAR>(pct);
        pkt.ucDCBrightness = static_cast<UCHAR>(pct);
        DWORD bytesRet = 0;
        BOOL ok = DeviceIoControl(hLcd, IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS,
                                  &pkt, sizeof(pkt), nullptr, 0, &bytesRet, nullptr);
        CloseHandle(hLcd);
        if (ok) return true;
    }

    static HMODULE s_hDxva2 = LoadLibraryExW(L"dxva2.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (s_hDxva2) {
        #pragma pack(push, 1)
        struct PhysMon { HANDLE hPhysicalMonitor; WCHAR szDesc[128]; };
        #pragma pack(pop)
        using GetNumFn = BOOL(WINAPI*)(HMONITOR, LPDWORD);
        using GetMonFn = BOOL(WINAPI*)(HMONITOR, DWORD, PhysMon*);
        using GetBriFn = BOOL(WINAPI*)(HANDLE, LPDWORD, LPDWORD, LPDWORD);
        using SetBriFn = BOOL(WINAPI*)(HANDLE, DWORD);
        using DestroyFn = BOOL(WINAPI*)(DWORD, PhysMon*);

        static auto fnNum = reinterpret_cast<GetNumFn>(GetProcAddress(s_hDxva2, "GetNumberOfPhysicalMonitorsFromHMONITOR"));
        static auto fnGet = reinterpret_cast<GetMonFn>(GetProcAddress(s_hDxva2, "GetPhysicalMonitorsFromHMONITOR"));
        static auto fnBri = reinterpret_cast<GetBriFn>(GetProcAddress(s_hDxva2, "GetMonitorBrightness"));
        static auto fnSet = reinterpret_cast<SetBriFn>(GetProcAddress(s_hDxva2, "SetMonitorBrightness"));
        static auto fnDes = reinterpret_cast<DestroyFn>(GetProcAddress(s_hDxva2, "DestroyPhysicalMonitors"));

        if (fnNum && fnGet && fnBri && fnSet && fnDes) {
            HMONITOR hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
            DWORD count = 0;
            if (hMon && fnNum(hMon, &count) && count > 0 && count <= 8) {
                std::vector<PhysMon> mons(count);
                if (fnGet(hMon, count, mons.data())) {
                    bool anySet = false;
                    for (DWORD i = 0; i < count; ++i) {
                        DWORD minB = 0, curB = 0, maxB = 100;
                        if (fnBri(mons[i].hPhysicalMonitor, &minB, &curB, &maxB) && maxB > minB) {
                            DWORD target = minB + MulDiv(pct, static_cast<int>(maxB - minB), 100);
                            if (fnSet(mons[i].hPhysicalMonitor, target)) anySet = true;
                        }
                    }
                    fnDes(count, mons.data());
                    if (anySet) return true;
                }
            }
        }
    }
    return false;
}

bool QuerySystemVolumeState(int& outPct, bool& outMuted) {
    if (!g_pEndVol) return false;
    float vol = 0.0f;
    BOOL muted = FALSE;
    if (SUCCEEDED(g_pEndVol->GetMasterVolumeLevelScalar(&vol)) &&
        SUCCEEDED(g_pEndVol->GetMute(&muted))) {
        outPct = std::clamp(static_cast<int>(std::lround(vol * 100.0f)), 0, 100);
        outMuted = (muted != FALSE);
        return true;
    }
    return false;
}

bool SetSystemVolumePercentFromUi(int pct, bool& outMuted) {
    if (!g_pEndVol) return false;
    pct = std::clamp(pct, 0, 100);
    g_pEndVol->SetMasterVolumeLevelScalar(pct / 100.0f, &kModVolumeSetContextGuid);
    if (pct > 0) {
        g_pEndVol->SetMute(FALSE, &kModVolumeSetContextGuid);
        outMuted = false;
    } else {
        BOOL m = FALSE;
        g_pEndVol->GetMute(&m);
        outMuted = (m != FALSE);
    }
    return true;
}

bool ToggleSystemMuteFromUi(int& outPct, bool& outMuted) {
    if (!g_pEndVol) return false;
    BOOL curMute = FALSE;
    float curVol = 0.0f;
    if (FAILED(g_pEndVol->GetMute(&curMute)) || FAILED(g_pEndVol->GetMasterVolumeLevelScalar(&curVol))) {
        return false;
    }
    BOOL nextMute = !curMute;
    g_pEndVol->SetMute(nextMute, &kModVolumeSetContextGuid);
    outPct = std::clamp(static_cast<int>(std::lround(curVol * 100.0f)), 0, 100);
    outMuted = (nextMute != FALSE);
    return true;
}

// ---------------------------------------------------------------------------
// 11. Popup Live Alerts 3 Giây Tức Thì (Rộng mặc định 600px, Đo động chữ không cụt, Sóng nhạc & Ảnh bìa)
// ---------------------------------------------------------------------------
enum class AlertKind {
    None = 0,
    Volume,
    Brightness,
    Charging,
    Discharging,
    LowBattery,
    BatterySaver,
    Network,
    VpnAlert,
    Bluetooth,
    UsbDevice,
    CapsLock,
    NumLock,
    ScrollLock,
    InsertKey,
    AdminAppOpened,
    AppClosed,
    ClipboardCopy,
    ClipboardCut,
    ClipboardPaste,
    FileRecycled,
    FileDeleted,
    RamWarning,
    RomWarning,
    DoNotDisturb,
    FocusMode,
    PrivacyAlert,
    ClockAlert,
    MediaAlert,
    TaskProgressAlert
};

struct AlertState {
    AlertKind kind = AlertKind::None;
    std::wstring icon;
    std::wstring title;
    std::wstring subtitle;
    std::wstring valueText;
    int percentage = -1;
    bool isWarning = false;
    bool isSuccess = false;
    bool mediaPlaying = false;
    uint64_t artworkId = 0;
    std::shared_ptr<DecodedBgraImage> artwork;
    ULONGLONG expireTick = 0;
    float smoothPct = 0.0f;
};

static std::mutex g_alertMutex;
static AlertState g_alert;
static float      g_animProgress = 0.0f;
static bool       g_animRunning = false;
static ULONGLONG  g_lastAnimFrameTick = 0;

static RECT  g_activeSlotRect = {};
static float g_activeCornerRadPx = 18.0f;
static UINT  g_activeDpi = 96;
static bool  g_activeIsVertical = false;
static bool  g_activeSlotIsFloatingBottom = false;

static bool       g_sliderDragging = false;
static bool       g_sliderHovered = false;
static bool       g_iconHovered = false;
static D2D1_RECT_F g_lastSliderTrackRect = {};
static D2D1_RECT_F g_lastIconHitRect = {};
static ULONGLONG  g_alertShownTick = 0;
static POINT      g_alertShownCursorPos = {};

static bool IsQuickActionAlertKind(AlertKind kind) {
    return kind == AlertKind::CapsLock ||
           kind == AlertKind::NumLock ||
           kind == AlertKind::ScrollLock ||
           kind == AlertKind::InsertKey ||
           kind == AlertKind::ClipboardCopy ||
           kind == AlertKind::ClipboardCut ||
           kind == AlertKind::ClipboardPaste;
}

void StartAnimLoop(HWND hwnd, UINT intervalMs = 12) {
    if (hwnd && IsWindow(hwnd)) {
        if (!g_animRunning) {
            g_lastAnimFrameTick = GetTickCount64();
        }
        SetTimer(hwnd, IDT_ANIM_FRAME, intervalMs, nullptr);
        g_animRunning = true;
    }
}

void StopAnimLoop(HWND hwnd) {
    if (g_animRunning && hwnd && IsWindow(hwnd)) {
        KillTimer(hwnd, IDT_ANIM_FRAME);
        g_animRunning = false;
    }
}

void TriggerLiveAlert(AlertKind kind, const std::wstring& icon, const std::wstring& title,
                      const std::wstring& subtitle, const std::wstring& valueText,
                      int percentage = -1, bool isWarning = false, bool isSuccess = false,
                      const std::shared_ptr<DecodedBgraImage>& artwork = nullptr,
                      uint64_t artworkId = 0, bool mediaPlaying = false) {
    if (!g_running.load(std::memory_order_relaxed)) return;

    int stdDur = g_alertDurationMsAtomic.load(std::memory_order_relaxed);
    int quickDur = g_quickAlertDurationMsAtomic.load(std::memory_order_relaxed);
    int durationMs = IsQuickActionAlertKind(kind) ? std::min(stdDur, quickDur) : stdDur;
    ULONGLONG now = GetTickCount64();
    if (kind == AlertKind::Volume || kind == AlertKind::Brightness) {
        g_lastVolOrBriAlertTick.store(now, std::memory_order_relaxed);
    }

    {
        std::lock_guard<std::mutex> lock(g_alertMutex);
        bool sameActive = (g_alert.kind == kind && now < g_alert.expireTick);
        if (!sameActive) {
            g_alert.smoothPct = (percentage >= 0) ? static_cast<float>(percentage) : 100.0f;
        }
        g_alert.kind = kind;
        g_alert.icon = icon;
        g_alert.title = title;
        g_alert.subtitle = subtitle;
        g_alert.valueText = valueText;
        g_alert.percentage = percentage;
        g_alert.isWarning = isWarning;
        g_alert.isSuccess = isSuccess;
        g_alert.mediaPlaying = mediaPlaying;
        g_alert.artworkId = artworkId;
        g_alert.artwork = artwork;
        g_alert.expireTick = now + static_cast<ULONGLONG>(durationMs);
    }

    if (g_alertHwnd && IsWindow(g_alertHwnd)) {
        PostMessageW(g_alertHwnd, WM_APP_SHOW_ALERT, static_cast<WPARAM>(kind), static_cast<LPARAM>(durationMs));
    }
}

void ShowCurrentVolumeAlert(int pct, bool muted) {
    g_lastVolumeAlertTick.store(GetTickCount64(), std::memory_order_relaxed);
    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
    std::wstring icon = (muted || pct == 0)
        ? L"\uE74F"
        : ((pct < 33) ? L"\uE993" : ((pct < 66) ? L"\uE994" : L"\uE995"));
    std::wstring title = muted
        ? PickText(vi, L"Âm lượng (Tắt)", L"Volume (Muted)")
        : PickText(vi, L"Âm lượng", L"Volume");
    TriggerLiveAlert(AlertKind::Volume, icon, title, L"", std::to_wstring(pct) + L"%", pct, muted, false);
}

void ShowCurrentBrightnessAlert(int bri) {
    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
    bri = std::clamp(bri, 0, 100);
    TriggerLiveAlert(AlertKind::Brightness, L"\uE706",
                     PickText(vi, L"Độ sáng", L"Brightness"),
                     L"", std::to_wstring(bri) + L"%", bri, false, false);
}

void PrepareAlertSlotAndLiquidGlass() {
    AlertKind curKind = AlertKind::None;
    {
        std::lock_guard<std::mutex> lock(g_alertMutex);
        curKind = g_alert.kind;
    }
    bool isSliderAlert = (curKind == AlertKind::Volume || curKind == AlertKind::Brightness);

    TaskbarPillsGeometry geo{};
    if (QueryTaskbarPillsGeometry(geo)) {
        if (geo.isVertical && !isSliderAlert) {
            // Khi Taskbar nằm cạnh Trái / Phải: đưa mọi thông báo (trừ thanh trượt Âm lượng / Độ sáng) xuống giữa đáy màn hình nằm ngang cho dễ đọc!
            HMONITOR hMon = MonitorFromWindow(geo.hTaskbar, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
                hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
                GetMonitorInfoW(hMon, &mi);
            }
            int workL = mi.rcWork.left;
            int workR = mi.rcWork.right;
            int workW = std::max(DpiScale(280, geo.dpi), workR - workL);
            int targetW = std::min(workW - DpiScale(32, geo.dpi), DpiScale(g_uiSettings.fixedPopupWidthDip, geo.dpi));
            int targetH = DpiScale(44, geo.dpi);
            int slotL = workL + (workW - targetW) / 2;
            int slotB = mi.rcMonitor.bottom - DpiScale(12, geo.dpi);

            g_activeSlotRect = { slotL, slotB - targetH, slotL + targetW, slotB };
            g_activeCornerRadPx = std::min(
                DpiScaleF(static_cast<float>(g_uiSettings.pillCornerRadiusDip), geo.dpi),
                targetH * 0.5f);
            g_activeDpi = geo.dpi;
            g_activeIsVertical = false;
            g_activeSlotIsFloatingBottom = true;

            // Popup nằm ngang tách riêng ở đáy màn hình -> không cần ẩn Thanh ứng dụng bên cạnh dọc!
            SetAppsPillHiddenForPopup(false);
        } else if (g_uiSettings.popupPosition == PopupPosition::AdjacentTaskbar) {
            // Chế độ nổi sát mép Taskbar (cách 8 DIP): giữ nguyên Thanh ứng dụng luôn hiển thị và bấm được
            HMONITOR hMon = MonitorFromWindow(geo.hTaskbar, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
                hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
                GetMonitorInfoW(hMon, &mi);
            }
            RECT rcAdj = geo.rcPopupSlotScreen;
            int gapAdj = DpiScale(8, geo.dpi);
            if (!geo.isVertical) {
                int slotH = rcAdj.bottom - rcAdj.top;
                bool isTopEdge = (geo.rcTaskbarScreen.top < (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2);
                if (isTopEdge) {
                    rcAdj.top = geo.rcTaskbarScreen.bottom + gapAdj;
                    rcAdj.bottom = rcAdj.top + slotH;
                } else {
                    rcAdj.bottom = geo.rcTaskbarScreen.top - gapAdj;
                    rcAdj.top = rcAdj.bottom - slotH;
                }
            } else {
                int slotW = rcAdj.right - rcAdj.left;
                bool isLeftEdge = (geo.rcTaskbarScreen.left < (mi.rcMonitor.left + mi.rcMonitor.right) / 2);
                if (isLeftEdge) {
                    rcAdj.left = geo.rcTaskbarScreen.right + gapAdj;
                    rcAdj.right = rcAdj.left + slotW;
                } else {
                    rcAdj.right = geo.rcTaskbarScreen.left - gapAdj;
                    rcAdj.left = rcAdj.right - slotW;
                }
            }

            g_activeSlotRect = rcAdj;
            g_activeCornerRadPx = geo.cornerDiamPx * 0.5f;
            g_activeDpi = geo.dpi;
            g_activeIsVertical = geo.isVertical;
            g_activeSlotIsFloatingBottom = true;

            SetAppsPillHiddenForPopup(false);
        } else {
            g_activeSlotRect = geo.rcPopupSlotScreen;
            g_activeCornerRadPx = geo.cornerDiamPx * 0.5f;
            g_activeDpi = geo.dpi;
            g_activeIsVertical = geo.isVertical;
            g_activeSlotIsFloatingBottom = false;

            // Ẩn sạch riêng Thanh ứng dụng ở giữa (cả mặt kính XAML lẫn icon), giữ nguyên Khay trạng thái!
            SetAppsPillHiddenForPopup(true);
        }
    } else {
        HMONITOR hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfoW(hMon, &mi);
        UINT dpi = g_alertHwnd ? GetDpiForWindow(g_alertHwnd) : 96;
        if (dpi == 0) dpi = 96;
        int w = DpiScale(g_uiSettings.fixedPopupWidthDip, dpi);
        int h = DpiScale(44, dpi);
        int x = mi.rcMonitor.left + ((mi.rcMonitor.right - mi.rcMonitor.left) - w) / 2;
        int y = mi.rcMonitor.bottom - h - DpiScale(12, dpi);
        g_activeSlotRect = { x, y, x + w, y + h };
        g_activeCornerRadPx = std::min(
            DpiScaleF(static_cast<float>(g_uiSettings.pillCornerRadiusDip), dpi),
            h * 0.5f);
        g_activeDpi = dpi;
        g_activeIsVertical = false;
        g_activeSlotIsFloatingBottom = true;
        SetAppsPillHiddenForPopup(false);
    }

    int sw = g_activeSlotRect.right - g_activeSlotRect.left;
    int sh = g_activeSlotRect.bottom - g_activeSlotRect.top;
    if (sw <= 16 || sh <= 16) return;

    bool canReuseTbRegion =
        !g_tbCachedRegion.halfSoft.empty() &&
        g_activeSlotRect.left   >= g_tbCachedRegion.capX &&
        g_activeSlotRect.top    >= g_tbCachedRegion.capY &&
        g_activeSlotRect.right  <= (g_tbCachedRegion.capX + g_tbCachedRegion.capW) &&
        g_activeSlotRect.bottom <= (g_tbCachedRegion.capY + g_tbCachedRegion.capH);

    if (canReuseTbRegion) {
        g_popupGlassCache.ReleaseGpu();
        g_popupGlassCache.w = sw;
        g_popupGlassCache.h = sh;
        g_popupGlassCache.isLight = g_tbCachedRegion.isLight;
        g_popupGlassCache.pixels.assign(static_cast<size_t>(sw) * sh, 0u);
        RECT rcFull = { 0, 0, sw, sh };
        RenderRefractedPillToBuffer(g_tbCachedRegion, g_activeSlotRect.left, g_activeSlotRect.top,
                                    g_popupGlassCache.pixels.data(), sw, sh, rcFull,
                                    g_activeCornerRadPx, g_activeDpi, g_uiSettings.glassClarity, false);
        g_popupGlassCache.valid = true;
        return;
    }

    PreblurredRegion slotReg;
    int pad = std::max(20, DpiScale(24, g_activeDpi));
    if (CaptureCleanBackdropBehindTaskbar(g_activeSlotRect, pad, g_activeDpi, g_uiSettings, slotReg)) {
        g_popupGlassCache.ReleaseGpu();
        g_popupGlassCache.w = sw;
        g_popupGlassCache.h = sh;
        g_popupGlassCache.isLight = slotReg.isLight;
        g_popupGlassCache.pixels.assign(static_cast<size_t>(sw) * sh, 0u);
        RECT rcFull = { 0, 0, sw, sh };
        RenderRefractedPillToBuffer(slotReg, g_activeSlotRect.left, g_activeSlotRect.top,
                                    g_popupGlassCache.pixels.data(), sw, sh, rcFull,
                                    g_activeCornerRadPx, g_activeDpi, g_uiSettings.glassClarity, false);
        g_popupGlassCache.valid = true;
    }
}

// Vẽ biểu tượng Pin chuẩn Windows 11 25H2+ (thân bo tròn tỉ lệ gốc, thanh đầy theo %, tia sét nằm giữa thân pin khi sạc)
void DrawWin11Battery25H2Icon(const D2D1_RECT_F& rBox, int percentage, bool isCharging, bool isSaver, bool isLow,
                              bool isLight, float alpha, UINT dpi) {
    if (!g_pDCRT || !g_pBrush || !g_pD2DFactory) return;

    float cx = (rBox.left + rBox.right) * 0.5f;
    float cy = (rBox.top + rBox.bottom) * 0.5f;

    float bodyW   = DpiScaleF(19.0f, dpi);
    float bodyH   = DpiScaleF(11.0f, dpi);
    float bodyR   = DpiScaleF(2.5f, dpi);
    float strokeW = DpiScaleF(1.35f, dpi);
    float nubW    = DpiScaleF(1.6f, dpi);
    float nubH    = DpiScaleF(4.4f, dpi);
    float nubGap  = DpiScaleF(0.9f, dpi);

    float totalW = bodyW + nubGap + nubW;
    float left   = cx - totalW * 0.5f;
    float top    = cy - bodyH * 0.5f;

    D2D1_COLOR_F colOutline = isLight
        ? D2D1::ColorF(0.12f, 0.14f, 0.18f, 0.92f * alpha)
        : D2D1::ColorF(0.95f, 0.97f, 1.00f, 0.94f * alpha);

    D2D1_COLOR_F colFill = colOutline;
    if (isCharging) {
        colFill = isLight
            ? D2D1::ColorF(0.08f, 0.62f, 0.30f, alpha)
            : D2D1::ColorF(0.42f, 0.89f, 0.54f, alpha);
    } else if (isSaver) {
        colFill = isLight
            ? D2D1::ColorF(0.84f, 0.58f, 0.02f, alpha)
            : D2D1::ColorF(0.98f, 0.78f, 0.24f, alpha);
    } else if (isLow) {
        colFill = isLight
            ? D2D1::ColorF(0.84f, 0.16f, 0.16f, alpha)
            : D2D1::ColorF(1.00f, 0.38f, 0.36f, alpha);
    }

    // 1. Vỏ pin bo tròn chuẩn 25H2
    D2D1_RECT_F rBody = D2D1::RectF(left, top, left + bodyW, top + bodyH);
    g_pBrush->SetColor(colOutline);
    g_pDCRT->DrawRoundedRectangle(D2D1::RoundedRect(rBody, bodyR, bodyR), g_pBrush, strokeW);

    // 2. Cực dương bên phải
    D2D1_RECT_F rNub = D2D1::RectF(
        rBody.right + nubGap, cy - nubH * 0.5f,
        rBody.right + nubGap + nubW, cy + nubH * 0.5f);
    g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rNub, nubW * 0.5f, nubW * 0.5f), g_pBrush);

    // 3. Mức pin bên trong theo phần trăm thực
    int effPct = (percentage >= 0) ? std::clamp(percentage, 0, 100) : (isSaver ? 75 : 100);
    float padIn = DpiScaleF(2.15f, dpi);
    float maxFillW = std::max(0.0f, bodyW - padIn * 2.0f);
    float fillW = maxFillW * (static_cast<float>(effPct) / 100.0f);
    if (fillW > 0.5f) {
        float fillH = std::max(1.0f, bodyH - padIn * 2.0f);
        float fillR = std::min(DpiScaleF(1.1f, dpi), fillW * 0.5f);
        D2D1_RECT_F rFill = D2D1::RectF(
            rBody.left + padIn, rBody.top + padIn,
            rBody.left + padIn + fillW, rBody.top + padIn + fillH);
        g_pBrush->SetColor(colFill);
        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rFill, fillR, fillR), g_pBrush);
    }

    // 4. Tia sét sạc nằm chính giữa thân pin (đúng phong cách Windows 11 25H2)
    if (isCharging) {
        float bx = rBody.left + bodyW * 0.5f;
        float by = cy;
        float sc = static_cast<float>(dpi) / 96.0f;
        ID2D1PathGeometry* pBolt = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pBolt)) && pBolt) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pBolt->Open(&pSink)) && pSink) {
                pSink->BeginFigure(D2D1::Point2F(bx + 1.1f * sc, by - 5.0f * sc), D2D1_FIGURE_BEGIN_FILLED);
                pSink->AddLine(D2D1::Point2F(bx - 2.9f * sc, by + 0.4f * sc));
                pSink->AddLine(D2D1::Point2F(bx - 0.1f * sc, by + 0.4f * sc));
                pSink->AddLine(D2D1::Point2F(bx - 1.2f * sc, by + 5.0f * sc));
                pSink->AddLine(D2D1::Point2F(bx + 2.9f * sc, by - 0.5f * sc));
                pSink->AddLine(D2D1::Point2F(bx + 0.1f * sc, by - 0.5f * sc));
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                pSink->Close();
                pSink->Release();

                // Viền tách nhẹ giúp tia sét sắc nét trên nền pin xanh lá
                g_pBrush->SetColor(isLight
                    ? D2D1::ColorF(0.96f, 0.97f, 1.00f, 0.85f * alpha)
                    : D2D1::ColorF(0.06f, 0.08f, 0.12f, 0.88f * alpha));
                g_pDCRT->DrawGeometry(pBolt, g_pBrush, DpiScaleF(1.35f, dpi));

                g_pBrush->SetColor(isLight
                    ? D2D1::ColorF(0.06f, 0.08f, 0.12f, alpha)
                    : D2D1::ColorF(1.00f, 1.00f, 1.00f, alpha));
                g_pDCRT->FillGeometry(pBolt, g_pBrush);
            }
            pBolt->Release();
        }
    }
}

// Tối ưu nội dung ngắn gọn cho Popup ở giữa khi Taskbar nằm cạnh Trái / Phải (không lặp tiêu đề, không cụt chữ)
void BuildCompactVerticalPopupTexts(const AlertState& snap, bool vi,
                                    std::wstring& outMidText, std::wstring& outBottomTag) {
    outMidText.clear();
    outBottomTag.clear();

    switch (snap.kind) {
        case AlertKind::Bluetooth:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            if (snap.valueText.find(L'%') != std::wstring::npos) {
                outBottomTag = snap.valueText;
            } else {
                outBottomTag = PickText(vi, snap.isSuccess ? L"BẬT" : L"NGẮT", snap.isSuccess ? L"ON" : L"OFF");
            }
            break;
        case AlertKind::Network:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag = (snap.icon == L"\uE839") ? L"LAN" : L"Wi-Fi";
            break;
        case AlertKind::VpnAlert:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : L"VPN";
            outBottomTag = snap.isSuccess ? L"VPN" : PickText(vi, L"TẮT", L"OFF");
            break;
        case AlertKind::AdminAppOpened:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag = L"Admin";
            break;
        case AlertKind::AppClosed:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag = L"Alt+F4";
            break;
        case AlertKind::CapsLock:
        case AlertKind::NumLock:
        case AlertKind::ScrollLock:
        case AlertKind::InsertKey:
        case AlertKind::DoNotDisturb:
        case AlertKind::FocusMode:
        case AlertKind::BatterySaver:
            outMidText = snap.title;
            outBottomTag = snap.valueText;
            break;
        case AlertKind::ClipboardCopy:
        case AlertKind::ClipboardCut:
        case AlertKind::ClipboardPaste:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            if (snap.valueText.size() <= 6) {
                outBottomTag = snap.valueText;
            } else {
                outBottomTag = (snap.kind == AlertKind::ClipboardCut)
                    ? PickText(vi, L"Cắt", L"Cut")
                    : ((snap.kind == AlertKind::ClipboardPaste)
                        ? PickText(vi, L"Dán", L"Paste")
                        : PickText(vi, L"Chép", L"Copy"));
            }
            break;
        case AlertKind::FileRecycled:
        case AlertKind::FileDeleted:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag = (snap.valueText.size() <= 6) ? snap.valueText : L"";
            break;
        case AlertKind::UsbDevice:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag = snap.isSuccess ? L"USB" : PickText(vi, L"RÚT", L"OUT");
            break;
        case AlertKind::PrivacyAlert:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag.clear();
            break;
        case AlertKind::ClockAlert:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag = snap.valueText;
            break;
        case AlertKind::MediaAlert:
            outMidText = snap.title;
            outBottomTag.clear();
            break;
        default:
            outMidText = !snap.subtitle.empty() ? snap.subtitle : snap.title;
            outBottomTag = (snap.valueText.size() <= 6) ? snap.valueText : L"";
            break;
    }
}

void RenderLiveAlertFrame(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    float t = std::clamp(g_animProgress, 0.0f, 1.0f);
    if (t <= 0.004f) {
        g_sliderDragging = false;
        g_sliderHovered = false;
        g_iconHovered = false;
        if (IsWindowVisible(hwnd)) {
            if (pOrigShowWindow) pOrigShowWindow(hwnd, SW_HIDE);
            else ShowWindow(hwnd, SW_HIDE);
        }
        SetAppsPillHiddenForPopup(false);
        return;
    }

    int canvasW = g_activeSlotRect.right - g_activeSlotRect.left;
    int canvasH = g_activeSlotRect.bottom - g_activeSlotRect.top;
    if (canvasW <= 20 || canvasH <= 20) return;

    UINT dpi = g_activeDpi ? g_activeDpi : 96;
    if (!EnsureRenderResources(dpi, g_uiSettings.fontScalePercent)) return;
    if (!g_alertCanvas.Ensure(canvasW, canvasH)) return;

    AlertState snap;
    {
        std::lock_guard<std::mutex> lock(g_alertMutex);
        snap = g_alert;
    }
    if (snap.kind == AlertKind::ClockAlert) {
        PersistentIndicators liveInd = GetPersistentIndicatorsSnapshot();
        ULONGLONG nowClock = GetTickCount64();
        if (snap.icon == L"\uE916" && liveInd.clock.stopwatchRunning) {
            snap.valueText = FormatClockCompactTime(liveInd.clock.CurrentStopwatchSeconds(nowClock));
        } else if (liveInd.clock.timerRunning) {
            snap.valueText = FormatClockCompactTime(liveInd.clock.CurrentTimerSeconds(nowClock));
        } else if (liveInd.clock.stopwatchRunning) {
            snap.valueText = FormatClockCompactTime(liveInd.clock.CurrentStopwatchSeconds(nowClock));
        }
    }
    if (snap.kind == AlertKind::None) return;

    bool interactiveSlider = (snap.kind == AlertKind::Volume || snap.kind == AlertKind::Brightness);
    ULONGLONG nowTick = GetTickCount64();

    float inv = 1.0f - t;
    float ease = 1.0f - inv * inv * inv;

    float curW = g_activeIsVertical ? (canvasW * (0.90f + 0.10f * ease)) : (canvasW * (0.86f + 0.14f * ease));
    float curH = g_activeIsVertical ? (canvasH * (0.86f + 0.14f * ease)) : (canvasH * (0.90f + 0.10f * ease));
    float pillL = (canvasW - curW) * 0.5f + 0.5f;
    float pillT = (canvasH - curH) * 0.5f + 0.5f;
    float pillR = pillL + curW - 1.0f;
    float pillB = pillT + curH - 1.0f;
    float rad   = std::min({ g_activeCornerRadPx, (pillR - pillL) * 0.5f, (pillB - pillT) * 0.5f });

    bool isLight = g_popupGlassCache.valid
        ? g_popupGlassCache.isLight
        : (g_uiSettings.colorMode == ColorMode::Light ? true
            : (g_uiSettings.colorMode == ColorMode::Dark ? false : g_cachedSystemLightTheme));

    RECT rcBind = { 0, 0, canvasW, canvasH };
    if (SUCCEEDED(g_pDCRT->BindDC(g_alertCanvas.hdcMem, &rcBind))) {
        g_pDCRT->BeginDraw();
        g_pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        g_pDCRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

        D2D1_RECT_F rcPill = D2D1::RectF(pillL, pillT, pillR, pillB);
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rcPill, rad, rad);

        if (EnsureGlassBrushFromSlotCache(g_popupGlassCache, g_pDCRT) && g_popupGlassCache.pBrush) {
            g_pDCRT->FillRoundedRectangle(rr, g_popupGlassCache.pBrush);
        } else {
            g_pBrush->SetColor(isLight
                ? D2D1::ColorF(0.95f, 0.96f, 0.99f, 0.88f)
                : D2D1::ColorF(0.10f, 0.11f, 0.15f, 0.88f));
            g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
        }

        ID2D1LinearGradientBrush* pRim = isLight ? g_pRimGradBrushLight : g_pRimGradBrushDark;
        if (pRim) {
            pRim->SetStartPoint(D2D1::Point2F(pillL, pillT));
            pRim->SetEndPoint(D2D1::Point2F(pillR, pillB));
            g_pDCRT->DrawRoundedRectangle(rr, pRim, DpiScaleF(1.05f, dpi));
        }
        if (isLight) {
            g_pBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.14f));
            g_pDCRT->DrawRoundedRectangle(rr, g_pBrush, DpiScaleF(0.75f, dpi));
        }

        float contentAlpha = std::clamp((ease - 0.08f) / 0.92f, 0.0f, 1.0f);
        g_lastSliderTrackRect = {};
        g_lastIconHitRect = {};

        if (contentAlpha > 0.02f) {
            D2D1_COLOR_F colAccent = isLight
                ? D2D1::ColorF(0.00f, 0.42f, 0.82f, contentAlpha)
                : D2D1::ColorF(0.42f, 0.84f, 1.00f, contentAlpha);

            if (snap.isWarning) {
                colAccent = isLight
                    ? D2D1::ColorF(0.82f, 0.16f, 0.16f, contentAlpha)
                    : D2D1::ColorF(1.00f, 0.44f, 0.42f, contentAlpha);
            } else if (snap.kind == AlertKind::MediaAlert) {
                colAccent = isLight
                    ? D2D1::ColorF(0.66f, 0.14f, 0.66f, contentAlpha)
                    : D2D1::ColorF(0.94f, 0.56f, 1.00f, contentAlpha);
            } else if (snap.isSuccess || snap.kind == AlertKind::Charging || snap.kind == AlertKind::BatterySaver) {
                colAccent = isLight
                    ? D2D1::ColorF(0.06f, 0.54f, 0.28f, contentAlpha)
                    : D2D1::ColorF(0.28f, 0.92f, 0.58f, contentAlpha);
            } else if (snap.kind == AlertKind::VpnAlert) {
                colAccent = isLight
                    ? D2D1::ColorF(0.00f, 0.48f, 0.56f, contentAlpha)
                    : D2D1::ColorF(0.24f, 0.94f, 0.88f, contentAlpha);
            } else if (snap.kind == AlertKind::Brightness || snap.kind == AlertKind::AdminAppOpened) {
                colAccent = isLight
                    ? D2D1::ColorF(0.80f, 0.48f, 0.02f, contentAlpha)
                    : D2D1::ColorF(1.00f, 0.80f, 0.26f, contentAlpha);
            } else if (snap.kind == AlertKind::DoNotDisturb || snap.kind == AlertKind::FocusMode) {
                colAccent = isLight
                    ? D2D1::ColorF(0.44f, 0.24f, 0.82f, contentAlpha)
                    : D2D1::ColorF(0.76f, 0.62f, 1.00f, contentAlpha);
            }

            D2D1_COLOR_F colText = isLight
                ? D2D1::ColorF(0.06f, 0.07f, 0.10f, contentAlpha)
                : D2D1::ColorF(0.97f, 0.98f, 1.00f, contentAlpha);
            D2D1_COLOR_F colSub = isLight
                ? D2D1::ColorF(0.24f, 0.27f, 0.33f, contentAlpha)
                : D2D1::ColorF(0.84f, 0.88f, 0.94f, contentAlpha);

            if (!g_activeIsVertical) {
                float padX = DpiScaleF(16.0f, dpi);
                bool isMediaPopup = (snap.kind == AlertKind::MediaAlert);
                float leftSlotW = isMediaPopup ? DpiScaleF(26.0f, dpi) : DpiScaleF(22.0f, dpi);

                D2D1_RECT_F rIcon = D2D1::RectF(pillL + padX, pillT, pillL + padX + leftSlotW, pillB);
                g_lastIconHitRect = D2D1::RectF(rIcon.left - 4.0f, pillT + 2.0f, rIcon.right + 4.0f, pillB - 2.0f);

                bool isBatteryPopup = (snap.kind == AlertKind::Charging || snap.kind == AlertKind::Discharging ||
                                       snap.kind == AlertKind::LowBattery || snap.kind == AlertKind::BatterySaver);
                if (isMediaPopup && snap.artwork && !snap.artwork->pixels.empty()) {
                    float artEdge = std::min(pillB - pillT - DpiScaleF(10.0f, dpi), DpiScaleF(26.0f, dpi));
                    float artTop  = (pillT + pillB - artEdge) * 0.5f;
                    D2D1_RECT_F rArt = D2D1::RectF(pillL + padX, artTop, pillL + padX + artEdge, artTop + artEdge);
                    DrawRoundedArtworkBox(rArt, DpiScaleF(6.0f, dpi), snap.artwork, snap.artworkId, isLight);
                } else if (isBatteryPopup) {
                    DrawWin11Battery25H2Icon(
                        rIcon, snap.percentage,
                        snap.kind == AlertKind::Charging,
                        snap.kind == AlertKind::BatterySaver,
                        snap.kind == AlertKind::LowBattery,
                        isLight, contentAlpha, dpi);
                } else {
                    if (snap.kind == AlertKind::Volume && g_iconHovered) {
                        g_pBrush->SetColor(isLight
                            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.08f * contentAlpha)
                            : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f * contentAlpha));
                        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(g_lastIconHitRect, 8.0f, 8.0f), g_pBrush);
                    }
                    if (g_fonts.pIcon && !snap.icon.empty()) {
                        g_pBrush->SetColor(colAccent);
                        g_pDCRT->DrawText(snap.icon.c_str(), static_cast<UINT32>(snap.icon.size()), g_fonts.pIcon, rIcon, g_pBrush);
                    }
                }

                // Đo chính xác bề rộng chuỗi bên phải (hoặc sóng nhạc 5 cột cho Media)
                float rightW = 0.0f;
                if (isMediaPopup) {
                    rightW = DpiScaleF(34.0f, dpi);
                } else if (!snap.valueText.empty() && g_fonts.pVal) {
                    float exactValW = MeasureTextExactWidthPx(g_fonts.pVal, snap.valueText);
                    rightW = std::clamp(exactValW + DpiScaleF(8.0f, dpi),
                                        DpiScaleF(48.0f, dpi), curW * 0.34f);
                }
                float valR = pillR - padX;
                float valL = valR - rightW;

                if (isMediaPopup) {
                    float waveH = DpiScaleF(18.0f, dpi);
                    D2D1_RECT_F rWave = D2D1::RectF(valL, (pillT + pillB - waveH) * 0.5f, valR, (pillT + pillB + waveH) * 0.5f);
                    DrawMusicWaveformBars(rWave, 5, snap.mediaPlaying, nowTick, colAccent, dpi);
                } else if (g_fonts.pVal && !snap.valueText.empty() && valR > valL) {
                    g_pBrush->SetColor(colAccent);
                    D2D1_RECT_F rVal = D2D1::RectF(valL, pillT, valR, pillB);
                    g_pDCRT->DrawText(snap.valueText.c_str(), static_cast<UINT32>(snap.valueText.size()),
                                      g_fonts.pVal, rVal, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }

                // Đo chính xác bề rộng tiêu đề DirectWrite để KHÔNG BAO GIỜ bị cắt cụt chữ!
                bool hasBarAndSub = (snap.percentage >= 0 && !snap.subtitle.empty());
                std::wstring displayTitle = hasBarAndSub ? (snap.title + L" · " + snap.subtitle) : snap.title;
                float titleL = rIcon.right + DpiScaleF(8.0f, dpi);

                float minCenterReserve = (snap.percentage >= 0)
                    ? DpiScaleF(110.0f, dpi)
                    : (!snap.subtitle.empty() ? DpiScaleF(80.0f, dpi) : DpiScaleF(10.0f, dpi));
                float maxAllowedTitleR = std::max(titleL + DpiScaleF(60.0f, dpi), valL - minCenterReserve);
                float measuredTitleW   = MeasureTextExactWidthPx(g_fonts.pTitle, displayTitle) + DpiScaleF(6.0f, dpi);
                float titleR           = std::min(maxAllowedTitleR, titleL + measuredTitleW);

                if (g_fonts.pTitle && !displayTitle.empty() && titleR > titleL + 12.0f) {
                    g_pBrush->SetColor(colText);
                    D2D1_RECT_F rTitle = D2D1::RectF(titleL, pillT, titleR, pillB);
                    g_pDCRT->DrawText(displayTitle.c_str(), static_cast<UINT32>(displayTitle.size()),
                                      g_fonts.pTitle, rTitle, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }

                float midL = titleR + DpiScaleF(12.0f, dpi);
                float midR = valL - DpiScaleF(12.0f, dpi);
                float midW = midR - midL;
                if (midW > DpiScaleF(20.0f, dpi)) {
                    if (snap.percentage >= 0) {
                        float trackH = DpiScaleF(interactiveSlider ? 6.0f : 5.5f, dpi);
                        float trackY = (pillT + pillB - trackH) * 0.5f;
                        D2D1_RECT_F rTrack = D2D1::RectF(midL, trackY, midR, trackY + trackH);
                        if (interactiveSlider) {
                            g_lastSliderTrackRect = rTrack;
                        }

                        g_pBrush->SetColor(isLight
                            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.16f * contentAlpha)
                            : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f * contentAlpha));
                        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rTrack, trackH * 0.5f, trackH * 0.5f), g_pBrush);

                        float ratio = std::clamp(snap.smoothPct / 100.0f, 0.0f, 1.0f);
                        float fillX = midL + midW * ratio;
                        if (ratio > 0.002f) {
                            float fillW = std::max(trackH, midW * ratio);
                            D2D1_RECT_F rFill = D2D1::RectF(midL, trackY, midL + fillW, trackY + trackH);
                            g_pBrush->SetColor(colAccent);
                            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rFill, trackH * 0.5f, trackH * 0.5f), g_pBrush);
                        }

                        if (interactiveSlider) {
                            float thumbR = DpiScaleF((g_sliderDragging || g_sliderHovered) ? 7.2f : 5.6f, dpi);
                            D2D1_POINT_2F ptThumb = D2D1::Point2F(std::clamp(fillX, midL, midR), trackY + trackH * 0.5f);
                            g_pBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f * contentAlpha));
                            g_pDCRT->FillEllipse(D2D1::Ellipse(ptThumb, thumbR, thumbR), g_pBrush);
                            g_pBrush->SetColor(colAccent);
                            g_pDCRT->FillEllipse(D2D1::Ellipse(ptThumb, thumbR * 0.52f, thumbR * 0.52f), g_pBrush);
                        }
                    } else if (!snap.subtitle.empty() && g_fonts.pSub) {
                        g_pBrush->SetColor(colSub);
                        D2D1_RECT_F rMid = D2D1::RectF(midL, pillT, midR, pillB);
                        g_pDCRT->DrawText(snap.subtitle.c_str(), static_cast<UINT32>(snap.subtitle.size()),
                                          g_fonts.pSub, rMid, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }
                }
            } else {
                // --- Bố cục Popup Dọc Tối Ưu (Cho Taskbar Trái / Phải: gọn, không thừa thông tin, không sót chữ) ---
                float padY = DpiScaleF(12.0f, dpi);
                float iconBoxH = DpiScaleF(24.0f, dpi);
                float bottomValH = DpiScaleF(20.0f, dpi);
                bool isBatteryPopup = (snap.kind == AlertKind::Charging || snap.kind == AlertKind::Discharging ||
                                       snap.kind == AlertKind::LowBattery || snap.kind == AlertKind::BatterySaver);

                D2D1_RECT_F rIcon = D2D1::RectF(pillL, pillT + padY, pillR, pillT + padY + iconBoxH);
                g_lastIconHitRect = rIcon;
                if (snap.kind == AlertKind::MediaAlert && snap.artwork && !snap.artwork->pixels.empty()) {
                    float artEdge = std::min(pillR - pillL - DpiScaleF(12.0f, dpi), DpiScaleF(24.0f, dpi));
                    float ax = (pillL + pillR - artEdge) * 0.5f;
                    DrawRoundedArtworkBox(D2D1::RectF(ax, pillT + padY, ax + artEdge, pillT + padY + artEdge),
                                          DpiScaleF(5.0f, dpi), snap.artwork, snap.artworkId, isLight);
                } else if (isBatteryPopup) {
                    DrawWin11Battery25H2Icon(
                        rIcon, snap.percentage,
                        snap.kind == AlertKind::Charging,
                        snap.kind == AlertKind::BatterySaver,
                        snap.kind == AlertKind::LowBattery,
                        isLight, contentAlpha, dpi);
                } else if (g_fonts.pIcon && !snap.icon.empty()) {
                    g_pBrush->SetColor(colAccent);
                    g_pDCRT->DrawText(snap.icon.c_str(), static_cast<UINT32>(snap.icon.size()), g_fonts.pIcon, rIcon, g_pBrush);
                }

                float midTop = pillT + padY + iconBoxH + DpiScaleF(8.0f, dpi);

                if (snap.kind == AlertKind::MediaAlert) {
                    D2D1_RECT_F rWave = D2D1::RectF(pillL + DpiScaleF(6.0f, dpi), pillB - padY - DpiScaleF(18.0f, dpi),
                                                    pillR - DpiScaleF(6.0f, dpi), pillB - padY);
                    DrawMusicWaveformBars(rWave, 4, snap.mediaPlaying, nowTick, colAccent, dpi);
                }

                if (snap.percentage >= 0) {
                    float midBot = pillB - padY - bottomValH - DpiScaleF(8.0f, dpi);
                    if (g_fonts.pValCenter && !snap.valueText.empty()) {
                        g_pBrush->SetColor(colAccent);
                        D2D1_RECT_F rVal = D2D1::RectF(pillL + 1.0f, pillB - padY - bottomValH, pillR - 1.0f, pillB - padY);
                        g_pDCRT->DrawText(snap.valueText.c_str(), static_cast<UINT32>(snap.valueText.size()),
                                          g_fonts.pValCenter, rVal, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }
                    if (midBot > midTop + DpiScaleF(24.0f, dpi)) {
                        float trackW = DpiScaleF(6.5f, dpi);
                        float trackX = (pillL + pillR - trackW) * 0.5f;
                        D2D1_RECT_F rTrack = D2D1::RectF(trackX, midTop, trackX + trackW, midBot);
                        if (interactiveSlider) {
                            g_lastSliderTrackRect = rTrack;
                        }

                        g_pBrush->SetColor(isLight
                            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.16f * contentAlpha)
                            : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f * contentAlpha));
                        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rTrack, trackW * 0.5f, trackW * 0.5f), g_pBrush);

                        float ratio = std::clamp(snap.smoothPct / 100.0f, 0.0f, 1.0f);
                        float trackH = midBot - midTop;
                        float fillY = midBot - trackH * ratio;
                        if (ratio > 0.002f) {
                            float fillH = std::max(trackW, trackH * ratio);
                            D2D1_RECT_F rFill = D2D1::RectF(trackX, midBot - fillH, trackX + trackW, midBot);
                            g_pBrush->SetColor(colAccent);
                            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rFill, trackW * 0.5f, trackW * 0.5f), g_pBrush);
                        }

                        if (interactiveSlider) {
                            float thumbR = DpiScaleF((g_sliderDragging || g_sliderHovered) ? 7.2f : 5.6f, dpi);
                            D2D1_POINT_2F ptThumb = D2D1::Point2F(trackX + trackW * 0.5f, std::clamp(fillY, midTop, midBot));
                            g_pBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f * contentAlpha));
                            g_pDCRT->FillEllipse(D2D1::Ellipse(ptThumb, thumbR, thumbR), g_pBrush);
                            g_pBrush->SetColor(colAccent);
                            g_pDCRT->FillEllipse(D2D1::Ellipse(ptThumb, thumbR * 0.52f, thumbR * 0.52f), g_pBrush);
                        }
                    }
                } else {
                    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                    std::wstring vMid, vBot;
                    BuildCompactVerticalPopupTexts(snap, vi, vMid, vBot);

                    float availTagW = (pillR - pillL) - DpiScaleF(6.0f, dpi);
                    bool showBotTag = false;
                    if (snap.kind != AlertKind::MediaAlert && !vBot.empty() && g_fonts.pBadgeVertTime) {
                        float tagW = MeasureTextExactWidthPx(g_fonts.pBadgeVertTime, vBot);
                        if (tagW <= availTagW) {
                            showBotTag = true;
                            g_pBrush->SetColor(colAccent);
                            D2D1_RECT_F rBot = D2D1::RectF(pillL + DpiScaleF(2.0f, dpi), pillB - padY - bottomValH,
                                                           pillR - DpiScaleF(2.0f, dpi), pillB - padY);
                            g_pDCRT->DrawText(vBot.c_str(), static_cast<UINT32>(vBot.size()),
                                              g_fonts.pBadgeVertTime, rBot, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                        }
                    }

                    float midBot = pillB - padY - ((showBotTag || snap.kind == AlertKind::MediaAlert)
                                                    ? (bottomValH + DpiScaleF(8.0f, dpi))
                                                    : DpiScaleF(4.0f, dpi));
                    if (midBot > midTop + DpiScaleF(28.0f, dpi) && g_fonts.pSub && !vMid.empty()) {
                        D2D1_POINT_2F center = D2D1::Point2F((pillL + pillR) * 0.5f, (midTop + midBot) * 0.5f);
                        float spanLen = std::max(12.0f, (midBot - midTop) - DpiScaleF(4.0f, dpi));
                        float crossW  = std::max(12.0f, (pillR - pillL) - DpiScaleF(6.0f, dpi));

                        D2D1::Matrix3x2F oldMat;
                        g_pDCRT->GetTransform(&oldMat);
                        g_pDCRT->SetTransform(D2D1::Matrix3x2F::Rotation(-90.0f, center) * oldMat);

                        g_pBrush->SetColor(colText);
                        D2D1_RECT_F rRot = D2D1::RectF(center.x - spanLen * 0.5f, center.y - crossW * 0.5f,
                                                       center.x + spanLen * 0.5f, center.y + crossW * 0.5f);
                        g_pDCRT->DrawText(vMid.c_str(), static_cast<UINT32>(vMid.size()),
                                          g_fonts.pSub, rRot, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                        g_pDCRT->SetTransform(oldMat);
                    }
                }
            }
        }

        HRESULT hr = g_pDCRT->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            ReleaseRenderResources();
            return;
        }
        if (SUCCEEDED(hr)) {
            BYTE masterAlpha = static_cast<BYTE>(std::clamp(static_cast<int>(std::lround(ease * 255.0f)), 1, 255));
            POINT ptSrc = { 0, 0 };
            POINT ptDst = { g_activeSlotRect.left, g_activeSlotRect.top };
            SIZE  szDst = { canvasW, canvasH };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA };

            HDC hdcScr = GetDC(nullptr);
            UpdateLayeredWindow(hwnd, hdcScr, &ptDst, &szDst, g_alertCanvas.hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
            ReleaseDC(nullptr, hdcScr);

            if (!IsWindowVisible(hwnd)) {
                if (pOrigShowWindow) pOrigShowWindow(hwnd, SW_SHOWNA);
                if (pOrigSetWindowPos) {
                    pOrigSetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
                }
            }
        }
    }
}

void ApplyMouseSliderPercent(HWND hwnd, int newPct) {
    newPct = std::clamp(newPct, 0, 100);
    AlertKind curKind = AlertKind::None;
    {
        std::lock_guard<std::mutex> lock(g_alertMutex);
        curKind = g_alert.kind;
    }
    if (curKind != AlertKind::Volume && curKind != AlertKind::Brightness) return;

    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
    int durationMs = g_alertDurationMsAtomic.load(std::memory_order_relaxed);
    ULONGLONG now = GetTickCount64();
    g_lastVolOrBriAlertTick.store(now, std::memory_order_relaxed);

    if (curKind == AlertKind::Volume) {
        bool muted = false;
        SetSystemVolumePercentFromUi(newPct, muted);
        std::wstring icon = (muted || newPct == 0)
            ? L"\uE74F"
            : ((newPct < 33) ? L"\uE993" : ((newPct < 66) ? L"\uE994" : L"\uE995"));
        std::wstring title = muted
            ? PickText(vi, L"Âm lượng (Tắt)", L"Volume (Muted)")
            : PickText(vi, L"Âm lượng", L"Volume");
        {
            std::lock_guard<std::mutex> lock(g_alertMutex);
            g_alert.icon = icon;
            g_alert.title = title;
            g_alert.percentage = newPct;
            g_alert.smoothPct = static_cast<float>(newPct);
            g_alert.valueText = std::to_wstring(newPct) + L"%";
            g_alert.isWarning = muted;
            g_alert.expireTick = now + static_cast<ULONGLONG>(durationMs);
        }
    } else {
        SetSystemBrightnessPercent(newPct);
        {
            std::lock_guard<std::mutex> lock(g_alertMutex);
            g_alert.percentage = newPct;
            g_alert.smoothPct = static_cast<float>(newPct);
            g_alert.valueText = std::to_wstring(newPct) + L"%";
            g_alert.expireTick = now + static_cast<ULONGLONG>(durationMs);
        }
    }

    SetTimer(hwnd, IDT_ALERT_EXPIRE, durationMs + 25, nullptr);
    RenderLiveAlertFrame(hwnd);
}

int ComputeSliderPctFromMousePoint(int ptX, int ptY) {
    if (g_lastSliderTrackRect.right <= g_lastSliderTrackRect.left ||
        g_lastSliderTrackRect.bottom <= g_lastSliderTrackRect.top) {
        return -1;
    }
    if (!g_activeIsVertical) {
        float w = std::max(1.0f, g_lastSliderTrackRect.right - g_lastSliderTrackRect.left);
        float ratio = (static_cast<float>(ptX) - g_lastSliderTrackRect.left) / w;
        return std::clamp(static_cast<int>(std::lround(ratio * 100.0f)), 0, 100);
    } else {
        float h = std::max(1.0f, g_lastSliderTrackRect.bottom - g_lastSliderTrackRect.top);
        float ratio = (g_lastSliderTrackRect.bottom - static_cast<float>(ptY)) / h;
        return std::clamp(static_cast<int>(std::lround(ratio * 100.0f)), 0, 100);
    }
}

// ---------------------------------------------------------------------------
// 12. Quyền Nhạy Cảm, Phát Nhạc GSMTC (Ảnh Bìa WIC + Sóng Nhạc) & Tiến Trình Tác Vụ (77%)
// ---------------------------------------------------------------------------
static std::mutex g_appFriendlyMutex;
static std::map<std::wstring, std::wstring> g_appFriendlyCache;

std::wstring LookupFriendlyExeDisplayName(std::wstring exeName, const wchar_t* fullPath = nullptr) {
    std::wstring key = ToLowerStr(exeName);
    {
        std::lock_guard<std::mutex> lock(g_appFriendlyMutex);
        auto it = g_appFriendlyCache.find(key);
        if (it != g_appFriendlyCache.end()) return it->second;
    }

    static constexpr std::pair<PCWSTR, PCWSTR> kKnownApps[] = {
        { L"chrome.exe",   L"Google Chrome" },
        { L"msedge.exe",   L"Microsoft Edge" },
        { L"firefox.exe",  L"Mozilla Firefox" },
        { L"brave.exe",    L"Brave" },
        { L"explorer.exe", L"File Explorer" },
        { L"7zfm.exe",     L"7-Zip" },
        { L"7zg.exe",      L"7-Zip" },
        { L"winrar.exe",   L"WinRAR" },
        { L"zalo.exe",     L"Zalo" },
        { L"discord.exe",  L"Discord" },
        { L"telegram.exe", L"Telegram" },
        { L"spotify.exe",  L"Spotify" },
        { L"vlc.exe",      L"VLC Media Player" },
        { L"code.exe",     L"Visual Studio Code" },
        { L"notepad++.exe",L"Notepad++" },
        { L"obs64.exe",    L"OBS Studio" },
        { L"zoom.exe",     L"Zoom" },
        { L"teams.exe",    L"Microsoft Teams" },
        { L"audacity.exe", L"Audacity" }
    };
    for (const auto& kv : kKnownApps) {
        if (key == kv.first) return kv.second;
    }

    std::wstring display = exeName;
    if (key.size() > 4 && key.substr(key.size() - 4) == L".exe") {
        display.resize(display.size() - 4);
        if (!display.empty()) display[0] = static_cast<wchar_t>(std::towupper(display[0]));
    }

    if (fullPath && fullPath[0]) {
        DWORD dummy = 0;
        DWORD verSize = GetFileVersionInfoSizeW(fullPath, &dummy);
        if (verSize > 0 && verSize <= 512 * 1024) {
            std::vector<BYTE> verBuf(verSize);
            if (GetFileVersionInfoW(fullPath, 0, verSize, verBuf.data())) {
                struct LangCp { WORD lang, cp; } *pTrans = nullptr;
                UINT cbTrans = 0;
                if (VerQueryValueW(verBuf.data(), L"\\VarFileInfo\\Translation",
                                   reinterpret_cast<void**>(&pTrans), &cbTrans) && pTrans && cbTrans >= sizeof(LangCp)) {
                    wchar_t subBlock[96] = {};
                    swprintf_s(subBlock, L"\\StringFileInfo\\%04x%04x\\FileDescription", pTrans[0].lang, pTrans[0].cp);
                    wchar_t* desc = nullptr;
                    UINT cchDesc = 0;
                    if (VerQueryValueW(verBuf.data(), subBlock, reinterpret_cast<void**>(&desc), &cchDesc) &&
                        desc && cchDesc > 1 && cchDesc < 128) {
                        display.assign(desc, cchDesc - 1);
                    }
                }
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_appFriendlyMutex);
        if (g_appFriendlyCache.size() < 256) {
            g_appFriendlyCache.emplace(key, display);
        }
    }
    return display;
}

std::wstring GetFriendlyProcessNameByPid(DWORD pid, std::wstring* outExeBase = nullptr) {
    if (pid == 0 || pid == 4) return {};
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return {};
    wchar_t path[2048] = {};
    DWORD cch = ARRAYSIZE(path);
    bool ok = (QueryFullProcessImageNameW(hProc, 0, path, &cch) != FALSE);
    CloseHandle(hProc);
    if (!ok || !path[0]) return {};
    const wchar_t* base = PathFindFileNameW(path);
    if (outExeBase) *outExeBase = base ? base : L"";
    return LookupFriendlyExeDisplayName(base ? base : L"", path);
}

// Xử lý tiến trình tác vụ TaskbandHWND (ITaskbarList3 -> Icon + 77%)
void SyncTaskbandProgressToUi(WPARAM completedWndParam) {
    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
    ULONGLONG now = GetTickCount64();

    if (!g_enableTaskProgressBadgeAtomic.load(std::memory_order_relaxed)) {
        bool cleared = false;
        {
            std::lock_guard<std::mutex> lock(g_persistMutex);
            if (g_persistIndicators.task.active) {
                g_persistIndicators.task = {};
                cleared = true;
            }
        }
        if (cleared) RenderCornerWidgetSurface(false);
        return;
    }

    if (completedWndParam != 0) {
        HWND hDone = reinterpret_cast<HWND>(completedWndParam);
        DWORD pid = 0;
        if (IsWindow(hDone)) GetWindowThreadProcessId(hDone, &pid);
        std::wstring app = GetFriendlyProcessNameByPid(pid);
        TriggerLiveAlert(
            AlertKind::TaskProgressAlert, L"\uE73E",
            PickText(vi, L"Hoàn tất tác vụ", L"Task Completed"),
            app.empty() ? PickText(vi, L"Tiến trình đã xong", L"Operation finished") : app,
            L"100%", 100, false, true);
    }

    struct Candidate {
        uintptr_t hwndKey = 0;
        int percent = -1;
        unsigned state = 0;
        bool newlyReadyToNotify = false;
    };
    std::vector<Candidate> activeList;
    bool hasPendingEntries = false;
    {
        std::lock_guard<std::mutex> lock(g_taskbandMutex);
        for (auto it = g_taskbandEntries.begin(); it != g_taskbandEntries.end(); ) {
            HWND w = reinterpret_cast<HWND>(it->first);
            if (!IsWindow(w) || !IsWindowVisible(w) || it->second.state == 0) {
                it = g_taskbandEntries.erase(it);
                continue;
            }
            if (it->second.percent >= 100) {
                if (it->second.reached100Tick == 0) {
                    it->second.reached100Tick = now;
                }
                // Tự động xóa sau 1.5 giây ở mức 100% (tránh bị treo 100% với app không gửi TBPF_NOPROGRESS)
                if (now - it->second.reached100Tick >= 1500) {
                    it = g_taskbandEntries.erase(it);
                    continue;
                }
            }
            if (now >= it->second.startedTick + 350) {
                if (it->second.state != 2 || it->second.percent >= 0) {
                    bool shouldNotifyStart = false;
                    if (!it->second.startNotified && it->second.percent >= 0 && it->second.percent < 100) {
                        it->second.startNotified = true;
                        shouldNotifyStart = true;
                    }
                    activeList.push_back({ it->first, it->second.percent, it->second.state, shouldNotifyStart });
                }
            }
            ++it;
        }
        hasPendingEntries = !g_taskbandEntries.empty();
    }
    if (g_alertHwnd && IsWindow(g_alertHwnd)) {
        if (hasPendingEntries) {
            SetTimer(g_alertHwnd, IDT_TASK_PROGRESS_CLEANUP, 500, nullptr);
        } else {
            KillTimer(g_alertHwnd, IDT_TASK_PROGRESS_CLEANUP);
        }
    }

    ActiveTaskDisplayInfo nextTask{};
    if (!activeList.empty()) {
        // Ưu tiên tác vụ có phần trăm cụ thể
        auto bestIt = std::find_if(activeList.begin(), activeList.end(), [](const Candidate& c) { return c.percent >= 0; });
        const Candidate& chosen = (bestIt != activeList.end()) ? *bestIt : activeList.front();

        DWORD pid = 0;
        GetWindowThreadProcessId(reinterpret_cast<HWND>(chosen.hwndKey), &pid);
        std::wstring appName = GetFriendlyProcessNameByPid(pid);

        nextTask.active = true;
        nextTask.percent = chosen.percent;
        nextTask.targetWindow = chosen.hwndKey;
        nextTask.appName = appName;

        for (const auto& c : activeList) {
            if (c.newlyReadyToNotify) {
                DWORD cPid = 0;
                GetWindowThreadProcessId(reinterpret_cast<HWND>(c.hwndKey), &cPid);
                std::wstring cApp = GetFriendlyProcessNameByPid(cPid);
                g_cornerSelectedKind = CornerItemKind::TaskProgress;
                const wchar_t* taskGlyph = PickTaskProgressGlyph(cApp, reinterpret_cast<HWND>(c.hwndKey));
                TriggerLiveAlert(
                    AlertKind::TaskProgressAlert, taskGlyph,
                    PickText(vi, L"Tiến trình đang chạy", L"Task in Progress"),
                    cApp, std::to_wstring(c.percent) + L"%", c.percent, false, false);
                break;
            }
        }
    }

    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_persistMutex);
        const auto& prev = g_persistIndicators.task;
        if (prev.active != nextTask.active ||
            prev.percent != nextTask.percent ||
            prev.targetWindow != nextTask.targetWindow) {
            g_persistIndicators.task = nextTask;
            changed = true;
        }
    }
    if (changed) {
        RenderCornerWidgetSurface(false);
    }
}

template <typename AsyncOp, typename TResult>
bool WaitWinRtAsyncBounded(AsyncOp const& op, TResult& out, DWORD timeoutMs) {
    try {
        HANDLE hDone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!hDone) return false;
        auto state = std::make_shared<std::atomic<bool>>(false);
        op.Completed([hDone, state](auto const&, auto const&) noexcept {
            if (!state->exchange(true)) SetEvent(hDone);
        });
        HANDLE waits[2] = { g_stopEvent, hDone };
        DWORD wr = WaitForMultipleObjects(2, waits, FALSE, timeoutMs);
        if (wr != WAIT_OBJECT_0 + 1) {
            try { op.Cancel(); } catch (...) {}
            WaitForSingleObject(hDone, 120);
            CloseHandle(hDone);
            return false;
        }
        CloseHandle(hDone);
        if (op.Status() == wf::AsyncStatus::Completed) {
            out = op.GetResults();
            return true;
        }
    } catch (...) {}
    return false;
}

// Bộ theo dõi Phát nhạc Hệ thống GSMTC + Giải mã Ảnh bìa WIC (0% CPU khi nghỉ, tự bắt sự kiện bài mới!)
class SystemMediaMonitor {
    using SessionMgr = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
    using Session    = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession;
    using MediaProps = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionMediaProperties;
    using PlayStatus = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

    SessionMgr mgr{ nullptr };
    SessionMgr::CurrentSessionChanged_revoker curSessionRevoker;
    SessionMgr::SessionsChanged_revoker sessionsRevoker;

    Session watchedSession{ nullptr };
    Session::PlaybackInfoChanged_revoker playRevoker;
    Session::MediaPropertiesChanged_revoker propRevoker;

    HANDLE wakeEvent = nullptr;
    std::atomic<bool> dirty{ true };
    ULONGLONG nextInitRetryTick = 0;

    std::wstring cachedArtTrackKey;
    std::shared_ptr<DecodedBgraImage> cachedArtwork;
    uint64_t cachedArtworkId = 0;
    uint64_t nextArtworkCounter = 1;

    bool bootstrapped = false;
    bool prevPlaying = false;
    std::wstring prevAnnouncedTrackKey;

    void SignalDirty() noexcept {
        dirty.store(true, std::memory_order_relaxed);
        if (wakeEvent) SetEvent(wakeEvent);
    }

public:
    explicit SystemMediaMonitor(HANDLE hWake) : wakeEvent(hWake) {}
    ~SystemMediaMonitor() { Stop(); }

    void Stop() {
        playRevoker.revoke();
        propRevoker.revoke();
        curSessionRevoker.revoke();
        sessionsRevoker.revoke();
        watchedSession = nullptr;
        mgr = nullptr;
        cachedArtwork.reset();
        cachedArtTrackKey.clear();
    }

    void PollAndSync() {
        if (!g_enableMediaBadgeAtomic.load(std::memory_order_relaxed)) {
            Stop();
            bool cleared = false;
            {
                std::lock_guard<std::mutex> lock(g_persistMutex);
                if (g_persistIndicators.media.available) {
                    g_persistIndicators.media = {};
                    cleared = true;
                }
            }
            if (cleared && g_alertHwnd && IsWindow(g_alertHwnd)) {
                PostMessageW(g_alertHwnd, WM_APP_INDICATORS_CHANGED, 0, 0);
            }
            return;
        }

        ULONGLONG now = GetTickCount64();
        if (!mgr) {
            if (now < nextInitRetryTick) return;
            nextInitRetryTick = now + 8000;
            try {
                SessionMgr obtained{ nullptr };
                if (WaitWinRtAsyncBounded(SessionMgr::RequestAsync(), obtained, 1500) && obtained) {
                    mgr = obtained;
                    curSessionRevoker = mgr.CurrentSessionChanged(winrt::auto_revoke,
                        [this](auto const&, auto const&) noexcept { SignalDirty(); });
                    sessionsRevoker = mgr.SessionsChanged(winrt::auto_revoke,
                        [this](auto const&, auto const&) noexcept { SignalDirty(); });
                    dirty.store(true, std::memory_order_relaxed);
                }
            } catch (...) {
                mgr = nullptr;
                return;
            }
        }
        if (!mgr) return;

        bool toggleCmd = g_requestMediaTogglePlayPause.exchange(false, std::memory_order_relaxed);
        if (!dirty.exchange(false, std::memory_order_relaxed) && !toggleCmd) {
            return;
        }

        try {
            Session chosen{ nullptr };
            bool chosenPlaying = false;

            auto allSessions = mgr.GetSessions();
            for (auto const& s : allSessions) {
                if (!s) continue;
                try {
                    auto st = s.GetPlaybackInfo().PlaybackStatus();
                    if (st == PlayStatus::Playing) {
                        chosen = s;
                        chosenPlaying = true;
                        break;
                    }
                } catch (...) {}
            }
            if (!chosen) {
                auto cur = mgr.GetCurrentSession();
                if (cur) {
                    try {
                        auto st = cur.GetPlaybackInfo().PlaybackStatus();
                        if (st == PlayStatus::Playing || st == PlayStatus::Paused) {
                            chosen = cur;
                            chosenPlaying = (st == PlayStatus::Playing);
                        }
                    } catch (...) {}
                }
            }

            if (chosen != watchedSession) {
                playRevoker.revoke();
                propRevoker.revoke();
                watchedSession = chosen;
                if (watchedSession) {
                    playRevoker = watchedSession.PlaybackInfoChanged(winrt::auto_revoke,
                        [this](auto const&, auto const&) noexcept { SignalDirty(); });
                    propRevoker = watchedSession.MediaPropertiesChanged(winrt::auto_revoke,
                        [this](auto const&, auto const&) noexcept { SignalDirty(); });
                }
            }

            if (toggleCmd && watchedSession) {
                bool ok = false;
                WaitWinRtAsyncBounded(watchedSession.TryTogglePlayPauseAsync(), ok, 800);
                try {
                    chosenPlaying = (watchedSession.GetPlaybackInfo().PlaybackStatus() == PlayStatus::Playing);
                } catch (...) {}
            }

            ActiveMediaDisplayInfo nextMedia{};
            if (watchedSession) {
                MediaProps props{ nullptr };
                if (WaitWinRtAsyncBounded(watchedSession.TryGetMediaPropertiesAsync(), props, 1200) && props) {
                    nextMedia.available = true;
                    nextMedia.playing = chosenPlaying;
                    nextMedia.title = props.Title().c_str();
                    nextMedia.artist = props.Artist().c_str();
                    nextMedia.sourceAppId = watchedSession.SourceAppUserModelId().c_str();

                    std::wstring trackKey = nextMedia.sourceAppId + L"|" + nextMedia.title + L"|" + nextMedia.artist;
                    if (trackKey != cachedArtTrackKey) {
                        cachedArtTrackKey = trackKey;
                        cachedArtwork.reset();
                        cachedArtworkId = 0;

                        try {
                            auto thumbRef = props.Thumbnail();
                            if (thumbRef) {
                                winrt::Windows::Storage::Streams::IRandomAccessStreamWithContentType stream{ nullptr };
                                if (WaitWinRtAsyncBounded(thumbRef.OpenReadAsync(), stream, 1200) &&
                                    stream && stream.Size() > 0 && stream.Size() <= 4 * 1024 * 1024) {
                                    uint32_t cb = static_cast<uint32_t>(stream.Size());
                                    winrt::Windows::Storage::Streams::DataReader reader(stream.GetInputStreamAt(0));
                                    uint32_t loaded = 0;
                                    if (WaitWinRtAsyncBounded(reader.LoadAsync(cb), loaded, 1200) && loaded > 0) {
                                        std::vector<uint8_t> bytes(loaded);
                                        reader.ReadBytes(bytes);
                                        reader.Close();
                                        stream.Close();
                                        cachedArtwork = DecodeMemoryImageWic(bytes.data(), static_cast<DWORD>(bytes.size()), 96);
                                        if (cachedArtwork) {
                                            cachedArtworkId = nextArtworkCounter++;
                                        }
                                    }
                                }
                            }
                        } catch (...) {}
                    }

                    nextMedia.artwork = cachedArtwork;
                    nextMedia.artworkId = cachedArtworkId;
                }
            }

            bool vi = g_useViAtomic.load(std::memory_order_relaxed);
            std::wstring curTrackKey = nextMedia.available ? (nextMedia.title + L"|" + nextMedia.artist) : L"";

            if (!bootstrapped) {
                bootstrapped = true;
                prevPlaying = nextMedia.playing;
                prevAnnouncedTrackKey = curTrackKey;
            } else {
                bool startedOrChanged = nextMedia.available && nextMedia.playing &&
                                        (!prevPlaying || (!nextMedia.title.empty() && curTrackKey != prevAnnouncedTrackKey));
                prevPlaying = nextMedia.playing;
                if (nextMedia.playing) prevAnnouncedTrackKey = curTrackKey;

                if (startedOrChanged) {
                    g_cornerSelectedKind = CornerItemKind::Media;
                    std::wstring showTitle = nextMedia.title.empty()
                        ? PickText(vi, L"Đang phát nhạc", L"Now Playing")
                        : nextMedia.title;
                    std::wstring showArtist = nextMedia.artist.empty()
                        ? PickText(vi, L"Trình phát phương tiện", L"Media Player")
                        : nextMedia.artist;
                    TriggerLiveAlert(
                        AlertKind::MediaAlert, L"\uEC4F",
                        showTitle, showArtist, L"", -1, false, false,
                        nextMedia.artwork, nextMedia.artworkId, true);
                }
            }

            bool uiChanged = false;
            {
                std::lock_guard<std::mutex> lock(g_persistMutex);
                const auto& prev = g_persistIndicators.media;
                if (prev.available != nextMedia.available ||
                    prev.playing != nextMedia.playing ||
                    prev.title != nextMedia.title ||
                    prev.artist != nextMedia.artist ||
                    prev.artworkId != nextMedia.artworkId) {
                    g_persistIndicators.media = nextMedia;
                    uiChanged = true;
                }
            }
            if (uiChanged && g_alertHwnd && IsWindow(g_alertHwnd)) {
                PostMessageW(g_alertHwnd, WM_APP_INDICATORS_CHANGED, 0, 0);
            }
        } catch (...) {
            Stop();
        }
    }
};

// Theo dõi phiên thu âm Micro thời gian thực qua Core Audio (IAudioSessionManager2)
struct MicCaptureSignals : winrt::implements<MicCaptureSignals, IAudioSessionEvents, IAudioSessionNotification, IMMNotificationClient> {
    HANDLE wakeEvent = nullptr;
    std::atomic<bool> devicesDirty{true};
    std::mutex mutex;
    std::vector<winrt::com_ptr<IAudioSessionControl>> incoming;

    explicit MicCaptureSignals(HANDLE hWake) : wakeEvent(hWake) {}

    HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl* session) noexcept override {
        try {
            if (session) {
                std::lock_guard<std::mutex> lock(mutex);
                if (incoming.size() < 64) {
                    winrt::com_ptr<IAudioSessionControl> copy;
                    copy.copy_from(session);
                    incoming.push_back(std::move(copy));
                }
            }
            if (wakeEvent) SetEvent(wakeEvent);
        } catch (...) {}
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState) noexcept override {
        if (wakeEvent) SetEvent(wakeEvent);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason) noexcept override {
        devicesDirty.store(true, std::memory_order_relaxed);
        if (wakeEvent) SetEvent(wakeEvent);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR, LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR, LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float, BOOL, LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD, float[], DWORD, LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID, LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) noexcept override {
        devicesDirty.store(true, std::memory_order_relaxed);
        if (wakeEvent) SetEvent(wakeEvent);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) noexcept override {
        devicesDirty.store(true, std::memory_order_relaxed);
        if (wakeEvent) SetEvent(wakeEvent);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) noexcept override {
        devicesDirty.store(true, std::memory_order_relaxed);
        if (wakeEvent) SetEvent(wakeEvent);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) noexcept override {
        if (flow == eRender && role == eMultimedia && g_alertHwnd && IsWindow(g_alertHwnd)) {
            PostMessageW(g_alertHwnd, WM_APP_AUDIO_ENDPOINT_CHANGED, 0, 0);
        }
        if (flow == eCapture) {
            devicesDirty.store(true, std::memory_order_relaxed);
            if (wakeEvent) SetEvent(wakeEvent);
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, PROPERTYKEY) noexcept override { return S_OK; }
};

class RealTimeMicMonitor {
    winrt::com_ptr<IMMDeviceEnumerator> enumerator;
    winrt::com_ptr<MicCaptureSignals> signals;
    std::vector<winrt::com_ptr<IAudioSessionManager2>> managers;
    struct SessionItem {
        winrt::com_ptr<IAudioSessionControl2> ctrl;
        std::wstring id;
        DWORD pid = 0;
        std::wstring appName;
    };
    std::vector<SessionItem> sessions;
    bool endpointReg = false;

    void AttachSession(winrt::com_ptr<IAudioSessionControl> const& s) {
        auto c2 = s.try_as<IAudioSessionControl2>();
        if (!c2 || sessions.size() >= 192) return;
        LPWSTR rawId = nullptr;
        if (FAILED(c2->GetSessionInstanceIdentifier(&rawId))) return;
        std::wstring sid(rawId ? rawId : L"");
        CoTaskMemFree(rawId);
        if (sid.empty()) return;
        for (const auto& ex : sessions) {
            if (ex.id == sid) return;
        }
        DWORD pid = 0;
        c2->GetProcessId(&pid);
        std::wstring app = GetFriendlyProcessNameByPid(pid);
        if (SUCCEEDED(c2->RegisterAudioSessionNotification(signals.get()))) {
            sessions.push_back({ c2, std::move(sid), pid, std::move(app) });
        }
    }

public:
    explicit RealTimeMicMonitor(HANDLE hWake) : signals(winrt::make_self<MicCaptureSignals>(hWake)) {}
    ~RealTimeMicMonitor() { Clear(); signals = nullptr; }

    void Clear() {
        if (endpointReg && enumerator) {
            enumerator->UnregisterEndpointNotificationCallback(signals.get());
        }
        endpointReg = false;
        for (const auto& m : managers) m->UnregisterSessionNotification(signals.get());
        for (const auto& s : sessions) s.ctrl->UnregisterAudioSessionNotification(signals.get());
        managers.clear();
        sessions.clear();
        enumerator = nullptr;
        if (signals) {
            std::lock_guard<std::mutex> lock(signals->mutex);
            signals->incoming.clear();
        }
    }

    void EnsureEndpoints() {
        if (!signals) return;
        if (!signals->devicesDirty.exchange(false, std::memory_order_relaxed) && enumerator) return;
        Clear();
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(enumerator.put())))) {
            return;
        }
        endpointReg = SUCCEEDED(enumerator->RegisterEndpointNotificationCallback(signals.get()));
        winrt::com_ptr<IMMDeviceCollection> col;
        if (FAILED(enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, col.put())) || !col) return;
        UINT count = 0;
        col->GetCount(&count);
        for (UINT i = 0; i < std::min<UINT>(count, 24); ++i) {
            winrt::com_ptr<IMMDevice> dev;
            winrt::com_ptr<IAudioSessionManager2> mgr;
            if (FAILED(col->Item(i, dev.put())) ||
                FAILED(dev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, mgr.put_void()))) {
                continue;
            }
            if (SUCCEEDED(mgr->RegisterSessionNotification(signals.get()))) {
                managers.push_back(mgr);
                winrt::com_ptr<IAudioSessionEnumerator> sEnum;
                if (SUCCEEDED(mgr->GetSessionEnumerator(sEnum.put())) && sEnum) {
                    int n = 0;
                    sEnum->GetCount(&n);
                    for (int j = 0; j < std::min(n, 128); ++j) {
                        winrt::com_ptr<IAudioSessionControl> c;
                        if (SUCCEEDED(sEnum->GetSession(j, c.put())) && c) AttachSession(c);
                    }
                }
            }
        }
    }

    bool QueryActiveMic(std::wstring& outAppName) {
        EnsureEndpoints();
        if (!signals) return false;

        std::vector<winrt::com_ptr<IAudioSessionControl>> inc;
        {
            std::lock_guard<std::mutex> lock(signals->mutex);
            inc.swap(signals->incoming);
        }
        for (const auto& c : inc) AttachSession(c);

        bool active = false;
        for (auto it = sessions.begin(); it != sessions.end(); ) {
            AudioSessionState st{};
            if (FAILED(it->ctrl->GetState(&st)) || st == AudioSessionStateExpired) {
                it->ctrl->UnregisterAudioSessionNotification(signals.get());
                it = sessions.erase(it);
                continue;
            }
            if (st == AudioSessionStateActive) {
                active = true;
                if (outAppName.empty() && !it->appName.empty()) {
                    outAppName = it->appName;
                }
            }
            ++it;
        }
        return active;
    }
};

struct CameraSensorCallback : winrt::implements<CameraSensorCallback, IMFSensorActivitiesReportCallback> {
    HANDLE wakeEvent = nullptr;
    std::shared_ptr<int> lifetime;
    std::mutex mutex;
    std::map<std::wstring, std::set<DWORD>> activeDevices;

    CameraSensorCallback(HANDLE hWake, std::shared_ptr<int> life) : wakeEvent(hWake), lifetime(std::move(life)) {}

    HRESULT STDMETHODCALLTYPE OnActivitiesReport(IMFSensorActivitiesReport* report) noexcept override {
        try {
            ULONG count = 0;
            if (!report || FAILED(report->GetCount(&count))) return S_OK;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (count == 0) activeDevices.clear();
                for (ULONG i = 0; i < std::min<ULONG>(count, 32); ++i) {
                    winrt::com_ptr<IMFSensorActivityReport> item;
                    if (FAILED(report->GetActivityReport(i, item.put()))) continue;
                    wchar_t sym[1024] = {};
                    ULONG cch = 0, nProcs = 0;
                    if (FAILED(item->GetSymbolicLink(sym, ARRAYSIZE(sym), &cch)) ||
                        FAILED(item->GetProcessCount(&nProcs))) {
                        continue;
                    }
                    std::set<DWORD> pids;
                    for (ULONG j = 0; j < std::min<ULONG>(nProcs, 64); ++j) {
                        winrt::com_ptr<IMFSensorProcessActivity> pa;
                        ULONG pid = 0;
                        BOOL streaming = FALSE;
                        if (SUCCEEDED(item->GetProcessActivity(j, pa.put())) &&
                            SUCCEEDED(pa->GetStreamingState(&streaming)) && streaming &&
                            SUCCEEDED(pa->GetProcessId(&pid)) && pid != 0) {
                            pids.insert(pid);
                        }
                    }
                    if (pids.empty()) activeDevices.erase(sym);
                    else activeDevices[sym] = std::move(pids);
                }
            }
            if (wakeEvent) SetEvent(wakeEvent);
        } catch (...) {}
        return S_OK;
    }
};

class RealTimeCameraMonitor {
    HMODULE hMfPlat = nullptr;
    HMODULE hMfSensor = nullptr;
    using MFShutdownFn = HRESULT(WINAPI*)();
    MFShutdownFn fnShutdown = nullptr;
    bool startedMf = false;
    winrt::com_ptr<IMFSensorActivityMonitor> monitor;
    winrt::com_ptr<CameraSensorCallback> callback;
    std::shared_ptr<int> lifetime = std::make_shared<int>(0);
    HANDLE wakeEvent = nullptr;

public:
    explicit RealTimeCameraMonitor(HANDLE hWake) : wakeEvent(hWake) {}
    ~RealTimeCameraMonitor() { Stop(); }

    void Start() {
        if (monitor) return;
        if (!hMfPlat) hMfPlat = LoadLibraryExW(L"mfplat.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        using MFStartupFn = HRESULT(WINAPI*)(ULONG, DWORD);
        auto fnStartup = hMfPlat ? reinterpret_cast<MFStartupFn>(GetProcAddress(hMfPlat, "MFStartup")) : nullptr;
        fnShutdown = hMfPlat ? reinterpret_cast<MFShutdownFn>(GetProcAddress(hMfPlat, "MFShutdown")) : nullptr;
        if (!startedMf && fnStartup && fnShutdown) {
            startedMf = SUCCEEDED(fnStartup(MF_VERSION, MFSTARTUP_NOSOCKET));
        }
        if (!startedMf) return;
        if (!hMfSensor) hMfSensor = LoadLibraryExW(L"mfsensorgroup.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        using CreateMonFn = HRESULT(WINAPI*)(IMFSensorActivitiesReportCallback*, IMFSensorActivityMonitor**);
        auto fnCreate = hMfSensor ? reinterpret_cast<CreateMonFn>(GetProcAddress(hMfSensor, "MFCreateSensorActivityMonitor")) : nullptr;
        if (!fnCreate) { Stop(); return; }
        callback = winrt::make_self<CameraSensorCallback>(wakeEvent, lifetime);
        if (FAILED(fnCreate(callback.get(), monitor.put())) || FAILED(monitor->Start())) {
            Stop();
        }
    }

    void Stop() {
        if (monitor) {
            monitor->Stop();
            winrt::com_ptr<IMFShutdown> sh;
            if (SUCCEEDED(monitor->QueryInterface(IID_PPV_ARGS(sh.put())))) sh->Shutdown();
            monitor = nullptr;
        }
        callback = nullptr;
        for (int i = 0; i < 30 && lifetime.use_count() > 1; ++i) Sleep(5);
        if (hMfSensor) { FreeLibrary(hMfSensor); hMfSensor = nullptr; }
        if (startedMf && fnShutdown) fnShutdown();
        startedMf = false;
        fnShutdown = nullptr;
        if (hMfPlat) { FreeLibrary(hMfPlat); hMfPlat = nullptr; }
    }

    bool QueryActiveCamera(std::wstring& outAppName) {
        Start();
        if (!callback) return false;
        std::set<DWORD> pids;
        {
            std::lock_guard<std::mutex> lock(callback->mutex);
            for (const auto& [dev, s] : callback->activeDevices) {
                pids.insert(s.begin(), s.end());
            }
        }
        if (pids.empty()) return false;
        for (DWORD pid : pids) {
            std::wstring app = GetFriendlyProcessNameByPid(pid);
            if (!app.empty()) {
                outAppName = app;
                break;
            }
        }
        return true;
    }
};

int QueryWnfQuietHoursProfile() {
    using NtQueryWnfStateDataFn = LONG(NTAPI*)(const WnfStateName*, const GUID*, const void*, ULONG*, void*, ULONG*);
    static auto fnQuery = reinterpret_cast<NtQueryWnfStateDataFn>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryWnfStateData"));
    if (!fnQuery) return -1;

    ULONG stamp = 0;
    DWORD profile = 0;
    ULONG cb = sizeof(profile);
    if (fnQuery(&kWnfQuietHoursStateName, nullptr, nullptr, &stamp, &profile, &cb) >= 0 &&
        cb == sizeof(DWORD)) {
        return static_cast<int>(profile);
    }
    return -1;
}

int QueryWnfLocationActiveSignal() {
    using NtQueryWnfStateDataFn = LONG(NTAPI*)(const WnfStateName*, const GUID*, const void*, ULONG*, void*, ULONG*);
    static auto fnQuery = reinterpret_cast<NtQueryWnfStateDataFn>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryWnfStateData"));
    if (!fnQuery) return -1;

    ULONG stamp = 0;
    BOOL active = FALSE;
    ULONG cb = sizeof(active);
    if (fnQuery(&kWnfLocationStateName, nullptr, nullptr, &stamp, &active, &cb) >= 0 &&
        cb == sizeof(BOOL) && (active == FALSE || active == TRUE)) {
        return active ? 1 : 0;
    }
    return -1;
}

void SampleConsentStorePrivacy(bool& ioMic, std::wstring& ioMicApp,
                               bool& ioCam, std::wstring& ioCamApp,
                               bool& ioLoc, std::wstring& ioLocApp) {
    if (QueryWnfLocationActiveSignal() == 1) {
        ioLoc = true;
    }

    FILETIME ftNow{};
    GetSystemTimeAsFileTime(&ftNow);
    uint64_t nowFt = (static_cast<uint64_t>(ftNow.dwHighDateTime) << 32) | ftNow.dwLowDateTime;
    uint64_t uptimeMs = GetTickCount64();

    std::set<std::wstring> runningProcs;
    bool snapTaken = false;
    auto ensureRunningProcs = [&]() {
        if (snapTaken) return;
        snapTaken = true;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe{};
            pe.dwSize = sizeof(pe);
            if (Process32FirstW(hSnap, &pe)) {
                do { runningProcs.insert(ToLowerStr(pe.szExeFile)); } while (Process32NextW(hSnap, &pe));
            }
            CloseHandle(hSnap);
        }
    };

    constexpr PCWSTR kConsentRoot = L"Software\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\";
    struct CapTarget { PCWSTR subkey; bool* flag; std::wstring* appOut; };
    CapTarget targets[3] = {
        { L"microphone", &ioMic, &ioMicApp },
        { L"webcam",     &ioCam, &ioCamApp },
        { L"location",   &ioLoc, &ioLocApp }
    };

    for (HKEY hive : { HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE }) {
        for (auto& t : targets) {
            if (*t.flag && !t.appOut->empty()) continue;
            std::wstring fullPath = std::wstring(kConsentRoot) + t.subkey;
            HKEY hCap = nullptr;
            if (RegOpenKeyExW(hive, fullPath.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &hCap) != ERROR_SUCCESS) {
                continue;
            }

            auto checkEntry = [&](HKEY hParent, const wchar_t* subName, bool isDesktop) {
                HKEY hApp = nullptr;
                if (RegOpenKeyExW(hParent, subName, 0, KEY_READ, &hApp) != ERROR_SUCCESS) return;
                uint64_t start = 0, stop = 0;
                DWORD cb = sizeof(start);
                bool hasStart = (RegGetValueW(hApp, nullptr, L"LastUsedTimeStart", RRF_RT_REG_QWORD, nullptr, &start, &cb) == ERROR_SUCCESS);
                cb = sizeof(stop);
                bool hasStop  = (RegGetValueW(hApp, nullptr, L"LastUsedTimeStop",  RRF_RT_REG_QWORD, nullptr, &stop,  &cb) == ERROR_SUCCESS);
                RegCloseKey(hApp);

                if (!hasStart || start == 0 || start > nowFt) return;
                if (hasStop && stop >= start) return;
                if ((nowFt - start) / 10000ULL > uptimeMs + 60000ULL) return;

                std::wstring friendly;
                if (isDesktop) {
                    std::wstring rawPath = subName;
                    std::replace(rawPath.begin(), rawPath.end(), L'#', L'\\');
                    size_t sl = rawPath.find_last_of(L'\\');
                    std::wstring exeName = (sl != std::wstring::npos) ? rawPath.substr(sl + 1) : rawPath;
                    if (exeName.empty()) return;
                    ensureRunningProcs();
                    if (runningProcs.find(ToLowerStr(exeName)) == runningProcs.end()) return;
                    friendly = LookupFriendlyExeDisplayName(exeName, rawPath.c_str());
                } else {
                    std::wstring pkg = subName;
                    size_t us = pkg.find(L'_');
                    if (us != std::wstring::npos) pkg.resize(us);
                    size_t dot = pkg.find_last_of(L'.');
                    if (dot != std::wstring::npos && dot + 1 < pkg.size()) pkg = pkg.substr(dot + 1);
                    friendly = pkg;
                }

                *t.flag = true;
                if (t.appOut->empty() && !friendly.empty()) {
                    *t.appOut = friendly;
                }
            };

            for (DWORD idx = 0; idx < 512; ++idx) {
                wchar_t keyName[512] = {};
                DWORD cch = ARRAYSIZE(keyName);
                if (RegEnumKeyExW(hCap, idx, keyName, &cch, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
                if (_wcsicmp(keyName, L"NonPackaged") != 0) {
                    checkEntry(hCap, keyName, false);
                } else {
                    HKEY hDesk = nullptr;
                    if (RegOpenKeyExW(hCap, L"NonPackaged", 0, KEY_READ, &hDesk) == ERROR_SUCCESS) {
                        for (DWORD n = 0; n < 512; ++n) {
                            wchar_t deskApp[1024] = {};
                            DWORD cchDesk = ARRAYSIZE(deskApp);
                            if (RegEnumKeyExW(hDesk, n, deskApp, &cchDesk, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
                            checkEntry(hDesk, deskApp, true);
                        }
                        RegCloseKey(hDesk);
                    }
                }
                if (*t.flag && !t.appOut->empty()) break;
            }
            RegCloseKey(hCap);
        }
    }
}

// ---------------------------------------------------------------------------
// 13. Bộ Đọc Hẹn Giờ (Timer) & Bấm Giờ (Stopwatch) Từ Ứng Dụng Đồng Hồ Windows
// ---------------------------------------------------------------------------
namespace ClockReader {

enum class Kind { Stopwatch = 0, Timer = 1 };
enum class RunState { Unknown, Paused, Running };

struct ParsedTime {
    int h = 0, m = 0, s = 0, ms = 0;
    bool hasSec = false;
};

bool ParseTimeText(std::wstring text, ParsedTime& out) {
    std::erase_if(text, [](wchar_t c) {
        return c == 0x200e || c == 0x200f || (c >= 0x2066 && c <= 0x2069) || (c >= 0x202a && c <= 0x202e);
    });
    for (auto& c : text) {
        if (c == L'：') c = L':';
        if (c == 0xa0 || c == 0x202f) c = L' ';
    }
    size_t colon = text.find(L':');
    if (colon == std::wstring::npos || colon == 0) return false;
    size_t start = colon;
    while (start > 0 && iswspace(text[start - 1])) --start;
    while (start > 0 && text[start - 1] >= L'0' && text[start - 1] <= L'9') --start;
    if (start == colon || text[start] < L'0' || text[start] > L'9') return false;

    size_t at = start;
    int vals[3]{};
    int cnt = 0;
    while (cnt < 3) {
        if (at >= text.size() || text[at] < L'0' || text[at] > L'9') return false;
        int v = 0, dig = 0;
        while (at < text.size() && text[at] >= L'0' && text[at] <= L'9') {
            if (++dig > 3) return false;
            v = v * 10 + (text[at++] - L'0');
        }
        vals[cnt++] = v;
        while (at < text.size() && iswspace(text[at])) ++at;
        if (at >= text.size() || text[at] != L':') break;
        if (cnt == 3) return false;
        ++at;
        while (at < text.size() && iswspace(text[at])) ++at;
    }
    if (cnt < 2 || vals[1] >= 60 || (cnt == 3 && vals[2] >= 60)) return false;
    out = { vals[0], vals[1], cnt == 3 ? vals[2] : 0, 0, cnt == 3 };
    if (at + 1 < text.size() && (text[at] == L'.' || text[at] == L',') && text[at + 1] >= L'0' && text[at + 1] <= L'9') {
        ++at;
        int place = 100;
        while (at < text.size() && text[at] >= L'0' && text[at] <= L'9') {
            if (place > 0) { out.ms += (text[at] - L'0') * place; place /= 10; }
            ++at;
        }
    }
    return true;
}

bool ParseDurationMs(std::wstring text, int64_t& outMs) {
    ParsedTime pt;
    if (ParseTimeText(text, pt)) {
        outMs = (pt.hasSec ? (int64_t(pt.h) * 3600 + pt.m * 60 + pt.s) : (int64_t(pt.h) * 60 + pt.m)) * 1000 + pt.ms;
        return true;
    }
    text = ToLowerStr(std::move(text));
    int64_t total = 0;
    bool found = false;
    unsigned units = 0;
    for (size_t pos = 0; pos < text.size(); ) {
        if (text[pos] < L'0' || text[pos] > L'9') { ++pos; continue; }
        size_t beg = pos;
        int64_t whole = 0;
        while (pos < text.size() && text[pos] >= L'0' && text[pos] <= L'9') {
            if (pos - beg > 6) return false;
            whole = whole * 10 + (text[pos++] - L'0');
        }
        while (pos < text.size() && iswspace(text[pos])) ++pos;
        size_t ws = pos;
        while (pos < text.size() && (iswalpha(text[pos]) || (text[pos] >= 0x80 && !iswspace(text[pos])))) ++pos;
        auto unit = text.substr(ws, pos - ws);
        int64_t scale = 0;
        unsigned bit = 0;
        if (unit == L"hour" || unit == L"hours" || unit == L"hr" || unit == L"hrs" || unit == L"giờ") { scale = 3600000; bit = 1; }
        else if (unit == L"minute" || unit == L"minutes" || unit == L"min" || unit == L"mins" || unit == L"phút") { scale = 60000; bit = 2; }
        else if (unit == L"second" || unit == L"seconds" || unit == L"sec" || unit == L"secs" || unit == L"giây") { scale = 1000; bit = 4; }
        else continue;
        if (units & bit) return false;
        units |= bit;
        found = true;
        total += whole * scale;
    }
    if (!found || total > 999LL * 3600000) return false;
    outMs = total;
    return true;
}

bool HasAnySub(const std::wstring& s, std::initializer_list<PCWSTR> words) {
    for (PCWSTR w : words) {
        if (s.find(w) != std::wstring::npos) return true;
    }
    return false;
}

bool IsClockProcess(DWORD pid) {
    using GetPackageFamilyNameFn = LONG(WINAPI*)(HANDLE, UINT32*, PWSTR);
    static auto fnGetPkg = reinterpret_cast<GetPackageFamilyNameFn>(
        GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "GetPackageFamilyName"));
    if (!fnGetPkg || pid == 0) return false;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;
    wchar_t fam[256] = {};
    UINT32 cch = ARRAYSIZE(fam);
    LONG st = fnGetPkg(hProc, &cch, fam);
    CloseHandle(hProc);
    return (st == ERROR_SUCCESS && _wcsicmp(fam, L"Microsoft.WindowsAlarms_8wekyb3d8bbwe") == 0);
}

struct ClockWndInfo {
    HWND elem = nullptr;
    HWND top = nullptr;
    DWORD pid = 0;
};

ClockWndInfo LocateClockWindow() {
    struct ScanCtx {
        ClockWndInfo found;
        std::map<DWORD, bool> pidCache;
        unsigned count = 0;
    } ctx;

    struct ClockEnumCb {
        struct ChildCtx { ScanCtx* parent; HWND top; unsigned cnt = 0; };
        static BOOL CALLBACK ChildProc(HWND ch, LPARAM clp) {
            auto& x = *reinterpret_cast<ChildCtx*>(clp);
            if (++x.cnt > 48) return FALSE;
            DWORD cpid = 0;
            GetWindowThreadProcessId(ch, &cpid);
            auto [cit, cins] = x.parent->pidCache.try_emplace(cpid, false);
            if (cins) cit->second = IsClockProcess(cpid);
            if (cit->second) {
                x.parent->found = { ch, x.top, cpid };
                return FALSE;
            }
            return TRUE;
        }
        static BOOL CALLBACK TopProc(HWND w, LPARAM lp) {
            auto& c = *reinterpret_cast<ScanCtx*>(lp);
            if (++c.count > 400) return FALSE;
            if (!IsWindowVisible(w)) return TRUE;
            DWORD pid = 0;
            GetWindowThreadProcessId(w, &pid);
            auto [it, ins] = c.pidCache.try_emplace(pid, false);
            if (ins) it->second = IsClockProcess(pid);
            if (it->second) {
                c.found = { w, w, pid };
                return FALSE;
            }
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, ARRAYSIZE(cls));
            if (_wcsicmp(cls, L"ApplicationFrameWindow") != 0) return TRUE;
            ChildCtx cc{ &c, w };
            EnumChildWindows(w, ChildProc, reinterpret_cast<LPARAM>(&cc));
            return c.found.elem ? FALSE : TRUE;
        }
    };
    EnumWindows(ClockEnumCb::TopProc, reinterpret_cast<LPARAM>(&ctx));

    return ctx.found;
}

struct Node {
    std::wstring id, name, value, runtimeKey;
    int parent = -1, type = 0, toggle = -1;
    bool selected = false, enabled = false, offscreen = true;
    RECT rect{};
};

int CachedIntProp(IUIAutomationElement* el, PROPERTYID prop) {
    VARIANT v{};
    VariantInit(&v);
    int res = -1;
    if (SUCCEEDED(el->GetCachedPropertyValue(prop, &v))) {
        if (v.vt == VT_BOOL) res = (v.boolVal != VARIANT_FALSE) ? 1 : 0;
        else if (v.vt == VT_I4) res = v.lVal;
    }
    VariantClear(&v);
    return res;
}

std::wstring CachedStrProp(IUIAutomationElement* el, PROPERTYID prop) {
    VARIANT v{};
    VariantInit(&v);
    std::wstring res;
    if (SUCCEEDED(el->GetCachedPropertyValue(prop, &v))) {
        if (v.vt == VT_BSTR && v.bstrVal) {
            res.assign(v.bstrVal, std::min<UINT>(SysStringLen(v.bstrVal), 384));
        } else if (v.vt == (VT_ARRAY | VT_I4) && v.parray && SafeArrayGetDim(v.parray) == 1) {
            LONG lb = 0, ub = -1;
            if (SUCCEEDED(SafeArrayGetLBound(v.parray, 1, &lb)) &&
                SUCCEEDED(SafeArrayGetUBound(v.parray, 1, &ub)) && (ub - lb) < 24) {
                for (LONG i = lb; i <= ub; ++i) {
                    LONG part = 0;
                    if (SUCCEEDED(SafeArrayGetElement(v.parray, &i, &part))) {
                        res += std::to_wstring(part) + L".";
                    }
                }
            }
        }
    }
    VariantClear(&v);
    return res;
}

bool NodeUnder(const std::vector<Node>& nodes, int ch, int par) {
    for (int i = ch, d = 0; i >= 0 && i < static_cast<int>(nodes.size()) && d < 28; i = nodes[i].parent, ++d) {
        if (i == par) return true;
    }
    return false;
}

struct ObservedItem {
    std::wstring key;
    std::wstring label;
    Kind kind = Kind::Stopwatch;
    RunState run = RunState::Unknown;
    int64_t ms = 0;
    bool finished = false;
};

int DetectPage(const std::vector<Node>& nodes) {
    for (const auto& n : nodes) {
        if (!n.offscreen && (n.selected || n.toggle == 1)) {
            auto nm = ToLowerStr(n.name);
            if (HasAnySub(nm, { L"stopwatch", L"bấm giờ" }) || n.id.find(L"stopwatch") != std::wstring::npos) return 0;
            if (HasAnySub(nm, { L"timer", L"hẹn giờ" }) || n.id == L"timerbutton") return 1;
            if (HasAnySub(nm, { L"alarm", L"báo thức" }) || n.id == L"alarmbutton") return 2;
        }
    }
    for (const auto& n : nodes) {
        if (!n.offscreen) {
            if (HasAnySub(n.id, { L"stopwatchtimer", L"stopwatchdisplay", L"stopwatchplaypause", L"stopwatchreset" })) return 0;
            if (HasAnySub(n.id, { L"timertime", L"countdown", L"timerdisplay", L"timercontrol", L"timerplaypause", L"timerpause" })) return 1;
        }
    }
    return -1;
}

bool IsTransportCtrl(const Node& n) {
    if (n.offscreen || !n.enabled ||
        (n.type != UIA_ButtonControlTypeId && n.type != UIA_CheckBoxControlTypeId && n.type != UIA_CustomControlTypeId)) {
        return false;
    }
    auto nm = ToLowerStr(n.name);
    return HasAnySub(n.id, { L"playpause", L"pausebutton", L"startbutton", L"resumebutton" }) ||
           HasAnySub(nm, { L"pause", L"resume", L"tạm dừng", L"tiếp tục", L"bắt đầu" }) ||
           nm == L"start" || nm == L"stop" || nm == L"dừng";
}

std::vector<ObservedItem> ExtractClockEntries(IUIAutomation* uia, HWND hClockElem, int& outPage) {
    outPage = -1;
    std::vector<ObservedItem> out;
    if (!uia || !hClockElem) return out;

    ULONGLONG deadline = GetTickCount64() + 900;
    winrt::com_ptr<IUIAutomationCacheRequest> cache;
    if (FAILED(uia->CreateCacheRequest(cache.put()))) return out;
    for (PROPERTYID pid : {
            UIA_NamePropertyId, UIA_AutomationIdPropertyId, UIA_ControlTypePropertyId,
            UIA_IsOffscreenPropertyId, UIA_IsEnabledPropertyId, UIA_BoundingRectanglePropertyId,
            UIA_SelectionItemIsSelectedPropertyId, UIA_ToggleToggleStatePropertyId,
            UIA_ValueValuePropertyId, UIA_RuntimeIdPropertyId }) {
        cache->AddProperty(pid);
    }
    cache->put_TreeScope(TreeScope_Element);
    winrt::com_ptr<IUIAutomationCondition> rawCond;
    if (SUCCEEDED(uia->get_RawViewCondition(rawCond.put())) && rawCond) {
        cache->put_TreeFilter(rawCond.get());
    }
    winrt::com_ptr<IUIAutomationElement> root;
    if (FAILED(uia->ElementFromHandleBuildCache(hClockElem, cache.get(), root.put())) || !root) return out;
    winrt::com_ptr<IUIAutomationTreeWalker> walker;
    if (FAILED(uia->get_RawViewWalker(walker.put())) || !walker) return out;

    struct QItem { winrt::com_ptr<IUIAutomationElement> el; int parent; unsigned depth; };
    std::vector<QItem> q;
    q.push_back({ std::move(root), -1, 0 });
    std::vector<Node> nodes;
    nodes.reserve(256);

    for (size_t i = 0; i < q.size() && i < 420 && GetTickCount64() < deadline; ++i) {
        auto cur = q[i];
        Node nd;
        nd.parent = cur.parent;
        auto* el = cur.el.get();
        nd.name = CachedStrProp(el, UIA_NamePropertyId);
        nd.id = ToLowerStr(CachedStrProp(el, UIA_AutomationIdPropertyId));
        nd.value = CachedStrProp(el, UIA_ValueValuePropertyId);
        nd.runtimeKey = CachedStrProp(el, UIA_RuntimeIdPropertyId);
        nd.type = CachedIntProp(el, UIA_ControlTypePropertyId);
        nd.offscreen = CachedIntProp(el, UIA_IsOffscreenPropertyId) != 0;
        nd.enabled = CachedIntProp(el, UIA_IsEnabledPropertyId) == 1;
        nd.selected = CachedIntProp(el, UIA_SelectionItemIsSelectedPropertyId) == 1;
        nd.toggle = CachedIntProp(el, UIA_ToggleToggleStatePropertyId);
        el->get_CachedBoundingRectangle(&nd.rect);

        bool omit = (nd.id.find(L"lapsandsplits") != std::wstring::npos || nd.id.find(L"laplist") != std::wstring::npos);
        int idx = static_cast<int>(nodes.size());
        nodes.push_back(std::move(nd));
        if (omit || (i > 0 && nodes.back().offscreen) || cur.depth >= 22) continue;

        winrt::com_ptr<IUIAutomationElement> ch;
        if (FAILED(walker->GetFirstChildElementBuildCache(el, cache.get(), ch.put()))) continue;
        while (ch && q.size() < 420 && GetTickCount64() < deadline) {
            q.push_back({ ch, idx, cur.depth + 1 });
            winrt::com_ptr<IUIAutomationElement> nxt;
            if (FAILED(walker->GetNextSiblingElementBuildCache(ch.get(), cache.get(), nxt.put()))) break;
            ch = std::move(nxt);
        }
    }

    int page = DetectPage(nodes);
    outPage = page;
    if (page != 0 && page != 1) return out;

    for (const auto& n : nodes) {
        if (!n.offscreen && n.type == UIA_ButtonControlTypeId &&
            HasAnySub(n.id, { L"savetimer", L"timersave", L"savebutton" })) {
            return out;
        }
    }

    auto findRow = [&](int index) -> int {
        if (page == 0) return 0;
        for (int p = index, d = 0; p >= 0 && d < 22; p = nodes[p].parent, ++d) {
            if (nodes[p].type == UIA_ListItemControlTypeId || nodes[p].type == UIA_DataItemControlTypeId ||
                HasAnySub(nodes[p].id, { L"timercard", L"timeritem", L"timercontrol" })) {
                return p;
            }
        }
        for (int p = index, d = 0; p >= 0 && d < 22; p = nodes[p].parent, ++d) {
            for (int c = 0; c < static_cast<int>(nodes.size()); ++c) {
                if (IsTransportCtrl(nodes[c]) && NodeUnder(nodes, c, p)) return p;
            }
        }
        return -1;
    };

    auto isEndOrTitle = [&](int index, int row) -> bool {
        for (int p = index, d = 0; p >= 0 && p != row && d < 22; p = nodes[p].parent, ++d) {
            if (HasAnySub(nodes[p].id, { L"timername", L"durationlabel", L"originalduration", L"initialduration",
                                         L"endtime", L"finishtime", L"completiontime", L"targettime" })) {
                return true;
            }
        }
        auto nm = ToLowerStr(nodes[index].name);
        return HasAnySub(nm, { L"ends at", L"finishes at", L"kết thúc lúc", L"hết giờ lúc" });
    };

    auto readoutStr = [&](int index) -> std::wstring {
        const auto& nd = nodes[index];
        if (!nd.value.empty()) return nd.value;
        int64_t tmp = 0;
        if (ParseDurationMs(nd.name, tmp)) return nd.name;
        std::wstring joined;
        unsigned cnt = 0;
        for (int k = 0; k < static_cast<int>(nodes.size()); ++k) {
            const auto& part = nodes[k];
            if (part.offscreen || part.parent != index || part.type != UIA_TextControlTypeId) continue;
            auto txt = part.value.empty() ? part.name : part.value;
            bool frag = !txt.empty() && std::all_of(txt.begin(), txt.end(), [](wchar_t c) {
                return iswdigit(c) || iswspace(c) || c == L':' || c == L'：' || c == L'.' || c == L',' || c == 0x200e || c == 0x200f;
            });
            if (!frag || ++cnt > 12) return nd.name;
            joined += txt;
        }
        return cnt >= 3 ? joined : nd.name;
    };

    struct Cand { int node; int64_t durMs; long long score; };
    std::map<int, Cand> bestByRow;

    for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
        const auto& n = nodes[i];
        if (n.offscreen) continue;
        if (n.type != UIA_TextControlTypeId && n.type != UIA_CustomControlTypeId &&
            n.type != UIA_GroupControlTypeId && n.type != UIA_PaneControlTypeId &&
            n.type != UIA_ListItemControlTypeId && n.type != UIA_DataItemControlTypeId) {
            continue;
        }
        int row = findRow(i);
        if (row < 0 || isEndOrTitle(i, row)) continue;

        auto txt = readoutStr(i);
        ParsedTime pt;
        int64_t durMs = 0;
        bool hasColon = ParseTimeText(txt, pt);
        if (!ParseDurationMs(txt, durMs)) continue;

        bool durHint = HasAnySub(n.id, { L"remaining", L"countdown", L"stopwatchtime", L"stopwatchdisplay" }) ||
                       HasAnySub(ToLowerStr(n.name), { L"remaining", L"còn lại" });
        if (page == 1 && hasColon && !pt.hasSec && !durHint) continue;

        long long area = std::min<long long>(99999,
            std::max(0L, n.rect.right - n.rect.left) * static_cast<long long>(std::max(0L, n.rect.bottom - n.rect.top)));
        bool secWord = HasAnySub(ToLowerStr(txt), { L"second", L" sec", L"giây" });
        long long score = area + ((hasColon && pt.hasSec) || (!hasColon && secWord) ? 8000000LL : (hasColon ? 4000000LL : 1000000LL));
        if (!n.value.empty()) score += 100000LL;
        if (durHint || HasAnySub(n.id, { L"timertext", L"timedisplay", L"readout" })) score += 200000LL;

        auto it = bestByRow.find(row);
        if (it == bestByRow.end() || it->second.score < score) {
            bestByRow[row] = { i, durMs, score };
        }
    }

    for (const auto& [row, cand] : bestByRow) {
        ObservedItem item;
        item.kind = (page == 0) ? Kind::Stopwatch : Kind::Timer;
        item.ms = cand.durMs;
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            if (!nodes[i].offscreen && NodeUnder(nodes, i, row) &&
                HasAnySub(nodes[i].id, { L"timername" }) && nodes[i].type == UIA_TextControlTypeId) {
                item.label = nodes[i].name.substr(0, 80);
                break;
            }
        }
        item.key = (page == 0)
            ? L"stopwatch"
            : (!nodes[row].runtimeKey.empty() ? nodes[row].runtimeKey : (nodes[row].id + L":" + item.label + L":" + std::to_wstring(row)));

        bool hasPause = false, hasStart = false;
        std::wstring rowAllText;
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            const auto& n = nodes[i];
            if (n.offscreen || !NodeUnder(nodes, i, row)) continue;
            if (n.type == UIA_TextControlTypeId && rowAllText.size() < 1024) rowAllText += ToLowerStr(n.name) + L" ";
            if (!IsTransportCtrl(n)) continue;
            auto nm = ToLowerStr(n.name);
            bool p = HasAnySub(nm, { L"pause", L"tạm dừng" }) || nm == L"stop" || nm == L"dừng";
            bool s = HasAnySub(nm, { L"start", L"resume", L"bắt đầu", L"tiếp tục" });
            if (p || s) { hasPause |= p; hasStart |= s; continue; }
            if (n.id.find(L"playpause") == std::wstring::npos) {
                hasPause |= HasAnySub(n.id, { L"pausebutton" });
                hasStart |= HasAnySub(n.id, { L"startbutton", L"resumebutton" });
            }
        }
        item.run = (hasPause == hasStart) ? RunState::Unknown : (hasPause ? RunState::Running : RunState::Paused);
        item.finished = (page == 1 && cand.durMs == 0 &&
                         HasAnySub(rowAllText, { L"time's up", L"timer finished", L"hết giờ", L"đã hết giờ" }));
        out.push_back(std::move(item));
    }
    return out;
}

} // namespace ClockReader

DWORD WINAPI ClockMonitorThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    winrt::com_ptr<IUIAutomation> uia;
    {
        winrt::com_ptr<IUIAutomation2> uia2;
        if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
                                       __uuidof(IUIAutomation2), uia2.put_void())) && uia2) {
            uia2->put_ConnectionTimeout(200);
            uia2->put_TransactionTimeout(200);
            uia2->put_AutoSetFocus(FALSE);
            uia = uia2;
        } else {
            CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
                             __uuidof(IUIAutomation), uia.put_void());
        }
    }

    struct TrackedTimer {
        std::wstring key;
        std::wstring label;
        int64_t baseMs = 0;
        ULONGLONG sampleTick = 0;
        bool running = false;
        bool finishedNotified = false;
    };

    std::vector<TrackedTimer> timers;
    bool swRunning = false;
    int64_t swBaseMs = 0;
    ULONGLONG swSampleTick = 0;

    ClockReader::ClockWndInfo cachedWnd;
    ULONGLONG nextFindTick = 0;
    bool bootstrapped = false;

    while (g_running.load(std::memory_order_relaxed)) {
        ULONGLONG now = GetTickCount64();
        bool enabled = g_enableClockBadgesAtomic.load(std::memory_order_relaxed);

        if (!enabled) {
            timers.clear();
            swRunning = false;
            bool changed = false;
            {
                std::lock_guard<std::mutex> lock(g_persistMutex);
                if (g_persistIndicators.clock.timerRunning || g_persistIndicators.clock.stopwatchRunning) {
                    g_persistIndicators.clock = {};
                    changed = true;
                }
            }
            if (changed && g_alertHwnd && IsWindow(g_alertHwnd)) {
                PostMessageW(g_alertHwnd, WM_APP_INDICATORS_CHANGED, 0, 0);
            }
            if (WaitForSingleObject(g_stopEvent, 2000) == WAIT_OBJECT_0) break;
            continue;
        }

        if (now >= nextFindTick || (cachedWnd.elem && !IsWindow(cachedWnd.elem))) {
            cachedWnd = ClockReader::LocateClockWindow();
            nextFindTick = now + 2000;
        }
        if (cachedWnd.elem && (!IsWindow(cachedWnd.elem) || !IsWindowVisible(cachedWnd.top))) {
            cachedWnd = {};
            nextFindTick = 0;
        }

        bool appVisible = cachedWnd.elem && !IsIconic(cachedWnd.top);
        int observedPage = -1;
        std::vector<ClockReader::ObservedItem> obs;
        if (appVisible && uia) {
            DWORD pid = 0;
            GetWindowThreadProcessId(cachedWnd.elem, &pid);
            if (pid == cachedWnd.pid) {
                obs = ClockReader::ExtractClockEntries(uia.get(), cachedWnd.elem, observedPage);
                now = GetTickCount64();
            } else {
                cachedWnd = {};
                appVisible = false;
            }
        }

        bool vi = g_useViAtomic.load(std::memory_order_relaxed);

        if (appVisible && observedPage == 0) {
            bool seenRunningSw = false;
            for (const auto& o : obs) {
                if (o.kind != ClockReader::Kind::Stopwatch) continue;
                bool nowRun = (o.run == ClockReader::RunState::Running);
                if (o.run == ClockReader::RunState::Unknown && swRunning) {
                    int64_t expected = swBaseMs + static_cast<int64_t>(now - swSampleTick);
                    if (std::llabs(o.ms - expected) <= 2500 && o.ms > swBaseMs) nowRun = true;
                }
                if (nowRun) {
                    seenRunningSw = true;
                    bool newlyStarted = !swRunning;
                    swRunning = true;
                    swBaseMs = o.ms;
                    swSampleTick = now;
                    if (newlyStarted && bootstrapped) {
                        g_cornerSelectedKind = CornerItemKind::Stopwatch;
                        TriggerLiveAlert(
                            AlertKind::ClockAlert, L"\uE916",
                            PickText(vi, L"Đang bấm giờ", L"Stopwatch Started"),
                            PickText(vi, L"Đồng hồ Windows", L"Windows Clock"),
                            FormatClockCompactTime(o.ms / 1000), -1, false, true);
                    }
                }
            }
            if (!seenRunningSw) {
                swRunning = false;
                swBaseMs = 0;
            }
        } else if (!cachedWnd.top || !IsWindow(cachedWnd.top)) {
            bool anyClockProc = false;
            HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnap != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32W pe{};
                pe.dwSize = sizeof(pe);
                if (Process32FirstW(hSnap, &pe)) {
                    do {
                        if (_wcsicmp(pe.szExeFile, L"Time.exe") == 0 && ClockReader::IsClockProcess(pe.th32ProcessID)) {
                            anyClockProc = true;
                            break;
                        }
                    } while (Process32NextW(hSnap, &pe));
                }
                CloseHandle(hSnap);
            }
            if (!anyClockProc) {
                swRunning = false;
                swBaseMs = 0;
            }
        }

        if (appVisible && observedPage == 1) {
            std::vector<TrackedTimer> nextTimers;
            for (const auto& o : obs) {
                if (o.kind != ClockReader::Kind::Timer) continue;
                auto itOld = std::find_if(timers.begin(), timers.end(), [&](const TrackedTimer& t) {
                    return t.key == o.key || (!o.label.empty() && t.label == o.label);
                });

                bool nowRun = (o.run == ClockReader::RunState::Running);
                if (o.run == ClockReader::RunState::Unknown && itOld != timers.end() && itOld->running) {
                    int64_t prevRem = std::max<int64_t>(0, itOld->baseMs - static_cast<int64_t>(now - itOld->sampleTick));
                    if (o.ms < itOld->baseMs || std::llabs(o.ms - prevRem) <= 2500) nowRun = true;
                }

                if (o.finished) {
                    if (itOld == timers.end() || !itOld->finishedNotified) {
                        TriggerLiveAlert(
                            AlertKind::ClockAlert, L"\uEA8F",
                            PickText(vi, L"Đã hết giờ hẹn!", L"Timer Finished!"),
                            o.label.empty() ? PickText(vi, L"Đồng hồ hẹn giờ", L"Windows Timer") : o.label,
                            L"00:00", -1, true, false);
                    }
                    continue;
                }

                if (nowRun && o.ms > 0) {
                    bool wasRunning = (itOld != timers.end() && itOld->running);
                    nextTimers.push_back({ o.key, o.label, o.ms, now, true, false });
                    if (!wasRunning && bootstrapped) {
                        g_cornerSelectedKind = CornerItemKind::Timer;
                        TriggerLiveAlert(
                            AlertKind::ClockAlert, L"\uE916",
                            PickText(vi, L"Đã bật hẹn giờ", L"Timer Started"),
                            o.label.empty() ? PickText(vi, L"Đang đếm ngược", L"Countdown active") : o.label,
                            FormatClockCompactTime((o.ms + 999) / 1000), -1, false, true);
                    }
                }
            }
            timers = std::move(nextTimers);
        }

        for (auto it = timers.begin(); it != timers.end(); ) {
            int64_t remMs = it->baseMs - static_cast<int64_t>(now >= it->sampleTick ? (now - it->sampleTick) : 0);
            if (remMs <= 0) {
                TriggerLiveAlert(
                    AlertKind::ClockAlert, L"\uEA8F",
                    PickText(vi, L"Đã hết giờ hẹn!", L"Timer Finished!"),
                    it->label.empty() ? PickText(vi, L"Đồng hồ hẹn giờ", L"Windows Timer") : it->label,
                    L"00:00", -1, true, false);
                it = timers.erase(it);
            } else {
                ++it;
            }
        }

        bootstrapped = true;

        ActiveClockDisplayInfo nextInfo{};
        nextInfo.clockTopHwnd = cachedWnd.top;
        if (!timers.empty()) {
            auto bestIt = std::min_element(timers.begin(), timers.end(), [&](const TrackedTimer& a, const TrackedTimer& b) {
                int64_t ra = a.baseMs - static_cast<int64_t>(now - a.sampleTick);
                int64_t rb = b.baseMs - static_cast<int64_t>(now - b.sampleTick);
                return ra < rb;
            });
            nextInfo.timerRunning = true;
            nextInfo.timerBaseMs = bestIt->baseMs;
            nextInfo.timerSampleTick = bestIt->sampleTick;
            nextInfo.timerLabel = bestIt->label;
        }
        if (swRunning) {
            nextInfo.stopwatchRunning = true;
            nextInfo.stopwatchBaseMs = swBaseMs;
            nextInfo.stopwatchSampleTick = swSampleTick;
        }

        bool stateChanged = false;
        {
            std::lock_guard<std::mutex> lock(g_persistMutex);
            const auto& prev = g_persistIndicators.clock;
            if (prev.timerRunning != nextInfo.timerRunning ||
                prev.stopwatchRunning != nextInfo.stopwatchRunning ||
                prev.clockTopHwnd != nextInfo.clockTopHwnd ||
                (nextInfo.timerRunning && std::llabs(prev.CurrentTimerSeconds(now) - nextInfo.CurrentTimerSeconds(now)) >= 1) ||
                (nextInfo.stopwatchRunning && std::llabs(prev.CurrentStopwatchSeconds(now) - nextInfo.CurrentStopwatchSeconds(now)) >= 1)) {
                g_persistIndicators.clock = nextInfo;
                stateChanged = true;
            } else {
                g_persistIndicators.clock = nextInfo;
            }
        }
        if (stateChanged && g_alertHwnd && IsWindow(g_alertHwnd)) {
            PostMessageW(g_alertHwnd, WM_APP_INDICATORS_CHANGED, 0, 0);
        }

        DWORD sleepMs = appVisible ? 320 : ((nextInfo.timerRunning || nextInfo.stopwatchRunning) ? 900 : 1800);
        if (WaitForSingleObject(g_stopEvent, sleepMs) == WAIT_OBJECT_0) break;
    }

    uia = nullptr;
    if (SUCCEEDED(hrCom)) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// 14. Các Bộ Nhận Tín Hiệu Âm lượng, USB, Xóa tệp, Mạng, Bluetooth, RAM/ROM & WNF Tức Thì
// ---------------------------------------------------------------------------
class VolumeCallbackClient : public IAudioEndpointVolumeCallback {
    LONG _ref = 1;
    bool _init = false;

public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppv = static_cast<IAudioEndpointVolumeCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&_ref);
        if (r == 0) delete this;
        return r;
    }
    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) override {
        if (!pNotify) return E_INVALIDARG;
        if (!_init) {
            _init = true;
            return S_OK;
        }
        if (IsEqualGUID(pNotify->guidEventContext, kModVolumeSetContextGuid)) {
            return S_OK;
        }
        if (g_enableVolumeAlertAtomic.load(std::memory_order_relaxed)) {
            int pct = std::clamp(static_cast<int>(std::lround(pNotify->fMasterVolume * 100.0f)), 0, 100);
            ShowCurrentVolumeAlert(pct, pNotify->bMuted != FALSE);
        }
        return S_OK;
    }
};

static VolumeCallbackClient* g_pVolClient = nullptr;

void RebindDefaultRenderVolumeEndpoint() {
    if (g_pEndVol && g_pVolClient) {
        g_pEndVol->UnregisterControlChangeNotify(g_pVolClient);
    }
    if (g_pVolClient) { g_pVolClient->Release(); g_pVolClient = nullptr; }
    if (g_pEndVol)    { g_pEndVol->Release();    g_pEndVol = nullptr; }
    if (g_pAudioDev)  { g_pAudioDev->Release();  g_pAudioDev = nullptr; }

    if (!g_pAudioEnum) {
        CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                         __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&g_pAudioEnum));
    }
    if (g_pAudioEnum) {
        if (SUCCEEDED(g_pAudioEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &g_pAudioDev)) && g_pAudioDev) {
            if (SUCCEEDED(g_pAudioDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER,
                                                nullptr, reinterpret_cast<void**>(&g_pEndVol))) && g_pEndVol) {
                g_pVolClient = new VolumeCallbackClient();
                g_pEndVol->RegisterControlChangeNotify(g_pVolClient);
                AUDIO_VOLUME_NOTIFICATION_DATA initData{};
                float v = 0.0f;
                BOOL m = FALSE;
                g_pEndVol->GetMasterVolumeLevelScalar(&v);
                g_pEndVol->GetMute(&m);
                initData.fMasterVolume = v;
                initData.bMuted = m;
                g_pVolClient->OnNotify(&initData);
            }
        }
    }
}

bool IsUserFacingFilePath(const wchar_t* path) {
    if (!path || !path[0]) return false;
    if (StrStrIW(path, L"\\AppData\\") ||
        StrStrIW(path, L"\\ProgramData\\") ||
        StrStrIW(path, L"\\Windows\\") ||
        StrStrIW(path, L"\\Temp\\") ||
        StrStrIW(path, L"\\tmp\\") ||
        StrStrIW(path, L"\\$Recycle.Bin\\") ||
        StrStrIW(path, L"\\System Volume Information\\") ||
        StrStrIW(path, L"\\.git\\") ||
        StrStrIW(path, L"\\node_modules\\") ||
        StrStrIW(path, L"\\__pycache__\\")) {
        return false;
    }
    const wchar_t* fname = PathFindFileNameW(path);
    if (!fname || !fname[0]) return false;
    if (fname[0] == L'~' || _wcsicmp(fname, L"desktop.ini") == 0 || _wcsicmp(fname, L"thumbs.db") == 0) {
        return false;
    }
    const wchar_t* ext = PathFindExtensionW(fname);
    if (ext && (_wcsicmp(ext, L".tmp") == 0 || _wcsicmp(ext, L".temp") == 0 ||
                _wcsicmp(ext, L".crdownload") == 0 || _wcsicmp(ext, L".part") == 0 ||
                _wcsicmp(ext, L".lock") == 0 || _wcsicmp(ext, L".etl") == 0 ||
                _wcsicmp(ext, L".log") == 0)) {
        return false;
    }
    return true;
}

static std::map<std::wstring, std::wstring> g_usbKnownNames;

std::wstring LookupUsbInterfaceFriendlyName(const wchar_t* deviceInterfacePath) {
    if (!deviceInterfacePath || !deviceInterfacePath[0]) return {};
    HDEVINFO hInfo = SetupDiCreateDeviceInfoList(nullptr, nullptr);
    if (hInfo == INVALID_HANDLE_VALUE) return {};

    std::wstring result;
    SP_DEVICE_INTERFACE_DATA ifData{};
    ifData.cbSize = sizeof(ifData);
    if (SetupDiOpenDeviceInterfaceW(hInfo, deviceInterfacePath, 0, &ifData)) {
        DWORD reqSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hInfo, &ifData, nullptr, 0, &reqSize, nullptr);
        if (reqSize >= sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W)) {
            std::vector<BYTE> buf(reqSize, 0);
            auto* detail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buf.data());
            detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
            SP_DEVINFO_DATA devData{};
            devData.cbSize = sizeof(devData);
            if (SetupDiGetDeviceInterfaceDetailW(hInfo, &ifData, detail, reqSize, nullptr, &devData)) {
                wchar_t nameBuf[256] = {};
                if (SetupDiGetDeviceRegistryPropertyW(hInfo, &devData, SPDRP_FRIENDLYNAME, nullptr,
                        reinterpret_cast<PBYTE>(nameBuf), sizeof(nameBuf) - sizeof(wchar_t), nullptr) && nameBuf[0]) {
                    result = nameBuf;
                } else if (SetupDiGetDeviceRegistryPropertyW(hInfo, &devData, SPDRP_DEVICEDESC, nullptr,
                        reinterpret_cast<PBYTE>(nameBuf), sizeof(nameBuf) - sizeof(wchar_t), nullptr) && nameBuf[0]) {
                    result = nameBuf;
                }
            }
        }
    }
    SetupDiDestroyDeviceInfoList(hInfo);
    return result;
}

void PrepopulateConnectedUsbNames() {
    HDEVINFO hInfo = SetupDiGetClassDevsW(&kGuidDevInterfaceUsbDevice, nullptr, nullptr,
                                          DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hInfo == INVALID_HANDLE_VALUE) return;

    SP_DEVICE_INTERFACE_DATA ifData{};
    ifData.cbSize = sizeof(ifData);
    for (DWORD i = 0; i < 256 && SetupDiEnumDeviceInterfaces(hInfo, nullptr, &kGuidDevInterfaceUsbDevice, i, &ifData); ++i) {
        DWORD reqSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hInfo, &ifData, nullptr, 0, &reqSize, nullptr);
        if (reqSize < sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W)) continue;
        std::vector<BYTE> buf(reqSize, 0);
        auto* detail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buf.data());
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        SP_DEVINFO_DATA devData{};
        devData.cbSize = sizeof(devData);
        if (SetupDiGetDeviceInterfaceDetailW(hInfo, &ifData, detail, reqSize, nullptr, &devData)) {
            wchar_t nameBuf[256] = {};
            if ((SetupDiGetDeviceRegistryPropertyW(hInfo, &devData, SPDRP_FRIENDLYNAME, nullptr,
                    reinterpret_cast<PBYTE>(nameBuf), sizeof(nameBuf) - sizeof(wchar_t), nullptr) && nameBuf[0]) ||
                (SetupDiGetDeviceRegistryPropertyW(hInfo, &devData, SPDRP_DEVICEDESC, nullptr,
                    reinterpret_cast<PBYTE>(nameBuf), sizeof(nameBuf) - sizeof(wchar_t), nullptr) && nameBuf[0])) {
                g_usbKnownNames[ToLowerStr(detail->DevicePath)] = nameBuf;
            }
        }
    }
    SetupDiDestroyDeviceInfoList(hInfo);
}

static std::atomic<int> g_win11FocusSessionState{-1};
static std::atomic<ULONGLONG> g_lastFocusTransitionTick{0};

struct MemoryAlertsTracker {
    unsigned fired = 0;
    int Observe(unsigned percent) {
        int level = 0;
        constexpr unsigned levels[] = { 80, 90, 95 };
        for (unsigned i = 0; i < 3; ++i) {
            if (percent + 3 < levels[i]) fired &= ~(1u << i);
            if (percent >= levels[i] && !(fired & (1u << i))) {
                fired |= (1u << i);
                level = static_cast<int>(levels[i]);
            }
        }
        return level;
    }
};

// Bộ theo dõi Mốc Sạc Pin (80%, 90%, 100%) & Mốc Pin Yếu khi Xả (50%, 20%, 10%, 5%)
struct BatteryMilestoneTracker {
    unsigned chargeFiredMask = 0;    // bit 0: 80%, bit 1: 90%, bit 2: 100%
    unsigned dischargeFiredMask = 0; // bit 0: 50%, bit 1: 20%, bit 2: 10%, bit 3: 5%
    bool bootstrapped = false;

    void SeedInitialState(bool acPlugged, int pct) {
        pct = std::clamp(pct, 0, 100);
        chargeFiredMask = 0;
        dischargeFiredMask = 0;
        if (acPlugged) {
            constexpr int cLevels[3] = { 80, 90, 100 };
            for (int i = 0; i < 3; ++i) {
                if (pct >= cLevels[i]) chargeFiredMask |= (1u << i);
            }
        } else {
            constexpr int dLevels[4] = { 50, 20, 10, 5 };
            for (int i = 0; i < 4; ++i) {
                if (pct <= dLevels[i]) dischargeFiredMask |= (1u << i);
            }
        }
        bootstrapped = true;
    }

    int ObserveChargeMilestone(int pct) {
        constexpr int cLevels[3] = { 80, 90, 100 };
        dischargeFiredMask = 0;
        int hit = 0;
        for (int i = 0; i < 3; ++i) {
            if (pct <= cLevels[i] - 3) chargeFiredMask &= ~(1u << i);
            if (pct >= cLevels[i] && !(chargeFiredMask & (1u << i))) {
                chargeFiredMask |= (1u << i);
                hit = cLevels[i];
            }
        }
        return bootstrapped ? hit : 0;
    }

    int ObserveDischargeMilestone(int pct) {
        constexpr int dLevels[4] = { 50, 20, 10, 5 };
        chargeFiredMask = 0;
        int hit = 0;
        for (int i = 0; i < 4; ++i) {
            if (pct >= dLevels[i] + 3) dischargeFiredMask &= ~(1u << i);
            if (pct <= dLevels[i] && !(dischargeFiredMask & (1u << i))) {
                dischargeFiredMask |= (1u << i);
                hit = dLevels[i];
            }
        }
        return bootstrapped ? hit : 0;
    }
};

std::wstring FormatMemorySize(uint64_t bytes) {
    wchar_t buffer[48] = {};
    if (bytes >= 1024ull * 1024 * 1024) {
        swprintf_s(buffer, L"%.1f GB", bytes / (1024.0 * 1024 * 1024));
    } else {
        swprintf_s(buffer, L"%.0f MB", bytes / (1024.0 * 1024));
    }
    return buffer;
}

std::wstring FindTopRamProcessLabel() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return {};

    struct AppMem { std::wstring display; uint64_t ws = 0; };
    std::map<std::wstring, AppMem> groups;

    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    unsigned visited = 0;
    if (Process32FirstW(snap, &pe)) {
        do {
            if (++visited > 4096 || !g_running.load(std::memory_order_relaxed)) break;
            if (pe.th32ProcessID == 0 || pe.th32ProcessID == 4) continue;
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
            if (!hProc) hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
            if (!hProc) continue;

            PROCESS_MEMORY_COUNTERS_EX pmc{};
            if (K32GetProcessMemoryInfo(hProc, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
                std::wstring key = ToLowerStr(pe.szExeFile);
                auto& item = groups[key];
                if (item.display.empty()) item.display = LookupFriendlyExeDisplayName(pe.szExeFile);
                item.ws += pmc.WorkingSetSize;
            }
            CloseHandle(hProc);
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);

    std::wstring bestLabel;
    uint64_t bestWs = 0;
    for (const auto& [k, v] : groups) {
        if (v.ws > bestWs) {
            bestWs = v.ws;
            bestLabel = v.display + L" (" + FormatMemorySize(v.ws) + L")";
        }
    }
    return bestLabel;
}

struct BtConnectedDev {
    std::wstring key;
    std::wstring name;
    int percent = -1;
};

int ExtractBtPercentProp(wf::IInspectable const& obj) {
    auto val = obj.try_as<wf::IPropertyValue>();
    if (!val) return -1;
    int64_t num = -1;
    switch (val.Type()) {
        case wf::PropertyType::UInt8:  num = val.GetUInt8();  break;
        case wf::PropertyType::UInt16: num = val.GetUInt16(); break;
        case wf::PropertyType::UInt32: num = val.GetUInt32(); break;
        case wf::PropertyType::Int16:  num = val.GetInt16();  break;
        case wf::PropertyType::Int32:  num = val.GetInt32();  break;
        default: break;
    }
    return (num >= 0 && num <= 100) ? static_cast<int>(num) : -1;
}

bool ScanConnectedBluetoothMap(std::map<std::wstring, BtConnectedDev>& outMap) {
    outMap.clear();
    using namespace winrt::Windows::Devices::Enumeration;
    using namespace winrt::Windows::Devices::Bluetooth;

    std::vector<DeviceInformation> endpoints;
    bool anyQueryOk = false;
    auto props = winrt::single_threaded_vector<winrt::hstring>({
        L"System.Devices.ContainerId",
        L"System.Devices.Aep.ContainerId",
        L"System.Devices.BatteryLife",
        L"System.Devices.Aep.IsConnected"
    });

    for (bool isLe : { false, true }) {
        try {
            auto selector = isLe
                ? BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected)
                : BluetoothDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected);
            DeviceInformationCollection col{ nullptr };
            if (WaitWinRtAsyncBounded(DeviceInformation::FindAllAsync(selector, props, DeviceInformationKind::AssociationEndpoint), col, 1000) && col) {
                anyQueryOk = true;
                for (auto const& d : col) {
                    if (endpoints.size() >= 32) break;
                    endpoints.push_back(d);
                }
            }
        } catch (...) {}
    }
    if (!anyQueryOk) return false;

    std::map<std::wstring, int> containerBattery;
    constexpr DEVPROPKEY kDriverBatteryKey = { { 0x104ea319, 0x6ee2, 0x4701, { 0xbd, 0x47, 0x8d, 0xdb, 0xf4, 0x25, 0xbb, 0xe5 } }, 2 };
    constexpr DEVPROPKEY kShellBatteryKey  = { { 0x49cd1f76, 0x5626, 0x4b17, { 0xa4, 0xe8, 0x18, 0xb4, 0xaa, 0x1a, 0x22, 0x13 } }, 10 };
    constexpr DEVPROPKEY kContainerIdKey   = { { 0x8c7ed206, 0x3f8a, 0x4827, { 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c } }, 2 };

    for (PCWSTR enumName : { L"BTHENUM", L"BTHLEDEVICE", L"BTHLE", L"HID" }) {
        HDEVINFO hInfo = SetupDiGetClassDevsW(nullptr, enumName, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
        if (hInfo == INVALID_HANDLE_VALUE) continue;
        SP_DEVINFO_DATA devData{};
        devData.cbSize = sizeof(devData);
        for (DWORD i = 0; i < 384 && SetupDiEnumDeviceInfo(hInfo, i, &devData); ++i) {
            if (!g_running.load(std::memory_order_relaxed)) break;
            GUID container{};
            DEVPROPTYPE propType = 0;
            DWORD bytes = 0;
            if (!SetupDiGetDevicePropertyW(hInfo, &devData, &kContainerIdKey, &propType,
                    reinterpret_cast<PBYTE>(&container), sizeof(container), &bytes, 0) ||
                propType != DEVPROP_TYPE_GUID) {
                continue;
            }
            for (const auto& bKey : { kDriverBatteryKey, kShellBatteryKey }) {
                DWORD bVal = 0;
                if (SetupDiGetDevicePropertyW(hInfo, &devData, &bKey, &propType,
                        reinterpret_cast<PBYTE>(&bVal), sizeof(bVal), &bytes, 0) && bVal <= 100 &&
                    ((propType == DEVPROP_TYPE_BYTE && bytes == 1) || (propType == DEVPROP_TYPE_UINT32 && bytes == 4))) {
                    containerBattery[ToLowerStr(std::wstring(winrt::to_hstring(container)))] = static_cast<int>(bVal);
                    break;
                }
            }
        }
        SetupDiDestroyDeviceInfoList(hInfo);
    }

    for (auto const& dev : endpoints) {
        try {
            BtConnectedDev item;
            item.key = ToLowerStr(std::wstring(dev.Id()));
            item.name = dev.Name().c_str();
            if (item.name.empty()) item.name = L"Bluetooth";

            auto mapProps = dev.Properties();
            if (mapProps.HasKey(L"System.Devices.BatteryLife")) {
                item.percent = ExtractBtPercentProp(mapProps.Lookup(L"System.Devices.BatteryLife"));
            }
            for (PCWSTR cKey : { L"System.Devices.Aep.ContainerId", L"System.Devices.ContainerId" }) {
                if (!mapProps.HasKey(cKey)) continue;
                auto pv = mapProps.Lookup(cKey).try_as<wf::IPropertyValue>();
                if (pv && pv.Type() == wf::PropertyType::Guid && pv.GetGuid() != winrt::guid{}) {
                    item.key = ToLowerStr(std::wstring(winrt::to_hstring(pv.GetGuid())));
                    auto itB = containerBattery.find(item.key);
                    if (item.percent < 0 && itB != containerBattery.end()) item.percent = itB->second;
                    break;
                }
            }

            auto itExist = outMap.find(item.key);
            if (itExist == outMap.end()) {
                outMap.emplace(item.key, std::move(item));
            } else {
                if (itExist->second.percent < 0 && item.percent >= 0) itExist->second.percent = item.percent;
                if (itExist->second.name == L"Bluetooth" && item.name != L"Bluetooth") itExist->second.name = item.name;
            }
        } catch (...) {}
    }
    return true;
}

template <typename Mgr = winrt::Windows::UI::Shell::DetectedFocusSessionManager>
struct OptionalFocusSessionSubscription {
    Mgr mgr{ nullptr };
    winrt::event_token token{};
    bool subscribed = false;

    void Start() {
        if constexpr (!std::is_same_v<Mgr, WhNoFocusSessionManagerTag>) {
            try {
                if (Mgr::IsSupported()) {
                    mgr = Mgr::GetDefault();
                    if (mgr) {
                        g_win11FocusSessionState.store(mgr.IsFocusActive() ? 1 : 0, std::memory_order_relaxed);
                        token = mgr.IsFocusActiveChanged([](Mgr const& sender, wf::IInspectable const&) {
                            try {
                                bool active = sender.IsFocusActive();
                                int prev = g_win11FocusSessionState.exchange(active ? 1 : 0, std::memory_order_relaxed);
                                if (prev >= 0 && prev != (active ? 1 : 0) &&
                                    g_enableDndFocusAlertAtomic.load(std::memory_order_relaxed)) {
                                    g_lastFocusTransitionTick.store(GetTickCount64(), std::memory_order_relaxed);
                                    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                                    TriggerLiveAlert(
                                        AlertKind::FocusMode, L"\uE823",
                                        PickText(vi, L"Chế độ tập trung", L"Focus Mode"),
                                        PickText(vi, active ? L"Đã bắt đầu phiên tập trung" : L"Đã kết thúc phiên tập trung",
                                                     active ? L"Focus session started" : L"Focus session ended"),
                                        PickText(vi, active ? L"ĐÃ BẬT" : L"ĐÃ TẮT", active ? L"ON" : L"OFF"),
                                        -1, !active, active);
                                }
                            } catch (...) {}
                        });
                        subscribed = true;
                    }
                }
            } catch (...) {}
        }
    }

    void Stop() {
        if constexpr (!std::is_same_v<Mgr, WhNoFocusSessionManagerTag>) {
            if (subscribed && mgr) {
                try { mgr.IsFocusActiveChanged(token); } catch (...) {}
                subscribed = false;
                mgr = nullptr;
            }
        }
    }
};

static void* g_wnfSubQuietHours = nullptr;
static void* g_wnfSubLocation = nullptr;

LONG NTAPI WnfQuietHoursCallback(WnfStateName, ULONG, void*, void*, const void*, ULONG) {
    if (g_running.load(std::memory_order_relaxed) && g_alertHwnd && IsWindow(g_alertHwnd)) {
        PostMessageW(g_alertHwnd, WM_APP_WNF_QUIETHOURS, 0, 0);
    }
    return 0;
}

LONG NTAPI WnfLocationCallback(WnfStateName, ULONG, void*, void*, const void*, ULONG) {
    if (g_running.load(std::memory_order_relaxed) && g_watcherWakeEvent) {
        SetEvent(g_watcherWakeEvent);
    }
    return 0;
}

void StartWnfInstantSubscriptions() {
    using RtlSubscribeFn = LONG(NTAPI*)(void**, WnfStateName, ULONG,
        LONG(NTAPI*)(WnfStateName, ULONG, void*, void*, const void*, ULONG),
        void*, void*, ULONG, ULONG);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    auto fnSub = hNtdll ? reinterpret_cast<RtlSubscribeFn>(GetProcAddress(hNtdll, "RtlSubscribeWnfStateChangeNotification")) : nullptr;
    if (!fnSub) return;

    fnSub(&g_wnfSubQuietHours, kWnfQuietHoursStateName, 0, WnfQuietHoursCallback, nullptr, nullptr, 0, 0);
    fnSub(&g_wnfSubLocation,   kWnfLocationStateName,   0, WnfLocationCallback,   nullptr, nullptr, 0, 0);
}

void StopWnfInstantSubscriptions() {
    using RtlUnsubFn = LONG(NTAPI*)(void*);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    auto fnUnsub = hNtdll ? reinterpret_cast<RtlUnsubFn>(GetProcAddress(hNtdll, "RtlUnsubscribeWnfNotificationWaitForCompletion")) : nullptr;
    if (fnUnsub) {
        if (g_wnfSubQuietHours) { fnUnsub(g_wnfSubQuietHours); g_wnfSubQuietHours = nullptr; }
        if (g_wnfSubLocation)   { fnUnsub(g_wnfSubLocation);   g_wnfSubLocation = nullptr; }
    }
}

// Kiểm tra kết nối VPN đang hoạt động (Windows VPN, WireGuard, OpenVPN, Hotspot Shield, Cloudflare WARP, Tailscale...)
bool DetectActiveVpnConnection(std::wstring& outVpnName) {
    outVpnName.clear();
    static HMODULE s_hIphlp = LoadLibraryExW(L"iphlpapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    using GetAdaptersAddressesFn = ULONG(WINAPI*)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);
    static auto fnGetAdapters = s_hIphlp
        ? reinterpret_cast<GetAdaptersAddressesFn>(GetProcAddress(s_hIphlp, "GetAdaptersAddresses"))
        : nullptr;

    auto isExcludedVirtual = [](const std::wstring& low) -> bool {
        static constexpr PCWSTR kExcl[] = {
            L"loopback", L"teredo", L"isatap", L"6to4", L"ip-https",
            L"hyper-v", L"vethernet", L"wsl", L"vmware", L"virtualbox",
            L"bluetooth", L"wi-fi direct", L"microsoft kernel", L"npcap"
        };
        for (PCWSTR k : kExcl) {
            if (low.find(k) != std::wstring::npos) return true;
        }
        return false;
    };

    auto isKnownVpnKeyword = [](const std::wstring& low) -> bool {
        static constexpr PCWSTR kVpn[] = {
            L"vpn", L"wireguard", L"wintun", L"tap-windows", L"openvpn",
            L"hotspot shield", L"hydra", L"catapulta", L"cloudflare", L"warp",
            L"tailscale", L"nordlynx", L"proton", L"mullvad", L"expressvpn",
            L"surfshark", L"zerotier", L"anyconnect", L"forti", L"globalprotect",
            L"windscribe", L"softether", L"radmin", L"hamachi", L"outline",
            L"sing-box", L"clash", L"v2ray", L"mihomo", L"tun2socks"
        };
        for (PCWSTR k : kVpn) {
            if (low.find(k) != std::wstring::npos) return true;
        }
        return false;
    };

    if (fnGetAdapters) {
        ULONG bufLen = 32 * 1024;
        std::vector<BYTE> buf(bufLen);
        ULONG ret = fnGetAdapters(AF_UNSPEC,
                                  GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                  nullptr, reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buf.data()), &bufLen);
        if (ret == ERROR_BUFFER_OVERFLOW && bufLen <= 2 * 1024 * 1024) {
            buf.resize(bufLen);
            ret = fnGetAdapters(AF_UNSPEC,
                                GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                nullptr, reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buf.data()), &bufLen);
        }
        if (ret == ERROR_SUCCESS) {
            for (auto* a = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buf.data()); a != nullptr; a = a->Next) {
                if (a->OperStatus != IfOperStatusUp || !a->FirstUnicastAddress) continue;
                std::wstring friendly = a->FriendlyName ? a->FriendlyName : L"";
                std::wstring desc     = a->Description ? a->Description : L"";
                std::wstring combined = ToLowerStr(friendly + L" " + desc);
                if (isExcludedVirtual(combined)) continue;

                bool isVpnType = (a->IfType == IF_TYPE_PPP || a->IfType == IF_TYPE_TUNNEL || a->IfType == IF_TYPE_PROP_VIRTUAL);
                if (isVpnType || isKnownVpnKeyword(combined)) {
                    outVpnName = !friendly.empty() ? friendly : (!desc.empty() ? desc : L"VPN");
                    return true;
                }
            }
        }
    }

    try {
        using namespace winrt::Windows::Networking::Connectivity;
        auto profs = NetworkInformation::GetConnectionProfiles();
        for (auto const& p : profs) {
            if (!p) continue;
            if (p.GetNetworkConnectivityLevel() == NetworkConnectivityLevel::None) continue;
            uint32_t iana = 0;
            try {
                auto ad = p.NetworkAdapter();
                if (ad) iana = ad.IanaInterfaceType();
            } catch (...) {}
            std::wstring pName = p.ProfileName().c_str();
            std::wstring lowName = ToLowerStr(pName);
            if (isExcludedVirtual(lowName)) continue;
            if (iana == 23 || iana == 53 || iana == 131 || isKnownVpnKeyword(lowName)) {
                outVpnName = pName.empty() ? L"VPN" : pName;
                return true;
            }
        }
    } catch (...) {}

    return false;
}

// Luồng sự kiện MTA: Phát nhạc GSMTC + Micro/Cam/Vị trí + Điểm phát sóng + Mạng LAN/Wi-Fi + Bluetooth + RAM/ROM
DWORD WINAPI EventWatcherThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    std::atomic<bool> netDirty{ true };
    std::atomic<bool> btDirty{ true };

    winrt::event_token netToken{};
    bool hasNetSub = false;
    try {
        netToken = winrt::Windows::Networking::Connectivity::NetworkInformation::NetworkStatusChanged(
            [&netDirty](wf::IInspectable const&) noexcept {
                netDirty.store(true, std::memory_order_relaxed);
                if (g_watcherWakeEvent) SetEvent(g_watcherWakeEvent);
            });
        hasNetSub = true;
    } catch (...) {}

    using Watcher = winrt::Windows::Devices::Enumeration::DeviceWatcher;
    std::array<Watcher, 2> btWatchers{ nullptr, nullptr };
    std::array<Watcher::Added_revoker, 2> btAdded;
    std::array<Watcher::Updated_revoker, 2> btUpdated;
    std::array<Watcher::Removed_revoker, 2> btRemoved;

    auto onBtChanged = [&btDirty](auto const&, auto const&) noexcept {
        btDirty.store(true, std::memory_order_relaxed);
        if (g_watcherWakeEvent) SetEvent(g_watcherWakeEvent);
    };

    for (size_t i = 0; i < 2; ++i) {
        try {
            using namespace winrt::Windows::Devices::Enumeration;
            using namespace winrt::Windows::Devices::Bluetooth;
            auto selector = i
                ? BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected)
                : BluetoothDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected);
            auto props = winrt::single_threaded_vector<winrt::hstring>({
                L"System.Devices.Aep.IsConnected",
                L"System.Devices.BatteryLife"
            });
            btWatchers[i] = DeviceInformation::CreateWatcher(selector, props, DeviceInformationKind::AssociationEndpoint);
            btAdded[i]   = btWatchers[i].Added(winrt::auto_revoke, onBtChanged);
            btUpdated[i] = btWatchers[i].Updated(winrt::auto_revoke, onBtChanged);
            btRemoved[i] = btWatchers[i].Removed(winrt::auto_revoke, onBtChanged);
            btWatchers[i].Start();
        } catch (...) {
            btAdded[i].revoke();
            btUpdated[i].revoke();
            btRemoved[i].revoke();
            btWatchers[i] = nullptr;
        }
    }

    HANDLE hPrivacyEventCu = CreateEventW(nullptr, FALSE, TRUE, nullptr);
    HANDLE hPrivacyEventLm = CreateEventW(nullptr, FALSE, TRUE, nullptr);
    HKEY hKeyConsentCu = nullptr;
    HKEY hKeyConsentLm = nullptr;
    constexpr PCWSTR kConsentBase = L"Software\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore";
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kConsentBase, 0, KEY_NOTIFY | KEY_READ | KEY_WOW64_64KEY, &hKeyConsentCu) == ERROR_SUCCESS && hPrivacyEventCu) {
        RegNotifyChangeKeyValue(hKeyConsentCu, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, hPrivacyEventCu, TRUE);
    }
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, kConsentBase, 0, KEY_NOTIFY | KEY_READ | KEY_WOW64_64KEY, &hKeyConsentLm) == ERROR_SUCCESS && hPrivacyEventLm) {
        RegNotifyChangeKeyValue(hKeyConsentLm, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, hPrivacyEventLm, TRUE);
    }

    SystemMediaMonitor mediaMonitor(g_watcherWakeEvent);
    RealTimeMicMonitor micMonitor(g_watcherWakeEvent);
    RealTimeCameraMonitor camMonitor(g_watcherWakeEvent);

    OptionalFocusSessionSubscription<> focusSub;
    focusSub.Start();

    bool netInitialized = false;
    bool lastNetConnected = false;
    uint32_t lastNetIana = 0;
    std::wstring lastNetProfileName;

    bool btInitialized = false;
    std::map<std::wstring, BtConnectedDev> connectedBt;

    bool curHotspotOn = false;
    int  curHotspotClients = 0;
    ULONGLONG lastHotspotPollTick = 0;

    bool vpnBootstrapped = false;
    bool curVpnConnected = false;
    std::wstring curVpnName;
    ULONGLONG lastVpnPollTick = 0;

    bool privacyBootstrapped = false;
    bool prevMic = false, prevCam = false, prevLoc = false;

    MemoryAlertsTracker ramTracker;
    bool ramRomBootstrapped = false;
    uint32_t lowRomAlertedDrivesMask = 0;
    ULONGLONG lastRamCheckTick = 0;
    ULONGLONG lastRomCheckTick = 0;

    while (g_running.load(std::memory_order_relaxed)) {
        ULONGLONG now = GetTickCount64();

        // 0. Phát nhạc hệ thống (Ảnh bìa bài hát + Sóng nhạc)
        mediaMonitor.PollAndSync();

        // 1. Mạng LAN / Wi-Fi, VPN & Điểm phát sóng
        bool netSignaled = netDirty.exchange(false, std::memory_order_relaxed);
        bool needHotspotRefresh = g_enableHotspotBadgeAtomic.load(std::memory_order_relaxed) &&
                                  (lastHotspotPollTick == 0 || (now - lastHotspotPollTick >= (curHotspotOn ? 2500 : 4000)));
        bool needVpnRefresh = g_enableVpnBadgeAtomic.load(std::memory_order_relaxed) &&
                              (netSignaled || lastVpnPollTick == 0 || (now - lastVpnPollTick >= 2500));

        if (needVpnRefresh) {
            lastVpnPollTick = now;
            std::wstring detectedVpnName;
            bool nowVpn = DetectActiveVpnConnection(detectedVpnName);
            if (!vpnBootstrapped) {
                vpnBootstrapped = true;
                curVpnConnected = nowVpn;
                curVpnName = detectedVpnName;
            } else if (nowVpn != curVpnConnected) {
                bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                if (nowVpn) {
                    g_cornerSelectedKind = CornerItemKind::Vpn;
                    TriggerLiveAlert(
                        AlertKind::VpnAlert, L"\uE705",
                        PickText(vi, L"Đã kết nối VPN", L"VPN Connected"),
                        detectedVpnName.empty() ? L"VPN" : detectedVpnName,
                        PickText(vi, L"Đã bảo vệ", L"Protected"),
                        -1, false, true);
                } else {
                    TriggerLiveAlert(
                        AlertKind::VpnAlert, L"\uE705",
                        PickText(vi, L"Đã ngắt kết nối VPN", L"VPN Disconnected"),
                        curVpnName.empty() ? L"VPN" : curVpnName,
                        PickText(vi, L"Đã ngắt", L"Disconnected"),
                        -1, true, false);
                }
                curVpnConnected = nowVpn;
                curVpnName = detectedVpnName;
            } else if (nowVpn && !detectedVpnName.empty()) {
                curVpnName = detectedVpnName;
            }
        } else if (!g_enableVpnBadgeAtomic.load(std::memory_order_relaxed)) {
            curVpnConnected = false;
            curVpnName.clear();
        }

        if (netSignaled || needHotspotRefresh) {
            if (netInitialized && !needHotspotRefresh) {
                if (WaitForSingleObject(g_stopEvent, 120) == WAIT_OBJECT_0) break;
                netDirty.store(false, std::memory_order_relaxed);
            }
            lastHotspotPollTick = GetTickCount64();

            try {
                using namespace winrt::Windows::Networking::Connectivity;
                using namespace winrt::Windows::Networking::NetworkOperators;

                ConnectionProfile activeProf = NetworkInformation::GetInternetConnectionProfile();
                auto allProfs = NetworkInformation::GetConnectionProfiles();
                if (!activeProf) {
                    for (auto const& p : allProfs) {
                        if (!p) continue;
                        auto lvl = p.GetNetworkConnectivityLevel();
                        if (lvl == NetworkConnectivityLevel::InternetAccess ||
                            lvl == NetworkConnectivityLevel::ConstrainedInternetAccess ||
                            lvl == NetworkConnectivityLevel::LocalAccess) {
                            activeProf = p;
                            break;
                        }
                    }
                }

                bool nowConnected = false;
                uint32_t nowIana = 0;
                std::wstring nowName;

                if (activeProf) {
                    auto lvl = activeProf.GetNetworkConnectivityLevel();
                    if (lvl != NetworkConnectivityLevel::None) {
                        nowConnected = true;
                        nowName = activeProf.ProfileName().c_str();
                        try {
                            auto adapter = activeProf.NetworkAdapter();
                            if (adapter) nowIana = adapter.IanaInterfaceType();
                        } catch (...) {}
                    }
                }

                if (!netInitialized) {
                    netInitialized = true;
                    lastNetConnected = nowConnected;
                    lastNetIana = nowIana;
                    lastNetProfileName = nowName;
                } else {
                    bool newlyConnected = nowConnected &&
                        (!lastNetConnected || nowIana != lastNetIana || (!nowName.empty() && nowName != lastNetProfileName));
                    lastNetConnected = nowConnected;
                    lastNetIana = nowIana;
                    lastNetProfileName = nowName;

                    if (newlyConnected && g_enableNetworkAlertAtomic.load(std::memory_order_relaxed)) {
                        bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                        if (nowIana == 6) {
                            TriggerLiveAlert(
                                AlertKind::Network, L"\uE839",
                                PickText(vi, L"Đã kết nối mạng LAN", L"Ethernet Connected"),
                                nowName.empty() ? PickText(vi, L"Kết nối mạng có dây", L"Wired LAN network") : nowName,
                                PickText(vi, L"Mạng dây", L"Wired LAN"),
                                -1, false, true);
                        } else if (nowIana == 71) {
                            TriggerLiveAlert(
                                AlertKind::Network, L"\uE701",
                                PickText(vi, L"Đã kết nối Wi-Fi", L"Wi-Fi Connected"),
                                nowName,
                                PickText(vi, L"Đã kết nối", L"Connected"),
                                -1, false, true);
                        } else {
                            TriggerLiveAlert(
                                AlertKind::Network, L"\uE774",
                                PickText(vi, L"Đã kết nối mạng", L"Network Connected"),
                                nowName,
                                PickText(vi, L"Trực tuyến", L"Online"),
                                -1, false, true);
                        }
                    }
                }

                bool hsOn = false;
                int  hsClients = 0;
                if (g_enableHotspotBadgeAtomic.load(std::memory_order_relaxed)) {
                    auto checkProfileHotspot = [&](ConnectionProfile const& p) -> bool {
                        if (!p) return false;
                        try {
                            auto mgr = NetworkOperatorTetheringManager::CreateFromConnectionProfile(p);
                            if (mgr && mgr.TetheringOperationalState() == TetheringOperationalState::On) {
                                hsOn = true;
                                try { hsClients = static_cast<int>(mgr.ClientCount()); } catch (...) { hsClients = 0; }
                                return true;
                            }
                        } catch (...) {}
                        return false;
                    };
                    if (!checkProfileHotspot(activeProf)) {
                        for (auto const& p : allProfs) {
                            if (checkProfileHotspot(p)) break;
                        }
                    }
                }
                if (hsOn && !curHotspotOn) {
                    g_cornerSelectedKind = CornerItemKind::Hotspot;
                }
                curHotspotOn = hsOn;
                curHotspotClients = std::max(0, hsClients);
            } catch (...) {}
        }

        // 2. Quyền nhạy cảm Micro, Camera, Vị trí
        {
            bool micOn = false, camOn = false, locOn = false;
            std::wstring micApp, camApp, locApp;

            if (g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed)) {
                micOn = micMonitor.QueryActiveMic(micApp);
                camOn = camMonitor.QueryActiveCamera(camApp);
                SampleConsentStorePrivacy(micOn, micApp, camOn, camApp, locOn, locApp);
            } else {
                micMonitor.Clear();
                camMonitor.Stop();
            }

            if (!privacyBootstrapped) {
                privacyBootstrapped = true;
                prevMic = micOn;
                prevCam = camOn;
                prevLoc = locOn;
            } else if (g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed)) {
                bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                if (micOn && !prevMic) {
                    g_cornerSelectedKind = CornerItemKind::Microphone;
                    TriggerLiveAlert(
                        AlertKind::PrivacyAlert, L"\uE720",
                        PickText(vi, L"Micro đang bật", L"Microphone Active"),
                        micApp.empty() ? PickText(vi, L"Ứng dụng đang thu âm", L"Audio capture active") : micApp,
                        PickText(vi, L"ĐANG THU", L"IN USE"), -1, false, false);
                }
                if (camOn && !prevCam) {
                    g_cornerSelectedKind = CornerItemKind::Camera;
                    TriggerLiveAlert(
                        AlertKind::PrivacyAlert, L"\uE714",
                        PickText(vi, L"Camera đang bật", L"Camera Active"),
                        camApp.empty() ? PickText(vi, L"Ứng dụng đang dùng máy ảnh", L"Camera stream active") : camApp,
                        PickText(vi, L"ĐANG QUAY", L"IN USE"), -1, true, false);
                }
                if (locOn && !prevLoc) {
                    g_cornerSelectedKind = CornerItemKind::Location;
                    TriggerLiveAlert(
                        AlertKind::PrivacyAlert, L"\uE707",
                        PickText(vi, L"Đang truy cập Vị trí", L"Location Active"),
                        locApp.empty() ? PickText(vi, L"Dịch vụ định vị Windows", L"Windows Location Service") : locApp,
                        PickText(vi, L"ĐỊNH VỊ", L"IN USE"), -1, false, false);
                }
                prevMic = micOn;
                prevCam = camOn;
                prevLoc = locOn;
            }

            bool indChanged = false;
            {
                std::lock_guard<std::mutex> lock(g_persistMutex);
                bool nextHs = g_enableHotspotBadgeAtomic.load(std::memory_order_relaxed) ? curHotspotOn : false;
                int  nextHsCnt = nextHs ? curHotspotClients : 0;
                bool nextVpn = g_enableVpnBadgeAtomic.load(std::memory_order_relaxed) ? curVpnConnected : false;
                std::wstring nextVpnName = nextVpn ? curVpnName : L"";
                if (g_persistIndicators.vpnConnected != nextVpn ||
                    g_persistIndicators.vpnName != nextVpnName ||
                    g_persistIndicators.hotspotOn != nextHs ||
                    g_persistIndicators.hotspotClients != nextHsCnt ||
                    g_persistIndicators.micActive != micOn ||
                    g_persistIndicators.camActive != camOn ||
                    g_persistIndicators.locActive != locOn) {
                    g_persistIndicators.vpnConnected = nextVpn;
                    g_persistIndicators.vpnName = nextVpnName;
                    g_persistIndicators.hotspotOn = nextHs;
                    g_persistIndicators.hotspotClients = nextHsCnt;
                    g_persistIndicators.micActive = micOn;
                    g_persistIndicators.camActive = camOn;
                    g_persistIndicators.locActive = locOn;
                    indChanged = true;
                }
            }
            if (indChanged && g_alertHwnd && IsWindow(g_alertHwnd)) {
                PostMessageW(g_alertHwnd, WM_APP_INDICATORS_CHANGED, 0, 0);
            }
        }

        // 3. Bluetooth kết nối / ngắt kết nối
        if (btDirty.exchange(false, std::memory_order_relaxed)) {
            if (btInitialized) {
                if (WaitForSingleObject(g_stopEvent, 150) == WAIT_OBJECT_0) break;
                btDirty.store(false, std::memory_order_relaxed);
            }

            std::map<std::wstring, BtConnectedDev> nowBt;
            if (ScanConnectedBluetoothMap(nowBt)) {
                if (!btInitialized) {
                    btInitialized = true;
                    connectedBt = std::move(nowBt);
                } else {
                    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                    bool allowBt = g_enableBluetoothAlertAtomic.load(std::memory_order_relaxed);

                    for (const auto& [k, dev] : nowBt) {
                        if (connectedBt.find(k) == connectedBt.end() && allowBt) {
                            std::wstring val = (dev.percent >= 0)
                                ? (std::to_wstring(dev.percent) + L"%")
                                : PickText(vi, L"Đã kết nối", L"Connected");
                            TriggerLiveAlert(
                                AlertKind::Bluetooth, L"\uE702",
                                PickText(vi, L"Đã kết nối Bluetooth", L"Bluetooth Connected"),
                                dev.name, val, -1, false, true);
                        }
                    }
                    for (const auto& [k, oldDev] : connectedBt) {
                        if (nowBt.find(k) == nowBt.end() && allowBt) {
                            TriggerLiveAlert(
                                AlertKind::Bluetooth, L"\uE702",
                                PickText(vi, L"Đã ngắt Bluetooth", L"Bluetooth Disconnected"),
                                oldDev.name,
                                PickText(vi, L"Ngắt kết nối", L"Disconnected"),
                                -1, true, false);
                        }
                    }
                    connectedBt = std::move(nowBt);
                }
            }
        }

        // 4. Cảnh báo RAM (80/90/95%) & ROM (<10% trống)
        now = GetTickCount64();
        if (g_enableRamRomAlertAtomic.load(std::memory_order_relaxed)) {
            if (lastRamCheckTick == 0 || now - lastRamCheckTick >= 8000) {
                lastRamCheckTick = now;
                MEMORYSTATUSEX ms{};
                ms.dwLength = sizeof(ms);
                if (GlobalMemoryStatusEx(&ms)) {
                    int crossedLevel = ramTracker.Observe(ms.dwMemoryLoad);
                    if (crossedLevel > 0 && ramRomBootstrapped) {
                        bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                        std::wstring topApp = FindTopRamProcessLabel();
                        int pct = static_cast<int>(std::clamp<DWORD>(ms.dwMemoryLoad, 0, 100));
                        TriggerLiveAlert(
                            AlertKind::RamWarning, L"\uE950",
                            PickText(vi, L"Cảnh báo RAM", L"High RAM Usage"),
                            topApp, std::to_wstring(pct) + L"%", pct, true, false);
                    }
                }
            }

            if (lastRomCheckTick == 0 || now - lastRomCheckTick >= 30000) {
                lastRomCheckTick = now;
                DWORD drives = GetLogicalDrives();
                for (int i = 0; i < 26; ++i) {
                    if (!(drives & (1u << i))) continue;
                    wchar_t root[] = { static_cast<wchar_t>(L'A' + i), L':', L'\\', 0 };
                    if (GetDriveTypeW(root) != DRIVE_FIXED) continue;

                    ULARGE_INTEGER freeB{}, totalB{};
                    if (GetDiskFreeSpaceExW(root, &freeB, &totalB, nullptr) && totalB.QuadPart > 0) {
                        uint32_t bit = (1u << i);
                        if (freeB.QuadPart * 100ull > totalB.QuadPart * 12ull) {
                            lowRomAlertedDrivesMask &= ~bit;
                        }
                        if (freeB.QuadPart * 10ull < totalB.QuadPart && !(lowRomAlertedDrivesMask & bit)) {
                            lowRomAlertedDrivesMask |= bit;
                            if (ramRomBootstrapped) {
                                bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                                int usedPct = static_cast<int>(std::clamp<uint64_t>(
                                    ((totalB.QuadPart - freeB.QuadPart) * 100ull) / totalB.QuadPart, 0, 100));
                                std::wstring driveName = std::wstring(1, static_cast<wchar_t>(L'A' + i)) + L":";
                                std::wstring freeStr = std::wstring(PickText(vi, L"Trống ", L"Free ")) + FormatMemorySize(freeB.QuadPart);
                                TriggerLiveAlert(
                                    AlertKind::RomWarning, L"\uEDA2",
                                    std::wstring(PickText(vi, L"Ổ đĩa sắp đầy (", L"Low Disk (")) + driveName + L")",
                                    freeStr, std::to_wstring(usedPct) + L"%", usedPct, true, false);
                            }
                        }
                    }
                }
            }
            ramRomBootstrapped = true;
        }

        HANDLE waits[4] = { g_stopEvent, g_watcherWakeEvent, nullptr, nullptr };
        DWORD nWaits = 2;
        if (hKeyConsentCu && hPrivacyEventCu) waits[nWaits++] = hPrivacyEventCu;
        if (hKeyConsentLm && hPrivacyEventLm) waits[nWaits++] = hPrivacyEventLm;

        DWORD waitMs = curHotspotOn ? 2500 : 3500;
        DWORD wr = WaitForMultipleObjects(nWaits, waits, FALSE, waitMs);
        if (wr == WAIT_OBJECT_0) break;
        if (hKeyConsentCu && hPrivacyEventCu && wr == WAIT_OBJECT_0 + 2) {
            RegNotifyChangeKeyValue(hKeyConsentCu, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, hPrivacyEventCu, TRUE);
        }
        if (hKeyConsentLm && hPrivacyEventLm && wr >= WAIT_OBJECT_0 + 2) {
            RegNotifyChangeKeyValue(hKeyConsentLm, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, hPrivacyEventLm, TRUE);
        }
    }

    focusSub.Stop();
    mediaMonitor.Stop();
    micMonitor.Clear();
    camMonitor.Stop();
    if (hKeyConsentCu) RegCloseKey(hKeyConsentCu);
    if (hKeyConsentLm) RegCloseKey(hKeyConsentLm);
    if (hPrivacyEventCu) CloseHandle(hPrivacyEventCu);
    if (hPrivacyEventLm) CloseHandle(hPrivacyEventLm);

    for (size_t i = 0; i < 2; ++i) {
        btAdded[i].revoke();
        btUpdated[i].revoke();
        btRemoved[i].revoke();
        if (btWatchers[i]) {
            try { btWatchers[i].Stop(); } catch (...) {}
            btWatchers[i] = nullptr;
        }
    }
    if (hasNetSub) {
        try {
            winrt::Windows::Networking::Connectivity::NetworkInformation::NetworkStatusChanged(netToken);
        } catch (...) {}
    }

    if (SUCCEEDED(hrCom)) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// 15. Bảng Tạm (Copy / Cut / Paste — Đếm Chính Xác 100% Toàn Bộ Số Ký Tự Không Giới Hạn 65536!)
// ---------------------------------------------------------------------------
std::wstring MakeCleanSingleLinePreview(const wchar_t* raw, size_t maxBufWchars, size_t maxPreviewChars, uint64_t& outTotalChars) {
    outTotalChars = 0;
    if (!raw || maxBufWchars == 0 || !raw[0]) return {};

    // Đếm độ dài thực toàn bộ chuỗi (hỗ trợ hàng chục triệu ký tự trong <0.2ms, không dừng ở 65536!)
    size_t fullLen = wcsnlen(raw, maxBufWchars);
    outTotalChars = static_cast<uint64_t>(fullLen);

    std::wstring clean;
    clean.reserve(maxPreviewChars + 8);
    bool prevSpace = true;
    size_t scanLimit = std::min<size_t>(fullLen, 512);
    for (size_t i = 0; i < scanLimit && clean.size() < maxPreviewChars; ++i) {
        wchar_t ch = raw[i];
        if (ch == L'\r' || ch == L'\n' || ch == L'\t' || iswspace(ch)) {
            if (!prevSpace) {
                clean.push_back(L' ');
                prevSpace = true;
            }
        } else if (ch >= 0x20) {
            clean.push_back(ch);
            prevSpace = false;
        }
    }
    while (!clean.empty() && clean.back() == L' ') clean.pop_back();
    if (fullLen > clean.size() && !clean.empty()) {
        clean += L"…";
    }
    return clean;
}

void InspectAndTriggerClipboardAlert(bool isPasteTrigger, bool wasRecentCutShortcut) {
    static UINT s_fmtDropEffect = RegisterClipboardFormatW(L"Preferred DropEffect");
    static UINT s_fmtPng        = RegisterClipboardFormatW(L"PNG");
    static UINT s_fmtExcludeMon = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");

    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
    if (!OpenClipboard(nullptr)) return;

    bool isProtected = (s_fmtExcludeMon != 0 && IsClipboardFormatAvailable(s_fmtExcludeMon));
    bool isCut = wasRecentCutShortcut;
    if (!isPasteTrigger && s_fmtDropEffect != 0 && IsClipboardFormatAvailable(s_fmtDropEffect)) {
        if (HANDLE hDe = GetClipboardData(s_fmtDropEffect)) {
            if (const DWORD* pEff = static_cast<const DWORD*>(GlobalLock(hDe))) {
                if ((*pEff & 2 /*DROPEFFECT_MOVE*/) != 0 && (*pEff & 1 /*DROPEFFECT_COPY*/) == 0) {
                    isCut = true;
                }
                GlobalUnlock(hDe);
            }
        }
    }

    AlertKind kind = isPasteTrigger
        ? AlertKind::ClipboardPaste
        : (isCut ? AlertKind::ClipboardCut : AlertKind::ClipboardCopy);
    const wchar_t* icon = isPasteTrigger ? L"\uE77F" : (isCut ? L"\uE8C6" : L"\uE8C8");

    // 1. Tệp / Thư mục (CF_HDROP)
    if (IsClipboardFormatAvailable(CF_HDROP)) {
        if (HANDLE hDropData = GetClipboardData(CF_HDROP)) {
            if (HDROP hDrop = static_cast<HDROP>(GlobalLock(hDropData))) {
                UINT cnt = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                if (cnt > 0) {
                    wchar_t path[MAX_PATH] = {};
                    DragQueryFileW(hDrop, 0, path, MAX_PATH);
                    std::wstring name = PathFindFileNameW(path);
                    std::wstring sub = (cnt == 1) ? name : (name + L" +" + std::to_wstring(cnt - 1));
                    std::wstring val = FormatUintWithSeparators(cnt, vi) + PickText(vi, L" tệp", cnt == 1 ? L" file" : L" files");
                    const wchar_t* title = isPasteTrigger
                        ? PickText(vi, L"Đã dán tệp", L"Pasted Files")
                        : (isCut ? PickText(vi, L"Đã cắt tệp", L"Cut Files")
                                 : PickText(vi, L"Đã sao chép tệp", L"Copied Files"));
                    GlobalUnlock(hDropData);
                    CloseClipboard();
                    TriggerLiveAlert(kind, icon, title, sub, val, -1, false, true);
                    return;
                }
                GlobalUnlock(hDropData);
            }
        }
    }

    // 2. Văn bản (CF_UNICODETEXT — đếm đủ 100% số ký tự thực tế, ví dụ 197.529 ký tự!)
    if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        if (HANDLE hTxt = GetClipboardData(CF_UNICODETEXT)) {
            SIZE_T byteCap = GlobalSize(hTxt);
            size_t maxWchars = (byteCap >= sizeof(wchar_t)) ? (byteCap / sizeof(wchar_t)) : 0;
            if (const wchar_t* pStr = static_cast<const wchar_t*>(GlobalLock(hTxt))) {
                uint64_t totalChars = 0;
                std::wstring preview = isProtected
                    ? PickText(vi, L"Nội dung được bảo mật", L"Protected content")
                    : MakeCleanSingleLinePreview(pStr, maxWchars, 40, totalChars);
                GlobalUnlock(hTxt);
                if (totalChars > 0 || isProtected) {
                    CloseClipboard();
                    std::wstring val = isProtected
                        ? PickText(vi, L"Bảo mật", L"Protected")
                        : (FormatUintWithSeparators(totalChars, vi) + PickText(vi, L" ký tự", L" chars"));
                    const wchar_t* title = isPasteTrigger
                        ? PickText(vi, L"Đã dán văn bản", L"Pasted Text")
                        : (isCut ? PickText(vi, L"Đã cắt văn bản", L"Cut Text")
                                 : PickText(vi, L"Đã sao chép văn bản", L"Copied Text"));
                    TriggerLiveAlert(kind, icon, title, preview, val, -1, false, true);
                    return;
                }
            }
        }
    }

    // 3. Hình ảnh (CF_DIB / CF_DIBV5 / PNG / CF_BITMAP)
    bool hasImg = IsClipboardFormatAvailable(CF_DIB) ||
                  IsClipboardFormatAvailable(CF_DIBV5) ||
                  IsClipboardFormatAvailable(CF_BITMAP) ||
                  (s_fmtPng != 0 && IsClipboardFormatAvailable(s_fmtPng));
    if (hasImg) {
        std::wstring dimText;
        if (IsClipboardFormatAvailable(CF_DIB)) {
            if (HANDLE hDib = GetClipboardData(CF_DIB)) {
                if (const auto* bih = static_cast<const BITMAPINFOHEADER*>(GlobalLock(hDib))) {
                    int w = std::abs(bih->biWidth);
                    int h = std::abs(bih->biHeight);
                    if (w > 0 && h > 0 && w <= 32768 && h <= 32768) {
                        dimText = std::to_wstring(w) + L" × " + std::to_wstring(h);
                    }
                    GlobalUnlock(hDib);
                }
            }
        }
        CloseClipboard();
        const wchar_t* imgIcon = isPasteTrigger ? L"\uE77F" : (isCut ? L"\uE8C6" : L"\uEB9F");
        const wchar_t* title = isPasteTrigger
            ? PickText(vi, L"Đã dán hình ảnh", L"Pasted Image")
            : (isCut ? PickText(vi, L"Đã cắt hình ảnh", L"Cut Image")
                     : PickText(vi, L"Đã sao chép hình ảnh", L"Copied Image"));
        std::wstring val = dimText.empty() ? PickText(vi, L"Hình ảnh", L"Image") : dimText;
        std::wstring sub = dimText.empty() ? PickText(vi, L"Ảnh trong bảng tạm", L"Clipboard image") : L"";
        TriggerLiveAlert(kind, imgIcon, title, sub, val, -1, false, true);
        return;
    }

    CloseClipboard();
}

// ---------------------------------------------------------------------------
// 16. Window Procedures:
//     - CornerWidgetWndProc (Viên thuốc góc trái/trên có nút < > & bấm mở Cài đặt/Nhạc/Tiến trình)
//     - LiveAlertWndProc (Popup 3s 600px + Phím khóa <10ms + Alt+F4 + Mốc Pin + Kéo chuột Slider)
// ---------------------------------------------------------------------------
static UINT  g_wmShellHook = 0;
static ULONG g_shNotifyRegId = 0;

int HitTestCornerWidgetZone(HWND hwnd, int x, int y) {
    RECT rc{};
    if (!GetClientRect(hwnd, &rc)) return 0;
    int cw = rc.right - rc.left;
    int ch = rc.bottom - rc.top;
    if (x < 0 || y < 0 || x >= cw || y >= ch) return 0;

    auto items = BuildActiveCornerItems(GetPersistentIndicatorsSnapshot(), GetTickCount64());
    if (items.empty()) return 0;
    bool multi = (items.size() >= 2);
    if (!multi) return 2;

    if (!g_lastCornerIsVert) {
        int btnW = DpiScale(20, g_lastCornerDpi);
        if (x < btnW) return 1;        // Nút <
        if (x >= cw - btnW) return 3;  // Nút >
        return 2;                      // Vùng chính ở giữa
    } else {
        int arrowsH = DpiScale(18, g_lastCornerDpi);
        if (y >= ch - arrowsH) {
            return (x < cw / 2) ? 1 : 3;
        }
        return 2;
    }
}

LRESULT CALLBACK CornerWidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool s_trackingMouse = false;

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;

        case WM_MOUSEMOVE: {
            if (!s_trackingMouse) {
                TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                s_trackingMouse = true;
            }
            int zone = HitTestCornerWidgetZone(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            if (zone != g_cornerHoverZone) {
                g_cornerHoverZone = zone;
                RenderCornerWidgetSurface(false);
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            s_trackingMouse = false;
            if (g_cornerHoverZone != 0) {
                g_cornerHoverZone = 0;
                RenderCornerWidgetSurface(false);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            SetCapture(hwnd);
            return 0;
        }

        case WM_LBUTTONUP: {
            if (GetCapture() == hwnd) {
                ReleaseCapture();
            }
            int zone = HitTestCornerWidgetZone(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            auto items = BuildActiveCornerItems(GetPersistentIndicatorsSnapshot(), GetTickCount64());
            int curIdx = ResolveSelectedCornerIndex(items);
            if (curIdx >= 0 && !items.empty()) {
                int n = static_cast<int>(items.size());
                if (zone == 1 && n >= 2) {
                    int prevIdx = (curIdx - 1 + n) % n;
                    g_cornerSelectedKind = items[prevIdx].kind;
                    RefreshTaskbarGlassSurfaces(false);
                } else if (zone == 3 && n >= 2) {
                    int nextIdx = (curIdx + 1) % n;
                    g_cornerSelectedKind = items[nextIdx].kind;
                    RefreshTaskbarGlassSurfaces(false);
                } else if (zone == 2) {
                    ExecuteCornerItemAction(items[curIdx]);
                }
            }
            return 0;
        }

        case WM_RBUTTONDOWN: {
            return 0;
        }

        case WM_RBUTTONUP:
        case WM_CONTEXTMENU: {
            Wh_Log(L"Corner pill right-clicked -> opening Modern Taskbar Settings");
            OpenModernTaskbarSettingsApp();
            return 0;
        }

        case WM_MOUSEWHEEL: {
            auto items = BuildActiveCornerItems(GetPersistentIndicatorsSnapshot(), GetTickCount64());
            int curIdx = ResolveSelectedCornerIndex(items);
            if (curIdx >= 0 && items.size() >= 2) {
                int n = static_cast<int>(items.size());
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                int nextIdx = (delta > 0) ? ((curIdx - 1 + n) % n) : ((curIdx + 1) % n);
                g_cornerSelectedKind = items[nextIdx].kind;
                RefreshTaskbarGlassSurfaces(false);
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void SyncUiTimers(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    if (g_uiSettings.enableDndFocusAlert) {
        SetTimer(hwnd, IDT_DND_FALLBACK_POLL, 600, nullptr);
    } else {
        KillTimer(hwnd, IDT_DND_FALLBACK_POLL);
    }

    if (g_uiSettings.enableSplitTaskbar) {
        SetTimer(hwnd, IDT_TB_BACKDROP_WATCH, 120, nullptr);
    } else {
        KillTimer(hwnd, IDT_TB_BACKDROP_WATCH);
    }
}

std::wstring FormatDriveLettersFromMask(DWORD mask) {
    std::wstring res;
    for (int i = 0; i < 26; ++i) {
        if (mask & (1u << i)) {
            if (!res.empty()) res += L", ";
            res += static_cast<wchar_t>(L'A' + i);
            res += L':';
        }
    }
    return res;
}

void CheckAndAnnounceQuietHoursChange(int& ioLastQuietProfile) {
    if (!g_uiSettings.enableDndFocusAlert) return;
    int curProfile = QueryWnfQuietHoursProfile();
    if (curProfile < 0) return;
    if (ioLastQuietProfile < 0) {
        ioLastQuietProfile = curProfile;
        return;
    }
    if (curProfile == ioLastQuietProfile) return;

    int prevProfile = ioLastQuietProfile;
    ioLastQuietProfile = curProfile;

    ULONGLONG focusTick = g_lastFocusTransitionTick.load(std::memory_order_relaxed);
    bool focusActiveNow = (g_win11FocusSessionState.load(std::memory_order_relaxed) == 1);
    if (GetTickCount64() - focusTick <= 1000 || focusActiveNow) return;

    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
    if (curProfile == 2) {
        TriggerLiveAlert(
            AlertKind::FocusMode, L"\uE823",
            PickText(vi, L"Chế độ tập trung", L"Focus Mode"),
            PickText(vi, L"Chỉ cho phép báo thức ưu tiên", L"Alarms only mode active"),
            PickText(vi, L"ĐÃ BẬT", L"ON"), -1, false, true);
    } else if (curProfile == 1) {
        TriggerLiveAlert(
            AlertKind::DoNotDisturb, L"\uE708",
            PickText(vi, L"Không làm phiền", L"Do Not Disturb"),
            PickText(vi, L"Đã tắt tiếng thông báo", L"Notifications silenced"),
            PickText(vi, L"ĐÃ BẬT", L"ON"), -1, false, true);
    } else if (curProfile == 0) {
        if (prevProfile == 2) {
            TriggerLiveAlert(
                AlertKind::FocusMode, L"\uE823",
                PickText(vi, L"Chế độ tập trung", L"Focus Mode"),
                PickText(vi, L"Đã tắt chế độ tập trung", L"Focus mode disabled"),
                PickText(vi, L"ĐÃ TẮT", L"OFF"), -1, false, false);
        } else {
            TriggerLiveAlert(
                AlertKind::DoNotDisturb, L"\uE708",
                PickText(vi, L"Không làm phiền", L"Do Not Disturb"),
                PickText(vi, L"Đã bật lại thông báo", L"Notifications active"),
                PickText(vi, L"ĐÃ TẮT", L"OFF"), -1, false, false);
        }
    }
}

// Kiểm tra tiến trình vừa mở cửa sổ có đang chạy với quyền Quản trị (Administrator / UAC Elevated) hay không
static std::set<DWORD> g_notifiedElevatedPids;

bool IsCurrentProcessElevated() {
    static int s_cached = -1;
    if (s_cached >= 0) return s_cached != 0;
    HANDLE hTok = nullptr;
    TOKEN_ELEVATION elev{};
    DWORD cb = 0;
    bool res = false;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hTok)) {
        if (GetTokenInformation(hTok, TokenElevation, &elev, sizeof(elev), &cb)) {
            res = (elev.TokenIsElevated != 0);
        }
        CloseHandle(hTok);
    }
    s_cached = res ? 1 : 0;
    return res;
}

bool IsProcessElevatedForAdminAlert(DWORD pid, std::wstring& outAppName) {
    outAppName.clear();
    if (pid == 0 || pid == 4 || pid == GetCurrentProcessId()) return false;

    DWORD mySession = 0, targetSession = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &mySession) ||
        !ProcessIdToSessionId(pid, &targetSession) ||
        mySession != targetSession) {
        return false;
    }

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;

    wchar_t path[1024] = {};
    DWORD cch = ARRAYSIZE(path);
    bool gotPath = (QueryFullProcessImageNameW(hProc, 0, path, &cch) != FALSE && path[0] != 0);

    bool elevated = false;
    HANDLE hTok = nullptr;
    if (OpenProcessToken(hProc, TOKEN_QUERY, &hTok)) {
        TOKEN_ELEVATION elev{};
        DWORD cb = 0;
        if (GetTokenInformation(hTok, TokenElevation, &elev, sizeof(elev), &cb)) {
            elevated = (elev.TokenIsElevated != 0);
        }
        CloseHandle(hTok);
    } else if (GetLastError() == ERROR_ACCESS_DENIED && !IsCurrentProcessElevated()) {
        // explorer.exe chạy ở Medium-IL nên OpenProcessToken trên tiến trình High-IL (Admin/UAC) trả ERROR_ACCESS_DENIED
        elevated = true;
    }
    CloseHandle(hProc);

    if (!elevated) return false;

    if (gotPath) {
        std::wstring baseLow = ToLowerStr(PathFindFileNameW(path));
        static constexpr PCWSTR kIgnoreExes[] = {
            L"explorer.exe", L"consent.exe", L"credentialuibroker.exe",
            L"searchhost.exe", L"startmenuexperiencehost.exe", L"shellexperiencehost.exe",
            L"lockapp.exe", L"applicationframehost.exe", L"svchost.exe",
            L"dwm.exe", L"csrss.exe", L"taskhostw.exe", L"sihost.exe",
            L"ctfmon.exe", L"runtimebroker.exe", L"securityhealthsystray.exe"
        };
        for (PCWSTR ign : kIgnoreExes) {
            if (baseLow == ign) return false;
        }
        outAppName = LookupFriendlyExeDisplayName(PathFindFileNameW(path), path);
    }
    return true;
}

void SeedExistingTopWindowPids() {
    g_notifiedElevatedPids.clear();
    struct SeedCb {
        static BOOL CALLBACK Proc(HWND w, LPARAM) {
            if (!IsWindowVisible(w)) return TRUE;
            DWORD pid = 0;
            if (GetWindowThreadProcessId(w, &pid) && pid != 0) {
                g_notifiedElevatedPids.insert(pid);
            }
            return TRUE;
        }
    };
    EnumWindows(SeedCb::Proc, 0);
}

struct PendingAltF4Candidate {
    HWND hwnd = nullptr;
    std::wstring label;
    ULONGLONG pressTick = 0;
    bool reported = false;
};

LRESULT CALLBACK LiveAlertWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool s_acInit = false;
    static DWORD s_lastAc = 0;
    static bool s_batInit = false;
    static DWORD s_lastBat = 100;
    static BatteryMilestoneTracker s_batMilestones;
    static bool s_briInit = false;
    static DWORD s_lastBri = 100;
    static bool s_saverInit = false;
    static DWORD s_lastSaver = 0;

    static int   s_lastQuietProfile = -1;
    static DWORD s_lastClipSeq = 0;
    static ULONGLONG s_lastPasteKeyTick = 0;
    static bool s_trackingAlertMouse = false;
    static PendingAltF4Candidate s_pendingAltF4;

    static UINT s_pendingRecycledCount = 0;
    static UINT s_pendingDeletedCount = 0;
    static std::wstring s_pendingFileFirstName;
    static ULONGLONG s_lastRecycleTouchTick = 0;

    static bool s_pendingUsbArrival = false;
    static bool s_pendingUsbRemoval = false;
    static std::wstring s_pendingUsbName;
    static std::wstring s_pendingUsbDrive;

    if (g_wmShellHook != 0 && msg == g_wmShellHook) {
        if (wParam == HSHELL_WINDOWDESTROYED && s_pendingAltF4.hwnd != nullptr && !s_pendingAltF4.reported) {
            if (reinterpret_cast<HWND>(lParam) == s_pendingAltF4.hwnd &&
                (GetTickCount64() - s_pendingAltF4.pressTick <= 1800)) {
                s_pendingAltF4.reported = true;
                KillTimer(hwnd, IDT_ALTF4_VERIFY);
                if (g_uiSettings.enableAppExitAlert) {
                    bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                    TriggerLiveAlert(
                        AlertKind::AppClosed, L"\uE711",
                        PickText(vi, L"Đã thoát ứng dụng", L"Application Closed"),
                        s_pendingAltF4.label, L"Alt + F4", -1, false, false);
                }
            }
        }
        if (wParam == HSHELL_WINDOWCREATED && g_uiSettings.enableAdminAppAlert) {
            HWND hCreated = reinterpret_cast<HWND>(lParam);
            if (hCreated && IsWindow(hCreated) &&
                hCreated != hwnd && hCreated != g_cornerWidgetHwnd && hCreated != g_cachedTaskbarHwnd) {
                LONG_PTR ex = GetWindowLongPtrW(hCreated, GWL_EXSTYLE);
                if (!(ex & WS_EX_TOOLWINDOW)) {
                    DWORD pid = 0;
                    GetWindowThreadProcessId(hCreated, &pid);
                    if (pid != 0 && g_notifiedElevatedPids.find(pid) == g_notifiedElevatedPids.end()) {
                        std::wstring appName;
                        if (IsProcessElevatedForAdminAlert(pid, appName)) {
                            if (g_notifiedElevatedPids.size() > 256) {
                                g_notifiedElevatedPids.clear();
                            }
                            g_notifiedElevatedPids.insert(pid);
                            if (appName.empty()) {
                                wchar_t wt[256] = {};
                                GetWindowTextW(hCreated, wt, ARRAYSIZE(wt));
                                appName = wt;
                            }
                            bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                            if (appName.empty()) {
                                appName = PickText(vi, L"Ứng dụng hệ thống", L"Application");
                            }
                            TriggerLiveAlert(
                                AlertKind::AdminAppOpened, L"\uEA18",
                                PickText(vi, L"Đã mở quyền Quản trị", L"Opened as Administrator"),
                                appName, L"Admin", -1, false, false);
                        }
                    }
                }
            }
        }
        if (wParam == HSHELL_WINDOWDESTROYED && g_enableTaskProgressBadgeAtomic.load(std::memory_order_relaxed)) {
            bool hadTask = false;
            {
                std::lock_guard<std::mutex> lock(g_taskbandMutex);
                hadTask = !g_taskbandEntries.empty();
            }
            if (hadTask) {
                SyncTaskbandProgressToUi(0);
            }
        }
        if (wParam == HSHELL_WINDOWCREATED || wParam == HSHELL_WINDOWDESTROYED ||
            wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_REDRAW) {
            if (g_uiSettings.enableSplitTaskbar) {
                PostFastSyncToTaskbar(3);
                SetTimer(hwnd, IDT_SHELLHOOK_DEBOUNCE, 45, nullptr);
            }
        }
        return 0;
    }

    switch (msg) {
        case WM_CREATE: {
            AddClipboardFormatListener(hwnd);
            s_lastClipSeq = GetClipboardSequenceNumber();

            g_wmShellHook = RegisterWindowMessageW(L"SHELLHOOK");
            RegisterShellHookWindow(hwnd);
            SeedExistingTopWindowPids();

            PIDLIST_ABSOLUTE pidlDesktop = nullptr;
            if (SUCCEEDED(SHGetSpecialFolderLocation(hwnd, CSIDL_DESKTOP, &pidlDesktop)) && pidlDesktop) {
                SHChangeNotifyEntry entry = { pidlDesktop, TRUE };
                g_shNotifyRegId = SHChangeNotifyRegister(
                    hwnd,
                    SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
                    SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_DELETE | SHCNE_RMDIR | SHCNE_CREATE,
                    WM_APP_SHNOTIFY,
                    1, &entry);
                CoTaskMemFree(pidlDesktop);
            }

            // Khởi tạo trạng thái Pin ban đầu từ hệ thống để mốc sạc 80/90/100% & pin còn 50/20/10/5% hoạt động chuẩn xác ngay từ đầu
            SYSTEM_POWER_STATUS sps{};
            if (GetSystemPowerStatus(&sps)) {
                bool acPlugged = (sps.ACLineStatus == 1);
                int initPct = (sps.BatteryLifePercent <= 100) ? static_cast<int>(sps.BatteryLifePercent) : 100;
                s_acInit = true;
                s_lastAc = acPlugged ? 0 : 1;
                s_batInit = true;
                s_lastBat = static_cast<DWORD>(initPct);
                s_batMilestones.SeedInitialState(acPlugged, initPct);
            }

            s_lastQuietProfile = QueryWnfQuietHoursProfile();
            SyncUiTimers(hwnd);
            return 0;
        }

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_NCHITTEST: {
            if (g_animProgress <= 0.15f) return HTTRANSPARENT;
            if (g_activeSlotIsFloatingBottom) {
                AlertKind kind = AlertKind::None;
                { std::lock_guard<std::mutex> lock(g_alertMutex); kind = g_alert.kind; }
                if (kind != AlertKind::Volume && kind != AlertKind::Brightness) {
                    return HTTRANSPARENT;
                }
            }
            return HTCLIENT;
        }

        case WM_SETCURSOR: {
            AlertKind kind = AlertKind::None;
            { std::lock_guard<std::mutex> lock(g_alertMutex); kind = g_alert.kind; }
            if ((kind == AlertKind::Volume || kind == AlertKind::Brightness) &&
                (g_sliderDragging || g_sliderHovered || g_iconHovered)) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
        }

        case WM_MOUSEMOVE: {
            if (!s_trackingAlertMouse) {
                TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                s_trackingAlertMouse = true;
            }
            int mx = GET_X_LPARAM(lParam);
            int my = GET_Y_LPARAM(lParam);

            if (g_sliderDragging) {
                int pct = ComputeSliderPctFromMousePoint(mx, my);
                if (pct >= 0) ApplyMouseSliderPercent(hwnd, pct);
                return 0;
            }

            AlertKind kind = AlertKind::None;
            { std::lock_guard<std::mutex> lock(g_alertMutex); kind = g_alert.kind; }
            if (kind != AlertKind::None && kind != AlertKind::Volume && kind != AlertKind::Brightness) {
                if (g_uiSettings.dismissOnHover && !g_activeSlotIsFloatingBottom && g_animProgress > 0.2f) {
                    POINT ptCur{};
                    GetCursorPos(&ptCur);
                    bool cursorMoved = (std::abs(ptCur.x - g_alertShownCursorPos.x) >= 4 ||
                                        std::abs(ptCur.y - g_alertShownCursorPos.y) >= 4);
                    if (cursorMoved || (GetTickCount64() - g_alertShownTick >= 180)) {
                        {
                            std::lock_guard<std::mutex> lock(g_alertMutex);
                            g_alert.expireTick = 0;
                        }
                        KillTimer(hwnd, IDT_ALERT_EXPIRE);
                        StartAnimLoop(hwnd, 12);
                    }
                }
                return 0;
            }
            if (kind == AlertKind::Volume || kind == AlertKind::Brightness) {
                float hitPad = DpiScaleF(10.0f, g_activeDpi);
                bool onSlider = (g_lastSliderTrackRect.right > g_lastSliderTrackRect.left) &&
                                (mx >= g_lastSliderTrackRect.left - hitPad && mx <= g_lastSliderTrackRect.right + hitPad &&
                                 my >= g_lastSliderTrackRect.top - hitPad  && my <= g_lastSliderTrackRect.bottom + hitPad);
                bool onIcon = (kind == AlertKind::Volume) &&
                              (mx >= g_lastIconHitRect.left && mx <= g_lastIconHitRect.right &&
                               my >= g_lastIconHitRect.top  && my <= g_lastIconHitRect.bottom);
                if (onSlider != g_sliderHovered || onIcon != g_iconHovered) {
                    g_sliderHovered = onSlider;
                    g_iconHovered = onIcon;
                    RenderLiveAlertFrame(hwnd);
                }
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            s_trackingAlertMouse = false;
            if (!g_sliderDragging && (g_sliderHovered || g_iconHovered)) {
                g_sliderHovered = false;
                g_iconHovered = false;
                RenderLiveAlertFrame(hwnd);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int mx = GET_X_LPARAM(lParam);
            int my = GET_Y_LPARAM(lParam);
            AlertKind kind = AlertKind::None;
            { std::lock_guard<std::mutex> lock(g_alertMutex); kind = g_alert.kind; }

            if (kind == AlertKind::Volume || kind == AlertKind::Brightness) {
                if (kind == AlertKind::Volume &&
                    mx >= g_lastIconHitRect.left && mx <= g_lastIconHitRect.right &&
                    my >= g_lastIconHitRect.top  && my <= g_lastIconHitRect.bottom) {
                    int pct = 0;
                    bool muted = false;
                    if (ToggleSystemMuteFromUi(pct, muted)) {
                        ShowCurrentVolumeAlert(pct, muted);
                    }
                    return 0;
                }

                float hitPad = DpiScaleF(14.0f, g_activeDpi);
                if (g_lastSliderTrackRect.right > g_lastSliderTrackRect.left &&
                    mx >= g_lastSliderTrackRect.left - hitPad && mx <= g_lastSliderTrackRect.right + hitPad &&
                    my >= g_lastSliderTrackRect.top - hitPad  && my <= g_lastSliderTrackRect.bottom + hitPad) {
                    g_sliderDragging = true;
                    SetCapture(hwnd);
                    int pct = ComputeSliderPctFromMousePoint(mx, my);
                    if (pct >= 0) ApplyMouseSliderPercent(hwnd, pct);
                    return 0;
                }
            } else {
                {
                    std::lock_guard<std::mutex> lock(g_alertMutex);
                    g_alert.expireTick = 0;
                }
                KillTimer(hwnd, IDT_ALERT_EXPIRE);
                StartAnimLoop(hwnd, 12);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_sliderDragging) {
                g_sliderDragging = false;
                ReleaseCapture();
                int durationMs = g_alertDurationMsAtomic.load(std::memory_order_relaxed);
                {
                    std::lock_guard<std::mutex> lock(g_alertMutex);
                    g_alert.expireTick = GetTickCount64() + static_cast<ULONGLONG>(durationMs);
                }
                SetTimer(hwnd, IDT_ALERT_EXPIRE, durationMs + 25, nullptr);
                RenderLiveAlertFrame(hwnd);
            }
            return 0;
        }

        case WM_CAPTURECHANGED: {
            g_sliderDragging = false;
            return 0;
        }

        case WM_MOUSEWHEEL: {
            AlertKind kind = AlertKind::None;
            int curPct = 0;
            {
                std::lock_guard<std::mutex> lock(g_alertMutex);
                kind = g_alert.kind;
                curPct = g_alert.percentage;
            }
            if ((kind == AlertKind::Volume || kind == AlertKind::Brightness) && curPct >= 0) {
                int notches = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
                if (notches == 0) notches = (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? 1 : -1;
                ApplyMouseSliderPercent(hwnd, std::clamp(curPct + notches * 2, 0, 100));
            }
            return 0;
        }

        // Phản hồi tức khắc <10ms từ WH_KEYBOARD_LL cho Caps Lock, Num Lock, Scroll Lock, Insert!
        case WM_APP_KEYLOCK_EVENT: {
            if (!g_uiSettings.enableKeyLockAlert) return 0;
            bool vi = g_useViAtomic.load(std::memory_order_relaxed);
            DWORD vk = static_cast<DWORD>(wParam);
            bool isOn = (lParam != 0);

            if (vk == VK_CAPITAL) {
                TriggerLiveAlert(AlertKind::CapsLock, L"\uE765", L"Caps Lock",
                                 PickText(vi, isOn ? L"Chữ hoa đang bật" : L"Chữ thường", isOn ? L"Uppercase active" : L"Standard case"),
                                 PickText(vi, isOn ? L"BẬT" : L"TẮT", isOn ? L"ON" : L"OFF"),
                                 -1, !isOn, isOn);
            } else if (vk == VK_NUMLOCK) {
                TriggerLiveAlert(AlertKind::NumLock, L"\uE765", L"Num Lock",
                                 PickText(vi, isOn ? L"Bàn phím số đang bật" : L"Bàn phím số đã khóa", isOn ? L"Numpad active" : L"Numpad locked"),
                                 PickText(vi, isOn ? L"BẬT" : L"TẮT", isOn ? L"ON" : L"OFF"),
                                 -1, !isOn, isOn);
            } else if (vk == VK_SCROLL) {
                TriggerLiveAlert(AlertKind::ScrollLock, L"\uE765", L"Scroll Lock",
                                 PickText(vi, isOn ? L"Khóa cuộn đang bật" : L"Khóa cuộn đã tắt", isOn ? L"Scroll lock active" : L"Scroll lock off"),
                                 PickText(vi, isOn ? L"BẬT" : L"TẮT", isOn ? L"ON" : L"OFF"),
                                 -1, false, isOn);
            } else if (vk == VK_INSERT) {
                TriggerLiveAlert(AlertKind::InsertKey, L"\uE765", L"Insert",
                                 PickText(vi, isOn ? L"Chế độ ghi đè" : L"Chế độ chèn", isOn ? L"Overwrite mode" : L"Insert mode"),
                                 PickText(vi, isOn ? L"BẬT" : L"TẮT", isOn ? L"ON" : L"OFF"),
                                 -1, false, isOn);
            }
            return 0;
        }

        case WM_APP_VOLKEY_EVENT: {
            if (g_uiSettings.enableVolumeAlert) {
                SetTimer(hwnd, IDT_VOLKEY_FALLBACK, 24, nullptr);
            }
            return 0;
        }

        case WM_APP_PASTE_EVENT: {
            if (g_uiSettings.enableClipboardAlert) {
                ULONGLONG now = GetTickCount64();
                if (now - s_lastPasteKeyTick >= 200) {
                    s_lastPasteKeyTick = now;
                    InspectAndTriggerClipboardAlert(true, false);
                }
            }
            return 0;
        }

        case WM_APP_ALTF4_PRESSED: {
            if (!g_uiSettings.enableAppExitAlert) return 0;
            HWND hFg = GetForegroundWindow();
            if (!hFg) return 0;
            HWND hRoot = GetAncestor(hFg, GA_ROOT);
            if (!hRoot || !IsWindow(hRoot) || hRoot == hwnd || hRoot == g_cornerWidgetHwnd || hRoot == g_cachedTaskbarHwnd) {
                return 0;
            }
            wchar_t cls[64] = {};
            if (GetClassNameW(hRoot, cls, ARRAYSIZE(cls))) {
                if (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
                    _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
                    _wcsicmp(cls, L"Progman") == 0 ||
                    _wcsicmp(cls, L"WorkerW") == 0) {
                    return 0;
                }
            }

            DWORD pid = 0;
            GetWindowThreadProcessId(hRoot, &pid);
            std::wstring exeBase;
            std::wstring appName = GetFriendlyProcessNameByPid(pid, &exeBase);

            wchar_t winTitle[256] = {};
            GetWindowTextW(hRoot, winTitle, ARRAYSIZE(winTitle));

            std::wstring bestLabel = appName;
            if (_wcsicmp(exeBase.c_str(), L"ApplicationFrameHost.exe") == 0 && winTitle[0]) {
                bestLabel = winTitle;
            } else if (bestLabel.empty() && winTitle[0]) {
                bestLabel = winTitle;
            }
            if (bestLabel.empty()) {
                bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                bestLabel = PickText(vi, L"Cửa sổ ứng dụng", L"Application window");
            }

            s_pendingAltF4 = { hRoot, bestLabel, GetTickCount64(), false };
            SetTimer(hwnd, IDT_ALTF4_VERIFY, 75, nullptr);
            return 0;
        }

        case WM_APP_TASK_PROGRESS_EVENT: {
            SyncTaskbandProgressToUi(wParam);
            return 0;
        }

        case WM_APP_OSD_INTERCEPTED: {
            ULONGLONG now = GetTickCount64();
            if (g_llAltDown.load(std::memory_order_relaxed) ||
                g_llWinDown.load(std::memory_order_relaxed) ||
                (now - g_lastAltOrWinKeyTick.load(std::memory_order_relaxed) < 750)) {
                return 0;
            }
            if (now - g_lastVolOrBriAlertTick.load(std::memory_order_relaxed) < 85) {
                return 0;
            }

            bool volActiveNow = false;
            {
                std::lock_guard<std::mutex> lock(g_alertMutex);
                volActiveNow = (g_alert.kind == AlertKind::Volume && g_animProgress > 0.01f);
            }
            bool recentVolKey   = (now - g_lastVolKeyPressTick.load(std::memory_order_relaxed) <= 550);
            bool recentVolAlert = volActiveNow || (now - g_lastVolumeAlertTick.load(std::memory_order_relaxed) < 3500);

            if (recentVolKey) {
                if (g_uiSettings.enableVolumeAlert) {
                    int pct = 0;
                    bool muted = false;
                    if (QuerySystemVolumeState(pct, muted)) {
                        ShowCurrentVolumeAlert(pct, muted);
                    }
                }
                return 0;
            }

            if (g_uiSettings.enableBrightnessAlert) {
                int bri = -1;
                if (QuerySystemBrightnessPercent(bri) && bri >= 0) {
                    bool briChanged = !s_briInit || (static_cast<DWORD>(bri) != s_lastBri);
                    if (briChanged) {
                        s_briInit = true;
                        s_lastBri = static_cast<DWORD>(bri);
                        ShowCurrentBrightnessAlert(bri);
                        return 0;
                    }
                    // Brightness did NOT change: if user was adjusting Volume (e.g. pressing Vol+ at 100% or Vol- at 0%),
                    // keep showing Volume instead of jumping to Brightness!
                    if (recentVolAlert) {
                        if (g_uiSettings.enableVolumeAlert) {
                            int pct = 0;
                            bool muted = false;
                            if (QuerySystemVolumeState(pct, muted)) {
                                ShowCurrentVolumeAlert(pct, muted);
                            }
                        }
                        return 0;
                    }
                    // Only allow unchanged brightness OSD at 0% / 100% boundary when not recently adjusting Volume
                    if (bri == 0 || bri == 100) {
                        ShowCurrentBrightnessAlert(bri);
                        return 0;
                    }
                }
            }
            return 0;
        }

        case WM_APP_AUDIO_ENDPOINT_CHANGED: {
            RebindDefaultRenderVolumeEndpoint();
            return 0;
        }

        case WM_APP_WNF_QUIETHOURS: {
            CheckAndAnnounceQuietHoursChange(s_lastQuietProfile);
            return 0;
        }

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE: {
            RefreshSystemThemeAndAlignmentCache();
            g_wallpaperCache.lastStatTick = 0;
            RefreshTaskbarGlassSurfaces(true);
            return 0;
        }

        case WM_APP_XAML_BOUNDS_CHANGED:
        case WM_APP_INDICATORS_CHANGED: {
            RefreshTaskbarGlassSurfaces(false);
            if (msg == WM_APP_INDICATORS_CHANGED && g_animProgress > 0.01f) {
                bool isClockAlert = false;
                {
                    std::lock_guard<std::mutex> lock(g_alertMutex);
                    isClockAlert = (g_alert.kind == AlertKind::ClockAlert);
                }
                if (isClockAlert) {
                    RenderLiveAlertFrame(hwnd);
                }
            }
            return 0;
        }

        case WM_APP_RELOAD_SETTINGS: {
            g_uiSettings = ReadSettingsFromWindhawk(wParam != 0);
            SyncUiTimers(hwnd);
            RefreshOpenAppWindowCountOnce();
            if (HWND hTb = FindCurrentProcessTaskbarWnd()) {
                RunOnWindowThread(hTb, [](void* p) {
                    bool needSync = g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
                                    g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
                    SyncTaskbarXamlOnOwnerThread(static_cast<HWND>(p), !needSync);
                }, hTb);
            }
            RefreshTaskbarGlassSurfaces(true);
            if (g_watcherWakeEvent) SetEvent(g_watcherWakeEvent);
            if (g_animProgress > 0.01f) {
                PrepareAlertSlotAndLiquidGlass();
                RenderLiveAlertFrame(hwnd);
            }
            return 0;
        }

        case WM_APP_SHOW_ALERT: {
            UINT durationMs = static_cast<UINT>(std::max<LPARAM>(500, lParam));
            g_alertShownTick = GetTickCount64();
            GetCursorPos(&g_alertShownCursorPos);
            SetTimer(hwnd, IDT_ALERT_EXPIRE, durationMs + 20, nullptr);
            SweepExistingHardwareOsdWindows();

            AlertKind incomingKind = static_cast<AlertKind>(wParam);
            bool incomingIsSlider = (incomingKind == AlertKind::Volume || incomingKind == AlertKind::Brightness);
            TaskbarPillsGeometry checkGeo{};
            bool hasGeo = QueryTaskbarPillsGeometry(checkGeo);
            bool wantFloatingBottom = (g_uiSettings.popupPosition == PopupPosition::AdjacentTaskbar) ||
                                      (hasGeo && checkGeo.isVertical && !incomingIsSlider);

            if (g_animProgress <= 0.005f || !g_popupGlassCache.valid ||
                wantFloatingBottom != g_activeSlotIsFloatingBottom) {
                PrepareAlertSlotAndLiquidGlass();
                g_animProgress = std::max(g_animProgress, 0.36f);
            }
            RenderLiveAlertFrame(hwnd);
            StartAnimLoop(hwnd, 12);
            return 0;
        }

        case WM_APP_SHNOTIFY: {
            if (!g_uiSettings.enableFileDeleteAlert) return 0;
            using SHChangeNotifyLockFn = HANDLE(WINAPI*)(HANDLE, DWORD, PIDLIST_ABSOLUTE**, LONG*);
            using SHChangeNotifyUnlockFn = BOOL(WINAPI*)(HANDLE);
            static HMODULE s_hShell32 = GetModuleHandleW(L"shell32.dll");
            static auto fnLock = s_hShell32
                ? reinterpret_cast<SHChangeNotifyLockFn>(
                      GetProcAddress(s_hShell32, "SHChangeNotify_Lock")
                          ? GetProcAddress(s_hShell32, "SHChangeNotify_Lock")
                          : GetProcAddress(s_hShell32, MAKEINTRESOURCEA(644)))
                : nullptr;
            static auto fnUnlock = s_hShell32
                ? reinterpret_cast<SHChangeNotifyUnlockFn>(
                      GetProcAddress(s_hShell32, "SHChangeNotify_Unlock")
                          ? GetProcAddress(s_hShell32, "SHChangeNotify_Unlock")
                          : GetProcAddress(s_hShell32, MAKEINTRESOURCEA(645)))
                : nullptr;

            PIDLIST_ABSOLUTE* ppidl = nullptr;
            LONG lEvent = 0;
            HANDLE hLock = (fnLock && fnUnlock)
                ? fnLock(reinterpret_cast<HANDLE>(wParam), static_cast<DWORD>(lParam), &ppidl, &lEvent)
                : nullptr;
            if (hLock) {
                wchar_t path1[MAX_PATH] = {};
                wchar_t path2[MAX_PATH] = {};
                if (ppidl && ppidl[0]) SHGetPathFromIDListW(ppidl[0], path1);
                if (ppidl && ppidl[1]) SHGetPathFromIDListW(ppidl[1], path2);
                fnUnlock(hLock);

                LONG baseEv = (lEvent & ~SHCNE_INTERRUPT);
                if (baseEv == SHCNE_CREATE && path1[0] && StrStrIW(path1, L"\\$Recycle.Bin\\")) {
                    s_lastRecycleTouchTick = GetTickCount64();
                } else if ((baseEv == SHCNE_RENAMEITEM || baseEv == SHCNE_RENAMEFOLDER) &&
                           IsUserFacingFilePath(path1) && path2[0] && StrStrIW(path2, L"\\$Recycle.Bin\\")) {
                    if (s_pendingRecycledCount == 0 && s_pendingDeletedCount == 0) {
                        s_pendingFileFirstName = PathFindFileNameW(path1);
                    }
                    ++s_pendingRecycledCount;
                    SetTimer(hwnd, IDT_FILEOP_COALESCE, 65, nullptr);
                } else if ((baseEv == SHCNE_DELETE || baseEv == SHCNE_RMDIR) && IsUserFacingFilePath(path1)) {
                    if (s_pendingRecycledCount == 0 && s_pendingDeletedCount == 0) {
                        s_pendingFileFirstName = PathFindFileNameW(path1);
                    }
                    ++s_pendingDeletedCount;
                    SetTimer(hwnd, IDT_FILEOP_COALESCE, 65, nullptr);
                }
            }
            return 0;
        }

        case WM_DEVICECHANGE: {
            if (!g_uiSettings.enableUsbAlert || lParam == 0) return TRUE;
            if (wParam != DBT_DEVICEARRIVAL && wParam != DBT_DEVICEREMOVECOMPLETE) return TRUE;

            const auto* hdr = reinterpret_cast<const DEV_BROADCAST_HDR*>(lParam);
            bool isArrival = (wParam == DBT_DEVICEARRIVAL);

            if (hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                const auto* dbi = reinterpret_cast<const DEV_BROADCAST_DEVICEINTERFACE_W*>(lParam);
                if (IsEqualGUID(dbi->dbcc_classguid, kGuidDevInterfaceUsbDevice)) {
                    std::wstring key = ToLowerStr(dbi->dbcc_name);
                    std::wstring friendly;
                    if (isArrival) {
                        friendly = LookupUsbInterfaceFriendlyName(dbi->dbcc_name);
                        if (!friendly.empty()) g_usbKnownNames[key] = friendly;
                        s_pendingUsbArrival = true;
                        s_pendingUsbRemoval = false;
                    } else {
                        auto it = g_usbKnownNames.find(key);
                        if (it != g_usbKnownNames.end()) {
                            friendly = it->second;
                            g_usbKnownNames.erase(it);
                        }
                        s_pendingUsbRemoval = true;
                        s_pendingUsbArrival = false;
                    }
                    if (!friendly.empty() && s_pendingUsbName.empty()) s_pendingUsbName = friendly;
                    SetTimer(hwnd, IDT_USB_DEBOUNCE, 130, nullptr);
                }
            } else if (hdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                const auto* dbv = reinterpret_cast<const DEV_BROADCAST_VOLUME*>(lParam);
                if ((dbv->dbcv_flags & DBTF_MEDIA) == 0) {
                    std::wstring drv = FormatDriveLettersFromMask(dbv->dbcv_unitmask);
                    if (!drv.empty()) {
                        s_pendingUsbDrive = drv;
                        s_pendingUsbArrival = isArrival;
                        s_pendingUsbRemoval = !isArrival;
                        SetTimer(hwnd, IDT_USB_DEBOUNCE, 130, nullptr);
                    }
                }
            }
            return TRUE;
        }

        case WM_POWERBROADCAST: {
            if (wParam == PBT_POWERSETTINGCHANGE && lParam != 0) {
                const auto* pbs = reinterpret_cast<const POWERBROADCAST_SETTING*>(lParam);
                if (pbs->DataLength >= sizeof(DWORD)) {
                    DWORD val = *reinterpret_cast<const DWORD*>(pbs->Data);
                    bool vi = g_useViAtomic.load(std::memory_order_relaxed);

                    if (IsEqualGUID(pbs->PowerSetting, kGuidVideoBrightness)) {
                        SweepExistingHardwareOsdWindows();
                        if (!s_briInit) {
                            s_briInit = true;
                            s_lastBri = val;
                        } else if (!g_sliderDragging && (val != s_lastBri || val == 0 || val == 100)) {
                            s_lastBri = val;
                            if (g_uiSettings.enableBrightnessAlert && val <= 100) {
                                ShowCurrentBrightnessAlert(static_cast<int>(val));
                            }
                        }
                    } else if (IsEqualGUID(pbs->PowerSetting, kGuidAcDcPowerSource)) {
                        if (!s_acInit) {
                            s_acInit = true;
                            s_lastAc = val;
                        } else if (val != s_lastAc) {
                            s_lastAc = val;
                            int pct = static_cast<int>(std::clamp<DWORD>(s_lastBat, 0, 100));
                            bool acPlugged = (val == 0);
                            s_batMilestones.SeedInitialState(acPlugged, pct);

                            if (g_uiSettings.enableChargingAlert) {
                                if (acPlugged) {
                                    TriggerLiveAlert(AlertKind::Charging, L"\uE83E",
                                                     PickText(vi, L"Đang sạc pin", L"Charging"),
                                                     L"", std::to_wstring(pct) + L"%", pct, false, true);
                                } else {
                                    TriggerLiveAlert(AlertKind::Discharging, L"\uE83F",
                                                     PickText(vi, L"Đang dùng pin", L"On Battery"),
                                                     L"", std::to_wstring(pct) + L"%", pct, false, false);
                                }
                            }
                        }
                    } else if (IsEqualGUID(pbs->PowerSetting, kGuidBatteryPercentage)) {
                        int pct = static_cast<int>(std::clamp<DWORD>(val, 0, 100));
                        SYSTEM_POWER_STATUS sps{};
                        bool acPlugged = (s_lastAc == 0);
                        if (GetSystemPowerStatus(&sps) && (sps.ACLineStatus == 0 || sps.ACLineStatus == 1)) {
                            acPlugged = (sps.ACLineStatus == 1);
                            s_lastAc = acPlugged ? 0 : 1;
                        }

                        if (!s_batInit) {
                            s_batInit = true;
                            s_lastBat = val;
                            s_batMilestones.SeedInitialState(acPlugged, pct);
                        } else {
                            s_lastBat = val;
                            if (acPlugged) {
                                int cMilestone = s_batMilestones.ObserveChargeMilestone(pct);
                                if (cMilestone > 0 && g_uiSettings.enableChargingAlert) {
                                    if (cMilestone == 100) {
                                        TriggerLiveAlert(
                                            AlertKind::Charging, L"\uEA93",
                                            PickText(vi, L"Pin đã sạc đầy", L"Battery Fully Charged"),
                                            PickText(vi, L"Đã đạt mốc 100%", L"Reached 100%"),
                                            L"100%", 100, false, true);
                                    } else {
                                        std::wstring sub = std::wstring(PickText(vi, L"Đã sạc đạt mốc ", L"Charged to ")) +
                                                           std::to_wstring(cMilestone) + L"%";
                                        TriggerLiveAlert(
                                            AlertKind::Charging, L"\uE83E",
                                            PickText(vi, L"Đang sạc pin", L"Battery Charging"),
                                            sub, std::to_wstring(pct) + L"%", pct, false, true);
                                    }
                                }
                            } else {
                                int dMilestone = s_batMilestones.ObserveDischargeMilestone(pct);
                                if (dMilestone > 0 && g_uiSettings.enableChargingAlert) {
                                    if (dMilestone == 50) {
                                        TriggerLiveAlert(
                                            AlertKind::Discharging, L"\uE855",
                                            PickText(vi, L"Mức pin còn 50%", L"Battery at 50%"),
                                            PickText(vi, L"Đang sử dụng nguồn pin", L"Running on battery power"),
                                            std::to_wstring(pct) + L"%", pct, false, false);
                                    } else if (dMilestone == 20) {
                                        TriggerLiveAlert(
                                            AlertKind::LowBattery, L"\uE852",
                                            PickText(vi, L"Pin yếu (20%)", L"Low Battery (20%)"),
                                            PickText(vi, L"Hãy kết nối bộ sạc", L"Connect your charger"),
                                            std::to_wstring(pct) + L"%", pct, true, false);
                                    } else if (dMilestone == 10) {
                                        TriggerLiveAlert(
                                            AlertKind::LowBattery, L"\uE851",
                                            PickText(vi, L"Pin rất yếu (10%)", L"Very Low Battery (10%)"),
                                            PickText(vi, L"Cần cắm sạc ngay", L"Plug in your charger soon"),
                                            std::to_wstring(pct) + L"%", pct, true, false);
                                    } else if (dMilestone == 5) {
                                        TriggerLiveAlert(
                                            AlertKind::LowBattery, L"\uE850",
                                            PickText(vi, L"Pin nguy kịch (5%)", L"Critical Battery (5%)"),
                                            PickText(vi, L"Sắp tắt nguồn, hãy cắm sạc!", L"Plug in immediately!"),
                                            std::to_wstring(pct) + L"%", pct, true, false);
                                    }
                                }
                            }
                        }
                    } else if (IsEqualGUID(pbs->PowerSetting, kGuidPowerSavingStatus)) {
                        if (!s_saverInit) {
                            s_saverInit = true;
                            s_lastSaver = val;
                        } else if (val != s_lastSaver) {
                            s_lastSaver = val;
                            if (g_uiSettings.enableBatterySaverAlert) {
                                bool saverOn = (val != 0);
                                TriggerLiveAlert(
                                    AlertKind::BatterySaver, L"\uEBAA",
                                    PickText(vi, L"Tiết kiệm pin", L"Battery Saver"),
                                    PickText(vi, saverOn ? L"Đã bật chế độ tiết kiệm pin" : L"Đã tắt chế độ tiết kiệm pin",
                                                 saverOn ? L"Energy saver enabled" : L"Energy saver disabled"),
                                    PickText(vi, saverOn ? L"ĐÃ BẬT" : L"ĐÃ TẮT", saverOn ? L"ON" : L"OFF"),
                                    -1, false, saverOn);
                            }
                        }
                    }
                }
            }
            return TRUE;
        }

        case WM_CLIPBOARDUPDATE: {
            if (!g_uiSettings.enableClipboardAlert) return 0;
            DWORD seq = GetClipboardSequenceNumber();
            if (seq == s_lastClipSeq) return 0;
            s_lastClipSeq = seq;
            SetTimer(hwnd, IDT_CLIPBOARD_DEBOUNCE, 15, nullptr);
            return 0;
        }

        case WM_TIMER: {
            if (wParam == IDT_TASK_PROGRESS_CLEANUP) {
                SyncTaskbandProgressToUi(0);
                return 0;
            }

            if (wParam == IDT_ALTF4_VERIFY) {
                ULONGLONG now = GetTickCount64();
                if (!s_pendingAltF4.hwnd || s_pendingAltF4.reported || (now - s_pendingAltF4.pressTick > 1600)) {
                    KillTimer(hwnd, IDT_ALTF4_VERIFY);
                    s_pendingAltF4 = {};
                    return 0;
                }
                if (!IsWindow(s_pendingAltF4.hwnd) || !IsWindowVisible(s_pendingAltF4.hwnd)) {
                    s_pendingAltF4.reported = true;
                    KillTimer(hwnd, IDT_ALTF4_VERIFY);
                    if (g_uiSettings.enableAppExitAlert) {
                        bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                        TriggerLiveAlert(
                            AlertKind::AppClosed, L"\uE711",
                            PickText(vi, L"Đã thoát ứng dụng", L"Application Closed"),
                            s_pendingAltF4.label, L"Alt + F4", -1, false, false);
                    }
                    s_pendingAltF4 = {};
                }
                return 0;
            }

            if (wParam == IDT_VOLKEY_FALLBACK) {
                KillTimer(hwnd, IDT_VOLKEY_FALLBACK);
                if (g_uiSettings.enableVolumeAlert) {
                    ULONGLONG now = GetTickCount64();
                    if (now - g_lastVolOrBriAlertTick.load(std::memory_order_relaxed) >= 35) {
                        int pct = 0;
                        bool muted = false;
                        if (QuerySystemVolumeState(pct, muted)) {
                            ShowCurrentVolumeAlert(pct, muted);
                        }
                    }
                }
                return 0;
            }

            if (wParam == IDT_CORNER_CLOCK_TICK || wParam == IDT_CORNER_WAVE_TICK) {
                RenderCornerWidgetSurface(false);
                if (wParam == IDT_CORNER_CLOCK_TICK && g_animProgress > 0.01f) {
                    bool needRepaintAlert = false;
                    {
                        std::lock_guard<std::mutex> lock(g_alertMutex);
                        if (g_alert.kind == AlertKind::ClockAlert) {
                            PersistentIndicators snap = GetPersistentIndicatorsSnapshot();
                            ULONGLONG now = GetTickCount64();
                            std::wstring liveTxt;
                            if (g_alert.icon == L"\uE916" && snap.clock.stopwatchRunning) {
                                liveTxt = FormatClockCompactTime(snap.clock.CurrentStopwatchSeconds(now));
                            } else if (snap.clock.timerRunning) {
                                liveTxt = FormatClockCompactTime(snap.clock.CurrentTimerSeconds(now));
                            } else if (snap.clock.stopwatchRunning) {
                                liveTxt = FormatClockCompactTime(snap.clock.CurrentStopwatchSeconds(now));
                            }
                            if (!liveTxt.empty() && liveTxt != g_alert.valueText) {
                                g_alert.valueText = liveTxt;
                                needRepaintAlert = true;
                            }
                        }
                    }
                    if (needRepaintAlert) {
                        RenderLiveAlertFrame(hwnd);
                    }
                }
                return 0;
            }

            if (wParam == IDT_DND_FALLBACK_POLL) {
                CheckAndAnnounceQuietHoursChange(s_lastQuietProfile);
                return 0;
            }

            if (wParam == IDT_SHELLHOOK_DEBOUNCE) {
                KillTimer(hwnd, IDT_SHELLHOOK_DEBOUNCE);
                if (g_uiSettings.enableSplitTaskbar) {
                    RefreshOpenAppWindowCountOnce();
                    RefreshTaskbarGlassSurfaces(false);
                }
                return 0;
            }

            if (wParam == IDT_TB_BACKDROP_WATCH) {
                if (g_uiSettings.enableSplitTaskbar) {
                    TaskbarPillsGeometry geo{};
                    if (QueryTaskbarPillsGeometry(geo)) {
                        HMONITOR hMon = MonitorFromWindow(geo.hTaskbar, MONITOR_DEFAULTTONEAREST);
                        MONITORINFO mi = { sizeof(mi) };
                        if (hMon && GetMonitorInfoW(hMon, &mi)) {
                            EnsureMonitorWallpaperLoaded(mi.rcMonitor);
                        }
                        uint64_t h = ComputeTaskbarBackdropSignature(geo.rcTaskbarScreen);
                        static bool s_wasFullscreenOnMon = false;
                        bool isFsNow = (hMon && mi.cbSize == sizeof(mi)) ? IsFullscreenWindowOnMonitor(hMon, mi.rcMonitor) : false;
                        bool fsToggled = (isFsNow != s_wasFullscreenOnMon);
                        s_wasFullscreenOnMon = isFsNow;

                        if (h != g_lastTbBackdropHash || !EqualRect(&geo.rcTaskbarScreen, &g_lastTbScreenRect)) {
                            RefreshTaskbarGlassSurfaces(true);
                        } else if (fsToggled || geo.nativeAutoHide) {
                            RenderCornerWidgetSurface(false);
                        }
                    }
                }
                return 0;
            }

            if (wParam == IDT_CLIPBOARD_DEBOUNCE) {
                KillTimer(hwnd, IDT_CLIPBOARD_DEBOUNCE);
                if (g_uiSettings.enableClipboardAlert) {
                    ULONGLONG cutTick = g_lastCutShortcutTick.exchange(0, std::memory_order_relaxed);
                    bool wasCut = (cutTick > 0 && (GetTickCount64() - cutTick <= 650));
                    InspectAndTriggerClipboardAlert(false, wasCut);
                }
                return 0;
            }

            if (wParam == IDT_FILEOP_COALESCE) {
                KillTimer(hwnd, IDT_FILEOP_COALESCE);
                bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                ULONGLONG now = GetTickCount64();

                bool movedToTrash = (s_pendingRecycledCount > 0) ||
                                    (s_pendingDeletedCount > 0 && (now - s_lastRecycleTouchTick <= 350));
                UINT totalCnt = s_pendingRecycledCount + s_pendingDeletedCount;
                std::wstring firstFile = s_pendingFileFirstName;

                s_pendingRecycledCount = 0;
                s_pendingDeletedCount = 0;
                s_pendingFileFirstName.clear();

                if (totalCnt > 0 && g_uiSettings.enableFileDeleteAlert) {
                    std::wstring sub = (totalCnt == 1)
                        ? firstFile
                        : (firstFile + L" +" + std::to_wstring(totalCnt - 1));
                    std::wstring val = FormatUintWithSeparators(totalCnt, vi) + PickText(vi, L" tệp", totalCnt == 1 ? L" item" : L" items");
                    if (movedToTrash) {
                        TriggerLiveAlert(
                            AlertKind::FileRecycled, L"\uE74D",
                            PickText(vi, L"Đã chuyển vào Thùng rác", L"Moved to Recycle Bin"),
                            sub, val, -1, false, false);
                    } else {
                        TriggerLiveAlert(
                            AlertKind::FileDeleted, L"\uE74D",
                            PickText(vi, L"Đã xóa tệp", L"Deleted File"),
                            sub, val, -1, true, false);
                    }
                }
                return 0;
            }

            if (wParam == IDT_USB_DEBOUNCE) {
                KillTimer(hwnd, IDT_USB_DEBOUNCE);
                bool vi = g_useViAtomic.load(std::memory_order_relaxed);
                bool arrival = s_pendingUsbArrival;
                bool removal = s_pendingUsbRemoval;
                std::wstring name = s_pendingUsbName;
                std::wstring drv  = s_pendingUsbDrive;

                s_pendingUsbArrival = false;
                s_pendingUsbRemoval = false;
                s_pendingUsbName.clear();
                s_pendingUsbDrive.clear();

                if ((arrival || removal) && g_uiSettings.enableUsbAlert) {
                    std::wstring sub = name;
                    if (!drv.empty()) {
                        sub = sub.empty()
                            ? (std::wstring(PickText(vi, L"Ổ đĩa USB (", L"USB Drive (")) + drv + L")")
                            : (sub + L" (" + drv + L")");
                    }
                    if (sub.empty()) sub = PickText(vi, L"Thiết bị ngoại vi USB", L"USB Peripheral");

                    if (arrival) {
                        TriggerLiveAlert(
                            AlertKind::UsbDevice, L"\uE88E",
                            PickText(vi, L"Đã kết nối USB", L"USB Connected"),
                            sub, PickText(vi, L"Kết nối", L"Connected"), -1, false, true);
                    } else {
                        TriggerLiveAlert(
                            AlertKind::UsbDevice, L"\uE88E",
                            PickText(vi, L"Đã ngắt kết nối USB", L"USB Disconnected"),
                            sub, PickText(vi, L"Đã rút", L"Removed"), -1, true, false);
                    }
                }
                return 0;
            }

            if (wParam == IDT_ALERT_EXPIRE) {
                if (g_sliderDragging) {
                    return 0;
                }
                KillTimer(hwnd, IDT_ALERT_EXPIRE);
                StartAnimLoop(hwnd, 12);
                return 0;
            }

            if (wParam == IDT_ANIM_FRAME) {
                ULONGLONG now = GetTickCount64();
                float dt = (g_lastAnimFrameTick > 0 && now > g_lastAnimFrameTick)
                    ? std::clamp(static_cast<float>(now - g_lastAnimFrameTick) / 1000.0f, 0.001f, 0.055f)
                    : 0.012f;
                g_lastAnimFrameTick = now;

                bool active = false;
                bool pctMoving = false;
                bool keepMediaWave = false;
                ULONGLONG expCopy = 0;

                {
                    std::lock_guard<std::mutex> lock(g_alertMutex);
                    if (g_alert.kind != AlertKind::None) {
                        if (g_sliderDragging) {
                            g_alert.expireTick = now + static_cast<ULONGLONG>(g_alertDurationMsAtomic.load(std::memory_order_relaxed));
                        }
                        expCopy = g_alert.expireTick;
                        if (g_alert.percentage >= 0) {
                            float target = static_cast<float>(g_alert.percentage);
                            float diff = target - g_alert.smoothPct;
                            if (!g_sliderDragging && std::fabs(diff) > 0.08f) {
                                g_alert.smoothPct += diff * (1.0f - std::exp(-dt * 26.0f));
                                pctMoving = true;
                            } else {
                                g_alert.smoothPct = target;
                            }
                        }
                        active = (now < g_alert.expireTick);
                        keepMediaWave = active && (g_alert.kind == AlertKind::MediaAlert) && g_alert.mediaPlaying;
                    }
                }

                if (active) {
                    g_animProgress = std::min(1.0f, g_animProgress + dt / 0.135f);
                } else {
                    g_animProgress = std::max(0.0f, g_animProgress - dt / 0.130f);
                    if (g_animProgress <= 0.004f) {
                        g_animProgress = 0.0f;
                        std::lock_guard<std::mutex> lock(g_alertMutex);
                        if (now >= g_alert.expireTick) {
                            g_alert.kind = AlertKind::None;
                        }
                    }
                }

                RenderLiveAlertFrame(hwnd);

                bool settled = active ? (g_animProgress >= 0.999f) : (g_animProgress <= 0.001f);
                if (settled && !pctMoving) {
                    if (active && expCopy > now) {
                        UINT rem = static_cast<UINT>(std::min<ULONGLONG>(15000, expCopy - now + 15));
                        SetTimer(hwnd, IDT_ALERT_EXPIRE, rem, nullptr);
                    }
                    if (keepMediaWave) {
                        // Giữ sóng nhạc trên Popup chuyển động mượt nhẹ ở 36ms (~28fps) đến hết 3 giây
                        StartAnimLoop(hwnd, 36);
                    } else {
                        StopAnimLoop(hwnd);
                    }
                }
                return 0;
            }
            break;
        }

        case WM_DESTROY: {
            if (g_shNotifyRegId != 0) {
                SHChangeNotifyDeregister(g_shNotifyRegId);
                g_shNotifyRegId = 0;
            }
            DeregisterShellHookWindow(hwnd);
            RemoveClipboardFormatListener(hwnd);
            StopAnimLoop(hwnd);
            for (UINT_PTR tid : { IDT_ALERT_EXPIRE, IDT_SHELLHOOK_DEBOUNCE, IDT_CLIPBOARD_DEBOUNCE,
                                  IDT_FILEOP_COALESCE, IDT_USB_DEBOUNCE, IDT_TB_BACKDROP_WATCH,
                                  IDT_VOLKEY_FALLBACK, IDT_CORNER_CLOCK_TICK, IDT_CORNER_WAVE_TICK,
                                  IDT_DND_FALLBACK_POLL, IDT_ALTF4_VERIFY, IDT_TASK_PROGRESS_CLEANUP }) {
                KillTimer(hwnd, tid);
            }
            PostQuitMessage(0);
            return 0;
        }
    }
    if (g_msgReloadSettings != 0 && msg == g_msgReloadSettings) {
        PostMessageW(hwnd, WM_APP_RELOAD_SETTINGS, wParam, lParam);
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// 17. Luồng Giao Diện & Điểm Vào Windhawk Mod
// ---------------------------------------------------------------------------
DWORD WINAPI ModUiThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_uiSettings = ReadSettingsFromWindhawk();
    PrepopulateConnectedUsbNames();

    HINSTANCE hInst = GetModInstanceHandle();

    const wchar_t* kCornerClass = L"WindhawkLiquidGlassCornerWidgetV8";
    WNDCLASSEXW wcCorner = { sizeof(wcCorner) };
    wcCorner.lpfnWndProc = CornerWidgetWndProc;
    wcCorner.hInstance = hInst;
    wcCorner.hCursor = LoadCursorW(nullptr, IDC_HAND);
    wcCorner.lpszClassName = kCornerClass;
    RegisterClassExW(&wcCorner);

    const wchar_t* kAlertClass = L"WindhawkLiquidGlassLiveAlertV8";
    WNDCLASSEXW wcAlert = { sizeof(wcAlert) };
    wcAlert.lpfnWndProc = LiveAlertWndProc;
    wcAlert.hInstance = hInst;
    wcAlert.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcAlert.lpszClassName = kAlertClass;
    RegisterClassExW(&wcAlert);

    DWORD exInteractive = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

    g_cornerWidgetHwnd = CreateWindowExW(exInteractive, kCornerClass, L"Taskbar Liquid Glass Corner Pill", WS_POPUP,
                                         0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);
    g_alertHwnd        = CreateWindowExW(exInteractive, kAlertClass, L"Liquid Glass Live Alert", WS_POPUP,
                                         0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);
    if (g_alertHwnd && g_msgReloadSettings != 0) {
        ChangeWindowMessageFilterEx(g_alertHwnd, g_msgReloadSettings, MSGFLT_ALLOW, nullptr);
    }

    RefreshOpenAppWindowCountOnce();
    if (HWND hTb = FindCurrentProcessTaskbarWnd()) {
        RunOnWindowThread(hTb, [](void* p) {
            bool needSync = g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
                            g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
            SyncTaskbarXamlOnOwnerThread(static_cast<HWND>(p), !needSync);
        }, hTb);
    }
    RefreshTaskbarGlassSurfaces(true);
    SweepExistingHardwareOsdWindows();
    RebindDefaultRenderVolumeEndpoint();
    StartWnfInstantSubscriptions();

    HPOWERNOTIFY hNotifyAc    = nullptr;
    HPOWERNOTIFY hNotifyBat   = nullptr;
    HPOWERNOTIFY hNotifyBri   = nullptr;
    HPOWERNOTIFY hNotifySaver = nullptr;
    HDEVNOTIFY   hNotifyUsb   = nullptr;

    if (g_alertHwnd) {
        hNotifyAc    = RegisterPowerSettingNotification(g_alertHwnd, &kGuidAcDcPowerSource,   DEVICE_NOTIFY_WINDOW_HANDLE);
        hNotifyBat   = RegisterPowerSettingNotification(g_alertHwnd, &kGuidBatteryPercentage, DEVICE_NOTIFY_WINDOW_HANDLE);
        hNotifyBri   = RegisterPowerSettingNotification(g_alertHwnd, &kGuidVideoBrightness,   DEVICE_NOTIFY_WINDOW_HANDLE);
        hNotifySaver = RegisterPowerSettingNotification(g_alertHwnd, &kGuidPowerSavingStatus, DEVICE_NOTIFY_WINDOW_HANDLE);

        DEV_BROADCAST_DEVICEINTERFACE_W dbFilter{};
        dbFilter.dbcc_size = sizeof(dbFilter);
        dbFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        dbFilter.dbcc_classguid = kGuidDevInterfaceUsbDevice;
        hNotifyUsb = RegisterDeviceNotificationW(g_alertHwnd, &dbFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
    }

    MSG msg{};
    while (g_running.load(std::memory_order_relaxed) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    StopWnfInstantSubscriptions();

    if (g_pEndVol && g_pVolClient) g_pEndVol->UnregisterControlChangeNotify(g_pVolClient);
    if (g_pVolClient) { g_pVolClient->Release(); g_pVolClient = nullptr; }
    if (g_pEndVol)    { g_pEndVol->Release();    g_pEndVol = nullptr; }
    if (g_pAudioDev)  { g_pAudioDev->Release();  g_pAudioDev = nullptr; }
    if (g_pAudioEnum) { g_pAudioEnum->Release(); g_pAudioEnum = nullptr; }

    if (hNotifyUsb)   UnregisterDeviceNotification(hNotifyUsb);
    if (hNotifySaver) UnregisterPowerSettingNotification(hNotifySaver);
    if (hNotifyBri)   UnregisterPowerSettingNotification(hNotifyBri);
    if (hNotifyBat)   UnregisterPowerSettingNotification(hNotifyBat);
    if (hNotifyAc)    UnregisterPowerSettingNotification(hNotifyAc);

    if (g_alertHwnd && IsWindow(g_alertHwnd))               { DestroyWindow(g_alertHwnd);        g_alertHwnd = nullptr; }
    if (g_cornerWidgetHwnd && IsWindow(g_cornerWidgetHwnd)) { DestroyWindow(g_cornerWidgetHwnd); g_cornerWidgetHwnd = nullptr; }

    ReleaseRenderResources();
    g_tbCachedRegion = {};
    g_wallpaperCache = {};
    g_usbKnownNames.clear();
    UnregisterClassW(kAlertClass, hInst);
    UnregisterClassW(kCornerClass, hInst);
    if (SUCCEEDED(hrCom)) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// 18. Native WinUI 3 Settings App (100% Hosted in Dedicated windhawk.exe -tool-mod)
//     Ported directly from modern-run-dialog-ui.wh.cpp (Win11 Settings style)
// ---------------------------------------------------------------------------
static std::wstring FindWindhawkExePath() {
    // 1. Check running windhawk.exe processes first
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe{ sizeof(pe) };
        if (Process32FirstW(snap, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, L"windhawk.exe") == 0) {
                    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                    if (hProc) {
                        wchar_t buf[MAX_PATH] = {};
                        DWORD sz = ARRAYSIZE(buf);
                        if (QueryFullProcessImageNameW(hProc, 0, buf, &sz) && sz > 0) {
                            CloseHandle(hProc);
                            CloseHandle(snap);
                            return buf;
                        }
                        CloseHandle(hProc);
                    }
                }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
    }
    // 2. Fallback: Windhawk registry / default Program Files path
    wchar_t regPath[MAX_PATH] = {};
    DWORD cb = sizeof(regPath);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Windhawk", L"AppPath",
                     RRF_RT_REG_SZ, nullptr, regPath, &cb) == ERROR_SUCCESS && regPath[0]) {
        std::wstring p = regPath;
        if (!p.empty() && p.back() != L'\\') p += L'\\';
        p += L"windhawk.exe";
        if (GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES) return p;
    }
    const wchar_t* kDefaultPath = L"C:\\Program Files\\Windhawk\\windhawk.exe";
    if (GetFileAttributesW(kDefaultPath) != INVALID_FILE_ATTRIBUTES) return kDefaultPath;
    return L"";
}

static DWORD WINAPI LaunchToolModAndOpenSettingsThreadProc(LPVOID) {
    UINT msgOpen = RegisterWindowMessageW(L"Windhawk_OpenTaskbarSettings_" WH_MOD_ID);
    HWND hHost = FindWindowW(kSettingsHostClassName, nullptr);
    if (!hHost || !IsWindow(hHost)) {
        std::wstring whExe = FindWindhawkExePath();
        if (!whExe.empty()) {
            std::wstring cmdLine = L"\"" + whExe + L"\" -tool-mod \"" WH_MOD_ID L"\"";
            std::vector<wchar_t> cmdBuf(cmdLine.begin(), cmdLine.end());
            cmdBuf.push_back(L'\0');
            STARTUPINFOW si{};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_FORCEOFFFEEDBACK;
            PROCESS_INFORMATION pi{};
            if (CreateProcessW(whExe.c_str(), cmdBuf.data(), nullptr, nullptr, FALSE,
                               NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                Wh_Log(L"Launched dedicated windhawk.exe -tool-mod process for Settings UI");
                for (int tries = 0; tries < 30; ++tries) {
                    Sleep(100);
                    hHost = FindWindowW(kSettingsHostClassName, nullptr);
                    if (hHost && IsWindow(hHost)) break;
                }
            } else {
                Wh_Log(L"Failed to launch windhawk.exe -tool-mod (err=%lu)", GetLastError());
            }
        }
    }
    if (hHost && IsWindow(hHost) && msgOpen != 0) {
        DWORD hostPid = 0;
        GetWindowThreadProcessId(hHost, &hostPid);
        if (hostPid != 0) AllowSetForegroundWindow(hostPid);
        PostMessageW(hHost, msgOpen, 0, 0);
    } else {
        Wh_Log(L"Modern Taskbar Settings host in windhawk.exe not available");
    }
    return 0;
}

void OpenModernTaskbarSettingsApp() {
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (g_msgOpenSettings == 0) {
        g_msgOpenSettings = RegisterWindowMessageW(L"Windhawk_OpenTaskbarSettings_" WH_MOD_ID);
    }
    HWND hHost = FindWindowW(kSettingsHostClassName, nullptr);
    if (hHost && IsWindow(hHost) && g_msgOpenSettings != 0) {
        DWORD hostPid = 0;
        GetWindowThreadProcessId(hHost, &hostPid);
        if (hostPid != 0) {
            AllowSetForegroundWindow(hostPid);
        }
        PostMessageW(hHost, g_msgOpenSettings, 0, 0);
        return;
    }
    // Strictly never run Settings UI in explorer.exe — spawn windhawk.exe -tool-mod asynchronously
    if (HANDLE hTh = CreateThread(nullptr, 0, LaunchToolModAndOpenSettingsThreadProc, nullptr, 0, nullptr)) {
        CloseHandle(hTh);
    }
}

void NotifyExplorerReloadSettings() {
    if (g_msgReloadSettings == 0) {
        g_msgReloadSettings = RegisterWindowMessageW(L"Windhawk_ReloadTaskbarSettings_" WH_MOD_ID);
    }
    if (HWND hAlert = FindWindowW(L"WindhawkLiquidGlassLiveAlertV8", nullptr); hAlert && IsWindow(hAlert)) {
        PostMessageW(hAlert, g_msgReloadSettings, 0, 0);
    }
}

static bool IsSystemAppLightTheme() {
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

static D2D1_COLOR_F GetWindowsSystemAccentColor(bool isLight) {
    DWORD argb = 0;
    BOOL opaque = FALSE;
    COLORREF c = RGB(0, 120, 212);
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque))) {
        BYTE r = static_cast<BYTE>((argb >> 16) & 0xFF);
        BYTE g = static_cast<BYTE>((argb >> 8) & 0xFF);
        BYTE b = static_cast<BYTE>(argb & 0xFF);
        if (r + g + b > 45) c = RGB(r, g, b);
    }
    float r = GetRValue(c) / 255.0f;
    float g = GetGValue(c) / 255.0f;
    float b = GetBValue(c) / 255.0f;
    if (!isLight) {
        r = std::min(1.0f, r * 0.68f + 0.32f);
        g = std::min(1.0f, g * 0.68f + 0.32f);
        b = std::min(1.0f, b * 0.68f + 0.32f);
    }
    return D2D1::ColorF(r, g, b, 1.0f);
}

static const wchar_t* GetPreferredFluentFontFamily() {
    return L"Segoe UI Variable Text";
}

static void ApplyWin11SettingsWindowFrame(HWND hWnd, bool isLight) {
    BOOL dark = isLight ? FALSE : TRUE;
    DwmSetWindowAttribute(hWnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));
    DWORD corner = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hWnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/, &corner, sizeof(corner));
    COLORREF captionCol = isLight ? RGB(243, 244, 247) : RGB(32, 32, 34);
    DwmSetWindowAttribute(hWnd, 35 /*DWMWA_CAPTION_COLOR*/, &captionCol, sizeof(captionCol));
}

static HICON LoadModernTaskbarSettingsIcon() {
    // Draw a crisp custom 32x32 ARGB Modern Taskbar pill icon so it never looks like a generic folder
    constexpr int S = 32;
    BITMAPV5HEADER bi{};
    bi.bV5Size        = sizeof(BITMAPV5HEADER);
    bi.bV5Width       = S;
    bi.bV5Height      = -S;
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00FF0000;
    bi.bV5GreenMask   = 0x0000FF00;
    bi.bV5BlueMask    = 0x000000FF;
    bi.bV5AlphaMask   = 0xFF000000;

    void* bits = nullptr;
    HDC hdc = GetDC(nullptr);
    HBITMAP hColor = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!hColor || !bits) return LoadIconW(nullptr, IDI_APPLICATION);

    auto* px = static_cast<uint32_t*>(bits);
    memset(px, 0, S * S * sizeof(uint32_t));
    auto putPx = [&](int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        if (x < 0 || x >= S || y < 0 || y >= S) return;
        px[y * S + x] = (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
    };
    // Rounded dark-blue glass app badge background
    for (int y = 2; y < S - 2; ++y) {
        for (int x = 2; x < S - 2; ++x) {
            float dx = std::max(0.0f, std::max(8.0f - x, x - 23.0f));
            float dy = std::max(0.0f, std::max(8.0f - y, y - 23.0f));
            float d = std::sqrt(dx * dx + dy * dy);
            if (d <= 6.2f) {
                uint8_t r = static_cast<uint8_t>(24 + (x * 20) / S);
                uint8_t g = static_cast<uint8_t>(112 + (y * 32) / S);
                uint8_t b = static_cast<uint8_t>(218 + (x * 25) / S);
                putPx(x, y, r, g, b, 255);
            }
        }
    }
    // Crisp white floating taskbar pill at bottom + 3 mini app dots
    for (int y = 20; y <= 25; ++y) {
        for (int x = 6; x <= 25; ++x) {
            float dx = std::max(0.0f, std::max(8.5f - x, x - 22.5f));
            float dy = std::abs(y - 22.5f);
            if (std::sqrt(dx * dx + dy * dy) <= 2.7f) {
                putPx(x, y, 255, 255, 255, 245);
            }
        }
    }
    for (int cx : { 11, 16, 21 }) {
        for (int y = 21; y <= 24; ++y) {
            for (int x = cx - 1; x <= cx + 1; ++x) {
                putPx(x, y, 22, 118, 220, 255);
            }
        }
    }
    // Subtle top Live Alert accent pill
    for (int y = 9; y <= 13; ++y) {
        for (int x = 9; x <= 22; ++x) {
            float dx = std::max(0.0f, std::max(11.0f - x, x - 20.0f));
            float dy = std::abs(y - 11.0f);
            if (std::sqrt(dx * dx + dy * dy) <= 2.1f) {
                putPx(x, y, 235, 246, 255, 220);
            }
        }
    }

    HBITMAP hMask = CreateBitmap(S, S, 1, 1, nullptr);
    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmMask = hMask;
    ii.hbmColor = hColor;
    HICON hIcon = CreateIconIndirect(&ii);
    DeleteObject(hColor);
    DeleteObject(hMask);
    return hIcon ? hIcon : LoadIconW(nullptr, IDI_APPLICATION);
}

static void DrawWin11NativeToggle(ID2D1RenderTarget* pRT, float rightX, float cy, UINT dpi,
                                  bool checked, bool isLight, bool hovered, D2D1_COLOR_F accentCol, bool amber = false) {
    float swW = DpiScaleF(40.0f, dpi);
    float swH = DpiScaleF(20.0f, dpi);
    float leftX = rightX - swW;
    D2D1_RECT_F rc = D2D1::RectF(leftX, cy - swH * 0.5f, rightX, cy + swH * 0.5f);
    float r = swH * 0.5f;

    D2D1_COLOR_F trackCol = checked
        ? (amber ? D2D1::ColorF(0.85f, 0.50f, 0.05f, 1.0f) : accentCol)
        : (isLight ? D2D1::ColorF(0.95f, 0.95f, 0.96f, 1.0f) : D2D1::ColorF(0.18f, 0.18f, 0.20f, 1.0f));
    D2D1_COLOR_F borderCol = checked
        ? trackCol
        : (isLight ? D2D1::ColorF(0.48f, 0.49f, 0.52f, 1.0f) : D2D1::ColorF(0.62f, 0.63f, 0.66f, 1.0f));
    D2D1_COLOR_F thumbCol = checked
        ? (isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) : D2D1::ColorF(0.06f, 0.06f, 0.08f, 1.0f))
        : (isLight ? D2D1::ColorF(0.36f, 0.37f, 0.40f, 1.0f) : D2D1::ColorF(0.82f, 0.83f, 0.86f, 1.0f));

    ID2D1SolidColorBrush *pTr = nullptr, *pBd = nullptr, *pTh = nullptr;
    if (SUCCEEDED(pRT->CreateSolidColorBrush(trackCol, &pTr))) {
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rc, r, r), pTr);
        pTr->Release();
    }
    if (!checked && SUCCEEDED(pRT->CreateSolidColorBrush(borderCol, &pBd))) {
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, r, r), pBd, DpiScaleF(1.2f, dpi));
        pBd->Release();
    }
    float thumbR = DpiScaleF(hovered ? 6.8f : 6.0f, dpi);
    float thumbCx = checked ? (rightX - r) : (leftX + r);
    if (SUCCEEDED(pRT->CreateSolidColorBrush(thumbCol, &pTh))) {
        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbCx, cy), thumbR, thumbR), pTh);
        pTh->Release();
    }
}

struct SettingsHitRect {
    int id = 0;
    RECT rc{};
    std::wstring strVal;
    int intVal = 0;
};

struct Win11SettingsState {
    HWND hWnd = nullptr;
    UINT dpi = 96;
    bool isLight = false;
    ModSettings settings{};
    int activeTab = 0;
    int scrollY = 0;
    int maxScroll = 0;
    int hoveredId = 0;
    HICON hAppIcon = nullptr;
    std::vector<SettingsHitRect> hits;

    ID2D1Factory* pD2DFactory = nullptr;
    IDWriteFactory* pDWriteFactory = nullptr;
    ID2D1HwndRenderTarget* pHwndRT = nullptr;
};

static bool EnsureSettingsGraphics(Win11SettingsState* st) {
    if (!st) return false;
    if (!st->pD2DFactory) {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &st->pD2DFactory))) return false;
    }
    if (!st->pDWriteFactory) {
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                       reinterpret_cast<IUnknown**>(&st->pDWriteFactory)))) return false;
    }
    return true;
}

static bool GetSettingBool(const ModSettings& s, const std::wstring& k) {
    if (k == L"enableSplitTaskbar")      return s.enableSplitTaskbar;
    if (k == L"replaceAppsPillOnAlert")  return s.replaceAppsPillOnAlert;
    if (k == L"enableMediaBadge")        return s.enableMediaBadge;
    if (k == L"enableTaskProgressBadge") return s.enableTaskProgressBadge;
    if (k == L"enableVpnBadge")          return s.enableVpnBadge;
    if (k == L"enableHotspotBadge")      return s.enableHotspotBadge;
    if (k == L"enablePrivacyBadges")     return s.enablePrivacyBadges;
    if (k == L"enableClockBadges")       return s.enableClockBadges;
    if (k == L"dismissOnHover")          return s.dismissOnHover;
    if (k == L"hideDefaultOsd")          return s.hideDefaultOsd;
    if (k == L"enableVolumeAlert")       return s.enableVolumeAlert;
    if (k == L"enableBrightnessAlert")   return s.enableBrightnessAlert;
    if (k == L"enableClipboardAlert")    return s.enableClipboardAlert;
    if (k == L"enableFileDeleteAlert")   return s.enableFileDeleteAlert;
    if (k == L"enableKeyLockAlert")      return s.enableKeyLockAlert;
    if (k == L"enableAdminAppAlert")     return s.enableAdminAppAlert;
    if (k == L"enableAppExitAlert")      return s.enableAppExitAlert;
    if (k == L"enableChargingAlert")     return s.enableChargingAlert;
    if (k == L"enableBatterySaverAlert") return s.enableBatterySaverAlert;
    if (k == L"enableNetworkAlert")      return s.enableNetworkAlert;
    if (k == L"enableBluetoothAlert")    return s.enableBluetoothAlert;
    if (k == L"enableUsbAlert")          return s.enableUsbAlert;
    if (k == L"enableRamRomAlert")       return s.enableRamRomAlert;
    if (k == L"enableDndFocusAlert")     return s.enableDndFocusAlert;
    return false;
}

static int GetSettingInt(const ModSettings& s, const std::wstring& k) {
    if (k == L"fixedPopupWidthDip")      return s.fixedPopupWidthDip;
    if (k == L"fixedPopupVertHeightDip") return s.fixedPopupVertHeightDip;
    if (k == L"iconPaddingDip")          return s.iconPaddingDip;
    if (k == L"pillCornerRadiusDip")     return s.pillCornerRadiusDip;
    if (k == L"pillVerticalMarginDip")   return s.pillVerticalMarginDip;
    if (k == L"glassClarity")            return s.glassClarity;
    if (k == L"fontScalePercent")        return s.fontScalePercent;
    if (k == L"displayDurationMs")       return s.displayDurationMs;
    if (k == L"quickAlertDurationMs")    return s.quickAlertDurationMs;
    return 0;
}

static std::wstring GetSettingStr(const ModSettings& s, const std::wstring& k) {
    if (k == L"popupPosition") return (s.popupPosition == PopupPosition::AdjacentTaskbar) ? L"adjacent_taskbar" : L"inline_taskbar";
    if (k == L"popupSizeMode") return (s.popupSizeMode == PopupSizeMode::SyncApps) ? L"sync_apps" : L"fixed";
    if (k == L"colorMode") {
        if (s.colorMode == ColorMode::Dark) return L"dark";
        if (s.colorMode == ColorMode::Light) return L"light";
        return L"auto";
    }
    if (k == L"language") {
        if (s.lang == UiLang::Vi) return L"vi";
        if (s.lang == UiLang::En) return L"en";
        return L"auto";
    }
    return L"";
}

// --- Native WinUI 3 ComboBox Flyout (ported from modern-run-dialog-ui) ---
struct ComboFlyoutState {
    std::vector<std::wstring> labels;
    int selectedIdx = 0;
    int hoverIdx = -1;
    int chosenIdx = -1;
    bool isLight = false;
    UINT dpi = 96;
    COLORREF accent = RGB(0, 120, 212);
    bool done = false;
};

static LRESULT CALLBACK ComboFlyoutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* fs = reinterpret_cast<ComboFlyoutState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_NCCREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }
        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE && fs) {
                fs->done = true;
                DestroyWindow(hwnd);
            }
            return 0;
        }
        case WM_KEYDOWN: {
            if (!fs) break;
            if (wParam == VK_ESCAPE) {
                fs->done = true;
                DestroyWindow(hwnd);
            } else if (wParam == VK_UP) {
                fs->hoverIdx = (fs->hoverIdx <= 0) ? static_cast<int>(fs->labels.size()) - 1 : fs->hoverIdx - 1;
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == VK_DOWN) {
                fs->hoverIdx = (fs->hoverIdx + 1) % static_cast<int>(fs->labels.size());
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == VK_RETURN) {
                fs->chosenIdx = (fs->hoverIdx >= 0) ? fs->hoverIdx : fs->selectedIdx;
                fs->done = true;
                DestroyWindow(hwnd);
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!fs) break;
            int y = GET_Y_LPARAM(lParam);
            int pad = MulDiv(5, fs->dpi, 96);
            int rowH = MulDiv(34, fs->dpi, 96);
            int idx = (y - pad) / rowH;
            if (idx < 0 || idx >= static_cast<int>(fs->labels.size())) idx = -1;
            if (idx != fs->hoverIdx) {
                fs->hoverIdx = idx;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!fs) break;
            int y = GET_Y_LPARAM(lParam);
            int pad = MulDiv(5, fs->dpi, 96);
            int rowH = MulDiv(34, fs->dpi, 96);
            int idx = (y - pad) / rowH;
            if (idx >= 0 && idx < static_cast<int>(fs->labels.size())) {
                fs->chosenIdx = idx;
            }
            fs->done = true;
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            if (fs) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                COLORREF bg  = fs->isLight ? RGB(249, 250, 252) : RGB(44, 44, 46);
                COLORREF brd = fs->isLight ? RGB(220, 222, 228) : RGB(68, 68, 72);
                COLORREF hov = fs->isLight ? RGB(236, 238, 243) : RGB(58, 59, 64);
                COLORREF txt = fs->isLight ? RGB(26, 27, 30)    : RGB(244, 245, 248);

                HBRUSH hBg = CreateSolidBrush(bg);
                FillRect(hdc, &rc, hBg);
                DeleteObject(hBg);

                HPEN hPen = CreatePen(PS_SOLID, 1, brd);
                HGDIOBJ oldP = SelectObject(hdc, hPen);
                HGDIOBJ oldB = SelectObject(hdc, GetStockObject(NULL_BRUSH));
                Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
                SelectObject(hdc, oldB);
                SelectObject(hdc, oldP);
                DeleteObject(hPen);

                int pad = MulDiv(5, fs->dpi, 96);
                int rowH = MulDiv(34, fs->dpi, 96);
                HFONT hFont = CreateFontW(-MulDiv(13, fs->dpi, 96), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");
                HGDIOBJ oldF = SelectObject(hdc, hFont);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, txt);

                for (int i = 0; i < static_cast<int>(fs->labels.size()); ++i) {
                    RECT rr{ pad, pad + i * rowH, rc.right - pad, pad + (i + 1) * rowH - MulDiv(2, fs->dpi, 96) };
                    if (i == fs->hoverIdx || i == fs->selectedIdx) {
                        HBRUSH hRow = CreateSolidBrush(hov);
                        HPEN hNull = CreatePen(PS_NULL, 0, 0);
                        HGDIOBJ op2 = SelectObject(hdc, hNull);
                        HGDIOBJ ob2 = SelectObject(hdc, hRow);
                        int r = MulDiv(5, fs->dpi, 96);
                        RoundRect(hdc, rr.left, rr.top, rr.right, rr.bottom, r, r);
                        SelectObject(hdc, ob2);
                        SelectObject(hdc, op2);
                        DeleteObject(hRow);
                        DeleteObject(hNull);
                    }
                    if (i == fs->selectedIdx) {
                        int pw = MulDiv(3, fs->dpi, 96);
                        int ph = MulDiv(16, fs->dpi, 96);
                        int py = (rr.top + rr.bottom - ph) / 2;
                        RECT pill{ rr.left + MulDiv(2, fs->dpi, 96), py, rr.left + MulDiv(2, fs->dpi, 96) + pw, py + ph };
                        HBRUSH hAcc = CreateSolidBrush(fs->accent);
                        FillRect(hdc, &pill, hAcc);
                        DeleteObject(hAcc);
                    }
                    RECT tr = rr;
                    tr.left += MulDiv(14, fs->dpi, 96);
                    tr.right -= MulDiv(10, fs->dpi, 96);
                    DrawTextW(hdc, fs->labels[i].c_str(), -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS);
                }

                SelectObject(hdc, oldF);
                DeleteObject(hFont);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static int ShowWinUI3ComboFlyout(HWND hOwner, const RECT& ctrlRect,
                                 const std::vector<std::wstring>& labels,
                                 int selectedIdx, bool isLight, UINT dpi) {
    if (labels.empty()) return -1;
    HINSTANCE hInst = GetModInstanceHandle();
    static bool s_reg = false;
    if (!s_reg) {
        WNDCLASSEXW wc{ sizeof(wc) };
        wc.style = CS_DROPSHADOW;
        wc.lpfnWndProc = ComboFlyoutWndProc;
        wc.hInstance = hInst;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kComboFlyoutClassName;
        RegisterClassExW(&wc);
        s_reg = true;
    }

    D2D1_COLOR_F acc = GetWindowsSystemAccentColor(isLight);
    ComboFlyoutState fs;
    fs.labels = labels;
    fs.selectedIdx = selectedIdx;
    fs.hoverIdx = selectedIdx;
    fs.isLight = isLight;
    fs.dpi = dpi;
    fs.accent = RGB(static_cast<BYTE>(acc.r * 255), static_cast<BYTE>(acc.g * 255), static_cast<BYTE>(acc.b * 255));

    POINT pt{ ctrlRect.left, ctrlRect.bottom };
    ClientToScreen(hOwner, &pt);
    int w = std::max<int>(ctrlRect.right - ctrlRect.left, MulDiv(185, dpi, 96));
    int pad = MulDiv(5, dpi, 96);
    int rowH = MulDiv(34, dpi, 96);
    int h = pad * 2 + rowH * static_cast<int>(labels.size());

    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (GetMonitorInfoW(hMon, &mi)) {
        if (pt.y + h > mi.rcWork.bottom) {
            POINT ptTop{ ctrlRect.left, ctrlRect.top };
            ClientToScreen(hOwner, &ptTop);
            pt.y = std::max<LONG>(mi.rcWork.top, ptTop.y - h - MulDiv(4, dpi, 96));
        }
        if (pt.x + w > mi.rcWork.right) pt.x = mi.rcWork.right - w - MulDiv(6, dpi, 96);
    }

    HWND hPop = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kComboFlyoutClassName, L"",
                                WS_POPUP, pt.x, pt.y + MulDiv(4, dpi, 96), w, h,
                                hOwner, nullptr, hInst, &fs);
    if (!hPop) return -1;

    DWORD cornerPref = 2;
    DwmSetWindowAttribute(hPop, 33, &cornerPref, sizeof(cornerPref));
    ShowWindow(hPop, SW_SHOW);
    SetFocus(hPop);

    MSG msg{};
    while (!fs.done && IsWindow(hPop) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return fs.chosenIdx;
}

// Control IDs for Settings hits
enum SettingsHitId : int {
    kHitNavBase = 1000,   // 1000..1003
    kHitResetAll = 1099,
    kHitToggleBase = 2000,
    kHitComboBase = 3000,
    kHitStepMinusBase = 4000,
    kHitStepPlusBase = 5000,
    kHitStepAutoBase = 6000,
};

struct StepperSpec {
    const wchar_t* key;
    int minVal;
    int maxVal;
    int stepVal;
    int defVal;
    const wchar_t* unit;
};

static const StepperSpec kStepperSpecs[] = {
    { L"fixedPopupWidthDip",      280, 1000, 20,  600, L"px" }, // 0
    { L"fixedPopupVertHeightDip", 180,  560, 20,  280, L"px" }, // 1
    { L"iconPaddingDip",            4,   24,  2,    8, L"px" }, // 2
    { L"pillCornerRadiusDip",       8,   24,  2,   20, L"px" }, // 3
    { L"pillVerticalMarginDip",     0,    6,  1,    3, L"px" }, // 4
    { L"glassClarity",           -100,  100,  5,  -85, L""   }, // 5
    { L"fontScalePercent",         90,  125,  5,  100, L"%"  }, // 6
    { L"displayDurationMs",      1000,15000,250, 3000, L"ms" }, // 7
    { L"quickAlertDurationMs",    500, 5000,100, 1300, L"ms" }, // 8
};

static void PaintWin11SettingsWindow(HWND hWnd, Win11SettingsState* st) {
    if (!hWnd) return;
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rcClient{};
    GetClientRect(hWnd, &rcClient);
    int w = rcClient.right - rcClient.left;
    int h = rcClient.bottom - rcClient.top;
    if (!st || w <= 0 || h <= 0) {
        EndPaint(hWnd, &ps);
        return;
    }

    if (!EnsureSettingsGraphics(st)) {
        HBRUSH hFallback = CreateSolidBrush(st->isLight ? RGB(243, 244, 247) : RGB(32, 32, 34));
        FillRect(hdc, &rcClient, hFallback);
        DeleteObject(hFallback);
        EndPaint(hWnd, &ps);
        return;
    }

    D2D1_SIZE_U sz = D2D1::SizeU(static_cast<UINT32>(w), static_cast<UINT32>(h));
    if (!st->pHwndRT) {
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
            96.0f, 96.0f);
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
            hWnd, sz, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
        if (FAILED(st->pD2DFactory->CreateHwndRenderTarget(rtProps, hwndProps, &st->pHwndRT))) {
            st->pHwndRT = nullptr;
        }
    } else {
        D2D1_SIZE_U curSz = st->pHwndRT->GetPixelSize();
        if (curSz.width != sz.width || curSz.height != sz.height) {
            st->pHwndRT->Resize(sz);
        }
    }

    ID2D1HwndRenderTarget* pRT = st->pHwndRT;
    if (pRT) {
        pRT->BeginDraw();
        pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

        UINT dpi = st->dpi;
        auto S = [&](float v) { return DpiScaleF(v, dpi); };
        bool isLight = st->isLight;
        bool vi = ComputeUseVietnamese(st->settings.lang);

        // Authentic Windows 11 Settings Palette (exact match to modern-run-dialog-ui.wh.cpp)
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
        st->pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(24.0f), L"", &pFmtHeader);
        st->pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.5f), L"", &pFmtTitle);
        st->pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(11.5f), L"", &pFmtSub);
        st->pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.5f), L"", &pFmtNav);
        st->pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(12.5f), L"", &pFmtCtl);
        st->pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.0f), L"", &pFmtRight);
        st->pDWriteFactory->CreateTextFormat(L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(16.0f), L"", &pFmtGlyph);
        if (pFmtNav)   pFmtNav->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if (pFmtCtl)   { pFmtCtl->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); pFmtCtl->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }
        if (pFmtRight) { pFmtRight->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING); pFmtRight->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }
        if (pFmtGlyph) { pFmtGlyph->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); pFmtGlyph->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }
        for (auto* tf : { pFmtTitle, pFmtSub, pFmtNav }) {
            if (tf) tf->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }

        st->hits.clear();
        auto addHit = [&](int id, const D2D1_RECT_F& r, const std::wstring& sv = L"", int iv = 0) {
            RECT ri{ static_cast<LONG>(r.left), static_cast<LONG>(r.top), static_cast<LONG>(r.right), static_cast<LONG>(r.bottom) };
            st->hits.push_back({ id, ri, sv, iv });
        };

        // 1. Left Navigation Sidebar (exact modern-run-dialog-ui layout)
        float sideW = S(238.0f);
        pRT->FillRectangle(D2D1::RectF(0, 0, sideW, static_cast<float>(h)), pBrSide);

        // App Header Badge on Sidebar (circular accent-tinted icon badge)
        {
            D2D1_RECT_F rBadge = D2D1::RectF(S(16), S(16), S(54), S(54));
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rBadge, S(19), S(19)), pBrChipSel);
            if (pFmtGlyph) pRT->DrawText(L"\uE74C", 1, pFmtGlyph, rBadge, pBrAccent);
            if (pFmtTitle) pRT->DrawText(L"Modern Taskbar", 14, pFmtTitle, D2D1::RectF(S(64), S(18), sideW - S(10), S(38)), pBrText);
            const wchar_t* subVer = L"v8.1.0";
            if (pFmtSub)   pRT->DrawText(subVer, static_cast<UINT32>(wcslen(subVer)), pFmtSub, D2D1::RectF(S(64), S(37), sideW - S(10), S(55)), pBrSub);
        }

        struct NavDef { int tab; const wchar_t* glyph; const wchar_t* label; };
        NavDef kNavs[] = {
            { 0, L"\uE74C", vi ? L"Bố cục & Popup"       : L"Layout & Popup" },
            { 1, L"\uE790", vi ? L"Giao diện & Ngôn ngữ" : L"Appearance & Language" },
            { 2, L"\uE945", vi ? L"Live Activity góc"    : L"Corner Live Activity" },
            { 3, L"\uEA8F", vi ? L"Thông báo Live Alerts": L"Live Alerts" }
        };
        float navY = S(76.0f);
        for (const auto& nv : kNavs) {
            D2D1_RECT_F rNav = D2D1::RectF(S(12), navY, sideW - S(12), navY + S(38));
            int navId = kHitNavBase + nv.tab;
            bool sel = (st->activeTab == nv.tab);
            bool hov = (st->hoveredId == navId);
            if (sel || hov) {
                pRT->FillRoundedRectangle(D2D1::RoundedRect(rNav, S(5), S(5)), sel ? pBrNavSel : pBrCardHov);
            }
            if (sel) {
                pRT->FillRoundedRectangle(D2D1::RoundedRect(
                    D2D1::RectF(rNav.left + S(1), rNav.top + S(10), rNav.left + S(4), rNav.bottom - S(10)), S(1.5f), S(1.5f)), pBrAccent);
            }
            if (pFmtGlyph) {
                pRT->DrawText(nv.glyph, static_cast<UINT32>(wcslen(nv.glyph)), pFmtGlyph,
                              D2D1::RectF(rNav.left + S(10), rNav.top, rNav.left + S(38), rNav.bottom), sel ? pBrAccent : pBrSub);
            }
            if (pFmtNav) {
                pRT->DrawText(nv.label, static_cast<UINT32>(wcslen(nv.label)), pFmtNav,
                              D2D1::RectF(rNav.left + S(44), rNav.top, rNav.right - S(8), rNav.bottom), pBrText);
            }
            addHit(navId, rNav, L"", nv.tab);
            navY += S(42.0f);
        }

        // Bottom Sidebar Reset-to-Defaults button
        {
            D2D1_RECT_F rRst = D2D1::RectF(S(12), static_cast<float>(h) - S(48), sideW - S(12), static_cast<float>(h) - S(14));
            bool hov = (st->hoveredId == kHitResetAll);
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rRst, S(5), S(5)), hov ? pBrCardHov : pBrCtlBg);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rRst, S(5), S(5)), pBrBorder, 1.0f);
            if (pFmtGlyph) pRT->DrawText(L"\uE777", 1, pFmtGlyph, D2D1::RectF(rRst.left + S(8), rRst.top, rRst.left + S(34), rRst.bottom), pBrSub);
            const wchar_t* rstTxt = vi ? L"Khôi phục mặc định" : L"Reset to Defaults";
            if (pFmtNav) pRT->DrawText(rstTxt, static_cast<UINT32>(wcslen(rstTxt)), pFmtNav,
                                       D2D1::RectF(rRst.left + S(38), rRst.top, rRst.right - S(8), rRst.bottom), pBrText);
            addHit(kHitResetAll, rRst);
        }

        // 2. Right Content Area (Breadcrumb Header + Grouped SettingsExpander Cards)
        float contentX = sideW + S(26.0f);
        float contentW = std::max(S(360.0f), static_cast<float>(w) - contentX - S(30.0f));
        pRT->PushAxisAlignedClip(D2D1::RectF(sideW, 0, static_cast<float>(w), static_cast<float>(h)), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        float curY = S(20.0f) - static_cast<float>(st->scrollY);
        std::wstring fullHeader = std::wstring(L"Modern Taskbar  \u203A  ") + kNavs[st->activeTab].label;
        if (pFmtHeader && pBrText) {
            pRT->DrawText(fullHeader.c_str(), static_cast<UINT32>(fullHeader.size()), pFmtHeader,
                          D2D1::RectF(contentX, curY, contentX + contentW, curY + S(38)), pBrText);
        }
        curY += S(52.0f);

        auto beginGroupCard = [&](const wchar_t* groupTitle, const wchar_t* groupSub, int rowCount, float rowH) -> std::pair<D2D1_RECT_F, float> {
            float headerH = S(62.0f);
            float totalH = headerH + rowCount * rowH;
            D2D1_RECT_F rc = D2D1::RectF(contentX, curY, contentX + contentW, curY + totalH);
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rc, S(6), S(6)), pBrCard);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, S(6), S(6)), pBrBorder, 1.0f);
            if (pFmtTitle) pRT->DrawText(groupTitle, static_cast<UINT32>(wcslen(groupTitle)), pFmtTitle, D2D1::RectF(rc.left + S(18), rc.top + S(13), rc.right - S(42), rc.top + S(33)), pBrText);
            if (pFmtSub)   pRT->DrawText(groupSub,   static_cast<UINT32>(wcslen(groupSub)),   pFmtSub,   D2D1::RectF(rc.left + S(18), rc.top + S(34), rc.right - S(42), rc.top + S(54)), pBrSub);
            if (pFmtGlyph) pRT->DrawText(L"\uE70E", 1, pFmtGlyph, D2D1::RectF(rc.right - S(40), rc.top + S(14), rc.right - S(14), rc.top + S(48)), pBrSub);
            curY += totalH + S(12.0f);
            return { rc, rc.top + headerH };
        };

        auto drawToggleRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int id,
                                 const wchar_t* key, const wchar_t* glyph,
                                 const wchar_t* title, const wchar_t* desc) {
            bool checked = GetSettingBool(st->settings, key);
            pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
            D2D1_RECT_F rRow = D2D1::RectF(grpRc.left + 1.0f, rowTop + 1.0f, grpRc.right - 1.0f, rowTop + rowH - 1.0f);
            bool hov = (st->hoveredId == id);
            if (hov) pRT->FillRectangle(rRow, pBrCardHov);
            float cy = rowTop + rowH * 0.5f;
            if (pFmtGlyph) pRT->DrawText(glyph, static_cast<UINT32>(wcslen(glyph)), pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
            if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(124), rowTop + S(31)), pBrText);
            if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(124), rowTop + rowH - S(6)), pBrSub);
            const wchar_t* stateTxt = checked ? (vi ? L"Bật" : L"On") : (vi ? L"Tắt" : L"Off");
            if (pFmtRight) {
                pRT->DrawText(stateTxt, static_cast<UINT32>(wcslen(stateTxt)), pFmtRight,
                              D2D1::RectF(grpRc.right - S(118), rowTop, grpRc.right - S(68), rowTop + rowH), pBrText);
            }
            DrawWin11NativeToggle(pRT, grpRc.right - S(18.0f), cy, dpi, checked, isLight, hov, colAccent);
            addHit(id, rRow, key, checked ? 0 : 1);
        };

        auto drawComboRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int id,
                                const wchar_t* key, const wchar_t* glyph,
                                const wchar_t* title, const wchar_t* desc, const std::wstring& curLabel) {
            pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
            if (pFmtGlyph) pRT->DrawText(glyph, static_cast<UINT32>(wcslen(glyph)), pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
            if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(215), rowTop + S(31)), pBrText);
            if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(215), rowTop + rowH - S(6)), pBrSub);
            D2D1_RECT_F rcCombo = D2D1::RectF(grpRc.right - S(196), rowTop + S(14), grpRc.right - S(18), rowTop + rowH - S(14));
            bool hov = (st->hoveredId == id);
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcCombo, S(4), S(4)), hov ? pBrCardHov : pBrCtlBg);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcCombo, S(4), S(4)), pBrBorder, 1.0f);
            if (pFmtNav) pRT->DrawText(curLabel.c_str(), static_cast<UINT32>(curLabel.size()), pFmtNav,
                                       D2D1::RectF(rcCombo.left + S(12), rcCombo.top, rcCombo.right - S(28), rcCombo.bottom), pBrText);
            if (pFmtGlyph) pRT->DrawText(L"\uE70D", 1, pFmtGlyph,
                                         D2D1::RectF(rcCombo.right - S(26), rcCombo.top, rcCombo.right - S(6), rcCombo.bottom), pBrSub);
            addHit(id, rcCombo, key);
        };

        auto drawStepperRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int specIdx,
                                  const wchar_t* glyph, const wchar_t* title, const wchar_t* desc) {
            const auto& sp = kStepperSpecs[specIdx];
            int curV = GetSettingInt(st->settings, sp.key);
            std::wstring valStr = std::to_wstring(curV) + sp.unit;
            int minId = kHitStepMinusBase + specIdx;
            int plsId = kHitStepPlusBase  + specIdx;
            int autId = kHitStepAutoBase  + specIdx;

            pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
            if (pFmtGlyph) pRT->DrawText(glyph, static_cast<UINT32>(wcslen(glyph)), pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
            if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(225), rowTop + S(31)), pBrText);
            if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(225), rowTop + rowH - S(6)), pBrSub);

            D2D1_RECT_F rcReset = D2D1::RectF(grpRc.right - S(72), rowTop + S(15), grpRc.right - S(18), rowTop + rowH - S(15));
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcReset, S(4), S(4)), st->hoveredId == autId ? pBrCardHov : pBrCtlBg);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcReset, S(4), S(4)), pBrBorder, 1.0f);
            if (pFmtCtl) pRT->DrawText(L"Auto", 4, pFmtCtl, rcReset, pBrText);
            addHit(autId, rcReset, sp.key, specIdx);

            D2D1_RECT_F rcPlus = D2D1::RectF(rcReset.left - S(34), rowTop + S(15), rcReset.left - S(6), rowTop + rowH - S(15));
            D2D1_RECT_F rcVal  = D2D1::RectF(rcPlus.left  - S(62), rowTop + S(15), rcPlus.left  - S(4), rowTop + rowH - S(15));
            D2D1_RECT_F rcMin  = D2D1::RectF(rcVal.left   - S(30), rowTop + S(15), rcVal.left   - S(4), rowTop + rowH - S(15));
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcMin, S(4), S(4)), st->hoveredId == minId ? pBrCardHov : pBrCtlBg);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcMin, S(4), S(4)), pBrBorder, 1.0f);
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcPlus, S(4), S(4)), st->hoveredId == plsId ? pBrCardHov : pBrCtlBg);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcPlus, S(4), S(4)), pBrBorder, 1.0f);
            if (pFmtCtl) {
                pRT->DrawText(L"\u2212", 1, pFmtCtl, rcMin, pBrText);
                pRT->DrawText(valStr.c_str(), static_cast<UINT32>(valStr.size()), pFmtCtl, rcVal, pBrText);
                pRT->DrawText(L"+", 1, pFmtCtl, rcPlus, pBrText);
            }
            addHit(minId, rcMin, sp.key, specIdx);
            addHit(plsId, rcPlus, sp.key, specIdx);
        };

        float rowH = S(62.0f);
        if (st->activeTab == 0) {
            auto [g1, y1] = beginGroupCard(
                vi ? L"Taskbar Tách Đôi & Viên thuốc" : L"Split Taskbar & Pills",
                vi ? L"Tùy chỉnh nền kính Liquid Glass và hình dáng viên thuốc" : L"Configure Liquid Glass taskbar pills and spacing",
                4, rowH);
            drawToggleRow(g1, y1 + 0 * rowH, rowH, kHitToggleBase + 0, L"enableSplitTaskbar", L"\uE74C",
                          vi ? L"Taskbar Tách Đôi Liquid Glass" : L"Liquid Glass Split Taskbar",
                          vi ? L"Tách thanh Taskbar thành Thanh ứng dụng & Khay trạng thái" : L"Split taskbar into optical Liquid Glass Apps & System Tray pills");
            drawStepperRow(g1, y1 + 1 * rowH, rowH, 2, L"\uE8AB",
                           vi ? L"Độ lề ngoài viên thuốc" : L"Pill Outer Padding",
                           vi ? L"Khoảng đệm ngang hai bên biểu tượng trong viên thuốc" : L"Horizontal padding around icons inside each pill");
            drawStepperRow(g1, y1 + 2 * rowH, rowH, 3, L"\uE91B",
                           vi ? L"Độ bo góc viên thuốc" : L"Pill Corner Radius",
                           vi ? L"Bán kính bo tròn các góc viên thuốc & Popup" : L"Corner radius for taskbar pills and popups");
            drawStepperRow(g1, y1 + 3 * rowH, rowH, 4, L"\uE76F",
                           vi ? L"Lề trên/dưới viên thuốc" : L"Pill Vertical Margin",
                           vi ? L"Khoảng cách giữa mép viên thuốc và cạnh Taskbar" : L"Vertical margin between pill edges and taskbar bounds");

            auto [g2, y2] = beginGroupCard(
                vi ? L"Vị trí & Kích thước Popup Live Alert" : L"Live Alert Popup Position & Size",
                vi ? L"Kiểm soát cách Popup biến hình trên hoặc nổi sát Taskbar" : L"Control how alerts morph inline or float adjacent to the taskbar",
                5, rowH);
            std::wstring posLbl = (st->settings.popupPosition == PopupPosition::AdjacentTaskbar)
                ? (vi ? L"Nổi sát mép Taskbar" : L"Adjacent to Taskbar")
                : (vi ? L"Ngay trên Taskbar" : L"Inline on Taskbar");
            drawComboRow(g2, y2 + 0 * rowH, rowH, kHitComboBase + 0, L"popupPosition", L"\uE8A7",
                         vi ? L"Vị trí hiển thị Popup" : L"Popup Position",
                         vi ? L"Biến hình ngay trên Taskbar hoặc nổi sát mép Taskbar" : L"Morph inline on taskbar or float adjacent to taskbar edge", posLbl);
            std::wstring szLbl = (st->settings.popupSizeMode == PopupSizeMode::SyncApps)
                ? (vi ? L"Khớp Thanh ứng dụng" : L"Match Apps Pill")
                : (vi ? L"Kích thước cố định" : L"Fixed Size");
            drawComboRow(g2, y2 + 1 * rowH, rowH, kHitComboBase + 1, L"popupSizeMode", L"\uE740",
                         vi ? L"Chế độ kích thước Popup" : L"Popup Size Mode",
                         vi ? L"Dùng độ rộng cố định hoặc khớp Thanh ứng dụng" : L"Fixed dimensions or sync with Apps Pill width", szLbl);
            drawStepperRow(g2, y2 + 2 * rowH, rowH, 0, L"\uE9A6",
                           vi ? L"Chiều rộng Popup ngang" : L"Horizontal Popup Width",
                           vi ? L"Chiều rộng Popup khi Taskbar ở cạnh Dưới / Trên" : L"Popup width on Bottom / Top taskbar");
            drawStepperRow(g2, y2 + 3 * rowH, rowH, 1, L"\uE9A5",
                           vi ? L"Chiều cao Popup dọc" : L"Vertical Popup Height",
                           vi ? L"Chiều cao thanh trượt khi Taskbar ở cạnh Trái / Phải" : L"Slider height on Left / Right taskbar");
            drawToggleRow(g2, y2 + 4 * rowH, rowH, kHitToggleBase + 1, L"replaceAppsPillOnAlert", L"\uE890",
                          vi ? L"Ẩn Thanh ứng dụng khi hiện Popup" : L"Hide Apps Pill During Popup",
                          vi ? L"Tạm ẩn Thanh ứng dụng (giữ nguyên Khay trạng thái)" : L"Temporarily hide Apps Pill while inline alert is shown");
        } else if (st->activeTab == 1) {
            auto [g1, y1] = beginGroupCard(
                vi ? L"Mặt kính Liquid Glass & Màu sắc" : L"Liquid Glass & Color Mode",
                vi ? L"Tinh chỉnh độ trong suốt, tông màu Sáng/Tối và cỡ chữ" : L"Tune optical glass clarity, color theme, and text scale",
                3, rowH);
            drawStepperRow(g1, y1 + 0 * rowH, rowH, 5, L"\uE706",
                           vi ? L"Độ trong suốt kính Liquid Glass" : L"Liquid Glass Clarity",
                           vi ? L"Âm = kính trong suốt thấy nền; Dương = kính mờ đậm" : L"Negative = clearer optical glass; Positive = stronger frosted tint");
            std::wstring colLbl = (st->settings.colorMode == ColorMode::Dark) ? (vi ? L"Luôn Tối" : L"Always Dark")
                                : (st->settings.colorMode == ColorMode::Light) ? (vi ? L"Luôn Sáng" : L"Always Light")
                                : (vi ? L"Tự động theo nền" : L"Auto (Adaptive)");
            drawComboRow(g1, y1 + 1 * rowH, rowH, kHitComboBase + 2, L"colorMode", L"\uE790",
                         vi ? L"Chế độ màu sắc" : L"Color Mode",
                         vi ? L"Tự thích ứng hình nền hoặc cố định Tối / Sáng" : L"Auto-adapt to wallpaper brightness or force Dark / Light", colLbl);
            drawStepperRow(g1, y1 + 2 * rowH, rowH, 6, L"\uE8D2",
                           vi ? L"Tỷ lệ cỡ chữ" : L"Font Scale",
                           vi ? L"Phóng to hoặc thu nhỏ chữ trên Popup Live Alerts" : L"Scale text size inside Live Alert popups");

            auto [g2, y2] = beginGroupCard(
                vi ? L"Ngôn ngữ hiển thị" : L"Display Language",
                vi ? L"Áp dụng cho cả Live Alerts và cửa sổ Cài đặt này" : L"Applies to both Live Alerts and this Settings window",
                1, rowH);
            std::wstring langLbl = (st->settings.lang == UiLang::Vi) ? L"Tiếng Việt"
                                 : (st->settings.lang == UiLang::En) ? L"English"
                                 : (vi ? L"Tự động theo Windows" : L"Auto (System)");
            drawComboRow(g2, y2 + 0 * rowH, rowH, kHitComboBase + 3, L"language", L"\uF2B7",
                         vi ? L"Ngôn ngữ giao diện" : L"Interface Language",
                         vi ? L"Chọn Tiếng Việt, English hoặc Tự động theo hệ thống" : L"Choose Vietnamese, English, or System Auto", langLbl);
        } else if (st->activeTab == 2) {
            auto [g1, y1] = beginGroupCard(
                vi ? L"Thẻ Live Activity góc trái / trên" : L"Corner Live Activity Pills",
                vi ? L"Bật hoặc tắt từng loại hoạt động ghim ở góc Taskbar" : L"Toggle persistent corner activity indicators on the taskbar",
                6, rowH);
            drawToggleRow(g1, y1 + 0 * rowH, rowH, kHitToggleBase + 2, L"enableMediaBadge", L"\uE8D6",
                          vi ? L"Phát nhạc (Ảnh bìa + Sóng nhạc)" : L"Media Player (Art + Waveform)",
                          vi ? L"Hiện ảnh bìa, sóng nhạc và bấm để mở đúng tab đang phát" : L"Show album art & waveform; click to jump to active media tab");
            drawToggleRow(g1, y1 + 1 * rowH, rowH, kHitToggleBase + 3, L"enableTaskProgressBadge", L"\uE896",
                          vi ? L"Tiến trình tác vụ (%)" : L"Task Progress Badge (%)",
                          vi ? L"Theo dõi tải xuống, chép/giải nén tệp và tự ẩn khi xong" : L"Track downloads, file copies & installs with auto-clear at 100%");
            drawToggleRow(g1, y1 + 2 * rowH, rowH, kHitToggleBase + 4, L"enableVpnBadge", L"\uE705",
                          vi ? L"Kết nối VPN" : L"VPN Live Activity",
                          vi ? L"Ghim trạng thái VPN ở góc và báo khi kết nối / ngắt VPN" : L"Pin VPN badge in corner pill and alert on connect/disconnect");
            drawToggleRow(g1, y1 + 3 * rowH, rowH, kHitToggleBase + 5, L"enableHotspotBadge", L"\uE704",
                          vi ? L"Điểm phát sóng (Mobile Hotspot)" : L"Mobile Hotspot Badge",
                          vi ? L"Hiện biểu tượng phát Wi-Fi khi bật Mobile Hotspot" : L"Show Mobile Hotspot indicator when active");
            drawToggleRow(g1, y1 + 4 * rowH, rowH, kHitToggleBase + 6, L"enablePrivacyBadges", L"\uE720",
                          vi ? L"Micro, Camera & Định vị" : L"Microphone, Camera & Location",
                          vi ? L"Cảnh báo quyền riêng tư khi ứng dụng dùng Mic/Cam/Vị trí" : L"Show live privacy badges when apps access Mic, Camera, or Location");
            drawToggleRow(g1, y1 + 5 * rowH, rowH, kHitToggleBase + 7, L"enableClockBadges", L"\uE916",
                          vi ? L"Đồng hồ (Hẹn giờ & Bấm giờ)" : L"Clock (Timer & Stopwatch)",
                          vi ? L"Hiển thị đếm ngược / bấm giờ từ ứng dụng Windows Clock" : L"Display running Timer or Stopwatch from Windows Clock");
        } else if (st->activeTab == 3) {
            auto [g1, y1] = beginGroupCard(
                vi ? L"Thời lượng & Hành vi thông minh" : L"Timing & Non-Blocking Behavior",
                vi ? L"Cân bằng thời lượng hiển thị và tự nhường chỗ khi bấm Taskbar" : L"Balance alert duration and hover-dismiss behavior",
                4, rowH);
            drawStepperRow(g1, y1 + 0 * rowH, rowH, 7, L"\uE823",
                           vi ? L"Thời lượng thông báo chính" : L"Main Alert Duration",
                           vi ? L"Thanh trượt Âm lượng/Độ sáng và thông báo chi tiết" : L"Sliders and detailed system notifications");
            drawStepperRow(g1, y1 + 1 * rowH, rowH, 8, L"\uE945",
                           vi ? L"Thời lượng thao tác nhanh" : L"Quick Action Duration",
                           vi ? L"Thời lượng ngắn cho phím khóa & Copy/Cut/Paste" : L"Short duration for lock keys & Copy/Cut/Paste");
            drawToggleRow(g1, y1 + 2 * rowH, rowH, kHitToggleBase + 8, L"dismissOnHover", L"\uE8B0",
                          vi ? L"Tự thu gọn khi rê chuột" : L"Auto-Dismiss on Hover",
                          vi ? L"Rê chuột vào thông báo chỉ đọc sẽ thu gọn để bấm Taskbar" : L"Dismiss read-only inline alerts on mouse hover");
            drawToggleRow(g1, y1 + 3 * rowH, rowH, kHitToggleBase + 9, L"hideDefaultOsd", L"\uED1A",
                          vi ? L"Ẩn OSD mặc định của Windows" : L"Hide Default Windows OSD",
                          vi ? L"Chặn thanh âm lượng/độ sáng gốc của Windows" : L"Suppress default Windows volume/brightness flyout");

            auto [g2, y2] = beginGroupCard(
                vi ? L"Phần cứng, Bàn phím & Clipboard" : L"Hardware, Keyboard & Clipboard",
                vi ? L"Phản hồi tức thì <10ms cho phím tắt và thanh trượt" : L"Instant <10ms feedback for shortcuts and interactive sliders",
                7, rowH);
            drawToggleRow(g2, y2 + 0 * rowH, rowH, kHitToggleBase + 10, L"enableVolumeAlert", L"\uE767",
                          vi ? L"Thanh trượt Âm lượng" : L"Volume Slider Alert",
                          vi ? L"Kéo chuột chỉnh âm lượng & bấm loa để tắt tiếng" : L"Interactive volume slider with drag, scroll & click-to-mute");
            drawToggleRow(g2, y2 + 1 * rowH, rowH, kHitToggleBase + 11, L"enableBrightnessAlert", L"\uE706",
                          vi ? L"Thanh trượt Độ sáng màn hình" : L"Brightness Slider Alert",
                          vi ? L"Kéo chuột chỉnh độ sáng trực tiếp trên Taskbar" : L"Interactive screen brightness slider");
            drawToggleRow(g2, y2 + 2 * rowH, rowH, kHitToggleBase + 12, L"enableClipboardAlert", L"\uE8C8",
                          vi ? L"Sao chép / Cắt / Dán (Clipboard)" : L"Clipboard (Copy / Cut / Paste)",
                          vi ? L"Đếm đủ 100% số ký tự văn bản, hình ảnh & tệp" : L"Full character count, image & file clipboard alerts");
            drawToggleRow(g2, y2 + 3 * rowH, rowH, kHitToggleBase + 13, L"enableFileDeleteAlert", L"\uE74D",
                          vi ? L"Xóa tệp vào Thùng rác" : L"Recycle Bin / File Delete",
                          vi ? L"Thông báo khi chuyển tệp hoặc thư mục vào Thùng rác" : L"Notify when items are moved to the Recycle Bin");
            drawToggleRow(g2, y2 + 4 * rowH, rowH, kHitToggleBase + 14, L"enableKeyLockAlert", L"\uE765",
                          vi ? L"Phím khóa (Caps / Num / Scroll / Insert)" : L"Lock Keys (Caps / Num / Scroll / Insert)",
                          vi ? L"Báo tức thì <10ms trạng thái BẬT / TẮT phím khóa" : L"Instant <10ms ON/OFF feedback for lock keys");
            drawToggleRow(g2, y2 + 5 * rowH, rowH, kHitToggleBase + 15, L"enableAdminAppAlert", L"\uEA18",
                          vi ? L"Ứng dụng Quyền Quản trị (UAC)" : L"Administrator App (UAC) Alert",
                          vi ? L"Báo khi có ứng dụng mở với quyền Administrator" : L"Notify when an app launches elevated as Administrator");
            drawToggleRow(g2, y2 + 6 * rowH, rowH, kHitToggleBase + 16, L"enableAppExitAlert", L"\uE8BB",
                          vi ? L"Thoát ứng dụng (Alt + F4)" : L"App Closed (Alt + F4)",
                          vi ? L"Thông báo khi đóng cửa sổ ứng dụng bằng Alt + F4" : L"Confirm when an app is closed with Alt + F4");

            auto [g3, y3] = beginGroupCard(
                vi ? L"Hệ thống, Pin & Kết nối" : L"System, Battery & Connectivity",
                vi ? L"Theo dõi sạc pin, mạng, Bluetooth, USB và tài nguyên máy" : L"Monitor battery milestones, network, Bluetooth, USB & resources",
                7, rowH);
            drawToggleRow(g3, y3 + 0 * rowH, rowH, kHitToggleBase + 17, L"enableChargingAlert", L"\uE83F",
                          vi ? L"Sạc pin & Mốc Pin (80/90/100%, 50/20/10/5%)" : L"Charging & Battery Milestones",
                          vi ? L"Báo cắm/rút sạc và các mốc sạc đầy / pin yếu" : L"Alert on charger state and key charge/low-battery milestones");
            drawToggleRow(g3, y3 + 1 * rowH, rowH, kHitToggleBase + 18, L"enableBatterySaverAlert", L"\uEBC0",
                          vi ? L"Tiết kiệm pin (Energy Saver)" : L"Energy Saver Alert",
                          vi ? L"Báo khi bật hoặc tắt chế độ Tiết kiệm pin" : L"Notify when Energy Saver toggles");
            drawToggleRow(g3, y3 + 2 * rowH, rowH, kHitToggleBase + 19, L"enableNetworkAlert", L"\uE701",
                          vi ? L"Mạng Wi-Fi / Ethernet / Máy bay" : L"Network & Airplane Mode",
                          vi ? L"Thông báo kết nối, mất mạng hoặc Chế độ máy bay" : L"Notify on Wi-Fi, Ethernet & Airplane Mode changes");
            drawToggleRow(g3, y3 + 3 * rowH, rowH, kHitToggleBase + 20, L"enableBluetoothAlert", L"\uE702",
                          vi ? L"Thiết bị Bluetooth" : L"Bluetooth Device Alerts",
                          vi ? L"Báo khi tai nghe / thiết bị Bluetooth kết nối hoặc ngắt" : L"Notify when Bluetooth peripherals connect or disconnect");
            drawToggleRow(g3, y3 + 4 * rowH, rowH, kHitToggleBase + 21, L"enableUsbAlert", L"\uE88E",
                          vi ? L"Ổ đĩa & Thiết bị USB" : L"USB Drive & Device Alerts",
                          vi ? L"Báo khi cắm hoặc rút ổ USB, thiết bị ngoại vi" : L"Notify when USB drives or devices are connected/removed");
            drawToggleRow(g3, y3 + 5 * rowH, rowH, kHitToggleBase + 22, L"enableRamRomAlert", L"\uE9D9",
                          vi ? L"Cảnh báo RAM / Ổ đĩa đầy" : L"High RAM / Low Disk Warnings",
                          vi ? L"Cảnh báo khi RAM hoặc dung lượng ổ đĩa chạm ngưỡng cao" : L"Warn when RAM usage is high or disk space is critically low");
            drawToggleRow(g3, y3 + 6 * rowH, rowH, kHitToggleBase + 23, L"enableDndFocusAlert", L"\uE708",
                          vi ? L"Không làm phiền (DND) & Focus" : L"Do Not Disturb & Focus Alerts",
                          vi ? L"Thông báo khi bật/tắt Không làm phiền hoặc Focus" : L"Notify when Do Not Disturb or Focus mode changes");
        }

        float totalContentH = curY + static_cast<float>(st->scrollY) + S(24.0f);
        st->maxScroll = std::max<int>(0, static_cast<int>(std::ceil(totalContentH - static_cast<float>(h))));
        st->scrollY = std::clamp(st->scrollY, 0, st->maxScroll);

        // Scrollbar thumb when scrollable
        if (st->maxScroll > 0) {
            float trackTop = S(14.0f);
            float trackBot = static_cast<float>(h) - S(14.0f);
            float trackH = trackBot - trackTop;
            float thumbH = std::max(S(34.0f), trackH * (static_cast<float>(h) / totalContentH));
            float ratio = static_cast<float>(st->scrollY) / static_cast<float>(st->maxScroll);
            float thumbY = trackTop + ratio * (trackH - thumbH);
            pRT->FillRoundedRectangle(D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(w) - S(10), thumbY, static_cast<float>(w) - S(5), thumbY + thumbH),
                S(2.5f), S(2.5f)), pBrSub);
        }

        pRT->PopAxisAlignedClip();

        HRESULT hr = pRT->EndDraw();
        auto safeRel = [](auto*& p) { if (p) { p->Release(); p = nullptr; } };
        safeRel(pFmtHeader); safeRel(pFmtTitle); safeRel(pFmtSub);
        safeRel(pFmtNav); safeRel(pFmtCtl); safeRel(pFmtRight); safeRel(pFmtGlyph);
        safeRel(pBrSide); safeRel(pBrCard); safeRel(pBrCardHov); safeRel(pBrBorder); safeRel(pBrDiv);
        safeRel(pBrText); safeRel(pBrSub); safeRel(pBrAccent); safeRel(pBrNavSel);
        safeRel(pBrCtlBg); safeRel(pBrChipSel);

        if (hr == D2DERR_RECREATE_TARGET) {
            if (st->pHwndRT) { st->pHwndRT->Release(); st->pHwndRT = nullptr; }
        }
    }
    EndPaint(hWnd, &ps);
}

static void RefreshSettingsWindowAfterChange(HWND hWnd, Win11SettingsState* st) {
    st->settings = ReadSettingsFromWindhawk(false);
    st->isLight = (st->settings.colorMode == ColorMode::Light) ? true
                : ((st->settings.colorMode == ColorMode::Dark) ? false : IsSystemAppLightTheme());
    bool vi = ComputeUseVietnamese(st->settings.lang);
    SetWindowTextW(hWnd, vi ? L"Cài đặt Modern Taskbar for Windows 11" : L"Modern Taskbar for Windows 11 Settings");
    ApplyWin11SettingsWindowFrame(hWnd, st->isLight);
    InvalidateRect(hWnd, nullptr, FALSE);
    NotifyExplorerReloadSettings();
}

LRESULT CALLBACK Win11TaskbarSettingsWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* st = reinterpret_cast<Win11SettingsState*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (g_msgReloadSettings != 0 && uMsg == g_msgReloadSettings && st) {
        st->settings = ReadSettingsFromWindhawk(false);
        st->isLight = (st->settings.colorMode == ColorMode::Light) ? true
                    : ((st->settings.colorMode == ColorMode::Dark) ? false : IsSystemAppLightTheme());
        ApplyWin11SettingsWindowFrame(hWnd, st->isLight);
        InvalidateRect(hWnd, nullptr, FALSE);
        return 1;
    }

    switch (uMsg) {
        case WM_NCCREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return DefWindowProcW(hWnd, uMsg, wParam, lParam);
        }
        case WM_CREATE: {
            if (st) {
                st->hWnd = hWnd;
                ApplyWin11SettingsWindowFrame(hWnd, st->isLight);
                st->hAppIcon = LoadModernTaskbarSettingsIcon();
                if (st->hAppIcon) {
                    SendMessageW(hWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(st->hAppIcon));
                    SendMessageW(hWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(st->hAppIcon));
                }
            }
            return 0;
        }
        case WM_DPICHANGED: {
            if (st) {
                st->dpi = HIWORD(wParam);
                if (st->dpi == 0) st->dpi = 96;
                if (const auto* prc = reinterpret_cast<const RECT*>(lParam)) {
                    SetWindowPos(hWnd, nullptr, prc->left, prc->top,
                                 prc->right - prc->left, prc->bottom - prc->top,
                                 SWP_NOZORDER | SWP_NOACTIVATE);
                }
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_SIZE:
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        case WM_GETMINMAXINFO: {
            auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
            UINT dpi = st ? st->dpi : 96;
            mmi->ptMinTrackSize.x = MulDiv(740, dpi, 96);
            mmi->ptMinTrackSize.y = MulDiv(480, dpi, 96);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            PaintWin11SettingsWindow(hWnd, st);
            return 0;
        case WM_MOUSEWHEEL: {
            if (st) {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                st->scrollY = std::clamp(st->scrollY - MulDiv(delta, st->dpi, 96) / 2, 0, st->maxScroll);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!st) break;
            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            int newHov = 0;
            for (auto it = st->hits.rbegin(); it != st->hits.rend(); ++it) {
                if (PtInRect(&it->rc, pt)) {
                    newHov = it->id;
                    break;
                }
            }
            if (newHov != st->hoveredId) {
                st->hoveredId = newHov;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!st) break;
            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            for (auto it = st->hits.rbegin(); it != st->hits.rend(); ++it) {
                if (!PtInRect(&it->rc, pt)) continue;
                int id = it->id;
                bool changed = false;

                if (id >= kHitNavBase && id < kHitNavBase + 4) {
                    st->activeTab = it->intVal;
                    st->scrollY = 0;
                    st->hoveredId = 0;
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                } else if (id == kHitResetAll) {
                    ClearAllInAppOverrides();
                    changed = true;
                } else if (id >= kHitToggleBase && id < kHitComboBase) {
                    SaveInAppIntOverride(it->strVal.c_str(), it->intVal);
                    changed = true;
                } else if (id >= kHitStepMinusBase && id < kHitStepPlusBase) {
                    const auto& sp = kStepperSpecs[it->intVal];
                    int cur = GetSettingInt(st->settings, sp.key);
                    SaveInAppIntOverride(sp.key, std::clamp(cur - sp.stepVal, sp.minVal, sp.maxVal));
                    changed = true;
                } else if (id >= kHitStepPlusBase && id < kHitStepAutoBase) {
                    const auto& sp = kStepperSpecs[it->intVal];
                    int cur = GetSettingInt(st->settings, sp.key);
                    SaveInAppIntOverride(sp.key, std::clamp(cur + sp.stepVal, sp.minVal, sp.maxVal));
                    changed = true;
                } else if (id >= kHitStepAutoBase && id < kHitStepAutoBase + 32) {
                    const auto& sp = kStepperSpecs[it->intVal];
                    SaveInAppIntOverride(sp.key, sp.defVal);
                    changed = true;
                } else if (id >= kHitComboBase && id < kHitStepMinusBase) {
                    bool vi = ComputeUseVietnamese(st->settings.lang);
                    std::vector<std::pair<std::wstring, std::wstring>> opts;
                    if (it->strVal == L"popupPosition") {
                        opts = {
                            { L"inline_taskbar",   vi ? L"Ngay trên Taskbar"   : L"Inline on Taskbar" },
                            { L"adjacent_taskbar", vi ? L"Nổi sát mép Taskbar" : L"Adjacent to Taskbar" }
                        };
                    } else if (it->strVal == L"popupSizeMode") {
                        opts = {
                            { L"fixed",     vi ? L"Kích thước cố định"  : L"Fixed Size" },
                            { L"sync_apps", vi ? L"Khớp Thanh ứng dụng" : L"Match Apps Pill" }
                        };
                    } else if (it->strVal == L"colorMode") {
                        opts = {
                            { L"auto",  vi ? L"Tự động theo nền" : L"Auto (Adaptive)" },
                            { L"dark",  vi ? L"Luôn Tối"         : L"Always Dark" },
                            { L"light", vi ? L"Luôn Sáng"        : L"Always Light" }
                        };
                    } else if (it->strVal == L"language") {
                        opts = {
                            { L"auto", vi ? L"Tự động theo Windows" : L"Auto (System)" },
                            { L"vi",   L"Tiếng Việt" },
                            { L"en",   L"English" }
                        };
                    }
                    std::wstring cur = GetSettingStr(st->settings, it->strVal);
                    std::vector<std::wstring> labels;
                    int selIdx = 0;
                    for (int oi = 0; oi < static_cast<int>(opts.size()); ++oi) {
                        labels.push_back(opts[oi].second);
                        if (opts[oi].first == cur) selIdx = oi;
                    }
                    int chosen = ShowWinUI3ComboFlyout(hWnd, it->rc, labels, selIdx, st->isLight, st->dpi);
                    if (chosen >= 0 && chosen < static_cast<int>(opts.size()) && chosen != selIdx) {
                        SaveInAppStringOverride(it->strVal.c_str(), opts[chosen].first);
                        changed = true;
                    }
                }

                if (changed) {
                    RefreshSettingsWindowAfterChange(hWnd, st);
                }
                return 0;
            }
            return 0;
        }
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
        case WM_NCDESTROY: {
            HWND expected = hWnd;
            g_hWin11SettingsWnd.compare_exchange_strong(expected, nullptr);
            if (st) {
                if (st->pHwndRT) { st->pHwndRT->Release(); st->pHwndRT = nullptr; }
                if (st->pDWriteFactory) { st->pDWriteFactory->Release(); st->pDWriteFactory = nullptr; }
                if (st->pD2DFactory) { st->pD2DFactory->Release(); st->pD2DFactory = nullptr; }
                if (st->hAppIcon) { DestroyIcon(st->hAppIcon); st->hAppIcon = nullptr; }
                delete st;
            }
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static void ShowOrFocusWin11TaskbarSettingsWindow() {
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (HWND hExisting = g_hWin11SettingsWnd.load(); hExisting && IsWindow(hExisting)) {
        if (IsIconic(hExisting)) ShowWindow(hExisting, SW_RESTORE);
        ShowWindow(hExisting, SW_SHOW);
        BringWindowToTop(hExisting);
        SetForegroundWindow(hExisting);
        return;
    }

    auto* st = new Win11SettingsState();
    st->settings = ReadSettingsFromWindhawk(false);
    POINT curPt{};
    GetCursorPos(&curPt);
    HMONITOR hMon = MonitorFromPoint(curPt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpi = 96;
    if (HDC hdc = GetDC(nullptr)) {
        dpi = static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX));
        ReleaseDC(nullptr, hdc);
    }
    st->dpi = dpi;
    st->isLight = (st->settings.colorMode == ColorMode::Light) ? true
                : ((st->settings.colorMode == ColorMode::Dark) ? false : IsSystemAppLightTheme());

    int winW = DpiScale(880, dpi);
    int winH = DpiScale(600, dpi);
    MONITORINFO mi{ sizeof(mi) };
    int posX = 120, posY = 90;
    if (GetMonitorInfoW(hMon, &mi)) {
        posX = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - winW) / 2;
        posY = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - winH) / 2;
    }

    bool vi = ComputeUseVietnamese(st->settings.lang);
    HWND hWnd = CreateWindowExW(
        WS_EX_APPWINDOW, kWin11SettingsClassName,
        vi ? L"Cài đặt Modern Taskbar for Windows 11" : L"Modern Taskbar for Windows 11 Settings",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        posX, posY, winW, winH,
        nullptr, nullptr, GetModInstanceHandle(), st);
    if (!hWnd) {
        delete st;
        return;
    }
    g_hWin11SettingsWnd.store(hWnd);
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
    BringWindowToTop(hWnd);
    SetForegroundWindow(hWnd);
}

static LRESULT CALLBACK SettingsHostWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == g_msgOpenSettings && g_msgOpenSettings != 0) {
        ShowOrFocusWin11TaskbarSettingsWindow();
        return 1;
    }
    if (uMsg == WM_CLOSE) {
        if (HWND hSet = g_hWin11SettingsWnd.load(); hSet && IsWindow(hSet)) {
            DestroyWindow(hSet);
        }
        DestroyWindow(hWnd);
        return 0;
    }
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI SettingsHostThreadProc(LPVOID param) {
    HANDLE hReadyEvent = static_cast<HANDLE>(param);
    MSG initMsg{};
    PeekMessageW(&initMsg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    HINSTANCE hInst = GetModInstanceHandle();
    WNDCLASSEXW wcSet{ sizeof(wcSet) };
    wcSet.style = CS_HREDRAW | CS_VREDRAW;
    wcSet.lpfnWndProc = Win11TaskbarSettingsWndProc;
    wcSet.hInstance = hInst;
    wcSet.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcSet.lpszClassName = kWin11SettingsClassName;
    RegisterClassExW(&wcSet);

    WNDCLASSEXW wcHost{ sizeof(wcHost) };
    wcHost.lpfnWndProc = SettingsHostWndProc;
    wcHost.hInstance = hInst;
    wcHost.lpszClassName = kSettingsHostClassName;
    RegisterClassExW(&wcHost);

    HWND hHost = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kSettingsHostClassName, L"",
        WS_POPUP, -32000, -32000, 0, 0, nullptr, nullptr, hInst, nullptr);
    if (hHost) {
        if (g_msgOpenSettings != 0) {
            ChangeWindowMessageFilterEx(hHost, g_msgOpenSettings, MSGFLT_ALLOW, nullptr);
        }
        g_hSettingsHostWnd.store(hHost);
        Wh_Log(L"Modern Taskbar Settings IPC host ready in windhawk.exe (HWND=%p, PID=%lu)", hHost, GetCurrentProcessId());
    }
    if (hReadyEvent) SetEvent(hReadyEvent);

    if (hHost) {
        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        HWND expected = hHost;
        g_hSettingsHostWnd.compare_exchange_strong(expected, nullptr);
    }

    UnregisterClassW(kSettingsHostClassName, hInst);
    UnregisterClassW(kWin11SettingsClassName, hInst);
    CoUninitialize();
    return 0;
}

static void StopSettingsHostCleanly() {
    if (HWND hHost = g_hSettingsHostWnd.load(); hHost && IsWindow(hHost)) {
        PostMessageW(hHost, WM_CLOSE, 0, 0);
    }
    if (g_hSettingsHostThread) {
        if (g_settingsHostThreadId != 0) {
            PostThreadMessageW(g_settingsHostThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_hSettingsHostThread, 2500);
        CloseHandle(g_hSettingsHostThread);
        g_hSettingsHostThread = nullptr;
        g_settingsHostThreadId = 0;
    }
}

// ---------------------------------------------------------------------------
// 19. Explorer Process Lifecycle & windhawk.exe -tool-mod Lifecycle
// ---------------------------------------------------------------------------
static bool IsCurrentProcessExplorer() {
    wchar_t path[MAX_PATH] = {};
    DWORD n = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (n == 0 || n >= ARRAYSIZE(path)) return false;
    const wchar_t* name = wcsrchr(path, L'\\');
    name = name ? (name + 1) : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

static BOOL TaskbarModInit() {
    // Chỉ chạy trong tiến trình explorer.exe chính sở hữu Shell_TrayWnd (chặn tuyệt đối lỗi hiện 2 Popup cùng lúc khi có explorer.exe phụ!)
    if (HWND hExistingTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        DWORD trayPid = 0;
        GetWindowThreadProcessId(hExistingTray, &trayPid);
        if (trayPid != 0 && trayPid != GetCurrentProcessId()) {
            return FALSE;
        }
    }
    g_singletonMutex = CreateMutexW(nullptr, TRUE, L"Local\\Windhawk_DynamicTaskbarLiveAlerts_PrimaryShell_V8");
    if (g_singletonMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
        return FALSE;
    }

    Wh_Log(L"Initializing Modern Taskbar for Windows 11 (Dynamic Taskbar) v8.1.0...");
    g_msgOpenSettings   = RegisterWindowMessageW(L"Windhawk_OpenTaskbarSettings_" WH_MOD_ID);
    g_msgReloadSettings = RegisterWindowMessageW(L"Windhawk_ReloadTaskbarSettings_" WH_MOD_ID);
    g_unloading.store(false, std::memory_order_relaxed);
    g_running.store(true, std::memory_order_relaxed);
    g_uiSettings = ReadSettingsFromWindhawk(false);

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        if (void* fnSw = reinterpret_cast<void*>(GetProcAddress(hUser32, "ShowWindow"))) {
            Wh_SetFunctionHook(fnSw, reinterpret_cast<void*>(HookedShowWindow), reinterpret_cast<void**>(&pOrigShowWindow));
        }
        if (void* fnSwp = reinterpret_cast<void*>(GetProcAddress(hUser32, "SetWindowPos"))) {
            Wh_SetFunctionHook(fnSwp, reinterpret_cast<void*>(HookedSetWindowPos), reinterpret_cast<void**>(&pOrigSetWindowPos));
        }
        if (void* fnSwca = reinterpret_cast<void*>(GetProcAddress(hUser32, "SetWindowCompositionAttribute"))) {
            Wh_SetFunctionHook(fnSwca, reinterpret_cast<void*>(HookedSetWindowCompositionAttribute),
                               reinterpret_cast<void**>(&pOrigSetWindowCompositionAttribute));
        }
    }

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        if (void* fnLoadLib = reinterpret_cast<void*>(GetProcAddress(hKernelBase, "LoadLibraryExW"))) {
            Wh_SetFunctionHook(fnLoadLib, reinterpret_cast<void*>(HookedLoadLibraryExW),
                               reinterpret_cast<void**>(&pOrigLoadLibraryExW));
        }
    }

    if (HMODULE hTaskbarDll = GetModuleHandleW(L"Taskbar.dll")) {
        TryHookTaskbarDllSymbols(hTaskbarDll, false);
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_watcherWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    g_uiThread      = CreateThread(nullptr, 0, ModUiThreadProc,        nullptr, 0, &g_uiThreadId);
    g_kbdHookThread = CreateThread(nullptr, 0, KbdHookThreadProc,      nullptr, 0, &g_kbdHookThreadId);
    g_watcherThread = CreateThread(nullptr, 0, EventWatcherThreadProc, nullptr, 0, nullptr);
    g_clockThread   = CreateThread(nullptr, 0, ClockMonitorThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

static void TaskbarModAfterInit() {
    if (HMODULE hTaskbarDll = GetModuleHandleW(L"Taskbar.dll")) {
        TryHookTaskbarDllSymbols(hTaskbarDll, true);
    }
    if (HWND hTb = FindCurrentProcessTaskbarWnd()) {
        RunOnWindowThread(hTb, [](void* p) {
            bool needSync = g_enableSplitTaskbarAtomic.load(std::memory_order_relaxed) ||
                            g_enablePrivacyBadgesAtomic.load(std::memory_order_relaxed);
            SyncTaskbarXamlOnOwnerThread(static_cast<HWND>(p), !needSync);
        }, hTb);
    }
    if (g_alertHwnd && IsWindow(g_alertHwnd)) {
        PostMessageW(g_alertHwnd, WM_APP_XAML_BOUNDS_CHANGED, 0, 0);
    }
}

static void TaskbarModSettingsChanged() {
    if (g_alertHwnd && IsWindow(g_alertHwnd)) {
        PostMessageW(g_alertHwnd, WM_APP_RELOAD_SETTINGS, 1, 0);
    }
}

static void TaskbarModBeforeUninit() {
    g_unloading.store(true, std::memory_order_relaxed);
    g_running.store(false, std::memory_order_relaxed);

    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_watcherWakeEvent) SetEvent(g_watcherWakeEvent);

    RestoreDefaultTaskbarEverything();
}

static void TaskbarModUninit() {
    Wh_Log(L"Uninitializing Modern Taskbar for Windows 11 (Dynamic Taskbar) v8.1.0...");
    g_unloading.store(true, std::memory_order_relaxed);
    g_running.store(false, std::memory_order_relaxed);

    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_watcherWakeEvent) SetEvent(g_watcherWakeEvent);

    if (g_kbdHookThread) {
        if (g_kbdHookThreadId != 0) {
            PostThreadMessageW(g_kbdHookThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_kbdHookThread, 1500);
        CloseHandle(g_kbdHookThread);
        g_kbdHookThread = nullptr;
    }

    if (g_alertHwnd && IsWindow(g_alertHwnd)) {
        PostMessageW(g_alertHwnd, WM_CLOSE, 0, 0);
    }

    if (g_clockThread) {
        WaitForSingleObject(g_clockThread, 2500);
        CloseHandle(g_clockThread);
        g_clockThread = nullptr;
    }
    if (g_watcherThread) {
        WaitForSingleObject(g_watcherThread, 2500);
        CloseHandle(g_watcherThread);
        g_watcherThread = nullptr;
    }
    if (g_uiThread) {
        if (g_uiThreadId != 0) {
            PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_uiThread, 2500);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(g_taskbandMutex);
        g_taskbandEntries.clear();
    }

    if (g_watcherWakeEvent) {
        CloseHandle(g_watcherWakeEvent);
        g_watcherWakeEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_singletonMutex) {
        ReleaseMutex(g_singletonMutex);
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
    }
}

static BOOL WhTool_ModInit() {
    g_unloading.store(false, std::memory_order_relaxed);
    ReadSettingsFromWindhawk(false);
    g_msgOpenSettings   = RegisterWindowMessageW(L"Windhawk_OpenTaskbarSettings_" WH_MOD_ID);
    g_msgReloadSettings = RegisterWindowMessageW(L"Windhawk_ReloadTaskbarSettings_" WH_MOD_ID);

    HANDLE hReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hSettingsHostThread = CreateThread(nullptr, 0, SettingsHostThreadProc, hReadyEvent, 0, &g_settingsHostThreadId);
    if (!g_hSettingsHostThread) {
        if (hReadyEvent) CloseHandle(hReadyEvent);
        return FALSE;
    }
    if (hReadyEvent) {
        WaitForSingleObject(hReadyEvent, 3000);
        CloseHandle(hReadyEvent);
    }
    Wh_Log(L"Modern Taskbar Settings tool-mod host started (PID=%lu)", GetCurrentProcessId());
    return TRUE;
}

static void WhTool_ModSettingsChanged() {
    ReadSettingsFromWindhawk(true);
    if (HWND hSet = g_hWin11SettingsWnd.load(); hSet && IsWindow(hSet) && g_msgReloadSettings != 0) {
        PostMessageW(hSet, g_msgReloadSettings, 1, 0);
    }
}

static void WhTool_ModUninit() {
    g_unloading.store(true, std::memory_order_relaxed);
    StopSettingsHostCleanly();
}

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    if (IsCurrentProcessExplorer()) {
        g_isExplorerProcess.store(true, std::memory_order_relaxed);
        return TaskbarModInit();
    }

    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
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
        g_toolModProcessMutex = CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook), nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isExplorerProcess.load(std::memory_order_relaxed)) {
        TaskbarModAfterInit();
        return;
    }
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileNameW(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) return;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    auto pCreateProcessInternalW =
        reinterpret_cast<CreateProcessInternalW_t>(GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!pCreateProcessInternalW) return;

    STARTUPINFOW si{};
    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi{};
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcessInternalW failed (err=%lu)", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModBeforeUninit() {
    if (g_isExplorerProcess.load(std::memory_order_relaxed)) {
        TaskbarModBeforeUninit();
    }
}

void Wh_ModSettingsChanged() {
    if (g_isExplorerProcess.load(std::memory_order_relaxed)) {
        TaskbarModSettingsChanged();
        return;
    }
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorerProcess.load(std::memory_order_relaxed)) {
        TaskbarModUninit();
        return;
    }
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModUninit();
    if (g_toolModProcessMutex) {
        ReleaseMutex(g_toolModProcessMutex);
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }
    ExitProcess(0);
}