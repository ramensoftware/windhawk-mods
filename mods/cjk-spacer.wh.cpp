// ==WindhawkMod==
// @id              cjk-spacer
// @name            CJK Spacer
// @description     Add spaces between CJK characters and letters or digits in Explorer context menus and tooltips
// @version         0.1.9
// @author          aenerv7
// @github          https://github.com/aenerv7
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0.
//
// The Win32 menu hook technique is based in part on the open-source Windhawk
// "Text Replace" mod by m417z. The XAML diagnostics connection is based on the
// Windows 11 styler mods by m417z and ExplorerTAP from TranslucentTB.

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

## Screenshot

Before:

![Before enabling CJK Spacer](https://raw.githubusercontent.com/aenerv7/Dox/main/Windhawk/CJKSpacer/Before.png)

After:

![After enabling CJK Spacer](https://raw.githubusercontent.com/aenerv7/Dox/main/Windhawk/CJKSpacer/After.png)

The transformation is idempotent. Punctuation and existing whitespace break a
boundary and are preserved. Win32 `&` mnemonic markers and tab-separated
keyboard shortcut text are also preserved.

## Supported UI

- Classic Win32 popup menus are rewritten through public `HMENU` APIs. This
  happens only while `TrackPopupMenu` is active, including items added or
  changed while the menu is open. It covers File Explorer, desktop, taskbar,
  tray, and jump-list context menus hosted by `explorer.exe`.
- Classic Win32 tooltips, including legacy notification-area icon tooltips, are
  rewritten only through theme handles opened for the `TOOLTIP` class, covering
  both text measurement and drawing without touching unrelated GDI text.
  Existing tooltip controls are discovered when the mod is enabled in a
  running Explorer. Basic or classic-theme tooltips drawn directly with
  `DrawTextW` aren't covered.
- Windows 11 XAML context menus and pointer tooltips are handled at the XAML
  element level. Only `TextBlock` elements below a `MenuFlyoutPresenter` or
  `ToolTip` are changed; ordinary XAML text and taskbar thumbnail previews are
  outside this scope. Popup visibility is maintained from accessibility
  show/hide/destroy events, and modified text is restored when an element is
  unloaded. This path is disabled by default; enable `modernUiText` to opt in.

The modern path supports both Windows.UI.Xaml and Microsoft.UI.Xaml content
hosted by Explorer. XAML Diagnostics allows one consumer per XAML connection,
so another diagnostics-based customization tool can prevent this path from
initializing.

The mod is injected only into `explorer.exe`. Start, Search, and some shell
flyouts hosted by `StartMenuExperienceHost.exe`, `SearchHost.exe`, or
`ShellExperienceHost.exe` are outside its scope.

The mod doesn't edit system files, registry values, or file names. A file name
shown in a classic menu can nevertheless be displayed with inserted spaces.
The modern path temporarily changes target `TextBlock` values and restores
them when the elements are unloaded. The classic path updates strings stored in
active `HMENU` objects, so menu text read through accessibility APIs also
contains the inserted spaces while the popup is open. Rewritten strings are
recorded and restored when the popup closes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- classicMenus: true
  $name: Classic context menus
  $description: Process classic Win32 popup-menu text.
- classicTooltips: true
  $name: Classic Win32 tooltips
  $description: Process text in legacy tooltips such as notification-area icon tooltips.
- modernUiText: false
  $name: Windows 11 context menus and tooltips
  $description: Process text elements in Explorer XAML context menus and pointer tooltips.
- characterMode: unicode
  $name: Non-CJK character set
  $description: Choose which letters and digits form a spacing boundary with CJK.
  $options:
  - unicode: Unicode letters and digits
  - ascii: ASCII letters and digits only
*/
// ==/WindhawkModSettings==

#include <xamlom.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <windows.h>

#include <commctrl.h>  // TOOLTIPS_CLASSW
#include <uxtheme.h>
#include <vsstyle.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace {

enum class CharacterKind {
    Other,
    Cjk,
    Word,
    Extend,
};

std::atomic_bool g_classicMenus{true};
std::atomic_bool g_classicTooltips{true};
std::atomic_bool g_modernUiText{false};
std::atomic_bool g_unicodeLettersAndDigits{true};

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
    const bool isKana =
        IsInRange(codePoint, 0x3040, 0x30FF) &&
        codePoint != 0x30A0 && codePoint != 0x30FB;
    if (IsInRange(codePoint, 0x2E80, 0x2FDF) ||
        codePoint == 0x3005 || codePoint == 0x3007 ||
        isKana ||
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
    if (codePoint < 0x80) {
        return IsAsciiLetterOrDigit(codePoint);
    }

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

// A token is a base code point, its combining marks/variation selectors, and
// an optional Win32 mnemonic '&' prefix. Keeping the prefix in the token makes
// "打开&Open" become "打开 &Open", which Windows displays as "打开 Open"
// while preserving the O mnemonic.
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

struct RewrittenMenuItem {
    HMENU rootMenu;
    HMENU menu;
    UINT index;
    std::wstring original;
    std::wstring spaced;
};

std::mutex g_rewrittenMenuItemsMutex;
std::vector<RewrittenMenuItem> g_rewrittenMenuItems;
thread_local unsigned int g_classicPopupMenuDepth = 0;
thread_local HMENU g_classicPopupRootMenu = nullptr;
thread_local bool g_restoringClassicMenuText = false;

void RememberRewrittenMenuItem(HMENU menu,
                               UINT index,
                               std::wstring original,
                               std::wstring spaced);
int FindMenuItemIndexByText(
    HMENU menu,
    const std::wstring& expectedText);

bool IsStringMenuFlags(UINT flags) {
    return !(flags & (MF_BITMAP | MF_OWNERDRAW | MF_SEPARATOR));
}

int FindMenuItemIndex(HMENU menu, UINT item, bool byPosition) {
    const int itemCount = GetMenuItemCount(menu);
    if (itemCount < 0) {
        return -1;
    }

    if (byPosition) {
        if (item == static_cast<UINT>(-1)) {
            return itemCount > 0 ? itemCount - 1 : -1;
        }

        return item < static_cast<UINT>(itemCount)
                   ? static_cast<int>(item)
                   : -1;
    }

    for (int index = 0; index < itemCount; ++index) {
        MENUITEMINFOW itemInfo = {};
        itemInfo.cbSize = sizeof(itemInfo);
        itemInfo.fMask = MIIM_ID;
        if (GetMenuItemInfoW(
                menu, index, TRUE, &itemInfo) &&
            itemInfo.wID == item) {
            return index;
        }
    }

    return -1;
}

BOOL WINAPI InsertMenuWHook(HMENU menu,
                            UINT position,
                            UINT flags,
                            UINT_PTR itemId,
                            LPCWSTR newItem) {
    if (!g_restoringClassicMenuText && g_classicPopupMenuDepth > 0 &&
        g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags) && ContainsCjkCodePoint(newItem)) {
        const std::wstring spaced = AddCjkSpacing(newItem);
        if (spaced != newItem) {
            Wh_Log(L"Applied CJK spacing (classic/InsertMenuW)");
            const BOOL result = g_originalInsertMenuW(
                menu, position, flags, itemId, spaced.c_str());
            if (result) {
                int index;
                if (flags & MF_BYPOSITION) {
                    index = FindMenuItemIndex(
                        menu, position, true);
                } else if (flags & MF_POPUP) {
                    index = FindMenuItemIndexByText(
                        menu, spaced);
                } else {
                    index = FindMenuItemIndex(
                        menu, static_cast<UINT>(itemId),
                        false);
                }
                if (index >= 0) {
                    RememberRewrittenMenuItem(
                        menu, static_cast<UINT>(index), newItem, spaced);
                }
            }
            return result;
        }
    }

    return g_originalInsertMenuW(menu, position, flags, itemId, newItem);
}

BOOL WINAPI AppendMenuWHook(HMENU menu,
                            UINT flags,
                            UINT_PTR itemId,
                            LPCWSTR newItem) {
    if (!g_restoringClassicMenuText && g_classicPopupMenuDepth > 0 &&
        g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags) && ContainsCjkCodePoint(newItem)) {
        const std::wstring spaced = AddCjkSpacing(newItem);
        if (spaced != newItem) {
            Wh_Log(L"Applied CJK spacing (classic/AppendMenuW)");
            const BOOL result = g_originalAppendMenuW(
                menu, flags, itemId, spaced.c_str());
            if (result) {
                const int index = GetMenuItemCount(menu) - 1;
                if (index >= 0) {
                    RememberRewrittenMenuItem(
                        menu, static_cast<UINT>(index), newItem, spaced);
                }
            }
            return result;
        }
    }

    return g_originalAppendMenuW(menu, flags, itemId, newItem);
}

BOOL WINAPI ModifyMenuWHook(HMENU menu,
                            UINT position,
                            UINT flags,
                            UINT_PTR itemId,
                            LPCWSTR newItem) {
    if (!g_restoringClassicMenuText && g_classicPopupMenuDepth > 0 &&
        g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags) && ContainsCjkCodePoint(newItem)) {
        const std::wstring spaced = AddCjkSpacing(newItem);
        if (spaced != newItem) {
            Wh_Log(L"Applied CJK spacing (classic/ModifyMenuW)");
            const int index = FindMenuItemIndex(
                menu, position, (flags & MF_BYPOSITION) != 0);
            const BOOL result = g_originalModifyMenuW(
                menu, position, flags, itemId, spaced.c_str());
            if (result && index >= 0) {
                RememberRewrittenMenuItem(
                    menu, static_cast<UINT>(index), newItem, spaced);
            }
            return result;
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

MENUITEMINFOW CopyMenuItemInfoForRewrite(
    const MENUITEMINFOW* itemInfo) {
    MENUITEMINFOW copy = {};
    const size_t copySize =
        itemInfo->cbSize < sizeof(copy) ? itemInfo->cbSize
                                        : sizeof(copy);
    std::memcpy(&copy, itemInfo, copySize);
    return copy;
}

bool ReadMenuItemText(HMENU menu,
                      UINT index,
                      std::wstring* text) {
    MENUITEMINFOW itemInfo = {};
    itemInfo.cbSize = sizeof(itemInfo);
    itemInfo.fMask = MIIM_FTYPE | MIIM_STRING;
    if (!GetMenuItemInfoW(menu, index, TRUE, &itemInfo) ||
        (itemInfo.fType &
         (MFT_BITMAP | MFT_OWNERDRAW | MFT_SEPARATOR))) {
        return false;
    }

    std::vector<wchar_t> buffer(itemInfo.cch + 1);
    itemInfo.dwTypeData = buffer.data();
    itemInfo.cch = static_cast<UINT>(buffer.size());
    if (!GetMenuItemInfoW(menu, index, TRUE, &itemInfo)) {
        return false;
    }

    text->assign(buffer.data(), itemInfo.cch);
    return true;
}

int FindMenuItemIndexByText(HMENU menu,
                            const std::wstring& expectedText) {
    const int itemCount = GetMenuItemCount(menu);
    for (int index = 0; index < itemCount; ++index) {
        std::wstring current;
        if (ReadMenuItemText(
                menu, static_cast<UINT>(index), &current) &&
            current == expectedText) {
            return index;
        }
    }

    return -1;
}

void RememberRewrittenMenuItem(HMENU menu,
                               UINT index,
                               std::wstring original,
                               std::wstring spaced) {
    std::lock_guard<std::mutex> guard(g_rewrittenMenuItemsMutex);

    if (g_rewrittenMenuItems.size() >= 256) {
        std::erase_if(g_rewrittenMenuItems, [](const auto& item) {
            return !IsMenu(item.menu);
        });
    }

    g_rewrittenMenuItems.push_back(
        {g_classicPopupRootMenu, menu, index, std::move(original),
         std::move(spaced)});
}

void RestoreRewrittenMenuItems(HMENU rootMenu = nullptr) {
    std::vector<RewrittenMenuItem> items;
    {
        std::lock_guard<std::mutex> guard(
            g_rewrittenMenuItemsMutex);
        if (!rootMenu) {
            items.swap(g_rewrittenMenuItems);
        } else {
            for (auto iterator = g_rewrittenMenuItems.begin();
                 iterator != g_rewrittenMenuItems.end();) {
                if (iterator->rootMenu == rootMenu) {
                    items.push_back(std::move(*iterator));
                    iterator = g_rewrittenMenuItems.erase(iterator);
                } else {
                    ++iterator;
                }
            }
        }
    }

    const bool wasRestoring = g_restoringClassicMenuText;
    g_restoringClassicMenuText = true;
    for (auto iterator = items.rbegin(); iterator != items.rend();
         ++iterator) {
        if (!IsMenu(iterator->menu)) {
            continue;
        }

        int index = static_cast<int>(iterator->index);
        std::wstring current;
        if (!ReadMenuItemText(
                iterator->menu, iterator->index, &current) ||
            current != iterator->spaced) {
            index = FindMenuItemIndexByText(
                iterator->menu, iterator->spaced);
            if (index < 0) {
                continue;
            }
        }

        MENUITEMINFOW replacement = {};
        replacement.cbSize = sizeof(replacement);
        replacement.fMask = MIIM_STRING;
        replacement.dwTypeData =
            const_cast<wchar_t*>(iterator->original.c_str());

        SetMenuItemInfoW(
            iterator->menu, static_cast<UINT>(index), TRUE,
            &replacement);
    }
    g_restoringClassicMenuText = wasRestoring;
}

BOOL WINAPI InsertMenuItemWHook(HMENU menu,
                                UINT item,
                                BOOL byPosition,
                                LPCMENUITEMINFOW itemInfo) {
    if (!g_restoringClassicMenuText && g_classicPopupMenuDepth > 0 &&
        g_classicMenus.load(std::memory_order_relaxed) &&
        MenuItemInfoContainsString(itemInfo) &&
        ContainsCjkCodePoint(itemInfo->dwTypeData)) {
        const std::wstring spaced = AddCjkSpacing(itemInfo->dwTypeData);
        if (spaced != itemInfo->dwTypeData) {
            Wh_Log(L"Applied CJK spacing (classic/InsertMenuItemW)");
            MENUITEMINFOW copy =
                CopyMenuItemInfoForRewrite(itemInfo);
            copy.dwTypeData = const_cast<wchar_t*>(spaced.c_str());
            const BOOL result = g_originalInsertMenuItemW(
                menu, item, byPosition, &copy);
            if (result) {
                const int index = FindMenuItemIndex(
                    menu,
                    byPosition || !(itemInfo->fMask & MIIM_ID)
                        ? item
                        : itemInfo->wID,
                    byPosition != FALSE);
                if (index >= 0) {
                    RememberRewrittenMenuItem(
                        menu, static_cast<UINT>(index),
                        itemInfo->dwTypeData, spaced);
                }
            }
            return result;
        }
    }

    return g_originalInsertMenuItemW(menu, item, byPosition, itemInfo);
}

BOOL WINAPI SetMenuItemInfoWHook(HMENU menu,
                                 UINT item,
                                 BOOL byPosition,
                                 LPCMENUITEMINFOW itemInfo) {
    if (!g_restoringClassicMenuText && g_classicPopupMenuDepth > 0 &&
        g_classicMenus.load(std::memory_order_relaxed) &&
        MenuItemInfoContainsString(itemInfo) &&
        ContainsCjkCodePoint(itemInfo->dwTypeData)) {
        const std::wstring spaced = AddCjkSpacing(itemInfo->dwTypeData);
        if (spaced != itemInfo->dwTypeData) {
            Wh_Log(L"Applied CJK spacing (classic/SetMenuItemInfoW)");
            const int index = FindMenuItemIndex(
                menu, item, byPosition != FALSE);
            MENUITEMINFOW copy =
                CopyMenuItemInfoForRewrite(itemInfo);
            copy.dwTypeData = const_cast<wchar_t*>(spaced.c_str());
            const BOOL result = g_originalSetMenuItemInfoW(
                menu, item, byPosition, &copy);
            if (result && index >= 0) {
                RememberRewrittenMenuItem(
                    menu, static_cast<UINT>(index),
                    itemInfo->dwTypeData, spaced);
            }
            return result;
        }
    }

    return g_originalSetMenuItemInfoW(menu, item, byPosition, itemInfo);
}

constexpr unsigned int kMaxMenuDepth = 16;

void RewriteMenuTree(HMENU menu, unsigned int depth = 0) {
    if (!menu || depth >= kMaxMenuDepth) {
        return;
    }

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
            std::wstring original;
            if (ReadMenuItemText(menu, index, &original) &&
                ContainsCjkCodePoint(original)) {
                const std::wstring spaced = AddCjkSpacing(original);
                if (spaced != original) {
                    Wh_Log(L"Applied CJK spacing (classic/pre-display)");

                    MENUITEMINFOW replacement = {};
                    replacement.cbSize = sizeof(replacement);
                    replacement.fMask = MIIM_STRING;
                    replacement.dwTypeData =
                        const_cast<wchar_t*>(spaced.c_str());
                    const BOOL rewritten =
                        g_originalSetMenuItemInfoW
                            ? g_originalSetMenuItemInfoW(
                                  menu, index, TRUE, &replacement)
                            : SetMenuItemInfoW(
                                  menu, index, TRUE, &replacement);
                    if (rewritten) {
                        RememberRewrittenMenuItem(
                            menu, index, std::move(original), spaced);
                    }
                }
            }
        }

        if (itemInfo.hSubMenu) {
            RewriteMenuTree(itemInfo.hSubMenu, depth + 1);
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
        const HMENU previousRootMenu = g_classicPopupRootMenu;
        g_classicPopupRootMenu = menu;
        ++g_classicPopupMenuDepth;
        RewriteMenuTree(menu);
        const BOOL result = g_originalTrackPopupMenu(
            menu, flags, x, y, reserved, owner, rect);
        RestoreRewrittenMenuItems(menu);
        --g_classicPopupMenuDepth;
        g_classicPopupRootMenu = previousRootMenu;
        return result;
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
        const HMENU previousRootMenu = g_classicPopupRootMenu;
        g_classicPopupRootMenu = menu;
        ++g_classicPopupMenuDepth;
        RewriteMenuTree(menu);
        const BOOL result = g_originalTrackPopupMenuEx(
            menu, flags, x, y, owner, parameters);
        RestoreRewrittenMenuItems(menu);
        --g_classicPopupMenuDepth;
        g_classicPopupRootMenu = previousRootMenu;
        return result;
    }

    return g_originalTrackPopupMenuEx(menu, flags, x, y, owner, parameters);
}

// -------------------------------------------------------------------------
// Classic Win32 tooltips
// -------------------------------------------------------------------------

using OpenThemeData_t = decltype(&OpenThemeData);
using OpenThemeDataEx_t = decltype(&OpenThemeDataEx);
using OpenThemeDataForDpi_t = decltype(&OpenThemeDataForDpi);
using CloseThemeData_t = decltype(&CloseThemeData);
using GetWindowTheme_t = decltype(&GetWindowTheme);
using GetThemeTextExtent_t = decltype(&GetThemeTextExtent);
using DrawThemeText_t = decltype(&DrawThemeText);
using DrawThemeTextEx_t = decltype(&DrawThemeTextEx);

OpenThemeData_t g_originalOpenThemeData;
OpenThemeDataEx_t g_originalOpenThemeDataEx;
OpenThemeDataForDpi_t g_originalOpenThemeDataForDpi;
CloseThemeData_t g_originalCloseThemeData;
GetWindowTheme_t g_getWindowTheme;
GetThemeTextExtent_t g_originalGetThemeTextExtent;
DrawThemeText_t g_originalDrawThemeText;
DrawThemeTextEx_t g_originalDrawThemeTextEx;

std::shared_mutex g_classicTooltipThemesMutex;
std::unordered_set<HTHEME> g_classicTooltipThemes;
std::atomic_uint g_classicTooltipThemeCount{0};
thread_local bool g_rewritingClassicTooltipText = false;

bool IsClassicTooltipThemeClassList(LPCWSTR classList) {
    if (!classList) {
        return false;
    }

    constexpr wchar_t needle[] = L"tooltip";
    constexpr size_t needleLength = ARRAYSIZE(needle) - 1;
    for (const wchar_t* cursor = classList; *cursor; ++cursor) {
        const bool startsToken =
            cursor == classList || cursor[-1] == L':' ||
            cursor[-1] == L';';
        if (startsToken &&
            _wcsnicmp(cursor, needle, needleLength) == 0) {
            const wchar_t trailing = cursor[needleLength];
            if (trailing == L'\0' || trailing == L';') {
                return true;
            }
        }
    }

    return false;
}

bool IsTrackedClassicTooltipTheme(HTHEME theme) {
    if (!theme ||
        g_classicTooltipThemeCount.load(
            std::memory_order_relaxed) == 0) {
        return false;
    }

    std::shared_lock<std::shared_mutex> guard(
        g_classicTooltipThemesMutex);
    return g_classicTooltipThemes.contains(theme);
}

void TrackClassicTooltipTheme(HTHEME theme) {
    if (!theme) {
        return;
    }

    bool inserted;
    {
        std::lock_guard<std::shared_mutex> guard(
            g_classicTooltipThemesMutex);
        inserted = g_classicTooltipThemes.insert(theme).second;
        g_classicTooltipThemeCount.store(
            static_cast<unsigned int>(g_classicTooltipThemes.size()),
            std::memory_order_relaxed);
    }

    if (inserted) {
        Wh_Log(L"Tracking classic Win32 tooltip theme");
    }
}

void UntrackClassicTooltipTheme(HTHEME theme) {
    if (!theme) {
        return;
    }

    std::lock_guard<std::shared_mutex> guard(
        g_classicTooltipThemesMutex);
    g_classicTooltipThemes.erase(theme);
    g_classicTooltipThemeCount.store(
        static_cast<unsigned int>(g_classicTooltipThemes.size()),
        std::memory_order_relaxed);
}

void ClearTrackedClassicTooltipThemes() {
    std::lock_guard<std::shared_mutex> guard(
        g_classicTooltipThemesMutex);
    g_classicTooltipThemes.clear();
    g_classicTooltipThemeCount.store(0, std::memory_order_relaxed);
}

bool IsClassicTooltipWindow(HWND window) {
    wchar_t className[64];
    return GetClassNameW(
               window, className, ARRAYSIZE(className)) > 0 &&
           _wcsicmp(className, TOOLTIPS_CLASSW) == 0;
}

bool IsClassicTooltipTarget(HTHEME theme, HDC dc) {
    const HWND window = dc ? WindowFromDC(dc) : nullptr;
    return window ? IsClassicTooltipWindow(window)
                  : IsTrackedClassicTooltipTheme(theme);
}

BOOL CALLBACK EnumExistingClassicTooltipWindow(HWND window, LPARAM) {
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId() &&
        IsClassicTooltipWindow(window)) {
        TrackClassicTooltipTheme(g_getWindowTheme(window));
    }

    return TRUE;
}

BOOL CALLBACK EnumExistingClassicTooltipTopLevelWindow(
    HWND window,
    LPARAM) {
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }

    EnumExistingClassicTooltipWindow(window, 0);
    EnumChildWindows(
        window, EnumExistingClassicTooltipWindow, 0);
    return TRUE;
}

