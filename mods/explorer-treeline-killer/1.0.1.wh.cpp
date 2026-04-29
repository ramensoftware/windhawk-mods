// ==WindhawkMod==
// @id              explorer-treeline-killer
// @name            Explorer TreeLine Killer
// @description     Hide navigation pane separator lines and control upper/lower spacing in Explorer
// @version         1.0.1
// @author          Languster
// @github          https://github.com/Languster
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer TreeLine Killer

**Explorer TreeLine Killer** is a Windhawk mod for Windows Explorer that hides the built-in separator lines in the left navigation pane and lets you switch the upper and lower spacing between standard and compact behavior.

It is useful if you want the navigation pane to look cleaner, tighter, and more visually consistent.

## Features
- hide the built-in separator lines used by Explorer
- remove the upper empty space between the Home/Gallery block and the pinned items list
- remove the lower empty space between the pinned items list and **This PC**
- choose between tighter compact spacing and more standard Windows-like spacing for each section

## Recommended setup
For the best Explorer navigation pane experience, it is recommended to use this mod together with [Explorer TreeItem Tweaker](https://windhawk.net/mods/explorer-treeitem-tweaker) and [Explorer Navigation Tree Offset](https://windhawk.net/mods/explorer-navigation-pane-tweaks).

![Before / After](https://raw.githubusercontent.com/Languster/windhawk-mods/main/mods_assets/explorer-treeline-killer-before-after.jpg)

## Compatibility
Tested and confirmed compatible with **Windows 11 versions 23H2 through 25H2**.
Other versions have not been tested.

## What the options do
- **Fully hide system separator lines**
  Hides the default separator line used by Explorer in the lower special container.

- **Remove upper empty space**
  Removes the gap between the Home/Gallery block (when those entries are visible) and the pinned items list.
  Enabled = compact mode. Disabled = standard spacing.

- **Remove lower empty space**
  Removes the gap between the pinned items list and **This PC**.
  Enabled = compact mode. Disabled = standard spacing.

## Notes
- the upper option matters only when the Home/Gallery block is visible
- the lower option controls the spacing before **This PC**
- this mod is designed only for Explorer's left navigation pane

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideSystemSeparatorLines: true
  $name: Fully hide system separator lines
  $description: Hides the default separator line used by the lower special container.
- removeUpperGap: true
  $name: Remove upper empty space
  $description: Removes the gap between the Home/Gallery block (when visible) and the pinned items list. Enabled = compact mode, disabled = standard spacing.
- removeLowerGap: true
  $name: Remove lower empty space
  $description: Removes the gap between the pinned items list and This PC. Enabled = compact mode, disabled = standard spacing.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_utils.h>

#include <string>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#ifndef TVIF_INTEGRAL
#define TVIF_INTEGRAL 0x0080
#endif

struct Settings {
    int normalItemsHeightPx;
    int blockUnderGalleryRows;
    int thisComputerRows;
    bool hideSystemSeparatorLines;
    bool removeUpperGap;
    bool removeLowerGap;
    int separatorMaskThickness;
    bool debugLogging;
    bool logPaint;
    bool logParentPaint;
    bool logCustomDraw;
    bool logVisibleItems;
    bool logTallItemProbe;
    bool showGapOverlay;
    bool showTallDeadZoneOverlay;
    int maxLoggedVisibleItems;
    int hitTestStep;
};

static Settings g_settings = {};
static DWORD g_currentPid = 0;
static std::unordered_set<HWND> g_seenExplorerWindows;
static std::unordered_set<HWND> g_seenTreeWindows;
static std::unordered_set<HWND> g_attachedTreeWindows;
static std::unordered_set<HWND> g_attachedParentWindows;
static std::unordered_map<unsigned long long, int> g_appliedItemRows;
static thread_local int g_normalizeDepth = 0;

using CreateWindowExW_t = decltype(&CreateWindowExW);
using CreateWindowExA_t = decltype(&CreateWindowExA);

static CreateWindowExW_t CreateWindowExW_Original = nullptr;
static CreateWindowExA_t CreateWindowExA_Original = nullptr;

struct VisibleTreeItemInfo {
    HTREEITEM item;
    std::wstring text;
    RECT rc;
    int depth;
};

struct TreeRoleState {
    unsigned long long upperKey = 0;
    unsigned long long lowerKey = 0;
};

static std::unordered_map<HWND, TreeRoleState> g_treeRoleStates;

static int ClampRows(int rows) {
    if (rows < 1) return 1;
    if (rows > 4) return 4;
    return rows;
}

static int ClampNormalItemsHeightPx(int px) {
    if (px <= 0) return 0;
    if (px < 24) return 24;
    if (px > 120) return 120;
    return px;
}

static int ClampMaskThickness(int value) {
    if (value < 1) return 1;
    if (value > 6) return 6;
    return value;
}

static void LoadSettings() {
    g_settings.normalItemsHeightPx = 0;
    g_settings.hideSystemSeparatorLines = Wh_GetIntSetting(L"hideSystemSeparatorLines") != 0;
    g_settings.removeUpperGap = Wh_GetIntSetting(L"removeUpperGap") != 0;
    g_settings.removeLowerGap = Wh_GetIntSetting(L"removeLowerGap") != 0;
    g_settings.blockUnderGalleryRows = g_settings.removeUpperGap ? 1 : 2;
    g_settings.thisComputerRows = g_settings.removeLowerGap ? 1 : 2;
    g_settings.separatorMaskThickness = 3;
    g_settings.debugLogging = false;

    g_settings.logPaint = false;
    g_settings.logParentPaint = false;
    g_settings.logCustomDraw = g_settings.debugLogging;
    g_settings.logVisibleItems = g_settings.debugLogging;
    g_settings.logTallItemProbe = g_settings.debugLogging;
    g_settings.showGapOverlay = false;
    g_settings.showTallDeadZoneOverlay = false;
    g_settings.maxLoggedVisibleItems = 80;
    g_settings.hitTestStep = 4;
}

static std::wstring GetWindowClassString(HWND hwnd) {
    wchar_t buf[256] = {};
    GetClassNameW(hwnd, buf, ARRAYSIZE(buf));
    return buf;
}

static std::wstring GetWindowTextString(HWND hwnd) {
    wchar_t buf[512] = {};
    GetWindowTextW(hwnd, buf, ARRAYSIZE(buf));
    return buf;
}

static std::wstring RectToString(const RECT& rc) {
    wchar_t buf[128];
    wsprintfW(buf, L"(%ld,%ld)-(%ld,%ld) %ldx%ld",
              rc.left, rc.top, rc.right, rc.bottom,
              rc.right - rc.left, rc.bottom - rc.top);
    return buf;
}

static std::wstring GetWindowPath(HWND hwnd) {
    std::vector<std::wstring> parts;
    HWND current = hwnd;
    while (current) {
        std::wstring cls = GetWindowClassString(current);
        if (cls.empty()) {
            cls = L"?";
        }
        parts.push_back(cls);
        current = GetParent(current);
    }

    std::wstring result;
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        if (!result.empty()) {
            result += L" -> ";
        }
        result += *it;
    }
    return result;
}

static std::wstring DecodeTreeStyle(DWORD style) {
    std::wstring s;
    auto add = [&](const wchar_t* name) {
        if (!s.empty()) s += L"|";
        s += name;
    };

    if (style & TVS_HASBUTTONS) add(L"HASBUTTONS");
    if (style & TVS_HASLINES) add(L"HASLINES");
    if (style & TVS_LINESATROOT) add(L"LINESATROOT");
    if (style & TVS_SHOWSELALWAYS) add(L"SHOWSELALWAYS");
    if (style & TVS_TRACKSELECT) add(L"TRACKSELECT");
    if (style & TVS_FULLROWSELECT) add(L"FULLROWSELECT");
    if (style & TVS_INFOTIP) add(L"INFOTIP");
    if (style & TVS_NONEVENHEIGHT) add(L"NONEVENHEIGHT");
    if (style & TVS_NOTOOLTIPS) add(L"NOTOOLTIPS");
    if (style & TVS_NOHSCROLL) add(L"NOHSCROLL");
    if (style & TVS_SINGLEEXPAND) add(L"SINGLEEXPAND");
    if (style & TVS_CHECKBOXES) add(L"CHECKBOXES");

    if (s.empty()) {
        s = L"(none)";
    }

    return s;
}

static const wchar_t* NotifyCodeToString(UINT code) {
    if (code == (UINT)NM_CUSTOMDRAW) return L"NM_CUSTOMDRAW";
    if (code == (UINT)NM_CLICK) return L"NM_CLICK";
    if (code == (UINT)NM_DBLCLK) return L"NM_DBLCLK";
    if (code == (UINT)NM_RCLICK) return L"NM_RCLICK";
    if (code == (UINT)TVN_SELCHANGINGW) return L"TVN_SELCHANGINGW";
    if (code == (UINT)TVN_SELCHANGEDW) return L"TVN_SELCHANGEDW";
    if (code == (UINT)TVN_ITEMEXPANDINGW) return L"TVN_ITEMEXPANDINGW";
    if (code == (UINT)TVN_ITEMEXPANDEDW) return L"TVN_ITEMEXPANDEDW";
    if (code == (UINT)TVN_GETINFOTIPW) return L"TVN_GETINFOTIPW";
    if (code == (UINT)TVN_BEGINDRAGW) return L"TVN_BEGINDRAGW";
    if (code == (UINT)TVN_BEGINLABELEDITW) return L"TVN_BEGINLABELEDITW";
    if (code == (UINT)TVN_ENDLABELEDITW) return L"TVN_ENDLABELEDITW";
    if (code == (UINT)TVN_DELETEITEMW) return L"TVN_DELETEITEMW";
    if (code == (UINT)TVN_ITEMCHANGEDW) return L"TVN_ITEMCHANGEDW";
    if (code == (UINT)TVN_ITEMCHANGINGW) return L"TVN_ITEMCHANGINGW";
    return L"(other)";
}

static std::wstring DrawStageToString(DWORD stage) {
    std::wstring s;
    auto add = [&](const wchar_t* name) {
        if (!s.empty()) s += L"|";
        s += name;
    };

    DWORD baseStage = (stage & 0xFFFF);

    if (baseStage == CDDS_PREPAINT) add(L"PREPAINT");
    if (baseStage == CDDS_POSTPAINT) add(L"POSTPAINT");
    if (baseStage == CDDS_PREERASE) add(L"PREERASE");
    if (baseStage == CDDS_POSTERASE) add(L"POSTERASE");
    if (stage & CDDS_ITEM) add(L"ITEM");
    if (stage & CDDS_SUBITEM) add(L"SUBITEM");

    if (stage == CDDS_ITEMPREPAINT) add(L"ITEMPREPAINT");
    if (stage == CDDS_ITEMPOSTPAINT) add(L"ITEMPOSTPAINT");
    if (stage == CDDS_ITEMPREERASE) add(L"ITEMPREERASE");
    if (stage == CDDS_ITEMPOSTERASE) add(L"ITEMPOSTERASE");

    if (s.empty()) {
        wchar_t buf[64];
        wsprintfW(buf, L"0x%08lX", stage);
        s = buf;
    }

    return s;
}

static std::wstring GetTreeItemText(HWND tree, HTREEITEM item) {
    wchar_t text[512] = {};
    TVITEMW tvi = {};
    tvi.mask = TVIF_TEXT | TVIF_STATE | TVIF_CHILDREN | TVIF_HANDLE;
    tvi.hItem = item;
    tvi.pszText = text;
    tvi.cchTextMax = ARRAYSIZE(text);

    if (!TreeView_GetItem(tree, &tvi)) {
        return L"<failed>";
    }

    return text;
}

static int GetTreeItemDepth(HWND tree, HTREEITEM item) {
    int depth = 0;
    HTREEITEM parent = TreeView_GetParent(tree, item);
    while (parent) {
        depth++;
        parent = TreeView_GetParent(tree, parent);
    }
    return depth;
}

static bool TryGetTreeItemRect(HWND tree, HTREEITEM item, RECT* rc) {
    RECT temp = {};
    temp.left = 1;
    if (!TreeView_GetItemRect(tree, item, &temp, FALSE)) {
        return false;
    }

    RECT client = {};
    GetClientRect(tree, &client);

    if (temp.bottom <= temp.top || temp.bottom < 0 || temp.top > client.bottom + 1000) {
        return false;
    }

    *rc = temp;
    return true;
}

static std::vector<VisibleTreeItemInfo> GetVisibleTreeItems(HWND tree, int maxItems) {
    std::vector<VisibleTreeItemInfo> items;
    HTREEITEM item = TreeView_GetFirstVisible(tree);

    while (item && (int)items.size() < maxItems) {
        RECT rc = {};
        if (TryGetTreeItemRect(tree, item, &rc)) {
            VisibleTreeItemInfo info = {};
            info.item = item;
            info.text = GetTreeItemText(tree, item);
            info.rc = rc;
            info.depth = GetTreeItemDepth(tree, item);
            items.push_back(info);
        }

        item = TreeView_GetNextVisible(tree, item);
    }

    return items;
}


static int GetVisibleBaseItemHeight(const std::vector<VisibleTreeItemInfo>& items) {
    int minHeight = 0;
    for (const auto& info : items) {
        int h = info.rc.bottom - info.rc.top;
        if (h <= 0) {
            continue;
        }
        if (minHeight == 0 || h < minHeight) {
            minHeight = h;
        }
    }
    return minHeight;
}

static bool TryGetTreeItemIntegral(HWND tree, HTREEITEM item, int* integral) {
    TVITEMEXW tvi = {};
    tvi.mask = TVIF_HANDLE | TVIF_INTEGRAL;
    tvi.hItem = item;

    if (!SendMessageW(tree, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
        return false;
    }

    *integral = tvi.iIntegral;
    return true;
}

static bool SetTreeItemIntegral(HWND tree, HTREEITEM item, int integral) {
    TVITEMEXW tvi = {};
    tvi.mask = TVIF_HANDLE | TVIF_INTEGRAL;
    tvi.hItem = item;
    tvi.iIntegral = integral;

    return SendMessageW(tree, TVM_SETITEMW, 0, (LPARAM)&tvi) != 0;
}

static unsigned long long MakeTreeItemKey(HWND tree, HTREEITEM item) {
    return ((unsigned long long)(ULONG_PTR)tree << 32) ^ (unsigned long long)(ULONG_PTR)item;
}

static size_t CountDepth1Items(const std::vector<VisibleTreeItemInfo>& items) {
    size_t count = 0;
    for (const auto& info : items) {
        if (info.depth == 1) {
            count++;
        }
    }
    return count;
}

static int GetDepth1OrdinalAtIndex(const std::vector<VisibleTreeItemInfo>& items, size_t index) {
    if (index >= items.size() || items[index].depth != 1) {
        return -1;
    }

    int ordinal = 0;
    for (size_t i = 0; i < index; i++) {
        if (items[i].depth == 1) {
            ordinal++;
        }
    }
    return ordinal;
}

static bool IsProbablyTallSpecialItem(const VisibleTreeItemInfo& info, int baseHeight);

static int FindVisibleIndexByKey(HWND tree, const std::vector<VisibleTreeItemInfo>& items, unsigned long long key) {
    if (!key) {
        return -1;
    }

    for (size_t i = 0; i < items.size(); i++) {
        if (MakeTreeItemKey(tree, items[i].item) == key) {
            return (int)i;
        }
    }
    return -1;
}

static void RefreshTreeRoleState(HWND tree, const std::vector<VisibleTreeItemInfo>& items, int baseHeight) {
    auto& state = g_treeRoleStates[tree];

    std::vector<size_t> depth1Indices;
    std::vector<size_t> tallDepth1Indices;
    depth1Indices.reserve(items.size());
    tallDepth1Indices.reserve(items.size());

    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].depth != 1) {
            continue;
        }

        depth1Indices.push_back(i);

        if (IsProbablyTallSpecialItem(items[i], baseHeight)) {
            tallDepth1Indices.push_back(i);
        }
    }

    const int lastDepth1Ordinal = depth1Indices.empty() ? -1 : (int)depth1Indices.size() - 1;

    if (tallDepth1Indices.size() >= 2) {
        state.upperKey = MakeTreeItemKey(tree, items[tallDepth1Indices[0]].item);
        state.lowerKey = MakeTreeItemKey(tree, items[tallDepth1Indices[1]].item);
        return;
    }

    if (tallDepth1Indices.size() == 1) {
        size_t tallIndex = tallDepth1Indices[0];
        unsigned long long key = MakeTreeItemKey(tree, items[tallIndex].item);
        int depth1Ordinal = GetDepth1OrdinalAtIndex(items, tallIndex);

        if (depth1Ordinal == lastDepth1Ordinal) {
            state.lowerKey = key;
            if (state.upperKey == key) {
                state.upperKey = 0;
            }
        } else {
            state.upperKey = key;
            if (state.lowerKey == key) {
                state.lowerKey = 0;
            }
        }
    }

    int upperVisibleIndex = FindVisibleIndexByKey(tree, items, state.upperKey);
    if (upperVisibleIndex < 0 ||
        items[(size_t)upperVisibleIndex].depth != 1 ||
        GetDepth1OrdinalAtIndex(items, (size_t)upperVisibleIndex) == 0) {
        state.upperKey = 0;
    }

    int lowerVisibleIndex = FindVisibleIndexByKey(tree, items, state.lowerKey);
    if (lowerVisibleIndex < 0 ||
        items[(size_t)lowerVisibleIndex].depth != 1 ||
        GetDepth1OrdinalAtIndex(items, (size_t)lowerVisibleIndex) != lastDepth1Ordinal) {
        state.lowerKey = 0;
    }
}


