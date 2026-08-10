// ==WindhawkMod==
// @id              taskbar-separators-prototype
// @name            Taskbar Separators - Prototype
// @description     Creates genuine independently reorderable taskbar separator pins.
// @version         0.2
// @author          meteoni
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lpropsys -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Prototype for genuine Windows 11 taskbar separators.

Each configured separator is represented by its own real pinned Shell shortcut.
The mod:
1. Writes a compact embedded multi-size separator icon to its Windhawk storage folder.
2. Creates one uniquely identified .lnk per configured separator.
3. Pins each shortcut with the private PinManager COM interface.
4. Moves each pin to its configured position.
5. Gives only the generated separator TaskListButtons configurable normal/small widths.
6. Unpins the separators and deletes the generated files when the mod unloads.

The position setting is 1-based for the user: 1 means the first pinned position.
Width overrides are per separator. They are applied after the normal
TaskListButton::UpdateVisualStates hook chain returns, so they compose with
the Taskbar height and icon size mod regardless of hook installation order.

This is still a prototype. Click/context-menu suppression and richer XAML styling
can be added separately.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- identifierPrefix: WindhawkSeparator-8F31A7D2
  $name: Separator identifier
  $description: >-
    File/display-name prefix for generated separator shortcuts. Unsupported
    filename/regex characters are replaced with underscores.
- separators:
    - - index: 5
        $name: Position
        $description: 1 = first pinned taskbar position.
      - width: 14
        $name: Width
        $description: Width for the normal taskbar icon mode.
      - widthSmall: 10
        $name: Small width
        $description: Width for the small taskbar icon mode.
    - - index: 10
        $name: Position
        $description: 1 = first pinned taskbar position.
      - width: 14
        $name: Width
        $description: Width for the normal taskbar icon mode.
      - widthSmall: 10
        $name: Small width
        $description: Width for the small taskbar icon mode.
  $name: Separators
  $description: Add or remove entries to control how many separators are created.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>

#include <algorithm>
#include <atomic>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// -----------------------------------------------------------------------------
// Private taskbar PinManager COM ABI, verified on the current Windows build.
// -----------------------------------------------------------------------------

static const CLSID CLSID_PinManager = {
    0xA5C8D635, 0xB4ED, 0x452B,
    {0x81, 0x09, 0x95, 0x01, 0x78, 0x10, 0x96, 0xD1}
};

static const IID IID_IPinManagerInterop2 = {
    0x87D9E034, 0x56D0, 0x4F8C,
    {0xBE, 0x59, 0x99, 0x7B, 0x01, 0x75, 0x47, 0x10}
};

static const IID IID_IPinManagerInterop3 = {
    0x27216D91, 0x78B8, 0x406D,
    {0x85, 0xC4, 0x5F, 0xDB, 0xF5, 0x1C, 0xD1, 0x6B}
};

enum PINNEDLISTMODIFYCALLER : int {
    // Observed from a native modern taskbar unpin on this build.
    PMC_JUMPVIEWBROKER = 11,

    PMC_TASKBANDINSERT = 23,
    PMC_TASKBANDMODIFY = 24,
    PMC_TASKBANDPIN = 25,
    PMC_TASKBANDPINGROUP = 26,
    PMC_TASKBANDREORDER = 27,
};

