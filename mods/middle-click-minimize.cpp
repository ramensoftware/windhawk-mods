// ==WindhawkMod==
// @id              middle-click-minimize
// @name            Middle Click Minimize
// @description     Minimiza a janela ao clicar com o botão do meio (scroll) na barra de título.
// @version         1.1.2
// @author          Ar4ujo_009
// @github          Ar4ujo009
// @include         *
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- blacklist: "windhawk.exe"
  $name: "Aplicativos Ignorados (Lista Negra)"
  $description: "Nomes dos executáveis separados por vírgula. O mod NÃO funcionará nestes programas. (Exemplo: steam.exe, vivaldi.exe)"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <string>

bool IsProcessBlacklisted() {
    PCWSTR blacklistStr = Wh_GetStringSetting(L"blacklist");
    if (!blacklistStr) return false;

    std::wstring list = blacklistStr;
    Wh_FreeStringSetting(blacklistStr);

    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? (exeName + 1) : exePath;

    size_t pos = 0;
    std::wstring token;
    std::wstring delimiter = L",";
    std::wstring s = list + L",";

    while ((pos = s.find(delimiter)) != std::wstring::npos) {
        token = s.substr(0, pos);
        token.erase(0, token.find_first_not_of(L" \t\r\n"));
        token.erase(token.find_last_not_of(L" \t\r\n") + 1);
        if (!token.empty() && _wcsicmp(exeName, token.c_str()) == 0) {
            return true;
        }
        s.erase(0, pos + delimiter.length());
    }
    return false;
}

typedef LRESULT(WINAPI *DispatchMessageW_t)(const MSG *lpMsg);
DispatchMessageW_t DispatchMessageW_Original;

LRESULT WINAPI DispatchMessageW_Hook(const MSG *lpMsg) {
    if (lpMsg && lpMsg->message == WM_NCMBUTTONDOWN && lpMsg->wParam == HTCAPTION) {
        HWND hRoot = GetAncestor(lpMsg->hwnd, GA_ROOT);
        PostMessage(hRoot, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return 0; 
    }
    return DispatchMessageW_Original(lpMsg);
}

typedef LRESULT(WINAPI *DispatchMessageA_t)(const MSG *lpMsg);
DispatchMessageA_t DispatchMessageA_Original;

LRESULT WINAPI DispatchMessageA_Hook(const MSG *lpMsg) {
    if (lpMsg && lpMsg->message == WM_NCMBUTTONDOWN && lpMsg->wParam == HTCAPTION) {
        HWND hRoot = GetAncestor(lpMsg->hwnd, GA_ROOT);
        PostMessage(hRoot, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return 0;
    }
    return DispatchMessageA_Original(lpMsg);
}

BOOL Wh_ModInit() {
    if (IsProcessBlacklisted()) {
        Wh_Log(L"Processo na blacklist. O mod Middle Click Minimize foi ignorado aqui.");
        return FALSE;
    }

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) return FALSE;

    FARPROC pDispatchMessageW = GetProcAddress(hUser32, "DispatchMessageW");
    if (pDispatchMessageW) {
        Wh_SetFunctionHook((void*)pDispatchMessageW, (void*)DispatchMessageW_Hook, (void**)&DispatchMessageW_Original);
    }

    FARPROC pDispatchMessageA = GetProcAddress(hUser32, "DispatchMessageA");
    if (pDispatchMessageA) {
        Wh_SetFunctionHook((void*)pDispatchMessageA, (void*)DispatchMessageA_Hook, (void**)&DispatchMessageA_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Mod Middle Click Minimize descarregado.");
}
