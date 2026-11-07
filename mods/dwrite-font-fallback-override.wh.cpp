// ==WindhawkMod==
// @id              dwrite-font-fallback-override
// @name            Font Fallback Override
// @description     Intercepts DirectWrite font fallback and GDI text draw routines to replace fonts for specific characters
// @version         2.0
// @license         MIT
// @author          Muhammad Ragib Hasin
// @github          https://github.com/RagibHasin
// @include         *
// @compilerOptions -luser32 -lgdi32 -ldwrite -lole32 -lwindowsapp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Font Fallback Override

Hooks DirectWrite's font fallback system to override font selection for specific
Unicode character ranges across all applications.

## Features

  - Per-Unicode-range fallback rules.
  - Works with...
    - classic GDI APIs: `DrawTextW`, `DrawTextExW`, `DrawThemeText`,
      `DrawThemeTextEx`, `DrawShadowText`, `TextOutW` and `ExtTextOutW`.
    - all DirectWrite APIs.
  - Preserves the original font's size, weight, italic and charset.
  - Detects whether the currently selected font contains a glyph and only
    replaces the font if it does not.

## Example

Configure:

U+0980-U+09FF -> Noto Sans Bengali
U+4E00-U+9FFF -> Microsoft YaHei

A string such as:

    "Hello বাংলা 世界"

can therefore be rendered as:

    original font -> Bengali fallback -> original font -> CJK fallback

| Before            | After           |
| ----------------- | --------------- |
| ![Before][before] | ![After][after] |

## Notice

This mod only works on Windows 8.1 or later.

[before]:
https://github.com/RagibHasin/font-fallback-override.wh.cpp/blob/master/assets/before.png?raw=true
[after]:
https://github.com/RagibHasin/font-fallback-override.wh.cpp/blob/master/assets/after.png?raw=true
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- rules:
  - - startChar: "0980"
      $name: Start Character (Unicode hex, e.g., 4E00)
    - endChar: "09FF"
      $name: End Character (Unicode hex, e.g., 9FFF)
    - fontFamily: "Noto Sans Bengali"
      $name: Font Family Name
  $name: Fallback Rules
  $description: Define custom font fallback rules for character ranges
*/
// ==/WindhawkModSettings==

#include <dwrite.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#include <unknwn.h>
#include <winrt/base.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>

// WHAM@5 begin 1: src/includes/dwrite/hooks/CreateTextFormat.h
// WHAM@5 transform 3: guard-open
#ifndef WHAM_ONCE_5FB280520F4B3E886797F130
#define WHAM_ONCE_5FB280520F4B3E886797F130
// WHAM@5 end-transform 3
// WHAM@5 transform 2: pragma-once
// WHAM@5 end-transform 2

#include <dwrite.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#include <winrt/base.h>

using winrt::com_ptr;

using CreateTextFormat0_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteFactory8* self,
    const WCHAR* family_name,
    IDWriteFontCollection* collection,
    DWRITE_FONT_WEIGHT weight,
    DWRITE_FONT_STYLE style,
    DWRITE_FONT_STRETCH stretch,
    FLOAT size,
    const WCHAR* locale,
    IDWriteTextFormat** format
);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
CreateTextFormat0_t g_originalCreateTextFormat0 = nullptr;

STDMETHODIMP CreateTextFormat0_Hook(
    IDWriteFactory8* self,
    WCHAR const* fontFamilyName,
    IDWriteFontCollection* fontCollection,
    DWRITE_FONT_WEIGHT fontWeight,
    DWRITE_FONT_STYLE fontStyle,
    DWRITE_FONT_STRETCH fontStretch,
    FLOAT fontSize,
    WCHAR const* localeName,
    IDWriteTextFormat** textFormat
) {
    Wh_Log(L"IDWriteFacotry::CreateTextFormat");
    auto hr = g_originalCreateTextFormat0(
        self,
        fontFamilyName,
        fontCollection,
        fontWeight,
        fontStyle,
        fontStretch,
        fontSize,
        localeName,
        textFormat
    );

    if (SUCCEEDED(hr) && textFormat && *textFormat) {
        // Try to set custom fallback on the text layout
        com_ptr<IDWriteTextFormat> format;
        format.copy_from(*textFormat);

        auto format3 = format.try_as<IDWriteTextFormat3>();
        if (format3) {
            com_ptr<IDWriteFontFallback> customFallback;
            self->GetSystemFontFallback(customFallback.put());

            format3->SetFontFallback(customFallback.get());

            Wh_Log(L"CreateTextFormat: Injected fallback into text format");
        }
    }
    return hr;
}

using CreateTextFormat6_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteFactory8* self,
    const WCHAR* familyname,
    IDWriteFontCollection* collection,
    const DWRITE_FONT_AXIS_VALUE* axis_values,
    UINT32 num_axis,
    FLOAT fontsize,
    const WCHAR* localename,
    IDWriteTextFormat3** text_format
);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
CreateTextFormat6_t g_originalCreateTextFormat6 = nullptr;

STDMETHODIMP CreateTextFormat6_Hook(
    IDWriteFactory8* self,
    const WCHAR* familyname,
    IDWriteFontCollection* collection,
    const DWRITE_FONT_AXIS_VALUE* axis_values,
    UINT32 num_axis,
    FLOAT fontsize,
    const WCHAR* localename,
    IDWriteTextFormat3** text_format
) {
    Wh_Log(L"IDWriteFactory6::CreateTextFormat");
    auto hr = g_originalCreateTextFormat6(
        self, familyname, collection, axis_values, num_axis, fontsize, localename, text_format
    );
    if (SUCCEEDED(hr) && text_format && *text_format) {
        // Try to set custom fallback on the text format
        com_ptr<IDWriteFontFallback> customFallback;
        self->GetSystemFontFallback(customFallback.put());
        (*text_format)->SetFontFallback(customFallback.get());

        Wh_Log(L"CreateTextFormat: Injected fallback into text format");
    }
    return hr;
}
// WHAM@5 transform 4: guard-close
#endif // WHAM_ONCE_5FB280520F4B3E886797F130
// WHAM@5 end-transform 4
// WHAM@5 end 1
// WHAM@5 begin 5: src/includes/dwrite/hooks/CreateTextLayout.h
// WHAM@5 transform 7: guard-open
#ifndef WHAM_ONCE_5A5E1BC89048DB5EA03C578D
#define WHAM_ONCE_5A5E1BC89048DB5EA03C578D
// WHAM@5 end-transform 7
// WHAM@5 transform 6: pragma-once
// WHAM@5 end-transform 6

#include <dwrite.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#include <winrt/base.h>

using winrt::com_ptr;

using CreateTextLayout0_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteFactory8* self,
    WCHAR const* string,
    UINT32 stringLength,
    IDWriteTextFormat* textFormat,
    FLOAT maxWidth,
    FLOAT maxHeight,
    IDWriteTextLayout** textLayout
);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
CreateTextLayout0_t g_originalCreateTextLayout0 = nullptr;

STDMETHODIMP CreateTextLayout0_Hook(
    IDWriteFactory8* self,
    WCHAR const* string,
    UINT32 stringLength,
    IDWriteTextFormat* textFormat,
    FLOAT maxWidth,
    FLOAT maxHeight,
    IDWriteTextLayout** textLayout
) {
    Wh_Log(L"IDWriteFactory::CreateTextLayout");
    auto hr = g_originalCreateTextLayout0(
        self, string, stringLength, textFormat, maxWidth, maxHeight, textLayout
    );
    if (SUCCEEDED(hr) && textLayout && *textLayout) {
        // Try to set custom fallback on the text layout
        com_ptr<IDWriteTextLayout> layout;
        layout.copy_from(*textLayout);

        auto layout4 = layout.try_as<IDWriteTextLayout4>();
        if (layout4) {
            com_ptr<IDWriteFontFallback> customFallback;
            self->GetSystemFontFallback(customFallback.put());
            layout4->SetFontFallback(customFallback.get());

            Wh_Log(L"CreateTextLayout: Injected fallback into text layout");
        }
    }
    return hr;
}
// WHAM@5 transform 8: guard-close
#endif // WHAM_ONCE_5A5E1BC89048DB5EA03C578D
// WHAM@5 end-transform 8
// WHAM@5 end 5
// WHAM@5 begin 9: src/includes/dwrite/hooks/GetSystemFontFallback.h
// WHAM@5 transform 29: guard-open
#ifndef WHAM_ONCE_D23B9011C36ABC92BA38DD66
#define WHAM_ONCE_D23B9011C36ABC92BA38DD66
// WHAM@5 end-transform 29
// WHAM@5 transform 10: pragma-once
// WHAM@5 end-transform 10

#include <dwrite.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#include <winrt/base.h>

// WHAM@5 begin 11: src/includes/fallback_rule.h
// WHAM@5 transform 26: guard-open
#ifndef WHAM_ONCE_DCF41FB6E61DA6C9C499C9C8
#define WHAM_ONCE_DCF41FB6E61DA6C9C499C9C8
// WHAM@5 end-transform 26
// WHAM@5 transform 12: pragma-once
// WHAM@5 end-transform 12

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <windhawk_utils.h>

// WHAM@5 begin 13: src/includes/gdi/font_cache.h
// WHAM@5 transform 19: guard-open
#ifndef WHAM_ONCE_11FE0A21086DAA0DF4227EC8
#define WHAM_ONCE_11FE0A21086DAA0DF4227EC8
// WHAM@5 end-transform 19
// WHAM@5 transform 14: pragma-once
// WHAM@5 end-transform 14

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <variant>

// WHAM@5 begin 15: src/includes/utils.h
// WHAM@5 transform 17: guard-open
#ifndef WHAM_ONCE_907797D9C34DD4CA1E313972
#define WHAM_ONCE_907797D9C34DD4CA1E313972
// WHAM@5 end-transform 17
// WHAM@5 transform 16: pragma-once
// WHAM@5 end-transform 16

#include <cstdint>
#include <cwchar>
#include <mutex>
#include <shared_mutex>
#include <string_view>
#include <utility>

#ifdef LOG_MUTEX
#include <source_location>
#include <sstream>
#include <string>

#include <windhawk_api.h>
#endif

constexpr auto hiSurrogateBase = 0xD800UL;
constexpr auto loSurrogateBase = 0xDC00UL;
constexpr auto postSurrogateBase = 0xE000UL;
constexpr auto suppmentaryPlaneBase = 0x10000UL;

/// Unicode decoding.
///
/// GDI APIs expose UTF-16 strings. The fallback rules use Unicode scalar
/// values, so combine surrogate pairs before testing ranges.
///
/// Invalid/unpaired surrogates are returned as their UTF-16 value. This makes
/// the behavior deterministic rather than silently discarding them.
static std::pair<
    char32_t,
    size_t>
DecodeCodePoint(const std::wstring_view text) {
    auto ch = static_cast<uint32_t>(text[0]);

    if (ch >= hiSurrogateBase && ch < loSurrogateBase && 1 < text.size()) {
        auto loSurrogate = static_cast<uint32_t>(text[1]);

        constexpr auto hiSurrogateShiftMask = 10UL;

        if (loSurrogate >= loSurrogateBase && loSurrogate < postSurrogateBase) {
            return {
                suppmentaryPlaneBase + ((ch - hiSurrogateBase) << hiSurrogateShiftMask)
                    + (loSurrogate - loSurrogateBase),
                2
            };
        }
    }

    return {ch, 1};
}

