// ==WindhawkMod==
// @id              win32-ui-modernizer
// @name            Win32 UI Modernizer
// @name:pt         Modernizador de Interface Win32
// @name:es         Modernizador de Interfaz Win32
// @description     Modernizes legacy Win32 UI elements
// @description:pt  Moderniza elementos antigos da interface Win32
// @description:es  Moderniza elementos heredados de la interfaz Win32
// @version         1.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -lcomctl32 -ld2d1 -luxtheme
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Win32 UI Modernizer

Modernizes legacy Win32 UI elements across all applications.

### Features
- **ComboBox dropdowns**: Rounded corners, custom border, background and text colors
- **Rounded selection**: Rounded item backgrounds in Explorer nav pane and content area
- **Accent colorize**: Shift system colors and theme bitmaps to accent color
- **Accent marquee**: Use Windows accent color for drag-selection rectangles
- **Focus rectangle**: Suppress the dotted focus rectangle for modern look
- **Marquee selection**: Replace dotted drag-selection with modern semi-transparent style
- **TreeView insertion mark**: Clean opaque line instead of legacy zigzag
- **TreeView dotted lines**: Remove legacy dotted connecting lines
- **Explorer nav divider**: Remove the vertical separator line
- **Drag-drop normalizer**: Replace the drag overlay with a clean rounded rectangle
- Automatic light/dark mode detection
- Compatible with Translucent Windows mod

