// ==WindhawkMod==
// @id              taskmgr-link-speed
// @name            Task Manager Show Link Speed
// @name:zh-CN      任务管理器显示链路速度
// @description     Replace DNS names in Task Manager with realtime link speeds because we care more about it.
// @description:zh-CN     在任务管理器网卡页面中显示实际链路速度
// @version         1.0.0
// @author          Joe Ye
// @github          https://github.com/JoeYe-233
// @include         Taskmgr.exe
// @include         Taskmgr10.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Show Link Speed

This mod introduces a simple yet powerful enhancement to the Task Manager's Network tab by replacing the DNS names of network adapters with their real-time link speeds. This allows users to quickly assess the performance of their network connections without needing to open network details and cross-reference adapter names.

Works for both Windows 10 and Windows 11 version of Task Manager, also supports `Windows 10 classic task manager for Windows 11` by Winaero. Supports all types of network adapters (Ethernet, Wi-Fi, etc.) by directly reading the link speed from the system's network interface data structures. By default this mod will use the system's built-in multilingual "Link speed" as the label text in Task Manager. You can also customize the label text through the mod settings.

*Note: this mod needs pdb symbol of `taskmgr.exe` to work. Windhawk will download it automatically when you launch Task Manager first time after you installed the mod (the popup at right bottom corner of your screen, please make sure that it shows percentage like "Loading symbols... 0% (taskmgr.exe)", wait until it reaches 100% and the pop up disappear, otherwise please switch your network and try again) please wait patiently and let Windhawk do the rest*

# Before vs After
![](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/taskmgr-link-speed-before-vs-after.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CustomLabelOverrideEnabled: false
  $name: Enable custom label text
  $name:zh-CN: 启用自定义标签文本
  $description: Default = False, which means using the system's built-in multilingual "Link speed" as the label text in Task Manager. Enable this option to use custom text.
  $description:zh-CN: 图表页面默认使用系统自带的多语言“链接速度”作为项目名称，如需自定义文本可启用该选项
- CustomLabelOverrideText: ""
  $name: Custom label text
  $name:zh-CN: 自定义标签文本
  $description: "Custom label text for Task Manager (example: Link speed)"
  $description:zh-CN: "任务管理器中显示的自定义标签文本（例如: 链路速度）"
*/
// ==/WindhawkModSettings==

// Following the exact order of includes is crucial to prevent conflicts with network data types
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <wchar.h>
#include <string_view>
#include <regex>
#include <windhawk_utils.h>

// ------------------------------------------------
// Global variables and function pointers
// ------------------------------------------------
typedef __int64 (__fastcall *CAdapter_Update_t)(void* pThis, bool* pbRefresh, unsigned long historyIndex);
CAdapter_Update_t Original_CAdapter_Update = nullptr;

// Official GetAdapterStat function
typedef unsigned __int64 (__fastcall *GetAdapterStat_t)(void* pThis, unsigned long arg1, int arg2, int arg3);
GetAdapterStat_t pGetAdapterStat = nullptr;

// UpdateDNSName (used to obtain address for assembly analysis)
typedef void (__fastcall *UpdateDNSName_t)(void* pThis, const unsigned __int16* a2, void* a3);
UpdateDNSName_t Original_CAdapter_UpdateDNSName = nullptr;

// UIProps offset (default 0x1F90)
size_t g_UIPropsOffset = 0x1F90; 

// ------------------------------------------------
// Utility function: use Windhawk Wh_Disasm regex to extract offset
// ------------------------------------------------
size_t OffsetFromAssemblyRegex(void* func, size_t defValue, std::regex regex, int limit = 100) {
    if (!func) return defValue;
    
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret" || s == "retn") {
            break; // Function ends, exit early
        }

        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_search(s.begin(), s.end(), match, regex)) {
            // Parse the first captured group (hex string)
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Regex offset extraction failed for %p. Using default.", func);
    return defValue;
}

