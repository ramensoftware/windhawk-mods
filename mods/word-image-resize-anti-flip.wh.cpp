// ==WindhawkMod==
// @id             word-image-resize-anti-flip
// @name           Word Image Resize Anti-Flip
// @name:zh-CN     Word 图片调整大小时防翻转
// @description    Prevents unintended flipping when dragging handles to resize images in Word.
// @description:zh-CN 在 Microsoft Word 中调整图片大小时防止图片意外翻转
// @version        1.0.0
// @author         Joe Ye
// @github         https://github.com/JoeYe-233
// @include        winword.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Word Image Resize Anti-Flip

This mod fixes the long-standing issue of images being flipped unexpectedly when resizing via dragging the image handles in Word. Before, once the user dragged the resize handles past the anchor point in any direction, the image would flip unexpectedly. The mod works by intercepting the drag update function and dynamically clamping the mouse position parameter to prevent crossing over the anchor point, which is the root cause of the flipping behavior. It also includes an "anti-flip" feature that detects the original drag direction and maintains it even if the user tries to cross over the anchor point, providing a more intuitive resizing experience.

*Note: this mod needs pdb symbol of `oart.dll` to work. The symbol file is expected to be ~35MB in size. Windhawk will download it automatically when you launch Word first time after you installed the mod (the popup at right bottom corner of your screen, please make sure that it shows percentage like "Loading symbols... 0% (oart.dll)", wait until it reaches 100% and the pop up disappear, otherwise please switch your network and try again) please wait patiently and **relaunch Word AS ADMINISTRATOR at least once** after it finishes, this is to write symbols being used to SymbolCache, which speeds up launching later on.*

## ⚠️ Note:
- It is advised to **turn off automatic updates** for Office applications, as PDB may need to be downloaded every time after updates.
- Please relaunch Word as administrator *at least once* after installing the mod and wait for symbol download to complete, this is to write symbols being used to SymbolCache, which speeds up launching later on.

# Before
![Before](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-image-resize-anti-flip-before.gif)

# After
![After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-image-resize-anti-flip-after.gif)

