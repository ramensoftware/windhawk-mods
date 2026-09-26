// ==WindhawkMod==
// @id              audio-level-hud
// @name            Audio Level HUD
// @description     A small, always-on-top, Fluent 2-styled overlay showing live VU meters for mic input and system output.
// @version         1.0.0
// @author          AKS HAY
// @github          https://github.com/sysakshay
// @license         GPL-3.0
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32 -lmmdevapi -ld3d11 -ldxgi -ldcomp -ld2d1 -ldwrite -luser32 -lgdi32 -ldwmapi -lcomctl32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Audio Level HUD

A lightweight, always-on-top, Fluent 2-styled desktop overlay for Windows that
displays real-time VU meters for both **Microphone Input** and **System Audio
Output**.

The microphone meter opens a capture stream while it is visible. Windows will
show the microphone privacy indicator during that time. Hiding the HUD or
choosing System Output Only closes that stream.

![Preview](https://raw.githubusercontent.com/sysakshay/audio-level-hud-wh/main/previews/preview1.png)
![Settings
Preview](https://raw.githubusercontent.com/sysakshay/audio-level-hud-wh/main/previews/preview2.png)

## Key Features
- **DirectComposition & DXGI SwapChain**: Hardware-accelerated presentation with
`DXGI_ALPHA_MODE_PREMULTIPLIED` for pixel-perfect anti-aliased rounded corners
without GDI artifacts or corner leak lines.
- **Liquid Glass Design**: Translucent card styling, specular rim
lighting, top glass reflection sheen, and 3D glossy level meter pills.
- **Dual Real-time VU Meters**: WASAPI peak meters for mic capture and system
audio output.
- **Overlay Customization**: Corner snapping, custom drag-and-drop positioning,
opacity, and click-through mode.
- **Optional Global Hotkeys**: Set shortcuts in Windhawk settings to show/hide
the HUD or toggle click-through mode. Both are disabled by default.

---
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hud_scale: '100'
  $name: HUD Scale
  $description: Size of the HUD overlay.
  $options:
    - '25': 25% - Tiny
    - '35': 35% - Extra Small
    - '50': 50% - Small
    - '65': 65% - Compact
    - '75': 75% - Medium
    - '85': 85% - Slightly Smaller
    - '100': 100% - Default (Full Size)
- enable_peak_hold: true
  $name: Show Peak Hold Indicator
  $description: Display a persistent peak level marker line on the VU meters.
- peak_hold_duration: '300'
  $name: Peak Hold Duration
  $description: Delay before peak markers begin falling back.
  $options:
    - '200': 200 ms (Fast / Responsive)
    - '300': 300 ms (Balanced - Studio Standard)
    - '500': 500 ms (Smooth)
    - '1000': 1000 ms (Long Hold)
- show_meters: both
  $name: Show Audio Meters
  $description: Choose which audio meters to display.
  $options:
    - both: Show Both Meters
    - mic: Show Microphone Only
    - system: Show System Output Only
- show_labels: true
  $name: Show Text Labels
  $description: Display Mic and System text labels next to the icons.
- position: top-right
  $name: Screen Position
  $description: Screen corner position to dock the overlay HUD.
  $options:
    - top-right: Top Right
    - top-left: Top Left
    - bottom-right: Bottom Right
    - bottom-left: Bottom Left
    - custom: Custom (drag overlay to desired location)
- card_opacity: '88'
  $name: Card Opacity
  $description: Opacity of the HUD background (%)
  $options:
    - '0': 0% - Fully Transparent
    - '20': 20% - Ultra Transparent
    - '40': 40% - Very Light
    - '60': 60% - Light
    - '75': 75% - Medium
    - '88': 88% - Default
    - '100': 100% - Opaque
- click_through: true
  $name: Click-Through Mode
  $description: Allow mouse clicks to pass through the HUD to underlying windows.
- toggle_hotkey: ""
  $name: Toggle Hotkey
  $description: Global shortcut string to show/hide the HUD (e.g., Ctrl+Shift+M, Alt+Shift+A).
- clickthrough_hotkey: ""
  $name: Click-Through Hotkey
  $description: Global shortcut string to toggle Click-Through mode on/off (e.g., Ctrl+Shift+C, Alt+Shift+C).
- fps_limit: '30'
  $name: Refresh Rate (FPS)
  $description: Target update frequency for the level meters.
  $options:
    - '15': 15 FPS (Ultra Low Overhead)
    - '30': 30 FPS (Balanced - Recommended)
    - '60': 60 FPS (Ultra Smooth)
- color_theme: fluent
  $name: Color Theme
  $description: Visual theme for the VU level bars.
  $options:
    - fluent: Fluent Standard (Green & Amber)
    - neon: Cyber Neon (Cyan & Magenta)
    - emerald: Emerald Glow (Green & Lime)
    - sunset: Sunset Amber (Orange & Gold)
    - monochrome: Minimal Silver
- layout: horizontal
  $name: HUD Layout Orientation
  $description: Choose between traditional horizontal bar HUD or vertical sidebar dock.
  $options:
    - horizontal: Horizontal Bar (Default)
    - vertical: Vertical Sidebar
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <audiopolicy.h>
#include <commctrl.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi1_2.h>
#include <endpointvolume.h>
#include <math.h>
#include <mmdeviceapi.h>
#include <propidl.h>
#include <string>
#include <strsafe.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windows.h>
#include <windowsx.h>

#define HUD_WINDOW_CLASS L"Windhawk_AudioLevelHud_Class"
#define FLYOUT_WINDOW_CLASS L"Windhawk_AudioLevelHud_Flyout"
#define HUD_TIMER_ID 1001
#define HUD_HOTKEY_ID 2001
#define HUD_CLICKTHRU_HOTKEY_ID 2002
#define WM_HUD_RELOAD_SETTINGS (WM_USER + 101)
// Flyout control IDs
#define FLYOUT_CTRL_OPACITY 3001
#define FLYOUT_CTRL_OP_LBL 3002
#define FLYOUT_CTRL_BOTH 3003
#define FLYOUT_CTRL_MIC_ONLY 3004
#define FLYOUT_CTRL_SYS_ONLY 3005
#define FLYOUT_CTRL_CLICKTHRU 3006
#define FLYOUT_CTRL_MIC_COMBO 3007
#define FLYOUT_CTRL_MIC_LBL 3008
#define FLYOUT_CTRL_SCALE 3009
#define FLYOUT_CTRL_SCALE_LBL 3010
#define FLYOUT_CTRL_PEAKHOLD 3011
#define FLYOUT_CTRL_SHOWLABELS 3012
#define FLYOUT_CTRL_LAYOUT_HORZ 3013
#define FLYOUT_CTRL_LAYOUT_VERT 3014

#ifndef PKEY_Device_FriendlyName
static const PROPERTYKEY PKEY_Device_FriendlyName_Local = {
    {0xa45c254e,
     0xdf1c,
     0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};
#define PKEY_Device_FriendlyName PKEY_Device_FriendlyName_Local
#endif

#ifndef PKEY_Device_DeviceDesc
static const PROPERTYKEY PKEY_Device_DeviceDesc_Local = {
    {0xa45c254e,
     0xdf1c,
     0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    2};
#define PKEY_Device_DeviceDesc PKEY_Device_DeviceDesc_Local
#endif

// Interface definition for IAudioMeterInformation in case endpointvolume.h only
// forward-declares it
#ifndef __IAudioMeterInformation_INTERFACE_DEFINED__
#define __IAudioMeterInformation_INTERFACE_DEFINED__
struct IAudioMeterInformation : public IUnknown {
  virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float *pfPeak) = 0;
  virtual HRESULT STDMETHODCALLTYPE
  GetMeteringChannelCount(UINT *pnChannelCount) = 0;
  virtual HRESULT STDMETHODCALLTYPE
  GetChannelsPeakValues(UINT32 u32ChannelCount, float *afPeakValues) = 0;
  virtual HRESULT STDMETHODCALLTYPE
  QueryHardwareSupport(DWORD *pdwHardwareSupportMask) = 0;
};
#endif

// Undocumented SetWindowCompositionAttribute definitions for native Windows
// Fluent Acrylic blur
typedef enum _ACCENT_STATE {
  ACCENT_DISABLED = 0,
  ACCENT_ENABLE_GRADIENT = 1,
  ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
  ACCENT_ENABLE_BLURBEHIND = 3,
  ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
  ACCENT_INVALID_STATE = 5
} ACCENT_STATE;

typedef struct _ACCENT_POLICY {
  ACCENT_STATE AccentState;
  DWORD AccentFlags;
  DWORD GradientColor;
  DWORD AnimationId;
} ACCENT_POLICY;

typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
  DWORD Attribute;
  PVOID pvData;
  SIZE_T cbData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef BOOL(WINAPI *pfnSetWindowCompositionAttribute)(
    HWND, WINDOWCOMPOSITIONATTRIBDATA *);

// Structure for Audio Endpoint metering
struct AudioEndpointTracker {
  IMMDevice *pDevice = nullptr;
  IAudioClient *pAudioClient = nullptr;
  IAudioMeterInformation *pMeter = nullptr;
  IAudioEndpointVolume *pVolume = nullptr;
  float currentLevel = 0.0f;
  float peakHold = 0.0f;
  DWORD peakHoldTime = 0;
  DWORD lastUpdateTick = 0;
  DWORD clipTime = 0;
  DWORD lastRetryTick = 0;
  BOOL isMuted = FALSE;
  BOOL isClipping = FALSE;

  void Release() {
    if (pAudioClient) {
      pAudioClient->Stop();
      pAudioClient->Release();
      pAudioClient = nullptr;
    }
    if (pMeter) {
      pMeter->Release();
      pMeter = nullptr;
    }
    if (pVolume) {
      pVolume->Release();
      pVolume = nullptr;
    }
    if (pDevice) {
      pDevice->Release();
      pDevice = nullptr;
    }
  }
};

// Global Mod Settings
struct ModSettings {
  WCHAR position[32] = L"top-right";
  int opacity = 88;
  BOOL showMic = TRUE;
  BOOL showSystem = TRUE;
  BOOL showLabels = TRUE;
  BOOL clickThrough = TRUE;
  WCHAR toggleHotkey[64] = L"";
  WCHAR clickthroughHotkey[64] = L"";
  int fpsLimit = 30;
  WCHAR colorTheme[32] = L"fluent";
  WCHAR micDevice[256] = L"default";
  int hudScale = 100;
  BOOL enablePeakHold = TRUE;
  int peakHoldDuration = 300;
  WCHAR layout[32] = L"horizontal";
};

static ModSettings g_Settings;
static std::atomic<HWND> g_hHudWnd = NULL;
static HWND g_hFlyoutWnd = NULL; // Settings flyout popup window
static HBRUSH g_hFlyoutBgBrush =
    NULL; // Reusable dark bg brush for flyout controls
static HFONT g_hFlyoutControlFont = NULL;
static HANDLE g_hHudThread = NULL;
static HANDLE g_hStopEvent = NULL;
static DWORD g_dwThreadId = 0;
static BOOL g_bHudVisible = TRUE;

static IMMDeviceEnumerator *g_pEnumerator = nullptr;
static AudioEndpointTracker g_MicTracker;
static AudioEndpointTracker g_SystemTracker;

// Direct3D11 / DXGI / DirectComposition / Direct2D COM Pointers
static ID3D11Device *g_pD3DDevice = nullptr;
static ID3D11DeviceContext *g_pD3DContext = nullptr;
static IDXGISwapChain1 *g_pSwapChain = nullptr;
static ID2D1Factory1 *g_pD2DFactory1 = nullptr;
static ID2D1Device *g_pD2DDevice = nullptr;
static ID2D1DeviceContext *g_pD2DContext = nullptr;
static ID2D1Bitmap1 *g_pD2DTargetBitmap = nullptr;
static IDCompositionDevice *g_pDCompDevice = nullptr;
static IDCompositionTarget *g_pDCompTarget = nullptr;
static IDCompositionVisual *g_pDCompVisual = nullptr;
static IDWriteFactory *g_pDWriteFactory = nullptr;
static IDWriteTextFormat *g_pIconFontFormat = nullptr;
static IDWriteTextFormat *g_pLabelFontFormat = nullptr;
static IDWriteTextFormat *g_pLabelFontFormatVert = nullptr;
static IDWriteTextFormat *g_pValueFontFormat = nullptr;
static IDWriteTextFormat *g_pValueFontFormatVert = nullptr;

// Helper COM GUID definitions
static const CLSID CLSID_MMDevEnum = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDevEnum = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioMeterInfo = {
    0xC02216F6,
    0x8C67,
    0x4B5B,
    {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};
static const IID IID_IAudioEndVol = __uuidof(IAudioEndpointVolume);

// Forward declarations
static LRESULT CALLBACK HudWndProc(HWND hWnd, UINT message, WPARAM wParam,
                                   LPARAM lParam);
static DWORD WINAPI HudThreadProc(LPVOID lpParam);
static void ApplyCornerPreference(HWND hWnd);
static void LoadModSettings();
static void PositionHudWindow();
static void InitAudioTracker(EDataFlow dataFlow, AudioEndpointTracker &tracker);
static void UpdateAudioLevels();
static void RegisterGlobalHotkey();
static void UnregisterGlobalHotkey();
static HRESULT InitDirectComposition(HWND hWnd);
static void CleanupDirectComposition();
static void RenderHud();
// Flyout
static LRESULT CALLBACK FlyoutWndProc(HWND hWnd, UINT message, WPARAM wParam,
                                      LPARAM lParam);
static void ShowSettingsFlyout();
static void DismissFlyout();

// Logarithmic Audio dB VU Meter Ratio Mapping (-60 dB to 0 dB mapped to 0.0
// to 1.0 with DAW-style expansion in the -20 dB to 0 dB range)
static float LinearToMeterRatio(float linearLevel) {
  if (linearLevel <= 0.0001f)
    return 0.0f; // -60 dB floor
  float dB = 20.0f * log10f(linearLevel);
  if (dB < -60.0f)
    return 0.0f;
  if (dB >= 0.0f)
    return 1.0f;

  // DAW-style piecewise curve:
  // -60 dB to -20 dB (40 dB range) maps to 0.0 .. 0.45 ratio
  // -20 dB to 0 dB   (20 dB range) maps to 0.45 .. 1.0 ratio
  if (dB < -20.0f) {
    return 0.45f * ((dB + 60.0f) / 40.0f);
  } else {
    return 0.45f + 0.55f * ((dB + 20.0f) / 20.0f);
  }
}

typedef enum PreferredAppMode {
  Default,
  AllowDark,
  ForceDark,
  ForceLight,
  Max
} PreferredAppMode;

typedef PreferredAppMode(WINAPI *pfnSetPreferredAppMode)(
    PreferredAppMode appMode);
typedef BOOL(WINAPI *pfnAllowDarkModeForWindow)(HWND hWnd, BOOL allow);

static void EnableDarkModeForControl(HWND hCtrl) {
  if (!hCtrl)
    return;
  HMODULE hUxtheme = GetModuleHandleW(L"uxtheme.dll");
  if (hUxtheme) {
    pfnAllowDarkModeForWindow pAllowDarkModeForWindow =
        (pfnAllowDarkModeForWindow)GetProcAddress(hUxtheme,
                                                  MAKEINTRESOURCEA(133));
    if (pAllowDarkModeForWindow) {
      pAllowDarkModeForWindow(hCtrl, TRUE);
    }
  }
  SetWindowTheme(hCtrl, L"DarkMode_Explorer", NULL);
}

// Let DirectComposition supply the transparent rounded corners
static void ApplyCornerPreference(HWND hWnd) {
  if (!hWnd)
    return;

  // DWMWCP_DONOTROUND (1): Don't let DWM apply its own ~8px corner clip.
  // With WS_EX_NOREDIRECTIONBITMAP + DirectComposition
  // DXGI_ALPHA_MODE_PREMULTIPLIED, D2D's transparent (0,0,0,0) corner pixels
  // define the visual shape naturally. DWMWCP_ROUND (2) conflicts by imposing a
  // ~8px DWM clip over D2D's 16px curves, causing rectangular corner bleed
  // artifacts.
  DWORD cornerPref = 1; // DWMWCP_DONOTROUND = 1
  DwmSetWindowAttribute(hWnd, 33, &cornerPref, sizeof(cornerPref));

  // NOTE: SetWindowCompositionAttribute is intentionally NOT used here.
  // We use WS_EX_NOREDIRECTIONBITMAP + DirectComposition SwapChain1 with
  // DXGI_ALPHA_MODE_PREMULTIPLIED. Using ACCENT_ENABLE_ACRYLICBLURBEHIND
  // alongside DirectComposition causes the DWM to apply acrylic to the full
  // rectangular window bounds, bleeding opaque blur outside the D2D rounded
  // rect and producing visible square corner artifacts. DirectComposition
  // handles compositing natively via premultiplied alpha Ã¢â‚¬â€ no accent policy
  // needed.
}

static IMMDevice *FindDeviceByNameOrId(EDataFlow flow,
                                       const std::wstring &targetName) {
  if (!g_pEnumerator)
    return nullptr;
  IMMDeviceCollection *pCol = nullptr;
  if (FAILED(g_pEnumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE,
                                               &pCol)) ||
      !pCol) {
    return nullptr;
  }

  UINT count = 0;
  pCol->GetCount(&count);
  IMMDevice *pMatchedDevice = nullptr;

  std::wstring targetLower = targetName;
  std::transform(targetLower.begin(), targetLower.end(), targetLower.begin(),
                 ::towlower);

  for (UINT i = 0; i < count; i++) {
    IMMDevice *pCandidate = nullptr;
    if (SUCCEEDED(pCol->Item(i, &pCandidate)) && pCandidate) {
      // 1. Check Device ID match
      LPWSTR pwszID = nullptr;
      if (SUCCEEDED(pCandidate->GetId(&pwszID)) && pwszID) {
        std::wstring idLower = pwszID;
        CoTaskMemFree(pwszID);
        std::transform(idLower.begin(), idLower.end(), idLower.begin(),
                       ::towlower);
        if (idLower == targetLower) {
          pMatchedDevice = pCandidate;
          break;
        }
      }

      // 2. Check Friendly Name & Device Desc match
      IPropertyStore *pProps = nullptr;
      if (SUCCEEDED(pCandidate->OpenPropertyStore(STGM_READ, &pProps)) &&
          pProps) {
        std::wstring nameStr;
        PROPVARIANT varName;
        PropVariantInit(&varName);
        if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &varName)) &&
            varName.vt == VT_LPWSTR && varName.pwszVal) {
          nameStr = varName.pwszVal;
        }
        PropVariantClear(&varName);

        if (nameStr.empty()) {
          PROPVARIANT varDesc;
          PropVariantInit(&varDesc);
          if (SUCCEEDED(pProps->GetValue(PKEY_Device_DeviceDesc, &varDesc)) &&
              varDesc.vt == VT_LPWSTR && varDesc.pwszVal) {
            nameStr = varDesc.pwszVal;
          }
          PropVariantClear(&varDesc);
        }

        pProps->Release();

        if (!nameStr.empty()) {
          std::wstring friendlyLower = nameStr;
          std::transform(friendlyLower.begin(), friendlyLower.end(),
                         friendlyLower.begin(), ::towlower);

          if (friendlyLower.find(targetLower) != std::wstring::npos ||
              targetLower.find(friendlyLower) != std::wstring::npos) {
            pMatchedDevice = pCandidate;
            Wh_Log(L"Matched audio endpoint (%s): %s",
                   flow == eCapture ? L"Capture" : L"Render", nameStr.c_str());
            break;
          }
        }
      }
      pCandidate->Release();
    }
  }

  pCol->Release();
  return pMatchedDevice;
}

