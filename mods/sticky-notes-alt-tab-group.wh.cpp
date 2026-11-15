// ==WindhawkMod==
// @id              sticky-notes-alt-tab-group
// @name            Sticky Notes: Alt+Tab group
// @name:ko         스티커 메모 Alt+Tab 하나로 묶기
// @description     Group Microsoft Sticky Notes into one Alt+Tab entry, bring open notes forward together, and use compact previews.
// @description:ko  스티커 메모를 Alt+Tab 한 항목으로 묶고, 전환하면 열린 메모를 함께 표시하며 미리보기를 작게 만듭니다.
// @version         1.1.1
// @author          Choihyunseok1
// @github          https://github.com/Choihyunseok1
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -ldwmapi -luuid
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Sticky Notes: Alt+Tab group

Keep multiple Microsoft Sticky Notes windows from filling the Alt+Tab switcher.

- Show one representative Sticky Notes window in Alt+Tab.
- Switch to Sticky Notes to bring all its open windows on the current virtual
  desktop forward together.
  - Minimized notes are restored; closed notes are not opened.
  - Switching from another application by clicking a note or its taskbar button
    also brings the open notes forward.
- Use compact Sticky Notes previews, with a default maximum source size of
  260 by 160 pixels while preserving the aspect ratio.
  - Other shell views that use the same thumbnail factory can also use this
    compact source size. The shell controls the final display scale.
  - Matching uses the note window's extended frame bounds.
- Keep note contents, window sizes, positions, and ownership unchanged.

## Compatibility

- Windows 11, x64, with the standard XAML Alt+Tab switcher.
  - Locally tested on Windows 11 25H2, build 26200.9457, Windhawk 1.7.3,
    and Microsoft Sticky Notes 6.1.4.0.
  - Other Windows builds and the newer OneNote-based Sticky Notes experience
    have not been tested.
- Uses private shell symbols, which may change after Windows updates.
- Tested alongside Alt+Tab per monitor 1.2 with its cursor-monitor filter.
  - That mod's monitor filtering still applies. Notes restored after selection
    can be on any monitor on the current virtual desktop.
- Disabling this mod restores the normal switcher behavior.
  - Do not enable both a local copy and the catalog version simultaneously.

## Settings

- **Preview maximum width / height**: maximum reported Sticky Notes thumbnail
  source dimensions. Values at or below zero use the defaults (260 / 160).

## Credits

