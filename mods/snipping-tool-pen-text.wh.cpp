// ==WindhawkMod==
// @id                 snipping-tool-pen-text
// @name               Snipping Tool Pen Text
// @name:pt-BR         Snipping Tool: texto com a caneta
// @description        Adds a text tool to the Windows 11 Snipping Tool that writes with the app's own pen
// @description:pt-BR  Adiciona ao Snipping Tool do Windows 11 uma ferramenta de texto que escreve com a própria caneta do app
// @version            1.0.2
// @author             Cris
// @github             https://github.com/cristianosm
// @license            MIT
// @include            SnippingTool.exe
// @compilerOptions    -lgdi32 -luser32 -ldwmapi -ladvapi32 -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snipping Tool Pen Text

The Windows 11 Snipping Tool has a pen, a highlighter and shapes, but no text
tool. This mod adds one: it "writes" your text on the screenshot using the
**Snipping Tool's own pen**.

Since the text becomes real pen strokes, it lives inside the editor: undo it
with Ctrl+Z, erase it with the eraser, save and copy as usual.

## Preview

![Snipping Tool Pen Text demo](https://raw.githubusercontent.com/cristianosm/windhawk-assets/main/snipping-tool-pen-text-demo.gif)

## How to use

1. Open a screenshot in the Snipping Tool editor.
2. Pick the pen color and thickness. The pen is selected automatically when
   writing, and the previous tool (highlighter or eraser) is restored after.
3. Click the floating **T** button (near the window corner) or press
   **Ctrl+Alt+Shift+T**. With the hotkey, the text starts at the mouse position.
4. Type the text. A live preview shows where it will be written, in the pen's
   color and thickness.
   - **Position**: click on the image where the text should start
     (the preview follows the mouse).
   - **Alt+Arrows**: move the text 10 px (100 px with Shift). The step is
     configurable.
   - **Height** and **Speed**: use the - / + buttons, the mouse wheel or the
     Up/Down keys. The last values used are remembered.
   - **Ctrl+Enter**: write. **Shift+Enter** (or Enter): new line.
     **Esc**: cancel.
5. While the text is written, the physical mouse is ignored (optional).
   Press Esc to stop.

## Notes

- The text uses a single-stroke font (Hershey "futural", public domain), with
  support for accented Latin letters (á à â ã é ê í ó ô õ ú ü ç ñ) and º ª °.
- The dialog follows the Windows light/dark theme and accent color, and is
  shown in English or Portuguese (automatic, or chosen in the settings).
- The preview reads the pen color and thickness from the Snipping Tool's own
  settings. Thickness matches the result at 100% editor zoom.
- If curves come out jagged, lower the writing speed.
- The floating button position is configurable (corner + X/Y offsets).

## Português

Este mod adiciona uma ferramenta de texto ao Snipping Tool do Windows 11. O
texto é escrito com a **própria caneta do Snipping Tool**, então fica dentro do
editor: dá para desfazer com Ctrl+Z, apagar com a borracha, salvar e copiar.

Como usar: escolha a cor e a espessura da caneta (ela é selecionada
automaticamente ao escrever, e a ferramenta anterior volta depois), clique no
botão flutuante **T** ou use **Ctrl+Alt+Shift+T**, digite o texto e confira a
prévia.
**Posicionar** escolhe onde o texto começa; **Alt+Setas** ajusta a posição;
**Ctrl+Enter** escreve, **Shift+Enter** quebra a linha e **Esc** cancela.
A janela aparece em português automaticamente quando o Windows está em
português.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $name:pt-BR: Idioma
  $description: "Language of the text dialog. Automatic follows the Windows display language."
  $description:pt-BR: "Idioma da caixa de texto. Automático segue o idioma do Windows."
  $options:
  - auto: Automatic
  - en: English
  - pt: Português
  $options:pt-BR:
  - auto: Automático
  - en: English
  - pt: Português
- hotkey: Ctrl+Alt+Shift+T
  $name: Hotkey
  $name:pt-BR: Atalho
  $description: "Key combination that opens the text dialog, for example Ctrl+Alt+Shift+T or Ctrl+Shift+F2"
  $description:pt-BR: "Combinação de teclas que abre a caixa de texto, por exemplo Ctrl+Alt+Shift+T ou Ctrl+Shift+F2"
- writeSpeed: "3"
  $name: Writing speed (1 to 10)
  $name:pt-BR: Velocidade de escrita (1 a 10)
  $description: "Initial speed. 1 = slower and more precise, 10 = faster. The dialog remembers the last speed used; changing this setting resets it."
  $description:pt-BR: "Velocidade inicial. 1 = mais lento e preciso, 10 = mais rápido. A caixa de texto lembra a última velocidade usada; mudar aqui volta a usar este valor."
  $options:
  - "1": "1 (slowest)"
  - "2": "2"
  - "3": "3"
  - "4": "4"
  - "5": "5"
  - "6": "6"
  - "7": "7"
  - "8": "8"
  - "9": "9"
  - "10": "10 (fastest)"
  $options:pt-BR:
  - "1": "1 (mais lento)"
  - "2": "2"
  - "3": "3"
  - "4": "4"
  - "5": "5"
  - "6": "6"
  - "7": "7"
  - "8": "8"
  - "9": "9"
  - "10": "10 (mais rápido)"
- blockMouse: true
  $name: Lock the mouse while writing
  $name:pt-BR: Travar o mouse durante a escrita
  $description: "Ignores the physical mouse while the text is written, so an accidental move doesn't ruin the letters. Esc stops."
  $description:pt-BR: "Ignora o mouse físico enquanto o texto é escrito, para um movimento acidental não estragar as letras. Esc interrompe."
- dialogPosition: bottomCenter
  $name: Text dialog position
  $name:pt-BR: Posição da caixa de texto
  $options:
  - bottomCenter: Bottom center of the Snipping Tool window
  - auto: Near the text
  $options:pt-BR:
  - bottomCenter: Embaixo, no centro da janela do Snipping Tool
  - auto: Perto do texto
- defaultSize: 28
  $name: Default letter height (px)
  $name:pt-BR: Altura padrão das letras (px)
  $description: "Height of uppercase letters, in screen pixels. The last height used is remembered."
  $description:pt-BR: "Altura das letras maiúsculas, em pixels de tela. O último tamanho usado é lembrado."
- nudgeStep: 10
  $name: Alt+Arrow step (px)
  $name:pt-BR: Passo do Alt+Seta (px)
  $description: "How many pixels Alt+Arrow moves the text (1 to 200). With Shift, it moves 10 times more."
  $description:pt-BR: "Quantos pixels o Alt+Seta move o texto (1 a 200). Com Shift, move 10 vezes mais."
- showButton: true
  $name: Show floating button
  $name:pt-BR: Mostrar botão flutuante
- buttonCorner: topRight
  $name: Button reference corner
  $name:pt-BR: Canto de referência do botão
  $options:
  - topLeft: Top left
  - topRight: Top right
  - bottomLeft: Bottom left
  - bottomRight: Bottom right
  $options:pt-BR:
  - topLeft: Superior esquerdo
  - topRight: Superior direito
  - bottomLeft: Inferior esquerdo
  - bottomRight: Inferior direito
- buttonOffsetX: 140
  $name: Button horizontal offset (px)
  $name:pt-BR: Distância horizontal do botão (px)
  $description: "Distance from the window's side edge (on the chosen corner's side) to the button, in pixels at 100% scale."
  $description:pt-BR: "Distância entre a borda lateral da janela (do lado do canto escolhido) e o botão, em pixels na escala 100%."
- buttonOffsetY: 0
  $name: Button vertical offset (px)
  $name:pt-BR: Distância vertical do botão (px)
  $description: "Distance from the window's top or bottom edge (on the chosen corner's side) to the button, in pixels at 100% scale."
  $description:pt-BR: "Distância entre a borda superior ou inferior da janela (do lado do canto escolhido) e o botão, em pixels na escala 100%."
- previewColor: pen
  $name: Preview color
  $name:pt-BR: Cor da prévia
  $description: "With the automatic option, the preview uses the color and thickness of the pen selected in the Snipping Tool."
  $description:pt-BR: "Com a opção automática, a prévia usa a cor e a espessura da caneta selecionada no Snipping Tool."
  $options:
  - pen: Same as the pen (automatic)
  - red: Red
  - blue: Blue
  - green: Green
  - yellow: Yellow
  - black: Black
  - white: White
  - accent: Windows accent color
  $options:pt-BR:
  - pen: Mesma cor da caneta (automático)
  - red: Vermelho
  - blue: Azul
  - green: Verde
  - yellow: Amarelo
  - black: Preto
  - white: Branco
  - accent: Cor de destaque do Windows
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <inspectable.h>
#include <winstring.h>
#include <uiautomation.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <string>
#include <vector>

//+-------------------------------------
//|Single-stroke glyphs. Defined at the end of the file.
//+---------------------------------------------------------------------------
//|Font data (declarations)
//+---------------------------------------------------------------------------
struct GlyphIndex {
    unsigned short cp;
    short width;
    unsigned short offset;
};
extern const short kGlyphData[];
extern const GlyphIndex kGlyphIndex[];
extern const int kGlyphCount;

namespace {

//+-------------------------------------
//|Font layout units (half Hershey units), limits, messages and class names.
//+---------------------------------------------------------------------------
//|Constants
//+---------------------------------------------------------------------------
constexpr double kCapLine = -24.0;
constexpr double kCapHeight = 42.0;
constexpr double kLineHeight = 80.0;

constexpr int kMinSize = 6;
constexpr int kMaxSize = 400;
constexpr int kMinNudge = 1;
constexpr int kMaxNudge = 200;
constexpr int kMinSpeed = 1;
constexpr int kMaxSpeed = 10;

constexpr UINT WM_APP_OPEN = WM_APP + 1;
constexpr UINT WM_APP_DRAW = WM_APP + 2;
constexpr UINT WM_APP_RELOAD = WM_APP + 3;
constexpr UINT WM_APP_ENDPICK = WM_APP + 4;
constexpr UINT WM_APP_THEME = WM_APP + 5;
constexpr UINT_PTR kDialogTimer = 1;
constexpr UINT kDialogTimerMs = 250;
constexpr int kHotkeyId = 1;
constexpr UINT_PTR kEditorRecheckTimer = 2;
constexpr UINT kEditorRecheckMs = 300;
constexpr int kEditorRecheckTries = 10;

//+---------------------------------------------------------------------------
//|Fallback drawing area when the annotation canvas can't be found: window
//|frame minus the title bar and toolbar at the top and a margin elsewhere
//|(96-DPI units).
constexpr int kEditorTopInset = 80;
constexpr int kEditorEdgeInset = 8;

constexpr wchar_t kMsgClass[] = L"WhPenTextMsg";
constexpr wchar_t kButtonClass[] = L"WhPenTextButton";
constexpr wchar_t kDialogClass[] = L"WhPenTextDialog";
constexpr wchar_t kPreviewClass[] = L"WhPenTextPreview";
constexpr wchar_t kPickClass[] = L"WhPenTextPick";
constexpr wchar_t kHintClass[] = L"WhPenTextHint";

constexpr COLORREF kPickFill = RGB(75, 24, 24);
constexpr COLORREF kPickFillPressed = RGB(58, 18, 18);
constexpr COLORREF kPickBorder = RGB(110, 40, 40);

//+---------------------------------------------------------------------------
//|Snipping Tool pen palette, in the order of the color grid (row by row).
//|PenBrushIndex in the app settings is an index into this list.
constexpr COLORREF kPenPalette[] = {
    RGB(0, 0, 0),       RGB(255, 255, 255), RGB(209, 211, 212),
    RGB(167, 169, 172), RGB(128, 130, 133), RGB(88, 89, 91),
    RGB(179, 21, 100),  RGB(230, 27, 27),   RGB(255, 85, 0),
    RGB(255, 170, 0),   RGB(255, 206, 0),   RGB(255, 230, 0),
    RGB(162, 230, 27),  RGB(38, 230, 0),    RGB(0, 128, 85),
    RGB(0, 170, 204),   RGB(0, 77, 230),    RGB(61, 0, 184),
    RGB(102, 0, 204),   RGB(96, 0, 128),    RGB(247, 215, 196),
    RGB(187, 145, 103), RGB(142, 86, 46),   RGB(97, 61, 48),
    RGB(255, 128, 255), RGB(255, 198, 128), RGB(255, 255, 128),
    RGB(128, 255, 158), RGB(128, 214, 255), RGB(188, 179, 255),
};
constexpr int kPenPaletteCount = sizeof(kPenPalette) / sizeof(kPenPalette[0]);

//+---------------------------------------------------------------------------
//|Dialog control IDs.
enum : int {
    kIdEdit = 1001,
    kIdSize = 1002,
    kIdMinus = 1003,
    kIdPlus = 1004,
    kIdPick = 1005,
    kIdHint = 1006,
    kIdLabel = 1007,
    kIdClose = 1008,
    kIdTitle = 1009,
    kIdCaption = 1010,
    kIdSpeed = 1011,
    kIdSpeedMinus = 1012,
    kIdSpeedPlus = 1013,
};

//+-------------------------------------
//|Plain data types shared by the whole mod.
//+---------------------------------------------------------------------------
//|Types
//+---------------------------------------------------------------------------
enum class Corner { TopLeft, TopRight, BottomLeft, BottomRight };
enum class Language { Auto, English, Portuguese };
enum class Mode { Idle, Dialog, Picking, Drawing };

struct Settings {
    Language language = Language::Auto;
    UINT mods = MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    UINT vk = 'T';
    int speed = 3;
    bool blockMouse = true;
    bool dialogBottomCenter = true;
    int defaultSize = 28;
    int nudgeStep = 10;
    bool showButton = true;
    Corner corner = Corner::TopRight;
    int offsetX = 140;
    int offsetY = 0;
    bool previewAccent = false;
    bool previewFromPen = true;
    COLORREF previewColor = RGB(232, 17, 35);
};

//+---------------------------------------------------------------------------
//|User-visible texts of the dialog (one set per language).
struct UiStrings {
    const wchar_t* title;
    const wchar_t* lineBreakHint;
    const wchar_t* heightLabel;
    const wchar_t* speedLabel;
    const wchar_t* pick;
    const wchar_t* write;
    const wchar_t* cancel;
    const wchar_t* pickShortcut;
    const wchar_t* writeShortcut;
    const wchar_t* cancelShortcut;
    const wchar_t* pickBanner;
    const wchar_t* windowTitle;
    const wchar_t* outsideWarning;
};

const UiStrings kStringsEn = {
    L"Text to write with the pen",
    L"[Shift] + [Enter] = New line",
    L"Height (px)",
    L"Speed :",
    L"Position\u2026",
    L"Write",
    L"Cancel",
    L"[Alt]+Arrow",
    L"[Ctrl] +[Enter]",
    L"[Esc]",
    L"Click where the text should start \u00B7 arrows adjust \u00B7 Esc cancels",
    L"Pen text",
    L"The text goes outside the editor area",
};

const UiStrings kStringsPt = {
    L"Texto a escrever com a caneta",
    L"[Shift] + [Enter] = Quebra linha",
    L"Altura (px)",
    L"Velocidade :",
    L"Posicionar\u2026",
    L"Escrever",
    L"Cancelar",
    L"[Alt]+Seta",
    L"[Ctrl] +[Enter]",
    L"[Esc]",
    L"Clique onde o texto deve come\u00E7ar \u00B7 setas ajustam \u00B7 Esc cancela",
    L"Texto com a caneta",
    L"O texto sai da \u00E1rea do editor",
};

struct PenInfo {
    bool ok = false;
    COLORREF color = RGB(230, 27, 27);
    double width = 2.0;  // DIPs
};

struct Theme {
    bool dark = false;
    COLORREF bg, surface, surfaceHover, surfacePressed, border, text,
        textSecondary, accent, accentPressed, accentText, warning;
};

struct PtD {
    double x;
    double y;
};
using Stroke = std::vector<PtD>;

struct ButtonState {
    HWND hwnd = nullptr;
    HWND target = nullptr;
    HFONT font = nullptr;
    UINT fontDpi = 0;
    RECT lastRect = {};
    bool hover = false;
    bool pressed = false;
};

struct DialogState {
    HWND hwnd = nullptr;
    HWND edit = nullptr;
    HWND sizeEdit = nullptr;
    HWND speedEdit = nullptr;
    HWND hint = nullptr;
    HWND target = nullptr;
    HFONT font = nullptr;
    HFONT fontSmall = nullptr;
    HFONT fontTitle = nullptr;
    HFONT fontIcon = nullptr;
    UINT dpi = 96;
    std::vector<RECT> frames;  // client rects of the boxes behind the edits
    POINT origin = {};
    int size = 28;
    int speed = 3;
    bool closing = false;
    bool warning = false;     // hint shows the "outside the editor" warning
    bool hiddenAway = false;  // hidden while another app is in front
};

struct PreviewState {
    HWND hwnd = nullptr;
    POINT windowPos = {};
    std::vector<Stroke> strokes;  // screen coordinates
};

struct PickState {
    HWND overlay = nullptr;
    HWND hint = nullptr;
    POINT savedOrigin = {};
    RECT safe = {};  // drawing area, fixed for the whole pick
    HFONT hintFont = nullptr;
    bool ending = false;
};

struct DrawJob {
    HWND target = nullptr;
    std::wstring text;
    POINT origin = {};
    int size = 0;
    int speed = 3;
};

//+-------------------------------------
//|Mod state. Everything runs on the worker thread unless noted.
//+---------------------------------------------------------------------------
//|Globals
//+---------------------------------------------------------------------------
Settings g_settings;
PenInfo g_pen;
Theme g_theme;
HBRUSH g_bgBrush = nullptr;
HBRUSH g_surfaceBrush = nullptr;

std::atomic<bool> g_stop{false};
HANDLE g_thread = nullptr;
DWORD g_threadId = 0;
HINSTANCE g_hInst = nullptr;
HWND g_msgWnd = nullptr;
HWINEVENTHOOK g_locationHook = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;
HWINEVENTHOOK g_minimizeHook = nullptr;
HANDLE g_stopEvent = nullptr;
IUIAutomation* g_uia = nullptr;

//+---------------------------------------------------------------------------
//|Cached "does this window show an image" result for the foreground editor.
HWND g_editorChecked = nullptr;
bool g_editorHasImage = false;
HWND g_recheckHwnd = nullptr;  // window being re-checked after it appeared
int g_recheckLeft = 0;
Mode g_mode = Mode::Idle;

ButtonState g_btn;
DialogState g_dlg;
PreviewState g_preview;
PickState g_pick;
DrawJob g_job;

//+---------------------------------------------------------------------------
//|Last text origin, relative to the editor window's top-left corner.
HWND g_lastTarget = nullptr;
POINT g_lastRel = {};

//+---------------------------------------------------------------------------
//|Foreground tracking and hotkey registration state.
bool g_wasForeground = false;
bool g_hotkeyRegistered = false;
bool g_themePending = false;  // a theme refresh is already queued

//+-------------------------------------
//|Functions used before their definition.
//+---------------------------------------------------------------------------
//|Forward declarations
//+---------------------------------------------------------------------------
void UpdatePreview();
void StartPick();
void EndPick(bool accept);

//+-------------------------------------
//|Limits v to the range [lo, hi].
//+---------------------------------------------------------------------------
//|Clamp
//+---------------------------------------------------------------------------
int Clamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

//+-------------------------------------
//|Removes leading and trailing whitespace.
//+---------------------------------------------------------------------------
//|Trim
//+---------------------------------------------------------------------------
std::wstring Trim(const std::wstring& s) {
    size_t a = 0, b = s.size();
    while (a < b && iswspace(s[a])) a++;
    while (b > a && iswspace(s[b - 1])) b--;
    return s.substr(a, b - a);
}

//+-------------------------------------
//|True while the key is physically held down.
//+---------------------------------------------------------------------------
//|KeyDown
//+---------------------------------------------------------------------------
bool KeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

//+-------------------------------------
//|True for the four arrow keys.
//+---------------------------------------------------------------------------
//|IsArrowKey
//+---------------------------------------------------------------------------
bool IsArrowKey(WPARAM vk) {
    return vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN;
}

//+-------------------------------------
//|Window text as a std::wstring.
//+---------------------------------------------------------------------------
//|GetText
//+---------------------------------------------------------------------------
std::wstring GetText(HWND hwnd) {
    int len = GetWindowTextLengthW(hwnd);
    std::wstring s(len + 1, L'\0');
    GetWindowTextW(hwnd, s.data(), len + 1);
    s.resize(len);
    return s;
}

//+-------------------------------------
//|Scales a 96-DPI value to the given DPI.
//+---------------------------------------------------------------------------
//|Scale
//+---------------------------------------------------------------------------
int Scale(int value, UINT dpi) {
    return MulDiv(value, (int)dpi, 96);
}

//+-------------------------------------
//|DPI of a window, 96 if unknown.
//+---------------------------------------------------------------------------
//|WindowDpi
//+---------------------------------------------------------------------------
UINT WindowDpi(HWND hwnd) {
    UINT dpi = GetDpiForWindow(hwnd);
    return dpi ? dpi : 96;
}

//+-------------------------------------
//|Visible window bounds (without the invisible resize border).
//+---------------------------------------------------------------------------
//|GetFrameRect
//+---------------------------------------------------------------------------
RECT GetFrameRect(HWND hwnd) {
    RECT r = {};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &r,
                                     sizeof(r)))) {
        GetWindowRect(hwnd, &r);
    }
    return r;
}

//+-------------------------------------
//|True once the mod is being unloaded.
//+---------------------------------------------------------------------------
//|Stopping
//+---------------------------------------------------------------------------
bool Stopping() {
    return g_stop;
}

