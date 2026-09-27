// ==WindhawkMod==
// @id              edge-native-ui-tweaks
// @name            Edge Native UI Tweaks
// @description     Use Windows folder icons for Favorites and restore Edge's classic yellow folder icons.
// @version         1.0
// @author          Dron007
// @github          https://github.com/Dron007
// @include         msedge.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModSettings==
/*
- favorites:
  - useWindowsFolderIcon: true
    $name: "Use Windows folder icons"
    $description: "Use Windows folder icons on the Favorites bar, in folder menus, and for drag images. Also restores Edge's yellow folder icons in the Favorites flyout, pane, and page."
  $name: "Favorites"
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Edge Native UI Tweaks

Customize Favorites folder icons in Microsoft Edge's native browser interface without affecting web content.

![Favorites folder icons](https://i.imgur.com/idipoQV.png)

## Getting started

1. Enable the mod.
2. Restart Edge. If closing and reopening it doesn't restart the browser process, use `edge://restart`.
3. Enable or disable **Use Windows folder icons** under **Favorites**.

Once the mod is active, the setting applies immediately without restarting Edge.

Disabling or uninstalling the mod restores Edge's original folder icons in the running browser. Re-enabling the mod in that same Edge instance may require a restart; use `edge://restart` if closing and reopening Edge doesn't do it.

## Features

### Favorites

When **Use Windows folder icons** is enabled:

- Favorites bar folders, their drop-down menus, and folder drag images use the Windows system folder icon.
- The Favorites flyout, pinned Favorites pane, and Favorites page use Edge's older yellow folder icons instead of the newer monoline icons.

## Notes

- Tested with **Microsoft Edge 154.0.4258.37 x64**.
- Edge controls its newer monochrome folder style through the `msFavoritesMonolineFolder` feature. The mod suppresses that path while the option is enabled. This keeps the implementation simpler and avoids hooking private Favorites menu/button classes.
- The older non-monoline path is still present in current Edge but may eventually be removed by Microsoft. If that happens, the yellow Edge icons in the Favorites flyout, pane, and page may no longer be restorable. The Windows icons on the Favorites bar, its folder menus, and drag images use a separate icon hook.
- The mod relies on Edge's internal native UI symbols, so Edge updates can occasionally require compatibility adjustments. If the required symbols aren't available, the folder-icon tweak is left inactive rather than partially applied.
- After an Edge update, Windhawk may need to download and resolve new `msedge.dll` symbols. The mod waits up to 5 seconds for cached symbols. If resolution takes longer, Edge continues with its normal UI while symbol preparation finishes in the background. A temporary tray icon shows the current status and notifies you when the symbols are ready; restart Edge once after preparation completes. If closing and reopening Edge doesn't restart the browser process, use `edge://restart`.
- A full fresh-symbol fallback can still take several minutes. In an Edge 154.0.4258.37 test with the local PDB and Windhawk symbol cache removed, `msedge.dll.pdb` was **676.0 MB (644.7 MiB)** and total preparation took **515.1 seconds (8 min 35 sec)** on a **75 Mbit/s** connection and an **Intel Core i7-8700K**. The PDB download itself took about **8 min 32 sec**, while symbol resolution after the PDB was opened took under **2 seconds**. In this test Microsoft's symbol server delivered the `.pdb` directly rather than the compressed `.pd_` variant, and effective transfer speed was only about **10.6 Mbit/s**, so the download dominated the total time. Times vary with symbol-server/CDN throughput, connection, CPU, and Edge build.
- Disabling, updating, or uninstalling the mod while fresh symbol preparation is still running can leave Windhawk in `uninitializing` until that work completes. This is intentional: the symbol resolver can't be cancelled safely while code from the mod is still executing.
- Edge processes launched with `--remote-debugging-pipe` are intentionally skipped so automation sessions don't pay the native UI symbol-resolution cost.
- The Windows system folder icon is a raster icon. It doesn't follow Edge theme colors and isn't fully DPI-aware on high-DPI or mixed-DPI monitor setups.
*/
// ==/WindhawkModReadme==
// clang-format on

#include <shellapi.h>
#include <windhawk_utils.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cwchar>
#include <mutex>
#include <unordered_map>
#include <vector>

static constexpr PCWSTR kEdgeSymbolServer =
    L"https://msdl.microsoft.com/download/symbols";
static constexpr DWORD kEdgeSymbolStartupWaitMs = 5000;
static constexpr wchar_t kEdgeWidgetWindowClassPrefix[] = L"Chrome_WidgetWin_";

// -----------------------------------------------------------------------------
// Opaque Chromium types
// -----------------------------------------------------------------------------

struct ImageModelOpaque;

static constexpr size_t kOpaqueObjectStorageSize = 4096;
static constexpr size_t kOpaqueObjectGuardSize = 256;
static constexpr unsigned char kOpaqueObjectGuardValue = 0xA5;

