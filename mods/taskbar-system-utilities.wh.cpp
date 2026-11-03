// ==WindhawkMod==
// @id              taskbar-system-utilities
// @name            Taskbar System Utilities
// @description     Native WinUI taskbar tools: Restart Explorer, Clean Temp, Flush DNS, Reset GPU and Get IP.
// @version         1.0
// @author          Marco Petrucci
// @github          https://github.com/marco00petrucci
// @donateUrl       https://www.paypal.me/marco00petrucci
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lws2_32 -lwininet -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar System Utilities

Modern system utilities seamlessly integrated into the Windows 11 Taskbar XAML
context menu. This mod provides a suite of native tools with a **modern WinUI
interface**, **asynchronous non-blocking execution**, and **zero UAC prompts**
for taskbar actions.

## Features
- **Restart Explorer**: Instantly restarts `explorer.exe` cleanly.
- **Clean Temp**: Asynchronous C++ recursive cleaner for `%TEMP%` and
  accessible files in `%WINDIR%\Temp` with a sleek WinUI summary card.
- **Clean DNS Cache**: Flushes the DNS resolver cache directly in memory via
  `dnsapi.dll!DnsFlushResolverCache`.
- **Restart Graphics Driver**: Triggers the native `Win + Ctrl + Shift + B`
  kernel WDDM reset without UAC prompts.
- **Get Current IP**: Asynchronously queries Local IPv4 (WinSock) and Public IP
  (WinINet), displaying a modern WinUI card.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showRestartExplorer: true
  $name: Enable Restart Explorer
  $description: Show Restart Explorer in taskbar menu.
- textRestartExplorer: Restart Explorer
  $name: Restart Explorer Label
  $description: Custom text for Restart Explorer.
- showCleanTemp: true
  $name: Enable Clean Temp
  $description: Show Clean Temp in taskbar menu.
- textCleanTemp: Clean Temp
  $name: Clean Temp Label
  $description: Custom text for Clean Temp.
- cleanRecycleBin: false
  $name: Empty Recycle Bin on Clean Temp
  $description: Also empty Recycle Bin when cleaning temp.
- notifyCleanTemp: true
  $name: Show WinUI Dialog after Clean Temp
  $description: Show modern WinUI summary dialog after cleaning.
- showFlushDns: true
  $name: Enable Clean DNS Cache
  $description: Show Clean DNS Cache in taskbar menu.
- textFlushDns: Clean DNS Cache
  $name: Clean DNS Cache Label
  $description: Custom text for Clean DNS Cache.
- notifyFlushDns: true
  $name: Show WinUI Dialog after Clean DNS
  $description: Show modern WinUI dialog when DNS cache is flushed.
- showRestartGpu: true
  $name: Enable Restart Graphics Driver
  $description: Show Restart Graphics Driver in taskbar menu.
- textRestartGpu: Restart Graphics Driver
  $name: Restart Graphics Driver Label
  $description: Custom text for Restart Graphics Driver.
- showGetIp: true
  $name: Enable Get Current IP
  $description: Show Get Current IP in taskbar menu.
- textGetIp: Get Current IP
  $name: Get Current IP Label
  $description: Custom text for Get Current IP.
- copyIpToClipboard: true
  $name: Auto-Copy IP to Clipboard
  $description: Copy detected IPs to clipboard automatically.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <windows.h>
#include <wininet.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

#include <atomic>
#include <cctype>
#include <chrono>
#include <cwchar>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wu = winrt::Windows::UI;
namespace wuc = winrt::Windows::UI::Core;
namespace wut = winrt::Windows::UI::Text;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// ============================================================================
// Global State & Lifetime Synchronization
// ============================================================================

struct ModSettings {
    bool showRestartExplorer = true;
    std::wstring textRestartExplorer = L"Restart Explorer";

    bool showCleanTemp = true;
    std::wstring textCleanTemp = L"Clean Temp";
    bool cleanRecycleBin = false;
    bool notifyCleanTemp = true;

    bool showFlushDns = true;
    std::wstring textFlushDns = L"Clean DNS Cache";
    bool notifyFlushDns = true;

    bool showRestartGpu = true;
    std::wstring textRestartGpu = L"Restart Graphics Driver";