### Credits
- AccentColorizer system color and theme bitmap hue-shifting by
  [krlvm](https://github.com/krlvm/AccentColorizer)
- DragDrop bitmap normalization based on
  [DragDropNormalizer](https://github.com/krlvm/DragDropNormalizer) by krlvm

### Notes
- Resizable ComboBox dropdowns (with drag handles) are not styled due to
  technical limitations with the rounding technique
- Rounded selection may conflict with Translucent Windows if both modify
  the same theme elements

### Requirements
- Windows 11 version 22000 (21H2) or later


---

# Modernizador de Interface Win32 (Português)

Moderniza elementos antigos da interface Win32 em todos os aplicativos.

### Recursos
- **Menus suspensos do ComboBox**: Cantos arredondados, borda personalizada e cores de fundo e texto
- **Seleção arredondada**: Fundos de itens arredondados no painel de navegação e área de conteúdo do Explorer
- **Colorização com cor de destaque**: Ajusta cores do sistema e bitmaps de tema para a cor de destaque
- **Seleção por arrasto com cor de destaque**: Usa a cor de destaque do Windows para retângulos de seleção
- **Retângulo de foco moderno**: Remove o retângulo pontilhado para um visual mais moderno
- **Seleção por arrasto moderna**: Substitui a seleção pontilhada por um estilo semitransparente moderno
- **Marca de inserção do TreeView**: Linha limpa e opaca em vez do antigo zigue-zague
- **Linhas pontilhadas do TreeView**: Remove as linhas pontilhadas de conexão
- **Divisor de navegação do Explorer**: Remove a linha divisória vertical entre painéis
- **Normalizador de arrastar e soltar**: Substitui a sobreposição de arrastar por um retângulo arredondado limpo
- Detecção automática de modo claro/escuro
- Compatível com o mod Translucent Windows

### Créditos
- Colorização de cores do sistema e bitmaps baseada em
  [AccentColorizer](https://github.com/krlvm/AccentColorizer) por krlvm
- Normalização do bitmap de arrastar baseada em
  [DragDropNormalizer](https://github.com/krlvm/DragDropNormalizer) por krlvm

### Observações
- ComboBox redimensionáveis (com alças de redimensionamento) não são estilizados
  devido a limitações técnicas do método de arredondamento
- A seleção arredondada pode entrar em conflito com o mod Translucent Windows
  se ambos modificarem os mesmos elementos do tema

### Requisitos
- Windows 11 versão 22000 (21H2) ou posterior


---

# Modernizador de Interfaz Win32 (Español)

Moderniza elementos heredados de la interfaz Win32 en todas las aplicaciones.

### Características
- **Menús desplegables de ComboBox**: Esquinas redondeadas, borde personalizado y colores de fondo y texto
- **Selección redondeada**: Fondos de elementos redondeados en el panel de navegación y área de contenido del Explorador
- **Colorización con color de acento**: Ajusta colores del sistema y bitmaps del tema al color de acento
- **Selección por arrastre con color de acento**: Usa el color de acento de Windows para rectángulos de selección
- **Rectángulo de enfoque moderno**: Elimina el rectángulo punteado para un aspecto moderno
- **Selección por arrastre moderna**: Reemplaza la selección punteada por un estilo semitransparente moderno
- **Marca de inserción de TreeView**: Línea limpia y opaca en lugar del antiguo zigzag
- **Líneas punteadas de TreeView**: Elimina las líneas punteadas de conexión
- **Divisor de navegación del Explorador**: Elimina la línea divisoria vertical entre paneles
- **Normalizador de arrastrar y soltar**: Reemplaza la superposición de arrastre por un rectángulo redondeado limpio
- Detección automática de modo claro/oscuro
- Compatible con el mod Translucent Windows

### Créditos
- Colorización de colores del sistema y bitmaps basada en
  [AccentColorizer](https://github.com/krlvm/AccentColorizer) por krlvm
- Normalización del bitmap de arrastre basada en
  [DragDropNormalizer](https://github.com/krlvm/DragDropNormalizer) por krlvm

### Notas
- Los ComboBox redimensionables (con controles de redimensionamiento) no se estilizan
  debido a limitaciones técnicas del método de redondeo
- La selección redondeada puede entrar en conflicto con el mod Translucent Windows
  si ambos modifican los mismos elementos del tema

### Requisitos
- Windows 11 versión 22000 (21H2) o posterior

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ComboBoxSection:
  - Enabled: TRUE

  - CornerRadius: 5
    $name: Corner radius
    $name:pt: Raio dos cantos
    $name:es: Radio de las esquinas
    $description: Radius of the rounded corners in pixels (default 5, 0 for square)
    $description:pt: Raio dos cantos arredondados em pixels (padrão 5, 0 para quadrado)
    $description:es: Radio de las esquinas redondeadas en píxeles (predeterminado 5, 0 para cuadrado)

  - AutoColors: TRUE
    $name: Automatic colors (light/dark mode)
    $name:pt: Cores automáticas (modo claro/escuro)
    $name:es: Colores automáticos (modo claro/oscuro)
    $description: >-
     Automatically detect light/dark mode and use appropriate colors.
     Disable to use the custom colors below.
    $description:pt: >-
     Detecta automaticamente o modo claro/escuro e usa as cores apropriadas.
     Desative para usar as cores personalizadas abaixo.
    $description:es: >-
     Detecta automáticamente el modo claro/oscuro y usa los colores apropiados.
     Desactívelo para usar los colores personalizados de abajo.

  - BorderColor: "454545"
    $name: Custom border color
    $name:pt: Cor da borda personalizada
    $name:es: Color de borde personalizado
    $description: >-
     Hex RGB e.g. 454545 (only when Automatic colors is off)
    $description:pt: >-
     Hex RGB por exemplo 454545 (apenas quando cores automáticas estiverem desativadas)
    $description:es: >-
      Hex RGB por ejemplo 454545 (solo cuando los colores automáticos estén desactivados)

  - BackgroundColor: "191919"
    $name: Custom background color
    $name:pt: Cor de fundo personalizada
    $name:es: Color de fondo personalizado
    $description: >-
     Hex RGB e.g. 191919 (only when Automatic colors is off)
    $description:pt: >-
     Hex RGB por exemplo 191919 (apenas quando cores automáticas estiverem desativadas)
    $description:es: >-
     Hex RGB por ejemplo 191919 (solo cuando los colores automáticos estén desactivados)

  - TextColor: "FFFFFF"
    $name: Custom text color
    $name:pt: Cor de texto personalizada
    $name:es: Color de texto personalizado
    $description: >-
     Hex RGB e.g. FFFFFF (only when Automatic colors is off)
    $description:pt: >-
     Hex RGB por exemplo FFFFFF (apenas quando cores automáticas estiverem desativadas)
    $description:es: >-
     Hex RGB por ejemplo FFFFFF (solo cuando los colores automáticos estén desactivados)

  $name: ── ComboBox Dropdowns ──
  $name:pt: ── Menus suspensos do ComboBox ──
  $name:es: ── Menús desplegables de ComboBox ──
  $description: Enable ComboBox dropdown styling (colors, border, background)
  $description:pt: Ativa a estilização dos menus suspensos do ComboBox (cores, borda e fundo)
  $description:es: Activa el estilo de los menús desplegables de ComboBox (colores, borde y fondo)

- TreeViewSection:
  - Enabled: TRUE
  
  - ModernInsertMark: TRUE
    $name: Modern insertion mark
    $name:pt: Marca de inserção moderna
    $name:es: Marca de inserción moderna
    $description: Replace the legacy drag insertion mark with a clean line
    $description:pt: Substitui a marca de inserção de arrastar antiga por uma linha limpa
    $description:es: Reemplaza la marca de inserción antigua al arrastrar por una línea limpia

  - RemoveTreeLines: TRUE
    $name: Remove dotted tree lines
    $name:pt: Remover linhas pontilhadas da árvore
    $name:es: Eliminar líneas punteadas del árbol
    $description: Remove the dotted connecting lines (e.g. in regedit)
    $description:pt: Remove as linhas pontilhadas de conexão (ex. no regedit)
    $description:es: Elimina las líneas punteadas de conexión (por ejemplo en regedit)

  $name: ── TreeView ──
  $name:pt: ── TreeView ──
  $name:es: ── TreeView ──
  $description: Enable TreeView modernizations
  $description:pt: Ativa modernizações do TreeView
  $description:es: Activa modernizaciones del TreeView

- GeneralSection:
  - Enabled: TRUE

  - ModernFocusRect: TRUE
    $name: Modern focus rectangle
    $name:pt: Retângulo de foco moderno
    $name:es: Rectángulo de enfoque moderno
    $description: >-
     Replace the dotted focus rectangle with a subtle rounded highlight.
     Affects legacy controls like TreeView and ListView in apps like regedit.
    $description:pt: >-
     Substitui o retângulo de foco pontilhado por um destaque arredondado sutil.
     Afeta controles antigos como TreeView e ListView em apps como o regedit.
    $description:es: >-
     Reemplaza el rectángulo de enfoque punteado por un resaltado redondeado sutil.
     Afecta controles heredados como TreeView y ListView en aplicaciones como regedit.
  - AccentMarquee: FALSE
    $name: Accent color marquee selection
    $name:pt: Seleção de arrasto com cor de destaque
    $name:es: Selección de arrastre con color de acento
    $description: >-
     Use the Windows accent color for the drag-selection rectangle
     system-wide (affects COLOR_HOTLIGHT)
    $description:pt: >-
     Usa a cor de destaque do Windows para o retângulo de seleção por arrasto
     em todo o sistema (afeta COLOR_HOTLIGHT)
    $description:es: >-
     Usa el color de acento de Windows para el rectángulo de selección por arrastre
     en todo el sistema (afecta COLOR_HOTLIGHT)
  - AccentColorize: FALSE
    $name: Accent colorize system colors
    $name:pt: Colorir cores do sistema com a cor de destaque
    $name:es: Colorear colores del sistema con el color de acento
    $description: >-
     Shift system UI colors (highlight, hotlight, menus, captions) to match
     the Windows accent color hue. Based on AccentColorizer by krlvm.
    $description:pt: >-
     Ajusta as cores da interface do sistema (highlight, hotlight, menus, títulos)
     para combinar com o matiz da cor de destaque do Windows.
     Baseado no AccentColorizer de krlvm.
    $description:es: >-
     Ajusta los colores de la interfaz del sistema (highlight, hotlight, menús, títulos)
     para coincidir con el tono del color de acento de Windows.
     Basado en AccentColorizer de krlvm.
  - AccentStyles: FALSE
    $name: Accent colorize theme bitmaps
    $name:pt: Colorir bitmaps do tema com a cor de destaque
    $name:es: Colorear bitmaps del tema con el color de acento
    $description: >-
     Shift the hue of theme bitmaps (selection backgrounds, buttons, scrollbars)
     to match the accent color. Based on AccentColorizer by krlvm.
     Applies to Explorer TreeView, ListView, ItemsView selections etc.
    $description:pt: >-
     Ajusta o matiz dos bitmaps do tema (fundos de seleção, botões, barras de rolagem)
     para combinar com a cor de destaque. Baseado no AccentColorizer de krlvm.
    $description:es: >-
     Ajusta el tono de los bitmaps del tema (fondos de selección, botones, barras de desplazamiento)
     para coincidir con el color de acento. Basado en AccentColorizer de krlvm.
  - NormalizeDragDrop: FALSE
    $name: Normalize drag-drop overlay
    $name:pt: Normalizar sobreposição de arrastar e soltar
    $name:es: Normalizar superposición de arrastrar y soltar
    $description: >-
     Replace the drag-drop overlay bitmap with a clean rounded rectangle.
     Based on DragDropNormalizer by krlvm.
    $description:pt: >-
     Substitui o bitmap de sobreposição de arrastar e soltar por um retângulo arredondado limpo.
     Baseado no DragDropNormalizer de krlvm.
    $description:es: >-
     Reemplaza el bitmap de superposición de arrastrar y soltar por un rectángulo redondeado limpio.
     Basado en DragDropNormalizer de krlvm.

  $name: ── General ──
  $name:pt: ── Geral ──
  $name:es: ── General ──
  $description: Enable general UI modernizations. Do not use the accent color options with AccentColorizer or Translucent Windows.
  $description:pt: Ativa modernizações gerais da interface. Não use as opções de cor de destaque com o AccentColorizer ou o Translucent Windows.
  $description:es: Activa modernizaciones generales de la interfaz. No utilice las opciones de color de acento con AccentColorizer o Translucent Windows.

- ExplorerSection:
  - Enabled: TRUE
  
  - RoundedSelection: FALSE
    $name: Rounded selection backgrounds
    $name:pt: Fundos de seleção arredondados
    $name:es: Fondos de selección redondeados
    $description: >-
     Round the corners of item selection/hover backgrounds in Explorer
     navigation pane and content area. May conflict with Translucent Windows.
    $description:pt: >-
     Arredonda os cantos dos fundos de seleção/hover no painel de navegação
     e área de conteúdo do Explorer. Pode conflitar com Translucent Windows.
    $description:es: >-
     Redondea las esquinas de los fondos de selección/hover en el panel
     de navegación y el área de contenido del Explorador.
  - RemoveNavDivider: FALSE
    $name: Remove navigation divider
    $name:pt: Remover divisor de navegação
    $name:es: Eliminar divisor de navegación
    $description: Remove the vertical divider line between panels (off by default)
    $description:pt: Remove a linha divisória vertical entre os painéis (desativado por padrão)
    $description:es: Elimina la línea divisoria vertical entre paneles (desactivado por defecto)
  - RemoveNavDividerTW: FALSE
    $name: Remove navigation divider (Translucent Windows compatibility)
    $name:pt: Remover divisor de navegação (compatibilidade com Translucent Windows)
    $name:es: Eliminar divisor de navegación (compatibilidad con Translucent Windows)
    $description: >-
     Use this method instead when Translucent Windows mod is active.
     May need reapplying after explorer.exe restarts.
    $description:pt: >-
     Use este método quando o mod Translucent Windows estiver ativo.
     Pode ser necessário reaplicar após reiniciar o explorer.exe.
    $description:es: >-
     Use este método cuando el mod Translucent Windows esté activo.
     Puede ser necesario reaplicarlo después de reiniciar explorer.exe.
  - ModernGroupHeaders: FALSE
    $name: Modern group headers (WinUI-style pills)
    $name:pt: Cabeçalhos de grupo modernos (estilo WinUI)
    $name:es: Encabezados de grupo modernos (estilo WinUI)
    $description: >-
     Replaces the flat section headers (Folders, Devices and Drives) with
     compact pill-shaped backgrounds. Suppresses the separator line.
     Compatible with Translucent Windows.
    $description:pt: >-
     Substitui os cabeçalhos planos de seção (Pastas, Dispositivos e unidades)
     por fundos compactos em formato de pílula. Remove a linha separadora.
    $description:es: >-
     Reemplaza los encabezados planos de sección (Carpetas, Dispositivos y unidades)
     por fondos compactos con forma de píldora. Elimina la línea separadora.

  $name: ── Explorer ──
  $name:pt: ── Explorer ──
  $name:es: ── Explorador ──
  $description: Enable Explorer-specific modernizations
  $description:pt: Ativa modernizações específicas do Explorer
  $description:es: Activa modernizaciones específicas del Explorador
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dwmapi.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <vsstyle.h>
#include <d2d1.h>
#include <wrl.h>
#include <string>
#include <math.h>

#ifndef GBF_DIRECT
#define GBF_DIRECT 0x00000001
#endif

#ifndef TMT_DIBDATA
#define TMT_DIBDATA 2
#endif

struct {
    BOOL     ComboSection     = TRUE;
    INT      CornerRadius     = 5;
    BOOL     AutoColors       = TRUE;
    COLORREF BorderClr        = RGB(0x45, 0x45, 0x45);
    COLORREF BgColor          = RGB(0x19, 0x19, 0x19);
    COLORREF TextColor        = RGB(0xFF, 0xFF, 0xFF);
    BOOL     TreeSection      = TRUE;
    BOOL     ModernInsert     = TRUE;
    BOOL     RemoveTreeLines  = TRUE;
    COLORREF InsertMarkClr    = RGB(0x6B, 0x6B, 0x6B);
    BOOL     GeneralSection   = TRUE;
    BOOL     ModernFocusRect  = TRUE;
    BOOL     AccentMarquee    = FALSE;
    BOOL     AccentColorize   = FALSE;
    BOOL     AccentStyles     = FALSE;
    BOOL     NormalizeDragDrop = FALSE;
    BOOL     ExplorerSection  = TRUE;
    BOOL     RoundedSelection = FALSE;
    BOOL     RemoveNavDivider = FALSE;
    BOOL     RemoveNavDividerTW = FALSE;
    BOOL     ModernGroupHeaders = FALSE;
} g_settings;

static HBRUSH        g_bgBrush       = nullptr;
static ID2D1Factory* g_d2dFactory    = nullptr;
static HHOOK         g_msgHook       = nullptr;
static COLORREF      g_origHotlight  = 0;
static COLORREF      g_origHighlight = 0;
static bool          g_hotlightSaved = false;

// AccentColorize — mesmos elementos que o AccentColorizer
static const INT g_accentElems[] = {
    COLOR_ACTIVECAPTION, COLOR_GRADIENTACTIVECAPTION,
    COLOR_HIGHLIGHT, COLOR_HOTLIGHT, COLOR_MENUHILIGHT
};
static const int g_nAccentElems = sizeof(g_accentElems) / sizeof(*g_accentElems);
static COLORREF  g_origAccentCols[5] = {};
static bool      g_accentSaved = false;

static const wchar_t* PROP_LB_SUB = L"WH_MW_LB";
static const wchar_t* PROP_CB_SUB = L"WH_MW_CB";
static const wchar_t* PROP_TV_SUB = L"WH_MW_TV";
static const wchar_t* PROP_LASTW  = L"WH_MW_W";
static const wchar_t* PROP_LASTH  = L"WH_MW_H";

// ── Helpers ──────────────────────────────────────────────────────────────────

static bool ParseHexColor(const wchar_t* str, COLORREF& out)
{
    if (!str || !*str) return false;
    unsigned long val = wcstoul(str, nullptr, 16);
    out = RGB((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
    return true;
}

static bool IsSystemDarkMode()
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD val = 1, size = sizeof(val);
        RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr,
                         (LPBYTE)&val, &size);
        RegCloseKey(hKey);
        return (val == 0);
    }
    return true;
}

// AccentPalette layout: Light3(0), Light2(4), Light1(8), Base(12), Dark1(16), Dark2(20), Dark3(24)
// Cada entrada: R, G, B, padding (4 bytes)
static bool ReadAccentPalette(BYTE* palette, DWORD paletteSize)
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD size = paletteSize;
        if (RegQueryValueExW(hKey, L"AccentPalette", nullptr, nullptr, palette, &size) == ERROR_SUCCESS)
        { RegCloseKey(hKey); return true; }
        RegCloseKey(hKey);
    }
    return false;
}

static COLORREF GetAccentFromPalette(int offset)
{
    BYTE palette[32] = {};
    if (ReadAccentPalette(palette, sizeof(palette)))
        return RGB(palette[offset], palette[offset + 1], palette[offset + 2]);
    // Fallback
    DWORD color = 0; BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque)))
        return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    return GetSysColor(COLOR_HOTLIGHT);
}

