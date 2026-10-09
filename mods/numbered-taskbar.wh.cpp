// ==WindhawkMod==
// @id              numbered-taskbar
// @name            Numbered taskbar
// @description     Adds numbers (1-0) to taskbar icons corresponding to Win + Number shortcut
// @version         1.0.0
// @author          maciejlewand72
// @github          https://github.com/maciejlewand72
// @include         windhawk.exe
// @compilerOptions -luiautomationcore -lcomdlg32 -lole32 -loleaut32 -luser32 -lgdi32 -luuid -ld2d1 -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Numbered taskbar
Displays numbers 1 through 0 on the first 10 application icons in the taskbar.
These numbers correspond to the `Win + Number` keyboard shortcuts which launch
the respective applications.

![Screenshot](https://i.imgur.com/K4eY6yd.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- showOnlyOnWinKey: true
  $name: Show only when WIN key is held down
- delayMs: 0
  $name: Delay before showing numbers (ms)

- fontSize: 16
  $name: Font Size
- fontColor: 0xFFFFFFFF
  $name: Font Color (ARGB)
- outlineColor: 0xFF000000
  $name: Outline Color (ARGB)
- offsetX: 2
  $name: X Offset (from top-left of icon)
- offsetY: 2
  $name: Y Offset (from top-left of icon)
- updateIntervalMs: 50
  $name: Update Interval (ms)

*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <d2d1.h>
#include <dwrite.h>
#include <mutex>
#include <string>
#include <thread>
#include <uiautomation.h>
#include <vector>
#include <windows.h>

struct ModSettings {
  int fontSize;
  UINT32 fontColor;
  UINT32 outlineColor;
  int offsetX;
  int offsetY;
  int updateIntervalMs;
  bool showOnlyOnWinKey;
  int delayMs;
} g_settings;

std::mutex g_settingsMutex;
std::atomic<bool> g_modStopping(false);
std::thread *g_workerThread = nullptr;
HWND g_overlayHwnd = NULL;

ID2D1Factory *g_pD2DFactory = nullptr;
IDWriteFactory *g_pDWriteFactory = nullptr;

HHOOK g_keyboardHook = NULL;
HWINEVENTHOOK g_winEventHook = NULL;

#define WM_APP_WINKBD (WM_APP + 1)
#define WM_APP_TASKBAR_CHANGED (WM_APP + 2)
#define WM_APP_RECTS_FETCHED (WM_APP + 3)

#define TIMER_DELAY_ID 1
#define TIMER_UPDATE_ID 2

void LoadSettings() {
  ModSettings newSettings;
  newSettings.fontSize = Wh_GetIntSetting(L"fontSize");
  newSettings.fontColor = Wh_GetIntSetting(L"fontColor");
  newSettings.outlineColor = Wh_GetIntSetting(L"outlineColor");
  newSettings.offsetX = Wh_GetIntSetting(L"offsetX");
  newSettings.offsetY = Wh_GetIntSetting(L"offsetY");
  newSettings.showOnlyOnWinKey = Wh_GetIntSetting(L"showOnlyOnWinKey") != 0;

  newSettings.delayMs = Wh_GetIntSetting(L"delayMs");
  newSettings.delayMs = std::max(newSettings.delayMs, 0);
  newSettings.fontSize = std::max(newSettings.fontSize, 1);
  newSettings.fontSize = std::min(newSettings.fontSize, 200);

  newSettings.offsetX = std::max(newSettings.offsetX, -1000);
  newSettings.offsetX = std::min(newSettings.offsetX, 1000);
  newSettings.offsetY = std::max(newSettings.offsetY, -1000);
  newSettings.offsetY = std::min(newSettings.offsetY, 1000);

  newSettings.updateIntervalMs = Wh_GetIntSetting(L"updateIntervalMs");
  newSettings.updateIntervalMs = std::max(newSettings.updateIntervalMs, 50);
  newSettings.updateIntervalMs = std::min(newSettings.updateIntervalMs, 60000);

  std::scoped_lock lock(g_settingsMutex);
  g_settings = newSettings;
}

void WhTool_ModSettingsChanged() {
  LoadSettings();
  if (g_overlayHwnd) PostMessage(g_overlayHwnd, WM_APP_TASKBAR_CHANGED, 0, 0);
}

std::vector<RECT> GetTaskbarButtonRects(IUIAutomation *pUIAutomation) {
  std::vector<RECT> rects;
  if (!pUIAutomation) return rects;

  HWND hwndTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
  if (hwndTaskbar) {
    IUIAutomationElement *pTaskbarElement = NULL;
    if (SUCCEEDED(pUIAutomation->ElementFromHandle(hwndTaskbar, &pTaskbarElement)) && pTaskbarElement) {
      IUIAutomationCondition *pButtonCondition = NULL;
      VARIANT varType;
      VariantInit(&varType);
      varType.vt = VT_I4;
      varType.lVal = UIA_ButtonControlTypeId;
      pUIAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varType, &pButtonCondition);

      if (pButtonCondition) {
        IUIAutomationElement *pSearchRoot = pTaskbarElement;

        IUIAutomationCondition *pClassCondition1 = NULL;
        VARIANT varClass1;
        VariantInit(&varClass1);
        varClass1.vt = VT_BSTR;
        varClass1.bstrVal = SysAllocString(L"Taskbar.TaskList");
        pUIAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId, varClass1, &pClassCondition1);

        IUIAutomationCondition *pClassCondition2 = NULL;
        VARIANT varClass2;
        VariantInit(&varClass2);
        varClass2.vt = VT_BSTR;
        varClass2.bstrVal = SysAllocString(L"MSTaskListWClass");
        pUIAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId, varClass2, &pClassCondition2);

        IUIAutomationCondition *pOrCondition = NULL;
        pUIAutomation->CreateOrCondition(pClassCondition1, pClassCondition2, &pOrCondition);

        IUIAutomationElement *pTaskListElement = NULL;
        if (SUCCEEDED(pTaskbarElement->FindFirst(TreeScope_Subtree, pOrCondition, &pTaskListElement)) && pTaskListElement)
          pSearchRoot = pTaskListElement;

        SysFreeString(varClass1.bstrVal);
        SysFreeString(varClass2.bstrVal);
        if (pClassCondition1) pClassCondition1->Release();
        if (pClassCondition2) pClassCondition2->Release();
        if (pOrCondition) pOrCondition->Release();

        IUIAutomationElementArray *pElementArray = NULL;
        if (SUCCEEDED(pSearchRoot->FindAll(TreeScope_Subtree, pButtonCondition, &pElementArray)) && pElementArray) {
          int count = 0;
          pElementArray->get_Length(&count);

          for (int i = 0; i < count; i++) {
            IUIAutomationElement *pElement = NULL;
            if (SUCCEEDED(pElementArray->GetElement(i, &pElement)) && pElement) {
              BSTR autoId = NULL;
              pElement->get_CurrentAutomationId(&autoId);
              bool isAppButton = true;
              if (autoId) {
                std::wstring sid(autoId);
                if (sid == L"StartButton" || sid == L"SearchButton" || sid == L"TaskViewButton" ||
                    sid == L"WidgetsButton" || sid == L"ChatButton" || sid == L"ShowDesktopButton" ||
                    sid == L"SystemTray" || sid == L"NotificationCenterButton" || sid == L"ControlCenterButton" ||
                    sid == L"ChevronButton" || sid == L"FocusAssistButton" || sid == L"CopilotButton" ||
                    sid == L"PenWorkspaceButton" || sid == L"TouchKeyboardModeButton" || sid == L"SystemTrayIcon")
                  isAppButton = false;
                SysFreeString(autoId);
              }

              BSTR className = NULL;
              pElement->get_CurrentClassName(&className);
              if (className) {
                std::wstring cls(className);
                if (cls == L"Taskbar.SystemTrayIcon" || cls == L"Taskbar.StartButton" || cls == L"SystemTray.NormalButton" ||
                    cls == L"SystemTray.AccentButton" || cls == L"SystemTray.OmniButtonCenter")
                  isAppButton = false;
                SysFreeString(className);
              }

              if (isAppButton) {
                RECT bounds;
                if (SUCCEEDED(pElement->get_CurrentBoundingRectangle(&bounds)))
                  if (bounds.right > bounds.left && bounds.bottom > bounds.top) rects.push_back(bounds);
              }
              pElement->Release();
            }
          }
          pElementArray->Release();
        }
        if (pTaskListElement) pTaskListElement->Release();
        pButtonCondition->Release();
      }
      pTaskbarElement->Release();
    }
  }

  bool isVertical = false;
  RECT taskbarRect = {0};
  if (hwndTaskbar)
    if (GetWindowRect(hwndTaskbar, &taskbarRect))
      if ((taskbarRect.bottom - taskbarRect.top) > (taskbarRect.right - taskbarRect.left)) isVertical = true;

  if (isVertical) {
    std::ranges::sort(rects, [&taskbarRect](const RECT &a, const RECT &b) {
      int bucketA = (a.left - taskbarRect.left) / 20;
      int bucketB = (b.left - taskbarRect.left) / 20;
      if (bucketA != bucketB) return bucketA < bucketB;
      return a.top < b.top;
    });
  } else {
    std::ranges::sort(rects, [&taskbarRect](const RECT &a, const RECT &b) {
      int bucketA = (a.top - taskbarRect.top) / 20;
      int bucketB = (b.top - taskbarRect.top) / 20;
      if (bucketA != bucketB) return bucketA < bucketB;
      return a.left < b.left;
    });
  }
  return rects;
}