//+-------------------------------------
//|Sleeps in short steps; returns false early if the mod is unloading.
//+---------------------------------------------------------------------------
//|SleepUnlessStopping
//+---------------------------------------------------------------------------
bool SleepUnlessStopping(int ms) {
    for (int waited = 0; waited < ms; waited += 10) {
        if (Stopping()) return false;
        Sleep(10);
    }
    return !Stopping();
}

//+-------------------------------------
//|Fallback drawing area, estimated from the window frame.
//+---------------------------------------------------------------------------
//|FallbackSafeRect
//+---------------------------------------------------------------------------
RECT FallbackSafeRect(HWND target) {
    RECT r = GetFrameRect(target);
    UINT dpi = WindowDpi(target);
    r.left += Scale(kEditorEdgeInset, dpi);
    r.right -= Scale(kEditorEdgeInset, dpi);
    r.top += Scale(kEditorTopInset, dpi);
    r.bottom -= Scale(kEditorEdgeInset, dpi);
    return r;
}

//+-------------------------------------
//|Moves a point inside a rectangle (unchanged if the rectangle is empty).
//+---------------------------------------------------------------------------
//|ClampToRect
//+---------------------------------------------------------------------------
POINT ClampToRect(POINT pt, const RECT& r) {
    if (IsRectEmpty(&r)) return pt;
    pt.x = Clamp(pt.x, r.left, r.right - 1);
    pt.y = Clamp(pt.y, r.top, r.bottom - 1);
    return pt;
}

//+-------------------------------------
//|Work area of the monitor that contains the point.
//+---------------------------------------------------------------------------
//|GetWorkArea
//+---------------------------------------------------------------------------
RECT GetWorkArea(POINT pt) {
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    return mi.rcWork;
}

//+-------------------------------------
//|Reads a string setting into a std::wstring (empty if missing).
//+---------------------------------------------------------------------------
//|ReadStringSetting
//+---------------------------------------------------------------------------
std::wstring ReadStringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value;  // never null
    Wh_FreeStringSetting(value);
    return result;
}

//+-------------------------------------
//|Applies one hotkey token (modifier or key). False if unknown.
//+---------------------------------------------------------------------------
//|ParseHotkeyToken
//+---------------------------------------------------------------------------
bool ParseHotkeyToken(const std::wstring& tok, UINT* mods, UINT* vk) {
    if (tok == L"ctrl" || tok == L"control") {
        *mods |= MOD_CONTROL;
    } else if (tok == L"alt") {
        *mods |= MOD_ALT;
    } else if (tok == L"shift") {
        *mods |= MOD_SHIFT;
    } else if (tok == L"win" || tok == L"windows") {
        *mods |= MOD_WIN;
    } else if (tok.size() == 1 && ((tok[0] >= L'a' && tok[0] <= L'z') ||
                                   (tok[0] >= L'0' && tok[0] <= L'9'))) {
        *vk = (UINT)towupper(tok[0]);
    } else if (tok.size() >= 2 && tok[0] == L'f') {
        int n = _wtoi(tok.c_str() + 1);
        if (n < 1 || n > 24) return false;
        *vk = VK_F1 + (n - 1);
    } else {
        return false;
    }
    return true;
}

//+-------------------------------------
//|Parses text like "Ctrl+Alt+Shift+T" or "Ctrl+F2".
//+---------------------------------------------------------------------------
//|ParseHotkey
//+---------------------------------------------------------------------------
bool ParseHotkey(const std::wstring& s, UINT* modsOut, UINT* vkOut) {
    UINT mods = 0, vk = 0;
    size_t start = 0;
    while (start <= s.size()) {
        size_t plus = s.find(L'+', start);
        if (plus == std::wstring::npos) plus = s.size();
        std::wstring tok = Trim(s.substr(start, plus - start));
        for (auto& c : tok) c = (wchar_t)towlower(c);
        start = plus + 1;
        if (tok.empty()) continue;
        if (!ParseHotkeyToken(tok, &mods, &vk)) return false;
    }
    if (!vk || !mods) return false;
    *modsOut = mods;
    *vkOut = vk;
    return true;
}

//+-------------------------------------
//|Interface language: automatic (Windows language), English or Portuguese.
//+---------------------------------------------------------------------------
//|ReadLanguageSetting
//+---------------------------------------------------------------------------
void ReadLanguageSetting(Settings* s) {
    std::wstring lang = ReadStringSetting(L"language");
    if (lang == L"en") s->language = Language::English;
    else if (lang == L"pt") s->language = Language::Portuguese;
    else s->language = Language::Auto;
}

//+-------------------------------------
//|Hotkey setting, with Ctrl+Alt+Shift+T as fallback.
//+---------------------------------------------------------------------------
//|ReadHotkeySetting
//+---------------------------------------------------------------------------
void ReadHotkeySetting(Settings* s) {
    std::wstring hotkey = ReadStringSetting(L"hotkey");
    if (ParseHotkey(hotkey, &s->mods, &s->vk)) return;
    Wh_Log(L"Invalid hotkey '%s', using Ctrl+Alt+Shift+T", hotkey.c_str());
    s->mods = MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    s->vk = 'T';
}

//+-------------------------------------
//|Writing speed, default letter height, Alt+Arrow step, mouse lock and
//|dialog position.
//+---------------------------------------------------------------------------
//|ReadWritingSettings
//+---------------------------------------------------------------------------
void ReadWritingSettings(Settings* s) {
    int speed = _wtoi(ReadStringSetting(L"writeSpeed").c_str());
    s->speed = speed > 0 ? Clamp(speed, kMinSpeed, kMaxSpeed) : 3;

    int size = Wh_GetIntSetting(L"defaultSize");
    s->defaultSize = size > 0 ? Clamp(size, kMinSize, kMaxSize) : 28;

    int nudge = Wh_GetIntSetting(L"nudgeStep");
    s->nudgeStep = nudge > 0 ? Clamp(nudge, kMinNudge, kMaxNudge) : 10;

    s->blockMouse = Wh_GetIntSetting(L"blockMouse") != 0;
    s->dialogBottomCenter = ReadStringSetting(L"dialogPosition") != L"auto";
}

//+-------------------------------------
//|Floating button visibility, corner and offsets.
//+---------------------------------------------------------------------------
//|ReadButtonSettings
//+---------------------------------------------------------------------------
void ReadButtonSettings(Settings* s) {
    s->showButton = Wh_GetIntSetting(L"showButton") != 0;

    std::wstring corner = ReadStringSetting(L"buttonCorner");
    if (corner == L"topLeft") s->corner = Corner::TopLeft;
    else if (corner == L"bottomLeft") s->corner = Corner::BottomLeft;
    else if (corner == L"bottomRight") s->corner = Corner::BottomRight;
    else s->corner = Corner::TopRight;

    s->offsetX = Clamp(Wh_GetIntSetting(L"buttonOffsetX"), -2000, 5000);
    s->offsetY = Clamp(Wh_GetIntSetting(L"buttonOffsetY"), -2000, 5000);
}

//+-------------------------------------
//|Preview color: pen color (default), Windows accent or a fixed color.
//+---------------------------------------------------------------------------
//|ReadPreviewColorSetting
//+---------------------------------------------------------------------------
void ReadPreviewColorSetting(Settings* s) {
    std::wstring col = ReadStringSetting(L"previewColor");
    s->previewAccent = false;
    s->previewFromPen = false;
    if (col == L"pen" || col.empty()) s->previewFromPen = true;
    else if (col == L"blue") s->previewColor = RGB(0, 99, 177);
    else if (col == L"green") s->previewColor = RGB(16, 124, 16);
    else if (col == L"yellow") s->previewColor = RGB(255, 200, 0);
    else if (col == L"black") s->previewColor = RGB(0, 0, 0);
    else if (col == L"white") s->previewColor = RGB(255, 255, 255);
    else if (col == L"accent") s->previewAccent = true;
    else s->previewColor = RGB(232, 17, 35);
}

//+-------------------------------------
//|Reads all mod settings into g_settings.
//+---------------------------------------------------------------------------
//|LoadSettings
//+---------------------------------------------------------------------------
void LoadSettings() {
    Settings s;
    ReadLanguageSetting(&s);
    ReadHotkeySetting(&s);
    ReadWritingSettings(&s);
    ReadButtonSettings(&s);
    ReadPreviewColorSetting(&s);
    g_settings = s;
}

//+-------------------------------------
//|True when the dialog should be shown in Portuguese.
//+---------------------------------------------------------------------------
//|UsePortuguese
//+---------------------------------------------------------------------------
bool UsePortuguese() {
    if (g_settings.language != Language::Auto)
        return g_settings.language == Language::Portuguese;
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE;
}

//+-------------------------------------
//|Dialog texts in the current language.
//+---------------------------------------------------------------------------
//|Text
//+---------------------------------------------------------------------------
const UiStrings& Text() {
    return UsePortuguese() ? kStringsPt : kStringsEn;
}

//+-------------------------------------
//|Maps speed 1..10 to point/stroke delays (geometric scale).
//+---------------------------------------------------------------------------
//|SpeedToDelays
//+---------------------------------------------------------------------------
void SpeedToDelays(int speed, int* pointUs, int* strokeMs) {
    double t = (Clamp(speed, kMinSpeed, kMaxSpeed) - 1) / 9.0;
    double f = std::pow(0.05, t);
    *pointUs = (int)std::lround(6000.0 * f);
    *strokeMs = (int)std::lround(40.0 * f);
}

