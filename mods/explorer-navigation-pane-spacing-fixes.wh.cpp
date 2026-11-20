// ==WindhawkMod==
// @id              explorer-navigation-pane-spacing-fixes
// @name            File Explorer navigation pane: thin divider, even padding
// @description     Tidies up the navigation pane of File Explorer: a thin divider next to the files area, even padding on all sides, faded or hidden pin icons, longer or shorter separators and a vertical offset of the expand arrows
// @version         1.0
// @author          NDruce
// @github          https://github.com/NDruce
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer navigation pane: thin divider, even padding

A set of small adjustments of the navigation pane of File Explorer: the tree
of folders on the left of a folder window. Every adjustment has a setting, and
can be changed or turned off.

## Before and after

The pin icons are set to 10% opacity in this screenshot, and the pointer is on
the folder Data (D:), which shows the expand arrows. The left half is without
the mod, the right half is with the mod.

![Navigation pane, before and after](https://i.imgur.com/WKnigly.png)

## What it does

* **Thin divider.** The strip between the navigation pane and the files area
  is replaced with a line of 1 pixel. By default, the line has the color of
  the line below the command bar, for the light and for the dark theme. The
  color is configurable. The navigation pane can still be resized by dragging
  the divider.
* **Even padding.** The gap to the left of the items of the navigation pane is
  made the same as the gap to the right of them. The size is configurable. A
  space above and below the contents of the navigation pane can be added too.
* **Pin icons.** The pin icons to the right of the pinned folders can be made
  less visible, with a configurable opacity, or hidden.
* **Separators.** The lines which separate the groups of the navigation pane
  can be made longer or shorter. They can go from edge to edge.
* **Expand arrows.** The arrows can be moved vertically.

## Defaults

The divider, the padding, the pin icons and the offset of the arrows are
adjusted by default. To get the stock look of a part back, use these values:

* Thin divider: off.
* Padding of the navigation pane: -1.
* Vertical padding of the navigation pane: 0.
* Hide the pin icons: off, with the opacity of 0.
* Vertical offset of the expand arrows: 0.

The adjustment of the separators is turned off by default.

## Other mods

The thin divider and the padding have no counterpart in other mods. For the
rest, there are mods which do related things in a different way:

* **Explorer TreeItem Tweaker** and **Classic Explorer Treeview** can hide the
  pin icons. This mod can also fade them instead of hiding them.
* **Explorer TreeLine Killer** hides the separators and changes the spacing
  around them. This mod keeps the separators and changes their length.
* **Explorer Navigation Tree Offset** shifts the tree and can remove the
  expand arrows. This mod moves the arrows vertically by a pixel or two.

The mods can be used together. A part of this mod which is set to its stock
value does nothing at all, so if another mod takes care of the pin icons, the
separators or the arrows, set that part to the stock value here, as listed
above. The combinations with those mods weren't tested.

The files area of a folder window is handled by a separate mod, **File
Explorer List view indent fix and files area spacing**.

## Scaling and themes

The padding and the side gaps of the separators follow the display scaling.
The offset of the expand arrows is a small correction, and is applied in
physical pixels regardless of the scaling.

The mod was only tested with a single monitor. With several monitors of
different scaling, the side gaps of the separators can be off on a monitor
whose scaling differs from the one of the main monitor. A folder window which
was moved to such a monitor can also keep the adjusted padding after the mod
is turned off, until it's reopened.

The automatic color of the thin divider is chosen when a folder window is
opened. Windows which are already open when the theme changes keep the old
color until they're reopened. The divider is left alone in the high contrast
mode.

## Other programs

By default, the mod only applies to File Explorer (`explorer.exe`). That's the
recommended setup, and nothing has to be configured for it.

The same navigation pane is used by the Open/Save dialogs of other programs.
To adjust it there too, add the programs to the custom process inclusion list
in the Advanced tab of the mod, by the name of the executable file, for
example `notepad.exe`. `explorer.exe` doesn't have to be added, it's always
included.

It's also possible to add `*`, which means all programs. The mod is then
loaded into every process, including the ones which never show a file dialog,
so adding only the programs you need is the lighter option.

Only 64-bit programs are supported. In a program other than File Explorer, the
mod does nothing until the program loads `explorerframe.dll`, which usually
happens when a file dialog is opened for the first time. From then on, the
vertical offset of the expand arrows applies to all tree views of that
program.

## How it works

The divider and the padding are properties of DirectUI elements of the frame
of a folder window. They're changed when a frame is created, and the windows
which are already open are adjusted when the mod is loaded. The functions
which are exported by `dui70.dll` are used for that. The adjustments are only
made to elements which have the expected stock values.

The pin icons are the state images of the tree of the navigation pane. The mod
gives the tree a copy of its state image list with faded images, and leaves
the list of File Explorer itself untouched.

The separators are adjusted while File Explorer draws them, which relies on
the debug symbols of `explorerframe.dll`. If that part can't be set up on a
Windows build, the rest of the mod still works. The expand arrows are moved
while they're drawn with the theme of a tree view.

No files are modified. When the mod is turned off, the stock look is restored.

Tested on Windows 11 build 26300 (26H2 preview) at 125% scaling, with the
light and the dark theme.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- navPaneDivider: true
  $name: Thin divider between the navigation pane and the files area
  $description: >-
    Replaces the strip between the navigation pane and the files area with a
    line of 1 pixel, of the same color as the line below the command bar. The
    navigation pane can still be resized by dragging the divider.
- navPaneDividerColor: ""
  $name: Color of the thin divider
  $description: >-
    In the #RRGGBB format, for example #3A3A3A. Leave empty to use a color
    which matches the light or the dark theme. Used if the thin divider is
    turned on.
- navPanePadding: 1
  $name: Padding of the navigation pane
  $description: >-
    The gap to the left and to the right of the items of the navigation pane,
    the same on both sides, from 0 to 32. The stock gaps are 2 on the left and
    1 on the right. Set to -1 to keep the stock gaps.
- navPanePaddingVertical: 6
  $name: Vertical padding of the navigation pane
  $description: >-
    The space above and below the contents of the navigation pane, up to 64.
    Set to 0 to keep the stock look.
- navPinIconsHide: true
  $name: Hide the pin icons
  $description: >-
    Hides the pin icons which are shown to the right of the pinned folders.
- navPinIconOpacity: 0
  $name: Opacity of the pin icons
  $description: >-
    In percent, from 1 to 99. Used if the pin icons aren't hidden. Set to 0 to
    keep the stock look of the pin icons.
- navSeparatorInset: 0
  $name: Side gaps of the separators
  $description: >-
    Added to the gaps on the left and on the right of the horizontal lines
    which separate the groups of the navigation pane, from -64 to 64. The
    stock gap is 8. A negative value makes the lines longer: with -8, they go
    from edge to edge of the navigation pane. A positive value makes them
    shorter. Set to 0 to keep the stock gaps.
- navArrowOffset: -1
  $name: Vertical offset of the expand arrows
  $description: >-
    In pixels, from -8 to 8. A negative value moves the arrows up. Applies to
    all tree views of the process, and takes effect when the tree is redrawn.
    Set to 0 to keep the stock position.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>

#include <algorithm>
#include <atomic>
#include <map>
#include <vector>

static struct {
    bool navPaneDivider;
    // Zero for the color of the theme.
    ULONG navPaneDividerColor;
    // Negative to keep the stock padding.
    int navPanePadding;
    int navPanePaddingVertical;
    bool navPinIconsHide;
    int navPinIconOpacity;
    // Added to the stock side gaps of the separators, zero to keep them.
    int navSeparatorInset;
    int navArrowOffset;
} g_settings;

static HMODULE g_dui70Module;
static HMODULE g_explorerFrameModule;
static HMODULE g_shell32Module;
static UINT g_runFromWindowThreadMsg;

// Set once all the functions below are resolved and the hooks are set. Until
// then, the windows must not be touched.
static std::atomic<bool> g_initialized;
// In a program other than File Explorer, the hooks are only set once the
// program loads explorerframe.dll by itself.
static bool g_lateInit;
static std::atomic<bool> g_lateInitStarted;
static std::atomic<int> g_lateInitRefCount;
static std::atomic<bool> g_unloading;

// dui70.dll exports.
using PropertyProc_t = void*(WINAPI*)();
static void*(WINAPI* Element_GetParent)(void* pThis);
static ATOM(WINAPI* Element_GetID)(void* pThis);
static void*(WINAPI* Element_FindDescendent)(void* pThis, ATOM id);
static const RECT*(WINAPI* Element_GetPadding)(void* pThis, void** ppValue);
static int(WINAPI* Element_GetWidth)(void* pThis);
static HRESULT(WINAPI* Element_SetWidth)(void* pThis, int width);
static HRESULT(WINAPI* Element_SetPadding)(void* pThis, int l, int t, int r, int b);
static HRESULT(WINAPI* Element_RemoveLocalValue)(void* pThis, PropertyProc_t prop);
static HRESULT(WINAPI* Element_SetBorderThickness)(void* pThis, int l, int t, int r, int b);
static HRESULT(WINAPI* Element_SetBorderColor)(void* pThis, ULONG color);
static HRESULT(WINAPI* Element_SetBackgroundColor)(void* pThis, ULONG color);
static PropertyProc_t Element_BorderThicknessProp;
static PropertyProc_t Element_BorderColorProp;
static PropertyProc_t Element_BackgroundProp;
static void(WINAPI* Value_Release)(void* pThis);
static int(WINAPI* DUIXmlParser_ScaleRelativePixels)(void* pThis, int value);
static HRESULT(WINAPI* DUIXmlParser_CreateElement_Original)(void* pThis, PCWSTR resid, void* substitute, void* parent, DWORD* deferCookie, void** element);

static bool StartsWith(PCWSTR str, PCWSTR prefix) {
    return wcsncmp(str, prefix, wcslen(prefix)) == 0;
}

static RECT GetElementPadding(void* element) {
    RECT result{};
    void* value = nullptr;
    if (const RECT* rc = Element_GetPadding(element, &value)) {
        result = *rc;
    }

    if (value) {
        Value_Release(value);
    }

    return result;
}

// The colors of the line below the command bar, in the DirectUI format.
constexpr ULONG kDividerColorDark = 0xFF3A3A3A;
constexpr ULONG kDividerColorLight = 0xFFE5E5E5;

static bool IsDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

static bool IsHighContrast() {
    HIGHCONTRASTW highContrast{.cbSize = sizeof(highContrast)};
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, 0) &&
           (highContrast.dwFlags & HCF_HIGHCONTRASTON);
}

