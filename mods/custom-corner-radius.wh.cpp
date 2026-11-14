// ==WindhawkMod==
// @id              custom-corner-radius
// @name            Custom Window Corner Radius
// @description     Customizes window corner radius in Windows 11, making corners more or less rounded
// @version         1.4
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lwevtapi -ld2d1 -ld3d11
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
# Custom Window Corner Radius

Customizes Windows 11 app window corner radius. Make corners more rounded than
the default 8px, or reduce the radius for less rounded or completely sharp
corners.

The mod was [originally
submitted](https://github.com/ramensoftware/windhawk-mods/pull/3587) by
[Kanak415](https://github.com/kanak-buet19). The option for maximized and
snapped windows is based on a
[mod](https://github.com/ramensoftware/windhawk-mods/pull/5022) by [Alexey
Lavrinenko](https://github.com/leshaalexey).

![Screenshot](https://i.imgur.com/mMGkBwc.png)

## ⚠ Important usage note ⚠

This mod needs to hook into `dwm.exe` to work. Please navigate to Windhawk's
Settings > Advanced settings > More advanced settings > Process inclusion list,
and make sure that `dwm.exe` is in the list.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

## Additional notes

- Some elements, such as context menus, use a smaller radius (4px by default).
  This can be customized separately with the "Small corner radius" option.
- Standard tooltips can be customized separately with the "Tooltip corner
  radius" option.
- Each window corner can have its own radius with the "Per-corner radius"
  option, for example to round only the top corners.
- Windows 11 squares off the corners of maximized and snapped windows. The
  "Rounded corners for maximized and snapped windows" option enables rounding
  for them.
- Some elements, such as the taskbar, the Start menu, and the notification
  center, are unaffected by this mod. Some of them can be customized using other
  mods, such as Windows 11 Taskbar Styler.
- Disabling the mod instantly restores default behavior - no system files are
  modified.

## Compatibility

- When using this mod alongside Translucent Flyouts, set its `CornerType` option
  to `0` ("Don't Change") to prevent conflicts between the two.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- radius: 12
  $name: Corner radius
  $description: >-
    Corner radius in pixels. Default Win11 is 8. Use smaller values (e.g. 4 or
    0) for less rounded or sharp corners, or larger values (e.g. 10-20) for more
    rounded corners. Values above 20 may cause visual artifacts depending on
    your DPI scaling.

    Set to -1 to keep the original radius.
- perCornerRadius: ""
  $name: Per-corner radius
  $description: >-
    Overrides the radius of each corner, in the order top-left, top-right,
    bottom-right, bottom-left. For example, 8,8,0,0 rounds only the top corners.
- smallRadius: 6
  $name: Small corner radius
  $description: >-
    Corner radius for elements that use a smaller radius, such as context menus.
    Default Win11 is 4.

    Set to -1 to keep the original radius.
- tooltipRadius: -1
  $name: Tooltip corner radius
  $description: >-
    Corner radius for standard tooltips. Note that this doesn't affect modern
    (WinUI) tooltips. Values above 8 may cause visual artifacts depending on
    your DPI scaling.

    Set to -1 to leave tooltips unchanged.
- roundMaximizedAndSnapped: none
  $name: Rounded corners for maximized and snapped windows
  $description: >-
    Windows 11 squares off the corners of maximized and snapped windows. This
    option enables rounding for them using the "Corner radius" value above.
  $options:
  - none: Leave them square
  - snapped: Round snapped windows
  - snappedAndMaximized: Round snapped and maximized windows
- excludedPrograms: [""]
  $name: Excluded programs
  $description: >-
    Windows of these programs keep their original corner radius.

    Entries can be process names, paths or application IDs, for example:

    mspaint.exe

    C:\Windows\System32\notepad.exe

    Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>  // Must appear before propkey.h

#include <d2d1_1.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <propkey.h>
#include <propsys.h>
#include <winevt.h>
#include <winrt/base.h>
#include <winternl.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <mutex>
#include <regex>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

enum class RoundMaximizedAndSnapped {
    none,
    snapped,
    snappedAndMaximized,
};

// Corners are indexed in the order top-left, top-right, bottom-left,
// bottom-right throughout.
constexpr int kCornerCount = 4;

struct {
    float radius;
    // How far each corner's radius falls short of `radius`, in DIPs. All zero
    // unless per-corner radii are configured, in which case `radius` is the
    // largest of them.
    float cornerRadiusDelta[kCornerCount];
    bool perCornerRadius;
    float smallRadius;
    float tooltipRadius;
    RoundMaximizedAndSnapped roundMaximizedAndSnapped;
    std::unordered_set<std::wstring> excludedPrograms;
} g_settings;

// Guards `g_settings.excludedPrograms`, which DWM reads while the settings are
// being reloaded.
std::shared_mutex g_excludedProgramsMutex;

using GetWindowData_t = void*(WINAPI*)(void* pThis);
GetWindowData_t GetWindowData_Original;

using IsMaximizedOrSnapped_t = bool(WINAPI*)(void* pThis);
IsMaximizedOrSnapped_t IsMaximizedOrSnapped_Original;

// Captured (not hooked) address of CWindowData::IsGhostWindow. We disassemble
// its first few instructions at init time to recover the HWND member offset.
void* IsGhostWindow_Func;

// HWND offset within CWindowData. Recovered at init time from the first
// `mov rcx, qword ptr [rcx+disp]` instruction in CWindowData::IsGhostWindow,
// which loads its HWND member as the first argument to GetPropW. Stays at
// SIZE_MAX if the pattern can't be matched, in which case HWND lookup is
// disabled and tooltip-specific behavior gracefully degrades.
size_t g_windowDataHwndOffset = SIZE_MAX;

// Scans the first `limit` instructions of `func` for a match against `regex`
// and returns the value of the first capture group parsed as hex. Mirrors the
// helper used by taskbar-button-scroll / taskbar-icon-size.
size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Failed for %p", func);
    return defValue;
}

HWND HwndFromTopLevelWindow(void* pThis) {
    if (g_windowDataHwndOffset == SIZE_MAX || !GetWindowData_Original) {
        return nullptr;
    }
    void* pData = GetWindowData_Original(pThis);
    if (!pData) {
        return nullptr;
    }
    HWND hwnd = *(HWND*)((BYTE*)pData + g_windowDataHwndOffset);
    return IsWindow(hwnd) ? hwnd : nullptr;
}

bool HwndHasClass(HWND hwnd, PCWSTR className) {
    if (!hwnd) {
        return false;
    }
    WCHAR buf[32];
    return GetClassNameW(hwnd, buf, ARRAYSIZE(buf)) &&
           _wcsicmp(buf, className) == 0;
}

bool IsTopLevelWindowTooltip(void* pThis) {
    return HwndHasClass(HwndFromTopLevelWindow(pThis), L"tooltips_class32");
}

typedef struct _SYSTEM_PROCESS_ID_INFORMATION {
    HANDLE ProcessId;
    UNICODE_STRING ImageName;
} SYSTEM_PROCESS_ID_INFORMATION;

// The image path of a process in NT form, e.g.
// \Device\HarddiskVolume3\Windows\System32\notepad.exe. Unlike
// QueryFullProcessImageName, this needs no handle to the process, which dwm.exe
// can't get: it runs under a virtual account that isn't in the DACL of the
// processes owning the windows it composes.
std::wstring GetProcessImageNtPath(DWORD processId) {
    using NtQuerySystemInformation_t =
        LONG(NTAPI*)(ULONG systemInformationClass, PVOID systemInformation,
                     ULONG systemInformationLength, PULONG returnLength);
    static NtQuerySystemInformation_t pNtQuerySystemInformation = []() {
        HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
        return ntdll ? (NtQuerySystemInformation_t)GetProcAddress(
                           ntdll, "NtQuerySystemInformation")
                     : nullptr;
    }();
    if (!pNtQuerySystemInformation) {
        return std::wstring{};
    }

    constexpr ULONG kSystemProcessIdInformation = 88;
    constexpr LONG kStatusInfoLengthMismatch = 0xC0000004;

    SYSTEM_PROCESS_ID_INFORMATION info{
        .ProcessId = (HANDLE)(ULONG_PTR)processId,
    };

    // An empty ImageName makes the call report the size it needs.
    LONG status = pNtQuerySystemInformation(kSystemProcessIdInformation, &info,
                                            sizeof(info), nullptr);
    if (status != kStatusInfoLengthMismatch) {
        Wh_Log(L"Size query failed for pid=%u: %08X", processId, status);
        return std::wstring{};
    }

    std::wstring path(info.ImageName.MaximumLength / sizeof(WCHAR), L'\0');
    info.ImageName.Buffer = path.data();
    info.ImageName.Length = 0;

    status = pNtQuerySystemInformation(kSystemProcessIdInformation, &info,
                                       sizeof(info), nullptr);
    if (status < 0) {
        Wh_Log(L"Query failed for pid=%u: %08X", processId, status);
        return std::wstring{};
    }

    path.resize(info.ImageName.Length / sizeof(WCHAR));
    return path;
}

// Turns \Device\HarddiskVolume3\Windows\... into C:\Windows\..., or returns an
// empty string if no drive letter maps to the device.
std::wstring NtPathToDosPath(const std::wstring& ntPath) {
    WCHAR drives[512];
    DWORD len = GetLogicalDriveStrings(ARRAYSIZE(drives), drives);
    if (!len || len > ARRAYSIZE(drives)) {
        return std::wstring{};
    }

    for (PCWSTR drive = drives; *drive; drive += wcslen(drive) + 1) {
        WCHAR driveName[] = {drive[0], L':', L'\0'};
        WCHAR target[MAX_PATH];
        if (!QueryDosDevice(driveName, target, ARRAYSIZE(target))) {
            continue;
        }

        size_t targetLen = wcslen(target);
        if (ntPath.length() > targetLen && ntPath[targetLen] == L'\\' &&
            _wcsnicmp(ntPath.c_str(), target, targetLen) == 0) {
            return driveName + ntPath.substr(targetLen);
        }
    }

    return std::wstring{};
}

// The AppUserModelID explicitly set on a window. It's what identifies packaged
// apps, whose windows belong to a shared host process. shell32 is resolved on
// demand to keep it out of dwm.exe unless exclusions are actually used.
std::wstring GetWindowAppId(HWND hWnd) {
    using SHGetPropertyStoreForWindow_t =
        HRESULT(WINAPI*)(HWND hwnd, REFIID riid, void** ppv);
    static SHGetPropertyStoreForWindow_t pSHGetPropertyStoreForWindow = []() {
        HMODULE shell32 = LoadLibraryEx(L"shell32.dll", nullptr,
                                        LOAD_LIBRARY_SEARCH_SYSTEM32);
        return shell32 ? (SHGetPropertyStoreForWindow_t)GetProcAddress(
                             shell32, "SHGetPropertyStoreForWindow")
                       : nullptr;
    }();

    std::wstring result;

    if (!pSHGetPropertyStoreForWindow) {
        Wh_Log(L"SHGetPropertyStoreForWindow isn't available");
        return result;
    }

    IPropertyStore* propertyStore;
    HRESULT hr =
        pSHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&propertyStore));
    if (FAILED(hr)) {
        Wh_Log(L"SHGetPropertyStoreForWindow failed for hwnd=%p: %08X", hWnd,
               hr);
        return result;
    }

    PROPVARIANT pv;
    PropVariantInit(&pv);
    hr = propertyStore->GetValue(PKEY_AppUserModel_ID, &pv);
    if (SUCCEEDED(hr)) {
        if (pv.vt == VT_LPWSTR && pv.pwszVal) {
            result = pv.pwszVal;
        }
        PropVariantClear(&pv);
    } else {
        Wh_Log(L"GetValue failed for hwnd=%p: %08X", hWnd, hr);
    }

    propertyStore->Release();
    return result;
}

void MakeUpper(std::wstring* str) {
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, str->data(),
                  static_cast<int>(str->length()), str->data(),
                  static_cast<int>(str->length()), nullptr, nullptr, 0);
}