// Dark mode → Light1 (offset 8) — visível mas não excessivamente claro
// Light mode → Base (offset 12) — padrão
static COLORREF GetSystemAccentColor()
{
    return GetAccentFromPalette(IsSystemDarkMode() ? 8 : 12);
}

// Base accent (offset 12) — sempre, para hue reference
static COLORREF GetAccentBase()
{
    return GetAccentFromPalette(12);
}

static D2D1_COLOR_F GetSystemAccentColorD2D(bool dark)
{
    // Light1 for dark mode, Base for light mode — same as GetSystemAccentColor
    COLORREF c = dark ? GetAccentFromPalette(8) : GetAccentFromPalette(12);
    return D2D1::ColorF(
        GetRValue(c) / 255.0f,
        GetGValue(c) / 255.0f,
        GetBValue(c) / 255.0f);
}

// HSV conversion — same technique as AccentColorizer
struct hsv_t { double h, s, v; };

static hsv_t rgb2hsv(double r, double g, double b)
{
    hsv_t out;
    double mn = r < g ? r : g; mn = mn < b ? mn : b;
    double mx = r > g ? r : g; mx = mx > b ? mx : b;
    out.v = mx;
    double delta = mx - mn;
    if (delta < 0.00001) { out.s = 0; out.h = 0; return out; }
    if (mx > 0.0) out.s = delta / mx; else { out.s = 0; out.h = 0; return out; }
    if (r >= mx) out.h = (g - b) / delta;
    else if (g >= mx) out.h = 2.0 + (b - r) / delta;
    else out.h = 4.0 + (r - g) / delta;
    out.h *= 60.0;
    if (out.h < 0.0) out.h += 360.0;
    return out;
}

static COLORREF hsv2rgb_cr(hsv_t in)
{
    double hh = in.h;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    long i = (long)hh;
    double ff = hh - i;
    double p = in.v * (1.0 - in.s);
    double q = in.v * (1.0 - in.s * ff);
    double t = in.v * (1.0 - in.s * (1.0 - ff));
    double r, g, b;
    switch (i) {
    case 0: r=in.v; g=t; b=p; break;
    case 1: r=q; g=in.v; b=p; break;
    case 2: r=p; g=in.v; b=t; break;
    case 3: r=p; g=q; b=in.v; break;
    case 4: r=t; g=p; b=in.v; break;
    default: r=in.v; g=p; b=q; break;
    }
    return RGB((BYTE)r, (BYTE)g, (BYTE)b);
}

