// ==WindhawkMod==
// @id              nvme-led
// @name            NVME/HDD LED
// @description     Minimal per-drive disk activity LEDs for the Windows notification area, with an optional draggable always-on-top floating view.
// @version         1.0.0
// @author          BrainyBitz
// @github          https://github.com/brainybitz
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lpdh -lgdi32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# NVME/HDD LED

A deliberately tiny disk-activity indicator designed to behave like a physical HDD/SSD activity LED.

- One notification-area icon per configured drive.
- Each tray icon is a large outlined drive letter: `C`, `G`, etc., with a transparent background.
- Black letter with a light outline = idle.
- Black letter with a bright red outline flicker = activity. The flicker is a visual busy signal, not one flash per individual I/O request.
- By default, every currently mounted local disk gets a tray letter. You can still specify a custom drive list in settings.
- Left-click a tray letter to start a floating view with that drive. While the floating view is open, left-clicking other tray letters adds them.
- Left-click a letter inside the floating view to remove just that drive. A click only removes on button release over the same letter; dragging moves the window instead.
- Right-click any tray letter for **Reset NVME/HDD LED**, **Remove <drive>**, or **Quit NVME/HDD LED**.
- The floating view is borderless, always on top, and can be dragged from anywhere with the normal arrow cursor.
- Hovering the floating view shows: `Drag to move. Click letter to clear.` / `Right click to close view.`.
- Right-click the floating view to hide it.
- No graphs, transfer-rate panels, or other monitoring UI.

Windows controls whether a notification icon is visible beside the clock or placed
in the overflow menu. Pin/drag each letter into the visible tray once if desired.

Because this Windhawk version runs inside the shell Explorer process, Windows 11 may
label its entry as `Windows Explorer` under **Other system tray icons**. The tray
icons and their hover text still identify NVME/HDD LED normally.

This mod runs only in the shell Explorer process and creates a small worker thread
and hidden message window. It does not hook taskbar internals.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- drive_list: ALL
  $name: Drives
  $description: ALL shows every mounted local fixed/removable disk. Or enter comma/space separated drive letters, for example C,D,G.
- poll_interval_ms: 50
  $name: Poll interval (ms)
  $description: How often drive activity is sampled. Lower values make the indicator react faster.
- activity_hold_ms: 220
  $name: Activity persistence (ms)
  $description: How long a recent I/O event keeps the rapid LED flicker alive.
- flicker_interval_ms: 50
  $name: Flicker interval (ms)
  $description: How quickly the LED alternates while activity is present. 50 ms gives a fast hardware-style flicker.
- floating_font_size: 20
  $name: Floating font size
  $description: Point size used by the optional floating view.
- floating_opacity: 235
  $name: Floating opacity
  $description: 40-255. 255 is fully opaque.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <commctrl.h>

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <cwctype>
#include <string>
#include <vector>

namespace {

constexpr UINT WM_TRAYICON = WM_APP + 21;
constexpr UINT WM_APPLY_SETTINGS = WM_APP + 22;
constexpr UINT_PTR TIMER_ACTIVITY = 1;
constexpr UINT TRAY_ID_BASE = 4200;
constexpr UINT IDM_TRAY_RESET = 9101;
constexpr UINT IDM_TRAY_REMOVE = 9102;
constexpr UINT IDM_TRAY_QUIT = 9103;
constexpr wchar_t kHostClass[] = L"WindhawkHddLedHost";
constexpr wchar_t kFloatClass[] = L"WindhawkHddLedFloat";

struct DriveState {
    wchar_t letter = L'?';
    PDH_HCOUNTER counter = nullptr;
    bool counterValid = false;
    bool lit = false;
    bool activityEnvelope = false;
    bool flickerPhase = false;
    ULONGLONG lastActivityTick = 0;
    ULONGLONG lastFlickerTick = 0;
    HICON idleIcon = nullptr;
    HICON activeIcon = nullptr;
    NOTIFYICONDATAW nid{};
    bool trayAdded = false;
};

HWND g_hostWnd = nullptr;
HWND g_floatWnd = nullptr;
HWND g_floatTooltip = nullptr;
HANDLE g_workerThread = nullptr;
UINT g_taskbarCreatedMsg = 0;
PDH_HQUERY g_pdhQuery = nullptr;
std::vector<DriveState> g_drives;
std::vector<wchar_t> g_floatDrives;
std::vector<wchar_t> g_removedDrives;
bool g_quitRequested = false;

// Floating-window pointer/drag state. A drive is removed only when a genuine
// click completes over the same letter; any drag gesture only moves the box.
bool g_floatMouseDown = false;
bool g_floatDragging = false;
POINT g_floatDownScreen{};
RECT g_floatStartRect{};
int g_floatPressedIndex = -1;

bool g_autoAllDrives = true;
DWORD g_lastEligibleDriveMask = 0;
ULONGLONG g_lastDriveScanTick = 0;

int g_pollMs = 50;
int g_holdMs = 220;
int g_flickerMs = 50;
int g_floatFontPt = 20;
BYTE g_floatOpacity = 235;
bool g_floatVisible = false;
bool g_shuttingDown = false;
HANDLE g_singletonMutex = nullptr;
bool g_hostClassRegistered = false;
bool g_floatClassRegistered = false;
ULONGLONG g_lastTrayRetryTick = 0;


int CALLBACK FontEnumProc(const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM lParam) {
    *reinterpret_cast<bool*>(lParam) = true;
    return 0;  // One match is enough.
}

bool IsFontInstalled(const wchar_t* faceName) {
    HDC dc = GetDC(nullptr);
    if (!dc) return false;

    LOGFONTW lf{};
    lf.lfCharSet = DEFAULT_CHARSET;
    wcsncpy_s(lf.lfFaceName, faceName, _TRUNCATE);

    bool found = false;
    EnumFontFamiliesExW(dc, &lf, FontEnumProc,
                        reinterpret_cast<LPARAM>(&found), 0);
    ReleaseDC(nullptr, dc);
    return found;
}

const wchar_t* PreferredDriveFont() {
    // Prefer Nexa if the user has it installed. Some Nexa packages register
    // the bold face as a separate family, so check both common names.
    static const wchar_t* font = []() -> const wchar_t* {
        if (IsFontInstalled(L"Nexa Bold")) return L"Nexa Bold";
        if (IsFontInstalled(L"Nexa")) return L"Nexa";
        if (IsFontInstalled(L"Corbel")) return L"Corbel";
        return L"Segoe UI";
    }();
    return font;
}


GUID DriveTrayGuid(wchar_t letter) {
    // Stable, deterministic GUID per drive letter. Using NIF_GUID makes the
    // notification icon identity independent of the host HWND/uID pair and
    // prevents accidental duplicate icons if Explorer recreates our host.
    GUID guid{
        static_cast<DWORD>(0xD15C0000u + static_cast<unsigned>(letter)),
        0x8A31,
        0x4E67,
        {0xA5, 0x27, 0x6F, 0x84, 0x9A, 0xC0, 0x10,
         static_cast<BYTE>(letter)}
    };
    return guid;
}

bool IsShellExplorerProcess() {
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        return false;
    }