// Initialize Audio Endpoint (Mic capture or System render)
static void InitAudioTracker(EDataFlow dataFlow,
                             AudioEndpointTracker &tracker) {
  tracker.Release();
  tracker.lastRetryTick = GetTickCount();

  if (!g_pEnumerator) {
    HRESULT hr = CoCreateInstance(CLSID_MMDevEnum, NULL, CLSCTX_ALL,
                                  IID_IMMDevEnum, (void **)&g_pEnumerator);
    if (FAILED(hr) || !g_pEnumerator)
      return;
  }

  IMMDevice *pDevice = nullptr;

  if (dataFlow == eCapture) {
    if (_wcsicmp(g_Settings.micDevice, L"default") == 0 ||
        wcslen(g_Settings.micDevice) == 0) {
      g_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    } else if (_wcsicmp(g_Settings.micDevice, L"communications") == 0) {
      g_pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications,
                                             &pDevice);
    } else {
      // Search capture endpoints first
      pDevice = FindDeviceByNameOrId(eCapture, g_Settings.micDevice);
      // If not found in capture endpoints, search render endpoints (for
      // Voicemeeter / virtual outputs)
      if (!pDevice) {
        pDevice = FindDeviceByNameOrId(eRender, g_Settings.micDevice);
      }
      // If still not found, fallback to default capture endpoint
      if (!pDevice) {
        g_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
      }
    }
  } else {
    g_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
  }

  if (pDevice) {
    tracker.pDevice = pDevice;

    // Only a visible capture meter needs an active stream. Render endpoint
    // metering reports the output mix without creating a render stream.
    if (dataFlow == eCapture && g_Settings.showMic && g_bHudVisible) {
      IAudioClient *pAudioClient = nullptr;
      if (SUCCEEDED(pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
                                      (void **)&pAudioClient)) && pAudioClient) {
        WAVEFORMATEX *pwfx = nullptr;
        if (SUCCEEDED(pAudioClient->GetMixFormat(&pwfx)) && pwfx) {
          HRESULT hrInit = pAudioClient->Initialize(
              AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, pwfx, NULL);
          CoTaskMemFree(pwfx);
          if (SUCCEEDED(hrInit) && SUCCEEDED(pAudioClient->Start())) {
            tracker.pAudioClient = pAudioClient;
          } else {
            pAudioClient->Release();
          }
        } else {
          pAudioClient->Release();
        }
      }
    }

    if (FAILED(pDevice->Activate(IID_IAudioMeterInfo, CLSCTX_ALL, NULL,
                                 (void **)&tracker.pMeter)) ||
        !tracker.pMeter) {
      if (tracker.pAudioClient) {
        tracker.pAudioClient->GetService(IID_IAudioMeterInfo,
                                         (void **)&tracker.pMeter);
      }
    }

    pDevice->Activate(IID_IAudioEndVol, CLSCTX_ALL, NULL,
                      (void **)&tracker.pVolume);
  }
}

static void RefreshAudioTrackers() {
  if (g_bHudVisible && g_Settings.showMic)
    InitAudioTracker(eCapture, g_MicTracker);
  else
    g_MicTracker.Release();

  if (g_bHudVisible && g_Settings.showSystem)
    InitAudioTracker(eRender, g_SystemTracker);
  else
    g_SystemTracker.Release();
}

// Update Audio Levels from WASAPI peak meters
static void UpdateAudioLevels() {
  DWORD currentTick = GetTickCount();

  // 1. Update Microphone Input Level
  if (g_Settings.showMic) {
    if (!g_MicTracker.pMeter &&
        currentTick - g_MicTracker.lastRetryTick >= 3000) {
      InitAudioTracker(eCapture, g_MicTracker);
    }

    if (g_MicTracker.pMeter) {
      float rawPeak = 0.0f;
      HRESULT hr = g_MicTracker.pMeter->GetPeakValue(&rawPeak);
      if (FAILED(hr)) {
        g_MicTracker.Release();
        g_MicTracker.lastRetryTick = currentTick;
      } else {
        if (g_MicTracker.pVolume) {
          g_MicTracker.pVolume->GetMute(&g_MicTracker.isMuted);
        }

        float dt = (g_MicTracker.lastUpdateTick > 0)
                       ? (currentTick - g_MicTracker.lastUpdateTick) / 1000.0f
                       : 0.033f;
        if (dt > 0.1f)
          dt = 0.033f;
        g_MicTracker.lastUpdateTick = currentTick;

        if (rawPeak >= g_MicTracker.currentLevel) {
          g_MicTracker.currentLevel = rawPeak;
        } else {
          g_MicTracker.currentLevel =
              std::max(rawPeak, g_MicTracker.currentLevel - dt * 2.2f);
        }

        int holdDuration = g_Settings.peakHoldDuration;
        if (holdDuration <= 0)
          holdDuration = 300;

        if (rawPeak >= g_MicTracker.peakHold) {
          g_MicTracker.peakHold = rawPeak;
          g_MicTracker.peakHoldTime = currentTick;
        } else if (currentTick - g_MicTracker.peakHoldTime >
                   (DWORD)holdDuration) {
          float newPeak = g_MicTracker.peakHold - dt * 1.2f;
          g_MicTracker.peakHold = std::max(g_MicTracker.currentLevel, newPeak);
        }

        if (rawPeak >= 0.98f) {
          g_MicTracker.isClipping = TRUE;
          g_MicTracker.clipTime = currentTick;
        } else if (currentTick - g_MicTracker.clipTime > 1500) {
          g_MicTracker.isClipping = FALSE;
        }
      }
    }
  }

  // 2. Update System Audio Output Level (endpoint peak meter)
  if (g_Settings.showSystem) {
    if (!g_SystemTracker.pMeter &&
        currentTick - g_SystemTracker.lastRetryTick >= 3000) {
      InitAudioTracker(eRender, g_SystemTracker);
    }

    if (g_SystemTracker.pMeter) {
      float rawPeak = 0.0f;
      HRESULT hr = g_SystemTracker.pMeter->GetPeakValue(&rawPeak);
      if (FAILED(hr)) {
        g_SystemTracker.Release();
        g_SystemTracker.lastRetryTick = currentTick;
      } else {
        if (g_SystemTracker.pVolume) {
          g_SystemTracker.pVolume->GetMute(&g_SystemTracker.isMuted);
        }

        float dt =
            (g_SystemTracker.lastUpdateTick > 0)
                ? (currentTick - g_SystemTracker.lastUpdateTick) / 1000.0f
                : 0.033f;
        if (dt > 0.1f)
          dt = 0.033f;
        g_SystemTracker.lastUpdateTick = currentTick;

        if (rawPeak >= g_SystemTracker.currentLevel) {
          g_SystemTracker.currentLevel = rawPeak;
        } else {
          g_SystemTracker.currentLevel =
              std::max(rawPeak, g_SystemTracker.currentLevel - dt * 2.2f);
        }

        int holdDuration = g_Settings.peakHoldDuration;
        if (holdDuration <= 0)
          holdDuration = 300;

        if (rawPeak >= g_SystemTracker.peakHold) {
          g_SystemTracker.peakHold = rawPeak;
          g_SystemTracker.peakHoldTime = currentTick;
        } else if (currentTick - g_SystemTracker.peakHoldTime >
                   (DWORD)holdDuration) {
          float newPeak = g_SystemTracker.peakHold - dt * 1.2f;
          g_SystemTracker.peakHold =
              std::max(g_SystemTracker.currentLevel, newPeak);
        }

        if (rawPeak >= 0.98f) {
          g_SystemTracker.isClipping = TRUE;
          g_SystemTracker.clipTime = currentTick;
        } else if (currentTick - g_SystemTracker.clipTime > 1500) {
          g_SystemTracker.isClipping = FALSE;
        }
      }
    }
  }
}