static char32_t ParseHex(const WCHAR* str) {
    if (!str || !*str) {
        return 0;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    return std::wcstoul(str, nullptr, 16);
}

// MARK: Excerpt from https://github.com/dragazo/rustex begins

// a rust-style mutex - holds a value of type T and manages access via guards around SharedLockable.
// in order to ensure the soundness of the data contract, you should not access the stored value
// except directly via the handles.
template <typename T, typename SharedLockable = std::shared_mutex>
class Mutex {
  private: // -- detail -- //
    template <typename T_cv, template <typename> typename Locker>
    class guard_common {
      private: // -- detail -- //
        T_cv* ptr;
        Locker<SharedLockable> lock;
#ifdef LOG_MUTEX
        std::source_location _location;
#endif

        friend Mutex;
        explicit guard_common(
            const Mutex& m
#ifdef LOG_MUTEX
            ,
            std::source_location location = std::source_location::current()
#endif
        )
            : ptr(std::addressof(m.raw_val))
            , lock(m.raw_mut)
#ifdef LOG_MUTEX
            , _location(location)
#endif
        {
#ifdef LOG_MUTEX
            std::wostringstream trace;
            trace << "file: " << _location.file_name() << '(' << _location.line() << ':'
                  << _location.column() << ") `" << _location.function_name() << "`: ";
            auto traceStr = trace.str();
            Wh_Log(L"%s : locked mutex", traceStr.c_str());
#endif
        }

      public: // -- access -- //
        T_cv* operator->() const noexcept {
            return ptr;
        }
        T_cv& operator*() const noexcept {
            return *ptr;
        }

#ifdef LOG_MUTEX
        guard_common(const guard_common&) = delete;
        guard_common(guard_common&&) = delete;
        guard_common& operator=(const guard_common&) = delete;
        guard_common& operator=(guard_common&&) = delete;

        ~guard_common() {
            std::wostringstream trace;
            trace << "file: " << _location.file_name() << '(' << _location.line() << ':'
                  << _location.column() << ") `" << _location.function_name() << "`: ";
            auto traceStr = trace.str();
            Wh_Log(L"%s : unlocked mutex", traceStr.c_str());
        }
#endif
    };

    mutable SharedLockable raw_mut;
    mutable T raw_val;

  public: // -- guards -- //
    // when these guards are moved-from they are considered invalid and should not be used at all
    // afterwards (like rust)
    using mut_guard = guard_common<T, std::lock_guard>;
    using guard = guard_common<const T, std::shared_lock>;

    // -- intrinsics -- //
    // constructs the wrapped object of type T with the specified arguments
    template <typename... Args>
    constexpr explicit Mutex(Args&&... args)
        requires(std::is_constructible_v<
                 T,
                 Args...>)
        : raw_val(std::forward<Args>(args)...) {
    }

    Mutex(const Mutex&) = delete;
    Mutex(Mutex&&) = delete;
    ~Mutex() = default;

    Mutex& operator=(const Mutex&) = delete;
    Mutex& operator=(Mutex&&) = delete;

    // -- access -- //
    // returns a handle that facilitates shared readonly access of the wrapped value
    [[nodiscard]] guard lock(
#ifdef LOG_MUTEX
        std::source_location location = std::source_location::current()
#endif
    ) const {
        return guard{
            *this
#ifdef LOG_MUTEX
            ,
            location
#endif
        };
    }
    // returns a handle that facilitates unique read/write access of the wrapped value
    [[nodiscard]] mut_guard lock_mut(
#ifdef LOG_MUTEX
        std::source_location location = std::source_location::current()
#endif
    ) const {
        return mut_guard{
            *this
#ifdef LOG_MUTEX
            ,
            location
#endif
        };
    }
};

// MARK: Excerpt from https://github.com/dragazo/rustex ends
// WHAM@5 transform 18: guard-close
#endif // WHAM_ONCE_907797D9C34DD4CA1E313972
// WHAM@5 end-transform 18
// WHAM@5 end 15

// Replacement font cache.
//
// A replacement HFONT must have approximately the same metrics/style as the
// original font. We therefore derive LOGFONTW from the currently selected font
// and only replace lfFaceName.
//
// Cache key is original HFONT + replacement family.
//
// The cache is intentionally simple. Font handles are generally long-lived in
// classic Win32 applications.

struct FontCacheKey {
    LOGFONTW originalFont;
    std::wstring family;

    bool operator==(const FontCacheKey& other) const {
        return family == other.family && originalFont.lfHeight == other.originalFont.lfHeight
               && originalFont.lfWidth == other.originalFont.lfWidth
               && originalFont.lfEscapement == other.originalFont.lfEscapement
               && originalFont.lfOrientation == other.originalFont.lfOrientation
               && originalFont.lfWeight == other.originalFont.lfWeight
               && originalFont.lfItalic == other.originalFont.lfItalic
               && originalFont.lfUnderline == other.originalFont.lfUnderline
               && originalFont.lfStrikeOut == other.originalFont.lfStrikeOut
               && originalFont.lfCharSet == other.originalFont.lfCharSet
               && originalFont.lfOutPrecision == other.originalFont.lfOutPrecision
               && originalFont.lfClipPrecision == other.originalFont.lfClipPrecision
               && originalFont.lfQuality == other.originalFont.lfQuality
               && originalFont.lfPitchAndFamily == other.originalFont.lfPitchAndFamily
               && std::wstring_view(static_cast<const wchar_t*>(originalFont.lfFaceName))
                      == std::wstring_view(
                          static_cast<const wchar_t*>(other.originalFont.lfFaceName)
                      );
    }
};

template <>
class std::hash<FontCacheKey> {
  public:
    static constexpr size_t operator()(const FontCacheKey& key) {
        auto hash = 0UZ;

        auto combine = [&](const auto& v) {
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
            hash ^= std::hash<std::remove_cvref_t<decltype(v)>>{}(v) + 0x9e3779b9 + (hash << 6)
                    + (hash >> 2);
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
        };

        combine(key.originalFont.lfHeight);
        combine(key.originalFont.lfWidth);
        combine(key.originalFont.lfEscapement);
        combine(key.originalFont.lfOrientation);
        combine(key.originalFont.lfWeight);
        combine(key.originalFont.lfItalic);
        combine(key.originalFont.lfUnderline);
        combine(key.originalFont.lfStrikeOut);
        combine(key.originalFont.lfCharSet);
        combine(key.originalFont.lfOutPrecision);
        combine(key.originalFont.lfClipPrecision);
        combine(key.originalFont.lfQuality);
        combine(key.originalFont.lfPitchAndFamily);
        combine(std::wstring_view(static_cast<const wchar_t*>(key.originalFont.lfFaceName)));
        combine(key.family);

        return hash;
    }
};

struct ManagedFontHandle {
  public:
    ManagedFontHandle(const HFONT font)
        : font(font) {
    }
    ~ManagedFontHandle() {
        DeleteObject(font);
    }

    ManagedFontHandle(const ManagedFontHandle&) = delete;
    ManagedFontHandle(ManagedFontHandle&&) = delete;
    ManagedFontHandle& operator=(const ManagedFontHandle&) = delete;
    ManagedFontHandle& operator=(ManagedFontHandle&&) = delete;

    HFONT operator*() const {
        return font;
    }

    bool operator==(const ManagedFontHandle& other) const {
        return font == other.font;
    }
    bool operator==(const HFONT other) const {
        return font == other;
    }

    bool operator!=(const ManagedFontHandle& other) const {
        return font != other.font;
    }
    bool operator!=(const HFONT other) const {
        return font != other;
    }

  private:
    HFONT font;
};

bool operator==(
    const HFONT a,
    const ManagedFontHandle& b
) {
    return b == a;
}
bool operator!=(
    const HFONT a,
    const ManagedFontHandle& b
) {
    return b != a;
}

using FontHandle = std::variant<std::shared_ptr<ManagedFontHandle>, HFONT>;

HFONT get(const FontHandle& handle) {
    if (std::holds_alternative<HFONT>(handle)) {
        return std::get<HFONT>(handle);
    }
    return **std::get<std::shared_ptr<ManagedFontHandle>>(handle);
}

static const Mutex<std::unordered_map<FontCacheKey, std::shared_ptr<ManagedFontHandle>>>
    g_fontCache;

static void ClearFontCache() {
    auto fontCache = g_fontCache.lock_mut();

    for (auto& [_, font] : *fontCache) {
        font.reset();
    }

    fontCache->clear();
}
// WHAM@5 transform 20: guard-close
#endif // WHAM_ONCE_11FE0A21086DAA0DF4227EC8
// WHAM@5 end-transform 20
// WHAM@5 end 13
// WHAM@5 begin 21: src/includes/gdi/font_utils.h
// WHAM@5 transform 23: guard-open
#ifndef WHAM_ONCE_29903FDD98B8FA8B569F43D9
#define WHAM_ONCE_29903FDD98B8FA8B569F43D9
// WHAM@5 end-transform 23
// WHAM@5 transform 22: pragma-once
// WHAM@5 end-transform 22

#include <algorithm>
#include <string_view>
#include <vector>

struct SelectedObjectGuard {
    HDC hdc;
    HGDIOBJ previous;
    bool valid;

    SelectedObjectGuard(
        HDC dc,
        HGDIOBJ obj
    )
        : hdc(dc)
        , previous(SelectObject(
              dc,
              obj
          ))
        , valid(previous && previous != HGDI_ERROR) {
    }

    ~SelectedObjectGuard() {
        if (valid) {
            SelectObject(hdc, previous);
        }
    }

    SelectedObjectGuard(const SelectedObjectGuard&) = delete;
    SelectedObjectGuard& operator=(const SelectedObjectGuard&) = delete;

    SelectedObjectGuard(SelectedObjectGuard&&) = delete;
    SelectedObjectGuard&& operator=(SelectedObjectGuard&&) = delete;

    operator bool() const {
        return valid;
    }
};

/// Check whether the selected HFONT contains a glyph.
///
/// GetGlyphIndicesW returns 0xFFFF for an unavailable glyph.
///
/// Important:
///
/// This checks the actual selected GDI font, not merely the font family name.
/// Consequently it also handles fonts whose Unicode coverage is different
/// between faces.
static bool FontHasGlyph(
    HDC hdc,
    HFONT font,
    const std::wstring_view text
) {
    if (text.empty()) {
        return false;
    }

    const SelectedObjectGuard fontGuard(hdc, font);
    if (!fontGuard) {
        return false;
    }

    std::vector<WORD> glyphs(text.size());
    const auto result = GetGlyphIndicesW(
        hdc,
        text.cbegin(),
        static_cast<int>(text.size()),
        glyphs.data(),
        GGI_MARK_NONEXISTING_GLYPHS
    );
    if (result == GDI_ERROR) {
        return false;
    }

    constexpr auto invalid_glyph_id = 0xFFFFU;
    return std::ranges::all_of(glyphs, [&](auto glyph) {
        return glyph != invalid_glyph_id;
    });
}
// WHAM@5 transform 24: guard-close
#endif // WHAM_ONCE_29903FDD98B8FA8B569F43D9
// WHAM@5 end-transform 24
// WHAM@5 end 21
// WHAM@5 reference 25: src/includes/utils.h -> 15

struct FallbackRule {
    char32_t startChar;
    char32_t endChar;
    std::wstring fontFamily;

  public:
    /// Create a fallback HFONT based on the current selected font.
    ///
    /// We intentionally preserve:
    ///
    ///   height
    ///   width
    ///   escapement
    ///   orientation
    ///   weight
    ///   italic
    ///   underline
    ///   strikeout
    ///   charset
    ///   output precision
    ///   clip precision
    ///   quality
    ///   pitch
    ///
    /// Only lfFaceName changes.
    ///
    /// This is important because blindly calling CreateFontW with only a family
    /// name would lose DPI-scaled size and visual characteristics of the
    /// control.
    FontHandle GetReplacementFont(
        HDC hdc,
        HFONT originalFont,
        const std::wstring_view text
    ) const {
        LOGFONTW logFont{};
        if (!originalFont || FontHasGlyph(hdc, originalFont, text) || fontFamily.empty()
            || GetObjectW(originalFont, sizeof(logFont), &logFont) != sizeof(logFont)) {
            return originalFont;
        }

        FontCacheKey key{.originalFont = logFont, .family = fontFamily};

        {
            auto fontCache = g_fontCache.lock();

            auto it = fontCache->find(key);
            if (it != fontCache->end()) {
                return it->second;
            }
        }

        wcsncpy_s(
            static_cast<wchar_t*>(logFont.lfFaceName), LF_FACESIZE, fontFamily.c_str(), _TRUNCATE
        );

        const auto replacement = CreateFontIndirectW(&logFont);
        if (!replacement) {
            return originalFont;
        }

        if (!FontHasGlyph(hdc, replacement, text)) {
            DeleteObject(replacement);
            return originalFont;
        }

        {
            const auto [it, inserted] = g_fontCache.lock_mut()->emplace(
                key, std::make_shared<ManagedFontHandle>(replacement)
            );

            // check if another thread already inserted it
            if (!inserted) {
                return it->second;
            }
        }

        return replacement;
    }
};

static const Mutex<std::shared_ptr<std::vector<FallbackRule>>> g_fallbackRules;

static void LoadRules() {
    auto fallbackRules = std::make_shared<std::vector<FallbackRule>>();

    using WindhawkUtils::StringSetting;

    for (int i = 0;; ++i) {
        const auto startChar = StringSetting::make(L"rules[%d].startChar", i);

        if (wcslen(startChar) == 0) {
            break;
        }

        const auto endChar = StringSetting::make(L"rules[%d].endChar", i);
        const auto fontFamily = StringSetting::make(L"rules[%d].fontFamily", i);

        const auto rule = fallbackRules->emplace_back(
            ParseHex(startChar), ParseHex(endChar), std::wstring(fontFamily)
        );

        if (rule.startChar > rule.endChar) {
            Wh_Log(L"Ignoring invalid rule %d: U+%04X > U+%04X", i, rule.startChar, rule.endChar);
            fallbackRules->pop_back();
            continue;
        }

        if (rule.fontFamily.empty()) {
            Wh_Log(L"Ignoring rule %d with empty font family", i);
            fallbackRules->pop_back();
            continue;
        }

        Wh_Log(
            L"Loaded rule %d: U+%04X-U+%04X -> %s",
            i,
            rule.startChar,
            rule.endChar,
            rule.fontFamily.c_str()
        );
    }

    Wh_Log(L"Loaded %zu fallback rules", fallbackRules->size());

    *g_fallbackRules.lock_mut() = fallbackRules;
}

/// Find the user rule that applies to a Unicode scalar.
///
/// Rules are evaluated in declaration order. The first matching rule wins,
/// matching the spirit of the supplied DirectWrite mod.
static std::shared_ptr<FallbackRule> FindRule(char32_t codePoint) {
    if (codePoint < hiSurrogateBase) {
        Wh_Log(
            L"codePoint: %lc (U+%X)",
            static_cast<wchar_t>(codePoint),
            static_cast<uint32_t>(codePoint)
        );
    } else {
        Wh_Log(L"codePoint: %x", static_cast<uint32_t>(codePoint));
    }

    auto fallbackRules = g_fallbackRules.lock();

    for (auto& rule : **fallbackRules) {
        if (rule.startChar <= codePoint && codePoint <= rule.endChar) {
            return {*fallbackRules, &rule};
        }
    }

    return nullptr;
}
// WHAM@5 transform 27: guard-close
#endif // WHAM_ONCE_DCF41FB6E61DA6C9C499C9C8
// WHAM@5 end-transform 27
// WHAM@5 end 11
// WHAM@5 reference 28: src/includes/utils.h -> 15

using winrt::com_ptr;

static const Mutex<com_ptr<IDWriteFontFallback1>> g_dwriteFallback;

using GetSystemFontFallback_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteFactory8* self,
    IDWriteFontFallback1** fallback
);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static GetSystemFontFallback_t g_originalGetSystemFontFallback = nullptr;