//+-------------------------------------
//|Minimal WinRT ABI declarations, so no WinRT headers or import libraries
//|are needed. Only the vtable slots up to the methods used are declared.
//+---------------------------------------------------------------------------
//|Pen settings: WinRT interfaces
//+---------------------------------------------------------------------------
struct IAppDataStaticsAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Current(IInspectable** value) = 0;
};
struct IAppDataAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Version(UINT32* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVersionAsync(UINT32, void*, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearAllAsync(void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearAsync(int, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LocalSettings(IInspectable** value) = 0;
};
struct IAppDataContainerAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Name(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Locality(int* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Values(IInspectable** value) = 0;
};
struct IMapStringObjectAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Lookup(HSTRING key, IInspectable** value) = 0;
};
struct IPropertyValueAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Type(int* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsNumericScalar(boolean* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUInt8(BYTE* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInt16(INT16* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUInt16(UINT16* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInt32(INT32* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUInt32(UINT32* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInt64(INT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUInt64(UINT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSingle(FLOAT* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDouble(DOUBLE* value) = 0;
};

//+---------------------------------------------------------------------------
//|Interface IDs: IApplicationDataStatics, IApplicationData,
//|IApplicationDataContainer, IMap<String, Object>, IPropertyValue.
constexpr GUID kIidAppDataStatics = {
    0x5612147b, 0xe843, 0x45e3, {0x94, 0xd8, 0x06, 0x16, 0x9e, 0x3c, 0x8e, 0x17}};
constexpr GUID kIidAppData = {
    0xc3da6fb7, 0xb744, 0x4b45, {0xb0, 0xb8, 0x22, 0x3a, 0x09, 0x38, 0xd0, 0xdc}};
constexpr GUID kIidAppDataContainer = {
    0xc5aefd1e, 0xf467, 0x40ba, {0x85, 0x66, 0xab, 0x64, 0x0a, 0x44, 0x1e, 0x1d}};
constexpr GUID kIidMapStringObject = {
    0x1b0d3570, 0x0877, 0x5ec2, {0x8a, 0x2c, 0x3b, 0x95, 0x39, 0x50, 0x6a, 0xca}};
constexpr GUID kIidPropertyValue = {
    0x4bd682dd, 0x7554, 0x40e9, {0x9a, 0x9b, 0x82, 0x65, 0x4e, 0xde, 0x7e, 0x62}};

//+---------------------------------------------------------------------------
//|combase.dll functions, loaded at runtime.
struct ComBaseApi {
    HRESULT(WINAPI* RoInitialize)(int) = nullptr;
    void(WINAPI* RoUninitialize)() = nullptr;
    HRESULT(WINAPI* RoGetActivationFactory)(HSTRING, REFIID, void**) = nullptr;
    HRESULT(WINAPI* WindowsCreateString)(LPCWSTR, UINT32, HSTRING*) = nullptr;
    HRESULT(WINAPI* WindowsDeleteString)(HSTRING) = nullptr;
};

//+---------------------------------------------------------------------------
//|Raw result of reading PenBrushIndex / PenWidth.
struct PenReadResult {
    bool hasIndex = false;
    bool hasWidth = false;
    double index = -1;
    double width = 0;
    HRESULT hr = S_OK;
};

//+-------------------------------------
//|Resolves an exported function into a typed pointer.
//+---------------------------------------------------------------------------
//|LoadProc
//+---------------------------------------------------------------------------
template <typename T>
void LoadProc(HMODULE m, const char* name, T* out) {
    *out = reinterpret_cast<T>(reinterpret_cast<void*>(GetProcAddress(m, name)));
}

//+-------------------------------------
//|Loads the combase.dll functions. False if any is missing.
//+---------------------------------------------------------------------------
//|GetComBaseApi
//+---------------------------------------------------------------------------
bool GetComBaseApi(ComBaseApi* api) {
    HMODULE m = LoadLibraryExW(L"combase.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!m) return false;
    LoadProc(m, "RoInitialize", &api->RoInitialize);
    LoadProc(m, "RoUninitialize", &api->RoUninitialize);
    LoadProc(m, "RoGetActivationFactory", &api->RoGetActivationFactory);
    LoadProc(m, "WindowsCreateString", &api->WindowsCreateString);
    LoadProc(m, "WindowsDeleteString", &api->WindowsDeleteString);
    return api->RoInitialize && api->RoUninitialize &&
           api->RoGetActivationFactory && api->WindowsCreateString &&
           api->WindowsDeleteString;
}

//+-------------------------------------
//|Releases a COM pointer and clears it.
//+---------------------------------------------------------------------------
//|SafeRelease
//+---------------------------------------------------------------------------
template <typename T>
void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

//+-------------------------------------
//|Reads a numeric value (double or int32) from the settings map.
//+---------------------------------------------------------------------------
//|LookupNumber
//+---------------------------------------------------------------------------
bool LookupNumber(const ComBaseApi& api, IMapStringObjectAbi* map,
                  const wchar_t* key, double* out) {
    HSTRING hkey = nullptr;
    if (FAILED(api.WindowsCreateString(key, (UINT32)wcslen(key), &hkey)))
        return false;
    IInspectable* obj = nullptr;
    HRESULT hr = map->Lookup(hkey, &obj);
    api.WindowsDeleteString(hkey);
    if (FAILED(hr) || !obj) return false;

    bool ok = false;
    IPropertyValueAbi* pv = nullptr;
    if (SUCCEEDED(obj->QueryInterface(kIidPropertyValue, (void**)&pv))) {
        INT32 i = 0;
        DOUBLE d = 0;
        if (SUCCEEDED(pv->GetDouble(&d))) {
            *out = d;
            ok = true;
        } else if (SUCCEEDED(pv->GetInt32(&i))) {
            *out = i;
            ok = true;
        }
    }
    SafeRelease(pv);
    SafeRelease(obj);
    return ok;
}

//+-------------------------------------
//|Gets ApplicationData.Current.LocalSettings.Values as a map.
//+---------------------------------------------------------------------------
//|OpenLocalSettingsValues
//+---------------------------------------------------------------------------
HRESULT OpenLocalSettingsValues(const ComBaseApi& api,
                                IMapStringObjectAbi** values) {
    IAppDataStaticsAbi* statics = nullptr;
    IInspectable* currentInsp = nullptr;
    IAppDataAbi* appData = nullptr;
    IInspectable* localInsp = nullptr;
    IAppDataContainerAbi* local = nullptr;
    IInspectable* valuesInsp = nullptr;

    const wchar_t cls[] = L"Windows.Storage.ApplicationData";
    HSTRING hcls = nullptr;
    HRESULT hr = api.WindowsCreateString(cls, (UINT32)wcslen(cls), &hcls);
    if (SUCCEEDED(hr))
        hr = api.RoGetActivationFactory(hcls, kIidAppDataStatics, (void**)&statics);
    if (hcls) api.WindowsDeleteString(hcls);
    if (SUCCEEDED(hr)) hr = statics->get_Current(&currentInsp);
    if (SUCCEEDED(hr))
        hr = currentInsp->QueryInterface(kIidAppData, (void**)&appData);
    if (SUCCEEDED(hr)) hr = appData->get_LocalSettings(&localInsp);
    if (SUCCEEDED(hr))
        hr = localInsp->QueryInterface(kIidAppDataContainer, (void**)&local);
    if (SUCCEEDED(hr)) hr = local->get_Values(&valuesInsp);
    if (SUCCEEDED(hr))
        hr = valuesInsp->QueryInterface(kIidMapStringObject, (void**)values);

    SafeRelease(valuesInsp);
    SafeRelease(local);
    SafeRelease(localInsp);
    SafeRelease(appData);
    SafeRelease(currentInsp);
    SafeRelease(statics);
    return hr;
}

//+---------------------------------------------------------------------------
//|combase.dll functions, loaded by the worker thread together with its
//|WinRT apartment (see WorkerThread).
ComBaseApi g_comApi;
bool g_comReady = false;

//+-------------------------------------
//|Reads the pen values through WinRT (current, in-memory values).
//|Runs on the worker thread, which owns the WinRT apartment.
//+---------------------------------------------------------------------------
//|ReadPenWinRt
//+---------------------------------------------------------------------------
bool ReadPenWinRt(PenReadResult* res) {
    if (!g_comReady) return false;
    IMapStringObjectAbi* values = nullptr;
    HRESULT hr = OpenLocalSettingsValues(g_comApi, &values);
    if (SUCCEEDED(hr)) {
        res->hasIndex = LookupNumber(g_comApi, values, L"PenBrushIndex", &res->index);
        res->hasWidth = LookupNumber(g_comApi, values, L"PenWidth", &res->width);
    }
    SafeRelease(values);
    res->hr = hr;
    if (FAILED(hr)) {
        Wh_Log(L"Pen read (WinRT) failed: 0x%08X", (unsigned)hr);
        return false;
    }
    return res->hasIndex || res->hasWidth;
}

//+-------------------------------------
//|Package family name of this process (Snipping Tool's by default).
//+---------------------------------------------------------------------------
//|GetPackageFamily
//+---------------------------------------------------------------------------
std::wstring GetPackageFamily() {
    std::wstring family = L"Microsoft.ScreenSketch_8wekyb3d8bbwe";
    using GetPfnFn = LONG(WINAPI*)(UINT32*, PWSTR);
    GetPfnFn getPfn = nullptr;
    LoadProc(GetModuleHandleW(L"kernel32.dll"), "GetCurrentPackageFamilyName",
             &getPfn);
    if (getPfn) {
        wchar_t pfn[256];
        UINT32 len = 256;
        if (getPfn(&len, pfn) == ERROR_SUCCESS) family = pfn;
    }
    return family;
}

//+-------------------------------------
//|Path of the app settings hive (Settings\settings.dat).
//+---------------------------------------------------------------------------
//|SettingsDatPath
//+---------------------------------------------------------------------------
std::wstring SettingsDatPath() {
    wchar_t base[MAX_PATH] = L"";
    ExpandEnvironmentStringsW(L"%LOCALAPPDATA%", base, MAX_PATH);
    return std::wstring(base) + L"\\Packages\\" + GetPackageFamily() +
           L"\\Settings\\settings.dat";
}

//+-------------------------------------
//|Reads a LocalSettings value from the hive. Values are stored as raw data
//|followed by an 8-byte timestamp, so only the first bytes are used.
//+---------------------------------------------------------------------------
//|QueryAppValue
//+---------------------------------------------------------------------------
bool QueryAppValue(HKEY key, const wchar_t* name, void* out, DWORD outSize) {
    BYTE buf[64];
    DWORD size = sizeof(buf), type = 0;
    if (RegQueryValueExW(key, name, nullptr, &type, buf, &size) != ERROR_SUCCESS)
        return false;
    if (size < outSize) return false;
    memcpy(out, buf, outSize);
    return true;
}

//+-------------------------------------
//|Fallback: reads the pen values directly from the settings hive file.
//+---------------------------------------------------------------------------
//|ReadPenRegistry
//+---------------------------------------------------------------------------
bool ReadPenRegistry(PenReadResult* res) {
    std::wstring path = SettingsDatPath();
    HKEY hive = nullptr;
    LONG err = RegLoadAppKeyW(path.c_str(), &hive, KEY_READ, 0, 0);
    if (err != ERROR_SUCCESS) {
        Wh_Log(L"Pen read (file) failed (%ld): %s", err, path.c_str());
        return false;
    }
    HKEY local = nullptr;
    if (RegOpenKeyExW(hive, L"LocalState", 0, KEY_READ, &local) ==
        ERROR_SUCCESS) {
        int index = -1;
        double width = 0;
        res->hasIndex = QueryAppValue(local, L"PenBrushIndex", &index, sizeof(index));
        res->hasWidth = QueryAppValue(local, L"PenWidth", &width, sizeof(width));
        res->index = index;
        res->width = width;
        RegCloseKey(local);
    }
    RegCloseKey(hive);
    return res->hasIndex || res->hasWidth;
}

//+-------------------------------------
//|Converts the raw values into a PenInfo (palette color + width).
//+---------------------------------------------------------------------------
//|ToPenInfo
//+---------------------------------------------------------------------------
PenInfo ToPenInfo(const PenReadResult& res) {
    PenInfo info;
    int index = (int)std::lround(res.index);
    if (res.hasIndex && index >= 0 && index < kPenPaletteCount) {
        info.color = kPenPalette[index];
        info.ok = true;
    }
    if (res.hasWidth && res.width > 0 && res.width < 200) info.width = res.width;
    return info;
}

//+-------------------------------------
//|Updates g_pen with the Snipping Tool's current pen color and width.
//+---------------------------------------------------------------------------
//|ReadPenInfo
//+---------------------------------------------------------------------------
void ReadPenInfo() {
    PenReadResult res;
    const wchar_t* source = L"WinRT";
    if (!ReadPenWinRt(&res)) {
        res = PenReadResult();
        source = L"file";
        if (!ReadPenRegistry(&res)) {
            g_pen = PenInfo();
            return;
        }
    }
    g_pen = ToPenInfo(res);
    Wh_Log(L"Pen (%s): index %d, width x10 = %d", source,
           res.hasIndex ? (int)std::lround(res.index) : -1,
           (int)std::lround(g_pen.width * 10));
}

//+-------------------------------------
//|Reads a DWORD from HKEY_CURRENT_USER.
//+---------------------------------------------------------------------------
//|ReadDword
//+---------------------------------------------------------------------------
bool ReadDword(const wchar_t* subKey, const wchar_t* name, DWORD* out) {
    DWORD size = sizeof(DWORD);
    return RegGetValueW(HKEY_CURRENT_USER, subKey, name, RRF_RT_REG_DWORD,
                        nullptr, out, &size) == ERROR_SUCCESS;
}

//+-------------------------------------
//|Mixes color a towards b by t (0..1).
//+---------------------------------------------------------------------------
//|Blend
//+---------------------------------------------------------------------------
COLORREF Blend(COLORREF a, COLORREF b, double t) {
    auto mix = [t](int x, int y) { return (int)std::lround(x + (y - x) * t); };
    return RGB(mix(GetRValue(a), GetRValue(b)), mix(GetGValue(a), GetGValue(b)),
               mix(GetBValue(a), GetBValue(b)));
}

//+-------------------------------------
//|True when Windows apps use the dark theme.
//+---------------------------------------------------------------------------
//|IsDarkMode
//+---------------------------------------------------------------------------
bool IsDarkMode() {
    DWORD light = 1;
    ReadDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
              L"AppsUseLightTheme", &light);
    return light == 0;
}

//+-------------------------------------
//|Neutral colors for the dark or light theme.
//+---------------------------------------------------------------------------
//|SetBaseColors
//+---------------------------------------------------------------------------
void SetBaseColors(Theme* t) {
    if (t->dark) {
        t->bg = RGB(32, 32, 32);
        t->surface = RGB(45, 45, 45);
        t->surfaceHover = RGB(56, 56, 56);
        t->surfacePressed = RGB(39, 39, 39);
        t->border = RGB(72, 72, 72);
        t->text = RGB(255, 255, 255);
        t->textSecondary = RGB(200, 200, 200);
        t->warning = RGB(255, 153, 164);
    } else {
        t->bg = RGB(243, 243, 243);
        t->surface = RGB(255, 255, 255);
        t->surfaceHover = RGB(246, 246, 246);
        t->surfacePressed = RGB(235, 235, 235);
        t->border = RGB(210, 210, 210);
        t->text = RGB(26, 26, 26);
        t->textSecondary = RGB(96, 96, 96);
        t->warning = RGB(196, 43, 28);
    }
}

//+-------------------------------------
//|Windows accent color, its pressed variant and a readable text color.
//+---------------------------------------------------------------------------
//|SetAccentColors
//+---------------------------------------------------------------------------
void SetAccentColors(Theme* t) {
    DWORD accent = 0;
    if (ReadDword(L"Software\\Microsoft\\Windows\\DWM", L"AccentColor", &accent)) {
        t->accent = (COLORREF)(accent & 0x00FFFFFF);  // 0xAABBGGRR
    } else {
        t->accent = RGB(0, 120, 212);
    }
    if (t->dark) t->accent = Blend(t->accent, RGB(255, 255, 255), 0.25);
    t->accentPressed = Blend(t->accent,
                             t->dark ? RGB(0, 0, 0) : RGB(255, 255, 255), 0.2);
    double lum = 0.2126 * GetRValue(t->accent) + 0.7152 * GetGValue(t->accent) +
                 0.0722 * GetBValue(t->accent);
    t->accentText = lum > 150 ? RGB(0, 0, 0) : RGB(255, 255, 255);
}

//+-------------------------------------
//|Recreates the shared background brushes.
//+---------------------------------------------------------------------------
//|RecreateBrushes
//+---------------------------------------------------------------------------
void RecreateBrushes() {
    if (g_bgBrush) DeleteObject(g_bgBrush);
    if (g_surfaceBrush) DeleteObject(g_surfaceBrush);
    g_bgBrush = CreateSolidBrush(g_theme.bg);
    g_surfaceBrush = CreateSolidBrush(g_theme.surface);
}

//+-------------------------------------
//|Loads the current Windows theme into g_theme.
//+---------------------------------------------------------------------------
//|LoadTheme
//+---------------------------------------------------------------------------
void LoadTheme() {
    Theme t;
    t.dark = IsDarkMode();
    SetBaseColors(&t);
    SetAccentColors(&t);
    g_theme = t;
    RecreateBrushes();
}

//+-------------------------------------
//|True for messages sent when the theme or accent color changes.
//+---------------------------------------------------------------------------
//|IsThemeChangeMessage
//+---------------------------------------------------------------------------
bool IsThemeChangeMessage(UINT msg, LPARAM lParam) {
    if (msg == WM_DWMCOLORIZATIONCOLORCHANGED) return true;
    return msg == WM_SETTINGCHANGE && lParam &&
           lstrcmpiW((LPCWSTR)lParam, L"ImmersiveColorSet") == 0;
}

//+-------------------------------------
//|Dark title bar flag, rounded corners and border color (Windows 11).
//+---------------------------------------------------------------------------
//|ApplyWindowTheme
//+---------------------------------------------------------------------------
void ApplyWindowTheme(HWND hwnd, bool roundSmall, bool roundLarge = false) {
    BOOL dark = g_theme.dark;
    DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark,
                          sizeof(dark));
    if (!roundSmall && !roundLarge) return;
    int pref = roundLarge ? 2 /* DWMWCP_ROUND */ : 3 /* DWMWCP_ROUNDSMALL */;
    DwmSetWindowAttribute(hwnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &pref,
                          sizeof(pref));
    COLORREF border = g_theme.border;
    DwmSetWindowAttribute(hwnd, 34 /* DWMWA_BORDER_COLOR */, &border,
                          sizeof(border));
}

//+-------------------------------------
//|Reloads the theme and repaints every window of the mod.
//+---------------------------------------------------------------------------
//|RefreshThemedWindows
//+---------------------------------------------------------------------------
void RefreshThemedWindows() {
    LoadTheme();
    if (g_btn.hwnd) {
        ApplyWindowTheme(g_btn.hwnd, true);
        InvalidateRect(g_btn.hwnd, nullptr, FALSE);
    }
    if (g_dlg.hwnd) {
        ApplyWindowTheme(g_dlg.hwnd, false, true);
        RedrawWindow(g_dlg.hwnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
    }
    UpdatePreview();
}

//+-------------------------------------
//|Queues one theme refresh; several windows receive the same broadcast.
//+---------------------------------------------------------------------------
//|QueueThemeRefresh
//+---------------------------------------------------------------------------
void QueueThemeRefresh() {
    if (g_themePending || !g_msgWnd) return;
    g_themePending = true;
    PostMessageW(g_msgWnd, WM_APP_THEME, 0, 0);
}

//+-------------------------------------
//|Segoe UI font at the given size and DPI.
//+---------------------------------------------------------------------------
//|MakeFont
//+---------------------------------------------------------------------------
HFONT MakeFont(UINT dpi, int points, int weight,
               const wchar_t* face = L"Segoe UI") {
    return CreateFontW(-MulDiv(points, (int)dpi, 72), 0, 0, 0, weight, FALSE,
                       FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, face);
}

//+-------------------------------------
//|Rounded rectangle with fill and 1 px border.
//+---------------------------------------------------------------------------
//|FillRoundRect
//+---------------------------------------------------------------------------
void FillRoundRect(HDC hdc, const RECT& rc, int radius, COLORREF fill,
                   COLORREF border) {
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

//+-------------------------------------
//|Single-line centered text in the given font and color.
//+---------------------------------------------------------------------------
//|DrawCenteredText
//+---------------------------------------------------------------------------
void DrawCenteredText(HDC hdc, const wchar_t* text, RECT rc, HFONT font,
                      COLORREF color) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    HGDIOBJ old = SelectObject(hdc, font);
    DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old);
}

//+---------------------------------------------------------------------------
//|UI Automation IDs (local constants, no uuid library needed).
constexpr GUID kClsidCUIAutomation = {
    0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
constexpr GUID kIidIUIAutomation = {
    0x30cbe57d, 0xd9d0, 0x452a, {0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee}};

//+---------------------------------------------------------------------------
//|AutomationId of the Snipping Tool's annotation canvas. It only exists
//|while an image is open in the editor.
constexpr wchar_t kCanvasAutomationId[] = L"AnnotateInkCanvas";

//+-------------------------------------
//|Creates the UI Automation client (COM must be initialized on this thread).
//+---------------------------------------------------------------------------
//|CreateUiAutomation
//+---------------------------------------------------------------------------
void CreateUiAutomation() {
    HRESULT hr = CoCreateInstance(kClsidCUIAutomation, nullptr,
                                  CLSCTX_INPROC_SERVER, kIidIUIAutomation,
                                  (void**)&g_uia);
    if (FAILED(hr)) {
        g_uia = nullptr;
        Wh_Log(L"UI Automation unavailable (0x%08X)", (unsigned)hr);
    }
}

//+-------------------------------------
//|Releases the UI Automation client.
//+---------------------------------------------------------------------------
//|ReleaseUiAutomation
//+---------------------------------------------------------------------------
void ReleaseUiAutomation() {
    if (g_uia) g_uia->Release();
    g_uia = nullptr;
}

//+-------------------------------------
//|Finds the annotation canvas inside a window. Returns false if there is
//|none (no image open) or it can't be queried.
//+---------------------------------------------------------------------------
//|FindCanvasRect
//+---------------------------------------------------------------------------
bool FindCanvasRect(HWND hwnd, RECT* rc) {
    if (!g_uia || !hwnd) return false;
    IUIAutomationElement* root = nullptr;
    IUIAutomationCondition* cond = nullptr;
    IUIAutomationElement* canvas = nullptr;
    bool found = false;

    VARIANT id;
    VariantInit(&id);
    id.vt = VT_BSTR;
    id.bstrVal = SysAllocString(kCanvasAutomationId);
    if (SUCCEEDED(g_uia->ElementFromHandle(hwnd, &root)) && root &&
        SUCCEEDED(g_uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, id,
                                                 &cond)) &&
        SUCCEEDED(root->FindFirst(TreeScope_Descendants, cond, &canvas)) &&
        canvas) {
        RECT r = {};
        if (SUCCEEDED(canvas->get_CurrentBoundingRectangle(&r)) &&
            r.right > r.left && r.bottom > r.top) {
            *rc = r;
            found = true;
        }
    }
    VariantClear(&id);
    if (canvas) canvas->Release();
    if (cond) cond->Release();
    if (root) root->Release();
    return found;
}

//+-------------------------------------
//|True if UI Automation can be used to detect the editor state.
//+---------------------------------------------------------------------------
//|CanDetectCanvas
//+---------------------------------------------------------------------------
bool CanDetectCanvas() {
    return g_uia != nullptr;
}

//+-------------------------------------
//|Area of the editor where the pen may draw (screen coordinates): the
//|annotation canvas. Empty if UI Automation works but finds no canvas; an
//|estimate from the window frame only if UI Automation is unavailable.
//+---------------------------------------------------------------------------
//|EditorSafeRect
//+---------------------------------------------------------------------------
RECT EditorSafeRect(HWND target) {
    RECT canvas;
    if (!FindCanvasRect(target, &canvas))
        return CanDetectCanvas() ? RECT{} : FallbackSafeRect(target);
    UINT dpi = WindowDpi(target);
    InflateRect(&canvas, -Scale(4, dpi), -Scale(4, dpi));  // small margin
    return canvas;
}

//+---------------------------------------------------------------------------
//|IUIAutomationSelectionItemPattern IID and the toolbar buttons' AutomationIds.
constexpr GUID kIidSelectionItemPattern = {
    0xa8efa66a, 0x0fda, 0x421a, {0x91, 0x94, 0x38, 0x02, 0x1f, 0x35, 0x78, 0xea}};
constexpr wchar_t kPenButtonAutomationId[] = L"InkToolbarBallpointPenButton";
constexpr const wchar_t* kOtherToolAutomationIds[] = {
    L"InkToolbarHighlighterButton",
    L"InkToolbarEraserButton",
};

//+-------------------------------------
//|Selection pattern of a toolbar button, found by AutomationId (nullptr if
//|not found). The caller releases it.
//+---------------------------------------------------------------------------
//|FindToolButton
//+---------------------------------------------------------------------------
IUIAutomationSelectionItemPattern* FindToolButton(HWND target,
                                                  const wchar_t* automationId) {
    if (!g_uia) return nullptr;
    IUIAutomationElement* root = nullptr;
    IUIAutomationCondition* cond = nullptr;
    IUIAutomationElement* button = nullptr;
    IUIAutomationSelectionItemPattern* sel = nullptr;

    VARIANT id;
    VariantInit(&id);
    id.vt = VT_BSTR;
    id.bstrVal = SysAllocString(automationId);
    if (SUCCEEDED(g_uia->ElementFromHandle(target, &root)) && root &&
        SUCCEEDED(g_uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, id,
                                                 &cond)) &&
        SUCCEEDED(root->FindFirst(TreeScope_Descendants, cond, &button)) &&
        button) {
        if (FAILED(button->GetCurrentPatternAs(UIA_SelectionItemPatternId,
                                               kIidSelectionItemPattern,
                                               (void**)&sel))) {
            sel = nullptr;
        }
    }
    VariantClear(&id);
    if (button) button->Release();
    if (cond) cond->Release();
    if (root) root->Release();
    return sel;
}

//+-------------------------------------
//|True if the toolbar button exists and is the selected tool.
//+---------------------------------------------------------------------------
//|IsToolSelected
//+---------------------------------------------------------------------------
bool IsToolSelected(HWND target, const wchar_t* automationId) {
    IUIAutomationSelectionItemPattern* sel = FindToolButton(target, automationId);
    if (!sel) return false;
    BOOL selected = FALSE;
    sel->get_CurrentIsSelected(&selected);
    sel->Release();
    return selected;
}

//+-------------------------------------
//|Selects a toolbar button. Returns true if it was found.
//+---------------------------------------------------------------------------
//|SelectTool
//+---------------------------------------------------------------------------
bool SelectTool(HWND target, const wchar_t* automationId) {
    IUIAutomationSelectionItemPattern* sel = FindToolButton(target, automationId);
    if (!sel) return false;
    sel->Select();
    sel->Release();
    SleepUnlessStopping(100);
    return true;
}

//+-------------------------------------
//|Selects the ballpoint pen if another tool (highlighter, eraser) is active.
//|Returns the AutomationId of that tool, to restore it after writing, or
//|nullptr if nothing has to be restored.
//+---------------------------------------------------------------------------
//|EnsurePenSelected
//+---------------------------------------------------------------------------
const wchar_t* EnsurePenSelected(HWND target) {
    if (!g_uia || IsToolSelected(target, kPenButtonAutomationId)) return nullptr;

    const wchar_t* previous = nullptr;
    for (const wchar_t* toolId : kOtherToolAutomationIds) {
        if (IsToolSelected(target, toolId)) {
            previous = toolId;
            break;
        }
    }
    Wh_Log(L"Selecting the pen tool (previous: %s)",
           previous ? previous : L"unknown");
    return SelectTool(target, kPenButtonAutomationId) ? previous : nullptr;
}

//+-------------------------------------
//|Selects again the tool that was active before writing (if any).
//+---------------------------------------------------------------------------
//|RestoreTool
//+---------------------------------------------------------------------------
void RestoreTool(HWND target, const wchar_t* automationId) {
    if (!automationId || !IsWindow(target)) return;
    Wh_Log(L"Restoring tool %s", automationId);
    SelectTool(target, automationId);
}

//+-------------------------------------
//|Glyph of a character (binary search), nullptr if not in the font.
//+---------------------------------------------------------------------------
//|FindGlyph
//+---------------------------------------------------------------------------
const GlyphIndex* FindGlyph(wchar_t ch) {
    if (ch == 0x00A0) ch = L' ';  // non-breaking space
    int lo = 0, hi = kGlyphCount - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (kGlyphIndex[mid].cp == ch) return &kGlyphIndex[mid];
        if (kGlyphIndex[mid].cp < ch) lo = mid + 1;
        else hi = mid - 1;
    }
    return nullptr;
}

//+-------------------------------------
//|Appends a glyph's strokes, scaled and offset to screen coordinates.
//+---------------------------------------------------------------------------
//|AppendGlyphStrokes
//+---------------------------------------------------------------------------
void AppendGlyphStrokes(const GlyphIndex* g, double ox, double oy, double penX,
                        double penY, double scale, std::vector<Stroke>* out) {
    const short* p = kGlyphData + g->offset;
    int nStrokes = *p++;
    for (int i = 0; i < nStrokes; i++) {
        int n = *p++;
        Stroke st;
        st.reserve(n);
        for (int j = 0; j < n; j++) {
            double x = p[0], y = p[1];
            p += 2;
            st.push_back({ox + (penX + x) * scale,
                          oy + (penY + y - kCapLine) * scale});
        }
        if (st.size() >= 2) out->push_back(std::move(st));
    }
}

//+-------------------------------------
//|Converts text into strokes. (ox, oy) is the top-left of the first line;
//|capPx is the height of uppercase letters in pixels.
//+---------------------------------------------------------------------------
//|LayoutText
//+---------------------------------------------------------------------------
std::vector<Stroke> LayoutText(const std::wstring& text, double ox, double oy,
                               double capPx) {
    std::vector<Stroke> out;
    double scale = capPx / kCapHeight;
    double penX = 0, penY = 0;
    const GlyphIndex* space = FindGlyph(L' ');
    const GlyphIndex* fallback = FindGlyph(L'?');

    for (wchar_t ch : text) {
        if (ch == L'\r') continue;
        if (IS_LOW_SURROGATE(ch)) continue;  // one "?" per character, not per unit
        if (ch == L'\n') {
            penX = 0;
            penY += kLineHeight;
            continue;
        }
        if (ch == L'\t') {
            penX += space->width * 4;
            continue;
        }
        const GlyphIndex* g = FindGlyph(ch);
        if (!g) g = fallback;
        AppendGlyphStrokes(g, ox, oy, penX, penY, scale, &out);
        penX += g->width;
    }
    return out;
}

//+-------------------------------------
//|Adds points so consecutive points are at most maxStep apart.
//+---------------------------------------------------------------------------
//|Densify
//+---------------------------------------------------------------------------
Stroke Densify(const Stroke& st, double maxStep) {
    Stroke out;
    if (st.empty()) return out;
    out.push_back(st[0]);
    for (size_t i = 1; i < st.size(); i++) {
        PtD a = st[i - 1], b = st[i];
        double d = std::hypot(b.x - a.x, b.y - a.y);
        int n = (int)std::ceil(d / maxStep);
        for (int k = 1; k <= n; k++) {
            double t = (double)k / n;
            out.push_back({a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t});
        }
        if (n == 0) out.push_back(b);
    }
    return out;
}

//+-------------------------------------
//|Short delays for the pen strokes, without busy-waiting: a high-resolution
//|waitable timer (Windows 10 1803+), or Sleep as a fallback.
//+---------------------------------------------------------------------------
//|Delayer
//+---------------------------------------------------------------------------
struct Delayer {
    HANDLE timer = nullptr;

    Delayer() {
        timer = CreateWaitableTimerExW(nullptr, nullptr,
                                       0x00000002 /* CREATE_WAITABLE_TIMER_HIGH_RESOLUTION */,
                                       TIMER_ALL_ACCESS);
    }

    ~Delayer() {
        if (timer) CloseHandle(timer);
    }

    void WaitUs(int us) {
        if (us <= 0) return;
        if (!timer) {
            Sleep((us + 999) / 1000);
            return;
        }
        LARGE_INTEGER due;
        due.QuadPart = -(LONGLONG)us * 10;  // relative, 100 ns units
        if (SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE))
            WaitForSingleObject(timer, INFINITE);
    }
};

//+-------------------------------------
//|Moves the mouse to a screen point, optionally pressing/releasing a button.
//+---------------------------------------------------------------------------
//|SendMouse
//+---------------------------------------------------------------------------
void SendMouse(double x, double y, DWORD extraFlags) {
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw < 2 || vh < 2) return;

    INPUT in = {};
    in.type = INPUT_MOUSE;
    in.mi.dx = (LONG)std::lround((x - vx) * 65535.0 / (vw - 1));
    in.mi.dy = (LONG)std::lround((y - vy) * 65535.0 / (vh - 1));
    in.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE |
                    MOUSEEVENTF_VIRTUALDESK | extraFlags;
    SendInput(1, &in, sizeof(in));
}

//+-------------------------------------
//|Stops writing on Esc, mod unload or when the editor loses focus.
//+---------------------------------------------------------------------------
//|ShouldAbort
//+---------------------------------------------------------------------------
bool ShouldAbort(HWND target) {
    if (g_stop) return true;
    if (KeyDown(VK_ESCAPE)) return true;
    HWND fg = GetForegroundWindow();
    return fg && GetAncestor(fg, GA_ROOT) != target;
}

//+-------------------------------------
//|Low-level hook: swallows physical (non-injected) mouse input.
//+---------------------------------------------------------------------------
//|BlockMouseProc
//+---------------------------------------------------------------------------
LRESULT CALLBACK BlockMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* ms = (MSLLHOOKSTRUCT*)lParam;
        if (!(ms->flags & LLMHF_INJECTED)) return 1;
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

//+-------------------------------------
//|Blocks the physical mouse while the text is written. The hook lives on
//|its own thread so it is always serviced quickly.
//+---------------------------------------------------------------------------
//|MouseBlocker
//+---------------------------------------------------------------------------
struct MouseBlocker {
    HANDLE thread = nullptr;
    DWORD threadId = 0;
    HANDLE ready = nullptr;

    static DWORD WINAPI Proc(LPVOID param) {
        auto* self = (MouseBlocker*)param;
        MSG msg;
        PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);  // create the queue
        HHOOK hook = SetWindowsHookExW(WH_MOUSE_LL, BlockMouseProc, g_hInst, 0);
        if (!hook) Wh_Log(L"Mouse lock failed: %u", GetLastError());
        SetEvent(self->ready);
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (hook) UnhookWindowsHookEx(hook);
        return 0;
    }

    void Start() {
        ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        thread = CreateThread(nullptr, 0, Proc, this, 0, &threadId);
        if (thread) WaitForSingleObject(ready, INFINITE);
    }

    void Stop() {
        if (thread) {
            PostThreadMessageW(threadId, WM_QUIT, 0, 0);
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
            thread = nullptr;
        }
        if (ready) {
            CloseHandle(ready);
            ready = nullptr;
        }
    }
};

//+---------------------------------------------------------------------------
//|Timing and button flags used while drawing.
struct DrawParams {
    DWORD down;
    DWORD up;
    int pointDelayUs;
    int strokeDelayUs;
    RECT safe;  // editor area where the pen may draw
    Delayer* delay;
};

//+-------------------------------------
//|Primary button flags (honors swapped buttons) and speed delays.
//+---------------------------------------------------------------------------
//|MakeDrawParams
//+---------------------------------------------------------------------------
DrawParams MakeDrawParams(HWND target, int speed, Delayer* delay) {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON) != 0;
    DrawParams p;
    p.down = swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    p.up = swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
    int strokeMs = 0;
    SpeedToDelays(speed, &p.pointDelayUs, &strokeMs);
    p.strokeDelayUs = strokeMs * 1000;
    p.safe = EditorSafeRect(target);
    p.delay = delay;
    return p;
}

//+-------------------------------------
//|True if the point is inside the editor area and the editor is the window
//|that receives input there (nothing else covers it).
//+---------------------------------------------------------------------------
//|PointOnTarget
//+---------------------------------------------------------------------------
bool PointOnTarget(HWND target, const RECT& safe, const PtD& p) {
    POINT pt = {(LONG)std::lround(p.x), (LONG)std::lround(p.y)};
    if (!PtInRect(&safe, pt)) return false;
    HWND hit = WindowFromPoint(pt);
    return hit && GetAncestor(hit, GA_ROOT) == target;
}

//+-------------------------------------
//|Drags the mouse along one stroke. False if the user aborted or a point
//|falls outside the editor (the pen is lifted before leaving it).
//+---------------------------------------------------------------------------
//|DrawStroke
//+---------------------------------------------------------------------------
bool DrawStroke(HWND target, const Stroke& raw, const DrawParams& p) {
    Stroke st = Densify(raw, 4.0);
    if (!PointOnTarget(target, p.safe, st[0])) {
        Wh_Log(L"Stroke starts outside the editor, stopping");
        return false;
    }

    SendMouse(st[0].x, st[0].y, 0);
    p.delay->WaitUs(p.strokeDelayUs / 2);
    SendMouse(st[0].x, st[0].y, p.down);
    p.delay->WaitUs(p.pointDelayUs);

    size_t last = 0;
    bool ok = true;
    for (size_t i = 1; i < st.size(); i++) {
        if (!PointOnTarget(target, p.safe, st[i])) {
            Wh_Log(L"Stroke leaves the editor, stopping");
            ok = false;
            break;
        }
        SendMouse(st[i].x, st[i].y, 0);
        last = i;
        p.delay->WaitUs(p.pointDelayUs);
        if (ShouldAbort(target)) {
            ok = false;
            break;
        }
    }
    SendMouse(st[last].x, st[last].y, p.up);
    return ok;
}

//+-------------------------------------
//|Writes all strokes with the pen, then restores the cursor position.
//+---------------------------------------------------------------------------
//|DrawStrokes
//+---------------------------------------------------------------------------
void DrawStrokes(HWND target, const std::vector<Stroke>& strokes, int speed) {
    Delayer delay;
    DrawParams p = MakeDrawParams(target, speed, &delay);
    POINT orig;
    GetCursorPos(&orig);

    MouseBlocker blocker;
    if (g_settings.blockMouse) blocker.Start();

    for (const Stroke& raw : strokes) {
        if (ShouldAbort(target)) break;
        if (!DrawStroke(target, raw, p)) break;
        p.delay->WaitUs(p.strokeDelayUs);
    }

    SendMouse(orig.x, orig.y, 0);
    blocker.Stop();
}

//+-------------------------------------
//|EnumChildWindows callback: finds a child owned by this process.
//+---------------------------------------------------------------------------
//|FindOurChildProc
//+---------------------------------------------------------------------------
BOOL CALLBACK FindOurChildProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    *(bool*)lParam = true;
    return FALSE;
}

//+-------------------------------------
//|True if the window belongs to this process, directly or hosted inside an
//|ApplicationFrameHost frame.
//+---------------------------------------------------------------------------
//|BelongsToUs
//+---------------------------------------------------------------------------
bool BelongsToUs(HWND hwnd) {
    if (!hwnd) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) return true;
    wchar_t cls[64] = L"";
    GetClassNameW(hwnd, cls, 64);
    if (lstrcmpW(cls, L"ApplicationFrameWindow") != 0) return false;
    bool found = false;
    EnumChildWindows(hwnd, FindOurChildProc, (LPARAM)&found);
    return found;
}

//+-------------------------------------
//|True for windows created by this mod.
//+---------------------------------------------------------------------------
//|IsOurUiWindow
//+---------------------------------------------------------------------------
bool IsOurUiWindow(HWND hwnd) {
    wchar_t cls[64] = L"";
    if (!GetClassNameW(hwnd, cls, 64)) return false;
    for (const wchar_t* c : {kButtonClass, kDialogClass, kPreviewClass,
                             kPickClass, kHintClass}) {
        if (lstrcmpW(cls, c) == 0) return true;
    }
    return false;
}

//+-------------------------------------
//|True when a Snipping Tool window is in the foreground.
//+---------------------------------------------------------------------------
//|IsOurProcessForeground
//+---------------------------------------------------------------------------
bool IsOurProcessForeground() {
    HWND fg = GetForegroundWindow();
    return fg && BelongsToUs(fg);
}

//+-------------------------------------
//|True for a visible, framed, reasonably large window (the editor).
//+---------------------------------------------------------------------------
//|LooksLikeEditor
//+---------------------------------------------------------------------------
bool LooksLikeEditor(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if ((style & WS_CAPTION) != WS_CAPTION && !(style & WS_THICKFRAME))
        return false;
    RECT r = GetFrameRect(hwnd);
    return r.right - r.left >= 300 && r.bottom - r.top >= 200;
}

//+-------------------------------------
//|Re-checks a window a few times, since its canvas may only be created a
//|moment after it becomes the foreground window.
//+---------------------------------------------------------------------------
//|StartEditorRecheck
//+---------------------------------------------------------------------------
void StartEditorRecheck(HWND hwnd) {
    if (hwnd == g_recheckHwnd) return;
    g_recheckHwnd = hwnd;
    g_recheckLeft = kEditorRecheckTries;
    SetTimer(g_msgWnd, kEditorRecheckTimer, kEditorRecheckMs, nullptr);
}

//+-------------------------------------
//|Stops the re-check timer.
//+---------------------------------------------------------------------------
//|StopEditorRecheck
//+---------------------------------------------------------------------------
void StopEditorRecheck() {
    if (g_msgWnd) KillTimer(g_msgWnd, kEditorRecheckTimer);
    g_recheckLeft = 0;
}

//+-------------------------------------
//|True if the editor shows an image (its annotation canvas exists). The
//|result is cached per window; true if UI Automation is unavailable.
//+---------------------------------------------------------------------------
//|EditorHasImage
//+---------------------------------------------------------------------------
bool EditorHasImage(HWND root) {
    if (!CanDetectCanvas()) return true;
    if (root == g_editorChecked) return g_editorHasImage;
    RECT rc;
    g_editorChecked = root;
    g_editorHasImage = FindCanvasRect(root, &rc);
    if (g_editorHasImage) StopEditorRecheck();
    else StartEditorRecheck(root);
    return g_editorHasImage;
}

//+-------------------------------------
//|Forgets the cached editor state (foreground changed).
//+---------------------------------------------------------------------------
//|ResetEditorCheck
//+---------------------------------------------------------------------------
void ResetEditorCheck() {
    StopEditorRecheck();
    g_editorChecked = nullptr;
    g_recheckHwnd = nullptr;
}

//+-------------------------------------
//|The Snipping Tool editor window, if it is in the foreground and shows an
//|image (the start screen without a capture doesn't count).
//+---------------------------------------------------------------------------
//|FindEditorTarget
//+---------------------------------------------------------------------------
HWND FindEditorTarget() {
    HWND fg = GetForegroundWindow();
    if (!fg) return nullptr;
    HWND root = GetAncestor(fg, GA_ROOT);
    if (!root || IsOurUiWindow(root) || !BelongsToUs(root)) return nullptr;
    if (!LooksLikeEditor(root)) return nullptr;
    return EditorHasImage(root) ? root : nullptr;
}

//+-------------------------------------
//|Preview color: pen color, Windows accent or the configured color.
//+---------------------------------------------------------------------------
//|PreviewColor
//+---------------------------------------------------------------------------
COLORREF PreviewColor() {
    if (g_settings.previewFromPen) return g_pen.color;
    return g_settings.previewAccent ? g_theme.accent : g_settings.previewColor;
}

//+-------------------------------------
//|Preview line width in screen pixels (pen width when available).
//+---------------------------------------------------------------------------
//|PreviewWidth
//+---------------------------------------------------------------------------
int PreviewWidth() {
    if (!g_settings.previewFromPen || !g_pen.ok) return 2;
    int w = (int)std::lround(g_pen.width * g_dlg.dpi / 96.0);
    return Clamp(w, 1, 60);
}

//+-------------------------------------
//|Transparent color key, always different from the preview color.
//+---------------------------------------------------------------------------
//|PreviewKey
//+---------------------------------------------------------------------------
COLORREF PreviewKey() {
    return PreviewColor() == RGB(255, 0, 255) ? RGB(0, 255, 0)
                                               : RGB(255, 0, 255);
}

//+-------------------------------------
//|Draws the text strokes in the preview window.
//+---------------------------------------------------------------------------
//|PaintPreviewStrokes
//+---------------------------------------------------------------------------
void PaintPreviewStrokes(HDC hdc, int ox, int oy) {
    HPEN pen = CreatePen(PS_SOLID, PreviewWidth(), PreviewColor());
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    std::vector<POINT> pts;
    for (const Stroke& st : g_preview.strokes) {
        pts.clear();
        for (const PtD& p : st) {
            pts.push_back({(LONG)std::lround(p.x) - ox,
                           (LONG)std::lround(p.y) - oy});
        }
        Polyline(hdc, pts.data(), (int)pts.size());
    }
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

//+-------------------------------------
//|Draws the small crosshair at the text origin.
//+---------------------------------------------------------------------------
//|PaintCrosshair
//+---------------------------------------------------------------------------
void PaintCrosshair(HDC hdc, int ox, int oy) {
    HPEN pen = CreatePen(PS_SOLID, 1, g_theme.accent);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    int cx = g_dlg.origin.x - ox, cy = g_dlg.origin.y - oy;
    MoveToEx(hdc, cx - 6, cy, nullptr);
    LineTo(hdc, cx + 7, cy);
    MoveToEx(hdc, cx, cy - 6, nullptr);
    LineTo(hdc, cx, cy + 7);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

//+-------------------------------------
//|WM_PAINT of the preview: key-colored background, strokes, crosshair.
//+---------------------------------------------------------------------------
//|OnPreviewPaint
//+---------------------------------------------------------------------------
void OnPreviewPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    HBRUSH bg = CreateSolidBrush(PreviewKey());
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    int ox = g_preview.windowPos.x, oy = g_preview.windowPos.y;
    PaintPreviewStrokes(hdc, ox, oy);
    PaintCrosshair(hdc, ox, oy);
    EndPaint(hwnd, &ps);
}

//+-------------------------------------
//|Click-through layered window that shows where the text will go.
//+---------------------------------------------------------------------------
//|PreviewWndProc
//+---------------------------------------------------------------------------
LRESULT CALLBACK PreviewWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            OnPreviewPaint(hwnd);
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//+-------------------------------------
//|Bounding box of the preview strokes, including the origin.
//+---------------------------------------------------------------------------
//|PreviewBounds
//+---------------------------------------------------------------------------
RECT PreviewBounds() {
    POINT o = g_dlg.origin;
    double minX = o.x - 8, minY = o.y - 8, maxX = o.x + 8, maxY = o.y + 8;
    for (const Stroke& st : g_preview.strokes) {
        for (const PtD& p : st) {
            minX = std::fmin(minX, p.x);
            minY = std::fmin(minY, p.y);
            maxX = std::fmax(maxX, p.x);
            maxY = std::fmax(maxY, p.y);
        }
    }
    int pad = 4 + PreviewWidth() / 2;
    return {(LONG)std::floor(minX) - pad, (LONG)std::floor(minY) - pad,
            (LONG)std::ceil(maxX) + pad, (LONG)std::ceil(maxY) + pad};
}

//+-------------------------------------
//|Re-lays out the text and moves/resizes the preview window.
//+---------------------------------------------------------------------------
//|UpdatePreview
//+---------------------------------------------------------------------------
void UpdatePreview() {
    if (!g_preview.hwnd || !g_dlg.hwnd || g_dlg.hiddenAway) return;
    if (g_dlg.warning) {
        g_dlg.warning = false;
        SetWindowTextW(g_dlg.hint, Text().lineBreakHint);
    }
    POINT o = g_dlg.origin;
    g_preview.strokes = LayoutText(GetText(g_dlg.edit), o.x, o.y, g_dlg.size);

    RECT b = PreviewBounds();
    g_preview.windowPos = {b.left, b.top};
    SetLayeredWindowAttributes(g_preview.hwnd, PreviewKey(), 235,
                               LWA_COLORKEY | LWA_ALPHA);

    //+---------------------------------------------------------------------------
    //|Below the dialog while typing; on top of everything while picking.
    bool dialogVisible = g_mode == Mode::Dialog && g_dlg.hwnd &&
                         IsWindowVisible(g_dlg.hwnd);
    HWND after = dialogVisible ? g_dlg.hwnd : HWND_TOPMOST;
    SetWindowPos(g_preview.hwnd, after, b.left, b.top, b.right - b.left,
                 b.bottom - b.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_preview.hwnd, nullptr, TRUE);
}

//+-------------------------------------
//|Moves the text origin by (dx, dy) pixels.
//+---------------------------------------------------------------------------
//|NudgeOrigin
//+---------------------------------------------------------------------------
void NudgeOrigin(int dx, int dy) {
    g_dlg.origin.x += dx;
    g_dlg.origin.y += dy;
    UpdatePreview();
}

//+-------------------------------------
//|Creates the (still hidden) preview window.
//+---------------------------------------------------------------------------
//|CreatePreviewWindow
//+---------------------------------------------------------------------------
void CreatePreviewWindow(POINT origin) {
    g_preview = PreviewState();
    g_preview.hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kPreviewClass, L"", WS_POPUP, origin.x, origin.y, 1, 1, nullptr,
        nullptr, g_hInst, nullptr);
}

//+-------------------------------------
//|Hides the floating button.
//+---------------------------------------------------------------------------
//|HideButton
//+---------------------------------------------------------------------------
void HideButton() {
    if (g_btn.hwnd && IsWindowVisible(g_btn.hwnd)) ShowWindow(g_btn.hwnd, SW_HIDE);
    g_btn.target = nullptr;
    g_btn.lastRect = {};
}

//+-------------------------------------
//|Draws the floating button: themed square, "T" and an accent ink line.
//+---------------------------------------------------------------------------
//|PaintFloatingButton
//+---------------------------------------------------------------------------
void PaintFloatingButton(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    COLORREF fill = g_btn.pressed ? g_theme.surfacePressed
                    : g_btn.hover ? g_theme.surfaceHover
                                  : g_theme.surface;
    HBRUSH b = CreateSolidBrush(fill);
    FillRect(hdc, &rc, b);
    DeleteObject(b);

    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    RECT tr = rc;
    tr.bottom -= h / 8;
    DrawCenteredText(hdc, L"T", tr, g_btn.font, g_theme.text);

    int lw = w * 5 / 12, th = h >= 36 ? 3 : 2;
    RECT line = {(w - lw) / 2, h - h / 4, (w - lw) / 2 + lw, h - h / 4 + th};
    HBRUSH ab = CreateSolidBrush(g_theme.accent);
    FillRect(hdc, &line, ab);
    DeleteObject(ab);
    EndPaint(hwnd, &ps);
}

//+-------------------------------------
//|Starts hover tracking so WM_MOUSELEAVE arrives.
//+---------------------------------------------------------------------------
//|OnButtonMouseMove
//+---------------------------------------------------------------------------
void OnButtonMouseMove(HWND hwnd) {
    if (g_btn.hover) return;
    g_btn.hover = true;
    TRACKMOUSEEVENT tme = {};
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
    InvalidateRect(hwnd, nullptr, FALSE);
}

//+-------------------------------------
//|Click completed inside the button: open the text dialog.
//+---------------------------------------------------------------------------
//|OnButtonMouseUp
//+---------------------------------------------------------------------------
void OnButtonMouseUp(HWND hwnd, LPARAM lParam) {
    bool wasPressed = g_btn.pressed;
    g_btn.pressed = false;
    if (GetCapture() == hwnd) ReleaseCapture();
    InvalidateRect(hwnd, nullptr, FALSE);
    POINT pt = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
    RECT rc;
    GetClientRect(hwnd, &rc);
    if (wasPressed && PtInRect(&rc, pt)) PostMessageW(g_msgWnd, WM_APP_OPEN, 1, 0);
}

//+-------------------------------------
//|Floating "T" button. Never takes focus from the Snipping Tool.
//+---------------------------------------------------------------------------
//|ButtonWndProc
//+---------------------------------------------------------------------------
LRESULT CALLBACK ButtonWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32649)));  // hand
            return TRUE;
        case WM_MOUSEMOVE:
            OnButtonMouseMove(hwnd);
            return 0;
        case WM_MOUSELEAVE:
            g_btn.hover = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_LBUTTONDOWN:
            g_btn.pressed = true;
            SetCapture(hwnd);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_LBUTTONUP:
            OnButtonMouseUp(hwnd, lParam);
            return 0;
        case WM_CAPTURECHANGED:
            if (g_btn.pressed) {
                g_btn.pressed = false;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            PaintFloatingButton(hwnd);
            return 0;
    }
    if (IsThemeChangeMessage(msg, lParam)) QueueThemeRefresh();
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//+-------------------------------------
//|Window the button should stick to, or nullptr to hide it.
//+---------------------------------------------------------------------------
//|ResolveButtonTarget
//+---------------------------------------------------------------------------
HWND ResolveButtonTarget() {
    if (!g_settings.showButton) return nullptr;
    HWND target = nullptr;
    if (g_mode == Mode::Dialog && IsOurProcessForeground()) target = g_dlg.target;
    else if (g_mode == Mode::Idle) target = FindEditorTarget();
    if (!target || !IsWindow(target) || IsIconic(target)) return nullptr;
    return target;
}

