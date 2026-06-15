// ==WindhawkMod==
// @id              tray-hover-expand
// @name            Tray hover expand
// @description     Open the hidden tray icons flyout on hover instead of clicking the chevron; optionally collapse it when the cursor leaves
// @version         1.4.2
// @author          wygodad
// @github          https://github.com/wygodad
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray hover expand

![Demo](https://i.imgur.com/qsNUNpj.png)

Opens the hidden tray icons flyout (the "Show Hidden Icons" chevron) when you
hover the cursor over it, instead of having to click. Optionally collapses it
again once the cursor leaves the opened icons.

It works through UI Automation, so it does not hook internal shell functions —
it is relatively safe and resilient across Windows builds. It runs as a tool
mod in a dedicated process and does not inject into the shell.

## How it differs from similar mods
- *Taskbar tray icons hide on hover* auto-hides the whole tray area and reveals
  it on hover.
- *Show all taskbar notification icons* forces all hidden icons to always show.
- This mod keeps the standard Windows overflow flyout and simply opens it on
  hover (and optionally closes it when you move away).

## How the chevron is detected
The "Show Hidden Icons" chevron has no language-independent unique identifier on
Windows 11 (it shares AutomationId `SystemTrayIcon` with the clock, volume,
battery, etc., and exposes no ExpandCollapse pattern). So detection is a hybrid:
1. Match the button by name (the keyword list covers English and Polish by
   default and can be extended in the settings).
2. If no name matches (other languages), fall back to the leftmost tray button
   with the configured AutomationId, which is normally the chevron.

## Notes
- For unsupported languages, add your locale's chevron name to the "Chevron name
  keywords" setting, or rely on the leftmost-tray-icon fallback.
- If auto-collapse does not work, the flyout window class name may differ on your
  build. Change it in the "Flyout window class" setting.

---

## Opis po polsku

Otwiera schowek ukrytych ikon zasobnika (strzałkę „Pokaż ukryte ikony") po
najechaniu kursorem, bez konieczności klikania. Opcjonalnie zwija go z powrotem,
gdy kursor opuści otwarte ikony.

Mod działa przez UI Automation, więc nie hookuje wewnętrznych funkcji powłoki —
jest stosunkowo bezpieczny i odporny na zmiany między kompilacjami Windows.
Działa jako „tool mod" w osobnym procesie i nie wstrzykuje się do powłoki.

### Czym różni się od podobnych modów
- *Taskbar tray icons hide on hover* automatycznie ukrywa cały obszar zasobnika
  i odsłania go po najechaniu.
- *Show all taskbar notification icons* wymusza stałe wyświetlanie wszystkich
  ukrytych ikon.
- Ten mod zachowuje standardowy schowek Windows i po prostu otwiera go po
  najechaniu (a opcjonalnie zamyka po odjechaniu kursorem).

### Jak wykrywana jest strzałka
Strzałka „Pokaż ukryte ikony" nie ma w Windows 11 unikalnego, niezależnego od
języka identyfikatora (dzieli AutomationId `SystemTrayIcon` z zegarem,
głośnością, baterią itd. i nie udostępnia wzorca ExpandCollapse). Wykrywanie
jest więc hybrydowe:
1. Dopasowanie przycisku po nazwie (domyślna lista słów kluczowych obejmuje
   angielski i polski; można ją rozszerzyć w ustawieniach).
2. Gdy nazwa nie pasuje (inne języki) — pierwszy od lewej przycisk zasobnika ze
   skonfigurowanym AutomationId, którym zwykle jest strzałka.

### Uwagi
- Dla nieobsługiwanych języków dodaj nazwę strzałki w swoim języku do ustawienia
  „Słowa kluczowe nazwy strzałki" albo polegaj na powyższym mechanizmie zapasowym.
- Jeśli auto-zwijanie nie działa, nazwa klasy okna schowka może się różnić na
  Twojej kompilacji systemu. Zmień ją w ustawieniu „Klasa okna schowka".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoClose: true
  $name: Collapse when the cursor leaves
  $name:pl-PL: Zwijaj po odjechaniu kursorem
  $description: After the cursor leaves the opened icons (and the chevron), the flyout closes itself.
  $description:pl-PL: Gdy kursor opuści otwarte ikony (i strzałkę), schowek sam się zamyka.
- pollInterval: 50
  $name: Polling interval (ms)
  $name:pl-PL: Częstotliwość sprawdzania (ms)
  $description: How often to check the cursor position. Lower = smoother, more CPU.
  $description:pl-PL: Co ile sprawdzać pozycję kursora. Mniej = płynniej, większe użycie CPU.
- grace: 200
  $name: Collapse delay (ms)
  $name:pl-PL: Opóźnienie zwijania (ms)
  $description: How long the cursor must stay outside the area before the flyout closes (prevents flicker).
  $description:pl-PL: Jak długo kursor musi pozostawać poza obszarem, zanim schowek się zamknie (zapobiega miganiu).
- pad: 4
  $name: Hit area padding (pixels)
  $name:pl-PL: Margines obszaru najechania (piksele)
  $description: Enlarges the hover area around the chevron button.
  $description:pl-PL: Powiększa obszar najechania wokół przycisku strzałki.
- keywords: ["ukryte ikony", "hidden icons", "rozwiń", "overflow"]
  $name: Chevron name keywords
  $name:pl-PL: Słowa kluczowe nazwy strzałki
  $description: Case-insensitive substrings used to match the chevron button name. Add your locale's name for "Show Hidden Icons" here.
  $description:pl-PL: Fragmenty nazwy przycisku strzałki (wielkość liter bez znaczenia). Dodaj tu nazwę „Pokaż ukryte ikony" w swoim języku.
- flyoutClass: TopLevelWindowForOverflowXamlIsland
  $name: Flyout window class
  $name:pl-PL: Klasa okna schowka
  $description: Window class name of the opened flyout (used to tell whether the cursor is over the icons). Change it if auto-collapse does not work.
  $description:pl-PL: Nazwa klasy okna otwartego schowka (służy do sprawdzania, czy kursor jest nad ikonami). Zmień ją, jeśli auto-zwijanie nie działa.
- trayIconAutomationId: SystemTrayIcon
  $name: Tray icon AutomationId (fallback)
  $name:pl-PL: AutomationId ikony zasobnika (mechanizm zapasowy)
  $description: Used only when the chevron is not matched by name. The leftmost tray button with this AutomationId is then assumed to be the chevron.
  $description:pl-PL: Używane tylko, gdy strzałki nie uda się dopasować po nazwie. Za strzałkę uznawany jest wtedy pierwszy od lewej przycisk zasobnika o tym AutomationId.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <atomic>
#include <string>
#include <vector>
#include <algorithm>

#ifndef __IUIAutomation_FWD_DEFINED__
#error "UI Automation headers are missing"
#endif

struct Settings {
    bool autoClose = true;
    int pollInterval = 50;
    int grace = 200;
    int pad = 4;
    std::wstring flyoutClass = L"TopLevelWindowForOverflowXamlIsland";
    std::wstring trayIconAutomationId = L"SystemTrayIcon";
    std::vector<std::wstring> keywords = {
        L"ukryte ikony", L"hidden icons", L"rozwiń", L"overflow"
    };
};

// g_settings is guarded by g_settingsLock; the worker thread keeps a private
// snapshot and refreshes it when g_settingsGeneration changes, so it never
// reads the strings while the settings thread reassigns them.
static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<int> g_settingsGeneration{0};

static std::atomic<bool> g_running{false};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;

static Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::towlower);
    return r;
}

