// ==WindhawkMod==
// @id              win7-open-with-dialog
// @name            Windows 7 Open With Dialog
// @description     Public-API Windows 7-style Open With picker for Windows 10 and 11
// @version         2.6.0
// @author          babamohammed
// @license         MIT
// @include         explorer.exe
// @include         OpenWith.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -lshlwapi -lversion -lbcrypt -lcrypt32 -ladvapi32 -lcomctl32 -luxtheme -luser32 -lgdi32 -luuid -lwinpthread
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Open With Dialog Recreation

This mod recreates the classic Windows 7 **Open with** dialog on Windows 10 and
11, replacing the modern picker with a lightweight native alternative while
keeping the original file and application paths untouched.

The mod has been tested primarily on **Windows 10 21H2 x64**. The public API and
classic context-menu paths are designed to fail safely on unsupported builds.

## Features

- **Windows 7-style dialog**: Recreates the classic layout with recommended and
  other-program groups, Browse, Web search and the Always use checkbox.
- **Unknown-file double click**: Intercepts the `DelegateExecute`/
  `IExecuteCommand` path used when a file type has no valid association.
- **Open with context menu**: Redirects the canonical `openas` command without a
  hard-coded translation table; the localized caption is loaded from Windows.
- **Properties → Change**: Supports association-only requests without opening
  the selected file.
- **Persistent defaults**: Tries `IAssocHandler::MakeDefault`, verifies the
  Windows 10 `UserChoice` value and hash, and keeps a mod-local executable/
  ProgID fallback when Windows rejects the system association.
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
  handles, windows, icons, image lists, fonts, BCrypt objects and temporary
  registry renames, with exception containment around hook boundaries.

## Requirements

- **Windows 10 or Windows 11 x64**
- **Windhawk** with injection enabled for `explorer.exe`, `OpenWith.exe` and
  `rundll32.exe`
- The mod setting **Enable the Windows 7-style picker** must be enabled

## Note

The experimental registry-route virtualization option is disabled by default.
The normal implementation uses COM, Shell APIs and context-menu interception and
does not require permanent spoofing of `DelegateExecute` values.

When the user explicitly selects **Always use** or invokes **Properties →
Change**, the mod is expected to persist an association. All other Open With
selections open the file once without changing the default.

## Known limitations

- **One file at a time**: Multiple-selection Open With requests aren't handled.
- **No default for extensionless files**: They can be opened, but there is no
  extension to associate persistently.
- **Composite extensions**: Windows normally associates `archive.tar.gz` by
  `.gz`; the mod follows the same final-suffix behavior.
- **Store/UWP handlers**: Some AppX handlers don't expose a filesystem
  executable. They rely on their registered ProgID and may fail open if the
  registration is incomplete.
- **UserChoice protections**: Future Windows builds can change or lock the hash
  format. If system persistence fails, the mod-local mapping still prevents a
  repeated picker while the mod is enabled.
- **Experimental virtualization**: Enable it only for diagnostics on builds
  where the normal COM path can't be intercepted.

## Credits

- **ReactOS** — Inspiration
- **aubymori** - Inspiration
- **Image supplied by the user** — document-and-magnifier dialog icon.

The PS-SFTA-derived hash portion is used under the MIT License: permission is
hereby granted, free of charge, to any person obtaining a copy to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies, subject to
inclusion of the copyright and permission notice. The software is provided
“AS IS”, without warranty of any kind.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replaceSystemDialog: true
  $name: Enable the Windows 7-style picker
  $description: When enabled, replace Open With through the stable CLSID_OpenWithMenu context-menu contract, SHOpenWithDialog, the exact openas ShellExecute verb, or the known modern launcher path. When disabled, requests are passed to Windows.
- experimentalRegistryVirtualization: false
  $name: Experimental registry route virtualization
  $description: Disabled by default. Hides selected modern routing values only in Explorer. Enable only for diagnostics; the COM replacement no longer requires it.
- contextMenuTextRedirect: true
  $name: Context-menu text redirect fallback
  $description: Detect the localized Windows Open With item in classic HMENU context menus and redirect only its selected command. The label is loaded from the current Windows MUI resources; translations are not hard-coded.
- showWebLink: true
  $name: Show the Web search link
  $description: Show the Windows 7-style Web link. Only the sanitized file extension is sent to the browser search.
- defaultAssociationBehavior: disabled
  $name: Always use checkbox behavior
  $description: The checkbox now uses IAssocHandler::MakeDefault. This setting controls only the fallback used when Windows rejects the association request.
  $options:
    - disabled: Show an error only
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
    - pt: Português (Brasil)
    - tr: Türkçe
    - ru: Русский
    - zh: 简体中文
    - nl: Nederlands
    - pl: Polski
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <bcrypt.h>
#include <sddl.h>
#include <wincrypt.h>
#include <windowsx.h>
#include <commctrl.h>
#include <exdisp.h>
#include <shldisp.h>
#include <objidl.h>
#include <ocidl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <uxtheme.h>
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
#include <unordered_set>
#include <vector>

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

class BoolFlagGuard {
   public:
    explicit BoolFlagGuard(bool& flag) : flag_(flag), old_(flag) {
        flag_ = true;
    }
    ~BoolFlagGuard() { flag_ = old_; }
    BoolFlagGuard(const BoolFlagGuard&) = delete;
    BoolFlagGuard& operator=(const BoolFlagGuard&) = delete;

   private:
    bool& flag_;
    bool old_;
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

static const GUID kBhidDataObject = {
    0xB8C0BD9F,
    0xED24,
    0x455C,
    {0x83, 0xE6, 0xD5, 0x39, 0x0C, 0x4F, 0xE8, 0xC4}};

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
        L"Se il programma desiderato non \u00E8 presente nell\u2019elenco o nel computer, \u00E8 possibile <A ID=\"WebSearch\">cercare il programma appropriato nel Web</A>.",
        L"Apri con...",
        L"Programmi",
        L"Tutti i file",
        L"OK",
        L"Annulla",
        L"Nessun programma registrato pu\u00F2 aprire questo tipo di file.",
        L"Il programma selezionato non \u00E8 riuscito ad aprire il file.",
    }},
    {0x040A, {  // Spanish
        L"Abrir con",
        L"Elija el programa que desea usar para abrir este archivo:",
        L"Archivo:",
        L"Programas recomendados",
        L"Otros programas",
        L"Escriba una descripci\u00F3n para este tipo de archivo:",
        L"&Usar siempre el programa seleccionado para este tipo de archivo",
        L"&Examinar...",
        L"Si el programa que desea no est\u00E1 en la lista o en el equipo, puede <A ID=\"WebSearch\">buscar el programa adecuado en Internet</A>.",
        L"Abrir con...",
        L"Programas",
        L"Todos los archivos",
        L"Aceptar",
        L"Cancelar",
        L"Ning\u00FAn programa registrado puede abrir este tipo de archivo.",
        L"El programa seleccionado no pudo abrir el archivo.",
    }},
    {0x040C, {  // French
        L"Ouvrir avec",
        L"Choisissez le programme \u00E0 utiliser pour ouvrir ce fichier :",
        L"Fichier :",
        L"Programmes recommand\u00E9s",
        L"Autres programmes",
        L"Tapez une description \u00E0 utiliser pour ce type de fichier :",
        L"&Toujours utiliser ce programme pour ce type de fichier",
        L"&Parcourir...",
        L"Si le programme souhait\u00E9 ne figure pas dans la liste ou sur votre ordinateur, vous pouvez <A ID=\"WebSearch\">rechercher le programme appropri\u00E9 sur le Web</A>.",
        L"Ouvrir avec...",
        L"Programmes",
        L"Tous les fichiers",
        L"OK",
        L"Annuler",
        L"Aucun programme enregistr\u00E9 ne peut ouvrir ce type de fichier.",
        L"Le programme s\u00E9lectionn\u00E9 n\u2019a pas pu ouvrir le fichier.",
    }},
    {0x0416, {  // PortugueseBrazil
        L"Abrir com",
        L"Escolha o programa que deseja usar para abrir este arquivo:",
        L"Arquivo:",
        L"Programas recomendados",
        L"Outros programas",
        L"Digite uma descri\u00E7\u00E3o para este tipo de arquivo:",
        L"&Sempre usar o programa selecionado para este tipo de arquivo",
        L"&Procurar...",
        L"Se o programa desejado n\u00E3o estiver na lista ou no computador, voc\u00EA poder\u00E1 <A ID=\"WebSearch\">procurar o programa apropriado na Web</A>.",
        L"Abrir com...",
        L"Programas",
        L"Todos os arquivos",
        L"OK",
        L"Cancelar",
        L"Nenhum programa registrado pode abrir este tipo de arquivo.",
        L"O programa selecionado n\u00E3o conseguiu abrir o arquivo.",
    }},
    {0x041F, {  // Turkish
        L"Birlikte a\u00E7",
        L"Bu dosyay\u0131 a\u00E7mak i\u00E7in kullanmak istedi\u011Finiz program\u0131 se\u00E7in:",
        L"Dosya:",
        L"\u00D6nerilen Programlar",
        L"Di\u011Fer Programlar",
        L"Bu dosya t\u00FCr\u00FC i\u00E7in kullanmak istedi\u011Finiz a\u00E7\u0131klamay\u0131 yaz\u0131n:",
        L"Bu dosya t\u00FCr\u00FC i\u00E7in &her zaman se\u00E7ili program\u0131 kullan",
        L"&G\u00F6zat...",
        L"\u0130stedi\u011Finiz program listede veya bilgisayar\u0131n\u0131zda yoksa <A ID=\"WebSearch\">Web\u2019de uygun program\u0131 arayabilirsiniz</A>.",
        L"Birlikte a\u00E7...",
        L"Programlar",
        L"T\u00FCm dosyalar",
        L"Tamam",
        L"\u0130ptal",
        L"Bu dosya t\u00FCr\u00FCn\u00FC a\u00E7abilecek kay\u0131tl\u0131 bir program yok.",
        L"Se\u00E7ilen program dosyay\u0131 a\u00E7amad\u0131.",
    }},
    {0x0419, {  // Russian
        L"\u041E\u0442\u043A\u0440\u044B\u0442\u044C \u0441 \u043F\u043E\u043C\u043E\u0449\u044C\u044E",
        L"\u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0443, \u043A\u043E\u0442\u043E\u0440\u0443\u044E \u0441\u043B\u0435\u0434\u0443\u0435\u0442 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u043B\u044F \u043E\u0442\u043A\u0440\u044B\u0442\u0438\u044F \u044D\u0442\u043E\u0433\u043E \u0444\u0430\u0439\u043B\u0430:",
        L"\u0424\u0430\u0439\u043B:",
        L"\u0420\u0435\u043A\u043E\u043C\u0435\u043D\u0434\u0443\u0435\u043C\u044B\u0435 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u044B",
        L"\u0414\u0440\u0443\u0433\u0438\u0435 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u044B",
        L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043E\u043F\u0438\u0441\u0430\u043D\u0438\u0435 \u0434\u043B\u044F \u0444\u0430\u0439\u043B\u043E\u0432 \u044D\u0442\u043E\u0433\u043E \u0442\u0438\u043F\u0430:",
        L"&\u0412\u0441\u0435\u0433\u0434\u0430 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u044D\u0442\u0443 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0443 \u0434\u043B\u044F \u0444\u0430\u0439\u043B\u043E\u0432 \u044D\u0442\u043E\u0433\u043E \u0442\u0438\u043F\u0430",
        L"&\u041E\u0431\u0437\u043E\u0440...",
        L"\u0415\u0441\u043B\u0438 \u043D\u0443\u0436\u043D\u043E\u0439 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u044B \u043D\u0435\u0442 \u0432 \u0441\u043F\u0438\u0441\u043A\u0435 \u0438\u043B\u0438 \u043D\u0430 \u043A\u043E\u043C\u043F\u044C\u044E\u0442\u0435\u0440\u0435, \u043C\u043E\u0436\u043D\u043E <A ID=\"WebSearch\">\u043D\u0430\u0439\u0442\u0438 \u043F\u043E\u0434\u0445\u043E\u0434\u044F\u0449\u0443\u044E \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0443 \u0432 \u0418\u043D\u0442\u0435\u0440\u043D\u0435\u0442\u0435</A>.",
        L"\u041E\u0442\u043A\u0440\u044B\u0442\u044C \u0441 \u043F\u043E\u043C\u043E\u0449\u044C\u044E...",
        L"\u041F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u044B",
        L"\u0412\u0441\u0435 \u0444\u0430\u0439\u043B\u044B",
        L"\u041E\u041A",
        L"\u041E\u0442\u043C\u0435\u043D\u0430",
        L"\u041D\u0435\u0442 \u0437\u0430\u0440\u0435\u0433\u0438\u0441\u0442\u0440\u0438\u0440\u043E\u0432\u0430\u043D\u043D\u044B\u0445 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C, \u0441\u043F\u043E\u0441\u043E\u0431\u043D\u044B\u0445 \u043E\u0442\u043A\u0440\u044B\u0442\u044C \u0444\u0430\u0439\u043B \u044D\u0442\u043E\u0433\u043E \u0442\u0438\u043F\u0430.",
        L"\u0412\u044B\u0431\u0440\u0430\u043D\u043D\u043E\u0439 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0435 \u043D\u0435 \u0443\u0434\u0430\u043B\u043E\u0441\u044C \u043E\u0442\u043A\u0440\u044B\u0442\u044C \u0444\u0430\u0439\u043B.",
    }},
    {0x0804, {  // ChineseSimplified
        L"\u6253\u5F00\u65B9\u5F0F",
        L"\u9009\u62E9\u8981\u7528\u6765\u6253\u5F00\u6B64\u6587\u4EF6\u7684\u7A0B\u5E8F:",
        L"\u6587\u4EF6:",
        L"\u63A8\u8350\u7684\u7A0B\u5E8F",
        L"\u5176\u4ED6\u7A0B\u5E8F",
        L"\u952E\u5165\u7528\u4E8E\u6B64\u6587\u4EF6\u7C7B\u578B\u7684\u8BF4\u660E:",
        L"\u59CB\u7EC8\u4F7F\u7528\u9009\u62E9\u7684\u7A0B\u5E8F\u6253\u5F00\u8FD9\u79CD\u6587\u4EF6",
        L"\u6D4F\u89C8...",
        L"\u5982\u679C\u6240\u9700\u7A0B\u5E8F\u4E0D\u5728\u5217\u8868\u4E2D\u6216\u8BA1\u7B97\u673A\u4E0A\uFF0C\u53EF\u4EE5<A ID=\"WebSearch\">\u5728 Web \u4E0A\u67E5\u627E\u9002\u5F53\u7684\u7A0B\u5E8F</A>\u3002",
        L"\u6253\u5F00\u65B9\u5F0F...",
        L"\u7A0B\u5E8F",
        L"\u6240\u6709\u6587\u4EF6",
        L"\u786E\u5B9A",
        L"\u53D6\u6D88",
        L"\u6CA1\u6709\u5DF2\u6CE8\u518C\u7684\u7A0B\u5E8F\u53EF\u4EE5\u6253\u5F00\u8FD9\u79CD\u6587\u4EF6\u3002",
        L"\u6240\u9009\u7A0B\u5E8F\u65E0\u6CD5\u6253\u5F00\u6B64\u6587\u4EF6\u3002",
    }},
    {0x0413, {  // Dutch
        L"Openen met",
        L"Kies het programma waarmee u dit bestand wilt openen:",
        L"Bestand:",
        L"Aanbevolen programma's",
        L"Andere programma's",
        L"Typ een beschrijving voor dit bestandstype:",
        L"Dit type bestand &altijd met het geselecteerde programma openen",
        L"&Bladeren...",
        L"Als het gewenste programma niet in de lijst of op uw computer staat, kunt u <A ID=\"WebSearch\">op internet naar het juiste programma zoeken</A>.",
        L"Openen met...",
        L"Programma's",
        L"Alle bestanden",
        L"OK",
        L"Annuleren",
        L"Er is geen geregistreerd programma dat dit bestandstype kan openen.",
        L"Het geselecteerde programma kan het bestand niet openen.",
    }},
    {0x0415, {  // Polish
        L"Otwieranie za pomoc\u0105",
        L"Wybierz program, kt\u00F3rego chcesz u\u017Cy\u0107 do otwarcia tego pliku:",
        L"Plik:",
        L"Zalecane programy",
        L"Inne programy",
        L"Wpisz opis u\u017Cywany dla tego typu pliku:",
        L"&Zawsze u\u017Cywaj tego programu do plik\u00F3w tego typu",
        L"&Przegl\u0105daj...",
        L"Je\u015Bli odpowiedniego programu nie ma na li\u015Bcie ani na komputerze, mo\u017Cesz <A ID=\"WebSearch\">wyszuka\u0107 go w Internecie</A>.",
        L"Otwieranie za pomoc\u0105...",
        L"Programy",
        L"Wszystkie pliki",
        L"OK",
        L"Anuluj",
        L"\u017Baden zarejestrowany program nie mo\u017Ce otworzy\u0107 tego typu pliku.",
        L"Wybrany program nie m\u00F3g\u0142 otworzy\u0107 pliku.",
    }},
};