static void ApplyAccentColorize()
{
    if (g_settings.AccentColorize)
    {
        if (!g_accentSaved)
        {
            for (int i = 0; i < g_nAccentElems; i++)
                g_origAccentCols[i] = GetSysColor(g_accentElems[i]);
            g_accentSaved = true;
        }

        // Usa a accent color da paleta diretamente (Light2 no dark, Base no light)
        COLORREF accent = GetSystemAccentColor();

        COLORREF newCols[5];
        for (int i = 0; i < g_nAccentElems; i++)
            newCols[i] = accent;

        SetSysColors(g_nAccentElems, g_accentElems, newCols);
    }
    else if (g_accentSaved)
    {
        SetSysColors(g_nAccentElems, g_accentElems, g_origAccentCols);
        g_accentSaved = false;
    }
}

static void ApplyAutoColors()
{
    if (IsSystemDarkMode())
    {
        g_settings.BgColor   = RGB(0x19, 0x19, 0x19);
        g_settings.BorderClr = RGB(0x45, 0x45, 0x45);
        g_settings.TextColor = RGB(0xFF, 0xFF, 0xFF);
    }
    else
    {
        g_settings.BgColor   = RGB(0xF9, 0xF9, 0xF9);
        g_settings.BorderClr = RGB(0xD0, 0xD0, 0xD0);
        g_settings.TextColor = RGB(0x1A, 0x1A, 0x1A);
    }
}

static void RecreateBrush()
{
    if (g_bgBrush) DeleteObject(g_bgBrush);
    g_bgBrush = CreateSolidBrush(g_settings.BgColor);
}

static void ApplyAccentMarquee()
{
    // Se AccentColorize está ativo, ele já cuida de COLOR_HOTLIGHT/HIGHLIGHT
    if (g_settings.AccentColorize) return;

    if (g_settings.AccentMarquee)
    {
        if (!g_hotlightSaved)
        {
            g_origHotlight  = GetSysColor(COLOR_HOTLIGHT);
            g_origHighlight = GetSysColor(COLOR_HIGHLIGHT);
            g_hotlightSaved = true;
        }
        // Usa a accent color diretamente (Light2 no dark mode, Base no light)
        COLORREF accent = GetSystemAccentColor();

        INT elems[2]    = { COLOR_HOTLIGHT, COLOR_HIGHLIGHT };
        COLORREF cols[2] = { accent, accent };
        SetSysColors(2, elems, cols);
    }
    else if (g_hotlightSaved)
    {
        INT elems[2]    = { COLOR_HOTLIGHT, COLOR_HIGHLIGHT };
        COLORREF cols[2] = { g_origHotlight, g_origHighlight };
        SetSysColors(2, elems, cols);
        g_hotlightSaved = false;
    }
}

// Forward declarations
static void ModifyThemeStyles();
static void NormalizeDragDrop();

static void UpdateThemeColors()
{
    if (g_settings.AutoColors)
    {
        COLORREF old = g_settings.BgColor;
        ApplyAutoColors();
        if (old != g_settings.BgColor)
            RecreateBrush();
    }
    ApplyAccentColorize();
    ApplyAccentMarquee();
    if (g_settings.AccentStyles)  ModifyThemeStyles();
    if (g_settings.NormalizeDragDrop) NormalizeDragDrop();
}

static BOOL CALLBACK RefreshExplorerEnum(HWND hwnd, LPARAM)
{
    wchar_t cls[64];
    GetClassNameW(hwnd, cls, 64);
    if (_wcsicmp(cls, L"CabinetWClass") == 0)
        RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE);
    return TRUE;
}

typedef HRESULT(WINAPI* GetThemeClass_t)(HTHEME, LPCWSTR, INT);
static GetThemeClass_t pGetThemeClass = nullptr;

static std::wstring GetThemeClassName(HTHEME hTheme)
{
    if (pGetThemeClass)
    {
        WCHAR buf[256] = {};
        if (SUCCEEDED(pGetThemeClass(hTheme, buf, 256)))
            return buf;
    }
    return L"";
}

static bool IsInsideExplorer(HWND hFrom)
{
    HWND root = GetAncestor(hFrom, GA_ROOT);
    if (!root) return false;

    wchar_t cls[64] = {};
    GetClassNameW(root, cls, ARRAYSIZE(cls));

    return (_wcsicmp(cls, L"CabinetWClass") == 0 ||
            _wcsicmp(cls, L"ExploreWClass") == 0);
}

static bool IsResizableCombo(HWND hwnd)
{
    if (GetWindowLongW(hwnd, GWL_STYLE) & WS_THICKFRAME) return true;
    HWND hParent = GetParent(hwnd);
    if (hParent)
    {
        wchar_t cls[32] = {};
        GetClassNameW(hParent, cls, ARRAYSIZE(cls));
        if (_wcsicmp(cls, L"ComboBox") == 0 &&
            (GetWindowLongW(hParent, GWL_STYLE) & WS_THICKFRAME))
            return true;
    }
    return false;
}

// ── Theme change hook ────────────────────────────────────────────────────────

static LRESULT CALLBACK ThemeMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        MSG* msg = (MSG*)lParam;
        if ((msg->message == WM_SETTINGCHANGE && msg->lParam &&
             wcscmp((LPCWSTR)msg->lParam, L"ImmersiveColorSet") == 0) ||
            msg->message == WM_THEMECHANGED)
        {
            UpdateThemeColors();
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ══════════════════════════════════════════════════════════════════════════════
// ── COMBOBOX DROPDOWN ────────────────────────────────────────────────────────
// ══════════════════════════════════════════════════════════════════════════════

static void ApplyRoundedRegion(HWND hwnd)
{
    if (g_settings.CornerRadius <= 0 || IsResizableCombo(hwnd)) return;
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;
    int gr = g_settings.CornerRadius + 3;
    HRGN hRgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, gr * 2, gr * 2);
    if (hRgn) SetWindowRgn(hwnd, hRgn, TRUE);
    SetPropW(hwnd, PROP_LASTW, (HANDLE)(LONG_PTR)w);
    SetPropW(hwnd, PROP_LASTH, (HANDLE)(LONG_PTR)h);
}

static void DrawD2DBorder(HWND hwnd)
{
    if (!g_d2dFactory || g_settings.CornerRadius <= 0 || IsResizableCombo(hwnd)) return;
    HDC hdc = GetDC(hwnd);
    if (!hdc) return;
    RECT rc; GetClientRect(hwnd, &rc);
    int w = rc.right, h = rc.bottom, r = g_settings.CornerRadius;

    D2D1_RENDER_TARGET_PROPERTIES rtP = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRT;
    if (FAILED(g_d2dFactory->CreateDCRenderTarget(&rtP, &pRT))) { ReleaseDC(hwnd, hdc); return; }
    RECT brc = {0,0,w,h};
    if (FAILED(pRT->BindDC(hdc, &brc))) { ReleaseDC(hwnd, hdc); return; }

    FLOAT fr=(FLOAT)r, fw=(FLOAT)w, fh=(FLOAT)h;
    D2D1_COLOR_F bc = D2D1::ColorF(GetRValue(g_settings.BorderClr)/255.f, GetGValue(g_settings.BorderClr)/255.f, GetBValue(g_settings.BorderClr)/255.f);
    D2D1_COLOR_F bg = D2D1::ColorF(GetRValue(g_settings.BgColor)/255.f, GetGValue(g_settings.BgColor)/255.f, GetBValue(g_settings.BgColor)/255.f);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bBr, bgBr;
    pRT->CreateSolidColorBrush(bc, &bBr);
    pRT->CreateSolidColorBrush(bg, &bgBr);
    pRT->BeginDraw();
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0,0,fw,fh), fr+3, fr+3), bgBr.Get(), 6.f);
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(.5f,.5f,fw-.5f,fh-.5f), fr, fr), bBr.Get(), 1.f);
    pRT->EndDraw();
    ReleaseDC(hwnd, hdc);
}

