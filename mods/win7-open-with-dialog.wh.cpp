// ==WindhawkMod==
// @id              win7-open-with-dialog
// @name            Windows 7 Open With Dialog
// @description     This mod restores the classic Windows 7 "Open with" dialog on Windows 10 and 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         MIT
// @include         explorer.exe
// @include         OpenWith.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshell32 -lshlwapi -lversion -ladvapi32 -lcomctl32 -luxtheme -ldwmapi -luser32 -lgdi32 -luuid -lwinpthread
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Open With Dialog Recreation

This mod restores the classic Windows 7 **Open with** dialog on Windows 10 and
11, replacing (when the mod is active) the modern picker with an accurate recreation of the original Windows 7 one while
keeping the original file and application paths untouched.

The mod has been tested primarily on **Windows 10 21H2** and **Windows 11 25H2**. The public API and
classic context-menu paths are designed to fail safely on unsupported builds.
## Screenshot 

![openwith](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/openwith.PNG)

## Features

- **Windows 7-style dialog**: Recreates the classic layout with recommended and
  other-program groups, Browse, Web search and the Always use checkbox.
- **Open with context menu**: Redirects the canonical `openas` command through
  the stable `CLSID_OpenWithMenu` context-menu contract.
- **Unknown-file double click**: Hooks the real OpenWith.exe COM server methods
  without substituting mod-owned COM objects.
- **Properties → Change**: Recognizes association-only launcher requests and
  selects a default without opening the file.
- **Persistent defaults**: Uses `IAssocHandler::MakeDefault` and can open
  Windows Default Apps if the protected system association rejects the change.
- **Direct selected-program launch**: Uses the selected ProgID/class key or the
  selected executable directly, avoiding recursive re-entry into Open With.
- **Browse application registration**: Programs selected through Browse are
  added to the current user's `Applications`, `SupportedTypes`, `OpenWithList`,
  `MRUList` and `OpenWithProgids` keys and appear immediately in the dialog.
- **Extensionless files**: Enumerates registered applications through a
   `HKCR\Applications` fallback.
- **Localized interface**: English, Italian, Spanish, French, Brazilian
  Portuguese, Turkish, Russian, Simplified Chinese, Dutch and Polish, with
  automatic language detection.
- **Short file display**: Shows `File: photo.png` instead of the full path. The
  full path is retained internally for execution.
- **Custom icon**: Uses the supplied document-and-magnifier artwork, embedded as
  transparent 32×32 BGRA Base64.
- **DPI aware**: Scales the window and controls for the owner monitor.
- **Defensive lifetime management**: Uses RAII for COM pointers, registry keys,
  handles, windows, icons, image lists, fonts and worker-owned dialogs, with
  exception containment around hook boundaries.

## Requirements

- **Windows 10 or Windows 11 x64**
- **Windhawk** with injection enabled for `explorer.exe`, `OpenWith.exe` and
  `rundll32.exe`
- The mod setting **Enable the Windows 7-style picker** must be enabled

## Note

When the user explicitly selects **Always use**, the mod attempts to persist an
association. All other Open With selections open the file once without changing
the default.

## Known limitations

- **One file at a time**: Multiple-selection Open With requests aren't handled.
- **No default for extensionless files**: They can be opened, but there is no
  extension to associate persistently.
- **Composite extensions**: Windows normally associates `archive.tar.gz` by
  `.gz`; the mod follows the same final-suffix behavior.
- **Store/UWP handlers**: Some AppX handlers don't expose a filesystem
  executable. They rely on their registered ProgID and may fail open if the
  registration is incomplete.
- **Protected associations**: Windows can reject changes to a protected
  association; use Windows Default Apps when that happens.

## Credits

- **ReactOS** — Inspiration
- **aubymori** - Inspiration
- **Image supplied by the user** — document-and-magnifier dialog icon.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replaceSystemDialog: true
  $name: Enable the Windows 7-style picker
  $description: When enabled, replace Open With through the stable CLSID_OpenWithMenu context-menu contract, SHOpenWithDialog, the exact openas ShellExecute verb, or detours on the real OpenWith.exe server. When disabled, requests are passed to Windows.
# NOTE: the "darkMode" appearance setting has been temporarily removed.
# The dark-theme implementation is still in the source, kept commented
# out so it can be restored later without digging through git history
# (see the long note above the DarkModeActivation block).
- showWebLink: true
  $name: Show the Web search link
  $description: Show the Windows 7-style Web link. Only the sanitized file extension is sent to the browser search.
- defaultAssociationBehavior: disabled
  $name: Always use checkbox behavior
  $description: The checkbox now uses IAssocHandler::MakeDefault. This setting controls only the fallback used when Windows rejects the association request.
  $options:
    - disabled: Don't open Settings on failure
    - openSettings: Open Windows Default Apps on failure
- language: auto
  $name: Language
  $description: Language used by the recreated picker.
  $options:
    - auto: Automatic
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - de: Deutsch
    - pt-BR: Português (Brasil)
    - pt-PT: Português (Portugal)
    - ru: Русский
    - zh-CN: 简体中文
    - zh-TW: 繁體中文
    - ja: 日本語
    - ko: 한국어
    - tr: Türkçe
    - nl: Nederlands
    - pl: Polski
    - sv: Svenska
    - da: Dansk
    - nb: Norsk
    - fi: Suomi
    - cs: Čeština
    - hu: Magyar
    - el: Ελληνικά
    - ar: العربية
    - he: עברית
    - ro: Română
    - uk: Українська
    - bg: Български
    - sk: Slovenčina
    - hr: Hrvatski
    - id: Bahasa Indonesia
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <objidl.h>
#include <ocidl.h>
#include <oleidl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <uxtheme.h>
#include <dwmapi.h>

// -----------------------------------------------------------------------------
// DARK THEME - TEMPORARILY DISABLED (SETTING REMOVED, CODE KEPT FOR FUTURE USE)
// -----------------------------------------------------------------------------
// The "darkMode" Windhawk setting has been removed for now, but the entire
// dark-theme implementation is intentionally KEPT in this file, commented
// out, instead of being deleted. Why?
//
// 1) Deleting it would be a waste of time. The feature is already written
//    and working: throwing it away means that whoever re-adds it in the
//    future has to re-write, re-understand and re-debug the same logic
//    from scratch. Keeping it commented costs nothing and preserves all
//    the decisions that went into it (colors, theming calls, control
//    styles, checkbox/button handling).
//
// 2) Git history is not a substitute for the code itself. This project
//    has many commits, and recovering the exact revision that contained
//    the dark theme would mean digging through dozens of diffs and stale
//    branches, then manually re-assembling the scattered pieces. Here the
//    code stays in context, next to the very functions it belongs to, so
//    restoring it is a matter of uncommenting blocks, not archaeology.
//
// 3) Commented code is still reviewable code. Anyone reading the source
//    can see how the dark theme was implemented and can already suggest
//    improvements (better contrast, cleaner owner-draw handling,
//    different activation APIs) without having to hunt for an old commit
//    first.
//
// Re-enabling the dark theme later requires:
//   a) restoring the "darkMode" option in the ==WindhawkModSettings==
//      block,
//   b) uncommenting every block marked "DISABLED DARK THEME" below
//      (the DWM defines, DarkModeActivation, ThemeMode/AppsUseDarkTheme,
//      the dark branches in RefreshPickerThemeResources/ApplyPickerTheme,
//      the EnsureAlwaysUseLabel/UpdateButtonHover helpers, the WM_DRAWITEM
//      and hover handlers, and the WM_CTLCOLOR dark branches),
//   c) restoring the original ResolveDarkMode() implementation (it
//      currently returns false and keeps the dialog on the light theme at
//      all times).
//
// Nothing was removed: every dark-mode line is preserved verbatim below.
// -----------------------------------------------------------------------------
// #ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
// #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
// #endif
// #ifndef DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1
// #define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19
// #endif
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <vector>

// -----------------------------------------------------------------------------
// Undocumented dark-mode activation APIs.
//
// SetWindowTheme(hwnd, L"DarkMode_Explorer", ...) alone only recolors a
// handful of surfaces. Without also calling into these ordinal-only uxtheme
// exports, comctl32 keeps drawing ListView group headers, selection
// highlight, WS_EX_CLIENTEDGE borders and scrollbars using their light-theme
// colors, which is why those pieces stay light/illegible even though the
// rest of the dialog is dark.
// -----------------------------------------------------------------------------
// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
// namespace DarkModeActivation {
// enum class AppMode { Default, AllowDark, ForceDark, ForceLight, Max };
// using SetPreferredAppMode_t = AppMode(WINAPI*)(AppMode);
// using FlushMenuThemes_t = void(WINAPI*)();
// using AllowDarkModeForWindow_t = bool(WINAPI*)(HWND, bool);

// static HMODULE g_hUxtheme = nullptr;
// static SetPreferredAppMode_t pSetPreferredAppMode = nullptr;
// static FlushMenuThemes_t pFlushMenuThemes = nullptr;
// static AllowDarkModeForWindow_t pAllowDarkModeForWindow = nullptr;
// static bool g_resolved = false;

// static void Resolve() {
//     if (g_resolved) return;
//     g_resolved = true;
//     g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr,
//                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
//     if (!g_hUxtheme) return;
//     pSetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_t>(
//         GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135)));
//     pFlushMenuThemes = reinterpret_cast<FlushMenuThemes_t>(
//         GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(136)));
//     pAllowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindow_t>(
//         GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(133)));
// }

// Recursively opts a window and every descendant into (or out of) dark
// rendering, then asks it to re-pull its theme. Safe to call repeatedly;
// harmless if the ordinals weren't resolved (e.g. older/locked-down builds).
// static void Apply(HWND window, bool dark) {
//     Resolve();
//     if (!window) return;
//     if (pSetPreferredAppMode)
//         pSetPreferredAppMode(dark ? AppMode::AllowDark : AppMode::Default);
//     if (pAllowDarkModeForWindow) {
//         pAllowDarkModeForWindow(window, dark);
//         for (HWND child = GetWindow(window, GW_CHILD); child;
//              child = GetWindow(child, GW_HWNDNEXT)) {
//             pAllowDarkModeForWindow(child, dark);
//         }
//     }
//     SendMessageW(window, WM_THEMECHANGED, 0, 0);
//     for (HWND child = GetWindow(window, GW_CHILD); child;
//          child = GetWindow(child, GW_HWNDNEXT)) {
//         SendMessageW(child, WM_THEMECHANGED, 0, 0);
//     }
//     if (pFlushMenuThemes) pFlushMenuThemes();
// }
// }  // namespace DarkModeActivation

// -----------------------------------------------------------------------------
// Move-only owners.
// -----------------------------------------------------------------------------

template <typename T>
class ComPtr {
   public:
    ComPtr() = default;
    explicit ComPtr(T* value) : value_(value) {}
    ~ComPtr() { Reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : value_(other.Detach()) {}
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) Reset(other.Detach());
        return *this;
    }
    T* Get() const { return value_; }
    T* operator->() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    T** Put() {
        Reset();
        return &value_;
    }
    T* Detach() {
        T* value = value_;
        value_ = nullptr;
        return value;
    }
    void Reset(T* value = nullptr) {
        if (value_) value_->Release();
        value_ = value;
    }

   private:
    T* value_ = nullptr;
};

class IconOwner {
   public:
    IconOwner() = default;
    explicit IconOwner(HICON value) : value_(value) {}
    ~IconOwner() { Reset(); }
    IconOwner(const IconOwner&) = delete;
    IconOwner& operator=(const IconOwner&) = delete;
    IconOwner(IconOwner&& other) noexcept : value_(other.Detach()) {}
    IconOwner& operator=(IconOwner&& other) noexcept {
        if (this != &other) Reset(other.Detach());
        return *this;
    }
    HICON Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    HICON Detach() {
        HICON value = value_;
        value_ = nullptr;
        return value;
    }
    void Reset(HICON value = nullptr) {
        if (value_) DestroyIcon(value_);
        value_ = value;
    }

   private:
    HICON value_ = nullptr;
};

class ImageListOwner {
   public:
    ~ImageListOwner() { Reset(); }
    ImageListOwner() = default;
    ImageListOwner(const ImageListOwner&) = delete;
    ImageListOwner& operator=(const ImageListOwner&) = delete;
    HIMAGELIST Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HIMAGELIST value = nullptr) {
        if (value_) ImageList_Destroy(value_);
        value_ = value;
    }

   private:
    HIMAGELIST value_ = nullptr;
};

class FontOwner {
   public:
    ~FontOwner() { Reset(); }
    FontOwner() = default;
    FontOwner(const FontOwner&) = delete;
    FontOwner& operator=(const FontOwner&) = delete;
    HFONT Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HFONT value = nullptr) {
        if (value_) DeleteObject(value_);
        value_ = value;
    }

   private:
    HFONT value_ = nullptr;
};

class BrushOwner {
   public:
    ~BrushOwner() { Reset(); }
    BrushOwner() = default;
    BrushOwner(const BrushOwner&) = delete;
    BrushOwner& operator=(const BrushOwner&) = delete;
    HBRUSH Get() const { return value_; }
    explicit operator bool() const { return value_ != nullptr; }
    void Reset(HBRUSH value = nullptr) {
        if (value_) DeleteObject(value_);
        value_ = value;
    }

   private:
    HBRUSH value_ = nullptr;
};

static std::atomic<HWND> g_activeBrowseHwnd{nullptr};
static std::atomic<bool> g_shuttingDown{false};

struct HandleDeleter {
    void operator()(HANDLE handle) const noexcept {
        if (handle && handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    }
};
using WinHandle = std::unique_ptr<
    std::remove_pointer<HANDLE>::type, HandleDeleter>;

class RegKeyOwner {
   public:
    RegKeyOwner() = default;
    explicit RegKeyOwner(HKEY value) : value_(value) {}
    ~RegKeyOwner() { Reset(); }
    RegKeyOwner(const RegKeyOwner&) = delete;
    RegKeyOwner& operator=(const RegKeyOwner&) = delete;
    RegKeyOwner(RegKeyOwner&& other) noexcept : value_(other.Detach()) {}
    RegKeyOwner& operator=(RegKeyOwner&& other) noexcept {
        if (this != &other) Reset(other.Detach());
        return *this;
    }
    HKEY Get() const { return value_; }
    HKEY* Put() {
        Reset();
        return &value_;
    }
    explicit operator bool() const { return value_ != nullptr; }
    HKEY Detach() {
        HKEY value = value_;
        value_ = nullptr;
        return value;
    }
    void Reset(HKEY value = nullptr) {
        if (value_) RegCloseKey(value_);
        value_ = value;
    }

   private:
    HKEY value_ = nullptr;
};

class WindowOwner {
   public:
    WindowOwner() = default;
    explicit WindowOwner(HWND value) : value_(value) {}
    ~WindowOwner() { Reset(); }
    WindowOwner(const WindowOwner&) = delete;
    WindowOwner& operator=(const WindowOwner&) = delete;
    HWND Get() const { return value_; }
    explicit operator bool() const { return value_ && IsWindow(value_); }
    void Reset(HWND value = nullptr) {
        if (value_ && IsWindow(value_)) DestroyWindow(value_);
        value_ = value;
    }

   private:
    HWND value_ = nullptr;
};

class ComApartment {
   public:
    explicit ComApartment(DWORD flags)
        : result_(CoInitializeEx(nullptr, flags)) {}
    ~ComApartment() {
        if (SUCCEEDED(result_)) CoUninitialize();
    }
    HRESULT Result() const { return result_; }
    bool Ready() const { return SUCCEEDED(result_); }