STDMETHODIMP GetSystemFontFallback_Hook(
    IDWriteFactory8* self,
    IDWriteFontFallback1** fallback
) {
    Wh_Log(L"GetSystemFontFallback");
    auto dwriteFallback = g_dwriteFallback.lock_mut();

    if (*dwriteFallback == nullptr) {
        com_ptr<IDWriteFontFallback1> fallback0;
        auto hr = g_originalGetSystemFontFallback(self, fallback0.put());

        com_ptr<IDWriteFontFallbackBuilder> builder;
        if (FAILED(self->CreateFontFallbackBuilder(builder.put()))) {
            *fallback = fallback0.detach();
            return hr;
        };

        auto fallbackRules = g_fallbackRules.lock();
        for (const auto& rule : **fallbackRules) {
            const DWRITE_UNICODE_RANGE range{rule.startChar, rule.endChar};
            auto fontFamily = rule.fontFamily.c_str();
            if (FAILED(builder->AddMapping(&range, 1, &fontFamily, 1))) {
                *fallback = fallback0.detach();
                return hr;
            };

            Wh_Log(
                L"FontFallback: U+%04X - U+%04X -> %s", rule.startChar, rule.endChar, fontFamily
            );
        }

        if (FAILED(builder->AddMappings(fallback0.get()))) {
            *fallback = fallback0.detach();
            return hr;
        };

        com_ptr<IDWriteFontFallback> fallback1;
        if (FAILED(builder->CreateFontFallback(fallback1.put()))) {
            *fallback = fallback0.detach();
            return hr;
        };
        fallback1.as(*dwriteFallback);

        Wh_Log(L"Injected overrides");
    }

    dwriteFallback->copy_to(fallback);
    return S_OK;
}
// WHAM@5 transform 30: guard-close
#endif // WHAM_ONCE_D23B9011C36ABC92BA38DD66
// WHAM@5 end-transform 30
// WHAM@5 end 9
// WHAM@5 reference 31: src/includes/fallback_rule.h -> 11
// WHAM@5 reference 32: src/includes/gdi/font_cache.h -> 13
// WHAM@5 begin 33: src/includes/gdi/hooks/DrawTextW.h
// WHAM@5 transform 48: guard-open
#ifndef WHAM_ONCE_9BDBA01438861D6E04D46260
#define WHAM_ONCE_9BDBA01438861D6E04D46260
// WHAM@5 end-transform 48
// WHAM@5 transform 34: pragma-once
// WHAM@5 end-transform 34

#include <algorithm>
#include <functional>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <windhawk_api.h>

// WHAM@5 reference 35: src/includes/gdi/font_utils.h -> 21
// WHAM@5 begin 36: src/includes/gdi/text_run.h
// WHAM@5 transform 41: guard-open
#ifndef WHAM_ONCE_FF4C6D7DC454A3E24FE07AE4
#define WHAM_ONCE_FF4C6D7DC454A3E24FE07AE4
// WHAM@5 end-transform 41
// WHAM@5 transform 37: pragma-once
// WHAM@5 end-transform 37

#include <optional>
#include <string_view>
#include <utility>
#include <vector>

// WHAM@5 reference 38: src/includes/fallback_rule.h -> 11
// WHAM@5 reference 39: src/includes/gdi/font_cache.h -> 13
// WHAM@5 reference 40: src/includes/utils.h -> 15

/// Run representation.
///
/// A run is a contiguous UTF-16 range that should be rendered with one font.
struct TextRun {
    std::wstring_view text;
    FontHandle font;
};

/// Build runs.
///
/// Example:
///
///   original original original Bengali Bengali original
///
/// becomes:
///
///   [0,3] original
///   [3,5] Bengali
///   [8,...] original
///
/// We deliberately use character-level tests, rather than testing the whole
/// string, because GDI fallback is effectively a glyph/run problem.
static std::optional<std::pair<
    std::vector<TextRun>,
    bool>>
BuildRuns(
    HDC hdc,
    HFONT originalFont,
    std::wstring_view text
) {
    if (text.empty()) {
        return std::nullopt;
    }

    std::vector<TextRun> runs;
    auto replaced = false;

    while (!text.empty()) {
        const auto [codePoint, charLen] = DecodeCodePoint(text);

        if (charLen <= 0) {
            break;
        }

        const auto rule = FindRule(codePoint);
        const auto ch = text.substr(0, charLen);
        auto selectedFont = rule ? rule->GetReplacementFont(hdc, originalFont, ch) : originalFont;

        replaced |= get(selectedFont) != originalFont;

        if (!runs.empty() && runs.back().font == selectedFont
            && runs.back().text.cend() == text.cbegin()) {
            runs.back().text = {runs.back().text.cbegin(), ch.cend()};
        } else {
            runs.emplace_back(ch, selectedFont);
        }

        text = text.substr(charLen);
    }

    Wh_Log(L"> #runs: %d", runs.size());

    return std::pair{runs, replaced};
}
// WHAM@5 transform 42: guard-close
#endif // WHAM_ONCE_FF4C6D7DC454A3E24FE07AE4
// WHAM@5 end-transform 42
// WHAM@5 end 36
// WHAM@5 begin 43: src/includes/hooks.h
// WHAM@5 transform 45: guard-open
#ifndef WHAM_ONCE_048057A8FDB52DFB4101C9C9
#define WHAM_ONCE_048057A8FDB52DFB4101C9C9
// WHAM@5 end-transform 45
// WHAM@5 transform 44: pragma-once
// WHAM@5 end-transform 44

#include <uxtheme.h>

#include <windhawk_utils.h>

// Original function pointers.

using TextOutW_t = decltype(&TextOutW);
using ExtTextOutW_t = decltype(&ExtTextOutW);
using DrawTextW_t = decltype(&DrawTextW);
using DrawTextExW_t = decltype(&DrawTextExW);
using DrawThemeText_t = decltype(&DrawThemeText);
using DrawThemeTextEx_t = decltype(&DrawThemeTextEx);
using DrawShadowText_t = decltype(&DrawShadowText);

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)

static TextOutW_t g_originalTextOutW = nullptr;
static ExtTextOutW_t g_originalExtTextOutW = nullptr;
static DrawTextW_t g_originalDrawTextW = nullptr;
static DrawTextExW_t g_originalDrawTextExW = nullptr;
static DrawThemeText_t g_originalDrawThemeText = nullptr;
static DrawThemeTextEx_t g_originalDrawThemeTextEx = nullptr;
static DrawShadowText_t g_originalDrawShadowText = nullptr;

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/// Thread-local recursion guard.
///
/// Our implementation itself calls GDI APIs such as SelectObject,
/// GetGlyphIndicesW and GetTextExtentPoint32W. Those calls must not recursively
/// enter our hooks.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static thread_local bool g_inHook = false;

struct HookGuard {
    bool previous;

    HookGuard()
        : previous(g_inHook) {
        g_inHook = true;
    }
    ~HookGuard() {
        g_inHook = previous;
    }

    HookGuard(const HookGuard&) = delete;
    const HookGuard& operator=(const HookGuard&) = delete;

    HookGuard(HookGuard&&) = delete;
    HookGuard&& operator=(HookGuard&&) = delete;
};

// Resolves and hooks exported GDI functions.
//
// Uses GetModuleHandle/GetProcAddress rather than hardcoding addresses.
template <typename Prototype>
static bool HookFunctionFromDLL(
    const wchar_t* moduleName,
    const char* functionName,
    Prototype* hook,
    Prototype** original
) {
    const auto module = GetModuleHandleW(moduleName);

    if (!module) {
        Wh_Log(L"GetModuleHandleW(%s) failed", moduleName);

        return false;
    }

    const auto target =
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));

    if (!target) {
        Wh_Log(L"GetProcAddress(%s) failed", functionName);

        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"SetFunctionHook(%s) failed", functionName);

        return false;
    }

    Wh_Log(L"Hooked %ls!%s", moduleName, functionName);

    return true;
}

template <typename Prototype>
static bool HookFunctionInVtable(
    IUnknown* vtable,
    uint32_t idx,
    Prototype* hook,
    Prototype** original
) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const auto target = reinterpret_cast<Prototype*>((*reinterpret_cast<void***>(vtable))[idx]);

    if (!target) {
        Wh_Log(L"Failed to find entry #%d of the Vtable", idx);

        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"SetFunctionHook failed for entry #%d", idx);

        return false;
    }

    Wh_Log(L"Hooked entry #%d", idx);

    return true;
}
// WHAM@5 transform 46: guard-close
#endif // WHAM_ONCE_048057A8FDB52DFB4101C9C9
// WHAM@5 end-transform 46
// WHAM@5 end 43
// WHAM@5 reference 47: src/includes/utils.h -> 15

using namespace std::literals;

// DrawText support.
//
// The layout below follows the structure of DrawTextExW in text.c:
// paragraphs are split into logical lines, DT_WORDBREAK is applied at word
// boundaries, tabs are expanded to tab stops, prefixes are removed and
// underlined, and the ellipsis modes are handled before rendering.
//
// Unlike the native implementation, every text measurement and every text
// draw is performed through g_originalDrawTextW. The currently selected font
// is changed for each fallback run, so the layout uses the metrics of the
// font that will actually draw each character.

struct ParsedDrawText {
    std::wstring text;
    // For every UTF-16 code unit in text, the source offset consumed to produce
    // it. Both halves of a surrogate pair point to the end of that pair.
    std::vector<const wchar_t*> sourceEnds;
    std::vector<size_t> underlineOffsets;

    void AppendParsedCharacter(std::wstring_view text) {
        const auto [_, charLen] = DecodeCodePoint(text);
        const auto ch = text.substr(0, charLen);
        this->text.append(ch);
        sourceEnds.append_range(std::views::repeat(text.cend(), charLen));
    }

  public:
    ParsedDrawText() = default;