    bool showGetIp = true;
    std::wstring textGetIp = L"Get Current IP";
    bool copyIpToClipboard = true;
};

std::mutex g_settingsMutex;
ModSettings g_settings;

static constexpr wchar_t kGuardItemName[] = L"WindhawkSystemUtilitiesGuardItem";

std::atomic<bool> g_taskbarViewModuleHooked{false};
std::atomic<bool> g_restartStarted{false};
std::atomic<bool> g_unloading{false};
std::atomic<int> g_activeAsyncTasks{0};

thread_local int g_taskbarSettingsMenuDepth = 0;
thread_local bool g_currentMenuInjected = false;

std::mutex g_targetMutex;
winrt::weak_ref<wux::FrameworkElement> g_lastTargetWeak;
winrt::weak_ref<wuc::CoreDispatcher> g_dispatcherWeak;

void SetLastTarget(wux::FrameworkElement const& target) {
    std::lock_guard<std::mutex> lock(g_targetMutex);
    g_lastTargetWeak = target;
    g_dispatcherWeak = target.Dispatcher();
}

wux::FrameworkElement GetLastTargetOnUIThread() {
    std::lock_guard<std::mutex> lock(g_targetMutex);
    return g_lastTargetWeak.get();
}

wuc::CoreDispatcher GetLastDispatcher() {
    std::lock_guard<std::mutex> lock(g_targetMutex);
    return g_dispatcherWeak.get();
}

template <typename F>
void RunAsyncTracked(F&& fn) {
    if (g_unloading.load()) {
        return;
    }

    g_activeAsyncTasks.fetch_add(1);
    try {
        std::thread([task = std::forward<F>(fn)]() mutable {
            try {
                if (!g_unloading.load()) {
                    task();
                }
            } catch (...) {
                Wh_Log(L"Background task threw an exception");
            }
            g_activeAsyncTasks.fetch_sub(1);
        }).detach();
    } catch (...) {
        g_activeAsyncTasks.fetch_sub(1);
    }
}

ModSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// ============================================================================
// WinUI 11 Flyout Card Builder
// ============================================================================

using DialogRow = std::pair<std::wstring, std::wstring>;

wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(a, r, g, b));
}

wuxc::FontIcon CreateFluentIcon(const wchar_t* glyph, double size = 16.0) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    icon.FontSize(size);
    return icon;
}