static bool TryGetTreeItemIntegral(HWND tree, HTREEITEM item, int* integral);
static bool IsProbablyTallSpecialItem(const VisibleTreeItemInfo& info, int baseHeight);
static void AttachTreeHooks(HWND tree);
static void LogTreeSnapshot(HWND tree);
static void ApplyGlobalItemHeight(HWND tree);


static int GetTallDepth1OrdinalAtIndex(const std::vector<VisibleTreeItemInfo>& items, size_t index, int baseHeight) {
    if (index >= items.size()) {
        return -1;
    }

    if (items[index].depth != 1 || !IsProbablyTallSpecialItem(items[index], baseHeight)) {
        return -1;
    }

    int ordinal = 0;
    for (size_t i = 0; i < index; i++) {
        if (items[i].depth == 1 && IsProbablyTallSpecialItem(items[i], baseHeight)) {
            ordinal++;
        }
    }

    return ordinal;
}

enum class SpecialStripeRole {
    None,
    UpperSpecial,
    LowerSpecial,
};

static SpecialStripeRole GetSpecialStripeRoleForItem(
    HWND tree,
    HTREEITEM item,
    VisibleTreeItemInfo* outInfo = nullptr,
    int* outBaseHeight = nullptr
) {
    if (!item || !IsWindow(tree)) {
        return SpecialStripeRole::None;
    }

    auto items = GetVisibleTreeItems(tree, 256);
    if (items.empty()) {
        return SpecialStripeRole::None;
    }

    int baseHeight = GetVisibleBaseItemHeight(items);
    RefreshTreeRoleState(tree, items, baseHeight);

    auto stateIt = g_treeRoleStates.find(tree);
    if (stateIt == g_treeRoleStates.end()) {
        return SpecialStripeRole::None;
    }

    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].item != item) {
            continue;
        }

        if (outInfo) {
            *outInfo = items[i];
        }
        if (outBaseHeight) {
            *outBaseHeight = baseHeight;
        }

        unsigned long long key = MakeTreeItemKey(tree, item);
        if (stateIt->second.upperKey && key == stateIt->second.upperKey) {
            return SpecialStripeRole::UpperSpecial;
        }
        if (stateIt->second.lowerKey && key == stateIt->second.lowerKey) {
            return SpecialStripeRole::LowerSpecial;
        }

        return SpecialStripeRole::None;
    }

    return SpecialStripeRole::None;
}