    ParsedDrawText(
        std::wstring_view text,
        const UINT format
    ) {
        this->text.reserve(text.size());
        sourceEnds.reserve(text.size());

        const auto noPrefix = (format & DT_NOPREFIX) != 0;
        const auto hidePrefix = (format & DT_HIDEPREFIX) != 0;

        while (!text.empty()) {
            if (!noPrefix && text[0] == L'\x1F') {
                // Win16 Katakana access-key marker: discard the marker and its
                // letter.
                text = text.substr(std::min(text.size(), 2UZ));
                continue;
            }

            if (!noPrefix && (text[0] == L'&' || text[0] == L'\x1E')) {
                if (text.size() == 1) {
                    // A trailing ampersand is literal text.
                    AppendParsedCharacter(text.substr(0, 1));
                    text = text.substr(1);
                    continue;
                }

                if (text.starts_with(L"&&")) {
                    // && becomes one literal ampersand.
                    this->text.push_back(L'&');
                    text = text.substr(2);
                    sourceEnds.push_back(text.cbegin());
                    continue;
                }

                const auto [_, charLen] = DecodeCodePoint(text.substr(1));
                const auto sourceEnd = charLen + 1;
                if (!hidePrefix) {
                    underlineOffsets.push_back(this->text.size());
                }
                AppendParsedCharacter(text.substr(1, charLen));
                text = text.substr(sourceEnd);
                continue;
            }

            const auto [_, charLen] = DecodeCodePoint(text);
            AppendParsedCharacter(text.substr(0, charLen));
            text = text.substr(charLen);
        }
    }
};

static bool IsLineBreakCharacter(wchar_t ch) {
    return ch == L'\r' || ch == L'\n';
}

static bool IsDrawTextBreakCharacter(wchar_t ch) {
    return ch == L' ' || ch == L'\t';
}

static size_t NextCodePointBoundary(
    const std::wstring_view text,
    const size_t position,
    const size_t end
) {
    if (position >= end) {
        return end;
    }

    const auto [_, charLen] = DecodeCodePoint(text.substr(position, end - position));
    return position + charLen;
}

struct DrawTextLine {
    size_t start = 0;
    size_t end = 0;
    size_t next = 0;
    SIZE size{};
    std::wstring renderedText;
    std::vector<size_t> underlineOffsets;
    bool ellipsified = false;
};

static constexpr auto ellipsis = L"..."sv;

struct Modification {
    size_t start;
    size_t length;
    std::wstring replacement;
};

struct Context {
    HDC hdc;
    std::wstring_view text;
    RECT* rect;
    UINT format;
    DRAWTEXTPARAMS* params;
    bool allowModifyString;
    HFONT originalFont;
    ParsedDrawText parsed;
    std::vector<TextRun> runs;

  public:
    // Measure one string using a specific font. DT_CALCRECT is deliberately sent
    // to the trampoline, not to the hooked DrawTextW entry point.
    [[nodiscard]] std::optional<SIZE> MeasureTextWithFont(const TextRun& run) const {
        if (run.text.empty()) {
            return SIZE{0, 0};
        }

        const SelectedObjectGuard fontGuard(hdc, get(run.font));
        if (!fontGuard) {
            return std::nullopt;
        }

        RECT measure{0, 0, INT_MAX / 4, INT_MAX / 4};
        const auto measureFormat =
            DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX | (format & DT_RTLREADING);
        const auto result = g_originalDrawTextW(
            hdc, run.text.cbegin(), static_cast<int>(run.text.size()), &measure, measureFormat
        );
        if (result <= 0) {
            return std::nullopt;
        }

        SIZE measured{
            std::max(0L, measure.right - measure.left), std::max(0L, measure.bottom - measure.top)
        };
        if (!measured.cy) {
            measured.cy = result;
        }
        return measured;
    }

    /// Measure a run sequence with each run's actual font.
    ///
    /// This is the central measurement primitive used by wrapping, alignment, tabs,
    /// and underlines.
    [[nodiscard]] std::optional<SIZE> MeasureTextWithFallback(const std::wstring_view text) const {
        if (text.empty()) {
            return SIZE{0, 0};
        }

        Wh_Log(L">");
        const auto runs = BuildRuns(hdc, originalFont, text);
        if (!runs) {
            return std::nullopt;
        }

        SIZE measured{0, 0};
        for (const auto& run : runs->first) {
            const auto runSize = MeasureTextWithFont(run);
            if (!runSize) {
                return std::nullopt;
            }

            measured.cx += runSize->cx;
            measured.cy = std::max(measured.cy, runSize->cy);
        }

        return measured;
    }

    // Measure a line with expanded tabs. Tab stops are relative to the line's
    // left edge, as they are in DrawTextExW.
    [[nodiscard]] std::optional<SIZE> MeasureDrawTextRange(
        const std::wstring_view text,
        const size_t start,
        const size_t end,
        const int tabWidth
    ) const {
        if (end <= start) {
            return SIZE{0, 0};
        }

        if (!(format & DT_EXPANDTABS)) {
            return MeasureTextWithFallback(text.substr(start, end - start));
        }

        const auto safeTabWidth = std::max(1, tabWidth);

        SIZE measured{0, 0};
        auto segmentStart = start;
        for (auto i = start; i <= end; ++i) {
            if (i != end && text[i] != L'\t') {
                continue;
            }

            if (i > segmentStart) {
                const auto segmentSize =
                    MeasureTextWithFallback(text.substr(segmentStart, i - segmentStart));
                if (!segmentSize) {
                    return std::nullopt;
                }

                measured.cx += segmentSize->cx;
                measured.cy = std::max(measured.cy, segmentSize->cy);
            }

            if (i < end) {
                measured.cx = ((measured.cx / safeTabWidth) + 1) * safeTabWidth;
                segmentStart = i + 1;
            }
        }

        return measured;
    }

    // Find the longest UTF-16-safe prefix whose fallback-font width fits.
    [[nodiscard]] size_t FindFittingPrefix(
        const std::wstring_view text,
        const size_t start,
        const size_t end,
        const int width,
        const int tabWidth
    ) const {
        std::vector<size_t> boundaries;
        boundaries.push_back(start);
        for (auto i = start; i < end;) {
            i = NextCodePointBoundary(text, i, end);
            boundaries.push_back(i);
        }

        auto low = 0UZ;
        auto high = boundaries.size() - 1;
        while (low < high) {
            const auto middle = low + (high - low + 1) / 2;
            const auto measured = MeasureDrawTextRange(text, start, boundaries[middle], tabWidth);
            if (!measured) {
                high = middle - 1;
                continue;
            }

            if (measured->cx <= width) {
                low = middle;
            } else {
                high = middle - 1;
            }
        }

        return boundaries[low];
    }

    [[nodiscard]] std::wstring MakeEndEllipsis(
        const std::wstring_view text,
        const int width,
        const int tabWidth,
        const bool wordEllipsis
    ) const {
        auto bestEnd = 0UZ;
        std::wstring candidate(ellipsis);
        const auto size = MeasureDrawTextRange(candidate, 0, candidate.size(), tabWidth);
        if (!size) {
            return candidate;
        }

        for (auto end = text.size(); end > 0;) {
            std::wstring test{text.substr(0, end)};
            test += ellipsis;
            const auto size = MeasureDrawTextRange(test, 0, test.size(), tabWidth);
            if (size && size->cx <= width) {
                bestEnd = end;
                candidate = std::move(test);
                break;
            }
            auto newEnd = end - 1;
            if (newEnd > 0 && DecodeCodePoint(text.substr(newEnd - 1)).second == 2) {
                --newEnd;
            }

            end = newEnd;
        }

        if (wordEllipsis && bestEnd > 0 && bestEnd < text.size()) {
            auto wordEnd = bestEnd;
            while (wordEnd > 0 && !IsDrawTextBreakCharacter(text[wordEnd - 1])) {
                --wordEnd;
            }
            if (wordEnd > 0) {
                std::wstring wordCandidate{text.substr(0, wordEnd)};
                wordCandidate += ellipsis;
                const auto size =
                    MeasureDrawTextRange(wordCandidate, 0, wordCandidate.size(), tabWidth);
                if (size && size->cx <= width) {
                    candidate = std::move(wordCandidate);
                }
            }
        }

        // Per text.c, the ellipsis itself is returned even when it cannot fit.
        return candidate;
    }

    [[nodiscard]] std::wstring MakePathEllipsis(
        const std::wstring_view text,
        const int width,
        const int tabWidth
    ) const {
        const auto slash = text.find_last_of(L"\\/");
        if (slash == std::wstring::npos) {
            return MakeEndEllipsis(text, width, tabWidth, false);
        }

        auto prefix = text.substr(0, slash);
        const auto suffix = text.substr(slash);
        std::wstring candidate;

        for (;;) {
            candidate.clear();
            candidate.append(prefix);
            candidate.append(ellipsis);
            candidate.append(suffix);

            const auto size = MeasureDrawTextRange(candidate, 0, candidate.size(), tabWidth);
            if (size && size->cx <= width) {
                return candidate;
            }

            if (prefix.empty()) {
                break;
            }

            auto newLength = prefix.size() - 1;
            if (newLength > 0 && static_cast<uint16_t>(prefix[newLength]) >= loSurrogateBase
                && static_cast<uint16_t>(prefix[newLength]) < postSurrogateBase) {
                --newLength;
            }

            prefix = prefix.substr(0, std::max(0UZ, newLength));
        }

        std::wstring output{ellipsis};
        output += suffix;

        return output;
    }

    bool DrawTextLineWithFallback(
        const std::wstring_view text,
        const int x,
        const int y,
        const RECT* const clipRect,
        const int tabWidth
    ) const {
        const auto drawFormat =
            (format
             & ~(DT_CALCRECT | DT_WORDBREAK | DT_CENTER | DT_RIGHT | DT_VCENTER | DT_BOTTOM
                 | DT_EXPANDTABS | DT_TABSTOP | DT_END_ELLIPSIS | DT_PATH_ELLIPSIS
                 | DT_WORD_ELLIPSIS | DT_MODIFYSTRING | DT_HIDEPREFIX))
            | DT_SINGLELINE | DT_NOPREFIX;

        auto currentX = x;
        auto segmentStart = 0UZ;
        const auto safeTabWidth = std::max(1, tabWidth);

        auto drawSegment = [&](const std::wstring_view text) -> bool {
            if (text.empty()) {
                return true;
            }

            Wh_Log(L">");
            const auto runs = BuildRuns(hdc, originalFont, text);
            if (!runs) {
                return false;
            }

            for (const auto& run : runs->first) {
                const auto runSize = MeasureTextWithFont(run);
                if (!runSize) {
                    return false;
                }

                const SelectedObjectGuard fontGuard(hdc, get(run.font));
                if (!fontGuard) {
                    return false;
                }

                auto drawRect = clipRect ? *clipRect : RECT{currentX, y, INT_MAX / 4, INT_MAX / 4};
                drawRect.left = currentX;
                drawRect.top = y;

                const auto result = g_originalDrawTextW(
                    hdc, run.text.cbegin(), static_cast<int>(run.text.size()), &drawRect, drawFormat
                );
                if (result <= 0) {
                    return false;
                }

                currentX += runSize->cx;
            }

            return true;
        };

        if (!(format & DT_EXPANDTABS)) {
            return drawSegment(text);
        }

        for (auto i = 0UZ; i <= text.size(); ++i) {
            if (i != text.size() && text[i] != L'\t') {
                continue;
            }

            if (!drawSegment(text.substr(segmentStart, i - segmentStart))) {
                return false;
            }

            if (i < text.size()) {
                const auto lineOffset = currentX - x;
                currentX = x + ((lineOffset / safeTabWidth) + 1) * safeTabWidth;
                segmentStart = i + 1;
            }
        }

        return true;
    }

    void DrawDrawTextUnderline(
        const std::wstring_view lineText,
        const size_t offset,
        const int x,
        const int y,
        const RECT* const clipRect,
        const int tabWidth
    ) const {
        if (offset < 0 || offset >= lineText.size()) {
            return;
        }

        const auto before = MeasureDrawTextRange(lineText, 0, offset, tabWidth);
        if (!before) {
            return;
        }
        const auto through = MeasureDrawTextRange(lineText, 0, offset + 1, tabWidth);
        if (!through) {
            return;
        }

        auto startX = x + before->cx;
        auto endX = x + through->cx - 1;
        const auto lineY = y;
        auto underlineStartX = startX;
        auto underlineEndX = endX;

        if (clipRect) {
            if (startX > clipRect->right || endX < clipRect->left || lineY < clipRect->top
                || lineY > clipRect->bottom) {
                return;
            }

            underlineStartX = std::max(underlineStartX, clipRect->left);
            underlineEndX = std::min(underlineEndX, clipRect->right);
        }

        if (underlineEndX < underlineStartX) {
            return;
        }

        const HPEN pen = CreatePen(PS_SOLID, 1, GetTextColor(hdc));
        if (!pen) {
            return;
        }

        {
            const SelectedObjectGuard penGuard(hdc, pen);
            if (penGuard) {
                MoveToEx(hdc, static_cast<int>(underlineStartX), lineY, nullptr);
                LineTo(hdc, static_cast<int>(underlineEndX + 1), lineY);
            }
        }
        DeleteObject(pen);
    }

