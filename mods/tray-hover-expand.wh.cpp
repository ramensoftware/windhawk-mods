// ==WindhawkMod==
// @id              tray-hover-expand
// @name            Tray hover expand
// @description     Open the hidden tray icons flyout on hover instead of clicking the chevron; optionally collapse it when the cursor leaves
// @version         1.7.0
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
Only tray elements are ever considered, so no taskbar application button can be
selected. Among those, detection goes:
1. By class name, which is language-independent: the chevron is the only tray
   element whose class name is `SystemTray.NormalButton` while its AutomationId
   is `SystemTrayIcon`. Notification icons share the class but use a different
   AutomationId, and the clock, volume, network, battery and "show desktop"
   buttons share the AutomationId but use other classes.
2. By name, in case a future Windows build renames those classes. The keyword
   list covers the most common display languages and can be extended.
3. Otherwise nothing happens. The mod does not guess, because invoking an
   unidentified tray button would open whatever it happens to be.

## Notes
- Windows 11 only. The class names this mod identifies the chevron by are
  Windows 11 shell types, so on Windows 10 nothing is identified and the mod
  does nothing.
- If the chevron is not identified, the mod logs every tray candidate it saw
  (class name, AutomationId, position, name). That log is what to attach to a
  bug report.
- Updating the mod does not change settings you have already saved. If you saved
  settings before v1.7.0 and your language is missing, delete every entry under
  "Chevron name keywords" and save: an empty list restores the built-in one.
- Set "Hover delay" to a small value (e.g. 150 ms) if the flyout opens when you
  only brush past the chevron on the way to the clock.
- If auto-collapse does not work, the flyout window class name may differ on your
  build. Change it in the "Flyout window class" setting.
- Windows shows a tooltip for the chevron: "Show hidden icons" before the flyout
  opens, which can appear ahead of it, and "Hide" once it is open, which can cover
  the bottom row of icons. Enable "Hide the chevron tooltip" to suppress both.
- By default the flyout is not opened while a fullscreen app is in the foreground
  (e.g. a fullscreen video or a game), so it can't pop up over the content. Turn
  off "Do not activate over fullscreen apps" to always activate.

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
Brane pod uwagę są wyłącznie elementy zasobnika, więc żaden przycisk aplikacji
z paska zadań nie może zostać wybrany. Wśród nich wykrywanie przebiega tak:
1. Po nazwie klasy, niezależnie od języka: strzałka jest jedynym elementem
   zasobnika o klasie `SystemTray.NormalButton` i jednocześnie AutomationId
   `SystemTrayIcon`. Ikony powiadomień mają tę samą klasę, ale inne
   AutomationId, a zegar, głośność, sieć, bateria i „pokaż pulpit" mają to samo
   AutomationId, ale inne klasy.
2. Po nazwie, na wypadek gdyby przyszła kompilacja Windows zmieniła te klasy.
   Lista słów kluczowych obejmuje najpopularniejsze języki i można ją rozszerzyć.
3. W przeciwnym razie nic się nie dzieje. Mod nie zgaduje, bo uruchomienie
   nierozpoznanego przycisku zasobnika otworzyłoby cokolwiek, czym on jest.

### Uwagi
- Tylko Windows 11. Nazwy klas, po których rozpoznawana jest strzałka, to typy
  powłoki Windows 11, więc w Windows 10 nic nie zostanie rozpoznane i mod nie
  robi nic.
- Gdy strzałka nie zostanie rozpoznana, mod zapisuje w logu wszystkich
  kandydatów z zasobnika (nazwa klasy, AutomationId, pozycja, nazwa). To ten log
  warto dołączyć do zgłoszenia błędu.
- Aktualizacja moda nie zmienia ustawień, które zostały już zapisane. Jeśli
  zapisywałeś ustawienia przed wersją 1.7.0, a brakuje Twojego języka, usuń
  wszystkie pozycje w polu „Słowa kluczowe nazwy strzałki" i zapisz: pusta lista
  przywraca wbudowaną.
- Ustaw „Opóźnienie otwierania" na niewielką wartość (np. 150 ms), jeśli schowek
  otwiera się przy samym przejeżdżaniu obok strzałki w drodze do zegara.
- Jeśli auto-zwijanie nie działa, nazwa klasy okna schowka może się różnić na
  Twojej kompilacji systemu. Zmień ją w ustawieniu „Klasa okna schowka".
- Windows pokazuje dla strzałki podpowiedź: „Pokaż ukryte ikony" przed otwarciem
  schowka, która potrafi wyprzedzić jego pojawienie się, oraz „Ukryj" po otwarciu,
  która zasłania dolny rząd ikon. Włącz „Ukryj podpowiedź strzałki", aby wyłączyć
  obie.
