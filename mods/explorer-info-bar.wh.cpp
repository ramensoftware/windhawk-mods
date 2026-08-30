// ==WindhawkMod==
// @id              explorer-info-bar
// @name            Explorer Info Bar+
// @description     Enhances File Explorer's bottom info bar with drive, content, selection, and single-file details, with customizable styles and colors.
// @version         1.0.0
// @author          digART
// @github          https://github.com/digart11
// @homepage        https://github.com/digart11/explorer-info-bar
// @license         GPL-3.0-only
// @include         explorer.exe
// @compilerOptions -lole32 -lshell32 -luuid -lgdi32 -lcomctl32 -lpropsys -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Info Bar+

A customizable Windhawk mod that enhances the bottom info bar in Windows 11 File Explorer.

Explorer Info Bar+ adds drive, folder, selection, literal file-extension, and basic media/image metadata information while keeping the native Explorer look and adapting to light, dark, and customized themes.

Unlike mods that restore the classic status bar or focus only on metadata, Explorer Info Bar+ combines several information groups in one modern, configurable Windows 11 info bar.

## Preview

![Explorer Info Bar+ preview](https://raw.githubusercontent.com/digart11/explorer-info-bar/main/images/explorer-info-bar-preview.png)

## Features

- Drive free-space information
- Current folder content summary
  - Folder count
  - File count
  - Immediate file size total
- Selection information
  - Selected folders
  - Selected files
  - Selected file size
- Single-file details when available
  - Real file extension
  - Image resolution
  - Video resolution
  - Media duration
- Three display styles
  - Simple
  - Flat panes
  - Soft cards
- Configurable section order
- Individual section visibility controls
- Custom text and panel colors
- Automatic theme-derived colors by default
- Works with File Explorer's native status area rather than replacing Explorer itself

## Compatibility / Why this mod is separate

Explorer Info Bar+ uses the native Windows 11 bottom status area; it does not restore or create a classic `msctls_statusbar32`-style status bar. It combines drive, content, and selection information with configurable ordering, styles, colors, literal extension display, and basic image/media metadata in one native-style bar. This is a different presentation and design from Classic Explorer Status Bar and PreVista Explorer Status Bar.

Do not enable Classic Explorer Status Bar or PreVista Explorer Status Bar at the same time as Explorer Info Bar+. Both try to control the same bottom Explorer area and can conflict or overlap.

Explorer Info Bar+ paints over the native status-row text, including output from Explorer Status Bar Metadata. Do not enable Explorer Status Bar Metadata at the same time because its output will be covered. When Explorer Status Bar Metadata is not used, Explorer Info Bar+'s optional Single File Details can provide file extension, dimensions, and duration when available.

## Examples

Typical information shown by the mod:

```text
Drive D: 150.7GB free
Content: 15 folders / 25 files (77.2MB)
Selected: 2 folders / 4 files (571KB)
```

When one file is selected, additional details can appear:

```text
.jpg (4032×3024)
.jpeg (6000×4000)
.mp4 (1920×1080, 01:23:43)
.mp3 (00:03:47)
.doc
.pdf
```
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: simple
  $name: Style
  $description: Choose how the info bar sections are displayed.
  $options:
    - simple: Simple
    - panes: Flat panes
    - cards: Soft cards

- order: drive-content-selection
  $name: Section order
  $description: Choose the left-to-right order of Drive, Content and Selected.
  $options:
    - drive-content-selection: Drive / Content / Selected
    - drive-selection-content: Drive / Selected / Content
    - content-drive-selection: Content / Drive / Selected
    - content-selection-drive: Content / Selected / Drive
    - selection-drive-content: Selected / Drive / Content
    - selection-content-drive: Selected / Content / Drive

- showDrive: true
  $name: Show Drive
  $description: Show drive free-space information.

- showContent: true
  $name: Show Content
  $description: Show folder and file totals for the current location.

- showSelection: true
  $name: Show Selected
  $description: Show information about the current selection.

- singleFileDetails: false
  $name: Show Single File Details
  $description: Show file extension and details when possible.

- textColor: auto
  $name: Text color
  $description: "Use auto to inherit Explorer, or enter #RRGGBB."

- driveColor: auto
  $name: Drive panel color
  $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."

- contentColor: auto
  $name: Content panel color
  $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."

- selectionColor: auto
  $name: Selected panel color
  $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."

- fileDetailsColor: auto
  $name: File Details panel color
  $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."

- dividerColor: auto
  $name: Divider / border color (Soft Cards style only)
  $description: "Sets the Soft Cards border color. Use auto to derive from Explorer, or enter #RRGGBB."
*/
// ==/WindhawkModSettings==


#include <windows.h>
#include <windhawk_utils.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <commctrl.h>
#include <propkey.h>
#include <propvarutil.h>
#include <objidl.h>
#include <objbase.h>

#include <string>
#include <array>
#include <atomic>
#include <algorithm>
#include <cwctype>
#include <cwchar>
#include <cstdlib>
#include <vector>
#include <utility>

#define CWM_GETISHELLBROWSER (WM_USER + 7)

static constexpr DWORD kInitialRefreshDelayMs = 1000;
static constexpr DWORD kRefreshIntervalMs = 30000;
static constexpr ULONGLONG kShellBrowserRegistrationRetryMs = 2000;
static constexpr ULONGLONG kContentFailedRetryMs = 60000;
static constexpr ULONGLONG kMetadataRetryMs = 5000;
static constexpr ULONGLONG kStatusRowValidationIntervalMs = 500;
static constexpr ULONGLONG kNativeRowBackgroundSampleIntervalMs = 2000;

static std::atomic<bool> g_unloading{false};

static SRWLOCK g_subclassLock = SRWLOCK_INIT;

enum class InfoBarStyle
{
    Simple,
    Panes,
    Cards
};

enum class InfoBarSection
{
    Drive,
    Content,
    Selection
};

enum CacheChangeFlags : unsigned
{
    CacheChangeNone = 0,
    CacheChangeDrive = 1 << 0,
    CacheChangeContent = 1 << 1,
    CacheChangeSelection = 1 << 2,
    CacheChangeFileDetails = 1 << 3
};

struct InfoBarLayoutGeometry
{
    HWND hwnd = nullptr;
    std::array<int, 3> sectionLeft{-1, -1, -1};
    int fileDetailsLeft = -1;
    int usableRight = -1;
    InfoBarStyle style = InfoBarStyle::Simple;
    std::array<InfoBarSection, 3> sectionOrder{};
    bool showDrive = false;
    bool showContent = false;
    bool showSelection = false;
    bool singleFileDetails = false;
};

struct TrackedDirectUiState
{
    HWND hwnd = nullptr;
    ULONGLONG selectionGeneration = 1;
    bool hasLayout = false;
    InfoBarLayoutGeometry layout;
    COLORREF automaticRowBackground = CLR_INVALID;
    bool hasSampledNativeRowBackground = false;
    ULONGLONG lastNativeRowBackgroundSampleTick = 0;
    UINT dpi = 96;
    HFONT infoBarFont = nullptr;
    HWND shellTab = nullptr;
    DWORD shellBrowserCookie = 0;
    ULONGLONG lastShellBrowserRegistrationRetryTick = 0;
    HWND validatedDefView = nullptr;
    bool hasValidatedStatusRow = false;
    RECT validatedStatusRow{};
    ULONGLONG lastStatusRowValidationTick = 0;
};

static std::vector<TrackedDirectUiState> g_trackedWindows;
static std::vector<HWND> g_installingDirectUiWindows;

struct ColorOverride
{
    bool enabled = false;
    COLORREF value = CLR_INVALID;
};

struct ModSettings
{
    InfoBarStyle style = InfoBarStyle::Simple;

    std::array<InfoBarSection, 3> sectionOrder
    {
        InfoBarSection::Drive,
        InfoBarSection::Content,
        InfoBarSection::Selection
    };

    ColorOverride textColor;
    ColorOverride driveColor;
    ColorOverride contentColor;
    ColorOverride selectionColor;
    ColorOverride fileDetailsColor;
    ColorOverride dividerColor;

    bool showDrive = true;
    bool showContent = true;
    bool showSelection = true;
    bool singleFileDetails = false;
};

static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static ModSettings g_settings;

static UINT g_refreshDirectUiMessage = 0;

static LRESULT CALLBACK DirectUiSubclassProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR
);

static void TryAttachExplorerDirectUiWindow(HWND hwnd);

// ============================================================
// State
// ============================================================

static DWORD g_pid = 0;

static HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_workerWakeEvent = nullptr;
static HANDLE g_selectionWinEventThread = nullptr;
static HANDLE g_selectionWinEventThreadReady = nullptr;
static HANDLE g_selectionWinEventStopEvent = nullptr;
static HWINEVENTHOOK g_selectionWinEventHook = nullptr;
static HWINEVENTHOOK g_windowCreateWinEventHook = nullptr;
static constexpr ULONGLONG kSelectionWinEventDebounceMs = 200;
static constexpr ULONGLONG kSelectionWinEventMaxLatencyMs = 800;
static ULONGLONG g_selectionWinEventBurstStartTick = 0;
static UINT_PTR g_selectionWinEventWakeTimer = 0;

static CRITICAL_SECTION g_cacheLock;

static bool IsWorkerStopRequested()
{
    return
        g_unloading.load(std::memory_order_acquire) ||
        (
            g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
        );
}

struct ContentRefreshCache
{
    bool valid = false;
    std::wstring folderIdentity;
    int itemCount = -1;
    int files = 0;
    int folders = 0;
    ULONGLONG directFileBytes = 0;
    ULONGLONG lastFullScanTick = 0;
    bool scanFailed = false;
    std::wstring failedFolderIdentity;
    int failedItemCount = -1;
    ULONGLONG failedScanTick = 0;
};

struct SingleFileMetadataCache
{
    bool valid = false;
    std::wstring path;
    std::wstring details;
    ULONGLONG retryAfterTick = 0;
};

struct DriveRefreshCache
{
    std::wstring path;
    ULONGLONG freeBytes = 0;
    ULONGLONG totalBytes = 0;
    wchar_t driveLetter = L'?';
    ULONGLONG lastRefreshTick = 0;
};

struct SingleSelectionRefreshCache
{
    std::wstring identity;
    ULONGLONG lastFilesystemRefreshTick = 0;
};

struct WindowDataCache
{
    HWND hwnd = nullptr;
    int selected = -1;
    int selectedFiles = 0;
    int selectedFolders = 0;
    ULONGLONG selectedBytes = 0;
    ULONGLONG selectionGeneration = 0;
    std::wstring contentGroup;
    std::wstring selectionGroup;
    std::wstring driveGroup;
    std::wstring fileDetailsGroup;
    ContentRefreshCache contentRefresh;
    SingleFileMetadataCache metadata;
    DriveRefreshCache driveRefresh;
    SingleSelectionRefreshCache singleSelectionRefresh;
    ULONGLONG lastWorkerRefreshTick = 0;
    ULONGLONG lastPaintWakeTick = 0;
};

static std::vector<WindowDataCache> g_windowDataCaches;


static std::wstring GetStringSetting(
    PCWSTR name
)
{
    PCWSTR raw =
        Wh_GetStringSetting(
            name
        );

    std::wstring value =
        raw ? raw : L"";

    if (raw)
        Wh_FreeStringSetting(raw);

    return value;
}

static ColorOverride ParseColorOverride(
    const std::wstring& value
)
{
    ColorOverride result;

    if (
        value.empty() ||
        _wcsicmp(
            value.c_str(),
            L"auto"
        ) == 0
    )
    {
        return result;
    }

    if (
        value.length() != 7 ||
        value[0] != L'#'
    )
    {
        return result;
    }

    for (size_t i = 1; i < value.length(); i++)
    {
        const wchar_t ch = value[i];

        if (!(
                (ch >= L'0' && ch <= L'9') ||
                (ch >= L'a' && ch <= L'f') ||
                (ch >= L'A' && ch <= L'F')
            ))
        {
            return result;
        }
    }

    wchar_t* end = nullptr;

    unsigned long rgb =
        wcstoul(
            value.c_str() + 1,
            &end,
            16
        );

    if (
        !end ||
        *end != L'\0' ||
        rgb > 0xFFFFFF
    )
    {
        return result;
    }

    result.enabled = true;
    result.value =
        RGB(
            (rgb >> 16) & 0xFF,
            (rgb >> 8) & 0xFF,
            rgb & 0xFF
        );

    return result;
}

static std::array<InfoBarSection, 3> ParseSectionOrder(
    const std::wstring& order
)
{
    if (order == L"drive-selection-content")
    {
        return
        {
            InfoBarSection::Drive,
            InfoBarSection::Selection,
            InfoBarSection::Content
        };
    }

    if (order == L"content-drive-selection")
    {
        return
        {
            InfoBarSection::Content,
            InfoBarSection::Drive,
            InfoBarSection::Selection
        };
    }

    if (order == L"content-selection-drive")
    {
        return
        {
            InfoBarSection::Content,
            InfoBarSection::Selection,
            InfoBarSection::Drive
        };
    }

    if (order == L"selection-drive-content")
    {
        return
        {
            InfoBarSection::Selection,
            InfoBarSection::Drive,
            InfoBarSection::Content
        };
    }

    if (order == L"selection-content-drive")
    {
        return
        {
            InfoBarSection::Selection,
            InfoBarSection::Content,
            InfoBarSection::Drive
        };
    }

    return
    {
        InfoBarSection::Drive,
        InfoBarSection::Content,
        InfoBarSection::Selection
    };
}

static void LoadSettings()
{
    ModSettings settings;

    const std::wstring style =
        GetStringSetting(
            L"style"
        );

    if (style == L"panes")
        settings.style = InfoBarStyle::Panes;
    else if (style == L"cards")
        settings.style = InfoBarStyle::Cards;

    settings.sectionOrder =
        ParseSectionOrder(
            GetStringSetting(
                L"order"
            )
        );

    settings.textColor =
        ParseColorOverride(
            GetStringSetting(
                L"textColor"
            )
        );

    settings.driveColor =
        ParseColorOverride(
            GetStringSetting(
                L"driveColor"
            )
        );

    settings.contentColor =
        ParseColorOverride(
            GetStringSetting(
                L"contentColor"
            )
        );

    settings.selectionColor =
        ParseColorOverride(
            GetStringSetting(
                L"selectionColor"
            )
        );

    settings.fileDetailsColor =
        ParseColorOverride(
            GetStringSetting(
                L"fileDetailsColor"
            )
        );

    settings.dividerColor =
        ParseColorOverride(
            GetStringSetting(
                L"dividerColor"
            )
        );

    settings.showDrive =
        Wh_GetIntSetting(
            L"showDrive"
        ) != 0;

    settings.showContent =
        Wh_GetIntSetting(
            L"showContent"
        ) != 0;

    settings.showSelection =
        Wh_GetIntSetting(
            L"showSelection"
        ) != 0;

    settings.singleFileDetails =
        Wh_GetIntSetting(
            L"singleFileDetails"
        ) != 0;

    AcquireSRWLockExclusive(
        &g_settingsLock
    );

    g_settings =
        settings;

    ReleaseSRWLockExclusive(
        &g_settingsLock
    );
}

