// ==WindhawkMod==
// @id              fix-10-taskbar-notifications
// @name            Fix 11's notifications crash in Win10 taskbar under Win11 24H2+
// @description     Stops notifications from crashing explorer
// @version         0.1
// @author          ilovethisgame
// @github          https://github.com/bozohi
// @include         %ProgramData%\Windhawk\Engine\ModsWritable\LegacyStore\explorer.exe
// @architecture    amd64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod patches the Explorer shell provided by the Win10 taskbar mod at runtime.

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
    struct NOC_REFINED_NOTIFICATION const **iterator,
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

static bool patch_applied { false };
static uint8_t patch_bytes[] { 0xC3, 0x90, 0x90, 0x90, 0x90 }; // ret + 4 nop
static constexpr size_t patch_size { sizeof patch_bytes };

static uint8_t orig_bytes[patch_size] { 0 };
static uint8_t *target;

BOOL Wh_ModInit(void) {
    Wh_Log(L"Initializing explorer patch");

    HMODULE explorer { GetModuleHandleW(NULL) };
    if (!explorer) {
        Wh_Log(L"Failed to get explorer base address");
        return FALSE;
    }

    target = (uint8_t *) explorer + 0x14DC40;
    Wh_Log(L"Target address: 0x%llx", target);

    if (!memcmp((LPVOID) target, (LPVOID) patch_bytes, patch_size)) {
        Wh_Log(L"Explorer was already patched, quitting");
        return TRUE;
    }

    DWORD old_protect {};
    if (!VirtualProtect((LPVOID) target, patch_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        Wh_Log(L"FAILED TO CHANGE MEMORY PROTECTION: %d", GetLastError());
        return FALSE;
    }

    memcpy_s((void *) orig_bytes, patch_size, (const void *) target, patch_size);

    bool failed { false };
    if (!memcpy_s((void *) target, patch_size, patch_bytes, patch_size)) {
        Wh_Log(L"Patch applied successfully");
    } else {
        Wh_Log(L"FAILED TO APPLY PATCH");
        failed = true;
    }

    DWORD temp = 0;
    VirtualProtect((void *) target, patch_size, old_protect, &temp);

    if (failed) {
        return FALSE;
    }

    patch_applied = true;

    FlushInstructionCache(GetCurrentProcess(), (void *) target, patch_size);

    return TRUE;
}

void Wh_ModUninit(void) {
    if (!patch_applied) {
        Wh_Log(L"UNINIT: Patch was not applied, skipping revert");
        return;
    }

    if (orig_bytes[0] == 0x00) {
        Wh_Log(L"UNINIT: Original bytes did not get copied correctly, aborting revert");
        return;
    }

    DWORD old_protect {};
    if (!VirtualProtect((LPVOID) target, patch_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        Wh_Log(L"UNINIT: FAILED TO CHANGE MEMORY PROTECTION: %d", GetLastError());
        return;
    }

    if (!memcpy_s((void *) target, patch_size, orig_bytes, patch_size)) {
        Wh_Log(L"UNINIT: Original bytes rewritten successfully");
    } else {
        Wh_Log(L"UNINIT: FAILED TO REVERT PATCH");
    }

    DWORD temp = 0;
    VirtualProtect((void *) target, patch_size, old_protect, &temp);

    FlushInstructionCache(GetCurrentProcess(), (void *) target, patch_size);
}
