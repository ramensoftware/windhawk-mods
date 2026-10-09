// ==WindhawkMod==
// @id              dwrite-font-fallback-override
// @name            DirectWrite Font Fallback Override
// @description     Intercepts DirectWrite font fallback to replace fonts for specific characters
// @version         1.0
// @license         MIT
// @author          Muhammad Ragib Hasin
// @github          https://github.com/RagibHasin
// @include         *
// @compilerOptions -ldwrite -lole32 -lwindowsapp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DirectWrite Font Fallback Override

Hooks DirectWrite's font fallback system to override font selection for specific
Unicode character ranges across all applications.

## Features
- Global font fallback override
- Per-character-range customization
- Real-time configuration updates

## Usage
Configure fallback rules in settings to map character ranges to specific fonts.
For example, force all CJK characters (U+4E00-U+9FFF) to use "Microsoft YaHei".
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

#include <mutex>
#include <string>
#include <vector>

#include <windhawk_api.h>
#include <windhawk_utils.h>

using namespace winrt;

// Configuration
struct FallbackRule {
    char32_t startChar;
    char32_t endChar;
    std::wstring fontFamily;
};

std::vector<FallbackRule> g_fallbackRules;
std::mutex g_rulesMutex;

com_ptr<IDWriteFontFallback1> g_fallback;
std::mutex g_fallbackInitMutex;

using GetSystemFontFallback_t = HRESULT(
    STDMETHODCALLTYPE*)(IDWriteFactory8* self, IDWriteFontFallback1** fallback);

GetSystemFontFallback_t g_originalGetSystemFontFallback = nullptr;

STDMETHODIMP GetSystemFontFallback_Hook(IDWriteFactory8* self,
                                        IDWriteFontFallback1** fallback) {
    Wh_Log(L"GetSystemFontFallback");
    std::lock_guard<std::mutex> lock(g_fallbackInitMutex);

    if (g_fallback == nullptr) {
        com_ptr<IDWriteFontFallback1> fallback0 = nullptr;
        auto hr = g_originalGetSystemFontFallback(self, fallback0.put());

        com_ptr<IDWriteFontFallbackBuilder> builder = nullptr;

#define check_or(h)                     \
    if (FAILED(h)) {                    \
        *fallback = fallback0.detach(); \
        return hr;                      \
    }
        check_or(self->CreateFontFallbackBuilder(builder.put()));

        for (const auto& rule : g_fallbackRules) {
            DWRITE_UNICODE_RANGE range{rule.startChar, rule.endChar};
            auto fontFamily = rule.fontFamily.c_str();
            check_or(builder->AddMapping(&range, 1, &fontFamily, 1));

            Wh_Log(L"FontFallback: U+%04X - U+%04X -> %s", rule.startChar,
                   rule.endChar, fontFamily);
        }
        check_or(builder->AddMappings(fallback0.get()));

        com_ptr<IDWriteFontFallback> fallback1 = nullptr;
        check_or(builder->CreateFontFallback(fallback1.put()));
        fallback1.as(g_fallback);

        Wh_Log(L"Injected overrides");
    }

    g_fallback.copy_to(fallback);
    return S_OK;
}

using CreateTextFormat0_t =
    HRESULT(STDMETHODCALLTYPE*)(IDWriteFactory8* self,
                                const WCHAR* family_name,
                                IDWriteFontCollection* collection,
                                DWRITE_FONT_WEIGHT weight,
                                DWRITE_FONT_STYLE style,
                                DWRITE_FONT_STRETCH stretch,
                                FLOAT size,
                                const WCHAR* locale,
                                IDWriteTextFormat** format);

CreateTextFormat0_t g_originalCreateTextFormat0 = nullptr;