bool IsWindowExcluded(HWND hWnd) {
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);

    std::wstring processPathUpper;
    if (dwProcessId) {
        std::wstring ntPath = GetProcessImageNtPath(dwProcessId);
        processPathUpper = NtPathToDosPath(ntPath);
        if (processPathUpper.empty()) {
            // Without a drive letter no configured path can match, but the
            // last component is still the file name.
            processPathUpper = std::move(ntPath);
        }
        MakeUpper(&processPathUpper);
    }

    if (!processPathUpper.empty()) {
        if (g_settings.excludedPrograms.contains(processPathUpper)) {
            Wh_Log(L"hwnd=%p excluded by path: %s", hWnd,
                   processPathUpper.c_str());
            return true;
        }

        size_t fileNamePos = processPathUpper.rfind(L'\\');
        if (fileNamePos != std::wstring::npos) {
            std::wstring fileNameUpper =
                processPathUpper.substr(fileNamePos + 1);
            if (!fileNameUpper.empty() &&
                g_settings.excludedPrograms.contains(fileNameUpper)) {
                Wh_Log(L"hwnd=%p excluded by file name: %s", hWnd,
                       fileNameUpper.c_str());
                return true;
            }
        }
    }

    std::wstring appIdUpper = GetWindowAppId(hWnd);
    MakeUpper(&appIdUpper);
    if (!appIdUpper.empty() &&
        g_settings.excludedPrograms.contains(appIdUpper)) {
        Wh_Log(L"hwnd=%p excluded by app id: %s", hWnd, appIdUpper.c_str());
        return true;
    }

    Wh_Log(L"hwnd=%p not excluded, path=[%s], appId=[%s]", hWnd,
           processPathUpper.c_str(), appIdUpper.c_str());
    return false;
}

constexpr WCHAR kWindowExclusionProp[] = L"Windhawk_Excluded_" WH_MOD_ID;

const HANDLE kWindowNotExcluded = (HANDLE)1;
const HANDLE kWindowExcluded = (HANDLE)2;

// Resolving the program of a window is expensive, and the hooks run as part of
// composing every frame, so the verdict is cached in a window property. The
// properties are dropped when the settings change and when the mod is unloaded.
bool IsWindowExcludedCached(HWND hWnd) {
    HANDLE prop = GetProp(hWnd, kWindowExclusionProp);
    if (!prop) {
        prop = IsWindowExcluded(hWnd) ? kWindowExcluded : kWindowNotExcluded;
        if (!SetProp(hWnd, kWindowExclusionProp, prop)) {
            Wh_Log(L"SetProp failed for hwnd=%p: %u", hWnd, GetLastError());
        }
    }

    return prop == kWindowExcluded;
}

// Windows whose HWND can't be recovered are never excluded, same as the other
// HWND-based checks.
bool IsTopLevelWindowExcluded(void* pThis) {
    // Held until the verdict is cached, so that one based on the previous
    // exclusions can't be stored after they're replaced and the cache cleared.
    std::shared_lock lock(g_excludedProgramsMutex);

    if (g_settings.excludedPrograms.empty()) {
        return false;
    }

    HWND hwnd = HwndFromTopLevelWindow(pThis);
    if (!hwnd) {
        Wh_Log(L"No hwnd for %p, can't check exclusions", pThis);
        return false;
    }

    return IsWindowExcludedCached(hwnd);
}

