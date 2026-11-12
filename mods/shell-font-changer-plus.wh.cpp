// ==WindhawkMod==
// @id              shell-font-changer-plus
// @name            Shell Font Changer Plus
// @description     Any installed font in Explorer, Start, Search and Settings, with icons and emoji left intact
// @version         1.3.3
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @homepage        https://github.com/DavidHiFi/davids-windhawk-mods
// @license         MIT
// @donateUrl       https://ko-fi.com/DavidHiFi
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         ShellExperienceHost.exe
// @include         SearchHost.exe
// @include         ShellHost.exe
// @include         SystemSettings.exe
// @compilerOptions -lgdi32 -luxtheme -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shell Font Changer Plus

Change the text font in File Explorer, Start, Search and Settings to any font
you have installed. Icons and emoji keep their own glyphs, so buttons never
turn into empty boxes.

![Shell Font Changer Plus preview](https://raw.githubusercontent.com/DavidHiFi/davids-windhawk-mods/main/media/previews/shell-font-changer-plus.png)

## Features

- **Shell-wide text.** File Explorer, Start, Search and Settings all follow the
  font you pick.
- **Icons and emoji stay intact.** Symbol and icon fonts keep their own glyphs,
  so buttons never show empty boxes.
- **Weight is kept.** Semibold and bold text stays semibold and bold.
- **Classic, themed and modern text.** GDI text, visual-style themes and
  DirectWrite layouts are all covered.
- **Text fits its controls.** Text that would overflow shrinks a little, to no
  less than 85% of its height or 80% of its width. If it still overflows, the
  original UI font draws it.
- **Clean to remove.** The mod writes nothing to the registry, so turning it off
  leaves nothing behind.

## How to use

Type an installed font family name in the settings, such as `FiraCode Nerd Font`
or `Inter`. Leave the field empty, or type `None`, to turn the mod off.

If the font is not installed, or its name is 32 characters or longer, the mod
does not start. The Windhawk log says why.

Some apps, like Chrome and Edge, draw text with their own fonts and keep them.
Windows that were already open may need to be reopened.

Turn off the original Explorer Font Changer before you turn this one on.

## Credits

Based on [Explorer Font Changer](https://windhawk.net/mods/explorer-font-changer)
by Gabriela Cristei. MIT.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- font:
  - name: FiraCode Nerd Font
    $name: Font family
    $description: An installed font family, such as Inter. Leave empty or type None to keep the original font.
  $name: Text font
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>
#include <dwrite.h>
#include <string>
#include <cwctype>
#include <algorithm>
#include <vector>
#include <map>
#include <cmath>

namespace policy {
std::wstring target;
bool enabled = false;

bool protectedFace(const wchar_t* face, BYTE charset = DEFAULT_CHARSET) {
    if (charset == SYMBOL_CHARSET) return true;
    std::wstring name = face ? face : L"";
    std::transform(name.begin(), name.end(), name.begin(), towlower);
    for (const auto* word : {L"symbol", L"icons", L"mdl2", L"emoji", L"wingdings", L"webdings", L"marlett", L"fontawesome", L"font awesome"}) {
        if (name.find(word) != std::wstring::npos) return true;
    }
    return false;
}

bool protectedText(const wchar_t* text, UINT length) {
    if (!text) return true;
    for (UINT i = 0; i < length; ++i) {
        unsigned c = text[i];
        // PUA and surrogate pairs can encode shell icons, emoji or scripts.
        // Keep the complete original run rather than interpreting glyph IDs.
        if ((c >= 0xE000 && c <= 0xF8FF) || (c >= 0xD800 && c <= 0xDFFF)) return true;
    }
    return false;
}
UINT textLength(const wchar_t* text, int length) {
    return !text ? 0 : length < 0 ? static_cast<UINT>(wcslen(text)) : static_cast<UINT>(length);
}
// GDI draws these format controls as nothing when a face lacks them, so they
// must not force a fallback. Explorer wraps every formatted date in U+200E.
bool invisibleControl(unsigned c) {
    return c < L' ' || c == 0x061C || (c >= 0x200B && c <= 0x200F) || (c >= 0x202A && c <= 0x202E) ||
           (c >= 0x2061 && c <= 0x2064) || (c >= 0x206A && c <= 0x206F) || (c >= 0xFE00 && c <= 0xFE0F) ||
           c == 0xFEFF;
}
int nameWeight(const wchar_t* face) {
    std::wstring name = face ? face : L"";
    std::transform(name.begin(), name.end(), name.begin(), towlower);
    // Longer names first, so " semibold" is not read as " bold".
    const std::pair<const wchar_t*, int> suffixes[] = {
        {L" extrabold", 800}, {L" ultrabold", 800}, {L" semibold", 600}, {L" demibold", 600},
        {L" black", 900}, {L" heavy", 900}, {L" bold", 700}, {L" medium", 500}};
    for (const auto& [suffix, weight] : suffixes) {
        size_t size = wcslen(suffix);
        if (name.size() > size && name.compare(name.size() - size, size, suffix) == 0) return weight;
    }
    return 0;
}

// Round the original height by the fit scale, keeping its sign so a
// character-cell height stays a character-cell height and a cell of at
// least one pixel remains.
LONG fitHeight(LONG height, double scale) {
    if (!height) return height;
    LONG scaled = static_cast<LONG>(std::lround(std::fabs(static_cast<double>(height)) * scale));
    if (scaled < 1) scaled = 1;
    return height < 0 ? -scaled : scaled;
}
}

// Scope the selected font to one call. Restoring it before DeleteObject is
// required even for DT_CALCRECT, nested drawing calls and failed draws.
class FontScope {
    HDC dc = nullptr;
    HFONT replacement = nullptr;
    HGDIOBJ previous = nullptr;
    LOGFONTW font{};  // target face at the original height
    double scale = 1.0;
    LONG naturalWidth = 0;
    const wchar_t* text = nullptr;
    UINT length = 0;

    HFONT build() const {
        LOGFONTW scaled = font;
        scaled.lfHeight = policy::fitHeight(font.lfHeight, scale);
        return CreateFontIndirectW(&scaled);
    }
public:
    FontScope(HDC hdc, const wchar_t* text, UINT length, bool glyphIndices = false)
        : text(text), length(length) {
        if (!policy::enabled || !hdc || glyphIndices || policy::protectedText(text, length)) return;
        LOGFONTW font{};
        if (GetObjectW(GetCurrentObject(hdc, OBJ_FONT), sizeof(font), &font) != sizeof(font)) return;
        // '@' faces are vertical CJK layouts; a horizontal face would rotate them.
        if (font.lfFaceName[0] == L'@' || policy::protectedFace(font.lfFaceName, font.lfCharSet) ||
            _wcsicmp(font.lfFaceName, policy::target.c_str()) == 0) return;
        // Face names such as "Segoe UI Black" carry their weight in the name.
        TEXTMETRICW metrics{};
        if (GetTextMetricsW(hdc, &metrics) && metrics.tmWeight > font.lfWeight) font.lfWeight = metrics.tmWeight;
        font.lfWeight = std::max<LONG>(font.lfWeight, policy::nameWeight(font.lfFaceName));
        // A script charset the target lacks would make the mapper pick another face.
        font.lfCharSet = DEFAULT_CHARSET;
        scale = 1.0;
        // A stretched or raster average width belongs to the original face.
        font.lfWidth = 0;
        wcscpy_s(font.lfFaceName, policy::target.c_str());
        this->font = font;
        replacement = build();
        if (!replacement) return;
        previous = SelectObject(hdc, replacement);
        if (!previous || previous == HGDI_ERROR) {
            DeleteObject(replacement);
            replacement = nullptr;
            return;
        }
        dc = hdc;
        TEXTMETRICW natural{};if(GetTextMetricsW(dc,&natural)) naturalWidth=natural.tmAveCharWidth;
        wchar_t realized[LF_FACESIZE]{};
        // DirectWrite family names are not always GDI face names. Never let
        // the mapper substitute a third font for a name GDI does not know.
        bool missing = !GetTextFaceW(dc, LF_FACESIZE, realized) || _wcsicmp(realized, policy::target.c_str()) != 0;
        if (!missing && length) {
            std::vector<WORD> glyphs(length);
            missing = GetGlyphIndicesW(dc, text, length, glyphs.data(), GGI_MARK_NONEXISTING_GLYPHS) == GDI_ERROR;
            for (UINT i = 0; !missing && i < length; ++i) {
                if (glyphs[i] == 0xffff && !policy::invisibleControl(text[i])) missing = true;
            }
        }
        // GDI font linking varies by family. Preserve the original run
        // when the replacement lacks a character instead of drawing tofu.
        if (missing) {
            SelectObject(dc, previous);
            DeleteObject(replacement);
            replacement = nullptr;
            dc = nullptr;
        }
    }
    ~FontScope() {
        if (dc) SelectObject(dc, previous);
        if (replacement) DeleteObject(replacement);
    }
    FontScope(const FontScope&) = delete;
    FontScope& operator=(const FontScope&) = delete;

    static constexpr double kMinWidth = 0.8;
    static constexpr double kMinScale = 0.85;
    bool active() const { return dc != nullptr; }
    // Give the run back to the original font. Its metrics are what the
    // control was laid out for, so it fits where the target cannot.
    void revert() {
        if (!dc) return;
        SelectObject(dc, previous);
        DeleteObject(replacement);
        replacement = nullptr;
        dc = nullptr;
    }
    double currentScale() const { return scale; }
    // Rebuild the replacement at a smaller scale. Returns false when the
    // requested scale is not smaller or the rebuild fails, so the caller
    // stops shrinking; on failure the previous font stays selected.
    bool condense(double ratio) {
        if (!dc || ratio >= 1.0 || ratio <= 0) return false;
        TEXTMETRICW metrics{};
        if (!GetTextMetricsW(dc,&metrics)) return false;
        LONG width = std::max<LONG>(1,static_cast<LONG>(std::floor(metrics.tmAveCharWidth * ratio)));
        // Narrower than this is unreadable; the caller falls back to the original face.
        width=std::max<LONG>(width,static_cast<LONG>(std::ceil(naturalWidth * kMinWidth)));
        if (width >= metrics.tmAveCharWidth) return false;
        LONG saved = font.lfWidth;
        font.lfWidth = width;
        HFONT rebuilt = build();
        if (!rebuilt) {font.lfWidth=saved;return false;}
        HGDIOBJ outgoing=SelectObject(dc,rebuilt);
        if (!outgoing || outgoing==HGDI_ERROR) {DeleteObject(rebuilt);font.lfWidth=saved;return false;}
        if (outgoing!=previous) DeleteObject(outgoing);
        replacement=rebuilt;
        return true;
    }
    bool rescale(double next) {
        if (!dc || !(next < scale) || next < kMinScale) return false;
        double prior = scale;
        scale = next;
        HFONT rebuilt = build();
        if (!rebuilt) {
            scale = prior;
            return false;
        }
        HGDIOBJ outgoing = SelectObject(dc, rebuilt);
        if (!outgoing || outgoing == HGDI_ERROR) {
            DeleteObject(rebuilt);
            scale = prior;
            return false;
        }
        // `outgoing` is the previous replacement; the original font is
        // still owned by `previous` and is restored by the destructor.
        if (outgoing != previous) DeleteObject(outgoing);
        replacement = rebuilt;
        return true;
    }
};

decltype(&DrawTextW) drawTextOriginal;
decltype(&DrawTextExW) drawTextExOriginal;
decltype(&GetTextExtentPoint32W) extentOriginal;

// Shrink a substituted font, within readable limits, until the block fits
// the box the application gave it, else hand it back to the original font. Fixed-size boxes are designed around the original font, so this
// guard is what stops buttons, labels and static text from clipping.
void fitBox(FontScope& scope, HDC dc, LPCWSTR text, UINT length, const RECT* rect, UINT flags,
            LPDRAWTEXTPARAMS params, bool probeHeight) {
    if (!scope.active() || !rect || !length) return;
    // Measuring calls and ellipsis draws report or truncate rather than clip.
    if (flags & (DT_CALCRECT | DT_END_ELLIPSIS | DT_PATH_ELLIPSIS | DT_WORD_ELLIPSIS)) return;
    // Some controls (the Properties checkboxes) pass a box wider than the
    // window they draw in; anything past the window's client edge is clipped.
    RECT visible = *rect;
    RECT client{};
    bool bounded = false;
    if (HWND window = WindowFromDC(dc); window && GetClientRect(window, &client))
        bounded = true;
    // Themed controls draw into a buffered-paint bitmap the size of the
    // control; its clip box is that size. Skip the 1x1 stock bitmap.
    else if (GetObjectType(dc) == OBJ_MEMDC && GetClipBox(dc, &client) != ERROR && client.right - client.left > 1)
        bounded = true;
    if (bounded) {
        const LONG left = std::max(visible.left, client.left), right = std::min(visible.right, client.right);
        if (flags & DT_CENTER) {
            // Centred text grows from the box's centre, so the nearer edge limits both sides.
            const LONG centre = (visible.left + visible.right) / 2;
            const LONG half = std::min(centre - left, right - centre);
            if (half > 0) { visible.left = centre - half; visible.right = centre + half; }
        } else if (right > left) {
            visible.left = left;
            visible.right = right;
        }
    }
    rect = &visible;
    const int boxWidth = rect->right - rect->left - (params ? params->iLeftMargin + params->iRightMargin : 0);
    const int boxHeight = rect->bottom - rect->top;
    if (boxWidth <= 0 || boxHeight <= 0) return;
    const bool singleLine = (flags & DT_SINGLELINE) != 0;
    for (int tries = 0; tries < 16 && scope.active(); ++tries) {
        if (singleLine) {
            SIZE size{};
            if (extentOriginal && extentOriginal(dc, text, static_cast<int>(length), &size) && size.cx > boxWidth) {
                if (scope.condense(static_cast<double>(boxWidth) / size.cx)) continue;
                scope.revert();
                break;
            }
            TEXTMETRICW metrics{};
            if (GetTextMetricsW(dc, &metrics)) {
                const int line = metrics.tmHeight;
                if (line > boxHeight) {
                    if (scope.rescale(scope.currentScale() * static_cast<double>(boxHeight) / line * 0.98)) continue;
                    scope.revert();
                    break;
                }
            }
            break;
        }
        if (!probeHeight || !(drawTextOriginal || (params && drawTextExOriginal))) break;
        // Wrapped text re-flows inside the width, so only the block height
        // can still overrun the box. DT_CALCRECT measures without drawing.
        RECT probe = *rect;
        const UINT probeFlags = (flags | DT_CALCRECT) & ~DT_MODIFYSTRING;
        if (params && drawTextExOriginal) {
            DRAWTEXTPARAMS probeParams = *params;
            drawTextExOriginal(dc, const_cast<LPWSTR>(text), static_cast<int>(length), &probe, probeFlags, &probeParams);
        } else {
            drawTextOriginal(dc, text, static_cast<int>(length), &probe, probeFlags);
        }
        const int needed = probe.bottom - probe.top;
        double ratio = 1.0;
        if (needed > boxHeight) ratio = std::min(ratio, static_cast<double>(boxHeight) / needed);
        const int neededWidth = probe.right - probe.left;
        if (neededWidth > rect->right - rect->left)
            ratio = std::min(ratio, static_cast<double>(rect->right - rect->left) / neededWidth);
        if (ratio < 1.0) {
            if (scope.condense(ratio)) continue;
            if (scope.rescale(scope.currentScale() * ratio * 0.98)) continue;
            scope.revert();
        }
        break;
    }
}

int WINAPI drawTextHook(HDC dc, LPCWSTR text, int length, LPRECT rect, UINT flags) {
    FontScope font(dc, text, policy::textLength(text, length));
    fitBox(font, dc, text, policy::textLength(text, length), rect, flags, nullptr, true);
    return drawTextOriginal(dc, text, length, rect, flags);
}
int WINAPI drawTextExHook(HDC dc, LPWSTR text, int length, LPRECT rect, UINT flags, LPDRAWTEXTPARAMS params) {
    FontScope font(dc, text, policy::textLength(text, length));
    fitBox(font, dc, text, policy::textLength(text, length), rect, flags, params, true);
    return drawTextExOriginal(dc, text, length, rect, flags, params);
}
decltype(&TextOutW) textOutOriginal;
BOOL WINAPI textOutHook(HDC dc, int x, int y, LPCWSTR text, int length) {
    FontScope font(dc, text, length < 0 ? 0 : length);
    return textOutOriginal(dc, x, y, text, length);
}
decltype(&ExtTextOutW) extTextOutOriginal;
BOOL WINAPI extTextOutHook(HDC dc, int x, int y, UINT flags, const RECT* rect, LPCWSTR text, UINT length, const INT* widths) {
    // Pre-shaped glyph indices belong to the original face, not the new face.
    FontScope font(dc, text, length, (flags & ETO_GLYPH_INDEX) != 0 || widths != nullptr);
    return extTextOutOriginal(dc, x, y, flags, rect, text, length, widths);
}
BOOL WINAPI extentHook(HDC dc, LPCWSTR text, int length, LPSIZE size) {
    FontScope font(dc, text, length < 0 ? 0 : length);
    return extentOriginal(dc, text, length, size);
}
decltype(&GetTextExtentExPointW) extentExOriginal;
BOOL WINAPI extentExHook(HDC dc, LPCWSTR text, int length, int maxExtent, LPINT fit, LPINT widths, LPSIZE size) {
    FontScope font(dc, text, length < 0 ? 0 : length);
    return extentExOriginal(dc, text, length, maxExtent, fit, widths, size);
}
decltype(&DrawThemeText) themeOriginal;
HRESULT WINAPI themeHook(HTHEME theme, HDC dc, int part, int state, LPCWSTR text, int length, DWORD flags, DWORD flags2, const RECT* rect) {
    FontScope font(dc, text, policy::textLength(text, length));
    fitBox(font, dc, text, policy::textLength(text, length), rect, flags, nullptr, true);
    return themeOriginal(theme, dc, part, state, text, length, flags, flags2, rect);
}
decltype(&DrawThemeTextEx) themeExOriginal;
HRESULT WINAPI themeExHook(HTHEME theme, HDC dc, int part, int state, LPCWSTR text, int length, DWORD flags, LPRECT rect, const DTTOPTS* opts) {
    FontScope font(dc, text, policy::textLength(text, length));
    fitBox(font, dc, text, policy::textLength(text, length), rect, flags, nullptr, true);
    return themeExOriginal(theme, dc, part, state, text, length, flags, rect, opts);
}

void updateLayout(IDWriteTextLayout* layout, const WCHAR* text, UINT length, IDWriteTextFormat* format) {
    if (!policy::enabled || !layout || !format || !length || policy::protectedText(text, length)) return;
    std::wstring family(format->GetFontFamilyNameLength() + 1, L'\0');
    if (FAILED(format->GetFontFamilyName(family.data(), static_cast<UINT>(family.size()))) || policy::protectedFace(family.c_str())) return;
    IDWriteFontCollection* collection = nullptr;
    if (FAILED(format->GetFontCollection(&collection)) || !collection) return;
    UINT index = 0;
    BOOL exists = FALSE;
    collection->FindFamilyName(policy::target.c_str(), &index, &exists);
    collection->Release();
    // A custom font collection without the target keeps its original family.
    if (!exists) return;
    DWRITE_TEXT_RANGE range{0, length};
    // Legacy GDI names such as "Segoe UI Semibold" carry their weight in the
    // name, and DirectWrite does not resolve them. Keep that weight.
    auto weight = static_cast<DWRITE_FONT_WEIGHT>(policy::nameWeight(family.c_str()));
    if (weight > format->GetFontWeight()) layout->SetFontWeight(weight, range);
    layout->SetFontFamilyName(policy::target.c_str(), range);
    float size = format->GetFontSize();
    // Keep the requested size unless the actual control bounds require a reduction.
    const float widthLimit = layout->GetMaxWidth();
    const float heightLimit = layout->GetMaxHeight();
    for (int attempt = 0; attempt < 16; ++attempt) {
        DWRITE_TEXT_METRICS metrics{};
        if (FAILED(layout->GetMetrics(&metrics))) break;
        float ratio = 1.0f;
        if (widthLimit > 0 && metrics.widthIncludingTrailingWhitespace > widthLimit)
            ratio = std::min(ratio, widthLimit / metrics.widthIncludingTrailingWhitespace);
        if (heightLimit > 0 && metrics.height > heightLimit)
            ratio = std::min(ratio, heightLimit / metrics.height);
        if (ratio >= 0.999f) break;
        size *= ratio * 0.98f;
        if (FAILED(layout->SetFontSize(size, range))) break;
    }
}
using LayoutFn = HRESULT (STDMETHODCALLTYPE*)(IDWriteFactory*, const WCHAR*, UINT32, IDWriteTextFormat*, FLOAT, FLOAT, IDWriteTextLayout**);
LayoutFn layoutOriginal;
HRESULT STDMETHODCALLTYPE layoutHook(IDWriteFactory* factory, const WCHAR* text, UINT32 length, IDWriteTextFormat* format, FLOAT width, FLOAT height, IDWriteTextLayout** output) {
    auto hr = layoutOriginal(factory, text, length, format, width, height, output);
    if (SUCCEEDED(hr) && output) updateLayout(*output, text, length, format);
    return hr;
}
using GdiLayoutFn = HRESULT (STDMETHODCALLTYPE*)(IDWriteFactory*, const WCHAR*, UINT32, IDWriteTextFormat*, FLOAT, FLOAT, FLOAT, const DWRITE_MATRIX*, BOOL, IDWriteTextLayout**);
GdiLayoutFn gdiLayoutOriginal;
HRESULT STDMETHODCALLTYPE gdiLayoutHook(IDWriteFactory* factory, const WCHAR* text, UINT32 length, IDWriteTextFormat* format, FLOAT width, FLOAT height, FLOAT scale, const DWRITE_MATRIX* transform, BOOL natural, IDWriteTextLayout** output) {
    auto hr = gdiLayoutOriginal(factory, text, length, format, width, height, scale, transform, natural, output);
    if (SUCCEEDED(hr) && output) updateLayout(*output, text, length, format);
    return hr;
}

bool loadSettings() {
    const auto* name = Wh_GetStringSetting(L"font.name");
    policy::target = name ? name : L"";
    Wh_FreeStringSetting(name);
    policy::enabled = !policy::target.empty() && _wcsicmp(policy::target.c_str(), L"None") != 0;
    if (!policy::enabled) return true;
    if (policy::target.size() >= LF_FACESIZE || policy::protectedFace(policy::target.c_str())) {
        Wh_Log(L"Choose a text font family shorter than 32 characters.");
        return false;
    }
    IDWriteFactory* factory = nullptr;
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&factory)))) return false;
    IDWriteFontCollection* fonts = nullptr;
    BOOL exists = FALSE;
    UINT index = 0;
    if (SUCCEEDED(factory->GetSystemFontCollection(&fonts))) {
        fonts->FindFamilyName(policy::target.c_str(), &index, &exists);
        fonts->Release();
    }
    factory->Release();
    if (!exists) Wh_Log(L"Font family is not installed: %s", policy::target.c_str());
    return exists != FALSE;
}

