// ==WindhawkMod==
// @id              rounded-selection-rectangle
// @name            Rounded Selection Rectangle
// @name:pt         Retângulo de Seleção Arredondado
// @name:es         Rectángulo de Selección Redondeado
// @description     Rounded selection rectangles with a gentle fade-out
// @description:pt  Retângulos de seleção arredondados com um desaparecimento suave
// @description:es  Rectángulos de selección redondeados con un desvanecimiento suave
// @version         1.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         *
// @compilerOptions -ld2d1 -lgdi32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Rounded Selection Rectangle / Retângulo de Seleção Arredondado](https://raw.githubusercontent.com/crazyboyybs/assets/main/rounded-selection-rectangle.gif)

## English

Gives the rectangle you draw to select files rounded corners and a gentle fade
when you release the mouse. Works on the desktop, in File Explorer and in apps
that use standard Windows list controls.

- Adjust the corner roundness, border width and background opacity.
- Choose background and border colors independently, or keep **Windows default**
  for either one.
- Set the fade-out duration, or use **0** to turn it off. The mod respects the
  Windows animation setting.

In **Background color** and **Border color**, choose **Custom color** and enter a
color such as `#8B5CF6`. **Windows default** ignores the custom value and follows
the system color. Custom colors affect only the selection rectangle.

Compatible with **Win32 UI Modernizer**.

Other selection-rectangle mods, such as **Desktop Marquee Fade** and **Classic
Selection Rectangle Color**, can overlap with this mod's fade or override its
colors. Disable overlapping features if you use them together.

This mod depends on symbols published by Microsoft. If a Windows update has no
published symbols, or changes the symbols the mod uses, the mod will stop working
until compatible symbols or an updated mod are available.

### Rounded selection in Home (WinUI)

