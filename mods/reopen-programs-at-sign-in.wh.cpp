// ==WindhawkMod==
// @id              reopen-programs-at-sign-in
// @name            Reopen Programs at Sign-in
// @description     Reopens the programs that were open before shutdown/restart the next time you sign in
// @version         2.0.1
// @author          lima26x
// @github          https://github.com/brdantas26
// @include         explorer.exe
// @compilerOptions -ladvapi32 -ldwmapi -lsecur32 -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Reopen Programs at Sign-in

While the mod is enabled, it keeps track of which programs have a window open.
The next time you sign in to Windows, those programs are opened again. To turn
the feature off, disable the mod.

- Only programs are reopened (one every 2 seconds, starting 15 seconds after
  sign-in by default). Documents/tabs depend on each program.
- Programs running as administrator and Windows components (File Explorer,
  Control Panel...) are not reopened.
- Programs that already have a window open (e.g. started by Startup) are not
  opened twice.
- Windhawk's own programs are never reopened.
- Programs whose window belongs to a helper process (e.g. Steam, Chromium
  apps) may not come back as they were.
- Nothing is reopened if the mod is enabled long after signing in, or if
  Explorer restarts during the day: only a fresh sign-in triggers it.
- Each Windows account keeps its own list.

Windows has a similar built-in option (Settings → Accounts → Sign-in options →
"Automatically save my restartable apps and restart them when I sign back in").
It only restores apps that support it (e.g. Office, browsers); this mod reopens
any program, using its executable path.

---

# Reabrir programas ao entrar (Português)

Enquanto o mod estiver ativado, ele anota quais programas têm janela aberta.
Na próxima vez que você entrar no Windows, esses programas são abertos de novo.
Para desligar a função, desative o mod.

- Só os programas são reabertos (um a cada 2 segundos, começando 15 segundos
  depois de entrar, por padrão). Documentos e abas dependem de cada programa.
- Programas abertos como administrador e componentes do Windows (Explorador de
  Arquivos, Painel de Controle...) não são reabertos.
- Programas que já têm uma janela aberta (por exemplo, os que iniciam com o
  Windows) não são abertos duas vezes.
- Os programas do próprio Windhawk nunca são reabertos.
- Programas cuja janela pertence a um processo auxiliar (por exemplo, Steam e
  apps baseados no Chromium) podem não voltar como estavam.
- Nada é reaberto se o mod for ativado muito depois do login, nem se o Explorer
  reiniciar durante o dia: só um login novo dispara a restauração.
- Cada conta do Windows tem a sua própria lista.

O Windows tem uma opção parecida (Configurações → Contas → Opções de entrada →
"Salvar automaticamente meus aplicativos reiniciáveis e reiniciá-los quando eu
entrar novamente"). Ela só restaura apps compatíveis (como Office e
navegadores); este mod reabre qualquer programa, pelo caminho do executável.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- restoreDelay: 15
  $name: Delay after sign-in (seconds)
  $name:pt-BR: Atraso após entrar (segundos)
  $description: How long to wait after signing in before reopening programs (0-600).
  $description:pt-BR: Quanto esperar depois de entrar antes de reabrir os programas (0-600).
- maxPrograms: 25
  $name: Maximum number of programs to reopen
  $name:pt-BR: Número máximo de programas a reabrir
  $description: Upper limit for the saved list (1-50).
  $description:pt-BR: Limite máximo da lista salva (1-50).
*/
// ==/WindhawkModSettings==

#include <appmodel.h>
#include <dwmapi.h>
#include <ntsecapi.h>
#include <sddl.h>
#include <shellapi.h>

#ifndef DWM_CLOAKED_SHELL
#define DWM_CLOAKED_SHELL 0x2  // may be missing in Windhawk's headers
#endif

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <string>
#include <vector>

extern "C" IMAGE_DOS_HEADER __ImageBase;  // this mod's DLL

constexpr wchar_t kSessionClass[] = L"ReopenPrograms_Session";
constexpr UINT_PTR kTimerSnapshot = 1, kTimerRestoreStart = 2,
                   kTimerRestoreStep = 3;