void DiscoverExistingClassicTooltipThemes() {
    if (g_getWindowTheme) {
        EnumWindows(
            EnumExistingClassicTooltipTopLevelWindow, 0);
    }
}

HTHEME WINAPI OpenThemeDataHook(HWND window, LPCWSTR classList) {
    HTHEME theme = g_originalOpenThemeData(window, classList);
    if (IsClassicTooltipThemeClassList(classList)) {
        TrackClassicTooltipTheme(theme);
    }

    return theme;
}

HTHEME WINAPI OpenThemeDataExHook(HWND window,
                                  LPCWSTR classList,
                                  DWORD flags) {
    HTHEME theme =
        g_originalOpenThemeDataEx(window, classList, flags);
    if (IsClassicTooltipThemeClassList(classList)) {
        TrackClassicTooltipTheme(theme);
    }

    return theme;
}

HTHEME WINAPI OpenThemeDataForDpiHook(HWND window,
                                      LPCWSTR classList,
                                      UINT dpi) {
    HTHEME theme =
        g_originalOpenThemeDataForDpi(window, classList, dpi);
    if (IsClassicTooltipThemeClassList(classList)) {
        TrackClassicTooltipTheme(theme);
    }

    return theme;
}

HRESULT WINAPI CloseThemeDataHook(HTHEME theme) {
    UntrackClassicTooltipTheme(theme);
    return g_originalCloseThemeData(theme);
}

