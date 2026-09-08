// ==WindhawkMod==
// @id            office-fix-account-disp-name
// @name          Office Fix Account Display Name
// @name:zh-CN    Office 修复右上角账户名显示
// @description   Fixes the inverted First/Last name and the hardcoded space in Office apps. Supports custom display name override.
// @description:zh-CN 彻底修复 Office 软件右上角账户名中英倒置以及强行插入空格的问题，支持自定义名称
// @version       1.0.0
// @author        Joe Ye
// @github        https://github.com/JoeYe-233
// @include       winword.exe
// @include       excel.exe
// @include       powerpnt.exe
// @include       outlook.exe
// @include       onenote.exe
// @include       visio.exe
// @include       winproj.exe
// @include       mspub.exe
// @include       msaccess.exe
// @compilerOptions -luser32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Office Fix Account Display Name

Mainly for Microsoft Office 2013-2019 (including 2021+ with/without Office UI Reverter Mod)

*Will only affect the UI display name. Author name in document properties or comments will remain unaffected.*

## English

Take full control over your Microsoft Office account display name! This mod allows you to seamlessly override the name shown in the top-right corner of Office applications. You can customize it to any text you prefer, or clear the string completely to hide your name for better privacy. 

- As an added bonus for CJK (Chinese, Japanese, Korean) users, the default option "[auto]" automatically detects and fixes the long-standing issue of inverted names (i.e., fix the problem of showing "Given Name + space + Family Name" instead of the correct "Family Name + Given Name") 

**How it works:** The mod hooks into the `CalculateDisplayName` function within `mso30win32client.dll` to intercept and dynamically modify the generated display name string.

*Note: This mod requires the PDB symbol file for `mso30win32client.dll` to function correctly. The symbol file is somewhat large (~39MB). Windhawk will automatically begin downloading it the first time you launch Word after installation. Look for a popup in the bottom-right corner of your screen showing the download progress (e.g., "Loading symbols... 0% (mso30win32client.dll)"). Please wait patiently until it reaches 100% and disappears. If it hangs, try switching your network. Crucially, make sure to **relaunch Word AS ADMINISTRATOR at least once** after the download finishes. This writes the used symbols into the SymbolCache, significantly speeding up future launch times.*

## ⚠️ Important Notes:
* It is highly recommended to **turn off automatic updates** for your Office applications. Office updates will change the DLL versions, forcing Windhawk to re-download the large PDB files every time.
* Do not forget the mandatory **Administrator relaunch** after the initial symbol download to ensure caching works correctly.

## Before vs After
![Before-After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/office-fix-account-disp-name-before-vs-aft.png)


## 简体中文

主要针对 Microsoft Office 2013-2019（包括 2021 及更高版本，无论是否安装 Office UI Reverter Mod）

*仅会影响 UI 显示名称，文档属性或评论中的作者名称不受影响*

---

受够了 Office 右上角账户名永远显示成“名 姓”（例如“三 张”，中间还硬塞一个空格）吗？这个 Mod 专为彻底解决此痛点而生。

在默认设置“[auto]”下，它会自动检测 CJK（中日韩）字符，完美去除多余空格并将名称规范化为符合直觉的“姓+名”顺序。除此之外，你也可以在设置中强制覆盖显示内容，改成任何你想要的自定义名称，甚至留空来实现彻底隐藏账户名。

**原理解析：**
通过 Hook `mso30win32client.dll` 内部的 `CalculateDisplayName` 函数，在内存级别动态修改显示字符串。

*注意：本 Mod 依赖 `mso30win32client.dll` 的 PDB 符号文件才能生效。该文件体积约 39MB。安装本 Mod 后首次启动 Word 时，Windhawk 会自动开始下载符号。请留意屏幕右下角的弹窗进度提示（如“正在加载... 0% (mso30win32client.dll)”）。请耐心等待进度达到 100% 且弹窗消失（如果卡住请尝试切换网络）。符号下载完成后，**务必以管理员身份重新启动 Word 至少一次**。这一步非常关键，它能将符号写入系统 SymbolCache，从而避免以后每次启动都卡顿。*

## ⚠️ 注意事项：
* 强烈建议**关闭 Office 自动更新**。因为每次 Office 更新都会改变核心 DLL 的版本，导致 Windhawk 需要重新下载庞大的 PDB 符号文件。
* 安装并下载完符号后，请务必**以管理员身份运行一次 Word**，以确保符号缓存成功写入。

