// ==WindhawkMod==
// @id              icons-only-for-folders-and-shortcuts
// @name            Icons Only for Folders and Shortcuts
// @description     Shows plain icons instead of thumbnails for folders and shortcuts in Explorer's large icon views, without the thumbnail frame and enlarged without blur, while files keep their thumbnails
// @name:ru         Значки вместо эскизов для папок и ярлыков
// @description:ru  Показывает в крупных видах Проводника обычные значки вместо эскизов для папок и ярлыков - без рамки эскиза и увеличенные без размытия; у файлов эскизы остаются
// @version         2.8.3
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lpropsys -lshlwapi -lgdi32 -lcomctl32 -lpsapi -static-libstdc++ -static-libgcc
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Icons Only for Folders and Shortcuts

In the large icon views of Explorer, Windows draws folders and shortcuts as
thumbnails: a folder becomes a preview of the files inside it, and a small
classic icon ends up in a white frame. This mod keeps plain icons for file
system folders and `.lnk` shortcuts, removes the thumbnail frame around them
and enlarges them to the size of the view with nearest-neighbour scaling, so
classic 32x32 icons stay sharp instead of blurred. Files keep their thumbnails.

Only the file list of Explorer folder windows is touched. The desktop
(including the Recycle Bin), the folder tree, other programs and virtual
folders keep the normal Windows behaviour. Whenever the mod cannot be sure a
drawing step is safe, it leaves the original Windows drawing in place.

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

## Version history

Filesystem folders and .lnk shortcuts use icons. File thumbnails remain enabled.

Version 2.8.3 keeps icon enlargement active during inline folder creation and
renaming. A label editor can make the view's overall clip region complex even
when an individual icon is fully visible. Capture now checks clipping inside
that icon's cell and restores the original DC state before drawing. Real holes
inside a cell still use the safe native fallback. No new repaint triggers,
desktop changes, timers or shell-item queries were added.

Version 2.8.2 requests IShellItem from the private item helper and explicitly
queries IShellItem2 before using its property methods. The optimized helper in
the local Windows build ignores the requested IID and returns IShellItem, so
requesting IShellItem2 directly is not safe. Failed interface/property queries
retain the native cutoff. Theme/display invalidation is content-view-only.
The pixel renderer and shortcut-overlay transport are unchanged.

Version 2.8.1 removes the global type-only thumbnail-cutoff hook. Only an item
positively identified as filesystem-backed can override the native cutoff.
Virtual items (including Recycle Bin), failed queries and unknown types keep
the original manager result. The 2.8 pixel/overlay renderer is unchanged.

Version 2.8 corrects tight frames and anchors the arrow glyph, not its padded cell:
- Exact-fit 50px cells can contain 48px native icons with no empty moat.
  Only their verified outer frame lines are removed; every inner pixel stays.
- The outer overlay draws once on its own saved RGB background with alpha
  initially zero. Valid coverage allows compositing just the arrow glyph at
  the main icon's lower-left corner, without transferring its old background.
- A framed or unframed arrow keeps its native size. Large unframed main icons
  also supply their bounds. If coverage cannot be established, the native
  position is retained rather than guessing transparency.

The single-pass, scroll-independent rendering introduced in 2.7 is retained:
- Only SysListView32 content inside Explorer folder windows is processed.
  Desktop (including Recycle Bin), tree icons and other windows are bypassed.
- The outer image-list draw executes once into a complete private cell on a
  verified flat background. All scaling and overlay composition happen there;
  only the final copy is clipped to the visible window/update region.
- Before BeginPaint the Explorer view's update region is expanded and erased
  so transferred scroll pixels cannot mix old and new image sizes. Scroll
  messages also request a fresh paint; there is no timer or repaint loop.
- Only a verified low-contrast frame around isolated icon content is removed.
  If the complete-cell preparation is unsafe (e.g. a nonuniform background or
  transformed DC), the original destination path remains the fallback.
- Frame thickness is measured from connected border lines, not assumed to be
  four pixels. Compact 54px cells and overlay layers below 32px are supported.
- Frame removal preserves the original icon position when no enlargement fits.
- Nested draws on the cell destination are processed before compositing finishes.
  Once a child is corrected, its ancestors cannot enlarge the result again.
- A lower-left overlay layer is never enlarged. Its verified frame is restored
  from the pre-draw pixels, including any main-icon pixels underneath the frame.
- Overlay glyph coordinates follow a corrected main icon within the same item draw.
  A coloured frame is checked against each pre-draw pixel, including uniform
  colour fields and frame sides hidden by the main icon's existing outline.
- Nearest-neighbour enlargement preserves classic icon pixels without blur.
- Each original method runs once, without black/white replay or shell-item
  extraction during painting. All GDI writes are flushed before DIB reads.
  The fallback never enlarges an incomplete source fragment.
- DPI is read from the current list-view window for each draw, not from the
  primary display at startup. No rendered image cache is shared across monitors.
- The previous off-screen black/white replay and hard-coded CLVDrawState offsets
  have been removed.

The standalone regression tests live in folder-icon-scaler-tests. This file is
self-contained for installation via Windhawk.

---

## По-русски

В крупных видах Проводника Windows рисует папки и ярлыки как эскизы: папка
превращается в превью лежащих в ней файлов, а маленький классический значок
оказывается в белой рамке. Мод оставляет для папок файловой системы и ярлыков
`.lnk` обычные значки, убирает вокруг них рамку эскиза и увеличивает их до
размера вида без сглаживания, так что классические значки 32x32 остаются
чёткими, а не размытыми. У файлов эскизы сохраняются.

Затрагивается только список файлов в окнах папок Проводника. Рабочий стол
(включая Корзину), дерево папок, другие программы и виртуальные папки ведут
себя как обычно. Если мод не уверен, что шаг отрисовки безопасен, остаётся
обычная отрисовка Windows.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folders: true
  $name: Folders
  $name:ru: Папки
- shortcuts: true
  $name: Shortcuts (.lnk)
  $name:ru: Ярлыки (.lnk)
- upscaleSmallIcons: true
  $name: Enlarge small icons without blur in large views
  $name:ru: Увеличивать маленькие значки без размытия в крупных видах
- minimumCanvasSize: 64
  $name: Minimum image-list size for enlargement
  $name:ru: Минимальный размер image list для увеличения
  $description: Smaller views can still have their icon frame removed.
  $description:ru: В меньших видах рамка значка всё равно удаляется.
- smallContentPercent: 65
  $name: Small-content detection limit (%)
  $name:ru: Порог обнаружения маленького значка (%)
  $description: >-
    An image is enlarged only when both its width and height occupy no more
    than this percentage of the image-list cell.
  $description:ru: >-
    Изображение увеличивается, только если и его ширина, и высота занимают не
    больше указанной доли ячейки image list.
- scalePercent: 200
  $name: Enlargement (%)
  $name:ru: Увеличение (%)
- maximumFillPercent: 80
  $name: Maximum cell fill (%)
  $name:ru: Максимальное заполнение ячейки (%)
  $description: Prevents an enlarged icon from touching the cell edges.
  $description:ru: Не позволяет увеличенному значку касаться краёв ячейки.
*/
// ==/WindhawkModSettings==


#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <vector>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commctrl.h>
#include <shobjidl.h>
#include <wrl/client.h>

