// ==WindhawkMod==
// @id              cjk-spacer
// @name            CJK Spacer
// @description     Add spaces between CJK characters and letters or digits in Explorer context menus and tooltips; modern XAML UI is opt-in and best-effort because it may conflict with other XAML Diagnostics mods
// @version         0.1.25
// @author          aenerv7
// @github          https://github.com/aenerv7
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luxtheme
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

On Windows 11, the primary Explorer and desktop context menus are modern XAML
menus. The `modernUiText` setting is disabled by default because each named
XAML Diagnostics connection permits only one consumer, and tools such as
Taskbar Styler or File Explorer Styler may already use or block those
connections. Enable it explicitly if you want those menus and pointer
tooltips processed. `classicMenus` still covers the Win32 menu opened through
"Show more options". The modern path is intentionally retained as an opt-in,
best-effort implementation because it covers the primary Windows 11 menu; the
connection conflict is left visible to the user rather than silently dropping
that coverage.

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
Hiragana, fullwidth Katakana, and halfwidth Katakana letters are all
classified as CJK characters for boundary detection; kana punctuation remains
excluded.

## Supported UI

- Classic Win32 popup menus are rewritten through public `HMENU` APIs. Menus
  created through `CreatePopupMenu` are handled while they are constructed,
  then associated with their `TrackPopupMenu` call and restored when the popup
  closes. Existing popup handles are also scanned immediately before display.
  It covers File Explorer, desktop, taskbar, and jump-list context menus that
  Explorer itself hosts.
  Context menus shown by third-party notification-area icon processes are
  outside the injected `explorer.exe` process and aren't covered.
  Classic menu bars and window system menus do not use these popup APIs and
  aren't covered.
- Classic Win32 tooltips, including legacy notification-area icon tooltips, are
  rewritten only through theme handles opened for the `TOOLTIP` class, covering
  both text measurement and drawing without touching unrelated GDI text.
  Shared theme handles retain a reference for every Tooltip window that opened
  them. A DC mapped to another window is rejected, while buffered DCs that do
  not map to a window are accepted.
  Existing tooltip controls are discovered when the mod is enabled in a
  running Explorer. Basic or classic-theme tooltips drawn directly with
  `DrawTextW` aren't covered.
- Windows 11 XAML context menus and pointer tooltips are handled at the XAML
  source-element level. Only plain local string values in the `Text` source of
  XAML menu items and `ToolTip.Content` are changed; bindings, styles,
  presenter `TextBlock` values and ordinary XAML text are untouched. Taskbar
  thumbnail preview content is normally not a plain local string and therefore
  is not changed. A source is changed when XAML Diagnostics reports
  that it was added and restored when it is removed or the mod unloads. This
  path is disabled by default; enable `modernUiText` to opt in.

The modern path supports both Windows.UI.Xaml and Microsoft.UI.Xaml content
hosted by Explorer. Each named XAML Diagnostics connection allows one consumer,
so another diagnostics-based customization tool, including Windows 11 Taskbar
Styler or Windows 11 File Explorer Styler, can prevent this path from
initializing. Connection attempts are scheduled outside the Windows loader
  path; a single-flight worker retains requests that arrive while it is running,
  tracks the Windows.UI.Xaml and Microsoft.UI.Xaml connections independently,
  and retries only the connection that was lost. The worker pins the mod image
  until `FreeLibraryAndExitThread`, so it cannot execute unloaded mod code.
  `Wh_ModAfterInit` covers XAML modules that are already loaded, while the
  `LoadLibraryExW` hook covers modules loaded later, including the
  `CoreMessagingXP.dll` static-import path used by
  `Microsoft.Internal.FrameworkUdk.dll`.
  If a connection attempt is denied or otherwise fails, a newly loaded XAML
  module or an established connection being disconnected provides the next
  retry opportunity.
When Taskbar Styler is configured to alert on competing XAML Diagnostics
consumers, enabling this path can show its confirmation dialog.
The mod is injected only into `explorer.exe`. Start, Search, and some shell
flyouts hosted by `StartMenuExperienceHost.exe`, `SearchHost.exe`, or
`ShellExperienceHost.exe` are outside its scope.

The mod doesn't edit system files, registry values, or file names. A file name
shown in a classic menu can nevertheless be displayed with inserted spaces.
The modern path temporarily changes target source values on their XAML UI
threads and restores them when the source leaves the visual tree or the mod
unloads. The classic path updates
strings stored in active `HMENU` objects, so menu text read through
accessibility APIs also contains the inserted spaces while the popup is open.
Other menu-cleanup mods that match items by their displayed text, such as
Remove Context Menu Items, may therefore need rules that account for the
temporary spaces while the popup is open.
Text Replace targets the same classic menu APIs but performs literal
substitutions, so it cannot express this CJK/non-CJK boundary rule; when both
mods are enabled, their hook order can affect which temporary text each sees.
Rewritten strings are recorded and restored when the popup closes. If multiple
items have identical rewritten text, text-based fallback restoration can select
the first match; the idempotent transformation does not compound spaces.
If a displayed popup item is changed dynamically, Windows may not remeasure it;
the added spaces can clip on uncommon paths. Tooltip measurement and drawing
also need compatible DC targets to keep sizing consistent; disable
`classicTooltips` if a specific control clips after spacing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- classicMenus: true
  $name: Classic context menus
  $description: Process classic Win32 popup-menu text during construction and display; temporary live-menu changes can affect other text-matching menu mods until the popup closes.
- classicTooltips: true
  $name: Classic Win32 tooltips
  $description: Process text in legacy tooltips such as notification-area icon tooltips.
- modernUiText: false
  $name: Windows 11 context menus and tooltips
  $description: Process text elements in Explorer XAML context menus and pointer tooltips (disabled by default; enable for the primary Windows 11 menu). Each named XAML Diagnostics connection has one consumer, so this can conflict with Windows 11 Taskbar Styler, Windows 11 File Explorer Styler, or other diagnostics-based tools.
- characterMode: unicode
  $name: Non-CJK character set
  $description: Choose which letters and digits form a spacing boundary with CJK.
  $options:
  - unicode: Unicode letters and digits
  - ascii: ASCII letters and digits only
*/
// ==/WindhawkModSettings==

#include <xamlom.h>

#include <windhawk_utils.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <functional>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
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
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
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

    // CJK radicals, Kangxi radicals, Hiragana, fullwidth Katakana, Bopomofo,
    // Hangul compatibility Jamo, CJK strokes, and Katakana phonetic
    // extensions. All Hiragana and Katakana letters, including halfwidth
    // Katakana below, are treated as CJK for spacing purposes. Kana/CJK
    // punctuation is deliberately excluded except for the ideographic
    // iteration mark and ideographic zero.
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

    // GetStringTypeW classifies UTF-16 code units. Supplementary-plane
    // letters represented by surrogate pairs therefore aren't reliably
    // reported as Unicode letters/digits and remain Other in unicode mode.
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
using CreatePopupMenu_t = decltype(&CreatePopupMenu);
using DestroyMenu_t = decltype(&DestroyMenu);
using TrackPopupMenu_t = decltype(&TrackPopupMenu);
using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);

InsertMenuW_t g_originalInsertMenuW;
AppendMenuW_t g_originalAppendMenuW;
ModifyMenuW_t g_originalModifyMenuW;
InsertMenuItemW_t g_originalInsertMenuItemW;
SetMenuItemInfoW_t g_originalSetMenuItemInfoW;
CreatePopupMenu_t g_originalCreatePopupMenu;
DestroyMenu_t g_originalDestroyMenu;
TrackPopupMenu_t g_originalTrackPopupMenu;
TrackPopupMenuEx_t g_originalTrackPopupMenuEx;

struct RewrittenMenuItem {
    HMENU rootMenu;
    HMENU menu;
    UINT index;
    std::wstring original;
    std::wstring spaced;
};

constexpr UINT kUnknownMenuItemIndex = static_cast<UINT>(-1);

std::mutex g_rewrittenMenuItemsMutex;
std::vector<RewrittenMenuItem> g_rewrittenMenuItems;
std::atomic_uint g_rewrittenMenuItemCount{0};
thread_local unsigned int g_classicPopupMenuDepth = 0;
thread_local HMENU g_classicPopupRootMenu = nullptr;
thread_local bool g_restoringClassicMenuText = false;

std::shared_mutex g_createdClassicPopupMenusMutex;
std::unordered_set<HMENU> g_createdClassicPopupMenus;
std::atomic_uint g_createdClassicPopupMenuCount{0};