struct IPinManagerInterop2 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE PinItemToTaskbarShim(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE PinItemFromTrustedCaller(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyPrependDefaultTaskbarLayout() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyAppendDefaultTaskbarLayout() = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyInPlaceTaskbarLayout(
        int layoutType) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyReorderTaskbarLayout(
        int layoutType,
        int value) = 0;

    virtual HRESULT STDMETHODCALLTYPE UnpinTaskbarItem(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE UpdatePinnedTaskbarItem(
        PCUIDLIST_ABSOLUTE oldPidl,
        PCUIDLIST_ABSOLUTE newPidl,
        PINNEDLISTMODIFYCALLER caller) = 0;
};

struct IPinManagerInterop3 : IPinManagerInterop2 {
    virtual HRESULT STDMETHODCALLTYPE MoveTaskbarPin(
        PCUIDLIST_ABSOLUTE pidl,
        int index,
        PINNEDLISTMODIFYCALLER caller) = 0;
};

// -----------------------------------------------------------------------------
// Settings/state.
// -----------------------------------------------------------------------------

struct SeparatorSetting {
    int ordinal;       // Stable within this loaded settings snapshot: 1, 2, ...
    int targetIndex;   // Zero-based value passed to MoveTaskbarPin.
    double width;      // Normal taskbar icon mode.
    double widthSmall; // Small taskbar icon mode.
};

struct Settings {
    std::wstring identifierPrefix;
    std::vector<SeparatorSetting> separators;
};

static Settings g_settings;
static std::wstring g_storagePath;
static std::wstring g_iconPath;

static std::atomic<bool> g_taskbarViewDllHooked;
static std::atomic<bool> g_unloading;
static bool g_hasDynamicIconScaling;

// Internal AppUserModelID namespace. Kept independent from the user-visible
// filename prefix so user changes don't accidentally create invalid AppIDs.
static constexpr wchar_t kInternalAppIdPrefix[] =
    L"Windhawk.TaskbarSeparator.8F31A7D2";

// -----------------------------------------------------------------------------
// Embedded icon.
//
// Compact 32-bit multi-size ICO extracted from the nicer separator asset.
// Keeps exact 16/24/32/48 px taskbar-sized frames and drops redundant legacy/256 px variants.
// -----------------------------------------------------------------------------

static constexpr unsigned char kSeparatorIcon[] = {
    0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x20, 0x00, 0x6F, 0x03, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x10, 0x10,
    0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x96, 0x00, 0x00, 0x00, 0x95, 0x03,
    0x00, 0x00, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00,
    0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x08, 0x06, 0x00, 0x00, 0x00, 0x5C, 0x72, 0xA8, 0x66, 0x00,
    0x00, 0x03, 0x36, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0xED, 0xDC, 0x3D,
    0x4E, 0x03, 0x31, 0x14, 0x85, 0xD1, 0xEB, 0xFC, 0x40, 0x4F, 0x07, 0x25,
    0xAB, 0xCA, 0x0E, 0x93, 0x9D, 0xB0, 0x10, 0x56, 0x40, 0x83, 0x04, 0x84,
    0x31, 0x45, 0x68, 0x90, 0x90, 0xF0, 0x48, 0x14, 0xB1, 0x7D, 0x8E, 0x44,
    0x97, 0x50, 0xBC, 0x51, 0x3E, 0xD9, 0xD2, 0xD3, 0x24, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xBF, 0x8A, 0x11, 0x8C, 0xE7,
    0x74, 0x3A, 0x6D, 0x5A, 0x3F, 0x7B, 0x38, 0x1C, 0x16, 0x13, 0x9B, 0xD7,
    0xCE, 0x08, 0x86, 0xB4, 0x4D, 0xD2, 0x1A, 0x81, 0x37, 0xE3, 0x9A, 0xD7,
    0xC6, 0x08, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40,
    0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0xA0, 0x7F, 0x36, 0x01,
    0x3B, 0x71, 0x3C, 0x1E, 0x4B, 0x2E, 0x1B, 0x7E, 0x7F, 0xAA, 0xB5, 0x3E,
    0x26, 0xB9, 0x6B, 0xFC, 0xD7, 0x4F, 0xA6, 0x2B, 0x00, 0x5C, 0xBF, 0xB2,
    0xE2, 0xC4, 0x76, 0x97, 0xE4, 0xDE, 0xC8, 0x70, 0x05, 0x00, 0x04, 0x00,
    0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10,
    0x00, 0x40, 0x00, 0x80, 0x09, 0xD8, 0x04, 0xEC, 0xC7, 0xDA, 0x4D, 0xC0,
    0x07, 0x23, 0xC3, 0x09, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00,
    0x04, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x26,
    0x60, 0x13, 0x70, 0x4C, 0x6B, 0xB6, 0x06, 0x11, 0x00, 0x06, 0xB3, 0xF5,
    0x6C, 0x71, 0x05, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x80, 0x49, 0xD8,
    0x16, 0x1B, 0xF7, 0xB9, 0xDE, 0x1A, 0x03, 0x02, 0x30, 0x88, 0x52, 0xCA,
    0xDA, 0x93, 0xDD, 0xD6, 0xD4, 0x70, 0x05, 0x00, 0x04, 0x00, 0x10, 0x00,
    0x40, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40,
    0x00, 0x80, 0x09, 0xD8, 0x04, 0xEC, 0x4B, 0xEB, 0x3A, 0xE0, 0x3E, 0xC9,
    0x8D, 0x71, 0xE1, 0x04, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00,
    0x02, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02,
    0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00,
    0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80,
    0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02,
    0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00,
    0xF0, 0x6F, 0x76, 0x46, 0xD0, 0x87, 0x5A, 0xEB, 0xDA, 0xAF, 0x14, 0x53,
    0xC3, 0x09, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00,
    0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x26, 0x60, 0x13,
    0xB0, 0x2F, 0xAD, 0xDB, 0x7D, 0x1B, 0x71, 0x47, 0x00, 0xC6, 0xFB, 0xF1,
    0x97, 0x15, 0xCF, 0x75, 0x6F, 0x64, 0xB8, 0x02, 0x00, 0x02, 0x00, 0x08,
    0x00, 0x20, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00,
    0x20, 0x00, 0x80, 0x00, 0x00, 0x23, 0xB3, 0x0A, 0xDC, 0x89, 0x52, 0xBC,
    0xE4, 0x17, 0x27, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10,
    0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0xE0, 0x17,
    0x36, 0x01, 0x3B, 0x51, 0x6B, 0x35, 0x04, 0x9C, 0x00, 0x00, 0x01, 0x00,
    0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10,
    0x00, 0x40, 0x00, 0x00, 0x01, 0x98, 0x85, 0xB5, 0x41, 0x9A, 0x58, 0x05,
    0xEE, 0xEB, 0x47, 0x5D, 0x45, 0x00, 0x27, 0x00, 0x40, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00,
    0x10, 0x00, 0xE0, 0x07, 0x9B, 0x80, 0x7D, 0x69, 0xDD, 0xEE, 0x3B, 0x27,
    0xF9, 0x30, 0x2E, 0x04, 0x60, 0xDE, 0x50, 0x2C, 0xC6, 0x80, 0x2B, 0x00,
    0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x08, 0x00, 0x20,
    0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00,
    0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00,
    0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20,
    0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00,
    0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00,
    0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20,
    0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x24, 0xD9, 0x19, 0x41, 0x57, 0x6A,
    0xE3, 0xE7, 0xDE, 0x92, 0xBC, 0x1A, 0x17, 0x4E, 0x00, 0x80, 0x00, 0x00,
    0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02,
    0x00, 0x08, 0x00, 0x30, 0x01, 0x9B, 0x80, 0x63, 0x5A, 0xBE, 0xFF, 0x40,
    0x00, 0x26, 0xF4, 0x99, 0xE4, 0xC3, 0x18, 0x70, 0x05, 0x00, 0x04, 0x00,
    0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10,
    0x00, 0x40, 0x00, 0x80, 0x09, 0xD8, 0x04, 0xEC, 0x47, 0x4D, 0xFB, 0x4B,
    0x41, 0xCF, 0x49, 0xDE, 0x8D, 0x0C, 0x01, 0x98, 0xD3, 0x92, 0xCB, 0x3A,
    0x30, 0xB8, 0x02, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00,
    0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0xC0, 0xE8, 0x6C,
    0x02, 0xF6, 0xA3, 0xA6, 0x7D, 0xBB, 0xEF, 0x39, 0xC9, 0xAB, 0x91, 0xE1,
    0x04, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x02,
    0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x13, 0xB0, 0x09, 0xD8,
    0x97, 0xD6, 0x97, 0x82, 0xBE, 0xE4, 0xF2, 0x5E, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAE, 0xC9, 0x17, 0x41, 0x64, 0x2F,
    0xF8, 0x93, 0x03, 0xD8, 0xF7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E,
    0x44, 0xAE, 0x42, 0x60, 0x82, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A,
    0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0xF3,
    0xFF, 0x61, 0x00, 0x00, 0x00, 0x5D, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA,
    0xC5, 0x90, 0xC1, 0x09, 0x80, 0x50, 0x0C, 0x43, 0x13, 0x11, 0x9C, 0xC4,
    0xD9, 0xDB, 0x79, 0x5C, 0x46, 0x04, 0x21, 0x5E, 0xBC, 0xFE, 0xB6, 0xFC,
    0x22, 0xE6, 0x9A, 0xF2, 0x92, 0x06, 0x08, 0xE4, 0xEE, 0x0B, 0x12, 0x0D,
    0x0F, 0xCC, 0x8C, 0x92, 0xF6, 0x69, 0xC0, 0xEB, 0xB5, 0x00, 0x00, 0xB0,
    0x65, 0x80, 0x75, 0x64, 0x90, 0x2C, 0x01, 0xD2, 0x91, 0xA6, 0x1B, 0x48,
    0x2A, 0x05, 0x44, 0x07, 0x8C, 0x02, 0xAA, 0x2F, 0xA8, 0x03, 0x10, 0x80,
    0xFB, 0xD7, 0x06, 0x25, 0x65, 0x80, 0xB3, 0xBB, 0xC1, 0xD5, 0x05, 0x1C,
    0xF8, 0x5A, 0x0F, 0x6C, 0x9F, 0x13, 0x28, 0xFF, 0x6A, 0xE4, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};

static_assert(sizeof(kSeparatorIcon) == 1067);

// -----------------------------------------------------------------------------
// Small helpers.
// -----------------------------------------------------------------------------

static std::wstring JoinPath(
    const std::wstring& directory,
    const std::wstring& fileName) {
    if (directory.empty()) {
        return fileName;
    }

    if (directory.back() == L'\\' || directory.back() == L'/') {
        return directory + fileName;
    }

    return directory + L"\\" + fileName;
}

static std::wstring SanitizePrefix(std::wstring value) {
    // Keep this intentionally restrictive:
    // - safe as a Windows filename
    // - easy to use later in a XAML regex
    // - no escaping surprises
    for (wchar_t& ch : value) {
        const bool allowed =
            (ch >= L'a' && ch <= L'z') ||
            (ch >= L'A' && ch <= L'Z') ||
            (ch >= L'0' && ch <= L'9') ||
            ch == L'-' ||
            ch == L'_';

        if (!allowed) {
            ch = L'_';
        }
    }

    while (!value.empty() &&
           (value.back() == L' ' || value.back() == L'.')) {
        value.pop_back();
    }

    if (value.empty()) {
        value = L"WindhawkSeparator-8F31A7D2";
    }

    // Keep generated paths and future XAML selectors reasonably short.
    constexpr size_t kMaxPrefixLength = 64;
    if (value.size() > kMaxPrefixLength) {
        value.resize(kMaxPrefixLength);
    }

    return value;
}

static std::wstring GetSeparatorBaseName(int ordinal) {
    return g_settings.identifierPrefix + L"-" + std::to_wstring(ordinal);
}

static std::wstring GetSeparatorShortcutPath(int ordinal) {
    return JoinPath(
        g_storagePath,
        GetSeparatorBaseName(ordinal) + L".lnk");
}

static std::wstring GetSeparatorAppId(int ordinal) {
    return std::wstring(kInternalAppIdPrefix) +
           L"." +
           std::to_wstring(ordinal);
}

static bool FileExists(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES &&
           !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool DeleteFileIfPresent(const std::wstring& path) {
    if (!FileExists(path)) {
        return true;
    }

    if (DeleteFileW(path.c_str())) {
        return true;
    }

    Wh_Log(
        L"[FILES] DeleteFileW('%s') failed error=%u",
        path.c_str(),
        GetLastError());

    return false;
}

static bool WriteBinaryFile(
    const std::wstring& path,
    const void* data,
    DWORD size) {
    HANDLE file = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(
            L"[FILES] CreateFileW('%s') failed error=%u",
            path.c_str(),
            GetLastError());
        return false;
    }

    DWORD written = 0;
    BOOL ok = WriteFile(
        file,
        data,
        size,
        &written,
        nullptr);

    DWORD error = ok ? ERROR_SUCCESS : GetLastError();
    CloseHandle(file);

    if (!ok || written != size) {
        Wh_Log(
            L"[FILES] WriteFile('%s') failed/incomplete "
            L"error=%u written=%u expected=%u",
            path.c_str(),
            error,
            written,
            size);
        return false;
    }

    return true;
}

static bool LoadSettings() {
    g_settings = {};

    PCWSTR prefixSetting = Wh_GetStringSetting(L"identifierPrefix");
    if (prefixSetting) {
        g_settings.identifierPrefix = SanitizePrefix(prefixSetting);
        Wh_FreeStringSetting(prefixSetting);
    } else {
        g_settings.identifierPrefix =
            L"WindhawkSeparator-8F31A7D2";
    }

    // Wh_GetIntSetting returns 0 for a missing setting.
    // Positions are therefore deliberately user-facing 1-based values.
    constexpr int kMaxSeparators = 64;

    for (int i = 0; i < kMaxSeparators; i++) {
        int position =
            Wh_GetIntSetting(L"separators[%d].index", i);

        if (position == 0) {
            break;
        }

        if (position < 1) {
            Wh_Log(
                L"[SETTINGS] Ignoring separator %d: invalid position=%d",
                i + 1,
                position);
            continue;
        }

        int width =
            Wh_GetIntSetting(L"separators[%d].width", i);
        int widthSmall =
            Wh_GetIntSetting(L"separators[%d].widthSmall", i);

        // Keep old settings snapshots usable if these fields didn't exist yet.
        if (width <= 0) {
            width = 14;
        }
        if (widthSmall <= 0) {
            widthSmall = 10;
        }

        g_settings.separators.push_back({
            .ordinal = i + 1,
            .targetIndex = position - 1,
            .width = static_cast<double>(width),
            .widthSmall = static_cast<double>(widthSmall),
        });
    }

    Wh_Log(
        L"[SETTINGS] prefix='%s' separators=%zu",
        g_settings.identifierPrefix.c_str(),
        g_settings.separators.size());

    for (const auto& separator : g_settings.separators) {
        Wh_Log(
            L"[SETTINGS] separator=%d targetIndex=%d (user position=%d) "
            L"width=%g widthSmall=%g",
            separator.ordinal,
            separator.targetIndex,
            separator.targetIndex + 1,
            separator.width,
            separator.widthSmall);
    }

    return true;
}

static bool InitializeStoragePath() {
    wchar_t buffer[32768] = {};

    size_t chars =
        Wh_GetModStoragePath(buffer, ARRAYSIZE(buffer));

    if (!chars || !buffer[0]) {
        Wh_Log(L"[FILES] Wh_GetModStoragePath failed");
        return false;
    }

    g_storagePath = buffer;

    // The API provides a usable mod storage directory, but this is harmless
    // if the directory already exists.
    if (!CreateDirectoryW(g_storagePath.c_str(), nullptr)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            Wh_Log(
                L"[FILES] CreateDirectoryW('%s') failed error=%u",
                g_storagePath.c_str(),
                error);
            return false;
        }
    }

    g_iconPath =
        JoinPath(
            g_storagePath,
            g_settings.identifierPrefix + L".ico");

    Wh_Log(L"[FILES] storage='%s'", g_storagePath.c_str());
    Wh_Log(L"[FILES] icon='%s'", g_iconPath.c_str());

    return true;
}


// -----------------------------------------------------------------------------
// TaskListButton width styling.
//
// This deliberately owns only our generated separator buttons. In particular,
// it does NOT override MediumTaskbarButtonExtent/SmallTaskbarButtonExtent
// resources globally.
//
// Compatibility with "Taskbar height and icon size":
// - that mod writes its global per-instance extents before calling its
//   UpdateVisualStates trampoline;
// - this mod calls its own trampoline first and applies the separator-specific
//   override only on the unwind path.
// Therefore our matched separator width wins regardless of hook nesting order,
// while all unmatched buttons remain completely untouched.
// -----------------------------------------------------------------------------

static void* g_taskListButtonUpdateButtonPaddingAddress;
static void* g_taskListButtonUpdateIconColumnDefinitionAddress;
static void* g_taskbarConfigurationGetIconHeightInViewPixelsAddress;

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
static TaskListButton_UpdateVisualStates_t
    g_taskListButtonUpdateVisualStatesOriginal;

static std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };

