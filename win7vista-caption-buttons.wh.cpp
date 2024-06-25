// ==WindhawkMod==
// @id				win7vista-caption-buttons
// @name 			Restore Windows 7/Vista Caption Buttons.
// @description			this mod allows windows vista styled caption buttons.
// @version			0.1
// @author			PepperMarioYT
// @github			https://github.com/PepperMarioYT
// @include			dwm.exe
// @architecture		x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# The leaker known as ImSwordQueen. The leaker of mods while sleeping
## I, PepperMarioYT did not create this mod, instead it was created by Dulappy (https://github.com/Dulappy), i just modified it for Project Longhorn
##### Modified: enabled Vista caption buttons by default.
##### supports: only supports Vanadium, recconended 21H2
##### This does not include aero, you need the Aero10 Theme
##### add dwm.exe to the inclusion list
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Divisor: 21
  $name: Divisor
  $description: The divisor in CTopLevelWindow_UpdateNCAreaButton_Hook (default 21)
- VistaMaximize: true
  $name: Vista caption style
  $description: At 96dpi, add 1 pixel to the width of the Maximize/Restore button and subtracts 1 pixel from the button height.
*/
// ==/WindhawkModSettings==

#include <windef.h>
#include <wingdi.h>
#include <dwmapi.h>
#include <D3D9Types.h>
#include <d2d1effects.h>
#include <winuser.h>
#include <math.h>
#include <wincodec.h>


#define ID_CLOSE 3
#define ID_MAXRES 2
#define ID_MIN 1

#define STATE_MAXIMIZED 4

#define TYPE_TOOL 2

int divisor = 21;
BOOL vistaMaximize = false;

int (WINAPI *GetSystemMetricsForDpiOrig)(int nIndex, UINT dpi);
int WINAPI GetSystemMetricsForDpiHook(int nIndex, UINT dpi) {
    return GetSystemMetricsForDpiOrig(nIndex, dpi);
}

/* =============================================================================== */

typedef void (*CTopLevelWindow_UpdatePinnedParts_t)(void* pThis);
CTopLevelWindow_UpdatePinnedParts_t CTopLevelWindow_UpdatePinnedParts_Original;

typedef void (*CVisual_SetInsetFromParentTop_t)(void* pThis, int inset);
CVisual_SetInsetFromParentTop_t CVisual_SetInsetFromParentTop_Original;

typedef void (*CVisual_SetInsetFromParentLeft_t)(void* pThis, int inset);
CVisual_SetInsetFromParentLeft_t CVisual_SetInsetFromParentLeft_Original;

typedef void (*CVisual_SetDirtyFlags_t)(void* pThis, int flag);
CVisual_SetDirtyFlags_t CVisual_SetDirtyFlags_Original;

typedef void (*CVisual_SetInsetFromParent_t)(void* pThis, struct _MARGINS const *);
CVisual_SetInsetFromParent_t CVisual_SetInsetFromParent_Original;

typedef void (*CText_SetSize_t)(void* pThis, struct tagSIZE const *);
CText_SetSize_t CText_SetSize_Original;

typedef void (*CSolidColorLegacyMilBrushProxy_Update_t)(void* pThis, double alpha, struct _D3DCOLORVALUE const *);
CSolidColorLegacyMilBrushProxy_Update_t CSolidColorLegacyMilBrushProxy_Update_Original;

typedef void (*MilInstrumentationCheckHR_MaybeFailFast_t)(unsigned long, long const * const, unsigned int, long, unsigned int, void*);
MilInstrumentationCheckHR_MaybeFailFast_t MilInstrumentationCheckHR_MaybeFailFast_Original;

typedef long (*CCompositor_CreateMilBrushProxy_t)(__int64, long **);
CCompositor_CreateMilBrushProxy_t CCompositor_CreateMilBrushProxy_Original;

typedef long (*CCompositor_CreateMatrixTransformProxy_t)(__int64, char*);
CCompositor_CreateMatrixTransformProxy_t CCompositor_CreateMatrixTransformProxy_Original;

typedef long (*CPushTransformInstruction_Create_t)(__int64*, volatile int**);
CPushTransformInstruction_Create_t CPushTransformInstruction_Create_Original;

