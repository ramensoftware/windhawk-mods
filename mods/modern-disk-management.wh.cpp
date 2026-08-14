// ==WindhawkMod==
// @id              modern-disk-management
// @name            Modern Disk Management
// @description     Replaces diskmgmt.msc with a modern dark disk manager
// @version         3.4.0
// @author          emirerkul991-1yssssss
// @github          https://github.com/emirerkul991-1yssssss
// @license         MIT
// @include         mmc.exe
// @compilerOptions -ldwmapi -lgdiplus -lgdi32 -lmsimg32 -lole32 -loleaut32 -lshell32 -lshlwapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Disk Management

`diskmgmt.msc` is an MMC console from 2001: a grey table on top, a hatched
partition strip below, and a right-click menu for everything. This is the same
tool, drawn properly - the volume table and the graphical map are both still
there, in a dark window with real typography, shell icons and a partition map
that shows unallocated space where it actually is.

The layout is the console's, deliberately. A volume table listing every volume
on every disk with layout, type, file system, status, capacity and free space;
below it a map per physical disk, each partition a box sized to its share of
the drive, coloured by what it is. Selecting a volume in either view selects it
in the other, and the bar along the bottom acts on whatever is selected.

## What it does and does not do

It **reads** the disk layout and draws it. Everything that could destroy data is
handed to Windows rather than reimplemented:

| Action | Handled by |
|-|-|
| Properties | drawn here, read-only |
| Format | `SHFormatDrive`, the standard Format dialog |
| Open | Explorer, at the drive's own root |
| Eject | the standard lock, dismount and eject sequence |

There is deliberately no partition editing here. Writing a partition editor is
how data gets lost, and Windows already ships tools that do it properly - run
`diskpart`, or Settings' Disks & volumes, for changing letters, extending,
shrinking or converting. Set `WH_DISKMGMT_CLASSIC=1` to get the original
console back for one launch.

Double-click a volume, or press Enter, for its properties; the arrow keys move
the selection, F5 re-reads the disks and Esc closes the window.

## How it works

`diskmgmt.msc` is hosted by `mmc.exe`, so the mod loads there, checks the
command line, and shows its own window instead of letting the console load.
Any other snap-in - `services.msc`, `eventvwr.msc`, a custom console - runs
untouched.

Disk and volume data comes from ordinary Win32: `FindFirstVolume` for the
volumes, `IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS` to place each one on a physical
disk, and `IOCTL_STORAGE_QUERY_PROPERTY` for the disk's model name. The console
requires administrator rights, so the window inherits them and every query
succeeds; nothing here needs rights the original did not.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wallpaperTint: true
  $name: Tint the window with the wallpaper
  $description: Mixes the desktop wallpaper's dominant colour into the background, the way Mica does.
- showEmptyVolumes: true
  $name: Show volumes without a drive letter
  $description: EFI system partitions, recovery partitions and similar.
- verboseLog: false
  $name: Log enumeration details
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <commoncontrols.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shldisp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windowsx.h>
#include <tlhelp32.h>
#include <winioctl.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

struct Settings {
    bool wallpaperTint = true;
    bool showEmptyVolumes = true;
    bool verboseLog = false;
};

Settings g_settings;

// -----------------------------------------------------------------------------
// Look
// -----------------------------------------------------------------------------

namespace ui {

constexpr COLORREF kWindow = RGB(26, 26, 28);
constexpr COLORREF kCard = RGB(39, 39, 42);
constexpr COLORREF kCardBorder = RGB(55, 55, 59);
constexpr COLORREF kTextPrimary = RGB(255, 255, 255);
constexpr COLORREF kTextSecondary = RGB(158, 158, 165);
constexpr COLORREF kTextTertiary = RGB(120, 120, 126);
constexpr COLORREF kTextDisabled = RGB(108, 108, 114);
constexpr COLORREF kBarTrack = RGB(58, 58, 62);
// Blue and grey only. The boot volume takes the system accent, data volumes a
// muted steel blue, and everything the system keeps to itself is grey - so the
// hierarchy is carried by how saturated a colour is rather than by hue.
constexpr COLORREF kBarSystem = RGB(124, 126, 134);   // EFI, reserved, recovery
constexpr COLORREF kBarData = RGB(116, 146, 184);     // data volumes
constexpr COLORREF kUnallocated = RGB(72, 72, 78);
constexpr COLORREF kHealthy = RGB(130, 170, 215);
constexpr COLORREF kCloseHover = RGB(196, 43, 28);

constexpr int kWindowWidth = 1180;
constexpr int kWindowHeight = 800;
constexpr int kTitleHeight = 44;
constexpr int kCloseWidth = 46;
constexpr int kPadding = 22;
constexpr int kCardRadius = 8;
constexpr int kActionBarHeight = 62;
constexpr int kDiskTileWidth = 176;
constexpr int kMapHeight = 104;
constexpr int kButtonHeight = 32;
constexpr int kScrollBarWidth = 4;

// The system accent, used for the boot volume so the disk the system runs from
// is identifiable at a glance.
COLORREF AccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        int r = (color >> 16) & 0xFF;
        int g = (color >> 8) & 0xFF;
        int b = color & 0xFF;
        if (r + g + b > 120) {
            return RGB(r, g, b);
        }
    }
    return RGB(76, 164, 224);
}

int Scale(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

void FillRoundRect(HDC dc, const RECT& rect, int radius, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius * 2,
              radius * 2);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

COLORREF MixColors(COLORREF base, COLORREF tint, int tintPercent) {
    auto mix = [&](int b, int t) {
        return (b * (100 - tintPercent) + t * tintPercent) / 100;
    };
    return RGB(mix(GetRValue(base), GetRValue(tint)),
               mix(GetGValue(base), GetGValue(tint)),
               mix(GetBValue(base), GetBValue(tint)));
}

// The wallpaper's dominant colour, mixed into a base - the same trick the Run
// dialog uses, since DWM will not draw Mica on a window like this.
COLORREF WallpaperTinted(COLORREF base, int percent) {
    WCHAR path[MAX_PATH] = L"";
    if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0) ||
        !path[0]) {
        return base;
    }

    ULONG_PTR token = 0;
    Gdiplus::GdiplusStartupInput input;
    if (Gdiplus::GdiplusStartup(&token, &input, nullptr) != Gdiplus::Ok) {
        return base;
    }

    COLORREF result = base;
    {
        Gdiplus::Bitmap wallpaper(path);
        if (wallpaper.GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::Bitmap average(1, 1, PixelFormat32bppARGB);
            Gdiplus::Graphics graphics(&average);
            graphics.SetInterpolationMode(
                Gdiplus::InterpolationModeHighQualityBilinear);
            if (graphics.DrawImage(&wallpaper, 0, 0, 1, 1) == Gdiplus::Ok) {
                Gdiplus::Color color;
                if (average.GetPixel(0, 0, &color) == Gdiplus::Ok) {
                    result = MixColors(
                        base, RGB(color.GetR(), color.GetG(), color.GetB()),
                        percent);
                }
            }
        }
    }
    Gdiplus::GdiplusShutdown(token);
    return result;
}

// Decimal units, the way drive manufacturers and Settings' Disks & volumes
// report capacity: 1 GB is 1000 MB, not 1024. A 1 TB disk reads as 1 TB.
std::wstring FormatSize(ULONGLONG bytes) {
    constexpr PCWSTR kUnits[] = {L"bytes", L"kB", L"MB", L"GB", L"TB", L"PB"};
    double value = static_cast<double>(bytes);
    int unit = 0;
    while (value >= 1000.0 && unit + 1 < static_cast<int>(ARRAYSIZE(kUnits))) {
        value /= 1000.0;
        unit++;
    }
    WCHAR text[64];
    _snwprintf(text, ARRAYSIZE(text) - 1, unit == 0 ? L"%.0f %s" : L"%.2f %s",
               value, kUnits[unit]);
    text[ARRAYSIZE(text) - 1] = L'\0';
    return text;
}

}  // namespace ui

// -----------------------------------------------------------------------------
// Enumeration
// -----------------------------------------------------------------------------

struct VolumeInfo {
    std::wstring guidPath;    // the volume GUID path, with trailing separator
    std::wstring letter;      // "C:" or empty
    std::wstring label;       // "Windows-SSD"
    std::wstring fileSystem;  // NTFS, FAT32, ...
    std::wstring role;        // "EFI system partition", "Recovery partition", ...
    ULONGLONG size = 0;
    ULONGLONG freeSpace = 0;
    ULONGLONG offset = 0;  // start on the physical disk
    int diskNumber = -1;
    bool isBoot = false;
    bool isSystem = false;  // EFI / reserved / recovery, not user storage
    HBITMAP image = nullptr;
    HICON icon = nullptr;  // fallback when there is no path to render from
};

// GPT partition type GUIDs. Declared here rather than relying on the SDK's
// DEFINE_GUID symbols, which need INITGUID to be linkable.
const GUID kPartitionSystem = {
    0xC12A7328, 0xF81F, 0x11D2, {0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B}};
const GUID kPartitionMsftReserved = {
    0xE3C9E316, 0x0B5C, 0x4DB8, {0x81, 0x7D, 0xF9, 0x2D, 0xF0, 0x02, 0x15, 0xAE}};
const GUID kPartitionBasicData = {
    0xEBD0A0A2, 0xB9E5, 0x4433, {0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7}};
const GUID kPartitionMsftRecovery = {
    0xDE94BBA4, 0x06D1, 0x4D40, {0xA1, 0x6A, 0xBF, 0xD5, 0x01, 0x79, 0xD6, 0xAC}};

struct PartitionMeta {
    ULONGLONG offset = 0;
    ULONGLONG length = 0;
    std::wstring role;
    bool isSystem = false;
};

// What each partition on a disk actually is, straight from the partition
// table - the only way to tell an EFI system partition from a data volume when
// neither has a label or a drive letter.
std::vector<PartitionMeta> QueryPartitions(int diskNumber, std::wstring* style) {
    std::vector<PartitionMeta> partitions;

    WCHAR path[64];
    _snwprintf(path, ARRAYSIZE(path) - 1, L"\\\\.\\PhysicalDrive%d", diskNumber);
    path[ARRAYSIZE(path) - 1] = L'\0';

    HANDLE disk = CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (disk == INVALID_HANDLE_VALUE) {
        return partitions;
    }

    std::vector<BYTE> buffer(sizeof(DRIVE_LAYOUT_INFORMATION_EX) +
                             128 * sizeof(PARTITION_INFORMATION_EX));
    DWORD returned = 0;
    if (DeviceIoControl(disk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, nullptr, 0,
                        buffer.data(), static_cast<DWORD>(buffer.size()),
                        &returned, nullptr)) {
        auto* layout =
            reinterpret_cast<DRIVE_LAYOUT_INFORMATION_EX*>(buffer.data());
        for (DWORD i = 0; i < layout->PartitionCount; i++) {
            const PARTITION_INFORMATION_EX& entry = layout->PartitionEntry[i];
            if (entry.PartitionStyle == PARTITION_STYLE_GPT) {
                if (style) {
                    *style = L"GPT";
                }
                PartitionMeta meta;
                meta.offset =
                    static_cast<ULONGLONG>(entry.StartingOffset.QuadPart);
                meta.length =
                    static_cast<ULONGLONG>(entry.PartitionLength.QuadPart);
                const GUID& type = entry.Gpt.PartitionType;
                if (IsEqualGUID(type, kPartitionSystem)) {
                    meta.role = L"EFI system partition";
                    meta.isSystem = true;
                } else if (IsEqualGUID(type, kPartitionMsftReserved)) {
                    meta.role = L"Microsoft reserved partition";
                    meta.isSystem = true;
                } else if (IsEqualGUID(type, kPartitionMsftRecovery)) {
                    meta.role = L"Recovery partition";
                    meta.isSystem = true;
                } else if (IsEqualGUID(type, kPartitionBasicData)) {
                    meta.role = L"Basic data partition";
                } else {
                    // The GPT name is set for most other types; fall back to it.
                    WCHAR name[37]{};
                    memcpy(name, entry.Gpt.Name, sizeof(entry.Gpt.Name));
                    name[36] = L'\0';
                    meta.role = name[0] ? name : L"Partition";
                    meta.isSystem = true;
                }
                partitions.push_back(std::move(meta));
            } else if (entry.PartitionStyle == PARTITION_STYLE_MBR) {
                if (style) {
                    *style = L"MBR";
                }
                // An MBR layout always comes back with four entries, whether or
                // not they describe anything.
                if (entry.Mbr.PartitionType == PARTITION_ENTRY_UNUSED ||
                    entry.PartitionLength.QuadPart == 0) {
                    continue;
                }
                PartitionMeta meta;
                meta.offset =
                    static_cast<ULONGLONG>(entry.StartingOffset.QuadPart);
                meta.length =
                    static_cast<ULONGLONG>(entry.PartitionLength.QuadPart);
                meta.role = entry.Mbr.BootIndicator ? L"System partition"
                                                    : L"Primary partition";
                meta.isSystem = entry.Mbr.BootIndicator != FALSE;
                partitions.push_back(std::move(meta));
            }
        }
    }
    CloseHandle(disk);
    return partitions;
}

// What to call a volume in the list. A volume with no label and no letter is
// not a "local disk" - it is almost always the EFI system partition or a
// recovery partition, and saying so is the whole point of showing it.
std::wstring VolumeDisplayName(const VolumeInfo& volume) {
    if (!volume.label.empty()) {
        return volume.letter.empty() ? volume.label
                                     : volume.label + L" (" + volume.letter + L")";
    }
    if (!volume.letter.empty()) {
        return L"Local Disk (" + volume.letter + L")";
    }
    if (!volume.role.empty()) {
        return volume.role;
    }
    return L"Unnamed volume";
}

// One span of the physical disk, in order: either a partition - pointing at the
// volume living on it - or the empty space between two of them. The graphical
// map is drawn from these, which is why unallocated space has to be a thing the
// model knows about rather than something the painter infers.
struct DiskSegment {
    ULONGLONG offset = 0;
    ULONGLONG length = 0;
    int volumeIndex = -1;    // -1 when nothing is mounted on it
    bool unallocated = false;  // free space, as opposed to an unmounted partition
    std::wstring role;
    bool isSystem = false;
};

struct DiskInfo {
    int number = -1;
    std::wstring model;
    std::wstring style;  // "GPT" or "MBR"
    ULONGLONG size = 0;
    std::vector<VolumeInfo> volumes;
    std::vector<DiskSegment> segments;
    HBITMAP image = nullptr;
    HICON icon = nullptr;
    bool removable = false;
};