// DirectUI element IDs are atoms from the local atom table. An atom doesn't
// exist until the markup which uses it is loaded. It's deleted when the last
// user of it is gone, for example when the last folder window is closed, and
// it can get a different value when it's added again. So the atoms must be
// looked up every time, and must not be remembered.
static ATOM GetNavPaneSizerAtom() {
    return FindAtomW(L"PageSpaceControlSizer");
}

// The strip between the navigation pane and the files area consists of the
// sizer of the navigation pane, which is drawn with a theme part, and of a
// pixel of the background of the navigation pane host next to it. Makes both
// transparent and draws a line of 1 pixel as the left border of the sizer, or
// restores the stock look. The size of the sizer isn't changed, so that it
// stays as easy to drag as before.
static void ApplyNavPaneDivider(void* sizer, bool apply) {
    ATOM hostAtom = FindAtomW(L"ProperTreeHost");
    void* parent = Element_GetParent(sizer);
    void* host = parent && hostAtom ? Element_FindDescendent(parent, hostAtom) : nullptr;
    if (apply) {
        if (IsHighContrast()) {
            return;
        }

        Element_SetBorderThickness(sizer, 1, 0, 0, 0);
        ULONG color = g_settings.navPaneDividerColor;
        if (!color) {
            color = IsDarkMode() ? kDividerColorDark : kDividerColorLight;
        }

        Element_SetBorderColor(sizer, color);
        Element_SetBackgroundColor(sizer, 0);
        if (host) {
            Element_SetBackgroundColor(host, 0);
        }
    } else {
        // The stock values come from the style sheet.
        Element_RemoveLocalValue(sizer, Element_BorderThicknessProp);
        Element_RemoveLocalValue(sizer, Element_BorderColorProp);
        Element_RemoveLocalValue(sizer, Element_BackgroundProp);
        if (host) {
            Element_RemoveLocalValue(host, Element_BackgroundProp);
        }
    }
}

