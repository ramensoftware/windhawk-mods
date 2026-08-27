// ==WindhawkMod==
// @id              explorer-info-bar
// @name            Explorer Info Bar
// @description     Enhances File Explorer's bottom info bar with drive, content, selection, and single-file details, with customizable styles and colors.
// @version         1.0.0
// @author          digART
// @github          https://github.com/digart11
// @homepage        https://github.com/digart11/explorer-info-bar
// @license         GPL-3.0-only
// @include         explorer.exe
// @compilerOptions -lole32 -lshell32 -luuid -lgdi32 -lcomctl32 -lpropsys
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Info Bar

A customizable Windhawk mod that enhances the bottom info bar in Windows 11 File Explorer.

Explorer Info Bar adds useful drive, folder, selection, and single-file information while keeping the native Explorer look and adapting to light, dark, and customized themes.

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

## Examples

Typical information shown by the mod:

Drive D: 150.7GB free
Content: 15 folders / 25 files (77.2MB)
Selected: 2 folders / 4 files (571KB)

When one file is selected, additional details can appear:

.jpg (4032&times;3024)
.jpeg (6000&times;4000)
.mp4 (1920&times;1080, 01:23:43)
.mp3 (00:03:47)
.doc
.pdf
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

- singleFileDetails: true
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

static constexpr UINT kNativeStatusTextFormat = 0x00000824;
static constexpr int kStatusRowHeight = 24;
static constexpr ULONGLONG kStatusMarkerLifetimeMs = 250;
static constexpr DWORD kInitialRefreshDelayMs = 1000;
static constexpr DWORD kRefreshIntervalMs = 500;
static constexpr ULONGLONG kContentFailedRetryMs = 2000;
static constexpr ULONGLONG kMetadataRetryMs = 5000;

// ============================================================
// DrawText hook
// ============================================================

using DrawTextW_t = int (WINAPI*)(
    HDC,
    LPCWSTR,
    int,
    LPRECT,
    UINT
);

static DrawTextW_t DrawTextW_Original = nullptr;

using BitBlt_t = BOOL (WINAPI*)(
    HDC,
    int,
    int,
    int,
    int,
    HDC,
    int,
    int,
    DWORD
);

static BitBlt_t BitBlt_Original = nullptr;

// Correlate Explorer's buffered status render with the final DirectUIHWND copy.
thread_local HDC g_statusSourceDc = nullptr;
thread_local ULONGLONG g_statusMarkTick = 0;
thread_local RECT g_statusRowRect{};
thread_local bool g_insideFinalPaint = false;

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
    bool hasLayout = false;
    InfoBarLayoutGeometry layout;
    COLORREF stableRowBackground = CLR_INVALID;
    COLORREF stableNativeTextColor = CLR_INVALID;
    HWND shellTab = nullptr;
    DWORD shellBrowserCookie = 0;
};

static std::vector<TrackedDirectUiState> g_trackedWindows;

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
    bool singleFileDetails = true;
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

// ============================================================
// State
// ============================================================

static DWORD g_pid = 0;

static HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_workerWakeEvent = nullptr;

static CRITICAL_SECTION g_cacheLock;

struct ContentRefreshCache
{
    bool valid = false;
    std::wstring folderIdentity;
    int itemCount = -1;
    int files = 0;
    int folders = 0;
    ULONGLONG directFileBytes = 0;
    ULONGLONG lastFullScanTick = 0;
    std::wstring lastAttemptFolderIdentity;
    ULONGLONG lastScanAttemptTick = 0;
};

struct SingleFileMetadataCache
{
    bool valid = false;
    std::wstring path;
    std::wstring details;
    ULONGLONG retryAfterTick = 0;
};