// ------------------------------------------------
// Memory safety check helper functions
// ------------------------------------------------
bool IsMemoryAccessible(const void* ptr, size_t size, bool requireWritable) {
    if (!ptr || size == 0) return false;

    static thread_local const void* g_lastCheckedPtr = nullptr;
    static thread_local size_t g_lastCheckedSize = 0;
    static thread_local bool g_lastRequireWritable = false;
    static thread_local bool g_lastCheckResult = false;

    if (ptr == g_lastCheckedPtr &&
        size == g_lastCheckedSize &&
        requireWritable == g_lastRequireWritable) {
        return g_lastCheckResult;
    }

    bool isAccessible = true;

    uintptr_t start = (uintptr_t)ptr;
    uintptr_t end = start + size;
    uintptr_t current = start;
    if (end < start) {
        isAccessible = false;
        goto CACHE_AND_RETURN;
    }

    while (current < end) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery((LPCVOID)current, &mbi, sizeof(mbi))) {
            isAccessible = false;
            break;
        }

        if (mbi.State != MEM_COMMIT) {
            isAccessible = false;
            break;
        }

        DWORD protect = mbi.Protect;
        if (protect & (PAGE_GUARD | PAGE_NOACCESS)) {
            isAccessible = false;
            break;
        }

        bool hasAccess = false;
        if (requireWritable) {
            hasAccess = (protect & (PAGE_READWRITE | PAGE_WRITECOPY |
                                    PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0;
        } else {
            hasAccess = (protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY |
                                    PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                                    PAGE_EXECUTE_WRITECOPY)) != 0;
        }

        if (!hasAccess) {
            isAccessible = false;
            break;
        }

        uintptr_t regionEnd = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
        if (regionEnd <= current) {
            isAccessible = false;
            break;
        }

        current = (regionEnd < end) ? regionEnd : end;
    }

CACHE_AND_RETURN:
    g_lastCheckedPtr = ptr;
    g_lastCheckedSize = size;
    g_lastRequireWritable = requireWritable;
    g_lastCheckResult = isAccessible;
    return isAccessible;
}

bool IsMemoryReadable(const void* ptr, size_t size) {
    return IsMemoryAccessible(ptr, size, false);
}

bool IsMemoryWritable(void* ptr, size_t size) {
    return IsMemoryAccessible(ptr, size, true);
}

// ==========================================
// Additional module: Intercept LoadStringW at the lowest level (perfect internationalization solution)
// ==========================================

typedef int (WINAPI *LoadStringW_t)(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax);
LoadStringW_t Original_LoadStringW_User32 = nullptr;
LoadStringW_t Original_LoadStringW_KernelBase = nullptr;

bool g_CustomLabelEnabled = false;
wchar_t g_CustomLabelText[128] = {0};

void LoadUserSettings() {
    g_CustomLabelEnabled = (Wh_GetIntSetting(L"CustomLabelOverrideEnabled") != 0);
    g_CustomLabelText[0] = L'\0';

    PCWSTR customText = Wh_GetStringSetting(L"CustomLabelOverrideText");
    if (customText) {
        wcsncpy_s(g_CustomLabelText, _countof(g_CustomLabelText), customText, _TRUNCATE);
        Wh_FreeStringSetting(customText);
    }
}

int BuildLabelText(HINSTANCE hInstance, wchar_t* outBuf, size_t outBufCount, LoadStringW_t OriginalFunc) {
    if (g_CustomLabelEnabled && g_CustomLabelText[0] != L'\0') {
        wcsncpy_s(outBuf, outBufCount, g_CustomLabelText, _TRUNCATE);
        return (int)wcslen(outBuf);
    }

    return OriginalFunc(hInstance, 34669, outBuf, (int)outBufCount);
}

