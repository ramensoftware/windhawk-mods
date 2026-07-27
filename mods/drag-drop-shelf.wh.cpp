// ==WindhawkMod==
// @id              drag-drop-shelf
// @name            Drag & Drop Shelf
// @description     A tray shelf you drag files, text, or images onto, then drag back out into any window
// @version         0.8.0
// @author          Ashish
// @github          https://github.com/ashish01-dev
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -lgdi32 -ldwmapi -lshlwapi -luuid
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only

// ==WindhawkModReadme==
/*
# Drag & Drop Shelf

Adds a small icon to the system tray. Click it to open a shelf panel:

- **Drag files, folders, text, or images onto the panel** to store them
  there. Each dropped item becomes a small Windows 11 style card showing an
  icon/thumbnail and a name.
- **Drag a card back out** onto any other window (File Explorer, an email
  draft, a chat app, Word, Paint, ...) to drop the file, text, or image
  there, exactly as if you'd dragged it from its original location.
- Click the small **x** on a hovered card to remove just that item, or
  right-click a card / the panel for more options (copy path, clear all).
- The panel closes automatically when you click elsewhere.

This is meant as a temporary "holding area" for drag-and-drop, similar in
spirit to macOS's Shelf/Yoink utilities, so you don't have to arrange two
windows side by side just to drag one thing between them.

## Notes

- File and folder items are stored by reference (their path), so they still
  point at the real file — removing a card doesn't delete anything.
- Text items store the copied text itself.
- Image items (e.g. an image dragged straight off a web page with no
  backing file) are held in memory as a bitmap for the lifetime of the
  session; they are not written to disk.
- Optionally, file/folder items can be remembered across Explorer restarts
  (see settings). Text and image items are always session-only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxItems: 12
  $name: Maximum items
  $description: Oldest items are automatically dropped once the shelf is full.
- popupWidth: 300
  $name: Shelf width (pixels, at 100% DPI)
- itemHeight: 64
  $name: Card height (pixels, at 100% DPI)
- rememberFilesAcrossRestarts: true
  $name: Remember dropped files/folders
  $description: File and folder items (not text or images) are saved to disk and restored the next time Explorer starts.
- removeItemAfterDrag: false
  $name: Remove item after dragging it out
  $description: When on, a card disappears from the shelf once you successfully drag it into another window. When off, cards stay until you remove them manually.
- theme: auto
  $name: Color Theme
  $description: Choose visual style for the shelf box.
  $options:
  - auto: System Default (Auto)
  - dark: Dark Mode
  - light: Light Mode
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <oleidl.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cwchar>
#include <fstream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Settings
// ----------------------------------------------------------------------------

struct Settings {
    int maxItems = 12;
    int popupWidth = 300;
    int itemHeight = 64;
    bool rememberFiles = true;
    bool removeAfterDrag = false;
    int theme = 0;
};

Settings g_settings;

void LoadSettings() {
    g_settings.maxItems = std::min(std::max(Wh_GetIntSetting(L"maxItems"), 1), 60);
    g_settings.popupWidth =
        std::min(std::max(Wh_GetIntSetting(L"popupWidth"), 220), 480);
    g_settings.itemHeight =
        std::min(std::max(Wh_GetIntSetting(L"itemHeight"), 48), 96);
    g_settings.rememberFiles =
        Wh_GetIntSetting(L"rememberFilesAcrossRestarts") != 0;
    g_settings.removeAfterDrag =
        Wh_GetIntSetting(L"removeItemAfterDrag") != 0;

    PCWSTR themeSetting = Wh_GetStringSetting(L"theme");
    if (themeSetting) {
        if (wcscmp(themeSetting, L"dark") == 0) {
            g_settings.theme = 1;
        } else if (wcscmp(themeSetting, L"light") == 0) {
            g_settings.theme = 2;
        } else {
            g_settings.theme = 0;
        }
        Wh_FreeStringSetting(themeSetting);
    } else {
        g_settings.theme = 0;
    }
}

// ----------------------------------------------------------------------------
// Shared helpers (styling matches the Alt+Tab Plus companion window look)
// ----------------------------------------------------------------------------

bool IsSystemDarkMode() {
    DWORD appsUseLightTheme = 1;
    DWORD size = sizeof(appsUseLightTheme);
    LONG status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &appsUseLightTheme,
        &size);
    if (status != ERROR_SUCCESS) {
        return true;
    }
    return appsUseLightTheme == 0;
}

bool IsShelfDarkMode() {
    if (g_settings.theme == 1) return true;
    if (g_settings.theme == 2) return false;
    return IsSystemDarkMode();
}

int ScaleForDpi(HWND window, int value) {
    UINT dpi = GetDpiForWindow(window);
    return MulDiv(value, dpi ? static_cast<int>(dpi) : 96, 96);
}

HFONT CreateShelfFont(HWND window, int points, bool semibold) {
    UINT dpi = GetDpiForWindow(window);
    int height = -MulDiv(points, dpi ? static_cast<int>(dpi) : 96, 72);
    return CreateFontW(height, 0, 0, 0, semibold ? FW_SEMIBOLD : FW_NORMAL,
                        FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                        L"Segoe UI");
}

void ConfigureRoundedPopup(HWND window, bool dark) {
    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_VALUE = 20;
    constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE_VALUE = 33;
    constexpr DWORD DWMWCP_ROUND_VALUE = 2;
    constexpr DWORD DWMWA_BORDER_COLOR_VALUE = 34;

    BOOL darkValue = dark;
    DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE_VALUE,
                          &darkValue, sizeof(darkValue));

    DWORD cornerPreference = DWMWCP_ROUND_VALUE;
    DwmSetWindowAttribute(window, DWMWA_WINDOW_CORNER_PREFERENCE_VALUE,
                          &cornerPreference, sizeof(cornerPreference));

    COLORREF borderColor = 0xFFFFFFFF; // DWMWA_COLOR_NONE - removes default white border
    DwmSetWindowAttribute(window, DWMWA_BORDER_COLOR_VALUE,
                          &borderColor, sizeof(borderColor));
}

void SetPopupRegion(HWND window, int width, int height) {
    int radius = ScaleForDpi(window, 12);
    HRGN region = CreateRoundRectRgn(0, 0, width + 1, height + 1, radius,
                                      radius);
    if (!SetWindowRgn(window, region, TRUE)) {
        DeleteObject(region);
    }
}

// ----------------------------------------------------------------------------
// Shelf item model
// ----------------------------------------------------------------------------

enum class ShelfItemType { File, Text, Image };

struct ShelfItem {
    ShelfItemType type = ShelfItemType::File;
    std::wstring path;         // File: full path to a file or folder.
    std::wstring text;         // Text: the copied text content.
    std::wstring displayName;  // Shown on the card.
    HBITMAP thumbnail = nullptr;  // Image: owned DIB section bitmap.
    int thumbWidth = 0;
    int thumbHeight = 0;
};

void UpdatePanelSize();

std::vector<ShelfItem> g_items;

void FreeShelfItem(ShelfItem& item) {
    if (item.thumbnail) {
        DeleteObject(item.thumbnail);
        item.thumbnail = nullptr;
    }
}

void ClearShelfItems() {
    for (auto& item : g_items) {
        FreeShelfItem(item);
    }
    g_items.clear();
    UpdatePanelSize();
}

std::wstring TrimForDisplay(const std::wstring& text, size_t maxLen) {
    std::wstring trimmed;
    trimmed.reserve(std::min(text.size(), maxLen) + 1);
    for (wchar_t ch : text) {
        if (ch == L'\r') {
            continue;
        }
        trimmed.push_back(ch == L'\n' ? L' ' : ch);
        if (trimmed.size() >= maxLen) {
            break;
        }
    }
    if (text.size() > trimmed.size()) {
        trimmed += L"\u2026";
    }
    return trimmed;
}

// ----------------------------------------------------------------------------
// Persistence (file/folder paths only)
// ----------------------------------------------------------------------------

std::wstring GetPersistenceFilePath() {
    wchar_t localAppData[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0,
                                localAppData))) {
        return L"";
    }
    std::wstring dir = std::wstring(localAppData) + L"\\DragDropShelf";
    CreateDirectoryW(dir.c_str(), nullptr);
    return dir + L"\\shelf-items.txt";
}

bool g_hasCustomPos = false;
POINT g_customPos{0, 0};
bool g_hasCustomSize = false;
int g_customWidth = 300;
int g_customHeight = 250;

void SaveShelfPersistence() {
    std::wstring path = GetPersistenceFilePath();
    if (path.empty()) {
        return;
    }
    std::wofstream out(path.c_str(), std::ios::trunc);
    if (!out) {
        return;
    }
    if (g_hasCustomPos) {
        out << L"[POS]:" << g_customPos.x << L"," << g_customPos.y << L"\n";
    }
    if (g_hasCustomSize) {
        out << L"[SIZE]:" << g_customWidth << L"," << g_customHeight << L"\n";
    }
    if (g_settings.rememberFiles) {
        for (const auto& item : g_items) {
            if (item.type == ShelfItemType::File) {
                out << item.path << L"\n";
            }
        }
    }
}

void AddFileItem(const std::wstring& filePath);

void LoadShelfPersistence() {
    std::wstring path = GetPersistenceFilePath();
    if (path.empty()) {
        return;
    }
    std::wifstream in(path.c_str());
    if (!in) {
        return;
    }
    std::wstring line;
    while (std::getline(in, line)) {
        if (line.rfind(L"[POS]:", 0) == 0) {
            int x = 0, y = 0;
            if (swscanf_s(line.c_str() + 6, L"%d,%d", &x, &y) == 2) {
                g_customPos.x = x;
                g_customPos.y = y;
                g_hasCustomPos = true;
            }
        } else if (line.rfind(L"[SIZE]:", 0) == 0) {
            int w = 0, h = 0;
            if (swscanf_s(line.c_str() + 7, L"%d,%d", &w, &h) == 2) {
                g_customWidth = w;
                g_customHeight = h;
                g_hasCustomSize = true;
            }
        } else if (g_settings.rememberFiles && !line.empty() && PathFileExistsW(line.c_str())) {
            AddFileItem(line);
        }
    }
}

// ----------------------------------------------------------------------------
// Forward declarations for panel window / tray
// ----------------------------------------------------------------------------

HWND g_panelWindow = nullptr;
HWND g_trayWindow = nullptr;
DWORD g_shelfThreadId = 0;
HANDLE g_shelfThread = nullptr;
HANDLE g_shelfThreadReady = nullptr;
std::atomic<bool> g_isDraggingOut{false};
bool g_isCollapsed = false;

void UpdatePanelSize();

void InvalidatePanel() {
    if (g_panelWindow) {
        InvalidateRect(g_panelWindow, nullptr, TRUE);
    }
}

// ----------------------------------------------------------------------------
// Adding items
// ----------------------------------------------------------------------------

void TrimToMaxItems() {
    while (static_cast<int>(g_items.size()) > g_settings.maxItems) {
        FreeShelfItem(g_items.back());
        g_items.pop_back();
    }
}

void AddFileItem(const std::wstring& filePath) {
    for (size_t i = 0; i < g_items.size(); i++) {
        if (g_items[i].type == ShelfItemType::File &&
            lstrcmpiW(g_items[i].path.c_str(), filePath.c_str()) == 0) {
            ShelfItem item = std::move(g_items[i]);
            g_items.erase(g_items.begin() + i);
            g_items.insert(g_items.begin(), std::move(item));
            SaveShelfPersistence();
            UpdatePanelSize();
            return;
        }
    }
    ShelfItem item;
    item.type = ShelfItemType::File;
    item.path = filePath;
    const wchar_t* name = PathFindFileNameW(filePath.c_str());
    item.displayName = (name && *name) ? name : filePath;
    g_items.insert(g_items.begin(), std::move(item));
    TrimToMaxItems();
    SaveShelfPersistence();
    UpdatePanelSize();
}

void AddTextItem(const std::wstring& text) {
    if (text.empty()) {
        return;
    }
    ShelfItem item;
    item.type = ShelfItemType::Text;
    item.text = text;
    item.displayName = TrimForDisplay(text, 60);
    g_items.insert(g_items.begin(), std::move(item));
    TrimToMaxItems();
    UpdatePanelSize();
}

void AddImageItem(HBITMAP bitmap, int width, int height) {
    if (!bitmap) {
        return;
    }
    ShelfItem item;
    item.type = ShelfItemType::Image;
    item.thumbnail = bitmap;
    item.thumbWidth = width;
    item.thumbHeight = height;
    item.displayName = L"Image";
    g_items.insert(g_items.begin(), std::move(item));
    TrimToMaxItems();
    UpdatePanelSize();
}

void RemoveItemAt(size_t index) {
    if (index >= g_items.size()) {
        return;
    }
    FreeShelfItem(g_items[index]);
    g_items.erase(g_items.begin() + index);
    SaveShelfPersistence();
    UpdatePanelSize();
}

// ----------------------------------------------------------------------------
// IDataObject / IDropSource / IEnumFORMATETC for dragging a card back out
// ----------------------------------------------------------------------------

class ShelfEnumFormatEtc final : public IEnumFORMATETC {
   public:
    
    virtual ~ShelfEnumFormatEtc() = default;
explicit ShelfEnumFormatEtc(std::vector<FORMATETC> formats, ULONG pos = 0)
        : m_formats(std::move(formats)), m_pos(pos) {}

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IEnumFORMATETC) {
            *ppv = static_cast<IEnumFORMATETC*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() override { return ++m_refCount; }
    ULONG __stdcall Release() override {
        ULONG count = --m_refCount;
        if (count == 0) {
            delete this;
        }
        return count;
    }

    HRESULT __stdcall Next(ULONG celt,
                           FORMATETC* rgelt,
                           ULONG* pceltFetched) override {
        ULONG fetched = 0;
        while (fetched < celt && m_pos < m_formats.size()) {
            rgelt[fetched] = m_formats[m_pos];
            m_pos++;
            fetched++;
        }
        if (pceltFetched) {
            *pceltFetched = fetched;
        }
        return fetched == celt ? S_OK : S_FALSE;
    }
    HRESULT __stdcall Skip(ULONG celt) override {
        m_pos = std::min(m_pos + celt,
                          static_cast<ULONG>(m_formats.size()));
        return S_OK;
    }
    HRESULT __stdcall Reset() override {
        m_pos = 0;
        return S_OK;
    }
    HRESULT __stdcall Clone(IEnumFORMATETC** ppEnum) override {
        if (!ppEnum) {
            return E_POINTER;
        }
        *ppEnum = new ShelfEnumFormatEtc(m_formats, m_pos);
        return S_OK;
    }

   private:
    std::atomic<ULONG> m_refCount{1};
    std::vector<FORMATETC> m_formats;
    ULONG m_pos;
};

class ShelfDataObject final : public IDataObject {
   public:
    
    virtual ~ShelfDataObject() { FreeShelfItem(m_item); }
explicit ShelfDataObject(ShelfItem item) : m_item(std::move(item)) {
        FORMATETC fmt{};
        fmt.ptd = nullptr;
        fmt.dwAspect = DVASPECT_CONTENT;
        fmt.lindex = -1;
        if (m_item.type == ShelfItemType::File) {
            fmt.cfFormat = CF_HDROP;
            fmt.tymed = TYMED_HGLOBAL;
            m_formats.push_back(fmt);
        } else if (m_item.type == ShelfItemType::Text) {
            fmt.cfFormat = CF_UNICODETEXT;
            fmt.tymed = TYMED_HGLOBAL;
            m_formats.push_back(fmt);
        } else {
            fmt.cfFormat = CF_DIB;
            fmt.tymed = TYMED_HGLOBAL;
            m_formats.push_back(fmt);
        }
    }

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IDataObject) {
            *ppv = static_cast<IDataObject*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() override { return ++m_refCount; }
    ULONG __stdcall Release() override {
        ULONG count = --m_refCount;
        if (count == 0) {
            delete this;
        }
        return count;
    }

    bool Matches(const FORMATETC* in) const {
        return !m_formats.empty() && in &&
               in->cfFormat == m_formats[0].cfFormat &&
               (in->tymed & m_formats[0].tymed);
    }

    HRESULT __stdcall GetData(FORMATETC* pformatetcIn,
                              STGMEDIUM* pmedium) override {
        if (!pformatetcIn || !pmedium) {
            return E_POINTER;
        }
        if (!Matches(pformatetcIn)) {
            return DV_E_FORMATETC;
        }
        ZeroMemory(pmedium, sizeof(STGMEDIUM));
        pmedium->tymed = TYMED_HGLOBAL;

        if (m_item.type == ShelfItemType::File) {
            size_t pathChars = m_item.path.size() + 2;
            size_t bytes = sizeof(DROPFILES) +
                            pathChars * sizeof(wchar_t);
            HGLOBAL hGlobal =
                GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
            if (!hGlobal) {
                return E_OUTOFMEMORY;
            }
            auto* dropFiles =
                static_cast<DROPFILES*>(GlobalLock(hGlobal));
            dropFiles->pFiles = sizeof(DROPFILES);
            dropFiles->fWide = TRUE;
            wchar_t* dest = reinterpret_cast<wchar_t*>(
                reinterpret_cast<BYTE*>(dropFiles) + sizeof(DROPFILES));
            memcpy(dest, m_item.path.c_str(), (m_item.path.size() + 1) * sizeof(wchar_t));
            GlobalUnlock(hGlobal);
            pmedium->hGlobal = hGlobal;
            return S_OK;
        }

        if (m_item.type == ShelfItemType::Text) {
            size_t chars = m_item.text.size() + 1;
            HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                                          chars * sizeof(wchar_t));
            if (!hGlobal) {
                return E_OUTOFMEMORY;
            }
            auto* dest = static_cast<wchar_t*>(GlobalLock(hGlobal));
            memcpy(dest, m_item.text.c_str(), chars * sizeof(wchar_t));
            GlobalUnlock(hGlobal);
            pmedium->hGlobal = hGlobal;
            return S_OK;
        }

        // Image: rebuild a packed DIB (BITMAPINFOHEADER + pixels) from the
        // stored 32bpp DIB section bitmap.
        BITMAP bmp{};
        if (!GetObject(m_item.thumbnail, sizeof(bmp), &bmp)) {
            return E_FAIL;
        }
        BITMAPINFOHEADER header{};
        header.biSize = sizeof(BITMAPINFOHEADER);
        header.biWidth = bmp.bmWidth;
        header.biHeight = bmp.bmHeight;
        header.biPlanes = 1;
        header.biBitCount = 32;
        header.biCompression = BI_RGB;
        size_t rowBytes = static_cast<size_t>(bmp.bmWidth) * 4;
        size_t pixelBytes = rowBytes * bmp.bmHeight;
        HGLOBAL hGlobal = GlobalAlloc(
            GMEM_MOVEABLE | GMEM_ZEROINIT,
            sizeof(BITMAPINFOHEADER) + pixelBytes);
        if (!hGlobal) {
            return E_OUTOFMEMORY;
        }
        auto* dest = static_cast<BYTE*>(GlobalLock(hGlobal));
        memcpy(dest, &header, sizeof(header));
        HDC screenDc = GetDC(nullptr);
        BITMAPINFO bmi{};
        bmi.bmiHeader = header;
        GetDIBits(screenDc, m_item.thumbnail, 0, bmp.bmHeight,
                  dest + sizeof(BITMAPINFOHEADER), &bmi, DIB_RGB_COLORS);
        ReleaseDC(nullptr, screenDc);
        GlobalUnlock(hGlobal);
        pmedium->hGlobal = hGlobal;
        return S_OK;
    }

    HRESULT __stdcall GetDataHere(FORMATETC*, STGMEDIUM*) override {
        return E_NOTIMPL;
    }
    HRESULT __stdcall QueryGetData(FORMATETC* pformatetc) override {
        return Matches(pformatetc) ? S_OK : DV_E_FORMATETC;
    }
    HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC*,
                                            FORMATETC* out) override {
        if (out) {
            ZeroMemory(out, sizeof(FORMATETC));
        }
        return DATA_S_SAMEFORMATETC;
    }
    HRESULT __stdcall SetData(FORMATETC*, STGMEDIUM*, BOOL) override {
        return E_NOTIMPL;
    }
    HRESULT __stdcall EnumFormatEtc(DWORD direction,
                                    IEnumFORMATETC** ppEnum) override {
        if (!ppEnum) {
            return E_POINTER;
        }
        if (direction != DATADIR_GET) {
            *ppEnum = nullptr;
            return E_NOTIMPL;
        }
        *ppEnum = new ShelfEnumFormatEtc(m_formats);
        return S_OK;
    }
    HRESULT __stdcall DAdvise(FORMATETC*,
                              DWORD,
                              IAdviseSink*,
                              DWORD*) override {
        return E_NOTIMPL;
    }
    HRESULT __stdcall DUnadvise(DWORD) override { return E_NOTIMPL; }
    HRESULT __stdcall EnumDAdvise(IEnumSTATDATA**) override {
        return E_NOTIMPL;
    }

   private:
    std::atomic<ULONG> m_refCount{1};
    ShelfItem m_item;
    std::vector<FORMATETC> m_formats;
};

class ShelfDropSource final : public IDropSource {
   public:
    
    virtual ~ShelfDropSource() = default;
HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IDropSource) {
            *ppv = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() override { return ++m_refCount; }
    ULONG __stdcall Release() override {
        ULONG count = --m_refCount;
        if (count == 0) {
            delete this;
        }
        return count;
    }

    HRESULT __stdcall QueryContinueDrag(BOOL escapePressed,
                                        DWORD keyState) override {
        if (escapePressed) {
            return DRAGDROP_S_CANCEL;
        }
        if (!(keyState & MK_LBUTTON)) {
            return DRAGDROP_S_DROP;
        }
        return S_OK;
    }
    HRESULT __stdcall GiveFeedback(DWORD) override {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }

   private:
    std::atomic<ULONG> m_refCount{1};
};

void BeginItemDrag(size_t index) {
    if (index >= g_items.size()) {
        return;
    }
    ShelfItem itemCopy = g_items[index];
    if (itemCopy.type == ShelfItemType::Image && itemCopy.thumbnail) {
        // Duplicate the bitmap so the data object owns an independent copy.
        HDC screenDc = GetDC(nullptr);
        HDC srcDc = CreateCompatibleDC(screenDc);
        HDC dstDc = CreateCompatibleDC(screenDc);
        HBITMAP copy = CreateCompatibleBitmap(
            screenDc, itemCopy.thumbWidth, itemCopy.thumbHeight);
        HGDIOBJ oldSrc = SelectObject(srcDc, itemCopy.thumbnail);
        HGDIOBJ oldDst = SelectObject(dstDc, copy);
        BitBlt(dstDc, 0, 0, itemCopy.thumbWidth, itemCopy.thumbHeight, srcDc,
               0, 0, SRCCOPY);
        SelectObject(srcDc, oldSrc);
        SelectObject(dstDc, oldDst);
        DeleteDC(srcDc);
        DeleteDC(dstDc);
        ReleaseDC(nullptr, screenDc);
        itemCopy.thumbnail = copy;
    }

    auto* dataObject = new ShelfDataObject(itemCopy);
    auto* dropSource = new ShelfDropSource();

    g_isDraggingOut = true;
    DWORD effect = 0;
    DoDragDrop(dataObject, dropSource, DROPEFFECT_COPY | DROPEFFECT_LINK,
               &effect);
    g_isDraggingOut = false;

    dataObject->Release();
    dropSource->Release();

    if (g_settings.removeAfterDrag && effect != DROPEFFECT_NONE) {
        // The index may have shifted if the user also modified the shelf
        // mid-drag (rare); re-find the same logical item defensively.
        if (index < g_items.size()) {
            RemoveItemAt(index);
        }
    }
}

// ----------------------------------------------------------------------------
// IDropTarget for the panel (accepting drops)
// ----------------------------------------------------------------------------

class ShelfDropTarget final : public IDropTarget {
   public:
    
    virtual ~ShelfDropTarget() = default;
explicit ShelfDropTarget(HWND window) : m_window(window) {}

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() override { return ++m_refCount; }
    ULONG __stdcall Release() override {
        ULONG count = --m_refCount;
        if (count == 0) {
            delete this;
        }
        return count;
    }

    HRESULT __stdcall DragEnter(IDataObject* dataObject,
                               DWORD,
                               POINTL,
                               DWORD* effect) override {
        m_hasHdrop = SupportsFormat(dataObject, CF_HDROP);
        m_hasText = SupportsFormat(dataObject, CF_UNICODETEXT);
        m_hasDib = SupportsFormat(dataObject, CF_DIB);
        *effect = (m_hasHdrop || m_hasText || m_hasDib) ? DROPEFFECT_COPY
                                                        : DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT __stdcall DragOver(DWORD, POINTL, DWORD* effect) override {
        *effect = (m_hasHdrop || m_hasText || m_hasDib) ? DROPEFFECT_COPY
                                                        : DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT __stdcall DragLeave() override { return S_OK; }

    HRESULT __stdcall Drop(IDataObject* dataObject,
                           DWORD,
                           POINTL,
                           DWORD* effect) override {
        bool handled = false;

        if (m_hasHdrop) {
            FORMATETC fmt{CF_HDROP, nullptr, DVASPECT_CONTENT, -1,
                          TYMED_HGLOBAL};
            STGMEDIUM stg{};
            if (SUCCEEDED(dataObject->GetData(&fmt, &stg))) {
                auto hDrop = static_cast<HDROP>(stg.hGlobal);
                UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                for (UINT i = 0; i < count; i++) {
                    wchar_t buffer[MAX_PATH]{};
                    if (DragQueryFileW(hDrop, i, buffer, MAX_PATH)) {
                        AddFileItem(buffer);
                        handled = true;
                    }
                }
                ReleaseStgMedium(&stg);
            }
        }

        if (!handled && m_hasText) {
            FORMATETC fmt{CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1,
                          TYMED_HGLOBAL};
            STGMEDIUM stg{};
            if (SUCCEEDED(dataObject->GetData(&fmt, &stg))) {
                auto* text = static_cast<wchar_t*>(GlobalLock(stg.hGlobal));
                if (text) {
                    AddTextItem(text);
                    handled = true;
                    GlobalUnlock(stg.hGlobal);
                }
                ReleaseStgMedium(&stg);
            }
        }

        if (!handled && m_hasDib) {
            FORMATETC fmt{CF_DIB, nullptr, DVASPECT_CONTENT, -1,
                          TYMED_HGLOBAL};
            STGMEDIUM stg{};
            if (SUCCEEDED(dataObject->GetData(&fmt, &stg))) {
                auto* header =
                    static_cast<BITMAPINFOHEADER*>(GlobalLock(stg.hGlobal));
                if (header) {
                    HDC screenDc = GetDC(nullptr);
                    BITMAPINFO bmi{};
                    bmi.bmiHeader = *header;
                    void* bits = nullptr;
                    HBITMAP bitmap = CreateDIBSection(
                        screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
                    if (bitmap && bits) {
                        const BYTE* pixels =
                            reinterpret_cast<const BYTE*>(header) +
                            header->biSize;
                        size_t rowBytes =
                            (static_cast<size_t>(header->biWidth) *
                                 (header->biBitCount / 8) +
                             3) &
                            ~static_cast<size_t>(3);
                        memcpy(bits, pixels,
                               rowBytes * std::abs(header->biHeight));
                        AddImageItem(bitmap, header->biWidth,
                                     std::abs(header->biHeight));
                        handled = true;
                    }
                    ReleaseDC(nullptr, screenDc);
                    GlobalUnlock(stg.hGlobal);
                }
                ReleaseStgMedium(&stg);
            }
        }

        *effect = handled ? DROPEFFECT_COPY : DROPEFFECT_NONE;
        if (handled) {
            ShowWindow(m_window, SW_SHOWNOACTIVATE);
        }
        return S_OK;
    }

   private:
    static bool SupportsFormat(IDataObject* dataObject, CLIPFORMAT format) {
        FORMATETC fmt{format, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        return dataObject->QueryGetData(&fmt) == S_OK;
    }

    std::atomic<ULONG> m_refCount{1};
    HWND m_window;
    bool m_hasHdrop = false;
    bool m_hasText = false;
    bool m_hasDib = false;
};

// ----------------------------------------------------------------------------
// Panel layout + painting
// ----------------------------------------------------------------------------

struct HitRegion {
    RECT rect;
    size_t itemIndex;
    bool isRemoveButton;
};

std::vector<HitRegion> g_hitRegions;
int g_hoveredIndex = -1;
bool g_hoveredIsRemove = false;
bool g_hoveredHeaderClose = false;
bool g_hoveredHeaderArrow = false;
bool g_hoveredHeaderTrash = false;
int g_scrollOffset = 0;
POINT g_mouseDownPoint{};
int g_mouseDownIndex = -1;
bool g_dragArmed = false;

void DrawItemIcon(HWND window,
                  HDC dc,
                  const ShelfItem& item,
                  const RECT& iconRect,
                  int iconSize,
                  bool dark) {
    int x = iconRect.left;
    int y = iconRect.top;

    if (item.type == ShelfItemType::File) {
        SHFILEINFOW fileInfo{};
        SHGetFileInfoW(item.path.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON);
        if (fileInfo.hIcon) {
            DrawIconEx(dc, x, y, fileInfo.hIcon, iconSize, iconSize, 0,
                       nullptr, DI_NORMAL);
            DestroyIcon(fileInfo.hIcon);
            return;
        }
    } else if (item.type == ShelfItemType::Image && item.thumbnail) {
        HDC memDc = CreateCompatibleDC(dc);
        HGDIOBJ old = SelectObject(memDc, item.thumbnail);
        StretchBlt(dc, x, y, iconSize, iconSize, memDc, 0, 0,
                  item.thumbWidth, item.thumbHeight, SRCCOPY);
        SelectObject(memDc, old);
        DeleteDC(memDc);
        return;
    }

    // Fallback letter badge (also used for Text items).
    const COLORREF badge =
        item.type == ShelfItemType::Text
            ? (dark ? RGB(176, 133, 74) : RGB(158, 110, 55))
            : (dark ? RGB(74, 106, 176) : RGB(55, 86, 158));
    HBRUSH badgeBrush = CreateSolidBrush(badge);
    RECT badgeRect{x, y, x + iconSize, y + iconSize};
    FillRect(dc, &badgeRect, badgeBrush);
    DeleteObject(badgeBrush);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    const wchar_t* letter = item.type == ShelfItemType::Text ? L"T" : L"I";
    HFONT font = CreateShelfFont(window, 11, true);
    HGDIOBJ oldFont = SelectObject(dc, font);
    DrawTextW(dc, letter, -1, &badgeRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

void AddMenuItem(HMENU menu, UINT id, const wchar_t* text) {
    MENUITEMINFOW mii{sizeof(MENUITEMINFOW)};
    mii.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
    mii.fType = MFT_STRING;
    mii.fState = MFS_ENABLED;
    mii.wID = id;
    mii.dwTypeData = const_cast<wchar_t*>(text);
    mii.cch = static_cast<UINT>(wcslen(text));
    InsertMenuItemW(menu, id, FALSE, &mii);
}

void OpenFileLocation(const std::wstring& path) {
    if (path.empty()) return;
    std::wstring args = L"/select,\"" + path + L"\"";
    ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
}

void OpenFileDirectly(const std::wstring& path) {
    if (path.empty()) return;
    ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void PaintPanel(HWND window) {
    PAINTSTRUCT paint;
    HDC dc = BeginPaint(window, &paint);
    if (!dc) {
        return;
    }

    RECT client;
    GetClientRect(window, &client);
    HDC bufferDc = CreateCompatibleDC(dc);
    HBITMAP bufferBitmap =
        CreateCompatibleBitmap(dc, client.right, client.bottom);
    HGDIOBJ oldBitmap = SelectObject(bufferDc, bufferBitmap);

    bool dark = IsShelfDarkMode();
    const COLORREF background = dark ? RGB(32, 32, 32) : RGB(245, 245, 245);
    const COLORREF itemBackground =
        dark ? RGB(45, 45, 45) : RGB(232, 232, 232);
    const COLORREF itemHover = dark ? RGB(65, 65, 65) : RGB(215, 215, 215);
    const COLORREF primary = dark ? RGB(255, 255, 255) : RGB(20, 20, 20);
    const COLORREF secondary = dark ? RGB(190, 190, 190) : RGB(90, 90, 90);
    const COLORREF removeBadge = dark ? RGB(90, 90, 90) : RGB(200, 200, 200);

    HBRUSH backgroundBrush = CreateSolidBrush(background);
    FillRect(bufferDc, &client, backgroundBrush);
    DeleteObject(backgroundBrush);

    const COLORREF btnHoverBg = dark ? RGB(65, 65, 65) : RGB(220, 220, 220);

    SetBkMode(bufferDc, TRANSPARENT);
    SetTextColor(bufferDc, primary);

    int btnSize = ScaleForDpi(window, 24);
    int btnTop = ScaleForDpi(window, 10);
    int btnGap = ScaleForDpi(window, 4);

    // 1. Header close button (far right: 24x24px, 10pt bold cross)
    RECT headerCloseRect{client.right - btnSize - ScaleForDpi(window, 10),
                        btnTop,
                        client.right - ScaleForDpi(window, 10),
                        btnTop + btnSize};

    // 2. Clear All Broom button (middle: 24x24px, 10pt broom icon)
    RECT headerBroomRect{headerCloseRect.left - btnSize - btnGap,
                         btnTop,
                         headerCloseRect.left - btnGap,
                         btnTop + btnSize};

    // 3. Expand/Collapse Arrow button (left: 24x24px, 10pt arrow icon)
    RECT headerArrowRect{headerBroomRect.left - btnSize - btnGap,
                         btnTop,
                         headerBroomRect.left - btnGap,
                         btnTop + btnSize};

    // Title Shelf (N) aligned in the exact same 24px horizontal band
    SetTextColor(bufferDc, primary);
    HFONT headingFont = CreateShelfFont(window, 10, true);
    HGDIOBJ oldFont = SelectObject(bufferDc, headingFont);
    RECT headingRect{ScaleForDpi(window, 14), btnTop,
                     headerArrowRect.left - ScaleForDpi(window, 6),
                     btnTop + btnSize};
    wchar_t heading[64];
    _snwprintf_s(heading, _TRUNCATE, L"Shelf (%zu)", g_items.size());
    DrawTextW(bufferDc, heading, -1, &headingRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(bufferDc, oldFont);
    DeleteObject(headingFont);

    // Render Close Button
    if (g_hoveredHeaderClose) {
        HBRUSH btnBrush = CreateSolidBrush(btnHoverBg);
        HPEN nullPen = static_cast<HPEN>(GetStockObject(NULL_PEN));
        HGDIOBJ ob = SelectObject(bufferDc, btnBrush);
        HGDIOBJ op = SelectObject(bufferDc, nullPen);
        int radius = ScaleForDpi(window, 6);
        RoundRect(bufferDc, headerCloseRect.left, headerCloseRect.top,
                  headerCloseRect.right, headerCloseRect.bottom, radius, radius);
        SelectObject(bufferDc, op);
        SelectObject(bufferDc, ob);
        DeleteObject(btnBrush);
    }

    SetTextColor(bufferDc, g_hoveredHeaderClose ? RGB(255, 90, 90) : secondary);
    HFONT headerXFont = CreateShelfFont(window, 10, true);
    oldFont = SelectObject(bufferDc, headerXFont);
    DrawTextW(bufferDc, L"\u00D7", -1, &headerCloseRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(bufferDc, oldFont);
    DeleteObject(headerXFont);

    // Render Broom Button (Primary/Theme color, matching context menu)
    if (g_hoveredHeaderTrash) {
        HBRUSH btnBrush = CreateSolidBrush(btnHoverBg);
        HPEN nullPen = static_cast<HPEN>(GetStockObject(NULL_PEN));
        HGDIOBJ ob = SelectObject(bufferDc, btnBrush);
        HGDIOBJ op = SelectObject(bufferDc, nullPen);
        int radius = ScaleForDpi(window, 6);
        RoundRect(bufferDc, headerBroomRect.left, headerBroomRect.top,
                  headerBroomRect.right, headerBroomRect.bottom, radius, radius);
        SelectObject(bufferDc, op);
        SelectObject(bufferDc, ob);
        DeleteObject(btnBrush);
    }

    SetTextColor(bufferDc, primary);
    HFONT broomFont = CreateShelfFont(window, 10, true);
    oldFont = SelectObject(bufferDc, broomFont);
    static const wchar_t kBroomStr[] = { 0xD83E, 0xDDF9, 0x0000 };
    DrawTextW(bufferDc, kBroomStr, -1, &headerBroomRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(bufferDc, oldFont);
    DeleteObject(broomFont);

    // Render Arrow Button
    if (g_hoveredHeaderArrow) {
        HBRUSH btnBrush = CreateSolidBrush(btnHoverBg);
        HPEN nullPen = static_cast<HPEN>(GetStockObject(NULL_PEN));
        HGDIOBJ ob = SelectObject(bufferDc, btnBrush);
        HGDIOBJ op = SelectObject(bufferDc, nullPen);
        int radius = ScaleForDpi(window, 6);
        RoundRect(bufferDc, headerArrowRect.left, headerArrowRect.top,
                  headerArrowRect.right, headerArrowRect.bottom, radius, radius);
        SelectObject(bufferDc, op);
        SelectObject(bufferDc, ob);
        DeleteObject(btnBrush);
    }

    SetTextColor(bufferDc, primary);
    HFONT arrowFont = CreateShelfFont(window, 10, true);
    oldFont = SelectObject(bufferDc, arrowFont);
    DrawTextW(bufferDc, g_isCollapsed ? L"\u25BC" : L"\u25B2", -1, &headerArrowRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(bufferDc, oldFont);
    DeleteObject(arrowFont);

    g_hitRegions.clear();

    HitRegion closeHeaderRegion{headerCloseRect, static_cast<size_t>(-1), true};
    g_hitRegions.push_back(closeHeaderRegion);

    HitRegion arrowHeaderRegion{headerArrowRect, static_cast<size_t>(-2), true};
    g_hitRegions.push_back(arrowHeaderRegion);

    HitRegion trashHeaderRegion{headerBroomRect, static_cast<size_t>(-3), true};
    g_hitRegions.push_back(trashHeaderRegion);

    const int contentTop = ScaleForDpi(window, 44);

    // Sub-header separator line for visual polish
    HPEN linePen = CreatePen(PS_SOLID, 1, dark ? RGB(55, 55, 55) : RGB(220, 220, 220));
    HGDIOBJ oldPen = SelectObject(bufferDc, linePen);
    MoveToEx(bufferDc, ScaleForDpi(window, 12), contentTop - ScaleForDpi(window, 6), nullptr);
    LineTo(bufferDc, client.right - ScaleForDpi(window, 12), contentTop - ScaleForDpi(window, 6));
    SelectObject(bufferDc, oldPen);
    DeleteObject(linePen);

    if (g_isCollapsed) {
        BitBlt(dc, 0, 0, client.right, client.bottom, bufferDc, 0, 0, SRCCOPY);
        SelectObject(bufferDc, oldBitmap);
        DeleteObject(bufferBitmap);
        DeleteDC(bufferDc);
        EndPaint(window, &paint);
        return;
    }

    const int marginX = ScaleForDpi(window, 10);
    const int tileGap = ScaleForDpi(window, 8);
    const int minTileWidth = ScaleForDpi(window, 80);
    int clientW = static_cast<int>(client.right);
    int cols = std::max(1, static_cast<int>((clientW - 2 * marginX + tileGap) / (minTileWidth + tileGap)));
    const int tileWidth = std::max(ScaleForDpi(window, 40), (clientW - 2 * marginX - (cols - 1) * tileGap) / cols);
    const int tileHeight = ScaleForDpi(window, 84);
    const int iconSize = ScaleForDpi(window, 36);
    const int removeSize = ScaleForDpi(window, 22);

    if (g_items.empty()) {
        HFONT emptyFont = CreateShelfFont(window, 9, false);
        oldFont = SelectObject(bufferDc, emptyFont);
        SetTextColor(bufferDc, secondary);
        RECT emptyRect{ScaleForDpi(window, 16), contentTop,
                       client.right - ScaleForDpi(window, 16),
                       client.bottom - ScaleForDpi(window, 10)};
        DrawTextW(bufferDc,
                  L"Drag files, folders, text, or images here. "
                  L"Drag an icon back out to drop it elsewhere.",
                  -1, &emptyRect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(bufferDc, oldFont);
        DeleteObject(emptyFont);
    } else {
        for (size_t i = 0; i < g_items.size(); i++) {
            const ShelfItem& item = g_items[i];
            int row = static_cast<int>(i) / cols;
            int col = static_cast<int>(i) % cols;
            int x = marginX + col * (tileWidth + tileGap);
            int y = contentTop + row * (tileHeight + tileGap) - g_scrollOffset;
            RECT tileRect{x, y, x + tileWidth, y + tileHeight};

            if (y + tileHeight < contentTop || y > client.bottom) {
                continue;
            }

            bool hovered = g_hoveredIndex == static_cast<int>(i);
            HBRUSH itemBrush = CreateSolidBrush(hovered ? itemHover : itemBackground);
            HPEN nullPen = static_cast<HPEN>(GetStockObject(NULL_PEN));
            HGDIOBJ oldBrush = SelectObject(bufferDc, itemBrush);
            HGDIOBJ oldPen = SelectObject(bufferDc, nullPen);
            int radius = ScaleForDpi(window, 8);
            RoundRect(bufferDc, tileRect.left, tileRect.top, tileRect.right,
                      tileRect.bottom, radius, radius);
            SelectObject(bufferDc, oldPen);
            SelectObject(bufferDc, oldBrush);
            DeleteObject(itemBrush);

            RECT iconRect{tileRect.left + (tileWidth - iconSize) / 2,
                          tileRect.top + ScaleForDpi(window, 8),
                          tileRect.left + (tileWidth - iconSize) / 2 + iconSize,
                          tileRect.top + ScaleForDpi(window, 8) + iconSize};
            DrawItemIcon(window, bufferDc, item, iconRect, iconSize, dark);

            RECT titleRect{tileRect.left + ScaleForDpi(window, 4),
                           tileRect.top + iconSize + ScaleForDpi(window, 12),
                           tileRect.right - ScaleForDpi(window, 4),
                           tileRect.bottom - ScaleForDpi(window, 4)};
            HFONT titleFont = CreateShelfFont(window, 8, false);
            oldFont = SelectObject(bufferDc, titleFont);
            SetTextColor(bufferDc, primary);
            DrawTextW(bufferDc, item.displayName.c_str(), -1, &titleRect,
                      DT_CENTER | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX);
            SelectObject(bufferDc, oldFont);
            DeleteObject(titleFont);

            HitRegion cardRegion{tileRect, i, false};
            g_hitRegions.push_back(cardRegion);

            if (hovered) {
                RECT removeRect{tileRect.right - removeSize - ScaleForDpi(window, 4),
                                tileRect.top + ScaleForDpi(window, 4),
                                tileRect.right - ScaleForDpi(window, 4),
                                tileRect.top + ScaleForDpi(window, 4) + removeSize};
                bool removeHover = g_hoveredIsRemove;
                HBRUSH removeBrush = CreateSolidBrush(removeBadge);
                HGDIOBJ ob = SelectObject(bufferDc, removeBrush);
                HGDIOBJ op = SelectObject(bufferDc, nullPen);
                RoundRect(bufferDc, removeRect.left, removeRect.top,
                          removeRect.right, removeRect.bottom, removeSize, removeSize);
                SelectObject(bufferDc, op);
                SelectObject(bufferDc, ob);
                DeleteObject(removeBrush);

                SetTextColor(bufferDc, removeHover ? RGB(255, 90, 90) : primary);
                HFONT xFont = CreateShelfFont(window, 10, true);
                oldFont = SelectObject(bufferDc, xFont);
                DrawTextW(bufferDc, L"\u00D7", -1, &removeRect,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                SelectObject(bufferDc, oldFont);
                DeleteObject(xFont);

                HitRegion removeRegion{removeRect, i, true};
                g_hitRegions.push_back(removeRegion);
            }
        }
    }

    // Smooth 1px theme border
    HPEN borderPen = CreatePen(PS_SOLID, 1, dark ? RGB(50, 50, 50) : RGB(215, 215, 215));
    HGDIOBJ oldBorderPen = SelectObject(bufferDc, borderPen);
    HGDIOBJ oldBorderBrush = SelectObject(bufferDc, GetStockObject(NULL_BRUSH));
    int bRadius = ScaleForDpi(window, 12);
    RoundRect(bufferDc, 0, 0, client.right, client.bottom, bRadius, bRadius);
    SelectObject(bufferDc, oldBorderBrush);
    SelectObject(bufferDc, oldBorderPen);
    DeleteObject(borderPen);

    BitBlt(dc, 0, 0, client.right, client.bottom, bufferDc, 0, 0, SRCCOPY);
    SelectObject(bufferDc, oldBitmap);
    DeleteObject(bufferBitmap);
    DeleteDC(bufferDc);
    EndPaint(window, &paint);
}

int GetMaxScroll(HWND window);

HHOOK g_mouseHook = nullptr;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_MOUSEWHEEL) {
        if (g_panelWindow && IsWindowVisible(g_panelWindow)) {
            auto* hs = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            RECT rc;
            GetWindowRect(g_panelWindow, &rc);
            if (PtInRect(&rc, hs->pt)) {
                short delta = static_cast<short>(HIWORD(hs->mouseData));
                int maxScroll = GetMaxScroll(g_panelWindow);
                if (maxScroll > 0) {
                    g_scrollOffset -= (delta / WHEEL_DELTA) * ScaleForDpi(g_panelWindow, 50);
                    g_scrollOffset = std::min(std::max(g_scrollOffset, 0), maxScroll);
                    InvalidateRect(g_panelWindow, nullptr, TRUE);
                }
                return 1;
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

int GetMaxScroll(HWND window) {
    if (g_isCollapsed || g_items.empty()) return 0;
    RECT client;
    GetClientRect(window, &client);
    const int marginX = ScaleForDpi(window, 10);
    const int tileGap = ScaleForDpi(window, 8);
    const int minTileWidth = ScaleForDpi(window, 80);
    int clientW = static_cast<int>(client.right);
    int cols = std::max(1, static_cast<int>((clientW - 2 * marginX + tileGap) / (minTileWidth + tileGap)));
    int tileHeight = ScaleForDpi(window, 84);
    int totalRows = static_cast<int>((g_items.size() + cols - 1) / cols);
    int visibleHeight = static_cast<int>(client.bottom) - ScaleForDpi(window, 44);
    int totalHeight = totalRows * (tileHeight + tileGap) - tileGap;
    if (totalHeight <= visibleHeight) return 0;
    return totalHeight - visibleHeight + ScaleForDpi(window, 12);
}

int PanelContentHeight(HWND window) {
    int contentTop = ScaleForDpi(window, 44);
    if (g_isCollapsed) {
        return contentTop;
    }
    if (g_items.empty()) {
        return contentTop + ScaleForDpi(window, 60);
    }
    const int cols = 3;
    int tileHeight = ScaleForDpi(window, 84);
    int tileGap = ScaleForDpi(window, 8);
    int bottomMargin = ScaleForDpi(window, 12);
    int totalRows = static_cast<int>((g_items.size() + cols - 1) / cols);
    int visibleRows = std::min(totalRows, 2);
    return contentTop + visibleRows * (tileHeight + tileGap) - tileGap + bottomMargin;
}

void UpdatePanelSize() {
    if (!g_panelWindow) return;
    int width = g_hasCustomSize ? g_customWidth : ScaleForDpi(g_panelWindow, g_settings.popupWidth);
    int height = g_isCollapsed
        ? ScaleForDpi(g_panelWindow, 44)
        : (g_hasCustomSize ? g_customHeight : std::min(PanelContentHeight(g_panelWindow), ScaleForDpi(g_panelWindow, 600)));
    int maxScroll = GetMaxScroll(g_panelWindow);
    g_scrollOffset = std::min(std::max(g_scrollOffset, 0), maxScroll);
    RECT rc;
    GetWindowRect(g_panelWindow, &rc);
    SetWindowPos(g_panelWindow, HWND_TOPMOST, rc.left, rc.top, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
    SetPopupRegion(g_panelWindow, width, height);
    InvalidateRect(g_panelWindow, nullptr, TRUE);
}

// ----------------------------------------------------------------------------
// Panel window procedure
// ----------------------------------------------------------------------------

const HitRegion* FindHitRegion(POINT point, bool wantRemove) {
    for (auto it = g_hitRegions.rbegin(); it != g_hitRegions.rend(); ++it) {
        if (it->isRemoveButton == wantRemove && PtInRect(&it->rect, point)) {
            return &(*it);
        }
    }
    return nullptr;
}

void HidePanel() {
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_panelWindow) {
        ShowWindow(g_panelWindow, SW_HIDE);
    }
}

LRESULT CALLBACK PanelWndProc(HWND window,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            auto* dropTarget = new ShelfDropTarget(window);
            RegisterDragDrop(window, dropTarget);
            SetWindowLongPtrW(window, GWLP_USERDATA,
                             reinterpret_cast<LONG_PTR>(dropTarget));
            return 0;
        }
        case WM_DESTROY: {
            auto* dropTarget = reinterpret_cast<ShelfDropTarget*>(
                GetWindowLongPtrW(window, GWLP_USERDATA));
            RevokeDragDrop(window);
            if (dropTarget) {
                dropTarget->Release();
            }
            return 0;
        }
        case WM_PAINT:
            PaintPanel(window);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                SetCursor(LoadCursorW(nullptr, IDC_ARROW));
                if (GetForegroundWindow() != window && GetCapture() != window) {
                    SetForegroundWindow(window);
                }
                return TRUE;
            }
            return DefWindowProcW(window, message, wParam, lParam);
        case WM_MOUSEMOVE: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (GetForegroundWindow() != window && wParam == 0) {
                SetForegroundWindow(window);
            }
            const HitRegion* removeHit = FindHitRegion(point, true);
            const HitRegion* cardHit = FindHitRegion(point, false);

            int newHovered = cardHit ? static_cast<int>(cardHit->itemIndex) : -1;
            bool newRemoveHover = (removeHit != nullptr && removeHit->itemIndex < g_items.size());
            bool newHeaderClose = (removeHit != nullptr && removeHit->itemIndex == static_cast<size_t>(-1));
            bool newHeaderArrow = (removeHit != nullptr && removeHit->itemIndex == static_cast<size_t>(-2));
            bool newHeaderTrash = (removeHit != nullptr && removeHit->itemIndex == static_cast<size_t>(-3));

            if (newHovered != g_hoveredIndex ||
                newRemoveHover != g_hoveredIsRemove ||
                newHeaderClose != g_hoveredHeaderClose ||
                newHeaderArrow != g_hoveredHeaderArrow ||
                newHeaderTrash != g_hoveredHeaderTrash) {
                g_hoveredIndex = newHovered;
                g_hoveredIsRemove = newRemoveHover;
                g_hoveredHeaderClose = newHeaderClose;
                g_hoveredHeaderArrow = newHeaderArrow;
                g_hoveredHeaderTrash = newHeaderTrash;
                InvalidateRect(window, nullptr, TRUE);
            }

            if (g_dragArmed && g_mouseDownIndex >= 0) {
                int dx = point.x - g_mouseDownPoint.x;
                int dy = point.y - g_mouseDownPoint.y;
                if (std::abs(dx) > GetSystemMetrics(SM_CXDRAG) ||
                    std::abs(dy) > GetSystemMetrics(SM_CYDRAG)) {
                    g_dragArmed = false;
                    size_t index = static_cast<size_t>(g_mouseDownIndex);
                    g_mouseDownIndex = -1;
                    ReleaseCapture();
                    BeginItemDrag(index);
                }
            }

            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, window, 0};
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            g_hoveredIndex = -1;
            g_hoveredIsRemove = false;
            g_hoveredHeaderClose = false;
            g_hoveredHeaderArrow = false;
            g_hoveredHeaderTrash = false;
            InvalidateRect(window, nullptr, TRUE);
            return 0;
        case WM_MOUSEWHEEL: {
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            int maxScroll = GetMaxScroll(window);
            if (maxScroll > 0) {
                g_scrollOffset -= (zDelta / WHEEL_DELTA) * ScaleForDpi(window, 50);
                g_scrollOffset = std::min(std::max(g_scrollOffset, 0), maxScroll);
                InvalidateRect(window, nullptr, TRUE);
            }
            return 0;
        }
        case WM_NCHITTEST: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            POINT clientPt = pt;
            ScreenToClient(window, &clientPt);
            RECT client;
            GetClientRect(window, &client);

            int border = ScaleForDpi(window, 8);

            if (clientPt.y < border && clientPt.x < border) return HTTOPLEFT;
            if (clientPt.y < border && clientPt.x > client.right - border) return HTTOPRIGHT;
            if (clientPt.y > client.bottom - border && clientPt.x < border) return HTBOTTOMLEFT;
            if (clientPt.y > client.bottom - border && clientPt.x > client.right - border) return HTBOTTOMRIGHT;

            if (clientPt.y < border) return HTTOP;
            if (clientPt.y > client.bottom - border) return HTBOTTOM;
            if (clientPt.x < border) return HTLEFT;
            if (clientPt.x > client.right - border) return HTRIGHT;

            const HitRegion* removeHit = FindHitRegion(clientPt, true);
            if (removeHit) {
                return HTCLIENT;
            }

            int contentTop = ScaleForDpi(window, 44);
            if (clientPt.y < contentTop) {
                return HTCAPTION;
            }

            return HTCLIENT;
        }
        case WM_LBUTTONDOWN: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const HitRegion* removeHit = FindHitRegion(point, true);
            if (removeHit) {
                if (removeHit->itemIndex == static_cast<size_t>(-1)) {
                    HidePanel();
                    return 0;
                }
                if (removeHit->itemIndex == static_cast<size_t>(-2)) {
                    g_isCollapsed = !g_isCollapsed;
                    UpdatePanelSize();
                    return 0;
                }
                if (removeHit->itemIndex == static_cast<size_t>(-3)) {
                    if (!g_items.empty()) {
                        int res = MessageBoxW(window,
                            L"Are you sure you want to remove all items from the shelf?",
                            L"Clear Shelf",
                            MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
                        if (res == IDYES) {
                            ClearShelfItems();
                        }
                    }
                    return 0;
                }
                RemoveItemAt(removeHit->itemIndex);
                return 0;
            }
            const HitRegion* cardHit = FindHitRegion(point, false);
            if (cardHit) {
                g_mouseDownPoint = point;
                g_mouseDownIndex = static_cast<int>(cardHit->itemIndex);
                g_dragArmed = true;
                SetCapture(window);
            }
            return 0;
        }
        case WM_LBUTTONUP:
            g_dragArmed = false;
            g_mouseDownIndex = -1;
            if (GetCapture() == window) {
                ReleaseCapture();
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const HitRegion* removeHit = FindHitRegion(point, true);
            if (removeHit) {
                return 0;
            }
            const HitRegion* cardHit = FindHitRegion(point, false);
            if (!cardHit || cardHit->itemIndex >= g_items.size()) {
                return 0;
            }
            HMENU menu = CreatePopupMenu();
            if (g_items[cardHit->itemIndex].type == ShelfItemType::File) {
                AddMenuItem(menu, 1, L"🚀 Open File");
                AddMenuItem(menu, 2, L"📁 Go to File (Open Location)");
                AddMenuItem(menu, 3, L"📋 Copy Path");
                AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            }
            AddMenuItem(menu, 4, L"❌ Remove Item");
            AddMenuItem(menu, 5, L"🧹 Clear All");

            POINT screenPoint = point;
            ClientToScreen(window, &screenPoint);
            SetForegroundWindow(window);
            int selected = TrackPopupMenuEx(
                menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
                screenPoint.x, screenPoint.y, window, nullptr);
            PostMessageW(window, WM_NULL, 0, 0);
            DestroyMenu(menu);

            if (cardHit && cardHit->itemIndex < g_items.size()) {
                const auto& item = g_items[cardHit->itemIndex];
                if (selected == 1 && item.type == ShelfItemType::File) {
                    OpenFileDirectly(item.path);
                } else if (selected == 2 && item.type == ShelfItemType::File) {
                    OpenFileLocation(item.path);
                } else if (selected == 3 && item.type == ShelfItemType::File) {
                    std::wstring quoted = L"\"" + item.path + L"\"";
                    if (OpenClipboard(window)) {
                        EmptyClipboard();
                        size_t bytes = (quoted.size() + 1) * sizeof(wchar_t);
                        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bytes);
                        if (hGlobal) {
                            auto* dest = static_cast<wchar_t*>(GlobalLock(hGlobal));
                            memcpy(dest, quoted.c_str(), bytes);
                            GlobalUnlock(hGlobal);
                            SetClipboardData(CF_UNICODETEXT, hGlobal);
                        }
                        CloseClipboard();
                    }
                } else if (selected == 4) {
                    RemoveItemAt(cardHit->itemIndex);
                }
            }
            if (selected == 5) {
                ClearShelfItems();
            }
            return 0;
        }
        case WM_EXITSIZEMOVE: {
            if (window && IsWindowVisible(window)) {
                RECT rc;
                GetWindowRect(window, &rc);
                g_customPos.x = rc.left;
                g_customPos.y = rc.top;
                g_hasCustomPos = true;
                if (!g_isCollapsed) {
                    g_customWidth = rc.right - rc.left;
                    g_customHeight = rc.bottom - rc.top;
                    g_hasCustomSize = true;
                }
                SaveShelfPersistence();
            }
            return 0;
        }
        case WM_WINDOWPOSCHANGED: {
            auto* pos = reinterpret_cast<WINDOWPOS*>(lParam);
            if (pos && IsWindowVisible(window)) {
                if (!(pos->flags & SWP_NOMOVE) && GetCapture() != window && !g_isDraggingOut) {
                    RECT rc;
                    GetWindowRect(window, &rc);
                    g_customPos.x = rc.left;
                    g_customPos.y = rc.top;
                    g_hasCustomPos = true;
                }
                if (!(pos->flags & SWP_NOSIZE) && !g_isCollapsed) {
                    RECT rc;
                    GetWindowRect(window, &rc);
                    int w = rc.right - rc.left;
                    int h = rc.bottom - rc.top;
                    if (w > 100 && h > 80) {
                        g_customWidth = w;
                        g_customHeight = h;
                        g_hasCustomSize = true;
                        SetPopupRegion(window, w, h);
                        InvalidateRect(window, nullptr, FALSE);
                    }
                }
                SaveShelfPersistence();
            }
            break;
        }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

void ShowPanelNear(POINT anchor) {
    if (!g_panelWindow) {
        return;
    }

    int width = g_hasCustomSize ? g_customWidth : ScaleForDpi(g_panelWindow, g_settings.popupWidth);
    int height = g_hasCustomSize ? g_customHeight : std::min(PanelContentHeight(g_panelWindow), ScaleForDpi(g_panelWindow, 560));

    int x = 0;
    int y = 0;

    if (g_hasCustomPos) {
        x = g_customPos.x;
        y = g_customPos.y;
    } else {
        HMONITOR monitor =
            MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{sizeof(MONITORINFO)};
        GetMonitorInfoW(monitor, &monitorInfo);
        RECT work = monitorInfo.rcWork;

        x = std::min(static_cast<int>(work.right - width - 4),
                     std::max(static_cast<int>(anchor.x - width), static_cast<int>(work.left)));
        y = std::max(static_cast<int>(work.top), static_cast<int>(anchor.y - height - 8));
        if (y + height > work.bottom) {
            y = work.bottom - height - 4;
        }
    }

    ConfigureRoundedPopup(g_panelWindow, IsShelfDarkMode());
    SetWindowPos(g_panelWindow, HWND_TOPMOST, x, y, width, height,
                SWP_SHOWWINDOW | SWP_NOACTIVATE);
    SetPopupRegion(g_panelWindow, width, height);
    ShowWindow(g_panelWindow, SW_SHOWNOACTIVATE);
    if (!g_mouseHook) {
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc, GetModuleHandleW(nullptr), 0);
    }
    InvalidateRect(g_panelWindow, nullptr, TRUE);
}

void TogglePanel(POINT anchor) {
    if (!g_panelWindow) {
        return;
    }
    if (IsWindowVisible(g_panelWindow)) {
        HidePanel();
    } else {
        ShowPanelNear(anchor);
    }
}

// ----------------------------------------------------------------------------
// Tray icon
// ----------------------------------------------------------------------------

constexpr UINT WM_SHELF_TRAYICON = WM_APP + 1;
constexpr UINT TRAY_ICON_ID = 1;
UINT g_taskbarCreatedMessage = 0;

HICON CreateShelfTrayIcon() {
    int size = GetSystemMetrics(SM_CXSMICON);
    if (size <= 0) {
        size = 16;
    }

    HDC screenDc = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screenDc);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP color =
        CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ oldBitmap = SelectObject(dc, color);

    RECT full{0, 0, size, size};
    HBRUSH transparentBrush = CreateSolidBrush(RGB(255, 0, 255));
    FillRect(dc, &full, transparentBrush);
    DeleteObject(transparentBrush);

    HBRUSH trayBrush = CreateSolidBrush(RGB(90, 140, 220));
    HPEN nullPen = static_cast<HPEN>(GetStockObject(NULL_PEN));
    HGDIOBJ oldBrush = SelectObject(dc, trayBrush);
    HGDIOBJ oldPen = SelectObject(dc, nullPen);
    int inset = std::max(1, size / 8);
    RoundRect(dc, inset, inset, size - inset, size - inset, size / 4,
              size / 4);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(trayBrush);

    // Small downward "drop" arrow.
    HPEN arrowPen = CreatePen(PS_SOLID, std::max(1, size / 10),
                              RGB(255, 255, 255));
    HGDIOBJ oldArrowPen = SelectObject(dc, arrowPen);
    int midX = size / 2;
    int top = size / 3;
    int bottom = size - size / 3;
    MoveToEx(dc, midX, top, nullptr);
    LineTo(dc, midX, bottom);
    MoveToEx(dc, midX - size / 6, bottom - size / 6, nullptr);
    LineTo(dc, midX, bottom);
    LineTo(dc, midX + size / 6, bottom - size / 6);
    SelectObject(dc, oldArrowPen);
    DeleteObject(arrowPen);

    SelectObject(dc, oldBitmap);

    HBITMAP mask = CreateBitmap(size, size, 1, 1, nullptr);
    HDC maskDc = CreateCompatibleDC(screenDc);
    HGDIOBJ oldMaskBitmap = SelectObject(maskDc, mask);
    HDC colorDc = CreateCompatibleDC(screenDc);
    HGDIOBJ oldColorBitmap = SelectObject(colorDc, color);
    SetBkColor(colorDc, RGB(255, 0, 255));
    BitBlt(maskDc, 0, 0, size, size, colorDc, 0, 0, SRCCOPY);
    SelectObject(colorDc, oldColorBitmap);
    SelectObject(maskDc, oldMaskBitmap);
    DeleteDC(colorDc);
    DeleteDC(maskDc);

    ICONINFO iconInfo{};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmMask = mask;
    iconInfo.hbmColor = color;
    HICON icon = CreateIconIndirect(&iconInfo);

    DeleteObject(mask);
    DeleteObject(color);
    DeleteDC(dc);
    ReleaseDC(nullptr, screenDc);
    return icon;
}

void AddTrayIcon(HWND window) {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = window;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_SHELF_TRAYICON;
    nid.hIcon = CreateShelfTrayIcon();
    wcsncpy(nid.szTip, L"Drag & Drop Shelf", 127);
    nid.szTip[127] = L'\0';
    Shell_NotifyIconW(NIM_ADD, &nid);
    nid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &nid);
    if (nid.hIcon) {
        DestroyIcon(nid.hIcon);
    }
}

void RemoveTrayIcon(HWND window) {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = window;
    nid.uID = TRAY_ICON_ID;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

LRESULT CALLBACK TrayWndProc(HWND window,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam) {
    if (message == g_taskbarCreatedMessage && g_taskbarCreatedMessage) {
        AddTrayIcon(window);
        return 0;
    }

    switch (message) {
        case WM_SHELF_TRAYICON: {
            UINT uMsg = LOWORD(lParam);
            switch (uMsg) {
                case NIN_SELECT:
                case WM_LBUTTONUP:
                case WM_LBUTTONDBLCLK: {
                    POINT cursor;
                    GetCursorPos(&cursor);
                    TogglePanel(cursor);
                    return 0;
                }
                case WM_CONTEXTMENU:
                case WM_RBUTTONUP: {
                    HMENU menu = CreatePopupMenu();
                    bool isVisible = g_panelWindow && IsWindowVisible(g_panelWindow);
                    AddMenuItem(menu, 1, isVisible ? L"Hide Shelf" : L"Show Shelf");
                    AddMenuItem(menu, 2, L"Clear All Items");
                    AddMenuItem(menu, 3, L"Reset Shelf Position");
                    
                    POINT cursor;
                    GetCursorPos(&cursor);
                    
                    SetForegroundWindow(window);
                    
                    int selected = TrackPopupMenuEx(
                        menu,
                        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
                        cursor.x, cursor.y,
                        window,
                        nullptr
                    );
                    PostMessageW(window, WM_NULL, 0, 0);
                    DestroyMenu(menu);

                    if (selected == 1) {
                        TogglePanel(cursor);
                    } else if (selected == 2) {
                        ClearShelfItems();
                    } else if (selected == 3) {
                        g_hasCustomPos = false;
                        if (g_panelWindow && IsWindowVisible(g_panelWindow)) {
                            ShowPanelNear(cursor);
                        }
                    }
                    return 0;
                }
            }
            return 0;
        }
        case WM_DESTROY:
            RemoveTrayIcon(window);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

// ----------------------------------------------------------------------------
// Dedicated UI thread (own message loop, like Windhawk's companion-window
// mods use, so the shelf doesn't depend on hooking Explorer's own loop)
// ----------------------------------------------------------------------------

const wchar_t kTrayClassName[] = L"DragDropShelfTrayWnd";
const wchar_t kPanelClassName[] = L"DragDropShelfPanelWnd";

DWORD WINAPI ShelfThreadProc(LPVOID) {
    OleInitialize(nullptr);

    HINSTANCE instance = GetModuleHandleW(nullptr);
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSEXW trayClass{sizeof(WNDCLASSEXW)};
    trayClass.lpfnWndProc = TrayWndProc;
    trayClass.hInstance = instance;
    trayClass.lpszClassName = kTrayClassName;
    RegisterClassExW(&trayClass);

    WNDCLASSEXW panelClass{sizeof(WNDCLASSEXW)};
    panelClass.lpfnWndProc = PanelWndProc;
    panelClass.hInstance = instance;
    panelClass.lpszClassName = kPanelClassName;
    panelClass.hbrBackground = nullptr;
    RegisterClassExW(&panelClass);

    g_trayWindow =
        CreateWindowExW(0, kTrayClassName, L"Drag & Drop Shelf", WS_POPUP, 0,
                        0, 0, 0, nullptr, nullptr, instance, nullptr);
    g_panelWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kPanelClassName, L"Shelf",
        WS_POPUP, 0, 0, g_settings.popupWidth, 200, g_trayWindow, nullptr,
        instance, nullptr);

    AddTrayIcon(g_trayWindow);
    LoadShelfPersistence();

    if (g_shelfThreadReady) {
        SetEvent(g_shelfThreadReady);
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    ClearShelfItems();
    if (g_panelWindow) {
        DestroyWindow(g_panelWindow);
        g_panelWindow = nullptr;
    }
    if (g_trayWindow) {
        DestroyWindow(g_trayWindow);
        g_trayWindow = nullptr;
    }
    UnregisterClassW(kPanelClassName, instance);
    UnregisterClassW(kTrayClassName, instance);
    OleUninitialize();
    return 0;
}

bool StartShelfThread() {
    g_shelfThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_shelfThreadReady) {
        return false;
    }
    g_shelfThread =
        CreateThread(nullptr, 0, ShelfThreadProc, nullptr, 0,
                     &g_shelfThreadId);
    if (!g_shelfThread) {
        CloseHandle(g_shelfThreadReady);
        g_shelfThreadReady = nullptr;
        return false;
    }
    DWORD wait = WaitForSingleObject(g_shelfThreadReady, 3000);
    CloseHandle(g_shelfThreadReady);
    g_shelfThreadReady = nullptr;
    return wait == WAIT_OBJECT_0;
}

void StopShelfThread() {
    if (!g_shelfThread) {
        return;
    }
    PostThreadMessageW(g_shelfThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_shelfThread, 5000);
    CloseHandle(g_shelfThread);
    g_shelfThread = nullptr;
    g_shelfThreadId = 0;
}

// ----------------------------------------------------------------------------
// Windhawk mod entry points
// ----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Drag & Drop Shelf");
    LoadSettings();
    if (!StartShelfThread()) {
        Wh_Log(L"Failed to start the shelf UI thread");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Reloading Drag & Drop Shelf settings");
    LoadSettings();
    if (g_panelWindow && IsWindowVisible(g_panelWindow)) {
        ConfigureRoundedPopup(g_panelWindow, IsShelfDarkMode());
        SetWindowPos(g_panelWindow, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        InvalidateRect(g_panelWindow, nullptr, TRUE);
        UpdateWindow(g_panelWindow);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Drag & Drop Shelf");
    StopShelfThread();
}