bool IsClassicTooltipTextPart(int partId) {
    return partId == TTP_STANDARD ||
           partId == TTP_STANDARDTITLE ||
           partId == TTP_BALLOON ||
           partId == TTP_BALLOONTITLE;
}

bool BuildSpacedClassicTooltipText(LPCWSTR text,
                                   int textLength,
                                   DWORD textFlags,
                                   std::wstring* spaced) {
    if (!text || textLength < -1) {
        return false;
    }

    const size_t length =
        textLength < 0 ? wcslen(text)
                       : static_cast<size_t>(textLength);
    if (length == 0) {
        return false;
    }

    const std::wstring_view original(text, length);
    if (!ContainsCjkCodePoint(original)) {
        return false;
    }

    const bool preserveMnemonics = !(textFlags & DT_NOPREFIX);
    *spaced = AddCjkSpacing(original, preserveMnemonics);
    return spaced->size() != original.size();
}

HRESULT WINAPI GetThemeTextExtentHook(HTHEME theme,
                                      HDC dc,
                                      int partId,
                                      int stateId,
                                      LPCWSTR text,
                                      int textLength,
                                      DWORD textFlags,
                                      LPCRECT boundingRectangle,
                                      LPRECT extentRectangle) {
    if (!g_classicTooltips.load(std::memory_order_relaxed) ||
        g_rewritingClassicTooltipText ||
        !IsClassicTooltipTextPart(partId) ||
        !IsClassicTooltipTarget(theme, dc)) {
        return g_originalGetThemeTextExtent(
            theme, dc, partId, stateId, text, textLength, textFlags,
            boundingRectangle, extentRectangle);
    }

    std::wstring spaced;
    if (!BuildSpacedClassicTooltipText(
            text, textLength, textFlags, &spaced)) {
        return g_originalGetThemeTextExtent(
            theme, dc, partId, stateId, text, textLength, textFlags,
            boundingRectangle, extentRectangle);
    }

    Wh_Log(L"Applied CJK spacing "
           L"(classic tooltip/GetThemeTextExtent)");
    g_rewritingClassicTooltipText = true;
    const HRESULT result = g_originalGetThemeTextExtent(
        theme, dc, partId, stateId, spaced.c_str(),
        static_cast<int>(spaced.size()), textFlags,
        boundingRectangle, extentRectangle);
    g_rewritingClassicTooltipText = false;
    return result;
}