static bool ShouldHideSeparatorForRoleInCompactMode(SpecialStripeRole role) {
    switch (role) {
        case SpecialStripeRole::UpperSpecial:
            return false;
        case SpecialStripeRole::LowerSpecial:
            return g_settings.thisComputerRows <= 1;
        default:
            return false;
    }
}

static COLORREF SampleTreeColor(HDC hdc, int x, int y, int maxX, int maxY);

static void PaintSeparatorMaskForSpecialItem(HWND tree, HDC hdc, const RECT& itemRect, int thickness) {
    if (!hdc || !IsWindow(tree)) {
        return;
    }

    RECT client = {};
    GetClientRect(tree, &client);
    if (client.right <= 0 || client.bottom <= 0) {
        return;
    }

    if (thickness <= 0) {
        thickness = 1;
    }

    int top = itemRect.top;
    int bottom = top + thickness;
    if (top < 0) {
        bottom += -top;
        top = 0;
    }
    if (bottom > client.bottom) {
        bottom = client.bottom;
    }
    if (bottom <= top) {
        return;
    }

    COLORREF color = CLR_INVALID;
    int sampleX = client.right > 8 ? client.right - 8 : client.right - 1;
    int sampleY = bottom + 6;
    for (int dy = 0; dy <= 8 && color == CLR_INVALID; dy++) {
        color = SampleTreeColor(hdc, sampleX, sampleY + dy, client.right, client.bottom);
    }
    if (color == CLR_INVALID) {
        color = SampleTreeColor(hdc, client.right / 2, bottom + 6, client.right, client.bottom);
    }
    if (color == CLR_INVALID) {
        color = RGB(32, 32, 32);
    }

    RECT rc = { client.left, top, client.right, bottom };
    HBRUSH brush = CreateSolidBrush(color);
    if (brush) {
        FillRect(hdc, &rc, brush);
        DeleteObject(brush);
    }
}

