// ==WindhawkMod==
// @id              explorer-font-changer
// @name            Explorer Font Changer
// @description     Comic Sans anybody? Every new GDI font in the Explorer context will be changed to your font of choice.
// @version         0.1
// @author          Gabriela Cristei
// @github          https://github.com/cristeigabriela
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Font Changer

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

using CreateFontIndirectWHook_t = decltype(&CreateFontIndirectW);
CreateFontIndirectWHook_t pOriginal = nullptr;
HFONT __stdcall CreateFontIndirectWHook(const LOGFONTW* font) {
    HWND currWindow = GetActiveWindow();
    // TODO(gabriela): localize strings or at least check by icon instead.
    // this will only work for english!
    char title[300] = {0};
    if (GetWindowTextA(currWindow, &title[0], sizeof(title)) != 0
            && strstr(title, "Run") == title) {
        return pOriginal(font);
    }

    LPCWSTR str = Wh_GetStringSetting(L"font.name");
    if (wcsstr(str, L"None") != str) {
        size_t len = wcslen(str);
        if (len <= 31) {
            LOGFONTW* mFont = (LOGFONTW*)font;
            wcscpy((wchar_t*)mFont->lfFaceName, str);
            mFont->lfFaceName[len] = 0;
        } else {
            Wh_Log(L"Font name too long! (%d characters, max: 31)", len);
        }
    }
    Wh_FreeStringSetting(str);

    return pOriginal(font);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE gdi32 = LoadLibrary(L"gdi32.dll");
    CreateFontIndirectWHook_t CreateFontIndirectW =
        (CreateFontIndirectWHook_t)GetProcAddress(gdi32, "CreateFontIndirectW");

    Wh_SetFunctionHook((void*)CreateFontIndirectW,
                       (void*)CreateFontIndirectWHook,
                       (void**)&pOriginal);
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