// Kept in this single source so the standalone regression harness tests the
// same pixel and DC validation code that Windhawk compiles.
namespace FolderScaler {
constexpr unsigned int kNeverUseThumbnail = 0x7fffffff;
constexpr PROPERTYKEY kItemType = {
    {0x28636aa6, 0x953d, 0x11d2, {0xb5, 0xd6, 0x00, 0xc0, 0x4f, 0xd9, 0x18, 0xd0}}, 11};

bool ShouldForceFileSystemIcon(HRESULT attributeStatus, SFGAOF attributes,
                              HRESULT typeStatus, PCWSTR type,
                              bool folders, bool shortcuts) {
    // A generic "Folder" type does not prove a filesystem folder. In
    // particular, the original manager must remain authoritative for virtual
    // items and asynchronous updates whose item/attributes are not available.
    if (FAILED(attributeStatus) || !(attributes & SFGAO_FILESYSTEM) ||
        FAILED(typeStatus) || !type) return false;
    return (folders && (_wcsicmp(type, L"Directory") == 0 || _wcsicmp(type, L"Folder") == 0)) ||
           (shortcuts && _wcsicmp(type, L".lnk") == 0);
}

template<typename Original>
unsigned int ItemThumbnailCutoff(IShellItem* item, bool folders, bool shortcuts,
                                Original&& original) {
    if (!folders && !shortcuts) return original();
    SFGAOF attributes = 0;
    HRESULT attributeStatus = item ? item->GetAttributes(SFGAO_FILESYSTEM, &attributes) : E_FAIL;
    if (SUCCEEDED(attributeStatus) && (attributes & SFGAO_FILESYSTEM)) {
        // _GetItem can return only IShellItem even when passed IID_IShellItem2.
        // Never call an extended vtable slot without a real QueryInterface.
        Microsoft::WRL::ComPtr<IShellItem2> properties;
        if (FAILED(item->QueryInterface(IID_PPV_ARGS(&properties))) || !properties)
            return original();
        PWSTR type = nullptr;
        HRESULT typeStatus = properties->GetString(kItemType, &type);
        bool force = ShouldForceFileSystemIcon(attributeStatus, attributes, typeStatus, type,
                                              folders, shortcuts);
        CoTaskMemFree(type);
        if (force) return kNeverUseThumbnail;
    }
    // There is intentionally NO type-only hook underneath this fallback.
    return original();
}

struct Settings {
    unsigned int minimumCanvasSize = 64;
    unsigned int smallContentPercent = 65;
    unsigned int scalePercent = 200;
    unsigned int maximumFillPercent = 80;
    bool upscale = true;
};

struct Result {
    int left = 0, top = 0, width = 0, height = 0;
    int destinationWidth = 0, destinationHeight = 0;
    bool enlarged = false;
    bool sourceClipped = false;
};

int ColorDistance(uint32_t a, uint32_t b) {
    int result = 0;
    for (int shift : {0, 8, 16}) {
        result = std::max(result, std::abs(
            static_cast<int>((a >> shift) & 255) -
            static_cast<int>((b >> shift) & 255)));
    }
    return result;
}

int NearestSourceCoordinate(int destination, int sourceSize, int targetSize) {
    return std::min(static_cast<int>(
        (static_cast<int64_t>(destination) * 2 + 1) * sourceSize /
        (static_cast<int64_t>(targetSize) * 2)), sourceSize - 1);
}

bool IsExplorerRootClass(const wchar_t* name) {
    return wcscmp(name, L"CabinetWClass") == 0 ||
           wcscmp(name, L"ExploreWClass") == 0;
}

// Stack-only state for one real draw. The documented overlay bits identify
// image-list overlays; CLayerImageList is also a composition boundary. A
// small lower-left child is an overlay only within such an explicit boundary,
// never merely because an arbitrary icon happens to look like a badge.
struct DrawContext {
    DrawContext* parent;
    HDC dc;
    RECT bounds;
    bool overlayBoundary;
    bool childChanged = false;
    unsigned int depth;
    DrawContext* overlayOwner = nullptr;
    RECT mainIconBounds{};
    bool haveMainIconBounds = false;

    DrawContext(DrawContext* previous, HDC target, const RECT& rectangle,
                bool isOverlayBoundary)
        : parent(previous), dc(target), bounds(rectangle),
          overlayBoundary(isOverlayBoundary), depth(previous ? previous->depth + 1 : 0) {
        if (parent && parent->dc == dc && parent->overlayOwner) {
            overlayOwner = parent->overlayOwner;
            return;
        }
        int width = bounds.right - bounds.left, height = bounds.bottom - bounds.top;
        for (auto* ancestor = parent; ancestor; ancestor = ancestor->parent) {
            if (ancestor->dc != dc) break;
            const RECT& outer = ancestor->bounds;
            if (ancestor->overlayBoundary && bounds.left == outer.left &&
                bounds.bottom == outer.bottom && width >= 16 && height >= 16 &&
                width * 2 <= outer.right - outer.left &&
                height * 2 <= outer.bottom - outer.top) {
                overlayOwner = ancestor;
                break;
            }
        }
    }

    bool IsOverlayLayer() const { return overlayOwner != nullptr; }

    void RememberMainIcon(const Result& result) {
        if (IsOverlayLayer() || result.sourceClipped) return;
        LONG left = bounds.left + (result.enlarged ?
            (bounds.right - bounds.left - result.destinationWidth) / 2 : result.left);
        LONG top = bounds.top + (result.enlarged ?
            (bounds.bottom - bounds.top - result.destinationHeight) / 2 : result.top);
        RECT icon{left, top, left + result.destinationWidth, top + result.destinationHeight};
        for (auto* ancestor = parent; ancestor; ancestor = ancestor->parent) {
            if (ancestor->dc != dc) break;
            // Coordinates live only for this item draw, not in a cross-item,
            // cross-window or cross-DPI cache.
            ancestor->mainIconBounds = icon;
            ancestor->haveMainIconBounds = true;
        }
    }

    void MarkChanged() {
        for (auto* ancestor = parent; ancestor; ancestor = ancestor->parent) {
            if (ancestor->dc != dc) break;
            ancestor->childChanged = true;
        }
    }
};

void ObserveMainIcon(const uint32_t* before, const uint32_t* after,
                     int w, int h, const RECT& visible, DrawContext& context) {
    if (context.IsOverlayLayer() || context.childChanged || !context.parent ||
        visible.left || visible.top || visible.right != w || visible.bottom != h) return;
    bool composedItem = false;
    for (auto* parent = context.parent; parent && parent->dc == context.dc; parent = parent->parent)
        composedItem |= parent->overlayBoundary;
    if (!composedItem) return;
    int left = w, top = h, right = -1, bottom = -1;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            size_t i = static_cast<size_t>(y) * w + x;
            if (ColorDistance(before[0], before[i]) > 3) return;
            if (ColorDistance(before[i], after[i]) <= 3) continue;
            left = std::min(left, x); top = std::min(top, y);
            right = std::max(right, x); bottom = std::max(bottom, y);
        }
    if (right < left || bottom < top) return;
    Result result{left, top, right - left + 1, bottom - top + 1,
        right - left + 1, bottom - top + 1};
    context.RememberMainIcon(result);
}