## 效果对比
![Before-After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/office-fix-account-disp-name-before-vs-after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CustomNameText: "[auto]"
  $name: Custom display name
  $name:zh-CN: 自定义显示名称
  $description: \"[auto]\" for smart mode (if your MS account has CJK names, will be normalized ([Family Name][Given Name] instead of [Given Name] [Family Name]), if doesn't have CJK names, \"[auto]\" results in MS default redacted name \"Megan Bowen\"). Clear the text to hide. Enter text to override. Anything other than \"[auto]\" will be treated as custom name and directly injected (note that, if the length exceeds current memory capacity, it will be truncated).
  $description:zh-CN: 填入 [auto] 启用智能模式（中文姓名自动规范化显示），输入其他文本覆盖显示名称（注意长度不能超过当前内存容量，否则会被强制截断）为空则隐藏名称
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <atomic>

// =============================================================
// Global state and settings variables
// =============================================================
std::atomic<bool> g_msoHooked{false}; 

// Global variable for the custom setting
char g_CustomNameText[256] = {0}; // Stores UTF-8 data

#ifdef _WIN64
    #define SYM_CalculateDisplayName L"?CalculateDisplayName@AccountInfo@Msoa@@AEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ"
#else
    #define SYM_CalculateDisplayName L"?CalculateDisplayName@AccountInfo@Msoa@@ABE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ"
#endif
// =============================================================
// MSVC std::string (std::basic_string<char>) Memory Layout
// Compatible with MSVC ABI from VS2015 to VS2022+ (ABI Stable)
// https://github.com/microsoft/STL/blob/main/stl/inc/xstring
// =============================================================
struct MSVC_String {
    union {
        char  _Buf[16]; // SSO (Small String Optimization) buffer
        char* _Ptr;     // Pointer to heap allocated string (if size >= 16)
    } _Bx;
    size_t _Mysize;     // Current length of the string
    size_t _Myres;      // Current capacity of the buffer
};
// =============================================================
// Settings loading and encoding conversion
// =============================================================
void LoadUserSettings() {
    g_CustomNameText[0] = '\0';

    PCWSTR customTextW = Wh_GetStringSetting(L"CustomNameText");
    if (customTextW) {
        // Core: convert Windhawk's UTF-16 wide string to UTF-8 for std::string
        WideCharToMultiByte(CP_UTF8, 0, customTextW, -1, g_CustomNameText, sizeof(g_CustomNameText), NULL, NULL);
        Wh_FreeStringSetting(customTextW);
    }
}

// =============================================================
// Hook logic: smart mode and forced override
// =============================================================

typedef void* (__thiscall *CalculateDisplayName_t)(void* pThis, void* pOutStr);
CalculateDisplayName_t pOrig_CalculateDisplayName = nullptr;

void* __thiscall Hook_CalculateDisplayName(void* pThis, void* pOutStr) {
    void* ret = pOrig_CalculateDisplayName(pThis, pOutStr);

    if (!pOutStr) return ret;

    // Conveniently treat the output string as our MSVC_String struct to access its internal buffer and metadata
    MSVC_String* pStr = (MSVC_String*)pOutStr;

    // Determine the actual string data pointer based on whether it's using SSO or heap allocation
    char* strData = (pStr->_Myres < 16) ? pStr->_Bx._Buf : pStr->_Bx._Ptr;
    size_t len = pStr->_Mysize;
    size_t cap = pStr->_Myres;

    if (strData == nullptr) return ret;

    // =====================================================
    // Branch 1: settings are completely cleared, stealth mode (use a regular space as placeholder)
    // =====================================================
    if (g_CustomNameText[0] == '\0') {
        const char spaceStr[] = " ";
        size_t spaceLen = 1;
        
        if (spaceLen <= cap) { // In theory, a short string is always within capacity
            memcpy(strData, spaceStr, spaceLen);
            strData[spaceLen] = '\0';
            pStr->_Mysize = spaceLen;
        }
        return ret; 
    }

    // =====================================================
    // Branch 2: user entered a custom name (non-empty and not [auto])
    // =====================================================
    if (strcmp(g_CustomNameText, "[auto]") != 0) {
        size_t customLen = strlen(g_CustomNameText);
        
        // Safety check: if the custom name exceeds the current string object's capacity, truncate it
        if (customLen > cap) {
            customLen = cap; 
            Wh_Log(L"[Warning] Custom name truncated to fit current memory capacity!");
        }

        memcpy(strData, g_CustomNameText, customLen);
        strData[customLen] = '\0';
        pStr->_Mysize = customLen;

        return ret;
    }

    // =====================================================
    // Branch 3: keep the default [auto] and enable smart mode (CJK detection)
    // =====================================================
    if (len > 0) {
        bool isCJK = false;
        wchar_t wbuf[256];
        int wlen = MultiByteToWideChar(CP_UTF8, 0, strData, (int)len, wbuf, 256);
        
        // Broadly detect the East Asian character range (U+2E80 to U+9FFF)
        for (int i = 0; i < wlen; ++i) {
            if (wbuf[i] >= 0x2E80 && wbuf[i] <= 0x9FFF) {
                isCJK = true;
                break;
            }
        }

        if (isCJK) {
            // CJK characters detected: perform "space removal + name swapping"
            for (size_t i = 0; i < len; ++i) {
                if (strData[i] == ' ') {
                    size_t givenNameLen = i;
                    size_t familyNameLen = len - i - 1;

                    char temp[256];
                    if (len < sizeof(temp)) {
                        memcpy(temp, strData + i + 1, familyNameLen);        // Family name
                        memcpy(temp + familyNameLen, strData, givenNameLen); // Given name
                        temp[familyNameLen + givenNameLen] = '\0';

                        memcpy(strData, temp, familyNameLen + givenNameLen + 1);
                        pStr->_Mysize = familyNameLen + givenNameLen;
                        
                        // Wh_Log(L"[Fix] CJK name swapped and space removed!");
                    }
                    break;
                }
            }
        } else {
            const char* defaultName = "Megan Bowen";
            size_t defLen = strlen(defaultName);
            
            if (defLen > cap) defLen = cap; 
            
            memcpy(strData, defaultName, defLen);
            strData[defLen] = '\0';
            pStr->_Mysize = defLen;
            
            // Wh_Log(L"[Fix] Non-CJK name replaced with default value!");
        }
    }

    return ret;
}

// =============================================================
// Symbol hooking loader
// =============================================================
void ScanAndHookMso() {
    if (g_msoHooked) return;

    HMODULE hMsoClient = GetModuleHandleW(L"mso30win32client.dll");
    if (!hMsoClient) return;
    
    //mso30win32client.dll
    WindhawkUtils::SYMBOL_HOOK msoHook[] = {
        {
            { SYM_CalculateDisplayName },
            (void**)&pOrig_CalculateDisplayName,
            (void*)Hook_CalculateDisplayName, 
            false
        }
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {0};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE; 
    options.onlineCacheUrl = L"";

    Wh_Log(L"[Init] Attempting to hook CalculateDisplayName...");
    if (WindhawkUtils::HookSymbols(hMsoClient, msoHook, ARRAYSIZE(msoHook), &options)) {
        Wh_ApplyHookOperations();
        Wh_Log(L"[Success] CalculateDisplayName symbols resolved and hooked successfully.");
        g_msoHooked = true; 
    } else {
        Wh_Log(L"[Error] Failed to resolve symbols.");
    }
}

// =============================================================
// Late-load catching
// =============================================================
typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *LoadLibraryW_t)(LPCWSTR);
LoadLibraryExW_t pOrig_LoadLibraryExW = nullptr;
LoadLibraryW_t pOrig_LoadLibraryW = nullptr;

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hMod = pOrig_LoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hMod && lpLibFileName && 
       StrStrIW(lpLibFileName, L"mso30win32client.dll")) {
        ScanAndHookMso();
    }
    return hMod;
}

