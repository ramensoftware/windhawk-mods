// ==WindhawkMod==
// @id           windows-11-native-four-column-snap-layout
// @name         Windows 11 Native Four Column Snap Layout
// @description  Adds an extra native Windows 11 Snap Layout with four equal vertical columns
// @version      1.0
// @author       Hoffelhas
// @github       https://github.com/Hoffelhas
// @include      explorer.exe
// @architecture amd64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Native Four Column Snap Layout

Adds an extra native Windows 11 Snap Layout with four equal vertical columns:

`| 25% | 25% | 25% | 25% |`

The layout is integrated into Windows' own Snap Layout UI and appears in:

- **Win+Z**
- **Maximize-button hover**
- **Snap Bar** when dragging a window to the top of the screen

Because the mod extends the native Snap Layout model, snapping continues to use
Windows' normal Snap Assist and Snap Groups behavior.

## Compatibility

This mod relies on undocumented Windows internals and private symbols from
`SnapLayout.dll`. Windows updates can change these implementation details.

The mod contains structural checks before modifying a layout. If the expected
native layout structure isn't found, it leaves that layout set unchanged rather
than patching an unknown structure.

Developed and tested on Windows 11 build **26200.9445** on x86-64.

If a Windows update changes the relevant private symbols, Windhawk may be unable
to load the mod until it is updated.
*/
// ==/WindhawkModReadme==

// Source code is published under the MIT License.

#include <windhawk_utils.h>
#include <windows.h>
#include <cstring>

