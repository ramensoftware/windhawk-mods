// ==WindhawkMod==
// @id              mouse-button-remap
// @name            Mouse Button Remapper
// @description     Remap mouse buttons (side, middle, left, right) to keyboard keys with optional modifiers, double-click actions, a master toggle hotkey and game mode
// @version         1.7.2
// @author          wygodad
// @github          https://github.com/wygodad
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mouse Button Remapper

![Demo](https://i.imgur.com/z2ndWbk.png)

Remaps mouse buttons to keyboard keys, optionally with modifiers (Ctrl,
Shift, Alt, Win). Supported buttons:

- **Upper side button** = "Forward" (XButton2) — on most mice this is the
  side button closer to the front of the mouse.
- **Lower side button** = "Back" (XButton1).
- **Middle button** = pressing the scroll wheel.
- **Left button** and **Right button** — see the warning below.

Each button has its own on/off switch — when a button is disabled, it
keeps its original function. If your mouse has the side buttons swapped,
simply configure the other one.

The mod runs as a tool mod in a dedicated process and installs a global
low-level mouse hook, so it works system-wide (in every application)
without injecting into the shell or other programs.

## Mouse compatibility

By default, on virtually all mice the two side buttons are reported as
standard mouse buttons — **Forward** (XButton2) and **Back** (XButton1) —
and the middle button as the wheel click. Those are exactly the events
this mod intercepts, so it works out of the box on a normal mouse with no
vendor software.

However, if the mouse's own software or onboard firmware remaps a button
to a **multimedia or keyboard function** (e.g. "Refresh", "Play/Pause",
"Browser Back"), that press is no longer sent as a mouse button — it
arrives as a media/keyboard command that a mouse hook cannot see, so this
mod can no longer remap it. If a button is not being intercepted, open
your mouse software and set it back to the plain **Forward** / **Back** /
**Wheel click** function, then assign the keyboard key here in the mod.
In other words: let the mouse send a normal button, and let the mod do
the remapping — do not remap the same button in both places.

## Configuration

Each button has its own section in the mod settings:

1. **Enabled** — the master switch for that button. When unchecked, the
   button behaves exactly as the manufacturer intended and all other
   fields in the section are ignored. This lets you remap only one button
   and leave the rest untouched.

2. **Key** — the keyboard key sent while the button is held. Pick one
   from the dropdown (F1–F12, Enter, Esc, arrows, media keys, …). The
   key press mirrors the button press: key-down when you press the
   button, key-up when you release it. The original button action is
   suppressed only when the remap is active.

3. **Custom key** — used only when **Key** is set to
   *Custom (use field below)*. It lets you type any key the dropdown
   does not list. Accepted values:
   - `F1` … `F24`
   - single letters and digits: `A` … `Z`, `0` … `9`
   - named keys: `Enter`, `Esc`, `Tab`, `Space`, `Backspace`, `Delete`,
     `Insert`, `Home`, `End`, `PageUp`, `PageDown`, `Up`, `Down`,
     `Left`, `Right`, `PlayPause`, `Mute`, `VolumeUp`, `VolumeDown`
   - any Windows virtual-key code in hex, e.g. `0x74` (= F5) — see the
     "Virtual-Key Codes" page in Microsoft docs for the full list
   - a full combo such as `Ctrl+F5` or `Ctrl + Shift + T` — modifiers
     typed here are combined with the **Modifier** dropdown selection

   If the value cannot be parsed, the remap is inactive and the button
   keeps its original function (a note is written to the mod's log,
   visible via "Show log" in Windhawk).

4. **Modifier** — keys held together with the main key, e.g. selecting
   `Ctrl` with key `F5` sends Ctrl+F5 (hard refresh in browsers).

5. **Double-click action** — optionally send a different key on
   double-click (e.g. single click = F5, double click = Ctrl+Shift+T).
   The double-click key/custom key/modifier fields work exactly like the
   single-click ones. Note: while this is enabled, the single-click
   action fires only after the system double-click interval passes
   without a second click, so it has a short delay; press-and-hold
   mirroring is also disabled for that button. When disabled (default),
   single clicks are instant.

Changes are applied immediately after saving the settings — no restart
needed.

## Master toggle hotkey

An optional global keyboard shortcut that suspends/resumes all remaps at
once — handy when the buttons' original functions are temporarily
needed. Pick the modifiers and the key from the dropdowns; if the chosen
combination is already taken by another application, the registration
fails and a note is written to the mod's log — just pick a different
combination. Every toggle shows a short on-screen indicator
("Mouse remap: ON/OFF"); a confirmation sound can be enabled with the
"Play a sound on toggle" option. The indicator scales with the screen
and appears on the monitor under the cursor, so it stays readable on
large / high-resolution displays. Remapping always starts active after
the mod (re)starts.

**Troubleshooting:** if pressing the shortcut only produces a plain
system beep and no on-screen indicator, the shortcut did not get
registered. Open the mod's log (Show log in Windhawk) and look for a
"RegisterHotKey failed" line — error 1409 means another application
already owns that combination; pick a different one and save again.

## Game mode

When enabled, remapping is automatically suspended while the foreground
window covers the entire screen — typical for games and fullscreen
video. Note that this also applies to browsers in fullscreen (F11) mode.

The fullscreen state is tracked efficiently: instead of being recomputed
inside the mouse hook on every click, it is cached and only refreshed via
window events (foreground change and window move/resize) while game mode
is enabled, so the hook stays lightweight.

## ⚠️ Warning: left and right button

Remapping the **left button** disables normal clicking system-wide while
enabled — you will not be able to click anything with it, including the
Windhawk window. Keep keyboard navigation in mind (Tab/Enter), or simply
disable the mod from Windhawk using the keyboard if you lock yourself
out. Remapping the **right button** similarly removes access to context
menus (the keyboard Menu key still works). Both are disabled by default;
enable them only if you know what you are doing.

---

# Mouse Button Remapper (polski)

Mod przemapowuje przyciski myszy na klawisze klawiatury, opcjonalnie z
modyfikatorami (Ctrl, Shift, Alt, Win). Obsługiwane przyciski: **górny
boczny** (Do przodu / XButton2), **dolny boczny** (Wstecz / XButton1),
**środkowy** (klik kółka) oraz — z ostrzeżeniem poniżej — **lewy** i
**prawy**. Mod działa jako „tool mod" w osobnym procesie i instaluje
globalny hook myszy, więc działa we wszystkich aplikacjach bez
wstrzykiwania się do powłoki ani innych programów.

## Zgodność myszek

Domyślnie w praktycznie każdej myszce dwa boczne przyciski są zgłaszane
jako standardowe przyciski myszy — **Do przodu** (XButton2) i **Wstecz**
(XButton1) — a środkowy jako klik kółka. Dokładnie te zdarzenia mod
przechwytuje, więc na zwykłej myszce bez oprogramowania działa od razu.

Jeśli jednak oprogramowanie myszki lub jej firmware przypisze przyciskowi
**funkcję multimedialną lub klawiaturową** (np. „Refresh", „Play/Pause",
„Wstecz przeglądarki"), takie naciśnięcie nie jest już wysyłane jako
przycisk myszy — przychodzi jako komenda multimedialna/klawiatury, której
hook myszy nie widzi, więc mod nie może go przemapować. Jeśli któryś
przycisk nie jest przechwytywany, w oprogramowaniu myszki ustaw go z
powrotem na zwykłe **Do przodu** / **Wstecz** / **klik kółka**, a klawisz
przypisz tutaj w modzie. Krótko: niech myszka wysyła normalny przycisk, a
przemapowaniem zajmie się mod — nie mapuj tego samego przycisku w obu
miejscach naraz.

## Konfiguracja

Każdy przycisk ma własną sekcję:

1. **Włączony** — główny włącznik. Gdy odznaczony, przycisk działa tak,
   jak ustawił producent, a pozostałe pola są ignorowane. Dzięki temu
   można przemapować tylko jeden przycisk, a resztę zostawić.

2. **Klawisz** — klawisz wysyłany podczas trzymania przycisku, wybierany
   z listy (F1–F12, Enter, Esc, strzałki, klawisze multimedialne, …).
   Wciśnięcie klawisza odwzorowuje wciśnięcie przycisku: key-down przy
   naciśnięciu, key-up przy puszczeniu.

3. **Klawisz niestandardowy** — używany tylko, gdy **Klawisz** =
   *Niestandardowy (pole poniżej)*. Akceptuje:
   - `F1` … `F24`,
   - pojedyncze litery i cyfry: `A` … `Z`, `0` … `9`,
   - nazwy: `Enter`, `Esc`, `Tab`, `Space`, `Backspace`, `Delete`,
     `Insert`, `Home`, `End`, `PageUp`, `PageDown`, `Up`, `Down`,
     `Left`, `Right`, `PlayPause`, `Mute`, `VolumeUp`, `VolumeDown`,
   - dowolny kod virtual-key w hex, np. `0x74` (= F5) — pełna lista na
     stronie "Virtual-Key Codes" w dokumentacji Microsoftu,
   - pełną kombinację, np. `Ctrl+F5` albo `Ctrl + Shift + T` —
     modyfikatory wpisane tutaj sumują się z dropdownem **Modyfikator**.

   Jeśli wartości nie da się sparsować, bind jest nieaktywny i przycisk
   zachowuje oryginalną funkcję (informacja trafia do logu moda —
   "Show log" w Windhawk).

4. **Modyfikator** — klawisze trzymane razem z głównym klawiszem, np.
   `Ctrl` + klawisz `F5` wysyła Ctrl+F5 (twarde odświeżenie strony).

5. **Akcja podwójnego kliknięcia** — opcjonalnie inny klawisz przy
   dwukliku (np. pojedyncze kliknięcie = F5, podwójne = Ctrl+Shift+T).
   Pola klawisza/klawisza niestandardowego/modyfikatora dwukliku
   działają tak samo jak dla pojedynczego kliknięcia. Uwaga: gdy ta
   opcja jest włączona, akcja pojedynczego kliknięcia wykonuje się
   dopiero po upływie systemowego czasu dwukliku (krótkie opóźnienie),
   a odwzorowanie przytrzymania przycisku jest dla niego wyłączone.
   Gdy opcja jest wyłączona (domyślnie), pojedyncze kliknięcia działają
   natychmiast.

Zmiany działają natychmiast po zapisaniu ustawień — bez restartu.

## Skrót włączania/wyłączania bindów

Opcjonalny globalny skrót klawiszowy, który jednym naciśnięciem
wstrzymuje/wznawia wszystkie bindy — przydatny, gdy chwilowo potrzebne
są oryginalne funkcje przycisków. Modyfikatory i klawisz wybierasz z
list — jeśli kombinacja jest już zajęta przez inny program, rejestracja
się nie powiedzie, a informacja trafi do logu moda — wystarczy wybrać
inną kombinację. Każde przełączenie pokazuje krótki wskaźnik na ekranie
("Mouse remap: ON/OFF"); dźwięk potwierdzenia można włączyć opcją
„Odtwarzaj dźwięk przy przełączeniu". Wskaźnik skaluje się do rozmiaru
ekranu i pojawia się na monitorze pod kursorem, więc pozostaje czytelny
na dużych ekranach i wysokiej rozdzielczości. Po (re)starcie moda bindy
są zawsze aktywne.

**Rozwiązywanie problemów:** jeśli naciśnięcie skrótu daje tylko zwykły
systemowy „ding" i wskaźnik się nie pokazuje, skrót nie został
zarejestrowany. Otwórz log moda (Show log w Windhawk) i poszukaj linii
"RegisterHotKey failed" — błąd 1409 oznacza, że kombinację zajmuje już
inna aplikacja; wybierz inną i zapisz ponownie.

## Tryb gry

Gdy włączony, bindy są automatycznie wstrzymywane, dopóki okno na
pierwszym planie zajmuje cały ekran — typowe dla gier i pełnoekranowego
wideo. Uwaga: dotyczy to też przeglądarki w trybie pełnoekranowym (F11).

Stan pełnego ekranu jest śledzony wydajnie: zamiast przeliczać go w haku
myszy przy każdym kliknięciu, jest buforowany i odświeżany tylko przez
zdarzenia okien (zmiana aktywnego okna oraz zmiana rozmiaru/pozycji),
i tylko gdy tryb gry jest włączony — dzięki czemu hak pozostaje lekki.

## ⚠️ Ostrzeżenie: lewy i prawy przycisk

Przemapowanie **lewego przycisku** blokuje normalne klikanie w całym
systemie — nie klikniesz nim niczego, łącznie z oknem Windhawka. W razie
zablokowania użyj nawigacji klawiaturą (Tab/Enter) albo wyłącz mod w
Windhawku klawiaturą. Przemapowanie **prawego przycisku** odbiera dostęp
do menu kontekstowych (działa jeszcze klawisz Menu na klawiaturze). Oba
są domyślnie wyłączone — włączaj tylko świadomie.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- xbutton2:
  - enabled: true
    $name: Enabled
    $name:pl-PL: Włączony
    $description: When off, the button keeps its original function and the fields below are ignored
    $description:pl-PL: Gdy wyłączone, przycisk zachowuje oryginalną funkcję, a poniższe pola są ignorowane
  - key: F5
    $name: Key
    $name:pl-PL: Klawisz
    $description: Key sent while the button is held; pick "Custom" for keys not listed here
    $description:pl-PL: Klawisz wysyłany podczas trzymania przycisku; wybierz „Niestandardowy" dla klawiszy spoza listy
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - customKey: ""
    $name: Custom key
    $name:pl-PL: Klawisz niestandardowy
    $description: "Used when Key = Custom. E.g. T, F13, 0x74, or a combo like Ctrl+F5 (see mod details for full syntax)"
    $description:pl-PL: "Używane gdy Klawisz = Niestandardowy. Np. T, F13, 0x74 lub kombinacja Ctrl+F5 (pełna składnia w opisie moda)"
  - mods: none
    $name: Modifier
    $name:pl-PL: Modyfikator
    $description: Held together with the key, e.g. Ctrl + F5
    $description:pl-PL: Trzymany razem z klawiszem, np. Ctrl + F5
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  - dblEnabled: false
    $name: Double-click action
    $name:pl-PL: Akcja podwójnego kliknięcia
    $description: "Send a different key on double-click. Note: the single-click action is then delayed by the double-click interval"
    $description:pl-PL: "Wysyłaj inny klawisz przy dwukliku. Uwaga: akcja pojedynczego kliknięcia jest wtedy opóźniona o czas oczekiwania na dwuklik"
  - dblKey: F5
    $name: Double-click key
    $name:pl-PL: Klawisz dwukliku
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - dblCustomKey: ""
    $name: Double-click custom key
    $name:pl-PL: Niestandardowy klawisz dwukliku
    $description: "Used when Double-click key = Custom; same syntax as Custom key"
    $description:pl-PL: "Używane gdy Klawisz dwukliku = Niestandardowy; składnia jak dla klawisza niestandardowego"
  - dblMods: none
    $name: Double-click modifier
    $name:pl-PL: Modyfikator dwukliku
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  $name: "Upper side button (Forward)"
  $name:pl-PL: "Górny przycisk boczny (Do przodu)"
- xbutton1:
  - enabled: false
    $name: Enabled
    $name:pl-PL: Włączony
    $description: When off, the button keeps its original function and the fields below are ignored
    $description:pl-PL: Gdy wyłączone, przycisk zachowuje oryginalną funkcję, a poniższe pola są ignorowane
  - key: F5
    $name: Key
    $name:pl-PL: Klawisz
    $description: Key sent while the button is held; pick "Custom" for keys not listed here
    $description:pl-PL: Klawisz wysyłany podczas trzymania przycisku; wybierz „Niestandardowy" dla klawiszy spoza listy
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - customKey: ""
    $name: Custom key
    $name:pl-PL: Klawisz niestandardowy
    $description: "Used when Key = Custom. E.g. T, F13, 0x74, or a combo like Ctrl+F5 (see mod details for full syntax)"
    $description:pl-PL: "Używane gdy Klawisz = Niestandardowy. Np. T, F13, 0x74 lub kombinacja Ctrl+F5 (pełna składnia w opisie moda)"
  - mods: none
    $name: Modifier
    $name:pl-PL: Modyfikator
    $description: Held together with the key, e.g. Ctrl + F5
    $description:pl-PL: Trzymany razem z klawiszem, np. Ctrl + F5
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  - dblEnabled: false
    $name: Double-click action
    $name:pl-PL: Akcja podwójnego kliknięcia
    $description: "Send a different key on double-click. Note: the single-click action is then delayed by the double-click interval"
    $description:pl-PL: "Wysyłaj inny klawisz przy dwukliku. Uwaga: akcja pojedynczego kliknięcia jest wtedy opóźniona o czas oczekiwania na dwuklik"
  - dblKey: F5
    $name: Double-click key
    $name:pl-PL: Klawisz dwukliku
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - dblCustomKey: ""
    $name: Double-click custom key
    $name:pl-PL: Niestandardowy klawisz dwukliku
    $description: "Used when Double-click key = Custom; same syntax as Custom key"
    $description:pl-PL: "Używane gdy Klawisz dwukliku = Niestandardowy; składnia jak dla klawisza niestandardowego"
  - dblMods: none
    $name: Double-click modifier
    $name:pl-PL: Modyfikator dwukliku
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  $name: "Lower side button (Back)"
  $name:pl-PL: "Dolny przycisk boczny (Wstecz)"
- middle:
  - enabled: false
    $name: Enabled
    $name:pl-PL: Włączony
    $description: When off, the button keeps its original function and the fields below are ignored
    $description:pl-PL: Gdy wyłączone, przycisk zachowuje oryginalną funkcję, a poniższe pola są ignorowane
  - key: F5
    $name: Key
    $name:pl-PL: Klawisz
    $description: Key sent while the button is held; pick "Custom" for keys not listed here
    $description:pl-PL: Klawisz wysyłany podczas trzymania przycisku; wybierz „Niestandardowy" dla klawiszy spoza listy
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - customKey: ""
    $name: Custom key
    $name:pl-PL: Klawisz niestandardowy
    $description: "Used when Key = Custom. E.g. T, F13, 0x74, or a combo like Ctrl+F5 (see mod details for full syntax)"
    $description:pl-PL: "Używane gdy Klawisz = Niestandardowy. Np. T, F13, 0x74 lub kombinacja Ctrl+F5 (pełna składnia w opisie moda)"
  - mods: none
    $name: Modifier
    $name:pl-PL: Modyfikator
    $description: Held together with the key, e.g. Ctrl + F5
    $description:pl-PL: Trzymany razem z klawiszem, np. Ctrl + F5
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  - dblEnabled: false
    $name: Double-click action
    $name:pl-PL: Akcja podwójnego kliknięcia
    $description: "Send a different key on double-click. Note: the single-click action is then delayed by the double-click interval"
    $description:pl-PL: "Wysyłaj inny klawisz przy dwukliku. Uwaga: akcja pojedynczego kliknięcia jest wtedy opóźniona o czas oczekiwania na dwuklik"
  - dblKey: F5
    $name: Double-click key
    $name:pl-PL: Klawisz dwukliku
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - dblCustomKey: ""
    $name: Double-click custom key
    $name:pl-PL: Niestandardowy klawisz dwukliku
    $description: "Used when Double-click key = Custom; same syntax as Custom key"
    $description:pl-PL: "Używane gdy Klawisz dwukliku = Niestandardowy; składnia jak dla klawisza niestandardowego"
  - dblMods: none
    $name: Double-click modifier
    $name:pl-PL: Modyfikator dwukliku
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  $name: "Middle button (wheel click)"
  $name:pl-PL: "Środkowy przycisk (klik kółka)"
- leftbtn:
  - enabled: false
    $name: "Enabled — ⚠️ disables normal left-clicking system-wide!"
    $name:pl-PL: "Włączony — ⚠️ blokuje normalne klikanie lewym przyciskiem w całym systemie!"
    $description: Read the warning in the mod details before enabling
    $description:pl-PL: Przeczytaj ostrzeżenie w opisie moda przed włączeniem
  - key: F5
    $name: Key
    $name:pl-PL: Klawisz
    $description: Key sent while the button is held; pick "Custom" for keys not listed here
    $description:pl-PL: Klawisz wysyłany podczas trzymania przycisku; wybierz „Niestandardowy" dla klawiszy spoza listy
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - customKey: ""
    $name: Custom key
    $name:pl-PL: Klawisz niestandardowy
    $description: "Used when Key = Custom. E.g. T, F13, 0x74, or a combo like Ctrl+F5 (see mod details for full syntax)"
    $description:pl-PL: "Używane gdy Klawisz = Niestandardowy. Np. T, F13, 0x74 lub kombinacja Ctrl+F5 (pełna składnia w opisie moda)"
  - mods: none
    $name: Modifier
    $name:pl-PL: Modyfikator
    $description: Held together with the key, e.g. Ctrl + F5
    $description:pl-PL: Trzymany razem z klawiszem, np. Ctrl + F5
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  - dblEnabled: false
    $name: Double-click action
    $name:pl-PL: Akcja podwójnego kliknięcia
    $description: "Send a different key on double-click. Note: the single-click action is then delayed by the double-click interval"
    $description:pl-PL: "Wysyłaj inny klawisz przy dwukliku. Uwaga: akcja pojedynczego kliknięcia jest wtedy opóźniona o czas oczekiwania na dwuklik"
  - dblKey: F5
    $name: Double-click key
    $name:pl-PL: Klawisz dwukliku
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - dblCustomKey: ""
    $name: Double-click custom key
    $name:pl-PL: Niestandardowy klawisz dwukliku
    $description: "Used when Double-click key = Custom; same syntax as Custom key"
    $description:pl-PL: "Używane gdy Klawisz dwukliku = Niestandardowy; składnia jak dla klawisza niestandardowego"
  - dblMods: none
    $name: Double-click modifier
    $name:pl-PL: Modyfikator dwukliku
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  $name: "Left button"
  $name:pl-PL: "Lewy przycisk"
- rightbtn:
  - enabled: false
    $name: "Enabled — ⚠️ disables right-click context menus system-wide!"
    $name:pl-PL: "Włączony — ⚠️ blokuje menu kontekstowe prawego przycisku w całym systemie!"
    $description: Read the warning in the mod details before enabling
    $description:pl-PL: Przeczytaj ostrzeżenie w opisie moda przed włączeniem
  - key: F5
    $name: Key
    $name:pl-PL: Klawisz
    $description: Key sent while the button is held; pick "Custom" for keys not listed here
    $description:pl-PL: Klawisz wysyłany podczas trzymania przycisku; wybierz „Niestandardowy" dla klawiszy spoza listy
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - customKey: ""
    $name: Custom key
    $name:pl-PL: Klawisz niestandardowy
    $description: "Used when Key = Custom. E.g. T, F13, 0x74, or a combo like Ctrl+F5 (see mod details for full syntax)"
    $description:pl-PL: "Używane gdy Klawisz = Niestandardowy. Np. T, F13, 0x74 lub kombinacja Ctrl+F5 (pełna składnia w opisie moda)"
  - mods: none
    $name: Modifier
    $name:pl-PL: Modyfikator
    $description: Held together with the key, e.g. Ctrl + F5
    $description:pl-PL: Trzymany razem z klawiszem, np. Ctrl + F5
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  - dblEnabled: false
    $name: Double-click action
    $name:pl-PL: Akcja podwójnego kliknięcia
    $description: "Send a different key on double-click. Note: the single-click action is then delayed by the double-click interval"
    $description:pl-PL: "Wysyłaj inny klawisz przy dwukliku. Uwaga: akcja pojedynczego kliknięcia jest wtedy opóźniona o czas oczekiwania na dwuklik"
  - dblKey: F5
    $name: Double-click key
    $name:pl-PL: Klawisz dwukliku
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Space
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Arrow Up
    - down: Arrow Down
    - left: Arrow Left
    - right: Arrow Right
    - playpause: Media Play/Pause
    - mute: Mute
    - volumeup: Volume Up
    - volumedown: Volume Down
    - custom: Custom (use field below)
    $options:pl-PL:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - enter: Enter
    - esc: Esc
    - tab: Tab
    - space: Spacja
    - backspace: Backspace
    - delete: Delete
    - insert: Insert
    - home: Home
    - end: End
    - pageup: Page Up
    - pagedown: Page Down
    - up: Strzałka w górę
    - down: Strzałka w dół
    - left: Strzałka w lewo
    - right: Strzałka w prawo
    - playpause: Odtwórz/Pauza
    - mute: Wycisz
    - volumeup: Głośniej
    - volumedown: Ciszej
    - custom: Niestandardowy (pole poniżej)
  - dblCustomKey: ""
    $name: Double-click custom key
    $name:pl-PL: Niestandardowy klawisz dwukliku
    $description: "Used when Double-click key = Custom; same syntax as Custom key"
    $description:pl-PL: "Używane gdy Klawisz dwukliku = Niestandardowy; składnia jak dla klawisza niestandardowego"
  - dblMods: none
    $name: Double-click modifier
    $name:pl-PL: Modyfikator dwukliku
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  $name: "Right button"
  $name:pl-PL: "Prawy przycisk"
- toggleHotkey:
  - enabled: false
    $name: Enabled
    $name:pl-PL: Włączony
    $description: Global shortcut that suspends/resumes all remaps; a system sound confirms each toggle
    $description:pl-PL: Globalny skrót wstrzymujący/wznawiający wszystkie bindy; każde przełączenie potwierdza dźwięk systemowy
  - mods: ctrl+alt
    $name: Shortcut modifiers
    $name:pl-PL: Modyfikatory skrótu
    $description: Pick a combination that is not used by other applications
    $description:pl-PL: Wybierz kombinację nieużywaną przez inne aplikacje
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
    $options:pl-PL:
    - none: Brak
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - win: Win
    - ctrl+shift: Ctrl+Shift
    - ctrl+alt: Ctrl+Alt
    - alt+shift: Alt+Shift
    - ctrl+alt+shift: Ctrl+Alt+Shift
  - key: F10
    $name: Shortcut key
    $name:pl-PL: Klawisz skrótu
    $options:
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - "0": "0"
    - "1": "1"
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
    - "7": "7"
    - "8": "8"
    - "9": "9"
    - "A": "A"
    - "B": "B"
    - "C": "C"
    - "D": "D"
    - "E": "E"
    - "F": "F"
    - "G": "G"
    - "H": "H"
    - "I": "I"
    - "J": "J"
    - "K": "K"
    - "L": "L"
    - "M": "M"
    - "N": "N"
    - "O": "O"
    - "P": "P"
    - "Q": "Q"
    - "R": "R"
    - "S": "S"
    - "T": "T"
    - "U": "U"
    - "V": "V"
    - "W": "W"
    - "X": "X"
    - "Y": "Y"
    - "Z": "Z"
  - sound: false
    $name: Play a sound on toggle
    $name:pl-PL: Odtwarzaj dźwięk przy przełączeniu
    $description: The on-screen indicator is always shown; this adds a system sound on top
    $description:pl-PL: Wskaźnik na ekranie pokazuje się zawsze; ta opcja dodaje do tego dźwięk systemowy
  $name: "Master toggle hotkey (suspend/resume all remaps)"
  $name:pl-PL: "Skrót włączania/wyłączania bindów"
- gameMode: false
  $name: "Game mode — suspend remapping while a fullscreen app is active"
  $name:pl-PL: "Tryb gry — wstrzymaj bindy, gdy aktywna jest aplikacja pełnoekranowa"
  $description: Keeps the bindings out of the way in games and fullscreen video (also applies to fullscreen F11 browsers)
  $description:pl-PL: Bindy nie przeszkadzają w grach i pełnoekranowym wideo (dotyczy też przeglądarki w trybie F11)
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <atomic>
#include <wchar.h>

enum { BTN_X1, BTN_X2, BTN_MID, BTN_LEFT, BTN_RIGHT, BTN_COUNT };

// Modifier bitmask reuses the RegisterHotKey MOD_* constants:
// MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN.

struct KeyCombo {
    UINT vk;
    UINT mods;
};

struct ButtonConfig {
    bool enabled;
    KeyCombo single;
    bool dblEnabled;
    KeyCombo dbl;
};

struct Settings {
    ButtonConfig buttons[BTN_COUNT];
    bool gameMode;
    bool hotkeyEnabled;
    bool hotkeySound;
    UINT hotkeyMods;
    UINT hotkeyVk;
};

// g_settings is guarded by g_settingsLock; the hook thread keeps a private
// snapshot (g_hookSettings) and refreshes it when g_settingsGeneration
// changes, so the hook never reads the settings while the settings thread
// reassigns them.
Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
std::atomic<int> g_settingsGeneration{0};

// Hook-thread-private snapshot; only touched on the hook thread.
Settings g_hookSettings;
int g_hookSettingsGeneration = -1;

// Combo currently held down because we swallowed the button-down event;
// vk 0 if not held. Lets the key-up match the key-down even if the
// settings change while the button is pressed.
KeyCombo g_held[BTN_COUNT];

// Number of button-up events to swallow because the matching button-down
// was swallowed in double-click mode (where no key is held).
int g_swallowUps[BTN_COUNT];

// Pending single-click timers used in double-click mode: the single-click
// action fires only if no second click arrives within the double-click
// interval. All timer activity runs on the hook thread, so no locking is
// needed.
UINT_PTR g_clickTimer[BTN_COUNT];
KeyCombo g_pendingSingle[BTN_COUNT];

// Master switch toggled by the hotkey. Always starts active.
std::atomic<bool> g_active{true};

HHOOK g_hook;
HANDLE g_hookThread;
DWORD g_hookThreadId;

// Cached "the foreground window is fullscreen" state for game mode. Computing it
// inside the low-level mouse hook on every click would add work to a global,
// latency-sensitive callback, so it is recomputed on the hook thread only when
// the relevant window events fire (see WinEventProc) and merely read in the
// hook. Both the WinEvent callback and the mouse hook run on the hook thread; it
// is kept atomic to document the cross-callback sharing.
// Caching approach suggested by @m417z during code review.
std::atomic<bool> g_foregroundFullscreen{false};
HWINEVENTHOOK g_winEventHooks[2];

// On-screen indicator shown when the toggle hotkey fires. Lives on the
// hook thread, which also dispatches its WM_PAINT/WM_TIMER messages.
HWND g_osdWnd;
HFONT g_osdFont;
HBRUSH g_osdBrush;
constexpr WCHAR kOsdClass[] = L"WindhawkMouseRemapOsd";

constexpr int kHotkeyId = 1;
constexpr UINT kMsgSettingsChanged = WM_APP + 1;

UINT ParseKeyName(PCWSTR s) {
    if (!*s) {
        return 0;
    }

    if ((s[0] == L'F' || s[0] == L'f') && s[1] >= L'0' && s[1] <= L'9') {
        int n = _wtoi(s + 1);
        if (n >= 1 && n <= 24) {
            return VK_F1 + n - 1;
        }
    }

    if (!s[1]) {
        WCHAR c = towupper(s[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return c;
        }
    }

    static const struct {
        PCWSTR name;
        UINT vk;
    } kNames[] = {
        {L"enter", VK_RETURN},     {L"esc", VK_ESCAPE},
        {L"escape", VK_ESCAPE},    {L"tab", VK_TAB},
        {L"space", VK_SPACE},      {L"backspace", VK_BACK},
        {L"delete", VK_DELETE},    {L"del", VK_DELETE},
        {L"insert", VK_INSERT},    {L"home", VK_HOME},
        {L"end", VK_END},          {L"pageup", VK_PRIOR},
        {L"pagedown", VK_NEXT},    {L"up", VK_UP},
        {L"down", VK_DOWN},        {L"left", VK_LEFT},
        {L"right", VK_RIGHT},      {L"playpause", VK_MEDIA_PLAY_PAUSE},
        {L"mute", VK_VOLUME_MUTE}, {L"volumeup", VK_VOLUME_UP},
        {L"volumedown", VK_VOLUME_DOWN},
    };
    for (const auto& entry : kNames) {
        if (_wcsicmp(s, entry.name) == 0) {
            return entry.vk;
        }
    }

    if (s[0] == L'0' && (s[1] == L'x' || s[1] == L'X')) {
        UINT vk = wcstoul(s, nullptr, 16);
        if (vk > 0 && vk < 0x100) {
            return vk;
        }
    }

    return 0;
}

UINT ParseModifierToken(PCWSTR s) {
    if (_wcsicmp(s, L"ctrl") == 0 || _wcsicmp(s, L"control") == 0) {
        return MOD_CONTROL;
    }
    if (_wcsicmp(s, L"shift") == 0) {
        return MOD_SHIFT;
    }
    if (_wcsicmp(s, L"alt") == 0) {
        return MOD_ALT;
    }
    if (_wcsicmp(s, L"win") == 0 || _wcsicmp(s, L"windows") == 0) {
        return MOD_WIN;
    }
    return 0;
}

// Parses "F5", "Ctrl+F5", "Ctrl + Shift + T", etc. Returns true if a key
// was found; modifiers are OR-ed into *mods.
bool ParseKeyCombo(PCWSTR s, UINT* vk, UINT* mods) {
    *vk = 0;

    WCHAR buf[128];
    wcsncpy_s(buf, s, _TRUNCATE);

    WCHAR* context = nullptr;
    for (WCHAR* token = wcstok_s(buf, L"+", &context); token;
         token = wcstok_s(nullptr, L"+", &context)) {
        while (*token == L' ') {
            token++;
        }
        WCHAR* end = token + wcslen(token);
        while (end > token && end[-1] == L' ') {
            *--end = L'\0';
        }

        UINT mod = ParseModifierToken(token);
        if (mod) {
            *mods |= mod;
            continue;
        }

        UINT key = ParseKeyName(token);
        if (!key || *vk) {
            *vk = 0;
            return false;
        }
        *vk = key;
    }

    return *vk != 0;
}

// Parses dropdown values like "none", "ctrl", "ctrl+alt+shift".
UINT ParseModsList(PCWSTR s) {
    UINT mods = 0;

    WCHAR buf[64];
    wcsncpy_s(buf, s, _TRUNCATE);

    WCHAR* context = nullptr;
    for (WCHAR* token = wcstok_s(buf, L"+", &context); token;
         token = wcstok_s(nullptr, L"+", &context)) {
        mods |= ParseModifierToken(token);
    }

    return mods;
}

bool IsExtendedKey(UINT vk) {
    switch (vk) {
        case VK_INSERT:
        case VK_DELETE:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_LWIN:
        case VK_RWIN:
            return true;
    }
    return false;
}

void AddKeyInput(INPUT* inputs, UINT* count, UINT vk, bool down) {
    INPUT& input = inputs[(*count)++];
    input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = (down ? 0 : KEYEVENTF_KEYUP) |
                       (IsExtendedKey(vk) ? KEYEVENTF_EXTENDEDKEY : 0);
}

void SendCombo(UINT vk, UINT mods, bool down) {
    static const struct {
        UINT bit;
        UINT vk;
    } kModKeys[] = {
        {MOD_CONTROL, VK_CONTROL},
        {MOD_SHIFT, VK_SHIFT},
        {MOD_ALT, VK_MENU},
        {MOD_WIN, VK_LWIN},
    };

    INPUT inputs[ARRAYSIZE(kModKeys) + 1];
    UINT count = 0;

    if (down) {
        // Modifiers first, then the key.
        for (const auto& mod : kModKeys) {
            if (mods & mod.bit) {
                AddKeyInput(inputs, &count, mod.vk, true);
            }
        }
        AddKeyInput(inputs, &count, vk, true);
    } else {
        // Key first, then modifiers in reverse order.
        AddKeyInput(inputs, &count, vk, false);
        for (int i = ARRAYSIZE(kModKeys) - 1; i >= 0; i--) {
            if (mods & kModKeys[i].bit) {
                AddKeyInput(inputs, &count, kModKeys[i].vk, false);
            }
        }
    }

    SendInput(count, inputs, sizeof(INPUT));
}

// Full press: down followed by up. Used for double-click mode actions,
// where mirroring the physical button hold is not possible.
void SendComboPress(const KeyCombo& combo) {
    SendCombo(combo.vk, combo.mods, true);
    SendCombo(combo.vk, combo.mods, false);
}

// True when the foreground window covers its entire monitor (typical for
// games and fullscreen video). The desktop itself is excluded.
bool IsForegroundFullscreen() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        return false;
    }

    WCHAR className[64];
    if (GetClassName(hwnd, className, ARRAYSIZE(className)) &&
        (_wcsicmp(className, L"Progman") == 0 ||
         _wcsicmp(className, L"WorkerW") == 0)) {
        return false;
    }

    RECT windowRect;
    if (!GetWindowRect(hwnd, &windowRect)) {
        return false;
    }

    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    if (!GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                        &monitorInfo)) {
        return false;
    }

    return windowRect.left <= monitorInfo.rcMonitor.left &&
           windowRect.top <= monitorInfo.rcMonitor.top &&
           windowRect.right >= monitorInfo.rcMonitor.right &&
           windowRect.bottom >= monitorInfo.rcMonitor.bottom;
}