    DWORD shellPid = 0;
    GetWindowThreadProcessId(shellWindow, &shellPid);
    return shellPid == GetCurrentProcessId();
}

int ClampInt(int value, int low, int high) {
    return std::max(low, std::min(value, high));
}

bool IsAllDrivesSetting(const std::wstring& text) {
    std::wstring compact;
    for (wchar_t ch : text) {
        if (!iswspace(ch)) compact.push_back(static_cast<wchar_t>(towupper(ch)));
    }
    return compact.empty() || compact == L"ALL" || compact == L"*";
}

DWORD EligibleLocalDriveMask() {
    DWORD present = GetLogicalDrives();
    DWORD eligible = 0;
    for (int i = 0; i < 26; ++i) {
        DWORD bit = 1u << i;
        if (!(present & bit)) continue;

        wchar_t root[] = L"A:\\";
        root[0] = static_cast<wchar_t>(L'A' + i);
        UINT type = GetDriveTypeW(root);
        if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE ||
            type == DRIVE_RAMDISK) {
            eligible |= bit;
        }
    }
    return eligible;
}

std::vector<wchar_t> EnumerateLocalDriveLetters() {
    std::vector<wchar_t> result;
    DWORD mask = EligibleLocalDriveMask();
    for (int i = 0; i < 26; ++i) {
        if (mask & (1u << i)) {
            result.push_back(static_cast<wchar_t>(L'A' + i));
        }
    }
    if (result.empty()) result.push_back(L'C');
    return result;
}

std::vector<wchar_t> ParseDriveLetters(const std::wstring& text) {
    std::vector<wchar_t> result;
    std::wstring token;

    auto commitToken = [&]() {
        for (wchar_t ch : token) {
            if (iswalpha(ch)) {
                wchar_t letter = static_cast<wchar_t>(towupper(ch));
                if (letter >= L'A' && letter <= L'Z' &&
                    std::find(result.begin(), result.end(), letter) == result.end()) {
                    result.push_back(letter);
                }
                break;
            }
        }
        token.clear();
    };

    for (size_t i = 0; i <= text.size(); ++i) {
        wchar_t ch = (i < text.size()) ? text[i] : L',';
        if (ch == L',' || ch == L';' || iswspace(ch)) {
            if (!token.empty()) {
                commitToken();
            }
        } else {
            token.push_back(ch);
        }
    }

    if (result.empty()) {
        result.push_back(L'C');
    }

    return result;
}

HICON CreateLetterIcon(wchar_t letter, COLORREF outlineColor) {
    // Large black glyph with a contrasting outline on a transparent canvas.
    // The black center remains legible on light taskbars, while the outline
    // keeps the letter readable on dark taskbars. Activity changes only the
    // outline color, preserving a stable drive-letter silhouette.
    constexpr int kSize = 32;
    constexpr int kOutlineRadius = 2;

    auto createMask = [&](bool outlined, void** bitsOut) -> HBITMAP {
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = kSize;
        bmi.bmiHeader.biHeight = -kSize;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        HDC screenDc = GetDC(nullptr);
        HBITMAP bitmap = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS,
                                          bitsOut, nullptr, 0);
        ReleaseDC(nullptr, screenDc);
        if (!bitmap || !*bitsOut) return nullptr;

        HDC dc = CreateCompatibleDC(nullptr);
        HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
        RECT full{0, 0, kSize, kSize};
        HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(dc, &full, black);
        DeleteObject(black);

        HFONT font = CreateFontW(
            -30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS,
            PreferredDriveFont());
        HGDIOBJ oldFont = SelectObject(dc, font);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));

        wchar_t glyph[2] = {letter, L'\0'};
        auto drawAtOffset = [&](int dx, int dy) {
            RECT rc{dx, dy, kSize + dx, kSize + dy};
            DrawTextW(dc, glyph, 1, &rc,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        };

        if (outlined) {
            for (int y = -kOutlineRadius; y <= kOutlineRadius; ++y) {
                for (int x = -kOutlineRadius; x <= kOutlineRadius; ++x) {
                    if (x * x + y * y <= kOutlineRadius * kOutlineRadius) {
                        drawAtOffset(x, y);
                    }
                }
            }
        } else {
            drawAtOffset(0, 0);
        }

        SelectObject(dc, oldFont);
        SelectObject(dc, oldBitmap);
        DeleteObject(font);
        DeleteDC(dc);
        return bitmap;
    };

    void* outlineBits = nullptr;
    void* fillBits = nullptr;
    HBITMAP outlineBitmap = createMask(true, &outlineBits);
    HBITMAP fillBitmap = createMask(false, &fillBits);
    if (!outlineBitmap || !fillBitmap || !outlineBits || !fillBits) {
        if (outlineBitmap) DeleteObject(outlineBitmap);
        if (fillBitmap) DeleteObject(fillBitmap);
        return nullptr;
    }

    // Build the final ARGB icon into the outline bitmap. GDI does not create
    // useful alpha values for text, so mask luminance becomes our alpha.
    BYTE outR = GetRValue(outlineColor);
    BYTE outG = GetGValue(outlineColor);
    BYTE outB = GetBValue(outlineColor);
    auto* outlinePixels = static_cast<DWORD*>(outlineBits);
    auto* fillPixels = static_cast<DWORD*>(fillBits);

    for (int i = 0; i < kSize * kSize; ++i) {
        DWORD op = outlinePixels[i];
        DWORD fp = fillPixels[i];

        BYTE ob = static_cast<BYTE>(op & 0xFF);
        BYTE og = static_cast<BYTE>((op >> 8) & 0xFF);
        BYTE or_ = static_cast<BYTE>((op >> 16) & 0xFF);
        BYTE fb = static_cast<BYTE>(fp & 0xFF);
        BYTE fg = static_cast<BYTE>((fp >> 8) & 0xFF);
        BYTE fr = static_cast<BYTE>((fp >> 16) & 0xFF);

        BYTE outlineAlpha = std::max(or_, std::max(og, ob));
        BYTE fillAlpha = std::max(fr, std::max(fg, fb));
        BYTE borderAlpha = outlineAlpha > fillAlpha
                               ? static_cast<BYTE>(outlineAlpha - fillAlpha)
                               : 0;
        BYTE alpha = std::max(outlineAlpha, fillAlpha);

        // Black fill contributes no RGB. Only the outline contributes color.
        // Values are premultiplied for a 32-bit alpha icon.
        BYTE r = static_cast<BYTE>((outR * borderAlpha) / 255);
        BYTE g = static_cast<BYTE>((outG * borderAlpha) / 255);
        BYTE b = static_cast<BYTE>((outB * borderAlpha) / 255);

        outlinePixels[i] = (static_cast<DWORD>(alpha) << 24) |
                           (static_cast<DWORD>(r) << 16) |
                           (static_cast<DWORD>(g) << 8) |
                           static_cast<DWORD>(b);
    }

    DeleteObject(fillBitmap);

    HBITMAP maskBitmap = CreateBitmap(kSize, kSize, 1, 1, nullptr);
    if (!maskBitmap) {
        DeleteObject(outlineBitmap);
        return nullptr;
    }
    HDC maskDc = CreateCompatibleDC(nullptr);
    HGDIOBJ oldMask = SelectObject(maskDc, maskBitmap);
    PatBlt(maskDc, 0, 0, kSize, kSize, BLACKNESS);
    SelectObject(maskDc, oldMask);
    DeleteDC(maskDc);

    ICONINFO iconInfo{};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = outlineBitmap;
    iconInfo.hbmMask = maskBitmap;

    HICON icon = CreateIconIndirect(&iconInfo);
    DeleteObject(maskBitmap);
    DeleteObject(outlineBitmap);
    return icon;
}