struct alignas(64) OpaqueObjectStorage {
    unsigned char data[kOpaqueObjectStorageSize];
    unsigned char guard[kOpaqueObjectGuardSize];
};

static OpaqueObjectStorage g_skBitmapStorage;
static OpaqueObjectStorage g_imageSkiaStorage;

static void PrepareOpaqueObjectStorage(OpaqueObjectStorage& storage) {
    std::fill_n(storage.guard, kOpaqueObjectGuardSize, kOpaqueObjectGuardValue);
}

static bool IsOpaqueObjectGuardIntact(const OpaqueObjectStorage& storage) {
    return std::all_of(
        storage.guard, storage.guard + kOpaqueObjectGuardSize,
        [](unsigned char value) { return value == kOpaqueObjectGuardValue; });
}

// -----------------------------------------------------------------------------
// Function types
// -----------------------------------------------------------------------------

// Logical:
// ui::ImageModel chrome::GetBookmarkFolderIcon(
//     BookmarkFolderIconType,
//     ui::ColorVariant);
//
// Win64: hidden return buffer is the first argument.
using GetBookmarkFolderIconFn =
    ImageModelOpaque* (*)(ImageModelOpaque* result,
                          int iconType,
                          uintptr_t colorVariantOpaque);

// bool bookmarks::IsMonolineFolderEnabled()
using IsMonolineFolderEnabledFn = bool (*)();

// SkBitmap IconUtil::CreateSkBitmapFromHICON(HICON)
using CreateSkBitmapFromHICONFn = void* (*)(void* result, HICON icon);

// gfx::ImageSkia gfx::ImageSkia::CreateFrom1xBitmap(const SkBitmap&)
using ImageSkiaCreateFrom1xBitmapFn = void* (*)(void* result,
                                                const void* bitmap);

using OpaqueObjectDtorFn = void (*)(void*);

// ui::ImageModel ui::ImageModel::FromImageSkia(const gfx::ImageSkia&)
using ImageModelFromImageSkiaFn =
    ImageModelOpaque* (*)(ImageModelOpaque* result, const void* imageSkia);

using BookmarkBarViewOnThemeChangedFn = void (*)(void* self);
using BookmarkBarViewDeletingDtorFn = void* (*)(void* self, unsigned int flags);

// -----------------------------------------------------------------------------
// Resolved symbols
// -----------------------------------------------------------------------------

static GetBookmarkFolderIconFn g_GetBookmarkFolderIconOriginal;
static IsMonolineFolderEnabledFn g_IsMonolineFolderEnabledOriginal;
static CreateSkBitmapFromHICONFn g_CreateSkBitmapFromHICON;
static ImageSkiaCreateFrom1xBitmapFn g_ImageSkiaCreateFrom1xBitmap;
static OpaqueObjectDtorFn g_SkBitmapDtor;
static OpaqueObjectDtorFn g_ImageSkiaDtor;
static ImageModelFromImageSkiaFn g_ImageModelFromImageSkia;
static BookmarkBarViewOnThemeChangedFn g_BookmarkBarViewOnThemeChangedOriginal;
static BookmarkBarViewDeletingDtorFn g_BookmarkBarViewDeletingDtorOriginal;

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static std::atomic_bool g_edgeSetupStarted = false;
static std::atomic_bool g_hooksActivated = false;
static std::atomic_bool g_symbolResolutionSucceeded = false;

// If the bounded startup wait expires, symbol preparation can continue only to
// populate Windhawk's cache. Hooks aren't activated late in that Edge instance.
static std::atomic_bool g_hookActivationAbandoned = false;

static std::mutex g_workerMutex;
static std::condition_variable g_workerCondition;
static bool g_unloading = false;
static bool g_edgeSetupInProgress = false;
static HANDLE g_symbolResolutionThread;
static HANDLE g_symbolResolutionDoneEvent;
static HANDLE g_symbolNotificationThread;
static HANDLE g_symbolNotificationStopEvent;

static std::atomic_bool g_useWindowsFolderIcon = true;
static std::atomic_bool g_folderIconFeatureReady = false;
static std::atomic_bool g_windowsFolderReady = false;

static std::once_flag g_windowsFolderOnce;
static std::mutex g_windowsFolderImageMutex;
static DWORD g_windowsFolderThreadId = 0;

static std::mutex g_bookmarkBarsMutex;
static std::unordered_map<void*, DWORD> g_bookmarkBars;

static void ClearEdgeRuntimeReadiness() {
    g_folderIconFeatureReady.store(false, std::memory_order_release);
}

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