static ModSettings GetSettingsSnapshot()
{
    AcquireSRWLockShared(
        &g_settingsLock
    );

    ModSettings settings =
        g_settings;

    ReleaseSRWLockShared(
        &g_settingsLock
    );

    return settings;
}

// ============================================================
// Helpers
// ============================================================

enum class ByteFormat
{
    Adaptive,
    OneDecimal
};

static std::wstring FormatBytes(
    ULONGLONG bytes,
    ByteFormat format = ByteFormat::Adaptive
)
{
    wchar_t buf[64] = {};

    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;

    if (bytes >= static_cast<ULONGLONG>(TB))
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            format == ByteFormat::OneDecimal
                ? L"%.1fTB"
                : L"%.2fTB",
            static_cast<double>(bytes) / TB
        );
    }
    else if (bytes >= static_cast<ULONGLONG>(GB))
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            format == ByteFormat::OneDecimal
                ? L"%.1fGB"
                : L"%.2fGB",
            static_cast<double>(bytes) / GB
        );
    }
    else if (bytes >= static_cast<ULONGLONG>(MB))
    {
        const double mb =
            static_cast<double>(bytes) / MB;

        if (format == ByteFormat::OneDecimal)
        {
            swprintf(
                buf,
                ARRAYSIZE(buf),
                L"%.1fMB",
                mb
            );
        }
        else if (mb >= 100.0)
        {
            swprintf(
                buf,
                ARRAYSIZE(buf),
                L"%.0fMB",
                mb
            );
        }
        else
        {
            swprintf(
                buf,
                ARRAYSIZE(buf),
                L"%.2fMB",
                mb
            );
        }
    }
    else if (bytes >= static_cast<ULONGLONG>(KB))
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            L"%.0fKB",
            static_cast<double>(bytes) / KB
        );
    }
    else
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            L"%lluB",
            bytes
        );
    }

    return buf;
}


static std::wstring GetLiteralExtension(
    const std::wstring& path
)
{
    const size_t slash =
        path.find_last_of(
            L"\\/"
        );

    const size_t dot =
        path.find_last_of(
            L'.'
        );

    if (
        dot == std::wstring::npos ||
        dot + 1 >= path.length() ||
        (
            slash != std::wstring::npos &&
            dot < slash
        )
    )
    {
        return L"";
    }

    // Preserve the actual extension, including the dot and original case.
    return path.substr(
        dot
    );
}

enum class PropertyReadResult
{
    Value,
    Missing,
    Failed
};

static PropertyReadResult ReadUInt32Property(
    IShellItem2* item,
    REFPROPERTYKEY key,
    UINT32* value
)
{
    if (!item || !value)
        return PropertyReadResult::Failed;

    PROPVARIANT property;
    PropVariantInit(&property);

    HRESULT hr =
        item->GetProperty(
            key,
            &property
        );

    if (FAILED(hr))
    {
        PropVariantClear(&property);
        return PropertyReadResult::Failed;
    }

    if (
        property.vt == VT_EMPTY ||
        property.vt == VT_NULL
    )
    {
        PropVariantClear(&property);
        return PropertyReadResult::Missing;
    }

    ULONG convertedValue = 0;
    hr =
        PropVariantToUInt32(
            property,
            &convertedValue
        );

    PropVariantClear(&property);

    if (FAILED(hr))
        return PropertyReadResult::Missing;

    *value =
        static_cast<UINT32>(
            convertedValue
        );

    return PropertyReadResult::Value;
}

static PropertyReadResult ReadUInt64Property(
    IShellItem2* item,
    REFPROPERTYKEY key,
    ULONGLONG* value
)
{
    if (!item || !value)
        return PropertyReadResult::Failed;

    PROPVARIANT property;
    PropVariantInit(&property);

    HRESULT hr =
        item->GetProperty(
            key,
            &property
        );

    if (FAILED(hr))
    {
        PropVariantClear(&property);
        return PropertyReadResult::Failed;
    }

    if (
        property.vt == VT_EMPTY ||
        property.vt == VT_NULL
    )
    {
        PropVariantClear(&property);
        return PropertyReadResult::Missing;
    }

    hr =
        PropVariantToUInt64(
            property,
            value
        );

    PropVariantClear(&property);

    return SUCCEEDED(hr)
        ? PropertyReadResult::Value
        : PropertyReadResult::Missing;
}

static std::wstring FormatMediaDuration(
    ULONGLONG duration100ns
)
{
    if (duration100ns == 0)
        return L"";

    const ULONGLONG totalSeconds =
        duration100ns /
        10000000ULL;

    const ULONGLONG hours =
        totalSeconds /
        3600ULL;

    const ULONGLONG minutes =
        (
            totalSeconds %
            3600ULL
        ) /
        60ULL;

    const ULONGLONG seconds =
        totalSeconds %
        60ULL;

    wchar_t buffer[64] = {};

    swprintf(
        buffer,
        ARRAYSIZE(buffer),
        L"%02llu:%02llu:%02llu",
        hours,
        minutes,
        seconds
    );

    return buffer;
}

static std::wstring BuildSingleFileDetails(
    const std::wstring& path,
    bool* transientFailure
)
{
    if (transientFailure)
        *transientFailure = false;

    if (path.empty())
        return L"";

    const std::wstring extension =
        GetLiteralExtension(path);

    std::wstring result =
        extension.empty()
            ? L"no extension"
            : extension;

    if (IsWorkerStopRequested())
        return result;

    IShellItem* localItem = nullptr;

    if (
        FAILED(
            SHCreateItemFromParsingName(
                path.c_str(),
                nullptr,
                IID_PPV_ARGS(&localItem)
            )
        ) ||
        !localItem
    )
    {
        if (transientFailure)
            *transientFailure = true;

        return result;
    }

    IShellItem2* item2 = nullptr;

    const HRESULT item2Hr =
        localItem->QueryInterface(
            IID_PPV_ARGS(&item2)
        );

    localItem->Release();

    if (FAILED(item2Hr) || !item2)
    {
        if (transientFailure)
            *transientFailure = true;

        return result;
    }

    if (IsWorkerStopRequested())
    {
        item2->Release();
        return result;
    }

    UINT32 imageWidth = 0;
    UINT32 imageHeight = 0;
    UINT32 videoWidth = 0;
    UINT32 videoHeight = 0;
    ULONGLONG duration = 0;

    const PropertyReadResult imageWidthResult =
        ReadUInt32Property(
            item2,
            PKEY_Image_HorizontalSize,
            &imageWidth
        );

    const PropertyReadResult imageHeightResult =
        ReadUInt32Property(
            item2,
            PKEY_Image_VerticalSize,
            &imageHeight
        );

    const PropertyReadResult videoWidthResult =
        ReadUInt32Property(
            item2,
            PKEY_Video_FrameWidth,
            &videoWidth
        );

    const PropertyReadResult videoHeightResult =
        ReadUInt32Property(
            item2,
            PKEY_Video_FrameHeight,
            &videoHeight
        );

    const PropertyReadResult durationResult =
        ReadUInt64Property(
            item2,
            PKEY_Media_Duration,
            &duration
        );

    item2->Release();

    if (transientFailure)
    {
        *transientFailure =
            imageWidthResult == PropertyReadResult::Failed ||
            imageHeightResult == PropertyReadResult::Failed ||
            videoWidthResult == PropertyReadResult::Failed ||
            videoHeightResult == PropertyReadResult::Failed ||
            durationResult == PropertyReadResult::Failed;
    }

    UINT32 width = 0;
    UINT32 height = 0;

    if (
        videoWidthResult == PropertyReadResult::Value &&
        videoHeightResult == PropertyReadResult::Value &&
        videoWidth > 0 &&
        videoHeight > 0
    )
    {
        width = videoWidth;
        height = videoHeight;
    }
    else if (
        imageWidthResult == PropertyReadResult::Value &&
        imageHeightResult == PropertyReadResult::Value &&
        imageWidth > 0 &&
        imageHeight > 0
    )
    {
        width = imageWidth;
        height = imageHeight;
    }

    const std::wstring durationText =
        (
            durationResult == PropertyReadResult::Value &&
            duration > 0
        )
            ? FormatMediaDuration(duration)
            : L"";

    const bool hasDimensions =
        width > 0 &&
        height > 0;

    if (!hasDimensions && durationText.empty())
        return result;

    result += L" (";

    if (hasDimensions)
    {
        wchar_t dimensions[64] = {};

        swprintf(
            dimensions,
            ARRAYSIZE(dimensions),
            L"%u\x00D7%u",
            width,
            height
        );

        result += dimensions;
    }

    if (!durationText.empty())
    {
        if (hasDimensions)
            result += L", ";

        result += durationText;
    }

    result += L")";
    return result;
}

static bool GetFilesystemInfo(
    const wchar_t* path,
    bool* isDirectory,
    ULONGLONG* fileSize
)
{
    if (!path || !*path)
        return false;

    WIN32_FILE_ATTRIBUTE_DATA data{};

    if (!GetFileAttributesExW(
            path,
            GetFileExInfoStandard,
            &data))
    {
        return false;
    }

    bool directory =
        (data.dwFileAttributes &
         FILE_ATTRIBUTE_DIRECTORY) != 0;

    if (isDirectory)
        *isDirectory = directory;

    if (fileSize)
    {
        if (directory)
        {
            *fileSize = 0;
        }
        else
        {
            *fileSize =
                (static_cast<ULONGLONG>(
                    data.nFileSizeHigh
                ) << 32) |
                data.nFileSizeLow;
        }
    }

    return true;
}

// ============================================================
// Explorer window / COM ownership
// ============================================================

static HWND FindAncestorByClass(
    HWND hwnd,
    PCWSTR className
)
{
    for (HWND current = hwnd; current; current = GetParent(current))
    {
        wchar_t cls[128] = {};

        if (
            GetClassNameW(
                current,
                cls,
                ARRAYSIZE(cls)
            ) &&
            wcscmp(cls, className) == 0
        )
        {
            return current;
        }
    }

    return nullptr;
}

static IShellBrowser* TryGetShellBrowserOnOwnerThread(
    HWND shellTab
)
{
    if (!shellTab)
        return nullptr;

    const DWORD ownerThread =
        GetWindowThreadProcessId(
            shellTab,
            nullptr
        );

    if (
        !ownerThread ||
        ownerThread != GetCurrentThreadId()
    )
    {
        return nullptr;
    }

    auto* browser =
        reinterpret_cast<IShellBrowser*>(
            SendMessageW(
                shellTab,
                CWM_GETISHELLBROWSER,
                0,
                0
            )
        );

    if (browser)
        browser->AddRef();

    return browser;
}

static HRESULT CreateGlobalInterfaceTable(
    IGlobalInterfaceTable** table
)
{
    if (!table)
        return E_POINTER;

    *table = nullptr;

    return CoCreateInstance(
        CLSID_StdGlobalInterfaceTable,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(table)
    );
}

static void RevokeShellBrowserCookie(
    DWORD cookie
)
{
    if (!cookie)
        return;

    // Most revocations run on an Explorer UI apartment, but teardown can also
    // reach this helper after a window has already disappeared. Ensure COM is
    // available on that fallback thread without changing an existing apartment.
    const HRESULT initHr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    const bool shouldUninitialize =
        SUCCEEDED(initHr);

    if (
        FAILED(initHr) &&
        initHr != RPC_E_CHANGED_MODE
    )
    {
        Wh_Log(
            L"Shell browser revoke COM setup failed cookie=%lu HRESULT=0x%08X",
            cookie,
            static_cast<unsigned>(initHr)
        );
        return;
    }

    IGlobalInterfaceTable* table = nullptr;
    const HRESULT createHr =
        CreateGlobalInterfaceTable(&table);

    if (FAILED(createHr) || !table)
    {
        Wh_Log(
            L"Global Interface Table revoke setup failed cookie=%lu HRESULT=0x%08X",
            cookie,
            static_cast<unsigned>(createHr)
        );

        if (shouldUninitialize)
            CoUninitialize();

        return;
    }

    const HRESULT revokeHr =
        table->RevokeInterfaceFromGlobal(cookie);

    table->Release();

    if (shouldUninitialize)
        CoUninitialize();

    if (FAILED(revokeHr))
    {
        Wh_Log(
            L"Shell browser revoke failed cookie=%lu HRESULT=0x%08X",
            cookie,
            static_cast<unsigned>(revokeHr)
        );
    }
}

static bool EnsureShellBrowserRegistration(
    HWND hwnd
)
{
    if (
        !hwnd ||
        g_unloading.load(std::memory_order_acquire)
    )
    {
        return false;
    }

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    const bool alreadyRegistered =
        existing != g_trackedWindows.end() &&
        existing->shellBrowserCookie != 0;

    ReleaseSRWLockShared(&g_subclassLock);

    if (alreadyRegistered)
        return true;

    const DWORD ownerThread =
        GetWindowThreadProcessId(
            hwnd,
            nullptr
        );

    if (
        !ownerThread ||
        ownerThread != GetCurrentThreadId()
    )
    {
        return false;
    }

    HWND shellTab =
        FindAncestorByClass(
            hwnd,
            L"ShellTabWindowClass"
        );

    if (!shellTab)
        return false;

    IShellBrowser* browser =
        TryGetShellBrowserOnOwnerThread(shellTab);

    if (!browser)
        return false;

    IGlobalInterfaceTable* table = nullptr;
    HRESULT hr =
        CreateGlobalInterfaceTable(&table);

    DWORD newCookie = 0;

    if (SUCCEEDED(hr) && table)
    {
        hr =
            table->RegisterInterfaceInGlobal(
                browser,
                IID_IShellBrowser,
                &newCookie
            );
    }

    browser->Release();

    if (table)
        table->Release();

    if (FAILED(hr) || !newCookie)
    {
        Wh_Log(
            L"Shell browser registration failed hwnd=%p HRESULT=0x%08X",
            hwnd,
            static_cast<unsigned>(hr)
        );
        return false;
    }

    DWORD oldCookie = 0;
    bool stored = false;

    AcquireSRWLockExclusive(&g_subclassLock);

    auto tracked =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        tracked != g_trackedWindows.end() &&
        !g_unloading.load(std::memory_order_acquire)
    )
    {
        oldCookie = tracked->shellBrowserCookie;
        tracked->shellTab = shellTab;
        tracked->shellBrowserCookie = newCookie;
        stored = true;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);

    if (oldCookie)
        RevokeShellBrowserCookie(oldCookie);

    if (!stored)
    {
        RevokeShellBrowserCookie(newCookie);
        return false;
    }

    if (g_workerWakeEvent)
        SetEvent(g_workerWakeEvent);

    return true;
}

// ============================================================
// Redraw
// ============================================================

static int ScaleForDpi(
    UINT dpi,
    int value
)
{
    return MulDiv(
        value,
        dpi ? static_cast<int>(dpi) : 96,
        96
    );
}

static int ScaleForWindow(
    HWND hwnd,
    int value
)
{
    const UINT dpi =
        hwnd ? GetDpiForWindow(hwnd) : 96;

    return ScaleForDpi(dpi, value);
}