bool DeleteTrayRegistration(DriveState& drive, size_t index) {
    // Delete by the stable GUID first. Do this even when trayAdded is false:
    // NIM_MODIFY can fail and clear our bookkeeping while Explorer still has
    // the old icon registered, which is how a ghost icon can be left behind.
    NOTIFYICONDATAW byGuid{};
    byGuid.cbSize = sizeof(byGuid);
    byGuid.hWnd = g_hostWnd;
    byGuid.uID = TRAY_ID_BASE + static_cast<UINT>(index);
    byGuid.uFlags = NIF_GUID;
    byGuid.guidItem = DriveTrayGuid(drive.letter);
    BOOL guidDeleted = Shell_NotifyIconW(NIM_DELETE, &byGuid);

    // Also try the traditional HWND/uID identity as a cleanup fallback. Older
    // registrations from an earlier development build may not have used the
    // GUID identity consistently.
    NOTIFYICONDATAW byId{};
    byId.cbSize = sizeof(byId);
    byId.hWnd = g_hostWnd;
    byId.uID = TRAY_ID_BASE + static_cast<UINT>(index);
    BOOL idDeleted = Shell_NotifyIconW(NIM_DELETE, &byId);

    drive.trayAdded = false;
    Wh_Log(L"%c: tray delete guid=%d id=%d", drive.letter,
           guidDeleted, idDeleted);
    return guidDeleted || idDeleted;
}

void DeleteTrayIcons() {
    for (size_t i = 0; i < g_drives.size(); ++i) {
        auto& drive = g_drives[i];
        DeleteTrayRegistration(drive, i);
        if (drive.idleIcon) DestroyIcon(drive.idleIcon);
        if (drive.activeIcon) DestroyIcon(drive.activeIcon);
        drive.idleIcon = nullptr;
        drive.activeIcon = nullptr;
    }
}

void CloseCounters() {
    if (g_pdhQuery) {
        PdhCloseQuery(g_pdhQuery);
        g_pdhQuery = nullptr;
    }
    for (auto& drive : g_drives) {
        drive.counter = nullptr;
        drive.counterValid = false;
    }
}

void BuildTooltip(DriveState& drive) {
    wchar_t root[] = L"C:\\";
    root[0] = drive.letter;

    wchar_t volumeName[MAX_PATH]{};
    if (GetVolumeInformationW(root, volumeName, ARRAYSIZE(volumeName), nullptr,
                              nullptr, nullptr, nullptr, 0) &&
        volumeName[0]) {
        // Explicit line breaks keep the standard tray tooltip compact and
        // readable instead of relying on Windows to wrap a long sentence.
        swprintf(drive.nid.szTip, ARRAYSIZE(drive.nid.szTip),
                 L"%c: %s\nNVME/HDD LED\nClick to add to floating view",
                 drive.letter, volumeName);
    } else {
        swprintf(drive.nid.szTip, ARRAYSIZE(drive.nid.szTip),
                 L"%c:\nNVME/HDD LED\nClick to add to floating view", drive.letter);
    }
}

bool AddTrayIcon(DriveState& drive, size_t index) {
    drive.nid = {};
    drive.nid.cbSize = sizeof(drive.nid);
    drive.nid.hWnd = g_hostWnd;
    drive.nid.uID = TRAY_ID_BASE + static_cast<UINT>(index);
    drive.nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID;
    drive.nid.guidItem = DriveTrayGuid(drive.letter);
    drive.nid.uCallbackMessage = WM_TRAYICON;
    drive.nid.hIcon = drive.lit ? drive.activeIcon : drive.idleIcon;
    BuildTooltip(drive);

    if (!drive.nid.hIcon) {
        Wh_Log(L"%c: custom icon creation failed", drive.letter);
        return false;
    }

    BOOL ok = Shell_NotifyIconW(NIM_ADD, &drive.nid);
    drive.trayAdded = ok != FALSE;
    Wh_Log(L"%c: Shell_NotifyIcon(NIM_ADD) = %d", drive.letter, ok);
    return ok != FALSE;
}