// True means output is a complete replacement for after. False leaves output
// untouched and requires keeping the original draw. Every observable frame
// side must be continuous (at least three sides must be visible). The source
// must be completely visible before enlargement; a clipped fragment can only
// have its verified frame removed. Identified overlays allow two adjacent
// frame sides over an existing main icon, but never scale their content.
bool ProcessCapturedPixels(const uint32_t* before, const uint32_t* after,
                           int width, int height, unsigned int dpi,
                           const Settings& settings,
                           std::vector<uint32_t>& output, Result& result,
                           const RECT* captureArea = nullptr,
                           bool overlayLayer = false) {
    const int minimumSide = overlayLayer ? 16 : 32;
    if (!before || !after || width < minimumSide || height < minimumSide ||
        width > 1024 || height > 1024 || dpi < 48 || dpi > 768) {
        return false;
    }
    RECT visible = captureArea ? *captureArea : RECT{0, 0, width, height};
    if (visible.left < 0 || visible.top < 0 ||
        visible.right > width || visible.bottom > height ||
        visible.right - visible.left < minimumSide ||
        visible.bottom - visible.top < minimumSide) return false;
    const bool seeLeft = visible.left == 0, seeTop = visible.top == 0;
    const bool seeRight = visible.right == width, seeBottom = visible.bottom == height;
    if (static_cast<int>(seeLeft) + seeTop + seeRight + seeBottom < 3) return false;
    const size_t count = static_cast<size_t>(width) * height;
    const uint32_t background = before[static_cast<size_t>(visible.top) * width + visible.left];
    std::vector<unsigned char> changed(count, 0);
    for (int y = visible.top; y < visible.bottom; ++y) {
      for (int x = visible.left; x < visible.right; ++x) {
        const size_t i = static_cast<size_t>(y) * width + x;
        // Repainting over an existing icon, wallpaper, a gradient, or an
        // incomplete background cannot safely reconstruct transparency.
        // An identified overlay is not rescaled or reconstructed. Restoring
        // just its frame from before preserves the main icon beneath it, so
        // this path can safely use a nonuniform pre-draw image.
        if (!overlayLayer && ColorDistance(before[i], background) > 3) return false;
        changed[i] = ColorDistance(before[i], after[i]) > 3;
      }
    }

    const int searchBand = std::clamp(
        static_cast<int>((4 * dpi + 95) / 96), 1,
        std::min(width, height) / 4);
    int band = searchBand;
    int detectedBand = 0;
    std::vector<unsigned char> frameMask(count, 0);
    auto differs = [&](int x, int y) {
        size_t i = static_cast<size_t>(y) * width + x;
        return changed[i] != 0 && !frameMask[i];
    };
    auto framePixel = [&](int x, int y) {
        size_t i = static_cast<size_t>(y) * width + x;
        const uint32_t pixel = after[i];
        if (overlayLayer) {
            // Over an existing coloured icon the frame itself is coloured by
            // alpha blending. Check its small change from the saved pixels,
            // not its absolute grey value on a presumed flat background.
            return differs(x, y) && ColorDistance(before[i], pixel) <= 80;
        }
        // Classic thumbnail outlines are low-contrast neutral lines. A
        // saturated icon outline or a black photo edge is not this frame.
        int b = pixel & 255, g = (pixel >> 8) & 255, r = (pixel >> 16) & 255;
        return differs(x, y) && ColorDistance(pixel, background) <= 80 &&
               std::max({b, g, r}) - std::min({b, g, r}) <= 20;
    };
    auto horizontalSide = [&](bool lower) {
        if (lower ? !seeBottom : !seeTop) return true;
        int first = std::max(band, static_cast<int>(visible.left));
        int last = std::min(width - band, static_cast<int>(visible.right));
        bool found = false;
        for (int n = 0; n < searchBand; ++n) {
            int y = lower ? height - 1 - n : n;
            int hits = 0;
            for (int x = first; x < last; ++x) hits += framePixel(x, y);
            if (hits * 100 >= (last - first) * 85) {
                found = true;
                detectedBand = std::max(detectedBand, n + 1);
            } else if (found) break;
        }
        return found;
    };
    auto verticalSide = [&](bool rightSide) {
        if (rightSide ? !seeRight : !seeLeft) return true;
        int first = std::max(band, static_cast<int>(visible.top));
        int last = std::min(height - band, static_cast<int>(visible.bottom));
        bool found = false;
        for (int n = 0; n < searchBand; ++n) {
            int x = rightSide ? width - 1 - n : n;
            int hits = 0;
            for (int y = first; y < last; ++y) hits += framePixel(x, y);
            if (hits * 100 >= (last - first) * 85) {
                found = true;
                detectedBand = std::max(detectedBand, n + 1);
            } else if (found) break;
        }
        return found;
    };
    bool topSide = horizontalSide(false), bottomSide = horizontalSide(true);
    bool leftSide = verticalSide(false), rightSide = verticalSide(true);
    if (overlayLayer) {
        // A frame blended over a large icon may disappear into its existing
        // outline on two sides (Pixelformer). Require two adjacent observable
        // sides; all other changed border pixels are still validated below.
        if (!((seeTop && topSide) || (seeBottom && bottomSide)) ||
            !((seeLeft && leftSide) || (seeRight && rightSide))) return false;
    } else if (!topSide || !bottomSide || !leftSide || !rightSide) return false;
    if (!detectedBand) return false;
    // Search broadly, but reserve only the measured frame thickness. A fixed
    // 4px band ate into a 48px icon's 3px margin in the real 54px view cells.
    band = detectedBand;
    // A 50px compact cell has a native 48px image plus just the two frame
    // pixels. There is no blank moat at the left/right of that native icon.
    // In this exact-fit case only the verified outer lines may be removed;
    // never flood into or rescale the adjacent image pixels.
    const bool tightNativeCell = !overlayLayer && seeLeft && seeRight && seeTop && seeBottom &&
        width - 2 * band == static_cast<int>(48 * dpi / 96) &&
        height - 2 * band == static_cast<int>(48 * dpi / 96) &&
        band <= static_cast<int>((dpi + 95) / 96);

    // Include the verified line's connected antialias/shadow pixels without
    // reserving the entire search margin. This traversal cannot leave that
    // narrow margin, cross a clear pixel gap, or consume a high-contrast edge.
    // The later clear-moat check still rejects an icon connected to the frame.
    std::vector<size_t> pending;
    auto addFramePixel = [&](int x, int y) {
        if (x < visible.left || x >= visible.right || y < visible.top || y >= visible.bottom ||
            (x >= searchBand && x < width - searchBand &&
             y >= searchBand && y < height - searchBand)) return;
        size_t i = static_cast<size_t>(y) * width + x;
        if (frameMask[i] || !framePixel(x, y)) return;
        frameMask[i] = 1;
        pending.push_back(i);
    };
    for (int y = visible.top; y < visible.bottom; ++y)
        for (int x = visible.left; x < visible.right; ++x)
            if (x < band || x >= width - band || y < band || y >= height - band)
                addFramePixel(x, y);
    for (size_t n = 0; !tightNativeCell && n < pending.size(); ++n) {
        int x = static_cast<int>(pending[n] % width), y = static_cast<int>(pending[n] / width);
        addFramePixel(x - 1, y); addFramePixel(x + 1, y);
        addFramePixel(x, y - 1); addFramePixel(x, y + 1);
    }
    if (!tightNativeCell) for (size_t i : pending) {
        int x = static_cast<int>(i % width), y = static_cast<int>(i / width);
        for (POINT next : {POINT{x - 1, y}, POINT{x + 1, y}, POINT{x, y - 1}, POINT{x, y + 1}}) {
            if (next.x >= searchBand && next.x < width - searchBand &&
                next.y >= searchBand && next.y < height - searchBand &&
                next.x >= visible.left && next.x < visible.right &&
                next.y >= visible.top && next.y < visible.bottom &&
                changed[static_cast<size_t>(next.y) * width + next.x]) return false;
        }
    }

    // The verified sides do not authorize deleting unrelated corner pixels
    // (for example a shortcut overlay or a coloured fragment of an icon).
    for (int y = visible.top; y < visible.bottom; ++y) {
        for (int x = visible.left; x < visible.right; ++x) {
            if ((x < band || x >= width - band ||
                 y < band || y >= height - band) &&
                differs(x, y) && !framePixel(x, y)) return false;
        }
    }

    // One clear moat pixel suffices even in the real 58px compact cells,
    // where a 48px icon has only five pixels of margin on either side.
    // At an actual clip edge retain every source pixel. A touching icon can
    // have its verified outer frame removed, but cannot be enlarged.
    bool sourceClipped = false;
    if (!tightNativeCell) {
        int left = std::max(band, static_cast<int>(visible.left));
        int right = std::min(width - 1 - band, static_cast<int>(visible.right) - 1);
        int top = std::max(band, static_cast<int>(visible.top));
        int bottom = std::min(height - 1 - band, static_cast<int>(visible.bottom) - 1);
        for (int x = left; x <= right; ++x) {
            if (differs(x, top)) {
                if (seeTop) return false;
                sourceClipped = true;
            }
            if (differs(x, bottom)) {
                if (seeBottom) return false;
                sourceClipped = true;
            }
        }
        for (int y = top; y <= bottom; ++y) {
            if (differs(left, y)) {
                if (seeLeft) return false;
                sourceClipped = true;
            }
            if (differs(right, y)) {
                if (seeRight) return false;
                sourceClipped = true;
            }
        }
    }
    int left = width, top = height, right = -1, bottom = -1;
    size_t contentPixels = 0;
    const int margin = band + (tightNativeCell ? 0 : 1);
    const int firstX = std::max(margin, static_cast<int>(visible.left));
    const int lastX = std::min(width - margin, static_cast<int>(visible.right));
    const int firstY = std::max(margin, static_cast<int>(visible.top));
    const int lastY = std::min(height - margin, static_cast<int>(visible.bottom));
    for (int y = firstY; y < lastY; ++y) {
        for (int x = firstX; x < lastX; ++x) {
            if (!differs(x, y)) continue;
            left = std::min(left, x); right = std::max(right, x);
            top = std::min(top, y); bottom = std::max(bottom, y);
            ++contentPixels;
        }
    }
    if (right < left || bottom < top) return false;
    int contentWidth = right - left + 1, contentHeight = bottom - top + 1;
    // Never replace a frame by a tiny remnant of an otherwise erased icon.
    int minimumContent = overlayLayer ? 8 : 12;
    int minimumWidth = sourceClipped && (!seeLeft || !seeRight) ? 1 : minimumContent;
    int minimumHeight = sourceClipped && (!seeTop || !seeBottom) ? 1 : minimumContent;
    if (contentWidth < minimumWidth || contentHeight < minimumHeight ||
        contentPixels < static_cast<size_t>(contentWidth * contentHeight) / 8) {
        return false;
    }
    // Compact 64px cells can hold a 48px icon. The 65% enlargement limit must
    // not stop frame removal there, but large photo thumbnails stay original.
    unsigned int cleanupLimit = std::clamp(
        std::max(80u, settings.smallContentPercent), 80u, 90u);
    const int nativeIconLimit = static_cast<int>(48 * dpi / 96);
    const int compactCellLimit = static_cast<int>(64 * dpi / 96);
    bool compactNativeIcon = width <= compactCellLimit && height <= compactCellLimit &&
        contentWidth <= nativeIconLimit && contentHeight <= nativeIconLimit;
    if (!compactNativeIcon &&
        (contentWidth * 100 > width * static_cast<int>(cleanupLimit) ||
         contentHeight * 100 > height * static_cast<int>(cleanupLimit))) {
        return false;
    }

    bool isSmall = contentWidth * 100 <=
                     width * static_cast<int>(settings.smallContentPercent) &&
                 contentHeight * 100 <=
                     height * static_cast<int>(settings.smallContentPercent);
    double scale = 1.0;
    if (!sourceClipped && !overlayLayer && !tightNativeCell && settings.upscale && isSmall &&
        width >= static_cast<int>(settings.minimumCanvasSize) &&
        height >= static_cast<int>(settings.minimumCanvasSize)) {
        scale = std::min({
            settings.scalePercent / 100.0,
            width * settings.maximumFillPercent / (100.0 * contentWidth),
            height * settings.maximumFillPercent / (100.0 * contentHeight)});
    }
    result = {left, top, contentWidth, contentHeight, contentWidth,
              contentHeight, scale > 1.05};
    result.sourceClipped = sourceClipped;

    if (!result.enlarged) {
        // Do not recenter an unscaled icon: preserve the original position,
        // overlay, alpha/compositing result, and every interior pixel.
        output.assign(after, after + count);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (frameMask[static_cast<size_t>(y) * width + x] ||
                    x < band || x >= width - band ||
                    y < band || y >= height - band) {
                    size_t i = static_cast<size_t>(y) * width + x;
                    output[i] = before[i];
                }
            }
        }
        return true;
    }

    result.destinationWidth = std::clamp(
        static_cast<int>(std::lround(contentWidth * scale)), 1, width);
    result.destinationHeight = std::clamp(
        static_cast<int>(std::lround(contentHeight * scale)), 1, height);
    int destinationLeft = (width - result.destinationWidth) / 2;
    int destinationTop = (height - result.destinationHeight) / 2;
    output.assign(before, before + count);
    for (int y = 0; y < result.destinationHeight; ++y) {
        int sy = top + NearestSourceCoordinate(
            y, contentHeight, result.destinationHeight);
        for (int x = 0; x < result.destinationWidth; ++x) {
            int sx = left + NearestSourceCoordinate(
                x, contentWidth, result.destinationWidth);
            output[static_cast<size_t>(destinationTop + y) * width +
                   destinationLeft + x] =
                frameMask[static_cast<size_t>(sy) * width + sx] ?
                    before[static_cast<size_t>(sy) * width + sx] :
                    after[static_cast<size_t>(sy) * width + sx];
        }
    }
    return true;
}