namespace {

constexpr SIZE_T kSnapLayoutSize = 0x50;
constexpr SIZE_T kSnapZoneSize = 0x38;
constexpr SIZE_T kNativeLayoutCount = 6;
constexpr SIZE_T kCustomLayoutCount = 7;

constexpr int kNativePickerHeight = 244;
constexpr int kExtraPickerHeight = 80;

struct RawVector {
    void* first;
    void* last;
    void* end;
};

// The Snap Bar asks SnapModel::Layouts() more than once while loading.
// These thread-local flags distinguish that path from the Win+Z/maximize
// flyout path and avoid appending the custom layout more than once per load.
thread_local bool g_inSnapBarLoad = false;
thread_local bool g_snapBarCustomAdded = false;

// Set after the normal flyout path successfully appends the custom layout.
// Thread-local for consistency with the Snap Bar state.
thread_local bool g_flyoutCustomAdded = false;

// Keep track of SnapLayout.dll so a reference acquired by this mod can be
// released again when the mod unloads.
HMODULE g_snapLayoutModule = nullptr;
bool g_loadedSnapLayoutModule = false;


// -----------------------------------------------------------------------------
// Native functions resolved from SnapLayout.dll
// -----------------------------------------------------------------------------

using Layouts_t = RawVector* (__cdecl*)(
    void* thisPtr,
    RawVector* returnBuffer
);

Layouts_t Layouts_Original = nullptr;


using EmplaceLayout_t = void* (__cdecl*)(
    void* vectorThis,
    const void* sourceLayout
);

EmplaceLayout_t EmplaceLayout_Original = nullptr;


using PickerHeight_t = int (__cdecl*)(
    void* thisPtr,
    int* value
);

PickerHeight_t PickerHeight_Original = nullptr;


using SnapBarLoadLayouts_t = void (__cdecl*)(
    void* thisPtr,
    double scale,
    int options,
    bool flag
);

SnapBarLoadLayouts_t SnapBarLoadLayouts_Original = nullptr;


// This pass-through hook exists so Windhawk resolves the private vector helper
// and exposes a callable original/trampoline pointer.
void* __cdecl EmplaceLayout_Hook(
    void* vectorThis,
    const void* sourceLayout)
{
    return EmplaceLayout_Original(
        vectorThis,
        sourceLayout
    );
}


int __cdecl PickerHeight_Hook(
    void* thisPtr,
    int* value)
{
    int hr =
        PickerHeight_Original(
            thisPtr,
            value
        );

    // The extra flyout item occupies one more row.
    // Only adjust the exact native height observed on the supported build,
    // and only after the custom flyout layout was actually added.
    if (hr == 0 &&
        value &&
        g_flyoutCustomAdded &&
        *value == kNativePickerHeight)
    {
        *value += kExtraPickerHeight;
    }

    return hr;
}


void __cdecl SnapBarLoadLayouts_Hook(
    void* thisPtr,
    double scale,
    int options,
    bool flag)
{
    const bool previousInSnapBarLoad =
        g_inSnapBarLoad;

    const bool previousCustomAdded =
        g_snapBarCustomAdded;

    g_inSnapBarLoad = true;
    g_snapBarCustomAdded = false;

    SnapBarLoadLayouts_Original(
        thisPtr,
        scale,
        options,
        flag
    );

    g_inSnapBarLoad =
        previousInSnapBarLoad;

    g_snapBarCustomAdded =
        previousCustomAdded;
}


// -----------------------------------------------------------------------------
// Raw SnapLayout / SnapZone helpers
// -----------------------------------------------------------------------------

SIZE_T GetVectorCount(
    const RawVector* vec,
    SIZE_T elementSize)
{
    if (!vec ||
        !vec->first ||
        !vec->last ||
        elementSize == 0)
    {
        return 0;
    }

    const auto* first =
        reinterpret_cast<const BYTE*>(
            vec->first
        );

    const auto* last =
        reinterpret_cast<const BYTE*>(
            vec->last
        );

    if (last < first) {
        return 0;
    }

    const SIZE_T bytes =
        static_cast<SIZE_T>(
            last - first
        );

    if (bytes % elementSize != 0) {
        return 0;
    }

    return bytes / elementSize;
}


SIZE_T GetZoneCount(
    const void* layout)
{
    if (!layout) {
        return 0;
    }

    void* first = nullptr;
    void* last = nullptr;

    memcpy(
        &first,
        reinterpret_cast<const BYTE*>(layout) + 0x28,
        sizeof(first)
    );

    memcpy(
        &last,
        reinterpret_cast<const BYTE*>(layout) + 0x30,
        sizeof(last)
    );

    if (!first || !last) {
        return 0;
    }

    const auto* firstBytes =
        reinterpret_cast<const BYTE*>(
            first
        );

    const auto* lastBytes =
        reinterpret_cast<const BYTE*>(
            last
        );

    if (lastBytes < firstBytes) {
        return 0;
    }

    const SIZE_T bytes =
        static_cast<SIZE_T>(
            lastBytes - firstBytes
        );

    if (bytes % kSnapZoneSize != 0) {
        return 0;
    }

    return bytes / kSnapZoneSize;
}


void WriteU32(
    void* base,
    SIZE_T offset,
    unsigned int value)
{
    memcpy(
        reinterpret_cast<BYTE*>(base) + offset,
        &value,
        sizeof(value)
    );
}


bool IsExpectedSourceLayout(
    const void* layout)
{
    if (!layout) {
        return false;
    }

    unsigned int columns = 0;
    unsigned int rows = 0;

    memcpy(
        &columns,
        reinterpret_cast<const BYTE*>(layout) + 0x20,
        sizeof(columns)
    );

    memcpy(
        &rows,
        reinterpret_cast<const BYTE*>(layout) + 0x24,
        sizeof(rows)
    );

    return
        columns == 2 &&
        rows == 2 &&
        GetZoneCount(layout) == 4;
}


// -----------------------------------------------------------------------------
// Convert copied native quadrant layout into four equal columns
// -----------------------------------------------------------------------------

bool PatchToFourColumns(
    void* layout)
{
    if (!IsExpectedSourceLayout(layout)) {
        return false;
    }

    void* zoneFirst = nullptr;
    void* zoneLast = nullptr;

    memcpy(
        &zoneFirst,
        reinterpret_cast<BYTE*>(layout) + 0x28,
        sizeof(zoneFirst)
    );

    memcpy(
        &zoneLast,
        reinterpret_cast<BYTE*>(layout) + 0x30,
        sizeof(zoneLast)
    );

    if (!zoneFirst || !zoneLast) {
        return false;
    }

    const SIZE_T zoneBytes =
        reinterpret_cast<BYTE*>(zoneLast) -
        reinterpret_cast<BYTE*>(zoneFirst);

    if (zoneBytes != 4 * kSnapZoneSize) {
        return false;
    }

    // SnapLayout grid:
    // 4 columns × 1 row.
    WriteU32(layout, 0x20, 4);
    WriteU32(layout, 0x24, 1);

    // Relevant SnapZone fields:
    //
    // +0x20 OriginColumn
    // +0x24 OriginRow
    // +0x28 ColumnSpan
    // +0x2C RowSpan
    // +0x30 GridUnitType (preserved)
    //
    // Result:
    //
    // | 0 | 1 | 2 | 3 |
    //
    // with each zone spanning exactly one column.
    for (unsigned int i = 0; i < 4; i++) {

        BYTE* zone =
            reinterpret_cast<BYTE*>(zoneFirst) +
            i * kSnapZoneSize;

        WriteU32(
            zone,
            0x20,
            i
        );

        WriteU32(
            zone,
            0x24,
            0
        );

        WriteU32(
            zone,
            0x28,
            1
        );

        WriteU32(
            zone,
            0x2C,
            1
        );
    }

    return true;
}


// -----------------------------------------------------------------------------
// Deep-copy and append one native layout
// -----------------------------------------------------------------------------

bool CloneAndAppendFourColumn(
    RawVector* vec,
    SIZE_T sourceIndex)
{
    if (!vec ||
        !EmplaceLayout_Original)
    {
        return false;
    }

    const SIZE_T count =
        GetVectorCount(
            vec,
            kSnapLayoutSize
        );

    // Safety check:
    // this build is expected to return six native layouts.
    if (count != kNativeLayoutCount ||
        sourceIndex >= count)
    {
        return false;
    }

    BYTE* source =
        reinterpret_cast<BYTE*>(
            vec->first
        ) +
        sourceIndex * kSnapLayoutSize;

    if (!IsExpectedSourceLayout(source)) {
        return false;
    }

    // Use SnapLayout.dll's own std::vector insertion helper.
    //
    // SnapLayout contains non-trivial members such as std::wstring and
    // std::vector<SnapZone>, so a raw memcpy clone would duplicate ownership
    // pointers and would be unsafe.
    void* newLayout =
        EmplaceLayout_Original(
            vec,
            source
        );

    if (!newLayout) {
        return false;
    }

    if (GetVectorCount(
            vec,
            kSnapLayoutSize) !=
        kCustomLayoutCount)
    {
        return false;
    }

    return PatchToFourColumns(
        newLayout
    );
}


// -----------------------------------------------------------------------------
// SnapModel::Layouts hook
// -----------------------------------------------------------------------------

RawVector* __cdecl Layouts_Hook(
    void* thisPtr,
    RawVector* returnBuffer)
{
    RawVector* result =
        Layouts_Original(
            thisPtr,
            returnBuffer
        );

    if (!returnBuffer) {
        return result;
    }

    if (g_inSnapBarLoad) {

        // Snap Bar ordering observed on the supported build:
        //
        // 0: 2x1 / 2 zones
        // 1: 3x1 / 2 zones
        // 2: 2x2 / 3 zones
        // 3: 2x2 / 4 zones  <-- source
        // 4: 3x1 / 3 zones
        // 5: 4x1 / 3 zones
        //
        // SnapBar asks for SnapModel::Layouts() more than once while loading,
        // so append the custom layout only once for that load.
        if (!g_snapBarCustomAdded &&
            CloneAndAppendFourColumn(
                returnBuffer,
                3))
        {
            g_snapBarCustomAdded = true;
        }

        return result;
    }

    // Win+Z / maximize-hover ordering observed on the supported build:
    //
    // Element 4 is the native 2x2 / four-zone source layout.
    if (CloneAndAppendFourColumn(
            returnBuffer,
            4))
    {
        g_flyoutCustomAdded = true;
    }

    return result;
}

}  // namespace


// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    g_snapLayoutModule =
        GetModuleHandleW(
            L"SnapLayout.dll"
        );

    if (!g_snapLayoutModule) {

        // On the tested Windows 11 build, SnapLayout.dll is part of the
        // MicrosoftWindows.Client.Core system app.
        //
        // Load it explicitly if Explorer hasn't loaded it yet so Windhawk can
        // resolve and install the private-symbol hooks.
        g_snapLayoutModule =
            LoadLibraryExW(
                L"C:\\WINDOWS\\SystemApps\\MicrosoftWindows.Client.Core_cw5n1h2txyewy\\SnapLayout.dll",
                nullptr,
                LOAD_WITH_ALTERED_SEARCH_PATH
            );

        if (g_snapLayoutModule) {
            g_loadedSnapLayoutModule = true;
        }
    }

    if (!g_snapLayoutModule) {
        Wh_Log(
            L"Failed to load SnapLayout.dll"
        );

        return FALSE;
    }

    // SnapLayout.dll
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
            EmplaceLayout_Hook,
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
            g_snapLayoutModule,
            snapLayoutDllHooks,
            ARRAYSIZE(snapLayoutDllHooks)))
    {
        Wh_Log(
            L"Failed to resolve one or more SnapLayout.dll symbols"
        );

        if (g_loadedSnapLayoutModule) {
            FreeLibrary(
                g_snapLayoutModule
            );

            g_snapLayoutModule = nullptr;
            g_loadedSnapLayoutModule = false;
        }

        return FALSE;
    }

    return TRUE;
}


void Wh_ModUninit()
{
    if (g_loadedSnapLayoutModule &&
        g_snapLayoutModule)
    {
        FreeLibrary(
            g_snapLayoutModule
        );

        g_snapLayoutModule = nullptr;
        g_loadedSnapLayoutModule = false;
    }
}
