// ==WindhawkMod==
// @id              alt-tab-plus
// @name            Alt+Tab Plus
// @description     Adds common programs and recently modified files to the Windows 11 Alt+Tab overlay
// @version         0.3.1
// @author          BlueFinch
// @github 			BlueFinch3000
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lversion -ldwmapi -lgdi32
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
//
// The XAML diagnostics attachment pattern is adapted from Windows 11 Taskbar
// Styler by m417z and the ExplorerTAP work it credits.

// ==WindhawkModReadme==
/*
# Alt+Tab Plus

Alt+Tab Plus augments the native Windows 11 Alt+Tab overlay with two clickable
sections:

- **Common programs**, ranked from Windows' per-user app-switch usage data and
  the mod's local foreground-app history, with optional manually configured
  programs pinned first.
- **Recent files**, ranked by last-modified time from folders you choose.
  Alternatively, recent files can be grouped by up to four programs using
  those programs' Windows Jump Lists.

The native open-window list remains in the middle, so normal Alt+Tab keyboard
behavior is unchanged. The added cards can be opened with the mouse.

## Layouts

- **Columns**: common programs on the left, open windows in the center, recent
  files on the right. The optional adjusted thumbnail area keeps the native
  Alt+Tab thumbnails inside the space between the two side panels.
- **Top strip**: common programs and recent files above open windows.
- **Bottom strip**: open windows above common programs and recent files.

## Privacy and performance

All ranking and file scanning happens locally. The mod never sends data over the
network. File scanning runs on a background thread and is bounded by the maximum
depth and item limits in Settings. Hidden, system, reparse-point, and temporary
files are skipped.

## Compatibility

This mod supports the Windows 11 XAML Alt+Tab experience. It doesn't support the
classic Windows 10 switcher or third-party switcher replacements.

The default companion-window renderer doesn't use XAML diagnostics and can
coexist with Windows 11 Taskbar Styler, File Explorer Styler, UWPSpy,
ExplorerBlurMica, and TranslucentTB. An optional integrated XAML renderer is
available, but Windows allows only one XAML diagnostics consumer per process.

After installing the source as a local Windhawk mod, restart Explorer once if
the cards don't appear on the first Alt+Tab invocation.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- renderMode: companion
  $name: Renderer
  $description: Companion is compatible with other XAML styling mods. Integrated XAML is experimental and requires exclusive access to XAML diagnostics.
  $options:
  - companion: Compatible companion panels
  - xaml: Integrated XAML panels (experimental)
- layout: columns
  $name: Layout
  $description: Placement of common programs, open windows, and recent files.
  $options:
  - columns: Columns (programs / windows / files)
  - top: Top strip (programs + files above windows)
  - bottom: Bottom strip (windows above programs + files)
- columnThumbnailArea: adjusted
  $name: Column thumbnail area
  $description: In Columns layout, Adjusted constrains the native Alt+Tab thumbnails to the space between the inner edges of the side panels. Windows default leaves their native size and position unchanged.
  $options:
  - default: Windows default
  - adjusted: Adjusted between panels
- commonProgramCount: 5
  $name: Common program count
  $description: Maximum number of common programs shown in a section.
- recentFileCount: 5
  $name: Recent file count
  $description: Maximum number of recently modified files shown when Recent files source is All.
- recentFilesSource: all
  $name: Recent files source
  $description: All scans the configured folders. Program specific reads the Windows Recent Jump List for each configured program.
  $options:
  - all: All
  - programSpecific: Program specific
- recentPrograms:
  - - name: ""
      $name: Program name
      $description: Label shown above this program's recent files.
    - appId: ""
      $name: AppUserModelID
      $description: The program's Windows AppUserModelID, as used by its Jump List.
    - recentFileCount: 5
      $name: Recent files
      $description: Number of recent files to request for this program.
  $name: Programs for program-specific recent files
  $description: Configure one program plus up to three additional programs. Entries after the fourth are ignored.
- learnFromWindowsUsage: true
  $name: Rank programs automatically
  $description: Use Windows' counts and local app-switch history to rank programs.
- commonPrograms:
  - - name: ""
      $name: Name
    - command: ""
      $name: Command
      $description: Executable, file, folder, URL, or shell:AppsFolder command.
    - arguments: ""
      $name: Arguments
    - workingDirectory: ""
      $name: Working directory
  $name: Pinned common programs
  $description: Optional programs that are always listed before automatic results.
- recentFolders:
  - "%USERPROFILE%\\Desktop"
  - "%USERPROFILE%\\Documents"
  - "%USERPROFILE%\\Downloads"
  $name: Folders for recent files
  $description: Environment variables are supported. Subfolders are scanned.
- recentExtensions: "doc;docx;xls;xlsx;ppt;pptx;pdf;txt;rtf;md;csv;png;jpg;jpeg;gif;webp;svg;zip;7z;cpp;c;h;hpp;cs;js;jsx;ts;tsx;py;rs;go;java;json;yaml;yml;html;css"
  $name: Recent file extensions
  $description: Semicolon- or comma-separated extensions without dots. Use * for all files.
- scanMaxDepth: 5
  $name: Scan depth
  $description: Maximum number of subfolder levels scanned below each recent folder.
- scanMaxItems: 20000
  $name: Scan item limit
  $description: Maximum files and folders examined during each refresh.
- refreshSeconds: 60
  $name: Refresh interval
  $description: Seconds between background refreshes of programs and recent files.
- panelWidth: 270
  $name: Side panel width
  $description: Width in pixels of each card in the Columns layout.
*/
// ==/WindhawkModSettings==

// ==WindhawkModChangelog==
/*
## 0.3.1

- Fixed adjusted column boundaries by constraining the actual native Alt+Tab
  XAML host window in addition to the internal positioning hook.
- Reapplies the corridor while Alt+Tab is active so later Windows layout passes
  can't restore the full-screen thumbnail area.

## 0.3.0

- Added an adjustable native thumbnail area for the Columns layout.
- Added All and Program specific recent-file sources.
- Added up to four program-specific Jump List sections with an independent
  recent-file count for each program.

## 0.2.0

- Added compatible Win32 companion panels as the default renderer.
- Fixed compatibility with Windows 11 Taskbar Styler and File Explorer Styler,
  which already occupy Explorer's XAML diagnostics connection.
- Kept the integrated XAML renderer as an optional experimental setting.

## 0.1.0

- Initial implementation.
*/
// ==/WindhawkModChangelog==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <xamlom.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

namespace wf = winrt::Windows::Foundation;
namespace wut = winrt::Windows::UI::Text;
namespace wuv = winrt::Windows::UI::ViewManagement;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxa = winrt::Windows::UI::Xaml::Automation;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

using namespace std::string_view_literals;

enum class LayoutMode {
    Columns,
    Top,
    Bottom,
};

enum class RenderMode {
    Companion,
    Xaml,
};

enum class ColumnThumbnailArea {
    WindowsDefault,
    Adjusted,
};

enum class RecentFilesSource {
    All,
    ProgramSpecific,
};

struct ProgramSetting {
    std::wstring name;
    std::wstring command;
    std::wstring arguments;
    std::wstring workingDirectory;
};

struct RecentProgramSetting {
    std::wstring name;
    std::wstring appId;
    int recentFileCount = 5;
};

struct Settings {
    RenderMode renderMode = RenderMode::Companion;
    LayoutMode layout = LayoutMode::Columns;
    ColumnThumbnailArea columnThumbnailArea =
        ColumnThumbnailArea::Adjusted;
    int commonProgramCount = 5;
    int recentFileCount = 5;
    RecentFilesSource recentFilesSource = RecentFilesSource::All;
    std::vector<RecentProgramSetting> recentPrograms;
    bool learnFromWindowsUsage = true;
    std::vector<ProgramSetting> commonPrograms;
    std::vector<std::wstring> recentFolders;
    std::unordered_set<std::wstring> recentExtensions;
    bool allExtensions = false;
    int scanMaxDepth = 5;
    int scanMaxItems = 20000;
    int refreshSeconds = 60;
    int panelWidth = 270;
};

struct LaunchEntry {
    std::wstring title;
    std::wstring subtitle;
    std::wstring command;
    std::wstring arguments;
    std::wstring workingDirectory;
    ULONGLONG score = 0;
    bool file = false;
};

struct RecentFileGroup {
    std::wstring title;
    std::vector<LaunchEntry> entries;
};

struct UsageRecord {
    std::wstring path;
    ULONGLONG switches = 0;
    ULONGLONG lastSeen = 0;
};

std::mutex g_settingsMutex;
Settings g_settings;

std::mutex g_entriesMutex;
std::vector<LaunchEntry> g_commonPrograms;
std::vector<LaunchEntry> g_recentFiles;
std::vector<RecentFileGroup> g_recentFileGroups;
std::atomic<ULONGLONG> g_entriesGeneration = 0;

std::mutex g_usageMutex;
std::vector<UsageRecord> g_usageRecords;
std::wstring g_lastForegroundProgram;

std::atomic<bool> g_stopWorker = false;
HANDLE g_workerWakeEvent = nullptr;
std::thread g_worker;

template <auto Fn>
struct FunctionDeleter {
    template <typename T>
    void operator()(T* value) const {
        Fn(value);
    }
};

using StringSettingPtr =
    std::unique_ptr<const WCHAR[],
                    FunctionDeleter<Wh_FreeStringSetting>>;

std::wstring GetStringSetting(PCWSTR name) {
    StringSettingPtr value(Wh_GetStringSetting(name));
    return value ? std::wstring(value.get()) : std::wstring();
}

std::wstring GetIndexedStringSetting(PCWSTR format, int index) {
    StringSettingPtr value(Wh_GetStringSetting(format, index));
    return value ? std::wstring(value.get()) : std::wstring();
}

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return std::towlower(ch); });
    return value;
}

std::wstring Trim(std::wstring value) {
    auto isSpace = [](wchar_t ch) { return std::iswspace(ch) != 0; };
    value.erase(value.begin(),
                std::find_if_not(value.begin(), value.end(), isSpace));
    value.erase(
        std::find_if_not(value.rbegin(), value.rend(), isSpace).base(),
        value.end());
    return value;
}

std::wstring ExpandEnvironment(const std::wstring& value) {
    if (value.empty()) {
        return value;
    }

    DWORD size = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (!size) {
        return value;
    }

    std::wstring expanded(size, L'\0');
    DWORD written =
        ExpandEnvironmentStringsW(value.c_str(), expanded.data(), size);
    if (!written || written > size) {
        return value;
    }

    expanded.resize(written - 1);
    return expanded;
}

std::wstring FileNameFromPath(const std::wstring& value) {
    const size_t separator = value.find_last_of(L"\\/");
    return separator == std::wstring::npos ? value
                                           : value.substr(separator + 1);
}

std::wstring DirectoryNameFromPath(const std::wstring& value) {
    const size_t separator = value.find_last_of(L"\\/");
    if (separator == std::wstring::npos) {
        return {};
    }

    std::wstring directory = value.substr(0, separator);
    return FileNameFromPath(directory);
}

std::wstring RemoveExtension(std::wstring value) {
    const size_t dot = value.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        value.resize(dot);
    }
    return value;
}