#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)

    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);

    static RtlQueryFeatureConfiguration_t queryFeatureConfiguration = [] {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        return ntdll
                   ? reinterpret_cast<RtlQueryFeatureConfiguration_t>(
                         GetProcAddress(ntdll, "RtlQueryFeatureConfiguration"))
                   : nullptr;
    }();

    if (!queryFeatureConfiguration) {
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {};
    INT64 changeStamp = 0;

    HRESULT hr = queryFeatureConfiguration(
        featureId,
        1,
        &changeStamp,
        &feature);

    if (FAILED(hr)) {
        return std::nullopt;
    }

    switch (feature.enabledState) {
        case FEATURE_ENABLED_STATE_DISABLED:
            return false;
        case FEATURE_ENABLED_STATE_ENABLED:
            return true;
        default:
            return std::nullopt;
    }
}

static LONG GetMediumTaskbarButtonExtentOffset() {
    static LONG offset = []() -> LONG {
        if (!g_taskListButtonUpdateIconColumnDefinitionAddress) {
            Wh_Log(
                L"[STYLE] UpdateIconColumnDefinition address unavailable");
            return 0;
        }

        // x64 logic from the upstream Taskbar height and icon size mod.
        // Search for a movsd that loads the per-instance medium extent,
        // followed by a subsd using the loaded value.
        const BYTE* start =
            reinterpret_cast<const BYTE*>(
                g_taskListButtonUpdateIconColumnDefinitionAddress);
        const BYTE* end = start + 0x200;

        LONG offsetCandidate = 0;
        LONG foundOffset = 0;

        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x10 &&
                (p[3] & 0xC0) == 0x80) {
                offsetCandidate =
                    *reinterpret_cast<const LONG*>(p + 4);
            }

            if (p[0] == 0xF2 && p[1] == 0x44 &&
                p[2] == 0x0F && p[3] == 0x10 &&
                (p[4] & 0xC0) == 0x80) {
                offsetCandidate =
                    *reinterpret_cast<const LONG*>(p + 5);
            }

            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x5C &&
                (p[3] & 0xC0) == 0x80) {
                foundOffset = offsetCandidate;
                break;
            }

            if (p[0] == 0xF2 && p[1] == 0x44 &&
                p[2] == 0x0F && p[3] == 0x5C &&
                (p[4] & 0xC0) == 0x80) {
                foundOffset = offsetCandidate;
                break;
            }
        }

        Wh_Log(
            L"[STYLE] mediumTaskbarButtonExtentOffset=0x%X",
            foundOffset);

        return (foundOffset < 0 || foundOffset > 0xFFFF)
                   ? 0
                   : foundOffset;
    }();

    return offset;
}

