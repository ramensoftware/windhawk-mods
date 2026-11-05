// ==WindhawkMod==
// @id              taskbar-folders
// @name            Taskbar Folders
// @description     Android-style app folders on the Windows 11 taskbar
// @description:fr-FR Dossiers d'applications façon Android dans la barre des tâches de Windows 11
// @version         0.1.0
// @author          Sylvain
// @github          https://github.com/sdeparte
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lpropsys -lshlwapi -lshcore -ld2d1 -ldwrite -ldwmapi -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Folders

App folders on the Windows 11 taskbar, Android style: one button groups several
shortcuts; hovering it opens a window that shows the apps and launches them.

The mod's menus and dialogs are in French when the Windows display language is
French, in English otherwise.

## Usage

- **Create a folder**: right-click an empty area of the taskbar › *New folder*;
  the app picker opens right away to fill it.
- **Add apps**: small **+** at the top right of the folder window, or right-click
  the folder button › *Add apps…*. Checked apps leave the taskbar and move into
  the folder.
- **Open**: hover the button. A click keeps the window open (arrow keys + Enter
  to launch, Esc to close).
- **Reorder**: drag an icon inside the folder window, or Ctrl + arrow keys.
- **Remove an app**: right-click the app in the folder window › *Remove from folder*.
- **Folder icon**: right-click the button › *Choose a Windows icon…* or
  *Choose an image…* (*Restore the mini grid* to go back to the first 4 apps).
- **Rename / delete**: right-click the folder button.
- The button can be moved like any other pinned app.

Settings: maximum columns (the grid balances itself according to the number of
apps), icon size, spacing, open and close delays, background effect (blur like
translucent-windows, Windows acrylic or opaque) and blur tint.

## Where the data lives