The Alt+Tab visibility-filtering approach is adapted from
[Alt+Tab per monitor](https://windhawk.net/mods/alt-tab-per-monitor) by L3r0y and
[Alt+Tab: Hide ghost windows](https://windhawk.net/mods/alt-tab-hide-ghost-windows)
by David Trapp (CherryDT). Their MIT notices are retained below.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- previewMaxWidth: 260
  $name: Preview maximum width
  $description: Maximum thumbnail source width in pixels. Zero or negative uses 260.
- previewMaxHeight: 160
  $name: Preview maximum height
  $description: Maximum thumbnail source height in pixels. Zero or negative uses 160.
*/
// ==/WindhawkModSettings==

/*
MIT License

Copyright (c) 2026 Choihyunseok1
Copyright (c) 2025 L3r0yThingz
Copyright (c) 2025 David Trapp (CherryDT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <windhawk_utils.h>
#include <dwmapi.h>
#include <shobjidl.h>
#include <atomic>
#include <vector>
#include <cwchar>

std::atomic<DWORD> buildThread{0}, lastThread{0};
std::atomic<ULONGLONG> buildTime{0};
std::atomic<HWND> representative{nullptr};
void *win32Vtable, *winrtVtable;
using NativeWindow = HRESULT(WINAPI*)(void*, HWND*);
NativeWindow win32Window, winrtWindow;
using Visible = HRESULT(WINAPI*)(void*, void*, BOOL*);
Visible originalVisible;
using Create = HRESULT(WINAPI*)(void*, void*, void*);
Create originalCreate;
HANDLE stopEvent, worker;
IVirtualDesktopManager* desktops;
bool wasNotes;
struct PreviewSize { float width, height; };
struct PreviewBounds { float x, y, width, height; };
using SourceSize = HRESULT(WINAPI*)(void*, PreviewSize*);
using SourceBounds = HRESULT(WINAPI*)(void*, PreviewBounds*);
SourceSize originalSourceSize;
SourceBounds sourceBounds;

HRESULT WINAPI SourceSizeHook(void* self, PreviewSize* size);

bool NotesProcess(HWND window) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    wchar_t path[32768]; DWORD length = ARRAYSIZE(path);
    bool result = false;
    if (QueryFullProcessImageNameW(process, 0, path, &length)) {
        auto name = wcsrchr(path, L'\\');
        result = _wcsicmp(name ? name + 1 : path, L"Microsoft.Notes.exe") == 0;
    }
    CloseHandle(process);
    return result;
}
bool Notes(HWND window) {
    if (!window) return false;
    if (NotesProcess(window)) return true;
    wchar_t cls[128];
    if (!GetClassNameW(window, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"ApplicationFrameWindow")) return false;
    bool found = false;
    EnumChildWindows(window, [](HWND child, LPARAM data) -> BOOL {
        if (!NotesProcess(child)) return TRUE;
        *reinterpret_cast<bool*>(data) = true; return FALSE;
    }, reinterpret_cast<LPARAM>(&found));
    return found;
}
HRESULT WINAPI SourceSizeHook(void* self, PreviewSize* size) {
    HRESULT hr = originalSourceSize(self, size);
    if (FAILED(hr) || size->width <= 0 || size->height <= 0) return hr;
    PreviewBounds bounds{};
    if (FAILED(sourceBounds(self, &bounds))) return hr;
    struct Match { PreviewBounds bounds; bool notes = false; } match{bounds};
    EnumWindows([](HWND window, LPARAM data) -> BOOL {
        if (!IsWindowVisible(window)) return TRUE;
        auto& m = *reinterpret_cast<Match*>(data);
        RECT r{};
        if (FAILED(DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r)))) return TRUE;
        auto closeEnough = [](float a, float b) { return a - b < 2 && b - a < 2; };
        if (closeEnough(m.bounds.x, r.left) && closeEnough(m.bounds.y, r.top) &&
            closeEnough(m.bounds.width, r.right - r.left) && closeEnough(m.bounds.height, r.bottom - r.top)) {
            // Only inspect the process after the cheap bounds match.
            if (!Notes(window)) return TRUE;
            m.notes = true; return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&match));
    if (match.notes) {
        int width = Wh_GetIntSetting(L"previewMaxWidth");
        int height = Wh_GetIntSetting(L"previewMaxHeight");
        if (width <= 0) width = 260;
        if (height <= 0) height = 160;
        float scale = 1.0f;
        if (size->width > width) scale = width / size->width;
        if (size->height * scale > height) scale = height / size->height;
        size->width *= scale; size->height *= scale;
    }
    return hr;
}
HRESULT WINAPI VisibleHook(void* self, void* view, BOOL* visible) {
    HRESULT hr = originalVisible(self, view, visible);
    if (FAILED(hr) || !*visible) return hr;
    DWORD thread = GetCurrentThreadId();
    if (buildThread != thread && (lastThread != thread || GetTickCount64() - buildTime > 250)) return hr;
    HWND window = nullptr;
    void* table = *reinterpret_cast<void**>(view);
    if (table == win32Vtable) hr = win32Window(view, &window);
    else if (table == winrtVtable) hr = winrtWindow(view, &window);
    else return S_OK;
    if (FAILED(hr) || !Notes(window)) return S_OK;
    HWND expected = nullptr;
    representative.compare_exchange_strong(expected, window);
    if (representative != window) *visible = FALSE;
    return S_OK;
}
HRESULT WINAPI CreateHook(void* a, void* b, void* c) {
    representative = nullptr;
    buildThread = GetCurrentThreadId(); lastThread = GetCurrentThreadId();
    buildTime = GetTickCount64();
    HRESULT hr = originalCreate(a, b, c);
    buildThread = 0;
    return hr;
}
void CALLBACK Foreground(HWINEVENTHOOK, DWORD, HWND window, LONG object, LONG, DWORD, DWORD) {
    if (object != OBJID_WINDOW || !window) return;
    bool notes = Notes(window), bring = notes && !wasNotes;
    wasNotes = notes;
    if (!bring) return;
    std::vector<HWND> windows;
    EnumWindows([](HWND candidate, LPARAM data) -> BOOL {
        if (!IsWindowVisible(candidate) || !Notes(candidate)) return TRUE;
        BOOL current = FALSE;
        if (!desktops || FAILED(desktops->IsWindowOnCurrentVirtualDesktop(candidate, &current)) || !current) return TRUE;
        DWORD cloaked = 0;
        DwmGetWindowAttribute(candidate, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        if (!cloaked) reinterpret_cast<std::vector<HWND>*>(data)->push_back(candidate);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
        if (*it == window) continue;
        if (IsIconic(*it)) ShowWindowAsync(*it, SW_SHOWNOACTIVATE);
        SetWindowPos(*it, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
    SetWindowPos(window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    Wh_Log(L"Raised %u Sticky Notes windows", static_cast<unsigned>(windows.size()));
}
DWORD WINAPI Run(void*) {
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&desktops));
    wasNotes = Notes(GetForegroundWindow());
    auto hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, Foreground, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!hook) Wh_Log(L"Foreground hook failed: %u", GetLastError());
    MSG msg;
    while (MsgWaitForMultipleObjects(1, &stopEvent, FALSE, INFINITE, QS_ALLINPUT) != WAIT_OBJECT_0) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    }
    if (hook) UnhookWinEvent(hook);
    if (desktops) { desktops->Release(); desktops = nullptr; }
    if (SUCCEEDED(init)) CoUninitialize();
    return 0;
}
BOOL Wh_ModInit() {
    HMODULE shell = LoadLibraryW(L"twinui.pcshell.dll");
    if (!shell) return FALSE;
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"}, &originalVisible, VisibleHook},
        {{LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"}, &win32Vtable},
        {{LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"}, &win32Window},
        {{LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"}, &winrtVtable},
        {{LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"}, &winrtWindow},
        {{LR"(long __cdecl XamlAltTabViewHost_CreateInstance(struct XamlViewHostInitializeArgs const &,struct _GUID const &,void * *))"}, &originalCreate, CreateHook},
    };
    if (!WindhawkUtils::HookSymbols(shell, hooks, ARRAYSIZE(hooks))) return FALSE;
    HMODULE model = LoadLibraryW(L"DesktopSwitcherDataModel.dll");
    // DesktopSwitcherDataModel.dll
    WindhawkUtils::SYMBOL_HOOK previewHooks[] = {
        {{L"public: virtual long __cdecl WindowThumbnailFactoryBase::get_SourceSize(struct Windows::Foundation::Size *)"}, &originalSourceSize, SourceSizeHook},
        {{L"public: virtual long __cdecl WindowThumbnailFactoryBase::get_Bounds(struct Windows::Foundation::Rect *)"}, &sourceBounds},
    };
    if (!model || !WindhawkUtils::HookSymbols(model, previewHooks, ARRAYSIZE(previewHooks))) return FALSE;
    stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return FALSE;
    worker = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
    if (!worker) { CloseHandle(stopEvent); stopEvent = nullptr; return FALSE; }
    return TRUE;
}
void Wh_ModUninit() {
    if (stopEvent) SetEvent(stopEvent);
    if (worker) { WaitForSingleObject(worker, INFINITE); CloseHandle(worker); }
    if (stopEvent) CloseHandle(stopEvent);
}