HRESULT WINAPI DrawThemeTextHook(HTHEME theme,
                                 HDC dc,
                                 int partId,
                                 int stateId,
                                 LPCWSTR text,
                                 int textLength,
                                 DWORD textFlags,
                                 DWORD textFlags2,
                                 LPCRECT rectangle) {
    if (!g_classicTooltips.load(std::memory_order_relaxed) ||
        g_rewritingClassicTooltipText ||
        !IsClassicTooltipTextPart(partId) ||
        !IsClassicTooltipTarget(theme, dc)) {
        return g_originalDrawThemeText(
            theme, dc, partId, stateId, text, textLength, textFlags,
            textFlags2, rectangle);
    }

    std::wstring spaced;
    if (!BuildSpacedClassicTooltipText(
            text, textLength, textFlags, &spaced)) {
        return g_originalDrawThemeText(
            theme, dc, partId, stateId, text, textLength, textFlags,
            textFlags2, rectangle);
    }

    Wh_Log(L"Applied CJK spacing (classic tooltip/DrawThemeText)");
    g_rewritingClassicTooltipText = true;
    const HRESULT result = g_originalDrawThemeText(
        theme, dc, partId, stateId, spaced.c_str(),
        static_cast<int>(spaced.size()), textFlags, textFlags2,
        rectangle);
    g_rewritingClassicTooltipText = false;
    return result;
}

