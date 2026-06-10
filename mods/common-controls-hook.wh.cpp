// ==WindhawkMod==
// @id              common-controls-hook
// @name            Common Controls Hook
// @description     为旧版 Win32 应用程序强制启用 Common Controls v6 视觉样式。完美移植自 LuicdLabs/comctl32v6hook。
// @version         1.0.0
// @author          LucidLabs
// @github          https://github.com/LuicdLabs
// @include         *
// @exclude         conhost.exe
// @exclude         cmd.exe
// @exclude         powershell.exe
// @exclude         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

#include <windows.h>
#include <commctrl.h>

HANDLE g_hActCtx = INVALID_HANDLE_VALUE;

// 动态创建 v6 激活上下文（为了干净环保，写入临时文件后会立即删除）
bool InitActCtx() {
    WCHAR tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    
    WCHAR manifestPath[MAX_PATH];
    lstrcpyW(manifestPath, tempPath);
    WCHAR pidStr[32];
    wsprintfW(pidStr, L"wh_v6_%u.manifest", GetCurrentProcessId());
    lstrcatW(manifestPath, pidStr);

    // 生成清单文件
    HANDLE hFile = CreateFileW(manifestPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        const char* manifestContent = 
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
            "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\n"
            "  <dependency>\n"
            "    <dependentAssembly>\n"
            "      <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" processorArchitecture=\"*\" publicKeyToken=\"6595b64144ccf1df\" language=\"*\" />\n"
            "    </dependentAssembly>\n"
            "  </dependency>\n"
            "</assembly>\n";
        DWORD written;
        WriteFile(hFile, manifestContent, lstrlenA(manifestContent), &written, NULL);
        CloseHandle(hFile);
    }

    ACTCTXW actCtx = { sizeof(ACTCTXW) };
    actCtx.lpSource = manifestPath;
    g_hActCtx = CreateActCtxW(&actCtx);
    
    // CreateActCtxW 读取完成后会自动载入内存，原文件可安全删除
    DeleteFileW(manifestPath);

    return g_hActCtx != INVALID_HANDLE_VALUE;
}

// RAII: 自动激活和恢复上下文环境
class ActCtxGuard {
    ULONG_PTR cookie;
    BOOL activated;
public:
    ActCtxGuard() {
        activated = FALSE;
        if (g_hActCtx != INVALID_HANDLE_VALUE) {
            activated = ActivateActCtx(g_hActCtx, &cookie);
        }
    }
    ~ActCtxGuard() {
        if (activated) {
            DeactivateActCtx(0, cookie);
        }
    }
};

// ==========================================
// User32 API Hook 定义 (对应原项目的替换逻辑)
// ==========================================

#define DEFINE_HOOK(RET_TYPE, NAME, ARGS, ARGS_PASS) \
    typedef RET_TYPE (WINAPI *NAME##_t) ARGS; \
    NAME##_t NAME##_orig; \
    RET_TYPE WINAPI NAME##_hook ARGS { \
        ActCtxGuard guard; \
        return NAME##_orig ARGS_PASS; \
    }

// 1. CreateWindowEx
DEFINE_HOOK(HWND, CreateWindowExW, 
    (DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam),
    (dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))
DEFINE_HOOK(HWND, CreateWindowExA, 
    (DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam),
    (dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))

// 2. DialogBoxParam
DEFINE_HOOK(INT_PTR, DialogBoxParamW, 
    (HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(INT_PTR, DialogBoxParamA, 
    (HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))

// 3. DialogBoxIndirectParam
DEFINE_HOOK(INT_PTR, DialogBoxIndirectParamW, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEW hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(INT_PTR, DialogBoxIndirectParamA, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEA hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))

// 4. CreateDialogParam
DEFINE_HOOK(HWND, CreateDialogParamW, 
    (HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(HWND, CreateDialogParamA, 
    (HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam))

// 5. CreateDialogIndirectParam
DEFINE_HOOK(HWND, CreateDialogIndirectParamW, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEW hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))
DEFINE_HOOK(HWND, CreateDialogIndirectParamA, 
    (HINSTANCE hInstance, LPCDLGTEMPLATEA hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam),
    (hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam))

// 6. MessageBox
DEFINE_HOOK(int, MessageBoxW, 
    (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType),
    (hWnd, lpText, lpCaption, uType))
DEFINE_HOOK(int, MessageBoxA, 
    (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType),
    (hWnd, lpText, lpCaption, uType))

// 7. MessageBoxEx
DEFINE_HOOK(int, MessageBoxExW, 
    (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId),
    (hWnd, lpText, lpCaption, uType, wLanguageId))
DEFINE_HOOK(int, MessageBoxExA, 
    (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId),
    (hWnd, lpText, lpCaption, uType, wLanguageId))

// 8. MessageBoxIndirect
DEFINE_HOOK(int, MessageBoxIndirectW, 
    (const MSGBOXPARAMSW* lpmbp), (lpmbp))
DEFINE_HOOK(int, MessageBoxIndirectA, 
    (const MSGBOXPARAMSA* lpmbp), (lpmbp))

// 9. MessageBoxTimeout (隐藏 API)
DEFINE_HOOK(int, MessageBoxTimeoutW, 
    (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds),
    (hWnd, lpText, lpCaption, uType, wLanguageId, dwMilliseconds))
DEFINE_HOOK(int, MessageBoxTimeoutA, 
    (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds),
    (hWnd, lpText, lpCaption, uType, wLanguageId, dwMilliseconds))

// ==========================================
// 模组初始化入口
// ==========================================
BOOL Wh_ModInit() {
    // 寻找 user32.dll，如果不包含该模块大概率是纯后台/命令行进程，无需注入
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        return FALSE;
    }

    // 初始化 v6 激活上下文
    if (!InitActCtx()) {
        Wh_Log(L"Failed to create activation context");
        return FALSE;
    }

    // 批量注册 Hook
    #define INSTALL_HOOK(name) \
        do { \
            void* addr = (void*)GetProcAddress(hUser32, #name); \
            if (addr) Wh_SetFunctionHook(addr, (void*)name##_hook, (void**)&name##_orig); \
        } while(0)

    INSTALL_HOOK(CreateWindowExW);
    INSTALL_HOOK(CreateWindowExA);
    INSTALL_HOOK(DialogBoxParamW);
    INSTALL_HOOK(DialogBoxParamA);
    INSTALL_HOOK(DialogBoxIndirectParamW);
    INSTALL_HOOK(DialogBoxIndirectParamA);
    INSTALL_HOOK(CreateDialogParamW);
    INSTALL_HOOK(CreateDialogParamA);
    INSTALL_HOOK(CreateDialogIndirectParamW);
    INSTALL_HOOK(CreateDialogIndirectParamA);
    INSTALL_HOOK(MessageBoxW);
    INSTALL_HOOK(MessageBoxA);
    INSTALL_HOOK(MessageBoxExW);
    INSTALL_HOOK(MessageBoxExA);
    INSTALL_HOOK(MessageBoxIndirectW);
    INSTALL_HOOK(MessageBoxIndirectA);
    INSTALL_HOOK(MessageBoxTimeoutW);
    INSTALL_HOOK(MessageBoxTimeoutA);

    return TRUE;
}