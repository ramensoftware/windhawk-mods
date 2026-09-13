// ==WindhawkMod==
// @id           windows-11-native-four-column-snap-layout
// @name         Windows 11 Native Four Column Snap Layout
// @description  Adds an extra native Windows 11 Snap Layout with four equal vertical columns
// @version      1.1
// @author       Hoffelhas
// @github       https://github.com/Hoffelhas
// @license      MIT
// @include      explorer.exe
// @architecture x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Native Four Column Snap Layout

Adds an extra native Windows 11 Snap Layout with four equal vertical columns.
This is especially useful on ultrawide monitors, where four equal vertical
zones make better use of the available horizontal screen space.

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

// Native flyout geometry observed on the tested Windows build.
//
// With six native layouts:
//   compact picker:  166 DIP
//   expanded picker: 244 DIP
//
// The difference corresponds to one additional layout row.
constexpr int kCompactPickerHeight = 166;
constexpr int kExpandedPickerHeight = 244;

constexpr int kAdditionalLayoutRowHeight =
    kExpandedPickerHeight - kCompactPickerHeight;  // 78 DIP

struct RawVector {
    void* first;
    void* last;
    void* end;
};

thread_local bool g_inSnapBarLoad = false;
thread_local bool g_snapBarCustomAdded = false;

thread_local bool g_flyoutCustomAdded = false;
thread_local SIZE_T g_flyoutLayoutsBefore = 0;
thread_local SIZE_T g_flyoutLayoutsAfter = 0;

// Window-group suggestion count associated with the current flyout.
//
// The typed WinRT vector reports:
//   0 suggestions for the compact one-window state
//   2 suggestions for the two-window state
//   3 suggestions for the three-or-more-window state
//
// -1 means no valid count has been observed for the current flyout yet.
thread_local int g_flyoutSuggestionCount = -1;

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

using PickerHeight_t =
    int(__cdecl*)(void* thisPtr, int* value);

PickerHeight_t PickerHeight_Original = nullptr;

using SuggestionVectorSize_t =
    int(__cdecl*)(void* thisPtr, unsigned int* value);

SuggestionVectorSize_t SuggestionVectorSize_Original = nullptr;

using SnapBarLoadLayouts_t =
    void(__cdecl*)(
        void* thisPtr,
        double scale,
        int options,
        bool flag);

SnapBarLoadLayouts_t SnapBarLoadLayouts_Original = nullptr;

// -----------------------------------------------------------------------------
// kernelbase!LoadLibraryExW
// -----------------------------------------------------------------------------

using LoadLibraryExW_t = decltype(&LoadLibraryExW);

LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

// -----------------------------------------------------------------------------
// Snap Bar state guard
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
// Raw vector helpers
// -----------------------------------------------------------------------------

SIZE_T GetVectorCount(
    const RawVector* vec,
    SIZE_T elementSize) {
    if (!vec ||
        !vec->first ||
        !vec->last ||
        !vec->end) {
        return 0;
    }

    const auto* first =
        reinterpret_cast<const BYTE*>(
            vec->first);

    const auto* last =
        reinterpret_cast<const BYTE*>(
            vec->last);

    const auto* end =
        reinterpret_cast<const BYTE*>(
            vec->end);

    if (last < first ||
        end < last) {
        return 0;
    }

    const SIZE_T sizeBytes =
        static_cast<SIZE_T>(
            last - first);

    const SIZE_T capacityBytes =
        static_cast<SIZE_T>(
            end - first);

    if (sizeBytes % elementSize != 0 ||
        capacityBytes % elementSize != 0) {
        return 0;
    }

    return sizeBytes / elementSize;
}

bool GetZoneVector(
    const void* layout,
    BYTE** firstOut,
    SIZE_T* countOut) {
    if (!layout ||
        !firstOut ||
        !countOut) {
        return false;
    }

    RawVector zones{};

    const BYTE* layoutBytes =
        reinterpret_cast<const BYTE*>(
            layout);

    memcpy(
        &zones.first,
        layoutBytes + 0x28,
        sizeof(zones.first));

    memcpy(
        &zones.last,
        layoutBytes + 0x30,
        sizeof(zones.last));

    memcpy(
        &zones.end,
        layoutBytes + 0x38,
        sizeof(zones.end));

    const SIZE_T count =
        GetVectorCount(
            &zones,
            kSnapZoneSize);

    if (count == 0 ||
        count > kMaxReasonableZoneCount) {
        return false;
    }

    *firstOut =
        reinterpret_cast<BYTE*>(
            zones.first);

    *countOut = count;

    return true;
}

unsigned int ReadU32(
    const void* base,
    SIZE_T offset) {
    unsigned int value = 0;

    memcpy(
        &value,
        reinterpret_cast<const BYTE*>(base) +
            offset,
        sizeof(value));

    return value;
}

void WriteU32(
    void* base,
    SIZE_T offset,
    unsigned int value) {
    memcpy(
        reinterpret_cast<BYTE*>(base) +
            offset,
        &value,
        sizeof(value));
}