HRESULT WINAPI DrawThemeTextExHook(HTHEME theme,
                                   HDC dc,
                                   int partId,
                                   int stateId,
                                   LPCWSTR text,
                                   int textLength,
                                   DWORD textFlags,
                                   LPRECT rectangle,
                                   const DTTOPTS* options) {
    if (!g_classicTooltips.load(std::memory_order_relaxed) ||
        g_rewritingClassicTooltipText ||
        !IsClassicTooltipTextPart(partId) ||
        !IsClassicTooltipTarget(theme, dc)) {
        return g_originalDrawThemeTextEx(
            theme, dc, partId, stateId, text, textLength, textFlags,
            rectangle, options);
    }

    std::wstring spaced;
    if (!BuildSpacedClassicTooltipText(
            text, textLength, textFlags, &spaced)) {
        return g_originalDrawThemeTextEx(
            theme, dc, partId, stateId, text, textLength, textFlags,
            rectangle, options);
    }

    Wh_Log(L"Applied CJK spacing (classic tooltip/DrawThemeTextEx)");
    g_rewritingClassicTooltipText = true;
    const HRESULT result = g_originalDrawThemeTextEx(
        theme, dc, partId, stateId, spaced.c_str(),
        static_cast<int>(spaced.size()), textFlags, rectangle,
        options);
    g_rewritingClassicTooltipText = false;
    return result;
}

// -------------------------------------------------------------------------
// Windows 11 XAML context menus and tooltips
// -------------------------------------------------------------------------

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace mux = winrt::Microsoft::UI::Xaml;

struct VisibleModernPopup {
    DWORD threadId;
};

std::mutex g_visibleModernPopupsMutex;
std::unordered_map<HWND, VisibleModernPopup> g_visibleModernPopups;
HWINEVENTHOOK g_modernPopupWinEventHook;

bool IsModernPopupClassName(LPCWSTR className) {
    return className &&
           (_wcsicmp(className, L"Xaml_WindowedPopupClass") == 0 ||
            _wcsicmp(className,
                     L"Microsoft.UI.Content.PopupWindowSiteBridge") == 0);
}

bool IsModernPopupWindow(HWND window) {
    wchar_t className[128];
    return window &&
           GetClassNameW(window, className, ARRAYSIZE(className)) > 0 &&
           IsModernPopupClassName(className);
}

bool TrackVisibleModernPopup(HWND window) {
    if (!window || !IsWindowVisible(window) ||
        !IsModernPopupWindow(window)) {
        return false;
    }

    DWORD processId;
    const DWORD threadId =
        GetWindowThreadProcessId(window, &processId);
    if (!threadId || processId != GetCurrentProcessId()) {
        return false;
    }

    std::lock_guard<std::mutex> guard(g_visibleModernPopupsMutex);
    g_visibleModernPopups[window] = {threadId};
    return true;
}

struct FindVisibleModernPopupData {
    bool found;
};

BOOL CALLBACK FindVisibleModernPopupForThread(HWND window, LPARAM parameter) {
    auto* data =
        reinterpret_cast<FindVisibleModernPopupData*>(parameter);
    if (IsWindowVisible(window) && IsModernPopupWindow(window)) {
        data->found = true;
        TrackVisibleModernPopup(window);
        return FALSE;
    }

    return TRUE;
}

bool IsModernPopupVisibleOnThread(DWORD threadId) {
    {
        std::lock_guard<std::mutex> guard(
            g_visibleModernPopupsMutex);
        for (auto iterator = g_visibleModernPopups.begin();
             iterator != g_visibleModernPopups.end();) {
            if (!IsWindow(iterator->first) ||
                !IsWindowVisible(iterator->first) ||
                !IsModernPopupWindow(iterator->first)) {
                iterator = g_visibleModernPopups.erase(iterator);
                continue;
            }

            if (iterator->second.threadId == threadId) {
                return true;
            }

            ++iterator;
        }
    }

    // SHOW normally populates the map before XAML Loaded runs. This fallback
    // covers an already-visible popup and delayed out-of-context delivery.
    FindVisibleModernPopupData data{};
    EnumThreadWindows(
        threadId, FindVisibleModernPopupForThread,
        reinterpret_cast<LPARAM>(&data));
    return data.found;
}

BOOL CALLBACK DiscoverVisibleModernPopup(HWND window, LPARAM) {
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId() &&
        IsWindowVisible(window)) {
        TrackVisibleModernPopup(window);
    }

    return TRUE;
}

void ClearVisibleModernPopups() {
    std::lock_guard<std::mutex> guard(g_visibleModernPopupsMutex);
    g_visibleModernPopups.clear();
}

bool IsTargetModernAncestorClass(std::wstring_view className) {
    return className.ends_with(L".MenuFlyoutPresenter") ||
           className.ends_with(L".ToolTip");
}

template <typename TextBlock, typename VisualTreeHelper>
bool IsTargetModernTextElement(const TextBlock& textBlock) {
    auto current = VisualTreeHelper::GetParent(textBlock);
    for (unsigned int depth = 0; depth < 64; ++depth) {
        if (!current) {
            return false;
        }

        const auto className = winrt::get_class_name(current);
        if (IsTargetModernAncestorClass(
                std::wstring_view(className.c_str(), className.size()))) {
            return true;
        }

        current = VisualTreeHelper::GetParent(current);
    }

    return false;
}

class ModernTextStateBase {
public:
    explicit ModernTextStateBase(DWORD threadId)
        : m_threadId(threadId) {}
    virtual ~ModernTextStateBase() = default;

    DWORD ThreadId() const {
        return m_threadId;
    }

    virtual void Apply() = 0;
    virtual void Restore() = 0;
    virtual void Detach() = 0;

private:
    DWORD m_threadId;
};

std::mutex g_modernTextStatesMutex;
std::vector<std::weak_ptr<ModernTextStateBase>> g_modernTextStates;

void RegisterModernTextState(
    const std::shared_ptr<ModernTextStateBase>& state) {
    std::lock_guard<std::mutex> guard(g_modernTextStatesMutex);
    g_modernTextStates.emplace_back(state);
}

void RefreshModernTextStates(DWORD threadId, bool apply) {
    std::vector<std::shared_ptr<ModernTextStateBase>> states;
    {
        std::lock_guard<std::mutex> guard(g_modernTextStatesMutex);
        auto output = g_modernTextStates.begin();
        for (auto iterator = g_modernTextStates.begin();
             iterator != g_modernTextStates.end(); ++iterator) {
            if (auto state = iterator->lock()) {
                *output++ = *iterator;
                if (state->ThreadId() == threadId) {
                    states.push_back(std::move(state));
                }
            }
        }
        g_modernTextStates.erase(output, g_modernTextStates.end());
    }

    for (const auto& state : states) {
        apply ? state->Apply() : state->Restore();
    }
}

