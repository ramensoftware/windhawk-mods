// ==WindhawkMod==
// @id              cjk-spacer
// @name            CJK Spacer
// @description     Add spaces between CJK characters and letters or digits in Explorer menus, tooltips, and popups
// @version         0.1.5
// @author          aenerv7
// @github          https://github.com/aenerv7
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
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
  includes File Explorer, desktop, taskbar, tray, and jump-list menus hosted by
  `explorer.exe`.
- Classic Win32 tooltips, including legacy notification-area icon tooltips, are
  rewritten only through theme handles opened for the `TOOLTIP` class, covering
  both text measurement and drawing without touching unrelated GDI text. Basic
  or classic-theme tooltips drawn directly with `DrawTextW` aren't covered.
- Windows 11 XAML popup text uses an experimental, display-only
  DirectWrite hook. This includes context menus, tooltips, and other Explorer
  flyouts. While a tracked popup is visible, other XAML text laid out on the
  same UI thread can also be transformed; unrelated Explorer threads remain
  unaffected. Taskbar thumbnail previews are explicitly excluded.

The modern path is intentionally conservative and can miss text if a Windows
build uses a different popup window class. Disable `modernUiText` if it causes
a problem.

The mod is injected only into `explorer.exe`. Start, Search, and some shell
flyouts hosted by `StartMenuExperienceHost.exe`, `SearchHost.exe`, or
`ShellExperienceHost.exe` are outside its scope.

The mod doesn't edit system files, registry values, or file names. A file name
shown in a classic menu can nevertheless be displayed with inserted spaces.
The modern DirectWrite path changes display text only. The classic path updates
strings stored in `HMENU` objects, so menu text read through accessibility APIs
also contains the inserted spaces while the mod is enabled. Strings rewritten
just before display are recorded and restored when the mod is disabled.
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
- modernUiText: true
  $name: Windows 11 popup text (experimental)
  $description: Process DirectWrite text in Explorer XAML menus, tooltips, and flyouts.
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
#include <cstring>
#include <cwchar>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <dwrite.h>
#include <uxtheme.h>
#include <windows.h>

namespace {

enum class CharacterKind {
    Other,
    Cjk,
    Word,
    Extend,
};

std::atomic_bool g_classicMenus{true};
std::atomic_bool g_classicTooltips{true};
std::atomic_bool g_modernUiText{true};
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
    HMENU menu;
    UINT index;
    std::wstring original;
    std::wstring spaced;
};

std::mutex g_rewrittenMenuItemsMutex;
std::vector<RewrittenMenuItem> g_rewrittenMenuItems;

bool IsStringMenuFlags(UINT flags) {
    return !(flags & (MF_BITMAP | MF_OWNERDRAW | MF_SEPARATOR));
}

BOOL WINAPI InsertMenuWHook(HMENU menu,
                            UINT position,
                            UINT flags,
                            UINT_PTR itemId,
                            LPCWSTR newItem) {
    if (g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags) && ContainsCjkCodePoint(newItem)) {
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
        IsStringMenuFlags(flags) && ContainsCjkCodePoint(newItem)) {
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
        IsStringMenuFlags(flags) && ContainsCjkCodePoint(newItem)) {
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

    for (auto& item : g_rewrittenMenuItems) {
        if (item.menu == menu && item.index == index) {
            if (item.spaced != spaced) {
                item.original = std::move(original);
                item.spaced = std::move(spaced);
            }
            return;
        }
    }

    g_rewrittenMenuItems.push_back(
        {menu, index, std::move(original), std::move(spaced)});
}

void RestoreRewrittenMenuItems() {
    std::vector<RewrittenMenuItem> items;
    {
        std::lock_guard<std::mutex> guard(
            g_rewrittenMenuItemsMutex);
        items.swap(g_rewrittenMenuItems);
    }

    for (auto iterator = items.rbegin(); iterator != items.rend();
         ++iterator) {
        if (!IsMenu(iterator->menu)) {
            continue;
        }

        std::wstring current;
        if (!ReadMenuItemText(iterator->menu, iterator->index,
                              &current) ||
            current != iterator->spaced) {
            continue;
        }

        MENUITEMINFOW replacement = {};
        replacement.cbSize = sizeof(replacement);
        replacement.fMask = MIIM_STRING;
        replacement.dwTypeData =
            const_cast<wchar_t*>(iterator->original.c_str());

        SetMenuItemInfoW(
            iterator->menu, iterator->index, TRUE, &replacement);
    }
}

