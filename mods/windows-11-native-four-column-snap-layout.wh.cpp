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

Add an extra native Windows 11 Snap Layout with four equal vertical columns. This is especially useful on ultrawide monitors, where four equal vertical zones make better use of the available horizontal screen space.

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

The mod is currently tested only on x86-64 Windows. Because Windhawk's
`x86-64` architecture setting also allows loading into predefined native shell
processes on ARM64 Windows, ARM64 may attempt to load the mod, but compatibility
there has not been verified.

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

constexpr SIZE_T kMaxReasonableLayoutCount = 32;
constexpr SIZE_T kMaxReasonableZoneCount = 16;

// The native Win+Z/maximize-button picker displays two layout previews per row
// on the Windows implementation observed during development.
constexpr SIZE_T kItemsPerRow = 2;

struct RawVector {
    void* first;
    void* last;
    void* end;
};

// SnapBarViewModel asks SnapModel::Layouts() more than once during one load.
// Keep this state thread-local because it describes one synchronous Snap Bar
// load operation on the current thread.
thread_local bool g_inSnapBarLoad = false;
thread_local bool g_snapBarCustomAdded = false;

// The picker height is requested after its Layouts() call on the same thread
// on the Windows implementation observed during development.
thread_local bool g_flyoutCustomAdded = false;
thread_local SIZE_T g_flyoutLayoutsBefore = 0;
thread_local SIZE_T g_flyoutLayoutsAfter = 0;

// Prevent duplicate SnapLayout.dll symbol-hook registration if multiple DLL
// loads occur concurrently.
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

bool HookSnapLayoutDll(HMODULE module, bool applyHookOperations);

// -----------------------------------------------------------------------------
// Scope guard for Snap Bar thread-local state
// -----------------------------------------------------------------------------

class ScopedSnapBarState {
  public:
    ScopedSnapBarState()
        : previousInSnapBarLoad_(g_inSnapBarLoad),
          previousCustomAdded_(g_snapBarCustomAdded) {
        g_inSnapBarLoad = true;
        g_snapBarCustomAdded = false;
    }

    ~ScopedSnapBarState() {
        g_inSnapBarLoad = previousInSnapBarLoad_;
        g_snapBarCustomAdded = previousCustomAdded_;
    }

    ScopedSnapBarState(const ScopedSnapBarState&) = delete;
    ScopedSnapBarState& operator=(const ScopedSnapBarState&) = delete;

  private:
    bool previousInSnapBarLoad_;
    bool previousCustomAdded_;
};

// -----------------------------------------------------------------------------
// Raw SnapLayout / SnapZone helpers
// -----------------------------------------------------------------------------