bool PathIsExistingFile(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool PathIsExistingDirectory(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring GetExecutableDescription(const std::wstring& path) {
    DWORD ignored = 0;
    DWORD versionSize =
        GetFileVersionInfoSizeW(path.c_str(), &ignored);
    if (!versionSize) {
        return {};
    }

    std::vector<BYTE> versionData(versionSize);
    if (!GetFileVersionInfoW(path.c_str(), 0, versionSize,
                             versionData.data())) {
        return {};
    }

    struct LanguageAndCodePage {
        WORD language;
        WORD codePage;
    };

    LanguageAndCodePage* translations = nullptr;
    UINT translationsSize = 0;
    if (!VerQueryValueW(versionData.data(), L"\\VarFileInfo\\Translation",
                        reinterpret_cast<void**>(&translations),
                        &translationsSize) ||
        translationsSize < sizeof(LanguageAndCodePage)) {
        return {};
    }

    WCHAR query[96];
    _snwprintf_s(query, _TRUNCATE,
                 L"\\StringFileInfo\\%04x%04x\\FileDescription",
                 translations[0].language, translations[0].codePage);

    WCHAR* description = nullptr;
    UINT descriptionSize = 0;
    if (!VerQueryValueW(versionData.data(), query,
                        reinterpret_cast<void**>(&description),
                        &descriptionSize) ||
        !description || !descriptionSize) {
        return {};
    }

    return Trim(description);
}

std::wstring FriendlyProgramName(const std::wstring& command) {
    if (command.rfind(L"shell:AppsFolder\\", 0) == 0) {
        std::wstring appId =
            command.substr(std::wstring_view(L"shell:AppsFolder\\").size());
        size_t bang = appId.find(L'!');
        if (bang != std::wstring::npos) {
            appId.resize(bang);
        }
        size_t underscore = appId.find(L'_');
        if (underscore != std::wstring::npos) {
            appId.resize(underscore);
        }
        if (appId.rfind(L"Microsoft.", 0) == 0) {
            appId.erase(0, std::wstring_view(L"Microsoft.").size());
        }
        return appId.empty() ? L"App" : appId;
    }

    std::wstring expanded = ExpandEnvironment(command);
    if (PathIsExistingFile(expanded)) {
        std::wstring description = GetExecutableDescription(expanded);
        if (!description.empty()) {
            return description;
        }
    }

    std::wstring name = RemoveExtension(FileNameFromPath(expanded));
    return name.empty() ? command : name;
}

std::unordered_set<std::wstring> ParseExtensions(
    const std::wstring& value,
    bool* allExtensions) {
    std::unordered_set<std::wstring> result;
    *allExtensions = false;

    size_t start = 0;
    while (start <= value.size()) {
        size_t end = value.find_first_of(L";,", start);
        std::wstring extension =
            Trim(value.substr(start, end == std::wstring::npos
                                        ? std::wstring::npos
                                        : end - start));
        if (!extension.empty() && extension.front() == L'.') {
            extension.erase(extension.begin());
        }
        extension = ToLower(std::move(extension));

        if (extension == L"*") {
            *allExtensions = true;
            result.clear();
            return result;
        }
        if (!extension.empty()) {
            result.insert(std::move(extension));
        }

        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }

    return result;
}

void LoadSettings() {
    Settings settings;

    if (GetStringSetting(L"renderMode") == L"xaml") {
        settings.renderMode = RenderMode::Xaml;
    }

    std::wstring layout = GetStringSetting(L"layout");
    if (layout == L"top") {
        settings.layout = LayoutMode::Top;
    } else if (layout == L"bottom") {
        settings.layout = LayoutMode::Bottom;
    }

    if (GetStringSetting(L"columnThumbnailArea") == L"default") {
        settings.columnThumbnailArea =
            ColumnThumbnailArea::WindowsDefault;
    }
    if (GetStringSetting(L"recentFilesSource") ==
        L"programSpecific") {
        settings.recentFilesSource =
            RecentFilesSource::ProgramSpecific;
    }

    settings.commonProgramCount =
        std::clamp(Wh_GetIntSetting(L"commonProgramCount"), 0, 12);
    settings.recentFileCount =
        std::clamp(Wh_GetIntSetting(L"recentFileCount"), 0, 12);
    settings.learnFromWindowsUsage =
        Wh_GetIntSetting(L"learnFromWindowsUsage") != 0;
    settings.scanMaxDepth =
        std::clamp(Wh_GetIntSetting(L"scanMaxDepth"), 0, 12);
    settings.scanMaxItems =
        std::clamp(Wh_GetIntSetting(L"scanMaxItems"), 100, 100000);
    settings.refreshSeconds =
        std::clamp(Wh_GetIntSetting(L"refreshSeconds"), 10, 3600);
    settings.panelWidth =
        std::clamp(Wh_GetIntSetting(L"panelWidth"), 210, 420);

    for (int index = 0;; index++) {
        ProgramSetting program;
        program.command = ExpandEnvironment(
            GetIndexedStringSetting(L"commonPrograms[%d].command", index));
        if (program.command.empty()) {
            break;
        }
        program.name =
            GetIndexedStringSetting(L"commonPrograms[%d].name", index);
        program.arguments = ExpandEnvironment(
            GetIndexedStringSetting(L"commonPrograms[%d].arguments", index));
        program.workingDirectory = ExpandEnvironment(GetIndexedStringSetting(
            L"commonPrograms[%d].workingDirectory", index));
        settings.commonPrograms.push_back(std::move(program));
    }

    for (int index = 0; index < 4; index++) {
        RecentProgramSetting program;
        program.appId = Trim(GetIndexedStringSetting(
            L"recentPrograms[%d].appId", index));
        if (program.appId.empty()) {
            break;
        }
        program.name = Trim(GetIndexedStringSetting(
            L"recentPrograms[%d].name", index));
        if (program.name.empty()) {
            program.name = program.appId;
        }
        program.recentFileCount = std::clamp(
            Wh_GetIntSetting(L"recentPrograms[%d].recentFileCount",
                             index),
            0, 20);
        settings.recentPrograms.push_back(std::move(program));
    }

    for (int index = 0;; index++) {
        std::wstring folder = ExpandEnvironment(
            GetIndexedStringSetting(L"recentFolders[%d]", index));
        if (folder.empty()) {
            break;
        }
        while (folder.size() > 3 &&
               (folder.back() == L'\\' || folder.back() == L'/')) {
            folder.pop_back();
        }
        settings.recentFolders.push_back(std::move(folder));
    }

    settings.recentExtensions = ParseExtensions(
        GetStringSetting(L"recentExtensions"), &settings.allExtensions);

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(settings);
}

bool IsIgnoredProgram(const std::wstring& path) {
    static constexpr std::wstring_view ignoredNames[] = {
        L"applicationframehost.exe"sv,
        L"audiodg.exe"sv,
        L"backgroundtaskhost.exe"sv,
        L"conhost.exe"sv,
        L"ctfmon.exe"sv,
        L"dwm.exe"sv,
        L"lockapp.exe"sv,
        L"searchhost.exe"sv,
        L"shellexperiencehost.exe"sv,
        L"startmenuexperiencehost.exe"sv,
        L"systemsettings.exe"sv,
        L"textinputhost.exe"sv,
    };

    std::wstring fileName = ToLower(FileNameFromPath(path));
    return std::find(std::begin(ignoredNames), std::end(ignoredNames),
                     fileName) != std::end(ignoredNames);
}

struct UsageStoreRecord {
    ULONGLONG switches;
    ULONGLONG lastSeen;
    WCHAR path[520];
};

struct UsageStore {
    DWORD magic;
    DWORD version;
    DWORD count;
    DWORD reserved;
    UsageStoreRecord records[64];
};

constexpr DWORD kUsageStoreMagic = 0x50544157;  // "WATP"
constexpr DWORD kUsageStoreVersion = 1;

void LoadUsageRecords() {
    UsageStore store{};
    if (Wh_GetBinaryValue(L"programUsageV1", &store, sizeof(store)) !=
            sizeof(store) ||
        store.magic != kUsageStoreMagic ||
        store.version != kUsageStoreVersion ||
        store.count > ARRAYSIZE(store.records)) {
        return;
    }

    std::vector<UsageRecord> records;
    records.reserve(store.count);
    for (DWORD index = 0; index < store.count; index++) {
        store.records[index].path[
            ARRAYSIZE(store.records[index].path) - 1] = L'\0';
        if (!store.records[index].path[0] ||
            !store.records[index].switches) {
            continue;
        }
        records.push_back({
            .path = store.records[index].path,
            .switches = store.records[index].switches,
            .lastSeen = store.records[index].lastSeen,
        });
    }

    std::lock_guard lock(g_usageMutex);
    g_usageRecords = std::move(records);
}

void SaveUsageRecords() {
    UsageStore store{
        .magic = kUsageStoreMagic,
        .version = kUsageStoreVersion,
        .count = 0,
        .reserved = 0,
        .records = {},
    };

    {
        std::lock_guard lock(g_usageMutex);
        store.count = static_cast<DWORD>(
            std::min(g_usageRecords.size(),
                     static_cast<size_t>(ARRAYSIZE(store.records))));
        for (DWORD index = 0; index < store.count; index++) {
            const auto& source = g_usageRecords[index];
            auto& destination = store.records[index];
            destination.switches = source.switches;
            destination.lastSeen = source.lastSeen;
            wcsncpy_s(destination.path, source.path.c_str(), _TRUNCATE);
        }
    }

    if (!Wh_SetBinaryValue(L"programUsageV1", &store, sizeof(store))) {
        Wh_Log(L"Failed to persist common-program usage");
    }
}

std::vector<UsageRecord> CopyUsageRecords() {
    std::lock_guard lock(g_usageMutex);
    return g_usageRecords;
}

ULONGLONG CurrentFileTime() {
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    ULARGE_INTEGER value;
    value.LowPart = fileTime.dwLowDateTime;
    value.HighPart = fileTime.dwHighDateTime;
    return value.QuadPart;
}

std::wstring GetForegroundProgramPath() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return {};
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(foreground, &processId);
    if (!processId) {
        return {};
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }

    std::wstring path(32768, L'\0');
    DWORD pathLength = static_cast<DWORD>(path.size());
    BOOL success =
        QueryFullProcessImageNameW(process, 0, path.data(), &pathLength);
    CloseHandle(process);
    if (!success || !pathLength) {
        return {};
    }

    path.resize(pathLength);
    return path;
}

void TrackForegroundProgram() {
    std::wstring path = GetForegroundProgramPath();
    if (path.empty() || IsIgnoredProgram(path)) {
        return;
    }

    std::wstring normalized = ToLower(path);
    if (normalized == g_lastForegroundProgram) {
        return;
    }
    g_lastForegroundProgram = normalized;

    const ULONGLONG now = CurrentFileTime();
    std::lock_guard lock(g_usageMutex);
    auto iterator = std::find_if(
        g_usageRecords.begin(), g_usageRecords.end(),
        [&normalized](const UsageRecord& record) {
            return ToLower(record.path) == normalized;
        });
    if (iterator == g_usageRecords.end()) {
        g_usageRecords.push_back({
            .path = std::move(path),
            .switches = 1,
            .lastSeen = now,
        });
    } else {
        iterator->switches++;
        iterator->lastSeen = now;
    }

    std::stable_sort(
        g_usageRecords.begin(), g_usageRecords.end(),
        [](const UsageRecord& left, const UsageRecord& right) {
            if (left.switches != right.switches) {
                return left.switches > right.switches;
            }
            return left.lastSeen > right.lastSeen;
        });
    if (g_usageRecords.size() > 64) {
        g_usageRecords.resize(64);
    }
}

std::vector<LaunchEntry> DiscoverCommonPrograms(
    const Settings& settings) {
    std::vector<LaunchEntry> result;
    std::unordered_set<std::wstring> seen;

    for (const auto& configured : settings.commonPrograms) {
        std::wstring dedupeKey = ToLower(configured.command);
        if (!seen.insert(dedupeKey).second) {
            continue;
        }

        LaunchEntry entry;
        entry.title = configured.name.empty()
                          ? FriendlyProgramName(configured.command)
                          : configured.name;
        entry.subtitle = L"Pinned";
        entry.command = configured.command;
        entry.arguments = configured.arguments;
        entry.workingDirectory = configured.workingDirectory;
        entry.score = std::numeric_limits<ULONGLONG>::max();
        result.push_back(std::move(entry));
    }

    if (settings.learnFromWindowsUsage) {
        HKEY key = nullptr;
        constexpr PCWSTR kFeatureUsagePath =
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
            L"FeatureUsage\\AppSwitched";
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kFeatureUsagePath, 0,
                          KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
            DWORD valueCount = 0;
            DWORD maximumValueNameLength = 0;
            RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, nullptr, nullptr,
                             nullptr, &valueCount, &maximumValueNameLength,
                             nullptr, nullptr, nullptr);

            std::vector<WCHAR> valueName(maximumValueNameLength + 2);
            for (DWORD index = 0; index < valueCount; index++) {
                DWORD valueNameLength =
                    static_cast<DWORD>(valueName.size());
                DWORD type = 0;
                DWORD score = 0;
                DWORD scoreSize = sizeof(score);
                LONG status = RegEnumValueW(
                    key, index, valueName.data(), &valueNameLength, nullptr,
                    &type, reinterpret_cast<BYTE*>(&score), &scoreSize);
                if (status != ERROR_SUCCESS || type != REG_DWORD || !score) {
                    continue;
                }

                std::wstring path(valueName.data(), valueNameLength);
                path = ExpandEnvironment(path);
                if (!PathIsExistingFile(path) || IsIgnoredProgram(path)) {
                    continue;
                }

                std::wstring dedupeKey = ToLower(path);
                if (!seen.insert(dedupeKey).second) {
                    continue;
                }

                LaunchEntry entry;
                entry.title = FriendlyProgramName(path);
                entry.subtitle = L"Frequently used";
                entry.command = std::move(path);
                entry.score = score;
                result.push_back(std::move(entry));
            }
            RegCloseKey(key);
        }

        for (const auto& usage : CopyUsageRecords()) {
            if (!PathIsExistingFile(usage.path) ||
                IsIgnoredProgram(usage.path)) {
                continue;
            }

            std::wstring normalized = ToLower(usage.path);
            auto existing = std::find_if(
                result.begin(), result.end(),
                [&normalized](const LaunchEntry& entry) {
                    return ToLower(entry.command) == normalized;
                });
            if (existing != result.end()) {
                if (existing->score !=
                    std::numeric_limits<ULONGLONG>::max()) {
                    existing->score += usage.switches;
                }
                continue;
            }

            if (!seen.insert(normalized).second) {
                continue;
            }

            LaunchEntry entry;
            entry.title = FriendlyProgramName(usage.path);
            entry.subtitle = L"Frequently used";
            entry.command = usage.path;
            entry.score = usage.switches;
            result.push_back(std::move(entry));
        }
    }

    const size_t configuredCount = settings.commonPrograms.size();
    if (configuredCount < result.size()) {
        std::stable_sort(
            result.begin() +
                static_cast<std::vector<LaunchEntry>::difference_type>(
                    std::min(configuredCount, result.size())),
            result.end(),
            [](const LaunchEntry& left, const LaunchEntry& right) {
                if (left.score != right.score) {
                    return left.score > right.score;
                }
                return left.title < right.title;
            });
    }

    if (result.size() >
        static_cast<size_t>(settings.commonProgramCount)) {
        result.resize(settings.commonProgramCount);
    }
    return result;
}

