// ==WindhawkMod==
// @id              mshtml-native-controls
// @name            MSHTML Tweaks
// @description     Various tweaks for the IE engine
// @version         1.0
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MSHTML Tweaks
* Various tweaks for Internet Explorer and all the other apps using its engine (Trident/MSHTML), such as HTML Help, HelpPane, MSHTA, MMC, etc.
* This mod can bring back old controls, customize the font smoothing type (disable the forced ClearType), and disable the DirectComposition rendering.
* Special thanks to @xalejandro for the ClearType disabler.
## Notes regarding the old control styles
* Note that this mod only restores IE11's built-in Windows 7 behavior for old control styling, which uses hardcoded/fake Aero styles that do not actually match the current system theme.
* Only the scrollbar will use actual bitmaps from the current visual style (or classic); however, they may look glitchy, especially on HiDPI displays.
* You may want to disable the visual styles for controls in Internet Options (`inetcpl.cpl`) -> Advanced -> `Enable visual styles on buttons and controls in web pages` to make them look better on older themes such as XP.
![Comparison of fake and real control styles](https://raw.githubusercontent.com/Ingan121/files/refs/heads/master/ie_fakeaero.png)
## If Internet Explorer hangs on startup
* Internet Explorer (`iexplore.exe`) uses a 32-bit subprocess by default, and the subprocess will fail to save its symbol downloaded from the Windhawk server.
* To fix this, please run the following in the run dialog (Win+R) to launch a 32-bit MSHTML as a main process to load the symbols:
* `C:\Windows\SysWOW64\hh.exe http://www.ingan121.com/` (or any other `http` URL that isn't `https`)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- oldcontrols: true
  $name: Enable Windows 7 style controls
  $name:ko-KR: Windows 7 스타일 컨트롤 활성화
  $description: Enable the old Windows 7 style controls in MSHTML, which will replace the Windows 8 style controls. This also restores the true native scrollbar styles.
  $description:ko-KR: MSHTML에서 Windows 8 스타일의 새로운 컨트롤을 Windows 7 스타일의 이전 컨트롤로 바꿉니다. 이 옵션은 스크롤바도 시스템 스타일을 따르게 합니다.
- textrendering: default
  $name: Text rendering mode
  $name:ko-KR: 텍스트 렌더링 모드
  $options:
    - default: Default (ClearType regardless of system setting)
    - no-cleartype: Non-ClearType Antialiasing
    - grayscale: Grayscale Antialiasing
    - aliased: Aliased
  $options:ko-KR:
    - default: 기본값 (시스템 설정과 관계없이 ClearType)
    - no-cleartype: ClearType 없는 안티앨리어싱
    - grayscale: 그레이스케일 안티앨리어싱
    - aliased: 안티앨리어싱 없음
  $description: Customize the text rendering mode in MSHTML, which will affect how fonts are displayed.
  $description:ko-KR: MSHTML에서 텍스트 렌더링 모드를 사용자 정의하여 글꼴이 어떻게 표시되는지 조절할 수 있습니다.
- nodcomp: false
  $name: Disable DirectComposition
  $name:ko-KR: DirectComposition 비활성화
  $description: Disable DirectComposition rendering in MSHTML, which will make the web content visible without DWM.
  $description:ko-KR: MSHTML에서 DirectComposition 렌더링을 비활성화하여 DWM 없이도 웹 콘텐츠가 보이도록 합니다.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <d2d1.h>
#include <d2d1_1.h>

#ifdef _WIN64
#define SSTDCALL L"__cdecl"
#else
#define SSTDCALL L"__stdcall"
#endif

enum TextRenderingMode {
    Default,
    NoClearType,
    Grayscale,
    Aliased
};

struct settings {
    bool oldControls = true;
    TextRenderingMode textRenderingMode = Default;
    bool noDComp = false;
} g_settings;

thread_local bool g_fDrawGlyphRun = false;

typedef __int64 (__stdcall* CTridentPrivateDebugAPI_SetEnableWebControlVisuals_t)(void* pThis, int enable);
CTridentPrivateDebugAPI_SetEnableWebControlVisuals_t SetEnableWebControlVisuals;

typedef __int64 (__stdcall* CTridentPrivateDebugAPI_SetDCompEnabled_t)(void* pThis, int enable);
CTridentPrivateDebugAPI_SetDCompEnabled_t SetDCompEnabled;

typedef int (__stdcall* CTooltip_ShowModernTip_t)(void* pThis, const unsigned __int16* a2, HWND a3, __int64 a4, void* a5, __int64 a6, int a7, void* a8, int a9, int a10, int a11);
CTooltip_ShowModernTip_t CTooltip_ShowModernTip_original;
int __stdcall CTooltip_ShowModernTip_hook(void* pThis, void* unused, const unsigned __int16* a2, HWND a3, __int64 a4, void* a5, __int64 a6, int a7, void* a8, int a9, int a10, int a11) {
    return E_NOTIMPL; // Deny to show the older one
}

void (_fastcall *CDXRenderTarget_DrawGlyphRun_orig)(void *, void *, void *, void *, void *, void *, void *, int, int, int, int);
void _fastcall CDXRenderTarget_DrawGlyphRun_hook(void *pThis, void *a2, void *a3, void *a4, void *a5, void *a6, void *a7, int a8, int a9, int a10, int a11)
{
    g_fDrawGlyphRun = true;
    CDXRenderTarget_DrawGlyphRun_orig(pThis, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
    g_fDrawGlyphRun = false;
}

void (STDMETHODCALLTYPE *D2DDeviceContextBase_ID2D1DeviceContext_DrawGlyphRun_orig)(ID2D1DeviceContext *pThis, D2D1_POINT_2F baselineOrigin, DWRITE_GLYPH_RUN *glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION *glyphRunDescription, ID2D1Brush *foregroundBrush, DWRITE_MEASURING_MODE measuringMode);
void STDMETHODCALLTYPE D2DDeviceContextBase_ID2D1DeviceContext_DrawGlyphRun_hook(ID2D1DeviceContext *pThis, D2D1_POINT_2F baselineOrigin, DWRITE_GLYPH_RUN *glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION *glyphRunDescription, ID2D1Brush *foregroundBrush, DWRITE_MEASURING_MODE measuringMode)
{
    if (pThis && g_fDrawGlyphRun && g_settings.textRenderingMode != Default)
    {
        pThis->SetTextRenderingParams(NULL);
        D2D1_TEXT_ANTIALIAS_MODE antialiasMode = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
        switch (g_settings.textRenderingMode) {
            case NoClearType:
                antialiasMode = D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
                break;
            case Grayscale:
                antialiasMode = D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
                break;
            case Aliased:
                antialiasMode = D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
                break;
            default:
                break;
        }
        pThis->SetTextAntialiasMode(antialiasMode);
    }

    return D2DDeviceContextBase_ID2D1DeviceContext_DrawGlyphRun_orig(pThis, baselineOrigin, glyphRun, glyphRunDescription, foregroundBrush, DWRITE_MEASURING_MODE_GDI_CLASSIC);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_original;
HMODULE WINAPI LoadLibraryExW_hook(const wchar_t* lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE res = LoadLibraryExW_original(lpLibFileName, hFile, dwFlags);
    if (res == NULL) {
        return NULL;
    }
    wchar_t moduleName[MAX_PATH];
    if (GetModuleFileNameW(res, moduleName, MAX_PATH)) {
        if (wcsstr(_wcsupr(moduleName), L"MSHTML.DLL") != NULL) {
            Wh_Log(L"mshtml.dll loaded, hooking symbols");
            WindhawkUtils::SYMBOL_HOOK mshtmlHooks[] = {
                {
                    {
                        L"public: virtual long " SSTDCALL " CTridentPrivateDebugAPI::SetEnableWebControlVisuals(int)"
                    },
                    (void**)&SetEnableWebControlVisuals,
                    NULL,
                    FALSE
                },
                {
                    {
                        L"public: virtual long " SSTDCALL " CTridentPrivateDebugAPI::SetDCompEnabled(int)"
                    },
                    (void**)&SetDCompEnabled,
                    NULL,
                    FALSE
                },
                {
                    {
                        #ifdef _WIN64
                        L"private: long __cdecl CTooltip::ShowModernTip(unsigned short *,struct HWND__ *,struct tagMSG *,struct tagRECT *,unsigned __int64,int,struct tagPOINT *,int,enum TOOLTIP_PLACEMENT,enum TOOLTIP_TEXTSIZE)"
                        #else
                        L"private: long __thiscall CTooltip::ShowModernTip(unsigned short *,struct HWND__ *,struct tagMSG *,struct tagRECT *,unsigned long,int,struct tagPOINT *,int,enum TOOLTIP_PLACEMENT,enum TOOLTIP_TEXTSIZE)"
                        #endif
                    },
                    (void**)&CTooltip_ShowModernTip_original,
                    (void*)CTooltip_ShowModernTip_hook,
                    FALSE
                },
                {
                    {
                        #ifdef _WIN64
                        L"public: virtual void __cdecl CDXRenderTarget::DrawGlyphRun(class CPointF,struct DWRITE_GLYPH_RUN const *,struct ID2D1Brush *,unsigned short const *,unsigned int,unsigned short const *,enum IDispFont::RenderingMode,enum ColorFontMode)const "
                        #else
                        L"public: virtual void __thiscall CDXRenderTarget::DrawGlyphRun(class CPointF,struct DWRITE_GLYPH_RUN const *,struct ID2D1Brush *,unsigned short const *,unsigned int,unsigned short const *,enum IDispFont::RenderingMode,enum ColorFontMode)const "
                        #endif
                    },
                    (void**)&CDXRenderTarget_DrawGlyphRun_orig,
                    (void*)CDXRenderTarget_DrawGlyphRun_hook,
                    FALSE
                },
            };
            WindhawkUtils::SYMBOL_HOOK d2d1Hook {
                {
                    #ifdef _WIN64
                    L"public: virtual void __cdecl D2DDeviceContextBase<struct ID2D1DeviceContext6,struct ID2D1DeviceContext6,class null_type>::DrawGlyphRun(struct D2D_POINT_2F,struct DWRITE_GLYPH_RUN const *,struct DWRITE_GLYPH_RUN_DESCRIPTION const *,struct ID2D1Brush *,enum DWRITE_MEASURING_MODE)",
                    L"public: virtual void __cdecl D2DDeviceContextBase<struct ID2D1DeviceContext7,struct ID2D1DeviceContext7,class null_type>::DrawGlyphRun(struct D2D_POINT_2F,struct DWRITE_GLYPH_RUN const *,struct DWRITE_GLYPH_RUN_DESCRIPTION const *,struct ID2D1Brush *,enum DWRITE_MEASURING_MODE)"
                    #else
                    L"public: virtual void __stdcall D2DDeviceContextBase<struct ID2D1DeviceContext6,struct ID2D1DeviceContext6,class null_type>::DrawGlyphRun(struct D2D_POINT_2F,struct DWRITE_GLYPH_RUN const *,struct DWRITE_GLYPH_RUN_DESCRIPTION const *,struct ID2D1Brush *,enum DWRITE_MEASURING_MODE)",
                    L"public: virtual void __stdcall D2DDeviceContextBase<struct ID2D1DeviceContext7,struct ID2D1DeviceContext7,class null_type>::DrawGlyphRun(struct D2D_POINT_2F,struct DWRITE_GLYPH_RUN const *,struct DWRITE_GLYPH_RUN_DESCRIPTION const *,struct ID2D1Brush *,enum DWRITE_MEASURING_MODE)"
                    #endif
                },
                &D2DDeviceContextBase_ID2D1DeviceContext_DrawGlyphRun_orig,
                D2DDeviceContextBase_ID2D1DeviceContext_DrawGlyphRun_hook,
                false
            };
            if (!WindhawkUtils::HookSymbols(res, mshtmlHooks, ARRAYSIZE(mshtmlHooks))) {
                Wh_Log(L"HookSymbols failed");
            } else {
                HMODULE d2d1 = GetModuleHandleW(L"d2d1.dll"); // should already be loaded at this point
                if (d2d1) {
                    if (WindhawkUtils::HookSymbols(d2d1, &d2d1Hook, 1)) {
                        Wh_Log(L"d2d1.dll HookSymbols OK");
                    }
                }
                Wh_ApplyHookOperations();

                if (g_settings.oldControls && SetEnableWebControlVisuals) {
                    SetEnableWebControlVisuals(NULL, FALSE);
                    Wh_Log(L"SetEnableWebControlVisuals called");
                }
                if (g_settings.noDComp && SetDCompEnabled) {
                    SetDCompEnabled(NULL, FALSE);
                    Wh_Log(L"SetDCompEnabled called");
                }
            }
        }
    }
    return res;
}

TextRenderingMode TextRenderingModeStringToEnum(LPCWSTR str) {
    if (wcscmp(str, L"default") == 0) {
        return Default;
    } else if (wcscmp(str, L"no-cleartype") == 0) {
        return NoClearType;
    } else if (wcscmp(str, L"grayscale") == 0) {
        return Grayscale;
    } else if (wcscmp(str, L"aliased") == 0) {
        return Aliased;
    } else {
        return Default;
    }
}

void LoadSettings() {
    g_settings.oldControls = Wh_GetIntSetting(L"oldcontrols");
    LPCWSTR textRenderingModeStr = Wh_GetStringSetting(L"textrendering");
    g_settings.textRenderingMode = TextRenderingModeStringToEnum(textRenderingModeStr);
    Wh_FreeStringSetting(textRenderingModeStr);
    g_settings.noDComp = Wh_GetIntSetting(L"nodcomp");
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    // Loading mshtml.dll in every process is definitely not a good idea, so hook this and begin mshtml hooks only when mshtml.dll is actually loading.
    if (!Wh_SetFunctionHook((void*)LoadLibraryExW, (void*)LoadLibraryExW_hook, (void**)&LoadLibraryExW_original)) {
        Wh_Log(L"Wh_SetFunctionHook LoadLibraryExW failed");
    }
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
