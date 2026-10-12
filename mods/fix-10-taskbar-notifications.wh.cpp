// ==WindhawkMod==
// @id              fix-10-taskbar-notifications
// @name            Fix 11's notifications crash in Win10 taskbar under Win11 24H2+
// @description     Stops notifications from crashing explorer
// @version         0.1
// @author          ilovethisgame
// @github          https://github.com/bozohi
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod hooks the function responsible for crashing the Explorer shell provided by the Win10 taskbar mod at runtime.

But it is not confirmed if notifications arrive to the Notification Center.
Thus, all this mod does is let notifications render without crashing Win10 Explorer but not hand them over to it (probably for the best.)

**!Important! You MUST install, follow the instructions and run at least 2 times the [Win10 taskbar mod.](https://windhawk.net/mods/win10-taskbar-on-win11-24h2)**
**If you don't follow these instructions carefully, the mod will not apply correctly.**

# Technical details

When a notification is rendered, it invokes from explorer a method called 
```cpp
NotificationDataSink::NotificationsAdded(
    NotificationDataSink *this,
    int32_t _arg2,
    struct NOC_REFINED_NOTIFICATION *const *iterator,
    uint32_t length
);
```
.

**Given I haven't experienced any destabilization from explorer thus far, I claim that this has something to do with the Notification Center.**

I won't go too deep into details of the function, but the crash is observed as a `NULL` pointer exception when it tries to access `*(iterator + 0x88)` for the first time, which is probably where the actual elements are.

Normally, it would use that pointer to add 0x150 in a do-while loop until the notification thing elements are exhausted, but instead it's just `NULL`.
I haven't given a look at the iterator memory to see if it's more than just the first element that's `NULL`, or more or all of them are such.

There is no `NULL` check in that loop, so it is likely that the iterator was never supposed to have `NULL` elements in the first place.
*/
// ==/WindhawkModReadme==

#include<cstdint>
#include<windhawk_api.h>

using NotificationsAdded_t = HRESULT (*)(void *pthis, uint32_t arg2, void* const* iterator, uint32_t length);

static HRESULT NotificationsAdded_hook(void *pthis, uint32_t arg2, void* const* iterator, uint32_t length) {
    Wh_Log(L"Suppressing %u notification(s)", length);

    return S_OK;
}

static uint8_t *fn_ptr;

extern BOOL Wh_ModInit(void) {
    Wh_Log(L"Initializing explorer function hook");

    HMODULE explorer { GetModuleHandleW(NULL) };
    if (!explorer) {
        Wh_Log(L"Failed to get explorer base address");
        
        return FALSE;
    }

    auto dos_header = (PIMAGE_DOS_HEADER) explorer;
    auto nt_headers = (PIMAGE_NT_HEADERS64) ((uint8_t *) explorer + dos_header->e_lfanew);

    if (nt_headers->FileHeader.TimeDateStamp != 0x7AC6EEC3 ||
        nt_headers->OptionalHeader.SizeOfImage != 0x442000)
    {
        Wh_Log(L"Unexpected explorer build, not hooking");

        return FALSE;
    }

    fn_ptr = (uint8_t *) explorer + 0x14DC40;
    Wh_Log(L"NotificationsAdded address: 0x%p", fn_ptr);

    if (!Wh_SetFunctionHook(fn_ptr, (void *) NotificationsAdded_hook, nullptr)) {
        Wh_Log(L"Error occurred during hooking");
        
        return FALSE;
    }

    return TRUE;
}
