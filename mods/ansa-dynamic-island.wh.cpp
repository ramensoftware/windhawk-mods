// ==WindhawkMod==
// @id              ansa-dynamic-island
// @name            Ansa Dynamic Island
// @description     A sleek Dynamic Island for Windows featuring spring physics, notch & pill modes, live media controls with wavy progress bar, ShelfBook drag-and-drop, calendar, tasks, and hardware HUDs.
// @description:it-IT Una Dynamic Island fluida per Windows con fisica a molla, modalità pillola o notch, player musicale con onda animata, ShelfBook drag-and-drop, calendario, task rapidi e HUD hardware.
// @version         6.5.1
// @author          dev-apox
// @github          https://github.com/dev-apox
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lgdiplus -luser32 -lshell32 -lwininet -lole32 -loleaut32 -lmmdevapi -ldxva2 -lwbemuuid -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ansa Dynamic Island

A sleek, fluent **Dynamic Island** experience for Windows 10 & 11, crafted from scratch by **dev-apox**. Built with native GDI+ vector rendering, damped spring physics, and deep Windows integration.

- **GitHub Repository:** [github.com/dev-apox/ansa-dynamic-island](https://github.com/dev-apox/ansa-dynamic-island)
- **Releases & Changelog:** [View Releases](https://github.com/dev-apox/ansa-dynamic-island/releases)
- **Issues & Feedback:** [Report a Bug](https://github.com/dev-apox/ansa-dynamic-island/issues)

*Clean idle pill displaying the circular battery ring, 12-hour digital clock, and animated audio visualizer during playback.*

---

## ✨ Features

### 🎵 Live Media Control & One UI 9.0 Wavy Bar
*Real-time album artwork extraction, adaptive ambient glow matching the cover art, playback controls, and an interactive One UI 9.0 wavy progress bar with click-to-seek.*

### 📁 ShelfBook (Drag & Drop File Staging)
*Drop files or folders directly onto the island to stage them with color-coded type badges, and drag them out anytime into any browser, chat, or Explorer window.*

### 📅 Interactive Calendar
*Quick-glance date card paired with an interactive 7-day strip and daily schedule preview right at the top of your screen.*

### ✅ Inline Things To Do
*Type, check off, and clear personal tasks directly inside the island without opening any external app.*

### ⚡ Additional Capabilities
- **Adaptive Hardware HUDs:** Instant visual feedback for **Volume & Mute**, **Screen Brightness** (with scroll-wheel adjustment), **Battery & Charging**, **Bluetooth** connect/disconnect events, and **Wi-Fi** status.
- **Toast Notification Interceptor:** Captures Windows toast banners and renders them inside the island with a pulsing accent glow.
- **Fluid Spring Physics Engine:** Smooth, interruptible spring animations with selectable *Normal* and *Bouncy* presets and micro-recoil feedback.
- **Smart Fullscreen Auto-Hide:** Automatically hides when games, videos, or fullscreen apps are active.

---

## 🖐️ Mouse Gestures

| Action | Gesture / Input |
| :--- | :--- |
| **Expand Hub** | Hover over the island or touch the top-center edge of the screen |
| **Switch Hub Pages** | **Two-finger horizontal swipe** on a Precision Touchpad, click-and-drag horizontally, or `Left` / `Right` arrow keys |
| **Adjust Volume / Brightness** | Scroll the mouse wheel / two-finger vertical scroll while the Volume or Brightness HUD is visible |
| **Open Ansa Studio** | Right-click anywhere on the island or click the gear icon |
| **Stage / Retrieve Files** | Drag files onto the island to store in **ShelfBook**; drag cards out to drop them into any app |

---

## ⚙️ Settings (Ansa Studio)

Right-click the island (or click the gear icon) to customize everything live inside **Ansa Studio**:

### 🎨 Style & Physics
*Switch between Floating Pill and Curved Top Notch, adjust UI Scale (100%–150%), choose visual themes (Album Glow, Dark Glass, OLED Black), set Normal or Bouncy spring physics, and toggle Fullscreen Auto-Hide.*

### 🧩 Idle & Behavior
*Customize the idle layout (Ring + Clock, Ring Only, Clock Only, or Minimal), media module, top screen edge hover expansion, Bluetooth HUD, and notification interception.*

### 🌈 Accent Theme
*Pick your live system accent color from six curated presets: iOS Blue, Indigo, Emerald, Amber, Crimson, and Pure Silver.*

---

## 👤 Author
Made by **dev-apox**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- IslandStyle: floating
  $name: Island Style
  $description: Choose between floating pill or curved top notch.
  $options:
  - floating: Floating Pill
  - notch: Top Notch
- UiScale: "115"
  $name: UI Scale (%)
  $description: Overall scale multiplier for high-DPI or larger screens.
  $options:
  - "100": 100% (Standard)
  - "115": 115% (Large - Recommended)
  - "130": 130% (Extra Large)
  - "150": 150% (2K / 4K Display)
- TopMargin: 12
  $name: Top Screen Offset (pixels)
- IdleWidth: 196
  $name: Idle Base Width (pixels)
- IdleHeight: 42
  $name: Idle Base Height (pixels)
- ThemeMode: glow
  $name: Visual Theme
  $options:
  - glow: OLED Black + Adaptive Glow
  - oled: Pure OLED Black
  - glass: Frosted Dark Glass
- AccentColor: blue
  $name: Accent Color
  $options:
  - blue: iOS Blue
  - purple: Indigo Purple
  - green: Emerald Green
  - orange: Solar Amber
  - pink: Crimson
  - silver: Pure Silver
- IdleDisplay: ring_clock
  $name: Idle Content
  $options:
  - ring_clock: Battery Ring (No %) + 12h Clock
  - ring_only: Battery Ring Only (No %)
  - clock: 12h Digital Clock Only
  - minimal: Minimal Mode (Clean Pill)
- MinimalMode: false
  $name: Ultra-Minimal Idle Mode
  $description: Hides clock and ring in idle state for a pure minimal bar until hovered or triggered.
- HideFullscreen: true
  $name: Auto-Hide in Fullscreen
  $description: Automatically hides the Dynamic Island when games, videos, or apps run in fullscreen.
- IdleOpacity: "100"
  $name: Idle Opacity (%)
  $options:
  - "100": 100% (Solid)
  - "85": 85% (Subtle)
  - "70": 70% (Translucent)
- EdgeHoverExpand: true
  $name: Expand on Top Screen Edge Hover
- EnableMedia: true
  $name: Enable Media Player & One UI 9.0 Wave Bar
- EnableVolume: true
  $name: Enable Volume & Mute HUD
- EnableBrightness: true
  $name: Enable Screen Brightness HUD
- EnableBattery: true
  $name: Enable One UI Battery & Charging HUD
- EnableBluetooth: true
  $name: Enable Bluetooth Connect/Disconnect HUD
- EnableNetwork: true
  $name: Enable Network / Wi-Fi HUD
- EnableNotifications: true
  $name: Enable Windows Toast Notification Interceptor
- SpringFeel: normal
  $name: Animation Physics Feel
  $options:
  - normal: Normal
  - bouncy: Bouncy
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shlobj.h>
#include <wininet.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <wbemidl.h>
#include <shlwapi.h>
#include <psapi.h>

#include <cstdio>
#include <cwchar>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <mutex>
#include <atomic>

using namespace Gdiplus;

// ============================================================================
// SECTION 1: COM, WINRT & BLUETOOTH INTERFACES (ZERO EXTERNAL DEPENDENCIES)
// ============================================================================

typedef void* HSTRING_LOCAL;

typedef HRESULT(WINAPI* PFN_RoGetActivationFactory)(HSTRING_LOCAL activatableClassId, REFIID iid, void** factory);
typedef HRESULT(WINAPI* PFN_WindowsCreateString)(LPCWSTR sourceString, UINT32 length, HSTRING_LOCAL* string);
typedef HRESULT(WINAPI* PFN_WindowsDeleteString)(HSTRING_LOCAL string);
typedef PCWSTR(WINAPI* PFN_WindowsGetStringRawBuffer)(HSTRING_LOCAL string, UINT32* length);
typedef HRESULT(WINAPI* PFN_CreateStreamOverRandomAccessStream)(IUnknown* randomAccessStream, REFIID riid, void** ppv);

static PFN_RoGetActivationFactory pRoGetActivationFactory = NULL;
static PFN_WindowsCreateString pWindowsCreateString = NULL;
static PFN_WindowsDeleteString pWindowsDeleteString = NULL;
static PFN_WindowsGetStringRawBuffer pWindowsGetStringRawBuffer = NULL;
static PFN_CreateStreamOverRandomAccessStream pCreateStreamOverRandomAccessStream = NULL;

#ifndef WM_POINTERUPDATE
#define WM_POINTERUPDATE 0x0245
#define WM_POINTERDOWN   0x0246
#define WM_POINTERUP     0x0247
#endif

struct POINTER_INFO_LOCAL {
    DWORD pointerType;
    UINT32 pointerId;
    UINT32 frameId;
    UINT32 pointerFlags;
    HANDLE sourceDevice;
    HWND hwndTarget;
    POINT ptPixelLocation;
    POINT ptHimetricLocation;
    POINT ptPixelLocationRaw;
    POINT ptHimetricLocationRaw;
    DWORD dwTime;
    UINT32 historyCount;
    INT32 InputData;
    DWORD dwKeyStates;
    UINT64 PerformanceCount;
    int ButtonChangeType;
};

typedef BOOL(WINAPI* PFN_GetPointerInfo)(UINT32 pointerId, POINTER_INFO_LOCAL* pointerInfo);
static PFN_GetPointerInfo pGetPointerInfo = NULL;

#pragma pack(push, 1)
struct BTH_ADDR_LOCAL {
    ULONGLONG ullLong;
};

struct BLUETOOTH_DEVICE_SEARCH_PARAMS_LOCAL {
    DWORD dwSize;
    BOOL fReturnAuthenticated;
    BOOL fReturnRemembered;
    BOOL fReturnUnknown;
    BOOL fReturnConnected;
    BOOL fIssueInquiry;
    UCHAR cTimeoutMultiplier;
    HANDLE hRadio;
};

struct BLUETOOTH_DEVICE_INFO_LOCAL {
    DWORD dwSize;
    BTH_ADDR_LOCAL Address;
    ULONG ulClassofDevice;
    BOOL fConnected;
    BOOL fRemembered;
    BOOL fAuthenticated;
    SYSTEMTIME stLastSeen;
    SYSTEMTIME stLastUsed;
    WCHAR szName[248];
};
#pragma pack(pop)

typedef HANDLE(WINAPI* PFN_BluetoothFindFirstDevice)(const BLUETOOTH_DEVICE_SEARCH_PARAMS_LOCAL* pbtsp, BLUETOOTH_DEVICE_INFO_LOCAL* pbtdi);
typedef BOOL(WINAPI* PFN_BluetoothFindNextDevice)(HANDLE hFind, BLUETOOTH_DEVICE_INFO_LOCAL* pbtdi);
typedef BOOL(WINAPI* PFN_BluetoothFindDeviceClose)(HANDLE hFind);

static PFN_BluetoothFindFirstDevice pBluetoothFindFirstDevice = NULL;
static PFN_BluetoothFindNextDevice pBluetoothFindNextDevice = NULL;
static PFN_BluetoothFindDeviceClose pBluetoothFindDeviceClose = NULL;

enum AsyncStatusLocal {
    AsyncStatus_Started = 0,
    AsyncStatus_Completed = 1,
    AsyncStatus_Canceled = 2,
    AsyncStatus_Error = 3
};

MIDL_INTERFACE("00000036-0000-0000-C000-000000000046")
IAsyncInfoLocal : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Id(unsigned int* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Status(AsyncStatusLocal* status) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* errorCode) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
};

template <typename TResult>
struct IAsyncOperationLocal : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Completed(IUnknown* handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(IUnknown** handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResults(TResult* results) = 0;
};

MIDL_INTERFACE("905A0FE1-BC53-11DF-8C49-001E4FC686DA")
IRandomAccessStreamLocal : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Size(UINT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Size(UINT64 value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInputStreamAt(UINT64 position, IUnknown** stream) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOutputStreamAt(UINT64 position, IUnknown** stream) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Position(UINT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE Seek(UINT64 position) = 0;
    virtual HRESULT STDMETHODCALLTYPE CloneStream(IRandomAccessStreamLocal** stream) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CanRead(boolean* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CanWrite(boolean* value) = 0;
};

MIDL_INTERFACE("33EE3134-1DD6-4E3A-8067-D1C162E8642B")
IRandomAccessStreamReferenceLocal : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE OpenReadAsync(IAsyncOperationLocal<IRandomAccessStreamLocal*>** operation) = 0;
};

MIDL_INTERFACE("68856CF6-ADB4-54B2-AC16-05837907ACB6")
ISMTC_MediaProperties : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Title(HSTRING_LOCAL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Subtitle(HSTRING_LOCAL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AlbumArtist(HSTRING_LOCAL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Artist(HSTRING_LOCAL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AlbumTitle(HSTRING_LOCAL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TrackNumber(INT32* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Genres(IUnknown** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AlbumTrackCount(INT32* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PlaybackType(IUnknown** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Thumbnail(IRandomAccessStreamReferenceLocal** value) = 0;
};

MIDL_INTERFACE("94B4B6CF-E8BA-51AD-87A7-C10ADE106127")
ISMTC_PlaybackInfo : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Controls(IUnknown** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PlaybackStatus(int* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PlaybackType(IUnknown** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AutoRepeatMode(IUnknown** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PlaybackRate(IUnknown** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsShuffleActive(IUnknown** value) = 0;
};

MIDL_INTERFACE("EDE34136-6F25-588D-8ECF-EA5B6735AAA5")
ISMTC_TimelineProperties : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartTime(INT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EndTime(INT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MinSeekTime(INT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxSeekTime(INT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Position(INT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LastUpdatedTime(INT64* value) = 0;
};

MIDL_INTERFACE("7148C835-9B14-5AE2-AB85-39B1C9706684")
ISMTC_Session : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SourceAppUserModelId(HSTRING_LOCAL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetMediaPropertiesAsync(IAsyncOperationLocal<ISMTC_MediaProperties*>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTimelineProperties(ISMTC_TimelineProperties** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPlaybackInfo(ISMTC_PlaybackInfo** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryPlayAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryPauseAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryStopAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryRecordAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryFastForwardAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryRewindAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TrySkipNextAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TrySkipPreviousAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryChangeChannelUpAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryChangeChannelDownAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryTogglePlayPauseAsync(IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryChangeAutoRepeatModeAsync(int mode, IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryChangePlaybackRateAsync(double rate, IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryChangeShuffleActiveAsync(boolean active, IAsyncOperationLocal<boolean>** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryChangePlaybackPositionAsync(INT64 requestedPlaybackPosition, IAsyncOperationLocal<boolean>** operation) = 0;
};

MIDL_INTERFACE("CACE9EAC-5E2E-5016-8007-EB475D4B01D6")
ISMTC_SessionManager : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSession(ISMTC_Session** value) = 0;
};

MIDL_INTERFACE("2050C4EE-11A0-57DE-AED7-C97C70338245")
ISMTC_SessionManagerStatics : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING_LOCAL* className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestAsync(IAsyncOperationLocal<ISMTC_SessionManager*>** operation) = 0;
};

MIDL_INTERFACE("C02216F6-8C67-4B5B-9D00-D008E73E0064")
IMyAudioMeterInformation : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* pdwHardwareSupportMask) = 0;
};

static const IID IID_IMyAudioMeterInformation = {
    0xC02216F6, 0x8C67, 0x4B5B, { 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64 }
};

MIDL_INTERFACE("D22108AA-8AC5-49A5-837B-37BBB3D7591E")
IMyUIAutomationElement : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE SetFocus(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeId(SAFEARRAY** runtimeId) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirst(int scope, IUnknown* condition, IMyUIAutomationElement** found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAll(int scope, IUnknown* condition, IUnknown** found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirstBuildCache(int scope, IUnknown* condition, IUnknown* cacheRequest, IMyUIAutomationElement** found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAllBuildCache(int scope, IUnknown* condition, IUnknown* cacheRequest, IMyUIAutomationElement** found) = 0;
    virtual HRESULT STDMETHODCALLTYPE BuildUpdatedCache(IUnknown* cacheRequest, IMyUIAutomationElement** updatedElement) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValue(int propertyId, VARIANT* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValueEx(int propertyId, BOOL ignoreDefaultValue, VARIANT* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValue(int propertyId, VARIANT* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValueEx(int propertyId, BOOL ignoreDefaultValue, VARIANT* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPatternAs(int patternId, REFIID riid, void** patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPatternAs(int patternId, REFIID riid, void** patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPattern(int patternId, IUnknown** patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPattern(int patternId, IUnknown** patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedParent(IMyUIAutomationElement** parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedChildren(IUnknown** children) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentProcessId(int* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentControlType(int* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentLocalizedControlType(BSTR* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentName(BSTR* retVal) = 0;
};

MIDL_INTERFACE("14314595-B4BC-4055-95F2-58F2E42C9855")
IMyUIAutomationElementArray : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE get_Length(int* length) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetElement(int index, IMyUIAutomationElement** element) = 0;
};

MIDL_INTERFACE("30CBE57D-D9D0-452A-AB13-7AC5AC4825EE")
IMyUIAutomation : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE CompareElements(IMyUIAutomationElement* el1, IMyUIAutomationElement* el2, BOOL* areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE CompareRuntimeIds(SAFEARRAY* runtimeId1, SAFEARRAY* runtimeId2, BOOL* areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElement(IMyUIAutomationElement** root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandle(HWND hwnd, IMyUIAutomationElement** element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPoint(POINT pt, IMyUIAutomationElement** element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElement(IMyUIAutomationElement** element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElementBuildCache(IUnknown* cacheRequest, IMyUIAutomationElement** root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandleBuildCache(HWND hwnd, IUnknown* cacheRequest, IMyUIAutomationElement** element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPointBuildCache(POINT pt, IUnknown* cacheRequest, IMyUIAutomationElement** element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElementBuildCache(IUnknown* cacheRequest, IMyUIAutomationElement** element) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTreeWalker(IUnknown* pCondition, IUnknown** walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewWalker(IUnknown** walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewWalker(IUnknown** walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewWalker(IUnknown** walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewCondition(IUnknown** condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewCondition(IUnknown** condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewCondition(IUnknown** condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCacheRequest(IUnknown** cacheRequest) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTrueCondition(IUnknown** newCondition) = 0;
};

static const CLSID CLSID_MyCUIAutomation = {
    0xFF48DBA4, 0x60EF, 0x4201, { 0xAA, 0x87, 0x54, 0x10, 0x3E, 0xEF, 0x59, 0x4E }
};
static const IID IID_IMyUIAutomation = {
    0x30CBE57D, 0xD9D0, 0x452A, { 0xAB, 0x13, 0x7A, 0xC5, 0xAC, 0x48, 0x25, 0xEE }
};

// ============================================================================
// SECTION 2: OLE FILE DRAG-OUT (DRAG FILES FROM SHELBOOK TO EXPLORER/APPS)
// ============================================================================

class SimpleEnumFormatEtc : public IEnumFORMATETC {
    LONG m_refCount;
    ULONG m_index;
    FORMATETC m_fmt;
public:
    SimpleEnumFormatEtc() : m_refCount(1), m_index(0) {
        m_fmt.cfFormat = CF_HDROP;
        m_fmt.ptd = NULL;
        m_fmt.dwAspect = DVASPECT_CONTENT;
        m_fmt.lindex = -1;
        m_fmt.tymed = TYMED_HGLOBAL;
    }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IEnumFORMATETC) {
            *ppv = static_cast<IEnumFORMATETC*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() override {
        LONG c = InterlockedDecrement(&m_refCount);
        if (c == 0) delete this;
        return c;
    }
    STDMETHODIMP Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) override {
        if (!rgelt) return E_INVALIDARG;
        if (m_index == 0 && celt > 0) {
            rgelt[0] = m_fmt;
            m_index = 1;
            if (pceltFetched) *pceltFetched = 1;
            return (celt == 1) ? S_OK : S_FALSE;
        }
        if (pceltFetched) *pceltFetched = 0;
        return S_FALSE;
    }
    STDMETHODIMP Skip(ULONG celt) override {
        m_index += celt;
        return (m_index <= 1) ? S_OK : S_FALSE;
    }
    STDMETHODIMP Reset() override {
        m_index = 0;
        return S_OK;
    }
    STDMETHODIMP Clone(IEnumFORMATETC** ppenum) override {
        SimpleEnumFormatEtc* p = new SimpleEnumFormatEtc();
        p->m_index = m_index;
        *ppenum = p;
        return S_OK;
    }
};

class FileDropDataObject : public IDataObject {
    LONG m_refCount;
    std::wstring m_filePath;
public:
    FileDropDataObject(const std::wstring& path) : m_refCount(1), m_filePath(path) {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDataObject) {
            *ppv = static_cast<IDataObject*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() override {
        LONG c = InterlockedDecrement(&m_refCount);
        if (c == 0) delete this;
        return c;
    }
    STDMETHODIMP GetData(FORMATETC* pfmt, STGMEDIUM* pmed) override {
        if (!pfmt || !pmed) return E_INVALIDARG;
        if (pfmt->cfFormat != CF_HDROP || !(pfmt->tymed & TYMED_HGLOBAL)) return DV_E_FORMATETC;

        SIZE_T bytes = sizeof(DROPFILES) + (m_filePath.size() + 2) * sizeof(wchar_t);
        HGLOBAL hGlob = GlobalAlloc(GHND | GMEM_SHARE, bytes);
        if (!hGlob) return E_OUTOFMEMORY;

        DROPFILES* pDf = (DROPFILES*)GlobalLock(hGlob);
        pDf->pFiles = sizeof(DROPFILES);
        pDf->fWide = TRUE;
        wchar_t* pDest = (wchar_t*)((BYTE*)pDf + sizeof(DROPFILES));
        wcscpy_s(pDest, m_filePath.size() + 1, m_filePath.c_str());
        pDest[m_filePath.size() + 1] = L'\0';
        GlobalUnlock(hGlob);

        pmed->tymed = TYMED_HGLOBAL;
        pmed->hGlobal = hGlob;
        pmed->pUnkForRelease = NULL;
        return S_OK;
    }
    STDMETHODIMP GetDataHere(FORMATETC*, STGMEDIUM*) override { return E_NOTIMPL; }
    STDMETHODIMP QueryGetData(FORMATETC* pfmt) override {
        if (!pfmt) return E_INVALIDARG;
        return (pfmt->cfFormat == CF_HDROP && (pfmt->tymed & TYMED_HGLOBAL)) ? S_OK : DV_E_FORMATETC;
    }
    STDMETHODIMP GetCanonicalFormatEtc(FORMATETC*, FORMATETC* pOut) override {
        if (pOut) pOut->ptd = NULL;
        return E_NOTIMPL;
    }
    STDMETHODIMP SetData(FORMATETC*, STGMEDIUM*, BOOL) override { return E_NOTIMPL; }
    STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenum) override {
        if (dwDirection == DATADIR_GET && ppenum) {
            *ppenum = new SimpleEnumFormatEtc();
            return S_OK;
        }
        return E_NOTIMPL;
    }
    STDMETHODIMP DAdvise(FORMATETC*, DWORD, IAdviseSink*, DWORD*) override { return OLE_E_ADVISENOTSUPPORTED; }
    STDMETHODIMP DUnadvise(DWORD) override { return OLE_E_ADVISENOTSUPPORTED; }
    STDMETHODIMP EnumDAdvise(IEnumSTATDATA**) override { return OLE_E_ADVISENOTSUPPORTED; }
};

class SimpleDropSource : public IDropSource {
    LONG m_refCount;
public:
    SimpleDropSource() : m_refCount(1) {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDropSource) {
            *ppv = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() override {
        LONG c = InterlockedDecrement(&m_refCount);
        if (c == 0) delete this;
        return c;
    }
    STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) override {
        if (fEscapePressed) return DRAGDROP_S_CANCEL;
        if (!(grfKeyState & MK_LBUTTON)) return DRAGDROP_S_DROP;
        return S_OK;
    }
    STDMETHODIMP GiveFeedback(DWORD) override { return DRAGDROP_S_USEDEFAULTCURSORS; }
};

void StartExternalFileDrag(const std::wstring& filePath) {
    FileDropDataObject* pData = new FileDropDataObject(filePath);
    SimpleDropSource* pSrc = new SimpleDropSource();
    DWORD dwEffect = DROPEFFECT_NONE;
    DoDragDrop(pData, pSrc, DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK, &dwEffect);
    pData->Release();
    pSrc->Release();
}

// ============================================================================
// SECTION 3: DATA MODELS, CONFIGURATION & STATE ENUMS
// ============================================================================

enum IslandViewMode {
    VIEW_IDLE = 0,
    VIEW_EXPANDED_HUB,
    VIEW_SETTINGS_STUDIO,
    VIEW_HUD_VOLUME,
    VIEW_HUD_BRIGHTNESS,
    VIEW_HUD_BATTERY,
    VIEW_HUD_BLUETOOTH,
    VIEW_HUD_NETWORK,
    VIEW_TOAST_NOTIFICATION
};

enum HubPage {
    PAGE_MEDIA = 0,
    PAGE_SHELFBOOK = 1,
    PAGE_CALENDAR = 2,
    PAGE_TODO = 3,
    PAGE_COUNT = 4
};

enum SettingsTab {
    TAB_STYLE = 0,
    TAB_MODULES = 1,
    TAB_COLORS = 2
};

enum ThemeType {
    THEME_GLOW = 0,
    THEME_OLED = 1,
    THEME_GLASS = 2
};

enum IdleDisplayType {
    IDLE_RING_CLOCK = 0,
    IDLE_RING_ONLY = 1,
    IDLE_CLOCK = 2,
    IDLE_MINIMAL = 3
};

enum SpringPreset {
    SPRING_NORMAL = 0,
    SPRING_BOUNCY = 1
};

struct IslandConfig {
    bool isNotch;
    float uiScale;
    int topMargin;
    float baseIdleW;
    float baseIdleH;
    ThemeType theme;
    int accentIdx;
    IdleDisplayType idleDisplay;
    bool minimalMode;
    bool hideFullscreen;
    int idleOpacity;
    bool edgeHoverExpand;
    bool enableMedia;
    bool enableVolume;
    bool enableBrightness;
    bool enableBattery;
    bool enableBluetooth;
    bool enableNetwork;
    bool enableNotifications;
    SpringPreset springPreset;
};

struct ShelfEntry {
    std::wstring fullPath;
    std::wstring fileName;
    std::wstring extBadge;
    std::wstring sizeText;
    Color badgeColor;
    bool isDirectory;
};

struct TodoItem {
    std::wstring text;
    bool done;
};

struct MediaState {
    bool hasSession;
    bool isPlaying;
    std::wstring title;
    std::wstring artist;
    std::wstring album;
    std::wstring sourceApp;
    INT64 posTicks;
    INT64 endTicks;
    float progress;
    Bitmap* pAlbumArt;
    Color dominantColor;
};

struct HudPayload {
    IslandViewMode mode;
    std::wstring title;
    std::wstring subtitle;
    std::wstring bodyText;
    float progress;
    float animProgress;
    Color accent;
    ULONGLONG expireTick;
    int subState;
};

struct ConnectedBtDevice {
    ULONGLONG addr;
    std::wstring name;
};

// ============================================================================
// SECTION 4: GLOBAL ENGINE STATE & SYNCHRONIZATION
// ============================================================================

static IslandConfig g_cfg;
static HWND g_hWnd = NULL;
static HANDLE g_hUiThread = NULL;
static HANDLE g_hWorkerThread = NULL;
static std::atomic<bool> g_running{ true };
static ULONG_PTR g_gdiplusToken = 0;
static HMODULE g_hComBase = NULL;
static HMODULE g_hShCore = NULL;
static HMODULE g_hBthProps = NULL;

struct SpringVal {
    float current;
    float velocity;
    float target;

    void Reset(float v) {
        current = v;
        velocity = 0.0f;
        target = v;
    }

    void Step(float stiffness, float damping) {
        float force = (target - current) * stiffness;
        velocity = (velocity + force) * damping;
        current += velocity;
        if (std::abs(target - current) < 0.03f && std::abs(velocity) < 0.03f) {
            current = target;
            velocity = 0.0f;
        }
    }
};

static SpringVal g_sprW;
static SpringVal g_sprH;
static SpringVal g_sprGlow;
static SpringVal g_sprBounceY;
static SpringVal g_sprBounceX;
static SpringVal g_sprPageScroll;

static IslandViewMode g_viewMode = VIEW_IDLE;
static int g_hubPage = PAGE_MEDIA;
static SettingsTab g_settingsTab = TAB_STYLE;

static bool g_isHovered = false;
static bool g_isMouseDown = false;
static bool g_isDraggingPage = false;
static bool g_isHiddenByFullscreen = false;
static POINT g_mouseDownPos = { 0, 0 };
static float g_dragStartPageScroll = 0.0f;
static int g_dragShelfSlot = -1;

// Touchpad Two-Finger Gesture & Pointer State
static float g_touchpadAccumX = 0.0f;
static float g_touchpadAccumY = 0.0f;
static ULONGLONG g_lastTouchpadEventTick = 0;
static ULONGLONG g_lastTouchpadSnapTick = 0;
static POINT g_gestureStartPt = { 0, 0 };
static UINT32 g_activePointerId = 0;
static POINT g_pointerStartPt = { 0, 0 };
static float g_pointerStartScroll = 0.0f;

// Continuous Animation Phases
static float g_wavePhase = 0.0f;
static float g_waveAmplitude = 0.0f;
static float g_oneUiPulsePhase = 0.0f;
static float g_idleBatSmooth = 1.0f;

static HudPayload g_hud;
static std::mutex g_mediaMutex;
static MediaState g_media;
static std::atomic<int> g_pendingMediaCommand{ 0 };
static std::atomic<INT64> g_pendingSeekTicks{ -1 };

// ShelfBook State
static std::vector<ShelfEntry> g_shelf;
static int g_shelfPage = 0;

// Calendar & Personalized To-Do State
static int g_selectedCalDayOffset = 0;
static int g_calEventCycle = 0;
static std::vector<TodoItem> g_todos;
static bool g_isTypingTask = false;
static std::wstring g_taskDraft = L"";

// Hardware Monitors
static IAudioEndpointVolume* g_pVolume = NULL;
static IMyAudioMeterInformation* g_pMeter = NULL;
static HANDLE g_hPhysMonitor = NULL;
static IWbemServices* g_pWbemServices = NULL;
static IMyUIAutomation* g_pUIAuto = NULL;

static float g_audioPeak = 0.0f;
static float g_eqBars[5] = { 0.1f, 0.2f, 0.15f, 0.25f, 0.1f };
static float g_lastVolume = -1.0f;
static BOOL g_lastMute = FALSE;
static int g_lastBrightness = -1;
static BYTE g_lastACStatus = 255;
static int g_lastBatPct = -1;
static bool g_lastNetConnected = true;
static HWND g_lastToastHwnd = NULL;
static std::wstring g_lastToastSignature = L"";
static std::vector<ConnectedBtDevice> g_lastBtDevices;
static bool g_btInitialized = false;

// Interactive Hitboxes
struct HitBoxes {
    RECT btnSettings;
    RECT pageDots[4];
    // Page 0: Media
    RECT btnPrev;
    RECT btnPlay;
    RECT btnNext;
    RECT barProgress;
    // Page 1: ShelfBook
    RECT shelfSlotOpen[2];
    RECT shelfSlotDel[2];
    RECT shelfPrevPage;
    RECT shelfNextPage;
    RECT shelfClearAll;
    // Page 2: Calendar
    RECT calDays[7];
    RECT calEventPill;
    // Page 3: Things To Do
    RECT todoRows[3];
    RECT todoRowDel[3];
    RECT todoAddBtn;
    RECT todoClearDone;
    RECT todoInputSave;
    RECT todoInputCancel;
    // Settings Studio
    RECT setClose;
    RECT setTabStyle;
    RECT setTabModules;
    RECT setTabColors;
    RECT optBtn[6];
    RECT colorSwatches[6];
};

static HitBoxes g_hit = {};

// ============================================================================
// SECTION 5: COLOR PALETTE & BATTERY COLOR LOGIC
// ============================================================================

Color GetAccentColorByIndex(int idx) {
    switch ((idx % 6 + 6) % 6) {
        case 0: return Color(255, 10, 132, 255);  // iOS System Blue
        case 1: return Color(255, 191, 90, 242);  // Indigo Purple
        case 2: return Color(255, 48, 209, 88);   // Emerald Green
        case 3: return Color(255, 255, 159, 10);  // Solar Amber
        case 4: return Color(255, 255, 55, 95);   // Crimson
        default: return Color(255, 229, 229, 234);// Pure Silver
    }
}

std::wstring GetAccentNameByIndex(int idx) {
    switch ((idx % 6 + 6) % 6) {
        case 0: return L"iOS Blue";
        case 1: return L"Indigo";
        case 2: return L"Emerald";
        case 3: return L"Amber";
        case 4: return L"Crimson";
        default: return L"Silver";
    }
}

Color GetBatteryDynamicColor(int pct, bool isCharging) {
    if (isCharging && pct >= 30) {
        return Color(255, 48, 209, 88);
    }
    if (pct <= 15) {
        return Color(255, 255, 69, 58);
    }
    if (pct < 30) {
        return Color(255, 255, 159, 10);
    }
    return Color(255, 48, 209, 88);
}

std::wstring FormatDurationTicks(INT64 ticks100ns) {
    if (ticks100ns < 0) ticks100ns = 0;
    INT64 totalSec = ticks100ns / 10000000LL;
    int mins = (int)(totalSec / 60);
    int secs = (int)(totalSec % 60);
    wchar_t buf[32];
    swprintf_s(buf, L"%d:%02d", mins, secs);
    return buf;
}

std::wstring FormatFileSize(ULONGLONG bytes) {
    wchar_t buf[64];
    if (bytes < 1024ULL) swprintf_s(buf, L"%llu B", bytes);
    else if (bytes < 1024ULL * 1024ULL) swprintf_s(buf, L"%.1f KB", bytes / 1024.0);
    else if (bytes < 1024ULL * 1024ULL * 1024ULL) swprintf_s(buf, L"%.1f MB", bytes / (1024.0 * 1024.0));
    else swprintf_s(buf, L"%.2f GB", bytes / (1024.0 * 1024.0 * 1024.0));
    return buf;
}

std::wstring ToUpperString(const std::wstring& s) {
    std::wstring out = s;
    for (size_t i = 0; i < out.size(); i++) out[i] = towupper(out[i]);
    return out;
}

std::wstring CleanAppDisplayName(const std::wstring& aumid) {
    if (aumid.empty()) return L"Media Player";
    std::wstring lower = aumid;
    for (size_t i = 0; i < lower.size(); i++) lower[i] = towlower(lower[i]);

    if (lower.find(L"spotify") != std::wstring::npos) return L"Spotify";
    if (lower.find(L"chrome") != std::wstring::npos) return L"Chrome";
    if (lower.find(L"msedge") != std::wstring::npos) return L"Edge";
    if (lower.find(L"brave") != std::wstring::npos) return L"Brave";
    if (lower.find(L"firefox") != std::wstring::npos) return L"Firefox";
    if (lower.find(L"opera") != std::wstring::npos) return L"Opera";
    if (lower.find(L"applemusic") != std::wstring::npos) return L"Apple Music";
    if (lower.find(L"tidal") != std::wstring::npos) return L"TIDAL";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC";
    if (lower.find(L"zune") != std::wstring::npos || lower.find(L"mediaplayer") != std::wstring::npos) return L"Media Player";
    if (lower.find(L"discord") != std::wstring::npos) return L"Discord";

    size_t bang = aumid.find(L'!');
    std::wstring base = (bang != std::wstring::npos) ? aumid.substr(0, bang) : aumid;
    size_t dot = base.find_last_of(L'.');
    if (dot != std::wstring::npos && dot + 1 < base.size()) base = base.substr(dot + 1);
    if (base.size() > 20) base = base.substr(0, 20);
    return base;
}

Color ExtractDominantColor(Bitmap* bmp) {
    if (!bmp || bmp->GetLastStatus() != Ok) return GetAccentColorByIndex(g_cfg.accentIdx);
    UINT w = bmp->GetWidth();
    UINT h = bmp->GetHeight();
    if (w == 0 || h == 0) return GetAccentColorByIndex(g_cfg.accentIdx);

    double sumR = 0.0, sumG = 0.0, sumB = 0.0, totalWeight = 0.0;
    UINT stepX = std::max(1U, w / 16U);
    UINT stepY = std::max(1U, h / 16U);

    for (UINT y = 0; y < h; y += stepY) {
        for (UINT x = 0; x < w; x += stepX) {
            Color px;
            if (bmp->GetPixel(x, y, &px) == Ok) {
                float r = px.GetR() / 255.0f;
                float g = px.GetG() / 255.0f;
                float b = px.GetB() / 255.0f;
                float maxC = std::max(r, std::max(g, b));
                float minC = std::min(r, std::min(g, b));
                float sat = (maxC > 0.001f) ? ((maxC - minC) / maxC) : 0.0f;
                float lum = (maxC + minC) * 0.5f;

                if (lum > 0.12f && lum < 0.90f) {
                    double weight = 0.25 + sat * 2.6;
                    sumR += px.GetR() * weight;
                    sumG += px.GetG() * weight;
                    sumB += px.GetB() * weight;
                    totalWeight += weight;
                }
            }
        }
    }

    if (totalWeight < 0.1) return GetAccentColorByIndex(g_cfg.accentIdx);

    BYTE outR = (BYTE)std::min(255.0, (sumR / totalWeight) * 1.15);
    BYTE outG = (BYTE)std::min(255.0, (sumG / totalWeight) * 1.15);
    BYTE outB = (BYTE)std::min(255.0, (sumB / totalWeight) * 1.15);
    return Color(255, outR, outG, outB);
}

// ============================================================================
// SECTION 6: CONFIGURATION, FULLSCREEN CHECK & SPRING PHYSICS
// ============================================================================

void LoadWindhawkConfiguration() {
    PCWSTR style = Wh_GetStringSetting(L"IslandStyle");
    g_cfg.isNotch = (wcscmp(style, L"notch") == 0);
    Wh_FreeStringSetting(style);

    PCWSTR scaleStr = Wh_GetStringSetting(L"UiScale");
    int scalePct = _wtoi(scaleStr);
    Wh_FreeStringSetting(scaleStr);
    if (scalePct < 80 || scalePct > 200) scalePct = 115;
    g_cfg.uiScale = scalePct / 100.0f;

    g_cfg.topMargin = Wh_GetIntSetting(L"TopMargin");
    if (g_cfg.topMargin < 0) g_cfg.topMargin = 12;

    g_cfg.baseIdleW = (float)Wh_GetIntSetting(L"IdleWidth");
    g_cfg.baseIdleH = (float)Wh_GetIntSetting(L"IdleHeight");
    if (g_cfg.baseIdleW < 120.0f) g_cfg.baseIdleW = 196.0f;
    if (g_cfg.baseIdleH < 32.0f) g_cfg.baseIdleH = 42.0f;

    PCWSTR theme = Wh_GetStringSetting(L"ThemeMode");
    if (wcscmp(theme, L"oled") == 0) g_cfg.theme = THEME_OLED;
    else if (wcscmp(theme, L"glass") == 0) g_cfg.theme = THEME_GLASS;
    else g_cfg.theme = THEME_GLOW;
    Wh_FreeStringSetting(theme);

    PCWSTR acc = Wh_GetStringSetting(L"AccentColor");
    if (wcscmp(acc, L"purple") == 0) g_cfg.accentIdx = 1;
    else if (wcscmp(acc, L"green") == 0) g_cfg.accentIdx = 2;
    else if (wcscmp(acc, L"orange") == 0) g_cfg.accentIdx = 3;
    else if (wcscmp(acc, L"pink") == 0) g_cfg.accentIdx = 4;
    else if (wcscmp(acc, L"silver") == 0) g_cfg.accentIdx = 5;
    else g_cfg.accentIdx = 0;
    Wh_FreeStringSetting(acc);

    PCWSTR idleDisp = Wh_GetStringSetting(L"IdleDisplay");
    if (wcscmp(idleDisp, L"ring_only") == 0) g_cfg.idleDisplay = IDLE_RING_ONLY;
    else if (wcscmp(idleDisp, L"clock") == 0) g_cfg.idleDisplay = IDLE_CLOCK;
    else if (wcscmp(idleDisp, L"minimal") == 0) g_cfg.idleDisplay = IDLE_MINIMAL;
    else g_cfg.idleDisplay = IDLE_RING_CLOCK;
    Wh_FreeStringSetting(idleDisp);

    g_cfg.minimalMode = Wh_GetIntSetting(L"MinimalMode") != 0;
    g_cfg.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;

    PCWSTR opacityStr = Wh_GetStringSetting(L"IdleOpacity");
    int opacityPct = _wtoi(opacityStr);
    Wh_FreeStringSetting(opacityStr);
    if (opacityPct < 30 || opacityPct > 100) opacityPct = 100;
    g_cfg.idleOpacity = opacityPct;

    g_cfg.edgeHoverExpand = Wh_GetIntSetting(L"EdgeHoverExpand") != 0;
    g_cfg.enableMedia = Wh_GetIntSetting(L"EnableMedia") != 0;
    g_cfg.enableVolume = Wh_GetIntSetting(L"EnableVolume") != 0;
    g_cfg.enableBrightness = Wh_GetIntSetting(L"EnableBrightness") != 0;
    g_cfg.enableBattery = Wh_GetIntSetting(L"EnableBattery") != 0;
    g_cfg.enableBluetooth = Wh_GetIntSetting(L"EnableBluetooth") != 0;
    g_cfg.enableNetwork = Wh_GetIntSetting(L"EnableNetwork") != 0;
    g_cfg.enableNotifications = Wh_GetIntSetting(L"EnableNotifications") != 0;

    PCWSTR spring = Wh_GetStringSetting(L"SpringFeel");
    if (wcscmp(spring, L"bouncy") == 0) g_cfg.springPreset = SPRING_BOUNCY;
    else g_cfg.springPreset = SPRING_NORMAL;
    Wh_FreeStringSetting(spring);
}

bool IsForegroundWindowFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hWnd || !IsWindowVisible(fg) || IsIconic(fg)) return false;

    wchar_t cls[128] = { 0 };
    GetClassNameW(fg, cls, 128);
    if (wcscmp(cls, L"Progman") == 0 ||
        wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }

    RECT rcWin = {};
    if (!GetWindowRect(fg, &rcWin)) return false;

    HMONITOR hMon = MonitorFromPoint({ 10, 10 }, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(hMon, &mi)) return false;

    return (rcWin.left <= mi.rcMonitor.left &&
            rcWin.top <= mi.rcMonitor.top &&
            rcWin.right >= mi.rcMonitor.right &&
            rcWin.bottom >= mi.rcMonitor.bottom);
}

void GetSpringConstants(float& stiffness, float& damping) {
    if (g_cfg.springPreset == SPRING_BOUNCY) {
        stiffness = 0.175f;
        damping = 0.84f;
    } else {
        stiffness = 0.145f;
        damping = 0.77f;
    }
}

bool IsMediaCurrentlyPlaying() {
    if (!g_cfg.enableMedia) return false;
    std::lock_guard<std::mutex> lock(g_mediaMutex);
    return (g_media.hasSession && g_media.isPlaying);
}

bool ShouldShowIdleVisualizer() {
    return IsMediaCurrentlyPlaying() || (g_audioPeak > 0.006f);
}

void ComputeTargetDimensions(IslandViewMode mode, float& outW, float& outH) {
    float s = g_cfg.uiScale;
    switch (mode) {
        case VIEW_EXPANDED_HUB:
            outW = 436.0f * s;
            outH = 152.0f * s;
            break;
        case VIEW_SETTINGS_STUDIO:
            outW = 436.0f * s;
            outH = 204.0f * s;
            break;
        case VIEW_HUD_VOLUME:
        case VIEW_HUD_BRIGHTNESS:
        case VIEW_HUD_BATTERY:
        case VIEW_HUD_BLUETOOTH:
        case VIEW_HUD_NETWORK:
            outW = 360.0f * s;
            outH = 66.0f * s;
            break;
        case VIEW_TOAST_NOTIFICATION:
            outW = 418.0f * s;
            outH = 84.0f * s;
            break;
        case VIEW_IDLE:
        default: {
            if (g_cfg.minimalMode || g_cfg.idleDisplay == IDLE_MINIMAL) {
                outW = 136.0f * s;
                outH = 30.0f * s;
            } else {
                float extra = 0.0f;
                if (ShouldShowIdleVisualizer()) {
                    extra += 30.0f;
                }
                if (!g_shelf.empty()) {
                    extra += 16.0f;
                }
                outW = (g_cfg.baseIdleW + extra) * s;
                outH = g_cfg.baseIdleH * s;
            }
            break;
        }
    }

    if (g_cfg.isNotch) {
        outH += 6.0f * s;
    }
}

void ActivateHudEvent(IslandViewMode mode, const std::wstring& title, const std::wstring& sub, const std::wstring& body, float progress, Color accent, DWORD durationMs, int subState = 0) {
    if (g_isTypingTask) return;

    bool isSameActiveHud = (g_viewMode == mode && g_hud.expireTick > GetTickCount64());
    g_viewMode = mode;
    g_hud.mode = mode;
    g_hud.title = title;
    g_hud.subtitle = sub;
    g_hud.bodyText = body;
    g_hud.progress = std::max(0.0f, std::min(1.0f, progress));
    if (!isSameActiveHud) {
        g_hud.animProgress = g_hud.progress;
        g_oneUiPulsePhase = 0.0f;
    }
    g_hud.accent = accent;
    g_hud.expireTick = GetTickCount64() + durationMs;
    g_hud.subState = subState;

    float tw = 0.0f, th = 0.0f;
    ComputeTargetDimensions(mode, tw, th);
    g_sprW.target = tw;
    g_sprH.target = th;
    if (!isSameActiveHud) {
        g_sprBounceY.velocity = (mode == VIEW_TOAST_NOTIFICATION) ? 2.2f : 1.3f;
        if (mode == VIEW_TOAST_NOTIFICATION) {
            g_sprGlow.velocity = 0.14f;
        }
    }
}

// ============================================================================
// SECTION 7: WINRT SMTC MEDIA & ALBUM ART DECODER
// ============================================================================

bool InitWinRTFunctions() {
    g_hComBase = LoadLibraryW(L"combase.dll");
    if (!g_hComBase) return false;

    pRoGetActivationFactory = (PFN_RoGetActivationFactory)GetProcAddress(g_hComBase, "RoGetActivationFactory");
    pWindowsCreateString = (PFN_WindowsCreateString)GetProcAddress(g_hComBase, "WindowsCreateString");
    pWindowsDeleteString = (PFN_WindowsDeleteString)GetProcAddress(g_hComBase, "WindowsDeleteString");
    pWindowsGetStringRawBuffer = (PFN_WindowsGetStringRawBuffer)GetProcAddress(g_hComBase, "WindowsGetStringRawBuffer");

    g_hShCore = LoadLibraryW(L"shcore.dll");
    if (g_hShCore) {
        pCreateStreamOverRandomAccessStream = (PFN_CreateStreamOverRandomAccessStream)GetProcAddress(g_hShCore, "CreateStreamOverRandomAccessStream");
    }

    return (pRoGetActivationFactory && pWindowsCreateString && pWindowsDeleteString && pWindowsGetStringRawBuffer);
}

std::wstring HStringToWString(HSTRING_LOCAL hstr) {
    if (!hstr || !pWindowsGetStringRawBuffer) return L"";
    UINT32 len = 0;
    PCWSTR raw = pWindowsGetStringRawBuffer(hstr, &len);
    if (!raw || len == 0) return L"";
    return std::wstring(raw, len);
}

template <typename T>
bool WaitForWinRTAsync(IAsyncOperationLocal<T>* pOp, T* pResult, DWORD timeoutMs = 200) {
    if (!pOp || !pResult) return false;
    IAsyncInfoLocal* pInfo = NULL;
    IID iidAsyncInfo = { 0x00000036, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
    if (FAILED(pOp->QueryInterface(iidAsyncInfo, (void**)&pInfo)) || !pInfo) {
        return false;
    }

    ULONGLONG start = GetTickCount64();
    AsyncStatusLocal status = AsyncStatus_Started;
    while (GetTickCount64() - start < timeoutMs) {
        if (FAILED(pInfo->get_Status(&status))) break;
        if (status != AsyncStatus_Started) break;
        Sleep(4);
    }

    bool ok = false;
    if (status == AsyncStatus_Completed) {
        if (SUCCEEDED(pOp->GetResults(pResult))) {
            ok = true;
        }
    }
    pInfo->Close();
    pInfo->Release();
    return ok;
}

Bitmap* DecodeThumbnailToBitmap(IRandomAccessStreamReferenceLocal* pThumbRef) {
    if (!pThumbRef) return NULL;

    IAsyncOperationLocal<IRandomAccessStreamLocal*>* pOpenOp = NULL;
    if (FAILED(pThumbRef->OpenReadAsync(&pOpenOp)) || !pOpenOp) return NULL;

    IRandomAccessStreamLocal* pStream = NULL;
    if (!WaitForWinRTAsync(pOpenOp, &pStream, 300) || !pStream) {
        pOpenOp->Release();
        return NULL;
    }
    pOpenOp->Release();

    Bitmap* resultBmp = NULL;

    if (pCreateStreamOverRandomAccessStream) {
        IStream* pComStream = NULL;
        if (SUCCEEDED(pCreateStreamOverRandomAccessStream(pStream, IID_IStream, (void**)&pComStream)) && pComStream) {
            LARGE_INTEGER zero = {};
            pComStream->Seek(zero, STREAM_SEEK_SET, NULL);
            Bitmap* rawBmp = Bitmap::FromStream(pComStream, FALSE);
            if (rawBmp && rawBmp->GetLastStatus() == Ok && rawBmp->GetWidth() > 0 && rawBmp->GetHeight() > 0) {
                UINT bw = 180U;
                UINT bh = 180U;
                Bitmap* scaled = new Bitmap(bw, bh, PixelFormat32bppARGB);
                Graphics gScale(scaled);
                gScale.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                gScale.SetSmoothingMode(SmoothingModeHighQuality);
                gScale.DrawImage(rawBmp, 0, 0, bw, bh);
                resultBmp = scaled;
            }
            delete rawBmp;
            pComStream->Release();
        }
    }

    pStream->Release();
    return resultBmp;
}

struct WindowMediaFallback {
    bool found;
    std::wstring title;
    std::wstring artist;
    std::wstring appName;
};

BOOL CALLBACK FallbackMediaEnumProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0) return TRUE;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return TRUE;

    wchar_t procPath[MAX_PATH] = { 0 };
    DWORD pathLen = MAX_PATH;
    std::wstring exeName = L"";
    if (QueryFullProcessImageNameW(hProc, 0, procPath, &pathLen)) {
        std::wstring full = procPath;
        size_t slash = full.find_last_of(L"\\/");
        exeName = (slash != std::wstring::npos) ? full.substr(slash + 1) : full;
        for (size_t i = 0; i < exeName.size(); i++) exeName[i] = towlower(exeName[i]);
    }
    CloseHandle(hProc);

    wchar_t titleBuf[512] = { 0 };
    GetWindowTextW(hwnd, titleBuf, 512);
    if (wcslen(titleBuf) < 4) return TRUE;

    std::wstring wTitle = titleBuf;
    WindowMediaFallback* res = (WindowMediaFallback*)lParam;

    if (exeName == L"spotify.exe" && !res->found) {
        if (wTitle != L"Spotify" && wTitle != L"Spotify Free" && wTitle != L"Spotify Premium") {
            res->found = true;
            res->appName = L"Spotify";
            size_t sep = wTitle.find(L" - ");
            if (sep != std::wstring::npos) {
                res->artist = wTitle.substr(0, sep);
                res->title = wTitle.substr(sep + 3);
            } else {
                res->title = wTitle;
                res->artist = L"Spotify";
            }
            return FALSE;
        }
    }

    if (!res->found && (exeName == L"chrome.exe" || exeName == L"msedge.exe" || exeName == L"brave.exe" || exeName == L"firefox.exe" || exeName == L"opera.exe")) {
        const wchar_t* suffixes[] = {
            L" - YouTube", L" - SoundCloud", L" - Apple Music", L" - Spotify", L" - Twitch"
        };
        for (int i = 0; i < 5; i++) {
            size_t pos = wTitle.find(suffixes[i]);
            if (pos != std::wstring::npos && pos > 0) {
                res->found = true;
                res->title = wTitle.substr(0, pos);
                res->artist = suffixes[i] + 3;
                res->appName = (exeName == L"chrome.exe") ? L"Chrome" : (exeName == L"msedge.exe" ? L"Edge" : L"Browser");
                return FALSE;
            }
        }
    }

    return TRUE;
}

void PollSmtcMediaSession(ISMTC_SessionManager* pManager) {
    static std::wstring s_lastTrackKey = L"";
    static int s_artRetryCount = 0;
    bool gotSmtc = false;

    if (pManager) {
        ISMTC_Session* pSession = NULL;
        if (SUCCEEDED(pManager->GetCurrentSession(&pSession)) && pSession) {
            int cmd = g_pendingMediaCommand.exchange(0);
            if (cmd == 1) {
                IAsyncOperationLocal<boolean>* pOp = NULL;
                if (SUCCEEDED(pSession->TryTogglePlayPauseAsync(&pOp)) && pOp) pOp->Release();
            } else if (cmd == 2) {
                IAsyncOperationLocal<boolean>* pOp = NULL;
                if (SUCCEEDED(pSession->TrySkipNextAsync(&pOp)) && pOp) pOp->Release();
            } else if (cmd == 3) {
                IAsyncOperationLocal<boolean>* pOp = NULL;
                if (SUCCEEDED(pSession->TrySkipPreviousAsync(&pOp)) && pOp) pOp->Release();
            }

            INT64 seekTarget = g_pendingSeekTicks.exchange(-1);
            if (seekTarget >= 0) {
                IAsyncOperationLocal<boolean>* pOp = NULL;
                if (SUCCEEDED(pSession->TryChangePlaybackPositionAsync(seekTarget, &pOp)) && pOp) pOp->Release();
            }

            HSTRING_LOCAL hAumid = NULL;
            std::wstring appName = L"Media";
            if (SUCCEEDED(pSession->get_SourceAppUserModelId(&hAumid)) && hAumid) {
                appName = CleanAppDisplayName(HStringToWString(hAumid));
                pWindowsDeleteString(hAumid);
            }

            int playbackStatus = 0;
            ISMTC_PlaybackInfo* pPlayInfo = NULL;
            if (SUCCEEDED(pSession->GetPlaybackInfo(&pPlayInfo)) && pPlayInfo) {
                pPlayInfo->get_PlaybackStatus(&playbackStatus);
                pPlayInfo->Release();
            }

            INT64 posTicks = 0, endTicks = 0;
            ISMTC_TimelineProperties* pTimeInfo = NULL;
            if (SUCCEEDED(pSession->GetTimelineProperties(&pTimeInfo)) && pTimeInfo) {
                pTimeInfo->get_Position(&posTicks);
                pTimeInfo->get_EndTime(&endTicks);
                pTimeInfo->Release();
            }

            IAsyncOperationLocal<ISMTC_MediaProperties*>* pPropsOp = NULL;
            if (SUCCEEDED(pSession->TryGetMediaPropertiesAsync(&pPropsOp)) && pPropsOp) {
                ISMTC_MediaProperties* pProps = NULL;
                if (WaitForWinRTAsync(pPropsOp, &pProps, 160) && pProps) {
                    HSTRING_LOCAL hTitle = NULL, hArtist = NULL, hAlbum = NULL;
                    pProps->get_Title(&hTitle);
                    pProps->get_Artist(&hArtist);
                    pProps->get_AlbumTitle(&hAlbum);

                    std::wstring title = HStringToWString(hTitle);
                    std::wstring artist = HStringToWString(hArtist);
                    std::wstring album = HStringToWString(hAlbum);

                    if (hTitle) pWindowsDeleteString(hTitle);
                    if (hArtist) pWindowsDeleteString(hArtist);
                    if (hAlbum) pWindowsDeleteString(hAlbum);

                    if (!title.empty()) {
                        gotSmtc = true;
                        std::wstring trackKey = appName + L"|" + title + L"|" + artist;
                        bool isNewTrack = (trackKey != s_lastTrackKey);
                        if (isNewTrack) {
                            s_lastTrackKey = trackKey;
                            s_artRetryCount = 0;
                        }

                        bool needArt = isNewTrack;
                        {
                            std::lock_guard<std::mutex> lock(g_mediaMutex);
                            if (!g_media.pAlbumArt && s_artRetryCount < 8) {
                                needArt = true;
                            }
                        }

                        Bitmap* newArt = NULL;
                        Color domColor = GetAccentColorByIndex(g_cfg.accentIdx);

                        if (needArt) {
                            s_artRetryCount++;
                            IRandomAccessStreamReferenceLocal* pThumbRef = NULL;
                            if (SUCCEEDED(pProps->get_Thumbnail(&pThumbRef)) && pThumbRef) {
                                newArt = DecodeThumbnailToBitmap(pThumbRef);
                                pThumbRef->Release();
                            }
                            if (newArt) {
                                domColor = ExtractDominantColor(newArt);
                            }
                        }

                        std::lock_guard<std::mutex> lock(g_mediaMutex);
                        g_media.hasSession = true;
                        g_media.isPlaying = (playbackStatus == 4);
                        g_media.title = title;
                        g_media.artist = artist.empty() ? appName : artist;
                        g_media.album = album;
                        g_media.sourceApp = appName;
                        g_media.posTicks = posTicks;
                        g_media.endTicks = endTicks;
                        if (endTicks > 0) {
                            g_media.progress = std::max(0.0f, std::min(1.0f, (float)((double)posTicks / (double)endTicks)));
                        } else {
                            g_media.progress = (g_media.isPlaying) ? fmod(g_media.progress + 0.004f, 1.0f) : g_media.progress;
                        }

                        if (isNewTrack) {
                            if (g_media.pAlbumArt) {
                                delete g_media.pAlbumArt;
                                g_media.pAlbumArt = NULL;
                            }
                            if (newArt) {
                                g_media.pAlbumArt = newArt;
                                g_media.dominantColor = domColor;
                            }
                        } else if (newArt) {
                            if (g_media.pAlbumArt) delete g_media.pAlbumArt;
                            g_media.pAlbumArt = newArt;
                            g_media.dominantColor = domColor;
                        }
                    }
                    pProps->Release();
                }
                pPropsOp->Release();
            }
            pSession->Release();
        }
    }

    if (!gotSmtc) {
        WindowMediaFallback fb = { false, L"", L"", L"" };
        EnumWindows(FallbackMediaEnumProc, (LPARAM)&fb);
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        if (fb.found) {
            g_media.hasSession = true;
            g_media.isPlaying = (g_audioPeak > 0.008f);
            g_media.title = fb.title;
            g_media.artist = fb.artist;
            g_media.sourceApp = fb.appName;
            if (g_media.isPlaying) {
                g_media.progress = fmod(g_media.progress + 0.005f, 1.0f);
            }
        } else if (g_audioPeak <= 0.005f) {
            g_media.hasSession = false;
            g_media.isPlaying = false;
            s_lastTrackKey = L"";
        }
    }
}

// ============================================================================
// SECTION 8: HARDWARE TELEMETRY (WMI BRIGHTNESS, BLUETOOTH, AUDIO, TOASTS)
// ============================================================================

void InitBluetoothApi() {
    g_hBthProps = LoadLibraryW(L"bthprops.cpl");
    if (!g_hBthProps) {
        g_hBthProps = LoadLibraryW(L"irprops.cpl");
    }
    if (g_hBthProps) {
        pBluetoothFindFirstDevice = (PFN_BluetoothFindFirstDevice)GetProcAddress(g_hBthProps, "BluetoothFindFirstDevice");
        pBluetoothFindNextDevice = (PFN_BluetoothFindNextDevice)GetProcAddress(g_hBthProps, "BluetoothFindNextDevice");
        pBluetoothFindDeviceClose = (PFN_BluetoothFindDeviceClose)GetProcAddress(g_hBthProps, "BluetoothFindDeviceClose");
    }
}

void PollBluetoothDevices() {
    if (!pBluetoothFindFirstDevice || !pBluetoothFindNextDevice || !pBluetoothFindDeviceClose) return;

    BLUETOOTH_DEVICE_SEARCH_PARAMS_LOCAL sp = {};
    sp.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS_LOCAL);
    sp.fReturnAuthenticated = TRUE;
    sp.fReturnRemembered = TRUE;
    sp.fReturnConnected = TRUE;
    sp.fReturnUnknown = FALSE;
    sp.fIssueInquiry = FALSE;
    sp.cTimeoutMultiplier = 1;
    sp.hRadio = NULL;

    BLUETOOTH_DEVICE_INFO_LOCAL bdi = {};
    bdi.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_LOCAL);

    std::vector<ConnectedBtDevice> currentList;
    HANDLE hFind = pBluetoothFindFirstDevice(&sp, &bdi);
    if (hFind) {
        do {
            if (bdi.fConnected) {
                ConnectedBtDevice dev;
                dev.addr = bdi.Address.ullLong;
                dev.name = (wcslen(bdi.szName) > 0) ? bdi.szName : L"Bluetooth Device";
                currentList.push_back(dev);
            }
            ZeroMemory(&bdi, sizeof(bdi));
            bdi.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_LOCAL);
        } while (pBluetoothFindNextDevice(hFind, &bdi));
        pBluetoothFindDeviceClose(hFind);
    }

    if (!g_btInitialized) {
        g_lastBtDevices = currentList;
        g_btInitialized = true;
        return;
    }

    for (size_t i = 0; i < currentList.size(); i++) {
        bool existed = false;
        for (size_t j = 0; j < g_lastBtDevices.size(); j++) {
            if (g_lastBtDevices[j].addr == currentList[i].addr) {
                existed = true;
                break;
            }
        }
        if (!existed) {
            std::wstring* pName = new std::wstring(currentList[i].name);
            PostMessageW(g_hWnd, WM_APP + 6, 1, (LPARAM)pName);
        }
    }

    for (size_t i = 0; i < g_lastBtDevices.size(); i++) {
        bool stillConnected = false;
        for (size_t j = 0; j < currentList.size(); j++) {
            if (currentList[j].addr == g_lastBtDevices[i].addr) {
                stillConnected = true;
                break;
            }
        }
        if (!stillConnected) {
            std::wstring* pName = new std::wstring(g_lastBtDevices[i].name);
            PostMessageW(g_hWnd, WM_APP + 6, 0, (LPARAM)pName);
        }
    }

    g_lastBtDevices = currentList;
}

void InitWmiBrightness() {
    IWbemLocator* pLoc = NULL;
    if (SUCCEEDED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc)) && pLoc) {
        BSTR ns = SysAllocString(L"ROOT\\WMI");
        if (SUCCEEDED(pLoc->ConnectServer(ns, NULL, NULL, 0, 0, 0, 0, &g_pWbemServices)) && g_pWbemServices) {
            CoSetProxyBlanket(g_pWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                              RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
        }
        SysFreeString(ns);
        pLoc->Release();
    }
}

int ReadCurrentBrightnessPercent() {
    if (g_pWbemServices) {
        BSTR lang = SysAllocString(L"WQL");
        BSTR query = SysAllocString(L"SELECT CurrentBrightness FROM WmiMonitorBrightness");
        IEnumWbemClassObject* pEnum = NULL;
        int result = -1;

        if (SUCCEEDED(g_pWbemServices->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnum)) && pEnum) {
            IWbemClassObject* pObj = NULL;
            ULONG returned = 0;
            if (SUCCEEDED(pEnum->Next(50, 1, &pObj, &returned)) && returned > 0 && pObj) {
                VARIANT vtProp;
                VariantInit(&vtProp);
                if (SUCCEEDED(pObj->Get(L"CurrentBrightness", 0, &vtProp, 0, 0))) {
                    if (vtProp.vt == VT_UI1) result = (int)vtProp.bVal;
                    else if (vtProp.vt == VT_I4) result = (int)vtProp.lVal;
                    else if (vtProp.vt == VT_I2) result = (int)vtProp.iVal;
                }
                VariantClear(&vtProp);
                pObj->Release();
            }
            pEnum->Release();
        }
        SysFreeString(query);
        SysFreeString(lang);
        if (result >= 0 && result <= 100) return result;
    }

    if (g_hPhysMonitor) {
        DWORD minB = 0, curB = 0, maxB = 100;
        if (GetMonitorBrightness(g_hPhysMonitor, &minB, &curB, &maxB) && maxB > minB) {
            return (int)std::round(((double)(curB - minB) / (double)(maxB - minB)) * 100.0);
        }
    }

    return -1;
}

bool SetBrightnessPercent(int targetPct) {
    targetPct = std::max(0, std::min(100, targetPct));
    if (g_hPhysMonitor) {
        DWORD minB = 0, curB = 0, maxB = 100;
        if (GetMonitorBrightness(g_hPhysMonitor, &minB, &curB, &maxB) && maxB > minB) {
            DWORD val = minB + (DWORD)std::round(((double)targetPct / 100.0) * (double)(maxB - minB));
            if (SetMonitorBrightness(g_hPhysMonitor, val)) {
                return true;
            }
        }
    }
    return false;
}

struct ToastScanContext {
    bool found;
    HWND hwnd;
    std::wstring appTitle;
    std::wstring messageBody;
};

BOOL CALLBACK ScanToastWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;

    wchar_t cls[128] = { 0 };
    GetClassNameW(hwnd, cls, 128);
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") != 0) return TRUE;

    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_TOPMOST)) return TRUE;

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) return TRUE;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w < 280 || w > 520 || h < 60 || h > 360) return TRUE;

    ToastScanContext* ctx = (ToastScanContext*)lParam;
    ctx->found = true;
    ctx->hwnd = hwnd;

    wchar_t winTitle[256] = { 0 };
    GetWindowTextW(hwnd, winTitle, 256);
    ctx->appTitle = (wcslen(winTitle) > 0) ? winTitle : L"Notification";

    if (g_pUIAuto) {
        IMyUIAutomationElement* pRoot = NULL;
        if (SUCCEEDED(g_pUIAuto->ElementFromHandle(hwnd, &pRoot)) && pRoot) {
            IUnknown* pCond = NULL;
            if (SUCCEEDED(g_pUIAuto->CreateTrueCondition(&pCond)) && pCond) {
                IUnknown* pArrUnk = NULL;
                if (SUCCEEDED(pRoot->FindAll(4, pCond, &pArrUnk)) && pArrUnk) {
                    IID iidArr = { 0x14314595, 0xB4BC, 0x4055, { 0x95, 0xF2, 0x58, 0xF2, 0xE4, 0x2C, 0x98, 0x55 } };
                    IMyUIAutomationElementArray* pArr = NULL;
                    if (SUCCEEDED(pArrUnk->QueryInterface(iidArr, (void**)&pArr)) && pArr) {
                        int count = 0;
                        pArr->get_Length(&count);
                        std::vector<std::wstring> lines;
                        for (int i = 0; i < std::min(count, 12); i++) {
                            IMyUIAutomationElement* pEl = NULL;
                            if (SUCCEEDED(pArr->GetElement(i, &pEl)) && pEl) {
                                BSTR bName = NULL;
                                if (SUCCEEDED(pEl->get_CurrentName(&bName)) && bName) {
                                    std::wstring line = bName;
                                    SysFreeString(bName);
                                    if (line.size() > 1 && line != L"Close" && line != L"Dismiss" && line != L"Settings") {
                                        if (std::find(lines.begin(), lines.end(), line) == lines.end()) {
                                            lines.push_back(line);
                                        }
                                    }
                                }
                                pEl->Release();
                            }
                        }
                        if (!lines.empty()) ctx->appTitle = lines[0];
                        if (lines.size() >= 2) {
                            ctx->messageBody = lines[1];
                            if (lines.size() >= 3) ctx->messageBody += L" — " + lines[2];
                        }
                        pArr->Release();
                    }
                    pArrUnk->Release();
                }
                pCond->Release();
            }
            pRoot->Release();
        }
    }

    return FALSE;
}

DWORD WINAPI BackgroundTelemetryWorker(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    IMMDeviceEnumerator* pEnum = NULL;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum)) && pEnum) {
        IMMDevice* pDev = NULL;
        if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDev)) && pDev) {
            pDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&g_pVolume);
            pDev->Activate(IID_IMyAudioMeterInformation, CLSCTX_ALL, NULL, (void**)&g_pMeter);
            pDev->Release();
        }
        pEnum->Release();
    }

    InitWmiBrightness();
    InitBluetoothApi();

    HMONITOR hMon = MonitorFromPoint({ 10, 10 }, MONITOR_DEFAULTTOPRIMARY);
    DWORD numMons = 0;
    if (GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &numMons) && numMons > 0) {
        PHYSICAL_MONITOR* pMons = new PHYSICAL_MONITOR[numMons];
        if (GetPhysicalMonitorsFromHMONITOR(hMon, numMons, pMons)) {
            g_hPhysMonitor = pMons[0].hPhysicalMonitor;
        }
        delete[] pMons;
    }

    CoCreateInstance(CLSID_MyCUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IMyUIAutomation, (void**)&g_pUIAuto);

    ISMTC_SessionManager* pSmtcManager = NULL;
    if (InitWinRTFunctions()) {
        HSTRING_LOCAL hClass = NULL;
        const wchar_t* clsName = L"Windows.Media.Control.GlobalSystemMediaTransportControlsSessionManager";
        if (SUCCEEDED(pWindowsCreateString(clsName, (UINT32)wcslen(clsName), &hClass)) && hClass) {
            IID iidStatics = { 0x2050C4EE, 0x11A0, 0x57DE, { 0xAE, 0xD7, 0xC9, 0x7C, 0x70, 0x33, 0x82, 0x45 } };
            ISMTC_SessionManagerStatics* pStatics = NULL;
            if (SUCCEEDED(pRoGetActivationFactory(hClass, iidStatics, (void**)&pStatics)) && pStatics) {
                IAsyncOperationLocal<ISMTC_SessionManager*>* pReqOp = NULL;
                if (SUCCEEDED(pStatics->RequestAsync(&pReqOp)) && pReqOp) {
                    WaitForWinRTAsync(pReqOp, &pSmtcManager, 500);
                    pReqOp->Release();
                }
                pStatics->Release();
            }
            pWindowsDeleteString(hClass);
        }
    }

    int tickCounter = 0;
    bool wasVolDownPressed = false;
    bool wasVolUpPressed = false;
    bool wasBriDownPressed = false;
    bool wasBriUpPressed = false;

    while (g_running.load()) {
        if (g_pMeter) {
            float peak = 0.0f;
            if (SUCCEEDED(g_pMeter->GetPeakValue(&peak))) {
                g_audioPeak = peak;
            }
        }

        if (g_cfg.enableVolume) {
            bool volDownNow = (GetAsyncKeyState(VK_VOLUME_DOWN) & 0x8000) != 0;
            bool volUpNow = (GetAsyncKeyState(VK_VOLUME_UP) & 0x8000) != 0;

            if (volDownNow && !wasVolDownPressed) {
                if (g_lastMute || (g_lastVolume >= 0.0f && g_lastVolume <= 0.005f)) {
                    PostMessageW(g_hWnd, WM_APP + 1, 3, 0);
                }
            }
            if (volUpNow && !wasVolUpPressed) {
                if (!g_lastMute && g_lastVolume >= 0.995f) {
                    PostMessageW(g_hWnd, WM_APP + 1, 4, 100);
                }
            }

            wasVolDownPressed = volDownNow;
            wasVolUpPressed = volUpNow;
        }

        if (g_cfg.enableBrightness) {
            bool briDownNow = (GetAsyncKeyState(0xE2) & 0x8000) != 0;
            bool briUpNow = (GetAsyncKeyState(0xE3) & 0x8000) != 0;

            if (briDownNow && !wasBriDownPressed && g_lastBrightness == 0) {
                PostMessageW(g_hWnd, WM_APP + 2, 1, 0);
            }
            if (briUpNow && !wasBriUpPressed && g_lastBrightness == 100) {
                PostMessageW(g_hWnd, WM_APP + 2, 2, 100);
            }

            wasBriDownPressed = briDownNow;
            wasBriUpPressed = briUpNow;
        }

        if (g_cfg.enableVolume && g_pVolume && (tickCounter % 2 == 0)) {
            float vol = 0.0f;
            BOOL mute = FALSE;
            if (SUCCEEDED(g_pVolume->GetMasterVolumeLevelScalar(&vol)) && SUCCEEDED(g_pVolume->GetMute(&mute))) {
                if (g_lastVolume >= 0.0f && (std::abs(vol - g_lastVolume) > 0.005f || mute != g_lastMute)) {
                    int pct = (int)std::round(vol * 100.0f);
                    if (mute || pct == 0) PostMessageW(g_hWnd, WM_APP + 1, 1, 0);
                    else PostMessageW(g_hWnd, WM_APP + 1, 2, (LPARAM)pct);
                }
                g_lastVolume = vol;
                g_lastMute = mute;
            }
        }

        if (g_cfg.enableBrightness && (tickCounter % 3 == 0)) {
            int bri = ReadCurrentBrightnessPercent();
            if (bri >= 0) {
                if (g_lastBrightness >= 0 && bri != g_lastBrightness) {
                    PostMessageW(g_hWnd, WM_APP + 2, 0, (LPARAM)bri);
                }
                g_lastBrightness = bri;
            }
        }

        if (g_cfg.enableMedia && (tickCounter % 5 == 0)) {
            PollSmtcMediaSession(pSmtcManager);
        }

        if (tickCounter % 7 == 0) {
            if (g_cfg.enableBattery) {
                SYSTEM_POWER_STATUS sps;
                if (GetSystemPowerStatus(&sps)) {
                    int pct = (sps.BatteryLifePercent <= 100) ? (int)sps.BatteryLifePercent : 100;
                    if (g_lastACStatus != 255 && g_lastACStatus != sps.ACLineStatus) {
                        PostMessageW(g_hWnd, WM_APP + 3, (WPARAM)sps.ACLineStatus, (LPARAM)pct);
                    } else if (sps.ACLineStatus == 1 && pct == 100 && g_lastBatPct != 100 && g_lastBatPct != -1) {
                        PostMessageW(g_hWnd, WM_APP + 3, 2, 100);
                    } else if (sps.ACLineStatus == 0 && pct == 20 && g_lastBatPct > 20) {
                        PostMessageW(g_hWnd, WM_APP + 3, 3, 20);
                    }
                    g_lastACStatus = sps.ACLineStatus;
                    g_lastBatPct = pct;
                }
            }

            if (g_cfg.enableNetwork) {
                DWORD flags = 0;
                bool connected = InternetGetConnectedState(&flags, 0);
                if (connected != g_lastNetConnected) {
                    PostMessageW(g_hWnd, WM_APP + 4, connected ? 1 : 0, 0);
                    g_lastNetConnected = connected;
                }
            }

            if (g_cfg.enableNotifications) {
                ToastScanContext toastCtx = { false, NULL, L"", L"" };
                EnumWindows(ScanToastWindowsProc, (LPARAM)&toastCtx);
                if (toastCtx.found) {
                    std::wstring sig = toastCtx.appTitle + L"|" + toastCtx.messageBody;
                    if (toastCtx.hwnd != g_lastToastHwnd || sig != g_lastToastSignature) {
                        g_lastToastHwnd = toastCtx.hwnd;
                        g_lastToastSignature = sig;
                        std::wstring* pPack = new std::wstring(toastCtx.appTitle + L"\n" + toastCtx.messageBody);
                        PostMessageW(g_hWnd, WM_APP + 5, 0, (LPARAM)pPack);
                    }
                } else {
                    g_lastToastHwnd = NULL;
                }
            }
        }

        if (g_cfg.enableBluetooth && (tickCounter % 18 == 0)) {
            PollBluetoothDevices();
        }

        tickCounter++;
        Sleep(48);
    }

    if (pSmtcManager) pSmtcManager->Release();
    if (g_pUIAuto) g_pUIAuto->Release();
    if (g_hPhysMonitor) DestroyPhysicalMonitor(g_hPhysMonitor);
    if (g_pWbemServices) g_pWbemServices->Release();
    if (g_pMeter) g_pMeter->Release();
    if (g_pVolume) g_pVolume->Release();
    if (g_hBthProps) FreeLibrary(g_hBthProps);
    if (g_hShCore) FreeLibrary(g_hShCore);
    if (g_hComBase) FreeLibrary(g_hComBase);

    CoUninitialize();
    return 0;
}

// ============================================================================
// SECTION 9: CUSTOM APPLE & ONE UI 9.0 VECTOR SHAPE & WAVE RENDERER
// ============================================================================

void BuildIslandGeometry(GraphicsPath& path, float x, float y, float w, float h, bool isNotch) {
    if (isNotch) {
        float topY = y - 2.0f;
        float earW = std::min(16.0f * g_cfg.uiScale, h * 0.36f);
        float earH = std::min(14.0f * g_cfg.uiScale, h * 0.34f);
        float botR = std::min(h * 0.46f, 24.0f * g_cfg.uiScale);

        if (earH + botR > h * 0.96f) {
            float scale = (h * 0.96f) / (earH + botR);
            earH *= scale;
            botR *= scale;
        }
        if (botR * 2.0f > w) botR = w * 0.5f;

        path.AddLine(x - earW, topY, x + w + earW, topY);
        path.AddLine(x + w + earW, topY, x + w + earW, y);
        path.AddBezier(
            x + w + earW, y,
            x + w + earW * 0.35f, y,
            x + w, y + earH * 0.35f,
            x + w, y + earH
        );
        path.AddLine(x + w, y + earH, x + w, y + h - botR);
        path.AddArc(x + w - botR * 2.0f, y + h - botR * 2.0f, botR * 2.0f, botR * 2.0f, 0.0f, 90.0f);
        path.AddLine(x + w - botR, y + h, x + botR, y + h);
        path.AddArc(x, y + h - botR * 2.0f, botR * 2.0f, botR * 2.0f, 90.0f, 90.0f);
        path.AddLine(x, y + h - botR, x, y + earH);
        path.AddBezier(
            x, y + earH,
            x, y + earH * 0.35f,
            x - earW * 0.35f, y,
            x - earW, y
        );
        path.AddLine(x - earW, y, x - earW, topY);
        path.CloseFigure();
    } else {
        float compactLimit = (g_cfg.baseIdleH + 8.0f) * g_cfg.uiScale;
        float r = (h <= compactLimit) ? (h / 2.0f) : std::min(h * 0.38f, 34.0f * g_cfg.uiScale);
        if (r * 2.0f > w) r = w / 2.0f;

        path.AddArc(x, y, r * 2.0f, r * 2.0f, 180.0f, 90.0f);
        path.AddArc(x + w - r * 2.0f, y, r * 2.0f, r * 2.0f, 270.0f, 90.0f);
        path.AddArc(x + w - r * 2.0f, y + h - r * 2.0f, r * 2.0f, r * 2.0f, 0.0f, 90.0f);
        path.AddArc(x, y + h - r * 2.0f, r * 2.0f, r * 2.0f, 90.0f, 90.0f);
        path.CloseFigure();
    }
}

void BuildPillRect(GraphicsPath& path, float x, float y, float w, float h, float radius) {
    float r = std::min(radius, std::min(w, h) / 2.0f);
    if (r <= 0.5f) {
        path.AddRectangle(RectF(x, y, w, h));
        return;
    }
    path.AddArc(x, y, r * 2.0f, r * 2.0f, 180.0f, 90.0f);
    path.AddArc(x + w - r * 2.0f, y, r * 2.0f, r * 2.0f, 270.0f, 90.0f);
    path.AddArc(x + w - r * 2.0f, y + h - r * 2.0f, r * 2.0f, r * 2.0f, 0.0f, 90.0f);
    path.AddArc(x, y + h - r * 2.0f, r * 2.0f, r * 2.0f, 90.0f, 90.0f);
    path.CloseFigure();
}

void DrawCircularBatteryDonut(Graphics& g, float cx, float cy, float outerRadius, float thickness, float pct, bool charging, Color col, BYTE alpha) {
    float d = (outerRadius - thickness * 0.5f) * 2.0f;
    float left = cx - (outerRadius - thickness * 0.5f);
    float top = cy - (outerRadius - thickness * 0.5f);

    Pen trackPen(Color((BYTE)(alpha * 0.22f), 255, 255, 255), thickness);
    trackPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    g.DrawEllipse(&trackPen, left, top, d, d);

    float clamped = std::max(0.02f, std::min(1.0f, pct));
    float sweep = clamped * 360.0f;

    if (charging) {
        float pulse = 0.5f + 0.5f * sinf(g_oneUiPulsePhase * 2.2f);
        Pen glowPen(Color((BYTE)(alpha * (0.20f + 0.25f * pulse)), col.GetR(), col.GetG(), col.GetB()), thickness + 3.0f);
        g.DrawArc(&glowPen, left, top, d, d, -90.0f, sweep);
    }

    Pen arcPen(Color(alpha, col.GetR(), col.GetG(), col.GetB()), thickness);
    arcPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    g.DrawArc(&arcPen, left, top, d, d, -90.0f, sweep);
}

void DrawOneUi9WavyProgressBar(Graphics& g, float x, float y, float w, float h, float progress, Color waveCol, BYTE alpha) {
    float clamped = std::max(0.0f, std::min(1.0f, progress));
    float splitX = x + w * clamped;
    float centerY = y + h * 0.5f;

    if (splitX < x + w - 2.0f) {
        Pen trackPen(Color((BYTE)(alpha * 0.24f), 255, 255, 255), h * 0.68f);
        trackPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        g.DrawLine(&trackPen, std::max(x, splitX + 3.0f), centerY, x + w, centerY);
    }

    float playedW = splitX - x;
    if (playedW > 2.0f) {
        float amp = (h * 0.62f) * g_waveAmplitude;
        float freq = 0.28f / std::max(0.7f, g_cfg.uiScale);
        int steps = std::max(4, (int)(playedW / 2.0f));

        std::vector<PointF> pts;
        pts.reserve(steps + 1);
        for (int i = 0; i <= steps; i++) {
            float t = (float)i / (float)steps;
            float px = x + t * playedW;
            float edgeTaper = std::min(1.0f, std::min(t * 6.0f, (1.0f - t) * 6.0f));
            float py = centerY + sinf((px - x) * freq - g_wavePhase) * amp * edgeTaper;
            pts.push_back(PointF(px, py));
        }

        Pen wavePen(Color(alpha, waveCol.GetR(), waveCol.GetG(), waveCol.GetB()), h * 0.82f);
        wavePen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        wavePen.SetLineJoin(LineJoinRound);
        g.DrawCurve(&wavePen, pts.data(), (int)pts.size(), 0.45f);
    }

    float thumbR = h * 0.92f;
    SolidBrush thumbBr(Color(alpha, 255, 255, 255));
    g.FillEllipse(&thumbBr, splitX - thumbR, centerY - thumbR, thumbR * 2.0f, thumbR * 2.0f);
}

void DrawCapsuleProgressBar(Graphics& g, float x, float y, float w, float h, float progress, Color fillCol, BYTE alpha) {
    GraphicsPath track;
    BuildPillRect(track, x, y, w, h, h / 2.0f);
    SolidBrush trackBrush(Color((BYTE)(alpha * 0.22f), 255, 255, 255));
    g.FillPath(&trackBrush, &track);

    float clamped = std::max(0.0f, std::min(1.0f, progress));
    if (clamped > 0.005f) {
        float fw = std::max(h, w * clamped);
        GraphicsPath fill;
        BuildPillRect(fill, x, y, fw, h, h / 2.0f);
        SolidBrush fillBrush(Color(alpha, fillCol.GetR(), fillCol.GetG(), fillCol.GetB()));
        g.FillPath(&fillBrush, &fill);
    }
}

void DrawVectorSpeakerIcon(Graphics& g, float x, float y, float sz, float level, Color col, BYTE alpha) {
    Color c(alpha, col.GetR(), col.GetG(), col.GetB());
    SolidBrush br(c);
    Pen pen(c, sz * 0.09f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);

    float recoilNudge = g_sprBounceX.current * 0.28f;
    float sx = x + recoilNudge;

    PointF cone[6] = {
        PointF(sx + sz * 0.12f, y + sz * 0.36f),
        PointF(sx + sz * 0.30f, y + sz * 0.36f),
        PointF(sx + sz * 0.52f, y + sz * 0.18f),
        PointF(sx + sz * 0.52f, y + sz * 0.82f),
        PointF(sx + sz * 0.30f, y + sz * 0.64f),
        PointF(sx + sz * 0.12f, y + sz * 0.64f)
    };
    g.FillPolygon(&br, cone, 6);

    float wavePulse = 1.0f + 0.08f * sinf(g_oneUiPulsePhase * 3.0f);
    if (level <= 0.005f) {
        Pen mutePen(Color(alpha, 255, 69, 58), sz * 0.10f);
        mutePen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        g.DrawLine(&mutePen, sx + sz * 0.64f, y + sz * 0.36f, sx + sz * 0.88f, y + sz * 0.64f);
        g.DrawLine(&mutePen, sx + sz * 0.88f, y + sz * 0.36f, sx + sz * 0.64f, y + sz * 0.64f);
    } else {
        g.DrawArc(&pen, sx + sz * 0.44f, y + sz * 0.30f, sz * 0.28f * wavePulse, sz * 0.40f, -55.0f, 110.0f);
        if (level > 0.48f) {
            g.DrawArc(&pen, sx + sz * 0.44f, y + sz * 0.16f, sz * 0.48f * wavePulse, sz * 0.68f, -55.0f, 110.0f);
        }
    }
}

void DrawVectorSunIcon(Graphics& g, float x, float y, float sz, float level, Color col, BYTE alpha) {
    Color c(alpha, col.GetR(), col.GetG(), col.GetB());
    SolidBrush br(c);
    Pen pen(c, sz * 0.09f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);

    float recoilNudge = g_sprBounceX.current * 0.28f;
    float cx = x + sz * 0.5f + recoilNudge;
    float cy = y + sz * 0.5f;
    float coreR = sz * (0.18f + 0.04f * level);
    g.FillEllipse(&br, cx - coreR, cy - coreR, coreR * 2.0f, coreR * 2.0f);

    float rIn = sz * 0.29f;
    float rOut = sz * (0.37f + 0.10f * level);
    float rot = level * 1.2f;
    for (int i = 0; i < 8; i++) {
        float ang = rot + i * 3.14159265f / 4.0f;
        g.DrawLine(&pen, cx + cosf(ang) * rIn, cy + sinf(ang) * rIn,
                         cx + cosf(ang) * rOut, cy + sinf(ang) * rOut);
    }
}

void DrawOneUiBatteryHudIcon(Graphics& g, float x, float y, float sz, float level, bool charging, Color col, BYTE alpha) {
    float cx = x + sz * 0.5f;
    float cy = y + sz * 0.5f;
    float r = sz * 0.44f;

    if (charging) {
        float wave = fmod(g_oneUiPulsePhase * 0.45f, 1.0f);
        float rippleR = r * (0.85f + wave * 0.55f);
        BYTE rippleA = (BYTE)(alpha * (1.0f - wave) * 0.55f);
        Pen ripplePen(Color(rippleA, col.GetR(), col.GetG(), col.GetB()), 2.0f);
        g.DrawEllipse(&ripplePen, cx - rippleR, cy - rippleR, rippleR * 2.0f, rippleR * 2.0f);
    }

    DrawCircularBatteryDonut(g, cx, cy, r, sz * 0.13f, level, charging, col, alpha);

    SolidBrush boltBr(Color(alpha, col.GetR(), col.GetG(), col.GetB()));
    if (charging) {
        PointF bolt[6] = {
            PointF(cx + sz * 0.04f, cy - sz * 0.20f),
            PointF(cx - sz * 0.11f, cy + sz * 0.02f),
            PointF(cx - sz * 0.01f, cy + sz * 0.02f),
            PointF(cx - sz * 0.04f, cy + sz * 0.20f),
            PointF(cx + sz * 0.11f, cy - sz * 0.02f),
            PointF(cx + sz * 0.01f, cy - sz * 0.02f)
        };
        g.FillPolygon(&boltBr, bolt, 6);
    } else {
        float dotR = sz * 0.10f;
        g.FillEllipse(&boltBr, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
    }
}

void DrawVectorBluetoothIcon(Graphics& g, float x, float y, float sz, bool connected, Color col, BYTE alpha) {
    float cx = x + sz * 0.5f;
    float cy = y + sz * 0.5f;

    if (connected) {
        float wave = fmod(g_oneUiPulsePhase * 0.55f, 1.0f);
        float r = sz * (0.35f + wave * 0.35f);
        BYTE ra = (BYTE)(alpha * (1.0f - wave) * 0.65f);
        Pen wavePen(Color(ra, col.GetR(), col.GetG(), col.GetB()), 1.8f);
        g.DrawEllipse(&wavePen, cx - r, cy - r, r * 2.0f, r * 2.0f);
    }

    SolidBrush circleBg(Color((BYTE)(alpha * 0.22f), col.GetR(), col.GetG(), col.GetB()));
    g.FillEllipse(&circleBg, x, y, sz, sz);

    Pen pen(Color(alpha, col.GetR(), col.GetG(), col.GetB()), sz * 0.085f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    pen.SetLineJoin(LineJoinRound);

    PointF rune[6] = {
        PointF(cx - sz * 0.14f, cy - sz * 0.13f),
        PointF(cx + sz * 0.14f, cy + sz * 0.13f),
        PointF(cx, cy + sz * 0.26f),
        PointF(cx, cy - sz * 0.26f),
        PointF(cx + sz * 0.14f, cy - sz * 0.13f),
        PointF(cx - sz * 0.14f, cy + sz * 0.13f)
    };
    g.DrawLines(&pen, rune, 6);
}

void DrawVectorWifiIcon(Graphics& g, float x, float y, float sz, bool online, Color col, BYTE alpha) {
    Color c(alpha, col.GetR(), col.GetG(), col.GetB());
    Pen pen(c, sz * 0.09f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    SolidBrush br(c);

    g.DrawArc(&pen, x + sz * 0.10f, y + sz * 0.22f, sz * 0.80f, sz * 0.80f, 225.0f, 90.0f);
    g.DrawArc(&pen, x + sz * 0.26f, y + sz * 0.38f, sz * 0.48f, sz * 0.48f, 225.0f, 90.0f);
    g.FillEllipse(&br, x + sz * 0.43f, y + sz * 0.66f, sz * 0.14f, sz * 0.14f);

    if (!online) {
        Pen slash(Color(alpha, 255, 69, 58), sz * 0.09f);
        slash.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        g.DrawLine(&slash, x + sz * 0.20f, y + sz * 0.20f, x + sz * 0.80f, y + sz * 0.80f);
    }
}

void DrawVectorBellIcon(Graphics& g, float x, float y, float sz, Color col, BYTE alpha) {
    Color c(alpha, col.GetR(), col.GetG(), col.GetB());
    SolidBrush br(c);
    g.FillPie(&br, x + sz * 0.22f, y + sz * 0.18f, sz * 0.56f, sz * 0.56f, 180.0f, 180.0f);
    g.FillRectangle(&br, x + sz * 0.22f, y + sz * 0.44f, sz * 0.56f, sz * 0.24f);
    GraphicsPath base;
    BuildPillRect(base, x + sz * 0.14f, y + sz * 0.64f, sz * 0.72f, sz * 0.10f, sz * 0.05f);
    g.FillPath(&br, &base);
    g.FillEllipse(&br, x + sz * 0.41f, y + sz * 0.75f, sz * 0.18f, sz * 0.14f);
}

void DrawVectorGearIcon(Graphics& g, float cx, float cy, float r, Color col, BYTE alpha) {
    Color c(alpha, col.GetR(), col.GetG(), col.GetB());
    Pen pen(c, r * 0.26f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    g.DrawEllipse(&pen, cx - r * 0.55f, cy - r * 0.55f, r * 1.1f, r * 1.1f);
    for (int i = 0; i < 8; i++) {
        float a = i * 3.14159265f / 4.0f;
        g.DrawLine(&pen, cx + cosf(a) * r * 0.62f, cy + sinf(a) * r * 0.62f,
                         cx + cosf(a) * r * 0.88f, cy + sinf(a) * r * 0.88f);
    }
}

void DrawMediaGlyph(Graphics& g, int glyphType, float cx, float cy, float sz, Color col, BYTE alpha) {
    Color c(alpha, col.GetR(), col.GetG(), col.GetB());
    SolidBrush br(c);

    if (glyphType == 1) {
        PointF tri[3] = {
            PointF(cx - sz * 0.32f, cy - sz * 0.44f),
            PointF(cx + sz * 0.46f, cy),
            PointF(cx - sz * 0.32f, cy + sz * 0.44f)
        };
        g.FillPolygon(&br, tri, 3);
    } else if (glyphType == 2) {
        float bw = sz * 0.24f;
        float bh = sz * 0.82f;
        GraphicsPath b1, b2;
        BuildPillRect(b1, cx - sz * 0.34f, cy - bh * 0.5f, bw, bh, bw * 0.35f);
        BuildPillRect(b2, cx + sz * 0.10f, cy - bh * 0.5f, bw, bh, bw * 0.35f);
        g.FillPath(&br, &b1);
        g.FillPath(&br, &b2);
    } else if (glyphType == 3) {
        PointF tri1[3] = {
            PointF(cx - sz * 0.44f, cy - sz * 0.36f),
            PointF(cx + sz * 0.04f, cy),
            PointF(cx - sz * 0.44f, cy + sz * 0.36f)
        };
        PointF tri2[3] = {
            PointF(cx - sz * 0.02f, cy - sz * 0.36f),
            PointF(cx + sz * 0.44f, cy),
            PointF(cx - sz * 0.02f, cy + sz * 0.36f)
        };
        g.FillPolygon(&br, tri1, 3);
        g.FillPolygon(&br, tri2, 3);
    } else {
        PointF tri1[3] = {
            PointF(cx + sz * 0.44f, cy - sz * 0.36f),
            PointF(cx - sz * 0.04f, cy),
            PointF(cx + sz * 0.44f, cy + sz * 0.36f)
        };
        PointF tri2[3] = {
            PointF(cx + sz * 0.02f, cy - sz * 0.36f),
            PointF(cx - sz * 0.44f, cy),
            PointF(cx + sz * 0.02f, cy + sz * 0.36f)
        };
        g.FillPolygon(&br, tri1, 3);
        g.FillPolygon(&br, tri2, 3);
    }
}

// ============================================================================
// SECTION 10: COMPLETE BORDERLESS 32-BIT PER-PIXEL ALPHA RENDERER
// ============================================================================

void RenderAnsaDynamicIsland(HWND hWnd) {
    if (g_isHiddenByFullscreen) return;

    float s = g_cfg.uiScale;
    float glowMargin = 26.0f * s;

    int winW = (int)std::ceil(g_sprW.current + glowMargin * 2.0f + 44.0f * s);
    int winH = (int)std::ceil(g_sprH.current + glowMargin * 2.0f);
    if (winW <= 20 || winH <= 20) return;

    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = winW;
    bmi.bmiHeader.biHeight = -winH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = NULL;
    HBITMAP hBmp = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBmp);

    Graphics g(memDC);
    g.SetSmoothingMode(SmoothingModeHighQuality);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.Clear(Color(0, 0, 0, 0));

    float baseCenterOffset = (winW - g_sprW.current) * 0.5f;
    float padX = baseCenterOffset + g_sprBounceX.current;
    float padY = g_cfg.isNotch ? 0.0f : (glowMargin * 0.65f + g_sprBounceY.current);
    float w = g_sprW.current;
    float h = g_sprH.current;

    Color userAccent = GetAccentColorByIndex(g_cfg.accentIdx);
    Color activeGlowColor = userAccent;
    bool mediaActive = false;
    bool isNotification = (g_viewMode == VIEW_TOAST_NOTIFICATION);

    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        mediaActive = (g_cfg.enableMedia && g_media.hasSession);
        if (mediaActive && g_media.isPlaying && !isNotification) {
            activeGlowColor = g_media.dominantColor;
        }
    }
    if (isNotification) {
        activeGlowColor = g_hud.accent;
    }

    if (!g_cfg.isNotch) {
        GraphicsPath shadowPath;
        BuildIslandGeometry(shadowPath, padX, padY + 4.0f * s, w, h, false);
        SolidBrush shadowBrush(Color(92, 0, 0, 0));
        g.FillPath(&shadowBrush, &shadowPath);
    }

    if ((g_cfg.theme == THEME_GLOW || isNotification) && g_sprGlow.current > 0.02f) {
        float gAlpha = g_sprGlow.current;
        if (isNotification) {
            float pulse = 0.82f + 0.18f * sinf(g_oneUiPulsePhase * 2.6f);
            gAlpha = std::min(1.0f, gAlpha * 1.35f * pulse);
        }
        int layers = isNotification ? 4 : 3;
        float baseAlpha = isNotification ? 56.0f : 42.0f;

        for (int layer = layers; layer >= 1; layer--) {
            float spread = layer * 3.6f * s;
            GraphicsPath glowPath;
            BuildIslandGeometry(glowPath, padX - spread, g_cfg.isNotch ? 0.0f : (padY - spread * 0.7f),
                                w + spread * 2.0f, h + spread * (g_cfg.isNotch ? 1.0f : 1.5f), g_cfg.isNotch);
            BYTE a = (BYTE)std::min(220.0f, gAlpha * (baseAlpha / (float)layer));
            SolidBrush glowBrush(Color(a, activeGlowColor.GetR(), activeGlowColor.GetG(), activeGlowColor.GetB()));
            g.FillPath(&glowBrush, &glowPath);
        }
    }

    GraphicsPath islandPath;
    BuildIslandGeometry(islandPath, padX, padY, w, h, g_cfg.isNotch);

    if (g_cfg.theme == THEME_GLASS) {
        LinearGradientBrush glassBrush(
            PointF(padX, padY), PointF(padX, padY + h),
            Color(240, 22, 22, 28), Color(248, 10, 10, 14)
        );
        g.FillPath(&glassBrush, &islandPath);
    } else {
        SolidBrush oledBrush(Color(254, 4, 4, 6));
        g.FillPath(&oledBrush, &islandPath);
    }

    g.SetClip(&islandPath);

    float innerPadY = g_cfg.isNotch ? (padY + 2.0f * s) : padY;
    float innerH = g_cfg.isNotch ? (h - 2.0f * s) : h;

    FontFamily fontFam(L"Segoe UI");
    StringFormat leftFmt;
    leftFmt.SetLineAlignment(StringAlignmentCenter);
    leftFmt.SetTrimming(StringTrimmingEllipsisCharacter);
    leftFmt.SetFormatFlags(StringFormatFlagsNoWrap);

    StringFormat centerFmt;
    centerFmt.SetAlignment(StringAlignmentCenter);
    centerFmt.SetLineAlignment(StringAlignmentCenter);
    centerFmt.SetTrimming(StringTrimmingEllipsisCharacter);
    centerFmt.SetFormatFlags(StringFormatFlagsNoWrap);

    StringFormat rightFmt;
    rightFmt.SetAlignment(StringAlignmentFar);
    rightFmt.SetLineAlignment(StringAlignmentCenter);
    rightFmt.SetFormatFlags(StringFormatFlagsNoWrap);

    float idleBaselineW = (g_cfg.baseIdleW + 46.0f) * s;
    float expandProgress = std::max(0.0f, std::min(1.0f, (w - idleBaselineW) / (150.0f * s)));

    ZeroMemory(&g_hit, sizeof(g_hit));

    // ========================================================================
    // RENDER MODE A: COMPACT IDLE PILL / NOTCH
    // ========================================================================
    if (expandProgress < 0.35f) {
        BYTE alphaIdle = (BYTE)(255.0f * (1.0f - expandProgress * 2.85f));
        bool isMinimal = (g_cfg.minimalMode || g_cfg.idleDisplay == IDLE_MINIMAL);
        bool showIdleViz = ShouldShowIdleVisualizer();

        if (isMinimal) {
            float dotSz = 7.0f * s;
            SolidBrush dotBr(Color(alphaIdle, userAccent.GetR(), userAccent.GetG(), userAccent.GetB()));
            g.FillEllipse(&dotBr, padX + 14.0f * s, innerPadY + (innerH - dotSz) * 0.5f, dotSz, dotSz);

            if (showIdleViz) {
                float barW = 2.6f * s;
                float barGap = 2.4f * s;
                float eqTotalW = 5 * barW + 4 * barGap;
                float eqX = padX + w - 14.0f * s - eqTotalW;
                float centerY = innerPadY + innerH * 0.5f;

                for (int i = 0; i < 5; i++) {
                    float amp = g_eqBars[i];
                    float bh = (4.0f + amp * 12.0f) * s;
                    SolidBrush eqBr(Color(alphaIdle, activeGlowColor.GetR(), activeGlowColor.GetG(), activeGlowColor.GetB()));
                    GraphicsPath barP;
                    BuildPillRect(barP, eqX + i * (barW + barGap), centerY - bh * 0.5f, barW, bh, barW * 0.5f);
                    g.FillPath(&eqBr, &barP);
                }
            }
        } else {
            float leftCursor = padX + 15.0f * s;
            float ringR = 10.5f * s;

            if (!g_shelf.empty()) {
                float dotR = 7.0f * s;
                SolidBrush badgeDot(Color(alphaIdle, userAccent.GetR(), userAccent.GetG(), userAccent.GetB()));
                g.FillEllipse(&badgeDot, leftCursor, innerPadY + (innerH - dotR) * 0.5f, dotR, dotR);
                leftCursor += dotR + 6.0f * s;

                Font shFont(&fontFam, 13.0f * s, FontStyleBold, UnitPixel);
                SolidBrush shBrush(Color(alphaIdle, 235, 240, 248));
                std::wstring shText = std::to_wstring(g_shelf.size()) + (g_shelf.size() == 1 ? L" File" : L" Files");
                RectF shRect(leftCursor, innerPadY, 68.0f * s, innerH);
                g.DrawString(shText.c_str(), -1, &shFont, shRect, &leftFmt, &shBrush);
            } else if (g_cfg.idleDisplay == IDLE_RING_CLOCK || g_cfg.idleDisplay == IDLE_RING_ONLY) {
                SYSTEM_POWER_STATUS sps;
                int pctInt = 100;
                bool isCharging = false;
                if (GetSystemPowerStatus(&sps) && sps.BatteryLifePercent <= 100) {
                    pctInt = (int)sps.BatteryLifePercent;
                    isCharging = (sps.ACLineStatus == 1);
                }
                float targetPct = pctInt / 100.0f;
                g_idleBatSmooth += (targetPct - g_idleBatSmooth) * 0.18f;

                Color bCol = GetBatteryDynamicColor(pctInt, isCharging);
                float ringThick = 3.4f * s;
                DrawCircularBatteryDonut(g, leftCursor + ringR, innerPadY + innerH * 0.5f, ringR, ringThick, g_idleBatSmooth, isCharging, bCol, alphaIdle);
            }

            // Build right-side items from right edge inward so the animated visualizer sits after the 12h clock
            float rightCursor = padX + w - 15.0f * s;

            if (showIdleViz) {
                float barW = 2.6f * s;
                float barGap = 2.4f * s;
                float eqTotalW = 5 * barW + 4 * barGap;
                rightCursor -= eqTotalW;
                float centerY = innerPadY + innerH * 0.5f;

                for (int i = 0; i < 5; i++) {
                    float amp = g_eqBars[i];
                    float bh = (4.0f + amp * 14.0f) * s;
                    SolidBrush eqBr(Color(alphaIdle, activeGlowColor.GetR(), activeGlowColor.GetG(), activeGlowColor.GetB()));
                    GraphicsPath barP;
                    BuildPillRect(barP, rightCursor + i * (barW + barGap), centerY - bh * 0.5f, barW, bh, barW * 0.5f);
                    g.FillPath(&eqBr, &barP);
                }
                rightCursor -= 10.0f * s;
            }

            if (g_cfg.idleDisplay == IDLE_RING_CLOCK || g_cfg.idleDisplay == IDLE_CLOCK) {
                time_t now = time(0);
                struct tm ts;
                localtime_s(&ts, &now);
                wchar_t timeBuf[16] = { 0 };
                int h12 = ts.tm_hour % 12;
                if (h12 == 0) h12 = 12;
                swprintf_s(timeBuf, 16, L"%d:%02d", h12, ts.tm_min);

                float clkW = 58.0f * s;
                rightCursor -= clkW;
                Font clkFont(&fontFam, 19.5f * s, FontStyleBold, UnitPixel);
                SolidBrush clkBrush(Color(alphaIdle, 245, 245, 250));
                RectF clkRect(rightCursor, innerPadY - 0.5f * s, clkW, innerH);
                g.DrawString(timeBuf, -1, &clkFont, clkRect, &rightFmt, &clkBrush);
            }
        }
    }
    // ========================================================================
    // RENDER MODE B: EXPANDED VIEWS
    // ========================================================================
    else {
        BYTE a = (BYTE)(255.0f * std::min(1.0f, (expandProgress - 0.35f) * 1.6f));
        SolidBrush whiteBr(Color(a, 255, 255, 255));
        SolidBrush mutedBr(Color(a, 168, 168, 180));
        SolidBrush dimBr(Color(a, 115, 115, 128));

        // --------------------------------------------------------------------
        // B1. IN-ISLAND ANSA STUDIO SETTINGS
        // --------------------------------------------------------------------
        if (g_viewMode == VIEW_SETTINGS_STUDIO) {
            float m = 16.0f * s;

            Font headFont(&fontFam, 14.0f * s, FontStyleBold, UnitPixel);
            RectF headRect(padX + m, innerPadY + 12.0f * s, w - 105.0f * s, 22.0f * s);
            g.DrawString(L"Ansa Studio • Personalization", -1, &headFont, headRect, &leftFmt, &whiteBr);

            float closeW = 64.0f * s, closeH = 24.0f * s;
            float closeX = padX + w - m - closeW;
            float closeY = innerPadY + 11.0f * s;
            GraphicsPath closeP;
            BuildPillRect(closeP, closeX, closeY, closeW, closeH, closeH * 0.5f);
            SolidBrush closeBg(Color(a, 46, 46, 54));
            g.FillPath(&closeBg, &closeP);
            Font btnFont(&fontFam, 11.2f * s, FontStyleBold, UnitPixel);
            RectF closeR(closeX, closeY, closeW, closeH);
            g.DrawString(L"Done", -1, &btnFont, closeR, &centerFmt, &whiteBr);
            g_hit.setClose = { (LONG)closeX, (LONG)closeY, (LONG)(closeX + closeW), (LONG)(closeY + closeH) };

            float tabY = innerPadY + 42.0f * s;
            float tabH = 25.0f * s;
            float tabW = (w - m * 2.0f - 12.0f * s) / 3.0f;
            const wchar_t* tabLabels[3] = { L"Style & Physics", L"Idle & Behavior", L"Accent Theme" };
            RECT* tabHits[3] = { &g_hit.setTabStyle, &g_hit.setTabModules, &g_hit.setTabColors };

            for (int t = 0; t < 3; t++) {
                float tx = padX + m + t * (tabW + 6.0f * s);
                GraphicsPath tp;
                BuildPillRect(tp, tx, tabY, tabW, tabH, 8.0f * s);
                bool activeTab = ((int)g_settingsTab == t);
                SolidBrush tBg(activeTab ? Color(a, userAccent.GetR(), userAccent.GetG(), userAccent.GetB()) : Color(a, 26, 26, 32));
                g.FillPath(&tBg, &tp);
                RectF tr(tx, tabY, tabW, tabH);
                g.DrawString(tabLabels[t], -1, &btnFont, tr, &centerFmt, activeTab ? &whiteBr : &mutedBr);
                *tabHits[t] = { (LONG)tx, (LONG)tabY, (LONG)(tx + tabW), (LONG)(tabY + tabH) };
            }

            float bodyY = innerPadY + 75.0f * s;
            float cardH = 33.0f * s;
            float rowGap = 6.0f * s;
            float halfW = (w - m * 2.0f - 8.0f * s) * 0.5f;
            SolidBrush cardBg(Color(a, 24, 24, 30));

            if (g_settingsTab == TAB_STYLE) {
                std::wstring labels[6];
                labels[0] = g_cfg.isNotch ? L"Shape: Top Notch" : L"Shape: Floating Pill";
                wchar_t scaleStr[48];
                swprintf_s(scaleStr, L"UI Scale: %d%%", (int)std::round(g_cfg.uiScale * 100.0f));
                labels[1] = scaleStr;
                labels[2] = (g_cfg.theme == THEME_GLOW) ? L"Theme: Album Glow" : (g_cfg.theme == THEME_GLASS ? L"Theme: Dark Glass" : L"Theme: OLED Black");
                labels[3] = (g_cfg.springPreset == SPRING_BOUNCY) ? L"Physics: Bouncy" : L"Physics: Normal";
                labels[4] = g_cfg.hideFullscreen ? L"Fullscreen: Auto-Hide" : L"Fullscreen: Always Show";
                labels[5] = L"Idle Opacity: " + std::to_wstring(g_cfg.idleOpacity) + L"%";

                for (int idx = 0; idx < 6; idx++) {
                    int row = idx / 2;
                    int col = idx % 2;
                    float cx = padX + m + col * (halfW + 8.0f * s);
                    float cy = bodyY + row * (cardH + rowGap);
                    GraphicsPath cp;
                    BuildPillRect(cp, cx, cy, halfW, cardH, 9.0f * s);
                    g.FillPath(&cardBg, &cp);
                    RectF cr(cx, cy, halfW, cardH);
                    g.DrawString(labels[idx].c_str(), -1, &btnFont, cr, &centerFmt, &whiteBr);
                    g_hit.optBtn[idx] = { (LONG)cx, (LONG)cy, (LONG)(cx + halfW), (LONG)(cy + cardH) };
                }
            } else if (g_settingsTab == TAB_MODULES) {
                std::wstring labels[6];
                labels[0] = (g_cfg.idleDisplay == IDLE_RING_CLOCK) ? L"Idle: Ring + Clock" :
                            (g_cfg.idleDisplay == IDLE_RING_ONLY ? L"Idle: Ring Only" :
                            (g_cfg.idleDisplay == IDLE_CLOCK ? L"Idle: Clock Only" : L"Idle: Minimal"));
                labels[1] = g_cfg.minimalMode ? L"Minimal Mode: ON" : L"Minimal Mode: OFF";
                labels[2] = g_cfg.enableMedia ? L"Media Player: ON" : L"Media Player: OFF";
                labels[3] = g_cfg.edgeHoverExpand ? L"Edge Hover: ON" : L"Edge Hover: OFF";
                labels[4] = g_cfg.enableBluetooth ? L"Bluetooth HUD: ON" : L"Bluetooth HUD: OFF";
                labels[5] = g_cfg.enableNotifications ? L"Notifications: ON" : L"Notifications: OFF";

                for (int idx = 0; idx < 6; idx++) {
                    int row = idx / 2;
                    int col = idx % 2;
                    float cx = padX + m + col * (halfW + 8.0f * s);
                    float cy = bodyY + row * (cardH + rowGap);
                    GraphicsPath cp;
                    BuildPillRect(cp, cx, cy, halfW, cardH, 9.0f * s);
                    g.FillPath(&cardBg, &cp);
                    RectF cr(cx, cy, halfW, cardH);
                    g.DrawString(labels[idx].c_str(), -1, &btnFont, cr, &centerFmt, &whiteBr);
                    g_hit.optBtn[idx] = { (LONG)cx, (LONG)cy, (LONG)(cx + halfW), (LONG)(cy + cardH) };
                }
            } else {
                Font subFont(&fontFam, 12.0f * s, FontStyleRegular, UnitPixel);
                std::wstring curColLabel = L"Selected Accent: " + GetAccentNameByIndex(g_cfg.accentIdx);
                RectF lblR(padX + m, bodyY + 4.0f * s, w - m * 2.0f, 20.0f * s);
                g.DrawString(curColLabel.c_str(), -1, &subFont, lblR, &leftFmt, &mutedBr);

                float swY = bodyY + 36.0f * s;
                float swSz = 44.0f * s;
                float totalSwW = 6 * swSz + 5 * (12.0f * s);
                float startSwX = padX + (w - totalSwW) * 0.5f;

                for (int c = 0; c < 6; c++) {
                    float sx = startSwX + c * (swSz + 12.0f * s);
                    Color sc = GetAccentColorByIndex(c);
                    SolidBrush swBr(Color(a, sc.GetR(), sc.GetG(), sc.GetB()));
                    g.FillEllipse(&swBr, sx, swY, swSz, swSz);

                    if ((g_cfg.accentIdx % 6) == c) {
                        SolidBrush innerDot(Color(a, 255, 255, 255));
                        g.FillEllipse(&innerDot, sx + swSz * 0.36f, swY + swSz * 0.36f, swSz * 0.28f, swSz * 0.28f);
                    }
                    g_hit.colorSwatches[c] = { (LONG)sx, (LONG)swY, (LONG)(sx + swSz), (LONG)(swY + swSz) };
                }
            }
        }
        // --------------------------------------------------------------------
        // B2. DRAGGABLE & TOUCHPAD-SWIPEABLE 4-PAGE EXPANDED HUB
        // --------------------------------------------------------------------
        else if (g_viewMode == VIEW_EXPANDED_HUB) {
            float m = 16.0f * s;
            float scroll = g_sprPageScroll.current;

            float gearSz = 24.0f * s;
            float gearX = padX + w - m - gearSz;
            float gearY = innerPadY + 12.0f * s;
            GraphicsPath gearPill;
            BuildPillRect(gearPill, gearX, gearY, gearSz, gearSz, gearSz * 0.5f);
            SolidBrush gearBg(Color(a, 30, 30, 36));
            g.FillPath(&gearBg, &gearPill);
            DrawVectorGearIcon(g, gearX + gearSz * 0.5f, gearY + gearSz * 0.5f, gearSz * 0.32f, Color(255, 215, 215, 225), a);
            g_hit.btnSettings = { (LONG)gearX, (LONG)gearY, (LONG)(gearX + gearSz), (LONG)(gearY + gearSz) };

            Font titleFont(&fontFam, 13.5f * s, FontStyleBold, UnitPixel);
            Font subFont(&fontFam, 11.5f * s, FontStyleRegular, UnitPixel);
            Font smallBold(&fontFam, 10.5f * s, FontStyleBold, UnitPixel);

            for (int p = 0; p < PAGE_COUNT; p++) {
                float pageDelta = (float)p - scroll;
                if (std::abs(pageDelta) >= 0.995f) continue;

                float px = padX + pageDelta * w;
                float pageGearX = px + w - m - gearSz;
                float pageAlphaF = std::max(0.0f, 1.0f - std::abs(pageDelta));
                BYTE pa = (BYTE)(a * pageAlphaF);
                SolidBrush pWhite(Color(pa, 255, 255, 255));
                SolidBrush pMuted(Color(pa, 165, 165, 178));
                SolidBrush pDim(Color(pa, 115, 115, 128));

                // ============================================================
                // PAGE 0: MUSIC PLAYER WITH CENTERED TRANSPORT CONTROLS
                // ============================================================
                if (p == PAGE_MEDIA) {
                    float artSz = 72.0f * s;
                    float artX = px + m;
                    float artY = innerPadY + 14.0f * s;

                    GraphicsPath artClip;
                    BuildPillRect(artClip, artX, artY, artSz, artSz, 16.0f * s);
                    GraphicsState artState = g.Save();
                    g.SetClip(&artClip, CombineModeIntersect);

                    bool drewRealArt = false;
                    std::wstring trackTitle = L"No Active Media";
                    std::wstring trackArtist = L"Play music on Spotify, Chrome or Apple Music";
                    std::wstring trackApp = L"Idle";
                    float trackProg = 0.0f;
                    INT64 posT = 0, endT = 0;
                    bool playing = false;

                    {
                        std::lock_guard<std::mutex> lock(g_mediaMutex);
                        if (g_media.hasSession) {
                            trackTitle = g_media.title;
                            trackArtist = g_media.artist;
                            trackApp = g_media.sourceApp;
                            trackProg = g_media.progress;
                            posT = g_media.posTicks;
                            endT = g_media.endTicks;
                            playing = g_media.isPlaying;
                            if (g_media.pAlbumArt && g_media.pAlbumArt->GetLastStatus() == Ok) {
                                g.DrawImage(g_media.pAlbumArt, artX, artY, artSz, artSz);
                                drewRealArt = true;
                            }
                        }
                    }

                    if (!drewRealArt) {
                        LinearGradientBrush artGrad(
                            PointF(artX, artY), PointF(artX + artSz, artY + artSz),
                            Color(pa, activeGlowColor.GetR(), activeGlowColor.GetG(), activeGlowColor.GetB()),
                            Color(pa, 22, 22, 32)
                        );
                        g.FillRectangle(&artGrad, artX, artY, artSz, artSz);

                        SolidBrush noteBr(Color(pa, 255, 255, 255));
                        Pen notePen(Color(pa, 255, 255, 255), 3.0f * s);
                        g.FillEllipse(&noteBr, artX + artSz * 0.28f, artY + artSz * 0.56f, artSz * 0.22f, artSz * 0.16f);
                        g.DrawLine(&notePen, artX + artSz * 0.48f, artY + artSz * 0.62f, artX + artSz * 0.48f, artY + artSz * 0.26f);
                        g.DrawLine(&notePen, artX + artSz * 0.48f, artY + artSz * 0.26f, artX + artSz * 0.68f, artY + artSz * 0.34f);
                    }
                    g.Restore(artState);

                    float textX = artX + artSz + 12.0f * s;
                    float metaMaxW = (pageGearX - 8.0f * s) - textX;

                    RectF titleRect(textX, innerPadY + 13.0f * s, metaMaxW, 20.0f * s);
                    g.DrawString(trackTitle.c_str(), -1, &titleFont, titleRect, &leftFmt, &pWhite);

                    std::wstring subLine = trackArtist;
                    if (!trackApp.empty() && trackApp != L"Idle") subLine += L"  •  " + trackApp;
                    RectF artistRect(textX, innerPadY + 33.0f * s, metaMaxW, 18.0f * s);
                    g.DrawString(subLine.c_str(), -1, &subFont, artistRect, &leftFmt, &pMuted);

                    float barY = innerPadY + 60.0f * s;
                    float barH = 5.5f * s;
                    float timeColW = 34.0f * s;
                    float barX = textX + timeColW;
                    float barW = (px + w - m) - barX - timeColW;

                    std::wstring elapsedStr = (endT > 0) ? FormatDurationTicks(posT) : L"0:00";
                    std::wstring totalStr = (endT > 0) ? FormatDurationTicks(endT) : L"--:--";

                    RectF elRect(textX, barY - 5.0f * s, timeColW - 4.0f * s, 15.0f * s);
                    RectF totRect(barX + barW + 4.0f * s, barY - 5.0f * s, timeColW - 4.0f * s, 15.0f * s);
                    g.DrawString(elapsedStr.c_str(), -1, &smallBold, elRect, &leftFmt, &pDim);
                    g.DrawString(totalStr.c_str(), -1, &smallBold, totRect, &rightFmt, &pDim);

                    DrawOneUi9WavyProgressBar(g, barX, barY, barW, barH, trackProg, activeGlowColor, pa);

                    float ctrlCenterY = innerPadY + 98.0f * s;
                    float ctrlCenterX = px + w * 0.5f;
                    float btnRadius = 15.0f * s;

                    float prevX = ctrlCenterX - 46.0f * s;
                    DrawMediaGlyph(g, 0, prevX, ctrlCenterY, 16.0f * s, Color(255, 235, 235, 240), pa);

                    GraphicsPath playCircle;
                    playCircle.AddEllipse(ctrlCenterX - 16.0f * s, ctrlCenterY - 16.0f * s, 32.0f * s, 32.0f * s);
                    SolidBrush playBg(Color(pa, 255, 255, 255));
                    g.FillPath(&playBg, &playCircle);
                    DrawMediaGlyph(g, playing ? 2 : 1, ctrlCenterX, ctrlCenterY, 15.0f * s, Color(255, 10, 10, 14), pa);

                    float nextX = ctrlCenterX + 46.0f * s;
                    DrawMediaGlyph(g, 3, nextX, ctrlCenterY, 16.0f * s, Color(255, 235, 235, 240), pa);

                    if (p == g_hubPage) {
                        g_hit.barProgress = { (LONG)barX, (LONG)(barY - 8.0f * s), (LONG)(barX + barW), (LONG)(barY + barH + 8.0f * s) };
                        g_hit.btnPrev = { (LONG)(prevX - btnRadius), (LONG)(ctrlCenterY - btnRadius), (LONG)(prevX + btnRadius), (LONG)(ctrlCenterY + btnRadius) };
                        g_hit.btnPlay = { (LONG)(ctrlCenterX - 18.0f * s), (LONG)(ctrlCenterY - 18.0f * s), (LONG)(ctrlCenterX + 18.0f * s), (LONG)(ctrlCenterY + 18.0f * s) };
                        g_hit.btnNext = { (LONG)(nextX - btnRadius), (LONG)(ctrlCenterY - btnRadius), (LONG)(nextX + btnRadius), (LONG)(ctrlCenterY + btnRadius) };
                    }
                }
                // ============================================================
                // PAGE 1: SHELBOOK
                // ============================================================
                else if (p == PAGE_SHELFBOOK) {
                    RectF hdrR(px + m, innerPadY + 12.0f * s, 180.0f * s, 22.0f * s);
                    std::wstring shHdr = L"ShelfBook";
                    if (!g_shelf.empty()) shHdr += L"  (" + std::to_wstring(g_shelf.size()) + L")";
                    g.DrawString(shHdr.c_str(), -1, &titleFont, hdrR, &leftFmt, &pWhite);

                    if (!g_shelf.empty()) {
                        float clrW = 62.0f * s, clrH = 22.0f * s;
                        float clrX = pageGearX - clrW - 8.0f * s;
                        float clrY = innerPadY + 13.0f * s;
                        GraphicsPath clrP;
                        BuildPillRect(clrP, clrX, clrY, clrW, clrH, clrH * 0.5f);
                        SolidBrush clrBg(Color(pa, 48, 24, 28));
                        g.FillPath(&clrBg, &clrP);
                        SolidBrush clrFg(Color(pa, 255, 69, 58));
                        RectF clrR(clrX, clrY, clrW, clrH);
                        g.DrawString(L"Clear All", -1, &smallBold, clrR, &centerFmt, &clrFg);
                        if (p == g_hubPage) {
                            g_hit.shelfClearAll = { (LONG)clrX, (LONG)clrY, (LONG)(clrX + clrW), (LONG)(clrY + clrH) };
                        }
                    }

                    float bodyY = innerPadY + 42.0f * s;
                    float bodyH = 78.0f * s;
                    float bodyW = w - m * 2.0f;

                    if (g_shelf.empty()) {
                        GraphicsPath dropZone;
                        BuildPillRect(dropZone, px + m, bodyY, bodyW, bodyH, 14.0f * s);
                        SolidBrush dzBg(Color(pa, 20, 20, 26));
                        g.FillPath(&dzBg, &dropZone);
                        RectF dzR(px + m, bodyY, bodyW, bodyH);
                        g.DrawString(L"Drop files here to store in ShelfBook\nDrag files out with mouse anytime", -1, &subFont, dzR, &centerFmt, &pMuted);
                    } else {
                        int totalPages = ((int)g_shelf.size() + 1) / 2;
                        g_shelfPage = ((g_shelfPage % totalPages) + totalPages) % totalPages;

                        float navW = 20.0f * s;
                        float slotsAreaX = px + m + (totalPages > 1 ? navW : 0.0f);
                        float slotsAreaW = bodyW - (totalPages > 1 ? navW * 2.0f : 0.0f);
                        float slotGap = 8.0f * s;
                        float slotW = (slotsAreaW - slotGap) * 0.5f;

                        if (totalPages > 1) {
                            RectF prevPgR(px + m, bodyY, navW, bodyH);
                            RectF nextPgR(px + w - m - navW, bodyY, navW, bodyH);
                            g.DrawString(L"‹", -1, &titleFont, prevPgR, &centerFmt, &pMuted);
                            g.DrawString(L"›", -1, &titleFont, nextPgR, &centerFmt, &pMuted);
                            if (p == g_hubPage) {
                                g_hit.shelfPrevPage = { (LONG)prevPgR.X, (LONG)prevPgR.Y, (LONG)(prevPgR.X + prevPgR.Width), (LONG)(prevPgR.Y + prevPgR.Height) };
                                g_hit.shelfNextPage = { (LONG)nextPgR.X, (LONG)nextPgR.Y, (LONG)(nextPgR.X + nextPgR.Width), (LONG)(nextPgR.Y + nextPgR.Height) };
                            }
                        }

                        for (int slot = 0; slot < 2; slot++) {
                            int itemIdx = g_shelfPage * 2 + slot;
                            if (itemIdx >= (int)g_shelf.size()) break;
                            const ShelfEntry& item = g_shelf[itemIdx];

                            float sx = slotsAreaX + slot * (slotW + slotGap);
                            GraphicsPath cardP;
                            BuildPillRect(cardP, sx, bodyY, slotW, bodyH, 14.0f * s);
                            SolidBrush cardBg(Color(pa, 22, 22, 28));
                            g.FillPath(&cardBg, &cardP);

                            float bW = 36.0f * s, bH = 22.0f * s;
                            float bX = sx + 10.0f * s, bY = bodyY + 10.0f * s;
                            GraphicsPath bp;
                            BuildPillRect(bp, bX, bY, bW, bH, 6.0f * s);
                            SolidBrush bBr(Color(pa, item.badgeColor.GetR(), item.badgeColor.GetG(), item.badgeColor.GetB()));
                            g.FillPath(&bBr, &bp);
                            RectF bR(bX, bY, bW, bH);
                            g.DrawString(item.extBadge.c_str(), -1, &smallBold, bR, &centerFmt, &pWhite);

                            float delSz = 18.0f * s;
                            float delX = sx + slotW - delSz - 8.0f * s;
                            float delY = bodyY + 10.0f * s;
                            SolidBrush delBg(Color(pa, 48, 24, 28));
                            g.FillEllipse(&delBg, delX, delY, delSz, delSz);
                            SolidBrush delFg(Color(pa, 255, 69, 58));
                            RectF delR(delX, delY, delSz, delSz);
                            g.DrawString(L"✕", -1, &smallBold, delR, &centerFmt, &delFg);

                            RectF fnR(sx + 10.0f * s, bodyY + 37.0f * s, slotW - 20.0f * s, 18.0f * s);
                            g.DrawString(item.fileName.c_str(), -1, &smallBold, fnR, &leftFmt, &pWhite);

                            std::wstring meta = item.sizeText + L" • Drag/Click";
                            RectF mtR(sx + 10.0f * s, bodyY + 55.0f * s, slotW - 20.0f * s, 15.0f * s);
                            g.DrawString(meta.c_str(), -1, &smallBold, mtR, &leftFmt, &pDim);

                            if (p == g_hubPage) {
                                g_hit.shelfSlotDel[slot] = { (LONG)delX, (LONG)delY, (LONG)(delX + delSz), (LONG)(delY + delSz) };
                                g_hit.shelfSlotOpen[slot] = { (LONG)sx, (LONG)bodyY, (LONG)(sx + slotW), (LONG)(bodyY + bodyH) };
                            }
                        }
                    }
                }
                // ============================================================
                // PAGE 2: CALENDAR
                // ============================================================
                else if (p == PAGE_CALENDAR) {
                    time_t now = time(0) + (time_t)g_selectedCalDayOffset * 86400;
                    struct tm ts;
                    localtime_s(&ts, &now);

                    const wchar_t* months[12] = { L"January", L"February", L"March", L"April", L"May", L"June", L"July", L"August", L"September", L"October", L"November", L"December" };
                    const wchar_t* weekdays[7] = { L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday" };

                    float leftCardW = 104.0f * s;
                    float bodyY = innerPadY + 16.0f * s;
                    float bodyH = 104.0f * s;
                    GraphicsPath dateCard;
                    BuildPillRect(dateCard, px + m, bodyY, leftCardW, bodyH, 16.0f * s);
                    SolidBrush dcBg(Color(pa, 22, 22, 28));
                    g.FillPath(&dcBg, &dateCard);

                    Font bigDayFont(&fontFam, 34.0f * s, FontStyleBold, UnitPixel);
                    SolidBrush accBr(Color(pa, userAccent.GetR(), userAccent.GetG(), userAccent.GetB()));
                    std::wstring dayNumStr = std::to_wstring(ts.tm_mday);
                    RectF dayNumR(px + m, bodyY + 12.0f * s, leftCardW, 42.0f * s);
                    g.DrawString(dayNumStr.c_str(), -1, &bigDayFont, dayNumR, &centerFmt, &accBr);

                    RectF wkR(px + m, bodyY + 56.0f * s, leftCardW, 18.0f * s);
                    g.DrawString(weekdays[ts.tm_wday], -1, &smallBold, wkR, &centerFmt, &pWhite);

                    std::wstring monYr = std::wstring(months[ts.tm_mon]).substr(0, 3) + L" " + std::to_wstring(1900 + ts.tm_year);
                    RectF myR(px + m, bodyY + 74.0f * s, leftCardW, 16.0f * s);
                    g.DrawString(monYr.c_str(), -1, &smallBold, myR, &centerFmt, &pMuted);

                    float rightX = px + m + leftCardW + 12.0f * s;
                    float rightW = (px + w - m) - rightX;
                    float stripTopW = rightW - gearSz - 8.0f * s;
                    float cellW = stripTopW / 7.0f;

                    const wchar_t* shortDays[7] = { L"S", L"M", L"T", L"W", L"T", L"F", L"S" };
                    for (int d = 0; d < 7; d++) {
                        int offset = d - 3;
                        time_t dt = time(0) + (time_t)offset * 86400;
                        struct tm dts;
                        localtime_s(&dts, &dt);

                        float cx = rightX + d * cellW;
                        bool selected = (offset == g_selectedCalDayOffset);

                        RectF lblR(cx, bodyY, cellW, 16.0f * s);
                        g.DrawString(shortDays[dts.tm_wday], -1, &smallBold, lblR, &centerFmt, &pDim);

                        float pillY = bodyY + 18.0f * s;
                        float pillH = 26.0f * s;
                        if (selected) {
                            GraphicsPath selP;
                            BuildPillRect(selP, cx + 2.0f * s, pillY, cellW - 4.0f * s, pillH, 8.0f * s);
                            g.FillPath(&accBr, &selP);
                        }
                        RectF numR(cx, pillY, cellW, pillH);
                        g.DrawString(std::to_wstring(dts.tm_mday).c_str(), -1, &smallBold, numR, &centerFmt, selected ? &pWhite : &pMuted);

                        if (p == g_hubPage) {
                            g_hit.calDays[d] = { (LONG)cx, (LONG)bodyY, (LONG)(cx + cellW), (LONG)(pillY + pillH) };
                        }
                    }

                    float evY = bodyY + 54.0f * s;
                    float evH = 50.0f * s;
                    GraphicsPath evP;
                    BuildPillRect(evP, rightX, evY, rightW, evH, 12.0f * s);
                    SolidBrush evBg(Color(pa, 22, 22, 28));
                    g.FillPath(&evBg, &evP);

                    const wchar_t* sampleEvents[3] = {
                        L"•  Focus & Deep Work Session",
                        L"•  Review Weekly Goals & Tasks",
                        L"•  No Conflicts • Free Afternoon"
                    };
                    RectF evR(rightX + 12.0f * s, evY, rightW - 24.0f * s, evH);
                    g.DrawString(sampleEvents[g_calEventCycle % 3], -1, &subFont, evR, &leftFmt, &pWhite);

                    if (p == g_hubPage) {
                        g_hit.calEventPill = { (LONG)rightX, (LONG)evY, (LONG)(rightX + rightW), (LONG)(evY + evH) };
                    }
                }
                // ============================================================
                // PAGE 3: PERSONALIZED THINGS TO DO (INLINE TYPING EDITOR)
                // ============================================================
                else if (p == PAGE_TODO) {
                    RectF hdrR(px + m, innerPadY + 12.0f * s, 140.0f * s, 22.0f * s);
                    g.DrawString(L"Things To Do", -1, &titleFont, hdrR, &leftFmt, &pWhite);

                    if (!g_isTypingTask) {
                        float addW = 86.0f * s, addH = 22.0f * s;
                        float addX = pageGearX - addW - 8.0f * s;
                        float addY = innerPadY + 13.0f * s;
                        GraphicsPath addP;
                        BuildPillRect(addP, addX, addY, addW, addH, addH * 0.5f);
                        SolidBrush addBg(Color(pa, userAccent.GetR(), userAccent.GetG(), userAccent.GetB()));
                        g.FillPath(&addBg, &addP);
                        RectF addR(addX, addY, addW, addH);
                        g.DrawString(L"+ Add Task", -1, &smallBold, addR, &centerFmt, &pWhite);

                        float clW = 74.0f * s, clH = 22.0f * s;
                        float clX = addX - clW - 6.0f * s;
                        GraphicsPath clP;
                        BuildPillRect(clP, clX, addY, clW, clH, clH * 0.5f);
                        SolidBrush clBg(Color(pa, 30, 30, 36));
                        g.FillPath(&clBg, &clP);
                        RectF clR(clX, addY, clW, clH);
                        g.DrawString(L"Clear Done", -1, &smallBold, clR, &centerFmt, &pMuted);

                        if (p == g_hubPage) {
                            g_hit.todoAddBtn = { (LONG)addX, (LONG)addY, (LONG)(addX + addW), (LONG)(addY + addH) };
                            g_hit.todoClearDone = { (LONG)clX, (LONG)addY, (LONG)(clX + clW), (LONG)(addY + addH) };
                        }
                    }

                    float listY = innerPadY + 42.0f * s;
                    float rowW = w - m * 2.0f;

                    if (g_isTypingTask) {
                        float boxH = 34.0f * s;
                        float saveW = 54.0f * s;
                        float cancelW = 28.0f * s;
                        float fieldW = rowW - saveW - cancelW - 12.0f * s;

                        GraphicsPath inputP;
                        BuildPillRect(inputP, px + m, listY, fieldW, boxH, 10.0f * s);
                        SolidBrush inputBg(Color(pa, 26, 26, 34));
                        g.FillPath(&inputBg, &inputP);
                        Pen focusPen(Color(pa, userAccent.GetR(), userAccent.GetG(), userAccent.GetB()), 1.6f * s);
                        g.DrawPath(&focusPen, &inputP);

                        bool caretOn = ((GetTickCount64() / 480) % 2) == 0;
                        std::wstring displayDraft = g_taskDraft.empty()
                            ? (caretOn ? L"| Type your task..." : L"Type your task...")
                            : (g_taskDraft + (caretOn ? L"|" : L""));
                        Font inpFont(&fontFam, 12.0f * s, g_taskDraft.empty() ? FontStyleItalic : FontStyleRegular, UnitPixel);
                        RectF inpR(px + m + 10.0f * s, listY, fieldW - 18.0f * s, boxH);
                        g.DrawString(displayDraft.c_str(), -1, &inpFont, inpR, &leftFmt, g_taskDraft.empty() ? &pMuted : &pWhite);

                        float saveX = px + m + fieldW + 6.0f * s;
                        GraphicsPath saveP;
                        BuildPillRect(saveP, saveX, listY, saveW, boxH, 10.0f * s);
                        SolidBrush saveBg(Color(pa, 48, 209, 88));
                        g.FillPath(&saveBg, &saveP);
                        RectF saveR(saveX, listY, saveW, boxH);
                        g.DrawString(L"Save", -1, &smallBold, saveR, &centerFmt, &pWhite);

                        float canX = saveX + saveW + 6.0f * s;
                        GraphicsPath canP;
                        BuildPillRect(canP, canX, listY, cancelW, boxH, 10.0f * s);
                        SolidBrush canBg(Color(pa, 48, 24, 28));
                        g.FillPath(&canBg, &canP);
                        SolidBrush canFg(Color(pa, 255, 69, 58));
                        RectF canR(canX, listY, cancelW, boxH);
                        g.DrawString(L"✕", -1, &smallBold, canR, &centerFmt, &canFg);

                        if (p == g_hubPage) {
                            g_hit.todoInputSave = { (LONG)saveX, (LONG)listY, (LONG)(saveX + saveW), (LONG)(listY + boxH) };
                            g_hit.todoInputCancel = { (LONG)canX, (LONG)listY, (LONG)(canX + cancelW), (LONG)(listY + boxH) };
                        }

                        float subListY = listY + boxH + 6.0f * s;
                        float rowH = 22.0f * s;
                        int maxSub = std::min(2, (int)g_todos.size());
                        for (int t = 0; t < maxSub; t++) {
                            float ry = subListY + t * (rowH + 4.0f * s);
                            GraphicsPath rowP;
                            BuildPillRect(rowP, px + m, ry, rowW, rowH, 8.0f * s);
                            SolidBrush rowBg(Color(pa, 18, 18, 24));
                            g.FillPath(&rowBg, &rowP);
                            Font todoFont(&fontFam, 11.0f * s, g_todos[t].done ? FontStyleStrikeout : FontStyleRegular, UnitPixel);
                            RectF txtR(px + m + 12.0f * s, ry, rowW - 24.0f * s, rowH);
                            g.DrawString(g_todos[t].text.c_str(), -1, &todoFont, txtR, &leftFmt, &pMuted);
                        }
                    } else {
                        float rowH = 25.0f * s;
                        float rowGap = 4.0f * s;

                        if (g_todos.empty()) {
                            GraphicsPath emptyP;
                            BuildPillRect(emptyP, px + m, listY, rowW, 76.0f * s, 12.0f * s);
                            SolidBrush emptyBg(Color(pa, 20, 20, 26));
                            g.FillPath(&emptyBg, &emptyP);
                            RectF empR(px + m, listY, rowW, 76.0f * s);
                            g.DrawString(L"No tasks yet • Click '+ Add Task' to write your own", -1, &subFont, empR, &centerFmt, &pMuted);
                        } else {
                            int visibleRows = std::min(3, (int)g_todos.size());
                            for (int t = 0; t < visibleRows; t++) {
                                float ry = listY + t * (rowH + rowGap);
                                GraphicsPath rowP;
                                BuildPillRect(rowP, px + m, ry, rowW, rowH, 8.0f * s);
                                SolidBrush rowBg(Color(pa, 20, 20, 26));
                                g.FillPath(&rowBg, &rowP);

                                float chkSz = 14.0f * s;
                                float chkX = px + m + 10.0f * s;
                                float chkY = ry + (rowH - chkSz) * 0.5f;

                                if (g_todos[t].done) {
                                    SolidBrush chkFill(Color(pa, 48, 209, 88));
                                    g.FillEllipse(&chkFill, chkX, chkY, chkSz, chkSz);
                                    Pen tickPen(Color(pa, 10, 10, 14), 2.0f * s);
                                    tickPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
                                    g.DrawLine(&tickPen, chkX + chkSz * 0.28f, chkY + chkSz * 0.52f, chkX + chkSz * 0.45f, chkY + chkSz * 0.70f);
                                    g.DrawLine(&tickPen, chkX + chkSz * 0.45f, chkY + chkSz * 0.70f, chkX + chkSz * 0.75f, chkY + chkSz * 0.32f);
                                } else {
                                    SolidBrush chkEmpty(Color(pa, 44, 44, 54));
                                    g.FillEllipse(&chkEmpty, chkX, chkY, chkSz, chkSz);
                                }

                                float delSz = 16.0f * s;
                                float delX = px + m + rowW - delSz - 8.0f * s;
                                float delY = ry + (rowH - delSz) * 0.5f;
                                SolidBrush delFg(Color(pa, 130, 130, 145));
                                RectF delR(delX, delY, delSz, delSz);
                                g.DrawString(L"✕", -1, &smallBold, delR, &centerFmt, &delFg);

                                Font todoFont(&fontFam, 11.5f * s, g_todos[t].done ? FontStyleStrikeout : FontStyleRegular, UnitPixel);
                                RectF txtR(chkX + chkSz + 10.0f * s, ry, (delX - 6.0f * s) - (chkX + chkSz + 10.0f * s), rowH);
                                g.DrawString(g_todos[t].text.c_str(), -1, &todoFont, txtR, &leftFmt, g_todos[t].done ? &pDim : &pWhite);

                                if (p == g_hubPage) {
                                    g_hit.todoRowDel[t] = { (LONG)(delX - 4.0f * s), (LONG)ry, (LONG)(px + m + rowW), (LONG)(ry + rowH) };
                                    g_hit.todoRows[t] = { (LONG)(px + m), (LONG)ry, (LONG)(delX - 4.0f * s), (LONG)(ry + rowH) };
                                }
                            }
                        }
                    }
                }
            }

            float dotSz = 6.0f * s;
            float dotGap = 8.0f * s;
            float totalDotsW = PAGE_COUNT * dotSz + (PAGE_COUNT - 1) * dotGap;
            float dotsX = padX + (w - totalDotsW) * 0.5f;
            float dotsY = innerPadY + innerH - 13.0f * s;

            for (int d = 0; d < PAGE_COUNT; d++) {
                float dx = dotsX + d * (dotSz + dotGap);
                bool active = (d == g_hubPage);
                SolidBrush dBr(active ? Color(a, 255, 255, 255) : Color((BYTE)(a * 0.30f), 255, 255, 255));
                g.FillEllipse(&dBr, dx, dotsY, dotSz, dotSz);
                g_hit.pageDots[d] = { (LONG)(dx - 4.0f * s), (LONG)(dotsY - 6.0f * s), (LONG)(dx + dotSz + 4.0f * s), (LONG)(dotsY + dotSz + 6.0f * s) };
            }
        }
        // --------------------------------------------------------------------
        // B3. GLOWING EXPANDED TOAST NOTIFICATION BANNER
        // --------------------------------------------------------------------
        else if (g_viewMode == VIEW_TOAST_NOTIFICATION) {
            float m = 18.0f * s;
            float icoSz = 38.0f * s;
            float icoX = padX + m;
            float icoY = innerPadY + (innerH - icoSz) * 0.5f;

            GraphicsPath icoPill;
            BuildPillRect(icoPill, icoX, icoY, icoSz, icoSz, 11.0f * s);
            SolidBrush icoBg(Color(a, g_hud.accent.GetR(), g_hud.accent.GetG(), g_hud.accent.GetB()));
            g.FillPath(&icoBg, &icoPill);
            DrawVectorBellIcon(g, icoX + 5.0f * s, icoY + 5.0f * s, icoSz - 10.0f * s, Color(255, 255, 255, 255), a);

            float tx = icoX + icoSz + 13.0f * s;
            float tw = (padX + w - m) - tx;
            Font tFont(&fontFam, 14.0f * s, FontStyleBold, UnitPixel);
            Font bFont(&fontFam, 12.2f * s, FontStyleRegular, UnitPixel);

            RectF tR(tx, innerPadY + 15.0f * s, tw, 22.0f * s);
            g.DrawString(g_hud.title.c_str(), -1, &tFont, tR, &leftFmt, &whiteBr);

            std::wstring msgText = g_hud.bodyText.empty() ? g_hud.subtitle : g_hud.bodyText;
            RectF bR(tx, innerPadY + 39.0f * s, tw, 24.0f * s);
            g.DrawString(msgText.c_str(), -1, &bFont, bR, &leftFmt, &mutedBr);
        }
        // --------------------------------------------------------------------
        // B4. HARDWARE TELEMETRY HUDs
        // --------------------------------------------------------------------
        else {
            float m = 18.0f * s;
            float icoSz = 28.0f * s;
            float icoX = padX + m;
            float icoY = innerPadY + (innerH - icoSz) * 0.5f;

            float smoothProg = g_hud.animProgress;

            if (g_viewMode == VIEW_HUD_VOLUME) {
                DrawVectorSpeakerIcon(g, icoX, icoY, icoSz, smoothProg, Color(255, 255, 255, 255), a);
            } else if (g_viewMode == VIEW_HUD_BRIGHTNESS) {
                DrawVectorSunIcon(g, icoX, icoY, icoSz, smoothProg, Color(255, 255, 255, 255), a);
            } else if (g_viewMode == VIEW_HUD_BATTERY) {
                DrawOneUiBatteryHudIcon(g, icoX, icoY, icoSz, smoothProg, g_hud.subState == 1, g_hud.accent, a);
            } else if (g_viewMode == VIEW_HUD_BLUETOOTH) {
                DrawVectorBluetoothIcon(g, icoX, icoY, icoSz, g_hud.subState == 1, g_hud.accent, a);
            } else if (g_viewMode == VIEW_HUD_NETWORK) {
                DrawVectorWifiIcon(g, icoX, icoY, icoSz, smoothProg > 0.5f, g_hud.accent, a);
            }

            Font lblFont(&fontFam, 13.5f * s, FontStyleBold, UnitPixel);
            float lblX = icoX + icoSz + 10.0f * s;

            if (g_viewMode == VIEW_HUD_VOLUME || g_viewMode == VIEW_HUD_BRIGHTNESS || g_viewMode == VIEW_HUD_BATTERY) {
                float lblW = 92.0f * s;
                RectF lblR(lblX, innerPadY, lblW, innerH);
                g.DrawString(g_hud.title.c_str(), -1, &lblFont, lblR, &leftFmt, &whiteBr);

                float pctW = 46.0f * s;
                float pctX = padX + w - m - pctW;
                float barX = lblX + lblW + 6.0f * s;
                float barW = pctX - barX - 10.0f * s;
                float barH = 8.0f * s;
                float barY = innerPadY + (innerH - barH) * 0.5f;

                if (barW > 20.0f * s) {
                    DrawCapsuleProgressBar(g, barX, barY, barW, barH, smoothProg, g_hud.accent, a);
                }

                SolidBrush accBr(Color(a, g_hud.accent.GetR(), g_hud.accent.GetG(), g_hud.accent.GetB()));
                RectF pctR(pctX, innerPadY, pctW, innerH);
                g.DrawString(g_hud.subtitle.c_str(), -1, &lblFont, pctR, &rightFmt, &accBr);
            } else {
                float subW = 96.0f * s;
                float titleW = (padX + w - m) - lblX - subW;
                RectF lblR(lblX, innerPadY, titleW, innerH);
                g.DrawString(g_hud.title.c_str(), -1, &lblFont, lblR, &leftFmt, &whiteBr);

                SolidBrush accBr(Color(a, g_hud.accent.GetR(), g_hud.accent.GetG(), g_hud.accent.GetB()));
                RectF subR(padX + w - m - subW, innerPadY, subW, innerH);
                g.DrawString(g_hud.subtitle.c_str(), -1, &lblFont, subR, &rightFmt, &accBr);
            }
        }
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int winX = (screenW - winW) / 2;
    int winY = g_cfg.isNotch ? 0 : std::max(0, (int)(g_cfg.topMargin - glowMargin * 0.65f));

    BYTE masterAlpha = (g_viewMode == VIEW_IDLE) ? (BYTE)std::round((g_cfg.idleOpacity / 100.0f) * 255.0f) : 255;

    POINT ptSrc = { 0, 0 };
    POINT ptDst = { winX, winY };
    SIZE szWnd = { winW, winH };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, screenDC, &ptDst, &szWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(memDC, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}

// ============================================================================
// SECTION 11: SHELF FILE INGESTION & PERSONALIZED TODO HELPERS
// ============================================================================

void AddFileToShelf(const std::wstring& fullPath) {
    for (size_t i = 0; i < g_shelf.size(); i++) {
        if (_wcsicmp(g_shelf[i].fullPath.c_str(), fullPath.c_str()) == 0) {
            g_shelfPage = (int)(i / 2);
            return;
        }
    }

    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    bool isDir = false;
    ULONGLONG fileSize = 0;
    if (GetFileAttributesExW(fullPath.c_str(), GetFileExInfoStandard, &fad)) {
        isDir = (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        ULARGE_INTEGER uli;
        uli.LowPart = fad.nFileSizeLow;
        uli.HighPart = fad.nFileSizeHigh;
        fileSize = uli.QuadPart;
    }

    size_t slash = fullPath.find_last_of(L"\\/");
    std::wstring name = (slash != std::wstring::npos) ? fullPath.substr(slash + 1) : fullPath;

    std::wstring ext = isDir ? L"DIR" : L"FILE";
    Color badgeCol(255, 10, 132, 255);

    if (!isDir) {
        size_t dot = name.find_last_of(L'.');
        if (dot != std::wstring::npos && dot + 1 < name.size()) {
            ext = ToUpperString(name.substr(dot + 1));
            if (ext.size() > 4) ext = ext.substr(0, 4);

            if (ext == L"PDF" || ext == L"DOC" || ext == L"DOCX" || ext == L"TXT") {
                badgeCol = Color(255, 255, 69, 58);
            } else if (ext == L"PNG" || ext == L"JPG" || ext == L"JPEG" || ext == L"WEBP" || ext == L"SVG") {
                badgeCol = Color(255, 191, 90, 242);
            } else if (ext == L"MP4" || ext == L"MKV" || ext == L"MP3" || ext == L"WAV" || ext == L"FLAC") {
                badgeCol = Color(255, 255, 55, 95);
            } else if (ext == L"ZIP" || ext == L"RAR" || ext == L"7Z") {
                badgeCol = Color(255, 255, 159, 10);
            } else if (ext == L"EXE" || ext == L"MSI" || ext == L"BAT" || ext == L"CPP" || ext == L"PY") {
                badgeCol = Color(255, 48, 209, 88);
            }
        }
    }

    ShelfEntry entry;
    entry.fullPath = fullPath;
    entry.fileName = name;
    entry.extBadge = ext;
    entry.sizeText = isDir ? L"Folder" : FormatFileSize(fileSize);
    entry.badgeColor = badgeCol;
    entry.isDirectory = isDir;

    g_shelf.push_back(entry);
    g_shelfPage = (int)((g_shelf.size() - 1) / 2);
}

void CommitCustomTaskDraft() {
    std::wstring trimmed = g_taskDraft;
    while (!trimmed.empty() && trimmed.front() == L' ') trimmed.erase(trimmed.begin());
    while (!trimmed.empty() && trimmed.back() == L' ') trimmed.pop_back();

    if (!trimmed.empty()) {
        if (g_todos.size() >= 3) {
            bool removedDone = false;
            for (size_t i = 0; i < g_todos.size(); i++) {
                if (g_todos[i].done) {
                    g_todos.erase(g_todos.begin() + i);
                    removedDone = true;
                    break;
                }
            }
            if (!removedDone) {
                g_todos.erase(g_todos.begin());
            }
        }
        g_todos.push_back({ trimmed, false });
    }
    g_taskDraft.clear();
    g_isTypingTask = false;
}

void TriggerMediaAction(int actionCode, WORD fallbackVk) {
    bool hasSmtc = false;
    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        hasSmtc = g_media.hasSession;
        if (actionCode == 1) g_media.isPlaying = !g_media.isPlaying;
    }
    if (hasSmtc) {
        g_pendingMediaCommand.store(actionCode);
    } else {
        keybd_event(fallbackVk, 0, 0, 0);
        keybd_event(fallbackVk, 0, KEYEVENTF_KEYUP, 0);
    }
    g_sprBounceY.velocity = 1.4f;
}

void StepHubPageBy(int deltaPages) {
    int next = std::max(0, std::min((int)PAGE_COUNT - 1, g_hubPage + deltaPages));
    if (next != g_hubPage) {
        g_hubPage = next;
        g_sprPageScroll.target = (float)g_hubPage;
        g_sprBounceX.velocity = (deltaPages > 0 ? -2.2f : 2.2f) * g_cfg.uiScale;
    } else {
        g_sprPageScroll.target = (float)g_hubPage;
        g_sprBounceX.velocity = (deltaPages > 0 ? -4.5f : 4.5f) * g_cfg.uiScale;
    }
}

// ============================================================================
// SECTION 12: WINDOW PROCEDURE, TOUCHPAD GESTURES & INLINE TASK INPUT
// ============================================================================

LRESULT CALLBACK AnsaWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            DragAcceptFiles(hWnd, TRUE);
            pGetPointerInfo = (PFN_GetPointerInfo)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetPointerInfo");
            GESTURECONFIG gc = { GID_PAN, GC_PAN, 0 };
            SetGestureConfig(hWnd, 0, 1, &gc, sizeof(GESTURECONFIG));
            SetTimer(hWnd, 1, 13, NULL);
            SetTimer(hWnd, 2, 140, NULL);
            break;
        }

        case WM_CHAR: {
            if (g_isTypingTask && g_viewMode == VIEW_EXPANDED_HUB && g_hubPage == PAGE_TODO) {
                wchar_t ch = (wchar_t)wParam;
                if (ch == L'\r' || ch == L'\n') {
                    CommitCustomTaskDraft();
                } else if (ch == 0x1B) {
                    g_isTypingTask = false;
                    g_taskDraft.clear();
                } else if (ch == L'\b') {
                    if (!g_taskDraft.empty()) {
                        g_taskDraft.pop_back();
                    }
                } else if (ch >= 32 && g_taskDraft.size() < 44) {
                    g_taskDraft.push_back(ch);
                }
                return 0;
            }
            break;
        }

        case WM_APP + 1: {
            if (wParam == 1) {
                ActivateHudEvent(VIEW_HUD_VOLUME, L"Muted", L"0%", L"", 0.0f, Color(255, 255, 255, 255), 1800, 0);
                g_sprBounceX.velocity = -9.5f * g_cfg.uiScale;
            } else if (wParam == 3) {
                ActivateHudEvent(VIEW_HUD_VOLUME, L"Muted", L"0%", L"", 0.0f, Color(255, 255, 255, 255), 1800, 2);
                g_sprBounceX.velocity = -15.0f * g_cfg.uiScale;
            } else if (wParam == 4) {
                ActivateHudEvent(VIEW_HUD_VOLUME, L"Volume", L"100%", L"", 1.0f, Color(255, 255, 255, 255), 1800, 3);
                g_sprBounceX.velocity = +15.0f * g_cfg.uiScale;
            } else {
                int pct = (int)lParam;
                ActivateHudEvent(VIEW_HUD_VOLUME, L"Volume", std::to_wstring(pct) + L"%", L"", pct / 100.0f, Color(255, 255, 255, 255), 1800, 1);
                if (pct == 100) g_sprBounceX.velocity = +7.5f * g_cfg.uiScale;
            }
            break;
        }
        case WM_APP + 2: {
            int pct = (int)lParam;
            if (wParam == 1) {
                ActivateHudEvent(VIEW_HUD_BRIGHTNESS, L"Brightness", L"0%", L"", 0.0f, Color(255, 255, 255, 255), 1800, 1);
                g_sprBounceX.velocity = -15.0f * g_cfg.uiScale;
            } else if (wParam == 2) {
                ActivateHudEvent(VIEW_HUD_BRIGHTNESS, L"Brightness", L"100%", L"", 1.0f, Color(255, 255, 255, 255), 1800, 2);
                g_sprBounceX.velocity = +15.0f * g_cfg.uiScale;
            } else {
                ActivateHudEvent(VIEW_HUD_BRIGHTNESS, L"Brightness", std::to_wstring(pct) + L"%", L"", pct / 100.0f, Color(255, 255, 255, 255), 1800, 0);
                if (pct == 0) g_sprBounceX.velocity = -8.0f * g_cfg.uiScale;
                else if (pct == 100) g_sprBounceX.velocity = +8.0f * g_cfg.uiScale;
            }
            break;
        }
        case WM_APP + 3: {
            int pct = (int)lParam;
            bool isCharging = (wParam == 1 || wParam == 2);
            Color batCol = GetBatteryDynamicColor(pct, isCharging);
            if (wParam == 1) {
                ActivateHudEvent(VIEW_HUD_BATTERY, L"Charging", std::to_wstring(pct) + L"%", L"", pct / 100.0f, batCol, 3200, 1);
            } else if (wParam == 0) {
                ActivateHudEvent(VIEW_HUD_BATTERY, L"On Battery", std::to_wstring(pct) + L"%", L"", pct / 100.0f, batCol, 2800, 0);
            } else if (wParam == 2) {
                ActivateHudEvent(VIEW_HUD_BATTERY, L"Fully Charged", L"100%", L"", 1.0f, batCol, 3600, 1);
            } else if (wParam == 3) {
                ActivateHudEvent(VIEW_HUD_BATTERY, L"Low Battery", L"20%", L"", 0.20f, batCol, 3600, 0);
            }
            break;
        }
        case WM_APP + 4: {
            if (wParam == 1) {
                ActivateHudEvent(VIEW_HUD_NETWORK, L"Wi-Fi", L"Connected", L"", 1.0f, Color(255, 48, 209, 88), 2500, 1);
            } else {
                ActivateHudEvent(VIEW_HUD_NETWORK, L"Wi-Fi", L"Offline", L"", 0.0f, Color(255, 255, 69, 58), 3000, 0);
            }
            break;
        }
        case WM_APP + 5: {
            std::wstring* pPack = (std::wstring*)lParam;
            if (pPack) {
                size_t nl = pPack->find(L'\n');
                std::wstring title = (nl != std::wstring::npos) ? pPack->substr(0, nl) : *pPack;
                std::wstring body = (nl != std::wstring::npos) ? pPack->substr(nl + 1) : L"";
                delete pPack;
                ActivateHudEvent(VIEW_TOAST_NOTIFICATION, title, L"", body, 1.0f, GetAccentColorByIndex(g_cfg.accentIdx), 4000, 0);
            }
            break;
        }
        case WM_APP + 6: {
            std::wstring* pDevName = (std::wstring*)lParam;
            std::wstring devName = pDevName ? *pDevName : L"Bluetooth Device";
            delete pDevName;
            if (wParam == 1) {
                ActivateHudEvent(VIEW_HUD_BLUETOOTH, devName, L"Connected", L"", 1.0f, Color(255, 10, 132, 255), 3200, 1);
            } else {
                ActivateHudEvent(VIEW_HUD_BLUETOOTH, devName, L"Disconnected", L"", 0.0f, Color(255, 255, 159, 10), 2800, 0);
            }
            break;
        }

        case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wParam;
            UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
            for (UINT i = 0; i < fileCount; i++) {
                wchar_t pathBuf[MAX_PATH];
                if (DragQueryFileW(hDrop, i, pathBuf, MAX_PATH)) {
                    AddFileToShelf(pathBuf);
                }
            }
            DragFinish(hDrop);

            g_isTypingTask = false;
            g_viewMode = VIEW_EXPANDED_HUB;
            g_hubPage = PAGE_SHELFBOOK;
            g_sprPageScroll.target = (float)PAGE_SHELFBOOK;
            g_hud.expireTick = GetTickCount64() + 3600;
            float tw = 0.0f, th = 0.0f;
            ComputeTargetDimensions(VIEW_EXPANDED_HUB, tw, th);
            g_sprW.target = tw;
            g_sprH.target = th;
            g_sprBounceY.velocity = 1.8f;
            break;
        }

        case WM_MOUSEHWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (g_viewMode == VIEW_EXPANDED_HUB && !g_isTypingTask) {
                ULONGLONG now = GetTickCount64();
                if (now - g_lastTouchpadEventTick > 360) {
                    g_touchpadAccumX = 0.0f;
                }
                g_lastTouchpadEventTick = now;
                g_touchpadAccumX += (float)delta;

                float previewOffset = std::max(-0.38f, std::min(0.38f, g_touchpadAccumX / 90.0f));
                g_sprPageScroll.target = std::max(-0.22f, std::min((float)(PAGE_COUNT - 1) + 0.22f, (float)g_hubPage + previewOffset));

                if (std::abs(g_touchpadAccumX) >= 24.0f && (now - g_lastTouchpadSnapTick > 180)) {
                    int dir = (g_touchpadAccumX > 0.0f) ? 1 : -1;
                    StepHubPageBy(dir);
                    g_touchpadAccumX = 0.0f;
                    g_lastTouchpadSnapTick = now;
                }
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (g_viewMode == VIEW_HUD_VOLUME && g_pVolume) {
                float vol = 0.0f;
                g_pVolume->GetMasterVolumeLevelScalar(&vol);
                if (delta > 0 && vol >= 0.995f) {
                    PostMessageW(hWnd, WM_APP + 1, 4, 100);
                } else if (delta < 0 && vol <= 0.005f) {
                    PostMessageW(hWnd, WM_APP + 1, 3, 0);
                } else {
                    float nextVol = std::max(0.0f, std::min(1.0f, vol + (delta > 0 ? 0.05f : -0.05f)));
                    g_pVolume->SetMasterVolumeLevelScalar(nextVol, NULL);
                    g_pVolume->SetMute(nextVol <= 0.005f, NULL);
                }
                return 0;
            } else if (g_viewMode == VIEW_HUD_BRIGHTNESS) {
                int curB = (g_lastBrightness >= 0) ? g_lastBrightness : 50;
                if (delta > 0 && curB >= 100) {
                    PostMessageW(hWnd, WM_APP + 2, 2, 100);
                } else if (delta < 0 && curB <= 0) {
                    PostMessageW(hWnd, WM_APP + 2, 1, 0);
                } else {
                    int nextB = std::max(0, std::min(100, curB + (delta > 0 ? 5 : -5)));
                    if (SetBrightnessPercent(nextB)) {
                        g_lastBrightness = nextB;
                        PostMessageW(hWnd, WM_APP + 2, 0, nextB);
                    }
                }
                return 0;
            } else if (g_viewMode == VIEW_EXPANDED_HUB && !g_isTypingTask) {
                if (g_hubPage == PAGE_SHELFBOOK && g_shelf.size() > 2) {
                    g_shelfPage += (delta < 0) ? 1 : -1;
                } else {
                    ULONGLONG now = GetTickCount64();
                    if (now - g_lastTouchpadEventTick > 360) g_touchpadAccumY = 0.0f;
                    g_lastTouchpadEventTick = now;
                    g_touchpadAccumY += (float)delta;
                    if (std::abs(g_touchpadAccumY) >= 32.0f && (now - g_lastTouchpadSnapTick > 180)) {
                        int dir = (g_touchpadAccumY < 0.0f) ? 1 : -1;
                        StepHubPageBy(dir);
                        g_touchpadAccumY = 0.0f;
                        g_lastTouchpadSnapTick = now;
                    }
                }
                return 0;
            }
            break;
        }

        case WM_POINTERDOWN:
        case WM_POINTERUPDATE:
        case WM_POINTERUP: {
            if (pGetPointerInfo && g_viewMode == VIEW_EXPANDED_HUB && !g_isTypingTask) {
                UINT32 pid = LOWORD(wParam);
                POINTER_INFO_LOCAL pi = {};
                if (pGetPointerInfo(pid, &pi) && (pi.pointerType == 2 || pi.pointerType == 3)) {
                    if (msg == WM_POINTERDOWN) {
                        g_activePointerId = pid;
                        g_pointerStartPt = pi.ptPixelLocation;
                        g_pointerStartScroll = g_sprPageScroll.current;
                    } else if (msg == WM_POINTERUPDATE && pid == g_activePointerId) {
                        int dx = pi.ptPixelLocation.x - g_pointerStartPt.x;
                        float span = std::max(120.0f, g_sprW.current * 0.65f);
                        float nextScroll = g_pointerStartScroll - ((float)dx / span);
                        g_sprPageScroll.current = std::max(-0.25f, std::min((float)(PAGE_COUNT - 1) + 0.25f, nextScroll));
                        g_sprPageScroll.target = g_sprPageScroll.current;
                    } else if (msg == WM_POINTERUP && pid == g_activePointerId) {
                        g_activePointerId = 0;
                        int snapped = (int)std::round(g_sprPageScroll.current);
                        g_hubPage = std::max(0, std::min((int)PAGE_COUNT - 1, snapped));
                        g_sprPageScroll.target = (float)g_hubPage;
                    }
                    return 0;
                }
            }
            break;
        }

        case WM_GESTURE: {
            GESTUREINFO gi = { sizeof(GESTUREINFO) };
            if (GetGestureInfo((HGESTUREINFO)lParam, &gi)) {
                if (gi.dwID == GID_PAN && g_viewMode == VIEW_EXPANDED_HUB && !g_isTypingTask) {
                    if (gi.dwFlags & GF_BEGIN) {
                        g_gestureStartPt = { gi.ptsLocation.x, gi.ptsLocation.y };
                        g_dragStartPageScroll = g_sprPageScroll.current;
                    } else if (gi.dwFlags & GF_END) {
                        int snapped = (int)std::round(g_sprPageScroll.current);
                        g_hubPage = std::max(0, std::min((int)PAGE_COUNT - 1, snapped));
                        g_sprPageScroll.target = (float)g_hubPage;
                    } else {
                        int dx = gi.ptsLocation.x - g_gestureStartPt.x;
                        float span = std::max(120.0f, g_sprW.current * 0.65f);
                        float nextScroll = g_dragStartPageScroll - ((float)dx / span);
                        g_sprPageScroll.current = std::max(-0.25f, std::min((float)(PAGE_COUNT - 1) + 0.25f, nextScroll));
                        g_sprPageScroll.target = g_sprPageScroll.current;
                    }
                    CloseGestureInfoHandle((HGESTUREINFO)lParam);
                    return 0;
                }
                CloseGestureInfoHandle((HGESTUREINFO)lParam);
            }
            break;
        }

        case WM_KEYDOWN: {
            if (!g_isTypingTask && g_viewMode == VIEW_EXPANDED_HUB) {
                if (wParam == VK_RIGHT) {
                    StepHubPageBy(1);
                } else if (wParam == VK_LEFT) {
                    StepHubPageBy(-1);
                }
            }
            break;
        }

        case WM_MOUSEMOVE: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

            if (!g_isHovered) {
                g_isHovered = true;
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);

                if (g_viewMode == VIEW_IDLE) {
                    g_viewMode = VIEW_EXPANDED_HUB;
                    g_hud.expireTick = 0;
                    float tw = 0.0f, th = 0.0f;
                    ComputeTargetDimensions(VIEW_EXPANDED_HUB, tw, th);
                    g_sprW.target = tw;
                    g_sprH.target = th;
                }
            }

            if (g_isMouseDown && g_viewMode == VIEW_EXPANDED_HUB && !g_isTypingTask) {
                int dx = pt.x - g_mouseDownPos.x;
                int dy = pt.y - g_mouseDownPos.y;
                float dragThresh = 10.0f * g_cfg.uiScale;

                if (g_dragShelfSlot >= 0 && (std::abs(dx) > dragThresh || std::abs(dy) > dragThresh)) {
                    int itemIdx = g_shelfPage * 2 + g_dragShelfSlot;
                    g_isMouseDown = false;
                    g_dragShelfSlot = -1;
                    ReleaseCapture();
                    if (itemIdx >= 0 && itemIdx < (int)g_shelf.size()) {
                        StartExternalFileDrag(g_shelf[itemIdx].fullPath);
                    }
                    break;
                }

                if (!g_isDraggingPage && std::abs(dx) > dragThresh) {
                    g_isDraggingPage = true;
                    SetCapture(hWnd);
                }

                if (g_isDraggingPage) {
                    float pageSpanW = std::max(150.0f, g_sprW.current * 0.85f);
                    float nextScroll = g_dragStartPageScroll - ((float)dx / pageSpanW);
                    nextScroll = std::max(-0.25f, std::min((float)(PAGE_COUNT - 1) + 0.25f, nextScroll));
                    g_sprPageScroll.current = nextScroll;
                    g_sprPageScroll.target = nextScroll;
                    g_sprPageScroll.velocity = 0.0f;
                }
            }
            break;
        }

        case WM_MOUSELEAVE: {
            if (!g_isDraggingPage && !g_isTypingTask) {
                g_isHovered = false;
                g_isMouseDown = false;
                if (g_viewMode == VIEW_EXPANDED_HUB || g_viewMode == VIEW_SETTINGS_STUDIO) {
                    g_viewMode = VIEW_IDLE;
                    float tw = 0.0f, th = 0.0f;
                    ComputeTargetDimensions(VIEW_IDLE, tw, th);
                    g_sprW.target = tw;
                    g_sprH.target = th;
                }
            }
            break;
        }

        case WM_LBUTTONDOWN: {
            g_isMouseDown = true;
            g_isDraggingPage = false;
            g_mouseDownPos = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            g_dragStartPageScroll = g_sprPageScroll.current;
            g_dragShelfSlot = -1;

            if (g_viewMode == VIEW_EXPANDED_HUB && g_hubPage == PAGE_SHELFBOOK && !g_shelf.empty()) {
                for (int slot = 0; slot < 2; slot++) {
                    if (PtInRect(&g_hit.shelfSlotOpen[slot], g_mouseDownPos) && !PtInRect(&g_hit.shelfSlotDel[slot], g_mouseDownPos)) {
                        g_dragShelfSlot = slot;
                        break;
                    }
                }
            }
            break;
        }

        case WM_LBUTTONUP: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            bool wasDraggingPage = g_isDraggingPage;
            g_isMouseDown = false;
            g_isDraggingPage = false;
            g_dragShelfSlot = -1;
            if (GetCapture() == hWnd) ReleaseCapture();

            if (wasDraggingPage && g_viewMode == VIEW_EXPANDED_HUB) {
                int snapped = (int)std::round(g_sprPageScroll.current);
                g_hubPage = std::max(0, std::min((int)PAGE_COUNT - 1, snapped));
                g_sprPageScroll.target = (float)g_hubPage;
                break;
            }

            if (g_viewMode == VIEW_SETTINGS_STUDIO) {
                if (PtInRect(&g_hit.setClose, pt)) {
                    g_viewMode = VIEW_EXPANDED_HUB;
                    float tw = 0.0f, th = 0.0f;
                    ComputeTargetDimensions(VIEW_EXPANDED_HUB, tw, th);
                    g_sprW.target = tw;
                    g_sprH.target = th;
                } else if (PtInRect(&g_hit.setTabStyle, pt)) {
                    g_settingsTab = TAB_STYLE;
                } else if (PtInRect(&g_hit.setTabModules, pt)) {
                    g_settingsTab = TAB_MODULES;
                } else if (PtInRect(&g_hit.setTabColors, pt)) {
                    g_settingsTab = TAB_COLORS;
                } else if (g_settingsTab == TAB_STYLE) {
                    if (PtInRect(&g_hit.optBtn[0], pt)) {
                        g_cfg.isNotch = !g_cfg.isNotch;
                    } else if (PtInRect(&g_hit.optBtn[1], pt)) {
                        if (g_cfg.uiScale < 1.10f) g_cfg.uiScale = 1.15f;
                        else if (g_cfg.uiScale < 1.25f) g_cfg.uiScale = 1.30f;
                        else if (g_cfg.uiScale < 1.45f) g_cfg.uiScale = 1.50f;
                        else g_cfg.uiScale = 1.00f;
                        float tw = 0.0f, th = 0.0f;
                        ComputeTargetDimensions(VIEW_SETTINGS_STUDIO, tw, th);
                        g_sprW.target = tw;
                        g_sprH.target = th;
                    } else if (PtInRect(&g_hit.optBtn[2], pt)) {
                        g_cfg.theme = (ThemeType)(((int)g_cfg.theme + 1) % 3);
                    } else if (PtInRect(&g_hit.optBtn[3], pt)) {
                        g_cfg.springPreset = (g_cfg.springPreset == SPRING_NORMAL) ? SPRING_BOUNCY : SPRING_NORMAL;
                    } else if (PtInRect(&g_hit.optBtn[4], pt)) {
                        g_cfg.hideFullscreen = !g_cfg.hideFullscreen;
                    } else if (PtInRect(&g_hit.optBtn[5], pt)) {
                        if (g_cfg.idleOpacity == 100) g_cfg.idleOpacity = 85;
                        else if (g_cfg.idleOpacity == 85) g_cfg.idleOpacity = 70;
                        else g_cfg.idleOpacity = 100;
                    }
                } else if (g_settingsTab == TAB_MODULES) {
                    if (PtInRect(&g_hit.optBtn[0], pt)) {
                        g_cfg.idleDisplay = (IdleDisplayType)(((int)g_cfg.idleDisplay + 1) % 4);
                        g_cfg.minimalMode = (g_cfg.idleDisplay == IDLE_MINIMAL);
                    } else if (PtInRect(&g_hit.optBtn[1], pt)) {
                        g_cfg.minimalMode = !g_cfg.minimalMode;
                    } else if (PtInRect(&g_hit.optBtn[2], pt)) {
                        g_cfg.enableMedia = !g_cfg.enableMedia;
                    } else if (PtInRect(&g_hit.optBtn[3], pt)) {
                        g_cfg.edgeHoverExpand = !g_cfg.edgeHoverExpand;
                    } else if (PtInRect(&g_hit.optBtn[4], pt)) {
                        g_cfg.enableBluetooth = !g_cfg.enableBluetooth;
                    } else if (PtInRect(&g_hit.optBtn[5], pt)) {
                        g_cfg.enableNotifications = !g_cfg.enableNotifications;
                    }
                } else if (g_settingsTab == TAB_COLORS) {
                    for (int c = 0; c < 6; c++) {
                        if (PtInRect(&g_hit.colorSwatches[c], pt)) {
                            g_cfg.accentIdx = c;
                            break;
                        }
                    }
                }
            } else if (g_viewMode == VIEW_EXPANDED_HUB) {
                for (int d = 0; d < PAGE_COUNT; d++) {
                    if (PtInRect(&g_hit.pageDots[d], pt)) {
                        g_isTypingTask = false;
                        g_hubPage = d;
                        g_sprPageScroll.target = (float)d;
                        return 0;
                    }
                }

                if (PtInRect(&g_hit.btnSettings, pt)) {
                    g_isTypingTask = false;
                    g_viewMode = VIEW_SETTINGS_STUDIO;
                    float tw = 0.0f, th = 0.0f;
                    ComputeTargetDimensions(VIEW_SETTINGS_STUDIO, tw, th);
                    g_sprW.target = tw;
                    g_sprH.target = th;
                } else if (g_hubPage == PAGE_MEDIA) {
                    if (PtInRect(&g_hit.btnPrev, pt)) {
                        TriggerMediaAction(3, VK_MEDIA_PREV_TRACK);
                    } else if (PtInRect(&g_hit.btnPlay, pt)) {
                        TriggerMediaAction(1, VK_MEDIA_PLAY_PAUSE);
                    } else if (PtInRect(&g_hit.btnNext, pt)) {
                        TriggerMediaAction(2, VK_MEDIA_NEXT_TRACK);
                    } else if (PtInRect(&g_hit.barProgress, pt)) {
                        float ratio = (float)(pt.x - g_hit.barProgress.left) / (float)std::max(1L, g_hit.barProgress.right - g_hit.barProgress.left);
                        ratio = std::max(0.0f, std::min(1.0f, ratio));
                        std::lock_guard<std::mutex> lock(g_mediaMutex);
                        if (g_media.hasSession && g_media.endTicks > 0) {
                            g_media.progress = ratio;
                            g_media.posTicks = (INT64)(g_media.endTicks * ratio);
                            g_pendingSeekTicks.store(g_media.posTicks);
                        }
                    }
                } else if (g_hubPage == PAGE_SHELFBOOK && !g_shelf.empty()) {
                    if (PtInRect(&g_hit.shelfClearAll, pt)) {
                        g_shelf.clear();
                        g_shelfPage = 0;
                    } else if (PtInRect(&g_hit.shelfPrevPage, pt)) {
                        g_shelfPage--;
                    } else if (PtInRect(&g_hit.shelfNextPage, pt)) {
                        g_shelfPage++;
                    } else {
                        for (int slot = 0; slot < 2; slot++) {
                            int itemIdx = g_shelfPage * 2 + slot;
                            if (itemIdx >= (int)g_shelf.size()) break;
                            if (PtInRect(&g_hit.shelfSlotDel[slot], pt)) {
                                g_shelf.erase(g_shelf.begin() + itemIdx);
                                break;
                            } else if (PtInRect(&g_hit.shelfSlotOpen[slot], pt)) {
                                ShellExecuteW(NULL, L"open", g_shelf[itemIdx].fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                                break;
                            }
                        }
                    }
                } else if (g_hubPage == PAGE_CALENDAR) {
                    for (int d = 0; d < 7; d++) {
                        if (PtInRect(&g_hit.calDays[d], pt)) {
                            g_selectedCalDayOffset = d - 3;
                            break;
                        }
                    }
                    if (PtInRect(&g_hit.calEventPill, pt)) {
                        g_calEventCycle++;
                    }
                } else if (g_hubPage == PAGE_TODO) {
                    if (g_isTypingTask) {
                        if (PtInRect(&g_hit.todoInputSave, pt)) {
                            CommitCustomTaskDraft();
                        } else if (PtInRect(&g_hit.todoInputCancel, pt)) {
                            g_isTypingTask = false;
                            g_taskDraft.clear();
                        }
                    } else {
                        if (PtInRect(&g_hit.todoAddBtn, pt)) {
                            g_isTypingTask = true;
                            g_taskDraft.clear();
                            SetForegroundWindow(hWnd);
                            SetFocus(hWnd);
                        } else if (PtInRect(&g_hit.todoClearDone, pt)) {
                            for (int i = (int)g_todos.size() - 1; i >= 0; i--) {
                                if (g_todos[i].done) g_todos.erase(g_todos.begin() + i);
                            }
                        } else {
                            for (int t = 0; t < std::min(3, (int)g_todos.size()); t++) {
                                if (PtInRect(&g_hit.todoRowDel[t], pt)) {
                                    g_todos.erase(g_todos.begin() + t);
                                    break;
                                } else if (PtInRect(&g_hit.todoRows[t], pt)) {
                                    g_todos[t].done = !g_todos[t].done;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        case WM_RBUTTONUP: {
            g_isTypingTask = false;
            if (g_viewMode != VIEW_SETTINGS_STUDIO) {
                g_viewMode = VIEW_SETTINGS_STUDIO;
                g_hud.expireTick = 0;
                float tw = 0.0f, th = 0.0f;
                ComputeTargetDimensions(VIEW_SETTINGS_STUDIO, tw, th);
                g_sprW.target = tw;
                g_sprH.target = th;
            } else {
                g_viewMode = VIEW_IDLE;
                float tw = 0.0f, th = 0.0f;
                ComputeTargetDimensions(VIEW_IDLE, tw, th);
                g_sprW.target = tw;
                g_sprH.target = th;
            }
            break;
        }

        case WM_TIMER: {
            if (wParam == 1) {
                if (g_isHiddenByFullscreen) return 0;

                if (g_hud.expireTick > 0 && GetTickCount64() > g_hud.expireTick && !g_isTypingTask) {
                    g_hud.expireTick = 0;
                    g_viewMode = g_isHovered ? VIEW_EXPANDED_HUB : VIEW_IDLE;
                }

                if (g_lastTouchpadEventTick > 0 && (GetTickCount64() - g_lastTouchpadEventTick > 340)) {
                    g_lastTouchpadEventTick = 0;
                    g_touchpadAccumX = 0.0f;
                    g_touchpadAccumY = 0.0f;
                    if (!g_isDraggingPage) {
                        g_sprPageScroll.target = (float)g_hubPage;
                    }
                }

                float tw = 0.0f, th = 0.0f;
                ComputeTargetDimensions(g_viewMode, tw, th);
                g_sprW.target = tw;
                g_sprH.target = th;

                bool isMediaPlaying = IsMediaCurrentlyPlaying();
                bool wantGlow = isMediaPlaying || (g_viewMode != VIEW_IDLE);
                g_sprGlow.target = wantGlow ? 1.0f : 0.0f;

                float stiff = 0.145f, damp = 0.77f;
                GetSpringConstants(stiff, damp);
                g_sprW.Step(stiff, damp);
                g_sprH.Step(stiff, damp);
                g_sprGlow.Step(0.11f, 0.80f);
                g_sprBounceY.Step(0.22f, 0.68f);
                g_sprBounceX.Step(0.32f, 0.58f);
                if (!g_isDraggingPage && g_activePointerId == 0) {
                    g_sprPageScroll.Step(0.18f, 0.74f);
                }

                g_hud.animProgress += (g_hud.progress - g_hud.animProgress) * 0.24f;
                g_oneUiPulsePhase += 0.11f;

                float targetWaveAmp = isMediaPlaying ? 1.0f : 0.0f;
                g_waveAmplitude += (targetWaveAmp - g_waveAmplitude) * 0.14f;
                if (g_waveAmplitude > 0.01f) {
                    g_wavePhase += 0.16f;
                }

                float peak = g_audioPeak;
                static float eqPhase = 0.0f;
                eqPhase += 0.24f;
                float weights[5] = { 0.78f, 1.15f, 1.35f, 1.10f, 0.75f };
                bool activeAudio = isMediaPlaying || (peak > 0.006f);
                for (int i = 0; i < 5; i++) {
                    float wave1 = 0.5f + 0.5f * sinf(eqPhase + i * 1.35f);
                    float wave2 = 0.5f + 0.5f * cosf(eqPhase * 1.65f + i * 2.10f);
                    float rhythm = wave1 * 0.65f + wave2 * 0.35f;
                    float targetBar = 0.0f;
                    if (activeAudio) {
                        float effectiveLevel = std::max(isMediaPlaying ? 0.42f : 0.0f, std::min(1.0f, peak * 1.85f));
                        targetBar = std::min(1.0f, effectiveLevel * weights[i] * (0.35f + 0.85f * rhythm));
                    }
                    g_eqBars[i] += (targetBar - g_eqBars[i]) * 0.32f;
                }

                RenderAnsaDynamicIsland(hWnd);
            } else if (wParam == 2) {
                if (g_cfg.hideFullscreen && !g_isTypingTask) {
                    bool fs = IsForegroundWindowFullscreen();
                    if (fs && !g_isHiddenByFullscreen) {
                        g_isHiddenByFullscreen = true;
                        ShowWindow(hWnd, SW_HIDE);
                    } else if (!fs && g_isHiddenByFullscreen) {
                        g_isHiddenByFullscreen = false;
                        ShowWindow(hWnd, SW_SHOWNA);
                    }
                } else if (g_isHiddenByFullscreen) {
                    g_isHiddenByFullscreen = false;
                    ShowWindow(hWnd, SW_SHOWNA);
                }

                if (!g_isHiddenByFullscreen && g_cfg.edgeHoverExpand && g_viewMode == VIEW_IDLE && !g_isHovered) {
                    POINT pt;
                    if (GetCursorPos(&pt)) {
                        int screenW = GetSystemMetrics(SM_CXSCREEN);
                        int centerX = screenW / 2;
                        int zoneHalfW = (int)(g_sprW.current * 0.5f + 30.0f * g_cfg.uiScale);
                        if (pt.y <= 3 && std::abs(pt.x - centerX) <= zoneHalfW) {
                            g_viewMode = VIEW_EXPANDED_HUB;
                            g_hud.expireTick = GetTickCount64() + 2800;
                        }
                    }
                }
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI IslandUiThread(LPVOID lpParam) {
    OleInitialize(NULL);
    GdiplusStartupInput gdiInput;
    GdiplusStartup(&g_gdiplusToken, &gdiInput, NULL);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = AnsaWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"AnsaDynamicIslandDevApoxClass";
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClassW(&wc);

    float initW = 0.0f, initH = 0.0f;
    ComputeTargetDimensions(VIEW_IDLE, initW, initH);
    g_sprW.Reset(initW);
    g_sprH.Reset(initH);
    g_sprGlow.Reset(0.0f);
    g_sprBounceY.Reset(0.0f);
    g_sprBounceX.Reset(0.0f);
    g_sprPageScroll.Reset(0.0f);

    g_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        wc.lpszClassName, L"Ansa Dynamic Island",
        WS_POPUP | WS_VISIBLE,
        0, 0, (int)initW + 80, (int)initH + 56,
        NULL, NULL, wc.hInstance, NULL
    );

    ActivateHudEvent(VIEW_TOAST_NOTIFICATION, L"Ansa Dynamic Island", L"", L"Made by dev-apox", 1.0f, GetAccentColorByIndex(g_cfg.accentIdx), 3600, 0);

    MSG msg;
    while (g_running.load() && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        if (g_media.pAlbumArt) {
            delete g_media.pAlbumArt;
            g_media.pAlbumArt = NULL;
        }
    }

    GdiplusShutdown(g_gdiplusToken);
    OleUninitialize();
    return 0;
}

// ============================================================================
// SECTION 13: WINDHAWK LIFECYCLE ENTRY POINTS
// ============================================================================

BOOL Wh_ModInit() {
    LoadWindhawkConfiguration();
    g_running.store(true);

    g_hWorkerThread = CreateThread(NULL, 0, BackgroundTelemetryWorker, NULL, 0, NULL);
    g_hUiThread = CreateThread(NULL, 0, IslandUiThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    g_running.store(false);

    if (g_hWnd) {
        PostMessageW(g_hWnd, WM_CLOSE, 0, 0);
    }
    if (g_hUiThread) {
        WaitForSingleObject(g_hUiThread, 2500);
        CloseHandle(g_hUiThread);
        g_hUiThread = NULL;
    }
    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, 2500);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = NULL;
    }

    UnregisterClassW(L"AnsaDynamicIslandDevApoxClass", GetModuleHandle(NULL));
}

void Wh_ModSettingsChanged() {
    LoadWindhawkConfiguration();
}