static void ParseAndRegisterHotkey(HWND hWnd, int id, PCWSTR hotkeyStr) {
  if (!hWnd || !hotkeyStr || wcslen(hotkeyStr) == 0)
    return;

  UINT fsModifiers = MOD_NOREPEAT;
  UINT vkCode = 0;

  WCHAR buffer[64];
  StringCchCopyW(buffer, 64, hotkeyStr);

  WCHAR *context = nullptr;
  WCHAR *token = wcstok_s(buffer, L"+", &context);
  while (token) {
    if (_wcsicmp(token, L"Ctrl") == 0 || _wcsicmp(token, L"Control") == 0) {
      fsModifiers |= MOD_CONTROL;
    } else if (_wcsicmp(token, L"Shift") == 0) {
      fsModifiers |= MOD_SHIFT;
    } else if (_wcsicmp(token, L"Alt") == 0) {
      fsModifiers |= MOD_ALT;
    } else if (_wcsicmp(token, L"Win") == 0) {
      fsModifiers |= MOD_WIN;
    } else if (wcslen(token) == 1) {
      WCHAR ch = towupper(token[0]);
      if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9')) {
        vkCode = ch;
      }
    } else if (_wcsicmp(token, L"F1") == 0)
      vkCode = VK_F1;
    else if (_wcsicmp(token, L"F2") == 0)
      vkCode = VK_F2;
    else if (_wcsicmp(token, L"F3") == 0)
      vkCode = VK_F3;
    else if (_wcsicmp(token, L"F4") == 0)
      vkCode = VK_F4;
    else if (_wcsicmp(token, L"F5") == 0)
      vkCode = VK_F5;
    else if (_wcsicmp(token, L"F6") == 0)
      vkCode = VK_F6;
    else if (_wcsicmp(token, L"F7") == 0)
      vkCode = VK_F7;
    else if (_wcsicmp(token, L"F8") == 0)
      vkCode = VK_F8;
    else if (_wcsicmp(token, L"F9") == 0)
      vkCode = VK_F9;
    else if (_wcsicmp(token, L"F10") == 0)
      vkCode = VK_F10;
    else if (_wcsicmp(token, L"F11") == 0)
      vkCode = VK_F11;
    else if (_wcsicmp(token, L"F12") == 0)
      vkCode = VK_F12;

    token = wcstok_s(NULL, L"+", &context);
  }

  if (vkCode != 0) {
    RegisterHotKey(hWnd, id, fsModifiers, vkCode);
  }
}

static void RegisterGlobalHotkey() {
  if (!g_hHudWnd)
    return;
  UnregisterGlobalHotkey();

  ParseAndRegisterHotkey(g_hHudWnd, HUD_HOTKEY_ID, g_Settings.toggleHotkey);
  ParseAndRegisterHotkey(g_hHudWnd, HUD_CLICKTHRU_HOTKEY_ID,
                         g_Settings.clickthroughHotkey);
}

static void UnregisterGlobalHotkey() {
  if (g_hHudWnd) {
    UnregisterHotKey(g_hHudWnd, HUD_HOTKEY_ID);
    UnregisterHotKey(g_hHudWnd, HUD_CLICKTHRU_HOTKEY_ID);
  }
}

// Load Mod Settings from Windhawk API with dual-sync (Panel + Local Storage)
static void LoadModSettings() {
  PCWSTR strVal = nullptr;

  strVal = Wh_GetStringSetting(L"position");
  if (strVal) {
    StringCchCopyW(g_Settings.position, 32, strVal);
    Wh_FreeStringSetting(strVal);
  }

  PCWSTR layoutStr = Wh_GetStringSetting(L"layout");
  std::wstring whLayout = layoutStr ? layoutStr : L"horizontal";
  if (layoutStr)
    Wh_FreeStringSetting(layoutStr);

  WCHAR lastWhLayout[32] = {0};
  Wh_GetStringValue(L"rt_lastWhLayout", lastWhLayout, ARRAYSIZE(lastWhLayout));
  if (whLayout != lastWhLayout) {
    Wh_SetStringValue(L"rt_lastWhLayout", whLayout.c_str());
    Wh_DeleteValue(L"rt_layout");
    StringCchCopyW(g_Settings.layout, 32, whLayout.c_str());
  } else {
    WCHAR savedLayout[32] = {0};
    if (Wh_GetStringValue(L"rt_layout", savedLayout, ARRAYSIZE(savedLayout))) {
      StringCchCopyW(g_Settings.layout, 32, savedLayout);
    } else {
      StringCchCopyW(g_Settings.layout, 32, whLayout.c_str());
    }
  }

  // 1. Scale
  PCWSTR scaleValStr = Wh_GetStringSetting(L"hud_scale");
  int windhawkScale = scaleValStr ? _wtoi(scaleValStr) : 100;
  if (scaleValStr)
    Wh_FreeStringSetting(scaleValStr);
  if (windhawkScale <= 0)
    windhawkScale = 100;

  int lastWindhawkScale = Wh_GetIntValue(L"rt_lastWindhawkScale", -1);
  if (lastWindhawkScale != windhawkScale) {
    Wh_SetIntValue(L"rt_lastWindhawkScale", windhawkScale);
    Wh_DeleteValue(L"rt_hudScale");
    g_Settings.hudScale = windhawkScale;
  } else {
    int savedScale = Wh_GetIntValue(L"rt_hudScale", -1);
    g_Settings.hudScale = (savedScale != -1) ? savedScale : windhawkScale;
  }

  // 2. Click-Through
  int whClickThrough = Wh_GetIntSetting(L"click_through");
  int lastWhClickThrough = Wh_GetIntValue(L"rt_lastWhClickThrough", -1);
  if (lastWhClickThrough != whClickThrough) {
    Wh_SetIntValue(L"rt_lastWhClickThrough", whClickThrough);
    Wh_DeleteValue(L"rt_clickThrough");
    g_Settings.clickThrough = whClickThrough;
  } else {
    g_Settings.clickThrough =
        Wh_GetIntValue(L"rt_clickThrough", whClickThrough);
  }

  // 3. Opacity
  PCWSTR opStr = Wh_GetStringSetting(L"card_opacity");
  int whOpacity = opStr ? _wtoi(opStr) : 88;
  if (opStr)
    Wh_FreeStringSetting(opStr);
  int lastWhOpacity = Wh_GetIntValue(L"rt_lastWhOpacity", -1);
  if (lastWhOpacity != whOpacity) {
    Wh_SetIntValue(L"rt_lastWhOpacity", whOpacity);
    Wh_DeleteValue(L"rt_opacity");
    g_Settings.opacity = whOpacity;
  } else {
    int savedOpacity = Wh_GetIntValue(L"rt_opacity", -1);
    g_Settings.opacity = (savedOpacity != -1) ? savedOpacity : whOpacity;
  }
  if (g_Settings.opacity < 0)
    g_Settings.opacity = 0;
  if (g_Settings.opacity > 100)
    g_Settings.opacity = 100;

  // 4. Show Meters
  PCWSTR showMetersStr = Wh_GetStringSetting(L"show_meters");
  std::wstring whShowMeters = showMetersStr ? showMetersStr : L"both";
  if (showMetersStr)
    Wh_FreeStringSetting(showMetersStr);

  WCHAR lastWhMeters[32] = {0};
  Wh_GetStringValue(L"rt_lastWhShowMeters", lastWhMeters,
                    ARRAYSIZE(lastWhMeters));
  if (whShowMeters != lastWhMeters) {
    Wh_SetStringValue(L"rt_lastWhShowMeters", whShowMeters.c_str());
    Wh_DeleteValue(L"rt_showMeters");
  } else {
    WCHAR savedMeters[32] = {0};
    if (Wh_GetStringValue(L"rt_showMeters", savedMeters,
                          ARRAYSIZE(savedMeters))) {
      whShowMeters = savedMeters;
    }
  }

  if (_wcsicmp(whShowMeters.c_str(), L"mic") == 0) {
    g_Settings.showMic = TRUE;
    g_Settings.showSystem = FALSE;
  } else if (_wcsicmp(whShowMeters.c_str(), L"system") == 0) {
    g_Settings.showMic = FALSE;
    g_Settings.showSystem = TRUE;
  } else {
    g_Settings.showMic = TRUE;
    g_Settings.showSystem = TRUE;
  }

  // 5. Show Labels
  int whShowLabels = Wh_GetIntSetting(L"show_labels");
  int lastWhShowLabels = Wh_GetIntValue(L"rt_lastWhShowLabels", -1);
  if (lastWhShowLabels != whShowLabels) {
    Wh_SetIntValue(L"rt_lastWhShowLabels", whShowLabels);
    Wh_DeleteValue(L"rt_showLabels");
    g_Settings.showLabels = whShowLabels;
  } else {
    g_Settings.showLabels = Wh_GetIntValue(L"rt_showLabels", whShowLabels);
  }

  // 6. Peak Hold
  int whPeakHold = Wh_GetIntSetting(L"enable_peak_hold");
  int lastWhPeakHold = Wh_GetIntValue(L"rt_lastWhPeakHold", -1);
  if (lastWhPeakHold != whPeakHold) {
    Wh_SetIntValue(L"rt_lastWhPeakHold", whPeakHold);
    Wh_DeleteValue(L"rt_enablePeakHold");
    g_Settings.enablePeakHold = whPeakHold;
  } else {
    g_Settings.enablePeakHold =
        Wh_GetIntValue(L"rt_enablePeakHold", whPeakHold);
  }

  // Peak Hold Duration
  PCWSTR peakDurStr = Wh_GetStringSetting(L"peak_hold_duration");
  g_Settings.peakHoldDuration = peakDurStr ? _wtoi(peakDurStr) : 300;
  if (peakDurStr)
    Wh_FreeStringSetting(peakDurStr);

  // Hotkey, FPS, Theme
  strVal = Wh_GetStringSetting(L"toggle_hotkey");
  if (strVal) {
    StringCchCopyW(g_Settings.toggleHotkey, 64, strVal);
    Wh_FreeStringSetting(strVal);
  } else {
    g_Settings.toggleHotkey[0] = L'\0';
  }

  strVal = Wh_GetStringSetting(L"clickthrough_hotkey");
  if (strVal) {
    StringCchCopyW(g_Settings.clickthroughHotkey, 64, strVal);
    Wh_FreeStringSetting(strVal);
  } else {
    g_Settings.clickthroughHotkey[0] = L'\0';
  }

  strVal = Wh_GetStringSetting(L"fps_limit");
  g_Settings.fpsLimit = strVal ? _wtoi(strVal) : 30;
  if (strVal)
    Wh_FreeStringSetting(strVal);
  if (g_Settings.fpsLimit <= 0)
    g_Settings.fpsLimit = 30;

  strVal = Wh_GetStringSetting(L"color_theme");
  if (strVal) {
    StringCchCopyW(g_Settings.colorTheme, 32, strVal);
    Wh_FreeStringSetting(strVal);
  }

  // Mic Device
  WCHAR savedMic[256] = {0};
  if (Wh_GetStringValue(L"rt_micDevice", savedMic, ARRAYSIZE(savedMic))) {
    StringCchCopyW(g_Settings.micDevice, 256, savedMic);
  } else {
    StringCchCopyW(g_Settings.micDevice, 256, L"default");
  }

  // Visibility state
  g_bHudVisible = Wh_GetIntValue(L"rt_hudVisible", TRUE);
}

// Position HUD according to monitor work area and settings
static void PositionHudWindow() {
  if (!g_hHudWnd)
    return;

  float dpiScale = GetDpiForWindow(g_hHudWnd) / 96.0f;

  HMONITOR hMonitor = MonitorFromWindow(g_hHudWnd, MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO mi = {sizeof(MONITORINFO)};
  GetMonitorInfoW(hMonitor, &mi);

  RECT workArea = mi.rcWork;

  BOOL isVertical = (_wcsicmp(g_Settings.layout, L"vertical") == 0);
  float scale = g_Settings.hudScale / 100.0f;
  if (scale < 0.25f)
    scale = 0.25f;

  int activeChannels =
      (g_Settings.showMic ? 1 : 0) + (g_Settings.showSystem ? 1 : 0);
  if (activeChannels < 1)
    activeChannels = 1;

  int baseW = isVertical ? (activeChannels == 2 ? 136 : 76) : 500;
  int baseH = isVertical ? 420 : (activeChannels == 1 ? 62 : 104);

  int w = (int)roundf(baseW * scale * dpiScale);
  int h = (int)roundf(baseH * scale * dpiScale);
  int margin = (int)roundf(24 * dpiScale);

  int x = workArea.right - w - margin;
  int y = workArea.top + margin;

  if (_wcsicmp(g_Settings.position, L"custom") == 0) {
    x = Wh_GetIntValue(L"custom_x", workArea.right - w - margin);
    y = Wh_GetIntValue(L"custom_y", workArea.top + margin);
  } else if (_wcsicmp(g_Settings.position, L"top-left") == 0) {
    x = workArea.left + margin;
    y = workArea.top + margin;
  } else if (_wcsicmp(g_Settings.position, L"bottom-right") == 0) {
    x = workArea.right - w - margin;
    y = workArea.bottom - h - margin;
  } else if (_wcsicmp(g_Settings.position, L"bottom-left") == 0) {
    x = workArea.left + margin;
    y = workArea.bottom - h - margin;
  } else {
    // top-right (default preset)
    x = workArea.right - w - margin;
    y = workArea.top + margin;
  }

  x = std::clamp(x, (int)workArea.left,
                 std::max((int)workArea.left, (int)workArea.right - w));
  y = std::clamp(y, (int)workArea.top,
                 std::max((int)workArea.top, (int)workArea.bottom - h));

  SetWindowPos(g_hHudWnd, HWND_TOPMOST, x, y, w, h,
               SWP_NOACTIVATE | SWP_SHOWWINDOW);

  // Apply click-through styles dynamically

  LONG_PTR exStyle = GetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE);
  if (g_Settings.clickThrough) {
    exStyle |= (WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE, exStyle);
    SetLayeredWindowAttributes(g_hHudWnd, 0, 255, LWA_ALPHA);
  } else {
    exStyle &= ~(WS_EX_TRANSPARENT | WS_EX_LAYERED);
    SetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE, exStyle);
  }
  SetWindowPos(g_hHudWnd, NULL, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                   SWP_FRAMECHANGED);
}