static void LoadSettings() {
    bool useWindowsFolderIcon =
        Wh_GetIntSetting(L"favorites.useWindowsFolderIcon") != 0;

    g_useWindowsFolderIcon.store(useWindowsFolderIcon,
                                 std::memory_order_relaxed);

    Wh_Log(L"Windows Favorites folder icon: %ls",
           useWindowsFolderIcon ? L"enabled" : L"disabled");
}

// -----------------------------------------------------------------------------
// Windows folder ImageSkia
// -----------------------------------------------------------------------------

static void CreateWindowsFolderImage() {
    std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

    if (!g_CreateSkBitmapFromHICON || !g_ImageSkiaCreateFrom1xBitmap ||
        !g_SkBitmapDtor || !g_ImageSkiaDtor || !g_ImageModelFromImageSkia) {
        Wh_Log(L"Windows folder image helpers unavailable");
        return;
    }

    SHSTOCKICONINFO iconInfo = {};
    iconInfo.cbSize = sizeof(iconInfo);

    HRESULT hr = SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICON | SHGSI_SMALLICON,
                                    &iconInfo);

    if (FAILED(hr) || !iconInfo.hIcon) {
        Wh_Log(L"SHGetStockIconInfo(SIID_FOLDER) failed: 0x%08X",
               static_cast<unsigned int>(hr));
        return;
    }

    PrepareOpaqueObjectStorage(g_skBitmapStorage);

    g_CreateSkBitmapFromHICON(g_skBitmapStorage.data, iconInfo.hIcon);

    DestroyIcon(iconInfo.hIcon);

    if (!IsOpaqueObjectGuardIntact(g_skBitmapStorage)) {
        Wh_Log(
            L"ERROR: SkBitmap exceeded reserved opaque storage; "
            L"skipping destructor");
        return;
    }

    PrepareOpaqueObjectStorage(g_imageSkiaStorage);

    g_ImageSkiaCreateFrom1xBitmap(g_imageSkiaStorage.data,
                                  g_skBitmapStorage.data);

    g_SkBitmapDtor(g_skBitmapStorage.data);

    if (!IsOpaqueObjectGuardIntact(g_imageSkiaStorage)) {
        Wh_Log(L"ERROR: gfx::ImageSkia exceeded reserved opaque storage");
        return;
    }

    g_windowsFolderThreadId = GetCurrentThreadId();

    g_windowsFolderReady.store(true, std::memory_order_release);

    Wh_Log(L"Windows Favorites folder image ready on UI thread %lu",
           g_windowsFolderThreadId);
}

static void DestroyWindowsFolderImageOnCurrentThread() {
    std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

    if (!g_windowsFolderReady.load(std::memory_order_acquire)) {
        return;
    }

    DWORD threadId = GetCurrentThreadId();

    if (g_windowsFolderThreadId != threadId) {
        Wh_Log(
            L"Windows folder image cleanup skipped on non-owning thread %lu "
            L"(owner %lu)",
            threadId, g_windowsFolderThreadId);
        return;
    }

    g_windowsFolderReady.store(false, std::memory_order_release);

    g_ImageSkiaDtor(g_imageSkiaStorage.data);
    g_windowsFolderThreadId = 0;
}

// -----------------------------------------------------------------------------
// Folder icon hooks
// -----------------------------------------------------------------------------

static ImageModelOpaque* GetBookmarkFolderIconHook(
    ImageModelOpaque* result,
    int iconType,
    uintptr_t colorVariantOpaque) {
    constexpr int kNormal = 0;

    if (iconType != kNormal ||
        !g_folderIconFeatureReady.load(std::memory_order_relaxed) ||
        !g_useWindowsFolderIcon.load(std::memory_order_relaxed)) {
        return g_GetBookmarkFolderIconOriginal(result, iconType,
                                               colorVariantOpaque);
    }

    std::call_once(g_windowsFolderOnce, CreateWindowsFolderImage);

    std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

    if (!g_useWindowsFolderIcon.load(std::memory_order_relaxed) ||
        !g_windowsFolderReady.load(std::memory_order_acquire)) {
        return g_GetBookmarkFolderIconOriginal(result, iconType,
                                               colorVariantOpaque);
    }

    return g_ImageModelFromImageSkia(result, g_imageSkiaStorage.data);
}

// Edge uses this bookmark-specific helper to select its monochrome folder
// icon path. Returning false while the option is enabled keeps Edge on the
// normal bookmark-folder path, where GetBookmarkFolderIcon is hooked above.
static bool IsMonolineFolderEnabledHook() {
    if (!g_folderIconFeatureReady.load(std::memory_order_relaxed) ||
        !g_useWindowsFolderIcon.load(std::memory_order_relaxed)) {
        return g_IsMonolineFolderEnabledOriginal();
    }

    return false;
}

// -----------------------------------------------------------------------------
// BookmarkBarView tracking
//
// Avoid the BookmarkBarView constructor: its signature contains Chromium-owned
// by-value objects and is more sensitive to version changes. OnThemeChanged is
// a simple virtual void method and already rebuilds the bar's appearance.
// -----------------------------------------------------------------------------