static FrameworkElement GetTaskListButtonElement(void* pThis) {
    FrameworkElement element = nullptr;

    // Same C++/WinRT subobject adjustment used by the existing taskbar mods.
    IUnknown* xamlUnknown =
        reinterpret_cast<IUnknown*>(pThis) + 3;

    HRESULT hr = xamlUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(element));

    if (FAILED(hr)) {
        return nullptr;
    }

    return element;
}

static const SeparatorSetting* FindSeparatorByAutomationName(
    std::wstring_view name) {
    if (g_settings.identifierPrefix.empty() || name.empty()) {
        return nullptr;
    }

    const std::wstring needle =
        g_settings.identifierPrefix + L"-";

    size_t pos = name.find(needle);

    while (pos != std::wstring_view::npos) {
        size_t digitPos = pos + needle.size();

        if (digitPos < name.size() &&
            name[digitPos] >= L'0' &&
            name[digitPos] <= L'9') {
            unsigned int ordinal = 0;
            size_t end = digitPos;

            while (end < name.size() &&
                   name[end] >= L'0' &&
                   name[end] <= L'9') {
                ordinal =
                    ordinal * 10 +
                    static_cast<unsigned int>(name[end] - L'0');

                if (ordinal > 100000) {
                    break;
                }

                end++;
            }

            for (const auto& separator : g_settings.separators) {
                if (separator.ordinal ==
                    static_cast<int>(ordinal)) {
                    return &separator;
                }
            }
        }

        pos = name.find(needle, pos + 1);
    }

    return nullptr;
}