// Icons come from the shell so a drive with a custom icon shows it, and the
// larger system image list is used so nothing is an upscaled 16px bitmap.
// Icons are loaded as 32-bit bitmaps through IShellItemImageFactory - the same
// API Explorer uses - rather than as HICONs from the system image lists.
//
// Two reasons. The image lists only offer 16, 32, 48 and 256 pixel sources, so
// anything else is a resampled smear; and their icons come back without
// premultiplied alpha, which is why the Windows badge on the system drive
// rendered as a flat blue rectangle.
//
// The factory renders at exactly the size asked for, so no resampling happens
// on the way to the screen either.
//
// What it does not do is premultiply. The bitmaps come back with colour
// channels brighter than their own alpha, which is impossible in premultiplied
// data - measured, not assumed. AlphaBlend with AC_SRC_ALPHA requires
// premultiplied pixels, and handing it straight ones makes every partly
// transparent pixel blow out to full strength. That is what turned the Windows
// badge on the system drive into a blue rectangle and the drive's indicator
// into a green square, and why it got worse as the icons got smaller: a 32
// pixel drive icon has about ten partly transparent pixels, a 24 pixel one has
// sixty.
void Premultiply(HBITMAP bitmap) {
    // Any drawing still queued for this bitmap has to land before its bits are
    // read behind GDI's back.
    GdiFlush();

    DIBSECTION section{};
    if (GetObject(bitmap, sizeof(section), &section) == sizeof(section) &&
        section.dsBm.bmBitsPixel == 32 && section.dsBm.bmBits) {
        auto* pixels = static_cast<BYTE*>(section.dsBm.bmBits);
        size_t count = static_cast<size_t>(section.dsBm.bmWidth) *
                       std::abs(section.dsBmih.biHeight);
        for (size_t i = 0; i < count; i++) {
            BYTE* pixel = pixels + i * 4;
            BYTE alpha = pixel[3];
            if (alpha == 255) {
                continue;
            }
            pixel[0] = static_cast<BYTE>(pixel[0] * alpha / 255);
            pixel[1] = static_cast<BYTE>(pixel[1] * alpha / 255);
            pixel[2] = static_cast<BYTE>(pixel[2] * alpha / 255);
        }
        return;
    }

    // Not a DIB section: go through GDI for the pixels instead.
    BITMAP info{};
    if (!GetObject(bitmap, sizeof(info), &info) || info.bmWidth <= 0 ||
        info.bmHeight <= 0) {
        return;
    }

    BITMAPINFO header{};
    header.bmiHeader.biSize = sizeof(header.bmiHeader);
    header.bmiHeader.biWidth = info.bmWidth;
    header.bmiHeader.biHeight = -info.bmHeight;  // top down
    header.bmiHeader.biPlanes = 1;
    header.bmiHeader.biBitCount = 32;
    header.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> pixels(static_cast<size_t>(info.bmWidth) * info.bmHeight *
                             4);
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return;
    }
    if (GetDIBits(screen, bitmap, 0, info.bmHeight, pixels.data(), &header,
                  DIB_RGB_COLORS)) {
        for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
            BYTE alpha = pixels[i + 3];
            if (alpha == 255) {
                continue;
            }
            pixels[i] = static_cast<BYTE>(pixels[i] * alpha / 255);
            pixels[i + 1] = static_cast<BYTE>(pixels[i + 1] * alpha / 255);
            pixels[i + 2] = static_cast<BYTE>(pixels[i + 2] * alpha / 255);
        }
        SetDIBits(screen, bitmap, 0, info.bmHeight, pixels.data(), &header,
                  DIB_RGB_COLORS);
    }
    ReleaseDC(nullptr, screen);
}

HBITMAP ShellImage(const std::wstring& path, int size) {
    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_PPV_ARGS(&item))) ||
        !item) {
        return nullptr;
    }

    IShellItemImageFactory* factory = nullptr;
    HBITMAP bitmap = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
        SIZE requested{size, size};
        factory->GetImage(requested, SIIGBF_ICONONLY, &bitmap);
        factory->Release();
    }
    item->Release();

    if (bitmap) {
        Premultiply(bitmap);
    }
    return bitmap;
}

// Draws a premultiplied 32-bit bitmap with its alpha intact.
void DrawShellImage(HDC dc, HBITMAP bitmap, int x, int y, int size) {
    BITMAP info{};
    if (!GetObject(bitmap, sizeof(info), &info)) {
        return;
    }
    HDC memory = CreateCompatibleDC(dc);
    HGDIOBJ old = SelectObject(memory, bitmap);
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(dc, x, y, size, size, memory, 0, 0, info.bmWidth, info.bmHeight,
               blend);
    SelectObject(memory, old);
    DeleteDC(memory);
}

// The system image list whose native size is at least the size the icon will
// be drawn at, so icons are downscaled rather than blown up.
int ImageListForSize(int drawnSize) {
    // Exact matches where possible: 16, 32 and 48 are the native sizes, and
    // drawing one of those 1:1 is the difference between a crisp icon and a
    // resampled smear.
    if (drawnSize <= 16) {
        return SHIL_SMALL;
    }
    if (drawnSize <= 32) {
        return SHIL_LARGE;  // 32px
    }
    if (drawnSize <= 48) {
        return SHIL_EXTRALARGE;  // 48px
    }
    return SHIL_JUMBO;  // 256px
}

HICON IconFromSystemIndex(int index, int drawnSize) {
    IImageList* images = nullptr;
    if (FAILED(SHGetImageList(ImageListForSize(drawnSize), IID_PPV_ARGS(&images))) ||
        !images) {
        return nullptr;
    }
    HICON icon = nullptr;
    images->GetIcon(index, ILD_TRANSPARENT, &icon);
    images->Release();
    return icon;
}

HICON ShellIcon(const std::wstring& path, int drawnSize) {
    SHFILEINFOW info{};
    if (!SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &info, sizeof(info),
                        SHGFI_SYSICONINDEX)) {
        return nullptr;
    }
    return IconFromSystemIndex(info.iIcon, drawnSize);
}

HICON StockDriveIcon(SHSTOCKICONID id, int drawnSize) {
    SHSTOCKICONINFO info{};
    info.cbSize = sizeof(info);
    // The index into the system image list, not the 32px icon handle: that is
    // what allows a 48px or 256px source to be used.
    if (SUCCEEDED(SHGetStockIconInfo(id, SHGSI_SYSICONINDEX, &info))) {
        if (HICON icon = IconFromSystemIndex(info.iSysImageIndex, drawnSize)) {
            return icon;
        }
    }
    if (SUCCEEDED(SHGetStockIconInfo(id, SHGSI_ICON | SHGSI_LARGEICON, &info))) {
        return info.hIcon;
    }
    return nullptr;
}

// Icons are requested at the exact size they are drawn at, so the shell's own
// scaler produces them and nothing is resampled on the way to the screen. That
// size depends on the window's DPI, which enumeration has no other way to know.
UINT g_iconDpi = 96;

int VolumeIconPixels() {
    return MulDiv(32, static_cast<int>(g_iconDpi), 96);
}

int DiskIconPixels() {
    return MulDiv(32, static_cast<int>(g_iconDpi), 96);
}

// Removable and USB disks get the removable-drive icon, so a memory stick does
// not look like an internal SSD.
bool IsRemovableDisk(int diskNumber) {
    WCHAR path[64];
    _snwprintf(path, ARRAYSIZE(path) - 1, L"\\\\.\\PhysicalDrive%d", diskNumber);
    path[ARRAYSIZE(path) - 1] = L'\0';

    HANDLE disk = CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (disk == INVALID_HANDLE_VALUE) {
        return false;
    }

    STORAGE_PROPERTY_QUERY query{};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;
    BYTE buffer[1024]{};
    DWORD returned = 0;
    bool removable = false;
    if (DeviceIoControl(disk, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query),
                        buffer, sizeof(buffer), &returned, nullptr)) {
        auto* descriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer);
        removable = descriptor->RemovableMedia ||
                    descriptor->BusType == BusTypeUsb;
    }
    CloseHandle(disk);
    return removable;
}

std::wstring TrimSpaces(const std::wstring& value) {
    size_t first = value.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        return L"";
    }
    size_t last = value.find_last_not_of(L" \t");
    return value.substr(first, last - first + 1);
}

// Model name straight from the storage stack, e.g. "Samsung SSD 980 PRO".
std::wstring QueryDiskModel(int diskNumber) {
    WCHAR path[64];
    _snwprintf(path, ARRAYSIZE(path) - 1, L"\\\\.\\PhysicalDrive%d", diskNumber);
    path[ARRAYSIZE(path) - 1] = L'\0';

    HANDLE disk = CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (disk == INVALID_HANDLE_VALUE) {
        return L"";
    }

    STORAGE_PROPERTY_QUERY query{};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[1024]{};
    DWORD returned = 0;
    std::wstring model;
    if (DeviceIoControl(disk, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query),
                        buffer, sizeof(buffer), &returned, nullptr)) {
        auto* descriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer);
        auto ansiAt = [&](DWORD offset) -> std::wstring {
            if (!offset || offset >= returned) {
                return L"";
            }
            const char* text = reinterpret_cast<const char*>(buffer) + offset;
            int needed = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);
            if (needed <= 1) {
                return L"";
            }
            std::wstring wide(static_cast<size_t>(needed - 1), L'\0');
            MultiByteToWideChar(CP_ACP, 0, text, -1, wide.data(), needed);
            return TrimSpaces(wide);
        };

        std::wstring vendor = ansiAt(descriptor->VendorIdOffset);
        std::wstring product = ansiAt(descriptor->ProductIdOffset);
        model = vendor.empty() ? product : vendor + L" " + product;
        model = TrimSpaces(model);
    }
    CloseHandle(disk);
    return model;
}

ULONGLONG QueryDiskSize(int diskNumber) {
    WCHAR path[64];
    _snwprintf(path, ARRAYSIZE(path) - 1, L"\\\\.\\PhysicalDrive%d", diskNumber);
    path[ARRAYSIZE(path) - 1] = L'\0';

    HANDLE disk = CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (disk == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DISK_GEOMETRY_EX geometry{};
    DWORD returned = 0;
    ULONGLONG size = 0;
    if (DeviceIoControl(disk, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, nullptr, 0,
                        &geometry, sizeof(geometry), &returned, nullptr)) {
        size = static_cast<ULONGLONG>(geometry.DiskSize.QuadPart);
    }
    CloseHandle(disk);
    return size;
}

// Which physical disk a volume lives on, and where it starts.
bool QueryVolumePlacement(const std::wstring& guidPath, int* diskNumber,
                          ULONGLONG* offset) {
    // The device path must not carry its trailing backslash.
    std::wstring devicePath = guidPath;
    if (!devicePath.empty() && devicePath.back() == L'\\') {
        devicePath.pop_back();
    }

    HANDLE volume = CreateFileW(devicePath.c_str(), 0,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                OPEN_EXISTING, 0, nullptr);
    if (volume == INVALID_HANDLE_VALUE) {
        return false;
    }

    BYTE buffer[1024]{};
    DWORD returned = 0;
    bool ok = false;
    if (DeviceIoControl(volume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, nullptr, 0,
                        buffer, sizeof(buffer), &returned, nullptr)) {
        auto* extents = reinterpret_cast<VOLUME_DISK_EXTENTS*>(buffer);
        if (extents->NumberOfDiskExtents > 0) {
            *diskNumber = static_cast<int>(extents->Extents[0].DiskNumber);
            *offset =
                static_cast<ULONGLONG>(extents->Extents[0].StartingOffset.QuadPart);
            ok = true;
        }
    }
    CloseHandle(volume);
    return ok;
}

void ReleaseDiskIcons(std::vector<DiskInfo>& disks) {
    for (auto& disk : disks) {
        if (disk.image) {
            DeleteObject(disk.image);
            disk.image = nullptr;
        }
        if (disk.icon) {
            DestroyIcon(disk.icon);
            disk.icon = nullptr;
        }
        for (auto& volume : disk.volumes) {
            if (volume.image) {
                DeleteObject(volume.image);
                volume.image = nullptr;
            }
            if (volume.icon) {
                DestroyIcon(volume.icon);
                volume.icon = nullptr;
            }
        }
    }
}

// Probes \\.\PhysicalDriveN so disks with nothing mounted still show up. A
// brand-new or uninitialised disk reports no volumes at all, and building the
// list from volumes alone made exactly the disk you opened this window to look
// at invisible.
std::vector<int> EnumeratePhysicalDisks() {
    std::vector<int> numbers;
    // 64 is well past any consumer machine, and each miss is a cheap failed
    // CreateFile rather than an enumeration.
    for (int number = 0; number < 64; number++) {
        WCHAR path[64];
        _snwprintf(path, ARRAYSIZE(path) - 1, L"\\\\.\\PhysicalDrive%d", number);
        path[ARRAYSIZE(path) - 1] = L'\0';

        HANDLE device =
            CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                        OPEN_EXISTING, 0, nullptr);
        if (device == INVALID_HANDLE_VALUE) {
            continue;
        }
        CloseHandle(device);
        numbers.push_back(number);
    }
    return numbers;
}