// The stock padding of the navigation pane host and the stock width of its
// sizer, which covers a part of the right padding.
constexpr int kNavPanePaddingLeft = 2;
constexpr int kNavPanePaddingRight = 4;
constexpr int kNavPaneSizerWidth = 3;

// Sets the configured gap on both sides of the items of the navigation pane
// and the configured space above and below its contents, or restores the
// stock padding. On the right, the sizer covers a part of the padding, so the
// padding is larger by the width of the sizer. Only touches the padding if it
// has the stock values or the adjusted values. A new element which isn't in a
// window yet holds the values for 100% scaling.
static void ApplyNavPanePadding(void* sizer, int dpi, bool apply, bool refresh) {
    ATOM hostAtom = FindAtomW(L"ProperTreeHost");
    void* parent = Element_GetParent(sizer);
    void* host = parent && hostAtom ? Element_FindDescendent(parent, hostAtom) : nullptr;
    if (!host) {
        return;
    }

    RECT rawStock{kNavPanePaddingLeft, 0, kNavPanePaddingRight, 0};
    RECT stock{MulDiv(kNavPanePaddingLeft, dpi, 96), 0, MulDiv(kNavPanePaddingRight, dpi, 96), 0};
    RECT adjusted = stock;
    if (g_settings.navPanePadding >= 0) {
        int gap = MulDiv(g_settings.navPanePadding, dpi, 96);
        adjusted.left = gap;
        adjusted.right = gap + MulDiv(kNavPaneSizerWidth, dpi, 96);
    }

    adjusted.top = adjusted.bottom = MulDiv(g_settings.navPanePaddingVertical, dpi, 96);
    if (EqualRect(&adjusted, &stock)) {
        return;
    }

    RECT padding = GetElementPadding(host);
    if (apply && (EqualRect(&padding, &stock) || EqualRect(&padding, &rawStock))) {
        Element_SetPadding(host, adjusted.left, adjusted.top, adjusted.right, adjusted.bottom);
        if (adjusted.top > 0) {
            // The background of the host is darker than the navigation pane.
            Element_SetBackgroundColor(host, 0);
        }
    } else if (!apply && EqualRect(&padding, &adjusted)) {
        Element_SetPadding(host, stock.left, stock.top, stock.right, stock.bottom);
        if (adjusted.top > 0 && !g_settings.navPaneDivider) {
            Element_RemoveLocalValue(host, Element_BackgroundProp);
        }
    } else {
        return;
    }

    // The window of the navigation pane is only moved when the size of the
    // host changes. Change and restore the width of the pane.
    if (refresh) {
        int width = Element_GetWidth(parent);
        if (width > 0) {
            Element_SetWidth(parent, width + 1);
            Element_SetWidth(parent, width);
        }
    }
}

// The separators between the groups of the navigation pane.
//
// A separator isn't an item. The first item of a group is two rows tall, and
// CNscTree::DrawDivider draws a line in its upper row with Polyline. The ends
// of the line are moved on the way, without relying on the layout of
// CNscTree. The height of a separator can't be changed that way: the height
// of a tree item is a whole number of rows.
static void(WINAPI* CNscTree_DrawDivider_Original)(void* pThis, HDC hdc, HTREEITEM item);
static LRESULT(WINAPI* SendMessageW_Original)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL(WINAPI* Polyline_Original)(HDC hdc, const POINT* points, int count);

// Set while a separator is being drawn by the current thread.
static thread_local bool t_drawingNavSeparator;

// The pin icons of the navigation pane.
//
// A pin icon is the state image of a tree item, which the tree draws on the
// right. The state image list of the tree is replaced with a copy in which
// the images are faded or fully transparent. The list of File Explorer itself
// isn't touched: it stays the owner of its list, and gets its own list back
// when it asks for it.
struct NavStateImages {
    HIMAGELIST original;
    HIMAGELIST adjusted;
};

static SRWLOCK g_navStateImagesLock = SRWLOCK_INIT;
static std::map<HWND, NavStateImages> g_navStateImages;

// The opacity of the pin icons in percent, or a negative value to keep them
// as they are.
static int GetNavPinIconOpacity() {
    if (g_settings.navPinIconsHide) {
        return 0;
    }

    return g_settings.navPinIconOpacity > 0 ? g_settings.navPinIconOpacity : -1;
}

static bool IsNavPaneTree(HWND hWnd) {
    WCHAR className[32];
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) && wcscmp(className, L"SysTreeView32") == 0 &&
           GetClassNameW(GetParent(hWnd), className, ARRAYSIZE(className)) &&
           wcscmp(className, L"NamespaceTreeControl") == 0;
}

// Sends a message without the adjustments of the hook below.
static LRESULT SendMessageRaw(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return SendMessageW_Original ? SendMessageW_Original(hWnd, msg, wParam, lParam)
                                 : SendMessageW(hWnd, msg, wParam, lParam);
}