class DibSurface {
public:
    DibSurface(int width, int height, HDC compatibleWith) {
        dc_ = CreateCompatibleDC(compatibleWith);
        if (!dc_) return;
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bitmap_ = CreateDIBSection(dc_, &bmi, DIB_RGB_COLORS,
            reinterpret_cast<void**>(&pixels_), nullptr, 0);
        if (bitmap_) oldBitmap_ = SelectObject(dc_, bitmap_);
    }
    DibSurface(const DibSurface&) = delete;
    DibSurface& operator=(const DibSurface&) = delete;
    ~DibSurface() {
        // Drain queued uses before releasing memory owned by a DIB section.
        if (dc_) GdiFlush();
        if (oldBitmap_ && oldBitmap_ != HGDI_ERROR) SelectObject(dc_, oldBitmap_);
        if (bitmap_) DeleteObject(bitmap_);
        if (dc_) DeleteDC(dc_);
    }
    explicit operator bool() const {
        return dc_ && bitmap_ && pixels_ && oldBitmap_ &&
               oldBitmap_ != HGDI_ERROR;
    }
    HDC dc() const { return dc_; }
    uint32_t* pixels() const { return pixels_; }
private:
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ oldBitmap_ = nullptr;
    uint32_t* pixels_ = nullptr;
};

bool GetCaptureArea(HDC dc, int x, int y, int width, int height, RECT& visible,
                    int minimumVisibleExtent = 32) {
    if (!dc || width < 16 || height < 16 || width > 1024 || height > 1024 ||
        x < -32768 || y < -32768 || x > 32768 || y > 32768 ||
        GetMapMode(dc) != MM_TEXT || (GetLayout(dc) & LAYOUT_RTL)) return false;
    if (GetGraphicsMode(dc) == GM_ADVANCED) {
        XFORM transform{};
        if (!GetWorldTransform(dc, &transform) ||
            transform.eM11 != 1 || transform.eM22 != 1 ||
            transform.eM12 != 0 || transform.eM21 != 0) return false;
    }

    RECT clip{};
    RECT requested{x, y, x + width, y + height};
    // Only read pixels that exist both in the DC and its current paint region.
    // Their bounds are passed to the analyzer; unseen pixels aren't evidence
    // of a clear background, a complete icon or a missing frame side.
    int clipType = GetClipBox(dc, &clip);
    if (clipType == COMPLEXREGION) {
        // An inline label editor (new folder/F2) punches a hole in the whole
        // view's clip, not necessarily in THIS image. Test the effective region
        // inside the requested cell; a bounding-box-only check would also
        // accept hidden pixels. GetClipBox includes the DC's visible region,
        // unlike GetClipRgn alone, which only describes the application clip.
        // Restore before any native draw, including all rejection paths.
        int saved = SaveDC(dc);
        if (!saved) return false;
        clipType = IntersectClipRect(dc, requested.left, requested.top,
            requested.right, requested.bottom) == ERROR ? ERROR : GetClipBox(dc, &clip);
        if (!RestoreDC(dc, saved)) return false;
    }
    if (clipType != SIMPLEREGION) return false;
    if (!IntersectRect(&visible, &clip, &requested)) return false;
    POINT corners[] = {{x, y}, {x + width, y}, {x, y + height}};
    if (!LPtoDP(dc, corners, 3) ||
        corners[1].x - corners[0].x != width ||
        corners[1].y != corners[0].y ||
        corners[2].y - corners[0].y != height ||
        corners[2].x != corners[0].x) return false;
    if (GetObjectType(dc) == OBJ_MEMDC) {
        BITMAP bitmap{};
        if (!GetObjectW(GetCurrentObject(dc, OBJ_BITMAP),
                        sizeof(bitmap), &bitmap)) return false;
        int offsetX = corners[0].x - x, offsetY = corners[0].y - y;
        RECT bitmapBounds{-offsetX, -offsetY,
                          bitmap.bmWidth - offsetX,
                          std::abs(bitmap.bmHeight) - offsetY};
        RECT bounded{};
        if (!IntersectRect(&bounded, &visible, &bitmapBounds)) return false;
        visible = bounded;
    }
    OffsetRect(&visible, -x, -y);
    if (visible.right - visible.left < minimumVisibleExtent ||
        visible.bottom - visible.top < minimumVisibleExtent) return false;
    return true;
}