static void RefreshTaskListButtonLayout(void* pThis) {
    // Call real function entry points, not our own trampolines.
    //
    // UpdateButtonPadding is intentionally invoked through its target address:
    // if Taskbar height and icon size hooks it, that hook participates.
    if (g_taskListButtonUpdateButtonPaddingAddress) {
        auto updateButtonPadding =
            reinterpret_cast<void(WINAPI*)(void*)>(
                g_taskListButtonUpdateButtonPaddingAddress);

        updateButtonPadding(pThis);
    }

    // Unlike the modified taskbar-icon-size experiment, our width write occurs
    // after UpdateVisualStates has already returned. Re-run the column update
    // explicitly so the XAML layout consumes the new extent immediately.
    if (g_taskListButtonUpdateIconColumnDefinitionAddress) {
        auto updateIconColumnDefinition =
            reinterpret_cast<void(WINAPI*)(void*)>(
                g_taskListButtonUpdateIconColumnDefinitionAddress);

        updateIconColumnDefinition(pThis);
    }
}

static void ApplySeparatorWidthOverride(void* pThis) {
    if (g_unloading ||
        !g_taskListButtonUpdateIconColumnDefinitionAddress) {
        return;
    }

    FrameworkElement element = GetTaskListButtonElement(pThis);
    if (!element) {
        return;
    }

    winrt::hstring automationName;

    try {
        automationName =
            winrt::Windows::UI::Xaml::Automation::
                AutomationProperties::GetName(element);
    } catch (...) {
        return;
    }

    std::wstring_view name{
        automationName.c_str(),
        automationName.size()
    };

    const SeparatorSetting* separator =
        FindSeparatorByAutomationName(name);

    if (!separator) {
        return;
    }

    LONG mediumOffset =
        GetMediumTaskbarButtonExtentOffset();

    if (!mediumOffset) {
        return;
    }

    auto* mediumExtent =
        reinterpret_cast<double*>(
            reinterpret_cast<BYTE*>(pThis) + mediumOffset);

    bool changed = false;

    if (*mediumExtent >= 1 &&
        *mediumExtent < 10000 &&
        *mediumExtent != separator->width) {
        Wh_Log(
            L"[STYLE] '%s' medium width %g -> %g",
            automationName.c_str(),
            *mediumExtent,
            separator->width);

        *mediumExtent = separator->width;
        changed = true;
    }

    // On DynamicIconScaling builds the small extent immediately precedes
    // the medium extent. Don't assume that layout on older taskbars.
    if (g_hasDynamicIconScaling) {
        double* smallExtent = mediumExtent - 1;

        if (*smallExtent >= 1 &&
            *smallExtent < 10000 &&
            *smallExtent != separator->widthSmall) {
            Wh_Log(
                L"[STYLE] '%s' small width %g -> %g",
                automationName.c_str(),
                *smallExtent,
                separator->widthSmall);

            *smallExtent = separator->widthSmall;
            changed = true;
        }
    }

    if (changed) {
        RefreshTaskListButtonLayout(pThis);
    }
}

