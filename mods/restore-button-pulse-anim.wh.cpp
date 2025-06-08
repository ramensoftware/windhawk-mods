// ==WindhawkMod==
// @id              restore-button-pulse-anim
// @name            Restore Button Pulse Animation
// @description     Restores the pulse animation that focused buttons played in Windows Vista/7
// @version         1.0.2
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luxtheme -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Restore Button Pulse Animation
In Windows Vista and 7, buttons played a "pulse" animation when they were
focused. This mod restores that functionality.

![Preview](https://raw.githubusercontent.com/aubymori/images/main/restore-button-pulse-anim-preview.gif)

## Note
While the theme state for the pulse animation still exists in Windows 8 and 10 to
accomodate third party applications that custom draw controls, the images in the
default theme are identical, and you will not see an animation. Make sure you're
using a Windows Vista/7-styled theme in order to see the effect.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <psapi.h>
#include <vector>

struct DPISCALEINFO
{
    UINT m_uDpiX;
    UINT m_uDpiY;
    bool m_fDPIAware : 1;
    bool m_fIsThemingEnabled : 1;
    bool m_fIsIgnoringDpiChanges : 1;
};

typedef struct tagControlInfo
{
    HWND hwnd;
    HWND hwndParent;
    UINT style;
    DWORD dwCustom;
#ifdef _WIN64
    BOOL bUnicode;
    BOOL bInFakeCustomDraw;
#else
    BOOL bUnicode : 1;
    BOOL bInFakeCustomDraw : 1;
#endif
    UINT uiCodePage;
    DWORD dwExStyle;
    int iVersion;
    WORD wUIState;
    DPISCALEINFO dpi;
} CCONTROLINFO, *LPCCONTROLINFO;

typedef struct tagWW
{
    DWORD dwState;
    DWORD dwState2;
    DWORD dwExStyle;
    DWORD dwStyle;
} WW, *PWW;

typedef struct tagBUTN
{
    CCONTROLINFO ci;
    UINT buttonState;
    void *hFont;
    void *hImage;
    bool fPaintKbdCuesOnly : 1;
    bool fIsShieldIconSet : 1;
    bool fBufferedPaintInit : 1;
    bool fCachedSizeIsCheckbox : 1;
    RECT rcText;
    HDC hdcSave;
    BUTTON_IMAGELIST biml;
    int fILOwner;
    int fILHasPriority;
    PWW pww;
    SIZE sizeGlyph;
    UINT uSplitStyle;
    HIMAGELIST himlGlyph;
    HTHEME hTheme;
    int iStateId;
    int iDefaultStateId;
    // We don't need anything past iDefaultStateId.
    // CCompositedDraw cd;
    // CommandLink *pCommandLink;
    // SIZE cachedPartSize;
} BUTN, *PBUTN;

struct BTNTHEMEID
{
    int iPartId = -1;
    int iStateId = -1;
};

UINT OppositePulse(UINT uState)
{
    return (uState == PBS_DEFAULTED) ? PBS_DEFAULTED_ANIMATING : PBS_DEFAULTED;
}

BOOL IsDefPushBtn(DWORD dwStyle, const BTNTHEMEID &themeId)
{
    // Unsure what this case is for? There is no default window
    // style with the value 0x1000. Keeping it anyway.
    if (dwStyle & 0x1000)
        return FALSE;

    if (themeId.iPartId == BP_PUSHBUTTON
    && themeId.iStateId == PBS_DEFAULTED)
        return TRUE;
        
    if (themeId.iPartId == BP_COMMANDLINK
    && themeId.iStateId == PBS_DEFAULTED)
        return TRUE;

    return FALSE;
}

HRESULT (__fastcall *Button_GetThemeIds)(PBUTN pbutn, BTNTHEMEID *pBtnThemeId);
DWORD (__fastcall *Button_SoftFadeDuration)(HTHEME hTheme, int iPartId, int iStateIdFrom, int iStateIdTo);
BOOL (__fastcall *Button_IsSoftFadeCapable)(PBUTN pbutn);

// Real function, but inlined a lot of the time.
BOOL WINAPI Button_IsParentActive(PBUTN pbutn)
{
    return GetForegroundWindow() == GetAncestor(pbutn->ci.hwnd, GA_ROOT);
}

// The check for whether a button is animating relies on a removed member,
// just use a list of animating buttons
std::vector<PBUTN> animatingButtons;

inline bool Button_IsAnimating(PBUTN pbutn)
{
    return std::find(animatingButtons.begin(), animatingButtons.end(), pbutn) != animatingButtons.end();
}

inline void Button_SetAnimating(PBUTN pbutn, bool fAnimating)
{
    if (fAnimating)
    {
        if (!Button_IsAnimating(pbutn))
            animatingButtons.push_back(pbutn);
    }
    else
    {
        animatingButtons.erase(std::remove(animatingButtons.begin(), animatingButtons.end(), pbutn), animatingButtons.end());
    }
}

void CALLBACK Button_DefaultStateAnimationTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    KillTimer(hwnd, idEvent);
    PBUTN pbutn = (PBUTN)GetWindowLongPtrW(hwnd, 0);
    if (pbutn)
    {
        BTNTHEMEID btnThemeId;
        Button_GetThemeIds(pbutn, &btnThemeId);
        if (Button_IsSoftFadeCapable(pbutn)
        && (btnThemeId.iPartId = BP_PUSHBUTTON || btnThemeId.iPartId == BP_COMMANDLINK)
        && btnThemeId.iStateId == PBS_DEFAULTED)
        {
            if (Button_IsParentActive(pbutn))
            {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            else
            {
                if (pbutn->iStateId != PBS_DEFAULTED)
                    InvalidateRect(hwnd, nullptr, FALSE);
                pbutn->iStateId = PBS_DEFAULTED;
            }

            DWORD dwSoftFadeDuration = Button_SoftFadeDuration(pbutn->hTheme, btnThemeId.iPartId, PBS_DEFAULTED, PBS_DEFAULTED_ANIMATING);
            SetTimer(hwnd, (UINT_PTR)pbutn, dwSoftFadeDuration, Button_DefaultStateAnimationTimerProc);
            Button_SetAnimating(pbutn, true);
        }
        else
        {
            Button_SetAnimating(pbutn, false);
        }
    }
}

void Button_DefPushBtnPulse(
    PBUTN pbutn,
    BP_ANIMATIONPARAMS *pAnimationParams,
    BTNTHEMEID *pThemeIdTo,
    BTNTHEMEID *pThemeIdFrom
)
{
    DWORD dwPulseDuration = Button_SoftFadeDuration(pbutn->hTheme, pThemeIdTo->iPartId, PBS_DEFAULTED, PBS_DEFAULTED_ANIMATING);
    int iState = pbutn->iStateId;
    if ((iState == PBS_DEFAULTED || iState == PBS_DEFAULTED_ANIMATING) && Button_IsAnimating(pbutn))
    {
        pThemeIdFrom->iStateId = pbutn->iStateId;
        pThemeIdTo->iStateId = OppositePulse(pbutn->iStateId);
        pAnimationParams->style = BPAS_SINE;
        pAnimationParams->dwDuration = dwPulseDuration;
    }
    else
    {
        SetTimer(pbutn->ci.hwnd, (UINT_PTR)pbutn, dwPulseDuration, Button_DefaultStateAnimationTimerProc);
        Button_SetAnimating(pbutn, true);
        pbutn->iStateId = PBS_DEFAULTED_ANIMATING;
    }
}

void (__fastcall *Button_PaintImpl)(PBUTN pbutn, HDC hdc, BOOL fSoftFadeCapable, BTNTHEMEID const &btnThemeId);

void (__fastcall *Button_PaintDirectly_orig)(PBUTN, HDC, BOOL);
void __fastcall Button_PaintDirectly_hook(PBUTN pbutn, HDC hdc, BOOL fEnableDoubleBuffer)
{
    BTNTHEMEID btnThemeId;
    Button_GetThemeIds(pbutn, &btnThemeId);

    // Draw directly if we can't soft fade.
    BOOL fSoftFadeCapable = Button_IsSoftFadeCapable(pbutn);
    if (!fEnableDoubleBuffer || !fSoftFadeCapable)
    {
        Button_PaintImpl(pbutn, hdc, fSoftFadeCapable, btnThemeId);
        return;
    }

    DWORD dwSoftFadeDuration = Button_SoftFadeDuration(pbutn->hTheme, btnThemeId.iPartId, pbutn->iStateId, btnThemeId.iStateId);
    if (!pbutn->fBufferedPaintInit)
    {
        BufferedPaintInit();
        pbutn->fBufferedPaintInit = true;
    }
    if (!BufferedPaintRenderAnimation(pbutn->ci.hwnd, hdc))
    {
        // Attempt animation
        BP_ANIMATIONPARAMS animParams = { sizeof(BP_ANIMATIONPARAMS) };
        RECT rc;
        GetClientRect(pbutn->ci.hwnd, &rc);
        animParams.dwDuration = dwSoftFadeDuration;
        BTNTHEMEID oldThemeId = { btnThemeId.iPartId, pbutn->iStateId };
        animParams.style = BPAS_LINEAR;
        if (IsDefPushBtn(pbutn->pww->dwStyle, btnThemeId))
            Button_DefPushBtnPulse(pbutn, &animParams, &btnThemeId, &oldThemeId);
        HDC hdcFrom, hdcTo;
        HDC hdcTarget = (HDC)BeginBufferedAnimation(
            pbutn->ci.hwnd,
            hdc,
            &rc,
            BPBF_COMPATIBLEBITMAP,
            nullptr,
            &animParams,
            &hdcFrom,
            &hdcTo
        );
        if (hdcTarget)
        {
            // Draw states onto animation HDCs if we have them.
            if (hdcFrom)
                Button_PaintImpl(pbutn, hdcFrom, fSoftFadeCapable, oldThemeId);
            if (hdcTo)
                Button_PaintImpl(pbutn, hdcTo, fSoftFadeCapable, btnThemeId);
            pbutn->iStateId = btnThemeId.iStateId;
            EndBufferedAnimation((HANIMATIONBUFFER)hdcTarget, TRUE);
            return;
        }
        
        // Draw directly if animation fails.
        Button_PaintImpl(pbutn, hdc, fSoftFadeCapable, btnThemeId);
    }
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
{ 
    void *pFixedFileInfo = nullptr; 
    UINT uPtrLen = 0; 

    HRSRC hResource = 
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION); 
    if (hResource)
    { 
        HGLOBAL hGlobal = LoadResource(hModule, hResource); 
        if (hGlobal)
        { 
            void *pData = LockResource(hGlobal); 
            if (pData)
            { 
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                { 
                    pFixedFileInfo = nullptr; 
                    uPtrLen = 0; 
                } 
            } 
        } 
    } 

    if (puPtrLen)
    { 
        *puPtrLen = uPtrLen; 
    } 
  
     return (VS_FIXEDFILEINFO *)pFixedFileInfo; 
 } 

/**
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  */
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

#ifdef _WIN64
#   define  SSTDCALL  L"__cdecl"
#else
#   define  SSTDCALL  L"__stdcall"
#endif

const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
    {
        {
            L"long "
            SSTDCALL
            L" Button_GetThemeIds(struct tagBUTN const *,struct BTNTHEMEID *)"
        },
        &Button_GetThemeIds,
        nullptr,
        false
    },
    {
        {
            L"int "
            SSTDCALL
            L" Button_IsSoftFadeCapable(struct tagBUTN *)"
        },
        &Button_IsSoftFadeCapable,
        nullptr,
        false
    },
    {
        {
            L"void "
            SSTDCALL
            L" Button_PaintImpl(struct tagBUTN *,struct HDC__ *,int,struct BTNTHEMEID const &)"
        },
        &Button_PaintImpl,
        nullptr,
        false
    },
    {
        {
            L"unsigned long "
            SSTDCALL
            L" Button_SoftFadeDuration(void *,int,int,int)"
        },
        &Button_SoftFadeDuration,
        nullptr,
        false
    },
    {
        {
            L"void "
            SSTDCALL
            L" Button_PaintDirectly(struct tagBUTN *,struct HDC__ *,int)"
        },
        &Button_PaintDirectly_orig,
        Button_PaintDirectly_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hComCtl,
        comctl32DllHooks,
        ARRAYSIZE(comctl32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in comctl32.dll");
        return FALSE;
    }

    return TRUE;
}