std::vector<DiskInfo> EnumerateDisks() {
    // Volume queries hit the hardware, and an empty card reader or optical
    // drive answers with a hard error - which Windows turns into a modal "There
    // is no disk in the drive" box, from a window opened to look at drives.
    DWORD previousErrorMode = 0;
    SetThreadErrorMode(SEM_FAILCRITICALERRORS, &previousErrorMode);

    std::vector<VolumeInfo> volumes;

    WCHAR volumeName[MAX_PATH];
    HANDLE find = FindFirstVolumeW(volumeName, ARRAYSIZE(volumeName));
    if (find != INVALID_HANDLE_VALUE) {
        do {
            VolumeInfo volume;
            volume.guidPath = volumeName;

            WCHAR label[MAX_PATH] = L"";
            WCHAR fileSystem[64] = L"";
            DWORD serial = 0, maxComponent = 0, flags = 0;
            if (GetVolumeInformationW(volumeName, label, ARRAYSIZE(label), &serial,
                                      &maxComponent, &flags, fileSystem,
                                      ARRAYSIZE(fileSystem))) {
                volume.label = label;
                volume.fileSystem = fileSystem;
            }

            ULARGE_INTEGER available{}, total{}, free{};
            if (GetDiskFreeSpaceExW(volumeName, &available, &total, &free)) {
                volume.size = total.QuadPart;
                volume.freeSpace = free.QuadPart;
            }

            // Drive letter, when the volume has one mounted.
            WCHAR paths[512] = L"";
            DWORD pathsLength = 0;
            if (GetVolumePathNamesForVolumeNameW(volumeName, paths,
                                                 ARRAYSIZE(paths), &pathsLength) &&
                paths[0]) {
                std::wstring first = paths;
                if (first.size() >= 2 && first[1] == L':') {
                    volume.letter = first.substr(0, 2);
                }
            }

            QueryVolumePlacement(volume.guidPath, &volume.diskNumber,
                                 &volume.offset);

            WCHAR windowsDir[MAX_PATH] = L"";
            if (GetWindowsDirectoryW(windowsDir, ARRAYSIZE(windowsDir)) &&
                !volume.letter.empty()) {
                volume.isBoot = _wcsnicmp(windowsDir, volume.letter.c_str(), 2) == 0;
            }

            // A mounted volume gets its own shell icon; the rest get a
            // generic drive, since EFI and recovery partitions have none.
            if (!volume.letter.empty()) {
                volume.image = ShellImage(volume.letter + L"\\",
                                          VolumeIconPixels());
            }
            if (!volume.image) {
                volume.icon = StockDriveIcon(SIID_DRIVEFIXED, VolumeIconPixels());
            }

            volumes.push_back(std::move(volume));
        } while (FindNextVolumeW(find, volumeName, ARRAYSIZE(volumeName)));
        FindVolumeClose(find);
    }

    // Start from the physical disks, then hang the volumes off them. Doing it
    // the other way round hides any disk with nothing mounted.
    std::vector<DiskInfo> disks;
    for (int number : EnumeratePhysicalDisks()) {
        DiskInfo disk;
        disk.number = number;
        disk.model = QueryDiskModel(number);
        if (disk.model.empty()) {
            disk.model = L"Disk " + std::to_wstring(number);
        }
        disk.size = QueryDiskSize(number);
        disk.removable = IsRemovableDisk(number);
        disks.push_back(std::move(disk));
    }

    for (const auto& volume : volumes) {
        if (volume.diskNumber < 0) {
            continue;
        }
        auto existing = std::find_if(
            disks.begin(), disks.end(),
            [&](const DiskInfo& disk) { return disk.number == volume.diskNumber; });
        if (existing != disks.end()) {
            existing->volumes.push_back(volume);
            continue;
        }
        // A volume on a disk the probe missed - keep it rather than drop it.
        DiskInfo disk;
        disk.number = volume.diskNumber;
        disk.model = QueryDiskModel(disk.number);
        if (disk.model.empty()) {
            disk.model = L"Disk " + std::to_wstring(disk.number);
        }
        disk.size = QueryDiskSize(disk.number);
        disk.removable = IsRemovableDisk(disk.number);
        disk.volumes.push_back(volume);
        disks.push_back(std::move(disk));
    }

    std::sort(disks.begin(), disks.end(),
              [](const DiskInfo& a, const DiskInfo& b) {
                  return a.number < b.number;
              });

    for (auto& disk : disks) {
        for (const auto& volume : disk.volumes) {
            if (!volume.letter.empty()) {
                disk.image = ShellImage(volume.letter + L"\\", DiskIconPixels());
                if (disk.image) {
                    break;
                }
            }
        }
        if (!disk.image) {
            disk.icon = StockDriveIcon(
                disk.removable ? SIID_DRIVEREMOVE : SIID_DRIVEFIXED,
                DiskIconPixels());
        }

        // Attach each volume to its partition table entry, matched by where it
        // starts on the disk.
        std::vector<PartitionMeta> partitions =
            QueryPartitions(disk.number, &disk.style);
        for (auto& volume : disk.volumes) {
            for (const auto& partition : partitions) {
                if (partition.offset == volume.offset) {
                    volume.role = partition.role;
                    volume.isSystem = partition.isSystem;
                    break;
                }
            }
        }

        std::sort(disk.volumes.begin(), disk.volumes.end(),
                  [](const VolumeInfo& a, const VolumeInfo& b) {
                      return a.offset < b.offset;
                  });
        if (!disk.size) {
            for (const auto& volume : disk.volumes) {
                disk.size += volume.size;
            }
        }

        // The map: every partition in disk order, with the space between them
        // called out as unallocated. Alignment slack - the megabyte before the
        // first partition, the tail after the last - is not worth a box, so
        // only gaps big enough to hold something are shown.
        constexpr ULONGLONG kSmallestGap = 16ull * 1024 * 1024;
        std::sort(partitions.begin(), partitions.end(),
                  [](const PartitionMeta& a, const PartitionMeta& b) {
                      return a.offset < b.offset;
                  });

        ULONGLONG cursor = 0;
        for (const auto& partition : partitions) {
            if (partition.offset > cursor &&
                partition.offset - cursor >= kSmallestGap) {
                DiskSegment gap;
                gap.offset = cursor;
                gap.length = partition.offset - cursor;
                gap.unallocated = true;
                disk.segments.push_back(std::move(gap));
            }

            DiskSegment segment;
            segment.offset = partition.offset;
            segment.length = partition.length;
            segment.role = partition.role;
            segment.isSystem = partition.isSystem;
            for (size_t i = 0; i < disk.volumes.size(); i++) {
                if (disk.volumes[i].offset == partition.offset) {
                    segment.volumeIndex = static_cast<int>(i);
                    break;
                }
            }
            disk.segments.push_back(std::move(segment));
            cursor = partition.offset + partition.length;
        }
        if (disk.size > cursor && disk.size - cursor >= kSmallestGap) {
            DiskSegment tail;
            tail.offset = cursor;
            tail.length = disk.size - cursor;
            tail.unallocated = true;
            disk.segments.push_back(std::move(tail));
        }

        // No partition table to read - an unformatted or unreadable disk. Fall
        // back to the volumes themselves so the map is not blank.
        if (disk.segments.empty()) {
            for (size_t i = 0; i < disk.volumes.size(); i++) {
                DiskSegment segment;
                segment.offset = disk.volumes[i].offset;
                segment.length = disk.volumes[i].size;
                segment.volumeIndex = static_cast<int>(i);
                segment.role = disk.volumes[i].role;
                segment.isSystem = disk.volumes[i].isSystem;
                disk.segments.push_back(std::move(segment));
            }
        }
    }
    std::sort(disks.begin(), disks.end(),
              [](const DiskInfo& a, const DiskInfo& b) {
                  return a.number < b.number;
              });

    SetThreadErrorMode(previousErrorMode, nullptr);

    if (g_settings.verboseLog) {
        Wh_Log(L"enumerated %zu disks", disks.size());
    }
    return disks;
}

// -----------------------------------------------------------------------------
// Actions - every one of these is Windows doing the work
// -----------------------------------------------------------------------------

// Asks the user's own Explorer to open a path, by reaching the Shell.Application
// object that belongs to the desktop's shell view and calling ShellExecute
// through it.
//
// Why not just ShellExecute the folder: this window runs elevated, because the
// console it replaces requires it. An elevated process launching explorer.exe
// hands the request to the already-running non-elevated Explorer, which opens a
// window, then re-navigates it to the default view - the folder appears for an
// instant and is replaced by This PC. Going through the desktop's own shell
// dispatch keeps the whole operation inside the user's session, so the window
// stays where it was sent.
bool OpenViaDesktopShell(const std::wstring& path) {
    // Defined here rather than linked: mingw's import libraries do not export
    // either of these shell GUIDs.
    static const CLSID kClsidShellWindows = {
        0x9BA05972,
        0xF6A8,
        0x11CF,
        {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};
    static const GUID kSidTopLevelBrowser = {
        0x4C96BE40,
        0x915C,
        0x11CF,
        {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

    IShellWindows* windows = nullptr;
    if (FAILED(CoCreateInstance(kClsidShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&windows))) ||
        !windows) {
        return false;
    }

    VARIANT location;
    VariantInit(&location);
    location.vt = VT_I4;
    location.lVal = CSIDL_DESKTOP;
    VARIANT empty;
    VariantInit(&empty);

    long desktopHwnd = 0;
    IDispatch* dispatch = nullptr;
    HRESULT hr = windows->FindWindowSW(&location, &empty, SWC_DESKTOP,
                                       &desktopHwnd, SWFO_NEEDDISPATCH,
                                       &dispatch);
    windows->Release();
    VariantClear(&location);
    if (FAILED(hr) || !dispatch) {
        return false;
    }

    bool launched = false;
    IServiceProvider* provider = nullptr;
    if (SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS(&provider))) && provider) {
        IShellBrowser* browser = nullptr;
        if (SUCCEEDED(provider->QueryService(kSidTopLevelBrowser,
                                             IID_PPV_ARGS(&browser))) &&
            browser) {
            IShellView* view = nullptr;
            if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
                IDispatch* viewDispatch = nullptr;
                if (SUCCEEDED(view->GetItemObject(SVGIO_BACKGROUND,
                                                  IID_PPV_ARGS(&viewDispatch))) &&
                    viewDispatch) {
                    IShellFolderViewDual* folderView = nullptr;
                    if (SUCCEEDED(viewDispatch->QueryInterface(
                            IID_PPV_ARGS(&folderView))) &&
                        folderView) {
                        IDispatch* appDispatch = nullptr;
                        if (SUCCEEDED(folderView->get_Application(&appDispatch)) &&
                            appDispatch) {
                            IShellDispatch2* shell = nullptr;
                            if (SUCCEEDED(appDispatch->QueryInterface(
                                    IID_PPV_ARGS(&shell))) &&
                                shell) {
                                BSTR file = SysAllocString(path.c_str());
                                VARIANT show;
                                VariantInit(&show);
                                show.vt = VT_I4;
                                show.lVal = SW_SHOWNORMAL;
                                launched = SUCCEEDED(shell->ShellExecute(
                                    file, empty, empty, empty, show));
                                SysFreeString(file);
                                shell->Release();
                            }
                            appDispatch->Release();
                        }
                        folderView->Release();
                    }
                    viewDispatch->Release();
                }
                view->Release();
            }
            browser->Release();
        }
        provider->Release();
    }
    dispatch->Release();
    return launched;
}

// Safely removes a volume: locks it, dismounts it, and tells the drive to eject
// its media. Windows' own "Safely Remove Hardware" does the same sequence; the
// lock is what makes it safe, since it fails rather than pulling the rug out
// from under open handles.
bool EjectVolume(HWND owner, const VolumeInfo& volume) {
    if (volume.letter.empty()) {
        return false;
    }

    // "\\.\C:" - the device path for the volume, with no trailing separator.
    std::wstring devicePath = L"\\\\.\\" + volume.letter;
    HANDLE device = CreateFileW(devicePath.c_str(), GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                OPEN_EXISTING, 0, nullptr);
    if (device == INVALID_HANDLE_VALUE) {
        MessageBoxW(owner, L"The volume could not be opened for ejecting.",
                    L"Eject", MB_OK | MB_ICONWARNING);
        return false;
    }

    DWORD returned = 0;
    bool locked = false;
    for (int attempt = 0; attempt < 10 && !locked; attempt++) {
        locked = DeviceIoControl(device, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0,
                                 &returned, nullptr) != FALSE;
        if (!locked) {
            Sleep(150);
        }
    }
    if (!locked) {
        CloseHandle(device);
        MessageBoxW(owner,
                    L"The volume is in use and could not be locked. Close any "
                    L"programs using it and try again.",
                    L"Eject", MB_OK | MB_ICONWARNING);
        return false;
    }

    DeviceIoControl(device, FSCTL_DISMOUNT_VOLUME, nullptr, 0, nullptr, 0,
                    &returned, nullptr);

    PREVENT_MEDIA_REMOVAL allow{};
    allow.PreventMediaRemoval = FALSE;
    DeviceIoControl(device, IOCTL_STORAGE_MEDIA_REMOVAL, &allow, sizeof(allow),
                    nullptr, 0, &returned, nullptr);

    bool ejected = DeviceIoControl(device, IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0,
                                   nullptr, 0, &returned, nullptr) != FALSE;
    CloseHandle(device);

    if (!ejected) {
        MessageBoxW(owner, L"Windows could not eject this drive.", L"Eject",
                    MB_OK | MB_ICONWARNING);
        return false;
    }
    MessageBoxW(owner, L"The drive can now be safely removed.", L"Eject",
                MB_OK | MB_ICONINFORMATION);
    return true;
}

// Opens the drive root - "C:\" - in Explorer. Only volumes with a letter can
// be opened this way; a volume GUID path technically resolves, but Explorer
// shows it as an unnavigable oddity, so those are refused instead.
void OpenInExplorer(HWND owner, const VolumeInfo& volume) {
    if (volume.letter.empty()) {
        MessageBoxW(owner,
                    L"This volume has no drive letter, so there is nothing for "
                    L"File Explorer to open.",
                    L"Open", MB_OK | MB_ICONINFORMATION);
        return;
    }

    std::wstring target = volume.letter + L"\\";  // "C:" -> "C:\"
    if (!PathFileExistsW(target.c_str())) {
        MessageBoxW(owner,
                    (L"Windows cannot access " + target + L".").c_str(), L"Open",
                    MB_OK | MB_ICONWARNING);
        return;
    }

    if (OpenViaDesktopShell(target)) {
        return;
    }

    // Fallback for the case where the desktop shell cannot be reached (no
    // Explorer running, for instance). This is the path that lands on the
    // default view, but it is better than doing nothing.
    Wh_Log(L"desktop shell unavailable, falling back to explorer.exe");
    ShellExecuteW(nullptr, nullptr, L"explorer.exe", target.c_str(), nullptr,
                  SW_SHOWNORMAL);
}

void ShowProperties(HWND owner, const VolumeInfo& volume) {
    std::wstring target =
        volume.letter.empty() ? volume.guidPath : volume.letter + L"\\";
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_NOASYNC;
    info.hwnd = owner;
    info.lpVerb = L"properties";
    info.lpFile = target.c_str();
    info.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&info);
}

// The shell's own Format dialog, with its own warnings.
void FormatVolume(HWND owner, const VolumeInfo& volume) {
    // Defence in depth: the button is not drawn for the boot volume, but the
    // action must refuse it too rather than relying on the UI.
    if (volume.isBoot) {
        MessageBoxW(owner,
                    L"Windows is running from this volume, so it cannot be "
                    L"formatted.",
                    L"Format", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (volume.letter.empty()) {
        MessageBoxW(owner,
                    L"This volume has no drive letter, so Windows' Format "
                    L"dialog cannot target it.\n\nUse Classic Disk Management "
                    L"for volumes without a letter.",
                    L"Format", MB_OK | MB_ICONINFORMATION);
        return;
    }

    using SHFormatDrive_t = DWORD(WINAPI*)(HWND, UINT, UINT, UINT);
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    auto formatDrive = reinterpret_cast<SHFormatDrive_t>(
        GetProcAddress(shell32, "SHFormatDrive"));
    if (!formatDrive) {
        return;
    }
    int driveIndex = towupper(volume.letter[0]) - L'A';
    formatDrive(owner, static_cast<UINT>(driveIndex), 0xFFFF /* SHFMT_ID_DEFAULT */,
                0);
}

// -----------------------------------------------------------------------------
// The window
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Shared helpers, used by both windows.
// -----------------------------------------------------------------------------

HINSTANCE ModuleInstance() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ModuleInstance), &module);
    return reinterpret_cast<HINSTANCE>(module);
}