// Creates a copy of an icon with the given opacity.
static HICON CreateFadedIcon(HICON icon, int opacity) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) {
        return nullptr;
    }

    HICON result = nullptr;
    BITMAP bm{};
    if (info.hbmColor && GetObjectW(info.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 && bm.bmHeight > 0 &&
        bm.bmWidth <= 256 && bm.bmHeight <= 256) {
        int width = bm.bmWidth;
        int height = bm.bmHeight;
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        std::vector<DWORD> color(width * height);
        std::vector<DWORD> mask(width * height);
        HDC dc = GetDC(nullptr);
        BITMAPINFO biColor = bi;
        BITMAPINFO biMask = bi;
        bool hasColor = GetDIBits(dc, info.hbmColor, 0, height, color.data(), &biColor, DIB_RGB_COLORS) == height;
        bool hasMask = info.hbmMask &&
                       GetDIBits(dc, info.hbmMask, 0, height, mask.data(), &biMask, DIB_RGB_COLORS) == height;

        void* bits = nullptr;
        HBITMAP newColor = hasColor ? CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
        ReleaseDC(nullptr, dc);

        if (newColor && bits) {
            bool hasAlpha = false;
            for (DWORD pixel : color) {
                if (pixel >> 24) {
                    hasAlpha = true;
                    break;
                }
            }

            // The pixels which end up fully transparent are marked in the
            // mask, as an icon without any alpha is drawn by its mask.
            int maskStride = ((width + 15) / 16) * 2;
            std::vector<BYTE> newMaskBits(maskStride * height, 0);
            DWORD* out = (DWORD*)bits;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    DWORD pixel = color[y * width + x];
                    int alpha = hasAlpha ? (int)(pixel >> 24) : (hasMask && (mask[y * width + x] & 0xFFFFFF) ? 0 : 255);
                    alpha = alpha * opacity / 100;
                    if (alpha <= 0) {
                        out[y * width + x] = 0;
                        newMaskBits[y * maskStride + x / 8] |= (BYTE)(0x80 >> (x % 8));
                    } else {
                        out[y * width + x] = ((DWORD)alpha << 24) | (pixel & 0xFFFFFF);
                    }
                }
            }

            HBITMAP newMask = CreateBitmap(width, height, 1, 1, newMaskBits.data());
            if (newMask) {
                ICONINFO newInfo{};
                newInfo.fIcon = TRUE;
                newInfo.hbmMask = newMask;
                newInfo.hbmColor = newColor;
                result = CreateIconIndirect(&newInfo);
                DeleteObject(newMask);
            }
        }

        if (newColor) {
            DeleteObject(newColor);
        }
    }

    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }

    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }

    return result;
}

// Creates a copy of an image list in which all the images have the given
// opacity.
static HIMAGELIST CreateFadedImageList(HIMAGELIST source, int opacity) {
    HIMAGELIST copy = ImageList_Duplicate(source);
    if (!copy) {
        return nullptr;
    }

    int count = ImageList_GetImageCount(copy);
    for (int i = 0; i < count; i++) {
        HICON icon = ImageList_GetIcon(source, i, ILD_TRANSPARENT);
        if (!icon) {
            continue;
        }

        if (HICON faded = CreateFadedIcon(icon, opacity)) {
            ImageList_ReplaceIcon(copy, i, faded);
            DestroyIcon(faded);
        }

        DestroyIcon(icon);
    }

    return copy;
}

// Replaces the state image list of a navigation pane tree with an adjusted
// copy, or restores the list of File Explorer. Called from the thread of the
// tree.
static void ApplyNavPinIcons(HWND tree, bool apply) {
    NavStateImages previous{};
    AcquireSRWLockExclusive(&g_navStateImagesLock);
    if (auto it = g_navStateImages.find(tree); it != g_navStateImages.end()) {
        previous = it->second;
        g_navStateImages.erase(it);
    }
    ReleaseSRWLockExclusive(&g_navStateImagesLock);

    HIMAGELIST current = (HIMAGELIST)SendMessageRaw(tree, TVM_GETIMAGELIST, TVSIL_STATE, 0);
    bool changed = false;
    if (previous.adjusted) {
        if (current == previous.adjusted) {
            SendMessageRaw(tree, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)previous.original);
            current = previous.original;
            changed = true;
        }

        ImageList_Destroy(previous.adjusted);
    }

    int opacity = apply ? GetNavPinIconOpacity() : -1;
    if (opacity >= 0 && current) {
        if (HIMAGELIST adjusted = CreateFadedImageList(current, opacity)) {
            SendMessageRaw(tree, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)adjusted);
            AcquireSRWLockExclusive(&g_navStateImagesLock);
            g_navStateImages[tree] = NavStateImages{current, adjusted};
            ReleaseSRWLockExclusive(&g_navStateImagesLock);
            changed = true;
        }
    }

    if (changed) {
        InvalidateRect(tree, nullptr, TRUE);
    }
}