ULONGLONG FileTimeToUint64(const FILETIME& fileTime) {
    ULARGE_INTEGER value;
    value.LowPart = fileTime.dwLowDateTime;
    value.HighPart = fileTime.dwHighDateTime;
    return value.QuadPart;
}

bool HasAllowedExtension(const std::wstring& path,
                         const Settings& settings) {
    if (settings.allExtensions) {
        return true;
    }

    std::wstring fileName = FileNameFromPath(path);
    size_t dot = fileName.find_last_of(L'.');
    if (dot == std::wstring::npos || dot + 1 >= fileName.size()) {
        return false;
    }

    return settings.recentExtensions.contains(
        ToLower(fileName.substr(dot + 1)));
}

bool IsTemporaryFileName(const std::wstring& name) {
    if (name.empty() || name.front() == L'~') {
        return true;
    }

    std::wstring lower = ToLower(name);
    return lower.ends_with(L".tmp") || lower.ends_with(L".temp") ||
           lower.ends_with(L".part") || lower.ends_with(L".crdownload");
}

std::vector<LaunchEntry> DiscoverRecentFiles(const Settings& settings) {
    struct FolderToScan {
        std::wstring path;
        int depth;
    };
    struct FileCandidate {
        std::wstring path;
        ULONGLONG modified;
    };

    std::vector<FolderToScan> folders;
    for (const auto& folder : settings.recentFolders) {
        if (PathIsExistingDirectory(folder)) {
            folders.push_back({folder, 0});
        }
    }

    std::vector<FileCandidate> candidates;
    candidates.reserve(std::min(settings.scanMaxItems, 4096));
    int scannedItems = 0;

    while (!folders.empty() && scannedItems < settings.scanMaxItems &&
           !g_stopWorker.load()) {
        FolderToScan current = std::move(folders.back());
        folders.pop_back();

        std::wstring pattern = current.path + L"\\*";
        WIN32_FIND_DATAW findData;
        HANDLE find = FindFirstFileExW(
            pattern.c_str(), FindExInfoBasic, &findData,
            FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
        if (find == INVALID_HANDLE_VALUE) {
            continue;
        }

        do {
            if (++scannedItems > settings.scanMaxItems ||
                g_stopWorker.load()) {
                break;
            }

            std::wstring_view name(findData.cFileName);
            if (name == L"." || name == L"..") {
                continue;
            }

            if (findData.dwFileAttributes &
                (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM |
                 FILE_ATTRIBUTE_REPARSE_POINT)) {
                continue;
            }

            std::wstring path =
                current.path + L"\\" + std::wstring(name);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (current.depth < settings.scanMaxDepth) {
                    folders.push_back(
                        {std::move(path), current.depth + 1});
                }
                continue;
            }

            if (IsTemporaryFileName(std::wstring(name)) ||
                !HasAllowedExtension(path, settings)) {
                continue;
            }

            candidates.push_back(
                {std::move(path),
                 FileTimeToUint64(findData.ftLastWriteTime)});
        } while (FindNextFileW(find, &findData));

        FindClose(find);
    }

    std::sort(candidates.begin(), candidates.end(),
              [](const FileCandidate& left, const FileCandidate& right) {
                  if (left.modified != right.modified) {
                      return left.modified > right.modified;
                  }
                  return left.path < right.path;
              });

    if (candidates.size() >
        static_cast<size_t>(settings.recentFileCount)) {
        candidates.resize(settings.recentFileCount);
    }

    std::vector<LaunchEntry> result;
    result.reserve(candidates.size());
    for (auto& candidate : candidates) {
        LaunchEntry entry;
        entry.title = FileNameFromPath(candidate.path);
        entry.subtitle = DirectoryNameFromPath(candidate.path);
        entry.command = std::move(candidate.path);
        entry.score = candidate.modified;
        entry.file = true;
        result.push_back(std::move(entry));
    }
    return result;
}

std::optional<std::wstring> GetJumpListObjectPath(
    IObjectArray* objects,
    UINT index) {
    IShellItem* shellItem = nullptr;
    if (SUCCEEDED(objects->GetAt(
            index, IID_PPV_ARGS(&shellItem)))) {
        PWSTR path = nullptr;
        HRESULT result =
            shellItem->GetDisplayName(SIGDN_FILESYSPATH, &path);
        shellItem->Release();
        if (SUCCEEDED(result) && path) {
            std::wstring value(path);
            CoTaskMemFree(path);
            return value;
        }
        if (path) {
            CoTaskMemFree(path);
        }
    }

    IShellLinkW* shellLink = nullptr;
    if (FAILED(objects->GetAt(index, IID_PPV_ARGS(&shellLink)))) {
        return std::nullopt;
    }

    WCHAR path[32768]{};
    WIN32_FIND_DATAW findData{};
    HRESULT result = shellLink->GetPath(
        path, ARRAYSIZE(path), &findData, SLGP_RAWPATH);
    shellLink->Release();
    if (FAILED(result) || !path[0]) {
        return std::nullopt;
    }
    return std::wstring(path);
}

RecentFileGroup DiscoverProgramRecentFiles(
    const RecentProgramSetting& program) {
    RecentFileGroup group{
        .title = program.name,
        .entries = {},
    };
    if (program.recentFileCount <= 0) {
        return group;
    }

    static constexpr CLSID kApplicationDocumentLists = {
        0x86bec222,
        0x30f2,
        0x47e0,
        {0x9f, 0x25, 0x60, 0xd1, 0x1c, 0xd7, 0x5c, 0x28},
    };
    IApplicationDocumentLists* documentLists = nullptr;
    HRESULT result = CoCreateInstance(
        kApplicationDocumentLists, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&documentLists));
    if (FAILED(result)) {
        Wh_Log(L"Couldn't create the document-list service for %s: 0x%08X",
               program.appId.c_str(), result);
        return group;
    }

    result = documentLists->SetAppID(program.appId.c_str());
    if (FAILED(result)) {
        Wh_Log(L"Couldn't select the Jump List for %s: 0x%08X",
               program.appId.c_str(), result);
        documentLists->Release();
        return group;
    }

    IObjectArray* objects = nullptr;
    result = documentLists->GetList(
        ADLT_RECENT, static_cast<UINT>(program.recentFileCount),
        IID_PPV_ARGS(&objects));
    documentLists->Release();
    if (FAILED(result) || !objects) {
        Wh_Log(L"Couldn't read the Recent Jump List for %s: 0x%08X",
               program.appId.c_str(), result);
        return group;
    }

    UINT count = 0;
    objects->GetCount(&count);
    std::unordered_set<std::wstring> seen;
    for (UINT index = 0;
         index < count &&
         group.entries.size() <
             static_cast<size_t>(program.recentFileCount);
         index++) {
        auto path = GetJumpListObjectPath(objects, index);
        if (!path || !PathIsExistingFile(*path) ||
            IsTemporaryFileName(FileNameFromPath(*path))) {
            continue;
        }

        std::wstring key = ToLower(*path);
        if (!seen.insert(std::move(key)).second) {
            continue;
        }

        WIN32_FILE_ATTRIBUTE_DATA attributes{};
        ULONGLONG modified = 0;
        if (GetFileAttributesExW(path->c_str(),
                                 GetFileExInfoStandard,
                                 &attributes)) {
            modified =
                FileTimeToUint64(attributes.ftLastWriteTime);
        }

        LaunchEntry entry;
        entry.title = FileNameFromPath(*path);
        std::wstring folder = DirectoryNameFromPath(*path);
        entry.subtitle = program.name;
        if (!folder.empty()) {
            entry.subtitle += L" | " + folder;
        }
        entry.command = std::move(*path);
        entry.score = modified;
        entry.file = true;
        group.entries.push_back(std::move(entry));
    }
    objects->Release();
    return group;
}

std::vector<RecentFileGroup> DiscoverRecentFileGroups(
    const Settings& settings) {
    std::vector<RecentFileGroup> groups;
    if (settings.recentFilesSource == RecentFilesSource::All) {
        groups.push_back({
            .title = L"Recent files",
            .entries = DiscoverRecentFiles(settings),
        });
        return groups;
    }

    groups.reserve(settings.recentPrograms.size());
    for (const auto& program : settings.recentPrograms) {
        if (g_stopWorker.load()) {
            break;
        }
        groups.push_back(DiscoverProgramRecentFiles(program));
    }
    return groups;
}

Settings CopySettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

void RefreshEntries() {
    Settings settings = CopySettings();
    std::vector<LaunchEntry> programs =
        DiscoverCommonPrograms(settings);
    std::vector<RecentFileGroup> fileGroups =
        DiscoverRecentFileGroups(settings);
    std::vector<LaunchEntry> files;
    for (const auto& group : fileGroups) {
        files.insert(files.end(), group.entries.begin(),
                     group.entries.end());
    }
    if (g_stopWorker.load()) {
        return;
    }

    std::lock_guard lock(g_entriesMutex);
    g_commonPrograms = std::move(programs);
    g_recentFiles = std::move(files);
    g_recentFileGroups = std::move(fileGroups);
    g_entriesGeneration++;
}

void StartWorker() {
    g_stopWorker = false;
    g_workerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_workerWakeEvent) {
        Wh_Log(L"Failed to create worker wake event: %u", GetLastError());
        return;
    }

    g_worker = std::thread([] {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        ULONGLONG nextRefresh = 0;

        while (!g_stopWorker.load()) {
            TrackForegroundProgram();

            ULONGLONG now = GetTickCount64();
            if (now >= nextRefresh) {
                RefreshEntries();
                SaveUsageRecords();

                Settings settings = CopySettings();
                nextRefresh =
                    now + static_cast<ULONGLONG>(
                              settings.refreshSeconds) *
                              1000;
            }

            WaitForSingleObject(g_workerWakeEvent, 2000);
        }

        SaveUsageRecords();
        CoUninitialize();
    });
}