void ClearWindowExclusionProps() {
    EnumWindows(
        [](HWND hWnd, LPARAM) -> BOOL {
            RemoveProp(hWnd, kWindowExclusionProp);
            return TRUE;
        },
        0);
}

float RadiusForOriginal(float orig, bool isTooltip) {
    // In new builds, multiple hooks fire in sequence (GetRadiusFromCornerStyle
    // -> GetFloatCornerRadiusForCurrentStyle -> SetBorderParameters), so a
    // downstream hook may see a value already replaced by an upstream hook.
    // Skip replacement if the value already matches a configured radius to keep
    // the function idempotent.
    if (orig == g_settings.radius || orig == g_settings.smallRadius ||
        orig == g_settings.tooltipRadius) {
        return orig;
    }

    if (isTooltip && g_settings.tooltipRadius >= 0.0f) {
        return g_settings.tooltipRadius;
    }

    // Win11 defaults: 4.0 for smaller radius, 8.0 for larger radius. Use middle
    // point as a threshold. Don't override if new value is negative.
    float newValue = orig < 6.0f ? g_settings.smallRadius : g_settings.radius;
    if (newValue < 0.0f) {
        return orig;
    }

    return newValue;
}

// Whether `radius`, as returned by RadiusForOriginal, is the window radius,
// the one per-corner radii apply to.
bool IsPerCornerRadius(float radius) {
    return g_settings.perCornerRadius && radius == g_settings.radius;
}

// DWM computes a single radius per window and hands it to the functions that
// build the clip geometry and draw the border. Per-corner radii are applied
// there: while a call whose output should get them is in progress, this holds
// how far each corner falls short of that single radius, in pixels, and the
// hooks on those functions adjust each corner accordingly.
struct CornerRadiusDeltas {
    float px[kCornerCount];
};

thread_local const CornerRadiusDeltas* g_cornerRadiusDeltas;

class CornerRadiusDeltasScope {
   public:
    explicit CornerRadiusDeltasScope(const CornerRadiusDeltas* deltas)
        : m_prev(g_cornerRadiusDeltas) {
        g_cornerRadiusDeltas = deltas;
    }
    ~CornerRadiusDeltasScope() { g_cornerRadiusDeltas = m_prev; }

    CornerRadiusDeltasScope(const CornerRadiusDeltasScope&) = delete;
    CornerRadiusDeltasScope& operator=(const CornerRadiusDeltasScope&) = delete;

   private:
    const CornerRadiusDeltas* m_prev;
};

float AdjustedCornerRadius(float radius, int corner) {
    return std::max(radius + g_cornerRadiusDeltas->px[corner], 0.0f);
}

CornerRadiusDeltas CornerRadiusDeltasForDpi(int dpi) {
    CornerRadiusDeltas deltas;
    for (int i = 0; i < kCornerCount; i++) {
        deltas.px[i] = g_settings.cornerRadiusDelta[i] * dpi / 96.0f;
    }
    return deltas;
}

// Deltas for the content clip of each CWindowBorder, keyed by the border. The
// clip is rebuilt on every resize from just the border's stored radius and
// DPI, so the deltas are kept from when its parameters were set.
std::mutex g_borderCornerRadiusDeltasMutex;
std::unordered_map<void*, CornerRadiusDeltas> g_borderCornerRadiusDeltas;

void SetBorderCornerRadiusDeltas(void* border,
                                 const CornerRadiusDeltas* deltas) {
    std::lock_guard lock(g_borderCornerRadiusDeltasMutex);
    if (deltas) {
        g_borderCornerRadiusDeltas[border] = *deltas;
    } else {
        g_borderCornerRadiusDeltas.erase(border);
    }
}

bool GetBorderCornerRadiusDeltas(void* border, CornerRadiusDeltas* deltas) {
    std::lock_guard lock(g_borderCornerRadiusDeltasMutex);
    auto it = g_borderCornerRadiusDeltas.find(border);
    if (it == g_borderCornerRadiusDeltas.end()) {
        return false;
    }
    *deltas = it->second;
    return true;
}

using CWindowBorder_Destructor_t = void(WINAPI*)(void* pThis);
CWindowBorder_Destructor_t CWindowBorder_Destructor_Original;
void WINAPI CWindowBorder_Destructor_Hook(void* pThis) {
    SetBorderCornerRadiusDeltas(pThis, nullptr);
    CWindowBorder_Destructor_Original(pThis);
}

// The geometry DWM clips window content with. It takes an X and Y radius per
// corner, but every caller passes the same radius in all eight.
using SetRectangle_t = long(WINAPI*)(void* pThis,
                                     float left,
                                     float top,
                                     float right,
                                     float bottom,
                                     float topLeftX,
                                     float topLeftY,
                                     float topRightX,
                                     float topRightY,
                                     float bottomLeftX,
                                     float bottomLeftY,
                                     float bottomRightX,
                                     float bottomRightY,
                                     bool flag);
SetRectangle_t SetRectangle_Original;
long WINAPI SetRectangle_Hook(void* pThis,
                              float left,
                              float top,
                              float right,
                              float bottom,
                              float topLeftX,
                              float topLeftY,
                              float topRightX,
                              float topRightY,
                              float bottomLeftX,
                              float bottomLeftY,
                              float bottomRightX,
                              float bottomRightY,
                              bool flag) {
    if (g_cornerRadiusDeltas) {
        Wh_Log(L"> %f", topLeftX);
        topLeftX = topLeftY = AdjustedCornerRadius(topLeftX, 0);
        topRightX = topRightY = AdjustedCornerRadius(topRightX, 1);
        bottomLeftX = bottomLeftY = AdjustedCornerRadius(bottomLeftX, 2);
        bottomRightX = bottomRightY = AdjustedCornerRadius(bottomRightX, 3);
    }

    return SetRectangle_Original(pThis, left, top, right, bottom, topLeftX,
                                 topLeftY, topRightX, topRightY, bottomLeftX,
                                 bottomLeftY, bottomRightX, bottomRightY, flag);
}

// Sets the content clip from the border rect, the stored radius and DPI, and
// the border thickness. Called on every resize.
using SetClipRectangle_t = void(WINAPI*)(void* pThis,
                                         void* geometry,
                                         const RECT& rect);
SetClipRectangle_t SetClipRectangle_Original;
void WINAPI SetClipRectangle_Hook(void* pThis,
                                  void* geometry,
                                  const RECT& rect) {
    CornerRadiusDeltas deltas;
    bool perCorner = GetBorderCornerRadiusDeltas(pThis, &deltas);
    CornerRadiusDeltasScope scope(perCorner ? &deltas : nullptr);
    SetClipRectangle_Original(pThis, geometry, rect);
}

