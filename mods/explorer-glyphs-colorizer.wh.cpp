// ==WindhawkMod==
// @id              explorer-glyphs-colorizer
// @name            Explorer Glyphs Colorizer
// @name:pt         Colorizador de Glyphs do Explorador
// @name:es         Colorizador de Glifos del Explorador
// @description     Customize the blue details of File Explorer glyphs without changing the original files
// @description:pt  Personalize os detalhes azuis dos glyphs do Explorador sem alterar os arquivos originais
// @description:es  Personaliza los detalles azules de los glifos del Explorador sin modificar los archivos originales
// @version         1.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -ladvapi32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Glyphs Colorizer

Give the blue details in File Explorer's glyphs a color that fits your desktop.
Choose the Windows accent color, a custom color, or a neutral shade that blends
with the rest of the glyphs.

The color change is not permanent, and the original Windows files are never
touched. If you choose the Windows accent color, changing it usually updates
glyphs in open windows. Changing the mod settings does too. Some glyphs may
need a click or navigation to update. If they still do not change, restart
File Explorer to reload them. Turning the mod off brings back their original
colors; restart Explorer if any open window still shows the old color.

Supports File Explorer on Windows 11.
On ARM PCs, glyphs in already open windows may need an Explorer restart after
the color changes.

## Gallery / Galeria / Galería

### Neutral / Neutro

![Neutral 1](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Neutro%201.jpg)
![Neutral 2](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Neutro%202.jpg)
![Neutral 3](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Neutro%203.jpg)
![Neutral 4](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Neutro%204.jpg)

### Purple / Roxo / Morado

![Purple 1](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Roxo%201.jpg)
![Purple 2](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Roxo%202.jpg)

### Green / Verde