File Explorer's **Home** page uses WinUI. To round its selection rectangle,
install and enable [Windows 11 File Explorer Styler](https://windhawk.net/mods/windows-11-file-explorer-styler) in Windhawk.
In that mod's settings, add an entry under **Control styles** and set **Target**
to:

```text
Microsoft.UI.Xaml.Shapes.Rectangle#SelectionRectangle
```

Under **Styles**, add each of these four lines as a separate style:

```text
Fill:=<SolidColorBrush Color="{ThemeResource SystemAccentColor}" Opacity="0.25" />
Stroke:=<SolidColorBrush Color="{ThemeResource SystemAccentColor}" />
RadiusX:=8
RadiusY:=8
```

Save the settings. This uses the Windows accent color, a 25% opacity fill and
8-unit corner radii. To change the roundness, adjust both `RadiusX` and `RadiusY`.
These styles are configured separately from this mod's settings.

## Português

Deixa o retângulo que você arrasta para selecionar arquivos com cantos
arredondados e um desaparecimento suave ao soltar o mouse. Funciona na área de
trabalho, no Explorador de Arquivos e em aplicativos com listas padrão do Windows.

- Ajuste o arredondamento dos cantos, a espessura da borda e a opacidade do fundo.
- Escolha as cores do fundo e da borda separadamente, ou mantenha o **Padrão do
  Windows** em cada uma.
- Defina a duração do desaparecimento, ou use **0** para desativá-lo. O mod
  respeita a configuração de animações do Windows.

Em **Cor do fundo** e **Cor da borda**, escolha **Cor personalizada** e informe uma
cor como `#8B5CF6`. **Padrão do Windows** ignora o valor personalizado e acompanha a
cor do sistema. As cores personalizadas afetam somente o retângulo de seleção.

Compatível com o **Win32 UI Modernizer**.

Outros mods de retângulo de seleção, como **Desktop Marquee Fade** e **Classic
Selection Rectangle Color**, podem sobrepor o fade ou as cores deste mod.
Desative as funções equivalentes se usar esses mods juntos.

Este mod depende dos símbolos publicados pela Microsoft. Se uma atualização do
Windows não tiver símbolos publicados, ou alterar os símbolos usados pelo mod,
ele deixará de funcionar até que símbolos compatíveis ou uma atualização do mod
estejam disponíveis.

### Seleção arredondada na pasta Início (WinUI)

A pasta **Início** do Explorador de Arquivos usa WinUI. Para arredondar seu
retângulo de seleção, instale e ative o [Windows 11 File Explorer Styler](https://windhawk.net/mods/windows-11-file-explorer-styler)
no Windhawk. Nas configurações desse mod, adicione uma entrada em **Control styles**
(estilos dos controles) e preencha **Target** (alvo) com:

```text
Microsoft.UI.Xaml.Shapes.Rectangle#SelectionRectangle
```

Em **Styles** (estilos), adicione cada uma destas quatro linhas como um estilo separado:

```text
Fill:=<SolidColorBrush Color="{ThemeResource SystemAccentColor}" Opacity="0.25" />
Stroke:=<SolidColorBrush Color="{ThemeResource SystemAccentColor}" />
RadiusX:=8
RadiusY:=8
```

Salve as configurações. Isso usa a cor de destaque do Windows, fundo com 25% de
opacidade e cantos de 8 unidades. Para mudar o arredondamento, ajuste `RadiusX`
e `RadiusY`. Esses estilos são configurados separadamente das opções deste mod.

## Español

Da esquinas redondeadas y un desvanecimiento suave al rectángulo que arrastras
para seleccionar archivos. Funciona en el escritorio, en el Explorador de archivos
y en aplicaciones con listas estándar de Windows.

- Ajusta las esquinas, el grosor del borde y la opacidad del fondo.
- Elige los colores de fondo y borde por separado, o conserva el
  **Predeterminado de Windows** para cada uno.
- Ajusta la duración del desvanecimiento, o usa **0** para desactivarlo. El mod
  respeta la configuración de animaciones de Windows.

En **Color de fondo** y **Color del borde**, selecciona **Color personalizado** e
introduce un color como `#8B5CF6`. **Predeterminado de Windows** ignora el valor
personalizado y sigue el color del sistema. Los colores personalizados afectan
sólo al rectángulo de selección.

Compatible con **Win32 UI Modernizer**.

Otros mods del rectángulo de selección, como **Desktop Marquee Fade** y **Classic
Selection Rectangle Color**, pueden superponer el desvanecimiento o los colores.
Desactiva las funciones equivalentes si usas esos mods juntos.

Este mod depende de los símbolos publicados por Microsoft. Si una actualización
de Windows no tiene símbolos publicados, o cambia los símbolos que usa el mod,
dejará de funcionar hasta que haya símbolos compatibles o una actualización
del mod.

### Selección redondeada en Inicio (WinUI)

La página **Inicio** del Explorador de archivos usa WinUI. Para redondear su
rectángulo de selección, instala y activa [Windows 11 File Explorer Styler](https://windhawk.net/mods/windows-11-file-explorer-styler)
en Windhawk. En los ajustes de ese mod, añade una entrada en **Control styles**
(estilos de controles) e introduce lo siguiente en **Target** (destino):

```text
Microsoft.UI.Xaml.Shapes.Rectangle#SelectionRectangle
```

En **Styles** (estilos), añade cada una de estas cuatro líneas como un estilo separado:

```text
Fill:=<SolidColorBrush Color="{ThemeResource SystemAccentColor}" Opacity="0.25" />
Stroke:=<SolidColorBrush Color="{ThemeResource SystemAccentColor}" />
RadiusX:=8
RadiusY:=8
```

Guarda los ajustes. Esto usa el color de énfasis de Windows, un fondo con un 25%
de opacidad y esquinas de 8 unidades. Para cambiar el redondeado, ajusta `RadiusX`
y `RadiusY`. Estos estilos se configuran por separado de las opciones de este mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CornerRadius: 8
  $name: Corner radius (DIPs)
  $name:pt: Raio dos cantos (DIPs)
  $name:es: Radio de las esquinas (DIPs)
- BorderWidth: 1
  $name: Border width (DIPs)
  $name:pt: Espessura da borda (DIPs)
  $name:es: Grosor del borde (DIPs)
- FillOpacity: 25
  $name: Fill opacity (%)
  $name:pt: Opacidade do preenchimento (%)
  $name:es: Opacidad del relleno (%)
- DirectUI: true
  $name: File Explorer and file dialogs
  $name:pt: Explorador de Arquivos e diálogos de arquivos
  $name:es: Explorador de archivos y diálogos de archivos
- NativeListViews: true
  $name: Desktop and compatible apps
  $name:pt: Área de trabalho e aplicativos compatíveis
  $name:es: Escritorio y aplicaciones compatibles
- FillColor:
    - Source: windows
      $name: Color source
      $name:pt: Origem da cor
      $name:es: Origen del color
      $options:
        - windows: Windows default
        - custom: Custom color
      $options:pt:
        - windows: Padrão do Windows
        - custom: Cor personalizada
      $options:es:
        - windows: Predeterminado de Windows
        - custom: Color personalizado
    - Custom: "#0078D7"
      $name: Custom color (#RRGGBB)
      $name:pt: Cor personalizada (#RRGGBB)
      $name:es: Color personalizado (#RRGGBB)
      $description: Used only when Custom color is selected above.
      $description:pt: Usada somente quando Cor personalizada estiver selecionada acima.
      $description:es: Se usa sólo cuando Color personalizado está seleccionado arriba.
  $name: Background color
  $name:pt: Cor do fundo
  $name:es: Color de fondo
- BorderColor:
    - Source: windows
      $name: Color source
      $name:pt: Origem da cor
      $name:es: Origen del color
      $options:
        - windows: Windows default
        - custom: Custom color
      $options:pt:
        - windows: Padrão do Windows
        - custom: Cor personalizada
      $options:es:
        - windows: Predeterminado de Windows
        - custom: Color personalizado
    - Custom: "#0078D7"
      $name: Custom color (#RRGGBB)
      $name:pt: Cor personalizada (#RRGGBB)
      $name:es: Color personalizado (#RRGGBB)
      $description: Used only when Custom color is selected above.
      $description:pt: Usada somente quando Cor personalizada estiver selecionada acima.
      $description:es: Se usa sólo cuando Color personalizado está seleccionado arriba.
  $name: Border color
  $name:pt: Cor da borda
  $name:es: Color del borde
- FadeDuration: 180
  $name: Fade-out duration (ms, 0 disables)
  $name:pt: Duração do fade-out (ms, 0 desativa)
  $name:es: Duración del desvanecimiento (ms, 0 desactiva)
  $description: Fades fill and border together. Range 0-1000 ms; respects Windows animation settings.
  $description:pt: Desaparece preenchimento e borda juntos. Intervalo de 0-1000 ms; respeita as animações do Windows.
  $description:es: Desvanece relleno y borde juntos. Rango 0-1000 ms; respeta las animaciones de Windows.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <d2d1.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <iterator>
#include <memory>
#include <string_view>

using Microsoft::WRL::ComPtr;

#ifdef _WIN64
#define MEMBER_CALL __cdecl
#define MEMBER_TEXT L"__cdecl"
#else
#define MEMBER_CALL __thiscall
#define MEMBER_TEXT L"__thiscall"
#endif

struct SelectionColor {
    bool custom = false;
    COLORREF value = 0;
};

struct Settings {
    int radius;
    int border;
    int opacity;
    bool directUI;
    bool listViews;
    int fadeMs = 180;
    SelectionColor fillColor;
    SelectionColor borderColor;
};

static Settings g_settings;
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_hookApplyFailed{false};
static HANDLE g_moduleEvent;
static HANDLE g_worker;
static SRWLOCK g_symbolLock = SRWLOCK_INIT;
// These references are owned by the demand queue, including unsupported v5.
static HMODULE g_requestedListModules[4];
static bool g_duiContextSeen;
static std::atomic<bool> g_duiContextAttempted{false};
static SRWLOCK g_renderLock = SRWLOCK_INIT;

class ExclusiveLock {
    SRWLOCK* lock_;
public:
    explicit ExclusiveLock(SRWLOCK* lock) : lock_(lock) {
        AcquireSRWLockExclusive(lock_);
    }
    ~ExclusiveLock() { ReleaseSRWLockExclusive(lock_); }
};

// These objects have no DLL-detach destructors. Normal unload destroys them
// after Windhawk has drained the callbacks; process exit leaves cleanup to OS.
struct Renderer {
    ComPtr<ID2D1Factory> factory;
    ComPtr<ID2D1DCRenderTarget> target;
    ComPtr<ID2D1SolidColorBrush> brush;
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    int width = 0;
    int height = 0;

    void ResetSurface() {
        brush.Reset();
        target.Reset();
        if (dc && previous) SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        dc = nullptr;
        bitmap = nullptr;
        previous = nullptr;
        width = height = 0;
    }
    ~Renderer() { ResetSurface(); }

    bool EnsureSurface(int w, int h) {
        if (target && w <= width && h <= height) return true;
        int capacityWidth = std::max(width, (w + 63) & ~63);
        int capacityHeight = std::max(height, (h + 63) & ~63);
        if (LONGLONG(capacityWidth) * capacityHeight > 16777216) {
            capacityWidth = w;
            capacityHeight = h;
        }
        ResetSurface();
        dc = CreateCompatibleDC(nullptr);
        if (!dc) return false;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = capacityWidth;
        info.bmiHeader.biHeight = -capacityHeight;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* pixels = nullptr;
        bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
        if (!bitmap) { ResetSurface(); return false; }
        previous = SelectObject(dc, bitmap);
        if (!previous || previous == HGDI_ERROR) {
            previous = nullptr;
            ResetSurface();
            return false;
        }
        const auto properties = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_SOFTWARE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                             D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
        HRESULT hr = factory->CreateDCRenderTarget(&properties, &target);
        if (SUCCEEDED(hr)) {
            hr = target->CreateSolidColorBrush(D2D1::ColorF(0, 0.f), &brush);
        }
        if (FAILED(hr)) { ResetSurface(); return false; }
        width = capacityWidth;
        height = capacityHeight;
        return true;
    }
};
static Renderer* g_renderer;

using ShutdownQuery = BOOLEAN(NTAPI*)();
static ShutdownQuery g_shutdownInProgress;
static constexpr LONGLONG kRetainedSurfacePixels = 1024 * 1024; // 4 MiB, excluding D2D resources.

static void TrimRenderSurface() {
    // Never acquire a render lock or release D2D during process detach. The
    // ordinary mod-unload path releases everything explicitly in Wh_ModUninit.
    if (g_unloading.load() || !g_shutdownInProgress || g_shutdownInProgress()) return;
    ExclusiveLock lock(&g_renderLock);
    if (g_renderer && LONGLONG(g_renderer->width) * g_renderer->height > kRetainedSurfacePixels)
        g_renderer->ResetSurface();
}

static bool HighContrast() {
    HIGHCONTRASTW state{};
    state.cbSize = sizeof(state);
    return !SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(state), &state, 0) ||
           (state.dwFlags & HCF_HIGHCONTRASTON);
}

static UINT PaintDpi(HDC dc) {
    HWND hwnd = WindowFromDC(dc);
    if (!hwnd) hwnd = GetCapture();
    if (hwnd) {
        const UINT dpi = GetDpiForWindow(hwnd);
        if (dpi) return dpi;
    }
    const int dpi = GetDeviceCaps(dc, LOGPIXELSX);
    return dpi > 0 ? dpi : 96;
}

static bool ParseColor(PCWSTR text, COLORREF* value) {
    if (!text || !value) return false;
    std::wstring_view hex(text);
    if (hex.starts_with(L"#")) hex.remove_prefix(1);
    if (hex.size() != 6) return false;
    unsigned rgb = 0;
    for (wchar_t digit : hex) {
        unsigned nibble;
        if (digit >= L'0' && digit <= L'9') nibble = digit - L'0';
        else if (digit >= L'a' && digit <= L'f') nibble = digit - L'a' + 10;
        else if (digit >= L'A' && digit <= L'F') nibble = digit - L'A' + 10;
        else return false;
        rgb = (rgb << 4) | nibble;
    }
    *value = RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255);
    return true;
}

static SelectionColor LoadColor(PCWSTR section) {
    auto source = WindhawkUtils::StringSetting::make(L"%s.Source", section);
    // Windows mode ignores the custom value completely, including invalid text.
    if (_wcsicmp(source, L"custom") != 0) return {};
    auto text = WindhawkUtils::StringSetting::make(L"%s.Custom", section);
    SelectionColor color;
    color.custom = ParseColor(text, &color.value);
    if (!color.custom) Wh_Log(L"Invalid %s.Custom color; using Windows default", section);
    return color;
}

static D2D1_COLOR_F SystemColor(int index, float alpha) {
    COLORREF color = GetSysColor(index);
    return D2D1::ColorF(GetRValue(color) / 255.f, GetGValue(color) / 255.f,
                       GetBValue(color) / 255.f, alpha);
}

static D2D1_COLOR_F RenderColor(const SelectionColor& color, int systemIndex, float alpha) {
    if (!color.custom) return SystemColor(systemIndex, alpha);
    // Custom colors go straight to the D2D brush, without querying system colors.
    return D2D1::ColorF(GetRValue(color.value) / 255.f, GetGValue(color.value) / 255.f,
                       GetBValue(color.value) / 255.f, alpha);
}

// Rendering is transactional: EndDraw only touches our transparent DIB.
// The host is updated once, after all D2D operations have succeeded.
static bool PaintMarquee(HDC dc, const RECT& bounds, const RECT* clip,
                          bool allowTranslation = false, float opacity = 1.f, UINT dpi = 0) {
    if (opacity <= 0.f) return true;
    if (g_unloading.load() || HighContrast() || !dc) return false;
    if (GetMapMode(dc) != MM_TEXT || (GetLayout(dc) & LAYOUT_RTL)) return false;
    if (GetGraphicsMode(dc) == GM_ADVANCED) {
        XFORM x{};
        if (!GetWorldTransform(dc, &x) || x.eM11 != 1 || x.eM22 != 1 ||
            x.eM12 != 0 || x.eM21 != 0 ||
            (!allowTranslation && (x.eDx != 0 || x.eDy != 0))) return false;
    }
    if (bounds.right <= bounds.left || bounds.bottom <= bounds.top) return true;
    RECT visible = bounds;
    if (clip && !IntersectRect(&visible, &visible, clip)) return true;
    RECT dcClip{};
    int region = GetClipBox(dc, &dcClip);
    if (region == ERROR) return false;
    if (region == NULLREGION || !IntersectRect(&visible, &visible, &dcClip)) return true;
    const LONGLONG w = LONGLONG(visible.right) - visible.left;
    const LONGLONG h = LONGLONG(visible.bottom) - visible.top;
    if (w > 16384 || h > 16384 || w * h > 16777216) return false;
    const float scale = (dpi ? dpi : PaintDpi(dc)) / 96.f;
    ExclusiveLock lock(&g_renderLock);
    if (g_unloading.load() || !g_renderer) return false;
    Renderer& r = *g_renderer;
    if (!r.EnsureSurface(static_cast<int>(w), static_cast<int>(h))) return false;
    RECT local{0, 0, static_cast<LONG>(w), static_cast<LONG>(h)};
    HRESULT hr = r.target->BindDC(r.dc, &local);
    if (FAILED(hr)) return false;

    const float fullWidth = static_cast<float>(LONGLONG(bounds.right) - bounds.left);
    const float fullHeight = static_cast<float>(LONGLONG(bounds.bottom) - bounds.top);
    const float halfSize = std::min(fullWidth, fullHeight) / 2.f;
    const float stroke = std::min(g_settings.border * scale, halfSize);
    const float radius = std::min(g_settings.radius * scale, halfSize);
    const auto outer = D2D1::RectF(
        float(LONGLONG(bounds.left) - visible.left),
        float(LONGLONG(bounds.top) - visible.top),
        float(LONGLONG(bounds.right) - visible.left),
        float(LONGLONG(bounds.bottom) - visible.top));
    const auto inset = D2D1::RectF(outer.left + stroke / 2, outer.top + stroke / 2,
                                 outer.right - stroke / 2, outer.bottom - stroke / 2);
    const float insetRadius = std::max(0.f, radius - stroke / 2);
    r.target->BeginDraw();
    r.target->SetTransform(D2D1::Matrix3x2F::Identity());
    r.target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    r.target->Clear(D2D1::ColorF(0, 0.f));
    r.brush->SetColor(RenderColor(g_settings.fillColor, COLOR_HIGHLIGHT, g_settings.opacity / 100.f));
    r.target->FillRoundedRectangle(D2D1::RoundedRect(outer, radius, radius), r.brush.Get());
    if (stroke > 0) {
        r.brush->SetColor(RenderColor(g_settings.borderColor, COLOR_HOTLIGHT, 1.f));
        r.target->DrawRoundedRectangle(D2D1::RoundedRect(inset, insetRadius, insetRadius),
                                      r.brush.Get(), stroke);
    }
    hr = r.target->EndDraw();
    if (FAILED(hr)) {
        r.ResetSurface();
        return false;
    }
    BLENDFUNCTION blend{AC_SRC_OVER, 0,
        static_cast<BYTE>(std::clamp(opacity, 0.f, 1.f) * 255.f + .5f), AC_SRC_ALPHA};
    return GdiAlphaBlend(dc, visible.left, visible.top, static_cast<int>(w),
                         static_cast<int>(h), r.dc, 0, 0, static_cast<int>(w),
                         static_cast<int>(h), blend) != FALSE;
}

namespace Fade {
enum { Free, Reserved, Busy };
struct Owner {
    std::atomic<unsigned> state{Free};
    std::atomic<DWORD> thread{0};
    HANDLE handle = nullptr; // Protected by ownersLock, including unload scans.
};
static SRWLOCK ownersLock = SRWLOCK_INIT;
static LARGE_INTEGER frequency{};
static UINT cleanupMessage;
static std::atomic<UINT_PTR> timerSerial{1};
static std::atomic<ULONGLONG> slotSerial{1};
static double Now() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return frequency.QuadPart ? now.QuadPart * 1000.0 / frequency.QuadPart : double(GetTickCount64());
}
static float Opacity(double start, double now, int duration) {
    double progress = duration > 0 ? std::clamp((now - start) / duration, 0., 1.) : 1.;
    return float(1. - progress * progress * progress);
}
static bool Allowed() {
    BOOL enabled = FALSE;
    return !g_unloading.load() && g_settings.fadeMs > 0 && !HighContrast() &&
        SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0) && enabled;
}
static bool Owned(const Owner& owner) {
    return owner.state.load(std::memory_order_acquire) == Busy &&
           owner.thread.load() == GetCurrentThreadId();
}
static bool Reserve(Owner& owner) {
    unsigned expected = Free;
    if (!owner.state.compare_exchange_strong(expected, Reserved)) return false;
    ExclusiveLock lock(&ownersLock);
    if (g_unloading.load()) { owner.state.store(Free); return false; }
    owner.handle = OpenThread(SYNCHRONIZE, FALSE, GetCurrentThreadId());
    if (!owner.handle) { owner.state.store(Free); return false; }
    owner.thread.store(GetCurrentThreadId());
    return true;
}
static void Release(Owner& owner) {
    ExclusiveLock lock(&ownersLock);
    owner.state.store(Reserved);
    if (owner.handle) CloseHandle(owner.handle);
    owner.handle = nullptr;
    owner.thread.store(0);
    owner.state.store(Free, std::memory_order_release);
}
static void ReclaimExited(Owner& owner) {
    ExclusiveLock lock(&ownersLock);
    if (owner.state.load() != Free && owner.handle &&
        WaitForSingleObject(owner.handle, 0) == WAIT_OBJECT_0) {
        CloseHandle(owner.handle);
        owner.handle = nullptr;
        owner.thread.store(0);
        owner.state.store(Free);
    }
}

struct WinSlot {
    Owner owner;
    std::atomic<HWND> hwnd{nullptr};
    RECT rect{};
    UINT_PTR timer = 0;
    ULONGLONG generation = 0;
    UINT dpi = 96;
    double start = 0;
    bool fading = false;
    bool cancel = false;
};
static WinSlot windows[16];
// The list in WM_PAINT/WM_PRINTCLIENT, and the DC its caller passed in wParam.
struct PaintScope {
    WinSlot* slot;
    HDC dc;
};
static thread_local PaintScope painting;
struct DragScope {
    DragScope* previous;
    HWND hwnd = nullptr;
    WinSlot* slot = nullptr;
    ULONGLONG generation = 0;
    bool cancelled = false;
};
static thread_local DragScope* dragging;
static LRESULT CALLBACK WinSubclass(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

static void StopWin(WinSlot& slot, bool repaint = true) {
    if (!Owned(slot.owner)) return;
    HWND hwnd = slot.hwnd.load();
    if (slot.timer) KillTimer(hwnd, slot.timer);
    slot.timer = 0;
    slot.fading = false;
    if (repaint) InvalidateRect(hwnd, &slot.rect, FALSE);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, WinSubclass);
    slot.hwnd.store(nullptr);
    Release(slot.owner);
    TrimRenderSurface();
}
static WinSlot* ObserveWin(HWND hwnd, const RECT& rect) {
    if (!dragging || g_unloading.load() || !g_settings.fadeMs || !hwnd || !cleanupMessage) return nullptr;
    wchar_t name[32];
    if (GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId() ||
        !GetClassNameW(hwnd, name, std::size(name)) || _wcsicmp(name, L"SysListView32")) return nullptr;
    WinSlot* slot = nullptr;
    for (auto& candidate : windows)
        if (Owned(candidate.owner) && candidate.hwnd.load() == hwnd) slot = &candidate;
    if (!slot) {
        for (auto& candidate : windows) {
            if (!Reserve(candidate.owner)) continue;
            slot = &candidate;
            slot->hwnd.store(hwnd);
            slot->rect = rect;
            slot->timer = 0;
            slot->generation = slotSerial.fetch_add(1);
            slot->fading = slot->cancel = false;
            if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, WinSubclass,
                    reinterpret_cast<DWORD_PTR>(slot))) {
                slot->hwnd.store(nullptr);
                Release(slot->owner);
                return nullptr;
            }
            slot->owner.state.store(Busy, std::memory_order_release);
            break;
        }
    }
    if (slot) {
        slot->rect = rect;
        slot->dpi = GetDpiForWindow(hwnd);
        dragging->slot = slot;
        dragging->hwnd = hwnd;
        dragging->generation = slot->generation;
    }
    return slot;
}
static void BeginWin(DragScope& drag) {
    WinSlot* slot = drag.slot;
    if (!slot || !Owned(slot->owner) || slot->hwnd.load() != drag.hwnd ||
        slot->generation != drag.generation || slot->fading) return;
    if (drag.cancelled || slot->cancel || !Allowed() || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        StopWin(*slot);
        return;
    }
    slot->start = Now();
    // Each run has a fresh ID: queued ticks from a killed timer cannot end a new fade.
    UINT_PTR id = reinterpret_cast<UINT_PTR>(windows) + timerSerial.fetch_add(1);
    slot->timer = SetTimer(drag.hwnd, id, 10, nullptr);
    if (!slot->timer) { StopWin(*slot); return; }
    slot->fading = true;
    InvalidateRect(drag.hwnd, &slot->rect, FALSE);
}
// Paints into a buffer that is about to be presented to `target`. Only the
// list's own paint DC qualifies: buffers opened by custom-draw handlers target
// its memory DC, and child windows have their own paint DCs.
static void PaintWin(HDC target, HDC dc, const RECT* clip) {
    WinSlot* slot = painting.slot;
    if (!slot || !Owned(slot->owner) || !slot->fading || g_unloading.load()) return;
    if (painting.dc ? target != painting.dc : WindowFromDC(target) != slot->hwnd.load()) return;
    PaintMarquee(dc, slot->rect, clip, false, Opacity(slot->start, Now(), g_settings.fadeMs), slot->dpi);
}
static LRESULT CALLBACK WinSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR data) {
    auto& slot = *reinterpret_cast<WinSlot*>(data);
    if (!Owned(slot.owner) || slot.hwnd.load() != hwnd) return DefSubclassProc(hwnd, msg, wp, lp);
    if (msg == cleanupMessage || msg == WM_NCDESTROY) {
        if (dragging && dragging->slot == &slot) dragging->cancelled = true;
        StopWin(slot, msg != WM_NCDESTROY);
        return msg == cleanupMessage ? 0 : DefSubclassProc(hwnd, msg, wp, lp);
    }
    if (msg == WM_TIMER && wp == slot.timer && slot.timer) {
        if (g_unloading.load() || Opacity(slot.start, Now(), g_settings.fadeMs) <= 0) StopWin(slot);
        else InvalidateRect(hwnd, &slot.rect, FALSE);
        return 0;
    }
    bool cancel = msg == WM_CANCELMODE || msg == WM_SIZE || msg == WM_MOUSEWHEEL ||
        msg == WM_MOUSEHWHEEL || msg == WM_VSCROLL || msg == WM_HSCROLL ||
        msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_KEYDOWN ||
        msg == WM_THEMECHANGED || msg == WM_SETTINGCHANGE || msg == WM_STYLECHANGED ||
        msg == WM_DPICHANGED_BEFOREPARENT || msg == WM_DPICHANGED_AFTERPARENT ||
        (msg == WM_CAPTURECHANGED && lp != 0);
    if (cancel) {
        if (dragging && dragging->slot == &slot) dragging->cancelled = true;
        if (slot.fading) { StopWin(slot); return DefSubclassProc(hwnd, msg, wp, lp); }
        slot.cancel = true;
    }
    if (msg == WM_PAINT || msg == WM_PRINTCLIENT) {
        PaintScope previous = painting;
        painting = {&slot, reinterpret_cast<HDC>(wp)};
        LRESULT result = DefSubclassProc(hwnd, msg, wp, lp);
        painting = previous;
        return result;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// GMA_ACTION, measured in duser.dll on x64/x86. The callback receives
// GMA_ACTIONINFO; its pvData/fFinished offsets are decoded in Tick below.
struct Action {
    DWORD size;
    float delay, duration, period;
    int repeat;
    DWORD pause;
    void(WINAPI* callback)(void*);
    void* data;
};
static_assert(sizeof(Action) == (sizeof(void*) == 8 ? 0x28 : 0x20));
using CreateAction = HANDLE(WINAPI*)(const Action*);
using Delete = BOOL(WINAPI*)(HANDLE);
using Invalidate = BOOL(WINAPI*)(HANDLE);
using Node = HANDLE(MEMBER_CALL*)(void*);
using Visible = HRESULT(MEMBER_CALL*)(void*, bool);
static CreateAction createAction;
static Delete deleteOriginal;
static Invalidate invalidate;
static Node getNode;
static Visible visibleOriginal;
static HMODULE duserModule;
static std::atomic<bool> duiReady{false};
struct DuiSlot {
    Owner owner;
    void* element = nullptr;
    HANDLE node = nullptr;
    HANDLE action = nullptr;
    double start = 0;
    bool canceling = false;
    bool deleting = false;
    bool hideRequested = false;
    bool hidden = false;
    UINT dpi = 0;
};
static DuiSlot elements[16];
static thread_local DuiSlot* fadingElement;
static thread_local DuiSlot* endingElement;
static void WINAPI Tick(void*);
static void Hide(DuiSlot& slot) {
    if (slot.element && slot.hideRequested && !slot.hidden) {
        slot.hidden = true;
        visibleOriginal(slot.element, false);
    }
}
static void CancelDui(bool hide) {
    DuiSlot* slot = fadingElement;
    if (!slot || !Owned(slot->owner)) { fadingElement = nullptr; return; }
    if (slot->deleting) return;
    slot->canceling = true;
    if (!hide) slot->hideRequested = false;
    slot->deleting = true;
    BOOL deleted = !slot->action || deleteOriginal(slot->action);
    slot->deleting = false;
    if (!deleted) {
        // Keep the slot alive until its final callback if cancellation failed.
        if (hide) Hide(*slot);
        return;
    }
    slot->action = nullptr;
    // Retire before SetVisible: native property listeners can re-enter StartMarquee.
    fadingElement = nullptr;
    slot->owner.state.store(Reserved);
    if (hide) Hide(*slot);
    Release(slot->owner);
    TrimRenderSurface();
}
static DuiSlot* BeginDui(void* element, UINT dpi = 0) {
    if (!duiReady.load() || !Allowed() || !getNode || !visibleOriginal || !createAction || !deleteOriginal) return nullptr;
    CancelDui(true);
    if (fadingElement) return nullptr;
    HANDLE node = getNode(element);
    if (!node) return nullptr;
    for (auto& slot : elements) {
        if (!Reserve(slot.owner)) continue;
        slot.element = element;
        slot.dpi = dpi;
        slot.node = node;
        slot.start = Now();
        slot.canceling = slot.deleting = slot.hideRequested = slot.hidden = false;
        slot.action = nullptr;
        fadingElement = &slot;
        slot.owner.state.store(Busy, std::memory_order_release);
        Action action{sizeof(Action), 0.f, g_settings.fadeMs / 1000.f, 0.f, 0, 8, Tick, &slot};
        slot.action = createAction(&action);
        if (!slot.action) {
            fadingElement = nullptr;
            Release(slot.owner);
            return nullptr;
        }
        return &slot;
    }
    return nullptr;
}
static void WINAPI Tick(void* info) {
    // GMA_ACTIONINFO: pvData is +8/+4 and fFinished is +0x20/+0x18 (x64/x86).
    auto* bytes = static_cast<BYTE*>(info);
    auto* slot = *reinterpret_cast<DuiSlot**>(bytes + sizeof(void*));
    bool finished = *reinterpret_cast<BOOL*>(bytes + (sizeof(void*) == 8 ? 0x20 : 0x18)) != FALSE;
    if (!slot || !Owned(slot->owner)) return;
    if (slot->deleting) return;
    if (slot->canceling && !finished) return;
    if (finished) {
        slot->action = nullptr; // Natural completion already deletes the native handle.
        slot->canceling = true;
        if (fadingElement == slot) fadingElement = nullptr;
        slot->owner.state.store(Reserved);
        Hide(*slot);
        Release(slot->owner);
        TrimRenderSurface();
    } else if (g_unloading.load()) {
        Hide(*slot);
    } else if (slot->hideRequested && slot->node) {
        invalidate(slot->node);
    }
}
static float DuiOpacity(void* element) {
    DuiSlot* slot = fadingElement;
    return slot && Owned(slot->owner) && slot->element == element && slot->hideRequested && !slot->canceling ?
        Opacity(slot->start, Now(), g_settings.fadeMs) : 1.f;
}
static bool IsFading(void* element) {
    DuiSlot* slot = fadingElement;
    return slot && Owned(slot->owner) && slot->element == element;
}
static UINT DuiDpi(void* element) {
    return IsFading(element) ? fadingElement->dpi : 0;
}
static HRESULT MEMBER_CALL VisibleHook(void* element, bool visible) {
    DuiSlot* slot = endingElement;
    if (!visible && slot && Owned(slot->owner) && slot->element == element &&
        !slot->canceling && !g_unloading.load()) {
        slot->hideRequested = true;
        return S_OK;
    }
    if (IsFading(element) && !fadingElement->canceling) CancelDui(false);
    return visibleOriginal(element, visible);
}
static void DestroyDui(void* element) {
    if (IsFading(element)) {
        fadingElement->element = nullptr;
        fadingElement->node = nullptr;
        CancelDui(false);
    }
}
static void AfterDeleteHandle() {
    if (fadingElement && Owned(fadingElement->owner)) {
        DWORD error = GetLastError();
        SetLastError(0);
        createAction(nullptr);
        // DU_E_NOCONTEXT: the owner thread outlived its DirectUser context.
        if (GetLastError() == 0x80040014) {
            // Context destruction removes actions without delivering a final tick.
            for (auto& slot : elements) if (Owned(slot.owner)) Release(slot.owner);
            fadingElement = endingElement = nullptr;
            TrimRenderSurface();
        }
        SetLastError(error);
    }
}
static BOOL WINAPI DeleteHook(HANDLE handle) {
    BOOL result = deleteOriginal(handle);
    AfterDeleteHandle();
    return result;
}
static void Drain() {
    for (;;) {
        bool pending = false;
        for (auto& slot : windows) {
            ReclaimExited(slot.owner);
            if (slot.owner.state.load() == Busy) {
                HWND hwnd = slot.hwnd.load();
                if (hwnd) SendMessageTimeoutW(hwnd, cleanupMessage, 0, 0,
                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, nullptr);
            }
            pending |= slot.owner.state.load() != Free;
        }
        for (auto& slot : elements) {
            ReclaimExited(slot.owner);
            pending |= slot.owner.state.load() != Free;
        }
        if (!pending) break;
        Sleep(10);
    }
}
} // namespace Fade

// Win32: _PaintMarquee's first IntersectRect receives the repaint clip and
// full marquee bounds. Scope that call instead of reading CLV private fields.
using ListPaint = void(MEMBER_CALL*)(void*, HDC, void*, const RECT*);
using ListDrag = void(MEMBER_CALL*)(void*, int, int);
struct ListModule {
    HMODULE module = nullptr;
    ListPaint original = nullptr;
    ListDrag dragOriginal = nullptr;
    bool installed = false;
    bool fadeInstalled = false;
    std::atomic<bool> ready{false};
    std::atomic<bool> fadeReady{false};
};
static ListModule g_listModules[4];
struct ListPaintScope {
    HDC dc;
    const RECT* clip;
    bool pending = true;
    bool trackFade = false;
};
static thread_local ListPaintScope* t_listPaint;
static decltype(&IntersectRect) IntersectRect_Original;

static BOOL WINAPI IntersectRect_Hook(LPRECT result, LPCRECT first, LPCRECT second) {
    ListPaintScope* scope = t_listPaint;
    // Consume before painting: D2D/GDI can call IntersectRect recursively.
    if (scope && scope->pending && first == scope->clip && result && second) {
        scope->pending = false;
        RECT full = *second;
        BOOL intersects = IntersectRect_Original(result, first, second);
        if (intersects && PaintMarquee(scope->dc, full, result)) {
            if (scope->trackFade) Fade::ObserveWin(GetCapture(), full);
            SetRectEmpty(result);
            return FALSE;
        }
        return intersects;
    }
    return IntersectRect_Original(result, first, second);
}

template<size_t Index>
static void MEMBER_CALL ListPaint_Hook(void* self, HDC dc, void* unknown, const RECT* clip) {
    ListPaintScope scope{dc, clip, true, g_listModules[Index].fadeReady.load()};
    ListPaintScope* previous = t_listPaint;
    t_listPaint = !g_unloading.load() && g_listModules[Index].ready.load() ? &scope : nullptr;
    g_listModules[Index].original(self, dc, unknown, clip);
    t_listPaint = previous;
    // Without DragSelect there is no reliable end notification. Bound the
    // cache after each paint on that fallback path instead of retaining a large DIB.
    if (!scope.pending && !g_listModules[Index].dragOriginal) TrimRenderSurface();
}
// The alpha marquee is drawn only in double-buffered lists, which present
// each work area with EndBufferedPaint. Compose the fade into that buffer
// before it reaches the screen.
static decltype(&EndBufferedPaint) EndBufferedPaint_Original;
static decltype(&GetBufferedPaintDC) getBufferedPaintDC;
static decltype(&GetBufferedPaintTargetDC) getBufferedPaintTargetDC;
static decltype(&GetBufferedPaintTargetRect) getBufferedPaintTargetRect;

static HRESULT WINAPI EndBufferedPaint_Hook(HPAINTBUFFER buffer, BOOL update) {
    RECT target;
    if (Fade::painting.slot && buffer && update &&
        SUCCEEDED(getBufferedPaintTargetRect(buffer, &target))) {
        Fade::PaintWin(getBufferedPaintTargetDC(buffer), getBufferedPaintDC(buffer), &target);
    }
    return EndBufferedPaint_Original(buffer, update);
}

template<size_t Index>
static void MEMBER_CALL ListDrag_Hook(void* self, int x, int y) {
    if (g_unloading.load() || !g_listModules[Index].fadeReady.load() || !Fade::Allowed()) {
        g_listModules[Index].dragOriginal(self, x, y);
        TrimRenderSurface();
        return;
    }
    for (auto& slot : Fade::windows) if (Fade::Owned(slot.owner) && slot.fading) Fade::StopWin(slot);
    Fade::DragScope scope{Fade::dragging};
    Fade::dragging = &scope;
    g_listModules[Index].dragOriginal(self, x, y);
    Fade::dragging = scope.previous;
    Fade::BeginWin(scope);
    // No slot/action exists when animation is disabled, unsupported or canceled.
    if (!scope.slot || !Fade::Owned(scope.slot->owner) || !scope.slot->fading)
        TrimRenderSurface();
}

// DirectUI paints the same element through background-only and border paths.
// Own its visual at the outer Paint call, while retaining native output bounds.
using Start = void(MEMBER_CALL*)(void*, POINT, void*, int, int, int, void*);
using End = void(MEMBER_CALL*)(void*);
using ElementPaint = void(MEMBER_CALL*)(void*, HDC, const RECT*, const RECT*, RECT*, RECT*);
static Start Start_Original;
static End End_Original;
static ElementPaint ElementPaint_Original;
static End Destroy_Original;
static std::atomic<bool> g_duiReady{false};
static thread_local void* t_selector;
static thread_local bool t_selectorPainted;
static thread_local UINT t_selectorDpi;

static void MEMBER_CALL Start_Hook(void* self, POINT point, void* element,
                                   int initial, int button, int direction, void* item) {
    Fade::CancelDui(true);
    if (!g_unloading.load() && g_duiReady.load()) {
        t_selector = self;
        t_selectorPainted = false;
        t_selectorDpi = 0;
    }
    Start_Original(self, point, element, initial, button, direction, item);
}
static void MEMBER_CALL End_Hook(void* self) {
    Fade::DuiSlot* previous = Fade::endingElement;
    Fade::DuiSlot* fade = nullptr;
    if (!g_unloading.load() && self == t_selector && t_selectorPainted) {
        fade = Fade::IsFading(self) ? Fade::fadingElement : Fade::BeginDui(self, t_selectorDpi);
    }
    Fade::endingElement = fade;
    End_Original(self);
    Fade::endingElement = previous;
    if (fade && Fade::Owned(fade->owner) && !fade->hideRequested) Fade::CancelDui(false);
    if (!Fade::IsFading(self)) {
        if (t_selector == self) t_selector = nullptr;
        TrimRenderSurface();
    }
}
static void MEMBER_CALL Destroy_Hook(void* self) {
    Fade::DestroyDui(self);
    if (t_selector == self) {
        t_selector = nullptr;
        TrimRenderSurface();
    }
    Destroy_Original(self);
}
static void MEMBER_CALL ElementPaint_Hook(void* self, HDC dc, const RECT* rect,
                                         const RECT* clip, RECT* exclude, RECT* target) {
    if (!g_unloading.load() && g_duiReady.load() &&
        (self == t_selector || Fade::IsFading(self)) && rect && dc) {
        int saved = SaveDC(dc);
        if (saved) {
            UINT dpi = Fade::DuiDpi(self);
            if (!dpi) dpi = PaintDpi(dc);
            // DUser can supply a translated DC with bounds in logical units.
            // AlphaBlend applies that destination transform to logical bounds.
            if (PaintMarquee(dc, *rect, clip, true, Fade::DuiOpacity(self), dpi) &&
                IntersectClipRect(dc, 0, 0, 0, 0) != ERROR) {
                if (self == t_selector) {
                    t_selectorPainted = true;
                    t_selectorDpi = dpi;
                }
                // Preserve native geometry/lifetime work. An empty destination
                // clip suppresses border, background and chained GDI painters.
                ElementPaint_Original(self, dc, rect, clip, exclude, target);
                RestoreDC(dc, saved);
                return;
            }
            RestoreDC(dc, saved);
        }
    }
    ElementPaint_Original(self, dc, rect, clip, exclude, target);
}

static HMODULE g_duiModule;
static HMODULE g_frameModule;
static bool g_duiAttempted;
static bool g_frameAttempted;
static bool g_duiExportsInstalled;
static bool g_frameInstalled;
static bool g_duiFadeInstalled;

template<typename T>
static bool HookExport(HMODULE module, const char* name64, const char* name32, T hook, T* original) {
#ifdef _WIN64
    const char* name = name64;
#else
    const char* name = name32;
#endif
    auto address = reinterpret_cast<T>(GetProcAddress(module, name));
    if (!address || !WindhawkUtils::SetFunctionHook(address, hook, original)) {
        Wh_Log(L"DirectUI export unavailable or hook failed: %S", name);
        return false;
    }
    return true;
}

static void InstallDirectUI() {
    if (g_duiModule && !g_duiAttempted) {
        g_duiAttempted = true;
        // All callbacks tolerate partial installation; readiness requires the full set.
        bool ok = HookExport(g_duiModule,
            "?Paint@Element@DirectUI@@UEAAXPEAUHDC__@@PEBUtagRECT@@1PEAU4@2@Z",
            "?Paint@Element@DirectUI@@UAEXPAUHDC__@@PBUtagRECT@@1PAU4@2@Z",
            ElementPaint_Hook, &ElementPaint_Original);
        ok = HookExport(g_duiModule, "?OnDestroy@Element@DirectUI@@UEAAXXZ",
            "?OnDestroy@Element@DirectUI@@UAEXXZ", Destroy_Hook, &Destroy_Original) && ok;
        g_duiExportsInstalled = ok;
        if (g_settings.fadeMs > 0 &&
            GetModuleHandleExW(0, L"duser.dll", &Fade::duserModule)) {
            Fade::createAction = reinterpret_cast<Fade::CreateAction>(GetProcAddress(Fade::duserModule, "CreateAction"));
            Fade::invalidate = reinterpret_cast<Fade::Invalidate>(GetProcAddress(Fade::duserModule, "InvalidateGadget"));
            auto remove = reinterpret_cast<Fade::Delete>(GetProcAddress(Fade::duserModule, "DeleteHandle"));
#ifdef _WIN64
            Fade::getNode = reinterpret_cast<Fade::Node>(GetProcAddress(g_duiModule,
                "?GetDisplayNode@Element@DirectUI@@QEAAPEAUHGADGET__@@XZ"));
#else
            Fade::getNode = reinterpret_cast<Fade::Node>(GetProcAddress(g_duiModule,
                "?GetDisplayNode@Element@DirectUI@@QAEPAUHGADGET__@@XZ"));
#endif
            bool visibility = HookExport(g_duiModule,
                "?SetVisible@Element@DirectUI@@QEAAJ_N@Z",
                "?SetVisible@Element@DirectUI@@QAEJ_N@Z", Fade::VisibleHook, &Fade::visibleOriginal);
            bool deletion = remove && WindhawkUtils::SetFunctionHook(remove, Fade::DeleteHook, &Fade::deleteOriginal);
            g_duiFadeInstalled = visibility && deletion && Fade::createAction && Fade::invalidate && Fade::getNode;
            if (!g_duiFadeInstalled) Wh_Log(L"DirectUser fade APIs unavailable; marquee hides immediately");
        }
    }
    if (g_frameModule && !g_frameAttempted) {
        g_frameAttempted = true;
        WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
            {{L"public: void " MEMBER_TEXT L" UIMarqueeSelector::StartMarquee(struct tagPOINT,class DirectUI::Element *,enum INITIALSELECTION,enum CLICKBUTTON,enum __MIDL___MIDL_itf_itemsview_0000_0004_0001,class UIItem *)"},
             &Start_Original, Start_Hook, true},
            {{L"public: void " MEMBER_TEXT L" UIMarqueeSelector::EndMarquee(void)"},
             &End_Original, End_Hook, true},
        };
        g_frameInstalled = WindhawkUtils::HookSymbols(g_frameModule, explorerframe_dll_hooks,
            std::size(explorerframe_dll_hooks)) && Start_Original && End_Original;
        if (!g_frameInstalled) {
            Wh_Log(L"Explorer marquee symbols unavailable; DirectUI remains native");
        }
    }
}

static HMODULE g_uxthemeModule;
static bool g_bufferedPaintAttempted;
static bool g_bufferedPaintInstalled;

static bool InstallBufferedPaint() {
    if (g_bufferedPaintAttempted) return g_bufferedPaintInstalled;
    g_bufferedPaintAttempted = true;
    if (!GetModuleHandleExW(0, L"uxtheme.dll", &g_uxthemeModule)) return false;
    getBufferedPaintDC = reinterpret_cast<decltype(getBufferedPaintDC)>(
        GetProcAddress(g_uxthemeModule, "GetBufferedPaintDC"));
    getBufferedPaintTargetDC = reinterpret_cast<decltype(getBufferedPaintTargetDC)>(
        GetProcAddress(g_uxthemeModule, "GetBufferedPaintTargetDC"));
    getBufferedPaintTargetRect = reinterpret_cast<decltype(getBufferedPaintTargetRect)>(
        GetProcAddress(g_uxthemeModule, "GetBufferedPaintTargetRect"));
    auto end = reinterpret_cast<decltype(&EndBufferedPaint)>(
        GetProcAddress(g_uxthemeModule, "EndBufferedPaint"));
    g_bufferedPaintInstalled = getBufferedPaintDC && getBufferedPaintTargetDC &&
        getBufferedPaintTargetRect && end &&
        WindhawkUtils::SetFunctionHook(end, EndBufferedPaint_Hook, &EndBufferedPaint_Original);
    return g_bufferedPaintInstalled;
}

static void InstallListView(HMODULE module, size_t index) {
    static constexpr ListPaint hooks[] = {
        ListPaint_Hook<0>, ListPaint_Hook<1>, ListPaint_Hook<2>, ListPaint_Hook<3>};
    static constexpr ListDrag dragHooks[] = {
        ListDrag_Hook<0>, ListDrag_Hook<1>, ListDrag_Hook<2>, ListDrag_Hook<3>};
    g_listModules[index].module = module;
    auto getVersion = reinterpret_cast<HRESULT(CALLBACK*)(DLLVERSIONINFO*)>(
        GetProcAddress(module, "DllGetVersion"));
    DLLVERSIONINFO version{};
    version.cbSize = sizeof(version);
    if (!getVersion || FAILED(getVersion(&version)) || version.dwMajorVersion < 6) return;
    // Keep the same optional symbol set even when fade is disabled. The cache
    // is per module; separate calls with different sets overwrite one another.
    WindhawkUtils::SYMBOL_HOOK comctl32_dll_hooks[] = {
        {{L"private: void " MEMBER_TEXT L" CLVDrawManager::_PaintMarquee(struct HDC__ *,void *,struct tagRECT const *)"},
         &g_listModules[index].original, hooks[index], true},
        {{L"public: void " MEMBER_TEXT L" CLVSelectionManager::DragSelect(int,int)"},
         &g_listModules[index].dragOriginal, dragHooks[index], true},
    };
    bool resolved = WindhawkUtils::HookSymbols(module, comctl32_dll_hooks, std::size(comctl32_dll_hooks));
    auto& entry = g_listModules[index];
    entry.installed = resolved && entry.original;
    entry.fadeInstalled = g_settings.fadeMs > 0 && entry.installed && entry.dragOriginal &&
        InstallBufferedPaint();
    if (!entry.installed)
        Wh_Log(L"List view alpha-marquee hook unavailable in module %p; remains native", module);
    else if (g_settings.fadeMs > 0 && !entry.fadeInstalled)
        Wh_Log(L"List view fade hooks unavailable; marquee hides immediately");
}

// Resolution is gated by actual controls, not by DLL presence. Many programs
// load comctl32/dui70 without ever creating a supported selection surface.
static bool DiscoverContexts() {
    HMODULE lists[std::size(g_requestedListModules)];
    bool directUI;
    {
        ExclusiveLock lock(&g_symbolLock);
        std::copy(std::begin(g_requestedListModules), std::end(g_requestedListModules), lists);
        directUI = g_duiContextSeen;
    }
    bool changed = false;
    for (size_t i = 0; i < std::size(lists); ++i) {
        if (g_unloading.load()) return changed;
        if (lists[i] && !g_listModules[i].module) {
            InstallListView(lists[i], i);
            changed = true;
        }
    }
    if (directUI && !g_duiContextAttempted.load() && !g_unloading.load()) {
        // Neither module is loaded on behalf of an unrelated host. Keep both
        // alive only once a real Shell/DirectUI window and both DLLs exist.
        HMODULE dui = nullptr, frame = nullptr;
        GetModuleHandleExW(0, L"dui70.dll", &dui);
        GetModuleHandleExW(0, L"ExplorerFrame.dll", &frame);
        if (dui && frame) {
            g_duiModule = dui;
            g_frameModule = frame;
            InstallDirectUI();
            g_duiContextAttempted.store(true);
            changed = true;
        } else {
            if (dui) FreeLibrary(dui);
            if (frame) FreeLibrary(frame);
        }
    }
    return changed;
}

static bool EnsureRenderer() {
    ExclusiveLock lock(&g_renderLock);
    if (g_renderer) return true;
    auto renderer = std::make_unique<Renderer>();
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, renderer->factory.GetAddressOf());
    if (FAILED(hr)) { Wh_Log(L"D2D factory creation failed: %08X", hr); return false; }
    g_renderer = renderer.release();
    return true;
}

static DWORD WINAPI ModuleWorker(void*) {
    while (WaitForSingleObject(g_moduleEvent, INFINITE) == WAIT_OBJECT_0 && !g_unloading.load()) {
        bool changed = DiscoverContexts();
        if (changed && !g_unloading.load()) {
            // HookSymbols can partially stage hooks even on failure. Always
            // apply the batch, keeping each incomplete capability disabled.
            bool applied = Wh_ApplyHookOperations();
            if (!applied) g_hookApplyFailed.store(true);
            bool capable = g_duiExportsInstalled && g_frameInstalled;
            for (auto& module : g_listModules) capable = capable || module.installed;
            bool ready = applied && capable && EnsureRenderer();
            g_duiReady.store(ready && g_duiExportsInstalled && g_frameInstalled &&
                Start_Original && End_Original && ElementPaint_Original && Destroy_Original);
            Fade::duiReady.store(g_duiFadeInstalled && g_duiReady.load());
            for (auto& module : g_listModules) {
                module.ready.store(ready && module.installed);
                module.fadeReady.store(ready && module.installed && module.fadeInstalled);
            }
            if (!applied) {
                // Windhawk can remove disabled hooks after a partial apply.
                // A later batch must never republish those stale capabilities.
                Wh_Log(L"Failed to apply marquee hooks; reload the mod to enable custom drawing");
                break;
            }
        }
    }
    return 0;
}

// Caller holds g_symbolLock. BeforeUninit takes the same lock before stopping
// the worker, so a late window creation cannot start a thread after teardown.
static void RequestSymbolWork() {
    if (g_unloading.load() || g_hookApplyFailed.load()) return;
    if (!g_worker) {
        g_moduleEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_moduleEvent) { Wh_Log(L"Failed to create symbol worker event"); return; }
        g_worker = CreateThread(nullptr, 0, ModuleWorker, nullptr, 0, nullptr);
        if (!g_worker) {
            CloseHandle(g_moduleEvent);
            g_moduleEvent = nullptr;
            Wh_Log(L"Failed to start symbol worker; marquee remains native");
            return;
        }
    }
    SetEvent(g_moduleEvent);
}

static void ObserveWindow(HWND hwnd) {
    if (!hwnd || g_unloading.load() || g_hookApplyFailed.load()) return;
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || pid != GetCurrentProcessId()) return;
    wchar_t name[32];
    if (!GetClassNameW(hwnd, name, std::size(name))) return;
    if (g_settings.directUI && !g_duiContextAttempted.load() &&
        (_wcsicmp(name, L"DirectUIHWND") == 0 || _wcsicmp(name, L"SHELLDLL_DefView") == 0) &&
        GetModuleHandleW(L"dui70.dll") && GetModuleHandleW(L"ExplorerFrame.dll")) {
        ExclusiveLock lock(&g_symbolLock);
        if (!g_unloading.load() && !g_duiContextAttempted.load()) {
            g_duiContextSeen = true;
            RequestSymbolWork();
        }
    }
    if (!g_settings.listViews || _wcsicmp(name, WC_LISTVIEWW) != 0) return;
    // Use the class procedure, not the subclassed window procedure. This picks
    // the exact SxS comctl32 that implements this list, rather than every copy.
    auto proc = reinterpret_cast<LPCWSTR>(GetClassLongPtrW(hwnd, GCLP_WNDPROC));
    HMODULE module = nullptr;
    if (!proc || !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, proc, &module)) return;
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(module, path, std::size(path));
    const wchar_t* base = length && length < std::size(path) ? wcsrchr(path, L'\\') : nullptr;
    if (base && _wcsicmp(base + 1, L"comctl32.dll") == 0) {
        ExclusiveLock lock(&g_symbolLock);
        if (!g_unloading.load()) {
            size_t slot = std::size(g_requestedListModules);
            bool known = false;
            for (size_t i = 0; i < std::size(g_requestedListModules); ++i) {
                if (g_requestedListModules[i] == module) known = true;
                if (!g_requestedListModules[i] && slot == std::size(g_requestedListModules)) slot = i;
            }
            if (!known && slot < std::size(g_requestedListModules)) {
                g_requestedListModules[slot] = module;
                module = nullptr; // transfer this reference to the demand queue
                RequestSymbolWork();
            } else if (known && !g_worker) {
                RequestSymbolWork(); // retry a failed worker/event creation
            }
        }
    }
    if (module) FreeLibrary(module);
}