static void BookmarkBarViewOnThemeChangedHook(void* self) {
    g_BookmarkBarViewOnThemeChangedOriginal(self);

    if (!g_folderIconFeatureReady.load(std::memory_order_relaxed)) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);
    g_bookmarkBars[self] = GetCurrentThreadId();
}

static void* BookmarkBarViewDeletingDtorHook(void* self, unsigned int flags) {
    {
        std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);
        g_bookmarkBars.erase(self);
    }

    return g_BookmarkBarViewDeletingDtorOriginal(self, flags);
}

// -----------------------------------------------------------------------------
// Run code on an Edge UI thread
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc = void(WINAPI*)(void* parameter);

static UINT GetRunFromWindowThreadMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    return message;
}

static bool RunFromWindowThread(HWND hwnd,
                                RunFromWindowThreadProc proc,
                                void* parameter) {
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);

    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(parameter);
        return true;
    }

    struct Param {
        RunFromWindowThreadProc proc;
        void* parameter;
    };

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);

                if (cwp->message == GetRunFromWindowThreadMessage()) {
                    Param* param = reinterpret_cast<Param*>(cwp->lParam);

                    param->proc(param->parameter);
                }
            }

            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);

    if (!hook) {
        return false;
    }

    Param param{proc, parameter};

    SendMessageW(hwnd, GetRunFromWindowThreadMessage(), 0,
                 reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return true;
}

// -----------------------------------------------------------------------------
// Find an Edge window for a UI thread
// -----------------------------------------------------------------------------

static HWND FindWindowForThread(DWORD threadId) {
    struct Context {
        HWND first = nullptr;
        HWND edge = nullptr;
    } context;

    EnumThreadWindows(
        threadId,
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            Context* context = reinterpret_cast<Context*>(lParam);

            if (!context->first) {
                context->first = hwnd;
            }

            wchar_t className[128] = {};

            if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
                if (wcsncmp(className, kEdgeWidgetWindowClassPrefix,
                            ARRAYSIZE(kEdgeWidgetWindowClassPrefix) - 1) == 0) {
                    context->edge = hwnd;
                    return FALSE;
                }
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));

    return context.edge ? context.edge : context.first;
}

static void WINAPI DestroyWindowsFolderImageOnCurrentThreadProc(void*) {
    DestroyWindowsFolderImageOnCurrentThread();
}

static void DestroyWindowsFolderImageOnOwningThread() {
    DWORD threadId = 0;

    {
        std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

        if (!g_windowsFolderReady.load(std::memory_order_acquire)) {
            return;
        }

        threadId = g_windowsFolderThreadId;
    }

    if (!threadId) {
        return;
    }

    HWND hwnd = FindWindowForThread(threadId);

    if (!hwnd) {
        Wh_Log(
            L"Windows folder image cleanup: no window found for owning UI "
            L"thread "
            L"%lu; leaving it alive",
            threadId);
        return;
    }

    if (!RunFromWindowThread(hwnd, DestroyWindowsFolderImageOnCurrentThreadProc,
                             nullptr)) {
        Wh_Log(
            L"Windows folder image cleanup: failed to dispatch to owning UI "
            L"thread %lu; leaving it alive",
            threadId);
    }
}

// -----------------------------------------------------------------------------
// Live folder-icon refresh
// -----------------------------------------------------------------------------

static void WINAPI RefreshBookmarkBarsOnCurrentThread(void*) {
    if (!g_BookmarkBarViewOnThemeChangedOriginal) {
        return;
    }

    DWORD threadId = GetCurrentThreadId();

    std::vector<void*> bookmarkBars;

    {
        std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);

        for (const auto& [bookmarkBar, bookmarkBarThreadId] : g_bookmarkBars) {
            if (bookmarkBarThreadId == threadId) {
                bookmarkBars.push_back(bookmarkBar);
            }
        }
    }

    for (void* bookmarkBar : bookmarkBars) {
        g_BookmarkBarViewOnThemeChangedOriginal(bookmarkBar);
    }
}

static void RefreshExistingBookmarkBars() {
    if (!g_folderIconFeatureReady.load(std::memory_order_relaxed)) {
        return;
    }

    std::vector<DWORD> threadIds;

    {
        std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);

        for (const auto& [bookmarkBar, threadId] : g_bookmarkBars) {
            threadIds.push_back(threadId);
        }
    }

    std::sort(threadIds.begin(), threadIds.end());

    threadIds.erase(std::unique(threadIds.begin(), threadIds.end()),
                    threadIds.end());

    for (DWORD threadId : threadIds) {
        HWND hwnd = FindWindowForThread(threadId);

        if (!hwnd) {
            continue;
        }

        RunFromWindowThread(hwnd, RefreshBookmarkBarsOnCurrentThread, nullptr);
    }
}