void DrawNumbers(const std::vector<RECT> &rects, HWND hwndTaskbar, const ModSettings &settings, HDC hdcScreen, HDC hdcMem,
                 HBITMAP &hBitmap, HBITMAP &hOldBitmap, void *&pvBits, int &cachedWidth, int &cachedHeight,
                 ID2D1DCRenderTarget *&pDCRT) {
  if (!g_overlayHwnd) return;

  RECT taskbarRect;
  GetWindowRect(hwndTaskbar, &taskbarRect);

  int width = taskbarRect.right - taskbarRect.left;
  int height = taskbarRect.bottom - taskbarRect.top;

  if (width <= 0 || height <= 0) return;

  SetWindowPos(g_overlayHwnd, HWND_TOPMOST, taskbarRect.left, taskbarRect.top, width, height, SWP_NOACTIVATE);

  if (width != cachedWidth || height != cachedHeight) {
    if (hBitmap) {
      SelectObject(hdcMem, hOldBitmap);
      DeleteObject(hBitmap);
      hBitmap = NULL;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (!hBitmap) {
      cachedWidth = 0;
      cachedHeight = 0;
      return;
    }
    hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    cachedWidth = width;
    cachedHeight = height;

    if (pDCRT) {
      RECT bounds = {0, 0, width, height};
      pDCRT->BindDC(hdcMem, &bounds);
    }
  }

  if (pvBits && cachedWidth > 0 && cachedHeight > 0) memset(pvBits, 0, cachedWidth * cachedHeight * 4);

  if (!pDCRT && g_pD2DFactory) {
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 
        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
    g_pD2DFactory->CreateDCRenderTarget(&props, &pDCRT);
    if (pDCRT) {
      RECT bounds = {0, 0, cachedWidth, cachedHeight};
      pDCRT->BindDC(hdcMem, &bounds);
    }
  }

  if (pDCRT) {
    pDCRT->BeginDraw();
    pDCRT->Clear(D2D1::ColorF(0, 0.0f));

    IDWriteTextFormat *pTextFormat = nullptr;
    if (g_pDWriteFactory) {
      g_pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, (FLOAT)settings.fontSize, L"en-us", &pTextFormat);
    }

    if (pTextFormat) {
      pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

      ID2D1SolidColorBrush *pTextBrush = nullptr;
      ID2D1SolidColorBrush *pOutlineBrush = nullptr;

      auto MakeColorF = [](UINT32 color) {
        float a = ((color >> 24) & 0xFF) / 255.0f;
        float r = ((color >> 16) & 0xFF) / 255.0f;
        float g = ((color >> 8) & 0xFF) / 255.0f;
        float b = (color & 0xFF) / 255.0f;
        return D2D1::ColorF(r, g, b, a);
      };

      pDCRT->CreateSolidColorBrush(MakeColorF(settings.fontColor), &pTextBrush);
      pDCRT->CreateSolidColorBrush(MakeColorF(settings.outlineColor), &pOutlineBrush);

      int index = 1;
      for (const RECT &r : rects) {
        if (index > 10) break;

        std::wstring text = std::to_wstring(index == 10 ? 0 : index);
        int x = r.left - taskbarRect.left + settings.offsetX;
        int y = r.top - taskbarRect.top + settings.offsetY;

        IDWriteTextLayout *pTextLayout = nullptr;
        g_pDWriteFactory->CreateTextLayout(text.c_str(), text.length(), pTextFormat, 1000.0f, 1000.0f, &pTextLayout);
        if (pTextLayout && pTextBrush && pOutlineBrush) {
          float offsets[8][2] = {{-2, -2}, {0, -2}, {2, -2}, {-2, 0}, {2, 0},   {-2, 2}, {0, 2},  {2, 2}};
          for (int i = 0; i < 8; i++)
            pDCRT->DrawTextLayout(D2D1::Point2F(x + offsets[i][0], y + offsets[i][1]), pTextLayout, pOutlineBrush);
          pDCRT->DrawTextLayout(D2D1::Point2F(x, y), pTextLayout, pTextBrush);
          pTextLayout->Release();
        }

        index++;
      }

      if (pTextBrush) pTextBrush->Release();
      if (pOutlineBrush) pOutlineBrush->Release();
      pTextFormat->Release();
    }

    pDCRT->EndDraw();
  }

  BLENDFUNCTION blend = {0};
  blend.BlendOp = AC_SRC_OVER;
  blend.SourceConstantAlpha = 255;
  blend.AlphaFormat = AC_SRC_ALPHA;

  POINT ptPos = {taskbarRect.left, taskbarRect.top};
  SIZE size = {width, height};
  POINT ptSrc = {0, 0};

  UpdateLayeredWindow(g_overlayHwnd, hdcScreen, &ptPos, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  static bool s_isFetching = false;
  static HDC hdcScreen = NULL;
  static HDC hdcMem = NULL;
  static HBITMAP hBitmap = NULL;
  static HBITMAP hOldBitmap = NULL;
  static void *pvBits = nullptr;
  static int cachedWidth = 0;
  static int cachedHeight = 0;
  static ID2D1DCRenderTarget *pDCRT = nullptr;

  static bool winKeyDown = false;
  static bool numbersVisible = false;

  if (uMsg == WM_CREATE) {
    hdcScreen = GetDC(NULL);
    hdcMem = CreateCompatibleDC(hdcScreen);
    return 0;
  } else if (uMsg == WM_DESTROY) {
    if (pDCRT) {
      pDCRT->Release();
      pDCRT = nullptr;
    }
    if (hBitmap) {
      SelectObject(hdcMem, hOldBitmap);
      DeleteObject(hBitmap);
    }
    if (hdcMem) DeleteDC(hdcMem);
    if (hdcScreen) ReleaseDC(NULL, hdcScreen);
    return 0;
  } else if (uMsg == WM_NCHITTEST) {
    return HTTRANSPARENT;
  } else if (uMsg == WM_APP_WINKBD) {
    bool pressed = (wParam != 0);
    if (pressed && !winKeyDown) {
      winKeyDown = true;
      ModSettings s;
      {
        std::scoped_lock lock(g_settingsMutex);
        s = g_settings;
      }
      if (s.showOnlyOnWinKey) {
        if (s.delayMs > 0) SetTimer(hwnd, TIMER_DELAY_ID, s.delayMs, NULL);
        else SendMessage(hwnd, WM_APP_TASKBAR_CHANGED, 0, 0);
      }
    } else if (!pressed && winKeyDown) {
      winKeyDown = false;
      ModSettings s;
      {
        std::scoped_lock lock(g_settingsMutex);
        s = g_settings;
      }
      if (s.showOnlyOnWinKey) {
        KillTimer(hwnd, TIMER_DELAY_ID);
        ShowWindow(hwnd, SW_HIDE);
        numbersVisible = false;
      }
    }
    return 0;
  } else if (uMsg == WM_APP_TASKBAR_CHANGED) {
    ModSettings s;
    {
      std::scoped_lock lock(g_settingsMutex);
      s = g_settings;
    }
    SetTimer(hwnd, TIMER_UPDATE_ID, s.updateIntervalMs, NULL);
    return 0;
    } else if (uMsg == WM_TIMER) {
    if (wParam == TIMER_DELAY_ID) {
      KillTimer(hwnd, TIMER_DELAY_ID);
      SendMessage(hwnd, WM_APP_TASKBAR_CHANGED, 0, 0);
    } else if (wParam == TIMER_UPDATE_ID) {
      if (s_isFetching) return 0;
      
      s_isFetching = true;
      KillTimer(hwnd, TIMER_UPDATE_ID);

      ModSettings s;
      {
        std::scoped_lock lock(g_settingsMutex);
        s = g_settings;
      }

      if (s.showOnlyOnWinKey && !winKeyDown) {
        ShowWindow(hwnd, SW_HIDE);
        numbersVisible = false;
        s_isFetching = false;
      } else {
        std::thread([hwnd]() {
          CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
          IUIAutomation *pUIA = NULL;
          std::vector<RECT> *pRects = new std::vector<RECT>();
          
          if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                                         __uuidof(IUIAutomation), (void **)&pUIA))) {
            *pRects = GetTaskbarButtonRects(pUIA);
            pUIA->Release();
          }
          CoUninitialize();
          
          if (!PostMessage(hwnd, WM_APP_RECTS_FETCHED, (WPARAM)pRects, 0)) delete pRects;
        }).detach();
      }
    }
    return 0;
  } else if (uMsg == WM_APP_RECTS_FETCHED) {
    std::vector<RECT> *pRects = (std::vector<RECT> *)wParam;
    s_isFetching = false;

    ModSettings s;
    {
      std::scoped_lock lock(g_settingsMutex);
      s = g_settings;
    }

    if (s.showOnlyOnWinKey && !winKeyDown) {
      if (numbersVisible) {
        ShowWindow(hwnd, SW_HIDE);
        numbersVisible = false;
      }
    } else {
      HWND hwndTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
      if (hwndTaskbar && IsWindowVisible(hwndTaskbar)) {
        if (!numbersVisible) {
          ShowWindow(hwnd, SW_SHOWNA);
          SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
          numbersVisible = true;
        }
        DrawNumbers(*pRects, hwndTaskbar, s, hdcScreen, hdcMem, hBitmap, hOldBitmap, pvBits, cachedWidth, cachedHeight, pDCRT);
      } else {
        if (numbersVisible) {
          ShowWindow(hwnd, SW_HIDE);
          numbersVisible = false;
        }
      }
    }
    delete pRects;
    return 0;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread,
                           DWORD dwmsEventTime) {
  if (!hwnd) return;
  HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
  if (!taskbar) return;

  if (hwnd == taskbar || GetAncestor(hwnd, GA_ROOT) == taskbar)
    if (g_overlayHwnd) PostMessage(g_overlayHwnd, WM_APP_TASKBAR_CHANGED, 0, 0);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    KBDLLHOOKSTRUCT *pKbd = (KBDLLHOOKSTRUCT *)lParam;
    if (pKbd->vkCode == VK_LWIN || pKbd->vkCode == VK_RWIN) {
      if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        if (g_overlayHwnd) PostMessage(g_overlayHwnd, WM_APP_WINKBD, 1, 0);
      } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        if (g_overlayHwnd) PostMessage(g_overlayHwnd, WM_APP_WINKBD, 0, 0);
      }
    }
  }
  return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

