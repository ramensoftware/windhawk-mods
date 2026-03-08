// ==WindhawkMod==
// @id              numbered-taskbar
// @name            Numbered taskbar
// @description     Adds numbers (1-0) to taskbar icons corresponding to Win + Number shortcut
// @version         1.0.0
// @author          maciejlewand72
// @github          https://github.com/maciejlewand72
// @include         explorer.exe
// @compilerOptions -luiautomationcore -lcomdlg32 -lgdiplus -lole32 -loleaut32 -luser32 -lgdi32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Numbered taskbar
Displays numbers 1 through 0 on the first 10 application icons in the taskbar.
These numbers correspond to the `Win + Number` keyboard shortcuts which launch the respective applications.
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
#include <gdiplus.h>
#include <mutex>
#include <string>
#include <thread>
#include <uiautomation.h>
#include <vector>
#include <windows.h>

using namespace Gdiplus;

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

void LoadSettings() {
  ModSettings newSettings;
  newSettings.fontSize = Wh_GetIntSetting(L"fontSize");
  newSettings.fontColor = Wh_GetIntSetting(L"fontColor");
  newSettings.outlineColor = Wh_GetIntSetting(L"outlineColor");
  newSettings.offsetX = Wh_GetIntSetting(L"offsetX");
  newSettings.offsetY = Wh_GetIntSetting(L"offsetY");
  newSettings.showOnlyOnWinKey = Wh_GetIntSetting(L"showOnlyOnWinKey") != 0;

  newSettings.delayMs = Wh_GetIntSetting(L"delayMs");
  if (newSettings.delayMs < 0) newSettings.delayMs = 0;
  if (newSettings.fontSize < 1) newSettings.fontSize = 1;
  if (newSettings.fontSize > 200) newSettings.fontSize = 200;

  if (newSettings.offsetX < -1000) newSettings.offsetX = -1000;
  if (newSettings.offsetX > 1000) newSettings.offsetX = 1000;
  if (newSettings.offsetY < -1000) newSettings.offsetY = -1000;
  if (newSettings.offsetY > 1000) newSettings.offsetY = 1000;

  newSettings.updateIntervalMs = Wh_GetIntSetting(L"updateIntervalMs");
  if (newSettings.updateIntervalMs < 50) newSettings.updateIntervalMs = 50;
  if (newSettings.updateIntervalMs > 60000) newSettings.updateIntervalMs = 60000;

  std::lock_guard<std::mutex> lock(g_settingsMutex);
  g_settings = newSettings;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

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
                    cls == L"SystemTray.AccentButton" || cls == L"SystemTray.OmniButtonCenter") isAppButton = false;
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
  if (hwndTaskbar) {
    RECT taskbarRect;
    if (GetWindowRect(hwndTaskbar, &taskbarRect))
      if ((taskbarRect.bottom - taskbarRect.top) > (taskbarRect.right - taskbarRect.left)) isVertical = true;
  }

  if (isVertical) std::sort(rects.begin(), rects.end(), [](const RECT &a, const RECT &b) { return a.top < b.top; });
  else std::sort(rects.begin(), rects.end(), [](const RECT &a, const RECT &b) { return a.left < b.left; });
  return rects;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_NCHITTEST) return HTTRANSPARENT;
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void DrawNumbers(const std::vector<RECT> &rects, HWND hwndTaskbar, const ModSettings &settings, HDC hdcScreen, HDC hdcMem,
                 HBITMAP &hBitmap, HBITMAP &hOldBitmap, void *&pvBits, int &cachedWidth, int &cachedHeight) {
  if (!g_overlayHwnd) return;

  RECT taskbarRect;
  GetWindowRect(hwndTaskbar, &taskbarRect);

  int width = taskbarRect.right - taskbarRect.left;
  int height = taskbarRect.bottom - taskbarRect.top;

  if (width <= 0 || height <= 0) return;

  SetWindowPos(g_overlayHwnd, HWND_TOPMOST, taskbarRect.left, taskbarRect.top, width, height, SWP_NOACTIVATE | SWP_NOZORDER);

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
  }

  if (pvBits && cachedWidth > 0 && cachedHeight > 0) memset(pvBits, 0, cachedWidth * cachedHeight * 4);

  Graphics graphics(hdcMem);
  graphics.SetSmoothingMode(SmoothingModeAntiAlias);

  FontFamily fontFamily(L"Segoe UI");
  Font font(&fontFamily, (REAL)settings.fontSize, FontStyleBold, UnitPixel);

  SolidBrush textBrush(Color(settings.fontColor));

  StringFormat format;
  format.SetAlignment(StringAlignmentNear);
  format.SetLineAlignment(StringAlignmentNear);

  int index = 1;
  for (const RECT &r : rects) {
    if (index > 10) break;

    std::wstring text = std::to_wstring(index == 10 ? 0 : index);

    int x = r.left - taskbarRect.left + settings.offsetX;
    int y = r.top - taskbarRect.top + settings.offsetY;

    GraphicsPath path;
    path.AddString(text.c_str(), -1, &fontFamily, FontStyleBold, (REAL)settings.fontSize, Point(x, y), &format);

    Pen outlinePen(Color(settings.outlineColor), 2.0f);
    outlinePen.SetLineJoin(LineJoinRound);

    graphics.DrawPath(&outlinePen, &path);
    graphics.FillPath(&textBrush, &path);

    index++;
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

void WorkerThreadWrapper() {
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

  WNDCLASS wc = {0};
  wc.lpfnWndProc = OverlayWndProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = L"WindhawkTaskbarNumbersOverlay";
  RegisterClass(&wc);

  g_overlayHwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                                 wc.lpszClassName, L"", WS_POPUP, 0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);

  IUIAutomation *pUIAutomation = NULL;
  CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void **)&pUIAutomation);

  ULONGLONG winKeyPressTime = 0;

  HDC hdcScreen = GetDC(NULL);
  HDC hdcMem = CreateCompatibleDC(hdcScreen);
  HBITMAP hBitmap = NULL;
  HBITMAP hOldBitmap = NULL;
  void *pvBits = nullptr;
  int cachedWidth = 0;
  int cachedHeight = 0;

  while (!g_modStopping) {
    ModSettings currentSettings;
    {
      std::lock_guard<std::mutex> lock(g_settingsMutex);
      currentSettings = g_settings;
    }

    bool isWinKeyPressed = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 || (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    bool shouldShow = true;

    if (currentSettings.showOnlyOnWinKey) {
      if (isWinKeyPressed) {
        if (winKeyPressTime == 0) winKeyPressTime = GetTickCount64();
        if (GetTickCount64() - winKeyPressTime < (ULONGLONG)currentSettings.delayMs) shouldShow = false;
      } else {
        winKeyPressTime = 0;
        shouldShow = false;
      }
    }

    HWND hwndTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (shouldShow && hwndTaskbar && IsWindowVisible(hwndTaskbar)) {
      if (!IsWindowVisible(g_overlayHwnd))
        ShowWindow(g_overlayHwnd, SW_SHOWNA);
      SetWindowPos(g_overlayHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
      std::vector<RECT> rects = GetTaskbarButtonRects(pUIAutomation);
      DrawNumbers(rects, hwndTaskbar, currentSettings, hdcScreen, hdcMem, hBitmap, hOldBitmap, pvBits, cachedWidth, cachedHeight);
    } else if (IsWindowVisible(g_overlayHwnd)) ShowWindow(g_overlayHwnd, SW_HIDE);

    Sleep(currentSettings.updateIntervalMs);
  }

  if (hBitmap) {
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
  }
  DeleteDC(hdcMem);
  ReleaseDC(NULL, hdcScreen);

  if (pUIAutomation) pUIAutomation->Release();

  if (g_overlayHwnd) {
    DestroyWindow(g_overlayHwnd);
    g_overlayHwnd = NULL;
  }
  UnregisterClass(L"WindhawkTaskbarNumbersOverlay", wc.hInstance);

  CoUninitialize();
}

ULONG_PTR g_gdiplusToken;

BOOL Wh_ModInit() {
  Wh_Log(L"Init");
  LoadSettings();

  GdiplusStartupInput gdiplusStartupInput;
  GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

  g_modStopping = false;
  g_workerThread = new std::thread(WorkerThreadWrapper);

  return TRUE;
}

void Wh_ModUninit() {
  Wh_Log(L"Uninit");
  g_modStopping = true;
  if (g_workerThread && g_workerThread->joinable()) g_workerThread->join();
  delete g_workerThread;
  g_workerThread = nullptr;

  GdiplusShutdown(g_gdiplusToken);
}