// A rounded rectangle path with the active deltas applied to `radius` at each
// corner. Replaces the D2D rounded rectangles the border surface is drawn
// with, which have a single radius.
HRESULT CreateAdjustedRoundedRectangleGeometry(ID2D1Factory* factory,
                                               const D2D1_RECT_F& rect,
                                               float radius,
                                               ID2D1PathGeometry** geometry) {
    float maxRadius =
        std::min(rect.right - rect.left, rect.bottom - rect.top) / 2;
    float radii[kCornerCount];
    for (int i = 0; i < kCornerCount; i++) {
        radii[i] = std::min(AdjustedCornerRadius(radius, i), maxRadius);
    }

    winrt::com_ptr<ID2D1PathGeometry> path;
    HRESULT hr = factory->CreatePathGeometry(path.put());
    if (FAILED(hr)) {
        return hr;
    }

    winrt::com_ptr<ID2D1GeometrySink> sink;
    hr = path->Open(sink.put());
    if (FAILED(hr)) {
        return hr;
    }

    auto arcTo = [&sink](D2D1_POINT_2F point, float r) {
        if (r > 0) {
            sink->AddArc(D2D1::ArcSegment(point, D2D1::SizeF(r, r), 0.0f,
                                          D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                          D2D1_ARC_SIZE_SMALL));
        }
    };

    // Clockwise from the top edge.
    sink->BeginFigure(D2D1::Point2F(rect.left + radii[0], rect.top),
                      D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(rect.right - radii[1], rect.top));
    arcTo(D2D1::Point2F(rect.right, rect.top + radii[1]), radii[1]);
    sink->AddLine(D2D1::Point2F(rect.right, rect.bottom - radii[3]));
    arcTo(D2D1::Point2F(rect.right - radii[3], rect.bottom), radii[3]);
    sink->AddLine(D2D1::Point2F(rect.left + radii[2], rect.bottom));
    arcTo(D2D1::Point2F(rect.left, rect.bottom - radii[2]), radii[2]);
    sink->AddLine(D2D1::Point2F(rect.left, rect.top + radii[0]));
    arcTo(D2D1::Point2F(rect.left + radii[0], rect.top), radii[0]);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);

    hr = sink->Close();
    if (FAILED(hr)) {
        return hr;
    }

    *geometry = path.detach();
    return S_OK;
}

using FillRoundedRectangle_t = void(WINAPI*)(ID2D1RenderTarget* pThis,
                                             const D2D1_ROUNDED_RECT* rect,
                                             ID2D1Brush* brush);

void FillRoundedRectangleWithDeltas(FillRoundedRectangle_t original,
                                    ID2D1RenderTarget* pThis,
                                    const D2D1_ROUNDED_RECT* rect,
                                    ID2D1Brush* brush) {
    if (g_cornerRadiusDeltas) {
        Wh_Log(L"> %f", rect->radiusX);
        winrt::com_ptr<ID2D1Factory> factory;
        pThis->GetFactory(factory.put());
        winrt::com_ptr<ID2D1PathGeometry> geometry;
        HRESULT hr = CreateAdjustedRoundedRectangleGeometry(
            factory.get(), rect->rect, rect->radiusX, geometry.put());
        if (SUCCEEDED(hr)) {
            pThis->FillGeometry(geometry.get(), brush);
            return;
        }
        Wh_Log(L"Failed: %08X", hr);
    }

    original(pThis, rect, brush);
}

// The border ring is filled on the device context DWM gets from the
// composition surface, and the shadow shape on a bitmap render target made
// from it. Those are separate classes in d2d1.dll, whose implementations of
// this method may or may not be folded into one function.
FillRoundedRectangle_t FillRoundedRectangle_Original;
void WINAPI FillRoundedRectangle_Hook(ID2D1RenderTarget* pThis,
                                      const D2D1_ROUNDED_RECT* rect,
                                      ID2D1Brush* brush) {
    FillRoundedRectangleWithDeltas(FillRoundedRectangle_Original, pThis, rect,
                                   brush);
}

FillRoundedRectangle_t BitmapTargetFillRoundedRectangle_Original;
void WINAPI BitmapTargetFillRoundedRectangle_Hook(ID2D1RenderTarget* pThis,
                                                  const D2D1_ROUNDED_RECT* rect,
                                                  ID2D1Brush* brush) {
    FillRoundedRectangleWithDeltas(BitmapTargetFillRoundedRectangle_Original,
                                   pThis, rect, brush);
}

using CreateRoundedRectangleGeometry_t =
    HRESULT(WINAPI*)(ID2D1Factory* pThis,
                     const D2D1_ROUNDED_RECT* rect,
                     ID2D1RoundedRectangleGeometry** geometry);
CreateRoundedRectangleGeometry_t CreateRoundedRectangleGeometry_Original;
HRESULT WINAPI
CreateRoundedRectangleGeometry_Hook(ID2D1Factory* pThis,
                                    const D2D1_ROUNDED_RECT* rect,
                                    ID2D1RoundedRectangleGeometry** geometry) {
    if (g_cornerRadiusDeltas) {
        Wh_Log(L"> %f", rect->radiusX);
        winrt::com_ptr<ID2D1PathGeometry> path;
        HRESULT hr = CreateAdjustedRoundedRectangleGeometry(
            pThis, rect->rect, rect->radiusX, path.put());
        if (SUCCEEDED(hr)) {
            // The border code only combines the result with another geometry
            // and releases it, which any ID2D1Geometry supports.
            *geometry =
                reinterpret_cast<ID2D1RoundedRectangleGeometry*>(path.detach());
            return S_OK;
        }
        Wh_Log(L"Failed: %08X", hr);
    }

    return CreateRoundedRectangleGeometry_Original(pThis, rect, geometry);
}

// Draws the border and its shadow into the surface a window's border brush
// stretches as a nine-grid. DWM caches the result by these parameters.
using CreateBorderSurface_t = long(WINAPI*)(float radius,
                                            int dpi,
                                            const void* color,
                                            int borderStyle,
                                            int shadowStyle,
                                            void** surface);
CreateBorderSurface_t CreateBorderSurface_Original;
long WINAPI CreateBorderSurface_Hook(float radius,
                                     int dpi,
                                     const void* color,
                                     int borderStyle,
                                     int shadowStyle,
                                     void** surface) {
    Wh_Log(L"> %f dpi=%d", radius, dpi);

    CornerRadiusDeltas deltas;
    bool perCorner = IsPerCornerRadius(radius);
    if (perCorner) {
        deltas = CornerRadiusDeltasForDpi(dpi);
    }

    CornerRadiusDeltasScope scope(perCorner ? &deltas : nullptr);
    return CreateBorderSurface_Original(radius, dpi, color, borderStyle,
                                        shadowStyle, surface);
}

struct D2DFunctions {
    void* createRoundedRectangleGeometry;
    void* fillRoundedRectangle;
    void* bitmapTargetFillRoundedRectangle;
};

// d2d1.dll's implementations of ID2D1Factory::CreateRoundedRectangleGeometry
// and ID2D1RenderTarget::FillRoundedRectangle. d2d1.dll doesn't export them,
// so they're read off the vtables of throwaway objects. A D3D device with the
// null driver is enough to get a device context of the same class as the one
// DWM draws the border surface with.
bool ResolveD2DFunctions(D2DFunctions* functions) {
    winrt::com_ptr<ID2D1Factory1> factory;
    HRESULT hr =
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                          __uuidof(ID2D1Factory1), nullptr, factory.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"D2D1CreateFactory failed: %08X", hr);
        return false;
    }

    winrt::com_ptr<ID3D11Device> d3dDevice;
    hr =
        D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_NULL, nullptr,
                          D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                          D3D11_SDK_VERSION, d3dDevice.put(), nullptr, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: %08X", hr);
        return false;
    }

    winrt::com_ptr<ID2D1Device> d2dDevice;
    hr = factory->CreateDevice(d3dDevice.try_as<IDXGIDevice>().get(),
                               d2dDevice.put());
    if (FAILED(hr)) {
        Wh_Log(L"CreateDevice failed: %08X", hr);
        return false;
    }

    winrt::com_ptr<ID2D1DeviceContext> deviceContext;
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                        deviceContext.put());
    if (FAILED(hr)) {
        Wh_Log(L"CreateDeviceContext failed: %08X", hr);
        return false;
    }

    // A compatible render target needs a target to be compatible with.
    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    winrt::com_ptr<ID2D1Bitmap1> bitmap;
    winrt::com_ptr<ID2D1BitmapRenderTarget> bitmapTarget;
    hr = deviceContext->CreateBitmap(D2D1::SizeU(1, 1), nullptr, 0,
                                     &bitmapProperties, bitmap.put());
    if (SUCCEEDED(hr)) {
        deviceContext->SetTarget(bitmap.get());
        hr = deviceContext->CreateCompatibleRenderTarget(bitmapTarget.put());
    }
    if (FAILED(hr)) {
        Wh_Log(L"Bitmap render target creation failed: %08X", hr);
    }

    // Vtable slots, counted from IUnknown.
    constexpr int kCreateRoundedRectangleGeometrySlot = 6;
    constexpr int kFillRoundedRectangleSlot = 19;
    auto vtableEntry = [](IUnknown* object, int slot) {
        return (*reinterpret_cast<void***>(object))[slot];
    };

    functions->createRoundedRectangleGeometry =
        vtableEntry(factory.get(), kCreateRoundedRectangleGeometrySlot);
    functions->fillRoundedRectangle =
        vtableEntry(deviceContext.get(), kFillRoundedRectangleSlot);
    functions->bitmapTargetFillRoundedRectangle =
        bitmapTarget
            ? vtableEntry(bitmapTarget.get(), kFillRoundedRectangleSlot)
            : nullptr;
    return true;
}