void ReAddTrayIcons() {
    g_lastTrayRetryTick = 0;
    for (size_t i = 0; i < g_drives.size(); ++i) {
        // Clear any stale registration before adding the current icon. This is
        // especially useful after Explorer/taskbar recreation or hot reloads.
        DeleteTrayRegistration(g_drives[i], i);
        AddTrayIcon(g_drives[i], i);
    }
}

void RetryMissingTrayIcons() {
    // A failed tray registration should not result in Shell_NotifyIcon being
    // called on every 50 ms activity tick. One retry per second is responsive
    // enough for shell recovery while keeping a persistent failure cheap.
    ULONGLONG now = GetTickCount64();
    if (now - g_lastTrayRetryTick < 1000) {
        return;
    }
    g_lastTrayRetryTick = now;

    for (size_t i = 0; i < g_drives.size(); ++i) {
        if (!g_drives[i].trayAdded) {
            AddTrayIcon(g_drives[i], i);
        }
    }
}

void SetDriveLit(DriveState& drive, bool lit) {
    if (drive.lit == lit) {
        return;
    }
    drive.lit = lit;
    drive.nid.hIcon = lit ? drive.activeIcon : drive.idleIcon;
    if (drive.trayAdded) {
        if (!Shell_NotifyIconW(NIM_MODIFY, &drive.nid)) {
            drive.trayAdded = false;
        }
    }
}

SIZE FloatingClientSize() {
    int cellWidth = std::max(26, g_floatFontPt + 8);
    SIZE size{};
    size.cx = 12 + cellWidth * static_cast<int>(g_floatDrives.size());
    size.cy = std::max(34, g_floatFontPt + 16);
    return size;
}

void EnsureFloatPositionOnScreen(int& x, int& y, int width, int height) {
    RECT desired{x, y, x + width, y + height};
    HMONITOR monitor = MonitorFromRect(&desired, MONITOR_DEFAULTTONULL);
    if (monitor) return;

    MONITORINFO mi{sizeof(mi)};
    HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (GetMonitorInfoW(primary, &mi)) {
        x = mi.rcWork.right - width - 20;
        y = mi.rcWork.bottom - height - 70;
    } else {
        x = 40;
        y = 40;
    }
}

void ResizeFloatingWindow() {
    if (!g_floatWnd || g_floatDrives.empty()) return;

    SIZE s = FloatingClientSize();
    RECT wr{};
    GetWindowRect(g_floatWnd, &wr);
    int x = wr.left;
    int y = wr.top;
    EnsureFloatPositionOnScreen(x, y, s.cx, s.cy);

    SetWindowPos(g_floatWnd, HWND_TOPMOST, x, y, s.cx, s.cy,
                 SWP_NOACTIVATE);
    InvalidateRect(g_floatWnd, nullptr, TRUE);
}