    [[nodiscard]] std::optional<DrawTextLine> BuildNextDrawTextLine(
        const size_t position,
        const int availableWidth,
        const int tabWidth,
        const bool lastLine
    ) const {
        const std::wstring_view text(parsed.text);

        const auto paragraphEnd =
            (format & DT_SINGLELINE)
                ? text.size()
                : (std::ranges::find_if(text.substr(position), &IsLineBreakCharacter)
                   - text.cbegin());

        DrawTextLine line{
            .start = position, .end = paragraphEnd, .next = paragraphEnd, .ellipsified = false
        };

        Wh_Log(L"line.next: %zu", line.next);

        // DrawText's word wrapping is opt-in. Without DT_WORDBREAK, a paragraph is
        // one line and is only clipped/ellipsified according to the requested
        // flags.
        if ((format & DT_WORDBREAK) && !(format & DT_SINGLELINE) && paragraphEnd > position) {
            const auto fullSize = MeasureDrawTextRange(text, position, paragraphEnd, tabWidth);
            if (!fullSize) {
                return std::nullopt;
            }

            if (fullSize->cx > availableWidth) {
                const auto fitEnd =
                    FindFittingPrefix(text, position, paragraphEnd, availableWidth, tabWidth);

                Wh_Log(
                    L"fullWidth: %ld, availableWidth: %ld, fitEnd: %zu",
                    fullSize->cx,
                    availableWidth,
                    fitEnd
                );

                const auto lastBreakRange = std::ranges::find_last_if(
                    text.substr(position, fitEnd - position), &IsDrawTextBreakCharacter
                );
                const auto lastBreak =
                    lastBreakRange.empty()
                        ? fitEnd
                        : static_cast<size_t>(lastBreakRange.begin() - text.cbegin());

                Wh_Log(L"lastBreak: %zu", lastBreak);

                if (lastBreak >= position) {
                    // A left-aligned line may retain the whitespace that fitted
                    // after the previous word. Center/right alignment breaks before
                    // it.
                    if (!(format & (DT_RIGHT | DT_CENTER))) {
                        line.end = line.next = lastBreak + 1;
                    } else {
                        line.end = lastBreak;
                        line.next = lastBreak + 1;
                    }
                    while (line.next < paragraphEnd && IsDrawTextBreakCharacter(text[line.next])) {
                        ++line.next;
                    }
                } else if (
                    (format & DT_EDITCONTROL) && !(format & (DT_WORD_ELLIPSIS | DT_PATH_ELLIPSIS))
                ) {
                    line.end = fitEnd;
                    if (line.end <= position) {
                        line.end = NextCodePointBoundary(text, position, paragraphEnd);
                    }
                    line.next = line.end;
                } else {
                    // text.c keeps an unbreakable word intact unless DT_EDITCONTROL
                    // permits breaking within words. It can therefore extend past
                    // width.
                    auto wordEnd = fitEnd;
                    while (wordEnd < paragraphEnd && !IsDrawTextBreakCharacter(text[wordEnd])) {
                        // NOLINTNEXTLINE(readability-suspicious-call-argument)
                        wordEnd = NextCodePointBoundary(text, wordEnd, paragraphEnd);
                    }
                    if (wordEnd <= position) {
                        wordEnd = NextCodePointBoundary(text, position, paragraphEnd);
                    }
                    line.end = line.next = wordEnd;
                    while (line.next < paragraphEnd && IsDrawTextBreakCharacter(text[line.next])) {
                        ++line.next;
                    }
                }
                Wh_Log(L"line.next: %zu", line.next);
            }
        }

        // Consume CR/LF as a single line separator, including CRLF and LFCR.
        if (line.next == paragraphEnd && paragraphEnd < text.size()) {
            const auto first = text[paragraphEnd];
            ++line.next;
            if (line.next < text.size() && IsLineBreakCharacter(text[line.next])
                && text[line.next] != first) {
                ++line.next;
            }
        }
        Wh_Log(L"line.next: %zu", line.next);

        // Empty paragraphs still represent a logical line.
        line.end = std::max(line.end, line.start);

        line.renderedText.assign(text, line.start, line.end - line.start);
        const auto lineSize =
            MeasureDrawTextRange(line.renderedText, 0, line.renderedText.size(), tabWidth);
        if (!lineSize) {
            return std::nullopt;
        }
        line.size = *lineSize;

        const auto tooWide = line.size.cx > availableWidth;
        const auto requestPathEllipsis = (format & DT_PATH_ELLIPSIS) && tooWide;
        const auto requestWordEllipsis = (format & DT_WORD_ELLIPSIS) && tooWide;
        const auto remainderIsOnlyLineBreak = line.end == paragraphEnd && line.next >= text.size();
        const auto requestEndEllipsis =
            (format & DT_END_ELLIPSIS)
            && ((lastLine && line.next < text.size()) || (remainderIsOnlyLineBreak && tooWide));

        if (requestPathEllipsis || requestWordEllipsis || requestEndEllipsis) {
            if (requestPathEllipsis) {
                line.renderedText = MakePathEllipsis(line.renderedText, availableWidth, tabWidth);
            }

            if (requestWordEllipsis || requestEndEllipsis) {
                // text.c can apply end/word ellipsis after path ellipsis as well.
                line.renderedText = MakeEndEllipsis(
                    line.renderedText, availableWidth, tabWidth, requestWordEllipsis
                );
            }

            line.ellipsified = true;
            const auto lineSize =
                MeasureDrawTextRange(line.renderedText, 0, line.renderedText.size(), tabWidth);
            if (!lineSize) {
                return std::nullopt;
            }
            line.size = *lineSize;
        }

        return line;
    }

    int execute() {
        TEXTMETRICW tm{};
        if (!GetTextMetricsW(hdc, &tm)) {
            return 0;
        }

        const auto originalRect = *rect;
        POINT origin{rect->left, rect->top};
        auto width = rect->right - rect->left;
        const auto lineHeight =
            (format & DT_EXTERNALLEADING) ? tm.tmHeight + tm.tmExternalLeading : tm.tmHeight;
        const auto safeLineHeight = std::max(lineHeight, 1L);

        auto leftMargin = 0;
        auto rightMargin = 0;
        if (params) {
            leftMargin = params->iLeftMargin;
            rightMargin = params->iRightMargin;
            width -= leftMargin + rightMargin;
            if (!(format & (DT_CENTER | DT_RIGHT))) {
                origin.x += leftMargin;
            }
            params->uiLengthDrawn = 0;
        }

        auto tabWidth = 0;
        if (format & DT_EXPANDTABS) {
            const auto tabStop =
                ((format & DT_TABSTOP) && params && params->iTabLength) ? params->iTabLength : 8;
            tabWidth = std::max(1, static_cast<int>(tm.tmAveCharWidth * tabStop));
        }

        if (format & DT_CALCRECT) {
            format |= DT_NOCLIP;
        }

        auto invertY = false;
        if (GetGraphicsMode(hdc) == GM_COMPATIBLE) {
            SIZE windowExt{};
            SIZE viewportExt{};
            if (GetWindowExtEx(hdc, &windowExt) && GetViewportExtEx(hdc, &viewportExt)
                && ((windowExt.cy > 0) != (viewportExt.cy > 0))) {
                invertY = true;
            }
        }

        auto maxWidth = 0L;
        const wchar_t* processedSource = nullptr;
        auto position = 0UZ;
        std::vector<Modification> modifications;

        for (;;) {
            Wh_Log(L"position: %zu", position);
            const auto offsetY =
                ((format & DT_EDITCONTROL) ? 2 * safeLineHeight - 1 : safeLineHeight);
            const auto lastLine = !(format & DT_NOCLIP)
                                  && (invertY ? origin.y - offsetY < rect->bottom
                                              : origin.y + offsetY > rect->bottom);

            auto line = BuildNextDrawTextLine(position, width, tabWidth, lastLine);
            if (!line) {
                // The fallback runs were already identified, so do not redraw the
                // text through the native function after a partial render.
                return 0;
            }

            Wh_Log(L"renderedText: %s", line->renderedText.c_str());

            for (auto prefixOffset : parsed.underlineOffsets) {
                if (prefixOffset >= line->start && prefixOffset < line->end) {
                    line->underlineOffsets.push_back(prefixOffset - line->start);
                }
            }

            if (line->ellipsified) {
                line->underlineOffsets.clear();
            }

            if (params) {
                const auto* const sourceEnd =
                    line->next > 0 ? parsed.sourceEnds[line->next - 1] : processedSource;
                params->uiLengthDrawn +=
                    static_cast<UINT>(std::max(0Z, sourceEnd - processedSource));
                processedSource = std::max(processedSource, sourceEnd);
            }

            auto lineX = origin.x;
            if (format & DT_CENTER) {
                lineX = (rect->left + leftMargin + rect->right - rightMargin - line->size.cx) / 2;
            } else if (format & DT_RIGHT) {
                lineX = rect->right - line->size.cx - rightMargin;
            }

            auto lineY = origin.y;
            if (format & DT_SINGLELINE) {
                if (format & DT_VCENTER) {
                    lineY = rect->top + (rect->bottom - rect->top) / 2 - line->size.cy / 2;
                } else if (format & DT_BOTTOM) {
                    lineY = rect->bottom - line->size.cy;
                }
            }

            if (!(format & DT_CALCRECT)) {
                RECT clipRect = *rect;
                const auto* clip = (format & DT_NOCLIP) ? nullptr : &clipRect;

                if (!DrawTextLineWithFallback(line->renderedText, lineX, lineY, clip, tabWidth)) {
                    return 0;
                }

                if (!(format & DT_HIDEPREFIX)) {
                    for (auto underlineOffset : line->underlineOffsets) {
                        DrawDrawTextUnderline(
                            line->renderedText,
                            underlineOffset,
                            lineX,
                            lineY + tm.tmAscent + 1,
                            clip,
                            tabWidth
                        );
                    }
                }
            } else {
                maxWidth = std::max(maxWidth, line->size.cx);
            }

            if (allowModifyString && (format & DT_MODIFYSTRING) && line->ellipsified
                && line->end > line->start) {
                const auto sourceStart =
                    line->start > 0 ? parsed.sourceEnds[line->start - 1] : text.cbegin();
                const auto sourceEnd = parsed.sourceEnds[line->end - 1];
                if (sourceEnd >= sourceStart) {
                    modifications.emplace_back(
                        sourceStart - text.cbegin(),
                        sourceEnd - sourceStart,
                        std::move(line->renderedText)
                    );
                }
            }

            if (invertY) {
                origin.y -= safeLineHeight;
            } else {
                origin.y += safeLineHeight;
            }

            position = line->next;
            if (position >= parsed.text.size() || lastLine) {
                break;
            }
        }

        if (format & DT_CALCRECT) {
            rect->right = rect->left + maxWidth;
            rect->bottom = origin.y;
            if (params) {
                rect->right += leftMargin + rightMargin;
            }
        }

        if (allowModifyString && (format & DT_MODIFYSTRING) && !modifications.empty()) {
            std::wstring modified(text);
            std::ranges::sort(modifications, std::ranges::greater{}, &Modification::start);
            for (const auto& [start, length, replacement] : modifications) {
                if (start <= modified.size() && start + length <= modified.size()) {
                    modified.replace(start, length, replacement);
                }
            }

            // DrawTextExW requires room for four extra WCHARs when DT_MODIFYSTRING
            // is specified. Preserve that contract while always terminating the
            // result.
            const auto copyCount = std::min(modified.size() + 1, text.size() + 4);
            if (copyCount) {
                memcpy(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<wchar_t*>(text.data()),
                    modified.c_str(),
                    copyCount * sizeof(wchar_t)
                );
            }
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast,cppcoreguidelines-pro-bounds-pointer-arithmetic)
            (const_cast<wchar_t*>(text.end()))[3] = L'\0';
        }

        const auto result = origin.y - originalRect.top;
        return result == 0 ? 1 : result;
    }

    static std::optional<Context> Make(
        HDC hdc,
        LPWSTR text,
        int count,
        LPRECT rect,
        UINT format,
        LPDRAWTEXTPARAMS params,
        bool allowModifyString
    ) {
        if (!text || !rect || count < -1 || !g_originalDrawTextW) {
            return std::nullopt;
        }

        Context ctx{
            .hdc = hdc,
            .text = std::wstring_view(text, count < 0 ? wcslen(text) : count),
            .rect = rect,
            .format = format,
            .params = params,
            .allowModifyString = allowModifyString,
            .originalFont = static_cast<HFONT>(GetCurrentObject(hdc, OBJ_FONT))
        };

        Wh_Log(
            L"text: \"%.s\", count: %d, size: %d", ctx.text.size(), text, count, ctx.text.size()
        );

        if (ctx.text.size() > INT_MAX / 4) {
            return std::nullopt;
        }

        if (params && params->cbSize != sizeof(DRAWTEXTPARAMS)) {
            return std::nullopt;
        }

        if (!ctx.originalFont) {
            return std::nullopt;
        }

        if (ctx.format & DT_SINGLELINE) {
            ctx.format &= ~DT_WORDBREAK;
        }

        ctx.parsed = ParsedDrawText(ctx.text, format);
        if (ctx.parsed.text.empty() || ctx.parsed.text.size() > static_cast<size_t>(INT_MAX)) {
            return std::nullopt;
        }

        auto runs = BuildRuns(hdc, ctx.originalFont, ctx.parsed.text);
        if (!runs || !runs->second) {
            return std::nullopt;
        }

        ctx.runs = std::move(runs->first);

        return ctx;
    }
};