// File Explorer sets or retrieves the state image list of a navigation pane
// tree.
static LRESULT NavStateImageListMessage(HWND hWnd, UINT msg, LPARAM lParam) {
    if (msg == TVM_GETIMAGELIST) {
        LRESULT result = SendMessageW_Original(hWnd, msg, TVSIL_STATE, lParam);
        AcquireSRWLockExclusive(&g_navStateImagesLock);
        if (auto it = g_navStateImages.find(hWnd); it != g_navStateImages.end() && (HIMAGELIST)result == it->second.adjusted) {
            result = (LRESULT)it->second.original;
        }
        ReleaseSRWLockExclusive(&g_navStateImagesLock);
        return result;
    }

    HIMAGELIST newList = (HIMAGELIST)lParam;
    int opacity = g_unloading ? -1 : GetNavPinIconOpacity();
    HIMAGELIST adjusted = newList && opacity >= 0 ? CreateFadedImageList(newList, opacity) : nullptr;
    LRESULT result = SendMessageW_Original(hWnd, msg, TVSIL_STATE, (LPARAM)(adjusted ? adjusted : newList));

    NavStateImages previous{};
    AcquireSRWLockExclusive(&g_navStateImagesLock);
    if (auto it = g_navStateImages.find(hWnd); it != g_navStateImages.end()) {
        previous = it->second;
        g_navStateImages.erase(it);
    }

    if (adjusted) {
        // Destroy the copies of the trees which are gone. Nothing else does
        // that if a tree is destroyed with its state image list still set.
        std::erase_if(g_navStateImages, [](const auto& entry) {
            if (IsWindow(entry.first)) {
                return false;
            }

            ImageList_Destroy(entry.second.adjusted);
            return true;
        });

        g_navStateImages[hWnd] = NavStateImages{newList, adjusted};
    }
    ReleaseSRWLockExclusive(&g_navStateImagesLock);

    if (previous.adjusted) {
        // The previous list is returned to the caller, which might destroy it.
        if ((HIMAGELIST)result == previous.adjusted) {
            result = (LRESULT)previous.original;
        }

        ImageList_Destroy(previous.adjusted);
    }

    return result;
}

// Destroys the copies which are left. A copy which is still used by a tree
// is left alone. No message is sent while the lock is held, as the thread of
// the tree might be waiting for the lock.
static void FreeNavStateImages() {
    std::map<HWND, NavStateImages> images;
    AcquireSRWLockExclusive(&g_navStateImagesLock);
    images.swap(g_navStateImages);
    ReleaseSRWLockExclusive(&g_navStateImagesLock);

    for (const auto& [tree, entry] : images) {
        // The handle of a tree which is gone might belong to another window
        // by now.
        DWORD processId = 0;
        bool ownTree = IsWindow(tree) && GetWindowThreadProcessId(tree, &processId) &&
                       processId == GetCurrentProcessId() && IsNavPaneTree(tree);
        if (!ownTree || (HIMAGELIST)SendMessageRaw(tree, TVM_GETIMAGELIST, TVSIL_STATE, 0) != entry.adjusted) {
            ImageList_Destroy(entry.adjusted);
        }
    }
}

static void WINAPI CNscTree_DrawDivider_Hook(void* pThis, HDC hdc, HTREEITEM item) {
    t_drawingNavSeparator = true;
    CNscTree_DrawDivider_Original(pThis, hdc, item);
    t_drawingNavSeparator = false;
}

static LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if ((msg == TVM_SETIMAGELIST || msg == TVM_GETIMAGELIST) && wParam == TVSIL_STATE && IsNavPaneTree(hWnd)) {
        return NavStateImageListMessage(hWnd, msg, lParam);
    }

    return SendMessageW_Original(hWnd, msg, wParam, lParam);
}

static BOOL WINAPI Polyline_Hook(HDC hdc, const POINT* points, int count) {
    if (!t_drawingNavSeparator || !g_settings.navSeparatorInset || count != 2 || !points ||
        points[0].y != points[1].y) {
        return Polyline_Original(hdc, points, count);
    }

    // The line is drawn in the pixels of the device. If the tree is drawn
    // through a memory device context, the window isn't known, and the
    // scaling of the system is used.
    HWND window = WindowFromDC(hdc);
    int dpi = window ? (int)GetDpiForWindow(window) : 0;
    if (dpi <= 0) {
        dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    }

    int inset = MulDiv(g_settings.navSeparatorInset, dpi, 96);
    POINT adjusted[2] = {points[0], points[1]};
    if (adjusted[0].x > adjusted[1].x) {
        std::swap(adjusted[0], adjusted[1]);
    }

    adjusted[0].x += inset;
    adjusted[1].x -= inset;
    if (adjusted[0].x >= adjusted[1].x) {
        return TRUE;
    }

    return Polyline_Original(hdc, adjusted, 2);
}

// uxtheme.dll
constexpr int kTVP_TREEITEM = 1;
constexpr int kTVP_GLYPH = 2;
constexpr int kTVP_HOTGLYPH = 4;
static BOOL(WINAPI* Uxtheme_IsThemePartDefined)(HANDLE hTheme, int partId, int stateId);
// Exported by ordinal only.
using GetThemeClass_t = HRESULT(WINAPI*)(HANDLE hTheme, LPWSTR buffer, int size);
static GetThemeClass_t Uxtheme_GetThemeClass;
static HRESULT(WINAPI* DrawThemeBackground_Original)(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const RECT* clipRect);
static HRESULT(WINAPI* DrawThemeBackgroundEx_Original)(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const void* options);

// Returns true for the expand arrow of a tree view, such as the navigation
// pane.
static bool IsTreeViewGlyph(HANDLE hTheme, int partId, const RECT* rect) {
    if (!g_settings.navArrowOffset || (partId != kTVP_GLYPH && partId != kTVP_HOTGLYPH) || !rect) {
        return false;
    }

    // The theme class is identified by its name, such as "Explorer::TreeView"
    // or "DarkMode_Explorer::TreeView". Other classes use the same part
    // numbers for other things, for example for the buttons of up-down and
    // pager controls.
    if (Uxtheme_GetThemeClass) {
        WCHAR themeClass[64] = {};
        if (SUCCEEDED(Uxtheme_GetThemeClass(hTheme, themeClass, ARRAYSIZE(themeClass)))) {
            themeClass[ARRAYSIZE(themeClass) - 1] = L'\0';
            constexpr size_t kSuffixLength = 8;
            size_t length = wcslen(themeClass);
            if (length > 0 &&
                (length < kSuffixLength || _wcsicmp(themeClass + length - kSuffixLength, L"TreeView") != 0)) {
                return false;
            }
        }
    }

    // Also check the parts: a tree view theme has parts up to 4. If the name
    // of the class can't be retrieved, that's the only check.
    return Uxtheme_IsThemePartDefined(hTheme, kTVP_TREEITEM, 0) &&
           Uxtheme_IsThemePartDefined(hTheme, kTVP_HOTGLYPH, 0) &&
           !Uxtheme_IsThemePartDefined(hTheme, kTVP_HOTGLYPH + 1, 0);
}