static void DrawCompactSeparatorMasksToHdc(HWND tree, HDC hdc) {
    if (!g_settings.hideSystemSeparatorLines || !IsWindow(tree) || !hdc) {
        return;
    }

    auto items = GetVisibleTreeItems(tree, 256);
    if (items.empty()) {
        return;
    }

    int compactThickness = std::max(5, g_settings.separatorMaskThickness + 2);
    int baseHeight = GetVisibleBaseItemHeight(items);

    for (size_t i = 0; i < items.size(); i++) {
        const auto& info = items[i];
        if (info.depth != 1) {
            continue;
        }

        int ordinal = GetTallDepth1OrdinalAtIndex(items, i, baseHeight);
        SpecialStripeRole role = SpecialStripeRole::None;
        if (ordinal == 0) {
            role = SpecialStripeRole::UpperSpecial;
        } else if (ordinal == 1) {
            role = SpecialStripeRole::LowerSpecial;
        }

        if (!ShouldHideSeparatorForRoleInCompactMode(role)) {
            continue;
        }

        PaintSeparatorMaskForSpecialItem(tree, hdc, info.rc, compactThickness);
    }
}

static int GetEffectiveBaseHeight(HWND tree, const std::vector<VisibleTreeItemInfo>& items) {
    int visibleBase = GetVisibleBaseItemHeight(items);
    if (g_settings.normalItemsHeightPx > 0) {
        return g_settings.normalItemsHeightPx;
    }
    if (visibleBase > 0) {
        return visibleBase;
    }
    int globalItemHeight = (int)SendMessageW(tree, TVM_GETITEMHEIGHT, 0, 0);
    return globalItemHeight > 0 ? globalItemHeight : 48;
}

static int GetSeparatorMaskYForItem(const VisibleTreeItemInfo& info, int baseHeight) {
    int height = info.rc.bottom - info.rc.top;
    if (height >= baseHeight + baseHeight / 2) {
        return info.rc.top + baseHeight / 2;
    }
    return info.rc.top + 1;
}

static COLORREF SampleTreeColor(HDC hdc, int x, int y, int maxX, int maxY) {
    if (maxX <= 0 || maxY <= 0) {
        return CLR_INVALID;
    }
    if (x < 0) x = 0;
    if (x >= maxX) x = maxX - 1;
    if (y < 0) y = 0;
    if (y >= maxY) y = maxY - 1;
    return GetPixel(hdc, x, y);
}

static void PaintSeparatorMaskLine(HDC hdc, const RECT& client, int y, int thickness) {
    if (client.right <= 0 || client.bottom <= 0) {
        return;
    }

    int left = client.right >= 32 ? 16 : 0;
    int right = client.right >= 32 ? client.right - 16 : client.right;
    if (right <= left) {
        left = 0;
        right = client.right;
    }

    int top = y - thickness / 2;
    int bottom = top + thickness;
    if (top < 0) {
        bottom += -top;
        top = 0;
    }
    if (bottom > client.bottom) {
        bottom = client.bottom;
    }
    if (bottom <= top) {
        return;
    }

    COLORREF color = CLR_INVALID;
    int sampleX = client.right > 8 ? client.right - 8 : client.right - 1;
    for (int dy = -3; dy <= 3 && color == CLR_INVALID; dy++) {
        color = SampleTreeColor(hdc, sampleX, y + dy, client.right, client.bottom);
    }
    if (color == CLR_INVALID) {
        color = SampleTreeColor(hdc, client.right / 2, top > 0 ? top - 1 : bottom, client.right, client.bottom);
    }
    if (color == CLR_INVALID) {
        color = RGB(32, 32, 32);
    }

    RECT rc = { left, top, right, bottom };
    HBRUSH brush = CreateSolidBrush(color);
    if (brush) {
        FillRect(hdc, &rc, brush);
        DeleteObject(brush);
    }
}

static void DrawSystemSeparatorMasks(HWND tree) {
    if (!g_settings.hideSystemSeparatorLines || !IsWindowVisible(tree)) {
        return;
    }

    HDC hdc = GetDC(tree);
    if (!hdc) {
        return;
    }

    DrawCompactSeparatorMasksToHdc(tree, hdc);
    ReleaseDC(tree, hdc);
}