STDMETHODIMP CreateTextFormat0_Hook(IDWriteFactory8* self,
                                    WCHAR const* fontFamilyName,
                                    IDWriteFontCollection* fontCollection,
                                    DWRITE_FONT_WEIGHT fontWeight,
                                    DWRITE_FONT_STYLE fontStyle,
                                    DWRITE_FONT_STRETCH fontStretch,
                                    FLOAT fontSize,
                                    WCHAR const* localeName,
                                    IDWriteTextFormat** textFormat) {
    Wh_Log(L"IDWriteFacotry::CreateTextFormat");
    auto hr = g_originalCreateTextFormat0(self, fontFamilyName, fontCollection,
                                          fontWeight, fontStyle, fontStretch,
                                          fontSize, localeName, textFormat);

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

using CreateTextLayout0_t =
    HRESULT(STDMETHODCALLTYPE*)(IDWriteFactory8* self,
                                WCHAR const* string,
                                UINT32 stringLength,
                                IDWriteTextFormat* textFormat,
                                FLOAT maxWidth,
                                FLOAT maxHeight,
                                IDWriteTextLayout** textLayout);

CreateTextLayout0_t g_originalCreateTextLayout0 = nullptr;

STDMETHODIMP CreateTextLayout0_Hook(IDWriteFactory8* self,
                                    WCHAR const* string,
                                    UINT32 stringLength,
                                    IDWriteTextFormat* textFormat,
                                    FLOAT maxWidth,
                                    FLOAT maxHeight,
                                    IDWriteTextLayout** textLayout) {
    Wh_Log(L"IDWriteFactory::CreateTextLayout");
    auto hr =
        g_originalCreateTextLayout0(self, string, stringLength, textFormat,
                                    maxWidth, maxHeight, textLayout);
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

using CreateTextFormat6_t =
    HRESULT(STDMETHODCALLTYPE*)(IDWriteFactory8* self,
                                const WCHAR* familyname,
                                IDWriteFontCollection* collection,
                                const DWRITE_FONT_AXIS_VALUE* axis_values,
                                UINT32 num_axis,
                                FLOAT fontsize,
                                const WCHAR* localename,
                                IDWriteTextFormat3** text_format);

CreateTextFormat6_t g_originalCreateTextFormat6 = nullptr;

STDMETHODIMP CreateTextFormat6_Hook(IDWriteFactory8* self,
                                    const WCHAR* familyname,
                                    IDWriteFontCollection* collection,
                                    const DWRITE_FONT_AXIS_VALUE* axis_values,
                                    UINT32 num_axis,
                                    FLOAT fontsize,
                                    const WCHAR* localename,
                                    IDWriteTextFormat3** text_format) {
    Wh_Log(L"IDWriteFactory6::CreateTextFormat");
    auto hr = g_originalCreateTextFormat6(self, familyname, collection,
                                          axis_values, num_axis, fontsize,
                                          localename, text_format);
    if (SUCCEEDED(hr) && text_format && *text_format) {
        // Try to set custom fallback on the text format
        com_ptr<IDWriteFontFallback> customFallback;
        self->GetSystemFontFallback(customFallback.put());
        (*text_format)->SetFontFallback(customFallback.get());

        Wh_Log(L"CreateTextFormat: Injected fallback into text format");
    }
    return hr;
}

// Parse hex string to char32_t
char32_t ParseHex(const WCHAR* str) {
    char32_t value = 0;
    swscanf_s(str, L"%X", &value);
    return value;
}

// Load settings
void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_rulesMutex);
    g_fallbackRules.clear();

    using WindhawkUtils::StringSetting;

    // Load array of rules
    for (int i = 0;; i++) {
        // Check if this array element exists by trying to read enabled flag
        auto startChar = StringSetting::make(L"rules[%d].startChar", i);

        if (wcslen(startChar) == 0)
            break;  // No more rules

        auto endChar = StringSetting::make(L"rules[%d].endChar", i);
        auto fontFamily = StringSetting::make(L"rules[%d].fontFamily", i);

        FallbackRule rule{
            ParseHex(startChar),
            ParseHex(endChar),
            std::wstring(fontFamily),
        };
        g_fallbackRules.emplace_back(rule);

        Wh_Log(L"Loaded rule %d: U+%04X-U+%04X -> %s", i, rule.startChar,
               rule.endChar, rule.fontFamily.c_str());
    }

    Wh_Log(L"Loaded %zu rules total", g_fallbackRules.size());
}

constexpr void* get_item_in_vtable(IUnknown* vtable, uint32_t idx) {
    return (*reinterpret_cast<void***>(vtable))[idx];
}

BOOL Wh_ModInit() {
    Wh_Log(L"DirectWrite Font Fallback Override (C++/WinRT) - Initializing");

    LoadSettings();

    IUnknown* factory;
    auto hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                  __uuidof(IDWriteFactory), &factory);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to create first factory");
        return FALSE;
    }

    // IUnknown        :  3 :  0
    // IDWriteFactory  : 21 :  3
    // IDWriteFactory1 :  2 : 24
    // IDWriteFactory2 :  5 : 26
    // IDWriteFactory3 :  9 : 31
    // IDWriteFactory4 :  3 : 40
    // IDWriteFactory5 :  5 : 43
    // IDWriteFactory6 :  7 : 48

    auto pGetSystemFontFallback =
        get_item_in_vtable(factory, 26);  // 1st of IDWriteFactory2
    auto pCreateTextFormat0 =
        get_item_in_vtable(factory, 15);  // 13th of IDWriteFactory
    auto pCreateTextLayout0 =
        get_item_in_vtable(factory, 18);  // 16th of IDWriteFactory
    auto pCreateTextFormat6 =
        get_item_in_vtable(factory, 54);  // 7th of IDWriteFactory6

    com_ptr<IDWriteFactory> factory0(factory, take_ownership_from_abi);
    bool supportsFactory6 = (bool)factory0.try_as<IDWriteFactory6>();

    if (!pGetSystemFontFallback) {
        Wh_Log(L"Failed to find GetSystemFontFallback");
        return FALSE;
    }
    Wh_SetFunctionHook(pGetSystemFontFallback,
                       (void*)GetSystemFontFallback_Hook,
                       (void**)&g_originalGetSystemFontFallback);
    Wh_Log(L"Successfully hooked GetSystemFontFallback");

    if (!pCreateTextFormat0) {
        Wh_Log(L"Failed to find CreateTextFormat0");
        return FALSE;
    }
    Wh_SetFunctionHook(pCreateTextFormat0, (void*)CreateTextFormat0_Hook,
                       (void**)&g_originalCreateTextFormat0);
    Wh_Log(L"Successfully hooked CreateTextFormat0");

    if (!pCreateTextLayout0) {
        Wh_Log(L"Failed to find CreateTextLayout0");
        return FALSE;
    }
    Wh_SetFunctionHook(pCreateTextLayout0, (void*)CreateTextLayout0_Hook,
                       (void**)&g_originalCreateTextLayout0);
    Wh_Log(L"Successfully hooked CreateTextLayout0");

    if (!supportsFactory6) {
        Wh_Log(L"System does not support IDWriteFactory6");
        Wh_Log(L"Did not attempt hooking CreateTextFormat6");
        return TRUE;
    }
    if (!pCreateTextFormat6) {
        Wh_Log(L"Failed to find CreateTextFormat6");
        return FALSE;
    }
    Wh_SetFunctionHook(pCreateTextFormat6, (void*)CreateTextFormat6_Hook,
                       (void**)&g_originalCreateTextFormat6);
    Wh_Log(L"Successfully hooked CreateTextFormat6");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"DirectWrite Font Fallback Override (C++/WinRT) - Uninitializing");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
    g_fallback = nullptr;
}
