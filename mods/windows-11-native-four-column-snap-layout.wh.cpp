// ==WindhawkMod==
// @id           windows-11-native-four-column-snap-layout
// @name         Windows 11 Native Four Column Snap Layout
// @description  Adds an extra native Windows 11 Snap Layout with four equal vertical columns
// @version      1.0
// @author       Hoffelhas
// @github       https://github.com/Hoffelhas
// @license      MIT
// @include      explorer.exe
// @architecture x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Native Four Column Snap Layout

Adds an extra native Windows 11 Snap Layout with four equal vertical columns:

![Windows 11 four-column Snap Layout](https://i.imgur.com/a3sGZSk.png)

## What it does

- Adds the four-column layout as an additional option without replacing the
  existing Windows layouts.
- Appears in **Win+Z**, **maximize-button hover**, and the **Snap Bar** shown
  when dragging a window to the top of the screen.
- Integrates with Windows' native Snap Layout model, so Snap Assist and Snap
  Groups continue to work normally.

## Compatibility

This mod relies on undocumented Windows internals and private symbols from
`SnapLayout.dll`. Windows updates can change these implementation details.

The mod searches the layouts provided by Windows for a compatible native
four-zone quadrant layout before creating the additional layout. If a compatible
source layout isn't found, that layout set is left unchanged.

Developed and tested on Windows 11 build **26200.9445** on x86-64.

Currently tested only on x86-64 Windows. ARM64 compatibility has not been
verified.

If a Windows update changes the relevant private symbols or internal structure,
Windhawk may be unable to load the mod until it is updated.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>

#include <atomic>
#include <cstring>

namespace {

constexpr SIZE_T kSnapLayoutSize = 0x50;
constexpr SIZE_T kSnapZoneSize = 0x38;

// Sanity limits for data obtained through undocumented internal structures.
constexpr SIZE_T kMaxReasonableLayoutCount = 32;
constexpr SIZE_T kMaxReasonableZoneCount = 16;

// Adding one more item can create an additional row in the Win+Z/maximize
// picker. 80 logical pixels is the row-height increase observed on the tested
// Windows build.
constexpr int kExtraPickerHeight = 80;
constexpr int kMinReasonablePickerHeight = 160;
constexpr int kMaxReasonablePickerHeight = 400;

struct RawVector {
    void* first;
    void* last;
    void* end;
};

// SnapBarViewModel asks SnapModel::Layouts() more than once during one load.
// These flags identify that path and prevent the additional layout from being
// inserted more than once into the Snap Bar load operation.
thread_local bool g_inSnapBarLoad = false;
thread_local bool g_snapBarCustomAdded = false;

// Updated for every normal flyout Layouts() call. PickerHeight_Hook uses this
// to increase the picker height only when the custom layout was actually added.
thread_local bool g_flyoutCustomAdded = false;

// 0 = SnapLayout.dll hooks not registered.
// 1 = registration is currently in progress or has completed.
//
// A single flag prevents simultaneous LoadLibraryExW calls from trying to
// resolve the same private symbols at the same time. It's reset if symbol
// resolution fails, allowing a later load notification to retry.
std::atomic<bool> g_snapLayoutHookClaimed{false};

// -----------------------------------------------------------------------------
// Native SnapLayout.dll functions
// -----------------------------------------------------------------------------

using Layouts_t =
    RawVector*(__cdecl*)(void* thisPtr, RawVector* returnBuffer);
Layouts_t Layouts_Original = nullptr;

using EmplaceLayout_t =
    void*(__cdecl*)(void* vectorThis, const void* sourceLayout);
EmplaceLayout_t EmplaceLayout_Original = nullptr;

using PickerHeight_t = int(__cdecl*)(void* thisPtr, int* value);
PickerHeight_t PickerHeight_Original = nullptr;

using SnapBarLoadLayouts_t =
    void(__cdecl*)(void* thisPtr, double scale, int options, bool flag);
SnapBarLoadLayouts_t SnapBarLoadLayouts_Original = nullptr;

// -----------------------------------------------------------------------------
// kernelbase!LoadLibraryExW
// -----------------------------------------------------------------------------

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

// Forward declaration.
bool HookSnapLayoutDll(HMODULE module, bool applyHookOperations);

// -----------------------------------------------------------------------------
// Raw SnapLayout / SnapZone helpers
// -----------------------------------------------------------------------------

SIZE_T GetVectorCount(const RawVector* vec, SIZE_T elementSize) {
    if (!vec || !vec->first || !vec->last || !vec->end ||
        elementSize == 0) {
        return 0;
    }

    const auto* first = reinterpret_cast<const BYTE*>(vec->first);
    const auto* last = reinterpret_cast<const BYTE*>(vec->last);
    const auto* end = reinterpret_cast<const BYTE*>(vec->end);

    if (last < first || end < last) {
        return 0;
    }

    const SIZE_T bytes = static_cast<SIZE_T>(last - first);

    if (bytes % elementSize != 0) {
        return 0;
    }

    return bytes / elementSize;
}

bool GetZoneVector(const void* layout,
                   BYTE** firstOut,
                   BYTE** lastOut,
                   SIZE_T* countOut) {
    if (!layout || !firstOut || !lastOut || !countOut) {
        return false;
    }

    void* first = nullptr;
    void* last = nullptr;
    void* end = nullptr;

    const auto* layoutBytes = reinterpret_cast<const BYTE*>(layout);

    memcpy(&first, layoutBytes + 0x28, sizeof(first));
    memcpy(&last, layoutBytes + 0x30, sizeof(last));
    memcpy(&end, layoutBytes + 0x38, sizeof(end));

    if (!first || !last || !end) {
        return false;
    }

    auto* firstBytes = reinterpret_cast<BYTE*>(first);
    auto* lastBytes = reinterpret_cast<BYTE*>(last);
    auto* endBytes = reinterpret_cast<BYTE*>(end);

    if (lastBytes < firstBytes || endBytes < lastBytes) {
        return false;
    }

    const SIZE_T bytes = static_cast<SIZE_T>(lastBytes - firstBytes);

    if (bytes % kSnapZoneSize != 0) {
        return false;
    }

    const SIZE_T count = bytes / kSnapZoneSize;

    if (count > kMaxReasonableZoneCount) {
        return false;
    }

    *firstOut = firstBytes;
    *lastOut = lastBytes;
    *countOut = count;

    return true;
}

void WriteU32(void* base, SIZE_T offset, unsigned int value) {
    memcpy(reinterpret_cast<BYTE*>(base) + offset, &value, sizeof(value));
}

bool ReadU32(const void* base, SIZE_T offset, unsigned int* value) {
    if (!base || !value) {
        return false;
    }

    memcpy(value, reinterpret_cast<const BYTE*>(base) + offset,
           sizeof(*value));

    return true;
}

// Verify that the candidate isn't merely "2x2 with four zones", but actually
// represents the normal four-quadrant Windows layout:
//
// +---+---+
// | 0 | 1 |
// +---+---+
// | 2 | 3 |
// +---+---+
//
// Zone ordering itself doesn't matter.
bool IsNativeQuadrantLayout(const void* layout) {
    if (!layout) {
        return false;
    }

    unsigned int columns = 0;
    unsigned int rows = 0;

    if (!ReadU32(layout, 0x20, &columns) ||
        !ReadU32(layout, 0x24, &rows) || columns != 2 || rows != 2) {
        return false;
    }

    BYTE* zoneFirst = nullptr;
    BYTE* zoneLast = nullptr;
    SIZE_T zoneCount = 0;

    if (!GetZoneVector(layout, &zoneFirst, &zoneLast, &zoneCount) ||
        zoneCount != 4) {
        return false;
    }

    bool found[2][2] = {};

    for (SIZE_T i = 0; i < zoneCount; i++) {
        const BYTE* zone = zoneFirst + i * kSnapZoneSize;

        unsigned int originColumn = 0;
        unsigned int originRow = 0;
        unsigned int columnSpan = 0;
        unsigned int rowSpan = 0;

        if (!ReadU32(zone, 0x20, &originColumn) ||
            !ReadU32(zone, 0x24, &originRow) ||
            !ReadU32(zone, 0x28, &columnSpan) ||
            !ReadU32(zone, 0x2C, &rowSpan)) {
            return false;
        }

        if (originColumn >= 2 || originRow >= 2 || columnSpan != 1 ||
            rowSpan != 1 || found[originRow][originColumn]) {
            return false;
        }

        found[originRow][originColumn] = true;
    }

    return found[0][0] && found[0][1] && found[1][0] && found[1][1];
}

// -----------------------------------------------------------------------------
// Convert a copied quadrant layout to four equal vertical columns
// -----------------------------------------------------------------------------

bool PatchToFourColumns(void* layout) {
    if (!IsNativeQuadrantLayout(layout)) {
        Wh_Log(L"Copied source layout no longer matches the expected "
               L"2x2 four-quadrant structure");
        return false;
    }

    BYTE* zoneFirst = nullptr;
    BYTE* zoneLast = nullptr;
    SIZE_T zoneCount = 0;

    if (!GetZoneVector(layout, &zoneFirst, &zoneLast, &zoneCount) ||
        zoneCount != 4) {
        Wh_Log(L"Unexpected zone vector while patching custom layout");
        return false;
    }

    // SnapLayout:
    // +0x20 = column count
    // +0x24 = row count
    WriteU32(layout, 0x20, 4);
    WriteU32(layout, 0x24, 1);

    // SnapZone:
    // +0x20 OriginColumn
    // +0x24 OriginRow
    // +0x28 ColumnSpan
    // +0x2C RowSpan
    // +0x30 GridUnitType -- preserved from the native source layout.
    for (unsigned int i = 0; i < 4; i++) {
        BYTE* zone = zoneFirst + i * kSnapZoneSize;

        WriteU32(zone, 0x20, i);
        WriteU32(zone, 0x24, 0);
        WriteU32(zone, 0x28, 1);
        WriteU32(zone, 0x2C, 1);
    }

    return true;
}

// -----------------------------------------------------------------------------
// Find, deep-copy, and append the native quadrant layout
// -----------------------------------------------------------------------------

bool CloneAndAppendFourColumn(RawVector* vec) {
    if (!vec || !EmplaceLayout_Original) {
        Wh_Log(L"Can't append custom layout: invalid vector or unresolved "
               L"emplace helper");
        return false;
    }

    const SIZE_T count = GetVectorCount(vec, kSnapLayoutSize);

    if (count == 0 || count > kMaxReasonableLayoutCount) {
        Wh_Log(L"Can't inspect Snap Layout set: invalid layout count %zu",
               count);
        return false;
    }

    const BYTE* vectorFirst =
        reinterpret_cast<const BYTE*>(vec->first);

    SIZE_T sourceIndex = count;

    for (SIZE_T i = 0; i < count; i++) {
        const BYTE* candidate = vectorFirst + i * kSnapLayoutSize;

        if (IsNativeQuadrantLayout(candidate)) {
            sourceIndex = i;
            break;
        }
    }

    if (sourceIndex == count) {
        Wh_Log(L"No compatible 2x2 four-quadrant source layout among %zu "
               L"native layouts",
               count);
        return false;
    }

    // Save the source pointer only until insertion. The insertion operation can
    // reallocate the vector, so neither this pointer nor vec->first may be used
    // afterwards to refer to the old storage.
    const void* source =
        vectorFirst + sourceIndex * kSnapLayoutSize;

    // Use SnapLayout.dll's own std::vector insertion implementation.
    //
    // SnapLayout owns non-trivial members including std::wstring and
    // std::vector<SnapZone>, so copying its bytes directly would duplicate
    // ownership pointers and be unsafe.
    void* newLayout = EmplaceLayout_Original(vec, source);

    if (!newLayout) {
        Wh_Log(L"Failed to clone native Snap Layout");
        return false;
    }

    if (!PatchToFourColumns(newLayout)) {
        // This should be unreachable after the source-layout validation and
        // native deep copy above. If Windows changes the copy semantics, log it
        // rather than modifying any other object.
        Wh_Log(L"Native Snap Layout clone couldn't be converted");
        return false;
    }

    Wh_Log(L"Added four-column Snap Layout using native source index %zu",
           sourceIndex);

    return true;
}

// -----------------------------------------------------------------------------
// SnapLayout.dll hooks
// -----------------------------------------------------------------------------

RawVector* __cdecl Layouts_Hook(void* thisPtr, RawVector* returnBuffer) {
    RawVector* result = Layouts_Original(thisPtr, returnBuffer);

    if (!returnBuffer) {
        return result;
    }

    if (g_inSnapBarLoad) {
        // SnapBarViewModel requests a fresh Layouts() vector more than once
        // during a single load operation. Modifying only the first one is the
        // behavior verified on the tested Windows build.
        if (!g_snapBarCustomAdded) {
            g_snapBarCustomAdded =
                CloneAndAppendFourColumn(returnBuffer);
        }

        return result;
    }

    // Don't latch this state permanently. If a later Layouts() call returns a
    // layout set we can't safely extend, PickerHeight_Hook must not enlarge its
    // picker.
    g_flyoutCustomAdded =
        CloneAndAppendFourColumn(returnBuffer);

    return result;
}

int __cdecl PickerHeight_Hook(void* thisPtr, int* value) {
    const int hr = PickerHeight_Original(thisPtr, value);

    // Adding one item can add one further row to the picker. Avoid pinning this
    // to the exact 244-pixel value observed on one build, but retain a sanity
    // range so an unexpected ABI/semantic change isn't blindly modified.
    if (hr == 0 && value && g_flyoutCustomAdded &&
        *value >= kMinReasonablePickerHeight &&
        *value <= kMaxReasonablePickerHeight) {
        *value += kExtraPickerHeight;
    }

    return hr;
}

void __cdecl SnapBarLoadLayouts_Hook(void* thisPtr,
                                     double scale,
                                     int options,
                                     bool flag) {
    const bool previousInSnapBarLoad = g_inSnapBarLoad;
    const bool previousCustomAdded = g_snapBarCustomAdded;

    g_inSnapBarLoad = true;
    g_snapBarCustomAdded = false;

    SnapBarLoadLayouts_Original(thisPtr, scale, options, flag);

    g_inSnapBarLoad = previousInSnapBarLoad;
    g_snapBarCustomAdded = previousCustomAdded;
}

// -----------------------------------------------------------------------------
// Late loading of SnapLayout.dll
// -----------------------------------------------------------------------------

bool HookSnapLayoutDll(HMODULE module, bool applyHookOperations) {
    if (!module) {
        return false;
    }

    bool expected = false;

    if (!g_snapLayoutHookClaimed.compare_exchange_strong(expected, true)) {
        // Another call is already resolving the symbols, or they were resolved
        // successfully earlier.
        return true;
    }

    WindhawkUtils::SYMBOL_HOOK snapLayoutDllHooks[] = {
        {
            {
                LR"(public: class std::vector<struct SnapLayout,class std::allocator<struct SnapLayout> > __cdecl SnapModel::Layouts(void)const )",
            },
            &Layouts_Original,
            Layouts_Hook,
        },
        {
            {
                LR"(private: struct SnapLayout & __cdecl std::vector<struct SnapLayout,class std::allocator<struct SnapLayout> >::_Emplace_one_at_back<struct SnapLayout const &>(struct SnapLayout const &))",
            },
            &EmplaceLayout_Original,
            nullptr,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SnapLayout::implementation::SnapLayoutPickerViewModel,struct winrt::SnapLayout::ISnapLayoutPickerViewModel>::get_PickerHeight(int *))",
            },
            &PickerHeight_Original,
            PickerHeight_Hook,
        },
        {
            {
                LR"(public: void __cdecl winrt::SnapLayout::implementation::SnapBarViewModel::LoadLayouts(double,enum winrt::SnapLayout::SnapModelOptions,bool))",
            },
            &SnapBarLoadLayouts_Original,
            SnapBarLoadLayouts_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            snapLayoutDllHooks,
            ARRAYSIZE(snapLayoutDllHooks))) {
        Wh_Log(L"Failed to resolve one or more SnapLayout.dll symbols");

        // Permit a later attempt if this was a transient failure.
        g_snapLayoutHookClaimed = false;
        return false;
    }

    Wh_Log(L"SnapLayout.dll symbols resolved successfully");

    // Hooks registered after Wh_ModInit() aren't applied automatically until
    // requested. This path is used when SnapLayout.dll loads naturally later
    // in Explorer's lifetime.
    if (applyHookOperations && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply SnapLayout.dll hook operations");
        return false;
    }

    return true;
}