// -----------------------------------------------------------------------------
// Resolve Edge symbols
// -----------------------------------------------------------------------------

static bool InstallEdgeHooks(HMODULE edgeDll) {
    wchar_t path[32768] = {};

    if (GetModuleFileNameW(edgeDll, path, ARRAYSIZE(path))) {
        Wh_Log(L"Resolving symbols for: %ls", path);
    }

    WindhawkUtils::SYMBOL_HOOK msedgeDllHooks[] = {
        {{LR"(?GetBookmarkFolderIcon@chrome@@YA?AVImageModel@ui@@W4BookmarkFolderIconType@1@VColorVariant@3@@Z)"},
         &g_GetBookmarkFolderIconOriginal,
         GetBookmarkFolderIconHook,
         false},

        {{LR"(?IsMonolineFolderEnabled@bookmarks@@YA_NXZ)"},
         &g_IsMonolineFolderEnabledOriginal,
         IsMonolineFolderEnabledHook,
         false},

        {{LR"(?CreateSkBitmapFromHICON@IconUtil@@SA?AVSkBitmap@@PEAUHICON__@@@Z)"},
         &g_CreateSkBitmapFromHICON,
         nullptr,
         false},

        {{LR"(?CreateFrom1xBitmap@ImageSkia@gfx@@SA?AV12@AEBVSkBitmap@@@Z)"},
         &g_ImageSkiaCreateFrom1xBitmap,
         nullptr,
         false},

        {{LR"(??1SkBitmap@@QEAA@XZ)"}, &g_SkBitmapDtor, nullptr, false},

        {{LR"(??1ImageSkia@gfx@@QEAA@XZ)"}, &g_ImageSkiaDtor, nullptr, false},

        {{LR"(?FromImageSkia@ImageModel@ui@@SA?AV12@AEBVImageSkia@gfx@@@Z)"},
         &g_ImageModelFromImageSkia,
         nullptr,
         false},

        {{LR"(?OnThemeChanged@BookmarkBarView@@UEAAXXZ)"},
         &g_BookmarkBarViewOnThemeChangedOriginal,
         BookmarkBarViewOnThemeChangedHook,
         false},

        {{LR"(??_GBookmarkBarView@@UEAAPEAXI@Z)"},
         &g_BookmarkBarViewDeletingDtorOriginal,
         BookmarkBarViewDeletingDtorHook,
         false},
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {};
    options.optionsSize = sizeof(options);
    options.symbolServer = kEdgeSymbolServer;
    options.noUndecoratedSymbols = TRUE;

    if (!WindhawkUtils::HookSymbols(edgeDll, msedgeDllHooks,
                                    ARRAYSIZE(msedgeDllHooks), &options)) {
        Wh_Log(
            L"Failed to resolve the required Edge Favorites symbols; "
            L"leaving Edge UI unchanged");
        return false;
    }

    g_folderIconFeatureReady.store(true, std::memory_order_release);

    Wh_Log(L"Edge Favorites folder icon symbols resolved");
    return true;
}

// -----------------------------------------------------------------------------
// Slow symbol-resolution notifications
// -----------------------------------------------------------------------------

static void UpdateSymbolTooltip(NOTIFYICONDATAW& notifyIcon,
                                const wchar_t* text) {
    notifyIcon.uFlags = NIF_TIP | NIF_SHOWTIP;
    wcsncpy_s(notifyIcon.szTip, text, _TRUNCATE);

    if (!Shell_NotifyIconW(NIM_MODIFY, &notifyIcon)) {
        Wh_Log(L"Failed to update symbol-resolution tooltip: %lu",
               GetLastError());
    }
}

static void ShowSymbolNotification(NOTIFYICONDATAW& notifyIcon,
                                   const wchar_t* title,
                                   const wchar_t* text,
                                   DWORD infoFlags) {
    notifyIcon.uFlags = NIF_INFO;
    notifyIcon.dwInfoFlags = infoFlags | NIIF_NOSOUND;

    wcsncpy_s(notifyIcon.szInfoTitle, title, _TRUNCATE);

    wcsncpy_s(notifyIcon.szInfo, text, _TRUNCATE);

    if (!Shell_NotifyIconW(NIM_MODIFY, &notifyIcon)) {
        Wh_Log(L"Failed to show symbol-resolution notification: %lu",
               GetLastError());
    }
}

static DWORD WaitForHandlesWithMessageLoop(const HANDLE* handles,
                                           DWORD handleCount) {
    for (;;) {
        DWORD waitResult = MsgWaitForMultipleObjectsEx(
            handleCount, handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        if (waitResult >= WAIT_OBJECT_0 &&
            waitResult < WAIT_OBJECT_0 + handleCount) {
            return waitResult;
        }

        if (waitResult == WAIT_OBJECT_0 + handleCount) {
            MSG message;

            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) {
                    return WAIT_ABANDONED;
                }

                TranslateMessage(&message);
                DispatchMessageW(&message);
            }

            continue;
        }

        return waitResult;
    }
}