constexpr UINT kSnapshotIntervalMs = 20 * 1000;  // the list used at next sign-in
constexpr UINT kRestoreStepMs = 2 * 1000;        // one app at a time, no CPU spike
// Separates the saved programs. '?' can't appear in a file name or an AUMID,
// and survives the mod storage (portable mode drops '\n' characters).
constexpr wchar_t kListSeparator = L'?';
// Restore only if the sign-in is at most this old (FILETIME units, 100 ns).
constexpr ULONGLONG kMaxLogonAge = 10ULL * 60 * 10000000;

HANDLE g_sessionThread;
std::atomic<HWND> g_sessionWnd{nullptr};
std::atomic<bool> g_stopping{false};
std::atomic<int> g_restoreDelaySec{15};
std::atomic<int> g_maxApps{25};
// Touched only by the session thread.
bool g_endingSession, g_restoring;
std::vector<std::wstring> g_restoreQueue;
std::wstring g_lastSaved;

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) c = towlower(c);
    return s;
}

void LoadSettings() {
    g_restoreDelaySec = std::clamp(Wh_GetIntSetting(L"restoreDelay"), 0, 600);
    g_maxApps = std::clamp(Wh_GetIntSetting(L"maxPrograms"), 1, 50);
}

// ---------------------------------------------------------------------------
// Per-user storage
// ---------------------------------------------------------------------------

// The mod's storage is shared by every account on the PC, so each value name
// gets the user's SID appended: each account keeps its own list.
const std::wstring& UserSid() {
    static const std::wstring sid = [] {
        std::wstring result;
        HANDLE token;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
            DWORD size;
            PWSTR str;
            if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
                ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer)->User.Sid,
                                       &str)) {
                result = str;
                LocalFree(str);
            }
            CloseHandle(token);
        }
        return result;
    }();
    return sid;
}

// Empty = SID unknown: nothing is saved or restored.
std::wstring PerUserValueName(PCWSTR name) {
    return UserSid().empty() ? std::wstring() : std::wstring(name) + L"_" + UserSid();
}

std::wstring ReadStringValue(PCWSTR name) {
    std::wstring valueName = PerUserValueName(name);
    if (valueName.empty()) return {};
    std::vector<wchar_t> buf(32768);
    Wh_GetStringValue(valueName.c_str(), buf.data(), buf.size());
    return buf.data();
}

bool WriteStringValue(PCWSTR name, const std::wstring& value) {
    std::wstring valueName = PerUserValueName(name);
    return !valueName.empty() && Wh_SetStringValue(valueName.c_str(), value.c_str());
}

// ---------------------------------------------------------------------------
// Which programs are open
// ---------------------------------------------------------------------------

const std::wstring& WindowsDir() {
    static const std::wstring dir = [] {
        wchar_t buf[MAX_PATH];
        GetWindowsDirectoryW(buf, ARRAYSIZE(buf));
        return ToLower(buf) + L"\\";
    }();
    return dir;
}

// Same rule the taskbar uses: a visible top-level window that is not a tool
// window and is either unowned or explicitly marked as an app window. No title
// bar required: apps like Discord/Spotify/Steam draw their own.
bool IsAppWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) return false;

    // Hidden by the app itself (cloaked) = not really open. Windows on another
    // virtual desktop are cloaked by the shell and DO count.
    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked & ~DWM_CLOAKED_SHELL) return false;

    // Invisible 0x0 helper windows (minimized windows keep a real size).
    RECT rc;
    if (!IsIconic(hwnd) && GetWindowRect(hwnd, &rc) && IsRectEmpty(&rc))
        return false;

    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_APPWINDOW) return true;
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    return !GetWindow(hwnd, GW_OWNER);  // owned = dialog/popup
}

// Windhawk's own programs with a window are never relaunched: the tray app, the
// Windhawk 2.0 tool-mod hosts (windhawk-mod*.exe) and the editor
// (<Windhawk folder>\UI\VSCodium.exe). Matched by name, so other
// programs that share a folder with a portable Windhawk are not affected, and
// the disk is only probed for a VSCodium.exe.
bool IsWindhawkProgram(const std::wstring& exePath) {
    std::wstring lower = ToLower(exePath);
    size_t slash = lower.rfind(L'\\');
    if (slash == std::wstring::npos || slash == 0) return false;
    std::wstring name = lower.substr(slash + 1);
    if (name == L"windhawk.exe" || name.starts_with(L"windhawk-mod")) return true;
    if (name != L"vscodium.exe") return false;
    // The editor: <Windhawk folder>\UI\VSCodium.exe.
    size_t parent = lower.rfind(L'\\', slash - 1);
    return parent != std::wstring::npos &&
           lower.compare(parent, slash - parent + 1, L"\\ui\\") == 0 &&
           GetFileAttributesW((exePath.substr(0, parent + 1) + L"windhawk.exe").c_str()) !=
               INVALID_FILE_ATTRIBUTES;
}