typedef long (*CRenderDataVisual_AddInstruction_t)(void* pThis, volatile signed __int32 *);
CRenderDataVisual_AddInstruction_t CRenderDataVisual_AddInstruction_Original;

typedef long (*CBitmapSource_Create_t)(struct IWICBitmap*, const struct _MARGINS*, long long **);
CBitmapSource_Create_t CBitmapSource_Create_Original;

typedef long (*CDrawImageInstruction_Create_t)(long long*, const struct tagRECT*, volatile int **);
CDrawImageInstruction_Create_t CDrawImageInstruction_Create_Original;

typedef long (*CPopInstruction_Create_t)(volatile int **);
CPopInstruction_Create_t CPopInstruction_Create_Original;

typedef void (*CTopLevelWindow_GetAccentBlurBehindBrush_t)(void*, long long**, long long**, void*);
CTopLevelWindow_GetAccentBlurBehindBrush_t CTopLevelWindow_GetAccentBlurBehindBrush_Original;

typedef void (*CTopLevelWindow_OnAccentPolicyUpdated_t)(void*);
CTopLevelWindow_OnAccentPolicyUpdated_t CTopLevelWindow_OnAccentPolicyUpdated_Original;

typedef void (*CText_ReleaseResources_t)(void* pThis);
CText_ReleaseResources_t CText_ReleaseResources_Original;

long long* DesktopManagerInstance = nullptr;

long long (*CButton_DrawStateW_Original)(void* pThis, void* CAtlasButton, UINT ButtonState);

long long CButton_DrawStateW_Hook(void* pThis, void* CAtlasButton, UINT ButtonState) {
    /*Wh_Log(L"We runnin");
    if (ButtonState!=4 && *(DWORD *)((long long)pThis + 328) >= 4u) {
    Wh_Log(L"We runnin but inside");
        long long v10 = 1;
        if ((*(BYTE *)((long long)pThis + 280) & 0x10) == 0) {
            v10 = ButtonState;
        }
        if ( *(DWORD *)((long long)pThis + 360) >= 4u ) {
            Wh_Log(L"We runnin but double inside");
            *(long long *)((long long)pThis + 368) = *(long long *)(*(long long *)((long long)pThis + 336) + 8 * v10);
        }
    }*/

    return CButton_DrawStateW_Original(pThis, CAtlasButton, ButtonState);
}

// void (*CText_SetColor_Original)(void *pThis, unsigned long color);
// 
// void CText_SetColor_Hook(void *pThis, unsigned long color) {
    // color = 0x000000;
    // return CText_SetColor_Original(pThis, color);
// }

long (*CTopLevelWindow_UpdateMarginsDependentOnStyle_Original)(void* pThis);

long CTopLevelWindow_UpdateMarginsDependentOnStyle_Hook(void* pThis) {
    long retvalue = CTopLevelWindow_UpdateMarginsDependentOnStyle_Original(pThis);
    //long retvalue = 0;

    long long CVisualInstance = *((long long *)pThis + 91);

    MARGINS somemargins = *(struct _MARGINS *)(CVisualInstance + 280);
    //Wh_Log(L"%i", *((DWORD *)pThis + 19));

    // DesktopManagerInstance + 60 CONTROLS SOMETHING RELATED TO VISUAL BORDER SIZE
    double v9 = *((double*)*DesktopManagerInstance + 60); // THIS IS HOW YOU USE DESKTOPMANAGERINSTANCE
    //Wh_Log(L"%llu", v9);

    return retvalue;
}

long (*CText_ValidateResources_Original)(void* pThis);