static BOOL CALLBACK ObserveChild(HWND hwnd, LPARAM) {
    ObserveWindow(hwnd);
    return TRUE;
}
static BOOL CALLBACK ObserveTop(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        ObserveWindow(hwnd);
        EnumChildWindows(hwnd, ObserveChild, 0);
    }
    return TRUE;
}

// Same creation point as Modernizer: covers dialog-manager/internal window
// creation as well as CreateWindowExA/W. No private symbols are needed here.
using NativeCreate = HWND(WINAPI*)(DWORD, void*, LPCWSTR, void*, DWORD, LONG, LONG,
    LONG, LONG, HWND, HMENU, HINSTANCE, LPVOID, DWORD, DWORD, DWORD, void*);
static NativeCreate NativeCreate_Original;
static HWND WINAPI NativeCreate_Hook(DWORD ex, void* cls, LPCWSTR name, void* title,
        DWORD style, LONG x, LONG y, LONG w, LONG h, HWND parent, HMENU menu,
        HINSTANCE instance, LPVOID param, DWORD show, DWORD flags1, DWORD flags2, void* flags) {
    HWND hwnd = NativeCreate_Original(ex, cls, name, title, style, x, y, w, h,
        parent, menu, instance, param, show, flags1, flags2, flags);
    DWORD error = GetLastError();
    ObserveWindow(hwnd);
    SetLastError(error);
    return hwnd;
}