void StopWorker() {
    g_stopWorker = true;
    if (g_workerWakeEvent) {
        SetEvent(g_workerWakeEvent);
    }
    if (g_worker.joinable()) {
        g_worker.join();
    }
    if (g_workerWakeEvent) {
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
    }
}

struct EntrySnapshot {
    Settings settings;
    std::vector<LaunchEntry> programs;
    std::vector<LaunchEntry> files;
    std::vector<RecentFileGroup> fileGroups;
    ULONGLONG generation = 0;
};

EntrySnapshot GetEntrySnapshot() {
    EntrySnapshot result;
    result.settings = CopySettings();
    std::lock_guard lock(g_entriesMutex);
    result.programs = g_commonPrograms;
    result.files = g_recentFiles;
    result.fileGroups = g_recentFileGroups;
    result.generation = g_entriesGeneration.load();
    return result;
}

wuxm::SolidColorBrush MakeBrush(BYTE alpha,
                                BYTE red,
                                BYTE green,
                                BYTE blue) {
    wuxm::SolidColorBrush brush;
    brush.Color({alpha, red, green, blue});
    return brush;
}

bool IsDarkTheme(const wux::FrameworkElement& element) {
    try {
        return element.ActualTheme() != wux::ElementTheme::Light;
    } catch (...) {
        return true;
    }
}

void Launch(const LaunchEntry& entry) {
    HINSTANCE result = ShellExecuteW(
        nullptr, L"open", entry.command.c_str(),
        entry.arguments.empty() ? nullptr : entry.arguments.c_str(),
        entry.workingDirectory.empty()
            ? nullptr
            : entry.workingDirectory.c_str(),
        SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        Wh_Log(L"ShellExecute failed for %s: %Id", entry.command.c_str(),
               reinterpret_cast<INT_PTR>(result));
    }
}

HMODULE GetCurrentModuleHandle();

constexpr UINT WM_ALT_TAB_PLUS_SHOW = WM_APP + 0x4A1;
constexpr UINT WM_ALT_TAB_PLUS_HIDE = WM_APP + 0x4A2;
constexpr UINT_PTR kCompanionTimerId = 0x415450;

struct CompanionWindowState {
    bool programs = false;
    bool dark = true;
    bool showGroupHeadings = false;
    int hoveredIndex = -1;
    int scrollOffset = 0;
    int contentHeight = 0;
    std::vector<LaunchEntry> entries;
    std::vector<RecentFileGroup> groups;
    std::vector<RECT> hitRects;
    std::vector<LaunchEntry> hitEntries;
};

HANDLE g_companionThread = nullptr;
HANDLE g_companionThreadReady = nullptr;
DWORD g_companionThreadId = 0;
HWND g_programsCompanionWindow = nullptr;
HWND g_filesCompanionWindow = nullptr;
std::atomic<HMONITOR> g_companionMonitor = nullptr;
std::atomic<ULONGLONG> g_companionShowTick = 0;
std::atomic<ULONGLONG> g_companionGeneration = 0;
std::atomic<bool> g_companionVisible = false;
std::atomic<int> g_xamlPanelCount = 0;
std::atomic<DWORD> g_altTabShowThreadId = 0;
std::atomic<HWND> g_nativeAltTabHostWindow = nullptr;

bool IsSystemDarkMode() {
    DWORD appsUseLightTheme = 1;
    DWORD size = sizeof(appsUseLightTheme);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr,
        &appsUseLightTheme, &size);
    return appsUseLightTheme == 0;
}

int ScaleForDpi(HWND window, int value) {
    UINT dpi = GetDpiForWindow(window);
    return MulDiv(value, dpi ? static_cast<int>(dpi) : 96, 96);
}

bool GetAdjustedAltTabCorridor(RECT* corridor) {
    Settings settings = CopySettings();
    if (settings.renderMode != RenderMode::Companion ||
        settings.layout != LayoutMode::Columns ||
        settings.columnThumbnailArea !=
            ColumnThumbnailArea::Adjusted) {
        return false;
    }

    HMONITOR monitor = g_companionMonitor.load();
    if (!monitor) {
        monitor = MonitorFromWindow(GetForegroundWindow(),
                                    MONITOR_DEFAULTTOPRIMARY);
    }
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    HWND dpiWindow = g_programsCompanionWindow;
    const int margin = ScaleForDpi(dpiWindow, 24);
    const int panelWidth =
        ScaleForDpi(dpiWindow, settings.panelWidth);
    const int breathingRoom = ScaleForDpi(dpiWindow, 8);
    corridor->left =
        monitorInfo.rcWork.left + margin + panelWidth + breathingRoom;
    corridor->top = monitorInfo.rcWork.top;
    corridor->right =
        monitorInfo.rcWork.right - margin - panelWidth - breathingRoom;
    corridor->bottom = monitorInfo.rcWork.bottom;
    return corridor->right - corridor->left >=
           ScaleForDpi(dpiWindow, 96);
}

void ConstrainNativeAltTabWindow();