//+-------------------------------------
//|Creates the button window on first use.
//+---------------------------------------------------------------------------
//|EnsureButtonWindow
//+---------------------------------------------------------------------------
bool EnsureButtonWindow() {
    if (g_btn.hwnd) return true;
    g_btn.hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kButtonClass,
        Text().windowTitle, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, g_hInst,
        nullptr);
    if (!g_btn.hwnd) return false;
    ApplyWindowTheme(g_btn.hwnd, true);
    return true;
}

//+-------------------------------------
//|Recreates the button font when the DPI changes.
//+---------------------------------------------------------------------------
//|EnsureButtonFont
//+---------------------------------------------------------------------------
void EnsureButtonFont(UINT dpi) {
    if (dpi == g_btn.fontDpi) return;
    if (g_btn.font) DeleteObject(g_btn.font);
    g_btn.font = MakeFont(dpi, 12, FW_SEMIBOLD);
    g_btn.fontDpi = dpi;
    InvalidateRect(g_btn.hwnd, nullptr, FALSE);
}

//+-------------------------------------
//|Button rectangle from the target window, corner and offsets.
//+---------------------------------------------------------------------------
//|ComputeButtonRect
//+---------------------------------------------------------------------------
RECT ComputeButtonRect(HWND target, UINT dpi) {
    RECT r = GetFrameRect(target);
    int size = Scale(32, dpi);
    int ox = Scale(g_settings.offsetX, dpi), oy = Scale(g_settings.offsetY, dpi);
    bool left = g_settings.corner == Corner::TopLeft ||
                g_settings.corner == Corner::BottomLeft;
    bool top = g_settings.corner == Corner::TopLeft ||
               g_settings.corner == Corner::TopRight;
    int x = left ? r.left + ox : r.right - ox - size;
    int y = top ? r.top + oy : r.bottom - oy - size;
    return {x, y, x + size, y + size};
}