// Recomputes the cached fullscreen state. Runs on the hook thread (from the
// WinEvent callback and when game mode is first enabled).
void RecomputeForegroundFullscreen() {
    g_foregroundFullscreen.store(IsForegroundFullscreen(),
                                 std::memory_order_relaxed);
}

// Refreshes the cache when the foreground window changes or a top-level window
// moves/resizes (e.g. a browser entering or leaving F11 fullscreen). Delivered
// to the hook thread's message loop (WINEVENT_OUTOFCONTEXT).
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                           LONG, DWORD, DWORD) {
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        // Ignore the constant stream of cursor/caret location changes; react
        // only to a top-level window's own bounds changing.
        if (idObject != OBJID_WINDOW || hwnd != GetForegroundWindow()) {
            return;
        }
    }
    RecomputeForegroundFullscreen();
}

// Fires when no second click arrived within the double-click interval —
// performs the delayed single-click action. Runs on the hook thread.
void CALLBACK SingleClickTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    for (int i = 0; i < BTN_COUNT; i++) {
        if (g_clickTimer[i] == id) {
            g_clickTimer[i] = 0;
            if (g_pendingSingle[i].vk) {
                SendComboPress(g_pendingSingle[i]);
            }
            break;
        }
    }
}

// Refreshes the hook thread's private settings snapshot if the settings
// changed since the last call. Runs on the hook thread.
void RefreshHookSettings() {
    int generation = g_settingsGeneration.load(std::memory_order_acquire);
    if (generation != g_hookSettingsGeneration) {
        AcquireSRWLockShared(&g_settingsLock);
        g_hookSettings = g_settings;
        ReleaseSRWLockShared(&g_settingsLock);
        g_hookSettingsGeneration = generation;
    }
}