![Green 1](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Verde%201.jpg)
![Green 2](https://raw.githubusercontent.com/crazyboyybs/assets/main/Explorer%20Glyphs%20Colorizer/Verde%202.jpg)

## Inspiration

- [AccentColorizer-E11](https://github.com/krlvm/AccentColorizer-E11) inspired
  the idea of colorizing File Explorer glyphs.
- [Start button colorizer](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-start-button-colorizer.wh.cpp)
  inspired the goal of recoloring native UI without replacing its files.

---

# Colorizador de Glyphs do Explorador

Dê aos detalhes azuis dos glyphs do Explorador uma cor que combine com seu
desktop. Escolha a cor de destaque do Windows, uma cor personalizada ou um tom
neutro que combine com o restante dos glyphs.

A mudança de cor não é permanente, e os arquivos originais do Windows nunca
são tocados. Se você escolher a cor de destaque do Windows, mudá-la geralmente
atualiza os glyphs nas janelas abertas. Mudar as configurações também. Alguns
glyphs podem precisar de um clique ou de navegação para atualizar. Se ainda
não mudarem, reinicie o Explorador para recarregá-los. Ao desativar o mod, as
cores originais voltam; reinicie o Explorador se alguma janela ainda mostrar
a cor anterior.

Compatível com o Explorador do Windows 11.
Em PCs ARM, glyphs em janelas já abertas podem precisar de uma reinicialização
do Explorador após a mudança de cor.

## Inspiração

- [AccentColorizer-E11](https://github.com/krlvm/AccentColorizer-E11) inspirou
  a ideia de colorir os glyphs do Explorador.
- [Start button colorizer](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-start-button-colorizer.wh.cpp)
  inspirou o objetivo de recolorir a interface nativa sem substituir arquivos.

---

# Colorizador de Glifos del Explorador

Dé a los detalles azules de los glifos del Explorador un color que combine con
su escritorio. Elija el color de énfasis de Windows, un color personalizado o
un tono neutro que combine con el resto de los glifos.

El cambio de color no es permanente, y los archivos originales de Windows
nunca se modifican. Si elige el color de énfasis de Windows, cambiarlo suele
actualizar los glifos en las ventanas abiertas. Cambiar la configuración
también. Algunos glifos pueden necesitar un clic o una navegación para
actualizarse. Si todavía no cambian, reinicie el Explorador para volver a
cargarlos. Al desactivar el mod, vuelven los colores originales; reinicie el
Explorador si alguna ventana sigue mostrando el color anterior.

Compatible con el Explorador de Windows 11.
En equipos ARM, los glifos de las ventanas abiertas pueden necesitar reiniciar
el Explorador después de cambiar el color.

## Inspiración

- [AccentColorizer-E11](https://github.com/krlvm/AccentColorizer-E11) inspiró
  la idea de colorear los glifos del Explorador.
- [Start button colorizer](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-start-button-colorizer.wh.cpp)
  inspiró el objetivo de recolorear la interfaz nativa sin reemplazar archivos.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- colorMode: accent
  $name: Color source
  $name:pt: Origem da cor
  $name:es: Origen del color
  $description: Choose the Windows accent indicator shade, a neutral glyph stroke color, or a custom RGB color.
  $description:pt: Escolha o tom do indicador de destaque, uma cor neutra para os traços ou uma cor RGB personalizada.
  $description:es: Elija el tono del indicador de énfasis, un color neutro para los trazos o un color RGB personalizado.
  $options:
  - accent: Windows accent indicator
  - neutral: Match neutral glyph color
  - custom: Custom color
- customColor: "#FF8C00"
  $name: Custom color
  $name:pt: Cor personalizada
  $name:es: Color personalizado
  $description: RGB hex color, such as #FF8C00. Used when Color source is Custom color.
  $description:pt: Cor RGB hexadecimal, como #FF8C00. Usada quando a origem é Cor personalizada.
  $description:es: Color RGB hexadecimal, como #FF8C00. Se usa cuando el origen es Color personalizado.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <winternl.h>
#include <windhawk_utils.h>

#include <array>
#include <atomic>
#include <cstring>
#include <iterator>
#include <mutex>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxi = winrt::Microsoft::UI::Xaml::Input;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace muxim = winrt::Microsoft::UI::Xaml::Media::Imaging;

using NtCreateFile_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK,
                                      POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK,
                                      PLARGE_INTEGER, ULONG, ULONG, ULONG,
                                      ULONG, PVOID, ULONG);
using NtOpenFile_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK,
                                    POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK,
                                    ULONG, ULONG);
using NtReadFile_t = NTSTATUS(NTAPI*)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID,
                                    PIO_STATUS_BLOCK, PVOID, ULONG,
                                    PLARGE_INTEGER, PULONG);
using NtClose_t = NTSTATUS(NTAPI*)(HANDLE);

static NtCreateFile_t g_originalNtCreateFile;
static NtOpenFile_t g_originalNtOpenFile;
static NtReadFile_t g_originalNtReadFile;
static NtClose_t g_originalNtClose;
static std::atomic<bool> g_unloading{false};

enum class SvgTheme : unsigned char { None, Dark, Light };
enum class ColorMode : unsigned char { Accent, Neutral, Custom };
struct TrackedFile {
    HANDLE handle = nullptr;
    SvgTheme theme = SvgTheme::None;
};
static SRWLOCK g_filesLock = SRWLOCK_INIT;
static std::array<TrackedFile, 256> g_files{};
static std::atomic<unsigned int> g_fileCount{0};
static std::atomic<bool> g_fileOverflowLogged{false};
// Protected by g_filesLock; avoids scanning unused slots on every read/close.
static size_t g_fileScanLimit = 0;

struct Settings {
    std::atomic<ColorMode> colorMode{ColorMode::Accent};
    std::atomic<COLORREF> customColor{RGB(0xFF, 0x8C, 0x00)};
    std::atomic<COLORREF> darkAccent{RGB(0x4C, 0xC2, 0xFF)};
    std::atomic<COLORREF> lightAccent{RGB(0x00, 0x78, 0xD4)};
};
static Settings g_settings;

// Verified for this x64 FileExplorerExtensions.dll: RCX=this, RDX=return
// storage, R8=Uri, R9=Size, and the double is passed on the stack.
using GetResolverSvg_t = muxim::SvgImageSource* (*)(
    void*, muxim::SvgImageSource*, wf::Uri const&, wf::Size const&, double);
using PointerPressed_t = void (*)(
    void*, wf::IInspectable const&, muxi::PointerRoutedEventArgs const&);
static GetResolverSvg_t g_originalGetResolverSvg;
static PointerPressed_t g_originalPointerPressed;
static PointerPressed_t g_originalWave1PointerPressed;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_originalLoadLibraryExW;
static std::atomic<bool> g_explorerSymbolsAttempted{false};
static std::atomic<unsigned int> g_symbolSetupActive{0};

struct TrackedSource {
    winrt::weak_ref<muxim::SvgImageSource> weak;
    wf::Uri uri{nullptr};
    void* identity = nullptr;
    DWORD threadId = 0;
    COLORREF appliedColor = 0;
    bool hasAppliedColor = false;
};
static std::mutex g_sourcesMutex;
[[clang::no_destroy]] static std::array<TrackedSource, 4096> g_sources{};
static size_t g_sourceCount = 0;
static std::atomic<bool> g_sourceOverflowLogged{false};

static std::mutex g_uiThreadsMutex;
static std::array<DWORD, 256> g_uiThreads{};
static size_t g_uiThreadCount = 0;
static HANDLE g_watcherStopEvent;
static HANDLE g_watcherThread;

static void ScheduleColorRefresh();
static COLORREF CurrentColor(SvgTheme theme);

template <typename Char>
static Char LowerAscii(Char c) {
    return c >= static_cast<Char>('A') && c <= static_cast<Char>('Z')
               ? c + ('a' - 'A') : c;
}

static bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (LowerAscii(a[i]) != LowerAscii(b[i])) return false;
    }
    return true;
}

static bool ContainsIgnoreCase(std::wstring_view text,
                               std::wstring_view fragment) {
    if (text.size() < fragment.size()) return false;
    for (size_t i = 0; i <= text.size() - fragment.size(); ++i) {
        if (EqualsIgnoreCase(text.substr(i, fragment.size()), fragment)) {
            return true;
        }
    }
    return false;
}