static HFONT CreateInfoBarFont(UINT dpi)
{
    return CreateFontW(
        -ScaleForDpi(dpi, 12),
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH,
        L"Segoe UI"
    );
}

static void RefreshTrackedDpiAndFont(HWND hwnd)
{
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 96;

    if (!dpi)
        dpi = 96;

    bool needsFont = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        existing != g_trackedWindows.end() &&
        (
            existing->dpi != dpi ||
            !existing->infoBarFont
        )
    )
    {
        needsFont = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    if (!needsFont)
        return;

    HFONT newFont = CreateInfoBarFont(dpi);
    HFONT oldFont = nullptr;

    AcquireSRWLockExclusive(&g_subclassLock);

    auto tracked =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        tracked != g_trackedWindows.end() &&
        (
            tracked->dpi != dpi ||
            !tracked->infoBarFont
        )
    )
    {
        oldFont = tracked->infoBarFont;
        tracked->dpi = dpi;
        tracked->infoBarFont = newFont;
        newFont = nullptr;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);

    if (oldFont)
        DeleteObject(oldFont);

    if (newFont)
        DeleteObject(newFont);
}

static bool GetPaintResources(
    HWND hwnd,
    UINT* dpi,
    HFONT* font
)
{
    if (!dpi || !font)
        return false;

    bool found = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        *dpi = existing->dpi ? existing->dpi : 96;
        *font = existing->infoBarFont;
        found = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);
    return found;
}

static HWND FindShellDefViewDescendant(HWND directUi)
{
    if (!directUi || !IsWindow(directUi))
        return nullptr;

    // Explorer nests SHELLDLL_DefView below an immediate DirectUI child.
    // Match only the structural class relationship used by Explorer; window
    // text is neither queried nor involved in target selection.
    for (
        HWND child = FindWindowExW(directUi, nullptr, nullptr, nullptr);
        child;
        child = FindWindowExW(directUi, child, nullptr, nullptr)
    )
    {
        HWND defView =
            FindWindowExW(
                child,
                nullptr,
                L"SHELLDLL_DefView",
                nullptr
            );

        if (defView)
            return defView;
    }

    return nullptr;
}

static void StoreValidatedStatusRow(
    HWND hwnd,
    HWND defView,
    const RECT* row
)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->validatedDefView = row ? defView : nullptr;
        existing->hasValidatedStatusRow = row != nullptr;
        existing->validatedStatusRow = row ? *row : RECT{};
        existing->lastStatusRowValidationTick = GetTickCount64();
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static bool RefreshValidatedStatusRow(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd))
        return false;

    UINT dpi = 96;
    HFONT unusedFont = nullptr;

    if (!GetPaintResources(hwnd, &dpi, &unusedFont))
    {
        dpi = GetDpiForWindow(hwnd);

        if (!dpi)
            dpi = 96;
    }

    const HWND defView = FindShellDefViewDescendant(hwnd);

    DWORD directUiPid = 0;
    DWORD defViewPid = 0;
    const DWORD directUiThread =
        GetWindowThreadProcessId(hwnd, &directUiPid);
    const DWORD defViewThread =
        defView
            ? GetWindowThreadProcessId(defView, &defViewPid)
            : 0;

    RECT client{};
    RECT mappedDefView{};

    bool valid =
        defView &&
        IsWindow(defView) &&
        IsChild(hwnd, defView) &&
        directUiThread &&
        defViewThread == directUiThread &&
        defViewPid == directUiPid &&
        GetClientRect(hwnd, &client) &&
        GetWindowRect(defView, &mappedDefView);

    if (valid)
    {
        SetLastError(ERROR_SUCCESS);

        const int mapped =
            MapWindowPoints(
                nullptr,
                hwnd,
                reinterpret_cast<POINT*>(&mappedDefView),
                2
            );

        if (!mapped && GetLastError() != ERROR_SUCCESS)
            valid = false;
    }

    const LONG tolerance = ScaleForDpi(dpi, 2);

    if (
        valid &&
        (
            client.right <= client.left ||
            client.bottom <= client.top ||
            mappedDefView.right <= mappedDefView.left ||
            mappedDefView.bottom <= mappedDefView.top ||
            mappedDefView.left < client.left - tolerance ||
            mappedDefView.right > client.right + tolerance ||
            mappedDefView.top < client.top - tolerance ||
            mappedDefView.bottom <= client.top ||
            mappedDefView.bottom > client.bottom + tolerance
        )
    )
    {
        valid = false;
    }

    RECT row{};

    if (valid)
    {
        row = client;
        row.top = std::min(mappedDefView.bottom, client.bottom);

        // A zero/sliver gap is normal when Explorer's status bar is hidden.
        // Require a small DPI-scaled minimum before treating it as a row.
        if (
            row.bottom - row.top <
                static_cast<LONG>(ScaleForDpi(dpi, 8))
        )
        {
            valid = false;
        }
    }

    StoreValidatedStatusRow(
        hwnd,
        valid ? defView : nullptr,
        valid ? &row : nullptr
    );

    return valid;
}

static bool IsStatusRowRevalidationDue(HWND hwnd)
{
    ULONGLONG lastValidationTick = 0;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        lastValidationTick =
            existing->lastStatusRowValidationTick;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    return
        !lastValidationTick ||
        GetTickCount64() - lastValidationTick >=
            kStatusRowValidationIntervalMs;
}

static bool GetValidatedStatusRow(
    HWND hwnd,
    RECT* row
)
{
    if (!row)
        return false;

    HWND defView = nullptr;
    RECT cachedRow{};
    bool found = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        existing != g_trackedWindows.end() &&
        existing->hasValidatedStatusRow
    )
    {
        defView = existing->validatedDefView;
        cachedRow = existing->validatedStatusRow;
        found = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    RECT client{};

    if (
        !found ||
        !hwnd ||
        !IsWindow(hwnd) ||
        !defView ||
        !IsWindow(defView) ||
        !IsChild(hwnd, defView) ||
        !GetClientRect(hwnd, &client) ||
        cachedRow.left != client.left ||
        cachedRow.right != client.right ||
        cachedRow.bottom != client.bottom ||
        cachedRow.top < client.top ||
        cachedRow.top >= cachedRow.bottom
    )
    {
        if (found)
            StoreValidatedStatusRow(hwnd, nullptr, nullptr);

        return false;
    }

    *row = cachedRow;
    return true;
}

static bool InvalidateInfoBarWindow(
    HWND hwnd,
    int partialLeft = -1,
    int partialRight = -1
)
{
    if (!hwnd || !IsWindow(hwnd))
        return false;

    RECT row{};

    if (!GetValidatedStatusRow(hwnd, &row))
        return false;

    const int clientRight =
        row.right;

    if (
        partialLeft >= 0 &&
        partialRight > partialLeft
    )
    {
        row.left =
            std::min(
                std::max(partialLeft, 0),
                clientRight
            );

        row.right =
            std::min(
                partialRight,
                clientRight
            );

        if (row.right <= row.left)
            return false;
    }

    // Ask DirectUI to repaint only the bottom row.
    // Our subclass paints our info after DirectUI finishes its own WM_PAINT.
    return InvalidateRect(
               hwnd,
               &row,
               FALSE
           ) != FALSE;
}

static size_t GetSectionGeometryIndex(
    InfoBarSection section
)
{
    if (section == InfoBarSection::Drive)
        return 0;

    if (section == InfoBarSection::Content)
        return 1;

    return 2;
}

struct AutomaticThemeSnapshot
{
    COLORREF rowBackground = CLR_INVALID;
    bool hasSampledNativeRowBackground = false;
    ULONGLONG lastNativeRowBackgroundSampleTick = 0;
};

static void StoreLayoutGeometry(
    const InfoBarLayoutGeometry& geometry
)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == geometry.hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->layout = geometry;
        existing->hasLayout = true;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static DWORD UntrackDirectUiWindowLocked(
    HWND hwnd,
    HFONT* infoBarFont = nullptr
)
{
    DWORD shellBrowserCookie = 0;

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        shellBrowserCookie =
            existing->shellBrowserCookie;

        if (infoBarFont)
            *infoBarFont = existing->infoBarFont;

        g_trackedWindows.erase(existing);
    }

    return shellBrowserCookie;
}

static DWORD GetShellBrowserCookieSnapshot(
    HWND hwnd
)
{
    DWORD cookie = 0;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
        cookie = existing->shellBrowserCookie;

    ReleaseSRWLockShared(&g_subclassLock);
    return cookie;
}

static AutomaticThemeSnapshot GetAutomaticThemeSnapshot(
    HWND hwnd
)
{
    AutomaticThemeSnapshot result;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        result.rowBackground =
            existing->automaticRowBackground;

        result.hasSampledNativeRowBackground =
            existing->hasSampledNativeRowBackground;

        result.lastNativeRowBackgroundSampleTick =
            existing->lastNativeRowBackgroundSampleTick;
    }

    ReleaseSRWLockShared(&g_subclassLock);
    return result;
}

static void UpdateAutomaticRowBackground(
    HWND hwnd,
    COLORREF rowBackground,
    bool sampledNativeRowBackground
)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->automaticRowBackground = rowBackground;
        existing->hasSampledNativeRowBackground =
            sampledNativeRowBackground;

        if (sampledNativeRowBackground)
        {
            existing->lastNativeRowBackgroundSampleTick =
                GetTickCount64();
        }
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static void InvalidateAutomaticTheme(HWND hwnd)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->automaticRowBackground = CLR_INVALID;
        existing->hasSampledNativeRowBackground = false;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static void RefreshInfoBarWindow(
    HWND hwnd,
    unsigned changes = CacheChangeNone
)
{
    if (!hwnd || !IsWindow(hwnd))
        return;

    if (changes == CacheChangeNone)
    {
        InvalidateInfoBarWindow(hwnd);
        return;
    }

    InfoBarLayoutGeometry geometry;
    bool hasGeometry = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto tracked =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    const bool found =
        tracked != g_trackedWindows.end();

    if (found)
    {
        geometry = tracked->layout;
        hasGeometry = tracked->hasLayout;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    if (!found)
        return;

    const ModSettings settings =
        GetSettingsSnapshot();

    unsigned visibleChanges = changes;

    if (!settings.showDrive)
        visibleChanges &= ~CacheChangeDrive;

    if (!settings.showContent)
        visibleChanges &= ~CacheChangeContent;

    if (!settings.showSelection)
        visibleChanges &= ~CacheChangeSelection;

    if (!settings.singleFileDetails)
        visibleChanges &= ~CacheChangeFileDetails;

    if (visibleChanges == CacheChangeNone)
        return;

    RECT client{};

    const bool currentClientAvailable =
        GetClientRect(
            hwnd,
            &client
        );

    const int currentControlSafeRight =
        currentClientAvailable
            ? std::max(
                0,
                static_cast<int>(client.right) -
                    ScaleForWindow(hwnd, 64)
            )
            : -1;

    const int currentUsableRight =
        currentClientAvailable
            ? std::max(
                ScaleForWindow(hwnd, 6),
                std::min(
                    currentControlSafeRight,
                    client.right > ScaleForWindow(hwnd, 220)
                        ? static_cast<int>(
                            client.right - ScaleForWindow(hwnd, 220)
                        )
                        : currentControlSafeRight
                )
            )
            : -1;

    const bool layoutMatchesSettings =
        hasGeometry &&
        geometry.style == settings.style &&
        geometry.sectionOrder == settings.sectionOrder &&
        geometry.showDrive == settings.showDrive &&
        geometry.showContent == settings.showContent &&
        geometry.showSelection == settings.showSelection &&
        geometry.singleFileDetails == settings.singleFileDetails &&
        geometry.usableRight == currentUsableRight;

    if (!layoutMatchesSettings)
    {
        InvalidateInfoBarWindow(hwnd);
        return;
    }

    int partialLeft = -1;

    auto IncludeLeft =
        [&](int left)
        {
            if (left < 0)
                return false;

            if (
                partialLeft < 0 ||
                left < partialLeft
            )
            {
                partialLeft = left;
            }

            return true;
        };

    bool geometryComplete = true;

    if (visibleChanges & CacheChangeDrive)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.sectionLeft[
                    GetSectionGeometryIndex(
                        InfoBarSection::Drive
                    )
                ]
            );
    }

    if (visibleChanges & CacheChangeContent)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.sectionLeft[
                    GetSectionGeometryIndex(
                        InfoBarSection::Content
                    )
                ]
            );
    }

    if (visibleChanges & CacheChangeSelection)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.sectionLeft[
                    GetSectionGeometryIndex(
                        InfoBarSection::Selection
                    )
                ]
            );
    }

    if (visibleChanges & CacheChangeFileDetails)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.fileDetailsLeft
            );
    }

    if (
        !geometryComplete ||
        partialLeft < 0 ||
        geometry.usableRight <= partialLeft
    )
    {
        InvalidateInfoBarWindow(hwnd);
        return;
    }

    InvalidateInfoBarWindow(
        hwnd,
        partialLeft,
        geometry.usableRight
    );
}