bool HookD2DFunctions() {
    D2DFunctions functions;
    if (!ResolveD2DFunctions(&functions)) {
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(
            (CreateRoundedRectangleGeometry_t)
                functions.createRoundedRectangleGeometry,
            CreateRoundedRectangleGeometry_Hook,
            &CreateRoundedRectangleGeometry_Original) ||
        !WindhawkUtils::SetFunctionHook(
            (FillRoundedRectangle_t)functions.fillRoundedRectangle,
            FillRoundedRectangle_Hook, &FillRoundedRectangle_Original)) {
        Wh_Log(L"Hooking D2D functions failed");
        return false;
    }

    if (functions.bitmapTargetFillRoundedRectangle &&
        functions.bitmapTargetFillRoundedRectangle !=
            functions.fillRoundedRectangle &&
        !WindhawkUtils::SetFunctionHook(
            (FillRoundedRectangle_t)functions.bitmapTargetFillRoundedRectangle,
            BitmapTargetFillRoundedRectangle_Hook,
            &BitmapTargetFillRoundedRectangle_Original)) {
        Wh_Log(L"Hooking the bitmap render target failed");
        return false;
    }

    return true;
}

using CVisual_Hide_t = void(WINAPI*)(void* pThis);
CVisual_Hide_t CVisual_Hide_Original;

constexpr WCHAR kSysShadowHiddenProp[] = L"Windhawk_SysShadowHidden_" WH_MOD_ID;

// Hides the visual of a SysShadow companion window so the legacy rectangular
// drop shadow stops being composited. CVisual::Hide counts requests, so DWM
// showing the window doesn't undo this one. SysShadow windows are short-lived
// and the request is never withdrawn, so the property recording which visual
// holds it is never removed either.
void HideSysShadowWindow(void* pThis, HWND hwnd) {
    if (GetProp(hwnd, kSysShadowHiddenProp) == pThis) {
        return;
    }

    Wh_Log(L"> hiding SysShadow hwnd=%p", hwnd);
    CVisual_Hide_Original(pThis);
    if (!SetProp(hwnd, kSysShadowHiddenProp, pThis)) {
        Wh_Log(L"SetProp failed for hwnd=%p: %u", hwnd, GetLastError());
    }
}

// The CTopLevelWindow whose UpdateWindowVisuals call is currently running, or
// null outside of one. SetBorderParameters gets a CWindowBorder, which offers
// no way back to the window it belongs to.
thread_local void* g_updateWindowVisualsTarget;

using UpdateWindowVisuals_t = long(WINAPI*)(void* pThis);
UpdateWindowVisuals_t UpdateWindowVisuals_Original;
long WINAPI UpdateWindowVisuals_Hook(void* pThis) {
    if (CVisual_Hide_Original && g_settings.tooltipRadius >= 0.0f &&
        !IsTopLevelWindowExcluded(pThis)) {
        HWND hwnd = HwndFromTopLevelWindow(pThis);
        if (HwndHasClass(hwnd, L"SysShadow")) {
            HideSysShadowWindow(pThis, hwnd);
            return 0;
        }
    }

    void* prevTarget = g_updateWindowVisualsTarget;
    g_updateWindowVisualsTarget = pThis;
    long ret = UpdateWindowVisuals_Original(pThis);
    g_updateWindowVisualsTarget = prevTarget;
    return ret;
}

using GetEffectiveCornerStyle_t = int(WINAPI*)(void* pThis);
GetEffectiveCornerStyle_t GetEffectiveCornerStyle_Original;
int WINAPI GetEffectiveCornerStyle_Hook(void* pThis) {
    int orig = GetEffectiveCornerStyle_Original(pThis);
    // Tooltips report DWMWCP_DONOTROUND, meaning DWM won't round them.
    // Promote them to DWMWCP_ROUNDSMALL so the rounding pipeline kicks in:
    // GetShadowStyle returns a rounded-shadow style, and
    // GetRadiusFromCornerStyle returns a non-zero radius that our hooks
    // override to the configured tooltipRadius via RadiusForOriginal.
    if (orig == DWMWCP_DONOTROUND && g_settings.tooltipRadius >= 0.0f &&
        !IsTopLevelWindowExcluded(pThis) && IsTopLevelWindowTooltip(pThis)) {
        Wh_Log(L"> cornerStyle DONOTROUND -> ROUNDSMALL (tooltip)");
        return DWMWCP_ROUNDSMALL;
    }
    return orig;
}

bool ShouldRoundMaximizedOrSnapped(void* pThis) {
    if (g_settings.roundMaximizedAndSnapped == RoundMaximizedAndSnapped::none ||
        g_settings.radius < 0.0f || !IsMaximizedOrSnapped_Original ||
        !IsMaximizedOrSnapped_Original(pThis)) {
        return false;
    }

    // A zero radius can also come from an app asking for DWMWCP_DONOTROUND.
    // Only the squaring should be undone, so consult the corner style, which
    // the squaring doesn't touch.
    if (GetEffectiveCornerStyle_Original &&
        GetEffectiveCornerStyle_Original(pThis) == DWMWCP_DONOTROUND) {
        return false;
    }

    if (g_settings.roundMaximizedAndSnapped ==
        RoundMaximizedAndSnapped::snappedAndMaximized) {
        return true;
    }

    // Snapped windows only: a maximized window is zoomed, a snapped one isn't.
    // Without an HWND there's no way to tell the two apart, so leave the window
    // alone.
    HWND hwnd = HwndFromTopLevelWindow(pThis);
    return hwnd && !IsZoomed(hwnd);
}

// Only GetFloatCornerRadiusForCurrentStyle squares maximized and snapped
// windows, so only it passes canRoundMaximizedOrSnapped. Elsewhere a zero
// radius comes from the corner style and is left alone.
float AdjustCornerRadius(void* pThis,
                         float orig,
                         bool canRoundMaximizedOrSnapped) {
    if (IsTopLevelWindowExcluded(pThis)) {
        return orig;
    }

    if (orig > 0) {
        bool isTooltip =
            g_settings.tooltipRadius >= 0.0f && IsTopLevelWindowTooltip(pThis);
        Wh_Log(L"> %f isTooltip=%d", orig, isTooltip);
        return RadiusForOriginal(orig, isTooltip);
    }

    if (canRoundMaximizedOrSnapped && ShouldRoundMaximizedOrSnapped(pThis)) {
        Wh_Log(L"> %f -> %f (maximized or snapped)", orig, g_settings.radius);
        return g_settings.radius;
    }

    return orig;
}

// While CTopLevelWindow3D::UpdateAnimatedResources runs, whether the radius
// last returned by GetRadiusFromCornerStyle_Hook is the window radius, and null
// outside of it. UpdateAnimatedResources scales that radius for DPI and
// animation progress and hands it straight to CreateRectangleGeometry, which
// is where the per-corner radii for the animated clip are applied.
thread_local bool* g_animatedRadiusPerCorner;