static SvgTheme ThemeFromPath(POBJECT_ATTRIBUTES attributes) {
    if (!attributes || !attributes->ObjectName) return SvgTheme::None;
    const UNICODE_STRING& name = *attributes->ObjectName;
    if (!name.Buffer || name.Length == 0 || name.Length > 4096 ||
        name.Length % sizeof(wchar_t) != 0) return SvgTheme::None;

    std::wstring_view path(name.Buffer, name.Length / sizeof(wchar_t));
    if (path.size() < 4 ||
        !EqualsIgnoreCase(path.substr(path.size() - 4), L".svg") ||
        !ContainsIgnoreCase(path, L"\\SystemApps\\") ||
        !ContainsIgnoreCase(path, L"\\FileExplorerExtensions\\Assets\\images\\contrast-standard\\")) {
        return SvgTheme::None;
    }
    if (ContainsIgnoreCase(path, L"\\theme-dark\\")) return SvgTheme::Dark;
    if (ContainsIgnoreCase(path, L"\\theme-light\\")) return SvgTheme::Light;
    return SvgTheme::None;
}

static void TrackFile(HANDLE handle, SvgTheme theme) {
    if (!handle || handle == INVALID_HANDLE_VALUE || theme == SvgTheme::None) return;
    AcquireSRWLockExclusive(&g_filesLock);
    size_t freeIndex = g_fileScanLimit;
    for (size_t i = 0; i < g_fileScanLimit; ++i) {
        auto& slot = g_files[i];
        if (slot.theme != SvgTheme::None && slot.handle == handle) {
            slot.theme = theme;
            ReleaseSRWLockExclusive(&g_filesLock);
            return;
        }
        if (freeIndex == g_fileScanLimit && slot.theme == SvgTheme::None)
            freeIndex = i;
    }
    bool overflow = freeIndex == g_files.size();
    if (!overflow) {
        if (freeIndex == g_fileScanLimit) ++g_fileScanLimit;
        g_files[freeIndex] = {handle, theme};
        g_fileCount.fetch_add(1, std::memory_order_release);
    }
    ReleaseSRWLockExclusive(&g_filesLock);
    if (overflow && !g_fileOverflowLogged.exchange(true))
        Wh_Log(L"Tracked SVG file limit reached; some glyphs may retain their original color");
}

static SvgTheme FindFile(HANDLE handle) {
    if (!g_fileCount.load(std::memory_order_acquire)) return SvgTheme::None;
    AcquireSRWLockShared(&g_filesLock);
    SvgTheme theme = SvgTheme::None;
    for (size_t i = 0; i < g_fileScanLimit; ++i) {
        const auto& slot = g_files[i];
        if (slot.theme != SvgTheme::None && slot.handle == handle) {
            theme = slot.theme;
            break;
        }
    }
    ReleaseSRWLockShared(&g_filesLock);
    return theme;
}

static SvgTheme ForgetFile(HANDLE handle) {
    if (!g_fileCount.load(std::memory_order_acquire)) return SvgTheme::None;
    AcquireSRWLockExclusive(&g_filesLock);
    SvgTheme theme = SvgTheme::None;
    for (size_t i = 0; i < g_fileScanLimit; ++i) {
        auto& slot = g_files[i];
        if (slot.theme != SvgTheme::None && slot.handle == handle) {
            theme = slot.theme;
            slot = {};
            while (g_fileScanLimit &&
                   g_files[g_fileScanLimit - 1].theme == SvgTheme::None)
                --g_fileScanLimit;
            g_fileCount.fetch_sub(1, std::memory_order_release);
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_filesLock);
    return theme;
}

static int HexDigit(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    c = LowerAscii(c);
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    return -1;
}

static bool ParseColor(std::wstring_view value, COLORREF* color) {
    if (value.size() == 7 && value[0] == L'#') value.remove_prefix(1);
    if (value.size() != 6) return false;
    unsigned int rgb = 0;
    for (wchar_t c : value) {
        int digit = HexDigit(c);
        if (digit < 0) return false;
        rgb = (rgb << 4) | digit;
    }
    *color = RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    return true;
}

// Same AccentPalette entries as Modernizer's GetAccentIndicator: Light2 for
// dark UI (offset 4) and Dark1 for light UI (offset 16).
static void RefreshAccentColors() {
    BYTE palette[32]{};
    DWORD size = sizeof(palette);
    DWORD type = 0;
    bool valid = false;
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                      0, KEY_READ, &key) == ERROR_SUCCESS) {
        valid = RegQueryValueExW(key, L"AccentPalette", nullptr, &type,
                                 palette, &size) == ERROR_SUCCESS &&
                type == REG_BINARY && size >= 20;
        RegCloseKey(key);
    }
    if (valid) {
        g_settings.darkAccent.store(RGB(palette[4], palette[5], palette[6]),
                                    std::memory_order_relaxed);
        g_settings.lightAccent.store(RGB(palette[16], palette[17], palette[18]),
                                     std::memory_order_relaxed);
        return;
    }

    DWORD argb = 0;
    BOOL opaque = FALSE;
    COLORREF fallback = SUCCEEDED(DwmGetColorizationColor(&argb, &opaque))
                            ? RGB((argb >> 16) & 0xFF, (argb >> 8) & 0xFF,
                                  argb & 0xFF)
                            : GetSysColor(COLOR_HOTLIGHT);
    g_settings.darkAccent.store(fallback, std::memory_order_relaxed);
    g_settings.lightAccent.store(fallback, std::memory_order_relaxed);
}

