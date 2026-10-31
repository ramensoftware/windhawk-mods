// ==WindhawkMod==
// @id              windows-terminal-start-minimized
// @name            Windows Terminal Start Minimized
// @name:pt-BR      Windows Terminal iniciar minimizado
// @description     When Windows Terminal opens at sign-in, it starts minimized or straight in the notification area. Requires Terminal's "Launch on machine startup" option
// @description:pt-BR Quando o Windows Terminal abre no logon, ele já inicia minimizado ou direto na área de notificação. Requer a opção "Iniciar na inicialização da máquina" do Terminal
// @version         1.0.0
// @author          caliberda
// @github          https://github.com/cesarkali
// @homepage        https://caliberda.com.br
// @donateUrl       https://livepix.gg/caliberda
// @include         WindowsTerminal.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Terminal Start Minimized

**English** | [Português](#português)

Made in Brazil by **caliberda** · bugs and ideas: [an issue on
GitHub](https://github.com/ramensoftware/windhawk-mods/issues/new?title=%5BWindows%20Terminal%20Start%20Minimized%5D%20&body=Mod%20author%3A%20%40cesarkali%0A%0A) · Instagram [@cesar.kali](https://www.instagram.com/cesar.kali) ·
[caliberda.com.br](https://caliberda.com.br)

When Windows Terminal opens at sign-in, this mod makes it start **minimized**,
or straight in the **notification area**, instead of on the screen.

## Required: turn on Terminal's options

The mod works through Terminal's own options and never changes them, so it
**does nothing until you turn them on**. Open Terminal's settings (`Ctrl+,`):

1. **Required:** **Startup** > **Launch on machine startup** (called **Launch
   at login** in newer versions). [Microsoft's
   guide](https://learn.microsoft.com/windows/terminal/customize-settings/startup#launch-on-machine-startup).
   Terminal must also be on in Windows **Settings** > **Apps** > **Startup**.
2. **To start in the notification area:** **Appearance** > **Hide Terminal in
   the notification area when it is minimized**. [Microsoft's
   guide](https://learn.microsoft.com/windows/terminal/customize-settings/interaction#minimize-to-notification-area).
   Without it, Terminal starts minimized on the taskbar.

Then sign out and back in (or restart) to see it work.

## What changes

Without the mod, Terminal opens on the screen at sign-in even with both options
on, and only goes to the notification area after you minimize it. With the mod:

* With option 2 on, it goes straight to the notification area, without showing
  on the screen.
* With option 2 off, it starts minimized on the taskbar.

It doesn't take the focus from what you're doing. Click its icon to bring it
back, in the size Terminal would have used (maximized, if Terminal is set to
launch maximized).

## How it works

The mod runs only inside Windows Terminal. When Windows starts Terminal through
its **Launch on machine startup** option, the windows of that launch (one, or
one per window when Terminal restores your last session) are shown minimized
instead of on the screen. Opening Terminal any other way (Start menu, taskbar,
`wt` command, a shortcut) isn't changed, even right after you sign in.

The mod stops acting when you bring back one of those windows, or one minute
after Terminal started.

## Known limitations

* Only the Microsoft Store (packaged) version of Terminal has the **Launch on
  machine startup** option. The portable (zip) version is left unchanged.
* If Windhawk starts after Terminal, the mod can't act on that sign-in.

## Contact

I'm **caliberda**, a developer from Brazil. Write in English, Portuguese or
your own language.

* **Bugs and ideas: an issue on GitHub**, the best way: [report a
  bug](https://github.com/ramensoftware/windhawk-mods/issues/new?labels=mod-bug&title=%5BWindows%20Terminal%20Start%20Minimized%5D%20&body=Mod%20author%3A%20%40cesarkali%0A%0A) or [suggest an
  idea](https://github.com/ramensoftware/windhawk-mods/issues/new?labels=mod-enhancement&title=%5BWindows%20Terminal%20Start%20Minimized%5D%20&body=Mod%20author%3A%20%40cesarkali%0A%0A).
  It mentions me (@cesarkali), so I see it.
* **Instagram [@cesar.kali](https://www.instagram.com/cesar.kali)**: to talk to me
  and follow my projects.
* **[caliberda.com.br](https://caliberda.com.br)**: my site, with my other
  projects and a Discord link.

---

## Português

Feito no Brasil por **caliberda** · pode falar comigo em português · bugs e
ideias: [uma issue no
GitHub](https://github.com/ramensoftware/windhawk-mods/issues/new?title=%5BWindows%20Terminal%20Start%20Minimized%5D%20&body=Mod%20author%3A%20%40cesarkali%0A%0A) · Instagram
[@cesar.kali](https://www.instagram.com/cesar.kali) · [caliberda.com.br](https://caliberda.com.br)

Quando o Windows Terminal abre no logon, este mod faz ele **já iniciar
minimizado**, ou direto na **área de notificação**, em vez de aparecer na tela.

### Obrigatório: ligue as opções do Terminal

O mod funciona pelas opções do próprio Terminal e nunca as altera, então ele
**não faz nada até você ligá-las**. Abra as configurações do Terminal
(`Ctrl+,`):

1. **Obrigatório:** **Inicialização** > **Iniciar na inicialização da
   máquina** (chamada **Iniciar no logon** nas versões mais novas). [Guia da
   Microsoft](https://learn.microsoft.com/pt-br/windows/terminal/customize-settings/startup#launch-on-machine-startup).
   O Terminal também precisa estar ligado em **Configurações** do Windows >
   **Aplicativos** > **Inicialização**.
2. **Para iniciar na área de notificação:** **Aparência** > **Ocultar o
   Terminal na área de notificação quando estiver minimizado**. [Guia da
   Microsoft](https://learn.microsoft.com/pt-br/windows/terminal/customize-settings/interaction#minimize-to-notification-area).
   Sem ela, o Terminal inicia minimizado na barra de tarefas.

Depois, saia da conta e entre de novo (ou reinicie) para ver funcionando.

### O que muda

Sem o mod, o Terminal abre na tela no logon mesmo com as duas opções ligadas, e
só vai para a área de notificação depois que você o minimiza. Com o mod:

* Com a opção 2 ligada, ele vai direto para a área de notificação, sem aparecer
  na tela.
* Com a opção 2 desligada, ele inicia minimizado na barra de tarefas.

Ele não tira o foco do que você está fazendo. Clique no ícone dele para
trazê-lo de volta, no tamanho que o Terminal usaria (maximizado, se o Terminal
estiver configurado para abrir maximizado).

### Como funciona

O mod roda só dentro do Windows Terminal. Quando o Windows abre o Terminal pela
opção **Iniciar na inicialização da máquina**, as janelas dessa inicialização
(uma, ou uma por janela quando o Terminal restaura a última sessão) aparecem
minimizadas em vez de na tela. Abrir o Terminal de outro jeito (menu Iniciar,
barra de tarefas, comando `wt`, um atalho) não muda, mesmo logo depois de você
entrar.

O mod para de agir quando você traz de volta uma dessas janelas, ou um minuto
depois de o Terminal iniciar.

### Limitações conhecidas

* Só a versão da Microsoft Store (empacotada) do Terminal tem a opção
  **Iniciar na inicialização da máquina**. A versão portátil (zip) não muda.
* Se o Windhawk iniciar depois do Terminal, o mod não consegue agir naquele
  logon.

### Contato

Sou o **caliberda**, desenvolvedor brasileiro. Pode escrever em português.

* **Bugs e ideias: uma issue no GitHub**, o melhor caminho: [relatar um
  bug](https://github.com/ramensoftware/windhawk-mods/issues/new?labels=mod-bug&title=%5BWindows%20Terminal%20Start%20Minimized%5D%20&body=Mod%20author%3A%20%40cesarkali%0A%0A) ou [sugerir uma
  ideia](https://github.com/ramensoftware/windhawk-mods/issues/new?labels=mod-enhancement&title=%5BWindows%20Terminal%20Start%20Minimized%5D%20&body=Mod%20author%3A%20%40cesarkali%0A%0A).
  Ela já me marca (@cesarkali), então eu vejo.
* **Instagram [@cesar.kali](https://www.instagram.com/cesar.kali)**: para conversar
  comigo e acompanhar meus projetos.
* **[caliberda.com.br](https://caliberda.com.br)**: meu site, com meus outros
  projetos e um link para o Discord.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <appmodel.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cwchar>
#include <mutex>
#include <vector>

#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.ApplicationModel.h>

// The sign-in launch, when the mod keeps Terminal's windows minimized, lasts at
// most this long after the process started.
constexpr ULONGLONG kSignInLaunchMs = 60 * 1000;

enum class Phase { Undecided, SignInLaunch, Over };

std::atomic<Phase> g_phase{Phase::Undecided};
ULONGLONG g_signInLaunchEnd;  // GetTickCount64() time, set in Wh_ModInit.

std::mutex g_mutex;
std::vector<HWND> g_launchWindows;     // Guarded by g_mutex.
std::vector<HWND> g_minimizedWindows;  // Guarded by g_mutex.

// The window whose next SetForegroundWindow call is skipped.
thread_local HWND g_skipForegroundFor;

void EndSignInLaunch(PCWSTR reason) {
    if (g_phase.exchange(Phase::Over) != Phase::Over) {
        Wh_Log(L"Sign-in launch over: %s", reason);
    }
}

// Whether the sign-in launch is still on, ending it when its time is up or the
// user brought back a window the mod minimized. Called with g_mutex held.
bool SignInLaunchActive() {
    if (g_phase != Phase::SignInLaunch) {
        return false;
    }
    if (GetTickCount64() >= g_signInLaunchEnd) {
        EndSignInLaunch(L"time limit");
        return false;
    }
    // A taskbar button click restores the window without a call in this
    // process, so the window's state is what tells.
    for (HWND hWnd : g_minimizedWindows) {
        if (IsWindowVisible(hWnd) && !IsIconic(hWnd)) {
            EndSignInLaunch(L"a minimized window is on the screen");
            return false;
        }
    }
    return true;
}

ULONGLONG ProcessAgeMs() {
    FILETIME creation, exit, kernel, user, now;
    if (!GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel,
                         &user)) {
        return ULLONG_MAX;
    }
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER start{.LowPart = creation.dwLowDateTime,
                         .HighPart = creation.dwHighDateTime};
    ULARGE_INTEGER current{.LowPart = now.dwLowDateTime,
                           .HighPart = now.dwHighDateTime};
    if (current.QuadPart < start.QuadPart) {
        return 0;
    }
    return (current.QuadPart - start.QuadPart) / 10000;
}

bool IsTerminalWindow(HWND hWnd) {
    WCHAR className[64];
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0;
}

bool IsShowCommand(int nCmdShow) {
    switch (nCmdShow) {
        case SW_SHOWNORMAL:
        case SW_SHOWMAXIMIZED:
        case SW_SHOWNOACTIVATE:
        case SW_SHOW:
        case SW_SHOWNA:
        case SW_RESTORE:
        case SW_SHOWDEFAULT:
            return true;
    }
    return false;
}

// Packaged desktop apps get the startup task activation kind when Windows
// starts them through a windows.startupTask extension, which is how Terminal's
// "Launch on machine startup" option works.
bool LaunchedByStartupTask() {
    using namespace winrt::Windows::ApplicationModel;
    try {
        auto args = AppInstance::GetActivatedEventArgs();
        if (!args) {
            Wh_Log(L"No activation arguments");
            return false;
        }
        auto kind = args.Kind();
        Wh_Log(L"Activation kind: %d", static_cast<int>(kind));
        return kind == Activation::ActivationKind::StartupTask;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"GetActivatedEventArgs failed: 0x%08X",
               static_cast<unsigned>(e.code()));
        return false;
    }
}

BOOL CALLBACK AddTerminalWindow(HWND hWnd, LPARAM lParam) {
    DWORD processId;
    if (GetWindowThreadProcessId(hWnd, &processId) &&
        processId == GetCurrentProcessId() && IsTerminalWindow(hWnd)) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
    }
    return TRUE;
}

// Shows a new Terminal window minimized instead of with nCmdShow, if it's part
// of the sign-in launch. Returns false to let the window show normally.
bool StartMinimized(HWND hWnd, int nCmdShow) {
    if (g_phase == Phase::Undecided) {
        // Decided once, on the first window: the activation kind doesn't change.
        if (!LaunchedByStartupTask()) {
            EndSignInLaunch(L"not launched at sign-in");
            return false;
        }
        // Terminal runs all its windows in one process. It creates the windows
        // of its launch (one, or one per window of a restored layout) before
        // its message loop starts, and shows them only after that. A window the
        // user opens meanwhile (Start menu, taskbar, a shortcut, `wt`) is handed
        // over by another Terminal process through a message, so it's created
        // later and isn't in this list.
        std::vector<HWND> launchWindows;
        EnumWindows(AddTerminalWindow, reinterpret_cast<LPARAM>(&launchWindows));
        Wh_Log(L"Sign-in launch with %zu window(s)", launchWindows.size());
        {
            std::lock_guard lock(g_mutex);
            g_launchWindows = std::move(launchWindows);
        }
        Phase expected = Phase::Undecided;
        g_phase.compare_exchange_strong(expected, Phase::SignInLaunch);
    }

    {
        std::lock_guard lock(g_mutex);
        if (!SignInLaunchActive()) {
            return false;
        }
        if (std::find(g_minimizedWindows.begin(), g_minimizedWindows.end(),
                      hWnd) != g_minimizedWindows.end()) {
            // Terminal is bringing back a window the mod minimized (its
            // notification area icon, a `wt` command).
            EndSignInLaunch(L"a minimized window was brought back");
            return false;
        }
        if (std::find(g_launchWindows.begin(), g_launchWindows.end(), hWnd) ==
            g_launchWindows.end()) {
            Wh_Log(L"Window %p was opened by the user, showing it", hWnd);
            return false;
        }
    }

    if (IsWindowVisible(hWnd)) {
        return false;
    }

    // SetWindowPlacement instead of ShowWindow(SW_SHOWMINNOACTIVE) so that a
    // window Terminal would show maximized is maximized when restored. The
    // window gets WM_SIZE with SIZE_MINIMIZED either way, which is when
    // Terminal hides it in the notification area if that option is on.
    WINDOWPLACEMENT placement{};
    placement.length = sizeof(placement);
    if (!GetWindowPlacement(hWnd, &placement)) {
        return false;
    }
    placement.showCmd = SW_SHOWMINNOACTIVE;
    if (nCmdShow == SW_SHOWMAXIMIZED) {
        placement.flags |= WPF_RESTORETOMAXIMIZED;
    }
    if (!SetWindowPlacement(hWnd, &placement)) {
        Wh_Log(L"SetWindowPlacement failed: %u", GetLastError());
        return false;
    }

    {
        std::lock_guard lock(g_mutex);
        g_minimizedWindows.push_back(hWnd);
    }
    g_skipForegroundFor = hWnd;
    Wh_Log(L"Started window %p minimized (show command %d)", hWnd, nCmdShow);
    return true;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_phase != Phase::Over && IsShowCommand(nCmdShow) &&
        IsTerminalWindow(hWnd) && StartMinimized(hWnd, nCmdShow)) {
        // The return value is the previous visibility: the window was hidden.
        return FALSE;
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Original;
BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    // Terminal brings each new window to the front right after showing it.
    // Skip that call for a window the mod started minimized, so that the
    // keyboard focus stays in the app the user is in.
    HWND skip = g_skipForegroundFor;
    if (skip) {
        g_skipForegroundFor = nullptr;
        if (hWnd == skip && (IsIconic(hWnd) || !IsWindowVisible(hWnd))) {
            return FALSE;
        }
    }
    return SetForegroundWindow_Original(hWnd);
}

// Whether a restore Terminal posted to hWnd is skipped: during the sign-in
// launch, for a window the mod minimized.
bool KeepMinimized(HWND hWnd) {
    std::lock_guard lock(g_mutex);
    if (!SignInLaunchActive()) {
        return false;
    }
    if (std::find(g_minimizedWindows.begin(), g_minimizedWindows.end(),
                  hWnd) == g_minimizedWindows.end()) {
        return false;
    }
    Wh_Log(L"Kept window %p minimized: Terminal asked to restore it", hWnd);
    return true;
}

using PostMessageW_t = decltype(&PostMessageW);
PostMessageW_t PostMessageW_Original;
BOOL WINAPI PostMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam,
                              LPARAM lParam) {
    // A tab's console (ConPTY) reports the window as shown when its shell
    // starts, and Terminal restores a minimized window when that happens, by
    // posting SC_RESTORE (IslandWindow::ShowWindowChanged). Clicks on the
    // taskbar button don't come from this process, and the notification area
    // icon restores with ShowWindow, so they still work.
    if (Msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_RESTORE &&
        g_phase == Phase::SignInLaunch && KeepMinimized(hWnd)) {
        return TRUE;
    }
    return PostMessageW_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // Only the packaged Terminal has the startup task.
    UINT32 length = 0;
    if (GetCurrentPackageFullName(&length, nullptr) ==
        APPMODEL_ERROR_NO_PACKAGE) {
        Wh_Log(L"Unpackaged Terminal, nothing to do");
        return FALSE;
    }

    ULONGLONG age = ProcessAgeMs();
    if (age >= kSignInLaunchMs) {
        Wh_Log(L"Terminal started %llu ms ago, nothing to do", age);
        return FALSE;
    }
    g_signInLaunchEnd = GetTickCount64() + (kSignInLaunchMs - age);

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow,
                                   SetForegroundWindow_Hook,
                                   &SetForegroundWindow_Original);
    WindhawkUtils::SetFunctionHook(PostMessageW, PostMessageW_Hook,
                                   &PostMessageW_Original);
    return TRUE;
}