static void WINAPI TaskListButton_UpdateVisualStates_Hook(
    void* pThis) {
    // Critical for coexistence with taskbar-icon-size:
    // let Windows and every hook downstream of us finish first.
    g_taskListButtonUpdateVisualStatesOriginal(pThis);

    // Then take the final word only for our own separator instances.
    ApplySeparatorWidthOverride(pThis);
}

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");

    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

static bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"
            },
            &g_taskListButtonUpdateButtonPaddingAddress,
            nullptr,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"
            },
            &g_taskListButtonUpdateIconColumnDefinitionAddress,
            nullptr,
            true, // Missing on older taskbar builds.
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"
            },
            &g_taskListButtonUpdateVisualStatesOriginal,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {
                LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"
            },
            &g_taskbarConfigurationGetIconHeightInViewPixelsAddress,
            nullptr,
            true, // DynamicIconScaling-era builds.
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            hooks,
            ARRAYSIZE(hooks))) {
        Wh_Log(L"[STYLE] HookSymbols(Taskbar view) failed");
        return false;
    }

    if (g_taskListButtonUpdateIconColumnDefinitionAddress) {
        GetMediumTaskbarButtonExtentOffset();
    }

    constexpr UINT kDynamicIconScaling = 29785184;

    g_hasDynamicIconScaling =
        g_taskbarConfigurationGetIconHeightInViewPixelsAddress &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true);

    Wh_Log(
        L"[STYLE] Taskbar view hooks ready; "
        L"DynamicIconScaling=%d",
        g_hasDynamicIconScaling);

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_loadLibraryExWOriginal;

static HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags) {
    HMODULE module =
        g_loadLibraryExWOriginal(
            lpLibFileName,
            hFile,
            dwFlags);

    if (!module) {
        return module;
    }

    if (!g_taskbarViewDllHooked &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllHooked.exchange(true)) {
        Wh_Log(
            L"[STYLE] Taskbar view module loaded: %s",
            lpLibFileName ? lpLibFileName : L"<unknown>");

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        } else {
            g_taskbarViewDllHooked = false;
        }
    }

    return module;
}

static bool InitializeTaskbarStylingHooks() {
    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllHooked = true;

        if (!HookTaskbarViewDllSymbols(module)) {
            g_taskbarViewDllHooked = false;
            return false;
        }

        return true;
    }

    Wh_Log(
        L"[STYLE] Taskbar view module not loaded yet; "
        L"installing LoadLibraryExW watcher");

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        return false;
    }

    auto loadLibraryExW =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));

    if (!loadLibraryExW) {
        return false;
    }

    WindhawkUtils::SetFunctionHook(
        loadLibraryExW,
        LoadLibraryExW_Hook,
        &g_loadLibraryExWOriginal);

    return true;
}


// -----------------------------------------------------------------------------
// Shortcut creation.
// -----------------------------------------------------------------------------

static HRESULT SetShortcutAppId(
    IShellLinkW* shellLink,
    const std::wstring& appId) {
    IPropertyStore* propertyStore = nullptr;

    HRESULT hr = shellLink->QueryInterface(
        IID_PPV_ARGS(&propertyStore));

    if (FAILED(hr)) {
        return hr;
    }

    PROPVARIANT value;
    PropVariantInit(&value);

    hr = InitPropVariantFromString(
        appId.c_str(),
        &value);

    if (SUCCEEDED(hr)) {
        hr = propertyStore->SetValue(
            PKEY_AppUserModel_ID,
            value);
    }

    if (SUCCEEDED(hr)) {
        hr = propertyStore->Commit();
    }

    PropVariantClear(&value);
    propertyStore->Release();

    return hr;
}