(Image courtesy of [Ralf1403](https://pixabay.com/photos/love-valentines-day-dice-game-9344644/) from Pixabay)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <winternl.h>

std::atomic<bool> g_bMsoLoaded{false};

// -------------------------------------------------------------------------
// Architecture calling conventions and macros
// -------------------------------------------------------------------------
#ifdef _WIN64
    #define WH_CALLCONV
    #define THISCALL_DUMMY_DECL
    #define THISCALL_DUMMY_CALL
    #define SYM_UpdateDragElement L"?UpdateDragElement@DragElementTracker@Art@@AEAAXAEAUDragInfo@2@KAEBVPoint64@2@AEBVRect64@2@AEAV42@@Z"
    #define SYM_GetOppositeHandle L"?GetDragElementOriginalOppositeHandlePosition@DragInfo@Art@@QEBA?BVPoint64@2@K@Z"
#else
    #define WH_CALLCONV __fastcall
    #define THISCALL_DUMMY_DECL , void* pDummyEdx
    #define THISCALL_DUMMY_CALL , pDummyEdx
    #define SYM_UpdateDragElement L"?UpdateDragElement@DragElementTracker@Art@@AAEXAAUDragInfo@2@KABVPoint64@2@ABVRect64@2@AAV42@@Z"
    #define SYM_GetOppositeHandle L"?GetDragElementOriginalOppositeHandlePosition@DragInfo@Art@@QBE?BVPoint64@2@K@Z"
#endif

// -------------------------------------------------------------------------
// Data structures
// -------------------------------------------------------------------------
struct Point64 {
    int64_t x;
    int64_t y;
};

struct Rect64 {
    int64_t left;
    int64_t top;
    int64_t right;
    int64_t bottom;
};

const double EMUS_PER_CM = 360000.0;
// Add a safety margin constant: 36000 EMU is approximately 1mm (~4px)
const int64_t SAFE_MARGIN_EMU = 36000;
// -------------------------------------------------------------------------
// Function pointer definitions
// -------------------------------------------------------------------------
typedef void (WH_CALLCONV *UpdateDragElement_t)(
    void* pThis THISCALL_DUMMY_DECL, void* pDragInfo, unsigned int elementIndex,
    const Point64* pNewMousePos, const Rect64* pBounds, Point64* pOutOffset
);
UpdateDragElement_t pOrig_UpdateDragElement = nullptr;

// Define the GetOppositeHandle function pointer.
// Note: because the return value is a 16-byte struct, MSVC __thiscall passes
// the return-value buffer pointer as a hidden argument.
typedef void (WH_CALLCONV *GetOppositeHandle_t)(
    void* pThisDragInfo THISCALL_DUMMY_DECL,
    Point64* pOutResult, // C++ ABI hidden return-value pointer
    unsigned int elementIndex
);
GetOppositeHandle_t pOrig_GetOppositeHandle = nullptr;

// Use thread_local to prevent race conditions caused by state overwrites across multiple document windows/threads.
thread_local void *g_lastDragInfo = nullptr;
thread_local unsigned int g_lastElementIndex = 0xFFFFFFFF;
thread_local Point64 g_lastAnchorPos = {-1, -1};
thread_local int g_expectedSignX = 0;
thread_local int g_expectedSignY = 0;
thread_local void *g_lastTrackerThis = nullptr;
thread_local bool g_isTrackerValid = false;

// Offset definition: from reversing the CalculateDragElements function, there is
// a switch condition (or a switch-like if/else chain) whose selector is trackerMode.
/*Example on 32-bit:
    switch ( *(_DWORD *)(v19 + 640) ) <---- 640 is the offset in 32-bit Word,
  {
    case 0:
    case 0xC:
      .....
      goto LABEL_13;
    case 1:
    case 5:
    case 6:
      goto LABEL_13;
    case 2:
    case 3:
===============================================
  On 64-bit:
  v15 = *((_DWORD *)this + 178); <---- On 64-bit Word we should use 712 = 178 * 4
  if ( v15 > 6 )
  {
    v33 = v15 - 7;
    if ( !v33 )
      goto LABEL_10;
    v34 = v33 - 1;
    if ( !v34 )
      goto LABEL_10;
    v35 = v34 - 1;
    if ( !v35 )
      goto LABEL_10;
    v36 = v35 - 1;
    if ( !v36 )
      goto LABEL_10;
    v37 = v36 - 1;
    if ( !v37 )
      goto LABEL_10;
    if ( v37 != 1 )
      goto LABEL_28;
    */
#ifdef _WIN64
    // Offset for 64-bit Word: 178 * sizeof(DWORD) = 712
    const int TRACKER_OFFSET = 712;
#else
    // Offset for 32-bit Word: 640
    const int TRACKER_OFFSET = 640;
#endif

void WH_CALLCONV Hook_UpdateDragElement(
    void* pThis THISCALL_DUMMY_DECL, void* pDragInfo, unsigned int elementIndex,
    const Point64* pNewMousePos, const Rect64* pBounds, Point64* pOutOffset
) {
    if (!pNewMousePos || !pThis || !pDragInfo) {
        pOrig_UpdateDragElement(pThis THISCALL_DUMMY_CALL, pDragInfo, elementIndex, pNewMousePos, pBounds, pOutOffset);
        return;
    }

    uint32_t trackerMode = 0;
    void *targetAddress = (char *)pThis + TRACKER_OFFSET;

    // [Robust fix]: Cache memory validation results.
    // Perform the expensive VirtualQuery system call only when encountering a new Tracker object.
    // During the same drag operation (where rapid mouse movement may trigger hundreds or thousands of calls),
    // the overhead is only a pointer comparison.
    if (pThis != g_lastTrackerThis)
    {
        g_lastTrackerThis = pThis;
        MEMORY_BASIC_INFORMATION mbi;
        g_isTrackerValid = (VirtualQuery(targetAddress, &mbi, sizeof(mbi)) &&
                            (mbi.State == MEM_COMMIT) &&
                            !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)));
    }

    // If the memory was validated as safe previously, read directly via raw pointer for maximum speed and safety.
    if (g_isTrackerValid)
    {
        trackerMode = *(uint32_t *)targetAddress;
    }

    if ((trackerMode == 2 || trackerMode == 3) && pOrig_GetOppositeHandle) {

        Point64 anchorPos = {0};
        pOrig_GetOppositeHandle(pDragInfo THISCALL_DUMMY_CALL, &anchorPos, elementIndex);

        // [Cache Invalidation]: If this is a new drag action (DragInfo address changed, handle changed, or anchor moved)
        if (pDragInfo != g_lastDragInfo ||
            elementIndex != g_lastElementIndex ||
            anchorPos.x != g_lastAnchorPos.x ||
            anchorPos.y != g_lastAnchorPos.y)
        {

            g_lastDragInfo = pDragInfo;
            g_lastElementIndex = elementIndex;
            g_lastAnchorPos = anchorPos;

            // Record original direction: 1 means handle should be to the right/below the anchor,
            // -1 means handle should be to the left/above the anchor
            g_expectedSignX = (pNewMousePos->x >= anchorPos.x) ? 1 : -1;
            g_expectedSignY = (pNewMousePos->y >= anchorPos.y) ? 1 : -1;

            // Wh_Log(L"[*] New drag session detected: Handle=%u, Expected Direction(X:%d, Y:%d)", elementIndex, g_expectedSignX, g_expectedSignY);
        }

        bool bSpoofed = false;
        Point64 fakeMousePos = *pNewMousePos;

        // ================= Dynamic clamping on X axis =================
        if (g_expectedSignX == 1 && pNewMousePos->x < (anchorPos.x + SAFE_MARGIN_EMU)) {
            fakeMousePos.x = anchorPos.x + SAFE_MARGIN_EMU;
            bSpoofed = true;
        } else if (g_expectedSignX == -1 && pNewMousePos->x > (anchorPos.x - SAFE_MARGIN_EMU)) {
            fakeMousePos.x = anchorPos.x - SAFE_MARGIN_EMU;
            bSpoofed = true;
        }

        // ================= Dynamic clamping on Y axis =================
        if (g_expectedSignY == 1 && pNewMousePos->y < (anchorPos.y + SAFE_MARGIN_EMU)) {
            fakeMousePos.y = anchorPos.y + SAFE_MARGIN_EMU;
            bSpoofed = true;
        } else if (g_expectedSignY == -1 && pNewMousePos->y > (anchorPos.y - SAFE_MARGIN_EMU)) {
            fakeMousePos.y = anchorPos.y - SAFE_MARGIN_EMU;
            bSpoofed = true;
        }

        if (bSpoofed) {
            // Wh_Log(L"[!] Bidirectional interception triggered: original X=%.2f, original Y=%.2f -> locked to X=%.2f, Y=%.2f",
            //        (double)pNewMousePos->x / EMUS_PER_CM, (double)pNewMousePos->y / EMUS_PER_CM,
            //        (double)fakeMousePos.x / EMUS_PER_CM, (double)fakeMousePos.y / EMUS_PER_CM);

            pOrig_UpdateDragElement(pThis THISCALL_DUMMY_CALL, pDragInfo, elementIndex, &fakeMousePos, pBounds, pOutOffset);
            return;
        }
    }

    pOrig_UpdateDragElement(pThis THISCALL_DUMMY_CALL, pDragInfo, elementIndex, pNewMousePos, pBounds, pOutOffset);
}