static void LoadSettings() {
    auto mode = WindhawkUtils::StringSetting::make(L"colorMode");
    ColorMode colorMode = ColorMode::Accent;
    if (_wcsicmp(mode, L"neutral") == 0)
        colorMode = ColorMode::Neutral;
    else if (_wcsicmp(mode, L"custom") == 0)
        colorMode = ColorMode::Custom;
    auto text = WindhawkUtils::StringSetting::make(L"customColor");
    COLORREF customColor;
    if (ParseColor(static_cast<PCWSTR>(text), &customColor)) {
        g_settings.customColor.store(customColor, std::memory_order_relaxed);
    }
    RefreshAccentColors();
    g_settings.colorMode.store(colorMode, std::memory_order_release);
}

static bool MatchesColor(const char* bytes, const char* color) {
    for (int i = 0; i < 7; ++i) {
        if (LowerAscii(bytes[i]) != LowerAscii(color[i])) return false;
    }
    return true;
}

static void RecolorSvgBytes(void* buffer, size_t size,
                            SvgTheme theme, COLORREF color) {
    const char* original = theme == SvgTheme::Dark ? "#4CC2FF" : "#0078D4";
    constexpr char hex[] = "0123456789ABCDEF";
    char replacement[7] = {'#'};
    BYTE channels[3] = {GetRValue(color), GetGValue(color), GetBValue(color)};
    for (int i = 0; i < 3; ++i) {
        replacement[1 + i * 2] = hex[channels[i] >> 4];
        replacement[2 + i * 2] = hex[channels[i] & 0xF];
    }

    auto* bytes = static_cast<char*>(buffer);
    for (size_t i = 0; i + 7 <= size; ++i) {
        if (bytes[i] == '#' && MatchesColor(bytes + i, original)) {
            memcpy(bytes + i, replacement, sizeof(replacement));
            i += 6;
        }
    }
}

static NTSTATUS NTAPI NtCreateFile_Hook(
    PHANDLE fileHandle, ACCESS_MASK access, POBJECT_ATTRIBUTES attributes,
    PIO_STATUS_BLOCK ioStatus, PLARGE_INTEGER allocationSize,
    ULONG fileAttributes, ULONG shareAccess, ULONG disposition,
    ULONG options, PVOID eaBuffer, ULONG eaLength) {
    NTSTATUS status = g_originalNtCreateFile(
        fileHandle, access, attributes, ioStatus, allocationSize,
        fileAttributes, shareAccess, disposition, options, eaBuffer, eaLength);
    if (!g_unloading.load(std::memory_order_relaxed) &&
        NT_SUCCESS(status) && fileHandle) {
        SvgTheme theme = ThemeFromPath(attributes);
        if (theme != SvgTheme::None) {
            TrackFile(*fileHandle, theme);
        }
    }
    return status;
}

static NTSTATUS NTAPI NtOpenFile_Hook(
    PHANDLE fileHandle, ACCESS_MASK access, POBJECT_ATTRIBUTES attributes,
    PIO_STATUS_BLOCK ioStatus, ULONG shareAccess, ULONG options) {
    NTSTATUS status = g_originalNtOpenFile(fileHandle, access, attributes,
                                           ioStatus, shareAccess, options);
    if (!g_unloading.load(std::memory_order_relaxed) &&
        NT_SUCCESS(status) && fileHandle) {
        SvgTheme theme = ThemeFromPath(attributes);
        if (theme != SvgTheme::None) {
            TrackFile(*fileHandle, theme);
        }
    }
    return status;
}

static NTSTATUS NTAPI NtReadFile_Hook(
    HANDLE fileHandle, HANDLE event, PIO_APC_ROUTINE apcRoutine,
    PVOID apcContext, PIO_STATUS_BLOCK ioStatus, PVOID buffer, ULONG length,
    PLARGE_INTEGER byteOffset, PULONG key) {
    SvgTheme theme = FindFile(fileHandle);
    NTSTATUS status = g_originalNtReadFile(fileHandle, event, apcRoutine,
                                           apcContext, ioStatus, buffer,
                                           length, byteOffset, key);
    if (theme == SvgTheme::None ||
        g_unloading.load(std::memory_order_relaxed) ||
        status != 0 || !ioStatus || !buffer ||
        ioStatus->Information > length) {
        return status;
    }

    RecolorSvgBytes(buffer, ioStatus->Information, theme,
                    CurrentColor(theme));
    return status;
}