using UpdateAnimatedResources_t = long(WINAPI*)(void* pThis);
UpdateAnimatedResources_t UpdateAnimatedResources_Original;
long WINAPI UpdateAnimatedResources_Hook(void* pThis) {
    bool perCorner = false;
    bool* prev = g_animatedRadiusPerCorner;
    g_animatedRadiusPerCorner = &perCorner;
    long ret = UpdateAnimatedResources_Original(pThis);
    g_animatedRadiusPerCorner = prev;
    return ret;
}

using GetRadiusFromCornerStyle_t = float(WINAPI*)(void* pThis);
GetRadiusFromCornerStyle_t GetRadiusFromCornerStyle_Original;
float WINAPI GetRadiusFromCornerStyle_Hook(void* pThis) {
    float radius =
        AdjustCornerRadius(pThis, GetRadiusFromCornerStyle_Original(pThis),
                           /*canRoundMaximizedOrSnapped=*/false);
    if (g_animatedRadiusPerCorner) {
        *g_animatedRadiusPerCorner = IsPerCornerRadius(radius);
    }
    return radius;
}

// The clip of a window in a minimize or restore animation. The radius shrinks
// along with the window, so the deltas are scaled with it rather than by DPI.
using CreateRectangleGeometry_t = long(WINAPI*)(const void* rect,
                                                float radius,
                                                void** geometry);
CreateRectangleGeometry_t CreateRectangleGeometry_Original;
long WINAPI CreateRectangleGeometry_Hook(const void* rect,
                                         float radius,
                                         void** geometry) {
    CornerRadiusDeltas deltas;
    bool perCorner =
        radius > 0 && g_animatedRadiusPerCorner && *g_animatedRadiusPerCorner;
    if (perCorner) {
        Wh_Log(L"> %f", radius);
        for (int i = 0; i < kCornerCount; i++) {
            deltas.px[i] =
                radius * g_settings.cornerRadiusDelta[i] / g_settings.radius;
        }
    }

    CornerRadiusDeltasScope scope(perCorner ? &deltas : nullptr);
    return CreateRectangleGeometry_Original(rect, radius, geometry);
}

using GetFloatCornerRadiusForCurrentStyle_t = float(WINAPI*)(void* pThis);
GetFloatCornerRadiusForCurrentStyle_t
    GetFloatCornerRadiusForCurrentStyle_Original;
float WINAPI GetFloatCornerRadiusForCurrentStyle_Hook(void* pThis) {
    return AdjustCornerRadius(
        pThis, GetFloatCornerRadiusForCurrentStyle_Original(pThis),
        /*canRoundMaximizedOrSnapped=*/true);
}

using SetBorderParameters_t = long(WINAPI*)(void* pThis,
                                            const RECT& borderRect,
                                            float cornerRadius,
                                            int dpi,
                                            const void* color,
                                            int borderStyle,
                                            int shadowStyle);
SetBorderParameters_t SetBorderParameters_Original;
long WINAPI SetBorderParameters_Hook(void* pThis,
                                     const RECT& borderRect,
                                     float cornerRadius,
                                     int dpi,
                                     const void* color,
                                     int borderStyle,
                                     int shadowStyle) {
    bool perCorner = false;
    if (cornerRadius > 0) {
        // pThis here is a CWindowBorder, not a CTopLevelWindow, so there's no
        // straightforward way to recover the HWND for tooltip detection. This
        // path is only used by old builds where the radius is computed inline.
        // The enclosing UpdateWindowVisuals call is what identifies the window
        // for the exclusion check.
        bool excluded = g_updateWindowVisualsTarget &&
                        IsTopLevelWindowExcluded(g_updateWindowVisualsTarget);
        if (!excluded) {
            Wh_Log(L"> %f", cornerRadius);
            cornerRadius = RadiusForOriginal(cornerRadius, false);
            perCorner = IsPerCornerRadius(cornerRadius);
        }
    }

    CornerRadiusDeltas deltas;
    if (perCorner) {
        deltas = CornerRadiusDeltasForDpi(dpi);
    }
    SetBorderCornerRadiusDeltas(pThis, perCorner ? &deltas : nullptr);

    return SetBorderParameters_Original(pThis, borderRect, cornerRadius, dpi,
                                        color, borderStyle, shadowStyle);
}

// The corner radius is only recomputed when DWM refreshes a window's visuals.
// dwm.exe's notification window turns WM_SYSCOLORCHANGE into an internal
// settings-change message that marks every window dirty, so the next frame
// re-runs UpdateWindowVisuals for all of them.
void RequestDwmRefresh() {
    HWND hDwm = FindWindow(L"Dwm", nullptr);
    if (!hDwm) {
        Wh_Log(L"DWM notification window wasn't found");
        return;
    }

    PostMessage(hDwm, WM_SYSCOLORCHANGE, 0, 0);
}

// The hooks per-corner radii depend on are only set at init, and only when
// per-corner radii are configured, which is the uncommon case. Configuring them
// later reloads the mod.
enum class PerCornerRadiusHooks {
    notSet,
    set,
    unsupported,
};

PerCornerRadiusHooks g_perCornerRadiusHooks = PerCornerRadiusHooks::notSet;

// `value` moved `ulps` floats up.
float FloatAbove(int value, int ulps) {
    float result = static_cast<float>(value);
    for (int i = 0; i < ulps; i++) {
        result = std::nextafter(result, std::numeric_limits<float>::max());
    }
    return result;
}

// Settings loads so far in this dwm.exe process, which is how long DWM's cache
// of border surfaces lives. It's passed from one load of the mod to the next on
// DWM's notification window.
int g_loadCount;

constexpr WCHAR kLoadCountProp[] = L"Windhawk_LoadCount_" WH_MOD_ID;

// Parses four comma-separated integers, given clockwise from the top-left
// corner, into `radii`. Leaves `radii` unchanged on failure.
bool ParseCornerRadii(PCWSTR text, int radii[kCornerCount]) {
    constexpr int kCornerIndex[kCornerCount] = {0, 1, 3, 2};

    int values[kCornerCount];
    PCWSTR p = text;
    for (int i = 0; i < kCornerCount; i++) {
        PWSTR end;
        values[kCornerIndex[i]] = wcstol(p, &end, 10);
        if (end == p) {
            return false;
        }

        while (iswspace(*end)) {
            end++;
        }

        if (*end != (i < kCornerCount - 1 ? L',' : L'\0')) {
            return false;
        }

        p = end + 1;
    }

    std::copy(values, values + kCornerCount, radii);
    return true;
}