static LRESULT CALLBACK ComboParentSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uId, DWORD_PTR)
{
    if (uMsg == WM_CTLCOLORLISTBOX) {
        SetBkColor((HDC)wParam, g_settings.BgColor);
        SetTextColor((HDC)wParam, g_settings.TextColor);
        return (LRESULT)g_bgBrush;
    }
    if (uMsg == WM_NCDESTROY) { RemoveWindowSubclass(hWnd, ComboParentSubclass, uId); RemovePropW(hWnd, PROP_CB_SUB); }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK ComboLBoxSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uId, DWORD_PTR)
{
    switch (uMsg) {
    case WM_WINDOWPOSCHANGED: {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!IsResizableCombo(hWnd) && g_settings.CornerRadius > 0) {
            RECT rc; GetWindowRect(hWnd, &rc);
            int w=rc.right-rc.left, h=rc.bottom-rc.top;
            if (w != (int)(LONG_PTR)GetPropW(hWnd, PROP_LASTW) || h != (int)(LONG_PTR)GetPropW(hWnd, PROP_LASTH))
                ApplyRoundedRegion(hWnd);
        }
        return res;
    }
    case WM_NCPAINT:
        if (g_settings.CornerRadius > 0 && !IsResizableCombo(hWnd)) return 0;
        break;
    case WM_PAINT: {
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        DrawD2DBorder(hWnd);
        return res;
    }
    case WM_ERASEBKGND: {
        RECT rc; GetClientRect(hWnd, &rc);
        FillRect((HDC)wParam, &rc, g_bgBrush);
        return 1;
    }
    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, ComboLBoxSubclass, uId);
        RemovePropW(hWnd, PROP_LB_SUB); RemovePropW(hWnd, PROP_LASTW); RemovePropW(hWnd, PROP_LASTH);
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ══════════════════════════════════════════════════════════════════════════════
// ── TREEVIEW INSERTION MARK ──────────────────────────────────────────────────
// ══════════════════════════════════════════════════════════════════════════════

static HTREEITEM g_insertMarkItem = nullptr;
static BOOL g_insertMarkAfter = FALSE;
constexpr UINT TVM_SETINSERTMARK_ = 0x111A;
constexpr UINT TVM_GETITEMRECT_   = 0x1104;

static void DrawModernInsertMark(HWND tv, HDC hdc)
{
    if (!g_insertMarkItem || !g_d2dFactory) return;
    RECT ir = {}; *(HTREEITEM*)&ir = g_insertMarkItem;
    if (!SendMessageW(tv, TVM_GETITEMRECT_, TRUE, (LPARAM)&ir)) return;
    int lineY = g_insertMarkAfter ? ir.bottom : ir.top;
    RECT cr; GetClientRect(tv, &cr);

    D2D1_RENDER_TARGET_PROPERTIES rtP = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),0,0,D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,D2D1_FEATURE_LEVEL_DEFAULT);
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRT;
    if (FAILED(g_d2dFactory->CreateDCRenderTarget(&rtP, &pRT))) return;
    int bt = lineY-4, bb = lineY+4;
    if (bt<0) bt=0; if (bb>cr.bottom) bb=cr.bottom;
    RECT brc = {0,bt,cr.right,bb};
    if (FAILED(pRT->BindDC(hdc, &brc))) return;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> br;
    pRT->CreateSolidColorBrush(D2D1::ColorF(GetRValue(g_settings.InsertMarkClr)/255.f, GetGValue(g_settings.InsertMarkClr)/255.f, GetBValue(g_settings.InsertMarkClr)/255.f), &br);
    pRT->BeginDraw();
    pRT->FillRectangle(D2D1::RectF(4.f, (FLOAT)(lineY-bt)-1.f, (FLOAT)cr.right-4.f, (FLOAT)(lineY-bt)+1.f), br.Get());
    pRT->EndDraw();
}

static LRESULT CALLBACK TreeViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uId, DWORD_PTR)
{
    if (uMsg == TVM_SETINSERTMARK_ && g_settings.ModernInsert) {
        g_insertMarkAfter = (BOOL)wParam; g_insertMarkItem = (HTREEITEM)lParam;
        if (!g_insertMarkItem) { auto r = DefSubclassProc(hWnd, uMsg, wParam, lParam); InvalidateRect(hWnd,0,TRUE); return r; }
        InvalidateRect(hWnd,0,FALSE); return TRUE;
    }
    if (uMsg == WM_PAINT) {
        auto res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (g_insertMarkItem && g_settings.ModernInsert) { HDC hdc=GetDC(hWnd); if(hdc){DrawModernInsertMark(hWnd,hdc);ReleaseDC(hWnd,hdc);} }
        return res;
    }
    if (uMsg == WM_NCDESTROY) { RemoveWindowSubclass(hWnd, TreeViewSubclass, uId); RemovePropW(hWnd, PROP_TV_SUB); g_insertMarkItem=nullptr; }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ══════════════════════════════════════════════════════════════════════════════
// ── MODERN FOCUS RECTANGLE ───────────────────────────────────────────────────
// ══════════════════════════════════════════════════════════════════════════════

static decltype(&DrawFocusRect) DrawFocusRect_orig = nullptr;
BOOL WINAPI DrawFocusRect_hook(HDC hdc, CONST RECT* lprc)
{
    if (g_settings.ModernFocusRect)
        return TRUE;  // Suprime o retângulo pontilhado — estilo moderno Win11

    return DrawFocusRect_orig(hdc, lprc);
}

// ══════════════════════════════════════════════════════════════════════════════
// ── ROUNDED SELECTION BACKGROUNDS ────────────────────────────────────────────
// ══════════════════════════════════════════════════════════════════════════════

static bool DrawRoundedItemBg(HDC hdc, LPCRECT pRect, INT iStateId, bool isTreeView)
{
    if (!g_d2dFactory || !pRect) return false;
    int w = pRect->right - pRect->left, h = pRect->bottom - pRect->top;
    if (w <= 0 || h <= 0) return false;

    D2D1_RENDER_TARGET_PROPERTIES rtP = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),0,0,
        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRT;
    if (FAILED(g_d2dFactory->CreateDCRenderTarget(&rtP, &pRT))) return false;
    RECT brc = *pRect;
    if (FAILED(pRT->BindDC(hdc, &brc))) return false;

    FLOAT fw=(FLOAT)w, fh=(FLOAT)h, cr=5.f, padX = isTreeView ? 1.f : 0.f;
    bool dark = IsSystemDarkMode();
    D2D1_COLOR_F fill;

    // Cores opacas pré-misturadas (sem alpha — compatível com qualquer DC)
    switch (iStateId) {
    case 1: // NORMAL — borda de foco arredondada (sem fill)
    {
        D2D1_COLOR_F borderClr = dark ? D2D1::ColorF(0.35f,0.35f,0.35f) : D2D1::ColorF(0.75f,0.75f,0.75f);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBr;
        pRT->CreateSolidColorBrush(borderClr, &borderBr);
        pRT->BeginDraw();
        pRT->DrawRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(padX + 0.5f, 0.5f, fw - padX - 0.5f, fh - 0.5f), cr, cr),
            borderBr.Get(), 1.0f);
        pRT->EndDraw();
        return true;
    }
    case 2: fill = dark ? D2D1::ColorF(0.22f,0.22f,0.22f) : D2D1::ColorF(0.93f,0.93f,0.93f); break; // HOT
    case 3: fill = dark ? D2D1::ColorF(0.25f,0.25f,0.25f) : D2D1::ColorF(0.90f,0.90f,0.90f); break; // SELECTED
    case 6: fill = dark ? D2D1::ColorF(0.28f,0.28f,0.28f) : D2D1::ColorF(0.87f,0.87f,0.87f); break; // HOTSELECTED
    case 5: fill = dark ? D2D1::ColorF(0.20f,0.20f,0.20f) : D2D1::ColorF(0.95f,0.95f,0.95f); break; // SELECTEDNOTFOCUS
    default: return false;
    }

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> br;
    pRT->CreateSolidColorBrush(fill, &br);
    pRT->BeginDraw();
    pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(padX, 0, fw-padX, fh), cr, cr), br.Get());
    pRT->EndDraw();
    return true;
}