static bool IsProbablyTallSpecialItem(const VisibleTreeItemInfo& info, int baseHeight);

static int GetDesiredRowsForVisibleItem(HWND tree, const std::vector<VisibleTreeItemInfo>& items, size_t index, int baseHeight) {
    if (index >= items.size() || !IsWindow(tree)) {
        return 0;
    }

    const auto& info = items[index];
    if (info.depth != 1) {
        return 0;
    }

    auto it = g_treeRoleStates.find(tree);
    if (it == g_treeRoleStates.end()) {
        return 0;
    }

    unsigned long long key = MakeTreeItemKey(tree, info.item);
    if (it->second.upperKey && key == it->second.upperKey) {
        return ClampRows(g_settings.blockUnderGalleryRows);
    }
    if (it->second.lowerKey && key == it->second.lowerKey) {
        return ClampRows(g_settings.thisComputerRows);
    }

    return 0;
}

static bool ApplyRowsToItemEarly(HWND tree, HTREEITEM item, int desiredRows, const wchar_t* reason) {
    if (g_normalizeDepth > 0 || !item || desiredRows <= 0) {
        return false;
    }

    int oldRows = 0;
    if (!TryGetTreeItemIntegral(tree, item, &oldRows)) {
        return false;
    }

    if (oldRows == desiredRows) {
        g_appliedItemRows[MakeTreeItemKey(tree, item)] = desiredRows;
        return false;
    }

    g_normalizeDepth++;
    bool ok = SetTreeItemIntegral(tree, item, desiredRows);
    g_normalizeDepth--;

    if (ok) {
        g_appliedItemRows[MakeTreeItemKey(tree, item)] = desiredRows;
    }

    if (g_settings.debugLogging) {
        std::wstring text = GetTreeItemText(tree, item);
        Wh_Log(L"[TREE APPLY HEIGHT] hwnd=%p item=%p text=\"%s\" reason=%s oldRows=%d newRows=%d ok=%d",
               tree,
               item,
               text.c_str(),
               reason ? reason : L"(unknown)",
               oldRows,
               desiredRows,
               ok ? 1 : 0);
    }

    return ok;
}

static void ApplyGlobalItemHeight(HWND tree) {
    if (!IsWindow(tree) || g_settings.normalItemsHeightPx <= 0) {
        return;
    }

    int current = (int)SendMessageW(tree, TVM_GETITEMHEIGHT, 0, 0);
    if (current == g_settings.normalItemsHeightPx) {
        return;
    }

    SendMessageW(tree, TVM_SETITEMHEIGHT, (WPARAM)g_settings.normalItemsHeightPx, 0);

    if (g_settings.debugLogging) {
        Wh_Log(L"[TREE APPLY NORMAL HEIGHT] hwnd=%p oldPx=%d newPx=%d",
               tree,
               current,
               g_settings.normalItemsHeightPx);
    }
}

static void ApplyDesiredRowsToVisibleItems(HWND tree, const wchar_t* reason) {
    if (g_normalizeDepth > 0 || !IsWindow(tree)) {
        return;
    }

    auto items = GetVisibleTreeItems(tree, 256);
    int baseHeight = GetVisibleBaseItemHeight(items);
    RefreshTreeRoleState(tree, items, baseHeight);

    for (size_t i = 0; i < items.size(); i++) {
        int desiredRows = GetDesiredRowsForVisibleItem(tree, items, i, baseHeight);
        if (desiredRows <= 0) {
            continue;
        }

        ApplyRowsToItemEarly(tree, items[i].item, desiredRows, reason);
    }
}

static bool IsProbablyTallSpecialItem(const VisibleTreeItemInfo& info, int baseHeight) {
    int height = info.rc.bottom - info.rc.top;
    if (baseHeight <= 0 || height <= 0) {
        return false;
    }

    return height >= baseHeight + baseHeight / 2;
}

static std::wstring BuildTallItemHitRunString(HWND tree, const VisibleTreeItemInfo& info, int x, int step) {
    if (step <= 0) {
        step = 4;
    }

    std::wstring result;
    bool hasRun = false;
    bool runHit = false;
    int runStart = info.rc.top;

    for (int y = info.rc.top; y < info.rc.bottom; y += step) {
        TVHITTESTINFO hti = {};
        hti.pt.x = x;
        hti.pt.y = y;
        HTREEITEM hit = TreeView_HitTest(tree, &hti);
        bool isHit = (hit == info.item);

        if (!hasRun) {
            hasRun = true;
            runHit = isHit;
            runStart = y;
            continue;
        }

        if (isHit != runHit) {
            wchar_t buf[96];
            wsprintfW(buf, L"%s:%d-%d", runHit ? L"HIT" : L"MISS", runStart, y);
            if (!result.empty()) {
                result += L"|";
            }
            result += buf;
            runHit = isHit;
            runStart = y;
        }
    }

    if (hasRun) {
        wchar_t buf[96];
        wsprintfW(buf, L"%s:%d-%d", runHit ? L"HIT" : L"MISS", runStart, info.rc.bottom);
        if (!result.empty()) {
            result += L"|";
        }
        result += buf;
    }

    if (result.empty()) {
        result = L"(none)";
    }

    return result;
}

static void AnalyzeTallItems(HWND tree, const std::vector<VisibleTreeItemInfo>& items) {
    if (!g_settings.logTallItemProbe) {
        return;
    }

    RECT client = {};
    GetClientRect(tree, &client);

    int baseHeight = GetVisibleBaseItemHeight(items);
    int globalItemHeight = (int)SendMessageW(tree, TVM_GETITEMHEIGHT, 0, 0);
    int hitX = client.right / 2;

    Wh_Log(L"[TREE METRICS] hwnd=%p visibleBaseHeight=%d globalItemHeight=%d hitTestX=%d hitTestStep=%d",
           tree,
           baseHeight,
           globalItemHeight,
           hitX,
           g_settings.hitTestStep);

    for (const auto& info : items) {
        if (!IsProbablyTallSpecialItem(info, baseHeight)) {
            continue;
        }

        int height = info.rc.bottom - info.rc.top;
        int extra = height - baseHeight;
        int rows = 0;
        bool gotIntegral = TryGetTreeItemIntegral(tree, info.item, &rows);

        Wh_Log(L"[TREE TALL ITEM] hwnd=%p item=%p text=\"%s\" rect=%s height=%d baseHeight=%d extra=%d rows=%s%d",
               tree,
               info.item,
               info.text.c_str(),
               RectToString(info.rc).c_str(),
               height,
               baseHeight,
               extra,
               gotIntegral ? L"" : L"?",
               gotIntegral ? rows : 0);

        if (g_settings.logTallItemProbe) {
            std::wstring runs = BuildTallItemHitRunString(tree, info, hitX, g_settings.hitTestStep);
            Wh_Log(L"[TREE TALL HITTEST] hwnd=%p item=%p text=\"%s\" x=%d runs=%s",
                   tree,
                   info.item,
                   info.text.c_str(),
                   hitX,
                   runs.c_str());
        }

    }
}