// In 2.6 each draw was analyzed after the window clip had already discarded
// source pixels. That cannot yield a stable image near a viewport edge. Here
// the outer draw runs ONCE into a full-size private canvas. Nested icon/overlay
// processing sees the complete source; only presentation is window-clipped.
// No rendered images, shell-item pointers or coordinates survive this call.
// A true return means draw was invoked: callers MUST NOT replay it, including
// E_PENDING/failure paths. False means the original has not been called.
template<typename Draw>
bool TryDrawCompleteCell(const IMAGELISTDRAWPARAMS& parameters,
                         int width, int height, Draw&& draw, HRESULT& result) {
    bool invoked = false;
    result = E_FAIL;
    if (width < 32 || height < 32) return false;
    try {
        RECT visible{};
        if (!GetCaptureArea(parameters.hdcDst, parameters.x, parameters.y,
                            width, height, visible, 1)) return false;
        int visibleWidth = visible.right - visible.left;
        int visibleHeight = visible.bottom - visible.top;
        DibSurface observed(visibleWidth, visibleHeight, parameters.hdcDst);
        if (!observed || !BitBlt(observed.dc(), 0, 0, visibleWidth, visibleHeight,
            parameters.hdcDst, parameters.x + visible.left,
            parameters.y + visible.top, SRCCOPY) || !GdiFlush()) return false;
        uint32_t canvasBackground = observed.pixels()[0];
        for (size_t i = 0; i < static_cast<size_t>(visibleWidth) * visibleHeight; ++i)
            if (ColorDistance(observed.pixels()[i], canvasBackground) > 3) return false;

        DibSurface canvas(width, height, parameters.hdcDst);
        if (!canvas) return false;
        std::fill_n(canvas.pixels(), static_cast<size_t>(width) * height, canvasBackground);
        // Keep original logical x/y for every nested call. Only the DC viewport
        // changes; both real and buffered draws remain 1:1 in device pixels.
        if (!SetViewportOrgEx(canvas.dc(), -parameters.x, -parameters.y, nullptr))
            return false;
        for (UINT kind : {OBJ_FONT, OBJ_PEN, OBJ_BRUSH}) {
            HGDIOBJ object = GetCurrentObject(parameters.hdcDst, kind);
            if (object) SelectObject(canvas.dc(), object);
        }
        SetBkMode(canvas.dc(), GetBkMode(parameters.hdcDst));
        SetBkColor(canvas.dc(), GetBkColor(parameters.hdcDst));
        SetTextColor(canvas.dc(), GetTextColor(parameters.hdcDst));
        SetTextAlign(canvas.dc(), GetTextAlign(parameters.hdcDst));
        SetROP2(canvas.dc(), GetROP2(parameters.hdcDst));
        SetStretchBltMode(canvas.dc(), GetStretchBltMode(parameters.hdcDst));
        POINT brushOrigin{}, deviceOrigin{parameters.x, parameters.y};
        if (GetBrushOrgEx(parameters.hdcDst, &brushOrigin) &&
            LPtoDP(parameters.hdcDst, &deviceOrigin, 1))
            SetBrushOrgEx(canvas.dc(), brushOrigin.x - deviceOrigin.x,
                brushOrigin.y - deviceOrigin.y, nullptr);

        IMAGELISTDRAWPARAMS buffered = parameters;
        buffered.hdcDst = canvas.dc();
        invoked = true;
        result = draw(&buffered);
        GdiFlush();
        if (!BitBlt(parameters.hdcDst, parameters.x + visible.left,
                parameters.y + visible.top, visibleWidth, visibleHeight,
                canvas.dc(), parameters.x + visible.left,
                parameters.y + visible.top, SRCCOPY)) {
            // A second presentation method, not a second shell draw.
            BITMAPINFO bmi{};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = width;
            bmi.bmiHeader.biHeight = -height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            StretchDIBits(parameters.hdcDst, parameters.x, parameters.y,
                width, height, 0, 0, width, height, canvas.pixels(), &bmi,
                DIB_RGB_COLORS, SRCCOPY);
        }
        GdiFlush();
        return true;
    } catch (...) {
        return invoked;
    }
}

bool IsScrollMessage(UINT message, WPARAM key) {
    return message == WM_VSCROLL || message == WM_HSCROLL ||
           message == WM_MOUSEWHEEL || message == WM_MOUSEHWHEEL ||
           message == LVM_SCROLL || (message == WM_KEYDOWN &&
               (key == VK_UP || key == VK_DOWN || key == VK_LEFT ||
                key == VK_RIGHT || key == VK_PRIOR || key == VK_NEXT ||
                key == VK_HOME || key == VK_END));
}

bool ShouldInvalidateContentView(bool contentView, UINT message, WPARAM key, bool enabled) {
    // Theme/display messages are also sent by classic-desktop modifications
    // during their updates. The desktop is outside this renderer's scope.
    constexpr UINT kDpiChangedAfterParent = 0x02e3;
    return contentView && (message == kDpiChangedAfterParent || message == WM_DISPLAYCHANGE ||
        message == WM_THEMECHANGED || (enabled && IsScrollMessage(message, key)));
}

// The overlay is drawn once over the saved RGB with destination alpha zero.
// AlphaBlend then supplies source coverage, independently of the old RGB.
// Recover premultiplied source colour, not the composited background. Legacy
// GDI draws which do not supply valid alpha are rejected without moving them.
bool CompositeBadge(const uint32_t* before, const uint32_t* drawn, int width,
                    int height, const Result& glyph, std::vector<uint32_t>& target,
                    int targetWidth, int targetHeight, int targetX, int targetY) {
    if (!before || !drawn || width < 1 || height < 1 || glyph.width < 1 || glyph.height < 1 ||
        glyph.left < 0 || glyph.top < 0 || glyph.left + glyph.width > width ||
        glyph.top + glyph.height > height || targetX < 0 || targetY < 0 ||
        targetX + glyph.width > targetWidth || targetY + glyph.height > targetHeight ||
        target.size() != static_cast<size_t>(targetWidth) * targetHeight) return false;
    bool coverage = false;
    // Validate before touching the destination: failure cannot leave half an arrow.
    for (int y = 0; y < glyph.height; ++y)
        for (int x = 0; x < glyph.width; ++x) {
            size_t i = static_cast<size_t>(glyph.top + y) * width + glyph.left + x;
            int alpha = drawn[i] >> 24;
            if (!alpha && ColorDistance(before[i], drawn[i]) != 0) return false;
            coverage |= alpha != 0;
            for (int shift : {0, 8, 16}) {
                int source = ((drawn[i] >> shift) & 255) -
                    (((before[i] >> shift) & 255) * (255 - alpha) + 127) / 255;
                if (source < -1 || source > alpha + 1) return false;
            }
        }
    if (!coverage) return false;
    for (int y = 0; y < glyph.height; ++y)
        for (int x = 0; x < glyph.width; ++x) {
            size_t i = static_cast<size_t>(glyph.top + y) * width + glyph.left + x;
            size_t destination = static_cast<size_t>(targetY + y) * targetWidth + targetX + x;
            int alpha = drawn[i] >> 24;
            if (!alpha) continue;
            uint32_t pixel = target[destination] & 0xff000000;
            for (int shift : {0, 8, 16}) {
                int source = ((drawn[i] >> shift) & 255) -
                    (((before[i] >> shift) & 255) * (255 - alpha) + 127) / 255;
                int channel = std::clamp(source, 0, alpha) +
                    (((target[destination] >> shift) & 255) * (255 - alpha) + 127) / 255;
                pixel |= static_cast<uint32_t>(std::clamp(channel, 0, 255)) << shift;
            }
            target[destination] = pixel;
        }
    return true;
}

bool MeasureAlphaBadge(const uint32_t* before, const uint32_t* after,
                       int w, int h, unsigned int dpi, Result& glyph) {
    int left = w, top = h, right = -1, bottom = -1;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            size_t i = static_cast<size_t>(y) * w + x;
            if (!(after[i] >> 24)) {
                if (ColorDistance(before[i], after[i])) return false;
                continue;
            }
            left = std::min(left, x); top = std::min(top, y);
            right = std::max(right, x); bottom = std::max(bottom, y);
        }
    if (right < left || bottom < top || right - left + 1 > static_cast<int>(32 * dpi / 96) ||
        bottom - top + 1 > static_cast<int>(32 * dpi / 96)) return false;
    glyph = {left, top, right - left + 1, bottom - top + 1,
        right - left + 1, bottom - top + 1};
    return true;
}