// How to relaunch the app that owns `pid`: "A|<AUMID>" for Store/packaged
// apps, "P|<exe path>" for classic programs, or "" = do not relaunch (and
// `whySkipped`, if given, says why, for the log).
std::wstring AppKey(DWORD pid, std::wstring* whySkipped = nullptr) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        if (whySkipped) *whySkipped = L"pid " + std::to_wstring(pid) + L" (no access)";
        return {};
    }

    // Elevated (admin) apps are skipped: relaunching them would pop UAC
    // prompts at sign-in. If we can't even read the token, assume elevated.
    bool elevated = true;
    HANDLE token;
    if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation{};
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elevation,
                                sizeof(elevation), &size))
            elevated = elevation.TokenIsElevated;
        CloseHandle(token);
    }

    wchar_t path[MAX_PATH * 2];
    DWORD pathLen = ARRAYSIZE(path);
    bool hasPath = QueryFullProcessImageNameW(process, 0, path, &pathLen);
    std::wstring lower = hasPath ? ToLower(path) : L"";

    wchar_t aumid[130];  // APPLICATION_USER_MODEL_ID_MAX_LENGTH (missing in Windhawk's headers)
    UINT32 aumidLen = ARRAYSIZE(aumid);
    bool packaged = GetApplicationUserModelId(process, &aumidLen, aumid) == ERROR_SUCCESS;
    CloseHandle(process);

    // Windows' own components (Explorer, Start/Search UI, rundll32,
    // consoles...) are never relaunched: without their arguments they would do
    // nothing useful or show errors. Packaged apps that live in the Windows
    // folder (e.g. Settings) are fine, except the shell's own (SystemApps).
    std::wstring reason;
    if (!hasPath)
        reason = L"(no path)";
    else if (elevated)
        reason = L"(running as administrator)";
    else if (IsWindhawkProgram(path))
        reason = L"(Windhawk)";
    else if (packaged && lower.find(L"\\systemapps\\") == std::wstring::npos)
        return L"A|" + std::wstring(aumid);
    else if (!packaged && lower.rfind(WindowsDir(), 0) != 0)
        return L"P|" + std::wstring(path);
    else
        reason = L"(Windows component)";

    if (whySkipped)
        *whySkipped = (hasPath ? std::wstring(path) : L"pid " + std::to_wstring(pid)) +
                      L" " + reason;
    return {};
}

struct Snapshot {
    std::vector<std::wstring> apps;     // keys to relaunch
    std::vector<std::wstring> skipped;  // for the log only
};

void AddUnique(std::vector<std::wstring>& list, const std::wstring& item) {
    if (std::find(list.begin(), list.end(), item) == list.end()) list.push_back(item);
}

