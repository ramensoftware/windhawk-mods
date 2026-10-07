// ==WindhawkMod==
// @id              reopen-programs-at-sign-in
// @name            Reopen Programs at Sign-in
// @description     Reopens the programs that were open before shutdown/restart the next time you sign in
// @version         1.2.0
// @author          lima26x
// @github          https://github.com/brdantas26
// @include         explorer.exe
// @compilerOptions -luser32 -ladvapi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Reopen Programs at Sign-in

When the mod is enabled for the first time, a small window asks whether you
want the programs that were open to be reopened automatically the next time
you sign in to Windows. To open that window again (and turn it on/off), press
**Ctrl + Alt + Shift + R**.

- Only programs are reopened (one every 2 seconds, starting 15 s after
  sign-in). Documents/tabs depend on each program.
- Programs running as administrator and Windows components (File Explorer,
  Control Panel...) are not reopened.
- Programs that are already running (e.g. from Startup) are not opened twice.
- If Explorer restarts during the day, nothing is reopened.

---

# Reabrir programas ao entrar (Português)

Quando o mod é ativado pela primeira vez, uma pequena janela pergunta se você
quer que os programas que estavam abertos sejam reabertos automaticamente na
próxima vez que você entrar no Windows. Para abrir essa janela de novo (e
ligar/desligar a função), pressione **Ctrl + Alt + Shift + R**.

- Só os programas são reabertos (um a cada 2 segundos, começando 15 s depois
  de entrar). Documentos e abas dependem de cada programa.
- Programas abertos como administrador e componentes do Windows (Explorador de
  Arquivos, Painel de Controle...) não são reabertos.
- Programas que já estão abertos (por exemplo, os que iniciam com o Windows)
  não são abertos duas vezes.
- Se o Explorer reiniciar durante o dia, nada é reaberto.
*/
// ==/WindhawkModReadme==

#include <appmodel.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <tlhelp32.h>

#ifndef DWM_CLOAKED_SHELL
#define DWM_CLOAKED_SHELL 0x2  // may be missing in Windhawk's headers
#endif

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cwctype>
#include <string>
#include <vector>

extern "C" IMAGE_DOS_HEADER __ImageBase;  // this mod's DLL

constexpr wchar_t kSessionClass[] = L"ReopenPrograms_Session";
constexpr UINT_PTR kTimerSnapshot = 1, kTimerRestoreStart = 2,
                   kTimerRestoreStep = 3, kTimerFirstRun = 4;
constexpr UINT kSnapshotIntervalMs = 20 * 1000;  // the list used at next sign-in
constexpr UINT kRestoreDelayMs = 15 * 1000;      // let the desktop finish loading
constexpr UINT kRestoreStepMs = 2 * 1000;        // one app at a time, no CPU spike
constexpr size_t kMaxApps = 25;
constexpr int kHotkeyId = 1;  // Ctrl + Alt + Shift + R

HANDLE g_sessionThread;
std::atomic<HWND> g_sessionWnd{nullptr};
std::atomic<HWND> g_dialogWnd{nullptr};
std::atomic<bool> g_stopping{false};
// Touched only by the session thread.
bool g_dialogOpen, g_endingSession, g_restoring;
std::vector<std::wstring> g_restoreQueue;
std::wstring g_lastSaved;

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) c = towlower(c);
    return s;
}

// Formats with standard printf rules (%ls = wide string) and sends to the
// Windhawk log (visible with "Debug logging" enabled in the mod's Advanced tab).
void Log(PCWSTR format, ...) {
    wchar_t msg[4096];
    va_list args;
    va_start(args, format);
    vswprintf(msg, ARRAYSIZE(msg), format, args);
    va_end(args);
    Wh_Log(L"%s", msg);
}

// -1 = never asked, 0 = off, 1 = on. Kept in the mod's own storage.
int RestoreChoice() { return Wh_GetIntValue(L"restoreEnabled", -1); }

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

std::wstring ReadSavedSession() {
    std::vector<wchar_t> buf(32768);
    Wh_GetStringValue(L"lastSession", buf.data(), buf.size());
    return buf.data();
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
    if (snap.apps.size() >= kMaxApps || !IsAppWindow(hwnd)) return TRUE;

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
    // just which window is in front.
    std::sort(snap.apps.begin(), snap.apps.end());
    std::sort(snap.skipped.begin(), snap.skipped.end());

    std::wstring joined, skipped;
    for (const auto& app : snap.apps) joined += app + L'\n';
    for (const auto& s : snap.skipped) skipped += s + L'\n';
    // Only write when something changed (no registry write every 20 s).
    if (joined != g_lastSaved && Wh_SetStringValue(L"lastSession", joined.c_str())) {
        g_lastSaved = joined;
        Log(L"Saved %u app(s):\n%ls", (unsigned)snap.apps.size(), joined.c_str());
        if (!skipped.empty()) Log(L"Not saved (by design):\n%ls", skipped.c_str());
    }
}