- Domyślnie schowek nie otwiera się, gdy na pierwszym planie jest aplikacja
  pełnoekranowa (np. film na pełnym ekranie lub gra), więc nie wyskoczy on na
  wierzchu treści. Wyłącz „Nie aktywuj na aplikacjach pełnoekranowych", aby
  aktywować zawsze.
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
- hoverDelay: 0
  $name: Hover delay (ms)
  $name:pl-PL: Opóźnienie otwierania (ms)
  $description: How long the cursor must stay on the chevron before the flyout opens. 0 opens immediately; a small value stops the flyout from opening when you only brush past the chevron on the way to the clock.
  $description:pl-PL: Jak długo kursor musi pozostać na strzałce, zanim schowek się otworzy. 0 otwiera natychmiast, a niewielka wartość zapobiega otwieraniu przy samym przejeżdżaniu obok strzałki w drodze do zegara.
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
- keywords: ["hidden icons", "ukryte ikony", "verborgen pictogrammen", "ausgeblendete symbole", "icônes masquées", "iconos ocultos", "icone nascoste", "ícones ocultos", "skryté ikony", "rejtett ikonok", "pictograme ascunse", "dolda ikoner", "skjulte ikoner", "piilotetut kuvakkeet", "gizli simgeleri", "скрытые значки", "приховані піктограми", "κρυφών εικονιδίων", "隐藏的图标", "隱藏的圖示", "隠れている", "숨겨진 아이콘"]
  $name: Chevron name keywords
  $name:pl-PL: Słowa kluczowe nazwy strzałki
  $description: Only used when the chevron cannot be identified by its class name. Case-insensitive substrings, matched against tray buttons only. The most common Windows display languages are covered by default.
  $description:pl-PL: Używane tylko wtedy, gdy strzałki nie da się rozpoznać po nazwie klasy. Fragmenty nazwy (wielkość liter bez znaczenia), porównywane wyłącznie z przyciskami zasobnika. Domyślnie pokryte są najpopularniejsze języki interfejsu Windows.
- chevronClass: SystemTray.NormalButton
  $name: Chevron class name
  $name:pl-PL: Nazwa klasy strzałki
  $description: Primary, language-independent identification. The chevron is the only tray element that has this class name together with the AutomationId below. Change it if a future Windows build renames it.
  $description:pl-PL: Podstawowe rozpoznawanie, niezależne od języka. Strzałka jest jedynym elementem zasobnika, który ma tę nazwę klasy razem z poniższym AutomationId. Zmień ją, jeśli przyszła kompilacja Windows zmieni nazwę.
- positionalFallback: false
  $name: Guess the chevron by position as a last resort
  $name:pl-PL: W ostateczności zgaduj strzałkę po pozycji
  $description: When the chevron matches neither the class name nor any keyword, assume it is the leftmost tray button. This is a guess and can select a different button, such as Quick Settings, so it is off by default and the mod simply does nothing instead.
  $description:pl-PL: Gdy strzałka nie pasuje ani do nazwy klasy, ani do żadnego słowa kluczowego, uznaje za nią pierwszy od lewej przycisk zasobnika. To zgadywanie i może trafić w inny przycisk, na przykład Szybkie ustawienia, dlatego domyślnie jest wyłączone, a mod po prostu nic nie robi.
- suppressInFullscreen: true
  $name: Do not activate over fullscreen apps
  $name:pl-PL: Nie aktywuj na aplikacjach pełnoekranowych
  $description: When a fullscreen app is in the foreground (e.g. a fullscreen video or a game), hovering the chevron's location will not open the flyout, so it can't pop up over the content.
  $description:pl-PL: Gdy na pierwszym planie jest aplikacja pełnoekranowa (np. film na pełnym ekranie lub gra), najechanie w miejsce strzałki nie otworzy schowka, więc nie wyskoczy on na wierzchu treści.
- hideTooltip: false
  $name: Hide the chevron tooltip
  $name:pl-PL: Ukryj podpowiedź strzałki
  $description: While the cursor is on the chevron, hide the tooltip Windows shows for it. That covers both "Show hidden icons", which can appear before the flyout does, and "Hide", which covers the bottom row of icons once it is open.
  $description:pl-PL: Gdy kursor jest na strzałce, ukrywa pokazywaną dla niej podpowiedź Windows. Dotyczy to zarówno „Pokaż ukryte ikony", która potrafi pojawić się przed schowkiem, jak i „Ukryj", która po otwarciu zasłania dolny rząd ikon.
- flyoutClass: TopLevelWindowForOverflowXamlIsland
  $name: Flyout window class
  $name:pl-PL: Klasa okna schowka
  $description: Window class name of the opened flyout (used to tell whether the cursor is over the icons). Change it if auto-collapse does not work.
  $description:pl-PL: Nazwa klasy okna otwartego schowka (służy do sprawdzania, czy kursor jest nad ikonami). Zmień ją, jeśli auto-zwijanie nie działa.
- tooltipClass: Xaml_WindowedPopupClass
  $name: Tooltip window class
  $name:pl-PL: Klasa okna podpowiedzi
  $description: Window class of the chevron tooltip, hidden only when "Hide the chevron tooltip" is enabled. Change it if hiding does not work on your build.
  $description:pl-PL: Klasa okna podpowiedzi strzałki, ukrywanej tylko gdy włączono „Ukryj podpowiedź strzałki". Zmień ją, jeśli ukrywanie nie działa na Twojej kompilacji.
- trayIconAutomationId: SystemTrayIcon
  $name: Chevron AutomationId
  $name:pl-PL: AutomationId strzałki
  $description: The AutomationId paired with "Chevron class name" to identify the chevron. It also tells tray elements apart from taskbar buttons. Change it if a future Windows build renames it.
  $description:pl-PL: AutomationId strzałki, używane razem z powyższą nazwą klasy. Służy też do odróżnienia elementów zasobnika od przycisków aplikacji na pasku zadań.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <atomic>
#include <string>
#include <string_view>
#include <vector>