static NTSTATUS NTAPI NtClose_Hook(HANDLE handle) {
    SvgTheme theme = ForgetFile(handle);
    NTSTATUS status = g_originalNtClose(handle);
    if (theme != SvgTheme::None && !NT_SUCCESS(status)) {
        TrackFile(handle, theme);
    }
    return status;
}

static SvgTheme ThemeFromUri(wf::Uri const& uri) {
    if (!uri) return SvgTheme::None;
    auto raw = uri.RawUri();
    std::wstring_view path(raw.c_str(), raw.size());
    if (path.size() < 4 ||
        !EqualsIgnoreCase(path.substr(path.size() - 4), L".svg") ||
        !ContainsIgnoreCase(path, L"/SystemApps/") ||
        !ContainsIgnoreCase(path,
            L"/FileExplorerExtensions/Assets/images/contrast-standard/")) {
        return SvgTheme::None;
    }
    if (ContainsIgnoreCase(path, L"/theme-dark/")) return SvgTheme::Dark;
    if (ContainsIgnoreCase(path, L"/theme-light/")) return SvgTheme::Light;
    return SvgTheme::None;
}

static COLORREF CurrentColor(SvgTheme theme) {
    ColorMode mode = g_settings.colorMode.load(std::memory_order_acquire);
    if (mode == ColorMode::Custom)
        return g_settings.customColor.load(std::memory_order_relaxed);
    if (mode == ColorMode::Neutral)
        return theme == SvgTheme::Dark ? RGB(0xE0, 0xDF, 0xDF)
                                       : RGB(0x55, 0x55, 0x55);
    return theme == SvgTheme::Dark
        ? g_settings.darkAccent.load(std::memory_order_relaxed)
        : g_settings.lightAccent.load(std::memory_order_relaxed);
}

static void RegisterUiThread() {
    if (g_unloading.load(std::memory_order_acquire)) return;
    DWORD threadId = GetCurrentThreadId();
    std::lock_guard lock(g_uiThreadsMutex);
    if (g_unloading.load(std::memory_order_acquire)) return;
    for (size_t i = 0; i < g_uiThreadCount; ++i)
        if (g_uiThreads[i] == threadId) return;
    if (g_uiThreadCount == g_uiThreads.size()) return;
    g_uiThreads[g_uiThreadCount++] = threadId;
}

static bool CaptureSource(muxim::SvgImageSource const& source,
                          bool fromVisualTree) {
    try {
        auto uri = source.UriSource();
        SvgTheme theme = ThemeFromUri(uri);
        if (theme == SvgTheme::None) return false;
        DWORD threadId = GetCurrentThreadId();
        void* identity = winrt::get_abi(source);
        auto weak = winrt::make_weak(source);
        {
            std::unique_lock lock(g_sourcesMutex);
            for (size_t i = 0; i < g_sourceCount;) {
                // Resolve XAML weak references only on their owning UI thread.
                if (g_sources[i].threadId == threadId &&
                    !g_sources[i].weak.get()) {
                    g_sources[i] = std::move(g_sources[g_sourceCount - 1]);
                    g_sources[--g_sourceCount] = {};
                    continue;
                }
                if (g_sources[i].threadId == threadId &&
                    g_sources[i].identity == identity) {
                    if (g_sources[i].uri.RawUri() != uri.RawUri()) {
                        g_sources[i].uri = uri;
                        g_sources[i].hasAppliedColor = false;
                    }
                    return true;
                }
                ++i;
            }
            if (g_sourceCount == g_sources.size()) {
                lock.unlock();
                if (!g_sourceOverflowLogged.exchange(true))
                    Wh_Log(L"Tracked glyph limit reached; live color updates may be incomplete");
                return false;
            }
            auto& tracked = g_sources[g_sourceCount++];
            tracked.weak = std::move(weak);
            tracked.uri = uri;
            tracked.identity = identity;
            tracked.threadId = threadId;
            tracked.appliedColor = CurrentColor(theme);
            tracked.hasAppliedColor = !fromVisualTree;
        }
        RegisterUiThread();
        return true;
    } catch (...) {
    }
    return false;
}

static muxim::SvgImageSource* GetResolverSvg_Hook(
    void* self, muxim::SvgImageSource* result, wf::Uri const& uri,
    wf::Size const& size, double scale) {
    auto returned = g_originalGetResolverSvg(self, result, uri, size, scale);
    if (!g_unloading.load(std::memory_order_relaxed) && result && *result) {
        try {
            if (ThemeFromUri(uri) != SvgTheme::None)
                CaptureSource(*result, false);
        } catch (...) {
        }
    }
    return returned;
}