template<typename Draw>
bool TryDrawAnchoredOverlay(const IMAGELISTDRAWPARAMS& p, DrawContext& context,
                            unsigned int dpi, const Settings& settings, Draw&& draw,
                            HRESULT& status) {
    if (!context.overlayOwner || (context.parent && context.parent->IsOverlayLayer())) return false;
    const RECT owner = context.overlayOwner->bounds;
    const int w = context.bounds.right - context.bounds.left;
    const int h = context.bounds.bottom - context.bounds.top;
    const int ow = owner.right - owner.left, oh = owner.bottom - owner.top;
    const int ox = p.x - owner.left, oy = p.y - owner.top;
    bool invoked = false;
    status = E_FAIL;
    try {
        RECT visible{};
        if (w < 16 || h < 16 || ow > 1024 || oh > 1024 || ox < 0 || oy < 0 ||
            ox + w > ow || oy + h > oh ||
            !GetCaptureArea(p.hdcDst, owner.left, owner.top, ow, oh, visible, 1) ||
            visible.left || visible.top || visible.right != ow || visible.bottom != oh) return false;
        DibSurface saved(ow, oh, p.hdcDst), layer(w, h, p.hdcDst);
        if (!saved || !layer || !BitBlt(saved.dc(), 0, 0, ow, oh, p.hdcDst,
            owner.left, owner.top, SRCCOPY) || !GdiFlush()) return false;
        std::vector<uint32_t> before(static_cast<size_t>(w) * h);
        std::vector<uint32_t> composed(saved.pixels(), saved.pixels() + static_cast<size_t>(ow) * oh);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                layer.pixels()[y * w + x] = before[y * w + x] =
                    saved.pixels()[(oy + y) * ow + ox + x] & 0xffffff;
        if (!SetViewportOrgEx(layer.dc(), -p.x, -p.y, nullptr)) return false;
        for (UINT kind : {OBJ_FONT, OBJ_PEN, OBJ_BRUSH}) {
            HGDIOBJ object = GetCurrentObject(p.hdcDst, kind);
            if (object) SelectObject(layer.dc(), object);
        }
        SetBkColor(layer.dc(), GetBkColor(p.hdcDst));
        SetTextColor(layer.dc(), GetTextColor(p.hdcDst));
        SetBkMode(layer.dc(), GetBkMode(p.hdcDst));
        SetTextAlign(layer.dc(), GetTextAlign(p.hdcDst));
        SetROP2(layer.dc(), GetROP2(p.hdcDst));
        SetStretchBltMode(layer.dc(), GetStretchBltMode(p.hdcDst));
        IMAGELISTDRAWPARAMS isolated = p;
        isolated.hdcDst = layer.dc();
        struct NativePresentation {
            HDC destination, source;
            int x, y, w, h;
            bool pending = false;
            ~NativePresentation() {
                if (!pending) return;
                GdiFlush();
                BitBlt(destination, x, y, w, h, source, x, y, SRCCOPY);
                GdiFlush();
            }
        } presentation{p.hdcDst, layer.dc(), p.x, p.y, w, h};
        invoked = true;
        presentation.pending = true;
        status = draw(&isolated);
        GdiFlush();
        // Present native output even if recognition fails; never call original twice.
        Result glyph;
        std::vector<uint32_t> cleaned;
        bool changed = SUCCEEDED(status) && ProcessCapturedPixels(before.data(), layer.pixels(),
            w, h, dpi, settings, cleaned, glyph, nullptr, true);
        bool anchored = false;
        bool haveGlyph = changed || (SUCCEEDED(status) &&
            MeasureAlphaBadge(before.data(), layer.pixels(), w, h, dpi, glyph));
        if (haveGlyph && context.overlayOwner->haveMainIconBounds) {
            const RECT main = context.overlayOwner->mainIconBounds;
            anchored = CompositeBadge(before.data(), layer.pixels(), w, h, glyph, composed,
                ow, oh, main.left - owner.left, main.bottom - owner.top - glyph.height);
        }
        if (!anchored) {
            const uint32_t* source = changed ? cleaned.data() : layer.pixels();
            for (int y = 0; y < h; ++y)
                std::copy_n(source + y * w, w, composed.data() + (oy + y) * ow + ox);
        }
        std::copy(composed.begin(), composed.end(), saved.pixels());
        if (BitBlt(p.hdcDst, owner.left, owner.top, ow, oh, saved.dc(), 0, 0, SRCCOPY) && GdiFlush()) {
            presentation.pending = false;
            if (changed || anchored) context.MarkChanged();
        }
        return true;
    } catch (...) {
        return invoked;
    }
}

// Called before the control's BeginPaint, never after EndPaint. The same
// paint validates this expanded update region, so there is no repaint loop.
void PrepareStablePaint(HWND window) {
    if (window) InvalidateRect(window, nullptr, TRUE);
}
}  // namespace FolderScaler

#ifndef FOLDER_SCALER_TEST
#include <cstdarg>
#include <initguid.h>
#include <psapi.h>
#include <propsys.h>
#include <propvarutil.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windhawk_utils.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;
std::atomic<bool> g_folders = true, g_shortcuts = true;
std::atomic<bool> g_upscaleSmallIcons = true;
std::atomic<unsigned int> g_minimumCanvasSize = 64;
std::atomic<unsigned int> g_smallContentPercent = 65;
std::atomic<unsigned int> g_scalePercent = 200;
std::atomic<unsigned int> g_maximumFillPercent = 80;
std::atomic<bool> g_ready = false, g_unloading = false;
std::atomic<int> g_debugMessagesRemaining = 160;
thread_local HWND g_paintView = nullptr;
thread_local bool g_bypassDrawProcessing = false;
thread_local bool g_completeCellDraw = false;
thread_local FolderScaler::DrawContext* g_drawContext = nullptr;

UINT ViewDpi(HWND window) {
    // Resolve at runtime: Windhawk's compiler headers can target a pre-1607
    // Windows SDK even though Explorer is running on a modern Windows build.
    using GetDpiForWindowFn = UINT (WINAPI*)(HWND);
    static const auto getDpi = reinterpret_cast<GetDpiForWindowFn>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    return getDpi ? getDpi(window) : 0;
}

void DebugLog(PCWSTR format, ...) {
    if (g_debugMessagesRemaining.fetch_sub(1, std::memory_order_relaxed) <= 0) return;
    wchar_t body[512];
    va_list args;
    va_start(args, format);
    vswprintf(body, ARRAYSIZE(body), format, args);
    va_end(args);
    Wh_Log(L"[FolderIconScaler] %ls", body);
}

template<typename T>
struct ScopedValue {
    T& target;
    T previous;
    ScopedValue(T& targetValue, T value) : target(targetValue), previous(targetValue) {
        target = value;
    }
    ~ScopedValue() { target = previous; }
};

// Thumbnail selection is item-aware only. A type-only global hook also
// intercepts virtual-item fallbacks and defeats the filesystem check below.
using CImageManager_GetItem_t = HRESULT (*)(void*, void*, REFIID, void**);
CImageManager_GetItem_t CImageManager_GetItem_Original;
using CImageManager_GetThumbnailCutoff_t = unsigned int (*)(void*, void*);
CImageManager_GetThumbnailCutoff_t CImageManager_GetThumbnailCutoff_Original;
unsigned int CImageManager_GetThumbnailCutoff_Hook(void* manager, void* store) {
    // _GetItem is used only in this established call context. Never call it
    // from the paint hook, nor treat virtual folders such as Recycle Bin as
    // filesystem folders.
    // Private _GetItem is optimized for IID_IShellItem in some Windows builds;
    // the signature alone does not mean its REFIID argument is honored.
    ComPtr<IShellItem> item;
    HRESULT itemStatus = CImageManager_GetItem_Original(manager, store, IID_PPV_ARGS(&item));
    return FolderScaler::ItemThumbnailCutoff(SUCCEEDED(itemStatus) ? item.Get() : nullptr,
        g_folders.load(), g_shortcuts.load(), [&]() {
            return CImageManager_GetThumbnailCutoff_Original(manager, store);
        });
}

bool HasClass(HWND window, PCWSTR expected) {
    wchar_t name[64]{};
    return GetClassNameW(window, name, ARRAYSIZE(name)) &&
           wcscmp(name, expected) == 0;
}
bool IsExplorerContentView(HWND window) {
    if (!HasClass(window, L"SysListView32")) return false;
    wchar_t rootClass[64]{};
    HWND root = GetAncestor(window, GA_ROOT);
    if (!GetClassNameW(root, rootClass, ARRAYSIZE(rootClass)) ||
        !FolderScaler::IsExplorerRootClass(rootClass)) return false;
    for (HWND parent = GetParent(window); parent && parent != root;
         parent = GetParent(parent)) {
        if (HasClass(parent, L"SHELLDLL_DefView")) return true;
    }
    return false;
}