SIZE_T GetVectorCount(const RawVector* vec, SIZE_T elementSize) {
    if (!vec || !vec->first || !vec->last || !vec->end) {
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

bool GetZoneVector(const void* layout, BYTE** firstOut, SIZE_T* countOut) {
    if (!layout || !firstOut || !countOut) {
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
    *countOut = count;

    return true;
}

unsigned int ReadU32(const void* base, SIZE_T offset) {
    unsigned int value = 0;

    memcpy(&value,
           reinterpret_cast<const BYTE*>(base) + offset,
           sizeof(value));

    return value;
}

void WriteU32(void* base, SIZE_T offset, unsigned int value) {
    memcpy(reinterpret_cast<BYTE*>(base) + offset,
           &value,
           sizeof(value));
}

bool IsNativeQuadrantLayout(const void* layout) {
    if (!layout) {
        return false;
    }

    if (ReadU32(layout, 0x20) != 2 ||
        ReadU32(layout, 0x24) != 2) {
        return false;
    }

    BYTE* zoneFirst = nullptr;
    SIZE_T zoneCount = 0;

    if (!GetZoneVector(layout, &zoneFirst, &zoneCount) ||
        zoneCount != 4) {
        return false;
    }

    bool found[2][2] = {};

    for (SIZE_T i = 0; i < zoneCount; i++) {
        const BYTE* zone =
            zoneFirst + i * kSnapZoneSize;

        const unsigned int originColumn =
            ReadU32(zone, 0x20);

        const unsigned int originRow =
            ReadU32(zone, 0x24);

        const unsigned int columnSpan =
            ReadU32(zone, 0x28);

        const unsigned int rowSpan =
            ReadU32(zone, 0x2C);

        if (originColumn >= 2 ||
            originRow >= 2 ||
            columnSpan != 1 ||
            rowSpan != 1 ||
            found[originRow][originColumn]) {
            return false;
        }

        found[originRow][originColumn] = true;
    }

    return found[0][0] &&
           found[0][1] &&
           found[1][0] &&
           found[1][1];
}

// -----------------------------------------------------------------------------
// Convert a copied native quadrant layout to four equal vertical columns
// -----------------------------------------------------------------------------

bool PatchToFourColumns(void* layout) {
    if (!IsNativeQuadrantLayout(layout)) {
        Wh_Log(
            L"Copied source layout no longer matches the expected "
            L"2x2 four-quadrant structure");
        return false;
    }

    BYTE* zoneFirst = nullptr;
    SIZE_T zoneCount = 0;

    if (!GetZoneVector(layout, &zoneFirst, &zoneCount) ||
        zoneCount != 4) {
        Wh_Log(
            L"Unexpected zone vector while patching custom layout");
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
        BYTE* zone =
            zoneFirst + i * kSnapZoneSize;

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
        Wh_Log(
            L"Can't append custom layout: invalid vector or unresolved "
            L"emplace helper");
        return false;
    }

    const SIZE_T count =
        GetVectorCount(vec, kSnapLayoutSize);

    if (count == 0 ||
        count > kMaxReasonableLayoutCount) {
        Wh_Log(
            L"Can't inspect Snap Layout set: invalid layout count %zu",
            count);
        return false;
    }

    const BYTE* vectorFirst =
        reinterpret_cast<const BYTE*>(vec->first);

    SIZE_T sourceIndex = count;

    for (SIZE_T i = 0; i < count; i++) {
        const BYTE* candidate =
            vectorFirst + i * kSnapLayoutSize;

        if (IsNativeQuadrantLayout(candidate)) {
            sourceIndex = i;
            break;
        }
    }

    if (sourceIndex == count) {
        Wh_Log(
            L"No compatible 2x2 four-quadrant source layout among %zu "
            L"native layouts",
            count);
        return false;
    }

    // This pointer is valid only until the insertion below. The native vector
    // helper may reallocate the vector's backing storage.
    const void* source =
        vectorFirst + sourceIndex * kSnapLayoutSize;

    // Use SnapLayout.dll's own native vector helper so all non-trivial members,
    // including the nested std::wstring and std::vector<SnapZone>, are copied
    // with the correct allocator and ownership semantics.
    void* newLayout =
        EmplaceLayout_Original(vec, source);

    if (!newLayout) {
        Wh_Log(
            L"Failed to clone native Snap Layout");
        return false;
    }

    // The source is fully validated immediately before the native deep copy,
    // so failure here isn't expected. If it does happen, the clone remains in
    // the vector because no verified erase helper is currently available.
    if (!PatchToFourColumns(newLayout)) {
        Wh_Log(
            L"Native Snap Layout clone couldn't be converted");
        return false;
    }

    Wh_Log(
        L"Added four-column Snap Layout using native source index %zu",
        sourceIndex);

    return true;
}

// -----------------------------------------------------------------------------
// SnapLayout.dll hooks
// -----------------------------------------------------------------------------

RawVector* __cdecl Layouts_Hook(
    void* thisPtr,
    RawVector* returnBuffer) {
    RawVector* result =
        Layouts_Original(thisPtr, returnBuffer);

    if (!result) {
        return result;
    }

    if (g_inSnapBarLoad) {
        if (!g_snapBarCustomAdded) {
            g_snapBarCustomAdded =
                CloneAndAppendFourColumn(result);
        }

        return result;
    }

    g_flyoutLayoutsBefore =
        GetVectorCount(result, kSnapLayoutSize);

    g_flyoutCustomAdded =
        CloneAndAppendFourColumn(result);

    g_flyoutLayoutsAfter =
        g_flyoutCustomAdded
            ? GetVectorCount(result, kSnapLayoutSize)
            : g_flyoutLayoutsBefore;

    return result;
}

int __cdecl PickerHeight_Hook(
    void* thisPtr,
    int* value) {
    const int hr =
        PickerHeight_Original(thisPtr, value);

    // Layouts() and get_PickerHeight() are observed to execute synchronously on
    // the same thread for the native Win+Z/maximize-button picker.
    //
    // Scale the complete native picker height proportionally rather than
    // adding a fixed pixel count. This automatically follows display scaling.
    if (hr == 0 &&
        value &&
        g_flyoutCustomAdded &&
        g_flyoutLayoutsBefore > 0 &&
        g_flyoutLayoutsAfter > g_flyoutLayoutsBefore) {
        const SIZE_T rowsBefore =
            (g_flyoutLayoutsBefore +
             kItemsPerRow - 1) /
            kItemsPerRow;

        const SIZE_T rowsAfter =
            (g_flyoutLayoutsAfter +
             kItemsPerRow - 1) /
            kItemsPerRow;

        if (rowsAfter > rowsBefore) {
            *value =
                MulDiv(
                    *value,
                    static_cast<int>(rowsAfter),
                    static_cast<int>(rowsBefore));
        }
    }

    return hr;
}

void __cdecl SnapBarLoadLayouts_Hook(
    void* thisPtr,
    double scale,
    int options,
    bool flag) {
    // Restore thread-local state even if the native C++/WinRT implementation
    // exits via an exception.
    ScopedSnapBarState scopedState;

    SnapBarLoadLayouts_Original(
        thisPtr,
        scale,
        options,
        flag);
}

// -----------------------------------------------------------------------------
// Late loading of SnapLayout.dll
// -----------------------------------------------------------------------------

bool HookSnapLayoutDll(
    HMODULE module,
    bool applyHookOperations) {
    if (!module) {
        return false;
    }

    bool expected = false;

    if (!g_snapLayoutHookClaimed.compare_exchange_strong(
            expected,
            true)) {
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
        Wh_Log(
            L"Failed to resolve one or more SnapLayout.dll symbols");

        return false;
    }

    Wh_Log(
        L"SnapLayout.dll symbols resolved successfully");

    if (applyHookOperations &&
        !Wh_ApplyHookOperations()) {
        Wh_Log(
            L"Failed to apply SnapLayout.dll hook operations");
        return false;
    }

    return true;
}

void HandleSnapLayoutDllIfLoaded(
    bool applyHookOperations) {
    if (g_snapLayoutHookClaimed.load()) {
        return;
    }

    HMODULE module =
        GetModuleHandleW(L"SnapLayout.dll");

    if (module) {
        HookSnapLayoutDll(
            module,
            applyHookOperations);
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags) {
    HMODULE module =
        LoadLibraryExW_Original(
            lpLibFileName,
            hFile,
            dwFlags);

    if (module &&
        !g_snapLayoutHookClaimed.load()) {
        // Check the loaded-module state rather than relying on the filename
        // passed to this call. SnapLayout.dll may also arrive as a dependency.
        HandleSnapLayoutDllIfLoaded(true);
    }

    return module;
}

}  // namespace

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    if (HMODULE snapLayoutModule =
            GetModuleHandleW(L"SnapLayout.dll")) {
        if (!HookSnapLayoutDll(
                snapLayoutModule,
                false)) {
            return FALSE;
        }

        return TRUE;
    }

    HMODULE kernelBaseModule =
        GetModuleHandleW(L"kernelbase.dll");

    if (!kernelBaseModule) {
        Wh_Log(
            L"Failed to get kernelbase.dll");
        return FALSE;
    }

    auto pKernelBaseLoadLibraryExW =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(
                kernelBaseModule,
                "LoadLibraryExW"));

    if (!pKernelBaseLoadLibraryExW) {
        Wh_Log(
            L"Failed to resolve kernelbase!LoadLibraryExW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            pKernelBaseLoadLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original)) {
        Wh_Log(
            L"Failed to hook kernelbase!LoadLibraryExW");
        return FALSE;
    }

    Wh_Log(
        L"SnapLayout.dll isn't loaded yet; waiting for Windows to load it");

    return TRUE;
}

void Wh_ModAfterInit() {
    // Close the race where SnapLayout.dll loads after the initial check but
    // before the LoadLibraryExW hook becomes active.
    HandleSnapLayoutDllIfLoaded(true);
}