static void CaptureFromVisualTree(wf::IInspectable const& sender) {
    try {
        auto root = sender.try_as<mux::DependencyObject>();
        if (!root) return;
        for (unsigned int i = 0; i < 10; ++i) {
            auto className = winrt::get_class_name(root);
            if (wcsstr(className.c_str(), L"CommandBarControl")) break;
            auto parent = muxm::VisualTreeHelper::GetParent(root);
            if (!parent) break;
            root = parent;
        }

        std::vector<mux::DependencyObject> stack;
        stack.reserve(512);
        unsigned int visited = 0;
        stack.push_back(root);
        while (!stack.empty() && visited < 512) {
            auto element = std::move(stack.back());
            stack.pop_back();
            if (!element) continue;
            ++visited;
            muxim::SvgImageSource svg{nullptr};
            if (auto icon = element.try_as<muxc::ImageIcon>())
                svg = icon.Source().try_as<muxim::SvgImageSource>();
            else if (auto image = element.try_as<muxc::Image>())
                svg = image.Source().try_as<muxim::SvgImageSource>();
            if (svg) CaptureSource(svg, true);
            int children = muxm::VisualTreeHelper::GetChildrenCount(element);
            for (int i = children - 1; i >= 0 && stack.size() < 512; --i)
                stack.push_back(muxm::VisualTreeHelper::GetChild(element, i));
        }
    } catch (...) {
    }
}

static void RefreshOnCurrentThread() {
    if (g_unloading.load(std::memory_order_relaxed)) return;
    DWORD threadId = GetCurrentThreadId();
    std::vector<TrackedSource> snapshot;
    {
        std::lock_guard lock(g_sourcesMutex);
        for (size_t i = 0; i < g_sourceCount; ++i)
            if (g_sources[i].threadId == threadId)
                snapshot.push_back(g_sources[i]);
    }

    for (auto const& tracked : snapshot) {
        wf::Uri currentUri{nullptr};
        try {
            if (auto source = tracked.weak.get()) {
                // Explorer can replace the URI during a real light/dark switch.
                currentUri = source.UriSource();
                SvgTheme theme = ThemeFromUri(currentUri);
                if (theme == SvgTheme::None) continue;
                COLORREF color = CurrentColor(theme);
                bool uriChanged = tracked.uri.RawUri() != currentUri.RawUri();
                if (!uriChanged && tracked.hasAppliedColor &&
                    tracked.appliedColor == color)
                    continue;
                source.UriSource(nullptr);
                source.UriSource(currentUri);
                std::lock_guard lock(g_sourcesMutex);
                for (size_t i = 0; i < g_sourceCount; ++i) {
                    if (g_sources[i].threadId == threadId &&
                        g_sources[i].identity == tracked.identity) {
                        g_sources[i].uri = currentUri;
                        g_sources[i].appliedColor = color;
                        g_sources[i].hasAppliedColor = true;
                        break;
                    }
                }
            } else {
                std::lock_guard lock(g_sourcesMutex);
                for (size_t i = 0; i < g_sourceCount; ++i) {
                    if (g_sources[i].threadId == threadId &&
                        g_sources[i].identity == tracked.identity) {
                        g_sources[i] = std::move(g_sources[g_sourceCount - 1]);
                        g_sources[--g_sourceCount] = {};
                        break;
                    }
                }
            }
        } catch (...) {
            try {
                if (auto source = tracked.weak.get())
                    source.UriSource(currentUri ? currentUri : tracked.uri);
            } catch (...) {
            }
        }
    }

    bool hasSources = false;
    {
        std::lock_guard lock(g_sourcesMutex);
        for (size_t i = 0; i < g_sourceCount; ++i) {
            if (g_sources[i].threadId == threadId) {
                hasSources = true;
                break;
            }
        }
    }
    if (!hasSources) {
        std::lock_guard lock(g_uiThreadsMutex);
        for (size_t i = 0; i < g_uiThreadCount; ++i) {
            if (g_uiThreads[i] == threadId) {
                g_uiThreads[i] = g_uiThreads[g_uiThreadCount - 1];
                g_uiThreads[--g_uiThreadCount] = 0;
                break;
            }
        }
    }
}

static UINT RunOnUiThreadMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_ExplorerGlyphsColorizer_RunOnUiThread");
    return message;
}

struct ThreadWindow {
    HWND window = nullptr;
};

static HWND FindThreadWindow(DWORD threadId) {
    ThreadWindow found;
    EnumThreadWindows(threadId, [](HWND window, LPARAM data) -> BOOL {
        auto& found = *reinterpret_cast<ThreadWindow*>(data);
        wchar_t className[64];
        if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"CabinetWClass") == 0) {
            found.window = window;
            return FALSE;
        }
        if (!found.window) found.window = window;
        return TRUE;
    }, reinterpret_cast<LPARAM>(&found));
    return found.window;
}

struct RunOnUiThreadParam {
    HWND window;
    DWORD threadId;
    void (*callback)();
    bool called = false;
};