// ══════════════════════════════════════════════════════════════════════════════
// ── DRAWTHEMEBACKGROUND HOOKS ────────────────────────────────────────────────
// ══════════════════════════════════════════════════════════════════════════════

static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_orig = nullptr;

constexpr INT TVP_TREEITEM_ = 1;
constexpr INT LVP_LISTITEM_ = 1;

static bool HandleThemeDraw(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!pRect) return false;

    // Nav divider removal
    if ((g_settings.RemoveNavDivider || g_settings.RemoveNavDividerTW) && (iPartId == 3 || iPartId == 4))
    {
        std::wstring cls = GetThemeClassName(hTheme);
        if (cls == L"PreviewPane" || cls == L"NavPane")
        {
            if (g_settings.RemoveNavDividerTW) return true;
            if (IsSystemDarkMode()) {
                RECT rc = *pRect; rc.left--; rc.right++;
                FillRect(hdc, &rc, g_bgBrush);
            }
            return true;
        }
    }

// Rounded selection backgrounds
if (g_settings.RoundedSelection)
{
    std::wstring cls = GetThemeClassName(hTheme);

    // TreeView: "TreeView" ou "Explorer::TreeView"
    if (iPartId == TVP_TREEITEM_ &&
        (cls == L"TreeView" || cls == L"Explorer::TreeView"))
    {
        bool focused = (iStateId == 4 || iStateId == 5);

        if (iStateId == 1) // NORMAL
            return DrawRoundedItemBg(hdc, pRect, iStateId, true);

        if (iStateId >= 2)
            return DrawRoundedItemBg(hdc, pRect, iStateId, focused);
    }

    // TreeView COMMONPROPS_PART (part 0) — fundo do controle
    if (iPartId == 0 &&
        (cls == L"TreeView" || cls == L"Explorer::TreeView"))
    {
        // Suprime — deixa o fundo transparente (ou do parent)
        return true;
    }

    // ListView: "ItemsView", "ListView", "Explorer::ListView", "ItemsView::ListView"
    if (iPartId == LVP_LISTITEM_ &&
        (cls == L"ItemsView" || cls == L"ListView" ||
         cls == L"Explorer::ListView" || cls == L"ItemsView::ListView"))
    {
        bool focused = (iStateId == 4 || iStateId == 5);

        if (iStateId == 1) // NORMAL
            return DrawRoundedItemBg(hdc, pRect, iStateId, false);

        if (iStateId >= 2)
            return DrawRoundedItemBg(hdc, pRect, iStateId, focused);
    }
}

    // Modern group headers — pill background for ALL states + suppress line
    if (g_settings.ModernGroupHeaders)
    {
        std::wstring cls = GetThemeClassName(hTheme);
        bool isLV = (cls == L"ItemsView" || cls == L"ListView" ||
                     cls == L"Explorer::ListView" || cls == L"ItemsView::ListView");
        if (isLV)
        {
            // Focus/selection border (part 3) on group headers → draw pill with white border
            if (iPartId == 3 && (iStateId == 1 || iStateId == 2))
            {
                int w = pRect->right - pRect->left;
                if (w > 400 && g_d2dFactory)
                {
                    int h = pRect->bottom - pRect->top;
                    if (h <= 0) return true;

                    int pillW = (int)(w * 0.22f);
                    if (pillW < 100) pillW = 100;
                    if (pillW > 250) pillW = 250;

                    RECT pillRc = {
                        pRect->left + 1,
                        pRect->top,
                        pRect->left + 1 + pillW,
                        pRect->bottom
                    };

                    float ph = (float)h;
                    float pw = (float)pillW;
                    float cr = ph / 2.0f;

                    D2D1_RENDER_TARGET_PROPERTIES rtP = D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
                    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRT;
                    if (SUCCEEDED(g_d2dFactory->CreateDCRenderTarget(&rtP, &pRT)) &&
                        SUCCEEDED(pRT->BindDC(hdc, &pillRc)))
                    {
                        bool dark = IsSystemDarkMode();
                        D2D1_COLOR_F fill = dark ? D2D1::ColorF(0.14f, 0.14f, 0.16f)
                                                 : D2D1::ColorF(0.90f, 0.90f, 0.92f);
                        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> br;
                        pRT->CreateSolidColorBrush(fill, &br);
                        pRT->BeginDraw();

                        // Fill pill
                        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(
                            D2D1::RectF(1.0f, 1.0f, pw - 1.0f, ph - 1.0f), cr - 1.0f, cr - 1.0f);
                        pRT->FillRoundedRectangle(rr, br.Get());

                        // White thick border (WinUI style)
                        D2D1_COLOR_F borderClr = dark ? D2D1::ColorF(0.85f, 0.85f, 0.85f)
                                                      : D2D1::ColorF(0.20f, 0.20f, 0.20f);
                        br->SetColor(borderClr);
                        pRT->DrawRoundedRectangle(
                            D2D1::RoundedRect(
                                D2D1::RectF(1.0f, 1.0f, pw - 1.0f, ph - 1.0f), cr - 1.0f, cr - 1.0f),
                            br.Get(), 2.0f);

                        pRT->EndDraw();
                    }
                    return true;
                }
            }

            // Group header background (part 6): draw pill within pRect bounds
            if (iPartId == 6 /*LVP_GROUPHEADER*/ && g_d2dFactory)
            {
                int h = pRect->bottom - pRect->top;
                int w = pRect->right - pRect->left;
                if (h <= 0 || w <= 0) return true;

                int pillW = (int)(w * 0.22f);
                if (pillW < 100) pillW = 100;
                if (pillW > 250) pillW = 250;

                // Use full pRect height — no expansion, no clipping
                RECT pillRc = {
                    pRect->left + 1,
                    pRect->top,
                    pRect->left + 1 + pillW,
                    pRect->bottom
                };

                float ph = (float)h;
                float pw = (float)pillW;
                float cr = ph / 2.0f;

                bool isHovered = (iStateId % 2 == 0);

                D2D1_RENDER_TARGET_PROPERTIES rtP = D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT,
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                    0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
                Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRT;
                if (SUCCEEDED(g_d2dFactory->CreateDCRenderTarget(&rtP, &pRT)) &&
                    SUCCEEDED(pRT->BindDC(hdc, &pillRc)))
                {
                    bool dark = IsSystemDarkMode();
                    D2D1_COLOR_F fill;
                   if (isHovered)
{
    D2D1_COLOR_F accent = GetSystemAccentColorD2D(dark);

    if (dark)
    {
        // no dark mode a accent precisa ser suavizada
        fill = D2D1::ColorF(accent.r * 0.55f, accent.g * 0.55f, accent.b * 0.55f);
    }
    else
    {
        // no light mode ela deve ficar MAIS CLARA
        fill = D2D1::ColorF(
            accent.r * 0.35f + 0.65f,
            accent.g * 0.35f + 0.65f,
            accent.b * 0.35f + 0.65f
        );
    }
}
                   else
                        fill = dark ? D2D1::ColorF(0.14f, 0.14f, 0.16f)
                                    : D2D1::ColorF(0.90f, 0.90f, 0.92f);

                    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> br;
                    pRT->CreateSolidColorBrush(fill, &br);
                    pRT->BeginDraw();
                    pRT->FillRoundedRectangle(
                        D2D1::RoundedRect(D2D1::RectF(0, 0, pw, ph), cr, cr), br.Get());
                    pRT->EndDraw();
                }
                return true;
            }
        }
    }

    return false;
}