struct WindowDataCache
{
    HWND hwnd = nullptr;
    int selected = -1;
    std::wstring contentGroup = L"Loading...";
    std::wstring selectionGroup;
    std::wstring driveGroup;
    std::wstring fileDetailsGroup;
    ContentRefreshCache contentRefresh;
    SingleFileMetadataCache metadata;
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
    IShellItem* item,
    const std::wstring& path,
    bool* transientFailure
)
{
    if (transientFailure)
        *transientFailure = false;

    if (!item || path.empty())
        return L"";

    const std::wstring extension =
        GetLiteralExtension(path);

    std::wstring result =
        extension.empty()
            ? L"no extension"
            : extension;

    IShellItem2* item2 = nullptr;

    if (
        FAILED(
            item->QueryInterface(
                IID_PPV_ARGS(&item2)
            )
        ) ||
        !item2
    )
    {
        if (transientFailure)
            *transientFailure = true;

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
        existing->shellTab == shellTab &&
        existing->shellBrowserCookie != 0;

    ReleaseSRWLockShared(&g_subclassLock);

    if (alreadyRegistered)
        return true;

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

static int ScaleForWindow(
    HWND hwnd,
    int value
)
{
    const UINT dpi =
        hwnd ? GetDpiForWindow(hwnd) : 96;

    return MulDiv(
        value,
        dpi ? static_cast<int>(dpi) : 96,
        96
    );
}

static bool GetBottomStatusRowRect(
    HWND hwnd,
    RECT* row
)
{
    if (!hwnd || !row || !IsWindow(hwnd))
        return false;

    RECT client{};

    if (!GetClientRect(hwnd, &client))
        return false;

    *row = client;
    row->top =
        client.bottom > ScaleForWindow(hwnd, kStatusRowHeight)
            ? client.bottom - ScaleForWindow(hwnd, kStatusRowHeight)
            : 0;

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

    if (!GetBottomStatusRowRect(hwnd, &row))
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

struct StableThemeSnapshot
{
    COLORREF rowBackground = CLR_INVALID;
    COLORREF nativeTextColor = CLR_INVALID;
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
    HWND hwnd
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

static StableThemeSnapshot GetStableThemeStateSnapshot(
    HWND hwnd
)
{
    StableThemeSnapshot result;

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
            existing->stableRowBackground;

        result.nativeTextColor =
            existing->stableNativeTextColor;
    }

    ReleaseSRWLockShared(&g_subclassLock);
    return result;
}

static void UpdateStableThemeState(
    HWND hwnd,
    COLORREF rowBackground,
    bool updateBackground,
    COLORREF nativeTextColor,
    bool updateTextColor
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
        if (updateBackground)
            existing->stableRowBackground = rowBackground;

        if (updateTextColor)
            existing->stableNativeTextColor = nativeTextColor;
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

    const int currentUsableRight =
        currentClientAvailable
            ? (
                client.right > ScaleForWindow(hwnd, 220)
                    ? static_cast<int>(
                        client.right - ScaleForWindow(hwnd, 220)
                    )
                    : static_cast<int>(client.right)
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
        contentGroup = L"Loading...";
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
    const ContentRefreshCache& contentRefreshCache,
    const SingleFileMetadataCache& metadataCache
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

    cache->contentRefresh = contentRefreshCache;
    cache->metadata = metadataCache;

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
            if (
                findData.dwFileAttributes &
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
    IShellItem* item,
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
            item,
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
    IShellBrowser* browser
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

    ULONGLONG freeBytes = 0;
    ULONGLONG driveTotalBytes = 0;
    wchar_t driveLetter = L'?';

    if (
        settings.showDrive &&
        currentPath.length() >= 2 &&
        currentPath[1] == L':'
    )
    {
        ULARGE_INTEGER freeAvailable{};
        ULARGE_INTEGER totalBytes{};

        if (
            GetDiskFreeSpaceExW(
                currentPath.c_str(),
                &freeAvailable,
                &totalBytes,
                nullptr
            )
        )
        {
            freeBytes = freeAvailable.QuadPart;
            driveTotalBytes = totalBytes.QuadPart;
        }

        driveLetter =
            static_cast<wchar_t>(
                towupper(currentPath[0])
            );
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

    const ULONGLONG now = GetTickCount64();

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

        const bool retryThrottled =
            folderIdentity ==
                contentCache.lastAttemptFolderIdentity &&
            now - contentCache.lastScanAttemptTick <
                kContentFailedRetryMs;

        if (
            (
                !sameFolder ||
                itemCountChanged
            ) &&
            !retryThrottled
        )
        {
            contentCache.lastAttemptFolderIdentity = folderIdentity;
            contentCache.lastScanAttemptTick = now;

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
            }

        }
    }

    const bool useCachedContent =
        settings.showContent &&
        contentCache.valid &&
        !folderIdentity.empty() &&
        folderIdentity == contentCache.folderIdentity;

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
    int selectedFiles = 0;
    int selectedFolders = 0;
    ULONGLONG selectedBytes = 0;
    std::wstring selectionOverrideText;
    std::wstring singleFileDetails;
    bool keepSingleFileMetadataCache = false;

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

            const bool enumerateSelection =
                selectionCount <= kMaxDetailedSelectionItems &&
                (
                    settings.showSelection ||
                    (
                        settings.singleFileDetails &&
                        selectionCount == 1
                    )
                );

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

                    PWSTR path = nullptr;

                    if (
                        SUCCEEDED(
                            item->GetDisplayName(
                                SIGDN_FILESYSPATH,
                                &path
                            )
                        ) &&
                        path
                    )
                    {
                        bool directory = false;
                        ULONGLONG size = 0;

                        if (
                            GetFilesystemInfo(
                                path,
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
                                            item,
                                            path
                                        );
                                }
                            }
                        }

                        CoTaskMemFree(path);
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
            contentCache,
            metadataCache
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
    };

    while (true)
    {
        std::vector<WorkerTarget> targets;

        bool snapshotFailed = false;

        AcquireSRWLockShared(&g_subclassLock);

        try
        {
            targets.reserve(g_trackedWindows.size());

            for (const TrackedDirectUiState& state : g_trackedWindows)
            {
                if (state.shellBrowserCookie)
                {
                    targets.push_back({
                        state.hwnd,
                        state.shellBrowserCookie
                    });
                }
            }
        }
        catch (...)
        {
            snapshotFailed = true;
        }

        ReleaseSRWLockShared(&g_subclassLock);

        if (snapshotFailed)
        {
            Wh_Log(L"Worker target snapshot failed");
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
                    browser
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

    HWND current = GetParent(hwnd);
    bool sawDuiView = false;
    bool sawShellTab = false;

    while (current)
    {
        wchar_t cls[128] = {};
        if (GetClassNameW(current, cls, ARRAYSIZE(cls)))
        {
            if (!sawDuiView && wcscmp(cls, L"DUIViewWndClassName") == 0)
                sawDuiView = true;
            else if (sawDuiView && wcscmp(cls, L"ShellTabWindowClass") == 0)
                sawShellTab = true;
            else if (sawShellTab && wcscmp(cls, L"CabinetWClass") == 0)
            {
                DWORD pid = 0;
                GetWindowThreadProcessId(current, &pid);
                return pid == g_pid;
            }
        }

        current = GetParent(current);
    }

    return false;
}

static bool StatusMarkIsFresh(
    HDC source
)
{
    if (!g_statusSourceDc)
        return false;

    ULONGLONG now =
        GetTickCount64();

    if (
        now - g_statusMarkTick >
        kStatusMarkerLifetimeMs
    )
    {
        g_statusSourceDc = nullptr;
        return false;
    }

    return source ==
        g_statusSourceDc;
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

static COLORREF PickBackgroundColor(
    HDC hdc,
    HWND hwnd,
    const RECT& row,
    int selected
)
{
    // Keep the native unselected status-row background stable per window.
    // Explorer can temporarily tint the row while items are selected.
    const StableThemeSnapshot stable =
        GetStableThemeStateSnapshot(hwnd);

    if (
        selected > 0 &&
        stable.rowBackground != CLR_INVALID
    )
    {
        return stable.rowBackground;
    }

    const int y =
        row.top +
        ((row.bottom - row.top) / 2);

    const int samples[] =
    {
        ScaleForWindow(hwnd, 420),
        ScaleForWindow(hwnd, 520),
        ScaleForWindow(hwnd, 620),
        ScaleForWindow(hwnd, 720)
    };

    COLORREF chosen = CLR_INVALID;

    for (int x : samples)
    {
        if (x >= row.right)
            continue;

        const COLORREF sample =
            GetPixel(
                hdc,
                x,
                y
            );

        if (sample != CLR_INVALID)
        {
            chosen = sample;
            break;
        }
    }

    if (chosen == CLR_INVALID)
    {
        if (stable.rowBackground != CLR_INVALID)
            return stable.rowBackground;

        chosen = RGB(32, 32, 32);
    }

    if (selected <= 0)
    {
        UpdateStableThemeState(
            hwnd,
            chosen,
            true,
            CLR_INVALID,
            false
        );
    }

    return chosen;
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

    DrawTextW_Original(
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

static COLORREF PickNativeTextColor(
    HDC hdc,
    HWND hwnd,
    COLORREF background,
    int selected
)
{
    const StableThemeSnapshot stable =
        GetStableThemeStateSnapshot(hwnd);

    if (
        selected > 0 &&
        stable.nativeTextColor != CLR_INVALID
    )
    {
        return stable.nativeTextColor;
    }

    COLORREF candidate =
        GetTextColor(hdc);

    if (
        candidate == CLR_INVALID ||
        std::abs(
            ColorLuminance(candidate) -
            ColorLuminance(background)
        ) < 70
    )
    {
        candidate =
            GetContrastingTextColor(
                background
            );
    }

    if (selected <= 0)
    {
        UpdateStableThemeState(
            hwnd,
            CLR_INVALID,
            false,
            candidate,
            true
        );
    }

    return candidate;
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

    RECT client{};

    if (!GetClientRect(
            hwnd,
            &client))
    {
        return;
    }

    RECT row =
        g_statusRowRect;

    // Fall back to Explorer's bottom status-row height if the native rect is unavailable.
    if (
        row.bottom <= row.top ||
        row.top < 0 ||
        row.bottom >
            client.bottom + ScaleForWindow(hwnd, 2)
    )
    {
        row.top =
            client.bottom > ScaleForWindow(hwnd, kStatusRowHeight)
                ? client.bottom - ScaleForWindow(hwnd, kStatusRowHeight)
                : 0;

        row.bottom =
            client.bottom;
    }

    row.left = ScaleForWindow(hwnd, 6);

    // Preserve Explorer's right-side controls.
    row.right =
        client.right > ScaleForWindow(hwnd, 220)
            ? client.right - ScaleForWindow(hwnd, 220)
            : client.right;

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
        row;

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
            &row,
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

    // Match Explorer's native info-bar font metrics.
    HFONT font =
        CreateFontW(
            -ScaleForWindow(hwnd, 12),
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
        PickNativeTextColor(
            hdc,
            hwnd,
            background,
            selected
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
        ScaleForWindow(hwnd, 14);

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
            ScaleForWindow(hwnd, cards ? 10 : 12);

        const int gap =
            ScaleForWindow(hwnd, cards ? 8 : 6);

        // Flat panes should begin flush with the left edge.
        // Cards keep a tiny 2 px inset so the rounded border isn't clipped.
        int paneX =
            cards
                ? row.left + ScaleForWindow(hwnd, 2)
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
                row.top + ScaleForWindow(hwnd, cards ? 3 : 1),
                paneX + width,
                row.bottom - ScaleForWindow(hwnd, cards ? 3 : 1)
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
                        ScaleForWindow(hwnd, 6),
                        ScaleForWindow(hwnd, 6)
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

            DrawTextW_Original(
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

    if (font)
    {
        DeleteObject(
            font
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
        !hwnd ||
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        return DirectUiSubclassResult::Failed;
    }

    DWORD hwndThread =
        GetWindowThreadProcessId(
            hwnd,
            nullptr
        );

    if (
        hwndThread !=
        GetCurrentThreadId()
    )
    {
        Wh_Log(
            L"DirectUI subclass skipped: wrong thread hwnd=%p hwndTid=%lu currentTid=%lu",
            hwnd,
            hwndThread,
            GetCurrentThreadId()
        );

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
        !WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd,
            DirectUiSubclassProc,
            0
        )
    )
    {
        ReleaseSRWLockExclusive(
            &g_subclassLock
        );

        Wh_Log(
            L"DirectUI subclass install failed hwnd=%p error=%lu",
            hwnd,
            GetLastError()
        );

        return DirectUiSubclassResult::Failed;
    }

    try
    {
        TrackedDirectUiState state;
        state.hwnd = hwnd;
        g_trackedWindows.push_back(
            std::move(state)
        );
    }
    catch (...)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hwnd,
            DirectUiSubclassProc
        );

        ReleaseSRWLockExclusive(
            &g_subclassLock
        );

        Wh_Log(
            L"DirectUI subclass tracking failed hwnd=%p",
            hwnd
        );

        return DirectUiSubclassResult::Failed;
    }

    ReleaseSRWLockExclusive(
        &g_subclassLock
    );

    EnsureWindowDataCache(hwnd);
    EnsureShellBrowserRegistration(hwnd);

    // Run the follow-up invalidation after the native paint that installed us.
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
            InvalidateInfoBarWindow(hwnd);

        return 0;
    }

    if (msg == WM_NCDESTROY)
    {
        const DWORD shellBrowserCookie =
            GetShellBrowserCookieSnapshot(hwnd);

        EraseWindowDataCache(hwnd);
        RevokeShellBrowserCookie(shellBrowserCookie);

        AcquireSRWLockExclusive(&g_subclassLock);
        UntrackDirectUiWindowLocked(hwnd);
        ReleaseSRWLockExclusive(&g_subclassLock);

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

    if (msg == WM_PAINT)
    {
        EnsureWindowDataCache(hwnd);
        EnsureShellBrowserRegistration(hwnd);

        RECT updateRect{};

        const BOOL hasUpdateRect =
            GetUpdateRect(
                hwnd,
                &updateRect,
                FALSE
            );

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
            g_insideFinalPaint =
                true;

            PaintFinalInfoBar(
                hdc,
                hwnd,
                hasUpdateRect
                    ? &updateRect
                    : nullptr
            );

            g_insideFinalPaint =
                false;

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
// DrawText marker hook
// ============================================================

static int WINAPI DrawTextW_Hook(
    HDC hdc,
    LPCWSTR text,
    int count,
    LPRECT rect,
    UINT format
)
{
    if (!DrawTextW_Original)
        return 0;

    // Language-independent marker: Explorer's native information row uses a
    // distinctive DrawText format. Don't inspect the localized text at all.
    // The BitBlt hook below performs the decisive validation by mapping this
    // source rect into the destination DirectUIHWND and requiring it to land
    // at the bottom of that Explorer view.
    if (
        !g_insideFinalPaint &&
        rect &&
        format == kNativeStatusTextFormat &&
        rect->bottom > rect->top &&
        rect->right > rect->left
    )
    {
        g_statusSourceDc = hdc;
        g_statusMarkTick = GetTickCount64();
        g_statusRowRect = *rect;
    }

    return DrawTextW_Original(
        hdc,
        text,
        count,
        rect,
        format
    );
}

// ============================================================
// Final DirectUI copy hook
// ============================================================

static BOOL WINAPI BitBlt_Hook(
    HDC hdcDest,
    int xDest,
    int yDest,
    int width,
    int height,
    HDC hdcSrc,
    int xSrc,
    int ySrc,
    DWORD rop
)
{
    bool relevant =
        !g_insideFinalPaint &&
        StatusMarkIsFresh(
            hdcSrc
        );

    BOOL result =
        BitBlt_Original(
            hdcDest,
            xDest,
            yDest,
            width,
            height,
            hdcSrc,
            xSrc,
            ySrc,
            rop
        );

    if (!relevant)
        return result;

    HWND hwnd =
        WindowFromDC(
            hdcDest
        );

    if (!IsExplorerDirectUiTarget(hwnd))
        return result;

    // Map the candidate source text rect into destination coordinates and only
    // accept it when it actually lands on Explorer's bottom information row.
    RECT client{};
    if (!GetClientRect(hwnd, &client))
        return result;

    RECT mappedRow = g_statusRowRect;
    mappedRow.top = yDest + (g_statusRowRect.top - ySrc);
    mappedRow.bottom = yDest + (g_statusRowRect.bottom - ySrc);
    mappedRow.left = xDest + (g_statusRowRect.left - xSrc);
    mappedRow.right = xDest + (g_statusRowRect.right - xSrc);

    const int rowHeight = mappedRow.bottom - mappedRow.top;
    const int expectedHeight = ScaleForWindow(hwnd, kStatusRowHeight);
    const int bottomTolerance = ScaleForWindow(hwnd, 4);

    if (
        rowHeight < ScaleForWindow(hwnd, 16) ||
        rowHeight > ScaleForWindow(hwnd, 34) ||
        std::abs(mappedRow.bottom - client.bottom) > bottomTolerance ||
        std::abs(rowHeight - expectedHeight) > ScaleForWindow(hwnd, 10)
    )
    {
        return result;
    }

    g_statusRowRect = mappedRow;

    if (
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        return result;
    }

    // This hook runs on DirectUI's UI thread, which is required for subclassing.
    // Install the subclass here so every future WM_PAINT ends with our row.
    const DirectUiSubclassResult subclassResult =
        EnsureDirectUiSubclass(
            hwnd
        );

    // The subclass cannot retroactively catch the WM_PAINT that is already
    // in progress when it is first installed, so finish this current frame
    // once directly after the relevant BitBlt.
    if (
        subclassResult ==
        DirectUiSubclassResult::NewlyInstalled
    )
    {
        g_insideFinalPaint =
            true;

        PaintFinalInfoBar(
            hdcDest,
            hwnd
        );

        g_insideFinalPaint =
            false;
    }

    return result;
}


// ============================================================
// Windhawk lifecycle
// ============================================================

struct ActivateExistingExplorerContext
{
    DWORD pid;
};

static BOOL CALLBACK ActivateDirectUiChildProc(
    HWND hwnd,
    LPARAM lParam
)
{
    auto* context =
        reinterpret_cast<ActivateExistingExplorerContext*>(
            lParam
        );

    if (
        g_unloading.load(std::memory_order_acquire) ||
        !IsWindowVisible(hwnd)
    )
    {
        return TRUE;
    }

    DWORD pid = 0;

    GetWindowThreadProcessId(
        hwnd,
        &pid
    );

    if (pid != context->pid)
        return TRUE;

    wchar_t className[128] = {};

    if (
        GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) &&
        wcscmp(className, L"DirectUIHWND") == 0
    )
    {
        // The resulting native paint reaches BitBlt_Hook on this window's UI
        // thread, where the existing correlation safely selects the target.
        InvalidateInfoBarWindow(hwnd);
    }

    return TRUE;
}

static BOOL CALLBACK ActivateExistingExplorerProc(
    HWND hwnd,
    LPARAM lParam
)
{
    auto* context =
        reinterpret_cast<ActivateExistingExplorerContext*>(
            lParam
        );

    if (g_unloading.load(std::memory_order_acquire))
        return FALSE;

    DWORD pid = 0;

    GetWindowThreadProcessId(
        hwnd,
        &pid
    );

    if (
        pid != context->pid ||
        !IsWindowVisible(hwnd)
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
        ActivateDirectUiChildProc,
        lParam
    );

    return TRUE;
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

    HMODULE user32 =
        GetModuleHandleW(
            L"user32.dll"
        );

    HMODULE gdi32 =
        GetModuleHandleW(
            L"gdi32.dll"
        );

    if (!user32 || !gdi32)
    {
        Wh_Log(
            L"required system module not loaded"
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    void* drawTextTarget =
        reinterpret_cast<void*>(
            GetProcAddress(
                user32,
                "DrawTextW"
            )
        );

    void* bitBltTarget =
        reinterpret_cast<void*>(
            GetProcAddress(
                gdi32,
                "BitBlt"
            )
        );

    if (!drawTextTarget || !bitBltTarget)
    {
        Wh_Log(
            L"required GDI targets not found"
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    if (
        !Wh_SetFunctionHook(
            drawTextTarget,
            reinterpret_cast<void*>(
                DrawTextW_Hook
            ),
            reinterpret_cast<void**>(
                &DrawTextW_Original
            )
        )
    )
    {
        Wh_Log(
            L"DrawTextW hook failed"
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    if (
        !Wh_SetFunctionHook(
            bitBltTarget,
            reinterpret_cast<void*>(
                BitBlt_Hook
            ),
            reinterpret_cast<void**>(
                &BitBlt_Original
            )
        )
    )
    {
        Wh_Log(
            L"BitBlt hook failed"
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

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
    ActivateExistingExplorerContext context{
        g_pid
    };

    EnumWindows(
        ActivateExistingExplorerProc,
        reinterpret_cast<LPARAM>(
            &context
        )
    );

}


void Wh_ModSettingsChanged()
{
    LoadSettings();

    if (g_workerWakeEvent)
        SetEvent(g_workerWakeEvent);

    RefreshInfoBars();
}

void Wh_ModBeforeUninit()
{
    g_unloading.store(
        true,
        std::memory_order_release
    );

    if (g_stopEvent)
        SetEvent(g_stopEvent);

    if (g_workerThreadId)
        CoCancelCall(g_workerThreadId, 0);

    // Snapshot under the lock, then remove subclasses outside it. The
    // Windhawk helper handles cross-thread subclass removal without the
    // custom retry/message/barrier machinery that could previously loop
    // forever on a hung Explorer UI thread.
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
        windows.clear();
    }
    ReleaseSRWLockShared(&g_subclassLock);

    for (HWND hwnd : windows)
    {
        if (hwnd && IsWindow(hwnd))
        {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hwnd,
                DirectUiSubclassProc
            );
        }

        DWORD shellBrowserCookie = 0;
        AcquireSRWLockExclusive(&g_subclassLock);
        shellBrowserCookie = UntrackDirectUiWindowLocked(hwnd);
        ReleaseSRWLockExclusive(&g_subclassLock);

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

    if (g_workerThreadId)
        CoCancelCall(g_workerThreadId, 0);

    if (g_workerThread)
    {
        // The cache and COM state must outlive the worker. Wait until the
        // worker has fully stopped before releasing shared resources.
        const DWORD workerWait =
            WaitForSingleObject(
                g_workerThread,
                5000
            );

        if (workerWait == WAIT_TIMEOUT)
        {
            Wh_Log(L"Worker did not stop within 5 seconds; terminating it to avoid blocking mod unload");
            TerminateThread(g_workerThread, 0);
            WaitForSingleObject(g_workerThread, 1000);
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
