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
- font:
  - name: "None"
  $name: Custom font
  $description: This font will be used for all future fonts in Explorer processes.
*/
// ==/WindhawkModSettings==

using CreateFontIndirectWHook_t = decltype(&CreateFontIndirectW);
CreateFontIndirectWHook_t pOriginal = nullptr;
HFONT __stdcall CreateFontIndirectWHook(const LOGFONTW* font) {
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