void ShowModernWinUICardOnUIThread(const std::wstring& titleText,
                                   const wchar_t* glyphIcon,
                                   const std::vector<DialogRow>& rows,
                                   const std::wstring& footerNote = L"") {
    if (g_unloading.load()) {
        return;
    }

    try {
        auto target = GetLastTargetOnUIThread();
        if (!target || !target.XamlRoot()) {
            Wh_Log(
                L"Cannot display WinUI card: taskbar target or XamlRoot "
                L"unavailable");
            return;
        }

        wuxc::Flyout flyout;
        flyout.ShouldConstrainToRootBounds(false);
        flyout.Placement(wuxcp::FlyoutPlacementMode::Top);
        flyout.XamlRoot(target.XamlRoot());

        // Strip default FlyoutPresenter padding/border so our card is seamless
        wux::Interop::TypeName presenterType{
            winrt::hstring{winrt::name_of<wuxc::FlyoutPresenter>()},
            wux::Interop::TypeKind::Metadata};
        wux::Style presenterStyle(presenterType);
        presenterStyle.Setters().Append(
            wux::Setter(wuxc::Control::PaddingProperty(),
                        winrt::box_value(wux::Thickness{0, 0, 0, 0})));
        presenterStyle.Setters().Append(
            wux::Setter(wuxc::Control::BorderThicknessProperty(),
                        winrt::box_value(wux::Thickness{0, 0, 0, 0})));
        presenterStyle.Setters().Append(wux::Setter(
            wuxc::Control::BackgroundProperty(), MakeBrush(0, 0, 0, 0)));
        flyout.FlyoutPresenterStyle(presenterStyle);

        // Main Windows 11 Dark Acrylic-like Card Container (#202020 + #383838
        // border)
        wuxc::Border card;
        card.Background(MakeBrush(255, 32, 32, 32));
        card.BorderBrush(MakeBrush(255, 58, 58, 58));
        card.BorderThickness({1, 1, 1, 1});
        card.CornerRadius({12, 12, 12, 12});
        card.Padding({22, 20, 22, 18});
        card.Width(360);

        wuxc::StackPanel mainStack;
        mainStack.Spacing(14);

        // 1. Header Row (Accent Icon + Bold Title)
        wuxc::StackPanel headerStack;
        headerStack.Orientation(wuxc::Orientation::Horizontal);
        headerStack.Spacing(12);

        wuxc::Border iconBadge;
        iconBadge.Background(MakeBrush(35, 96, 205, 255));
        iconBadge.CornerRadius({8, 8, 8, 8});
        iconBadge.Width(36);
        iconBadge.Height(36);

        auto headerIcon = CreateFluentIcon(glyphIcon, 18.0);
        headerIcon.Foreground(MakeBrush(255, 96, 205, 255));
        headerIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        headerIcon.VerticalAlignment(wux::VerticalAlignment::Center);
        iconBadge.Child(headerIcon);
        headerStack.Children().Append(iconBadge);

        wuxc::TextBlock titleBlock;
        titleBlock.Text(titleText);
        titleBlock.FontWeight(wut::FontWeights::SemiBold());
        titleBlock.FontSize(16);
        titleBlock.Foreground(MakeBrush(255, 255, 255, 255));
        titleBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        headerStack.Children().Append(titleBlock);

        mainStack.Children().Append(headerStack);

        // 2. Inner Content Box for structured Key-Value metrics
        wuxc::Border metricsBox;
        metricsBox.Background(MakeBrush(255, 43, 43, 43));
        metricsBox.BorderBrush(MakeBrush(255, 53, 53, 53));
        metricsBox.BorderThickness({1, 1, 1, 1});
        metricsBox.CornerRadius({8, 8, 8, 8});
        metricsBox.Padding({14, 12, 14, 12});

        wuxc::StackPanel rowsStack;
        rowsStack.Spacing(8);

        for (const auto& [label, val] : rows) {
            wuxc::Grid rowGrid;
            wuxc::ColumnDefinition col0;
            col0.Width(wux::GridLength{1.0, wux::GridUnitType::Auto});
            wuxc::ColumnDefinition col1;
            col1.Width(wux::GridLength{1.0, wux::GridUnitType::Star});
            rowGrid.ColumnDefinitions().Append(col0);
            rowGrid.ColumnDefinitions().Append(col1);

            wuxc::TextBlock lblBlock;
            lblBlock.Text(label);
            lblBlock.FontSize(13);
            lblBlock.Foreground(MakeBrush(255, 175, 175, 175));
            lblBlock.VerticalAlignment(wux::VerticalAlignment::Center);
            lblBlock.Margin({0, 0, 16, 0});
            rowGrid.Children().Append(lblBlock);
            wuxc::Grid::SetColumn(lblBlock, 0);

            wuxc::TextBlock valBlock;
            valBlock.Text(val);
            valBlock.FontSize(13);
            valBlock.FontWeight(wut::FontWeights::SemiBold());
            valBlock.Foreground(MakeBrush(255, 245, 245, 245));
            valBlock.HorizontalAlignment(wux::HorizontalAlignment::Right);
            valBlock.VerticalAlignment(wux::VerticalAlignment::Center);
            valBlock.TextAlignment(wux::TextAlignment::Right);
            valBlock.TextWrapping(wux::TextWrapping::Wrap);
            valBlock.IsTextSelectionEnabled(true);
            rowGrid.Children().Append(valBlock);
            wuxc::Grid::SetColumn(valBlock, 1);

            rowsStack.Children().Append(rowGrid);
        }

        metricsBox.Child(rowsStack);
        mainStack.Children().Append(metricsBox);

        // 3. Footer Bar (Optional status badge + Pill Button)
        wuxc::Grid footerGrid;

        if (!footerNote.empty()) {
            wuxc::StackPanel noteStack;
            noteStack.Orientation(wuxc::Orientation::Horizontal);
            noteStack.Spacing(6);
            noteStack.VerticalAlignment(wux::VerticalAlignment::Center);

            auto checkIcon = CreateFluentIcon(L"\xE73E", 13.0);
            checkIcon.Foreground(MakeBrush(255, 108, 203, 95));
            noteStack.Children().Append(checkIcon);

            wuxc::TextBlock noteBlock;
            noteBlock.Text(footerNote);
            noteBlock.FontSize(12);
            noteBlock.Foreground(MakeBrush(255, 160, 160, 160));
            noteStack.Children().Append(noteBlock);

            footerGrid.Children().Append(noteStack);
        }

        wuxc::Button okButton;
        okButton.Content(winrt::box_value(L"OK"));
        okButton.Width(76);
        okButton.Height(30);
        okButton.CornerRadius({6, 6, 6, 6});
        okButton.Background(MakeBrush(255, 0, 120, 212));
        okButton.Foreground(MakeBrush(255, 255, 255, 255));
        okButton.BorderThickness({0, 0, 0, 0});
        okButton.HorizontalAlignment(wux::HorizontalAlignment::Right);

        winrt::weak_ref<wuxc::Flyout> weakFlyout(flyout);
        okButton.Click(
            [weakFlyout](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (auto fl = weakFlyout.get()) {
                    fl.Hide();
                }
            });

        footerGrid.Children().Append(okButton);
        mainStack.Children().Append(footerGrid);

        card.Child(mainStack);
        flyout.Content(card);
        flyout.ShowAt(target);
    } catch (...) {
        Wh_Log(L"Failed to display WinUI Flyout card");
    }
}