// DrawThemeBackground might call DrawThemeBackgroundEx on some builds. The
// offset is applied once, by the outer call.
static thread_local bool t_inDrawThemeBackground;

static HRESULT WINAPI DrawThemeBackground_Hook(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const RECT* clipRect) {
    if (!t_inDrawThemeBackground && IsTreeViewGlyph(hTheme, partId, rect)) {
        RECT rc = *rect;
        OffsetRect(&rc, 0, g_settings.navArrowOffset);
        t_inDrawThemeBackground = true;
        HRESULT result = DrawThemeBackground_Original(hTheme, hdc, partId, stateId, &rc, clipRect);
        t_inDrawThemeBackground = false;
        return result;
    }

    return DrawThemeBackground_Original(hTheme, hdc, partId, stateId, rect, clipRect);
}

static HRESULT WINAPI DrawThemeBackgroundEx_Hook(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const void* options) {
    if (!t_inDrawThemeBackground && IsTreeViewGlyph(hTheme, partId, rect)) {
        RECT rc = *rect;
        OffsetRect(&rc, 0, g_settings.navArrowOffset);
        return DrawThemeBackgroundEx_Original(hTheme, hdc, partId, stateId, &rc, options);
    }

    return DrawThemeBackgroundEx_Original(hTheme, hdc, partId, stateId, rect, options);
}

// The frame of a folder window is created from a template. Adjusts its
// navigation pane right when it's created.
static HRESULT WINAPI DUIXmlParser_CreateElement_Hook(void* pThis, PCWSTR resid, void* substitute, void* parent, DWORD* deferCookie, void** element) {
    HRESULT hr = DUIXmlParser_CreateElement_Original(pThis, resid, substitute, parent, deferCookie, element);
    if (FAILED(hr) || !element || !*element || !resid || g_unloading) {
        return hr;
    }

    bool divider = g_settings.navPaneDivider;
    bool padding = g_settings.navPanePadding >= 0 || g_settings.navPanePaddingVertical > 0;

    // The items, the items areas and the cells of the files area are created
    // all the time, and aren't of interest.
    if ((!divider && !padding) || StartsWith(resid, L"item.") || StartsWith(resid, L"collection.") ||
        StartsWith(resid, L"property.")) {
        return hr;
    }

    ATOM atom = GetNavPaneSizerAtom();
    if (!atom) {
        return hr;
    }

    void* sizer = Element_GetID(*element) == atom ? *element : Element_FindDescendent(*element, atom);
    if (!sizer) {
        return hr;
    }

    if (divider) {
        ApplyNavPaneDivider(sizer, true);
    }

    if (padding) {
        int dpi = DUIXmlParser_ScaleRelativePixels(pThis, 96);
        if (dpi > 0) {
            ApplyNavPanePadding(sizer, dpi, true, false);
        }
    }

    return hr;
}

// Returns the root element of a DirectUIHWND window.
static void* GetRootElement(HWND hWnd) {
    // A DirectUIHWND window keeps a pointer to its root element, or null if
    // there's no element. It's read from the thread of the window.
    void* element = (void*)GetWindowLongPtrW(hWnd, 0);
    if (!element) {
        return nullptr;
    }

    // Make sure that it's indeed an element.
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            *(PCWSTR*)element, &module) ||
        (module != g_explorerFrameModule && module != g_dui70Module && module != g_shell32Module)) {
        return nullptr;
    }

    return element;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(HWND hWnd, void* param);

struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t proc;
    HWND hWnd;
    void* procParam;
};

static LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runFromWindowThreadMsg) {
            auto* param = (RunFromWindowThreadParam*)cwp->lParam;
            param->proc(param->hWnd, param->procParam);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(hWnd, procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHookProc, nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParam param{proc, hWnd, procParam};
    SendMessageW(hWnd, g_runFromWindowThreadMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

// Applies or restores the adjustments of the existing windows.
struct ApplyParam {
    bool active;
    bool divider;
    bool navPadding;
};

static void WINAPI ApplyToNavTreeWindow(HWND hWnd, void* param) {
    ApplyNavPinIcons(hWnd, ((const ApplyParam*)param)->active);
}

// The frame of a folder window: the navigation pane, the files area and the
// divider between them.
static void WINAPI ApplyToFrameWindow(HWND hWnd, void* param) {
    const ApplyParam& applyParam = *(const ApplyParam*)param;
    ATOM atom = GetNavPaneSizerAtom();
    void* element = (applyParam.divider || applyParam.navPadding) && atom ? GetRootElement(hWnd) : nullptr;
    void* sizer = element ? Element_FindDescendent(element, atom) : nullptr;
    if (!sizer) {
        return;
    }

    if (applyParam.divider) {
        ApplyNavPaneDivider(sizer, applyParam.active);
    }

    if (applyParam.navPadding) {
        ApplyNavPanePadding(sizer, GetDpiForWindow(hWnd), applyParam.active, true);
    }
}

// Applies the adjustments to the existing windows, or restores the stock
// values if not active.
static void ApplyToExistingWindows(bool active) {
    if (!g_initialized) {
        return;
    }

    ApplyParam enumParam{
        .active = active,
        .divider = g_settings.navPaneDivider,
        .navPadding = g_settings.navPanePadding >= 0 || g_settings.navPanePaddingVertical > 0,
    };

    auto enumChildProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        if (IsNavPaneTree(hWnd)) {
            RunFromWindowThread(hWnd, ApplyToNavTreeWindow, (void*)lParam);
            return TRUE;
        }

        WCHAR className[32];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) && wcscmp(className, L"DirectUIHWND") == 0 &&
            GetClassNameW(GetParent(hWnd), className, ARRAYSIZE(className)) &&
            wcscmp(className, L"DUIViewWndClassName") == 0) {
            RunFromWindowThread(hWnd, ApplyToFrameWindow, (void*)lParam);
        }

        return TRUE;
    };

    auto enumProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD processId = 0;
        if (GetWindowThreadProcessId(hWnd, &processId) && processId == GetCurrentProcessId()) {
            auto enumChildProc = (WNDENUMPROC)((void**)lParam)[0];
            EnumChildWindows(hWnd, enumChildProc, (LPARAM)((void**)lParam)[1]);
        }

        return TRUE;
    };

    void* params[] = {(void*)(WNDENUMPROC)enumChildProc, &enumParam};
    EnumWindows(enumProc, (LPARAM)params);
}