// Regular weight Segoe UI, for the window caption.
HFONT MakeFontRegular(UINT dpi, int points) {
    LOGFONTW font{};
    font.lfHeight = -MulDiv(points, static_cast<int>(dpi), 72);
    font.lfWeight = FW_NORMAL;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    font.lfQuality = CLEARTYPE_QUALITY;
    font.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    wcscpy_s(font.lfFaceName, L"Segoe UI");
    return CreateFontIndirectW(&font);
}

// Segoe UI, bold. The face name alone is not enough: without an explicit
// charset and TrueType precision the font mapper is free to substitute
// something else, which is how the buttons ended up in a generic sans.
HFONT MakeFont(UINT dpi, int points, bool bold) {
    LOGFONTW font{};
    font.lfHeight = -MulDiv(points, static_cast<int>(dpi), 72);
    font.lfWeight = bold ? FW_BOLD : FW_SEMIBOLD;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    font.lfQuality = CLEARTYPE_QUALITY;
    font.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    wcscpy_s(font.lfFaceName, L"Segoe UI");
    HFONT created = CreateFontIndirectW(&font);
    if (created) {
        return created;
    }
    wcscpy_s(font.lfFaceName, L"Segoe UI Variable Text");
    return CreateFontIndirectW(&font);
}

// -----------------------------------------------------------------------------
// Volume properties
//
// Drawn here rather than handed to the shell's property sheet, which is the one
// piece of 2001 this window was meant to get away from. Everything shown is
// read-only.
// -----------------------------------------------------------------------------