//+-------------------------------------
//|Shows, moves or hides the floating button to follow the editor.
//+---------------------------------------------------------------------------
//|UpdateButton
//+---------------------------------------------------------------------------
void UpdateButton() {
    HWND target = ResolveButtonTarget();
    if (!target) {
        HideButton();
        return;
    }
    if (!EnsureButtonWindow()) return;

    UINT dpi = WindowDpi(target);
    EnsureButtonFont(dpi);
    RECT nr = ComputeButtonRect(target, dpi);

    bool visible = IsWindowVisible(g_btn.hwnd) != FALSE;
    if (!visible || !EqualRect(&nr, &g_btn.lastRect) || g_btn.target != target) {
        SetWindowPos(g_btn.hwnd, HWND_TOPMOST, nr.left, nr.top,
                     nr.right - nr.left, nr.bottom - nr.top,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        g_btn.lastRect = nr;
    }
    g_btn.target = target;
}

//+-------------------------------------
//|WinEvent hook: keeps the button attached while the editor moves or appears.
//+---------------------------------------------------------------------------
//|LocationChangedProc
//+---------------------------------------------------------------------------
void CALLBACK LocationChangedProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject,
                                  LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) return;
    //+---------------------------------------------------------------------------
    //|The attached editor moved, or the foreground window may have just become
    //|a usable editor (shown or resized after a capture).
    if (hwnd == g_btn.target) {
        UpdateButton();
    } else if (!g_btn.target && hwnd == GetForegroundWindow()) {
        //+---------------------------------------------------------------------------
        //|Query the canvas again (an image may have been opened in place),
        //|unless a re-check cycle is already running.
        if (g_recheckLeft == 0) {
            g_editorChecked = nullptr;
            g_recheckHwnd = nullptr;
        }
        UpdateButton();
    }
}

//+-------------------------------------
//|Deletes the fonts owned by the dialog.
//+---------------------------------------------------------------------------
//|DeleteDialogFonts
//+---------------------------------------------------------------------------
void DeleteDialogFonts() {
    for (HFONT f : {g_dlg.font, g_dlg.fontSmall, g_dlg.fontTitle, g_dlg.fontIcon}) {
        if (f) DeleteObject(f);
    }
}

//+-------------------------------------
//|Closes the dialog and its preview (also ends pick mode).
//+---------------------------------------------------------------------------
//|CloseDialog
//+---------------------------------------------------------------------------
void CloseDialog() {
    if (g_msgWnd) KillTimer(g_msgWnd, kDialogTimer);
    if (g_mode == Mode::Picking) EndPick(false);
    if (g_preview.hwnd) DestroyWindow(g_preview.hwnd);
    g_preview = PreviewState();
    g_dlg.closing = true;
    if (g_dlg.hwnd) DestroyWindow(g_dlg.hwnd);
    DeleteDialogFonts();
    g_dlg = DialogState();
    if (g_mode == Mode::Dialog || g_mode == Mode::Picking) g_mode = Mode::Idle;
}

//+-------------------------------------
//|Sets the letter height and syncs the box and the preview.
//+---------------------------------------------------------------------------
//|SetSize
//+---------------------------------------------------------------------------
void SetSize(int size) {
    size = Clamp(size, kMinSize, kMaxSize);
    g_dlg.size = size;
    std::wstring s = std::to_wstring(size);
    if (GetText(g_dlg.sizeEdit) != s) SetWindowTextW(g_dlg.sizeEdit, s.c_str());
    UpdatePreview();
}

//+-------------------------------------
//|Changes the height by one step (bigger steps for big text or Shift).
//+---------------------------------------------------------------------------
//|StepSize
//+---------------------------------------------------------------------------
void StepSize(int dir) {
    int step = g_dlg.size >= 40 ? 4 : 2;
    if (KeyDown(VK_SHIFT)) step *= 4;
    SetSize(g_dlg.size + dir * step);
}

//+-------------------------------------
//|Sets the writing speed and syncs the box.
//+---------------------------------------------------------------------------
//|SetSpeed
//+---------------------------------------------------------------------------
void SetSpeed(int speed) {
    speed = Clamp(speed, kMinSpeed, kMaxSpeed);
    g_dlg.speed = speed;
    std::wstring s = std::to_wstring(speed);
    if (GetText(g_dlg.speedEdit) != s) SetWindowTextW(g_dlg.speedEdit, s.c_str());
}

//+-------------------------------------
//|Changes the writing speed by one.
//+---------------------------------------------------------------------------
//|StepSpeed
//+---------------------------------------------------------------------------
void StepSpeed(int dir) {
    SetSpeed(g_dlg.speed + dir);
}

//+-------------------------------------
//|Last speed used, unless the speed setting changed since then.
//+---------------------------------------------------------------------------
//|InitialSpeed
//+---------------------------------------------------------------------------
int InitialSpeed() {
    if (Wh_GetIntValue(L"lastSpeedBase", -1) != g_settings.speed)
        return g_settings.speed;
    return Clamp(Wh_GetIntValue(L"lastSpeed", g_settings.speed), kMinSpeed,
                 kMaxSpeed);
}

//+-------------------------------------
//|Last letter height used, or the default from the settings.
//+---------------------------------------------------------------------------
//|InitialSize
//+---------------------------------------------------------------------------
int InitialSize() {
    return Clamp(Wh_GetIntValue(L"lastSize", g_settings.defaultSize), kMinSize,
                 kMaxSize);
}

//+-------------------------------------
//|Remembers height, speed and origin for the next time.
//+---------------------------------------------------------------------------
//|SaveDialogValues
//+---------------------------------------------------------------------------
void SaveDialogValues() {
    Wh_SetIntValue(L"lastSize", g_dlg.size);
    Wh_SetIntValue(L"lastSpeed", g_dlg.speed);
    Wh_SetIntValue(L"lastSpeedBase", g_settings.speed);
    RECT r = GetFrameRect(g_dlg.target);
    g_lastTarget = g_dlg.target;
    g_lastRel = {g_dlg.origin.x - r.left, g_dlg.origin.y - r.top};
}

//+-------------------------------------
//|True if every point of the text lies inside the editor area.
//+---------------------------------------------------------------------------
//|TextFitsEditor
//+---------------------------------------------------------------------------
bool TextFitsEditor(const std::wstring& text) {
    RECT safe = EditorSafeRect(g_dlg.target);
    POINT o = g_dlg.origin;
    for (const Stroke& st : LayoutText(text, o.x, o.y, g_dlg.size)) {
        for (const PtD& p : st) {
            POINT pt = {(LONG)std::lround(p.x), (LONG)std::lround(p.y)};
            if (!PtInRect(&safe, pt)) return false;
        }
    }
    return true;
}

//+-------------------------------------
//|Shows the "outside the editor" warning in place of the header hint.
//+---------------------------------------------------------------------------
//|ShowOutsideWarning
//+---------------------------------------------------------------------------
void ShowOutsideWarning() {
    g_dlg.warning = true;
    SetWindowTextW(g_dlg.hint, Text().outsideWarning);
    InvalidateRect(g_dlg.hint, nullptr, TRUE);
    MessageBeep(MB_ICONWARNING);
}

//+-------------------------------------
//|"Write": closes the dialog and queues the drawing job.
//+---------------------------------------------------------------------------
//|ConfirmDialog
//+---------------------------------------------------------------------------
void ConfirmDialog() {
    std::wstring text = GetText(g_dlg.edit);
    if (Trim(text).empty()) {
        CloseDialog();
        return;
    }
    if (!TextFitsEditor(text)) {
        ShowOutsideWarning();
        return;
    }
    SaveDialogValues();
    g_job.target = g_dlg.target;
    g_job.text = text;
    g_job.origin = g_dlg.origin;
    g_job.size = g_dlg.size;
    g_job.speed = g_dlg.speed;

    CloseDialog();
    g_mode = Mode::Drawing;
    HideButton();
    PostMessageW(g_msgWnd, WM_APP_DRAW, 0, 0);
}

//+-------------------------------------
//|Draws the "x" close button.
//+---------------------------------------------------------------------------
//|DrawCloseButton
//+---------------------------------------------------------------------------
void DrawCloseButton(const DRAWITEMSTRUCT* d) {
    bool down = (d->itemState & ODS_SELECTED) != 0;
    COLORREF fill = down ? g_theme.surfacePressed : g_theme.surface;
    RECT rc = d->rcItem;
    FillRect(d->hDC, &rc, g_bgBrush);
    FillRoundRect(d->hDC, rc, Scale(6, g_dlg.dpi), fill, fill);
    DrawCenteredText(d->hDC, L"\uE8BB", rc, g_dlg.fontIcon, g_theme.textSecondary);
}

//+---------------------------------------------------------------------------
//|Fill, border and text colors of an action button.
struct ButtonColors {
    COLORREF fill;
    COLORREF border;
    COLORREF text;
};

//+-------------------------------------
//|Colors by role: accent for "Write", dark red for "Position".
//+---------------------------------------------------------------------------
//|GetButtonColors
//+---------------------------------------------------------------------------
ButtonColors GetButtonColors(UINT id, bool pressed) {
    if (id == IDOK) {
        COLORREF fill = pressed ? g_theme.accentPressed : g_theme.accent;
        return {fill, fill, g_theme.accentText};
    }
    if (id == kIdPick) {
        return {pressed ? kPickFillPressed : kPickFill, kPickBorder,
                RGB(255, 255, 255)};
    }
    return {pressed ? g_theme.surfacePressed : g_theme.surface, g_theme.border,
            g_theme.text};
}

//+-------------------------------------
//|Draws a rounded action button with its label.
//+---------------------------------------------------------------------------
//|DrawActionButton
//+---------------------------------------------------------------------------
void DrawActionButton(const DRAWITEMSTRUCT* d) {
    bool pressed = (d->itemState & ODS_SELECTED) != 0;
    bool focus = (d->itemState & ODS_FOCUS) && !(d->itemState & ODS_NOFOCUSRECT);
    ButtonColors c = GetButtonColors(d->CtlID, pressed);
    RECT rc = d->rcItem;
    FillRect(d->hDC, &rc, g_bgBrush);
    FillRoundRect(d->hDC, rc, Scale(8, g_dlg.dpi), c.fill,
                  focus ? g_theme.text : c.border);
    wchar_t label[64] = L"";
    GetWindowTextW(d->hwndItem, label, 64);
    DrawCenteredText(d->hDC, label, rc, g_dlg.font, c.text);
}

//+-------------------------------------
//|WM_DRAWITEM for the owner-drawn buttons.
//+---------------------------------------------------------------------------
//|OnDialogDrawItem
//+---------------------------------------------------------------------------
bool OnDialogDrawItem(const DRAWITEMSTRUCT* d) {
    if (d->CtlType != ODT_BUTTON) return false;
    if (d->CtlID == kIdClose) DrawCloseButton(d);
    else DrawActionButton(d);
    return true;
}

//+-------------------------------------
//|WM_PAINT: draws the rounded boxes behind the edit controls.
//+---------------------------------------------------------------------------
//|OnDialogPaint
//+---------------------------------------------------------------------------
void OnDialogPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    int radius = Scale(8, g_dlg.dpi);
    for (const RECT& fr : g_dlg.frames) {
        FillRoundRect(hdc, fr, radius, g_theme.surface, g_theme.border);
    }
    EndPaint(hwnd, &ps);
}

//+-------------------------------------
//|No system title bar: the header area drags the window.
//+---------------------------------------------------------------------------
//|OnDialogNcHitTest
//+---------------------------------------------------------------------------
LRESULT OnDialogNcHitTest(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    LRESULT hit = DefWindowProcW(hwnd, WM_NCHITTEST, wParam, lParam);
    if (hit != HTCLIENT) return hit;
    POINT pt = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
    ScreenToClient(hwnd, &pt);
    return pt.y < Scale(44, g_dlg.dpi) ? HTCAPTION : hit;
}

//+-------------------------------------
//|Edit/static colors from the current theme.
//+---------------------------------------------------------------------------
//|OnDialogCtlColor
//+---------------------------------------------------------------------------
LRESULT OnDialogCtlColor(UINT msg, HDC hdc, HWND control) {
    if (msg == WM_CTLCOLOREDIT) {
        SetTextColor(hdc, g_theme.text);
        SetBkColor(hdc, g_theme.surface);
        return (LRESULT)g_surfaceBrush;
    }
    int id = GetDlgCtrlID(control);
    bool secondary = id == kIdHint || id == kIdCaption;
    COLORREF color = secondary ? g_theme.textSecondary : g_theme.text;
    if (id == kIdHint && g_dlg.warning) color = g_theme.warning;
    SetTextColor(hdc, color);
    SetBkColor(hdc, g_theme.bg);
    return (LRESULT)g_bgBrush;
}

//+-------------------------------------
//|Height box: follows typing, normalizes the value on focus loss.
//+---------------------------------------------------------------------------
//|OnSizeEditCommand
//+---------------------------------------------------------------------------
void OnSizeEditCommand(int code) {
    if (code == EN_KILLFOCUS) {
        SetSize(g_dlg.size);
        return;
    }
    if (code != EN_CHANGE) return;
    int v = _wtoi(GetText(g_dlg.sizeEdit).c_str());
    if (v < kMinSize || v > kMaxSize) return;
    g_dlg.size = v;
    UpdatePreview();
}

//+-------------------------------------
//|Speed box: follows typing, normalizes the value on focus loss.
//+---------------------------------------------------------------------------
//|OnSpeedEditCommand
//+---------------------------------------------------------------------------
void OnSpeedEditCommand(int code) {
    if (code == EN_KILLFOCUS) {
        SetSpeed(g_dlg.speed);
        return;
    }
    if (code != EN_CHANGE) return;
    int v = _wtoi(GetText(g_dlg.speedEdit).c_str());
    if (v >= kMinSpeed && v <= kMaxSpeed) g_dlg.speed = v;
}

//+-------------------------------------
//|WM_COMMAND: buttons and edit notifications.
//+---------------------------------------------------------------------------
//|OnDialogCommand
//+---------------------------------------------------------------------------
void OnDialogCommand(int id, int code) {
    if (g_dlg.closing) return;
    switch (id) {
        case IDOK: ConfirmDialog(); break;
        case IDCANCEL:
        case kIdClose: CloseDialog(); break;
        case kIdPick: StartPick(); break;
        case kIdMinus: StepSize(-1); break;
        case kIdPlus: StepSize(+1); break;
        case kIdSpeedMinus: StepSpeed(-1); break;
        case kIdSpeedPlus: StepSpeed(+1); break;
        case kIdSize: OnSizeEditCommand(code); break;
        case kIdSpeed: OnSpeedEditCommand(code); break;
        case kIdEdit:
            if (code == EN_CHANGE) UpdatePreview();
            break;
    }
}

//+-------------------------------------
//|On activation, re-reads the pen (it may have changed meanwhile).
//+---------------------------------------------------------------------------
//|OnDialogActivate
//+---------------------------------------------------------------------------
void OnDialogActivate(WPARAM wParam) {
    if (LOWORD(wParam) == WA_INACTIVE || !g_settings.previewFromPen) return;
    if (g_dlg.closing || !g_preview.hwnd) return;
    ReadPenInfo();
    UpdatePreview();
}

//+-------------------------------------
//|Text dialog window procedure.
//+---------------------------------------------------------------------------
//|DialogWndProc
//+---------------------------------------------------------------------------
LRESULT CALLBACK DialogWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case DM_GETDEFID:
            return MAKELRESULT(IDOK, DC_HASDEFID);
        case WM_NCHITTEST:
            return OnDialogNcHitTest(hwnd, wParam, lParam);
        case WM_ERASEBKGND: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect((HDC)wParam, &rc, g_bgBrush);
            return 1;
        }
        case WM_PAINT:
            OnDialogPaint(hwnd);
            return 0;
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
            return OnDialogCtlColor(msg, (HDC)wParam, (HWND)lParam);
        case WM_DRAWITEM:
            if (OnDialogDrawItem((DRAWITEMSTRUCT*)lParam)) return TRUE;
            break;
        case WM_COMMAND:
            OnDialogCommand(LOWORD(wParam), HIWORD(wParam));
            return 0;
        case WM_CLOSE:
            CloseDialog();
            return 0;
        case WM_ACTIVATE:
            OnDialogActivate(wParam);
            break;
    }
    if (IsThemeChangeMessage(msg, lParam)) QueueThemeRefresh();
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//+-------------------------------------
//|Area the text is expected to occupy (grows with the text), used to keep
//|the dialog from covering it.
//+---------------------------------------------------------------------------
//|EstimatePreviewRect
//+---------------------------------------------------------------------------
RECT EstimatePreviewRect() {
    int pad = Scale(16, g_dlg.dpi);
    POINT o = g_dlg.origin;
    RECT r = {o.x - pad, o.y - pad, o.x + pad, o.y + pad};
    for (const Stroke& st : g_preview.strokes) {
        for (const PtD& p : st) {
            r.left = std::min(r.left, (LONG)p.x - pad);
            r.top = std::min(r.top, (LONG)p.y - pad);
            r.right = std::max(r.right, (LONG)p.x + pad);
            r.bottom = std::max(r.bottom, (LONG)p.y + pad);
        }
    }
    //+---------------------------------------------------------------------------
    //|Leave room for text that hasn't been typed yet.
    LONG minWidth = std::max(Scale(360, g_dlg.dpi), g_dlg.size * 12);
    r.right = std::max(r.right, o.x + minWidth);
    r.bottom = std::max(r.bottom, (LONG)(o.y + g_dlg.size * 3));
    return r;
}

//+-------------------------------------
//|Candidate dialog positions, in order of preference.
//+---------------------------------------------------------------------------
//|DialogCandidates
//+---------------------------------------------------------------------------
std::vector<POINT> DialogCandidates(bool fromButton, int dw, int dh,
                                    const RECT& pr) {
    auto S = [](int v) { return Scale(v, g_dlg.dpi); };
    std::vector<POINT> c;
    if (g_settings.dialogBottomCenter && IsWindow(g_dlg.target)) {
        RECT tr = GetFrameRect(g_dlg.target);
        int cx = (tr.left + tr.right) / 2 - dw / 2;
        c.push_back({cx, tr.bottom - dh - S(16)});
        c.push_back({cx, tr.top + S(56)});
    }
    if (fromButton && g_btn.hwnd && IsWindowVisible(g_btn.hwnd)) {
        RECT br;
        GetWindowRect(g_btn.hwnd, &br);
        c.push_back({br.right - dw, br.bottom + S(8)});
    }
    c.push_back({pr.right + S(8), g_dlg.origin.y - S(40)});
    c.push_back({pr.left - dw - S(8), g_dlg.origin.y - S(40)});
    c.push_back({g_dlg.origin.x - S(20), pr.top - dh - S(8)});
    c.push_back({g_dlg.origin.x - S(20), pr.bottom + S(8)});
    return c;
}

//+-------------------------------------
//|Keeps a w x h box at p inside the work area.
//+---------------------------------------------------------------------------
//|ClampToWorkArea
//+---------------------------------------------------------------------------
POINT ClampToWorkArea(POINT p, int w, int h, const RECT& wa) {
    if (p.x + w > wa.right) p.x = wa.right - w;
    if (p.x < wa.left) p.x = wa.left;
    if (p.y + h > wa.bottom) p.y = wa.bottom - h;
    if (p.y < wa.top) p.y = wa.top;
    return p;
}

//+-------------------------------------
//|Moves the dialog to the first candidate that doesn't cover the text.
//+---------------------------------------------------------------------------
//|PlaceDialog
//+---------------------------------------------------------------------------
void PlaceDialog(bool fromButton) {
    RECT cur;
    GetWindowRect(g_dlg.hwnd, &cur);
    int dw = cur.right - cur.left, dh = cur.bottom - cur.top;
    RECT wa = GetWorkArea(g_dlg.origin);
    RECT pr = EstimatePreviewRect();
    std::vector<POINT> candidates = DialogCandidates(fromButton, dw, dh, pr);

    POINT chosen = ClampToWorkArea(candidates.front(), dw, dh, wa);
    for (POINT c : candidates) {
        POINT p = ClampToWorkArea(c, dw, dh, wa);
        RECT dr = {p.x, p.y, p.x + dw, p.y + dh}, tmp;
        if (!IntersectRect(&tmp, &dr, &pr)) {
            chosen = p;
            break;
        }
    }
    SetWindowPos(g_dlg.hwnd, HWND_TOPMOST, chosen.x, chosen.y, 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE);
}

//+-------------------------------------
//|Re-places the dialog if it now covers the text.
//+---------------------------------------------------------------------------
//|PlaceDialogIfCovering
//+---------------------------------------------------------------------------
void PlaceDialogIfCovering() {
    RECT dr, pr = EstimatePreviewRect(), tmp;
    GetWindowRect(g_dlg.hwnd, &dr);
    if (IntersectRect(&tmp, &dr, &pr)) PlaceDialog(false);
}

//+-------------------------------------
//|Default text origin inside the editor (upper-left area).
//+---------------------------------------------------------------------------
//|DefaultOrigin
//+---------------------------------------------------------------------------
POINT DefaultOrigin(const RECT& wr) {
    return {wr.left + (wr.right - wr.left) / 5,
            wr.top + (wr.bottom - wr.top) * 3 / 10};
}