// Dispatches the WinUI Flyout creation back onto the Taskbar XAML UI thread
void DispatchWinUICard(std::wstring titleText,
                       const wchar_t* glyphIcon,
                       std::vector<DialogRow> rows,
                       std::wstring footerNote = L"") {
    auto dispatcher = GetLastDispatcher();
    if (!dispatcher) {
        Wh_Log(L"Cannot dispatch WinUI card: CoreDispatcher unavailable");
        return;
    }

    try {
        dispatcher.RunAsync(
            wuc::CoreDispatcherPriority::Normal,
            [title = std::move(titleText), glyphIcon, rows = std::move(rows),
             footer = std::move(footerNote)]() {
                ShowModernWinUICardOnUIThread(title, glyphIcon, rows, footer);
            });
    } catch (...) {
        Wh_Log(L"CoreDispatcher::RunAsync failed");
    }
}

bool CopyTextToClipboard(const std::wstring& text) {
    HWND tempOwner = CreateWindowExW(0, L"STATIC", nullptr, 0, 0, 0, 0, 0,
                                     HWND_MESSAGE, nullptr, nullptr, nullptr);
    HWND owner = tempOwner ? tempOwner : GetShellWindow();

    bool opened = false;
    for (int i = 0; i < 6 && !opened; ++i) {
        opened = OpenClipboard(owner) != FALSE;
        if (!opened) {
            Sleep(25);
        }
    }

    bool copied = false;
    if (opened) {
        EmptyClipboard();
        size_t byteSize = (text.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, byteSize);
        if (hMem) {
            if (void* ptr = GlobalLock(hMem)) {
                memcpy(ptr, text.c_str(), byteSize);
                GlobalUnlock(hMem);
                if (SetClipboardData(CF_UNICODETEXT, hMem)) {
                    copied = true;
                } else {
                    GlobalFree(hMem);
                }
            } else {
                GlobalFree(hMem);
            }
        }
        CloseClipboard();
    }

    if (tempOwner) {
        DestroyWindow(tempOwner);
    }
    return copied;
}

// ============================================================================
// Native Action Implementations
// ============================================================================