// Core replacement logic: extracted into a shared function for both hooks
int ProcessLoadStringHook(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax, LoadStringW_t OriginalFunc) {
    // Target resource ID: 34554 (DNS name)
    if (uID == 34554) {
        wchar_t labelText[128] = {0};
        int labelLen = BuildLabelText(hInstance, labelText, _countof(labelText), OriginalFunc);
        if (labelLen <= 0) {
            return OriginalFunc(hInstance, uID, lpBuffer, cchBufferMax);
        }

        if (cchBufferMax == 0) {
            // [DirectUI zero-copy mode]
            // When cchBufferMax is 0, the API expects a read-only string pointer,
            // and the preceding 16-bit value must store the string length.
            static wchar_t fakeRes[256] = {0};

            size_t maxCopy = _countof(fakeRes) - 3; // Prefix + colon + terminator
            size_t copyLen = (size_t)labelLen;
            if (copyLen > maxCopy) {
                copyLen = maxCopy;
            }

            fakeRes[0] = (wchar_t)(copyLen + 1); // Length prefix: text length + colon
            wcsncpy_s(&fakeRes[1], _countof(fakeRes) - 1, labelText, copyLen);
            fakeRes[copyLen + 1] = L':';
            fakeRes[copyLen + 2] = L'\0';

            // Per API contract, cast and write the pointer into lpBuffer
            *(PWSTR*)lpBuffer = &fakeRes[1];
            return (int)fakeRes[0];
        } else {
            // [Standard copy mode]
            wcsncpy_s(lpBuffer, cchBufferMax, labelText, _TRUNCATE);
            int len = (int)wcslen(lpBuffer);
            if (len > 0 && len + 1 < cchBufferMax) {
                lpBuffer[len] = L':';
                lpBuffer[len + 1] = L'\0';
                return len + 1;
            }
            return len;
        }
    }
    // Pass through all other string resources
    return OriginalFunc(hInstance, uID, lpBuffer, cchBufferMax);
}

// User32 hook
int WINAPI Hooked_LoadStringW_User32(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax) {
    return ProcessLoadStringHook(hInstance, uID, lpBuffer, cchBufferMax, Original_LoadStringW_User32);
}

// KernelBase hook (many Win10/11 internal calls are forwarded here)
int WINAPI Hooked_LoadStringW_KernelBase(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax) {
    return ProcessLoadStringHook(hInstance, uID, lpBuffer, cchBufferMax, Original_LoadStringW_KernelBase);
}

// ------------------------------------------------
// Core logic: Update hook and speed retrieval
// ------------------------------------------------
__int64 __fastcall Hooked_CAdapter_Update(void* pThis, bool* pbRefresh, unsigned long historyIndex) {
    __int64 result = Original_CAdapter_Update(pThis, pbRefresh, historyIndex);
    if (!pThis) return result;

    unsigned int adapterCount = *(unsigned int*)((char*)pThis + 0x10);
    void** pAdapterArray = *(void***)((char*)pThis + 0x8); 

    if (!pAdapterArray || adapterCount == 0 || adapterCount > 100 || !pGetAdapterStat) return result;

    for (unsigned int i = 0; i < adapterCount; i++) {
        char* pAdapterInfoEx = (char*)pAdapterArray[i];
        if (!pAdapterInfoEx) continue;

        // 1. Call the official getter to obtain LinkSpeedBps (cmd = 4)
        ULONG64 linkSpeedBps = pGetAdapterStat(pThis, i, 4, 0);

        if (linkSpeedBps == 0 || linkSpeedBps == (ULONG64)-1) continue;

        wchar_t speedStr[261] = {0};
        double speedGbps = (double)linkSpeedBps / 1000000000.0;
        double speedMbps = (double)linkSpeedBps / 1000000.0;
        
        if (speedGbps >= 1.0) {
            swprintf(speedStr, 261, L"%.1f Gbps", speedGbps);
        } else {
            swprintf(speedStr, 261, L"%.0f Mbps", speedMbps);
        }

        // 2. Write into the UIProps buffer using the dynamic offset
        char** ppUIProps = (char**)(pAdapterInfoEx + g_UIPropsOffset);
        if (!IsMemoryReadable(ppUIProps, sizeof(void*))) continue;
        
        char* pUIProps = *ppUIProps;
        if (!pUIProps) continue;

        wchar_t* pDnsBuffer = (wchar_t*)(pUIProps + 0x30); // Internal string offset 0x30 is highly stable

        if (IsMemoryWritable(pDnsBuffer, 261 * sizeof(wchar_t))) {
            wcscpy_s(pDnsBuffer, 261, speedStr);
        }
    }

    return result;
}