struct Settings {
    bool autoClose = true;
    int pollInterval = 50;
    int hoverDelay = 0;
    int grace = 200;
    int pad = 4;
    bool hideTooltip = false;
    bool suppressInFullscreen = true;
    bool positionalFallback = false;
    std::wstring flyoutClass = L"TopLevelWindowForOverflowXamlIsland";
    std::wstring tooltipClass = L"Xaml_WindowedPopupClass";
    std::wstring trayIconAutomationId = L"SystemTrayIcon";
    std::wstring chevronClass = L"SystemTray.NormalButton";
    // Fragments of the chevron's name ("Show hidden icons") across the most
    // common Windows display languages. Each entry is a distinctive part of the
    // name rather than the whole string, so wording differences between builds
    // still match.
    std::vector<std::wstring> keywords = {
        L"hidden icons",            // English
        L"ukryte ikony",            // Polish
        L"verborgen pictogrammen",  // Dutch
        L"ausgeblendete symbole",   // German
        L"icônes masquées",         // French
        L"iconos ocultos",          // Spanish
        L"icone nascoste",          // Italian
        L"ícones ocultos",          // Portuguese
        L"skryté ikony",            // Czech, Slovak
        L"rejtett ikonok",          // Hungarian
        L"pictograme ascunse",      // Romanian
        L"dolda ikoner",            // Swedish
        L"skjulte ikoner",          // Danish, Norwegian
        L"piilotetut kuvakkeet",    // Finnish
        L"gizli simgeleri",         // Turkish
        L"скрытые значки",          // Russian
        L"приховані піктограми",    // Ukrainian
        L"κρυφών εικονιδίων",       // Greek
        L"隐藏的图标",                // Chinese (Simplified)
        L"隱藏的圖示",                // Chinese (Traditional)
        L"隠れている",                // Japanese
        L"숨겨진 아이콘"              // Korean
    };
    // Lowercased copy of `keywords`, built once in LoadSettings so that name
    // matching does not lowercase every keyword for every candidate element.
    std::vector<std::wstring> keywordsLower;
};

// UIA class names of tray elements all share this prefix, which is what makes
// it possible to exclude the rest of the taskbar from the search.
// Every tray element's class name starts with this; taskbar buttons do not.
static constexpr std::wstring_view TRAY_CLASS_PREFIX = L"SystemTray.";

// Some builds are reported to expose the chevron with this AutomationId instead
// of the configured one. Accepting both costs nothing, because the class name
// plus AutomationId pair is only used when exactly one element matches it.
// Deliberately fixed rather than exposed as a setting: the configurable value
// is the one a user would have to change on a future build, and this one only
// widens what already matches.
static constexpr std::wstring_view CHEVRON_AUTOMATION_ID_ALT = L"ChevronButton";

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

// CharLowerBuffW is used instead of towlower because the latter follows the C
// locale and would leave non-ASCII letters (Cyrillic, Greek, ...) untouched,
// breaking case-insensitive matching for those locales.
static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    if (!r.empty()) {
        CharLowerBuffW(&r[0], (DWORD)r.size());
    }
    return r;
}