// Restart Explorer
void OnRestartExplorerClick(wf::IInspectable const&,
                            wux::RoutedEventArgs const&) {
    if (g_restartStarted.exchange(true)) {
        return;
    }

    wchar_t commandLine[] =
        L"cmd.exe /d /c \"taskkill /f /im explorer.exe >nul 2>nul & start \"\" "
        L"explorer.exe\"";

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {};

    if (CreateProcessW(nullptr, commandLine, nullptr, nullptr, FALSE,
                       CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, nullptr,
                       nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        g_restartStarted = false;
    }
}

// Native C++ Recursive Temp Cleaner
struct CleanStats {
    DWORD filesDeleted = 0;
    DWORD filesSkipped = 0;
    ULONGLONG bytesFreed = 0;
};

std::wstring ToExtendedPath(const std::wstring& path) {
    if (path.rfind(L"\\\\?\\", 0) == 0) {
        return path;
    }
    return L"\\\\?\\" + path;
}

bool IsSafeCleanTarget(const std::wstring& path) {
    if (path.size() < 4) {
        return false;
    }

    auto equalsEnv = [&](const wchar_t* var) {
        wchar_t buf[MAX_PATH] = {};
        DWORD n = GetEnvironmentVariableW(var, buf, MAX_PATH);
        if (n == 0 || n >= MAX_PATH) {
            return false;
        }
        std::wstring v(buf);
        while (!v.empty() && v.back() == L'\\') {
            v.pop_back();
        }
        return _wcsicmp(v.c_str(), path.c_str()) == 0;
    };

    return !(equalsEnv(L"USERPROFILE") || equalsEnv(L"SystemRoot") ||
             equalsEnv(L"SystemDrive"));
}

void CleanDirectoryRecursive(const std::wstring& dirPath,
                             bool removeRoot,
                             CleanStats& stats) {
    if (g_unloading.load()) {
        return;
    }

    std::wstring searchMask = dirPath + L"\\*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchMask.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }

        std::wstring fullPath = dirPath + L"\\" + fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                CleanDirectoryRecursive(fullPath, true, stats);
            }
        } else {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
                SetFileAttributesW(
                    fullPath.c_str(),
                    fd.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
            }

            ULONGLONG fileSize =
                (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) |
                fd.nFileSizeLow;
            if (DeleteFileW(fullPath.c_str())) {
                stats.filesDeleted++;
                stats.bytesFreed += fileSize;
            } else {
                stats.filesSkipped++;
            }
        }
    } while (!g_unloading.load() && FindNextFileW(hFind, &fd));

    FindClose(hFind);

    if (removeRoot && !g_unloading.load()) {
        RemoveDirectoryW(dirPath.c_str());
    }
}

void OnCleanTempClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();
        CleanStats stats;

        wchar_t tempPath[MAX_PATH] = {};
        if (GetTempPathW(MAX_PATH, tempPath) > 0) {
            std::wstring userTemp(tempPath);
            while (!userTemp.empty() && userTemp.back() == L'\\') {
                userTemp.pop_back();
            }
            if (IsSafeCleanTarget(userTemp)) {
                CleanDirectoryRecursive(ToExtendedPath(userTemp), false, stats);
            } else {
                Wh_Log(L"Temp path rejected by safety check: %s",
                       userTemp.c_str());
            }
        }

        wchar_t winDir[MAX_PATH] = {};
        if (GetWindowsDirectoryW(winDir, MAX_PATH) >= 3) {
            std::wstring sysTemp = std::wstring(winDir) + L"\\Temp";
            CleanDirectoryRecursive(ToExtendedPath(sysTemp), false, stats);
        }

        if (cfg.cleanRecycleBin && !g_unloading.load()) {
            SHEmptyRecycleBinW(
                nullptr, nullptr,
                SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
        }

        if (cfg.notifyCleanTemp && !g_unloading.load()) {
            double mbFreed =
                static_cast<double>(stats.bytesFreed) / (1024.0 * 1024.0);
            wchar_t mbBuf[64] = {};
            swprintf_s(mbBuf, L"%.2f MB", mbFreed);

            std::vector<DialogRow> rows = {
                {L"Deleted files", std::to_wstring(stats.filesDeleted)},
                {L"In-use skipped", std::to_wstring(stats.filesSkipped)},
                {L"Space recovered", mbBuf},
            };

            std::wstring footer =
                (stats.filesDeleted == 0 && stats.filesSkipped == 0)
                    ? L"No removable temp files found"
                    : L"Cleanup completed";

            DispatchWinUICard(L"Temporary Files Cleaned", L"\xE74D",
                              std::move(rows), std::move(footer));
        }
    });
}

// Clean DNS Cache
using DnsFlushResolverCache_t = BOOL(WINAPI*)();

void OnFlushDnsClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    BOOL flushed = FALSE;
    if (HMODULE hDnsApi = LoadLibraryW(L"dnsapi.dll")) {
        auto pFlush = reinterpret_cast<DnsFlushResolverCache_t>(
            GetProcAddress(hDnsApi, "DnsFlushResolverCache"));
        if (pFlush) {
            flushed = pFlush();
        }
        FreeLibrary(hDnsApi);
    }

    auto cfg = GetSettingsSnapshot();
    if (cfg.notifyFlushDns) {
        std::vector<DialogRow> rows = {
            {L"Resolver Cache", flushed ? L"Flushed" : L"Failed"},
            {L"Method", L"dnsapi.dll (In-Memory)"},
        };
        ShowModernWinUICardOnUIThread(
            L"DNS Cache", L"\xE774", rows,
            flushed ? L"Cache cleared successfully" : L"Error flushing cache");
    }
}

// Restart Graphics Driver
void OnRestartGpuClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        Sleep(150);
        static constexpr WORD kKeys[] = {VK_LWIN, VK_CONTROL, VK_SHIFT, 'B'};
        INPUT inputs[8] = {};

        for (size_t i = 0; i < 4; ++i) {
            inputs[i].type = INPUT_KEYBOARD;
            inputs[i].ki.wVk = kKeys[i];

            inputs[i + 4].type = INPUT_KEYBOARD;
            inputs[i + 4].ki.wVk = kKeys[3 - i];
            inputs[i + 4].ki.dwFlags = KEYEVENTF_KEYUP;
        }

        if (SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT)) !=
            ARRAYSIZE(inputs)) {
            Wh_Log(L"SendInput failed: %lu", GetLastError());
        }
    });
}

// Get Current IP
std::wstring GetLocalIPv4() {
    std::wstring result = L"Disconnected";
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock != INVALID_SOCKET) {
            sockaddr_in serv = {};
            serv.sin_family = AF_INET;
            InetPtonW(AF_INET, L"8.8.8.8", &serv.sin_addr);
            serv.sin_port = htons(53);

            if (connect(sock, reinterpret_cast<sockaddr*>(&serv),
                        sizeof(serv)) == 0) {
                sockaddr_in name = {};
                int namelen = sizeof(name);
                if (getsockname(sock, reinterpret_cast<sockaddr*>(&name),
                                &namelen) == 0) {
                    wchar_t buffer[INET_ADDRSTRLEN] = {};
                    if (InetNtopW(AF_INET, &name.sin_addr, buffer,
                                  INET_ADDRSTRLEN)) {
                        result = buffer;
                    }
                }
            }
            closesocket(sock);
        }
        WSACleanup();
    }
    return result;
}

std::wstring GetPublicIP() {
    std::wstring result = L"Unavailable";
    HINTERNET hInternet =
        InternetOpenW(L"WindhawkSystemUtilities/1.0",
                      INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (hInternet) {
        DWORD timeout = 2500;
        InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                           sizeof(timeout));
        InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                           sizeof(timeout));

        HINTERNET hConnect = InternetOpenUrlW(
            hInternet, L"https://api.ipify.org", nullptr, 0,
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                INTERNET_FLAG_SECURE,
            0);
        if (hConnect) {
            char buffer[64] = {};
            DWORD bytesRead = 0;
            if (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1,
                                 &bytesRead) &&
                bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::string raw(buffer);
                while (!raw.empty() &&
                       std::isspace(static_cast<unsigned char>(raw.back()))) {
                    raw.pop_back();
                }

                in_addr addr4;
                in6_addr addr6;
                if (InetPtonA(AF_INET, raw.c_str(), &addr4) == 1 ||
                    InetPtonA(AF_INET6, raw.c_str(), &addr6) == 1) {
                    wchar_t wbuffer[64] = {};
                    MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), -1, wbuffer,
                                        64);
                    result = wbuffer;
                }
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }
    return result;
}

void OnGetIpClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();

        std::wstring localIp = GetLocalIPv4();
        std::wstring publicIp = GetPublicIP();

        if (g_unloading.load()) {
            return;
        }

        std::wstring footer;
        if (cfg.copyIpToClipboard) {
            if (CopyTextToClipboard(L"Local IPv4: " + localIp +
                                    L"\r\nPublic IP: " + publicIp)) {
                footer = L"Copied to clipboard";
            }
        }

        std::vector<DialogRow> rows = {
            {L"Local IPv4", localIp},
            {L"Public IP", publicIp},
        };

        DispatchWinUICard(L"Network IP Addresses", L"\xE968", std::move(rows),
                          std::move(footer));
    });
}

// ============================================================================
// Taskbar Context Menu Injection Hooks
// ============================================================================

bool HStringEquals(winrt::hstring const& value, const wchar_t* text) {
    return std::wstring_view(value.c_str(), value.size()) == text;
}

bool IsNamedItem(wuxc::MenuFlyoutItemBase const& baseItem,
                 const wchar_t* name) {
    try {
        if (auto frameworkElement = baseItem.try_as<wux::FrameworkElement>()) {
            return HStringEquals(frameworkElement.Name(), name);
        }
    } catch (...) {
    }
    return false;
}

bool IsSeparator(wuxc::MenuFlyoutItemBase const& baseItem) {
    try {
        return !!baseItem.try_as<wuxc::MenuFlyoutSeparator>();
    } catch (...) {
    }
    return false;
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

struct MenuItemSpec {
    bool enabled;
    std::wstring text;
    const wchar_t* glyph;
    void (*handler)(wf::IInspectable const&, wux::RoutedEventArgs const&);
};

void AppendInjectedItems(void* vectorThis) {
    auto cfg = GetSettingsSnapshot();

    const MenuItemSpec specs[] = {
        {cfg.showRestartExplorer, cfg.textRestartExplorer, L"\xE72C",
         &OnRestartExplorerClick},
        {cfg.showCleanTemp, cfg.textCleanTemp, L"\xE74D", &OnCleanTempClick},
        {cfg.showFlushDns, cfg.textFlushDns, L"\xE774", &OnFlushDnsClick},
        {cfg.showRestartGpu, cfg.textRestartGpu, L"\xE7F4", &OnRestartGpuClick},
        {cfg.showGetIp, cfg.textGetIp, L"\xE968", &OnGetIpClick},
    };

    bool anyInjected = false;
    for (const auto& spec : specs) {
        if (!spec.enabled) {
            continue;
        }

        wuxc::MenuFlyoutItem menuItem;
        if (!anyInjected) {
            menuItem.Name(kGuardItemName);
        }
        menuItem.Text(spec.text);
        menuItem.Icon(CreateFluentIcon(spec.glyph));
        menuItem.Click(wux::RoutedEventHandler{spec.handler});

        MenuFlyoutItemBaseVector_Append_Original(vectorThis, menuItem);
        anyInjected = true;
    }

    if (anyInjected) {
        wuxc::MenuFlyoutSeparator separator;
        MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);
        g_currentMenuInjected = true;
    }
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    if (g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (!IsSeparator(item) && !IsNamedItem(item, kGuardItemName)) {
                AppendInjectedItems(pThis);
            }
        } catch (...) {
            Wh_Log(L"Taskbar menu append inspection failed");
        }
    }

    MenuFlyoutItemBaseVector_Append_Original(pThis, item);
}

struct ScopedTaskbarSettingsMenuBuild {
    bool outer = false;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_currentMenuInjected = false;
        }
    }

    ~ScopedTaskbarSettingsMenuBuild() { g_taskbarSettingsMenuDepth--; }
};

using ContextMenus_ShowTaskbarSettingsContextMenu_t =
    void(__cdecl*)(wux::FrameworkElement const& target,
                   void* taskbarSettings,
                   wuxi::ContextRequestedEventArgs const& args,
                   unsigned long long options);
ContextMenus_ShowTaskbarSettingsContextMenu_t
    ContextMenus_ShowTaskbarSettingsContextMenu_Original;

void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(
    wux::FrameworkElement const& target,
    void* taskbarSettings,
    wuxi::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    try {
        if (target && target.XamlRoot()) {
            SetLastTarget(target);
        }
    } catch (...) {
    }

    ScopedTaskbarSettingsMenuBuild scopedBuild;

    ContextMenus_ShowTaskbarSettingsContextMenu_Original(
        target, taskbarSettings, args, options);
}