static DWORD WINAPI SymbolNotificationThreadProc(void*) {
    HWND window = CreateWindowExW(0, L"STATIC", L"", WS_OVERLAPPED, 0, 0, 0, 0,
                                  nullptr, nullptr, nullptr, nullptr);

    if (!window) {
        Wh_Log(L"Failed to create symbol notification window: %lu",
               GetLastError());
        return 0;
    }

    NOTIFYICONDATAW notifyIcon = {};
    notifyIcon.cbSize = sizeof(notifyIcon);
    notifyIcon.hWnd = window;
    notifyIcon.uID = 1;
    notifyIcon.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    notifyIcon.hIcon = LoadIconW(nullptr, IDI_INFORMATION);

    wcsncpy_s(
        notifyIcon.szTip,
        L"Edge Native UI Tweaks: preparing Edge symbols in the background",
        _TRUNCATE);

    if (!Shell_NotifyIconW(NIM_ADD, &notifyIcon)) {
        Wh_Log(L"Failed to create symbol notification icon: %lu",
               GetLastError());
        DestroyWindow(window);
        return 0;
    }

    notifyIcon.uVersion = NOTIFYICON_VERSION_4;

    Shell_NotifyIconW(NIM_SETVERSION, &notifyIcon);

    if (WaitForSingleObject(g_symbolResolutionDoneEvent, 0) != WAIT_OBJECT_0) {
        ShowSymbolNotification(
            notifyIcon, L"Edge Native UI Tweaks",
            L"Edge symbols are being prepared in the background. Edge started "
            L"without the tweak; you can keep using it.",
            NIIF_INFO);
    }

    HANDLE waitHandles[] = {g_symbolResolutionDoneEvent,
                            g_symbolNotificationStopEvent};

    DWORD waitResult =
        WaitForHandlesWithMessageLoop(waitHandles, ARRAYSIZE(waitHandles));

    if (waitResult == WAIT_OBJECT_0) {
        bool success =
            g_symbolResolutionSucceeded.load(std::memory_order_acquire);

        if (success) {
            UpdateSymbolTooltip(notifyIcon,
                                L"Edge Native UI Tweaks: symbols ready - "
                                L"restart Edge to activate the mod");

            ShowSymbolNotification(notifyIcon, L"Edge Native UI Tweaks",
                                   L"Edge symbol analysis is complete. Restart "
                                   L"Edge to activate the mod. "
                                   L"If closing and reopening Edge doesn't "
                                   L"work, open edge://restart.",
                                   NIIF_INFO);
        } else {
            UpdateSymbolTooltip(notifyIcon,
                                L"Edge Native UI Tweaks: symbol analysis "
                                L"failed - see the Windhawk "
                                L"mod log");

            ShowSymbolNotification(notifyIcon, L"Edge Native UI Tweaks",
                                   L"Edge symbol analysis failed. The mod "
                                   L"wasn't activated; see the "
                                   L"Windhawk mod log.",
                                   NIIF_ERROR);
        }

        HANDLE stopHandles[] = {g_symbolNotificationStopEvent};

        waitResult =
            WaitForHandlesWithMessageLoop(stopHandles, ARRAYSIZE(stopHandles));

        if (waitResult != WAIT_OBJECT_0 && waitResult != WAIT_ABANDONED) {
            Wh_Log(L"Symbol notification stop wait failed: %lu",
                   GetLastError());
        }
    } else if (waitResult != WAIT_OBJECT_0 + 1 &&
               waitResult != WAIT_ABANDONED) {
        Wh_Log(L"Symbol notification wait failed: %lu", GetLastError());
    }

    Shell_NotifyIconW(NIM_DELETE, &notifyIcon);

    DestroyWindow(window);
    return 0;
}

static void StartSymbolNotifications() {
    std::lock_guard<std::mutex> lock(g_workerMutex);

    if (g_unloading || g_symbolNotificationThread) {
        return;
    }

    g_symbolNotificationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_symbolNotificationStopEvent) {
        Wh_Log(L"Failed to create symbol notification stop event: %lu",
               GetLastError());
        return;
    }

    g_symbolNotificationThread = CreateThread(
        nullptr, 0, SymbolNotificationThreadProc, nullptr, 0, nullptr);

    if (!g_symbolNotificationThread) {
        Wh_Log(L"Failed to create symbol notification thread: %lu",
               GetLastError());

        CloseHandle(g_symbolNotificationStopEvent);

        g_symbolNotificationStopEvent = nullptr;
    }
}

// -----------------------------------------------------------------------------
// Bounded startup symbol resolution
// -----------------------------------------------------------------------------

