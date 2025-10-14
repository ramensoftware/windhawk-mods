// ==WindhawkMod==
// @id              explorer-font-changer
// @name            Explorer Font Changer
// @description     Comic Sans anybody? Every new GDI font in the Explorer context will be changed to your font of choice.
// @version         0.1
// @author          Gabriela Cristei
// @github          https://github.com/cristeigabriela
// @include         explorer.exe
// @compilerOptions -std=c++23
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Font Changer

![Preview](https://raw.githubusercontent.com/cristeigabriela/images/refs/heads/main/windhawk-explorer-font-changer.png)

Every single newly created Explorer GDI font will be replaced to your font, through hooking
the GDI32 "CreateFontIndirectW" function.

This will only apply to newly created GDI handles, as this mod does not support existing ones.

Basically, restart your Explorer first for this to take effect.

# Disabling Explorer Font Changer
Simply change the font name to "None", or turn off the plugin.

### Formerly did this with WinDbg for a joke, but liked it
https://bsky.app/profile/cristei.bsky.social/post/3lzmqhdlhc22q
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
  $name: Custom font
  $description: This font will be used for all font creation in Windows.
*/
// ==/WindhawkModSettings==

// The source code of the mod starts here. This sample was inspired by the great
// article of Kyle Halladay, X64 Function Hooking by Example:
// http://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
// If you're new to terms such as code injection and function hooking, the
// article is great to get started.

#include <memory>
#include <string>

using namespace std::string_view_literals;

// Manage setting resource using `std::unique_ptr`.
using string_setting_raii_t = std::unique_ptr<
    const WCHAR, decltype(&Wh_FreeStringSetting)
>;

using create_font_indirectw_hook_t = decltype(&CreateFontIndirectW);
create_font_indirectw_hook_t create_font_indirectw_original = nullptr;

HFONT WINAPI create_font_indirectw_hook(const LOGFONTW* _font) {
    // Cast the const away for operations. Could be unsafe.
    auto font = const_cast<LOGFONTW*>(_font);

    auto set_font_name = string_setting_raii_t(
        Wh_GetStringSetting(L"font.name"),
        Wh_FreeStringSetting
    );
    auto font_name = std::wstring_view(set_font_name.get());

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

            Wh_Log(L"Succesfully changed font to \"%s\"", face_name);
        } else {
            Wh_Log(L"Trying to change font to \"%s\": size too long (%d)", font_name.data(), font_name.size());
        }
    }

    return create_font_indirectw_original(_font);
}

BOOL Wh_ModInit() {
    auto gdi32 = LoadLibraryW(L"gdi32.dll");
    auto create_font_indirectw = reinterpret_cast<create_font_indirectw_hook_t>(
        GetProcAddress(gdi32, "CreateFontIndirectW")
    );

    Wh_SetFunctionHook(
        (void*)create_font_indirectw,
        (void*)create_font_indirectw_hook,
        (void**)&create_font_indirectw_original
    );

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