long CText_ValidateResources_Hook(void* pThis) {
    //return 0;
    long retvalue = CText_ValidateResources_Original(pThis);
    //long retvalue = 0;
    /*DWORD v1 = *((DWORD *)pThis + 20);
    if ((v1 & 0x1000)) {
        CText_ReleaseResources_Original(pThis);
        if ( *((unsigned long long *)pThis + 36) ) {
            long long* v10 = (long long*)*((long long*)*DesktopManagerInstance + 33);
            if (v10) {
                HDC hdc = (HDC)*((long long *)v10 + 13);
                if (hdc) {
                    HBITMAP Bitmap = CreateBitmap(512, 30, 1u, 32, 0);
                    if (Bitmap) {
                        SelectObject(hdc, Bitmap);
                        RECT fillbox;
                        fillbox.left = 0;
                        fillbox.right = 512;
                        fillbox.top = 0;
                        fillbox.bottom = 30;
                        HBRUSH Brush = CreateSolidBrush(RGB(3, 48, 124));
                        FillRect(hdc, &fillbox, Brush);
                        GdiFlush();
                        IWICImagingFactory* ImagingFactory = *((IWICImagingFactory**)*DesktopManagerInstance + 39);
                        IWICBitmap* newBitmap;
                        ImagingFactory->CreateBitmapFromHBITMAP(Bitmap, 0, WICBitmapIgnoreAlpha, &newBitmap);
                        WICPixelFormatGUID test1;
                        newBitmap->GetPixelFormat(&test1);
                        Wh_Log(L"%i", test1.Data1);
                        Wh_Log(L"%i", test1.Data2);
                        Wh_Log(L"%i", test1.Data3);
                        Wh_Log(L"%llu", test1.Data4);
                        
                        CCompositor_CreateMatrixTransformProxy_Original(*((long long *)*DesktopManagerInstance + 5), (char *)pThis + 392);
                        volatile int * createdInstruction = 0;
                        CPushTransformInstruction_Create_Original(*((long long **)pThis + 49), &createdInstruction);
                        CRenderDataVisual_AddInstruction_Original(pThis, createdInstruction);

                        long long * CBitmapSource = 0;
                        MARGINS test;
                        test.cxLeftWidth = 8;
                        test.cxRightWidth = 8;
                        test.cyBottomHeight = 8;
                        test.cyTopHeight = 8;
                        CBitmapSource_Create_Original(newBitmap, 0, &CBitmapSource); // CRASHES DWM
                        Wh_Log(L"does this run1?");
                        volatile int* CDrawImageInstruction = 0;
                        CDrawImageInstruction_Create_Original(CBitmapSource, &fillbox, &CDrawImageInstruction);

                        CRenderDataVisual_AddInstruction_Original(pThis, CDrawImageInstruction);
                        volatile int* CPopInst = 0;
                        CPopInstruction_Create_Original(&CPopInst);
                        CRenderDataVisual_AddInstruction_Original(pThis, CPopInst);
                        Wh_Log(L"does this run2?");
                    }
                }
            }
        }
        *((DWORD *)pThis + 20) &= ~0x1000u;
    }*/
    //long retvalue = 0;
    return retvalue;
}

long (*CTopLevelWindow_UpdateColorizationColor_Original)(void* pThis);

long CTopLevelWindow_UpdateColorizationColor_Hook(void* pThis) {
    return CTopLevelWindow_UpdateColorizationColor_Original(pThis);
}

void (*CText_SetBackgroundColor_Original)(void* pThis, unsigned long color);

void CText_SetBackgroundColor_Hook(void* pThis, unsigned long color) {
    return CText_SetBackgroundColor_Original(pThis, color);
}

long (*CTopLevelWindow_UpdateNCAreaBackground_Original)(void* pThis);

long CTopLevelWindow_UpdateNCAreaBackground_Hook(void* pThis) {
    //Wh_Log(L"%i", (int)*((double *)*DesktopManagerInstance + 60));
    //*((double *)*DesktopManagerInstance + 60) = 1;
    /*Wh_Log(L"%i", *((DWORD *)pThis + 154));

    RECT rc = *(struct tagRECT *)(*((long long *)pThis + 91) + 48);
    Wh_Log(L"%i", rc.left);
    Wh_Log(L"%i", rc.right);
    Wh_Log(L"%i", rc.top);
    Wh_Log(L"%i", rc.bottom);*/

    //*((DWORD *)pThis + 157) = -20;

    long retvalue = CTopLevelWindow_UpdateNCAreaBackground_Original(pThis);
    
    float *colorData = (float *)*((long long *)pThis + 73);

    struct _D3DCOLORVALUE color;
    color.a = 0.5;
    color.b = colorData[6];
    color.g = colorData[5];
    color.r = colorData[4];
    
    //Wh_Log(L"%i", *(DWORD *)(*((long long *)pThis + 91) + 152));
    /*float v28[4];
    v28[0] = 1.0;
    v28[1] = 1.0;
    v28[2] = 1.0;
    long long *v24;
    long long *v25;
    v24 = 0;
    v25 = 0;*/
    //CTopLevelWindow_GetAccentBlurBehindBrush_Original(pThis, &v24, &v25, &v28);

    return retvalue;
}