// -------------------------------------------------------------------------
// Module loading and hook injection
// -------------------------------------------------------------------------
void ScanAndHookOart() {
    // Check state before getting the handle; lock g_bMsoLoaded only after hook succeeds.
    if (g_bMsoLoaded.load()) return; 
    HMODULE hMso = GetModuleHandleW(L"oart.dll");
    if (!hMso) return;
    
    //oart.dll
    WindhawkUtils::SYMBOL_HOOK oartHook[] = {
        {
            { SYM_UpdateDragElement },
            (void**)&pOrig_UpdateDragElement,
            (void*)Hook_UpdateDragElement,
            false
        },
        {
            { SYM_GetOppositeHandle },
            (void**)&pOrig_GetOppositeHandle,
            nullptr, // We only obtain the function pointer; do not hook it
            false
        }
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {0};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    options.onlineCacheUrl = L"";

    if (WindhawkUtils::HookSymbols(hMso, oartHook, ARRAYSIZE(oartHook), &options)) {
        g_bMsoLoaded.store(true); // Hook succeeded; lock the state in a closed loop.
        Wh_ApplyHookOperations();
        if (pOrig_GetOppositeHandle) {
            Wh_Log(L"[+] Successfully resolved GetOppositeHandle; anti-flip feature is ready.");
        } else {
            Wh_Log(L"[-] Warning: failed to resolve GetOppositeHandle; anti-flip feature is unavailable.");
        }
    }
}

// =============================================================
// LdrRegisterDllNotification (OS-level module-load subscription)
// =============================================================

#define LDR_DLL_NOTIFICATION_REASON_LOADED 1

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
    ULONG Flags;
    PCUNICODE_STRING FullDllName;
    PCUNICODE_STRING BaseDllName;
    PVOID DllBase;
    ULONG SizeOfImage;
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
    LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
    struct {
        ULONG Flags;
        PCUNICODE_STRING FullDllName;
        PCUNICODE_STRING BaseDllName;
        PVOID DllBase;
        ULONG SizeOfImage;
    } Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef VOID (NTAPI *PLDR_DLL_NOTIFICATION_FUNCTION)(
    ULONG NotificationReason,
    PLDR_DLL_NOTIFICATION_DATA NotificationData,
    PVOID Context
);

typedef NTSTATUS (NTAPI *LdrRegisterDllNotification_t)(
    ULONG Flags,
    PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
    PVOID Context,
    PVOID *Cookie
);

typedef NTSTATUS (NTAPI *LdrUnregisterDllNotification_t)(
    PVOID Cookie
);

void* g_DllNotificationCookie = nullptr;
LdrUnregisterDllNotification_t pLdrUnregisterDllNotification = nullptr;

// Use an atomic flag to prevent concurrent creation of multiple threads.
std::atomic<bool> g_bThreadSpawned{false};

// =============================================================
// LdrRegisterDllNotification Callback and Helper Thread
// =============================================================

DWORD WINAPI DelayedHookThread(LPVOID lpParam) {
    Sleep(500); // Minimal delay to escape Loader Lock.
    ScanAndHookOart();
    g_bThreadSpawned.store(false); // Reset state after completion to allow subsequent retries.
    return 0;
}

VOID NTAPI DllNotificationCallback(ULONG Reason, PLDR_DLL_NOTIFICATION_DATA Data, PVOID Context) {
    if (Reason == LDR_DLL_NOTIFICATION_REASON_LOADED) {
        if (Data->Loaded.BaseDllName && Data->Loaded.BaseDllName->Buffer) {
            
            // Strict and safe exact string matching to avoid false positives (e.g., foart.dll or oart.dll.mui).
            const WCHAR* target = L"oart.dll";
            size_t targetLen = 8; // Hardcoded length.
            size_t baseLen = Data->Loaded.BaseDllName->Length / sizeof(WCHAR);
            
            if (baseLen == targetLen && _wcsnicmp(Data->Loaded.BaseDllName->Buffer, target, targetLen) == 0) {
                Wh_Log(L"[!] Ultimate subscription: OS-level capture detected oart.dll registered in the PEB.");

                // Use atomic compare-and-swap to prevent concurrent creation of multiple probe threads in multithreaded scenarios.
                bool expected = false;
                if (g_bThreadSpawned.compare_exchange_strong(expected, true)) {
                    HANDLE hThread = CreateThread(nullptr, 0, DelayedHookThread, nullptr, 0, nullptr);
                    if (hThread) {
                        CloseHandle(hThread); // Preferred minimal strategy: fire-and-forget.
                    } else {
                        g_bThreadSpawned.store(false); // Roll back if thread creation fails.
                    }
                }
            }
        }
    }
}

// =============================================================
// Windhawk Mod Lifecycle Functions
// =============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Word Drag Element Spoof Loaded. Initializing...");

    // Reset state.
    g_bMsoLoaded.store(false);
    g_bThreadSpawned.store(false);
    // 1. If oart.dll is already in memory (e.g., when recompiling/reapplying the mod), hook immediately
    if (GetModuleHandleW(L"oart.dll")) {
        ScanAndHookOart();
    } else {
        // 2. Register native DLL load notifications
        HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
        if (hNtdll) {
            auto pLdrRegisterDllNotification = (LdrRegisterDllNotification_t)GetProcAddress(hNtdll, "LdrRegisterDllNotification");
            pLdrUnregisterDllNotification = (LdrUnregisterDllNotification_t)GetProcAddress(hNtdll, "LdrUnregisterDllNotification");

            if (pLdrRegisterDllNotification) {
                NTSTATUS status = pLdrRegisterDllNotification(0, DllNotificationCallback, nullptr, &g_DllNotificationCookie);
                if (status >= 0) {
                    Wh_Log(L"[*] Module-level listener registered successfully; silently waiting for oart.dll to load...");
                }
            }
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    // Always unregister the callback to avoid Word crashes after the mod is unloaded
    if (g_DllNotificationCookie && pLdrUnregisterDllNotification) {
        pLdrUnregisterDllNotification(g_DllNotificationCookie);
        g_DllNotificationCookie = nullptr;
    }
    Wh_Log(L"Mod Unloaded.");
}