static bool NameMatches(const std::wstring& name, const Settings& s) {
    std::wstring low = ToLower(name);
    for (const auto& k : s.keywordsLower) {
        if (!k.empty() && low.find(k) != std::wstring::npos) return true;
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

// logCandidates asks for a one-off diagnostic dump when identification fails;
// *logged reports whether one was actually emitted, so that the caller only
// counts a streak as reported when there was something to report. logWeakMatch
// is a separate opt-in for the "identified, but not by class name" lines, which
// the caller silences after the first one: on a build where the class match
// never works, they would otherwise be logged on every acquisition.
static IUIAutomationElement* FindOverflowButton(IUIAutomation* pAuto,
                                                const Settings& s,
                                                bool logCandidates,
                                                bool* logged,
                                                bool* logWeakMatch) {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return nullptr;

    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(pAuto->ElementFromHandle(hTaskbar, &pRoot)) || !pRoot) return nullptr;

    IUIAutomationCondition* pCond = nullptr;
    VARIANT v; VariantInit(&v);
    v.vt = VT_I4; v.lVal = UIA_ButtonControlTypeId;
    pAuto->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &pCond);

    // Reading five properties per button one at a time is one cross-process call
    // each, i.e. a few hundred round trips per walk. A cache request collapses
    // them into the single FindAllBuildCache call. The default element mode is
    // Full, so the returned elements still support GetCurrentPatternAs, which is
    // what DoExpand and DoCollapse need.
    IUIAutomationCacheRequest* pCache = nullptr;
    if (SUCCEEDED(pAuto->CreateCacheRequest(&pCache)) && pCache) {
        pCache->AddProperty(UIA_ClassNamePropertyId);
        pCache->AddProperty(UIA_AutomationIdPropertyId);
        pCache->AddProperty(UIA_NamePropertyId);
        pCache->AddProperty(UIA_IsOffscreenPropertyId);
        pCache->AddProperty(UIA_BoundingRectanglePropertyId);
    }
    const bool cached = (pCache != nullptr);

    // The Shell_TrayWnd subtree contains every taskbar button: Start, Search,
    // the task list entries, and only then the tray. Candidates are therefore
    // restricted to tray elements first, so that neither a name keyword nor the
    // positional fallback can ever select an application button (whose name is
    // the window title and can contain anything).
    //
    // Three-step detection, most reliable first:
    //  1) class name + AutomationId, which is language-independent: the chevron
    //     is the only tray element that is a `chevronClass` button carrying
    //     `trayIconAutomationId`. Notification icons share the class but use a
    //     different AutomationId, and the clock, volume, network, battery and
    //     "show desktop" buttons share the AutomationId but use other classes.
    //  2) match by name, for builds where those class names change.
    //  3) nothing: when the chevron cannot be identified, no element is
    //     returned. Invoking an unidentified tray button would open whatever it
    //     happens to be, which is how the chevron was mistaken for Quick
    //     Settings in the field, so guessing is opt-in only.
    struct Candidate {
        IUIAutomationElement* el;
        std::wstring className;
        std::wstring automationId;
        std::wstring name;
        RECT rect;
    };
    std::vector<Candidate> cands;

    IUIAutomationElementArray* pArr = nullptr;
    HRESULT hrFind = pCond
        ? (cached ? pRoot->FindAllBuildCache(TreeScope_Subtree, pCond, pCache, &pArr)
                  : pRoot->FindAll(TreeScope_Subtree, pCond, &pArr))
        : E_FAIL;
    if (SUCCEEDED(hrFind) && pArr) {
        int n = 0; pArr->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* e = nullptr;
            if (FAILED(pArr->GetElement(i, &e)) || !e) continue;

            BSTR cls = nullptr;
            if (cached) e->get_CachedClassName(&cls); else e->get_CurrentClassName(&cls);
            std::wstring className = cls ? cls : L"";
            if (cls) SysFreeString(cls);

            BSTR aid = nullptr;
            if (cached) e->get_CachedAutomationId(&aid); else e->get_CurrentAutomationId(&aid);
            std::wstring automationId = aid ? aid : L"";
            if (aid) SysFreeString(aid);

            // Only tray elements are eligible; everything else on the taskbar
            // is discarded before any name or position test is applied. Without
            // this, a task list button (named after its window title) could
            // match a keyword and then be invoked.
            bool isTrayElement = className.starts_with(TRAY_CLASS_PREFIX) ||
                                 automationId == s.trayIconAutomationId ||
                                 automationId == CHEVRON_AUTOMATION_ID_ALT;
            if (!isTrayElement) { e->Release(); continue; }

            // Elements that are not rendered report an empty {0,0,0,0}
            // rectangle, which would otherwise pass every geometric test.
            BOOL offscreen = FALSE;
            RECT r{};
            HRESULT hrOff = cached ? e->get_CachedIsOffscreen(&offscreen)
                                   : e->get_CurrentIsOffscreen(&offscreen);
            HRESULT hrRect = cached ? e->get_CachedBoundingRectangle(&r)
                                    : e->get_CurrentBoundingRectangle(&r);
            if (FAILED(hrOff) || offscreen || FAILED(hrRect) ||
                r.right <= r.left || r.bottom <= r.top) {
                e->Release();
                continue;
            }

            BSTR nm = nullptr;
            if (cached) e->get_CachedName(&nm); else e->get_CurrentName(&nm);
            std::wstring name = nm ? nm : L"";
            if (nm) SysFreeString(nm);

            cands.push_back({e, className, automationId, name, r});
        }
        pArr->Release();
    }
    if (pCache) pCache->Release();
    if (pCond) pCond->Release();
    pRoot->Release();

    int chosen = -1;

    // 1) Language-independent identification, accepted only when it is
    // unambiguous. If a future build gives several tray elements this same
    // signature, picking one of them arbitrarily would be a guess.
    int classMatches = 0, firstClassMatch = -1;
    for (size_t i = 0; i < cands.size(); i++) {
        if (cands[i].className == s.chevronClass &&
            (cands[i].automationId == s.trayIconAutomationId ||
             cands[i].automationId == CHEVRON_AUTOMATION_ID_ALT)) {
            classMatches++;
            if (firstClassMatch < 0) firstClassMatch = (int)i;
        }
    }
    if (classMatches == 1) {
        chosen = firstClassMatch;
    } else if (classMatches > 1 && logWeakMatch) {
        *logWeakMatch = true;
        Wh_Log(L"%d tray elements share the chevron signature, falling back to name",
               classMatches);
    }

    // 2) Name match, for builds where the class names change.
    if (chosen < 0) {
        for (size_t i = 0; i < cands.size(); i++) {
            if (NameMatches(cands[i].name, s)) {
                chosen = (int)i;
                if (logWeakMatch) {
                    *logWeakMatch = true;
                    Wh_Log(L"Chevron matched by name, not by class name");
                }
                break;
            }
        }
    }

    // 3) Optional positional guess, off by default.
    if (chosen < 0 && s.positionalFallback) {
        for (size_t i = 0; i < cands.size(); i++) {
            // Same admission test as isTrayElement, so a build exposing the
            // chevron under the alternate id still has candidates here.
            if (cands[i].automationId != s.trayIconAutomationId &&
                cands[i].automationId != CHEVRON_AUTOMATION_ID_ALT) continue;
            if (chosen < 0 || cands[i].rect.left < cands[chosen].rect.left) {
                chosen = (int)i;
            }
        }
        if (chosen >= 0 && logWeakMatch) {
            *logWeakMatch = true;
            Wh_Log(L"Chevron guessed by position: name=%s class=%s x=%d",
                   cands[chosen].name.c_str(), cands[chosen].className.c_str(),
                   (int)cands[chosen].rect.left);
        }
    }

    // Nothing identified: dump the candidates so that a single log makes the
    // next unsupported build actionable, and do not touch anything.
    // Logged once per failure streak: the retry runs every few seconds, and
    // repeating the whole table would bury the rest of the log. An empty table
    // just means nothing was rendered (a retracted taskbar), which needs no
    // diagnosis, so it is not worth a line.
    if (chosen < 0 && logCandidates && !cands.empty()) {
        if (logged) *logged = true;
        Wh_Log(L"Chevron not identified among %d tray candidates:", (int)cands.size());
        for (size_t i = 0; i < cands.size(); i++) {
            Wh_Log(L"  [%d] class=%s aid=%s x=%d name=%s", (int)i,
                   cands[i].className.c_str(), cands[i].automationId.c_str(),
                   (int)cands[i].rect.left, cands[i].name.c_str());
        }
    }

    IUIAutomationElement* result = nullptr;
    for (size_t i = 0; i < cands.size(); i++) {
        if ((int)i == chosen) {
            result = cands[i].el;
        } else {
            cands[i].el->Release();
        }
    }
    return result;
}