void BuildRestoreQueue() {
    // Apps already running (e.g. from Startup) are not launched twice.
    std::vector<std::wstring> running;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe{sizeof(pe)};
        for (BOOL ok = Process32FirstW(snap, &pe); ok; ok = Process32NextW(snap, &pe))
            running.push_back(AppKey(pe.th32ProcessID));
        CloseHandle(snap);
    }

    std::wstring saved = ReadSavedSession();
    for (size_t start = 0, end; (end = saved.find(L'\n', start)) != saved.npos;
         start = end + 1) {
        std::wstring key = saved.substr(start, end - start);
        if (key.size() <= 2) continue;
        if (std::find(running.begin(), running.end(), key) != running.end())
            Log(L"Already running, skipped: %ls", key.c_str());
        else
            g_restoreQueue.push_back(key);
    }
    Log(L"Restoring %u app(s)", (unsigned)g_restoreQueue.size());
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
        Log(L"Launched %ls", value.c_str());
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        Log(L"Could not launch %ls (error %u)", value.c_str(),
            (unsigned)GetLastError());
    }
}

HRESULT CALLBACK DialogCallback(HWND hwnd, UINT msg, WPARAM, LPARAM, LONG_PTR) {
    if (msg == TDN_CREATED) {
        g_dialogWnd = hwnd;
        if (g_stopping) PostMessageW(hwnd, WM_CLOSE, 0, 0);
        SetForegroundWindow(hwnd);
    } else if (msg == TDN_DESTROYED) {
        g_dialogWnd = nullptr;
    }
    return S_OK;
}

// Options window texts, one set per language.
struct DialogText {
    PCWSTR title, question, content, statusLabel, on, off, onButton, offButton,
        footer, messageBoxHint;
};

constexpr DialogText kTextPt = {
    L"Reabrir programas ao entrar",
    L"Reabrir seus programas quando você entrar no Windows?",
    L"Quando está ligado, o computador anota quais programas estão abertos. "
    L"Depois que você desligar ou reiniciar, esses programas são abertos de "
    L"novo, sozinhos, assim que você entrar no Windows.\n\n"
    L"Bom saber:\n"
    L"• Só os programas são reabertos. Documentos e abas dependem de cada programa.\n"
    L"• Programas abertos como administrador não são reabertos, por segurança.\n"
    L"• Os programas abrem um de cada vez, para não deixar o computador lento.\n\n",
    L"Situação atual: ",
    L"LIGADO",
    L"DESLIGADO",
    L"Ligar\nReabrir automaticamente os programas que estavam abertos",
    L"Desligar\nIniciar o Windows normalmente, sem reabrir nada",
    L"Para abrir esta janela de novo, pressione Ctrl + Alt + Shift + R.",
    L"\n\nSim = Ligar     Não = Desligar",
};

constexpr DialogText kTextEn = {
    L"Reopen programs at sign-in",
    L"Reopen your programs when you sign in to Windows?",
    L"When this is on, your computer keeps track of which programs are open. "
    L"After you shut down or restart, those programs open again by "
    L"themselves as soon as you sign in to Windows.\n\n"
    L"Good to know:\n"
    L"• Only the programs are reopened. Documents and tabs depend on each program.\n"
    L"• Programs running as administrator are not reopened, for safety.\n"
    L"• Programs open one at a time, so your computer doesn't slow down.\n\n",
    L"Current status: ",
    L"ON",
    L"OFF",
    L"Turn on\nAutomatically reopen the programs that were open",
    L"Turn off\nStart Windows normally, without reopening anything",
    L"To open this window again, press Ctrl + Alt + Shift + R.",
    L"\n\nYes = Turn on     No = Turn off",
};

// Portuguese if Windows is in Portuguese (Brazil or Portugal), else English.
const DialogText& Text() {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE ? kTextPt
                                                                        : kTextEn;
}