HFONT CreateCompanionFont(HWND window, int points, bool bold) {
    UINT dpi = GetDpiForWindow(window);
    int height = -MulDiv(points, dpi ? static_cast<int>(dpi) : 96, 72);
    return CreateFontW(height, 0, 0, 0,
                       bold ? FW_SEMIBOLD : FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void DrawEntryIcon(HWND window,
                   HDC dc,
                   const LaunchEntry& entry,
                   const RECT& itemRect,
                   int iconSize,
                   bool dark) {
    SHFILEINFOW fileInfo{};
    UINT flags = SHGFI_ICON | SHGFI_SMALLICON;
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;
    if (!PathIsExistingFile(entry.command)) {
        flags |= SHGFI_USEFILEATTRIBUTES;
    }

    if (SHGetFileInfoW(entry.command.c_str(), attributes, &fileInfo,
                       sizeof(fileInfo), flags) &&
        fileInfo.hIcon) {
        int x = itemRect.left + ScaleForDpi(window, 12);
        int y = itemRect.top + (itemRect.bottom - itemRect.top - iconSize) / 2;
        DrawIconEx(dc, x, y, fileInfo.hIcon, iconSize, iconSize, 0, nullptr,
                   DI_NORMAL);
        DestroyIcon(fileInfo.hIcon);
        return;
    }

    const COLORREF badge =
        dark ? RGB(74, 106, 176) : RGB(55, 86, 158);
    HBRUSH badgeBrush = CreateSolidBrush(badge);
    int x = itemRect.left + ScaleForDpi(window, 12);
    int y = itemRect.top + (itemRect.bottom - itemRect.top - iconSize) / 2;
    RECT badgeRect{x, y, x + iconSize, y + iconSize};
    FillRect(dc, &badgeRect, badgeBrush);
    DeleteObject(badgeBrush);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    WCHAR letter[2] = {
        entry.file ? L'F'
                   : (entry.title.empty()
                          ? L'A'
                          : static_cast<WCHAR>(
                                std::towupper(entry.title.front()))),
        L'\0'};
    HFONT font = CreateCompanionFont(window, 9, true);
    HGDIOBJ oldFont = SelectObject(dc, font);
    DrawTextW(dc, letter, -1, &badgeRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

void PaintCompanionWindow(HWND window,
                          CompanionWindowState& state) {
    PAINTSTRUCT paint;
    HDC dc = BeginPaint(window, &paint);
    if (!dc) {
        return;
    }

    RECT client;
    GetClientRect(window, &client);
    HDC bufferDc = CreateCompatibleDC(dc);
    HBITMAP bufferBitmap =
        CreateCompatibleBitmap(dc, client.right, client.bottom);
    HGDIOBJ oldBitmap = SelectObject(bufferDc, bufferBitmap);

    const COLORREF background =
        state.dark ? RGB(31, 31, 31) : RGB(245, 245, 245);
    const COLORREF itemBackground =
        state.dark ? RGB(47, 47, 47) : RGB(230, 230, 230);
    const COLORREF itemHover =
        state.dark ? RGB(64, 64, 64) : RGB(215, 215, 215);
    const COLORREF primary =
        state.dark ? RGB(248, 248, 248) : RGB(24, 24, 24);
    const COLORREF secondary =
        state.dark ? RGB(190, 190, 190) : RGB(90, 90, 90);

    HBRUSH backgroundBrush = CreateSolidBrush(background);
    FillRect(bufferDc, &client, backgroundBrush);
    DeleteObject(backgroundBrush);

    SetBkMode(bufferDc, TRANSPARENT);
    SetTextColor(bufferDc, primary);
    HFONT headingFont = CreateCompanionFont(window, 11, true);
    HGDIOBJ oldFont = SelectObject(bufferDc, headingFont);
    RECT headingRect{
        ScaleForDpi(window, 16),
        ScaleForDpi(window, 12),
        client.right - ScaleForDpi(window, 12),
        ScaleForDpi(window, 42)};
    const WCHAR* heading =
        state.programs ? L"Common programs" : L"Recent files";
    DrawTextW(bufferDc, heading, -1, &headingRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(bufferDc, oldFont);
    DeleteObject(headingFont);

    state.hitRects.clear();
    state.hitEntries.clear();
    const int itemLeft = ScaleForDpi(window, 10);
    const int itemRight = client.right - ScaleForDpi(window, 10);
    const int itemHeight = ScaleForDpi(window, 54);
    const int itemGap = ScaleForDpi(window, 4);
    const int itemStride = itemHeight + itemGap;
    const int sectionHeight = ScaleForDpi(window, 24);
    const int contentTop = ScaleForDpi(window, 45);
    const int availableHeight =
        std::max(0, static_cast<int>(client.bottom) - contentTop);
    int groupHeadingHeight = 0;
    if (state.showGroupHeadings && !state.entries.empty()) {
        groupHeadingHeight =
            static_cast<int>(state.groups.size()) * sectionHeight;
    }
    state.contentHeight =
        state.entries.empty()
            ? ScaleForDpi(window, 58)
            : static_cast<int>(state.entries.size()) * itemStride +
                  groupHeadingHeight;
    state.scrollOffset = std::clamp(
        state.scrollOffset, 0,
        std::max(0, state.contentHeight - availableHeight));
    int top = contentTop - state.scrollOffset;
    int savedDc = SaveDC(bufferDc);
    IntersectClipRect(bufferDc, 0, contentTop, client.right,
                      client.bottom);

    if (state.entries.empty()) {
        HFONT emptyFont = CreateCompanionFont(window, 9, false);
        oldFont = SelectObject(bufferDc, emptyFont);
        SetTextColor(bufferDc, secondary);
        RECT emptyRect{ScaleForDpi(window, 16), top,
                       client.right - ScaleForDpi(window, 16),
                       client.bottom - ScaleForDpi(window, 10)};
        const WCHAR* emptyText =
            state.programs ? L"Learning your commonly used programs..."
                           : L"No matching recent files";
        DrawTextW(bufferDc, emptyText, -1, &emptyRect,
                  DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(bufferDc, oldFont);
        DeleteObject(emptyFont);
    }

    auto drawEntry = [&](const LaunchEntry& entry) {
        RECT itemRect{itemLeft, top, itemRight, top + itemHeight};
        if (itemRect.bottom <= contentTop ||
            itemRect.top >= client.bottom) {
            top += itemStride;
            return;
        }

        const int hitIndex =
            static_cast<int>(state.hitRects.size());
        RECT hitRect = itemRect;
        hitRect.top = std::max<LONG>(hitRect.top, contentTop);
        hitRect.bottom =
            std::min<LONG>(hitRect.bottom, client.bottom);
        state.hitRects.push_back(hitRect);
        state.hitEntries.push_back(entry);
        HBRUSH itemBrush = CreateSolidBrush(
            state.hoveredIndex == hitIndex ? itemHover
                                           : itemBackground);
        HPEN nullPen =
            static_cast<HPEN>(GetStockObject(NULL_PEN));
        HGDIOBJ oldBrush = SelectObject(bufferDc, itemBrush);
        HGDIOBJ oldPen = SelectObject(bufferDc, nullPen);
        int radius = ScaleForDpi(window, 8);
        RoundRect(bufferDc, itemRect.left, itemRect.top, itemRect.right,
                  itemRect.bottom, radius, radius);
        SelectObject(bufferDc, oldPen);
        SelectObject(bufferDc, oldBrush);
        DeleteObject(itemBrush);

        const int iconSize = ScaleForDpi(window, 24);
        DrawEntryIcon(window, bufferDc, entry, itemRect, iconSize,
                      state.dark);

        const int textLeft = itemRect.left + ScaleForDpi(window, 46);
        RECT titleRect{textLeft, itemRect.top + ScaleForDpi(window, 7),
                       itemRect.right - ScaleForDpi(window, 10),
                       itemRect.top + ScaleForDpi(window, 29)};
        HFONT titleFont = CreateCompanionFont(window, 9, true);
        oldFont = SelectObject(bufferDc, titleFont);
        SetTextColor(bufferDc, primary);
        DrawTextW(bufferDc, entry.title.c_str(), -1, &titleRect,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                      DT_NOPREFIX);
        SelectObject(bufferDc, oldFont);
        DeleteObject(titleFont);

        RECT subtitleRect{
            textLeft, itemRect.top + ScaleForDpi(window, 28),
            itemRect.right - ScaleForDpi(window, 10),
            itemRect.bottom - ScaleForDpi(window, 5)};
        HFONT subtitleFont = CreateCompanionFont(window, 8, false);
        oldFont = SelectObject(bufferDc, subtitleFont);
        SetTextColor(bufferDc, secondary);
        DrawTextW(bufferDc, entry.subtitle.c_str(), -1, &subtitleRect,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                      DT_NOPREFIX);
        SelectObject(bufferDc, oldFont);
        DeleteObject(subtitleFont);

        top += itemStride;
    };

    if (state.showGroupHeadings && !state.entries.empty()) {
        HFONT sectionFont = CreateCompanionFont(window, 8, true);
        for (const auto& group : state.groups) {
            RECT sectionRect{
                ScaleForDpi(window, 16), top,
                client.right - ScaleForDpi(window, 12),
                top + sectionHeight};
            oldFont = SelectObject(bufferDc, sectionFont);
            SetTextColor(bufferDc, secondary);
            DrawTextW(bufferDc, group.title.c_str(), -1,
                      &sectionRect,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE |
                          DT_END_ELLIPSIS | DT_NOPREFIX);
            SelectObject(bufferDc, oldFont);
            top += sectionHeight;
            for (const auto& entry : group.entries) {
                drawEntry(entry);
            }
        }
        DeleteObject(sectionFont);
    } else {
        for (const auto& entry : state.entries) {
            drawEntry(entry);
        }
    }
    RestoreDC(bufferDc, savedDc);

    BitBlt(dc, 0, 0, client.right, client.bottom, bufferDc, 0, 0, SRCCOPY);
    SelectObject(bufferDc, oldBitmap);
    DeleteObject(bufferBitmap);
    DeleteDC(bufferDc);
    EndPaint(window, &paint);
}

void HideCompanionPanels() {
    if (g_programsCompanionWindow) {
        ShowWindow(g_programsCompanionWindow, SW_HIDE);
    }
    if (g_filesCompanionWindow) {
        ShowWindow(g_filesCompanionWindow, SW_HIDE);
    }
    g_companionVisible = false;
}

LRESULT CALLBACK CompanionWindowProc(HWND window,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam) {
    auto* state = reinterpret_cast<CompanionWindowState*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
        case WM_NCCREATE: {
            auto* create =
                reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(
                window, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return TRUE;
        }

        case WM_ERASEBKGND:
            return TRUE;

        case WM_PAINT:
            if (state) {
                PaintCompanionWindow(window, *state);
                return 0;
            }
            break;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_MOUSEMOVE:
            if (state) {
                POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                int hoveredIndex = -1;
                for (size_t index = 0; index < state->hitRects.size();
                     index++) {
                    if (PtInRect(&state->hitRects[index], point)) {
                        hoveredIndex = static_cast<int>(index);
                        break;
                    }
                }
                if (hoveredIndex != state->hoveredIndex) {
                    state->hoveredIndex = hoveredIndex;
                    InvalidateRect(window, nullptr, FALSE);
                }
                TRACKMOUSEEVENT track{};
                track.cbSize = sizeof(track);
                track.dwFlags = TME_LEAVE;
                track.hwndTrack = window;
                TrackMouseEvent(&track);
            }
            return 0;

        case WM_MOUSELEAVE:
            if (state && state->hoveredIndex != -1) {
                state->hoveredIndex = -1;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;

        case WM_MOUSEWHEEL:
            if (state) {
                RECT client{};
                GetClientRect(window, &client);
                const int contentTop = ScaleForDpi(window, 45);
                const int maxOffset = std::max(
                    0, state->contentHeight -
                           std::max(
                               0,
                               static_cast<int>(client.bottom) -
                                   contentTop));
                const int steps =
                    GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
                state->scrollOffset = std::clamp(
                    state->scrollOffset -
                        steps * ScaleForDpi(window, 48),
                    0, maxOffset);
                state->hoveredIndex = -1;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP:
            if (state) {
                POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                for (size_t index = 0; index < state->hitRects.size();
                     index++) {
                    if (PtInRect(&state->hitRects[index], point) &&
                        index < state->hitEntries.size()) {
                        LaunchEntry entry = state->hitEntries[index];
                        HideCompanionPanels();
                        Launch(entry);
                        return 0;
                    }
                }
            }
            return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void ConfigureCompanionWindow(HWND window, bool dark) {
    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_VALUE = 20;
    constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE_VALUE = 33;
    constexpr DWORD DWMWCP_ROUND_VALUE = 2;

    BOOL darkValue = dark;
    DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE_VALUE,
                          &darkValue, sizeof(darkValue));
    DWORD cornerPreference = DWMWCP_ROUND_VALUE;
    DwmSetWindowAttribute(window, DWMWA_WINDOW_CORNER_PREFERENCE_VALUE,
                          &cornerPreference, sizeof(cornerPreference));
}

void SetCompanionWindowRegion(HWND window, int width, int height) {
    int radius = ScaleForDpi(window, 12);
    HRGN region =
        CreateRoundRectRgn(0, 0, width + 1, height + 1, radius, radius);
    if (!SetWindowRgn(window, region, FALSE)) {
        DeleteObject(region);
    }
}

void ShowCompanionPanels() {
    if (!g_programsCompanionWindow || !g_filesCompanionWindow) {
        return;
    }

    EntrySnapshot snapshot = GetEntrySnapshot();
    auto* programState = reinterpret_cast<CompanionWindowState*>(
        GetWindowLongPtrW(g_programsCompanionWindow, GWLP_USERDATA));
    auto* fileState = reinterpret_cast<CompanionWindowState*>(
        GetWindowLongPtrW(g_filesCompanionWindow, GWLP_USERDATA));
    if (!programState || !fileState) {
        return;
    }

    programState->entries = snapshot.programs;
    fileState->entries = snapshot.files;
    fileState->groups = snapshot.fileGroups;
    fileState->showGroupHeadings =
        snapshot.settings.recentFilesSource ==
            RecentFilesSource::ProgramSpecific &&
        snapshot.settings.layout == LayoutMode::Columns;
    if (snapshot.settings.layout != LayoutMode::Columns) {
        constexpr size_t maximumStripEntries = 3;
        if (programState->entries.size() > maximumStripEntries) {
            programState->entries.resize(maximumStripEntries);
        }
        if (fileState->entries.size() > maximumStripEntries) {
            fileState->entries.resize(maximumStripEntries);
        }
        fileState->groups.clear();
    }
    programState->hoveredIndex = -1;
    fileState->hoveredIndex = -1;
    programState->scrollOffset = 0;
    fileState->scrollOffset = 0;
    programState->dark = fileState->dark = IsSystemDarkMode();

    HMONITOR monitor = g_companionMonitor.load();
    if (!monitor) {
        monitor = MonitorFromWindow(GetForegroundWindow(),
                                    MONITOR_DEFAULTTOPRIMARY);
    }
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return;
    }

    const RECT work = monitorInfo.rcWork;
    const int workWidth = work.right - work.left;
    const int workHeight = work.bottom - work.top;
    const int margin = ScaleForDpi(g_programsCompanionWindow, 24);
    int panelWidth =
        ScaleForDpi(g_programsCompanionWindow,
                    snapshot.settings.panelWidth);
    if (snapshot.settings.layout != LayoutMode::Columns) {
        panelWidth = std::clamp((workWidth - margin * 3) / 2,
                                ScaleForDpi(g_programsCompanionWindow, 260),
                                ScaleForDpi(g_programsCompanionWindow, 560));
    }

    auto panelHeight = [&](const CompanionWindowState& state) {
        const int visibleCount =
            static_cast<int>(
                std::max<size_t>(state.entries.size(), 1));
        int height = ScaleForDpi(
            g_programsCompanionWindow,
            51 + visibleCount * 58 +
                (state.showGroupHeadings &&
                         !state.entries.empty()
                     ? static_cast<int>(state.groups.size()) * 24
                     : 0));
        return std::min(height, workHeight - margin * 2);
    };
    int programsHeight = panelHeight(*programState);
    int filesHeight = panelHeight(*fileState);

    POINT programsPosition{};
    POINT filesPosition{};
    if (snapshot.settings.layout == LayoutMode::Columns) {
        programsPosition = {
            work.left + margin,
            work.top + (workHeight - programsHeight) / 2};
        filesPosition = {
            work.right - margin - panelWidth,
            work.top + (workHeight - filesHeight) / 2};
    } else {
        int totalWidth = panelWidth * 2 + margin;
        int left = work.left + (workWidth - totalWidth) / 2;
        bool top = snapshot.settings.layout == LayoutMode::Top;
        programsPosition = {
            left,
            top ? work.top + margin
                : work.bottom - margin - programsHeight};
        filesPosition = {
            left + panelWidth + margin,
            top ? work.top + margin : work.bottom - margin - filesHeight};
    }

    ConfigureCompanionWindow(g_programsCompanionWindow,
                             programState->dark);
    ConfigureCompanionWindow(g_filesCompanionWindow, fileState->dark);
    SetCompanionWindowRegion(g_programsCompanionWindow, panelWidth,
                             programsHeight);
    SetCompanionWindowRegion(g_filesCompanionWindow, panelWidth,
                             filesHeight);

    SetWindowPos(g_programsCompanionWindow, HWND_TOPMOST,
                 programsPosition.x, programsPosition.y, panelWidth,
                 programsHeight,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetWindowPos(g_filesCompanionWindow, HWND_TOPMOST, filesPosition.x,
                 filesPosition.y, panelWidth, filesHeight,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_programsCompanionWindow, nullptr, FALSE);
    InvalidateRect(g_filesCompanionWindow, nullptr, FALSE);

    g_companionGeneration = snapshot.generation;
    g_companionVisible = true;
}

DWORD WINAPI CompanionThreadProc(void*) {
    PeekMessageW(nullptr, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HINSTANCE instance = GetCurrentModuleHandle();
    constexpr PCWSTR kCompanionClassName =
        L"WindhawkAltTabPlusCompanion";
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = CompanionWindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kCompanionClassName;
    RegisterClassExW(&windowClass);

    CompanionWindowState programState{};
    programState.programs = true;
    CompanionWindowState fileState{};
    fileState.programs = false;
    constexpr DWORD extendedStyle =
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    using CreateWindowInBandRaw_t = HWND(WINAPI*)(
        DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU,
        HINSTANCE, void*, DWORD);
    auto createWindowInBand = reinterpret_cast<CreateWindowInBandRaw_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
                       "CreateWindowInBand"));
    constexpr DWORD ZBID_SYSTEM_TOOLS = 16;
    auto createCompanionWindow =
        [&](PCWSTR title, CompanionWindowState* state) {
            HWND window = nullptr;
            if (createWindowInBand) {
                window = createWindowInBand(
                    extendedStyle, kCompanionClassName, title, WS_POPUP, 0,
                    0, 0, 0, nullptr, nullptr, instance, state,
                    ZBID_SYSTEM_TOOLS);
            }
            if (!window) {
                window = CreateWindowExW(
                    extendedStyle, kCompanionClassName, title, WS_POPUP, 0,
                    0, 0, 0, nullptr, nullptr, instance, state);
            }
            return window;
        };
    g_programsCompanionWindow =
        createCompanionWindow(L"Common programs", &programState);
    g_filesCompanionWindow =
        createCompanionWindow(L"Recent files", &fileState);

    UINT_PTR timerId =
        SetTimer(nullptr, kCompanionTimerId, 50, nullptr);
    SetEvent(g_companionThreadReady);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (!message.hwnd && message.message == WM_ALT_TAB_PLUS_SHOW) {
            ShowCompanionPanels();
            continue;
        }
        if (!message.hwnd && message.message == WM_ALT_TAB_PLUS_HIDE) {
            HideCompanionPanels();
            continue;
        }
        if (!message.hwnd && message.message == WM_TIMER &&
            message.wParam == timerId) {
            if (g_companionVisible.load()) {
                const ULONGLONG elapsed =
                    GetTickCount64() - g_companionShowTick.load();
                if (elapsed > 250 &&
                    !(GetAsyncKeyState(VK_MENU) & 0x8000)) {
                    HideCompanionPanels();
                } else {
                    ConstrainNativeAltTabWindow();
                    if (g_companionGeneration.load() !=
                        g_entriesGeneration.load()) {
                        ShowCompanionPanels();
                    } else {
                        SetWindowPos(
                            g_programsCompanionWindow, HWND_TOPMOST, 0, 0,
                            0, 0,
                            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                                SWP_NOOWNERZORDER);
                        SetWindowPos(
                            g_filesCompanionWindow, HWND_TOPMOST, 0, 0, 0,
                            0,
                            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                                SWP_NOOWNERZORDER);
                    }
                }
            }
            continue;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (timerId) {
        KillTimer(nullptr, timerId);
    }
    if (g_programsCompanionWindow) {
        DestroyWindow(g_programsCompanionWindow);
        g_programsCompanionWindow = nullptr;
    }
    if (g_filesCompanionWindow) {
        DestroyWindow(g_filesCompanionWindow);
        g_filesCompanionWindow = nullptr;
    }
    UnregisterClassW(kCompanionClassName, instance);
    return 0;
}

bool StartCompanionThread() {
    g_companionThreadReady =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_companionThreadReady) {
        return false;
    }

    g_companionThread = CreateThread(
        nullptr, 0, CompanionThreadProc, nullptr, 0, &g_companionThreadId);
    if (!g_companionThread) {
        CloseHandle(g_companionThreadReady);
        g_companionThreadReady = nullptr;
        return false;
    }

    DWORD wait = WaitForSingleObject(g_companionThreadReady, 3000);
    CloseHandle(g_companionThreadReady);
    g_companionThreadReady = nullptr;
    return wait == WAIT_OBJECT_0;
}

void StopCompanionThread() {
    if (!g_companionThread) {
        return;
    }
    PostThreadMessageW(g_companionThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_companionThread, 5000);
    CloseHandle(g_companionThread);
    g_companionThread = nullptr;
    g_companionThreadId = 0;
}

void RequestCompanionPanels() {
    Settings settings = CopySettings();
    if (settings.renderMode != RenderMode::Companion ||
        !g_companionThreadId) {
        return;
    }

    HWND foreground = GetForegroundWindow();
    g_companionMonitor =
        MonitorFromWindow(foreground, MONITOR_DEFAULTTOPRIMARY);
    g_companionShowTick = GetTickCount64();
    PostThreadMessageW(g_companionThreadId, WM_ALT_TAB_PLUS_SHOW, 0, 0);
}

wuxc::Button CreateEntryButton(const LaunchEntry& entry,
                               bool dark,
                               double width) {
    auto primaryBrush = dark ? MakeBrush(0xFF, 0xF5, 0xF5, 0xF5)
                             : MakeBrush(0xFF, 0x18, 0x18, 0x18);
    auto secondaryBrush = dark ? MakeBrush(0xB8, 0xFF, 0xFF, 0xFF)
                               : MakeBrush(0xA8, 0x00, 0x00, 0x00);
    auto surfaceBrush = dark ? MakeBrush(0x25, 0xFF, 0xFF, 0xFF)
                             : MakeBrush(0x16, 0x00, 0x00, 0x00);

    wuxc::Button button;
    button.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    button.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
    button.Padding({10, 8, 10, 8});
    button.Margin({0, 0, 0, 6});
    button.MinHeight(52);
    if (width > 0) {
        button.Width(width);
    }
    button.Background(surfaceBrush);
    button.BorderThickness({0, 0, 0, 0});
    button.CornerRadius({8, 8, 8, 8});

    wuxc::Grid row;
    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width({34, wux::GridUnitType::Pixel});
    wuxc::ColumnDefinition textColumn;
    textColumn.Width({1, wux::GridUnitType::Star});
    row.ColumnDefinitions().Append(iconColumn);
    row.ColumnDefinitions().Append(textColumn);

    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(entry.file ? L"\uE8A5" : L"\uE71D");
    icon.FontSize(18);
    icon.Foreground(primaryBrush);
    icon.VerticalAlignment(wux::VerticalAlignment::Center);
    wuxc::Grid::SetColumn(icon, 0);
    row.Children().Append(icon);

    wuxc::StackPanel textStack;
    wuxc::TextBlock title;
    title.Text(entry.title);
    title.Foreground(primaryBrush);
    title.FontSize(13);
    title.FontWeight(wut::FontWeights::SemiBold());
    title.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    textStack.Children().Append(title);

    if (!entry.subtitle.empty()) {
        wuxc::TextBlock subtitle;
        subtitle.Text(entry.subtitle);
        subtitle.Foreground(secondaryBrush);
        subtitle.FontSize(11);
        subtitle.Margin({0, 2, 0, 0});
        subtitle.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        textStack.Children().Append(subtitle);
    }

    wuxc::Grid::SetColumn(textStack, 1);
    row.Children().Append(textStack);
    button.Content(row);

    wuxa::AutomationProperties::SetName(
        button, entry.file ? L"Recent file: " + entry.title
                           : L"Common program: " + entry.title);
    wuxc::ToolTipService::SetToolTip(
        button, winrt::box_value(entry.command));

    button.Click([entry](const wf::IInspectable&,
                         const wux::RoutedEventArgs&) { Launch(entry); });
    return button;
}

wuxc::Border CreateSectionCard(
    const std::wstring& heading,
    const std::vector<LaunchEntry>& entries,
    bool dark,
    double width,
    size_t maximumEntries) {
    auto primaryBrush = dark ? MakeBrush(0xFF, 0xF5, 0xF5, 0xF5)
                             : MakeBrush(0xFF, 0x18, 0x18, 0x18);
    auto secondaryBrush = dark ? MakeBrush(0xA8, 0xFF, 0xFF, 0xFF)
                               : MakeBrush(0x98, 0x00, 0x00, 0x00);
    auto cardBrush = dark ? MakeBrush(0xEC, 0x20, 0x20, 0x20)
                          : MakeBrush(0xF2, 0xF4, 0xF4, 0xF4);
    auto borderBrush = dark ? MakeBrush(0x25, 0xFF, 0xFF, 0xFF)
                            : MakeBrush(0x20, 0x00, 0x00, 0x00);

    wuxc::Border card;
    card.Width(width);
    card.Padding({12, 12, 12, 8});
    card.Background(cardBrush);
    card.BorderBrush(borderBrush);
    card.BorderThickness({1, 1, 1, 1});
    card.CornerRadius({12, 12, 12, 12});

    wuxc::StackPanel content;
    wuxc::TextBlock headingText;
    headingText.Text(heading);
    headingText.Foreground(primaryBrush);
    headingText.FontSize(14);
    headingText.FontWeight(wut::FontWeights::SemiBold());
    headingText.Margin({2, 0, 2, 10});
    content.Children().Append(headingText);

    const size_t count = std::min(entries.size(), maximumEntries);
    for (size_t index = 0; index < count; index++) {
        content.Children().Append(
            CreateEntryButton(entries[index], dark, 0));
    }

    if (!count) {
        wuxc::TextBlock empty;
        empty.Text(heading == L"Common programs"
                       ? L"No ranked programs yet"
                       : L"No matching recent files");
        empty.Foreground(secondaryBrush);
        empty.FontSize(12);
        empty.TextWrapping(wux::TextWrapping::Wrap);
        empty.Margin({4, 8, 4, 12});
        content.Children().Append(empty);
    }

    card.Child(content);
    return card;
}

struct NativeLayoutState {
    wux::Thickness margin{};
    wux::HorizontalAlignment horizontalAlignment =
        wux::HorizontalAlignment::Stretch;
    wux::VerticalAlignment verticalAlignment =
        wux::VerticalAlignment::Stretch;
    double maxWidth = std::numeric_limits<double>::infinity();
};

struct PanelRefreshState {
    winrt::weak_ref<wuxc::Grid> root;
    winrt::weak_ref<wux::FrameworkElement> panel;
    ULONGLONG generation = 0;
    bool refreshing = false;
};

struct AugmentedRoot {
    winrt::weak_ref<wuxc::Grid> root;
    winrt::weak_ref<wux::FrameworkElement> nativeBackground;
    winrt::weak_ref<wux::FrameworkElement> panel;
    winrt::event_token layoutUpdatedToken{};
    std::shared_ptr<PanelRefreshState> refreshState;
    NativeLayoutState original;
};

thread_local bool g_initializedForThread = false;
thread_local std::vector<AugmentedRoot> g_augmentedRoots;

std::optional<std::pair<wuxc::Grid, wux::FrameworkElement>>
FindAltTabRootAndBackground(const wux::FrameworkElement& element) {
    wux::DependencyObject current = element;
    wuxc::Grid modalRoot{nullptr};
    wux::FrameworkElement background{nullptr};
    bool isAltTab = false;

    for (int level = 0; current && level < 32; level++) {
        auto frameworkElement = current.try_as<wux::FrameworkElement>();
        if (frameworkElement) {
            auto grid = frameworkElement.try_as<wuxc::Grid>();
            if (grid && frameworkElement.Name() == L"ModalRootGrid") {
                modalRoot = grid;
            }
            if (frameworkElement.Name() == L"BackgroundElement") {
                background = frameworkElement;
            }
        }

        try {
            std::wstring className = winrt::get_class_name(current).c_str();
            if (className.find(
                    L"ComposableShell.Experiences.Switcher.AltTab") !=
                std::wstring::npos) {
                isAltTab = true;
            }
        } catch (...) {
        }

        current = wuxm::VisualTreeHelper::GetParent(current);
    }

    if (!isAltTab || !modalRoot || !background) {
        return std::nullopt;
    }
    return std::make_pair(modalRoot, background);
}

bool IsAlreadyAugmented(const wuxc::Grid& root) {
    for (auto iterator = g_augmentedRoots.begin();
         iterator != g_augmentedRoots.end();) {
        auto existingRoot = iterator->root.get();
        if (!existingRoot) {
            iterator = g_augmentedRoots.erase(iterator);
            continue;
        }
        if (existingRoot == root) {
            return true;
        }
        ++iterator;
    }
    return false;
}

void ApplyNativeLayout(
    const EntrySnapshot& snapshot,
    const wuxc::Grid& root,
    const wux::FrameworkElement& nativeBackground) {
    const double screenWidth =
        root.ActualWidth() > 0 ? root.ActualWidth()
                              : static_cast<double>(GetSystemMetrics(SM_CXSCREEN));

    if (snapshot.settings.layout == LayoutMode::Columns) {
        const double reserved =
            2.0 * (snapshot.settings.panelWidth + 52.0);
        nativeBackground.HorizontalAlignment(
            wux::HorizontalAlignment::Center);
        nativeBackground.VerticalAlignment(
            wux::VerticalAlignment::Center);
        nativeBackground.MaxWidth(
            std::max(440.0, screenWidth - reserved));
    } else if (snapshot.settings.layout == LayoutMode::Top) {
        nativeBackground.HorizontalAlignment(
            wux::HorizontalAlignment::Center);
        nativeBackground.VerticalAlignment(
            wux::VerticalAlignment::Bottom);
        nativeBackground.Margin({36, 232, 36, 36});
        nativeBackground.MaxWidth(std::max(520.0, screenWidth - 72.0));
    } else {
        nativeBackground.HorizontalAlignment(
            wux::HorizontalAlignment::Center);
        nativeBackground.VerticalAlignment(wux::VerticalAlignment::Top);
        nativeBackground.Margin({36, 36, 36, 232});
        nativeBackground.MaxWidth(std::max(520.0, screenWidth - 72.0));
    }
}

wuxc::Grid BuildAugmentationPanel(const EntrySnapshot& snapshot,
                                  const wuxc::Grid& root) {
    bool dark = IsDarkTheme(root);
    wuxc::Grid overlay;
    overlay.Name(L"WindhawkAltTabPlusPanel");
    overlay.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    overlay.VerticalAlignment(wux::VerticalAlignment::Stretch);
    wuxc::Canvas::SetZIndex(overlay, 100);
    wuxa::AutomationProperties::SetName(
        overlay, L"Alt+Tab Plus shortcuts");

    if (snapshot.settings.layout == LayoutMode::Columns) {
        auto programs = CreateSectionCard(
            L"Common programs", snapshot.programs, dark,
            snapshot.settings.panelWidth, snapshot.programs.size());
        programs.HorizontalAlignment(wux::HorizontalAlignment::Left);
        programs.VerticalAlignment(wux::VerticalAlignment::Center);
        programs.Margin({24, 24, 0, 24});
        overlay.Children().Append(programs);

        auto files = CreateSectionCard(
            L"Recent files", snapshot.files, dark,
            snapshot.settings.panelWidth, snapshot.files.size());
        files.HorizontalAlignment(wux::HorizontalAlignment::Right);
        files.VerticalAlignment(wux::VerticalAlignment::Center);
        files.Margin({0, 24, 24, 24});
        overlay.Children().Append(files);
    } else {
        wuxc::StackPanel strip;
        strip.Orientation(wuxc::Orientation::Horizontal);
        strip.HorizontalAlignment(wux::HorizontalAlignment::Center);
        strip.VerticalAlignment(
            snapshot.settings.layout == LayoutMode::Top
                ? wux::VerticalAlignment::Top
                : wux::VerticalAlignment::Bottom);
        strip.Margin({24, 24, 24, 24});

        const double cardWidth =
            std::clamp(root.ActualWidth() / 2.0 - 42.0, 260.0, 560.0);
        const size_t stripCount = 3;

        auto programs =
            CreateSectionCard(L"Common programs", snapshot.programs, dark,
                              cardWidth, stripCount);
        programs.Margin({0, 0, 8, 0});
        strip.Children().Append(programs);

        auto files = CreateSectionCard(L"Recent files", snapshot.files, dark,
                                       cardWidth, stripCount);
        files.Margin({8, 0, 0, 0});
        strip.Children().Append(files);

        overlay.Children().Append(strip);
    }

    return overlay;
}

void RefreshPanelIfNeeded(
    const std::shared_ptr<PanelRefreshState>& state) {
    const ULONGLONG generation = g_entriesGeneration.load();
    if (state->refreshing || state->generation == generation) {
        return;
    }

    auto root = state->root.get();
    if (!root) {
        return;
    }

    state->refreshing = true;
    try {
        auto oldPanel = state->panel.get();
        if (oldPanel) {
            uint32_t index = 0;
            if (root.Children().IndexOf(oldPanel, index)) {
                root.Children().RemoveAt(index);
            }
        }

        EntrySnapshot snapshot = GetEntrySnapshot();
        auto newPanel = BuildAugmentationPanel(snapshot, root);
        root.Children().Append(newPanel);
        state->panel =
            winrt::make_weak(newPanel.as<wux::FrameworkElement>());
        state->generation = snapshot.generation;
    } catch (...) {
        Wh_Log(L"Failed to refresh Alt+Tab Plus panel: %08X",
               winrt::to_hresult());
    }
    state->refreshing = false;
}

void AttachAugmentation(const wux::FrameworkElement& switchItemList) {
    auto rootAndBackground =
        FindAltTabRootAndBackground(switchItemList);
    if (!rootAndBackground) {
        return;
    }

    auto [root, nativeBackground] = *rootAndBackground;
    if (IsAlreadyAugmented(root)) {
        return;
    }

    EntrySnapshot snapshot = GetEntrySnapshot();
    NativeLayoutState original{
        .margin = nativeBackground.Margin(),
        .horizontalAlignment = nativeBackground.HorizontalAlignment(),
        .verticalAlignment = nativeBackground.VerticalAlignment(),
        .maxWidth = nativeBackground.MaxWidth(),
    };

    ApplyNativeLayout(snapshot, root, nativeBackground);
    wuxc::Grid panel = BuildAugmentationPanel(snapshot, root);
    root.Children().Append(panel);

    auto refreshState = std::make_shared<PanelRefreshState>();
    refreshState->root = winrt::make_weak(root);
    refreshState->panel =
        winrt::make_weak(panel.as<wux::FrameworkElement>());
    refreshState->generation = snapshot.generation;
    winrt::event_token layoutUpdatedToken = root.LayoutUpdated(
        [refreshState](const wf::IInspectable&,
                       const wf::IInspectable&) {
            RefreshPanelIfNeeded(refreshState);
        });

    g_augmentedRoots.push_back({
        .root = winrt::make_weak(root),
        .nativeBackground = winrt::make_weak(nativeBackground),
        .panel = winrt::make_weak(panel.as<wux::FrameworkElement>()),
        .layoutUpdatedToken = layoutUpdatedToken,
        .refreshState = std::move(refreshState),
        .original = original,
    });
    g_xamlPanelCount++;

    Wh_Log(L"Attached Alt+Tab Plus panel");
}

void RemoveAugmentationsForCurrentThread() {
    const int panelCount =
        static_cast<int>(g_augmentedRoots.size());
    for (const auto& augmented : g_augmentedRoots) {
        try {
            auto root = augmented.root.get();
            if (root && augmented.layoutUpdatedToken.value) {
                root.LayoutUpdated(augmented.layoutUpdatedToken);
            }

            auto panel = augmented.refreshState
                             ? augmented.refreshState->panel.get()
                             : augmented.panel.get();
            if (root && panel) {
                uint32_t index = 0;
                if (root.Children().IndexOf(panel, index)) {
                    root.Children().RemoveAt(index);
                }
            }

            auto nativeBackground = augmented.nativeBackground.get();
            if (nativeBackground) {
                nativeBackground.Margin(augmented.original.margin);
                nativeBackground.HorizontalAlignment(
                    augmented.original.horizontalAlignment);
                nativeBackground.VerticalAlignment(
                    augmented.original.verticalAlignment);
                nativeBackground.MaxWidth(augmented.original.maxWidth);
            }
        } catch (...) {
            Wh_Log(L"Failed to remove an Alt+Tab Plus panel: %08X",
                   winrt::to_hresult());
        }
    }
    g_augmentedRoots.clear();
    if (panelCount) {
        g_xamlPanelCount.fetch_sub(panelCount);
    }
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &module);
    return module;
}

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : xamlDiagnostics_(site.as<IXamlDiagnostics>()) {
        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID parameter) -> DWORD {
                auto watcher =
                    static_cast<VisualTreeWatcher*>(parameter);
                HRESULT result =
                    watcher->xamlDiagnostics_.as<IVisualTreeService3>()
                        ->AdviseVisualTreeChange(watcher);
                watcher->Release();
                if (FAILED(result)) {
                    Wh_Log(L"AdviseVisualTreeChange failed: %08X", result);
                }
                return 0;
            },
            this, 0, nullptr);
        if (thread) {
            AddRef();
            CloseHandle(thread);
        }
    }

    void Unadvise() {
        HRESULT result =
            xamlDiagnostics_.as<IVisualTreeService3>()
                ->UnadviseVisualTreeChange(this);
        if (FAILED(result)) {
            Wh_Log(L"UnadviseVisualTreeChange failed: %08X", result);
        }
    }

   private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) override try {
        if (mutationType != Add || !g_initializedForThread ||
            !element.Type ||
            !wcsstr(element.Type, L"SwitchItemList")) {
            return S_OK;
        }

        wf::IInspectable inspectable;
        winrt::check_hresult(
            xamlDiagnostics_->GetIInspectableFromHandle(
                element.Handle,
                reinterpret_cast<::IInspectable**>(
                    winrt::put_abi(inspectable))));
        auto frameworkElement =
            inspectable.try_as<wux::FrameworkElement>();
        if (frameworkElement) {
            AttachAugmentation(frameworkElement);
        }
        return S_OK;
    } catch (...) {
        Wh_Log(L"Visual tree callback failed: %08X", winrt::to_hresult());
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle,
        VisualElementState,
        LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> xamlDiagnostics_;
};

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {D56AC3EE-3764-4EB0-AEB2-B52A93D1392D}
static constexpr CLSID CLSID_AltTabPlusTap = {
    0xd56ac3ee,
    0x3764,
    0x4eb0,
    {0xae, 0xb2, 0xb5, 0x2a, 0x93, 0xd1, 0x39, 0x2d}};

class AltTabPlusTap
    : public winrt::implements<AltTabPlusTap,
                               IObjectWithSite,
                               winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override try {
        if (g_visualTreeWatcher) {
            g_visualTreeWatcher->Unadvise();
            g_visualTreeWatcher = nullptr;
        }

        site_.copy_from(site);
        if (site_) {
            FreeLibrary(GetCurrentModuleHandle());
            g_visualTreeWatcher =
                winrt::make_self<VisualTreeWatcher>(site_);
        }
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid,
                                      void** result) noexcept override {
        return site_.as(iid, result);
    }

   private:
    winrt::com_ptr<IUnknown> site_;
};

template <typename T>
class SimpleFactory
    : public winrt::implements<SimpleFactory<T>,
                               IClassFactory,
                               winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE CreateInstance(
        IUnknown* outer,
        REFIID iid,
        void** result) override try {
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }
        *result = nullptr;
        return winrt::make<T>().as(iid, result);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) STDAPI DllGetClassObject(REFCLSID clsid,
                                               REFIID iid,
                                               void** result) try {
    if (clsid != CLSID_AltTabPlusTap) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    *result = nullptr;
    return winrt::make<SimpleFactory<AltTabPlusTap>>().as(iid, result);
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

std::atomic<bool> g_tapInitialized = false;
bool g_inInjectTap = false;

using InitializeXamlDiagnosticsEx_t =
    decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectTap() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR modulePath[MAX_PATH];
    DWORD length =
        GetModuleFileNameW(module, modulePath, ARRAYSIZE(modulePath));
    if (!length || length == ARRAYSIZE(modulePath)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HMODULE xamlModule = LoadLibraryExW(
        L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!xamlModule) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    auto initialize = reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
        GetProcAddress(xamlModule, "InitializeXamlDiagnosticsEx"));
    if (!initialize) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    g_inInjectTap = true;
    HRESULT result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (int index = 1; index <= 10000; index++) {
        WCHAR connectionName[64];
        _snwprintf_s(connectionName, _TRUNCATE,
                     L"VisualDiagConnection%d", index);
        result = initialize(connectionName, GetCurrentProcessId(), L"",
                            modulePath, CLSID_AltTabPlusTap, nullptr);
        if (result != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }
    g_inInjectTap = false;
    return result;
}

void InitializeForCurrentThread() {
    g_initializedForThread = true;
}

void UninitializeForCurrentThread() {
    RemoveAugmentationsForCurrentThread();
    g_initializedForThread = false;
}

void InitializeTap() {
    if (g_tapInitialized.exchange(true)) {
        return;
    }

    HRESULT result = InjectTap();
    if (FAILED(result)) {
        g_tapInitialized = false;
        Wh_Log(L"InitializeXamlDiagnosticsEx failed: %08X", result);
    }
}

void UninitializeTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->Unadvise();
        g_visualTreeWatcher = nullptr;
    }
    g_tapInitialized = false;
}

using RunFromWindowThreadProc =
    void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc procedure,
                         void* procedureParameter) {
    static const UINT runMessage = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParameter {
        RunFromWindowThreadProc procedure;
        void* parameter;
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        procedure(procedureParameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM, LPARAM parameter) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* message =
                    reinterpret_cast<const CWPSTRUCT*>(parameter);
                if (message->message == runMessage) {
                    auto* run = reinterpret_cast<RunParameter*>(
                        message->lParam);
                    run->procedure(run->parameter);
                }
            }
            return CallNextHookEx(nullptr, code, 0, parameter);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunParameter parameter{procedure, procedureParameter};
    SendMessageW(window, runMessage, 0,
                 reinterpret_cast<LPARAM>(&parameter));
    UnhookWindowsHookEx(hook);
    return true;
}