static bool PtInRectPad(const RECT& r, POINT pt, int pad) {
    return pt.x >= r.left - pad && pt.x <= r.right + pad &&
           pt.y >= r.top - pad  && pt.y <= r.bottom + pad;
}

static const ULONGLONG ACTION_COOLDOWN_MS = 300;
// Retry the (expensive) chevron lookup quickly while the cursor is at the
// taskbar, and only occasionally while it is elsewhere. See the worker loop.
static const ULONGLONG REFIND_NEAR_MS = 250;
static const ULONGLONG REFIND_MAX_MS = 4000;
// How far from the taskbar the cursor still counts as "at the taskbar". Covers
// the sliver an auto-hidden taskbar leaves at the screen edge while it slides in.
static const int TASKBAR_REVEAL_PAD = 32;
static const ULONGLONG RECT_REFRESH_MS = 750;
static const ULONGLONG IDLE_STATE_CHECK_MS = 500;

// Cheap (pure Win32) resolution of the flyout window — no UIA calls. The
// chevron does not support the ExpandCollapse pattern anyway, so the flyout
// window's visibility is the only reliable state signal. Returns the window
// handle if the flyout is open, or nullptr otherwise. Resolving the handle
// once per tick lets callers reuse it instead of each calling FindWindowW.
// Only windows owned by the taskbar count. The class is user-settable, so a
// user pointing it at a more generic class must not make the mod track another
// application's window.
static HWND GetVisibleFlyout(const Settings& s, DWORD taskbarPid) {
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.flyoutClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        if (taskbarPid) {
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid != taskbarPid) continue;
        }
        return h;
    }
    return nullptr;
}

// A tray icon's context menu is a separate window that usually extends past the
// flyout, so the cursor sitting on it counts as having left the flyout. Menus
// opened with TrackPopupMenu, which is what most tray icons use, all share this
// window class, so their presence is a reliable "the user is still busy" signal
// even when the click that opened the menu was never observed.
static bool IsPopupMenuOpen() {
    // Enumerate rather than sampling the first hit: menu windows are created
    // per thread and kept alive hidden afterwards, so the first one in Z-order
    // is often a leftover from some process that showed a menu earlier, and
    // testing only that one would report "no menu" while one is on screen.
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, L"#32768", nullptr))) {
        if (IsWindowVisible(h)) return true;
    }
    return false;
}

static bool PtOverWindow(HWND hwnd, POINT pt) {
    if (!hwnd) return false;
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return false;
    return PtInRect(&r, pt);
}

// Hide the chevron's "Hide" tooltip, which Windows pops up over the chevron
// while the flyout is open and can cover the bottom row of icons. The tooltip
// is a separate window class from the flyout, but that class is a generic XAML
// popup host, so only windows positioned over the chevron (horizontally
// overlapping it and sitting at or above its top, i.e. inside the flyout zone)
// are hidden — never popups elsewhere on screen.
// The tooltip class is the generic WinUI popup host, used by many apps, so only
// windows owned by the taskbar's own process are eligible. Without that check,
// another application's popup could be hidden with no way for the user to bring
// it back.
// The tooltip is matched against the flyout rather than against "above the
// chevron", because which side it sits on depends on where the taskbar is: with
// the taskbar on top, both the flyout and the tooltip are below the chevron.
static void HideChevronTooltip(const Settings& s, const RECT& chevron,
                               HWND flyout, DWORD taskbarPid) {
    if (!taskbarPid) return;
    RECT fly{};
    bool hasFlyout = flyout && GetWindowRect(flyout, &fly);

    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.tooltipClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        if (pid != taskbarPid) continue;
        RECT r;
        if (!GetWindowRect(h, &r)) continue;
        // Horizontally aligned with the chevron, and either overlapping the
        // flyout (the "Hide" tooltip shown while it is open) or sitting right
        // next to the chevron (the "Show hidden icons" tooltip, which can beat
        // the flyout to the screen). Both tests are direction-agnostic, so they
        // hold with the taskbar at the bottom and at the top.
        if (h == flyout) continue;
        bool overlapsX = r.left <= chevron.right && r.right >= chevron.left;
        bool overlapsFlyout = hasFlyout &&
                              r.left < fly.right && r.right > fly.left &&
                              r.top < fly.bottom && r.bottom > fly.top;
        LONG band = chevron.bottom - chevron.top;
        LONG gapAbove = chevron.top - r.bottom;
        LONG gapBelow = r.top - chevron.bottom;
        bool besideChevron = (gapAbove >= 0 && gapAbove <= band) ||
                             (gapBelow >= 0 && gapBelow <= band);
        if (overlapsX && (overlapsFlyout || besideChevron)) {
            // Async: the window belongs to explorer, and the synchronous form
            // marshals into its UI thread and blocks until that thread handles
            // it. Hiding a tooltip a frame later is not noticeable; stalling
            // the poll loop behind a busy shell is.
            ShowWindowAsync(h, SW_HIDE);
        }
    }
}