long (*CVisual_SetSize_Original)(PVOID pThis, struct tagSIZE const *);

long CVisual_SetSize_Hook(PVOID pThis, struct tagSIZE* size) {
    return CVisual_SetSize_Original(pThis, size);
}

bool (__thiscall *CButton__UpdateCurrentGlyphOpacity_Original)(void *, bool);
bool __thiscall CButton__UpdateCurrentGlyphOpacity_Hook(void *pThis, bool param_1) {
    *((float *)pThis + 101) = 1.0;
    return CButton__UpdateCurrentGlyphOpacity_Original(pThis, param_1);
}


long (*CTopLevelWindow_UpdateNCAreaButton_Original)(__int64, int, int, int, int*);

long CTopLevelWindow_UpdateNCAreaButton_Hook(__int64 pThis, int buttonId, int height, int offsetTop, int* offsetRight) {
    typedef DWORD CVisual;

    if ( !*(CVisual *)(pThis + 488 + 8 * buttonId) ) { // if this button doesn't exist on the window, return.
        return 0;
    }

    __int64 DPIValue = *(unsigned int *)(*(__int64 *)(pThis + 728) + 324);


    // WINDOWS 7 HEIGHT CALCULATION!!! KEPT HERE FOR FUTURE REFERENCE
    //height = floor((float)(GetSystemMetricsForDpiHook((int)31, (UINT)DPIValue)) * 0.95238096) + 0.5;

    bool isTool;
    int TBHeight;
    int width;
    if ( (*(BYTE *)(pThis + 592) & TYPE_TOOL) != 0 ) {
        isTool = TRUE;
        TBHeight = GetSystemMetricsForDpiHook((int)53, (UINT)DPIValue);
    }
    else {
        isTool = FALSE;
        TBHeight = GetSystemMetricsForDpiHook((int)31, (UINT)DPIValue);
    }

    if (isTool) {
        height = TBHeight;
        width = height;
    }
    else {
        height = floor((float)((float)TBHeight * (vistaMaximize ? 20 : 21) / divisor) + 0.5);
        if (buttonId==ID_CLOSE && (*(DWORD *)(pThis +  592) & 0xB00) != 0 ) {
            width = floor((float)((float)TBHeight * 49 / divisor) + 0.5);
        }
        else if (buttonId==ID_CLOSE && (*(DWORD *)(pThis +  592) & 0xB00) == 0 ) {
            width = floor((float)((float)TBHeight * 49 / divisor) + 0.5);
        }
        else if ((buttonId != ID_MIN || *(long long *)(pThis + 488)) && buttonId) {
            // width = floor((float)((float)TBHeight * 27 / divisor) + 0.5);
            width = floor((float)((float)TBHeight * (vistaMaximize ? 28 : 27) / divisor) + 0.5);
        }
        else {
            width = floor((float)((float)TBHeight * 29 / divisor) + 0.5);
        }
    }
    CVisual *buttonData; 
    buttonData = *(CVisual**)(pThis + 8 * buttonId + 488); // this fetches the address where all the data for the specific button is stored

    struct tagSIZE size;
    size.cx = width;
    size.cy = height;

    CVisual_SetInsetFromParentTop_Original(*(CVisual**)(pThis + 8 * buttonId + 488), offsetTop);

    if (buttonData[33] != *offsetRight) {
        buttonData[33] = *offsetRight;
        CVisual_SetDirtyFlags_Original(buttonData, 2);
        buttonData = *(CVisual**)(pThis + 8 * buttonId + 488);
    }

    long retnvalue = CVisual_SetSize_Original(buttonData, &size);
    if (retnvalue < 0) {
        MilInstrumentationCheckHR_MaybeFailFast_Original(0x14u, 0, 0, retnvalue, 0xC5Cu, 0);
    }
    else {
        *offsetRight += buttonData[30];
    }

    return retnvalue;
}