HMODULE WINAPI Hook_LoadLibraryW(LPCWSTR lpLibFileName) {
    HMODULE hMod = pOrig_LoadLibraryW(lpLibFileName);
    if (hMod && lpLibFileName && 
       StrStrIW(lpLibFileName, L"mso30win32client.dll")) {
        ScanAndHookMso();
    }
    return hMod;
}

// =============================================================
// Lifecycle
// =============================================================
BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Office Fix Display Name loaded. Initializing...");
    
    // Load user settings once during initialization
    LoadUserSettings();

    HMODULE hKernel = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernel) hKernel = GetModuleHandleW(L"kernel32.dll");

    if (hKernel) {
        void* pLoadLibraryExW = (void*)GetProcAddress(hKernel, "LoadLibraryExW");
        if (pLoadLibraryExW) Wh_SetFunctionHook(pLoadLibraryExW, (void*)Hook_LoadLibraryExW, (void**)&pOrig_LoadLibraryExW);

        void* pLoadLibraryW = (void*)GetProcAddress(hKernel, "LoadLibraryW");
        if (pLoadLibraryW) Wh_SetFunctionHook(pLoadLibraryW, (void*)Hook_LoadLibraryW, (void**)&pOrig_LoadLibraryW);
    }

    ScanAndHookMso();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[Exit] Office Fix Display Name Unloaded.");
}

// React dynamically to changes in the settings panel
void Wh_ModSettingsChanged() {
    LoadUserSettings();
    Wh_Log(L"[Settings] Custom name settings reloaded.");
}