static std::atomic<const LocalePack*> g_CurrentLocalePack{&g_Locales[0]};
#define LOC(id) (g_CurrentLocalePack.load(std::memory_order_acquire)->strings[id])

enum class DefaultBehavior {
    Disabled,
    OpenSettings,
};

static std::atomic<bool> g_replaceSystemDialog{true};
static std::atomic<bool> g_showWebLink{true};
static std::atomic<bool> g_contextMenuTextRedirect{true};
static std::atomic<bool> g_virtualizeLegacyOpenWithRegistry{false};
static std::atomic<int> g_languageSetting{0};  // 0 = automatic
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
        if (const LocalePack* locale =
                TryFindLocalePack(LANGIDFROMLCID(lcid))) {
            return locale;
        }
    }

    // LocaleNameToLCID can reject newer BCP-47 aliases such as zh-Hans on
    // older Windows 10 SDK surfaces. Keep a small language-tag fallback.
    if (!_wcsnicmp(localeName, L"it", 2)) return FindLocalePack(0x0410);
    if (!_wcsnicmp(localeName, L"es", 2)) return FindLocalePack(0x040A);
    if (!_wcsnicmp(localeName, L"fr", 2)) return FindLocalePack(0x040C);
    if (!_wcsnicmp(localeName, L"pt", 2)) return FindLocalePack(0x0416);
    if (!_wcsnicmp(localeName, L"tr", 2)) return FindLocalePack(0x041F);
    if (!_wcsnicmp(localeName, L"ru", 2)) return FindLocalePack(0x0419);
    if (!_wcsnicmp(localeName, L"zh", 2)) return FindLocalePack(0x0804);
    if (!_wcsnicmp(localeName, L"nl", 2)) return FindLocalePack(0x0413);
    if (!_wcsnicmp(localeName, L"pl", 2)) return FindLocalePack(0x0415);
    if (!_wcsnicmp(localeName, L"en", 2)) return FindLocalePack(0x0409);
    return nullptr;
}

static const LocalePack* DetectAutomaticLocale() {
    // GetUserDefaultUILanguage can report en-US in some short-lived Shell COM
    // hosts even when the user's preferred display-language list starts with
    // Italian (or another language). Follow the ordered user preference list
    // first, just like Windows resource loading does.
    ULONG languageCount = 0;
    ULONG bufferChars = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &languageCount, nullptr,
                                    &bufferChars) &&
        bufferChars > 1 && bufferChars < 32768) {
        try {
            std::vector<wchar_t> languages(bufferChars);
            if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &languageCount,
                                            languages.data(), &bufferChars)) {
                for (PCWSTR cursor = languages.data(); *cursor;
                     cursor += wcslen(cursor) + 1) {
                    if (const LocalePack* locale = LocalePackFromName(cursor)) {
                        return locale;
                    }
                }
            }
        } catch (...) {
        }
    }

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
    if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        if (const LocalePack* locale = LocalePackFromName(localeName)) {
            return locale;
        }
    }
    if (const LocalePack* locale =
            TryFindLocalePack(GetUserDefaultUILanguage())) {
        return locale;
    }
    if (GetSystemDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        if (const LocalePack* locale = LocalePackFromName(localeName)) {
            return locale;
        }
    }
    if (const LocalePack* locale =
            TryFindLocalePack(GetSystemDefaultUILanguage())) {
        return locale;
    }
    return &g_Locales[0];
}

static void DetermineLocale() {
    const int requested = g_languageSetting.load(std::memory_order_acquire);
    const LocalePack* selected = nullptr;
    switch (requested) {
        case 1: selected = FindLocalePack(0x0409); break;
        case 2: selected = FindLocalePack(0x0410); break;
        case 3: selected = FindLocalePack(0x040A); break;
        case 4: selected = FindLocalePack(0x040C); break;
        case 5: selected = FindLocalePack(0x0416); break;
        case 6: selected = FindLocalePack(0x041F); break;
        case 7: selected = FindLocalePack(0x0419); break;
        case 8: selected = FindLocalePack(0x0804); break;
        case 9: selected = FindLocalePack(0x0413); break;
        case 10: selected = FindLocalePack(0x0415); break;
        default: selected = DetectAutomaticLocale(); break;
    }
    if (!selected) selected = &g_Locales[0];
    g_CurrentLocalePack.store(selected, std::memory_order_release);
    Wh_Log(L"Standalone Open With locale: setting=%d selected=%04X "
           L"userUI=%04X systemUI=%04X",
           requested, selected->langId, GetUserDefaultUILanguage(),
           GetSystemDefaultUILanguage());
}