static void LogVisibleItemsAndGaps(HWND tree) {
    RECT client = {};
    GetClientRect(tree, &client);

    auto items = GetVisibleTreeItems(tree, g_settings.maxLoggedVisibleItems);

    Wh_Log(L"[TREE VISIBLE] hwnd=%p count=%llu client=%s",
           tree,
           (unsigned long long)items.size(),
           RectToString(client).c_str());

    if (g_settings.logVisibleItems) {
        for (const auto& info : items) {
            Wh_Log(L"[TREE VISIBLE ITEM] hwnd=%p depth=%d item=%p text=\"%s\" rect=%s",
                   tree,
                   info.depth,
                   info.item,
                   info.text.c_str(),
                   RectToString(info.rc).c_str());
        }
    }

    for (size_t i = 0; i + 1 < items.size(); i++) {
        const auto& a = items[i];
        const auto& b = items[i + 1];
        int gap = b.rc.top - a.rc.bottom;
        if (gap > 0) {
            RECT gapRc = {};
            gapRc.left = 0;
            gapRc.top = a.rc.bottom;
            gapRc.right = client.right;
            gapRc.bottom = b.rc.top;

            Wh_Log(L"[TREE GAP] hwnd=%p between=\"%s\" -> \"%s\" gap=%d rect=%s",
                   tree,
                   a.text.c_str(),
                   b.text.c_str(),
                   gap,
                   RectToString(gapRc).c_str());
        }
    }
}

static void LogTreeSnapshot(HWND tree) {
    if (!IsWindow(tree)) {
        return;
    }

    LONG_PTR style = GetWindowLongPtrW(tree, GWL_STYLE);
    int indent = (int)SendMessageW(tree, TVM_GETINDENT, 0, 0);
    int globalItemHeight = (int)SendMessageW(tree, TVM_GETITEMHEIGHT, 0, 0);
    RECT rc = {};
    GetWindowRect(tree, &rc);

    Wh_Log(L"[TREE SNAPSHOT] hwnd=%p class=%s style=0x%p treeStyle=%s indent=%d globalItemHeight=%d rect=%s path=%s",
           tree,
           GetWindowClassString(tree).c_str(),
           (void*)style,
           DecodeTreeStyle((DWORD)style).c_str(),
           indent,
           globalItemHeight,
           RectToString(rc).c_str(),
           GetWindowPath(tree).c_str());

    auto items = GetVisibleTreeItems(tree, g_settings.maxLoggedVisibleItems);
    LogVisibleItemsAndGaps(tree);
    AnalyzeTallItems(tree, items);
}

static void DumpWindowTreeRecursive(HWND hwnd, int depth, int maxDepth) {
    if (!IsWindow(hwnd) || depth > maxDepth) {
        return;
    }

    RECT rc = {};
    GetWindowRect(hwnd, &rc);
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    int id = GetDlgCtrlID(hwnd);

    std::wstring indent(depth * 2, L' ');
    std::wstring cls = GetWindowClassString(hwnd);
    std::wstring text = GetWindowTextString(hwnd);

    Wh_Log(L"[HWND TREE] %shwnd=%p class=%s id=%d style=0x%p ex=0x%p rect=%s text=\"%s\"",
           indent.c_str(),
           hwnd,
           cls.c_str(),
           id,
           (void*)style,
           (void*)exStyle,
           RectToString(rc).c_str(),
           text.c_str());

    for (HWND child = GetWindow(hwnd, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        DumpWindowTreeRecursive(child, depth + 1, maxDepth);
    }
}

static void DrawDebugOverlays(HWND tree) {
    if ((!g_settings.showGapOverlay && !g_settings.showTallDeadZoneOverlay) || !IsWindowVisible(tree)) {
        return;
    }

    RECT client = {};
    GetClientRect(tree, &client);

    auto items = GetVisibleTreeItems(tree, 256);
    if (items.empty()) {
        return;
    }

    HDC hdc = GetDC(tree);
    if (!hdc) {
        return;
    }

    HBRUSH gapBrush = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH missBrush = CreateSolidBrush(RGB(255, 0, 0));

    if (g_settings.showGapOverlay && gapBrush && items.size() >= 2) {
        for (size_t i = 0; i + 1 < items.size(); i++) {
            const auto& a = items[i];
            const auto& b = items[i + 1];
            int gap = b.rc.top - a.rc.bottom;
            if (gap <= 0) {
                continue;
            }

            RECT gapRc = {};
            gapRc.left = 0;
            gapRc.top = a.rc.bottom;
            gapRc.right = client.right < 24 ? client.right : 24;
            gapRc.bottom = b.rc.top;

            FillRect(hdc, &gapRc, gapBrush);
        }
    }

    if (g_settings.showTallDeadZoneOverlay && missBrush) {
        int baseHeight = GetVisibleBaseItemHeight(items);
        int hitX = client.right / 2;

        for (const auto& info : items) {
            if (!IsProbablyTallSpecialItem(info, baseHeight)) {
                continue;
            }

            bool runActive = false;
            bool runHit = false;
            int runStart = info.rc.top;

            for (int y = info.rc.top; y < info.rc.bottom; y += g_settings.hitTestStep) {
                TVHITTESTINFO hti = {};
                hti.pt.x = hitX;
                hti.pt.y = y;
                HTREEITEM hit = TreeView_HitTest(tree, &hti);
                bool isHit = (hit == info.item);

                if (!runActive) {
                    runActive = true;
                    runHit = isHit;
                    runStart = y;
                    continue;
                }

                if (isHit != runHit) {
                    if (!runHit) {
                        RECT missRc = {};
                        missRc.left = 0;
                        missRc.top = runStart;
                        missRc.right = client.right < 24 ? client.right : 24;
                        missRc.bottom = y;
                        FillRect(hdc, &missRc, missBrush);
                    }
                    runHit = isHit;
                    runStart = y;
                }
            }

            if (runActive && !runHit) {
                RECT missRc = {};
                missRc.left = 0;
                missRc.top = runStart;
                missRc.right = client.right < 24 ? client.right : 24;
                missRc.bottom = info.rc.bottom;
                FillRect(hdc, &missRc, missBrush);
            }
        }
    }

    if (gapBrush) DeleteObject(gapBrush);
    if (missBrush) DeleteObject(missBrush);
    ReleaseDC(tree, hdc);
}

static bool HasAncestorClass(HWND hwnd, const wchar_t* targetClass, int maxLevels) {
    HWND current = hwnd;
    for (int level = 0; current && level < maxLevels; level++) {
        if (GetWindowClassString(current) == targetClass) {
            return true;
        }
        current = GetParent(current);
    }
    return false;
}

static bool IsExplorerNavigationTree(HWND hwnd) {
    if (!IsWindow(hwnd) || GetWindowClassString(hwnd) != L"SysTreeView32") {
        return false;
    }

    if (!HasAncestorClass(hwnd, L"NamespaceTreeControl", 6)) {
        return false;
    }

    if (!HasAncestorClass(hwnd, L"CabinetWClass", 12)) {
        return false;
    }

    return true;
}

static void MaybeAttachTreeFromCreateHook(HWND hwnd, const wchar_t* apiName) {
    if (!IsExplorerNavigationTree(hwnd)) {
        return;
    }

    bool firstTime = g_seenTreeWindows.insert(hwnd).second;
    if (firstTime) {
        AttachTreeHooks(hwnd);

        if (g_settings.debugLogging) {
            Wh_Log(L"[TREE CREATE HOOK] api=%s hwnd=%p parent=%p path=%s",
                   apiName ? apiName : L"(unknown)",
                   hwnd,
                   GetParent(hwnd),
                   GetWindowPath(hwnd).c_str());
            LogTreeSnapshot(hwnd);
        }
    }
}

static LRESULT CALLBACK ParentSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
);