static void LoadSettings() {
    g_settings.navPaneDivider = Wh_GetIntSetting(L"navPaneDivider");

    // #RRGGBB, converted to the DirectUI format: opaque, red in the low byte.
    g_settings.navPaneDividerColor = 0;
    auto dividerColor = WindhawkUtils::StringSetting::make(L"navPaneDividerColor");
    PCWSTR hex = dividerColor;
    while (*hex == L' ' || *hex == L'#') {
        hex++;
    }

    PWSTR hexEnd = nullptr;
    ULONG rgb = wcstoul(hex, &hexEnd, 16);
    while (*hexEnd == L' ') {
        hexEnd++;
    }

    if (hexEnd - hex >= 6 && wcsspn(hex, L"0123456789abcdefABCDEF") == 6 && !*hexEnd) {
        g_settings.navPaneDividerColor = 0xFF000000 | ((rgb & 0xFF) << 16) | (rgb & 0xFF00) | ((rgb >> 16) & 0xFF);
    }

    // Values out of the range are brought to the nearest value of the range.
    g_settings.navPanePadding = std::clamp(Wh_GetIntSetting(L"navPanePadding"), -1, 32);
    g_settings.navPanePaddingVertical = std::clamp(Wh_GetIntSetting(L"navPanePaddingVertical"), 0, 64);
    g_settings.navPinIconsHide = Wh_GetIntSetting(L"navPinIconsHide");
    g_settings.navPinIconOpacity = std::clamp(Wh_GetIntSetting(L"navPinIconOpacity"), 0, 99);
    g_settings.navSeparatorInset = std::clamp(Wh_GetIntSetting(L"navSeparatorInset"), -64, 64);
    g_settings.navArrowOffset = std::clamp(Wh_GetIntSetting(L"navArrowOffset"), -8, 8);
}

template <typename T>
static bool GetProc(HMODULE module, PCSTR name, T* proc) {
    *proc = (T)GetProcAddress(module, name);
    if (!*proc) {
        Wh_Log(L"Missing export: %S", name);
        return false;
    }

    return true;
}

// The modules which were loaded by the mod, to release the references when
// the mod is unloaded or when setting it up fails.
static HMODULE g_loadedModules[3];

static HMODULE LoadSystemModule(PCWSTR name) {
    HMODULE module = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module) {
        for (HMODULE& slot : g_loadedModules) {
            if (!slot) {
                slot = module;
                break;
            }
        }
    }

    return module;
}

static void FreeLoadedModules() {
    for (HMODULE& slot : g_loadedModules) {
        if (slot) {
            FreeLibrary(slot);
            slot = nullptr;
        }
    }
}