template <typename TextBlock, typename VisualTreeHelper>
class ModernTextState final
    : public ModernTextStateBase,
      public std::enable_shared_from_this<
          ModernTextState<TextBlock, VisualTreeHelper>> {
public:
    explicit ModernTextState(const TextBlock& textBlock)
        : ModernTextStateBase(GetCurrentThreadId()),
          m_textBlock(winrt::make_weak(textBlock)) {}

    void Attach() {
        auto textBlock = m_textBlock.get();
        if (!textBlock) {
            return;
        }

        const auto self = this->shared_from_this();
        m_loadedToken = textBlock.Loaded(
            [self](const auto&, const auto&) {
                self->Apply();
            });
        m_unloadedToken = textBlock.Unloaded(
            [self](const auto&, const auto&) {
                self->Restore();
            });
        m_attached = true;
    }

    void Apply() override {
        if (!g_modernUiText.load(std::memory_order_relaxed) ||
            !IsModernPopupVisibleOnThread(ThreadId())) {
            return;
        }

        try {
            auto textBlock = m_textBlock.get();
            if (!textBlock || !textBlock.IsLoaded() ||
                !IsTargetModernTextElement<TextBlock, VisualTreeHelper>(
                    textBlock)) {
                return;
            }

            if (m_modified) {
                const auto current = textBlock.Text();
                if (std::wstring_view(current.c_str(), current.size()) ==
                    m_spacedText) {
                    return;
                }

                // The app updated the text while the popup was open. Use the
                // new value instead of restoring stale text over it.
                m_modified = false;
                m_originalText.clear();
                m_spacedText.clear();
            }

            const auto current = textBlock.Text();
            const std::wstring_view original(current.c_str(),
                                             current.size());
            if (!ContainsCjkCodePoint(original)) {
                return;
            }

            std::wstring spaced = AddCjkSpacing(original, false);
            if (spaced.size() == original.size()) {
                return;
            }

            m_originalText.assign(original);
            m_spacedText = std::move(spaced);
            m_modified = true;
            textBlock.Text(winrt::hstring(m_spacedText));
            Wh_Log(L"Applied CJK spacing (modern XAML TextBlock)");
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"Couldn't update modern XAML text: 0x%08X",
                   error.code());
        }
    }

    void Restore() override {
        if (!m_modified) {
            return;
        }

        try {
            auto textBlock = m_textBlock.get();
            if (textBlock) {
                const auto current = textBlock.Text();
                if (std::wstring_view(current.c_str(), current.size()) ==
                    m_spacedText) {
                    textBlock.Text(winrt::hstring(m_originalText));
                }
            }
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"Couldn't restore modern XAML text: 0x%08X",
                   error.code());
        }

        m_modified = false;
        m_originalText.clear();
        m_spacedText.clear();
    }

    void Detach() override {
        Restore();
        if (!m_attached) {
            return;
        }

        try {
            auto textBlock = m_textBlock.get();
            if (textBlock) {
                textBlock.Loaded(m_loadedToken);
                textBlock.Unloaded(m_unloadedToken);
            }
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"Couldn't detach modern XAML events: 0x%08X",
                   error.code());
        }

        m_attached = false;
    }

private:
    winrt::weak_ref<TextBlock> m_textBlock;
    winrt::event_token m_loadedToken{};
    winrt::event_token m_unloadedToken{};
    bool m_attached = false;
    bool m_modified = false;
    std::wstring m_originalText;
    std::wstring m_spacedText;
};

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_xamlDiagnostics(site.as<IXamlDiagnostics>()) {
        AddRef();
        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID parameter) -> DWORD {
                auto* watcher =
                    static_cast<VisualTreeWatcher*>(parameter);
                const HRESULT result =
                    watcher->m_xamlDiagnostics
                        .as<IVisualTreeService3>()
                        ->AdviseVisualTreeChange(watcher);
                if (FAILED(result)) {
                    Wh_Log(L"AdviseVisualTreeChange failed: 0x%08X",
                           result);
                }
                watcher->Release();
                return 0;
            },
            this, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            Release();
            Wh_Log(L"Couldn't create XAML watcher thread");
        }
    }

    void UnadviseVisualTreeChange() {
        const HRESULT result =
            m_xamlDiagnostics.as<IVisualTreeService3>()
                ->UnadviseVisualTreeChange(this);
        if (FAILED(result)) {
            Wh_Log(L"UnadviseVisualTreeChange failed: 0x%08X",
                   result);
        }

        for (auto& [handle, state] : m_textStates) {
            state->Detach();
        }
        m_textStates.clear();
    }

private:
    wf::IInspectable FromHandle(InstanceHandle handle) {
        wf::IInspectable object;
        winrt::check_hresult(
            m_xamlDiagnostics->GetIInspectableFromHandle(
                handle,
                reinterpret_cast<::IInspectable**>(
                    winrt::put_abi(object))));
        return object;
    }

    template <typename TextBlock, typename VisualTreeHelper>
    void RegisterTextBlock(InstanceHandle handle,
                           const TextBlock& textBlock) {
        if (!IsTargetModernTextElement<TextBlock, VisualTreeHelper>(
                textBlock)) {
            return;
        }

        auto state = std::make_shared<
            ModernTextState<TextBlock, VisualTreeHelper>>(textBlock);
        state->Attach();
        RegisterModernTextState(state);
        state->Apply();

        if (auto iterator = m_textStates.find(handle);
            iterator != m_textStates.end()) {
            iterator->second->Detach();
            iterator->second = std::move(state);
        } else {
            m_textStates.emplace(handle, std::move(state));
        }
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) override try {
        if (mutationType == Add &&
            g_modernUiText.load(std::memory_order_relaxed)) {
            const std::wstring_view type(
                element.Type ? element.Type : L"");
            if (!type.ends_with(L".TextBlock")) {
                return S_OK;
            }

            const auto inspectable = FromHandle(element.Handle);
            if (auto textBlock =
                    inspectable.try_as<
                        wux::Controls::TextBlock>()) {
                RegisterTextBlock<
                    wux::Controls::TextBlock,
                    wux::Media::VisualTreeHelper>(
                        element.Handle, textBlock);
            } else if (auto textBlock =
                           inspectable.try_as<
                               mux::Controls::TextBlock>()) {
                RegisterTextBlock<
                    mux::Controls::TextBlock,
                    mux::Media::VisualTreeHelper>(
                        element.Handle, textBlock);
            }
        } else if (mutationType == Remove) {
            if (auto iterator = m_textStates.find(element.Handle);
                iterator != m_textStates.end()) {
                iterator->second->Detach();
                m_textStates.erase(iterator);
            }
        }

        return S_OK;
    } catch (...) {
        Wh_Log(L"XAML visual-tree callback failed: 0x%08X",
               winrt::to_hresult());
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle,
        VisualElementState,
        LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_xamlDiagnostics;
    std::unordered_map<
        InstanceHandle,
        std::shared_ptr<ModernTextStateBase>> m_textStates;
};

std::mutex g_visualTreeWatchersMutex;
std::vector<winrt::com_ptr<VisualTreeWatcher>>
    g_visualTreeWatchers;
std::atomic_bool g_stoppingModernUi{false};

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
            &module)) {
        return nullptr;
    }

    return module;
}