void WorkerThreadWrapper() {
  CoInitializeEx(NULL, COINIT_MULTITHREADED);

  WNDCLASS wc = {0};
  wc.lpfnWndProc = OverlayWndProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = L"WindhawkTaskbarNumbersOverlay";
  RegisterClass(&wc);

  g_overlayHwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                                 wc.lpszClassName, L"", WS_POPUP, 0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);
  g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
  g_winEventHook = SetWinEventHook( EVENT_OBJECT_CREATE, EVENT_OBJECT_LOCATIONCHANGE, NULL, WinEventProc, 0,
                                   0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

  ModSettings currentSettings;
  {
    std::scoped_lock lock(g_settingsMutex);
    currentSettings = g_settings;
  }

  if (!currentSettings.showOnlyOnWinKey) PostMessage(g_overlayHwnd, WM_APP_TASKBAR_CHANGED, 0, 0);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (g_keyboardHook) UnhookWindowsHookEx(g_keyboardHook);
  if (g_winEventHook) UnhookWinEvent(g_winEventHook);

  if (g_overlayHwnd) {
    DestroyWindow(g_overlayHwnd);
    g_overlayHwnd = NULL;
  }
  UnregisterClass(L"WindhawkTaskbarNumbersOverlay", wc.hInstance);

  CoUninitialize();
}