long (*CTopLevelWindow_UpdateNCAreaPositionsAndSizes_Original)(void* pThis);

long CTopLevelWindow_UpdateNCAreaPositionsAndSizes_Hook(void *pThis) {

    bool isMaximized = *((BYTE *)pThis + 240) & STATE_MAXIMIZED;
    bool isTool = *((BYTE *)pThis + 592) & TYPE_TOOL;

    if (*((long long *)pThis + 60)) {
        DWORD *clientData = (DWORD *)*((long long *)pThis + 68);

        int insetLeft = *((DWORD *)pThis + 149);
        if (clientData[32] != insetLeft) {
            clientData[32] = insetLeft;
            CVisual_SetDirtyFlags_Original(clientData, 2);
        }
        
        int insetTop = *((DWORD *)pThis + 151);
        if (clientData[34] != insetTop) {
            clientData[34] = insetTop;
            CVisual_SetDirtyFlags_Original(clientData, 2);
        }
        
        int insetRight = *((DWORD *)pThis + 150);
        if (clientData[33] != insetRight) {
            clientData[33] = insetRight;
            CVisual_SetDirtyFlags_Original(clientData, 2);
        }
        
        int insetBottom = *((DWORD *)pThis + 152);
        if (clientData[35] != insetBottom) {
            clientData[35] = insetBottom;
            CVisual_SetDirtyFlags_Original(clientData, 2);
        }
        long *unknown = (long *)*((long long *)pThis + 37);
        if (unknown) {
            CVisual_SetInsetFromParent_Original(unknown, (const struct _MARGINS *)(*((long long *)pThis + 68) + 128));
        }
    }

    int insetRightNormal = *((DWORD *)pThis + 150);
    if (insetRightNormal <= 0) {
        // this+91 contains window data
        insetRightNormal = *(long long *)(*((long long *)pThis + 91) + 96);
    }
    long *borderSizeArray;
    if (isMaximized) {
        borderSizeArray = (long *)((char *)pThis + 644);
    }
    else {
        borderSizeArray = (long *)((char *)pThis + 628);
    }

    int insetRight;
    if (insetRightNormal - 2 <= borderSizeArray[1] + 2) {
        insetRight = borderSizeArray[1] + 2;
    }
    else {
        insetRight = insetRightNormal - 2;
    }
    
    int insetLeft = *((DWORD *)pThis + 149);

    __int64 DPIValue = *(unsigned int *)(*((long long *)pThis + 91) + 324);

    int height;
    if (isTool) {
        height = GetSystemMetricsForDpiHook(53, DPIValue);
    }
    else {
        height = floor((float)(GetSystemMetricsForDpiHook((int)31, (UINT)DPIValue)) * 0.95238096) + 0.5;
    }

    int insetTop = borderSizeArray[2];
    if (isTool) {
        if (*((DWORD *)pThis + 151) - height - 4 > insetTop) {
           insetTop = *((DWORD *)pThis + 151) - height - 4; 
        }
        insetRight++;
    }
    else if (isMaximized) {
        insetTop--;
    }
    else {
        insetTop++;
    }

    CTopLevelWindow_UpdateNCAreaButton_Hook((long long)pThis, 3, height, insetTop, &insetRight);
    CTopLevelWindow_UpdateNCAreaButton_Hook((long long)pThis, 2, height, insetTop, &insetRight);
    CTopLevelWindow_UpdateNCAreaButton_Hook((long long)pThis, 1, height, insetTop, &insetRight);
    CTopLevelWindow_UpdateNCAreaButton_Hook((long long)pThis, 0, height, insetTop, &insetRight);

    bool hasIcon = (__int64 *)*((long long *)pThis + 66);
    if (hasIcon) {
        __int64 someData = *((long long *)pThis + 91);
        int width;
        int height;
        if (*(long long *)(someData + 136) || (*((long *)pThis + 148) & 0x10000) == 0) {
            width = GetSystemMetricsForDpiHook(49, *(unsigned int *)(someData + 324));
            height = GetSystemMetricsForDpiHook(50, *(unsigned int *)(*((long long *)pThis + 91) + 324));
        }
        else {
            width = 0;
            height = 0;
        }
        
        long long *iconData;
        iconData = (__int64 *)*((long long *)pThis + 66);

        struct tagSIZE size;
        size.cx = width;
        size.cy = height;
        long retnvalue = CVisual_SetSize_Original(iconData, &size);
        if (retnvalue < 0) {
            MilInstrumentationCheckHR_MaybeFailFast_Original(0x14u, 0, 0, retnvalue, 0xC08u, 0);
            return (unsigned int)retnvalue;
        }

        int insetTop = borderSizeArray[2] + (*((DWORD *)pThis + 151) - *(DWORD *)(*((long long *)pThis + 66) + 124) - borderSizeArray[2]) / 2;

        CVisual_SetInsetFromParentTop_Original(iconData, insetTop);
        CVisual_SetInsetFromParentLeft_Original(iconData, insetLeft);

        long iconWidth = *(long *)(*((long long *)pThis + 66) + 120);

        if (iconWidth > 0) {
            insetLeft += iconWidth + 5;
        }
    }
    bool hasText = (DWORD *)*((long long *)pThis + 65);
    if (hasText) {
        DWORD *textData;
        textData = (DWORD *)*((long long *)pThis + 65);
        CVisual_SetInsetFromParentTop_Original(textData, borderSizeArray[2]);
        CVisual_SetInsetFromParentLeft_Original(textData, insetLeft);
        if ( textData[33] != insetRight ) {
            textData[33] = insetRight;
            CVisual_SetDirtyFlags_Original(textData, 2);
        }
        struct tagSIZE size;
        size.cx = textData[30];
        size.cy = *((DWORD *)pThis + 151) - borderSizeArray[2];
        CText_SetSize_Original(textData, &size);
    }

    CTopLevelWindow_UpdatePinnedParts_Original(pThis);

    return 0;
}