class WindhawkTap
    : public winrt::implements<WindhawkTap,
                               IObjectWithSite,
                               winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override try {
        m_site.copy_from(site);
        if (!site || g_stoppingModernUi.load(
                         std::memory_order_relaxed)) {
            return S_OK;
        }

        // Balance the reference added when XAML Diagnostics loads this module.
        if (HMODULE module = GetCurrentModuleHandle()) {
            FreeLibrary(module);
        }

        auto watcher =
            winrt::make_self<VisualTreeWatcher>(m_site);
        {
            std::lock_guard<std::mutex> guard(
                g_visualTreeWatchersMutex);
            g_visualTreeWatchers.push_back(watcher);
        }
        m_watcher = std::move(watcher);
        return S_OK;
    } catch (...) {
        const HRESULT result = winrt::to_hresult();
        Wh_Log(L"Couldn't create XAML watcher: 0x%08X", result);
        return result;
    }

    HRESULT STDMETHODCALLTYPE GetSite(
        REFIID interfaceId,
        void** object) noexcept override {
        return m_site.as(interfaceId, object);
    }

private:
    winrt::com_ptr<IUnknown> m_site;
    winrt::com_ptr<VisualTreeWatcher> m_watcher;
};

template <typename Class>
struct SimpleFactory
    : winrt::implements<SimpleFactory<Class>,
                        IClassFactory,
                        winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(
        IUnknown* outer,
        REFIID interfaceId,
        void** object) override try {
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }

        *object = nullptr;
        return winrt::make<Class>().as(interfaceId, object);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

// {3245D18D-2C18-4EA3-9A10-9C5A2B553976}
constexpr CLSID CLSID_CjkSpacerTap = {
    0x3245d18d,
    0x2c18,
    0x4ea3,
    {0x9a, 0x10, 0x9c, 0x5a, 0x2b, 0x55, 0x39, 0x76}};

}  // namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) _Use_decl_annotations_
STDAPI DllGetClassObject(REFCLSID classId,
                         REFIID interfaceId,
                         void** object) try {
    if (classId != CLSID_CjkSpacerTap) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    *object = nullptr;
    return winrt::make<SimpleFactory<WindhawkTap>>().as(
        interfaceId, object);
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) _Use_decl_annotations_
STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