// Returns true if the event was handled and should be swallowed.
bool HandleButtonDown(int button) {
    const ButtonConfig& config = g_hookSettings.buttons[button];
    if (!g_active || !config.enabled) {
        return false;
    }
    if (!config.single.vk && !(config.dblEnabled && config.dbl.vk)) {
        return false;
    }
    if (g_hookSettings.gameMode &&
        g_foregroundFullscreen.load(std::memory_order_relaxed)) {
        return false;
    }

    if (config.dblEnabled) {
        if (g_clickTimer[button]) {
            // Second click within the interval — double-click action.
            KillTimer(nullptr, g_clickTimer[button]);
            g_clickTimer[button] = 0;
            if (config.dbl.vk) {
                SendComboPress(config.dbl);
            }
        } else {
            g_pendingSingle[button] = config.single;
            g_clickTimer[button] = SetTimer(nullptr, 0, GetDoubleClickTime(),
                                            SingleClickTimerProc);
        }
        g_swallowUps[button]++;
        return true;
    }

    g_held[button] = config.single;
    SendCombo(config.single.vk, config.single.mods, true);
    return true;
}

// Returns true if the event was handled and should be swallowed.
bool HandleButtonUp(int button) {
    if (g_held[button].vk) {
        SendCombo(g_held[button].vk, g_held[button].mods, false);
        g_held[button] = {};
        return true;
    }
    if (g_swallowUps[button] > 0) {
        g_swallowUps[button]--;
        return true;
    }
    return false;
}