   private:
    HRESULT result_;
};

// -----------------------------------------------------------------------------
// Association interfaces. These are the SDK/Shell association-handler ABI used
// by SHAssocEnumHandlers; explicit local names avoid dependence on header age.
// -----------------------------------------------------------------------------

MIDL_INTERFACE("F04061AC-1659-4A3F-A954-775AA57FC083")
StandaloneAssocHandler : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE GetName(PWSTR* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUIName(PWSTR* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIconLocation(PWSTR* path,
                                                       int* index) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsRecommended() = 0;
    virtual HRESULT STDMETHODCALLTYPE MakeDefault(PCWSTR description) = 0;
    virtual HRESULT STDMETHODCALLTYPE Invoke(IDataObject* dataObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInvoker(IDataObject* dataObject,
                                                     IUnknown** invoker) = 0;
};

MIDL_INTERFACE("973810AE-9599-4B88-9E4D-6EE98C9552DA")
StandaloneEnumAssocHandlers : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG count,
                                           StandaloneAssocHandler** handlers,
                                           ULONG* fetched) = 0;
};

using SHAssocEnumHandlers_t = HRESULT(WINAPI*)(
    PCWSTR extension, DWORD filter, StandaloneEnumAssocHandlers** result);

/*static const GUID kBhidDataObject = {
    0xB8C0BD9F,
    0xED24,
    0x455C,
    {0x83, 0xE6, 0xD5, 0x39, 0x0C, 0x4F, 0xE8, 0xC4}};
*/
// -----------------------------------------------------------------------------
// Settings and complete localization catalog.
// -----------------------------------------------------------------------------

enum LocaleStringId {
    STR_TITLE,
    STR_INSTRUCTION,
    STR_FILE_LABEL,
    STR_RECOMMENDED,
    STR_OTHER,
    STR_DESCRIPTION,
    STR_ALWAYS_USE,
    STR_BROWSE,
    STR_WEB_LINK,
    STR_BROWSE_TITLE,
    STR_PROGRAMS,
    STR_ALL_FILES,
    STR_OK,
    STR_CANCEL,
    STR_NO_HANDLERS,
    STR_OPEN_FAILED,
    STR_COUNT,
};

struct LocalePack {
    LANGID langId;
    PCWSTR strings[STR_COUNT];
};

static const LocalePack g_Locales[] = {
    {0x0409, {  // English
        L"Open with",
        L"Choose the program you want to use to open this file:",
        L"File:",
        L"Recommended Programs",
        L"Other Programs",
        L"Type a description that you want to use for this kind of file:",
        L"&Always use the selected program to open this kind of file",
        L"&Browse...",
        L"If the program you want is not in the list or on your computer, you can <A ID=\"WebSearch\">look for the appropriate program on the Web</A>.",
        L"Open with...",
        L"Programs",
        L"All Files",
        L"OK",
        L"Cancel",
        L"No registered program can open this kind of file.",
        L"The selected program could not open the file.",
    }},
    {0x0410, {  // Italian
        L"Apri con",
        L"Scegliere il programma da utilizzare per aprire il file:",
        L"File:",
        L"Programmi consigliati",
        L"Altri programmi",
        L"Digitare una descrizione da utilizzare per questo tipo di file:",
        L"&Usa sempre il programma selezionato per questo tipo di file",
        L"&Sfoglia...",
        L"Se il programma desiderato non è presente nell'elenco o nel computer, è possibile <A ID=\"WebSearch\">cercare il programma appropriato nel Web</A>.",
        L"Apri con...",
        L"Programmi",
        L"Tutti i file",
        L"OK",
        L"Annulla",
        L"Nessun programma registrato può aprire questo tipo di file.",
        L"Impossibile aprire il file con il programma selezionato.",
    }},
    {0x040A, {  // Spanish
        L"Abrir con",
        L"Elija el programa que desea usar para abrir este archivo:",
        L"Archivo:",
        L"Programas recomendados",
        L"Otros programas",
        L"Escriba una descripción que desee usar para este tipo de archivo:",
        L"&Usar siempre el programa seleccionado para abrir este tipo de archivos",
        L"&Examinar...",
        L"Si el programa que desea no está en la lista o en el equipo, puede <A ID=\"WebSearch\">buscar el programa adecuado en la Web</A>.",
        L"Abrir con...",
        L"Programas",
        L"Todos los archivos",
        L"Aceptar",
        L"Cancelar",
        L"Ningún programa registrado puede abrir este tipo de archivo.",
        L"El programa seleccionado no pudo abrir el archivo.",
    }},
    {0x040C, {  // French
        L"Ouvrir avec",
        L"Choisissez le programme à utiliser pour ouvrir ce fichier :",
        L"Fichier :",
        L"Programmes recommandés",
        L"Autres programmes",
        L"Tapez une description que vous souhaitez utiliser pour ce type de fichier :",
        L"&Toujours utiliser le programme sélectionné pour ouvrir ce type de fichier",
        L"&Parcourir...",
        L"Si le programme souhaité ne figure pas dans la liste ou sur votre ordinateur, vous pouvez <A ID=\"WebSearch\">rechercher le programme approprié sur le Web</A>.",
        L"Ouvrir avec...",
        L"Programmes",
        L"Tous les fichiers",
        L"OK",
        L"Annuler",
        L"Aucun programme enregistré ne peut ouvrir ce type de fichier.",
        L"Le programme sélectionné n'a pas pu ouvrir le fichier.",
    }},
    {0x0407, {  // German
        L"Öffnen mit",
        L"Wählen Sie das Programm aus, das Sie zum Öffnen dieser Datei verwenden möchten:",
        L"Dateiname:",
        L"Empfohlene Programme",
        L"Andere Programme",
        L"Geben Sie eine Beschreibung für diesen Dateityp ein:",
        L"&Dateityp immer mit dem ausgewählten Programm öffnen",
        L"&Durchsuchen...",
        L"Falls das gewünschte Programm nicht aufgeführt ist, können Sie <A ID=\"WebSearch\">im Web nach dem entsprechenden Programm suchen</A>.",
        L"Öffnen mit...",
        L"Programme",
        L"Alle Dateien",
        L"OK",
        L"Abbrechen",
        L"Kein registriertes Programm kann diesen Dateityp öffnen.",
        L"Die Datei konnte nicht mit dem ausgewählten Programm geöffnet werden.",
    }},
    {0x0416, {  // Portuguese (Brazil)
        L"Abrir com",
        L"Escolha o programa que você deseja usar para abrir este arquivo:",
        L"Arquivo:",
        L"Programas Recomendados",
        L"Outros Programas",
        L"Digite uma descrição que você deseja usar para este tipo de arquivo:",
        L"&Sempre usar o programa selecionado para abrir este tipo de arquivo",
        L"&Procurar...",
        L"Se o programa desejado não estiver na lista ou no computador, você pode <A ID=\"WebSearch\">procurar o programa apropriado na Web</A>.",
        L"Abrir com...",
        L"Programas",
        L"Todos os Arquivos",
        L"OK",
        L"Cancelar",
        L"Nenhum programa registrado pode abrir este tipo de arquivo.",
        L"O programa selecionado não pôde abrir o arquivo.",
    }},
    {0x0816, {  // Portuguese (Portugal)
        L"Abrir com",
        L"Escolha o programa que pretende utilizar para abrir este ficheiro:",
        L"Ficheiro:",
        L"Programas Recomendados",
        L"Outros Programas",
        L"Escreva uma descrição que pretende utilizar para este tipo de ficheiro:",
        L"&Utilizar sempre o programa selecionado para abrir este tipo de ficheiro",
        L"&Procurar...",
        L"Se o programa pretendido não constar da lista, pode <A ID=\"WebSearch\">procurar o programa adequado na Web</A>.",
        L"Abrir com...",
        L"Programas",
        L"Todos os Ficheiros",
        L"OK",
        L"Cancelar",
        L"Nenhum programa registado pode abrir este tipo de ficheiro.",
        L"O programa selecionado não conseguiu abrir o ficheiro.",
    }},
    {0x0419, {  // Russian
        L"Выбор программы",
        L"Выберите программу для открытия этого файла:",
        L"Файл:",
        L"Рекомендуемые программы",
        L"Другие программы",
        L"Введите описание для этого типа файлов:",
        L"&Использовать выбранную программу для всех файлов этого типа",
        L"&Обзор...",
        L"Если нужной программы нет в списке, можно <A ID=\"WebSearch\">найти ее в Интернете</A>.",
        L"Выбор программы...",
        L"Программы",
        L"Все файлы",
        L"ОК",
        L"Отмена",
        L"Нет зарегистрированных программ для этого типа файлов.",
        L"Выбранная программа не может открыть этот файл.",
    }},
    {0x0804, {  // Chinese (Simplified)
        L"打开方式",
        L"选择您想用来打开此文件的程序:",
        L"文件:",
        L"推荐的程序",
        L"其他程序",
        L"键入您想用于此类文件的描述:",
        L"始终使用选择的程序打开这种文件(&A)",
        L"浏览(&B)...",
        L"如果您需要的程序不在列表中或计算机上，您可以<A ID=\"WebSearch\">在 Web 上查找适当的程序</A>。",
        L"打开方式...",
        L"程序",
        L"所有文件",
        L"确定",
        L"取消",
        L"没有已注册的程序可以打开此类文件。",
        L"所选程序无法打开该文件。",
    }},
    {0x0404, {  // Chinese (Traditional)
        L"開啟檔案",
        L"選擇要用來開啟此檔案的程式:",
        L"檔案:",
        L"建議的程式",
        L"其他程式",
        L"輸入要用於此類檔案的描述:",
        L"永遠使用選取的程式開啟這種檔案(&A)",
        L"瀏覽(&B)...",
        L"如果要使用的程式不在清單或電腦中，您可以<A ID=\"WebSearch\">在網路上尋找適當的程式</A>。",
        L"開啟檔案...",
        L"程式",
        L"所有檔案",
        L"確定",
        L"取消",
        L"沒有已註冊的程式可以開啟此類檔案。",
        L"選取的程式無法開啟檔案。",
    }},
    {0x0411, {  // Japanese
        L"ファイルを開くプログラムの選択",
        L"このファイルを開くプログラムを選択してください:",
        L"ファイル:",
        L"推奨されたプログラム",
        L"ほかのプログラム",
        L"このファイルの種類に使用する説明を入力してください:",
        L"この種類のファイルを開くときは、いつもこのプログラムを使う(&A)",
        L"参照(&B)...",
        L"使いたいプログラムが一覧にない場合は、<A ID=\"WebSearch\">Web で適切なプログラムを探す</A>ことができます。",
        L"ファイルを開くプログラムの選択...",
        L"プログラム",
        L"すべてのファイル",
        L"OK",
        L"キャンセル",
        L"この種類のファイルを開くことができる登録されたプログラムはありません。",
        L"選択したプログラムでファイルを開くことができませんでした。",
    }},
    {0x0412, {  // Korean
        L"연결 프로그램",
        L"이 파일을 열 때 사용할 프로그램을 선택하십시오:",
        L"파일:",
        L"권장하는 프로그램",
        L"기타 프로그램",
        L"이 파일 형식에 사용할 설명을 입력하십시오:",
        L"이 종류의 파일을 열 때 항상 선택된 프로그램 사용(&A)",
        L"찾아보기(&B)...",
        L"원하는 프로그램이 목록이나 컴퓨터에 없으면 <A ID=\"WebSearch\">웹에서 적절한 프로그램을 검색</A>할 수 있습니다.",
        L"연결 프로그램...",
        L"프로그램",
        L"모든 파일",
        L"확인",
        L"취소",
        L"이 파일 형식을 열 수 있는 등록된 프로그램이 없습니다.",
        L"선택한 프로그램으로 파일을 열 수 없습니다.",
    }},
    {0x041F, {  // Turkish
        L"Birlikte Aç",
        L"Bu dosyayı açmak için kullanmak istediğiniz programı seçin:",
        L"Dosya:",
        L"Önerilen Programlar",
        L"Diğer Programlar",
        L"Bu dosya türü için kullanmak istediğiniz bir açıklama yazın:",
        L"Bu tür dosyaları açmak için &her zaman seçili programı kullan",
        L"&Gözat...",
        L"İstediğiniz program listede veya bilgisayarınızda yoksa, <A ID=\"WebSearch\">Web'de uygun programı arayabilirsiniz</A>.",
        L"Birlikte aç...",
        L"Programlar",
        L"Tüm Dosyalar",
        L"Tamam",
        L"İptal",
        L"Kayıtlı hiçbir program bu tür dosyayı açamaz.",
        L"Seçilen program dosyayı açamadı.",
    }},
    {0x0413, {  // Dutch
        L"Openen met",
        L"Kies het programma dat u wilt gebruiken om dit bestand te openen:",
        L"Bestand:",
        L"Aanbevolen programma's",
        L"Andere programma's",
        L"Typ een beschrijving die u wilt gebruiken voor dit type bestand:",
        L"Dit type bestand &altijd met dit programma openen",
        L"&Bladeren...",
        L"Als het gewenste programma niet in de lijst of op de computer staat, kunt u <A ID=\"WebSearch\">op internet naar het juiste programma zoeken</A>.",
        L"Openen met...",
        L"Programma's",
        L"Alle bestanden",
        L"OK",
        L"Annuleren",
        L"Er is geen geregistreerd programma waarmee dit type bestand kan worden geopend.",
        L"Het geselecteerde programma kan het bestand niet openen.",
    }},
    {0x0415, {  // Polish
        L"Otwórz za pomocą",
        L"Wybierz program, którego chcesz użyć do otwarcia tego pliku:",
        L"Plik:",
        L"Polecane programy",
        L"Inne programy",
        L"Wpisz opis, którego chcesz użyć dla tego typu pliku:",
        L"&Zawsze używaj wybranego programu do otwierania tego typu plików",
        L"&Przeglądaj...",
        L"Jeśli żądanego programu nie ma na liście ani na komputerze, możesz <A ID=\"WebSearch\">poszukać odpowiedniego programu w sieci Web</A>.",
        L"Otwórz za pomocą...",
        L"Programy",
        L"Wszystkie pliki",
        L"OK",
        L"Anuluj",
        L"Żaden zarejestrowany program nie może otworzyć tego typu pliku.",
        L"Wybrany program nie mógł otworzyć pliku.",
    }},
    {0x041D, {  // Swedish
        L"Öppna med",
        L"Välj det program som du vill använda för att öppna den här filen:",
        L"Fil:",
        L"Rekommenderade program",
        L"Andra program",
        L"Ange en beskrivning som du vill använda för den här filtypen:",
        L"Använd &alltid detta program för att öppna den här filtypen",
        L"&Bläddra...",
        L"Om programmet inte finns i listan kan du <A ID=\"WebSearch\">söka efter lämpligt program på webben</A>.",
        L"Öppna med...",
        L"Program",
        L"Alla filer",
        L"OK",
        L"Avbryt",
        L"Det finns inget registrerat program som kan öppna den här filtypen.",
        L"Det markerade programmet kunde inte öppna filen.",
    }},
    {0x0406, {  // Danish
        L"Åbn med",
        L"Vælg det program, du vil bruge til at åbne filen med:",
        L"Fil:",
        L"Anbefalede programmer",
        L"Andre programmer",
        L"Skriv en beskrivelse, du vil bruge til denne type fil:",
        L"Brug &altid det valgte program til at åbne denne type fil",
        L"&Gennemse...",
        L"Hvis programmet ikke er på listen, kan du <A ID=\"WebSearch\">søge efter et program på internettet</A>.",
        L"Åbn med...",
        L"Programmer",
        L"Alle filer",
        L"OK",
        L"Annuller",
        L"Intet registreret program kan åbne denne filtype.",
        L"Det valgte program kunne ikke åbne filen.",
    }},
    {0x0414, {  // Norwegian
        L"Åpne i",
        L"Velg programmet du vil bruke til å åpne denne filen:",
        L"Fil:",
        L"Anbefalte programmer",
        L"Andre programmer",
        L"Skriv inn en beskrivelse du vil bruke for denne filtypen:",
        L"Bruk &alltid det valgte programmet til å åpne denne filtypen",
        L"&Bla gjennom...",
        L"Hvis programmet ikke finnes i listen, kan du <A ID=\"WebSearch\">søke etter et passende program på Internett</A>.",
        L"Åpne i...",
        L"Programmer",
        L"Alle filer",
        L"OK",
        L"Avbryt",
        L"Ingen registrerte programmer kan åpne denne filtypen.",
        L"Det valgte programmet kunne ikke åpne filen.",
    }},
    {0x040B, {  // Finnish
        L"Avaa sovelluksessa",
        L"Valitse sovellus, jolla haluat avata tämän tiedoston:",
        L"Tiedosto:",
        L"Suositellut sovellukset",
        L"Muut sovellukset",
        L"Kirjoita kuvaus, jota haluat käyttää tälle tiedostotyypille:",
        L"Käytä &aina valittua sovellusta tämän tiedostotyypin avaamiseen",
        L"&Selaa...",
        L"Jos haluamasi sovellus ei ole luettelossa, voit <A ID=\"WebSearch\">hakea sopivaa sovellusta verkosta</A>.",
        L"Avaa sovelluksessa...",
        L"Sovellukset",
        L"Kaikki tiedostot",
        L"OK",
        L"Peruuta",
        L"Yksikään rekisteröity sovellus ei voi avata tätä tiedostotyyppiä.",
        L"Valittu sovellus ei voinut avata tiedostoa.",
    }},
    {0x0405, {  // Czech
        L"Otevřít v programu",
        L"Vyberte program, který chcete použít k otevření tohoto souboru:",
        L"Soubor:",
        L"Doporučené programy",
        L"Jiné programy",
        L"Zadejte popis, který chcete použít pro tento typ souboru:",
        L"K otevření tohoto typu souboru &vždycky použít vybraný program",
        L"&Procházet...",
        L"Pokud požadovaný program není v seznamu, můžete <A ID=\"WebSearch\">vyhledat vhodný program na webu</A>.",
        L"Otevřít v programu...",
        L"Programy",
        L"Všechny soubory",
        L"OK",
        L"Storno",
        L"Žádný registrovaný program nemůže otevřít tento typ souboru.",
        L"Vybraný program nemohl soubor otevřít.",
    }},
    {0x040E, {  // Hungarian
        L"Társítás",
        L"Válassza ki a fájl megnyitásához használni kívánt programot:",
        L"Fájl:",
        L"Ajánlott programok",
        L"Egyéb programok",
        L"Írja be a fájltípushoz használni kívánt leírást:",
        L"&Mindig a kijelölt programmal nyissa meg ezt a fájltípust",
        L"&Tallózás...",
        L"Ha a kívánt program nem található a listában, <A ID=\"WebSearch\">megkeresheti a megfelelő programot a weben</A>.",
        L"Társítás...",
        L"Programok",
        L"Minden fájl",
        L"OK",
        L"Mégse",
        L"Nincs regisztrált program a fájltípus megnyitásához.",
        L"A kijelölt program nem tudta megnyitni a fájlt.",
    }},
    {0x0408, {  // Greek
        L"Άνοιγμα με",
        L"Επιλέξτε το πρόγραμμα που θέλετε να χρησιμοποιήσετε για το άνοιγμα αυτού του αρχείου:",
        L"Αρχείο:",
        L"Προτεινόμενα προγράμματα",
        L"Άλλα προγράμματα",
        L"Πληκτρολογήστε μια περιγραφή για αυτόν τον τύπο αρχείου:",
        L"&Να χρησιμοποιείται πάντα το επιλεγμένο πρόγραμμα για αυτόν τον τύπο αρχείου",
        L"&Αναζήτηση...",
        L"Εάν το πρόγραμμα δεν υπάρχει στη λίστα, μπορείτε να <A ID=\"WebSearch\">αναζητήσετε το κατάλληλο πρόγραμμα στο Web</A>.",
        L"Άνοιγμα με...",
        L"Προγράμματα",
        L"Όλα τα αρχεία",
        L"OK",
        L"Άκυρο",
        L"Κανένα καταχωρημένο πρόγραμμα δεν μπορεί να ανοίξει αυτόν τον τύπο αρχείου.",
        L"Δεν ήταν δυνατό το άνοιγμα του αρχείου από το επιλεγμένο πρόγραμμα.",
    }},
    {0x0401, {  // Arabic
        L"فتح باستخدام",
        L"اختر البرنامج الذي تريد استخدامه لفتح هذا الملف:",
        L"الملف:",
        L"البرامج الموصى بها",
        L"برامج أخرى",
        L"اكتب وصفاً تريد استخدامه لهذا النوع من الملفات:",
        L"استخدام البرنامج المحدد &دائماً لفتح هذا النوع من الملفات",
        L"&استعراض...",
        L"إذا لم يكن البرنامج المطلوب في القائمة، يمكنك <A ID=\"WebSearch\">البحث عن البرنامج المناسب على الويب</A>.",
        L"فتح باستخدام...",
        L"البرامج",
        L"كافة الملفات",
        L"موافق",
        L"إلغاء الأمر",
        L"لا يوجد برنامج مسجل يمكنه فتح هذا النوع من الملفات.",
        L"تعذر فتح الملف باستخدام البرنامج المحدد.",
    }},
    {0x040D, {  // Hebrew
        L"פתח באמצעות",
        L"בחר את התוכנית שברצונך להשתמש בה כדי לפתוח קובץ זה:",
        L"קובץ:",
        L"תוכניות מומלצות",
        L"תוכניות אחרות",
        L"הקלד תיאור שברצונך להשתמש בו עבור סוג קובץ זה:",
        L"השתמש &תמיד בתוכנית שנבחרה לפתיחת סוג קובץ זה",
        L"&עיון...",
        L"אם התוכנית הרצויה אינה מופיעה ברשימה, באפשרותך <A ID=\"WebSearch\">לחפש את התוכנית המתאימה באינטרנט</A>.",
        L"פתח באמצעות...",
        L"תוכניות",
        L"כל הקבצים",
        L"אישור",
        L"ביטול",
        L"אין תוכנית רשומה היכולה לפתוח סוג קובץ זה.",
        L"התוכנית שנבחרה לא הצליחה לפתוח את הקובץ.",
    }},
    {0x0418, {  // Romanian
        L"Deschidere cu",
        L"Alegeți programul pe care doriți să îl utilizați pentru a deschide acest fișier:",
        L"Fișier:",
        L"Programe recomandate",
        L"Alte programe",
        L"Tastați o descriere pe care doriți să o utilizați pentru acest tip de fișier:",
        L"Se utilizează &întotdeauna programul selectat pentru acest tip de fișier",
        L"&Răsfoire...",
        L"Dacă programul dorit nu este în listă, puteți <A ID=\"WebSearch\">căuta programul corespunzător pe Web</A>.",
        L"Deschidere cu...",
        L"Programe",
        L"Toate fișierele",
        L"OK",
        L"Anulare",
        L"Niciun program înregistrat nu poate deschide acest tip de fișier.",
        L"Programul selectat nu a putut deschide fișierul.",
    }},
    {0x0422, {  // Ukrainian
        L"Вибір програми",
        L"Виберіть програму для відкриття цього файлу:",
        L"Файл:",
        L"Рекомендовані програми",
        L"Інші програми",
        L"Введіть опис для цього типу файлів:",
        L"&Завжди використовувати вибрану програму для цього типу файлів",
        L"&Огляд...",
        L"Якщо потрібної програми немає у списку, можна <A ID=\"WebSearch\">знайти її в Інтернеті</A>.",
        L"Вибір програми...",
        L"Програми",
        L"Усі файли",
        L"ОК",
        L"Скасувати",
        L"Немає зареєстрованих програм для цього типу файлів.",
        L"Вибрана програма не змогла відкрити файл.",
    }},
    {0x0402, {  // Bulgarian
        L"Отваряне с",
        L"Изберете програмата, която искате да използвате за отваряне на този файл:",
        L"Файл:",
        L"Препоръчани програми",
        L"Други програми",
        L"Въведете описание за този тип файл:",
        L"&Винаги използвай избраната програма за този тип файл",
        L"&Преглед...",
        L"Ако желаната програма не е в списъка, можете да <A ID=\"WebSearch\">поърсите подходяща програма в интернет</A>.",
        L"Отваряне с...",
        L"Програми",
        L"Всички файлове",
        L"ОК",
        L"Отказ",
        L"Няма регистрирана програма за отваряне на този тип файл.",
        L"Избраната програма не може да отвори файла.",
    }},
    {0x041B, {  // Slovak
        L"Otvoriť v programe",
        L"Vyberte program, ktorý chcete použiť na otvorenie tohto súboru:",
        L"Súbor:",
        L"Odporúčané programy",
        L"Iné programy",
        L"Zadajte popis, ktorý chcete použiť pre tento typ súboru:",
        L"Na otvorenie tohto typu súboru &vždy použiť vybratý program",
        L"&Prehľadávať...",
        L"Ak požadovaný program nie je v zozname, môžete <A ID=\"WebSearch\">vyhľadať vhodný program na webe</A>.",
        L"Otvoriť v programe...",
        L"Programy",
        L"Všetky súbory",
        L"OK",
        L"Zrušiť",
        L"Žiadny zaregistrovaný program nemôže otvoriť tento typ súboru.",
        L"Vybratý program nemohol otvoriť súbor.",
    }},
    {0x041A, {  // Croatian
        L"Otvori pomoću",
        L"Odaberite program kojim želite otvoriti ovu datoteku:",
        L"Datoteka:",
        L"Preporučeni programi",
        L"Ostali programi",
        L"Upišite opis koji želite koristiti za ovu vrstu datoteke:",
        L"&Uvijek koristi odabrani program za otvaranje ove vrste datoteka",
        L"&Pregledaj...",
        L"Ako željeni program nije na popisu, možete <A ID=\"WebSearch\">potražiti odgovarajući program na webu</A>.",
        L"Otvori pomoću...",
        L"Programi",
        L"Sve datoteke",
        L"U redu",
        L"Odustani",
        L"Nijedan registrirani program ne može otvoriti ovu vrstu datoteke.",
        L"Odabrani program nije mogao otvoriti datoteku.",
    }},
    {0x0421, {  // Indonesian
        L"Buka dengan",
        L"Pilih program yang ingin Anda gunakan untuk membuka file ini:",
        L"File:",
        L"Program yang Disarankan",
        L"Program Lainnya",
        L"Ketik deskripsi yang ingin Anda gunakan untuk jenis file ini:",
        L"&Selalu gunakan program yang dipilih untuk membuka jenis file ini",
        L"&Telusuri...",
        L"Jika program yang Anda inginkan tidak ada dalam daftar, Anda dapat <A ID=\"WebSearch\">mencari program yang sesuai di Web</A>.",
        L"Buka dengan...",
        L"Program",
        L"Semua File",
        L"OK",
        L"Batal",
        L"Tidak ada program terdaftar yang dapat membuka jenis file ini.",
        L"Program yang dipilih tidak dapat membuka file.",
    }},
};

static std::atomic<const LocalePack*> g_CurrentLocalePack{&g_Locales[0]};
#define LOC(id) (g_CurrentLocalePack.load(std::memory_order_acquire)->strings[id])

enum class DefaultBehavior {
    Disabled,
    OpenSettings,
};

// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
// enum class ThemeMode {
//     Auto,
//     Light,
//     Dark,
// };

static std::atomic<bool> g_replaceSystemDialog{true};
static std::atomic<bool> g_showWebLink{true};
// DISABLED DARK THEME: static std::atomic<ThemeMode> g_themeMode{ThemeMode::Auto};
static std::atomic<DefaultBehavior> g_defaultBehavior{DefaultBehavior::Disabled};

static const LocalePack* TryFindLocalePack(LANGID langId) {
    const LANGID primary = PRIMARYLANGID(langId);
    for (const LocalePack& locale : g_Locales) {
        if (locale.langId == langId) return &locale;
    }
    for (const LocalePack& locale : g_Locales) {
        if (PRIMARYLANGID(locale.langId) == primary) return &locale;
    }
    return nullptr;
}

static const LocalePack* FindLocalePack(LANGID langId) {
    const LocalePack* locale = TryFindLocalePack(langId);
    return locale ? locale : &g_Locales[0];
}

static const LocalePack* LocalePackFromName(PCWSTR localeName) {
    if (!localeName || !*localeName) return nullptr;
    const LCID lcid = LocaleNameToLCID(localeName, 0);
    if (lcid) {
        if (const LocalePack* locale = TryFindLocalePack(LANGIDFROMLCID(lcid))) {
            return locale;
        }
    }
    return nullptr;
}

static const LocalePack* DetectAutomaticLocale() {
    ULONG languageCount = 0;
    ULONG bufferChars = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &languageCount, nullptr, &bufferChars) &&
        bufferChars > 1 && bufferChars < 32768) {
        try {
            std::vector<wchar_t> languages(bufferChars);
            if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &languageCount, languages.data(), &bufferChars)) {
                for (PCWSTR cursor = languages.data(); *cursor; cursor += wcslen(cursor) + 1) {
                    if (const LocalePack* locale = LocalePackFromName(cursor)) {
                        return locale;
                    }
                }
            }
        } catch (...) {}
    }

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
    if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        if (const LocalePack* locale = LocalePackFromName(localeName)) {
            return locale;
        }
    }
    if (const LocalePack* locale = TryFindLocalePack(GetUserDefaultUILanguage())) {
        return locale;
    }
    if (GetSystemDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        if (const LocalePack* locale = LocalePackFromName(localeName)) {
            return locale;
        }
    }
    if (const LocalePack* locale = TryFindLocalePack(GetSystemDefaultUILanguage())) {
        return locale;
    }
    return &g_Locales[0];
}

static std::wstring g_configuredLanguage = L"auto";
static std::mutex g_languageMutex;

static void DetermineLocale() {
    std::wstring requested;
    {
        std::lock_guard<std::mutex> lock(g_languageMutex);
        requested = g_configuredLanguage;
    }

    const LocalePack* selected = nullptr;
    if (!requested.empty() && _wcsicmp(requested.c_str(), L"auto") != 0 && _wcsicmp(requested.c_str(), L"0") != 0) {
        struct LangMapping {
            PCWSTR key;
            LANGID id;
        };
        static const LangMapping kMap[] = {
            {L"en", 0x0409}, {L"english", 0x0409},
            {L"it", 0x0410}, {L"italiano", 0x0410}, {L"italian", 0x0410},
            {L"es", 0x040A}, {L"español", 0x040A}, {L"spanish", 0x040A},
            {L"fr", 0x040C}, {L"français", 0x040C}, {L"french", 0x040C},
            {L"de", 0x0407}, {L"deutsch", 0x0407}, {L"german", 0x0407},
            {L"pt-br", 0x0416}, {L"pt_br", 0x0416}, {L"português (brasil)", 0x0416},
            {L"pt-pt", 0x0816}, {L"pt_pt", 0x0816}, {L"pt", 0x0416}, {L"portuguese", 0x0416},
            {L"ru", 0x0419}, {L"русский", 0x0419}, {L"russian", 0x0419},
            {L"zh-cn", 0x0804}, {L"zh-hans", 0x0804}, {L"zh_cn", 0x0804}, {L"zh", 0x0804}, {L"简体中文", 0x0804},
            {L"zh-tw", 0x0404}, {L"zh-hant", 0x0404}, {L"zh_tw", 0x0404}, {L"繁體中文", 0x0404},
            {L"ja", 0x0411}, {L"日本語", 0x0411}, {L"japanese", 0x0411},
            {L"ko", 0x0412}, {L"한국어", 0x0412}, {L"korean", 0x0412},
            {L"tr", 0x041F}, {L"türkçe", 0x041F}, {L"turkish", 0x041F},
            {L"nl", 0x0413}, {L"nederlands", 0x0413}, {L"dutch", 0x0413},
            {L"pl", 0x0415}, {L"polski", 0x0415}, {L"polish", 0x0415},
            {L"sv", 0x041D}, {L"svenska", 0x041D}, {L"swedish", 0x041D},
            {L"da", 0x0406}, {L"dansk", 0x0406}, {L"danish", 0x0406},
            {L"nb", 0x0414}, {L"no", 0x0414}, {L"norsk", 0x0414}, {L"norwegian", 0x0414},
            {L"fi", 0x040B}, {L"suomi", 0x040B}, {L"finnish", 0x040B},
            {L"cs", 0x0405}, {L"čeština", 0x0405}, {L"czech", 0x0405},
            {L"hu", 0x040E}, {L"magyar", 0x040E}, {L"hungarian", 0x040E},
            {L"el", 0x0408}, {L"ελληνικά", 0x0408}, {L"greek", 0x0408},
            {L"ar", 0x0401}, {L"العربية", 0x0401}, {L"arabic", 0x0401},
            {L"he", 0x040D}, {L"עברית", 0x040D}, {L"hebrew", 0x040D},
            {L"ro", 0x0418}, {L"română", 0x0418}, {L"romanian", 0x0418},
            {L"uk", 0x0422}, {L"українська", 0x0422}, {L"ukrainian", 0x0422},
            {L"bg", 0x0402}, {L"български", 0x0402}, {L"bulgarian", 0x0402},
            {L"sk", 0x041B}, {L"slovenčina", 0x041B}, {L"slovak", 0x041B},
            {L"hr", 0x041A}, {L"hrvatski", 0x041A}, {L"croatian", 0x041A},
            {L"id", 0x0421}, {L"bahasa indonesia", 0x0421}, {L"indonesian", 0x0421}
        };

        for (const auto& entry : kMap) {
            if (!_wcsicmp(requested.c_str(), entry.key)) {
                selected = FindLocalePack(entry.id);
                break;
            }
        }
        if (!selected) {
            for (const auto& entry : kMap) {
                size_t len = wcslen(entry.key);
                if (!_wcsnicmp(requested.c_str(), entry.key, len)) {
                    selected = FindLocalePack(entry.id);
                    break;
                }
            }
        }
    }

    if (!selected) selected = DetectAutomaticLocale();
    if (!selected) selected = &g_Locales[0];
    g_CurrentLocalePack.store(selected, std::memory_order_release);
    Wh_Log(L"Standalone Open With locale: requested=%s selected=%04X userUI=%04X systemUI=%04X",
           requested.c_str(), selected->langId, GetUserDefaultUILanguage(), GetSystemDefaultUILanguage());
}

// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
// static bool AppsUseDarkTheme() {
//     DWORD appsUseLightTheme = 1;
//     DWORD bytes = sizeof(appsUseLightTheme);
//     const LSTATUS status = RegGetValueW(
//         HKEY_CURRENT_USER,
//         L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
//         L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr,
//         &appsUseLightTheme, &bytes);
//     return status == ERROR_SUCCESS && appsUseLightTheme == 0;
// }

// DISABLED DARK THEME - stub that keeps the dialog on the light theme.
// The original implementation is preserved below, kept for future use.
static bool ResolveDarkMode() {
    return false;
    // switch (g_themeMode.load(std::memory_order_acquire)) {
    //     case ThemeMode::Dark:
    //         return true;
    //     case ThemeMode::Light:
    //         return false;
    //     case ThemeMode::Auto:
    //     default:
    //         return AppsUseDarkTheme();
    // }
}

static void LoadSettings() {
    const bool replace = Wh_GetIntSetting(L"replaceSystemDialog") != 0;
    const bool web = Wh_GetIntSetting(L"showWebLink") != 0;

    WindhawkUtils::StringSetting language =
        WindhawkUtils::StringSetting::make(L"language");
    {
        std::lock_guard<std::mutex> lock(g_languageMutex);
        g_configuredLanguage =
            (language.get() && *language.get()) ? language.get() : L"auto";
    }

    // DISABLED DARK THEME - the "darkMode" option was removed from the
    // settings; this block is kept commented for future use:
    // WindhawkUtils::StringSetting darkMode =
    //     WindhawkUtils::StringSetting::make(L"darkMode");
    // ThemeMode theme = ThemeMode::Auto;
    // if (darkMode.get() && !_wcsicmp(darkMode.get(), L"dark")) {
    //     theme = ThemeMode::Dark;
    // } else if (darkMode.get() && !_wcsicmp(darkMode.get(), L"light")) {
    //     theme = ThemeMode::Light;
    // }

    WindhawkUtils::StringSetting defaultBehavior =
        WindhawkUtils::StringSetting::make(L"defaultAssociationBehavior");
    const DefaultBehavior behavior =
        defaultBehavior.get() &&
        !_wcsicmp(defaultBehavior.get(), L"openSettings")
            ? DefaultBehavior::OpenSettings
            : DefaultBehavior::Disabled;

    g_defaultBehavior.store(behavior, std::memory_order_release);
    // g_themeMode.store(theme, std::memory_order_release);  // DISABLED DARK THEME
    g_showWebLink.store(web, std::memory_order_release);
    g_replaceSystemDialog.store(replace, std::memory_order_release);
    DetermineLocale();
}

// -----------------------------------------------------------------------------
// Picker state and association enumeration.
// -----------------------------------------------------------------------------

struct HandlerEntry {
    ComPtr<StandaloneAssocHandler> handler;
    std::wstring displayName;
    std::wstring internalName;
    std::wstring progId;
    bool recommended = false;
    bool browsed = false;
    int imageIndex = -1;
};

struct PickerRequest {
    std::wstring path;
    HWND owner = nullptr;
    // Optional caller-owned event, signaled after the picker request finishes.
    HANDLE completionEvent = nullptr;
    // Properties -> Change selects a default but must not execute the file.
    bool setDefaultOnly = false;
};

struct PickerState {
    PickerRequest request;
    std::vector<HandlerEntry> handlers;
    ImageListOwner images;
    IconOwner headerIcon;
    FontOwner font;
    // Dark background brushes (DISABLED DARK THEME - kept for future use):
    // BrushOwner darkBgBrush;
    // BrushOwner darkCardBrush;
    HWND window = nullptr;
    HWND list = nullptr;
    HWND description = nullptr;
    HWND alwaysUse = nullptr;
    // Dark-mode checkbox label companion and owner-draw hover tracking
    // (DISABLED DARK THEME - kept for future use):
    // HWND alwaysUseLabel = nullptr;
    // int hoverButton = 0;
    bool finished = false;
    bool accepted = false;
    bool makeDefaultRequested = false;
    std::wstring associationDescription;
    bool openDefaultSettings = false;
    bool listUsesGroups = false;
    bool hasOtherGroup = false;
    // Always false while the dark theme is disabled (see ResolveDarkMode).
    bool isDarkMode = false;
    int chosenIndex = -1;
};

static SHAssocEnumHandlers_t ResolveHandlerEnumerator() {
    static SHAssocEnumHandlers_t function = [] {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        return shell32 ? reinterpret_cast<SHAssocEnumHandlers_t>(
                             GetProcAddress(shell32, "SHAssocEnumHandlers"))
                       : nullptr;
    }();
    return function;
}

static std::wstring TakeTaskString(PWSTR value) {
    if (!value) return {};
    std::wstring result;
    try { result = value; } catch (...) {}
    CoTaskMemFree(value);
    return result;
}

static std::wstring ExtensionOf(const std::wstring& path) {
    const wchar_t* fileName = PathFindFileNameW(path.c_str());
    const wchar_t* extension = fileName ? PathFindExtensionW(fileName) : nullptr;
    return extension ? extension : L"";
}

static bool IsSupportedFile(const std::wstring& path) {
    if (path.empty() || path.size() >= 32767) return false;
    const DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

static std::wstring ApplicationProgIdForExecutable(
    const std::wstring& executable) {
    PCWSTR name = PathFindFileNameW(executable.c_str());
    if (!name || !*name) return {};
    try {
        return std::wstring(L"Applications\\") + name;
    } catch (...) {
        return {};
    }
}

static std::wstring ResolveHandlerProgId(const std::wstring& internalName) {
    if (internalName.empty()) return {};
    if (!_wcsnicmp(internalName.c_str(), L"Applications\\", 13) ||
        !_wcsnicmp(internalName.c_str(), L"AppX", 4)) {
        return internalName;
    }
    if (GetFileAttributesW(internalName.c_str()) != INVALID_FILE_ATTRIBUTES)
        return ApplicationProgIdForExecutable(internalName);

    // Some handlers expose a bare executable name.
    if (PathFindExtensionW(internalName.c_str()) &&
        !_wcsicmp(PathFindExtensionW(internalName.c_str()), L".exe")) {
        try {
            return std::wstring(L"Applications\\") +
                   PathFindFileNameW(internalName.c_str());
        } catch (...) {
        }
    }
    // Otherwise GetName commonly returned the ProgID itself.
    return internalName;
}

static HRESULT AddExecutableToOpenWithMru(PCWSTR extension,
                                          PCWSTR executableName) {
    if (!extension || extension[0] != L'.' || !extension[1] ||
        !executableName || !*executableName) {
        return E_INVALIDARG;
    }
    wchar_t keyPath[1024] = {};
    if (swprintf_s(
            keyPath,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
            L"FileExts\\%s\\OpenWithList",
            extension) < 0) {
        return E_FAIL;
    }

    RegKeyOwner key;
    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER, keyPath, 0, nullptr, 0, KEY_READ | KEY_WRITE,
        nullptr, key.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    wchar_t mru[64] = {};
    DWORD bytes = sizeof(mru);
    RegGetValueW(key.Get(), nullptr, L"MRUList", RRF_RT_REG_SZ, nullptr,
                 mru, &bytes);

    wchar_t selectedLetter = 0;
    for (wchar_t letter = L'a'; letter <= L'z'; ++letter) {
        wchar_t name[2] = {letter, 0};
        wchar_t value[MAX_PATH] = {};
        bytes = sizeof(value);
        if (RegGetValueW(key.Get(), nullptr, name, RRF_RT_REG_SZ, nullptr,
                         value, &bytes) == ERROR_SUCCESS) {
            if (!_wcsicmp(value, executableName)) {
                selectedLetter = letter;
                break;
            }
        } else if (!selectedLetter) {
            selectedLetter = letter;
        }
    }
    if (!selectedLetter) return HRESULT_FROM_WIN32(ERROR_TOO_MANY_NAMES);

    wchar_t valueName[2] = {selectedLetter, 0};
    status = RegSetValueExW(
        key.Get(), valueName, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(executableName),
        static_cast<DWORD>((wcslen(executableName) + 1) * sizeof(wchar_t)));
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    std::wstring newMru(1, selectedLetter);
    for (PCWSTR cursor = mru; *cursor; ++cursor) {
        if (*cursor != selectedLetter) newMru.push_back(*cursor);
    }
    status = RegSetValueExW(
        key.Get(), L"MRUList", 0, REG_SZ,
        reinterpret_cast<const BYTE*>(newMru.c_str()),
        static_cast<DWORD>((newMru.size() + 1) * sizeof(wchar_t)));
    return HRESULT_FROM_WIN32(status);
}

static HRESULT EnsureUserApplicationRegistration(
    const std::wstring& executable, const std::wstring& extension,
    std::wstring* progIdOut) {
    if (!IsSupportedFile(executable)) return E_INVALIDARG;
    PCWSTR executableName = PathFindFileNameW(executable.c_str());
    if (!executableName || !*executableName) return E_INVALIDARG;

    std::wstring appPath;
    try {
        appPath = std::wstring(L"Software\\Classes\\Applications\\") +
                  executableName;
    } catch (...) {
        return E_OUTOFMEMORY;
    }

    RegKeyOwner appKey;
    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER, appPath.c_str(), 0, nullptr, 0,
        KEY_READ | KEY_WRITE, nullptr, appKey.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    RegKeyOwner commandKey;
    status = RegCreateKeyExW(appKey.Get(), L"shell\\open\\command", 0,
                             nullptr, 0, KEY_READ | KEY_WRITE, nullptr,
                             commandKey.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    std::wstring command;
    try {
        command = L"\"" + executable + L"\" \"%1\"";
    } catch (...) {
        return E_OUTOFMEMORY;
    }
    status = RegSetValueExW(
        commandKey.Get(), nullptr, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(command.c_str()),
        static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    if (extension.size() > 1 && extension[0] == L'.') {
        RegKeyOwner supportedTypes;
        if (RegCreateKeyExW(appKey.Get(), L"SupportedTypes", 0, nullptr, 0,
                            KEY_READ | KEY_WRITE, nullptr,
                            supportedTypes.Put(), nullptr) == ERROR_SUCCESS) {
            const wchar_t empty[] = L"";
            RegSetValueExW(supportedTypes.Get(), extension.c_str(), 0,
                           REG_SZ, reinterpret_cast<const BYTE*>(empty),
                           sizeof(empty));
        }
        AddExecutableToOpenWithMru(extension.c_str(), executableName);

        std::wstring progId = ApplicationProgIdForExecutable(executable);
        wchar_t progIdsPath[1024] = {};
        if (!progId.empty() &&
            swprintf_s(
                progIdsPath,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                L"FileExts\\%s\\OpenWithProgids",
                extension.c_str()) >= 0) {
            RegKeyOwner progIds;
            if (RegCreateKeyExW(HKEY_CURRENT_USER, progIdsPath, 0, nullptr,
                                0, KEY_READ | KEY_WRITE, nullptr,
                                progIds.Put(), nullptr) == ERROR_SUCCESS) {
                RegSetValueExW(progIds.Get(), progId.c_str(), 0, REG_NONE,
                               nullptr, 0);
            }
        }
    }

    if (progIdOut)
        *progIdOut = ApplicationProgIdForExecutable(executable);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

static bool HandlerExecutableExists(const std::wstring& executable, const std::wstring& progId) {
    if (!executable.empty()) {
        if (GetFileAttributesW(executable.c_str()) != INVALID_FILE_ATTRIBUTES) return true;
        wchar_t found[MAX_PATH] = {};
        if (SearchPathW(nullptr, executable.c_str(), L".exe", ARRAYSIZE(found), found, nullptr)) return true;
    }
    if (!progId.empty()) {
        wchar_t subKey[512] = {};
        if (swprintf_s(subKey, L"%s\\shell\\open\\command", progId.c_str()) > 0) {
            HKEY key = nullptr;
            if (RegOpenKeyExW(HKEY_CLASSES_ROOT, subKey, 0, KEY_READ, &key) == ERROR_SUCCESS) {
                RegCloseKey(key);
                return true;
            }
        }
    }
    return false;
}

static bool IsOpenWithExecutable(const std::wstring& executable) {
    if (executable.empty()) return false;
    PCWSTR fileName = PathFindFileNameW(executable.c_str());
    return fileName && !_wcsicmp(fileName, L"OpenWith.exe");
}

static bool IsOpenWithHandlerName(const std::wstring& internalName,
                                  const std::wstring& progId) {
    if (IsOpenWithExecutable(internalName)) return true;
    if (!_wcsicmp(progId.c_str(), L"Applications\\OpenWith.exe")) return true;
    return StrStrIW(internalName.c_str(), L"\\OpenWith.exe") != nullptr;
}

static bool RegistryValueExists(HKEY key, PCWSTR name) {
    return key && RegQueryValueExW(key, name, nullptr, nullptr, nullptr,
                                   nullptr) == ERROR_SUCCESS;
}

static std::wstring ExecutableFromCommand(PCWSTR command) {
    if (!command || !*command) return {};
    DWORD required = ExpandEnvironmentStringsW(command, nullptr, 0);
    std::wstring expanded;
    try {
        if (required > 1 && required < 32768) {
            std::vector<wchar_t> buffer(required);
            if (ExpandEnvironmentStringsW(command, buffer.data(), required))
                expanded.assign(buffer.data());
        }
        if (expanded.empty()) expanded.assign(command);
    } catch (...) {
        return {};
    }

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(expanded.c_str(), &argc);
    if (!argv || argc < 1) {
        if (argv) LocalFree(argv);
        return {};
    }
    std::wstring executable;
    try {
        executable.assign(argv[0]);
    } catch (...) {
    }
    LocalFree(argv);
    if (executable.empty()) return {};

    if (GetFileAttributesW(executable.c_str()) == INVALID_FILE_ATTRIBUTES) {
        wchar_t resolved[MAX_PATH] = {};
        if (SearchPathW(nullptr, executable.c_str(), nullptr,
                        ARRAYSIZE(resolved), resolved, nullptr)) {
            executable.assign(resolved);
        }
    }
    return GetFileAttributesW(executable.c_str()) != INVALID_FILE_ATTRIBUTES
               ? executable
               : std::wstring{};
}

static std::wstring ApplicationDisplayName(HKEY applicationKey,
                                            PCWSTR executableName,
                                            const std::wstring& executable) {
    wchar_t friendly[512] = {};
    DWORD bytes = sizeof(friendly);
    if (applicationKey &&
        RegGetValueW(applicationKey, nullptr, L"FriendlyAppName",
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                     friendly, &bytes) == ERROR_SUCCESS && friendly[0]) {
        return friendly;
    }

    DWORD ignored = 0;
    const DWORD versionBytes =
        GetFileVersionInfoSizeW(executable.c_str(), &ignored);
    if (versionBytes && versionBytes < 16 * 1024 * 1024) {
        try {
            std::vector<BYTE> version(versionBytes);
            if (GetFileVersionInfoW(executable.c_str(), 0, versionBytes,
                                    version.data())) {
                struct Translation { WORD language; WORD codePage; };
                Translation* translations = nullptr;
                UINT translationBytes = 0;
                if (VerQueryValueW(version.data(),
                                   L"\\VarFileInfo\\Translation",
                                   reinterpret_cast<void**>(&translations),
                                   &translationBytes) &&
                    translationBytes >= sizeof(Translation)) {
                    wchar_t query[128] = {};
                    swprintf_s(query,
                               L"\\StringFileInfo\\%04x%04x\\FileDescription",
                               translations[0].language,
                               translations[0].codePage);
                    PWSTR description = nullptr;
                    UINT chars = 0;
                    if (VerQueryValueW(version.data(), query,
                                       reinterpret_cast<void**>(&description),
                                       &chars) && description && chars > 1) {
                        return std::wstring(description, chars - 1);
                    }
                }
            }
        } catch (...) {
        }
    }

    std::wstring display = executableName ? executableName : L"";
    if (display.size() > 4 &&
        !_wcsicmp(display.c_str() + display.size() - 4, L".exe")) {
        display.resize(display.size() - 4);
    }
    return display;
}

// ReactOS first enumerates HKCR\\Applications, then marks extension-specific
// entries as recommended. This minimal fallback is especially important for a
// file with no extension, for which SHAssocEnumHandlers can return no object.
static bool EnumerateRegistryApplications(PickerState& state) {
    RegKeyOwner applications;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"Applications", 0, KEY_READ,
                      applications.Put()) != ERROR_SUCCESS) {
        return false;
    }

    DWORD index = 0;
    wchar_t executableName[512] = {};
    for (;;) {
        DWORD nameChars = ARRAYSIZE(executableName);
        const LSTATUS enumeration = RegEnumKeyExW(
            applications.Get(), index++, executableName, &nameChars, nullptr,
            nullptr, nullptr, nullptr);
        if (enumeration == ERROR_NO_MORE_ITEMS) break;
        if (enumeration != ERROR_SUCCESS) continue;

        RegKeyOwner application;
        if (RegOpenKeyExW(applications.Get(), executableName, 0, KEY_READ,
                          application.Put()) != ERROR_SUCCESS) {
            continue;
        }
        const bool hidden =
            RegistryValueExists(application.Get(), L"NoOpenWith") ||
            RegistryValueExists(application.Get(), L"NoStartPage");
        if (hidden) continue;

        wchar_t command[32768] = {};
        DWORD bytes = sizeof(command);
        if (RegGetValueW(application.Get(), L"shell\\open\\command", nullptr,
                         RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                         command, &bytes) != ERROR_SUCCESS) {
            continue;
        }
        std::wstring executable = ExecutableFromCommand(command);
        if (executable.empty() || IsOpenWithExecutable(executable)) continue;

        bool duplicate = false;
        for (const HandlerEntry& existing : state.handlers) {
            if (!existing.internalName.empty() &&
                !_wcsicmp(existing.internalName.c_str(), executable.c_str())) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate && HandlerExecutableExists(executable, ApplicationProgIdForExecutable(executable))) {
            HandlerEntry entry;
            entry.browsed = true;
            entry.internalName = executable;
            entry.progId = ApplicationProgIdForExecutable(executable);
            entry.displayName = ApplicationDisplayName(
                application.Get(), executableName, executable);
            if (!entry.displayName.empty())
                state.handlers.push_back(std::move(entry));
        }
    }

    std::stable_sort(state.handlers.begin(), state.handlers.end(),
        [](const HandlerEntry& a, const HandlerEntry& b) {
            if (a.recommended != b.recommended)
                return a.recommended > b.recommended;
            return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
        });
    Wh_Log(L"Standalone Open With: registry application fallback count=%u",
           static_cast<unsigned int>(state.handlers.size()));
    return !state.handlers.empty();
}

static bool EnumerateHandlers(PickerState& state) {
    SHAssocEnumHandlers_t enumerate = ResolveHandlerEnumerator();
    ComPtr<StandaloneEnumAssocHandlers> e;
    if (enumerate) {
        enumerate(ExtensionOf(state.request.path).c_str(), 0, e.Put());
    }

    while (e) {
        StandaloneAssocHandler* raw = nullptr;
        ULONG fetched = 0;
        if (e->Next(1, &raw, &fetched) != S_OK || fetched != 1 || !raw) break;
        HandlerEntry entry;
        entry.handler.Reset(raw);
        PWSTR value = nullptr;
        if (SUCCEEDED(entry.handler->GetUIName(&value)))
            entry.displayName = TakeTaskString(value);
        else if (value) CoTaskMemFree(value);
        value = nullptr;
        if (SUCCEEDED(entry.handler->GetName(&value)))
            entry.internalName = TakeTaskString(value);
        else if (value) CoTaskMemFree(value);
        if (entry.displayName.empty()) entry.displayName = entry.internalName;
        if (entry.displayName.empty()) continue;
        entry.progId = ResolveHandlerProgId(entry.internalName);
        if (IsOpenWithHandlerName(entry.internalName, entry.progId))
            continue;
        if (!HandlerExecutableExists(entry.internalName, entry.progId))
            continue;
        entry.recommended = entry.handler->IsRecommended() == S_OK;
        state.handlers.push_back(std::move(entry));
    }

    std::stable_sort(state.handlers.begin(), state.handlers.end(),
        [](const HandlerEntry& a, const HandlerEntry& b) {
            if (a.recommended != b.recommended) return a.recommended > b.recommended;
            return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
        });

    if (state.handlers.empty() || ExtensionOf(state.request.path).empty())
        EnumerateRegistryApplications(state);
    return !state.handlers.empty();
}

static HICON DefaultAppIcon() {
    HICON shared = LoadIconW(nullptr, IDI_APPLICATION);
    return shared ? CopyIcon(shared) : nullptr;
}

static HICON EntryIcon(const HandlerEntry& entry) {
    if (entry.handler) {
        PWSTR raw = nullptr;
        int index = 0;
        if (SUCCEEDED(entry.handler->GetIconLocation(&raw, &index)) && raw &&
            *raw && *raw != L'@') {
            const std::wstring location = TakeTaskString(raw);
            HICON icon = nullptr;
            if (ExtractIconExW(location.c_str(), index, &icon, nullptr, 1) > 0 && icon)
                return icon;
        } else if (raw) {
            CoTaskMemFree(raw);
        }
    }
    if (!entry.internalName.empty()) {
        SHFILEINFOW info{};
        UINT flags = SHGFI_ICON | SHGFI_LARGEICON;
        DWORD attributes = GetFileAttributesW(entry.internalName.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            attributes = FILE_ATTRIBUTE_NORMAL;
            flags |= SHGFI_USEFILEATTRIBUTES;
        }
        if (SHGetFileInfoW(entry.internalName.c_str(), attributes, &info,
                           sizeof(info), flags) && info.hIcon)
            return info.hIcon;
    }
    return DefaultAppIcon();
}


// 32x32 BGRA artwork derived from the user-supplied transparent PNG
// (document + magnifier), Lanczos-resampled at build time and encoded as raw
// top-down BGRA Base64 so the mod remains a single source file.
static const char kStandaloneIconBase64[] =
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAQAAAAEAAAABAAAAAQAAAAEAAAABAAAAAQAAAAEAAAABAAAA"
    "AQAAAAEAAAABAAAAAgAAAAAAAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKqq"
    "qgMAAAAAqKSgQdPRztTR0M3Qz87M0s/NzNLOzcrSzszJ0s3KydLMycfTzMnH0szJxdLJx8TTyMbD08vIxdHIxMHTurWzsoiIiA8A"
    "AAAAf39/AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAv7+/BAAAAACppqNZ////////////////"
    "//////////////////////////////////////////////////////r49v/k39z/zcbCw1xcRQsBAQEAf39/AgAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC/v38EAAAAAKilolP49/b9////+fr6+vz7+/z8+/v8/Pv7+/z7+/v7+/r5+/v6"
    "+fv7+fj7+/n3+/r49vv8+vn78O3r+9bT0fjz7ur/xL+6vT8fHwj///8Bf39/AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAL9/fwQAAAAApqOgVPj39//////7+/v7/vz8/P77+/v+/Pv7/fz7+f38+/n9/Pr5/fv5+P37+ff9+vj2/f37+f3w7ev9"
    "2tfU/fj18vjw6ub/xsG8tQAAAAX///8Bf39/AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAv39/BAAAAACjoJ1U+Pf3"
    "//////v7+/v+/Pz8/fz7+v38+/r9/Pv5/fz6+P37+fj9+/n3/fr49v36+PX9/Pn3/fDt6v3b2NX9//z6/fXx7/jx7Of/w725sAAA"
    "AAP///8B////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC/f38EAAAAAKKfnFX49/b/////+/v7/P38+/r9/Pv5/fz6+f38"
    "+fj9+/n4/fv59/37+ff9+/j2/fr39f37+fb97+zq/dvY1f3////9//37/fr39fj28Oz/xL66qlVVVQP///8CAAAAAQAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAo52aVPf29v/////6+/r5/fz7+f38+vn9+/n3/fv5+P37+ff9+/j2/fr39f359/X9+fb0"
    "/fr29P318vD90s7K/dLNyf3Tzsr90s3J/dDLx/jZ0sz/o56ZlAAAAACqqqoDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAf39/BAAA"
    "AACgnZdU9vX0//////r7+vj9+/n4/fz6+P3//Pr9/fv5/fz69/37+PX9+/j1/fr39f349fP9+PXy/fn18/3z7+397Onm/e3p5v3s"
    "6Ob97ern+/Xx7/2ppKHUAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/f38EAAAAAJ+ZllX29PP////9+vr49/3//fv9"
    "+vj4/evp6f3l4d795d/a/ejk4/3r6uv98/Du/fr28/349fL99/Tx/fn18/379/X9+vf0/fr39P369/T7//36/6ynpMoAAAAA////"
    "AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAnJmWVfb08v////36//37/eXi4f3Jv7b93MSp/enJof3ryJz96ceb/d/E"
    "o/3c08v96ebm/ffz7/349fL99/Tx/ff08f339PH99/Tx/fj08vv++vf/qqWizQAAAAD///8BAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAf39/BAAAAACdmpRU9fPx//////rV0dD9uqiU/e7Qqv3/58n9/erS/f3q1P3858/9+d27/fDNpP3XyLj93trZ/fn08f338/H9"
    "9/Tx/ff08f339PH99/Tx+/369/+qpaHNAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/f38EAAAAAJmWk1X49fP/7evr"
    "+ruplP321Kz9//Xm/fv6+P37+PP9++7f/fvo0/387t79/e7e/fLWs/3YzcD96OTi/fj08f328vD99/Tx/fbz8P339PH6/fn2/6um"
    "oswAAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAnJmWVfLv7v/d1c766cWY/f/u2v36+vf9/Pn1/fzx5P37"
    "58/9++nU/fvs2v377+L9//Pl/erStf3NyMT98+/s/ff08f328u/99fLv/ff08fr9+fb/q6aizAAAAAD///8BAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAf39/BAAAAACmoJpU4uDg/8myl/r31a79/PHk/fvt3P3769n9++TJ/fvn0P377Nr9++/g/fvx5f389/H9+uXO"
    "/b2xpP3f3Nr9+vbz/fXx7/z18u/89/Pw+vz49f+qpaHNAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/f38EAAAAAK2m"
    "oFTOy8r/xaaB+v/jxP3669j9+uTL/frjyP3758/9++rW/fvt3f378OP9+/Pp/fv49P3/8N39uqqX/cnGxf38+PX99PDt/fXx7v32"
    "8u/7/Pf0/6qloM0AAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAqKWfVcrGxf/JqYP6/+PD/frlzf365Mv9"
    "++bP/fvp1f377Nr9++7g/fvx5f379Oz9+/v8/f/v2/2un439v7y6/f359v3z7+z89fHt/fbx7vv89/T/qKOgzAAAAAD///8BAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAf39/BAAAAACjnZdU29nZ/9q+oPr63Ln9++bP/fvmz/366dX9++zb/fvu4P378eT9+/Po/fz4"
    "9P38+ff9++TL/ZKIfP3Fwr/9+/j1/fPu6/318Oz99fDt+/r28v+oo57MAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB/"
    "f38EAAAAAKCal1Tm4+L/z7+u+vDNpP3969b9+unV/fvt3f378OL9+/Lm/fvz6f389/H9+fn6/f/67/3Wwaj9fHhz/eDc2P349PH9"
    "9O/r/fPv6/307+z7+vXx/6mkn80AAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwQAAAAAl5SRVPDs6f/Hwr/62r6e"
    "/f3kyP368OP9+u/i/fvy5/379Ov9+/fy/fv7+/3/+O/99OLN/Y+Gev2gnJb98u/t/fTv6/3z7uv98+7q/fTv7Pr69fH/qKKezAAA"
    "AAD///8BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAf39/BAAAAACUkYtU9PHv/+nl4vrQycP98Na6/f/t1/3/+e/9//jx/f338f3/"
    "9+/9/+/e/e7Yvv2jmIz9jomC/dnW0v318e798+7q/fPu6v3y7en99O/r+vn07/+nop3MAAAAAP///wEAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAB/f38EAAAAAZmWkFX18u//497Y+ru2s/2oop39xrWj/d/KtP3038X99uHH/fDbwv3cy7j9rKSb/ZaQi/3LyMX98/Dt"
    "/fPv6/3z7+v98+7q/fLt6f3z7un7+fTv/6einc0AAAAA////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGZmZgUAAAAAjImCUNHL"
    "xv/Z1NH6eHJs/YB4cP2rpaH9kol+/aKXi/21q5/9vLOq/bOspP2xq6T919TR/fTw7f307+z99O/r/fTv6/3z7ur98u3p/fPt6fv4"
    "8u7/pqGczQAAAAD///8BAAAAAAAAAAAAAAAAAAAAAAAAAAD///8B////AhwAAAmhm5TCzMfE/oN8ePt6cmf94d/e/fDt6/3a1tP9"
    "xsK//cK+uv3Hw7791tLP/evp5/328u/99O/s/fPu6v3z7ur98u3p/fPt6f3y7Of98ezn+/nz7v+moZzNAAAAAP///wEAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAaqqqgMAAAAAmJGJrtbT0P+GhYT6PjYt/bGtqP308vD99/Pw/fn28/359vP9+PXz/fj18/359fL89vHt/fTv"
    "7P3z7ur98+7q/fLu6v3y7en98ezn/fHr5v3x6+b79/Ht/6ehncwAAAAA////AQAAAAAAAAAAAAAAAAAAAAC/v78EAAAAAI2Gf5LQ"
    "zcr/lJOT+EQ+Nv2BdGb91dPS/ff08f318u/99vLv/fby7v328e799fDs/fXw7P307+v98+7q/fPu6v3y7en98u3o/fHr5/3x6+b9"
    "8evm/fLs5/r38uz/pqGczAAAAAD///8BAAAAAAAAAAAAAAAAqlVVAwAAAACFfnVzx8PA/6SjovdHQz38eGxa/cPAv/zw7ev89fHu"
    "/PTw7f318e789PDt/PTv6/z07+v88+7q/PLt6fzy7en88ezn/PDr5vzw6uX88Orl/PDq5fzw6uT87+nk+/bw6v6noZzMAAAAAP//"
    "/wEAAAAAAAAAAH9/fwIAAAAAioF1Vbeyrv+urKz5UExI/2pdS/++u7f98vDv/vv49P78+PX9/Pj1/fz49f38+PT9+/fz/vr28v76"
    "9fD++vXw/vj07/738u3+9/Hs/vfx7P738ez+9/Hr/vfx6/748ev8//nz/6mkn80AAAAA////AQAAAAAAAAAA////AQAAAACGem/c"
    "xLu1/2NeWv5fUj/ihXtvfLi0sv3EwLv7wr66/MG9ufzBvbn8wby4/MG8uPy/urb8wLu2/L+6tvy/urb8vrm1/L+5tfy+uLP8vriz"
    "/L24s/y+uLP8vriz/L24s/rCvbf/l5KMyAAAAAD///8BAAAAAAAAAABVVVUDAAAAAHJiVFtqW0z5bl9L+YNuWTwAAAAAZ2FVKlVO"
    "RydVTkcnVU5HJ1VORydVTkcnVU5HJ1VORydVTkcnTkdHJ05ORydVTkcnVU5HJ05HRydOR0cnTkdHJ05HRydVTkcnTkdHJ1JMRihn"
    "Xl4bAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAEY4IyRdSjcpAAAAAP8AAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAB/f38CAAAAAAAAAAB/f38CAAAAAH9/fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/"
    "fwJ/f38Cf39/An9/fwJ/f38Cf39/An9/fwJ/f38Cf39/AgAAAAAAAAAAAAAAAA==";

static int Base64Digit(unsigned char value) {
    if (value >= 'A' && value <= 'Z') return value - 'A';
    if (value >= 'a' && value <= 'z') return value - 'a' + 26;
    if (value >= '0' && value <= '9') return value - '0' + 52;
    if (value == '+') return 62;
    if (value == '/') return 63;
    return -1;
}

static bool DecodeBase64(PCSTR encoded, std::vector<BYTE>& output) {
    output.clear();
    if (!encoded) return false;
    try {
        output.reserve(4096);
        unsigned int accumulator = 0;
        int bits = 0;
        for (const unsigned char* cursor =
                 reinterpret_cast<const unsigned char*>(encoded);
             *cursor; ++cursor) {
            if (*cursor == '=') break;
            const int digit = Base64Digit(*cursor);
            if (digit < 0) {
                if (*cursor == ' ' || *cursor == '\r' || *cursor == '\n' ||
                    *cursor == '\t') {
                    continue;
                }
                return false;
            }
            accumulator = (accumulator << 6) | static_cast<unsigned int>(digit);
            bits += 6;
            if (bits >= 8) {
                bits -= 8;
                output.push_back(static_cast<BYTE>(accumulator >> bits));
                accumulator &= (1u << bits) - 1u;
            }
        }
    } catch (...) {
        output.clear();
        return false;
    }
    return output.size() == 32u * 32u * 4u;
}

static HICON LoadStandaloneIcon() {
    std::vector<BYTE> pixels;
    if (!DecodeBase64(kStandaloneIconBase64, pixels)) return DefaultAppIcon();

    BITMAPV5HEADER header{};
    header.bV5Size = sizeof(header);
    header.bV5Width = 32;
    header.bV5Height = -32;  // top-down, matching the embedded byte order
    header.bV5Planes = 1;
    header.bV5BitCount = 32;
    header.bV5Compression = BI_BITFIELDS;
    header.bV5RedMask = 0x00FF0000;
    header.bV5GreenMask = 0x0000FF00;
    header.bV5BlueMask = 0x000000FF;
    header.bV5AlphaMask = 0xFF000000;

    void* colorBits = nullptr;
    HDC screen = GetDC(nullptr);
    HBITMAP color = screen
        ? CreateDIBSection(screen, reinterpret_cast<BITMAPINFO*>(&header),
                           DIB_RGB_COLORS, &colorBits, nullptr, 0)
        : nullptr;
    if (screen) ReleaseDC(nullptr, screen);
    if (!color || !colorBits) {
        if (color) DeleteObject(color);
        return DefaultAppIcon();
    }
    memcpy(colorBits, pixels.data(), pixels.size());

    BYTE maskBits[32 * 4] = {};
    HBITMAP mask = CreateBitmap(32, 32, 1, 1, maskBits);
    if (!mask) {
        DeleteObject(color);
        return DefaultAppIcon();
    }
    ICONINFO info{};
    info.fIcon = TRUE;
    info.hbmColor = color;
    info.hbmMask = mask;
    HICON icon = CreateIconIndirect(&info);
    DeleteObject(mask);
    DeleteObject(color);
    return icon ? icon : DefaultAppIcon();
}

// -----------------------------------------------------------------------------
// Native window and layout.
// -----------------------------------------------------------------------------

enum : int {
    IDC_SOW_ICON = 100,
    IDC_SOW_INSTRUCTION,
    IDC_SOW_FILE_LABEL,
    IDC_SOW_FILE_NAME,
    IDC_SOW_PROGRAMS,
    IDC_SOW_DESCRIPTION_LABEL,
    IDC_SOW_DESCRIPTION,
    IDC_SOW_ALWAYS_USE,
    IDC_SOW_BROWSE,
    IDC_SOW_WEB,
    // IDC_SOW_ALWAYS_USE_LABEL,  // DISABLED DARK THEME (checkbox label companion)
};

enum : int { GROUP_RECOMMENDED = 1, GROUP_OTHER = 2 };
static constexpr UINT WM_SOW_ACTIVATE = WM_APP + 0x217;
static constexpr UINT WM_SOW_SETTINGS_CHANGED = WM_APP + 0x218;
static const wchar_t kWindowClass[] = L"WindhawkStandaloneWin7OpenWith";
static std::atomic<HWND> g_currentWindow{nullptr};

static HINSTANCE ModInstance() {
    static HINSTANCE instance = [] {
        HMODULE module = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ModInstance), &module);
        return reinterpret_cast<HINSTANCE>(module);
    }();
    return instance;
}