//+-------------------------------------
//|Text origin: mouse position (hotkey) or last/default position (button).
//+---------------------------------------------------------------------------
//|ResolveOrigin
//+---------------------------------------------------------------------------
POINT ResolveOrigin(HWND target, bool fromButton) {
    POINT origin;
    if (!fromButton) {
        GetCursorPos(&origin);
        return ClampToRect(origin, EditorSafeRect(target));
    }
    RECT wr = GetFrameRect(target);
    if (g_lastTarget != target) return DefaultOrigin(wr);
    origin = {wr.left + g_lastRel.x, wr.top + g_lastRel.y};
    return PtInRect(&wr, origin) ? origin : DefaultOrigin(wr);
}

//+-------------------------------------
//|Creates the dialog fonts for its DPI.
//+---------------------------------------------------------------------------
//|CreateDialogFonts
//+---------------------------------------------------------------------------
void CreateDialogFonts() {
    UINT dpi = g_dlg.dpi;
    g_dlg.font = MakeFont(dpi, 10, FW_NORMAL);
    g_dlg.fontSmall = MakeFont(dpi, 8, FW_NORMAL);
    g_dlg.fontTitle = MakeFont(dpi, 12, FW_NORMAL);
    g_dlg.fontIcon = MakeFont(dpi, 7, FW_NORMAL, L"Segoe MDL2 Assets");
}

//+-------------------------------------
//|Creates a child control at 96-DPI coordinates and sets its font.
//+---------------------------------------------------------------------------
//|AddControl
//+---------------------------------------------------------------------------
HWND AddControl(const wchar_t* cls, const wchar_t* text, DWORD style, int x,
                int y, int w, int h, int id, HFONT font) {
    UINT dpi = g_dlg.dpi;
    HWND c = CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style,
                             Scale(x, dpi), Scale(y, dpi), Scale(w, dpi),
                             Scale(h, dpi), g_dlg.hwnd, (HMENU)(INT_PTR)id,
                             g_hInst, nullptr);
    SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);
    return c;
}

//+-------------------------------------
//|Registers a rounded box (drawn in WM_PAINT) at 96-DPI coordinates.
//+---------------------------------------------------------------------------
//|AddFrame
//+---------------------------------------------------------------------------
void AddFrame(int x, int y, int w, int h) {
    UINT dpi = g_dlg.dpi;
    g_dlg.frames.push_back(
        {Scale(x, dpi), Scale(y, dpi), Scale(x + w, dpi), Scale(y + h, dpi)});
}

//+-------------------------------------
//|Title, line-break hint and close button.
//+---------------------------------------------------------------------------
//|CreateHeader
//+---------------------------------------------------------------------------
void CreateHeader() {
    AddControl(L"STATIC", Text().title, SS_NOPREFIX, 26, 15, 300, 26, kIdTitle,
               g_dlg.fontTitle);
    g_dlg.hint = AddControl(L"STATIC", Text().lineBreakHint,
                            SS_RIGHT | SS_NOPREFIX, 232, 23, 230, 16, kIdHint,
                            g_dlg.fontSmall);
}

//+-------------------------------------
//|Multi-line text box.
//+---------------------------------------------------------------------------
//|CreateTextBox
//+---------------------------------------------------------------------------
void CreateTextBox() {
    AddFrame(24, 47, 448, 123);
    g_dlg.edit = AddControl(L"EDIT", L"",
                            WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL, 33, 55,
                            430, 107, kIdEdit, g_dlg.font);
}

//+-------------------------------------
//|Label + [-] [value] [+] row. Returns the value box.
//+---------------------------------------------------------------------------
//|CreateSpinRow
//+---------------------------------------------------------------------------
HWND CreateSpinRow(const wchar_t* label, DWORD labelAlign, int labelX,
                   int labelW, int x, int value, int minusId, int editId,
                   int plusId) {
    AddControl(L"STATIC", label, labelAlign | SS_NOPREFIX, labelX, 179, labelW,
               20, kIdLabel, g_dlg.font);
    AddControl(L"BUTTON", L"−", WS_TABSTOP | BS_OWNERDRAW, x, 175, 28, 28,
               minusId, g_dlg.font);
    AddFrame(x + 32, 175, 49, 28);
    HWND edit = AddControl(L"EDIT", std::to_wstring(value).c_str(),
                           WS_TABSTOP | ES_NUMBER | ES_CENTER, x + 36, 180, 41,
                           19, editId, g_dlg.font);
    AddControl(L"BUTTON", L"+", WS_TABSTOP | BS_OWNERDRAW, x + 85, 175, 28, 28,
               plusId, g_dlg.font);
    return edit;
}

//+-------------------------------------
//|Height (left) and speed (right) controls.
//+---------------------------------------------------------------------------
//|CreateSpinRows
//+---------------------------------------------------------------------------
void CreateSpinRows() {
    g_dlg.sizeEdit = CreateSpinRow(Text().heightLabel, SS_LEFT, 28, 78, 107,
                                   g_dlg.size, kIdMinus, kIdSize, kIdPlus);
    g_dlg.speedEdit = CreateSpinRow(Text().speedLabel, SS_RIGHT, 240, 110, 357,
                                    g_dlg.speed, kIdSpeedMinus, kIdSpeed,
                                    kIdSpeedPlus);
    SendMessageW(g_dlg.speedEdit, EM_LIMITTEXT, 2, 0);
}

//+-------------------------------------
//|Position / Write / Cancel buttons with their shortcuts underneath.
//+---------------------------------------------------------------------------
//|CreateActionButtons
//+---------------------------------------------------------------------------
void CreateActionButtons() {
    struct Action {
        const wchar_t* label;
        const wchar_t* shortcut;
        int x, w, id;
    };
    const Action actions[] = {
        {Text().pick, Text().pickShortcut, 108, 116, kIdPick},
        {Text().write, Text().writeShortcut, 238, 113, IDOK},
        {Text().cancel, Text().cancelShortcut, 363, 107, IDCANCEL},
    };
    for (const Action& a : actions) {
        AddControl(L"BUTTON", a.label, WS_TABSTOP | BS_OWNERDRAW, a.x, 235, a.w,
                   38, a.id, g_dlg.font);
    }
    for (const Action& a : actions) {
        AddControl(L"STATIC", a.shortcut, SS_CENTER | SS_NOPREFIX, a.x, 276, a.w,
                   15, kIdCaption, g_dlg.fontSmall);
    }
    AddControl(L"BUTTON", L"", BS_OWNERDRAW, 464, 13, 22, 22, kIdClose,
               g_dlg.font);
}

//+-------------------------------------
//|Creates the dialog window and all its controls (hidden).
//+---------------------------------------------------------------------------
//|CreateDialogWindow
//+---------------------------------------------------------------------------
bool CreateDialogWindow(POINT origin) {
    const DWORD style = WS_POPUP;
    const DWORD exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT;
    const int W = 492, H = 296;

    //+---------------------------------------------------------------------------
    //|Created at the origin so it picks up that monitor's DPI.
    g_dlg.hwnd = CreateWindowExW(exStyle, kDialogClass, Text().windowTitle,
                                 style, origin.x, origin.y, 10, 10, nullptr,
                                 nullptr, g_hInst, nullptr);
    if (!g_dlg.hwnd) return false;
    ApplyWindowTheme(g_dlg.hwnd, false, true);
    g_dlg.dpi = WindowDpi(g_dlg.hwnd);

    CreateDialogFonts();
    CreateHeader();
    CreateTextBox();
    CreateSpinRows();
    CreateActionButtons();

    RECT rc = {0, 0, Scale(W, g_dlg.dpi), Scale(H, g_dlg.dpi)};
    AdjustWindowRectExForDpi(&rc, style, FALSE, exStyle, g_dlg.dpi);
    SetWindowPos(g_dlg.hwnd, HWND_TOPMOST, 0, 0, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_NOMOVE | SWP_NOACTIVATE);
    return true;
}

//+-------------------------------------
//|Opens the text dialog for an editor window (from the button or hotkey).
//+---------------------------------------------------------------------------
//|OpenDialog
//+---------------------------------------------------------------------------
void OpenDialog(HWND target, bool fromButton) {
    if (g_mode == Mode::Dialog && g_dlg.hwnd) {
        SetForegroundWindow(g_dlg.hwnd);
        return;
    }
    if (g_mode != Mode::Idle) return;
    LoadTheme();
    if (g_settings.previewFromPen) ReadPenInfo();

    if (!target || !IsWindow(target)) return;

    g_dlg = DialogState();
    g_dlg.target = target;
    g_dlg.origin = ResolveOrigin(target, fromButton);
    g_dlg.speed = InitialSpeed();
    g_dlg.size = InitialSize();

    CreatePreviewWindow(g_dlg.origin);
    if (!CreateDialogWindow(g_dlg.origin)) {
        CloseDialog();
        return;
    }
    g_mode = Mode::Dialog;
    SetTimer(g_msgWnd, kDialogTimer, kDialogTimerMs, nullptr);

    PlaceDialog(fromButton);
    ShowWindow(g_dlg.hwnd, SW_SHOW);
    SetForegroundWindow(g_dlg.hwnd);
    SetFocus(g_dlg.edit);
    UpdatePreview();
}

//+-------------------------------------
//|Moves the text origin to the mouse position (kept inside the editor).
//+---------------------------------------------------------------------------
//|PickOriginFromCursor
//+---------------------------------------------------------------------------
void PickOriginFromCursor() {
    POINT pt;
    GetCursorPos(&pt);
    g_dlg.origin = ClampToRect(pt, g_pick.safe);
    UpdatePreview();
}

//+-------------------------------------
//|Pick mode keys: Esc cancels, Enter accepts, arrows nudge (Shift = 10 px).
//+---------------------------------------------------------------------------
//|OnPickKeyDown
//+---------------------------------------------------------------------------
bool OnPickKeyDown(WPARAM key) {
    int step = KeyDown(VK_SHIFT) ? 10 : 1;
    switch (key) {
        case VK_ESCAPE: PostMessageW(g_msgWnd, WM_APP_ENDPICK, 0, 0); return true;
        case VK_RETURN: PostMessageW(g_msgWnd, WM_APP_ENDPICK, 1, 0); return true;
        case VK_LEFT: NudgeOrigin(-step, 0); return true;
        case VK_RIGHT: NudgeOrigin(step, 0); return true;
        case VK_UP: NudgeOrigin(0, -step); return true;
        case VK_DOWN: NudgeOrigin(0, step); return true;
    }
    return false;
}

//+-------------------------------------
//|Invisible overlay over the editor that catches the "Position" click.
//+---------------------------------------------------------------------------
//|PickWndProc
//+---------------------------------------------------------------------------
LRESULT CALLBACK PickWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                             LPARAM lParam) {
    switch (msg) {
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32515)));  // cross
            return TRUE;
        case WM_ERASEBKGND: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect((HDC)wParam, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }
        case WM_MOUSEMOVE:
            PickOriginFromCursor();
            return 0;
        case WM_LBUTTONDOWN:
            PickOriginFromCursor();
            SetCapture(hwnd);
            return 0;
        case WM_LBUTTONUP:
            //+---------------------------------------------------------------------------
            //|Act on button-up so the release doesn't reach the canvas.
            if (GetCapture() == hwnd) ReleaseCapture();
            PostMessageW(g_msgWnd, WM_APP_ENDPICK, 1, 0);
            return 0;
        case WM_RBUTTONUP:
            PostMessageW(g_msgWnd, WM_APP_ENDPICK, 0, 0);
            return 0;
        case WM_KEYDOWN:
            if (OnPickKeyDown(wParam)) return 0;
            break;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && !g_pick.ending)
                PostMessageW(g_msgWnd, WM_APP_ENDPICK, 0, 0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//+-------------------------------------
//|Banner with instructions shown during pick mode.
//+---------------------------------------------------------------------------
//|HintWndProc
//+---------------------------------------------------------------------------
LRESULT CALLBACK HintWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                             LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, g_surfaceBrush);
            DrawCenteredText(hdc, Text().pickBanner, rc, g_pick.hintFont,
                             g_theme.text);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//+-------------------------------------
//|Creates and activates the overlay over the editor window.
//+---------------------------------------------------------------------------
//|CreatePickOverlay
//+---------------------------------------------------------------------------
bool CreatePickOverlay(const RECT& r) {
    g_pick.overlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kPickClass, L"",
        WS_POPUP, r.left, r.top, r.right - r.left, r.bottom - r.top, nullptr,
        nullptr, g_hInst, nullptr);
    if (!g_pick.overlay) return false;
    SetLayeredWindowAttributes(g_pick.overlay, 0, 1, LWA_ALPHA);
    ShowWindow(g_pick.overlay, SW_SHOW);
    SetForegroundWindow(g_pick.overlay);
    SetFocus(g_pick.overlay);
    return true;
}

//+-------------------------------------
//|Shows the instructions banner near the top of the editor.
//+---------------------------------------------------------------------------
//|CreatePickHint
//+---------------------------------------------------------------------------
void CreatePickHint(const RECT& r) {
    UINT dpi = WindowDpi(g_pick.overlay);
    g_pick.hintFont = MakeFont(dpi, 9, FW_NORMAL);
    int hw = Scale(460, dpi), hh = Scale(32, dpi);
    int hx = r.left + ((r.right - r.left) - hw) / 2, hy = r.top + Scale(56, dpi);
    g_pick.hint = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kHintClass, L"", WS_POPUP, hx, hy, hw, hh, nullptr, nullptr, g_hInst,
        nullptr);
    if (!g_pick.hint) return;
    ApplyWindowTheme(g_pick.hint, true);
    ShowWindow(g_pick.hint, SW_SHOWNOACTIVATE);
}

//+-------------------------------------
//|"Position": hides the dialog; the preview follows the mouse until a click.
//+---------------------------------------------------------------------------
//|StartPick
//+---------------------------------------------------------------------------
void StartPick() {
    if (g_mode != Mode::Dialog || !IsWindow(g_dlg.target)) return;
    g_mode = Mode::Picking;
    g_pick = PickState();
    g_pick.savedOrigin = g_dlg.origin;
    g_pick.safe = EditorSafeRect(g_dlg.target);
    ShowWindow(g_dlg.hwnd, SW_HIDE);
    HideButton();

    RECT r = GetFrameRect(g_dlg.target);
    if (!CreatePickOverlay(r)) {
        EndPick(false);
        return;
    }
    CreatePickHint(r);

    POINT pt;
    GetCursorPos(&pt);
    if (PtInRect(&g_pick.safe, pt)) g_dlg.origin = pt;
    UpdatePreview();  // also raises the preview above the overlay
}

//+-------------------------------------
//|Destroys the overlay, the banner and its font.
//+---------------------------------------------------------------------------
//|DestroyPickWindows
//+---------------------------------------------------------------------------
void DestroyPickWindows() {
    if (g_pick.hint) DestroyWindow(g_pick.hint);
    if (g_pick.overlay) DestroyWindow(g_pick.overlay);
    if (g_pick.hintFont) DeleteObject(g_pick.hintFont);
    g_pick = PickState();
}

//+-------------------------------------
//|Leaves pick mode, keeping (accept) or restoring the previous origin.
//+---------------------------------------------------------------------------
//|EndPick
//+---------------------------------------------------------------------------
void EndPick(bool accept) {
    if (g_mode != Mode::Picking) return;
    g_pick.ending = true;
    if (!accept) g_dlg.origin = g_pick.savedOrigin;
    DestroyPickWindows();

    g_mode = Mode::Dialog;
    if (g_dlg.hwnd) {
        if (accept) {
            UpdatePreview();
            PlaceDialogIfCovering();
        }
        ShowWindow(g_dlg.hwnd, SW_SHOW);
        SetForegroundWindow(g_dlg.hwnd);
        SetFocus(g_dlg.edit);
    }
    UpdatePreview();
    UpdateButton();
}

//+-------------------------------------
//|Gives focus back to the editor. False if it doesn't take it or the mod
//|is unloading.
//+---------------------------------------------------------------------------
//|BringTargetToFront
//+---------------------------------------------------------------------------
bool BringTargetToFront(HWND target) {
    SetForegroundWindow(target);
    for (int i = 0;
         i < 50 && GetAncestor(GetForegroundWindow(), GA_ROOT) != target; i++) {
        if (!SleepUnlessStopping(20)) return false;
    }
    if (GetAncestor(GetForegroundWindow(), GA_ROOT) != target) {
        Wh_Log(L"Could not bring the Snipping Tool back to the foreground");
        return false;
    }
    return SleepUnlessStopping(150);
}

//+-------------------------------------
//|Waits (up to 1 s) until mouse buttons, Esc and Enter are released.
//+---------------------------------------------------------------------------
//|WaitForInputRelease
//+---------------------------------------------------------------------------
void WaitForInputRelease() {
    for (int i = 0; i < 100 && !Stopping(); i++) {
        if (!KeyDown(VK_LBUTTON) && !KeyDown(VK_RBUTTON) &&
            !KeyDown(VK_ESCAPE) && !KeyDown(VK_RETURN)) {
            return;
        }
        Sleep(10);
    }
}

//+-------------------------------------
//|Writes the queued text into the editor with the pen.
//+---------------------------------------------------------------------------
//|RunDrawJob
//+---------------------------------------------------------------------------
void RunDrawJob() {
    HWND target = g_job.target;
    if (target && IsWindow(target) && BringTargetToFront(target)) {
        WaitForInputRelease();
        const wchar_t* previousTool = EnsurePenSelected(target);
        std::vector<Stroke> strokes =
            LayoutText(g_job.text, g_job.origin.x, g_job.origin.y, g_job.size);
        Wh_Log(L"Writing %d chars (%d strokes), height %d px, speed %d",
               (int)g_job.text.size(), (int)strokes.size(), g_job.size,
               g_job.speed);
        DrawStrokes(target, strokes, g_job.speed);
        RestoreTool(target, previousTool);
    }
    g_job = DrawJob();
    g_mode = Mode::Idle;
    UpdateButton();
}

//+-------------------------------------
//|Logs which window is in the foreground (diagnostics).
//+---------------------------------------------------------------------------
//|LogForeground
//+---------------------------------------------------------------------------
void LogForeground(const wchar_t* prefix) {
    HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    wchar_t cls[128] = L"";
    if (fg) {
        GetWindowThreadProcessId(fg, &pid);
        GetClassNameW(fg, cls, 128);
    }
    Wh_Log(L"%s (foreground window: PID %u, class '%s')", prefix, pid,
           cls);
}

//+-------------------------------------
//|Registers the hotkey while the Snipping Tool is in front, and releases it
//|otherwise, so it never blocks the combination in other apps.
//+---------------------------------------------------------------------------
//|UpdateHotkeyRegistration
//+---------------------------------------------------------------------------
void UpdateHotkeyRegistration(bool foreground) {
    if (foreground == g_hotkeyRegistered) return;
    if (!foreground) {
        UnregisterHotKey(g_msgWnd, kHotkeyId);
        g_hotkeyRegistered = false;
        return;
    }
    g_hotkeyRegistered = RegisterHotKey(g_msgWnd, kHotkeyId,
                                        g_settings.mods | MOD_NOREPEAT,
                                        g_settings.vk) != FALSE;
    if (!g_hotkeyRegistered) {
        Wh_Log(L"RegisterHotKey failed (%u); the hotkey may be in use",
               GetLastError());
    }
}

//+-------------------------------------
//|Hides the dialog and preview while another app is in front (they are
//|topmost), and shows them again when the Snipping Tool comes back.
//+---------------------------------------------------------------------------
//|UpdateDialogVisibility
//+---------------------------------------------------------------------------
void UpdateDialogVisibility(bool foreground) {
    if (g_mode != Mode::Dialog || !g_dlg.hwnd) return;
    if (!foreground && !g_dlg.hiddenAway) {
        g_dlg.hiddenAway = true;
        ShowWindow(g_dlg.hwnd, SW_HIDE);
        if (g_preview.hwnd) ShowWindow(g_preview.hwnd, SW_HIDE);
    } else if (foreground && g_dlg.hiddenAway) {
        g_dlg.hiddenAway = false;
        ShowWindow(g_dlg.hwnd, SW_SHOWNOACTIVATE);
        UpdatePreview();
    }
}

//+-------------------------------------
//|Foreground or minimize state changed: log, hotkey, dialog and button.
//+---------------------------------------------------------------------------
//|OnForegroundChanged
//+---------------------------------------------------------------------------
void OnForegroundChanged() {
    ResetEditorCheck();
    bool fg = IsOurProcessForeground();
    if (fg != g_wasForeground) {
        LogForeground(fg ? L"Snipping Tool active"
                         : L"Snipping Tool left the foreground");
        g_wasForeground = fg;
    }
    UpdateHotkeyRegistration(fg);
    UpdateDialogVisibility(fg);
    UpdateButton();
}

//+-------------------------------------
//|WinEvent hook for EVENT_SYSTEM_FOREGROUND and minimize start/end.
//+---------------------------------------------------------------------------
//|ForegroundEventProc
//+---------------------------------------------------------------------------
void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD,
                                  DWORD) {
    OnForegroundChanged();
}

//+-------------------------------------
//|Waits for the hotkey keys to be released, so the modifiers don't leak
//|into the text box.
//+---------------------------------------------------------------------------
//|WaitForHotkeyRelease
//+---------------------------------------------------------------------------
void WaitForHotkeyRelease() {
    for (int i = 0; i < 100 && !Stopping(); i++) {
        if (!KeyDown(VK_CONTROL) && !KeyDown(VK_MENU) && !KeyDown(VK_SHIFT) &&
            !KeyDown((int)g_settings.vk)) {
            return;
        }
        Sleep(10);
    }
}