BOOL WINAPI InsertMenuItemWHook(HMENU menu,
                                UINT item,
                                BOOL byPosition,
                                LPCMENUITEMINFOW itemInfo) {
    if (g_classicMenus.load(std::memory_order_relaxed) &&
        MenuItemInfoContainsString(itemInfo) &&
        ContainsCjkCodePoint(itemInfo->dwTypeData)) {
        const std::wstring spaced = AddCjkSpacing(itemInfo->dwTypeData);
        if (spaced != itemInfo->dwTypeData) {
            Wh_Log(L"Applied CJK spacing (classic/InsertMenuItemW)");
            MENUITEMINFOW copy =
                CopyMenuItemInfoForRewrite(itemInfo);
            copy.dwTypeData = const_cast<wchar_t*>(spaced.c_str());
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
        MenuItemInfoContainsString(itemInfo) &&
        ContainsCjkCodePoint(itemInfo->dwTypeData)) {
        const std::wstring spaced = AddCjkSpacing(itemInfo->dwTypeData);
        if (spaced != itemInfo->dwTypeData) {
            Wh_Log(L"Applied CJK spacing (classic/SetMenuItemInfoW)");
            MENUITEMINFOW copy =
                CopyMenuItemInfoForRewrite(itemInfo);
            copy.dwTypeData = const_cast<wchar_t*>(spaced.c_str());
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
                    if (SetMenuItemInfoW(
                            menu, index, TRUE, &replacement)) {
                        RememberRewrittenMenuItem(
                            menu, index, std::move(original), spaced);
                    }
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
// Classic Win32 tooltips
// -------------------------------------------------------------------------

using OpenThemeData_t = decltype(&OpenThemeData);
using OpenThemeDataEx_t = decltype(&OpenThemeDataEx);
using OpenThemeDataForDpi_t = decltype(&OpenThemeDataForDpi);
using CloseThemeData_t = decltype(&CloseThemeData);
using GetThemeTextExtent_t = decltype(&GetThemeTextExtent);
using DrawThemeText_t = decltype(&DrawThemeText);
using DrawThemeTextEx_t = decltype(&DrawThemeTextEx);

OpenThemeData_t g_originalOpenThemeData;
OpenThemeDataEx_t g_originalOpenThemeDataEx;
OpenThemeDataForDpi_t g_originalOpenThemeDataForDpi;
CloseThemeData_t g_originalCloseThemeData;
GetThemeTextExtent_t g_originalGetThemeTextExtent;
DrawThemeText_t g_originalDrawThemeText;
DrawThemeTextEx_t g_originalDrawThemeTextEx;

std::mutex g_classicTooltipThemesMutex;
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

    std::lock_guard<std::mutex> guard(g_classicTooltipThemesMutex);
    return g_classicTooltipThemes.contains(theme);
}

void TrackClassicTooltipTheme(HTHEME theme) {
    if (!theme) {
        return;
    }

    bool inserted;
    {
        std::lock_guard<std::mutex> guard(g_classicTooltipThemesMutex);
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

    std::lock_guard<std::mutex> guard(g_classicTooltipThemesMutex);
    g_classicTooltipThemes.erase(theme);
    g_classicTooltipThemeCount.store(
        static_cast<unsigned int>(g_classicTooltipThemes.size()),
        std::memory_order_relaxed);
}

void ClearTrackedClassicTooltipThemes() {
    std::lock_guard<std::mutex> guard(g_classicTooltipThemesMutex);
    g_classicTooltipThemes.clear();
    g_classicTooltipThemeCount.store(0, std::memory_order_relaxed);
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
        !IsTrackedClassicTooltipTheme(theme)) {
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
        !IsTrackedClassicTooltipTheme(theme)) {
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
        !IsTrackedClassicTooltipTheme(theme)) {
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
// Windows 11 XAML popup detection
// -------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID parameter,
    DWORD band);
using CreateWindowInBandEx_t = HWND(WINAPI*)(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID parameter,
    DWORD band,
    DWORD typeFlags);

CreateWindowExW_t g_originalCreateWindowExW;
CreateWindowInBand_t g_originalCreateWindowInBand;
CreateWindowInBandEx_t g_originalCreateWindowInBandEx;

enum class ModernWindowKind {
    Other,
    Popup,
    TaskbarThumbnail,
};

struct TrackedModernWindow {
    DWORD threadId;
    ModernWindowKind kind;
};

std::mutex g_trackedModernWindowsMutex;
std::unordered_map<HWND, TrackedModernWindow>
    g_trackedModernWindows;
std::atomic_uint g_trackedModernWindowCount{0};

ModernWindowKind ClassifyModernWindowClassName(LPCWSTR className) {
    if (!className) {
        return ModernWindowKind::Other;
    }

    if (_wcsicmp(className, L"Xaml_WindowedPopupClass") == 0) {
        return ModernWindowKind::Popup;
    }

    if (_wcsicmp(className, L"TaskListThumbnailWnd") == 0) {
        return ModernWindowKind::TaskbarThumbnail;
    }

    return ModernWindowKind::Other;
}

ModernWindowKind ClassifyModernWindow(HWND window) {
    wchar_t className[128];
    const int length = GetClassNameW(window, className, ARRAYSIZE(className));
    return length > 0 ? ClassifyModernWindowClassName(className)
                      : ModernWindowKind::Other;
}

bool TrackModernWindow(HWND window, ModernWindowKind kind) {
    if (!window || kind == ModernWindowKind::Other) {
        return false;
    }

    DWORD processId;
    const DWORD threadId =
        GetWindowThreadProcessId(window, &processId);
    if (!threadId || processId != GetCurrentProcessId()) {
        return false;
    }

    bool inserted;
    {
        std::lock_guard<std::mutex> guard(
            g_trackedModernWindowsMutex);
        const auto [iterator, wasInserted] =
            g_trackedModernWindows.try_emplace(
                window, TrackedModernWindow{threadId, kind});
        if (!wasInserted) {
            iterator->second = {threadId, kind};
        }
        inserted = wasInserted;
        g_trackedModernWindowCount.store(
            static_cast<unsigned int>(
                g_trackedModernWindows.size()),
            std::memory_order_relaxed);
    }

    if (inserted) {
        if (kind == ModernWindowKind::TaskbarThumbnail) {
            Wh_Log(L"Tracking excluded taskbar thumbnail");
        } else {
            Wh_Log(L"Tracking modern popup");
        }
    }
    return true;
}

bool TrackModernWindowIfRelevant(HWND window) {
    const ModernWindowKind kind = ClassifyModernWindow(window);
    return TrackModernWindow(window, kind);
}

void TrackCreatedModernWindow(HWND window, LPCWSTR className) {
    if (!window) {
        return;
    }

    const ModernWindowKind kind =
        className && !IS_INTRESOURCE(className)
            ? ClassifyModernWindowClassName(className)
            : ClassifyModernWindow(window);
    TrackModernWindow(window, kind);
}

BOOL CALLBACK EnumExistingModernWindow(HWND window, LPARAM) {
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId()) {
        TrackModernWindowIfRelevant(window);
    }

    return TRUE;
}

void DiscoverExistingModernWindows() {
    EnumWindows(EnumExistingModernWindow, 0);
}

void ClearTrackedModernWindows() {
    std::lock_guard<std::mutex> guard(
        g_trackedModernWindowsMutex);
    g_trackedModernWindows.clear();
    g_trackedModernWindowCount.store(0, std::memory_order_relaxed);
}

bool IsModernPopupActiveForCurrentThread() {
    if (g_trackedModernWindowCount.load(
            std::memory_order_relaxed) == 0) {
        return false;
    }

    const DWORD currentThreadId = GetCurrentThreadId();
    bool popupActive = false;
    bool taskbarThumbnailActive = false;

    std::lock_guard<std::mutex> guard(
        g_trackedModernWindowsMutex);
    for (auto iterator = g_trackedModernWindows.begin();
         iterator != g_trackedModernWindows.end();) {
        if (!IsWindow(iterator->first)) {
            iterator = g_trackedModernWindows.erase(iterator);
            g_trackedModernWindowCount.store(
                static_cast<unsigned int>(
                    g_trackedModernWindows.size()),
                std::memory_order_relaxed);
            continue;
        }

        if (iterator->second.threadId == currentThreadId &&
            IsWindowVisible(iterator->first)) {
            if (iterator->second.kind ==
                ModernWindowKind::TaskbarThumbnail) {
                taskbarThumbnailActive = true;
            } else if (iterator->second.kind ==
                       ModernWindowKind::Popup) {
                popupActive = true;
            }
        }

        ++iterator;
    }

    return popupActive && !taskbarThumbnailActive;
}

HWND WINAPI CreateWindowExWHook(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID parameter) {
    HWND window = g_originalCreateWindowExW(
        exStyle, className, windowName, style, x, y, width, height,
        parent, menu, instance, parameter);
    TrackCreatedModernWindow(window, className);
    return window;
}

HWND WINAPI CreateWindowInBandHook(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID parameter,
    DWORD band) {
    HWND window = g_originalCreateWindowInBand(
        exStyle, className, windowName, style, x, y, width, height,
        parent, menu, instance, parameter, band);
    TrackCreatedModernWindow(window, className);
    return window;
}

HWND WINAPI CreateWindowInBandExHook(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID parameter,
    DWORD band,
    DWORD typeFlags) {
    HWND window = g_originalCreateWindowInBandEx(
        exStyle, className, windowName, style, x, y, width, height,
        parent, menu, instance, parameter, band, typeFlags);
    TrackCreatedModernWindow(window, className);
    return window;
}

// -------------------------------------------------------------------------
// DirectWrite text layout used by Windows 11 XAML popups
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
    if (g_modernUiText.load(std::memory_order_relaxed) && text) {
        const std::wstring_view original(text, textLength);
        if (!ContainsCjkCodePoint(original) ||
            !IsModernPopupActiveForCurrentThread()) {
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
    if (g_modernUiText.load(std::memory_order_relaxed) && text) {
        const std::wstring_view original(text, textLength);
        if (!ContainsCjkCodePoint(original) ||
            !IsModernPopupActiveForCurrentThread()) {
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

bool HookDirectWrite() {
    HMODULE dwrite =
        LoadLibraryExW(L"dwrite.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dwrite) {
        Wh_Log(L"Couldn't load dwrite.dll");
        return false;
    }

    using DWriteCreateFactory_t =
        HRESULT(WINAPI*)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
    const auto createFactory = reinterpret_cast<DWriteCreateFactory_t>(
        GetProcAddress(dwrite, "DWriteCreateFactory"));
    if (!createFactory) {
        Wh_Log(L"Couldn't find DWriteCreateFactory");
        return false;
    }

    IDWriteFactory* factory = nullptr;
    const HRESULT result =
        createFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                      reinterpret_cast<IUnknown**>(&factory));
    if (FAILED(result) || !factory) {
        Wh_Log(L"DirectWrite factory creation failed: 0x%08X", result);
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(factory);
    const bool textLayoutHooked = Wh_SetFunctionHook(
        vtable[18], reinterpret_cast<void*>(CreateTextLayoutHook),
        reinterpret_cast<void**>(&g_originalCreateTextLayout));
    const bool gdiLayoutHooked = Wh_SetFunctionHook(
        vtable[19],
        reinterpret_cast<void*>(CreateGdiCompatibleTextLayoutHook),
        reinterpret_cast<void**>(
            &g_originalCreateGdiCompatibleTextLayout));

    factory->Release();

    if (!textLayoutHooked || !gdiLayoutHooked) {
        Wh_Log(L"Couldn't install one or more DirectWrite hooks");
    }

    return textLayoutHooked || gdiLayoutHooked;
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
        installedAnyHook |= InstallHook(
            CreateWindowExW, CreateWindowExWHook,
            &g_originalCreateWindowExW, L"CreateWindowExW");

        HMODULE user32Module = LoadLibraryExW(
            L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (user32Module) {
            const auto createWindowInBand =
                reinterpret_cast<CreateWindowInBand_t>(
                    GetProcAddress(user32Module, "CreateWindowInBand"));
            if (createWindowInBand) {
                installedAnyHook |= InstallHook(
                    createWindowInBand, CreateWindowInBandHook,
                    &g_originalCreateWindowInBand,
                    L"CreateWindowInBand");
            } else {
                Wh_Log(L"Couldn't find CreateWindowInBand");
            }

            const auto createWindowInBandEx =
                reinterpret_cast<CreateWindowInBandEx_t>(
                    GetProcAddress(user32Module, "CreateWindowInBandEx"));
            if (createWindowInBandEx) {
                installedAnyHook |= InstallHook(
                    createWindowInBandEx, CreateWindowInBandExHook,
                    &g_originalCreateWindowInBandEx,
                    L"CreateWindowInBandEx");
            } else {
                Wh_Log(L"Couldn't find CreateWindowInBandEx");
            }
        } else {
            Wh_Log(L"Couldn't load user32.dll");
        }

        installedAnyHook |= HookDirectWrite();
    }

    Wh_Log(L"CJK Spacer initialized (classicMenus=%d, "
           L"classicTooltips=%d, modern=%d, unicode=%d)",
           g_classicMenus.load(), g_classicTooltips.load(),
           g_modernUiText.load(),
           g_unicodeLettersAndDigits.load());
    return installedAnyHook ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    if (g_modernUiText.load(std::memory_order_relaxed)) {
        DiscoverExistingModernWindows();
    }
}

void Wh_ModUninit() {
    RestoreRewrittenMenuItems();
    ClearTrackedClassicTooltipThemes();
    ClearTrackedModernWindows();
    Wh_Log(L"CJK Spacer uninitialized");
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