static void LoadSettings() {
    const bool replace = Wh_GetIntSetting(L"replaceSystemDialog") != 0;
    const bool web = Wh_GetIntSetting(L"showWebLink") != 0;
    const bool contextMenuTextRedirect =
        Wh_GetIntSetting(L"contextMenuTextRedirect") != 0;
    const bool virtualizeLegacyOpenWithRegistry =
        Wh_GetIntSetting(L"experimentalRegistryVirtualization") != 0;
    WindhawkUtils::StringSetting language =
        WindhawkUtils::StringSetting::make(L"language");
    WindhawkUtils::StringSetting defaultBehavior =
        WindhawkUtils::StringSetting::make(L"defaultAssociationBehavior");

    int languageIndex = 0;
    PCWSTR languageValue = language.get();
    if (languageValue) {
        if (!_wcsicmp(languageValue, L"en")) languageIndex = 1;
        else if (!_wcsicmp(languageValue, L"it")) languageIndex = 2;
        else if (!_wcsicmp(languageValue, L"es")) languageIndex = 3;
        else if (!_wcsicmp(languageValue, L"fr")) languageIndex = 4;
        else if (!_wcsicmp(languageValue, L"pt") ||
                 !_wcsicmp(languageValue, L"pt-BR")) languageIndex = 5;
        else if (!_wcsicmp(languageValue, L"tr")) languageIndex = 6;
        else if (!_wcsicmp(languageValue, L"ru")) languageIndex = 7;
        else if (!_wcsicmp(languageValue, L"zh") ||
                 !_wcsicmp(languageValue, L"zh-CN")) languageIndex = 8;
        else if (!_wcsicmp(languageValue, L"nl")) languageIndex = 9;
        else if (!_wcsicmp(languageValue, L"pl")) languageIndex = 10;
    }
    const DefaultBehavior behavior =
        defaultBehavior.get() &&
        !_wcsicmp(defaultBehavior.get(), L"openSettings")
            ? DefaultBehavior::OpenSettings
            : DefaultBehavior::Disabled;

    g_languageSetting.store(languageIndex, std::memory_order_release);
    g_defaultBehavior.store(behavior, std::memory_order_release);
    g_showWebLink.store(web, std::memory_order_release);
    g_contextMenuTextRedirect.store(contextMenuTextRedirect,
                                    std::memory_order_release);
    g_virtualizeLegacyOpenWithRegistry.store(
        virtualizeLegacyOpenWithRegistry, std::memory_order_release);
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
    HWND window = nullptr;
    HWND list = nullptr;
    HWND description = nullptr;
    HWND alwaysUse = nullptr;
    bool finished = false;
    bool accepted = false;
    bool makeDefaultRequested = false;
    std::wstring associationDescription;
    bool openDefaultSettings = false;
    bool listUsesGroups = false;
    bool hasOtherGroup = false;
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
        if (!duplicate) {
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
    } else {
        const bool enableAssociation =
            handlerCanBeDefault && hasAssociableExtension;
        EnableWindow(state.alwaysUse, enableAssociation);
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
    SetWindowTheme(state.list, L"Explorer", nullptr);
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
    if (FAILED(dialog->Show(state.window))) return;
    ComPtr<IShellItem> item;
    if (FAILED(dialog->GetResult(item.Put())) || !item) return;
    PWSTR raw = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) || !raw) return;
    const std::wstring executable = TakeTaskString(raw);
    if (executable.empty()) return;
    if (IsOpenWithExecutable(executable)) {
        MessageBoxW(state.window, LOC(STR_OPEN_FAILED), LOC(STR_TITLE),
                    MB_OK | MB_ICONWARNING);
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
    const std::wstring extension = ExtensionOf(state.request.path);
    const HRESULT registrationHr = EnsureUserApplicationRegistration(
        executable, extension, &entry.progId);
    Wh_Log(L"Standalone Open With: browsed app registration exe=%s "
           L"progId=%s hr=0x%08X", executable.c_str(), entry.progId.c_str(),
           static_cast<unsigned int>(registrationHr));
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

static void BuildPickerControls(PickerState& state) {
    const UINT dpi = WindowDpi(state.window);
    NONCLIENTMETRICSW metrics{sizeof(metrics)};
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
        state.font.Reset(CreateFontIndirectW(&metrics.lfMessageFont));
    HFONT font = state.font ? state.font.Get() : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

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
    state.description = Child(state.window, WS_EX_CLIENTEDGE, WC_EDITW, L"",
          WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
          14, 320, 520, 24, IDC_SOW_DESCRIPTION, dpi, font);
    state.alwaysUse = Child(state.window, 0, WC_BUTTONW, LOC(STR_ALWAYS_USE),
          WS_TABSTOP | BS_AUTOCHECKBOX, 14, 354, 405, 22,
          IDC_SOW_ALWAYS_USE, dpi, font);
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
        case WM_COMMAND:
            if (!state) break;
            if (LOWORD(wParam) == IDOK) {
                const int index = SelectedIndex(*state);
                if (index >= 0 && static_cast<size_t>(index) < state->handlers.size()) {
                    state->chosenIndex = index;
                    state->accepted = true;
                    state->makeDefaultRequested =
                        state->request.setDefaultOnly ||
                        Button_GetCheck(state->alwaysUse) == BST_CHECKED;
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
            break;
        }
        case WM_SOW_ACTIVATE:
            ActivatePickerWindow(window);
            return 0;
        case WM_SOW_SETTINGS_CHANGED:
            if (state) ApplyLocalizedText(*state);
            return 0;
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

static std::wstring AssociationStorageName(const std::wstring& extension) {
    std::wstring name = L"userAssociation.";
    try {
        for (wchar_t value : extension) {
            if ((value >= L'a' && value <= L'z') ||
                (value >= L'A' && value <= L'Z') ||
                (value >= L'0' && value <= L'9')) {
                name.push_back(static_cast<wchar_t>(towlower(value)));
            } else {
                wchar_t encoded[8] = {};
                swprintf_s(encoded, L"_%04x", static_cast<unsigned>(value));
                name.append(encoded);
            }
        }
    } catch (...) {
        return {};
    }
    return name;
}

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

struct StoredModAssociation {
    std::wstring executable;
    std::wstring progId;
};

static bool StoreModAssociation(const std::wstring& extension,
                                const std::wstring& executable,
                                const std::wstring& progId) {
    if (extension.size() <= 1 || extension[0] != L'.' ||
        !IsSupportedFile(executable)) {
        return false;
    }
    const std::wstring name = AssociationStorageName(extension);
    if (name.empty()) return false;
    std::wstring progIdName = name + L".progId";
    const bool executableStored =
        Wh_SetStringValue(name.c_str(), executable.c_str());
    const bool progIdStored = progId.empty() ||
        Wh_SetStringValue(progIdName.c_str(), progId.c_str());
    const bool stored = executableStored && progIdStored;
    Wh_Log(L"Standalone Open With: local association store extension=%s "
           L"exe=%s progId=%s stored=%d", extension.c_str(),
           executable.c_str(), progId.c_str(), stored);
    return stored;
}

static StoredModAssociation LoadModAssociation(const std::wstring& path) {
    StoredModAssociation result;
    const std::wstring extension = ExtensionOf(path);
    if (extension.size() <= 1) return result;
    const std::wstring name = AssociationStorageName(extension);
    if (name.empty()) return result;
    try {
        std::vector<wchar_t> buffer(32768);
        const size_t chars = Wh_GetStringValue(name.c_str(), buffer.data(),
                                               buffer.size());
        if (!chars || chars >= buffer.size()) return result;
        result.executable.assign(buffer.data(), chars);
        if (!IsSupportedFile(result.executable)) {
            Wh_SetStringValue(name.c_str(), L"");
            result.executable.clear();
            return result;
        }

        const std::wstring progIdName = name + L".progId";
        std::fill(buffer.begin(), buffer.end(), L'\0');
        const size_t progIdChars = Wh_GetStringValue(
            progIdName.c_str(), buffer.data(), buffer.size());
        if (progIdChars && progIdChars < buffer.size())
            result.progId.assign(buffer.data(), progIdChars);
        return result;
    } catch (...) {
        return {};
    }
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

static HRESULT InvokeStoredAssociation(const StoredModAssociation& association,
                                       const std::wstring& path) {
    if (!association.progId.empty()) {
        const std::wstring commandTemplate =
            CommandTemplateForProgId(association.progId);
        if (!commandTemplate.empty()) {
            const HRESULT commandHr =
                InvokeCommandTemplate(commandTemplate, path);
            if (SUCCEEDED(commandHr)) return commandHr;
            Wh_Log(L"Standalone Open With: ProgID command failed progId=%s "
                   L"hr=0x%08X", association.progId.c_str(),
                   static_cast<unsigned int>(commandHr));
        }
    }
    return InvokeExecutableWithFile(association.executable, path);
}

static bool TryInvokeStoredAssociation(const std::wstring& path) {
    const StoredModAssociation association = LoadModAssociation(path);
    if (association.executable.empty()) return false;
    const HRESULT hr = InvokeStoredAssociation(association, path);
    Wh_Log(L"Standalone Open With: local association invoke extension=%s "
           L"progId=%s exe=%s hr=0x%08X", ExtensionOf(path).c_str(),
           association.progId.c_str(), association.executable.c_str(),
           static_cast<unsigned int>(hr));
    return SUCCEEDED(hr);
}

static HRESULT InvokeRegistered(const PickerState& state, StandaloneAssocHandler* handler) {
    if (!handler) return E_INVALIDARG;
    ComPtr<IShellItem> item;
    HRESULT hr = SHCreateItemFromParsingName(state.request.path.c_str(), nullptr,
                                             IID_PPV_ARGS(item.Put()));
    if (FAILED(hr) || !item) return hr;
    ComPtr<IDataObject> data;
    hr = item->BindToHandler(nullptr, kBhidDataObject, IID_IDataObject,
                             reinterpret_cast<void**>(data.Put()));
    return SUCCEEDED(hr) && data ? handler->Invoke(data.Get()) : hr;
}

static HRESULT InvokeSelectedHandler(const PickerState& state,
                                     HandlerEntry& selected) {
    try {
        StoredModAssociation association;
        association.progId = selected.progId;
        association.executable = ExecutableForHandler(selected);
        if (IsOpenWithExecutable(association.executable) ||
            IsOpenWithHandlerName(selected.internalName,
                                  association.progId)) {
            return HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION);
        }

        // Never pass a user-selected Win32 program back through generic Shell
        // association resolution: on an unknown type that can re-enter
        // OpenWith. Resolve and launch the command template directly instead.
        const HRESULT directHr =
            InvokeStoredAssociation(association, state.request.path);
        Wh_Log(L"Standalone Open With: selected handler direct result "
               L"progId=%s exe=%s hr=0x%08X",
               association.progId.c_str(), association.executable.c_str(),
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

// UserChoice hash algorithm translated to C++ from PS-SFTA's MIT-licensed
// implementation (Copyright 2022 Danysys), based on the reverse-engineered
// Windows 10 1703+ association format. The routine below is purpose-written
// for this mod and doesn't depend on Mozilla code.
class BCryptAlgorithmOwner {
   public:
    ~BCryptAlgorithmOwner() {
        if (value_) BCryptCloseAlgorithmProvider(value_, 0);
    }
    BCRYPT_ALG_HANDLE* Put() { return &value_; }
    BCRYPT_ALG_HANDLE Get() const { return value_; }
   private:
    BCRYPT_ALG_HANDLE value_ = nullptr;
};

class BCryptHashOwner {
   public:
    ~BCryptHashOwner() {
        if (value_) BCryptDestroyHash(value_);
    }
    BCRYPT_HASH_HANDLE* Put() { return &value_; }
    BCRYPT_HASH_HANDLE Get() const { return value_; }
   private:
    BCRYPT_HASH_HANDLE value_ = nullptr;
};

struct LocalFreeDeleter {
    void operator()(wchar_t* value) const noexcept {
        if (value) LocalFree(value);
    }
};
using LocalString = std::unique_ptr<wchar_t, LocalFreeDeleter>;

static std::wstring CurrentUserSidString() {
    HANDLE rawToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &rawToken))
        return {};
    WinHandle token(rawToken);

    DWORD bytes = 0;
    GetTokenInformation(token.get(), TokenUser, nullptr, 0, &bytes);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || !bytes) return {};
    try {
        std::vector<BYTE> buffer(bytes);
        if (!GetTokenInformation(token.get(), TokenUser, buffer.data(), bytes,
                                 &bytes)) {
            return {};
        }
        PWSTR rawSid = nullptr;
        if (!ConvertSidToStringSidW(
                reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid,
                &rawSid) || !rawSid) {
            return {};
        }
        LocalString sid(rawSid);
        return sid.get();
    } catch (...) {
        return {};
    }
}

static bool Md5Bytes(const BYTE* input, ULONG inputBytes,
                     DWORD output[4]) {
    BCryptAlgorithmOwner algorithm;
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
            algorithm.Put(), BCRYPT_MD5_ALGORITHM, nullptr, 0))) {
        return false;
    }
    BCryptHashOwner hash;
    if (!BCRYPT_SUCCESS(BCryptCreateHash(
            algorithm.Get(), hash.Put(), nullptr, 0, nullptr, 0, 0))) {
        return false;
    }
    if (!BCRYPT_SUCCESS(BCryptHashData(
            hash.Get(), const_cast<PUCHAR>(input), inputBytes, 0))) {
        return false;
    }
    return BCRYPT_SUCCESS(BCryptFinishHash(
        hash.Get(), reinterpret_cast<PUCHAR>(output), 16, 0));
}

static DWORD WordSwap(DWORD value) {
    return (value >> 16) | (value << 16);
}

static std::wstring GenerateWindows10UserChoiceHash(
    const std::wstring& extension, const std::wstring& sid,
    const std::wstring& progId, SYSTEMTIME timestamp) {
    timestamp.wSecond = 0;
    timestamp.wMilliseconds = 0;
    FILETIME fileTime{};
    if (!SystemTimeToFileTime(&timestamp, &fileTime)) return {};

    static constexpr PCWSTR kExperience =
        L"User Choice set via Windows User Experience "
        L"{D18B6DD5-6124-4341-9318-804003BAFA0B}";
    std::wstring input;
    try {
        wchar_t timestampText[17] = {};
        if (swprintf_s(timestampText, L"%08lx%08lx",
                       fileTime.dwHighDateTime,
                       fileTime.dwLowDateTime) < 0) {
            return {};
        }
        input.reserve(extension.size() + sid.size() + progId.size() +
                      16 + wcslen(kExperience));
        input.append(extension);
        input.append(sid);
        input.append(progId);
        input.append(timestampText);
        input.append(kExperience);
        CharLowerBuffW(input.data(), static_cast<DWORD>(input.size()));
    } catch (...) {
        return {};
    }

    const BYTE* inputBytes = reinterpret_cast<const BYTE*>(input.c_str());
    const int inputByteCount =
        static_cast<int>((input.size() + 1) * sizeof(wchar_t));
    constexpr int kDwordsPerBlock = 2;
    const int blockCount = inputByteCount /
                           (sizeof(DWORD) * kDwordsPerBlock);
    if (!blockCount) return {};

    DWORD md5[4] = {};
    if (!Md5Bytes(inputBytes, inputByteCount, md5)) return {};

    const DWORD c0[2][5] = {
        {md5[0] | 1, 0xCF98B111u, 0x87085B9Fu, 0x12CEB96Du,
         0x257E1D83u},
        {md5[1] | 1, 0xA27416F5u, 0xD38396FFu, 0x7C932B89u,
         0xBFA49F69u},
    };
    const DWORD c1[2][5] = {
        {md5[0] | 1, 0xEF0569FBu, 0x689B6B9Fu, 0x79F8A395u,
         0xC3EFEA97u},
        {md5[1] | 1, 0xC31713DBu, 0xDDCD1F0Fu, 0x59C3AF2Du,
         0x35BD1EC9u},
    };

    DWORD h0 = 0, h1 = 0, h0Acc = 0, h1Acc = 0;
    for (int i = 0; i < blockCount; ++i) {
        for (int j = 0; j < kDwordsPerBlock; ++j) {
            DWORD value = 0;
            memcpy(&value,
                   inputBytes +
                       (i * kDwordsPerBlock + j) * sizeof(DWORD),
                   sizeof(value));
            h0 += value;
            h0 *= c0[j][0];
            h0 = WordSwap(h0) * c0[j][1];
            h0 = WordSwap(h0) * c0[j][2];
            h0 = WordSwap(h0) * c0[j][3];
            h0 = WordSwap(h0) * c0[j][4];
            h0Acc += h0;

            h1 += value;
            h1 = WordSwap(h1) * c1[j][1] + h1 * c1[j][0];
            h1 = (h1 >> 16) * c1[j][2] + h1 * c1[j][3];
            h1 = WordSwap(h1) * c1[j][4] + h1;
            h1Acc += h1;
        }
    }

    DWORD result[2] = {h0 ^ h1, h0Acc ^ h1Acc};
    DWORD chars = 0;
    if (!CryptBinaryToStringW(
            reinterpret_cast<const BYTE*>(result), sizeof(result),
            CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &chars) ||
        !chars) {
        return {};
    }
    try {
        std::vector<wchar_t> encoded(chars);
        if (!CryptBinaryToStringW(
                reinterpret_cast<const BYTE*>(result), sizeof(result),
                CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                encoded.data(), &chars)) {
            return {};
        }
        return encoded.data();
    } catch (...) {
        return {};
    }
}

using RegRenameKey_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPCWSTR);

class RegistryRenameGuard {
   public:
    RegistryRenameGuard(RegRenameKey_t function, HKEY key,
                        std::wstring originalName)
        : function_(function), key_(key), originalName_(std::move(originalName)) {}
    ~RegistryRenameGuard() { Restore(); }
    void Activate() { active_ = true; }
    bool Restore() {
        if (!active_) return true;
        active_ = false;
        return function_ &&
               function_(key_, nullptr, originalName_.c_str()) ==
                   ERROR_SUCCESS;
    }
   private:
    RegRenameKey_t function_ = nullptr;
    HKEY key_ = nullptr;
    std::wstring originalName_;
    bool active_ = false;
};

static HRESULT WriteWindows10UserChoice(const std::wstring& extension,
                                        const std::wstring& progId) {
    if (extension.size() <= 1 || extension[0] != L'.' || progId.empty())
        return E_INVALIDARG;
    const std::wstring sid = CurrentUserSidString();
    if (sid.empty()) return E_FAIL;

    SYSTEMTIME timestamp{};
    GetSystemTime(&timestamp);
    const std::wstring hash = GenerateWindows10UserChoiceHash(
        extension, sid, progId, timestamp);
    if (hash.empty()) return E_FAIL;

    std::wstring associationPath;
    try {
        associationPath =
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
            L"FileExts\\" + extension;
    } catch (...) {
        return E_OUTOFMEMORY;
    }

    RegKeyOwner association;
    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER, associationPath.c_str(), 0, nullptr, 0,
        KEY_READ | KEY_WRITE, nullptr, association.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    HMODULE advapi = GetModuleHandleW(L"advapi32.dll");
    auto renameKey = advapi ? reinterpret_cast<RegRenameKey_t>(
                                  GetProcAddress(advapi, "RegRenameKey"))
                            : nullptr;
    RegistryRenameGuard renameGuard(renameKey, association.Get(), extension);
    if (renameKey) {
        GUID guid{};
        wchar_t temporaryName[64] = {};
        if (SUCCEEDED(CoCreateGuid(&guid)) &&
            StringFromGUID2(guid, temporaryName,
                            ARRAYSIZE(temporaryName)) &&
            renameKey(association.Get(), nullptr, temporaryName) ==
                ERROR_SUCCESS) {
            renameGuard.Activate();
        }
    }

    status = RegDeleteTreeW(association.Get(), L"UserChoice");
    if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND)
        return HRESULT_FROM_WIN32(status);

    RegKeyOwner userChoice;
    status = RegCreateKeyExW(
        association.Get(), L"UserChoice", 0, nullptr, 0,
        KEY_READ | KEY_WRITE, nullptr, userChoice.Put(), nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

    status = RegSetValueExW(
        userChoice.Get(), L"ProgId", 0, REG_SZ,
        reinterpret_cast<const BYTE*>(progId.c_str()),
        static_cast<DWORD>((progId.size() + 1) * sizeof(wchar_t)));
    if (status == ERROR_SUCCESS) {
        status = RegSetValueExW(
            userChoice.Get(), L"Hash", 0, REG_SZ,
            reinterpret_cast<const BYTE*>(hash.c_str()),
            static_cast<DWORD>((hash.size() + 1) * sizeof(wchar_t)));
    }
    userChoice.Reset();
    const bool restored = renameGuard.Restore();
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);
    if (!restored) return E_FAIL;

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, nullptr, nullptr);
    Wh_Log(L"Standalone Open With: wrote UserChoice extension=%s "
           L"progId=%s hash=%s", extension.c_str(), progId.c_str(),
           hash.c_str());
    return S_OK;
}