// DirectComposition + DXGI SwapChain Hardware Accelerated Initialization
static HRESULT InitDirectComposition(HWND hWnd) {
  CleanupDirectComposition();

  if (!hWnd)
    return E_POINTER;

  // 1. Create Direct3D 11 Device
  UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
  D3D_FEATURE_LEVEL featureLevel;
  HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                 createDeviceFlags, NULL, 0, D3D11_SDK_VERSION,
                                 &g_pD3DDevice, &featureLevel, &g_pD3DContext);
  if (FAILED(hr) || !g_pD3DDevice)
    return hr;

  // 2. Query DXGI Device
  IDXGIDevice *pDxgiDevice = nullptr;
  hr = g_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice),
                                    (void **)&pDxgiDevice);
  if (FAILED(hr) || !pDxgiDevice)
    return hr;

  // 3. Create Direct2D Factory 1 & Direct2D Device
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                         __uuidof(ID2D1Factory1), nullptr,
                         (void **)&g_pD2DFactory1);
  if (FAILED(hr) || !g_pD2DFactory1) {
    pDxgiDevice->Release();
    return hr;
  }

  hr = g_pD2DFactory1->CreateDevice(pDxgiDevice, &g_pD2DDevice);
  if (FAILED(hr) || !g_pD2DDevice) {
    pDxgiDevice->Release();
    return hr;
  }

  hr = g_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                         &g_pD2DContext);
  if (FAILED(hr) || !g_pD2DContext) {
    pDxgiDevice->Release();
    return hr;
  }

  g_pD2DContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
  g_pD2DContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

  // 4. Create DXGI Swap Chain 1 for Composition
  IDXGIAdapter *pDxgiAdapter = nullptr;
  hr = pDxgiDevice->GetAdapter(&pDxgiAdapter);
  if (SUCCEEDED(hr) && pDxgiAdapter) {
    IDXGIFactory2 *pDxgiFactory2 = nullptr;
    hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory2),
                                 (void **)&pDxgiFactory2);
    if (SUCCEEDED(hr) && pDxgiFactory2) {
      RECT clientRect;
      GetClientRect(hWnd, &clientRect);
      int scaledWidth = std::max(1L, clientRect.right - clientRect.left);
      int scaledHeight = std::max(1L, clientRect.bottom - clientRect.top);

      DXGI_SWAP_CHAIN_DESC1 desc = {0};
      desc.Width = scaledWidth;
      desc.Height = scaledHeight;
      desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      desc.Stereo = FALSE;
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      desc.BufferCount = 2;
      desc.Scaling = DXGI_SCALING_STRETCH;
      desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
      desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

      hr = pDxgiFactory2->CreateSwapChainForComposition(g_pD3DDevice, &desc,
                                                        NULL, &g_pSwapChain);
      pDxgiFactory2->Release();
    }
    pDxgiAdapter->Release();
  }

  if (!g_pSwapChain) {
    pDxgiDevice->Release();
    return E_FAIL;
  }

  // 5. Create DirectComposition Device, Target & Visual
  hr = DCompositionCreateDevice(pDxgiDevice, __uuidof(IDCompositionDevice),
                                (void **)&g_pDCompDevice);
  pDxgiDevice->Release();
  if (FAILED(hr) || !g_pDCompDevice)
    return hr;

  hr = g_pDCompDevice->CreateTargetForHwnd(hWnd, FALSE, &g_pDCompTarget);
  if (FAILED(hr) || !g_pDCompTarget)
    return hr;

  hr = g_pDCompDevice->CreateVisual(&g_pDCompVisual);
  if (FAILED(hr) || !g_pDCompVisual)
    return hr;

  g_pDCompVisual->SetContent(g_pSwapChain);
  g_pDCompTarget->SetRoot(g_pDCompVisual);
  g_pDCompDevice->Commit();

  // 6. Bind SwapChain BackBuffer Surface to Direct2D Context
  IDXGISurface *pDxgiBackBuffer = nullptr;
  hr = g_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface),
                               (void **)&pDxgiBackBuffer);
  if (SUCCEEDED(hr) && pDxgiBackBuffer) {
    float scale = g_Settings.hudScale / 100.0f;
    if (scale < 0.25f)
      scale = 0.25f;

    float dpi = (float)GetDpiForWindow(hWnd) * scale;
    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        dpi, dpi);

    hr = g_pD2DContext->CreateBitmapFromDxgiSurface(
        pDxgiBackBuffer, &bitmapProperties, &g_pD2DTargetBitmap);
    if (SUCCEEDED(hr) && g_pD2DTargetBitmap) {
      g_pD2DContext->SetTarget(g_pD2DTargetBitmap);
      g_pD2DContext->SetDpi(dpi, dpi);
    }
    pDxgiBackBuffer->Release();
  }

  // 7. DirectWrite Fonts Initialization with Adaptive Font Scale Compensation
  hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                           (IUnknown **)&g_pDWriteFactory);
  if (SUCCEEDED(hr) && g_pDWriteFactory) {
    float scale = g_Settings.hudScale / 100.0f;
    if (scale < 0.25f)
      scale = 0.25f;
    float fontComp = (scale < 1.0f) ? (1.0f / sqrtf(scale)) : 1.0f;

    hr = g_pDWriteFactory->CreateTextFormat(
        L"Segoe Fluent Icons", NULL, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 17.0f * fontComp, L"en-us",
        &g_pIconFontFormat);
    if (FAILED(hr) || !g_pIconFontFormat) {
      g_pDWriteFactory->CreateTextFormat(
          L"Segoe MDL2 Assets", NULL, DWRITE_FONT_WEIGHT_NORMAL,
          DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 17.0f * fontComp, L"en-us",
          &g_pIconFontFormat);
    }
    if (g_pIconFontFormat) {
      g_pIconFontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
      g_pIconFontFormat->SetParagraphAlignment(
          DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
      g_pIconFontFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f * fontComp, L"en-us",
        &g_pLabelFontFormat);
    if (g_pLabelFontFormat) {
      g_pLabelFontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      g_pLabelFontFormat->SetParagraphAlignment(
          DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
      g_pLabelFontFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f * fontComp, L"en-us",
        &g_pLabelFontFormatVert);
    if (g_pLabelFontFormatVert) {
      g_pLabelFontFormatVert->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
      g_pLabelFontFormatVert->SetParagraphAlignment(
          DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
      g_pLabelFontFormatVert->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 13.0f * fontComp, L"en-us", &g_pValueFontFormat);
    if (g_pValueFontFormat) {
      g_pValueFontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
      g_pValueFontFormat->SetParagraphAlignment(
          DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
      g_pValueFontFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 11.0f * fontComp, L"en-us", &g_pValueFontFormatVert);
    if (g_pValueFontFormatVert) {
      g_pValueFontFormatVert->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
      g_pValueFontFormatVert->SetParagraphAlignment(
          DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
      g_pValueFontFormatVert->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
  }

  return S_OK;
}

static void CleanupDirectComposition() {
  if (g_pIconFontFormat) {
    g_pIconFontFormat->Release();
    g_pIconFontFormat = nullptr;
  }
  if (g_pLabelFontFormat) {
    g_pLabelFontFormat->Release();
    g_pLabelFontFormat = nullptr;
  }
  if (g_pLabelFontFormatVert) {
    g_pLabelFontFormatVert->Release();
    g_pLabelFontFormatVert = nullptr;
  }
  if (g_pValueFontFormat) {
    g_pValueFontFormat->Release();
    g_pValueFontFormat = nullptr;
  }
  if (g_pValueFontFormatVert) {
    g_pValueFontFormatVert->Release();
    g_pValueFontFormatVert = nullptr;
  }
  if (g_pDWriteFactory) {
    g_pDWriteFactory->Release();
    g_pDWriteFactory = nullptr;
  }
  if (g_pD2DTargetBitmap) {
    g_pD2DTargetBitmap->Release();
    g_pD2DTargetBitmap = nullptr;
  }
  if (g_pD2DContext) {
    g_pD2DContext->Release();
    g_pD2DContext = nullptr;
  }
  if (g_pD2DDevice) {
    g_pD2DDevice->Release();
    g_pD2DDevice = nullptr;
  }
  if (g_pD2DFactory1) {
    g_pD2DFactory1->Release();
    g_pD2DFactory1 = nullptr;
  }
  if (g_pDCompVisual) {
    g_pDCompVisual->Release();
    g_pDCompVisual = nullptr;
  }
  if (g_pDCompTarget) {
    g_pDCompTarget->Release();
    g_pDCompTarget = nullptr;
  }
  if (g_pDCompDevice) {
    g_pDCompDevice->Release();
    g_pDCompDevice = nullptr;
  }
  if (g_pSwapChain) {
    g_pSwapChain->Release();
    g_pSwapChain = nullptr;
  }
  if (g_pD3DContext) {
    g_pD3DContext->Release();
    g_pD3DContext = nullptr;
  }
  if (g_pD3DDevice) {
    g_pD3DDevice->Release();
    g_pD3DDevice = nullptr;
  }
}

// Render HUD Card with DirectComposition + Direct2D Liquid Glass Pipeline
static void RenderHud() {
  if (!g_pD2DContext || !g_pSwapChain) {
    InitDirectComposition(g_hHudWnd);
    if (!g_pD2DContext || !g_pSwapChain)
      return;
  }

  g_pD2DContext->BeginDraw();
  g_pD2DContext->Clear(
      D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)); // Transparent premultiplied canvas

  D2D1_SIZE_F size = g_pD2DContext->GetSize();
  BOOL isVertical = (_wcsicmp(g_Settings.layout, L"vertical") == 0);
  float cornerRadius = 16.0f; // Flush with Windows 11 DWM native corner

  D2D1_ROUNDED_RECT cardRect = D2D1::RoundedRect(
      D2D1::RectF(0.5f, 0.5f, size.width - 0.5f, size.height - 0.5f),
      cornerRadius, cornerRadius);

  float masterOp = g_Settings.opacity / 100.0f;

  // 1. Fluent Dark Card Background
  ID2D1GradientStopCollection *pBaseStops = nullptr;
  D2D1_GRADIENT_STOP baseStops[2];
  baseStops[0].color =
      D2D1::ColorF(0.10f, 0.12f, 0.16f, masterOp); // Dark Fluent slate top
  baseStops[0].position = 0.0f;
  baseStops[1].color =
      D2D1::ColorF(0.06f, 0.07f, 0.10f, masterOp); // Darker Fluent slate bottom
  baseStops[1].position = 1.0f;
  g_pD2DContext->CreateGradientStopCollection(
      baseStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pBaseStops);

  ID2D1LinearGradientBrush *pBaseBgBrush = nullptr;
  if (pBaseStops) {
    g_pD2DContext->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(size.width, size.height)),
        pBaseStops, &pBaseBgBrush);
    pBaseStops->Release();
  }
  if (pBaseBgBrush) {
    g_pD2DContext->FillRoundedRectangle(cardRect, pBaseBgBrush);
    pBaseBgBrush->Release();
  }

  // 2. Top Specular Glass Gloss Sheen Reflection
  ID2D1GradientStopCollection *pSheenStops = nullptr;
  D2D1_GRADIENT_STOP sheenStops[2];
  sheenStops[0].color =
      D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f * masterOp);
  sheenStops[0].position = 0.0f;
  sheenStops[1].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f);
  sheenStops[1].position = 1.0f;
  g_pD2DContext->CreateGradientStopCollection(
      sheenStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pSheenStops);

  ID2D1LinearGradientBrush *pSheenBrush = nullptr;
  if (pSheenStops) {
    g_pD2DContext->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(0.0f, 0.0f),
            D2D1::Point2F(0.0f, size.height * (isVertical ? 0.25f : 0.45f))),
        pSheenStops, &pSheenBrush);
    pSheenStops->Release();
  }
  if (pSheenBrush) {
    D2D1_ROUNDED_RECT sheenRect = D2D1::RoundedRect(
        D2D1::RectF(1.0f, 1.0f, size.width - 1.0f, size.height * (isVertical ? 0.25f : 0.45f)),
        16.0f, 16.0f);
    g_pD2DContext->FillRoundedRectangle(sheenRect, pSheenBrush);
    pSheenBrush->Release();
  }

  // 3. Specular Rim Lighting
  ID2D1GradientStopCollection *pRimStops = nullptr;
  D2D1_GRADIENT_STOP rimStops[3];
  rimStops[0].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f * masterOp);
  rimStops[0].position = 0.0f;
  rimStops[1].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f * masterOp);
  rimStops[1].position = 0.45f;
  rimStops[2].color = D2D1::ColorF(0.02f, 0.05f, 0.10f, 0.18f * masterOp);
  rimStops[2].position = 1.0f;
  g_pD2DContext->CreateGradientStopCollection(
      rimStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pRimStops);

  ID2D1LinearGradientBrush *pRimBrush = nullptr;
  if (pRimStops) {
    g_pD2DContext->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(size.width, size.height)),
        pRimStops, &pRimBrush);
    pRimStops->Release();
  }
  if (pRimBrush) {
    g_pD2DContext->DrawRoundedRectangle(cardRect, pRimBrush, 1.2f);
    pRimBrush->Release();
  }

  int activeChannels =
      (g_Settings.showMic ? 1 : 0) + (g_Settings.showSystem ? 1 : 0);
  if (activeChannels == 0)
    activeChannels = 1;

  // Divider Line
  if (g_Settings.showMic && g_Settings.showSystem) {
    ID2D1SolidColorBrush *pDividerShadowBrush = nullptr;
    ID2D1SolidColorBrush *pDividerLightBrush = nullptr;
    g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.40f),
                                         &pDividerShadowBrush);
    g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f),
                                         &pDividerLightBrush);

    if (isVertical) {
      float midX = size.width / 2.0f;
      if (pDividerShadowBrush) {
        g_pD2DContext->DrawLine(D2D1::Point2F(midX + 0.5f, 36.0f),
                                D2D1::Point2F(midX + 0.5f, size.height - 36.0f),
                                pDividerShadowBrush, 1.0f);
        pDividerShadowBrush->Release();
      }
      if (pDividerLightBrush) {
        g_pD2DContext->DrawLine(D2D1::Point2F(midX - 0.5f, 36.0f),
                                D2D1::Point2F(midX - 0.5f, size.height - 36.0f),
                                pDividerLightBrush, 1.0f);
        pDividerLightBrush->Release();
      }
    } else {
      float midY = size.height / 2.0f;
      if (pDividerShadowBrush) {
        g_pD2DContext->DrawLine(D2D1::Point2F(16.0f, midY + 0.5f),
                                D2D1::Point2F(size.width - 40.0f, midY + 0.5f),
                                pDividerShadowBrush, 1.0f);
        pDividerShadowBrush->Release();
      }
      if (pDividerLightBrush) {
        g_pD2DContext->DrawLine(D2D1::Point2F(16.0f, midY - 0.5f),
                                D2D1::Point2F(size.width - 40.0f, midY - 0.5f),
                                pDividerLightBrush, 1.0f);
        pDividerLightBrush->Release();
      }
    }
  }

  // Color Palette Definitions
  D2D1_COLOR_F greenColor = D2D1::ColorF(0.13f, 0.77f, 0.37f, 1.0f); // Fluent Green
  D2D1_COLOR_F yellowColor = D2D1::ColorF(0.95f, 0.72f, 0.05f, 1.0f); // Fluent Yellow

  if (_wcsicmp(g_Settings.colorTheme, L"neon") == 0) {
    greenColor = D2D1::ColorF(0.00f, 0.90f, 1.00f);
    yellowColor = D2D1::ColorF(1.00f, 0.00f, 0.60f);
  } else if (_wcsicmp(g_Settings.colorTheme, L"emerald") == 0) {
    greenColor = D2D1::ColorF(0.00f, 0.72f, 0.42f);
    yellowColor = D2D1::ColorF(0.70f, 0.95f, 0.18f);
  } else if (_wcsicmp(g_Settings.colorTheme, L"sunset") == 0) {
    greenColor = D2D1::ColorF(1.00f, 0.55f, 0.00f);
    yellowColor = D2D1::ColorF(0.95f, 0.20f, 0.20f);
  } else if (_wcsicmp(g_Settings.colorTheme, L"monochrome") == 0) {
    greenColor = D2D1::ColorF(0.85f, 0.85f, 0.88f);
    yellowColor = D2D1::ColorF(0.70f, 0.70f, 0.75f);
  }

  ID2D1SolidColorBrush *pTextBrush = nullptr;
  ID2D1SolidColorBrush *pGreenBrush = nullptr;
  ID2D1SolidColorBrush *pYellowBrush = nullptr;
  ID2D1SolidColorBrush *pUnlitBrush = nullptr;
  ID2D1SolidColorBrush *pClipBrush = nullptr;
  ID2D1SolidColorBrush *pMuteBrush = nullptr;
  ID2D1SolidColorBrush *pPeakHoldBrush = nullptr;

  g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pTextBrush);
  g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.88f, 0.25f, 0.95f), &pPeakHoldBrush);
  g_pD2DContext->CreateSolidColorBrush(greenColor, &pGreenBrush);
  g_pD2DContext->CreateSolidColorBrush(yellowColor, &pYellowBrush);
  g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f), &pUnlitBrush);
  g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.20f, 0.20f, 1.0f), &pClipBrush);
  g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.40f, 0.45f, 0.4f), &pMuteBrush);

  ID2D1GradientStopCollection *pPillGlossStops = nullptr;
  D2D1_GRADIENT_STOP pillGlossStops[2];
  pillGlossStops[0].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.45f);
  pillGlossStops[0].position = 0.0f;
  pillGlossStops[1].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f);
  pillGlossStops[1].position = 1.0f;
  g_pD2DContext->CreateGradientStopCollection(pillGlossStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pPillGlossStops);

  ID2D1SolidColorBrush *pPillCapBrush = nullptr;
  g_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.60f), &pPillCapBrush);

  const int TOTAL_SEGMENTS = 25;
  const float SEG_GAP = 2.5f;
  const float SEG_CORNER = 2.5f;

  float scale = g_Settings.hudScale / 100.0f;
  if (scale < 0.25f) scale = 0.25f;
  float fontComp = (scale < 1.0f) ? (1.0f / sqrtf(scale)) : 1.0f;

  if (isVertical) {
    // Ã¢â€â‚¬Ã¢â€â‚¬ Vertical Orientation Renderer Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    int colIndex = 0;
    float colWidth = size.width / (float)activeChannels;

    auto DrawVerticalChannel = [&](AudioEndpointTracker &tracker, PCWSTR iconChar, PCWSTR labelText) {
      float colCenterX = (colIndex + 0.5f) * colWidth;

      // Icon at Top
      if (g_pIconFontFormat && pTextBrush) {
        D2D1_RECT_F iconRect = D2D1::RectF(colCenterX - 18.0f, 12.0f, colCenterX + 18.0f, 34.0f);
        g_pD2DContext->DrawTextW(iconChar, 1, g_pIconFontFormat, iconRect, pTextBrush);
      }

      // Channel Label ("Mic" / "System") right below icon
      float labelBottom = 34.0f;
      if (g_Settings.showLabels && (g_pLabelFontFormatVert || g_pLabelFontFormat) && pTextBrush) {
        labelBottom = 52.0f;
        D2D1_RECT_F labelRect = D2D1::RectF(colCenterX - 30.0f, 32.0f, colCenterX + 30.0f, 50.0f);
        IDWriteTextFormat *pLblFmt = g_pLabelFontFormatVert ? g_pLabelFontFormatVert : g_pLabelFontFormat;
        g_pD2DContext->DrawTextW(labelText, (UINT32)wcslen(labelText), pLblFmt, labelRect, pTextBrush);
      }

      // Vertical Level Meter Bar (25 pills stacked bottom-to-top)
      float barTop = labelBottom + 8.0f;
      float barBottom = size.height - 40.0f;
      float barTotalHeight = barBottom - barTop;
      float segHeight = (barTotalHeight - (TOTAL_SEGMENTS - 1) * SEG_GAP) / (float)TOTAL_SEGMENTS;
      float segWidth = 18.0f;

      int litCount = 0;
      if (!tracker.isMuted && tracker.currentLevel > 0.0001f) {
        float ratio = LinearToMeterRatio(tracker.currentLevel);
        litCount = (int)roundf(ratio * TOTAL_SEGMENTS);
        if (litCount < 1 && tracker.currentLevel > 0.001f) litCount = 1;
        if (litCount > TOTAL_SEGMENTS) litCount = TOTAL_SEGMENTS;
      }

      int peakIndex = -1;
      if (g_Settings.enablePeakHold && !tracker.isMuted && tracker.peakHold > 0.0001f) {
        float peakRatio = LinearToMeterRatio(tracker.peakHold);
        peakIndex = (int)roundf(peakRatio * TOTAL_SEGMENTS) - 1;
        if (peakIndex < 0 && tracker.peakHold > 0.001f) peakIndex = 0;
        if (peakIndex >= TOTAL_SEGMENTS) peakIndex = TOTAL_SEGMENTS - 1;
      }

      for (int i = 0; i < TOTAL_SEGMENTS; i++) {
        // Stack bottom-to-top (i = 0 at bottom)
        float segY = barBottom - (i + 1) * segHeight - i * SEG_GAP;
        float segX = colCenterX - segWidth / 2.0f;
        D2D1_ROUNDED_RECT segRect = D2D1::RoundedRect(
            D2D1::RectF(segX, segY, segX + segWidth, segY + segHeight),
            SEG_CORNER, SEG_CORNER);

        ID2D1SolidColorBrush *pBrush = pUnlitBrush;
        if (tracker.isMuted) {
          pBrush = pMuteBrush;
        } else if (i < litCount) {
          if (tracker.isClipping) {
            pBrush = pClipBrush;
          } else if (i >= 17) {
            pBrush = pYellowBrush;
          } else {
            pBrush = pGreenBrush;
          }
        } else if (i == peakIndex && pPeakHoldBrush) {
          pBrush = pPeakHoldBrush;
        }

        if (pBrush) {
          g_pD2DContext->FillRoundedRectangle(segRect, pBrush);

          // 3D Liquid Gloss Overlay on top half of active vertical segment
          if (pBrush != pUnlitBrush && pBrush != pMuteBrush && pPillGlossStops) {
            ID2D1LinearGradientBrush *pPillGlossBrush = nullptr;
            g_pD2DContext->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(0.0f, segY), D2D1::Point2F(0.0f, segY + segHeight * 0.5f)),
                pPillGlossStops, &pPillGlossBrush);

            if (pPillGlossBrush) {
              D2D1_ROUNDED_RECT topGlossRect = D2D1::RoundedRect(
                  D2D1::RectF(segX, segY, segX + segWidth, segY + segHeight * 0.5f),
                  SEG_CORNER, SEG_CORNER);
              g_pD2DContext->FillRoundedRectangle(topGlossRect, pPillGlossBrush);
              pPillGlossBrush->Release();
            }

            if (pPillCapBrush) {
              g_pD2DContext->DrawLine(
                  D2D1::Point2F(segX + 0.8f, segY + 0.5f),
                  D2D1::Point2F(segX + segWidth - 0.8f, segY + 0.5f),
                  pPillCapBrush, 1.0f);
            }
          }
        }
      }

      // Live dB text readout at bottom
      if ((g_pValueFontFormatVert || g_pValueFontFormat) && pTextBrush) {
        WCHAR dbText[32];
        ID2D1SolidColorBrush *pCurTextBrush = pTextBrush;
        IDWriteTextFormat *pValFmt = g_pValueFontFormatVert ? g_pValueFontFormatVert : g_pValueFontFormat;

        if (tracker.isMuted) {
          StringCchCopyW(dbText, 32, L"Muted");
        } else {
          if (tracker.isClipping) {
            pCurTextBrush = pClipBrush;
          }
          if (tracker.currentLevel <= 0.0001f) {
            StringCchCopyW(dbText, 32, L"-60 dB");
          } else {
            float dB = 20.0f * log10f(tracker.currentLevel);
            if (dB < -60.0f) dB = -60.0f;
            if (dB > 0.0f) dB = 0.0f;
            swprintf_s(dbText, L"%d dB", (int)roundf(dB));
          }
        }

        D2D1_RECT_F valueRect = D2D1::RectF(colCenterX - 26.0f, size.height - 32.0f, colCenterX + 26.0f, size.height - 4.0f);
        g_pD2DContext->DrawTextW(dbText, (UINT32)wcslen(dbText), pValFmt, valueRect, pCurTextBrush);
      }

      colIndex++;
    };

    if (g_Settings.showMic) {
      WCHAR micIconStr[2] = {L'\uE720', L'\0'};
      DrawVerticalChannel(g_MicTracker, micIconStr, L"Mic");
    }
    if (g_Settings.showSystem) {
      WCHAR sysIconStr[2] = {L'\uE767', L'\0'};
      DrawVerticalChannel(g_SystemTracker, sysIconStr, L"System");
    }
  } else {
    // Ã¢â€â‚¬Ã¢â€â‚¬ Horizontal Orientation Renderer Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
    float rowHeight = size.height / (float)activeChannels;
    float currentY = 0.0f;

    const float BAR_LEFT = g_Settings.showLabels ? (122.0f * (0.85f + 0.15f * fontComp)) : 48.0f;
    const float BAR_RIGHT = std::max(BAR_LEFT + 100.0f, size.width - (68.0f * fontComp));
    const float BAR_TOTAL_WIDTH = BAR_RIGHT - BAR_LEFT;
    const float SEG_WIDTH = (BAR_TOTAL_WIDTH - (TOTAL_SEGMENTS - 1) * SEG_GAP) / (float)TOTAL_SEGMENTS;

    auto DrawRow = [&](AudioEndpointTracker &tracker, PCWSTR iconChar, PCWSTR labelText) {
      float rowCenterY = currentY + rowHeight / 2.0f;

      // Icon
      if (g_pIconFontFormat && pTextBrush) {
        D2D1_RECT_F iconRect = D2D1::RectF(14.0f, currentY, 38.0f, currentY + rowHeight);
        g_pD2DContext->DrawTextW(iconChar, 1, g_pIconFontFormat, iconRect, pTextBrush);
      }

      // Label
      if (g_Settings.showLabels && g_pLabelFontFormat && pTextBrush) {
        D2D1_RECT_F labelRect = D2D1::RectF(48.0f, currentY, BAR_LEFT - 6.0f, currentY + rowHeight);
        g_pD2DContext->DrawTextW(labelText, (UINT32)wcslen(labelText), g_pLabelFontFormat, labelRect, pTextBrush);
      }

      // Horizontal segmented level bar
      float segHeight = 18.0f;
      float segTop = rowCenterY - segHeight / 2.0f;
      float segBottom = rowCenterY + segHeight / 2.0f;

      int litCount = 0;
      if (!tracker.isMuted && tracker.currentLevel > 0.0001f) {
        float ratio = LinearToMeterRatio(tracker.currentLevel);
        litCount = (int)roundf(ratio * TOTAL_SEGMENTS);
        if (litCount < 1 && tracker.currentLevel > 0.001f) litCount = 1;
        if (litCount > TOTAL_SEGMENTS) litCount = TOTAL_SEGMENTS;
      }

      int peakIndex = -1;
      if (g_Settings.enablePeakHold && !tracker.isMuted && tracker.peakHold > 0.0001f) {
        float peakRatio = LinearToMeterRatio(tracker.peakHold);
        peakIndex = (int)roundf(peakRatio * TOTAL_SEGMENTS) - 1;
        if (peakIndex < 0 && tracker.peakHold > 0.001f) peakIndex = 0;
        if (peakIndex >= TOTAL_SEGMENTS) peakIndex = TOTAL_SEGMENTS - 1;
      }

      for (int i = 0; i < TOTAL_SEGMENTS; i++) {
        float segX = BAR_LEFT + i * (SEG_WIDTH + SEG_GAP);
        D2D1_ROUNDED_RECT segRect = D2D1::RoundedRect(
            D2D1::RectF(segX, segTop, segX + SEG_WIDTH, segBottom), SEG_CORNER, SEG_CORNER);

        ID2D1SolidColorBrush *pBrush = pUnlitBrush;
        if (tracker.isMuted) {
          pBrush = pMuteBrush;
        } else if (i < litCount) {
          if (tracker.isClipping) {
            pBrush = pClipBrush;
          } else if (i >= 17) {
            pBrush = pYellowBrush;
          } else {
            pBrush = pGreenBrush;
          }
        } else if (i == peakIndex && pPeakHoldBrush) {
          pBrush = pPeakHoldBrush;
        }

        if (pBrush) {
          g_pD2DContext->FillRoundedRectangle(segRect, pBrush);

          if (pBrush != pUnlitBrush && pBrush != pMuteBrush && pPillGlossStops) {
            ID2D1LinearGradientBrush *pPillGlossBrush = nullptr;
            g_pD2DContext->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(0.0f, segTop), D2D1::Point2F(0.0f, segTop + 9.0f)),
                pPillGlossStops, &pPillGlossBrush);

            if (pPillGlossBrush) {
              D2D1_ROUNDED_RECT topGlossRect = D2D1::RoundedRect(
                  D2D1::RectF(segX, segTop, segX + SEG_WIDTH, segTop + 9.0f),
                  SEG_CORNER, SEG_CORNER);
              g_pD2DContext->FillRoundedRectangle(topGlossRect, pPillGlossBrush);
              pPillGlossBrush->Release();
            }

            if (pPillCapBrush) {
              g_pD2DContext->DrawLine(
                  D2D1::Point2F(segX + 0.8f, segTop + 0.5f),
                  D2D1::Point2F(segX + SEG_WIDTH - 0.8f, segTop + 0.5f),
                  pPillCapBrush, 1.0f);
            }
          }
        }
      }

      // Live dB text readout
      if (g_pValueFontFormat && pTextBrush) {
        WCHAR dbText[32];
        ID2D1SolidColorBrush *pCurTextBrush = pTextBrush;

        if (tracker.isMuted) {
          StringCchCopyW(dbText, 32, L"Muted");
        } else {
          if (tracker.isClipping) {
            pCurTextBrush = pClipBrush;
          }
          if (tracker.currentLevel <= 0.0001f) {
            StringCchCopyW(dbText, 32, L"-60 dB");
          } else {
            float dB = 20.0f * log10f(tracker.currentLevel);
            if (dB < -60.0f) dB = -60.0f;
            if (dB > 0.0f) dB = 0.0f;
            swprintf_s(dbText, L"%d dB", (int)roundf(dB));
          }
        }

        D2D1_RECT_F valueRect = D2D1::RectF(
            BAR_RIGHT + 4.0f, currentY, size.width - 24.0f, currentY + rowHeight);
        g_pD2DContext->DrawTextW(dbText, (UINT32)wcslen(dbText),
                                 g_pValueFontFormat, valueRect, pCurTextBrush);
      }

      currentY += rowHeight;
    };

    if (g_Settings.showMic) {
      WCHAR micIconStr[2] = {L'\uE720', L'\0'};
      DrawRow(g_MicTracker, micIconStr, L"Mic");
    }
    if (g_Settings.showSystem) {
      WCHAR sysIconStr[2] = {L'\uE767', L'\0'};
      DrawRow(g_SystemTracker, sysIconStr, L"System");
    }
  }

  // Draw Settings overflow button (Ã¢â€¹Â®) if clickable
  if (!g_Settings.clickThrough && g_pIconFontFormat && pTextBrush) {
    WCHAR moreIconStr[2] = {L'\uE712', L'\0'};

    if (isVertical) {
      D2D1_POINT_2F dotCenter = D2D1::Point2F(size.width / 2.0f, 23.0f);
      g_pD2DContext->SetTransform(D2D1::Matrix3x2F::Rotation(90.0f, dotCenter));
      D2D1_RECT_F dotsRect = D2D1::RectF(dotCenter.x - 14.0f, dotCenter.y - 14.0f, dotCenter.x + 14.0f, dotCenter.y + 14.0f);
      g_pD2DContext->DrawTextW(moreIconStr, 1, g_pIconFontFormat, dotsRect, pTextBrush);
      g_pD2DContext->SetTransform(D2D1::Matrix3x2F::Identity());
    } else {
      D2D1_POINT_2F center = D2D1::Point2F(size.width - 16.0f, size.height / 2.0f);
      g_pD2DContext->SetTransform(D2D1::Matrix3x2F::Rotation(90.0f, center));
      D2D1_RECT_F moreRect = D2D1::RectF(size.width - 32.0f, 0.0f, size.width, size.height);
      g_pD2DContext->DrawTextW(moreIconStr, 1, g_pIconFontFormat, moreRect, pTextBrush);
      g_pD2DContext->SetTransform(D2D1::Matrix3x2F::Identity());
    }
  }

  if (pPeakHoldBrush) pPeakHoldBrush->Release();
  if (pTextBrush) pTextBrush->Release();
  if (pGreenBrush) pGreenBrush->Release();
  if (pYellowBrush) pYellowBrush->Release();
  if (pUnlitBrush) pUnlitBrush->Release();
  if (pClipBrush) pClipBrush->Release();
  if (pMuteBrush) pMuteBrush->Release();
  if (pPillCapBrush) pPillCapBrush->Release();
  if (pPillGlossStops) pPillGlossStops->Release();

  HRESULT hrDraw = g_pD2DContext->EndDraw();
  if (SUCCEEDED(hrDraw)) {
    g_pSwapChain->Present(1, 0);
  } else if (hrDraw == D2DERR_RECREATE_TARGET) {
    CleanupDirectComposition();
    InitDirectComposition(g_hHudWnd);
  }
}

// Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬ Settings Flyout Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬

static void DismissFlyout() {
  if (g_hFlyoutWnd) {
    DestroyWindow(g_hFlyoutWnd);
    g_hFlyoutWnd = NULL;
  }
}

static LRESULT CALLBACK FlyoutWndProc(HWND hWnd, UINT message, WPARAM wParam,
                                      LPARAM lParam) {
  switch (message) {
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT dpi = GetDpiForWindow(hWnd);
    SetMapMode(hdc, MM_ANISOTROPIC);
    SetWindowExtEx(hdc, 96, 96, NULL);
    SetViewportExtEx(hdc, dpi, dpi, NULL);
    rc.right = MulDiv(rc.right, 96, dpi);
    rc.bottom = MulDiv(rc.bottom, 96, dpi);
    if (!g_hFlyoutBgBrush)
      g_hFlyoutBgBrush = CreateSolidBrush(RGB(20, 20, 32));
    FillRect(hdc, &rc, g_hFlyoutBgBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(210, 215, 230));
    HFONT hFont =
        CreateFontW(-12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                    DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);

    RECT lblRc = {14, 14, 120, 34};
    DrawTextW(hdc, L"Opacity", -1, &lblRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT scaleRc = {14, 60, 120, 80};
    DrawTextW(hdc, L"HUD Scale", -1, &scaleRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT orientRc = {14, 106, 200, 124};
    DrawTextW(hdc, L"Orientation", -1, &orientRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT secRc = {14, 186, 200, 204};
    DrawTextW(hdc, L"Display", -1, &secRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT sec2Rc = {14, 318, 200, 336};
    DrawTextW(hdc, L"Input Device", -1, &sec2Rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT sec3Rc = {14, 380, 200, 398};
    DrawTextW(hdc, L"Behavior", -1, &sec3Rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Subtle section separator lines
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(55, 55, 75));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 14, 54, NULL); LineTo(hdc, rc.right - 14, 54);
    MoveToEx(hdc, 14, 100, NULL); LineTo(hdc, rc.right - 14, 100);
    MoveToEx(hdc, 14, 180, NULL); LineTo(hdc, rc.right - 14, 180);
    MoveToEx(hdc, 14, 312, NULL); LineTo(hdc, rc.right - 14, 312);
    MoveToEx(hdc, 14, 374, NULL); LineTo(hdc, rc.right - 14, 374);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    SelectObject(hdc, hOld);
    DeleteObject(hFont);
    EndPaint(hWnd, &ps);
    return 0;
  }

  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORLISTBOX:
  case WM_CTLCOLOREDIT: {
    HDC hdcCtrl = (HDC)wParam;
    SetBkMode(hdcCtrl, TRANSPARENT);
    SetTextColor(hdcCtrl, RGB(245, 245, 255));
    if (!g_hFlyoutBgBrush)
      g_hFlyoutBgBrush = CreateSolidBrush(RGB(20, 20, 32));
    return (LRESULT)g_hFlyoutBgBrush;
  }

  case WM_DRAWITEM: {
    LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
    if (pDIS && pDIS->CtlType == ODT_BUTTON) {
      HDC hdc = pDIS->hDC;
      RECT rc = pDIS->rcItem;

      HBRUSH hBg = CreateSolidBrush(RGB(20, 20, 32));
      FillRect(hdc, &rc, hBg);
      DeleteObject(hBg);

      WCHAR text[128] = {0};
      GetWindowTextW(pDIS->hwndItem, text, 128);

      BOOL isChecked = FALSE;
      if (pDIS->CtlID == FLYOUT_CTRL_BOTH) {
        isChecked = (g_Settings.showMic && g_Settings.showSystem);
      } else if (pDIS->CtlID == FLYOUT_CTRL_MIC_ONLY) {
        isChecked = (g_Settings.showMic && !g_Settings.showSystem);
      } else if (pDIS->CtlID == FLYOUT_CTRL_SYS_ONLY) {
        isChecked = (!g_Settings.showMic && g_Settings.showSystem);
      } else if (pDIS->CtlID == FLYOUT_CTRL_LAYOUT_HORZ) {
        isChecked = (_wcsicmp(g_Settings.layout, L"vertical") != 0);
      } else if (pDIS->CtlID == FLYOUT_CTRL_LAYOUT_VERT) {
        isChecked = (_wcsicmp(g_Settings.layout, L"vertical") == 0);
      } else {
        isChecked = (SendMessageW(pDIS->hwndItem, BM_GETCHECK, 0, 0) == BST_CHECKED);
      }

      int circleX = rc.left + 2;
      int circleY = rc.top + (rc.bottom - rc.top - 16) / 2;

      HBRUSH hDotBrush = CreateSolidBrush(RGB(0, 153, 255));
      HPEN hRingPen = CreatePen(PS_SOLID, 2, isChecked ? RGB(0, 153, 255) : RGB(140, 145, 160));
      HPEN hOldPen = (HPEN)SelectObject(hdc, hRingPen);
      HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

      Ellipse(hdc, circleX, circleY, circleX + 16, circleY + 16);
      if (isChecked) {
        SelectObject(hdc, hDotBrush);
        Ellipse(hdc, circleX + 3, circleY + 3, circleX + 13, circleY + 13);
      }

      SelectObject(hdc, hOldPen);
      SelectObject(hdc, hOldBrush);
      DeleteObject(hRingPen);
      DeleteObject(hDotBrush);

      SetBkMode(hdc, TRANSPARENT);
      SetTextColor(hdc, RGB(245, 245, 255));
      HFONT hFont = (HFONT)SendMessageW(pDIS->hwndItem, WM_GETFONT, 0, 0);
      HFONT hOldFont = nullptr;
      if (hFont)
        hOldFont = (HFONT)SelectObject(hdc, hFont);

      RECT textRc = rc;
      textRc.left += 24;
      DrawTextW(hdc, text, -1, &textRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

      if (hOldFont)
        SelectObject(hdc, hOldFont);
      return TRUE;
    }
    return 0;
  }

  case WM_HSCROLL: {
    HWND hSlider = GetDlgItem(hWnd, FLYOUT_CTRL_OPACITY);
    if ((HWND)lParam == hSlider) {
      int val = (int)SendMessageW(hSlider, TBM_GETPOS, 0, 0);
      g_Settings.opacity = val;
      Wh_SetIntValue(L"rt_opacity", val);
      HWND hLbl = GetDlgItem(hWnd, FLYOUT_CTRL_OP_LBL);
      if (hLbl) {
        WCHAR buf[16];
        swprintf_s(buf, L"%d%%", val);
        SetWindowTextW(hLbl, buf);
      }
      if (g_hHudWnd)
        InvalidateRect(g_hHudWnd, NULL, FALSE);
    } else if ((HWND)lParam == GetDlgItem(hWnd, FLYOUT_CTRL_SCALE)) {
      int val = (int)SendMessageW((HWND)lParam, TBM_GETPOS, 0, 0);
      HWND hLbl = GetDlgItem(hWnd, FLYOUT_CTRL_SCALE_LBL);
      if (hLbl) {
        WCHAR buf[16];
        swprintf_s(buf, L"%d%%", val);
        SetWindowTextW(hLbl, buf);
      }
      if (LOWORD(wParam) == TB_ENDTRACK || LOWORD(wParam) == TB_THUMBPOSITION) {
        g_Settings.hudScale = val;
        Wh_SetIntValue(L"rt_hudScale", val);
        if (g_hHudWnd) {
          PositionHudWindow();
          InitDirectComposition(g_hHudWnd);
          InvalidateRect(g_hHudWnd, NULL, FALSE);
        }
      }
    }
    return 0;
  }

  case WM_COMMAND: {
    int id = LOWORD(wParam);
    int notif = HIWORD(wParam);
    if (notif == BN_CLICKED) {
      HWND hBtn = (HWND)lParam;
      BOOL checked = (SendMessageW(hBtn, BM_GETCHECK, 0, 0) == BST_CHECKED);
      if (id == FLYOUT_CTRL_BOTH) {
        g_Settings.showMic = TRUE;
        g_Settings.showSystem = TRUE;
        Wh_SetStringValue(L"rt_showMeters", L"both");
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_BOTH), BM_SETCHECK, BST_CHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_MIC_ONLY), BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_SYS_ONLY), BM_SETCHECK, BST_UNCHECKED, 0);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_BOTH), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_MIC_ONLY), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_SYS_ONLY), NULL, FALSE);
      } else if (id == FLYOUT_CTRL_MIC_ONLY) {
        g_Settings.showMic = TRUE;
        g_Settings.showSystem = FALSE;
        Wh_SetStringValue(L"rt_showMeters", L"mic");
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_BOTH), BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_MIC_ONLY), BM_SETCHECK, BST_CHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_SYS_ONLY), BM_SETCHECK, BST_UNCHECKED, 0);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_BOTH), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_MIC_ONLY), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_SYS_ONLY), NULL, FALSE);
      } else if (id == FLYOUT_CTRL_SYS_ONLY) {
        g_Settings.showMic = FALSE;
        g_Settings.showSystem = TRUE;
        Wh_SetStringValue(L"rt_showMeters", L"system");
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_BOTH), BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_MIC_ONLY), BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_SYS_ONLY), BM_SETCHECK, BST_CHECKED, 0);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_BOTH), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_MIC_ONLY), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_SYS_ONLY), NULL, FALSE);
      } else if (id == FLYOUT_CTRL_LAYOUT_HORZ) {
        StringCchCopyW(g_Settings.layout, 32, L"horizontal");
        Wh_SetStringValue(L"rt_layout", L"horizontal");
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_HORZ), BM_SETCHECK, BST_CHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_VERT), BM_SETCHECK, BST_UNCHECKED, 0);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_HORZ), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_VERT), NULL, FALSE);
        if (g_hHudWnd) {
          PositionHudWindow();
          InitDirectComposition(g_hHudWnd);
          InvalidateRect(g_hHudWnd, NULL, FALSE);
        }
      } else if (id == FLYOUT_CTRL_LAYOUT_VERT) {
        StringCchCopyW(g_Settings.layout, 32, L"vertical");
        Wh_SetStringValue(L"rt_layout", L"vertical");
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_HORZ), BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_VERT), BM_SETCHECK, BST_CHECKED, 0);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_HORZ), NULL, FALSE);
        InvalidateRect(GetDlgItem(hWnd, FLYOUT_CTRL_LAYOUT_VERT), NULL, FALSE);
        if (g_hHudWnd) {
          PositionHudWindow();
          InitDirectComposition(g_hHudWnd);
          InvalidateRect(g_hHudWnd, NULL, FALSE);
        }
      } else if (id == FLYOUT_CTRL_SHOWLABELS) {
        g_Settings.showLabels = checked;
        Wh_SetIntValue(L"rt_showLabels", checked);
      } else if (id == FLYOUT_CTRL_CLICKTHRU) {
        g_Settings.clickThrough = checked;
        Wh_SetIntValue(L"rt_clickThrough", checked);
        if (g_hHudWnd) {
          LONG_PTR ex = GetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE);
          if (checked)
            ex |= WS_EX_TRANSPARENT;
          else
            ex &= ~WS_EX_TRANSPARENT;
          SetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE, ex);
        }
      } else if (id == FLYOUT_CTRL_PEAKHOLD) {
        g_Settings.enablePeakHold = checked;
        Wh_SetIntValue(L"rt_enablePeakHold", checked);
      }
      if (id == FLYOUT_CTRL_BOTH || id == FLYOUT_CTRL_MIC_ONLY ||
          id == FLYOUT_CTRL_SYS_ONLY)
        RefreshAudioTrackers();
      if (g_hHudWnd)
        InvalidateRect(g_hHudWnd, NULL, FALSE);
    } else if (notif == CBN_SELCHANGE && id == FLYOUT_CTRL_MIC_COMBO) {
      HWND hCombo = (HWND)lParam;
      int index = (int)SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
      if (index != CB_ERR) {
        WCHAR selText[256] = {0};
        SendMessageW(hCombo, CB_GETLBTEXT, index, (LPARAM)selText);
        StringCchCopyW(g_Settings.micDevice, 256, selText);
        Wh_SetStringValue(L"rt_micDevice", selText);
        if (g_bHudVisible && g_Settings.showMic)
          InitAudioTracker(eCapture, g_MicTracker);
        if (g_hHudWnd)
          InvalidateRect(g_hHudWnd, NULL, FALSE);
      }
    }
    return 0;
  }

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE)
      DismissFlyout();
    return 0;

  case WM_ACTIVATE:
    if (LOWORD(wParam) == WA_INACTIVE)
      DismissFlyout();
    return 0;

  case WM_DESTROY:
    if (g_hFlyoutControlFont) {
      DeleteObject(g_hFlyoutControlFont);
      g_hFlyoutControlFont = NULL;
    }
    g_hFlyoutWnd = NULL;
    return 0;

  default:
    return DefWindowProcW(hWnd, message, wParam, lParam);
  }
}