// A named CALLBACK function (not a lambda): on 32-bit builds a lambda can't
// convert to the __stdcall WNDENUMPROC.
BOOL CALLBACK CollectAppWindow(HWND hwnd, LPARAM param) {
    auto& snap = *reinterpret_cast<Snapshot*>(param);
    if (!IsAppWindow(hwnd)) return TRUE;

    // Store apps (Calculator, Photos...) are shown inside a frame owned by
    // ApplicationFrameHost.exe; the real app owns the inner CoreWindow.
    HWND appWnd = hwnd;
    wchar_t cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        !wcscmp(cls, L"ApplicationFrameWindow")) {
        appWnd = FindWindowExW(hwnd, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
        if (!appWnd) return TRUE;  // minimized/suspended: inner window detached
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(appWnd, &pid);
    std::wstring why;
    std::wstring key = AppKey(pid, &why);
    if (key.empty())
        AddUnique(snap.skipped, why);
    else
        AddUnique(snap.apps, key);
    return TRUE;
}

// Records which programs have a main window open right now.
void SaveSnapshot() {
    // While Windows is shutting down, programs are already closing: a snapshot
    // now would overwrite the good list with an empty/partial one.
    if (GetSystemMetrics(SM_SHUTTINGDOWN)) return;

    Snapshot snap;
    EnumWindows(CollectAppWindow, reinterpret_cast<LPARAM>(&snap));
    // EnumWindows returns windows in Z-order, which changes every time you
    // switch apps. Sorting means "changed" = a program opened/closed, not
    // just which window is in front, and the cap below keeps a stable set.
    std::sort(snap.apps.begin(), snap.apps.end());
    std::sort(snap.skipped.begin(), snap.skipped.end());
    if (snap.apps.size() > static_cast<size_t>(g_maxApps))
        snap.apps.resize(g_maxApps);

    std::wstring joined, skipped;
    for (const auto& app : snap.apps) joined += app + kListSeparator;
    std::wstring readable = joined;  // same list, one program per line, for the log
    std::replace(readable.begin(), readable.end(), kListSeparator, L'\n');
    for (const auto& s : snap.skipped) skipped += s + L'\n';
    // Only write when something changed (no registry write every 20 s).
    if (joined != g_lastSaved && WriteStringValue(L"lastSession", joined)) {
        g_lastSaved = joined;
        Wh_Log(L"Saved %u app(s):\n%s", (unsigned)snap.apps.size(), readable.c_str());
        if (!skipped.empty()) Wh_Log(L"Not saved (by design):\n%s", skipped.c_str());
    }
}

// ---------------------------------------------------------------------------
// Reopening
// ---------------------------------------------------------------------------

void BuildRestoreQueue() {
    // Programs that already have a window (e.g. started by Startup) are not
    // launched twice. Compared by window, like the saved list: a background
    // process without a window (browser helper, tray app) doesn't count as open.
    Snapshot open;
    EnumWindows(CollectAppWindow, reinterpret_cast<LPARAM>(&open));

    std::wstring saved = ReadStringValue(L"lastSession");
    // Also accepts '\n', the separator 2.0.0 used on regular installs.
    for (size_t start = 0, end; (end = saved.find_first_of(L"?\n", start)) != saved.npos;
         start = end + 1) {
        std::wstring key = saved.substr(start, end - start);
        if (key.size() <= 2) continue;
        if (g_restoreQueue.size() >= static_cast<size_t>(g_maxApps)) break;
        if (std::find(open.apps.begin(), open.apps.end(), key) != open.apps.end())
            Wh_Log(L"Already open, skipped: %s", key.c_str());
        else
            g_restoreQueue.push_back(key);
    }
    Wh_Log(L"Restoring %u app(s)", (unsigned)g_restoreQueue.size());
}

void LaunchApp(const std::wstring& key) {
    std::wstring value = key.substr(2), app, cmdLine, dir;
    if (key[0] == L'A') {
        // "explorer.exe shell:AppsFolder\<AUMID>" starts a packaged app.
        app = WindowsDir() + L"explorer.exe";
        cmdLine = L"explorer.exe shell:AppsFolder\\" + value;
    } else {
        if (GetFileAttributesW(value.c_str()) == INVALID_FILE_ATTRIBUTES)
            return;  // uninstalled since then
        app = value;
        cmdLine = L"\"" + value + L"\"";
        dir = value.substr(0, value.rfind(L'\\'));  // many apps expect this
    }

    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi;
    if (CreateProcessW(app.c_str(), cmdLine.data(), nullptr, nullptr, FALSE, 0,
                       nullptr, dir.empty() ? nullptr : dir.c_str(), &si, &pi)) {
        Wh_Log(L"Launched %s", value.c_str());
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        Wh_Log(L"Could not launch %s (error %u)", value.c_str(),
               (unsigned)GetLastError());
    }
}

// The current sign-in's id (logon session) and when it started.
bool GetLogonInfo(LUID& id, ULONGLONG& logonTime) {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return false;
    TOKEN_STATISTICS stats;
    DWORD size;
    bool ok = GetTokenInformation(token, TokenStatistics, &stats, sizeof(stats), &size);
    CloseHandle(token);
    if (!ok) return false;

    id = stats.AuthenticationId;
    PSECURITY_LOGON_SESSION_DATA data = nullptr;
    if (LsaGetLogonSessionData(&id, &data) != 0 || !data) return false;
    logonTime = static_cast<ULONGLONG>(data->LogonTime.QuadPart);
    LsaFreeReturnBuffer(data);
    return true;
}

// True once per sign-in, and only if that sign-in is recent. This is what keeps
// an Explorer restart (same sign-in) or enabling the mod in the afternoon (old
// sign-in, possibly days-old list) from reopening everything.
bool ShouldRestoreThisSignIn() {
    LUID id;
    ULONGLONG logonTime;
    if (!GetLogonInfo(id, logonTime)) return false;

    std::wstring key = std::to_wstring(id.HighPart) + L":" +
                       std::to_wstring(id.LowPart) + L":" +
                       std::to_wstring(logonTime);
    if (ReadStringValue(L"lastRestoredSignIn") == key) return false;
    if (!WriteStringValue(L"lastRestoredSignIn", key)) return false;

    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULONGLONG now = (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    bool recent = now >= logonTime && now - logonTime <= kMaxLogonAge;
    Wh_Log(L"New sign-in detected, recent=%d", recent ? 1 : 0);
    return recent;
}

// ---------------------------------------------------------------------------
// Session thread: hidden window, timers and shutdown notifications
// ---------------------------------------------------------------------------

LRESULT CALLBACK SessionWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerSnapshot) {
                if (!g_endingSession && !g_restoring) SaveSnapshot();
            } else if (wParam == kTimerRestoreStart) {
                KillTimer(hwnd, wParam);
                BuildRestoreQueue();
                SetTimer(hwnd, kTimerRestoreStep, kRestoreStepMs, nullptr);
            } else if (wParam == kTimerRestoreStep) {
                // A Startup program may have opened its window since the queue
                // was built: skip it instead of starting a second copy.
                Snapshot open;
                EnumWindows(CollectAppWindow, reinterpret_cast<LPARAM>(&open));
                while (!g_restoreQueue.empty() &&
                       std::find(open.apps.begin(), open.apps.end(),
                                 g_restoreQueue.front()) != open.apps.end()) {
                    Wh_Log(L"Already open, skipped: %s", g_restoreQueue.front().c_str());
                    g_restoreQueue.erase(g_restoreQueue.begin());
                }
                if (g_restoreQueue.empty()) {
                    KillTimer(hwnd, wParam);
                    g_restoring = false;  // snapshots resume
                } else {
                    LaunchApp(g_restoreQueue.front());
                    g_restoreQueue.erase(g_restoreQueue.begin());
                }
            }
            return 0;

        // Shutdown/restart/sign-out: freeze the list. No snapshot here: this
        // helper is an ordinary app, so it is told in no particular order and
        // programs may already be closing. SM_SHUTTINGDOWN covers the rest.
        case WM_QUERYENDSESSION:
            g_endingSession = true;
            return TRUE;
        case WM_ENDSESSION:
            if (!wParam) g_endingSession = false;  // shutdown was cancelled
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI SessionThread(LPVOID) {
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    WNDCLASSW wc{};
    wc.lpfnWndProc = SessionWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = kSessionClass;
    RegisterClassW(&wc);

    // Hidden top-level window (not message-only: those don't receive
    // WM_QUERYENDSESSION).
    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kSessionClass, L"", WS_POPUP,
                                0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    g_sessionWnd = hwnd;
    if (hwnd) {
        g_lastSaved = ReadStringValue(L"lastSession");
        if (ShouldRestoreThisSignIn()) {
            g_restoring = true;
            SetTimer(hwnd, kTimerRestoreStart,
                     static_cast<UINT>(g_restoreDelaySec) * 1000, nullptr);
        }
        SetTimer(hwnd, kTimerSnapshot, kSnapshotIntervalMs, nullptr);

        if (g_stopping) DestroyWindow(hwnd);  // uninit raced with startup
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnregisterClassW(kSessionClass, instance);
    return 0;
}

// ---------------------------------------------------------------------------
// Tool mod lifecycle (the boilerplate below calls these)
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    LoadSettings();
    g_sessionThread = CreateThread(nullptr, 0, SessionThread, nullptr, 0, nullptr);
    return g_sessionThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    // Stop the session thread; the boilerplate then ends the process.
    if (!g_sessionThread) return;
    g_stopping = true;
    if (HWND wnd = g_sessionWnd) PostMessageW(wnd, WM_CLOSE, 0, 0);
    WaitForSingleObject(g_sessionThread, INFINITE);
    CloseHandle(g_sessionThread);
}


////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