static void RefreshInfoBars(
    unsigned changes = CacheChangeNone
)
{
    std::vector<HWND> windows;

    AcquireSRWLockShared(&g_subclassLock);

    try
    {
        windows.reserve(g_trackedWindows.size());

        for (const TrackedDirectUiState& state : g_trackedWindows)
            windows.push_back(state.hwnd);
    }
    catch (...)
    {
        ReleaseSRWLockShared(&g_subclassLock);
        Wh_Log(L"DirectUI refresh snapshot failed");
        return;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    for (HWND hwnd : windows)
        RefreshInfoBarWindow(hwnd, changes);
}

// ============================================================
// Cache
// ============================================================

static WindowDataCache* FindWindowDataCacheLocked(
    HWND hwnd
)
{
    const auto existing =
        std::find_if(
            g_windowDataCaches.begin(),
            g_windowDataCaches.end(),
            [&](const WindowDataCache& value)
            {
                return value.hwnd == hwnd;
            }
        );

    return existing != g_windowDataCaches.end()
        ? &*existing
        : nullptr;
}


static void WakeWorkerFromPaint(HWND hwnd)
{
    if (!hwnd || !g_workerWakeEvent)
        return;

    const ULONGLONG now = GetTickCount64();
    bool shouldWake = false;

    EnterCriticalSection(&g_cacheLock);

    if (WindowDataCache* cache = FindWindowDataCacheLocked(hwnd))
    {
        const bool dataIsStale =
            !cache->lastWorkerRefreshTick ||
            now - cache->lastWorkerRefreshTick >=
                kRefreshIntervalMs;

        const bool paintWakeIsDue =
            !cache->lastPaintWakeTick ||
            now - cache->lastPaintWakeTick >=
                kRefreshIntervalMs;

        if (dataIsStale && paintWakeIsDue)
        {
            cache->lastPaintWakeTick = now;
            shouldWake = true;
        }
    }

    LeaveCriticalSection(&g_cacheLock);

    if (!shouldWake)
        return;

    SetEvent(g_workerWakeEvent);
}

static bool AdvanceSelectionGenerationForWinEvent(HWND hwnd)
{
    if (!hwnd)
        return false;

    // Resolve the event's tab before taking g_subclassLock. GetParent and
    // GetClassNameW can call into the window manager and must remain outside
    // the tracked-state critical section.
    const HWND eventShellTab =
        FindAncestorByClass(
            hwnd,
            L"ShellTabWindowClass"
        );

    if (eventShellTab)
    {
        bool matched = false;

        AcquireSRWLockExclusive(&g_subclassLock);

        for (TrackedDirectUiState& state : g_trackedWindows)
        {
            if (state.shellTab == eventShellTab)
            {
                state.selectionGeneration++;
                matched = true;
            }
        }

        ReleaseSRWLockExclusive(&g_subclassLock);

        if (matched)
            return true;
    }

    const HWND eventRoot = GetAncestor(hwnd, GA_ROOT);

    if (!eventRoot)
        return false;

    wchar_t rootClassName[64]{};

    if (
        !GetClassNameW(
            eventRoot,
            rootClassName,
            ARRAYSIZE(rootClassName)
        ) ||
        wcscmp(rootClassName, L"CabinetWClass") != 0
    )
    {
        return false;
    }

    struct TrackedTabSnapshot
    {
        HWND hwnd;
        HWND shellTab;
    };

    // If the event HWND couldn't be tied directly to a registered shell tab,
    // use only a unique visible DirectUI target under the same Explorer root.
    // This preserves a safe recovery path without dirtying sibling tabs.
    std::vector<TrackedTabSnapshot> trackedTabs;
    bool snapshotFailed = false;

    AcquireSRWLockShared(&g_subclassLock);

    try
    {
        trackedTabs.reserve(g_trackedWindows.size());

        for (const TrackedDirectUiState& state : g_trackedWindows)
        {
            trackedTabs.push_back({
                state.hwnd,
                state.shellTab
            });
        }
    }
    catch (...)
    {
        snapshotFailed = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    if (snapshotFailed)
        return false;

    HWND fallbackHwnd = nullptr;
    HWND fallbackShellTab = nullptr;

    for (const TrackedTabSnapshot& tracked : trackedTabs)
    {
        if (
            IsWindowVisible(tracked.hwnd) &&
            GetAncestor(tracked.hwnd, GA_ROOT) == eventRoot
        )
        {
            if (fallbackHwnd)
                return false;

            fallbackHwnd = tracked.hwnd;
            fallbackShellTab = tracked.shellTab;
        }
    }

    if (!fallbackHwnd)
        return false;

    bool matched = false;

    AcquireSRWLockExclusive(&g_subclassLock);

    for (TrackedDirectUiState& state : g_trackedWindows)
    {
        if (
            state.hwnd == fallbackHwnd &&
            state.shellTab == fallbackShellTab
        )
        {
            state.selectionGeneration++;
            matched = true;
            break;
        }
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
    return matched;
}

static void CALLBACK SelectionWinEventWakeTimerProc(
    HWND,
    UINT,
    UINT_PTR timerId,
    DWORD
)
{
    KillTimer(nullptr, timerId);

    if (g_selectionWinEventWakeTimer == timerId)
        g_selectionWinEventWakeTimer = 0;

    if (g_unloading.load(std::memory_order_acquire))
        return;

    g_selectionWinEventBurstStartTick = 0;

    if (g_workerWakeEvent)
        SetEvent(g_workerWakeEvent);
}

static void WakeWorkerFromSelectionWinEvent()
{
    if (!g_workerWakeEvent)
        return;

    const ULONGLONG now = GetTickCount64();

    if (!g_selectionWinEventBurstStartTick)
        g_selectionWinEventBurstStartTick = now;

    if (g_selectionWinEventWakeTimer)
    {
        KillTimer(nullptr, g_selectionWinEventWakeTimer);
        g_selectionWinEventWakeTimer = 0;
    }

    const ULONGLONG burstElapsed =
        now - g_selectionWinEventBurstStartTick;

    if (burstElapsed >= kSelectionWinEventMaxLatencyMs)
    {
        g_selectionWinEventBurstStartTick = 0;
        SetEvent(g_workerWakeEvent);
        return;
    }

    const ULONGLONG remainingLatency =
        kSelectionWinEventMaxLatencyMs - burstElapsed;

    const UINT delay =
        static_cast<UINT>(
            std::min(
                kSelectionWinEventDebounceMs,
                remainingLatency
            )
        );

    g_selectionWinEventWakeTimer =
        SetTimer(
            nullptr,
            0,
            delay,
            SelectionWinEventWakeTimerProc
        );

    if (!g_selectionWinEventWakeTimer)
    {
        // A failed debounce timer must not leave the final selection stale.
        g_selectionWinEventBurstStartTick = 0;
        SetEvent(g_workerWakeEvent);
    }
}

static void CALLBACK SelectionWinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG,
    LONG,
    DWORD,
    DWORD
)
{
    if (
        // A one-item A -> B transition can be focus-only, with no selection
        // count change and no EVENT_OBJECT_SELECTION-family notification.
        event < EVENT_OBJECT_FOCUS ||
        event > EVENT_OBJECT_SELECTIONWITHIN ||
        g_unloading.load(std::memory_order_acquire) ||
        !AdvanceSelectionGenerationForWinEvent(hwnd)
    )
    {
        return;
    }

    WakeWorkerFromSelectionWinEvent();
}

static void CALLBACK ExplorerObjectCreateWinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG,
    DWORD,
    DWORD
)
{
    if (
        event != EVENT_OBJECT_CREATE ||
        idObject != OBJID_WINDOW ||
        !hwnd ||
        g_unloading.load(std::memory_order_acquire)
    )
    {
        return;
    }

    TryAttachExplorerDirectUiWindow(hwnd);
}

static DWORD WINAPI SelectionWinEventThreadProc(
    LPVOID
)
{
    // WINEVENT_OUTOFCONTEXT callbacks are delivered on the thread that
    // installed the hook, so this thread owns the hook and its message pump.
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_selectionWinEventHook =
        SetWinEventHook(
            EVENT_OBJECT_FOCUS,
            EVENT_OBJECT_SELECTIONWITHIN,
            nullptr,
            SelectionWinEventProc,
            g_pid,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    if (!g_selectionWinEventHook)
    {
        Wh_Log(
            L"selection WinEvent hook failed error=%lu",
            GetLastError()
        );
    }

    g_windowCreateWinEventHook =
        SetWinEventHook(
            EVENT_OBJECT_CREATE,
            EVENT_OBJECT_CREATE,
            nullptr,
            ExplorerObjectCreateWinEventProc,
            g_pid,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    if (!g_windowCreateWinEventHook)
    {
        Wh_Log(
            L"Explorer window-create WinEvent hook failed error=%lu",
            GetLastError()
        );
    }

    SetEvent(g_selectionWinEventThreadReady);

    if (
        !g_selectionWinEventHook &&
        !g_windowCreateWinEventHook
    )
    {
        return 1;
    }

    bool quit = false;

    while (!quit)
    {
        const DWORD waitResult =
            MsgWaitForMultipleObjectsEx(
                1,
                &g_selectionWinEventStopEvent,
                INFINITE,
                QS_ALLINPUT,
                MWMO_INPUTAVAILABLE
            );

        if (waitResult == WAIT_OBJECT_0)
        {
            break;
        }
        else if (waitResult == WAIT_OBJECT_0 + 1)
        {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    quit = true;
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        else
        {
            Wh_Log(
                L"WinEvent helper message wait failed result=%lu error=%lu",
                waitResult,
                GetLastError()
            );

            break;
        }
    }

    if (g_selectionWinEventWakeTimer)
    {
        KillTimer(nullptr, g_selectionWinEventWakeTimer);
        g_selectionWinEventWakeTimer = 0;
    }

    // Both hooks are installed, pumped and removed by this same owning thread.
    if (g_windowCreateWinEventHook)
    {
        UnhookWinEvent(g_windowCreateWinEventHook);
        g_windowCreateWinEventHook = nullptr;
    }

    if (g_selectionWinEventHook)
    {
        UnhookWinEvent(g_selectionWinEventHook);
        g_selectionWinEventHook = nullptr;
    }

    return 0;
}

static void StopSelectionWinEventThread()
{
    if (g_selectionWinEventThread)
    {
        if (g_selectionWinEventStopEvent)
            SetEvent(g_selectionWinEventStopEvent);

        WaitForSingleObject(g_selectionWinEventThread, INFINITE);
        CloseHandle(g_selectionWinEventThread);
        g_selectionWinEventThread = nullptr;
    }
}

static bool EnsureWindowDataCache(
    HWND hwnd
)
{
    EnterCriticalSection(&g_cacheLock);

    if (FindWindowDataCacheLocked(hwnd))
    {
        LeaveCriticalSection(&g_cacheLock);
        return true;
    }

    try
    {
        WindowDataCache cache;
        cache.hwnd = hwnd;
        g_windowDataCaches.push_back(std::move(cache));
    }
    catch (...)
    {
        LeaveCriticalSection(&g_cacheLock);
        Wh_Log(L"Window data cache allocation failed hwnd=%p", hwnd);
        return false;
    }

    LeaveCriticalSection(&g_cacheLock);
    return true;
}

static void EraseWindowDataCache(
    HWND hwnd
)
{
    EnterCriticalSection(&g_cacheLock);

    g_windowDataCaches.erase(
        std::remove_if(
            g_windowDataCaches.begin(),
            g_windowDataCaches.end(),
            [&](const WindowDataCache& value)
            {
                return value.hwnd == hwnd;
            }
        ),
        g_windowDataCaches.end()
    );

    LeaveCriticalSection(&g_cacheLock);
}

static WindowDataCache GetWindowDataCacheSnapshot(
    HWND hwnd
)
{
    WindowDataCache result;
    result.hwnd = hwnd;

    EnterCriticalSection(&g_cacheLock);

    if (WindowDataCache* existing = FindWindowDataCacheLocked(hwnd))
        result = *existing;

    LeaveCriticalSection(&g_cacheLock);
    return result;
}

static void GetCachedGroups(
    HWND hwnd,
    std::wstring& contentGroup,
    std::wstring& selectionGroup,
    std::wstring& driveGroup,
    std::wstring& fileDetailsGroup,
    int& selected
)
{
    EnterCriticalSection(&g_cacheLock);

    if (WindowDataCache* cache = FindWindowDataCacheLocked(hwnd))
    {
        contentGroup = cache->contentGroup;
        selectionGroup = cache->selectionGroup;
        driveGroup = cache->driveGroup;
        fileDetailsGroup = cache->fileDetailsGroup;
        selected = cache->selected;
    }
    else
    {
        contentGroup.clear();
        selectionGroup.clear();
        driveGroup.clear();
        fileDetailsGroup.clear();
        selected = 0;
    }

    LeaveCriticalSection(&g_cacheLock);
}

static unsigned UpdateCache(
    HWND hwnd,
    int files,
    int folders,
    int selected,
    int selectedFiles,
    int selectedFolders,
    ULONGLONG directFileBytes,
    ULONGLONG selectedBytes,
    ULONGLONG freeBytes,
    ULONGLONG driveTotalBytes,
    wchar_t driveLetter,
    const std::wstring& contentOverrideText,
    const std::wstring& selectionOverrideText,
    const std::wstring& fileDetailsText,
    ULONGLONG selectionGeneration,
    const ContentRefreshCache& contentRefreshCache,
    const SingleFileMetadataCache& metadataCache,
    const DriveRefreshCache& driveRefreshCache,
    const SingleSelectionRefreshCache& singleSelectionRefreshCache
)
{
    std::wstring driveText;

    if (
        driveLetter != L'?' &&
        driveTotalBytes > 0
    )
    {
        wchar_t driveBuf[256] = {};

        swprintf(
            driveBuf,
            ARRAYSIZE(driveBuf),
            L"Drive %c: %s free",
            driveLetter,
            FormatBytes(
                freeBytes,
                ByteFormat::OneDecimal
            ).c_str()
        );

        driveText = driveBuf;
    }

    std::wstring contentText;

    if (!contentOverrideText.empty())
    {
        contentText = contentOverrideText;
    }
    else
    {
        wchar_t contentBuf[256] = {};

        swprintf(
            contentBuf,
            ARRAYSIZE(contentBuf),
            L"Content: %d folder%s / %d file%s (%s)",
            folders,
            folders == 1 ? L"" : L"s",
            files,
            files == 1 ? L"" : L"s",
            FormatBytes(directFileBytes).c_str()
        );

        contentText = contentBuf;
    }

    std::wstring selectionText;

    if (selected > 0)
    {
        if (!selectionOverrideText.empty())
        {
            selectionText = selectionOverrideText;
        }
        else
        {
            wchar_t selectedBuf[256] = {};

            if (
                selectedFolders > 0 &&
                selectedFiles > 0
            )
            {
                swprintf(
                    selectedBuf,
                    ARRAYSIZE(selectedBuf),
                    L"Selected: %d folder%s / %d file%s (%s)",
                    selectedFolders,
                    selectedFolders == 1 ? L"" : L"s",
                    selectedFiles,
                    selectedFiles == 1 ? L"" : L"s",
                    FormatBytes(selectedBytes).c_str()
                );
            }
            else if (selectedFolders > 0)
            {
                swprintf(
                    selectedBuf,
                    ARRAYSIZE(selectedBuf),
                    L"Selected: %d folder%s",
                    selectedFolders,
                    selectedFolders == 1 ? L"" : L"s"
                );
            }
            else
            {
                swprintf(
                    selectedBuf,
                    ARRAYSIZE(selectedBuf),
                    L"Selected: %d file%s (%s)",
                    selectedFiles,
                    selectedFiles == 1 ? L"" : L"s",
                    FormatBytes(selectedBytes).c_str()
                );
            }

            selectionText = selectedBuf;
        }
    }

    unsigned changes = CacheChangeNone;

    EnterCriticalSection(&g_cacheLock);

    WindowDataCache* cache =
        FindWindowDataCacheLocked(hwnd);

    if (!cache)
    {
        // Cache creation belongs to the tracked-window lifecycle. Don't
        // recreate a cache here if this worker iteration raced with window
        // destruction and cleanup.
        LeaveCriticalSection(&g_cacheLock);
        return CacheChangeNone;
    }

    if (contentText != cache->contentGroup)
    {
        changes |= CacheChangeContent;
        cache->contentGroup = contentText;
    }

    if (
        selectionText != cache->selectionGroup ||
        selected != cache->selected
    )
    {
        changes |= CacheChangeSelection;
        cache->selected = selected;
        cache->selectionGroup = selectionText;
    }

    if (driveText != cache->driveGroup)
    {
        changes |= CacheChangeDrive;
        cache->driveGroup = driveText;
    }

    if (fileDetailsText != cache->fileDetailsGroup)
    {
        changes |= CacheChangeFileDetails;
        cache->fileDetailsGroup = fileDetailsText;
    }

    cache->selectedFiles = selectedFiles;
    cache->selectedFolders = selectedFolders;
    cache->selectedBytes = selectedBytes;
    cache->selectionGeneration = selectionGeneration;
    cache->contentRefresh = contentRefreshCache;
    cache->metadata = metadataCache;
    cache->driveRefresh = driveRefreshCache;
    cache->singleSelectionRefresh = singleSelectionRefreshCache;
    cache->lastWorkerRefreshTick = GetTickCount64();

    LeaveCriticalSection(&g_cacheLock);
    return changes;
}

static bool ScanFilesystemDirectory(
    const std::wstring& directoryPath,
    int* files,
    int* folders,
    ULONGLONG* directFileBytes
)
{
    if (
        directoryPath.empty() ||
        !files ||
        !folders ||
        !directFileBytes
    )
    {
        return false;
    }

    if (IsWorkerStopRequested())
        return false;

    SHELLSTATE state{};
    SHGetSetSettings(
        &state,
        SSF_SHOWALLOBJECTS | SSF_SHOWSUPERHIDDEN,
        FALSE
    );

    std::wstring searchPath = directoryPath;

    if (
        searchPath.back() != L'\\' &&
        searchPath.back() != L'/'
    )
    {
        searchPath += L'\\';
    }

    searchPath += L'*';

    WIN32_FIND_DATAW findData{};

    if (IsWorkerStopRequested())
        return false;

    HANDLE findHandle =
        FindFirstFileExW(
            searchPath.c_str(),
            FindExInfoBasic,
            &findData,
            FindExSearchNameMatch,
            nullptr,
            FIND_FIRST_EX_LARGE_FETCH
        );

    if (findHandle == INVALID_HANDLE_VALUE)
    {
        const DWORD error = GetLastError();

        if (
            error == ERROR_FILE_NOT_FOUND ||
            error == ERROR_NO_MORE_FILES
        )
        {
            *files = 0;
            *folders = 0;
            *directFileBytes = 0;
            return true;
        }

        return false;
    }

    int scannedFiles = 0;
    int scannedFolders = 0;
    ULONGLONG scannedBytes = 0;
    bool complete = true;

    while (true)
    {
        if (
            g_unloading.load(std::memory_order_acquire) ||
            (
                g_stopEvent &&
                WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
            )
        )
        {
            complete = false;
            break;
        }

        if (
            wcscmp(findData.cFileName, L".") != 0 &&
            wcscmp(findData.cFileName, L"..") != 0
        )
        {
            const DWORD attributes =
                findData.dwFileAttributes;

            const bool hidden =
                (attributes & FILE_ATTRIBUTE_HIDDEN) != 0;

            const bool system =
                (attributes & FILE_ATTRIBUTE_SYSTEM) != 0;

            if (
                hidden &&
                (
                    !state.fShowAllObjects ||
                    (
                        system &&
                        !state.fShowSuperHidden
                    )
                )
            )
            {
                goto nextEntry;
            }

            if (
                attributes &
                FILE_ATTRIBUTE_DIRECTORY
            )
            {
                scannedFolders++;
            }
            else
            {
                scannedFiles++;

                scannedBytes +=
                    (static_cast<ULONGLONG>(
                        findData.nFileSizeHigh
                    ) << 32) |
                    findData.nFileSizeLow;
            }
        }

nextEntry:
        if (FindNextFileW(findHandle, &findData))
            continue;

        if (GetLastError() != ERROR_NO_MORE_FILES)
            complete = false;

        break;
    }

    FindClose(findHandle);

    if (!complete)
        return false;

    *files = scannedFiles;
    *folders = scannedFolders;
    *directFileBytes = scannedBytes;
    return true;
}

static void ClearSingleFileMetadataCache(
    SingleFileMetadataCache* cache
)
{
    if (cache)
        *cache = SingleFileMetadataCache{};
}

static std::wstring GetSingleFileDetailsCached(
    SingleFileMetadataCache* cache,
    const std::wstring& path
)
{
    if (!cache)
        return L"";

    const ULONGLONG now = GetTickCount64();

    if (path == cache->path)
    {
        if (cache->valid)
            return cache->details;

        if (cache->retryAfterTick > now)
            return cache->details;
    }

    bool transientFailure = false;

    std::wstring details =
        BuildSingleFileDetails(
            path,
            &transientFailure
        );

    cache->path = path;
    cache->details = details;
    cache->valid = !transientFailure;
    cache->retryAfterTick =
        transientFailure
            ? now + kMetadataRetryMs
            : 0;

    return details;
}

// ============================================================
// Read Explorer state
// ============================================================

static unsigned ReadCurrentView(
    HWND hwnd,
    IShellBrowser* browser,
    ULONGLONG selectionGeneration
)
{
    if (!hwnd || !browser)
        return CacheChangeNone;

    const ModSettings settings =
        GetSettingsSnapshot();

    WindowDataCache state =
        GetWindowDataCacheSnapshot(hwnd);

    ContentRefreshCache contentCache =
        state.contentRefresh;

    SingleFileMetadataCache metadataCache =
        state.metadata;

    DriveRefreshCache driveCache =
        state.driveRefresh;

    SingleSelectionRefreshCache singleSelectionCache =
        state.singleSelectionRefresh;

    IShellView* shellView = nullptr;

    HRESULT hr =
        browser->QueryActiveShellView(
            &shellView
        );

    if (FAILED(hr) || !shellView)
        return CacheChangeNone;

    IFolderView2* folderView = nullptr;

    hr =
        shellView->QueryInterface(
            IID_PPV_ARGS(&folderView)
        );

    if (FAILED(hr) || !folderView)
    {
        shellView->Release();
        return CacheChangeNone;
    }

    std::wstring currentPath;
    std::wstring folderIdentity;

    IShellItem* folderItem = nullptr;

    hr =
        folderView->GetFolder(
            IID_PPV_ARGS(&folderItem)
        );

    if (SUCCEEDED(hr) && folderItem)
    {
        PWSTR path = nullptr;

        if (
            SUCCEEDED(
                folderItem->GetDisplayName(
                    SIGDN_FILESYSPATH,
                    &path
                )
            ) &&
            path
        )
        {
            currentPath = path;
            CoTaskMemFree(path);
        }

        PWSTR parsingName = nullptr;

        if (
            SUCCEEDED(
                folderItem->GetDisplayName(
                    SIGDN_DESKTOPABSOLUTEPARSING,
                    &parsingName
                )
            ) &&
            parsingName
        )
        {
            folderIdentity = parsingName;
            CoTaskMemFree(parsingName);
        }

        folderItem->Release();
    }

    if (folderIdentity.empty())
        folderIdentity = currentPath;

    const ULONGLONG now = GetTickCount64();

    ULONGLONG freeBytes = 0;
    ULONGLONG driveTotalBytes = 0;
    wchar_t driveLetter = L'?';

    if (
        settings.showDrive &&
        currentPath.length() >= 2 &&
        currentPath[1] == L':'
    )
    {
        const bool drivePathChanged =
            driveCache.path != currentPath;

        const bool driveRefreshDue =
            !driveCache.lastRefreshTick ||
            now - driveCache.lastRefreshTick >=
                kRefreshIntervalMs;

        if (drivePathChanged || driveRefreshDue)
        {
            ULARGE_INTEGER freeAvailable{};
            ULARGE_INTEGER totalBytes{};

            driveCache.path = currentPath;
            driveCache.freeBytes = 0;
            driveCache.totalBytes = 0;
            driveCache.driveLetter =
                static_cast<wchar_t>(
                    towupper(currentPath[0])
                );
            driveCache.lastRefreshTick = now;

            if (
                GetDiskFreeSpaceExW(
                    currentPath.c_str(),
                    &freeAvailable,
                    &totalBytes,
                    nullptr
                )
            )
            {
                driveCache.freeBytes = freeAvailable.QuadPart;
                driveCache.totalBytes = totalBytes.QuadPart;
            }
        }

        freeBytes = driveCache.freeBytes;
        driveTotalBytes = driveCache.totalBytes;
        driveLetter = driveCache.driveLetter;
    }
    else if (settings.showDrive)
    {
        driveCache = DriveRefreshCache{};
    }

    int total = -1;
    bool itemCountAvailable = false;
    std::wstring contentOverrideText;

    if (settings.showContent)
    {
        hr =
            folderView->ItemCount(
                SVGIO_ALLVIEW,
                &total
            );

        itemCountAvailable = SUCCEEDED(hr);
    }

    if (
        settings.showContent &&
        itemCountAvailable &&
        currentPath.empty()
    )
    {
        wchar_t virtualContent[128] = {};

        swprintf(
            virtualContent,
            ARRAYSIZE(virtualContent),
            L"Content: %d item%s",
            total,
            total == 1 ? L"" : L"s"
        );

        contentOverrideText = virtualContent;
    }
    else if (
        settings.showContent &&
        itemCountAvailable &&
        !currentPath.empty()
    )
    {
        const bool sameFolder =
            contentCache.valid &&
            !folderIdentity.empty() &&
            folderIdentity == contentCache.folderIdentity;

        const bool itemCountChanged =
            sameFolder &&
            total != contentCache.itemCount;

        const bool periodicFullScanDue =
            sameFolder &&
            (
                !contentCache.lastFullScanTick ||
                now - contentCache.lastFullScanTick >=
                    kRefreshIntervalMs
            );

        const bool sameFailedFolder =
            contentCache.scanFailed &&
            !folderIdentity.empty() &&
            folderIdentity == contentCache.failedFolderIdentity;

        const bool failedItemCountChanged =
            sameFailedFolder &&
            total != contentCache.failedItemCount;

        const bool failedRetryDue =
            sameFailedFolder &&
            now - contentCache.failedScanTick >=
                kContentFailedRetryMs;

        if (
            (
                !sameFolder ||
                itemCountChanged ||
                periodicFullScanDue
            ) &&
            (
                !sameFailedFolder ||
                failedItemCountChanged ||
                failedRetryDue
            )
        )
        {
            int scannedFiles = 0;
            int scannedFolders = 0;
            ULONGLONG scannedBytes = 0;

            if (
                ScanFilesystemDirectory(
                    currentPath,
                    &scannedFiles,
                    &scannedFolders,
                    &scannedBytes
                )
            )
            {
                contentCache.valid = true;
                contentCache.folderIdentity = folderIdentity;
                contentCache.itemCount = total;
                contentCache.files = scannedFiles;
                contentCache.folders = scannedFolders;
                contentCache.directFileBytes = scannedBytes;
                contentCache.lastFullScanTick = now;
                contentCache.scanFailed = false;
                contentCache.failedFolderIdentity.clear();
                contentCache.failedItemCount = -1;
                contentCache.failedScanTick = 0;
            }
            else
            {
                contentCache.valid = false;
                contentCache.scanFailed = true;
                contentCache.failedFolderIdentity = folderIdentity;
                contentCache.failedItemCount = total;
                contentCache.failedScanTick = now;
            }
        }
    }

    const bool useCachedContent =
        settings.showContent &&
        contentCache.valid &&
        !folderIdentity.empty() &&
        folderIdentity == contentCache.folderIdentity;

    const bool useFailedContentFallback =
        settings.showContent &&
        itemCountAvailable &&
        contentCache.scanFailed &&
        !folderIdentity.empty() &&
        folderIdentity == contentCache.failedFolderIdentity &&
        total == contentCache.failedItemCount;

    if (
        useFailedContentFallback &&
        contentOverrideText.empty()
    )
    {
        wchar_t failedContent[128] = {};

        swprintf(
            failedContent,
            ARRAYSIZE(failedContent),
            L"Content: %d item%s",
            total,
            total == 1 ? L"" : L"s"
        );

        contentOverrideText = failedContent;
    }

    if (
        settings.showContent &&
        !currentPath.empty() &&
        !useCachedContent &&
        contentOverrideText.empty()
    )
    {
        contentOverrideText = L"Content: Loading...";
    }

    const int files =
        useCachedContent
            ? contentCache.files
            : 0;

    const int folders =
        useCachedContent
            ? contentCache.folders
            : 0;

    const ULONGLONG directFileBytes =
        useCachedContent
            ? contentCache.directFileBytes
            : 0;

    int selected = 0;
    int selectedFiles = state.selectedFiles;
    int selectedFolders = state.selectedFolders;
    ULONGLONG selectedBytes = state.selectedBytes;
    std::wstring selectionOverrideText;
    std::wstring singleFileDetails = state.fileDetailsGroup;
    bool keepSingleFileMetadataCache = !state.fileDetailsGroup.empty();
    // Always obtain the selection count. Even when the Selected and
    // File Details sections are hidden, the painter uses the count to avoid
    // learning Explorer's temporary selected-row tint as the normal theme.
    IShellItemArray* selection = nullptr;

    hr =
        folderView->GetSelection(
            FALSE,
            &selection
        );

    if (SUCCEEDED(hr) && selection)
    {
        DWORD selectionCount = 0;

        if (
            SUCCEEDED(
                selection->GetCount(
                    &selectionCount
                )
            )
        )
        {
            selected =
                static_cast<int>(selectionCount);

            // Avoid hundreds or thousands of cross-apartment shell calls on
            // large selections. Keep the exact count, but only enumerate
            // individual selected items up to a conservative cap.
            constexpr DWORD kMaxDetailedSelectionItems = 256;

            const bool folderChanged =
                contentCache.folderIdentity != state.contentRefresh.folderIdentity;

            // A one-item selection needs an identity-sensitive fallback:
            // arrowing to another item keeps the count at one. Resolve that
            // item's identity on worker passes, but throttle filesystem reads
            // when the identity and event generation are unchanged.
            const bool singleSelectionFallback =
                selectionCount == 1 &&
                (
                    settings.showSelection ||
                    settings.singleFileDetails
                );

            const bool selectionDirty =
                selectionGeneration != state.selectionGeneration ||
                selected != state.selected ||
                folderChanged;

            const bool enumerateSelection =
                (
                    selectionDirty ||
                    singleSelectionFallback
                ) &&
                selectionCount <= kMaxDetailedSelectionItems &&
                (
                    settings.showSelection ||
                    (
                        settings.singleFileDetails &&
                        selectionCount == 1
                    )
                );

            if (selectionDirty)
            {
                selectedFiles = 0;
                selectedFolders = 0;
                selectedBytes = 0;
                singleFileDetails.clear();
                keepSingleFileMetadataCache = false;
                singleSelectionCache = SingleSelectionRefreshCache{};
            }

            if (enumerateSelection)
            {
                for (DWORD i = 0; i < selectionCount; i++)
                {
                    if (
                        g_unloading.load(std::memory_order_acquire) ||
                        (
                            g_stopEvent &&
                            WaitForSingleObject(
                                g_stopEvent,
                                0
                            ) == WAIT_OBJECT_0
                        )
                    )
                    {
                        break;
                    }

                    IShellItem* item = nullptr;

                    if (
                        FAILED(
                            selection->GetItemAt(
                                i,
                                &item
                            )
                        ) ||
                        !item
                    )
                    {
                        continue;
                    }

                    std::wstring filesystemPath;
                    PWSTR rawPath = nullptr;

                    if (
                        SUCCEEDED(
                            item->GetDisplayName(
                                SIGDN_FILESYSPATH,
                                &rawPath
                            )
                        ) &&
                        rawPath
                    )
                    {
                        filesystemPath = rawPath;
                        CoTaskMemFree(rawPath);
                    }

                    std::wstring itemIdentity =
                        filesystemPath;

                    if (
                        selectionCount == 1 &&
                        itemIdentity.empty()
                    )
                    {
                        PWSTR parsingName = nullptr;

                        if (
                            SUCCEEDED(
                                item->GetDisplayName(
                                    SIGDN_DESKTOPABSOLUTEPARSING,
                                    &parsingName
                                )
                            ) &&
                            parsingName
                        )
                        {
                            itemIdentity = parsingName;
                            CoTaskMemFree(parsingName);
                        }
                    }

                    bool refreshFilesystem =
                        selectionDirty;

                    if (selectionCount == 1)
                    {
                        const bool identityChanged =
                            itemIdentity != singleSelectionCache.identity;

                        const bool filesystemFallbackDue =
                            !singleSelectionCache.lastFilesystemRefreshTick ||
                            now - singleSelectionCache.lastFilesystemRefreshTick >=
                                kRefreshIntervalMs;

                        refreshFilesystem =
                            selectionDirty ||
                            identityChanged ||
                            filesystemFallbackDue;

                        if (
                            !selectionDirty &&
                            refreshFilesystem
                        )
                        {
                            selectedFiles = 0;
                            selectedFolders = 0;
                            selectedBytes = 0;
                            singleFileDetails.clear();
                            keepSingleFileMetadataCache = false;
                        }

                        singleSelectionCache.identity =
                            itemIdentity;
                    }

                    if (
                        refreshFilesystem &&
                        !filesystemPath.empty()
                    )
                    {
                        bool directory = false;
                        ULONGLONG size = 0;

                        if (selectionCount == 1)
                        {
                            singleSelectionCache.lastFilesystemRefreshTick =
                                now;
                        }

                        if (
                            GetFilesystemInfo(
                                filesystemPath.c_str(),
                                &directory,
                                &size
                            )
                        )
                        {
                            if (directory)
                            {
                                selectedFolders++;
                            }
                            else
                            {
                                selectedFiles++;
                                selectedBytes += size;

                                if (
                                    selectionCount == 1 &&
                                    settings.singleFileDetails
                                )
                                {
                                    keepSingleFileMetadataCache = true;
                                    singleFileDetails =
                                        GetSingleFileDetailsCached(
                                            &metadataCache,
                                            filesystemPath
                                        );
                                }
                            }
                        }
                    }

                    item->Release();
                }
            }
        }

        selection->Release();
    }

    if (!keepSingleFileMetadataCache)
        ClearSingleFileMetadataCache(&metadataCache);

    if (
        selected > 0 &&
        selectedFiles + selectedFolders != selected
    )
    {
        wchar_t virtualSelection[128] = {};

        swprintf(
            virtualSelection,
            ARRAYSIZE(virtualSelection),
            L"Selected: %d item%s",
            selected,
            selected == 1 ? L"" : L"s"
        );

        selectionOverrideText = virtualSelection;
    }

    if (
        g_unloading.load(std::memory_order_acquire) ||
        (
            g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
        )
    )
    {
        folderView->Release();
        shellView->Release();
        return CacheChangeNone;
    }

    const unsigned changes =
        UpdateCache(
            hwnd,
            files,
            folders,
            selected,
            selectedFiles,
            selectedFolders,
            directFileBytes,
            selectedBytes,
            freeBytes,
            driveTotalBytes,
            driveLetter,
            contentOverrideText,
            selectionOverrideText,
            singleFileDetails,
            selectionGeneration,
            contentCache,
            metadataCache,
            driveCache,
            singleSelectionCache
        );

    folderView->Release();
    shellView->Release();
    return changes;
}

// ============================================================
// Worker
// ============================================================

static bool WaitForWorkerStop(
    DWORD timeoutMs
)
{
    HANDLE handles[2] =
    {
        g_stopEvent,
        g_workerWakeEvent
    };

    const DWORD result =
        WaitForMultipleObjects(
            ARRAYSIZE(handles),
            handles,
            FALSE,
            timeoutMs
        );

    return result == WAIT_OBJECT_0;
}

static DWORD WINAPI WorkerThreadProc(
    LPVOID
)
{
    const HRESULT comHr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    Wh_Log(
        L"Explorer Info Bar worker start PID=%lu COM=0x%08X",
        g_pid,
        static_cast<unsigned>(comHr)
    );

    if (FAILED(comHr))
    {
        Wh_Log(
            L"Explorer Info Bar worker COM initialization failed "
            L"PID=%lu HRESULT=0x%08X",
            g_pid,
            static_cast<unsigned>(comHr)
        );
        return 0;
    }

    const bool callCancellationEnabled =
        SUCCEEDED(
            CoEnableCallCancellation(nullptr)
        );

    IGlobalInterfaceTable* table = nullptr;
    const HRESULT gitHr =
        CreateGlobalInterfaceTable(&table);

    if (FAILED(gitHr) || !table)
    {
        Wh_Log(
            L"Global Interface Table initialization failed HRESULT=0x%08X",
            static_cast<unsigned>(gitHr)
        );

        if (callCancellationEnabled)
            CoDisableCallCancellation(nullptr);

        CoUninitialize();
        return 0;
    }

    if (
        WaitForWorkerStop(
            kInitialRefreshDelayMs
        )
    )
    {
        table->Release();

        if (callCancellationEnabled)
            CoDisableCallCancellation(nullptr);

        CoUninitialize();
        return 0;
    }

    struct WorkerTarget
    {
        HWND hwnd;
        DWORD shellBrowserCookie;
        ULONGLONG selectionGeneration;
    };

    while (true)
    {
        std::vector<WorkerTarget> targetCandidates;
        std::vector<WorkerTarget> targets;
        std::vector<HWND> registrationRetries;

        bool snapshotFailed = false;
        const ULONGLONG now = GetTickCount64();

        AcquireSRWLockExclusive(&g_subclassLock);

        try
        {
            targetCandidates.reserve(g_trackedWindows.size());
            registrationRetries.reserve(g_trackedWindows.size());

            for (TrackedDirectUiState& state : g_trackedWindows)
            {
                if (state.shellBrowserCookie)
                {
                    targetCandidates.push_back({
                        state.hwnd,
                        state.shellBrowserCookie,
                        state.selectionGeneration
                    });
                }
                else if (
                    !state.lastShellBrowserRegistrationRetryTick ||
                    now - state.lastShellBrowserRegistrationRetryTick >=
                        kShellBrowserRegistrationRetryMs
                )
                {
                    registrationRetries.push_back(state.hwnd);
                    state.lastShellBrowserRegistrationRetryTick = now;
                }
            }
        }
        catch (...)
        {
            snapshotFailed = true;
        }

        ReleaseSRWLockExclusive(&g_subclassLock);

        if (snapshotFailed)
        {
            Wh_Log(L"Worker target snapshot failed");
            targetCandidates.clear();
            registrationRetries.clear();
        }

        for (HWND hwnd : registrationRetries)
        {
            if (
                g_unloading.load(std::memory_order_acquire) ||
                WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
            )
            {
                break;
            }

            if (
                g_refreshDirectUiMessage &&
                !PostMessageW(
                    hwnd,
                    g_refreshDirectUiMessage,
                    0,
                    0
                )
            )
            {
                Wh_Log(
                    L"Shell browser registration retry post failed "
                    L"hwnd=%p error=%lu",
                    hwnd,
                    GetLastError()
                );
            }
        }

        try
        {
            targets.reserve(targetCandidates.size());

            for (const WorkerTarget& candidate : targetCandidates)
            {
                // Inactive Windows 11 tabs keep their DirectUIHWND tracked,
                // but a hidden target cannot contribute visible output.
                if (IsWindowVisible(candidate.hwnd))
                    targets.push_back(candidate);
            }
        }
        catch (...)
        {
            Wh_Log(L"Visible worker target snapshot failed");
            targets.clear();
        }

        for (const WorkerTarget& target : targets)
        {
            if (
                g_unloading.load(std::memory_order_acquire) ||
                WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
            )
            {
                break;
            }

            IShellBrowser* browser = nullptr;

            const HRESULT hr =
                table->GetInterfaceFromGlobal(
                    target.shellBrowserCookie,
                    IID_IShellBrowser,
                    reinterpret_cast<void**>(&browser)
                );

            if (FAILED(hr) || !browser)
                continue;

            const unsigned changes =
                ReadCurrentView(
                    target.hwnd,
                    browser,
                    target.selectionGeneration
                );

            browser->Release();

            if (changes != CacheChangeNone)
            {
                RefreshInfoBarWindow(
                    target.hwnd,
                    changes
                );
            }
        }

        if (
            WaitForWorkerStop(
                kRefreshIntervalMs
            )
        )
        {
            break;
        }
    }

    table->Release();

    if (callCancellationEnabled)
        CoDisableCallCancellation(nullptr);

    CoUninitialize();

    Wh_Log(
        L"Explorer Info Bar worker end PID=%lu",
        g_pid
    );

    return 0;
}

// ============================================================
// Final-paint renderer
// ============================================================

static bool IsDirectUiWindow(
    HWND hwnd
)
{
    if (!hwnd)
        return false;

    wchar_t cls[128] = {};

    if (!GetClassNameW(
            hwnd,
            cls,
            ARRAYSIZE(cls)))
    {
        return false;
    }

    return wcscmp(
        cls,
        L"DirectUIHWND"
    ) == 0;
}

static bool IsExplorerDirectUiTarget(HWND hwnd)
{
    if (!IsDirectUiWindow(hwnd))
        return false;

    const auto hasClass =
        [](HWND window, PCWSTR expected)
        {
            wchar_t cls[128] = {};

            return
                window &&
                GetClassNameW(window, cls, ARRAYSIZE(cls)) &&
                wcscmp(cls, expected) == 0;
        };

    const HWND duiView = GetParent(hwnd);
    const HWND shellTab = duiView ? GetParent(duiView) : nullptr;
    const HWND cabinet = shellTab ? GetParent(shellTab) : nullptr;

    if (
        !hasClass(duiView, L"DUIViewWndClassName") ||
        !hasClass(shellTab, L"ShellTabWindowClass") ||
        !hasClass(cabinet, L"CabinetWClass") ||
        GetAncestor(hwnd, GA_ROOT) != cabinet
    )
    {
        return false;
    }

    DWORD targetPid = 0;
    const DWORD targetThread =
        GetWindowThreadProcessId(hwnd, &targetPid);

    const auto matchesTargetOwner =
        [&](HWND window)
        {
            DWORD pid = 0;

            return
                GetWindowThreadProcessId(window, &pid) == targetThread &&
                pid == targetPid;
        };

    return
        targetThread != 0 &&
        targetPid == g_pid &&
        matchesTargetOwner(duiView) &&
        matchesTargetOwner(shellTab) &&
        matchesTargetOwner(cabinet);
}

static int MeasureTextWidth(
    HDC hdc,
    const std::wstring& text
)
{
    if (text.empty())
        return 0;

    SIZE size{};

    if (!GetTextExtentPoint32W(
            hdc,
            text.c_str(),
            static_cast<int>(
                text.length()
            ),
            &size))
    {
        return 0;
    }

    return size.cx;
}

static int MeasureGapWidth(
    HDC hdc
)
{
    const wchar_t* gap =
        L"     ";

    SIZE size{};

    if (
        GetTextExtentPoint32W(
            hdc,
            gap,
            5,
            &size
        )
    )
    {
        return size.cx;
    }

    return 26;
}

static COLORREF GetAppThemeFallbackBackground()
{
    DWORD appsUseLightTheme = 1;
    DWORD valueSize = sizeof(appsUseLightTheme);

    const LSTATUS status =
        RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            RRF_RT_REG_DWORD,
            nullptr,
            &appsUseLightTheme,
            &valueSize
        );

    const bool useLightTheme =
        status != ERROR_SUCCESS ||
        appsUseLightTheme != 0;

    return useLightTheme
        ? RGB(255, 255, 255)
        : RGB(32, 32, 32);
}

static COLORREF PickBackgroundColor(
    HDC hdc,
    HWND hwnd,
    const RECT& row,
    const RECT* nativePaintRect,
    int selected
)
{
    AutomaticThemeSnapshot theme =
        GetAutomaticThemeSnapshot(hwnd);

    const bool fullRowRepaint =
        nativePaintRect &&
        nativePaintRect->left <= row.left &&
        nativePaintRect->right >= row.right &&
        nativePaintRect->top <= row.top &&
        nativePaintRect->bottom >= row.bottom;

    if (theme.rowBackground == CLR_INVALID)
    {
        theme.rowBackground =
            GetAppThemeFallbackBackground();

        UpdateAutomaticRowBackground(
            hwnd,
            theme.rowBackground,
            false
        );
    }

    if (!fullRowRepaint)
        return theme.rowBackground;

    if (
        theme.hasSampledNativeRowBackground &&
        GetTickCount64() - theme.lastNativeRowBackgroundSampleTick <
            kNativeRowBackgroundSampleIntervalMs
    )
    {
        return theme.rowBackground;
    }

    const int rowWidth = row.right - row.left;
    const int rowHeight = row.bottom - row.top;

    if (
        selected <= 0 &&
        nativePaintRect &&
        rowWidth > 0 &&
        rowHeight > 0
    )
    {
        const int y = row.top + (rowHeight / 2);
        const int sampleNumerators[] = { 9, 8, 7, 6, 5, 4 };
        const int safetyInset = 1;

        for (int numerator : sampleNumerators)
        {
            const int x =
                row.left +
                (rowWidth * numerator) / 10;

            if (
                x - safetyInset < nativePaintRect->left ||
                x + safetyInset >= nativePaintRect->right ||
                y - safetyInset < nativePaintRect->top ||
                y + safetyInset >= nativePaintRect->bottom
            )
            {
                continue;
            }

            const COLORREF sample = GetPixel(hdc, x, y);

            if (sample != CLR_INVALID)
            {
                UpdateAutomaticRowBackground(
                    hwnd,
                    sample,
                    true
                );

                return sample;
            }
        }
    }

    // A partial first paint might not expose any safe native pixels. Keep
    // sampling pending while using the app-theme preference as a safe fallback.
    return theme.rowBackground;
}

static void DrawFinalPiece(
    HDC hdc,
    int& x,
    const RECT& row,
    const std::wstring& text,
    COLORREF color
)
{
    if (text.empty())
        return;

    if (x >= row.right)
    {
        x = row.right;
        return;
    }

    int width =
        MeasureTextWidth(
            hdc,
            text
        );

    RECT rc =
        row;

    rc.left =
        x;

    rc.right =
        std::min(
            x + width + 4,
            static_cast<int>(
                row.right
            )
        );

    SetTextColor(
        hdc,
        color
    );

    DrawTextW(
        hdc,
        text.c_str(),
        -1,
        &rc,
        DT_SINGLELINE |
        DT_VCENTER |
        DT_NOPREFIX
    );

    x += width;
}

static void DrawFinalSeparator(
    HDC hdc,
    int& x,
    const RECT& row,
    COLORREF color
)
{
    x +=
        MeasureGapWidth(
            hdc
        );

    DrawFinalPiece(
        hdc,
        x,
        row,
        L"\x00B7",
        color
    );

    x +=
        MeasureGapWidth(
            hdc
        );
}

static int ColorLuminance(
    COLORREF color
)
{
    return (
        GetRValue(color) * 30 +
        GetGValue(color) * 59 +
        GetBValue(color) * 11
    ) / 100;
}

static COLORREF BlendColor(
    COLORREF base,
    COLORREF target,
    int targetPercent
)
{
    targetPercent =
        std::max(
            0,
            std::min(
                100,
                targetPercent
            )
        );

    const int basePercent =
        100 - targetPercent;

    return RGB(
        (
            GetRValue(base) * basePercent +
            GetRValue(target) * targetPercent
        ) / 100,
        (
            GetGValue(base) * basePercent +
            GetGValue(target) * targetPercent
        ) / 100,
        (
            GetBValue(base) * basePercent +
            GetBValue(target) * targetPercent
        ) / 100
    );
}

static COLORREF GetContrastingTextColor(
    COLORREF background
)
{
    return ColorLuminance(background) >= 140
        ? RGB(32, 32, 32)
        : RGB(232, 232, 232);
}

static COLORREF ResolveColor(
    const ColorOverride& colorOverride,
    COLORREF automaticColor
)
{
    return colorOverride.enabled
        ? colorOverride.value
        : automaticColor;
}

static void PaintFinalInfoBar(
    HDC hdc,
    HWND hwnd,
    const RECT* updateRect = nullptr
)
{
    if (!hdc || !hwnd)
        return;

    UINT dpi = 96;
    HFONT font = nullptr;

    if (!GetPaintResources(hwnd, &dpi, &font))
        return;

    RECT client{};

    if (!GetClientRect(
            hwnd,
            &client))
    {
        return;
    }

    RECT row{};

    bool hasValidatedRow =
        GetValidatedStatusRow(hwnd, &row);

    if (
        !hasValidatedRow ||
        IsStatusRowRevalidationDue(hwnd)
    )
    {
        if (
            !RefreshValidatedStatusRow(hwnd) ||
            !GetValidatedStatusRow(hwnd, &row)
        )
        {
            return;
        }
    }

    RECT coverRow = row;

    // The native controls occupy only the compact area at the far right.
    // Cover native status text up to that area independently of the wider
    // content-layout reservation below.
    coverRow.right =
        std::max(
            coverRow.left,
            client.right - ScaleForDpi(dpi, 64)
        );

    row.left = ScaleForDpi(dpi, 6);

    // Keep the existing conservative content margin without leaving the
    // native status-text area uncovered when the window is narrow.
    row.right =
        std::max(
            row.left,
            std::min(
                coverRow.right,
                client.right > ScaleForDpi(dpi, 220)
                    ? client.right - ScaleForDpi(dpi, 220)
                    : coverRow.right
            )
        );

    std::wstring contentGroup;
    std::wstring selectionGroup;
    std::wstring driveGroup;
    std::wstring fileDetailsGroup;

    int selected = 0;

    GetCachedGroups(
        hwnd,
        contentGroup,
        selectionGroup,
        driveGroup,
        fileDetailsGroup,
        selected
    );

    const ModSettings settings =
        GetSettingsSnapshot();

    const bool showFileDetails =
        settings.singleFileDetails &&
        !fileDetailsGroup.empty();

    const bool hasVisibleContent =
        (
            settings.showDrive &&
            !driveGroup.empty()
        ) ||
        (
            settings.showContent &&
            !contentGroup.empty()
        ) ||
        (
            settings.showSelection &&
            !selectionGroup.empty()
        ) ||
        showFileDetails;

    // Leave Explorer's native status row untouched when nothing custom is visible.
    if (!hasVisibleContent)
        return;

    RECT paintRect =
        coverRow;

    if (
        updateRect &&
        !IntersectRect(
            &paintRect,
            &paintRect,
            updateRect
        )
    )
    {
        return;
    }

    const int savedDc =
        SaveDC(hdc);

    if (!savedDc)
        return;

    IntersectClipRect(
        hdc,
        paintRect.left,
        paintRect.top,
        paintRect.right,
        paintRect.bottom
    );

    COLORREF background =
        PickBackgroundColor(
            hdc,
            hwnd,
            row,
            updateRect ? &paintRect : nullptr,
            selected
        );

    HBRUSH brush =
        CreateSolidBrush(
            background
        );

    if (brush)
    {
        FillRect(
            hdc,
            &coverRow,
            brush
        );

        DeleteObject(
            brush
        );
    }

    int oldBkMode =
        SetBkMode(
            hdc,
            TRANSPARENT
        );

    COLORREF oldTextColor =
        GetTextColor(
            hdc
        );

    HFONT oldFont = nullptr;

    if (font)
    {
        oldFont =
            reinterpret_cast<HFONT>(
                SelectObject(
                    hdc,
                    font
                )
            );
    }

    const COLORREF nativeText =
        GetContrastingTextColor(
            background
        );

    const COLORREF textColor =
        ResolveColor(
            settings.textColor,
            nativeText
        );

    // Auto mode must stay visually neutral and theme-compatible.
    // All three panels use the SAME theme-derived fill by default.
    // Individual sections differ only when the user explicitly overrides
    // Drive / Content / Selected colors in settings.
    const COLORREF automaticPanelColor =
        BlendColor(
            background,
            nativeText,
            6
        );

    const COLORREF drivePanelColor =
        ResolveColor(
            settings.driveColor,
            automaticPanelColor
        );

    const COLORREF contentPanelColor =
        ResolveColor(
            settings.contentColor,
            automaticPanelColor
        );

    const COLORREF selectionPanelColor =
        ResolveColor(
            settings.selectionColor,
            automaticPanelColor
        );

    const COLORREF fileDetailsPanelColor =
        ResolveColor(
            settings.fileDetailsColor,
            automaticPanelColor
        );

    const COLORREF automaticDividerColor =
        BlendColor(
            background,
            nativeText,
            22
        );

    // The custom divider/border override intentionally applies only to
    // Soft Cards. Simple and Flat Panes always derive their structure
    // from Explorer so changing this setting has no hidden side effects.
    const COLORREF cardBorderColor =
        ResolveColor(
            settings.dividerColor,
            automaticDividerColor
        );

    auto GetSectionText =
        [&](InfoBarSection section) -> const std::wstring&
        {
            if (section == InfoBarSection::Drive)
                return driveGroup;

            if (section == InfoBarSection::Content)
                return contentGroup;

            return selectionGroup;
        };

    auto IsSectionEnabled =
        [&](InfoBarSection section) -> bool
        {
            if (section == InfoBarSection::Drive)
                return settings.showDrive;

            if (section == InfoBarSection::Content)
                return settings.showContent;

            return settings.showSelection;
        };

    int x =
        ScaleForDpi(dpi, 14);

    bool drew =
        false;

    InfoBarLayoutGeometry geometry;
    geometry.hwnd = hwnd;
    geometry.usableRight = row.right;
    geometry.style = settings.style;
    geometry.sectionOrder = settings.sectionOrder;
    geometry.showDrive = settings.showDrive;
    geometry.showContent = settings.showContent;
    geometry.showSelection = settings.showSelection;
    geometry.singleFileDetails = settings.singleFileDetails;

    if (settings.style == InfoBarStyle::Simple)
    {
        for (InfoBarSection section : settings.sectionOrder)
        {
            if (!IsSectionEnabled(section))
                continue;

            geometry.sectionLeft[
                GetSectionGeometryIndex(section)
            ] = x;

            const std::wstring& value =
                GetSectionText(
                    section
                );

            if (value.empty())
                continue;

            if (drew)
            {
                DrawFinalSeparator(
                    hdc,
                    x,
                    row,
                    automaticDividerColor
                );
            }

            DrawFinalPiece(
                hdc,
                x,
                row,
                value,
                textColor
            );

            drew =
                true;
        }

        geometry.fileDetailsLeft =
            x;

        if (showFileDetails)
        {
            if (drew)
            {
                DrawFinalSeparator(
                    hdc,
                    x,
                    row,
                    automaticDividerColor
                );
            }

            DrawFinalPiece(
                hdc,
                x,
                row,
                fileDetailsGroup,
                textColor
            );
        }
    }
    else
    {
        const bool cards =
            settings.style == InfoBarStyle::Cards;

        const int padX =
            ScaleForDpi(dpi, cards ? 10 : 12);

        const int gap =
            ScaleForDpi(dpi, cards ? 8 : 6);

        // Flat panes should begin flush with the left edge.
        // Cards keep a tiny 2 px inset so the rounded border isn't clipped.
        int paneX =
            cards
                ? row.left + ScaleForDpi(dpi, 2)
                : 0;

        auto DrawBox =
            [&](const std::wstring& value,
                COLORREF fill,
                COLORREF border,
                COLORREF textColor)
        {
            if (value.empty())
                return;

            int width =
                MeasureTextWidth(
                    hdc,
                    value
                ) +
                padX * 2;

            RECT box{
                paneX,
                row.top + ScaleForDpi(dpi, cards ? 3 : 1),
                paneX + width,
                row.bottom - ScaleForDpi(dpi, cards ? 3 : 1)
            };

            if (box.right > row.right)
                box.right = row.right;

            if (cards)
            {
                HBRUSH fillBrush =
                    CreateSolidBrush(fill);

                HPEN pen =
                    CreatePen(
                        PS_SOLID,
                        1,
                        border
                    );

                if (fillBrush && pen)
                {
                    HBRUSH oldBrush =
                        reinterpret_cast<HBRUSH>(
                            SelectObject(hdc, fillBrush)
                        );

                    HPEN oldPen =
                        reinterpret_cast<HPEN>(
                            SelectObject(hdc, pen)
                        );

                    RoundRect(
                        hdc,
                        box.left,
                        box.top,
                        box.right,
                        box.bottom,
                        ScaleForDpi(dpi, 6),
                        ScaleForDpi(dpi, 6)
                    );

                    SelectObject(hdc, oldPen);
                    SelectObject(hdc, oldBrush);
                }

                if (pen)
                    DeleteObject(pen);

                if (fillBrush)
                    DeleteObject(fillBrush);
            }
            else
            {
                HBRUSH fillBrush =
                    CreateSolidBrush(fill);

                if (fillBrush)
                {
                    FillRect(
                        hdc,
                        &box,
                        fillBrush
                    );

                    DeleteObject(fillBrush);
                }
            }

            RECT textRect =
                box;

            textRect.left += padX;
            textRect.right -= padX;

            SetTextColor(
                hdc,
                textColor
            );

            DrawTextW(
                hdc,
                value.c_str(),
                -1,
                &textRect,
                DT_SINGLELINE |
                DT_VCENTER |
                DT_NOPREFIX |
                DT_END_ELLIPSIS
            );

            paneX =
                box.right +
                gap;
        };

        for (InfoBarSection section : settings.sectionOrder)
        {
            if (!IsSectionEnabled(section))
                continue;

            geometry.sectionLeft[
                GetSectionGeometryIndex(section)
            ] = paneX;

            const std::wstring& value =
                GetSectionText(
                    section
                );

            if (value.empty())
                continue;

            COLORREF panelColor =
                contentPanelColor;

            if (section == InfoBarSection::Drive)
                panelColor = drivePanelColor;
            else if (section == InfoBarSection::Selection)
                panelColor = selectionPanelColor;

            DrawBox(
                value,
                panelColor,
                cardBorderColor,
                textColor
            );
        }

        geometry.fileDetailsLeft =
            paneX;

        if (showFileDetails)
        {
            DrawBox(
                fileDetailsGroup,
                fileDetailsPanelColor,
                cardBorderColor,
                textColor
            );
        }
    }

    StoreLayoutGeometry(
        geometry
    );

    if (oldFont)
    {
        SelectObject(
            hdc,
            oldFont
        );
    }

    SetTextColor(
        hdc,
        oldTextColor
    );

    SetBkMode(
        hdc,
        oldBkMode
    );

    RestoreDC(
        hdc,
        savedDc
    );
}


enum class DirectUiSubclassResult
{
    Failed,
    AlreadyInstalled,
    NewlyInstalled
};

static DirectUiSubclassResult EnsureDirectUiSubclass(
    HWND hwnd
)
{
    if (
        !IsExplorerDirectUiTarget(hwnd) ||
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        return DirectUiSubclassResult::Failed;
    }

    AcquireSRWLockExclusive(
        &g_subclassLock
    );

    if (
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        ReleaseSRWLockExclusive(
            &g_subclassLock
        );

        return DirectUiSubclassResult::Failed;
    }

    if (
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        ) != g_trackedWindows.end()
    )
    {
        ReleaseSRWLockExclusive(
            &g_subclassLock
        );

        return DirectUiSubclassResult::AlreadyInstalled;
    }

    if (
        std::find(
            g_installingDirectUiWindows.begin(),
            g_installingDirectUiWindows.end(),
            hwnd
        ) != g_installingDirectUiWindows.end()
    )
    {
        ReleaseSRWLockExclusive(&g_subclassLock);
        return DirectUiSubclassResult::AlreadyInstalled;
    }

    try
    {
        g_installingDirectUiWindows.push_back(hwnd);
    }
    catch (...)
    {
        ReleaseSRWLockExclusive(&g_subclassLock);
        Wh_Log(L"DirectUI install tracking failed hwnd=%p", hwnd);
        return DirectUiSubclassResult::Failed;
    }

    // Never hold g_subclassLock while the utility synchronously marshals the
    // actual SetWindowSubclass call to the HWND's owning Explorer UI thread.
    ReleaseSRWLockExclusive(&g_subclassLock);

    if (
        !WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd,
            DirectUiSubclassProc,
            0
        )
    )
    {
        AcquireSRWLockExclusive(&g_subclassLock);
        g_installingDirectUiWindows.erase(
            std::remove(
                g_installingDirectUiWindows.begin(),
                g_installingDirectUiWindows.end(),
                hwnd
            ),
            g_installingDirectUiWindows.end()
        );
        ReleaseSRWLockExclusive(&g_subclassLock);

        Wh_Log(
            L"DirectUI subclass install failed hwnd=%p error=%lu",
            hwnd,
            GetLastError()
        );

        return DirectUiSubclassResult::Failed;
    }

    bool keepInstalled = false;
    bool trackingFailed = false;

    AcquireSRWLockExclusive(&g_subclassLock);

    const auto installing =
        std::find(
            g_installingDirectUiWindows.begin(),
            g_installingDirectUiWindows.end(),
            hwnd
        );

    if (
        installing != g_installingDirectUiWindows.end() &&
        !g_unloading.load(std::memory_order_acquire)
    )
    {
        try
        {
            TrackedDirectUiState state;
            state.hwnd = hwnd;
            g_trackedWindows.push_back(std::move(state));
            keepInstalled = true;
        }
        catch (...)
        {
            trackingFailed = true;
        }
    }

    if (installing != g_installingDirectUiWindows.end())
        g_installingDirectUiWindows.erase(installing);

    ReleaseSRWLockExclusive(&g_subclassLock);

    if (!keepInstalled)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hwnd,
            DirectUiSubclassProc
        );

        if (trackingFailed)
            Wh_Log(L"DirectUI subclass tracking failed hwnd=%p", hwnd);

        return DirectUiSubclassResult::Failed;
    }

    RefreshTrackedDpiAndFont(hwnd);
    EnsureWindowDataCache(hwnd);
    EnsureShellBrowserRegistration(hwnd);

    // Defer the first invalidation to the DirectUI owning thread.
    if (
        g_refreshDirectUiMessage &&
        !PostMessageW(
            hwnd,
            g_refreshDirectUiMessage,
            0,
            0
        )
    )
    {
        Wh_Log(
            L"DirectUI initial refresh post failed hwnd=%p error=%lu",
            hwnd,
            GetLastError()
        );
    }

    return DirectUiSubclassResult::NewlyInstalled;
}