BOOL Wh_ModInit(void) {
    Wh_Log(L">>> Taskmgr Real Link Speed Mod Initializing...");
    LoadUserSettings();
    HMODULE hTaskmgr = GetModuleHandleW(NULL);
    if (!hTaskmgr) return FALSE;

    // Register core hooks and resolve target function symbols
    WindhawkUtils::SYMBOL_HOOK taskmgrExeHooks[] = {
        {
            { L"public: long __cdecl CAdapter::Update(bool *,unsigned long)" },
            (void**)&Original_CAdapter_Update,
            (void*)Hooked_CAdapter_Update,
            false
        },
        {
            { L"public: unsigned __int64 __cdecl CAdapter::GetAdapterStat(unsigned long,enum NETCOLUMNID,int)" },
            (void**)&pGetAdapterStat,
            nullptr,
            false
        },
        {
            // Use a dummy hook to obtain the exact UpdateDNSName pointer
            { L"private: void __cdecl CAdapter::UpdateDNSName(unsigned short const *,struct ADAPTER_INFOEX *)" },
            (void**)&Original_CAdapter_UpdateDNSName,
            nullptr,
            true // Optional; base loading is not affected even if not found
        }
    };

    if (!WindhawkUtils::HookSymbols(hTaskmgr, taskmgrExeHooks, ARRAYSIZE(taskmgrExeHooks))) {
        Wh_Log(L"ERROR: Failed to hook main CAdapter symbols.");
        return FALSE;
    }

    // ------------------------------------------------
    // [ASM pattern extraction] extract 0x1F90
    // ------------------------------------------------
    if (Original_CAdapter_UpdateDNSName) {
        // Match pattern: mov rcx, [rdi + 0x1f90] or similar instructions
        // Require at least 3 hex digits to avoid matching short local offsets like +0x30
        std::regex rx(R"(mov\s+r\w+,\s*(?:qword ptr\s*)?\[r\w+\s*\+\s*0x([1-9a-fA-F][0-9a-fA-F]{2,})\])", std::regex_constants::icase);
        
        // Scan the first 100 instructions (Limit = 100)
        size_t extracted = OffsetFromAssemblyRegex((void*)Original_CAdapter_UpdateDNSName, 0x1F90, rx, 100);
        if (extracted != 0x1F90) {
            g_UIPropsOffset = extracted;
            Wh_Log(L"Dynamically extracted UIProps offset from ASM: 0x%zX", g_UIPropsOffset);
        } else {
            Wh_Log(L"Regex extraction resulted in default: 0x%zX", g_UIPropsOffset);
        }
    }

    // ------------------------------------------------
    // LoadStringW internationalization hook
    // ------------------------------------------------
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pLoadStringW = (void*)GetProcAddress(hUser32, "LoadStringW");
        Wh_SetFunctionHook(pLoadStringW, (void*)Hooked_LoadStringW_User32, (void**)&Original_LoadStringW_User32);
    }

    HMODULE hKernelBase = GetModuleHandleW(L"KernelBase.dll");
    if (hKernelBase) {
        void* pLoadStringW = (void*)GetProcAddress(hKernelBase, "LoadStringW");
        Wh_SetFunctionHook(pLoadStringW, (void*)Hooked_LoadStringW_KernelBase, (void**)&Original_LoadStringW_KernelBase);
    }

    Wh_Log(L"Hook applied successfully! Real speed active.");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"<<< Taskmgr Link Speed Mod Uninitialized");
}

void Wh_ModSettingsChanged(void) {
    LoadUserSettings();
    Wh_Log(L"Settings reloaded: Custom label override updated.");
}