// ==WindhawkMod==
// @id              explorer-font-changer
// @name            Explorer Font Changer
// @description     Comic Sans anybody? Change your GDI fonts at will! Also, gloooowy!
// @version         0.2
// @author          Gabriela Cristei
// @github          https://github.com/cristeigabriela
// @include         explorer.exe
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Font Changer

![Preview](https://raw.githubusercontent.com/cristeigabriela/images/refs/heads/main/windhawk-explorer-font-changer-0.2.png)

Change your Explorer GDI fonts at any time! Also, make text glow if you wish! Configurable at any time!

## Configuration:

- Font Face Name:
To not affect the Windows default-picked font face name, simply fill in the setting textbox with the value `None`.

- Glow text:
This toggles on the "Glow text" functionality, proxying calls to `user32!DrawTextW` and `user32!DrawTextExW` to `uxtheme.dll`'s "DrawGlowText" function. Thanks to the mod "Translucent Windows" for bringing this function to my attention!

- Glow: Red (0-255):
The red value of the glow text color (0-255).

- Glow: Green (0-255):
The green value of the glow text color (0-255).

- Glow: Blue (0-255):
The blue value of the glow text color (0-255).

- Glow: Alpha (0-255):
The alpha value of the glow text color (0-255).

- Glow: Intensity (1-..)
The intensity value of the glow that surrounds the text.

## System stability:

Explorer Font Changer is heavily tested against GDI handle leaks and there are none (known!) at the time of writing this.

This can be checked by others by going to System Informer, looking for `explorer.exe`, right-clicking the entry, going to "Miscellaneous", and then "GDI handles". Then, you can look at the font handles. Normally, you should see no handles present with your chosen font there!
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- font:
  - name: "None"
    $name: "Font Face Name"
  - glow: false
    $name: "Glow text"
  - r: 255
    $name: "Glow: Red (0-255)"
  - g: 255
    $name: "Glow: Green (0-255)"
  - b: 255
    $name: "Glow: Blue (0-255)"
  - a: 255
    $name: "Glow: Alpha (0-255)"
  - intensity: 4
    $name: "Glow: Intensity (1-..)"
  $name: Custom font
  $description: This font will be used for all font creation in Windows.
*/
// ==/WindhawkModSettings==


#include <string>
#include <windhawk_utils.h>
#include <uxtheme.h>

using namespace std::string_view_literals;
using namespace WindhawkUtils;

namespace util {
    StringSetting s_font_name;
    bool s_glow = false;
    int s_glow_r = 255;
    int s_glow_g = 255;
    int s_glow_b = 255;
    int s_glow_a = 255;
    int s_glow_intensity = 0;

    // Class to declare transparent RAII types.
    template <class T, auto Fn>
    class raii {
    public:
        raii() = delete;
        raii(T handle) : m_handle(handle) {}
        ~raii() {
            Fn(m_handle);
        }

        // we don't need operator= or raii(raii&&) yet...

        auto& get() {
            return m_handle;
        }
        
        auto get() const {
            return m_handle;
        }

    private:
        T m_handle;
    };

    // RAII `HFONT`.
    using unique_hfont_t = raii<HFONT, DeleteObject>;

    // Alters search path to look in `System32` folder.
    HMODULE load_os_library(
        LPCWSTR library
    ) {
        return LoadLibraryExW(
            library,
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32
        );
    }

    // Loosely reversed, initially referenced from Translucent Windows font, thanks!
    using draw_text_with_glow_t = HRESULT(WINAPI*)(
        HDC target_hdc,
        LPCWSTR text,
        UINT text_len,
        const RECT* rect,
        DWORD flags,
        COLORREF text_color,
        COLORREF glow_color,
        UINT glow_intensity,
        UINT unk1,
        BOOL unk2,
        // This one is useful in the scenario of painting within a hook,
        // as letting it fall back on, for example, `DrawTextW`, while
        // using this inside said function, would cause recursion.
        DTT_CALLBACK_PROC fallback_draw_text,
        LPARAM lparam
    );

    // Function is not exported by name, but it is found in the resource table.
    auto draw_text_with_glow_fn = reinterpret_cast<draw_text_with_glow_t>(
        GetProcAddress(
            load_os_library(L"uxtheme.dll"),
            MAKEINTRESOURCEA(126)
        )
    );

    // credit: "Translucent Windows" mod
    auto draw_text_with_glow(
        HDC target_hdc,
        const RECT* target_rect,
        const std::wstring_view text,
        DWORD format,
        COLORREF text_color,
        COLORREF glow_color,
        UINT glow_intensity,
        DTT_CALLBACK_PROC callback = nullptr,
        LPARAM lparam = 0
    ) {
        auto hr = S_FALSE;

        auto blend_fun = BLENDFUNCTION { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        auto bp_params = BP_PAINTPARAMS {
            .cbSize = sizeof(BP_PAINTPARAMS),
            .dwFlags = BPPF_ERASE,
            .prcExclude = nullptr,
            .pBlendFunction = &blend_fun
        };

        HDC buf_hdc = nullptr;
        auto buf_ptr = BeginBufferedPaint(
            target_hdc, target_rect, BPBF_TOPDOWNDIB, &bp_params, &buf_hdc);
        if (buf_ptr && buf_hdc) {
            SelectObject(buf_hdc, GetCurrentObject(target_hdc, OBJ_FONT));
            SetBkMode(buf_hdc, GetBkMode(target_hdc));
            SetBkColor(buf_hdc, GetBkColor(target_hdc));
            SetTextAlign(buf_hdc, GetTextAlign(target_hdc));
            SetTextCharacterExtra(buf_hdc, GetTextCharacterExtra(target_hdc));

            hr = draw_text_with_glow_fn(
                buf_hdc, text.data(), text.size(),
                target_rect, format, text_color, glow_color, glow_intensity,
                100, 100, callback, lparam
            );

            EndBufferedPaint(buf_ptr, TRUE);
        }

        return hr;
    }

    void change_font_in_struct(
        LOGFONTW* font
    ) {
        auto font_name = std::wstring_view(s_font_name.get());

        // Check font configuration.
        if (font_name != L"None"sv) {
            if (font_name.size() <= 31) {
                // `face_name` points to a total of 32 WORDs.
                auto face_name = static_cast<WCHAR*>(font->lfFaceName);

                std::memset(face_name, 0, ARRAYSIZE(font->lfFaceName));
                std::memcpy(
                    face_name, font_name.data(),
                    font_name.size() * sizeof(decltype(font_name)::value_type)
                );
            } else {
                Wh_Log(L"Trying to change font to \"%s\": size too long (%d)", font_name.data(), font_name.size());
            }
        }
    }

    std::pair<unique_hfont_t, LOGFONTW> hdc_update_font(
        HDC hdc
    ) {
        // Get current selected font.
        auto h_font = GetCurrentObject(hdc, OBJ_FONT);

        // Create struct for font.
        auto font = LOGFONTW { 0 };

        // Get `LOGFONTW` from font handle.
        GetObjectW(
            static_cast<HANDLE>(h_font),
            sizeof(font),
            static_cast<LPVOID>(&font)
        );

        // Change font to the font specified in settings.
        change_font_in_struct(&font);

        // Create new font.
        auto h_new_font = CreateFontIndirectW(&font);

        // Select font.
        SelectObject(hdc, h_new_font);

        return { h_new_font, font };
    }

    void update_settings() {
        s_font_name      = StringSetting::make(L"font.name");
        s_glow           = Wh_GetIntSetting(L"font.glow") == 1;
        s_glow_r         = Wh_GetIntSetting(L"font.r");
        s_glow_g         = Wh_GetIntSetting(L"font.g");
        s_glow_b         = Wh_GetIntSetting(L"font.b");
        s_glow_a         = Wh_GetIntSetting(L"font.a");
        s_glow_intensity = Wh_GetIntSetting(L"font.intensity");
    }

    bool is_glow_enabled() {
        return s_glow;
    }

    COLORREF get_glow_abgr() {
        // 1. Get the colors
        auto r = static_cast<uint8_t>(
            s_glow_r & 0xff
        );

        auto g = static_cast<uint8_t>(
            s_glow_g & 0xff
        );
        
        auto b = static_cast<uint8_t>(
            s_glow_b & 0xff
        );

        auto a = static_cast<uint8_t>(
            s_glow_a & 0xff
        );

        // 2. Generate ABGR
        auto result = static_cast<COLORREF>(
            (a << 24) | (b << 16) | (g << 8) | r
        );

        return result;
    }

    int get_glow_intensity() {
        return s_glow_intensity;
    }
}

using draw_textw_hook_t = decltype(&DrawTextW);
static draw_textw_hook_t draw_textw_original = nullptr;

using draw_text_exw_hook_t = decltype(&DrawTextExW);
static draw_text_exw_hook_t draw_text_exw_original = nullptr;

// NOTE(gabriela): intentionally used for `DrawTextExW` as well, as the glow DrawText
// equivalent expects this to be a call to `DrawTextW`. 
INT WINAPI dtt_callback(
    HDC hdc,
    LPWSTR lpchText,
    INT cchText,
    LPRECT lprc,
    UINT format,
    LPARAM lParam
) {
    return draw_textw_original(hdc, lpchText, cchText, lprc, format);
}

INT WINAPI draw_textw_hook(
    HDC hdc,
    LPCWSTR lpchText,
    INT cchText,
    LPRECT lprc,
    UINT format
) {
    // Update font on HDC to settings font, from current HFONT.
    auto [h_new_font, _] = util::hdc_update_font(hdc);

    if (format & DT_CALCRECT) {
        return draw_textw_original(hdc, lpchText, cchText, lprc, format);;
    }

    if (util::is_glow_enabled()) {
        auto text = std::wstring_view{ lpchText, wcslen(lpchText) };
        auto hr = util::draw_text_with_glow(
            hdc, lprc, text, format,
            GetTextColor(hdc), util::get_glow_abgr(), util::get_glow_intensity(),
            dtt_callback
        );

        if (SUCCEEDED(hr)) {
            // Call with DT_CALCRECT to get height of text in logical units.
            return draw_textw_original(hdc, lpchText, cchText, lprc, format | DT_CALCRECT);
        }
    }

    return draw_textw_original(hdc, lpchText, cchText, lprc, format);
}

INT WINAPI draw_text_exw_hook(
    HDC hdc,
    LPWSTR lpchText,
    INT cchText,
    LPRECT lprc,
    UINT format,
    LPDRAWTEXTPARAMS lpdtp
) {
    // Update font on HDC to settings font, from current HFONT.
    auto [h_new_font, _] = util::hdc_update_font(hdc);

    if (format & DT_CALCRECT) {
        return draw_text_exw_original(hdc, lpchText, cchText, lprc, format, lpdtp);
    }

    if (util::is_glow_enabled()) {
        auto text = std::wstring_view{ lpchText, wcslen(lpchText) };
        auto hr = util::draw_text_with_glow(
            hdc, lprc, text, format,
            GetTextColor(hdc), util::get_glow_abgr(), util::get_glow_intensity(),
            dtt_callback
        );

        if (SUCCEEDED(hr)) {
            // Call with DT_CALCRECT to get height of text in logical units.
            return draw_text_exw_original(hdc, lpchText, cchText, lprc, format | DT_CALCRECT, lpdtp);
        }
    }

    return draw_text_exw_original(hdc, lpchText, cchText, lprc, format, lpdtp);
}

BOOL Wh_ModInit() {
    // Get settings before applying hooks.
    util::update_settings();

    auto user32 = util::load_os_library(L"user32.dll");
    
    auto draw_textw = reinterpret_cast<draw_textw_hook_t>(
        GetProcAddress(user32, "DrawTextW")
    );
    Wh_SetFunctionHook(
        reinterpret_cast<void*>(draw_textw),
        reinterpret_cast<void*>(draw_textw_hook),
        reinterpret_cast<void**>(&draw_textw_original)
    );

    auto draw_text_exw = reinterpret_cast<draw_text_exw_hook_t>(
        GetProcAddress(user32, "DrawTextExW")
    );
    Wh_SetFunctionHook(
        reinterpret_cast<void*>(draw_text_exw),
        reinterpret_cast<void*>(draw_text_exw_hook),
        reinterpret_cast<void**>(&draw_text_exw_original)
    );

    return TRUE;
}

void Wh_ModSettingsChanged() {
    util::update_settings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