static LRESULT CALLBACK TreeSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
) {
    bool applyVisibleAfter = false;
    bool applySingleAfter = false;
    HTREEITEM changedItem = nullptr;
    const wchar_t* applyReason = nullptr;

    switch (uMsg) {
        case WM_SHOWWINDOW:
            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_SHOWWINDOW show=%llu status=%lld",
                       hwnd, (unsigned long long)wParam, (long long)lParam);
            }
            break;

        case WM_WINDOWPOSCHANGED: {
            RECT rc = {};
            GetWindowRect(hwnd, &rc);
            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_WINDOWPOSCHANGED rect=%s",
                       hwnd, RectToString(rc).c_str());
            }
            break;
        }

        case WM_SIZE: {
            RECT rc = {};
            GetWindowRect(hwnd, &rc);
            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_SIZE rect=%s",
                       hwnd, RectToString(rc).c_str());
            }
            break;
        }

        case WM_STYLECHANGED: {
            LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_STYLECHANGED style=0x%p treeStyle=%s",
                       hwnd, (void*)style, DecodeTreeStyle((DWORD)style).c_str());
            }
            break;
        }

        case WM_THEMECHANGED:
            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_THEMECHANGED", hwnd);
            }
            break;

        case WM_PAINT:
            if (g_settings.logPaint) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_PAINT", hwnd);
            }
            break;

        case WM_PRINTCLIENT:
            if (g_settings.logPaint) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_PRINTCLIENT flags=0x%llX",
                       hwnd, (unsigned long long)lParam);
            }
            break;

        case TVM_SETINDENT:
            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE MSG] hwnd=%p TVM_SETINDENT newIndent=%llu",
                       hwnd, (unsigned long long)wParam);
            }
            break;

        case TVM_INSERTITEMW:
        case TVM_INSERTITEMA:
            applySingleAfter = true;
            applyReason = L"insert";
            break;

        case TVM_SETITEMW:
        case TVM_SETITEMA: {
            const TVITEMW* tvi = reinterpret_cast<const TVITEMW*>(lParam);
            if (tvi) {
                changedItem = tvi->hItem;
                if (tvi->mask & (TVIF_INTEGRAL | TVIF_TEXT | TVIF_STATE | TVIF_CHILDREN)) {
                    applySingleAfter = true;
                    applyReason = L"setitem";
                }
            }
            break;
        }

        case WM_NCDESTROY:
            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE MSG] hwnd=%p WM_NCDESTROY", hwnd);
            }
            g_seenTreeWindows.erase(hwnd);
            g_attachedTreeWindows.erase(hwnd);
            g_treeRoleStates.erase(hwnd);

            HWND parent = GetParent(hwnd);
            if (parent) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(parent, ParentSubclassProc);
                g_attachedParentWindows.erase(parent);
            }
            break;
    }

    LRESULT result = DefSubclassProc(hwnd, uMsg, wParam, lParam);

    if (applySingleAfter && g_normalizeDepth == 0) {
        HTREEITEM item = changedItem;
        if (!item && (uMsg == TVM_INSERTITEMW || uMsg == TVM_INSERTITEMA)) {
            item = (HTREEITEM)result;
        }
        if (item && GetTreeItemDepth(hwnd, item) <= 1) {
            ApplyDesiredRowsToVisibleItems(hwnd, applyReason);
        }
    }

    if (applyVisibleAfter && g_normalizeDepth == 0) {
        ApplyGlobalItemHeight(hwnd);
        ApplyDesiredRowsToVisibleItems(hwnd, applyReason);
    }

    if (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT) {
        if (g_settings.hideSystemSeparatorLines) {
            DrawSystemSeparatorMasks(hwnd);
        }
        if (uMsg == WM_PAINT && (g_settings.showGapOverlay || g_settings.showTallDeadZoneOverlay)) {
            DrawDebugOverlays(hwnd);
        }
    }

    return result;
}