bool IsXamlHostWindow(HWND window, LPCWSTR suppliedClassName) {
    bool textualName =
        (reinterpret_cast<ULONG_PTR>(suppliedClassName) &
         ~static_cast<ULONG_PTR>(0xFFFF)) != 0;
    if (textualName &&
        (_wcsicmp(suppliedClassName,
                  L"XamlExplorerHostIslandWindow") == 0 ||
         _wcsicmp(suppliedClassName,
                  L"Shell_InputSwitchTopLevelWindow") == 0)) {
        return true;
    }

    WCHAR className[80];
    return GetClassNameW(window, className, ARRAYSIZE(className)) &&
           (_wcsicmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
            _wcsicmp(className,
                     L"Shell_InputSwitchTopLevelWindow") == 0);
}

void OnWindowCreated(HWND window, LPCWSTR suppliedClassName) {
    if (!window || !IsXamlHostWindow(window, suppliedClassName)) {
        return;
    }
    if (g_altTabShowThreadId.load() == GetCurrentThreadId()) {
        g_nativeAltTabHostWindow = window;
        ConstrainNativeAltTabWindow();
    }
    if (CopySettings().renderMode != RenderMode::Xaml) {
        return;
    }
    InitializeForCurrentThread();
    InitializeTap();
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD extendedStyle,
                                 LPCWSTR className,
                                 LPCWSTR windowName,
                                 DWORD style,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 HWND parent,
                                 HMENU menu,
                                 HINSTANCE instance,
                                 void* parameter) {
    HWND window = CreateWindowExW_Original(
        extendedStyle, className, windowName, style, x, y, width, height,
        parent, menu, instance, parameter);
    OnWindowCreated(window, className);
    return window;
}