static int WINAPI DrawTextW_Hook(
    HDC hdc,
    LPCWSTR text,
    int count,
    LPRECT rect,
    UINT format
) {
    if (g_inHook || !text || !rect) {
        return g_originalDrawTextW(hdc, text, count, rect, format);
    }

    const HookGuard _;

    const auto result = Context::Make(
                            hdc,
                            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                            const_cast<LPWSTR>(text),
                            count,
                            rect,
                            format,
                            nullptr,
                            false
    )
                            .transform(&Context::execute);

    return result ? *result : g_originalDrawTextW(hdc, text, count, rect, format);
}

static int WINAPI DrawTextExW_Hook(
    HDC hdc,
    LPWSTR text,
    int count,
    LPRECT rect,
    UINT format,
    LPDRAWTEXTPARAMS params
) {
    if (g_inHook || !text || !rect) {
        return g_originalDrawTextExW(hdc, text, count, rect, format, params);
    }

    const HookGuard _;

    const auto result =
        Context::Make(hdc, text, count, rect, format, params, true).transform(&Context::execute);
    return result ? *result : g_originalDrawTextExW(hdc, text, count, rect, format, params);
}
// WHAM@5 transform 49: guard-close
#endif // WHAM_ONCE_9BDBA01438861D6E04D46260
// WHAM@5 end-transform 49
// WHAM@5 end 33
// WHAM@5 begin 50: src/includes/gdi/hooks/DrawThemeText.h
// WHAM@5 transform 55: guard-open
#ifndef WHAM_ONCE_897489964A98944CB7267F46
#define WHAM_ONCE_897489964A98944CB7267F46
// WHAM@5 end-transform 55
// WHAM@5 transform 51: pragma-once
// WHAM@5 end-transform 51

#include <algorithm>
#include <concepts>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

// WHAM@5 reference 52: src/includes/gdi/font_utils.h -> 21
// WHAM@5 reference 53: src/includes/gdi/text_run.h -> 36
// WHAM@5 reference 54: src/includes/hooks.h -> 43

template <typename DrawRun>
using DrawRunResult = std::invoke_result_t<const DrawRun&, const TextRun&, RECT&, DWORD>;

// The themed text APIs accept a rectangle rather than an explicit origin.
// Render each fallback run into its own horizontally adjacent rectangle while
// retaining the caller's vertical alignment, clipping and themed options.
template <typename DrawRun>
    requires std::invocable<
        const DrawRun&,
        const TextRun&,
        RECT&,
        DWORD>
static std::optional<DrawRunResult<DrawRun>> RenderThemedTextRuns(
    HDC hdc,
    LPCWSTR text,
    int count,
    const RECT* rect,
    DWORD textFlags,
    DrawRun&& drawRun
) {
    if (!text || !rect || count <= 0) {
        return std::nullopt;
    }

    const std::wstring_view textView(text, count < 0 ? wcslen(text) : count);

    Wh_Log(L"text: \"%.s\", count: %d, size: %d", textView.size(), text, count, textView.size());

    const auto originalFont = static_cast<HFONT>(GetCurrentObject(hdc, OBJ_FONT));
    if (!originalFont) {
        return std::nullopt;
    }

    Wh_Log(L">");
    const auto runs = BuildRuns(hdc, originalFont, textView);
    if (!runs || !runs->second) {
        return std::nullopt;
    }

    std::vector<LONG> measuredWidths;
    measuredWidths.reserve(runs->first.size());

    int totalWidth = 0;
    for (const auto& run : runs->first) {
        const SelectedObjectGuard fontGuard(hdc, get(run.font));
        if (!fontGuard) {
            return std::nullopt;
        }

        SIZE extent{};
        if (!GetTextExtentPoint32W(
                hdc, run.text.data(), static_cast<int>(run.text.size()), &extent
            )) {
            return std::nullopt;
        }

        const auto width = std::max(0L, extent.cx);
        measuredWidths.push_back(width);
        totalWidth += width;
    }

    int currentX = rect->left;
    if (textFlags & DT_CENTER) {
        currentX = rect->left + ((rect->right - rect->left) - totalWidth) / 2;
    } else if (textFlags & DT_RIGHT) {
        currentX = rect->right - totalWidth;
    }

    const auto runFlags = textFlags & ~(DT_CENTER | DT_RIGHT);
    std::optional<DrawRunResult<DrawRun>> lastResult;
    for (const auto& [run, measuredWidth] : std::views::zip(runs->first, measuredWidths)) {
        RECT runRect = *rect;
        runRect.left = currentX;
        runRect.right = currentX + measuredWidth;

        const auto result = std::forward<DrawRun>(drawRun)(run, runRect, runFlags);
        if constexpr (std::is_same_v<DrawRunResult<DrawRun>, HRESULT>) {
            if (FAILED(result)) {
                return result;
            }
        } else {
            if (result <= 0) {
                return result;
            }
        }
        lastResult = result;
        currentX += measuredWidth;
    }

    return lastResult;
}

static HRESULT WINAPI DrawThemeText_Hook(
    HTHEME theme,
    HDC hdc,
    int partId,
    int stateId,
    LPCWSTR text,
    int count,
    DWORD textFlags,
    DWORD textFlags2,
    LPCRECT rect
) {
    if (g_inHook || !text || !rect || count <= 0 || (textFlags & DT_CALCRECT)) {
        return g_originalDrawThemeText(
            theme, hdc, partId, stateId, text, count, textFlags, textFlags2, rect
        );
    }

    const HookGuard _;
    const auto result = RenderThemedTextRuns(
        hdc, text, count, rect, textFlags, [&](const TextRun& run, RECT& runRect, DWORD runFlags) {
            const SelectedObjectGuard fontGuard(hdc, get(run.font));
            if (!fontGuard) {
                return E_FAIL;
            }
            return g_originalDrawThemeText(
                theme,
                hdc,
                partId,
                stateId,
                run.text.data(),
                static_cast<int>(run.text.size()),
                runFlags,
                textFlags2,
                &runRect
            );
        }
    );

    if (result) {
        return *result;
    }
    return g_originalDrawThemeText(
        theme, hdc, partId, stateId, text, count, textFlags, textFlags2, rect
    );
}

static HRESULT WINAPI DrawThemeTextEx_Hook(
    HTHEME theme,
    HDC hdc,
    int partId,
    int stateId,
    LPCWSTR text,
    int count,
    DWORD textFlags,
    LPRECT rect,
    const DTTOPTS* options
) {
    if (g_inHook || !text || !rect || count <= 0 || (textFlags & DT_CALCRECT)
        || (options && (options->dwFlags & DTT_CALCRECT))) {
        return g_originalDrawThemeTextEx(
            theme, hdc, partId, stateId, text, count, textFlags, rect, options
        );
    }

    const HookGuard _;

    const auto result = RenderThemedTextRuns(
        hdc, text, count, rect, textFlags, [&](const TextRun& run, RECT& runRect, DWORD runFlags) {
            const SelectedObjectGuard fontGuard(hdc, get(run.font));
            if (!fontGuard) {
                return E_FAIL;
            }
            return g_originalDrawThemeTextEx(
                theme,
                hdc,
                partId,
                stateId,
                run.text.data(),
                static_cast<int>(run.text.size()),
                runFlags,
                &runRect,
                options
            );
        }
    );

    if (result) {
        return *result;
    }
    return g_originalDrawThemeTextEx(
        theme, hdc, partId, stateId, text, count, textFlags, rect, options
    );
}

static int WINAPI DrawShadowText_Hook(
    HDC hdc,
    LPCWSTR text,
    UINT count,
    RECT* rect,
    DWORD flags,
    COLORREF textColor,
    COLORREF shadowColor,
    int shadowOffsetX,
    int shadowOffsetY
) {
    if (g_inHook || !text || !rect || count == 0 || count > INT_MAX) {
        return g_originalDrawShadowText(
            hdc, text, count, rect, flags, textColor, shadowColor, shadowOffsetX, shadowOffsetY
        );
    }

    const HookGuard _;
    const auto result = RenderThemedTextRuns(
        hdc,
        text,
        static_cast<int>(count),
        rect,
        flags,
        [&](const TextRun& run, RECT& runRect, DWORD runFlags) {
            const SelectedObjectGuard fontGuard(hdc, get(run.font));
            if (!fontGuard) {
                return 0;
            }
            return g_originalDrawShadowText(
                hdc,
                run.text.data(),
                static_cast<UINT>(run.text.size()),
                &runRect,
                runFlags,
                textColor,
                shadowColor,
                shadowOffsetX,
                shadowOffsetY
            );
        }
    );

    if (result) {
        return *result;
    }
    return g_originalDrawShadowText(
        hdc, text, count, rect, flags, textColor, shadowColor, shadowOffsetX, shadowOffsetY
    );
}
// WHAM@5 transform 56: guard-close
#endif // WHAM_ONCE_897489964A98944CB7267F46
// WHAM@5 end-transform 56
// WHAM@5 end 50
// WHAM@5 begin 57: src/includes/gdi/hooks/ExtTextOutW.h
// WHAM@5 transform 61: guard-open
#ifndef WHAM_ONCE_04CE2DE842A117CBD931D577
#define WHAM_ONCE_04CE2DE842A117CBD931D577
// WHAM@5 end-transform 61
// WHAM@5 transform 58: pragma-once
// WHAM@5 end-transform 58

#include <algorithm>
#include <ranges>
#include <span>
#include <string_view>

// WHAM@5 reference 59: src/includes/gdi/text_run.h -> 36
// WHAM@5 reference 60: src/includes/hooks.h -> 43