BOOL Wh_ModInit() {
    if (!loadSettings()) return FALSE;
    bool ok = true;
    #define HOOK(fn, hook, original) ok = Wh_SetFunctionHook(reinterpret_cast<void*>(fn), reinterpret_cast<void*>(hook), reinterpret_cast<void**>(&original)) && ok
    auto user = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto gdi = LoadLibraryExW(L"gdi32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto theme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!user || !gdi || !theme) return FALSE;
    // Explicit exports avoid MinGW import thunks inside this mod DLL.
    HOOK(GetProcAddress(user, "DrawTextW"), drawTextHook, drawTextOriginal);
    HOOK(GetProcAddress(user, "DrawTextExW"), drawTextExHook, drawTextExOriginal);
    HOOK(GetProcAddress(gdi, "TextOutW"), textOutHook, textOutOriginal);
    HOOK(GetProcAddress(gdi, "ExtTextOutW"), extTextOutHook, extTextOutOriginal);
    HOOK(GetProcAddress(gdi, "GetTextExtentPoint32W"), extentHook, extentOriginal);
    HOOK(GetProcAddress(gdi, "GetTextExtentExPointW"), extentExHook, extentExOriginal);
    HOOK(GetProcAddress(theme, "DrawThemeText"), themeHook, themeOriginal);
    HOOK(GetProcAddress(theme, "DrawThemeTextEx"), themeExHook, themeExOriginal);
    IDWriteFactory* factory = nullptr;
    if (SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&factory)))) {
        // IDWriteFactory: IUnknown 0..2; the two layout methods are 18 and 19.
        // Format objects stay untouched, so cached icon formats remain usable.
        auto table = *reinterpret_cast<void***>(factory);
        HOOK(table[18], layoutHook, layoutOriginal);
        HOOK(table[19], gdiLayoutHook, gdiLayoutOriginal);
        factory->Release();
    } else {
        ok = false;
    }
    #undef HOOK
    Wh_Log(L"Shell font changer initialized. Font: %s", policy::target.c_str());
    return ok;
}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