//+-------------------------------------
//|WM_HOTKEY: opens the dialog at the mouse, or focuses it if already open.
//+---------------------------------------------------------------------------
//|OnHotkey
//+---------------------------------------------------------------------------
void OnHotkey() {
    if (g_mode == Mode::Dialog && g_dlg.hwnd) {
        SetForegroundWindow(g_dlg.hwnd);
        return;
    }
    if (g_mode != Mode::Idle) return;
    g_editorChecked = nullptr;  // fresh check: an image may have just opened
    HWND target = FindEditorTarget();
    if (!target) return;  // start screen, no image to write on
    Wh_Log(L"Hotkey detected");
    WaitForHotkeyRelease();
    if (!Stopping()) OpenDialog(target, false);
}

//+-------------------------------------
//|Dialog timer: closes the dialog if the editor window went away.
//+---------------------------------------------------------------------------
//|OnDialogTimer
//+---------------------------------------------------------------------------
void OnDialogTimer() {
    bool dialogOpen = g_mode == Mode::Dialog || g_mode == Mode::Picking;
    if (dialogOpen && !IsWindow(g_dlg.target)) CloseDialog();
    if (!dialogOpen) KillTimer(g_msgWnd, kDialogTimer);
}

//+-------------------------------------
//|Re-check timer: queries the foreground editor again for its canvas.
//+---------------------------------------------------------------------------
//|OnEditorRecheckTimer
//+---------------------------------------------------------------------------
void OnEditorRecheckTimer() {
    bool lastTry = --g_recheckLeft <= 0;
    if (lastTry) {
        KillTimer(g_msgWnd, kEditorRecheckTimer);
        g_recheckLeft = 0;
    }
    g_editorChecked = nullptr;  // query again
    UpdateButton();
    if (lastTry && !g_editorHasImage && g_editorChecked) {
        Wh_Log(L"Editor window %p has no '%s' element (start screen, or the "
               L"Snipping Tool changed its UI)",
               g_editorChecked, kCanvasAutomationId);
    }
}

//+-------------------------------------
//|Applies changed mod settings (also re-registers the hotkey).
//+---------------------------------------------------------------------------
//|OnSettingsReload
//+---------------------------------------------------------------------------
void OnSettingsReload() {
    LoadSettings();
    UpdateHotkeyRegistration(false);
    OnForegroundChanged();
    g_btn.lastRect = {};
    UpdateButton();
    if (g_preview.hwnd) UpdatePreview();
    Wh_Log(L"Settings reloaded");
}

//+-------------------------------------
//|Hidden message window: timer, hotkey and commands for the worker thread.
//+---------------------------------------------------------------------------
//|MsgWndProc
//+---------------------------------------------------------------------------
LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kDialogTimer) OnDialogTimer();
            else if (wParam == kEditorRecheckTimer) OnEditorRecheckTimer();
            return 0;
        case WM_HOTKEY:
            if (wParam == kHotkeyId) OnHotkey();
            return 0;
        case WM_APP_OPEN:
            OpenDialog(g_btn.target, true);
            return 0;
        case WM_APP_THEME:
            g_themePending = false;
            RefreshThemedWindows();
            return 0;
        case WM_APP_DRAW:
            RunDrawJob();
            return 0;
        case WM_APP_ENDPICK:
            EndPick(wParam != 0);
            return 0;
        case WM_APP_RELOAD:
            OnSettingsReload();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//+---------------------------------------------------------------------------
//|Window classes of the mod.
struct ClassDef {
    const wchar_t* name;
    WNDPROC proc;
    LPCWSTR cursor;
    UINT style;
};
const ClassDef kClassDefs[] = {
    {kMsgClass, MsgWndProc, MAKEINTRESOURCEW(32512), 0},
    {kButtonClass, ButtonWndProc, MAKEINTRESOURCEW(32649), 0},
    {kDialogClass, DialogWndProc, MAKEINTRESOURCEW(32512), CS_DROPSHADOW},
    {kPreviewClass, PreviewWndProc, MAKEINTRESOURCEW(32512), 0},
    {kPickClass, PickWndProc, MAKEINTRESOURCEW(32515), 0},
    {kHintClass, HintWndProc, MAKEINTRESOURCEW(32512), 0},
};
constexpr int kClassCount = sizeof(kClassDefs) / sizeof(kClassDefs[0]);

//+-------------------------------------
//|Unregisters the first `count` window classes of the mod.
//+---------------------------------------------------------------------------
//|UnregisterClasses
//+---------------------------------------------------------------------------
void UnregisterClasses(int count = kClassCount) {
    for (int i = 0; i < count; i++) UnregisterClassW(kClassDefs[i].name, g_hInst);
}

//+-------------------------------------
//|Registers the window classes with this image's window procedures. Any
//|failure is an error (a class left over from another load must never be
//|reused); classes registered so far are removed again.
//+---------------------------------------------------------------------------
//|RegisterClasses
//+---------------------------------------------------------------------------
bool RegisterClasses() {
    for (int i = 0; i < kClassCount; i++) {
        const ClassDef& d = kClassDefs[i];
        UnregisterClassW(d.name, g_hInst);  // stale class of this module, if any
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = d.style;
        wc.lpfnWndProc = d.proc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = d.name;
        wc.hCursor = LoadCursorW(nullptr, d.cursor);
        if (!RegisterClassExW(&wc)) {
            Wh_Log(L"Failed to register window class %s: %u", d.name,
                   GetLastError());
            UnregisterClasses(i);
            return false;
        }
    }
    return true;
}

//+-------------------------------------
//|True for the dialog window or one of its controls.
//+---------------------------------------------------------------------------
//|IsDialogMessageTarget
//+---------------------------------------------------------------------------
bool IsDialogMessageTarget(HWND hwnd) {
    return g_dlg.hwnd && (hwnd == g_dlg.hwnd || IsChild(g_dlg.hwnd, hwnd));
}

//+-------------------------------------
//|Alt+arrows: moves the text by the configured step (10x with Shift).
//+---------------------------------------------------------------------------
//|HandleAltArrow
//+---------------------------------------------------------------------------
bool HandleAltArrow(const MSG& msg) {
    if (msg.message != WM_SYSKEYDOWN || !IsArrowKey(msg.wParam)) return false;
    int step = g_settings.nudgeStep * (KeyDown(VK_SHIFT) ? 10 : 1);
    int dx = msg.wParam == VK_LEFT ? -step : msg.wParam == VK_RIGHT ? step : 0;
    int dy = msg.wParam == VK_UP ? -step : msg.wParam == VK_DOWN ? step : 0;
    NudgeOrigin(dx, dy);
    return true;
}

//+-------------------------------------
//|Screen area covered by a [-] [value] [+] group.
//+---------------------------------------------------------------------------
//|GetSpinArea
//+---------------------------------------------------------------------------
bool GetSpinArea(int minusId, int plusId, RECT* area) {
    RECT rMinus, rPlus;
    HWND minus = GetDlgItem(g_dlg.hwnd, minusId);
    HWND plus = GetDlgItem(g_dlg.hwnd, plusId);
    if (!minus || !plus || !GetWindowRect(minus, &rMinus) ||
        !GetWindowRect(plus, &rPlus)) {
        return false;
    }
    *area = {rMinus.left, std::min(rMinus.top, rPlus.top), rPlus.right,
             std::max(rMinus.bottom, rPlus.bottom)};
    return true;
}

//+-------------------------------------
//|True if the screen point is over the given spin group.
//+---------------------------------------------------------------------------
//|IsOverSpin
//+---------------------------------------------------------------------------
bool IsOverSpin(POINT pt, int minusId, int plusId) {
    RECT area;
    return GetSpinArea(minusId, plusId, &area) && PtInRect(&area, pt);
}

//+-------------------------------------
//|Mouse wheel over the height or speed group changes the value.
//+---------------------------------------------------------------------------
//|HandleMouseWheel
//+---------------------------------------------------------------------------
bool HandleMouseWheel(const MSG& msg) {
    if (msg.message != WM_MOUSEWHEEL) return false;
    POINT pt = {(short)LOWORD(msg.lParam), (short)HIWORD(msg.lParam)};
    bool overSize = IsOverSpin(pt, kIdMinus, kIdPlus);
    bool overSpeed = !overSize && IsOverSpin(pt, kIdSpeedMinus, kIdSpeedPlus);
    if (!overSize && !overSpeed) return false;

    static int wheelAcc = 0;
    wheelAcc += GET_WHEEL_DELTA_WPARAM(msg.wParam);
    while (wheelAcc >= WHEEL_DELTA || wheelAcc <= -WHEEL_DELTA) {
        int dir = wheelAcc > 0 ? +1 : -1;
        overSize ? StepSize(dir) : StepSpeed(dir);
        wheelAcc -= dir * WHEEL_DELTA;
    }
    return true;
}

//+-------------------------------------
//|Ctrl+Enter writes; Enter/Shift+Enter add a line in the text box;
//|Enter on a button clicks it.
//+---------------------------------------------------------------------------
//|HandleEnterKey
//+---------------------------------------------------------------------------
bool HandleEnterKey(const MSG& msg) {
    if (msg.message != WM_KEYDOWN || msg.wParam != VK_RETURN) return false;
    if (KeyDown(VK_CONTROL)) {
        ConfirmDialog();
    } else if (msg.hwnd == g_dlg.edit) {
        SendMessageW(g_dlg.edit, EM_REPLACESEL, TRUE, (LPARAM)L"\r\n");
    } else {
        wchar_t cls[16] = L"";
        GetClassNameW(msg.hwnd, cls, 16);
        if (lstrcmpiW(cls, L"Button") == 0) SendMessageW(msg.hwnd, BM_CLICK, 0, 0);
    }
    return true;
}

//+-------------------------------------
//|Up/Down inside the height or speed box changes the value.
//+---------------------------------------------------------------------------
//|HandleUpDownKeys
//+---------------------------------------------------------------------------
bool HandleUpDownKeys(const MSG& msg) {
    if (msg.message != WM_KEYDOWN) return false;
    if (msg.wParam != VK_UP && msg.wParam != VK_DOWN) return false;
    int dir = msg.wParam == VK_UP ? 1 : -1;
    if (msg.hwnd == g_dlg.sizeEdit) {
        StepSize(dir);
        return true;
    }
    if (msg.hwnd == g_dlg.speedEdit) {
        StepSpeed(dir);
        return true;
    }
    return false;
}

//+-------------------------------------
//|Keyboard/mouse handling for the dialog. True if the message was consumed.
//+---------------------------------------------------------------------------
//|PreTranslateDialogMessage
//+---------------------------------------------------------------------------
bool PreTranslateDialogMessage(MSG& msg) {
    if (g_mode != Mode::Dialog || !IsDialogMessageTarget(msg.hwnd)) return false;
    if (HandleAltArrow(msg)) return true;
    if (HandleMouseWheel(msg)) return true;
    if (HandleEnterKey(msg)) return true;
    if (HandleUpDownKeys(msg)) return true;
    return IsDialogMessageW(g_dlg.hwnd, &msg) != FALSE;
}

//+-------------------------------------
//|WinEvent hooks: editor location (this process), foreground and minimize
//|(any process, since the process gaining the foreground raises them).
//+---------------------------------------------------------------------------
//|InstallEventHooks
//+---------------------------------------------------------------------------
void InstallEventHooks() {
    g_locationHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        LocationChangedProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_minimizeHook = SetWinEventHook(
        EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr,
        ForegroundEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
}

//+-------------------------------------
//|Removes the WinEvent hooks.
//+---------------------------------------------------------------------------
//|RemoveEventHooks
//+---------------------------------------------------------------------------
void RemoveEventHooks() {
    for (HWINEVENTHOOK* h : {&g_locationHook, &g_foregroundHook, &g_minimizeHook}) {
        if (*h) UnhookWinEvent(*h);
        *h = nullptr;
    }
}

//+-------------------------------------
//|Settings, theme, classes, message window and event hooks.
//+---------------------------------------------------------------------------
//|InitWorker
//+---------------------------------------------------------------------------
bool InitWorker() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    LoadSettings();
    LoadTheme();
    Wh_Log(L"Mod active in PID %u", GetCurrentProcessId());

    if (!RegisterClasses()) return false;
    g_msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, g_hInst, nullptr);
    if (!g_msgWnd) {
        Wh_Log(L"Failed to create the message window: %u", GetLastError());
        UnregisterClasses();
        return false;
    }
    InstallEventHooks();
    OnForegroundChanged();
    return true;
}

//+-------------------------------------
//|Message loop of the worker thread. Ends on WM_QUIT or when the stop event
//|is set (even if WM_QUIT could not be posted).
//+---------------------------------------------------------------------------
//|RunMessageLoop
//+---------------------------------------------------------------------------
void RunMessageLoop() {
    MSG msg;
    while (!Stopping()) {
        DWORD r = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE,
                                              QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (r == WAIT_OBJECT_0 || r == WAIT_FAILED) return;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return;
            if (PreTranslateDialogMessage(msg)) continue;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}

//+-------------------------------------
//|Destroys all windows, hooks and GDI objects of the mod.
//+---------------------------------------------------------------------------
//|CleanupWorker
//+---------------------------------------------------------------------------
void CleanupWorker() {
    RemoveEventHooks();
    UpdateHotkeyRegistration(false);
    CloseDialog();
    if (g_btn.hwnd) DestroyWindow(g_btn.hwnd);
    if (g_btn.font) DeleteObject(g_btn.font);
    g_btn = ButtonState();
    DestroyWindow(g_msgWnd);
    g_msgWnd = nullptr;
    UnregisterClasses();
    if (g_bgBrush) DeleteObject(g_bgBrush);
    if (g_surfaceBrush) DeleteObject(g_surfaceBrush);
    g_bgBrush = g_surfaceBrush = nullptr;
}

//+-------------------------------------
//|Initializes WinRT/COM (multithreaded apartment) for this thread, used to
//|read the pen settings and for UI Automation. Must be paired with
//|LeaveWinRt on the same thread.
//+---------------------------------------------------------------------------
//|EnterWinRt
//+---------------------------------------------------------------------------
bool EnterWinRt() {
    if (!GetComBaseApi(&g_comApi)) return false;
    g_comReady = SUCCEEDED(g_comApi.RoInitialize(1 /* RO_INIT_MULTITHREADED */));
    if (g_comReady) CreateUiAutomation();
    return g_comReady;
}

//+-------------------------------------
//|Leaves the WinRT apartment entered by EnterWinRt.
//+---------------------------------------------------------------------------
//|LeaveWinRt
//+---------------------------------------------------------------------------
void LeaveWinRt() {
    ReleaseUiAutomation();
    if (g_comReady) g_comApi.RoUninitialize();
    g_comReady = false;
}

//+-------------------------------------
//|Worker thread: owns every window of the mod.
//+---------------------------------------------------------------------------
//|WorkerThread
//+---------------------------------------------------------------------------
DWORD WINAPI WorkerThread(LPVOID) {
    EnterWinRt();
    if (InitWorker()) {
        RunMessageLoop();
        CleanupWorker();
    }
    LeaveWinRt();
    return 0;
}

}  // namespace

//+-------------------------------------
//|Called by Windhawk when the mod is loaded: starts the worker thread.
//+---------------------------------------------------------------------------
//|Wh_ModInit
//+---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&Wh_ModInit, (HMODULE*)&g_hInst);
    g_stop = false;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

//+-------------------------------------
//|Called by Windhawk when the mod is unloaded: stops the worker thread and
//|waits until it has exited.
//+---------------------------------------------------------------------------
//|Wh_ModUninit
//+---------------------------------------------------------------------------
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_stop = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_thread) {
        //+---------------------------------------------------------------------------
        //|Windhawk unloads the image right after this returns, so the worker
        //|must be gone: wait without a timeout (every wait it does is short
        //|or ends on g_stop).
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
}

//+-------------------------------------
//|Called by Windhawk when the settings change.
//+---------------------------------------------------------------------------
//|Wh_ModSettingsChanged
//+---------------------------------------------------------------------------
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    if (g_msgWnd) PostMessageW(g_msgWnd, WM_APP_RELOAD, 0, 0);
}