HRESULT WINAPI DrawThemeBackground_hook(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect)
{
    if (g_settings.ExplorerSection && HandleThemeDraw(hTheme, hdc, iPartId, iStateId, pRect))
        return S_OK;
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

HRESULT WINAPI DrawThemeBackgroundEx_hook(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, const DTBGOPTS* pOptions)
{
    if (g_settings.ExplorerSection && HandleThemeDraw(hTheme, hdc, iPartId, iStateId, pRect))
        return S_OK;
    return DrawThemeBackgroundEx_orig(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
}

// ══════════════════════════════════════════════════════════════════════════════
// ── WINDOW CREATION HOOK ─────────────────────────────────────────────────────
// ══════════════════════════════════════════════════════════════════════════════

using NtUserCreateWindowEx_t = HWND(WINAPI*)(DWORD, VOID*, LPCWSTR, VOID*, DWORD, LONG, LONG, LONG, LONG, HWND, HMENU, HINSTANCE, LPVOID, DWORD, DWORD, DWORD, VOID*);
NtUserCreateWindowEx_t NtUserCreateWindowEx_orig = nullptr;

HWND WINAPI NtUserCreateWindowEx_hook(DWORD dwExStyle, VOID* pClassName, LPCWSTR lpWindowName,
    VOID* pTitleName, DWORD dwStyle, LONG X, LONG Y, LONG nWidth, LONG nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam,
    DWORD dwShowCmd, DWORD dwFlags1, DWORD dwFlags2, VOID* qwFlags)
{
    HWND hwnd = NtUserCreateWindowEx_orig(dwExStyle, pClassName, lpWindowName, pTitleName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam,
        dwShowCmd, dwFlags1, dwFlags2, qwFlags);
    if (!hwnd) return hwnd;

    wchar_t cls[32] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    // ComboBox dropdown
    if (g_settings.ComboSection && _wcsicmp(cls, L"ComboLBox") == 0)
    {
        if (g_settings.CornerRadius > 0 && !IsResizableCombo(hwnd))
            ApplyRoundedRegion(hwnd);
        SetWindowSubclass(hwnd, ComboLBoxSubclass, 0, 0);
        SetPropW(hwnd, PROP_LB_SUB, (HANDLE)1);
        if (hWndParent && !GetPropW(hWndParent, PROP_CB_SUB)) {
            SetWindowSubclass(hWndParent, ComboParentSubclass, 1, 0);
            SetPropW(hWndParent, PROP_CB_SUB, (HANDLE)1);
        }
    }

    // TreeView
    if (g_settings.TreeSection && _wcsicmp(cls, L"SysTreeView32") == 0)
    {
        if (g_settings.RemoveTreeLines && !IsInsideExplorer(hWndParent)) {
            LONG st = GetWindowLongW(hwnd, GWL_STYLE);
            if (st & (TVS_HASLINES | TVS_LINESATROOT))
                SetWindowLongW(hwnd, GWL_STYLE, st & ~(TVS_HASLINES | TVS_LINESATROOT));
        }
        if (g_settings.ModernInsert && !GetPropW(hwnd, PROP_TV_SUB)) {
            SetWindowSubclass(hwnd, TreeViewSubclass, 2, 0);
            SetPropW(hwnd, PROP_TV_SUB, (HANDLE)1);
        }
    }

    // ListView — modern marquee (always on)
    if (_wcsicmp(cls, L"SysListView32") == 0) {
        DWORD exSt = (DWORD)SendMessageW(hwnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
        if (!(exSt & LVS_EX_DOUBLEBUFFER))
            SendMessageW(hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
    }

    return hwnd;
}

// ══════════════════════════════════════════════════════════════════════════════
// ── ACCENT STYLE MODIFICATION (based on AccentColorizer by krlvm) ────────────
// ══════════════════════════════════════════════════════════════════════════════

// Modifica bitmaps do tema via GetThemeBitmap/SetBitmapBits — mesma técnica
// do AccentColorizer. Shift-a o hue de cada pixel para o hue do accent color.

static int g_accentHue = 0;

static void AccentBitmapPixelHandler(BYTE* pBits, int size)
{
    for (int i = 0; i < size; i += 4)
    {
        int b = pBits[i], g = pBits[i+1], r = pBits[i+2];
        if (r == 0 && g == 0 && b == 0) continue;
        hsv_t hsv = rgb2hsv((double)r, (double)g, (double)b);
        hsv.h = (double)g_accentHue;
        COLORREF c = hsv2rgb_cr(hsv);
        pBits[i+2] = GetRValue(c);
        pBits[i+1] = GetGValue(c);
        pBits[i]   = GetBValue(c);
    }
}

static bool ModifyThemeBitmap(HTHEME hTheme, int part, int state, int prop)
{
    HBITMAP hbm = nullptr;
    if (FAILED(GetThemeBitmap(hTheme, part, state, prop, GBF_DIRECT, &hbm)) || !hbm)
        return false;
    BITMAP bm;
    GetObject(hbm, sizeof(bm), &bm);
    if (bm.bmBitsPixel != 32) return false;
    int size = bm.bmWidth * bm.bmHeight * 4;
    BYTE* bits = new BYTE[size];
    GetBitmapBits(hbm, size, bits);
    AccentBitmapPixelHandler(bits, size);
    SetBitmapBits(hbm, size, bits);
    delete[] bits;
    return true;
}

static void ModifyThemeStyles()
{
    // Usa o base accent para referência de hue (todas as variantes têm o mesmo hue)
    COLORREF accent = GetAccentBase();
    hsv_t accentHsv = rgb2hsv(
        (double)GetRValue(accent),
        (double)GetGValue(accent),
        (double)GetBValue(accent));
    g_accentHue = (int)accentHsv.h;

    HTHEME hTheme;
    int i, j, k;

    // Explorer::TreeView — seleção no nav pane
    hTheme = OpenThemeData(nullptr, L"Explorer::TreeView");
    if (hTheme) {
        for (i = 1; i <= 6; i++)
            for (j = 1; j <= 2; j++)
                ModifyThemeBitmap(hTheme, i, j, TMT_DIBDATA);
        for (j = 1; j <= 7; j++)
            for (k = 1; k <= 7; k++)
                ModifyThemeBitmap(hTheme, 4, j, k);
        CloseThemeData(hTheme);
    }

    // Explorer::ListView — seleção de arquivos
    hTheme = OpenThemeData(nullptr, L"Explorer::ListView");
    if (hTheme) {
        for (i = 1; i <= 10; i++)
            for (j = 1; j <= 16; j++)
                ModifyThemeBitmap(hTheme, i, j, TMT_DIBDATA);
        for (i = 8; i <= 9; i++)
            for (j = 1; j <= 7; j++)
                for (k = 1; k <= 7; k++)
                    ModifyThemeBitmap(hTheme, i, j, k);
        CloseThemeData(hTheme);
    }

    // ItemsView — grupos e headers
    hTheme = OpenThemeData(nullptr, L"ItemsView");
    if (hTheme) {
        for (i = 1; i <= 7; i++)
            ModifyThemeBitmap(hTheme, 3, i, TMT_DIBDATA);
        CloseThemeData(hTheme);
    }

    // ItemsView::ListView — seleção de arquivos
    hTheme = OpenThemeData(nullptr, L"ItemsView::ListView");
    if (hTheme) {
        for (i = 1; i <= 16; i++)
            for (j = 1; j <= 16; j++)
                ModifyThemeBitmap(hTheme, i, j, TMT_DIBDATA);
        for (i = 8; i <= 9; i++)
            for (j = 1; j <= 7; j++)
                for (k = 1; k <= 7; k++)
                    ModifyThemeBitmap(hTheme, i, j, k);
        CloseThemeData(hTheme);
    }

    // ListView
    hTheme = OpenThemeData(nullptr, L"ListView");
    if (hTheme) {
        for (i = 8; i <= 9; i++)
            for (j = 1; j <= 7; j++)
                for (k = 1; k <= 7; k++)
                    ModifyThemeBitmap(hTheme, i, j, k);
        CloseThemeData(hTheme);
    }

    // TreeView
    hTheme = OpenThemeData(nullptr, L"TreeView");
    if (hTheme) {
        for (i = 0; i <= 10; i++)
            for (j = 0; j <= 10; j++)
                for (k = 0; k <= 10; k++)
                    ModifyThemeBitmap(hTheme, i, j, k);
        CloseThemeData(hTheme);
    }

    // Navigation
    hTheme = OpenThemeData(nullptr, L"Navigation");
    if (hTheme) {
        for (i = 0; i <= 10; i++)
            for (j = 0; j <= 10; j++)
                for (k = 0; k <= 10; k++)
                    ModifyThemeBitmap(hTheme, i, j, k);
        CloseThemeData(hTheme);
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// ── DRAGDROP NORMALIZER (based on DragDropNormalizer by krlvm) ───────────────
// ══════════════════════════════════════════════════════════════════════════════

// Substitui o bitmap de overlay de drag-drop por um retângulo arredondado limpo.
// Gera o bitmap programaticamente em vez de usar array estático.

static void NormalizeDragDrop()
{
    HTHEME hTheme = OpenThemeData(nullptr, L"DragDrop");
    if (!hTheme) return;

    HBITMAP hbm = nullptr;
    if (FAILED(GetThemeBitmap(hTheme, 7, 0, TMT_DIBDATA, GBF_DIRECT, &hbm)) || !hbm)
    { CloseThemeData(hTheme); return; }

    BITMAP bm;
    GetObject(hbm, sizeof(bm), &bm);
    if (bm.bmBitsPixel != 32) { CloseThemeData(hTheme); return; }

    int w = bm.bmWidth, h = bm.bmHeight;
    int size = w * h * 4;
    BYTE* bits = new BYTE[size];

    float cr = 8.0f; // corner radius

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            BYTE* p = bits + (y * w + x) * 4;

            // Distância ao canto arredondado
            float dx = 0, dy = 0;
            if ((float)x < cr) dx = cr - (float)x;
            else if ((float)x > (float)(w - 1) - cr) dx = (float)x - ((float)(w - 1) - cr);
            if ((float)y < cr) dy = cr - (float)y;
            else if ((float)y > (float)(h - 1) - cr) dy = (float)y - ((float)(h - 1) - cr);

            float dist;
            if (dx > 0 && dy > 0)
                dist = sqrtf(dx * dx + dy * dy) - cr;
            else
                dist = -2.0f; // dentro

            if (dist < -1.5f)
            {
                // Interior — semi-transparente
                p[0] = 123; p[1] = 123; p[2] = 123; p[3] = 127;
            }
            else if (dist < 1.0f)
            {
                // Borda — anti-aliasing
                float alpha = (1.0f - (dist + 1.5f) / 2.5f);
                if (alpha < 0) alpha = 0; if (alpha > 1) alpha = 1;
                BYTE a = (BYTE)(alpha * 131.0f);
                BYTE c = (BYTE)(alpha * 127.0f);
                p[0] = c; p[1] = c; p[2] = c; p[3] = a;
            }
            else
            {
                p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
            }
        }
    }

    SetBitmapBits(hbm, size, bits);
    delete[] bits;
    CloseThemeData(hTheme);
}

// ── Settings ─────────────────────────────────────────────────────────────────

static void LoadSettings()
{
    g_settings.ComboSection    = Wh_GetIntSetting(L"ComboBoxSection");
    g_settings.CornerRadius    = Wh_GetIntSetting(L"CornerRadius");
    if (g_settings.CornerRadius < 0) g_settings.CornerRadius = 0;
    if (g_settings.CornerRadius > 20) g_settings.CornerRadius = 20;

    g_settings.AutoColors = Wh_GetIntSetting(L"AutoColors");
    if (g_settings.AutoColors) ApplyAutoColors();
    else {
        auto s1 = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"BorderColor"));   ParseHexColor(s1, g_settings.BorderClr);
        auto s2 = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"BackgroundColor")); ParseHexColor(s2, g_settings.BgColor);
        auto s3 = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"TextColor"));      ParseHexColor(s3, g_settings.TextColor);
    }

    g_settings.TreeSection     = Wh_GetIntSetting(L"TreeViewSection");
    g_settings.ModernInsert    = Wh_GetIntSetting(L"ModernInsertMark");
    g_settings.RemoveTreeLines = Wh_GetIntSetting(L"RemoveTreeLines");
    g_settings.GeneralSection    = Wh_GetIntSetting(L"GeneralSection");
    g_settings.ModernFocusRect   = Wh_GetIntSetting(L"ModernFocusRect");
    g_settings.AccentMarquee     = Wh_GetIntSetting(L"AccentMarquee");
    g_settings.AccentColorize    = Wh_GetIntSetting(L"AccentColorize");
    g_settings.AccentStyles      = Wh_GetIntSetting(L"AccentStyles");
    g_settings.NormalizeDragDrop = Wh_GetIntSetting(L"NormalizeDragDrop");
    g_settings.ExplorerSection   = Wh_GetIntSetting(L"ExplorerSection");
    g_settings.RoundedSelection  = Wh_GetIntSetting(L"RoundedSelection");
    g_settings.RemoveNavDivider  = Wh_GetIntSetting(L"RemoveNavDivider");
    g_settings.RemoveNavDividerTW = Wh_GetIntSetting(L"RemoveNavDividerTW");
    g_settings.ModernGroupHeaders = Wh_GetIntSetting(L"ModernGroupHeaders");

    RecreateBrush();
}