// True when the foreground window covers the whole monitor that the chevron is
// on, i.e. a fullscreen app (a fullscreen video, a game, etc.) whose content the
// flyout would pop up over. Maximized windows stop at the work area and
// therefore do not match, and a fullscreen app on another monitor does not
// suppress a taskbar that is fully visible here. The desktop and shell windows
// are excluded so an empty desktop is not mistaken for a fullscreen app.
static bool IsFullscreenOverChevron(const RECT& chevron) {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetShellWindow()) return false;

    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0) {
            return false;
        }
    }

    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (mon != MonitorFromRect(&chevron, MONITOR_DEFAULTTONEAREST)) return false;

    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return false;
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) return false;
    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
}

// ---- Worker thread ----

static DWORD WINAPI WorkerThread(LPVOID) {
    // UIA bounding rectangles are physical screen coordinates. Without per
    // monitor v2 awareness, GetCursorPos and GetWindowRect would be virtualized
    // on mixed-DPI setups and the hover test would compare different spaces.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

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
    ULONGLONG lastActionAt = 0;
    ULONGLONG insideSince = 0;
    DWORD taskbarPid = 0;
    bool overBtnPrev = false;
    bool dwellFired = false;
    bool loggedCandidates = false;
    // Survives a successful find, unlike loggedCandidates, so that a build
    // which always matches by name or by position logs that once rather than
    // on every acquisition.
    bool loggedWeakMatch = false;
    ULONGLONG lastWalkAt = 0;
    bool clickedInFlyout = false;
    bool anyBtnDownPrev = false;
    bool nearTaskbarPrev = false;
    int refindFailures = 0;

    while (g_running) {
        ULONGLONG now = GetTickCount64();

        // Read the generation before taking the snapshot. LoadSettings publishes
        // the settings and only then bumps the counter, so reading it afterwards
        // could record an update as already applied and drop it.
        int gen = g_settingsGeneration;
        if (gen != settingsGen) {
            s = GetSettingsSnapshot();
            settingsGen = gen;
            // Settings may change how the chevron is detected, so drop the
            // cached element and re-detect promptly with the new settings.
            if (pBtn) { pBtn->Release(); pBtn = nullptr; }
            haveRect = false;
            overBtnPrev = false;
            insideSince = 0;
            dwellFired = false;
            leftAt = 0;             // don't collapse on the first tick back
            flyoutBelievedOpen = false;
            clickedInFlyout = false;
            nextRefind = 0;
            loggedCandidates = false;
            loggedWeakMatch = false;
        }

        POINT pt; GetCursorPos(&pt);

        // The "pressed since the previous call" bit is desktop-wide state that
        // the first caller consumes, so polling it continuously would take it
        // away from every other application for the whole session. It is only
        // ever read while auto-collapse is watching an open flyout, so sample
        // it exactly then: staleness stays bounded by the polling interval for
        // as long as the result can matter, and the rest of the time the mod
        // does not touch it at all. flyoutBelievedOpen still holds the previous
        // tick's value here, which is what makes the first tick covered too.
        bool anyBtnDown = false;
        bool pressedSinceTick = false;
        if (s.autoClose && flyoutBelievedOpen) {
            SHORT kl = GetAsyncKeyState(VK_LBUTTON);
            SHORT kr = GetAsyncKeyState(VK_RBUTTON);
            SHORT km = GetAsyncKeyState(VK_MBUTTON);
            anyBtnDown = ((kl | kr | km) & 0x8000) != 0;
            pressedSinceTick = ((kl | kr | km) & 0x0001) != 0;
        }
        // Recorded here rather than at the end of the loop, which a `continue`
        // can skip: that would keep a two-ticks-old value and let the next tick
        // see a press edge that never happened.
        bool anyBtnDownEdge = anyBtnDown && !anyBtnDownPrev;
        anyBtnDownPrev = anyBtnDown;

        // Lazily (re-)find the button only when we don't have a valid one. A
        // destroyed or stale element makes the rectangle query below fail,
        // which clears pBtn and triggers a re-find — so there is no need for a
        // periodic cross-process subtree walk while the element is valid.
        //
        // Re-finding is driven by the cursor rather than by a fixed timer. The
        // rectangle guard drops the element whenever the chevron stops being
        // rendered, which for an auto-hiding taskbar happens on every retract,
        // so a fixed interval would leave the mod idle for up to that interval
        // after each reveal. Conversely, when nothing is hidden the chevron does
        // not exist at all and a timer would walk the taskbar subtree forever.
        // Retry while the cursor is at the taskbar, and not at all while it is
        // elsewhere, since the chevron cannot be hovered from there anyway.
        //
        // Consecutive failures back off. Without that, a user with no hidden
        // icons has no chevron to find, so every lookup fails and the fast
        // cadence would walk the taskbar subtree several times a second for the
        // whole session, forcing explorer to build automation peers on its UI
        // thread exactly while the user is working with the taskbar. The
        // counter resets when the chevron is found and when the cursor enters
        // the band, so an auto-hiding taskbar still re-acquires on the first or
        // second try after a reveal.
        RECT tb;
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        // At least as wide as the hit area, so there is no position the mod
        // treats as "on the chevron" but not as "at the taskbar".
        int revealPad = s.pad > TASKBAR_REVEAL_PAD ? s.pad : TASKBAR_REVEAL_PAD;
        bool nearTaskbar = hTaskbar && GetWindowRect(hTaskbar, &tb) &&
                           PtInRectPad(tb, pt, revealPad);
        if (nearTaskbar && !nearTaskbarPrev) {
            refindFailures = 0;
            nextRefind = 0;
        }
        nearTaskbarPrev = nearTaskbar;

        // The band-entry reset above clears the backoff, so a cursor crossing
        // the boundary repeatedly would otherwise earn one full walk per
        // crossing. This floor keeps the worst case at the fast cadence while
        // still re-acquiring promptly after an auto-hide reveal.
        if (!pBtn && nearTaskbar && now >= nextRefind &&
            now - lastWalkAt >= REFIND_NEAR_MS) {
            lastWalkAt = now;
            bool didLog = false;
            bool didLogWeak = false;
            pBtn = FindOverflowButton(pAuto, s, !loggedCandidates, &didLog,
                                      loggedWeakMatch ? nullptr : &didLogWeak);
            loggedCandidates = pBtn ? false : (loggedCandidates || didLog);
            loggedWeakMatch = loggedWeakMatch || didLogWeak;
            nextRectRefresh = 0;        // force a fresh rectangle below
            taskbarPid = 0;
            GetWindowThreadProcessId(hTaskbar, &taskbarPid);
            if (pBtn) {
                refindFailures = 0;
            } else {
                ULONGLONG delay = REFIND_NEAR_MS << (refindFailures < 4 ? refindFailures : 4);
                if (delay > REFIND_MAX_MS) delay = REFIND_MAX_MS;
                if (refindFailures < 100) refindFailures++;
                nextRefind = now + delay;
            }
        }

        if (pBtn) {
            // Expensive and RARE: query the button rectangle through UIA only
            // periodically, not every tick. The chevron rarely moves.
            // Only while the cursor is at the taskbar: elsewhere the refreshed
            // rectangle cannot change any decision, so paying two cross-process
            // calls a second forever would be pure waste.
            if (nearTaskbar && now >= nextRectRefresh) {
                // An element that is alive but not currently rendered (the
                // taskbar auto-hid, or the last hidden icon was removed) returns
                // S_OK with an empty rectangle rather than an error. Keeping it
                // would turn the top-left corner of the screen into a hover
                // hotspot, and the alternating valid/empty rectangle under a
                // stationary cursor is what makes the flyout cycle open and
                // closed. Treat it exactly like a stale element.
                RECT r{};
                BOOL offscreen = FALSE;
                if (SUCCEEDED(pBtn->get_CurrentBoundingRectangle(&r)) &&
                    r.right > r.left && r.bottom > r.top &&
                    SUCCEEDED(pBtn->get_CurrentIsOffscreen(&offscreen)) &&
                    !offscreen) {
                    cachedRect = r;
                    haveRect = true;
                } else {
                    pBtn->Release(); pBtn = nullptr;
                    haveRect = false;
                    overBtnPrev = false;    // stale once the button is gone
                    insideSince = 0;
                    dwellFired = false;
                    leftAt = 0;             // don't collapse on the first tick back
                    // Nothing recomputes these without haveRect, and the mouse
                    // sampling gate reads flyoutBelievedOpen, so leaving it set
                    // would resume polling the shared key state indefinitely.
                    flyoutBelievedOpen = false;
                    clickedInFlyout = false;
                    nextRefind = 0;
                    WaitForSingleObject(g_stopEvent, s.pollInterval);
                    continue;
                }
                nextRectRefresh = now + RECT_REFRESH_MS;
            }
        }

        if (haveRect) {
            // Cheap and EVERY TICK: only local Win32 calls.
            bool overBtn = PtInRectPad(cachedRect, pt, s.pad);
            bool cooling = (now - lastActionAt < ACTION_COOLDOWN_MS);

            // The cursor must remain on the chevron for the hover delay before
            // the flyout opens, and each stay produces at most one attempt. With
            // the default delay of 0 this is the plain enter edge.
            if (!overBtn) {
                insideSince = 0;
                dwellFired = false;
            } else if (!overBtnPrev) {
                insideSince = now;
                dwellFired = false;
            }
            bool enterEdge = overBtn && !dwellFired && !cooling &&
                             now - insideSince >= (ULONGLONG)s.hoverDelay;

            // The flyout state is only needed on the cursor-enter edge (to
            // avoid toggling an open flyout closed) and while auto-collapse is
            // watching an open flyout. When idle, throttle the check so a
            // manually opened flyout is still noticed without paying a
            // FindWindowW call on every tick.
            HWND flyoutHwnd = nullptr;
            bool wantState = enterEdge
                          || (s.autoClose && flyoutBelievedOpen)
                          || (s.hideTooltip && overBtn);
            if (wantState) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
            } else if (s.autoClose && now >= nextIdleStateCheck) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
                nextIdleStateCheck = now + IDLE_STATE_CHECK_MS;
            }
            bool flyoutVisible = (flyoutHwnd != nullptr);
            flyoutBelievedOpen = flyoutVisible || cooling;

            // A stay that has already seen an open flyout counts as served, so
            // re-opening requires leaving and re-entering the hit area. Without
            // this, moving up into the icons and back down onto the chevron
            // leaves the enter edge armed, and the click that dismisses the
            // flyout is immediately followed by the mod opening it again.
            if (overBtn && flyoutVisible) {
                dwellFired = true;
            }

            // Suppress the chevron's tooltip while the cursor is on it: both
            // the "Hide" tooltip covering the bottom row of icons once the
            // flyout is open, and the "Show hidden icons" one that can appear
            // before the flyout does.
            if (s.hideTooltip && overBtn) {
                HideChevronTooltip(s, cachedRect, flyoutHwnd, taskbarPid);
            }

            // Open only on the cursor-enter edge and only when the flyout is
            // not already open. The chevron acts as a toggle, so any redundant
            // Invoke would close it again. Skip activation over a fullscreen
            // app so the flyout can't pop up on top of a video or a game (the
            // taskbar is hidden there, but the cached chevron rect still matches
            // that screen area). Checked only on the edge, so it costs nothing
            // on idle ticks.
            if (enterEdge && !flyoutVisible) {
                dwellFired = true;      // at most one attempt per stay
                if (!(s.suppressInFullscreen && IsFullscreenOverChevron(cachedRect))) {
                    DoExpand(pBtn);
                    lastActionAt = now;
                    flyoutBelievedOpen = true;
                    leftAt = 0;
                    Wh_Log(L"OPEN");
                }
            }
            overBtnPrev = overBtn;

            // Detect a click inside the flyout: the user is interacting with
            // an icon (e.g. its popup window just opened and took focus).
            // Suspend auto-collapse until the flyout closes on its own —
            // collapsing via Invoke would steal focus and dismiss the popup
            // the user just opened.
            if ((pressedSinceTick || anyBtnDownEdge) &&
                flyoutVisible && PtOverWindow(flyoutHwnd, pt)) {
                clickedInFlyout = true;
                Wh_Log(L"Click inside flyout: auto-collapse suspended");
            }
            if (!flyoutVisible && !cooling) {
                clickedInFlyout = false;
            }

            // Auto-collapse once the cursor left both the button and the flyout.
            if (s.autoClose && flyoutVisible && !cooling && !clickedInFlyout) {
                bool overFlyout = PtOverWindow(flyoutHwnd, pt);
                if (overBtn || overFlyout || IsPopupMenuOpen()) {
                    leftAt = 0;
                } else if (leftAt == 0) {
                    leftAt = now;
                } else if (now - leftAt >= (ULONGLONG)s.grace) {
                    // The collapse is a second Invoke, i.e. a toggle, and the
                    // flyout does not disappear instantly. Without arming the
                    // cooldown here, the next ticks would still see it open and
                    // invoke the chevron again, re-opening what was just closed.
                    DoCollapse(pBtn);
                    lastActionAt = now;
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
    s.hoverDelay = (int)Wh_GetIntSetting(L"hoverDelay");
    s.grace = (int)Wh_GetIntSetting(L"grace");
    s.pad = (int)Wh_GetIntSetting(L"pad");
    s.hideTooltip = Wh_GetIntSetting(L"hideTooltip") != 0;
    s.suppressInFullscreen = Wh_GetIntSetting(L"suppressInFullscreen") != 0;
    s.positionalFallback = Wh_GetIntSetting(L"positionalFallback") != 0;
    if (s.pad < 0) s.pad = 0;
    if (s.pad > 64) s.pad = 64;     // a large pad turns much of the screen into
                                    // a hover hotspot; a negative one shrinks
                                    // the hit area below the button itself
    if (s.pollInterval < 10) s.pollInterval = 10;
    if (s.hoverDelay < 0) s.hoverDelay = 0;
    if (s.grace < 0) s.grace = 0;

    // Wh_GetStringSetting never returns NULL (it returns L"" on unset/error),
    // so only override the defaults with non-empty values.
    PCWSTR fc = Wh_GetStringSetting(L"flyoutClass");
    if (*fc) s.flyoutClass = fc;
    Wh_FreeStringSetting(fc);

    PCWSTR tc = Wh_GetStringSetting(L"tooltipClass");
    if (*tc) s.tooltipClass = tc;
    Wh_FreeStringSetting(tc);

    PCWSTR aid = Wh_GetStringSetting(L"trayIconAutomationId");
    if (*aid) s.trayIconAutomationId = aid;
    Wh_FreeStringSetting(aid);

    PCWSTR cc = Wh_GetStringSetting(L"chevronClass");
    if (*cc) s.chevronClass = cc;
    Wh_FreeStringSetting(cc);

    std::vector<std::wstring> keywords;
    for (int i = 0;; i++) {
        PCWSTR k = Wh_GetStringSetting(L"keywords[%d]", i);
        bool empty = !*k;
        if (!empty) keywords.push_back(k);
        Wh_FreeStringSetting(k);
        if (empty) break;
    }
    if (!keywords.empty()) s.keywords = std::move(keywords);

    s.keywordsLower.reserve(s.keywords.size());
    for (const auto& k : s.keywords) s.keywordsLower.push_back(ToLower(k));

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
        // Safe to wait without a timeout: the stop event interrupts the poll
        // sleep, and the cross-process UIA calls the worker can be inside have
        // their own timeouts, so the wait is bounded even if the shell is busy.
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