static bool UserChoiceMatches(const std::wstring& extension,
                              const std::wstring& progId) {
    if (extension.empty() || progId.empty()) return false;
    wchar_t path[1024] = {};
    if (swprintf_s(
            path,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
            L"FileExts\\%s\\UserChoice",
            extension.c_str()) < 0) {
        return false;
    }

    RegKeyOwner key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, path, 0, KEY_READ,
                      key.Put()) != ERROR_SUCCESS) {
        return false;
    }
    wchar_t currentProgId[1024] = {};
    DWORD bytes = sizeof(currentProgId);
    if (RegGetValueW(key.Get(), nullptr, L"ProgId", RRF_RT_REG_SZ,
                     nullptr, currentProgId, &bytes) != ERROR_SUCCESS ||
        _wcsicmp(currentProgId, progId.c_str())) {
        return false;
    }
    wchar_t storedHash[256] = {};
    bytes = sizeof(storedHash);
    if (RegGetValueW(key.Get(), nullptr, L"Hash", RRF_RT_REG_SZ,
                     nullptr, storedHash, &bytes) != ERROR_SUCCESS) {
        return false;
    }
    FILETIME writeTime{};
    if (RegQueryInfoKeyW(key.Get(), nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                         &writeTime) != ERROR_SUCCESS) {
        return false;
    }
    SYSTEMTIME timestamp{};
    if (!FileTimeToSystemTime(&writeTime, &timestamp)) return false;
    const std::wstring sid = CurrentUserSidString();
    const std::wstring expected = GenerateWindows10UserChoiceHash(
        extension, sid, progId, timestamp);
    return !expected.empty() && !_wcsicmp(storedHash, expected.c_str());
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

    const std::wstring executable = ExecutableForHandler(selected);
    const bool localStored = StoreModAssociation(
        extension, executable, selected.progId);

    if (UserChoiceMatches(extension, selected.progId)) return S_OK;
    const HRESULT userChoiceHr =
        WriteWindows10UserChoice(extension, selected.progId);
    const bool verified = SUCCEEDED(userChoiceHr) &&
                          UserChoiceMatches(extension, selected.progId);
    Wh_Log(L"Standalone Open With: UserChoice fallback extension=%s "
           L"progId=%s shellHr=0x%08X userChoiceHr=0x%08X verified=%d",
           extension.c_str(), selected.progId.c_str(),
           static_cast<unsigned int>(shellHr),
           static_cast<unsigned int>(userChoiceHr), verified);
    if (!verified && localStored) {
        // Don't leave an invalid UserChoice which makes Explorer repeatedly
        // invoke the system picker. With no valid UserChoice, the unknown-file
        // route reaches our COM adapter, which honors the stored executable.
        wchar_t associationPath[1024] = {};
        if (swprintf_s(
                associationPath,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                L"FileExts\\%s",
                extension.c_str()) >= 0) {
            RegKeyOwner association;
            if (RegOpenKeyExW(HKEY_CURRENT_USER, associationPath, 0,
                              KEY_READ | KEY_WRITE,
                              association.Put()) == ERROR_SUCCESS) {
                RegDeleteTreeW(association.Get(), L"UserChoice");
            }
        }
        SHChangeNotify(SHCNE_ASSOCCHANGED,
                       SHCNF_IDLIST | SHCNF_FLUSH, nullptr, nullptr);
    }
    return (verified || localStored)
               ? S_OK
               : (FAILED(userChoiceHr)
                      ? userChoiceHr
                      : HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}

static WinHandle g_stopEvent;
static WinHandle g_requestEvent;
static WinHandle g_workerReadyEvent;
static std::atomic<bool> g_shuttingDown{false};
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
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1);
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
    PickerCompletionSignal completion(request.completionEvent);
    if (g_shuttingDown.load(std::memory_order_acquire) || !IsSupportedFile(request.path)) return;
    PickerState state;
    state.request = std::move(request);
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
        MessageBoxW(nullptr, LOC(STR_OPEN_FAILED), LOC(STR_TITLE),
                    MB_OK | MB_ICONERROR);
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
        if (PostMessageW(current, WM_SOW_ACTIVATE, 0, 0)) return true;
    }

    std::lock_guard<std::mutex> lock(g_requestMutex);
    if (g_pendingRequest) {
        // Coalesce while the worker is between wake-up and window creation.
        // Explorer returns immediately and the first request remains authoritative.
        return true;
    }
    if (HWND current = g_currentWindow.load(std::memory_order_acquire)) {
        if (PostMessageW(current, WM_SOW_ACTIVATE, 0, 0)) return true;
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
// Last-resort classic HMENU redirect. This deliberately works at the visible
// menu boundary, but it doesn't contain a translation table: the localized
// Open With caption is loaded from Windows' current MUI resource.
// -----------------------------------------------------------------------------

// The Open-With context-menu COM handler and TrackPopupMenu run on the same
// Explorer UI thread. The COM hook fills this before the visible menu is shown.
static thread_local std::wstring g_lastOpenWithContextPath;

static std::wstring NormalizeMenuCaption(PCWSTR text) {
    std::wstring result;
    if (!text) return result;
    try {
        for (PCWSTR cursor = text; *cursor && *cursor != L'\t'; ++cursor) {
            wchar_t value = *cursor;
            if (value == L'&' || value == L'.' || value == L'\u2026' ||
                iswspace(value)) {
                continue;
            }
            result.push_back(static_cast<wchar_t>(towlower(value)));
        }
    } catch (...) {
        result.clear();
    }
    return result;
}

static void AddLocalizedCaption(std::vector<std::wstring>& captions,
                                PCWSTR value) {
    std::wstring normalized = NormalizeMenuCaption(value);
    if (normalized.empty()) return;
    if (std::find(captions.begin(), captions.end(), normalized) ==
        captions.end()) {
        captions.push_back(std::move(normalized));
    }
}

static std::vector<std::wstring> LoadLocalizedOpenWithCaptions() {
    std::vector<std::wstring> captions;
    wchar_t registryValue[512] = {};
    DWORD type = 0;
    DWORD bytes = sizeof(registryValue);
    if (RegGetValueW(HKEY_CLASSES_ROOT, L"Unknown\\shell\\openas", nullptr,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, &type,
                     registryValue, &bytes) == ERROR_SUCCESS) {
        wchar_t resolved[512] = {};
        if (registryValue[0] == L'@' &&
            SUCCEEDED(SHLoadIndirectString(registryValue, resolved,
                                           ARRAYSIZE(resolved), nullptr))) {
            AddLocalizedCaption(captions, resolved);
        } else {
            AddLocalizedCaption(captions, registryValue);
        }
    }

    // This is the resource referenced by the stock Unknown\\shell\\openas key.
    // SHLoadIndirectString selects the installed Windows display language.
    wchar_t resolved[512] = {};
    if (SUCCEEDED(SHLoadIndirectString(
            L"@%SystemRoot%\\System32\\shell32.dll,-5376", resolved,
            ARRAYSIZE(resolved), nullptr))) {
        AddLocalizedCaption(captions, resolved);
    }

    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    wchar_t direct[512] = {};
    if (shell32 && LoadStringW(shell32, 5376, direct, ARRAYSIZE(direct)))
        AddLocalizedCaption(captions, direct);

    for (const std::wstring& caption : captions)
        Wh_Log(L"Standalone Open With: localized HMENU key='%s'",
               caption.c_str());
    return captions;
}

static bool CaptionMatchesOpenWith(
    PCWSTR caption, const std::vector<std::wstring>& localizedCaptions) {
    const std::wstring normalized = NormalizeMenuCaption(caption);
    if (normalized.empty()) return false;
    return std::find(localizedCaptions.begin(), localizedCaptions.end(),
                     normalized) != localizedCaptions.end();
}

struct OpenWithMenuCommandScan {
    bool foundLocalizedContainer = false;
    std::unordered_set<UINT> commandIds;
};

static void AddLastActionableSubmenuCommand(
    HMENU submenu, OpenWithMenuCommandScan& scan) {
    if (!submenu) return;
    const int count = GetMenuItemCount(submenu);
    for (int position = count - 1; position >= 0; --position) {
        MENUITEMINFOW item{sizeof(item)};
        item.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STATE | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(submenu, position, TRUE, &item)) continue;
        if ((item.fType & MFT_SEPARATOR) ||
            (item.fState & (MFS_DISABLED | MFS_GRAYED))) {
            continue;
        }
        if (item.hSubMenu) {
            AddLastActionableSubmenuCommand(item.hSubMenu, scan);
            if (!scan.commandIds.empty()) return;
            continue;
        }
        if (item.wID != static_cast<UINT>(-1) && item.wID != 0)
            scan.commandIds.insert(item.wID);
        return;
    }
}

static void ScanMenuForOpenWith(
    HMENU menu, const std::vector<std::wstring>& localizedCaptions,
    OpenWithMenuCommandScan& scan) {
    if (!menu) return;
    const int count = GetMenuItemCount(menu);
    for (int position = 0; position < count; ++position) {
        MENUITEMINFOW query{sizeof(query)};
        query.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STATE | MIIM_SUBMENU |
                      MIIM_STRING;
        if (!GetMenuItemInfoW(menu, position, TRUE, &query)) continue;

        std::wstring text;
        if (query.cch) {
            try {
                text.resize(query.cch + 1);
                query.dwTypeData = text.data();
                query.cch = static_cast<UINT>(text.size());
                if (GetMenuItemInfoW(menu, position, TRUE, &query))
                    text.resize(wcslen(text.c_str()));
                else
                    text.clear();
            } catch (...) {
                text.clear();
            }
        }

        if (!(query.fType & MFT_SEPARATOR) &&
            CaptionMatchesOpenWith(text.c_str(), localizedCaptions)) {
            scan.foundLocalizedContainer = true;
            if (query.hSubMenu) {
                // The language-independent choice inside an Open With submenu
                // is its final actionable command (normally “Choose another
                // app”). Rescan after TrackPopupMenu too, because Windows can
                // populate this submenu lazily on WM_INITMENUPOPUP.
                AddLastActionableSubmenuCommand(query.hSubMenu, scan);
            } else if (query.wID != static_cast<UINT>(-1) && query.wID != 0) {
                scan.commandIds.insert(query.wID);
            }
        }

        if (query.hSubMenu)
            ScanMenuForOpenWith(query.hSubMenu, localizedCaptions, scan);
    }
}

static std::wstring SelectedFileFromExplorerAutomation(HWND menuOwner) {
    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows,
                                reinterpret_cast<void**>(shellWindows.Put()))) ||
        !shellWindows) {
        return {};
    }

    HWND wantedRoot = menuOwner ? GetAncestor(menuOwner, GA_ROOT) : nullptr;
    if (!wantedRoot) {
        POINT cursor{};
        if (GetCursorPos(&cursor))
            wantedRoot = GetAncestor(WindowFromPoint(cursor), GA_ROOT);
    }

    long count = 0;
    if (FAILED(shellWindows->get_Count(&count))) return {};
    for (long index = 0; index < count; ++index) {
        VARIANT itemIndex;
        VariantInit(&itemIndex);
        itemIndex.vt = VT_I4;
        itemIndex.lVal = index;

        ComPtr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(itemIndex, dispatch.Put())) || !dispatch)
            continue;
        ComPtr<IWebBrowserApp> browser;
        if (FAILED(dispatch->QueryInterface(
                IID_IWebBrowserApp,
                reinterpret_cast<void**>(browser.Put()))) || !browser) {
            continue;
        }

        SHANDLE_PTR browserHandle = 0;
        browser->get_HWND(&browserHandle);
        HWND browserWindow = reinterpret_cast<HWND>(browserHandle);
        if (wantedRoot && browserWindow != wantedRoot) continue;

        ComPtr<IDispatch> document;
        if (FAILED(browser->get_Document(document.Put())) || !document) continue;
        ComPtr<IShellFolderViewDual> folderView;
        if (FAILED(document->QueryInterface(
                IID_IShellFolderViewDual,
                reinterpret_cast<void**>(folderView.Put()))) || !folderView) {
            continue;
        }

        ComPtr<FolderItems> selected;
        if (FAILED(folderView->SelectedItems(selected.Put())) || !selected)
            continue;
        long selectedCount = 0;
        if (FAILED(selected->get_Count(&selectedCount)) || selectedCount != 1)
            continue;

        VARIANT selectedIndex;
        VariantInit(&selectedIndex);
        selectedIndex.vt = VT_I4;
        selectedIndex.lVal = 0;
        ComPtr<FolderItem> selectedItem;
        if (FAILED(selected->Item(selectedIndex, selectedItem.Put())) ||
            !selectedItem) {
            continue;
        }

        BSTR rawPath = nullptr;
        if (SUCCEEDED(selectedItem->get_Path(&rawPath)) && rawPath) {
            std::wstring path;
            try {
                path.assign(rawPath, SysStringLen(rawPath));
            } catch (...) {
            }
            SysFreeString(rawPath);
            if (IsSupportedFile(path)) return path;
        } else if (rawPath) {
            SysFreeString(rawPath);
        }
    }
    return {};
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
using TrackPopupMenu_t = decltype(&TrackPopupMenu);
static TrackPopupMenuEx_t TrackPopupMenuExOriginal = nullptr;
static TrackPopupMenu_t TrackPopupMenuOriginal = nullptr;
static thread_local bool g_insideTrackPopupHook = false;