// ============================================================================
// Symbol Hooking & Mod Lifecycle
// ============================================================================

using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW* versionInfo);

DWORD GetWindowsBuild() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto rtlGetVersion = ntdll ? reinterpret_cast<RtlGetVersion_t>(
                                     GetProcAddress(ntdll, "RtlGetVersion"))
                               : nullptr;
    if (!rtlGetVersion) {
        return 0;
    }

    OSVERSIONINFOW versionInfo = {};
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

    if (rtlGetVersion(&versionInfo) != 0) {
        return 0;
    }

    return versionInfo.dwBuildNumber;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
            },
            &ContextMenus_ShowTaskbarSettingsContextMenu_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )",
            },
            &MenuFlyoutItemBaseVector_Append_Original,
            MenuFlyoutItemBaseVector_Append_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, bool applyHookOperations) {
    if (!module || g_taskbarViewModuleHooked.load()) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewModuleHooked.exchange(true)) {
        return;
    }

    if (HookTaskbarViewDllSymbols(module)) {
        if (applyHookOperations) {
            Wh_ApplyHookOperations();
        }
    } else {
        Wh_Log(L"Failed to hook Taskbar.View symbols");
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, true);
    }
    return module;
}

std::wstring LoadStringSetting(const wchar_t* name,
                               const wchar_t* defaultValue) {
    auto val = WindhawkUtils::StringSetting::make(name);
    return (val.get() && val.get()[0] != L'\0') ? val.get() : defaultValue;
}

void LoadSettings() {
    ModSettings fresh;
    fresh.showRestartExplorer = Wh_GetIntSetting(L"showRestartExplorer");
    fresh.textRestartExplorer =
        LoadStringSetting(L"textRestartExplorer", L"Restart Explorer");

    fresh.showCleanTemp = Wh_GetIntSetting(L"showCleanTemp");
    fresh.textCleanTemp = LoadStringSetting(L"textCleanTemp", L"Clean Temp");
    fresh.cleanRecycleBin = Wh_GetIntSetting(L"cleanRecycleBin");
    fresh.notifyCleanTemp = Wh_GetIntSetting(L"notifyCleanTemp");

    fresh.showFlushDns = Wh_GetIntSetting(L"showFlushDns");
    fresh.textFlushDns = LoadStringSetting(L"textFlushDns", L"Clean DNS Cache");
    fresh.notifyFlushDns = Wh_GetIntSetting(L"notifyFlushDns");

    fresh.showRestartGpu = Wh_GetIntSetting(L"showRestartGpu");
    fresh.textRestartGpu =
        LoadStringSetting(L"textRestartGpu", L"Restart Graphics Driver");

    fresh.showGetIp = Wh_GetIntSetting(L"showGetIp");
    fresh.textGetIp = LoadStringSetting(L"textGetIp", L"Get Current IP");
    fresh.copyIpToClipboard = Wh_GetIntSetting(L"copyIpToClipboard");

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(fresh);
}

BOOL Wh_ModInit() {
    g_unloading.store(false);
    LoadSettings();

    DWORD build = GetWindowsBuild();
    if (build && build < 22000) {
        Wh_Log(L"Unsupported Windows build %lu", build);
        return TRUE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewModuleHooked.store(true);
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Failed to hook Taskbar.View symbols during init");
        }
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW =
        kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;
    if (loadLibraryExW) {
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewModuleHooked.load()) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            HandleLoadedModuleIfTaskbarView(taskbarViewModule, true);
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    g_unloading.store(true);

    // Wait for any active background worker threads (IP query or Temp cleanup) before allowing Windhawk to unmap the DLL from explorer.exe memory
    while (g_activeAsyncTasks.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    winrt::weak_ref<wux::FrameworkElement> oldTarget;
    winrt::weak_ref<wuc::CoreDispatcher> oldDispatcher;
    {
        std::lock_guard<std::mutex> lock(g_targetMutex);
        oldTarget = std::exchange(g_lastTargetWeak, nullptr);
        oldDispatcher = std::exchange(g_dispatcherWeak, nullptr);
    }
}