//+-------------------------------------
//|Generated from the Hershey 'futural' single-stroke font (public domain).
//|Units: half Hershey units. Cap line=-24, baseline=18. Layout per glyph:
//|nStrokes, then for each stroke: nPoints, x0,y0, x1,y1, ...
//+---------------------------------------------------------------------------
//|Font data 
//+---------------------------------------------------------------------------
const short kGlyphData[] = {
    0,2,2,10,-24,10,4,5,10,14,8,16,10,18,12,16,10,14,2,2,8,-24,8,-10,2,24,-24,24,-10,4,2,22,-32,8,32,2,34,
    -32,20,32,2,8,-6,36,-6,2,6,6,34,6,3,2,16,-32,16,26,2,24,-32,24,26,20,34,-18,30,-22,24,-24,16,-24,10,-22,
    6,-18,6,-14,8,-10,10,-8,14,-6,26,-2,30,0,32,2,34,6,34,12,30,16,24,18,16,18,10,16,6,12,3,2,42,-24,6,18,
    16,16,-24,20,-20,20,-16,18,-12,14,-10,10,-10,6,-14,6,-18,8,-22,12,-24,16,-24,20,-22,26,-20,32,-20,38,
    -22,42,-24,11,34,4,30,6,28,10,28,14,32,18,36,18,40,16,42,12,42,8,38,4,34,4,1,34,46,-6,46,-8,44,-10,42,
    -10,40,-8,38,-4,34,6,30,12,26,16,22,18,14,18,10,16,8,14,6,10,6,6,8,2,10,0,24,-8,26,-10,28,-14,28,-18,
    26,-22,22,-24,18,-22,16,-18,16,-14,18,-8,22,-2,32,12,36,16,40,18,44,18,46,16,46,14,1,7,10,-20,8,-22,10,
    -24,12,-22,12,-18,10,-14,8,-12,1,10,22,-32,18,-28,14,-22,10,-14,8,-4,8,4,10,14,14,22,18,28,22,32,1,10,
    6,-32,10,-28,14,-22,18,-14,20,-4,20,4,18,14,14,22,10,28,6,32,3,2,16,-12,16,12,2,6,-6,26,6,2,26,-6,6,6,
    2,2,26,-18,26,18,2,8,0,44,0,1,7,10,10,8,12,6,10,8,8,10,10,10,14,6,18,1,2,8,0,44,0,1,5,8,8,6,10,8,12,10,
    10,8,8,1,2,40,-32,4,32,1,17,18,-24,12,-22,8,-16,6,-6,6,0,8,10,12,16,18,18,22,18,28,16,32,10,34,0,34,-6,
    32,-16,28,-22,22,-24,18,-24,1,4,12,-16,16,-18,22,-24,22,18,1,14,8,-14,8,-16,10,-20,12,-22,16,-24,24,-24,
    28,-22,30,-20,32,-16,32,-12,30,-8,26,-2,6,18,34,18,1,15,10,-24,32,-24,20,-8,26,-8,30,-6,32,-4,34,2,34,
    6,32,12,28,16,22,18,16,18,10,16,8,14,6,10,2,3,26,-24,6,4,36,4,2,26,-24,26,18,1,17,30,-24,10,-24,8,-6,
    10,-8,16,-10,22,-10,28,-8,32,-4,34,2,34,6,32,12,28,16,22,18,16,18,10,16,8,14,6,10,1,23,32,-18,30,-22,
    24,-24,20,-24,14,-22,10,-16,8,-6,8,4,10,12,14,16,20,18,22,18,28,16,32,12,34,6,34,4,32,-2,28,-6,22,-8,
    20,-8,14,-6,10,-2,8,4,2,2,34,-24,14,18,2,6,-24,34,-24,1,29,16,-24,10,-22,8,-18,8,-14,10,-10,14,-8,22,
    -6,28,-4,32,0,34,4,34,10,32,14,30,16,24,18,16,18,10,16,8,14,6,10,6,4,8,0,12,-4,18,-6,26,-8,30,-10,32,
    -14,32,-18,30,-22,24,-24,16,-24,1,23,32,-10,30,-4,26,0,20,2,18,2,12,0,8,-4,6,-10,6,-12,8,-18,12,-22,18,
    -24,20,-24,26,-22,30,-18,32,-10,32,0,30,10,26,16,20,18,16,18,10,16,8,12,2,5,8,-6,6,-4,8,-2,10,-4,8,-6,
    5,8,8,6,10,8,12,10,10,8,8,2,5,8,-6,6,-4,8,-2,10,-4,8,-6,7,10,10,8,12,6,10,8,8,10,10,10,14,6,18,1,3,40,
    -18,8,0,40,18,2,2,8,-6,44,-6,2,8,6,44,6,1,3,8,-18,40,0,8,18,2,14,6,-14,6,-16,8,-20,10,-22,14,-24,22,-24,
    26,-22,28,-20,30,-16,30,-12,28,-8,26,-6,18,-2,18,4,5,18,14,16,16,18,18,20,16,18,14,4,13,36,-8,34,-12,
    30,-14,24,-14,20,-12,18,-10,16,-4,16,2,18,6,22,8,28,8,32,6,34,2,6,24,-14,20,-10,18,-4,18,2,20,6,22,8,
    29,36,-14,34,2,34,6,38,8,42,8,46,4,48,-2,48,-6,46,-12,44,-16,40,-20,36,-22,30,-24,24,-24,18,-22,14,-20,
    10,-16,8,-12,6,-6,6,0,8,6,10,10,14,14,18,16,24,18,30,18,36,16,40,14,42,12,4,38,-14,36,2,36,6,38,8,3,2,
    18,-24,2,18,2,18,-24,34,18,2,8,4,28,4,3,2,8,-24,8,18,9,8,-24,26,-24,32,-22,34,-20,36,-16,36,-12,34,-8,
    32,-6,26,-4,10,8,-4,26,-4,32,-2,34,0,36,4,36,10,34,14,32,16,26,18,8,18,1,18,36,-14,34,-18,30,-22,26,-24,
    18,-24,14,-22,10,-18,8,-14,6,-8,6,2,8,8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,2,2,8,-24,8,18,12,8,
    -24,22,-24,28,-22,32,-18,34,-14,36,-8,36,2,34,8,32,12,28,16,22,18,8,18,4,2,8,-24,8,18,2,8,-24,34,-24,
    2,8,-4,24,-4,2,8,18,34,18,3,2,8,-24,8,18,2,8,-24,34,-24,2,8,-4,24,-4,2,19,36,-14,34,-18,30,-22,26,-24,
    18,-24,14,-22,10,-18,8,-14,6,-8,6,2,8,8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,36,2,2,26,2,36,2,3,2,
    8,-24,8,18,2,36,-24,36,18,2,8,-4,36,-4,1,2,8,-24,8,18,1,10,24,-24,24,8,22,14,20,16,16,18,12,18,8,16,6,
    14,4,8,4,4,3,2,8,-24,8,18,2,36,-24,8,4,2,18,-6,36,18,2,2,8,-24,8,18,2,8,18,32,18,4,2,8,-24,8,18,2,8,-24,
    24,18,2,40,-24,24,18,2,40,-24,40,18,3,2,8,-24,8,18,2,8,-24,36,18,2,36,-24,36,18,1,21,18,-24,14,-22,10,
    -18,8,-14,6,-8,6,2,8,8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,38,2,38,-8,36,-14,34,-18,30,-22,26,-24,
    18,-24,2,2,8,-24,8,18,10,8,-24,26,-24,32,-22,34,-20,36,-16,36,-10,34,-6,32,-4,26,-2,8,-2,2,21,18,-24,
    14,-22,10,-18,8,-14,6,-8,6,2,8,8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,38,2,38,-8,36,-14,34,-18,30,
    -22,26,-24,18,-24,2,24,10,36,22,3,2,8,-24,8,18,10,8,-24,26,-24,32,-22,34,-20,36,-16,36,-12,34,-8,32,-6,
    26,-4,8,-4,2,22,-4,36,18,1,20,34,-18,30,-22,24,-24,16,-24,10,-22,6,-18,6,-14,8,-10,10,-8,14,-6,26,-2,
    30,0,32,2,34,6,34,12,30,16,24,18,16,18,10,16,6,12,2,2,16,-24,16,18,2,2,-24,30,-24,1,10,8,-24,8,6,10,12,
    14,16,20,18,24,18,30,16,34,12,36,6,36,-24,2,2,2,-24,18,18,2,34,-24,18,18,4,2,4,-24,14,18,2,24,-24,14,
    18,2,24,-24,34,18,2,44,-24,34,18,2,2,6,-24,34,18,2,34,-24,6,18,2,3,2,-24,18,-4,18,18,2,34,-24,18,-4,3,
    2,34,-24,6,18,2,6,-24,34,-24,2,6,18,34,18,4,2,8,-32,8,32,2,10,-32,10,32,2,8,-32,22,-32,2,8,32,22,32,1,
    2,0,-24,28,24,4,2,18,-32,18,32,2,20,-32,20,32,2,6,-32,20,-32,2,6,32,20,32,2,2,16,-28,0,0,2,16,-28,32,
    0,1,2,0,32,36,32,1,7,10,-14,6,-10,6,-6,8,-4,10,-6,8,-8,6,-6,2,2,30,-10,30,18,14,30,-4,26,-8,22,-10,16,
    -10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,2,2,8,-24,8,18,14,8,-4,12,-8,16,-10,22,-10,
    26,-8,30,-4,32,2,32,6,30,12,26,16,22,18,16,18,12,16,8,12,1,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,
    2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,2,2,30,-24,30,18,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,
    6,6,8,12,12,16,16,18,22,18,26,16,30,12,1,17,6,2,30,2,30,-2,28,-6,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,
    6,8,12,12,16,16,18,22,18,26,16,30,12,2,5,20,-24,16,-24,12,-22,10,-16,10,18,2,4,-10,18,-10,2,7,30,-10,
    30,22,28,28,26,30,22,32,16,32,12,30,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,
    22,18,26,16,30,12,2,2,8,-24,8,18,7,8,-2,14,-8,18,-10,24,-10,28,-8,30,-2,30,18,2,5,6,-24,8,-22,10,-24,
    8,-26,6,-24,2,8,-10,8,18,2,5,10,-24,12,-22,14,-24,12,-26,10,-24,5,12,-10,12,24,10,30,6,32,2,32,3,2,8,
    -24,8,18,2,28,-10,8,10,2,16,2,30,18,1,2,8,-24,8,18,3,2,8,-10,8,18,7,8,-2,14,-8,18,-10,24,-10,28,-8,30,
    -2,30,18,7,30,-2,36,-8,40,-10,46,-10,50,-8,52,-2,52,18,2,2,8,-10,8,18,7,8,-2,14,-8,18,-10,24,-10,28,-8,
    30,-2,30,18,1,17,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,32,6,32,2,30,-4,26,-8,22,
    -10,16,-10,2,2,8,-10,8,32,14,8,-4,12,-8,16,-10,22,-10,26,-8,30,-4,32,2,32,6,30,12,26,16,22,18,16,18,12,
    16,8,12,2,2,30,-10,30,32,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,
    30,12,2,2,8,-10,8,18,5,8,2,10,-4,14,-8,18,-10,24,-10,1,17,28,-4,26,-8,20,-10,14,-10,8,-8,6,-4,8,0,12,
    2,22,4,26,6,28,10,28,12,26,16,20,18,14,18,8,16,6,12,2,5,10,-24,10,10,12,16,16,18,20,18,2,4,-10,18,-10,
    2,7,8,-10,8,10,10,16,14,18,20,18,24,16,30,10,2,30,-10,30,18,2,2,4,-10,16,18,2,28,-10,16,18,4,2,6,-10,
    14,18,2,22,-10,14,18,2,22,-10,30,18,2,38,-10,30,18,2,2,6,-10,28,18,2,28,-10,6,18,2,2,4,-10,16,18,6,28,
    -10,16,18,12,26,8,30,4,32,2,32,3,2,28,-10,6,18,2,6,-10,28,-10,2,6,18,28,18,3,10,18,-32,14,-30,12,-28,
    10,-24,10,-20,12,-16,14,-14,16,-10,16,-6,12,-2,17,14,-30,12,-26,12,-22,14,-18,16,-16,18,-12,18,-8,16,
    -4,8,0,16,4,18,8,18,12,16,16,14,18,12,22,12,26,14,30,10,12,2,16,6,16,10,14,14,12,16,10,20,10,24,12,28,
    14,30,18,32,1,2,8,-32,8,32,3,10,10,-32,14,-30,16,-28,18,-24,18,-20,16,-16,14,-14,12,-10,12,-6,16,-2,17,
    14,-30,16,-26,16,-22,14,-18,12,-16,10,-12,10,-8,12,-4,20,0,12,4,10,8,10,12,12,16,14,18,16,22,16,26,14,
    30,10,16,2,12,6,12,10,14,14,16,16,18,20,18,24,16,28,14,30,10,32,2,11,6,6,6,2,8,-4,12,-6,16,-6,20,-4,28,
    2,32,4,36,4,40,2,42,-2,11,6,2,8,-2,12,-4,16,-4,20,-2,28,4,32,6,36,6,40,4,42,-2,42,-6,3,2,18,-24,18,-10,
    14,18,-21,16,-23,14,-24,10,-24,8,-23,6,-21,6,-18,6,-16,6,-13,8,-11,10,-10,14,-10,16,-11,18,-13,2,4,-6,
    20,-6,1,9,10,-24,6,-22,4,-18,6,-14,10,-12,14,-14,16,-18,14,-22,10,-24,2,17,10,-24,8,-23,6,-21,6,-18,6,
    -16,6,-13,8,-11,10,-10,14,-10,16,-11,18,-13,18,-16,18,-18,18,-21,16,-23,14,-24,10,-24,2,4,-6,20,-6,4,
    2,18,-24,2,18,2,18,-24,34,18,2,8,4,28,4,2,14,-35,21,-27,4,2,18,-24,2,18,2,18,-24,34,18,2,8,4,28,4,2,15,
    -27,22,-35,4,2,18,-24,2,18,2,18,-24,34,18,2,8,4,28,4,3,11,-27,18,-34,25,-27,4,2,18,-24,2,18,2,18,-24,
    34,18,2,8,4,28,4,7,10,-29,12,-32,15,-33,18,-31,21,-29,24,-29,26,-32,5,2,18,-24,2,18,2,18,-24,34,18,2,
    8,4,28,4,2,13,-32,13,-30,2,23,-32,23,-30,4,2,18,-24,2,18,2,18,-24,34,18,2,8,4,28,4,7,18,-35,15,-33,15,
    -30,18,-28,21,-30,21,-33,18,-35,2,18,36,-14,34,-18,30,-22,26,-24,18,-24,14,-22,10,-18,8,-14,6,-8,6,2,
    8,8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,6,23,18,23,22,27,24,27,27,23,29,19,28,5,2,8,-24,8,18,2,8,
    -24,34,-24,2,8,-4,24,-4,2,8,18,34,18,2,15,-35,22,-27,5,2,8,-24,8,18,2,8,-24,34,-24,2,8,-4,24,-4,2,8,18,
    34,18,2,16,-27,23,-35,5,2,8,-24,8,18,2,8,-24,34,-24,2,8,-4,24,-4,2,8,18,34,18,3,12,-27,19,-34,26,-27,
    6,2,8,-24,8,18,2,8,-24,34,-24,2,8,-4,24,-4,2,8,18,34,18,2,14,-32,14,-30,2,24,-32,24,-30,2,2,8,-24,8,18,
    2,4,-35,11,-27,2,2,8,-24,8,18,2,5,-27,12,-35,2,2,8,-24,8,18,3,1,-27,8,-34,15,-27,3,2,8,-24,8,18,2,3,-32,
    3,-30,2,13,-32,13,-30,4,2,8,-24,8,18,2,8,-24,36,18,2,36,-24,36,18,7,14,-29,16,-32,19,-33,22,-31,25,-29,
    28,-29,30,-32,2,21,18,-24,14,-22,10,-18,8,-14,6,-8,6,2,8,8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,38,
    2,38,-8,36,-14,34,-18,30,-22,26,-24,18,-24,2,18,-35,25,-27,2,21,18,-24,14,-22,10,-18,8,-14,6,-8,6,2,8,
    8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,38,2,38,-8,36,-14,34,-18,30,-22,26,-24,18,-24,2,19,-27,26,
    -35,2,21,18,-24,14,-22,10,-18,8,-14,6,-8,6,2,8,8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,38,2,38,-8,
    36,-14,34,-18,30,-22,26,-24,18,-24,3,15,-27,22,-34,29,-27,2,21,18,-24,14,-22,10,-18,8,-14,6,-8,6,2,8,
    8,10,12,14,16,18,18,26,18,30,16,34,12,36,8,38,2,38,-8,36,-14,34,-18,30,-22,26,-24,18,-24,7,14,-29,16,
    -32,19,-33,22,-31,25,-29,28,-29,30,-32,3,21,18,-24,14,-22,10,-18,8,-14,6,-8,6,2,8,8,10,12,14,16,18,18,
    26,18,30,16,34,12,36,8,38,2,38,-8,36,-14,34,-18,30,-22,26,-24,18,-24,2,17,-32,17,-30,2,27,-32,27,-30,
    2,10,8,-24,8,6,10,12,14,16,20,18,24,18,30,16,34,12,36,6,36,-24,2,18,-35,25,-27,2,10,8,-24,8,6,10,12,14,
    16,20,18,24,18,30,16,34,12,36,6,36,-24,2,19,-27,26,-35,2,10,8,-24,8,6,10,12,14,16,20,18,24,18,30,16,34,
    12,36,6,36,-24,3,15,-27,22,-34,29,-27,3,10,8,-24,8,6,10,12,14,16,20,18,24,18,30,16,34,12,36,6,36,-24,
    2,17,-32,17,-30,2,27,-32,27,-30,3,3,2,-24,18,-4,18,18,2,34,-24,18,-4,2,15,-27,22,-35,3,2,30,-10,30,18,
    14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,2,15,-23,22,-15,3,
    2,30,-10,30,18,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,2,16,
    -15,23,-23,3,2,30,-10,30,18,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,
    16,30,12,3,12,-15,19,-22,26,-15,3,2,30,-10,30,18,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,
    12,16,16,18,22,18,26,16,30,12,7,11,-17,13,-20,16,-21,19,-19,22,-17,25,-17,27,-20,4,2,30,-10,30,18,14,
    30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,2,14,-20,14,-18,2,24,
    -20,24,-18,3,2,30,-10,30,18,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,
    16,30,12,7,19,-23,16,-21,16,-18,19,-16,22,-18,22,-21,19,-23,2,14,30,-4,26,-8,22,-10,16,-10,12,-8,8,-4,
    6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,6,19,18,19,22,23,24,23,27,19,29,15,28,2,17,6,2,30,2,30,-2,
    28,-6,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,2,14,-23,21,-15,2,17,
    6,2,30,2,30,-2,28,-6,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,2,15,-15,
    22,-23,2,17,6,2,30,2,30,-2,28,-6,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,
    30,12,3,11,-15,18,-22,25,-15,3,17,6,2,30,2,30,-2,28,-6,26,-8,22,-10,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,
    16,16,18,22,18,26,16,30,12,2,13,-20,13,-18,2,23,-20,23,-18,2,2,8,-10,8,18,2,4,-23,11,-15,2,2,8,-10,8,
    18,2,5,-15,12,-23,2,2,8,-10,8,18,3,1,-15,8,-22,15,-15,3,2,8,-10,8,18,2,3,-20,3,-18,2,13,-20,13,-18,3,
    2,8,-10,8,18,7,8,-2,14,-8,18,-10,24,-10,28,-8,30,-2,30,18,7,11,-17,13,-20,16,-21,19,-19,22,-17,25,-17,
    27,-20,2,17,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,32,6,32,2,30,-4,26,-8,22,-10,
    16,-10,2,15,-23,22,-15,2,17,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,32,6,32,2,30,
    -4,26,-8,22,-10,16,-10,2,16,-15,23,-23,2,17,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,
    12,32,6,32,2,30,-4,26,-8,22,-10,16,-10,3,12,-15,19,-22,26,-15,2,17,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,
    16,16,18,22,18,26,16,30,12,32,6,32,2,30,-4,26,-8,22,-10,16,-10,7,11,-17,13,-20,16,-21,19,-19,22,-17,25,
    -17,27,-20,3,17,16,-10,12,-8,8,-4,6,2,6,6,8,12,12,16,16,18,22,18,26,16,30,12,32,6,32,2,30,-4,26,-8,22,
    -10,16,-10,2,14,-20,14,-18,2,24,-20,24,-18,3,7,8,-10,8,10,10,16,14,18,20,18,24,16,30,10,2,30,-10,30,18,
    2,15,-23,22,-15,3,7,8,-10,8,10,10,16,14,18,20,18,24,16,30,10,2,30,-10,30,18,2,16,-15,23,-23,3,7,8,-10,
    8,10,10,16,14,18,20,18,24,16,30,10,2,30,-10,30,18,3,12,-15,19,-22,26,-15,4,7,8,-10,8,10,10,16,14,18,20,
    18,24,16,30,10,2,30,-10,30,18,2,14,-20,14,-18,2,24,-20,24,-18,3,2,4,-10,16,18,6,28,-10,16,18,12,26,8,
    30,4,32,2,32,2,13,-15,20,-23,4,2,4,-10,16,18,6,28,-10,16,18,12,26,8,30,4,32,2,32,2,11,-20,11,-18,2,21,
    -20,21,-18,
};
const GlyphIndex kGlyphIndex[] = {
    {0x0020, 32, 0},
    {0x0021, 20, 1},
    {0x0022, 32, 18},
    {0x0023, 42, 29},
    {0x0024, 40, 50},
    {0x0025, 48, 102},
    {0x0026, 52, 164},
    {0x0027, 20, 234},
    {0x0028, 28, 250},
    {0x0029, 28, 272},
    {0x002A, 32, 294},
    {0x002B, 52, 310},
    {0x002C, 16, 321},
    {0x002D, 52, 337},
    {0x002E, 16, 343},
    {0x002F, 44, 355},
    {0x0030, 40, 361},
    {0x0031, 40, 397},
    {0x0032, 40, 407},
    {0x0033, 40, 437},
    {0x0034, 40, 469},
    {0x0035, 40, 482},
    {0x0036, 40, 518},
    {0x0037, 40, 566},
    {0x0038, 40, 577},
    {0x0039, 40, 637},
    {0x003A, 16, 685},
    {0x003B, 16, 708},
    {0x003C, 48, 735},
    {0x003D, 52, 743},
    {0x003E, 48, 754},
    {0x003F, 36, 762},
    {0x0040, 54, 803},
    {0x0041, 36, 912},
    {0x0042, 42, 928},
    {0x0043, 42, 974},
    {0x0044, 42, 1012},
    {0x0045, 38, 1043},
    {0x0046, 36, 1064},
    {0x0047, 42, 1080},
    {0x0048, 44, 1125},
    {0x0049, 16, 1141},
    {0x004A, 32, 1147},
    {0x004B, 42, 1169},
    {0x004C, 34, 1185},
    {0x004D, 48, 1196},
    {0x004E, 44, 1217},
    {0x004F, 44, 1233},
    {0x0050, 42, 1277},
    {0x0051, 44, 1304},
    {0x0052, 42, 1353},
    {0x0053, 40, 1385},
    {0x0054, 32, 1427},
    {0x0055, 44, 1438},
    {0x0056, 36, 1460},
    {0x0057, 48, 1471},
    {0x0058, 40, 1492},
    {0x0059, 36, 1503},
    {0x005A, 40, 1516},
    {0x005B, 28, 1532},
    {0x005C, 28, 1553},
    {0x005D, 28, 1559},
    {0x005E, 32, 1580},
    {0x005F, 36, 1591},
    {0x0060, 16, 1597},
    {0x0061, 38, 1613},
    {0x0062, 38, 1648},
    {0x0063, 36, 1683},
    {0x0064, 38, 1713},
    {0x0065, 36, 1748},
    {0x0066, 24, 1784},
    {0x0067, 38, 1801},
    {0x0068, 38, 1846},
    {0x0069, 16, 1867},
    {0x006A, 20, 1884},
    {0x006B, 34, 1907},
    {0x006C, 16, 1923},
    {0x006D, 60, 1929},
    {0x006E, 38, 1965},
    {0x006F, 38, 1986},
    {0x0070, 38, 2022},
    {0x0071, 38, 2057},
    {0x0072, 26, 2092},
    {0x0073, 34, 2109},
    {0x0074, 24, 2145},
    {0x0075, 38, 2162},
    {0x0076, 32, 2183},
    {0x0077, 44, 2194},
    {0x0078, 34, 2215},
    {0x0079, 32, 2226},
    {0x007A, 34, 2245},
    {0x007B, 28, 2261},
    {0x007C, 16, 2339},
    {0x007D, 28, 2345},
    {0x007E, 48, 2423},
    {0x00AA, 24, 2470},
    {0x00B0, 20, 2510},
    {0x00BA, 24, 2530},
    {0x00C0, 36, 2571},
    {0x00C1, 36, 2592},
    {0x00C2, 36, 2613},
    {0x00C3, 36, 2636},
    {0x00C4, 36, 2667},
    {0x00C5, 36, 2693},
    {0x00C7, 42, 2724},
    {0x00C8, 38, 2775},
    {0x00C9, 38, 2801},
    {0x00CA, 38, 2827},
    {0x00CB, 38, 2855},
    {0x00CC, 16, 2886},
    {0x00CD, 16, 2897},
    {0x00CE, 16, 2908},
    {0x00CF, 16, 2921},
    {0x00D1, 44, 2937},
    {0x00D2, 44, 2968},
    {0x00D3, 44, 3017},
    {0x00D4, 44, 3066},
    {0x00D5, 44, 3117},
    {0x00D6, 44, 3176},
    {0x00D9, 44, 3230},
    {0x00DA, 44, 3257},
    {0x00DB, 44, 3284},
    {0x00DC, 44, 3313},
    {0x00DD, 36, 3345},
    {0x00E0, 38, 3363},
    {0x00E1, 38, 3403},
    {0x00E2, 38, 3443},
    {0x00E3, 38, 3485},
    {0x00E4, 38, 3535},
    {0x00E5, 38, 3580},
    {0x00E7, 36, 3630},
    {0x00E8, 36, 3673},
    {0x00E9, 36, 3714},
    {0x00EA, 36, 3755},
    {0x00EB, 36, 3798},
    {0x00EC, 16, 3844},
    {0x00ED, 16, 3855},
    {0x00EE, 16, 3866},
    {0x00EF, 16, 3879},
    {0x00F1, 38, 3895},
    {0x00F2, 38, 3931},
    {0x00F3, 38, 3972},
    {0x00F4, 38, 4013},
    {0x00F5, 38, 4056},
    {0x00F6, 38, 4107},
    {0x00F9, 38, 4153},
    {0x00FA, 38, 4179},
    {0x00FB, 38, 4205},
    {0x00FC, 38, 4233},
    {0x00FD, 32, 4264},
    {0x00FF, 32, 4288},
};
const int kGlyphCount = sizeof(kGlyphIndex) / sizeof(kGlyphIndex[0]);
