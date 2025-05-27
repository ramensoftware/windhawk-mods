// ==WindhawkMod==
// @id				no-windows-ink-tooltips
// @name			Disable Windows Ink modifier tooltips
// @description		Stops DWM from rendering Windows Ink key modifier tooltips
// @version			1.0.0
// @author			wily-coyote
// @include			dwm.exe
// @github			https://github.com/wily-coyote
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod removes the tooltip that appears next to the cursor when any of the
modifier keys are pressed while using a Windows Ink tablet by preventing the
tooltip window from being created and updated.

![Comparison](https://i.imgur.com/zT6C8o3.png)

This mod requires injecting into DWM, which is a critical system process.
To use it, you should allow Windhawk to inject to DWM by adding `dwm.exe`
to the process inclusion list in the program's advanced settings.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>

long __cdecl (*CTrackingTooltip_Initialize_orig)(void *pThis, const tagPOINT* point, unsigned short* unk);
long __cdecl CTrackingTooltip_Initialize_hook(void *pThis, const tagPOINT* point, unsigned short* unk){
	return 0;
}

void __cdecl (*CTrackingTooltip_Update_orig)(void *pThis, const tagPOINT* point, unsigned short* unk);
void __cdecl CTrackingTooltip_Update_hook(void *pThis, const tagPOINT* point, unsigned short* unk){
	return;
}

void __cdecl (*CTrackingTooltip_UpdateTooltipLocation_orig)(void *pThis);
void __cdecl CTrackingTooltip_UpdateTooltipLocation_hook(void *pThis){
	return;
}

const WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
	{
		{
			L"public: long __cdecl CTrackingTooltip::Initialize(struct tagPOINT const *,unsigned short *)"
		},
		(void**)&CTrackingTooltip_Initialize_orig,
		(void*)CTrackingTooltip_Initialize_hook,
		false
	},
	{
		{
			L"public: void __cdecl CTrackingTooltip::Update(struct tagPOINT const *,unsigned short *)"
		},
		(void**)&CTrackingTooltip_Update_orig,
		(void*)CTrackingTooltip_Update_hook,
		false
	},
	{
		{
			L"protected: void __cdecl CTrackingTooltip::UpdateTooltipLocation(void)"
		},
		(void**)&CTrackingTooltip_UpdateTooltipLocation_orig,
		(void*)CTrackingTooltip_UpdateTooltipLocation_hook,
		false
	}
};

BOOL Wh_ModInit() {
	if(!WindhawkUtils::HookSymbols(GetModuleHandleW(L"udwm.dll"), udwmDllHooks, ARRAYSIZE(udwmDllHooks))){
		Wh_Log(L"Cannot hook uDWM.dll");
		return FALSE;
	}
	return TRUE;
}

void Wh_ModUninit() { }

void Wh_ModSettingsChanged() { }