`%LOCALAPPDATA%\TaskbarFolders\<id>\`: `folder.ini` (name and order) and one
shortcut per app. The `_pins` subdirectory holds the shortcuts pinned by the mod.

## Disabling or uninstalling

Folder buttons stay pinned when the mod is disabled: they show a folder icon and
do nothing. To remove them for good, delete the folders while the mod is enabled,
or unpin them by hand (right-click › *Unpin from taskbar*), then delete
`%LOCALAPPDATA%\TaskbarFolders`.

## Limitations

- Apps can't be dropped into a folder by dragging them on the taskbar: Windows
  moves the folder away during the drag. Use the **+** or *Add apps…*.
- Pinned Microsoft Store apps don't appear in the picker.
- To put an app back on the taskbar: launch it from the folder, then right-click
  its taskbar icon › *Pin to taskbar*.
- Win+N on a folder does nothing.
- Windows 11 only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- openDelay: 300
  $name: Open delay (ms)
  $description: How long to hover the button before the window opens.
- closeDelay: 400
  $name: Close delay (ms)
  $description: How long before the window closes once the mouse has left the button and the window.
- columns: 4
  $name: Columns (maximum)
  $description: >-
    Maximum number of columns (1 to 8). The window adapts to the number of apps
    to stay balanced: 1 app = 1 column, 2 to 4 = 2 columns, 5 to 9 = 3…
- iconSize: 32
  $name: Icon size (px)
  $description: Size of the icons in the window (16 to 64).
- spacing: 12
  $name: Spacing (px)
  $description: Space between the icons in the window (0 to 48).
- effect: blur
  $name: Background effect
  $description: >-
    Blur: same effect as the translucent-windows mod (very transparent background).
    Acrylic: standard Windows acrylic effect. Opaque: solid background.
  $options:
  - blur: Blur (like translucent-windows)
  - acrylic: Windows acrylic
  - opaque: Opaque
- tintColor: "3A232323"
  $name: Blur tint
  $description: AARRGGBB color blended into the blur (same format as translucent-windows).
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>
#include <bcrypt.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <propkey.h>
#include <propvarutil.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <windowsx.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

#include <atomic>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <numeric>
#include <shared_mutex>
#include <unordered_map>

// ----- begin logic.h -----

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace tbf {

// -----------------------------------------------------------------------------
// Identifiants
// -----------------------------------------------------------------------------

inline constexpr std::wstring_view kPinNamePrefix = L"TaskbarFolder-";
inline constexpr std::wstring_view kAppIdPrefix = L"TaskbarFolders.";
inline constexpr size_t kFolderIdLength = 8;

inline bool IsValidFolderId(std::wstring_view id) {
    if (id.size() != kFolderIdLength) {
        return false;
    }
    return std::all_of(id.begin(), id.end(), [](wchar_t c) {
        return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f');
    });
}

inline std::wstring FormatFolderId(uint32_t value) {
    static constexpr wchar_t kHex[] = L"0123456789abcdef";
    std::wstring id(kFolderIdLength, L'0');
    for (size_t i = 0; i < kFolderIdLength; ++i) {
        id[kFolderIdLength - 1 - i] = kHex[(value >> (4 * i)) & 0xF];
    }
    return id;
}

inline std::wstring PinShortcutStem(std::wstring_view id) {
    return std::wstring(kPinNamePrefix) + std::wstring(id);
}

inline std::wstring FolderAppId(std::wstring_view id) {
    return std::wstring(kAppIdPrefix) + std::wstring(id);
}

// Cherche "TaskbarFolder-<id>" n'importe où dans un texte (nom d'automatisation
// d'un bouton de la barre, nom de fichier…).
inline std::optional<std::wstring> ExtractFolderIdFromText(std::wstring_view text) {
    size_t pos = 0;
    while ((pos = text.find(kPinNamePrefix, pos)) != std::wstring_view::npos) {
        const size_t start = pos + kPinNamePrefix.size();
        if (start + kFolderIdLength <= text.size()) {
            const std::wstring_view candidate = text.substr(start, kFolderIdLength);
            const size_t after = start + kFolderIdLength;
            const bool atBoundary = after == text.size() || !std::iswxdigit(text[after]);
            if (atBoundary && IsValidFolderId(candidate)) {
                return std::wstring(candidate);
            }
        }
        pos = start;
    }
    return std::nullopt;
}

inline std::optional<std::wstring> FolderIdFromAppId(std::wstring_view appId) {
    if (!appId.starts_with(kAppIdPrefix)) {
        return std::nullopt;
    }
    const std::wstring_view rest = appId.substr(kAppIdPrefix.size());
    if (!IsValidFolderId(rest)) {
        return std::nullopt;
    }
    return std::wstring(rest);
}

// -----------------------------------------------------------------------------
// Texte
// -----------------------------------------------------------------------------

inline wchar_t FoldCase(wchar_t c) {
    return static_cast<wchar_t>(std::towlower(c));
}

inline bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(),
                      [](wchar_t x, wchar_t y) { return FoldCase(x) == FoldCase(y); });
}

inline bool LessIgnoreCase(std::wstring_view a, std::wstring_view b) {
    return std::lexicographical_compare(
        a.begin(), a.end(), b.begin(), b.end(),
        [](wchar_t x, wchar_t y) { return FoldCase(x) < FoldCase(y); });
}

inline std::wstring Trim(std::wstring_view s) {
    size_t begin = 0;
    size_t end = s.size();
    while (begin < end && std::iswspace(s[begin])) {
        ++begin;
    }
    while (end > begin && std::iswspace(s[end - 1])) {
        --end;
    }
    return std::wstring(s.substr(begin, end - begin));
}

inline bool HasLnkExtension(std::wstring_view fileName) {
    constexpr std::wstring_view kExtension = L".lnk";
    return fileName.size() > kExtension.size() &&
           EqualsIgnoreCase(fileName.substr(fileName.size() - kExtension.size()), kExtension);
}

inline std::wstring DisplayNameFromFile(std::wstring_view fileName) {
    if (HasLnkExtension(fileName)) {
        return std::wstring(fileName.substr(0, fileName.size() - 4));
    }
    return std::wstring(fileName);
}

// -----------------------------------------------------------------------------
// Ordre des applis
// -----------------------------------------------------------------------------

// `block` : contenu brut de GetPrivateProfileSectionW, "clé=valeur\0…\0\0".
// Les entrées sans '=' sont ignorées.
inline std::vector<std::pair<std::wstring, std::wstring>> ParseIniSection(std::wstring_view block) {
    std::vector<std::pair<std::wstring, std::wstring>> entries;
    size_t pos = 0;
    while (pos < block.size()) {
        size_t end = block.find(L'\0', pos);
        if (end == std::wstring_view::npos) {
            end = block.size();
        }
        const std::wstring_view entry = block.substr(pos, end - pos);
        if (entry.empty()) {
            break;
        }
        const size_t eq = entry.find(L'=');
        if (eq != std::wstring_view::npos) {
            entries.emplace_back(Trim(entry.substr(0, eq)), Trim(entry.substr(eq + 1)));
        }
        pos = end + 1;
    }
    return entries;
}

// Garde les clés numériques ("1", "2"…) à valeur non vide, triées par numéro.
inline std::vector<std::wstring> OrderFromSection(
    const std::vector<std::pair<std::wstring, std::wstring>>& entries) {
    std::vector<std::pair<long, std::wstring>> numbered;
    for (const auto& entry : entries) {
        const std::wstring& key = entry.first;
        if (key.empty() || key.size() > 6 || entry.second.empty()) {
            continue;
        }
        if (!std::all_of(key.begin(), key.end(), [](wchar_t c) { return c >= L'0' && c <= L'9'; })) {
            continue;
        }
        numbered.emplace_back(std::stol(key), entry.second);
    }
    std::stable_sort(numbered.begin(), numbered.end(),
                     [](const auto& a, const auto& b) { return a.first < b.first; });
    std::vector<std::wstring> order;
    for (const auto& item : numbered) {
        order.push_back(item.second);
    }
    return order;
}

// Applis présentes dans `files`, d'abord dans l'ordre de `order` (comparaison
// insensible à la casse, entrées absentes ignorées), puis les autres par nom.
inline std::vector<std::wstring> OrderApps(const std::vector<std::wstring>& order,
                                           const std::vector<std::wstring>& files) {
    std::vector<bool> used(files.size(), false);
    std::vector<std::wstring> result;
    for (const auto& wanted : order) {
        for (size_t i = 0; i < files.size(); ++i) {
            if (!used[i] && EqualsIgnoreCase(files[i], wanted)) {
                used[i] = true;
                result.push_back(files[i]);
                break;
            }
        }
    }
    std::vector<std::wstring> rest;
    for (size_t i = 0; i < files.size(); ++i) {
        if (!used[i]) {
            rest.push_back(files[i]);
        }
    }
    std::sort(rest.begin(), rest.end(),
              [](const std::wstring& a, const std::wstring& b) { return LessIgnoreCase(a, b); });
    result.insert(result.end(), rest.begin(), rest.end());
    return result;
}

// -----------------------------------------------------------------------------
// Géométrie
// -----------------------------------------------------------------------------

struct Rect {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    int Width() const { return right - left; }
    int Height() const { return bottom - top; }
    bool Contains(int x, int y) const { return x >= left && x < right && y >= top && y < bottom; }
    bool operator==(const Rect&) const = default;
};

struct Size {
    int width = 0;
    int height = 0;
};

enum class Edge { Bottom, Top, Left, Right };

// Bord de l'écran contre lequel est collée la barre : le plus proche du bouton.
inline Edge DetectEdge(const Rect& button, const Rect& monitor) {
    const int toBottom = monitor.bottom - button.bottom;
    const int toTop = button.top - monitor.top;
    const int toLeft = button.left - monitor.left;
    const int toRight = monitor.right - button.right;
    const int nearest = std::min({toBottom, toTop, toLeft, toRight});
    if (nearest == toBottom) return Edge::Bottom;
    if (nearest == toTop) return Edge::Top;
    if (nearest == toLeft) return Edge::Left;
    return Edge::Right;
}

// Fenêtre centrée sur le bouton, décollée de `gap` du bord de la barre côté
// écran, puis ramenée dans la zone de travail. Utiliser le bord du bouton en
// plus de la zone de travail gère les barres masquées automatiquement.
inline Rect ComputePopupRect(const Rect& button, Size popup, Edge edge, const Rect& workArea, int gap) {
    // Jamais plus grande que la zone de travail : le contenu est rogné plutôt
    // que de déborder hors de l'écran.
    popup.width = std::min(popup.width, std::max(0, workArea.Width() - 2 * gap));
    popup.height = std::min(popup.height, std::max(0, workArea.Height() - 2 * gap));
    int x = button.left + (button.Width() - popup.width) / 2;
    int y = button.top + (button.Height() - popup.height) / 2;
    switch (edge) {
        case Edge::Bottom:
            y = std::min(workArea.bottom, button.top) - gap - popup.height;
            break;
        case Edge::Top:
            y = std::max(workArea.top, button.bottom) + gap;
            break;
        case Edge::Left:
            x = std::max(workArea.left, button.right) + gap;
            break;
        case Edge::Right:
            x = std::min(workArea.right, button.left) - gap - popup.width;
            break;
    }
    const int minX = workArea.left + gap;
    const int maxX = std::max(minX, workArea.right - gap - popup.width);
    const int minY = workArea.top + gap;
    const int maxY = std::max(minY, workArea.bottom - gap - popup.height);
    x = std::clamp(x, minX, maxX);
    y = std::clamp(y, minY, maxY);
    return {x, y, x + popup.width, y + popup.height};
}

inline int Scaled(double value, double scale) {
    return static_cast<int>(std::lround(value * scale));
}

struct GridMetrics {
    int padding = 0;
    int titleHeight = 0;
    int cellWidth = 0;
    int cellHeight = 0;
    int iconSize = 0;
    int columns = 1;
    int spacing = 0;  // marge entre deux cases voisines
};

inline GridMetrics MakeGridMetrics(int iconSize, int columns, double scale, int spacing = 0) {
    GridMetrics m;
    m.padding = Scaled(12, scale);
    m.titleHeight = Scaled(36, scale);
    m.cellWidth = Scaled(iconSize + 48, scale);
    m.cellHeight = Scaled(iconSize + 40, scale);
    m.iconSize = Scaled(iconSize, scale);
    m.columns = std::max(1, columns);
    m.spacing = Scaled(std::max(0, spacing), scale);
    return m;
}

struct GridLayout {
    Size window;
    Rect title;
    std::vector<Rect> cells;
};

// Largeur constante (`columns` cases) comme un dossier Android ; au moins une
// rangée pour afficher le message « Dossier vide ».
inline GridLayout ComputeGridLayout(size_t count, const GridMetrics& m) {
    GridLayout layout;
    const size_t columns = static_cast<size_t>(m.columns);
    const int rows = count == 0 ? 1 : static_cast<int>((count + columns - 1) / columns);
    layout.window = {m.padding * 2 + m.columns * m.cellWidth + (m.columns - 1) * m.spacing,
                     m.padding * 2 + m.titleHeight + rows * m.cellHeight + (rows - 1) * m.spacing};
    layout.title = {m.padding, m.padding, layout.window.width - m.padding, m.padding + m.titleHeight};
    for (size_t i = 0; i < count; ++i) {
        const int col = static_cast<int>(i % columns);
        const int row = static_cast<int>(i / columns);
        const int left = m.padding + col * (m.cellWidth + m.spacing);
        const int top = m.padding + m.titleHeight + row * (m.cellHeight + m.spacing);
        layout.cells.push_back({left, top, left + m.cellWidth, top + m.cellHeight});
    }
    return layout;
}

// Grille équilibrée, proche du carré : 1 → 1, 2 à 4 → 2, 5 à 9 → 3…, sans
// dépasser `maxColumns`. Dossier vide : pleine largeur pour le message d'aide.
inline int BalancedColumns(size_t count, int maxColumns) {
    maxColumns = std::max(1, maxColumns);
    if (count == 0) {
        return maxColumns;
    }
    int columns = 1;
    while (static_cast<size_t>(columns) * static_cast<size_t>(columns) < count && columns < maxColumns) {
        ++columns;
    }
    return columns;
}

// Colonnes à utiliser : `m.columns` (réglage) au minimum, plus si les rangées
// dépasseraient `maxHeight`, mais jamais au-delà de ce que permet `maxWidth`.
inline int FitColumns(size_t count, const GridMetrics& m, int maxWidth, int maxHeight) {
    // n cases occupent n * case + (n - 1) * espacement.
    const int widthLimit =
        std::max(1, (maxWidth - 2 * m.padding + m.spacing) / std::max(1, m.cellWidth + m.spacing));
    const int maxRows = std::max(
        1, (maxHeight - 2 * m.padding - m.titleHeight + m.spacing) / std::max(1, m.cellHeight + m.spacing));
    const int needed = static_cast<int>((count + static_cast<size_t>(maxRows) - 1) / static_cast<size_t>(maxRows));
    return std::max(1, std::min(std::max(m.columns, needed), widthLimit));
}

// Déplace l'élément `from` à la position `to` (ramenée dans la liste) ; une
// source hors limites laisse la liste intacte.
template <typename T>
inline std::vector<T> MoveItem(std::vector<T> items, size_t from, size_t to) {
    if (from >= items.size()) {
        return items;
    }
    to = std::min(to, items.size() - 1);
    T moved = std::move(items[from]);
    items.erase(items.begin() + static_cast<std::ptrdiff_t>(from));
    items.insert(items.begin() + static_cast<std::ptrdiff_t>(to), std::move(moved));
    return items;
}

// Case visée par un glisser dans la grille : colonne et rangée sous le curseur
// (la moitié d'un espacement compte pour la case voisine), ramenées dans la
// grille, puis limitées au nombre de cases. -1 si la grille est vide.
inline int DropIndexAt(const GridLayout& layout, const GridMetrics& m, int x, int y) {
    const int count = static_cast<int>(layout.cells.size());
    if (count == 0) {
        return -1;
    }
    const int columns = std::max(1, m.columns);
    const int rows = (count + columns - 1) / columns;
    const int firstLeft = layout.cells[0].left;
    const int firstTop = layout.cells[0].top;
    const auto slot = [](int offset, int step) {
        return offset < 0 ? -1 : offset / std::max(1, step);
    };
    const int col = std::clamp(slot(x - firstLeft + m.spacing / 2, m.cellWidth + m.spacing), 0, columns - 1);
    const int row = std::clamp(slot(y - firstTop + m.spacing / 2, m.cellHeight + m.spacing), 0, rows - 1);
    return std::min(row * columns + col, count - 1);
}

// Petit bouton carré (« + ») calé à droite de la ligne de titre.
inline Rect TitleButtonRect(const Rect& title, int size) {
    const int top = title.top + (title.Height() - size) / 2;
    return {title.right - size, top, title.right, top + size};
}

inline int HitTestCell(const GridLayout& layout, int x, int y) {
    for (size_t i = 0; i < layout.cells.size(); ++i) {
        if (layout.cells[i].Contains(x, y)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

enum class NavKey { Left, Right, Up, Down };

inline int MoveSelection(int current, size_t count, int columns, NavKey key) {
    if (count == 0) {
        return -1;
    }
    if (current < 0) {
        return 0;
    }
    const int last = static_cast<int>(count) - 1;
    switch (key) {
        case NavKey::Right:
            return std::min(current + 1, last);
        case NavKey::Left:
            return std::max(current - 1, 0);
        case NavKey::Down:
            return current + columns <= last ? current + columns : current;
        case NavKey::Up:
            return current - columns >= 0 ? current - columns : current;
    }
    return current;
}

// Cases 2x2 de l'icône du dossier, dans un carré de `size` px.
inline std::vector<Rect> MiniGridCells(size_t count, int size) {
    const int padding = size / 8;
    const int gap = std::max(1, size / 32);
    const int cell = (size - 2 * padding - gap) / 2;
    std::vector<Rect> cells;
    for (size_t i = 0; i < std::min<size_t>(count, 4); ++i) {
        const int left = padding + static_cast<int>(i % 2) * (cell + gap);
        const int top = padding + static_cast<int>(i / 2) * (cell + gap);
        cells.push_back({left, top, left + cell, top + cell});
    }
    return cells;
}

// -----------------------------------------------------------------------------
// Images BGRA (pixels 0xAARRGGBB, alpha prémultiplié, lignes de haut en bas)
// -----------------------------------------------------------------------------

struct Bgra {
    int width = 0;
    int height = 0;
    std::vector<uint32_t> pixels;

    static Bgra Blank(int w, int h) {
        Bgra image;
        image.width = w;
        image.height = h;
        image.pixels.assign(static_cast<size_t>(w) * static_cast<size_t>(h), 0);
        return image;
    }

    bool Empty() const {
        return width <= 0 || height <= 0 ||
               pixels.size() != static_cast<size_t>(width) * static_cast<size_t>(height);
    }

    uint32_t At(int x, int y) const { return pixels[static_cast<size_t>(y) * width + x]; }
};

inline constexpr uint32_t Premultiply(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    auto mul = [a](uint8_t c) { return static_cast<uint32_t>((c * a + 127) / 255); };
    return (static_cast<uint32_t>(a) << 24) | (mul(r) << 16) | (mul(g) << 8) | mul(b);
}

// Composition « source sur destination » en alpha prémultiplié.
inline uint32_t Over(uint32_t src, uint32_t dst) {
    const uint32_t inverse = 255 - (src >> 24);
    auto channel = [&](int shift) {
        const uint32_t s = (src >> shift) & 0xFF;
        const uint32_t d = (dst >> shift) & 0xFF;
        return std::min<uint32_t>(255, s + (d * inverse + 127) / 255);
    };
    return (channel(24) << 24) | (channel(16) << 16) | (channel(8) << 8) | channel(0);
}

// Certaines vieilles icônes 32 bits ont un alpha entièrement nul : sans
// correction elles seraient invisibles.
inline void FixMissingAlpha(Bgra& image) {
    if (image.Empty()) {
        return;
    }
    bool anyAlpha = false;
    bool anyColor = false;
    for (uint32_t pixel : image.pixels) {
        if (pixel >> 24) {
            anyAlpha = true;
            break;
        }
        if (pixel & 0x00FFFFFFu) {
            anyColor = true;
        }
    }
    if (anyAlpha || !anyColor) {
        return;
    }
    for (uint32_t& pixel : image.pixels) {
        pixel |= 0xFF000000u;
    }
}

inline void FillRoundedRect(Bgra& image, const Rect& r, int radius, uint32_t color) {
    radius = std::clamp(radius, 0, std::min(r.Width(), r.Height()) / 2);
    const double inner = static_cast<double>(radius);
    for (int y = std::max(0, r.top); y < std::min(image.height, r.bottom); ++y) {
        for (int x = std::max(0, r.left); x < std::min(image.width, r.right); ++x) {
            const double cx = x + 0.5;
            const double cy = y + 0.5;
            const double nx = std::clamp(cx, r.left + inner, r.right - inner);
            const double ny = std::clamp(cy, r.top + inner, r.bottom - inner);
            const double dx = cx - nx;
            const double dy = cy - ny;
            if (dx * dx + dy * dy > inner * inner) {
                continue;
            }
            uint32_t& pixel = image.pixels[static_cast<size_t>(y) * image.width + x];
            pixel = Over(color, pixel);
        }
    }
}

// Dessine `src` centrée dans `box`, rognée à `box` et à `dst`.
inline void BlitCentered(Bgra& dst, const Bgra& src, const Rect& box) {
    if (src.Empty() || dst.Empty()) {
        return;
    }
    const int originX = box.left + (box.Width() - src.width) / 2;
    const int originY = box.top + (box.Height() - src.height) / 2;
    for (int sy = 0; sy < src.height; ++sy) {
        for (int sx = 0; sx < src.width; ++sx) {
            const int x = originX + sx;
            const int y = originY + sy;
            if (!box.Contains(x, y) || x < 0 || y < 0 || x >= dst.width || y >= dst.height) {
                continue;
            }
            uint32_t& pixel = dst.pixels[static_cast<size_t>(y) * dst.width + x];
            pixel = Over(src.At(sx, sy), pixel);
        }
    }
}

// Gris clair translucide : lisible sur une barre claire comme sombre.
inline constexpr uint32_t kMiniGridBackground = Premultiply(0x66, 0xC8, 0xC8, 0xC8);

inline Bgra ComposeMiniGrid(const std::vector<Bgra>& icons, int size, uint32_t background) {
    Bgra out = Bgra::Blank(size, size);
    FillRoundedRect(out, {0, 0, size, size}, size / 4, background);
    const auto cells = MiniGridCells(icons.size(), size);
    for (size_t i = 0; i < cells.size(); ++i) {
        BlitCentered(out, icons[i], cells[i]);
    }
    return out;
}

// -----------------------------------------------------------------------------
// Ajout d'applis épinglées à la barre
// -----------------------------------------------------------------------------

inline bool StartsWithIgnoreCase(std::wstring_view text, std::wstring_view prefix) {
    return text.size() >= prefix.size() && EqualsIgnoreCase(text.substr(0, prefix.size()), prefix);
}

// Raccourci du répertoire « User Pinned\TaskBar » qui correspond à une vraie
// appli : ni un dossier de ce mod, ni un séparateur du mod taskbar-icon-separators.
inline bool IsSelectablePinnedShortcut(std::wstring_view fileName) {
    return HasLnkExtension(fileName) && !StartsWithIgnoreCase(fileName, kPinNamePrefix) &&
           !StartsWithIgnoreCase(fileName, L"WindhawkSeparator.");
}

// `desired` s'il est libre, sinon « Nom (2).lnk », « Nom (3).lnk »…
inline std::wstring UniqueFileName(const std::vector<std::wstring>& existing, std::wstring_view desired) {
    auto taken = [&](std::wstring_view name) {
        return std::any_of(existing.begin(), existing.end(),
                           [&](const std::wstring& file) { return EqualsIgnoreCase(file, name); });
    };
    if (!taken(desired)) {
        return std::wstring(desired);
    }
    const size_t dot = desired.find_last_of(L'.');
    const std::wstring_view stem = dot == std::wstring_view::npos ? desired : desired.substr(0, dot);
    const std::wstring_view extension = dot == std::wstring_view::npos ? std::wstring_view() : desired.substr(dot);
    for (int n = 2;; ++n) {
        std::wstring candidate = std::wstring(stem) + L" (" + std::to_wstring(n) + L")" + std::wstring(extension);
        if (!taken(candidate)) {
            return candidate;
        }
    }
}

// -----------------------------------------------------------------------------
// Icône personnalisée du dossier
// -----------------------------------------------------------------------------

// Nom de fichier sans chemin (pas de « \ », « / » ni « : »).
inline bool IsBareFileName(std::wstring_view name) {
    return !name.empty() && name.find_first_of(L"\\/:") == std::wstring_view::npos;
}

// Extension en minuscules si le fichier est une image acceptée comme icône.
inline std::optional<std::wstring> NormalizedImageExtension(std::wstring_view path) {
    const size_t slash = path.find_last_of(L"\\/");
    const std::wstring_view name = slash == std::wstring_view::npos ? path : path.substr(slash + 1);
    const size_t dot = name.find_last_of(L'.');
    if (dot == std::wstring_view::npos || dot == 0) {
        return std::nullopt;
    }
    std::wstring extension(name.substr(dot));
    for (wchar_t& c : extension) {
        c = FoldCase(c);
    }
    for (std::wstring_view supported : {L".png", L".jpg", L".jpeg", L".ico", L".bmp"}) {
        if (extension == supported) {
            return extension;
        }
    }
    return std::nullopt;
}

// Fichier et numéro d'icône à lire pour un raccourci : l'icône qu'il déclare,
// sinon celle de sa cible (numéro 0). Aucun fichier (appli du Store…) : nullopt.
inline std::optional<std::pair<std::wstring, int>> IconSourceFor(std::wstring_view iconPath, int iconIndex,
                                                                std::wstring_view targetPath) {
    const std::wstring icon = Trim(iconPath);
    if (!icon.empty()) {
        return std::make_pair(icon, iconIndex);
    }
    const std::wstring target = Trim(targetPath);
    if (!target.empty()) {
        return std::make_pair(target, 0);
    }
    return std::nullopt;
}

// Les bitmaps couleur des icônes sont en alpha « droit » : on les convertit.
inline void PremultiplyImage(Bgra& image) {
    for (uint32_t& pixel : image.pixels) {
        pixel = Premultiply(static_cast<uint8_t>(pixel >> 24), static_cast<uint8_t>(pixel >> 16),
                            static_cast<uint8_t>(pixel >> 8), static_cast<uint8_t>(pixel));
    }
}

// IShellItemImageFactory renvoie parfois de l'alpha « droit » (icônes
// redimensionnées) : un seul pixel dont une couleur dépasse l'alpha le prouve,
// et sans conversion ces pixels s'ajouteraient au fond (liseré blanc). Une image
// déjà prémultipliée n'a jamais un tel pixel et reste intacte.
inline void EnsurePremultiplied(Bgra& image) {
    for (uint32_t pixel : image.pixels) {
        const uint32_t alpha = pixel >> 24;
        if (((pixel >> 16) & 0xFF) > alpha || ((pixel >> 8) & 0xFF) > alpha || (pixel & 0xFF) > alpha) {
            PremultiplyImage(image);
            return;
        }
    }
}

// Icône ancienne (alpha entièrement nul) : la transparence vient du masque,
// noir = opaque, blanc = transparent.
inline void ApplyIconMask(Bgra& color, const Bgra& mask) {
    if (color.Empty() || mask.width != color.width || mask.height != color.height) {
        return;
    }
    for (uint32_t pixel : color.pixels) {
        if (pixel >> 24) {
            return;
        }
    }
    for (size_t i = 0; i < color.pixels.size(); ++i) {
        const bool transparent = (mask.pixels[i] & 0x00FFFFFFu) != 0;
        color.pixels[i] = transparent ? 0u : (color.pixels[i] | 0xFF000000u);
    }
}

// -----------------------------------------------------------------------------
// Couleurs du réglage de teinte
// -----------------------------------------------------------------------------

// "AARRGGBB" ou "RRGGBB" (alpha FF), « # » et espaces autour tolérés, comme le
// réglage AccentBlurBehind du mod translucent-windows. Renvoie 0xAARRGGBB.
inline std::optional<uint32_t> ParseArgbHex(std::wstring_view text) {
    std::wstring hex = Trim(text);
    if (!hex.empty() && hex.front() == L'#') {
        hex.erase(0, 1);
    }
    if (hex.size() != 6 && hex.size() != 8) {
        return std::nullopt;
    }
    uint32_t value = 0;
    for (wchar_t c : hex) {
        uint32_t digit = 0;
        if (c >= L'0' && c <= L'9') {
            digit = c - L'0';
        } else if (c >= L'a' && c <= L'f') {
            digit = 10 + (c - L'a');
        } else if (c >= L'A' && c <= L'F') {
            digit = 10 + (c - L'A');
        } else {
            return std::nullopt;
        }
        value = (value << 4) | digit;
    }
    return hex.size() == 6 ? (0xFF000000u | value) : value;
}

// ACCENT_POLICY::GradientColor attend 0xAABBGGRR.
inline uint32_t ArgbToAbgr(uint32_t argb) {
    return (argb & 0xFF00FF00u) | ((argb >> 16) & 0xFFu) | ((argb & 0xFFu) << 16);
}

// -----------------------------------------------------------------------------
// Ouverture / fermeture de la fenêtre (horloge injectée, en ms)
// -----------------------------------------------------------------------------

enum class PopupMode { Closed, Hover, Pinned };
enum class PopupAction { None, Show, Close };

struct PopupTimings {
    uint64_t openDelay = 300;
    uint64_t closeDelay = 400;
    // Un clic sur le bouton juste après une fermeture par perte d'activation
    // est le clic qui a provoqué cette fermeture : il ne doit pas rouvrir.
    uint64_t toggleGuard = 300;
};

class PopupStateMachine {
public:
    explicit PopupStateMachine(PopupTimings timings = {}) : timings_(timings) {}

    void SetTimings(PopupTimings timings) { timings_ = timings; }
    PopupMode Mode() const { return mode_; }
    const std::wstring& FolderId() const { return folderId_; }
    const std::wstring& PendingFolderId() const { return pending_; }

    PopupAction OnHoverEnter(std::wstring_view id, uint64_t now) {
        if (mode_ == PopupMode::Closed) {
            if (pending_ != id) {
                pending_ = std::wstring(id);
                hoverStart_ = now;
            }
            return PopupAction::None;
        }
        outside_ = false;
        if (folderId_ == id) {
            return PopupAction::None;
        }
        folderId_ = std::wstring(id);
        return PopupAction::Show;
    }

    PopupAction OnTick(bool cursorInside, uint64_t now) {
        switch (mode_) {
            case PopupMode::Closed:
                if (pending_.empty()) {
                    return PopupAction::None;
                }
                if (!cursorInside) {
                    pending_.clear();
                    return PopupAction::None;
                }
                if (now - hoverStart_ < timings_.openDelay) {
                    return PopupAction::None;
                }
                mode_ = PopupMode::Hover;
                folderId_ = std::move(pending_);
                pending_.clear();
                outside_ = false;
                return PopupAction::Show;
            case PopupMode::Hover:
                if (cursorInside) {
                    outside_ = false;
                    return PopupAction::None;
                }
                if (!outside_) {
                    outside_ = true;
                    outsideSince_ = now;
                    return PopupAction::None;
                }
                if (now - outsideSince_ < timings_.closeDelay) {
                    return PopupAction::None;
                }
                return CloseNow();
            case PopupMode::Pinned:
                return PopupAction::None;
        }
        return PopupAction::None;
    }

    PopupAction OnButtonClick(std::wstring_view id, uint64_t now) {
        if (mode_ == PopupMode::Closed) {
            if (lastDeactivatedFolder_ == id && now - lastDeactivated_ < timings_.toggleGuard) {
                return PopupAction::None;
            }
            pending_.clear();
            mode_ = PopupMode::Pinned;
            folderId_ = std::wstring(id);
            return PopupAction::Show;
        }
        if (mode_ == PopupMode::Pinned && folderId_ == id) {
            return CloseNow();
        }
        mode_ = PopupMode::Pinned;
        folderId_ = std::wstring(id);
        outside_ = false;
        return PopupAction::Show;
    }

    PopupAction Pin() {
        if (mode_ != PopupMode::Hover) {
            return PopupAction::None;
        }
        mode_ = PopupMode::Pinned;
        return PopupAction::Show;
    }

    PopupAction OnDeactivated(uint64_t now) {
        if (mode_ != PopupMode::Pinned) {
            return PopupAction::None;
        }
        lastDeactivatedFolder_ = folderId_;
        lastDeactivated_ = now;
        return CloseNow();
    }

    PopupAction OnEscape() { return mode_ == PopupMode::Closed ? PopupAction::None : CloseNow(); }

    PopupAction OnAppLaunched() { return mode_ == PopupMode::Closed ? PopupAction::None : CloseNow(); }

    PopupAction OnFolderRemoved(std::wstring_view id) {
        if (pending_ == id) {
            pending_.clear();
        }
        if (mode_ != PopupMode::Closed && folderId_ == id) {
            return CloseNow();
        }
        return PopupAction::None;
    }

private:
    PopupAction CloseNow() {
        mode_ = PopupMode::Closed;
        folderId_.clear();
        pending_.clear();
        outside_ = false;
        return PopupAction::Close;
    }

    PopupTimings timings_;
    PopupMode mode_ = PopupMode::Closed;
    std::wstring folderId_;
    std::wstring pending_;
    uint64_t hoverStart_ = 0;
    bool outside_ = false;
    uint64_t outsideSince_ = 0;
    std::wstring lastDeactivatedFolder_;
    uint64_t lastDeactivated_ = 0;
};

}  // namespace tbf
// ----- end logic.h -----
// ----- begin store.h -----

// À inclure après logic.h (les headers de src/ ne s'incluent pas entre eux).

#include <windows.h>
#include <shellapi.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace tbf {

struct FolderApp {
    std::wstring fileName;  // "Steam.lnk"
    std::wstring path;      // chemin complet du .lnk
    std::wstring name;      // "Steam"
};

struct Folder {
    std::wstring id;
    std::wstring name;
    std::wstring dir;
    std::vector<FolderApp> apps;
    std::wstring iconPath;  // vide : mini-grille des applis
    int iconIndex = 0;      // numéro d'icône dans un .exe/.dll (0 pour une image)
};

// Nom (sans extension) de l'image copiée dans le répertoire du dossier.
inline constexpr wchar_t kIconImageStem[] = L"folder-icon";

inline constexpr wchar_t kDefaultFolderName[] = L"Folder";

inline bool PathExists(const std::wstring& path) {
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

inline bool IsDirectory(const std::wstring& path) {
    const DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

inline std::wstring JoinPath(std::wstring_view a, std::wstring_view b) {
    std::wstring result(a);
    if (!result.empty() && result.back() != L'\\') {
        result += L'\\';
    }
    result += b;
    return result;
}

// Corbeille (`recycle`) ou suppression définitive, fichier ou répertoire.
inline bool RecycleOrDelete(const std::wstring& path, bool recycle) {
    std::wstring from = path;
    from.push_back(L'\0');  // SHFileOperationW exige une double terminaison
    SHFILEOPSTRUCTW operation{};
    operation.wFunc = FO_DELETE;
    operation.pFrom = from.c_str();
    operation.fFlags = static_cast<FILEOP_FLAGS>(FOF_NO_UI | (recycle ? FOF_ALLOWUNDO : 0));
    return SHFileOperationW(&operation) == 0 && !operation.fAnyOperationsAborted;
}

// WritePrivateProfileStringW n'écrit en UTF-16 que si le fichier existe déjà
// avec une BOM ; sinon il écrit en ANSI et perd les caractères hors page de code.
inline bool EnsureUnicodeIni(const std::wstring& path) {
    if (PathExists(path)) {
        return true;
    }
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return GetLastError() == ERROR_FILE_EXISTS;
    }
    const WORD bom = 0xFEFF;
    DWORD written = 0;
    const BOOL ok = WriteFile(file, &bom, sizeof(bom), &written, nullptr);
    CloseHandle(file);
    return ok && written == sizeof(bom);
}

class FolderStore {
public:
    explicit FolderStore(std::wstring root) : root_(std::move(root)) {}

    const std::wstring& Root() const { return root_; }
    std::wstring PinsDir() const { return JoinPath(root_, L"_pins"); }
    std::wstring FolderDir(std::wstring_view id) const { return JoinPath(root_, id); }

    bool EnsureRoot() const {
        CreateDirectoryW(root_.c_str(), nullptr);
        CreateDirectoryW(PinsDir().c_str(), nullptr);
        return IsDirectory(root_) && IsDirectory(PinsDir());
    }

    std::vector<std::wstring> ListIds() const {
        std::vector<std::wstring> ids;
        WIN32_FIND_DATAW data;
        HANDLE find = FindFirstFileW(JoinPath(root_, L"*").c_str(), &data);
        if (find == INVALID_HANDLE_VALUE) {
            return ids;
        }
        do {
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && IsValidFolderId(data.cFileName)) {
                ids.emplace_back(data.cFileName);
            }
        } while (FindNextFileW(find, &data));
        FindClose(find);
        std::sort(ids.begin(), ids.end());
        return ids;
    }

    std::vector<Folder> List() const {
        std::vector<Folder> folders;
        for (const auto& id : ListIds()) {
            if (auto folder = Get(id)) {
                folders.push_back(std::move(*folder));
            }
        }
        return folders;
    }

    std::optional<Folder> Get(std::wstring_view id) const {
        if (!IsValidFolderId(id)) {
            return std::nullopt;
        }
        Folder folder;
        folder.id = std::wstring(id);
        folder.dir = FolderDir(id);
        if (!IsDirectory(folder.dir)) {
            return std::nullopt;
        }

        const std::wstring ini = JoinPath(folder.dir, L"folder.ini");
        wchar_t name[256] = {};
        GetPrivateProfileStringW(L"Folder", L"Name", kDefaultFolderName, name, ARRAYSIZE(name), ini.c_str());
        folder.name = Trim(name);
        if (folder.name.empty()) {
            folder.name = kDefaultFolderName;
        }

        std::vector<wchar_t> section(32768);
        const DWORD length = GetPrivateProfileSectionW(L"Order", section.data(),
                                                       static_cast<DWORD>(section.size()), ini.c_str());
        const auto order = OrderFromSection(ParseIniSection(std::wstring_view(section.data(), length)));

        std::vector<std::wstring> files;
        WIN32_FIND_DATAW data;
        HANDLE find = FindFirstFileW(JoinPath(folder.dir, L"*.lnk").c_str(), &data);
        if (find != INVALID_HANDLE_VALUE) {
            do {
                // "*.lnk" correspond aussi à "x.lnkx" via les noms courts 8.3.
                if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && HasLnkExtension(data.cFileName)) {
                    files.emplace_back(data.cFileName);
                }
            } while (FindNextFileW(find, &data));
            FindClose(find);
        }

        for (const auto& file : OrderApps(order, files)) {
            folder.apps.push_back({file, JoinPath(folder.dir, file), DisplayNameFromFile(file)});
        }

        wchar_t icon[MAX_PATH] = {};
        GetPrivateProfileStringW(L"Folder", L"Icon", L"", icon, ARRAYSIZE(icon), ini.c_str());
        const std::wstring iconValue = Trim(icon);
        if (!iconValue.empty()) {
            // Un nom simple désigne l'image copiée dans le répertoire du dossier.
            folder.iconPath = IsBareFileName(iconValue) ? JoinPath(folder.dir, iconValue) : iconValue;
            folder.iconIndex = static_cast<int>(GetPrivateProfileIntW(L"Folder", L"IconIndex", 0, ini.c_str()));
        }
        return folder;
    }

    // Remplace l'ordre des applis ([Order] 1=…, 2=…). Les fichiers absents de la
    // liste se placent après, par nom (voir OrderApps).
    bool SetOrder(std::wstring_view id, const std::vector<std::wstring>& fileNames) const {
        if (!IsValidFolderId(id) || !IsDirectory(FolderDir(id))) {
            return false;
        }
        const std::wstring ini = JoinPath(FolderDir(id), L"folder.ini");
        if (!EnsureUnicodeIni(ini)) {
            return false;
        }
        WritePrivateProfileStringW(L"Order", nullptr, nullptr, ini.c_str());  // vide la section
        for (size_t i = 0; i < fileNames.size(); ++i) {
            if (!WritePrivateProfileStringW(L"Order", std::to_wstring(i + 1).c_str(), fileNames[i].c_str(),
                                            ini.c_str())) {
                return false;
            }
        }
        return true;
    }

    // Copie un raccourci dans le dossier (renommé s'il existe déjà) ; renvoie
    // le nom du fichier créé.
    std::optional<std::wstring> AddApp(std::wstring_view id, const std::wstring& sourceLnk) const {
        if (!IsValidFolderId(id) || !IsDirectory(FolderDir(id)) || !PathExists(sourceLnk) ||
            IsDirectory(sourceLnk)) {
            return std::nullopt;
        }
        const size_t slash = sourceLnk.find_last_of(L"\\/");
        const std::wstring sourceName = slash == std::wstring::npos ? sourceLnk : sourceLnk.substr(slash + 1);
        if (!HasLnkExtension(sourceName)) {
            return std::nullopt;
        }
        std::vector<std::wstring> existing;
        if (auto folder = Get(id)) {
            for (const auto& app : folder->apps) {
                existing.push_back(app.fileName);
            }
        }
        const std::wstring name = UniqueFileName(existing, sourceName);
        if (!CopyFileW(sourceLnk.c_str(), JoinPath(FolderDir(id), name).c_str(), TRUE)) {
            return std::nullopt;
        }
        return name;
    }

    // Icône prise dans un .ico/.exe/.dll (chemin complet + numéro).
    bool SetIconResource(std::wstring_view id, const std::wstring& path, int index) const {
        if (!IsValidFolderId(id) || !IsDirectory(FolderDir(id)) || path.empty() || IsBareFileName(path)) {
            return false;
        }
        RemoveIconImages(id);
        return WriteIcon(id, path.c_str(), std::to_wstring(index).c_str());
    }

    // Image copiée dans le répertoire du dossier : l'original peut disparaître.
    bool SetIconImage(std::wstring_view id, const std::wstring& sourcePath) const {
        const auto extension = NormalizedImageExtension(sourcePath);
        if (!IsValidFolderId(id) || !IsDirectory(FolderDir(id)) || !extension || !PathExists(sourcePath) ||
            IsDirectory(sourcePath)) {
            return false;
        }
        const std::wstring fileName = std::wstring(kIconImageStem) + *extension;
        const std::wstring target = JoinPath(FolderDir(id), fileName);
        const std::wstring staging = target + L".tmp";
        if (!CopyFileW(sourcePath.c_str(), staging.c_str(), FALSE)) {
            return false;
        }
        RemoveIconImages(id);
        if (!MoveFileExW(staging.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            DeleteFileW(staging.c_str());
            return false;
        }
        return WriteIcon(id, fileName.c_str(), L"0");
    }

    bool ClearIcon(std::wstring_view id) const {
        if (!IsValidFolderId(id) || !IsDirectory(FolderDir(id))) {
            return false;
        }
        RemoveIconImages(id);
        return WriteIcon(id, nullptr, nullptr);
    }

    // `random` choisit l'id ; s'il est déjà pris, renvoie nullopt et l'appelant
    // retente avec une autre valeur.
    std::optional<std::wstring> Create(std::wstring_view name, uint32_t random) const {
        const std::wstring clean = Trim(name);
        if (clean.empty() || !EnsureRoot()) {
            return std::nullopt;
        }
        const std::wstring id = FormatFolderId(random);
        const std::wstring dir = FolderDir(id);
        if (!CreateDirectoryW(dir.c_str(), nullptr)) {
            return std::nullopt;
        }
        if (!WriteName(id, clean)) {
            RecycleOrDelete(dir, false);
            return std::nullopt;
        }
        return id;
    }

    bool Rename(std::wstring_view id, std::wstring_view name) const {
        const std::wstring clean = Trim(name);
        if (clean.empty() || !IsValidFolderId(id) || !IsDirectory(FolderDir(id))) {
            return false;
        }
        return WriteName(id, clean);
    }

    bool Delete(std::wstring_view id, bool recycle) const {
        if (!IsValidFolderId(id)) {
            return false;
        }
        const std::wstring dir = FolderDir(id);
        return IsDirectory(dir) && RecycleOrDelete(dir, recycle);
    }

    bool RemoveApp(std::wstring_view id, std::wstring_view fileName, bool recycle) const {
        if (!IsValidFolderId(id) || fileName.empty() ||
            fileName.find_first_of(L"\\/:") != std::wstring_view::npos || !HasLnkExtension(fileName)) {
            return false;
        }
        const std::wstring path = JoinPath(FolderDir(id), fileName);
        return PathExists(path) && !IsDirectory(path) && RecycleOrDelete(path, recycle);
    }

private:
    // `path` nul : supprime les clés (retour à la mini-grille).
    bool WriteIcon(std::wstring_view id, const wchar_t* path, const wchar_t* index) const {
        const std::wstring ini = JoinPath(FolderDir(id), L"folder.ini");
        return EnsureUnicodeIni(ini) && WritePrivateProfileStringW(L"Folder", L"Icon", path, ini.c_str()) &&
               WritePrivateProfileStringW(L"Folder", L"IconIndex", index, ini.c_str());
    }

    void RemoveIconImages(std::wstring_view id) const {
        for (const wchar_t* extension : {L".png", L".jpg", L".jpeg", L".ico", L".bmp"}) {
            DeleteFileW(JoinPath(FolderDir(id), std::wstring(kIconImageStem) + extension).c_str());
        }
    }

    bool WriteName(std::wstring_view id, const std::wstring& name) const {
        const std::wstring ini = JoinPath(FolderDir(id), L"folder.ini");
        return EnsureUnicodeIni(ini) && WritePrivateProfileStringW(L"Folder", L"Name", name.c_str(), ini.c_str());
    }

    std::wstring root_;
};

inline std::wstring DefaultStoreRoot() {
    wchar_t buffer[MAX_PATH] = {};
    const DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", buffer, MAX_PATH);
    if (!length || length >= MAX_PATH) {
        return L"";
    }
    return JoinPath(buffer, L"TaskbarFolders");
}

}  // namespace tbf
// ----- end store.h -----

// === SECTION: globals ===

enum class PopupEffect { Blur, Acrylic, Opaque };

struct ModSettings {
    int openDelay = 300;
    int closeDelay = 400;
    int columns = 4;
    int iconSize = 32;
    int spacing = 12;
    PopupEffect effect = PopupEffect::Blur;
    uint32_t tintColor = 0x3A232323;  // 0xAARRGGBB
};

static std::mutex g_settingsMutex;
static ModSettings g_settings;
static std::atomic<bool> g_unloading{false};
static std::unique_ptr<tbf::FolderStore> g_store;
static std::atomic<HWND> g_controllerHwnd{nullptr};

enum : UINT {
    WM_TBF_RELOAD = WM_APP + 1,
    WM_TBF_SETTINGS,
    WM_TBF_SHUTDOWN,
    WM_TBF_HOVER,
    WM_TBF_CLICK,
    WM_TBF_COMMAND,
};

// Minuteurs de la fenêtre de contrôle (thread UI du mod).
constexpr UINT_PTR kReloadTimer = 1;
constexpr UINT_PTR kReconcileTimer = 3;

struct ButtonMessage {
    std::wstring folderId;
    tbf::Rect button;  // rectangle écran du bouton, en pixels physiques
};

enum class FolderCommand {
    NewFolder,
    Rename,
    OpenLocation,
    Delete,
    IconResource,
    IconImage,
    IconReset,
    AddApps,
};

struct CommandMessage {
    FolderCommand command;
    std::wstring folderId;
};

// Définies dans les sections suivantes.
static bool InstallTaskbarHooks();
static void RefreshTaskbarButtons();
static void RestoreTaskbarButtons();
static void Popup_Create();
static void Popup_Destroy();
static void Popup_OnSnapshotChanged();
static void Popup_OnSettingsChanged();
static void Popup_OnHover(const ButtonMessage& message);
static void Popup_OnClick(const ButtonMessage& message);
static bool Popup_PreTranslate(MSG* message);
static void RunCommand(const CommandMessage& message);

// === SECTION: utilities ===

// Textes de l'interface : français si la langue d'affichage de Windows est le
// français, anglais sinon.
static bool UiIsFrench() {
    static const bool french = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_FRENCH;
    return french;
}

static const wchar_t* Tr(const wchar_t* english, const wchar_t* french) {
    return UiIsFrench() ? french : english;
}

static ModSettings LoadSettings() {
    ModSettings s;
    s.openDelay = std::clamp(Wh_GetIntSetting(L"openDelay"), 0, 5000);
    s.closeDelay = std::clamp(Wh_GetIntSetting(L"closeDelay"), 0, 5000);
    s.columns = std::clamp(Wh_GetIntSetting(L"columns"), 1, 8);
    s.iconSize = std::clamp(Wh_GetIntSetting(L"iconSize"), 16, 64);
    s.spacing = std::clamp(Wh_GetIntSetting(L"spacing"), 0, 48);
    const std::wstring effect = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"effect")).get();
    s.effect = effect == L"acrylic"  ? PopupEffect::Acrylic
               : effect == L"opaque" ? PopupEffect::Opaque
                                     : PopupEffect::Blur;
    const std::wstring tint = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"tintColor")).get();
    s.tintColor = tbf::ParseArgbHex(tint).value_or(0x3A232323);
    return s;
}

static ModSettings GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

static HINSTANCE ModuleInstance() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ModuleInstance), &module);
    return module;
}

static uint64_t Now() {
    return GetTickCount64();
}

static uint32_t RandomUInt32() {
    uint32_t value = 0;
    BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(&value), sizeof(value), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return value;
}

static tbf::Rect CursorRect() {
    POINT cursor{};
    GetCursorPos(&cursor);
    return {cursor.x - 24, cursor.y - 24, cursor.x + 24, cursor.y + 24};
}

// Poste un message au thread UI du mod ; la charge utile est libérée par le
// destinataire (ou ici si l'envoi échoue).
template <typename T>
static void PostToUi(UINT message, std::unique_ptr<T> payload) {
    HWND controller = g_controllerHwnd.load();
    if (!controller || g_unloading) {
        return;
    }
    if (PostMessageW(controller, message, 0, reinterpret_cast<LPARAM>(payload.get()))) {
        payload.release();
    }
}

// Pixels 32 bits d'un bitmap GDI (lignes de haut en bas), sans conversion d'alpha.
static tbf::Bgra BitmapToBgra(HBITMAP bitmap) {
    BITMAP info{};
    if (!bitmap || !GetObjectW(bitmap, sizeof(info), &info) || info.bmWidth <= 0 || info.bmHeight <= 0) {
        return {};
    }
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = info.bmWidth;
    bmi.bmiHeader.biHeight = -info.bmHeight;  // lignes de haut en bas
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    tbf::Bgra image = tbf::Bgra::Blank(info.bmWidth, info.bmHeight);
    HDC dc = GetDC(nullptr);
    const int lines = GetDIBits(dc, bitmap, 0, info.bmHeight, image.pixels.data(), &bmi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    return lines == info.bmHeight ? image : tbf::Bgra{};
}

// Image Shell d'un fichier, en BGRA prémultiplié : icône (SIIGBF_ICONONLY) ou
// miniature d'une image (SIIGBF_RESIZETOFIT).
static tbf::Bgra LoadShellImage(const std::wstring& path, int size, SIIGBF flags) {
    winrt::com_ptr<IShellItemImageFactory> factory;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(factory.put())))) {
        return {};
    }
    HBITMAP bitmap = nullptr;
    if (FAILED(factory->GetImage(SIZE{size, size}, flags, &bitmap)) || !bitmap) {
        return {};
    }
    tbf::Bgra image = BitmapToBgra(bitmap);
    DeleteObject(bitmap);
    tbf::FixMissingAlpha(image);
    tbf::EnsurePremultiplied(image);
    return image;
}


// Icône numéro `index` d'un .ico/.exe/.dll, en BGRA prémultiplié.
static tbf::Bgra LoadIconResource(const std::wstring& path, int index, int size) {
    HICON icon = nullptr;
    if (PrivateExtractIconsW(path.c_str(), index, size, size, &icon, nullptr, 1, 0) != 1 || !icon) {
        return {};
    }
    tbf::Bgra color;
    tbf::Bgra mask;
    ICONINFO info{};
    if (GetIconInfo(icon, &info)) {
        color = BitmapToBgra(info.hbmColor);
        mask = BitmapToBgra(info.hbmMask);
        if (info.hbmColor) {
            DeleteObject(info.hbmColor);
        }
        if (info.hbmMask) {
            DeleteObject(info.hbmMask);
        }
    }
    DestroyIcon(icon);
    tbf::ApplyIconMask(color, mask);
    tbf::PremultiplyImage(color);
    return color;
}

// Icône d'un fichier, en BGRA prémultiplié. Pour un raccourci, elle est lue
// directement dans son fichier source (icône déclarée, sinon cible) : le cache
// d'icônes du Shell, via IShellItemImageFactory, renvoie parfois l'icône
// générique de fichier. Repli sur le Shell si la lecture directe échoue.
static tbf::Bgra LoadShellIcon(const std::wstring& path, int size) {
    if (tbf::HasLnkExtension(path)) {
        winrt::com_ptr<IShellLinkW> link;
        winrt::com_ptr<IPersistFile> file;
        wchar_t iconPath[MAX_PATH] = {};
        int iconIndex = 0;
        wchar_t target[MAX_PATH] = {};
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(link.put()))) &&
            SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(file.put()))) &&
            SUCCEEDED(file->Load(path.c_str(), STGM_READ))) {
            link->GetIconLocation(iconPath, MAX_PATH, &iconIndex);
            link->GetPath(target, MAX_PATH, nullptr, 0);
        }
        if (auto source = tbf::IconSourceFor(iconPath, iconIndex, target)) {
            wchar_t expanded[MAX_PATH] = {};
            ExpandEnvironmentStringsW(source->first.c_str(), expanded, MAX_PATH);
            tbf::Bgra icon = LoadIconResource(expanded, source->second, size);
            if (!icon.Empty()) {
                return icon;
            }
        }
    }
    return LoadShellImage(path, size, SIIGBF_ICONONLY);
}

// La barre des tâches et ses menus suivent le thème « système », pas celui des applis.
static bool IsDarkTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS) {
        return true;
    }
    return value == 0;
}

// === SECTION: pins ===

// API COM privée de la barre (mêmes GUID que le mod taskbar-icon-separators).
static const CLSID CLSID_PinManager = {0xA5C8D635, 0xB4ED, 0x452B, {0x81, 0x09, 0x95, 0x01, 0x78, 0x10, 0x96, 0xD1}};
static const IID IID_IPinManagerInterop2 = {0x87D9E034, 0x56D0, 0x4F8C, {0xBE, 0x59, 0x99, 0x7B, 0x01, 0x75, 0x47, 0x10}};
static const IID IID_IPinManagerInterop3 = {0x27216D91, 0x78B8, 0x406D, {0x85, 0xC4, 0x5F, 0xDB, 0xF5, 0x1C, 0xD1, 0x6B}};

enum PINNEDLISTMODIFYCALLER : int {
    PMC_JUMPVIEWBROKER = 11,
    PMC_TASKBANDPIN = 25,
};

struct IPinManagerInterop2 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE PinItemToTaskbarShim(PCUIDLIST_ABSOLUTE pidl, PINNEDLISTMODIFYCALLER caller) = 0;
    virtual HRESULT STDMETHODCALLTYPE PinItemFromTrustedCaller(PCUIDLIST_ABSOLUTE pidl, PINNEDLISTMODIFYCALLER caller) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyPrependDefaultTaskbarLayout() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyAppendDefaultTaskbarLayout() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyInPlaceTaskbarLayout(int layoutType) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyReorderTaskbarLayout(int layoutType, int value) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnpinTaskbarItem(PCUIDLIST_ABSOLUTE pidl, PINNEDLISTMODIFYCALLER caller) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdatePinnedTaskbarItem(PCUIDLIST_ABSOLUTE oldPidl, PCUIDLIST_ABSOLUTE newPidl,
                                                              PINNEDLISTMODIFYCALLER caller) = 0;
};

struct IPinManagerInterop3 : IPinManagerInterop2 {
    virtual HRESULT STDMETHODCALLTYPE MoveTaskbarPin(PCUIDLIST_ABSOLUTE pidl, int index,
                                                     PINNEDLISTMODIFYCALLER caller) = 0;
};

static std::wstring PinShortcutPath(std::wstring_view id) {
    return tbf::JoinPath(g_store->PinsDir(), tbf::PinShortcutStem(id) + L".lnk");
}

// Quand un raccourci est épinglé, l'Explorateur en dépose une copie (même nom)
// dans ce répertoire.
static std::wstring UserPinnedTaskbarDir() {
    PWSTR roaming = nullptr;
    std::wstring dir;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &roaming))) {
        dir = tbf::JoinPath(roaming, L"Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar");
    }
    CoTaskMemFree(roaming);
    return dir;
}

static bool IsFolderPinned(std::wstring_view id) {
    return tbf::PathExists(tbf::JoinPath(UserPinnedTaskbarDir(), tbf::PinShortcutStem(id) + L".lnk"));
}

static HRESULT SetShortcutAppId(IShellLinkW* link, const std::wstring& appId) {
    winrt::com_ptr<IPropertyStore> store;
    HRESULT hr = link->QueryInterface(IID_PPV_ARGS(store.put()));
    if (FAILED(hr)) {
        return hr;
    }
    PROPVARIANT value;
    PropVariantInit(&value);
    hr = InitPropVariantFromString(appId.c_str(), &value);
    if (SUCCEEDED(hr)) {
        hr = store->SetValue(PKEY_AppUserModel_ID, value);
    }
    if (SUCCEEDED(hr)) {
        hr = store->Commit();
    }
    PropVariantClear(&value);
    return hr;
}

static HRESULT CreatePinShortcut(std::wstring_view id) {
    winrt::com_ptr<IShellLinkW> link;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(link.put()));
    if (FAILED(hr)) {
        return hr;
    }
    wchar_t system[MAX_PATH] = {};
    const UINT length = GetSystemDirectoryW(system, MAX_PATH);
    if (!length || length >= MAX_PATH) {
        return E_FAIL;
    }
    // systray.exe est un exécutable Windows inerte : un bouton orphelin cliqué
    // (mod désactivé) ne fait rien.
    const std::wstring target = tbf::JoinPath(system, L"systray.exe");
    if (!tbf::PathExists(target)) {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }
    const std::wstring stem = tbf::PinShortcutStem(id);
    hr = link->SetPath(target.c_str());
    if (SUCCEEDED(hr)) {
        hr = link->SetShowCmd(SW_HIDE);
    }
    SHSTOCKICONINFO icon{};
    icon.cbSize = sizeof(icon);
    if (SUCCEEDED(hr) && SUCCEEDED(SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICONLOCATION, &icon))) {
        hr = link->SetIconLocation(icon.szPath, icon.iIcon);
    }
    if (SUCCEEDED(hr)) {
        hr = link->SetDescription(stem.c_str());
    }
    if (SUCCEEDED(hr)) {
        hr = SetShortcutAppId(link.get(), tbf::FolderAppId(id));
    }
    winrt::com_ptr<IPersistFile> file;
    if (SUCCEEDED(hr)) {
        hr = link->QueryInterface(IID_PPV_ARGS(file.put()));
    }
    const std::wstring path = PinShortcutPath(id);
    if (SUCCEEDED(hr)) {
        hr = file->Save(path.c_str(), TRUE);
    }
    if (SUCCEEDED(hr)) {
        SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, path.c_str(), nullptr);
    }
    return hr;
}

static HRESULT CreatePinManager(winrt::com_ptr<IPinManagerInterop3>& pinManager) {
    winrt::com_ptr<IPinManagerInterop2> interop2;
    HRESULT hr = CoCreateInstance(CLSID_PinManager, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                                  IID_IPinManagerInterop2, interop2.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"[PIN] CoCreateInstance(PinManager) 0x%08X", static_cast<unsigned>(hr));
        return hr;
    }
    hr = interop2->QueryInterface(IID_IPinManagerInterop3, pinManager.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"[PIN] QueryInterface(IPinManagerInterop3) 0x%08X", static_cast<unsigned>(hr));
    }
    return hr;
}

static HRESULT WithPidl(const std::wstring& path, const std::function<HRESULT(PCIDLIST_ABSOLUTE)>& action) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
    if (FAILED(hr) || !pidl) {
        return FAILED(hr) ? hr : E_FAIL;
    }
    hr = action(pidl);
    CoTaskMemFree(pidl);
    return hr;
}

static bool PinFolder(IPinManagerInterop3* pinManager, std::wstring_view id) {
    const std::wstring path = PinShortcutPath(id);
    if (!tbf::PathExists(path)) {
        const HRESULT hr = CreatePinShortcut(id);
        if (FAILED(hr)) {
            Wh_Log(L"[PIN] Creating %s: 0x%08X", path.c_str(), static_cast<unsigned>(hr));
            return false;
        }
    }
    const HRESULT hr = WithPidl(path, [&](PCIDLIST_ABSOLUTE pidl) {
        return pinManager->PinItemFromTrustedCaller(pidl, PMC_TASKBANDPIN);
    });
    Wh_Log(L"[PIN] Pinning %s: 0x%08X", std::wstring(id).c_str(), static_cast<unsigned>(hr));
    return SUCCEEDED(hr);
}

static bool UnpinFolder(IPinManagerInterop3* pinManager, std::wstring_view id) {
    const std::wstring path = PinShortcutPath(id);
    // Le fichier est nécessaire pour obtenir le PIDL de l'élément épinglé.
    if (!tbf::PathExists(path) && FAILED(CreatePinShortcut(id))) {
        return false;
    }
    const HRESULT hr = WithPidl(path, [&](PCIDLIST_ABSOLUTE pidl) {
        return pinManager->UnpinTaskbarItem(pidl, PMC_JUMPVIEWBROKER);
    });
    Wh_Log(L"[PIN] Unpinning %s: 0x%08X", std::wstring(id).c_str(), static_cast<unsigned>(hr));
    if (SUCCEEDED(hr)) {
        DeleteFileW(path.c_str());
        SHChangeNotify(SHCNE_DELETE, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, path.c_str(), nullptr);
    }
    return SUCCEEDED(hr);
}

static std::vector<std::wstring> EnumeratePinIds(const std::wstring& dir) {
    std::vector<std::wstring> ids;
    if (dir.empty()) {
        return ids;
    }
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileW(tbf::JoinPath(dir, L"TaskbarFolder-*.lnk").c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        return ids;
    }
    do {
        if (auto id = tbf::ExtractFolderIdFromText(data.cFileName)) {
            ids.push_back(*id);
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    return ids;
}

// Épingle les dossiers qui ne le sont pas ; désépingle les épingles dont le
// répertoire de dossier a disparu. Renvoie faux si une opération a échoué.
static bool ReconcilePins(const std::vector<tbf::Folder>& folders) {
    winrt::com_ptr<IPinManagerInterop3> pinManager;
    if (FAILED(CreatePinManager(pinManager))) {
        return false;
    }
    bool ok = true;
    for (const auto& folder : folders) {
        if (!IsFolderPinned(folder.id)) {
            ok = PinFolder(pinManager.get(), folder.id) && ok;
        }
    }
    // Orpheline = son répertoire de dossier n'existe pas. On teste chaque id
    // individuellement : une énumération ratée ne doit jamais tout désépingler.
    if (!tbf::IsDirectory(g_store->Root())) {
        return ok;
    }
    auto candidates = EnumeratePinIds(g_store->PinsDir());
    auto pinned = EnumeratePinIds(UserPinnedTaskbarDir());
    candidates.insert(candidates.end(), pinned.begin(), pinned.end());
    std::sort(candidates.begin(), candidates.end());
    candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());
    for (const auto& id : candidates) {
        if (!tbf::IsDirectory(g_store->FolderDir(id))) {
            Wh_Log(L"[PIN] Orphan pin %s", id.c_str());
            ok = UnpinFolder(pinManager.get(), id) && ok;
        }
    }
    return ok;
}

// === SECTION: snapshot ===

// Taille de l'icône mini-grille : assez grande pour rester nette à 250 %.
constexpr int kMiniGridSize = 64;

struct MiniGridImage {
    uint64_t generation = 0;
    tbf::Bgra image;
};

// Écrit par le thread UI du mod uniquement ; lu partout sous verrou partagé.
static std::shared_mutex g_snapshotMutex;
static std::vector<tbf::Folder> g_folders;
static std::unordered_map<std::wstring, std::shared_ptr<const MiniGridImage>> g_miniGrids;

// Thread UI du mod uniquement.
static uint64_t g_miniGridGeneration = 0;
static int g_reconcileRetries = 0;

static std::optional<tbf::Folder> FindFolder(const std::wstring& id) {
    std::shared_lock lock(g_snapshotMutex);
    for (const auto& folder : g_folders) {
        if (folder.id == id) {
            return folder;
        }
    }
    return std::nullopt;
}

static std::shared_ptr<const MiniGridImage> FindMiniGrid(const std::wstring& id) {
    std::shared_lock lock(g_snapshotMutex);
    auto it = g_miniGrids.find(id);
    return it == g_miniGrids.end() ? nullptr : it->second;
}

// Icône personnalisée du dossier (image copiée ou icône Windows), vide si
// aucune n'est définie ou si elle ne se charge pas (retour à la mini-grille).
static tbf::Bgra LoadFolderIcon(const tbf::Folder& folder) {
    if (folder.iconPath.empty()) {
        return {};
    }
    tbf::Bgra icon = tbf::NormalizedImageExtension(folder.iconPath)
                         ? LoadShellImage(folder.iconPath, kMiniGridSize, SIIGBF_RESIZETOFIT)
                         : LoadIconResource(folder.iconPath, folder.iconIndex, kMiniGridSize);
    if (icon.Empty()) {
        Wh_Log(L"[ICON] %s,%d unreadable: using the mini grid", folder.iconPath.c_str(), folder.iconIndex);
    }
    return icon;
}

static void ReloadSnapshot() {
    auto folders = g_store->List();

    std::unordered_map<std::wstring, std::shared_ptr<const MiniGridImage>> grids;
    const auto cells = tbf::MiniGridCells(4, kMiniGridSize);
    const int cellSize = cells.empty() ? 16 : cells[0].Width();
    for (const auto& folder : folders) {
        auto grid = std::make_shared<MiniGridImage>();
        grid->generation = ++g_miniGridGeneration;
        const tbf::Bgra custom = LoadFolderIcon(folder);
        if (!custom.Empty()) {
            grid->image = tbf::Bgra::Blank(kMiniGridSize, kMiniGridSize);
            tbf::BlitCentered(grid->image, custom, {0, 0, kMiniGridSize, kMiniGridSize});
        } else {
            std::vector<tbf::Bgra> icons;
            for (size_t i = 0; i < folder.apps.size() && i < 4; ++i) {
                icons.push_back(LoadShellIcon(folder.apps[i].path, cellSize));
            }
            grid->image = tbf::ComposeMiniGrid(icons, kMiniGridSize, tbf::kMiniGridBackground);
        }
        grids.emplace(folder.id, std::move(grid));
    }

    {
        std::unique_lock lock(g_snapshotMutex);
        g_folders = folders;
        g_miniGrids = std::move(grids);
    }
    Wh_Log(L"[STORE] %d folder(s)", static_cast<int>(folders.size()));

    if (ReconcilePins(folders)) {
        g_reconcileRetries = 0;
    } else if (g_reconcileRetries < 12) {
        // Au démarrage d'explorer.exe, la barre n'est pas toujours prête.
        ++g_reconcileRetries;
        SetTimer(g_controllerHwnd.load(), kReconcileTimer, 5000, nullptr);
    }

    RefreshTaskbarButtons();
    Popup_OnSnapshotChanged();
}

// === SECTION: taskbar bridge ===

// Dernier rectangle connu de chaque bouton dossier (mis à jour au survol) : le
// hook de clic natif n'a pas accès à l'élément XAML.
static std::mutex g_buttonRectsMutex;
static std::unordered_map<std::wstring, tbf::Rect> g_buttonRects;

static void RememberButtonRect(const std::wstring& id, const tbf::Rect& rect) {
    std::lock_guard lock(g_buttonRectsMutex);
    g_buttonRects[id] = rect;
}

static tbf::Rect ButtonRectFor(const std::wstring& id) {
    std::lock_guard lock(g_buttonRectsMutex);
    auto it = g_buttonRects.find(id);
    return it != g_buttonRects.end() ? it->second : CursorRect();
}

// Exécute `procedure` sur le thread qui possède `window`, de façon synchrone
// (technique reprise du mod taskbar-icon-separators).
using RunFromWindowThreadProc = void(WINAPI*)(void* parameter);

struct RunFromWindowThreadParameters {
    RunFromWindowThreadProc procedure;
    void* parameter;
    bool executed;
};

static UINT RunFromWindowThreadMessage() {
    static const UINT message = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

static LRESULT CALLBACK RunFromWindowThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const auto* message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (message->message == RunFromWindowThreadMessage() && message->lParam) {
            auto* parameters = reinterpret_cast<RunFromWindowThreadParameters*>(message->lParam);
            if (!parameters->executed) {
                parameters->executed = true;
                parameters->procedure(parameters->parameter);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static bool RunFromWindowThread(HWND window, RunFromWindowThreadProc procedure, void* parameter) {
    const DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        procedure(parameter);
        return true;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHook, nullptr, threadId);
    if (!hook) {
        return false;
    }
    RunFromWindowThreadParameters parameters{procedure, parameter, false};
    SendMessageW(window, RunFromWindowThreadMessage(), 0, reinterpret_cast<LPARAM>(&parameters));
    UnhookWindowsHookEx(hook);
    return parameters.executed;
}

// Une fenêtre de barre des tâches (principale ou secondaire) par thread.
static std::vector<HWND> TaskbarWindows() {
    struct Context {
        DWORD processId;
        std::vector<HWND> windows;
        std::vector<DWORD> threads;
    } context{GetCurrentProcessId(), {}, {}};
    EnumWindows(
        [](HWND window, LPARAM lParam) -> BOOL {
            auto* context = reinterpret_cast<Context*>(lParam);
            DWORD processId = 0;
            const DWORD threadId = GetWindowThreadProcessId(window, &processId);
            if (processId != context->processId) {
                return TRUE;
            }
            wchar_t className[64] = {};
            GetClassNameW(window, className, ARRAYSIZE(className));
            if (_wcsicmp(className, L"Shell_TrayWnd") != 0 && _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
                return TRUE;
            }
            if (std::find(context->threads.begin(), context->threads.end(), threadId) == context->threads.end()) {
                context->threads.push_back(threadId);
                context->windows.push_back(window);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.windows;
}

// === SECTION: taskbar.dll hooks ===

static void* g_taskGroupGetAppId = nullptr;
static void* g_taskBtnGroupGetGroup = nullptr;

static std::optional<std::wstring> FolderIdFromTaskGroup(void* taskGroup) {
    if (!taskGroup || !g_taskGroupGetAppId) {
        return std::nullopt;
    }
    using GetAppId_t = const wchar_t*(WINAPI*)(void* pThis);
    const wchar_t* appId = reinterpret_cast<GetAppId_t>(g_taskGroupGetAppId)(taskGroup);
    if (!appId) {
        return std::nullopt;
    }
    return tbf::FolderIdFromAppId(appId);
}

static std::optional<std::wstring> FolderIdFromTaskBtnGroup(void* taskBtnGroup) {
    if (!taskBtnGroup || !g_taskBtnGroupGetGroup) {
        return std::nullopt;
    }
    using GetGroup_t = void*(WINAPI*)(void* pThis);
    return FolderIdFromTaskGroup(reinterpret_cast<GetGroup_t>(g_taskBtnGroupGetGroup)(taskBtnGroup));
}

static void OnFolderButtonClicked(const std::wstring& id) {
    Wh_Log(L"[CLICK] %s", id.c_str());
    PostToUi(WM_TBF_CLICK, std::make_unique<ButtonMessage>(ButtonMessage{id, ButtonRectFor(id)}));
}

using TaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis, void* taskGroup, void* taskItem,
                                                   const void* launcherOptions);
static TaskListWnd_HandleClick_t g_handleClickOriginal;

static HRESULT WINAPI TaskListWnd_HandleClick_Hook(void* pThis, void* taskGroup, void* taskItem,
                                                   const void* launcherOptions) {
    if (!g_unloading) {
        if (auto id = FolderIdFromTaskGroup(taskGroup)) {
            OnFolderButtonClicked(*id);
            return S_OK;  // ne jamais lancer systray.exe
        }
    }
    return g_handleClickOriginal(pThis, taskGroup, taskItem, launcherOptions);
}

using TaskListWnd_OnContextMenu_t = void(WINAPI*)(void* pThis, POINT point, HWND hwnd, bool keyboardInvoked,
                                                  void* taskGroup, void* taskItem);
static TaskListWnd_OnContextMenu_t g_onContextMenuOriginal;

// Ancien chemin du menu contextuel : on le coupe pour un dossier (le menu du
// dossier passe par les hooks XAML).
static void WINAPI TaskListWnd_OnContextMenu_Hook(void* pThis, POINT point, HWND hwnd, bool keyboardInvoked,
                                                  void* taskGroup, void* taskItem) {
    if (!g_unloading && FolderIdFromTaskGroup(taskGroup)) {
        return;
    }
    g_onContextMenuOriginal(pThis, point, hwnd, keyboardInvoked, taskGroup, taskItem);
}

using TaskBtnGroup_ShouldShowToolTip_t = int(WINAPI*)(void* pThis, void* taskItem);
static TaskBtnGroup_ShouldShowToolTip_t g_shouldShowToolTipOriginal;

static int WINAPI TaskBtnGroup_ShouldShowToolTip_Hook(void* pThis, void* taskItem) {
    if (!g_unloading && FolderIdFromTaskBtnGroup(pThis)) {
        return FALSE;
    }
    return g_shouldShowToolTipOriginal(pThis, taskItem);
}

using TaskGroup_GetToolTipText_t = HRESULT(WINAPI*)(void* pThis, void* taskItem, wchar_t* text, int cchText);
static TaskGroup_GetToolTipText_t g_getToolTipTextOriginal;

static HRESULT WINAPI TaskGroup_GetToolTipText_Hook(void* pThis, void* taskItem, wchar_t* text, int cchText) {
    if (!g_unloading && FolderIdFromTaskGroup(pThis)) {
        if (text && cchText > 0) {
            text[0] = L'\0';
        }
        return S_FALSE;
    }
    return g_getToolTipTextOriginal(pThis, taskItem, text, cchText);
}

static bool HookTaskbarDll(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"},
         &g_taskGroupGetAppId, nullptr},
        {{LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
         &g_handleClickOriginal, TaskListWnd_HandleClick_Hook},
        {{LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
         &g_taskBtnGroupGetGroup, nullptr, true},
        {{LR"(public: virtual void __cdecl CTaskListWnd::OnContextMenu(struct tagPOINT,struct HWND__ *,bool,struct ITaskGroup *,struct ITaskItem *))"},
         &g_onContextMenuOriginal, TaskListWnd_OnContextMenu_Hook, true},
        {{LR"(public: virtual int __cdecl CTaskBtnGroup::ShouldShowToolTip(struct ITaskItem *))"},
         &g_shouldShowToolTipOriginal, TaskBtnGroup_ShouldShowToolTip_Hook, true},
        {{LR"(public: virtual long __cdecl CTaskGroup::GetToolTipText(struct ITaskItem *,unsigned short *,int))"},
         &g_getToolTipTextOriginal, TaskGroup_GetToolTipText_Hook, true},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static bool HookTaskbarView(HMODULE module);  // section taskbar view hooks

static HMODULE GetTaskbarViewModule() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    return module ? module : GetModuleHandleW(L"ExplorerExtensions.dll");
}

static std::recursive_mutex g_hookMutex;
static std::atomic<bool> g_taskbarDllAttempted{false};
static std::atomic<bool> g_taskbarViewAttempted{false};
static thread_local bool t_installingHooks = false;

// Une seule tentative par module : en cas d'échec (symboles introuvables sur
// cette build), on journalise et on ne réessaie pas à chaque LoadLibrary.
static void TryHookLoadedModules(bool applyNow) {
    if (t_installingHooks) {
        return;  // HookSymbols charge lui-même des DLL
    }
    std::lock_guard lock(g_hookMutex);
    t_installingHooks = true;
    bool installed = false;
    if (!g_taskbarDllAttempted) {
        if (HMODULE module = GetModuleHandleW(L"taskbar.dll")) {
            g_taskbarDllAttempted = true;
            const bool ok = HookTaskbarDll(module);
            Wh_Log(L"[HOOK] taskbar.dll : %s", ok ? L"ok" : L"FAILED");
            installed |= ok;
        }
    }
    if (!g_taskbarViewAttempted) {
        if (HMODULE module = GetTaskbarViewModule()) {
            g_taskbarViewAttempted = true;
            const bool ok = HookTaskbarView(module);
            Wh_Log(L"[HOOK] Taskbar.View : %s", ok ? L"ok" : L"FAILED");
            installed |= ok;
        }
    }
    t_installingHooks = false;
    if (installed && applyNow) {
        Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_loadLibraryExWOriginal;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = g_loadLibraryExWOriginal(fileName, file, flags);
    if (module && !g_unloading && (!g_taskbarDllAttempted || !g_taskbarViewAttempted)) {
        TryHookLoadedModules(true);
    }
    return module;
}

static bool InstallTaskbarHooks() {
    TryHookLoadedModules(false);
    if (g_taskbarDllAttempted && g_taskbarViewAttempted) {
        return true;
    }
    auto target = reinterpret_cast<LoadLibraryExW_t>(
        GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "LoadLibraryExW"));
    if (!target || !WindhawkUtils::SetFunctionHook(target, LoadLibraryExW_Hook, &g_loadLibraryExWOriginal)) {
        Wh_Log(L"[HOOK] Can't watch LoadLibraryExW");
        return false;
    }
    return true;
}

// === SECTION: taskbar view hooks ===

namespace wux = winrt::Windows::UI::Xaml;

static void RevokeMenuCallbacksOnCurrentThread();  // section menus

// État d'un conteneur TaskListButton qui représente un dossier. Les conteneurs
// sont recyclés par la barre : tout ce que le mod y pose doit pouvoir être retiré.
struct FolderButton {
    winrt::weak_ref<wux::FrameworkElement> element;
    void* abi = nullptr;  // identité du conteneur (comparaison uniquement)
    DWORD threadId = 0;
    std::wstring folderId;
    winrt::event_token enteredToken{};
    winrt::weak_ref<wux::FrameworkElement> iconPanel;
    winrt::weak_ref<wux::Controls::Image> icon;
    wux::Media::ImageSource originalSource{nullptr};
    wux::Media::ImageSource appliedSource{nullptr};
    uint64_t appliedGeneration = 0;
    // Infobulle XAML d'origine (valeur locale, éventuellement « non définie »).
    winrt::Windows::Foundation::IInspectable originalButtonToolTip{nullptr};
    winrt::Windows::Foundation::IInspectable originalIconPanelToolTip{nullptr};
};

static std::mutex g_buttonsMutex;
static std::vector<std::shared_ptr<FolderButton>> g_buttons;

static wux::FrameworkElement GetTaskListButtonElement(void* pThis) {
    wux::FrameworkElement element = nullptr;
    // Même ajustement de sous-objet C++/WinRT que les autres mods de la barre.
    IUnknown* unknown = reinterpret_cast<IUnknown*>(pThis) + 3;
    if (FAILED(unknown->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(element)))) {
        return nullptr;
    }
    return element;
}

static std::optional<std::wstring> FolderIdFromElement(const wux::FrameworkElement& element) {
    if (!element) {
        return std::nullopt;
    }
    const winrt::hstring name = wux::Automation::AutomationProperties::GetName(element);
    return tbf::ExtractFolderIdFromText(std::wstring_view(name.c_str(), name.size()));
}

static wux::FrameworkElement FindChildByName(const wux::FrameworkElement& root, std::wstring_view name) {
    if (!root) {
        return nullptr;
    }
    const int count = wux::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wux::Media::VisualTreeHelper::GetChild(root, i).try_as<wux::FrameworkElement>();
        if (!child) {
            continue;
        }
        const winrt::hstring childName = child.Name();
        if (std::wstring_view(childName.c_str(), childName.size()) == name) {
            return child;
        }
        if (auto found = FindChildByName(child, name)) {
            return found;
        }
    }
    return nullptr;
}

static void RestoreLocalValue(const wux::DependencyObject& target, const wux::DependencyProperty& property,
                              const winrt::Windows::Foundation::IInspectable& value) {
    if (!value || value == wux::DependencyProperty::UnsetValue()) {
        target.ClearValue(property);
    } else {
        target.SetValue(property, value);
    }
}

// Rectangle écran (pixels physiques) de l'élément, calculé depuis la fenêtre
// hôte XAML de la barre sous le curseur ; repli sur un carré autour du curseur.
static tbf::Rect ElementScreenRect(const wux::FrameworkElement& element) {
    POINT cursor{};
    GetCursorPos(&cursor);
    HWND root = GetAncestor(WindowFromPoint(cursor), GA_ROOT);
    HWND bridge = root ? FindWindowExW(root, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr)
                       : nullptr;
    auto xamlRoot = element.XamlRoot();
    if (!bridge || !xamlRoot) {
        return CursorRect();
    }
    const double scale = xamlRoot.RasterizationScale();
    const auto origin = element.TransformToVisual(nullptr).TransformPoint({0, 0});
    POINT client{0, 0};
    ClientToScreen(bridge, &client);
    tbf::Rect rect;
    rect.left = client.x + static_cast<int>(std::lround(origin.X * scale));
    rect.top = client.y + static_cast<int>(std::lround(origin.Y * scale));
    rect.right = rect.left + static_cast<int>(std::lround(element.ActualWidth() * scale));
    rect.bottom = rect.top + static_cast<int>(std::lround(element.ActualHeight() * scale));
    // Garde-fou : si la géométrie calculée ne contient pas le curseur, elle est fausse.
    return rect.Contains(cursor.x, cursor.y) ? rect : CursorRect();
}

static wux::Media::ImageSource MakeImageSource(const tbf::Bgra& image) {
    wux::Media::Imaging::WriteableBitmap bitmap(image.width, image.height);
    auto buffer = bitmap.PixelBuffer();
    std::memcpy(buffer.data(), image.pixels.data(), image.pixels.size() * sizeof(uint32_t));
    bitmap.Invalidate();
    return bitmap;
}

// Thread du conteneur uniquement.
static void ApplyFolderVisuals(FolderButton& state) {
    auto image = state.icon.get();
    auto grid = FindMiniGrid(state.folderId);
    if (!image || !grid || grid->image.Empty()) {
        return;
    }
    const auto current = image.Source();
    const bool ours = state.appliedSource && current == state.appliedSource;
    if (!ours && current) {
        state.originalSource = current;  // la barre a posé une nouvelle icône
    }
    if (ours && grid->generation == state.appliedGeneration) {
        return;
    }
    auto source = MakeImageSource(grid->image);
    image.Source(source);
    state.appliedSource = source;
    state.appliedGeneration = grid->generation;
    if (auto element = state.element.get()) {
        if (auto folder = FindFolder(state.folderId)) {
            wux::Automation::AutomationProperties::SetHelpText(element, folder->name);
        }
    }
}

// Thread du conteneur uniquement.
static void ReleaseFolderButton(FolderButton& state) {
    auto element = state.element.get();
    if (element && state.enteredToken) {
        element.PointerEntered(state.enteredToken);
    }
    if (auto image = state.icon.get()) {
        if (state.appliedSource && image.Source() == state.appliedSource) {
            image.Source(state.originalSource);
        }
    }
    const auto toolTipProperty = wux::Controls::ToolTipService::ToolTipProperty();
    if (element) {
        RestoreLocalValue(element, toolTipProperty, state.originalButtonToolTip);
        wux::Automation::AutomationProperties::SetHelpText(element, L"");
    }
    if (auto iconPanel = state.iconPanel.get()) {
        RestoreLocalValue(iconPanel, toolTipProperty, state.originalIconPanelToolTip);
    }
    state.enteredToken = {};
    state.appliedSource = nullptr;
    state.appliedGeneration = 0;
}

static std::shared_ptr<FolderButton> AttachFolderButton(const wux::FrameworkElement& element,
                                                        const std::wstring& folderId) {
    auto state = std::make_shared<FolderButton>();
    state->element = winrt::make_weak(element);
    state->abi = winrt::get_abi(element);
    state->threadId = GetCurrentThreadId();
    state->folderId = folderId;

    std::weak_ptr<FolderButton> weak = state;
    state->enteredToken = element.PointerEntered(
        [weak](winrt::Windows::Foundation::IInspectable const& sender, wux::Input::PointerRoutedEventArgs const&) {
            try {
                auto self = weak.lock();
                if (!self || g_unloading) {
                    return;
                }
                auto fe = sender.try_as<wux::FrameworkElement>();
                const tbf::Rect rect = fe ? ElementScreenRect(fe) : CursorRect();
                RememberButtonRect(self->folderId, rect);
                PostToUi(WM_TBF_HOVER, std::make_unique<ButtonMessage>(ButtonMessage{self->folderId, rect}));
            } catch (...) {
            }
        });

    // L'infobulle du bouton de Windows 11 est portée par le ToolTipService XAML
    // du bouton et de son IconPanel : on la coupe sur les deux.
    const auto toolTipProperty = wux::Controls::ToolTipService::ToolTipProperty();
    const winrt::Windows::Foundation::IInspectable noToolTip{nullptr};
    state->originalButtonToolTip = element.ReadLocalValue(toolTipProperty);
    wux::Controls::ToolTipService::SetToolTip(element, noToolTip);

    if (auto iconPanel = FindChildByName(element, L"IconPanel")) {
        state->iconPanel = winrt::make_weak(iconPanel);
        state->originalIconPanelToolTip = iconPanel.ReadLocalValue(toolTipProperty);
        wux::Controls::ToolTipService::SetToolTip(iconPanel, noToolTip);
        if (auto image = FindChildByName(iconPanel, L"Icon").try_as<wux::Controls::Image>()) {
            state->icon = winrt::make_weak(image);
            state->originalSource = image.Source();
        }
    }
    if (!state->icon.get()) {
        Wh_Log(L"[XAML] Image#Icon not found for %s: keeping the static icon", folderId.c_str());
    }

    std::lock_guard lock(g_buttonsMutex);
    g_buttons.push_back(state);
    return state;
}

static void SyncFolderButton(const wux::FrameworkElement& element) {
    if (!element) {
        return;
    }
    const auto folderId = FolderIdFromElement(element);
    void* abi = winrt::get_abi(element);

    std::shared_ptr<FolderButton> state;
    {
        std::lock_guard lock(g_buttonsMutex);
        std::erase_if(g_buttons, [](const auto& b) { return !b->element.get(); });
        for (const auto& b : g_buttons) {
            if (b->abi == abi) {
                state = b;
                break;
            }
        }
    }

    if (state && (!folderId || state->folderId != *folderId)) {
        ReleaseFolderButton(*state);
        std::lock_guard lock(g_buttonsMutex);
        std::erase(g_buttons, state);
        state.reset();
    }
    if (!folderId) {
        return;
    }
    if (!state) {
        state = AttachFolderButton(element, *folderId);
    }
    ApplyFolderVisuals(*state);
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
static TaskListButton_UpdateVisualStates_t g_updateVisualStatesOriginal;
static thread_local bool t_inUpdateVisualStates = false;

static void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    g_updateVisualStatesOriginal(pThis);
    if (g_unloading || t_inUpdateVisualStates) {
        return;  // nos propres changements de propriétés peuvent ré-entrer ici
    }
    t_inUpdateVisualStates = true;
    try {
        SyncFolderButton(GetTaskListButtonElement(pThis));
    } catch (...) {
        Wh_Log(L"[XAML] SyncFolderButton failed");
    }
    t_inUpdateVisualStates = false;
}

static void WINAPI ApplyVisualsOnCurrentThread(void*) {
    std::vector<std::shared_ptr<FolderButton>> mine;
    {
        std::lock_guard lock(g_buttonsMutex);
        for (const auto& b : g_buttons) {
            if (b->threadId == GetCurrentThreadId()) {
                mine.push_back(b);
            }
        }
    }
    for (const auto& b : mine) {
        try {
            ApplyFolderVisuals(*b);
        } catch (...) {
        }
    }
}

static void WINAPI RestoreOnCurrentThread(void*) {
    std::vector<std::shared_ptr<FolderButton>> mine;
    {
        std::lock_guard lock(g_buttonsMutex);
        for (auto it = g_buttons.begin(); it != g_buttons.end();) {
            if ((*it)->threadId == GetCurrentThreadId()) {
                mine.push_back(*it);
                it = g_buttons.erase(it);
            } else {
                ++it;
            }
        }
    }
    for (const auto& b : mine) {
        try {
            ReleaseFolderButton(*b);
        } catch (...) {
        }
    }
    RevokeMenuCallbacksOnCurrentThread();
}

// Thread UI du mod : appliquer la nouvelle mini-grille sur chaque thread de barre.
static void RefreshTaskbarButtons() {
    for (HWND window : TaskbarWindows()) {
        RunFromWindowThread(window, ApplyVisualsOnCurrentThread, nullptr);
    }
}

// Wh_ModUninit : retirer tout ce que le mod a posé, sur le thread de chaque barre,
// avant que Windhawk ne décharge notre code.
static void RestoreTaskbarButtons() {
    for (HWND window : TaskbarWindows()) {
        RunFromWindowThread(window, RestoreOnCurrentThread, nullptr);
    }
}

// Hooks des menus : définis dans la section `menus`, déclarés ici (unique
// déclaration de leurs types et pointeurs d'origine) car HookTaskbarView les référence.
using TaskListButton_OnContextRequested_t = void(WINAPI*)(void* pThis, wux::UIElement const& sender,
                                                          wux::Input::ContextRequestedEventArgs const& args);
using TaskListButtonHandlers_HandleContextRequested_t = void(WINAPI*)(wux::UIElement const& sender,
                                                                      wux::Input::ContextRequestedEventArgs const& args);
using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(void* pThis, wux::UIElement const& sender, wux::Input::ContextRequestedEventArgs const& args);
using MenuFlyoutItemBaseVector_Append_t = void(__cdecl*)(void* pThis,
                                                         wux::Controls::MenuFlyoutItemBase const& item);
using ContextMenus_ShowTaskbarSettingsContextMenu_t =
    void(__cdecl*)(wux::FrameworkElement const& target, void* taskbarSettings,
                   wux::Input::ContextRequestedEventArgs const& args, unsigned long long options);

static TaskListButton_OnContextRequested_t g_onContextRequestedOriginal;
static TaskListButtonHandlers_HandleContextRequested_t g_handleContextRequestedOriginal;
static TaskbarResources_OnTaskListButtonContextRequested_t g_resourcesContextRequestedOriginal;
static MenuFlyoutItemBaseVector_Append_t g_menuAppendOriginal;
static ContextMenus_ShowTaskbarSettingsContextMenu_t g_showSettingsMenuOriginal;

static void WINAPI TaskListButton_OnContextRequested_Hook(void*, wux::UIElement const&,
                                                          wux::Input::ContextRequestedEventArgs const&);
static void WINAPI TaskListButtonHandlers_HandleContextRequested_Hook(wux::UIElement const&,
                                                                      wux::Input::ContextRequestedEventArgs const&);
static void WINAPI TaskbarResources_OnTaskListButtonContextRequested_Hook(void*, wux::UIElement const&,
                                                                          wux::Input::ContextRequestedEventArgs const&);
static void __cdecl MenuFlyoutItemBaseVector_Append_Hook(void*, wux::Controls::MenuFlyoutItemBase const&);
static void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(wux::FrameworkElement const&, void*,
                                                                     wux::Input::ContextRequestedEventArgs const&,
                                                                     unsigned long long);

static bool HookTaskbarView(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
         &g_updateVisualStatesOriginal, TaskListButton_UpdateVisualStates_Hook},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &g_onContextRequestedOriginal, TaskListButton_OnContextRequested_Hook, true},
        {{LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &g_handleContextRequestedOriginal, TaskListButtonHandlers_HandleContextRequested_Hook, true},
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &g_resourcesContextRequestedOriginal, TaskbarResources_OnTaskListButtonContextRequested_Hook, true},
        {{LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))"},
         &g_showSettingsMenuOriginal, ContextMenus_ShowTaskbarSettingsContextMenu_Hook, true},
        {{LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )"},
         &g_menuAppendOriginal, MenuFlyoutItemBaseVector_Append_Hook, true},
    };
    const bool ok = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
    Wh_Log(L"[HOOK] button menu=%d/%d/%d, taskbar menu=%d", g_onContextRequestedOriginal != nullptr,
           g_handleContextRequestedOriginal != nullptr, g_resourcesContextRequestedOriginal != nullptr,
           g_showSettingsMenuOriginal != nullptr && g_menuAppendOriginal != nullptr);
    return ok;
}

// === SECTION: popup ===

constexpr wchar_t kPopupClass[] = L"TaskbarFolders.Popup";
constexpr UINT_PTR kPopupTickTimer = 2;
constexpr UINT kPopupTickMs = 50;
constexpr int kPopupAddButton = -2;  // index de survol du « + » (les cases sont >= 0)
// Écart (px logiques) entre la fenêtre et la barre des tâches / les bords de l'écran.
constexpr int kPopupGap = 16;

// Attributs DWM définis ici : les en-têtes mingw peuvent ne pas les avoir.
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmSystemBackdropType = 38;
constexpr int kDwmCornerRound = 2;
constexpr int kDwmBackdropTransientWindow = 3;

struct PopupItem {
    tbf::FolderApp app;
    tbf::Bgra icon;
    bool broken = false;
};

// Thread UI du mod uniquement.
struct PopupState {
    HWND hwnd = nullptr;
    tbf::PopupStateMachine machine;
    tbf::Rect button{};
    std::wstring shownFolderId;
    std::wstring title;
    std::vector<PopupItem> items;
    tbf::GridLayout layout;
    tbf::Rect addRect{};  // petit « + » de la ligne de titre
    tbf::GridMetrics metrics;
    // Réorganisation par glisser dans la fenêtre.
    int pressIndex = -1;  // case sous le bouton gauche enfoncé
    POINT pressPoint{};
    bool dragging = false;
    int dragFrom = -1;
    int dragTo = -1;
    POINT dragPoint{};
    double scale = 1.0;
    int iconPx = 32;
    int columns = 4;  // colonnes effectives (réglage, élargi si l'écran est trop bas)
    int hot = -1;
    int selected = -1;
    bool dark = true;
    float tintAlpha = 1.0f;  // opacité de la teinte peinte par Direct2D
    bool menuOpen = false;
    bool trackingMouse = false;
    winrt::com_ptr<ID2D1Factory> d2d;
    winrt::com_ptr<IDWriteFactory> dwrite;
    winrt::com_ptr<ID2D1HwndRenderTarget> target;
    std::vector<winrt::com_ptr<ID2D1Bitmap>> bitmaps;
    winrt::com_ptr<IDWriteTextFormat> titleFormat;
    winrt::com_ptr<IDWriteTextFormat> labelFormat;
    winrt::com_ptr<IDWriteTextFormat> messageFormat;
    double formatsScale = 0;
};

static PopupState* g_popup = nullptr;

static void Popup_ShowAppMenu(int index, POINT screen);  // section menus

static bool IsShortcutBroken(const std::wstring& lnk) {
    winrt::com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(link.put())))) {
        return false;
    }
    winrt::com_ptr<IPersistFile> file;
    if (FAILED(link->QueryInterface(IID_PPV_ARGS(file.put()))) || FAILED(file->Load(lnk.c_str(), STGM_READ))) {
        return true;
    }
    wchar_t target[MAX_PATH] = {};
    if (link->GetPath(target, MAX_PATH, nullptr, SLGP_RAWPATH) != S_OK || !target[0]) {
        return false;  // cible non fichier (appli du Store, URL…) : on ne juge pas
    }
    wchar_t expanded[MAX_PATH] = {};
    ExpandEnvironmentStringsW(target, expanded, MAX_PATH);
    return !tbf::PathExists(expanded);
}

// API non documentée de user32, celle qu'utilise translucent-windows pour son
// effet « Flou ». Elle n'est pas interceptée par ce mod, contrairement à
// DWMWA_SYSTEMBACKDROP_TYPE qu'il force à DWMSBT_AUTO (aucun fond).
struct AccentPolicy {
    int accentState;
    int accentFlags;
    DWORD gradientColor;  // 0xAABBGGRR
    int animationId;
};

struct WindowCompositionAttributeData {
    DWORD attribute;
    PVOID data;
    SIZE_T size;
};

constexpr DWORD kWcaAccentPolicy = 19;
constexpr int kAccentDisabled = 0;
constexpr int kAccentAcrylicBlurBehind = 4;

static void SetAccent(HWND hwnd, int state, DWORD gradientColor) {
    using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);
    static const auto setAttribute = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));
    if (!setAttribute) {
        return;
    }
    AccentPolicy policy{state, 0, gradientColor, 0};
    WindowCompositionAttributeData data{kWcaAccentPolicy, &policy, sizeof(policy)};
    setAttribute(hwnd, &data);
}

// Applique l'effet de fond choisi (réglage « Effet de fond ») et renvoie
// l'opacité de la teinte que Direct2D doit peindre par-dessus.
static float ApplyBackdrop(HWND hwnd, bool dark) {
    const ModSettings settings = GetSettings();
    BOOL darkMode = dark;
    DwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode, &darkMode, sizeof(darkMode));
    int corner = kDwmCornerRound;
    DwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference, &corner, sizeof(corner));

    // Repartir d'un état neutre : les réglages peuvent changer à chaud.
    SetAccent(hwnd, kAccentDisabled, 0);
    DWM_BLURBEHIND blurOff{};
    blurOff.dwFlags = DWM_BB_ENABLE;
    blurOff.fEnable = FALSE;
    DwmEnableBlurBehindWindow(hwnd, &blurOff);
    MARGINS noMargins{0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hwnd, &noMargins);

    switch (settings.effect) {
        case PopupEffect::Blur: {
            // Le canal alpha de la zone client doit être pris en compte par DWM.
            HRGN region = CreateRectRgn(0, 0, -1, -1);
            DWM_BLURBEHIND blur{};
            blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
            blur.fEnable = TRUE;
            blur.hRgnBlur = region;
            DwmEnableBlurBehindWindow(hwnd, &blur);
            DeleteObject(region);
            SetAccent(hwnd, kAccentAcrylicBlurBehind, tbf::ArgbToAbgr(settings.tintColor));
            return 0.0f;  // la teinte est déjà dans le flou
        }
        case PopupEffect::Acrylic: {
            int backdrop = kDwmBackdropTransientWindow;
            if (SUCCEEDED(DwmSetWindowAttribute(hwnd, kDwmSystemBackdropType, &backdrop, sizeof(backdrop)))) {
                MARGINS margins{-1, -1, -1, -1};
                DwmExtendFrameIntoClientArea(hwnd, &margins);
                return dark ? 0.55f : 0.6f;
            }
            return 1.0f;
        }
        case PopupEffect::Opaque:
            return 1.0f;
    }
    return 1.0f;
}

static void Popup_ApplyDwm() {
    g_popup->tintAlpha = ApplyBackdrop(g_popup->hwnd, g_popup->dark);
}

static void Popup_EnsureFormats() {
    auto& p = *g_popup;
    if (p.formatsScale == p.scale && p.titleFormat) {
        return;
    }
    p.titleFormat = nullptr;
    p.labelFormat = nullptr;
    p.messageFormat = nullptr;
    const auto size = [&](float px) { return static_cast<float>(px * p.scale); };
    p.dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                               DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size(15), L"fr-fr",
                               p.titleFormat.put());
    p.dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                               DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size(12), L"fr-fr",
                               p.labelFormat.put());
    p.dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                               DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size(12), L"fr-fr",
                               p.messageFormat.put());
    if (!p.titleFormat || !p.labelFormat || !p.messageFormat) {
        return;
    }
    p.titleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    p.titleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    p.labelFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    p.labelFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    p.messageFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    p.messageFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    winrt::com_ptr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(p.dwrite->CreateEllipsisTrimmingSign(p.labelFormat.get(), ellipsis.put()))) {
        p.labelFormat->SetTrimming(&trimming, ellipsis.get());
        p.titleFormat->SetTrimming(&trimming, ellipsis.get());
    }
    p.formatsScale = p.scale;
}

static void Popup_EnsureTarget() {
    auto& p = *g_popup;
    if (p.target) {
        return;
    }
    RECT client{};
    GetClientRect(p.hwnd, &client);
    const auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f);
    const auto hwndProps =
        D2D1::HwndRenderTargetProperties(p.hwnd, D2D1::SizeU(client.right - client.left, client.bottom - client.top));
    if (FAILED(p.d2d->CreateHwndRenderTarget(props, hwndProps, p.target.put()))) {
        p.target = nullptr;
    }
    p.bitmaps.clear();
}

static void Popup_EnsureBitmaps() {
    auto& p = *g_popup;
    if (!p.target || p.bitmaps.size() == p.items.size()) {
        return;
    }
    p.bitmaps.clear();
    const auto props =
        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    for (const auto& item : p.items) {
        winrt::com_ptr<ID2D1Bitmap> bitmap;
        if (!item.icon.Empty()) {
            p.target->CreateBitmap(D2D1::SizeU(item.icon.width, item.icon.height), item.icon.pixels.data(),
                                   item.icon.width * 4, props, bitmap.put());
        }
        p.bitmaps.push_back(bitmap);
    }
}

static D2D1_RECT_F ToRectF(const tbf::Rect& r) {
    return D2D1::RectF(static_cast<float>(r.left), static_cast<float>(r.top), static_cast<float>(r.right),
                       static_cast<float>(r.bottom));
}

static void Popup_Paint() {
    auto& p = *g_popup;
    Popup_EnsureTarget();
    Popup_EnsureFormats();
    Popup_EnsureBitmaps();
    if (!p.target || !p.labelFormat) {
        return;
    }
    auto& t = *p.target;
    const float s = static_cast<float>(p.scale);
    const auto text = p.dark ? D2D1::ColorF(1, 1, 1, 0.95f) : D2D1::ColorF(0, 0, 0, 0.9f);
    const auto tint = p.dark ? D2D1::ColorF(0.13f, 0.13f, 0.13f, p.tintAlpha)
                             : D2D1::ColorF(0.95f, 0.95f, 0.95f, p.tintAlpha);
    const auto hot = p.dark ? D2D1::ColorF(1, 1, 1, 0.08f) : D2D1::ColorF(0, 0, 0, 0.05f);
    const auto focus = p.dark ? D2D1::ColorF(1, 1, 1, 0.45f) : D2D1::ColorF(0, 0, 0, 0.45f);

    winrt::com_ptr<ID2D1SolidColorBrush> textBrush, tintBrush, hotBrush, focusBrush;
    t.CreateSolidColorBrush(text, textBrush.put());
    t.CreateSolidColorBrush(tint, tintBrush.put());
    t.CreateSolidColorBrush(hot, hotBrush.put());
    t.CreateSolidColorBrush(focus, focusBrush.put());

    t.BeginDraw();
    t.Clear(D2D1::ColorF(0, 0, 0, 0));
    t.FillRectangle(D2D1::RectF(0, 0, static_cast<float>(p.layout.window.width),
                                static_cast<float>(p.layout.window.height)),
                    tintBrush.get());

    auto titleRect = ToRectF(p.layout.title);
    titleRect.left += 4 * s;
    titleRect.right = static_cast<float>(p.addRect.left) - 4 * s;
    t.DrawText(p.title.c_str(), static_cast<UINT32>(p.title.size()), p.titleFormat.get(), titleRect,
               textBrush.get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);


    // Pendant un glisser, les autres applis s'écartent pour montrer la place visée.
    std::vector<size_t> order(p.items.size());
    std::iota(order.begin(), order.end(), size_t{0});
    if (p.dragging && p.dragFrom >= 0 && p.dragTo >= 0) {
        order = tbf::MoveItem(order, static_cast<size_t>(p.dragFrom), static_cast<size_t>(p.dragTo));
    }
    for (size_t k = 0; k < order.size() && k < p.layout.cells.size(); ++k) {
        const size_t i = order[k];
        const auto& item = p.items[i];
        const tbf::Rect& cell = p.layout.cells[k];
        const auto cellRect = D2D1::RoundedRect(
            D2D1::RectF(cell.left + 2 * s, cell.top + 2 * s, cell.right - 2 * s, cell.bottom - 2 * s), 6 * s, 6 * s);
        if (p.dragging && static_cast<int>(i) == p.dragFrom) {
            t.DrawRoundedRectangle(cellRect, focusBrush.get(), 1.0f * s);  // place où l'appli va tomber
            continue;
        }
        if (!p.dragging && (static_cast<int>(i) == p.hot || static_cast<int>(i) == p.selected)) {
            t.FillRoundedRectangle(cellRect, hotBrush.get());
        }
        if (!p.dragging && static_cast<int>(i) == p.selected) {
            t.DrawRoundedRectangle(cellRect, focusBrush.get(), 1.5f * s);
        }
        const float iconLeft = cell.left + (cell.Width() - p.iconPx) / 2.0f;
        const float iconTop = cell.top + 8 * s;
        if (i < p.bitmaps.size() && p.bitmaps[i]) {
            t.DrawBitmap(p.bitmaps[i].get(),
                         D2D1::RectF(iconLeft, iconTop, iconLeft + p.iconPx, iconTop + p.iconPx),
                         item.broken ? 0.4f : 1.0f);
        }
        const std::wstring& label = item.broken ? std::wstring(Tr(L"Target not found", L"Cible introuvable")) : item.app.name;
        textBrush->SetOpacity(item.broken ? 0.6f : 1.0f);
        t.DrawText(label.c_str(), static_cast<UINT32>(label.size()), p.labelFormat.get(),
                   D2D1::RectF(cell.left + 4 * s, iconTop + p.iconPx + 4 * s, cell.right - 4 * s, cell.bottom - 2 * s),
                   textBrush.get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush->SetOpacity(1.0f);
    }

    // Icône glissée, sous le curseur.
    if (p.dragging && p.dragFrom >= 0 && p.dragFrom < static_cast<int>(p.bitmaps.size()) && p.bitmaps[p.dragFrom]) {
        const float half = p.iconPx / 2.0f;
        const float x = static_cast<float>(p.dragPoint.x);
        const float y = static_cast<float>(p.dragPoint.y);
        t.DrawBitmap(p.bitmaps[p.dragFrom].get(), D2D1::RectF(x - half, y - half, x + half, y + half), 0.85f);
    }

    // Petit « + » dans le coin supérieur droit : ajouter des applis.
    {
        const auto addRect = ToRectF(p.addRect);
        if (p.hot == kPopupAddButton) {
            t.FillRoundedRectangle(D2D1::RoundedRect(addRect, 4 * s, 4 * s), hotBrush.get());
        }
        const D2D1_POINT_2F center{(addRect.left + addRect.right) / 2, (addRect.top + addRect.bottom) / 2};
        const float arm = 5 * s;
        t.DrawLine({center.x - arm, center.y}, {center.x + arm, center.y}, textBrush.get(), 1.5f * s);
        t.DrawLine({center.x, center.y - arm}, {center.x, center.y + arm}, textBrush.get(), 1.5f * s);
    }

    if (p.items.empty()) {
        const std::wstring message =
            Tr(L"Empty folder — click + to add apps", L"Dossier vide — cliquez sur + pour ajouter des applis");
        const auto area = D2D1::RectF(static_cast<float>(p.layout.title.left), static_cast<float>(p.layout.title.bottom),
                                      static_cast<float>(p.layout.title.right),
                                      static_cast<float>(p.layout.window.height) - 12 * s);
        textBrush->SetOpacity(0.7f);
        t.DrawText(message.c_str(), static_cast<UINT32>(message.size()), p.messageFormat.get(), area,
                   textBrush.get());
        textBrush->SetOpacity(1.0f);
    }

    if (t.EndDraw() == static_cast<HRESULT>(D2DERR_RECREATE_TARGET)) {
        p.target = nullptr;
        p.bitmaps.clear();
    }
}

static void Popup_EnsureTick() {
    SetTimer(g_popup->hwnd, kPopupTickTimer, kPopupTickMs, nullptr);
}

static void Popup_Hide() {
    auto& p = *g_popup;
    ShowWindow(p.hwnd, SW_HIDE);
    p.shownFolderId.clear();
    p.items.clear();
    p.bitmaps.clear();
    p.hot = -1;
    p.selected = -1;
    if (p.machine.PendingFolderId().empty()) {
        KillTimer(p.hwnd, kPopupTickTimer);
    }
}

static void Popup_ShowCurrent() {
    auto& p = *g_popup;
    auto folder = FindFolder(p.machine.FolderId());
    if (!folder) {
        p.machine.OnFolderRemoved(p.machine.FolderId());
        Popup_Hide();
        return;
    }
    const ModSettings settings = GetSettings();

    const RECT buttonRect{p.button.left, p.button.top, p.button.right, p.button.bottom};
    HMONITOR monitor = MonitorFromRect(&buttonRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(monitor, &info);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    p.scale = dpiX / 96.0;
    p.iconPx = tbf::Scaled(settings.iconSize, p.scale);

    if (p.shownFolderId != folder->id || p.items.size() != folder->apps.size() || p.title != folder->name) {
        p.items.clear();
        p.bitmaps.clear();
        for (const auto& app : folder->apps) {
            PopupItem item;
            item.app = app;
            item.icon = LoadShellIcon(app.path, p.iconPx);
            item.broken = IsShortcutBroken(app.path);
            p.items.push_back(std::move(item));
        }
        p.hot = -1;
    }
    p.title = folder->name;
    p.shownFolderId = folder->id;
    tbf::GridMetrics metrics = tbf::MakeGridMetrics(settings.iconSize, settings.columns, p.scale, settings.spacing);
    const int gap = tbf::Scaled(kPopupGap, p.scale);
    const size_t cellCount = p.items.size();
    metrics.columns = tbf::BalancedColumns(cellCount, settings.columns);
    metrics.columns = tbf::FitColumns(cellCount, metrics, (info.rcWork.right - info.rcWork.left) - 2 * gap,
                                      (info.rcWork.bottom - info.rcWork.top) - 2 * gap);
    p.columns = metrics.columns;
    p.layout = tbf::ComputeGridLayout(cellCount, metrics);
    p.metrics = metrics;
    p.addRect = tbf::TitleButtonRect(p.layout.title, tbf::Scaled(24, p.scale));

    const bool pinned = p.machine.Mode() == tbf::PopupMode::Pinned;
    if (pinned && p.selected < 0 && !p.items.empty()) {
        p.selected = 0;
    }
    if (p.selected >= static_cast<int>(p.items.size())) {
        p.selected = p.items.empty() ? -1 : 0;
    }

    const tbf::Rect monitorRect{info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right, info.rcMonitor.bottom};
    const tbf::Rect workRect{info.rcWork.left, info.rcWork.top, info.rcWork.right, info.rcWork.bottom};
    const tbf::Rect rect = tbf::ComputePopupRect(p.button, p.layout.window, tbf::DetectEdge(p.button, monitorRect),
                                                 workRect, gap);

    LONG_PTR exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | (pinned ? 0 : WS_EX_NOACTIVATE);
    SetWindowLongPtrW(p.hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(p.hwnd, HWND_TOPMOST, rect.left, rect.top, rect.Width(), rect.Height(),
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED | (pinned ? 0 : SWP_NOACTIVATE));
    if (p.target) {
        p.target->Resize(D2D1::SizeU(rect.Width(), rect.Height()));
    }
    if (pinned) {
        SetForegroundWindow(p.hwnd);
        SetFocus(p.hwnd);
        if (GetForegroundWindow() != p.hwnd) {
            Wh_Log(L"[POPUP] Activation refused: Esc and clicking outside won't close the window");
        }
    }
    InvalidateRect(p.hwnd, nullptr, FALSE);
    Popup_EnsureTick();
}

static void Popup_Apply(tbf::PopupAction action) {
    switch (action) {
        case tbf::PopupAction::None:
            return;
        case tbf::PopupAction::Close:
            Popup_Hide();
            return;
        case tbf::PopupAction::Show:
            Popup_ShowCurrent();
            return;
    }
}

static void Popup_LaunchPath(const std::wstring& path) {
    AllowSetForegroundWindow(ASFW_ANY);
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"[LAUNCH] %s: error %u", path.c_str(), GetLastError());
    }
}

static void OpenAppPicker(const std::wstring& folderId);  // section picker

static void Popup_Launch(int index) {
    auto& p = *g_popup;
    if (index < 0 || index >= static_cast<int>(p.items.size()) || p.items[index].broken) {
        return;
    }
    const std::wstring path = p.items[index].app.path;
    Popup_Apply(p.machine.OnAppLaunched());  // masquer avant que l'appli ne s'affiche
    Popup_LaunchPath(path);
}

static int Popup_HitTest(int x, int y) {
    if (g_popup->addRect.Contains(x, y)) {
        return kPopupAddButton;
    }
    return tbf::HitTestCell(g_popup->layout, x, y);
}

static void Popup_CancelDrag() {
    auto& p = *g_popup;
    const bool wasDragging = p.dragging;
    p.dragging = false;
    p.pressIndex = -1;
    p.dragFrom = -1;
    p.dragTo = -1;
    if (GetCapture() == p.hwnd) {
        ReleaseCapture();
    }
    if (wasDragging) {
        InvalidateRect(p.hwnd, nullptr, FALSE);
    }
}

// Déplace l'appli `from` à la place `to` et enregistre le nouvel ordre.
static void Popup_ReorderTo(int from, int to) {
    auto& p = *g_popup;
    if (from < 0 || to < 0 || from == to || from >= static_cast<int>(p.items.size())) {
        return;
    }
    std::vector<std::wstring> names;
    for (const auto& item : p.items) {
        names.push_back(item.app.fileName);
    }
    names = tbf::MoveItem(names, static_cast<size_t>(from), static_cast<size_t>(to));
    if (g_store->SetOrder(p.shownFolderId, names)) {
        ReloadSnapshot();  // la fenêtre et la mini-grille suivent le nouvel ordre
    } else {
        Wh_Log(L"[ORDER] Can't save the order of %s", p.shownFolderId.c_str());
    }
}

static bool CursorInsidePopupOrButton() {
    auto& p = *g_popup;
    POINT cursor{};
    GetCursorPos(&cursor);
    if (p.button.Contains(cursor.x, cursor.y)) {
        return true;
    }
    if (!IsWindowVisible(p.hwnd)) {
        return false;
    }
    RECT window{};
    GetWindowRect(p.hwnd, &window);
    return PtInRect(&window, cursor) != FALSE;
}

static LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (!g_popup) {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    auto& p = *g_popup;
    switch (message) {
        case WM_TIMER:
            if (wParam == kPopupTickTimer) {
                Popup_Apply(p.machine.OnTick(CursorInsidePopupOrButton(), Now()));
                if (p.machine.Mode() == tbf::PopupMode::Closed && p.machine.PendingFolderId().empty()) {
                    KillTimer(hwnd, kPopupTickTimer);
                }
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return p.machine.Mode() == tbf::PopupMode::Pinned ? MA_ACTIVATE : MA_NOACTIVATE;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && !p.menuOpen) {
                Popup_Apply(p.machine.OnDeactivated(Now()));
            }
            return 0;
        case WM_LBUTTONDOWN: {
            const int index = Popup_HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            if (index >= 0) {
                p.pressIndex = index;
                p.pressPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                SetCapture(hwnd);
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            if (reinterpret_cast<HWND>(lParam) != hwnd && (p.dragging || p.pressIndex >= 0)) {
                p.dragging = false;  // capture perdue : glisser annulé
                p.pressIndex = -1;
                p.dragFrom = -1;
                p.dragTo = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSEMOVE: {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            if (p.pressIndex >= 0 && (wParam & MK_LBUTTON)) {
                if (!p.dragging && (std::abs(x - p.pressPoint.x) > GetSystemMetrics(SM_CXDRAG) ||
                                    std::abs(y - p.pressPoint.y) > GetSystemMetrics(SM_CYDRAG))) {
                    p.dragging = true;
                    p.dragFrom = p.pressIndex;
                    p.dragTo = p.pressIndex;
                    Popup_Apply(p.machine.Pin());  // la fenêtre reste ouverte pendant le glisser
                }
                if (p.dragging) {
                    p.dragPoint = {x, y};
                    p.dragTo = std::max(0, tbf::DropIndexAt(p.layout, p.metrics, x, y));
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }
            if (!p.trackingMouse) {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
                p.trackingMouse = TrackMouseEvent(&track) != FALSE;
            }
            const int hot = Popup_HitTest(x, y);
            if (hot != p.hot) {
                p.hot = hot;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            p.trackingMouse = false;
            if (p.hot != -1) {
                p.hot = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_LBUTTONUP: {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            if (p.dragging) {
                RECT client{};
                GetClientRect(hwnd, &client);
                const POINT point{x, y};
                const int from = p.dragFrom;
                const int to = p.dragTo;
                Popup_CancelDrag();
                if (PtInRect(&client, point)) {  // lâcher hors de la fenêtre : annulé
                    Popup_ReorderTo(from, to);
                }
                return 0;
            }
            const int pressed = p.pressIndex;
            Popup_CancelDrag();
            const int index = Popup_HitTest(x, y);
            if (index == kPopupAddButton) {
                const std::wstring folderId = p.shownFolderId;
                Popup_Apply(p.machine.OnEscape());
                OpenAppPicker(folderId);
            } else if (index >= 0 && index == pressed) {
                Popup_Launch(index);
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            const int index = tbf::HitTestCell(p.layout, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            POINT screen{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ClientToScreen(hwnd, &screen);
            Popup_ShowAppMenu(index, screen);
            return 0;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE && p.dragging) {
                Popup_CancelDrag();
                return 0;
            }
            const bool control = GetKeyState(VK_CONTROL) < 0;
            int next = p.selected;
            switch (wParam) {
                case VK_ESCAPE:
                    Popup_Apply(p.machine.OnEscape());
                    return 0;
                case VK_RETURN:
                    Popup_Launch(p.selected);
                    return 0;
                case VK_LEFT:
                    next = tbf::MoveSelection(p.selected, p.items.size(), p.columns, tbf::NavKey::Left);
                    break;
                case VK_RIGHT:
                    next = tbf::MoveSelection(p.selected, p.items.size(), p.columns, tbf::NavKey::Right);
                    break;
                case VK_UP:
                    next = tbf::MoveSelection(p.selected, p.items.size(), p.columns, tbf::NavKey::Up);
                    break;
                case VK_DOWN:
                    next = tbf::MoveSelection(p.selected, p.items.size(), p.columns, tbf::NavKey::Down);
                    break;
                default:
                    return 0;
            }
            if (next != p.selected) {
                const int from = p.selected;
                p.selected = next;
                if (control && from >= 0) {
                    Popup_ReorderTo(from, next);  // Ctrl + flèche : l'appli se déplace avec la sélection
                } else {
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        case WM_PAINT:
            Popup_Paint();
            ValidateRect(hwnd, nullptr);
            return 0;
        case WM_SIZE:
            if (p.target) {
                p.target->Resize(D2D1::SizeU(LOWORD(lParam), HIWORD(lParam)));
            }
            return 0;
        case WM_DPICHANGED:
            return 0;  // taille et position sont calculées par Popup_ShowCurrent
        case WM_SETTINGCHANGE:
            if (lParam && wcscmp(reinterpret_cast<const wchar_t*>(lParam), L"ImmersiveColorSet") == 0) {
                p.dark = IsDarkTheme();
                Popup_ApplyDwm();
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_CLOSE:
            Popup_Apply(p.machine.OnEscape());
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static tbf::PopupTimings TimingsFromSettings() {
    const ModSettings s = GetSettings();
    return {static_cast<uint64_t>(s.openDelay), static_cast<uint64_t>(s.closeDelay), 300};
}

static void Popup_Create() {
    g_popup = new PopupState();
    auto& p = *g_popup;
    p.machine.SetTimings(TimingsFromSettings());
    p.dark = IsDarkTheme();
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, p.d2d.put_void());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown**>(p.dwrite.put()));

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = PopupWndProc;
    wc.hInstance = ModuleInstance();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kPopupClass;
    RegisterClassExW(&wc);
    p.hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kPopupClass, L"Taskbar Folders",
                             WS_POPUP, 0, 0, 100, 100, nullptr, nullptr, ModuleInstance(), nullptr);
    if (!p.hwnd || !p.d2d || !p.dwrite) {
        Wh_Log(L"[POPUP] Creation failed (hwnd=%p d2d=%p dwrite=%p)", p.hwnd, p.d2d.get(), p.dwrite.get());
        return;
    }
    Popup_ApplyDwm();
}

static void Popup_Destroy() {
    if (!g_popup) {
        return;
    }
    if (g_popup->hwnd) {
        DestroyWindow(g_popup->hwnd);
    }
    delete g_popup;  // libère les objets COM sur le thread qui les a créés
    g_popup = nullptr;
    UnregisterClassW(kPopupClass, ModuleInstance());
}

static void Popup_OnHover(const ButtonMessage& message) {
    if (!g_popup || !g_popup->hwnd) {
        return;
    }
    auto& p = *g_popup;
    if (p.machine.Mode() == tbf::PopupMode::Closed || message.folderId != p.machine.FolderId()) {
        p.button = message.button;
    }
    Popup_Apply(p.machine.OnHoverEnter(message.folderId, Now()));
    Popup_EnsureTick();
}

static void Popup_OnClick(const ButtonMessage& message) {
    if (!g_popup || !g_popup->hwnd) {
        return;
    }
    auto& p = *g_popup;
    p.button = message.button;
    Popup_Apply(p.machine.OnButtonClick(message.folderId, Now()));
}

static void Popup_OnSnapshotChanged() {
    if (!g_popup || !g_popup->hwnd || g_popup->machine.Mode() == tbf::PopupMode::Closed) {
        return;
    }
    auto& p = *g_popup;
    p.shownFolderId.clear();  // force le rechargement des icônes et libellés
    if (!FindFolder(p.machine.FolderId())) {
        Popup_Apply(p.machine.OnFolderRemoved(p.machine.FolderId()));
        return;
    }
    Popup_ShowCurrent();
}

static void Popup_OnSettingsChanged() {
    if (!g_popup || !g_popup->hwnd) {
        return;
    }
    g_popup->machine.SetTimings(TimingsFromSettings());
    Popup_ApplyDwm();
    Popup_OnSnapshotChanged();
}

static bool Popup_PreTranslate(MSG*) {
    return false;
}

// === SECTION: picker ===

// Sélecteur « Ajouter des applis » : fenêtre modale du thread UI du mod, même
// style que la fenêtre du dossier, qui liste les applis épinglées à la barre.

constexpr wchar_t kPickerClass[] = L"TaskbarFolders.Picker";
constexpr int kPickerMaxColumns = 6;
constexpr int kPickerOk = -2;
constexpr int kPickerCancel = -3;

struct PickerItem {
    std::wstring path;
    std::wstring name;
    tbf::Bgra icon;
    bool checked = false;
};

struct PickerState {
    HWND hwnd = nullptr;
    std::wstring title;
    std::vector<PickerItem> items;
    tbf::GridLayout layout;
    int width = 0;
    tbf::Rect okRect{};
    tbf::Rect cancelRect{};
    double scale = 1.0;
    int iconPx = 32;
    int columns = 1;
    int hot = -1;  // case survolée, ou kPickerOk / kPickerCancel
    int selected = -1;
    bool dark = true;
    float tintAlpha = 1.0f;
    bool trackingMouse = false;
    bool done = false;
    bool accepted = false;
    winrt::com_ptr<ID2D1HwndRenderTarget> target;
    std::vector<winrt::com_ptr<ID2D1Bitmap>> bitmaps;
    winrt::com_ptr<IDWriteTextFormat> titleFormat;
    winrt::com_ptr<IDWriteTextFormat> labelFormat;
    winrt::com_ptr<IDWriteTextFormat> centeredFormat;
};

// Thread UI du mod uniquement (voir Prompt_UnregisterClass).
static bool g_pickerClassRegistered = false;

static void Picker_UnregisterClass() {
    if (g_pickerClassRegistered) {
        UnregisterClassW(kPickerClass, ModuleInstance());
        g_pickerClassRegistered = false;
    }
}

static int Picker_CheckedCount(const PickerState& s) {
    return static_cast<int>(std::count_if(s.items.begin(), s.items.end(), [](const auto& i) { return i.checked; }));
}

static int Picker_HitTest(const PickerState& s, int x, int y) {
    if (s.okRect.Contains(x, y)) {
        return kPickerOk;
    }
    if (s.cancelRect.Contains(x, y)) {
        return kPickerCancel;
    }
    return tbf::HitTestCell(s.layout, x, y);
}

static void Picker_Finish(PickerState& s, bool accepted) {
    // La première décision l'emporte : DestroyWindow envoie ensuite WM_ACTIVATE
    // (WA_INACTIVE), qui ne doit pas transformer « Déplacer » en annulation.
    if (s.done) {
        return;
    }
    s.accepted = accepted && Picker_CheckedCount(s) > 0;
    s.done = true;
}

static void Picker_Activate(PickerState& s, int target) {
    if (target == kPickerOk) {
        Picker_Finish(s, true);
    } else if (target == kPickerCancel) {
        Picker_Finish(s, false);
    } else if (target >= 0 && target < static_cast<int>(s.items.size())) {
        s.items[target].checked = !s.items[target].checked;
        s.selected = target;
        InvalidateRect(s.hwnd, nullptr, FALSE);
    }
}

static void Picker_Paint(PickerState& s) {
    if (!g_popup || !g_popup->d2d || !g_popup->dwrite) {
        return;
    }
    if (!s.target) {
        RECT client{};
        GetClientRect(s.hwnd, &client);
        const auto props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
        const auto hwndProps = D2D1::HwndRenderTargetProperties(
            s.hwnd, D2D1::SizeU(client.right - client.left, client.bottom - client.top));
        if (FAILED(g_popup->d2d->CreateHwndRenderTarget(props, hwndProps, s.target.put()))) {
            s.target = nullptr;
            return;
        }
        s.bitmaps.clear();
    }
    if (!s.titleFormat) {
        const auto size = [&](float px) { return static_cast<float>(px * s.scale); };
        auto& dwrite = *g_popup->dwrite;
        dwrite.CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size(15), L"fr-fr",
                                s.titleFormat.put());
        dwrite.CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size(12), L"fr-fr",
                                s.labelFormat.put());
        dwrite.CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size(13), L"fr-fr",
                                s.centeredFormat.put());
        if (!s.titleFormat || !s.labelFormat || !s.centeredFormat) {
            return;
        }
        s.titleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        s.titleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        s.labelFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        s.labelFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        s.centeredFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        s.centeredFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        winrt::com_ptr<IDWriteInlineObject> ellipsis;
        if (SUCCEEDED(dwrite.CreateEllipsisTrimmingSign(s.labelFormat.get(), ellipsis.put()))) {
            s.labelFormat->SetTrimming(&trimming, ellipsis.get());
            s.titleFormat->SetTrimming(&trimming, ellipsis.get());
        }
    }
    if (s.bitmaps.size() != s.items.size()) {
        s.bitmaps.clear();
        const auto props =
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        for (const auto& item : s.items) {
            winrt::com_ptr<ID2D1Bitmap> bitmap;
            if (!item.icon.Empty()) {
                s.target->CreateBitmap(D2D1::SizeU(item.icon.width, item.icon.height), item.icon.pixels.data(),
                                       item.icon.width * 4, props, bitmap.put());
            }
            s.bitmaps.push_back(bitmap);
        }
    }

    auto& t = *s.target;
    const float k = static_cast<float>(s.scale);
    const int checkedCount = Picker_CheckedCount(s);
    winrt::com_ptr<ID2D1SolidColorBrush> text, tint, hot, accent, accentText, neutral;
    t.CreateSolidColorBrush(s.dark ? D2D1::ColorF(1, 1, 1, 0.95f) : D2D1::ColorF(0, 0, 0, 0.9f), text.put());
    t.CreateSolidColorBrush(s.dark ? D2D1::ColorF(0.13f, 0.13f, 0.13f, s.tintAlpha)
                                   : D2D1::ColorF(0.95f, 0.95f, 0.95f, s.tintAlpha),
                            tint.put());
    t.CreateSolidColorBrush(s.dark ? D2D1::ColorF(1, 1, 1, 0.10f) : D2D1::ColorF(0, 0, 0, 0.06f), hot.put());
    t.CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.47f, 0.83f, 1.0f), accent.put());  // bleu Windows
    t.CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), accentText.put());
    t.CreateSolidColorBrush(s.dark ? D2D1::ColorF(1, 1, 1, 0.12f) : D2D1::ColorF(0, 0, 0, 0.08f), neutral.put());

    t.BeginDraw();
    t.Clear(D2D1::ColorF(0, 0, 0, 0));
    RECT client{};
    GetClientRect(s.hwnd, &client);
    t.FillRectangle(D2D1::RectF(0, 0, static_cast<float>(client.right), static_cast<float>(client.bottom)),
                    tint.get());

    const auto titleRect = D2D1::RectF(s.layout.title.left + 4 * k, static_cast<float>(s.layout.title.top),
                                       s.width - s.layout.title.left - 4 * k,
                                       static_cast<float>(s.layout.title.bottom));
    t.DrawText(s.title.c_str(), static_cast<UINT32>(s.title.size()), s.titleFormat.get(), titleRect, text.get(),
               D2D1_DRAW_TEXT_OPTIONS_CLIP);

    if (s.items.empty()) {
        const std::wstring message = Tr(L"No pinned apps to add.", L"Aucune appli épinglée à ajouter.");
        const auto area = D2D1::RectF(static_cast<float>(s.layout.title.left), static_cast<float>(s.layout.title.bottom),
                                      s.width - static_cast<float>(s.layout.title.left),
                                      static_cast<float>(s.layout.window.height));
        t.DrawText(message.c_str(), static_cast<UINT32>(message.size()), s.labelFormat.get(), area, text.get());
    }

    for (size_t i = 0; i < s.items.size(); ++i) {
        const auto& item = s.items[i];
        const tbf::Rect& cell = s.layout.cells[i];
        const auto cellRect = D2D1::RoundedRect(
            D2D1::RectF(cell.left + 2 * k, cell.top + 2 * k, cell.right - 2 * k, cell.bottom - 2 * k), 6 * k, 6 * k);
        if (item.checked || static_cast<int>(i) == s.hot || static_cast<int>(i) == s.selected) {
            t.FillRoundedRectangle(cellRect, hot.get());
        }
        if (static_cast<int>(i) == s.selected) {
            t.DrawRoundedRectangle(cellRect, text.get(), 1.0f * k);
        }
        const float iconLeft = cell.left + (cell.Width() - s.iconPx) / 2.0f;
        const float iconTop = cell.top + 8 * k;
        if (i < s.bitmaps.size() && s.bitmaps[i]) {
            t.DrawBitmap(s.bitmaps[i].get(), D2D1::RectF(iconLeft, iconTop, iconLeft + s.iconPx, iconTop + s.iconPx),
                         item.checked ? 1.0f : 0.85f);
        }
        if (item.checked) {
            // Pastille ✓ en haut à droite de l'icône.
            const float r = 8 * k;
            const D2D1_POINT_2F center{iconLeft + s.iconPx, iconTop};
            t.FillEllipse(D2D1::Ellipse(center, r, r), accent.get());
            t.DrawLine({center.x - 3.5f * k, center.y}, {center.x - 1 * k, center.y + 2.5f * k}, accentText.get(),
                       1.6f * k);
            t.DrawLine({center.x - 1 * k, center.y + 2.5f * k}, {center.x + 3.5f * k, center.y - 2.5f * k},
                       accentText.get(), 1.6f * k);
        }
        t.DrawText(item.name.c_str(), static_cast<UINT32>(item.name.size()), s.labelFormat.get(),
                   D2D1::RectF(cell.left + 4 * k, iconTop + s.iconPx + 4 * k, cell.right - 4 * k, cell.bottom - 2 * k),
                   text.get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }

    // Boutons du pied de fenêtre.
    const auto drawButton = [&](const tbf::Rect& r, const std::wstring& label, bool primary, bool enabled,
                                bool hovered) {
        const auto rect = D2D1::RoundedRect(ToRectF(r), 5 * k, 5 * k);
        ID2D1SolidColorBrush* fill = primary && enabled ? accent.get() : neutral.get();
        fill->SetOpacity(enabled ? (hovered ? 0.85f : 1.0f) : 0.5f);
        t.FillRoundedRectangle(rect, fill);
        fill->SetOpacity(1.0f);
        ID2D1SolidColorBrush* ink = primary && enabled ? accentText.get() : text.get();
        ink->SetOpacity(enabled ? 1.0f : 0.5f);
        t.DrawText(label.c_str(), static_cast<UINT32>(label.size()), s.centeredFormat.get(), ToRectF(r), ink);
        ink->SetOpacity(1.0f);
    };
    drawButton(s.cancelRect, Tr(L"Cancel", L"Annuler"), false, true, s.hot == kPickerCancel);
    const std::wstring okLabel =
        checkedCount > 0 ? Tr(L"Move (", L"Déplacer (") + std::to_wstring(checkedCount) + L")"
                          : std::wstring(Tr(L"Move", L"Déplacer"));
    drawButton(s.okRect, okLabel, true, checkedCount > 0, s.hot == kPickerOk);

    if (t.EndDraw() == static_cast<HRESULT>(D2DERR_RECREATE_TARGET)) {
        s.target = nullptr;
        s.bitmaps.clear();
    }
}

static LRESULT CALLBACK PickerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCCREATE) {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams));
    }
    auto* s = reinterpret_cast<PickerState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (!s) {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    switch (message) {
        case WM_PAINT:
            Picker_Paint(*s);
            ValidateRect(hwnd, nullptr);
            return 0;
        case WM_MOUSEMOVE: {
            if (!s->trackingMouse) {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
                s->trackingMouse = TrackMouseEvent(&track) != FALSE;
            }
            const int hot = Picker_HitTest(*s, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            if (hot != s->hot) {
                s->hot = hot;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            s->trackingMouse = false;
            s->hot = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_LBUTTONUP:
            Picker_Activate(*s, Picker_HitTest(*s, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            return 0;
        case WM_KEYDOWN: {
            int next = s->selected;
            switch (wParam) {
                case VK_ESCAPE:
                    Picker_Finish(*s, false);
                    return 0;
                case VK_RETURN:
                    Picker_Finish(*s, true);
                    return 0;
                case VK_SPACE:
                    Picker_Activate(*s, s->selected);
                    return 0;
                case VK_LEFT:
                    next = tbf::MoveSelection(s->selected, s->items.size(), s->columns, tbf::NavKey::Left);
                    break;
                case VK_RIGHT:
                    next = tbf::MoveSelection(s->selected, s->items.size(), s->columns, tbf::NavKey::Right);
                    break;
                case VK_UP:
                    next = tbf::MoveSelection(s->selected, s->items.size(), s->columns, tbf::NavKey::Up);
                    break;
                case VK_DOWN:
                    next = tbf::MoveSelection(s->selected, s->items.size(), s->columns, tbf::NavKey::Down);
                    break;
                default:
                    return 0;
            }
            if (next != s->selected) {
                s->selected = next;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                Picker_Finish(*s, false);  // comme un menu : un clic ailleurs annule
            }
            return 0;
        case WM_CLOSE:
            Picker_Finish(*s, false);
            return 0;
        case WM_DPICHANGED:
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// Applis épinglées à la barre qui peuvent entrer dans le dossier.
static std::vector<PickerItem> PinnedAppsFor(const tbf::Folder& folder, int iconPx) {
    std::vector<PickerItem> items;
    const std::wstring dir = UserPinnedTaskbarDir();
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileW(tbf::JoinPath(dir, L"*.lnk").c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        return items;
    }
    do {
        const std::wstring name = data.cFileName;
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || !tbf::IsSelectablePinnedShortcut(name)) {
            continue;
        }
        const bool alreadyInFolder = std::any_of(folder.apps.begin(), folder.apps.end(), [&](const auto& app) {
            return tbf::EqualsIgnoreCase(app.fileName, name);
        });
        if (!alreadyInFolder) {
            PickerItem item;
            item.path = tbf::JoinPath(dir, name);
            item.name = tbf::DisplayNameFromFile(name);
            items.push_back(std::move(item));
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    std::sort(items.begin(), items.end(),
              [](const PickerItem& a, const PickerItem& b) { return tbf::LessIgnoreCase(a.name, b.name); });
    for (auto& item : items) {
        item.icon = LoadShellIcon(item.path, iconPx);
    }
    return items;
}

// Thread UI du mod : copie un raccourci dans le dossier puis désépingle l'appli
// de la barre. Renvoie faux si la copie échoue (l'appli reste épinglée).
static bool MoveShortcutIntoFolder(const std::wstring& folderId, const std::wstring& shortcutPath,
                                   IPinManagerInterop3* pinManager) {
    auto added = g_store->AddApp(folderId, shortcutPath);
    if (!added) {
        Wh_Log(L"[MOVE] Can't copy %s", shortcutPath.c_str());
        return false;
    }
    if (pinManager) {
        const HRESULT hr = WithPidl(shortcutPath, [&](PCIDLIST_ABSOLUTE pidl) {
            return pinManager->UnpinTaskbarItem(pidl, PMC_JUMPVIEWBROKER);
        });
        Wh_Log(L"[MOVE] %s → folder %s, unpin 0x%08X", added->c_str(), folderId.c_str(),
               static_cast<unsigned>(hr));
    }
    return true;
}

// Thread UI du mod : choisit des applis épinglées, les copie dans le dossier
// puis les désépingle de la barre.
static void OpenAppPicker(const std::wstring& folderId) {
    auto folder = FindFolder(folderId);
    if (!folder || !g_popup) {
        return;
    }
    if (!g_pickerClassRegistered) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = PickerWndProc;
        wc.hInstance = ModuleInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kPickerClass;
        g_pickerClassRegistered = RegisterClassExW(&wc) != 0;
    }

    const ModSettings settings = GetSettings();
    PickerState s;
    s.title = UiIsFrench() ? L"Ajouter à « " + folder->name + L" »" : L"Add to \u201C" + folder->name + L"\u201D";
    s.dark = IsDarkTheme();

    // Ancrage sur le bouton du dossier (ou le curseur, juste après sa création).
    const tbf::Rect anchor = ButtonRectFor(folderId);
    const RECT anchorRect{anchor.left, anchor.top, anchor.right, anchor.bottom};
    HMONITOR monitor = MonitorFromRect(&anchorRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(monitor, &info);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    s.scale = dpiX / 96.0;
    s.iconPx = tbf::Scaled(settings.iconSize, s.scale);
    s.items = PinnedAppsFor(*folder, s.iconPx);

    const int gap = tbf::Scaled(kPopupGap, s.scale);
    const int footer = tbf::Scaled(56, s.scale);
    const int workWidth = (info.rcWork.right - info.rcWork.left) - 2 * gap;
    const int workHeight = (info.rcWork.bottom - info.rcWork.top) - 2 * gap;
    tbf::GridMetrics metrics = tbf::MakeGridMetrics(settings.iconSize, kPickerMaxColumns, s.scale, settings.spacing);
    metrics.columns = s.items.empty() ? 3 : tbf::BalancedColumns(s.items.size(), kPickerMaxColumns);
    metrics.columns = tbf::FitColumns(s.items.size(), metrics, workWidth, workHeight - footer);
    s.columns = metrics.columns;
    s.layout = tbf::ComputeGridLayout(s.items.size(), metrics);

    const int buttonWidth = tbf::Scaled(120, s.scale);
    const int buttonHeight = tbf::Scaled(32, s.scale);
    const int buttonGap = tbf::Scaled(8, s.scale);
    s.width = std::max(s.layout.window.width, 2 * buttonWidth + buttonGap + 2 * metrics.padding);
    const int height = s.layout.window.height + footer;
    const int buttonTop = s.layout.window.height + (footer - buttonHeight) / 2 - metrics.padding / 2;
    s.okRect = {s.width - metrics.padding - buttonWidth, buttonTop, s.width - metrics.padding, buttonTop + buttonHeight};
    s.cancelRect = {s.okRect.left - buttonGap - buttonWidth, buttonTop, s.okRect.left - buttonGap,
                    buttonTop + buttonHeight};
    if (!s.items.empty()) {
        s.selected = 0;
    }

    const tbf::Rect monitorRect{info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right, info.rcMonitor.bottom};
    const tbf::Rect workRect{info.rcWork.left, info.rcWork.top, info.rcWork.right, info.rcWork.bottom};
    const tbf::Rect rect =
        tbf::ComputePopupRect(anchor, {s.width, height}, tbf::DetectEdge(anchor, monitorRect), workRect, gap);

    s.hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kPickerClass, s.title.c_str(), WS_POPUP, rect.left,
                             rect.top, rect.Width(), rect.Height(), nullptr, nullptr, ModuleInstance(), &s);
    if (!s.hwnd) {
        Wh_Log(L"[PICKER] Window creation failed (%u)", GetLastError());
        return;
    }
    s.tintAlpha = ApplyBackdrop(s.hwnd, s.dark);
    ShowWindow(s.hwnd, SW_SHOW);
    SetForegroundWindow(s.hwnd);
    SetFocus(s.hwnd);

    MSG msg;
    while (!s.done) {
        const BOOL result = GetMessageW(&msg, nullptr, 0, 0);
        if (result <= 0) {
            if (result == 0) {
                PostQuitMessage(static_cast<int>(msg.wParam));  // laisser la boucle principale s'arrêter
            }
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    DestroyWindow(s.hwnd);
    if (!s.accepted) {
        return;
    }

    winrt::com_ptr<IPinManagerInterop3> pinManager;
    CreatePinManager(pinManager);
    for (const auto& item : s.items) {
        if (!item.checked) {
            continue;
        }
        MoveShortcutIntoFolder(folderId, item.path, pinManager.get());
    }
    ReloadSnapshot();
}

// === SECTION: menus ===

// --- Callbacks XAML : chaque délégué pointe dans notre DLL, il faut les révoquer
// sur leur thread avant le déchargement (RestoreOnCurrentThread).

struct MenuRegistration {
    winrt::weak_ref<wux::Controls::MenuFlyoutItem> item;
    winrt::event_token token;
    DWORD threadId;
};

static std::mutex g_menuMutex;
static std::vector<MenuRegistration> g_menuRegistrations;

static void RegisterMenuItem(const wux::Controls::MenuFlyoutItem& item, winrt::event_token token) {
    std::lock_guard lock(g_menuMutex);
    std::erase_if(g_menuRegistrations, [](const MenuRegistration& r) { return !r.item.get(); });
    g_menuRegistrations.push_back({winrt::make_weak(item), token, GetCurrentThreadId()});
}

static void RevokeMenuCallbacksOnCurrentThread() {
    std::vector<MenuRegistration> mine;
    {
        std::lock_guard lock(g_menuMutex);
        for (auto it = g_menuRegistrations.begin(); it != g_menuRegistrations.end();) {
            if (it->threadId == GetCurrentThreadId()) {
                mine.push_back(*it);
                it = g_menuRegistrations.erase(it);
            } else {
                ++it;
            }
        }
    }
    for (const auto& r : mine) {
        try {
            if (auto item = r.item.get()) {
                item.Click(r.token);
            }
        } catch (...) {
        }
    }
}

static void PostCommand(FolderCommand command, std::wstring folderId) {
    PostToUi(WM_TBF_COMMAND, std::make_unique<CommandMessage>(CommandMessage{command, std::move(folderId)}));
}

static wux::Controls::MenuFlyoutItem MakeMenuItem(const wchar_t* text, const wchar_t* glyph, FolderCommand command,
                                                  const std::wstring& folderId) {
    wux::Controls::MenuFlyoutItem item;
    item.Text(text);
    wux::Controls::FontIcon icon;
    icon.FontFamily(wux::Media::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    item.Icon(icon);
    auto token = item.Click([command, folderId](auto&&...) { PostCommand(command, folderId); });
    RegisterMenuItem(item, token);
    return item;
}

// Taskbar.View passe une même demande par plusieurs routes selon la build :
// on n'ouvre qu'un menu par dossier et par fenêtre de 300 ms.
static bool ShouldOpenFolderMenu(const std::wstring& folderId) {
    static std::wstring lastFolder;
    static uint64_t lastTime = 0;
    const uint64_t now = Now();
    if (folderId == lastFolder && now - lastTime < 300) {
        return false;
    }
    lastFolder = folderId;
    lastTime = now;
    return true;
}

static bool TryHandleFolderContextRequest(const wux::FrameworkElement& element) {
    if (g_unloading || !element) {
        return false;
    }
    const auto folderId = FolderIdFromElement(element);
    if (!folderId) {
        return false;
    }
    if (ShouldOpenFolderMenu(*folderId)) {
        try {
            wux::Controls::MenuFlyout menu;
            menu.Items().Append(MakeMenuItem(Tr(L"Add apps…", L"Ajouter des applis…"), L"\xE710", FolderCommand::AddApps, *folderId));
            menu.Items().Append(MakeMenuItem(Tr(L"Rename…", L"Renommer…"), L"\xE8AC", FolderCommand::Rename, *folderId));
            menu.Items().Append(
                MakeMenuItem(Tr(L"Open location", L"Ouvrir l'emplacement"), L"\xE838", FolderCommand::OpenLocation, *folderId));
            menu.Items().Append(wux::Controls::MenuFlyoutSeparator());
            menu.Items().Append(
                MakeMenuItem(Tr(L"Choose a Windows icon…", L"Choisir une icône Windows…"), L"\xE70F", FolderCommand::IconResource, *folderId));
            menu.Items().Append(MakeMenuItem(Tr(L"Choose an image…", L"Choisir une image…"), L"\xE8B9", FolderCommand::IconImage, *folderId));
            if (auto folder = FindFolder(*folderId); folder && !folder->iconPath.empty()) {
                menu.Items().Append(
                    MakeMenuItem(Tr(L"Restore the mini grid", L"Rétablir la mini-grille"), L"\xE72C", FolderCommand::IconReset, *folderId));
            }
            menu.Items().Append(wux::Controls::MenuFlyoutSeparator());
            menu.Items().Append(MakeMenuItem(Tr(L"Delete folder", L"Supprimer le dossier"), L"\xE74D", FolderCommand::Delete, *folderId));
            menu.ShowAt(element);
        } catch (...) {
            Wh_Log(L"[MENU] Can't show the folder menu");
        }
    }
    return true;  // toujours couper la jump list native d'un dossier
}

// Types et pointeurs d'origine des hooks ci-dessous : déclarés dans la section
// `taskbar view hooks`, car HookTaskbarView les référence.

static void WINAPI TaskListButton_OnContextRequested_Hook(void* pThis, wux::UIElement const& sender,
                                                          wux::Input::ContextRequestedEventArgs const& args) {
    bool handled = false;
    try {
        handled = TryHandleFolderContextRequest(GetTaskListButtonElement(pThis));
    } catch (...) {
    }
    if (!handled) {
        g_onContextRequestedOriginal(pThis, sender, args);
    }
}

static void WINAPI TaskListButtonHandlers_HandleContextRequested_Hook(wux::UIElement const& sender,
                                                                      wux::Input::ContextRequestedEventArgs const& args) {
    bool handled = false;
    try {
        handled = TryHandleFolderContextRequest(sender.try_as<wux::FrameworkElement>());
    } catch (...) {
    }
    if (!handled) {
        g_handleContextRequestedOriginal(sender, args);
    }
}

static void WINAPI TaskbarResources_OnTaskListButtonContextRequested_Hook(
    void* pThis, wux::UIElement const& sender, wux::Input::ContextRequestedEventArgs const& args) {
    bool handled = false;
    try {
        handled = TryHandleFolderContextRequest(sender.try_as<wux::FrameworkElement>());
    } catch (...) {
    }
    if (!handled) {
        g_resourcesContextRequestedOriginal(pThis, sender, args);
    }
}

// --- « Nouveau dossier » dans le menu contextuel de la barre (même technique
// que le mod de séparateurs : injection au premier Append du menu).

constexpr wchar_t kNewFolderMenuName[] = L"TaskbarFolders.NewFolder";
static thread_local int t_settingsMenuDepth = 0;
static thread_local bool t_settingsMenuInjected = false;

static void __cdecl MenuFlyoutItemBaseVector_Append_Hook(void* pThis, wux::Controls::MenuFlyoutItemBase const& item) {
    if (!g_unloading && t_settingsMenuDepth > 0 && !t_settingsMenuInjected) {
        t_settingsMenuInjected = true;
        try {
            auto newFolder = MakeMenuItem(Tr(L"New folder", L"Nouveau dossier"), L"\xE8F4", FolderCommand::NewFolder, L"");
            newFolder.Name(kNewFolderMenuName);
            g_menuAppendOriginal(pThis, newFolder);
            g_menuAppendOriginal(pThis, wux::Controls::MenuFlyoutSeparator());
        } catch (...) {
            Wh_Log(L"[MENU] Can't inject \"New folder\"");
        }
    }
    g_menuAppendOriginal(pThis, item);
}

static void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(wux::FrameworkElement const& target,
                                                                     void* taskbarSettings,
                                                                     wux::Input::ContextRequestedEventArgs const& args,
                                                                     unsigned long long options) {
    if (t_settingsMenuDepth++ == 0) {
        t_settingsMenuInjected = false;
    }
    g_showSettingsMenuOriginal(target, taskbarSettings, args, options);
    --t_settingsMenuDepth;
}

// --- Saisie d'un nom (fenêtre modale du thread UI du mod).

constexpr wchar_t kPromptClass[] = L"TaskbarFolders.Prompt";

struct PromptState {
    HWND edit = nullptr;
    std::wstring result;
    bool done = false;
    bool accepted = false;
};

static LRESULT CALLBACK PromptWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<PromptState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    switch (message) {
        case WM_NCCREATE:
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams));
            break;
        case WM_COMMAND:
            if (!state) {
                break;
            }
            if (LOWORD(wParam) == IDOK) {
                const int length = GetWindowTextLengthW(state->edit);
                std::wstring text(static_cast<size_t>(length) + 1, L'\0');
                GetWindowTextW(state->edit, text.data(), length + 1);
                text.resize(static_cast<size_t>(length));
                state->result = text;
                state->accepted = true;
                state->done = true;
                return 0;
            }
            if (LOWORD(wParam) == IDCANCEL) {
                state->done = true;
                return 0;
            }
            break;
        case WM_CLOSE:
            if (state) {
                state->done = true;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// Thread UI du mod uniquement. Une classe enregistrée par une DLL survit à son
// déchargement : sans désenregistrement, un rechargement du mod à la même adresse
// réutiliserait une procédure de fenêtre pointant dans l'ancien code.
static bool g_promptClassRegistered = false;

static void Prompt_UnregisterClass() {
    if (g_promptClassRegistered) {
        UnregisterClassW(kPromptClass, ModuleInstance());
        g_promptClassRegistered = false;
    }
}

static std::optional<std::wstring> PromptForText(const wchar_t* title, const wchar_t* label,
                                                 const std::wstring& initial) {
    bool& registered = g_promptClassRegistered;
    if (!registered) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = PromptWndProc;
        wc.hInstance = ModuleInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = kPromptClass;
        registered = RegisterClassExW(&wc) != 0;
    }

    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    const double scale = dpiX / 96.0;
    auto px = [&](int value) { return tbf::Scaled(value, scale); };
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(monitor, &info);

    const DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    const DWORD exStyle = WS_EX_TOPMOST | WS_EX_DLGMODALFRAME;
    RECT frame{0, 0, px(360), px(132)};
    AdjustWindowRectExForDpi(&frame, style, FALSE, exStyle, dpiX);
    const int width = frame.right - frame.left;
    const int height = frame.bottom - frame.top;
    const int x = (info.rcWork.left + info.rcWork.right - width) / 2;
    const int y = (info.rcWork.top + info.rcWork.bottom - height) / 2;

    PromptState state;
    HWND hwnd = CreateWindowExW(exStyle, kPromptClass, title, style, x, y, width, height, nullptr, nullptr,
                                ModuleInstance(), &state);
    if (!hwnd) {
        return std::nullopt;
    }
    NONCLIENTMETRICSW metrics{};
    metrics.cbSize = sizeof(metrics);
    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0, dpiX);
    HFONT font = CreateFontIndirectW(&metrics.lfMessageFont);

    HINSTANCE instance = ModuleInstance();
    HWND labelWindow = CreateWindowExW(0, L"STATIC", label, WS_CHILD | WS_VISIBLE, px(12), px(12), px(336), px(20),
                                       hwnd, nullptr, instance, nullptr);
    state.edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", initial.c_str(),
                                 WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, px(12), px(36), px(336), px(26),
                                 hwnd, reinterpret_cast<HMENU>(100), instance, nullptr);
    HWND ok = CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, px(176),
                              px(88), px(84), px(30), hwnd, reinterpret_cast<HMENU>(IDOK), instance, nullptr);
    HWND cancel = CreateWindowExW(0, L"BUTTON", Tr(L"Cancel", L"Annuler"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                  px(264), px(88), px(84), px(30), hwnd, reinterpret_cast<HMENU>(IDCANCEL), instance,
                                  nullptr);
    for (HWND control : {labelWindow, state.edit, ok, cancel}) {
        SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    }
    SendMessageW(state.edit, EM_SETLIMITTEXT, 64, 0);
    SendMessageW(state.edit, EM_SETSEL, 0, -1);
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(state.edit);

    MSG msg;
    while (!state.done) {
        const BOOL result = GetMessageW(&msg, nullptr, 0, 0);
        if (result <= 0) {
            if (result == 0) {
                PostQuitMessage(static_cast<int>(msg.wParam));  // laisser la boucle principale s'arrêter
            }
            break;
        }
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    DestroyWindow(hwnd);
    DeleteObject(font);
    if (!state.accepted) {
        return std::nullopt;
    }
    std::wstring clean = tbf::Trim(state.result);
    if (clean.empty()) {
        return std::nullopt;
    }
    return clean;
}

// --- Commandes (thread UI du mod).

static void OpenFileLocation(const std::wstring& lnk) {
    winrt::com_ptr<IShellLinkW> link;
    winrt::com_ptr<IPersistFile> file;
    wchar_t target[MAX_PATH] = {};
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(link.put()))) &&
        SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(file.put()))) && SUCCEEDED(file->Load(lnk.c_str(), STGM_READ)) &&
        link->GetPath(target, MAX_PATH, nullptr, 0) == S_OK && tbf::PathExists(target)) {
        const std::wstring args = L"/select,\"" + std::wstring(target) + L"\"";
        ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
        return;
    }
    const std::wstring args = L"/select,\"" + lnk + L"\"";
    ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
}

static void Popup_ShowAppMenu(int index, POINT screen) {
    if (!g_popup || index < 0 || index >= static_cast<int>(g_popup->items.size())) {
        return;
    }
    auto& p = *g_popup;
    Popup_Apply(p.machine.Pin());  // la fenêtre doit rester ouverte pendant le menu
    const tbf::FolderApp app = p.items[index].app;
    const bool broken = p.items[index].broken;
    const std::wstring folderId = p.shownFolderId;

    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING | (broken ? MF_GRAYED : 0), 1, Tr(L"Open", L"Ouvrir"));
    AppendMenuW(menu, MF_STRING | (broken ? MF_GRAYED : 0), 2, Tr(L"Open file location", L"Ouvrir l'emplacement du fichier"));
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 3, Tr(L"Remove from folder", L"Retirer du dossier"));
    p.menuOpen = true;
    SetForegroundWindow(p.hwnd);
    const int command = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screen.x, screen.y, p.hwnd, nullptr);
    p.menuOpen = false;
    DestroyMenu(menu);

    switch (command) {
        case 1:
            Popup_Apply(p.machine.OnAppLaunched());
            Popup_LaunchPath(app.path);
            break;
        case 2:
            Popup_Apply(p.machine.OnAppLaunched());
            OpenFileLocation(app.path);
            break;
        case 3:
            if (g_store->RemoveApp(folderId, app.fileName, true)) {
                ReloadSnapshot();
            } else {
                Wh_Log(L"[CMD] Can't remove %s", app.fileName.c_str());
            }
            break;
    }
}

static std::optional<std::wstring> PickImageFile() {
    winrt::com_ptr<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(dialog.put())))) {
        return std::nullopt;
    }
    const COMDLG_FILTERSPEC filters[] = {{L"Images (*.png, *.jpg, *.ico, *.bmp)", L"*.png;*.jpg;*.jpeg;*.ico;*.bmp"}};
    dialog->SetFileTypes(ARRAYSIZE(filters), filters);
    dialog->SetTitle(Tr(L"Choose the folder icon", L"Choisir l'icône du dossier"));
    FILEOPENDIALOGOPTIONS options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);
    if (dialog->Show(nullptr) != S_OK) {
        return std::nullopt;  // annulé
    }
    winrt::com_ptr<IShellItem> item;
    PWSTR path = nullptr;
    if (FAILED(dialog->GetResult(item.put())) || FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
        return std::nullopt;
    }
    std::wstring result(path);
    CoTaskMemFree(path);
    return result;
}

static void RunCommand(const CommandMessage& message) {
    switch (message.command) {
        case FolderCommand::NewFolder: {
            auto name = PromptForText(Tr(L"New folder", L"Nouveau dossier"), Tr(L"Folder name:", L"Nom du dossier :"),
                                      Tr(L"New folder", L"Nouveau dossier"));
            if (!name) {
                return;
            }
            for (int attempt = 0; attempt < 5; ++attempt) {
                if (auto id = g_store->Create(*name, RandomUInt32())) {
                    Wh_Log(L"[CMD] Folder created: %s", id->c_str());
                    ReloadSnapshot();      // épingle le nouveau bouton
                    OpenAppPicker(*id);    // et propose aussitôt de le remplir
                    return;
                }
            }
            Wh_Log(L"[CMD] Folder creation failed");
            return;
        }
        case FolderCommand::Rename: {
            auto folder = FindFolder(message.folderId);
            if (!folder) {
                return;
            }
            auto name = PromptForText(Tr(L"Rename folder", L"Renommer le dossier"), Tr(L"Folder name:", L"Nom du dossier :"),
                                      folder->name);
            if (name && g_store->Rename(message.folderId, *name)) {
                ReloadSnapshot();
            }
            return;
        }
        case FolderCommand::OpenLocation:
            ShellExecuteW(nullptr, L"open", g_store->FolderDir(message.folderId).c_str(), nullptr, nullptr,
                          SW_SHOWNORMAL);
            return;
        case FolderCommand::AddApps:
            OpenAppPicker(message.folderId);
            return;
        case FolderCommand::IconResource: {
            auto folder = FindFolder(message.folderId);
            if (!folder) {
                return;
            }
            wchar_t path[MAX_PATH] = {};
            int index = 3;  // dossier de imageres.dll
            if (!folder->iconPath.empty() && !tbf::NormalizedImageExtension(folder->iconPath)) {
                folder->iconPath.copy(path, MAX_PATH - 1);
                index = folder->iconIndex;
            } else {
                ExpandEnvironmentStringsW(L"%SystemRoot%\\System32\\imageres.dll", path, MAX_PATH);
            }
            if (!PickIconDlg(nullptr, path, MAX_PATH, &index)) {
                return;  // annulé
            }
            wchar_t expanded[MAX_PATH] = {};
            ExpandEnvironmentStringsW(path, expanded, MAX_PATH);
            if (g_store->SetIconResource(message.folderId, expanded, index)) {
                ReloadSnapshot();
            } else {
                Wh_Log(L"[CMD] Icon %s,%d rejected", expanded, index);
            }
            return;
        }
        case FolderCommand::IconImage: {
            auto path = PickImageFile();
            if (!path) {
                return;
            }
            if (g_store->SetIconImage(message.folderId, *path)) {
                ReloadSnapshot();
            } else {
                MessageBoxW(nullptr,
                            Tr(L"This image can't be used (accepted formats: PNG, JPG, ICO, BMP).",
                               L"Cette image ne peut pas être utilisée (formats acceptés : PNG, JPG, ICO, BMP)."),
                            L"Taskbar Folders", MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
            }
            return;
        }
        case FolderCommand::IconReset:
            if (g_store->ClearIcon(message.folderId)) {
                ReloadSnapshot();
            }
            return;
        case FolderCommand::Delete: {
            auto folder = FindFolder(message.folderId);
            if (!folder) {
                return;
            }
            const std::wstring text =
                UiIsFrench() ? L"Supprimer le dossier « " + folder->name +
                                   L" » ?\n\nLe bouton sera retiré de la barre des tâches et ses raccourcis "
                                   L"envoyés à la corbeille."
                             : L"Delete the folder \u201C" + folder->name +
                                   L"\u201D?\n\nThe button will be removed from the taskbar and its shortcuts "
                                   L"sent to the Recycle Bin.";
            if (MessageBoxW(nullptr, text.c_str(), L"Taskbar Folders",
                            MB_YESNO | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND) != IDYES) {
                return;
            }
            if (g_popup) {
                Popup_Apply(g_popup->machine.OnFolderRemoved(message.folderId));
            }
            winrt::com_ptr<IPinManagerInterop3> pinManager;
            if (SUCCEEDED(CreatePinManager(pinManager))) {
                UnpinFolder(pinManager.get(), message.folderId);
            }
            if (!g_store->Delete(message.folderId, true)) {
                Wh_Log(L"[CMD] Can't delete %s", message.folderId.c_str());
            }
            ReloadSnapshot();
            return;
        }
    }
}

// === SECTION: ui thread ===

constexpr wchar_t kControllerClass[] = L"TaskbarFolders.Controller";

static HANDLE g_uiThread = nullptr;
static HANDLE g_uiReady = nullptr;

static LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_TBF_RELOAD:
            SetTimer(hwnd, kReloadTimer, 300, nullptr);  // regroupe les rafales de notifications
            return 0;
        case WM_TIMER:
            if (wParam == kReloadTimer || wParam == kReconcileTimer) {
                KillTimer(hwnd, wParam);
                ReloadSnapshot();
            }
            return 0;
        case WM_TBF_SETTINGS:
            Popup_OnSettingsChanged();
            return 0;
        case WM_TBF_HOVER: {
            std::unique_ptr<ButtonMessage> payload(reinterpret_cast<ButtonMessage*>(lParam));
            Popup_OnHover(*payload);
            return 0;
        }
        case WM_TBF_CLICK: {
            std::unique_ptr<ButtonMessage> payload(reinterpret_cast<ButtonMessage*>(lParam));
            Popup_OnClick(*payload);
            return 0;
        }
        case WM_TBF_COMMAND: {
            std::unique_ptr<CommandMessage> payload(reinterpret_cast<CommandMessage*>(lParam));
            RunCommand(*payload);
            return 0;
        }
        case WM_TBF_SHUTDOWN:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_controllerHwnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// Ne recrée pas la racine : si l'utilisateur l'a supprimée, on attend qu'elle revienne.
static HANDLE OpenStoreWatch() {
    if (!tbf::IsDirectory(g_store->Root())) {
        return INVALID_HANDLE_VALUE;
    }
    HANDLE change = FindFirstChangeNotificationW(
        g_store->Root().c_str(), TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
    if (change == INVALID_HANDLE_VALUE) {
        Wh_Log(L"[UI] Can't watch %s", g_store->Root().c_str());
    }
    return change;
}

static DWORD WINAPI UiThreadProc(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ControllerWndProc;
    wc.hInstance = ModuleInstance();
    wc.lpszClassName = kControllerClass;
    RegisterClassExW(&wc);
    HWND controller = CreateWindowExW(0, kControllerClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                      ModuleInstance(), nullptr);
    g_controllerHwnd = controller;
    Popup_Create();
    SetEvent(g_uiReady);

    HANDLE change = OpenStoreWatch();

    if (controller) {
        ReloadSnapshot();
    }

    bool running = controller != nullptr;
    while (running) {
        const DWORD count = change != INVALID_HANDLE_VALUE ? 1 : 0;
        // Sans surveillance (racine supprimée…), on retente toutes les 5 s.
        const DWORD timeout = count ? INFINITE : 5000;
        const DWORD wait = MsgWaitForMultipleObjectsEx(count, &change, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (count && wait == WAIT_OBJECT_0) {
            PostMessageW(controller, WM_TBF_RELOAD, 0, 0);
            // Un handle qu'on ne peut pas réarmer resterait signalé et ferait
            // tourner cette boucle à vide en noyant la file de messages.
            if (!tbf::IsDirectory(g_store->Root()) || !FindNextChangeNotification(change)) {
                Wh_Log(L"[UI] Storage watch stopped");
                FindCloseChangeNotification(change);
                change = INVALID_HANDLE_VALUE;
            }
        } else if (!count && wait == WAIT_TIMEOUT) {
            change = OpenStoreWatch();
            if (change != INVALID_HANDLE_VALUE) {
                PostMessageW(controller, WM_TBF_RELOAD, 0, 0);
            }
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            if (!Popup_PreTranslate(&msg)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    if (change != INVALID_HANDLE_VALUE) {
        FindCloseChangeNotification(change);
    }
    Popup_Destroy();
    Prompt_UnregisterClass();
    Picker_UnregisterClass();
    UnregisterClassW(kControllerClass, ModuleInstance());
    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }
    return 0;
}

static void StartUiThread() {
    g_uiReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (g_uiThread) {
        WaitForSingleObject(g_uiReady, 5000);
    } else {
        Wh_Log(L"[UI] CreateThread failed (%u)", GetLastError());
    }
}

static void StopUiThread() {
    if (HWND controller = g_controllerHwnd.load()) {
        PostMessageW(controller, WM_TBF_SHUTDOWN, 0, 0);
    }
    if (g_uiThread) {
        // Attente sans limite (comme le mod de séparateurs) : décharger la DLL
        // sous un thread encore vivant ferait planter explorer.exe.
        if (WaitForSingleObject(g_uiThread, 5000) == WAIT_TIMEOUT) {
            Wh_Log(L"[UI] UI thread is slow to stop; waiting without a timeout");
            WaitForSingleObject(g_uiThread, INFINITE);
        }
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_uiReady) {
        CloseHandle(g_uiReady);
        g_uiReady = nullptr;
    }
}

// === SECTION: entry points ===

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    {
        std::lock_guard lock(g_settingsMutex);
        g_settings = LoadSettings();
    }
    const std::wstring root = tbf::DefaultStoreRoot();
    if (root.empty()) {
        Wh_Log(L"LOCALAPPDATA not found");
        return FALSE;
    }
    g_store = std::make_unique<tbf::FolderStore>(root);
    if (!g_store->EnsureRoot()) {
        Wh_Log(L"Can't create %s", root.c_str());
        return FALSE;
    }
    return InstallTaskbarHooks() ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    StartUiThread();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    StopUiThread();
}

void Wh_ModUninit() {
    RestoreTaskbarButtons();
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    {
        std::lock_guard lock(g_settingsMutex);
        g_settings = LoadSettings();
    }
    if (HWND controller = g_controllerHwnd.load()) {
        PostMessageW(controller, WM_TBF_SETTINGS, 0, 0);
    }
}