/// ExtTextOutW renderer.
///
/// If lpDx is supplied, we preserve the caller's exact character advances.
/// This is especially important for controls because they frequently supply
/// explicit positioning.
///
/// For the no-lpDx case we calculate run advances from the fallback font.
///
/// ETO_GLYPH_INDEX is deliberately bypassed because lpString then contains
/// glyph IDs rather than Unicode characters.
static BOOL RenderExtTextOutRuns(
    HDC hdc,
    int x,
    int y,
    UINT options,
    const RECT* rect,
    LPCWSTR text,
    UINT count,
    const INT* dx
) {
    Wh_Log(L">");

    // TODO: measure and draw for non-left alignment
    const auto textAlign = GetTextAlign(hdc);
    if ((options & ETO_GLYPH_INDEX) || (textAlign & TA_RIGHT) || (textAlign & TA_CENTER)) {
        return g_originalExtTextOutW(hdc, x, y, options, rect, text, count, dx);
    }

    Wh_Log(L">");

    const auto originalFont = static_cast<HFONT>(GetCurrentObject(hdc, OBJ_FONT));
    if (!originalFont) {
        return g_originalExtTextOutW(hdc, x, y, options, rect, text, count, dx);
    }

    Wh_Log(L">");

    const std::wstring_view textView(text, count);

    Wh_Log(L"text: \"%.s\", count: %d", count, text, count);

    const auto runs = BuildRuns(hdc, originalFont, textView);
    if (!runs || !runs->second) {
        return g_originalExtTextOutW(hdc, x, y, options, rect, text, count, dx);
    }

    if (options & ETO_OPAQUE) {
        g_originalExtTextOutW(hdc, x, y, ETO_OPAQUE, rect, nullptr, 0, nullptr);
    }

    options &= ~ETO_OPAQUE;

    const size_t dxFactor = (options & ETO_PDY) ? 2 : 1;
    const auto dxSpan = dx ? std::span{dx, textView.size() * dxFactor} : std::span<INT>{};
    auto currentX = x;
    for (const auto& run : runs->first) {
        const SelectedObjectGuard fontGuard(hdc, get(run.font));
        if (!fontGuard) {
            continue;
        }

        const auto runDx = dx ? dxSpan.subspan(run.text.cbegin() - text, run.text.size() * dxFactor)
                              : std::span<INT>{};
        const auto result = g_originalExtTextOutW(
            hdc, currentX, y, options, rect, run.text.cbegin(), run.text.size(), runDx.data()
        );

        if (dx) {
            auto indices = std::views::iota(0UZ, runDx.size());
            auto runDxOnly = std::views::zip(indices, runDx) | std::views::filter([](auto p) {
                                 return std::get<0>(p) % 2 == 0;
                             })
                             | std::views::elements<1>;
            currentX += (options & ETO_PDY) ? std::ranges::fold_left(runDxOnly, 0, std::plus{})
                                            : std::ranges::fold_left(runDx, 0, std::plus{});
        } else {
            SIZE extent{};
            GetTextExtentPoint32W(
                hdc, run.text.cbegin(), static_cast<int>(run.text.size()), &extent
            );
            currentX += extent.cx;
        }

        if (!result) {
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL WINAPI ExtTextOutW_Hook(
    HDC hdc,
    int x,
    int y,
    UINT options,
    const RECT* rect,
    LPCWSTR text,
    UINT count,
    const INT* dx
) {
    if (g_inHook || !text || count == 0) {
        return g_originalExtTextOutW(hdc, x, y, options, rect, text, count, dx);
    }

    const HookGuard _;

    return RenderExtTextOutRuns(hdc, x, y, options, rect, text, count, dx);
}
// WHAM@5 transform 62: guard-close
#endif // WHAM_ONCE_04CE2DE842A117CBD931D577
// WHAM@5 end-transform 62
// WHAM@5 end 57
// WHAM@5 begin 63: src/includes/gdi/hooks/LoadLibraryW.h
#include <algorithm>
#include <array>
#include <string_view>

#include <winnt.h>

#include <windhawk_api.h>

using LoadLibraryExW_t = decltype(&LoadLibraryExW);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
LoadLibraryExW_t g_originalLoadLibraryExW;

HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags
) {
    constexpr std::array hookedDLLs{L"gdi32.dll", L"user32.dll", L"comctl32.dll", L"uxtheme.dll"};

    HMODULE module = g_originalLoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (module
        && std::ranges::find(hookedDLLs, std::wstring_view{lpLibFileName}) != hookedDLLs.cend()) {
        Wh_ApplyHookOperations();
    }

    return module;
}
// WHAM@5 end 63
// WHAM@5 begin 64: src/includes/gdi/hooks/TextOutW.h
// WHAM@5 transform 68: guard-open
#ifndef WHAM_ONCE_2A8DE16B7CCB576C2F8E77E9
#define WHAM_ONCE_2A8DE16B7CCB576C2F8E77E9
// WHAM@5 end-transform 68
// WHAM@5 transform 65: pragma-once
// WHAM@5 end-transform 65

#include <string_view>

// WHAM@5 reference 66: src/includes/gdi/text_run.h -> 36
// WHAM@5 reference 67: src/includes/hooks.h -> 43

/// Render a TextOutW-style string.
///
/// `TextOutW` has no explicit per-character advance array, so when a fallback
/// run has different metrics we calculate the advance of the run using
/// GetTextExtentPoint32W and move the origin for the next run.
///
/// This is intentionally a "visual run" implementation rather than trying to
/// reproduce win32k's private fallback implementation.
static BOOL RenderTextOutRuns(
    HDC hdc,
    int x,
    int y,
    LPCWSTR text,
    int count
) {
    Wh_Log(L">");

    const auto originalFont = static_cast<HFONT>(GetCurrentObject(hdc, OBJ_FONT));
    if (!originalFont) {
        return g_originalTextOutW(hdc, x, y, text, count);
    }

    const std::wstring_view textView(text, count < 0 ? wcslen(text) : count);

    Wh_Log(L"text: \"%.s\", count: %d, size: %d", textView.size(), text, count, textView.size());

    Wh_Log(L">");
    const auto runs = BuildRuns(hdc, originalFont, textView);
    if (!runs || !runs->second) {
        return g_originalTextOutW(hdc, x, y, text, count);
    }

    int currentX = x;

    for (const auto& run : runs->first) {
        const SelectedObjectGuard fontGuard(hdc, get(run.font));
        if (!fontGuard) {
            continue;
        }

        const auto result = g_originalTextOutW(
            hdc, currentX, y, run.text.cbegin(), static_cast<int>(run.text.size())
        );
        if (!result) {
            return FALSE;
        }

        SIZE extent{};
        GetTextExtentPoint32W(hdc, run.text.cbegin(), static_cast<int>(run.text.size()), &extent);

        currentX += extent.cx;
    }

    return TRUE;
}

static BOOL WINAPI TextOutW_Hook(
    HDC hdc,
    int x,
    int y,
    LPCWSTR text,
    int count
) {
    if (g_inHook || !text || count <= 0) {
        return g_originalTextOutW(hdc, x, y, text, count);
    }

    const HookGuard _;

    return RenderTextOutRuns(hdc, x, y, text, count);
}
// WHAM@5 transform 69: guard-close
#endif // WHAM_ONCE_2A8DE16B7CCB576C2F8E77E9
// WHAM@5 end-transform 69
// WHAM@5 end 64
// WHAM@5 reference 70: src/includes/hooks.h -> 43

static bool InstallDWriteHooks() {
    IUnknown* factory = nullptr;
    auto hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &factory);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to create first DirectWrite factory");
        return false;
    }

    // IUnknown        :  3 :  0
    // IDWriteFactory  : 21 :  3
    // IDWriteFactory1 :  2 : 24
    // IDWriteFactory2 :  5 : 26
    // IDWriteFactory3 :  9 : 31
    // IDWriteFactory4 :  3 : 40
    // IDWriteFactory5 :  5 : 43
    // IDWriteFactory6 :  7 : 48

    constexpr auto offsetGetSystemFontFallback = 26UZ; // 1st of IDWriteFactory2
    constexpr auto offsetCreateTextFormat0 = 15UZ;     // 13th of IDWriteFactory
    constexpr auto offsetCreateTextLayout0 = 18UZ;     // 16th of IDWriteFactory
    constexpr auto offsetCreateTextFormat6 = 54UZ;     // 7th of IDWriteFactory6

    const winrt::com_ptr<IDWriteFactory> factory0(factory, winrt::take_ownership_from_abi);
    const auto supportsFactory6 = static_cast<bool>(factory0.try_as<IDWriteFactory6>());

    if (!HookFunctionInVtable(
            factory,
            offsetGetSystemFontFallback,
            GetSystemFontFallback_Hook,
            &g_originalGetSystemFontFallback
        )) {
        Wh_Log(L"Failed to hook GetSystemFontFallback");
        return false;
    }
    Wh_Log(L"Successfully hooked GetSystemFontFallback");

    if (!HookFunctionInVtable(
            factory, offsetCreateTextFormat0, CreateTextFormat0_Hook, &g_originalCreateTextFormat0
        )) {
        Wh_Log(L"Failed to find CreateTextFormat0");
        return false;
    }
    Wh_Log(L"Successfully hooked CreateTextFormat0");

    if (!HookFunctionInVtable(
            factory, offsetCreateTextLayout0, CreateTextLayout0_Hook, &g_originalCreateTextLayout0
        )) {
        Wh_Log(L"Failed to find CreateTextLayout0");
        return false;
    }
    Wh_Log(L"Successfully hooked CreateTextLayout0");

    if (!supportsFactory6) {
        Wh_Log(L"System does not support IDWriteFactory6");
        Wh_Log(L"Did not attempt hooking CreateTextFormat6");
        return true;
    }
    if (!HookFunctionInVtable(
            factory, offsetCreateTextFormat6, CreateTextFormat6_Hook, &g_originalCreateTextFormat6
        )) {
        Wh_Log(L"Failed to find CreateTextFormat6");
        return false;
    }
    Wh_Log(L"Successfully hooked CreateTextFormat6");

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Font Fallback Override - Initializing");

    LoadRules();

    const auto successDWrite = InstallDWriteHooks();
    if (!successDWrite) {
        Wh_Log(L"Failed to hook DirectWrite font fallback machinery");
    }

    const auto successGDI =
        (HookFunctionFromDLL(L"gdi32.dll", "TextOutW", TextOutW_Hook, &g_originalTextOutW)
         && HookFunctionFromDLL(
             L"gdi32.dll", "ExtTextOutW", ExtTextOutW_Hook, &g_originalExtTextOutW
         )
         && HookFunctionFromDLL(L"user32.dll", "DrawTextW", DrawTextW_Hook, &g_originalDrawTextW)
         && HookFunctionFromDLL(
             L"user32.dll", "DrawTextExW", DrawTextExW_Hook, &g_originalDrawTextExW
         )
         && HookFunctionFromDLL(
             L"uxtheme.dll", "DrawThemeText", DrawThemeText_Hook, &g_originalDrawThemeText
         )
         && HookFunctionFromDLL(
             L"uxtheme.dll", "DrawThemeTextEx", DrawThemeTextEx_Hook, &g_originalDrawThemeTextEx
         )
         && HookFunctionFromDLL(
             L"comctl32.dll", "DrawShadowText", DrawShadowText_Hook, &g_originalDrawShadowText
         ))
        || HookFunctionFromDLL(
            L"kernelbase.dll", "LoadLibraryExW", LoadLibraryExW_Hook, &g_originalLoadLibraryExW
        );

    if (!successGDI) {
        Wh_Log(L"Failed to hook one or more GDI routines");
    }

    return successDWrite || successGDI;
}