void SetFloatingVisible(bool visible, bool persist) {
    if (visible && g_floatDrives.empty()) visible = false;
    g_floatVisible = visible;
    if (persist) Wh_SetIntValue(L"floating_visible", visible ? 1 : 0);

    if (!g_floatWnd) return;

    if (visible) {
        ResizeFloatingWindow();
        ShowWindow(g_floatWnd, SW_SHOWNOACTIVATE);
        SetWindowPos(g_floatWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else {
        ShowWindow(g_floatWnd, SW_HIDE);
    }
}

int DriveIndexFromTrayId(UINT_PTR trayId) {
    if (trayId < TRAY_ID_BASE) return -1;
    size_t index = static_cast<size_t>(trayId - TRAY_ID_BASE);
    return index < g_drives.size() ? static_cast<int>(index) : -1;
}

bool FloatingContains(wchar_t letter) {
    return std::find(g_floatDrives.begin(), g_floatDrives.end(), letter) !=
           g_floatDrives.end();
}

bool IsDriveRemoved(wchar_t letter) {
    return std::find(g_removedDrives.begin(), g_removedDrives.end(), letter) !=
           g_removedDrives.end();
}

void RememberRemovedDrive(wchar_t letter) {
    if (!IsDriveRemoved(letter)) {
        g_removedDrives.push_back(letter);
    }
}

void PositionFloatingAboveTrayDrive(size_t driveIndex) {
    if (!g_floatWnd || driveIndex >= g_drives.size() || g_floatDrives.empty()) return;

    // A tray click already places the pointer over the icon, so use that point
    // as the anchor. This avoids relying on shell-specific tray geometry APIs.
    POINT anchor{};
    if (!GetCursorPos(&anchor)) return;

    SIZE size = FloatingClientSize();
    int x = anchor.x - size.cx / 2;
    int y = anchor.y - size.cy - 14;

    HMONITOR monitor = MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfoW(monitor, &mi)) {
        if (y < mi.rcWork.top) y = anchor.y + 14;
        if (x < mi.rcWork.left) x = mi.rcWork.left;
        if (x + size.cx > mi.rcWork.right) x = mi.rcWork.right - size.cx;
        if (y + size.cy > mi.rcWork.bottom) y = mi.rcWork.bottom - size.cy;
    }

    SetWindowPos(g_floatWnd, HWND_TOPMOST, x, y, size.cx, size.cy,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    Wh_SetIntValue(L"floating_x", x);
    Wh_SetIntValue(L"floating_y", y);
}

void AddTrayDriveToFloating(size_t driveIndex) {
    if (driveIndex >= g_drives.size()) return;
    wchar_t letter = g_drives[driveIndex].letter;
    bool wasHidden = !g_floatVisible || !IsWindowVisible(g_floatWnd);

    // A new floating session starts with only the tray letter that was clicked.
    if (wasHidden) g_floatDrives.clear();

    if (!FloatingContains(letter)) g_floatDrives.push_back(letter);

    SetFloatingVisible(true, true);
    if (wasHidden) {
        PositionFloatingAboveTrayDrive(driveIndex);
    } else {
        ResizeFloatingWindow();
    }
    InvalidateRect(g_floatWnd, nullptr, TRUE);
}

void RemoveFloatingDriveAt(size_t index) {
    if (index >= g_floatDrives.size()) return;
    g_floatDrives.erase(g_floatDrives.begin() + index);
    if (g_floatDrives.empty()) {
        SetFloatingVisible(false, true);
    } else {
        ResizeFloatingWindow();
        InvalidateRect(g_floatWnd, nullptr, TRUE);
    }
}

void ReadSettings();

void ResetLedUi() {
    g_removedDrives.clear();
    g_floatDrives.clear();
    SetFloatingVisible(false, true);
    Wh_SetIntValue(L"floating_x", INT_MIN);
    Wh_SetIntValue(L"floating_y", INT_MIN);
    ReadSettings();
}

void ReadSettings() {
    PCWSTR driveSetting = Wh_GetStringSetting(L"drive_list");
    std::wstring drivesText = driveSetting ? driveSetting : L"ALL";
    if (driveSetting) Wh_FreeStringSetting(driveSetting);

    g_pollMs = ClampInt(Wh_GetIntSetting(L"poll_interval_ms"), 25, 2000);
    g_holdMs = ClampInt(Wh_GetIntSetting(L"activity_hold_ms"), 50, 5000);
    g_flickerMs = ClampInt(Wh_GetIntSetting(L"flicker_interval_ms"), 25, 1000);
    g_floatFontPt = ClampInt(Wh_GetIntSetting(L"floating_font_size"), 10, 72);
    g_floatOpacity = static_cast<BYTE>(
        ClampInt(Wh_GetIntSetting(L"floating_opacity"), 40, 255));

    g_autoAllDrives = IsAllDrivesSetting(drivesText);
    std::vector<wchar_t> letters = g_autoAllDrives
        ? EnumerateLocalDriveLetters()
        : ParseDriveLetters(drivesText);

    // Drives removed from the tray stay removed for this mod session until
    // Reset is selected. This also prevents the ALL-drive rescan from
    // immediately bringing a removed drive back.
    letters.erase(
        std::remove_if(letters.begin(), letters.end(),
            [](wchar_t letter) { return IsDriveRemoved(letter); }),
        letters.end());

    g_lastEligibleDriveMask = EligibleLocalDriveMask();
    g_lastDriveScanTick = GetTickCount64();

    DeleteTrayIcons();
    CloseCounters();
    g_drives.clear();

    for (wchar_t letter : letters) {
        DriveState drive{};
        drive.letter = letter;
        drive.idleIcon = CreateLetterIcon(letter, RGB(220, 220, 220));
        drive.activeIcon = CreateLetterIcon(letter, RGB(255, 32, 32));
        g_drives.push_back(drive);
    }

    // Drop floating selections for drives that no longer exist.
    g_floatDrives.erase(
        std::remove_if(g_floatDrives.begin(), g_floatDrives.end(),
            [](wchar_t letter) {
                return std::none_of(g_drives.begin(), g_drives.end(),
                    [letter](const DriveState& d) { return d.letter == letter; });
            }),
        g_floatDrives.end());
    if (g_floatDrives.empty()) g_floatVisible = false;

    if (PdhOpenQueryW(nullptr, 0, &g_pdhQuery) == ERROR_SUCCESS) {
        for (auto& drive : g_drives) {
            wchar_t path[128]{};
            swprintf(path, ARRAYSIZE(path),
                     L"\\LogicalDisk(%c:)\\Disk Transfers/sec", drive.letter);
            if (PdhAddEnglishCounterW(g_pdhQuery, path, 0, &drive.counter) ==
                ERROR_SUCCESS) {
                drive.counterValid = true;
            }
        }
        PdhCollectQueryData(g_pdhQuery);
    }

    ReAddTrayIcons();

    if (g_floatWnd) {
        if (g_floatVisible && !g_floatDrives.empty()) {
            ResizeFloatingWindow();
            ShowWindow(g_floatWnd, SW_SHOWNOACTIVATE);
        } else {
            ShowWindow(g_floatWnd, SW_HIDE);
        }
    }

    if (g_hostWnd) {
        KillTimer(g_hostWnd, TIMER_ACTIVITY);
        SetTimer(g_hostWnd, TIMER_ACTIVITY, g_pollMs, nullptr);
    }
}

void SampleActivity() {
    if (!g_pdhQuery) return;
    if (PdhCollectQueryData(g_pdhQuery) != ERROR_SUCCESS) return;

    ULONGLONG now = GetTickCount64();
    bool anyChanged = false;

    for (auto& drive : g_drives) {
        bool activityNow = false;

        if (drive.counterValid) {
            PDH_FMT_COUNTERVALUE value{};
            DWORD counterType = 0;
            if (PdhGetFormattedCounterValue(drive.counter, PDH_FMT_DOUBLE,
                                            &counterType, &value) == ERROR_SUCCESS &&
                (value.CStatus == PDH_CSTATUS_VALID_DATA ||
                 value.CStatus == PDH_CSTATUS_NEW_DATA)) {
                // Any non-trivial transfer rate counts as real drive activity.
                activityNow = value.doubleValue > 0.05;
            }
        }

        if (activityNow) {
            drive.lastActivityTick = now;
        }

        bool envelopeNow = drive.lastActivityTick != 0 &&
                           (now - drive.lastActivityTick) <=
                               static_cast<ULONGLONG>(g_holdMs);

        if (!envelopeNow) {
            // Truly idle: settle to gray immediately.
            drive.activityEnvelope = false;
            drive.flickerPhase = false;
            drive.lastFlickerTick = 0;
            if (drive.lit) {
                SetDriveLit(drive, false);
                anyChanged = true;
            }
            continue;
        }

        // Activity is present (or was present very recently). Instead of
        // holding the icon solid red, pulse it quickly to mimic the familiar
        // visual language of a hardware disk LED. The flicker frequency is a
        // UX signal for "busy"; it is intentionally not a one-flash-per-I/O
        // representation of individual NVMe commands.
        if (!drive.activityEnvelope) {
            drive.activityEnvelope = true;
            drive.flickerPhase = true;
            drive.lastFlickerTick = now;
            if (!drive.lit) {
                SetDriveLit(drive, true);
                anyChanged = true;
            }
        } else if ((now - drive.lastFlickerTick) >=
                   static_cast<ULONGLONG>(g_flickerMs)) {
            drive.flickerPhase = !drive.flickerPhase;
            drive.lastFlickerTick = now;
            if (drive.lit != drive.flickerPhase) {
                SetDriveLit(drive, drive.flickerPhase);
                anyChanged = true;
            }
        }
    }

    if (anyChanged && g_floatWnd && IsWindowVisible(g_floatWnd)) {
        InvalidateRect(g_floatWnd, nullptr, FALSE);
    }
}

void CreateFloatingTooltip() {
    if (!g_floatWnd || g_floatTooltip) return;

    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);

    g_floatTooltip = CreateWindowExW(
        WS_EX_TOPMOST,
        TOOLTIPS_CLASSW,
        nullptr,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        g_floatWnd,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr);

    if (!g_floatTooltip) {
        Wh_Log(L"Floating tooltip creation failed: %lu", GetLastError());
        return;
    }

    SendMessageW(g_floatTooltip, TTM_SETMAXTIPWIDTH, 0, 180);
    SendMessageW(g_floatTooltip, TTM_SETDELAYTIME, TTDT_INITIAL, 450);
    SendMessageW(g_floatTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 5000);

    static wchar_t tooltipText[] = L"Drag to move. Click letter to clear.\r\nRight click to close view.";
    TOOLINFOW ti{};
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.hwnd = g_floatWnd;
    ti.uId = reinterpret_cast<UINT_PTR>(g_floatWnd);
    ti.lpszText = tooltipText;

    if (!SendMessageW(g_floatTooltip, TTM_ADDTOOLW, 0,
                      reinterpret_cast<LPARAM>(&ti))) {
        Wh_Log(L"Floating tooltip TTM_ADDTOOL failed");
        DestroyWindow(g_floatTooltip);
        g_floatTooltip = nullptr;
        return;
    }

    Wh_Log(L"Floating tooltip created");
}

int FloatingHitTest(POINT pt) {
    if (g_floatDrives.empty()) return -1;
    RECT client{};
    GetClientRect(g_floatWnd, &client);
    int cellWidth = std::max(26, g_floatFontPt + 8);
    if (pt.y < 2 || pt.y >= client.bottom - 2 || pt.x < 6) return -1;
    int index = (pt.x - 6) / cellWidth;
    if (index < 0 || index >= static_cast<int>(g_floatDrives.size())) return -1;

    // Keep the click target centered around the visible glyph rather than the
    // entire cell, so "click the letter" behaves as expected.
    int cellLeft = 6 + index * cellWidth;
    int center = cellLeft + cellWidth / 2;
    int halfHitWidth = std::max(8, cellWidth / 3);
    if (pt.x < center - halfHitWidth || pt.x > center + halfHitWidth) return -1;
    return index;
}

LRESULT CALLBACK FloatWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            g_floatMouseDown = true;
            g_floatDragging = false;
            POINT clientPt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            g_floatPressedIndex = FloatingHitTest(clientPt);
            g_floatDownScreen = clientPt;
            ClientToScreen(hwnd, &g_floatDownScreen);
            GetWindowRect(hwnd, &g_floatStartRect);
            SetCapture(hwnd);
            return 0;
        }

        case WM_MOUSEMOVE:
            if (g_floatMouseDown && (wParam & MK_LBUTTON)) {
                POINT current{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ClientToScreen(hwnd, &current);
                int dx = current.x - g_floatDownScreen.x;
                int dy = current.y - g_floatDownScreen.y;

                if (!g_floatDragging) {
                    int thresholdX = std::max(2, GetSystemMetrics(SM_CXDRAG));
                    int thresholdY = std::max(2, GetSystemMetrics(SM_CYDRAG));
                    if (std::abs(dx) >= thresholdX || std::abs(dy) >= thresholdY) {
                        g_floatDragging = true;
                    }
                }

                if (g_floatDragging) {
                    int x = g_floatStartRect.left + dx;
                    int y = g_floatStartRect.top + dy;
                    SetWindowPos(hwnd, HWND_TOPMOST, x, y, 0, 0,
                                 SWP_NOSIZE | SWP_NOACTIVATE);
                }
            }
            return 0;

        case WM_LBUTTONUP: {
            POINT releasePt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int releaseIndex = FloatingHitTest(releasePt);
            bool remove = g_floatMouseDown && !g_floatDragging &&
                          g_floatPressedIndex >= 0 &&
                          releaseIndex == g_floatPressedIndex;
            if (GetCapture() == hwnd) ReleaseCapture();
            g_floatMouseDown = false;
            g_floatDragging = false;
            g_floatPressedIndex = -1;
            if (remove) RemoveFloatingDriveAt(static_cast<size_t>(releaseIndex));
            return 0;
        }

        case WM_CAPTURECHANGED:
            g_floatMouseDown = false;
            g_floatDragging = false;
            g_floatPressedIndex = -1;
            return 0;

        case WM_RBUTTONUP:
            SetFloatingVisible(false, true);
            return 0;

        case WM_MOVE:
            if (!g_shuttingDown && IsWindowVisible(hwnd)) {
                RECT rc{};
                if (GetWindowRect(hwnd, &rc)) {
                    Wh_SetIntValue(L"floating_x", rc.left);
                    Wh_SetIntValue(L"floating_y", rc.top);
                }
            }
            return 0;

        case WM_DISPLAYCHANGE: {
            RECT rc{};
            GetWindowRect(hwnd, &rc);
            int x = rc.left;
            int y = rc.top;
            EnsureFloatPositionOnScreen(x, y, rc.right - rc.left,
                                        rc.bottom - rc.top);
            SetWindowPos(hwnd, HWND_TOPMOST, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOACTIVATE);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(hwnd, &ps);

            RECT client{};
            GetClientRect(hwnd, &client);
            HBRUSH background = CreateSolidBrush(RGB(28, 28, 28));
            FillRect(dc, &client, background);
            DeleteObject(background);

            HBRUSH border = CreateSolidBrush(RGB(68, 68, 68));
            FrameRect(dc, &client, border);
            DeleteObject(border);

            int dpi = 96;
            if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
                using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
                auto getDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
                    GetProcAddress(user32, "GetDpiForWindow"));
                if (getDpiForWindow) dpi = static_cast<int>(getDpiForWindow(hwnd));
            }

            int fontHeight = -MulDiv(g_floatFontPt, dpi, 72);
            HFONT font = CreateFontW(
                fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_NATURAL_QUALITY, DEFAULT_PITCH | FF_SWISS,
                PreferredDriveFont());
            HGDIOBJ oldFont = SelectObject(dc, font);
            SetBkMode(dc, TRANSPARENT);

            int cellWidth = std::max(26, g_floatFontPt + 8);
            for (size_t i = 0; i < g_floatDrives.size(); ++i) {
                wchar_t letter = g_floatDrives[i];
                auto it = std::find_if(g_drives.begin(), g_drives.end(),
                    [letter](const DriveState& d) { return d.letter == letter; });
                bool lit = it != g_drives.end() && it->lit;
                RECT cell{
                    6 + static_cast<int>(i) * cellWidth,
                    2,
                    6 + static_cast<int>(i + 1) * cellWidth,
                    client.bottom - 2};
                SetTextColor(dc, lit ? RGB(255, 32, 32) : RGB(145, 145, 145));
                wchar_t glyph[2] = {letter, L'\0'};
                DrawTextW(dc, glyph, 1, &cell,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }

            SelectObject(dc, oldFont);
            DeleteObject(font);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ShowTrayContextMenu(HWND hwnd, int driveIndex) {
    POINT pt{};
    if (!GetCursorPos(&pt)) {
        return;
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    AppendMenuW(menu, MF_STRING, IDM_TRAY_RESET, L"Reset NVME/HDD LED");

    wchar_t removeLabel[32] = L"Remove";
    if (driveIndex >= 0 && driveIndex < static_cast<int>(g_drives.size())) {
        swprintf_s(removeLabel, ARRAYSIZE(removeLabel), L"Remove %c:", g_drives[driveIndex].letter);
    }
    AppendMenuW(menu, MF_STRING, IDM_TRAY_REMOVE, removeLabel);

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_TRAY_QUIT, L"Quit NVME/HDD LED");

    // Required for notification-area context menus to dismiss correctly when
    // the user clicks elsewhere.
    SetForegroundWindow(hwnd);
    UINT command = TrackPopupMenu(
        menu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
        pt.x, pt.y, 0, hwnd, nullptr);
    DestroyMenu(menu);
    PostMessageW(hwnd, WM_NULL, 0, 0);

    if (command == IDM_TRAY_RESET) {
        Wh_Log(L"NVME/HDD LED: Reset selected from tray menu");
        ResetLedUi();
    } else if (command == IDM_TRAY_REMOVE) {
        if (driveIndex >= 0 && driveIndex < static_cast<int>(g_drives.size())) {
            wchar_t letter = g_drives[driveIndex].letter;
            Wh_Log(L"NVME/HDD LED: Remove %c: selected from tray menu", letter);
            RememberRemovedDrive(letter);
            ReadSettings();
        }
    } else if (command == IDM_TRAY_QUIT) {
        Wh_Log(L"NVME/HDD LED: Quit selected from tray menu");
        // Latch the quit state so an Explorer/taskbar restart doesn't silently
        // bring the LEDs back. Toggling the mod off in Windhawk clears this
        // latch; toggling it back on starts the UI again.
        g_quitRequested = true;
        Wh_SetIntValue(L"quit_latched", 1);
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
}

LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        ReAddTrayIcons();
        return 0;
    }

    switch (msg) {
        case WM_TRAYICON: {
            // Keep the default Shell_NotifyIcon callback semantics rather than
            // opting into NOTIFYICON_VERSION_4. In default mode lParam is the
            // mouse/notification message directly, which is simpler and has
            // proven more reliable for both clicks and standard hover tips.
            const UINT eventCode = static_cast<UINT>(lParam);
            if (eventCode == WM_LBUTTONUP || eventCode == NIN_SELECT ||
                eventCode == NIN_KEYSELECT) {
                Wh_Log(L"NVME/HDD LED tray activation: event=0x%X id=%llu",
                       eventCode, static_cast<unsigned long long>(wParam));
                int driveIndex = DriveIndexFromTrayId(static_cast<UINT_PTR>(wParam));
                if (driveIndex >= 0) AddTrayDriveToFloating(static_cast<size_t>(driveIndex));
            } else if (eventCode == WM_RBUTTONUP || eventCode == WM_CONTEXTMENU) {
                Wh_Log(L"NVME/HDD LED tray context menu: event=0x%X id=%llu",
                       eventCode, static_cast<unsigned long long>(wParam));
                int driveIndex = DriveIndexFromTrayId(static_cast<UINT_PTR>(wParam));
                ShowTrayContextMenu(hwnd, driveIndex);
            }
            return 0;
        }

        case WM_TIMER:
            if (wParam == TIMER_ACTIVITY) {
                if (g_autoAllDrives) {
                    ULONGLONG now = GetTickCount64();
                    if (now - g_lastDriveScanTick >= 2000) {
                        g_lastDriveScanTick = now;
                        DWORD mask = EligibleLocalDriveMask();
                        if (mask != g_lastEligibleDriveMask) {
                            ReadSettings();
                            return 0;
                        }
                    }
                }
                RetryMissingTrayIcons();
                SampleActivity();
            }
            return 0;

        case WM_APPLY_SETTINGS:
            ReadSettings();
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_shuttingDown = true;
            KillTimer(hwnd, TIMER_ACTIVITY);
            DeleteTrayIcons();
            CloseCounters();
            if (g_floatTooltip) {
                DestroyWindow(g_floatTooltip);
                g_floatTooltip = nullptr;
            }
            if (g_floatWnd) {
                DestroyWindow(g_floatWnd);
                g_floatWnd = nullptr;
            }
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThreadProc(void*) {
    // Serialize host lifetimes across a Windhawk hot-reload. The mutex is
    // acquired and released by this same worker thread, which avoids leaving
    // two generations of the UI alive during a source update.
    g_singletonMutex = CreateMutexW(
        nullptr, FALSE, L"Local\\WindhawkHddLed_ShellSingleton_v1");
    if (!g_singletonMutex) {
        Wh_Log(L"CreateMutex failed: %lu", GetLastError());
        return 0;
    }

    DWORD waitResult = WaitForSingleObject(g_singletonMutex, 5000);
    if (waitResult != WAIT_OBJECT_0 && waitResult != WAIT_ABANDONED) {
        Wh_Log(L"Could not acquire NVME/HDD LED singleton: %lu", waitResult);
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
        return 0;
    }

    HINSTANCE instance = GetModuleHandleW(nullptr);

    auto cleanupUiAndClasses = [&]() {
        if (g_floatWnd) {
            DestroyWindow(g_floatWnd);
            g_floatWnd = nullptr;
        }
        if (g_hostWnd) {
            DestroyWindow(g_hostWnd);
            g_hostWnd = nullptr;
        }
        if (g_floatClassRegistered) {
            UnregisterClassW(kFloatClass, instance);
            g_floatClassRegistered = false;
        }
        if (g_hostClassRegistered) {
            UnregisterClassW(kHostClass, instance);
            g_hostClassRegistered = false;
        }
    };

    // These classes are registered against explorer.exe rather than the mod
    // DLL. Explicitly remove stale registrations left by an older hot-reloaded
    // build before installing the current WNDPROC addresses.
    UnregisterClassW(kHostClass, instance);
    UnregisterClassW(kFloatClass, instance);

    WNDCLASSW hostClass{};
    hostClass.lpfnWndProc = HostWndProc;
    hostClass.hInstance = instance;
    hostClass.lpszClassName = kHostClass;
    if (!RegisterClassW(&hostClass)) {
        Wh_Log(L"RegisterClass(%s) failed: %lu", kHostClass, GetLastError());
        cleanupUiAndClasses();
        ReleaseMutex(g_singletonMutex);
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
        return 0;
    }
    g_hostClassRegistered = true;

    WNDCLASSW floatClass{};
    floatClass.lpfnWndProc = FloatWndProc;
    floatClass.hInstance = instance;
    floatClass.lpszClassName = kFloatClass;
    floatClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&floatClass)) {
        Wh_Log(L"RegisterClass(%s) failed: %lu", kFloatClass, GetLastError());
        cleanupUiAndClasses();
        ReleaseMutex(g_singletonMutex);
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
        return 0;
    }
    g_floatClassRegistered = true;

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    g_hostWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW, kHostClass, L"", WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    if (!g_hostWnd) {
        Wh_Log(L"Host window creation failed: %lu", GetLastError());
        cleanupUiAndClasses();
        ReleaseMutex(g_singletonMutex);
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
        return 0;
    }
    Wh_Log(L"NVME/HDD LED host started");

    int x = Wh_GetIntValue(L"floating_x", INT_MIN);
    int y = Wh_GetIntValue(L"floating_y", INT_MIN);
    SIZE floatSize{80, 40};
    if (x == INT_MIN || y == INT_MIN) {
        MONITORINFO mi{sizeof(mi)};
        HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
        if (GetMonitorInfoW(primary, &mi)) {
            x = mi.rcWork.right - floatSize.cx - 20;
            y = mi.rcWork.bottom - floatSize.cy - 70;
        } else {
            x = 40;
            y = 40;
        }
    }

    g_floatWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kFloatClass, L"", WS_POPUP,
        x, y, floatSize.cx, floatSize.cy,
        nullptr, nullptr, instance, nullptr);
    if (!g_floatWnd) {
        Wh_Log(L"Floating window creation failed: %lu", GetLastError());
    } else {
        Wh_Log(L"Floating window created: hwnd=%p", g_floatWnd);
        CreateFloatingTooltip();
    }

    // Floating selections are intentionally session-local. Start hidden; the
    // first tray click creates a fresh floating set containing only that drive.
    g_floatVisible = false;
    g_floatDrives.clear();
    Wh_SetIntValue(L"floating_visible", 0);

    ReadSettings();
    if (g_floatWnd) SetFloatingVisible(false, false);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    g_hostWnd = nullptr;

    // The windows are gone at this point. Unregister the Explorer-owned window
    // classes so a future Windhawk hot-reload can't retain stale WNDPROC addresses.
    cleanupUiAndClasses();

    if (g_singletonMutex) {
        ReleaseMutex(g_singletonMutex);
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
    }

    return 0;
}

} // namespace