static LRESULT CALLBACK RunOnUiThreadHook(int code, WPARAM wParam,
                                           LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (message->message == RunOnUiThreadMessage() && message->lParam) {
            auto* param = reinterpret_cast<RunOnUiThreadParam*>(message->lParam);
            if (message->hwnd == param->window &&
                param->threadId == GetCurrentThreadId() && !param->called) {
                param->called = true;
                try {
                    param->callback();
                } catch (...) {
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static bool RunOnUiThread(DWORD threadId, void (*callback)()) {
    if (threadId == GetCurrentThreadId()) {
        try {
            callback();
        } catch (...) {
            return false;
        }
        return true;
    }
    HWND window = FindThreadWindow(threadId);
    if (!window || !RunOnUiThreadMessage()) return false;
    DWORD processId;
    if (GetWindowThreadProcessId(window, &processId) != threadId ||
        processId != GetCurrentProcessId()) return false;

    // Avoid waiting on a UI thread that has already stopped pumping messages.
    DWORD_PTR result;
    if (!SendMessageTimeoutW(window, WM_NULL, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result))
        return false;

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnUiThreadHook,
                                  nullptr, threadId);
    if (!hook) return false;
    RunOnUiThreadParam param{window, threadId, callback};
    SendMessageW(window, RunOnUiThreadMessage(), 0,
                 reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return param.called;
}

static void ForEachUiThread(void (*callback)()) {
    std::array<DWORD, 256> threads{};
    size_t count = 0;
    {
        std::lock_guard lock(g_uiThreadsMutex);
        count = g_uiThreadCount;
        for (size_t i = 0; i < count; ++i) threads[i] = g_uiThreads[i];
    }
    for (size_t i = 0; i < count; ++i)
        RunOnUiThread(threads[i], callback);
}

static void ScheduleColorRefresh() {
    if (!g_unloading.load(std::memory_order_acquire))
        ForEachUiThread(RefreshOnCurrentThread);
}

static void PointerPressed_Hook(
    void* self, wf::IInspectable const& sender,
    muxi::PointerRoutedEventArgs const& args) {
    if (!g_unloading.load(std::memory_order_acquire)) {
        try {
            CaptureFromVisualTree(sender);
            RefreshOnCurrentThread();
        } catch (...) {
        }
    }
    g_originalPointerPressed(self, sender, args);
}

static void Wave1PointerPressed_Hook(
    void* self, wf::IInspectable const& sender,
    muxi::PointerRoutedEventArgs const& args) {
    if (!g_unloading.load(std::memory_order_acquire)) {
        try {
            CaptureFromVisualTree(sender);
            RefreshOnCurrentThread();
        } catch (...) {
        }
    }
    g_originalWave1PointerPressed(self, sender, args);
}

static DWORD WINAPI AccentWatcherThread(void*) {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (...) {
        return 0;
    }

    HKEY key = nullptr;
    LONG status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
        0, KEY_NOTIFY, &key);
    if (status != ERROR_SUCCESS) {
        winrt::uninit_apartment();
        return 0;
    }
    HANDLE changedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!changedEvent) {
        RegCloseKey(key);
        winrt::uninit_apartment();
        return 0;
    }

    COLORREF observedDark = g_settings.darkAccent.load(std::memory_order_relaxed);
    COLORREF observedLight = g_settings.lightAccent.load(std::memory_order_relaxed);
    HANDLE events[] = {g_watcherStopEvent, changedEvent};
    while (!g_unloading.load(std::memory_order_acquire)) {
        status = RegNotifyChangeKeyValue(
            key, FALSE, REG_NOTIFY_CHANGE_LAST_SET, changedEvent, TRUE);
        if (status != ERROR_SUCCESS) break;
        RefreshAccentColors();
        COLORREF dark = g_settings.darkAccent.load(std::memory_order_relaxed);
        COLORREF light = g_settings.lightAccent.load(std::memory_order_relaxed);
        if (dark != observedDark || light != observedLight) {
            observedDark = dark;
            observedLight = light;
            if (g_settings.colorMode.load(std::memory_order_acquire) ==
                ColorMode::Accent)
                ScheduleColorRefresh();
        }

        DWORD wait = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0) break;
        if (wait != WAIT_OBJECT_0 + 1) break;
    }
    CloseHandle(changedEvent);
    RegCloseKey(key);
    winrt::uninit_apartment();
    return 0;
}

static void RestoreOnCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<TrackedSource> snapshot;
    {
        std::lock_guard lock(g_sourcesMutex);
        for (size_t i = 0; i < g_sourceCount; ++i)
            if (g_sources[i].threadId == threadId)
                snapshot.push_back(g_sources[i]);
    }
    for (auto const& tracked : snapshot) {
        wf::Uri originalUri{nullptr};
        try {
            if (auto source = tracked.weak.get()) {
                // Use the source's current theme URI, which can differ from
                // the URI captured before a real light/dark mode switch.
                originalUri = source.UriSource();
                if (ThemeFromUri(originalUri) == SvgTheme::None) continue;
                source.UriSource(nullptr);
                source.UriSource(originalUri);
            }
        } catch (...) {
            try {
                if (auto source = tracked.weak.get())
                    source.UriSource(originalUri ? originalUri : tracked.uri);
            } catch (...) {
            }
        }
    }
}

static void RestoreNativeSources() {
    ForEachUiThread(RestoreOnCurrentThread);
}

#if defined(_M_X64)
// These private FileExplorerExtensions method signatures were verified on x64.
// On ARM64 the file-read hooks still recolor glyphs when Explorer loads them.
struct ActiveSymbolSetup {
    ActiveSymbolSetup() {
        g_symbolSetupActive.fetch_add(1, std::memory_order_acq_rel);
    }
    ~ActiveSymbolSetup() {
        if (g_symbolSetupActive.fetch_sub(1, std::memory_order_acq_rel) == 1)
            g_symbolSetupActive.notify_all();
    }
};