static BOOL FinishTrackedMenuCommand(UINT selectedCommand, UINT originalFlags,
                                     HWND owner,
                                     const std::wstring& selectedPath,
                                     const OpenWithMenuCommandScan& scan) {
    const bool selectedOpenWith =
        selectedCommand &&
        scan.commandIds.find(selectedCommand) != scan.commandIds.end();
    if (selectedOpenWith && IsSupportedFile(selectedPath) &&
        QueuePicker(owner, selectedPath.c_str())) {
        Wh_Log(L"Standalone Open With: redirected visible HMENU command id=%u "
               L"path=%s", selectedCommand, selectedPath.c_str());
        g_lastOpenWithContextPath.clear();
        // Tell a TPM_RETURNCMD caller that the menu was cancelled, so it doesn't
        // invoke Windows' original command. For normal TrackPopupMenu callers,
        // report that a command was handled.
        return (originalFlags & TPM_RETURNCMD) ? 0 : TRUE;
    }

    if (originalFlags & TPM_RETURNCMD)
        return static_cast<BOOL>(selectedCommand);
    if (selectedCommand && owner)
        SendMessageW(owner, WM_COMMAND, MAKEWPARAM(selectedCommand, 0), 0);
    return selectedCommand ? TRUE : FALSE;
}

static BOOL WINAPI TrackPopupMenuExHook(HMENU menu, UINT flags, int x, int y,
                                        HWND owner, LPTPMPARAMS parameters) {
    if (!TrackPopupMenuExOriginal || g_insideTrackPopupHook ||
        !g_contextMenuTextRedirect.load(std::memory_order_acquire)) {
        return TrackPopupMenuExOriginal
                   ? TrackPopupMenuExOriginal(menu, flags, x, y, owner,
                                              parameters)
                   : FALSE;
    }

    const std::vector<std::wstring> captions =
        LoadLocalizedOpenWithCaptions();
    OpenWithMenuCommandScan before;
    ScanMenuForOpenWith(menu, captions, before);
    if (!before.foundLocalizedContainer) {
        return TrackPopupMenuExOriginal(menu, flags, x, y, owner, parameters);
    }

    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitialize = SUCCEEDED(com);
    std::wstring selectedPath = SelectedFileFromExplorerAutomation(owner);
    if (uninitialize) CoUninitialize();
    if (!IsSupportedFile(selectedPath) &&
        IsSupportedFile(g_lastOpenWithContextPath)) {
        selectedPath = g_lastOpenWithContextPath;
        Wh_Log(L"Standalone Open With: using path captured by "
               L"IShellExtInit for localized HMENU");
    }
    if (!IsSupportedFile(selectedPath)) {
        Wh_Log(L"Standalone Open With: localized HMENU found but no single "
               L"Explorer file was resolved");
        return TrackPopupMenuExOriginal(menu, flags, x, y, owner, parameters);
    }

    UINT command = 0;
    {
        BoolFlagGuard guard(g_insideTrackPopupHook);
        command = static_cast<UINT>(TrackPopupMenuExOriginal(
            menu, flags | TPM_RETURNCMD, x, y, owner, parameters));
    }

    OpenWithMenuCommandScan after = before;
    ScanMenuForOpenWith(menu, captions, after);
    return FinishTrackedMenuCommand(command, flags, owner, selectedPath, after);
}

static BOOL WINAPI TrackPopupMenuHook(HMENU menu, UINT flags, int x, int y,
                                      int reserved, HWND owner,
                                      const RECT* exclude) {
    if (!TrackPopupMenuOriginal || g_insideTrackPopupHook ||
        !g_contextMenuTextRedirect.load(std::memory_order_acquire)) {
        return TrackPopupMenuOriginal
                   ? TrackPopupMenuOriginal(menu, flags, x, y, reserved, owner,
                                            exclude)
                   : FALSE;
    }

    const std::vector<std::wstring> captions =
        LoadLocalizedOpenWithCaptions();
    OpenWithMenuCommandScan before;
    ScanMenuForOpenWith(menu, captions, before);
    if (!before.foundLocalizedContainer)
        return TrackPopupMenuOriginal(menu, flags, x, y, reserved, owner,
                                      exclude);

    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitialize = SUCCEEDED(com);
    std::wstring selectedPath = SelectedFileFromExplorerAutomation(owner);
    if (uninitialize) CoUninitialize();
    if (!IsSupportedFile(selectedPath) &&
        IsSupportedFile(g_lastOpenWithContextPath)) {
        selectedPath = g_lastOpenWithContextPath;
        Wh_Log(L"Standalone Open With: using path captured by "
               L"IShellExtInit for localized HMENU");
    }
    if (!IsSupportedFile(selectedPath))
        return TrackPopupMenuOriginal(menu, flags, x, y, reserved, owner,
                                      exclude);

    UINT command = 0;
    {
        BoolFlagGuard guard(g_insideTrackPopupHook);
        command = static_cast<UINT>(TrackPopupMenuOriginal(
            menu, flags | TPM_RETURNCMD, x, y, reserved, owner, exclude));
    }

    OpenWithMenuCommandScan after = before;
    ScanMenuForOpenWith(menu, captions, after);
    return FinishTrackedMenuCommand(command, flags, owner, selectedPath, after);
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
static std::atomic<bool> g_usePartialOpenWithMenuFactory{false};

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
        if (identity && IsSupportedFile(path)) {
            g_lastOpenWithContextPath = path;
            std::lock_guard<std::mutex> lock(g_openWithMenuStateMutex);
            // Bound stale entries left when a context menu is dismissed.
            if (g_openWithMenuStates.size() > 256)
                g_openWithMenuStates.clear();
            g_openWithMenuStates[identity] =
                OpenWithMenuState{std::move(path), -1};
            Wh_Log(L"Standalone Open With: captured CLSID_OpenWithMenu item");
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
            g_lastOpenWithContextPath.clear();
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

// Minimal independent implementation used only if the system context-menu COM
// object cannot be probed. It implements the stable public interfaces needed by
// Explorer and inserts a single canonical "openas" command.
class PartialOpenWithMenu final : public IContextMenu, public IShellExtInit {
   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                             void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) ||
            IsEqualIID(iid, IID_IContextMenu)) {
            *object = static_cast<IContextMenu*>(this);
        } else if (IsEqualIID(iid, IID_IShellExtInit)) {
            *object = static_cast<IShellExtInit*>(this);
        } else {
            return E_NOINTERFACE;
        }
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

    HRESULT STDMETHODCALLTYPE Initialize(PCIDLIST_ABSOLUTE,
                                         IDataObject* dataObject,
                                         HKEY) override {
        try {
            path_ = PathFromDataObject(dataObject);
        } catch (...) {
            path_.clear();
        }
        return IsSupportedFile(path_) ? S_OK : E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU menu, UINT index,
                                               UINT first, UINT, UINT flags) override {
        if (!menu || path_.empty()) return E_FAIL;
        if (flags & (CMF_DEFAULTONLY | CMF_NOVERBS))
            return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

        MENUITEMINFOW item{sizeof(item)};
        item.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE;
        item.wID = first;
        item.fState = MFS_ENABLED;
        item.dwTypeData = const_cast<PWSTR>(LOC(STR_TITLE));
        if (!InsertMenuItemW(menu, index, TRUE, &item))
            return HRESULT_FROM_WIN32(GetLastError());
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
    }

    HRESULT STDMETHODCALLTYPE InvokeCommand(
        LPCMINVOKECOMMANDINFO info) override {
        if (!info || path_.empty()) return E_INVALIDARG;
        const bool ours = IS_INTRESOURCE(info->lpVerb)
                              ? LOWORD(reinterpret_cast<ULONG_PTR>(
                                    info->lpVerb)) == 0
                              : info->lpVerb &&
                                    !_stricmp(info->lpVerb, "openas");
        return ours && QueuePicker(info->hwnd, path_.c_str()) ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR command, UINT type,
                                               UINT*, LPSTR output,
                                               UINT chars) override {
        if (command != 0) return E_INVALIDARG;
        if (type == GCS_VALIDATEA || type == GCS_VALIDATEW) return S_OK;
        if (!output || !chars) return E_POINTER;
        if (type == GCS_VERBW) {
            lstrcpynW(reinterpret_cast<PWSTR>(output), L"openas", chars);
            return S_OK;
        }
        if (type == GCS_VERBA) {
            lstrcpynA(output, "openas", chars);
            return S_OK;
        }
        if (type == GCS_HELPTEXTW) {
            lstrcpynW(reinterpret_cast<PWSTR>(output), LOC(STR_INSTRUCTION),
                      chars);
            return S_OK;
        }
        if (type == GCS_HELPTEXTA) {
            lstrcpynA(output, "Choose a program to open this file", chars);
            return S_OK;
        }
        return E_NOTIMPL;
    }

   private:
    std::atomic<ULONG> references_{1};
    std::wstring path_;
};

class PartialOpenWithClassFactory final : public IClassFactory {
   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                             void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (!IsEqualIID(iid, IID_IUnknown) &&
            !IsEqualIID(iid, IID_IClassFactory))
            return E_NOINTERFACE;
        *object = static_cast<IClassFactory*>(this);
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
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID iid,
                                             void** object) override {
        if (outer) return CLASS_E_NOAGGREGATION;
        if (!object) return E_POINTER;
        *object = nullptr;
        PartialOpenWithMenu* menu = nullptr;
        try {
            menu = new PartialOpenWithMenu();
        } catch (...) {
            return E_OUTOFMEMORY;
        }
        const HRESULT hr = menu->QueryInterface(iid, object);
        menu->Release();
        return hr;
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) override { return S_OK; }

   private:
    std::atomic<ULONG> references_{1};
};

using DllGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);
static DllGetClassObject_t Shell32DllGetClassObjectOriginal = nullptr;

static HRESULT WINAPI Shell32DllGetClassObjectHook(REFCLSID clsid,
                                                    REFIID iid,
                                                    LPVOID* object) {
    try {
        if (object &&
            g_usePartialOpenWithMenuFactory.load(std::memory_order_acquire) &&
            IsEqualCLSID(clsid, kClsidOpenWithMenu) &&
            (IsEqualIID(iid, IID_IClassFactory) ||
             IsEqualIID(iid, IID_IUnknown))) {
            *object = nullptr;
            PartialOpenWithClassFactory* factory = nullptr;
            try {
                factory = new PartialOpenWithClassFactory();
            } catch (...) {
                return E_OUTOFMEMORY;
            }
            const HRESULT hr = factory->QueryInterface(iid, object);
            factory->Release();
            Wh_Log(L"Standalone Open With: supplied partial "
                   L"CLSID_OpenWithMenu class factory hr=0x%08X",
                   static_cast<unsigned int>(hr));
            return hr;
        }
    } catch (...) {
    }
    return Shell32DllGetClassObjectOriginal
               ? Shell32DllGetClassObjectOriginal(clsid, iid, object)
               : CLASS_E_CLASSNOTAVAILABLE;
}

// -----------------------------------------------------------------------------
// Public API and modern COM activation hooks. Every non-exact or unsupported
// request fails open.
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
    // A property sheet is a #32770 root containing SysTabControl32. Checking
    // only #32770 would also classify unrelated Shell dialogs as Properties.
    HWND root = GetAncestor(owner, GA_ROOT);
    if (!root) root = owner;
    if (!ClassNameEquals(root, L"#32770")) return false;
    return FindWindowExW(root, nullptr, WC_TABCONTROLW, nullptr) != nullptr;
}

static bool ShouldSetDefaultOnly(HWND owner, DWORD flags) {
    wchar_t ownerClass[128] = L"(none)";
    wchar_t rootClass[128] = L"(none)";
    HWND root = owner ? GetAncestor(owner, GA_ROOT) : nullptr;
    if (owner) GetClassNameW(owner, ownerClass, ARRAYSIZE(ownerClass));
    if (root) GetClassNameW(root, rootClass, ARRAYSIZE(rootClass));
    const bool properties =
        (flags & kImmersiveOpenWithDoNotExec) && IsFilePropertiesOwner(owner);
    Wh_Log(L"Standalone Open With: classify Launch flags=0x%08X "
           L"ownerClass=%s rootClass=%s intent=%s",
           flags, ownerClass, rootClass,
           properties ? L"set-default" : L"open-file");
    return properties;
}

// Windows 10/11 normally doesn't launch OpenWith.exe directly from Explorer.
// Explorer activates CLSID_ExecuteUnknown; DCOM's svchost.exe then starts
// "OpenWith.exe -Embedding", and the target path is passed later through this
// private interface (not on the command line). Returning this small in-process
// proxy prevents the DCOM server from being launched for supported requests.
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

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD,
                                            REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstanceOriginal = nullptr;

class OpenWithLauncherProxy final : public StandaloneOpenWithLauncher {
   public:
    explicit OpenWithLauncherProxy(DWORD classContext)
        : classContext_(classContext) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                             void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) ||
            IsEqualIID(iid, kIidOpenWithLauncher)) {
            *object = static_cast<StandaloneOpenWithLauncher*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
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

    HRESULT STDMETHODCALLTYPE Launch(HWND owner, PCWSTR path,
                                     DWORD flags) override {
        try {
            Wh_Log(L"Standalone Open With: IOpenWithLauncher::Launch "
                   L"owner=%p flags=0x%08X path=%s",
                   owner, flags, path ? path : L"(null)");

            // On Windows 10 21H2, bit 0x4 is observed both on association
            // management and on some automatic unknown-file paths. Classify by
            // the owning window instead of discarding every 0x4 request.
            const bool setDefaultOnly = ShouldSetDefaultOnly(owner, flags);
            if (!setDefaultOnly && path &&
                TryInvokeStoredAssociation(path)) {
                return S_OK;
            }
            if (QueuePicker(owner, path, setDefaultOnly)) return S_OK;
            if (path && IsSupportedFile(path)) return E_FAIL;
        } catch (...) {
            Wh_Log(L"Standalone Open With: launcher proxy exception; "
                   L"falling back to the system COM server");
        }

        // Preserve Windows behavior for disabled, unsupported or failed
        // requests. Calling the trampoline avoids re-entering our hook.
        if (!CoCreateInstanceOriginal) return E_FAIL;
        ComPtr<StandaloneOpenWithLauncher> original;
        const DWORD context = classContext_ ? classContext_ : CLSCTX_LOCAL_SERVER;
        HRESULT hr = CoCreateInstanceOriginal(
            kClsidExecuteUnknown, nullptr, context, kIidOpenWithLauncher,
            reinterpret_cast<void**>(original.Put()));
        return SUCCEEDED(hr) && original
                   ? original->Launch(owner, path, flags)
                   : hr;
    }

   private:
    std::atomic<ULONG> references_{1};
    DWORD classContext_;
};