constexpr unsigned int kMaxMenuDepth = 16;

class ScopedBoolValue final {
public:
    ScopedBoolValue(bool& value, bool replacement)
        : m_value(value), m_previous(value) {
        m_value = replacement;
    }

    ~ScopedBoolValue() {
        m_value = m_previous;
    }

private:
    bool& m_value;
    bool m_previous;
};

class ClassicPopupMenuStateGuard final {
public:
    explicit ClassicPopupMenuStateGuard(HMENU rootMenu)
        : m_previousRootMenu(g_classicPopupRootMenu) {
        g_classicPopupRootMenu = rootMenu;
        ++g_classicPopupMenuDepth;
    }

    ~ClassicPopupMenuStateGuard() {
        --g_classicPopupMenuDepth;
        g_classicPopupRootMenu = m_previousRootMenu;
    }

private:
    HMENU m_previousRootMenu;
};

void RememberRewrittenMenuItem(HMENU menu,
                               UINT index,
                               std::wstring original,
                               std::wstring spaced);
int FindMenuItemIndexByText(
    HMENU menu,
    const std::wstring& expectedText);

void CollectMenuTreeHandles(
    HMENU menu,
    std::unordered_set<HMENU>* handles,
    unsigned int depth = 0) {
    if (!menu || !handles || depth >= kMaxMenuDepth ||
        !handles->insert(menu).second) {
        return;
    }

    const int itemCount = GetMenuItemCount(menu);
    for (int index = 0; index < itemCount; ++index) {
        MENUITEMINFOW itemInfo = {};
        itemInfo.cbSize = sizeof(itemInfo);
        itemInfo.fMask = MIIM_SUBMENU;
        if (GetMenuItemInfoW(menu, index, TRUE, &itemInfo) &&
            itemInfo.hSubMenu) {
            CollectMenuTreeHandles(
                itemInfo.hSubMenu, handles, depth + 1);
        }
    }
}

bool IsCreatedClassicPopupMenu(HMENU menu) {
    if (!menu) {
        return false;
    }

    std::shared_lock<std::shared_mutex> guard(
        g_createdClassicPopupMenusMutex);
    return g_createdClassicPopupMenus.contains(menu);
}

bool ShouldRewriteClassicMenuDuringConstruction(HMENU menu) {
    return g_classicPopupMenuDepth > 0 ||
           IsCreatedClassicPopupMenu(menu);
}

HMENU WINAPI CreatePopupMenuHook() {
    HMENU menu = g_originalCreatePopupMenu();
    if (menu) {
        std::lock_guard<std::shared_mutex> guard(
            g_createdClassicPopupMenusMutex);
        constexpr size_t kCreatedPopupCleanupThreshold = 256;
        if (g_createdClassicPopupMenus.size() >=
            kCreatedPopupCleanupThreshold) {
            std::erase_if(g_createdClassicPopupMenus,
                          [](HMENU candidate) {
                              return !IsMenu(candidate);
                          });
        }
        g_createdClassicPopupMenus.insert(menu);
        g_createdClassicPopupMenuCount.store(
            static_cast<unsigned int>(
                g_createdClassicPopupMenus.size()),
            std::memory_order_relaxed);
    }
    return menu;
}

BOOL WINAPI DestroyMenuHook(HMENU menu) {
    if (g_rewrittenMenuItemCount.load(
            std::memory_order_relaxed) == 0 &&
        g_createdClassicPopupMenuCount.load(
            std::memory_order_relaxed) == 0) {
        return g_originalDestroyMenu(menu);
    }

    std::unordered_set<HMENU> menuTree;
    CollectMenuTreeHandles(menu, &menuTree);

    std::vector<RewrittenMenuItem> removedItems;

    {
        std::lock_guard<std::mutex> guard(
            g_rewrittenMenuItemsMutex);
        for (auto iterator = g_rewrittenMenuItems.begin();
             iterator != g_rewrittenMenuItems.end();) {
            if (menuTree.contains(iterator->menu)) {
                removedItems.push_back(std::move(*iterator));
                iterator = g_rewrittenMenuItems.erase(iterator);
            } else {
                ++iterator;
            }
        }
        g_rewrittenMenuItemCount.store(
            static_cast<unsigned int>(g_rewrittenMenuItems.size()),
            std::memory_order_relaxed);
    }

    {
        std::lock_guard<std::shared_mutex> guard(
            g_createdClassicPopupMenusMutex);
        for (HMENU handle : menuTree) {
            g_createdClassicPopupMenus.erase(handle);
        }
        g_createdClassicPopupMenuCount.store(
            static_cast<unsigned int>(
                g_createdClassicPopupMenus.size()),
            std::memory_order_relaxed);
    }

    const BOOL result = g_originalDestroyMenu(menu);
    if (!result) {
        {
            std::lock_guard<std::mutex> guard(
                g_rewrittenMenuItemsMutex);
            for (auto& item : removedItems) {
                g_rewrittenMenuItems.push_back(std::move(item));
            }
            g_rewrittenMenuItemCount.store(
                static_cast<unsigned int>(
                    g_rewrittenMenuItems.size()),
                std::memory_order_relaxed);
        }

        {
            std::lock_guard<std::shared_mutex> guard(
                g_createdClassicPopupMenusMutex);
            for (HMENU handle : menuTree) {
                if (IsMenu(handle)) {
                    g_createdClassicPopupMenus.insert(handle);
                }
            }
            g_createdClassicPopupMenuCount.store(
                static_cast<unsigned int>(
                    g_createdClassicPopupMenus.size()),
                std::memory_order_relaxed);
        }
    }

    return result;
}

void AssociatePendingRewrittenMenuItems(HMENU rootMenu) {
    std::unordered_set<HMENU> menuTree;
    CollectMenuTreeHandles(rootMenu, &menuTree);

    std::lock_guard<std::mutex> guard(g_rewrittenMenuItemsMutex);
    for (auto& item : g_rewrittenMenuItems) {
        if (!item.rootMenu && menuTree.contains(item.menu)) {
            item.rootMenu = rootMenu;
        }
    }
}