static UINT WindowDpi(HWND owner) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    using GetDpiForSystem_t = UINT(WINAPI*)();
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto getWindowDpi = user32 ? reinterpret_cast<GetDpiForWindow_t>(
                                     GetProcAddress(user32, "GetDpiForWindow"))
                               : nullptr;
    auto getSystemDpi = user32 ? reinterpret_cast<GetDpiForSystem_t>(
                                     GetProcAddress(user32, "GetDpiForSystem"))
                               : nullptr;
    UINT dpi = 96;
    if (owner && getWindowDpi) dpi = getWindowDpi(owner);
    else if (getSystemDpi) dpi = getSystemDpi();
    return dpi >= 48 && dpi <= 768 ? dpi : 96;
}

static int DpiScale(int value, UINT dpi) { return MulDiv(value, dpi, 96); }

static void ApplyFont(HWND window, HFONT font) {
    if (window && font) SendMessageW(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
}

static HWND Child(HWND parent, DWORD exStyle, PCWSTR cls, PCWSTR text,
                  DWORD style, int x, int y, int width, int height, int id,
                  UINT dpi, HFONT font) {
    HWND window = CreateWindowExW(exStyle, cls, text, WS_CHILD | WS_VISIBLE | style,
        DpiScale(x, dpi), DpiScale(y, dpi), DpiScale(width, dpi),
        DpiScale(height, dpi), parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        ModInstance(), nullptr);
    ApplyFont(window, font);
    return window;
}

// DISABLED DARK THEME - the checkbox label helper is kept commented
// below, together with its forward declaration:
// static void EnsureAlwaysUseLabel(PickerState& state);

static void RefreshPickerThemeResources(PickerState& state) {
    // ResolveDarkMode() always returns false while the dark theme is
    // disabled (see its stub above); the dark brushes are kept commented
    // for future use.
    state.isDarkMode = ResolveDarkMode();
    // if (state.isDarkMode) {
    //     if (!state.darkBgBrush)
    //         state.darkBgBrush.Reset(CreateSolidBrush(RGB(32, 32, 32)));
    //     if (!state.darkCardBrush)
    //         state.darkCardBrush.Reset(CreateSolidBrush(RGB(45, 45, 45)));
    // } else {
    //     state.darkBgBrush.Reset();
    //     state.darkCardBrush.Reset();
    // }
}

// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
// static void SetImmersiveDarkTitleBar(HWND window, bool enabled) {
//     if (!window) return;
//     BOOL useDark = enabled ? TRUE : FALSE;
//     HRESULT hr = DwmSetWindowAttribute(
//         window, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
//     if (FAILED(hr) &&
//         DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 !=
//             DWMWA_USE_IMMERSIVE_DARK_MODE) {
//         DwmSetWindowAttribute(
//             window, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1,
//             &useDark, sizeof(useDark));
//     }
// }

static void ApplyPickerTheme(PickerState& state) {
    RefreshPickerThemeResources(state);

    // DISABLED DARK THEME - the dark reconfiguration below is kept
    // commented for future use (see the long note above):
    // SetImmersiveDarkTitleBar(state.window, state.isDarkMode);
    // DarkModeActivation::Apply(state.window, state.isDarkMode);
    // (the activation call used to run before the per-control theme calls
    // so comctl32 switched its group-header/selection/client-edge/scrollbar
    // palettes to the dark variants)

    if (state.list) {
        // Dark variant (DISABLED DARK THEME - kept for future use):
        // if (state.isDarkMode) {
        //     SetWindowTheme(state.list, L"DarkMode_Explorer", nullptr);
        //     ListView_SetBkColor(state.list, RGB(32, 32, 32));
        //     ListView_SetTextBkColor(state.list, RGB(32, 32, 32));
        //     ListView_SetTextColor(state.list, RGB(240, 240, 240));
        // } else {
        SetWindowTheme(state.list, L"Explorer", nullptr);
        ListView_SetBkColor(state.list, RGB(255, 255, 255));
        ListView_SetTextBkColor(state.list, RGB(255, 255, 255));
        ListView_SetTextColor(state.list, RGB(0, 0, 0));
        // }
    }

    // DISABLED DARK THEME - the dark-only parts below are kept commented
    // for future use:
    //   - the "Always use" checkbox label was emptied and moved to a
    //     companion static (IDC_SOW_ALWAYS_USE_LABEL) so the checkbox glyph
    //     could take the DarkMode_Explorer theme while the label text stayed
    //     light through WM_CTLCOLORSTATIC (network flyout approach);
    //   - the three buttons became BS_OWNERDRAW and were painted by
    //     WM_DRAWITEM with the network flyout dark palette;
    //   - the description edit dropped its WS_EX_CLIENTEDGE border.
    // Original code:
    // const bool dark = state.isDarkMode;
    // if (state.alwaysUse) {
    //     SetWindowTextW(state.alwaysUse, dark ? L"" : LOC(STR_ALWAYS_USE));
    //     SetWindowTheme(state.alwaysUse,
    //                    dark ? L"DarkMode_Explorer" : nullptr, nullptr);
    // }
    // if (dark) EnsureAlwaysUseLabel(state);
    // if (state.alwaysUseLabel) {
    //     SetWindowTextW(state.alwaysUseLabel, LOC(STR_ALWAYS_USE));
    //     ShowWindow(state.alwaysUseLabel, dark ? SW_SHOW : SW_HIDE);
    //     EnableWindow(state.alwaysUseLabel,
    //                  state.alwaysUse ? IsWindowEnabled(state.alwaysUse)
    //                                  : TRUE);
    // }
    // static const int kOwnerDrawButtons[] = {IDOK, IDCANCEL, IDC_SOW_BROWSE};
    // for (const int id : kOwnerDrawButtons) {
    //     HWND button = GetDlgItem(state.window, id);
    //     if (!button) continue;
    //     LONG_PTR style = GetWindowLongPtrW(button, GWL_STYLE);
    //     if (dark) {
    //         style |= BS_OWNERDRAW;
    //         SetWindowTheme(button, L"DarkMode_Explorer", nullptr);
    //     } else {
    //         style &= ~BS_OWNERDRAW;
    //         SetWindowTheme(button, nullptr, nullptr);
    //     }
    //     SetWindowLongPtrW(button, GWL_STYLE, style);
    //     SetWindowPos(button, nullptr, 0, 0, 0, 0,
    //                  SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
    //                      SWP_NOACTIVATE | SWP_FRAMECHANGED);
    // }
    // if (state.description) {
    //     LONG_PTR exStyle = GetWindowLongPtrW(state.description, GWL_EXSTYLE);
    //     if (dark)
    //         exStyle &= ~WS_EX_CLIENTEDGE;
    //     else
    //         exStyle |= WS_EX_CLIENTEDGE;
    //     SetWindowLongPtrW(state.description, GWL_EXSTYLE, exStyle);
    //     SetWindowPos(state.description, nullptr, 0, 0, 0, 0,
    //                  SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
    //                      SWP_NOACTIVATE | SWP_FRAMECHANGED);
    // }

    for (HWND child = state.window ? GetWindow(state.window, GW_CHILD) : nullptr;
         child; child = GetWindow(child, GW_HWNDNEXT)) {
        // Dark mode gave every non-list child the DarkMode_Explorer theme,
        // except the statics and the checkbox/label pair (DISABLED DARK
        // THEME - kept for future use). Light mode leaves them unthemed.
        if (child != state.list) {
            SetWindowTheme(child, nullptr, nullptr);
        }
    }
    if (state.window) {
        RedrawWindow(state.window, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME |
                         RDW_ALLCHILDREN);
    }
}

static int SelectedIndex(PickerState& state) {
    const int item = state.list ? ListView_GetNextItem(state.list, -1, LVNI_SELECTED) : -1;
    if (item < 0) return -1;
    LVITEMW value{};
    value.mask = LVIF_PARAM;
    value.iItem = item;
    return ListView_GetItem(state.list, &value) ? static_cast<int>(value.lParam) : -1;
}

static void UpdateSelectionUi(PickerState& state) {
    const int index = SelectedIndex(state);
    const bool valid =
        index >= 0 && static_cast<size_t>(index) < state.handlers.size();
    EnableWindow(GetDlgItem(state.window, IDOK), valid);

    // A default association only makes sense for a real extension and for an
    // IAssocHandler capable of MakeDefault. Extensionless files can be opened,
    // but there is no extension to persist as a default.
    const std::wstring extension = ExtensionOf(state.request.path);
    const bool hasAssociableExtension =
        extension.size() > 1 && extension[0] == L'.';
    const bool handlerCanBeDefault =
        valid &&
        (state.handlers[static_cast<size_t>(index)].handler ||
         !state.handlers[static_cast<size_t>(index)].progId.empty());

    if (state.request.setDefaultOnly) {
        EnableWindow(state.alwaysUse, FALSE);
        Button_SetCheck(state.alwaysUse, BST_CHECKED);
        // DISABLED DARK THEME - label companion enable sync:
        // if (state.alwaysUseLabel)
        //     EnableWindow(state.alwaysUseLabel, FALSE);
    } else {
        const bool enableAssociation =
            handlerCanBeDefault && hasAssociableExtension;
        EnableWindow(state.alwaysUse, enableAssociation);
        // if (state.alwaysUseLabel)
        //     EnableWindow(state.alwaysUseLabel, enableAssociation);
        if (!enableAssociation)
            Button_SetCheck(state.alwaysUse, BST_UNCHECKED);
    }
}

static void AddGroup(HWND list, int id, PCWSTR title, bool collapsed) {
    LVGROUP group{};
    group.cbSize = sizeof(group);
    group.mask = LVGF_HEADER | LVGF_GROUPID | LVGF_STATE;
    group.pszHeader = const_cast<PWSTR>(title);
    group.iGroupId = id;
    group.stateMask = LVGS_COLLAPSIBLE | LVGS_COLLAPSED;
    group.state = LVGS_COLLAPSIBLE | (collapsed ? LVGS_COLLAPSED : 0);
    SendMessageW(list, LVM_INSERTGROUP, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(&group));
}

static void SetListGroupCollapsed(HWND list, int groupId, bool collapsed) {
    if (!list) return;
    LVGROUP group{};
    group.cbSize = sizeof(group);
    group.mask = LVGF_STATE;
    group.stateMask = LVGS_COLLAPSED;
    group.state = collapsed ? LVGS_COLLAPSED : 0;
    SendMessageW(list, LVM_SETGROUPINFO, groupId,
                 reinterpret_cast<LPARAM>(&group));
}

static int AddListItem(PickerState& state, size_t index) {
    HandlerEntry& entry = state.handlers[index];
    if (state.images && entry.imageIndex < 0) {
        IconOwner icon(EntryIcon(entry));
        if (icon) entry.imageIndex = ImageList_AddIcon(state.images.Get(), icon.Get());
    }
    LVITEMW item{};
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    if (state.listUsesGroups) item.mask |= LVIF_GROUPID;
    item.iItem = static_cast<int>(index);
    item.iImage = entry.imageIndex;
    item.iGroupId = entry.recommended ? GROUP_RECOMMENDED : GROUP_OTHER;
    item.lParam = static_cast<LPARAM>(index);
    item.pszText = entry.displayName.data();
    return ListView_InsertItem(state.list, &item);
}

static int FindListItemForHandler(PickerState& state, size_t handlerIndex) {
    if (!state.list) return -1;
    const int count = ListView_GetItemCount(state.list);
    for (int itemIndex = 0; itemIndex < count; ++itemIndex) {
        LVITEMW item{};
        item.mask = LVIF_PARAM;
        item.iItem = itemIndex;
        if (ListView_GetItem(state.list, &item) &&
            static_cast<size_t>(item.lParam) == handlerIndex) {
            return itemIndex;
        }
    }
    return -1;
}

static void InitializeList(PickerState& state) {
    ApplyPickerTheme(state);
    ListView_SetExtendedListViewStyle(state.list,
        LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    ListView_SetView(state.list, LV_VIEW_TILE);
    state.images.Reset(ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 8, 8));
    if (state.images) {
        ListView_SetImageList(state.list, state.images.Get(), LVSIL_NORMAL);
        ListView_SetImageList(state.list, state.images.Get(), LVSIL_SMALL);
    }
    bool recommended = false, other = false;
    for (const auto& entry : state.handlers) {
        recommended |= entry.recommended;
        other |= !entry.recommended;
    }
    if (!state.handlers.empty()) {
        state.listUsesGroups = true;
        ListView_EnableGroupView(state.list, TRUE);
        if (recommended) AddGroup(state.list, GROUP_RECOMMENDED, LOC(STR_RECOMMENDED), false);
        if (other) {
            AddGroup(state.list, GROUP_OTHER, LOC(STR_OTHER), recommended);
            state.hasOtherGroup = true;
        }
    }
    LVCOLUMNW column{};
    column.mask = LVCF_WIDTH;
    column.cx = 420;
    ListView_InsertColumn(state.list, 0, &column);
    for (size_t i = 0; i < state.handlers.size(); ++i) AddListItem(state, i);
    if (!state.handlers.empty()) {
        ListView_SetItemState(state.list, 0, LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
    }
    UpdateSelectionUi(state);
}

class BrowseDialogEvents final : public IFileDialogEvents {
   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (!IsEqualIID(iid, IID_IUnknown) &&
            !IsEqualIID(iid, IID_IFileDialogEvents)) {
            return E_NOINTERFACE;
        }
        *object = static_cast<IFileDialogEvents*>(this);
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return references_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG value =
            references_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (!value) delete this;
        return value;
    }

    HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog* dialog,
                                                IShellItem*) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnShareViolation(
        IFileDialog* dialog, IShellItem*,
        FDE_SHAREVIOLATION_RESPONSE* response) override {
        CaptureWindow(dialog);
        if (response) *response = FDESVR_DEFAULT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog* dialog) override {
        CaptureWindow(dialog);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnOverwrite(
        IFileDialog* dialog, IShellItem*,
        FDE_OVERWRITE_RESPONSE* response) override {
        CaptureWindow(dialog);
        if (response) *response = FDEOR_DEFAULT;
        return S_OK;
    }

   private:
    void CaptureWindow(IFileDialog* dialog) {
        if (!dialog) return;
        ComPtr<IOleWindow> oleWindow;
        if (FAILED(dialog->QueryInterface(
                IID_IOleWindow,
                reinterpret_cast<void**>(oleWindow.Put()))) ||
            !oleWindow) {
            return;
        }
        HWND window = nullptr;
        if (SUCCEEDED(oleWindow->GetWindow(&window)) && window) {
            g_activeBrowseHwnd.store(window, std::memory_order_release);
            if (g_shuttingDown.load(std::memory_order_acquire))
                PostMessageW(window, WM_CLOSE, 0, 0);
        }
    }

    std::atomic<ULONG> references_{1};
};

static void Browse(PickerState& state) {
    ComPtr<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dialog.Put()))) || !dialog)
        return;
    DWORD options = 0;
    if (SUCCEEDED(dialog->GetOptions(&options)))
        dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST);
    const COMDLG_FILTERSPEC filters[] = {
        {LOC(STR_PROGRAMS), L"*.exe;*.com;*.bat;*.cmd"},
        {LOC(STR_ALL_FILES), L"*.*"},
    };
    dialog->SetFileTypes(ARRAYSIZE(filters), filters);
    dialog->SetTitle(LOC(STR_BROWSE_TITLE));

    BrowseDialogEvents* rawEvents = nullptr;
    try {
        rawEvents = new BrowseDialogEvents();
    } catch (...) {
        return;
    }
    ComPtr<IFileDialogEvents> events(rawEvents);
    DWORD eventsCookie = 0;
    const bool advised = SUCCEEDED(dialog->Advise(events.Get(), &eventsCookie));
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        if (advised) dialog->Unadvise(eventsCookie);
        return;
    }
    const HRESULT showHr = dialog->Show(state.window);
    if (advised) dialog->Unadvise(eventsCookie);
    g_activeBrowseHwnd.store(nullptr, std::memory_order_release);
    if (FAILED(showHr) ||
        g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    ComPtr<IShellItem> item;
    if (FAILED(dialog->GetResult(item.Put())) || !item) return;
    PWSTR raw = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) || !raw) return;
    const std::wstring executable = TakeTaskString(raw);
    if (executable.empty()) return;
    if (IsOpenWithExecutable(executable)) {
        SetWindowTextW(GetDlgItem(state.window, IDC_SOW_INSTRUCTION),
                       LOC(STR_OPEN_FAILED));
        MessageBeep(MB_ICONWARNING);
        return;
    }
    for (size_t i = 0; i < state.handlers.size(); ++i) {
        if (!_wcsicmp(state.handlers[i].internalName.c_str(), executable.c_str())) {
            if (!state.handlers[i].recommended)
                SetListGroupCollapsed(state.list, GROUP_OTHER, false);
            const int itemIndex = FindListItemForHandler(state, i);
            if (itemIndex >= 0) {
                ListView_SetItemState(state.list, itemIndex,
                                      LVIS_SELECTED | LVIS_FOCUSED,
                                      LVIS_SELECTED | LVIS_FOCUSED);
                ListView_EnsureVisible(state.list, itemIndex, FALSE);
            }
            UpdateSelectionUi(state);
            return;
        }
    }
    HandlerEntry entry;
    entry.browsed = true;
    entry.internalName = executable;
    entry.progId = ApplicationProgIdForExecutable(executable);
    PCWSTR name = PathFindFileNameW(executable.c_str());
    entry.displayName = name && *name ? name : executable;
    if (entry.displayName.size() > 4 &&
        !_wcsicmp(entry.displayName.c_str() + entry.displayName.size() - 4, L".exe"))
        entry.displayName.resize(entry.displayName.size() - 4);
    state.handlers.push_back(std::move(entry));
    if (!state.listUsesGroups) {
        state.listUsesGroups = true;
        ListView_EnableGroupView(state.list, TRUE);
    }
    if (!state.hasOtherGroup) {
        AddGroup(state.list, GROUP_OTHER, LOC(STR_OTHER), false);
        state.hasOtherGroup = true;
    }
    // A pre-existing Other Programs group is initially collapsed when there
    // are recommended handlers. Expand it before selecting the browsed app so
    // the newly added item is immediately visible.
    if (state.listUsesGroups)
        SetListGroupCollapsed(state.list, GROUP_OTHER, false);

    const size_t index = state.handlers.size() - 1;
    const int insertedItem = AddListItem(state, index);
    if (insertedItem >= 0) {
        ListView_SetItemState(state.list, insertedItem,
                              LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(state.list, insertedItem, FALSE);
        SetFocus(state.list);
        RedrawWindow(state.list, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
    UpdateSelectionUi(state);
}

static void WebSearch(const std::wstring& path) {
    std::wstring query;
    for (wchar_t c : ExtensionOf(path)) {
        if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') ||
            (c >= L'0' && c <= L'9')) query.push_back(c);
    }
    if (query.empty()) query = L"unknown";
    const std::wstring url = L"https://www.bing.com/search?q=program+to+open+" + query + L"+file";
    ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void SetGroupTitle(HWND list, int groupId, PCWSTR title) {
    if (!list) return;
    LVGROUP group{};
    group.cbSize = sizeof(group);
    group.mask = LVGF_HEADER;
    group.pszHeader = const_cast<PWSTR>(title);
    SendMessageW(list, LVM_SETGROUPINFO, groupId,
                 reinterpret_cast<LPARAM>(&group));
}

static void ApplyLocalizedText(PickerState& state) {
    if (!state.window || !IsWindow(state.window)) return;
    SetWindowTextW(state.window, LOC(STR_TITLE));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_INSTRUCTION),
                   state.handlers.empty() ? LOC(STR_NO_HANDLERS)
                                          : LOC(STR_INSTRUCTION));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_FILE_LABEL),
                   LOC(STR_FILE_LABEL));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_DESCRIPTION_LABEL),
                   LOC(STR_DESCRIPTION));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_ALWAYS_USE),
                   LOC(STR_ALWAYS_USE));
    // DISABLED DARK THEME - dark mode emptied the checkbox text and
    // moved it to the companion static (kept for future use):
    // SetWindowTextW(GetDlgItem(state.window, IDC_SOW_ALWAYS_USE),
    //                state.isDarkMode ? L"" : LOC(STR_ALWAYS_USE));
    // if (state.alwaysUseLabel && IsWindow(state.alwaysUseLabel))
    //     SetWindowTextW(state.alwaysUseLabel, LOC(STR_ALWAYS_USE));
    SetWindowTextW(GetDlgItem(state.window, IDC_SOW_BROWSE), LOC(STR_BROWSE));
    HWND web = GetDlgItem(state.window, IDC_SOW_WEB);
    SetWindowTextW(web, LOC(STR_WEB_LINK));
    ShowWindow(web, g_showWebLink.load(std::memory_order_acquire)
                        ? SW_SHOW : SW_HIDE);
    SetWindowTextW(GetDlgItem(state.window, IDOK), LOC(STR_OK));
    SetWindowTextW(GetDlgItem(state.window, IDCANCEL), LOC(STR_CANCEL));
    SetGroupTitle(state.list, GROUP_RECOMMENDED, LOC(STR_RECOMMENDED));
    SetGroupTitle(state.list, GROUP_OTHER, LOC(STR_OTHER));
}

// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
// static void EnsureAlwaysUseLabel(PickerState& state) {
//     if (state.alwaysUseLabel && IsWindow(state.alwaysUseLabel)) return;
//     const UINT dpi = WindowDpi(state.window);
//     HFONT font = state.font ? state.font.Get()
//                             : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    // Dark-mode-only companion of the "Always use" checkbox: the
    // checkbox itself keeps an empty label (network flyout trick) so its
    // glyph can use the DarkMode_Explorer theme, while this static draws
    // the label text through WM_CTLCOLORSTATIC, which does honor our
    // light text color.
//     state.alwaysUseLabel =
//         Child(state.window, 0, WC_STATICW, LOC(STR_ALWAYS_USE),
//               SS_LEFT | SS_NOTIFY | SS_CENTERIMAGE, 35, 354, 380, 22,
//               IDC_SOW_ALWAYS_USE_LABEL, dpi, font);
// }

// static void UpdateButtonHover(PickerState& state, POINT screenPoint) {
//     int target = 0;
//     static const int kOwnerDrawButtons[] = {IDOK, IDCANCEL, IDC_SOW_BROWSE};
//     for (const int id : kOwnerDrawButtons) {
//         HWND button = GetDlgItem(state.window, id);
//         if (!button) continue;
//         RECT rect{};
//         if (GetWindowRect(button, &rect) && PtInRect(&rect, screenPoint)) {
//             target = id;
//             break;
//         }
//     }
//     if (target == state.hoverButton) return;
//     HWND previous =
//         state.hoverButton ? GetDlgItem(state.window, state.hoverButton) : nullptr;
//     state.hoverButton = target;
//     if (previous) InvalidateRect(previous, nullptr, FALSE);
//     if (target) InvalidateRect(GetDlgItem(state.window, target), nullptr, FALSE);
//     TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, state.window, 0};
//     TrackMouseEvent(&tme);
// }

static void BuildPickerControls(PickerState& state) {
    const UINT dpi = WindowDpi(state.window);
    NONCLIENTMETRICSW metrics{sizeof(metrics)};
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
        state.font.Reset(CreateFontIndirectW(&metrics.lfMessageFont));
    HFONT font = state.font ? state.font.Get() : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

    // DISABLED DARK THEME - dark mode resolved its theme here before
    // creating the controls so it could use owner-draw buttons and the
    // checkbox label companion (kept for future use):
    // state.isDarkMode = ResolveDarkMode();
    // const bool dark = state.isDarkMode;

    HWND icon = Child(state.window, 0, WC_STATICW, L"", SS_ICON | SS_REALSIZECONTROL,
                      14, 14, 34, 34, IDC_SOW_ICON, dpi, font);
    Child(state.window, 0, WC_STATICW, LOC(STR_INSTRUCTION), SS_LEFT,
          58, 15, 480, 20, IDC_SOW_INSTRUCTION, dpi, font);
    Child(state.window, 0, WC_STATICW, LOC(STR_FILE_LABEL), SS_LEFT,
          58, 40, 40, 18, IDC_SOW_FILE_LABEL, dpi, font);
    PCWSTR displayedFileName = PathFindFileNameW(state.request.path.c_str());
    if (!displayedFileName || !*displayedFileName)
        displayedFileName = state.request.path.c_str();
    Child(state.window, 0, WC_STATICW, displayedFileName,
          SS_LEFT | SS_PATHELLIPSIS | SS_NOPREFIX,
          98, 40, 435, 18, IDC_SOW_FILE_NAME, dpi, font);
    state.list = Child(state.window, WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
          WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS |
              LVS_SHAREIMAGELISTS | LVS_NOCOLUMNHEADER,
          14, 65, 532, 225, IDC_SOW_PROGRAMS, dpi, font);
    Child(state.window, 0, WC_STATICW, LOC(STR_DESCRIPTION), SS_LEFT,
          14, 300, 532, 18, IDC_SOW_DESCRIPTION_LABEL, dpi, font);
    // DISABLED DARK THEME - dark mode dropped WS_EX_CLIENTEDGE here
    // (kept for future use). Light mode keeps the classic client edge.
    state.description = Child(state.window, WS_EX_CLIENTEDGE, WC_EDITW, L"",
          WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
          14, 320, 520, 24, IDC_SOW_DESCRIPTION, dpi, font);
    // DISABLED DARK THEME - dark mode created the checkbox with an
    // empty label plus a companion static for the text (kept for
    // future use):
    // state.alwaysUse = Child(state.window, 0, WC_BUTTONW,
    //       dark ? L"" : LOC(STR_ALWAYS_USE),
    //       WS_TABSTOP | BS_AUTOCHECKBOX, 14, 354, 405, 22,
    //       IDC_SOW_ALWAYS_USE, dpi, font);
    // if (dark) EnsureAlwaysUseLabel(state);
    state.alwaysUse = Child(state.window, 0, WC_BUTTONW, LOC(STR_ALWAYS_USE),
          WS_TABSTOP | BS_AUTOCHECKBOX, 14, 354, 405, 22,
          IDC_SOW_ALWAYS_USE, dpi, font);
    // DISABLED DARK THEME - dark mode made the buttons below owner-draw
    // (BS_OWNERDRAW) and painted them in WM_DRAWITEM with the network
    // flyout palette (kept for future use).
    Child(state.window, 0, WC_BUTTONW, LOC(STR_BROWSE),
          WS_TABSTOP | BS_PUSHBUTTON, 452, 350, 94, 27,
          IDC_SOW_BROWSE, dpi, font);
    HWND web = Child(state.window, 0, WC_LINK, LOC(STR_WEB_LINK), WS_TABSTOP,
                     14, 386, 532, 38, IDC_SOW_WEB, dpi, font);
    ShowWindow(web, g_showWebLink.load(std::memory_order_acquire)
                        ? SW_SHOW : SW_HIDE);
    Child(state.window, 0, WC_BUTTONW, LOC(STR_OK),
          WS_TABSTOP | WS_DISABLED | BS_DEFPUSHBUTTON,
          392, 426, 74, 27, IDOK, dpi, font);
    Child(state.window, 0, WC_BUTTONW, LOC(STR_CANCEL),
          WS_TABSTOP | BS_PUSHBUTTON, 472, 426, 74, 27,
          IDCANCEL, dpi, font);

    state.headerIcon.Reset(LoadStandaloneIcon());
    if (state.headerIcon) {
        SendMessageW(icon, STM_SETICON,
                     reinterpret_cast<WPARAM>(state.headerIcon.Get()), 0);
        SendMessageW(state.window, WM_SETICON, ICON_SMALL,
                     reinterpret_cast<LPARAM>(state.headerIcon.Get()));
        SendMessageW(state.window, WM_SETICON, ICON_BIG,
                     reinterpret_cast<LPARAM>(state.headerIcon.Get()));
    }
    InitializeList(state);
    ApplyLocalizedText(state);
    if (state.request.setDefaultOnly && state.alwaysUse) {
        Button_SetCheck(state.alwaysUse, BST_CHECKED);
        EnableWindow(state.alwaysUse, FALSE);
    }
}

static void ActivatePickerWindow(HWND window) {
    if (!window || !IsWindow(window)) return;
    ShowWindow(window, SW_RESTORE);
    // A brief topmost pulse reliably raises an owned window without leaving it
    // permanently above unrelated applications.
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(window);
}