// --- Windhawk callbacks -----------------------------------------------------
// Keep this deliberately simple: Windhawk loads the mod into explorer.exe and
// we create a worker thread for the tray/floating UI. No taskbar hooks are used.

BOOL Wh_ModInit() {
    Wh_Log(L"NVME/HDD LED v1.0.0 initializing in explorer.exe (pid=%lu)",
           GetCurrentProcessId());

    // Windhawk can inject an @include explorer.exe mod into more than one
    // Explorer process. Only the Explorer instance that owns the desktop and
    // taskbar should create notification/floating UI.
    if (!IsShellExplorerProcess()) {
        Wh_Log(L"Not the shell Explorer process; no UI instance created");
        return TRUE;
    }

    if (Wh_GetIntValue(L"quit_latched", 0) != 0) {
        Wh_Log(L"NVME/HDD LED is quit-latched; toggle the mod off/on in Windhawk to restart it");
        return TRUE;
    }

    g_shuttingDown = false;
    g_quitRequested = false;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_hostWnd) {
        PostMessageW(g_hostWnd, WM_APPLY_SETTINGS, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"NVME/HDD LED v1.0.0 shutting down");

    // Disabling the mod in Windhawk is the intentional way to clear a prior
    // Quit latch. The next enable will create the LEDs again.
    Wh_SetIntValue(L"quit_latched", 0);
    g_quitRequested = false;
    g_shuttingDown = true;

    if (g_hostWnd) {
        PostMessageW(g_hostWnd, WM_CLOSE, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
}