void LoadSettings() {
    int radius = Wh_GetIntSetting(L"radius");

    int cornerRadius[kCornerCount] = {-1, -1, -1, -1};
    PCWSTR perCornerRadius = Wh_GetStringSetting(L"perCornerRadius");
    if (*perCornerRadius && !ParseCornerRadii(perCornerRadius, cornerRadius)) {
        Wh_Log(L"Invalid per-corner radius: %s", perCornerRadius);
    }
    Wh_FreeStringSetting(perCornerRadius);

    // Per-corner radii are applied as a reduction from the radius DWM works
    // with, so that radius is the largest of them. Negative corners follow
    // "Corner radius", and if that's -1 (keep the original radius), the
    // original is taken to be the Win11 default.
    constexpr int kDefaultRadius = 8;
    bool hasCornerRadius =
        std::any_of(cornerRadius, cornerRadius + kCornerCount,
                    [](int r) { return r >= 0; });
    if (radius < 0 && hasCornerRadius) {
        radius = kDefaultRadius;
    }

    for (int i = 0; i < kCornerCount; i++) {
        if (cornerRadius[i] < 0) {
            cornerRadius[i] = radius;
        }
    }
    int maxRadius =
        *std::max_element(cornerRadius, cornerRadius + kCornerCount);

    g_settings.perCornerRadius = false;
    for (int i = 0; i < kCornerCount; i++) {
        g_settings.cornerRadiusDelta[i] =
            static_cast<float>(cornerRadius[i] - maxRadius);
        if (g_settings.cornerRadiusDelta[i] != 0) {
            g_settings.perCornerRadius = true;
        }
    }

    if (g_settings.perCornerRadius &&
        g_perCornerRadiusHooks == PerCornerRadiusHooks::unsupported) {
        Wh_Log(L"Per-corner radius isn't supported, ignoring");
        g_settings.perCornerRadius = false;
    }

    // Use `std::nextafter` to get a value that's just slightly above the
    // integer, for two reasons:
    // 1. The original radius values are integer-based, so if the new value is
    //    exactly the same as the original value, it's impossible to determine
    //    whether the mod should override it or not if the custom value is
    //    identical to one of the original values (see RadiusForOriginal).
    // 2. If the zero value is used, some functions may treat it as a special
    //    case, for example dark mode menus will have a white border.
    //
    // The window radius gets an extra ulp per settings load on top of that.
    // Per-corner radii don't show in the radius DWM sees, so this is what
    // keeps the window radius apart from a small or tooltip radius with the
    // same integer, and what makes it a new key for DWM's cache of border
    // surfaces when only the per-corner radii change.
    g_loadCount++;
    g_settings.radius = FloatAbove(maxRadius, 1 + g_loadCount);
    g_settings.smallRadius = FloatAbove(Wh_GetIntSetting(L"smallRadius"), 1);
    g_settings.tooltipRadius =
        FloatAbove(Wh_GetIntSetting(L"tooltipRadius"), 1);

    PCWSTR roundMaximizedAndSnapped =
        Wh_GetStringSetting(L"roundMaximizedAndSnapped");
    g_settings.roundMaximizedAndSnapped = RoundMaximizedAndSnapped::none;
    if (wcscmp(roundMaximizedAndSnapped, L"snapped") == 0) {
        g_settings.roundMaximizedAndSnapped = RoundMaximizedAndSnapped::snapped;
    } else if (wcscmp(roundMaximizedAndSnapped, L"snappedAndMaximized") == 0) {
        g_settings.roundMaximizedAndSnapped =
            RoundMaximizedAndSnapped::snappedAndMaximized;
    }
    Wh_FreeStringSetting(roundMaximizedAndSnapped);

    std::unordered_set<std::wstring> excludedPrograms;

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            MakeUpper(&programUpper);

            Wh_Log(L"Excluded program: [%s]", programUpper.c_str());

            excludedPrograms.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    std::unique_lock lock(g_excludedProgramsMutex);
    g_settings.excludedPrograms = std::move(excludedPrograms);
}