static void TryAttachExplorerDirectUiWindow(HWND hwnd)
{
    if (IsExplorerDirectUiTarget(hwnd))
        EnsureDirectUiSubclass(hwnd);
}

static LRESULT CALLBACK DirectUiSubclassProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR
)
{
    if (
        g_refreshDirectUiMessage &&
        msg == g_refreshDirectUiMessage
    )
    {
        if (!g_unloading.load(std::memory_order_acquire))
        {
            EnsureWindowDataCache(hwnd);
            EnsureShellBrowserRegistration(hwnd);
            RefreshValidatedStatusRow(hwnd);
            InvalidateInfoBarWindow(hwnd);
        }

        return 0;
    }

    if (msg == WM_NCDESTROY)
    {
        const DWORD shellBrowserCookie =
            GetShellBrowserCookieSnapshot(hwnd);
        HFONT infoBarFont = nullptr;

        EraseWindowDataCache(hwnd);
        RevokeShellBrowserCookie(shellBrowserCookie);

        AcquireSRWLockExclusive(&g_subclassLock);
        UntrackDirectUiWindowLocked(hwnd, &infoBarFont);
        g_installingDirectUiWindows.erase(
            std::remove(
                g_installingDirectUiWindows.begin(),
                g_installingDirectUiWindows.end(),
                hwnd
            ),
            g_installingDirectUiWindows.end()
        );
        ReleaseSRWLockExclusive(&g_subclassLock);

        if (infoBarFont)
            DeleteObject(infoBarFont);

        return DefSubclassProc(
            hwnd,
            msg,
            wParam,
            lParam
        );
    }

    if (
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        return DefSubclassProc(
            hwnd,
            msg,
            wParam,
            lParam
        );
    }

    if (
        msg == WM_SIZE ||
        msg == WM_WINDOWPOSCHANGED ||
        msg == WM_DPICHANGED
    )
    {
        const LRESULT result =
            DefSubclassProc(
                hwnd,
                msg,
                wParam,
                lParam
            );

        if (
            msg == WM_WINDOWPOSCHANGED ||
            msg == WM_DPICHANGED
        )
        {
            RefreshTrackedDpiAndFont(hwnd);
        }

        if (
            msg != WM_WINDOWPOSCHANGED ||
            IsStatusRowRevalidationDue(hwnd)
        )
        {
            RefreshValidatedStatusRow(hwnd);
        }

        InvalidateInfoBarWindow(hwnd);
        return result;
    }

    if (
        msg == WM_THEMECHANGED ||
        msg == WM_SETTINGCHANGE ||
        msg == WM_SYSCOLORCHANGE
    )
    {
        const LRESULT result =
            DefSubclassProc(
                hwnd,
                msg,
                wParam,
                lParam
            );

        InvalidateAutomaticTheme(hwnd);
        InvalidateInfoBarWindow(hwnd);
        return result;
    }

    if (msg == WM_PAINT)
    {
        RECT updateRect{};

        const BOOL hasUpdateRect =
            GetUpdateRect(
                hwnd,
                &updateRect,
                FALSE
            );

        RECT row{};
        RECT intersection{};

        if (
            !hasUpdateRect ||
            (
                GetValidatedStatusRow(hwnd, &row) &&
                IntersectRect(
                    &intersection,
                    &updateRect,
                    &row
                )
            )
        )
        {
            WakeWorkerFromPaint(hwnd);
        }

        // Let DirectUI finish ALL of its own buffered painting first.
        LRESULT result =
            DefSubclassProc(
                hwnd,
                msg,
                wParam,
                lParam
            );

        HDC hdc =
            GetDC(hwnd);

        if (hdc)
        {
            PaintFinalInfoBar(
                hdc,
                hwnd,
                hasUpdateRect
                    ? &updateRect
                    : nullptr
            );

            ReleaseDC(
                hwnd,
                hdc
            );
        }

        return result;
    }

    return DefSubclassProc(
        hwnd,
        msg,
        wParam,
        lParam
    );
}