static void ShowSettingsFlyout() {
  if (g_hFlyoutWnd) {
    DismissFlyout();
    return;
  }

  if (!g_hHudWnd)
    return;

  RECT hudRect;
  GetWindowRect(g_hHudWnd, &hudRect);

  const int FLY_W = 250;
  const int FLY_H = 470;
  UINT dpi = GetDpiForWindow(g_hHudWnd);
  int flyW = MulDiv(FLY_W, dpi, 96);
  int flyH = MulDiv(FLY_H, dpi, 96);

  BOOL isVertical = (_wcsicmp(g_Settings.layout, L"vertical") == 0);
  int x = isVertical ? (hudRect.left + (hudRect.right - hudRect.left) / 2 - flyW / 2)
                     : (hudRect.right - flyW - 4);
  int y = hudRect.bottom + MulDiv(6, dpi, 96);

  MONITORINFO monitorInfo = {sizeof(monitorInfo)};
  GetMonitorInfoW(MonitorFromWindow(g_hHudWnd, MONITOR_DEFAULTTONEAREST),
                  &monitorInfo);
  RECT work = monitorInfo.rcWork;
  if (y + flyH > work.bottom)
    y = hudRect.top - flyH - MulDiv(6, dpi, 96);
  x = std::clamp(x, (int)work.left,
                 std::max((int)work.left, (int)work.right - flyW));
  y = std::clamp(y, (int)work.top,
                 std::max((int)work.top, (int)work.bottom - flyH));

  g_hFlyoutWnd =
      CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, FLYOUT_WINDOW_CLASS,
                      L"", WS_POPUP | WS_CLIPCHILDREN, x, y, flyW, flyH, NULL,
                      NULL, GetModuleHandle(NULL), NULL);

  if (!g_hFlyoutWnd)
    return;

  EnableDarkModeForControl(g_hFlyoutWnd);

  BOOL darkMode = TRUE;
  DwmSetWindowAttribute(g_hFlyoutWnd, 20, &darkMode, sizeof(darkMode));
  DWORD cornerPref = 2; // DWMWCP_ROUND
  DwmSetWindowAttribute(g_hFlyoutWnd, 33, &cornerPref, sizeof(cornerPref));

  g_hFlyoutControlFont =
      CreateFontW(-MulDiv(13, dpi, 96), 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

  HMODULE hInst = GetModuleHandle(NULL);
  const int PAD_X = 14;
  const int SLIDER_Y = 12;
  const int ROW_H = 24;

  auto ApplyControlStyle = [&](HWND hCtrl) {
    if (!hCtrl)
      return;
    EnableDarkModeForControl(hCtrl);
    if (g_hFlyoutControlFont)
      SendMessageW(hCtrl, WM_SETFONT, (WPARAM)g_hFlyoutControlFont, TRUE);
  };

  // Ã¢â€â‚¬Ã¢â€â‚¬ Opacity Row Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
  HWND hSlider = CreateWindowExW(0, TRACKBAR_CLASS, L"",
                                 WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
                                 PAD_X + 80, SLIDER_Y, 102, 26, g_hFlyoutWnd,
                                 (HMENU)FLYOUT_CTRL_OPACITY, hInst, NULL);
  SendMessageW(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
  SendMessageW(hSlider, TBM_SETPOS, TRUE, g_Settings.opacity);
  ApplyControlStyle(hSlider);

  WCHAR opBuf[16];
  swprintf_s(opBuf, L"%d%%", g_Settings.opacity);
  HWND hOpLbl =
      CreateWindowExW(0, L"STATIC", opBuf, WS_CHILD | WS_VISIBLE | SS_CENTER,
                      PAD_X + 80 + 106, SLIDER_Y + 4, 36, 18, g_hFlyoutWnd,
                      (HMENU)FLYOUT_CTRL_OP_LBL, hInst, NULL);
  ApplyControlStyle(hOpLbl);

  // Ã¢â€â‚¬Ã¢â€â‚¬ HUD Scale Row Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
  HWND hScaleSlider = CreateWindowExW(
      0, TRACKBAR_CLASS, L"", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
      PAD_X + 80, 58, 102, 26, g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_SCALE, hInst,
      NULL);
  SendMessageW(hScaleSlider, TBM_SETRANGE, TRUE, MAKELPARAM(25, 100));
  SendMessageW(hScaleSlider, TBM_SETPOS, TRUE, g_Settings.hudScale);
  ApplyControlStyle(hScaleSlider);

  WCHAR scaleBuf[16];
  swprintf_s(scaleBuf, L"%d%%", g_Settings.hudScale);
  HWND hScaleLbl =
      CreateWindowExW(0, L"STATIC", scaleBuf, WS_CHILD | WS_VISIBLE | SS_CENTER,
                      PAD_X + 80 + 106, 58 + 4, 36, 18, g_hFlyoutWnd,
                      (HMENU)FLYOUT_CTRL_SCALE_LBL, hInst, NULL);
  ApplyControlStyle(hScaleLbl);

  // Ã¢â€â‚¬Ã¢â€â‚¬ Orientation Row Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
  HWND hRadHorz =
      CreateWindowExW(0, WC_BUTTON, L"Horizontal Bar (Default)",
                      WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_GROUP,
                      PAD_X + 2, 124, FLY_W - PAD_X * 2, ROW_H, g_hFlyoutWnd,
                      (HMENU)FLYOUT_CTRL_LAYOUT_HORZ, hInst, NULL);
  ApplyControlStyle(hRadHorz);

  HWND hRadVert =
      CreateWindowExW(0, WC_BUTTON, L"Vertical Sidebar",
                      WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      PAD_X + 2, 148, FLY_W - PAD_X * 2, ROW_H, g_hFlyoutWnd,
                      (HMENU)FLYOUT_CTRL_LAYOUT_VERT, hInst, NULL);
  ApplyControlStyle(hRadVert);

  if (_wcsicmp(g_Settings.layout, L"vertical") == 0) {
    SendMessageW(hRadVert, BM_SETCHECK, BST_CHECKED, 0);
  } else {
    SendMessageW(hRadHorz, BM_SETCHECK, BST_CHECKED, 0);
  }

  // Ã¢â€â‚¬Ã¢â€â‚¬ Display Row Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
  HWND hRadBoth =
      CreateWindowExW(0, WC_BUTTON, L"Show Both Meters",
                      WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_GROUP,
                      PAD_X + 2, 204, FLY_W - PAD_X * 2, ROW_H, g_hFlyoutWnd,
                      (HMENU)FLYOUT_CTRL_BOTH, hInst, NULL);
  ApplyControlStyle(hRadBoth);

  HWND hRadMic = CreateWindowExW(
      0, WC_BUTTON, L"Show Microphone Only",
      WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, PAD_X + 2, 228, FLY_W - PAD_X * 2,
      ROW_H, g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_MIC_ONLY, hInst, NULL);
  ApplyControlStyle(hRadMic);

  HWND hRadSys = CreateWindowExW(
      0, WC_BUTTON, L"Show System Output Only",
      WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, PAD_X + 2, 252, FLY_W - PAD_X * 2,
      ROW_H, g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_SYS_ONLY, hInst, NULL);
  ApplyControlStyle(hRadSys);

  HWND hShowLblChk = CreateWindowExW(
      0, WC_BUTTON, L"  Show Text Labels (\"Mic\" / \"System\")",
      WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, PAD_X + 2, 278,
      FLY_W - PAD_X * 2, ROW_H, g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_SHOWLABELS,
      hInst, NULL);
  SendMessageW(hShowLblChk, BM_SETCHECK,
               g_Settings.showLabels ? BST_CHECKED : BST_UNCHECKED, 0);
  ApplyControlStyle(hShowLblChk);

  if (g_Settings.showMic && g_Settings.showSystem) {
    SendMessageW(hRadBoth, BM_SETCHECK, BST_CHECKED, 0);
  } else if (g_Settings.showMic) {
    SendMessageW(hRadMic, BM_SETCHECK, BST_CHECKED, 0);
  } else if (g_Settings.showSystem) {
    SendMessageW(hRadSys, BM_SETCHECK, BST_CHECKED, 0);
  }

  // Ã¢â€â‚¬Ã¢â€â‚¬ Input Device Row Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
  HWND hMicCombo = CreateWindowExW(
      0, WC_COMBOBOX, L"",
      CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED |
          WS_VISIBLE | WS_VSCROLL,
      PAD_X, 336, FLY_W - PAD_X * 2, 200,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_MIC_COMBO, hInst, NULL);
  ApplyControlStyle(hMicCombo);

  SendMessageW(hMicCombo, CB_ADDSTRING, 0, (LPARAM)L"default");
  SendMessageW(hMicCombo, CB_ADDSTRING, 0, (LPARAM)L"communications");

  IMMDeviceEnumerator *pEnum = nullptr;
  if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                                 __uuidof(IMMDeviceEnumerator),
                                 (void **)&pEnum))) {
    auto AddEndpointsToCombo = [&](EDataFlow flow) {
      IMMDeviceCollection *pCol = nullptr;
      if (SUCCEEDED(
              pEnum->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, &pCol)) &&
          pCol) {
        UINT count = 0;
        pCol->GetCount(&count);
        for (UINT i = 0; i < count; i++) {
          IMMDevice *pEndpoint = nullptr;
          if (SUCCEEDED(pCol->Item(i, &pEndpoint)) && pEndpoint) {
            IPropertyStore *pProps = nullptr;
            if (SUCCEEDED(pEndpoint->OpenPropertyStore(STGM_READ, &pProps)) &&
                pProps) {
              PROPVARIANT varName;
              PropVariantInit(&varName);
              if (SUCCEEDED(
                      pProps->GetValue(PKEY_Device_FriendlyName, &varName))) {
                if (varName.vt == VT_LPWSTR && varName.pwszVal) {
                  if (SendMessageW(hMicCombo, CB_FINDSTRINGEXACT, -1,
                                   (LPARAM)varName.pwszVal) == CB_ERR) {
                    SendMessageW(hMicCombo, CB_ADDSTRING, 0,
                                 (LPARAM)varName.pwszVal);
                  }
                }
                PropVariantClear(&varName);
              }
              pProps->Release();
            }
            pEndpoint->Release();
          }
        }
        pCol->Release();
      }
    };

    AddEndpointsToCombo(eCapture);
    AddEndpointsToCombo(eRender);
    pEnum->Release();
  }

  if (wcslen(g_Settings.micDevice) > 0) {
    int index = (int)SendMessageW(hMicCombo, CB_FINDSTRINGEXACT, -1,
                                  (LPARAM)g_Settings.micDevice);
    if (index != CB_ERR) {
      SendMessageW(hMicCombo, CB_SETCURSEL, index, 0);
    } else {
      index = (int)SendMessageW(hMicCombo, CB_FINDSTRING, -1,
                                (LPARAM)g_Settings.micDevice);
      if (index != CB_ERR)
        SendMessageW(hMicCombo, CB_SETCURSEL, index, 0);
    }
  }

  // Ã¢â€â‚¬Ã¢â€â‚¬ Behavior Row Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬Ã¢â€â‚¬
  HWND hCtChk = CreateWindowExW(
      0, WC_BUTTON, L"  Click-Through", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
      PAD_X + 2, 396, FLY_W - PAD_X * 2, ROW_H, g_hFlyoutWnd,
      (HMENU)FLYOUT_CTRL_CLICKTHRU, hInst, NULL);
  SendMessageW(hCtChk, BM_SETCHECK,
               g_Settings.clickThrough ? BST_CHECKED : BST_UNCHECKED, 0);
  ApplyControlStyle(hCtChk);

  HWND hPkChk =
      CreateWindowExW(0, WC_BUTTON, L"  Show Peak Hold Marker",
                      WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, PAD_X + 2, 420,
                      FLY_W - PAD_X * 2, ROW_H, g_hFlyoutWnd,
                      (HMENU)FLYOUT_CTRL_PEAKHOLD, hInst, NULL);
  SendMessageW(hPkChk, BM_SETCHECK,
               g_Settings.enablePeakHold ? BST_CHECKED : BST_UNCHECKED, 0);
  ApplyControlStyle(hPkChk);

  if (dpi != 96) {
    EnumChildWindows(g_hFlyoutWnd,
                     [](HWND child, LPARAM dpiValue) -> BOOL {
                       HWND parent = GetParent(child);
                       RECT rect;
                       GetWindowRect(child, &rect);
                       MapWindowPoints(HWND_DESKTOP, parent,
                                       reinterpret_cast<POINT *>(&rect), 2);
                       int width = rect.right - rect.left;
                       int height = rect.bottom - rect.top;
                       int targetDpi = static_cast<int>(dpiValue);
                       SetWindowPos(child, NULL,
                                    MulDiv(rect.left, targetDpi, 96),
                                    MulDiv(rect.top, targetDpi, 96),
                                    MulDiv(width, targetDpi, 96),
                                    MulDiv(height, targetDpi, 96),
                                    SWP_NOZORDER | SWP_NOACTIVATE);
                       return TRUE;
                     },
                     dpi);
  }

  ShowWindow(g_hFlyoutWnd, SW_SHOWNOACTIVATE);
  SetForegroundWindow(g_hFlyoutWnd);
}