static bool NameMatches(const std::wstring& name, const Settings& s) {
    std::wstring low = ToLower(name);
    for (const auto& k : s.keywords) {
        if (!k.empty() && low.find(ToLower(k)) != std::wstring::npos) return true;
    }
    return false;
}

// ---- UIA pattern helpers ----

static void DoExpand(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Expand();
        p->Release();
        return;
    }
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

static void DoCollapse(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Collapse();
        p->Release();
        return;
    }
    // No ExpandCollapse support: a second Invoke toggles the flyout closed.
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

// ---- Locating the chevron button ----

static IUIAutomationElement* FindOverflowButton(IUIAutomation* pAuto,
                                                const Settings& s) {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return nullptr;

    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(pAuto->ElementFromHandle(hTaskbar, &pRoot)) || !pRoot) return nullptr;

    IUIAutomationCondition* pCond = nullptr;
    VARIANT v; VariantInit(&v);
    v.vt = VT_I4; v.lVal = UIA_ButtonControlTypeId;
    pAuto->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &pCond);

    // Hybrid strategy:
    //  1) match by name (reliable for configured languages),
    //  2) fallback: the leftmost button with the configured AutomationId
    //     (language-independent but heuristic) — used only when no name matches.
    IUIAutomationElement* nameMatch = nullptr;
    IUIAutomationElement* leftmost = nullptr;
    LONG leftmostX = 0;

    IUIAutomationElementArray* pArr = nullptr;
    if (pCond && SUCCEEDED(pRoot->FindAll(TreeScope_Subtree, pCond, &pArr)) && pArr) {
        int n = 0; pArr->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* e = nullptr;
            if (FAILED(pArr->GetElement(i, &e)) || !e) continue;

            BSTR name = nullptr;
            e->get_CurrentName(&name);
            bool matched = (name && NameMatches(name, s));
            if (name) SysFreeString(name);
            if (matched) {
                nameMatch = e;           // keep the reference; a name match wins
                break;
            }

            // Fallback candidate: a tray button with the configured AutomationId.
            BSTR aid = nullptr;
            e->get_CurrentAutomationId(&aid);
            bool isTrayIcon = (aid && s.trayIconAutomationId == aid);
            if (aid) SysFreeString(aid);

            if (isTrayIcon) {
                RECT r;
                if (SUCCEEDED(e->get_CurrentBoundingRectangle(&r)) &&
                    (!leftmost || r.left < leftmostX)) {
                    if (leftmost) leftmost->Release();
                    leftmost = e;        // keep the new leftmost candidate
                    leftmostX = r.left;
                    e = nullptr;
                }
            }
            if (e) e->Release();
        }
        pArr->Release();
    }
    if (pCond) pCond->Release();
    pRoot->Release();

    if (nameMatch) {
        if (leftmost) leftmost->Release();
        return nameMatch;
    }
    return leftmost;   // nullptr if nothing matched
}