void updateSettings(){
    divisor = Wh_GetIntSetting(L"Divisor"); // Change this to 24 for Vista style
    if(divisor == 0)
        divisor = 21;
    vistaMaximize = Wh_GetIntSetting(L"VistaMaximize");
}

void Wh_ModSettingsChanged(){
    updateSettings();
}

BOOL Wh_ModInit(void) {
	Wh_Log(L"We're back");

    // This is for developers i am not a devleoper i just want to use the mod. Please stop not loading the mod.
    //bool already_running = !(*(USHORT*)((long long)NtCurrentTeb() + 0x17EE) & 0x0400);
    updateSettings();
    
    // if (!already_running) {
        // return FALSE;
    // }

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* GetSystemMetricsForDpiAddr = (void*)GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    Wh_SetFunctionHook(GetSystemMetricsForDpiAddr, (void*)GetSystemMetricsForDpiHook, (void**)&GetSystemMetricsForDpiOrig);

    /* --------------------------- */

	HMODULE uDWM = GetModuleHandle(L"uDWM.dll");
    if (!uDWM) return FALSE;

    WH_FIND_SYMBOL findSymbol;
    HANDLE findSymbolHandle = Wh_FindFirstSymbol(uDWM, nullptr, &findSymbol);
    if (!findSymbolHandle) {
        Wh_Log(L"Wh_FindFirstSymbol failed");
        return FALSE;
    }
    
    void* UpdateCurrentGlyphOpacityAddr = nullptr;
    void* CVisualSetSizeAddr = nullptr;
    void* PositionsAndSizesAddr = nullptr;
    void* UpdateNCAreaButtonAddr = nullptr;
    void* UpdateColorizationColorAddr = nullptr;
    void* UpdateNCAreaBackgroundAddr = nullptr;
    void* CTextSetBackgroundColorAddr = nullptr;
    void* ValidateResourcesAddr = nullptr;
    void* UpdateMarginsDependentOnStyleAddr = nullptr;
    void* SetColorAddr = nullptr;
    void* DrawStateAddr = nullptr;

    do {
        if (_wcsicmp(findSymbol.symbol, L"private: void __cdecl CButton::UpdateCurrentGlyphOpacity(bool)") == 0) {
            UpdateCurrentGlyphOpacityAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: virtual long __cdecl CVisual::SetSize(struct tagSIZE const *)") == 0) {
            CVisualSetSizeAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: long __cdecl CTopLevelWindow::UpdateNCAreaPositionsAndSizes(void)") == 0) {
            PositionsAndSizesAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: long __cdecl CTopLevelWindow::UpdateNCAreaButton(enum CTopLevelWindow::ButtonType,int,int,int *)") == 0) {
            UpdateNCAreaButtonAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: long __cdecl CTopLevelWindow::UpdateColorizationColor(void)") == 0) {
            UpdateColorizationColorAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: long __cdecl CTopLevelWindow::UpdateNCAreaBackground(void)") == 0) {
            UpdateNCAreaBackgroundAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: void __cdecl CText::SetBackgroundColor(unsigned long)") == 0) {
            CTextSetBackgroundColorAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: long __cdecl CText::ValidateResources(void)") == 0) {
            ValidateResourcesAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: bool __cdecl CTopLevelWindow::UpdateMarginsDependentOnStyle(void)") == 0) {
            UpdateMarginsDependentOnStyleAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: void __cdecl CText::SetColor(unsigned long)") == 0) {
            SetColorAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: long __cdecl CButton::DrawStateW(class CAtlasButton *,enum CButton::ButtonStates)") == 0) {
            DrawStateAddr = findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: void __cdecl CTopLevelWindow::OnAccentPolicyUpdated(void)") == 0) {
            CTopLevelWindow_OnAccentPolicyUpdated_Original = (CTopLevelWindow_OnAccentPolicyUpdated_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"void __cdecl MilInstrumentationCheckHR_MaybeFailFast(unsigned long,long const * const,unsigned int,long,unsigned int,void *)") == 0) {
            MilInstrumentationCheckHR_MaybeFailFast_Original = (MilInstrumentationCheckHR_MaybeFailFast_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: void __cdecl CVisual::SetInsetFromParentTop(int)") == 0) {
            CVisual_SetInsetFromParentTop_Original = (CVisual_SetInsetFromParentTop_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: void __cdecl CVisual::SetInsetFromParentLeft(int)") == 0) {
            CVisual_SetInsetFromParentLeft_Original = (CVisual_SetInsetFromParentLeft_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: virtual void __cdecl CVisual::SetDirtyFlags(unsigned long)") == 0) {
            CVisual_SetDirtyFlags_Original = (CVisual_SetDirtyFlags_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: long __cdecl CTopLevelWindow::UpdatePinnedParts(void)") == 0) {
            CTopLevelWindow_UpdatePinnedParts_Original = (CTopLevelWindow_UpdatePinnedParts_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: void __cdecl CVisual::SetInsetFromParent(struct _MARGINS const &)") == 0) {
            CVisual_SetInsetFromParent_Original = (CVisual_SetInsetFromParent_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: virtual long __cdecl CText::SetSize(struct tagSIZE const *)") == 0) {
            CText_SetSize_Original = (CText_SetSize_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: long __cdecl CSolidColorLegacyMilBrushProxy::Update(double,struct _D3DCOLORVALUE const &)") == 0) {
            CSolidColorLegacyMilBrushProxy_Update_Original = (CSolidColorLegacyMilBrushProxy_Update_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"protected: long __cdecl CCompositor::CreateProxy(class CSolidColorLegacyMilBrushProxy * *)") == 0) {
            CCompositor_CreateMilBrushProxy_Original = (CCompositor_CreateMilBrushProxy_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"protected: long __cdecl CCompositor::CreateProxy(class CMatrixTransformProxy * *)") == 0) {
            CCompositor_CreateMatrixTransformProxy_Original = (CCompositor_CreateMatrixTransformProxy_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: static long __cdecl CPushTransformInstruction::Create(class CBaseTransformProxy *,class CPushTransformInstruction * *)") == 0) {
            CPushTransformInstruction_Create_Original = (CPushTransformInstruction_Create_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: long __cdecl CRenderDataVisual::AddInstruction(class CRenderDataInstruction *)") == 0) {
            CRenderDataVisual_AddInstruction_Original = (CRenderDataVisual_AddInstruction_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: static long __cdecl CBitmapSource::Create(struct IWICBitmap *,struct _MARGINS const *,class CBitmapSource * *)") == 0) {
            CBitmapSource_Create_Original = (CBitmapSource_Create_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: static long __cdecl CDrawImageInstruction::Create(class CBitmapSource *,struct tagRECT const *,class CDrawImageInstruction * *)") == 0) {
            CDrawImageInstruction_Create_Original = (CDrawImageInstruction_Create_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: static long __cdecl CPopInstruction::Create(class CPopInstruction * *)") == 0) {
            CPopInstruction_Create_Original = (CPopInstruction_Create_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"public: long __cdecl CTopLevelWindow::GetAccentBlurBehindBrush(class CImageLegacyMilBrushProxy * *,class CCachedVisualImageProxy * *,struct MilPoint3F *)") == 0) {
            CTopLevelWindow_GetAccentBlurBehindBrush_Original = (CTopLevelWindow_GetAccentBlurBehindBrush_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: void __cdecl CText::ReleaseResources(void)") == 0) {
            CText_ReleaseResources_Original = (CText_ReleaseResources_t)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        else if (_wcsicmp(findSymbol.symbol, L"private: static class CDesktopManager * CDesktopManager::s_pDesktopManagerInstance") == 0) {
            DesktopManagerInstance = (long long*)findSymbol.address;
            Wh_Log(L"symbol: %s, Addr: %i", findSymbol.symbol, findSymbol.address);
        }
        /*else {
            Wh_Log(L"symbol: %s", findSymbol.symbol);
        }*/
    } while (Wh_FindNextSymbol(findSymbolHandle, &findSymbol));

    Wh_SetFunctionHook(UpdateCurrentGlyphOpacityAddr, (void*)CButton__UpdateCurrentGlyphOpacity_Hook, (void**)&CButton__UpdateCurrentGlyphOpacity_Original);
    Wh_SetFunctionHook(CVisualSetSizeAddr, (void*)CVisual_SetSize_Hook, (void**)&CVisual_SetSize_Original);
    Wh_SetFunctionHook(PositionsAndSizesAddr, (void*)CTopLevelWindow_UpdateNCAreaPositionsAndSizes_Hook, (void**)&CTopLevelWindow_UpdateNCAreaPositionsAndSizes_Original);
    Wh_SetFunctionHook(UpdateNCAreaButtonAddr, (void*)CTopLevelWindow_UpdateNCAreaButton_Hook, (void**)&CTopLevelWindow_UpdateNCAreaButton_Original);
    Wh_SetFunctionHook(UpdateColorizationColorAddr, (void*)CTopLevelWindow_UpdateColorizationColor_Hook, (void**)&CTopLevelWindow_UpdateColorizationColor_Original);
    Wh_SetFunctionHook(UpdateNCAreaBackgroundAddr, (void*)CTopLevelWindow_UpdateNCAreaBackground_Hook, (void**)&CTopLevelWindow_UpdateNCAreaBackground_Original);
    Wh_SetFunctionHook(CTextSetBackgroundColorAddr, (void*)CText_SetBackgroundColor_Hook, (void**)&CText_SetBackgroundColor_Original);
    Wh_SetFunctionHook(ValidateResourcesAddr, (void*)CText_ValidateResources_Hook, (void**)&CText_ValidateResources_Original);
    Wh_SetFunctionHook(UpdateMarginsDependentOnStyleAddr, (void*)CTopLevelWindow_UpdateMarginsDependentOnStyle_Hook, (void**)&CTopLevelWindow_UpdateMarginsDependentOnStyle_Original);
    // DWMBlurGlass already handles this.
    // Wh_SetFunctionHook(SetColorAddr, (void*)CText_SetColor_Hook, (void**)&CText_SetColor_Original);
    Wh_SetFunctionHook(DrawStateAddr, (void*)CButton_DrawStateW_Hook, (void**)&CButton_DrawStateW_Original);
    Wh_Log(L"It's over");
	return TRUE;
}