void ClearCreatedClassicPopupMenus() {
    std::lock_guard<std::shared_mutex> guard(
        g_createdClassicPopupMenusMutex);
    g_createdClassicPopupMenus.clear();
    g_createdClassicPopupMenuCount.store(0,
                                         std::memory_order_relaxed);
}

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
        itemInfo.fMask = MIIM_ID | MIIM_SUBMENU;
        if (GetMenuItemInfoW(
                menu, index, TRUE, &itemInfo) &&
            !itemInfo.hSubMenu &&
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
    if (!g_restoringClassicMenuText &&
        ShouldRewriteClassicMenuDuringConstruction(menu) &&
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
                    if (index < 0) {
                        index = GetMenuItemCount(menu) - 1;
                    }
                } else if (flags & MF_POPUP) {
                    index = FindMenuItemIndexByText(
                        menu, spaced);
                } else {
                    index = FindMenuItemIndex(
                        menu, static_cast<UINT>(itemId),
                        false);
                }
                RememberRewrittenMenuItem(
                    menu,
                    index >= 0 ? static_cast<UINT>(index)
                               : kUnknownMenuItemIndex,
                    newItem, spaced);
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
    if (!g_restoringClassicMenuText &&
        ShouldRewriteClassicMenuDuringConstruction(menu) &&
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
    if (!g_restoringClassicMenuText &&
        ShouldRewriteClassicMenuDuringConstruction(menu) &&
        g_classicMenus.load(std::memory_order_relaxed) && newItem &&
        IsStringMenuFlags(flags) && ContainsCjkCodePoint(newItem)) {
        const std::wstring spaced = AddCjkSpacing(newItem);
        if (spaced != newItem) {
            Wh_Log(L"Applied CJK spacing (classic/ModifyMenuW)");
            const int index = FindMenuItemIndex(
                menu, position, (flags & MF_BYPOSITION) != 0);
            const BOOL result = g_originalModifyMenuW(
                menu, position, flags, itemId, spaced.c_str());
            if (result) {
                RememberRewrittenMenuItem(
                    menu,
                    index >= 0 ? static_cast<UINT>(index)
                               : kUnknownMenuItemIndex,
                    newItem, spaced);
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
    // Keep the copied structure self-consistent when callers provide a
    // smaller version of MENUITEMINFOW.
    copy.cbSize = static_cast<UINT>(copySize);
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

    constexpr size_t kRewrittenMenuCleanupThreshold = 256;
    if (g_rewrittenMenuItems.size() >=
        kRewrittenMenuCleanupThreshold) {
        // Popup teardown normally clears this vector. If a menu is destroyed
        // before teardown, remove only entries that can no longer be restored;
        // never evict a live entry just to enforce an arbitrary cap.
        std::erase_if(g_rewrittenMenuItems, [](const auto& item) {
            return !IsMenu(item.menu);
        });
    }

    g_rewrittenMenuItems.push_back(
        {g_classicPopupRootMenu, menu, index, std::move(original),
         std::move(spaced)});
    g_rewrittenMenuItemCount.store(
        static_cast<unsigned int>(g_rewrittenMenuItems.size()),
        std::memory_order_relaxed);
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
        g_rewrittenMenuItemCount.store(
            static_cast<unsigned int>(g_rewrittenMenuItems.size()),
            std::memory_order_relaxed);
    }

    ScopedBoolValue restoringGuard(g_restoringClassicMenuText, true);
    for (auto iterator = items.rbegin(); iterator != items.rend();
         ++iterator) {
        if (!IsMenu(iterator->menu)) {
            continue;
        }

        int index = iterator->index == kUnknownMenuItemIndex
                        ? -1
                        : static_cast<int>(iterator->index);
        std::wstring current;
        if (index < 0 ||
            !ReadMenuItemText(
                iterator->menu, static_cast<UINT>(index), &current) ||
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
}

BOOL WINAPI InsertMenuItemWHook(HMENU menu,
                                UINT item,
                                BOOL byPosition,
                                LPCMENUITEMINFOW itemInfo) {
    if (!g_restoringClassicMenuText &&
        ShouldRewriteClassicMenuDuringConstruction(menu) &&
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
                // Without MIIM_ID, item identifies the insertion anchor, not
                // the new item. Leave the index unknown so restoration uses
                // the spaced text fallback instead of recording the anchor.
                const int index =
                    !byPosition && !(itemInfo->fMask & MIIM_ID)
                        ? -1
                        : FindMenuItemIndex(
                              menu,
                              byPosition ? item : itemInfo->wID,
                              byPosition != FALSE);
                const int insertedIndex =
                    index >= 0
                        ? index
                        : byPosition
                              ? GetMenuItemCount(menu) - 1
                              : FindMenuItemIndexByText(menu, spaced);
                RememberRewrittenMenuItem(
                    menu,
                    insertedIndex >= 0
                        ? static_cast<UINT>(insertedIndex)
                        : kUnknownMenuItemIndex,
                    itemInfo->dwTypeData, spaced);
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
    if (!g_restoringClassicMenuText &&
        ShouldRewriteClassicMenuDuringConstruction(menu) &&
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
            if (result) {
                RememberRewrittenMenuItem(
                    menu,
                    index >= 0 ? static_cast<UINT>(index)
                               : kUnknownMenuItemIndex,
                    itemInfo->dwTypeData, spaced);
            }
            return result;
        }
    }

    return g_originalSetMenuItemInfoW(menu, item, byPosition, itemInfo);
}

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
        ClassicPopupMenuStateGuard stateGuard(menu);
        AssociatePendingRewrittenMenuItems(menu);
        RewriteMenuTree(menu);
        const BOOL result = g_originalTrackPopupMenu(
            menu, flags, x, y, reserved, owner, rect);
        RestoreRewrittenMenuItems(menu);
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
        ClassicPopupMenuStateGuard stateGuard(menu);
        AssociatePendingRewrittenMenuItems(menu);
        RewriteMenuTree(menu);
        const BOOL result = g_originalTrackPopupMenuEx(
            menu, flags, x, y, owner, parameters);
        RestoreRewrittenMenuItems(menu);
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
struct ClassicTooltipTheme {
    unsigned int references = 0;
    bool hasTooltipWindow = false;
};

// A theme handle can be shared by multiple tooltip windows. Count every
// matching OpenThemeData call, while separately remembering whether at least
// one real tooltips_class32 window owns the handle for draw-time filtering.
std::unordered_map<HTHEME, ClassicTooltipTheme> g_classicTooltipThemes;
std::vector<HTHEME> g_discoveredClassicTooltipThemes;
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

bool IsClassicTooltipWindow(HWND window);

bool IsTrackedClassicTooltipTheme(HTHEME theme) {
    if (!theme ||
        g_classicTooltipThemeCount.load(
            std::memory_order_relaxed) == 0) {
        return false;
    }

    std::shared_lock<std::shared_mutex> guard(
        g_classicTooltipThemesMutex);
    const auto iterator = g_classicTooltipThemes.find(theme);
    return iterator != g_classicTooltipThemes.end() &&
           iterator->second.hasTooltipWindow;
}

void TrackClassicTooltipTheme(HTHEME theme, HWND window) {
    if (!theme) {
        return;
    }

    bool inserted;
    {
        std::lock_guard<std::shared_mutex> guard(
            g_classicTooltipThemesMutex);
        auto [iterator, wasInserted] =
            g_classicTooltipThemes.try_emplace(theme);
        inserted = wasInserted;
        ++iterator->second.references;
        if (window && IsClassicTooltipWindow(window)) {
            iterator->second.hasTooltipWindow = true;
        }
        g_classicTooltipThemeCount.store(
            static_cast<unsigned int>(g_classicTooltipThemes.size()),
            std::memory_order_relaxed);
    }

    if (inserted) {
        Wh_Log(L"Tracking classic Win32 tooltip theme");
    }
}

bool UntrackClassicTooltipTheme(HTHEME theme) {
    if (!theme ||
        g_classicTooltipThemeCount.load(
            std::memory_order_relaxed) == 0) {
        return false;
    }

    {
        std::shared_lock<std::shared_mutex> guard(
            g_classicTooltipThemesMutex);
        if (!g_classicTooltipThemes.contains(theme)) {
            return false;
        }
    }

    std::lock_guard<std::shared_mutex> guard(
        g_classicTooltipThemesMutex);
    const auto iterator = g_classicTooltipThemes.find(theme);
    if (iterator == g_classicTooltipThemes.end()) {
        return false;
    }
    if (--iterator->second.references == 0) {
        g_classicTooltipThemes.erase(iterator);
        g_classicTooltipThemeCount.store(
            static_cast<unsigned int>(g_classicTooltipThemes.size()),
            std::memory_order_relaxed);
    }
    return true;
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
    if (!IsTrackedClassicTooltipTheme(theme)) {
        return false;
    }

    const HWND window = dc ? WindowFromDC(dc) : nullptr;
    // Buffered theme drawing may not expose an HWND through WindowFromDC.
    // When it does, accept any actual classic tooltip window: a single HTHEME
    // is commonly shared by several tooltips in the same process.
    return !window || IsClassicTooltipWindow(window);
}

BOOL CALLBACK EnumExistingClassicTooltipWindow(HWND window, LPARAM) {
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId() &&
        IsClassicTooltipWindow(window) && g_getWindowTheme &&
        g_originalOpenThemeData && g_originalCloseThemeData) {
        // GetWindowTheme does not add a theme-data reference. Open the
        // handle ourselves so the discovery entry can be released exactly
        // once during module teardown.
        const HTHEME windowTheme = g_getWindowTheme(window);
        if (HTHEME theme =
                g_originalOpenThemeData(window, L"TOOLTIP")) {
            if (theme == windowTheme) {
                TrackClassicTooltipTheme(theme, window);
                g_discoveredClassicTooltipThemes.push_back(theme);
            } else {
                // A different DPI/theme context may resolve a different
                // cached handle. Do not retain a handle the control does not
                // actually use.
                g_originalCloseThemeData(theme);
            }
        }
    }

    return TRUE;
}

void DiscoverExistingClassicTooltipThemes() {
    if (g_getWindowTheme && g_originalOpenThemeData &&
        g_originalCloseThemeData) {
        EnumWindows(EnumExistingClassicTooltipWindow, 0);
    }
}

void CloseDiscoveredClassicTooltipThemes() {
    // Windhawk removes hooks before Wh_ModUninit. Use the linked import here,
    // not the trampoline that was valid while the hooks were active.
    for (HTHEME theme : g_discoveredClassicTooltipThemes) {
        CloseThemeData(theme);
    }
    g_discoveredClassicTooltipThemes.clear();
}

HTHEME WINAPI OpenThemeDataHook(HWND window, LPCWSTR classList) {
    HTHEME theme = g_originalOpenThemeData(window, classList);
    if (IsClassicTooltipThemeClassList(classList)) {
        TrackClassicTooltipTheme(theme, window);
    }

    return theme;
}

HTHEME WINAPI OpenThemeDataExHook(HWND window,
                                  LPCWSTR classList,
                                  DWORD flags) {
    HTHEME theme =
        g_originalOpenThemeDataEx(window, classList, flags);
    if (IsClassicTooltipThemeClassList(classList)) {
        TrackClassicTooltipTheme(theme, window);
    }

    return theme;
}

HTHEME WINAPI OpenThemeDataForDpiHook(HWND window,
                                      LPCWSTR classList,
                                      UINT dpi) {
    HTHEME theme =
        g_originalOpenThemeDataForDpi(window, classList, dpi);
    if (IsClassicTooltipThemeClassList(classList)) {
        TrackClassicTooltipTheme(theme, window);
    }

    return theme;
}

HRESULT WINAPI CloseThemeDataHook(HTHEME theme) {
    const HRESULT result = g_originalCloseThemeData(theme);
    if (SUCCEEDED(result)) {
        UntrackClassicTooltipTheme(theme);
    }
    return result;
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
    ScopedBoolValue rewritingGuard(
        g_rewritingClassicTooltipText, true);
    const HRESULT result = g_originalGetThemeTextExtent(
        theme, dc, partId, stateId, spaced.c_str(),
        static_cast<int>(spaced.size()), textFlags,
        boundingRectangle, extentRectangle);
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
    ScopedBoolValue rewritingGuard(
        g_rewritingClassicTooltipText, true);
    const HRESULT result = g_originalDrawThemeText(
        theme, dc, partId, stateId, spaced.c_str(),
        static_cast<int>(spaced.size()), textFlags, textFlags2,
        rectangle);
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
    ScopedBoolValue rewritingGuard(
        g_rewritingClassicTooltipText, true);
    const HRESULT result = g_originalDrawThemeTextEx(
        theme, dc, partId, stateId, spaced.c_str(),
        static_cast<int>(spaced.size()), textFlags, rectangle,
        options);
    return result;
}

// -------------------------------------------------------------------------
// Windows 11 XAML context menus and tooltips
// -------------------------------------------------------------------------

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace mux = winrt::Microsoft::UI::Xaml;

template <typename Element, typename DependencyProperty>
bool ReadLocalStringValue(const Element& element,
                          DependencyProperty property,
                          std::wstring* text) {
    const auto localValue = element.ReadLocalValue(property);
    if (!localValue ||
        localValue == DependencyProperty::UnsetValue()) {
        return false;
    }

    const auto propertyValue =
        localValue.template try_as<wf::IPropertyValue>();
    if (!propertyValue ||
        propertyValue.Type() != wf::PropertyType::String) {
        return false;
    }

    const auto value = propertyValue.GetString();
    text->assign(value.c_str(), value.size());
    return true;
}

struct TextSourceAccess {
    template <typename Element>
    static bool Read(const Element& element, std::wstring* text) {
        return ReadLocalStringValue(
            element, Element::TextProperty(), text);
    }

    template <typename Element>
    static void Write(const Element& element, std::wstring_view text) {
        element.Text(winrt::hstring(text));
    }

    static constexpr PCWSTR Name() {
        return L"menu source";
    }
};

template <typename ContentControl>
struct TooltipContentAccess {
    template <typename Element>
    static bool Read(const Element& element, std::wstring* text) {
        return ReadLocalStringValue(
            element, ContentControl::ContentProperty(), text);
    }

    template <typename Element>
    static void Write(const Element& element, std::wstring_view text) {
        element.Content(winrt::box_value(winrt::hstring(text)));
    }

    static constexpr PCWSTR Name() {
        return L"tooltip source";
    }
};

class ModernTextStateBase {
public:
    virtual ~ModernTextStateBase() = default;
    virtual bool Apply() = 0;
    virtual void Restore() = 0;
};

template <typename Element, typename SourceAccess>
class ModernTextState final : public ModernTextStateBase {
public:
    explicit ModernTextState(const Element& element)
        : m_element(winrt::make_weak(element)) {}

    bool Apply() override {
        try {
            auto element = m_element.get();
            if (!element) {
                return false;
            }

            std::wstring current;
            if (!SourceAccess::Read(element, &current)) {
                return false;
            }

            if (!ContainsCjkCodePoint(current)) {
                return false;
            }

            std::wstring spaced = AddCjkSpacing(current, false);
            if (spaced.size() == current.size()) {
                return false;
            }

            m_originalText = std::move(current);
            m_spacedText = std::move(spaced);
            SourceAccess::Write(element, m_spacedText);
            m_modified = true;
            Wh_Log(L"Applied CJK spacing (modern XAML %s)",
                   SourceAccess::Name());
            return true;
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"Couldn't update modern XAML source: 0x%08X",
                   static_cast<HRESULT>(error.code()));
            return false;
        }
    }

    void Restore() override {
        if (!m_modified) {
            return;
        }

        try {
            auto element = m_element.get();
            if (element) {
                std::wstring current;
                if (SourceAccess::Read(element, &current) &&
                    current == m_spacedText) {
                    SourceAccess::Write(element, m_originalText);
                }
            }
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"Couldn't restore modern XAML source: 0x%08X",
                   static_cast<HRESULT>(error.code()));
        }

        m_modified = false;
        m_originalText.clear();
        m_spacedText.clear();
    }

private:
    winrt::weak_ref<Element> m_element;
    bool m_modified = false;
    std::wstring m_originalText;
    std::wstring m_spacedText;
};

struct ModernElementKey {
    const void* watcher;
    InstanceHandle handle;

    bool operator==(const ModernElementKey&) const = default;
};

struct ModernElementKeyHash {
    size_t operator()(const ModernElementKey& key) const {
        const auto watcher =
            reinterpret_cast<uintptr_t>(key.watcher);
        return std::hash<uintptr_t>{}(watcher) ^
               (std::hash<InstanceHandle>{}(key.handle) << 1);
    }
};

bool IsModernMenuSourceType(std::wstring_view type) {
    return type.ends_with(L".MenuFlyoutItem") ||
           type.ends_with(L".ToggleMenuFlyoutItem") ||
           type.ends_with(L".RadioMenuFlyoutItem") ||
           type.ends_with(L".MenuFlyoutSubItem");
}

bool IsModernTooltipSourceType(std::wstring_view type) {
    return type.ends_with(L".ToolTip");
}

using ModernTextStateMap =
    std::unordered_map<ModernElementKey,
                       std::unique_ptr<ModernTextStateBase>,
                       ModernElementKeyHash>;

struct ModernThreadState {
    ModernTextStateMap states;
};

thread_local std::optional<ModernThreadState>
    g_modernTextStatesForThread{std::in_place};

extern std::atomic_bool g_stoppingModernUi;
std::mutex g_modernTextStateThreadsMutex;
std::unordered_map<DWORD, uint64_t> g_modernTextStateThreads;

bool GetThreadCreationTime(HANDLE thread, uint64_t* creationTime) {
    FILETIME creation;
    FILETIME exit;
    FILETIME kernel;
    FILETIME user;
    if (!GetThreadTimes(thread, &creation, &exit, &kernel, &user)) {
        return false;
    }

    ULARGE_INTEGER value;
    value.LowPart = creation.dwLowDateTime;
    value.HighPart = creation.dwHighDateTime;
    *creationTime = value.QuadPart;
    return true;
}

bool IsRegisteredThreadAlive(DWORD threadId, uint64_t creationTime) {
    HANDLE thread = OpenThread(
        SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION,
        FALSE, threadId);
    if (!thread) {
        return false;
    }

    uint64_t currentCreationTime;
    const bool alive =
        WaitForSingleObject(thread, 0) == WAIT_TIMEOUT &&
        GetThreadCreationTime(thread, &currentCreationTime) &&
        currentCreationTime == creationTime;
    CloseHandle(thread);
    return alive;
}

void PruneModernTextStateThreadsLocked() {
    for (auto iterator = g_modernTextStateThreads.begin();
         iterator != g_modernTextStateThreads.end();) {
        if (!IsRegisteredThreadAlive(iterator->first,
                                     iterator->second)) {
            iterator = g_modernTextStateThreads.erase(iterator);
        } else {
            ++iterator;
        }
    }
}

bool RegisterModernTextStateThread() {
    uint64_t creationTime;
    if (!GetThreadCreationTime(GetCurrentThread(),
                               &creationTime)) {
        Wh_Log(L"Couldn't read the current thread creation time");
        return false;
    }

    std::lock_guard<std::mutex> guard(
        g_modernTextStateThreadsMutex);
    // Pair the thread ID with its creation time. A dead UI thread's ID can be
    // recycled by a new thread that also owns windows; without this fingerprint
    // unload could dispatch cleanup into that unrelated thread.
    if (g_stoppingModernUi.load(std::memory_order_acquire)) {
        return false;
    }
    PruneModernTextStateThreadsLocked();
    g_modernTextStateThreads[GetCurrentThreadId()] = creationTime;
    return true;
}

void UnregisterModernTextStateThread() {
    std::lock_guard<std::mutex> guard(
        g_modernTextStateThreadsMutex);
    g_modernTextStateThreads.erase(GetCurrentThreadId());
}

void RemoveModernTextState(ModernThreadState& threadState,
                           const ModernElementKey& key) {
    auto node = threadState.states.extract(key);
    if (node.empty()) {
        return;
    }

    node.mapped()->Restore();
}

void CleanupModernTextStatesForCurrentThread() {
    if (!g_modernTextStatesForThread) {
        return;
    }

    ModernTextStateMap states;
    states.swap(g_modernTextStatesForThread->states);
    g_modernTextStatesForThread.reset();
    UnregisterModernTextStateThread();
    for (const auto& [key, state] : states) {
        state->Restore();
    }
}

HMODULE AcquireCurrentModuleReference();
enum class XamlDiagnosticsFlavor {
    None,
    Windows,
    Microsoft,
};

void RequestModernXamlDiagnosticsInitialization(
    XamlDiagnosticsFlavor retryFlavor = XamlDiagnosticsFlavor::None);

extern std::atomic_bool g_windowsUiXamlDiagnosticsConnected;
extern std::atomic_bool g_microsoftUiXamlDiagnosticsConnected;
extern std::atomic_bool g_windowsUiXamlDiagnosticsAttempted;
extern std::atomic_bool g_microsoftUiXamlDiagnosticsAttempted;
extern thread_local XamlDiagnosticsFlavor g_connectingXamlDiagnostics;

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_xamlDiagnostics(site.as<IXamlDiagnostics>()) {
        m_moduleReference = AcquireCurrentModuleReference();
        if (!m_moduleReference) {
            winrt::throw_hresult(HRESULT_FROM_WIN32(GetLastError()));
        }

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
                } else {
                    watcher->m_advised.store(
                        true, std::memory_order_release);
                    if (watcher->m_unadviseRequested.load(
                            std::memory_order_acquire)) {
                        watcher->UnadviseVisualTreeChange();
                    }
                }
                HMODULE module = watcher->m_moduleReference;
                watcher->Release();
                FreeLibraryAndExitThread(module, 0);
            },
            this, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            const DWORD error = GetLastError();
            Release();
            FreeLibrary(m_moduleReference);
            m_moduleReference = nullptr;
            Wh_Log(L"Couldn't create XAML watcher thread");
            winrt::throw_hresult(HRESULT_FROM_WIN32(error));
        }
    }

    void UnadviseVisualTreeChange() noexcept {
        m_unadviseRequested.store(true,
                                  std::memory_order_release);
        if (!m_advised.exchange(false,
                                std::memory_order_acq_rel)) {
            return;
        }

        HRESULT result;
        try {
            result = m_xamlDiagnostics.as<IVisualTreeService3>()
                         ->UnadviseVisualTreeChange(this);
        } catch (...) {
            result = winrt::to_hresult();
        }
        if (FAILED(result)) {
            Wh_Log(L"UnadviseVisualTreeChange failed: 0x%08X",
                   result);
        }
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

    template <typename Element, typename SourceAccess>
    bool TryRegisterSource(InstanceHandle handle,
                           const wf::IInspectable& inspectable) {
        auto element = inspectable.try_as<Element>();
        if (!element) {
            return false;
        }

        const ModernElementKey key{this, handle};
        auto& threadState = *g_modernTextStatesForThread;
        if (threadState.states.contains(key)) {
            RemoveModernTextState(threadState, key);
        }

        auto state =
            std::make_unique<ModernTextState<Element, SourceAccess>>(
                element);
        if (!state->Apply()) {
            if (threadState.states.empty()) {
                UnregisterModernTextStateThread();
            }
            return false;
        }

        if (threadState.states.empty() &&
            !RegisterModernTextStateThread()) {
            state->Restore();
            return false;
        }
        threadState.states.emplace(key, std::move(state));
        return true;
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) override try {
        if (g_stoppingModernUi.load(std::memory_order_acquire)) {
            return S_OK;
        }

        if (!g_modernTextStatesForThread) {
            g_modernTextStatesForThread.emplace();
        }

        const ModernElementKey key{this, element.Handle};
        auto& threadState = *g_modernTextStatesForThread;
        auto& states = threadState.states;

        if (mutationType == Add &&
            g_modernUiText.load(std::memory_order_relaxed) &&
            !g_stoppingModernUi.load(std::memory_order_relaxed)) {
            const std::wstring_view type(
                element.Type ? element.Type : L"");
            const bool isMenuSource = IsModernMenuSourceType(type);
            const bool isTooltipSource =
                IsModernTooltipSourceType(type);
            if (!isMenuSource && !isTooltipSource) {
                return S_OK;
            }

            const auto inspectable = FromHandle(element.Handle);
            if (isMenuSource) {
                if (TryRegisterSource<
                        wux::Controls::MenuFlyoutItem,
                        TextSourceAccess>(
                        element.Handle, inspectable) ||
                    TryRegisterSource<
                        wux::Controls::MenuFlyoutSubItem,
                        TextSourceAccess>(
                        element.Handle, inspectable) ||
                    TryRegisterSource<
                        mux::Controls::MenuFlyoutItem,
                        TextSourceAccess>(
                        element.Handle, inspectable) ||
                    TryRegisterSource<
                        mux::Controls::MenuFlyoutSubItem,
                        TextSourceAccess>(
                        element.Handle, inspectable)) {
                    return S_OK;
                }
            } else {
                if (TryRegisterSource<
                        wux::Controls::ToolTip,
                        TooltipContentAccess<
                            wux::Controls::ContentControl>>(
                        element.Handle, inspectable) ||
                    TryRegisterSource<
                        mux::Controls::ToolTip,
                        TooltipContentAccess<
                            mux::Controls::ContentControl>>(
                        element.Handle, inspectable)) {
                    return S_OK;
                }
            }
        } else if (mutationType == Remove) {
            if (states.contains(key)) {
                RemoveModernTextState(threadState, key);
                if (states.empty()) {
                    UnregisterModernTextStateThread();
                }
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
    HMODULE m_moduleReference = nullptr;
    std::atomic_bool m_advised{false};
    std::atomic_bool m_unadviseRequested{false};
};

std::mutex g_visualTreeWatchersMutex;
[[clang::no_destroy]]
std::optional<std::vector<winrt::com_ptr<VisualTreeWatcher>>>
        g_visualTreeWatchers{std::in_place};
std::atomic_bool g_stoppingModernUi{false};
std::atomic_uint64_t g_visualTreeWatcherGeneration;

bool RemoveVisualTreeWatcher(VisualTreeWatcher* watcher) {
    std::lock_guard<std::mutex> guard(g_visualTreeWatchersMutex);
    if (!g_visualTreeWatchers) {
        return false;
    }

    const size_t previousSize = g_visualTreeWatchers->size();
    std::erase_if(*g_visualTreeWatchers,
                  [watcher](const auto& candidate) {
                      return candidate.get() == watcher;
                  });
    return g_visualTreeWatchers->size() != previousSize;
}

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

HMODULE AcquireCurrentModuleReference() {
    HMODULE module;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR>(
                &AcquireCurrentModuleReference),
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
        if (m_watcher) {
            if (RemoveVisualTreeWatcher(m_watcher.get())) {
                m_watcher->UnadviseVisualTreeChange();
            }
            m_watcher = nullptr;
        }

        m_site.copy_from(site);
        if (!site) {
            const XamlDiagnosticsFlavor disconnectedFlavor =
                m_flavor;
            m_flavor = XamlDiagnosticsFlavor::None;
            if (disconnectedFlavor ==
                XamlDiagnosticsFlavor::Windows) {
                g_windowsUiXamlDiagnosticsConnected.store(
                    false, std::memory_order_release);
            } else if (disconnectedFlavor ==
                       XamlDiagnosticsFlavor::Microsoft) {
                g_microsoftUiXamlDiagnosticsConnected.store(
                    false, std::memory_order_release);
            }
            if (disconnectedFlavor != XamlDiagnosticsFlavor::None &&
                !g_stoppingModernUi.load(
                    std::memory_order_relaxed)) {
                RequestModernXamlDiagnosticsInitialization(
                    disconnectedFlavor);
            }
            return S_OK;
        }

        if (g_connectingXamlDiagnostics !=
            XamlDiagnosticsFlavor::None) {
            m_flavor = g_connectingXamlDiagnostics;
        }

        // Balance the reference added when XAML Diagnostics loads this module.
        if (HMODULE module = GetCurrentModuleHandle()) {
            FreeLibrary(module);
        }

        {
            std::lock_guard<std::mutex> guard(
                g_visualTreeWatchersMutex);
            if (g_stoppingModernUi.load(std::memory_order_acquire) ||
                !g_visualTreeWatchers) {
                return S_OK;
            }

            auto watcher =
                winrt::make_self<VisualTreeWatcher>(m_site);
            g_visualTreeWatchers->push_back(watcher);
            g_visualTreeWatcherGeneration.fetch_add(
                1, std::memory_order_release);
            m_watcher = std::move(watcher);
        }
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
    XamlDiagnosticsFlavor m_flavor = XamlDiagnosticsFlavor::None;
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
std::atomic_bool g_windowsUiXamlDiagnosticsConnected{false};
std::atomic_bool g_microsoftUiXamlDiagnosticsConnected{false};
std::atomic_bool g_windowsUiXamlDiagnosticsAttempted{false};
std::atomic_bool g_microsoftUiXamlDiagnosticsAttempted{false};
std::atomic_bool g_xamlDiagnosticsWorkerRunning{false};
std::atomic_bool g_xamlDiagnosticsRequestPending{false};
thread_local bool g_loadingXamlDiagnostics;
thread_local XamlDiagnosticsFlavor g_connectingXamlDiagnostics =
    XamlDiagnosticsFlavor::None;

HRESULT InjectXamlDiagnostics(HMODULE xamlModule,
                              LPCWSTR connectionPrefix,
                              XamlDiagnosticsFlavor flavor) {
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
        if (g_stoppingModernUi.load(std::memory_order_acquire)) {
            return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        }

        wchar_t connectionName[256];
        _snwprintf_s(connectionName, _TRUNCATE, L"%s%d",
                     connectionPrefix, index);
        const XamlDiagnosticsFlavor previousFlavor =
            g_connectingXamlDiagnostics;
        g_connectingXamlDiagnostics = flavor;
        result = initialize(
            connectionName, GetCurrentProcessId(), L"",
            modulePath, CLSID_CjkSpacerTap, nullptr);
        g_connectingXamlDiagnostics = previousFlavor;
        if (result != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    return result;
}

void EnsureModernXamlDiagnostics() {
    if (g_stoppingModernUi.load(std::memory_order_acquire) ||
        g_loadingXamlDiagnostics) {
        return;
    }

    ScopedBoolValue loadingGuard(g_loadingXamlDiagnostics, true);

    std::lock_guard<std::mutex> guard(
        g_xamlDiagnosticsInitializationMutex);
    if (g_stoppingModernUi.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_windowsUiXamlDiagnosticsConnected.load(
            std::memory_order_acquire) &&
        !g_windowsUiXamlDiagnosticsAttempted.load(
            std::memory_order_acquire)) {
        if (HMODULE module =
                GetModuleHandleW(L"Windows.UI.Xaml.dll")) {
            g_windowsUiXamlDiagnosticsAttempted.store(
                true, std::memory_order_release);
            const uint64_t watcherGeneration =
                g_visualTreeWatcherGeneration.load(
                    std::memory_order_acquire);
            const HRESULT result = InjectXamlDiagnostics(
                module, L"VisualDiagConnection",
                XamlDiagnosticsFlavor::Windows);
            if (SUCCEEDED(result) &&
                !g_stoppingModernUi.load(std::memory_order_acquire) &&
                g_visualTreeWatcherGeneration.load(
                    std::memory_order_acquire) >
                    watcherGeneration) {
                g_windowsUiXamlDiagnosticsConnected.store(
                    true, std::memory_order_release);
                Wh_Log(L"Connected to Windows.UI.Xaml diagnostics");
            } else if (SUCCEEDED(result)) {
                Wh_Log(L"Windows.UI.Xaml diagnostics returned success "
                       L"without creating a watcher; another diagnostics "
                       L"tool probably blocked the connection");
            } else if (result !=
                           HRESULT_FROM_WIN32(ERROR_NOT_FOUND) &&
                       result !=
                           HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
                Wh_Log(L"Windows.UI.Xaml diagnostics failed: "
                       L"0x%08X", result);
            }
        }
    }

    if (!g_microsoftUiXamlDiagnosticsConnected.load(
            std::memory_order_acquire) &&
        !g_microsoftUiXamlDiagnosticsAttempted.load(
            std::memory_order_acquire)) {
        if (HMODULE module = GetModuleHandleW(
                L"Microsoft.Internal.FrameworkUdk.dll")) {
            g_microsoftUiXamlDiagnosticsAttempted.store(
                true, std::memory_order_release);
            const uint64_t watcherGeneration =
                g_visualTreeWatcherGeneration.load(
                    std::memory_order_acquire);
            const HRESULT result = InjectXamlDiagnostics(
                module, L"WinUIVisualDiagConnection",
                XamlDiagnosticsFlavor::Microsoft);
            if (SUCCEEDED(result) &&
                !g_stoppingModernUi.load(std::memory_order_acquire) &&
                g_visualTreeWatcherGeneration.load(
                    std::memory_order_acquire) >
                    watcherGeneration) {
                g_microsoftUiXamlDiagnosticsConnected.store(
                    true, std::memory_order_release);
                Wh_Log(L"Connected to Microsoft.UI.Xaml diagnostics");
            } else if (SUCCEEDED(result)) {
                Wh_Log(L"Microsoft.UI.Xaml diagnostics returned success "
                       L"without creating a watcher; another diagnostics "
                       L"tool probably blocked the connection");
            } else if (result !=
                           HRESULT_FROM_WIN32(ERROR_NOT_FOUND) &&
                       result !=
                           HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
                Wh_Log(L"Microsoft.UI.Xaml diagnostics failed: "
                       L"0x%08X", result);
            }
        }
    }
}

bool NeedsXamlDiagnosticsConnection() {
    return (!g_windowsUiXamlDiagnosticsConnected.load(
                 std::memory_order_acquire) &&
            !g_windowsUiXamlDiagnosticsAttempted.load(
                std::memory_order_acquire) &&
            GetModuleHandleW(L"Windows.UI.Xaml.dll")) ||
           (!g_microsoftUiXamlDiagnosticsConnected.load(
                 std::memory_order_acquire) &&
            !g_microsoftUiXamlDiagnosticsAttempted.load(
                std::memory_order_acquire) &&
            GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll"));
}

void RequestModernXamlDiagnosticsInitialization(
    XamlDiagnosticsFlavor retryFlavor) {
    if (g_stoppingModernUi.load(std::memory_order_acquire)) {
        return;
    }

    // A new XAML module load or a previously established connection being
    // torn down is an explicit retry opportunity.
    if (retryFlavor == XamlDiagnosticsFlavor::Windows) {
        g_windowsUiXamlDiagnosticsAttempted.store(
            false, std::memory_order_release);
    } else if (retryFlavor == XamlDiagnosticsFlavor::Microsoft) {
        g_microsoftUiXamlDiagnosticsAttempted.store(
            false, std::memory_order_release);
    }

    // Once all currently loaded XAML diagnostics connections are established,
    // avoid creating another worker until SetSite(nullptr) explicitly marks
    // one as disconnected.
    if (!NeedsXamlDiagnosticsConnection()) {
        return;
    }

    g_xamlDiagnosticsRequestPending.store(
        true, std::memory_order_release);

    bool expected = false;
    if (!g_xamlDiagnosticsWorkerRunning.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel)) {
        return;
    }

    // Do not hold a mutex across these calls. The request can originate from
    // LoadLibraryExW, whose caller may still own the Windows loader lock.
    // The worker isn't joined during unload, so keep the mod image loaded
    // until the worker exits through FreeLibraryAndExitThread.
    HMODULE module = AcquireCurrentModuleReference();
    if (!module) {
        if (!g_stoppingModernUi.load(std::memory_order_acquire)) {
            g_xamlDiagnosticsWorkerRunning.store(
                false, std::memory_order_release);
            Wh_Log(L"Couldn't retain the mod for XAML diagnostics");
        }
        return;
    }

    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID parameter) -> DWORD {
            const HMODULE module = static_cast<HMODULE>(parameter);
            const HRESULT initializeResult =
                CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            while (!g_stoppingModernUi.load(std::memory_order_acquire) &&
                   g_xamlDiagnosticsRequestPending.exchange(
                       false, std::memory_order_acq_rel)) {
                EnsureModernXamlDiagnostics();
            }
            if (SUCCEEDED(initializeResult)) {
                CoUninitialize();
            }

            if (!g_stoppingModernUi.load(std::memory_order_acquire)) {
                g_xamlDiagnosticsWorkerRunning.store(
                    false, std::memory_order_release);
                if (g_xamlDiagnosticsRequestPending.load(
                        std::memory_order_acquire)) {
                    RequestModernXamlDiagnosticsInitialization();
                }
            }
            FreeLibraryAndExitThread(module, 0);
        },
        module, 0, nullptr);
    if (!thread) {
        const DWORD error = GetLastError();
        if (!g_stoppingModernUi.load(std::memory_order_acquire)) {
            g_xamlDiagnosticsWorkerRunning.store(
                false, std::memory_order_release);
            Wh_Log(L"Couldn't create the XAML diagnostics thread: %u",
                   error);
        }
        FreeLibrary(module);
        return;
    }

    CloseHandle(thread);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t g_originalLoadLibraryExW;

PCWSTR GetModuleFileNamePart(LPCWSTR path) {
    if (!path) {
        return nullptr;
    }

    PCWSTR fileName = wcsrchr(path, L'\\');
    return fileName ? fileName + 1 : path;
}

XamlDiagnosticsFlavor GetXamlDiagnosticsTriggerFlavor(LPCWSTR path) {
    const PCWSTR fileName = GetModuleFileNamePart(path);
    if (!fileName) {
        return XamlDiagnosticsFlavor::None;
    }

    if (_wcsicmp(fileName, L"Windows.UI.Xaml.dll") == 0) {
        return XamlDiagnosticsFlavor::Windows;
    }
    if (_wcsicmp(fileName,
                 L"Microsoft.Internal.FrameworkUdk.dll") == 0) {
        return XamlDiagnosticsFlavor::Microsoft;
    }
    return XamlDiagnosticsFlavor::None;
}

bool IsXamlDiagnosticsTriggerModule(LPCWSTR path) {
    const PCWSTR fileName = GetModuleFileNamePart(path);
    return GetXamlDiagnosticsTriggerFlavor(path) !=
               XamlDiagnosticsFlavor::None ||
           (fileName && _wcsicmp(fileName, L"CoreMessagingXP.dll") == 0);
}

bool IsXamlDiagnosticsTriggerModuleLoaded(LPCWSTR path) {
    const PCWSTR fileName = GetModuleFileNamePart(path);
    if (!fileName) {
        return false;
    }

    return GetModuleHandleW(fileName) != nullptr;
}

HMODULE WINAPI LoadLibraryExWHook(LPCWSTR fileName,
                                  HANDLE file,
                                  DWORD flags) {
    const bool triggerModule =
        IsXamlDiagnosticsTriggerModule(fileName);
    const XamlDiagnosticsFlavor triggerFlavor =
        GetXamlDiagnosticsTriggerFlavor(fileName);
    const bool triggerModuleWasLoaded =
        triggerModule &&
        IsXamlDiagnosticsTriggerModuleLoaded(fileName);
    HMODULE module =
        g_originalLoadLibraryExW(fileName, file, flags);
    if (module &&
        g_modernUiText.load(std::memory_order_relaxed) &&
        triggerModule) {
        // Don't load the TAP or activate COM while the caller might hold the
        // Windows loader lock.
        RequestModernXamlDiagnosticsInitialization(
            !triggerModuleWasLoaded
                ? triggerFlavor
                : XamlDiagnosticsFlavor::None);
    }
    return module;
}

bool IsModernXamlHostClassName(LPCWSTR className) {
    return className &&
           (_wcsicmp(
                className,
                L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
            _wcsicmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
            _wcsicmp(
                className, L"XamlExplorerHostIslandWindow_WASDK") == 0);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc_t procedure,
                         PVOID parameter) {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParameter {
        RunFromWindowThreadProc_t procedure;
        PVOID parameter;
    };

    const DWORD threadId =
        GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        procedure(parameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* call =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (call->message == message) {
                    auto* run = reinterpret_cast<RunParameter*>(
                        call->lParam);
                    run->procedure(run->parameter);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunParameter run{procedure, parameter};
    SendMessageW(window, message, 0,
                 reinterpret_cast<LPARAM>(&run));
    UnhookWindowsHookEx(hook);
    return true;
}

HWND FindWindowForThread(DWORD threadId) {
    struct FindWindowParameter {
        HWND first = nullptr;
        HWND xamlHost = nullptr;
    } parameter;

    EnumThreadWindows(
        threadId,
        [](HWND window, LPARAM parameter) -> BOOL {
            auto* state = reinterpret_cast<FindWindowParameter*>(parameter);
            if (!state->first) {
                state->first = window;
            }

            wchar_t className[128];
            if (GetClassNameW(
                    window, className, ARRAYSIZE(className)) > 0 &&
                IsModernXamlHostClassName(className)) {
                state->xamlHost = window;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&parameter));
    return parameter.xamlHost ? parameter.xamlHost : parameter.first;
}

void InitializeModernUi() {
    g_stoppingModernUi.store(true, std::memory_order_release);
    g_windowsUiXamlDiagnosticsConnected.store(
        false, std::memory_order_release);
    g_microsoftUiXamlDiagnosticsConnected.store(
        false, std::memory_order_release);
    g_windowsUiXamlDiagnosticsAttempted.store(
        false, std::memory_order_release);
    g_microsoftUiXamlDiagnosticsAttempted.store(
        false, std::memory_order_release);
    g_xamlDiagnosticsWorkerRunning.store(
        false, std::memory_order_release);
    g_xamlDiagnosticsRequestPending.store(
        false, std::memory_order_release);
    g_visualTreeWatcherGeneration.store(
        0, std::memory_order_release);
    {
        std::lock_guard<std::mutex> guard(
            g_visualTreeWatchersMutex);
        if (!g_visualTreeWatchers) {
            g_visualTreeWatchers.emplace();
        }
    }
    {
        std::lock_guard<std::mutex> guard(
            g_modernTextStateThreadsMutex);
        g_modernTextStateThreads.clear();
    }
    g_stoppingModernUi.store(false, std::memory_order_release);
}

void UninitializeModernUi() {
    g_stoppingModernUi.store(true, std::memory_order_release);

    std::vector<winrt::com_ptr<VisualTreeWatcher>> watchers;
    {
        std::lock_guard<std::mutex> guard(
            g_visualTreeWatchersMutex);
        if (g_visualTreeWatchers) {
            g_visualTreeWatchers->swap(watchers);
            g_visualTreeWatchers.reset();
        }
    }
    for (const auto& watcher : watchers) {
        watcher->UnadviseVisualTreeChange();
    }

    std::vector<std::pair<DWORD, uint64_t>> stateThreads;
    {
        std::lock_guard<std::mutex> guard(
            g_modernTextStateThreadsMutex);
        PruneModernTextStateThreadsLocked();
        stateThreads.assign(g_modernTextStateThreads.begin(),
                            g_modernTextStateThreads.end());
    }

    for (const auto& [threadId, creationTime] : stateThreads) {
        if (!IsRegisteredThreadAlive(threadId, creationTime)) {
            continue;
        }

        // Prefer an Explorer/taskbar XAML host window. If the host was
        // replaced, the helper still falls back to another window on the same
        // UI thread so cleanup can run on the owning thread.
        const HWND window = FindWindowForThread(threadId);
        if (!window ||
            !IsRegisteredThreadAlive(threadId, creationTime) ||
            !RunFromWindowThread(
                window,
                [](PVOID) {
                    CleanupModernTextStatesForCurrentThread();
                },
                nullptr)) {
            // Don't call Restore from the wrong thread. The state contains
            // weak references only and is released when its UI thread exits.
            Wh_Log(L"Leaving modern XAML state for unreachable thread %u",
                   threadId);
        }
    }

}

template <typename Function>
bool InstallHook(Function target,
                 Function hook,
                 Function* original,
                 const wchar_t* name) {
    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Couldn't hook %s", name);
        return false;
    }

    return true;
}

bool HookClassicMenus() {
    return InstallHook(
               CreatePopupMenu, CreatePopupMenuHook,
               &g_originalCreatePopupMenu, L"CreatePopupMenu") &&
           InstallHook(
               DestroyMenu, DestroyMenuHook,
               &g_originalDestroyMenu, L"DestroyMenu") &&
           InstallHook(
               InsertMenuW, InsertMenuWHook,
               &g_originalInsertMenuW, L"InsertMenuW") &&
           InstallHook(
               AppendMenuW, AppendMenuWHook,
               &g_originalAppendMenuW, L"AppendMenuW") &&
           InstallHook(
               ModifyMenuW, ModifyMenuWHook,
               &g_originalModifyMenuW, L"ModifyMenuW") &&
           InstallHook(
               InsertMenuItemW, InsertMenuItemWHook,
               &g_originalInsertMenuItemW, L"InsertMenuItemW") &&
           InstallHook(
               SetMenuItemInfoW, SetMenuItemInfoWHook,
               &g_originalSetMenuItemInfoW, L"SetMenuItemInfoW") &&
           InstallHook(
               TrackPopupMenu, TrackPopupMenuHook,
               &g_originalTrackPopupMenu, L"TrackPopupMenu") &&
           InstallHook(
               TrackPopupMenuEx, TrackPopupMenuExHook,
               &g_originalTrackPopupMenuEx, L"TrackPopupMenuEx");
}

bool HookClassicTooltipThemeDrawing() {
    HMODULE themeModule = GetModuleHandleW(L"uxtheme.dll");
    if (!themeModule) {
        Wh_Log(L"Couldn't find linked uxtheme.dll");
        return false;
    }

    g_getWindowTheme = reinterpret_cast<GetWindowTheme_t>(
        GetProcAddress(themeModule, "GetWindowTheme"));
    const auto openThemeData =
        reinterpret_cast<OpenThemeData_t>(
            GetProcAddress(themeModule, "OpenThemeData"));
    const auto openThemeDataEx =
        reinterpret_cast<OpenThemeDataEx_t>(
            GetProcAddress(themeModule, "OpenThemeDataEx"));
    const auto openThemeDataForDpi =
        reinterpret_cast<OpenThemeDataForDpi_t>(
            GetProcAddress(themeModule, "OpenThemeDataForDpi"));
    const auto closeThemeData =
        reinterpret_cast<CloseThemeData_t>(
            GetProcAddress(themeModule, "CloseThemeData"));
    const auto getThemeTextExtent =
        reinterpret_cast<GetThemeTextExtent_t>(
            GetProcAddress(themeModule, "GetThemeTextExtent"));
    const auto drawThemeText =
        reinterpret_cast<DrawThemeText_t>(
            GetProcAddress(themeModule, "DrawThemeText"));
    const auto drawThemeTextEx =
        reinterpret_cast<DrawThemeTextEx_t>(
            GetProcAddress(themeModule, "DrawThemeTextEx"));

    if (!g_getWindowTheme || !openThemeData || !openThemeDataEx ||
        !openThemeDataForDpi || !closeThemeData ||
        !getThemeTextExtent || !drawThemeText || !drawThemeTextEx) {
        Wh_Log(L"Couldn't resolve all required uxtheme functions");
        return false;
    }

    return InstallHook(
               openThemeData, OpenThemeDataHook,
               &g_originalOpenThemeData, L"OpenThemeData") &&
           InstallHook(
               openThemeDataEx, OpenThemeDataExHook,
               &g_originalOpenThemeDataEx, L"OpenThemeDataEx") &&
           InstallHook(
               openThemeDataForDpi, OpenThemeDataForDpiHook,
               &g_originalOpenThemeDataForDpi,
               L"OpenThemeDataForDpi") &&
           InstallHook(
               closeThemeData, CloseThemeDataHook,
               &g_originalCloseThemeData, L"CloseThemeData") &&
           InstallHook(
               getThemeTextExtent, GetThemeTextExtentHook,
               &g_originalGetThemeTextExtent,
               L"GetThemeTextExtent") &&
           InstallHook(
               drawThemeText, DrawThemeTextHook,
               &g_originalDrawThemeText, L"DrawThemeText") &&
           InstallHook(
               drawThemeTextEx, DrawThemeTextExHook,
               &g_originalDrawThemeTextEx, L"DrawThemeTextEx");
}

bool ReadUnicodeLettersAndDigitsSetting() {
    auto characterMode =
        WindhawkUtils::StringSetting::make(L"characterMode");
    return _wcsicmp(characterMode, L"ascii") != 0;
}

void LoadSettings() {
    g_classicMenus.store(Wh_GetIntSetting(L"classicMenus") != 0,
                         std::memory_order_relaxed);
    g_classicTooltips.store(
        Wh_GetIntSetting(L"classicTooltips") != 0,
        std::memory_order_relaxed);
    g_modernUiText.store(Wh_GetIntSetting(L"modernUiText") != 0,
                         std::memory_order_relaxed);

    const bool unicodeMode = ReadUnicodeLettersAndDigitsSetting();
    g_unicodeLettersAndDigits.store(unicodeMode,
                                    std::memory_order_relaxed);
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
        Wh_Log(L"No enabled UI targets");
        return FALSE;
    }

    if (classicMenusEnabled && !HookClassicMenus()) {
        return FALSE;
    }

    if (classicTooltipsEnabled &&
        !HookClassicTooltipThemeDrawing()) {
        return FALSE;
    }

    if (modernUiTextEnabled) {
        HMODULE kernelBaseModule =
            GetModuleHandleW(L"kernelbase.dll");
        const auto loadLibraryExW =
            kernelBaseModule
                ? reinterpret_cast<LoadLibraryExW_t>(
                      GetProcAddress(kernelBaseModule,
                                     "LoadLibraryExW"))
                : nullptr;
        if (loadLibraryExW) {
            if (!InstallHook(
                    loadLibraryExW, LoadLibraryExWHook,
                    &g_originalLoadLibraryExW,
                    L"kernelbase!LoadLibraryExW")) {
                return FALSE;
            }
        } else {
            Wh_Log(L"Couldn't find kernelbase!LoadLibraryExW");
            return FALSE;
        }
        InitializeModernUi();
    }

    Wh_Log(L"Initialized (classicMenus=%d, "
           L"classicTooltips=%d, modern=%d, unicode=%d)",
           g_classicMenus.load(), g_classicTooltips.load(),
           g_modernUiText.load(),
           g_unicodeLettersAndDigits.load());
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_classicTooltips.load(std::memory_order_relaxed)) {
        DiscoverExistingClassicTooltipThemes();
    }

    if (g_modernUiText.load(std::memory_order_relaxed)) {
        RequestModernXamlDiagnosticsInitialization();
    }
}

void Wh_ModBeforeUninit() {
    if (g_modernUiText.load(std::memory_order_relaxed)) {
        // Stop new COM callbacks before Windhawk begins removing function
        // hooks. UninitializeModernUi performs the owning-thread restoration.
        g_stoppingModernUi.store(true, std::memory_order_release);
    }
}

void Wh_ModUninit() {
    RestoreRewrittenMenuItems();
    ClearCreatedClassicPopupMenus();
    CloseDiscoveredClassicTooltipThemes();
    ClearTrackedClassicTooltipThemes();
    if (g_modernUiText.load(std::memory_order_relaxed)) {
        UninitializeModernUi();
    }
    Wh_Log(L"Uninitialized");
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    const bool hooksChanged =
        g_classicMenus.load(std::memory_order_relaxed) !=
            (Wh_GetIntSetting(L"classicMenus") != 0) ||
        g_classicTooltips.load(std::memory_order_relaxed) !=
            (Wh_GetIntSetting(L"classicTooltips") != 0) ||
        g_modernUiText.load(std::memory_order_relaxed) !=
            (Wh_GetIntSetting(L"modernUiText") != 0);
    g_unicodeLettersAndDigits.store(
        ReadUnicodeLettersAndDigitsSetting(),
        std::memory_order_relaxed);
    *reload = hooksChanged ? TRUE : FALSE;
    return TRUE;
}