LRESULT CALLBACK ListViewSubclass(HWND window, UINT message, WPARAM wParam,
                                 LPARAM lParam, DWORD_PTR) {
    if (g_unloading.load())
        return DefSubclassProc(window, message, wParam, lParam);

    // Use the actual window even with buffered rendering. Expand before
    // BeginPaint so transferred scroll pixels and narrow update strips cannot
    // leave a mixture of native and corrected images in one visible cell.
    if (message == WM_PAINT || message == WM_PRINTCLIENT) {
        bool contentView = IsExplorerContentView(window);
        if (contentView && message == WM_PAINT &&
            (g_folders.load() || g_shortcuts.load()))
            FolderScaler::PrepareStablePaint(window);
        ScopedValue<HWND> scope(g_paintView, contentView ? window : nullptr);
        return DefSubclassProc(window, message, wParam, lParam);
    }
    LRESULT result = DefSubclassProc(window, message, wParam, lParam);
    if (FolderScaler::ShouldInvalidateContentView(IsExplorerContentView(window), message, wParam,
        g_folders.load() || g_shortcuts.load())) {
        InvalidateRect(window, nullptr, TRUE);
    }
    return result;
}

void AttachListView(HWND window) {
    if (g_unloading.load() || !HasClass(window, L"SysListView32")) return;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(window, ListViewSubclass, 0)) {
        DebugLog(L"attached view=%p dpi=%u explorer=%d", window,
                 ViewDpi(window), IsExplorerContentView(window));
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
    LPCWSTR title, DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HINSTANCE instance, LPVOID parameter) {
    HWND window = CreateWindowExW_Original(exStyle, className, title, style,
        x, y, width, height, parent, menu, instance, parameter);
    if (window && g_ready.load()) AttachListView(window);
    return window;
}
using CreateWindowExA_t = decltype(&CreateWindowExA);
CreateWindowExA_t CreateWindowExA_Original;
HWND WINAPI CreateWindowExA_Hook(DWORD exStyle, LPCSTR className,
    LPCSTR title, DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HINSTANCE instance, LPVOID parameter) {
    HWND window = CreateWindowExA_Original(exStyle, className, title, style,
        x, y, width, height, parent, menu, instance, parameter);
    if (window && g_ready.load()) AttachListView(window);
    return window;
}

using ImageListDrawMethod = HRESULT (*)(void*, IMAGELISTDRAWPARAMS*);
enum Slot : size_t {
    CImageListSlot, CSparseSlot, CLayerSlot, CGangSlot, CMultiSlot,
    CContainerSlot, SlotCount
};
struct DrawHook {
    ImageListDrawMethod original = nullptr;
};
DrawHook g_drawHooks[SlotCount];

HRESULT ImageListDraw_Common(size_t slot, void* imageList,
                             IMAGELISTDRAWPARAMS* params) {
    auto original = [&]() {
        return g_drawHooks[slot].original(imageList, params);
    };
    auto bypass = [&]() {
        ScopedValue<bool> guard(g_bypassDrawProcessing, true);
        return original();
    };
    if (g_unloading.load() || g_bypassDrawProcessing || !g_paintView ||
        !params || params->cbSize < sizeof(IMAGELISTDRAWPARAMS) ||
        !imageList || !params->himl || !params->hdcDst || params->i < 0 ||
        params->xBitmap || params->yBitmap ||
        (!g_folders.load() && !g_shortcuts.load())) return bypass();

    // Mask-only, raster-op, glow, animation and similar auxiliary draws are
    // not final icon images. Let their normal rendering proceed unchanged.
    if (params->fState != ILS_NORMAL ||
        (params->fStyle & (ILD_MASK | ILD_ROP))) return bypass();
    // Internal scratch DCs are not final output. Do not alter intermediate
    // masks or colour planes; the outer real-destination draw stays eligible.
    if (g_drawContext && (g_drawContext->dc != params->hdcDst ||
                          g_drawContext->depth >= 16)) return bypass();
    int width = params->cx, height = params->cy;
    if (width <= 0 || height <= 0) {
        int nativeWidth = 0, nativeHeight = 0;
        if (!ImageList_GetIconSize(params->himl, &nativeWidth, &nativeHeight))
            return bypass();
        if (width <= 0) width = nativeWidth;
        if (height <= 0) height = nativeHeight;
    }
    if (width < 1 || height < 1 || width > 1024 || height > 1024 ||
        params->x < -32768 || params->x > 32768 ||
        params->y < -32768 || params->y > 32768) return bypass();
    if (!g_drawContext && !g_completeCellDraw) {
        HRESULT bufferedResult = E_FAIL;
        if (FolderScaler::TryDrawCompleteCell(*params, width, height,
                [&](IMAGELISTDRAWPARAMS* buffered) {
                    ScopedValue<bool> complete(g_completeCellDraw, true);
                    return ImageListDraw_Common(slot, imageList, buffered);
                }, bufferedResult)) return bufferedResult;
    }
    FolderScaler::DrawContext context(g_drawContext, params->hdcDst,
        RECT{params->x, params->y, params->x + width, params->y + height},
        (params->fStyle & ILD_OVERLAYMASK) != 0 || slot == CLayerSlot);
    if (g_completeCellDraw && context.IsOverlayLayer()) {
        HRESULT overlayStatus = E_FAIL;
        FolderScaler::Settings settings{g_minimumCanvasSize.load(), g_smallContentPercent.load(),
            g_scalePercent.load(), g_maximumFillPercent.load(), g_upscaleSmallIcons.load()};
        if (FolderScaler::TryDrawAnchoredOverlay(*params, context, ViewDpi(g_paintView), settings,
            [&](IMAGELISTDRAWPARAMS* isolated) {
                ScopedValue<bool> guard(g_bypassDrawProcessing, true);
                return g_drawHooks[slot].original(imageList, isolated);
            }, overlayStatus)) return overlayStatus;
    }
    ScopedValue<FolderScaler::DrawContext*> scope(g_drawContext, &context);
    RECT visible{};
    if (!FolderScaler::GetCaptureArea(
            params->hdcDst, params->x, params->y, width, height, visible,
            context.IsOverlayLayer() ? 16 : 32))
        return original();

    // Exactly one original draw is made, on the full cell DC when prepared
    // above, otherwise on the unchanged original destination as a fallback.
    // GetDpiForWindow is evaluated for this view on every draw, never taken
    // from the primary display or remembered from Explorer's startup.
    unsigned int dpi = ViewDpi(g_paintView);
    if (!dpi) return original();
    HRESULT originalResult = E_FAIL;
    bool originalCalled = false;
    try {
        FolderScaler::DibSurface before(width, height, params->hdcDst);
        FolderScaler::DibSurface after(width, height, params->hdcDst);
        if (!before || !after) return original();
        std::fill_n(before.pixels(), static_cast<size_t>(width) * height, 0);
        std::fill_n(after.pixels(), static_cast<size_t>(width) * height, 0);
        const int captureWidth = visible.right - visible.left;
        const int captureHeight = visible.bottom - visible.top;
        if (!BitBlt(before.dc(), visible.left, visible.top,
                    captureWidth, captureHeight, params->hdcDst,
                    params->x + visible.left, params->y + visible.top,
                    SRCCOPY)) return original();

        originalCalled = true;
        originalResult = original();
        if (context.childChanged || FAILED(originalResult) ||
            !BitBlt(after.dc(), visible.left, visible.top,
                    captureWidth, captureHeight, params->hdcDst,
                    params->x + visible.left, params->y + visible.top,
                    SRCCOPY) || !GdiFlush())
            return originalResult;
        // GdiFlush is REQUIRED before any access to CreateDIBSection pixels.
        // https://learn.microsoft.com/windows/win32/api/wingdi/nf-wingdi-createdibsection
        FolderScaler::Settings settings{
            g_minimumCanvasSize.load(), g_smallContentPercent.load(),
            g_scalePercent.load(), g_maximumFillPercent.load(),
            g_upscaleSmallIcons.load()
        };
        std::vector<uint32_t> output;
        FolderScaler::Result result;
        bool processed = FolderScaler::ProcessCapturedPixels(
                before.pixels(), after.pixels(), width, height, dpi,
                settings, output, result, &visible, context.IsOverlayLayer());
        if (!processed) {
            FolderScaler::ObserveMainIcon(before.pixels(), after.pixels(), width, height, visible, context);
            return originalResult;
        }

        FolderScaler::DibSurface replacement(width, height, params->hdcDst);
        if (!replacement) return originalResult;
        std::copy(output.begin(), output.end(), replacement.pixels());
        if (!BitBlt(params->hdcDst, params->x + visible.left, params->y + visible.top,
                    captureWidth, captureHeight, replacement.dc(),
                    visible.left, visible.top, SRCCOPY) || !GdiFlush()) {
            // Restore the already rendered original, without replaying it.
            BitBlt(params->hdcDst, params->x + visible.left, params->y + visible.top,
                   captureWidth, captureHeight, after.dc(),
                   visible.left, visible.top, SRCCOPY);
            GdiFlush();
            return originalResult;
        }
        context.MarkChanged();
        context.RememberMainIcon(result);
        DebugLog(L"frame removed view=%p dpi=%u slot=%u i=%d %dx%d -> %dx%d cell=%dx%d nearest=%d",
            g_paintView, dpi, static_cast<unsigned int>(slot), params->i,
            result.width, result.height, result.destinationWidth,
            result.destinationHeight, width, height, result.enlarged);
        return originalResult;
    } catch (...) {
        // In particular, an allocation failure must never make an Explorer
        // icon disappear or call the normal drawing method twice.
        return originalCalled ? originalResult : original();
    }
}