LRESULT CALLBACK OsdWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                            LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, g_active ? RGB(118, 255, 122)
                                       : RGB(255, 138, 128));
            HGDIOBJ oldFont = SelectObject(hdc, g_osdFont);
            WCHAR text[64];
            GetWindowText(hwnd, text, ARRAYSIZE(text));
            DrawText(hdc, text, -1, &rc,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldFont);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_TIMER:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            if (g_osdWnd == hwnd) {
                g_osdWnd = nullptr;
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowOsd(PCWSTR text) {
    if (g_osdWnd) {
        DestroyWindow(g_osdWnd);
    }

    // Show on the monitor under the cursor and scale everything with that
    // monitor's height, so the indicator stays readable on large / high-
    // resolution displays instead of being a tiny fixed-size popup.
    POINT cursor;
    GetCursorPos(&cursor);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY), &mi);
    int screenW = mi.rcMonitor.right - mi.rcMonitor.left;
    int screenH = mi.rcMonitor.bottom - mi.rcMonitor.top;

    int fontHeight = screenH / 36;
    if (fontHeight < 20) {
        fontHeight = 20;
    }

    if (g_osdFont) {
        DeleteObject(g_osdFont);
    }
    g_osdFont = CreateFont(-fontHeight, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
                           FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                           DEFAULT_PITCH, L"Segoe UI");

    // Size the window around the measured text.
    SIZE textSize{};
    HDC dc = GetDC(nullptr);
    HGDIOBJ oldFont = SelectObject(dc, g_osdFont);
    GetTextExtentPoint32(dc, text, (int)wcslen(text), &textSize);
    SelectObject(dc, oldFont);
    ReleaseDC(nullptr, dc);

    int padX = fontHeight;
    int padY = fontHeight * 2 / 3;
    int width = textSize.cx + padX * 2;
    int height = textSize.cy + padY * 2;
    int x = mi.rcMonitor.left + (screenW - width) / 2;
    int y = mi.rcMonitor.top + screenH / 11;

    g_osdWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED |
            WS_EX_TRANSPARENT,
        kOsdClass, text, WS_POPUP, x, y, width, height, nullptr, nullptr,
        GetModuleHandle(nullptr), nullptr);
    if (!g_osdWnd) {
        return;
    }

    SetLayeredWindowAttributes(g_osdWnd, 0, 235, LWA_ALPHA);
    ShowWindow(g_osdWnd, SW_SHOWNOACTIVATE);
    SetTimer(g_osdWnd, 1, 1400, nullptr);
}