BOOL Wh_ModInit() {
    g_settings = {
        std::clamp(Wh_GetIntSetting(L"CornerRadius"), 0, 64),
        std::clamp(Wh_GetIntSetting(L"BorderWidth"), 0, 16),
        std::clamp(Wh_GetIntSetting(L"FillOpacity"), 0, 100),
        Wh_GetIntSetting(L"DirectUI") != 0,
        Wh_GetIntSetting(L"NativeListViews") != 0,
        std::clamp(Wh_GetIntSetting(L"FadeDuration"), 0, 1000),
        LoadColor(L"FillColor"), LoadColor(L"BorderColor")};
    if (!g_settings.directUI && !g_settings.listViews) return FALSE;
    QueryPerformanceFrequency(&Fade::frequency);
    Fade::cleanupMessage = RegisterWindowMessageW(L"Windhawk_RoundedSelectionFadeCleanup_" WH_MOD_ID);
    g_shutdownInProgress = reinterpret_cast<ShutdownQuery>(GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "RtlDllShutdownInProgress"));
    HMODULE win32u = GetModuleHandleW(L"win32u.dll");
    auto create = win32u ? reinterpret_cast<NativeCreate>(GetProcAddress(win32u, "NtUserCreateWindowEx")) : nullptr;
    if (!create || !WindhawkUtils::SetFunctionHook(create, NativeCreate_Hook, &NativeCreate_Original) ||
        (g_settings.listViews &&
         !WindhawkUtils::SetFunctionHook(IntersectRect, IntersectRect_Hook, &IntersectRect_Original))) {
        Wh_Log(L"Failed to register marquee API hooks");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // One snapshot covers controls created before injection. Future controls
    // are observed after their native NtUserCreateWindowEx call returns.
    if (g_settings.directUI || g_settings.listViews) EnumWindows(ObserveTop, 0);
}