static HRESULT CreateSeparatorShortcut(
    int ordinal,
    const std::wstring& shortcutPath) {
    IShellLinkW* shellLink = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_ShellLink,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&shellLink));

    if (FAILED(hr)) {
        return hr;
    }

    wchar_t systemDirectory[MAX_PATH] = {};

    UINT systemDirectoryLength =
        GetSystemDirectoryW(
            systemDirectory,
            ARRAYSIZE(systemDirectory));

    if (!systemDirectoryLength ||
        systemDirectoryLength >= ARRAYSIZE(systemDirectory)) {
        shellLink->Release();
        return HRESULT_FROM_WIN32(GetLastError());
    }

    std::wstring target =
        JoinPath(systemDirectory, L"cmd.exe");

    // If the raw test button is accidentally clicked before XAML styling is
    // added, launch a hidden cmd.exe that exits immediately.
    hr = shellLink->SetPath(target.c_str());

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetArguments(L"/d /c exit");
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetShowCmd(SW_HIDE);
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetIconLocation(
            g_iconPath.c_str(),
            0);
    }

    std::wstring description =
        GetSeparatorBaseName(ordinal);

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetDescription(
            description.c_str());
    }

    // Give every separator a distinct AppUserModelID so the Shell considers
    // them distinct taskbar identities even though they share cmd.exe as the
    // harmless launch target.
    if (SUCCEEDED(hr)) {
        hr = SetShortcutAppId(
            shellLink,
            GetSeparatorAppId(ordinal));
    }

    if (SUCCEEDED(hr)) {
        IPersistFile* persistFile = nullptr;

        hr = shellLink->QueryInterface(
            IID_PPV_ARGS(&persistFile));

        if (SUCCEEDED(hr)) {
            hr = persistFile->Save(
                shortcutPath.c_str(),
                TRUE);

            persistFile->Release();
        }
    }

    shellLink->Release();

    Wh_Log(
        L"[FILES] Create shortcut #%d '%s' hr=0x%08X",
        ordinal,
        shortcutPath.c_str(),
        static_cast<unsigned int>(hr));

    return hr;
}

// -----------------------------------------------------------------------------
// PIDL + PinManager helpers.
// -----------------------------------------------------------------------------

static HRESULT GetPidlForPath(
    const std::wstring& path,
    PIDLIST_ABSOLUTE* pidl) {
    *pidl = nullptr;

    SFGAOF attrs = 0;

    HRESULT hr = SHParseDisplayName(
        path.c_str(),
        nullptr,
        pidl,
        0,
        &attrs);

    Wh_Log(
        L"[PIDL] '%s' -> hr=0x%08X pidl=%p",
        path.c_str(),
        static_cast<unsigned int>(hr),
        *pidl);

    return hr;
}

static HRESULT CreatePinManager(
    IPinManagerInterop3** pinManager) {
    *pinManager = nullptr;

    IPinManagerInterop2* interop2 = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_PinManager,
        nullptr,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_IPinManagerInterop2,
        reinterpret_cast<void**>(&interop2));

    if (FAILED(hr)) {
        Wh_Log(
            L"[PIN] CoCreateInstance(IPinManagerInterop2) "
            L"failed hr=0x%08X",
            static_cast<unsigned int>(hr));
        return hr;
    }

    hr = interop2->QueryInterface(
        IID_IPinManagerInterop3,
        reinterpret_cast<void**>(pinManager));

    interop2->Release();

    Wh_Log(
        L"[PIN] QueryInterface(IPinManagerInterop3) "
        L"hr=0x%08X ptr=%p",
        static_cast<unsigned int>(hr),
        *pinManager);

    return hr;
}

// -----------------------------------------------------------------------------
// Creation / positioning.
// -----------------------------------------------------------------------------