static LRESULT PickerWndProcBody(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    PickerState* state = reinterpret_cast<PickerState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    switch (message) {
        case WM_NCCREATE: {
            auto create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            state = static_cast<PickerState*>(create->lpCreateParams);
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
            if (state) { state->window = window; g_currentWindow.store(window, std::memory_order_release); }
            return TRUE;
        }
        case WM_CREATE:
            if (!state) return -1;
            BuildPickerControls(*state);
            return 0;
        case WM_ERASEBKGND: {
            RECT client{};
            GetClientRect(window, &client);
            // DISABLED DARK THEME - dark mode filled the background with
            // a RGB(32, 32, 32) brush (kept for future use):
            // HBRUSH brush = state && state->isDarkMode && state->darkBgBrush
            //                    ? state->darkBgBrush.Get()
            //                    : GetSysColorBrush(COLOR_3DFACE);
            HBRUSH brush = GetSysColorBrush(COLOR_3DFACE);
            FillRect(reinterpret_cast<HDC>(wParam), &client, brush);
            return 1;
        }
// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
//         case WM_DRAWITEM: {
//             if (!state || !state->isDarkMode) break;
//             auto item = reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);
//             if (!item) break;
//             if (item->CtlID != IDOK && item->CtlID != IDCANCEL &&
//                 item->CtlID != IDC_SOW_BROWSE) {
//                 break;
//             }
            // Same dark button painting as the network flyout mod.
//             const bool pressed = (item->itemState & ODS_SELECTED) != 0;
//             const bool disabled = (item->itemState & ODS_DISABLED) != 0;
//             const bool focused = (item->itemState & ODS_FOCUS) != 0;
//             const bool hovering =
//                 state->hoverButton == static_cast<int>(item->CtlID) &&
//                 !pressed && !disabled;
//             HDC hdcReal = item->hDC;
//             RECT rc = item->rcItem;
//             const int w = rc.right - rc.left;
//             const int h = rc.bottom - rc.top;
//             if (w <= 0 || h <= 0) break;
//             WCHAR text[128];
//             const int textLen =
//                 GetWindowTextW(item->hwndItem, text, ARRAYSIZE(text));
//             COLORREF bgColor = disabled   ? RGB(50, 50, 58)
//                                : pressed  ? RGB(35, 35, 45)
//                                : hovering ? RGB(70, 70, 85)
//                                           : RGB(60, 60, 72);
//             COLORREF lightColor =
//                 pressed ? RGB(25, 25, 32)
//                         : (hovering ? RGB(95, 95, 115) : RGB(85, 85, 100));
//             COLORREF darkColor =
//                 pressed ? RGB(60, 60, 72)
//                         : (hovering ? RGB(35, 35, 45) : RGB(25, 25, 32));
//             COLORREF textColor =
//                 disabled ? RGB(130, 130, 140) : RGB(255, 255, 255);
//             COLORREF hoverBorder = hovering ? RGB(90, 90, 120) : RGB(0, 0, 0);
//             HDC hdcMem = CreateCompatibleDC(hdcReal);
//             HBITMAP bitmapMem = CreateCompatibleBitmap(hdcReal, w, h);
//             HBITMAP oldBitmapMem =
//                 static_cast<HBITMAP>(SelectObject(hdcMem, bitmapMem));
//             RECT rcLocal{0, 0, w, h};
//             HBRUSH bgBrush = CreateSolidBrush(bgColor);
//             FillRect(hdcMem, &rcLocal, bgBrush);
//             DeleteObject(bgBrush);
//             HPEN penLight = CreatePen(PS_SOLID, 1, lightColor);
//             HPEN penDark = CreatePen(PS_SOLID, 1, darkColor);
//             HPEN penHover =
//                 hovering ? CreatePen(PS_SOLID, 1, hoverBorder) : nullptr;
//             HPEN oldPen = static_cast<HPEN>(SelectObject(hdcMem, penLight));
//             MoveToEx(hdcMem, 0, h - 1, nullptr);
//             LineTo(hdcMem, 0, 0);
//             LineTo(hdcMem, w - 1, 0);
//             SelectObject(hdcMem, penDark);
//             MoveToEx(hdcMem, w - 1, 0, nullptr);
//             LineTo(hdcMem, w - 1, h - 1);
//             LineTo(hdcMem, 0, h - 1);
//             if (hovering && penHover) {
//                 SelectObject(hdcMem, penHover);
//                 MoveToEx(hdcMem, 1, 1, nullptr);
//                 LineTo(hdcMem, w - 2, 1);
//                 LineTo(hdcMem, w - 2, h - 2);
//                 LineTo(hdcMem, 1, h - 2);
//                 LineTo(hdcMem, 1, 1);
//                 DeleteObject(penHover);
//             }
//             SelectObject(hdcMem, oldPen);
//             DeleteObject(penLight);
//             DeleteObject(penDark);
//             if (focused) {
//                 RECT rcFocus = rcLocal;
//                 InflateRect(&rcFocus, -3, -3);
//                 HGDIOBJ oldBrush =
//                     SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
//                 SetTextColor(hdcMem, RGB(150, 150, 165));
//                 DrawFocusRect(hdcMem, &rcFocus);
//                 SelectObject(hdcMem, oldBrush);
//             }
//             SetBkMode(hdcMem, TRANSPARENT);
//             SetTextColor(hdcMem, textColor);
//             HFONT oldFont = static_cast<HFONT>(SelectObject(
//                 hdcMem,
//                 reinterpret_cast<HFONT>(SendMessageW(item->hwndItem, WM_GETFONT, 0, 0))));
//             RECT rcText = rcLocal;
//             if (pressed) {
//                 rcText.left += 1;
//                 rcText.top += 1;
//             }
//             DrawTextW(hdcMem, text, textLen, &rcText,
//                       DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//             SelectObject(hdcMem, oldFont);
//             BitBlt(hdcReal, rc.left, rc.top, w, h, hdcMem, 0, 0, SRCCOPY);
//             SelectObject(hdcMem, oldBitmapMem);
//             DeleteObject(bitmapMem);
//             DeleteDC(hdcMem);
//             return TRUE;
//         }
// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
//         case WM_MOUSEMOVE: {
//             if (!state || !state->isDarkMode) break;
//             POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
//             ClientToScreen(window, &pt);
//             UpdateButtonHover(*state, pt);
//             break;
//         }
// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
//         case WM_SETCURSOR: {
            // Children that don't set their own cursor (the owner-draw
            // buttons) bubble WM_SETCURSOR up to the dialog, so hover
            // tracking keeps working while the pointer is over them.
//             if (!state || !state->isDarkMode) break;
//             POINT pt{};
//             GetCursorPos(&pt);
//             UpdateButtonHover(*state, pt);
//             SetCursor(LoadCursorW(nullptr, IDC_ARROW));
//             return TRUE;
//         }
// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
//         case WM_MOUSELEAVE: {
//             if (!state || !state->isDarkMode) break;
//             if (state->hoverButton != 0) {
//                 HWND button = GetDlgItem(window, state->hoverButton);
//                 state->hoverButton = 0;
//                 if (button) InvalidateRect(button, nullptr, FALSE);
//             }
//             break;
//         }
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX: {
            // DISABLED DARK THEME - dark mode recolored the edit/listbox
            // background to RGB(45, 45, 45) with light text (kept for
            // future use):
            // if (state && state->isDarkMode && state->darkCardBrush) {
            //     HDC hdc = reinterpret_cast<HDC>(wParam);
            //     SetBkMode(hdc, OPAQUE);
            //     SetBkColor(hdc, RGB(45, 45, 45));
            //     SetTextColor(hdc, RGB(240, 240, 240));
            //     return reinterpret_cast<LRESULT>(state->darkCardBrush.Get());
            // }
            break;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORDLG: {
            // DISABLED DARK THEME - dark mode drew statics with light
            // text RGB(240, 240, 240) (kept for future use):
            // if (state && state->isDarkMode && state->darkBgBrush) {
            //     HDC hdc = reinterpret_cast<HDC>(wParam);
            //     HWND control = reinterpret_cast<HWND>(lParam);
            //     SetBkMode(hdc, TRANSPARENT);
            //     const bool disabled =
            //         control && !IsWindowEnabled(control);
            //     SetTextColor(hdc, disabled ? RGB(140, 140, 140)
            //                                 : RGB(240, 240, 240));
            //     return reinterpret_cast<LRESULT>(state->darkBgBrush.Get());
            // }
            break;
        }
// DISABLED DARK THEME - kept commented for future use (see the long
// note above the DarkModeActivation block).
//         case WM_CTLCOLORBTN: {
//             if (state && state->isDarkMode && state->darkBgBrush) {
//                 HDC hdc = reinterpret_cast<HDC>(wParam);
//                 HWND control = reinterpret_cast<HWND>(lParam);
                // The themed DarkMode_Explorer checkbox glyph needs an
                // opaque background matching the dialog (network flyout
                // approach), otherwise it sits on a light patch.
//                 if (control == state->alwaysUse) {
//                     SetBkMode(hdc, OPAQUE);
//                     SetBkColor(hdc, RGB(32, 32, 32));
//                     return reinterpret_cast<LRESULT>(state->darkBgBrush.Get());
//                 }
//                 SetBkMode(hdc, TRANSPARENT);
//                 const bool disabled =
//                     control && !IsWindowEnabled(control);
//                 SetTextColor(hdc, disabled ? RGB(140, 140, 140)
//                                             : RGB(240, 240, 240));
//                 return reinterpret_cast<LRESULT>(state->darkBgBrush.Get());
//             }
//             break;
//         }
        case WM_COMMAND:
            if (!state) break;
            if (LOWORD(wParam) == IDOK) {
                const int index = SelectedIndex(*state);
                if (index >= 0 && static_cast<size_t>(index) < state->handlers.size()) {
                    state->chosenIndex = index;
                    state->accepted = true;
                    const std::wstring extension =
                        ExtensionOf(state->request.path);
                    state->makeDefaultRequested =
                        extension.size() > 1 && extension[0] == L'.' &&
                        (state->request.setDefaultOnly ||
                         Button_GetCheck(state->alwaysUse) == BST_CHECKED);
                    if (state->description) {
                        const int chars =
                            GetWindowTextLengthW(state->description);
                        if (chars > 0 && chars < 32767) {
                            try {
                                std::vector<wchar_t> text(chars + 1);
                                if (GetWindowTextW(state->description,
                                                   text.data(),
                                                   static_cast<int>(
                                                       text.size()))) {
                                    state->associationDescription.assign(
                                        text.data());
                                }
                            } catch (...) {
                            }
                        }
                    }
                    DestroyWindow(window);
                }
                return 0;
            }
            if (LOWORD(wParam) == IDCANCEL) { DestroyWindow(window); return 0; }
            if (LOWORD(wParam) == IDC_SOW_BROWSE) { Browse(*state); return 0; }
            // DISABLED DARK THEME - clicking the checkbox label static
            // used to toggle the companion checkbox (kept for future use):
            // if (LOWORD(wParam) == IDC_SOW_ALWAYS_USE_LABEL) {
            //     const bool checked =
            //         Button_GetCheck(state->alwaysUse) == BST_CHECKED;
            //     Button_SetCheck(state->alwaysUse,
            //                     checked ? BST_UNCHECKED : BST_CHECKED);
            //     return 0;
            // }
            break;
        case WM_NOTIFY: {
            if (!state) break;
            auto header = reinterpret_cast<NMHDR*>(lParam);
            if (header && header->idFrom == IDC_SOW_PROGRAMS) {
                if (header->code == LVN_ITEMCHANGED) UpdateSelectionUi(*state);
                if (header->code == NM_DBLCLK && SelectedIndex(*state) >= 0)
                    SendMessageW(window, WM_COMMAND, IDOK, 0);
                return 0;
            }
            if (header && header->idFrom == IDC_SOW_WEB &&
                (header->code == NM_CLICK || header->code == NM_RETURN)) {
                WebSearch(state->request.path);
                return 0;
            }
            if (header && header->idFrom == IDC_SOW_WEB &&
                header->code == NM_CUSTOMDRAW) {
                auto customDraw = reinterpret_cast<NMCUSTOMDRAW*>(lParam);
                if (customDraw->dwDrawStage == CDDS_PREPAINT) {
                    // SysLink doesn't route through WM_CTLCOLORSTATIC, so its
                    // plain (non-hyperlink) text keeps the default color and
                    // must be set explicitly. DISABLED DARK THEME - dark mode
                    // used RGB(240, 240, 240) here (kept for future use).
                    SetTextColor(customDraw->hdc, RGB(0, 0, 0));
                    return CDRF_DODEFAULT;
                }
                break;
            }
            break;
        }
        case WM_SOW_ACTIVATE:
            ActivatePickerWindow(window);
            return 0;
        case WM_SOW_SETTINGS_CHANGED:
            if (state) {
                ApplyPickerTheme(*state);
                ApplyLocalizedText(*state);
            }
            return 0;
        case WM_KEYDOWN: {
            if (wParam == VK_RETURN) {
                SendMessageW(window, WM_COMMAND, IDOK, 0);
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                SendMessageW(window, WM_COMMAND, IDCANCEL, 0);
                return 0;
            }
            break;
        }
        case WM_CLOSE: DestroyWindow(window); return 0;
        case WM_NCDESTROY:
            if (state) { state->finished = true; state->window = nullptr; }
            g_currentWindow.store(nullptr, std::memory_order_release);
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static LRESULT CALLBACK PickerWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    try { return PickerWndProcBody(window, message, wParam, lParam); }
    catch (...) {
        Wh_Log(L"Standalone Open With: exception contained in window proc");
        if (message != WM_NCDESTROY && IsWindow(window)) DestroyWindow(window);
        return 0;
    }
}

// -----------------------------------------------------------------------------
// Invocation and modeless worker-owned picker loop.
// -----------------------------------------------------------------------------

static std::wstring ExecutableForProgId(const std::wstring& progId) {
    if (progId.empty()) return {};
    wchar_t subKey[2048] = {};
    if (swprintf_s(subKey, L"%s\\shell\\open\\command",
                   progId.c_str()) < 0) {
        return {};
    }
    wchar_t command[32768] = {};
    DWORD bytes = sizeof(command);
    if (RegGetValueW(HKEY_CLASSES_ROOT, subKey, nullptr,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                     command, &bytes) != ERROR_SUCCESS) {
        return {};
    }
    return ExecutableFromCommand(command);
}

static std::wstring CommandTemplateForProgId(
    const std::wstring& progId) {
    if (progId.empty()) return {};
    wchar_t subKey[2048] = {};
    if (swprintf_s(subKey, L"%s\\shell\\open\\command",
                   progId.c_str()) < 0) {
        return {};
    }
    DWORD bytes = 0;
    if (RegGetValueW(HKEY_CLASSES_ROOT, subKey, nullptr,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                     nullptr, &bytes) != ERROR_SUCCESS || !bytes ||
        bytes > 65534 * sizeof(wchar_t)) {
        return {};
    }
    try {
        std::vector<wchar_t> command(bytes / sizeof(wchar_t) + 2);
        if (RegGetValueW(HKEY_CLASSES_ROOT, subKey, nullptr,
                         RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr,
                         command.data(), &bytes) != ERROR_SUCCESS) {
            return {};
        }
        return command.data();
    } catch (...) {
        return {};
    }
}

static std::wstring ExecutableForHandler(const HandlerEntry& handler) {
    if (IsSupportedFile(handler.internalName)) return handler.internalName;
    std::wstring executable = ExecutableForProgId(handler.progId);
    if (IsSupportedFile(executable)) return executable;
    if (!_wcsnicmp(handler.progId.c_str(), L"Applications\\", 13)) {
        PCWSTR executableName = handler.progId.c_str() + 13;
        wchar_t resolved[MAX_PATH] = {};
        if (SearchPathW(nullptr, executableName, nullptr, ARRAYSIZE(resolved),
                        resolved, nullptr)) {
            return resolved;
        }
    }
    return {};
}

static std::wstring QuoteCommandLineArgument(const std::wstring& value) {
    std::wstring result;
    try {
        result.push_back(L'"');
        size_t backslashes = 0;
        for (wchar_t c : value) {
            if (c == L'\\') {
                ++backslashes;
                continue;
            }
            if (c == L'"') {
                result.append(backslashes * 2 + 1, L'\\');
                result.push_back(L'"');
                backslashes = 0;
                continue;
            }
            result.append(backslashes, L'\\');
            backslashes = 0;
            result.push_back(c);
        }
        // Backslashes before the closing quote must be doubled.
        result.append(backslashes * 2, L'\\');
        result.push_back(L'"');
    } catch (...) {
        return {};
    }
    return result;
}

static bool ReplaceCommandPlaceholder(std::wstring& command,
                                      const std::wstring& placeholder,
                                      const std::wstring& replacement) {
    bool replaced = false;
    size_t position = 0;
    while ((position = command.find(placeholder, position)) !=
           std::wstring::npos) {
        command.replace(position, placeholder.size(), replacement);
        position += replacement.size();
        replaced = true;
    }
    return replaced;
}

static std::wstring BuildCommandLineFromTemplate(
    const std::wstring& commandTemplate, const std::wstring& path) {
    if (commandTemplate.empty() || path.empty()) return {};
    std::wstring command = commandTemplate;
    const std::wstring quotedPath = QuoteCommandLineArgument(path);
    if (quotedPath.empty()) return {};
    try {
        bool replaced = false;
        // Replace already-quoted placeholders first to avoid producing
        // doubled quotes such as ""C:\\file"".
        replaced |= ReplaceCommandPlaceholder(command, L"\"%1\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"\"%L\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"\"%l\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"\"%*\"", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%1", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%L", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%l", quotedPath);
        replaced |= ReplaceCommandPlaceholder(command, L"%*", quotedPath);
        if (!replaced) {
            command.push_back(L' ');
            command.append(quotedPath);
        }
        return command;
    } catch (...) {
        return {};
    }
}

static HRESULT CreateProcessFromCommandLine(const std::wstring& commandLine,
                                            PCWSTR currentDirectory = nullptr) {
    if (commandLine.empty()) return E_INVALIDARG;
    try {
        std::vector<wchar_t> mutableCommand(commandLine.begin(),
                                            commandLine.end());
        mutableCommand.push_back(L'\0');
        STARTUPINFOW startup{sizeof(startup)};
        PROCESS_INFORMATION process{};
        if (!CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr,
                            FALSE, 0, nullptr, currentDirectory, &startup,
                            &process)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        WinHandle processHandle(process.hProcess);
        WinHandle threadHandle(process.hThread);
        return S_OK;
    } catch (...) {
        return E_OUTOFMEMORY;
    }
}

static HRESULT InvokeCommandTemplate(const std::wstring& commandTemplate,
                                     const std::wstring& path) {
    const std::wstring commandLine =
        BuildCommandLineFromTemplate(commandTemplate, path);
    if (commandLine.empty()) return E_FAIL;
    const std::wstring executable = ExecutableFromCommand(commandLine.c_str());
    if (IsOpenWithExecutable(executable))
        return HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION);
    const HRESULT hr = CreateProcessFromCommandLine(commandLine);
    Wh_Log(L"Standalone Open With: direct command launch command=%s "
           L"hr=0x%08X", commandLine.c_str(),
           static_cast<unsigned int>(hr));
    return hr;
}

static HRESULT InvokeExecutableWithFile(const std::wstring& executable,
                                        const std::wstring& path) {
    if (!IsSupportedFile(executable) || !IsSupportedFile(path) ||
        IsOpenWithExecutable(executable)) {
        return E_INVALIDARG;
    }
    const std::wstring commandLine =
        QuoteCommandLineArgument(executable) + L" " +
        QuoteCommandLineArgument(path);
    const HRESULT hr = CreateProcessFromCommandLine(commandLine);
    Wh_Log(L"Standalone Open With: direct executable launch exe=%s file=%s "
           L"hr=0x%08X", executable.c_str(), path.c_str(),
           static_cast<unsigned int>(hr));
    return hr;
}

static HRESULT InvokeSelectedHandler(const PickerState& state,
                                     HandlerEntry& selected) {
    try {
        const std::wstring executable = ExecutableForHandler(selected);
        if (IsOpenWithExecutable(executable) ||
            IsOpenWithHandlerName(selected.internalName, selected.progId)) {
            return HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION);
        }

        // Never pass a user-selected Win32 program back through generic Shell
        // association resolution: on an unknown type that can re-enter
        // OpenWith. Resolve and launch the command template directly instead.
        HRESULT directHr = E_FAIL;
        if (!selected.progId.empty()) {
            const std::wstring commandTemplate =
                CommandTemplateForProgId(selected.progId);
            if (!commandTemplate.empty()) {
                directHr = InvokeCommandTemplate(commandTemplate,
                                                 state.request.path);
            }
        }
        if (FAILED(directHr)) {
            directHr = InvokeExecutableWithFile(executable,
                                                state.request.path);
        }
        Wh_Log(L"Standalone Open With: selected handler direct result "
               L"progId=%s exe=%s hr=0x%08X",
               selected.progId.c_str(), executable.c_str(),
               static_cast<unsigned int>(directHr));
        return directHr;
    } catch (...) {
        return E_FAIL;
    }
}

static HRESULT InvokeBrowsed(const PickerState& state,
                             const std::wstring& executable) {
    return InvokeExecutableWithFile(executable, state.request.path);
}

static HRESULT MakeSelectedDefault(PickerState& state,
                                   HandlerEntry& selected) {
    const std::wstring extension = ExtensionOf(state.request.path);
    if (extension.size() <= 1) return E_INVALIDARG;

    if (selected.progId.empty())
        selected.progId = ResolveHandlerProgId(selected.internalName);
    if (selected.progId.empty() && IsSupportedFile(selected.internalName)) {
        EnsureUserApplicationRegistration(selected.internalName, extension,
                                          &selected.progId);
    }
    if (selected.progId.empty()) return E_FAIL;

    HRESULT shellHr = E_NOTIMPL;
    if (selected.handler) {
        PCWSTR description = !state.associationDescription.empty()
            ? state.associationDescription.c_str()
            : extension.c_str();
        shellHr = selected.handler->MakeDefault(description);
        Wh_Log(L"Standalone Open With: IAssocHandler::MakeDefault handler=%s "
               L"progId=%s hr=0x%08X", selected.displayName.c_str(),
               selected.progId.c_str(),
               static_cast<unsigned int>(shellHr));
    }

    Wh_Log(L"Standalone Open With: association result extension=%s "
           L"progId=%s shellHr=0x%08X",
           extension.c_str(), selected.progId.c_str(),
           static_cast<unsigned int>(shellHr));

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSH,
                   nullptr, nullptr);
    return shellHr;
}

static WinHandle g_stopEvent;
static WinHandle g_requestEvent;
static WinHandle g_workerReadyEvent;
static std::atomic<bool> g_workerReady{false};
static std::mutex g_requestMutex;
static std::optional<PickerRequest> g_pendingRequest;
#if defined(__clang__)
[[clang::no_destroy]]
#endif
static std::optional<std::thread> g_worker;

static bool RegisterPickerClass() {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = PickerWndProc;
    wc.hInstance = ModInstance();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    // The per-window Base64 icon is assigned after creation and owned by the
    // picker state; the class itself owns no icon handle.
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszClassName = kWindowClass;
    return RegisterClassExW(&wc) != 0;
}

class PickerClassRegistration {
   public:
    bool Register() {
        if (registered_) return true;
        registered_ = RegisterPickerClass();
        return registered_;
    }
    ~PickerClassRegistration() {
        if (registered_) UnregisterClassW(kWindowClass, ModInstance());
    }
    PickerClassRegistration(const PickerClassRegistration&) = delete;
    PickerClassRegistration& operator=(const PickerClassRegistration&) = delete;
    PickerClassRegistration() = default;

   private:
    bool registered_ = false;
};

static HWND CreatePickerWindow(PickerState& state) {
    const UINT dpi = WindowDpi(state.request.owner);
    int width = DpiScale(560, dpi), height = DpiScale(465, dpi);
    RECT rect{0, 0, width, height};
    AdjustWindowRectEx(&rect, WS_POPUP | WS_CAPTION | WS_SYSMENU, FALSE,
                       WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    HMONITOR monitor = MonitorFromWindow(state.request.owner, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    const int x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - width) / 2;
    const int y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - height) / 2;
    return CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT,
        kWindowClass, LOC(STR_TITLE), WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y, width, height, state.request.owner, nullptr, ModInstance(), &state);
}

class PickerCompletionSignal {
   public:
    explicit PickerCompletionSignal(HANDLE event) : event_(event) {}
    ~PickerCompletionSignal() {
        if (event_) SetEvent(event_);
    }
    PickerCompletionSignal(const PickerCompletionSignal&) = delete;
    PickerCompletionSignal& operator=(const PickerCompletionSignal&) = delete;

   private:
    HANDLE event_;
};