// Resolves the functions and sets the hooks. The modules are loaded if they
// aren't loaded yet.
static BOOL SetupHooksImpl() {
    g_dui70Module = LoadSystemModule(L"dui70.dll");
    g_explorerFrameModule = LoadSystemModule(L"explorerframe.dll");
    if (!g_dui70Module || !g_explorerFrameModule) {
        return FALSE;
    }

    // The frame of a folder window belongs to shell32.dll.
    g_shell32Module = GetModuleHandleW(L"shell32.dll");

    HMODULE dui70 = g_dui70Module;
    HRESULT(WINAPI* createElement)(void*, PCWSTR, void*, void*, DWORD*, void**);
    if (!GetProc(dui70, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ", &Element_GetParent) ||
        !GetProc(dui70, "?GetID@Element@DirectUI@@QEAAGXZ", &Element_GetID) ||
        !GetProc(dui70, "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z", &Element_FindDescendent) ||
        !GetProc(dui70, "?GetPadding@Element@DirectUI@@QEAAPEBUtagRECT@@PEAPEAVValue@2@@Z", &Element_GetPadding) ||
        !GetProc(dui70, "?GetWidth@Element@DirectUI@@QEAAHXZ", &Element_GetWidth) ||
        !GetProc(dui70, "?SetWidth@Element@DirectUI@@QEAAJH@Z", &Element_SetWidth) ||
        !GetProc(dui70, "?SetPadding@Element@DirectUI@@QEAAJHHHH@Z", &Element_SetPadding) ||
        !GetProc(dui70, "?RemoveLocalValue@Element@DirectUI@@QEAAJP6APEBUPropertyInfo@2@XZ@Z", &Element_RemoveLocalValue) ||
        !GetProc(dui70, "?SetBorderThickness@Element@DirectUI@@QEAAJHHHH@Z", &Element_SetBorderThickness) ||
        !GetProc(dui70, "?SetBorderColor@Element@DirectUI@@QEAAJK@Z", &Element_SetBorderColor) ||
        !GetProc(dui70, "?SetBackgroundColor@Element@DirectUI@@QEAAJK@Z", &Element_SetBackgroundColor) ||
        !GetProc(dui70, "?BorderThicknessProp@Element@DirectUI@@SAPEBUPropertyInfo@2@XZ", &Element_BorderThicknessProp) ||
        !GetProc(dui70, "?BorderColorProp@Element@DirectUI@@SAPEBUPropertyInfo@2@XZ", &Element_BorderColorProp) ||
        !GetProc(dui70, "?BackgroundProp@Element@DirectUI@@SAPEBUPropertyInfo@2@XZ", &Element_BackgroundProp) ||
        !GetProc(dui70, "?Release@Value@DirectUI@@QEAAXXZ", &Value_Release) ||
        !GetProc(dui70, "?_ScaleRelativePixels@DUIXmlParser@DirectUI@@AEBAHH@Z", &DUIXmlParser_ScaleRelativePixels) ||
        !GetProc(dui70, "?CreateElement@DUIXmlParser@DirectUI@@QEAAJPEBGPEAVElement@2@1PEAKPEAPEAV32@@Z", &createElement)) {
        return FALSE;
    }

    // explorerframe.dll: the separators of the navigation pane. Optional, the
    // rest works without it.
    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {
            {LR"(private: void __cdecl CNscTree::DrawDivider(struct HDC__ *,struct _TREEITEM *))"},
            &CNscTree_DrawDivider_Original,
            CNscTree_DrawDivider_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(g_explorerFrameModule, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"HookSymbols failed, the separators can't be adjusted");
    }

    // The expand arrows of tree views.
    if (HMODULE uxtheme = LoadSystemModule(L"uxtheme.dll")) {
        Uxtheme_IsThemePartDefined = (decltype(Uxtheme_IsThemePartDefined))GetProcAddress(uxtheme, "IsThemePartDefined");
        // Optional, GetThemeClass is exported by ordinal 74.
        Uxtheme_GetThemeClass = (GetThemeClass_t)GetProcAddress(uxtheme, MAKEINTRESOURCEA(74));
        auto drawThemeBackground = (decltype(DrawThemeBackground_Original))GetProcAddress(uxtheme, "DrawThemeBackground");
        auto drawThemeBackgroundEx = (decltype(DrawThemeBackgroundEx_Original))GetProcAddress(uxtheme, "DrawThemeBackgroundEx");
        if (Uxtheme_IsThemePartDefined && drawThemeBackground && drawThemeBackgroundEx) {
            WindhawkUtils::SetFunctionHook(drawThemeBackground, DrawThemeBackground_Hook, &DrawThemeBackground_Original);
            WindhawkUtils::SetFunctionHook(drawThemeBackgroundEx, DrawThemeBackgroundEx_Hook, &DrawThemeBackgroundEx_Original);
        }
    }

    WindhawkUtils::SetFunctionHook(createElement, DUIXmlParser_CreateElement_Hook, &DUIXmlParser_CreateElement_Original);

    // The separators and the pin icons of the navigation pane.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    auto sendMessageW = user32 ? (decltype(SendMessageW_Original))GetProcAddress(user32, "SendMessageW") : nullptr;
    auto polyline = gdi32 ? (decltype(Polyline_Original))GetProcAddress(gdi32, "Polyline") : nullptr;
    if (sendMessageW && polyline) {
        WindhawkUtils::SetFunctionHook(sendMessageW, SendMessageW_Hook, &SendMessageW_Original);
        WindhawkUtils::SetFunctionHook(polyline, Polyline_Hook, &Polyline_Original);
    }

    return TRUE;
}

static BOOL SetupHooks() {
    if (SetupHooksImpl()) {
        return TRUE;
    }

    FreeLoadedModules();
    return FALSE;
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) {
        return false;
    }

    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

// Sets the hooks in a program which didn't have explorerframe.dll loaded when
// the mod was loaded. Only done once.
static void LateInit() {
    g_lateInitRefCount++;

    if (!g_unloading && !g_lateInitStarted.exchange(true)) {
        Wh_Log(L"explorerframe.dll is loaded, setting the hooks");

        if (!SetupHooks()) {
            Wh_Log(L"Setting the hooks failed");
        } else if (!g_unloading) {
            g_initialized = true;
            Wh_ApplyHookOperations();
        }
    }

    g_lateInitRefCount--;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module && !g_lateInitStarted) {
        DWORD lastError = GetLastError();
        if (GetModuleHandleW(L"explorerframe.dll")) {
            LateInit();
        }

        SetLastError(lastError);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_runFromWindowThreadMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    // File Explorer has explorerframe.dll and the modules it depends on
    // loaded anyway. The same is true for any program which has it loaded
    // already.
    if (IsExplorerProcess() || GetModuleHandleW(L"explorerframe.dll")) {
        if (!SetupHooks()) {
            return FALSE;
        }

        g_initialized = true;
        return TRUE;
    }

    // Another program, which might never show a file dialog. Don't load
    // anything into it, wait for the program to load explorerframe.dll.
    // dui70.dll is loaded along with it.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase ? (LoadLibraryExW_t)GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
    if (!loadLibraryExW ||
        !WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
        Wh_Log(L"Failed to hook LoadLibraryExW");
        return FALSE;
    }

    g_lateInit = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // In case explorerframe.dll was loaded before the LoadLibraryExW hook was
    // applied.
    if (g_lateInit && GetModuleHandleW(L"explorerframe.dll")) {
        LateInit();
    }

    ApplyToExistingWindows(true);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // Hooks can't be set after this function returns, and LateInit might
    // still be setting them. The symbols might be loading, which can take a
    // while.
    while (g_lateInitRefCount > 0) {
        Sleep(10);
    }

    ApplyToExistingWindows(false);
    FreeNavStateImages();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    FreeLoadedModules();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Restore with the old settings, then apply the new settings.
    ApplyToExistingWindows(false);
    LoadSettings();
    ApplyToExistingWindows(true);
}