static bool PtInRectPad(const RECT& r, POINT pt, int pad) {
    return pt.x >= r.left - pad && pt.x <= r.right + pad &&
           pt.y >= r.top - pad  && pt.y <= r.bottom + pad;
}

static const ULONGLONG ACTION_COOLDOWN_MS = 300;
static const ULONGLONG REFIND_INTERVAL_MS = 3000;
static const ULONGLONG RECT_REFRESH_MS = 750;
static const ULONGLONG IDLE_STATE_CHECK_MS = 500;

// Cheap (pure Win32) resolution of the flyout window — no UIA calls. The
// chevron does not support the ExpandCollapse pattern anyway, so the flyout
// window's visibility is the only reliable state signal. Returns the window
// handle if the flyout is open, or nullptr otherwise. Resolving the handle
// once per tick lets callers reuse it instead of each calling FindWindowW.
static HWND GetVisibleFlyout(const Settings& s) {
    HWND f = FindWindowW(s.flyoutClass.c_str(), nullptr);
    return (f && IsWindowVisible(f)) ? f : nullptr;
}

static bool PtOverWindow(HWND hwnd, POINT pt) {
    if (!hwnd) return false;
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return false;
    return PtInRect(&r, pt);
}

// ---- Worker thread ----

static DWORD WINAPI WorkerThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IUIAutomation* pAuto = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr,
            CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAuto)) || !pAuto) {
        Wh_Log(L"Failed to create IUIAutomation");
        CoUninitialize();
        return 0;
    }

    Settings s = GetSettingsSnapshot();
    int settingsGen = g_settingsGeneration;

    IUIAutomationElement* pBtn = nullptr;
    RECT cachedRect = {};
    bool haveRect = false;
    bool flyoutBelievedOpen = false;
    ULONGLONG leftAt = 0;
    ULONGLONG nextRefind = 0;
    ULONGLONG nextRectRefresh = 0;
    ULONGLONG nextIdleStateCheck = 0;
    ULONGLONG lastOpenAt = 0;
    bool overBtnPrev = false;
    bool clickedInFlyout = false;
    bool anyBtnDownPrev = false;

    while (g_running) {
        ULONGLONG now = GetTickCount64();

        if (g_settingsGeneration != settingsGen) {
            s = GetSettingsSnapshot();
            settingsGen = g_settingsGeneration;
            // Settings may change how the chevron is detected, so drop the
            // cached element and re-detect promptly with the new settings.
            if (pBtn) { pBtn->Release(); pBtn = nullptr; }
            haveRect = false;
            nextRefind = 0;
        }

        // Lazily (re-)find the button only when we don't have a valid one. A
        // destroyed or stale element makes the rectangle query below fail,
        // which clears pBtn and triggers a prompt re-find — so there is no need
        // for a periodic cross-process subtree walk while the element is valid.
        // When the chevron is absent (e.g. no hidden icons), throttle retries
        // to REFIND_INTERVAL_MS instead of walking the tree every tick.
        if (!pBtn && now >= nextRefind) {
            pBtn = FindOverflowButton(pAuto, s);
            nextRefind = now + REFIND_INTERVAL_MS;
            nextRectRefresh = 0;        // force a fresh rectangle below
        }

        if (pBtn) {
            // Expensive and RARE: query the button rectangle through UIA only
            // periodically, not every tick. The chevron rarely moves.
            if (now >= nextRectRefresh) {
                RECT r;
                if (SUCCEEDED(pBtn->get_CurrentBoundingRectangle(&r))) {
                    cachedRect = r;
                    haveRect = true;
                } else {
                    // The element is stale/destroyed (taskbar rebuilt). Drop it
                    // and re-find promptly on the next tick.
                    pBtn->Release(); pBtn = nullptr; haveRect = false;
                    nextRefind = 0;
                    WaitForSingleObject(g_stopEvent, s.pollInterval);
                    continue;
                }
                nextRectRefresh = now + RECT_REFRESH_MS;
            }
        }

        if (haveRect) {
            // Cheap and EVERY TICK: only local Win32 calls.
            POINT pt; GetCursorPos(&pt);
            bool overBtn = PtInRectPad(cachedRect, pt, s.pad);
            bool cooling = (now - lastOpenAt < ACTION_COOLDOWN_MS);
            bool enterEdge = overBtn && !overBtnPrev && !cooling;

            // The flyout state is only needed on the cursor-enter edge (to
            // avoid toggling an open flyout closed) and while auto-collapse is
            // watching an open flyout. When idle, throttle the check so a
            // manually opened flyout is still noticed without paying a
            // FindWindowW call on every tick.
            HWND flyoutHwnd = nullptr;
            if (enterEdge || (s.autoClose && flyoutBelievedOpen)) {
                flyoutHwnd = GetVisibleFlyout(s);
            } else if (s.autoClose && now >= nextIdleStateCheck) {
                flyoutHwnd = GetVisibleFlyout(s);
                nextIdleStateCheck = now + IDLE_STATE_CHECK_MS;
            }
            bool flyoutVisible = (flyoutHwnd != nullptr);
            flyoutBelievedOpen = flyoutVisible || cooling;

            // Open only on the cursor-enter edge and only when the flyout is
            // not already open. The chevron acts as a toggle, so any redundant
            // Invoke would close it again.
            if (enterEdge && !flyoutVisible) {
                DoExpand(pBtn);
                lastOpenAt = now;
                flyoutBelievedOpen = true;
                leftAt = 0;
                Wh_Log(L"OPEN");
            }
            overBtnPrev = overBtn;

            // Detect a click inside the flyout: the user is interacting with
            // an icon (e.g. its popup window just opened and took focus).
            // Suspend auto-collapse until the flyout closes on its own —
            // collapsing via Invoke would steal focus and dismiss the popup
            // the user just opened.
            bool anyBtnDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
                              (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ||
                              (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
            if (anyBtnDown && !anyBtnDownPrev && flyoutVisible &&
                PtOverWindow(flyoutHwnd, pt)) {
                clickedInFlyout = true;
                Wh_Log(L"Click inside flyout: auto-collapse suspended");
            }
            anyBtnDownPrev = anyBtnDown;
            if (!flyoutVisible && !cooling) {
                clickedInFlyout = false;
            }

            // Auto-collapse once the cursor left both the button and the flyout.
            if (s.autoClose && flyoutVisible && !cooling && !clickedInFlyout) {
                bool overFlyout = PtOverWindow(flyoutHwnd, pt);
                if (overBtn || overFlyout) {
                    leftAt = 0;
                } else if (leftAt == 0) {
                    leftAt = now;
                } else if (now - leftAt >= (ULONGLONG)s.grace) {
                    DoCollapse(pBtn);
                    leftAt = 0;
                    flyoutBelievedOpen = false;
                    Wh_Log(L"CLOSE after grace");
                }
            } else {
                leftAt = 0;
            }
        }

        // Interruptible sleep: WhTool_ModUninit signals g_stopEvent so the
        // thread wakes immediately regardless of the polling interval.
        WaitForSingleObject(g_stopEvent, s.pollInterval);
    }

    if (pBtn) pBtn->Release();
    if (pAuto) pAuto->Release();
    CoUninitialize();
    return 0;
}

// ---- Mod lifecycle (tool mod) ----

static void LoadSettings() {
    Settings s;

    s.autoClose = Wh_GetIntSetting(L"autoClose") != 0;
    s.pollInterval = (int)Wh_GetIntSetting(L"pollInterval");
    s.grace = (int)Wh_GetIntSetting(L"grace");
    s.pad = (int)Wh_GetIntSetting(L"pad");
    if (s.pollInterval < 10) s.pollInterval = 10;
    if (s.grace < 0) s.grace = 0;

    // Wh_GetStringSetting never returns NULL (it returns L"" on unset/error),
    // so only override the defaults with non-empty values.
    PCWSTR fc = Wh_GetStringSetting(L"flyoutClass");
    if (*fc) s.flyoutClass = fc;
    Wh_FreeStringSetting(fc);

    PCWSTR aid = Wh_GetStringSetting(L"trayIconAutomationId");
    if (*aid) s.trayIconAutomationId = aid;
    Wh_FreeStringSetting(aid);

    std::vector<std::wstring> keywords;
    for (int i = 0;; i++) {
        PCWSTR k = Wh_GetStringSetting(L"keywords[%d]", i);
        bool empty = !*k;
        if (!empty) keywords.push_back(k);
        Wh_FreeStringSetting(k);
        if (empty) break;
    }
    if (!keywords.empty()) s.keywords = std::move(keywords);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
    g_settingsGeneration++;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_running = true;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        g_running = false;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    g_running = false;
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        // Safe to wait without a timeout: the worker only blocks in the
        // interruptible wait above, so it exits promptly once signaled.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
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

    WCHAR commandLine[MAX_PATH + 2 +
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
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
        LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCWSTR,
        LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
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