static void HookExplorerSymbols(HMODULE module, bool applyNow) {
    ActiveSymbolSetup active;
    if (g_unloading.load(std::memory_order_acquire) || !module ||
        g_explorerSymbolsAttempted.exchange(true, std::memory_order_acq_rel))
        return;
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {LR"(public: struct winrt::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource __cdecl winrt::FileExplorerExtensions::implementation::AssetResolver::GetSvgImageSource(struct winrt::Windows::Foundation::Uri const &,struct winrt::Windows::Foundation::Size const &,double))"},
            &g_originalGetResolverSvg, GetResolverSvg_Hook, true,
        },
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::PointerPressedHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const &))"},
            &g_originalPointerPressed, PointerPressed_Hook, true,
        },
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::PointerPressedHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const &))"},
            &g_originalWave1PointerPressed, Wave1PointerPressed_Hook, true,
        },
    };
    try {
        bool hooked = WindhawkUtils::HookSymbols(
            module, fileExplorerExtensionsDllHooks,
            std::size(fileExplorerExtensionsDllHooks));
        if (applyNow && hooked) Wh_ApplyHookOperations();
    } catch (...) {
    }
}

static HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = g_originalLoadLibraryExW(fileName, file, flags);
    if (module && !g_unloading.load(std::memory_order_acquire) &&
        !g_explorerSymbolsAttempted.load(std::memory_order_acquire)) {
        HookExplorerSymbols(GetModuleHandleW(L"FileExplorerExtensions.dll"), true);
    }
    return module;
}
#endif

BOOL Wh_ModInit() {
    LoadSettings();
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) return FALSE;

    auto create = GetProcAddress(ntdll, "NtCreateFile");
    auto open = GetProcAddress(ntdll, "NtOpenFile");
    auto read = GetProcAddress(ntdll, "NtReadFile");
    auto close = GetProcAddress(ntdll, "NtClose");
    if (!create || !open || !read || !close) return FALSE;

    bool createHooked = WindhawkUtils::SetFunctionHook(
        reinterpret_cast<NtCreateFile_t>(create), NtCreateFile_Hook,
        &g_originalNtCreateFile);
    bool openHooked = WindhawkUtils::SetFunctionHook(
        reinterpret_cast<NtOpenFile_t>(open), NtOpenFile_Hook,
        &g_originalNtOpenFile);
    bool readHooked = WindhawkUtils::SetFunctionHook(
        reinterpret_cast<NtReadFile_t>(read), NtReadFile_Hook,
        &g_originalNtReadFile);
    bool closeHooked = WindhawkUtils::SetFunctionHook(
        reinterpret_cast<NtClose_t>(close), NtClose_Hook,
        &g_originalNtClose);
#if defined(_M_X64)
    if (HMODULE module = GetModuleHandleW(L"FileExplorerExtensions.dll")) {
        HookExplorerSymbols(module, false);
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibrary = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW")) : nullptr;
        if (loadLibrary)
            WindhawkUtils::SetFunctionHook(loadLibrary, LoadLibraryExW_Hook,
                                           &g_originalLoadLibraryExW);
    }
#endif
    return createHooked && openHooked && readHooked && closeHooked;
}

void Wh_ModAfterInit() {
#if defined(_M_X64)
    if (!g_explorerSymbolsAttempted.load(std::memory_order_acquire))
        HookExplorerSymbols(GetModuleHandleW(L"FileExplorerExtensions.dll"), true);
#endif
    g_watcherStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_watcherStopEvent) return;
    g_watcherThread = CreateThread(nullptr, 0, AccentWatcherThread,
                                   nullptr, 0, nullptr);
    if (!g_watcherThread) {
        CloseHandle(g_watcherStopEvent);
        g_watcherStopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ScheduleColorRefresh();
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);
    while (auto active = g_symbolSetupActive.load(std::memory_order_acquire))
        g_symbolSetupActive.wait(active);
    if (g_watcherThread) {
        SetEvent(g_watcherStopEvent);
        WaitForSingleObject(g_watcherThread, INFINITE);
        CloseHandle(g_watcherThread);
        g_watcherThread = nullptr;
    }
    if (g_watcherStopEvent) {
        CloseHandle(g_watcherStopEvent);
        g_watcherStopEvent = nullptr;
    }
}

void Wh_ModUninit() {
    // Windhawk removes the NtReadFile hook between BeforeUninit and Uninit.
    // Rebinding now decodes native SVG bytes instead of recolored bytes.
    RestoreNativeSources();
    {
        std::lock_guard lock(g_sourcesMutex);
        for (size_t i = 0; i < g_sourceCount; ++i) g_sources[i] = {};
        g_sourceCount = 0;
    }
    {
        std::lock_guard lock(g_uiThreadsMutex);
        g_uiThreadCount = 0;
    }
}