static HRESULT WINAPI CoCreateInstanceHook(
    REFCLSID clsid, LPUNKNOWN outer, DWORD classContext, REFIID iid,
    LPVOID* object) {
    try {
        if (!outer && object &&
            g_usePartialOpenWithMenuFactory.load(std::memory_order_acquire) &&
            IsEqualCLSID(clsid, kClsidOpenWithMenu) &&
            (IsEqualIID(iid, IID_IUnknown) ||
             IsEqualIID(iid, IID_IContextMenu) ||
             IsEqualIID(iid, IID_IShellExtInit))) {
            *object = nullptr;
            PartialOpenWithMenu* menu = nullptr;
            try {
                menu = new PartialOpenWithMenu();
            } catch (...) {
                return E_OUTOFMEMORY;
            }
            const HRESULT hr = menu->QueryInterface(iid, object);
            menu->Release();
            if (SUCCEEDED(hr)) {
                Wh_Log(L"Standalone Open With: supplied partial "
                       L"CLSID_OpenWithMenu object through CoCreateInstance");
                return hr;
            }
        }

        if (!outer && object &&
            g_replaceSystemDialog.load(std::memory_order_acquire) &&
            IsEqualCLSID(clsid, kClsidExecuteUnknown) &&
            (IsEqualIID(iid, IID_IUnknown) ||
             IsEqualIID(iid, kIidOpenWithLauncher))) {
            *object = nullptr;
            OpenWithLauncherProxy* proxy = nullptr;
            try {
                proxy = new OpenWithLauncherProxy(classContext);
            } catch (...) {
                return E_OUTOFMEMORY;
            }
            const HRESULT hr = proxy->QueryInterface(iid, object);
            proxy->Release();
            if (SUCCEEDED(hr)) {
                Wh_Log(L"Standalone Open With: intercepted "
                       L"CLSID_ExecuteUnknown activation (iid=%s)",
                       IsEqualIID(iid, IID_IUnknown)
                           ? L"IUnknown"
                           : L"IOpenWithLauncher");
                return hr;
            }
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: CoCreateInstance hook exception; "
               L"falling back to original");
    }

    return CoCreateInstanceOriginal
               ? CoCreateInstanceOriginal(clsid, outer, classContext, iid,
                                          object)
               : E_FAIL;
}

using CoCreateInstanceEx_t = HRESULT(WINAPI*)(
    REFCLSID, IUnknown*, DWORD, COSERVERINFO*, DWORD, MULTI_QI*);
static CoCreateInstanceEx_t CoCreateInstanceExOriginal = nullptr;

static bool MultiQiSupportsPartialOpenWithMenu(DWORD count,
                                                MULTI_QI* results) {
    if (!count || !results) return false;
    for (DWORD i = 0; i < count; ++i) {
        if (!results[i].pIID ||
            (!IsEqualIID(*results[i].pIID, IID_IUnknown) &&
             !IsEqualIID(*results[i].pIID, IID_IContextMenu) &&
             !IsEqualIID(*results[i].pIID, IID_IShellExtInit))) {
            return false;
        }
    }
    return true;
}

static bool MultiQiSupportsOpenWithLauncher(DWORD count,
                                             MULTI_QI* results) {
    if (!count || !results) return false;
    for (DWORD i = 0; i < count; ++i) {
        if (!results[i].pIID ||
            (!IsEqualIID(*results[i].pIID, IID_IUnknown) &&
             !IsEqualIID(*results[i].pIID, kIidOpenWithLauncher))) {
            return false;
        }
    }
    return true;
}

template <typename Object>
static HRESULT FillMultiQi(Object* object, DWORD count, MULTI_QI* results) {
    if (!object || !count || !results) return E_INVALIDARG;
    HRESULT overall = S_OK;
    for (DWORD i = 0; i < count; ++i) {
        results[i].pItf = nullptr;
        results[i].hr = results[i].pIID
                            ? object->QueryInterface(*results[i].pIID,
                                                     reinterpret_cast<void**>(
                                                         &results[i].pItf))
                            : E_NOINTERFACE;
        if (FAILED(results[i].hr)) overall = CO_S_NOTALLINTERFACES;
    }
    return overall;
}

static void LogRequestedMultiQi(PCWSTR label, DWORD count,
                                MULTI_QI* results) {
    if (!results) return;
    for (DWORD i = 0; i < count; ++i) {
        wchar_t iid[64] = L"(null)";
        if (results[i].pIID)
            StringFromGUID2(*results[i].pIID, iid, ARRAYSIZE(iid));
        Wh_Log(L"Standalone Open With: %s CoCreateInstanceEx IID[%u]=%s",
               label, i, iid);
    }
}

static HRESULT WINAPI CoCreateInstanceExHook(
    REFCLSID clsid, IUnknown* outer, DWORD classContext,
    COSERVERINFO* serverInfo, DWORD count, MULTI_QI* results) {
    try {
        if (!outer && !serverInfo &&
            g_usePartialOpenWithMenuFactory.load(std::memory_order_acquire) &&
            IsEqualCLSID(clsid, kClsidOpenWithMenu)) {
            LogRequestedMultiQi(L"CLSID_OpenWithMenu", count, results);
            if (MultiQiSupportsPartialOpenWithMenu(count, results)) {
                PartialOpenWithMenu* menu = nullptr;
                try {
                    menu = new PartialOpenWithMenu();
                } catch (...) {
                    return E_OUTOFMEMORY;
                }
                const HRESULT hr = FillMultiQi(menu, count, results);
                menu->Release();
                return hr;
            }
        }

        if (!outer && !serverInfo &&
            g_replaceSystemDialog.load(std::memory_order_acquire) &&
            IsEqualCLSID(clsid, kClsidExecuteUnknown)) {
            LogRequestedMultiQi(L"CLSID_ExecuteUnknown", count, results);
            if (MultiQiSupportsOpenWithLauncher(count, results)) {
                OpenWithLauncherProxy* launcher = nullptr;
                try {
                    launcher = new OpenWithLauncherProxy(classContext);
                } catch (...) {
                    return E_OUTOFMEMORY;
                }
                const HRESULT hr = FillMultiQi(launcher, count, results);
                launcher->Release();
                Wh_Log(L"Standalone Open With: intercepted "
                       L"CLSID_ExecuteUnknown through CoCreateInstanceEx");
                return hr;
            }
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: CoCreateInstanceEx hook exception; "
               L"falling back to original");
    }

    return CoCreateInstanceExOriginal
               ? CoCreateInstanceExOriginal(clsid, outer, classContext,
                                            serverInfo, count, results)
               : E_FAIL;
}

// -----------------------------------------------------------------------------
// OpenWith.exe-side COM server interception.
//
// Automatic unknown-file activation can bypass every exported Shell function
// in Explorer. The invariant boundary is the local COM server itself:
// OpenWith.exe registers CLSID_ExecuteUnknown with CoRegisterClassObject, its
// class factory creates IOpenWithLauncher, and Launch receives the real path.
// Discover and hook both vtable targets at runtime, without symbols/offsets.
// -----------------------------------------------------------------------------

using CoRegisterClassObject_t = HRESULT(WINAPI*)(
    REFCLSID, IUnknown*, DWORD, DWORD, LPDWORD);
using ServerFactoryCreateInstance_t = HRESULT(STDMETHODCALLTYPE*)(
    IClassFactory*, IUnknown*, REFIID, void**);
static CoRegisterClassObject_t CoRegisterClassObjectOriginal = nullptr;
static ServerFactoryCreateInstance_t ServerFactoryCreateInstanceOriginal =
    nullptr;
static std::mutex g_serverHookMutex;
static bool g_serverFactoryHookInstalled = false;

// Replacement COM object returned by OpenWith.exe's real class factory hook.
// Unlike detouring the original Launch implementation, this also catches RPC
// dispatch paths which obtain IID_IUnknown first and only QI the launcher after
// the class-factory call has returned.
static std::atomic<unsigned int> g_unsupportedReplacementQiLogs{0};