void Wh_ModBeforeUninit() {
    {
        ExclusiveLock lock(&g_symbolLock);
        g_unloading.store(true);
        if (g_moduleEvent) SetEvent(g_moduleEvent);
    }
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    g_duiReady.store(false);
    Fade::duiReady.store(false);
    for (auto& module : g_listModules) {
        module.ready.store(false);
        module.fadeReady.store(false);
    }
    Fade::Drain();
}

void Wh_ModUninit() {
    {
        ExclusiveLock lock(&g_renderLock);
        delete g_renderer;
        g_renderer = nullptr;
    }
    for (auto& module : g_listModules) module.module = nullptr;
    for (auto& module : g_requestedListModules) {
        if (module) FreeLibrary(module);
        module = nullptr;
    }
    if (g_duiModule) FreeLibrary(g_duiModule);
    if (g_frameModule) FreeLibrary(g_frameModule);
    if (Fade::duserModule) FreeLibrary(Fade::duserModule);
    Fade::duserModule = nullptr;
    if (g_uxthemeModule) FreeLibrary(g_uxthemeModule);
    g_uxthemeModule = nullptr;
    g_duiModule = g_frameModule = nullptr;
    if (g_moduleEvent) CloseHandle(g_moduleEvent);
    g_moduleEvent = nullptr;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    // Reload atomically replaces the immutable settings and hook capabilities.
    *reload = TRUE;
    return TRUE;
}