// -----------------------------------------------------------------------------
// Identify native 2x2 four-quadrant layout
// -----------------------------------------------------------------------------

bool IsNativeQuadrantLayout(
    const void* layout) {
    if (!layout) {
        return false;
    }

    if (ReadU32(layout, 0x20) != 2 ||
        ReadU32(layout, 0x24) != 2) {
        return false;
    }

    BYTE* zoneFirst = nullptr;
    SIZE_T zoneCount = 0;

    if (!GetZoneVector(
            layout,
            &zoneFirst,
            &zoneCount) ||
        zoneCount != 4) {
        return false;
    }

    bool found[2][2] = {};

    for (SIZE_T i = 0;
         i < zoneCount;
         i++) {
        const BYTE* zone =
            zoneFirst +
            i * kSnapZoneSize;

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

        found[originRow][originColumn] =
            true;
    }

    return found[0][0] &&
           found[0][1] &&
           found[1][0] &&
           found[1][1];
}

// -----------------------------------------------------------------------------
// Convert cloned quadrant layout to 4 equal columns
// -----------------------------------------------------------------------------

bool PatchToFourColumns(
    void* layout) {
    if (!IsNativeQuadrantLayout(
            layout)) {
        Wh_Log(
            L"Copied source layout no longer matches expected "
            L"2x2 four-quadrant structure");

        return false;
    }

    BYTE* zoneFirst = nullptr;
    SIZE_T zoneCount = 0;

    if (!GetZoneVector(
            layout,
            &zoneFirst,
            &zoneCount) ||
        zoneCount != 4) {
        Wh_Log(
            L"Unexpected zone vector while patching custom layout");

        return false;
    }

    WriteU32(layout, 0x20, 4);
    WriteU32(layout, 0x24, 1);

    for (unsigned int i = 0;
         i < 4;
         i++) {
        BYTE* zone =
            zoneFirst +
            i * kSnapZoneSize;

        WriteU32(zone, 0x20, i);
        WriteU32(zone, 0x24, 0);
        WriteU32(zone, 0x28, 1);
        WriteU32(zone, 0x2C, 1);
    }

    return true;
}

// -----------------------------------------------------------------------------
// Clone and append custom layout
// -----------------------------------------------------------------------------

bool CloneAndAppendFourColumn(
    RawVector* vec) {
    if (!vec ||
        !EmplaceLayout_Original) {
        Wh_Log(
            L"Can't append custom layout: invalid vector or unresolved "
            L"emplace helper");

        return false;
    }

    const SIZE_T count =
        GetVectorCount(
            vec,
            kSnapLayoutSize);

    if (count == 0 ||
        count > kMaxReasonableLayoutCount) {
        Wh_Log(
            L"Invalid Snap Layout count: %zu",
            count);

        return false;
    }

    const BYTE* vectorFirst =
        reinterpret_cast<const BYTE*>(
            vec->first);

    SIZE_T sourceIndex = count;

    for (SIZE_T i = 0;
         i < count;
         i++) {
        const BYTE* candidate =
            vectorFirst +
            i * kSnapLayoutSize;

        if (IsNativeQuadrantLayout(
                candidate)) {
            sourceIndex = i;
            break;
        }
    }

    if (sourceIndex == count) {
        Wh_Log(
            L"No compatible native 2x2 four-zone layout found");

        return false;
    }

    const void* source =
        vectorFirst +
        sourceIndex *
            kSnapLayoutSize;

    void* newLayout =
        EmplaceLayout_Original(
            vec,
            source);

    if (!newLayout ||
        newLayout !=
            reinterpret_cast<BYTE*>(
                vec->last) -
                kSnapLayoutSize) {
        Wh_Log(
            L"Unexpected appended layout position");

        return false;
    }

    if (!PatchToFourColumns(
            newLayout)) {
        Wh_Log(
            L"Failed to convert cloned layout to four columns");

        return false;
    }

    Wh_Log(
        L"Added four-column Snap Layout using native source index %zu",
        sourceIndex);

    return true;
}

// -----------------------------------------------------------------------------
// Layout hook
// -----------------------------------------------------------------------------

RawVector* __cdecl Layouts_Hook(
    void* thisPtr,
    RawVector* returnBuffer) {
    RawVector* result =
        Layouts_Original(
            thisPtr,
            returnBuffer);

    if (!result) {
        return result;
    }

    if (g_inSnapBarLoad) {
        if (!g_snapBarCustomAdded) {
            g_snapBarCustomAdded =
                CloneAndAppendFourColumn(
                    result);
        }

        return result;
    }

    // Start a new flyout state. The picker normally queries the typed
    // WindowGroupSuggestionViewModel vector again before requesting
    // PickerHeight.
    //
    // Invalidating here also prevents a stale count from a previous flyout
    // from causing the wrong height adjustment.
    g_flyoutSuggestionCount = -1;

    g_flyoutLayoutsBefore =
        GetVectorCount(
            result,
            kSnapLayoutSize);

    g_flyoutCustomAdded =
        CloneAndAppendFourColumn(
            result);

    g_flyoutLayoutsAfter =
        g_flyoutCustomAdded
            ? GetVectorCount(
                  result,
                  kSnapLayoutSize)
            : g_flyoutLayoutsBefore;

    return result;
}