static LRESULT CALLBACK ParentSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
) {
    HWND tree = (HWND)dwRefData;

    if (tree && !IsWindow(tree)) {
        tree = nullptr;
    }

    switch (uMsg) {
        case WM_NOTIFY: {
            LPNMHDR hdr = (LPNMHDR)lParam;
            if (tree && hdr && hdr->hwndFrom == tree) {
                if (hdr->code == (UINT)NM_CUSTOMDRAW) {
                    LPNMTVCUSTOMDRAW cd = (LPNMTVCUSTOMDRAW)lParam;
                    if (g_settings.logCustomDraw) {
                        Wh_Log(L"[PARENT NOTIFY] parent=%p tree=%p code=%s stage=%s itemSpec=0x%p rc=%s",
                               hwnd,
                               tree,
                               NotifyCodeToString(hdr->code),
                               DrawStageToString(cd->nmcd.dwDrawStage).c_str(),
                               (void*)cd->nmcd.dwItemSpec,
                               RectToString(cd->nmcd.rc).c_str());
                    }

                    if (g_settings.hideSystemSeparatorLines) {
                        DWORD stage = cd->nmcd.dwDrawStage;
                        if (stage == CDDS_PREPAINT) {
                            return CDRF_NOTIFYPOSTPAINT;
                        }
                        if (stage == CDDS_POSTPAINT) {
                            DrawCompactSeparatorMasksToHdc(tree, cd->nmcd.hdc);
                        }
                    }
                } else if (g_settings.debugLogging) {
                    Wh_Log(L"[PARENT NOTIFY] parent=%p tree=%p code=%s(%u)",
                           hwnd,
                           tree,
                           NotifyCodeToString(hdr->code),
                           hdr->code);
                }

                // Intentionally don't rescan/log-snapshot on expand/click here.
                // In v5 this could trigger extra normalization/redraw churn during child tree expansion.
            }
            break;
        }

        case WM_PAINT:
            if (g_settings.logParentPaint) {
                Wh_Log(L"[PARENT MSG] hwnd=%p tree=%p WM_PAINT class=%s",
                       hwnd, tree, GetWindowClassString(hwnd).c_str());
            }
            break;

        case WM_PRINTCLIENT:
            if (g_settings.logParentPaint) {
                Wh_Log(L"[PARENT MSG] hwnd=%p tree=%p WM_PRINTCLIENT flags=0x%llX class=%s",
                       hwnd,
                       tree,
                       (unsigned long long)lParam,
                       GetWindowClassString(hwnd).c_str());
            }
            break;

        case WM_NCDESTROY:
            g_attachedParentWindows.erase(hwnd);
            break;
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

static void AttachTreeHooks(HWND tree) {
    if (!IsWindow(tree)) {
        return;
    }

    if (!g_attachedTreeWindows.insert(tree).second) {
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(tree, TreeSubclassProc, 0)) {
        g_attachedTreeWindows.erase(tree);
        return;
    }

    ApplyGlobalItemHeight(tree);
    ApplyDesiredRowsToVisibleItems(tree, L"attach_once");

    // The tree sends NM_CUSTOMDRAW through WM_NOTIFY to its direct parent.
    // Subclassing higher ancestors isn't needed.
    HWND parent = GetParent(tree);
    if (parent && g_attachedParentWindows.insert(parent).second) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(parent, ParentSubclassProc, (DWORD_PTR)tree);
    }
}

static void FindAndAttachTreesRecursive(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }

    if (IsExplorerNavigationTree(hwnd)) {
        bool firstTime = g_seenTreeWindows.insert(hwnd).second;
        if (firstTime) {
            AttachTreeHooks(hwnd);

            if (g_settings.debugLogging) {
                Wh_Log(L"[TREE FOUND] hwnd=%p parent=%p path=%s",
                       hwnd, GetParent(hwnd), GetWindowPath(hwnd).c_str());
                LogTreeSnapshot(hwnd);
            }
        }
    }

    for (HWND child = GetWindow(hwnd, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        FindAndAttachTreesRecursive(child);
    }
}

static void CleanupHooksRecursive(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }

    if (IsExplorerNavigationTree(hwnd)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TreeSubclassProc);
        g_attachedTreeWindows.erase(hwnd);
        g_treeRoleStates.erase(hwnd);

        HWND parent = GetParent(hwnd);
        if (parent) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(parent, ParentSubclassProc);
            g_attachedParentWindows.erase(parent);
        }
    }

    for (HWND child = GetWindow(hwnd, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        CleanupHooksRecursive(child);
    }
}

struct EnumExplorerContext {
    std::vector<HWND> windows;
};

static BOOL CALLBACK EnumTopWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != g_currentPid) {
        return TRUE;
    }

    std::wstring cls = GetWindowClassString(hwnd);
    if (cls == L"CabinetWClass" || cls == L"ExploreWClass") {
        auto* ctx = (EnumExplorerContext*)lParam;
        ctx->windows.push_back(hwnd);
    }

    return TRUE;
}

static void ScanExplorerWindows() {
    EnumExplorerContext ctx;
    EnumWindows(EnumTopWindowsProc, (LPARAM)&ctx);

    for (HWND hwnd : ctx.windows) {
        bool firstTime = g_seenExplorerWindows.insert(hwnd).second;
        if (firstTime && g_settings.debugLogging) {
            RECT rc = {};
            GetWindowRect(hwnd, &rc);
            Wh_Log(L"[EXPLORER FOUND] hwnd=%p class=%s rect=%s text=\"%s\"",
                   hwnd,
                   GetWindowClassString(hwnd).c_str(),
                   RectToString(rc).c_str(),
                   GetWindowTextString(hwnd).c_str());

            DumpWindowTreeRecursive(hwnd, 0, 8);
        }

        FindAndAttachTreesRecursive(hwnd);
    }
}

static HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hwnd) {
        MaybeAttachTreeFromCreateHook(hwnd, L"CreateWindowExW");
    }

    return hwnd;
}

static HWND WINAPI CreateWindowExA_Hook(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExA_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hwnd) {
        MaybeAttachTreeFromCreateHook(hwnd, L"CreateWindowExA");
    }

    return hwnd;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"[MOD] settings changed: hideSystemSeparatorLines=%d removeUpperGap=%d removeLowerGap=%d upperRows=%d lowerRows=%d",
           g_settings.hideSystemSeparatorLines ? 1 : 0,
           g_settings.removeUpperGap ? 1 : 0,
           g_settings.removeLowerGap ? 1 : 0,
           g_settings.blockUnderGalleryRows,
           g_settings.thisComputerRows);
}

BOOL Wh_ModInit() {
    g_currentPid = GetCurrentProcessId();

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_TREEVIEW_CLASSES };
    InitCommonControlsEx(&icc);

    LoadSettings();

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"[MOD] failed to hook CreateWindowExW");
    }

    if (!Wh_SetFunctionHook((void*)CreateWindowExA, (void*)CreateWindowExA_Hook, (void**)&CreateWindowExA_Original)) {
        Wh_Log(L"[MOD] failed to hook CreateWindowExA");
    }

    Wh_Log(L"[MOD] init pid=%lu", g_currentPid);
    return TRUE;
}

void Wh_ModAfterInit() {
    // Attach to tree controls that already exist when the mod is enabled.
    // Newly created controls are handled by the CreateWindowExW/A hooks.
    ScanExplorerWindows();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[MOD] before uninit");

    EnumExplorerContext ctx;
    EnumWindows(EnumTopWindowsProc, (LPARAM)&ctx);
    for (HWND hwnd : ctx.windows) {
        CleanupHooksRecursive(hwnd);
    }
}