void Wh_ModUninit() {
    Wh_Log(L"Font Fallback Override - Uninitializing");

    ClearFontCache();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");

    LoadRules();

    *g_dwriteFallback.lock_mut() = nullptr;

    ClearFontCache();
}
// #region WHAM recovery
// clang-format off
// WHAM@5 recovery-begin
// {
//   "type": "manifest",
//   "version": 5,
//   "root": "src/font-fallback-override.cpp",
//   "files": [
//     {
//       "path": "src/font-fallback-override.cpp",
//       "hash": "9f4064f3363b9c023939b4a2e202f9bec46260e8c684d487067672871845d064",
//       "bom": false
//     },
//     {
//       "path": "src/includes/dwrite/hooks/CreateTextFormat.h",
//       "hash": "22e54aa8f6126b7bf8c23f11f5fcbd037c19a842a86ea91e93436361216323cc",
//       "bom": false
//     },
//     {
//       "path": "src/includes/dwrite/hooks/CreateTextLayout.h",
//       "hash": "d84719ab482392fb8c295a0ddc02e5307cbb062009f8d787edf4809035f8f886",
//       "bom": false
//     },
//     {
//       "path": "src/includes/dwrite/hooks/GetSystemFontFallback.h",
//       "hash": "4c4113b3435a5b83ddf7522fdcab1fe7c1f36570acdceb939cd582be9d35a7bd",
//       "bom": false
//     },
//     {
//       "path": "src/includes/fallback_rule.h",
//       "hash": "c038d809fb4ad6c1083ece3de17b6a39f8c0fc25a93d4035d8e43d5d1bad1127",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/font_cache.h",
//       "hash": "0e5d330203c624674673f661e12b14abd170198507bf3cbda27c5bf6487ced21",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/font_utils.h",
//       "hash": "ae47f0dd1206550ac9675307c8578f2a98492fd8104d1a3ea3f0c198a83d9592",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/hooks/DrawTextW.h",
//       "hash": "f4a0a5ada3524ca123e356dfbd5a22d103496849df506cefef0f90198df8ebdf",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/hooks/DrawThemeText.h",
//       "hash": "c3c1f1c2e79bf97d31db1620fc90cb41199daf4adcd815c8bda0bc52c43bf65c",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/hooks/ExtTextOutW.h",
//       "hash": "8dbb470d5f74e7a0807978e93b829843784d1f80d01afafa8f9d54d118fbb11b",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/hooks/LoadLibraryW.h",
//       "hash": "2546a05f6327ae7e22af4c1c183d306e8c900bc937506a31b46137691f5d9f14",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/hooks/TextOutW.h",
//       "hash": "921016abd1035f9ff00d1d3660d3fd26e89368a3a85a4afc8904679c048ca345",
//       "bom": false
//     },
//     {
//       "path": "src/includes/gdi/text_run.h",
//       "hash": "c7371d06ff1059e0e204e26a646517477946a5d65bc66458e898f5804e92c057",
//       "bom": false
//     },
//     {
//       "path": "src/includes/hooks.h",
//       "hash": "a94795504b031e217089a6bf0ee66fb96ffe0755c0fc435943d815b8bd120937",
//       "bom": false
//     },
//     {
//       "path": "src/includes/utils.h",
//       "hash": "be68d00ca0e6c9c098df48d04ddc9ab1b3ba373b070e3220f2c29e5737fd488f",
//       "bom": false
//     }
//   ],
//   "config": {
//     "formatVersion": 1,
//     "sourceDirectory": "src",
//     "outputDirectory": "output",
//     "entries": [
//       "font-fallback-override.cpp"
//     ],
//     "exclude": [],
//     "includeDirectories": [
//       "src"
//     ],
//     "externalIncludes": [],
//     "inlineAngleIncludes": false,
//     "generate": "onSave",
//     "debounceMs": 250
//   },
//   "tail": "",
//   "records": [
//     {
//       "type": "include",
//       "id": 1,
//       "path": "src/includes/dwrite/hooks/CreateTextFormat.h",
//       "directive": "#include \"includes/dwrite/hooks/CreateTextFormat.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 2,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "transform",
//       "id": 3,
//       "kind": "guard-open",
//       "hash": "dabe065232b1a9a9e73850cc34fba366ff890629de82ffe5f66cf259e024af6e"
//     },
//     {
//       "type": "transform",
//       "id": 4,
//       "kind": "guard-close",
//       "hash": "500dabe0b3dcc719527f88abb5468ee4494d5e4f401a1fcc7f158d5ec6903e2f"
//     },
//     {
//       "type": "include",
//       "id": 5,
//       "path": "src/includes/dwrite/hooks/CreateTextLayout.h",
//       "directive": "#include \"includes/dwrite/hooks/CreateTextLayout.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 6,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "transform",
//       "id": 7,
//       "kind": "guard-open",
//       "hash": "0f649b4117c53a2dbbe07254f063b4687369c9e421227b60bca9aceb64ec8c67"
//     },
//     {
//       "type": "transform",
//       "id": 8,
//       "kind": "guard-close",
//       "hash": "0d1a4f7fc416d39b808dfd9d2037c137dd758c41be61ad9464d0f199f3b42a1c"
//     },
//     {
//       "type": "include",
//       "id": 9,
//       "path": "src/includes/dwrite/hooks/GetSystemFontFallback.h",
//       "directive": "#include \"includes/dwrite/hooks/GetSystemFontFallback.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 10,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "include",
//       "id": 11,
//       "path": "src/includes/fallback_rule.h",
//       "directive": "#include \"includes/fallback_rule.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 12,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "include",
//       "id": 13,
//       "path": "src/includes/gdi/font_cache.h",
//       "directive": "#include \"includes/gdi/font_cache.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 14,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "include",
//       "id": 15,
//       "path": "src/includes/utils.h",
//       "directive": "#include \"includes/utils.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 16,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "transform",
//       "id": 17,
//       "kind": "guard-open",
//       "hash": "5352796baa0440ddcdac39e711118edba0e6fdb0e34bcb8f44e74830a80d0917"
//     },
//     {
//       "type": "transform",
//       "id": 18,
//       "kind": "guard-close",
//       "hash": "ded749d0d51c416bf1e00fa20c723ca937a7843f978d1bbd0d8201912040ccf4"
//     },
//     {
//       "type": "transform",
//       "id": 19,
//       "kind": "guard-open",
//       "hash": "69d52eb38a62ea3a33f026e4b75e1f197538710416fd210802ac26f83d4f316a"
//     },
//     {
//       "type": "transform",
//       "id": 20,
//       "kind": "guard-close",
//       "hash": "5baed470e8685c98a619ae1dac332b5cc8da92b4b6b59ce9a1158dc534a3a3de"
//     },
//     {
//       "type": "include",
//       "id": 21,
//       "path": "src/includes/gdi/font_utils.h",
//       "directive": "#include \"includes/gdi/font_utils.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 22,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "transform",
//       "id": 23,
//       "kind": "guard-open",
//       "hash": "01bf3d670f25449380cabedc96f03516734317d92e1951863f40e39cd1d70528"
//     },
//     {
//       "type": "transform",
//       "id": 24,
//       "kind": "guard-close",
//       "hash": "5b1673c8641542f124f0506a18cddc91734447253d9e93870bd6a8fedf608604"
//     },
//     {
//       "type": "reference",
//       "id": 25,
//       "path": "src/includes/utils.h",
//       "directive": "#include \"includes/utils.h\"\r\n",
//       "bodyId": 15
//     },
//     {
//       "type": "transform",
//       "id": 26,
//       "kind": "guard-open",
//       "hash": "7be9b9e47876721d2a60e60e664c8e553dac3c22cb354ffef0b7a21eb722bfca"
//     },
//     {
//       "type": "transform",
//       "id": 27,
//       "kind": "guard-close",
//       "hash": "ae80cd9291fe5fb08f1f5e9fd2a0c4092416a7316913c24f14684cd91d21dc91"
//     },
//     {
//       "type": "reference",
//       "id": 28,
//       "path": "src/includes/utils.h",
//       "directive": "#include \"includes/utils.h\"\r\n",
//       "bodyId": 15
//     },
//     {
//       "type": "transform",
//       "id": 29,
//       "kind": "guard-open",
//       "hash": "b90ecdc3cc83c071c8530002fac893e5f95a9297928266541265b11a24e96893"
//     },
//     {
//       "type": "transform",
//       "id": 30,
//       "kind": "guard-close",
//       "hash": "ac7fdf2c502d4ab1bd756e4beaac6e733fe7801f13b1ff9a6f91fc9f6733a491"
//     },
//     {
//       "type": "reference",
//       "id": 31,
//       "path": "src/includes/fallback_rule.h",
//       "directive": "#include \"includes/fallback_rule.h\"\r\n",
//       "bodyId": 11
//     },
//     {
//       "type": "reference",
//       "id": 32,
//       "path": "src/includes/gdi/font_cache.h",
//       "directive": "#include \"includes/gdi/font_cache.h\"\r\n",
//       "bodyId": 13
//     },
//     {
//       "type": "include",
//       "id": 33,
//       "path": "src/includes/gdi/hooks/DrawTextW.h",
//       "directive": "#include \"includes/gdi/hooks/DrawTextW.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 34,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "reference",
//       "id": 35,
//       "path": "src/includes/gdi/font_utils.h",
//       "directive": "#include \"includes/gdi/font_utils.h\"\r\n",
//       "bodyId": 21
//     },
//     {
//       "type": "include",
//       "id": 36,
//       "path": "src/includes/gdi/text_run.h",
//       "directive": "#include \"includes/gdi/text_run.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 37,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "reference",
//       "id": 38,
//       "path": "src/includes/fallback_rule.h",
//       "directive": "#include \"includes/fallback_rule.h\"\r\n",
//       "bodyId": 11
//     },
//     {
//       "type": "reference",
//       "id": 39,
//       "path": "src/includes/gdi/font_cache.h",
//       "directive": "#include \"includes/gdi/font_cache.h\"\r\n",
//       "bodyId": 13
//     },
//     {
//       "type": "reference",
//       "id": 40,
//       "path": "src/includes/utils.h",
//       "directive": "#include \"includes/utils.h\"\r\n",
//       "bodyId": 15
//     },
//     {
//       "type": "transform",
//       "id": 41,
//       "kind": "guard-open",
//       "hash": "63527483f33ac4f58cc2bfd02c3e39db2336bcb4c337588a669638b5e2cdedcf"
//     },
//     {
//       "type": "transform",
//       "id": 42,
//       "kind": "guard-close",
//       "hash": "ae780058adf22156f3100598439f4dba702c185da13902c54c5c2992fefb14fd"
//     },
//     {
//       "type": "include",
//       "id": 43,
//       "path": "src/includes/hooks.h",
//       "directive": "#include \"includes/hooks.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 44,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "transform",
//       "id": 45,
//       "kind": "guard-open",
//       "hash": "47ce1832f74395b28adeb6eeb493988177de40b3466cbd98272b4fe0ee1e2244"
//     },
//     {
//       "type": "transform",
//       "id": 46,
//       "kind": "guard-close",
//       "hash": "062160069f5aca93b9adf8a276ca5ea9a786972f7bdbff10b0a010668b2cc93e"
//     },
//     {
//       "type": "reference",
//       "id": 47,
//       "path": "src/includes/utils.h",
//       "directive": "#include \"includes/utils.h\"\r\n",
//       "bodyId": 15
//     },
//     {
//       "type": "transform",
//       "id": 48,
//       "kind": "guard-open",
//       "hash": "3170d2bfbf0b366042c92a249d05456413e8c632d07fe198630dfd1cb2df9872"
//     },
//     {
//       "type": "transform",
//       "id": 49,
//       "kind": "guard-close",
//       "hash": "f8ebf78fb17ae505a90a012a8e243daf3233a66f35a0ba86dcf8a222919150f3"
//     },
//     {
//       "type": "include",
//       "id": 50,
//       "path": "src/includes/gdi/hooks/DrawThemeText.h",
//       "directive": "#include \"includes/gdi/hooks/DrawThemeText.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 51,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "reference",
//       "id": 52,
//       "path": "src/includes/gdi/font_utils.h",
//       "directive": "#include \"includes/gdi/font_utils.h\"\r\n",
//       "bodyId": 21
//     },
//     {
//       "type": "reference",
//       "id": 53,
//       "path": "src/includes/gdi/text_run.h",
//       "directive": "#include \"includes/gdi/text_run.h\"\r\n",
//       "bodyId": 36
//     },
//     {
//       "type": "reference",
//       "id": 54,
//       "path": "src/includes/hooks.h",
//       "directive": "#include \"includes/hooks.h\"\r\n",
//       "bodyId": 43
//     },
//     {
//       "type": "transform",
//       "id": 55,
//       "kind": "guard-open",
//       "hash": "b16b568ddc505a4efe6c7a685754fef356d089d5e7bad02c33b2289b612e11ed"
//     },
//     {
//       "type": "transform",
//       "id": 56,
//       "kind": "guard-close",
//       "hash": "2a5b840791be81ee85ee5f2427bfa6621f9167f8ff2d2fc9d2154e7b582de482"
//     },
//     {
//       "type": "include",
//       "id": 57,
//       "path": "src/includes/gdi/hooks/ExtTextOutW.h",
//       "directive": "#include \"includes/gdi/hooks/ExtTextOutW.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 58,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "reference",
//       "id": 59,
//       "path": "src/includes/gdi/text_run.h",
//       "directive": "#include \"includes/gdi/text_run.h\"\r\n",
//       "bodyId": 36
//     },
//     {
//       "type": "reference",
//       "id": 60,
//       "path": "src/includes/hooks.h",
//       "directive": "#include \"includes/hooks.h\"\r\n",
//       "bodyId": 43
//     },
//     {
//       "type": "transform",
//       "id": 61,
//       "kind": "guard-open",
//       "hash": "1e798ba74e1ae7ec0a1e6e1c118987a81c5641d8c7ea7458c0817165578a1524"
//     },
//     {
//       "type": "transform",
//       "id": 62,
//       "kind": "guard-close",
//       "hash": "7740b01cab5e823df053f8bca219576c30bc06287975f5b19b7ddc6a50f8b2d1"
//     },
//     {
//       "type": "include",
//       "id": 63,
//       "path": "src/includes/gdi/hooks/LoadLibraryW.h",
//       "directive": "#include \"includes/gdi/hooks/LoadLibraryW.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 64,
//       "path": "src/includes/gdi/hooks/TextOutW.h",
//       "directive": "#include \"includes/gdi/hooks/TextOutW.h\"\r\n"
//     },
//     {
//       "type": "transform",
//       "id": 65,
//       "kind": "pragma-once",
//       "directive": "#pragma once\r\n",
//       "hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
//     },
//     {
//       "type": "reference",
//       "id": 66,
//       "path": "src/includes/gdi/text_run.h",
//       "directive": "#include \"includes/gdi/text_run.h\"\r\n",
//       "bodyId": 36
//     },
//     {
//       "type": "reference",
//       "id": 67,
//       "path": "src/includes/hooks.h",
//       "directive": "#include \"includes/hooks.h\"\r\n",
//       "bodyId": 43
//     },
//     {
//       "type": "transform",
//       "id": 68,
//       "kind": "guard-open",
//       "hash": "4c409a2861b5fa7bca3b6a235a2144d9905112c2434e11deaebe92f1c7d73739"
//     },
//     {
//       "type": "transform",
//       "id": 69,
//       "kind": "guard-close",
//       "hash": "d6f3664120ffd1b69eaf0f325d88b7480b5cc0085f9900cd2f06a52490e02ab0"
//     },
//     {
//       "type": "reference",
//       "id": 70,
//       "path": "src/includes/hooks.h",
//       "directive": "#include \"includes/hooks.h\"\r\n",
//       "bodyId": 43
//     }
//   ]
// }
// WHAM@5 recovery-end
// clang-format on
// #endregion