static DWORD WINAPI EdgeSymbolResolutionThreadProc(void* param) {
    HMODULE edgeDll = static_cast<HMODULE>(param);

    ULONGLONG startedAt = GetTickCount64();

    bool success = InstallEdgeHooks(edgeDll);

    if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
        ClearEdgeRuntimeReadiness();
    }

    g_symbolResolutionSucceeded.store(success, std::memory_order_release);

    ULONGLONG elapsed = GetTickCount64() - startedAt;

    Wh_Log(L"Edge symbol preparation finished in %llu ms: %ls", elapsed,
           success ? L"success" : L"FAILED");

    SetEvent(g_symbolResolutionDoneEvent);

    return 0;
}

static void StartEdgeHookSetup(HMODULE edgeDll) {
    {
        std::lock_guard<std::mutex> lock(g_workerMutex);

        if (g_unloading) {
            Wh_Log(L"Skipping Edge hook setup because the mod is unloading");
            return;
        }

        if (g_edgeSetupStarted.exchange(true)) {
            return;
        }

        g_edgeSetupInProgress = true;
    }

    struct SetupCompletionGuard {
        ~SetupCompletionGuard() {
            {
                std::lock_guard<std::mutex> lock(g_workerMutex);

                g_edgeSetupInProgress = false;
            }

            g_workerCondition.notify_all();
        }
    } setupCompletionGuard;

    {
        std::lock_guard<std::mutex> lock(g_workerMutex);

        if (g_unloading) {
            Wh_Log(
                L"Edge hook setup abandoned before starting symbol resolution");
            return;
        }

        g_symbolResolutionDoneEvent =
            CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }

    if (!g_symbolResolutionDoneEvent) {
        Wh_Log(
            L"Failed to create Edge symbol resolution event; continuing Edge "
            L"without the tweak");
        ClearEdgeRuntimeReadiness();
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_workerMutex);

        if (!g_unloading) {
            g_symbolResolutionThread =
                CreateThread(nullptr, 0, EdgeSymbolResolutionThreadProc,
                             edgeDll, 0, nullptr);
        }
    }

    if (!g_symbolResolutionThread) {
        Wh_Log(
            L"Failed to create Edge symbol resolution thread; continuing Edge "
            L"without the tweak");

        {
            std::lock_guard<std::mutex> lock(g_workerMutex);

            CloseHandle(g_symbolResolutionDoneEvent);

            g_symbolResolutionDoneEvent = nullptr;
        }

        ClearEdgeRuntimeReadiness();
        return;
    }

    ULONGLONG waitStartedAt = GetTickCount64();

    DWORD waitResult = WaitForSingleObject(g_symbolResolutionDoneEvent,
                                           kEdgeSymbolStartupWaitMs);

    ULONGLONG waited = GetTickCount64() - waitStartedAt;

    if (waitResult == WAIT_OBJECT_0) {
        bool success =
            g_symbolResolutionSucceeded.load(std::memory_order_acquire);

        Wh_Log(
            L"Edge symbol resolution completed within startup wait (%llu ms)",
            waited);

        if (!success) {
            return;
        }

        if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
            ClearEdgeRuntimeReadiness();

            Wh_Log(
                L"Edge hook activation was abandoned while symbol resolution "
                L"was "
                L"running");
            return;
        }

        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Wh_ApplyHookOperations failed");
            ClearEdgeRuntimeReadiness();
            return;
        }

        g_hooksActivated.store(true, std::memory_order_release);

        Wh_Log(L"Edge hooks activated during startup");

        return;
    }

    if (waitResult == WAIT_TIMEOUT) {
        g_hookActivationAbandoned.store(true, std::memory_order_release);

        ClearEdgeRuntimeReadiness();

        Wh_Log(
            L"Edge symbol startup wait timed out after %llu ms; continuing "
            L"Edge "
            L"without the tweak. After symbol analysis finishes, restart Edge "
            L"to "
            L"activate the mod",
            waited);

        StartSymbolNotifications();
        return;
    }

    g_hookActivationAbandoned.store(true, std::memory_order_release);

    ClearEdgeRuntimeReadiness();

    Wh_Log(L"Edge symbol wait failed: %lu; continuing Edge without the tweak",
           GetLastError());

    StartSymbolNotifications();
}

// -----------------------------------------------------------------------------
// Delayed msedge.dll loading
// -----------------------------------------------------------------------------

using LoadLibraryExWFn = decltype(&LoadLibraryExW);

static LoadLibraryExWFn g_LoadLibraryExWOriginal;