BOOL WhTool_ModInit() {
  Wh_Log(L"Init");
  LoadSettings();

  HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &g_pD2DFactory);
  if (FAILED(hr)) return FALSE;

  hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&g_pDWriteFactory));
  if (FAILED(hr)) {
    g_pD2DFactory->Release();
    g_pD2DFactory = nullptr;
    return FALSE;
  }

  g_modStopping = false;
  g_workerThread = new std::thread(WorkerThreadWrapper);

  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"Uninit");
  g_modStopping = true;
  if (g_overlayHwnd) PostMessage(g_overlayHwnd, WM_QUIT, 0, 0);
  if (g_workerThread && g_workerThread->joinable()) g_workerThread->join();
  delete g_workerThread;
  g_workerThread = nullptr;

  if (g_pDWriteFactory) {
    g_pDWriteFactory->Release();
    g_pDWriteFactory = nullptr;
  }
  if (g_pD2DFactory) {
    g_pD2DFactory->Release();
    g_pD2DFactory = nullptr;
  }
}

// Dedicated process code:

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
  Wh_Log(L">");
  ExitThread(0);
}

BOOL Wh_ModInit() {
  bool isExcluded = false;
  bool isToolModProcess = false;
  bool isCurrentToolModProcess = false;
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
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

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
    IMAGE_NT_HEADERS *ntHeaders =
        (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);

    DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
    void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

    Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
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

  using CreateProcessInternalW_t = BOOL(WINAPI *)(
      HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
      LPSECURITY_ATTRIBUTES lpProcessAttributes,
      LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
      DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
      LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
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