void HandleSnapLayoutDllIfLoaded(bool applyHookOperations) {
    if (g_snapLayoutHookClaimed.load()) {
        return;
    }

    HMODULE module = GetModuleHandleW(L"SnapLayout.dll");

    if (module) {
        HookSnapLayoutDll(module, applyHookOperations);
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module =
        LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && !g_snapLayoutHookClaimed.load()) {
        // Check the module handle rather than matching lpLibFileName. This also
        // catches SnapLayout.dll arriving as a dependency of another DLL.
        HandleSnapLayoutDllIfLoaded(true);
    }

    return module;
}

}  // namespace

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    // If SnapLayout.dll is already present, register its hooks now. Windhawk
    // will apply them automatically after Wh_ModInit returns.
    if (HMODULE snapLayoutModule = GetModuleHandleW(L"SnapLayout.dll")) {
        if (!HookSnapLayoutDll(snapLayoutModule, false)) {
            return FALSE;
        }

        return TRUE;
    }

    // SnapLayout.dll hasn't loaded yet. Intercept kernelbase!LoadLibraryExW so
    // it can be hooked when Windows naturally loads the Snap UI component.
    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");

    if (!kernelBaseModule) {
        Wh_Log(L"Failed to get kernelbase.dll");
        return FALSE;
    }

    auto pKernelBaseLoadLibraryExW =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBaseModule, "LoadLibraryExW"));

    if (!pKernelBaseLoadLibraryExW) {
        Wh_Log(L"Failed to resolve kernelbase!LoadLibraryExW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            pKernelBaseLoadLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original)) {
        Wh_Log(L"Failed to hook kernelbase!LoadLibraryExW");
        return FALSE;
    }

    Wh_Log(L"SnapLayout.dll isn't loaded yet; waiting for Windows to load it");

    return TRUE;
}

void Wh_ModAfterInit() {
    // Close the small race where SnapLayout.dll loads after the initial module
    // check but before the LoadLibraryExW hook becomes active. It also catches
    // cases where it was introduced as a static dependency.
    HandleSnapLayoutDllIfLoaded(true);
}

void Wh_ModUninit() {
}