namespace props {

constexpr PCWSTR kClassName = L"WindhawkModernDiskProperties";

struct PropState {
    HWND hwnd = nullptr;
    const VolumeInfo* volume = nullptr;
    const DiskInfo* disk = nullptr;
    HFONT fontTitle = nullptr;
    HFONT fontCaption = nullptr;  // title bar only, deliberately not bold
    HFONT fontBody = nullptr;
    HFONT fontSmall = nullptr;
    UINT dpi = 96;
    COLORREF cardColor = ui::kCard;
    COLORREF accentColor = RGB(76, 164, 224);
    RECT closeRect{};
    RECT openRect{};
    bool closeHovered = false;
    bool openHovered = false;
};

PropState* g_props;

void DrawField(PropState* state, HDC dc, int x, int* y, PCWSTR label,
               const std::wstring& value, int width) {
    SelectObject(dc, state->fontSmall);
    SetTextColor(dc, ui::kTextSecondary);
    RECT labelRect{x, *y, x + width, *y + ui::Scale(18, state->dpi)};
    DrawTextW(dc, label, -1, &labelRect,
              DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX);

    SelectObject(dc, state->fontBody);
    SetTextColor(dc, ui::kTextPrimary);
    RECT valueRect{x, *y + ui::Scale(17, state->dpi), x + width,
                   *y + ui::Scale(39, state->dpi)};
    DrawTextW(dc, value.c_str(), -1, &valueRect,
              DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS |
                  DT_NOPREFIX);
    *y += ui::Scale(46, state->dpi);
}

// The classic drive-properties doughnut, which is genuinely the clearest way to
// show used against free.
void DrawDoughnut(PropState* state, HDC dc, const RECT& box, ULONGLONG used,
                  ULONGLONG total) {
    HBRUSH freeBrush = CreateSolidBrush(ui::kBarTrack);
    HPEN nullPen = CreatePen(PS_SOLID, 1, ui::kBarTrack);
    HGDIOBJ oldBrush = SelectObject(dc, freeBrush);
    HGDIOBJ oldPen = SelectObject(dc, nullPen);
    Ellipse(dc, box.left, box.top, box.right, box.bottom);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(freeBrush);
    DeleteObject(nullPen);

    if (total && used) {
        double fraction = static_cast<double>(used) / total;
        if (fraction > 1.0) {
            fraction = 1.0;
        }
        int centreX = (box.left + box.right) / 2;
        int centreY = (box.top + box.bottom) / 2;
        int radius = (box.right - box.left) / 2;
        double angle = -3.14159265358979 / 2 + fraction * 2 * 3.14159265358979;

        HBRUSH usedBrush = CreateSolidBrush(state->accentColor);
        HPEN usedPen = CreatePen(PS_SOLID, 1, state->accentColor);
        oldBrush = SelectObject(dc, usedBrush);
        oldPen = SelectObject(dc, usedPen);
        if (fraction >= 0.999) {
            Ellipse(dc, box.left, box.top, box.right, box.bottom);
        } else {
            Pie(dc, box.left, box.top, box.right, box.bottom, centreX,
                box.top,  // start at 12 o'clock
                centreX + static_cast<int>(radius * cos(angle)),
                centreY + static_cast<int>(radius * sin(angle)));
        }
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(usedBrush);
        DeleteObject(usedPen);
    }

    // Punch the middle out to make it a ring.
    int inset = (box.right - box.left) / 3;
    RECT hole{box.left + inset, box.top + inset, box.right - inset,
              box.bottom - inset};
    HBRUSH holeBrush = CreateSolidBrush(state->cardColor);
    HPEN holePen = CreatePen(PS_SOLID, 1, state->cardColor);
    oldBrush = SelectObject(dc, holeBrush);
    oldPen = SelectObject(dc, holePen);
    Ellipse(dc, hole.left, hole.top, hole.right, hole.bottom);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(holeBrush);
    DeleteObject(holePen);

    if (total) {
        int percent = static_cast<int>(static_cast<double>(used) / total * 100);
        std::wstring caption = std::to_wstring(percent) + L"%";
        SelectObject(dc, state->fontBody);
        SetTextColor(dc, ui::kTextPrimary);
        RECT text = hole;
        DrawTextW(dc, caption.c_str(), -1, &text,
                  DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
    }
}

void Paint(PropState* state, HDC dc, const RECT& client) {
    HBRUSH background = CreateSolidBrush(state->cardColor);
    FillRect(dc, &client, background);
    DeleteObject(background);

    SetBkMode(dc, TRANSPARENT);
    const VolumeInfo& volume = *state->volume;
    int pad = ui::Scale(22, state->dpi);
    int titleHeight = ui::Scale(44, state->dpi);

    SelectObject(dc, state->fontCaption);
    SetTextColor(dc, ui::kTextPrimary);
    RECT titleBar{pad, 0, client.right, titleHeight};
    DrawTextW(dc, L"Properties", -1, &titleBar,
              DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX);

    state->closeRect = {client.right - ui::Scale(46, state->dpi), 0,
                        client.right, titleHeight};
    if (state->closeHovered) {
        HBRUSH brush = CreateSolidBrush(ui::kCloseHover);
        FillRect(dc, &state->closeRect, brush);
        DeleteObject(brush);
    }
    SetTextColor(dc, state->closeHovered ? ui::kTextPrimary : ui::kTextSecondary);
    RECT closeGlyph = state->closeRect;
    DrawTextW(dc, L"\x2715", -1, &closeGlyph,
              DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);

    // Heading: what this volume is.
    SelectObject(dc, state->fontTitle);
    SetTextColor(dc, ui::kTextPrimary);
    RECT heading{pad, titleHeight, client.right - pad,
                 titleHeight + ui::Scale(38, state->dpi)};
    std::wstring name = VolumeDisplayName(volume);
    DrawTextW(dc, name.c_str(), -1, &heading,
              DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS |
                  DT_NOPREFIX);

    SelectObject(dc, state->fontSmall);
    SetTextColor(dc, ui::kTextSecondary);
    RECT sub{pad, heading.bottom, client.right - pad,
             heading.bottom + ui::Scale(20, state->dpi)};
    std::wstring subtitle = state->disk->model + L"   \x2022   Disk " +
                            std::to_wstring(state->disk->number);
    DrawTextW(dc, subtitle.c_str(), -1, &sub,
              DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX);

    // Doughnut and its legend, for volumes that report usage.
    int contentTop = sub.bottom + ui::Scale(16, state->dpi);
    ULONGLONG used =
        volume.size > volume.freeSpace ? volume.size - volume.freeSpace : 0;
    int textLeft = pad;

    if (volume.size && !volume.letter.empty()) {
        int size = ui::Scale(132, state->dpi);
        RECT box{pad, contentTop, pad + size, contentTop + size};
        DrawDoughnut(state, dc, box, used, volume.size);

        int legendY = box.top + ui::Scale(28, state->dpi);
        int legendX = box.right + ui::Scale(20, state->dpi);
        int dot = ui::Scale(10, state->dpi);

        struct Entry {
            COLORREF color;
            PCWSTR label;
            ULONGLONG value;
        };
        const Entry entries[] = {
            {state->accentColor, L"Used", used},
            {ui::kBarTrack, L"Free", volume.freeSpace},
        };
        for (const auto& entry : entries) {
            RECT marker{legendX, legendY + ui::Scale(5, state->dpi),
                        legendX + dot, legendY + ui::Scale(5, state->dpi) + dot};
            ui::FillRoundRect(dc, marker, dot / 2, entry.color);

            SelectObject(dc, state->fontBody);
            SetTextColor(dc, ui::kTextPrimary);
            RECT text{legendX + dot + ui::Scale(10, state->dpi), legendY,
                      client.right - pad,
                      legendY + ui::Scale(22, state->dpi)};
            std::wstring caption =
                std::wstring(entry.label) + L"   " + ui::FormatSize(entry.value);
            DrawTextW(dc, caption.c_str(), -1, &text,
                      DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX);
            legendY += ui::Scale(30, state->dpi);
        }
        contentTop = box.bottom + ui::Scale(18, state->dpi);
    }

    // Fields, two columns.
    int columnWidth = (client.right - pad * 2 - ui::Scale(24, state->dpi)) / 2;
    int leftY = contentTop;
    int rightY = contentTop;

    DrawField(state, dc, textLeft, &leftY, L"File system",
              volume.fileSystem.empty() ? L"RAW" : volume.fileSystem, columnWidth);
    DrawField(state, dc, textLeft, &leftY, L"Capacity",
              ui::FormatSize(volume.size), columnWidth);
    DrawField(state, dc, textLeft, &leftY, L"Partition",
              volume.role.empty() ? L"Unknown" : volume.role, columnWidth);

    int rightX = pad + columnWidth + ui::Scale(24, state->dpi);
    DrawField(state, dc, rightX, &rightY, L"Drive letter",
              volume.letter.empty() ? L"None assigned" : volume.letter,
              columnWidth);
    DrawField(state, dc, rightX, &rightY, L"Free space",
              volume.size ? ui::FormatSize(volume.freeSpace) : L"Not reported",
              columnWidth);
    DrawField(state, dc, rightX, &rightY, L"Role",
              volume.isBoot      ? L"Boot volume"
              : volume.isSystem  ? L"System"
                                 : L"Data",
              columnWidth);

    // The volume path is long, so it gets the full width and sits clear of the
    // buttons rather than being drawn underneath them.
    int y = std::max(leftY, rightY);
    int buttonHeight = ui::Scale(32, state->dpi);
    int pathLimit = client.bottom - pad - buttonHeight - ui::Scale(58, state->dpi);
    if (y > pathLimit) {
        y = pathLimit;
    }
    DrawField(state, dc, textLeft, &y, L"Volume path", volume.guidPath,
              client.right - pad * 2);

    // Buttons.
    int buttonWidth = ui::Scale(170, state->dpi);
    state->openRect = {pad, client.bottom - pad - buttonHeight,
                       pad + buttonWidth, client.bottom - pad};
    if (!volume.letter.empty()) {
        ui::FillRoundRect(dc, state->openRect, ui::Scale(4, state->dpi),
                          ui::MixColors(state->cardColor, RGB(255, 255, 255),
                                        state->openHovered ? 20 : 12));
        SelectObject(dc, state->fontSmall);
        SetTextColor(dc, ui::kTextPrimary);
        RECT text = state->openRect;
        DrawTextW(dc, L"Open in File Explorer", -1, &text,
                  DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
    } else {
        state->openRect = {};
    }
}

LRESULT CALLBACK PropProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PropState* state = g_props;
    if (!state) {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    switch (message) {
        case WM_NCCALCSIZE:
            if (wParam == TRUE) {
                return 0;
            }
            break;

        case WM_NCHITTEST: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &point);
            if (point.y < ui::Scale(44, state->dpi) &&
                !PtInRect(&state->closeRect, point)) {
                return HTCAPTION;
            }
            return HTCLIENT;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT client;
            GetClientRect(hwnd, &client);
            HDC memory = CreateCompatibleDC(dc);
            HBITMAP bitmap =
                CreateCompatibleBitmap(dc, client.right, client.bottom);
            HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
            Paint(state, memory, client);
            BitBlt(dc, 0, 0, client.right, client.bottom, memory, 0, 0, SRCCOPY);
            SelectObject(memory, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(memory);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            bool close = PtInRect(&state->closeRect, point) != FALSE;
            bool open = PtInRect(&state->openRect, point) != FALSE;
            if (close != state->closeHovered || open != state->openHovered) {
                state->closeHovered = close;
                state->openHovered = open;
                InvalidateRect(hwnd, nullptr, FALSE);
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&track);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            state->closeHovered = false;
            state->openHovered = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_LBUTTONUP: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (PtInRect(&state->closeRect, point)) {
                DestroyWindow(hwnd);
            } else if (PtInRect(&state->openRect, point)) {
                OpenInExplorer(hwnd, *state->volume);
            }
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                DestroyWindow(hwnd);
            }
            return 0;

        case WM_DESTROY:
            // Emphatically not PostQuitMessage: this window shares its thread
            // with the disk window, and a WM_QUIT in that queue would end its
            // message loop and exit the process. A null message just wakes the
            // modal loop so it can see the window is gone.
            PostThreadMessageW(GetCurrentThreadId(), WM_NULL, 0, 0);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool g_classRegistered;

bool EnsureClass(HINSTANCE instance) {
    if (g_classRegistered) {
        return true;
    }
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = PropProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;
    if (RegisterClassExW(&wc)) {
        g_classRegistered = true;
        return true;
    }
    return false;
}

// As in diskui: only clear the flag when the class was really unregistered.
void UnregisterWindowClassIfNeeded(HINSTANCE instance) {
    if (g_classRegistered && UnregisterClassW(kClassName, instance)) {
        g_classRegistered = false;
    }
}

// Modal against the parent, on the same thread.
void Show(HWND parent, HINSTANCE instance, const DiskInfo& disk,
          const VolumeInfo& volume, COLORREF cardColor, COLORREF accentColor) {
    if (!EnsureClass(instance)) {
        return;
    }

    PropState state;
    state.volume = &volume;
    state.disk = &disk;
    state.cardColor = cardColor;
    state.accentColor = accentColor;
    g_props = &state;

    state.hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME, kClassName, L"Properties",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, CW_USEDEFAULT,
        CW_USEDEFAULT, 100, 100, parent, nullptr, instance, nullptr);
    if (!state.hwnd) {
        g_props = nullptr;
        return;
    }

    state.dpi = GetDpiForWindow(state.hwnd);
    state.fontTitle = MakeFont(state.dpi, 15, true);
    state.fontCaption = MakeFontRegular(state.dpi, 10);
    state.fontBody = MakeFont(state.dpi, 10, true);
    state.fontSmall = MakeFont(state.dpi, 9, true);

    BOOL dark = TRUE;
    DwmSetWindowAttribute(state.hwnd, 20, &dark, sizeof(dark));
    DWORD corner = 2;
    DwmSetWindowAttribute(state.hwnd, 33, &corner, sizeof(corner));

    int width = ui::Scale(540, state.dpi);
    int height = ui::Scale(540, state.dpi);
    RECT parentRect{};
    GetWindowRect(parent, &parentRect);
    SetWindowPos(state.hwnd, nullptr,
                 parentRect.left + ((parentRect.right - parentRect.left) - width) / 2,
                 parentRect.top + ((parentRect.bottom - parentRect.top) - height) / 2,
                 width, height,
                 SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);

    EnableWindow(parent, FALSE);
    ShowWindow(state.hwnd, SW_SHOW);
    SetForegroundWindow(state.hwnd);

    MSG msg;
    while (IsWindow(state.hwnd)) {
        BOOL result = GetMessageW(&msg, nullptr, 0, 0);
        if (result <= 0) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);

    if (state.fontTitle) DeleteObject(state.fontTitle);
    if (state.fontCaption) DeleteObject(state.fontCaption);
    if (state.fontBody) DeleteObject(state.fontBody);
    if (state.fontSmall) DeleteObject(state.fontSmall);
    g_props = nullptr;
}

}  // namespace props

namespace diskui {

constexpr PCWSTR kClassName = L"WindhawkModernDiskManagement";
constexpr UINT_PTR kScrollTimer = 1;

enum class ButtonKind { Properties, Format, Explorer, Eject, Refresh, Close };

struct Button {
    RECT rect{};
    ButtonKind kind = ButtonKind::Properties;
    std::wstring label;
    bool accent = false;
    bool enabled = true;
};

// A clickable volume - the same volume is a row in the table and a box on the
// map, and either one selects it.
struct Target {
    RECT rect{};
    int diskIndex = -1;
    int volumeIndex = -1;
};

struct State {
    HWND hwnd = nullptr;
    HFONT fontCaption = nullptr;  // title bar, regular weight: it is chrome
    HFONT fontSection = nullptr;
    HFONT fontName = nullptr;
    HFONT fontRow = nullptr;
    HFONT fontSmall = nullptr;
    HFONT fontButton = nullptr;
    UINT dpi = 96;
    COLORREF windowColor = ui::kWindow;
    COLORREF cardColor = ui::kCard;
    COLORREF accentColor = RGB(76, 164, 224);

    std::vector<DiskInfo> disks;
    std::vector<Button> buttons;  // rebuilt on every paint
    std::vector<Target> targets;  // rebuilt on every paint
    int hoveredButton = -1;
    POINT mouse{-1, -1};
    int selectedDisk = -1;
    int selectedVolume = -1;
    int scroll = 0;
    int scrollTarget = 0;
    int contentTop = 0;
    int contentBottom = 0;
    int contentHeight = 0;
    bool done = false;
};

State* g_state;

// -----------------------------------------------------------------------------
// Small painting helpers
// -----------------------------------------------------------------------------

constexpr UINT kTextLeft =
    DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS;
constexpr UINT kTextRight =
    DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX | DT_END_ELLIPSIS;

void DrawLabel(HDC dc, HFONT font, COLORREF color, const std::wstring& text,
               RECT rect, UINT flags = kTextLeft) {
    SelectObject(dc, font);
    SetTextColor(dc, color);
    DrawTextW(dc, text.c_str(), -1, &rect, flags);
}

// A rounded box with a hairline border - the shape everything in this window is
// built out of.
void DrawSurface(HDC dc, const RECT& rect, int radius, COLORREF fill,
                 COLORREF border) {
    ui::FillRoundRect(dc, rect, radius, fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius * 2,
              radius * 2);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(pen);
}

void FillPlain(HDC dc, const RECT& rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

// A dot, for the status lights.
void DrawDot(HDC dc, int x, int y, int size, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    Ellipse(dc, x, y, x + size, y + size);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

COLORREF Lighten(State* state, int percent) {
    return ui::MixColors(state->cardColor, RGB(255, 255, 255), percent);
}

// -----------------------------------------------------------------------------
// What a volume is called in each column
// -----------------------------------------------------------------------------

// The console's own vocabulary, kept deliberately: this is the same
// information, not a different tool with different words for it.
std::wstring StatusText(const VolumeInfo& volume) {
    if (volume.isBoot) {
        return L"Healthy (Boot, Page File, Crash Dump)";
    }
    if (volume.role == L"EFI system partition") {
        return L"Healthy (EFI System Partition)";
    }
    if (volume.role == L"Recovery partition") {
        return L"Healthy (Recovery Partition)";
    }
    if (volume.role == L"Microsoft reserved partition") {
        return L"Healthy (Reserved)";
    }
    if (!volume.letter.empty()) {
        return L"Healthy (Primary Partition)";
    }
    return volume.role.empty() ? L"Healthy" : L"Healthy (" + volume.role + L")";
}

std::wstring PercentFreeText(const VolumeInfo& volume) {
    if (!volume.size || volume.letter.empty()) {
        return L"\x2014";
    }
    int percent = static_cast<int>(volume.freeSpace * 100 / volume.size);
    return std::to_wstring(percent) + L" %";
}

// What a partition is called when the box is too narrow for its full name.
std::wstring ShortName(const std::wstring& name) {
    if (name == L"EFI system partition") {
        return L"EFI";
    }
    if (name == L"Microsoft reserved partition") {
        return L"Reserved";
    }
    if (name == L"Recovery partition") {
        return L"Recovery";
    }
    if (name == L"Basic data partition") {
        return L"Data";
    }
    return name;
}

// Colour by what the partition is for. The boot volume takes the system accent
// so the disk Windows runs from is obvious at a glance.
COLORREF SegmentColor(State* state, const DiskInfo& disk,
                      const DiskSegment& segment) {
    if (segment.unallocated) {
        return ui::kUnallocated;
    }
    if (segment.volumeIndex < 0) {
        return ui::kBarSystem;  // a partition, just not a mounted one
    }
    const VolumeInfo& volume = disk.volumes[segment.volumeIndex];
    if (volume.isBoot) {
        return state->accentColor;
    }
    if (volume.isSystem || segment.isSystem) {
        return ui::kBarSystem;
    }
    return ui::kBarData;
}

// -----------------------------------------------------------------------------
// Buttons
// -----------------------------------------------------------------------------

void DrawButton(State* state, HDC dc, const Button& button, int index) {
    bool hovered = index == state->hoveredButton && button.enabled;
    COLORREF fill;
    COLORREF text;
    if (!button.enabled) {
        fill = Lighten(state, 4);
        text = ui::kTextDisabled;
    } else if (button.accent) {
        fill = hovered ? ui::MixColors(state->accentColor, RGB(255, 255, 255), 12)
                       : state->accentColor;
        text = ui::kTextPrimary;
    } else {
        fill = Lighten(state, hovered ? 17 : 10);
        text = ui::kTextPrimary;
    }
    ui::FillRoundRect(dc, button.rect, ui::Scale(5, state->dpi), fill);
    DrawLabel(dc, state->fontButton, text, button.label, button.rect,
              DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
}

int AddButton(State* state, HDC dc, RECT rect, ButtonKind kind, PCWSTR label,
              bool accent, bool enabled) {
    Button button;
    button.rect = rect;
    button.kind = kind;
    button.label = label;
    button.accent = accent;
    button.enabled = enabled;
    state->buttons.push_back(std::move(button));
    int index = static_cast<int>(state->buttons.size()) - 1;
    DrawButton(state, dc, state->buttons.back(), index);
    return index;
}

// -----------------------------------------------------------------------------
// The volume table
// -----------------------------------------------------------------------------

struct Column {
    PCWSTR title;
    int width;      // design pixels; 0 means "take what is left"
    bool rightAlign;
    int dropOrder;  // dropped in this order as the window narrows; 0 never
};

// Column order is the console's, which is the order these facts are usually
// read in. Narrow windows lose columns from the right of the list first, so
// what remains is always the identifying information.
const Column kColumns[] = {
    {L"Volume", 0, false, 0},    {L"Layout", 74, false, 3},
    {L"Type", 62, false, 2},     {L"File system", 92, false, 6},
    {L"Status", 232, false, 5},  {L"Capacity", 88, true, 0},
    {L"Free space", 88, true, 4}, {L"% Free", 62, true, 1},
};
constexpr int kColumnCount = static_cast<int>(ARRAYSIZE(kColumns));

// Works out which columns fit and how wide each one is, given the space the
// table has. The Volume column absorbs whatever is left over.
void LayoutColumns(State* state, int tableWidth, bool visible[kColumnCount],
                   int widths[kColumnCount]) {
    int minimumVolume = ui::Scale(210, state->dpi);
    for (int i = 0; i < kColumnCount; i++) {
        visible[i] = true;
        widths[i] = ui::Scale(kColumns[i].width, state->dpi);
    }

    auto flexible = [&] {
        int used = 0;
        for (int i = 0; i < kColumnCount; i++) {
            if (visible[i]) {
                used += widths[i];
            }
        }
        return tableWidth - used;
    };

    for (int order = 1; order <= 6 && flexible() < minimumVolume; order++) {
        for (int i = 0; i < kColumnCount; i++) {
            if (kColumns[i].dropOrder == order) {
                visible[i] = false;
            }
        }
    }
    widths[0] = std::max(ui::Scale(120, state->dpi), flexible());
}

// Paints the table of every volume on every disk, and returns its height.
int PaintVolumeTable(State* state, HDC dc, int top, int left, int right) {
    int pad = ui::Scale(14, state->dpi);
    int headerHeight = ui::Scale(32, state->dpi);
    int rowHeight = ui::Scale(42, state->dpi);
    int radius = ui::Scale(ui::kCardRadius, state->dpi);

    int rows = 0;
    for (const auto& disk : state->disks) {
        for (const auto& volume : disk.volumes) {
            if (g_settings.showEmptyVolumes || !volume.letter.empty()) {
                rows++;
            }
        }
    }

    int cardHeight = pad / 2 + headerHeight + rows * rowHeight + pad / 2;
    RECT card{left, top, right, top + cardHeight};
    DrawSurface(dc, card, radius, state->cardColor, ui::kCardBorder);

    int tableLeft = card.left + pad;
    int tableRight = card.right - pad;
    bool visible[kColumnCount];
    int widths[kColumnCount];
    LayoutColumns(state, tableRight - tableLeft, visible, widths);

    // Column headers.
    int headerTop = card.top + pad / 2;
    {
        int x = tableLeft;
        for (int i = 0; i < kColumnCount; i++) {
            if (!visible[i]) {
                continue;
            }
            RECT cell{x, headerTop, x + widths[i], headerTop + headerHeight};
            cell.right -= ui::Scale(10, state->dpi);
            DrawLabel(dc, state->fontSmall, ui::kTextTertiary, kColumns[i].title,
                      cell, kColumns[i].rightAlign ? kTextRight : kTextLeft);
            x += widths[i];
        }
        RECT rule{tableLeft, headerTop + headerHeight - ui::Scale(1, state->dpi),
                  tableRight, headerTop + headerHeight};
        FillPlain(dc, rule, ui::kCardBorder);
    }

    int y = headerTop + headerHeight;
    for (size_t diskIndex = 0; diskIndex < state->disks.size(); diskIndex++) {
        const DiskInfo& disk = state->disks[diskIndex];
        for (size_t volumeIndex = 0; volumeIndex < disk.volumes.size();
             volumeIndex++) {
            const VolumeInfo& volume = disk.volumes[volumeIndex];
            if (!g_settings.showEmptyVolumes && volume.letter.empty()) {
                continue;
            }

            RECT row{card.left + ui::Scale(6, state->dpi), y,
                     card.right - ui::Scale(6, state->dpi), y + rowHeight};
            bool selected = state->selectedDisk == static_cast<int>(diskIndex) &&
                            state->selectedVolume == static_cast<int>(volumeIndex);
            bool hovered = PtInRect(&row, state->mouse) != FALSE;
            if (selected) {
                ui::FillRoundRect(dc, row, ui::Scale(6, state->dpi),
                                  ui::MixColors(state->cardColor,
                                                state->accentColor, 26));
                // The marker on the leading edge, so the selected row is still
                // obvious against a tinted background.
                RECT marker{row.left, row.top + ui::Scale(7, state->dpi),
                            row.left + ui::Scale(3, state->dpi),
                            row.bottom - ui::Scale(7, state->dpi)};
                ui::FillRoundRect(dc, marker, ui::Scale(2, state->dpi),
                                  state->accentColor);
            } else if (hovered) {
                ui::FillRoundRect(dc, row, ui::Scale(6, state->dpi),
                                  Lighten(state, 6));
            }

            Target target;
            target.rect = row;
            target.diskIndex = static_cast<int>(diskIndex);
            target.volumeIndex = static_cast<int>(volumeIndex);
            state->targets.push_back(target);

            std::wstring cells[kColumnCount] = {
                VolumeDisplayName(volume),
                L"Simple",
                L"Basic",
                volume.fileSystem.empty() ? L"RAW" : volume.fileSystem,
                StatusText(volume),
                ui::FormatSize(volume.size),
                volume.letter.empty() ? L"\x2014"
                                      : ui::FormatSize(volume.freeSpace),
                PercentFreeText(volume),
            };

            int x = tableLeft;
            for (int i = 0; i < kColumnCount; i++) {
                if (!visible[i]) {
                    continue;
                }
                RECT cell{x, row.top, x + widths[i], row.bottom};
                cell.right -= ui::Scale(10, state->dpi);

                if (i == 0) {
                    // The name column carries the volume's own icon.
                    int icon = ui::Scale(32, state->dpi);
                    int iconTop = row.top + ((row.bottom - row.top) - icon) / 2;
                    if (volume.image) {
                        DrawShellImage(dc, volume.image, cell.left, iconTop, icon);
                    } else if (volume.icon) {
                        DrawIconEx(dc, cell.left, iconTop, volume.icon, icon,
                                   icon, 0, nullptr, DI_NORMAL);
                    }
                    cell.left += icon + ui::Scale(10, state->dpi);
                    DrawLabel(dc, state->fontName, ui::kTextPrimary, cells[i],
                              cell);
                } else {
                    COLORREF color =
                        i == 4 ? ui::kTextTertiary : ui::kTextSecondary;
                    DrawLabel(dc, state->fontRow, color, cells[i], cell,
                              kColumns[i].rightAlign ? kTextRight : kTextLeft);
                }
                x += widths[i];
            }

            y += rowHeight;
        }
    }

    if (rows == 0) {
        RECT empty{tableLeft, y, tableRight, y + rowHeight};
        DrawLabel(dc, state->fontRow, ui::kTextSecondary,
                  L"No volumes could be read.", empty);
    }

    return cardHeight;
}

// -----------------------------------------------------------------------------
// The graphical map
// -----------------------------------------------------------------------------

// Divides the map's width between the segments in proportion to their size,
// with a floor so a 100 MB EFI partition is still visible and clickable. The
// space the floor takes comes out of the segments that can spare it.
std::vector<int> SegmentWidths(const DiskInfo& disk, int available, int floorWidth,
                               int count) {
    std::vector<int> widths(count, 0);
    if (count <= 0 || available <= 0) {
        return widths;
    }
    if (floorWidth * count > available) {
        floorWidth = available / count;
    }

    ULONGLONG total = 0;
    for (const auto& segment : disk.segments) {
        total += segment.length;
    }
    if (!total) {
        for (int i = 0; i < count; i++) {
            widths[i] = available / count;
        }
        return widths;
    }

    std::vector<double> raw(count, 0.0);
    for (int i = 0; i < count; i++) {
        raw[i] = static_cast<double>(disk.segments[i].length) / total * available;
    }

    double owed = 0.0;    // what the floor costs
    double spare = 0.0;   // what the segments above the floor can give up
    for (int i = 0; i < count; i++) {
        if (raw[i] < floorWidth) {
            owed += floorWidth - raw[i];
        } else {
            spare += raw[i] - floorWidth;
        }
    }

    int used = 0;
    for (int i = 0; i < count; i++) {
        double value = raw[i];
        if (value < floorWidth) {
            value = floorWidth;
        } else if (spare > 0.0) {
            value -= owed * (raw[i] - floorWidth) / spare;
        }
        widths[i] = std::max(1, static_cast<int>(value));
        used += widths[i];
    }

    // Rounding leaves a pixel or two; give them to the widest segment so the
    // map fills its width exactly.
    int slack = available - used;
    if (slack != 0) {
        int widest = 0;
        for (int i = 1; i < count; i++) {
            if (widths[i] > widths[widest]) {
                widest = i;
            }
        }
        widths[widest] = std::max(1, widths[widest] + slack);
    }
    return widths;
}

// One partition box on the map.
void PaintSegment(State* state, HDC dc, const DiskInfo& disk, int diskIndex,
                  const DiskSegment& segment, const RECT& box) {
    int radius = ui::Scale(6, state->dpi);
    int width = box.right - box.left;
    bool unallocated = segment.unallocated;
    bool hasVolume = segment.volumeIndex >= 0;
    bool selected = hasVolume && state->selectedDisk == diskIndex &&
                    state->selectedVolume == segment.volumeIndex;
    bool hovered = hasVolume && PtInRect(&box, state->mouse) != FALSE;

    COLORREF accent = SegmentColor(state, disk, segment);
    COLORREF fill = ui::MixColors(state->cardColor, accent, selected ? 24 : 12);
    if (hovered && !selected) {
        fill = ui::MixColors(fill, RGB(255, 255, 255), 7);
    }
    if (unallocated) {
        fill = ui::MixColors(state->cardColor, RGB(0, 0, 0), 18);
    }

    // The type stripe across the top is clipped to the box's own rounded
    // corners; drawing it as a plain rectangle would poke out of them.
    HRGN region = CreateRoundRectRgn(box.left, box.top, box.right + 1,
                                     box.bottom + 1, radius * 2, radius * 2);
    int saved = SaveDC(dc);
    SelectClipRgn(dc, region);
    FillPlain(dc, box, fill);

    if (unallocated) {
        // Hatching, the way every partition editor since 1995 has drawn space
        // that belongs to nothing. The lines are stroked rather than filled
        // with a hatch brush: a hatch brush paints its gaps in the DC's
        // background colour, which is white unless every caller remembers to
        // set it, and one that forgot is what made this box glow.
        HPEN pen = CreatePen(PS_SOLID, 1, Lighten(state, 9));
        HGDIOBJ oldPen = SelectObject(dc, pen);
        int span = box.bottom - box.top;
        int step = ui::Scale(8, state->dpi);
        for (int x = box.left - span; x < box.right; x += step) {
            MoveToEx(dc, x, box.bottom, nullptr);
            LineTo(dc, x + span, box.top);
        }
        SelectObject(dc, oldPen);
        DeleteObject(pen);
    } else {
        RECT stripe{box.left, box.top, box.right,
                    box.top + ui::Scale(4, state->dpi)};
        FillPlain(dc, stripe, accent);
    }
    RestoreDC(dc, saved);
    DeleteObject(region);

    // Border, heavier and accented when the segment is the selected one.
    {
        HPEN pen = CreatePen(PS_SOLID, selected ? ui::Scale(2, state->dpi) : 1,
                             selected ? accent : ui::kCardBorder);
        HGDIOBJ oldPen = SelectObject(dc, pen);
        HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
        RoundRect(dc, box.left, box.top, box.right, box.bottom, radius * 2,
                  radius * 2);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(pen);
    }

    if (hasVolume) {
        Target target;
        target.rect = box;
        target.diskIndex = diskIndex;
        target.volumeIndex = segment.volumeIndex;
        state->targets.push_back(target);
    }

    // Below a certain width there is no honest way to fit text; the stripe and
    // the tooltip-free legend carry the meaning instead.
    int inset = ui::Scale(9, state->dpi);
    if (width < ui::Scale(54, state->dpi)) {
        return;
    }

    bool narrow = width < ui::Scale(150, state->dpi);
    RECT text{box.left + inset, box.top + ui::Scale(12, state->dpi),
              box.right - inset, box.top + ui::Scale(32, state->dpi)};

    std::wstring name;
    if (hasVolume) {
        name = VolumeDisplayName(disk.volumes[segment.volumeIndex]);
    } else if (unallocated) {
        name = L"Unallocated";
    } else {
        // A partition Windows does not mount - the reserved partition, most
        // often. It is not free space, and saying so would be a lie about the
        // one thing this window exists to show.
        name = segment.role.empty() ? L"Partition" : segment.role;
    }
    if (narrow) {
        name = ShortName(name);
    }
    DrawLabel(dc, state->fontName,
              hasVolume ? ui::kTextPrimary : ui::kTextSecondary, name, text);

    RECT sizeLine{text.left, text.bottom, text.right,
                  text.bottom + ui::Scale(18, state->dpi)};
    std::wstring size = ui::FormatSize(segment.length);
    if (hasVolume) {
        const VolumeInfo& volume = disk.volumes[segment.volumeIndex];
        if (!volume.fileSystem.empty()) {
            size += L"  " + volume.fileSystem;
        }
    }
    DrawLabel(dc, state->fontSmall, ui::kTextSecondary, size, sizeLine);

    if (narrow) {
        return;
    }

    if (!hasVolume) {
        RECT status{sizeLine.left, sizeLine.bottom, sizeLine.right,
                    sizeLine.bottom + ui::Scale(18, state->dpi)};
        DrawLabel(dc, state->fontSmall, ui::kTextTertiary,
                  unallocated ? L"Free space" : L"Healthy (No drive letter)",
                  status);
    }

    if (hasVolume) {
        const VolumeInfo& volume = disk.volumes[segment.volumeIndex];
        RECT status{sizeLine.left, sizeLine.bottom, sizeLine.right,
                    sizeLine.bottom + ui::Scale(18, state->dpi)};
        DrawLabel(dc, state->fontSmall, ui::kTextTertiary, StatusText(volume),
                  status);

        // How full the volume is, for the ones where that means anything.
        if (volume.size && !volume.letter.empty()) {
            int barHeight = ui::Scale(4, state->dpi);
            RECT track{box.left + inset, box.bottom - inset - barHeight,
                       box.right - inset, box.bottom - inset};
            ui::FillRoundRect(dc, track, barHeight / 2, ui::kBarTrack);

            ULONGLONG used = volume.size > volume.freeSpace
                                 ? volume.size - volume.freeSpace
                                 : 0;
            int usedWidth = static_cast<int>(static_cast<double>(used) /
                                             volume.size *
                                             (track.right - track.left));
            if (usedWidth > 0) {
                RECT fillRect{track.left, track.top, track.left + usedWidth,
                              track.bottom};
                ui::FillRoundRect(dc, fillRect, barHeight / 2, accent);
            }

            RECT usage{track.left, track.top - ui::Scale(18, state->dpi),
                       track.right, track.top - ui::Scale(2, state->dpi)};
            DrawLabel(dc, state->fontSmall, ui::kTextTertiary,
                      ui::FormatSize(volume.freeSpace) + L" free", usage);
        }
    }
}

// One disk: the tile that names it, and the map of what is on it.
int PaintDiskMap(State* state, HDC dc, const DiskInfo& disk, int diskIndex,
                 int top, int left, int right) {
    int pad = ui::Scale(14, state->dpi);
    int radius = ui::Scale(ui::kCardRadius, state->dpi);
    int tileWidth = ui::Scale(ui::kDiskTileWidth, state->dpi);
    int mapHeight = ui::Scale(ui::kMapHeight, state->dpi);
    int cardHeight = pad * 2 + mapHeight;

    RECT card{left, top, right, top + cardHeight};
    DrawSurface(dc, card, radius, state->cardColor, ui::kCardBorder);

    // The tile: which disk this is, and what it is.
    int tileLeft = card.left + pad;
    int tileTop = card.top + pad;
    int icon = ui::Scale(32, state->dpi);
    if (disk.image) {
        DrawShellImage(dc, disk.image, tileLeft, tileTop, icon);
    } else if (disk.icon) {
        DrawIconEx(dc, tileLeft, tileTop, disk.icon, icon, icon, 0, nullptr,
                   DI_NORMAL);
    }

    int textLeft = tileLeft + icon + ui::Scale(10, state->dpi);
    RECT diskName{textLeft, tileTop, tileLeft + tileWidth,
                  tileTop + ui::Scale(18, state->dpi)};
    DrawLabel(dc, state->fontName, ui::kTextPrimary,
              L"Disk " + std::to_wstring(disk.number), diskName);
    RECT diskSize{textLeft, diskName.bottom, tileLeft + tileWidth,
                  diskName.bottom + ui::Scale(16, state->dpi)};
    DrawLabel(dc, state->fontSmall, ui::kTextSecondary,
              ui::FormatSize(disk.size), diskSize);

    RECT model{tileLeft, tileTop + ui::Scale(44, state->dpi),
               tileLeft + tileWidth, tileTop + ui::Scale(62, state->dpi)};
    DrawLabel(dc, state->fontSmall, ui::kTextTertiary, disk.model, model);

    int dot = ui::Scale(7, state->dpi);
    DrawDot(dc, tileLeft, model.bottom + ui::Scale(6, state->dpi), dot,
            ui::kHealthy);
    RECT online{tileLeft + dot + ui::Scale(7, state->dpi), model.bottom,
                tileLeft + tileWidth, model.bottom + ui::Scale(18, state->dpi)};
    std::wstring line = L"Online";
    if (!disk.style.empty()) {
        line += L"  \x2022  " + disk.style;
    }
    if (disk.removable) {
        line += L"  \x2022  Removable";
    }
    DrawLabel(dc, state->fontSmall, ui::kTextSecondary, line, online);

    // A rule between the tile and the map, so the two read as separate things.
    RECT divider{tileLeft + tileWidth + pad, card.top + pad,
                 tileLeft + tileWidth + pad + ui::Scale(1, state->dpi),
                 card.bottom - pad};
    FillPlain(dc, divider, ui::kCardBorder);

    int mapLeft = divider.right + pad;
    int mapRight = card.right - pad;
    int count = static_cast<int>(disk.segments.size());
    if (count <= 0 || mapRight <= mapLeft) {
        return cardHeight;
    }

    int gap = ui::Scale(6, state->dpi);
    int available = (mapRight - mapLeft) - gap * (count - 1);
    std::vector<int> widths =
        SegmentWidths(disk, available, ui::Scale(56, state->dpi), count);

    int x = mapLeft;
    for (int i = 0; i < count; i++) {
        RECT box{x, card.top + pad, x + widths[i], card.top + pad + mapHeight};
        if (box.right > mapRight) {
            box.right = mapRight;
        }
        if (box.right - box.left < ui::Scale(6, state->dpi)) {
            break;
        }
        PaintSegment(state, dc, disk, diskIndex, disk.segments[i], box);
        x += widths[i] + gap;
    }

    return cardHeight;
}

// -----------------------------------------------------------------------------
// The window
// -----------------------------------------------------------------------------

const VolumeInfo* Selected(State* state) {
    if (state->selectedDisk < 0 ||
        state->selectedDisk >= static_cast<int>(state->disks.size())) {
        return nullptr;
    }
    const DiskInfo& disk = state->disks[state->selectedDisk];
    if (state->selectedVolume < 0 ||
        state->selectedVolume >= static_cast<int>(disk.volumes.size())) {
        return nullptr;
    }
    return &disk.volumes[state->selectedVolume];
}

// The bar along the bottom: what is selected, and what can be done to it.
// Actions live here rather than on every row, so the window is a list of facts
// with one place to act from - which is how the console reads too.
void PaintActionBar(State* state, HDC dc, const RECT& client) {
    int pad = ui::Scale(ui::kPadding, state->dpi);
    int height = ui::Scale(ui::kActionBarHeight, state->dpi);
    RECT bar{0, client.bottom - height, client.right, client.bottom};

    FillPlain(dc, bar, ui::MixColors(state->windowColor, RGB(0, 0, 0), 22));
    RECT rule{0, bar.top, client.right, bar.top + ui::Scale(1, state->dpi)};
    FillPlain(dc, rule, ui::kCardBorder);

    const VolumeInfo* volume = Selected(state);

    int buttonHeight = ui::Scale(ui::kButtonHeight, state->dpi);
    int buttonTop = bar.top + ((bar.bottom - bar.top) - buttonHeight) / 2;
    int right = client.right - pad;
    int spacing = ui::Scale(8, state->dpi);

    struct Action {
        PCWSTR label;
        ButtonKind kind;
        int width;
        bool accent;
    };
    // Built right to left, so the list reads in reverse of how it appears.
    const Action actions[] = {
        {L"Properties", ButtonKind::Properties, 96, true},
        {L"Eject", ButtonKind::Eject, 68, false},
        {L"Format", ButtonKind::Format, 76, false},
        {L"Open", ButtonKind::Explorer, 68, false},
        {L"Refresh", ButtonKind::Refresh, 82, false},
    };

    for (const auto& action : actions) {
        // Eject is meaningless on a disk that cannot be removed, so it is not
        // shown at all rather than shown greyed for ever.
        if (action.kind == ButtonKind::Eject) {
            bool removable =
                state->selectedDisk >= 0 &&
                state->selectedDisk < static_cast<int>(state->disks.size()) &&
                state->disks[state->selectedDisk].removable;
            if (!removable) {
                continue;
            }
        }

        bool enabled = true;
        switch (action.kind) {
            case ButtonKind::Refresh:
                break;
            case ButtonKind::Properties:
                enabled = volume != nullptr;
                break;
            case ButtonKind::Explorer:
            case ButtonKind::Eject:
                enabled = volume && !volume->letter.empty();
                break;
            case ButtonKind::Format:
                // Windows cannot format the volume it is running from.
                enabled = volume && !volume->letter.empty() && !volume->isBoot;
                break;
            default:
                break;
        }

        int width = ui::Scale(action.width, state->dpi);
        RECT rect{right - width, buttonTop, right, buttonTop + buttonHeight};
        AddButton(state, dc, rect, action.kind, action.label, action.accent,
                  enabled);
        right -= width + spacing;
    }

    // What is selected, on the left.
    RECT label{pad, bar.top, right - pad, bar.bottom};
    if (volume) {
        RECT name{label.left, bar.top + ui::Scale(11, state->dpi), label.right,
                  bar.top + ui::Scale(30, state->dpi)};
        DrawLabel(dc, state->fontName, ui::kTextPrimary,
                  VolumeDisplayName(*volume), name);

        std::wstring detail = ui::FormatSize(volume->size);
        if (!volume->fileSystem.empty()) {
            detail += L"  \x2022  " + volume->fileSystem;
        }
        if (!volume->letter.empty()) {
            detail += L"  \x2022  " + ui::FormatSize(volume->freeSpace) + L" free";
        }
        RECT sub{label.left, name.bottom, label.right,
                 name.bottom + ui::Scale(17, state->dpi)};
        DrawLabel(dc, state->fontSmall, ui::kTextSecondary, detail, sub);
    } else {
        DrawLabel(dc, state->fontRow, ui::kTextTertiary,
                  L"Select a volume to act on it", label);
    }
}

// The key to the colours in the map.
void PaintLegend(State* state, HDC dc, int top, int left, int right) {
    struct Entry {
        PCWSTR label;
        COLORREF color;
    };
    const Entry entries[] = {
        {L"Boot volume", state->accentColor},
        {L"Data", ui::kBarData},
        {L"System, reserved and recovery", ui::kBarSystem},
        {L"Unallocated", ui::kUnallocated},
    };

    SelectObject(dc, state->fontSmall);
    int x = left;
    int chip = ui::Scale(10, state->dpi);
    for (const auto& entry : entries) {
        SIZE extent{};
        GetTextExtentPoint32W(dc, entry.label, lstrlenW(entry.label), &extent);
        int needed = chip + ui::Scale(7, state->dpi) + extent.cx;
        if (x + needed > right) {
            break;
        }
        RECT marker{x, top + ui::Scale(4, state->dpi), x + chip,
                    top + ui::Scale(4, state->dpi) + chip};
        ui::FillRoundRect(dc, marker, ui::Scale(3, state->dpi), entry.color);
        RECT text{x + chip + ui::Scale(7, state->dpi), top,
                  x + needed + ui::Scale(4, state->dpi),
                  top + ui::Scale(18, state->dpi)};
        DrawLabel(dc, state->fontSmall, ui::kTextTertiary, entry.label, text);
        x += needed + ui::Scale(18, state->dpi);
    }
}

void Paint(State* state, HDC dc, const RECT& client) {
    state->buttons.clear();
    state->targets.clear();

    FillPlain(dc, client, state->windowColor);
    SetBkMode(dc, TRANSPARENT);

    int pad = ui::Scale(ui::kPadding, state->dpi);
    int titleHeight = ui::Scale(ui::kTitleHeight, state->dpi);

    // Title bar.
    RECT title{pad, 0, client.right, titleHeight};
    DrawLabel(dc, state->fontCaption, ui::kTextPrimary, L"Disk Management",
              title);

    {
        RECT close{client.right - ui::Scale(ui::kCloseWidth, state->dpi), 0,
                   client.right, titleHeight};
        Button button;
        button.rect = close;
        button.kind = ButtonKind::Close;
        button.label = L"\x2715";
        state->buttons.push_back(std::move(button));
        bool hovered =
            state->hoveredButton == static_cast<int>(state->buttons.size()) - 1;
        if (hovered) {
            FillPlain(dc, close, ui::kCloseHover);
        }
        DrawLabel(dc, state->fontCaption,
                  hovered ? ui::kTextPrimary : ui::kTextSecondary, L"\x2715",
                  close, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
    }

    state->contentTop = titleHeight;
    state->contentBottom =
        client.bottom - ui::Scale(ui::kActionBarHeight, state->dpi);

    int saved = SaveDC(dc);
    IntersectClipRect(dc, 0, state->contentTop, client.right,
                      state->contentBottom);

    int left = pad;
    int right = client.right - pad;
    int y = state->contentTop + pad - state->scroll;

    // Volumes.
    {
        RECT heading{left, y, right, y + ui::Scale(26, state->dpi)};
        DrawLabel(dc, state->fontSection, ui::kTextPrimary, L"Volumes", heading);

        size_t volumeCount = 0;
        for (const auto& disk : state->disks) {
            volumeCount += disk.volumes.size();
        }
        std::wstring count =
            std::to_wstring(volumeCount) +
            (volumeCount == 1 ? L" volume on " : L" volumes on ") +
            std::to_wstring(state->disks.size()) +
            (state->disks.size() == 1 ? L" disk" : L" disks");
        DrawLabel(dc, state->fontSmall, ui::kTextTertiary, count, heading,
                  kTextRight);

        y = heading.bottom + ui::Scale(10, state->dpi);
        y += PaintVolumeTable(state, dc, y, left, right);
    }

    // Disk layout.
    y += ui::Scale(22, state->dpi);
    {
        RECT heading{left, y, right, y + ui::Scale(26, state->dpi)};
        DrawLabel(dc, state->fontSection, ui::kTextPrimary, L"Disk layout",
                  heading);
        PaintLegend(state, dc, y + ui::Scale(4, state->dpi),
                    left + ui::Scale(150, state->dpi), right);
        y = heading.bottom + ui::Scale(10, state->dpi);
    }

    for (size_t i = 0; i < state->disks.size(); i++) {
        y += PaintDiskMap(state, dc, state->disks[i], static_cast<int>(i), y,
                          left, right);
        y += ui::Scale(12, state->dpi);
    }

    if (state->disks.empty()) {
        RECT empty{left, y, right, y + ui::Scale(40, state->dpi)};
        DrawLabel(dc, state->fontRow, ui::kTextSecondary,
                  L"No disks could be read.", empty);
        y = empty.bottom;
    }

    state->contentHeight = y + state->scroll - state->contentTop + pad;
    RestoreDC(dc, saved);

    // Scroll indicator, only when there is somewhere to scroll.
    int viewport = state->contentBottom - state->contentTop;
    if (state->contentHeight > viewport && viewport > 0) {
        int trackWidth = ui::Scale(ui::kScrollBarWidth, state->dpi);
        int trackRight = client.right - ui::Scale(6, state->dpi);
        RECT track{trackRight - trackWidth,
                   state->contentTop + ui::Scale(6, state->dpi), trackRight,
                   state->contentBottom - ui::Scale(6, state->dpi)};
        ui::FillRoundRect(dc, track, trackWidth / 2,
                          ui::MixColors(state->windowColor, RGB(255, 255, 255), 6));

        int trackHeight = track.bottom - track.top;
        int thumbHeight =
            std::max(ui::Scale(32, state->dpi),
                     MulDiv(trackHeight, viewport, state->contentHeight));
        int maxScroll = state->contentHeight - viewport;
        int thumbTop =
            track.top + (maxScroll > 0 ? MulDiv(trackHeight - thumbHeight,
                                                state->scroll, maxScroll)
                                       : 0);
        RECT thumb{track.left, thumbTop, track.right, thumbTop + thumbHeight};
        ui::FillRoundRect(dc, thumb, trackWidth / 2,
                          ui::MixColors(state->windowColor, RGB(255, 255, 255), 28));
    }

    PaintActionBar(state, dc, client);
}

void Refresh(State* state) {
    ReleaseDiskIcons(state->disks);
    state->disks = EnumerateDisks();
    if (!Selected(state)) {
        state->selectedDisk = -1;
        state->selectedVolume = -1;
    }
    InvalidateRect(state->hwnd, nullptr, FALSE);
}

void Invoke(State* state, const Button& button) {
    if (!button.enabled) {
        return;
    }

    switch (button.kind) {
        case ButtonKind::Close:
            state->done = true;
            DestroyWindow(state->hwnd);
            return;
        case ButtonKind::Refresh:
            Refresh(state);
            return;
        default:
            break;
    }

    const VolumeInfo* selected = Selected(state);
    if (!selected) {
        return;
    }
    const DiskInfo& disk = state->disks[state->selectedDisk];
    VolumeInfo volume = *selected;  // the list is rebuilt under some of these

    switch (button.kind) {
        case ButtonKind::Properties:
            props::Show(state->hwnd, ModuleInstance(), disk, volume,
                        state->cardColor, state->accentColor);
            InvalidateRect(state->hwnd, nullptr, FALSE);
            break;
        case ButtonKind::Format:
            FormatVolume(state->hwnd, volume);
            Refresh(state);
            break;
        case ButtonKind::Explorer:
            OpenInExplorer(state->hwnd, volume);
            break;
        case ButtonKind::Eject:
            if (EjectVolume(state->hwnd, volume)) {
                state->selectedDisk = -1;
                state->selectedVolume = -1;
                Refresh(state);
            }
            break;
        default:
            break;
    }
}

// Moves the selection up or down the volume list, so the whole window can be
// driven from the keyboard.
void MoveSelection(State* state, int delta) {
    std::vector<std::pair<int, int>> order;
    for (size_t d = 0; d < state->disks.size(); d++) {
        for (size_t v = 0; v < state->disks[d].volumes.size(); v++) {
            if (g_settings.showEmptyVolumes ||
                !state->disks[d].volumes[v].letter.empty()) {
                order.emplace_back(static_cast<int>(d), static_cast<int>(v));
            }
        }
    }
    if (order.empty()) {
        return;
    }

    int current = -1;
    for (size_t i = 0; i < order.size(); i++) {
        if (order[i].first == state->selectedDisk &&
            order[i].second == state->selectedVolume) {
            current = static_cast<int>(i);
            break;
        }
    }

    int next = current < 0 ? (delta > 0 ? 0 : static_cast<int>(order.size()) - 1)
                           : current + delta;
    next = std::clamp(next, 0, static_cast<int>(order.size()) - 1);
    state->selectedDisk = order[next].first;
    state->selectedVolume = order[next].second;
    InvalidateRect(state->hwnd, nullptr, FALSE);
}

bool SelectAt(State* state, POINT point) {
    for (const auto& target : state->targets) {
        if (PtInRect(&target.rect, point)) {
            state->selectedDisk = target.diskIndex;
            state->selectedVolume = target.volumeIndex;
            InvalidateRect(state->hwnd, nullptr, FALSE);
            return true;
        }
    }
    return false;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    State* state = g_state;
    if (!state) {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    switch (message) {
        case WM_NCCALCSIZE:
            if (wParam == TRUE) {
                return 0;  // frame exists, but none of it is drawn
            }
            break;

        case WM_NCHITTEST: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &point);
            RECT client;
            GetClientRect(hwnd, &client);

            // Resize grips along the trimmed frame, so the window can still be
            // sized even though none of its border is drawn.
            int edge = ui::Scale(6, state->dpi);
            bool left = point.x < edge;
            bool right = point.x >= client.right - edge;
            bool top = point.y < edge;
            bool bottom = point.y >= client.bottom - edge;
            if (top && left) return HTTOPLEFT;
            if (top && right) return HTTOPRIGHT;
            if (bottom && left) return HTBOTTOMLEFT;
            if (bottom && right) return HTBOTTOMRIGHT;
            if (left) return HTLEFT;
            if (right) return HTRIGHT;
            if (top) return HTTOP;
            if (bottom) return HTBOTTOM;

            int titleHeight = ui::Scale(ui::kTitleHeight, state->dpi);
            int closeLeft = client.right - ui::Scale(ui::kCloseWidth, state->dpi);
            if (point.y < titleHeight && point.x < closeLeft) {
                return HTCAPTION;
            }
            return HTCLIENT;
        }

        case WM_SIZE:
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_GETMINMAXINFO: {
            auto* info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = ui::Scale(760, state->dpi);
            info->ptMinTrackSize.y = ui::Scale(480, state->dpi);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT client;
            GetClientRect(hwnd, &client);
            HDC memory = CreateCompatibleDC(dc);
            HBITMAP bitmap =
                CreateCompatibleBitmap(dc, client.right, client.bottom);
            HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
            Paint(state, memory, client);
            BitBlt(dc, 0, 0, client.right, client.bottom, memory, 0, 0, SRCCOPY);
            SelectObject(memory, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(memory);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SETCURSOR: {
            // A hand over anything that can be clicked.
            POINT point;
            GetCursorPos(&point);
            ScreenToClient(hwnd, &point);
            bool overTarget = false;
            for (const auto& button : state->buttons) {
                if (button.enabled && button.kind != ButtonKind::Close &&
                    PtInRect(&button.rect, point)) {
                    overTarget = true;
                    break;
                }
            }
            if (LOWORD(lParam) == HTCLIENT && overTarget) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
            break;
        }

        case WM_MOUSEMOVE: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            bool moved = point.x != state->mouse.x || point.y != state->mouse.y;
            state->mouse = point;
            int hovered = -1;
            for (size_t i = 0; i < state->buttons.size(); i++) {
                if (PtInRect(&state->buttons[i].rect, point)) {
                    hovered = static_cast<int>(i);
                    break;
                }
            }
            if (hovered != state->hoveredButton || moved) {
                state->hoveredButton = hovered;
                InvalidateRect(hwnd, nullptr, FALSE);
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&track);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            state->hoveredButton = -1;
            state->mouse = {-1, -1};
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_LBUTTONDOWN: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            SetFocus(hwnd);
            // Buttons act on the release; a click anywhere else selects.
            for (const auto& button : state->buttons) {
                if (PtInRect(&button.rect, point)) {
                    return 0;
                }
            }
            SelectAt(state, point);
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (SelectAt(state, point) && Selected(state)) {
                const DiskInfo& disk = state->disks[state->selectedDisk];
                props::Show(hwnd, ModuleInstance(), disk,
                            disk.volumes[state->selectedVolume],
                            state->cardColor, state->accentColor);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            // A copy: invoking can re-enumerate and rebuild the button list.
            for (size_t i = 0; i < state->buttons.size(); i++) {
                if (PtInRect(&state->buttons[i].rect, point)) {
                    Button button = state->buttons[i];
                    Invoke(state, button);
                    break;
                }
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            // Scroll proportionally to the wheel delta and the user's own
            // "lines per notch" setting, toward a target the timer eases into -
            // a fixed jump per notch is what made this feel steppy.
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            UINT lines = 3;
            SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &lines, 0);
            if (lines == 0 || lines > 20) {
                lines = 3;
            }
            int step = ui::Scale(static_cast<int>(lines) * 22, state->dpi);

            int viewport = state->contentBottom - state->contentTop;
            int maxScroll = std::max(0, state->contentHeight - viewport);

            state->scrollTarget -= MulDiv(delta, step, WHEEL_DELTA);
            state->scrollTarget = std::clamp(state->scrollTarget, 0, maxScroll);
            if (state->scrollTarget != state->scroll) {
                SetTimer(hwnd, kScrollTimer, 8, nullptr);
            }
            return 0;
        }

        case WM_TIMER:
            if (wParam == kScrollTimer) {
                int distance = state->scrollTarget - state->scroll;
                if (std::abs(distance) <= 1) {
                    state->scroll = state->scrollTarget;
                    KillTimer(hwnd, kScrollTimer);
                } else {
                    // Ease out: a fixed fraction of the remaining distance per
                    // tick, with a floor so the last pixels do not crawl.
                    int move = distance / 4;
                    if (move == 0) {
                        move = distance > 0 ? 1 : -1;
                    }
                    state->scroll += move;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    state->done = true;
                    DestroyWindow(hwnd);
                    break;
                case VK_F5:
                    Refresh(state);
                    break;
                case VK_DOWN:
                    MoveSelection(state, 1);
                    break;
                case VK_UP:
                    MoveSelection(state, -1);
                    break;
                case VK_RETURN:
                    if (Selected(state)) {
                        const DiskInfo& disk = state->disks[state->selectedDisk];
                        props::Show(hwnd, ModuleInstance(), disk,
                                    disk.volumes[state->selectedVolume],
                                    state->cardColor, state->accentColor);
                        InvalidateRect(hwnd, nullptr, FALSE);
                    }
                    break;
            }
            return 0;

        case WM_DPICHANGED: {
            // Storing the new DPI alone leaves every font and icon sized for
            // the old monitor. Rebuild them, then take the rectangle Windows
            // suggests - it accounts for the scale change.
            state->dpi = HIWORD(wParam);

            if (state->fontCaption) DeleteObject(state->fontCaption);
            if (state->fontSection) DeleteObject(state->fontSection);
            if (state->fontName) DeleteObject(state->fontName);
            if (state->fontRow) DeleteObject(state->fontRow);
            if (state->fontSmall) DeleteObject(state->fontSmall);
            if (state->fontButton) DeleteObject(state->fontButton);

            state->fontCaption = MakeFontRegular(state->dpi, 10);
            state->fontSection = MakeFont(state->dpi, 13, true);
            state->fontName = MakeFont(state->dpi, 10, false);
            state->fontRow = MakeFontRegular(state->dpi, 10);
            state->fontSmall = MakeFontRegular(state->dpi, 9);
            state->fontButton = MakeFont(state->dpi, 9, false);

            // Icons were rendered for the old scale; drop them and re-read at
            // the new one.
            g_iconDpi = state->dpi;
            ReleaseDiskIcons(state->disks);
            state->disks = EnumerateDisks();

            if (auto* suggested = reinterpret_cast<const RECT*>(lParam)) {
                SetWindowPos(hwnd, nullptr, suggested->left, suggested->top,
                             suggested->right - suggested->left,
                             suggested->bottom - suggested->top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_DESTROY:
            state->done = true;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool g_classRegistered;

// The class outlives the DLL unless it is taken down explicitly, and its window
// procedure would then point at unloaded code.
// Only clear the flag when the class really went away. UnregisterClassW fails
// while a window of the class still exists, and clearing it regardless would
// leave a registered class whose lpfnWndProc points into an unmapped module.
void UnregisterWindowClassIfNeeded() {
    if (g_classRegistered && UnregisterClassW(kClassName, ModuleInstance())) {
        g_classRegistered = false;
    }
}

bool EnsureClass() {
    if (g_classRegistered) {
        return true;
    }
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = ModuleInstance();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;
    if (RegisterClassExW(&wc)) {
        g_classRegistered = true;
        return true;
    }
    return false;
}

// The live window, for Wh_ModUninit to close on unload.
HWND WindowHandle() {
    return g_state ? g_state->hwnd : nullptr;
}

// Shows the window and runs it to completion. This owns the process: mmc.exe
// exits when the window closes.
bool Run() {
    if (!EnsureClass()) {
        return false;
    }

    State state;
    g_state = &state;

    state.hwnd = CreateWindowExW(
        0, kClassName, L"Disk Management",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, CW_USEDEFAULT,
        CW_USEDEFAULT, 100, 100, nullptr, nullptr, ModuleInstance(), nullptr);
    if (!state.hwnd) {
        g_state = nullptr;
        return false;
    }

    state.dpi = GetDpiForWindow(state.hwnd);
    state.fontCaption = MakeFontRegular(state.dpi, 10);
    state.fontSection = MakeFont(state.dpi, 13, true);
    state.fontName = MakeFont(state.dpi, 10, false);
    state.fontRow = MakeFontRegular(state.dpi, 10);
    state.fontSmall = MakeFontRegular(state.dpi, 9);
    state.fontButton = MakeFont(state.dpi, 9, false);
    state.accentColor = ui::AccentColor();

    if (g_settings.wallpaperTint) {
        state.windowColor = ui::WallpaperTinted(ui::kWindow, 14);
        state.cardColor = ui::WallpaperTinted(ui::kCard, 12);
    }

    BOOL dark = TRUE;
    DwmSetWindowAttribute(state.hwnd, 20, &dark, sizeof(dark));
    DWORD corner = 2;
    DwmSetWindowAttribute(state.hwnd, 33, &corner, sizeof(corner));

    int width = ui::Scale(ui::kWindowWidth, state.dpi);
    int height = ui::Scale(ui::kWindowHeight, state.dpi);
    HMONITOR monitor = MonitorFromWindow(state.hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    RECT work = monitorInfo.rcWork;
    width = std::min<int>(width, work.right - work.left - ui::Scale(80, state.dpi));
    height =
        std::min<int>(height, work.bottom - work.top - ui::Scale(80, state.dpi));
    SetWindowPos(state.hwnd, nullptr,
                 work.left + ((work.right - work.left) - width) / 2,
                 work.top + ((work.bottom - work.top) - height) / 2, width,
                 height, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);

    g_iconDpi = state.dpi;
    state.disks = EnumerateDisks();

    // Open on the boot volume: it is the one the window is usually opened to
    // look at, and it means the action bar is never empty on arrival.
    for (size_t d = 0; d < state.disks.size() && state.selectedDisk < 0; d++) {
        for (size_t v = 0; v < state.disks[d].volumes.size(); v++) {
            if (state.disks[d].volumes[v].isBoot) {
                state.selectedDisk = static_cast<int>(d);
                state.selectedVolume = static_cast<int>(v);
                break;
            }
        }
    }

    ShowWindow(state.hwnd, SW_SHOW);
    SetForegroundWindow(state.hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (state.fontCaption) DeleteObject(state.fontCaption);
    if (state.fontSection) DeleteObject(state.fontSection);
    if (state.fontName) DeleteObject(state.fontName);
    if (state.fontRow) DeleteObject(state.fontRow);
    if (state.fontSmall) DeleteObject(state.fontSmall);
    if (state.fontButton) DeleteObject(state.fontButton);
    g_state = nullptr;
    return true;
}

}  // namespace diskui

// -----------------------------------------------------------------------------
// Mod entry points
// -----------------------------------------------------------------------------

bool LaunchedForDiskManagement() {
    // An escape hatch: set WH_DISKMGMT_CLASSIC=1 to get the original console
    // for one launch, without disabling the mod.
    WCHAR classic[8] = L"";
    if (GetEnvironmentVariableW(L"WH_DISKMGMT_CLASSIC", classic,
                                ARRAYSIZE(classic)) &&
        classic[0] == L'1') {
        return false;
    }

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        return false;
    }
    bool isDiskManagement = false;
    for (int i = 1; i < argc; i++) {
        std::wstring argument = argv[i];
        for (auto& ch : argument) {
            ch = towlower(ch);
        }
        if (argument.find(L"diskmgmt.msc") != std::wstring::npos) {
            isDiskManagement = true;
            break;
        }
    }
    LocalFree(argv);
    return isDiskManagement;
}

void LoadSettings() {
    g_settings.wallpaperTint = Wh_GetIntSetting(L"wallpaperTint") != 0;
    g_settings.showEmptyVolumes = Wh_GetIntSetting(L"showEmptyVolumes") != 0;
    g_settings.verboseLog = Wh_GetIntSetting(L"verboseLog") != 0;
}

// -----------------------------------------------------------------------------
// Taking over the console
//
// MMC is intercepted where it creates its frame window, on its own main thread.
// An earlier version suspended that thread from a worker instead, which is a
// deadlock waiting to happen: the thread is stopped at whatever point it has
// reached, quite possibly holding the loader lock or a CRT lock, and this mod
// then calls CoInitializeEx, GDI+ and the shell for icons. Hooking
// CreateWindowExW is deterministic and needs no suspension at all.
//
// "MMCMainFrame" is MMC's frame window class, confirmed by enumerating the
// windows of a running mmc.exe.
// -----------------------------------------------------------------------------

constexpr PCWSTR kMmcFrameClass = L"MMCMainFrame";

// Set once the takeover has happened, so a second frame window - or a reentrant
// call - does not try to show a second copy of the window.
bool g_tookOver = false;

// Set by Wh_ModUninit. The UI runs on MMC's main thread inside the hook below,
// so unloading has to get that thread out of this module before returning.
volatile bool g_modUnloading = false;

// Signalled by the hook once it has finished with everything in this module.
HANDLE g_uiFinished = nullptr;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, PCWSTR className,
                                 PCWSTR windowName, DWORD style, int x, int y,
                                 int width, int height, HWND parent,
                                 HMENU menu, HINSTANCE instance,
                                 LPVOID param) {
    auto passThrough = [&] {
        return CreateWindowExW_Original(exStyle, className, windowName, style, x,
                                        y, width, height, parent, menu,
                                        instance, param);
    };

    // Class names can be atoms rather than pointers; ignore those.
    if (g_tookOver || g_modUnloading || IS_INTRESOURCE(className) ||
        _wcsicmp(className, kMmcFrameClass) != 0) {
        return passThrough();
    }
    g_tookOver = true;

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool shown = diskui::Run();
    CoUninitialize();

    if (!shown) {
        // Nothing was displayed, so let MMC build its own console rather than
        // leaving the user with no Disk Management at all.
        Wh_Log(L"could not create the window; falling back to the console");
        g_tookOver = false;
        return passThrough();
    }

    // The window has been closed. Tell Wh_ModUninit that this thread is done
    // with the module before doing anything else, then end the process - MMC
    // has nothing left to show.
    if (g_uiFinished) {
        SetEvent(g_uiFinished);
    }
    if (g_modUnloading) {
        // Unloading rather than a user close: return to MMC and let the real
        // console load, instead of killing the process out from under it.
        return passThrough();
    }
    ExitProcess(0);
}

// True when the process already owns a top-level window, which means it was
// running before this mod was loaded. Windhawk runs the mod's callbacks on its
// own engine thread in that case, and taking over then would show a second,
// redundant window over a console the user already has open.
BOOL CALLBACK HasOwnWindow(HWND hwnd, LPARAM param) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        *reinterpret_cast<bool*>(param) = true;
        return FALSE;
    }
    return TRUE;
}

bool ProcessAlreadyRunning() {
    bool found = false;
    EnumWindows(HasOwnWindow, reinterpret_cast<LPARAM>(&found));
    return found;
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!LaunchedForDiskManagement()) {
        // Not this snap-in. Returning FALSE unloads the mod from processes it
        // has nothing to do in; Windhawk reloads it after a settings change,
        // so this is not permanent.
        return FALSE;
    }

    if (ProcessAlreadyRunning()) {
        Wh_Log(L"mmc.exe was already running; leaving it alone");
        return FALSE;
    }

    g_uiFinished = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"could not hook CreateWindowExW");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

// Windhawk unmaps the module as soon as this returns, so no thread may still be
// running mod code by then. The UI runs on MMC's main thread inside the hook
// above, so close its window and wait for that thread to leave.
void Wh_ModUninit() {
    g_modUnloading = true;

    if (g_tookOver && g_uiFinished) {
        if (HWND hwnd = diskui::WindowHandle()) {
            // Async, so this never deadlocks against the UI thread.
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            WaitForSingleObject(g_uiFinished, 10000);
        }
    }
    if (g_uiFinished) {
        CloseHandle(g_uiFinished);
        g_uiFinished = nullptr;
    }

    diskui::UnregisterWindowClassIfNeeded();
    props::UnregisterWindowClassIfNeeded(ModuleInstance());
}
