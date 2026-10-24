// ==WindhawkMod==
// @id              hide-tray-icons-by-process
// @name            Hide Tray Icons by Process
// @name:ru-RU      Скрытие значков трея по процессам
// @description     Hides notification area (tray) icons that belong to processes from a user-defined list
// @description:ru-RU Скрывает значки в области уведомлений, принадлежащие процессам из заданного списка
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -luser32 -lkernel32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Tray Icons by Process / Скрытие значков трея по процессам

This mod hides icons of processes from a list given in settings. The icon gets hidden when it updates its status.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- processes:
  - example.exe
  $name: Processes whose tray icons are hidden
  $name:ru-RU: Процессы, чьи значки в трее нужно скрыть
  $description: One entry per process - file name (app.exe, case-insensitive) or full path.
  $description:ru-RU: По одной записи на процесс — имя файла (app.exe, без учёта регистра) или полный путь.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
 
#include <windows.h>
#include <commctrl.h>
 
#include <algorithm>
#include <atomic>
#include <string>
#include <vector>
 
// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
 
struct ProcessEntry {
    std::wstring value;   // в нижнем регистре
    bool isPath;          // содержит '\'
};
 
static std::vector<ProcessEntry> g_entries;
static SRWLOCK g_lock = SRWLOCK_INIT;
 
static std::wstring ToLower(std::wstring s) {
    if (!s.empty()) {
        CharLowerBuffW(s.data(), (DWORD)s.size());
    }
    return s;
}
 
static std::wstring Trim(const std::wstring& s) {
    size_t b = 0, e = s.size();
    while (b < e && (iswspace(s[b]) || s[b] == L'"')) b++;
    while (e > b && (iswspace(s[e - 1]) || s[e - 1] == L'"')) e--;
    return s.substr(b, e - b);
}
 
static void LoadSettings() {
    std::vector<ProcessEntry> entries;
 
    for (int i = 0; i < 1000; i++) {
        PCWSTR raw = Wh_GetStringSetting(L"processes[%d]", i);
        if (!raw) break;
        std::wstring value = Trim(raw);
        Wh_FreeStringSetting(raw);
 
        if (value.empty()) break;  // конец списка
 
        ProcessEntry entry;
        entry.value = ToLower(value);
        entry.isPath = entry.value.find(L'\\') != std::wstring::npos;
        entries.push_back(std::move(entry));
    }
 
    AcquireSRWLockExclusive(&g_lock);
    g_entries = std::move(entries);
    ReleaseSRWLockExclusive(&g_lock);
}
 
// ---------------------------------------------------------------------------
// Определение процесса-владельца значка
// ---------------------------------------------------------------------------
 
static bool GetProcessImagePath(DWORD pid, std::wstring& path) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
 
    WCHAR buffer[MAX_PATH * 4];
    DWORD size = ARRAYSIZE(buffer);
    BOOL ok = QueryFullProcessImageNameW(process, 0, buffer, &size);
    CloseHandle(process);
 
    if (!ok) {
        return false;
    }
 
    path.assign(buffer, size);
    return true;
}
 
static bool EntryMatches(const ProcessEntry& entry,
                         const std::wstring& fullPathLower,
                         const std::wstring& fileNameLower) {
    if (entry.isPath) {
        return entry.value == fullPathLower;
    }
 
    if (entry.value == fileNameLower) {
        return true;
    }
 
    // "app" == "app.exe"
    if (fileNameLower.size() > 4 &&
        fileNameLower.compare(fileNameLower.size() - 4, 4, L".exe") == 0 &&
        fileNameLower.compare(0, fileNameLower.size() - 4, entry.value) == 0) {
        return true;
    }
 
    return false;
}
 