// -----------------------------------------------------------------------------
// Window-group suggestion count
// -----------------------------------------------------------------------------

int __cdecl SuggestionVectorSize_Hook(
    void* thisPtr,
    unsigned int* value) {
    const int hr =
        SuggestionVectorSize_Original(
            thisPtr,
            value);

    if (SUCCEEDED(hr) &&
        value &&
        !g_inSnapBarLoad) {
        g_flyoutSuggestionCount =
            static_cast<int>(*value);
    }

    return hr;
}

// -----------------------------------------------------------------------------
// Picker height
// -----------------------------------------------------------------------------

int __cdecl PickerHeight_Hook(
    void* thisPtr,
    int* value) {
    const int hr =
        PickerHeight_Original(
            thisPtr,
            value);

    if (!SUCCEEDED(hr) ||
        !value ||
        !g_flyoutCustomAdded ||
        g_flyoutLayoutsBefore == 0 ||
        g_flyoutLayoutsAfter !=
            g_flyoutLayoutsBefore + 1) {
        return hr;
    }

    const int nativeHeight =
        *value;

    bool addLayoutRow = false;

    // Compact one-window state:
    //
    // Six native layouts fit in two rows, but adding the seventh layout
    // creates a third row. Windows still reports the original compact
    // 166-DIP picker height, so add one native row height.
    if (nativeHeight ==
        kCompactPickerHeight) {
        addLayoutRow = true;
    }

    // Expanded picker state:
    //
    // With two window-group suggestions, the native 244-DIP picker already
    // has enough vertical room for the added layout.
    //
    // With three suggestions, the same native height becomes too small once
    // the seventh layout creates the additional layout row.
    else if (
        nativeHeight ==
            kExpandedPickerHeight &&
        g_flyoutSuggestionCount >= 3) {
        addLayoutRow = true;
    }

    if (addLayoutRow) {
        *value +=
            kAdditionalLayoutRowHeight;
    }

    return hr;
}

// -----------------------------------------------------------------------------
// Snap Bar
// -----------------------------------------------------------------------------

void __cdecl SnapBarLoadLayouts_Hook(
    void* thisPtr,
    double scale,
    int options,
    bool flag) {
    ScopedSnapBarState scopedState;

    SnapBarLoadLayouts_Original(
        thisPtr,
        scale,
        options,
        flag);
}

// -----------------------------------------------------------------------------
// Symbol hooks
// -----------------------------------------------------------------------------

bool HookSnapLayoutDll(
    HMODULE module,
    bool applyHookOperations) {
    if (!module) {
        return false;
    }

    bool expected = false;

    if (!g_snapLayoutHookClaimed
             .compare_exchange_strong(
                 expected,
                 true)) {
        return true;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
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
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::impl::convertible_observable_vector<struct winrt::SnapLayout::WindowGroupSuggestionViewModel,class std::vector<struct winrt::SnapLayout::WindowGroupSuggestionViewModel,class std::allocator<struct winrt::SnapLayout::WindowGroupSuggestionViewModel> >,struct winrt::impl::single_threaded_collection_base>,struct winrt::Windows::Foundation::Collections::IVector<struct winrt::SnapLayout::WindowGroupSuggestionViewModel> >::get_Size(unsigned int *))",
            },
            &SuggestionVectorSize_Original,
            SuggestionVectorSize_Hook,
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
            hooks,
            ARRAYSIZE(hooks))) {
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

// -----------------------------------------------------------------------------
// Late SnapLayout.dll loading
// -----------------------------------------------------------------------------

void HandleSnapLayoutDllIfLoaded(
    bool applyHookOperations) {
    if (g_snapLayoutHookClaimed.load()) {
        return;
    }

    HMODULE module =
        GetModuleHandleW(
            L"SnapLayout.dll");

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
        HandleSnapLayoutDllIfLoaded(
            true);
    }

    return module;
}

}  // namespace

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (HMODULE snapLayoutModule =
            GetModuleHandleW(
                L"SnapLayout.dll")) {
        if (!HookSnapLayoutDll(
                snapLayoutModule,
                false)) {
            return FALSE;
        }

        return TRUE;
    }

    HMODULE kernelBaseModule =
        GetModuleHandleW(
            L"kernelbase.dll");

    if (!kernelBaseModule) {
        Wh_Log(
            L"Failed to get kernelbase.dll");

        return FALSE;
    }

    auto pLoadLibraryExW =
        reinterpret_cast<
            decltype(&LoadLibraryExW)>(
            GetProcAddress(
                kernelBaseModule,
                "LoadLibraryExW"));

    if (!pLoadLibraryExW) {
        Wh_Log(
            L"Failed to resolve kernelbase!LoadLibraryExW");

        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            pLoadLibraryExW,
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
    Wh_Log(L">");

    HandleSnapLayoutDllIfLoaded(
        true);
}