static bool CreateAndPinSeparators() {
    if (g_settings.separators.empty()) {
        Wh_Log(L"[PIN] No separators configured");
        return true;
    }

    if (!WriteBinaryFile(
            g_iconPath,
            kSeparatorIcon,
            static_cast<DWORD>(sizeof(kSeparatorIcon)))) {
        return false;
    }

    // First create every backing shortcut, before mutating the taskbar.
    for (const auto& separator : g_settings.separators) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        HRESULT hr =
            CreateSeparatorShortcut(
                separator.ordinal,
                shortcutPath);

        if (FAILED(hr)) {
            Wh_Log(
                L"[PIN] Failed to create separator #%d",
                separator.ordinal);
            return false;
        }
    }

    IPinManagerInterop3* pinManager = nullptr;

    HRESULT hr = CreatePinManager(&pinManager);
    if (FAILED(hr) || !pinManager) {
        return false;
    }

    // Pin all shortcuts first. PinItemFromTrustedCaller is the modern working
    // path on this build; PinItemToTaskbarShim was observed to be a no-op.
    bool success = true;

    for (const auto& separator : g_settings.separators) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            success = false;
            continue;
        }

        Wh_Log(
            L"[PIN] Pin separator #%d",
            separator.ordinal);

        HRESULT pinHr =
            pinManager->PinItemFromTrustedCaller(
                pidl,
                PMC_TASKBANDPIN);

        Wh_Log(
            L"[PIN] PinItemFromTrustedCaller #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(pinHr));

        if (FAILED(pinHr)) {
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    // Moving in ascending destination order makes multiple requested positions
    // deterministic as items are pulled forward from their initial appended
    // locations.
    std::vector<SeparatorSetting> moveOrder =
        g_settings.separators;

    std::stable_sort(
        moveOrder.begin(),
        moveOrder.end(),
        [](const SeparatorSetting& a, const SeparatorSetting& b) {
            return a.targetIndex < b.targetIndex;
        });

    for (const auto& separator : moveOrder) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            success = false;
            continue;
        }

        Wh_Log(
            L"[MOVE] separator #%d -> index=%d",
            separator.ordinal,
            separator.targetIndex);

        HRESULT moveHr =
            pinManager->MoveTaskbarPin(
                pidl,
                separator.targetIndex,
                PMC_TASKBANDREORDER);

        Wh_Log(
            L"[MOVE] MoveTaskbarPin #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(moveHr));

        if (FAILED(moveHr)) {
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    pinManager->Release();

    return success;
}

// -----------------------------------------------------------------------------
// Destruction / cleanup.
// -----------------------------------------------------------------------------

static bool UnpinAndDeleteSeparators() {
    if (g_settings.separators.empty()) {
        DeleteFileIfPresent(g_iconPath);
        return true;
    }

    IPinManagerInterop3* pinManager = nullptr;

    HRESULT hr = CreatePinManager(&pinManager);

    if (FAILED(hr) || !pinManager) {
        Wh_Log(
            L"[CLEANUP] Can't acquire PinManager; "
            L"leaving backing files in place to avoid broken pins");
        return false;
    }

    bool allUnpinned = true;

    // Reverse order isn't required because unpinning is PIDL-based, but it
    // minimizes visual/index churn while several separators disappear.
    for (auto it = g_settings.separators.rbegin();
         it != g_settings.separators.rend();
         ++it) {
        const auto& separator = *it;

        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            Wh_Log(
                L"[CLEANUP] Can't resolve separator #%d; "
                L"keeping its shortcut",
                separator.ordinal);
            allUnpinned = false;
            continue;
        }

        Wh_Log(
            L"[CLEANUP] Unpin separator #%d",
            separator.ordinal);

        // 11 is the exact caller value observed during the native modern
        // Notepad unpin trace on this build, and was already tested with
        // UnpinTaskbarItem successfully.
        HRESULT unpinHr =
            pinManager->UnpinTaskbarItem(
                pidl,
                PMC_JUMPVIEWBROKER);

        Wh_Log(
            L"[CLEANUP] UnpinTaskbarItem #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(unpinHr));

        CoTaskMemFree(pidl);

        if (SUCCEEDED(unpinHr)) {
            DeleteFileIfPresent(shortcutPath);
        } else {
            allUnpinned = false;
        }
    }

    pinManager->Release();

    if (allUnpinned) {
        DeleteFileIfPresent(g_iconPath);
    } else {
        Wh_Log(
            L"[CLEANUP] At least one unpin failed; "
            L"keeping shared icon '%s'",
            g_iconPath.c_str());
    }

    return allUnpinned;
}

// -----------------------------------------------------------------------------
// Windhawk lifetime.
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[INIT] Taskbar separator prototype loading");

    LoadSettings();

    if (!InitializeStoragePath()) {
        return FALSE;
    }

    if (!InitializeTaskbarStylingHooks()) {
        Wh_Log(
            L"[INIT] Failed to initialize TaskListButton styling hooks");
        return FALSE;
    }

    // Hook operations installed during Wh_ModInit are applied automatically
    // before Wh_ModAfterInit. Create the actual pins there so their first
    // UpdateVisualStates pass can already be intercepted.
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[INIT] Taskbar separator backend starting");

    // Close the small race where the module wasn't present in Wh_ModInit but
    // appeared before/around Wh_ModAfterInit.
    if (!g_taskbarViewDllHooked) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllHooked.exchange(true)) {
                if (HookTaskbarViewDllSymbols(module)) {
                    Wh_ApplyHookOperations();
                } else {
                    g_taskbarViewDllHooked = false;
                }
            }
        }
    }

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(hrInit);

    if (hrInit == RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"[INIT] COM already initialized with another "
            L"apartment model; continuing");
    } else if (FAILED(hrInit)) {
        Wh_Log(
            L"[INIT] CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(hrInit));
        return;
    }

    bool success =
        CreateAndPinSeparators();

    if (shouldUninitialize) {
        CoUninitialize();
    }

    if (!success) {
        Wh_Log(
            L"[INIT] One or more prototype operations failed; "
            L"mod remains loaded so unload can still clean up");
    }

    Wh_Log(L"[INIT] Taskbar separator prototype loaded");
}

void Wh_ModBeforeUninit() {
    // Stop re-applying widths while Windhawk is unwinding hooks and while the
    // backing taskbar items are about to be removed.
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L"[UNINIT] Taskbar separator prototype unloading");

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(hrInit);

    if (hrInit == RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"[UNINIT] COM already initialized with another "
            L"apartment model; continuing");
    } else if (FAILED(hrInit)) {
        Wh_Log(
            L"[UNINIT] CoInitializeEx failed hr=0x%08X; "
            L"can't safely remove separators",
            static_cast<unsigned int>(hrInit));
        return;
    }

    UnpinAndDeleteSeparators();

    if (shouldUninitialize) {
        CoUninitialize();
    }

    Wh_Log(L"[UNINIT] Taskbar separator prototype unloaded");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // Keep the old loaded settings intact. The old instance must unpin/delete
    // exactly the separator identities it created; the reloaded instance then
    // reads the new array (including new widths) and recreates fresh buttons.
    *bReload = TRUE;
    return TRUE;
}
