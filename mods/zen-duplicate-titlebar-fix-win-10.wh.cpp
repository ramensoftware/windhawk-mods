// ==WindhawkMod==
// @id              zen-duplicate-titlebar-fix-win-10
// @name            Zen Duplicate Titlebar Fix (Win10)
// @description     Removes the extra Windows titlebar buttons that can appear over Zen's own controls when using transparency mods on Windows 10.
// @version         1.0.0
// @author          landnthrn
// @github          https://github.com/landnthrn
// @homepage        https://github.com/landnthrn/zen-duplicate-titlebar-fix-win10
// @include         zen.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Duplicate titlebar buttons can occur on Zen when using transparent browser mods with DWMBlurGlass or OpenGlass, on Windows 10.     
This mod hides the extra Windows-native titlebar buttons and lets Zen's remain.

### Before
![Before fix](https://raw.githubusercontent.com/landnthrnnn/DUMP/refs/heads/main/GitHub/repos/zen-duplicate-titlebar-fix/before.png)

### After
![After fix](https://raw.githubusercontent.com/landnthrnnn/DUMP/refs/heads/main/GitHub/repos/zen-duplicate-titlebar-fix/after.png)

## How It Works

- Detects Zen's main browser windows and removes the `WS_SYSMENU` window style, preventing Windows from drawing its native titlebar buttons over Zen's.   
  Also prevents Zen from restoring that style while the mod is active.
- The fix involves removing the native Windows system menu from being used on Zen, normally accessible with `alt + space`
- Only targets `zen.exe`
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <cwchar>

 namespace {

 constexpr wchar_t kWindowClass[] = L"MozillaWindowClass";
 constexpr wchar_t kMarker[] =
     L"Windhawk.ZenHideDuplicateCaptionButtons";

 using CreateWindowExW_t = decltype(&CreateWindowExW);
 using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);

 CreateWindowExW_t CreateWindowExW_Original = nullptr;
 SetWindowLongPtrW_t SetWindowLongPtrW_Original = nullptr;

 bool g_isWindows10 = false;

 bool IsWindows10() {
     HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
     if (!ntdll)
         return false;

     using RtlGetVersion_t = LONG (WINAPI*)(OSVERSIONINFOW*);
     auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
         GetProcAddress(ntdll, "RtlGetVersion")
     );

     if (!rtlGetVersion)
         return false;

     OSVERSIONINFOW version = {};
     version.dwOSVersionInfoSize = sizeof(version);

     return rtlGetVersion(&version) == 0 &&
            version.dwMajorVersion == 10 &&
            version.dwBuildNumber < 22000;
 }

 constexpr LONG_PTR kRequiredStyles =
     WS_CAPTION | WS_THICKFRAME |
     WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

 constexpr UINT kRefreshFlags =
     SWP_NOMOVE | SWP_NOSIZE |
     SWP_NOZORDER | SWP_NOACTIVATE |
     SWP_FRAMECHANGED;

 bool IsZenBrowserWindow(HWND hwnd) {
     if (!hwnd || !IsWindow(hwnd))
         return false;

     DWORD processId = 0;
     GetWindowThreadProcessId(hwnd, &processId);

     if (processId != GetCurrentProcessId())
         return false;

     if (GetWindow(hwnd, GW_OWNER) != nullptr)
         return false;

     if (GetAncestor(hwnd, GA_ROOT) != hwnd)
         return false;

     wchar_t className[128] = {};

     if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)))
         return false;

     if (wcscmp(className, kWindowClass) != 0)
         return false;

     LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
     LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

     if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW))
         return false;

     return (style & kRequiredStyles) == kRequiredStyles;
 }

 void RefreshFrame(HWND hwnd) {
     SetWindowPos(
         hwnd,
         nullptr,
         0, 0, 0, 0,
         kRefreshFlags
     );
 }

 void ApplyFix(HWND hwnd) {
     if (!IsZenBrowserWindow(hwnd))
         return;

     LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

     if (!(style & WS_SYSMENU))
         return;

     LONG_PTR updated =
         style & ~static_cast<LONG_PTR>(WS_SYSMENU);

     SetLastError(ERROR_SUCCESS);

     LONG_PTR previous = SetWindowLongPtrW(
         hwnd,
         GWL_STYLE,
         updated
     );

     if (!previous && GetLastError() != ERROR_SUCCESS) {
         Wh_Log(L"Failed to update Zen window %p", hwnd);
         return;
     }

     SetPropW(
         hwnd,
         kMarker,
         reinterpret_cast<HANDLE>(1)
     );

     RefreshFrame(hwnd);

     Wh_Log(L"Removed ghost caption buttons: %p", hwnd);
 }

 void RestoreFix(HWND hwnd) {
     if (!IsWindow(hwnd))
         return;

     if (!GetPropW(hwnd, kMarker))
         return;

     RemovePropW(hwnd, kMarker);

     if (!IsZenBrowserWindow(hwnd))
         return;

     LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

     if (style & WS_SYSMENU)
         return;

     LONG_PTR updated =
         style | static_cast<LONG_PTR>(WS_SYSMENU);

     // Bypass our own SetWindowLongPtrW hook.
     auto original = SetWindowLongPtrW_Original
         ? SetWindowLongPtrW_Original
         : SetWindowLongPtrW;

     original(hwnd, GWL_STYLE, updated);

     RefreshFrame(hwnd);

     Wh_Log(L"Restored Zen window style: %p", hwnd);
 }

 BOOL CALLBACK ApplyExistingWindow(HWND hwnd, LPARAM) {
     ApplyFix(hwnd);
     return TRUE;
 }

 BOOL CALLBACK RestoreExistingWindow(HWND hwnd, LPARAM) {
     DWORD processId = 0;
     GetWindowThreadProcessId(hwnd, &processId);

     if (processId == GetCurrentProcessId())
         RestoreFix(hwnd);

     return TRUE;
 }

 HWND WINAPI CreateWindowExW_Hook(
     DWORD exStyle,
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
     LPVOID parameter
 ) {
     HWND hwnd = CreateWindowExW_Original(
         exStyle,
         className,
         windowName,
         style,
         x, y, width, height,
         parent,
         menu,
         instance,
         parameter
     );

     if (hwnd)
         ApplyFix(hwnd);

     return hwnd;
 }

 LONG_PTR WINAPI SetWindowLongPtrW_Hook(
     HWND hwnd,
     int index,
     LONG_PTR newValue
 ) {
     if (index == GWL_STYLE && IsZenBrowserWindow(hwnd)) {
         if ((newValue & kRequiredStyles) == kRequiredStyles &&
             (newValue & WS_SYSMENU)) {

             newValue &= ~static_cast<LONG_PTR>(WS_SYSMENU);

             SetLastError(ERROR_SUCCESS);

             LONG_PTR previous = SetWindowLongPtrW_Original(
                 hwnd,
                 index,
                 newValue
             );

             DWORD error = GetLastError();

             if (previous || error == ERROR_SUCCESS) {
                 SetPropW(
                     hwnd,
                     kMarker,
                     reinterpret_cast<HANDLE>(1)
                 );

                 Wh_Log(
                     L"Prevented WS_SYSMENU restoration: %p",
                     hwnd
                 );
             }

             SetLastError(error);
             return previous;
         }
     }

     return SetWindowLongPtrW_Original(
         hwnd,
         index,
         newValue
     );
 }

 } // namespace

 BOOL Wh_ModInit() {
     if (!IsWindows10()) {
         Wh_Log(L"Not Windows 10; leaving Zen windows unchanged");
         return TRUE;
     }

     g_isWindows10 = true;

     if (!Wh_SetFunctionHook(
         reinterpret_cast<void*>(CreateWindowExW),
         reinterpret_cast<void*>(CreateWindowExW_Hook),
         reinterpret_cast<void**>(&CreateWindowExW_Original)
     )) {
         Wh_Log(L"Failed to hook CreateWindowExW");
         return FALSE;
     }

     if (!Wh_SetFunctionHook(
         reinterpret_cast<void*>(SetWindowLongPtrW),
         reinterpret_cast<void*>(SetWindowLongPtrW_Hook),
         reinterpret_cast<void**>(&SetWindowLongPtrW_Original)
     )) {
         Wh_Log(L"Failed to hook SetWindowLongPtrW");
         return FALSE;
     }

     Wh_Log(L"Zen caption-button fix initialized");
     return TRUE;
 }

 void Wh_ModAfterInit() {
     if (g_isWindows10)
         EnumWindows(ApplyExistingWindow, 0);
 }

 void Wh_ModBeforeUninit() {
     if (g_isWindows10)
         EnumWindows(RestoreExistingWindow, 0);
 }

 void Wh_ModUninit() {
     Wh_Log(L"Zen caption-button fix unloaded");
 }