static void ShowPicker(PickerRequest request) {
    DetermineLocale();
    PickerCompletionSignal completion(request.completionEvent);
    if (g_shuttingDown.load(std::memory_order_acquire) || !IsSupportedFile(request.path)) return;
    PickerState state;
    state.request = std::move(request);
    RefreshPickerThemeResources(state);
    EnumerateHandlers(state);
    HWND window = CreatePickerWindow(state);
    if (!window) return;
    WindowOwner windowOwner(window);
    ShowWindow(window, SW_SHOWNORMAL);
    ActivatePickerWindow(window);

    HANDLE stopHandle = g_stopEvent.get();
    while (!state.finished) {
        const DWORD wait = MsgWaitForMultipleObjects(1, &stopHandle, FALSE,
                                                      INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            if (IsWindow(window)) DestroyWindow(window);
        }
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (!IsWindow(window)) break;
            if (!IsDialogMessageW(window, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        if (!IsWindow(window)) state.finished = true;
    }
    if (!state.accepted || state.chosenIndex < 0 ||
        static_cast<size_t>(state.chosenIndex) >= state.handlers.size() ||
        g_shuttingDown.load(std::memory_order_acquire)) return;
    HandlerEntry& selected =
        state.handlers[static_cast<size_t>(state.chosenIndex)];

    if (selected.browsed) {
        EnsureUserApplicationRegistration(selected.internalName,
                                          ExtensionOf(state.request.path),
                                          &selected.progId);
    }

    HRESULT defaultHr = S_OK;
    if (state.makeDefaultRequested)
        defaultHr = MakeSelectedDefault(state, selected);

    HRESULT invokeHr = S_OK;
    if (!state.request.setDefaultOnly) {
        invokeHr = selected.browsed
            ? InvokeBrowsed(state, selected.internalName)
            : InvokeSelectedHandler(state, selected);
    }

    if (FAILED(defaultHr) &&
        g_defaultBehavior.load(std::memory_order_acquire) ==
            DefaultBehavior::OpenSettings) {
        state.openDefaultSettings = true;
    }
    if (FAILED(defaultHr) || FAILED(invokeHr)) {
        Wh_Log(L"Standalone Open With: request failed default=0x%08X "
               L"invoke=0x%08X",
               static_cast<unsigned int>(defaultHr),
               static_cast<unsigned int>(invokeHr));
        MessageBeep(MB_ICONERROR);
    }

    if (state.openDefaultSettings) {
        ShellExecuteW(nullptr, L"open", L"ms-settings:defaultapps", nullptr,
                      nullptr, SW_SHOWNORMAL);
    }
}

static void WorkerMain() {
    ComApartment apartment(COINIT_APARTMENTTHREADED);
    if (!apartment.Ready()) {
        Wh_Log(L"Standalone Open With: worker COM initialization failed (0x%08X)",
               static_cast<unsigned int>(apartment.Result()));
        return;
    }
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_LISTVIEW_CLASSES | ICC_LINK_CLASS | ICC_STANDARD_CLASSES};
    if (!InitCommonControlsEx(&controls)) {
        Wh_Log(L"Standalone Open With: common-controls initialization failed");
        return;
    }
    MSG createQueue{};
    PeekMessageW(&createQueue, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    PickerClassRegistration classRegistration;
    if (!classRegistration.Register()) {
        Wh_Log(L"Standalone Open With: window class registration failed");
        return;
    }
    g_workerReady.store(true, std::memory_order_release);
    if (g_workerReadyEvent) SetEvent(g_workerReadyEvent.get());
    HANDLE handles[] = {g_stopEvent.get(), g_requestEvent.get()};
    for (;;) {
        const DWORD wait = MsgWaitForMultipleObjects(2, handles, FALSE, INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_OBJECT_0 + 1) {
            std::optional<PickerRequest> request;
            {
                std::lock_guard<std::mutex> lock(g_requestMutex);
                request.swap(g_pendingRequest);
            }
            if (request) {
                try { ShowPicker(std::move(*request)); }
                catch (...) { Wh_Log(L"Standalone Open With: picker request failed"); }
            }
        }
        if (wait == WAIT_OBJECT_0 + 2) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }
    g_workerReady.store(false, std::memory_order_release);
    if (HWND window = g_currentWindow.load(std::memory_order_acquire))
        SendMessageW(window, WM_CLOSE, 0, 0);
}

static void WorkerMainNoexcept() {
    try {
        WorkerMain();
    } catch (...) {
        g_workerReady.store(false, std::memory_order_release);
        Wh_Log(L"Standalone Open With: worker exception contained");
    }
    // Also release a waiter if initialization failed before Ready was set.
    if (g_workerReadyEvent) SetEvent(g_workerReadyEvent.get());
}

static bool WaitForPickerWorker(DWORD timeoutMilliseconds) {
    if (g_workerReady.load(std::memory_order_acquire)) return true;
    if (!g_workerReadyEvent) return false;
    WaitForSingleObject(g_workerReadyEvent.get(), timeoutMilliseconds);
    return g_workerReady.load(std::memory_order_acquire);
}

static bool QueuePicker(HWND owner, PCWSTR path,
                        bool setDefaultOnly = false) {
    if (!g_replaceSystemDialog.load(std::memory_order_acquire) ||
        !g_workerReady.load(std::memory_order_acquire) ||
        g_shuttingDown.load(std::memory_order_acquire) || !path) {
        Wh_Log(L"Standalone Open With: QueuePicker rejected early (replace=%d "
               L"workerReady=%d shuttingDown=%d hasPath=%d)",
               g_replaceSystemDialog.load(std::memory_order_acquire),
               g_workerReady.load(std::memory_order_acquire),
               g_shuttingDown.load(std::memory_order_acquire), path ? 1 : 0);
        return false;
    }
    std::wstring copy;
    try { copy = path; } catch (...) { return false; }
    if (!IsSupportedFile(copy)) {
        Wh_Log(L"Standalone Open With: QueuePicker rejected, unsupported file "
               L"(path=%s)", copy.c_str());
        return false;
    }

    if (HWND current = g_currentWindow.load(std::memory_order_acquire)) {
        PostMessageW(current, WM_SOW_ACTIVATE, 0, 0);
        return false;
    }

    std::lock_guard<std::mutex> lock(g_requestMutex);
    if (g_pendingRequest) {
        return false;
    }
    if (HWND current = g_currentWindow.load(std::memory_order_acquire)) {
        PostMessageW(current, WM_SOW_ACTIVATE, 0, 0);
        return false;
    }
    try {
        g_pendingRequest.emplace(
            PickerRequest{std::move(copy), owner, nullptr, setDefaultOnly});
    } catch (...) {
        return false;
    }
    if (!SetEvent(g_requestEvent.get())) { g_pendingRequest.reset(); return false; }
    return true;
}

static bool QueuePickerAndWait(HWND owner, PCWSTR path,
                               bool setDefaultOnly = false) {
    if (!path || !WaitForPickerWorker(3000) ||
        !g_replaceSystemDialog.load(std::memory_order_acquire) ||
        g_shuttingDown.load(std::memory_order_acquire)) {
        return false;
    }

    std::wstring copy;
    try {
        copy = path;
    } catch (...) {
        return false;
    }
    if (!IsSupportedFile(copy)) return false;

    WinHandle completion(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!completion) return false;
    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        if (g_pendingRequest ||
            g_currentWindow.load(std::memory_order_acquire)) {
            return false;
        }
        try {
            g_pendingRequest.emplace(
                PickerRequest{std::move(copy), owner, completion.get(),
                              setDefaultOnly});
        } catch (...) {
            return false;
        }
        if (!SetEvent(g_requestEvent.get())) {
            g_pendingRequest.reset();
            return false;
        }
    }

    HANDLE waits[] = {completion.get(), g_stopEvent.get()};
    DWORD completedIndex = 0;
    HRESULT waitResult = CoWaitForMultipleHandles(
        COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES,
        INFINITE, ARRAYSIZE(waits), waits, &completedIndex);
    if (FAILED(waitResult)) {
        const DWORD wait = WaitForMultipleObjects(ARRAYSIZE(waits), waits,
                                                   FALSE, INFINITE);
        completedIndex = wait == WAIT_OBJECT_0 ? 0 : 1;
    }
    return completedIndex == 0;
}

// -----------------------------------------------------------------------------
// Stable Open-With context-menu boundary (ReactOS-inspired).
// -----------------------------------------------------------------------------

// ReactOS implements Open With as the CLSID_OpenWithMenu shell extension. The
// stable boundary is IShellExtInit::Initialize (receives the selected item) plus
// IContextMenu::InvokeCommand (receives the canonical "openas" verb), before any
// Windows-version-specific picker implementation is involved.
static const CLSID kClsidOpenWithMenu = {
    0x09799AFB,
    0xAD67,
    0x11D1,
    {0xAB, 0xCD, 0x00, 0xC0, 0x4F, 0xC3, 0x09, 0x36}};

struct OpenWithMenuState {
    std::wstring path;
    int openAsOffset = -1;
};

static std::mutex g_openWithMenuStateMutex;
static std::unordered_map<void*, OpenWithMenuState> g_openWithMenuStates;

static void* ComIdentity(IUnknown* object) {
    if (!object) return nullptr;
    IUnknown* identity = nullptr;
    if (SUCCEEDED(object->QueryInterface(IID_IUnknown,
                                         reinterpret_cast<void**>(&identity))) &&
        identity) {
        void* key = identity;
        identity->Release();
        return key;
    }
    return object;
}

using SHCreateShellItemArrayFromDataObject_t = HRESULT(WINAPI*)(
    IDataObject*, REFIID, void**);

static std::wstring PathFromDataObject(IDataObject* dataObject) {
    if (!dataObject) return {};

    // Prefer the documented Shell item-array conversion. Resolve it at runtime
    // so an older SDK or OS doesn't become a hard dependency.
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    auto createArray = shell32
        ? reinterpret_cast<SHCreateShellItemArrayFromDataObject_t>(
              GetProcAddress(shell32,
                             "SHCreateShellItemArrayFromDataObject"))
        : nullptr;
    if (createArray) {
        ComPtr<IShellItemArray> array;
        if (SUCCEEDED(createArray(dataObject, IID_IShellItemArray,
                                  reinterpret_cast<void**>(array.Put()))) &&
            array) {
            DWORD count = 0;
            if (SUCCEEDED(array->GetCount(&count)) && count == 1) {
                ComPtr<IShellItem> item;
                if (SUCCEEDED(array->GetItemAt(0, item.Put())) && item) {
                    PWSTR raw = nullptr;
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH,
                                                       &raw)) && raw) {
                        return TakeTaskString(raw);
                    }
                    if (raw) CoTaskMemFree(raw);
                }
            }
        }
    }

    // CF_HDROP fallback for callers which expose only a classic data object.
    FORMATETC format{};
    format.cfFormat = CF_HDROP;
    format.dwAspect = DVASPECT_CONTENT;
    format.lindex = -1;
    format.tymed = TYMED_HGLOBAL;
    STGMEDIUM medium{};
    if (FAILED(dataObject->GetData(&format, &medium))) return {};

    std::wstring result;
    void* locked = medium.hGlobal ? GlobalLock(medium.hGlobal) : nullptr;
    if (locked) {
        HDROP drop = reinterpret_cast<HDROP>(locked);
        if (DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0) == 1) {
            const UINT chars = DragQueryFileW(drop, 0, nullptr, 0);
            if (chars && chars < 32767) {
                try {
                    std::vector<wchar_t> path(chars + 1);
                    if (DragQueryFileW(drop, 0, path.data(),
                                       static_cast<UINT>(path.size()))) {
                        result.assign(path.data());
                    }
                } catch (...) {
                }
            }
        }
        GlobalUnlock(medium.hGlobal);
    }
    ReleaseStgMedium(&medium);
    return result;
}

static bool CanonicalVerbIsOpenAs(IContextMenu* menu, UINT_PTR offset) {
    if (!menu) return false;

    wchar_t wide[64] = {};
    if (SUCCEEDED(menu->GetCommandString(
            offset, GCS_VERBW, nullptr, reinterpret_cast<LPSTR>(wide),
            ARRAYSIZE(wide))) &&
        !_wcsicmp(wide, L"openas")) {
        return true;
    }

    char narrow[64] = {};
    return SUCCEEDED(menu->GetCommandString(offset, GCS_VERBA, nullptr,
                                            narrow, ARRAYSIZE(narrow))) &&
           !_stricmp(narrow, "openas");
}

using OpenWithMenuInitialize_t = HRESULT(STDMETHODCALLTYPE*)(
    IShellExtInit*, PCIDLIST_ABSOLUTE, IDataObject*, HKEY);
using OpenWithMenuQueryContextMenu_t = HRESULT(STDMETHODCALLTYPE*)(
    IContextMenu*, HMENU, UINT, UINT, UINT, UINT);
using OpenWithMenuInvokeCommand_t = HRESULT(STDMETHODCALLTYPE*)(
    IContextMenu*, LPCMINVOKECOMMANDINFO);

static OpenWithMenuInitialize_t OpenWithMenuInitializeOriginal = nullptr;
static OpenWithMenuQueryContextMenu_t OpenWithMenuQueryContextMenuOriginal =
    nullptr;
static OpenWithMenuInvokeCommand_t OpenWithMenuInvokeCommandOriginal = nullptr;

static HRESULT STDMETHODCALLTYPE OpenWithMenuInitializeHook(
    IShellExtInit* self, PCIDLIST_ABSOLUTE folder, IDataObject* dataObject,
    HKEY classKey) {
    const HRESULT hr = OpenWithMenuInitializeOriginal
                           ? OpenWithMenuInitializeOriginal(
                                 self, folder, dataObject, classKey)
                           : E_FAIL;
    try {
        std::wstring path = PathFromDataObject(dataObject);
        void* identity = ComIdentity(static_cast<IUnknown*>(self));
        if (identity) {
            std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
            if (IsSupportedFile(path)) {
                if (g_openWithMenuStates.size() > 256)
                    g_openWithMenuStates.clear();
                g_openWithMenuStates[identity] =
                    OpenWithMenuState{std::move(path), -1};
                Wh_Log(L"Standalone Open With: captured CLSID_OpenWithMenu item");
            } else {
                g_openWithMenuStates.erase(identity); // Clean up stale state
            }
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: context-menu Initialize hook exception");
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE OpenWithMenuQueryContextMenuHook(
    IContextMenu* self, HMENU menu, UINT index, UINT first, UINT last,
    UINT flags) {
    const HRESULT hr = OpenWithMenuQueryContextMenuOriginal
                           ? OpenWithMenuQueryContextMenuOriginal(
                                 self, menu, index, first, last, flags)
                           : E_FAIL;
    try {
        if (SUCCEEDED(hr)) {
            const UINT count = HRESULT_CODE(hr);
            int openAsOffset = -1;
            for (UINT offset = 0; offset < count; ++offset) {
                if (CanonicalVerbIsOpenAs(self, offset)) {
                    openAsOffset = static_cast<int>(offset);
                    break;
                }
            }
            if (openAsOffset >= 0) {
                void* identity = ComIdentity(static_cast<IUnknown*>(self));
                std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
                auto found = g_openWithMenuStates.find(identity);
                if (found != g_openWithMenuStates.end())
                    found->second.openAsOffset = openAsOffset;
                Wh_Log(L"Standalone Open With: openas command offset=%d",
                       openAsOffset);
            }
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: QueryContextMenu hook exception");
    }
    return hr;
}

static bool InvocationUsesOpenAs(IContextMenu* self,
                                 LPCMINVOKECOMMANDINFO info,
                                 int knownOffset) {
    if (!info) return false;

    // MAKEINTRESOURCE(0) is a null pointer. It is nevertheless the valid
    // numeric command offset zero, which is exactly what CLSID_OpenWithMenu
    // reports on the affected Windows 10 build.
    if (IS_INTRESOURCE(info->lpVerb)) {
        const UINT_PTR offset = LOWORD(reinterpret_cast<ULONG_PTR>(info->lpVerb));
        if ((knownOffset >= 0 && offset == static_cast<UINT_PTR>(knownOffset)) ||
            CanonicalVerbIsOpenAs(self, offset)) {
            return true;
        }
    } else if (info->lpVerb && !_stricmp(info->lpVerb, "openas")) {
        return true;
    }

    if (info->cbSize >= sizeof(CMINVOKECOMMANDINFOEX) &&
        (info->fMask & CMIC_MASK_UNICODE)) {
        auto extended =
            reinterpret_cast<const CMINVOKECOMMANDINFOEX*>(info);
        if (extended->lpVerbW && !IS_INTRESOURCE(extended->lpVerbW) &&
            !_wcsicmp(extended->lpVerbW, L"openas")) {
            return true;
        }
    }
    return false;
}

static HRESULT STDMETHODCALLTYPE OpenWithMenuInvokeCommandHook(
    IContextMenu* self, LPCMINVOKECOMMANDINFO info) {
    try {
        std::wstring path;
        int openAsOffset = -1;
        void* identity = ComIdentity(static_cast<IUnknown*>(self));
        {
            std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
            auto found = g_openWithMenuStates.find(identity);
            if (found != g_openWithMenuStates.end()) {
                path = found->second.path;
                openAsOffset = found->second.openAsOffset;
            }
        }

        const bool openAs = InvocationUsesOpenAs(self, info, openAsOffset);
        const bool numericVerb = info && IS_INTRESOURCE(info->lpVerb);
        const UINT_PTR invokedOffset = numericVerb
            ? LOWORD(reinterpret_cast<ULONG_PTR>(info->lpVerb))
            : static_cast<UINT_PTR>(-1);
        Wh_Log(L"Standalone Open With: IContextMenu::InvokeCommand "
               L"openas=%d hasPath=%d numeric=%d offset=%llu known=%d",
               openAs, path.empty() ? 0 : 1, numericVerb,
               static_cast<unsigned long long>(invokedOffset), openAsOffset);
        if (openAs && !path.empty() &&
            QueuePicker(info ? info->hwnd : nullptr, path.c_str())) {
            std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
            g_openWithMenuStates.erase(identity);
            return S_OK;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: InvokeCommand hook exception; "
               L"falling back to Windows");
    }

    return OpenWithMenuInvokeCommandOriginal
               ? OpenWithMenuInvokeCommandOriginal(self, info)
               : E_FAIL;
}

template <typename Interface>
static void* InterfaceMethod(Interface* object, size_t index) {
    return object ? (*reinterpret_cast<void***>(object))[index] : nullptr;
}

static bool InstallOpenWithMenuMethodHooks() {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitialize = SUCCEEDED(comResult);
    if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Standalone Open With: COM probe initialization failed "
               L"(0x%08X)", static_cast<unsigned int>(comResult));
        return false;
    }

    bool result = false;
    {
        ComPtr<IUnknown> unknown;
        HRESULT hr = CoCreateInstance(
            kClsidOpenWithMenu, nullptr, CLSCTX_INPROC_SERVER, IID_IUnknown,
            reinterpret_cast<void**>(unknown.Put()));
        ComPtr<IContextMenu> contextMenu;
        ComPtr<IShellExtInit> shellExtInit;
        if (SUCCEEDED(hr) && unknown) {
            unknown->QueryInterface(IID_IContextMenu,
                                    reinterpret_cast<void**>(contextMenu.Put()));
            unknown->QueryInterface(IID_IShellExtInit,
                                    reinterpret_cast<void**>(shellExtInit.Put()));
        }

        if (contextMenu && shellExtInit) {
            auto initialize =
                reinterpret_cast<OpenWithMenuInitialize_t>(
                    InterfaceMethod(shellExtInit.Get(), 3));
            auto query =
                reinterpret_cast<OpenWithMenuQueryContextMenu_t>(
                    InterfaceMethod(contextMenu.Get(), 3));
            auto invoke =
                reinterpret_cast<OpenWithMenuInvokeCommand_t>(
                    InterfaceMethod(contextMenu.Get(), 4));

            bool initHook = initialize && WindhawkUtils::SetFunctionHook(
                initialize, OpenWithMenuInitializeHook,
                &OpenWithMenuInitializeOriginal);
            bool queryHook = query && WindhawkUtils::SetFunctionHook(
                query, OpenWithMenuQueryContextMenuHook,
                &OpenWithMenuQueryContextMenuOriginal);
            bool invokeHook = invoke && WindhawkUtils::SetFunctionHook(
                invoke, OpenWithMenuInvokeCommandHook,
                &OpenWithMenuInvokeCommandOriginal);
            result = initHook && invokeHook;
            Wh_Log(L"Standalone Open With: CLSID_OpenWithMenu vtable probe "
                   L"init=%p query=%p invoke=%p hooks=%d/%d/%d",
                   reinterpret_cast<void*>(initialize),
                   reinterpret_cast<void*>(query),
                   reinterpret_cast<void*>(invoke), initHook, queryHook,
                   invokeHook);
        } else {
            Wh_Log(L"Standalone Open With: CLSID_OpenWithMenu probe failed "
                   L"hr=0x%08X context=%d init=%d",
                   static_cast<unsigned int>(hr), contextMenu ? 1 : 0,
                   shellExtInit ? 1 : 0);
        }
    }

    if (uninitialize) CoUninitialize();
    return result;
}

// -----------------------------------------------------------------------------
// OpenWith.exe COM-server method detours.
//
// Unknown-file double click and Properties -> Change arrive at the local COM
// server without crossing an exported Shell function in Explorer. Hook the real
// class factory and the real object's methods; never return a mod-owned COM
// object, so no external reference can outlive the mod image.
// -----------------------------------------------------------------------------

static constexpr DWORD kImmersiveOpenWithDoNotExec = 0x00000004;

static bool ClassNameEquals(HWND window, PCWSTR expected) {
    if (!window || !expected) return false;
    wchar_t className[128] = {};
    return GetClassNameW(window, className, ARRAYSIZE(className)) &&
           !_wcsicmp(className, expected);
}

static bool IsFilePropertiesOwner(HWND owner) {
    if (!owner) return false;
    HWND root = GetAncestor(owner, GA_ROOT);
    if (!root) root = owner;
    if (!ClassNameEquals(root, L"#32770")) return false;
    return FindWindowExW(root, nullptr, WC_TABCONTROLW, nullptr) != nullptr;
}

static bool ShouldSetDefaultOnly(HWND owner, DWORD flags) {
    const bool properties =
        (flags & kImmersiveOpenWithDoNotExec) && IsFilePropertiesOwner(owner);
    Wh_Log(L"Standalone Open With: launcher intent flags=0x%08X owner=%p "
           L"setDefaultOnly=%d", flags, owner, properties);
    return properties;
}

MIDL_INTERFACE("6A283FE2-ECFA-4599-91C4-E80957137B26")
StandaloneOpenWithLauncher : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE Launch(HWND owner, PCWSTR path,
                                             DWORD flags) = 0;
};

static const CLSID kClsidExecuteUnknown = {
    0xE44E9428,
    0xBDBC,
    0x4987,
    {0xA0, 0x99, 0x40, 0xDC, 0x8F, 0xD2, 0x55, 0xE7}};
static const IID kIidOpenWithLauncher = {
    0x6A283FE2,
    0xECFA,
    0x4599,
    {0x91, 0xC4, 0xE8, 0x09, 0x57, 0x13, 0x7B, 0x26}};

struct ServerOpenWithState {
    std::wstring selectionPath;
    std::wstring parameterPath;
    HWND owner = nullptr;
};

static std::mutex g_serverHookMutex;
static std::mutex g_serverStateMutex;
static std::unordered_map<void*, ServerOpenWithState> g_serverStates;
static bool g_serverFactoryHookInstalled = false;

using CoRegisterClassObject_t = HRESULT(WINAPI*)(
    REFCLSID, IUnknown*, DWORD, DWORD, LPDWORD);
using ServerFactoryCreateInstance_t = HRESULT(STDMETHODCALLTYPE*)(
    IClassFactory*, IUnknown*, REFIID, void**);
using ServerLauncherLaunch_t = HRESULT(STDMETHODCALLTYPE*)(
    StandaloneOpenWithLauncher*, HWND, PCWSTR, DWORD);
using ServerExecute_t = HRESULT(STDMETHODCALLTYPE*)(IExecuteCommand*);
using ServerSetParameters_t = HRESULT(STDMETHODCALLTYPE*)(
    IExecuteCommand*, PCWSTR);
using ServerSetSelection_t = HRESULT(STDMETHODCALLTYPE*)(
    IObjectWithSelection*, IShellItemArray*);
using ServerSetSite_t = HRESULT(STDMETHODCALLTYPE*)(IObjectWithSite*, IUnknown*);

static CoRegisterClassObject_t CoRegisterClassObjectOriginal = nullptr;
static ServerFactoryCreateInstance_t ServerFactoryCreateInstanceOriginal =
    nullptr;
static ServerLauncherLaunch_t ServerLauncherLaunchOriginal = nullptr;
static ServerExecute_t ServerExecuteOriginal = nullptr;
static ServerSetParameters_t ServerSetParametersOriginal = nullptr;
static ServerSetSelection_t ServerSetSelectionOriginal = nullptr;
static ServerSetSite_t ServerSetSiteOriginal = nullptr;

static std::wstring PathFromShellItemArray(IShellItemArray* selection) {
    if (!selection) return {};
    DWORD count = 0;
    if (FAILED(selection->GetCount(&count)) || count != 1) return {};
    ComPtr<IShellItem> item;
    if (FAILED(selection->GetItemAt(0, item.Put())) || !item) return {};
    PWSTR raw = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) || !raw)
        return {};
    return TakeTaskString(raw);
}

static std::wstring PathFromExecuteParameters(PCWSTR parameters) {
    if (!parameters || !*parameters) return {};
    try {
        std::wstring candidate = parameters;
        const size_t first = candidate.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) return {};
        const size_t last = candidate.find_last_not_of(L" \t\r\n");
        candidate = candidate.substr(first, last - first + 1);
        if (candidate.size() >= 2 && candidate.front() == L'"' &&
            candidate.back() == L'"') {
            candidate = candidate.substr(1, candidate.size() - 2);
        }
        if (IsSupportedFile(candidate)) return candidate;

        int argc = 0;
        LPWSTR* argv = CommandLineToArgvW(parameters, &argc);
        if (!argv) return {};
        std::wstring result;
        if (argc == 1 && IsSupportedFile(argv[0])) result = argv[0];
        LocalFree(argv);
        return result;
    } catch (...) {
        return {};
    }
}

static HWND WindowFromSite(IUnknown* site) {
    if (!site) return nullptr;
    ComPtr<IOleWindow> oleWindow;
    if (FAILED(site->QueryInterface(
            IID_IOleWindow,
            reinterpret_cast<void**>(oleWindow.Put()))) || !oleWindow) {
        return nullptr;
    }
    HWND window = nullptr;
    return SUCCEEDED(oleWindow->GetWindow(&window)) ? window : nullptr;
}

template <typename Interface>
static void* ServerIdentity(Interface* object) {
    return object ? ComIdentity(static_cast<IUnknown*>(object)) : nullptr;
}

static void UpdateServerSelectionPath(void* identity, std::wstring path) {
    if (!identity) return;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    if (g_serverStates.size() > 256) g_serverStates.clear();
    g_serverStates[identity].selectionPath =
        IsSupportedFile(path) ? std::move(path) : std::wstring{};
}

static void UpdateServerParameterPath(void* identity, std::wstring path) {
    if (!identity) return;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    if (g_serverStates.size() > 256) g_serverStates.clear();
    g_serverStates[identity].parameterPath =
        IsSupportedFile(path) ? std::move(path) : std::wstring{};
}

static void UpdateServerOwner(void* identity, HWND owner) {
    if (!identity) return;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    if (g_serverStates.size() > 256) g_serverStates.clear();
    g_serverStates[identity].owner = owner;
}

static ServerOpenWithState TakeServerState(void* identity) {
    ServerOpenWithState state;
    if (!identity) return state;
    std::lock_guard<std::mutex> lock(g_serverStateMutex);
    auto found = g_serverStates.find(identity);
    if (found != g_serverStates.end()) {
        state = std::move(found->second);
        g_serverStates.erase(found);
    }
    return state;
}