namespace {

using InitializeXamlDiagnosticsEx_t =
    decltype(&InitializeXamlDiagnosticsEx);

std::mutex g_xamlDiagnosticsInitializationMutex;
bool g_windowsUiXamlDiagnosticsConnected;
bool g_microsoftUiXamlDiagnosticsConnected;

HRESULT InjectXamlDiagnostics(HMODULE xamlModule,
                              LPCWSTR connectionPrefix) {
    const auto initialize =
        reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
            GetProcAddress(xamlModule,
                           "InitializeXamlDiagnosticsEx"));
    if (!initialize) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    wchar_t modulePath[MAX_PATH];
    const DWORD pathLength =
        GetModuleFileNameW(module, modulePath,
                           ARRAYSIZE(modulePath));
    if (!pathLength || pathLength == ARRAYSIZE(modulePath)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (int index = 1; index <= 10000; ++index) {
        wchar_t connectionName[256];
        _snwprintf_s(connectionName, _TRUNCATE, L"%s%d",
                     connectionPrefix, index);
        result = initialize(
            connectionName, GetCurrentProcessId(), L"",
            modulePath, CLSID_CjkSpacerTap, nullptr);
        if (result != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    return result;
}

void EnsureModernXamlDiagnostics() {
    if (g_stoppingModernUi.load(std::memory_order_relaxed)) {
        return;
    }

    std::lock_guard<std::mutex> guard(
        g_xamlDiagnosticsInitializationMutex);

    if (!g_windowsUiXamlDiagnosticsConnected) {
        if (HMODULE module =
                GetModuleHandleW(L"Windows.UI.Xaml.dll")) {
            const HRESULT result = InjectXamlDiagnostics(
                module, L"VisualDiagConnection");
            if (SUCCEEDED(result)) {
                g_windowsUiXamlDiagnosticsConnected = true;
                Wh_Log(L"Connected to Windows.UI.Xaml diagnostics");
            } else if (result !=
                       HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
                Wh_Log(L"Windows.UI.Xaml diagnostics failed: "
                       L"0x%08X", result);
            }
        }
    }

    if (!g_microsoftUiXamlDiagnosticsConnected) {
        if (HMODULE module = GetModuleHandleW(
                L"Microsoft.Internal.FrameworkUdk.dll")) {
            const HRESULT result = InjectXamlDiagnostics(
                module, L"WinUIVisualDiagConnection");
            if (SUCCEEDED(result)) {
                g_microsoftUiXamlDiagnosticsConnected = true;
                Wh_Log(L"Connected to Microsoft.UI.Xaml diagnostics");
            } else if (result !=
                       HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
                Wh_Log(L"Microsoft.UI.Xaml diagnostics failed: "
                       L"0x%08X", result);
            }
        }
    }
}

void CALLBACK ModernPopupWinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND window,
    LONG objectId,
    LONG childId,
    DWORD,
    DWORD) {
    if (!window || objectId != OBJID_WINDOW ||
        childId != CHILDID_SELF ||
        !g_modernUiText.load(std::memory_order_relaxed)) {
        return;
    }

    DWORD processId;
    const DWORD threadId =
        GetWindowThreadProcessId(window, &processId);
    if (!threadId || processId != GetCurrentProcessId()) {
        return;
    }

    if (event == EVENT_OBJECT_SHOW) {
        if (!TrackVisibleModernPopup(window)) {
            return;
        }

        EnsureModernXamlDiagnostics();
        if (GetCurrentThreadId() == threadId) {
            RefreshModernTextStates(threadId, true);
        }
    } else if (event == EVENT_OBJECT_HIDE ||
               event == EVENT_OBJECT_DESTROY) {
        bool wasTracked;
        {
            std::lock_guard<std::mutex> guard(
                g_visibleModernPopupsMutex);
            wasTracked =
                g_visibleModernPopups.erase(window) != 0;
        }

        if (wasTracked && GetCurrentThreadId() == threadId) {
            // Restore on the UI thread which generated the in-context event.
            // Loaded applies spacing if a reusable popup is shown again.
            RefreshModernTextStates(threadId, false);
        }
    }
}

bool InitializeModernUi() {
    g_stoppingModernUi.store(false, std::memory_order_relaxed);

    g_modernPopupWinEventHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE,
        GetCurrentModuleHandle(), ModernPopupWinEventProc,
        GetCurrentProcessId(), 0, WINEVENT_INCONTEXT);
    if (!g_modernPopupWinEventHook) {
        Wh_Log(L"In-context popup event hook failed, trying "
               L"out-of-context");
        g_modernPopupWinEventHook = SetWinEventHook(
            EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr,
            ModernPopupWinEventProc, GetCurrentProcessId(), 0,
            WINEVENT_OUTOFCONTEXT);
    }

    EnumWindows(DiscoverVisibleModernPopup, 0);
    EnsureModernXamlDiagnostics();
    return g_modernPopupWinEventHook != nullptr;
}

void UninitializeModernUi() {
    g_stoppingModernUi.store(true, std::memory_order_relaxed);

    if (g_modernPopupWinEventHook) {
        UnhookWinEvent(g_modernPopupWinEventHook);
        g_modernPopupWinEventHook = nullptr;
    }

    std::vector<winrt::com_ptr<VisualTreeWatcher>> watchers;
    {
        std::lock_guard<std::mutex> guard(
            g_visualTreeWatchersMutex);
        watchers.swap(g_visualTreeWatchers);
    }
    for (const auto& watcher : watchers) {
        watcher->UnadviseVisualTreeChange();
    }

    {
        std::lock_guard<std::mutex> guard(g_modernTextStatesMutex);
        g_modernTextStates.clear();
    }
    ClearVisibleModernPopups();
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

bool HookClassicTooltipThemeDrawing() {
    HMODULE themeModule = LoadLibraryExW(
        L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!themeModule) {
        Wh_Log(L"Couldn't load uxtheme.dll");
        return false;
    }

    bool hooked = false;

    g_getWindowTheme = reinterpret_cast<GetWindowTheme_t>(
        GetProcAddress(themeModule, "GetWindowTheme"));
    if (!g_getWindowTheme) {
        Wh_Log(L"Couldn't find GetWindowTheme");
    }

    const auto openThemeData =
        reinterpret_cast<OpenThemeData_t>(
            GetProcAddress(themeModule, "OpenThemeData"));
    if (openThemeData) {
        hooked |= InstallHook(
            openThemeData, OpenThemeDataHook,
            &g_originalOpenThemeData, L"OpenThemeData");
    } else {
        Wh_Log(L"Couldn't find OpenThemeData");
    }

    const auto openThemeDataEx =
        reinterpret_cast<OpenThemeDataEx_t>(
            GetProcAddress(themeModule, "OpenThemeDataEx"));
    if (openThemeDataEx) {
        hooked |= InstallHook(
            openThemeDataEx, OpenThemeDataExHook,
            &g_originalOpenThemeDataEx, L"OpenThemeDataEx");
    } else {
        Wh_Log(L"Couldn't find OpenThemeDataEx");
    }

    const auto openThemeDataForDpi =
        reinterpret_cast<OpenThemeDataForDpi_t>(
            GetProcAddress(themeModule, "OpenThemeDataForDpi"));
    if (openThemeDataForDpi) {
        hooked |= InstallHook(
            openThemeDataForDpi, OpenThemeDataForDpiHook,
            &g_originalOpenThemeDataForDpi,
            L"OpenThemeDataForDpi");
    } else {
        Wh_Log(L"Couldn't find OpenThemeDataForDpi");
    }

    const auto closeThemeData =
        reinterpret_cast<CloseThemeData_t>(
            GetProcAddress(themeModule, "CloseThemeData"));
    if (closeThemeData) {
        hooked |= InstallHook(
            closeThemeData, CloseThemeDataHook,
            &g_originalCloseThemeData, L"CloseThemeData");
    } else {
        Wh_Log(L"Couldn't find CloseThemeData");
    }

    const auto getThemeTextExtent =
        reinterpret_cast<GetThemeTextExtent_t>(
            GetProcAddress(themeModule, "GetThemeTextExtent"));
    if (getThemeTextExtent) {
        hooked |= InstallHook(
            getThemeTextExtent, GetThemeTextExtentHook,
            &g_originalGetThemeTextExtent,
            L"GetThemeTextExtent");
    } else {
        Wh_Log(L"Couldn't find GetThemeTextExtent");
    }

    const auto drawThemeText =
        reinterpret_cast<DrawThemeText_t>(
            GetProcAddress(themeModule, "DrawThemeText"));
    if (drawThemeText) {
        hooked |= InstallHook(
            drawThemeText, DrawThemeTextHook,
            &g_originalDrawThemeText, L"DrawThemeText");
    } else {
        Wh_Log(L"Couldn't find DrawThemeText");
    }

    const auto drawThemeTextEx =
        reinterpret_cast<DrawThemeTextEx_t>(
            GetProcAddress(themeModule, "DrawThemeTextEx"));
    if (drawThemeTextEx) {
        hooked |= InstallHook(
            drawThemeTextEx, DrawThemeTextExHook,
            &g_originalDrawThemeTextEx, L"DrawThemeTextEx");
    } else {
        Wh_Log(L"Couldn't find DrawThemeTextEx");
    }

    return hooked;
}

void LoadSettings() {
    g_classicMenus.store(Wh_GetIntSetting(L"classicMenus") != 0,
                         std::memory_order_relaxed);
    g_classicTooltips.store(
        Wh_GetIntSetting(L"classicTooltips") != 0,
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

    const bool classicMenusEnabled =
        g_classicMenus.load(std::memory_order_relaxed);
    const bool classicTooltipsEnabled =
        g_classicTooltips.load(std::memory_order_relaxed);
    const bool modernUiTextEnabled =
        g_modernUiText.load(std::memory_order_relaxed);

    if (!classicMenusEnabled && !classicTooltipsEnabled &&
        !modernUiTextEnabled) {
        Wh_Log(L"CJK Spacer has no enabled UI targets");
        return FALSE;
    }

    bool installedAnyHook = false;

    if (classicMenusEnabled) {
        installedAnyHook |= InstallHook(
            InsertMenuW, InsertMenuWHook, &g_originalInsertMenuW,
            L"InsertMenuW");
        installedAnyHook |= InstallHook(
            AppendMenuW, AppendMenuWHook, &g_originalAppendMenuW,
            L"AppendMenuW");
        installedAnyHook |= InstallHook(
            ModifyMenuW, ModifyMenuWHook, &g_originalModifyMenuW,
            L"ModifyMenuW");
        installedAnyHook |= InstallHook(
            InsertMenuItemW, InsertMenuItemWHook,
            &g_originalInsertMenuItemW, L"InsertMenuItemW");
        installedAnyHook |= InstallHook(
            SetMenuItemInfoW, SetMenuItemInfoWHook,
            &g_originalSetMenuItemInfoW, L"SetMenuItemInfoW");
        installedAnyHook |= InstallHook(
            TrackPopupMenu, TrackPopupMenuHook,
            &g_originalTrackPopupMenu, L"TrackPopupMenu");
        installedAnyHook |= InstallHook(
            TrackPopupMenuEx, TrackPopupMenuExHook,
            &g_originalTrackPopupMenuEx, L"TrackPopupMenuEx");
    }

    if (classicTooltipsEnabled) {
        installedAnyHook |= HookClassicTooltipThemeDrawing();
    }

    if (modernUiTextEnabled) {
        installedAnyHook |= InitializeModernUi();
    }

    Wh_Log(L"CJK Spacer initialized (classicMenus=%d, "
           L"classicTooltips=%d, modern=%d, unicode=%d)",
           g_classicMenus.load(), g_classicTooltips.load(),
           g_modernUiText.load(),
           g_unicodeLettersAndDigits.load());
    return installedAnyHook ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    if (g_classicTooltips.load(std::memory_order_relaxed)) {
        DiscoverExistingClassicTooltipThemes();
    }

}

void Wh_ModUninit() {
    RestoreRewrittenMenuItems();
    ClearTrackedClassicTooltipThemes();
    if (g_modernUiText.load(std::memory_order_relaxed)) {
        UninitializeModernUi();
    }
    Wh_Log(L"CJK Spacer uninitialized");
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