// ============================================================
// Windhawk lifecycle
// ============================================================

static BOOL CALLBACK DiscoverDirectUiChildProc(
    HWND hwnd,
    LPARAM
)
{
    if (g_unloading.load(std::memory_order_acquire))
        return FALSE;

    TryAttachExplorerDirectUiWindow(hwnd);

    return TRUE;
}

static BOOL CALLBACK DiscoverExistingExplorerWindowProc(
    HWND hwnd,
    LPARAM
)
{
    if (g_unloading.load(std::memory_order_acquire))
        return FALSE;

    DWORD pid = 0;

    GetWindowThreadProcessId(
        hwnd,
        &pid
    );

    if (
        pid != g_pid
    )
    {
        return TRUE;
    }

    wchar_t className[128] = {};

    if (
        !GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) ||
        wcscmp(className, L"CabinetWClass") != 0
    )
    {
        return TRUE;
    }

    EnumChildWindows(
        hwnd,
        DiscoverDirectUiChildProc,
        0
    );

    return TRUE;
}

static void DiscoverExistingExplorerWindows()
{
    EnumWindows(
        DiscoverExistingExplorerWindowProc,
        0
    );
}

BOOL Wh_ModInit()
{
    g_unloading.store(
        false,
        std::memory_order_release
    );

    g_pid =
        GetCurrentProcessId();

    Wh_Log(
        L"========== Explorer Info Bar INIT PID=%lu ==========",
        g_pid
    );

    InitializeCriticalSection(
        &g_cacheLock
    );

    g_refreshDirectUiMessage =
        RegisterWindowMessageW(
            L"Windhawk_ExplorerInfoBar_RefreshDirectUi"
        );

    if (!g_refreshDirectUiMessage)
    {
        Wh_Log(
            L"DirectUI message registration failed error=%lu",
            GetLastError()
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    LoadSettings();

    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_stopEvent)
    {
        Wh_Log(
            L"stop event creation failed error=%lu",
            GetLastError()
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    g_workerWakeEvent =
        CreateEventW(
            nullptr,
            FALSE,
            FALSE,
            nullptr
        );

    if (!g_workerWakeEvent)
    {
        Wh_Log(
            L"worker wake event creation failed error=%lu",
            GetLastError()
        );

        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        DeleteCriticalSection(&g_cacheLock);
        return FALSE;
    }

    g_selectionWinEventThreadReady =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    g_selectionWinEventStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (
        !g_selectionWinEventThreadReady ||
        !g_selectionWinEventStopEvent
    )
    {
        Wh_Log(
            L"selection WinEvent helper event creation failed error=%lu",
            GetLastError()
        );

        if (g_selectionWinEventThreadReady)
        {
            CloseHandle(g_selectionWinEventThreadReady);
            g_selectionWinEventThreadReady = nullptr;
        }

        if (g_selectionWinEventStopEvent)
        {
            CloseHandle(g_selectionWinEventStopEvent);
            g_selectionWinEventStopEvent = nullptr;
        }
    }
    else
    {
        g_selectionWinEventThread =
            CreateThread(
                nullptr,
                0,
                SelectionWinEventThreadProc,
                nullptr,
                0,
                nullptr
            );

        if (!g_selectionWinEventThread)
        {
            Wh_Log(
                L"selection WinEvent helper creation failed error=%lu",
                GetLastError()
            );

            CloseHandle(g_selectionWinEventThreadReady);
            g_selectionWinEventThreadReady = nullptr;
            CloseHandle(g_selectionWinEventStopEvent);
            g_selectionWinEventStopEvent = nullptr;
        }
        else
        {
            // The helper signals only after queue creation and its hook
            // installation attempt, closing the startup/shutdown race.
            WaitForSingleObject(g_selectionWinEventThreadReady, INFINITE);
            CloseHandle(g_selectionWinEventThreadReady);
            g_selectionWinEventThreadReady = nullptr;
        }
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThreadProc,
            nullptr,
            0,
            &g_workerThreadId
        );

    if (!g_workerThread)
    {
        StopSelectionWinEventThread();

        if (g_selectionWinEventStopEvent)
        {
            CloseHandle(g_selectionWinEventStopEvent);
            g_selectionWinEventStopEvent = nullptr;
        }

        Wh_Log(
            L"worker creation failed error=%lu",
            GetLastError()
        );

        CloseHandle(
            g_stopEvent
        );

        g_stopEvent =
            nullptr;

        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
        g_workerThreadId = 0;

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    Wh_Log(
        L"Explorer Info Bar ready"
    );

    return TRUE;
}


void Wh_ModAfterInit()
{
    DiscoverExistingExplorerWindows();
}


void Wh_ModSettingsChanged()
{
    LoadSettings();

    // Force one detailed selection refresh because visibility/detail settings
    // may have changed even if the Explorer selection itself did not.
    AcquireSRWLockExclusive(&g_subclassLock);

    for (TrackedDirectUiState& state : g_trackedWindows)
        state.selectionGeneration++;

    ReleaseSRWLockExclusive(&g_subclassLock);

    if (g_workerWakeEvent)
        SetEvent(g_workerWakeEvent);

    RefreshInfoBars();
}

static void CancelWorkerComCall()
{
    if (!g_workerThread || !g_workerThreadId)
        return;

    const DWORD waitResult =
        WaitForSingleObject(
            g_workerThread,
            0
        );

    if (waitResult == WAIT_OBJECT_0)
        return;

    if (waitResult != WAIT_TIMEOUT)
    {
        Wh_Log(
            L"worker cancellation activity check failed "
            L"result=%lu error=%lu",
            waitResult,
            waitResult == WAIT_FAILED ? GetLastError() : ERROR_SUCCESS
        );
        return;
    }

    const HRESULT comHr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    const bool shouldUninitialize =
        comHr == S_OK ||
        comHr == S_FALSE;

    if (
        !shouldUninitialize &&
        comHr != RPC_E_CHANGED_MODE
    )
    {
        Wh_Log(
            L"worker cancellation COM initialization failed "
            L"HRESULT=0x%08X",
            static_cast<unsigned>(comHr)
        );
        return;
    }

    const HRESULT cancelHr =
        CoCancelCall(
            g_workerThreadId,
            0
        );

    if (shouldUninitialize)
        CoUninitialize();

    if (
        cancelHr != S_OK &&
        cancelHr != RPC_E_CALL_COMPLETE &&
        cancelHr != RPC_E_CALL_CANCELED
    )
    {
        Wh_Log(
            L"worker COM-call cancellation failed HRESULT=0x%08X",
            static_cast<unsigned>(cancelHr)
        );
    }
}

void Wh_ModBeforeUninit()
{
    g_unloading.store(
        true,
        std::memory_order_release
    );

    if (g_stopEvent)
        SetEvent(g_stopEvent);

    CancelWorkerComCall();

    if (g_workerThread)
        CancelSynchronousIo(g_workerThread);

    // Snapshot under the lock, then remove subclasses outside it. The
    // Windhawk helper handles cross-thread subclass removal without the
    // custom retry/message/barrier machinery that could previously loop
    // forever on a hung Explorer UI thread.
    std::vector<HWND> windows;

    AcquireSRWLockShared(&g_subclassLock);
    windows.reserve(g_trackedWindows.size());
    for (const TrackedDirectUiState& state : g_trackedWindows)
        windows.push_back(state.hwnd);
    ReleaseSRWLockShared(&g_subclassLock);

    for (HWND hwnd : windows)
    {
        if (hwnd && IsWindow(hwnd))
        {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hwnd,
                DirectUiSubclassProc
            );

            // The overlay is drawn after Explorer's native buffered paint, so
            // removing the subclass alone leaves those pixels on screen until
            // the next native repaint. Force the status row to repaint now
            // while g_unloading prevents any new overlay from being drawn.
            RECT row{};
            if (GetValidatedStatusRow(hwnd, &row))
            {
                RedrawWindow(
                    hwnd,
                    &row,
                    nullptr,
                    RDW_INVALIDATE |
                    RDW_ERASE |
                    RDW_UPDATENOW
                );
            }
        }

        DWORD shellBrowserCookie = 0;
        HFONT infoBarFont = nullptr;
        AcquireSRWLockExclusive(&g_subclassLock);
        shellBrowserCookie =
            UntrackDirectUiWindowLocked(hwnd, &infoBarFont);
        ReleaseSRWLockExclusive(&g_subclassLock);

        if (infoBarFont)
            DeleteObject(infoBarFont);

        EraseWindowDataCache(hwnd);
        RevokeShellBrowserCookie(shellBrowserCookie);
    }

    Wh_Log(L"Explorer Info Bar subclass teardown complete");
}

void Wh_ModUninit()
{
    if (g_stopEvent)
    {
        SetEvent(
            g_stopEvent
        );
    }

    StopSelectionWinEventThread();

    if (g_selectionWinEventStopEvent)
    {
        CloseHandle(g_selectionWinEventStopEvent);
        g_selectionWinEventStopEvent = nullptr;
    }

    if (g_workerThread)
    {
        // Cancellation can race with the worker entering its next COM or
        // filesystem call. Retry both mechanisms between bounded waits.
        // Never terminate a thread while it may own COM, CRT or cache locks.
        while (true)
        {
            CancelWorkerComCall();

            CancelSynchronousIo(g_workerThread);

            const DWORD waitResult =
                WaitForSingleObject(
                    g_workerThread,
                    500
                );

            if (waitResult == WAIT_OBJECT_0)
                break;

            if (waitResult != WAIT_TIMEOUT)
            {
                Wh_Log(
                    L"worker shutdown wait failed result=%lu error=%lu",
                    waitResult,
                    GetLastError()
                );

                DWORD exitCode = STILL_ACTIVE;

                if (
                    GetExitCodeThread(g_workerThread, &exitCode) &&
                    exitCode != STILL_ACTIVE
                )
                {
                    break;
                }

                Sleep(500);
            }
        }

        CloseHandle(
            g_workerThread
        );

        g_workerThread =
            nullptr;

        g_workerThreadId = 0;
    }

    if (g_stopEvent)
    {
        CloseHandle(
            g_stopEvent
        );

        g_stopEvent =
            nullptr;
    }

    if (g_workerWakeEvent)
    {
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
    }

    DeleteCriticalSection(
        &g_cacheLock
    );

    Wh_Log(
        L"========== Explorer Info Bar UNINIT PID=%lu ==========",
        g_pid
    );
}