// Returns true if at least two Dwminit warnings (Level=3) were logged in the
// Application event log within the last 60 seconds. DWM logs warnings here when
// it crashes and is restarted by the session manager, so repeated warnings are
// a strong signal that something in the desktop pipeline is unstable.
bool HasMultipleDwminitWarningsInLastMinute() {
    const WCHAR* queryPath = L"Application";
    const WCHAR* query =
        L"*[System[Provider[@Name='Dwminit'] and (Level=3) and "
        L"TimeCreated[timediff(@SystemTime) <= 60000]]]";

    EVT_HANDLE queryHandle = EvtQuery(nullptr,    // Local machine
                                      queryPath,  // Application log
                                      query, EvtQueryChannelPath);
    if (!queryHandle) {
        Wh_Log(L"EvtQuery failed with error: %u", GetLastError());
        return false;
    }

    EVT_HANDLE events[2] = {};
    DWORD returned = 0;
    constexpr DWORD kTimeout = 1000;
    BOOL ok =
        EvtNext(queryHandle, ARRAYSIZE(events), events, kTimeout, 0, &returned);
    if (!ok && GetLastError() != ERROR_NO_MORE_ITEMS) {
        Wh_Log(L"EvtNext failed with error: %u", GetLastError());
    }
    for (DWORD i = 0; i < returned; i++) {
        EvtClose(events[i]);
    }

    EvtClose(queryHandle);
    return ok && returned >= ARRAYSIZE(events);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // Skip the event log query unless the previous init is recent.
    FILETIME nowFt;
    GetSystemTimeAsFileTime(&nowFt);
    ULONGLONG now =
        ((ULONGLONG)nowFt.dwHighDateTime << 32) | nowFt.dwLowDateTime;
    ULONGLONG lastInitTime = 0;
    Wh_GetBinaryValue(L"lastInitTime", &lastInitTime, sizeof(lastInitTime));
    Wh_SetBinaryValue(L"lastInitTime", &now, sizeof(now));

    constexpr ULONGLONG kOneMinute = 60 * 10000000ULL;
    if (now - lastInitTime <= kOneMinute &&
        HasMultipleDwminitWarningsInLastMinute()) {
        Wh_Log(L"Refusing to load: multiple recent Dwminit warnings");
        return FALSE;
    }

    // The window is missing only in a process that's just starting, which has
    // nothing cached yet.
    if (HWND hDwm = FindWindow(L"Dwm", nullptr)) {
        g_loadCount = PtrToInt(GetProp(hDwm, kLoadCountProp));
    }

    LoadSettings();

    HMODULE udwm = GetModuleHandle(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"udwm.dll isn't loaded");
        return FALSE;
    }

    // Call tree for corner radius in each version:
    //
    // Old builds (e.g. 10.0.22621.6199):
    //   UpdateWindowVisuals
    //     -> GetEffectiveCornerStyle (inlined radius mapping, 8.0/4.0)
    //     -> SetBorderParameters (receives radius as param)
    //   CTopLevelWindow3D::UpdateAnimatedResources
    //     -> GetRadiusFromCornerStyle (DPI scaling inlined)
    //     -> ResourceHelper::CreateRectangleGeometry
    //
    // New builds (e.g. 10.0.26100.7920):
    //   UpdateWindowVisuals
    //     -> GetFloatCornerRadiusForCurrentStyle
    //       -> GetRadiusFromCornerStyle
    //     -> SetBorderParameters (receives radius as param)
    //   CTopLevelWindow3D::UpdateAnimatedResources
    //     -> GetDpiAdjustedFloatCornerRadius
    //       -> GetRadiusFromCornerStyle
    //     -> ResourceHelper::CreateRectangleGeometry
    //
    // In new builds, the squaring of maximized and snapped windows sits in
    // GetFloatCornerRadiusForCurrentStyle, which returns zero under the same
    // condition IsMaximizedOrSnapped tests, without consulting the corner style
    // at all. The animation path isn't squared, so restoring the rounding only
    // takes replacing that zero.
    //
    // Per-corner radii are applied one level down, where the single radius is
    // turned into geometry:
    //   SetBorderParameters
    //     -> CreateAndAttachBorderBrush
    //       -> CCachedBorderBrush::CreateBorderSurface (D2D drawing, cached)
    //     -> SetBorderRect (also called on resize)
    //       -> SetClipRectangle
    //         -> CRectangleGeometryProxy::SetRectangle (content clip)
    //   CTopLevelWindow3D::UpdateAnimatedResources
    //     -> ResourceHelper::CreateRectangleGeometry
    //       -> CRectangleGeometryProxy::SetRectangle (animated clip)

    bool perCorner = g_settings.perCornerRadius;

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        // Used to recover the HWND for tooltip detection. Returns the
        // CWindowData* stored on the CTopLevelWindow. Capture only, no hook.
        {
            {LR"(public: class CWindowData * __cdecl CTopLevelWindow::GetWindowData(void)const )"},
            &GetWindowData_Original,
            nullptr,
            true,  // Optional - tooltip detection is skipped if missing.
        },
        // Used to derive the HWND member offset on CWindowData at runtime by
        // disassembling the function's first `mov rcx, [rcx+disp]`. Capture
        // only, no hook.
        {
            {LR"(public: bool __cdecl CWindowData::IsGhostWindow(struct HWND__ * *)const )"},
            &IsGhostWindow_Func,
            nullptr,
            true,  // Optional - tooltip detection is skipped if missing.
        },
        // DWM's own answer about whether the surface being composed is a
        // maximized or snapped window. Every replacement made for such windows
        // is gated on it. Capture only, no hook.
        {
            {LR"(public: bool __cdecl CTopLevelWindow::IsMaximizedOrSnapped(void)const )"},
            &IsMaximizedOrSnapped_Original,
            nullptr,
            true,  // Optional - maximized/snapped rounding is skipped.
        },
        // Hides SysShadow companion windows and skips their visual updates so
        // the legacy rectangular drop shadow doesn't poke out beside the
        // rounded tooltip.
        {
            {LR"(private: long __cdecl CTopLevelWindow::UpdateWindowVisuals(void))"},
            &UpdateWindowVisuals_Original,
            UpdateWindowVisuals_Hook,
            true,  // Optional - SysShadow remains visible if missing.
        },
        // Used by the hook above to hide SysShadow windows. Capture only, no
        // hook.
        {
            {LR"(public: void __cdecl CVisual::Hide(void))"},
            &CVisual_Hide_Original,
            nullptr,
            true,  // Optional - SysShadow remains visible if missing.
        },
        // Used to promote tooltips from "no rounding" to "round small" so the
        // full DWM rounding pipeline kicks in (border + shadow + clip), instead
        // of us trying to force a radius onto a window DWM thinks is square.
        // Also consulted by ShouldRoundMaximizedOrSnapped.
        {
            {LR"(private: enum CORNER_STYLE __cdecl CTopLevelWindow::GetEffectiveCornerStyle(void))"},
            &GetEffectiveCornerStyle_Original,
            GetEffectiveCornerStyle_Hook,
            true,  // Optional - tooltip rounding skipped if missing.
        },
        // Covers the 3D animation path in both old and new builds.
        {
            {LR"(private: float __cdecl CTopLevelWindow::GetRadiusFromCornerStyle(void))"},
            &GetRadiusFromCornerStyle_Original,
            GetRadiusFromCornerStyle_Hook,
        },
        // Covers UpdateWindowVisuals in new builds, and with it the squaring of
        // maximized and snapped windows. Calls GetRadiusFromCornerStyle, but is
        // hooked separately since it zeroes the radius of its own accord and in
        // case a future build inlines that call. The access specifier differs
        // between builds, so both are listed.
        {
            {
                LR"(public: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))",

                // Older Windows 11 builds:
                LR"(private: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))",
            },
            &GetFloatCornerRadiusForCurrentStyle_Original,
            GetFloatCornerRadiusForCurrentStyle_Hook,
            true,  // Missing in earlier builds (e.g. 10.0.22621.6199).
        },
        // Covers UpdateWindowVisuals in old builds where the radius is
        // computed inline (no call to GetRadiusFromCornerStyle) and passed
        // directly to this function.
        {
            {LR"(public: long __cdecl CWindowBorder::SetBorderParameters(struct tagRECT const &,float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle))"},
            &SetBorderParameters_Original,
            SetBorderParameters_Hook,
        },
        // The rest are for per-corner radii, and are only hooked when those
        // are configured. Per-corner radii are disabled if any of the first
        // three is missing.
        {
            {LR"(public: long __cdecl CRectangleGeometryProxy::SetRectangle(float,float,float,float,float,float,float,float,float,float,float,float,bool))"},
            &SetRectangle_Original,
            perCorner ? SetRectangle_Hook : nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl CWindowBorder::SetClipRectangle(class CRectangleGeometryProxy *,struct tagRECT const &))"},
            &SetClipRectangle_Original,
            perCorner ? SetClipRectangle_Hook : nullptr,
            true,
        },
        {
            {LR"(public: static long __cdecl CWindowBorder::CCachedBorderBrush::CreateBorderSurface(float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle,struct Windows::UI::Composition::ICompositionSurface * *))"},
            &CreateBorderSurface_Original,
            perCorner ? CreateBorderSurface_Hook : nullptr,
            true,
        },
        {
            {LR"(public: virtual __cdecl CWindowBorder::~CWindowBorder(void))"},
            &CWindowBorder_Destructor_Original,
            perCorner ? CWindowBorder_Destructor_Hook : nullptr,
            true,  // Optional - stale entries stay in the per-border map.
        },
        {
            {LR"(public: static long __cdecl ResourceHelper::CreateRectangleGeometry(struct D2D_POINTANDSIZE_L const &,float,class CRectangleGeometryProxy * *))"},
            &CreateRectangleGeometry_Original,
            perCorner ? CreateRectangleGeometry_Hook : nullptr,
            true,  // Optional - animated clips keep a single radius.
        },
        {
            {LR"(private: long __cdecl CTopLevelWindow3D::UpdateAnimatedResources(void))"},
            &UpdateAnimatedResources_Original,
            perCorner ? UpdateAnimatedResources_Hook : nullptr,
            true,  // Optional - animated clips keep a single radius.
        },
    };

    if (!HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    if (perCorner) {
        bool hooked = SetRectangle_Original && SetClipRectangle_Original &&
                      CreateBorderSurface_Original && HookD2DFunctions();
        g_perCornerRadiusHooks = hooked ? PerCornerRadiusHooks::set
                                        : PerCornerRadiusHooks::unsupported;
        if (!hooked) {
            Wh_Log(L"Per-corner radius isn't supported");
            g_settings.perCornerRadius = false;
        }
    }

    // Hooks queued by HookSymbols aren't applied until Wh_ModInit returns, so
    // IsGhostWindow's bytes are still original here. Disassemble its prologue
    // to recover the HWND member offset on CWindowData. The function loads
    // this->hwnd as the first argument to GetPropW, so the first load whose
    // base register is the `this` pointer (rcx on x64, x0 on ARM64) is the
    // HWND member - the destination register is left unconstrained because
    // compilers may stage the value through a scratch register first.
    if (IsGhostWindow_Func) {
        g_windowDataHwndOffset = OffsetFromAssemblyRegex(
            IsGhostWindow_Func, SIZE_MAX,
#if defined(_M_X64)
            std::regex(R"(mov \w+, \[rcx\+0x([0-9a-f]+)\])",
                       std::regex_constants::icase),
#elif defined(_M_ARM64)
            std::regex(R"(ldr\s+\w+, \[x0, #0x([0-9a-f]+)\])",
                       std::regex_constants::icase),
#else
#error "Unsupported architecture"
#endif
            10);
        Wh_Log(L"windowDataHwndOffset=0x%zx", g_windowDataHwndOffset);
    } else {
        Wh_Log(L"IsGhostWindow wasn't found, HWND lookup is disabled");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    RequestDwmRefresh();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    if (g_settings.perCornerRadius &&
        g_perCornerRadiusHooks == PerCornerRadiusHooks::notSet) {
        *bReload = TRUE;
        return TRUE;
    }

    ClearWindowExclusionProps();
    RequestDwmRefresh();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (HWND hDwm = FindWindow(L"Dwm", nullptr)) {
        SetProp(hDwm, kLoadCountProp, IntToPtr(g_loadCount));
    }

    ClearWindowExclusionProps();
    RequestDwmRefresh();
}