class ServerOpenWithLauncherReplacement final
    : public StandaloneOpenWithLauncher,
      public IExecuteCommand,
      public IObjectWithSelection,
      public IObjectWithSite,
      public IInitializeCommand {
   public:
    explicit ServerOpenWithLauncherReplacement(IClassFactory* originalFactory)
        : originalFactory_(originalFactory) {
        if (originalFactory_) {
            originalFactory_->AddRef();
            // Keep OpenWith.exe's normal server lifetime accounting balanced
            // while our replacement object is exposed through COM.
            originalFactory_->LockServer(TRUE);
        }
    }

    ~ServerOpenWithLauncherReplacement() {
        if (originalFactory_) {
            originalFactory_->LockServer(FALSE);
            originalFactory_->Release();
        }
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                             void** object) override {
        try {
            if (!object) return E_POINTER;
            *object = nullptr;
            if (IsEqualIID(iid, IID_IUnknown) ||
                IsEqualIID(iid, kIidOpenWithLauncher)) {
                *object = static_cast<StandaloneOpenWithLauncher*>(this);
            } else if (IsEqualIID(iid, IID_IExecuteCommand)) {
                *object = static_cast<IExecuteCommand*>(this);
            } else if (IsEqualIID(iid, IID_IObjectWithSelection)) {
                *object = static_cast<IObjectWithSelection*>(this);
            } else if (IsEqualIID(iid, IID_IObjectWithSite)) {
                *object = static_cast<IObjectWithSite*>(this);
            } else if (IsEqualIID(iid, IID_IInitializeCommand)) {
                *object = static_cast<IInitializeCommand*>(this);
            } else {
                const unsigned int logIndex =
                    g_unsupportedReplacementQiLogs.fetch_add(
                        1, std::memory_order_relaxed);
                if (logIndex < 12) {
                    wchar_t iidText[64] = {};
                    StringFromGUID2(iid, iidText, ARRAYSIZE(iidText));
                    Wh_Log(L"Standalone Open With: replacement QI unsupported "
                           L"iid=%s", iidText);
                } else if (logIndex == 12) {
                    Wh_Log(L"Standalone Open With: further unsupported QI "
                           L"diagnostics suppressed");
                }
                return E_NOINTERFACE;
            }
            AddRef();
            return S_OK;
        } catch (...) {
            return E_FAIL;
        }
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

    // IOpenWithLauncher: used by Properties/Change and some direct callers.
    HRESULT STDMETHODCALLTYPE Launch(HWND owner, PCWSTR path,
                                     DWORD flags) override {
        try {
            Wh_Log(L"Standalone Open With: replacement COM Launch "
                   L"flags=0x%08X path=%s", flags,
                   path ? path : L"(null)");
            if (path &&
                g_replaceSystemDialog.load(std::memory_order_acquire)) {
                std::wstring copy;
                try {
                    copy.assign(path);
                } catch (...) {
                    return E_OUTOFMEMORY;
                }
                const bool setDefaultOnly =
                    ShouldSetDefaultOnly(owner, flags);
                if (!setDefaultOnly &&
                    TryInvokeStoredAssociation(copy)) {
                    return S_OK;
                }
                if (IsSupportedFile(copy)) {
                    if (QueuePickerAndWait(owner, copy.c_str(),
                                           setDefaultOnly)) {
                        return S_OK;
                    }
                    // A valid file handled by this replacement must never fall
                    // through to the modern picker merely because the worker
                    // was busy or shutting down.
                    return HRESULT_FROM_WIN32(ERROR_CANCELLED);
                }
            }
        } catch (...) {
            Wh_Log(L"Standalone Open With: replacement COM Launch exception");
        }
        return LaunchOriginal(owner, path, flags);
    }

    // IExecuteCommand: used by the Unknown/DelegateExecute double-click path.
    HRESULT STDMETHODCALLTYPE SetKeyState(DWORD keyState) override {
        keyState_ = keyState;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetParameters(PCWSTR parameters) override {
        try {
            parameters_ = parameters ? parameters : L"";
            return S_OK;
        } catch (...) {
            return E_OUTOFMEMORY;
        }
    }

    HRESULT STDMETHODCALLTYPE SetPosition(POINT position) override {
        position_ = position;
        hasPosition_ = true;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetShowWindow(int show) override {
        show_ = show;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetNoShowUI(BOOL noShowUi) override {
        noShowUi_ = noShowUi;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetDirectory(PCWSTR directory) override {
        try {
            directory_ = directory ? directory : L"";
            return S_OK;
        } catch (...) {
            return E_OUTOFMEMORY;
        }
    }

    HRESULT STDMETHODCALLTYPE Execute() override {
        try {
            const std::wstring path = SelectedPath();
            const HWND owner = SiteWindow();
            Wh_Log(L"Standalone Open With: replacement IExecuteCommand::Execute "
                   L"path=%s owner=%p", path.empty() ? L"(empty)" : path.c_str(),
                   owner);
            if (IsSupportedFile(path) &&
                g_replaceSystemDialog.load(std::memory_order_acquire)) {
                if (TryInvokeStoredAssociation(path)) return S_OK;
                if (QueuePickerAndWait(owner, path.c_str(), false))
                    return S_OK;
                return HRESULT_FROM_WIN32(ERROR_CANCELLED);
            }
        } catch (...) {
            Wh_Log(L"Standalone Open With: replacement Execute exception");
        }
        return ExecuteOriginal();
    }

    // IObjectWithSelection supplies the file for DelegateExecute.
    HRESULT STDMETHODCALLTYPE SetSelection(IShellItemArray* selection) override {
        try {
            if (selection) selection->AddRef();
            selection_.Reset(selection);
            return S_OK;
        } catch (...) {
            return E_FAIL;
        }
    }

    HRESULT STDMETHODCALLTYPE GetSelection(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        return selection_ ? selection_->QueryInterface(iid, object) : E_FAIL;
    }

    // IObjectWithSite supplies the Explorer/property-sheet owner.
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override {
        try {
            if (site) site->AddRef();
            site_.Reset(site);
            return S_OK;
        } catch (...) {
            return E_FAIL;
        }
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        return site_ ? site_->QueryInterface(iid, object) : E_FAIL;
    }

    // IInitializeCommand carries the canonical verb/property bag.
    HRESULT STDMETHODCALLTYPE Initialize(PCWSTR commandName,
                                         IPropertyBag* propertyBag) override {
        try {
            commandName_ = commandName ? commandName : L"";
            if (propertyBag) propertyBag->AddRef();
            propertyBag_.Reset(propertyBag);
            return S_OK;
        } catch (...) {
            return E_OUTOFMEMORY;
        }
    }

   private:
    std::wstring SelectedPath() const {
        if (selection_) {
            DWORD count = 0;
            if (SUCCEEDED(selection_->GetCount(&count)) && count == 1) {
                ComPtr<IShellItem> item;
                if (SUCCEEDED(selection_->GetItemAt(0, item.Put())) && item) {
                    PWSTR raw = nullptr;
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH,
                                                       &raw)) && raw) {
                        return TakeTaskString(raw);
                    }
                    if (raw) CoTaskMemFree(raw);
                }
            }
        }

        std::wstring candidate = parameters_;
        const size_t first = candidate.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) return {};
        const size_t last = candidate.find_last_not_of(L" \t\r\n");
        candidate = candidate.substr(first, last - first + 1);
        if (candidate.size() >= 2 && candidate.front() == L'"' &&
            candidate.back() == L'"') {
            candidate = candidate.substr(1, candidate.size() - 2);
        }
        return IsSupportedFile(candidate) ? candidate : std::wstring{};
    }

    HWND SiteWindow() const {
        if (site_) {
            ComPtr<IOleWindow> oleWindow;
            if (SUCCEEDED(site_->QueryInterface(
                    IID_IOleWindow,
                    reinterpret_cast<void**>(oleWindow.Put()))) &&
                oleWindow) {
                HWND window = nullptr;
                if (SUCCEEDED(oleWindow->GetWindow(&window)) && window)
                    return window;
            }
        }
        return GetForegroundWindow();
    }

    HRESULT LaunchOriginal(HWND owner, PCWSTR path, DWORD flags) {
        if (!originalFactory_ || !ServerFactoryCreateInstanceOriginal)
            return E_FAIL;
        ComPtr<StandaloneOpenWithLauncher> original;
        HRESULT hr = ServerFactoryCreateInstanceOriginal(
            originalFactory_, nullptr, kIidOpenWithLauncher,
            reinterpret_cast<void**>(original.Put()));
        if (FAILED(hr) || !original) return hr;
        return original->Launch(owner, path, flags);
    }

    HRESULT ExecuteOriginal() {
        if (!originalFactory_ || !ServerFactoryCreateInstanceOriginal)
            return E_FAIL;
        ComPtr<IExecuteCommand> original;
        HRESULT hr = ServerFactoryCreateInstanceOriginal(
            originalFactory_, nullptr, IID_IExecuteCommand,
            reinterpret_cast<void**>(original.Put()));
        if (FAILED(hr) || !original) return hr;

        if (!commandName_.empty() || propertyBag_) {
            ComPtr<IInitializeCommand> initialize;
            if (SUCCEEDED(original->QueryInterface(
                    IID_IInitializeCommand,
                    reinterpret_cast<void**>(initialize.Put()))) &&
                initialize) {
                initialize->Initialize(
                    commandName_.empty() ? nullptr : commandName_.c_str(),
                    propertyBag_.Get());
            }
        }
        if (selection_) {
            ComPtr<IObjectWithSelection> withSelection;
            if (SUCCEEDED(original->QueryInterface(
                    IID_IObjectWithSelection,
                    reinterpret_cast<void**>(withSelection.Put()))) &&
                withSelection) {
                withSelection->SetSelection(selection_.Get());
            }
        }
        if (site_) {
            ComPtr<IObjectWithSite> withSite;
            if (SUCCEEDED(original->QueryInterface(
                    IID_IObjectWithSite,
                    reinterpret_cast<void**>(withSite.Put()))) &&
                withSite) {
                withSite->SetSite(site_.Get());
            }
        }
        original->SetKeyState(keyState_);
        if (!parameters_.empty()) original->SetParameters(parameters_.c_str());
        if (hasPosition_) original->SetPosition(position_);
        original->SetShowWindow(show_);
        original->SetNoShowUI(noShowUi_);
        if (!directory_.empty()) original->SetDirectory(directory_.c_str());
        return original->Execute();
    }

    std::atomic<ULONG> references_{1};
    IClassFactory* originalFactory_ = nullptr;
    ComPtr<IShellItemArray> selection_;
    ComPtr<IUnknown> site_;
    ComPtr<IPropertyBag> propertyBag_;
    std::wstring commandName_;
    std::wstring parameters_;
    std::wstring directory_;
    DWORD keyState_ = 0;
    POINT position_{};
    bool hasPosition_ = false;
    int show_ = SW_SHOWNORMAL;
    BOOL noShowUi_ = FALSE;
};

static HRESULT STDMETHODCALLTYPE ServerFactoryCreateInstanceHook(
    IClassFactory* self, IUnknown* outer, REFIID iid, void** object) {
    try {
        wchar_t iidText[64] = {};
        StringFromGUID2(iid, iidText, ARRAYSIZE(iidText));

        if (!outer && object &&
            g_replaceSystemDialog.load(std::memory_order_acquire) &&
            (IsEqualIID(iid, IID_IUnknown) ||
             IsEqualIID(iid, kIidOpenWithLauncher) ||
             IsEqualIID(iid, IID_IExecuteCommand) ||
             IsEqualIID(iid, IID_IObjectWithSelection) ||
             IsEqualIID(iid, IID_IObjectWithSite) ||
             IsEqualIID(iid, IID_IInitializeCommand))) {
            *object = nullptr;
            ServerOpenWithLauncherReplacement* replacement = nullptr;
            try {
                replacement =
                    new ServerOpenWithLauncherReplacement(self);
            } catch (...) {
                return E_OUTOFMEMORY;
            }
            const HRESULT hr = replacement->QueryInterface(iid, object);
            replacement->Release();
            Wh_Log(L"Standalone Open With: server factory replaced object "
                   L"iid=%s hr=0x%08X object=%p", iidText,
                   static_cast<unsigned int>(hr),
                   object ? *object : nullptr);
            return hr;
        }

        const HRESULT hr = ServerFactoryCreateInstanceOriginal
            ? ServerFactoryCreateInstanceOriginal(self, outer, iid, object)
            : E_FAIL;
        Wh_Log(L"Standalone Open With: server factory passed through "
               L"iid=%s hr=0x%08X object=%p", iidText,
               static_cast<unsigned int>(hr),
               object ? *object : nullptr);
        return hr;
    } catch (...) {
        Wh_Log(L"Standalone Open With: server factory hook exception");
        return ServerFactoryCreateInstanceOriginal
            ? ServerFactoryCreateInstanceOriginal(self, outer, iid, object)
            : E_FAIL;
    }
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
    if (!createInstance || !Wh_SetFunctionHook(
                               reinterpret_cast<void*>(createInstance),
                               reinterpret_cast<void*>(
                                   ServerFactoryCreateInstanceHook),
                               reinterpret_cast<void**>(
                                   &ServerFactoryCreateInstanceOriginal))) {
        Wh_Log(L"Standalone Open With: failed to register server factory hook");
        return false;
    }
    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"Standalone Open With: failed to apply server factory hook");
        return false;
    }
    g_serverFactoryHookInstalled = true;
    Wh_Log(L"Standalone Open With: OpenWith.exe class-factory vtable hooked "
           L"at %p", reinterpret_cast<void*>(createInstance));
    return true;
}

static HRESULT WINAPI CoRegisterClassObjectHook(
    REFCLSID clsid, IUnknown* classObject, DWORD context, DWORD flags,
    LPDWORD registration) {
    try {
        wchar_t text[64] = {};
        StringFromGUID2(clsid, text, ARRAYSIZE(text));
        Wh_Log(L"Standalone Open With: CoRegisterClassObject clsid=%s "
               L"context=0x%X flags=0x%X", text, context, flags);
        if (IsEqualCLSID(clsid, kClsidExecuteUnknown))
            InstallServerFactoryHook(classObject);
    } catch (...) {
        Wh_Log(L"Standalone Open With: CoRegisterClassObject hook exception");
    }
    return CoRegisterClassObjectOriginal
               ? CoRegisterClassObjectOriginal(clsid, classObject, context,
                                               flags, registration)
               : E_FAIL;
}

// Direct OpenWith.exe command lines (drag/drop or explicit launch) don't use
// the COM server. If a real file is present in argv, replace the executable's
// entry point and run the standalone picker instead.
using ProcessEntryPoint_t = void(WINAPI*)();
static ProcessEntryPoint_t OpenWithEntryPointOriginal = nullptr;
static std::wstring g_directOpenWithPath;