HRESULT CImageList_Draw(void* list, IMAGELISTDRAWPARAMS* p) {
    return ImageListDraw_Common(CImageListSlot, list, p);
}
HRESULT CSparse_Draw(void* list, IMAGELISTDRAWPARAMS* p) {
    return ImageListDraw_Common(CSparseSlot, list, p);
}
HRESULT CLayer_Draw(void* list, IMAGELISTDRAWPARAMS* p) {
    return ImageListDraw_Common(CLayerSlot, list, p);
}
HRESULT CGang_Draw(void* list, IMAGELISTDRAWPARAMS* p) {
    return ImageListDraw_Common(CGangSlot, list, p);
}
HRESULT CMulti_Draw(void* list, IMAGELISTDRAWPARAMS* p) {
    return ImageListDraw_Common(CMultiSlot, list, p);
}
HRESULT CContainer_Draw(void* list, IMAGELISTDRAWPARAMS* p) {
    return ImageListDraw_Common(CContainerSlot, list, p);
}

// comctl32.dll
const WindhawkUtils::SYMBOL_HOOK comctl32Hooks[] = {
    {{L"public: virtual long __cdecl CImageList::Draw(struct _IMAGELISTDRAWPARAMS *)"},
     &g_drawHooks[CImageListSlot].original, CImageList_Draw},
    {{L"public: virtual long __cdecl CSparseImageList::Draw(struct _IMAGELISTDRAWPARAMS *)"},
     &g_drawHooks[CSparseSlot].original, CSparse_Draw},
    {{L"public: virtual long __cdecl CLayerImageList::Draw(struct _IMAGELISTDRAWPARAMS *)"},
     &g_drawHooks[CLayerSlot].original, CLayer_Draw},
    {{L"public: virtual long __cdecl CGangImageList::Draw(struct _IMAGELISTDRAWPARAMS *)"},
     &g_drawHooks[CGangSlot].original, CGang_Draw},
    {{L"public: virtual long __cdecl CMultiImageList::Draw(struct _IMAGELISTDRAWPARAMS *)"},
     &g_drawHooks[CMultiSlot].original, CMulti_Draw},
    {{L"public: virtual long __cdecl CImageListContainerBase::Draw(struct _IMAGELISTDRAWPARAMS *)"},
     &g_drawHooks[CContainerSlot].original, CContainer_Draw},
};
// windows.storage.dll
const WindhawkUtils::SYMBOL_HOOK storageHooks[] = {
    {{L"private: unsigned int __cdecl CImageManager::_GetThumbnailCutoff(struct IItemImageStore *)"},
     &CImageManager_GetThumbnailCutoff_Original, CImageManager_GetThumbnailCutoff_Hook},
    {{L"private: long __cdecl CImageManager::_GetItem(struct IItemImageStore *,struct _GUID const &,void * *)"},
     &CImageManager_GetItem_Original},
};

HMODULE FindLoadedComctl32() {
    HMODULE modules[1024];
    DWORD bytes = 0;
    if (!EnumProcessModules(GetCurrentProcess(), modules, sizeof(modules), &bytes))
        return nullptr;
    HMODULE fallback = nullptr;
    for (size_t i = 0; i < std::min<size_t>(bytes / sizeof(HMODULE), ARRAYSIZE(modules)); ++i) {
        wchar_t path[MAX_PATH]{};
        if (GetModuleFileNameW(modules[i], path, ARRAYSIZE(path)) &&
            StrCmpIW(PathFindFileNameW(path), L"comctl32.dll") == 0) {
            // A cold Explorer process can load both v5 and v6; prefer the
            // side-by-side v6 library which implements these image lists.
            if (StrStrIW(path, L"\\WinSxS\\")) return modules[i];
            fallback = modules[i];
        }
    }
    return fallback;
}

void LoadSettings() {
    g_folders = Wh_GetIntSetting(L"folders") != 0;
    g_shortcuts = Wh_GetIntSetting(L"shortcuts") != 0;
    g_upscaleSmallIcons = Wh_GetIntSetting(L"upscaleSmallIcons") != 0;
    g_minimumCanvasSize = std::clamp(Wh_GetIntSetting(L"minimumCanvasSize"), 32, 1024);
    g_smallContentPercent = std::clamp(Wh_GetIntSetting(L"smallContentPercent"), 20, 90);
    g_scalePercent = std::clamp(Wh_GetIntSetting(L"scalePercent"), 100, 400);
    g_maximumFillPercent = std::clamp(Wh_GetIntSetting(L"maximumFillPercent"), 25, 90);
}

enum class ViewAction { Attach, Repaint, Detach };
BOOL CALLBACK VisitChild(HWND window, LPARAM actionValue) {
    if (!HasClass(window, L"SysListView32")) return TRUE;
    auto action = static_cast<ViewAction>(actionValue);
    if (action == ViewAction::Attach) AttachListView(window);
    if (action == ViewAction::Detach)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, ListViewSubclass);
    // Desktop is also repainted to restore an icon damaged by an older mod,
    // but its pixels never pass through this version's modification path.
    RedrawWindow(window, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE);
    return TRUE;
}
BOOL CALLBACK VisitTop(HWND window, LPARAM action) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId()) EnumChildWindows(window, VisitChild, action);
    return TRUE;
}
void VisitViews(ViewAction action) {
    EnumWindows(VisitTop, static_cast<LPARAM>(action));
}

BOOL Wh_ModInit() {
    LoadSettings();
    HMODULE storage = LoadLibraryExW(L"windows.storage.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE controls = FindLoadedComctl32();
    if (!storage || !controls ||
        !WindhawkUtils::HookSymbols(storage, storageHooks, ARRAYSIZE(storageHooks)) ||
        !WindhawkUtils::HookSymbols(controls, comctl32Hooks, ARRAYSIZE(comctl32Hooks)))
        return FALSE;
    if (!WindhawkUtils::SetFunctionHook(
            CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original) ||
        !WindhawkUtils::SetFunctionHook(
            CreateWindowExA, CreateWindowExA_Hook, &CreateWindowExA_Original))
        return FALSE;
    Wh_Log(L"[FolderIconScaler] v2.8.3 initialized: cell-local clipping during label editing, native virtual items");
    return TRUE;
}
void Wh_ModAfterInit() {
    g_ready = true;
    VisitViews(ViewAction::Attach);
}
void Wh_ModSettingsChanged() {
    LoadSettings();
    g_debugMessagesRemaining = 160;
    VisitViews(ViewAction::Repaint);
}
void Wh_ModBeforeUninit() {
    g_unloading = true;
    g_ready = false;
    VisitViews(ViewAction::Detach);
}
void Wh_ModUninit() {}
#endif  // FOLDER_SCALER_TEST
