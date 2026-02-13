// ==WindhawkMod==
// @id              taskbar-auto-hide-custom-activation-area
// @name            Taskbar auto-hide custom activation area
// @description     Customize the taskbar auto-hide activation area, allowing to limit mouse unhiding to specific regions
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar auto-hide custom activation area

Customize the taskbar auto-hide activation area, allowing to limit mouse
unhiding to specific regions.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activationRegion: "40%-60%"
  $name: Activation region
  $description: >-
    A comma-separated list of regions along the taskbar where mouse hover will
    unhide it. Each region is a range like "100-200" (pixels) or "20%-50%"
    (percentage of taskbar length).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windowsx.h>

#include <atomic>
#include <optional>
#include <string_view>
#include <vector>

enum {
    kTrayUITimerUnhide = 3,
};

struct Region {
    bool isPercentage;
    int start;
    int end;
};

std::vector<Region> g_regions;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;

// https://stackoverflow.com/a/54364173
std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

bool SvToInt(std::wstring_view s, int* result) {
    if (s.empty()) {
        return false;
    }

    int value = 0;
    for (WCHAR c : s) {
        if (c < L'0' || c > L'9') {
            return false;
        }
        value = value * 10 + (c - L'0');
    }

    *result = value;
    return true;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

bool IsCursorInActivationRegion(HWND hWnd, POINT pt) {
    if (g_regions.empty()) {
        return false;
    }

    RECT rc;
    GetWindowRect(hWnd, &rc);

    bool isHorizontal = (rc.right - rc.left) >= (rc.bottom - rc.top);
    int taskbarLength;
    int cursorOffset;
    if (isHorizontal) {
        taskbarLength = rc.right - rc.left;
        cursorOffset = pt.x - rc.left;
    } else {
        taskbarLength = rc.bottom - rc.top;
        cursorOffset = pt.y - rc.top;
    }

    UINT dpi = GetDpiForWindow(hWnd);

    for (const auto& region : g_regions) {
        int start, end;
        if (region.isPercentage) {
            start = MulDiv(taskbarLength, region.start, 100);
            end = MulDiv(taskbarLength, region.end, 100);
        } else {
            start = MulDiv(region.start, dpi, 96);
            end = MulDiv(region.end, dpi, 96);
        }

        if (cursorOffset >= start && cursorOffset <= end) {
            return true;
        }
    }

    return false;
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    if (nIDEvent == kTrayUITimerUnhide && IsTaskbarWindow(hWnd)) {
        Wh_Log(L">");
        DWORD msgPos = GetMessagePos();
        POINT pt = {GET_X_LPARAM(msgPos), GET_Y_LPARAM(msgPos)};
        if (!IsCursorInActivationRegion(hWnd, pt)) {
            Wh_Log(L"Cursor not in activation region, skipping unhide");
            return 1;
        }
    }

    UINT_PTR ret = SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);

    return ret;
}

bool g_isPointerOverTaskbarFrame;

using ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t =
    void(WINAPI*)(void* pThis,
                  HWND hMMTaskbarWnd,
                  bool isPointerOver,
                  int inputDeviceKind);
ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t
    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original;
void WINAPI ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook(
    void* pThis,
    HWND hMMTaskbarWnd,
    bool isPointerOver,
    int inputDeviceKind) {
    Wh_Log(L"> isPointerOver=%d", isPointerOver);

    g_isPointerOverTaskbarFrame = isPointerOver;

    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
        pThis, hMMTaskbarWnd, isPointerOver, inputDeviceKind);

    g_isPointerOverTaskbarFrame = false;
}

// From ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged.
constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;
// From TaskbarFrame::OnScreenEdgeStrokePointerEntered
constexpr int kReasonOnScreenEdgeStrokePointerEntered = 8;

using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;
void WINAPI ViewCoordinator_UpdateIsExpanded_Hook(void* pThis,
                                                  HWND hMMTaskbarWnd,
                                                  int reason) {
    Wh_Log(L"> reason=%d", reason);

    if ((reason == kReasonIsPointerOverTaskbarFrameChanged &&
         g_isPointerOverTaskbarFrame) ||
        reason == kReasonOnScreenEdgeStrokePointerEntered) {
        DWORD msgPos = GetMessagePos();
        POINT pt = {GET_X_LPARAM(msgPos), GET_Y_LPARAM(msgPos)};
        if (!IsCursorInActivationRegion(hMMTaskbarWnd, pt)) {
            Wh_Log(L"Cursor not in activation region, skipping");
            return;
        }
    }

    ViewCoordinator_UpdateIsExpanded_Original(pThis, hMMTaskbarWnd, reason);
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original,
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &ViewCoordinator_UpdateIsExpanded_Original,
            ViewCoordinator_UpdateIsExpanded_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3)) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

std::optional<Region> ParseRegion(std::wstring_view regionStr) {
    auto parts = SplitStringView(regionStr, L"-");
    if (parts.size() != 2) {
        Wh_Log(L"Invalid region (expected start-end): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    auto startStr = TrimStringView(parts[0]);
    auto endStr = TrimStringView(parts[1]);

    bool startIsPercentage = !startStr.empty() && startStr.back() == L'%';
    bool endIsPercentage = !endStr.empty() && endStr.back() == L'%';
    if (startIsPercentage != endIsPercentage) {
        Wh_Log(L"Invalid region (mixed percent and pixel): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    bool isPercentage = startIsPercentage;
    if (isPercentage) {
        startStr.remove_suffix(1);
        endStr.remove_suffix(1);
    }

    int start;
    int end;
    if (!SvToInt(startStr, &start) || !SvToInt(endStr, &end)) {
        Wh_Log(L"Invalid region (non-numeric values): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    if (start >= end) {
        Wh_Log(L"Invalid region (start must be less than end): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    return Region{isPercentage, start, end};
}

void LoadSettings() {
    g_regions.clear();

    PCWSTR str = Wh_GetStringSetting(L"activationRegion");

    for (auto regionStr : SplitStringView(str, L",")) {
        regionStr = TrimStringView(regionStr);
        if (regionStr.empty()) {
            continue;
        }

        if (auto region = ParseRegion(regionStr)) {
            g_regions.push_back(*region);
        }
    }

    Wh_FreeStringSetting(str);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_winVersion >= WinVersion::Win11) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                Wh_Log(L"HookTaskbarViewDllSymbols failed");
                return FALSE;
            }
        } else {
            Wh_Log(L"Taskbar view module not loaded yet");

            HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
            auto pKernelBaseLoadLibraryExW =
                (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                          "LoadLibraryExW");
            WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        }
    }

    WindhawkUtils::SetFunctionHook(SetTimer, SetTimer_Hook, &SetTimer_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    return TRUE;
}