void SetActive(bool active) {
    g_active = active;
    if (!active) {
        for (int i = 0; i < BTN_COUNT; i++) {
            if (g_held[i].vk) {
                SendCombo(g_held[i].vk, g_held[i].mods, false);
                g_held[i] = {};
            }
            if (g_clickTimer[i]) {
                // Fire the pending single-click action so it is not lost.
                KillTimer(nullptr, g_clickTimer[i]);
                g_clickTimer[i] = 0;
                if (g_pendingSingle[i].vk) {
                    SendComboPress(g_pendingSingle[i]);
                }
            }
        }
    }
    ShowOsd(active ? L"Mouse remap: ON" : L"Mouse remap: OFF");
    if (g_hookSettings.hotkeySound) {
        MessageBeep(active ? MB_OK : MB_ICONASTERISK);
    }
    Wh_Log(L"Remapping %s", active ? L"resumed" : L"suspended");
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        RefreshHookSettings();

        const MSLLHOOKSTRUCT* info = (const MSLLHOOKSTRUCT*)lParam;
        bool handled = false;

        switch (wParam) {
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP: {
                int button = HIWORD(info->mouseData) == XBUTTON1 ? BTN_X1
                                                                 : BTN_X2;
                handled = wParam == WM_XBUTTONDOWN ? HandleButtonDown(button)
                                                   : HandleButtonUp(button);
                break;
            }
            case WM_MBUTTONDOWN:
                handled = HandleButtonDown(BTN_MID);
                break;
            case WM_MBUTTONUP:
                handled = HandleButtonUp(BTN_MID);
                break;
            case WM_LBUTTONDOWN:
                handled = HandleButtonDown(BTN_LEFT);
                break;
            case WM_LBUTTONUP:
                handled = HandleButtonUp(BTN_LEFT);
                break;
            case WM_RBUTTONDOWN:
                handled = HandleButtonDown(BTN_RIGHT);
                break;
            case WM_RBUTTONUP:
                handled = HandleButtonUp(BTN_RIGHT);
                break;
        }

        if (handled) {
            return 1;
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

// Hotkeys are per-thread, so this must run on the hook thread.
void UpdateHotkeyRegistration() {
    RefreshHookSettings();
    UnregisterHotKey(nullptr, kHotkeyId);
    if (g_hookSettings.hotkeyEnabled && g_hookSettings.hotkeyVk) {
        if (RegisterHotKey(nullptr, kHotkeyId,
                           g_hookSettings.hotkeyMods | MOD_NOREPEAT,
                           g_hookSettings.hotkeyVk)) {
            Wh_Log(L"Toggle hotkey registered (mods=0x%X, vk=0x%X)",
                   g_hookSettings.hotkeyMods, g_hookSettings.hotkeyVk);
        } else {
            Wh_Log(
                L"RegisterHotKey failed (%u) — the shortcut may already be "
                L"in use by another application; pick a different one",
                GetLastError());
        }
    } else {
        Wh_Log(L"Toggle hotkey disabled");
    }
}

// Starts/stops foreground tracking so the cached fullscreen state is kept up to
// date only while game mode is enabled. Runs on the hook thread.
void UpdateGameModeTracking() {
    RefreshHookSettings();
    bool wanted = g_hookSettings.gameMode;
    bool active = g_winEventHooks[0] != nullptr;
    if (wanted == active) {
        return;
    }
    if (wanted) {
        g_winEventHooks[0] = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            WinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        g_winEventHooks[1] = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
            WinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        RecomputeForegroundFullscreen();  // prime the cache
    } else {
        for (HWINEVENTHOOK& h : g_winEventHooks) {
            if (h) {
                UnhookWinEvent(h);
                h = nullptr;
            }
        }
        g_foregroundFullscreen.store(false, std::memory_order_relaxed);
    }
}

DWORD WINAPI HookThreadProc(LPVOID) {
    g_hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc,
                              GetModuleHandle(nullptr), 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        return 1;
    }

    g_osdBrush = CreateSolidBrush(RGB(28, 28, 28));
    // The OSD font is (re)created per show in ShowOsd so it can scale with the
    // current screen.
    WNDCLASS wc{};
    wc.lpfnWndProc = OsdWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kOsdClass;
    wc.hbrBackground = g_osdBrush;
    RegisterClass(&wc);

    UpdateHotkeyRegistration();
    UpdateGameModeTracking();

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd) {
            if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
                Wh_Log(L"Toggle hotkey pressed");
                SetActive(!g_active);
                continue;
            }
            if (msg.message == kMsgSettingsChanged) {
                UpdateHotkeyRegistration();
                UpdateGameModeTracking();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotKey(nullptr, kHotkeyId);
    for (HWINEVENTHOOK& h : g_winEventHooks) {
        if (h) {
            UnhookWinEvent(h);
            h = nullptr;
        }
    }
    UnhookWindowsHookEx(g_hook);
    g_hook = nullptr;

    // Release anything left pending or held down.
    for (int i = 0; i < BTN_COUNT; i++) {
        if (g_clickTimer[i]) {
            KillTimer(nullptr, g_clickTimer[i]);
            g_clickTimer[i] = 0;
        }
        if (g_held[i].vk) {
            SendCombo(g_held[i].vk, g_held[i].mods, false);
            g_held[i] = {};
        }
    }

    if (g_osdWnd) {
        DestroyWindow(g_osdWnd);
    }
    UnregisterClass(kOsdClass, GetModuleHandle(nullptr));
    if (g_osdFont) {
        DeleteObject(g_osdFont);
        g_osdFont = nullptr;
    }
    if (g_osdBrush) {
        DeleteObject(g_osdBrush);
        g_osdBrush = nullptr;
    }

    return 0;
}

KeyCombo LoadComboSettings(PCWSTR prefix, PCWSTR keyField, PCWSTR customField,
                           PCWSTR modsField, bool warnIfInvalid) {
    KeyCombo combo{};

    PCWSTR keySel = Wh_GetStringSetting(L"%s.%s", prefix, keyField);
    if (_wcsicmp(keySel, L"custom") == 0) {
        PCWSTR custom = Wh_GetStringSetting(L"%s.%s", prefix, customField);
        if (!ParseKeyCombo(custom, &combo.vk, &combo.mods) && warnIfInvalid) {
            Wh_Log(L"Unrecognized custom key for %s.%s: \"%s\"", prefix,
                   customField, custom);
        }
        Wh_FreeStringSetting(custom);
    } else {
        combo.vk = ParseKeyName(keySel);
        if (!combo.vk && warnIfInvalid) {
            Wh_Log(L"Unrecognized key for %s.%s: \"%s\"", prefix, keyField,
                   keySel);
        }
    }
    Wh_FreeStringSetting(keySel);

    PCWSTR modsList = Wh_GetStringSetting(L"%s.%s", prefix, modsField);
    combo.mods |= ParseModsList(modsList);
    Wh_FreeStringSetting(modsList);

    if (!combo.vk) {
        combo.mods = 0;
    }
    return combo;
}

void LoadButtonSettings(Settings& settings, int button, PCWSTR prefix) {
    ButtonConfig& config = settings.buttons[button];
    config.enabled = Wh_GetIntSetting(L"%s.enabled", prefix);
    config.single = LoadComboSettings(prefix, L"key", L"customKey", L"mods",
                                      config.enabled);
    config.dblEnabled = Wh_GetIntSetting(L"%s.dblEnabled", prefix);
    config.dbl = LoadComboSettings(prefix, L"dblKey", L"dblCustomKey",
                                   L"dblMods",
                                   config.enabled && config.dblEnabled);
}

// Note: Wh_GetStringSetting never returns NULL (it returns L"" when unset),
// so the parsers only need to handle empty strings.
void LoadSettings() {
    Settings settings{};

    LoadButtonSettings(settings, BTN_X1, L"xbutton1");
    LoadButtonSettings(settings, BTN_X2, L"xbutton2");
    LoadButtonSettings(settings, BTN_MID, L"middle");
    LoadButtonSettings(settings, BTN_LEFT, L"leftbtn");
    LoadButtonSettings(settings, BTN_RIGHT, L"rightbtn");

    settings.gameMode = Wh_GetIntSetting(L"gameMode");

    settings.hotkeyEnabled = Wh_GetIntSetting(L"toggleHotkey.enabled");
    settings.hotkeySound = Wh_GetIntSetting(L"toggleHotkey.sound");

    PCWSTR mods = Wh_GetStringSetting(L"toggleHotkey.mods");
    settings.hotkeyMods = ParseModsList(mods);
    Wh_FreeStringSetting(mods);

    PCWSTR key = Wh_GetStringSetting(L"toggleHotkey.key");
    settings.hotkeyVk = ParseKeyName(key);
    if (settings.hotkeyEnabled && !settings.hotkeyVk) {
        Wh_Log(L"Unrecognized toggle hotkey key: \"%s\"", key);
    }
    Wh_FreeStringSetting(key);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = settings;
    ReleaseSRWLockExclusive(&g_settingsLock);
    g_settingsGeneration.fetch_add(1, std::memory_order_release);
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_hookThread =
        CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    PostThreadMessage(g_hookThreadId, kMsgSettingsChanged, 0, 0);
}

void WhTool_ModUninit() {
    if (g_hookThread) {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
        // Safe to wait without a timeout: the hook thread only blocks in
        // GetMessage, so it exits promptly once WM_QUIT is posted.
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
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
    // Never run in session 0 (services / non-interactive). A mouse hook and an
    // on-screen indicator make no sense there, and Windhawk may load the mod
    // into such processes.
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