using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD,
    LPCWSTR,
    LPCWSTR,
    DWORD,
    int,
    int,
    int,
    int,
    HWND,
    HMENU,
    HINSTANCE,
    void*,
    DWORD);
CreateWindowInBand_t CreateWindowInBand_Original;

HWND WINAPI CreateWindowInBand_Hook(DWORD extendedStyle,
                                    LPCWSTR className,
                                    LPCWSTR windowName,
                                    DWORD style,
                                    int x,
                                    int y,
                                    int width,
                                    int height,
                                    HWND parent,
                                    HMENU menu,
                                    HINSTANCE instance,
                                    void* parameter,
                                    DWORD band) {
    HWND window = CreateWindowInBand_Original(
        extendedStyle, className, windowName, style, x, y, width, height,
        parent, menu, instance, parameter, band);
    OnWindowCreated(window, className);
    return window;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(
    DWORD,
    LPCWSTR,
    LPCWSTR,
    DWORD,
    int,
    int,
    int,
    int,
    HWND,
    HMENU,
    HINSTANCE,
    void*,
    DWORD,
    DWORD);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;

HWND WINAPI CreateWindowInBandEx_Hook(DWORD extendedStyle,
                                      LPCWSTR className,
                                      LPCWSTR windowName,
                                      DWORD style,
                                      int x,
                                      int y,
                                      int width,
                                      int height,
                                      HWND parent,
                                      HMENU menu,
                                      HINSTANCE instance,
                                      void* parameter,
                                      DWORD band,
                                      DWORD typeFlags) {
    HWND window = CreateWindowInBandEx_Original(
        extendedStyle, className, windowName, style, x, y, width, height,
        parent, menu, instance, parameter, band, typeFlags);
    OnWindowCreated(window, className);
    return window;
}

std::vector<HWND> GetXamlHostWindows() {
    std::vector<HWND> result;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            DWORD processId = 0;
            GetWindowThreadProcessId(window, &processId);
            if (processId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[80];
            if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
                (_wcsicmp(className,
                          L"XamlExplorerHostIslandWindow") == 0 ||
                 _wcsicmp(className,
                          L"Shell_InputSwitchTopLevelWindow") == 0)) {
                reinterpret_cast<std::vector<HWND>*>(parameter)
                    ->push_back(window);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

HWND FindVisibleNativeAltTabWindow() {
    HMONITOR targetMonitor = g_companionMonitor.load();
    HWND bestWindow = nullptr;
    ULONGLONG bestArea = 0;
    for (HWND window : GetXamlHostWindows()) {
        if (!IsWindowVisible(window)) {
            continue;
        }

        DWORD cloaked = 0;
        if (SUCCEEDED(DwmGetWindowAttribute(
                window, DWMWA_CLOAKED, &cloaked,
                sizeof(cloaked))) &&
            cloaked) {
            continue;
        }

        RECT rect{};
        if (!GetWindowRect(window, &rect)) {
            continue;
        }
        if (targetMonitor &&
            MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) !=
                targetMonitor) {
            continue;
        }

        const ULONGLONG width =
            static_cast<ULONGLONG>(
                std::max<LONG>(0, rect.right - rect.left));
        const ULONGLONG height =
            static_cast<ULONGLONG>(
                std::max<LONG>(0, rect.bottom - rect.top));
        const ULONGLONG area = width * height;
        if (area > bestArea) {
            bestWindow = window;
            bestArea = area;
        }
    }
    return bestWindow;
}

void ConstrainNativeAltTabWindow() {
    RECT corridor{};
    if (!GetAdjustedAltTabCorridor(&corridor)) {
        return;
    }

    HWND window = g_nativeAltTabHostWindow.load();
    if (!window || !IsWindow(window)) {
        window = FindVisibleNativeAltTabWindow();
        if (!window) {
            return;
        }
        g_nativeAltTabHostWindow = window;
    }

    RECT current{};
    if (!GetWindowRect(window, &current)) {
        return;
    }

    const int width = corridor.right - corridor.left;
    const int height = current.bottom - current.top;
    if (width <= 0 || height <= 0) {
        return;
    }
    if (current.left == corridor.left &&
        current.right == corridor.right) {
        return;
    }

    SetWindowPos(window, nullptr, corridor.left, current.top, width,
                 height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

void InitializeExistingXamlHosts() {
    if (CopySettings().renderMode != RenderMode::Xaml) {
        return;
    }

    bool foundHost = false;
    for (HWND window : GetXamlHostWindows()) {
        foundHost = true;
        RunFromWindowThread(
            window,
            [](void*) { InitializeForCurrentThread(); }, nullptr);
    }
    if (foundHost) {
        InitializeTap();
    }
}

using XamlAltTabViewHost_Show_t =
    HRESULT(WINAPI*)(void* instance,
                     void* immersiveMonitor,
                     int flags,
                     void* initialView);
XamlAltTabViewHost_Show_t XamlAltTabViewHost_Show_Original;

using ITaskGroupWindowInformation_Position_t =
    HRESULT(WINAPI*)(void* instance, wf::Rect* rect);
ITaskGroupWindowInformation_Position_t
    ITaskGroupWindowInformation_Position_Original;

bool GetAdjustedAltTabRect(const wf::Rect& original,
                           wf::Rect* adjusted) {
    RECT corridor{};
    if (!GetAdjustedAltTabCorridor(&corridor)) {
        return false;
    }

    *adjusted = original;
    adjusted->X = static_cast<float>(corridor.left);
    adjusted->Width =
        static_cast<float>(corridor.right - corridor.left);
    return true;
}

HRESULT WINAPI ITaskGroupWindowInformation_Position_Hook(
    void* instance,
    wf::Rect* rect) {
    if (g_altTabShowThreadId.load() != GetCurrentThreadId()) {
        return ITaskGroupWindowInformation_Position_Original(instance,
                                                              rect);
    }

    g_altTabShowThreadId = 0;
    wf::Rect adjusted{};
    if (rect && GetAdjustedAltTabRect(*rect, &adjusted)) {
        return ITaskGroupWindowInformation_Position_Original(
            instance, &adjusted);
    }
    return ITaskGroupWindowInformation_Position_Original(instance,
                                                          rect);
}

HRESULT WINAPI XamlAltTabViewHost_Show_Hook(void* instance,
                                            void* immersiveMonitor,
                                            int flags,
                                            void* initialView) {
    RequestCompanionPanels();
    const DWORD threadId = GetCurrentThreadId();
    g_altTabShowThreadId = threadId;
    HRESULT result = XamlAltTabViewHost_Show_Original(
        instance, immersiveMonitor, flags, initialView);
    ConstrainNativeAltTabWindow();
    if (g_altTabShowThreadId.load() == threadId) {
        g_altTabShowThreadId = 0;
    }
    return result;
}

bool HookAltTabShow() {
    HMODULE twinui =
        LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                       LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!twinui) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll: %u", GetLastError());
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
            &XamlAltTabViewHost_Show_Original,
            XamlAltTabViewHost_Show_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Internal_Shell_TaskGroups_ITaskGroupWindowInformation::Position(struct winrt::Windows::Foundation::Rect const &)const )",
                LR"(public: __cdecl winrt::impl::consume_Windows_Internal_Shell_TaskGroups_ITaskGroupWindowInformation::Position(struct winrt::Windows::Foundation::Rect const &)const)",
                LR"(public: __cdecl winrt::impl::consume_Windows_Internal_Shell_TaskGroups_ITaskGroupWindowInformation<struct winrt::Windows::Internal::Shell::TaskGroups::ITaskGroupWindowInformation>::Position(struct winrt::Windows::Foundation::Rect const &)const)",
            },
            &ITaskGroupWindowInformation_Position_Original,
            ITaskGroupWindowInformation_Position_Hook,
            true,
        },
    };
    if (!WindhawkUtils::HookSymbols(twinui, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Couldn't hook XamlAltTabViewHost::Show");
        return false;
    }
    if (!ITaskGroupWindowInformation_Position_Original) {
        Wh_Log(L"Native thumbnail adjustment isn't available on this Windows build");
    }
    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");
    LoadSettings();
    LoadUsageRecords();
    StartWorker();
    StartCompanionThread();
    HookAltTabShow();

    WindhawkUtils::SetFunctionHook(
        CreateWindowExW, CreateWindowExW_Hook,
        &CreateWindowExW_Original);

    HMODULE user32 = LoadLibraryExW(
        L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) {
        auto createWindowInBand =
            reinterpret_cast<CreateWindowInBand_t>(
                GetProcAddress(user32, "CreateWindowInBand"));
        if (createWindowInBand) {
            WindhawkUtils::SetFunctionHook(
                createWindowInBand, CreateWindowInBand_Hook,
                &CreateWindowInBand_Original);
        }

        auto createWindowInBandEx =
            reinterpret_cast<CreateWindowInBandEx_t>(
                GetProcAddress(user32, "CreateWindowInBandEx"));
        if (createWindowInBandEx) {
            WindhawkUtils::SetFunctionHook(
                createWindowInBandEx, CreateWindowInBandEx_Hook,
                &CreateWindowInBandEx_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    InitializeExistingXamlHosts();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Reloading settings");
    StopWorker();
    UninitializeTap();
    if (g_companionThreadId) {
        PostThreadMessageW(g_companionThreadId, WM_ALT_TAB_PLUS_HIDE, 0, 0);
    }

    for (HWND window : GetXamlHostWindows()) {
        RunFromWindowThread(
            window,
            [](void*) { UninitializeForCurrentThread(); }, nullptr);
    }

    LoadSettings();
    {
        std::lock_guard lock(g_entriesMutex);
        g_commonPrograms.clear();
        g_recentFiles.clear();
        g_recentFileGroups.clear();
    }
    StartWorker();
    InitializeExistingXamlHosts();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");
    StopWorker();
    UninitializeTap();
    StopCompanionThread();

    for (HWND window : GetXamlHostWindows()) {
        RunFromWindowThread(
            window,
            [](void*) { UninitializeForCurrentThread(); }, nullptr);
    }
}