// The options window: one on/off choice in plain language.
void ShowOptionsDialog() {
    if (g_dialogOpen) return;
    g_dialogOpen = true;

    enum { kOn = 100, kOff = 101 };
    const DialogText& text = Text();
    int current = RestoreChoice();
    std::wstring content = std::wstring(text.content) + text.statusLabel +
                           (current == 1 ? text.on : text.off);

    TASKDIALOG_BUTTON buttons[] = {
        {kOn, text.onButton},
        {kOff, text.offButton},
    };
    TASKDIALOGCONFIG cfg{sizeof(cfg)};
    cfg.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;
    cfg.dwCommonButtons = TDCBF_CLOSE_BUTTON;
    cfg.pszWindowTitle = text.title;
    cfg.pszMainIcon = TD_INFORMATION_ICON;
    cfg.pszMainInstruction = text.question;
    cfg.pszContent = content.c_str();
    cfg.cButtons = ARRAYSIZE(buttons);
    cfg.pButtons = buttons;
    cfg.nDefaultButton = current == 0 ? kOff : kOn;
    cfg.pszFooter = text.footer;
    cfg.pfCallback = DialogCallback;

    // Loaded at runtime so we get the comctl32 v6 that Explorer already uses
    // (TaskDialog doesn't exist in v5).
    int button = IDCANCEL;
    HMODULE comctl = LoadLibraryW(L"comctl32.dll");
    auto taskDialog = comctl ? reinterpret_cast<decltype(&TaskDialogIndirect)>(
                                   GetProcAddress(comctl, "TaskDialogIndirect"))
                             : nullptr;
    if (taskDialog) {
        taskDialog(&cfg, &button, nullptr, nullptr);
    } else {
        int r = MessageBoxW(nullptr,
                            (content + text.messageBoxHint).c_str(),
                            cfg.pszWindowTitle,
                            MB_YESNOCANCEL | MB_ICONQUESTION | MB_SETFOREGROUND);
        button = r == IDYES ? kOn : r == IDNO ? kOff : IDCANCEL;
    }
    if (comctl) FreeLibrary(comctl);

    if (button == kOn) {
        Wh_SetIntValue(L"restoreEnabled", 1);
        SaveSnapshot();
    } else if (button == kOff || current == -1) {
        Wh_SetIntValue(L"restoreEnabled", 0);  // closing on first run = off
    }

    g_dialogOpen = false;
    if (g_stopping) PostQuitMessage(0);
}

LRESULT CALLBACK SessionWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerFirstRun) {
                KillTimer(hwnd, wParam);
                ShowOptionsDialog();
            } else if (wParam == kTimerSnapshot) {
                if (!g_endingSession && !g_restoring && RestoreChoice() == 1)
                    SaveSnapshot();
            } else if (wParam == kTimerRestoreStart) {
                KillTimer(hwnd, wParam);
                BuildRestoreQueue();
                SetTimer(hwnd, kTimerRestoreStep, kRestoreStepMs, nullptr);
            } else if (wParam == kTimerRestoreStep) {
                if (g_restoreQueue.empty()) {
                    KillTimer(hwnd, wParam);
                    g_restoring = false;  // snapshots resume
                } else {
                    LaunchApp(g_restoreQueue.front());
                    g_restoreQueue.erase(g_restoreQueue.begin());
                }
            }
            return 0;

        case WM_HOTKEY:
            ShowOptionsDialog();
            return 0;

        // Shutdown/restart/sign-out: freeze the list. No snapshot here: Explorer
        // is usually told LAST, after programs have already closed.
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
            UnregisterHotKey(hwnd, kHotkeyId);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI SessionThread(LPVOID) {
    // Only one instance per user session, even with several explorer.exe
    // (e.g. "Launch folder windows in a separate process").
    HANDLE mutex = CreateMutexW(nullptr, FALSE, L"Local\\ReopenPrograms_Session");
    if (!mutex) return 0;
    // Normal: "explorer.exe shell:AppsFolder\..." (used to reopen Store apps)
    // and folder windows can start extra Explorer processes. Stay idle there.
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(mutex);
        return 0;
    }

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
        g_lastSaved = ReadSavedSession();
        if (!RegisterHotKey(hwnd, kHotkeyId,
                            MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'R'))
            Log(L"Hotkey Ctrl+Alt+Shift+R is taken by another program");

        // Volatile key = erased by Windows at sign-out. If we create it, this is
        // the first Explorer of this sign-in; if it already exists, Explorer
        // just restarted (crash) and we must NOT reopen everything again.
        HKEY key;
        DWORD disposition = 0;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\ReopenPrograms_Session",
                            0, nullptr, REG_OPTION_VOLATILE, KEY_READ, nullptr,
                            &key, &disposition) == ERROR_SUCCESS)
            RegCloseKey(key);

        int choice = RestoreChoice();
        Log(L"Started. choice=%d (-1 never asked, 0 off, 1 on), new sign-in=%d",
            choice, disposition == REG_CREATED_NEW_KEY ? 1 : 0);
        if (choice == -1) {
            SetTimer(hwnd, kTimerFirstRun, 3000, nullptr);  // first activation
        } else if (choice == 1 && disposition == REG_CREATED_NEW_KEY) {
            g_restoring = true;
            SetTimer(hwnd, kTimerRestoreStart, kRestoreDelayMs, nullptr);
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
    CloseHandle(mutex);
    return 0;
}

BOOL Wh_ModInit() {
    g_sessionThread = CreateThread(nullptr, 0, SessionThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    // Stop the session thread before the DLL is unloaded.
    if (g_sessionThread) {
        g_stopping = true;
        if (HWND dialog = g_dialogWnd) PostMessageW(dialog, WM_CLOSE, 0, 0);
        if (HWND wnd = g_sessionWnd) PostMessageW(wnd, WM_CLOSE, 0, 0);
        WaitForSingleObject(g_sessionThread, INFINITE);
        CloseHandle(g_sessionThread);
    }
}
