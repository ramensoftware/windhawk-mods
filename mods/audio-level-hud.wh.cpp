// ==WindhawkMod==
// @id              audio-level-hud
// @name            Audio Level HUD
// @description     A small, always-on-top, Fluent 2-styled overlay showing live VU meters for mic input and system output.
// @version         1.0.0
// @author          AKS HAY
// @github          https://github.com/sysakshay
// @include         explorer.exe
// @compilerOptions -lole32 -lmmdevapi -ld3d11 -ldxgi -ldcomp -ld2d1 -ldwrite -luser32 -lgdi32 -ldwmapi -lcomctl32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Audio Level HUD

A lightweight, always-on-top, Fluent 2-styled desktop overlay for Windows that
displays real-time VU meters for both **Microphone Input** and **System Audio
Output (Desktop Loopback)**.

![Preview](https://raw.githubusercontent.com/sysakshay/audio-level-hud-wh/main/previews/preview1.png)
![Settings Preview](https://raw.githubusercontent.com/sysakshay/audio-level-hud-wh/main/previews/preview2.png)

## Key Features
- **DirectComposition & DXGI SwapChain**: Hardware-accelerated presentation with `DXGI_ALPHA_MODE_PREMULTIPLIED` for pixel-perfect anti-aliased rounded corners without GDI artifacts or corner leak lines.
- **Liquid Glass Design**: Real-time DWM acrylic blur sampling, specular rim lighting, top glass reflection sheen, and 3D glossy level meter pills.
- **Dual Real-time VU Meters**: WASAPI peak meters for mic capture and system audio output.
- **Overlay Customization**: Corner snapping, custom drag-and-drop positioning, opacity, and click-through mode.
- **Global Hotkey Toggle**: Default `Ctrl+Shift+M` shortcut to show/hide the HUD.

---
*Created with Windhawk.*
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
  $description: 'Choose which audio meters to display. 💡 ᴛᴏ ꜱᴇʟᴇᴄᴛ ʏᴏᴜʀ ɪɴᴘᴜᴛ ᴅᴇᴠɪᴄᴇ, ᴘʟᴇᴀꜱᴇ ᴜꜱᴇ ᴛʜᴇ ᴛʜʀᴇᴇ-ᴅᴏᴛ (⋮) ᴍᴇɴᴜ ᴏɴ ᴛʜᴇ ʜᴜᴅ ɪᴛꜱᴇʟꜰ!'
  $options:
    - both: Show Both Meters
    - mic: Show Microphone Only
    - system: Show System Output Only
- position: top-right
  $name: Screen Position
  $description: Screen corner position to dock the overlay HUD.
  $options:
    - top-right: Top Right
    - top-left: Top Left
    - bottom-right: Bottom Right
    - bottom-left: Bottom Left
    - custom: Custom (double click and drag the hud overlay to your desired location)
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
- toggle_hotkey: "Ctrl+Shift+M"
  $name: Toggle Hotkey
  $description: Global shortcut string to show/hide the HUD (e.g., Ctrl+Shift+M, Alt+Shift+A).
- fps_limit: 30
  $name: Refresh Rate (FPS)
  $description: Target update frequency for the level meters.
  $options:
    - 15: 15 FPS (Ultra Low Overhead)
    - 30: 30 FPS (Balanced - Recommended)
    - 60: 60 FPS (Ultra Smooth)
- color_theme: fluent
  $name: Color Theme
  $description: Visual theme for the VU level bars.
  $options:
    - fluent: Fluent Standard (Green & Amber)
    - neon: Cyber Neon (Cyan & Magenta)
    - emerald: Emerald Glow (Green & Lime)
    - sunset: Sunset Amber (Orange & Gold)
    - monochrome: Minimal Silver
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <commctrl.h>
#include <uxtheme.h>
#include <audiopolicy.h>
#include <d3d11.h>
#include <dcomp.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi1_2.h>
#include <endpointvolume.h>
#include <math.h>
#include <mmdeviceapi.h>
#include <propidl.h>
#include <string>
#include <strsafe.h>
#include <windhawk_api.h>
#include <windows.h>
#include <windowsx.h>

#define HUD_WINDOW_CLASS     L"Windhawk_AudioLevelHud_Class"
#define FLYOUT_WINDOW_CLASS  L"Windhawk_AudioLevelHud_Flyout"
#define HUD_TIMER_ID         1001
#define HUD_HOTKEY_ID        2001
#define WM_HUD_RELOAD_SETTINGS (WM_USER + 101)
// Flyout control IDs
#define FLYOUT_CTRL_OPACITY  3001
#define FLYOUT_CTRL_OP_LBL   3002
#define FLYOUT_CTRL_BOTH     3003
#define FLYOUT_CTRL_MIC_ONLY 3004
#define FLYOUT_CTRL_SYS_ONLY 3005
#define FLYOUT_CTRL_CLICKTHRU 3006
#define FLYOUT_CTRL_MIC_COMBO 3007
#define FLYOUT_CTRL_MIC_LBL  3008
#define FLYOUT_CTRL_SCALE    3009
#define FLYOUT_CTRL_SCALE_LBL 3010
#define FLYOUT_CTRL_PEAKHOLD 3011

#ifndef PKEY_Device_FriendlyName
static const PROPERTYKEY PKEY_Device_FriendlyName_Local = {
    {0xa45c254e,
     0xdf1c,
     0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};
#define PKEY_Device_FriendlyName PKEY_Device_FriendlyName_Local
#endif

// Interface definition for IAudioMeterInformation in case endpointvolume.h only forward-declares it
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

// Undocumented SetWindowCompositionAttribute definitions for native Windows Fluent Acrylic blur
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
  IAudioMeterInformation *pMeter = nullptr;
  IAudioEndpointVolume *pVolume = nullptr;
  float currentLevel = 0.0f;
  float peakHold = 0.0f;
  DWORD peakHoldTime = 0;
  DWORD lastUpdateTick = 0;
  DWORD clipTime = 0;
  BOOL isMuted = FALSE;
  BOOL isClipping = FALSE;

  void Release() {
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
  BOOL clickThrough = TRUE;
  WCHAR toggleHotkey[64] = L"Ctrl+Shift+M";
  int fpsLimit = 30;
  WCHAR colorTheme[32] = L"fluent";
  WCHAR micDevice[64] = L"default";
  int hudScale = 100;
  BOOL enablePeakHold = TRUE;
  int peakHoldDuration = 300;
};

static ModSettings g_Settings;
static HWND g_hHudWnd = NULL;
static HWND g_hFlyoutWnd = NULL;     // Settings flyout popup window
static HBRUSH g_hFlyoutBgBrush = NULL; // Reusable dark bg brush for flyout controls
static HANDLE g_hHudThread = NULL;
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
static IDWriteTextFormat *g_pValueFontFormat = nullptr;

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
static void ApplyAcrylicBlur(HWND hWnd);
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
static LRESULT CALLBACK FlyoutWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void ShowSettingsFlyout();
static void DismissFlyout();
// Native Fluent 2 Windows Acrylic Blur effect
static void ApplyAcrylicBlur(HWND hWnd) {
  if (!hWnd)
    return;

  // DWMWCP_DONOTROUND (1): Don't let DWM apply its own ~8px corner clip.
  // With WS_EX_NOREDIRECTIONBITMAP + DirectComposition DXGI_ALPHA_MODE_PREMULTIPLIED,
  // D2D's transparent (0,0,0,0) corner pixels define the visual shape naturally.
  // DWMWCP_ROUND (2) conflicts by imposing a ~8px DWM clip over D2D's 16px curves,
  // causing rectangular corner bleed artifacts.
  DWORD cornerPref = 1; // DWMWCP_DONOTROUND = 1
  DwmSetWindowAttribute(hWnd, 33, &cornerPref, sizeof(cornerPref));

  // NOTE: SetWindowCompositionAttribute is intentionally NOT used here.
  // We use WS_EX_NOREDIRECTIONBITMAP + DirectComposition SwapChain1 with
  // DXGI_ALPHA_MODE_PREMULTIPLIED. Using ACCENT_ENABLE_ACRYLICBLURBEHIND
  // alongside DirectComposition causes the DWM to apply acrylic to the full
  // rectangular window bounds, bleeding opaque blur outside the D2D rounded
  // rect and producing visible square corner artifacts. DirectComposition
  // handles compositing natively via premultiplied alpha — no accent policy needed.
}

// Initialize Audio Endpoint (Mic capture or System render)
static void InitAudioTracker(EDataFlow dataFlow,
                             AudioEndpointTracker &tracker) {
  tracker.Release();

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
      IMMDeviceCollection *pCol = nullptr;
      HRESULT hrCol = g_pEnumerator->EnumAudioEndpoints(
          eCapture, DEVICE_STATE_ACTIVE, &pCol);
      if (SUCCEEDED(hrCol) && pCol) {
        UINT count = 0;
        pCol->GetCount(&count);

        for (UINT i = 0; i < count; i++) {
          IMMDevice *pCandidate = nullptr;
          if (SUCCEEDED(pCol->Item(i, &pCandidate)) && pCandidate) {
            IPropertyStore *pProps = nullptr;
            if (SUCCEEDED(pCandidate->OpenPropertyStore(STGM_READ, &pProps)) &&
                pProps) {
              PROPVARIANT varName;
              PropVariantInit(&varName);
              if (SUCCEEDED(
                      pProps->GetValue(PKEY_Device_FriendlyName, &varName))) {
                if (varName.vt == VT_LPWSTR && varName.pwszVal) {
                  std::wstring friendlyName = varName.pwszVal;
                  std::wstring targetName = g_Settings.micDevice;

                  std::transform(friendlyName.begin(), friendlyName.end(),
                                 friendlyName.begin(), ::towlower);
                  std::transform(targetName.begin(), targetName.end(),
                                 targetName.begin(), ::towlower);

                  if (friendlyName.find(targetName) != std::wstring::npos) {
                    pDevice = pCandidate;
                    pDevice->AddRef();
                    Wh_Log(L"Matched microphone endpoint: %s", varName.pwszVal);
                  }
                }
                PropVariantClear(&varName);
              }
              pProps->Release();
            }
            pCandidate->Release();
            if (pDevice)
              break;
          }
        }
        pCol->Release();
      }

      if (!pDevice) {
        g_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
      }
    }
  } else {
    g_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
  }

  if (pDevice) {
    tracker.pDevice = pDevice;
    pDevice->Activate(IID_IAudioMeterInfo, CLSCTX_ALL, NULL,
                      (void **)&tracker.pMeter);
    pDevice->Activate(IID_IAudioEndVol, CLSCTX_ALL, NULL,
                      (void **)&tracker.pVolume);
  }
}

// Update Audio Levels from WASAPI peak meters
static void UpdateAudioLevels() {
  DWORD currentTick = GetTickCount();

  // 1. Update Microphone Input Level
  if (g_Settings.showMic) {
    if (!g_MicTracker.pMeter) {
      InitAudioTracker(eCapture, g_MicTracker);
    }

    if (g_MicTracker.pMeter) {
      float rawPeak = 0.0f;
      HRESULT hr = g_MicTracker.pMeter->GetPeakValue(&rawPeak);
      if (FAILED(hr)) {
        InitAudioTracker(eCapture, g_MicTracker);
      } else {
        if (g_MicTracker.pVolume) {
          g_MicTracker.pVolume->GetMute(&g_MicTracker.isMuted);
        }

        float dt = (g_MicTracker.lastUpdateTick > 0) ? (currentTick - g_MicTracker.lastUpdateTick) / 1000.0f : 0.033f;
        if (dt > 0.1f) dt = 0.033f;
        g_MicTracker.lastUpdateTick = currentTick;

        if (rawPeak >= g_MicTracker.currentLevel) {
          g_MicTracker.currentLevel = rawPeak;
        } else {
          g_MicTracker.currentLevel =
              std::max(rawPeak, g_MicTracker.currentLevel - dt * 2.2f);
        }

        int holdDuration = g_Settings.peakHoldDuration;
        if (holdDuration <= 0) holdDuration = 300;

        if (rawPeak >= g_MicTracker.peakHold) {
          g_MicTracker.peakHold = rawPeak;
          g_MicTracker.peakHoldTime = currentTick;
        } else if (currentTick - g_MicTracker.peakHoldTime > (DWORD)holdDuration) {
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

  // 2. Update System Audio Output Level (WASAPI Loopback)
  if (g_Settings.showSystem) {
    if (!g_SystemTracker.pMeter) {
      InitAudioTracker(eRender, g_SystemTracker);
    }

    if (g_SystemTracker.pMeter) {
      float rawPeak = 0.0f;
      HRESULT hr = g_SystemTracker.pMeter->GetPeakValue(&rawPeak);
      if (FAILED(hr)) {
        InitAudioTracker(eRender, g_SystemTracker);
      } else {
        if (g_SystemTracker.pVolume) {
          g_SystemTracker.pVolume->GetMute(&g_SystemTracker.isMuted);
        }

        float dt = (g_SystemTracker.lastUpdateTick > 0) ? (currentTick - g_SystemTracker.lastUpdateTick) / 1000.0f : 0.033f;
        if (dt > 0.1f) dt = 0.033f;
        g_SystemTracker.lastUpdateTick = currentTick;

        if (rawPeak >= g_SystemTracker.currentLevel) {
          g_SystemTracker.currentLevel = rawPeak;
        } else {
          g_SystemTracker.currentLevel =
              std::max(rawPeak, g_SystemTracker.currentLevel - dt * 2.2f);
        }

        int holdDuration = g_Settings.peakHoldDuration;
        if (holdDuration <= 0) holdDuration = 300;

        if (rawPeak >= g_SystemTracker.peakHold) {
          g_SystemTracker.peakHold = rawPeak;
          g_SystemTracker.peakHoldTime = currentTick;
        } else if (currentTick - g_SystemTracker.peakHoldTime > (DWORD)holdDuration) {
          float newPeak = g_SystemTracker.peakHold - dt * 1.2f;
          g_SystemTracker.peakHold = std::max(g_SystemTracker.currentLevel, newPeak);
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

// Parse string like "Ctrl+Shift+M" into Win32 hotkey flags
static void RegisterGlobalHotkey() {
  if (!g_hHudWnd || wcslen(g_Settings.toggleHotkey) == 0)
    return;

  UnregisterGlobalHotkey();

  UINT fsModifiers = MOD_NOREPEAT;
  UINT vkCode = 0;

  WCHAR buffer[64];
  StringCchCopyW(buffer, 64, g_Settings.toggleHotkey);

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
      if (ch >= L'A' && ch <= L'Z') {
        vkCode = ch;
      } else if (ch >= L'0' && ch <= L'9') {
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
    RegisterHotKey(g_hHudWnd, HUD_HOTKEY_ID, fsModifiers, vkCode);
  }
}

static void UnregisterGlobalHotkey() {
  if (g_hHudWnd) {
    UnregisterHotKey(g_hHudWnd, HUD_HOTKEY_ID);
  }
}

// Load Mod Settings from Windhawk API
static void LoadModSettings() {
  PCWSTR strVal = nullptr;

  strVal = Wh_GetStringSetting(L"position");
  if (strVal) {
    StringCchCopyW(g_Settings.position, 32, strVal);
    Wh_FreeStringSetting(strVal);
  }

  PCWSTR scaleValStr = Wh_GetStringSetting(L"hud_scale");
  int windhawkScale = 100;
  if (scaleValStr) {
    windhawkScale = _wtoi(scaleValStr);
    Wh_FreeStringSetting(scaleValStr);
  }
  if (windhawkScale <= 0) windhawkScale = 100;
  int lastWindhawkScale = Wh_GetIntValue(L"rt_lastWindhawkScale", -1);
  
  if (lastWindhawkScale != windhawkScale) {
    Wh_SetIntValue(L"rt_lastWindhawkScale", windhawkScale);
    Wh_DeleteValue(L"rt_hudScale");
    g_Settings.hudScale = windhawkScale;
  } else {
    int savedScale = Wh_GetIntValue(L"rt_hudScale", -1);
    if (savedScale != -1) {
      g_Settings.hudScale = savedScale;
    } else {
      g_Settings.hudScale = windhawkScale;
    }
  }

  g_Settings.enablePeakHold = Wh_GetIntValue(L"rt_enablePeakHold", Wh_GetIntSetting(L"enable_peak_hold"));

  PCWSTR peakDurStr = Wh_GetStringSetting(L"peak_hold_duration");
  if (peakDurStr) {
    g_Settings.peakHoldDuration = _wtoi(peakDurStr);
    Wh_FreeStringSetting(peakDurStr);
  } else {
    g_Settings.peakHoldDuration = 300;
  }

  PCWSTR opStr = Wh_GetStringSetting(L"card_opacity");
  if (opStr) {
    g_Settings.opacity = _wtoi(opStr);
    Wh_FreeStringSetting(opStr);
  } else {
    g_Settings.opacity = 88;
  }
  if (g_Settings.opacity < 0) g_Settings.opacity = 0;
  if (g_Settings.opacity > 100) g_Settings.opacity = 100;
  
  PCWSTR showMeters = Wh_GetStringSetting(L"show_meters");
  if (showMeters) {
    if (_wcsicmp(showMeters, L"mic") == 0) {
      g_Settings.showMic = TRUE;
      g_Settings.showSystem = FALSE;
    } else if (_wcsicmp(showMeters, L"system") == 0) {
      g_Settings.showMic = FALSE;
      g_Settings.showSystem = TRUE;
    } else {
      g_Settings.showMic = TRUE;
      g_Settings.showSystem = TRUE;
    }
    Wh_FreeStringSetting(showMeters);
  }

  g_Settings.clickThrough = Wh_GetIntSetting(L"click_through");

  strVal = Wh_GetStringSetting(L"toggle_hotkey");
  if (strVal) {
    StringCchCopyW(g_Settings.toggleHotkey, 64, strVal);
    Wh_FreeStringSetting(strVal);
  }

  g_Settings.fpsLimit = Wh_GetIntSetting(L"fps_limit");
  if (g_Settings.fpsLimit <= 0)
    g_Settings.fpsLimit = 30;

  strVal = Wh_GetStringSetting(L"color_theme");
  if (strVal) {
    StringCchCopyW(g_Settings.colorTheme, 32, strVal);
    Wh_FreeStringSetting(strVal);
  }

  WCHAR savedMic[64] = {0};
  if (Wh_GetStringValue(L"rt_micDevice", savedMic, ARRAYSIZE(savedMic))) {
    StringCchCopyW(g_Settings.micDevice, 64, savedMic);
  } else {
    StringCchCopyW(g_Settings.micDevice, 64, L"default");
  }

}

// Position HUD according to monitor work area and settings
static void PositionHudWindow() {
  if (!g_hHudWnd)
    return;

  HMONITOR hMonitor = MonitorFromWindow(g_hHudWnd, MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO mi = {sizeof(MONITORINFO)};
  GetMonitorInfoW(hMonitor, &mi);

  RECT workArea = mi.rcWork;
  
  float scale = g_Settings.hudScale / 100.0f;
  if (scale < 0.25f) scale = 0.25f;
  int w = (int)roundf(500 * scale);
  int h = (int)roundf(104 * scale);

  int x = workArea.right - w - 24;
  int y = workArea.top + 24;

  if (_wcsicmp(g_Settings.position, L"custom") == 0) {
    x = Wh_GetIntValue(L"custom_x", workArea.right - w - 24);
    y = Wh_GetIntValue(L"custom_y", workArea.top + 24);
  } else if (_wcsicmp(g_Settings.position, L"top-left") == 0) {
    x = workArea.left + 24;
    y = workArea.top + 24;
  } else if (_wcsicmp(g_Settings.position, L"bottom-right") == 0) {
    x = workArea.right - w - 24;
    y = workArea.bottom - h - 24;
  } else if (_wcsicmp(g_Settings.position, L"bottom-left") == 0) {
    x = workArea.left + 24;
    y = workArea.bottom - h - 24;
  } else {
    // top-right (default preset)
    x = workArea.right - w - 24;
    y = workArea.top + 24;
  }

  SetWindowPos(g_hHudWnd, HWND_TOPMOST, x, y, w, h,
               SWP_NOACTIVATE | SWP_SHOWWINDOW);

  // Apply click-through styles dynamically
  LONG_PTR exStyle = GetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE);
  if (g_Settings.clickThrough) {
    exStyle |= WS_EX_TRANSPARENT;
  } else {
    exStyle &= ~WS_EX_TRANSPARENT;
  }
  SetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE, exStyle);
}

// DirectComposition + DXGI SwapChain Hardware Accelerated Initialization
static HRESULT InitDirectComposition(HWND hWnd) {
  CleanupDirectComposition();

  if (!hWnd)
    return E_POINTER;

  // 1. Create Direct3D 11 Device
  UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
  D3D_FEATURE_LEVEL featureLevel;
  HRESULT hr = D3D11CreateDevice(
      NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
      NULL, 0, D3D11_SDK_VERSION, &g_pD3DDevice, &featureLevel, &g_pD3DContext);
  if (FAILED(hr) || !g_pD3DDevice)
    return hr;

  // 2. Query DXGI Device
  IDXGIDevice *pDxgiDevice = nullptr;
  hr = g_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDxgiDevice);
  if (FAILED(hr) || !pDxgiDevice)
    return hr;

  // 3. Create Direct2D Factory 1 & Direct2D Device
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1),
                          nullptr, (void **)&g_pD2DFactory1);
  if (FAILED(hr) || !g_pD2DFactory1) {
    pDxgiDevice->Release();
    return hr;
  }

  hr = g_pD2DFactory1->CreateDevice(pDxgiDevice, &g_pD2DDevice);
  if (FAILED(hr) || !g_pD2DDevice) {
    pDxgiDevice->Release();
    return hr;
  }

  hr = g_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_pD2DContext);
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
    hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&pDxgiFactory2);
    if (SUCCEEDED(hr) && pDxgiFactory2) {
      float scale = g_Settings.hudScale / 100.0f;
      if (scale < 0.25f) scale = 0.25f;
      int scaledWidth = (int)roundf(500 * scale);
      int scaledHeight = (int)roundf(104 * scale);

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

      hr = pDxgiFactory2->CreateSwapChainForComposition(g_pD3DDevice, &desc, NULL, &g_pSwapChain);
      pDxgiFactory2->Release();
    }
    pDxgiAdapter->Release();
  }

  if (!g_pSwapChain) {
    pDxgiDevice->Release();
    return E_FAIL;
  }

  // 5. Create DirectComposition Device, Target & Visual
  hr = DCompositionCreateDevice(pDxgiDevice, __uuidof(IDCompositionDevice), (void **)&g_pDCompDevice);
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
  hr = g_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&pDxgiBackBuffer);
  if (SUCCEEDED(hr) && pDxgiBackBuffer) {
    float scale = g_Settings.hudScale / 100.0f;
    if (scale < 0.25f) scale = 0.25f;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f * scale, 96.0f * scale);

    hr = g_pD2DContext->CreateBitmapFromDxgiSurface(pDxgiBackBuffer, &bitmapProperties, &g_pD2DTargetBitmap);
    if (SUCCEEDED(hr) && g_pD2DTargetBitmap) {
      g_pD2DContext->SetTarget(g_pD2DTargetBitmap);
      g_pD2DContext->SetDpi(96.0f * scale, 96.0f * scale);
    }
    pDxgiBackBuffer->Release();
  }

  // 7. DirectWrite Fonts Initialization
  hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                           (IUnknown **)&g_pDWriteFactory);
  if (SUCCEEDED(hr) && g_pDWriteFactory) {
    hr = g_pDWriteFactory->CreateTextFormat(
        L"Segoe Fluent Icons", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 17.0f, L"en-us", &g_pIconFontFormat);
    if (FAILED(hr) || !g_pIconFontFormat) {
      g_pDWriteFactory->CreateTextFormat(
          L"Segoe MDL2 Assets", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
          DWRITE_FONT_STRETCH_NORMAL, 17.0f, L"en-us", &g_pIconFontFormat);
    }
    if (g_pIconFontFormat) {
      g_pIconFontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
      g_pIconFontFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-us", &g_pLabelFontFormat);
    if (g_pLabelFontFormat) {
      g_pLabelFontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      g_pLabelFontFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"en-us", &g_pValueFontFormat);
    if (g_pValueFontFormat) {
      g_pValueFontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
      g_pValueFontFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
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
  if (g_pValueFontFormat) {
    g_pValueFontFormat->Release();
    g_pValueFontFormat = nullptr;
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
  g_pD2DContext->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)); // Transparent premultiplied canvas

  D2D1_SIZE_F size = g_pD2DContext->GetSize();
  float cornerRadius = 16.0f; // Flush with Windows 11 DWM native corner rounding (DWMWCP_ROUND = 16px)

  D2D1_ROUNDED_RECT cardRect = D2D1::RoundedRect(
      D2D1::RectF(0.5f, 0.5f, size.width - 0.5f, size.height - 0.5f),
      cornerRadius, cornerRadius);

  float masterOp = g_Settings.opacity / 100.0f;

  // 1. Fluent Dark Card Background (alpha maps directly to masterOp for true 100% opacity)
  ID2D1GradientStopCollection *pBaseStops = nullptr;
  D2D1_GRADIENT_STOP baseStops[2];
  baseStops[0].color = D2D1::ColorF(0.10f, 0.12f, 0.16f, masterOp); // Dark Fluent slate top
  baseStops[0].position = 0.0f;
  baseStops[1].color = D2D1::ColorF(0.06f, 0.07f, 0.10f, masterOp); // Darker Fluent slate bottom
  baseStops[1].position = 1.0f;
  g_pD2DContext->CreateGradientStopCollection(
      baseStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pBaseStops);

  ID2D1LinearGradientBrush *pBaseBgBrush = nullptr;
  if (pBaseStops) {
    g_pD2DContext->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(0.0f, 0.0f),
            D2D1::Point2F(size.width, size.height)),
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
  sheenStops[0].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f * masterOp); // Softened to avoid white band artifact
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
            D2D1::Point2F(0.0f, size.height * 0.45f)),
        pSheenStops, &pSheenBrush);
    pSheenStops->Release();
  }
  if (pSheenBrush) {
    D2D1_ROUNDED_RECT sheenRect = D2D1::RoundedRect(
        D2D1::RectF(1.0f, 1.0f, size.width - 1.0f, size.height * 0.45f),
        16.0f, 16.0f);
    g_pD2DContext->FillRoundedRectangle(sheenRect, pSheenBrush);
    pSheenBrush->Release();
  }

  // 3. Specular Rim Lighting (N dot L Glass Edge Simulation)
  ID2D1GradientStopCollection *pRimStops = nullptr;
  D2D1_GRADIENT_STOP rimStops[3];
  rimStops[0].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f * masterOp); // Soft top-left specular highlight
  rimStops[0].position = 0.0f;
  rimStops[1].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f * masterOp); // Translucent edge
  rimStops[1].position = 0.45f;
  rimStops[2].color = D2D1::ColorF(0.02f, 0.05f, 0.10f, 0.18f * masterOp); // Subtle bottom-right shadow
  rimStops[2].position = 1.0f;
  g_pD2DContext->CreateGradientStopCollection(
      rimStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pRimStops);

  ID2D1LinearGradientBrush *pRimBrush = nullptr;
  if (pRimStops) {
    g_pD2DContext->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(0.0f, 0.0f),
            D2D1::Point2F(size.width, size.height)),
        pRimStops, &pRimBrush);
    pRimStops->Release();
  }
  if (pRimBrush) {
    g_pD2DContext->DrawRoundedRectangle(cardRect, pRimBrush, 1.2f);
    pRimBrush->Release();
  }

  // 4. (Top Rim Highlight Line removed — caused harsh white bar artifact at top edge)

  int activeRows =
      (g_Settings.showMic ? 1 : 0) + (g_Settings.showSystem ? 1 : 0);
  if (activeRows == 0)
    activeRows = 1;

  // 5. Horizontal Divider Line with dual pass (shadow + highlight)
  if (g_Settings.showMic && g_Settings.showSystem) {
    ID2D1SolidColorBrush *pDividerShadowBrush = nullptr;
    ID2D1SolidColorBrush *pDividerLightBrush = nullptr;
    g_pD2DContext->CreateSolidColorBrush(
        D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.40f), &pDividerShadowBrush);
    g_pD2DContext->CreateSolidColorBrush(
        D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f), &pDividerLightBrush);

    float midY = size.height / 2.0f;
    if (pDividerShadowBrush) {
      g_pD2DContext->DrawLine(
          D2D1::Point2F(16.0f, midY + 0.5f),
          D2D1::Point2F(size.width - 40.0f, midY + 0.5f),
          pDividerShadowBrush, 1.0f);
      pDividerShadowBrush->Release();
    }
    if (pDividerLightBrush) {
      g_pD2DContext->DrawLine(
          D2D1::Point2F(16.0f, midY - 0.5f),
          D2D1::Point2F(size.width - 40.0f, midY - 0.5f),
          pDividerLightBrush, 1.0f);
      pDividerLightBrush->Release();
    }
  }



  // Color Palette Definitions
  D2D1_COLOR_F greenColor = D2D1::ColorF(0.13f, 0.77f, 0.37f, 1.0f); // Fluent Green #22C55E
  D2D1_COLOR_F yellowColor = D2D1::ColorF(0.95f, 0.72f, 0.05f, 1.0f); // Fluent Yellow #F5B80D

  if (_wcsicmp(g_Settings.colorTheme, L"neon") == 0) {
    greenColor = D2D1::ColorF(0.00f, 0.90f, 1.00f);
    yellowColor = D2D1::ColorF(1.00f, 0.00f, 0.60f);
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

  g_pD2DContext->CreateSolidColorBrush(
      D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pTextBrush);
  g_pD2DContext->CreateSolidColorBrush(
      D2D1::ColorF(1.0f, 0.88f, 0.25f, 0.95f), &pPeakHoldBrush);
  g_pD2DContext->CreateSolidColorBrush(greenColor, &pGreenBrush);
  g_pD2DContext->CreateSolidColorBrush(yellowColor, &pYellowBrush);
  g_pD2DContext->CreateSolidColorBrush(
      D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f), &pUnlitBrush);
  g_pD2DContext->CreateSolidColorBrush(
      D2D1::ColorF(0.95f, 0.20f, 0.20f, 1.0f), &pClipBrush);
  g_pD2DContext->CreateSolidColorBrush(
      D2D1::ColorF(0.40f, 0.40f, 0.45f, 0.4f), &pMuteBrush);

  // Gradient stops for 3D liquid pill top gloss
  ID2D1GradientStopCollection *pPillGlossStops = nullptr;
  D2D1_GRADIENT_STOP pillGlossStops[2];
  pillGlossStops[0].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.45f);
  pillGlossStops[0].position = 0.0f;
  pillGlossStops[1].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f);
  pillGlossStops[1].position = 1.0f;
  g_pD2DContext->CreateGradientStopCollection(
      pillGlossStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pPillGlossStops);

  ID2D1SolidColorBrush *pPillCapBrush = nullptr;
  g_pD2DContext->CreateSolidColorBrush(
      D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.60f), &pPillCapBrush);

  float rowHeight = size.height / (float)activeRows;
  float currentY = 0.0f;

  const int TOTAL_SEGMENTS = 25;
  const float BAR_LEFT = 100.0f;
  const float BAR_RIGHT = std::max(BAR_LEFT + 120.0f, size.width - 85.0f);
  const float BAR_TOTAL_WIDTH = BAR_RIGHT - BAR_LEFT;
  const float SEG_GAP = 2.5f;
  const float SEG_WIDTH = (BAR_TOTAL_WIDTH - (TOTAL_SEGMENTS - 1) * SEG_GAP) / (float)TOTAL_SEGMENTS;
  const float SEG_CORNER = 2.5f;

  auto DrawRow = [&](AudioEndpointTracker &tracker, PCWSTR iconChar, PCWSTR labelText) {
    float rowCenterY = currentY + rowHeight / 2.0f;

    // A. Left-aligned Icon
    if (g_pIconFontFormat && pTextBrush) {
      D2D1_RECT_F iconRect = D2D1::RectF(16.0f, currentY, 40.0f, currentY + rowHeight);
      g_pD2DContext->DrawTextW(iconChar, 1, g_pIconFontFormat, iconRect, pTextBrush);
    }

    // B. Left-aligned Label ("Mic" / "System")
    if (g_pLabelFontFormat && pTextBrush) {
      D2D1_RECT_F labelRect = D2D1::RectF(42.0f, currentY, 96.0f, currentY + rowHeight);
      g_pD2DContext->DrawTextW(labelText, (UINT32)wcslen(labelText), g_pLabelFontFormat, labelRect, pTextBrush);
    }

    // C. Segmented Audio Level Meter (25 pills)
    float segHeight = 18.0f;
    float segTop = rowCenterY - segHeight / 2.0f;
    float segBottom = rowCenterY + segHeight / 2.0f;

    int litCount = 0;
    if (!tracker.isMuted && tracker.currentLevel > 0.001f) {
      litCount = (int)roundf(tracker.currentLevel * TOTAL_SEGMENTS);
      if (litCount < 1 && tracker.currentLevel > 0.005f) litCount = 1;
      if (litCount > TOTAL_SEGMENTS) litCount = TOTAL_SEGMENTS;
    }

    int peakIndex = -1;
    if (g_Settings.enablePeakHold && !tracker.isMuted && tracker.peakHold > 0.005f) {
      peakIndex = (int)roundf(tracker.peakHold * TOTAL_SEGMENTS) - 1;
      if (peakIndex < 0 && tracker.peakHold > 0.005f) peakIndex = 0;
      if (peakIndex >= TOTAL_SEGMENTS) peakIndex = TOTAL_SEGMENTS - 1;
    }

    for (int i = 0; i < TOTAL_SEGMENTS; i++) {
      float segX = BAR_LEFT + i * (SEG_WIDTH + SEG_GAP);
      D2D1_ROUNDED_RECT segRect = D2D1::RoundedRect(
          D2D1::RectF(segX, segTop, segX + SEG_WIDTH, segBottom),
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

        // 3D Liquid Glass Pill Gloss Overlay on active segments
        if (pBrush != pUnlitBrush && pBrush != pMuteBrush && pPillGlossStops) {
          ID2D1LinearGradientBrush *pPillGlossBrush = nullptr;
          g_pD2DContext->CreateLinearGradientBrush(
              D2D1::LinearGradientBrushProperties(
                  D2D1::Point2F(0.0f, segTop),
                  D2D1::Point2F(0.0f, segTop + 9.0f)),
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

    // D. Right-aligned Live dB numerical value & CLIP warning color
    if (g_pValueFontFormat && pTextBrush) {
      WCHAR dbText[32];
      ID2D1SolidColorBrush *pCurTextBrush = pTextBrush;

      if (tracker.isMuted) {
        StringCchCopyW(dbText, 32, L"Muted");
      } else {
        if (tracker.isClipping) {
          pCurTextBrush = pClipBrush; // Highlight text in red during clipping latch
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

      D2D1_RECT_F valueRect = D2D1::RectF(BAR_RIGHT + 4.0f, currentY, size.width - 30.0f, currentY + rowHeight);
      g_pD2DContext->DrawTextW(dbText, (UINT32)wcslen(dbText), g_pValueFontFormat, valueRect, pCurTextBrush);
    }

    currentY += rowHeight;
  };

  // Render Microphone Row
  if (g_Settings.showMic) {
    WCHAR micIconStr[2] = { L'\uE720', L'\0' };
    DrawRow(g_MicTracker, micIconStr, L"Mic");
  }

  // Render System Audio Row
  if (g_Settings.showSystem) {
    WCHAR sysIconStr[2] = { L'\uE767', L'\0' };
    DrawRow(g_SystemTracker, sysIconStr, L"System");
  }

  // Draw Settings overflow button (⋮) if clickable
  if (!g_Settings.clickThrough && g_pIconFontFormat && pTextBrush) {
    WCHAR moreIconStr[2] = { L'\uE712', L'\0' };
    
    // Rotate the text 90 degrees around its center to make the horizontal "..." vertical
    D2D1_POINT_2F center = D2D1::Point2F(size.width - 16.0f, size.height / 2.0f);
    g_pD2DContext->SetTransform(D2D1::Matrix3x2F::Rotation(90.0f, center));

    // Center it vertically on the right edge
    D2D1_RECT_F moreRect = D2D1::RectF(size.width - 32.0f, 0.0f, size.width, size.height);
    g_pD2DContext->DrawTextW(moreIconStr, 1, g_pIconFontFormat, moreRect, pTextBrush);

    // Reset transform
    g_pD2DContext->SetTransform(D2D1::Matrix3x2F::Identity());
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

// ─── Settings Flyout ────────────────────────────────────────────────────────

static void DismissFlyout() {
  if (g_hFlyoutWnd) {
    DestroyWindow(g_hFlyoutWnd);
    g_hFlyoutWnd = NULL;
  }
}

static LRESULT CALLBACK FlyoutWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    // Dark glass background fill
    if (!g_hFlyoutBgBrush) g_hFlyoutBgBrush = CreateSolidBrush(RGB(20, 20, 30));
    FillRect(hdc, &rc, g_hFlyoutBgBrush);
    // Section header text: "Opacity"
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(180, 180, 195));
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    RECT lblRc = {14, 14, 120, 34};
    DrawTextW(hdc, L"Opacity", -1, &lblRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT scaleRc = {14, 60, 120, 80};
    DrawTextW(hdc, L"HUD Scale", -1, &scaleRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT secRc = {14, 110, 200, 128};
    DrawTextW(hdc, L"Display", -1, &secRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT sec2Rc = {14, 222, 200, 240};
    DrawTextW(hdc, L"Input Device", -1, &sec2Rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT sec3Rc = {14, 284, 200, 302};
    DrawTextW(hdc, L"Behavior", -1, &sec3Rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    // Subtle section separator lines
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 65));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 14, 54, NULL); LineTo(hdc, rc.right - 14, 54);
    MoveToEx(hdc, 14, 104, NULL); LineTo(hdc, rc.right - 14, 104);
    MoveToEx(hdc, 14, 216, NULL); LineTo(hdc, rc.right - 14, 216);
    MoveToEx(hdc, 14, 278, NULL); LineTo(hdc, rc.right - 14, 278);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    SelectObject(hdc, hOld);
    EndPaint(hWnd, &ps);
    return 0;
  }

  // Make all child controls transparent over the dark background
  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLORBTN: {
    HDC hdcCtrl = (HDC)wParam;
    SetBkMode(hdcCtrl, TRANSPARENT);
    SetTextColor(hdcCtrl, RGB(210, 210, 225));
    if (!g_hFlyoutBgBrush) g_hFlyoutBgBrush = CreateSolidBrush(RGB(20, 20, 30));
    return (LRESULT)g_hFlyoutBgBrush;
  }

  case WM_HSCROLL: {
    // Opacity trackbar
    HWND hSlider = GetDlgItem(hWnd, FLYOUT_CTRL_OPACITY);
    if ((HWND)lParam == hSlider) {
      int val = (int)SendMessageW(hSlider, TBM_GETPOS, 0, 0);
      g_Settings.opacity = val;
      // Update live percentage label
      HWND hLbl = GetDlgItem(hWnd, FLYOUT_CTRL_OP_LBL);
      if (hLbl) {
        WCHAR buf[16];
        swprintf_s(buf, L"%d%%", val);
        SetWindowTextW(hLbl, buf);
      }
      if (g_hHudWnd) InvalidateRect(g_hHudWnd, NULL, FALSE);
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
    int id   = LOWORD(wParam);
    int notif = HIWORD(wParam);
    if (notif == BN_CLICKED) {
      HWND hBtn = (HWND)lParam;
      BOOL checked = (SendMessageW(hBtn, BM_GETCHECK, 0, 0) == BST_CHECKED);
      if (id == FLYOUT_CTRL_BOTH) {
        g_Settings.showMic = TRUE;
        g_Settings.showSystem = TRUE;
      } else if (id == FLYOUT_CTRL_MIC_ONLY) {
        g_Settings.showMic = TRUE;
        g_Settings.showSystem = FALSE;
      } else if (id == FLYOUT_CTRL_SYS_ONLY) {
        g_Settings.showMic = FALSE;
        g_Settings.showSystem = TRUE;
      } else if (id == FLYOUT_CTRL_CLICKTHRU) {
        g_Settings.clickThrough = checked;
        if (g_hHudWnd) {
          LONG_PTR ex = GetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE);
          if (checked) ex |=  WS_EX_TRANSPARENT;
          else         ex &= ~WS_EX_TRANSPARENT;
          SetWindowLongPtrW(g_hHudWnd, GWL_EXSTYLE, ex);
        }
      } else if (id == FLYOUT_CTRL_PEAKHOLD) {
        g_Settings.enablePeakHold = checked;
        Wh_SetIntValue(L"rt_enablePeakHold", checked);
      }
      if (g_hHudWnd) InvalidateRect(g_hHudWnd, NULL, FALSE);
    } else if (notif == CBN_SELCHANGE && id == FLYOUT_CTRL_MIC_COMBO) {
      HWND hCombo = (HWND)lParam;
      int index = (int)SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
      if (index != CB_ERR) {
        WCHAR selText[64] = {0};
        SendMessageW(hCombo, CB_GETLBTEXT, index, (LPARAM)selText);
        StringCchCopyW(g_Settings.micDevice, 64, selText);
        Wh_SetStringValue(L"rt_micDevice", selText);
        
        // Re-initialize audio tracker for mic with new device
        InitAudioTracker(eCapture, g_MicTracker);
        if (g_hHudWnd) InvalidateRect(g_hHudWnd, NULL, FALSE);
      }
    }
    return 0;
  }

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) DismissFlyout();
    return 0;

  case WM_ACTIVATE:
    // Dismiss when flyout loses activation (user clicked elsewhere)
    if (LOWORD(wParam) == WA_INACTIVE) DismissFlyout();
    return 0;

  case WM_DESTROY:
    g_hFlyoutWnd = NULL;
    return 0;

  default:
    return DefWindowProcW(hWnd, message, wParam, lParam);
  }
}

static void ShowSettingsFlyout() {
  // Toggle: dismiss if already open
  if (g_hFlyoutWnd) {
    DismissFlyout();
    return;
  }

  if (!g_hHudWnd) return;

  // Position flyout: bottom-left of the ⋮ button, aligned to HUD right edge
  RECT hudRect;
  GetWindowRect(g_hHudWnd, &hudRect);

  const int FLY_W = 250;
  const int FLY_H = 374;

  int x = hudRect.right  - FLY_W - 4;
  int y = hudRect.bottom + 6;

  // Flip above HUD if flyout would go off-screen bottom
  int screenH = GetSystemMetrics(SM_CYSCREEN);
  if (y + FLY_H > screenH) y = hudRect.top - FLY_H - 6;
  // Clamp to screen left
  if (x < 4) x = 4;

  g_hFlyoutWnd = CreateWindowExW(
      WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
      FLYOUT_WINDOW_CLASS, L"",
      WS_POPUP | WS_CLIPCHILDREN,
      x, y, FLY_W, FLY_H,
      NULL, NULL, GetModuleHandle(NULL), NULL);

  if (!g_hFlyoutWnd) return;

  // DWM dark title bar + rounded corners (DWMWCP_ROUND is OK here - no D2D/DComp conflict)
  BOOL darkMode = TRUE;
  DwmSetWindowAttribute(g_hFlyoutWnd, 20, &darkMode, sizeof(darkMode));
  DWORD cornerPref = 2; // DWMWCP_ROUND
  DwmSetWindowAttribute(g_hFlyoutWnd, 33, &cornerPref, sizeof(cornerPref));

  HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
  if (hUser32) {
    pfnSetWindowCompositionAttribute pSWCA = (pfnSetWindowCompositionAttribute)
        GetProcAddress(hUser32, "SetWindowCompositionAttribute");
    if (pSWCA) {
      ACCENT_POLICY accent = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, 0x20141420, 0};
      WINDOWCOMPOSITIONATTRIBDATA data = {19, &accent, sizeof(accent)};
      pSWCA(g_hFlyoutWnd, &data);
    }
  }

  HMODULE hInst = GetModuleHandle(NULL);
  const int PAD_X = 14;
  const int SLIDER_Y = 12;
  const int ROW_H = 26;

  // ── Opacity Row ──────────────────────────────────────────────
  HWND hSlider = CreateWindowExW(0, TRACKBAR_CLASS, L"",
      WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
      PAD_X + 80, SLIDER_Y, 102, 26,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_OPACITY, hInst, NULL);
  SendMessageW(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
  SendMessageW(hSlider, TBM_SETPOS,   TRUE, g_Settings.opacity);
  SetWindowTheme(hSlider, L"DarkMode_Explorer", NULL);

  // Percentage label next to slider
  WCHAR opBuf[16];
  swprintf_s(opBuf, L"%d%%", g_Settings.opacity);
  HWND hOpLbl = CreateWindowExW(0, L"STATIC", opBuf,
      WS_CHILD | WS_VISIBLE | SS_CENTER,
      PAD_X + 80 + 106, SLIDER_Y + 4, 36, 18,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_OP_LBL, hInst, NULL);
  SetWindowTheme(hOpLbl, L"DarkMode_Explorer", NULL);

  // ── HUD Scale Row ────────────────────────────────────────────
  HWND hScaleSlider = CreateWindowExW(0, TRACKBAR_CLASS, L"",
      WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
      PAD_X + 80, 58, 102, 26,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_SCALE, hInst, NULL);
  SendMessageW(hScaleSlider, TBM_SETRANGE, TRUE, MAKELPARAM(25, 100));
  SendMessageW(hScaleSlider, TBM_SETPOS,   TRUE, g_Settings.hudScale);
  SetWindowTheme(hScaleSlider, L"DarkMode_Explorer", NULL);

  WCHAR scaleBuf[16];
  swprintf_s(scaleBuf, L"%d%%", g_Settings.hudScale);
  HWND hScaleLbl = CreateWindowExW(0, L"STATIC", scaleBuf,
      WS_CHILD | WS_VISIBLE | SS_CENTER,
      PAD_X + 80 + 106, 58 + 4, 36, 18,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_SCALE_LBL, hInst, NULL);
  SetWindowTheme(hScaleLbl, L"DarkMode_Explorer", NULL);

  // ── Display Row ──────────────────────────────────────────────
  HWND hRadBoth = CreateWindowExW(0, WC_BUTTON, L"  Show Both Meters",
      WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
      PAD_X + 2, 130, FLY_W - PAD_X * 2, ROW_H,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_BOTH, hInst, NULL);
  
  HWND hRadMic = CreateWindowExW(0, WC_BUTTON, L"  Show Microphone Only",
      WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
      PAD_X + 2, 156, FLY_W - PAD_X * 2, ROW_H,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_MIC_ONLY, hInst, NULL);
      
  HWND hRadSys = CreateWindowExW(0, WC_BUTTON, L"  Show System Output Only",
      WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
      PAD_X + 2, 182, FLY_W - PAD_X * 2, ROW_H,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_SYS_ONLY, hInst, NULL);

  if (g_Settings.showMic && g_Settings.showSystem) {
    SendMessageW(hRadBoth, BM_SETCHECK, BST_CHECKED, 0);
  } else if (g_Settings.showMic) {
    SendMessageW(hRadMic, BM_SETCHECK, BST_CHECKED, 0);
  } else if (g_Settings.showSystem) {
    SendMessageW(hRadSys, BM_SETCHECK, BST_CHECKED, 0);
  }
  
  SetWindowTheme(hRadBoth, L"DarkMode_Explorer", NULL);
  SetWindowTheme(hRadMic,  L"DarkMode_Explorer", NULL);
  SetWindowTheme(hRadSys,  L"DarkMode_Explorer", NULL);

  // ── Input Device Row ──────────────────────────────────────────
  HWND hMicCombo = CreateWindowExW(0, WC_COMBOBOX, L"",
      CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
      PAD_X, 242, FLY_W - PAD_X * 2, 200, // 200 is dropdown list height
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_MIC_COMBO, hInst, NULL);
  
  SetWindowTheme(hMicCombo, L"DarkMode_Explorer", NULL);

  // Populate Combobox
  SendMessageW(hMicCombo, CB_ADDSTRING, 0, (LPARAM)L"default");
  SendMessageW(hMicCombo, CB_ADDSTRING, 0, (LPARAM)L"communications");

  IMMDeviceEnumerator *pEnum = nullptr;
  if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                                 __uuidof(IMMDeviceEnumerator), (void **)&pEnum))) {
    IMMDeviceCollection *pCol = nullptr;
    if (SUCCEEDED(pEnum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCol))) {
      UINT count = 0;
      pCol->GetCount(&count);
      for (UINT i = 0; i < count; i++) {
        IMMDevice *pEndpoint = nullptr;
        if (SUCCEEDED(pCol->Item(i, &pEndpoint))) {
          IPropertyStore *pProps = nullptr;
          if (SUCCEEDED(pEndpoint->OpenPropertyStore(STGM_READ, &pProps))) {
            PROPVARIANT varName;
            PropVariantInit(&varName);
            if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &varName))) {
              if (varName.vt == VT_LPWSTR && varName.pwszVal) {
                SendMessageW(hMicCombo, CB_ADDSTRING, 0, (LPARAM)varName.pwszVal);
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
    pEnum->Release();
  }

  // Pre-select the matching string
  if (wcslen(g_Settings.micDevice) > 0) {
    int index = (int)SendMessageW(hMicCombo, CB_FINDSTRINGEXACT, -1, (LPARAM)g_Settings.micDevice);
    if (index != CB_ERR) {
      SendMessageW(hMicCombo, CB_SETCURSEL, index, 0);
    } else {
      // Try partial match if exact fails
      index = (int)SendMessageW(hMicCombo, CB_FINDSTRING, -1, (LPARAM)g_Settings.micDevice);
      if (index != CB_ERR) SendMessageW(hMicCombo, CB_SETCURSEL, index, 0);
    }
  }

  // ── Behavior Row ─────────────────────────────────────────────
  HWND hCtChk = CreateWindowExW(0, WC_BUTTON, L"  Click-Through",
      WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
      PAD_X + 2, 304, FLY_W - PAD_X * 2, ROW_H,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_CLICKTHRU, hInst, NULL);
  SendMessageW(hCtChk, BM_SETCHECK, g_Settings.clickThrough ? BST_CHECKED : BST_UNCHECKED, 0);
  SetWindowTheme(hCtChk, L"DarkMode_Explorer", NULL);

  HWND hPkChk = CreateWindowExW(0, WC_BUTTON, L"  Show Peak Hold Marker",
      WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
      PAD_X + 2, 330, FLY_W - PAD_X * 2, ROW_H,
      g_hFlyoutWnd, (HMENU)FLYOUT_CTRL_PEAKHOLD, hInst, NULL);
  SendMessageW(hPkChk, BM_SETCHECK, g_Settings.enablePeakHold ? BST_CHECKED : BST_UNCHECKED, 0);
  SetWindowTheme(hPkChk, L"DarkMode_Explorer", NULL);

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
    ApplyAcrylicBlur(hWnd);
    InitAudioTracker(eCapture, g_MicTracker);
    InitAudioTracker(eRender, g_SystemTracker);
    InitDirectComposition(hWnd);
    InvalidateRect(hWnd, NULL, FALSE);
    return 0;

  case WM_LBUTTONDOWN: {
    if (!g_Settings.clickThrough) {
      int x = GET_X_LPARAM(lParam);
      RECT rc;
      GetClientRect(hWnd, &rc);
      float scale = g_Settings.hudScale / 100.0f;
      if (scale < 0.25f) scale = 0.25f;
      int hitArea = (int)roundf(35 * scale);
      if (x >= rc.right - hitArea) {
        ShowSettingsFlyout();
      }
    }
    return 0;
  }

  case WM_SIZE:
    if (g_pD2DContext && g_pSwapChain) {
      // Re-create swap chain buffers on size change if needed
    }
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
      ShowWindow(hWnd, g_bHudVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
    }
    return 0;

  case WM_NCHITTEST: {
    LRESULT hit = DefWindowProcW(hWnd, message, wParam, lParam);
    if (hit == HTCLIENT && !g_Settings.clickThrough) {
      POINT pt;
      pt.x = GET_X_LPARAM(lParam);
      pt.y = GET_Y_LPARAM(lParam);
      ScreenToClient(hWnd, &pt);
      RECT rc;
      GetClientRect(hWnd, &rc);
      float scale = g_Settings.hudScale / 100.0f;
      if (scale < 0.25f) scale = 0.25f;
      int hitArea = (int)roundf(35 * scale);
      if (pt.x >= rc.right - hitArea) {
        return HTCLIENT; // Allow LBUTTONDOWN on the dots
      }
      return HTCAPTION; // Allow dragging everywhere else
    }
    return hit;
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
  // Wait for the Explorer shell to fully load (useful during cold boots)
  // before we try to initialize COM, Audio Endpoints, and DirectComposition.
  int waitCount = 0;
  while (!FindWindowW(L"Shell_TrayWnd", NULL) && waitCount < 60) {
    Sleep(500);
    waitCount++;
  }
  Sleep(1000); // Extra buffer for audio services to spin up

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

  // Create Topmost Window for DirectComposition (WS_EX_NOREDIRECTIONBITMAP)
  float scale = g_Settings.hudScale / 100.0f;
  if (scale < 0.25f) scale = 0.25f;

  g_hHudWnd = CreateWindowExW(
      WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP |
          (g_Settings.clickThrough ? WS_EX_TRANSPARENT : 0),
      HUD_WINDOW_CLASS, L"Audio Level HUD", WS_POPUP, 0, 0, 
      (int)roundf(500 * scale),
      (int)roundf(104 * scale), 
      NULL, NULL, GetModuleHandle(NULL), NULL);

  if (!g_hHudWnd) {
    CoUninitialize();
    return 0;
  }

  PositionHudWindow();
  RegisterGlobalHotkey();
  ApplyAcrylicBlur(g_hHudWnd);

  // Initial audio setup
  InitAudioTracker(eCapture, g_MicTracker);
  InitAudioTracker(eRender, g_SystemTracker);

  ShowWindow(g_hHudWnd, SW_SHOWNOACTIVATE);
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
BOOL Wh_ModInit() {
  Wh_Log(L"Initializing Audio Level HUD mod...");

  LoadModSettings();

  // Spawn dedicated UI message loop thread
  g_hHudThread = CreateThread(NULL, 0, HudThreadProc, NULL, 0, &g_dwThreadId);
  if (!g_hHudThread) {
    Wh_Log(L"Failed to create Audio Level HUD thread.");
    return FALSE;
  }

  Wh_Log(L"Audio Level HUD thread created successfully.");
  return TRUE;
}

// Windhawk Lifecycle Uninitialization Callback
void Wh_ModUninit() {
  Wh_Log(L"Uninitializing Audio Level HUD mod...");

  if (g_hHudWnd) {
    PostMessageW(g_hHudWnd, WM_CLOSE, 0, 0);
  }

  if (g_hHudThread) {
    WaitForSingleObject(g_hHudThread, 2000);
    CloseHandle(g_hHudThread);
    g_hHudThread = NULL;
  }

  Wh_Log(L"Audio Level HUD uninitialized.");
}

// Windhawk Settings Changed Callback
void Wh_ModSettingsChanged() {
  Wh_Log(L"Settings changed, posting reload message to UI thread...");
  if (g_hHudWnd) {
    PostMessageW(g_hHudWnd, WM_HUD_RELOAD_SETTINGS, 0, 0);
  }
}