// Window Procedure for HUD Overlay
static LRESULT CALLBACK HudWndProc(HWND hWnd, UINT message, WPARAM wParam,
                                   LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    InitDirectComposition(hWnd);
    SetTimer(hWnd, HUD_TIMER_ID, 1000 / g_Settings.fpsLimit, NULL);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    RenderHud();
    EndPaint(hWnd, &ps);
    return 0;
  }

  case WM_HUD_RELOAD_SETTINGS:
    LoadModSettings();
    PositionHudWindow();
    RegisterGlobalHotkey();
    ApplyCornerPreference(hWnd);
    RefreshAudioTrackers();
    InitDirectComposition(hWnd);
    InvalidateRect(hWnd, NULL, FALSE);
    return 0;

  case WM_LBUTTONDOWN: {
    if (!g_Settings.clickThrough) {
      int x = GET_X_LPARAM(lParam);
      int y = GET_Y_LPARAM(lParam);
      RECT rc;
      GetClientRect(hWnd, &rc);
      float scale = g_Settings.hudScale / 100.0f *
                    GetDpiForWindow(hWnd) / 96.0f;
      if (scale < 0.25f)
        scale = 0.25f;
      BOOL isVertical = (_wcsicmp(g_Settings.layout, L"vertical") == 0);
      if (isVertical) {
        if (y <= (int)roundf(32 * scale)) {
          ShowSettingsFlyout();
        }
      } else {
        int hitArea = (int)roundf(35 * scale);
        if (x >= rc.right - hitArea) {
          ShowSettingsFlyout();
        }
      }
    }
    return 0;
  }

  case WM_SIZE:
    if (g_hHudWnd == hWnd && LOWORD(lParam) && HIWORD(lParam))
      InitDirectComposition(hWnd);
    return 0;

  case WM_DPICHANGED:
    DismissFlyout();
    if (lParam) {
      const RECT *suggested = reinterpret_cast<const RECT *>(lParam);
      SetWindowPos(hWnd, NULL, suggested->left, suggested->top,
                   suggested->right - suggested->left,
                   suggested->bottom - suggested->top,
                   SWP_NOZORDER | SWP_NOACTIVATE);
    }
    PositionHudWindow();
    InitDirectComposition(hWnd);
    InvalidateRect(hWnd, NULL, FALSE);
    return 0;

  case WM_TIMER:
    if (wParam == HUD_TIMER_ID && g_bHudVisible) {
      UpdateAudioLevels();
      InvalidateRect(hWnd, NULL, FALSE);
    }
    return 0;

  case WM_HOTKEY:
    if (wParam == HUD_HOTKEY_ID) {
      g_bHudVisible = !g_bHudVisible;
      Wh_SetIntValue(L"rt_hudVisible", g_bHudVisible);
      RefreshAudioTrackers();
      ShowWindow(hWnd, g_bHudVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
    } else if (wParam == HUD_CLICKTHRU_HOTKEY_ID) {
      g_Settings.clickThrough = !g_Settings.clickThrough;
      Wh_SetIntValue(L"rt_clickThrough", g_Settings.clickThrough);
      Wh_SetIntValue(L"rt_lastWhClickThrough", g_Settings.clickThrough);
      PositionHudWindow();
      if (g_hFlyoutWnd) {
        HWND hChk = GetDlgItem(g_hFlyoutWnd, FLYOUT_CTRL_CLICKTHRU);
        if (hChk)
          SendMessageW(hChk, BM_SETCHECK,
                       g_Settings.clickThrough ? BST_CHECKED : BST_UNCHECKED,
                       0);
      }
      InvalidateRect(hWnd, NULL, FALSE);
    }
    return 0;

  case WM_NCHITTEST: {
    if (g_Settings.clickThrough) {
      return HTTRANSPARENT;
    }
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    ScreenToClient(hWnd, &pt);
    RECT rc;
    GetClientRect(hWnd, &rc);
    float scale = g_Settings.hudScale / 100.0f *
                  GetDpiForWindow(hWnd) / 96.0f;
    if (scale < 0.25f)
      scale = 0.25f;
    BOOL isVertical = (_wcsicmp(g_Settings.layout, L"vertical") == 0);
    if (isVertical) {
      if (pt.y <= (int)roundf(32 * scale)) {
        return HTCLIENT; // Allow clicking three-dot menu at top of vertical HUD
      }
    } else {
      int hitArea = (int)roundf(35 * scale);
      if (pt.x >= rc.right - hitArea) {
        return HTCLIENT; // Allow LBUTTONDOWN on the dots
      }
    }
    return HTCAPTION; // Allow dragging everywhere else
  }

  case WM_EXITSIZEMOVE: {
    if (_wcsicmp(g_Settings.position, L"custom") == 0) {
      RECT rect;
      if (GetWindowRect(hWnd, &rect)) {
        Wh_SetIntValue(L"custom_x", rect.left);
        Wh_SetIntValue(L"custom_y", rect.top);
      }
    }
    return 0;
  }

  case WM_DISPLAYCHANGE:
    PositionHudWindow();
    return 0;

  case WM_CLOSE:
    DestroyWindow(hWnd);
    return 0;

  case WM_DESTROY:
    DismissFlyout();
    KillTimer(hWnd, HUD_TIMER_ID);
    UnregisterGlobalHotkey();
    CleanupDirectComposition();
    g_hHudWnd = NULL;
    PostQuitMessage(0);
    return 0;

  default:
    return DefWindowProcW(hWnd, message, wParam, lParam);
  }
}