static HRESULT STDMETHODCALLTYPE ServerSetSelectionHook(
    IObjectWithSelection* self, IShellItemArray* selection) {
    const HRESULT hr = ServerSetSelectionOriginal
                           ? ServerSetSelectionOriginal(self, selection)
                           : E_FAIL;
    if (SUCCEEDED(hr)) {
        try {
            UpdateServerSelectionPath(ServerIdentity(self),
                                      PathFromShellItemArray(selection));
        } catch (...) {
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE ServerSetParametersHook(
    IExecuteCommand* self, PCWSTR parameters) {
    const HRESULT hr = ServerSetParametersOriginal
                           ? ServerSetParametersOriginal(self, parameters)
                           : E_FAIL;
    if (SUCCEEDED(hr)) {
        try {
            UpdateServerParameterPath(
                ServerIdentity(self), PathFromExecuteParameters(parameters));
        } catch (...) {
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE ServerSetSiteHook(IObjectWithSite* self,
                                                    IUnknown* site) {
    const HRESULT hr = ServerSetSiteOriginal
                           ? ServerSetSiteOriginal(self, site)
                           : E_FAIL;
    if (SUCCEEDED(hr)) {
        try {
            UpdateServerOwner(ServerIdentity(self), WindowFromSite(site));
        } catch (...) {
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE ServerExecuteHook(IExecuteCommand* self) {
    try {
        ServerOpenWithState state = TakeServerState(ServerIdentity(self));
        if (!state.owner) state.owner = GetForegroundWindow();
        const std::wstring& path = IsSupportedFile(state.selectionPath)
                                       ? state.selectionPath
                                       : state.parameterPath;
        Wh_Log(L"Standalone Open With: server Execute path=%s owner=%p",
               path.empty() ? L"(empty)" : path.c_str(), state.owner);
        if (g_replaceSystemDialog.load(std::memory_order_acquire) &&
            IsSupportedFile(path) &&
            QueuePickerAndWait(state.owner, path.c_str())) {
            return S_OK;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: server Execute hook exception");
    }
    return ServerExecuteOriginal ? ServerExecuteOriginal(self) : E_FAIL;
}

static HRESULT STDMETHODCALLTYPE ServerLauncherLaunchHook(
    StandaloneOpenWithLauncher* self, HWND owner, PCWSTR path, DWORD flags) {
    try {
        const bool setDefaultOnly = ShouldSetDefaultOnly(owner, flags);
        Wh_Log(L"Standalone Open With: server Launch path=%s owner=%p "
               L"flags=0x%08X", path ? path : L"(null)", owner, flags);
        if (g_replaceSystemDialog.load(std::memory_order_acquire) && path &&
            IsSupportedFile(path) &&
            QueuePickerAndWait(owner, path, setDefaultOnly)) {
            return S_OK;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: server Launch hook exception");
    }
    return ServerLauncherLaunchOriginal
               ? ServerLauncherLaunchOriginal(self, owner, path, flags)
               : E_FAIL;
}

static bool InstallServerObjectMethodHooks(IUnknown* object) {
    if (!object || g_shuttingDown.load(std::memory_order_acquire)) return false;

    ComPtr<StandaloneOpenWithLauncher> launcher;
    object->QueryInterface(kIidOpenWithLauncher,
                           reinterpret_cast<void**>(launcher.Put()));
    ComPtr<IExecuteCommand> execute;
    object->QueryInterface(IID_IExecuteCommand,
                           reinterpret_cast<void**>(execute.Put()));
    ComPtr<IObjectWithSelection> selection;
    object->QueryInterface(IID_IObjectWithSelection,
                           reinterpret_cast<void**>(selection.Put()));
    ComPtr<IObjectWithSite> site;
    object->QueryInterface(IID_IObjectWithSite,
                           reinterpret_cast<void**>(site.Put()));

    std::lock_guard<std::mutex> lock(g_serverHookMutex);
    bool registered = false;
    if (launcher && !ServerLauncherLaunchOriginal) {
        auto method = reinterpret_cast<ServerLauncherLaunch_t>(
            InterfaceMethod(launcher.Get(), 3));
        registered |= method && WindhawkUtils::SetFunctionHook(
            method, ServerLauncherLaunchHook, &ServerLauncherLaunchOriginal);
    }
    if (execute) {
        if (!ServerSetParametersOriginal) {
            auto method = reinterpret_cast<ServerSetParameters_t>(
                InterfaceMethod(execute.Get(), 4));
            registered |= method && WindhawkUtils::SetFunctionHook(
                method, ServerSetParametersHook,
                &ServerSetParametersOriginal);
        }
        if (!ServerExecuteOriginal) {
            auto method = reinterpret_cast<ServerExecute_t>(
                InterfaceMethod(execute.Get(), 9));
            registered |= method && WindhawkUtils::SetFunctionHook(
                method, ServerExecuteHook, &ServerExecuteOriginal);
        }
    }
    if (selection && !ServerSetSelectionOriginal) {
        auto method = reinterpret_cast<ServerSetSelection_t>(
            InterfaceMethod(selection.Get(), 3));
        registered |= method && WindhawkUtils::SetFunctionHook(
            method, ServerSetSelectionHook, &ServerSetSelectionOriginal);
    }
    if (site && !ServerSetSiteOriginal) {
        auto method = reinterpret_cast<ServerSetSite_t>(
            InterfaceMethod(site.Get(), 3));
        registered |= method && WindhawkUtils::SetFunctionHook(
            method, ServerSetSiteHook, &ServerSetSiteOriginal);
    }

    if (registered && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Standalone Open With: failed to apply server object hooks");
        return false;
    }
    Wh_Log(L"Standalone Open With: server object methods launch=%d "
           L"parameters=%d execute=%d selection=%d site=%d",
           ServerLauncherLaunchOriginal != nullptr,
           ServerSetParametersOriginal != nullptr,
           ServerExecuteOriginal != nullptr,
           ServerSetSelectionOriginal != nullptr,
           ServerSetSiteOriginal != nullptr);
    return registered || ServerLauncherLaunchOriginal || ServerExecuteOriginal;
}

static HRESULT STDMETHODCALLTYPE ServerFactoryCreateInstanceHook(
    IClassFactory* self, IUnknown* outer, REFIID iid, void** object) {
    const HRESULT hr = ServerFactoryCreateInstanceOriginal
                           ? ServerFactoryCreateInstanceOriginal(
                                 self, outer, iid, object)
                           : E_FAIL;
    if (SUCCEEDED(hr) && !outer && object && *object) {
        try {
            InstallServerObjectMethodHooks(
                reinterpret_cast<IUnknown*>(*object));
        } catch (...) {
            Wh_Log(L"Standalone Open With: server object probe exception");
        }
    }
    return hr;
}

static bool InstallServerFactoryHook(IUnknown* classObject) {
    if (!classObject) return false;
    ComPtr<IClassFactory> factory;
    if (FAILED(classObject->QueryInterface(
            IID_IClassFactory,
            reinterpret_cast<void**>(factory.Put()))) || !factory) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_serverHookMutex);
    if (g_serverFactoryHookInstalled) return true;
    auto createInstance = reinterpret_cast<ServerFactoryCreateInstance_t>(
        InterfaceMethod(factory.Get(), 3));
    if (!createInstance || !WindhawkUtils::SetFunctionHook(
                               createInstance,
                               ServerFactoryCreateInstanceHook,
                               &ServerFactoryCreateInstanceOriginal)) {
        Wh_Log(L"Standalone Open With: failed to register server factory hook");
        return false;
    }
    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"Standalone Open With: failed to apply server factory hook");
        return false;
    }
    g_serverFactoryHookInstalled = true;
    Wh_Log(L"Standalone Open With: server class factory hooked at %p",
           reinterpret_cast<void*>(createInstance));
    return true;
}

static HRESULT WINAPI CoRegisterClassObjectHook(
    REFCLSID clsid, IUnknown* classObject, DWORD context, DWORD flags,
    LPDWORD registration) {
    try {
        if (IsEqualCLSID(clsid, kClsidExecuteUnknown)) {
            Wh_Log(L"Standalone Open With: ExecuteUnknown class registered");
            InstallServerFactoryHook(classObject);
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: class registration hook exception");
    }
    return CoRegisterClassObjectOriginal
               ? CoRegisterClassObjectOriginal(clsid, classObject, context,
                                               flags, registration)
               : E_FAIL;
}

// -----------------------------------------------------------------------------
// Direct OpenWith.exe command-line interception.
// -----------------------------------------------------------------------------

// Direct OpenWith.exe command lines (drag/drop or explicit launch) don't use
// the COM server. If a real file is present in argv, replace the executable's
// entry point and run the standalone picker instead.
using ProcessEntryPoint_t = void(WINAPI*)();
static ProcessEntryPoint_t OpenWithEntryPointOriginal = nullptr;
static std::wstring g_directOpenWithPath;

static void WINAPI OpenWithEntryPointHook() {
    Wh_Log(L"Standalone Open With: intercepted direct OpenWith.exe entry "
           L"path=%s", g_directOpenWithPath.c_str());
    bool handled = false;
    if (IsSupportedFile(g_directOpenWithPath) &&
        g_replaceSystemDialog.load(std::memory_order_acquire)) {
        handled = QueuePickerAndWait(nullptr, g_directOpenWithPath.c_str());
    }
    if (handled) {
        ExitProcess(0);
    } else if (OpenWithEntryPointOriginal) {
        OpenWithEntryPointOriginal();
    } else {
        ExitProcess(0);
    }
}

static bool InstallDirectOpenWithEntryHook(PCWSTR path) {
    if (!path || !IsSupportedFile(path)) return false;
    try {
        g_directOpenWithPath = path;
    } catch (...) {
        return false;
    }

    HMODULE executable = GetModuleHandleW(nullptr);
    if (!executable) return false;
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(executable);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<const BYTE*>(executable) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        !nt->OptionalHeader.AddressOfEntryPoint) {
        return false;
    }
    auto entry = reinterpret_cast<ProcessEntryPoint_t>(
        reinterpret_cast<BYTE*>(executable) +
        nt->OptionalHeader.AddressOfEntryPoint);
    const bool hooked = WindhawkUtils::SetFunctionHook(
        entry, OpenWithEntryPointHook, &OpenWithEntryPointOriginal);
    Wh_Log(L"Standalone Open With: direct entry hook=%d entry=%p path=%s",
           hooked, reinterpret_cast<void*>(entry), path);
    return hooked;
}

using CreateProcessW_t = BOOL(WINAPI*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
                                       LPSECURITY_ATTRIBUTES, BOOL, DWORD,
                                       LPVOID, LPCWSTR, LPSTARTUPINFOW,
                                       LPPROCESS_INFORMATION);
static CreateProcessW_t CreateProcessWOriginal = nullptr;

// Extracts the file path argument from an "OpenWith.exe -c <path>"-style
// command line. Returns an empty string if nothing usable is found.
static std::wstring ExtractOpenWithTargetPath(LPCWSTR commandLine) {
    if (!commandLine || !*commandLine) return L"";
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);
    if (!argv) return L"";
    std::wstring result;
    try {
        for (int i = 0; i < argc; ++i) {
            if (!_wcsicmp(argv[i], L"-c") && i + 1 < argc) {
                result = argv[i + 1];
                break;
            }
        }
        if (result.empty() && argc > 1) {
            // Fall back to the last argument if no explicit -c flag is found.
            result = argv[argc - 1];
        }
    } catch (...) {
        result.clear();
    }
    LocalFree(argv);
    return result;
}

static BOOL WINAPI CreateProcessWHook(
    LPCWSTR applicationName, LPWSTR commandLine,
    LPSECURITY_ATTRIBUTES processAttributes,
    LPSECURITY_ATTRIBUTES threadAttributes, BOOL inheritHandles,
    DWORD creationFlags, LPVOID environment, LPCWSTR currentDirectory,
    LPSTARTUPINFOW startupInfo, LPPROCESS_INFORMATION processInformation) {
    try {
        const std::wstring exe = applicationName
                                     ? applicationName
                                     : ExecutableFromCommand(commandLine ? commandLine : L"");
        const bool looksLikeOpenWith =
            !exe.empty() && !_wcsicmp(PathFindFileNameW(exe.c_str()), L"OpenWith.exe");
        if (looksLikeOpenWith &&
            g_replaceSystemDialog.load(std::memory_order_acquire)) {
            std::wstring path = ExtractOpenWithTargetPath(commandLine);
            if (!path.empty() && QueuePicker(nullptr, path.c_str())) {
                Wh_Log(L"Standalone Open With: blocked OpenWith.exe launch, "
                       L"showing custom picker (path=%s)", path.c_str());
                if (processInformation) {
                    ZeroMemory(processInformation, sizeof(*processInformation));
                }
                SetLastError(ERROR_CANCELLED);
                return FALSE;
            }
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: CreateProcessW hook exception, "
               L"falling back to original");
    }
    return CreateProcessWOriginal
        ? CreateProcessWOriginal(applicationName, commandLine,
                                 processAttributes, threadAttributes,
                                 inheritHandles, creationFlags, environment,
                                 currentDirectory, startupInfo,
                                 processInformation)
        : FALSE;
}

using SHOpenWithDialog_t = HRESULT(WINAPI*)(HWND, const OPENASINFO*);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR,
                                           LPCWSTR, INT);
static SHOpenWithDialog_t SHOpenWithDialogOriginal = nullptr;
static ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;
static ShellExecuteW_t ShellExecuteWOriginal = nullptr;

static bool IsOpenAsVerb(PCWSTR verb) {
    return verb && !_wcsicmp(verb, L"openas");
}

static HRESULT WINAPI SHOpenWithDialogHook(HWND owner, const OPENASINFO* info) {
    try {
        if (info && (info->oaifInFlags & OAIF_EXEC) &&
            g_replaceSystemDialog.load(std::memory_order_acquire) &&
            info->pcszFile && IsSupportedFile(info->pcszFile)) {
            if (QueuePickerAndWait(owner, info->pcszFile)) {
                return S_OK;
            }
        }
        Wh_Log(L"Standalone Open With: SHOpenWithDialog falling back to system "
               L"dialog (hasInfo=%d flags=0x%X file=%s replace=%d workerReady=%d "
               L"shuttingDown=%d)",
               info ? 1 : 0, info ? info->oaifInFlags : 0,
               info && info->pcszFile ? info->pcszFile : L"(null)",
               g_replaceSystemDialog.load(std::memory_order_acquire),
               g_workerReady.load(std::memory_order_acquire),
               g_shuttingDown.load(std::memory_order_acquire));
    } catch (...) {}
    return SHOpenWithDialogOriginal ? SHOpenWithDialogOriginal(owner, info) : E_FAIL;
}

static BOOL WINAPI ShellExecuteExWHook(SHELLEXECUTEINFOW* info) {
    try {
        if (info && IsOpenAsVerb(info->lpVerb) &&
            g_replaceSystemDialog.load(std::memory_order_acquire) &&
            info->lpFile && IsSupportedFile(info->lpFile)) {
            if (QueuePickerAndWait(info->hwnd, info->lpFile)) {
                info->hInstApp = reinterpret_cast<HINSTANCE>(33);
                info->hProcess = nullptr;
                return TRUE;
            }
        }
        if (info && IsOpenAsVerb(info->lpVerb)) {
            Wh_Log(L"Standalone Open With: ShellExecuteExW(openas) falling back "
                   L"to system dialog (file=%s replace=%d workerReady=%d "
                   L"shuttingDown=%d)",
                   info->lpFile ? info->lpFile : L"(null)",
                   g_replaceSystemDialog.load(std::memory_order_acquire),
                   g_workerReady.load(std::memory_order_acquire),
                   g_shuttingDown.load(std::memory_order_acquire));
        }
    } catch (...) {}
    return ShellExecuteExWOriginal ? ShellExecuteExWOriginal(info) : FALSE;
}

static HINSTANCE WINAPI ShellExecuteWHook(HWND owner, LPCWSTR verb, LPCWSTR file,
                                          LPCWSTR parameters, LPCWSTR directory,
                                          INT show) {
    try {
        if (IsOpenAsVerb(verb) && QueuePicker(owner, file))
            return reinterpret_cast<HINSTANCE>(33);
        if (IsOpenAsVerb(verb)) {
            Wh_Log(L"Standalone Open With: ShellExecuteW(openas) falling back "
                   L"to system dialog (file=%s replace=%d workerReady=%d "
                   L"shuttingDown=%d)",
                   file ? file : L"(null)",
                   g_replaceSystemDialog.load(std::memory_order_acquire),
                   g_workerReady.load(std::memory_order_acquire),
                   g_shuttingDown.load(std::memory_order_acquire));
        }
    } catch (...) {}
    return ShellExecuteWOriginal
        ? ShellExecuteWOriginal(owner, verb, file, parameters, directory, show)
        : reinterpret_cast<HINSTANCE>(SE_ERR_ACCESSDENIED);
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle.
// -----------------------------------------------------------------------------

static std::atomic<bool> g_isExplorerProcess{false};

static void StopWorker() {
    g_shuttingDown.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent.get());
    if (HWND browse = g_activeBrowseHwnd.load(std::memory_order_acquire))
        PostMessageW(browse, WM_CLOSE, 0, 0);
    if (HWND window = g_currentWindow.load(std::memory_order_acquire))
        PostMessageW(window, WM_CLOSE, 0, 0);
    if (g_worker) {
        g_worker->join();
        g_worker.reset();
    }
    g_activeBrowseHwnd.store(nullptr, std::memory_order_release);
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExWOriginal = nullptr;
static std::mutex g_shell32HookMutex;

static bool HookShell32Exports(HMODULE shell32) {
    if (!shell32 || g_shuttingDown.load(std::memory_order_acquire))
        return false;

    std::lock_guard<std::mutex> lock(g_shell32HookMutex);
    bool registered = false;
    auto openWith = reinterpret_cast<SHOpenWithDialog_t>(
        GetProcAddress(shell32, "SHOpenWithDialog"));
    auto executeEx = reinterpret_cast<ShellExecuteExW_t>(
        GetProcAddress(shell32, "ShellExecuteExW"));
    auto execute = reinterpret_cast<ShellExecuteW_t>(
        GetProcAddress(shell32, "ShellExecuteW"));
    if (openWith && !SHOpenWithDialogOriginal) {
        registered |= WindhawkUtils::SetFunctionHook(
            openWith, SHOpenWithDialogHook, &SHOpenWithDialogOriginal);
    }
    if (executeEx && !ShellExecuteExWOriginal) {
        registered |= WindhawkUtils::SetFunctionHook(
            executeEx, ShellExecuteExWHook, &ShellExecuteExWOriginal);
    }
    if (execute && !ShellExecuteWOriginal) {
        registered |= WindhawkUtils::SetFunctionHook(
            execute, ShellExecuteWHook, &ShellExecuteWOriginal);
    }
    return registered;
}

static HMODULE WINAPI LoadLibraryExWHook(LPCWSTR name, HANDLE file,
                                         DWORD flags) {
    HMODULE result = LoadLibraryExWOriginal
                         ? LoadLibraryExWOriginal(name, file, flags)
                         : nullptr;
    constexpr DWORD kDataOnly =
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
        LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    if (result && name && !(flags & kDataOnly) &&
        !_wcsicmp(PathFindFileNameW(name), L"shell32.dll") &&
        HookShell32Exports(result) && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Standalone Open With: failed to apply late shell32 hooks");
    }
    return result;
}

static bool VerifySystemBinariesExist() {
    try {
        wchar_t winDir[MAX_PATH] = {};
        if (!GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir))) return false;

        wchar_t sysDir[MAX_PATH] = {};
        if (!GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir))) return false;

        wchar_t explorerPath[MAX_PATH] = {};
        if (swprintf_s(explorerPath, L"%s\\explorer.exe", winDir) <= 0)
            return false;

        wchar_t openWithPath[MAX_PATH] = {};
        if (swprintf_s(openWithPath, L"%s\\OpenWith.exe", sysDir) <= 0)
            return false;

        const DWORD explorerAttrs = GetFileAttributesW(explorerPath);
        if (explorerAttrs == INVALID_FILE_ATTRIBUTES ||
            (explorerAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"Standalone Open With: explorer.exe verification failed "
                   L"at %s", explorerPath);
            return false;
        }

        const DWORD openWithAttrs = GetFileAttributesW(openWithPath);
        if (openWithAttrs == INVALID_FILE_ATTRIBUTES ||
            (openWithAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"Standalone Open With: OpenWith.exe verification failed "
                   L"at %s", openWithPath);
            return false;
        }

        return true;
    } catch (...) {
        return false;
    }
}

BOOL Wh_ModInit() {
    try {
        if (!VerifySystemBinariesExist()) {
            Wh_Log(L"Standalone Open With: required system binaries not found, "
                   L"aborting initialization");
            return FALSE;
        }

        wchar_t moduleName[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, moduleName, ARRAYSIZE(moduleName));
        const PCWSTR processFileName = PathFindFileNameW(moduleName);
        const bool isExplorer = processFileName &&
            !_wcsicmp(processFileName, L"explorer.exe");
        const bool isOpenWith = processFileName &&
            !_wcsicmp(processFileName, L"OpenWith.exe");
        g_isExplorerProcess.store(isExplorer, std::memory_order_release);

        std::wstring directOpenWithPath;
        if (isOpenWith) {
            directOpenWithPath =
                ExtractOpenWithTargetPath(GetCommandLineW());
        }

        LoadSettings();
        g_shuttingDown.store(false, std::memory_order_release);
        g_stopEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
        g_requestEvent.reset(CreateEventW(nullptr, FALSE, FALSE, nullptr));
        g_workerReadyEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
        if (!g_stopEvent || !g_requestEvent || !g_workerReadyEvent)
            return FALSE;
        g_worker.emplace(WorkerMainNoexcept);

        bool hookedLoadLibrary = false;
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelbase) kernelbase = GetModuleHandleW(L"kernel32.dll");
        if (kernelbase) {
            auto loadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
                GetProcAddress(kernelbase, "LoadLibraryExW"));
            if (loadLibraryExW) {
                hookedLoadLibrary = WindhawkUtils::SetFunctionHook(
                    loadLibraryExW, LoadLibraryExWHook,
                    &LoadLibraryExWOriginal);
            }
        }

        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        const bool hookedShellExports = HookShell32Exports(shell32);

        bool hookedServerRegistration = false;
        bool hookedDirectEntry = false;
        if (isOpenWith) {
            HMODULE combase = GetModuleHandleW(L"combase.dll");
            auto registerClassObject = combase
                ? reinterpret_cast<CoRegisterClassObject_t>(
                      GetProcAddress(combase, "CoRegisterClassObject"))
                : nullptr;
            if (!registerClassObject) {
                HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
                registerClassObject = ole32
                    ? reinterpret_cast<CoRegisterClassObject_t>(
                          GetProcAddress(ole32, "CoRegisterClassObject"))
                    : nullptr;
            }
            if (registerClassObject) {
                hookedServerRegistration = WindhawkUtils::SetFunctionHook(
                    registerClassObject, CoRegisterClassObjectHook,
                    &CoRegisterClassObjectOriginal);
            }
            if (IsSupportedFile(directOpenWithPath)) {
                hookedDirectEntry = InstallDirectOpenWithEntryHook(
                    directOpenWithPath.c_str());
            }
        }

        bool hookedCreateProcess = false;
        if (isExplorer) {
            HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
            auto createProcessW = kernel32
                ? reinterpret_cast<CreateProcessW_t>(
                      GetProcAddress(kernel32, "CreateProcessW"))
                : nullptr;
            if (createProcessW) {
                hookedCreateProcess = WindhawkUtils::SetFunctionHook(
                    createProcessW, CreateProcessWHook,
                    &CreateProcessWOriginal);
            }
        }

        // The stable context-menu vtable probe is intentionally deferred to
        // Wh_ModAfterInit so Explorer's main thread is not forced to activate a
        // shell extension during process initialization.
        const bool anyHook = hookedLoadLibrary || hookedShellExports ||
                             hookedServerRegistration || hookedDirectEntry ||
                             hookedCreateProcess || isExplorer;
        Wh_Log(L"Standalone Open With: init process=%s pid=%u shell32=%p "
               L"loadLibrary=%d shellExports=%d serverRegistration=%d "
               L"directEntry=%d createProcess=%d deferredMenuProbe=%d",
               moduleName, GetCurrentProcessId(), shell32,
               hookedLoadLibrary, hookedShellExports,
               hookedServerRegistration, hookedDirectEntry,
               hookedCreateProcess, isExplorer);
        if (!anyHook) {
            Wh_Log(L"An error has been found! The mod will disable itself for security.\n");
            StopWorker();
            return FALSE;
        }
        return TRUE;
    } catch (...) {
        StopWorker();
        return FALSE;
    }
}

void Wh_ModAfterInit() {
    if (!g_isExplorerProcess.load(std::memory_order_acquire) ||
        g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    try {
        const bool hooked = InstallOpenWithMenuMethodHooks();
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Standalone Open With: failed to apply deferred "
                   L"CLSID_OpenWithMenu hooks");
        } else if (!hooked) {
            Wh_Log(L"Standalone Open With: deferred context-menu hooks "
                   L"weren't available");
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: deferred context-menu probe failed");
    }
}

void Wh_ModSettingsChanged() {
    try {
        LoadSettings();
        if (HWND window = g_currentWindow.load(std::memory_order_acquire)) {
            PostMessageW(window, WM_SOW_SETTINGS_CHANGED, 0, 0);
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: settings reload failed");
    }
}

void Wh_ModUninit() {
    StopWorker();
    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        g_pendingRequest.reset();
    }
    {
        std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
        g_openWithMenuStates.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_serverStateMutex);
        g_serverStates.clear();
    }
    g_workerReadyEvent.reset();
    g_requestEvent.reset();
    g_stopEvent.reset();
}