static bool ShouldHideIconOfWindow(HWND ownerWindow, const wchar_t* operation) {
    if (!ownerWindow || !IsWindow(ownerWindow)) {
        return false;
    }
 
    DWORD pid = 0;
    GetWindowThreadProcessId(ownerWindow, &pid);
    if (!pid) {
        return false;
    }
 
    std::wstring path;
    if (!GetProcessImagePath(pid, path)) {
        return false;
    }
 
    std::wstring pathLower = ToLower(path);
    size_t slash = pathLower.find_last_of(L'\\');
    std::wstring nameLower =
        slash == std::wstring::npos ? pathLower : pathLower.substr(slash + 1);
 
    bool hide = false;
 
    AcquireSRWLockShared(&g_lock);
    for (const auto& entry : g_entries) {
        if (EntryMatches(entry, pathLower, nameLower)) {
            hide = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_lock);
 
    Wh_Log(L"%s from %s (pid %u) -> %s", operation, path.c_str(), pid,
           hide ? L"HIDDEN" : L"shown");
 
    return hide;
}
 
// ---------------------------------------------------------------------------
// Сабклассинг Shell_TrayWnd и перехват WM_COPYDATA
// ---------------------------------------------------------------------------
//
// Shell_NotifyIcon отправляет в Shell_TrayWnd сообщение WM_COPYDATA:
//   COPYDATASTRUCT::dwData == 1
//   COPYDATASTRUCT::lpData -> {
//       DWORD dwSignature;   // 0x34753423
//       DWORD dwMessage;     // NIM_ADD / NIM_MODIFY / NIM_DELETE / ...
//       struct {             // NOTIFYICONDATA с 32-битными значениями дескрипторов
//           DWORD cbSize;
//           DWORD hWnd;
//           DWORD uID;
//           ...
//       } nid;
//   }
 
constexpr ULONG_PTR kTrayCopyDataId = 1;
constexpr DWORD kTraySignature = 0x34753423;
constexpr size_t kOffsetMessage = 4;
constexpr size_t kOffsetHwnd = 12;
constexpr size_t kMinDataSize = 24;
 
static std::atomic<HWND> g_trayWnd;
 
static LRESULT CALLBACK TrayWndSubclassProc(HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR dwRefData) {
    if (uMsg == WM_COPYDATA && lParam) {
        const COPYDATASTRUCT* cds = (const COPYDATASTRUCT*)lParam;
 
        if (cds->dwData == kTrayCopyDataId && cds->lpData &&
            cds->cbData >= kMinDataSize) {
            const BYTE* data = (const BYTE*)cds->lpData;
 
            DWORD signature = *(const DWORD*)data;
            DWORD nimMessage = *(const DWORD*)(data + kOffsetMessage);
 
            if (signature == kTraySignature &&
                (nimMessage == NIM_ADD || nimMessage == NIM_MODIFY)) {
                // 32-битный дескриптор -> HWND (с расширением знака)
                HWND owner =
                    (HWND)(LONG_PTR)(LONG) * (const DWORD*)(data + kOffsetHwnd);
 
                const wchar_t* opName =
                    nimMessage == NIM_ADD ? L"NIM_ADD" : L"NIM_MODIFY";
 
                if (ShouldHideIconOfWindow(owner, opName)) {
                    if (nimMessage == NIM_MODIFY) {
                        // Если значок уже был добавлен до включения мода —
                        // удаляем его, превратив MODIFY в DELETE.
                        std::vector<BYTE> copy(data, data + cds->cbData);
                        *(DWORD*)(copy.data() + kOffsetMessage) = NIM_DELETE;
 
                        COPYDATASTRUCT cds2 = *cds;
                        cds2.lpData = copy.data();
                        DefSubclassProc(hWnd, uMsg, wParam, (LPARAM)&cds2);
                    }
 
                    // Для приложения всё выглядит успешным.
                    return TRUE;
                }
            }
        }
    }
 
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
 
static void SubclassTrayWindow(HWND hWnd) {
    // @include explorer.exe также загружает мод в отдельные процессы
    // Проводника, где FindWindowW вернёт окно из процесса оболочки.
    DWORD pid = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &pid) ||
        pid != GetCurrentProcessId()) {
        return;
    }
 
    HWND expected = nullptr;
    if (!g_trayWnd.compare_exchange_strong(expected, hWnd)) {
        return;
    }
 
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, TrayWndSubclassProc, 0)) {
        Wh_Log(L"Subclassed Shell_TrayWnd");
    } else {
        Wh_Log(L"Failed to subclass Shell_TrayWnd");
        g_trayWnd = nullptr;
    }
}
 
// Окно Shell_TrayWnd может появиться уже после загрузки мода
// (например, при старте Проводника), поэтому следим за CreateWindowExW.
using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;
 
static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                        LPCWSTR lpClassName,
                                        LPCWSTR lpWindowName,
                                        DWORD dwStyle,
                                        int X,
                                        int Y,
                                        int nWidth,
                                        int nHeight,
                                        HWND hWndParent,
                                        HMENU hMenu,
                                        HINSTANCE hInstance,
                                        LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
 
    if (hWnd && !g_trayWnd.load() &&
        ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xFFFF) &&  // не атом
        _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        SubclassTrayWindow(hWnd);
    }
 
    return hWnd;
}
 
// ---------------------------------------------------------------------------
// Точки входа Windhawk
// ---------------------------------------------------------------------------
 
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
 
    LoadSettings();
 
    // Окно уже существует (мод включили в работающем Проводнике).
    SubclassTrayWindow(FindWindowW(L"Shell_TrayWnd", nullptr));
 
    // Окно ещё не создано (мод загружен при старте Проводника).
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);
 
    return TRUE;
}
 
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
 
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
 
    if (HWND hWnd = g_trayWnd.exchange(nullptr)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                         TrayWndSubclassProc);
    }
}
 