// ── Mod lifecycle ────────────────────────────────────────────────────────────

BOOL Wh_ModInit()
{
    LoadSettings();

    g_msgHook = SetWindowsHookExW(WH_GETMESSAGE, ThemeMsgProc, nullptr, GetCurrentThreadId());
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory);

    HMODULE hUx = GetModuleHandleW(L"uxtheme.dll");
    if (hUx) pGetThemeClass = (GetThemeClass_t)GetProcAddress(hUx, MAKEINTRESOURCEA(74));

    HMODULE hW32 = GetModuleHandleW(L"win32u.dll");
if (!hW32)
{
    if (g_msgHook) {
        UnhookWindowsHookEx(g_msgHook);
        g_msgHook = nullptr;
    }

    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }

    return FALSE;
}

auto pCreate = (NtUserCreateWindowEx_t)GetProcAddress(hW32, "NtUserCreateWindowEx");
if (!pCreate)
{
    if (g_msgHook) {
        UnhookWindowsHookEx(g_msgHook);
        g_msgHook = nullptr;
    }

    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }

    return FALSE;
}

    WindhawkUtils::SetFunctionHook(pCreate, NtUserCreateWindowEx_hook, &NtUserCreateWindowEx_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeBackground, DrawThemeBackground_hook, &DrawThemeBackground_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx, DrawThemeBackgroundEx_hook, &DrawThemeBackgroundEx_orig);
    WindhawkUtils::SetFunctionHook(DrawFocusRect, DrawFocusRect_hook, &DrawFocusRect_orig);

    ApplyAccentColorize();
    ApplyAccentMarquee();
    if (g_settings.AccentStyles)  ModifyThemeStyles();
    if (g_settings.NormalizeDragDrop) NormalizeDragDrop();
    EnumWindows(RefreshExplorerEnum, 0);

    return TRUE;
}

void Wh_ModUninit()
{
    if (g_msgHook) { UnhookWindowsHookEx(g_msgHook); g_msgHook = nullptr; }
    if (g_accentSaved) { SetSysColors(g_nAccentElems, g_accentElems, g_origAccentCols); g_accentSaved = false; }
    if (g_hotlightSaved) {
        INT elems[2] = { COLOR_HOTLIGHT, COLOR_HIGHLIGHT };
        COLORREF cols[2] = { g_origHotlight, g_origHighlight };
        SetSysColors(2, elems, cols);
        g_hotlightSaved = false;
    }
    if (g_bgBrush)    { DeleteObject(g_bgBrush); g_bgBrush = nullptr; }
    if (g_d2dFactory) { g_d2dFactory->Release();  g_d2dFactory = nullptr; }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) { *bReload = TRUE; return TRUE; }