static HMODULE WINAPI LoadLibraryExWHook(LPCWSTR fileName,
                                         HANDLE file,
                                         DWORD flags) {
    HMODULE module = g_LoadLibraryExWOriginal(fileName, file, flags);

    if (!module || g_edgeSetupStarted.load()) {
        return module;
    }

    HMODULE edgeDll = GetModuleHandleW(L"msedge.dll");

    if (edgeDll && edgeDll == module) {
        StartEdgeHookSetup(edgeDll);
    }

    return module;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    int argc = 0;

    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    bool isSubprocess = false;
    bool isRemoteDebuggingPipe = false;

    for (int i = 1; i < argc; i++) {
        if (wcsncmp(argv[i], L"--type=", 7) == 0) {
            isSubprocess = true;
            break;
        }

        if (wcscmp(argv[i], L"--remote-debugging-pipe") == 0) {
            isRemoteDebuggingPipe = true;
        }
    }

    LocalFree(argv);

    if (isSubprocess) {
        return FALSE;
    }

    if (isRemoteDebuggingPipe) {
        Wh_Log(L"Skipping CDP-controlled Edge process");
        return FALSE;
    }

    LoadSettings();

    HMODULE edgeDll = GetModuleHandleW(L"msedge.dll");

    if (edgeDll) {
        return TRUE;
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");

    if (!kernelBase) {
        Wh_Log(L"Failed to get kernelbase.dll");
        return FALSE;
    }

    auto loadLibraryExW = reinterpret_cast<LoadLibraryExWFn>(
        GetProcAddress(kernelBase, "LoadLibraryExW"));

    if (!loadLibraryExW) {
        Wh_Log(L"Failed to get kernelbase!LoadLibraryExW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExWHook,
                                        &g_LoadLibraryExWOriginal)) {
        Wh_Log(L"Failed to hook kernelbase!LoadLibraryExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_edgeSetupStarted.load()) {
        return;
    }

    HMODULE edgeDll = GetModuleHandleW(L"msedge.dll");

    if (!edgeDll) {
        return;
    }

    StartEdgeHookSetup(edgeDll);
}

void Wh_ModSettingsChanged() {
    bool oldUseWindowsFolderIcon =
        g_useWindowsFolderIcon.load(std::memory_order_relaxed);

    LoadSettings();

    if (!g_hooksActivated.load(std::memory_order_acquire)) {
        return;
    }

    if (oldUseWindowsFolderIcon !=
        g_useWindowsFolderIcon.load(std::memory_order_relaxed)) {
        RefreshExistingBookmarkBars();
    }
}

void Wh_ModBeforeUninit() {
    HANDLE symbolNotificationThread = nullptr;
    HANDLE symbolNotificationStopEvent = nullptr;
    HANDLE symbolResolutionThread = nullptr;
    HANDLE symbolResolutionDoneEvent = nullptr;

    {
        std::unique_lock<std::mutex> lock(g_workerMutex);

        g_unloading = true;

        g_hookActivationAbandoned.store(true, std::memory_order_release);

        if (g_symbolNotificationStopEvent) {
            SetEvent(g_symbolNotificationStopEvent);
        }

        g_workerCondition.wait(lock, [] { return !g_edgeSetupInProgress; });

        if (g_symbolNotificationStopEvent) {
            SetEvent(g_symbolNotificationStopEvent);
        }

        symbolNotificationThread = g_symbolNotificationThread;

        symbolNotificationStopEvent = g_symbolNotificationStopEvent;

        symbolResolutionThread = g_symbolResolutionThread;

        symbolResolutionDoneEvent = g_symbolResolutionDoneEvent;
    }

    g_useWindowsFolderIcon.store(false, std::memory_order_relaxed);

    if (symbolNotificationThread) {
        WaitForSingleObject(symbolNotificationThread, INFINITE);
    }

    // HookSymbols can't be cancelled safely. Wait until the worker leaves mod
    // code before Windhawk unloads this DLL.
    if (symbolResolutionThread) {
        WaitForSingleObject(symbolResolutionThread, INFINITE);
    }

    if (symbolNotificationThread) {
        CloseHandle(symbolNotificationThread);
    }

    if (symbolNotificationStopEvent) {
        CloseHandle(symbolNotificationStopEvent);
    }

    if (symbolResolutionThread) {
        CloseHandle(symbolResolutionThread);
    }

    if (symbolResolutionDoneEvent) {
        CloseHandle(symbolResolutionDoneEvent);
    }

    {
        std::lock_guard<std::mutex> lock(g_workerMutex);

        g_symbolNotificationThread = nullptr;
        g_symbolNotificationStopEvent = nullptr;
        g_symbolResolutionThread = nullptr;
        g_symbolResolutionDoneEvent = nullptr;
    }

    if (!g_hooksActivated.load(std::memory_order_acquire)) {
        return;
    }

    RefreshExistingBookmarkBars();
    DestroyWindowsFolderImageOnOwningThread();
}

void Wh_ModUninit() {
    Wh_Log(L"Edge Native UI Tweaks unloaded");
}
