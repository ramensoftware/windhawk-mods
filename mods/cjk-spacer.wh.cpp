// ==WindhawkMod==
// @id              cjk-spacer
// @name            CJK Spacer
// @description     Add spaces between CJK characters and letters or digits in Explorer menus and tooltips
// @version         0.1.1
// @author          aenerv7
// @github          https://github.com/aenerv7
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0.
//
// The Win32 menu and DirectWrite hook techniques are based in part on the
// open-source Windhawk "Text Replace" mod by m417z.

// ==WindhawkModReadme==
/*
# CJK Spacer

Adds a normal ASCII space at a direct boundary between a CJK character and a
letter or digit in UI text hosted by `explorer.exe`.

Examples:

- `使用VS Code打开` becomes `使用 VS Code 打开`
- `压缩为ZIP文件` becomes `压缩为 ZIP 文件`
- `使用 VS Code 打开` is unchanged
- `打开(&O)` is unchanged

The transformation is idempotent. Punctuation and existing whitespace break a
boundary and are preserved. Win32 `&` mnemonic markers and tab-separated
keyboard shortcut text are also preserved.

## Supported UI

- Classic Win32 popup menus are rewritten through public `HMENU` APIs. This
  includes File Explorer, desktop, taskbar, tray, and jump-list menus hosted by
  `explorer.exe`.
- Windows 11 context menus and XAML/WinUI tooltips use an experimental,
  display-only DirectWrite hook. UI Automation is used when a popup is shown to
  distinguish menus and tooltips from other Explorer flyouts.

The modern path is intentionally conservative and can miss text if a Windows
build exposes a different popup accessibility tree. Disable `modernUiText` if
it causes a problem.

The mod doesn't edit system files, registry values, or file names. The modern
DirectWrite path changes display text only. The classic path updates strings
stored in `HMENU` objects, so menu text read through accessibility APIs also
contains the inserted spaces. Resource-loaded classic menus can retain those
strings after the mod is disabled until Explorer restarts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- classicMenus: true
  $name: Classic context menus
  $description: Process classic Win32 popup-menu text.
- modernUiText: true
  $name: Windows 11 menus and tooltips (experimental)
  $description: Process DirectWrite text in Explorer XAML/WinUI context menus and tooltips.
- characterMode: unicode
  $name: Non-CJK character set
  $description: Choose which letters and digits form a spacing boundary with CJK.
  $options:
  - unicode: Unicode letters and digits
  - ascii: ASCII letters and digits only
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <cstdint>
#include <cwchar>
#include <string>
#include <string_view>
#include <vector>

#include <dwrite.h>
#include <initguid.h>
#include <uiautomation.h>
#include <windows.h>

namespace {

enum class CharacterKind {
    Other,
    Cjk,
    Word,
    Extend,
};

std::atomic_bool g_classicMenus{true};
std::atomic_bool g_modernUiText{true};
std::atomic_bool g_unicodeLettersAndDigits{true};
std::atomic_uint g_activeModernPopupCount{0};

bool IsHighSurrogate(wchar_t value) {
    return value >= 0xD800 && value <= 0xDBFF;
}

bool IsLowSurrogate(wchar_t value) {
    return value >= 0xDC00 && value <= 0xDFFF;
}

uint32_t DecodeCodePoint(std::wstring_view text,
                         size_t offset,
                         size_t* codeUnitCount) {
    const uint32_t first = static_cast<uint16_t>(text[offset]);
    if (IsHighSurrogate(text[offset]) && offset + 1 < text.size() &&
        IsLowSurrogate(text[offset + 1])) {
        const uint32_t second = static_cast<uint16_t>(text[offset + 1]);
        *codeUnitCount = 2;
        return 0x10000 + ((first - 0xD800) << 10) + (second - 0xDC00);
    }

    *codeUnitCount = 1;
    return first;
}

bool IsInRange(uint32_t codePoint, uint32_t first, uint32_t last) {
    return codePoint >= first && codePoint <= last;
}

bool IsCjkCodePoint(uint32_t codePoint) {
    // Hangul Jamo.
    if (IsInRange(codePoint, 0x1100, 0x11FF)) {
        return true;
    }

    // CJK radicals, Kangxi radicals, kana, Bopomofo, Hangul compatibility
    // Jamo, CJK strokes, and Katakana phonetic extensions. CJK punctuation is
    // deliberately excluded except for the ideographic iteration mark and
    // ideographic zero.
    if (IsInRange(codePoint, 0x2E80, 0x2FDF) ||
        codePoint == 0x3005 || codePoint == 0x3007 ||
        IsInRange(codePoint, 0x3040, 0x30FF) ||
        IsInRange(codePoint, 0x3100, 0x318F) ||
        IsInRange(codePoint, 0x31A0, 0x31BF) ||
        IsInRange(codePoint, 0x31C0, 0x31EF) ||
        IsInRange(codePoint, 0x31F0, 0x31FF)) {
        return true;
    }

    // Unified ideographs, Extension A, and compatibility ideographs.
    if (IsInRange(codePoint, 0x3400, 0x4DBF) ||
        IsInRange(codePoint, 0x4E00, 0x9FFF) ||
        IsInRange(codePoint, 0xF900, 0xFAFF)) {
        return true;
    }

    // Hangul extensions and syllables.
    if (IsInRange(codePoint, 0xA960, 0xA97F) ||
        IsInRange(codePoint, 0xAC00, 0xD7AF) ||
        IsInRange(codePoint, 0xD7B0, 0xD7FF)) {
        return true;
    }

    // Halfwidth Katakana.
    if (IsInRange(codePoint, 0xFF66, 0xFF9D)) {
        return true;
    }

    // Kana Supplement, Kana Extended-A, and Small Kana Extension.
    if (IsInRange(codePoint, 0x1B000, 0x1B16F)) {
        return true;
    }

    // Supplementary CJK unified and compatibility ideographs. The broad
    // ranges intentionally include reserved gaps so newly assigned Han
    // characters continue to behave sensibly.
    return IsInRange(codePoint, 0x20000, 0x2FA1F) ||
           IsInRange(codePoint, 0x30000, 0x323AF);
}

bool IsExtendingCodePoint(uint32_t codePoint) {
    // Combining marks and variation selectors stay attached to the preceding
    // base character and don't interrupt a CJK/word boundary.
    return IsInRange(codePoint, 0x0300, 0x036F) ||
           IsInRange(codePoint, 0x1AB0, 0x1AFF) ||
           IsInRange(codePoint, 0x1DC0, 0x1DFF) ||
           IsInRange(codePoint, 0x20D0, 0x20FF) ||
           IsInRange(codePoint, 0xFE00, 0xFE0F) ||
           IsInRange(codePoint, 0xFE20, 0xFE2F) ||
           IsInRange(codePoint, 0xFF9E, 0xFF9F) ||
           IsInRange(codePoint, 0xE0100, 0xE01EF);
}

bool IsAsciiLetterOrDigit(uint32_t codePoint) {
    return (codePoint >= L'0' && codePoint <= L'9') ||
           (codePoint >= L'A' && codePoint <= L'Z') ||
           (codePoint >= L'a' && codePoint <= L'z');
}

bool IsUnicodeLetterOrDigit(uint32_t codePoint) {
    // Fullwidth Latin letters and digits already have ideographic advance
    // widths, so inserting an extra ASCII space is typographically redundant.
    if (IsInRange(codePoint, 0xFF10, 0xFF19) ||
        IsInRange(codePoint, 0xFF21, 0xFF3A) ||
        IsInRange(codePoint, 0xFF41, 0xFF5A)) {
        return false;
    }

    wchar_t utf16[2];
    int length;

    if (codePoint <= 0xFFFF) {
        utf16[0] = static_cast<wchar_t>(codePoint);
        length = 1;
    } else if (codePoint <= 0x10FFFF) {
        const uint32_t value = codePoint - 0x10000;
        utf16[0] = static_cast<wchar_t>(0xD800 + (value >> 10));
        utf16[1] = static_cast<wchar_t>(0xDC00 + (value & 0x3FF));
        length = 2;
    } else {
        return false;
    }

    WORD characterTypes[2] = {};
    if (!GetStringTypeW(CT_CTYPE1, utf16, length, characterTypes)) {
        return false;
    }

    for (int i = 0; i < length; ++i) {
        if (characterTypes[i] & (C1_ALPHA | C1_DIGIT)) {
            return true;
        }
    }

    return false;
}

CharacterKind ClassifyCodePoint(uint32_t codePoint) {
    if (IsExtendingCodePoint(codePoint)) {
        return CharacterKind::Extend;
    }

    if (IsCjkCodePoint(codePoint)) {
        return CharacterKind::Cjk;
    }

    const bool isWord =
        g_unicodeLettersAndDigits.load(std::memory_order_relaxed)
            ? IsUnicodeLetterOrDigit(codePoint)
            : IsAsciiLetterOrDigit(codePoint);
    return isWord ? CharacterKind::Word : CharacterKind::Other;
}

bool NeedsSpace(CharacterKind left, CharacterKind right) {
    return (left == CharacterKind::Cjk && right == CharacterKind::Word) ||
           (left == CharacterKind::Word && right == CharacterKind::Cjk);
}

// A token is a base code point, its combining marks/variation selectors, and
// an optional Win32 mnemonic '&' prefix. Keeping the prefix in the token makes
// "打开&Open" become "打开 &Open", which Windows displays as "打开 Open"
// while preserving the O mnemonic.
bool ContainsCjkCodePoint(std::wstring_view text) {
    size_t offset = 0;
    while (offset < text.size()) {
        size_t codeUnitCount;
        const uint32_t codePoint =
            DecodeCodePoint(text, offset, &codeUnitCount);
        if (IsCjkCodePoint(codePoint)) {
            return true;
        }
        offset += codeUnitCount;
    }

    return false;
}

std::wstring AddCjkSpacing(std::wstring_view text,
                           bool preserveMnemonics = true) {
    std::wstring result;
    result.reserve(text.size() + 4);

    CharacterKind previousKind = CharacterKind::Other;
    size_t offset = 0;

    while (offset < text.size()) {
        const size_t tokenStart = offset;

        if (preserveMnemonics && text[offset] == L'&') {
            if (offset + 1 < text.size() && text[offset + 1] == L'&') {
                // An escaped ampersand is visible punctuation.
                result.append(text.substr(offset, 2));
                offset += 2;
                previousKind = CharacterKind::Other;
                continue;
            }

            if (offset + 1 < text.size()) {
                ++offset;
            }
        }

        size_t codeUnitCount;
        const uint32_t codePoint =
            DecodeCodePoint(text, offset, &codeUnitCount);
        CharacterKind currentKind = ClassifyCodePoint(codePoint);
        offset += codeUnitCount;

        // Attach combining characters and variation selectors to this token.
        while (offset < text.size()) {
            size_t extensionLength;
            const uint32_t extension =
                DecodeCodePoint(text, offset, &extensionLength);
            if (!IsExtendingCodePoint(extension)) {
                break;
            }
            offset += extensionLength;
        }

        if (currentKind == CharacterKind::Extend) {
            // A malformed standalone combining mark doesn't reset the previous
            // visible character's classification.
            result.append(text.substr(tokenStart, offset - tokenStart));
            continue;
        }

        if (NeedsSpace(previousKind, currentKind)) {
            result.push_back(L' ');
        }

        result.append(text.substr(tokenStart, offset - tokenStart));
        previousKind = currentKind;
    }

    return result;
}

// -------------------------------------------------------------------------
// Classic Win32 menus
// -------------------------------------------------------------------------

using InsertMenuW_t = decltype(&InsertMenuW);
using AppendMenuW_t = decltype(&AppendMenuW);
using ModifyMenuW_t = decltype(&ModifyMenuW);
using InsertMenuItemW_t = decltype(&InsertMenuItemW);
using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
using TrackPopupMenu_t = decltype(&TrackPopupMenu);
using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);

InsertMenuW_t g_originalInsertMenuW;
AppendMenuW_t g_originalAppendMenuW;
ModifyMenuW_t g_originalModifyMenuW;
InsertMenuItemW_t g_originalInsertMenuItemW;
SetMenuItemInfoW_t g_originalSetMenuItemInfoW;
TrackPopupMenu_t g_originalTrackPopupMenu;
TrackPopupMenuEx_t g_originalTrackPopupMenuEx;

bool IsStringMenuFlags(UINT flags) {
    return !(flags & (MF_BITMAP | MF_OWNERDRAW | MF_SEPARATOR));
}

BOOL WINAPI InsertMenuWHook(HMENU menu,
                            UINT position,
                            UINT flags,
                            UINT_PTR itemId,
                            LPCWSTR newItem) {
    if (g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags)) {
        const std::wstring spaced = AddCjkSpacing(newItem);
        if (spaced != newItem) {
            Wh_Log(L"Applied CJK spacing (classic/InsertMenuW)");
            return g_originalInsertMenuW(menu, position, flags, itemId,
                                         spaced.c_str());
        }
    }

    return g_originalInsertMenuW(menu, position, flags, itemId, newItem);
}

BOOL WINAPI AppendMenuWHook(HMENU menu,
                            UINT flags,
                            UINT_PTR itemId,
                            LPCWSTR newItem) {
    if (g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags)) {
        const std::wstring spaced = AddCjkSpacing(newItem);
        if (spaced != newItem) {
            Wh_Log(L"Applied CJK spacing (classic/AppendMenuW)");
            return g_originalAppendMenuW(menu, flags, itemId, spaced.c_str());
        }
    }

    return g_originalAppendMenuW(menu, flags, itemId, newItem);
}

BOOL WINAPI ModifyMenuWHook(HMENU menu,
                            UINT position,
                            UINT flags,
                            UINT_PTR itemId,
                            LPCWSTR newItem) {
    if (g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags)) {
        const std::wstring spaced = AddCjkSpacing(newItem);
        if (spaced != newItem) {
            Wh_Log(L"Applied CJK spacing (classic/ModifyMenuW)");
            return g_originalModifyMenuW(menu, position, flags, itemId,
                                         spaced.c_str());
        }
    }

    return g_originalModifyMenuW(menu, position, flags, itemId, newItem);
}

bool MenuItemInfoContainsString(const MENUITEMINFOW* itemInfo) {
    if (!itemInfo || !itemInfo->dwTypeData) {
        return false;
    }

    if ((itemInfo->fMask & (MIIM_FTYPE | MIIM_TYPE)) &&
        (itemInfo->fType & (MFT_BITMAP | MFT_OWNERDRAW | MFT_SEPARATOR))) {
        return false;
    }

    return (itemInfo->fMask & MIIM_STRING) ||
           (itemInfo->fMask & MIIM_TYPE);
}

BOOL WINAPI InsertMenuItemWHook(HMENU menu,
                                UINT item,
                                BOOL byPosition,
                                LPCMENUITEMINFOW itemInfo) {
    if (g_classicMenus.load(std::memory_order_relaxed) &&
        MenuItemInfoContainsString(itemInfo)) {
        const std::wstring spaced = AddCjkSpacing(itemInfo->dwTypeData);
        if (spaced != itemInfo->dwTypeData) {
            Wh_Log(L"Applied CJK spacing (classic/InsertMenuItemW)");
            MENUITEMINFOW copy = *itemInfo;
            copy.dwTypeData = const_cast<wchar_t*>(spaced.c_str());
            copy.cch = static_cast<UINT>(spaced.size());
            return g_originalInsertMenuItemW(menu, item, byPosition, &copy);
        }
    }

    return g_originalInsertMenuItemW(menu, item, byPosition, itemInfo);
}

BOOL WINAPI SetMenuItemInfoWHook(HMENU menu,
                                 UINT item,
                                 BOOL byPosition,
                                 LPCMENUITEMINFOW itemInfo) {
    if (g_classicMenus.load(std::memory_order_relaxed) &&
        MenuItemInfoContainsString(itemInfo)) {
        const std::wstring spaced = AddCjkSpacing(itemInfo->dwTypeData);
        if (spaced != itemInfo->dwTypeData) {
            Wh_Log(L"Applied CJK spacing (classic/SetMenuItemInfoW)");
            MENUITEMINFOW copy = *itemInfo;
            copy.dwTypeData = const_cast<wchar_t*>(spaced.c_str());
            copy.cch = static_cast<UINT>(spaced.size());
            return g_originalSetMenuItemInfoW(menu, item, byPosition, &copy);
        }
    }

    return g_originalSetMenuItemInfoW(menu, item, byPosition, itemInfo);
}

void RewriteMenuTree(HMENU menu) {
    const int itemCount = GetMenuItemCount(menu);
    for (int index = 0; index < itemCount; ++index) {
        MENUITEMINFOW itemInfo = {};
        itemInfo.cbSize = sizeof(itemInfo);
        itemInfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;

        if (!GetMenuItemInfoW(menu, index, TRUE, &itemInfo)) {
            continue;
        }

        const bool isTextItem =
            !(itemInfo.fType &
              (MFT_BITMAP | MFT_OWNERDRAW | MFT_SEPARATOR));

        if (isTextItem && itemInfo.cch > 0) {
            std::vector<wchar_t> buffer(itemInfo.cch + 1);
            itemInfo.dwTypeData = buffer.data();
            itemInfo.cch = static_cast<UINT>(buffer.size());

            if (GetMenuItemInfoW(menu, index, TRUE, &itemInfo)) {
                const std::wstring original(buffer.data(), itemInfo.cch);
                const std::wstring spaced = AddCjkSpacing(original);
                if (spaced != original) {
                    Wh_Log(L"Applied CJK spacing (classic/pre-display)");

                    MENUITEMINFOW replacement = {};
                    replacement.cbSize = sizeof(replacement);
                    replacement.fMask = MIIM_STRING;
                    replacement.dwTypeData =
                        const_cast<wchar_t*>(spaced.c_str());
                    replacement.cch = static_cast<UINT>(spaced.size());
                    SetMenuItemInfoW(menu, index, TRUE, &replacement);
                }
            }
        }

        if (itemInfo.hSubMenu) {
            RewriteMenuTree(itemInfo.hSubMenu);
        }
    }
}

BOOL WINAPI TrackPopupMenuHook(HMENU menu,
                               UINT flags,
                               int x,
                               int y,
                               int reserved,
                               HWND owner,
                               const RECT* rect) {
    if (g_classicMenus.load(std::memory_order_relaxed)) {
        RewriteMenuTree(menu);
    }

    return g_originalTrackPopupMenu(menu, flags, x, y, reserved, owner, rect);
}

BOOL WINAPI TrackPopupMenuExHook(HMENU menu,
                                 UINT flags,
                                 int x,
                                 int y,
                                 HWND owner,
                                 LPTPMPARAMS parameters) {
    if (g_classicMenus.load(std::memory_order_relaxed)) {
        RewriteMenuTree(menu);
    }

    return g_originalTrackPopupMenuEx(menu, flags, x, y, owner, parameters);
}

// -------------------------------------------------------------------------
// Windows 11 XAML/WinUI menu and tooltip detection
// -------------------------------------------------------------------------

using ShowWindow_t = decltype(&ShowWindow);
using SetWindowPos_t = decltype(&SetWindowPos);
using DestroyWindow_t = decltype(&DestroyWindow);

ShowWindow_t g_originalShowWindow;
SetWindowPos_t g_originalSetWindowPos;
DestroyWindow_t g_originalDestroyWindow;

enum class ModernPopupKind : ULONG_PTR {
    Unclassified = 0,
    Menu = 1,
    ToolTip = 2,
};

constexpr wchar_t kModernPopupKindProperty[] =
    L"CJKSpacer_ModernPopupKind";

thread_local bool g_detectingModernPopupKind = false;

bool IsModernPopupClassName(LPCWSTR className) {
    if (!className ||
        reinterpret_cast<ULONG_PTR>(className) <= 0xFFFF) {
        return false;
    }

    return _wcsicmp(className, L"Xaml_WindowedPopupClass") == 0 ||
           _wcsicmp(className,
                    L"Microsoft.UI.Content.PopupWindowSiteBridge") == 0;
}

bool IsModernPopupWindow(HWND window) {
    wchar_t className[128];
    const int length = GetClassNameW(window, className, ARRAYSIZE(className));
    return length > 0 && IsModernPopupClassName(className);
}

template <typename Interface>
void ReleaseComInterface(Interface*& value) {
    if (value) {
        value->Release();
        value = nullptr;
    }
}

bool PopupContainsControlType(IUIAutomation* automation,
                              IUIAutomationElement* root,
                              CONTROLTYPEID controlType) {
    VARIANT propertyValue = {};
    propertyValue.vt = VT_I4;
    propertyValue.lVal = controlType;

    IUIAutomationCondition* condition = nullptr;
    HRESULT result = automation->CreatePropertyCondition(
        UIA_ControlTypePropertyId, propertyValue, &condition);
    if (FAILED(result) || !condition) {
        return false;
    }

    IUIAutomationElement* match = nullptr;
    result = root->FindFirst(TreeScope_Subtree, condition, &match);
    const bool found = SUCCEEDED(result) && match;
    ReleaseComInterface(condition);
    ReleaseComInterface(match);
    return found;
}

ModernPopupKind DetectModernPopupKind(HWND window) {
    if (g_detectingModernPopupKind) {
        return ModernPopupKind::Unclassified;
    }

    g_detectingModernPopupKind = true;

    CO_MTA_USAGE_COOKIE mtaCookie;
    const bool mtaUsageIncreased =
        SUCCEEDED(CoIncrementMTAUsage(&mtaCookie));

    IUIAutomation* automation = nullptr;
    IUIAutomationElement* root = nullptr;
    ModernPopupKind kind = ModernPopupKind::Unclassified;

    HRESULT result = CoCreateInstance(
        CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&automation));
    if (SUCCEEDED(result) && automation) {
        result = automation->ElementFromHandle(window, &root);
    }

    if (SUCCEEDED(result) && root) {
        if (PopupContainsControlType(automation, root,
                                     UIA_MenuControlTypeId)) {
            kind = ModernPopupKind::Menu;
        } else if (PopupContainsControlType(
                       automation, root, UIA_ToolTipControlTypeId)) {
            kind = ModernPopupKind::ToolTip;
        }
    }

    ReleaseComInterface(root);
    ReleaseComInterface(automation);

    if (mtaUsageIncreased) {
        CoDecrementMTAUsage(mtaCookie);
    }

    g_detectingModernPopupKind = false;
    return kind;
}

ModernPopupKind GetTrackedModernPopupKind(HWND window) {
    return static_cast<ModernPopupKind>(
        reinterpret_cast<ULONG_PTR>(
            GetPropW(window, kModernPopupKindProperty)));
}

bool IsSupportedModernPopupKind(ModernPopupKind kind) {
    return kind == ModernPopupKind::Menu ||
           kind == ModernPopupKind::ToolTip;
}

bool TrackModernPopup(HWND window) {
    if (GetTrackedModernPopupKind(window) !=
        ModernPopupKind::Unclassified) {
        return true;
    }

    const ModernPopupKind kind = DetectModernPopupKind(window);
    if (!IsSupportedModernPopupKind(kind)) {
        return false;
    }

    if (!SetPropW(window, kModernPopupKindProperty,
                  reinterpret_cast<HANDLE>(
                      static_cast<ULONG_PTR>(kind)))) {
        return false;
    }

    g_activeModernPopupCount.fetch_add(1, std::memory_order_relaxed);
    Wh_Log(L"Tracking modern %s popup",
           kind == ModernPopupKind::Menu ? L"menu" : L"tooltip");
    return true;
}

void UntrackModernPopup(HWND window) {
    const ModernPopupKind kind = GetTrackedModernPopupKind(window);
    if (!IsSupportedModernPopupKind(kind)) {
        return;
    }

    RemovePropW(window, kModernPopupKindProperty);
    const unsigned int previous =
        g_activeModernPopupCount.fetch_sub(1, std::memory_order_relaxed);
    if (previous == 0) {
        g_activeModernPopupCount.store(0, std::memory_order_relaxed);
    }
}

bool IsModernPopupActive() {
    return g_activeModernPopupCount.load(std::memory_order_relaxed) != 0;
}

BOOL WINAPI ShowWindowHook(HWND window, int command) {
    const bool modernUiEnabled =
        g_modernUiText.load(std::memory_order_relaxed);
    const bool isModernPopup = IsModernPopupWindow(window);
    const bool showing = command != SW_HIDE;

    if (modernUiEnabled && isModernPopup && showing) {
        TrackModernPopup(window);
    }

    const BOOL result = g_originalShowWindow(window, command);

    if (isModernPopup) {
        if (!showing || !IsWindowVisible(window)) {
            UntrackModernPopup(window);
        } else if (modernUiEnabled &&
                   GetTrackedModernPopupKind(window) ==
                   ModernPopupKind::Unclassified) {
            TrackModernPopup(window);
        }
    }

    return result;
}

BOOL WINAPI SetWindowPosHook(HWND window,
                            HWND insertAfter,
                            int x,
                            int y,
                            int width,
                            int height,
                            UINT flags) {
    const bool modernUiEnabled =
        g_modernUiText.load(std::memory_order_relaxed);
    const bool isModernPopup = IsModernPopupWindow(window);

    if (modernUiEnabled && isModernPopup &&
        (flags & SWP_SHOWWINDOW)) {
        TrackModernPopup(window);
    }

    const BOOL result = g_originalSetWindowPos(
        window, insertAfter, x, y, width, height, flags);

    if (isModernPopup && result) {
        if (flags & SWP_HIDEWINDOW) {
            UntrackModernPopup(window);
        } else if (modernUiEnabled && (flags & SWP_SHOWWINDOW) &&
                   GetTrackedModernPopupKind(window) ==
                       ModernPopupKind::Unclassified) {
            TrackModernPopup(window);
        }
    }

    return result;
}

BOOL WINAPI DestroyWindowHook(HWND window) {
    UntrackModernPopup(window);
    return g_originalDestroyWindow(window);
}

// -------------------------------------------------------------------------
// DirectWrite text layout used by the Windows 11 menu
// -------------------------------------------------------------------------

using IDWriteFactory_CreateTextLayout_t =
    HRESULT(STDMETHODCALLTYPE*)(IDWriteFactory* factory,
                                const WCHAR* text,
                                UINT32 textLength,
                                IDWriteTextFormat* textFormat,
                                FLOAT maxWidth,
                                FLOAT maxHeight,
                                IDWriteTextLayout** textLayout);

using IDWriteFactory_CreateGdiCompatibleTextLayout_t =
    HRESULT(STDMETHODCALLTYPE*)(IDWriteFactory* factory,
                                const WCHAR* text,
                                UINT32 textLength,
                                IDWriteTextFormat* textFormat,
                                FLOAT layoutWidth,
                                FLOAT layoutHeight,
                                FLOAT pixelsPerDip,
                                const DWRITE_MATRIX* transform,
                                BOOL useGdiNatural,
                                IDWriteTextLayout** textLayout);

IDWriteFactory_CreateTextLayout_t g_originalCreateTextLayout;
IDWriteFactory_CreateGdiCompatibleTextLayout_t
    g_originalCreateGdiCompatibleTextLayout;

HRESULT CreateTextLayoutWithSpacing(
    IDWriteFactory_CreateTextLayout_t originalFunction,
    const wchar_t* source,
    IDWriteFactory* factory,
    const WCHAR* text,
    UINT32 textLength,
    IDWriteTextFormat* textFormat,
    FLOAT maxWidth,
    FLOAT maxHeight,
    IDWriteTextLayout** textLayout) {
    if (g_modernUiText.load(std::memory_order_relaxed) && text &&
        IsModernPopupActive()) {
        const std::wstring_view original(text, textLength);
        if (!ContainsCjkCodePoint(original)) {
            return originalFunction(factory, text, textLength, textFormat,
                                    maxWidth, maxHeight, textLayout);
        }

        const std::wstring spaced = AddCjkSpacing(original, false);
        if (spaced.size() != original.size()) {
            Wh_Log(L"Applied CJK spacing (%s)", source);
            return originalFunction(
                factory, spaced.c_str(), static_cast<UINT32>(spaced.size()),
                textFormat, maxWidth, maxHeight, textLayout);
        }
    }

    return originalFunction(factory, text, textLength, textFormat, maxWidth,
                            maxHeight, textLayout);
}

HRESULT CreateGdiCompatibleTextLayoutWithSpacing(
    IDWriteFactory_CreateGdiCompatibleTextLayout_t originalFunction,
    const wchar_t* source,
    IDWriteFactory* factory,
    const WCHAR* text,
    UINT32 textLength,
    IDWriteTextFormat* textFormat,
    FLOAT layoutWidth,
    FLOAT layoutHeight,
    FLOAT pixelsPerDip,
    const DWRITE_MATRIX* transform,
    BOOL useGdiNatural,
    IDWriteTextLayout** textLayout) {
    if (g_modernUiText.load(std::memory_order_relaxed) && text &&
        IsModernPopupActive()) {
        const std::wstring_view original(text, textLength);
        if (!ContainsCjkCodePoint(original)) {
            return originalFunction(
                factory, text, textLength, textFormat, layoutWidth,
                layoutHeight, pixelsPerDip, transform, useGdiNatural,
                textLayout);
        }

        const std::wstring spaced = AddCjkSpacing(original, false);
        if (spaced.size() != original.size()) {
            Wh_Log(L"Applied CJK spacing (%s)", source);
            return originalFunction(
                factory, spaced.c_str(), static_cast<UINT32>(spaced.size()),
                textFormat, layoutWidth, layoutHeight, pixelsPerDip, transform,
                useGdiNatural, textLayout);
        }
    }

    return originalFunction(factory, text, textLength, textFormat, layoutWidth,
                            layoutHeight, pixelsPerDip, transform,
                            useGdiNatural, textLayout);
}

HRESULT STDMETHODCALLTYPE CreateTextLayoutHook(
    IDWriteFactory* factory,
    const WCHAR* text,
    UINT32 textLength,
    IDWriteTextFormat* textFormat,
    FLOAT maxWidth,
    FLOAT maxHeight,
    IDWriteTextLayout** textLayout) {
    return CreateTextLayoutWithSpacing(
        g_originalCreateTextLayout, L"modern/DirectWrite/CreateTextLayout",
        factory, text, textLength, textFormat, maxWidth, maxHeight,
        textLayout);
}

HRESULT STDMETHODCALLTYPE CreateGdiCompatibleTextLayoutHook(
    IDWriteFactory* factory,
    const WCHAR* text,
    UINT32 textLength,
    IDWriteTextFormat* textFormat,
    FLOAT layoutWidth,
    FLOAT layoutHeight,
    FLOAT pixelsPerDip,
    const DWRITE_MATRIX* transform,
    BOOL useGdiNatural,
    IDWriteTextLayout** textLayout) {
    return CreateGdiCompatibleTextLayoutWithSpacing(
        g_originalCreateGdiCompatibleTextLayout,
        L"modern/DirectWrite/CreateGdiCompatibleTextLayout", factory, text,
        textLength, textFormat, layoutWidth, layoutHeight, pixelsPerDip,
        transform, useGdiNatural, textLayout);
}

bool HookDWriteFactory(HMODULE module,
                       const char* factoryExport,
                       void* createTextLayoutHook,
                       IDWriteFactory_CreateTextLayout_t* originalTextLayout,
                       void* createGdiLayoutHook,
                       IDWriteFactory_CreateGdiCompatibleTextLayout_t*
                           originalGdiLayout,
                       const wchar_t* implementationName) {
    if (!module) {
        return false;
    }

    using DWriteCreateFactory_t =
        HRESULT(WINAPI*)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
    const auto createFactory = reinterpret_cast<DWriteCreateFactory_t>(
        GetProcAddress(module, factoryExport));
    if (!createFactory) {
        Wh_Log(L"Couldn't find %s factory export", implementationName);
        return false;
    }

    IDWriteFactory* factory = nullptr;
    const HRESULT result =
        createFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                      reinterpret_cast<IUnknown**>(&factory));
    if (FAILED(result) || !factory) {
        Wh_Log(L"%s factory creation failed: 0x%08X", implementationName,
               result);
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(factory);
    const bool textLayoutHooked = Wh_SetFunctionHook(
        vtable[18], createTextLayoutHook,
        reinterpret_cast<void**>(originalTextLayout));
    const bool gdiLayoutHooked = Wh_SetFunctionHook(
        vtable[19], createGdiLayoutHook,
        reinterpret_cast<void**>(originalGdiLayout));

    factory->Release();

    if (!textLayoutHooked || !gdiLayoutHooked) {
        Wh_Log(L"Couldn't install one or more %s hooks",
               implementationName);
    }

    return textLayoutHooked || gdiLayoutHooked;
}

bool HookDirectWrite() {
    bool hooked = false;

    HMODULE dwrite =
        LoadLibraryExW(L"dwrite.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwrite) {
        hooked |= HookDWriteFactory(
            dwrite, "DWriteCreateFactory",
            reinterpret_cast<void*>(CreateTextLayoutHook),
            &g_originalCreateTextLayout,
            reinterpret_cast<void*>(CreateGdiCompatibleTextLayoutHook),
            &g_originalCreateGdiCompatibleTextLayout, L"DirectWrite");
    } else {
        Wh_Log(L"Couldn't load dwrite.dll");
    }

    return hooked;
}

template <typename Function>
bool InstallHook(Function target,
                 Function hook,
                 Function* original,
                 const wchar_t* name) {
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(target),
                            reinterpret_cast<void*>(hook),
                            reinterpret_cast<void**>(original))) {
        Wh_Log(L"Couldn't hook %s", name);
        return false;
    }

    return true;
}

void LoadSettings() {
    g_classicMenus.store(Wh_GetIntSetting(L"classicMenus") != 0,
                         std::memory_order_relaxed);
    g_modernUiText.store(Wh_GetIntSetting(L"modernUiText") != 0,
                         std::memory_order_relaxed);

    PCWSTR characterMode = Wh_GetStringSetting(L"characterMode");
    const bool unicodeMode =
        _wcsicmp(characterMode, L"ascii") != 0;
    g_unicodeLettersAndDigits.store(unicodeMode,
                                    std::memory_order_relaxed);
    Wh_FreeStringSetting(characterMode);
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!g_classicMenus.load(std::memory_order_relaxed) &&
        !g_modernUiText.load(std::memory_order_relaxed)) {
        Wh_Log(L"CJK Spacer has no enabled UI targets");
        return FALSE;
    }

    bool installedAnyHook = false;

    installedAnyHook |= InstallHook(InsertMenuW, InsertMenuWHook,
                                    &g_originalInsertMenuW, L"InsertMenuW");
    installedAnyHook |= InstallHook(AppendMenuW, AppendMenuWHook,
                                    &g_originalAppendMenuW, L"AppendMenuW");
    installedAnyHook |= InstallHook(ModifyMenuW, ModifyMenuWHook,
                                    &g_originalModifyMenuW, L"ModifyMenuW");
    installedAnyHook |=
        InstallHook(InsertMenuItemW, InsertMenuItemWHook,
                    &g_originalInsertMenuItemW, L"InsertMenuItemW");
    installedAnyHook |=
        InstallHook(SetMenuItemInfoW, SetMenuItemInfoWHook,
                    &g_originalSetMenuItemInfoW, L"SetMenuItemInfoW");
    installedAnyHook |=
        InstallHook(TrackPopupMenu, TrackPopupMenuHook,
                    &g_originalTrackPopupMenu, L"TrackPopupMenu");
    installedAnyHook |=
        InstallHook(TrackPopupMenuEx, TrackPopupMenuExHook,
                    &g_originalTrackPopupMenuEx, L"TrackPopupMenuEx");

    installedAnyHook |= InstallHook(ShowWindow, ShowWindowHook,
                                    &g_originalShowWindow, L"ShowWindow");
    installedAnyHook |= InstallHook(SetWindowPos, SetWindowPosHook,
                    &g_originalSetWindowPos, L"SetWindowPos");
    installedAnyHook |= InstallHook(DestroyWindow, DestroyWindowHook,
                                    &g_originalDestroyWindow,
                                    L"DestroyWindow");

    installedAnyHook |= HookDirectWrite();

    Wh_Log(L"CJK Spacer initialized (classic=%d, modern=%d, unicode=%d)",
           g_classicMenus.load(), g_modernUiText.load(),
           g_unicodeLettersAndDigits.load());
    return installedAnyHook ? TRUE : FALSE;
}

void Wh_ModUninit() {
    Wh_Log(L"CJK Spacer uninitialized");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"CJK Spacer settings changed (classic=%d, modern=%d, unicode=%d)",
           g_classicMenus.load(), g_modernUiText.load(),
           g_unicodeLettersAndDigits.load());
}