// Dedicated Message Loop Thread Proc
static DWORD WINAPI HudThreadProc(LPVOID lpParam) {
  SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0)
    return 0;

  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

  INITCOMMONCONTROLSEX icex = {0};
  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC = ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
  InitCommonControlsEx(&icex);

  // Register Window Class
  WNDCLASSW wc = {0};
  wc.lpfnWndProc = HudWndProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = HUD_WINDOW_CLASS;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  RegisterClassW(&wc);

  // Register Flyout Class
  WNDCLASSW fw = {0};
  fw.lpfnWndProc = FlyoutWndProc;
  fw.hInstance = GetModuleHandle(NULL);
  fw.lpszClassName = FLYOUT_WINDOW_CLASS;
  fw.hCursor = LoadCursor(NULL, IDC_ARROW);
  RegisterClassW(&fw);

  if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
    UnregisterClassW(HUD_WINDOW_CLASS, GetModuleHandle(NULL));
    UnregisterClassW(FLYOUT_WINDOW_CLASS, GetModuleHandle(NULL));
    CoUninitialize();
    return 0;
  }

  // Create Topmost Window in ZBID_UIACCESS System Band for DirectComposition
  float scale = g_Settings.hudScale / 100.0f;
  if (scale < 0.25f)
    scale = 0.25f;
  float dpiScale = GetDpiForSystem() / 96.0f;

  BOOL isVertical = (_wcsicmp(g_Settings.layout, L"vertical") == 0);
  int baseW = isVertical ? 104 : 500;
  int baseH = isVertical ? 420 : 104;

  DWORD exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                  WS_EX_NOREDIRECTIONBITMAP;
  if (g_Settings.clickThrough) {
    exStyle |= (WS_EX_LAYERED | WS_EX_TRANSPARENT);
  }

  g_hHudWnd = CreateWindowExW(
      exStyle, HUD_WINDOW_CLASS, L"Audio Level HUD", WS_POPUP, 0, 0,
      (int)roundf(baseW * scale * dpiScale),
      (int)roundf(baseH * scale * dpiScale), NULL, NULL,
      GetModuleHandle(NULL), NULL);

  if (!g_hHudWnd) {
    UnregisterClassW(HUD_WINDOW_CLASS, GetModuleHandle(NULL));
    UnregisterClassW(FLYOUT_WINDOW_CLASS, GetModuleHandle(NULL));
    CoUninitialize();
    return 0;
  }

  if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
    DestroyWindow(g_hHudWnd);
    UnregisterClassW(HUD_WINDOW_CLASS, GetModuleHandle(NULL));
    UnregisterClassW(FLYOUT_WINDOW_CLASS, GetModuleHandle(NULL));
    CoUninitialize();
    return 0;
  }

  if (g_Settings.clickThrough) {
    SetLayeredWindowAttributes(g_hHudWnd, 0, 255, LWA_ALPHA);
  }

  PositionHudWindow();
  RegisterGlobalHotkey();
  ApplyCornerPreference(g_hHudWnd);

  // Initial audio setup
  RefreshAudioTrackers();

  ShowWindow(g_hHudWnd, g_bHudVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
  if (g_bHudVisible)
    UpdateWindow(g_hHudWnd);

  // Message Pump Loop
  MSG msg;
  while (GetMessageW(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  // Cleanup resources on thread exit
  g_MicTracker.Release();
  g_SystemTracker.Release();
  if (g_pEnumerator) {
    g_pEnumerator->Release();
    g_pEnumerator = nullptr;
  }

  UnregisterClassW(HUD_WINDOW_CLASS, GetModuleHandle(NULL));
  UnregisterClassW(FLYOUT_WINDOW_CLASS, GetModuleHandle(NULL));
  if (g_hFlyoutBgBrush) {
    DeleteObject(g_hFlyoutBgBrush);
    g_hFlyoutBgBrush = NULL;
  }
  CoUninitialize();
  return 0;
}

// Windhawk Lifecycle Initialization Callback
BOOL WhTool_ModInit() {
  Wh_Log(L"Initializing Audio Level HUD mod...");

  LoadModSettings();

  g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
  if (!g_hStopEvent)
    return FALSE;

  // Spawn dedicated UI message loop thread
  g_hHudThread = CreateThread(NULL, 0, HudThreadProc, NULL, 0, &g_dwThreadId);
  if (!g_hHudThread) {
    Wh_Log(L"Failed to create Audio Level HUD thread.");
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
    return FALSE;
  }

  Wh_Log(L"Audio Level HUD thread created successfully.");
  return TRUE;
}

// Windhawk Lifecycle Uninitialization Callback
void WhTool_ModUninit() {
  Wh_Log(L"Uninitializing Audio Level HUD mod...");

  if (g_hStopEvent)
    SetEvent(g_hStopEvent);

  if (g_hHudWnd) {
    SendMessageW(g_hHudWnd, WM_CLOSE, 0, 0);
  }

  if (g_hHudThread) {
    WaitForSingleObject(g_hHudThread, INFINITE);
    CloseHandle(g_hHudThread);
    g_hHudThread = NULL;
  }
  if (g_hStopEvent) {
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
  }

  Wh_Log(L"Audio Level HUD uninitialized.");
}

// Windhawk Settings Changed Callback
void WhTool_ModSettingsChanged() {
  Wh_Log(L"Settings changed, posting reload message to UI thread...");
  if (g_hHudWnd) {
    PostMessageW(g_hHudWnd, WM_HUD_RELOAD_SETTINGS, 0, 0);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
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
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