static void WINAPI OpenWithEntryPointHook() {
    Wh_Log(L"Standalone Open With: intercepted direct OpenWith.exe entry "
           L"path=%s", g_directOpenWithPath.c_str());
    if (IsSupportedFile(g_directOpenWithPath))
        QueuePickerAndWait(nullptr, g_directOpenWithPath.c_str());
    ExitProcess(0);
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
        // TEMPORARY DIAGNOSTIC: log every CreateProcessW call whose
        // application name or command line mentions "open", to see whether
        // explorer.exe launches OpenWith.exe via this API at all.
        if ((applicationName && StrStrIW(applicationName, L"open")) ||
            (commandLine && StrStrIW(commandLine, L"open"))) {
            Wh_Log(L"Standalone Open With DIAG: CreateProcessW app=%s cmd=%s",
                   applicationName ? applicationName : L"(null)",
                   commandLine ? commandLine : L"(null)");
        }

        const bool looksLikeOpenWith =
            (applicationName && StrStrIW(applicationName, L"OpenWith.exe")) ||
            (commandLine && StrStrIW(commandLine, L"OpenWith.exe"));
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
            Wh_Log(L"Standalone Open With DIAG: matched OpenWith.exe but did "
                   L"not block (pathEmpty=%d)", path.empty());
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
        // Without OAIF_EXEC the caller is asking Windows to manage an
        // association rather than open the supplied item. This implementation
        // never claims that unsupported operation.
        if (info && (info->oaifInFlags & OAIF_EXEC) &&
            QueuePicker(owner, info->pcszFile)) {
            return S_OK;
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
            QueuePicker(info->hwnd, info->lpFile)) {
            info->hInstApp = reinterpret_cast<HINSTANCE>(33);
            info->hProcess = nullptr;
            return TRUE;
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
// Process-local registry virtualization for legacy Open With routing.
//
// No registry values are written. Explorer sees selected modern routing values
// as absent, while every other key/value is forwarded unchanged. Disabling or
// unloading the mod restores the real view immediately.
// -----------------------------------------------------------------------------

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegOpenKeyW_t = decltype(&RegOpenKeyW);
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
using RegGetValueW_t = decltype(&RegGetValueW);
using RegCloseKey_t = decltype(&RegCloseKey);

static RegOpenKeyExW_t RegOpenKeyExWOriginal = nullptr;
static RegOpenKeyW_t RegOpenKeyWOriginal = nullptr;
static RegQueryValueExW_t RegQueryValueExWOriginal = nullptr;
static RegGetValueW_t RegGetValueWOriginal = nullptr;
static RegCloseKey_t RegCloseKeyOriginal = nullptr;
static std::mutex g_virtualRegistryMutex;
static std::unordered_map<HKEY, std::wstring> g_virtualRegistryPaths;

static std::wstring NormalizeRegistryPath(PCWSTR path) {
    std::wstring value = path ? path : L"";
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    while (!value.empty() && value.back() == L'\\') value.pop_back();
    return value;
}

static std::wstring PredefinedRegistryPath(HKEY key) {
    if (key == HKEY_CLASSES_ROOT) return L"hkcr";
    if (key == HKEY_CURRENT_USER) return L"hkcu";
    if (key == HKEY_LOCAL_MACHINE) return L"hklm";
    if (key == HKEY_USERS) return L"hku";
    return {};
}

static std::wstring TrackedRegistryPath(HKEY key) {
    if (std::wstring predefined = PredefinedRegistryPath(key);
        !predefined.empty()) {
        return predefined;
    }
    std::lock_guard<std::mutex> lock(g_virtualRegistryMutex);
    auto found = g_virtualRegistryPaths.find(key);
    return found != g_virtualRegistryPaths.end() ? found->second
                                                  : std::wstring{};
}

static bool EndsWithRegistryPath(const std::wstring& path, PCWSTR suffix) {
    const std::wstring normalizedSuffix = NormalizeRegistryPath(suffix);
    return path.size() >= normalizedSuffix.size() &&
           !_wcsicmp(path.c_str() + path.size() - normalizedSuffix.size(),
                     normalizedSuffix.c_str());
}

static bool ShouldHideVirtualRegistryValue(const std::wstring& keyPath,
                                           PCWSTR valueName) {
    if (!g_virtualizeLegacyOpenWithRegistry.load(
            std::memory_order_acquire) || keyPath.empty() || !valueName) {
        return false;
    }

    if (!_wcsicmp(valueName, L"DelegateExecute")) {
        static const PCWSTR commands[] = {
            L"unknown\\shell\\openas\\command",
            L"unknown\\shell\\open\\command",
            L"unknown\\shell\\invokedefaultverbinotherprocess\\command",
            L"unknown\\shell\\openwithsetdefaulton\\command",
            L"undecided\\shell\\open\\command",
        };
        for (PCWSTR command : commands) {
            if (EndsWithRegistryPath(keyPath, command)) return true;
        }
    }

    return !_wcsicmp(valueName, L"OpenWithLauncher") &&
           EndsWithRegistryPath(
               keyPath,
               L"software\\microsoft\\windows\\currentversion\\openwith");
}

static void TrackVirtualRegistryKey(HKEY parentKey, PCWSTR subKey,
                                    HKEY openedKey) {
    if (!openedKey) return;
    std::wstring parent = TrackedRegistryPath(parentKey);
    if (parent.empty()) return;
    std::wstring full = parent;
    if (subKey && *subKey) {
        full.push_back(L'\\');
        full.append(subKey);
    }
    std::lock_guard<std::mutex> lock(g_virtualRegistryMutex);
    g_virtualRegistryPaths[openedKey] =
        NormalizeRegistryPath(full.c_str());
}

static LSTATUS WINAPI RegOpenKeyWHook(HKEY key, LPCWSTR subKey,
                                      PHKEY result) {
    try {
        const LSTATUS status = RegOpenKeyWOriginal
            ? RegOpenKeyWOriginal(key, subKey, result)
            : ERROR_PROC_NOT_FOUND;
        if (status == ERROR_SUCCESS && result)
            TrackVirtualRegistryKey(key, subKey, *result);
        return status;
    } catch (...) {
        Wh_Log(L"Standalone Open With: RegOpenKeyW virtualization exception");
        return RegOpenKeyWOriginal
            ? RegOpenKeyWOriginal(key, subKey, result)
            : ERROR_PROC_NOT_FOUND;
    }
}

static LSTATUS WINAPI RegOpenKeyExWHook(HKEY key, LPCWSTR subKey,
                                        DWORD options, REGSAM access,
                                        PHKEY result) {
    try {
        const LSTATUS status = RegOpenKeyExWOriginal
            ? RegOpenKeyExWOriginal(key, subKey, options, access, result)
            : ERROR_PROC_NOT_FOUND;
        if (status == ERROR_SUCCESS && result)
            TrackVirtualRegistryKey(key, subKey, *result);
        return status;
    } catch (...) {
        Wh_Log(L"Standalone Open With: RegOpenKeyExW virtualization exception");
        return RegOpenKeyExWOriginal
            ? RegOpenKeyExWOriginal(key, subKey, options, access, result)
            : ERROR_PROC_NOT_FOUND;
    }
}

static LSTATUS WINAPI RegQueryValueExWHook(HKEY key, LPCWSTR valueName,
                                           LPDWORD reserved, LPDWORD type,
                                           LPBYTE data, LPDWORD bytes) {
    try {
        const std::wstring path = TrackedRegistryPath(key);
        if (ShouldHideVirtualRegistryValue(path, valueName)) {
            Wh_Log(L"Standalone Open With: virtual registry hides %s\\%s",
                   path.c_str(), valueName);
            return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: RegQueryValueExW virtualization exception");
    }
    return RegQueryValueExWOriginal
        ? RegQueryValueExWOriginal(key, valueName, reserved, type, data, bytes)
        : ERROR_PROC_NOT_FOUND;
}

static LSTATUS WINAPI RegGetValueWHook(HKEY key, LPCWSTR subKey,
                                       LPCWSTR valueName, DWORD flags,
                                       LPDWORD type, PVOID data,
                                       LPDWORD bytes) {
    try {
        std::wstring path = TrackedRegistryPath(key);
        if (!path.empty() && subKey && *subKey) {
            path.push_back(L'\\');
            path.append(subKey);
            path = NormalizeRegistryPath(path.c_str());
        }
        if (ShouldHideVirtualRegistryValue(path, valueName)) {
            Wh_Log(L"Standalone Open With: virtual RegGetValue hides %s\\%s",
                   path.c_str(), valueName);
            return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
        Wh_Log(L"Standalone Open With: RegGetValueW virtualization exception");
    }
    return RegGetValueWOriginal
        ? RegGetValueWOriginal(key, subKey, valueName, flags, type, data, bytes)
        : ERROR_PROC_NOT_FOUND;
}

static LSTATUS WINAPI RegCloseKeyHook(HKEY key) {
    try {
        {
            std::lock_guard<std::mutex> lock(g_virtualRegistryMutex);
            g_virtualRegistryPaths.erase(key);
        }
    } catch (...) {
    }
    return RegCloseKeyOriginal ? RegCloseKeyOriginal(key)
                               : ERROR_PROC_NOT_FOUND;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle.
// -----------------------------------------------------------------------------

static void StopWorker() {
    g_shuttingDown.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent.get());
    if (HWND window = g_currentWindow.load(std::memory_order_acquire))
        PostMessageW(window, WM_CLOSE, 0, 0);
    if (g_worker) {
        g_worker->join();
        g_worker.reset();
    }
}

BOOL Wh_ModInit() {
    try {
        LoadSettings();
        g_shuttingDown.store(false, std::memory_order_release);
        g_stopEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
        g_requestEvent.reset(CreateEventW(nullptr, FALSE, FALSE, nullptr));
        g_workerReadyEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
        if (!g_stopEvent || !g_requestEvent || !g_workerReadyEvent)
            return FALSE;
        g_worker.emplace(WorkerMainNoexcept);

        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        auto openWith = shell32 ? reinterpret_cast<SHOpenWithDialog_t>(
                                      GetProcAddress(shell32, "SHOpenWithDialog"))
                                : nullptr;
        auto executeEx = shell32 ? reinterpret_cast<ShellExecuteExW_t>(
                                      GetProcAddress(shell32, "ShellExecuteExW"))
                                 : nullptr;
        auto execute = shell32 ? reinterpret_cast<ShellExecuteW_t>(
                                    GetProcAddress(shell32, "ShellExecuteW"))
                               : nullptr;
        wchar_t moduleName[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, moduleName, ARRAYSIZE(moduleName));
        const PCWSTR processFileName = PathFindFileNameW(moduleName);
        const bool isExplorer = processFileName &&
            !_wcsicmp(processFileName, L"explorer.exe");
        const bool isOpenWithServer = processFileName &&
            !_wcsicmp(processFileName, L"OpenWith.exe");
        Wh_Log(L"Standalone Open With: init in process=%s pid=%u shell32=%p "
               L"openWithAddr=%p executeExAddr=%p executeAddr=%p cmd=%s",
               moduleName, GetCurrentProcessId(), shell32,
               reinterpret_cast<void*>(openWith),
               reinterpret_cast<void*>(executeEx),
               reinterpret_cast<void*>(execute), GetCommandLineW());
        bool anyHook = false;
        bool hookedOpenWith = false, hookedExecuteEx = false, hookedExecute = false;
        if (openWith) hookedOpenWith = WindhawkUtils::SetFunctionHook(
            openWith, SHOpenWithDialogHook, &SHOpenWithDialogOriginal);
        if (executeEx) hookedExecuteEx = WindhawkUtils::SetFunctionHook(
            executeEx, ShellExecuteExWHook, &ShellExecuteExWOriginal);
        if (execute) hookedExecute = WindhawkUtils::SetFunctionHook(
            execute, ShellExecuteWHook, &ShellExecuteWOriginal);

        bool hookedTrackPopupMenuEx = WindhawkUtils::SetFunctionHook(
            TrackPopupMenuEx, TrackPopupMenuExHook,
            &TrackPopupMenuExOriginal);
        bool hookedTrackPopupMenu = WindhawkUtils::SetFunctionHook(
            TrackPopupMenu, TrackPopupMenuHook,
            &TrackPopupMenuOriginal);

        bool hookedRegistryVirtualization = false;
        if (isExplorer &&
            g_virtualizeLegacyOpenWithRegistry.load(
                std::memory_order_acquire)) {
            const bool openExHook = WindhawkUtils::SetFunctionHook(
                RegOpenKeyExW, RegOpenKeyExWHook, &RegOpenKeyExWOriginal);
            const bool openHook = WindhawkUtils::SetFunctionHook(
                RegOpenKeyW, RegOpenKeyWHook, &RegOpenKeyWOriginal);
            const bool queryHook = WindhawkUtils::SetFunctionHook(
                RegQueryValueExW, RegQueryValueExWHook,
                &RegQueryValueExWOriginal);
            const bool getHook = WindhawkUtils::SetFunctionHook(
                RegGetValueW, RegGetValueWHook, &RegGetValueWOriginal);
            const bool closeHook = WindhawkUtils::SetFunctionHook(
                RegCloseKey, RegCloseKeyHook, &RegCloseKeyOriginal);
            hookedRegistryVirtualization =
                openExHook && openHook && queryHook && getHook && closeHook;
        }

        bool hookedServerRegistration = false;
        bool hookedDirectOpenWithEntry = false;
        if (isOpenWithServer) {
            HMODULE combaseModule = GetModuleHandleW(L"combase.dll");
            auto registerClassObject = combaseModule
                ? reinterpret_cast<CoRegisterClassObject_t>(
                      GetProcAddress(combaseModule, "CoRegisterClassObject"))
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

            const std::wstring directPath =
                ExtractOpenWithTargetPath(GetCommandLineW());
            if (IsSupportedFile(directPath)) {
                hookedDirectOpenWithEntry =
                    InstallDirectOpenWithEntryHook(directPath.c_str());
            }
        }

        // Probe the stable CLSID_OpenWithMenu interfaces and hook their current
        // vtable targets. No symbols, offsets or private Windows interfaces are
        // used; the addresses are rediscovered on every Explorer start.
        const bool hookedOpenWithMenu = InstallOpenWithMenuMethodHooks();
        g_usePartialOpenWithMenuFactory.store(
            !hookedOpenWithMenu, std::memory_order_release);

        bool hookedOpenWithFactory = false;
        auto shellDllGetClassObject = shell32
            ? reinterpret_cast<DllGetClassObject_t>(
                  GetProcAddress(shell32, "DllGetClassObject"))
            : nullptr;
        if (shellDllGetClassObject) {
            hookedOpenWithFactory = WindhawkUtils::SetFunctionHook(
                shellDllGetClassObject, Shell32DllGetClassObjectHook,
                &Shell32DllGetClassObjectOriginal);
        }

        bool hookedComActivation = false;
        auto coCreateInstance = reinterpret_cast<CoCreateInstance_t>(
            GetProcAddress(GetModuleHandleW(L"combase.dll"),
                           "CoCreateInstance"));
        if (!coCreateInstance) {
            HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
            coCreateInstance = ole32
                ? reinterpret_cast<CoCreateInstance_t>(
                      GetProcAddress(ole32, "CoCreateInstance"))
                : nullptr;
        }
        if (coCreateInstance) {
            hookedComActivation = WindhawkUtils::SetFunctionHook(
                coCreateInstance, CoCreateInstanceHook,
                &CoCreateInstanceOriginal);
        }

        bool hookedComActivationEx = false;
        HMODULE combase = GetModuleHandleW(L"combase.dll");
        auto coCreateInstanceEx = combase
            ? reinterpret_cast<CoCreateInstanceEx_t>(
                  GetProcAddress(combase, "CoCreateInstanceEx"))
            : nullptr;
        if (!coCreateInstanceEx) {
            HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
            coCreateInstanceEx = ole32
                ? reinterpret_cast<CoCreateInstanceEx_t>(
                      GetProcAddress(ole32, "CoCreateInstanceEx"))
                : nullptr;
        }
        if (coCreateInstanceEx) {
            hookedComActivationEx = WindhawkUtils::SetFunctionHook(
                coCreateInstanceEx, CoCreateInstanceExHook,
                &CoCreateInstanceExOriginal);
        }

        anyHook = hookedOpenWith || hookedExecuteEx || hookedExecute ||
                  hookedTrackPopupMenuEx || hookedTrackPopupMenu ||
                  hookedRegistryVirtualization ||
                  hookedServerRegistration || hookedDirectOpenWithEntry ||
                  hookedOpenWithMenu || hookedOpenWithFactory ||
                  hookedComActivation || hookedComActivationEx;
        Wh_Log(L"Standalone Open With: hook results pid=%u openWith=%d "
               L"executeEx=%d execute=%d trackPopup=%d/%d regVirtual=%d "
               L"serverReg=%d directEntry=%d menuMethods=%d menuFactory=%d "
               L"partialFactory=%d comActivation=%d comActivationEx=%d",
               GetCurrentProcessId(), hookedOpenWith, hookedExecuteEx,
               hookedExecute, hookedTrackPopupMenuEx, hookedTrackPopupMenu,
               hookedRegistryVirtualization, hookedServerRegistration,
               hookedDirectOpenWithEntry,
               hookedOpenWithMenu, hookedOpenWithFactory,
               g_usePartialOpenWithMenuFactory.load(std::memory_order_acquire),
               hookedComActivation, hookedComActivationEx);

        // Supplementary legacy/direct-launch diagnostic. The normal modern
        // route is DCOM and is handled by the CoCreateInstance hook below;
        // Explorer usually does not call CreateProcessW for OpenWith.exe.
        try {
            wchar_t selfName[MAX_PATH] = {};
            GetModuleFileNameW(nullptr, selfName, ARRAYSIZE(selfName));
            if (isExplorer) {
                HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
                auto createProcessW =
                    kernel32 ? reinterpret_cast<CreateProcessW_t>(
                                   GetProcAddress(kernel32, "CreateProcessW"))
                             : nullptr;
                bool hookedCreateProcess = false;
                if (createProcessW) {
                    hookedCreateProcess = WindhawkUtils::SetFunctionHook(
                        createProcessW, CreateProcessWHook,
                        &CreateProcessWOriginal);
                }
                Wh_Log(L"Standalone Open With: CreateProcessW hook=%d",
                       hookedCreateProcess);
            }
        } catch (...) {
            Wh_Log(L"Standalone Open With: CreateProcessW hook install "
                   L"failed with exception");
        }

        if (!anyHook) { StopWorker(); return FALSE; }
        Wh_Log(L"Standalone Open With initialized with API/COM hooks");
        return TRUE;
    } catch (...) {
        StopWorker();
        return FALSE;
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
        std::lock_guard<std::mutex> lock(g_virtualRegistryMutex);
        g_virtualRegistryPaths.clear();
    }
    g_workerReadyEvent.reset();
    g_requestEvent.reset();
    g_stopEvent.reset();
}
