// ==WindhawkMod==
// @id              win7-classic-autoplay-restorer
// @name            Windows 7 Classic AutoPlay Dialog
// @description     This mod restores the classic Windows 7 AutoPlay dialog for removable drives and optical media
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -luuid -lshell32 -lshlwapi -lgdi32 -luser32 -lcomctl32 -lmsimg32 -lwindowscodecs -lversion -ladvapi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Classic AutoPlay Dialog

## About 

This mod restores the classic Windows 7 AutoPlay dialog when inserting a USB drive, a disc, or a phone.

What is seen is a simple list of real actions (open folder, play with Windows
Media Player, view pictures with Windows Photo Viewer, ReadyBoost). A program
from the disc is offered only if clicked and only if the file is on that
volume.

The "Always do this" choice is remembered by the mod, not by Windows policy.
A privacy setting can hide the volume or device name in this window only.



## Screenshot 

![autoplay.png](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/autoplay.png)

## Notes  

The mod has been tested on Windows 10 21H2.

While the mod is enabled it briefly writes two values under
`HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\CancelAutoplay\CLSID`
to suppress native AutoPlay. These are removed when the mod unloads and are
also cleaned up unconditionally every time the mod starts, so a crash or a
forced Explorer restart cannot leave them behind for more than one session.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $description: This setting chooses the language of the AutoPlay dialog. Automatic follows the Windows display language.
  $options:
    - auto: Automatic (system)
    - en: English
    - it: Italian
    - de: Deutsch
    - es: Español
    - fr: Français
    - pt: Português
    - tr: Türkçe
    - ru: Русский
    - zh-CN: 简体中文
    - zh-TW: 繁體中文
    - ja: 日本語
    - ko: 한국어
    - ar: العربية
    - nl: Nederlands
    - pl: Polski
    - sv: Svenska
    - da: Dansk
    - no: Norsk
    - fi: Suomi
    - el: Ελληνικά
- suppressNativeAutoPlay: true
  $name: Suppress native AutoPlay notification
  $description: This setting hides the Windows 10/11 AutoPlay toast so only this classic dialog is shown. It does not change Windows AutoPlay policy.
- includeReadyBoost: true
  $name: Offer ReadyBoost
  $description: This setting shows Speed up my system (ReadyBoost) when the removable drive is large enough and writable.
- includeFixedDrives: false
  $name: Handle external fixed drives
  $description: This setting also opens the dialog for external hard drives that Windows reports as fixed disks.
- includeMtpDevices: true
  $name: Handle MTP / WPD devices
  $description: This setting shows the dialog for phones, cameras and media players that do not get a drive letter.
- includeNetworkDrives: false
  $name: Handle mapped network drives
  $description: This setting also shows the dialog when a mapped network drive becomes available. It is off by default.
- includeRamDisks: false
  $name: Handle RAM disks
  $description: This setting also shows the dialog for RAM disks. It is off by default.
- hideDeviceNames: false
  $name: Privacy mode
  $description: This setting hides volume labels and device names in this dialog only and shows a generic name such as Removable Disk.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winioctl.h>
#include <windowsx.h>
#include <dbt.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <objbase.h>
#include <wincodec.h>
#include <uxtheme.h>
#include <vssym32.h>
#ifndef WICBitmapInterpolationModeHighQualityCubic
#define WICBitmapInterpolationModeHighQualityCubic ((WICBitmapInterpolationMode)0x4)
#endif
#include <winver.h>
#include <shobjidl.h>
#include <objidl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cwctype>
#include <memory>
#include <string>
#include <vector>

template <typename T>
class ApComPtr {
    T* p = nullptr;
public:
    ApComPtr() = default;
    explicit ApComPtr(T* q) : p(q) {}
    ~ApComPtr() { reset(); }
    ApComPtr(const ApComPtr&) = delete;
    ApComPtr& operator=(const ApComPtr&) = delete;
    void reset(T* q = nullptr) {
        if (p) p->Release();
        p = q;
    }
    T** put() { reset(); return &p; }
    T* get() const { return p; }
    T* operator->() const { return p; }
    T* release() { T* q = p; p = nullptr; return q; }  // relinquish ownership
    explicit operator bool() const { return p != nullptr; }
};

struct ApRegKey {
    HKEY h = nullptr;
    ~ApRegKey() { if (h) { RegCloseKey(h); h = nullptr; } }
    ApRegKey() = default;
    ApRegKey(const ApRegKey&) = delete;
};

struct ApScopedHandle {
    HANDLE h = nullptr;
    ApScopedHandle() = default;
    explicit ApScopedHandle(HANDLE value) : h(value) {}
    ~ApScopedHandle() { reset(); }
    ApScopedHandle(const ApScopedHandle&) = delete;
    ApScopedHandle& operator=(const ApScopedHandle&) = delete;
    ApScopedHandle(ApScopedHandle&& other) noexcept : h(other.h) { other.h = nullptr; }
    ApScopedHandle& operator=(ApScopedHandle&& other) noexcept {
        if (this != &other) { reset(); h = other.h; other.h = nullptr; }
        return *this;
    }
    void reset(HANDLE value = nullptr) {
        if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
        h = value;
    }
    HANDLE get() const { return h; }
    explicit operator bool() const { return h && h != INVALID_HANDLE_VALUE; }
};

struct ApScopedCoInit {
    HRESULT hr = E_FAIL;
    explicit ApScopedCoInit(DWORD model = COINIT_APARTMENTTHREADED) {
        hr = CoInitializeEx(nullptr, model);
    }
    ~ApScopedCoInit() {
        if (SUCCEEDED(hr)) CoUninitialize();
    }
    ApScopedCoInit(const ApScopedCoInit&) = delete;
    ApScopedCoInit& operator=(const ApScopedCoInit&) = delete;
    bool available() const { return SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE; }
};

struct ApCoTaskStrs {
    std::vector<LPWSTR> ids;
    ~ApCoTaskStrs() {
        for (LPWSTR s : ids)
            if (s) CoTaskMemFree(s);
    }
};

// RAII for GDI objects that must be released with DeleteObject (HBITMAP,
// HBRUSH, HPEN, HFONT, HGDIOBJ). Prevents leaks when an allocating call
// (e.g. std::vector::push_back -> bad_alloc) throws mid-way through a
// function that holds several such objects.
struct ApScopedGdiObj {
    HGDIOBJ h = nullptr;
    ApScopedGdiObj() = default;
    explicit ApScopedGdiObj(HGDIOBJ value) : h(value) {}
    ~ApScopedGdiObj() { reset(); }
    ApScopedGdiObj(const ApScopedGdiObj&) = delete;
    ApScopedGdiObj& operator=(const ApScopedGdiObj&) = delete;
    void reset(HGDIOBJ value = nullptr) {
        if (h && h != HGDI_ERROR) DeleteObject(h);
        h = value;
    }
    HGDIOBJ get() const { return h; }
    HGDIOBJ release() { HGDIOBJ o = h; h = nullptr; return o; }
    explicit operator bool() const { return h != nullptr; }
};

// RAII for a GDI device context obtained from CreateCompatibleDC / GetDC,
// released with DeleteDC / ReleaseDC.
struct ApScopedDc {
    HDC h = nullptr;
    explicit ApScopedDc(HDC value = nullptr) : h(value) {}
    ~ApScopedDc() { reset(); }
    ApScopedDc(const ApScopedDc&) = delete;
    ApScopedDc& operator=(const ApScopedDc&) = delete;
    void reset(HDC value = nullptr) {
        if (h) DeleteDC(h);
        h = value;
    }
    HDC get() const { return h; }
    HDC release() { HDC o = h; h = nullptr; return o; }
    explicit operator bool() const { return h != nullptr; }
};

// RAII lock for a CRITICAL_SECTION: always releases on scope exit, so an
// exception thrown while the section is held (e.g. std::vector::push_back ->
// bad_alloc) cannot leave it locked forever and deadlock the mod.
struct ApScopedCriticalSection {
    LPCRITICAL_SECTION cs = nullptr;
    explicit ApScopedCriticalSection(LPCRITICAL_SECTION p) : cs(p) {
        if (cs) EnterCriticalSection(cs);
    }
    ~ApScopedCriticalSection() {
        if (cs) LeaveCriticalSection(cs);
    }
    ApScopedCriticalSection(const ApScopedCriticalSection&) = delete;
    ApScopedCriticalSection& operator=(const ApScopedCriticalSection&) = delete;
};

LRESULT CALLBACK AutoPlayDialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ListenerWndProc(HWND, UINT, WPARAM, LPARAM);

#define WMU_SHUTDOWN      (WM_USER + 0x601) // postato da Wh_ModUninit al listener
#define WMU_REBUILD       (WM_USER + 0x602) // ricostruisci dialogo (impostazioni)
#define WMU_SELF_REBUILD  (WM_APP  + 0x033) // dialogo -> posta WMU_REBUILD
#define WMU_PROCESS_QUEUE (WM_USER + 0x603) // processa coda volumi in attesa
#define WMU_APPLY_SUPPRESS (WM_USER + 0x604) // applica/rimuove IQueryCancelAutoPlay
#define WMU_CONTEXT_AUTOPLAY (WM_USER + 0x605) // clic destro: Apri AutoPlay...
#define WMU_ACTION_DONE     (WM_USER + 0x606) // worker azione terminata
#define IDT_READY         1                 // timer attesa prontezza volume
#define IDC_ALWAYS        1005

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)


// ============================================================================
// ============================================================================
static UINT  g_dpi = 96;
static UINT  g_resDpi = 0;              // dpi per cui sono state create le risorse
static inline int Scale(int v) { return MulDiv(v, (int)g_dpi, 96); }

static HWND g_hwndDialog   = nullptr;
static HWND g_hwndCheck    = nullptr;
static HWND g_hwndListener = nullptr;
static int  g_lockWinW = 0;
static int  g_lockWinH = 0;
static HICON g_hwndIconBig = nullptr;   // icone impostate con WM_SETICON (owned)
static HICON g_hwndIconSmall = nullptr;
static HANDLE g_hUiThread  = nullptr;
static DWORD  g_dwUiThreadId = 0;
static HANDLE g_evtListenerReady = nullptr;
static IWICImagingFactory* g_wic = nullptr;

static HFONT g_fontTitle = nullptr;     // 11pt semibold (titolo header)
static HFONT g_fontText = nullptr;      // 9pt regolare (riga principale opzioni)
static HFONT g_fontBold = nullptr;      // 9pt bold (opzione programma / sezioni)
static HFONT g_fontSmall = nullptr;     // 8pt (descrizioni / sottotitolo)
static HFONT g_fontLink = nullptr;      // 9pt (link)
static HFONT g_fontLinkUnder = nullptr; // 9pt sottolineato (link hover)

static HBITMAP g_bmpFolder = nullptr;      // cartella gialla Win7  (32)
static HBITMAP g_bmpSetup = nullptr;       // ingranaggio programma (32)
static HBITMAP g_bmpReadyBoost = nullptr;  // fulmine ReadyBoost    (32)
static HBITMAP g_bmpDrive48 = nullptr;     // disco rimovibile      (48, fallback header)
static HBITMAP g_bmpDisc48 = nullptr;      // disco ottico          (48, fallback header)
static HICON   g_hicoDrive48 = nullptr;    // header rimovibile (DrawIconEx)
static HICON   g_hicoDisc48 = nullptr;     // header ottico
static HBITMAP g_bmpLocal48 = nullptr;     // disco locale/fisso    (48, header)
static HICON   g_hicoLocal48 = nullptr;    // header disco locale (DrawIconEx)
static HBITMAP g_bmpPhone48 = nullptr;     // telefono MTP/WPD      (48)
static HICON   g_hicoPhone48 = nullptr;
static HBITMAP g_bmpPlay = nullptr;        // fallback Play         (32)
static HBITMAP g_bmpAutoPlay16 = nullptr;  // icona AutoPlay classica (16)

// ============================================================================
// ============================================================================
struct ModSettings {
    WORD langPrimary = 0;       // 0 = auto (PRIMARYLANGID)
    WORD langSub = 0;           // 0 = any sublang
    BOOL suppressNativeAutoPlay = TRUE;
    BOOL includeReadyBoost = TRUE;
    BOOL includeFixedDrives = FALSE;
    BOOL includeMtpDevices = TRUE;
    BOOL includeNetworkDrives = FALSE;
    BOOL includeRamDisks = FALSE;
    BOOL hideDeviceNames = FALSE;
} g_settings;

static void LoadSettings() {
    WindhawkUtils::StringSetting lang = WindhawkUtils::StringSetting::make(L"language");
    g_settings.langPrimary = 0;
    g_settings.langSub = 0;
    const wchar_t* s = lang.get();
    struct Map { const wchar_t* code; WORD pri; WORD sub; };
    static const Map kMap[] = {
        { L"en", 0x09, 0 }, { L"it", 0x10, 0 }, { L"de", 0x07, 0 },
        { L"es", 0x0A, 0 }, { L"fr", 0x0C, 0 }, { L"pt", 0x16, 0 },
        { L"tr", 0x1F, 0 }, { L"ru", 0x19, 0 },
        { L"zh-CN", 0x04, SUBLANG_CHINESE_SIMPLIFIED },
        { L"zh-TW", 0x04, SUBLANG_CHINESE_TRADITIONAL },
        { L"ja", 0x11, 0 }, { L"ko", 0x12, 0 }, { L"ar", 0x01, 0 },
        { L"nl", 0x13, 0 }, { L"pl", 0x15, 0 }, { L"sv", 0x1D, 0 },
        { L"da", 0x06, 0 }, { L"no", 0x14, 0 }, { L"fi", 0x0B, 0 },
        { L"el", 0x08, 0 },
    };
    if (s && s[0] && _wcsicmp(s, L"auto") != 0) {
        for (const auto& m : kMap) {
            if (_wcsicmp(s, m.code) == 0) {
                g_settings.langPrimary = m.pri;
                g_settings.langSub = m.sub;
                break;
            }
        }
    }
    g_settings.suppressNativeAutoPlay = Wh_GetIntSetting(L"suppressNativeAutoPlay") != 0;
    g_settings.includeReadyBoost      = Wh_GetIntSetting(L"includeReadyBoost") != 0;
    g_settings.includeFixedDrives     = Wh_GetIntSetting(L"includeFixedDrives") != 0;
    g_settings.includeMtpDevices      = Wh_GetIntSetting(L"includeMtpDevices") != 0;
    g_settings.includeNetworkDrives   = Wh_GetIntSetting(L"includeNetworkDrives") != 0;
    g_settings.includeRamDisks        = Wh_GetIntSetting(L"includeRamDisks") != 0;
    g_settings.hideDeviceNames        = Wh_GetIntSetting(L"hideDeviceNames") != 0;
}

// ============================================================================
// ============================================================================
struct LangPack {
    WORD lang;
    WORD subLang;
    const wchar_t *chooseAction;
    const wchar_t *chooseDisk;
    const wchar_t *chooseShort;
    const wchar_t *installRun;
    const wchar_t *publishedBy;
    const wchar_t *publisherUnknown;
    const wchar_t *generalOptions;
    const wchar_t *picturesOptions;
    const wchar_t *musicOptions;
    const wchar_t *mixedOptions;
    const wchar_t *openFolder;
    const wchar_t *usingExplorer;
    const wchar_t *speedUp;
    const wchar_t *usingReadyBoost;
    const wchar_t *playAudioCd;
    const wchar_t *playMedia;
    const wchar_t *usingPlayer;
    const wchar_t *viewPictures;
    const wchar_t *usingWindows;
    const wchar_t *runFile;
    const wchar_t *viewMore;
    const wchar_t *alwaysUsb;
    const wchar_t *alwaysSoftware;
    const wchar_t *alwaysCd;
    const wchar_t *alwaysPictures;
    const wchar_t *alwaysMusic;
    const wchar_t *alwaysGeneral;
    const wchar_t *alwaysMixed;
    const wchar_t *removableDisk;
    const wchar_t *cdDrive;
    const wchar_t *localDisk;
    const wchar_t *audioCd;
    const wchar_t *portableDevice;
    const wchar_t *windowTitle;
};
static const LangPack kLangPacks[] = {
    { 0x09, 0,
      L"Choose what to do with this device.",
      L"Choose what to do with this disk.",
      L"Choose what to do.",
      L"Install or run program from your media",
      L"Published by %s",
      L"Publisher not specified",
      L"General options",
      L"Pictures options",
      L"Music options",
      L"Mixed content options",
      L"Open folder to view files",
      L"using Windows Explorer",
      L"Speed up my system",
      L"using Windows ReadyBoost",
      L"Play audio CD",
      L"Play",
      L"using Windows Media Player",
      L"View pictures",
      L"using Windows Photo Viewer",
      L"Run %s",
      L"View more AutoPlay options in Control Panel",
      L"Always do this for USB devices:",
      L"Always do this for software and games:",
      L"Always do this for discs:",
      L"Always do this for pictures:",
      L"Always do this for music:",
      L"Always do this for general options:",
      L"Always do this for mixed content:",
      L"Removable Disk", L"CD Drive", L"Local Disk", L"Audio CD",
      L"Portable Device",
      L"AutoPlay" },
    { 0x10, 0,
      L"Scegli l'operazione da eseguire per questo dispositivo.",
      L"Scegli l'operazione da eseguire per questo disco.",
      L"Scegli l'operazione da eseguire.",
      L"Installa o esegui programma dal supporto inserito",
      L"Pubblicato da %s",
      L"Editore non specificato",
      L"Opzioni generali",
      L"Opzioni immagini",
      L"Opzioni musica",
      L"Opzioni contenuti misti",
      L"Apri cartella per visualizzare i file",
      L"utilizzando Esplora risorse",
      L"Velocizza il sistema",
      L"utilizzando Windows ReadyBoost",
      L"Riproduci CD audio",
      L"Riproduci",
      L"utilizzando Windows Media Player",
      L"Visualizza immagini",
      L"utilizzando Visualizzatore foto di Windows",
      L"Esegui %s",
      L"Visualizza altre opzioni AutoPlay nel Pannello di controllo",
      L"Esegui sempre questa operazione per dispositivi USB:",
      L"Esegui sempre questa operazione per software e giochi:",
      L"Esegui sempre questa operazione per i dischi:",
      L"Esegui sempre questa operazione per le immagini:",
      L"Esegui sempre questa operazione per la musica:",
      L"Esegui sempre questa operazione per le opzioni generali:",
      L"Esegui sempre questa operazione per i contenuti misti:",
      L"Disco rimovibile", L"Unità CD", L"Disco locale", L"CD audio",
      L"Dispositivo portatile",
      L"Riproduzione automatica" },
    { 0x07, 0,
      L"Wählen Sie, was mit diesem Gerät geschehen soll.",
      L"Wählen Sie, was mit dieser Disk geschehen soll.",
      L"Wählen Sie, was geschehen soll.",
      L"Programm von Ihrem Datenträger installieren oder ausführen",
      L"Veröffentlicht von %s",
      L"Herausgeber nicht angegeben",
      L"Allgemeine Optionen",
      L"Bildoptionen",
      L"Musikoptionen",
      L"Optionen für gemischte Inhalte",
      L"Ordner öffnen, um Dateien anzuzeigen",
      L"mit Windows Explorer",
      L"System beschleunigen",
      L"mit Windows ReadyBoost",
      L"Audio-CD abspielen",
      L"Abspielen",
      L"mit Windows Media Player",
      L"Bilder anzeigen",
      L"mit Windows-Fotoanzeige",
      L"%s ausführen",
      L"Weitere AutoPlay-Optionen in der Systemsteuerung anzeigen",
      L"Immer für USB-Geräte ausführen:",
      L"Immer für Software und Spiele ausführen:",
      L"Immer für Datenträger ausführen:",
      L"Immer für Bilder ausführen:",
      L"Immer für Musik ausführen:",
      L"Immer für allgemeine Optionen ausführen:",
      L"Immer für gemischte Inhalte ausführen:",
      L"Wechseldatenträger",
      L"CD-Laufwerk", L"Lokaler Datenträger", L"Audio-CD", L"Tragbares Gerät",
      L"AutoPlay" },
    { 0x0A, 0,
      L"Elija qué hacer con este dispositivo.",
      L"Elija qué hacer con este disco.",
      L"Elija qué hacer.",
      L"Instalar o ejecutar programa desde su medio",
      L"Publicado por %s",
      L"Editor no especificado",
      L"Opciones generales",
      L"Opciones de imágenes",
      L"Opciones de música",
      L"Opciones de contenido mixto",
      L"Abrir carpeta para ver archivos",
      L"con el Explorador de Windows",
      L"Acelerar mi sistema",
      L"con Windows ReadyBoost",
      L"Reproducir CD de audio",
      L"Reproducir",
      L"con Windows Media Player",
      L"Ver imágenes",
      L"con el Visor de fotos de Windows",
      L"Ejecutar %s",
      L"Ver más opciones de AutoPlay en el Panel de control",
      L"Siempre hacer esto para dispositivos USB:",
      L"Siempre hacer esto para software y juegos:",
      L"Siempre hacer esto para discos:",
      L"Siempre hacer esto para imágenes:",
      L"Siempre hacer esto para música:",
      L"Siempre hacer esto para opciones generales:",
      L"Siempre hacer esto para contenido mixto:",
      L"Disco extraíble",
      L"Unidad de CD", L"Disco local", L"CD de audio", L"Dispositivo portátil",
      L"Reproducción automática" },
    { 0x0C, 0,
      L"Choisissez l'action à effectuer pour ce périphérique.",
      L"Choisissez l'action à effectuer pour ce disque.",
      L"Choisissez l'action à effectuer.",
      L"Installer ou exécuter un programme à partir de votre support",
      L"Publié par %s",
      L"Éditeur non spécifié",
      L"Options générales",
      L"Options d'images",
      L"Options musicales",
      L"Options de contenu mixte",
      L"Ouvrir le dossier pour afficher les fichiers",
      L"avec l'Explorateur Windows",
      L"Accélérer mon système",
      L"avec Windows ReadyBoost",
      L"Lire le CD audio",
      L"Lire",
      L"avec Windows Media Player",
      L"Afficher les images",
      L"avec la Visionneuse de photos Windows",
      L"Exécuter %s",
      L"Afficher plus d'options AutoPlay dans le Panneau de configuration",
      L"Toujours effectuer cette action pour les périphériques USB :",
      L"Toujours effectuer cette action pour les logiciels et jeux :",
      L"Toujours effectuer cette action pour les disques :",
      L"Toujours effectuer cette action pour les images :",
      L"Toujours effectuer cette action pour la musique :",
      L"Toujours effectuer cette action pour les options générales :",
      L"Toujours effectuer cette action pour le contenu mixte :",
      L"Disque amovible",
      L"Lecteur CD", L"Disque local", L"CD audio", L"Périphérique portable",
      L"Lecture automatique" },
    { 0x16, 0,
      L"Escolha o que fazer com este dispositivo.",
      L"Escolha o que fazer com este disco.",
      L"Escolha o que fazer.",
      L"Instalar ou executar programa a partir do seu meio",
      L"Publicado por %s",
      L"Editor não especificado",
      L"Opções gerais",
      L"Opções de imagens",
      L"Opções de música",
      L"Opções de conteúdo misto",
      L"Abrir pasta para ver ficheiros",
      L"com o Explorador do Windows",
      L"Acelerar o meu sistema",
      L"com o Windows ReadyBoost",
      L"Reproduzir CD de áudio",
      L"Reproduzir",
      L"com o Windows Media Player",
      L"Ver imagens",
      L"com o Visualizador de fotografias do Windows",
      L"Executar %s",
      L"Ver mais opções do AutoPlay no Painel de Controlo",
      L"Fazer sempre isto para dispositivos USB:",
      L"Fazer sempre isto para software e jogos:",
      L"Fazer sempre isto para discos:",
      L"Fazer sempre isto para imagens:",
      L"Fazer sempre isto para música:",
      L"Fazer sempre isto para opções gerais:",
      L"Fazer sempre isto para conteúdo misto:",
      L"Disco removível",
      L"Unidade de CD", L"Disco local", L"CD de áudio", L"Dispositivo portátil",
      L"Reprodução automática" },
    { 0x1F, 0,
      L"Bu aygıtla ne yapılacağını seçin.",
      L"Bu diskle ne yapılacağını seçin.",
      L"Ne yapılacağını seçin.",
      L"Ortamınızdan program yükleyin veya çalıştırın",
      L"%s tarafından yayınlandı",
      L"Yayıncı belirtilmemiş",
      L"Genel seçenekler",
      L"Resim seçenekleri",
      L"Müzik seçenekleri",
      L"Karma içerik seçenekleri",
      L"Dosyaları görüntülemek için klasörü aç",
      L"Windows Gezgini ile",
      L"Sistemimi hızlandır",
      L"Windows ReadyBoost ile",
      L"Ses CD'sini oynat",
      L"Oynat",
      L"Windows Media Player ile",
      L"Resimleri görüntüle",
      L"Windows Fotoğraf Görüntüleyicisi ile",
      L"%s çalıştır",
      L"Denetim Masası'nda daha fazla AutoPlay seçeneğini görüntüle",
      L"USB aygıtları için her zaman bunu yap:",
      L"Yazılım ve oyunlar için her zaman bunu yap:",
      L"Diskler için her zaman bunu yap:",
      L"Resimler için her zaman bunu yap:",
      L"Müzik için her zaman bunu yap:",
      L"Genel seçenekler için her zaman bunu yap:",
      L"Karma içerik için her zaman bunu yap:",
      L"Çıkarılabilir Disk",
      L"CD Sürücüsü", L"Yerel Disk", L"Ses CD'si", L"Taşınabilir Aygıt",
      L"Otomatik Oynat" },
    { 0x19, 0,
      L"Выберите действие для этого устройства.",
      L"Выберите действие для этого диска.",
      L"Выберите действие.",
      L"Установить или запустить программу с носителя",
      L"Опубликовано %s",
      L"Издатель не указан",
      L"Общие параметры",
      L"Параметры изображений",
      L"Параметры музыки",
      L"Параметры смешанного содержимого",
      L"Открыть папку для просмотра файлов",
      L"с помощью Проводника Windows",
      L"Ускорить работу системы",
      L"с помощью Windows ReadyBoost",
      L"Воспроизвести аудио-CD",
      L"Воспроизвести",
      L"с помощью Windows Media Player",
      L"Просмотреть изображения",
      L"с помощью Просмотрщика фотографий Windows",
      L"Запустить %s",
      L"Дополнительные параметры автозапуска в Панели управления",
      L"Всегда выполнять это для USB-устройств:",
      L"Всегда выполнять это для программ и игр:",
      L"Всегда выполнять это для дисков:",
      L"Всегда выполнять это для изображений:",
      L"Всегда выполнять это для музыки:",
      L"Всегда выполнять это для общих параметров:",
      L"Всегда выполнять это для смешанного содержимого:",
      L"Съемный диск",
      L"CD-привод", L"Локальный диск", L"Аудио-CD", L"Переносное устройство",
      L"Автозапуск" },
    { 0x04, 2,
      L"选择对此设备执行的操作。",
      L"选择对此磁盘执行的操作。",
      L"选择要执行的操作。",
      L"从媒体安装或运行程序",
      L"由 %s 发布",
      L"未指定发布者",
      L"常规选项",
      L"图片选项",
      L"音乐选项",
      L"混合内容选项",
      L"打开文件夹以查看文件",
      L"使用 Windows 资源管理器",
      L"加速我的系统",
      L"使用 Windows ReadyBoost",
      L"播放音频 CD",
      L"播放",
      L"使用 Windows Media Player",
      L"查看图片",
      L"使用 Windows 照片查看器",
      L"运行 %s",
      L"在控制面板中查看更多自动播放选项",
      L"对 USB 设备始终执行此操作：",
      L"对软件和游戏始终执行此操作：",
      L"对光盘始终执行此操作：",
      L"对图片始终执行此操作：",
      L"对音乐始终执行此操作：",
      L"对常规选项始终执行此操作：",
      L"对混合内容始终执行此操作：",
      L"可移动磁盘",
      L"CD 驱动器", L"本地磁盘", L"音频 CD", L"便携设备",
      L"自动播放" },
    { 0x04, 1,
      L"選擇對這個裝置執行的動作。",
      L"選擇對這個磁碟執行的動作。",
      L"選擇要執行的動作。",
      L"從媒體安裝或執行程式",
      L"由 %s 發行",
      L"未指定發行者",
      L"一般選項",
      L"圖片選項",
      L"音樂選項",
      L"混合內容選項",
      L"開啟資料夾以檢視檔案",
      L"使用 Windows 檔案總管",
      L"加速我的系統",
      L"使用 Windows ReadyBoost",
      L"播放音訊 CD",
      L"播放",
      L"使用 Windows Media Player",
      L"檢視圖片",
      L"使用 Windows 相片檢視器",
      L"執行 %s",
      L"在控制台中檢視更多自動播放選項",
      L"對 USB 裝置永遠執行此動作：",
      L"對軟體和遊戲永遠執行此動作：",
      L"對光碟永遠執行此動作：",
      L"對圖片永遠執行此動作：",
      L"對音樂永遠執行此動作：",
      L"對一般選項永遠執行此動作：",
      L"對混合內容永遠執行此動作：",
      L"抽取式磁碟",
      L"CD 光碟機", L"本機磁碟", L"音訊 CD", L"可攜式裝置",
      L"自動播放" },
    { 0x11, 0,
      L"このデバイスで実行するアクションを選択してください。",
      L"このディスクで実行するアクションを選択してください。",
      L"実行するアクションを選択してください。",
      L"メディアからプログラムをインストールまたは実行する",
      L"%s によって発行されました",
      L"発行者が指定されていません",
      L"一般オプション",
      L"画像オプション",
      L"音楽オプション",
      L"混合コンテンツオプション",
      L"ファイルを表示するためにフォルダーを開く",
      L"Windows エクスプローラーを使用",
      L"システムを高速化する",
      L"Windows ReadyBoost を使用",
      L"オーディオ CD を再生する",
      L"再生",
      L"Windows Media Player を使用",
      L"画像を表示する",
      L"Windows フォト ビューアーを使用",
      L"%s を実行する",
      L"コントロール パネルでその他の自動再生オプションを表示する",
      L"USB デバイスに対して常にこれを行う:",
      L"ソフトウェアとゲームに対して常にこれを行う:",
      L"ディスクに対して常にこれを行う:",
      L"画像に対して常にこれを行う:",
      L"音楽に対して常にこれを行う:",
      L"一般オプションに対して常にこれを行う:",
      L"混合コンテンツに対して常にこれを行う:",
      L"リムーバブル ディスク",
      L"CD ドライブ", L"ローカル ディスク", L"オーディオ CD", L"ポータブル デバイス",
      L"自動再生" },
    { 0x12, 0,
      L"이 디바이스에서 수행할 작업을 선택하세요.",
      L"이 디스크에서 수행할 작업을 선택하세요.",
      L"수행할 작업을 선택하세요.",
      L"미디어에서 프로그램 설치 또는 실행",
      L"%s에 의해 게시됨",
      L"게시자가 지정되지 않음",
      L"일반 옵션",
      L"이미지 옵션",
      L"음악 옵션",
      L"혼합 콘텐츠 옵션",
      L"파일을 보기 위해 폴더 열기",
      L"Windows 탐색기 사용",
      L"시스템 속도 향상",
      L"Windows ReadyBoost 사용",
      L"오디오 CD 재생",
      L"재생",
      L"Windows Media Player 사용",
      L"이미지 보기",
      L"Windows 사진 뷰어 사용",
      L"%s 실행",
      L"제어판에서 추가 자동 실행 옵션 보기",
      L"USB 디바이스에 대해 항상 이 작업 수행:",
      L"소프트웨어 및 게임에 대해 항상 이 작업 수행:",
      L"디스크에 대해 항상 이 작업 수행:",
      L"이미지에 대해 항상 이 작업 수행:",
      L"음악에 대해 항상 이 작업 수행:",
      L"일반 옵션에 대해 항상 이 작업 수행:",
      L"혼합 콘텐츠에 대해 항상 이 작업 수행:",
      L"이동식 디스크",
      L"CD 드라이브", L"로컬 디스크", L"오디오 CD", L"휴대용 디바이스",
      L"자동 실행" },
    { 0x01, 0,
      L"اختر ما تريد فعله بهذا الجهاز.",
      L"اختر ما تريد فعله بهذا القرص.",
      L"اختر ما تريد فعله.",
      L"تثبيت أو تشغيل برنامج من الوسائط الخاصة بك",
      L"نشر بواسطة %s",
      L"لم يتم تحديد الناشر",
      L"خيارات عامة",
      L"خيارات الصور",
      L"خيارات الموسيقى",
      L"خيارات المحتوى المختلط",
      L"فتح المجلد لعرض الملفات",
      L"باستخدام مستكشف Windows",
      L"تسريع نظامي",
      L"باستخدام Windows ReadyBoost",
      L"تشغيل قرص مضغوط صوتي",
      L"تشغيل",
      L"باستخدام Windows Media Player",
      L"عرض الصور",
      L"باستخدام عارض صور Windows",
      L"تشغيل %s",
      L"عرض المزيد من خيارات التشغيل التلقائي في لوحة التحكم",
      L"قم دائماً بهذا لأجهزة USB:",
      L"قم دائماً بهذا للبرامج والألعاب:",
      L"قم دائماً بهذا للأقراص:",
      L"قم دائماً بهذا للصور:",
      L"قم دائماً بهذا للموسيقى:",
      L"قم دائماً بهذا للخيارات العامة:",
      L"قم دائماً بهذا للمحتوى المختلط:",
      L"قرص قابل للإزالة",
      L"محرك أقراص CD", L"قرص محلي", L"قرص مضغوط صوتي", L"جهاز محمول",
      L"التشغيل التلقائي" },
    { 0x13, 0,
      L"Kies wat u met dit apparaat wilt doen.",
      L"Kies wat u met deze schijf wilt doen.",
      L"Kies wat u wilt doen.",
      L"Programma van uw media installeren of uitvoeren",
      L"Gepubliceerd door %s",
      L"Uitgever niet opgegeven",
      L"Algemene opties",
      L"Afbeeldingsopties",
      L"Muziekopties",
      L"Opties voor gemengde inhoud",
      L"Map openen om bestanden te bekijken",
      L"met Windows Verkenner",
      L"Mijn systeem versnellen",
      L"met Windows ReadyBoost",
      L"Audio-cd afspelen",
      L"Afspelen",
      L"met Windows Media Player",
      L"Afbeeldingen bekijken",
      L"met Windows Foto-viewer",
      L"%s uitvoeren",
      L"Meer AutoPlay-opties bekijken in het Configuratiescherm",
      L"Dit altijd doen voor USB-apparaten:",
      L"Dit altijd doen voor software en games:",
      L"Dit altijd doen voor schijven:",
      L"Dit altijd doen voor afbeeldingen:",
      L"Dit altijd doen voor muziek:",
      L"Dit altijd doen voor algemene opties:",
      L"Dit altijd doen voor gemengde inhoud:",
      L"Verwisselbare schijf",
      L"CD-station", L"Lokale schijf", L"Audio-cd", L"Draagbaar apparaat",
      L"Automatisch afspelen" },
    { 0x15, 0,
      L"Wybierz, co zrobić z tym urządzeniem.",
      L"Wybierz, co zrobić z tym dyskiem.",
      L"Wybierz, co zrobić.",
      L"Zainstaluj lub uruchom program z nośnika",
      L"Opublikowane przez %s",
      L"Nie określono wydawcy",
      L"Opcje ogólne",
      L"Opcje obrazów",
      L"Opcje muzyki",
      L"Opcje zawartości mieszanej",
      L"Otwórz folder, aby wyświetlić pliki",
      L"za pomocą Eksploratora Windows",
      L"Przyspiesz mój system",
      L"za pomocą Windows ReadyBoost",
      L"Odtwórz płytę CD audio",
      L"Odtwórz",
      L"za pomocą Windows Media Player",
      L"Wyświetl obrazy",
      L"za pomocą Przeglądarki zdjęć systemu Windows",
      L"Uruchom %s",
      L"Wyświetl więcej opcji automatycznego odtwarzania w Panelu sterowania",
      L"Zawsze wykonuj to dla urządzeń USB:",
      L"Zawsze wykonuj to dla oprogramowania i gier:",
      L"Zawsze wykonuj to dla dysków:",
      L"Zawsze wykonuj to dla obrazów:",
      L"Zawsze wykonuj to dla muzyki:",
      L"Zawsze wykonuj to dla opcji ogólnych:",
      L"Zawsze wykonuj to dla zawartości mieszanej:",
      L"Dysk wymienny",
      L"Napęd CD", L"Dysk lokalny", L"Płyta CD audio", L"Urządzenie przenośne",
      L"Autoodtwarzanie" },
    { 0x1D, 0,
      L"Välj vad du vill göra med den här enheten.",
      L"Välj vad du vill göra med den här skivan.",
      L"Välj vad du vill göra.",
      L"Installera eller kör program från dina media",
      L"Publicerad av %s",
      L"Utgivare ej angiven",
      L"Allmänna alternativ",
      L"Bildalternativ",
      L"Musikalternativ",
      L"Alternativ för blandat innehåll",
      L"Öppna mappen för att visa filer",
      L"med Windows Utforskaren",
      L"Snabba upp mitt system",
      L"med Windows ReadyBoost",
      L"Spela upp ljud-CD",
      L"Spela upp",
      L"med Windows Media Player",
      L"Visa bilder",
      L"med Windows Fotovisare",
      L"Kör %s",
      L"Visa fler AutoPlay-alternativ i Kontrollpanelen",
      L"Gör alltid detta för USB-enheter:",
      L"Gör alltid detta för programvara och spel:",
      L"Gör alltid detta för skivor:",
      L"Gör alltid detta för bilder:",
      L"Gör alltid detta för musik:",
      L"Gör alltid detta för allmänna alternativ:",
      L"Gör alltid detta för blandat innehåll:",
      L"Flyttbar disk",
      L"CD-enhet", L"Lokal disk", L"Ljud-CD", L"Bärbar enhet",
      L"Automatisk uppspelning" },
    { 0x06, 0,
      L"Vælg, hvad der skal gøres med denne enhed.",
      L"Vælg, hvad der skal gøres med denne disk.",
      L"Vælg, hvad der skal gøres.",
      L"Installer eller kør program fra dit medie",
      L"Udgivet af %s",
      L"Udgiver ikke angivet",
      L"Generelle indstillinger",
      L"Billedindstillinger",
      L"Musikindstillinger",
      L"Indstillinger for blandet indhold",
      L"Åbn mappe for at se filer",
      L"med Windows Stifinder",
      L"Gør mit system hurtigere",
      L"med Windows ReadyBoost",
      L"Afspil lyd-CD",
      L"Afspil",
      L"med Windows Media Player",
      L"Se billeder",
      L"med Windows Fotofremviser",
      L"Kør %s",
      L"Se flere AutoPlay-indstillinger i Kontrolpanel",
      L"Gør altid dette for USB-enheder:",
      L"Gør altid dette for software og spil:",
      L"Gør altid dette for diske:",
      L"Gør altid dette for billeder:",
      L"Gør altid dette for musik:",
      L"Gør altid dette for generelle indstillinger:",
      L"Gør altid dette for blandet indhold:",
      L"Flytbar disk",
      L"CD-drev", L"Lokal disk", L"Lyd-CD", L"Bærbar enhed",
      L"AutoPlay" },
    { 0x14, 0,
      L"Velg hva du vil gjøre med denne enheten.",
      L"Velg hva du vil gjøre med denne disken.",
      L"Velg hva du vil gjøre.",
      L"Installer eller kjør program fra mediet",
      L"Publisert av %s",
      L"Utgiver ikke spesifisert",
      L"Generelle alternativer",
      L"Bildealternativer",
      L"Musikkalternativer",
      L"Alternativer for blandet innhold",
      L"Åpne mappen for å vise filer",
      L"med Windows Utforsker",
      L"Gjør systemet raskere",
      L"med Windows ReadyBoost",
      L"Spill av lyd-CD",
      L"Spill av",
      L"med Windows Media Player",
      L"Vis bilder",
      L"med Windows Bildevisning",
      L"Kjør %s",
      L"Vis flere AutoPlay-alternativer i Kontrollpanelet",
      L"Gjør alltid dette for USB-enheter:",
      L"Gjør alltid dette for programvare og spill:",
      L"Gjør alltid dette for disker:",
      L"Gjør alltid dette for bilder:",
      L"Gjør alltid dette for musikk:",
      L"Gjør alltid dette for generelle alternativer:",
      L"Gjør alltid dette for blandet innhold:",
      L"Flyttbar disk",
      L"CD-stasjon", L"Lokal disk", L"Lyd-CD", L"Bærbar enhet",
      L"AutoPlay" },
    { 0x0B, 0,
      L"Valitse, mitä tälle laitteelle tehdään.",
      L"Valitse, mitä tälle levylle tehdään.",
      L"Valitse, mitä tehdään.",
      L"Asenna tai suorita ohjelma tietovälineeltä",
      L"Julkaisija: %s",
      L"Julkaisijaa ei määritetty",
      L"Yleiset asetukset",
      L"Kuvien asetukset",
      L"Musiikin asetukset",
      L"Sekasisällön asetukset",
      L"Avaa kansio tiedostojen tarkastelemiseksi",
      L"käyttäen Windows Resurssienhallintaa",
      L"Nopeuta järjestelmääni",
      L"käyttäen Windows ReadyBoostia",
      L"Toista ääni-CD",
      L"Toista",
      L"käyttäen Windows Media Playeria",
      L"Näytä kuvat",
      L"käyttäen Windowsin valokuvien katselinta",
      L"Suorita %s",
      L"Näytä lisää Automaattisen toiston asetuksia Ohjauspaneelissa",
      L"Tee aina näin USB-laitteille:",
      L"Tee aina näin ohjelmistoille ja peleille:",
      L"Tee aina näin levyille:",
      L"Tee aina näin kuville:",
      L"Tee aina näin musiikille:",
      L"Tee aina näin yleisille asetuksille:",
      L"Tee aina näin sekasisällölle:",
      L"Siirrettävä levy",
      L"CD-asema", L"Paikallinen levy", L"Ääni-CD", L"Kannettava laite",
      L"Automaattinen toisto" },
    { 0x08, 0,
      L"Επιλέξτε τι θα κάνετε με αυτήν τη συσκευή.",
      L"Επιλέξτε τι θα κάνετε με αυτόν το δίσκο.",
      L"Επιλέξτε τι θα κάνετε.",
      L"Εγκατάσταση ή εκτέλεση προγράμματος από τα μέσα σας",
      L"Δημοσιεύθηκε από %s",
      L"Ο εκδότης δεν προσδιορίστηκε",
      L"Γενικές επιλογές",
      L"Επιλογές εικόνων",
      L"Επιλογές μουσικής",
      L"Επιλογές μικτού περιεχομένου",
      L"Άνοιγμα φακέλου για προβολή αρχείων",
      L"με την Εξερεύνηση των Windows",
      L"Επιτάχυνση του συστήματός μου",
      L"με το Windows ReadyBoost",
      L"Αναπαραγωγή CD ήχου",
      L"Αναπαραγωγή",
      L"με το Windows Media Player",
      L"Προβολή εικόνων",
      L"με την Προβολή φωτογραφιών των Windows",
      L"Εκτέλεση %s",
      L"Προβολή περισσότερων επιλογών Αυτόματης αναπαραγωγής στον Πίνακα Ελέγχου",
      L"Να γίνεται πάντα αυτό για συσκευές USB:",
      L"Να γίνεται πάντα αυτό για λογισμικό και παιχνίδια:",
      L"Να γίνεται πάντα αυτό για δίσκους:",
      L"Να γίνεται πάντα αυτό για εικόνες:",
      L"Να γίνεται πάντα αυτό για μουσική:",
      L"Να γίνεται πάντα αυτό για γενικές επιλογές:",
      L"Να γίνεται πάντα αυτό για μικτό περιεχόμενο:",
      L"Αφαιρούμενος δίσκος",
      L"Μονάδα CD", L"Τοπικός δίσκος", L"CD ήχου", L"Φορητή συσκευή",
      L"Αυτόματη αναπαραγωγή" },
};

static const LangPack* L() {
    WORD ui = 0x09, sub = 0;
    if (g_settings.langPrimary) {
        ui = g_settings.langPrimary;
        sub = g_settings.langSub;
    } else {
        LANGID lid = GetUserDefaultUILanguage();
        ui = PRIMARYLANGID(lid);
        sub = SUBLANGID(lid);
    }
    const LangPack* loose = &kLangPacks[0];
    for (const auto& p : kLangPacks) {
        if (p.lang != ui) continue;
        if (p.subLang == sub) return &p;
        if (p.subLang == 0) loose = &p;
    }
    return loose;
}

// ============================================================================
// ============================================================================
static const WCHAR* PHOTO_VIEWER_BASE64 = L"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";
static const WCHAR* USER_FOLDER_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACx"
    L"jwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAguSURBVFhHvZZ5UJT3GcdtmzadntN22slMM+0/"
    L"tU2dyXRa20mbsTXTVs1ULRpDOSwSQSRq1SCCF4gIAipCVUAFVg5BlFPuwyWoHEsQ2AWWYxf2hL2X"
    L"3WXvk2+fF7BJJtQg08kz8x129zc8z+f9Pu/zvO+aZ+FUstfZVU2VVmVOwtxMyh+smoRXlo6+mLCr"
    L"Gmq8pmY4VO2wah7CaaiFRV3otmlyhq2KzArLTHKqeTp5v015fr1ZeeGHS//2/wu7kg3YWuE1NcKh"
    L"ZkMxUQqLhj4b2uA0tsJpaoXDSN/192HTsGBWZtoIqN88nVQwN52cOCdLfs8kO7/eIEr77lLKF4tF"
    L"gDbMa25j3tEBh+YRRP05pCTMzdyETX2HilfBZWLDa+2Hzz4Kr2MCHjsXbnMngdbDri2ARXEFBJSz"
    L"lHblYVM8/C8AHE2Yd3Hhmu2BZOAWuB1xUE1cg11TDIfuHpwMyGwt3OSIx9wFr50Pn1NCkpHEMMuT"
    L"sZR25WGbafsEQB3gegL41PBaRiDl5oJHEOLBdJimWXBqy+DUVcBFIE59Df2tJaAGuIxsApgCtePF"
    L"AazTrZ8GWIDoBubdYM44jccw1p24AKER3oBVSW4QiENzDy5tOX0mIMNDeG3jmJNeWA1A82cBHA0A"
    L"PATQjI7ySHDZpyHsS4F8OAMqQTZMUhbsSro3VCT1XXKjgQD49PsqACzypv8B4AVz1lK8F5yGKIx1"
    L"nYOYexnK0WvQCXNgFN+CTXEbdlUhATTCbeHCJFkFgFnWuAxAPbVgDsxZU0EInlQfBO/DM5giFxQj"
    L"mdAIrsEwlU09vwm7ggWXroEABghqNQBSutrlALx0V9NZPSsYH5bvx0BbLISc85APXYJ6LBP6yasw"
    L"ibNhm8mlCamjkeyDQZTy4gBzEiq4HIBnDMxZXX4g2svC8bQ5GuNdCZBzU6kN6dALMmAUXYVVnkOT"
    L"UU+j+YRcWQWASVy7PIB7mK6wFnW5/0D73TD0NR7FeGcc7YdkakMatBOXYJi8AqvsKk0E7QcDG7PC"
    L"VQAYRTWfBbDX0kKintJZba4/2KWh6K0/jNHHJyF5mgjF0AVoxlIxK0iDRZoOm6qEbsQmciV1FQBT"
    L"1Z8GsNdg3lqFeftjMGcPbu5CW+kecOoPgP8oBuKP4jHNPQc1Pwn68fMwi1NpGgpoH1RBP7EKAMNk"
    L"1SKAmoV5WzUVr8C8+T581iayuIoA3kFbyW701L2PkY5oiHpPQT4YD9VwPLSj8ZgTnYdFdh12dRl0"
    L"46sAmBVWANZW+FR5mLeUw2e+C5/pDnxz1dTTCtTc8ENrcRB6avZhuP0oJjkxkPafhoJ3Ehr+KcxN"
    L"xpELybDRhtSNXQT4CV9bSr2y0AvKqXATfMqbVLQUPmMRvLP58BrKqKflqM7ZjtbCAHTXhGOIfQjC"
    L"7ihI+o5jZvA41MPHYBLEwiQ8SePIopZkoKcl6vuU9kuL2VcQOsE9srwOPkU2vMYCKp4Lj+4m3Noc"
    L"MGfV2VvRUvguuitDwWs9AEHnEboPPoC8/whU3MMwjh+FceIDmETnaGqu0/1xIS/vij8D8RXS54Po"
    L"JsrI8mp4Z67Bo78FjzYLbnUmHDMZkNEjuSrrb2gu3IXOqhBwW/dh4vFBiHoOQd53EIqBSBhG34dh"
    L"7AAM4/+CURBFuyATU5zEOEr9DRLTjueDaMdLye5yeGQZ8Giuwa26ApfiIo1YAbqqj6A66220FOxE"
    L"d1UwAYRh/NF+AoiEtDcCiqd7oR8Ow+xIGBWPhc9jJIAMTHSevUqpv0f6Nun5IJqxEnj1pfBIL1Px"
    L"dCqeCud0Es15Ce5e2kgAW9BWtAM91UHgtoRirCMMU51hkPa8By1vD/RDu6Hn7SYHojDvtdPkpNPT"
    L"M455M/ox6Uckph3fIS0Poh4tJtsL4ZakwjWTAqc8EU7pWTC/l176I7VgEx4W+4HzIBC85n9itH0P"
    L"Jp/sgYxDhbmB0HH9oSXNjh0iABsMwku0tk/kUuqfkX5K+iQI48jXSS+RFkPNL6Ke58EtSoJTdg4O"
    L"aRwc4lO0aIpwN/0vSwDb0VsTQABBBBAM4aNgSLsCoB3cBe3ADpIf9PyIBYBZYRo6a4/fptSvk14j"
    L"PQNhXveZF9eXSV8mLYZymMW2iP8NlygBDslpKn4CdlEMVCOF5MBGAtgC9p1d6H2wm6YgBHz2bkx0"
    L"BEDS9e5CYe3AdtI2AghfBKBt+LgyuohS/470axIDwkAwLjBX/3FxJhQ81gbVCAs20Vk4RLGwT0Uv"
    L"SDlcgNLLf0Zl1l/RttQCbstiCwSPQiDpDiTrd0JDxTX9W+khtRclHDntghS0348qodQbSL8nMRCM"
    L"A98kLT8NqqH8GC0/a6n4sQUph1i4fWG7qzIrmFZxODh1EeC2hWGEHYTxjkCIu6j3g36LAIME0BeE"
    L"tPsdBJCM5pLDjANvkRgApg1M/z/u+3Ixzcu/bqBd/gxAMZSPsM2vpiREvJFVmOZf3lJ0qI9TE2vn"
    L"tcXTU/EEFTxKtu8lF6gN3L9DTe1QDeyHciANmzf8cgelZFqwlvQDEnP3P38pafgFryh4eTALziwC"
    L"0GcGYP+2nx+L3PV6UOTO9Zsi/H77ZtWNfTtH2xPPSXpSWmS9qVY17yI0w2doJ8SgKjcydfOfXttE"
    L"6daRXiV9i8SM3cpieuBWiHKIXrMmj0P6NNsRvuUn8RF+v4jct+M32yLfeWNtaOhG5iZ6FswVfbWj"
    L"Ivqtj+pPRacnBLxJ35+NHDP3n3/Vy8UMNzdVx7+Mp41JZeFb14ZGbl37qwT/dUyyLy4YJzZuXPPS"
    L"4bfXvOzv/wIWrjjWrPkP7w5nryBaxJMAAAAASUVORK5CYII=";

static const WCHAR* USER_PHONE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAO0ElEQVR42u2by49k51XAf9/j3lv3"
    L"1ru7p3u6PfHM4Iw9g60k44xsQWIn9gIJC8HK3qAsIyHBKlskrPwFICS2IIRYEwnFChDY2UTROEij"
    L"yIGxnZlJz6vfXVX31n18Lxa3uoOFLLxohzL4SFXdq/v4fed9TsHn8v9bxK/qRm+++aZ8/fXXM6DX"
    L"6/XSqqoSIYQA8N6XRVHku7u7s9dee63+PwHgrbfeGozH42v9/vCG1vJ5IcRTSZKuRZEaa61TpVQU"
    L"RRFSSgHUxph8Pq/25vN8u67N2/N588P5C8c/fUW8Yj9TAG7evLnW7fb/QEr5+1KKL3a7Pa2UYnHS"
    L"OOdO/4YQiCJFFMWAwjSWxniq0jKdTQtry3/1vvmzb3zja9//TAC4efPm8Pz5rb9fW1t7yVqL9769"
    L"ifjlbUIIeO/x3mOtxZgaax1NY6krw3zuyGcVAYXSAlMXRmn+Ik7cn3z961+fLTWA999///cuXPjC"
    L"94RoT1tKCYCUihMGIQQArLUfAeKcp65ryrLm6GjGweEUHwSDbo9uN6Kopn+tJd++ceOGOctn1md5"
    L"sU6n82ySxDRNg5QSISRCCJRq/w/BY4whjiOiKAI4NQXvPXGsybKE/qDLytqQyWxOogVJogky+tZk"
    L"Nv1b4J+WFoBS6mLwYK0BIZBCIRB475BSYqzh337yE7IsQ2vN+vo66+sbCy2Ri49C60ASxySRZnd3"
    L"h3Lucd7Jcla+vNQAvPcbVVNTFDV1VaEiidaSOIqJlEbrmBdf/A2stTRNgxACax0QEEIghEBKifce"
    L"rTVZltI0DQcHh2itmM1mv3bWPuDMALz++uvKh7BWlCXv3/4Z7/74x6ysnSPgkUohpCBOYga9EYNB"
    L"n9FoTLfbpdfrE0UxWp+YScD7sDCdiChKyPOCTqdDXdvNN998U373u9/1Swkg+NAp84rjSU66/jTz"
    L"9At000CmHZlyxNLS2JL9/X0e3P8FpmnwAZI4pT8Ysrq6wsrKCr3+AK01UgqiKDoNm+DPPfXUUylQ"
    L"LKMJqOBlVNtW/QvXI2820aGDdTXBVQgsaWzpRw1ppyYZSLIoEAmDtwX3H9zn/ff/g/HqmGeeeY5I"
    L"KZzxaK0AD4RunufxUgK4cuVK4pzL6qYheEcUSYR0SO2RQkLcxyGwwhGyiNxZDhpHNTNUZUHwPbRa"
    L"Z5jUXNka0RQFJYK6aVBKI0RACNHNsiwDjpYOQJKcUwgfSQFKKZTXyAg6qcQ0lmlZUDkJBLxw9DoR"
    L"tQ8UQWB1F8SAygsaU2JEDP6QqvFY52izZRBC9LMsOwc8WDoAUuZCyS5xFJGmGcIJynqOjiUCR5pI"
    L"qsJQ1h5rDZNKkZcNoJAAAUKQhKjCE+GFwFqP92ERGRxSyqiqqmwpo0DTRCrLhEp1B6nbFyrqGhVr"
    L"fAgIBJ1YMK8aDvIAUiHxIBYZ4SIrbLzHeoeXAdM0WO+RKmKWTzHWyqgtHJYPgNa1VCpTsU5I4ghn"
    L"HUXVIOOYEDwQCEimlaFoPAiFwH3kGiEEnHZ4l4ESrdsLHu8dPkCWdUU1n0dLCUDVSoYglFIK7xyV"
    L"ccyMJdQGETxBSA6mOXnlQCoCIAinBUloCeCdpzYWnSriOMJY8EUgiRPSTiacMclymoBuZOa9DEBj"
    L"DXPjyJsG6tYepqXlqDAICcJZBIHWMD6qASEEyroh0Sn9rqA2DdOpwDlPURR0Oh2/pD5ASue8FMLR"
    L"1IbKOKrGoGLD3HqO8hqCBB8+tgQNAYJoaGyDijJSkSK1QGtNVZeE4Olma3o5TUApKUCAwBqLMYbG"
    L"WbBwmFdYH2it+uMr8BBALbI+ISBOYoJs6wLvPZFWCCF6S6oBjRBSiCAk4HHeYQNUtcH4gBAQhIeg"
    L"Ph4AEITEudY3KNmW0koplFTESYL1fjkBeO9DCCAQSKVazy0FFoeOVHvuQrZm8LEaINBa4INHhHDi"
    L"GpFCorSi00kIzqVLWw63JS0413Z4ZKTpRPHpi/xPEhBkstUg7wNSqDZRUgpCoNPpkE9zlhZAWAQ3"
    L"pVqbTXsdVJwS+GSOOwTIpEdKc9o6wwcirZFSopU67TMuYSJk5UkYi6K2euv1MnQnXSRCnxSAQyqH"
    L"EPKXmhMC49EI7zxKqWZpfYD3wUMgTTsQPCvjAT5JCN5DOAl//yUMisXX4rQDkEqHUuWp0YgFGK01"
    L"1lrqul7OPEBKaQW44AW9rM+VlQ7D/oypKKhETKkySmIMChdaozjJ/yWCNkxAKioi2tYYMhBkIND6"
    L"lEWrfTk1oK5rEUKgMQ0bW1/gd9bO4Z2hnBXMmim5cdRICt2hVhmF6lPKlErEOETbK0CSiITUTlEC"
    L"hHOIRefYGIO1FufccgKoKnDOShEsQml6wxWCrekPR6z5gDUOZx3eFFSmoqhmlLWlEhGV6lCqLpXq"
    L"4LSkpyVBx5wYxuLFT2YJ6dI6QR+QAYkKjn/5x3+gmBd85ctfYeuJLTppzGQ65ef37qOVZmtrkwuj"
    L"IbapaJqGxsww7pi5lwxVhBAReAuBU/U3xuDa5uDyAbDW+uC9F0Jw/9EDhILnb1xn5/EOa+tr/GL7"
    L"Ee+88w7PXL2GVJKf3/mQ0WjE5ctfRMcZPSnQShM8raY4D0ISgsQ5h3MWYxqstfOzBCDP6kLOOQPY"
    L"EECpiOeeew4pBEmSUBQ57777Lq+++iqXL11kOpkQxwn7+wfcv7+NEIKyrLj57rtsP7iPP80pFnDb"
    L"k8dah7W2WUoAx8fHhXN+7r1nPFohBMHGxnnG4zFFUXD16lWMMcxmM6qqWgxFLHfu/JymqfjBD96i"
    L"LAt2dx+zf7ALi/liCAEfAs57QvBorfOlBHDnztQTggtBIIRkMBjy4YcftiMxY6jrmrfffpuiKHj5"
    L"5Zc5f/48o9GIyWTCw4cPuXTpEpcvX2J/f5fJ5IiwePnWHNxpriDE2aaCZwZgczP2COHEIqTPZjPe"
    L"e+89yrIdhGxubiKlpNvtAoHJZMLx8TH37t2jrms2NzfZ3Nziq199njRt0+f29NvKcjEx8kLo5ZwO"
    L"CyFUCEGcpK/GGL70pS9z/vw6IQTW1tZ47rln2d7e5tGjR+zt7bG+vs5wOCRNU+I45u7du0RRxGg0"
    L"hiAWpUAgBH/SLQpCiOXUACml8t7Lk1H3cDii1+0znU5ZXVljcjzj2Wd/neFwyMOHD+n1ehhjeOGF"
    L"FxBC8KMf/YiiKBiNxiRJB+d82yHypxngyYhMLKUGZFkWTgy1HWxqev0+WisECiFKDg4mXL/+PFVV"
    L"0ev1uHjxIgcHB8RxzDe/+U0mkwlHR4esrq5/pHPknD8B4KSU9bKagHPeWymhrAr++Yc/ZDDs0+8P"
    L"GQ5GrK6ukqYdDg+PuHLlGZyzHB1NWFvboKpK5vM53W5G01ikEPjQjstPHGGrWc4YY6plbYnNQ+BQ"
    L"ac3t27e5eu0qvV6Xvb09dnYfMiuOmRdzpFSsrq4xGo0YjUYIJN2sh/NtrB+P+5jG/bfe4cIJ1tba"
    L"6VICeOONN9zNm7cOvPd0u12uXPkiRZFjrUEIqOua55+/ztHRMQcHB9y5c7BonkQYY1hfP8fKygpS"
    L"aKIoXozDw0mStYDgHajl3RHy1jrvPVmWce/ePY6ODsmyLuvr5ynLkiRJGQ4D0+mUKIqoqorRaEiv"
    L"1+P4+Jif/ewxUmiefuYZVsYrJ30G3OlaHabb7S4xAGiC91RVxXDYZ2VlTJ7nFEWbvB0dHbKzs0Ov"
    L"12N9fZ2maU5zgLqu22LHOx49us9wOEAKvagDTJscB1/XdW2XFoCz1jnXrrdEUYwxhr29PZqmIUkS"
    L"NjY22NraIs9zdnd3KcuSc+fOMZvNGAwGbGxs4L3j8PCoPXEBzaIOCMFjrSuHw2GzvAC837PWk8Qp"
    L"k+MJAc+FCxdIkoS9vT2iKKKuax4/foz3nvF4jPft6tx0OmV3dxfvHZ1OilIKZz3WGIyxCBFwzh0C"
    L"5dICqOt6x1qHc466rhkMe+zv71NV1en2VwiBK1euLEziiKZp2NjYoK5rQgjEcbtf0J66oGkMxjRo"
    L"rXDOzd944w23vD7Au8N53RCkoNtL8d5RliVxHJNlGcfHx0ynU8qyRGtNHMekaUq/38d7T57PODw6"
    L"4Ph4wnAwXpTAbStMSoH3nKn6nzmAKFL352UJSKy1rK2NSZKUsizZ29vj6aefJooiZrMZIQS0bp3c"
    L"rVu3TrPD0XCMc+2qnPNu0QVq0+EQzlb9zxxAnhe7Ku6ZWMdRMct5/Pgxk8kUrdvV2CRJTged0+mU"
    L"/f19er0ely5d4vDwkLquOTw8ZDAYYa3DmHahso0AAefM7lID6HYHh1Gk51EUD72HZ69eI0k6fPDB"
    L"BzjnODxsw6DWmn6/T7/fp65rOp0OVVWR5znOOYZDiVsAqOsG7zyWgDH20VkDkGd6MWke9LLOB6PB"
    L"kPHqObbvPyLPc5544gJNY0nTLpcvX2Y8HmOMYTKZIITg7t27KKV48sknuXz5Mkpp6qZZrNTW+EU5"
    L"LKU8XmoAr7zySpWl0d+sr69wbm2NJM7Y3ztiOs0ZDAakaQetNfv7+6cRoKqq03C4vb3NgwcPSDsp"
    L"dV3jXKBZ7Alqrel00v2lNoG2EVL/pff17168eP7Vg/2Mo+Mj6rpe5PyWTifhxo0bWGuZTCfsPN7h"
    L"iSeeYH19HSkljx/v4EPA+rDYDxR0uxlKSWct20sP4Nq1a7O9vb1vlWX9V6trg9/qD3vMZjNms7zd"
    L"Gmks8/nhwqt7kqTD3t4eeV7gvaffG5DnJZ1OSkCQZind3iYQ7jdNc/usn/dT+9HU7du3B5ubm3+c"
    L"JMkfRlHUdc4xmc5476f/znjcbopHUYR17fBDCIEPgcYYpFBEcURdNzx+dI9+v0dRFH/60ksvfecz"
    L"A+BEdnZ2fnMwGHwnjuPfllJmrf1PmM2mjEZjtrY2Tyo9pBQIqZCLX5rkec7+/i5VVf5dVVXffvHF"
    L"Fw/4rMpkMnmxqqo/t9beDZ9AyrIM9+5t37p169Yf3bx5M/u0nkv8qkHcuXPn/Gg0+ppS6mWl1Fe0"
    L"1k9KKfuAEkLUIYQja+1PJ5PJ9x49evT969evH3+azyP+lxVDvPPOO+N+v9+Nokh77yshRH7t2rUZ"
    L"n8vn8rn8KuQ/AT5nAjLS1UNwAAAAAElFTkSuQmCC";

static const WCHAR* USER_READYBOOST_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAYe0lEQVR42u17eZicZZXv7/222rfu"
    L"rq7qrXpLOktn6Swd0oEkkIQEkEUwIIqCKOOVUUcEda46oozeGa86o+P1itwBHBEcZgZQhsiaQBKW"
    L"LKST9Jqku3qrXqqrq7r2b/++950/Gnjm8Y73uWLCtN57nqf+qafqVJ3fe855z/md8xGcR/mL7z24"
    L"Llod3Voqq97JROLkj799zwEAJhaxkPOh5Cvfur/u6isu/XZVpPb6fFn3zMzOYXJqmk6MTxyIDw/d"
    L"+atHvjeyWAHgf18F3/zhI80f/sBVz7Yva9r17Av7pSef+JfsyRPHi+VSyVdX39Tqcrl2Oj3VT44N"
    L"nZIXIwDC7/Pl7bfd5uzsWP8wL7nb7/nyX8698Oxz9w0N9P4rwOsrN3TuMk3j+5sv2dmua/JXXn4G"
    L"n/uj84DPfvYbH6moit71yM9+mvnHxx+9emqk91eAVQQMJZ0c7xc8lcNtS9tudDodS9K54s9SU6OL"
    L"zgu4d/3NG2/kOU78RG9vD06eOPqt7NTw8d/8yPEDTz6dGB87EY1Ewg1NSzf+UYXAx2Nr6isrguuO"
    L"vdGXHRscfBwArr32474d11xxzXRqfncmnXafOzfwZcrY0eqqiov8XnfDHxUAbslTL8uaxzDNM3Nz"
    L"Y6kv3Hf/xVfu6voxpWxNYnIG1LIQDlWdc7jdWno+B1VW6B8VALIsk9nZFJyS0162ZYvPMPWHswW5"
    L"rTLopZLDAQLG+bzu3U7JYfX3DyA5mxxfjAC86xwwnUzO5vM5rSpcucRlh6KaphqJySQUVcX0RBz9"
    L"PcdpYnx4o2lZW3p6Tk0m0/ET/7uWDeL3H37mb3/65P6b/+BugZHBN4vru3ZcuXF9R5uiyUSW1fzM"
    L"9OSacwOnmW5aUFSL+QIhUiiWMDY8+M346eMv/6aOO770xb1XX3H53xi6sdcRiEZikeWvDA11m38Q"
    L"AACg1fWtuVhdzQc3rFu7aTg+0nDqxFHHDXtvxsWXbMXQ6CSxLZvkc9niqvVdJy7ecX3Hxm1XNC1Z"
    L"tYnvP3EoC4C2tK36WHWlf8vrR45jany0Ew7uxeGB7ok/mDpgeODEGeIIMockbVvSttxtWGCMcGio"
    L"q4OsWwhW1SDWvMwRDtfs8AcrrgwGQ3uj0egnLr3ypmujTSurOjeuv5owO3po/7NEELheJVP+bxMT"
    L"g8b5LtUveC+wesv7tobDVbdHI9EPd225RPQGqlCWVViGAkPXYFs2OJ6Dz+tFZTjMNcUawMHC1PQM"
    L"9u/fz4699kp/uVS85Vzf0b7f+G9sUZfCb4tUV9d7600f8revXi0YJsXQ2V4kx/thqgqCfgl+FwUz"
    L"CYpJB7Kzk7Q8P42NGzsRjdaiPtbMDYeHz3S/8XDfb6hliy4EvvDNv2ttWrrusr7u1868/d7NH/96"
    L"7Z/e/uFnL9+1ZZfAAa+8uA9TiRFs2rAOWy7eDrV0ALdc/wA2rOlGx7Jh1AYMTM340DcwjmyhBIcn"
    L"xCqra1fVNi2L9p84/Nx7ZfjvDMADDzzgbl990S99FZEvOP2R0NmeI89v3/6n3ls/edMvt2/d2KUU"
    L"8/TRR/4B0Ug19n7gBjTEYnA6PTg7dBAd616CK1CAy51ENnMGyvQQLlsu4OxMCLPZEnTDRqiiorO2"
    L"ebk+cPK11xYlAJsuv/kLNbUNHwsG/JgYi3e6fNUn9t6298716zr32qZOH3/sUSxrW4rrr30fOF6A"
    L"ZVsQeAKXqxpe3/Ww6LXgaDUOHzbRWdeHZlc3GgUT08ZKpEs6SiUZNZHIxa5A9YHRsyenFhUhctvn"
    L"vh50SL6zlLFIOOSlq9euIxOJSaOhvl5sbGrBm68+h6bGBtREwqDUwvp1a2GYFlTdAqWAaXJgjIDj"
    L"OBjqFOrK90Ga/Bm4AjBu3IB/LOzF5GwWFaEQVypkD/3wW5/dDcBYNB4gh1ysMdzcqOtqZzI5S+oa"
    L"GhFrqON50UFmJ0eglvO46n1XIhoJIz2XQjRSDcMCTNOCbVtgzARlBijVwYteGIHrIFICMnwETx/M"
    L"oLGxFTkhAofThcbGWBPl/SfjA8fPLhoAsvG4ffLIS8+FqmunLNtOUkrWNDbGOEIIzvV3Y+fOyxAM"
    L"BOCUBLQ21cOigGVRWJZNGKWgjILRhdxGqQWT42Drreh+6DDqnGPYyo1ipvZyOEI16NywhszPzwdf"
    L"f3nfLxZbL0AP7Xvswf1PPXhnOBwa9vqDXH4+harKEGpra8EYhcgDhmWDUUYWkjmDTSmhlMG2bUIp"
    L"hS2KEOPnMPnVv4Dn2k/hovowpIlxtBf6IDkENNdXsJbG2LbaZVvaFmUzdO3tX17R3NjS7BQllksn"
    L"IUkifr1vH1SlDJMS6CYDZYwRMIAxxhhljFEwxpglCHD2nEb+M5/GfNdmVFxzG0o11wHzQFP8GERq"
    L"YHImx3zBSnfLkmU7FiUA0erwlnAk6uZ5MFUpo2PtWlRVVWE2NQdVt6BqBnTdBKX0nSud43hAEuF5"
    L"9VUon7gD8c2b0Xjzh2EpZZRa94AqErREArxaxMDQOHKFEgJ+f9ei5AN8HldnbU01sukknE4JDqcT"
    L"HR0dcLvdUFQVBAABA0cAQggy6TkMDMfROjqKxr/+Nnq2bcHSu+4GMU0wYiMTbsG/kFWgSRlNuoyp"
    L"XBapZAqEkLb3ohz+nQFwOZxLolVeZGZkuFwuMEphmiY0TQNjFJRSMMYIz3OsVCyg53QPTg6cxRP7"
    L"X8KVK9uw/t774OB52Ja1kCUkBz4icKgspHBELSNOAkjPzcGy9ErU1zsxNaUuphAQeFGsdDsJqGVA"
    L"khxvGwzDMP79ixmGibPnhnDk+EkMDw5glBDkP3ATQhUVsAwdlNqwqQ3GGJyWCFvjwDHAskyUyyXY"
    L"luVsiUSkxRYCgm1ZrmzBhmoANqWYTaUgCDzqamrfAcOmNnieh2Xa6OvrhaGrMNUSqoJ+WIYBy7IW"
    L"fJsjIJoGUtRg2yLg9sJSTVimBkYtyvE8W3RJUFE1NpPKwbAIyqUyXnv9dSQmErCpDdOyYFomLMuC"
    L"oetoXdKC5W2tMHUZXZsvwurVa6HrOmx7oUCyKYU6O4vCzBwgumAEK6EpZYjEBk9oWSqXtcXmAWa5"
    L"LCtz6TR0k0KUnLhq926IkgjLtN4peCi1AUIg8DzuvPNOZOdvRG1tLURRgK4bAGNgYCA2Q8njxv27"
    L"r0BrPoeWqmpY/WfgdQIZaqQGBweNxQaAXSwWZ9OZeXAcAQUPWVXhsCwQLwfGGBhlYGyBAac2IAg8"
    L"YrEYRIEHIQCoAIsjsKkNatuojVbjhj//IuYLRWgWg8RkeBwMlm0NLMprsFQsDsyl01f6fT4wToIs"
    L"KyA+NyTLAsdxsG0bIABjb4WvxaDrAJgEl1OC4HTAphSapoETBHDcwnUZ9gcxm5xEXQXBubwMRVEO"
    L"/0e/f881aG5bunJjpLGlTRJFv67kNbmYS6RShb7DR8eGnu5B/sICIJfeSM7MADU1kEQBRcWAz+dG"
    L"f38/3G43WpqbYdn2gpszBsJzsAjAcxwACS6XE6ZpgCMOEEJAwEDpQkhI+hgiQXCHZjNzs3OZdwDY"
    L"UAP3h66p2V1XU/Px5hVrtzUuXRlwCDrk3BTUMg+lJKG5xmevXxmdvq2kvpqYSj5+109S+wFo5x2A"
    L"bDn1RnJmOuPxeqtsSmnAF8NEYhr9A4O4fOcOWJYFalPgrTCwwRZcHxSCIIDjCByShPGxMVRVViAa"
    L"jWA+L8NLJqFwUziTyiMxU/jlWP+x1J9dUx1Z0uD8YE0kfEfj0hWrm5Z3gBADxflzdC6fgqmrIKYF"
    L"Q3LhTLWH04geu1gI39Kx5bJbNu/hTrzxwjN/fvdPhl4+r6zw3NiQXNO4YpXX41urKhrjBAEVoRCW"
    L"LWmCIIpglC4YTAgotUEIwPMcfF4PeA5gjMLldMDldKBQLMLl8cFBypDHn0QuXyR//9SQbGTP/vCO"
    L"S/UPtcbC93ds3HzTyg3bI6HqaqaVp1gpl2CmVga1DTDC0Bd04vlYDnPNPXhenGBtzh3skjU7UV/v"
    L"qOOU5N46aerooUF77LzS4lX1bVMCx9/mcDqEuVQK0doG+DwuMNuATRksStHT04NIdSUEjofDIaJQ"
    L"yIMjBAG/D263E5WVFRAlJ0xDgTz+NFIzcTz5UpxIpZP2FStTeyp8wp7W5R3+FZ2XU9h5ZihzsG0N"
    L"oBYYNSERGyeogQdrXsNHV3mw3nERbnVdh23uEPITR3DsxV+yvt7TDp5Dx+Y665FDQ//xqs67YoXf"
    L"3P/4CddVtz7idLn/hON41tfbg+iuHRAgwDJkZOYzSKfnwfE8GKPgCUFyJgmlXEJLUww8zyMzn0c2"
    L"X8bowEHMDZ+Ay4pjR2yColZ32vC73S5Ca2o84FAGmA6OMDBCF46MLjQIXYFGtNVuQaToQJPuhl1K"
    L"on/wdfSd6kViZh5lndEqn7QW3vA2IP38eaXFJxPnvsZzZHvLkmVt0aoAlbMziNY1wnZ44FQNdKxd"
    L"BWrbICIPQhjq66KwbQtlWUGuqGFkbALnup+FS+7GijodAX8YoqOWGPIUlMIM5XgOnkAFwAxwxAbH"
    L"MYBSEGLCGYjAW7MO3qpmLLMUmPlxJOLHcfb0MUyMJVCQDdiMQ5WXIFYtYNxccf4BGOs/lvK4gh+T"
    L"JPHXK5c2hCqDHpqeGUNRA5yeANz+Cqg2BRF4qAaD4PRBU3S88upxzCeOI2D04dIWHyL1W8GJEmDP"
    L"w9bTUCUdsEsQRAfcvhAIVUCYBh46RHcA7ppdcFWvAS86YOTHkBk+hHj3S5gYGUa+oAGEIBKS4BAM"
    L"2DpAQ1uhyJ0x4PD5H4z0v/nCEUou33vglUOPWuZFNRWhAJuZmWPJmVnYjMDp8sDtcsIlMUCdgxdz"
    L"WNVEsK2rAVXRmwDeCUPNwlBSMC0LNgDR4YDL44PbXw1R5GHrRQgOD5wNOyFVd0KQgrDkUaTP/jMm"
    L"Tv0rEvEhFEsqOI5HY6wSfieDoemQuaWY5btQDO2GODcmXrDJ0ODxl1425Yt25nP5/xFrqN0ZCASI"
    L"ouo0ly+CmXnU+4pobZHQ2bkMK9ZdB0+wBtTQYCopGEoalp4DNYsLrs7RhWvS7YE/VA1OkCCGr4BY"
    L"tQWCVAFbTSA/+CASp/8JkyPDKMsMguhGU3MVPE4CQ1Gh8jGkvRdjYDaAs8OjWNMxB8BWLuhobHjg"
    L"2JnhgWO7h5s3fSBc4b63PSa2X7WaY+tXVmPZqutR2bwdcIZAlTSs8jQsdQ62nodtlECtIhhVAaaB"
    L"MBuiyMEbXA5f4/XgKy8GL/oBfQ6lcz/GdM8vkBg9i1KZQHIH0NAchMfBoBYKKBgNmPdvw2DKi/ho"
    L"AkopCcsyQQgHRVPTFxQAANi1Ab4vfjpWUelUxfpYI8Jt14ELdwFwgBppsOI4qJKCrc3D0nIwjSKo"
    L"JYOaMqitgeMFOF1+ODzNcDR/CsTVQDgzy5TRhzDT+zNMDPUvGO4Noa6pAh4HB0MpI6fVY87zfpxJ"
    L"uRHvTUBTEuC4hR4EYLAsE5nZVO8Fmw5v377defMtH/1oNETurg4oy90OCb7KVup2OSFwKlyCAQdv"
    L"g1ANtlGGpRVg6jnYZgnUNkB4EaJDgsRbEJ0VEOpvB3gP1MQTSPb9HFMjPciXKARHCBUVlfC6JWiK"
    L"jDKtwyzfhf6kE2NjCZiaDEHkQUDe6UpNy+J0w84eOfHmmuzU8PR594D/+rW/3tDUGPtRy5K2zZrF"
    L"YZ53Un+sCe6gExzTiKmXmKKmwPITELRpuNgUJN4EIIF3euEQRYhEB8fZ4Nwt4INboacOYLb3ISSG"
    L"TyKfNyG4KhCpr4XHyUMrl5CRI0gJV6E36cBwfBSmWoYoCeAFARzhQDgOtklRLBZJqSwjk8n83W8z"
    L"/vfygBtvvNHrr6w/aplGu2Xb1CFJaG5pRfuq1ViyZCmiNRF43SJEYYF1yRVUxM+dhD71K9R7xlHb"
    L"0ATREwJED3hBgiErmBt6CZNDR5Er6CB8EBXhWnhcItRSESVWhxS/BQOzHgzHx6CrJfA8t2A0IeB5"
    L"DmBAsVRCuSxzpmmgkC/8fLDvzT8BoJ/3HKCawuqGQKi9eelKVlEVxWxyCmf6utHf14totAbtq1Zj"
    L"7ep2LGtrhm4RjCZmMZYimJf3oOg2gczLqJMoOJrHTO9BzIyeQb6ogXBuVEZb4Xbw0EpZpPQGpMQ9"
    L"GEh6EB8Zh66OQOB5iKIIMPZOC14ul6AoGmcYOmRFTuTzue8mRs7eD8A+3xsijh//9ImbotHGL4lO"
    L"b7tpWQzMhiAIIIxienoC/ae7kc3MoqqqCsuXr0BjSytkzYKqG1jSVI+O9qXQDRMDL9wN1/w+li0T"
    L"wgkuBIIVcAomtHwSeSuKlLQT/bNBjI6NQ1OKb534wpCVcByobUOWZZRlmbNME6qizuYLufvHkxMP"
    L"QJZT53tFRvz+Tx69oaa2+W5J4DadfPVXCAUDdPnGPSC8BzzPQxQFON0ucKAYGxnCyRPHkEknEQpW"
    L"oKW1BR0da7F+7XKMjE/hmRdexexEP65uOYJITRQSLUDJDCOrBZGSdmEwU42RkXFoSn6BS1joqcFz"
    L"HBijkGUF5bLMWbYFuVyeL5QKD6Umkz/StNzk+R6Piz944LH310Zjn/dXRrqSk3H8r+/cRROjQ8hr"
    L"HD5260dw4x33ggguOBwSXE4HwpUu8BzBeCKD0ZEhnOo+jmI+g7q6OrS1teHw0VPwBwOIhCNYov0U"
    L"MdcQ5ooSZsVdGMzWYnR0AppSgMBzYAv00sJ0hFGoqgpZfttwuVgoFn+eTaW+Xyym39UzCeT/kOSk"
    L"nVfedHV1tP4ef7B6CwMHQ9eZaRns5NH9ePn5p7B202W44uoPIlhZBa/HC47pCIb8qAwIKCsWUhkZ"
    L"iqxCFBh6Th3H88/uQ9vylWhuXYZiuYz+wWG0iwcQa1mO05lmjIxMQpVzEPgFFwcICADbtiErMlRF"
    L"IbZtEUVR1WKp9IvUXOpvy9nZwQuyIPGVr337e7uvvOaeQFUD5jNZZlomA2PgeR6SwwlKbTgcDjAA"
    L"PEfR/+YBPPXoj7B8xQp88jN3I1S3EuWyjnw2jcHBQezfvx8To+ewfsMmiG4f4iNjsCwTfq8btmWg"
    L"lM9A4Ak4XgB5K84ZY1AUFXK5REzLIqqqmqVi8al0JvfdQibRfUFJ0blMZuk3vnoXrr3+g7Rr6x7o"
    L"lgDbXhh4ABQcz8GmNpxOJ8aHe/DgD76CeDyBwdEUui67Cl1VTeg9fQonT/dDKaswDROWZUJVZci6"
    L"Ccsy4HQ4kZmbgcgx8BwPxgBGKUAIFE2DIivEMA2iKgotFYvP5AvZ76aTk+d1h+i3AuDy+JTJxAS+"
    L"8dXPY+v2Xbjtjj/D0pXroJs2dE0DIQSiKIDngJVrN+G7D+/HqTdfxaWX7UK+WMJDDz+G2VQGDkmC"
    L"2+0EL/CwLBOKosDtd8Hn88Hr8UItzYMw650ZqKrqUFSFUJsSw9BRKBReyuVy35mbGdv/ntLioiAa"
    L"Lm8AQqGANw6/CHP6IHZcfTsuufZO+ANhGKYFSRQgSQI8bgk1K5Yi6Hfh4MHXMRwfg21TSIIAgefB"
    L"Czx4ngejFLquIezxgHA8NE2FYeiQBA66YUCRFdjU5kzTRKlYeL1YKHxnZnJk3wIH9F7PBTietSxd"
    L"jXA0htnJIcj6FP7nAw/g8X2v4o7/8lns2H01XB43nBKH3HwKB/d3o39gCKZpQeD5hfRCsNDeiiIs"
    L"Q4NtL+wNWKaFbHYeuVwOqqpCoRYYpZxhGCgWi6eK+eJ3ZqaGn8R78MjdbyVFL9vz/q07d+zsMgwd"
    L"Tk8FSjSAskqRTs9i+tQTSIz0oSa2DGOjI3jttaOYTqYAkIWTfmsoIvA8J/KUjMf72cnuIxBFB4Kh"
    L"KpQVFTPTU1DkMkxd5Ri1ST6fG8ik5788Otz3+VIxe/pCnvr/FQDzs+neSF2ssqura024qoI3DIsF"
    L"Q2FwHI9C2cDR7gHIioa6ugbo+kKpzQje2gwBEXnCKcVU6XT3Gyw+EhdFUSJuj5e5PV7k8gWUygXO"
    L"tkxSKuRHMvOZ+0bP9X2mWJg/BsDCeyi/FYB0eqZ86MCvn1YNdLev6li1qbMzapkGIZzERHcIiqYh"
    L"1hDDqtWroCgqLNuGTRk4Ak4tZVh6ZuTx3p43bjsTTzzm9fj8Lpd7qcfjEwWHk2SzGaKq8lm5kPvv"
    L"8emRT5fmZg7iPdoLfLelcOBL9/7Nl7o2b/lcuZT3vPLyATo5NYX2FSuw+aINyGSyUDWDy+ezmEnE"
    L"Xx8fH/7LV1555cV/r6C5rb2zsjJ8K2WcJ5dLPzd2ru95ACX8J8vv1Axt3f3+TTff+JG/ampu3Hnk"
    L"9cMwTYtu2tjBTU9N49zZwdGR+OBfPf/8sz//zzrNCw7A2zfkp+6695Pbtm2/N+j3V7959GDx1Mnu"
    L"H+1/cd8PyuVyGv+vyMp125fc+bmv37tu3UVr8P/lD1f+DTZsb8HvtMkCAAAAAElFTkSuQmCC";

static const WCHAR* USER_REMOVABLE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAAp9ElEQVR42u192XNc13nn75xzl97R2EGCiwkSXGVJiWTZYzuW"
    L"bKVsx5EnGUfMHzAvqZRf5mGmKjPlKkkvebAmU5mMpmoka+xJqhRnBFuKlfIiJxqTMkNKNhfZErUQIkWAAkhs3Y3e7nrOmYd7"
    L"zu3TTVAiaUoWpf7kLoBAN9C43+/7fb9vOddA3/rWt3c2KSX5TZ7/Tq+/1p99o41+GJ11vRf13V73mzjrt+3oj0xkXu2FvprX"
    L"3ginmT9DSkn7QLhBF/RKF/I3AcCNBFifCd6HvHstzr5RaaEnosm7AfKD7vgPrAYghMj3MiKvFxQbva/38nV9+y0xTJ+qb6Ky"
    L"68P6HvrWt7717SNI/3367Vvf+sKvb33rW9/61re+9e3DpxM+KNUOvZkc3++nfwij8UZO/frWr7b6dhOknb71rW9961vf+ta3"
    L"vvVLi771rW9961vf+ta3vvWtb33rW9/61re+9a1vfetb3/rWt771rW9961vf+ta3vvXa1a7W9/c2+ta3vl09a/TtI+BM2r98"
    L"Hw0gXOl7fQB8iJz+bodne+9y2gfAh8Dx/TuX9kHQt4+6068HCH3wfAhB0ReBfUboM8BH0en9PkAfDH37qDhso1r+N7E+A9zE"
    L"oOnX8n0g9K1vffvIs0K/CuinhL717fqMfAQihAAghw4dIgBwzz33yJmZGQDAwdOnJR58UBqqGgBkHwA3v9PpzMwM/vRP/5Rf"
    L"z+tnZmZotVqlu3fvlisrK/LgwYNSA+PDBhLyYXI6AEkIEfrrDz/8cH7PngP7i8XsdkJIKYoii3O+FoZhOwxDv9kM11Zb1erS"
    L"+fO1l19+OfzJT34SXo1zpZT00KFDFABWVlbk6dOn5YMPPihvRnCQm9npMzMzFEBXpH/rW98a2rJ9+5fKxeK9juPcWywUtpUG"
    L"ijSbyYASijAKEYYhAj9EGAZxEIVRGEZhFEWXojCqhEFwKYiiZSHERSLlYhgEl9aDYPGNV16Zf/PNN5szMzPeu72vQ4cOsZsF"
    L"HORmdPrBgwe7Iv2JJ54YHBsb+0qpVPpyPpf98sjIyMjw0CAs2wYI0xdeeK0G2u02ZZYFx3aI6zpgtr1hMRRHAaqVKprNJlrt"
    L"Ng+jyAvDsBqF0Zzv+3Oe718I4/DtwAvm1tfX5589ceKtZ779bQ9A/E7M8dhjj7HFxUUJQHwQgEFuBqdvlNMfffTRgenp6d/P"
    L"5/N/kM1mvrJpYnzT4OAgBOfgXAjf90SlUqGvvf4GOX7iBHnp1Cn88vgJLC5eRLFYxPDQkBwfH8Po6AgGBgaQzeUkJQSO48it"
    L"W7fKT9z1Cblv715qMUZBKKGUQEog8Vdy2VqtJur1Blqtdhz4fiOIwvkgCM+3/fbZ0A/nvdB7c2lx6exPf/rTtw8fPty6kqN1"
    L"StGM8dBDD8n3CxTkZqL3hx9+OH/bbbd9qVwufzWTcb88NjoyMTI8DCEEOI8F50IKKSkByMrKCn55/AROnz6NarWGKIrABUcQ"
    L"BFhfX0elUkW1WkV9vQ4/8EEJxdTOKdz7hc/j937vs9gxNYV8NguAQEgppZSQUoIAEoQIQiCllBQApZQRknwBhBAQQtBstbBe"
    L"q6Hd9kI/8GqBH51re+0z7Xb7TBzHb9VqtdlXjrwy+7c/+Nv1jZytU8mhQ4cAQLxXoCAfQHoHISR1+iOPPFLYt2/fFwuFwhez"
    L"2cx9I8PDk2OjI5BCIuaxlFJyzjmDBEm8IwEBMMbguA4sxzEoXoCHEaI4QhTHiOMYPOYQQoAAyGSzcDMufM+D7weIeQwpJSAk"
    L"RPImIZEAQUpASgkhBBJsCIBAEhCZPBOEEEIpIQQEIOq/KI6xXq+jvr4u2p5XC4Jg1vf9N9rt9qtBEJy5dOnSr5977rnFF154"
    L"wbsSKFRlIm7EMIh8EJw+OjpKPv/5z6e58+G/+7v8705O3l0ulf7Ycd37hoeGNo2ODkNKCc554vSYMwlJEk4mxl9DQFSoSCGS"
    L"j1ImT9NtAQIVqepFUkJKAc4FYhGDgCgHC0ghIKSEFImzhRSJ0wWHlFAAUKCAACTp/DsBiJTqt6S6JQEGJYSm/YcgDFGrVdGo"
    L"N4O2117yff9Uq9U643ner5aXl3956NChC72g0Nfvesrd3xoAruT0Rx99Jrd7d/EL+Xz+SxnX/aPBwfLW8bHR5CImV5dzIRgB"
    L"IYSxbgci+VzTb89v1P9T0asiWUUxdEQDgOhEuH6OUN/rRLuAENJgBgEex4ijWIGII4oiCCHU35v+3eqRgFJwIdXVlwREEEIg"
    L"AWYxRjRAhZCoVqtYX18P2+32Jc/zXmw0Gier1eoLr7766q9nZmYqNwUD6I7coUOHqOn073znO5np6el/Y9v2V7OZzP1DQ4Nb"
    L"x8ZGIZOLJ0EoF0IyypIc2+XchFnNX5I40XAYtMO1AzQY0og1/i1MejdAIRMGACSkgHqeSFOA/tlaIUopFQA44pgjDAMInjCJ"
    L"JOgATQMM6rUKbIowQCkRBEQSSimjjIIAzGKIghArq6toNBqVer3+wvLy8uPf+MY3nn7ggQfoQw89JK7VN9Z7KTBVzqKEkFiX"
    L"Yg88+aTzhfHxz2az2T/MuO4fDQyUdo6Pj4ESmriDMA6lqgghFmVXRJVSRDqqFcMrR6rvXO5wofI3RFcu7/q+Sh9COx0icZBI"
    L"fm4X7ZuRrX4WSVgetk1hWRa4EIjCAFEYgsdcZR39PjTbaLaQREogjgVDylKQlFHYti1s25abJzdbBGSoXC5/pbJW+Ypl0f/0"
    L"F3/xX/7rk08+ya41HZD3I9IfffRRe/fu3Xe4Wfe+Qjb3tXJ5cN/4uI50AsqsWAKMUEre7Q1J/ab1RSOJ0zpRrp4nDCqHBE+8"
    L"mjgw8WoS2SrXJxGIjiM1AKQEgezQvpkaUtYQyVtQX08+CkAghaKuEqIwaUQJmQjP7p+DFJyQqu1MCBhlYIwm2kUCbiYjBwcH"
    L"US6XY8dxrKeeemrt0Ucf3Xf48OFVKSW5FnF4IxiAPPDAA+See+7pivSDBw+yr3/967dms9n7s9nMHxXy+f2TmzcTSggiHiGK"
    L"otiyHEqZRQhgvZvnU3rvonjtaCROUg4VXVElUscJIy1Iw1kSygkqv+uf0eVwI+oTFpAAlEBMdYUSnaLDKKlWUHqDsoQVfN9P"
    L"mUCDBwaACSGglMKyGAihkFLCdTIYKA+gWCgQQgharZbNOef79+8fueWWW+4/fPjw/3rwwQfZOzWjbhQDpE43I/2BBx6gn/3s"
    L"Zw/ki8Wv5jKZrxWLhdu3bN7MKAU4F5BSxpxzKoSgbiYHx3URx/E7MUoS8SpXkvQ6mY7siWB0nJo403RGJ99DpYDU4UnYK5Ak"
    L"ThRSpD/PZIDU6UoLpEBKmUVASqJSkfpcfZ+AwPd8eL4HSgiYZaXsoB3PKAVlDJQmjnccB6VSCYVCAZRSxKqElVLCtu2YEMIe"
    L"f/zxH37jG9/4qpSSKgaQNxwAug41nQ4Azz333M5MJnOwWMzfn88Xbt28acK2GAPnHFLKWAhBdXqQQgCUwXWzkESCEprGeFLK"
    L"67xrCKzeiO8Sa0hLMClgAMPM8cKI8t5HR+hpB3OptUC3TuiwSMIAmv6h3q+QEsRITVKkiT39+RZjWF5ZQaVShes6KBZLcDMO"
    L"IJPeBWUUjFBIAtiWjWKhiNJACYwxRFGEKIp6g0bm83ny1FNPLf75n//5bgAtPRjrJdDrSgHa6ffccw9XyIoB4EfPPbdzIJP5"
    L"d/l87r5CPv/JTZs2ZRzbSp0ehqFyOrF0iaYvOo9jRLwFzjkc20E2m1X5UCBVdCBdatyk416lnn5vIyV/RbFmgET1C7Q26DxH"
    L"6wHZYYIUSEYJaZaUBoBNoUcAWIyhtr6OhYW3EYURCGNgloVcNgtCCZiVKF7KGAqFAgYGBmDbNuI4hud5SeOKJ40rMyVlMhkU"
    L"i8Wx6enpbbOzs68dPHiQzMzMyA0CXV4VAIw2pFDNi8TpP/rZlmLR+uNisXh/Ppf75ObNE5mM6yKKoh6nw+ot26RB6Z7noR1E"
    L"oCDgQiDruiiVS3AdGwDAhaZN0cn9QnTROLoutDA6c9oJIi3jtMO0WOtS9WmtrwHUyeepSDTTif69KVA7Tk4ZCAJSqIYTEkqP"
    L"oggLCws499Z5xHEEx7HBCINFGSzHSkQfJcjn8iiXy3BdF0II+L6fOl4/4jjuAkC73ZaZTIZt27ZtcnZ29rXl5WXyDowvNwTA"
    L"lZ3+oy3FYvGPS6XSl7MZ956xsbF8oZAHhIAfBHGz2SS2bVMppWUsTGxctiknMUoRBj48zwcA1EGxWq1hoFREqVSAYzugRIkv"
    L"LrrovKMFEmHWyckGIwjZeY2ORJP+UwGHnhTRUeD6Z3er825KNxlJlfcgyXAAlCZXOggCrK6tYWFhEZVKBSCAxSxwLjGxZQwD"
    L"5TIggVw+h/JAGZlMBlICYRimdJ/MOnj6b1MDEEKIZVnStm2SyWRyALCwsMDMuOtxehcILC3eTKc/+eSPRjdtKv7bYrH4Jdd1"
    L"/nDzxEQun88hjmNQSvnC2wv45fETdGRk2Lp06RK2bduK22+7NW26dNVrRr2uI5kygsFSARAClVodcSwgCUW90YKzZiOXyaKQ"
    L"z8F1XDg2g8UIBJBGpXbiZU5Q5ZfcsLkjjJyvevsqioSpBdQsIVXwXV28DiPITmmiu8tghIBzjmbLR6vto1pZQ7W2jkajCcE5"
    L"mMVgWxaKhSImJiYwPDyEfD6HcrmMQqGQjKHjOHW0pnvza6EqIw0AJL2G5Lk0mVK2mOF4oTwhNgKBpepG8cQTTwzu3Lnza9ls"
    L"9ouuY39pfGxsoFgsgvMYUgoeBAGEENS2bRZFIfKFAnbv2YsDBw6g1Wohjjksy0ovGqBLH2OIAjVPlQChFIODA8hkHKzXm2i0"
    L"PESRjzAO0W75qNVbsGwLOdeB47hwHQuWTcFU21dTNowaXV4GBpkOh+Rlil0aOX+DnJ72cbvzuNleFslsAlEUIwgiNJpNtNtt"
    L"1OvraPsBBI8hhYRtW7CyLgq5PAaHyigPlFEqlVAuD6BYLKYOvBrHB0GAMAzBOU+dr58XRZEDgAZBYKXTr46zNQi6UwAhRP7i"
    L"F7+4pVAo/GR4aHCyWCpCJD+cB4EPzgWVUjBKKRil4JxjcssktmzZkkzRCEGxkE/fdFK2mSre7MerUodSkGSKhoybgTNiY6BU"
    L"hOd78LwQQcQhBAePgGYsYAUclsXAbAZGLdiMwbIokmGKUJ03VXbJZGCT1vW96l8op0OkbdnkKmmnmh3E5D1yKcG5gOASXHDE"
    L"EUcYRwiCEF7gIwojhEEALwggOIeUHK5lgWUyyLg28vkcCoUispkMcrksBgYSx9u2DSklgiC4LM9rx0dRlDo9CAL4vo8oihLn"
    L"JSWkBEAajYZYXl72AOTCMHSVw7l6CAMMXfys8/Y3t2/fPhkEQbi8vMp0pDuOA8dxkM3mQAlBGIWAlIjCGJTRRGjJpJRjjCWl"
    L"jxZZ6sLqKNVgEKpVSikFpABXPVzbtmBZBeSzAjGPEUUxwpgj5qrUAgHnAASBBAFPJicg1IHFKKjNwAgFAQElBJSSBCCKLahi"
    L"RKEagoJLxDxKflcQIopiRFGImMeIYw4eJ+VgHEfgUQTOpfpeBB4lOVhwDiE5BBeglKKYz4BRBttmyLguMq6DjOuCMAbHtlEs"
    L"FlEqleA4DqSUqdM3yvNRFHVFvO/76edCCDA1ENNDp1qtFp07d66Sy+UKnHOtAWJVEnKjOdTFAtbzzz//mXK5/MXXXntd/vO/"
    L"/NS+dPESsSwL5XIZjuPAdV1ks1mMjY1icHAIrutiaHAIjuvAVd8nRCJseyCQoJSCKLaglCTA0CNUHWVabRMCKgmkSgtqpg5G"
    L"KYhjw7YsCDWmTWieQ4JD8AhSMggZgzALXNoAEQCzwZgFYlmwLBvUskAZQAlAaQf/ggMiBoIwRhAE4NIHeABBJGIpEHOhgKdK"
    L"QySvtyQFtS1IBkihWIzIBHAkAbXFGCzbUg4CKGEolooYGBiA67rpLMGM+I3oPgiC9KGdr1+jhbaUMm0Mra2trTSbzZbjOMUw"
    L"DGP111IAkeF40aMBiJXNZr8yPDzMHnvsW/wHz/wjmxifQKlUAiEEtm0jk8nAcRwwRiElkM1kkclmIITA6NgoMm4GpVIJ27Zt"
    L"Qz6fA7MsZDNZ2LYNy2KwLRuWZcG2GRhNUEsZVdEoIEBABQGHBCEUBAKgFIQL9TaTlighIu3OESkg4whcaCJXJSdJ0gIlFIRZ"
    L"oIyBMApGGAg1dgYESfK3UtORGufGPIIUMQgXYJInglVyCMIT8DI1G2AEkDRRBYSkzSzGKCizEkFo1PLZbDZ1vI74jRyv6b7X"
    L"8Vr0cc7V72HpR8uyRKPRoGfPnn01wYQsAvANuufpzHyD2szK5/P3Li8t4+c/f56cffMsLszNg8sE1ZZlIZPJIJfLwXVd5PN5"
    L"DA4OgjGGwcFBVCoVhGGETMbFsWPHIIRALpdDoZA8b2RkDPlCHhnHxdDwEPK5HHL5HGw7oW2LWaAs2ZRhFkvShUgYglICLmNQ"
    L"UDWYYQAVoLJDe6AAUR07EXdXCN0iL0lVXBozAnMMa2q7HuEH1Z00dU2iYwgIaNK6ZSzp4inWy6la/kqOv5o8r/9tCkOkJSaF"
    L"bdu6aiCzs7PkjTfeOA7AkVLaBuVHRrSLnhotqQJs2/74+fNzmJ+fp/l8TgkT/WcnKnd9fd3ccAFJlifhuhm4roPxiQmUisUk"
    L"DxaLqK27WFy8CEJeS/vbmUzCFENDQxgeGsLY+BgGB4eQy+WQz+dgWRYoISCUgFIGQIARBlhAHANUSgjR2bvTRilVrRcCpvvw"
    L"RE/xqHrfyUeiyz5VGeiKgBoln1rMUBVsR0RL2R1ClFBQRpKePWGgNPkbBwYGkM/n0/wshLhiPW/m+d7I17W+Zgod8bZtw7Zt"
    L"EEIQhqFoNBrkV7/61dIrr7zyEoByHMd+1w5c8uAGCLrEoGVZVq5aqyCKIji20yl3VBdL53GzyaP/uDAMEPgeKpVKIoQYhes6"
    L"cN1sWuY4jqOcnEer1cLCwkLSFrUtuE4GpYESRkfHMDoyokAxiGwui4ybASUEMY/TNioIAWM0UeoiUeVCaRoCAkkkiCQgVAvO"
    L"HsKjFEQCRAoIKsGkpnCSXpI45mm0EULTbSKSRn4S5ZSy9Lo4joNyuYx8Pq80j7yqsq7X6brUi6KoU1UZtX6SVpNSu9FoIAxD"
    L"ubCwQF9++eUfcM6bjDGbc942GEA/TBB0MwCjFPNz88kfwxi4WpCEAYLeqDPzEAA4ZstXlTWXLl3E4uJCGqWu4yKby6JQKGCw"
    L"PIiBciKKmq0WFhcXIaSEbVnIZrMol8sYHRvD5okJbNu+Hfl8ARHloJSk5aQGp0UJBFgCBCUuhdoz6O2BaZAAJBnaEIJ2q4VW"
    L"uw3P8yClwEB5EMV8QYk/koKLJEs5YJalNEmikUqlEorFYup47XDTiVeb5/VrdJPHyPOwbRtUtZQ9z5Oe58larcZOnjz54smT"
    L"J3/GGHM553UAgXr4AEKVBmJD/ff0AShFEPhwHAeUUvA41svvXds2IIobjHafNJbepFqJSioBAtuy9L5lOqtvNpuo1WqYnZ0F"
    L"AGQyGeTzeQwMlDE4OIjSQBEZN4NabR1vvfUWpBAYHBpCsVjE6OgoNm/ajMGhwYQlslkIKUBVqkooOcmPhBG1htVpQV++SZT8"
    L"BW+dP4/JyUkMDQ2h0WgkpamIQWniUELNPJ+UEoyxrpJOU7vp+KvN81rd6+fpVEtpUlonApxBCJHMUNpt0Wq16NLSEnn99df/"
    L"389//vO/Y4yFnHNPTQKb6uEpIEQ9+b97GCSEwMjICEqlUpqvOrpBdu++GStWZovU7I33fl0vbhIjEihNUlQQhPB9DysrK1Ab"
    L"kXBdF4VCAeVyGcPDwyCUoaVY4tSpUyCUopDPY3h4OHVcuTyAXC6fXjRCSSIwtfMoTbZ5pVCDpg54d+zYgeHh4fR3er6Hdqud"
    L"gEoJLl1zU0pRKBRQKpXgum5K9aajr6aeN+le53qtFzTYeqO+3W7LdruNRqNBz58/v3L8+PF/ePPNN4+qKG8CqAGoq0dbOT9W"
    L"1H/FkbDFOcfw8AjKAwPwFRq7lxyN3npPV613N878d0cnhPA8L21g6CmhTimUWrAsougZiKIIa2trWF1dxblz51KWKA8MYHhk"
    L"BPlCATyO4fs+5ufnEQSBYpEBFApFbNu2FSMjIygWi3BdVy1VCFDK0u0aSgCuKoOMm0FtfR2VSkU5mYBSS23jqLKS0jQ1JcMa"
    L"eZnAu548r7eH9XXT6t627TSltNtttFot0W636fLyMs6cOXP0hRdemGm1WguMsZhzvm4434x8vkHe35gB9EW21Oy5e/XJXIXW"
    L"AxnR2ZNXiO/MAAiiKIRCK4Ig6JyYoTQFVoeXBYRQoFD1PGMs+ZwS8DhGtVbF2toazp47B8ZY6oyx0WSa5vs+CKVYXV3F+fNv"
    L"gTGGgYESduzYgbGxcWzetBmFYg6EJH9PEIbp+6VKBDKlrAnpOJ4Qgmw2m9byGtQ6119vnu8VeTrXO46TVENG1LdaLTSbTTo/"
    L"P7924sSJmTNnzvwrgJAx5nPOqwDWATQU/eu8H29Q9m2cAqSUYaFQcIqlEpzARxx33pg69aIcrjpynKfloXa8vjCtVgu1WhX1"
    L"eh2husiEUlVVKOEFAiIFQGiXcOyc6yCdFSmJdClSUzoUS1y6dAmLi4uQUqaAKJVKGB0dRblcxtpaBWtrFQgpUCwWMDQ4jJ1T"
    L"U9i5axfGx8dBGUUYJA7x/QCATOt5Qghc101LOkppF9ivN8+bDZ0UgDTZEdRsJYTYKOqPvfjii99vtVpvM8Y457yunN9QUd9W"
    L"zo96Wr3vuhZGXnnllSUp5dgjj/xPWatVCY85JITe4Uv/cME5uBDJkIZ3Zu1xFKNWq2FpaQm1Wg1xHHfTvCGkdFSrvXcQUCUS"
    L"EuGoO3pJR7BThYB0Pk8aMN1pxEw/+ohXoVDAxMQmDAwUkc3nYDGGMAxh2RbGx8axb+8+7N69B9u3b0epVEK9XkelUoXjJD37"
    L"XC6Xii8A153n9ffMJpB+3zrP67o+iiK0Wi2d68nc3NzyiRMnvnf27NljACIj6nWubynKDw3Kx0Zq/wob3LCiKPrF6NjofeMT"
    L"48LzWky6yaAESjBx5XDBO/WxlBKe52FpaQkLCwuo1WoK0cp5ZsVIOi1YXU/rtq1+r+ngRj298xxDRKKzYdNbkia9Cjt9nRBC"
    L"HQBNDs7k8/nkRPDwEEoDJVzkF7FwYRHPPvtTFItFHDiwH3/yJ/fj9ttvQ7VaTR1/o/K8GfUauLZtp5WXzvXtdjuN+tnZ2SPH"
    L"jh37nud5S0bUm7nejHoz11/T4RArCIJT2Wz2vo9t/5hcXFhQw4VkHMsERxwzWCzpywsuUKslO22Li4uo1+sAkI41NQiQ9uc7"
    L"J3jMo1yayrUz9b91k6kT4UiB0duIIuYZP40wCoDQ5IO6yFBLGmtra1hZWQFjFgqFPEZGRjA6Oor19XX88Ic/wr/8y3N45JH/"
    L"gR07plJB2JuvrzXPm5s7+n1blpXmetXNS6O+1WrRubm55VOnTn1vdnZWR71nOF5Tvn8tQm9D6ldnB6xWq3WkVq3x6eld7KWX"
    L"ToFznoCAU0jBYNvJetLq6iouXLiApaUl+L4PSiny+XxXmujK6VImw5l0Y0Z326iiewmqhjRSibHOKVpNBSTpaZJOx87UCZpS"
    L"eiuLyz7SZO6gn+v7AS5cuID5+Xm4mQxGhoaxvLyCv/7r/46/+Zu/Uc/xu/r315rnzf69BrmOes0wOtd7nkdXVlYwOzv7r8eO"
    L"HZvxPO9ST9Q31MPbIOp/o2Pj1vPPP3+oVCrO7/jYjh3j4+NicXGRWlYyzmy321heXsbixUVU1iqI4xiu68J13TQqNLUJIUGI"
    L"3sYhnaNWqjkE3VPril6AqHas3hzupAHa7VwQlTVI2qbtfg1JVX16Ypgmyylmjz8ZcjH1/ESjrFXWwBjDsaNH8dJLL2Hv3r3p"
    L"/EM79GrzvGaMXpGndyug9gTNqL9w4cLyyZMnZ2ZnZ1/YIOrrPQo/ulJX77oA8NBDD4V33nnn96emdv7HW2+9Vbz99ts0aeVe"
    L"wurqKjzPgxACxWKxSwmbzpdSQPBkGqcXPswVbMjEGZ1zcyStAilJIhTK6VKKZJ+A6D0BmdI8SQbsqt9PjOgiSAoLBSadfrQE"
    L"kaQrxaRtbg1GJDOGtufhmWeewW233QbOeZdz3y3Pb9S/16WdzvV6vbvVagnf93Wu/9nRo0d/EIahzvXr7xD14t0aO9d6zsMC"
    L"gItzc//nZMb9D3fecSc5/9Z5eeRfj5AgCNJxcO9iZLqbzvUIVvScl+usZEkQfdo3BYD2h1RlHjVP/BKjGtDHRUg3c8AQk/r8"
    L"XJdANE4OU0LU7yM9qxCdrxFCwVhSqZw8eRLr6+spA5rOv1Ke1w0dM+o13euxbRAEaDabUrVz6dzc3MWTJ0/OnD179pcAYhX1"
    L"1Q2iPuhZ67phzgcAcvfdd1uHDx+O//Iv//J/33ffff9+YtMm8dJLp8Trr71Oz507R1dWVlCpVNBsNhGGkbHeJdMFDSFE99k9"
    L"c+kyvbuGSM646ed2nwVKevoSKa0L43OiTkXqZJJ+jRpO1OWhSjWUdFcgGhy9Lequ1xKCOI7xzW9+E/v27cPLL78Mzjk8z9sw"
    L"z5tRb9K9mevjOE5zvY76N99882dHjx79xzAMlymlQgjR283rrevF9Qi9qz4cevfdd1uvvvrqf/Y8b/Pu3bt//5Of/KR12623"
    L"YW1tDXNzc/yNN16n58/PkaXlJazX1tFutxGEQZJvVThftpXbddKm++tm8ycNTHWWLhV3Hf6GSRtdka8HUKRTNah7s3SEJHDZ"
    L"nF+DLgUFSZiCMYZatYbTp0/j1ltvRbPZRKzazmae3yjqteNd101Htu12O416z/Po/Pz8xRMnTnzv3LlzvzCivtbTzfOMPr64"
    L"ntLumgBw+PBhAGAKfV+79957P3ns2LH7p6en/2D//v1Te3bvYR//+MdRqVRw7tw5fu6tt8ilixfJ8vIyqdfX0Wq2VGtVpI2a"
    L"jW6eYJ7oRc/nJhv0niuQRocQvZ+bAAAgjeogHfGn28KmwCSdDWVCupR6vpDHhQsXktu2BAHa7ba5dt3VD+ic4LUuy/WtVgvt"
    L"dlv4vk9XVlY2jHrVx9flXfu9yvXvdjjUUgBwisWi02g0pNoomfz0pz/96f3793/+lltu+czu3bs37969B45toVKt4sKFC3xh"
    L"YYEsLCyQ5eVlkuzD1+F5nip/1Gau6v93JopIReJlnWqCVEBJtYJDCdFngVKW6Lo7iBaI+mcbekCDROp2tNx4M84UilEcoVgo"
    L"4rHHHsOPf/xjVKvVtBnEOe8q7bTIM6Pe9300Gg2d68nbb7+9cPLkye/35PqNoj7sGd0KvA9GlPNTEORyuYxt29n19XV9uMAF"
    L"MHHgwIFbfud3fucTe/fu/dSuXdPbdu3aaWWzWbRbLSwsLspKpSIWFhbo2uoaWu0WaTQa8D0vHbzo0awu38ypISHq3g3k8ren"
    L"m0lp2UA6SxpdJx9J50BsQgJErXslzJDePop0xtr6XEGaNggB5wKtVhPf/e538U//9E9YXV3t6gr21vTJVnTaxkWz2RRhGNKl"
    L"pSU5Ozv70yNHjjwDoKLrepXrG0bUB0YrV7wfUd8LAKoeGggWAAeA47pu1rKsjDpqpBerR8fGxrZ+5jOf+dT09PSte/bs2b95"
    L"86b8yMgooihCrVbD2tqaqFSqslKp0LXVFRJEEdZrta4deIuxpC6nDL3HCakuE806v+tOYD0lXTcSutgB6g5h+rCpqR3Qwwq6"
    L"jby6uoa///sn8NRTT2Ftba1r/1DTveu6XYsaRq4nc3Nz8ydOnHhybm7uJQCCMdY2hjd6Xt+b64VJkO8XAKweuhHGPlkYJGYn"
    L"a2+Oa9t2ptVqLS8vL689/fTTpwAMABi+++6779i1a2rP3r3779i0adPE2NiYvWPHFNrtFur1OqrVKq9Wq6TVbJK1SoXEcYxG"
    L"o9E1UdTLF2ldb0SsOkRgtJO7na9vLCN7gJH6uUdwdk+ku6+3lFJt/iA5BMJ5OqI11+SBpEPabDbRbDZFHMd0eXlZnjlz5tkj"
    L"R478AECNUso3yPVmXR9v0M17X28baxm/kBvnx8y1YgbACsOQhWGoweDYtp0Jw7AaRVH98OHD5w4fPmwDGB4bG5v89Kc+9buT"
    L"k1t37d67+47h4eGBqakp5jiOpkhUKxW+urZG2p5HatUqkUKi1WomSxoi2T2wjC2czl04KdRJqK6+AcHlQ6MOKci036C7kUh3"
    L"HGWqNbSS4CKG4zhpOtCO1x1Qpm58oRS+8DyP+r5Pe6JeqqjX5V3zCgpfXFmZvH8pYKOvmQ9qaAVqpAkLgA3ALhQKGSGE0263"
    L"tagUAPIAhnfu3Dl1++2337pl85Z9O3ZO7R0ZGS5OTk7CcR20W200Gk2srizz1bU14nkeqdfXiZSA7wfGid7OxDAdHnUNG412"
    L"8gbTwrTt23PXkRRYupNIkqhuNBr4q7/6b/iHf/guoihKlmUsy2zooNlsiiiK6MrKipidnf1nFfXrlNLYqOubGyh8fiNbuTcq"
    L"BWCD7RHzRCkxGCI0wMAAWM1mM9UOCTnYrpRStNvt1tmzZy+cPXv2eQAlAOU9+/bt2b93774tW7bs2bVr197x8fHivgP7meu4"
    L"aDabWF9fx9LSEq9UKqTdbpNGo0GSDZnQuJWaKucoTQZERgcRRuew0/KlXem1FyS6ZZ0sxXJMTk6C8zjN9bZtd4m8IAio6uad"
    L"O3HixPcuXLjwayPX167QzTP38wQ+ILfpJdf4HNIjHokBBGpUEylDKO3ghGHIoihixs8YAFCenp7es3///n3btm3fs2fP7j0T"
    L"ExPFLVsm4bgZrNdqqFWruHhpSaytrUrf86nneyCG98yGkp4RkHTXv7t30DVJ3KAXYVkWlpeX8Yk778QX7r0Xzz77LBhjurRD"
    L"q9UScRzTlZUV/sYbb/z46NGjP1RRz4UQ5sjWjPrwRk3uflsA6L2pANkADKQHDL3lZZouHMdxLMtyoijaEBBTU1M79+/fv3fH"
    L"1I5d+/ft3zs2NlbePDnJbMtCpVJFpbKGarUq6/V10fJ8EgUB0Tfx1ouU0rg9bHJ+oLMnAPQMhmTnAJhtW1hYvIj77/8TDA8P"
    L"4/Dhw4jjGPV6XQRBQH3fx9zc3LmTJ0/OzM/P/1rpk5bK9e/UzfutKPz38jZxuAIrXIkdzIYTU2CwDEC4URTRKIos4yIVAQyM"
    L"jIxM3nXXXbftnNo5Ob13+uMjQyPDxULRLZaKaLVaqFQqqNfr8H2Pt1ttBGFIhJSEqQUDSpLjW3qbWbeEO6miU2HoHP9nf/Zn"
    L"ePHFF3H69GmEYahzPT9z5oyO+rqR69c3iPoPXK5/LwFwJTD0fk57eg69vQc7WS6yHaUhWBRFLI5jqi5kFsCgbdulO+6445bp"
    L"6T3bp6Y+Nr1pYmIns6zs4OAgsywLnu+rTeJakq89T0ZxTGAJQhkFBSOMUBDCYKUVRnI4dW1tDbfccgs+97nP4fHHHxdBENAw"
    L"DHH+/PlzJ0+efHJ+fv5lSqkkhLQ3iPreXH9VW7kfNgBcDTuQDRpQdIMKQzOEbVmWK6W0DEDon1EGUNi6deum22+//fbNmzeX"
    L"x8bGDoyOjm4aGRnJjI6O0aTaaKHtt1CrV+G127LZaop2y4ffCgmlhCQtXA/5fIF8/etfl0eOHJFHjhyhjUYjOnPmzE+OHj36"
    L"IyPXX23Uf6Ad/34B4J1+lwkEXIEd2Aai0pJSupZl2QCY53mWUa04AAYB5KamPjY5Pb3nwN49ewYnt27dvW3L1i3FYtEZHh62"
    L"y+UyQAka9Tqq1RqWl5dQr9fxuc99ToZRRL7z7W/j/Pnzrx8/fvz/Xrx48YwR9abQM3v4+hAmPuiU/9sEwDtVFtIAgskOpIch"
    L"NhKWlp2YI6W0DZbQqcNWTJEbHh4euOuuuw6MbxofmfrYVGlqamrn2NjYeGlgwHVsi6wsrzhPPf20d/z48adPnDjxLACPMSZU"
    L"D99cyrzpcv0HDQDXUm5upCFoD0v0pg7Ltm3Hsizbsizb8zyiQMEMNe6oyiO7adOmrJSS7d67d/Nrp09fWllZOccYswAEnHPd"
    L"xWsaPfwQl69j35T/Z5LkJnlvpqg07vbTVXJuxBKsBxS2BoWUksVxTHzfN39HBIBYlsXiOA6Mvr2H7tO2N12uvxkBcC09iF4d"
    L"8U5MwXoe1LZtiqSNiSiKeJjsvulHiO5z9rxngHZTG/mQvPeuu7f0MAW5gp4wHzCcqm+lop3Njakdv9kj/sMEgKupMnp1RC8w"
    L"yBVmIeb9dIDL77DZB8BNljrM++fTd7kGG83mP5Bt3D4AbixTbBTZEn37yILhI2H/HwoRXA3Ui9WIAAAAAElFTkSuQmCC";

static const WCHAR* USER_LOCALDISK_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAYAAADimHc4AAAXo0lEQVR4nO17eYwc153e9+p8dXRV"
    L"XzO8KVIyKVKWZdkWpKUpWz7kZL0O1kvHshZY2AT8nwHvGkiwChYwrHCRdQA7iBAkBpwEkQ/KBkwd"
    L"tEhJCCFzYVmWacqhdfDSkJ4Zcs6e7pme6equ7q6uqvfyRx3TMxySQ4r22El9nGZ3VR9V7/t+73e9"
    L"KiBDhgwZMmTIkCFDhgwZMmTIkCFDhgwZMmTIkCFDhgwZMmTIkCFDhgz/r4Gs9QncLB555BHxM5/5"
    L"zC7OuRwEQctxnA6AzsTEROeJJ57oAQjX+hxXgz81AYSnnnrqHtM0/5WuaX9pmMb7JVEUAdILGesF"
    L"QdD2PK8dBEGz1/Mavh/Ufd+f9TxvzvO8iu/71W63W/UYq7kLC3OVSsV58sknWwD4Wg3oT0EAcujQ"
    L"obt1Xf+MptHPGobxwYFyWdE0DZxzLCws4OLFi3CcJgqFPCzLgmEY0DQNiqJAlCSwkKHrdeF1PbQ7"
    L"HbRdN+j2PNfreg3P86qe5013Op0Jz/MueZ53yXXdsVarNTUyMjL7wgsvtH+vg/t9/vi7wdNPP/1e"
    L"0zT/QqP0s7qh3zdQLquqqoJzDkIIJiYmcOToURw//s84e/YsHMdJvyvLMnRdh2VZsCwLu3ftwr7P"
    L"7cPeD+8FkJg7B+cA5wxhyOD7PrrdLlzXRaPRgOu6TrvdrnU6nfF2u32x1Wq90263L3S73ZGRkZHp"
    L"F198cf5WjPOPSoAjR47sUlX1LzRN+6yh6/eXyyUakc7AGANjHOAcIARBEKDX66HX68F1XdTn51Gr"
    L"1lCZqWBqcgqTU5OYn1/Ah/fswcOfehibNm4CIQScc3BwRH/Jc4J4gxCAczDOEYZheoyG42Bhfr7X"
    L"arVmW63WaLPZPNdqtc40m82zjUZj+Cc/+ckUgN6NjHnNBTh69OgOSumndV3/K13XHigVizqNLZ1x"
    L"BoCAEAIiCNEzISBEACEAIQIEQkCExf0giMhjDEEQAgTwez5830fIWPoe4xycRcLyeB/nHDyaFuDp"
    L"DLkyPHDOEQQBPM9Ds9lEvV7H/Px803GcsWazedZxnDdardYbjuO889xzz00B8K82/jUR4KWXXrqD"
    L"Uvrnuqb9lUbpnlK5ZGgaBSEEIAJEQYAgShBFAYIgQhCE9Lv9ZCUk9r9e8rzsdT/Jy4lnIQPjIcKA"
    L"gXEGFoaLgiw7bv/+/vPqny31eh31er25sLBwqdFovNVsNk/Ozs7+1vO8i1/+8pfrjz76aMg5/8MJ"
    L"cOzYse2U0n+paXSfRrU9pVIxp+s6REmCIIgQxYjoxMr7B5aSdg3Cr9i3CtKvEHPZb4ZhiDAMwcIw"
    L"cn+IHoyt/PsA0vNPjIZzDs/z0Gg0UKlUUK/XZxqNxkVBEP7tt771rdd/rwK8cPz4bUVN+5Sqqvs0"
    L"je4tFgq2RilESYIkyRBEMXItWHmqJwNg1yP6Rix+tb+z7PthGCIMAoRBcIVhXA+JQVFKMTg4iF6v"
    L"h+9+97s/tSzrX0u3jO0Y//uVV7bYivIwpXSfptGPlAqFvKZRhIyBc0DVNIiSvEg65ysm4TdN3K0S"
    L"YJkYhBDIigJRFNHtdsEZWxUfjDHIsoxisYhisQhJkiDLMt7znvd88vDhw3feEgF+9rOfbbJt+5OU"
    L"0n2Uqh8t5PNFXaNRFhGECIIAIQdMMwdRFONM5Non/cciQD845xBEEbIsw2k4kCQJkiRedQySJKFU"
    L"KqFYLEJRFHDO4ftRQrBz586cbdufumkBTp48uV6W5U+oqvo5jaoP5fP5sq7RaLrGpEeDB4KQQVLU"
    L"lPyr4Zp+ew1c0IogEbmVmRkosozBwcElInDOIQgCCoUCyuUyKKVp1tTr9RAEAQghGBwcRLlc/ugN"
    L"CXDixIl1pml+TJLEz1FKP5a3rEFd1wAAjPWTvnjyhEShK/ADMMYgCMJVU7s18fUr/M7VQAQCHnJU"
    L"KjPodDrotNsoFguQZQmMMRBCYFkWyuUyNC3iJcmMfN+PAnr8OVVVkc/n77quAL/4xS8GisXiQ4qi"
    L"fE6j6sdt216vaxScMQRhRDqwrJkSB504JQcBQRAGcJwmDEOHLMsp6Wtt6ddKLReHEo3HbbVRma6k"
    L"VbdpGlAUBYwx6LqOgYEBmKYZGyRbYvXJdiJCXK2vW1GAX505UzQZe4hSdZ+iKJ+0c7mNhmnAaTQw"
    L"OjoCzjkK+QKKxcJijs6TE46qyP6xiKIAxjl6foBew4EiK6BUhSSJIH1p3VrMgJWsfjEN5giCEK7b"
    L"QaPRgOM0EQRRTWXlclg3OABd11EqlWBZFgRBuIL4JJX1fT+t3DnnoJRCEAQzFeDVV18tFAqFjyiK"
    L"sk9VlIdzOXOzaejxSYYIfB/1+QVYuRw4AFmR4fsBKFWj6h0cnBPwpYU9OKJiQxIFAAxByNHtemh3"
    L"PYiCAFEUIUsiBIFAiMcdkcbAWfSc5t3XIXEl0q8nADhPJiw4R9QXCgJ43R7anS46nQ56Xg8hi9wr"
    L"VVVYlolyqYRSqQTbtiFJkQtKSPZ9HyyuI5J9nueh2+2i1+ulLZEwDAUJAE6fPv1x0zS/J4ribTnT"
    L"gEAiIjudLoS0zCfYunlT3IcJIYqLvpwA6E/K+LIXiSiiQEAAhAQIGEcYMvR8hjbvpfNdIEjFEAhS"
    L"PxZZ+LLq9SrPKenx5yMh+91dtM1Y0rJg0ezsBfD9Hnw/yvcZC8A4gySL0GUVmqoglzNRLpeRz+ch"
    L"y/KSANvv55cT73kePM9DEAQQRRGSJKHT6XjSoUOHFEVR/sOWLVtuc10X8/Pz6Ha7AABJFCHJMqS4"
    L"ShUEElmtJEJkYtQyEISIuJhoHheMPPkXu6NFH8shEA5Z4Ajj2cE4EDICxjgCzsGiFlD6+yQ+dmQM"
    L"gEAIAI4r/CfnCFmIkLGotcBChCFPrTEIoxZDwMI4fvnwA4agF8TuIgBnHIJAIKkSJFGGLEUzVJZl"
    L"5PN5FItFJF3ZfgtPXE0iRkJ4Qn6/QJRSdLtdNJvNOenOO+98wLbtB6anp3Hs2DEwxqEocpqvcs4h"
    L"SRJ0XYdtWbBsG1YuB6ppUGQZoiRGQsU5sShKEGNRFolhqQX3l/HgHAI4CCI3wDhHGPtgzkniwwCI"
    L"AARAEKPqWYyEiXpGfQIB8IMQflyxBn4A3w8QBD64H0CAj5AB0WJZcnwGSeAQZYDIIgQiQhBikTkH"
    L"EQTkcrk0pQSwxML7A6zv+1dYfD/5vh/FD0opHMfBwsLCRUmW5T/P5XLiM888g7vuei+2b789mfeR"
    L"i+h5cN0W6vU6ZmdrGBoaQqvVQhiGUBUFOctCPp+HHS+EqJRCkWVIsgRZkqEoMiRJioP1YnC7wjez"
    L"SCBwDhK/ZgFHmLaLo0YdIRHhJJ0Z8b5Y8DCMBGBhLEIQtxDC6BH1dUJwxkA4hwQOiMmZkUjz2EBM"
    L"M3I3hmGkselqAXa5xS8nPokLmqaBEIJKpYJGo/GKRCl9YG5uDqIo4bbbtqNanUtbu0IcJDXNwvbt"
    L"JezceRdEUUAYBmi3o47fzEwFMzMVjAwPo93pgBACTdNg5XKwbTtdoaKUglIKRVGgKDJEUYosHbEI"
    L"SwJu/MzYYjCOX0frAn3vL9m/OLPS4JRiMTXu+29J3EpimqZpKJfLyOVy/QHzqgG22+1ek/gwjJan"
    L"VVWFruvodDoYHh5u1+v1w5Isy9trtRrK5RK63V4cCxc7ksl0830/tTRBECBJKjZs2IKtW2+HKIoA"
    L"GLrdNhYWFlCrVTE9PY3pSgXn33kHXteDJIkwTRP5fB6lUgmFQgE5y4JGaTpDeOzHWRpQl5EdC5OI"
    L"wPu204QgXjtILDYZx2oqcEppmtkkVfv1/Hw/+cuJT9xTskJHKYXneRgdHcX4+Pj3T5w4cUYSRbHo"
    L"ui503YiLqpUbpIuLIYtTnTEGzwtiUQgEQUKxuAHr1m3GvfeKIATw/R6aTQe1WhWTkxOYnJzE26dP"
    L"o+26ECUJhUIB6wYHMbhuHYqFAnRdhyCKSJYKGQv7SF4uwMrFU6/XQ7vdBmMhRFECpTQNnMvBGIOi"
    L"KCgWiygUCpAkadXE9/v5/hQ0IZ4QAl3Xoes6RFFEu93G0NAQzp49e3B4ePjfAQCZmprqDg0NqZRq"
    L"yOfL6PX8ONsQlpB+5fZq94mQpMiVRQsskSitloOZmQrGxi5jfHwM1WoV3W4XhmFg/bp12LhpEwYH"
    L"B2EaBkDIFUGvf83gClLDEHfcsR2e50HTNFy+PA4/7sH0Ey/FBtDfLEv8fJIy9hdWK2U2CfkJ8WG8"
    L"kKMoShQTVRWMMdTrdZw+fdodGRn5xrFjx55AHNrI1NRU98KFC6qqUth2Cb4fvAsBrv2Zfgh9qSXA"
    L"4PseGo0FTE5N4NLoCMbGxrCwsABJkrBu3Tps27YNGzZsgGEY8cyLCGBxW7j/9wkhsVsEkmQisf6k"
    L"WWZbFkrlMlRVTfeHYZgSvzzArpTZ9LucxCMkGaOu6yCEoNvtYnJyEmfOnHljcnLyb3/+85+/tsSz"
    L"TExMNEZHRy1BEFEoDMQC3Ky1r16AZNCRO0maVIgDPwEhHJ1OG9XqDEZGhjE8PIzZ2RpEUcTmzZux"
    L"Y8cObNy4EaqqpsuASdocHYskf+mxCCFpZqPr+pJzSAhNLD6ZBStlNv3EJ2IJggBKKQwj6g8FQQDH"
    L"cXD+/Hl26dKl74yOjn7jrbfeWljOARkbGxurVCpbul0P5fL6uF26NgIkryOLZQAia5XlqFXh+z3U"
    L"alVcvHgBFy9eQL0+D8vKYffu3di1axeKxSK63S4ajUZaTCbHNQwD5XI5bZYl7iYhdLmfT0jvdzdX"
    L"y24Sd5N0QLvdLiqVCs6dO/e7ycnJf/Pyyy8fvWLwiQAjIyO/6nQ6e6amprF587Yl2c679/+rEaCf"
    L"/KViREE3TKc35xyiKMS1hYhut4OJyXGcO3sWIyOjUFUFH/jAB3DffffBtm3UajW0220UCgXYtr0k"
    L"I0pcy0p+Pslsku2VspukQNU0DYZhQJIk+L6PZrOJCxcu8EuXLj05Njb29d/85jeVq5EPAFKv1ztj"
    L"Wdae0dFRELJmV+itiDSdjwVMiOt2u6nvz6+z8C82fxqc+JieruCt376NJ554Alu2bMH+/fuxY8eO"
    L"tH28PMAmVr+azGa5u9F1HaZpQlVVBEGAZrOJarWKCxcuDFcqlcdeeuml51YzRvLmm29+acP69T84"
    L"9dvfYtOmrRAEecmgb5X7uZkZ0L+9WBP0v8/RlmbBAwIuBtC5DVO1wRHi9ddfx+uvn8Q3v/lNKIoC"
    L"13WX+Pl+4q9XwS53N7lcLo0h3W4XjuNgeHgYExMTT05PT3/91VdfnV6tkUmu677S7nTm169fX5ib"
    L"q2HDhs0Igv7e5h/y0qHrzcCl7wtEQGPWwYX6KeRzJQyEd8DQOlAUCQ888Gfw/R6eeuopfPWrX0Wt"
    L"Vkst+GqdykSgJN1dKbvJ5XKpu3FdF7VaDb/73e9GZmZmHnvhhReevdERS3v37r389ttv/2z79m2P"
    L"/OpXJ9DpuKDUQHQdzNWwXJSbFWn1hPd3GNKjCkB92sFss4b56gKGp6qgVEOxWMSuXbtx//0P4Pvf"
    L"/x5qtRokSUpnwbWIT1zN8mLKNE1omgbGGFzXheM4GB0d5ZOTk/9ramrqGzdi9f2QAMBxnCcaDWff"
    L"7t27pXPnzmFwcD1M0wIh/Q20W4+E1Gt0Ca76neg1R7PRwoZdebQmQzQZR6/n4cyZ05iensajj/41"
    L"Nm/ejDfffBMf+tCHMDExsSTDuRrxibuRZRmmacI0TQiCkKa71WoVo6OjQ9Vq9bEjR44ceTccCADw"
    L"4IMPnqhWq/9R13W87+670Wo5GBsbQb1ehe934vw88eOrFSRufq1avxtPAJKCbHBHDvYmDaIgotFo"
    L"4Pnnn8fRo0fRarWwbds2DA0NQVVVNJtNOI6DVqsF13XRbrfRbrfR6XRSUYIgqoOSesGyLHDO0Wq1"
    L"UKvVcO7cueDtt9/+zsWLFx98t+QDWFzTePHFF//9ww8/XC0UCn93++237wjDENVqFVNT4wAIcjkL"
    L"uZwFVdUhCNJVWwEr7V69CNfCCldSMA7GA1CqYcOGIt6qvYHnjxzB3NwsOOeo1+soFks4c+ZMasGt"
    L"VuuKlDJJKwkhoJSmQTapZBNfPzo6enp2dvbvn3/++WO3YkRAnwAHDhxgBw4c+G8HDx784datWz+l"
    L"adrf6Lr+ULlcKvZ6Pupzc5gYvwwOwDBMWJYNXTehKBSELO2gXqtPszL4MleUrsT0bV9jEIKEOX8e"
    L"zzzzDJxmlHK6rotms4lisZAGU845XNdNC67E3RBCIEkSDMNALpeDLMtpkHUcB5cvX/ZmZmb+69TU"
    L"1D+98sorCzc4uGuf+/IdX/ziFx0AzwJ49vDhw1ssy3pYUZTPqqr6YTufHwDnaDgOpqcnEAQBFIXC"
    L"smxYVh66bkKWFUQLYjfirhaxupiw2O9XJBULQRVnvZMp+QDg+z7abReCIILEzTxCCDqdzpJuqiiK"
    L"oJTCsqz0IirXddFqtVCtVjE+Pn5yfn7+seeee+4XNzyYVeCa1wXt27dvHMD3AHzv0KFD603TfFBR"
    L"lE/LsvzRfD7/Hkopup0OGo06KpUpCIIAwzBh20XYdh66bkCWlTSYR8TeWKp51U9xDlmWcfnSOEaG"
    L"TkIrixAkAhZE30/vDwCHKEpp68HzvLTXpaoqcrkcDMOAKIrwPC+9Q2ZsbKw5Ozv7raGhof986tSp"
    L"39ttSqu+Mu4LX/hCBcAzAJ754Q9/aOi6fq+iKJ+UZfkTiiy/38rl8gDQbrcxOTGKS5cCyLKCXM5G"
    L"oVCEbRegaQYkSY4FAW4uw1oUSBRF1OcW4F6Yw46B9Us+laxRMBa1LwghaWqpKEqa06uqijAM0Ww2"
    L"U6ufnJw8vrCw8PfPPvvsGzdxgjeEm7o29Etf+pIL4LX48Y8/+MEPtsqy/GeU0k+IorhXpXSnTanC"
    L"whBt18HsbAUs5FBUCtvOo1AowbbzoDRaqFiMH/33BeC6M4Zzhl137saxC8ex9f1Bav0AIElSTG50"
    L"GUji13VdRz6fT4Nsp9NJrX5iYqJWr9f/8fz58//91KlTV72r5VbillwdvX///jEAYwAOPf7448qW"
    L"LVt2Ukr3iKL4cVEU71cVdRvVNJEAaLsO5uZmEAQsLutt5PMF5HI2KNXidYLkmiNyzZgQBCHe+767"
    L"cOh/AO+8PAMiADwu4lWVQtcNeJ4HVVXjBSERAwMDaf+m1Wqh1WphZmYG1Wr1udnZ2X/46U9/euFW"
    L"cLJa3PL7Aw4cONADcCZ+/M9vf/vbhmVZdwqC8GFJkj6iqup9VFVvK5VyokAIPM/DxPgovF4PgiBC"
    L"03TYdh65nAVN0+PFexGMIW5RLyIMQ2zctBEPvO9jOHHqZdCChM5cdK2qaUYZTafTRj6fBxAVVoQQ"
    L"tNvttCaYnJwcW1hY+PrBgwcP3mouVoM/+D1ijz/+uGnb9k5VVe9XFOVBSumHDF2/3bJtRZHlKFeP"
    L"s5AgCCFJcpweWjAME4oSBXXOoyAbsgANehkz7cuovtPCpf8zi3eGzsO2bXzta1/D6OgI1q9fj3vv"
    L"vRfHjx9Hp9NBq9VCpVLhc3Nz35+bm/vG4cOHJ/7QPCRY87sk9+/fT7dt23a7YRgf1DRtj2EY95mm"
    L"udO27XzONMERBfb43l2EYbTQrmk6DMOEbBKElgNZUAGZIeduRNjj8HpdiKKA118/iUceeQRjY2M4"
    L"deoUXNfF9PT0+Xq9/g8//vGPn1/r8a+5ACtA+MpXvrKpWCzuNk3zPtM077dt+27LsjaXikVV0/W4"
    L"IdZCo+Gg2ZlHp+OB9Qi44kOVNKiiBkmSUa/PgVIVn//853Hw4EGMjIx48/Pz33Ec55+efvrp+loP"
    L"FPjjFOAK3HPPPcaePXtus237btu2P2jb9r25XG6nbdsbSqUStXIWRElEGIRod6L+juu6UBQFe/fu"
    L"xS9/+UscPnz4167rPvajH/3o1bUeTz/+JARYCQ899JB5xx13bNI0bYeu63cahrHTsqw78vn8ZtM0"
    L"S7quG6qqquPj4/Ovvfbafzp79ux/+fWvf91Z6/Nejj9ZAa4Bes8991gDAwN5WZaNZrM599prr42t"
    L"9UllyJAhQ4YMGTJkyJAhQ4YMGTJkyJAhQ4YMGTJkyJAhQ4YMGTJkyJDh/x/8XwRt0+HdMdROAAAA"
    L"AElFTkSuQmCC"
    ;
static const WCHAR* USER_DRIVE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACx"
    L"jwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAdmSURBVFhH7ZR5VJNXGsZxrMfOcMaRRqVahVpQ"
    L"q4Jap+OhLCowCDYMiEgRIhGCQVbFQiSEJWwNYV+UCIoSIAlbFIoQ0Qg4slrZVMQFBEWhLB5R3BCR"
    L"Z25iOn/0zEx7xvY/f+e8537f+e79nvd93nuv2nve8573vOfnaH3wgSFjwZwTiSsoVwTrPu5lr6RU"
    L"6X/0xx3k08y3M35fZiYt12y7sfkzXN++DLfpK3CfuRKNTsuf62rNpc3+y+ylqnk/MYPEvLePvwEL"
    L"1GetqTHTmeqiL0dP4GrcjdbDSLI+CnbpXNrhta4oRGT48itbrSOL9SnemmspQZ98rXV2pfVnVarl"
    L"786WxXP8uukr0ctZjQeH9DEkWoMX5evAttPODxZsu1rQ4oCUajPsExvDJs0IRskm+PSrxTzV8ncn"
    L"asPH5Y+D9PFEsAYTp9fhVdNaPKzRe+NE1Uk72cR6eeqyN7Iv2CCy1BgumYYwjzaAOkV9s2r5u6FO"
    L"OiDcSOlIs5hf62++KJ9lScmtip7X2SDQ7qG5mmS19qejqoMDSQMdKWfM4CsyxlaW0URpaeklmezs"
    L"uZKSktSCggLrwsJCPbFYrFFcXDxTEVwu9w8AFHvlfzNLXWP1vL/TS5f6Cpq3pci6mYe/v+Sw2ydT"
    L"b/Ec2oEo+6q+8Tw09cSivN0Lx+q2glNhCgcvm+ngIPYUEZk8XV72tLGxYVAmq+yVSqUykkg8SYhD"
    L"wkUkEumrZP47czbSkj862gPzol6EV/+IU90v0fIEcHR1S3d1ta3xDqaNxR73nYrJc5tmZXwDT64D"
    L"CoskuFBbi5rq8+i/dxePHj1EX28vuq53Tre1tU40Nze9kJ+TPykrKxs5efJkP3HkhkgkKRWJcrcr"
    L"nFFJK5mpSY/vmJvSivXZV+Bb1oOsy6Ok392vI7j7Xry4l4fTkhjkHY2AUBCIUwJnnORvxeDgAF6/"
    L"eY3nL58T8Uf4cXAI9+71486dO7h16yauXetES0sL6usuoqamGufPn8eNGzffnD1b2ejpydBTaRP7"
    L"585fOz+wdEoj5iI+yWiBrbgL3NphBGeVovhEKDAmBobSgfuBQI8lcE0P0iQqhoeGMDDwAH13+9Dd"
    L"cxtdXV24dvUqrnR0oKO9HY1NTSgqKoJEIkZrawtGRkYwPT1NnltfBwUF+yjF5fJLlG3MA1XqIReh"
    L"EVkHjdR26Obcgl3ZAMLTjuJyGRO4x8HkDSamOq0w1ayPN5XLkJnExu3uHlwnoh1XrqKltQ3NzZdQ"
    L"V1+Ps/JqSIpLkJp+CIczMtDa1oZXryYwPDyMmzdvoqKiAh4eHhxlAuXllXJTt8DpZYxYWHtzsDMg"
    L"ChsCD2FhXB1iEyPxqt0C050WmGozwGT9aryuWI5egRakp0rRc3cQt3ru43L7dcgvNKDizHmUnCqD"
    L"SFxEKi9BQ30DRkdHMTY2hg7iikQiQX5+Po4cOfLGzs4uxsHBYaZaTo7waQgvGTHHpAg9LEZQqhCh"
    L"6XnYG8SDn88O4LIe0PA5cG4Zpkt1AakOTkd+ibqL/0RjQwPZgDWQyc6QkClDLpcrxcbGHmF8fPzf"
    L"FaelpYHP5yMrK0tRfaeBgYHfrl27dNVyc0WP+Zl5OJAigmvUMTiFk4gWwiVeDIeoLDjupUPAMUBV"
    L"LOl72BpEehrCZ58n8gvEkFXKSALVkJ87h1qSCOktRkdG8ezZM3Ia+pTJKAR5PB4SEhIQHR09aW9v"
    L"f8vKyiqQRqOtJzFHTSwuHI87lA0v/gnYsw/DyIOHJfYczP8mHIvcYqDrl4TVQen4IjwDX4akYAM7"
    L"AptYB2HDZsE9IgR+UVwcyxVidGgUT8efYuD+ALG+Hjk5OUrhiIgI5chgMB6bm5vnEFETZ2dnbWX/"
    L"FZSUlH+fmZ07FZ4uhO1+Hj6l+mChLQuajiFY5BoBLS8+tPcnYql/HLR9uOQ9EEu9fKDvvw/WIc5w"
    L"i/CC47f7UVYtx92+PhQWFCqrDQsLQ1RUlGKc2L59+1Ui7k0s19u9e/eHKum3sNlsire3r1BaLh/g"
    L"ZxY/2xOSMG25Nwxf0AKg7xIEPW8eVh1IxOcHk7DULxKLPL6Fpps7dN2ZsEhaAcc7H8JuBwP+8THI"
    L"z8tDMIeD0NBQpThzD/Phpk2bjiuEd+7cuUgl+Z8hfZlPpdpaBgQclGYJi/tTsguG2anZr5i8w7AK"
    L"iIShfwT+6h+OVb4sLFM44OULavLfYFergY2GX8Mt5KDSbnLDgRPMUVTdYW5q7kEsX0E23SyVzC8y"
    L"gyQy28zMTHOLickSBmNPEJ8f94MgW9ibJpSOco/mT9Kj40AN52IziwVqiBP+kbkK1g40sBLjkJiQ"
    L"CFdX10FjY2Oho6PjSlL5AtV//z8U97W1tfWfLC0tF1pYWBi5uzO/i0tM/+GERNodf+z4g33c6Mmd"
    L"Pr7YHXgAvIT4CXKu20w3mjLodLrOz+/63wKlO1QqVWPLli1LTExM1ru77eVHhn1X5+XpWW9qZppB"
    L"Eljl4uJCUc3/3Zih6KmTk9M8IqhFYgO51QzIqG9jY/Nnxfe3034Namr/ArSri0nQRZPAAAAAAElF"
    L"TkSuQmCC";

static const WCHAR* USER_CDROM_DRIVE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACx"
    L"jwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAhzSURBVFhH3VZpUNPpGceeTmfaqeO2nR5rP2w7"
    L"ne5sv/ihY6dSRx3FqY476jjqeoKrtWo/rLqrsFCvyoDcSEQIkJCDHP+QhJg7hCQkQEJIiOE+BDyQ"
    L"cApBBIHw65OY6dbesNsP7W/mnTczef/P77mfJ+b/BlKp9GsavT5er9drrLW1s06XC+Fjttlm1Wq1"
    L"RiyVxoffRJ9/sdAZjXFGo7GzoaEB3d09mHnxAn+NqakptLS24v79+51CoTAu+tkXA7L4sNFUjXoi"
    L"7+joQGAogPn5+Sj1mxgeGoJGowWHwzkc/fzzQW8y7dDp9dDrdbDb7WhubkZPTw+CU8Eo5Zvo6xuA"
    L"3WGHWCIBm83eERWzMly9evVLGrWxUypQQsEoYTAYUFdXB7/fj7GR0SjlZxgZGYbT6YRWqwWfz0cx"
    L"m90dlhEVt3zo9eb4ysZEsKuPQFDOg0qigEVvgLuxEb29vRgeHkFwcjpCvrgQQpPbDblSieISNnLz"
    L"8lDIugvWvXvxUXHLh9ag0vGmvo1ExOCUZw/+KLmDigoRNGoNTCZrj7PJ42psbmnw+f2t4eQUiiuI"
    L"tBBlLDZkbBEqS0S4e/euLipu+SAFZtUt6WAPnMSZjo/wKzMbG8tV4Cu03W7vA1W1xSHUG82iOrdX"
    L"pdYYPKJiHqpKpFCUSsAv4oJVwEJOTs5sVNzyodMaUKN3wqpvgKm6BlxzNZLlFtibHtRZbQ5eTMzq"
    L"dfRsDVOp5DibfdYKriBYkJOP7LxsZOZm052D3NxcvJa2AsgVCvDK+Sjn0injgldSBoYneOVv77Yr"
    L"1Dpu9FmMXKnKd3n9dpFQNJaTmY30jAykZWQhI4OUyM5euQIMI5vNZxWg4C4LwjIeTFIt6tU2ynSP"
    L"q9bdVCVi5PlKtS7NVueqstodjglnPyat3fDozdDKKsBh3yMFslYeAvKATl+pgVtnh9/WBI+9EUq5"
    L"Ehwu76XT5bV7HrRZnO5ma4O72dY3+Gx8LjCFUPsIMB6guujByKAO+fk5K09CaWVlfI2uGg21DhhN"
    L"eghEQmRmZ4FaLVyuRrS2tQdGRiNsoUgtEkbGRjDU3wMstKLBJaVQpC2vDEnGKovF8hUaKj+srGTS"
    L"GYaZlTJSlHE4yM/NQ2lpCeqpGYVPR3v7a9a/wfzSErxeL1isguU3IiLeJJWKimUyxqXVqCfLSkuW"
    L"SkpLcedOAYqKimGgRhSueYfDEWlGCwsLUdo3IZUxSE29/Z+1YiJdxzCSFJlM2nT/vmrc52uen5wY"
    L"xcTEGFpbW2i4aHDvXhGEFQLY62tgtVrh8Xjw5Onjv1NgZmYW4erJy8vL/ZfWWyyc1TKZ7DzDSI0a"
    L"jTrgdDbMDgeGMDc3h8XFxYiwpaUQQqEQFkIL6O3rhUQmhogRQKfVo6mpCUODg1iMTsWZ6RdooDnA"
    L"4XJJQduSwaCpP3Mm4b0o3WdQKBS/lcsrNUolM2azVs92drQtDQ8P03h9bcniYgizsy8RDE5hfHwU"
    L"w4EABgefon+gH13dnTCbayChScclIr5AQF4RgccXhCdfeBfAEI3jJcoB8tDC5cuJ5yKkJpNrLU2o"
    L"m2q1KlBdbZpr9rpD09PBiHWvXgbR29MNr8+Prp5+PB0M4NnQMLl3EP39j9Dd04s2SjbfAz+aPF7q"
    L"AS4atw4YTGZUSBnk5t9BAYsFDyXdq1dzNKCG0dnZCdqQcPr06aSIAiqV1lxWVhri8XigrI6M1UaX"
    L"E+1tLXj6eACBwUfoo4QKLxstrR3wt3WitaMHXQ+foHfgGbp6n8Dd3AaTtQ5qKkuGeoFAKCFPMKhz"
    L"1GF0dBTPnz+Hz+ejYVURGcWFhYWhPXv2/Gn//v1fjikt4cyYq22w1NRCpzORAmaoVHpqszS9Ctng"
    L"8wSQV8phNOhhr7XRuHXC1+yl2xUptxpyu1ari8z58DGZTBGy588nKFTBv1hMiYe0tDSqlqKw9a0b"
    L"Nmz4w5EjR34Sw+MJgvX1TdRAGNpqwskTxNjEAsbGQxgKTJKbH8FibaAtppLqnIvM2xlUwyxIpRJo"
    L"ab2qMZthMhphIUXC2T9Ky8gL2gn7+/ojyoQJU1NTqfdn4ObNm/P79u3r2rFjx6XDhw+vp/OtGKFQ"
    L"FNRqjbidkYdLHyfj4qVPkZaeB7FYhUZ3K5UOaMUCKbSAh72jFGcfFEoNKVOO3II8ZGXlQCIWYzQw"
    L"iungNAafDJLrHeG9L0J87dq1yJ2QkDC5detWDpHGfvDBBz+OxD8MhlFV8XjCBSlZaK6x0VrlpdZa"
    L"R91NgOs30nDhQiJupeagqJgPpcIEZ0MLtVo/HHYPEstO4uOM80hJukYbTxMG+vshFokj1qakpODG"
    L"jRvhe27v3r1+Ij9LLn/v+PHjq6PUr3HlypW1Z8+e5xoMNY+rqrSTPJ54SSpVUtkYaNP10SI5CpvN"
    L"g/JyGa5dT8f585dw4WISJDw19knewoGHq3FwewLYtGrxKZETk5KQnJwcIT/14amxTZs2lYaJDx48"
    L"+IMo5T8GxeU7O3e+H3fx4icysVj5UKXS9Ekkihm5Uk3ZK4VeZyWr3VQh7RR/NaqVjTjK/BR7LGuw"
    L"/p1Y3E6/HXE3dTgkJSaFrfZt3bz1NLn8Z5R0X43S/FusIkW+vmXLlu9tj419OyHhw8vpaemNZWWC"
    L"Lrlc/Uyp0L4S8iWg35Bwq3Cu+H3s5v0Iu7ftA5dTjsyMTJw4ceLZxo0buQcOHPg5Wf7dqNyVIdyv"
    L"d+3a9Y24uLjvb9u27dcnT566lZWV7xSLFd0CPhO4k8me+ejcJ0j+NBm5tN9RXXs3/2ZzwrFjx975"
    L"XCv3P0HEOzt37lyzffv2t2NjY9efjP9d2vWUW/bfnznj2LxlM4sUePfo0aNro+//a1gVjumhQ4fe"
    L"IsJ1dH5JXW0D3b/YvXv3N8P/v372P4GYmD8DNdUV6E1bl94AAAAASUVORK5CYII=";

static const WCHAR* USER_DISC_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACx"
    L"jwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAnTSURBVFhHxVZ5bJPnGc841FXQIapqFYyqHD32"
    L"1yaVrhQ0aS2sQuoxqbQ0LYVSqpZBKVKBamo7WiDcuRw792HHSXwQ24njOPFtx3Fi45BgjpALQsIR"
    L"crDcCTmc2L89zxdM2xUmJk3bK/30ft97PL/net/3iXrYZjKZ/kD4sqysTEEIlJeX99J/iHv+53Ge"
    L"53V3t/x3mtlsfsNisZQ5nc6OxsZG3Lp1C0NDQwgGgwiHw0LP/zzO8w6HY0JXpLukVmv20vY5hF8I"
    L"gv7TRpassFqt9urqajQ0NODGjRvo6enB8PAwpqen8aA2NTWFjo4O2Gy24by8PLNEIvk9iZs1I/Uh"
    L"G5H/kQQ01dT4EQgEBMva29vR3d2NwcFBwep/1ybujGN0dBR+f01Qo9E0Z2RkvEViH04JJrc7HKis"
    L"dMPj8aCurg719fVobW0V3DwwMIDJyUnB/fdroVAIt8lTrHRLSwvOBgJhnU53XZKa+gaJnz3D8oA2"
    L"43ZbyGIxw2QqR0VFBXw+n+CFpqYmXL9+Hb29vRgfH79L9/M2PDSMq1ev4sLFC6giA1wuF9wez7RC"
    L"qayPjY39HdE82BPlpSaX0WAEuQ1GoxGUA3C73eRKPy5cuIArV66gq6sLd+7cESz918bxv3nzJi5e"
    L"vAiv1yvsLyoqEuQZSo1DOTk5p4jmUcLPE5Oz3eQogSxLDlW+BjpN0T0lOBS1tbX3kpHje7/G3rnU"
    L"cOkeObkeBQUFSE9Lg0qtQn5B/q2UlJQtRMen46eNzrNc27IVUvcu5KpSoJQroVXrYCgxwG6zoaqq"
    L"CufPn6cwXEN/Xx/6BwaCQ8PD947DxMQkLl++LJCTMdAUaiCTyiBJSUasKB5xibHQF+vH5XK5jejm"
    L"E37wAl8eZm8ScqfmITW4EIktb0FcHgOpIhsapQrGomI4bXb4vD72Rq/D6QrYXJUuZ2V1RV0gcLbj"
    L"VkcfK3b6tE/wmjwvD0kpYoE4WSyBIlOOkqxTuFTuR2FhYfeJEyfWE+0PCUkKfGlq/BrSqScQjwX4"
    L"W/C3+OvVaOxyHsDhwkxSJB9arQZqbVGbSqtXF5ea4o1mx/ESk+Voicl2stxqzzeZbW2F2sKQJCMF"
    L"SWIxctNyYJBpYMsrhSWvBAapBsVSNaRSeRcdy2NE+8sZdmp8jTpq1dDXH0D+5a8Qd2U3djftwwb/"
    L"MbxWloe38gzYn3mqW1uoU5gcrpgqfyDBX3chrbomkGF3V4vKLM6jSp1empOV01OUrSJSCluBEWVy"
    L"HVmfh4zUdCSKkxAvSkCuXD5CYXAQ7a8IM2Hgu7zScRpupxcudyWslZRAHiPSKg34yu7ABpVv+mCB"
    L"yeu02k94awKJyanph/+0du3mN9/8y7aUjCzygEOiN9uP61X66lKZLqjJUiA7LRMisQhxFIZE6ulG"
    L"hITDoVBM0KloJ9onCDNHkh8Us9kCi9kGm9kBp9mJCouL4u6EyVGBPINtWKU3lXh8Z46XmuxJb7/9"
    L"9kbatpSwJHpjdLRKW5RkqfAmqDXa4kSRaDQ2MQ4JSQlIkiQhWZI8Q04QS8TIzy8I6vX6Udq7iDCT"
    L"B5QDIZVKhRkooVQooCxQQJFfAFmuFOmZaQOFer3GU3PumN5kT169evU62vZrwqNbtmz5s77MkmB1"
    L"VYsKCnVFZPFIbEIc4hISkCASQZQkhpgsZwWSkyUkL3eKEjVMe39DmDmO7IH09HSkpaYhNSWVjo4E"
    L"Ccm0OSUJ0owcGHP1kzaj02TzVMdYXVWSr/9+4KvtO3e+unP37jXZcsUei9OTpDUYY6vsPmeD+ey4"
    L"R2OBTqZEVlo6RKIkxMYnIDYuEXHxiZDJ8ybvemAJYa6gAOdAjjQb4jRyU2YypDIpytQlqDVUocVx"
    L"Hg3uAKxGW7OpoiLe6vYcp2MgkSkLT6qL9McMZkeiwWRLyNdqJS1n6ttxri88fbYXQ4F2dNLlVe+2"
    L"oLJUQ8dZhmwykmSPUg60Ee1iwowH+BToS4pRqi7GGYMHDdZaNFYEcL6qDj6PFya6WHLluUF5gcJb"
    L"bncftzgrj+qJlIkL9WXH1Dpj4qXmlrqujs6pzsY2DFy8DrR2ALdvIDzShInhWgzedqLnlomSMKsr"
    L"NzfXTrQcwpkk5HvgXGUdmixncM7lh7/Ki6pKOg12K7TFOuRIc8iVIiGDq72+htNnzurdPn9BdfUZ"
    L"ZWNLq6G7p7d5cjI4OUV1QvftHlxtuYJrdG0PtjfQ63SR7sk6gotQQ/dA+hWxWBxPtAsJM8eQb0KX"
    L"wwVvVTW9gG7YHDaUGg1Qq9XIzMykJBKDLg/hTajx+8M+n/dOW3v7IF3HgxOTk3f4Ko60aXqk+vr7"
    L"0HbtGlqaGnCtJYDBf5ymGTs9Vn4olcrre/fu5VM0TyCPNH4LTtOrZygpQXFxMVuLNHpEOHtTqacw"
    L"Cc9y5GFqb28TipP7vYo8NjIyItQPly+3Uj0RoLfiKp2wrGsymcxKdD8kILU5a9eu3bFhw4abmzdv"
    L"xqYPPkB0dDTee+89bNy4EZ9++im2b9+OnTt3CtixYwc+//xz7Nq1C1988YUw9yB89tlnwhrGLsIn"
    L"n3wy9fHHHw999NFHPXR8uw4cOJASNXfu3JWvv/760MGDB/u+/fbbsf379+O7777D999/L+Dw4cOI"
    L"iYkBzd8bp404evQo6FHByZMnhZ7XMI4cOfKTsRMnjgvf33zzzTTtC5KcIeIYYL73339/JGrevHmv"
    L"rVy5cogWDO3bt2+gs7MTjP7+frBHuO/s6hRKsq1bt4IswLZt29gaoThhcJnG1jI4RJGxEgonj7EM"
    L"kj9FHu175513uqmeCDHfiy++2M8KrFu6dOkIWT9BpdQUlU1jTz/9dHjZsmWgXii/li9fhlOnTgnl"
    L"1YoVK/DMM8/g2WefFUqv5557TqiaqIjF888/j7a2NqGPfC9ZsmSKi1nKseDChQt7SP4I/zPfU089"
    L"NRy1YMGCdTQx9u6777KVocWLF48SxiNKjI2NgXsGC1y+fLkAVoRLdB7jcv3QoUOCYrye+8g3yQkx"
    L"YUQ2eSfM/8xHxo+xB159/PHHg5x4zc3NnGjhWbNmhRizZ88W6r85c+bQ+ZUKteGmTZsEF3OS8npy"
    L"o+CBDz/8UFjHmc/7GDxPcgRC2hdmhdavXy/8M9/8+fODQghIgWmOKWkVZgG88e7mEMcyMsZCmSQC"
    L"HuN+z549ghI8Hx8fL6xnZGdnC8YwIfeswAsvvCD8Mx95PxT1yCOPrCOXhl555RXBmv8VmI/CGmYF"
    L"llNmt65ZswYvvfSSgJdffhn05ApYtWrVve/I/I/HIojs4bkfz/M4/0fmf7yObsQBvoieeOyxx1Yv"
    L"WrTo4JNPPvmwiCEcJhy5C/7msUOE+62/H3jtKuEq/P+1qKh/AoMLBd6SoewcAAAAAElFTkSuQmCC";


static const WCHAR* FOLDER_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAEXElEQVR42u1ZTWhcVRT+zr1vJpOkmYmJ1mKTUBHTxojaFktIhbYK"
    L"RSmk3czowo0bQSnUlRUXxoAUXIgQdCMoiFDKjCCIdeFOwSzURdVFwVZCE63Vmkgyk0wy793zuXgTze/8NNV5iznwNu/Bu993v3O+"
    L"e955QDOa0YxmNGMbIdWek/W/UQA2nNno6KgRkTLH+q5sNm0brYAg3EXzwmNIXZ8GA1ZWyxOwPQk5/yOWABRJiMh/r4S3/gY5akTG"
    L"9MK5Rx4d3D/4fku8dbcfBACkIgGS8Dyrr4kuXrn88+siX33AbNoindNt73KFjZC1IML0zfRK4uw7z1w6+OSxfhRKgJHaVlIFWhOY"
    L"+v4HfnLh870vjU9eKb95WwRWNmIzIt5Gtoan9qGtPdmxq/j7nC7Oz0OMrYkBSbTE487zPA+2+ODJI7yZvAN29k/UrYKXBzs7gV8L"
    L"oGRyczWnEEC0J+HfmLrmvCBvlpdLNEaknrVVlceOHvrw8aN0UjazW6xQxqxxN/7467NPv/ju5bfOF2bWp9RaArm0AXIu8+zQ6YcG"
    L"7u5MppKOpK3Ncf/dAIGItaajLtqbKhouu3fwnueCgLtFvnyKHAUwtpFAmP85HRlGR1/vXWe6dnZJYb4o5hZRsHR7HEhV0daeCAYH"
    L"eo6/+0r/QZGxb7PZtM1kcm6DAiLg8SHEPGvjUMCznhgjt9ui61SBIGmtNWyLtSSr1sCd3SkagRYLeczli7DGNPRAVSoSiTisNUIj"
    L"NbjQzTl3/bcZY9t3wQ9aYKSh+EEAsuiwODMJ0cLy1gTKBXwqc+D0gSMjqa6+Qw7qLKIQJsbZ6W8wee2jJwB8nV5PICzgrA53S8d9"
    L"+x4+09WzR4LZSRFjEIW+jKR09ezB/YP7XxzuvvS2efrj/Eqr4q0+wIaSiMU9L86lBcAtiFAiIQBVhUUg5nkt6iO2ukP21jdkqr4K"
    L"A5ABSBMNAlQIA6j66q1rJzYWMVXglgFXAiJCAKqA80JsFV0oBRAOoA/Sj5QCoB9iSwGYq9QL0QHqhxcipID6IbaqzZw6QEug+mBE"
    L"CFAV0FKIrSoBakQJ2BBbNQLKUAHQByJioyABNSG26gqUa8BFiICGBKrUANemEH0wKgcZCaisSiFubaNgsMqFoqSAhNhSVacSKy5U"
    L"AhGVVoKAlrHV50LRIlCTCwEuoilUxlZ9sKVh+jCIVBFTGbYUNZzEhJYAF0TMRjXEVolA4CAGzqjzI6eAOsLAmcCtzes1Y5XDh7t9"
    L"PwhK1vqOGkAppuEfZAKQVGsJPwhKprPbJ2dkDQERkNm0nZjI5a+O7Bx/4N7EGx2tAucIafRHPQFrxS4uLOPqL4vjExMzeeTSVspz"
    L"oQ3DXRFI9s2BV/t7W54n2O4cRBrU01EBa0GBLPw0vfxe5uzlcyS4erQom02jyk9bT5zoSxQKQucaUwzWCnfsoFy8OLUEoCi1jhj4"
    L"P/5hqVmNLTBJhYGSRGCi8g/KSPx3a0YzIhh/A9oKWapJiplcAAAAAElFTkSuQmCC";

static const WCHAR* AUTOPLAY_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAMMUlEQVR42u1afWxc1ZX/nXvfzHjGTuwZYzskDobEDptkIQHKpjQJ"
    L"tmm3G7p0LVa1RRERInSziSpaFOgu+xUr0bZihVhl0WYNKQWktJE6g1TtSlvSljIOOCqBBDaEtEmTmDh2QmwTf8zHm5n37r1n/3gz"
    L"ju14xu5ClP2DNzrSzNV9956P3/m45w7w+XNtH7paCzPzlLWJiP9fKyIajcp4PG4xs5zOfEEgZpbxeNyKRqPyairvD9JyPB63iK7k"
    L"ZVVzW1XDquaqhlXNVaua26qukJgIeYHpWkCIotGo6Ojo0PnfcscPdt9ev3jB2pqayPqKUHl9eSjUZIwCAAhhIW3bp1J2emB4eOSt"
    L"gf6LBzv//tvvAdAAEI1GZUdHhwFw9WHW2dkpChq/5+uP1r3wSuzJX7xx8OjJM/2cSLusmdkwszJTyTCzZuZE2uWTZ/r5F28cPPrC"
    L"K7En7/n6o3UFi3R2doqrynw8HrfyX327X9z32KH3jg+OJXOsDLOdVSadcdzxZEaNJzNqdNw2k6kwns44rp1VRhnmsWSOD713fHD3"
    L"i/seA+CbtsdnjncJABu/tW3Za6/39IwkMuwo5pTtuKPjth5N2DyWyPB4sjSNJTI8mrB5dNzWKdtxHcU8lszya7/q6dn4rW3LJu81"
    L"l8eaK/NEpJ/+166Oe1ru7rpt9YpILuuqZCothRDWZR9mzO6T7DkeQbiuElorLg8G9YavrF1bXR3+zcoVTVuJKFrY81MLkI8y6vvP"
    L"dv3VfV+7d8+yxgYkE7YGwRIkAJ7ueXP3QyKCz5LkOI7lEPSdt62IVFRU/FSrrkoi+mE8HrdaW1vV/1mAaJRlayup7z/T9Y22P9+w"
    L"p3FJg0okUkJIIfOWmRrHOB/XeFKMY8w8B4AlhbeGNy5TdtYsb7rBcNu9e0Bdo62tra9Goyw7OopbwioBGwHAbH28c8mXW9fvaVp6"
    L"o0kkkkJIKZhLKJwZBIJhzzSCCDx9jgGkJEiiKa8LEiKVzmLFsgaTbF2/Z+Dxp95rb8dHzCyIyMxZgEJyISL5nz//9b7Vt64MJ5JJ"
    L"LYSQV3JzpXKVMSgvs2AYsHMuLCGmKx9SygnmJxuMhBBpO6f/5I6V4cGhP91HROvykZlmKkdmjLuxWEwQkXmu65Utd31xzZpMNqMA"
    L"SMOMUsSGYUnCSDKH7/zwbfQPjiNcHoCrTd4iDGMYUgoI8iDI+XFM+s7M0nGUWr/2rjW7ul7ZQkQmFouJOVkgr33z0JYnam9bfcvO"
    L"ULDMpFIpKYWY1UGZAcMAGYWei4SjPzmDp9aF8Rd33YREVkMpDcsSkFKUMiSICI7jysr5QXPH6lt3PvTQE9H29vbhmaxwhVTd3d2S"
    L"iPhLd9yx8eZlN4dz2awRRJSH9JU0adwwQ5AAEaE2XI5I0x/hn95M4Xt73wcrFxVB3wSIpr87fU0QkZNV5o9XLA9/sfm2jUTE3d3d"
    L"clYItbS06MYNGwKLFy98JBgMsOsq4S3MEyafQoVxw7AsCTtj46O+ftiZLAQbLLn5JnQnw3jghaM4fPIiqquC0IahjSm5JgA4yhUV"
    L"FWXcsLj+kcbGDYGWlhZdUoC8t3PHuq/e0nDD4pW5nAMGixk3mUZCEAaHhnH8xCmMJhIgEtCakbWzWLQggmxdI7a+eg67fvYByv0C"
    L"ZX4LShsUsUGBhFIuli5pWHl/R+stRMT56DizAN3d3QIAblhUv66uthbGaF3AdjHynFJi6JNL6D3bD5/fB5/lgzae4zKATNZByCIs"
    L"XrEC//Eh45Gud3BheBzhigCUZhgz89oAwWjW1y9YgKVNDesm8zijAMPDwwwA1ZGqu4PBAJRSBMYsmhdIp230D1xAKBSCVhpauZBC"
    L"wGgDNh7QlTZws1ksvWkhjtP1+MuuD/Ffv+lFuMIPIQhaz+wQSikqD5Xhukjk7sk8zihAe3u7aW5utkLBsgZmhjGGvEw5MzEbCAIu"
    L"Dg4BRDBsADb4wqrlsKSE1gYGk0ItANvOoXpeEPNubMK21y7hiZcPA1qhIuSDOwOkmA0RGKFgWUNzc7PV3t5uZhSgEKKGc4H55eWh"
    L"JdpVYAZxCQgBhJzjIplKw+/3w7ZtLGlYiHC4Eo6rvJhuphIA5BwFYTRubLwRPx+qwP3PHca7Jz5GdVWZB5spkQ1kDDBvXsWS4Vxg"
    L"ft4PqGgUilg+1sZ4ep/FcfPxGspzFZAHP7iuApHnHzMRM0MbRi6TxcLaMMYqG/DX0X48HTsGR+nL5QdfTj3GGJaWb26ZmPIxnedI"
    L"xZNa6fcMM1zXRUXQjzQF8G7vCHKuAmhSiM5/ih1+r8jESrkEz055qBRPmRNlgRATol8aGUNl+LoJbZPhomnb7/dhLJWDPXgWf7Mu"
    L"gi33rkMyo6AUg2iqOwgQaeVSUQsUsBUI5BLpVLpXSOmBiItHaWM0fD4fygIBKKVQFgyit+8CRsfGYUnpwWUyeYEBxAzp86Hv/Ceo"
    L"TZ9DdNMybL53OS4lHbjaY34CPURMgpBIpntrArnE9HJCTC/iDhw4oDJ2ti9f6nvsF/HigoWuq47AdZVX30PgyNHfwXVdIJ8nClFI"
    L"awNLCGQ10HfqIzzU6CL62Bdwc0M1PklkQQIAMcxU6LBhRtq2+w4cOKCmF3VTftTU1BAAjI2PvZnJZCCkYFMS+wylFObPq0B1uAp2"
    L"JgOf34KQFpTWEyWGZwkDv8+HiyMpiKGP8O/3L8DOB1aBSSBl52CJmcOdFIJTKRsjI2NvTuZxRh9oaWkxAHD+/IWeoaFh1NVcJ9kw"
    L"QFSytaSURl1tDZTWSNvp/CjBGAaMgbQkDFk4e/Y8Wq538YNNt2JRzXwMJ3LewUZQsUMVBJH8+OIgzp3r65nM44wWICLDzHSo55fH"
    L"zg0MHLf8fuQPE7NHIzDqFy3E9bW1IAA6r3XLspC0XYz09WLbnRZe2nInKueX45Okx3wJhIIZRvp8OHv23PHoT+LH8vg3s5bT+/fv"
    L"z138eOjlbDZHUgozpfKc/imMG4ZyFSKRKjQsroclCSQtDFwcRV1mAC9/czEebm3ESMrx/EV4WaoU9z4pTTJp07n+Cy+fPr0/N+dy"
    L"mpnpgyNH9p4+fWbU5/MLbTQXzcjT6nrX9ZoIWRc4f7YP31ySwUubVqJxUQTD41nkco4XZbh0HaqNYcvnE8d/e3L07fff38vMNGs5"
    L"XQinsVhMPP/8s0O/O3FieybnCMuyNM+ircKRkAAoECKcwL/8WRX+tm05DAmkMg58kuC4CjlHgUr4FTPD7/frRDojjh377fYfP//s"
    L"UP6Yy3Nq7uZrDSIi8erPftlz15fWrEmnkpqI5GxdH0GEjONFoJrKMoylHUhBU7otQgjMrwgWzSwzsw6FyuX+17sPfaPtq+uY2QDg"
    L"OR/qCxOZWfccOvLgiRO/Hw2FQqSNNuDSBwRtDMp8AqGA9JjP44UnYU4phWzOAfJtlWnQMaHycjp89Njor+MHH2RmXeqCRJQ4WJtY"
    L"LCZ2Pf13ve8Oby5//zHIlgWNNpow8XOtHy5xFCaJ3UeJs/x+kaZrANV0Ef+XW2MCYVC5uTps+LgwXc2d+3a0VvokBTjs2QTNRaL"
    L"cTwetzY+2P5h0/LVF6qrq9sikTA5jqPBEJ/mjqUQ2QJ+H9gYELEOhULy1Jk+sf/1Nzb/w5NbfxyPx6377rtPf+oLjiiz7CDSz/zb"
    L"no7bV6/uampqjGSzGeW6Sgox3Rtnb+4W5jEbVJQHuaI8qKX0W+9/8OHI24eObH1q+ZoYc/P7Iam0C3+9ranln259SsvrVi+fK3P"
    L"Z0G5rlJai7z/zkmAPNSMlMIEy4KWkITDR/7n4H+/9qtNL+5++vdz7Uz/wVdMnfG4tcPrFvt2v7hvy7LGpf+4uL6+1u/3wXUcZmat"
    L"jSFPYDMtywsDAFIIJiLp8/vJcVz09w8MnTh15p8f2/zg8wDcSXtcnTuyzs5OsXPnTsPMeODR79S1rl+/saa6emNdXc2t1ZEIAoGA"
    L"F420vrw6e71QECGXy+HSyAgGB4c/GL50ae87b72190c/em6QiLB9+3axY8cOc00u+Z7Z9LtleGKtZXzK9eXBfz1AX+gyRgzEfdz"
    L"Tu5UNucMjCfG3xofTR383uObrs0l3/SE11nkTqvt4e9WtbU97NHD360qBslrdc06o0VqampoeHiY29vbzfTEw8wUi8Um5nxWGv/8"
    L"rwafP5/y+V/wDRPq7sWYYAAAAABJRU5ErkJggg==";

static const WCHAR* SETUP_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAH+ElEQVR42u2ZS2xc5RXHf993XzPOPOz4EScmsZ2kBmIcMOKlNuXV"
    L"sgCKxKJl00rsqi4qEMt247Cp1F1bddlKLWqrrkpbVKkNImBICaUQASEE8rAntvOOPZkZz8x9fqeLmTiDgWQcbFVqfaSr0b1z7/3O"
    L"/zv/8z/n+y6s27r9f5ta0b0iK3viRkwApWRV3zkxMaG1UihAqTU+ABFRExMTerUioJrzomFPHmYFZI3ioARyCg4Xl419YwAmJkQ/"
    L"/7wyo9/6yd3PPP3wr7dv6RxQJgKl1NqwRwBbjkxf/OQ3fzrw7Hsv/vjfV3y4AQCiRECpIe/nv/vt+89894GRoxdCpi8FeLZG1iAZ"
    L"g9gw3ONxa5/LL34/eezZ7z19u0ghaEzX5+eFfa2Xaq0EdnWMDvf1Hz5TM8+9cBhLaXX9wN4gAoFEjPzs6d0yOtzXD0mH1sq/FmXt"
    L"6795UZAkPnGupsN6IH35lEqMrEkGWFpxoRRw/GxV5ySJQV93IPu6kgagtFLGUK/5VG1YSwD1mo8yBpRWn/LhxiPQsCiK8Ws+vqPW"
    L"FIBf84miuO1n2gOgFFEYEfgBgadIDGsEAAI/IAojcNVqASihgDhOmC8ukrYhitcGgWNr5ouLxHGCchtjrwKAPOVayBP37uQvk30c"
    L"nyuR8WzMKtNIa4UfRDx4Wx9P3LuVyX99BOSvC6ItAFFs6Mo4vPCjR6jUIyzdaIvaUUYjZunsWvVPqYY4ZNMOG9JOM8r51ckBpVhy"
    L"uCefbvR0y3wREUQEpa46KiI4trV0T5yYz322cW/jehDGX3jPl1KhKyDCyHxmmsUIWms81yaKDXGUoBRobXHonQ+wHZvuri56ezei"
    L"lL5mE7AS59sGIK29yrKKLkbwXJcgDNi3/0127hhieHArvh9Sq1V5+9D7CBD4Po8+8gC33jJC3Q9RNFpzpVSzhW7th9ov9O1FQFp+"
    L"5dO0cRyL2dOnee2NtyiVFzlz7gKd+Rw9XXnOn7+A0pquXI5SuUw63dGkkeB5HkYMcRRfBbF8rNWk0JXwtiavEVBKEwQRl8sVNnZ1"
    L"Ua3WePX1g+wc2sqhD44QxzG1eh2lFN0bu3BsjVYuJ6cLeK7LwJbNhGHUkjcrU682KSRLv/LpEFD3A3ZsH2Ro202cO38J13WYmT3N"
    L"iakCmQ0Z0qkUURRRq9XYP/lPxkZv5sjRYxRmzpDPZfj2k4+hLRsxDUq1jrXqFFoeAS/lkSQJWivGdt3CVGEfKePR19dHOpUCpZrq"
    L"ZIjjhJm5s5yYKpDN5ti4sZNi8TIHDr7DNx/6OnXfR6Gvvn8tKNSqEiLC0Y+P05FOsbErz4cffQwi9PT0ICLUfB/f90HAdV08z2Vg"
    L"YAvFhSJGDNVqjWxmA9uHB5u9z42tkdpWIVnSeoPjOJw+c459r7xGR0cHWisWF6v09/cjIgRhSIfnMH7bGI5tc3J6losLjYqazWU5"
    L"e/YsWzZv4qH795DP5wiCALUULVmuFasTgasgGvq+UCySSqXI5XL4vk86ncZ13SXnn3zsGziuC8AtIzvZP/kmhbnz5PM5RITtQ4Ns"
    L"6slTrNTRqrnCE1bkPI2Fenu0oWUAY0wz7EK5VKJWq2PbNlpr/LrPrSPDOK5LvUkjYwxju0YQSUiShGw2y6H3PuDYyRmCIETrz3Jf"
    L"VjcHpJnAgkbhByFjo7vYMTxEqVzm9QMHCVqk0HGcq20FYESwbXuJJq7rMj9f4eVX38CYhDvv2M347WP4QdikUftZrFdMoeZ5Ygyu"
    L"5zGyYxvjd+ymWq0BjYQ9MTWDUgrPdbBsG9uyODk9gzGNyhtFEalUmnw+Dygsy0I1XVkpheyVFLBWGRURUp7HmfOXOTFVwLYtgiDA"
    L"81wuLpTYP/kmY7tGsGyLqekZjhybJp1OISKUy2Vc16FcLuP7Pl2dXcQm+dxxVllGpUknwbJsCjMzvPX2O4RhRCaToVKp0N3dDUBh"
    L"7jzTs2dQKIxAOp3CdV0uXbrEls2bePjB+5mfX6DuB/T0dBNF0coKwIpVqGVmjBFcz2aqcIrFxRqdnXlq9TqVSoUwiujr7SWT2bCU"
    L"B5ZlEccxlUqFUqnE/Xu+Sjabw/NSWJZFGIYtkV1ZO9EWANOY9CUZVUoRhiH33X0XFy9c5HLxMgMDm3nga/dx+MhRZufmyGazuK5L"
    L"HMcsLlZRqpEf+Xyeo58cY+tNNzV7oGYzp1pkVMCY9kLRBoBEWVpZIsYkiVmaHhMnuK7LPXffRRiGDA0NYlkWuXwnf/7rSyRJohYW"
    L"iriuLXeOj1E4NcviYo10Os2pmVlOnJxiaHAbQRguqZcCEiOCYCxLOZCoLwFAiRHRSj1YPjlzccpy9HhXrgOUblZ9hYjh5q8MoTQE"
    L"foLrWszP+2htkclkEkTU448+ogcH+th92y7+8fJ+qrUaW7f009PdRTbjkYqdq0tNARGjtAOF0/PvQW+54cMX741eMwJ79wJMxi++"
    L"8ugPMtnMrwZ6swO+H4K+0rioRheJoFBYtqPmL8zhV6vZbDZnRQm8fPB40XULKpVKy9mS5tSpecbHB3nrw9ME706jtG6pXojnusye"
    L"K737x78feg4m47179+pV3F5/PA9zAkZ90QYAwPd/eM+mTf1bh4uVSu8vf7rvpcZ/M5Dfw7Y9e5j52xbtuugBbIKDrS9vb7y"
    L"DxxtHMvRL79+vedX+wNH+5+YmmvbiYm9anR0VAE89dR3TOua97/yiWnd1m3d1m3d/mftPyFq/rDVJSASAAAAAElFTkSuQmCC";

static const WCHAR* READYBOOST_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAHFklEQVR42u2ZW4xdZRWAv/X/e+9zzsyZ+9Ay3EK52FbQmhh4AKoi"
    L"iIZLVNCaEBMTMBp8KJqYKBATYlR8IEbAQESjKRCVaUx8IWoDGBMCakzwoWmo3GoLbac2nWnPnMu+/Gv5cPaczgBB5tIZHrqSlZ2s"
    L"7H/tdV/rXxtOw9qCrBQjM1sULxEBsPeFFRYr/JzllnLulHng1ju+OxK32pb3Ze/KM24l1jdxpjzy47tmVsIDskzLu3vvvddddsU1"
    L"D52xbvxLrWZTTXBvZb5ASsOq1ao0W63de17cfcv27bcdLUNqScpEywkbEdEv3n778NX1vtuGhwaTWrWC9773jqoC4Jybfw5VZXRk"
    L"dOv+Awe2iMjTk5OTHgirqkAvJJLEgBNpmo6laWpZlnVNaUaSJDjnSNOU+bQoirQomuKcL5b7/Wgl4l8x772XLMt4cnKndDopRVFw"
    L"y82fZ/36dfzuyUnSNCOEwOc+exMbNpwvWVa4EIK8LxSYD+12h06nQ1EUvRBqtzukaUoIoUdbKVhxBZxzeO8xs7fRTgW45TLIs4Vl"
    L"s2xQ/D9ameSypgpMTk76C8bHw5yFvXdoCIQS5+hhAU3Kdz399XooK9Dq94GyjNr4po9O/PqBn+wZGxsdbrfbNjU1JaqKmbFu3Toq"
    L"ScLBQ4c4STuDWq1PQdyfn/3rp3949527zMyLSFg1BUrh5eFf/ubuj2zZ/HXv/ISZOhBJkrj3XpEXqClJHEMZRkWeE1TNOY+aHn7x"
    L"X3se/sZXb/2RdZme+tnIzDzAQ7/YcU+j2bHlQqPZsYd+vuOe+bxPWRUqhy9lbOPAlks3b6/3VUInzfDOeXv7oIaPY0KeoiGUDhAE"
    L"Q1DwVVQ11PsqbPnw5u2MbXwQmJ0LzVNWRkXEBs/5YBxHSWJmHuk2WZknvWkgTioc2v0Mzee+zeDwYC+p260W49c+QN95VxLSjjMz"
    L"iaMkGaz5WERssRPqkvqAOG9qqiKCKZibN6kZOHG02x3e3PUdNo8cILI6JkA2C4OXk5x1GSHPMQQRQU1VFjpxFcpoaSkzQ81QU9RA"
    L"Q4GLY17ddT8T0ctUBtdTWIxEVWaziGTLt3BxgoZAr9ct417gli5/2YxMFwgfVaoc2vsPbM9PWT+xjixNcU7QtEEYv5La+VeTd1Ks"
    L"a/kFvFZZAcPKj5uCqWJAViiHnv4eGyY8hXoQehNpfPE28BEWCtTKsyWvNenEBpgahnWt31dj318eYTx9gerAKBqKbt3JW7Rrm4jP"
    L"/RQhTTGkq7Dasq9kS1ZAzbqWN0NDwMUVpg8eIH3xfs4+e5wsKxAB5z2ddhO/6XZ8/2jX+jgU3/WCdnmtfgiVSqgZZoq6mAN/vIvz"
    L"RhuoJGVgC5q3aNU+RLLhRvJWgyJkmBZINo2FtHt+LcZpM8PMCEVOMjzCG3/byejxpxi94BzSNMdHERoCSEQcZmj/6QuY5sRxhcax"
    L"w8g5n2HsE9/vzUirfx8w0KC4qMLxg6/z313f5JKJjOkjB8EC4iIq/cOIeAaj42g4SpRUaM1MEfo/ztgV95AHR0VtWbuJJSkwNEQ3"
    L"cU0x8WTH36D/ki/zZm0ACzkuSmjNHGH06O8598waWe6Jq33MzkxxpHYdZ9z0OOI8mrcxKhjG0BAcXz0PDJUlSCjSJgPnXc7wxVvL"
    L"cmpE/cKbzzzKQOMxlDpxIrSmD3Ok+jHGb9zRnYbyDmaC9TwwtMo5gBHKBAxZmyKdLUchI20lZC//lvpADcTRmpliqrKV0Rt2YDis"
    L"SEE8aqHksRZVSE9WIQVMPGaCqw5zYv8/GUx3k/SNkDePcaS6lZHru8KHIsVwZfe2Xjlem078FlRTggqtlyYZH/Jk7QaHO2cyeM3P"
    L"MIm7YYPrVZ75uDadWG0Bio9pHjtIZfoFKjHsnxmmcu3jUBlF82bXS3PCzzu3Jp24azl6qKGAqJ/m688yEv7NgeYE1eueIB79ACGd"
    L"xZgTnnfANegD8zsxpiCQ5znFSzs4wQSVTz5BPHwheXsacTH2DoHey4G1WauY9e4BGpC4zux//k7z6H5qN/4BP7SBojMNEi1I2JPj"
    L"t/boyxmol+SBeijEDKdzIQGEoDCzm7Nu/hUMbiS0j3VHZ7V33n3MhV6Xh6uHQlbFA2YmIbg8L4os8lFQVQ1qVrRPWO3C682GL7W8"
    L"edQCYiEECxq6z7eiBlNVjXwU8qLIQnD5Uv7YLMoDImKTZn7v3ucbr72678HNmzb/oN5fJ2hAxAEDYAVU6u+30i5tXDeH2/M8tqr"
    L"+x7cu/f5xk7w2xa54Fp0CG0TCeXq4z559DG5+KKLvgbWr0HLzYm8pwrmvDOQ5suvvPLonXd85b6S56K3c0u+TIv0Uq921VU3VBuN"
    L"WdOB97bvdw1vAwN1ee65pzpAex6v1YXlLmZXgscK/KU0War1yph7f/wrPg2n4TSsDfwPyjfOi6GXiSgAAAAASUVORK5CYII=";

static const WCHAR* DRIVE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAEwklEQVR42u2Yv49c1RXHP+e+N/Nm1rs7XuzFxiL8sgATsBUpioQc"
    L"YE2BKAApsmhCQZGCP4CeJhVNCkpDw4/CKEVQpFCAlGIlhKwUiZSkifghSrMsmJ2ZN7/eveebYgzrWMQ7Y781AuYrTTFPT++e7z3f"
    L"8z33HlhggQUWWOCnDJv1RUl2UwMzUy0fkmQ3O/h51s333ojLO9E5ucZOV7DfZEx0Vs3MLl2hEs0tIUkWzPTUb184fPbpJ9+89567"
    L"Ho4xSobtKwOZ8jyzjz797MKf/vLe8++ef3XbJZtbUpICGxv5udfffl/fE869/vb7bGzkksJcEtKUsbN6+8Hjv7/zNJC+7pUmdxuN"
    L"xpgZqtlJJNFoNCiaTWJKWuss6/jdd57mH5+smtlX+j9ZuHYNWBDYODkHUkza2r5kzaIJUv3KD4FL3T6H1jq0i4KYCIaNpzFcfxEj"
    L"dwMYDIc0m01uXT9MjBGruRJCCHR7fcpyQKto4nK8BhdCCPfdfzHGfSMgd8Bwd9x9pkzPkAFwOSkJMyPPMiTtC4Esyxi7Iwkl4bUQ"
    L"QKSUyLJAv1/S7Ze4O3WGLyC7LKGikZGSk6Qbz0CHDghiSjTynJWlNr1ut/bdnzofNBoZS0tLVDHRkiNNY9i57gx0wCXkIqZEURS0"
    L"Wy3q96DddptSwiWSC0fQgWsxyPf+9LSgpCmJfYn+f84uIPdpQctrqAGBu3AXYb+jv2q9WdrNDARE+sbWbhLcnXQ56zdMwKVvf/vR"
    L"gfdcs44M+PeQAa8jA52rNAk3KQNX1MAeJjRbI5OmriDbVZFdec34rud2/Yqb7n4dNdDp7EpIwlMiCxlmRpUimYVvF5w+D8RUESwg"
    L"Tb09CzbP1RuAJJGSIwSdzo3baEoieuJAY4lyNGCSJhxaWmPgI4Roh4L+sGScKg4fWGPoYwzDEd3BBJewGUkI0WrktF31uJBcRI8U"
    L"1uRfX/2Hlz9/gx2VvND5DU8f3cBx/v31x7w2+DNlGPHs4AxP3HKayib0BhNeurDNKGsT2NsEQgiUwzEvPtjm0c4KKdXkQkagN+zx"
    L"8sU3+Gx1m1Zo8oed89zXvoM7Wsc4V77Dzm0VrVDw1vZ7/OzSUR44fA9VGlA12sRilaDZCFQ+IGp6nKilkbmm5/JRNaZLSTsUFNbg"
    L"y1DSqwZUjYoyjGiFNk012Mmd3qgkyHAX/XHFiIpsRgK9UUWMIE/1SMiQxtWEg+0Ov1t+hld2/kgZ+pwNj/HzleNkIeMsG5zf/is7"
    L"ufNY/yQnb7mX7qSkyHOeOQoVfcK0JPea4VCtJG49sMQoOuC6IQLLKVpyFUZI/apvTx76NXflx+jHAb84eIKJR1Ia8/jqr7i9u05/"
    L"NOTk6n1MFPGJk5nx7P0H5xj/TW24rFyTmFxSsZyizU3AzCQpnDlzpntxa+vDU6dOPdEqWiQTDx05QWaBMg5pkNMEHPHgkRMEjKGP"
    L"ybR7PJ7M0wsuh5o33UKWc3Fr68P19aIrKZiZz5sBbW5uxmP3//K5PGu8eWR9/eGYoiSZA9lVl5pv9FrDZUd5ltvnX3xx4W9//+fz"
    L"m5ub8VqNxGbYEwE88shTa71eX76S9nUwF3qZVlaW7YMP3p1ptPiDH+7++MfrCyywwAIL/KjxX5LJaP7AuJFSAAAAAElFTkSuQmCC";

// ============================================================================
// ============================================================================
// Native Windows 7 icons encoded as Base64 PNG data.
static const WCHAR* WIN7_NATIVE_FOLDER_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAMAAABg3Am1AAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAX"
    L"cJy6UTwAAAH+UExURQAAAPz0y/71sfjtuvrywvnuwf3yq/3wnv74x/XluuvZpPXkmvXqsfzxpNvJmOXNdujWh/LikfDemPbp"
    L"qvjss/vyvNnFicu1Y93JcvDcefLgjPTklvbopPvsosawZc64ZerTevry0f/61P/62dXBcufTgvz22Pvsm+/kuv/84f/96cSs"
    L"W825bNS+cdfEfPXmofnrq////867cdzKguzYiPnmnPjnpPDesb+oXujRjr2mXcm1adnHgN/MiLSaUr+rZ8iybNzIfePZlfDe"
    L"lPPhlsu1ctC6funYj+7dkf/+9regWbGYUceyaerVifrkkreeWL6oYNnHeeHOg+7XgeHMefDdkfnli822afHdgbSbTO3aq7CW"
    L"TdnDbtrFdOvYffHafdO+bufQcbyjU9C6bOTNcquTSLWcUfbccuTMa+nQafDXc9O7Yu7WbLGVRd3GaefQbvrpk+bSodi/Ye7U"
    L"Zvjig8CnU8OrVMuzW+LJZejSkfroj+jSmbqgTdfAZObNYezQXauPQNG5XeDHYffge+bLXuvOWezQWvbdddvDX+HHX+nv6uTQ"
    L"msbJm3+1/9bUm6uTWrWeYe7Vab/EnWOh/9DQnbCdebSfc7ujb8Oscsy4fdO/gOfXhfDlx9PHrdDAmM65hdfChe/adMvDre/j"
    L"xenZsN3Km+XOlubQluPOmuTWtacCfQ0AAAABdFJOUwBA5thmAAAAAWJLR0Qx2dsdcgAAAAd0SU1FB+oIGAwpKkgxCawAAAAl"
    L"dEVYdGRhdGU6Y3JlYXRlADIwMjYtMDgtMjRUMTI6NDE6NDIrMDA6MDDGXK7lAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDI2LTA4"
    L"LTI0VDEyOjQxOjQyKzAwOjAwtwEWWQAAACh0RVh0ZGF0ZTp0aW1lc3RhbXAAMjAyNi0wOC0yNFQxMjo0MTo0MiswMDowMOAU"
    L"N4YAAAOBSURBVEjHtdPrW9pWHAfweHJBTRFTvCASqhYHzhFAwQwd2JbWubawWoTSwMy8LVihCqEaLxuzeNtcu3XXXnbvrv/l"
    L"foHyPHsRQPc8+73Ki+/nfM9JcjDsP0wTwokzAYTjJIkoHUU2nw60ILyllT5H4ARCer0eNWQtCJGozdDOnDeeIwiio4Oi9DqE"
    L"N9UDRCfe1W3qaTf3WigcEGwRhtUTtQBqbsa7rBf62s3mXjAdJNHfrzpdTYAGLpJd1m7boCp6LRYjQi3ka6RdEzhUgIZeJ9uG"
    L"33By7UxZsC63m8TrATTkaWqzjti4US8I1uWjx0hEawOqIgY8/JvdIxw36mdoC+RdJPLVa0CdHse4dcLGvRXwe2m63KAN+FcA"
    L"efjg5KUR7nLAfyUE4irJ2+tuCXkcwXHrtSludPDt6ZDv6hhe4wzVhs53+OB16yUnHMI/HWJoXyOAhvgb1ydvjnCjgTCACI27"
    L"G4CLfPDdWwBmbke9XjMTcZwGjN902i7P+sPeWDyGjA3BjeAt650p0+3BaDSWiHU0AgP8XXhNE86Z2b5oNJGIIVcjICRVkHpv"
    L"dnZuThQTbiOjCYQqaBagYfjO+ylT39y8KIpGS6g+6OSTdxes3Yspw9JpgJ5CwvIH48MAUktL89K86BqrBdzwc1Bw/QVoUEHa"
    L"sDJ/716PyzWtDSgE1x6GEpZVsJpKAchkJJo+XwOU8zo7JWQr4H56ZWUtk6GZGg0s5Fm7fd2eq4B8Kr0iy/JagaZDDs0GN6uD"
    L"/Pq6kE0+2NhcXVTSaQVEJmGmtzRAjoXlIb8d30nuLmxAw31Fzcsy06+5qVaWhdW3twut17K7DzY3Vz/MK8pHcrEo0x6XZkNl"
    L"+Xih9ePs7sbG3l7+oSIXK0B7S3ZffLsQLxRyKtgEAA1FdQCEtcB6BJYvlEq57CvwUNmH+EGxVkMkUs6Xqg17+X0ABweHtRpg"
    L"fYiLR1VwfPzJPuQPDhntBiEWh3xJknLLyQX4DKv5Y2X/UJ0rn9IBrU99Yi4dHUkqgPtzAUC+fObD4mePHmttCcN2RDUvnXz+"
    L"xeTEotOWSinKExDyl199rdmAYVsV8M233z199pybMZnWMk/ktYzkf9GmDb4Pl4H6+MOPP039HPAmROmXHoOh61dtgL2MVkF5"
    L"ftvxb4WY6d+x2nMi/huU0R85rN78KUl/YWearbOCl+G/zwb+p/kHzTcLVL8DX98AAAAASUVORK5CYII=";

static const WCHAR* WIN7_NATIVE_FLOPPY_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAX"
    L"cJy6UTwAAAAGYktHRAD/AP8A/6C9p5MAAAAldEVYdGRhdGU6Y3JlYXRlADIwMjYtMDgtMjRUMTI6NDE6NDIrMDA6MDDGXK7l"
    L"AAAAJXRFWHRkYXRlOm1vZGlmeQAyMDI2LTA4LTI0VDEyOjQxOjQyKzAwOjAwtwEWWQAAACh0RVh0ZGF0ZTp0aW1lc3RhbXAA"
    L"MjAyNi0wOC0yNFQxMjo0MTo0MiswMDowMOAUN4YAAAxhSURBVGje7ZltUFtXesd/ku6bhAR6ASOBebMwfhXB2Gu7AccYBreN"
    L"227cbWPsZNNOnNkvTttp2tnspzQfPJ1uMzud2dnWM92ddraZaZPmg7Nt4p2MZ2N7ccBgbMDGBmGZN2ODkAQCIQnQvVI/ADfG"
    L"+GVjrxN/6Jk5c+/ckc7z+5/nPM9z7rnw/+2bbYZv0vj27dvrNE0Tjh49XhoMDpVu3LjLvrCQLLlw4dS1c+dO/off7/cD2sPG"
    L"EJ4SmD2TyWyrrKw0NTQ0PGc2m51er7dkfHy8ZMeOHTnDw8O+srIybDYbmqahqvD97/8Tly7JxOMGYrHKP5iZ+bkK/Ctw+2Ei"
    L"nljA4cPfq6uq+rO/DodnnHZ7QXE6nSr2eseor99HTo6JhYUFMpkMqqpSVFREKpWiqKiIdDpNLBbDaDRiMpno7b1KRcV+fdz5"
    L"+flvAZ8C409VgNf7XENDw/N/dOLEx0xOTmMyWfjVr/4Tj6eC0lJJBzQajRgMBoxGo96TySTJZBKPx7Nq3EwmYwZMj7L/xAI0"
    L"TTOl05BIJLHZLPrzZchoNIqmaczMzKCqKtPT0yQSCWRZRhRF3G43hYWFj23/iQVkMpBOr35uNBrw+/2oqorFYiErKwur1crW"
    L"rVuRZRmTyUQoFNK98swJMBiMbNiwgVgsRiqVIhaLEYlE6O3tZXp6GofDgSRJuFyuZ9UDRvr7+1FVlaysLBwOB263G7fbrS+t"
    L"RCLB1NTUs+oBA8XFxWiaRiQSIRQKMT09TXt7O7IsY7VacTqdZGdnYzI9Mla/GQGjo6PIsowsy3g8Hmw2GzabbUVGMplMJJPp"
    L"r274aQswGg1UVVWtSKNGo5FwOEYgECAQCDAwMMrw8CQTE1Fstg3PloDxcZVE4irXrgUIBoMEg+MEAgkkyYXFUoQkubDZfheb"
    L"DWy2x7f/VASYzUWcOHECWXZhNhdhsezBZLLg9T6pta9JQFHRyw/9z8JChETiFsnkKLGYX39eUfE3z4aAh8FmZxtxuWy43Vbc"
    L"bjfx+F8SCon09//osez/VgRo92y1IpFWIpEW7HaB0tICdu/eSHX1i6xZ8z3C4TsMDg4yNzeHx+NhcFAmFHrGslAicYtjx95h"
    L"zx4HVqsVRVGIRCLcunVrBXxBQQHhsAhMP1sCBMFCcbGVWAw+/riXvr4Q5eUi+fnRFfAej4ebN5PPngAAs1khGp3nZz/rRJZl"
    L"DAYTOTmZFfB2u51MJvFE9h9/E/IIAaIoMjs7iyzLSJKkV+O74VOpFAsLC09k/6kEMcDU1BSRSESHt9ttq+CTySSqqn4zAo4c"
    L"OfKSqqr1ohh/6X4eCAYnGB8PIssyiqJgt9tXwX+tAl577bUqTdPqbTZbXWVl5R9u3LiRbdu20dzcz8zM6t+PjY2RSCRQFCeS"
    L"JOFwOFbBJxKJpyfg9ddft2uati+dTh/0+Xx7fT5fcUVFBUVFRWQyGb2n0+kHFrK8vDwkyaR74V74+fl5PQY0LUk02kU02sXU"
    L"1PW5ubm5GI84Ulkl4I033qhKp9N/7Ha767Zu3bqnoqKCiooKzGbzCuCV18x9BbhcLrKyspDlOD5fLl6veRW83+/n+nU/t29f"
    L"Z/16J5s3RykufoE7d8qVQGDz7q6urr8Kh8P/3NfX1wqoDxTw1ltv/bvX6z1YUVGRU1VVhcvl0gHvB31vf5CALVvyOXDAqc98"
    L"OBymubmZ7u5uUqkUNpsNn6+cAwde1b2kKAqVlZUIguBMp9OHWltbD7W0tMTa2tp+mclkzsZisZMDAwNBIKMLKC0t/fO9e/eS"
    L"l5eHLMtomrYK/v7g6Yd6wO12cubMGdra2hgYGMBoNFJWVkZDQ4P+omM2m1EUBUmSMJlMaJqGpmlMTk5y6dIlLl26RDgctr3y"
    L"yisvr1279uWRkZF/+eSTT9qbm5tfBCICwPz8PDdu3GBkZITc3FycTic5OTkoioLRaLwv+JcCVsZALNZPLObnJz95l3h8ivXr"
    L"11NWVobP50NRFMxmM5IkoSjKUoEz6OPMzMzcDc3zzz9PU1MTTqeTUCjEjRs3iURmcbtLdubl9f19KBT6O+Ho0aNVgiCgKAqa"
    L"pnHnzh2Gh4dxOp04HA7y8/NRFAWTybRCAEAmk8FojNLS8iNGR89jNicoKSnhxRe3UV5evmKWl+8lafGwK51Oo2ka8Xicjo4O"
    L"HbqmpoZDhw7hcrkIh8P09vYxPDyGqhro6emmuroGuz0PVVXLgXIhlUq5BEHAYrGgqiqqqupVNBqNMjQ0hMPhIC8vD7fbrYMv"
    L"C6mt/Rbl5UWo6l4URSEejyMIApqmIYoisiwjCIIOnclk9Jnu6OggFApRU1NDU1MTLpeLkZER2touEgpNoSjZ5OS4cLvL+bdf"
    L"HKfmbwv5n7f/i/r63wcQAUmYn5+fMZlMugdSqdTSgauq309OThIKhQgEAuTm5lJUVITVagVgYWGB3NxcHc5qtd53ucViMTo6"
    L"OnTo2traFdAtLa2EQlNIkgWr1U52dj42mx1RNBOLzWBakyQ+NY/PtwtRVL7MQh9++GFnfX09ZrNZn/1l8OV7URR174yNjTE6"
    L"OorFYqG4uBi3240oiiSTSf0gdxl8YmKC3t5eLl68SCgUYs+ePRw+fHgF9O3b45hMEjk5TjyeUmy2HJLJFC0tZ9i4sQqvtwC7"
    L"PZ/qQBO2DjuNr/4pzc2/WJFGVUEQCAaDfPTRR7z55psIgoCqqitE3Oud5Tze09ODx+OhsLCQNWvWkEqlOHfuHB988AEAtbW1"
    L"HDlyRIdubW1leHgEg0HEas3GbM4mEOinv/8zSkuLePfdf+TkyV8SjU7x05++x9tvH6e0tJqDB48iSWZEUUEU5ZV1YGhoaMZi"
    L"sWTv2rULi8WyAvRur9zrHVVVEQSBcDjM+Pi4HuibNm3ivffew2g00tfXR3t7OwMDA6TTGRTFitXqoK+vj0CgH7//mg4zNzfL"
    L"D37wF5w9e5bs7GwaGxspLPRiNucgyxZEUV7qCnNzc3FAEwBOnz5d39LS8nOfz7fFZrOxfft2/Ux/2RPLXrlbxN1xsnxtbm6m"
    L"ubmZ+vp6+vv7uXnzJoIgIklmJEnRZ/HTT0+uqh1DQ0Pk5+dz/Pg/sH17IyaTHZD0/6iqyuXLZ2hp+V81mUzOAJMCwIULF7qA"
    L"7169evXb58+ff6m8vHzz3r17xR07duB0OlcIuZ8n7o4TQRAwGAyoqko8HsdiyUYUZV1AKBTiypWzD9zb/PjH7xMKGTAaZSTJ"
    L"giBIBIOjnDnz37S3fzY7PDwwEgwG+4EhILy8F9KAKxMTE8MTExMfd3R0bOvu7n61pKRkd01NjWXnzp2Ul5eTTqd1TzwoTkRR"
    L"xGg0LlVXBYtFJJFI8MUXp/H7rxONTt4XvLGxkbq6RubnZSyWxYTS0nKKzz//QB0c9IeDweDN2dnZW8Ag0AZ0AWN3b+Y0YJLF"
    L"F9RAb2/v5729vdXXrl1rOnXqVM2WLVsK9+3bx+7du5Ek6YFxsuyBxeKlACY6Otppazuvg1ZX1/DDH77Lpk2bOHjwO+zf/x3A"
    L"STptJhC4xsWLp/nii0/mgsHgwOTk5GAqlboD9AAXlwRMA0l49FdKM+AE1jscjt9bt27dn5SVlRXX1taKDQ0NeupVVZX333+f"
    L"yspK+vv70TSNxsZGLl++gqpa8PuvUFiYh8+3m4UFEYNBxmpN4HCUEIsZmZ2dprPzHBcvfsbAgH88EokMR6PREWAEaAGuAiFg"
    L"lnt2pb/pZ1YJsAGlZrO5Ye3atS/l5eU9V1dXZ2lsbKS4uJhjx46xf/9+vWgdOHCA9vYuDAYXkmRGECRk2YIgfBkPfv9lOjvP"
    L"0tl5bi4cDo9MTEwMqKoaBLqBy0BgaVXMsbT7vLd91e/EpiUh+cC2tWvXHs7Ly/ud6urqvNraWl544QV6enpoa2ujqamJ9vYu"
    L"BKFwKYssBuXibJ+lq+scIyM3wxMTE4PRaHQIGAXal9b2OJAAHvnG/7gfug13La+tTqfz22vWrNnv9XrXFRcXE4/Heeedd2ht"
    L"7URR1iGKCj09rXR1/Zr+/q65qampsXA4PJhMJpfXdhtwA5hicW1nvgrIkzYZyAG8sizXeTyeQw6HY3N9fb24YcMmuruH6Or6"
    L"NaHQeHhycvJWOBweAG6xuEy6lu5jv8lsPy0By00ArEABsLOgoOC7OTk5lXNzc/Hx8fGBZDJ5G7gOXFq6Rpdm+5HvvV+XgLvH"
    L"zAJcwHPARiAMdLKYVWaB+d+msafZzEs9xWJQPtFs36/9Hxka7yaeL+NaAAAAAElFTkSuQmCC";

static const WCHAR* WIN7_NATIVE_DISC_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAX"
    L"cJy6UTwAAAAGYktHRAD/AP8A/6C9p5MAAAAldEVYdGRhdGU6Y3JlYXRlADIwMjYtMDgtMjRUMTI6NDE6NDIrMDA6MDDGXK7l"
    L"AAAAJXRFWHRkYXRlOm1vZGlmeQAyMDI2LTA4LTI0VDEyOjQxOjQyKzAwOjAwtwEWWQAAACh0RVh0ZGF0ZTp0aW1lc3RhbXAA"
    L"MjAyNi0wOC0yNFQxMjo0MTo0MyswMDowMEZjPDIAABI0SURBVGje7Zh5jF31dcc/93fXt9y3z3g8u2f12MY2BAI0G1mAEPbF"
    L"kHQjpI3SQKQkqpKmEpGiNGlVVUn/CSmt1DSQirQhRGzGBofa2CbGxsEb3u1Z7FnezNvv29/d+seMHdbgKoJWao509Z7ue797"
    L"z+d3vr/zO78Dv7ff2/9vk97rF27cuFEHRoUQn9B1/UpFUdbIstwnhAj4vu+6rjtt2/bRVqv160q1sl3XjaM33nDD2f8TAJs3"
    L"b75cluV7DMO4KRKJLG9vbycajRIIBM7/x3EcqtUqpWKJufk5CoXi7h8+8MBfnj179uX9+/e33vhM+b1y/plnnrk7EAj8SzQa"
    L"+WgqlTJjsRiGYaDrOrL8GzeEEBiGQTQWpWt5F6FQsHv9+vUb1qxZYy4sLByYmpqqvecAmzdv/qaua9/VND2p6xogIcsysiyj"
    L"KAqyLCNJbxaD3bJx7BZCVnRV0z6wcnS0PZvL/WpyYqL6ngFs3rTpy0KWv+u6rrGoWIEQAlmWUVUVTdPQNO1NAJ7rkS/mWchm"
    L"sKwSnuuhGcbaFf39RrFY3DY5Oem86wDPbNx0hedID9br1bDrughZIEnSeQBZltE0DVVVEUK8bmytXiObyVIulymVLAqFAs1G"
    L"Q0okkv3pubnZV1555cC7CvDMM5tUobnfKNezH6nmXWRJAckHFnUuhEBRZAKBAIZhvC4CrusyPz9PPp+nbJWxLItypUyhkMfF"
    L"DY8Mj0Ty+fyuiYmJvPJuAUieMtKKHLymMbCF1smLkRbG8MthJMdFBmRZxjB0qpWK12i1bEWWCYfDiqaqctkqk8vlKBQK5PN5"
    L"SmWLWr2G67hoQqFnYOTK7p7uqwOBwKl3D0Cp32h37Bgs9/6ScudxmoevxZxcRXs9geTZANTqzaokZWeFopSEkL1AQDMiZmh5"
    L"vVpNTc/OSOmFBerVCsIXJNQIkVCQmBqGSit4/Y03fOTI4SM/f9cAkL33qV4vevFiCqrKdGeOorNAaCZEd1llsFUsdyTix+Kp"
    L"tmwsFjMAUa5UCvOZfDafy4xm0+ku2YEuox1TD+B5UHFrTFbSuDWfxLLUalVVRy4UQADe/ygCbmBNZPp29LlrSCoVUlKdSU/m"
    L"ZLCNfdWg2x9wpvu6OjINX4g9L+2arFarzeGR0V4zYvrBsDnRpjejAUUJN3BI13JYrRq27+C6HsFAgEAgMCzLcteFAMiALiHF"
    L"u4yO6HRj7shr7rtvN8jz/H6nqqLSRlJKkZQ8BmWXdZrMQpR6f8IsasGwsW/PntN/991v//v8/Hz2i1+89+Zrb7jx49ForGkt"
    L"5ArTc2fCtuThSx4SAoFAlgVCknxNVTVVVRIXAqAACWD02yNf+7tZaWHnjsyul56dfWGviqjJktpq+M38GwfZbssoWgUEAnwf"
    L"yZfA9/DcJkkh2cGO1baLrOaymczc7OxEybJmbbt1WlWUj+NLou63XMupISQZVVaQpMUULOHjuZ4PkiTLinEhAB6Ajx843hiX"
    L"v3nF1776V8p9PDa5adue+QMv7z+zL513S6eLUnlysnrmwLlBruM6uWxOkZBwcfHwEUImpoRpN5O67EtKy21VuvtWrPz6N/76"
    L"OiGk2VUXrf+gJMt+o16X1i1faawJ9jBbmidbL2A16rQcB9f10SUZH/A817kQABcoApPPZ3Zu+3T9juH1Ky4z74r/yVV3+fdc"
    L"dfjYzjPHiqczWbfY2jd/aPvJ0vjLu0p7npQkZmRN7vM8n6geoS0Ypz2YIhwKUfUbRr5SWiYMpdDRuTxkRqJ3qKrsyrIi1+oN"
    L"V5X8ZFeiLR6PrmCVNEBV5MnXs+SsAvNFi1K1RbNZbzqOW74QAB9wgPyBwuF9r0zuTa/tHjOF2wRHY3XHxb2r+y/unS2cbV0e"
    L"fN+lWlVT7jnypTtUTT86tKy/z/QMzLAJuqAhHGZbBRZyC6JQKPT0uoON7q6eGUUIHNtVkPCDhpHo7ekc9GxXn83miAlBLGGS"
    L"iGkMyiauW+XkeN45+Gp53LbtzDsBSIC2tA6GbOxYT3B53G81FnXerNKsFfGFSkQJa3pA54w141wWvjinysr+9mD8k77vUVQa"
    L"NOwG1XqdQr5AMV9AVVUtoCkrXbvRFgzolhELEItFjUg4nNI01bBdh5zwmSuUMEotYtiEUjaKIRGJxrSyNXmq0ahPvROAAYSB"
    L"davCwzd+7/L7b7yka21C1jT8VgtcCaHKNCWHer2JcBR2Fnc9/UD6Ry98yr3dy1H5vCREsmqVaTabFAtFqpUqkiTR19dHNBKR"
    L"69VKeyiYam9PJYhEIudfrMoK7ak2NE2nWCgyW6gSajnEk747NVWu7t+//+Dp0+OnlXdwPgEMfzh++Wf/6f3fuXlV3/owQRdc"
    L"G99f1JZvSPi+hPBlaq0q+yqHfggQMSMvI0lby5Z1R6lQpN5oUKvVkBVBPJ6gra2Ner2+mFkkQavVwnEcFOU3LgkhiEejaLKg"
    L"UNLwpRjphaY4evToocOHD7xQKpXq4m2cV4AI0H6xedFdP1jzNxtWJYbDnl/Bd5ogJAQqQpPwdQnP8dElna2ZF1/6t/mfbgFI"
    L"p9Mf3bN7txWNRWvNVotyuYyu6wQCQTqWdYAPzWbzdQca3/ffrGFJIhgKs6w9SdSMeNu3vXhy375Xnt+zZ8+vzjn6VroPAFFD"
    L"Nq78es8Xb+8PdWpVv46GhqoHwJbx7BpNxUNRNfBsFFdmV23vj37xi1/cI0nSV4QkDV9yySWy67hKNBbD9zw8XyIWjxEwVarV"
    L"Kqqiouv6+UrUdV0URXnT2WCx1Ja8TZs3nanXy4d27979n7VarQpvXU6rQBTo/XjsQ5//Uscfr9FMVRIBGcMMg+NzevZU5v5d"
    L"/7ApGDJTK5MjIVo+LxVfqXV+deiGWDx6/dDgYJfke+rOHdvljRs3StFojN6ePoQsEW0XuJ6P15AJhg00TT9/qDl3QnsjQLVa"
    L"9TY/++y0VSqdeuqpp76/bdu2fSztT8rbAIRUSR39XHLDZYaiC1s4BNUgbsPmyVPPbfnK7m99p2/tiku18n994NrG1QQJMv3+"
    L"bPD2T23ACBh4nk8sHmdgaATP97BKFmenpzEiMdRIHbeoEQwFUTUNISR838d13dfM9qLZtk2hWGTb1q1SsViM5PP5xq233jp2"
    L"6aWXbv/Wt77FWwFISwDB5dqytRdpI7GaqGPKJiphHjn5802tPw3Ofe++f3ysXK5Ejx88qr6aPkRYNvnEPdcRCIUBkAUkEkkA"
    L"PM8jmUzRt6KfXb/aRWMhgO97hEKLdY0RCGCGw4TDJpqm4TgOpVKJSqVCNpfj+PFj5LI5KRwOhwcHB98P3orJyT2PAIW3AhAs"
    L"5v3AWGBoIEJI+ArEtQhPVJ5t8mfxa0aH+qXe3j6h6zqrVq1i/2Ov0hlfzoe71uD7Pp7n4XkerusufXdxHBfXdRkdHaVRbzAx"
    L"McGJEycJm2F6e3pRZBlZViiVSmQyGWamp8nmcmSzWUKhIENDQ3R1dSmpVLItHDbb7r//2zcDP367CMidyzvbRmJDa33JR1M1"
    L"Zpwssc906ldfexkuEqqi4fkeY2NjyLcJejp7aDZtPM9ZdN7zcGz7PMi5y3FauJ5Px/IOzIjJ/Pw8O3fsOJ9ChRC4jkOxVKK3"
    L"t5eVo6Ms6+ggHo+jqupS2hVcccWlHzxx4siPAek8wM6dO0Umk/lsqVS6d2JiYp2/pSpERWAoOofWj/MH116F63vk8nnmMwVC"
    L"pkkqnqB3RR+6odNq1RfbIOcdXqzdXcfBeU00Fj99hBB0dHTQ3t6G7bhUa3XSc3OcPn0aMxplbGyM/v4+DCNAo9Egm81iWRaG"
    L"YWCaZuKcYhSArVu3fjmTyfy9EELr7+uV4vE4x3+9j7Adoqw0GfnMGoQQOI6LGTBwoyEWcjlymSx6IISmKUTCJkIRi60+T8Lz"
    L"ZVzAQ8LzBbbj0HJcXMfFbtnYjk2j3qBSr5EvFLBKRWRgbOVKVq9eRVtbCsdxyefzWJZFPp8nn8+TSCSwLOvcWlWVHTte/smZ"
    L"MyeusyxLt22bYrFI2bKoFAtEAmH2BI/TVhYoQY1IJIIRCpFUZULhMPV6lVq9he3YlK0ikiyjqDqqqqMZITRFBh/qjSaNUolq"
    L"o0SjWcdptLDtBnariee6tMejDPf30Nm5nHg8ged5lEoWpVKJfD5PJpMhl8sBEIvFOHr0aA5YBtiK73tXjY+PJ3t7VzDQP0A2"
    L"n2VqapK9iTQVq8E++Sj8copK1UJVFMLhECNDw8QTcdrb24jHYzSbTZqNOi27RaNcpmy7iwvadXE8D9/zcT0H3/WQPA9F8lFU"
    L"iaAeJBAIkEqlSCTigEStVqNcLlMsFslkMiwsLFAul5EkiXg8zsTEBHv37j0JxIGsAsi+79O5fDmO59Pe3oFpxlBlnWM/n2Tk"
    L"A2tZf/0VlEplCoUslUqRbD7HsROnKZctAoZOW3uKRCJBKpkiFoui6R7NZgPHlvAcm0qtRqvVQlVVdE3DB1RVJZFIkEwmkWWZ"
    L"RqOBZVlYlkUulyOTyZDP52k0Gud7RzMzMzz00ENbgANAHago5zYP2/H41x/9kGajQUfHcrq7ezndkcZMdSGrOp3dEfoGelFV"
    L"8FyJfD7P3FyauZkpFjIL7Nt3kEIhRygUor+/n1QqtTi7iQQgYS9lJSHLxGIx2tra0DSNVqtFqVSiXF7sBS0sLJDL5ahUKggh"
    L"iEajOI7Djh07io8++uhPgSeAU0AaqEvbt+8+e/Dg3u5YLE4imWJqYpITJ44zNzeHlSmCIuju62F4ZCXr1l3CwMAw4XB4sRSV"
    L"QJIEvudQLBXJ5dLMzEwzMzNNpVLCB4yAQSQcZvXYKuLJJIZhEAqFcByHSqWy1DossbCwQCaToVQq4bougUAAXdeZnJx0n3zy"
    L"ye2Tk5OPAnuAWSAPNAGkLVtemD916lg8m11Q77zzD5EkaLZa1KpVCoUiC/PznDp9ijNTZ7CsxYebpsmKFUMMDA0zNDhMMtmG"
    L"4zi4ro2zpPVK1SJtTZKZzZJPl7DKJe67915M06RYLFKpVCgWi2SzWTKZDIVCgUajgaIomKZJq9XixRdfnNu4cePDwHPAFLAA"
    L"1HhNN0R6+ulnP6co8ndALD98eB+6btDV1U1XVyeRSAxV16nXqjSbLSrlCplMhqNHjzI9fZZsJnN+A+rp6aOrq5uOZR2klrWh"
    L"CpUHDnyZtvQlrOpej1UukUzGueWWW5icnDzv+Dm5eJ5HKBQ6p3X/8ccf3zkxMfETYC8wt1Q6tFiM/et2XgYGBm76wQ8e/Fky"
    L"uUwHQbGYJ50+w8mTJ+ju7qGnp4/Ozi50Y7F291wXx7FpNBrMzs1x/NgxJsbHyWaz2LaNrhvcfuun+eeJ+4h/4Sy/Xr+CP/+L"
    L"L1CrVbjttps5ePBVcrkcpVLp/JkgEolQr9fZs2fP/NNPP/0z4BlgckkyVd6mByUt1T8AXXffffctF62+6Pr+geFrenp6pcWf"
    L"JLLZOQ4d2kez2aKzs5MVA8NEIzFMM4znL6ZMSQLbdlmYTxMMBilkS3z/+N10bKjwyIoKt9+xgVVjYwwNDTI1NXU+NZqmiWEY"
    L"zM7O8sQTT7w4Pj7+MLB/yfHsktZ93saUpR9lYOGhhx56GNgCdMZi8dGbbrrx6iuv/NB1IyMrjY997JPIsrq40VUKbH/heTRN"
    L"Y9myLgYGBwgGggSDAXp7ezFNE1mawT3i4qR1oILnuviey8zMDOVymUAggGmaVKtVtmzZMvfcc8/9FHgWGAcyQIXf0vl7nYRe"
    L"W8gtXfrSVh0B+oDewYGBlTfdfMstY2Orh4eGRpVwOIzvC2zbJpud5eWXdxMMBunq6qazswtZUvjb3X9E8LICT11b5bbbbmPt"
    L"unUk4nEsy0JVVSYmJvzHH3/8l+l0+mfAQeDsazPMhZj0W+6LpQgpSzA60Ab0Ah3Dw0ODt9++4a6RkdG+wcGViq6reJ7Askqc"
    L"PTvO1NQ4QpM4NPsSr2w6wfXX30BfXy+jo6Ps3buXXbt2zT7//POPAM8vZZg0YF3IrF8IwBtNLEXmHJC2BJQC2iREz+jKsVXX"
    L"XHPNx0ZGRobWrbsk4rqudOTIq7i+jVUsLi5+36OQLzgPP/zw1vn5+f9gcUdNA7l30vrvCvDGMeciJC9FR1kCii5FKdnb279u"
    L"w513fvii1WtXHzl2UODTsiyr8uCDDz4GbAPOAPP8lgzzbgG8HdC59aMuRUjlN+soxmKno7k02wUWtf6mvP6/AfB2QOcidO6S"
    L"WOwkOIDN7zDrr7X/BrGGxne0DNalAAAAAElFTkSuQmCC";


static const WCHAR* WIN7_NATIVE_READYBOOST_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAARQAAAEYCAYAAACdsgkCAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAA"
    L"DsMAAA7DAcdvqGQAAABmZVhJZklJKgAIAAAAAQBphwQAAQAAABoAAAAAAAAAAwAAkAcABAAAADAyMzABoAMAAQAAAAEAAAAF"
    L"oAQAAQAAAEQAAAAAAAAAAgABAAIABAAAAFI5OAACAAcABAAAADAxMDAAAAAAIvvHMbnUA7gAAP8zSURBVHhe7L0FgF3Xdbbd"
    L"r5CmnKb5mjZJ0zbUQJM0zHZijh0zkyy0ZEu2JYuZpdEIZzTSjFgjHGZmZmZmZtYI33+969w9uhqPk7RN+/2x77aXzrnn4tx7"
    L"9nPetfbaa/+Bozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozma"
    L"ozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaozmaoznaf6sB+IjYX4n9jc3+2nabxv2/FPsLsY+K"
    L"/akYH/8nNvtjsT+y2R+K/R/byzqaoznah6kNDg9u6+0bQHt3Dxpb29DU1o7m9g7db2lrVWtsbkJtfR3q6htRXVOH8ooqlJZV"
    L"oKS0HMUlZSgqLkVhUQlKS0tRXl6OyspKNDY2jvf19R0iYGxv5WiO5mgf5NbT3z+vq6cbXX396BkYxPjVa5i8cRPXbkHtJm6p"
    L"Xbk6icHhIbS0tqO2rkEhkpObj7T0TCQmpSAmNh6RUTEICgpSCwkJQWxsLHJyctDQ0HDO9naO5miO9kFuzS0tIY2tLejs7cPg"
    L"6BgGx8cxfv06roqsuCZ2w2bjApTu/j5U1tYht7AI8ckpCImIhJ9/IM5fuISTp87A49gJHDtxHB7Hj+Hk6VM4d+E8gkKCkZuf"
    L"d8X2do7maI72QW7l1VUdxeVlqK5vQEtXFzr7+6egMnHjhqqU6wKU4fExtHS0o7SyCunZOQiLioZPQCAuXLyM4ydOwfXwERw4"
    L"6IKdu3ep7T94AIePuMHz/DkkJifB9naO5miO9kFuAoirxeUVqGpoQGt3Nzr7BjEwMYnRG9cxdlOAIjBRoFy5gqaODhSUlCIh"
    L"JRX+wSE4f9kLJ86chetRdzjt248dTnuwbcd2bN66BbucdsN5314ccT+KiKhIB1AczdE+DK2wtAz5xSUoq6lBY3s72vsH0T9+"
    L"BSPXr6lN3rqlQBmamND7+di4pGR4+wfgzPkLcD9xEvsOuWDrzl3YsGUrVq9dg+UrV2Dt+nXYuHmTQiUgKNABFEdztA9DIyCy"
    L"8vJRUduAhtYOtPUNKFCGJq9h7MYtjN+6iQncwtDVSdS3tiKnoBDR8Qnq7hAobh7H4HzgoAJlzYaNeHfFcixb/i5WrFqJDZs2"
    L"qmI5f/GCAyiO5mgfhkaFQqCUVdehprEFzd296BkZw+CVqwoUwoQ2cGUCVc1NU0ChQjl97jwOu3tgz/4D2LRtO1asWasweeud"
    L"t/H20newcvUqVSmMo9jeztEczdE+yI2ASM3MQnFFtQLFKJSRazcwKa4OR3u4pUKpbWtV+ETExOKSjy9OnvWE2xF37N13AFu3"
    L"7cCq1WvxzrKlWPzWEry5ZLHu0wXiiI/t7RzN0Rztg9zy8guRlpWN8pp6NLV3oaWnTxUKA7PG5aFRodS1tCCvqBhRcfEKlGOn"
    L"TuOQy2HsdnLGho2bsXTZcix5+y2FCY0qhfEUDiXb3s7RHM3RPsgtN69AR23yistQVd+E9p5+9A1bLs/ELUudcDs4eQUNbW2a"
    L"g8IkNi9vUSinzsDF1Q17nPdh85ZtWLlqDRYvXjxlS5YswdKlS+Hh4eEAiqM52oehUaGkpKajRFweKpTWrl70Do0qUK7YXJ7x"
    L"m0Df+BhqmpqQmZunGbGXLntr/glzT3bs3I31Gzbh3eUrFSKEyRtvvIGFCxfq9vDhww6gOJqjfRiaKJT2hMRkcWVKUS0KpaN3"
    L"wALK2BVMXL+FKzeA0cnr6B0e1lwVukcEChXKqdNn4SIuz65dTtggQFkuQLGHyeuvv44lby7GUbcjI7a3czRHc7QPcouOjt3B"
    L"lPkjRz1w5NhxjYsw2Eo7e+Gi2inPc/A4eUJzSpyc92DLli1Yt24d1q5di5UrV2LZsmV46623sGjRIoUIjUAhXFatWImzp89u"
    L"tb2dozmao33QW1FR0ffDw8PfDQgI8PDy8gr09/dP37NnD959912sXPEu3nl7CZYtfRurV63A6wvmYd7c2Xjh+WfxxOOPqj35"
    L"xGN48YXn4ObqeuLAgQMup06d2uzt7f1icHDwQ2lpBZ+yvY2jOZqjfVhbXl7OrrNnTuH8mZPwuuCJS+fO4IS7G3Zs2YjFC+cL"
    L"RH6F++69G9/9zjfx9X//Mha+Pr/M9lRHczRHc7Q7W05W5vYTxz3gefY0AgP84O/ng5MnjmHrlg1Y/ObreObpx/HA/b9QoHzl"
    L"y1/AooULEm1PdTRHczRHu7Pl5Wbv97p8EQH+vggJDoSfrzeoWLZv24QlixeqQvn53T/BD3/wHXz7W1/H/Lmzk2xPdTRHczRH"
    L"u7OVlJS4RkdHIy0tDSkpKYiMjISPjxcOHnBWoDzx+CO45xc/E6B8D9/4+tcw+7VXc2xPdTRHczRHu7MJUDwSExO1lGNLSwuq"
    L"qqqQkZGBM6JS1q5ZgTmzX1Go/Pzun4nb8y3Mmzsnw/ZUR3M0R3O0O1tJScXZ5JQ01Dc0YXBoBAODw2hqaRbFkoJjx49g3bo1"
    L"mDXrFTzyyC/x0EMPYOnSt1NsT3U0R3M0R7uzFZWURrNebGNTC4aGR7XW7MS1q2hoqENAoA9cXA5i9eqVmD17Fl544TmsXbvK"
    L"oVAczdEcbeZWXF7RnpGTqxXwh8bGp2rLtrQ0ISwsBO7uR7Bp0wYsWfImXnzxeWzevLHW9lRHczRH+7C3uq66v6+pqflBYUnh"
    L"w3kFebtYQCkoLBwVNbXoHRzC6JVJ2Q6gtLwMwcGBcHNzxZo1qxQo8+bNwYYN61BQUPDVvLy8P7G9pKM5mqN90FvHQMdncwvz"
    L"3cIiwttYkT40PAz+gQFaoZ51S1gw6ZDbEazfvAUbt27T8o4uR47qcecD+7F+4wYsX/ku3lzyBubMm43Zc1/Dy6++pPbuimXY"
    L"uHE9nJx2DYiCybh06cKp2NioBaWlpZ+3vb2jOZqj/b62gYGBvykuK5sVExubxbquPn6+CAwOgl+Av9Z5ZfHo6NgY3RIsNCqT"
    L"i94+Oq+HhadZ1nHRkrew4I038eocCyAEyazZr6rNnT8HCxbOx/zX5+ltqpUFC+apzZ8/F6+/Ph/Llr2DrVs3Y98+55Hg4GCf"
    L"nJycR3t7e//K9jEdzdE+3A3AX9fW1l6srKxcYjv0/5tWV1f391nZ2eeCwkJvehEgoSGIjo9DXFIiYhLitfZJSkamLoXBSm1J"
    L"aelqyekZalFxseLqhOKyrw88L17A0ePHRKUcxJ69zti0ZTNWr12lSmTJ24vVFr25EG8sXoSl776j20WLXsfixW/gnXfeUpAs"
    L"Xfq27tO4zwmDrJGyceNGcZvcJkJDQ93F1fqc7eM7mqN9eJqA5A9HR0cPMzfj4sWLOH78ODIzMw/b7v5/1pqHmv82NTP9tCiQ"
    L"ycjoKMQKQBJSkgUQaQKPdIFHFrLz85BfXKQ1Ye2NNWVLKiq1cBKBklfECm6ZiE9OQkRMNHzFHSJYuHgX19rZf3AfdjntxM7d"
    L"O7B7zy5s3LxBIbNh03qsWbdaXR4OKTO+Mt3Wrl2N1atX60xl2ooVKxQunLW8f/9+nDlzxq+5udkBF0f74DeqEpHpQyUlJQgI"
    L"CODVFdu3bwdn5Ur7Y9vD/tdaR0fHnxcWF+8SeIzRVaEKITgIkYycbIVHSUU5ymqqUF5brVZRV4PqxkbUNbdphXtuWeOksr4e"
    L"NQ2NqG1s0ufRCsUy5fWSBExxomzoFoWIcvERd+nshfPwPH8Wp8+ewhF3NzX3Y0dx1OMIDh7cj71798DZ2Um3tD17dqvx2O7d"
    L"u7Fr1y7s2LFjyrZu3YoNGzYIdNYoZAQuEzExMRtbW1v/zPbn/j9pPT09X6yqqmpjQp9cRCJthx3N0f57jTDp7u4dLS+vBLNH"
    L"z507Jx3noNYFYQf53zzxq6rq7snMzSuJi6cLk6huS25hAQpKipFfWiyAqENtcyMa2lp0HZ36jnY0dnWirb8P3SPD6Boa0bV3"
    L"WEipY2AIXf1DaOvuQ3NHtxqfQ6tralblUlRWDi4ERgWTnV+gRaqTMtLkfTPUjQoXRRQZG6NukneAj8ZmfPy8cfHyJZy74CnQ"
    L"OYNjJzzg7u4u0PFQpcPaK1xpkPVoDxw4ZNVZcXLGzp079TslXKhkuC9KMF4687/Y/vz/1Sau2FhLSxtENSGftXfTMtrlXPhz"
    L"292O5mj/+SYn0F+KMOnt6R1EQX4JLl3ygushF2xcvwGbNmzEkSNH/lfKG5aVVS8Oj46bTEhJR2pWrnTwEhSWVWq1+tK6Wq1I"
    L"39rXi/aBft22CkC6RkbRPT6OztFRdIyM6LZ3fEKOW2DpHh1D39gEekcn0NY3iAYBSlNXr1p9exeqmlpR3dCMWtu2tLYexdW1"
    L"KKyqRF55GXLLSpEtIMsQoKVmZyMxPRXJmZmITU7UQtaMx7BCPuM4/gFBGvA9d8lbXCgvnDl/Cac8L+DUaRZvEqVz9LjWp90v"
    L"gGGN2u07dmmd2nXrCJe1ogiPNqWkpDxu+zr+x1t7e9cL8QlJaGnt1MS+/oERZGTmIjo2sZPnhO1hjuZov32TE+fP+vpHusbG"
    L"r6G5pRPhETHw8DiOHdu2Y+XyFZjz2my88MILKC0tfcT2lN95yy4sOhASEY3E1AzkFBQjr7gcJZUCkCa5cgoAWKWe6qNreAg9"
    L"Y6PoHBoUUIygb2Ic/VcmpoDSNSbwmLiiKqV3bBgDcrtHHkeF0trbLyqlH82iVBrbulDT0q6vX9NogYTFq7kQGCvjF1dWoqCy"
    L"AvniThEqtIKyMlFHpcgtLkRmfi4yc3PU/aKKSRUXzLhNUTHRomjiwL/HPzgM3v5BWkqSS5py0TBWiWP1OIKFS3PQWFGf9WpX"
    L"rVqD9es3wsXFZTgzM/Nl29fzP9au3sTGsKhYxMYlKUxuycnQ2zeErOwCBAaFt/LcsD3U0RztNzc5YT7S0dlbyROpp3cICYlp"
    L"cPc4qbVT6efPnTsXDz/8MB555BE8+eSTqK6u/pntqb+TVl1b+2pEZLS6NFy6gmsK1za2oKm9G61dAhABAWu/9o+MKxy4VOjQ"
    L"1QlcxU218etXMTguimRYACLGxwxfGceIwGRsYhjjct/QiMCnW1yd1hbUM44iSqdaFEhpWQWKi4vBeBG3eXl5yM3NRn5hnlgB"
    L"8grykZufh5y8XGQLPHJzc/UxeXk5akWF+aLmcpGVmY6stGTkZqYhJyMV6WlJSE5ORnxiwtRwdVhYGJgPw6FsDmt7+Xjj7DlP"
    L"HV065HYYew8ewu69+7BzlxM2bd46FdQ9fPhwl4DlPtvX9Ttt8pP/ec/AIJgpHBgUhsioOIyNX1WotLX3ICY2CSGhETVy849s"
    L"T3E0R3v/RpgMDI6WDA2Po6OzD7l5xTh56hw2btqmhZhZM5UQ+dWvfoVHH30Ur7zyCmbPns3A3ddtL/Ffbk2trb+MjI4aYSFo"
    L"LnFRXl2j2aptXaJGOjoVJFzmYnjiKsauXcPEjRsKEK4/zNT48WsTamNXxzF+ZQwTk1dkO4EBUS6d3R3o7mlHS3M9qipLteNn"
    L"Z2UgRVyU6MgohAaLa+LrB69Ll3FO1MLpUydwzN0Dbodd4OpyEIddD1EhaBV7xkROnjypj+Pj+bzgoADERIQjOT4OmakpCpE8"
    L"gUl+Vjpy0lOQkZKItNREgUoiEhLiEBcXg5iYKERHRwpcwhEeGaFJdhzu5rA1g79c6pSJdQcPuylYuLwpg+Hr16/X0aFTp07V"
    L"V1VVfcP29f1OWt/w8JmyqmoMi3vYKErt9JnziIqOV4VCqJRX1OLSZV8eK7Y9xdEcbeYm58uf9fT2V4s6ETenA3HxyThx8iy2"
    L"bN2JlavWYfGSt/H8iy/g5VdfwZx5c/H6ooW69Oabb76pV0+5on/G9lL/qdbW1vZ3cXFxrcGhIeomlJSVori0RIOjDS2t6Ozt"
    L"Q9/QMIbGJ7UavZlbY+zaLVEm1ycxee2KQMRSIoOD/ejt7UZbW4vArhz5ohrYkZk6f/bsaQUDh2udnRkQ3Y2tW7ercZ9BUh5n"
    L"0JSPO3r0qJgHDh10w8ED8rx9LtjjtB87duzCli3bsHnjFmzasBk7t+/C7p27cGjffpxwP4rL588hRNRHgrg7hExmhoAlPVnV"
    L"SmpKAhKT6FYQKKEIixCLDod/SAD8ggLhFeCHs5cu4ITnGRw+5o7d+/fq0DVHiTgyxKAtocIgrq+vr4/tq/xvte6BgVdKK6sw"
    L"OiZQHr+iM61rausFnqcFmOFoaGxToMTGJOLkibMCxaQ821MdzdHe2+QEyuOU/ZLSSgQFh+PM2QuqTN5ZukJh8sKLL2Pu/Hm6"
    L"Mt6qNauxc/cutc2bN4ufv4rDoI21tbV/anu536oJhDYzryUqKgppGekoKilW96O+sQEdPb3oHx7BxLXrqkKMErkDJDdvCEiu"
    L"YnR8TCBic2OamxnbQVJSEry8vLiUhQJi2/adurD59t3OcNp3EAdd3eEhHePMucu4cFnUiW8QAoIjEBwWjaDQKLXQiFiEhMfo"
    L"8bDIJLmdKLcTEBgSC7/ACHlOCC5fDsTFS1z86xxcD3vAec8B7NyxB9u2SuffshO7d+3FwYMuWk/l0mWBjEAtNjZa1QotLiFW"
    L"3KAoxCTGIjQqTAO5fiFB8A70x0Vfb3hevoiT587CVdwg/h0cbuaoEL/35cuX63fPEbf09PRnbF/rf7r19fV9paquXidDXpm8"
    L"ptbbN4DWtg4UFBSJKjumQeSMzDwEBoTCS76v48dOIzrKoVQcbYbW09O3uqGxGYzu+/j667ozK1auxeo1G7DkrWV4bfZcvP3u"
    L"cqwQkOxy2o0Tp06qv8+h0cNH3LB1+zbMnz8fdXV1X7C95K9tra2tn/by9h3gollJyam64BYX3iouKdO4BgExKu4KoWHgcV1E"
    L"902xG7J/7cZ1USVy4l+dRN9AP1pa2zX+wc9/7vxlHDh4WADihM3aofcpPI57euOsdwi8BQaBUckIiUlHeHwWopJyEZtaoPuR"
    L"iTmITs7TY5HJlkWlyG2x8DjLwmJzdRuRIPcncpuNsLhMhMVkICQ6FQHhifAOioGnVxDcTpyH00F3bHc6oCM3O3bt1GH34ydP"
    L"4LK3l+a3MBmPwVu6PTFxsWLRCI+JQEhkKPyC/XHB+yLOXvTEqTOnmQCno2tUKlQpzLqlUiFUuKTHsWPHCv8rkxSzcnOTWaZh"
    L"YGRUXRsaFUptXQNS5bcJD4/Erp3OOOJ2DJcu+ihMaFRsMTFxFbaXcTRHs1pZWUVJVHSsrorH0YaNm7YoUBYuWoI5c19XhbJN"
    L"OsMlH28rGzUpUee/MPeCORZL312myVnS/tD2ku/b0tLSz1BGBwQGiy8eq1ZYVIKq6lp0dvVgaGQY10V5UH0QJmZ749Z1AcoN"
    L"vY/xkcHhIS2CRCBd9PbDHnFFNokq2LB5l+wfwfFTl+R4mHTaJEQlZgsc8hGeWoSItGLdhiYXICQpHyEJBQgmGFJKEJJYiKD4"
    L"fATzmFhgYgHC0koRm1uDiNQyhCXzMUVqoUmWhSTkITg+F0FxOQhNzFULjs+Wx2Sp+UWn4HKwAOaSH45KJ9wlCmbdpq1Yv3mb"
    L"KqUTZ87C1y9Avwd+rwzahoaHICqG842iEBwaBP9AP3h7CxDPnlUXjK6Yq6urKhPGVPjdc50gup8CmZvx8fHftn3dv7HJV/tH"
    L"dLPoVvJ7JkwmrlzVIWP+LhERUer2UHGtWb1BIeLhfhJ7nQ/Cafc+7N27X9yfxELbyzmao9H1KHfxPHdBE6+YB8GlNbkA+ILX"
    L"F+n24CFX9e2Z0MUs0oysTB3l4MgE3Z8Hf/kQ7rrrLhw4cMDX9pLvaXKe/mFAQHDbUZHPFy95gSM5VCY5ufnqq/f09mN8YhKT"
    L"N+jK3I6VUJnQGCO5dm1SgVNRVYmQiEgcPX4K2+TKuXrDNuxyPozTF/3hFRiN4Og0xAk4IpMKESGAiEouQlR6Ofzj89SC5XZA"
    L"Qr5aaFIxguRx3Iamlsp9xQiTx4amlSEmtxY+8QU4EZCIIDmeWNiIiIwKRGVVITq7SvbLEcnHZZUrqGghom7CRfFEpBXqPi0o"
    L"PgNhCQKY2DT4hyXgvE8I3Dw8Vbls2LoTG0VJHTzkJurqonTgCHGJRKnYgraRESEICw1GYGAg/Pz8cOHCBZw4cUKVyqFDhzRj"
    L"edu2bToCx5gKl0RdsGABH7fe9tX/2iZf8Uc4xeC0KE4L3KIGBdq9vb1a+pLwYjD43XdXYNGiN/HWW+/oiomMIW0XN3Lbth3i"
    L"iu1DSkraLttLOpqj/cEf+AcGee3c5XTL9fCRW1y39/WFb2Dtug1w9ziuysU/OEjT2wkUDp9SoWzcvAm/euxR/PKRh/Hggw/i"
    L"pZdeYpDwPSdWbW3tj07IVY5rAvv5BypMomPikJ2TpypjeGQMwhA1AxIDFro74+LaDI+OaIyF+Rt79ktH3LIdm3c44egJT/iG"
    L"xIgrkq7uCpVIZKp0blEcVBUxmVWqKAgPKg7ChKojJrNSQRCXXY3kggZklrUhrbgZ6eWy5X55OwoaB7HrmBe+9IMH4XouGGVt"
    L"w8iv70FOTSeKG/uQX9uBgrpOZJY3IaeqFWkltUgqqER8LiEjQMuQz5JegNjsElUu/GxRKQXqWtFVoot05lIA9rm4Y/O23Vi3"
    L"fjO2SgflXKmggECBSZiOHsUKXBhnCg4OVqVCqJw+fVrVikBcVMJeBQsXKOPQMmMrc+bM4fEg20/wvi23sKC4tLICJ8V9PX76"
    L"lLqVVCkcTuf7EFbz5s2T15uH559/Ue3NN5do0t0mUbIEClXK5cvejbaXdDRHs5r435+17f6B5znPom07tmPfgf046uEOb18f"
    L"tZS0VJXlhMlTzzytCW6zZs1SH55r+/JEDgkJmVqCMzs724nSnOqHKig0LAJMny8oLEZ3Tx9GJq5gbPKqBl2vXLe5N2JGpTBQ"
    L"yFwUJoERJGs2bJYr+z6c9LwsV/s4dWfi04sQm16M+OxKgUipgKMQvrG58IstQGhKud6mUkgtqkZ6oQAktxxZRbXILWMGbBta"
    L"esZR2zqAjKIaFDd0oqptCNWdI+gcB/aduoy//8I3sf/4OfRfAZp6RtE5Mome8esoqG5ESX0rKprlOa3dqO3oQdvgqG7LGlvF"
    L"mpFdVoXMkiqkFNQgLqtCYRcuYIlIylMAxjBWI8rlsl8Ijp30xI6de7SzbhNgnjpxEiFBwTq0HR4eDk6ApDH2QnV4xvMsjrgf"
    L"xUGXQxog375zh/4uVI1L3n4Lr82ZLepnU5EojY/bfo47WnFpsUt9c5N8/1e0gBSXW2V8rL2zA1lZGaKCDuPttxZzMTM8/+xz"
    L"eOxXj+L+++/HE088gbffXiqu1jrNT9q9ew+Sk1OX217W0Rxt5nbYzS2bQdjde5zgdvSI1hThibxi1Uo8/uQTeOmVlxUklMQc"
    L"gXByctKAIeW3v7+/Z1hYWBqDkDxJL3v5IDAoBFygnMFXwuT6jTuHgQkRM5rDXAhO2gsOj9CRmZXrNmGn8wEcO3sJgREJiE8r"
    L"QJJc+ePEtQhLyEFATJaokiKNfYSIOokW5UHXhe5JQkEt8iubUd/Rj+KaZmQWVKCivh2NbT1o7RrAhLxpS8cgsouq0NQ9hL4r"
    L"N9EzdhP914DDZ73w1R/8DKcv+2FCPlfn4AhG5HMPX72OvIpKTctvGxhEOxPu5DMPX72B0voG5JdXorm3D/Xtnahu6URZc4+o"
    L"mW4U1nYip6JFoFaJuIwiJGQWyd+Sh/jkHEQnpOko0+kzogy27sLyd1erAjghyo4QIchpTIjz9fdTOy/uCuNY/H1YaoEBchaD"
    L"WrNuLd5e+g4WLHyd23aB/B1xlbKKshcaBSasl9ve3aVAYUxs+coVCqeLF8/L77pVl2DlkquPPPwQnnjscdxzzz2qWEJDwwu9"
    L"vHw63d09SpKT0563vayjOdqvbydOnQrdvHWLzjKm306APP3003jsscd0n8OYPj4+coKFIigoSOHBxy98Y5GemBzu5JWU2aCs"
    L"S1JWVYn+4SEFxzXR1vbxEgsqQFtXL6LiEuF8wEUDmFQkdA0CIpMUIFQkUUn5ChIaA6O88nMbnV6K7Mo2tdTSJnVJCura0T4w"
    L"jv6xa6gRNcEU/q7+YYyMX0Pf4BhGhShDo5No7xnA8OQt9MvtwUkoQA4LwL76vR/hvK8/5JBm5g7KFZ0ZuJw/1DM0pLeZdMft"
    L"yJVrKKqu0pT8jsEBvb9nZFwgBDQPXEFdxxAqBS6F1S0oqGpGXnkdknOKkZ4nblJyNiJikxUs4VGJOO3phW0792HZirVwFreC"
    L"3yNHgxgYn0qGE8BfuHRRq84xV4VqhaNJBMPqtWvw7orlmDX7NcyZO//G2bPnZ/E3Lauu/mlVXa3Gphj0pluZnpmhUGJVumee"
    L"e1rr5i5fvozrDuk6zg89eD/uvefnWLL4jUm5UPxYTw5Hc7T/Sjvr6XmCvjnT7l9++WVNvad7w+QwufJp8C4hIUFnJNMt4tVx"
    L"8VtLFCYEDJPWGH/hSWzBxIIIgcIt4yVXrt9C39CoztvhBLqtO52wcesOHRmJik/Vq3lUWpG4C4VTRkVCkESllSA5v1Zv51e3"
    L"o7n/KrLKG5BRWiudeAJNvcPoEWCMyZv1j0+ie2AEo5M35Op8E4OjE+J2XZXPADl2HSNyrGuY6fy3IB4Ojl/2x7d/fi/O+flj"
    L"TDrf6NWrGLgygb4ReY0bAiJ5vf7xKzodoGdkTJULJyByRjMnIHKfkxB7x66qOiqpbRG3aFxgNCqKqRe1bb2oau5CQWWDgKhJ"
    L"3LBaVV6xKQLJhCwdgj5+5iJWrduo7ozLYVdN1+cQM1P4qSqoVAgUDi3TBaJSoapkdi3zht5ZthQvvfwqnn/hJUJjqLq2RkFi"
    L"fgf+JlQ+fPwrs17G/Q/eh3vv/YX8xgt1MfgHH7hP7B7Mn/daV2xs7JRb7GiO9l9uERERT+Tl5X0+Kyvrq1s2bVaYMGhnRiVY"
    L"R4SjP/TD31j8pk7NpxT3lpOdRY/oq49M3K4iT6k9KR2S+wRKc3sXImMTVJUw4OridkyTyxJTMqVz5SgswtLKrFGZeHFrEosQ"
    L"J+5Mcn69wiSvvE2u+u3ifkxiWFwVdtTKpnYMCSSGr4sLI0AZlTcjJMYFZCNXxa2auKZGuAyMisoYE+Vx5Qb65fagPHZA1JLb"
    L"RT/8+09+jvMBQRiT5w1MTGJg8qrCov/KpM5W7h4d1+d2Do8qjPhe3SNye2hcVMqYuE+iaq5cVfeopKZOXaGWnn609g+Kqrmp"
    L"n6eldwSVLV0oqmkV5dKC7NJ6JGaXCjALEZOaB5+AYFUgK1evUgVIeDAwTqiYGrmsj3vsxHGFDr9/xlQYT6Famb9goQKFAJK3"
    L"U1XC757brr5erfPCUpb33n8P7nvgXvziF3fjueeeUYXy9FNPYPEbC8rlgvEx2+ngaI72u2vV1dWfcHJyusERhmPHjunVkvkg"
    L"m7Zt1WDgIVcXvVLy5CVMmtvb7DJeb7s43GdOCeuPHD99RgOu67fswAXvQMQmZiEuNV9Nc0jSyxApAOHQLod5OSJT1zWB6vZR"
    L"ZBTVoUxcm86hq9KBRT2MCqzk9ak2BqWjj1y7IepCYCCwYAem+uA+3Rs+hqplQFQKt30CjD66QlduYlh6nmdQJL7+01/grG+g"
    L"viYf0yuKhADpF0h0DtDVuYleBmoFJAQIYdIzes0Gl2sKms7BYXQPDsprj+kM6ba+AX3vgUm6SA0obWhDQ/cgGvvGUNnWj5LG"
    L"LjWOHiWKO5ScVSAuYzJOnzuPzdt36ExkgsPX1xeRkeE6rcDPzweXvS9p7RW6Pgyob9iyGUvF7SHkX3z5Jd2WV1YoVK7euqYJ"
    L"gqnpKeImbVdX5/7778Vdd/0UTzzxmFwwPOdGRob8dNeu7W/YfnpHc7T/mVZZWfl3u3fvGeNUeyZnrVizHms2bdKRmFOe57SE"
    L"YlJaKlraWjX4yhPYGIcmme06MNSvK/Lt3e+MdaJ6XN2P68gNYyQxqSUITihEWFIFQlMqcSkyE96xmUgsrENCYTXya9swIIQq"
    L"qxdlUlWPnmFRFwIFdlIGWceu3lTlwSApj3F/VAAyJKAgAKZMXBSqGLotdHnG5bF0Y6hkCKEzPkH4ynd/jAu+wZiQDz9y5bq6"
    L"MFQ0g1cEPMPjFohky1hJ97AAhTY0odsucW8YyO0eGFa3iGAhTHiMj2/oYjylFrkVNWiX51R39KGhd1RdtdSiKqSV1ghYOkWx"
    L"1AlUGMDNQWBYFA64HtHRoO3btuDSRU/NVwkO8oN/gLdCha7mfoHK1t27sXrjegU9i2dzxcO3316CkpIi+R2uoaAgDydPHtdj"
    L"XLjsrp/9BI89+gh8vXwfsP3UjuZo/zutu7v7k5u27cTid97FalEXLkeP4ZKfHwLCwrQmCEcPTF7DpHRcbgmXa+KCMGWeUp3p"
    L"44wNsPgQg5GMH3BIlW5NdFY9YnNbEJhcjjkrtuFnj78I/4QMtAxPaqdrH2Jtk0mMS8cnCDh5UBihZqBC10ZEggZeDVTU3ZFj"
    L"VCpDol6GRD0RJpzFzBR0jjARHARNak4JNu3eh9yiClVYxj2i9Y+IAhlkKQVRRQKUXvk8NB4zWxpjNp10jQQqjK2wcpwBSpvc"
    L"rm3vFHXSj/quPlEoAp+JG4gUcDz4zEvYctBN/9bK5l5xl9pRLO5QXnElouOScfLUGezcsQ0bN6zB6ePuCpXIqFAdofE8f07z"
    L"Sva6uGCb0y6sWLVcC2k//fSTuoTqwkXzcNbzJLx9LmLb9k06mvPorx7Gc88+fSXI1/d3WpLC0Rztt2rSv/7w7IWLOtXey88f"
    L"4dExWkmeeSPdvT02mNyU/27nl3COTmVtnTw+EKvXb8KWrbsQFBqD5PQCJGYU6ShOdKook5RCTUTjsG9WTRdWObnh77/8bSzb"
    L"5oxegUG3yIU+6fSMPxAoBiTiyShcqEqoTujW0AxQxmyAGRE6KFRsQOHozMikwEmAwsW9Bscm9flj8jgqmR5RGgQUwWGAYgBC"
    L"oNjDxN4IE8uG5bZAxKZWCBRaOxWLQKVDXrexZwCN/UNoluOz31qOT/3b17HjsIcOX9d3DqGquUeA0oSi8lqUlNcgKzsXly9f"
    L"xq5dO7B61QocOnQAoaHBWh6BgdozAhXWV9m911mLaXNxMqqUp556Aj//xU/x8ivPY83aFXjr7TewaOEC5pv0X7583jGC42j/"
    L"b1pmdk4Bq8cTIKy5ytqrnALPFfeoRlSR3LymULlyY1J99sraGgXQ8lXrcfDwSUTGZoiaKUUcR2wS8xCemKvGEZ24nHLNPi1r"
    L"70emXJkfe20hvvmzB+TqnacuCYd1hQ+qJAxIqDRoxtUhTHi/BZObU0AhTAxQBrl+sagbDgVzhjOT7VSJiAs1KOAifPg8AwgD"
    L"FAMXo06mw8Q8nsZhZQMUWodAhbkrCpOhETT3DaGpdxBD8l5n/IPw9R//HM/MWYiy1i50jl3THBoGbutae1FW24yy6nqUV9dp"
    L"giBjWIxbcc1lFsUOCPBT9ccJiMxTYTxr3z5nsAI/1wt66aUX8KtHH8KDD92Lp595HPMXzMYCAU1ISMhPbT+tozna/24rLC0J"
    L"qK6vE4BUoKZBTm7ZtnW0a1EjgoTt5k3RKLduKFDGro6K25AHF3d3rNu8XeeyRMZnIymjAuFxBYhLLUdonDXvJiGrDFkl0lmk"
    L"45Q0tepVe1Re72JonAJl3tI16KfqEIiMCggY2yAQCJQxqgqa3EdjEhrBMn7tlhy3gKKQEXfHxFAIFBZuGhwft9wdAQpHfwiU"
    L"AZE/zEsZFjeESoQAoXrhPo37BMdvAgohQreHxmJRqk5kS4VCoHAImaNL1W1deGnBG/jevQ/AKyJa1Ult1wBa+0YtZSWfn7EZ"
    L"lqssraq1ylOWV2iVNQZjmXTIPBQfHy9ERUVooPb06ZOaQ8TkQ64T9PLLL+KZZ57Cww8/hAcevAePP/EInpXbft7e/yOV4BzN"
    L"0X5tK6usdKPS4FAw4yQtApKRsVGdVGaUyY0b0pu1SSeeGBW/Px9OB/Zh5bqNOHPJT0dvYsStiUqUK2xKOaKSaKXIr2hHSV2H"
    L"Jn1VNLWgsbsHg9fFtZFX6p0EFq/Zgm//7F54BUfoqAuBQjVigGLMHijqutiMysMEZjnSw1EWAoUlJenycDSKUFGXR8DT1isg"
    L"LKtFXXOnFVcZv6qKhLAwCuX93B1jBigKEhtUjEJp6RtQ4+gQlZDLybP4zl334O21G9Emn6myrUddIQZ5u4ZGUdfWicZOOSbG"
    L"YtoES5kolcq6RqRlZWuNWmbL7ty5HefPe4rqCIKX1yWd+2NN9NuqUHnllZfwOBPWfnkf7r3vbk1cm/Xqy2P+/v7/bPuZHc3R"
    L"/udba3fHy1yuomegH21dnapIjCohUCxVYjSKqIHhYZ2ZvGnbdmzYugNnLnghIT3fmsiXVKi5JCHxhQiKzUdaURM6h2+itn1A"
    L"wFKvwcuR69fU6ArQzYlOy8Ejz7yMF+a8rtXq5bC6OAQKbfS6uD1q3BeYyL66P4yRqFnBVvtRHgKFEBq+Iupj1HJ7qGgIntCY"
    L"FMxa+DYSUrP1uYSIBQgLKGY7HSrvvW2Zxk8EKMxfobX2ClB6+nUIOiWvAM/Omof7n3gGCdn56BWVxHlBdIf4XdQ0tiBNHlNY"
    L"KTDv7FZV09I7pFCp5lpD9c3ILy7TyZwsacDFx855ntLZyhxe5izl/fv36nEumcpYyqOP/VLtgft/IVC5F28vWVxv+6kdzdH+"
    L"Z5v03T8srixHe283hkSRcBYwA6+c8m62pl27dg1XpJPkSQdgtbRV6zfj9EVfTVKLSy+wAq9JBRp8jc+pQUZZM1ILazUxjRPv"
    L"WroHNYmMtWMnBVCMlwxcuaWqZNdBN3zvZz/HkVNn7wjG0sZEkdAMUKhQuOVQMN0aQoGAsR/lIVD0fgEK3Z0BUQZUMnQvgqOS"
    L"8MSLs+EbFKEukwEFFQxBwnT9mQFi3TbHCBHCREd6BCIECVWKCc7S7dl31APf+MFPcejYabQKtBhToTrh0HKjAKS5sxPl9Y1o"
    L"kttN3QOauMdtQ2cf6lq7Ud3UIUqlSVdFZDYtVyxct2o5Lpw/Az8fX61gd/y4h7g+TtiwYZ0GaOnuPPKrBzUTllDhkPG6tatj"
    L"bD+5ozna/1wrrqmuaOvpUddgeHzMSlK7xX8Jkpu4epUaQpSKuCgcHi4rr8b2HU5YsXoDPC8GIZ5Zn7ZZttHi8oSn5CKFM4BL"
    L"61BY14aazn7NxWAswbgsdFMICrofnIsjfRyVcqVevHwVTl/yVphQpVBhGIXC9HfGTdTlka3mp4g64esYhVJc04iCyjoFFTNe"
    L"qXI4T4dp+EzHZwyF70cAUhFxNrAZHiYguM/5QASGcYMy84p0GQ7us0I/t3SJOkVhGGVCmFjbQYURVQqBQndm1wEXvLthk+al"
    L"MLuWkKlp69DM2uaebrR2d6tSae7uRU1rF7rktfn5h+Xvau0bRkV9K+pbOlDT0IyS0nJRJd4KlG1bN8LHy5sTNuHp6anzsriy"
    L"4cqVy/HSy8/hyacexcO/vB/33/dz2T6oUNm/d4+L7Wd3NEf73bfiioqgpu4ua3Lc+PhU2jxholC5xVtWY55JeUUN9h86gg2b"
    L"d+CidwgS0koQk1yK6CSr1ADLK6aX1ChEskXCV7S1o2N4FIOiGOimEAaqOkRJcEtoMPhKdUGocEu1wvsIEhMjYVYsjfcTHNxS"
    L"WXCu0ICAgsqDz88tq0ZQdIK6DozPMKO2f3RUQDlpze8Rd4cKJauwGq8tekeBQoDQCBRChFAx6ft1ze0IjYpDTmGpgoT3WcqE"
    L"1frH74if2AOFSoVQ4dpANMZTukbGUNncikYBR3Mv1w3qBUHe1CFwkX2qFeautMhnaekfRXFdi6oUKha6P/ViTfL8mpoaBAX6"
    L"Y83qlVoykrVUCBWWkuTSqZs3b8SKlUvxwovP4InHH8GTT/xKZxXff989ePWVlxDg4+MY9XG0333rHen9t9LqavSOjmvy15Wb"
    L"1jwQujn81zKrXZXOXCe+PCf1LV+zCSfPeiM5sxSxSaWIT6vU0RwWGUrNr9RU8xa54maXl6NloA/DN68rJIzLou4MYyfyZpay"
    L"4MiLlchGwAhr1I0hSPhYBYoQyIqpULVYIzuECCcCqgoREPA9eEUPjUsWuGVqYJYuD6EyKs8nUIbE7SJQCiuasGTlBgSERk8B"
    L"hYAwIz18PYIjNikNUfHJukiYUSyMtRAovJ9AMS6PBRfL7MFiMm3NCBBjKwy+MrOW6oRAUXdHrK6jS10h1l1Jyi1UtaUTDtu7"
    L"dJGy+uYWtLa3gRMBOQuctWdZZY1AYTyFtU5cXQ/prGJrEqAFlV898kuFyi8feoDxlLb/bOFxR3O039jqGxtG2OF4hSdQCBOi"
    L"hEWi7YEinEF7Rw9Oe17Ssownzvloslp8eokok1JxdyoQm1auc3MyxNVp7pGr9fCIXJV70XdlVOS7uE3yOpzVawVZrYxXhYO8"
    L"No33c2vFQ64pWAgSPl5Hd2Sf4CBIDFAUMmJ8rgZm5XF0zsobWgVshTrXhs9ROMmWQBkYtXJQCsob8fbqTQiJjJ8CCkFCo7vD"
    L"LVcfTEjN1PgF349AMbEVGsFiDxRLpXB9Ze4PTbk+BiC1rR3qAhEyXB6VqyTqGsydokq4LyqFQKG6qm5tV5jwb2ns6kdJTb2O"
    L"/nBReFayb2wVV7K2XtSJL5YtW459+/bZSkmew4kTxzQpbv36tXh9wRxVKKx9YoyTAg/s23fWdho4mqP991tdXV0+K3ixQ9Id"
    L"MDC5HYAVnSLKgqM74+NXdNnSVWu34ODR05r1SvcmOqUYiVnVCInNQ0puDYprO1FS26aBVyoRjhGN3bwmWwHDDcuoOAgS5o0Y"
    L"mLBcAGEzJCAjHLglBAgDwo5QYUmEcYHKsDyR2bHj8kKEBI/RXeNj6BJxEiDjFHQvOMJigMm/kUHZwTH5e4WXRZXNWLp2G4LC"
    L"Y6cgQIiY4WIqFQMG3uZxAxGzpVrp6ZfbYl29okwEpLdjKQMClH4FDaHD4etWgQgXcm9s61R1QohoUFZAwn0qlAYBCreWC0QV"
    L"M4jy+ia53afuIlVOXXMbmkSxMKZSUVmNU6fOYOnSpVo60t/fV+fwHOO6P7t3YtXKZZg3dxZeeP5pPP7Yw1oDhVCZ9erLCAsK"
    L"cuSnONp/v3V2dv6ctUXHx8XVkasu1QBhQrfGNMKEUOGITopcpVl53mn/YYQnZCpMQhPztfQA65dklzQrTCoautHWO64uBd0Z"
    L"qgoCgq6UcanozhAiRk2MC8AIHhpBwmMDVxgTsWYxU3loDgoVCvcFJLKrLhNv0+XRURzZp9tEoDBWw1Ee4/IYoAyJ68NENn4+"
    L"zp15d/0OBEfETQGC0DBujRnt0cxaOU64mMfwOG/zeQPDomjEJekWV0ttyvWxYiq8TVVBuPA276MqYRCWEDHKhCBhFTgChkDh"
    L"lsdqRalwGF0nLMrnNwu/M6bCdZu5eFtpabnWoKX7Q3Vy6RJr056Ei8tBbN+2CUvfWYxXX3kBr7z8vKoTQuWpJx/HiuXLOhsb"
    L"Gz9qOy0czdH+a628vLx/aGgIk5OiTKRzEyZMMdE0E5pWlZYOfW1ST1bnvQcFKE4ICI9HQlaJgKRAq82HJRcjNrMU3RMQid6H"
    L"rOIadA1e0epscv5PdXy9TcUiRsAQIlQl4/Jm47fEJbl2BZO2fUKF9xMmvKpflY+ibosAgYrkKl+XAV0ByxXp7IQKQUVosJYJ"
    L"A7EEChUKyxHwuYSKUWIEChVKSXUrVmzchfCYJIWEURwmJ8UYFYpxhXjbbKla+Jz+IYGLTZ0QLEzFN0AhTKhIeJvHuU+VQvXC"
    L"YwQG3aEmgQqhQWsRVUL1Ud/aqlbV0KDPqWpqQXp+EYpq6lDb1o2y+haFSkNjK1oFOunp6br8BhPcPD3PiGo5oXDZv88Ja9es"
    L"wBuL5mPRwnkalCVUmEHLIkvu7u7nbKeFoznaf77V1tbu7O3v0/VvuE4LgUJ+XJdOyHbz+g3cvEEdcRMtTc26ONia9VtwwTcU"
    L"CRnFOh/HKspcqmUZM8vqtcYIA7FMyOLVf1yeTreGMKGiEBGhUNEhYHF7tEKaujmWO0SgcH/0hlU1jQlv/AR8PIFA9UEg8TWo"
    L"CFqZzi6dl3pK5+dcsblI8jfwSq4gkccz2KswssVQCBYqEA5Vl9W2Y93WvQiNSlCgMIhKQFCRGGgQUlQh1nwdK9GNZtwjE1NR"
    L"pSCdnM/XUgYCE+PuGMVCUNB4m/dRpRAsGktR90fAwn15DBUNYdLSJe4PA7YCmIqGJuSVVSKvokpHferaezTpTRec58iRAIdV"
    L"9piH4uS0C5cvX9RZx4ddD2DXzq1Yv24V5sx+RYHCcgYssvTcc89p8fGMjIxv2U4PR3O03751AX9RVl6pHZBV5xmXoCChESC4"
    L"Jd1YVALHWriOcFhEONZs3Aq3UxcEJNkCkGJEcKZwchFSsquRmluB0jq5QorUZ3aqzq2RXm5iI1QXcljNDAPTuK/Hmdhmi62M"
    L"Mw1fjK6RlhvgaxFA8njj9hAyxRXV2LHHWjWwSzou0+lZv5buDEdzCDKdqyMwMDDi6zHHxoKLvJa8f2lNGzbt3K9lKAkIEyfh"
    L"86iM+FosB6mxj+5uBQWPERY8Rquqb8Kx0+ewcuMWZOUXq5tkYi5GpdBMPIUKhXV1jVF5mNEdY8bVoftT12aN/jAtnwFdo2Iq"
    L"Gpo1rtLQ2qmjT/UCFq4ZzfWjGZTlZEIP98Pwunwex48dEZWyGzt3bdXZx5xE+Oprs/DCSy/imeee1ZUNBEL5tlPE0Rztt281"
    L"9Q2lXFuYMLEf1dG5OoyZKFBEYsiWBXqc9u3Hlj0HEBSfjvjscmu9G1EnGowVdVJRI358R5/69lQEDBoamHCI2Ezum8kMMKYA"
    L"YgPKlPE+8ZlUXciWt4k6zp3xCQzRDF3OH6ppaNTMXhEWmrx2exTIei5BMgWUq1dVwRAoZbWt2LzrgNZqMe4OgUBoaMzlimzH"
    L"xhQqOrlQbrMym3FfUrNysWn7Lrz57koc87ygHZ0FmiyVYgGF0CFM7IHSbmfGBWIMZTpMjFGFECh8fbo4BAm3VEj8PgjDitpG"
    L"nfPT3t2DrJxszZhdt3Y1PM+eVHM7fAB7BSpUL1wIn4viv/zabDwnUHn22acVMgEBAXfbThNHc7Tf3AYmJj7LkoxcK4dXdZ6M"
    L"BApzTqYUihy9cf0KmpsbdZ1eqhOv4CjEZZcgLClPl/iMTC0WP74DdS0DqG3p0xKJOqIjLzIdKLpvAwchYlSKgQUVgz1Q7NWK"
    L"ud+AxDyH+9wWlVeLuxKLhpZWUSis8A5VKaxyTzfLynG5DRKzpUKhiimva8O2PS6ITcqYcmesZDZrZIiP5+gRk+IIFBr3+Zr8"
    L"XJwRfNHHHwWimEyyHYeE+TqECl0XQsQeKHrbzgxkrBGd98KEt4060TwUgQuziTniw9wW5rQ0dfbrfZwPxByV2vo6JCTFY+vW"
    L"zdi8ZT3OnzuNE8e5cNg+dYVYu3bRm29g9vx5ePHVVxQor776MtauXVNmO1UczdF+cyurqq5s6+pWoLB4NGFCozpRrGhGrHTq"
    L"8VFdOmP9hi04cvysuDc5iMkoQVRGKeJyKkWhlKF16Cqae0Z1vRtOtDMQIVjsgaIqhbCxwWQmoBhFcgdMBDC8j52XHZzP4W0q"
    L"BwMVGl+Pfw8LMpugK4eUuYwGH8f3mQ4Uxj0IFK7fs3Ofm04OpKIwQOF7ahCX8BFVMl2p8D4qD/NZCBK6eoyBMHmNcDJxF+Mi"
    L"GbdHbRpQZlIo9sbXpSKhES40AoW3U3MLNDOY6fqEDqHCNY+4Fg8ru9H1OXrkMM6eOaHJbkzLZwV8LsExf9FCXTBs1qxXdIYy"
    L"5//I7/592+niaI72/q23d/xTVCdmNT+iw6ybY+br3LghLpBYbW0tuGTpjt37EBSZqJmvBAoXIg9LK0ZqcR36xCviEhacwKZx"
    L"kymICDDo5ogxQEqgTAeJgYkxA5T3mO2xjMMYY9zl9vOt+7kqISvtU50wcCuHMCZvaF6fYJgJKDUNndh70ANJ6bkKk+lA4eMN"
    L"TIxa4T5BQlgQKnx/qhIzZ4c1aBncJaAIEaoPE5SdiqnYQcXe5TFQme76mHIGBIlJjlN4CFDyy6uQX1GLquYONd7PYC+hkl9Y"
    L"gO07t+kMZDPiw8pvrJjPBcO4DAeVyrx5c/D8889i7tzZdIkybaeMozna+7fKquq8ngErgEnXgCBhfRDZtWBy08qM7e/vh59f"
    L"gEjlnTpPh6v4MeckOD4b0ZllSC6pQ3ZlE1pHrqJFOtCYPGtUOjBzRwgRQsXs2wOFMNHOLVdzYwoTwsHm8kx3ffg8E8zlY6lU"
    L"aOa5BigMMBMoHMXRoWkhD4FiVA0hwtedDpS6pm6tLJeSma/xiF8HFKNUaHxN3m+gwgCwFrMWoFGhMLDLoWcChPAxIz1TYLGD"
    L"CuMo9kCxB8tttdKtrg7dGioT4wLxOEd66rsGUN3GVP12hQpdIgaLWzu7tFwkCzMRJGfOnIK7+xFdDoXLdXD1waXvLlNlQoVC"
    L"Y+mDyMjQn9tOG0dztPe23t7ev2I2JYeIaYQIzVSsv3nrqmU3ryM/vxB7nA9g38GjiE7KQYK4ORwmTsipQnJhDfJq2lHbM6y1"
    L"X4fkySNUA7KdDhRuCRRu2bGnA8XARI/bIGJAojC5acVADIwIFvMaCiHZstPq7avXdEIjE98IFWbS0uVhkJa3DagMWEwMpb65"
    L"B4fdzyItu1AhQKDwNacDhZ/LTJzkbd7HER8ChftUJwoSAQuDsoQTVQrVx3SgqNmgYq9SjBEuNM1FsUGFqoMwoXHf3FawtHWj"
    L"orUbVe29WgSbVeF0cmGPuERNTSgqK9VF2QgQd3d3XaydCXBcFXLVqlW6tCzn+xAkrPbGrZPTTl/bqeNojvbe1tDQdKRTTs6B"
    L"wWEFCGcMTyWziVnRiOvo7u6Ej68/tmx3wmXfME2vj0kr0sW2YjLKMHfpejwzdzFCkjPQJ52bCWQs38i6JAYmJijLWArT4Hnc"
    L"QIEgsDce0/sMQMQ4jMytxlKmwCHAkX1uCSM+xwCFbhCVFgtkm1UC5SEalJ0a9ZkBKHyNhpZeLVeZkWsN9xIEfE2jQAxQFCIC"
    L"FAMV3sf3pxEYjKHo8hvyXgSLicfwfsLh/YBClaJKRcwEZw1MaEatEBzMTyFIuG+MCoVFmDpGr6K6axDngyLw+CtzsWrzDp0L"
    L"pEHa1lZExcXr0qX79+/HyZMntbrb3r0sxrRRgcJlNuj26HDyqy9j8eI3GEv5ou30cTRHu7OxKA+Hia9MXtMENkKE6oSNFdhu"
    L"3qC7cx0VFRXi6uzAYY/TiErMVleHS4FSnWSWNmLtroP4+o/vwQ/ufwgbnPeisL5e1QifbbZWMpmoBYGKLsIl72M6nz1I7M3A"
    L"xIBkymz3EyQ04/7QuG9cHgKFi1rR3TEuj5nnY5kolokrCiuFgsCD+Syl4rrtdzmOuMQMVTVMm2dAl6/RL2Dgc6h+uL3T+HqW"
    L"0QWjq0OYcDha3R8BE6FiilbTDEyMy2NvZvTHXqnQjOvDiYR0eQgUBmjp7nDLiY+sqB+bVYA3127Bt+++H1/74V3YsNNZZyvX"
    L"NrXqpEMWF3c/cVJHd7jiI2vQuri4aEYtCzVxnWNCZM6c1zRAS8Xi5LT7lO30cTRHu93qW5rebG7vsDqddHDj5higcOKfqpOe"
    L"dpy/cAmbt+3WjNjEzBIdHjZW1TakC1R5hcfilYVv4Ks//CHue+opHD13TpfdZDq9FTexUuvNaI+6PTaQzAQT2vsBZfpzjHKg"
    L"cZ9uC49zXaDJa+LO8bE0G1DoAtH4txMERqkQSM0dfRojWr/ZCS5uJ5BbUKpp83SVCAoFkzyPZSN/E1AIKCoUU6KAMCFUzEjQ"
    L"dKhQoRiVQrMfUp4JKAQJYycEC5UJh4oJ7XxxY3e5HMFPf/k4vvjdH+GVN97BhcBQVLd06m/COUCECofVQyOjsG3Hdl0Xma4P"
    L"gcIhZM5GXrVqBd56a7EGZWfPnqUxlTVrVt0qLi7+C9tp5GiOZrWS8rKOgZHRO4Ay1birN2+iqIjlHHfD5ehJhMVnIDG7XEES"
    L"m12BtKIGFNa1Y1Au61QiLAx00ssLjzz/PL75k59g7pK3ERITrwqFxlEfUyxJa5zYQYFmVAZNb88AFPtRHj6GEGGgMb+kHAWl"
    L"FdpR6J7o80V5sdwC5/VYJq9lA4oFGcZXqGDkPjHpi0jLLMCmbXuxZcd+rNu4A5e8A9De1a8KhSUOjLJhwPc3AYXuDRVEdnEZ"
    L"cuTzsbKblShnJbeZmrNTikWgQvtNYDHxFAKFEDFuDt2Zw6fO4ImXXsE3fvQzPPTsyzhwwlOXPGWlNy6QVtXCZLgOnZXMVIGy"
    L"qmrNK6LrQ6DQ7Tl4YB+2bN5IeODdd5dq/ISuD6GycOECLirmWKbU0W63kZGRv6tratSRHXYMwoQuj2madiK3B/sHEBYWJkBx"
    L"gk9ApLVcaEYZItNLEJ9bhbyqNpHQw1q9nTN4CQqCI1+k9H53D3z/rl/g5Xmva70OqhTGUczwMV2emYByx+3fABS6N1QkXFYi"
    L"t6hUgcL4ABUK75sJKJOiUggTyw0SiNy4qaNbBiipGfkKlD37j2LLdmdcuOyn84OoUJjPQliYfB1uDVwss+43QKFCIVAyCoqR"
    L"WVgyVSrSBGQNUIz1cWsDC4FitvYp+vZQYfzEFGViAJh1Xn759LP42YO/xBbnfUgtKNFSkd38HAKR8sZmdXl0hIjuUYeomu4e"
    L"REZHYd2G9RqUVai4HtLVCY1K4eqDhApdH0Jlx45tdbZTydEc7Q/+oLKu1rOjz6oTyys0WaIm/9BuSmdjCkqddNTDh4/Aaa8r"
    L"IuIzkZRToUlsTLPnyE5xQ6ecrFYRI4KEaoGw4D7LAyRl5SKA68zIcXvXhzEUmr3SmMmmw2Q6UAgTxjx4xdcgqM3tES5YsRWb"
    L"y2OAQphwNrLsqjHfhiDhlmpFDqGptQdnz/vBWYBy+OgpAUyu1jThxMOr8p3Q+PoECL872m2w3AkUnaksn42dnsakNro8/Kx0"
    L"e/i577BpUDFAoUqZMUgr+6pSuCj76LjONj52/iKiU9O1lCRjKdyWNTSpeiF0+BvwvVgykoWYON2iqLgUh1wOY+vWrRpHOXLY"
    L"DfucrUr5jKXQ7aEyoUohVLgkR3R09L/bTidH+7C3wvLSq30jQwoUBiTlHFO7Ydwe2VyVzpGUkIxtW3fhxOmLSEjn8hdlmsSW"
    L"VFCL8lbWQb2us1s5OsLgKjsxOxtVCKHCWAm3losjHU0gwK0ZPv51QNHjdiC5w2yPIVA4msPHcku4cJ9Kh52WrhyDsldFgtCE"
    L"GWoGDFQZhAk/s7p+8jz5E5BfVI1TZ701KNvW2acxFAKFSobuklEnfI5xmyyocFTJgopRKCYoqya3TQzF3mYCir1SsQeKPVRY"
    L"YIlAYSU3ljlgvRcWuWbxKCoRHudQMYHD2AlnWLeKW8pykZylzBga3R7OSA4MCsGmTZu0uttRtyNwPeSiKfrr1q3BsmXvaECW"
    L"QKExluLmdviY7XRytA9zGxoa+tvqxnoMTYibMmHN26G7oyadTJvsd4lPfvqkJ/Y6H0JIRCKSs8t0qVC6O+kVTegcv4WS+lYN"
    L"ALIT83XYmdiZObphQEKjQjFBWcZQGD/RoWMbGN7PVI3Y2XSgECB8PzNaRDNBWbo8HAa/xtot8hhjHBY3RnUih9QICVNOoaN7"
    L"BEGhcaiua9VRHtY1MaM8msIv6kQfb1Mot5WKGT2yclZY7oAg4fdBY+KcfT7LbwKKPUxmAgoDslQ+hAkVCMFBRUKYcLlTU/mN"
    L"QVjWTOGi7VlFJUhKz7JUTmeXAqVDnptfUKQuz7Zt23BM3NXDLq5aKpJAYaV8E5w1UJHjw/K1/R/baeVoH9ZW19S0krU0mDdB"
    L"haId6pZVetq0G3LVLSkpw85de3Hs5EVd8Y/xk/DkfCTmV2kSW+fYTXQMX9F1ZtiR2bkZm9DYhexTjVCpmFEdjj7QeJ/JlCUU"
    L"CITpZoBhRl+mA8X+ceyc3PJ9qTp4nCqCaoKxIc5HIjz0ts24TyMUCEKChVsek6eivWsQXr4hKK8S8ErH5+gQXSQCk4+jqiFU"
    L"jKtjzIwe0fidGIVCiFDFESimDIKZuWyAYuIoDM4qVMQUJtPUiT1QqFAYR6EqIUwIFbMqoRUroZsjkCFsRKFwW9PCSYTN+nzm"
    L"tLCyvlEpZzzPYv369Thx3AOuLgd1ZjJdHgZnly59W4FCY14K3Z6IiAhHrZQPe8srKW/lyUqYUK4zyZ5zdpgNy8AJSxVcv34V"
    L"/gFBWLVxJ3xDExCVkoeYtAKtyJacW46atgE0947pOsP0yU2QVTu8vJqpuqb5JtKxqBpo3DcuznR48Ph0Mwln7PDGjJvBfXuV"
    L"oPOPFCoCEfks3CdMGEPhX6bg0PcTcBh48DnyWYlTbjkzma/Njm7VLhlF/4SViEbX0D65jsbXJOgINeN60dXh36PPkc9AYHB0"
    L"iDCicZ+P42sbZdI9PHqHESoEAcsPUIHws3BLeBuzv819woXDx/ZmlIruM9lN1Apfl8qFw84MzhIyHHFqbG9HUkqyLrOxd/cO"
    L"uB911RUHucTpmnVrNRWfICFQFswXqCyYAw+PI4dsp5WjfVhbeU2DXh2Z5WlDiP5rzdmxul5nZzvcPI5h0+5DCIrL0hIFsVnF"
    L"SMwuRVZJLeo7BtEzek0rsJmRG6oOU1B65OZVDF+Xjm4HB2MzweT9gGLUyXSgmH0CgVv+BbRxcaVMkh6BYv1lVhFrulvs4Oav"
    L"NAAiSDjxkRDlPgHLx+lnkPfWavw2gNgrJYLFpN8blUQz+/z7qDRMAFiLYNvAwhEeKh/+DjSWp7S3npExNU4qNBMMp8OEADFG"
    L"YDBAa28KkU5xhbh0qdzmcwgUnaXc14+esXEN5NJFIlSoVJjoSIhs37wBx48exkEXcYF27dQRIM5CZhxF3Z65szB/3mviDu/q"
    L"tJ1WjvZhbC0tPXdxlTk9mcXlkfNcOx27nb1Cyc/PlxNpD1xPXNBKbPE5pYjJLEJqfoXWh2V5Ai7rSaDQpZkauRGIECqEyZAo"
    L"g+mAoJmOZ28zPY722wCF+R8czeHfYYa/uU/j4u16XIwAoIqwkGkpFN6n6kSAcu3apMZbWO6Aj1P4iCIhiPhYo2jsPwuP8Ttk"
    L"bIX7fF3GT3ibx3mbr0PI8O+h8uGWao0gITSoSKgaGN+gW8KgqUlcY4zEpNYb03wT23wd7pvb9maOs1oeJwnyNQgegoOv3Sff"
    L"mS5EPy4uK90leV+WYGCQllXdWBLS1WU/Dh9xxc49TjoDmdm0jKMwIDt3zqsKlNWr3kViYuInbKeXo33YWnl1XTiXyyRQ6PKY"
    L"zqfdisknYhMTY/SNFSgXAyJ1ZCc6o1AVSn5lk1zZbqK2rVdO1lGISLEpFELFUijq6tAEKpT9xqYgMcOx97OZgGLMuDsGilQm"
    L"Bih90kEqqip1hjRhwftNhzfuDp9HmBigqEoRNUKoWNmwnPNjGYO7hIm5zX17uFiBWmvSIeM4DKZm5xeB8Q8CiUAhOPi9ExQ6"
    L"wiIdmyUImGTGoCljGzQGTxnjYJ1YlibgDGFjvG2Mt5njYm+mNoopY0B1wuxYwofH+dp0d7h4W2N3D9ILi3WNH34WxmWYkxIf"
    L"H4stm9drAWs3USm79zprnRQChXEUdXsEKHR5WNj60nnP+bbTy9E+bK2kovoq/WWe2Lz62gOFRpXS09Ola+Hu3nsAQdEpWrk+"
    L"LqMIKXkVqGzqVmXCAkSsajYmPdRSKFaQ1RSYNjYTPH7bY2rGvbADCc2+Y/P2mFxp+XfQOHvay9sX5y9eQF9fn7oyPE6YECLc"
    L"8nncasCWKkb+bgKVjyNgCBwavx8N7BIoHIKWg/J2U98bAcLXpBEkhBAVSlpWLt5ZsRpPvvgynN2OoLS2HhVNrPXaqFtCJb+8"
    L"EmV1DVPG+wgSe7MHCM0eFgYe5hihQbNXKiyuRIVCpaNKRh5r1knOLS2X3zVLlz9VN0nUD3NSiosL4bxnp66NTKDsP3hAZyQT"
    L"KBw+Zj6KAQpjKUfcXEJsp5ejfdiaAEUDfpTbvFKzU9AMUNipqqqq9CRyPuiKmNQ8VSjxmcVIK2BF9W4MTgKtPSPoHBizuTos"
    L"QC0gsSkUKpNhcXe4vQMO04yujj1IZgKKiV3MBBQDBe388t5yCK1ytff28YOvXwBYvf+6KBiqLk501ECtPJ5mnqMTIMUVmgKK"
    L"3ObrGPhY343VzHfF+wkWgoMAYXCVpQbYIbXKPDtyQzOCI6Lx5Muv4Cvf+S4uBASifXAIVS2tyC2n2yhKRPbvhAcVR+uU8bY9"
    L"NGgGGtPhQVgYl4jujTGu0shCV4yfaAxF7reGm0W5yHtwUXYGbRmjoXJhXZyGpkZ4eBzF6jXLNQ3f5bCrFl5atWa1puEza5YV"
    L"8unyLHx9LvY47RiwnV6O9mFqIv//iUDRUQcBCqW46SQmfnJdFEFKSgq2bd+J42fOIzlH1El2iQIls5jVv3rQNy7wkH6qUl46"
    L"FPNKhq+KIpEOSrBwn+vnGKDMBAsTO7G/z95+W6BwqNZ0fAKDsZTBoREtx0DXRe9QUFpqjCCxfzwb77Ogc0PhQtBQeRjoqELh"
    L"bXk5ZtsysMq1dug6sqq8uh4NLdqpmVpfXF1nlVsUV4LLWlwMCkGubIuoUggKOV4hsCgVRUJ3w94MOOi+GEVib/ZKhPsmVmJA"
    L"QiVib4QJ3R4+Ru/nVgBkRnuY7MbHcJiZkOnq60d3bw+CgwOxbv0qzUtxc3PDnj17sG7dOs1H0dnHc1/F3HmzMG/+a1i/YTWi"
    L"oqI+bjvNHO3D0lrau14sq6q14if099nJpGtZ3cpSJ1wtMCg4FFu27oR/SASSBChR6QU6ulNQ1YzGLmt0h2vXMGGLCsUeKDR7"
    L"oJjhYnszwCBQph//zwKFcOBtdnwCg+qBW/5NHLHhjrWOkHWMbgmN+zQCRGdVy9/OxmNG9XDL12bQl0lmXNqikR3a1tkJkhJx"
    L"VZj7wW1htQCXMRAxujVUIIQH59DoPBqBCIFS2tCkiWh8HN0NY7w9HRj2xmP2AJkOEvsRHmOEBVUK9xmU5egS4zoECm93Do3r"
    L"Yusc7eGoENXLwMgw0jJSbWv4OClQWCOF+SmsQ6s1UgQkBMqrs17E0mVLBEDB37CdZo72YWkFJWVxzXJ1IlBMhqzpWASKxk/6"
    L"enHW87xWZuMSEomiThJyyxQspfUdqk7M6A5Bwroj9lAgTGg64kO1okldFljsgfGbzABnOlBM7MTe5aEZmNgbYcn/bzCGYlMo"
    L"xAbNPI/Q4d/P+9Vkn4FeU06AuRmsbkaI0C1hx6aSKKlvRk17t0CiTa2othFlolZKRHUQImXyWMKjRMBC422agQqtUgAyXaHQ"
    L"6IbYQ8S4NPb2fopkurX1j2ihJcLCAINzgOj+sKQC84g4jN4/cV1fk39z//AQSstLsHe/MzZv3qxze6hQuG9mHxMmtNlzxPVZ"
    L"MBve3t5P204zR/uwtKy8/An60Dp0Ka7N9BgKr9TMlnT3OIn9h44gPiVLQcIh4/TCKvG1r8gV7YqOGjCNXJPZbACYDhQDFXPc"
    L"HhbmGEFjf9zepgNlOkim2xRE5DNNmfxlDLhaewyg3pyCiX0wlfdNTF5Bt1y1OfuWgUuqD7oYhEh5Q7107A5REZYiUZdFOnpe"
    L"bYPAohXFYuXS+Wml8hhafm2d3G6buk2QTIcJjarEHiQKEx35mTk+YszAxIBiumluirgzCpR+JrJZ+Som05YxEzM9oqCyTo0T"
    L"CTkS1TUwgIaWZpw4dVxUygZVKKzoxpR8KhQGZqlQjEqh+3Pq1KmVttPM0T4sLTUzS08mBhEJE8ZA2NXYoXh1ZucrLinTtYrd"
    L"j59BYkYBkkShJOWUIbe8AYPyhLb+MT357gDKNVEgV23V4wUSNMsNug2Pmey3AcoVgRxtOkw0vmFnBiLyv5oBinXbAgqfR5jQ"
    L"TWNdV75H7+CQpp1zZT0uCkYlQpBUNLbp+sB1ApiKJsY6pPO3tSG3ugalcn95WzeSSipwyi8YwamZqJROT7Dk19ejtLUVRfJa"
    L"ZeLyGIBMgUTMuEWEiNlOgaRDICIuFI1ZrvbwsDe6MAYcxkzglabJb4Ni4s60DY7qbULEKC/OgOZvyLITsWnZyCmt0pyUriEB"
    L"DVdEFJXCItaMm3CSoM7r2bFzCijzX5+DeaJMaK++9hL2H9xXajvNHO3D0jJz86aK+hAm7FxTHVA67VUBQop0ji07duPC5QCk"
    L"ZBcjMcsa3SmqaUXvxA10i69D/9/ETe6AyaQVm6GZeTvmNs2AZCaATLf3UyjTgWJcFwXIDGYyX81jGffJEmhyYXEGPlkTRF0a"
    L"AYpRJpw9zWVAWEKRQKlubUFRHRWHqBHGTeR5xa2dyKiqw77T53D4ojdy6ppQIlAobBDXRsBDVUIXx16J2INkRlVig4mCpLtX"
    L"v+f3i49Mh4kBin02LRPWGCPpHJkAF4e3KsZZaf26L0qV9zPOQrBwhjIncXKqAVdB5FKzBMr+vfu0nIHTrt1TLs/ri+ZhLlWK"
    L"AOWVWS9iy9ZNk7bTzNE+DK2pt/cfi8rK9erEuSPMNjVyn0Z1woBsRGQsNm/fhcCQaAVKQmaRuDvVcsXuUnXC2InWhLXNmJ0C"
    L"ic0YM7HiJrz/TqD8Z6AyUwzl16kUOfxel4fH+beJcbmLfrkCswDRycu+orwKtEPTleHwLMGi+wqADiQXlOG0bzAScnKks7eh"
    L"oKZGYNJouTUCHgKlrKMHIak52H7kGAITUvU4XaFSAQ7dG3uQTE88MyB5jyIRkEzBRMxejdjbTIrEqBCapuvTfRkWmLBSm6bz"
    L"29dfGbduj1/DwBW5UIxNamCWF4G+8TF09PUhJSMdWzdvgbOAxM31sAKFM49XrHgXi95cgNkc6RGozJr9MlatWQnbqeZoH4ZW"
    L"Vdd0T6X49QQKYyj2w60m1sCsUl+/IGzb6YyI2GQkZxWpQskoqkFtW79cxSZ03V/OFuZJyU6vELFVf1ezAcVaw+a9Ls5vCxYD"
    L"FPvEtpmAYraEB/8WYwoTAczYpFWVXqfvS8eOTsuCn0CzSoDQ0N2vUClvbNKkMi1C1NWHBlEn7pf88c2f3gOPCxekg3fpqE1x"
    L"3e2YSbGomAp5bH5DOzy8gxCakomyZgsmDM4a92Y6SEyglSChGYDcARGm3tvMHiAzKREz14dG1WFMa9gKIFSdGKCIMWWAvz9n"
    L"PFOhtMnfyrKQA1dvon9SzgFRKZxLxO+ssLQEzk57sGvbdnV5CBSO/HDo+I3FrytQaHPmzcLbS9+Sbx2OUgYfllZRU7O+oqZW"
    L"XR6eUPYKRWMNt26ho6MD585fxk6n/YhJTFegMIaSU9agc3c4wjMobg/dBo4UUZ0QIjz5jE1XKITDfwcoM7k89sqEZu+6MWeE"
    L"Kfgj0pmYj6KujLgqVCFUAVQfZdKx66WTcqTGDOnS5SAwCIPKth4cOHUBn/7qN7HvqDta+noFPOLGCHDo8hSpye2Wbl2iIqe6"
    L"SWFS2zUgj2mxFtQSmJTVNSpE6prl/eR9GwRi9kFWujMGIvYg0ZwQMW5ndGXEjAohOIwREPZGgFB59Ixf1fgIb/O311E+OcYY"
    L"Cn9PXiBoQ/JFssoeizDxOy+vrlKQ7NiyVbeECyu4EShvLlmoIKE6oUp5fdEChIf7/IPtdHO0D3orr6o+zyUTmIPASYHsmPZA"
    L"YWuUTnf8tCd27T2I2MQsAUqJJrYVVbeou8MM2a5B68Tl5DkuDm4PkzuBchse0+3XgcSYPVBMUNYeJiZ2Yn+MjxmVTsPkrBbp"
    L"vAyycthXM0LpyjAgyriFwIMKwmxLGhp0SzeFyoWLYx04dQ6f/dq34HrytKiHHlEvjZbqkNehCiltEtfHZnw8QcLhYwZyucAW"
    L"81QIDoLEwKSxzYKJfVxkJphwdMaYgYlxZexdGhMHea9ZvxEBwgpufYyN2JSHqg8bUPgYjvJw6Lixq1/T9AkUjs4xNtYsrp6H"
    L"AHXHls0WUJydsWnTBixf+S4Wv/WmBmYZkCVQOAPZx8fnO7bTzdE+6C0zN6+yur4BPUOiUMbGtPMZoBApzCqtE9l/5NhJHDpy"
    L"DNFJWapOckvqUFBer7GTjv5x5BRX6pVtXHow4aAzZuUE50nKfXZ+U7iJ6sU+vmLMBHE1oDtV3NkqVmRmEnNrjLffo0zkUxMo"
    L"nMnLLa+8HBJvEneiUaxJOgezVc3EO5POXtMmbkd7h26ZeEblkV9Zofu0YnELCY7EnDwc9TyPqNR01HV1obSxEYW1tagRFUcw"
    L"qTsjr0s3iVmv04OtHFrn1nJxBCoCL1N7xJpFTKiwrADBYhlnGTP9nXNtOPOYZg8PS4kQIgTGuMKBw7zc8vagQIPqon2gX7c8"
    L"xrgRn8MAOu83UOFxbhkP42snZubofCM+jhcGJj129vbg9KkTApSNuqg6iy1xTs/ylSuw5O23RKHMxiwBCvNQOKfHw8N1lu10"
    L"c7QPesvIyR2rqqvXHAMqCxYSsoAiZ5QYixCVV9fA9egxuHicRlxqLlJyyzQw29gxgCHRxLRmkfWcwzM0TpXBYkJWer1RJgSW"
    L"cYPeDyg0AxQDD6t8oql6ZisuLUaQmMcYCNJYa4RKh+9J1cWlMwiS5o4eTYenKqASYOCT0LCCo7Y5MoSHQICxEyqXEvlemGOS"
    L"V1VlUyntCpX63j51hwpr6lSZlDc3KzwIo6KqGtjPvzEzhQkRHbURlUJTt0YAYiBiDw9j0yFigYTxEQsm0+MjhIgxgoHlNLlP"
    L"gHC/d2xUwTBwxVKMVHmMefGxhApVCavIDcv3yiH0AfnOGafh+zLTmXErXhx6Bwdw/txp7Ni6AW6HXeC8bw+2bNuKZQKUxW8t"
    L"EWUyB6+99qoC5fW5r8H9yKFlttPN0T7oTYACBmUZQ5kOFNq4nHwFJaXYd+gwjpw8h6TMQqTlVyA9rwx1rb1o65WTVM5ECyw3"
    L"MSBA4RCjqZdKlcIOztcmKHRZToHKTDChTakUG0TsjUCxVyYEidmnq0VXiIpEq4zZFAnVCEdqGKPQuIUcozpg5yYg6MrolmAQ"
    L"CFBZMCbCbUF1LbLLqjTgWsj1fWobdC3gjvFraBu+ghqBggZjGbwVt4aQ4YgMl6S4Q5XQRJkQJHWdvRrgtY+R2JtxcTT5jBAR"
    L"eJgyjjrcK4DgUG/viACCNibqQj4Pje4LXRN702FhAkX2h+S747ZfVB/dGkJCS0zId6eBWd43IY9R6Ais5TwYl/v4nGF5LFMK"
    L"dErF+AR8vC9i+5b1Wg6SmbOs3mYPFF38ywYUt0P7D9pON0f7oLesvHzQ5TGjPOyc9kAZHR9DTkEhnPYdxHHPyxqQJVDyy+q0"
    L"TEHPsABA/IwB0cg84a7ICUgzJx87OWMf6voITAgVHfWxs5mAwmVQOVeGIDEuDoEyHSomRqIqSOCli1wJNLjAFye80XibIyoE"
    L"CZUBOziDsBXNVByMlzAl3kqbL2beiGxzK+o0DlLcwOHgbhQ1irroG0VoWh52uZ+Bu1cgCupb0DQ4po8rrGsRxdIgUOlSGDEA"
    L"a0BizACFo0jM8VCI2LacV6OxEqbDM07SZyWgab6IQMRsu0cE0qOT1rCvmqgQPSaujIGLqBHmjwzSTRHScl0kAsKCiXy/Ij65"
    L"tjTvJyg4ksPg6yh/O/6GYmaBNi5sb47xsXweVU1IcKAChQt/7T+4T2cdL12xfAooXFKDGbML5szCob1OnrbTzdE+6C23sAi1"
    L"coVlZ9SZxtJJmfBlDxRCZ5fzfpy+6KtASc0rR0Flk/jkAgQ58YYYRxmUK9zETT2hBydvql+vMRU5IWmmAhmlNSX2+wFlymww"
    L"MRAxZtQKXRvCivEanuDM9CU8VI2IEiFEuE+QUJUwGMp9jqKwc7OIdsvguJi15ZKpHBamNfWPihIZQ033EFrkj6sRFdY0fB0B"
    L"STn47gNP4a/++Wv4+Be+gcUbdqCqW5TD+A20SMemaqGC6RV3r1PUA4dluTZRj6g2GhMA+6ji5Puy9q/rbQ7L8tig0JEjKlyA"
    L"y2zZyY2NSadmNTUqB9PJuW9s+jE+1rwG3RneR8UxIq89Jr+JeR++Lu/jAupd8jlpffJ5huQx3Ofi9vaw4QUiKjIc2zavw4H9"
    L"e+8ACmMo04Gyb8+uS7bTzdE+6C2vqFiBYq9QeNWX/wUn4ksLULLzC7Bzzz54egVo2j1jKHnlDVqywOW4J1Zs3IF1252xestu"
    L"2Tqpbdq1B9sEQud9AzSAyEAfRwno/xMoJpbyfkAhNBiQJVi4JUyoSmhUJMa9YTUxAw9uzb4Bi8n3oNHtoUpJyS3Cae9AeFz0"
    L"x9HzvmpHzvnA9cxlHD7rpfuegVFwO+cLl3N+OOEXgTPB8fjF06/hT/7hS2p/8PHP4l++/VNsPnQcl6OS4BeTCp/oJAQnZMA3"
    L"KgGhCWkIS0xHRHImIlOybltyDqJSchGfWYT4LBb3LkZSbjGS5TtNzitBisA6Jb8UaQWVSCssn9pyzlR6UQUyiqrVWCEvr6JR"
    L"pz7QeLuwWlQSraZJlFaLKK1WVV6VLV3yW3WhtK5VR+QIG0KFQCEgCL11O/fi+flvYsXWPdi09zC27D+i2017XbB6hzOOy8WE"
    L"wCOkeOGJjYlSoOzf54wDh/brGsgEylvvvK1BWQKF83no8jjv2u5lO90c7YPe6M4wKMsrPK/0HI25DZRbUy4PgXJOOmFCer4o"
    L"lFLUtg+gvLELi1dtwM8feRKPPT8Lj7/wGp57bb7a87MX4JlX52Lj7j0ar2CwzygV+u7WEKWVQWs/dEwzx8xjuD+V0CZb3maq"
    L"OGFCcHCeDeHBeh6mdCIViZlER4jwtqoTcUOikjPw2Iuv4f6nXsI9v3oWv3jkGfz84adx10NP4qe/fAp3y7Ef3vcE7nniZXzn"
    L"3ifwk0dewN1Pvoa/+ddv4i//+Rv4m8/9h9of/v2/4vPf+wV++PAz+MmvnsMPH3gcj740T17jSfzs4adw1yNP62ub179bXtsc"
    L"/8n9j6n97EF57INPyHvLfbL96QO0x+U5z+rjeZyf6+5fWs/nbT7mLnlNflY+n8f4+HsffU7N/E33P/4CHnjiRX3+j+97FI8+"
    L"P1tBRqBQsTB2QqAwx+YnD/wK//zV/8ATL8/BKwvfwnNzFuLZ2a/jhXlv6LGNu/aq+uTz+PvExUZPKRQChTEUAuXtpe8oUBhD"
    L"MUDZ4wDKh6fl5ObrKA5jD3R7GPln/EMuXjrfhUDhQk8EygVvf8Sn5SG9oFyvipxlXFDdiIziSuSW1aplFVfIVq6gFbXILCrX"
    L"YVT63QwCasIUA4fi478fUHibxkAub3NrYMJ9Qs+AROuv2sVJeIxDwTSFh4CGgVizrzOFBTb8bFml1UjMKVK1QBURnpiFkLh0"
    L"y3Q/AyEJOaI80hGUlI+AhFys3H0EHxF18uef+aqqlC/96EFsOHAc+8/44qR/JC6FJeJyWDx8IpPgHZGg5hueAB855hUSi8vB"
    L"MbgYEoNLobG4EBCFi4HRuBQkt23GY+fldTx9w/U+7p/1CVPjMdoZ71CcvBSEM37hOOVj7Z+4GKhm7Qfg2Hk/nPYKhrunjxpv"
    L"c3vAwxNcb5ouKlPp+TsT8oRKXnmN/l5F1Q26Lahs0GOcHFhS24Sy2uYpt4kxrLjYyDuAYkZ53lm2FLMFIgYoCwUue3Zu87ad"
    L"bo72QW/JKWm3GJRl5+TwIjsxczkIFMZShkdHUFhUAucDLjhzwUuHiwkUyurGnqEpH50+Of1zxlT0pBNTH17UCIOEBIq19KYt"
    L"hmIDCbf2KoTQoKSmcd+oEt7m6I3mlMhnZd4G8zeMIjGmIzgCEYKDZkZZOIpjqqFp0aOqeoVfQmYBolMyRbVkISY1Ry2a5S3T"
    L"C7QaHWvmstRlQl4lvKMz8NZGZ3zrF0/gx796CbOWbsLOI+dw3CcS3pEpiM4sQVpxDTJK63RJEVq27OeU1SuA6aLkVFpWWN2K"
    L"opo2teLadvlMHWql9Z1qFU09apXNvfLZ++Rv6Je/Z0CU4aBafScX7xoWQIygsZvrFI+gqWcUXBOJ1trHAklX0NYvbqGtvERL"
    L"7wjqO/q1UhvBzFiIeD5q/M0JmJmMv+fUY+Q3ZipBREggtm5aqy4PR3no8hAodHnmLZirCW3MRSFQdm7Z6Gs73Rztg94Sk1Im"
    L"y2pq9OrOoCk7N08aAoVDyCNjoygpLcfeg6446XkRadmFyMjnlawJ1W096n/rXA8ON165oSMQNJ2hKsbUbg5bMkjL4WSqE8ZT"
    L"jDoxQDFQoXGfMKHxthkKpoqiGvlNQFGo2GBi5ZlY2bAcziVQimvr9SqcmJWPiMQ0BMfGIyg6AcExiboNjIpHQGQCQuNS5Viq"
    L"qoyA2DSEJmXDOzwZRy8EY8O+Y1i/10O3e9zP4ZRvBAJF3UQIjMKSszVWYuIlBFRMWr5alLiMNO7HCrRiuS50Bq3IiqsIxBhX"
    L"4eJp1mPuvJ8LqvF+8zjeZrEr1qfROIwYg+Y0gl+H+GWbXVQlAK1CbkkNSkRVcl0gs06RwkK2BAb3ze9PmPM2a8TQ+FgaZ5+H"
    L"BwfoKA8Vihk2th/lMUBZJPtO27ect51ujvZBbwKUpqKKCo1BMGmK6mHqhBJpy5T1yqoa7Hdxg/vJswqUrMJyrQvCtGwOVXKo"
    L"k0OiHNYkRJjTcHsOye3sTcLE3uUxQKH6IDS45W1VLsyFkMcxiMuELwMQGpPBpsOEc25oM6kSgoSp76V11rAwh3e57nJmsXTC"
    L"7DzEp2eKZanFpWUiJiUdsSlZSMzI0yB0bFquFuUmHAKiUnDWPwL7jl/EzsOnsfvIWZwQ9yI4IVM7uFayk+8oJbfEsvxStdQC"
    L"6eg06eA0+6Arg60MutJ11KCrbKlqcsrFjWTQtaJO1U1eZb1u86vk83NbUa9buieFVY0KeVpxVZNmMRfJ44uohuT5JbJfLM+r"
    L"qm8VldeL4ZExBQOb/NxTYFFgyD/c8hzgfTSW0mTj3K6rkxMI8vPWxDaXQwemEtveWf4u3lyyWGMoBMprs19WhXLAeddO2+nm"
    L"aB/0lpqeEZdXUqJAYVYmgUKJy5NJh2nldq24CAcPH9Vs2dSsArnKiV8tVzmChC4OJ9exg3K+CXMbqEgYL7Hsdko3AWGM8DDK"
    L"hGZ/m1AhhAg4ZpEacBiY2B97P6BQkRAmmqhGN6e2SY0lGQkUU+s1r6JSl43IK6tQKyitQH4Jb/N4hU4pKJCOzY7KDsyRmAhR"
    L"KheCY+AZEKkxE4JGR16kw3IkhTkoFY00UUZNnTrKoiMtrd1TVtPeq6Uiua3t6BHr021dZ7/mqtgf520eZ0JcfdeAblnDl0Bv"
    L"6h6y1YcdRqu4oMZaurn28ZAou1FrK+5OW3c/OnsGdaXCyclJrehvmgEHjTChcX9ILhI3LJbY2k2Mj43A5+I5Bcph10PYs9cJ"
    L"m7ZsxtvvLsMbi9/UGApHeQgUW2Lb27bTzdE+6C0rJ/dgVkGBpqgz3Zsdn4E6e6A0yBXezeOEZssmpedqByusrBHl0K+BOmbF"
    L"MmGMEKCyoOtEiKgyEV+dxunxOkV+bEzNQOQOVWJTK3wcP4u9EjEwoVGt2APE3ox7Y2BiktUIEmMKFLo+AhUWSGKKfWl9gzy+"
    L"URfRYskCM8/HFJ0ub2i1gFQtzxG4UD1kClhpVAgER127fDYBAIPAnIowlajWJ519QDr0oFUlTSulDY7dLnQk35cmqYlq4FaB"
    L"bEtQU1dSXBO6k/aJapxDpcuUyJYJhEwu5NrM9sai21flhyQ2CAd1a+QG96k0dGda0/vECBSqkoKCIrTL9zzV5PjoyBA8Tx5T"
    L"l4ep97v37MLGzZsUKIvefGMqKEugzJ/9Kk4ccX3Gdro52ge95RcVLUnLykYF08qloxoo0J+my0OgNEsnPXHmnGbLxiZloFgk"
    L"tBmW5UlN4UwIMc+E6sOoEg4Rc+SIQ7xTUBkdtdLvBSh0c2gGKIQJH885OASIMX4uAxPeNorEHiTGxTGxEqNM7GFiv8/0ek2x"
    L"bxKANDfL8ziRz5rTQ+OcnBLO1dGJhFaWKxPiqD5odJ+oQGpFeRAgVAWc/ctZwATtHbOBBRjMZtViRWKmbABNoWGLNyk4xEwG"
    L"q8lMZWYrh3hpDHSryTGzGL0ujUp4yD5NwWFMAMLSDbKrdofa4AFuBC6GL8bMcypFefYKGNm4lAoPjgwPwsPNRUd5jri5Yufu"
    L"Hbok6VvLlmLhG4sUKJzLo4WqBSieJ09+33a6OdoHvZWVlX0/KS0dpVW12lGNuuCsUuakMLGMBZrPX/bRoePwmCQUydW5oqFZ"
    L"Ox7dEsZFaHyeqfxGFdI7PKyTArUWqYCEpupkwoqXsJgPjVPmue3mVVxkOSfyEVZ31AcR4+2ZQDIdJuriGAViBxOqFaNYCkSV"
    L"sL4r5+Fo9XkBi8JFbrNoUn1nl7w+X7tNt6bwEafyEyxUIiw1wDV4OPLEz8ygMWFooGgm83UOj0zNqbGyZycVJlSDxhXk98eZ"
    L"wIQ5TVdbFCM47OFBd5Sm8LBtDTyoRGjGXVHjPv+xNft9Nut+Me6L8bl8Db4eVeqkyBweZ7tByMiNnq5uuB7YOwWUHbu2Y+36"
    L"dVj8zttYsPB1vDZH1IkAhcPGzJS9fPnyp2ynm6N90FtDQ8MnE1NTUFxZqTKfQKFK0eJFcvYQKCzW7O0foEAJjYhFEXMTaupV"
    L"MbBDqBIR475RI4QIYWIPFOP+WMqEro21pfWIG8DOqbVBWuniMAh7u6o79wkZVQt2ULEHypSbIyCxYCLQmAYTYwYmrPGqNU9E"
    L"pRilQjPr55iyBsbF4pArYxdUIdZn61b3jKNQChHOzhXj98DvUr+bsXGrkBG/n4lraqpIpoGECs+asHcbJHcARbq2KhL5Xdjp"
    L"2eGNsdPPaPLYOyBiaCPN/rA+VkxdHTG+JpdYNcfMY1ljuK6mFgf3OmHbpvU4esQFO3Zsw5p1q7Hk7cWY//oCgYnl8iyYP1ft"
    L"2LFjf2I73Rztw9AiYqKRU1SEhrY2tPX3aTFiLhk6fuumKgoWJoqOicPmzVvh5xeAktJKUTR1U4V+OJpDZcIhXT6eLk0/4SGd"
    L"ih2Lxo51O1BrlRk0+5x6zwpkqkbaOSzMZLUeHY2gcX8KNGJTUBHjDF8tIcAcE4EJQcKyAtwas8BSr/vq5oiK4eNpWhzJZgSS"
    L"2SekCCwDElPj9Y5C0bbZwVr8SCBi3Jz3lBUQQOtwOl0dG1B6ba4OXRzGRFgyYET8kVHpvTSFh81UkYhxaXe6otyyf9vb76K9"
    L"32saV8da9Owm8nKzBSYbcfCAMw4d3ItdO7di5apleHPx67q+8bw5c8XmY/asOVj2zgpHTdkPW4uMjdEJgFUNDWjq7pKTfwSD"
    L"k1d0hT/CgUtKJKUk6/or5z3PobCwGBVy5WdcgZ2HCWuc3VtUXqVqhFB5L1Bsw8hj0qnEmJPCY+yAWqFM3AUChfCYbgSMlkm0"
    L"VTkzQLGHCSFx282x4DGTTQFFjAAxELGHCdWOPVBmhAmDxgYmnB1MuNK9EXeHEOHfRlOYMJ5kg0nP5HU1lgpgmQALJhZI7OMk"
    L"9jCxgHLTBhQzefN/pik3rN2ppkCR9ydQbkyOIz05ARvWr9WZxgTKzh1bsHrVu3jzjQWqSGaLuzNv7mzMnT0Ha1evcQDlw9Zi"
    L"ExMauDYPE9zqO9rRNTykKoXGeMfAyChy8/N02cmjbkeQl1cgAGnWWAI7EGU4FQrT3+nOmECsvd3OS7lthAkhYh8voRl3h0aY"
    L"GHViDxR2dtYxMcrEwOTXGR/Dx6o6mQYVAxTCxLhShMl0kJh6Jc0CkhaBCGuW0Ey9EgOTqb+Tbo66OraSi9NAYh90VZDIlgCh"
    L"W6MxEunINELE3qZ3+t9V4+vSrl27JheOQvT29t5ekpV2w8qS3bh2DdwOHcSBffuxeeMWrFy+Cm8sXIS5HC6e9SLmzaM9L+7Q"
    L"mk7baeZoH5aWmZN1OTYxCaXV1ahpbVGgcMmE7pFha0RmbBwVVZVwd3fHPue9yMzMRll1PRo6rRXqONLDaxhTuQkPxhPsYcK4"
    L"ibli61VbbtM9YFCTMKGpu2MLvBIYBijTYULTyX6MmQgAjJtj79ZMh4gxe3fHHiT2QDHKxB4o9jChIlGjKrEDiSl+ZGAy5d4R"
    L"JNNgMigwIUjeAxOx24rE2J0wYYj0fwombAYobARKT0+P7ZYFletXxnDmhIe6PO6HXbF3j7OqkHeXLsPC1+cLUF4VewVz574g"
    L"9hwOHtxxzHaaOdqHpZWVlT0WHB6hgdmy+jqNo7BcIIFC94UKpb6xAefPn8fO7TsQGxuP4ooazbXgcCnhwJEaBlmpVDg8TIjc"
    L"CRTGE1i2UDqedELj4kyBw7ZvAq/2++ri2FLqNa2+pV0BYNwcgoRxk+lAsQeJMVUnNnhMN+PqECSECl0dqhN7ZTIdKAYkmkui"
    L"wWkLKLowlpgZFjbDwfYuDm36CI4BCQFtmalM8z8LEtMMUOyDuapQCBSxjpZGDcgybsKA7B6n7Vi16m0sW7YQCxcKTAQk8+fN"
    L"ssoXzNWA7A9sp5mjfVhaS0vLpwICg8G6J5qGL24PYUKoMCZCa2lrRWBwkMZRvLx8NChb09yJ9r4RNY7QcNiX+SX2MDFGmBAk"
    L"zMblqIi9e2PAwa2By0wwIUh01rB0fsKBsCA4fh1QDEgInyl1YgcQe5spbjKTOtGYCV0dxkxsIGGshGbUCeNKtJlgQmVi4iX2"
    L"MJkJKP9bIDGN70Wjy2OaWQeaVlKQq8Wp9zrvguvhA9i5cyNWrXlLgbJg4SsCkhd1TeM5c+bgjUVvseL9n9lOs99Ji/DZdneo"
    L"19o9x/Y8m7Fzxd1V25ffVei646kNASeW31+Z7fl3toc52v/rFhwShsTUNBSUlaG6sRHt4jtTnZgh3+7eHsQnJmD37t3wOHYC"
    L"xWXVqKxrQUcv805E2g8zE5YKReDTaZVCsDcDE6bNc6jVjN5MB4qBykyqhDBRZSKAMNAgROxtJqBMh4k9UAgRAxKjSmaKm9jD"
    L"xMRMplQJg65iVm6JpUyYPazrFAlMZhoSNiCZSkaT7kozIGGnpk01c+A9d/xum/3Lc67P7SFn+dDi8kSHh2Dd2pU4dOgAWLZg"
    L"8+aNunzGW2+x2v1czJ49G6/Nma37K1atbLWdXv/l5nfm6d2uO/69023nl7Fn7SdxaNNncGjDP8F9w+dxbP0XcWbrV3Bux1dx"
    L"fMvn4bHlX3Bi2xfhLvuuG76EI1u+eeXoth95xlxe+N2OvGOfsL2ko/1vtIjI6PHo+ARVKDra09FhDf8KTBiY5SLZWTnZOHDg"
    L"AA65HEZmTiFqGtvRpnNGRsAq90xKKyqv1oAtIUL3x9htZUKAdE4NDxMe9jZdkdgbs3MVJgIOo0imQ4RmjhEo9jCxB8r0mMn0"
    L"0ZyZ3Bx7mNDVMcrEBF7p3hhlYtZwfj+YTCkS2Tcw+U8B5Y47f3fNvDTdHEuZ2PavUzfdhOep49iwfjWOHDmss4yZ0LZ02XIs"
    L"XLQEs16bh9dmz8fsOVzfeA42bdlwznZ6/adafNDaFQe3/nBy58p/wpFtn8Fp50/h1K6PI8P3+4jz/HdEHf8yYo99A/En/gNR"
    L"R7+KiCOfQ8zxz8vxf0HI0U8jzOOfcGr7X+K80ydxbNP/xemtAphVn5Pt9+C+9js4uvZuHFr/WK6ny5LdRfHun7a9raP9LltS"
    L"Supl38Ag5JeWaiyFC4UzKc1kt/YM9KOiphpnz3liy9btiEtKRXVdM9q7+tHRPYC+wTHIBXlKeTBRjWqFAVomejEJjpm4XK2P"
    L"o0FmSNgeJkahGMWicLHFM6gmCAFTz4TQMFAhOGgGMNNdHQMTe5tJobxfENYMC9vDhGYPExpViomZzKRMaKyFq2ny0j1pM2W5"
    L"0qYa99/P/hvtzviI1bjPo1zYrby8XI/xccblaW1pwgHn3dixfTNcDh/CdqcdWG3LkJ07b8GUMmHqPfNRnJ13zrWdXr+xAT5/"
    L"dOrQA/F71n8Obju+gEuu30TM+Z8h0/tBJJ39GWLdv4/EYz9Eksf3EXP43xHm+m8Icf08gl3+BcGun0Xw4U8jyO0f1QIP/wOC"
    L"3T4j9lkEuv2rHPuSHPsyfA98GZf2fBFnd/wrPDZ/SpTMp3Bi67/g5PYv4fi2r+LI5n/HsW3f7z/jdJ9f8PHZL1SlHXZk+P5X"
    L"W25u4f1+QcFgGj5VSmV9PTr7+zV93sorGUJ9c5PGUbZu24GL3n4oq6zThbM4g3V4zFoTmZ1FV58bYlasFZwlVAgUZuIyec4q"
    L"kvRehWLUCdXI1L4dTKayYG3gmAkoVCYGJr8OKDPBxD4Ia4BiQDLdzTGujhnFMQqFMNG5ODMoE9p0mBiQGHsPK8yB6fY7bIyX"
    L"GDXCuT5c2K1R3F42e5enID8XWzeyStseuLq5YPOOLXh39XIsXroE81+fh1mzGTt5DfPmzcHrC+bh7LFjn7edXu/bsiOcVrlu"
    L"+fnEwc3itjh9AREXvovIc19H4sXvIu3SD5F67idIPvFDJLoLTI78AAmu30Hk/q8g1EVgIiAJcvknBB76NAJdBCQun0TAoU/A"
    L"/9DH4XdAbN/fqgUc+L8IPPgPCNj/j/Db+0l47/mYPO6vxf5Snv9xAdMn4bf/H8R1+gRObfoEjm/4FDy3fwXHN30NZ7b/CGd2"
    L"/AIe6+/GiU0PdAW4zTpdGLn7e7aP72i/rl309kFkbBxYCZ9DyHR7CBVNVhsZRltXJ1IzM7DDaQ/2H3BFaVkV2gQK3T0DqlS6"
    L"WM5AXJ+OXmvqPLcmRd3UfqU64ZwXAxSjVOwDseriSCdnZ59SJgIH487Yuzv2QDEujz1MZnR3ZnB1DEyoTgiS6UAxozlTiWq2"
    L"mImByG2YWOvfTHdxTF7JHWpEyGA/gjMjJ8wd0+2/2ahG7NWJaXxpzjJmeQP7RpfHz9cb61evsEZ39jph/ZYNWLb8bSx6c4FW"
    L"uOfcHSa0zXl1FtauWNVtO63e04Atf3jZ9fHIM7u+D/eNX4HXgR8gxesRJF34BZIv/hiJ57+H1PPfRdzJryHK/UuIOSp25KuI"
    L"dPkSwvZ/ASH7/hVhh0SZHPqMBQoBRtCBv0Pg/r9B4L6/RMDeP9NtkPNfIMDpz8S4/SsEOsn9uwUku+S+PX8Of6eP6P2B+/4a"
    L"QQIef2eB0J6/g6/zJ+G/959k+8/y+l/FJacv4eSmz+LImk+JfQYnt34Jp3Z8FRf2fVfUzbcmRdX0XnJ9JDH07Gu7ilN3OUa1"
    L"TLtwyaszNDJKoJE1FUtp7uxE9+CgKhQCpayqEm4e7tiwcTNS07KmgNLW2aeuD+flcMSHKoVujwGKqhGBCuuucMv8Epq9UjFA"
    L"mUmZ2MPk18VPCBYDEnug2MNkJqC8X+xkeszEgMTAxICEZqXRizKRy/z0eIlRJQQJtYA9G+ztPW2mB9H+m+22K2M1AxjCxLy8"
    L"fQylv68He52dsHPrJrgfdcW+fc5YL2rlrbcWqyKZ9erLmDvrVbw5Zy6WzJ0Dp41rE2yn1VSL9Fu28rDTA8Meu3+AU9u/gEj3"
    L"byPP75fI9npQlMgvkCaW6vkzJJwSmBz/BqI9/k3si4g5Jnb0CwKRzyL04D8jwvVfEHTok6JM/g4BBz8O/wN/gwCBgv++vxAF"
    L"8ucChT9F0P6/sOAiwPDf/VH47fpTy3Z+RM1/15/AZ/sf4vK2P4DXtj/C5R1/DO9dct8eeZ6A6cK2j8B378cQ4vYpeO/7hNjf"
    L"I9zj82rBbv+MsCP/gkiPL8hjPoOzWz8Jz52fw+ntXxbAfBMX9t+NcwcexIk9D+LU/mfGfU69kZoSvmdRa3XohyteExufuPmS"
    L"jy+Y5FZYWibwqEZdS4uqlO5B6WSd7er2+AUFYsOmjfDx8VN53C2dsJu5KEzkEjenXQBT39aDulZxY8QIC1UhtiQ1AxNzmzY9"
    L"EGvUCYFg3JmZYGKvTLhvr0wMTOyBwlR9mgGKPUxmip0YmNgPDytMRgUiLH95B0ysOTl35JhI9zTKhPaeGMn/D5r9EDE/GksX"
    L"sBmVQvjk5+Vour3bof046uqCXTt2YsWKVXhr8VIsXfwu3pj7Jua+OAsLXnwR78x5Acf3LV/Ecyon0fkfzrs8Uuh56C547P46"
    L"Lh39LrJDH0NuwENIP383Mi/eK3Yf0jzvR4bnA0g4/lONl0S5/4eA5BuIOf41Ac+XEOoqakFcm4CDn1K3xs/1z+HtKhBw/Si8"
    L"Xf4MPgf/XOwv4bVfbO9f3bY9clvUiJfzR8T+WI79kZr37j8UgMjznf5E7fKuP8L5nf9H7A9wwen/wHv/R3F5rwDG5a/FPqbm"
    L"I+7RZYGXv8snEH1C4OL2r4g88m+IPf5NJJ2heyZQPP8A4k/fi0TPXyLE/ee47PxjnN35Q3GdfoyLTvfB/+Dj8HF9CkGesxFw"
    L"4fWOmKCVPsXZLvMHOmI/q53wg9TEb/6ox8lTCImI1JwUQoWxlNbubnTIFYpAaWxtQVpWpkheZxw+fFjcngp0SUfsECXSMyBu"
    L"Ube4SAIVVjpjCcKqRoGQwEUzXJvaFB4mcEujOiFY7OMntP+qq2MPEXszMLGWH72tTt4PJvbKhDCxHyImUBQmNqDchglzTG7P"
    L"y2H2q1Em/38Dyu3YyG0lQrDws/Eee8hwLs+F857YvGkD3A8fwkH57TeuXYeVy97FW4uW4PXZszH/pRew5LXnsHLRs1jzxkO4"
    L"eGQ+Lrk9hwsu9+HCoR8hyfsRZAX9Cmn+9yPV625xae5CxoWfK1CSz9yF8MPfRozHD6SDflf2v4nIo7SvIcztiwKSz4gK+UcB"
    L"yT/A78AnFBS+AhFfl4/C59BH4XvoL8T+SoDy1/ARdeElauWiKA3aZWeBibhAXvv+VEzAITChXdzxf+C1+4/hs0egsVdew1nu"
    L"F+h47/1T+Oz7KAIP/7UCRZ+79y8UVN7y2n6HPo7IE59VheLl9EkEu3xOYPJdgcp3EHX026Kw7ka29wNIvyB/1/lfIP3yg8jx"
    L"FYD6PYV8/+fEXkKaz3MIPPUIgi48hdCLLyHw4izCBSG+K5Ecc+BWbua5A7Yu+fvfzl26POzl54/45JSpWApVSktXB9p7RXG0"
    L"SKcVt+f02VPYtm0LEhMT0S5X+a6+QVUpHNnhUKhWLBNjKcKmTi570S1AEUXQzCFjLhd6W6UYoNhDhTDhXJv3UyfvB5SZYEKz"
    L"BwonFdoHYWfKN5lJmdjHTbiGs5oNJjMVQmIq/fThYBMv+W0bHzuT/S6afQyFgKkQN7dDAHtHESZp9XJR2b1rB/Y47cLhg/vg"
    L"tGMrtmxYj1XL3sLi+S9h/qyHsGTePdi69mEcO/gMLno8hctH5Ap96mHkhLyC3OCXkOv/vHS0Z1Dg+7xARMAibk6SACVcOmL0"
    L"6e8h8uS3EHvm29JZv47IY19BhMeX1L0IdRP3xvVT0nH/QYOnwa5WvCRo798j0FncHjHuB+37uLgqf4sAgQ3dHp89FiQIC2O8"
    L"7S2A8HW2YiwBAgp/AY7fnj9T83fmsb+C/96/ge8ebj+GUIFY8AHap9TVCj70T2L/jNDDX0DMyW8gyfN7iD3xXYS5fwtxZ36C"
    L"lEv3IN7zLqT5yt/ndR8SBS4pvo8gNeBJ5Ea8jNKEBShLWIzm3G1oyHZCU95+9FSewlhLEIaaI9BYEoiC9EvISLiMtoac52zd"
    L"8ve3RcXHLz1z/oKqlMzcPBSWl6tKaWxvRddAH6ob61HTUI+4hFisW7cGFy5cQFVNtaiTQVUoDMYy/ZwLp3cOjGlZRF3Xt0U6"
    L"r4DEpOuzQBOroTG3xN7NYfKaujoCB6vimlWO4P2AYhQM7f3UCW0moPy6IeJfBxN1c2ylGU11NcKEELGf4EcjTHj9t4fJbShM"
    L"67nv0wxA7J9rGZ8/7TXsH2DX7A9Pu0vAYm1ZnW1QQGqa1p0V0ISHh2L9ulVwcdmDw4d2Y9e2Vdi0egHemvsrrHzzYXjsn41L"
    L"J+bhgvuTCDz3NNLDX0VxwlwURL6ErMCnke0rncn3GSSffhjpZx+Xq/jD0ul+jvgLP0OoxzcRI1f4iJNfR/jJL4ub8AUFSdBh"
    L"6biHP40Qt89I5/0UAg/+vY7Y+EuHD9z7twhx/kcEO31GtvIY538QoHxCjn/MAorAwdfpIwoQf+ePwn+fDSCiMhhrYYwkSCx4"
    L"718jyPlv9Hmh+z+OEIJKwBTg/HGEiiIK2v+PCD/0WTnO2I24N4e/KLe/KCrqK4g6/u+IOP51hHl8HVEnvoNE+VsSxYVLOHc3"
    L"ki+L63bpPsRfegBJfo8iPeQ5ZEW+hsLERajMfBdVGevQnH8I7cUn0V99WWAShhs9acBIsZwwDfKDdMqP1Ius5EDkpYdW2brm"
    L"72erra3961NnTuOitxfSs7PE9clDcXkZapoa1AiWqrpaFBYX4eDBg9ixayfyCvJFnQxMuTxajoBFlTtYaLlPCy5XibWPTOo6"
    L"wqwKv2zDFsxd8jbSCoq0M2teiHR2qgYCwECCZgBiDxZzn1EpBi5mBjFfj1saAWViMtzyPTRQ3CWA6xaYiPEzz1TTxExoNCn1"
    L"mglri5uYdHp7N0ezX6Uz0owqmd6Jb4/t3J6pM9XMg+2NzWylcfeGPIer6vA1LKjYzDyHN23P4aZd/sYVazdg90EXXShN3S6b"
    L"3RTq3bJ90GuTsiPbWypTxIUbHsTBA3uwdetKuB3ZDuf972Ln9lexf+vj8D76CkLOzkXAiecRef4lFMe9hZq0d1AY8xryQp8T"
    L"dfIUMv0fQ6bvw8jwegipF+9Hiuc9SDz9M0Sf/L52xChRJlEnxb059jWBx+dEhfyrqpJQN6qAz4gyEXVyQNQIgSGdnh3eUiNi"
    L"ckzVipocP/AxhQWN8PA7YEGEWx7T4O3Bj1mPE4jQzGsRSEH7/y9CDv4jwlz+SSDyGQQdFJgd/GeEHPocwly/oCCJdv8aYgQk"
    L"/MxRp8RNExjGef4Qied+iiQBSsqFe5B66SFk+T+F7MAXkBs+W76PxShKeBelKetQnbMTjcVuaK2+hL7WKFztzwRGS+XEqZOT"
    L"pU2++F753gfkqxew3xpAaV4QSgtC99m65+9n8/P3r73gdVlUSrgOExMopdWVqG9tRm1zI2obRQ1UlCMiIgJr1q1FcGgImtra"
    L"dTSIQ8LsnKy7aoDCBci5EHdWZR027XfDvU89j+/9/D4sWb0WaYXFqhjqRC1QQRi3ZTpMpps9TPgcGhWKgYkBCreqfGxAMYFY"
    L"e6Dw89JY5Gk6ULQEgR1MNBNWlImqE1sQ1gBF80zkim5iJlZXn+7iWB2VELGA8D5AsR52+7bdi3CXqodQmXqu/eNsRjho2Ua5"
    L"2Sp/76pNW/HYCy/hhbnz4RMYiqHRSX0d1p0lP27YoKKmRZWuITHaH1vXLYTH4VVwd3kLnqeWwu/8EoSefQXJvq+JCnkDZfFv"
    L"oSrxLVTEv4GiiNdUlWT6P4F0AUnq5fuQfPEXSBb3hp0u9vQPBSbfRcyJbyGaIzlivNpHun9ZACJAcfkswg5bMAlx/Ufp4OLS"
    L"SEdXoLDTG5iIe0NA+B9479Zv/1+Lfcxu+zHb/eISHRQFIsbXpdGlsfZF8Rz6jMKEAeBQARvhxhhO+JEvIeLoVxR6hEnMiW8K"
    L"RL6P2PM/QsL5H4sJTOTvS/O6H9l+jyE36FkUCkiKIheiNP5dVKZuQm32HjQUHUZ79Rn0NvthqDcZEyOFuHWlRr7rZrEu+b4J"
    L"k37ZDsp2VH6QHnQ3JSI76czvd02ZlLS0RR4nT8Db3w+xiQnIEQVSWF6q7k5FXY2O9LCcQUFBAXbvccIup906ItTR16dZsVxH"
    L"h6qDrkTH8DjKxJ05cs4Lj740G9+775d45KXX4HbmPPJEYrOuCIHC6msEAgHB7UwQoRnQGKAYkBh3R5WOgMNAxR4oNOPqTIcJ"
    L"1QmBQpjYF0kyZQgMTHSOjioTsRvWiI7GTKRD2gOFffM3AcUyq/9ONXNgutm1qUP299P40tOPSTOfhRM0fcIiMHvxO7j7sWex"
    L"bMtuBCcm2oa1b+rCbteYecfXEaDcHGyDp+tKuG59FjGX30HoqTmIvjAPSd4L0JCxAVXJyxUmjAmURC/QeElW4LPICX4Gqd4P"
    L"q/RnR2OHS/D8CeLO/ABxJ78nnfI7ApL/QNQxudK7EyZf1U7LDkyghEinpjLh0LCqEwXInWblnfyt7baAggls+wQeTGYjROge"
    L"ESJyPy2Ajz8oUBLT1xV4MB7C9wp1FZdGLERAFnb4XwQinxeAfBERHl9W2EWfsCBCVRJ78j8Qc/o7UxBhzIQgSfN5BJkBT6Eg"
    L"7FWUxryOisRlApJ1ApLdaC4+go6asxho9cdEXzRujGXI91sh33Oj/EYtYuLi3OyRLa0Xt252yw8gULnagusjuYgLOfD7X6Tq"
    L"4GFXePn5IiwqEgkpycgvLUZBWYkqlbqmRlTX1qCoqAh+Af5YvXYNAkPDtPasJq0x0ClXRMYnLsiV8Pm5r+PL3/sJvvuLB7Fx"
    L"nwuSCorR0j+sRYeoTAgTE+OgERb27s10mBAkvy1QjJtjn3PCQKxxdQgSAxOqE8KE6mS6m2NgYs3PsVTJiMBjKgAr29/G3bF6"
    L"6+3jM99vzEKSvRkI6T933mWZ3ilwmJzQ+AdvcuEuLufKzzUhj2nuG4HrWW/c8/RLePzVV7Fp7zYUVubIY+WqaLsy4koL4n2c"
    L"Ee+1HsleS0VlzEdH1ja0JK9Da/oG1KWsRGXSUhTHLlJZnx38MjICnkGa7+NIEveGMEmUzsbYQvy5HylMYjkaclLcBFEnhAmv"
    L"+AYmVCfMfGUHD3H9tMZMbieszQQTaz9g799ZMJEtFYz//k/YMmMFRHYWdOgf1dSFUpjYXBl5XwKEFn7kCzaQfEkg8jUBiCio"
    L"U99E9BlRUwIR494QJonnRZFc/qUqMSqS3NCXURA1D6UJ76AiZS3qssW1KXRBW+VZ9DX5YaQnFpPDqfJjFMpvVCXGWEmrfNft"
    L"8jOL3SBUOsXVbMeNa3L8mrg/k7W4NZqBcO/N6G/P+Sdb1/z9bN6+3h4ex48hICQYrDnLWEpuYYG6P4yhsD5KSVkpMrIy4SLw"
    L"2bRtO0oqKsFlQq3i1ZMaz3ji5dfw6S99FW+v34rYrALUcCErUQRcxIowIDAIFO4TKoQEzR4oBioGIr8OKPYujz1MjKtDmBig"
    L"2CuTmWInMwFFYSKd035oeHphJPtRHXZo7ePT2vsdt6hgnm3/Sla85A4Xx0CEpi9mf8ACj3hk1to8cuP2K1mfkwuZbXfai189"
    L"9kvsdlolR8SlrbmA+Kg1iAlajNSQt5Aa8A5asw+gM+sgWpK2oyleOkvcchSFvy6Sfh7yw18TmLyI7KDnxAQoPo8i/vw9mvXK"
    L"jhd/9odTIFH3xgaSKA/C5MvSgf9NOzPVQbArR1A+ox2fUKCysAeJMQJFQaJm3CGBxv5PCjz+4TY47E1eV5WPvAffK/TwlxDu"
    L"9pUpVybq+FfVYk6KIjn9dRtIRI3IZ486+31En/0RYs/9TCGZ4vUg0n0eR07gC+ralMYsEtdmGcpT16sioWtDkHQ3imvTFYPx"
    L"oSzcuFIs0KiWn6Vev2dAgHGTMOmw4EGIXG3EdXGBrk5U4tpoBW6NFeHaQByCLqxAWYbPElvX/P1sVVVVf+XkvAcmlhKXlIi8"
    L"okLkFxcJOMpRW1+HUoFLcWkJfMU1WrVuPZgUR6Aw6MdsWSa0nfHyx1nfQFEeXWiUK2Nt1wBqpTMTKAQJA7E0A5TssgqFw28L"
    L"FAMSYyYQS6DYw8R+VIc2E0ym14MlUKarE4WJ9E57mNgDhQgwIzs0dmrt69Pa1PE77jQwYJc3r2RB5XYA19bMC9y6EyLGJiav"
    L"3H6IMblLvBrblp92TF56EPlZYYgJc0VE0CaE+i1CYsRCNJRsQUX6SrTm7UJH/l5URK1DY9IW1MStQmXM2yiOWKCqJM1P3Bvf"
    L"J5Hq85gqk6RL9yPF+z5RJeLiyNWcMNEAplEkauzI/6ZqwMAk5LA1wY+dn4rCuCrvDxSChMFaCyQc2jVxEFU5U0bVQ5eGcRGC"
    L"5HNiX9Aclyh3Ak7AYWI5U4rkPyxV4inumeePdZJi3AVRW/L3pfk/jiyBZ0H4PJRELUZ5/HJxbTagNmuXBlvbqzwtkHTHYmww"
    L"UwBRJidCrXzphIgAhCC5xW2X/AiyvSogmRTXZ7wGkyMlGB3OxehgNq4M5mCyPwkTPUEI8FyClNAjXrau+fvbzp0/H06VQrcm"
    L"KCwUGTnZqlQIFQZlCRNChXVSmI6/adtWTYhrbG/XCYCMpagbIb57U++gAKNdrFM6Pt0aK/hqry7o7uRX1cwIk+kujz1MCKKZ"
    L"gGJAYrZUJozr0GaCib2rY9TJdFdHYSKd0mTB2isT4+rYg8TYe9qMdxog3NYS9iC58+E26OjwDM081w460q5fY7arHJfH3bgi"
    L"7swtec6NYXlqE7prI5AUshPJYZsRdvFNFIgC6aw8gc5yd7QUHUJL4V5Ui3tTkboKZYlL1fIi3kRm0FzpVC8JRJ4QeDyKdL/H"
    L"NYYQd/5uxJz+sYBETF0cUSWn/sOWW/I1jUkQJpHHLFUSdsQa0VFlQhOgUF3QbTFAeV+wTANJqICENjNAPjflzlAV0c0iSCyY"
    L"iAqxxUf4WWPOymdmbsn5HyH+0l2aS5Ls8ysB51PIkL85P3IuimKXoCJxLapSt6A+21ljJF3V59DXEiiuTdwUSG5dr5PvXoAB"
    L"USFq3fJTiGtDRXK9Gbeu1uHmaBWuD5Vgoi8bg51J6OmIFotUZTPcHoqBJk+EX1ostqPI1i1/f1txcfHfHzh0EJ7nz8E3MEAD"
    L"tFl5uVaQVqBSXlmhW5qPQOfdVSvBHJby6hpNcuNkQCoVztFhLgrX8i2oprpoEoViDQ8TCgRJQXWtGo9x++uAQqOiMTDh87lV"
    L"mwEoJnZCZWKCxb9OnUwPxE7FTUSdGJjoXB251NsDxdIS/0mgvKfdBgPN/jXufArfibAw72o1c7+1ELocvyWP4RAkRw94VbzW"
    L"iOy440gP24OUgPVID1mNzjJX9FW7oa14PzpKDqMmY49cdXegKddJYLIaxYlvib2B7Mg5yAibi5xwAuU5ZPg/IVB5VJUJXYG4"
    L"83epMmHMhJ2TV3zCJPz4VxDu/m9iVnwi8vgXLbfDzVImQS7i5rh+So3qxHfvnTB5D1B0iNceJp+yhnrtFEn4HXGRfxMX6yuI"
    L"PvbviD3xDcQJPBjHYXBYYzqnZavxke8j/qJ8fgFJwuVfINn3IaQHPIlMUWI5YXNQELUE5YkrdNSmTtzAxgIPdFScQ39zIMZ7"
    L"4nFtJBc3JkttiqRFvm8bSG6KaZzE2t6abFCX5upQoYJkrCsFg63R6GwMQHuDFzoavdDb6IvBpsvoKD+EpICl8Dm+ptnWLX+/"
    L"27kL5yOPuB+FbBEaHobk1BSNm9i7P4ytpGWkw+3oES24w2U5OIzM2cpcM5kxFXZuKoSU3CJkl1ROJZgRFIQKFUduRZXCwgBk"
    L"OkiMMqEZGNmrFIWLDSpmVIfva2InRp3YA8VencwUO5muTlSZSK+9owyBGLu0gYk9UN7T3vcOq5m7iRLdyj/6cPmH7gq3Oryr"
    L"72a98w0BGx9zS/65zkN6g0CiM0aQtKK1JhhpUbuQHb8diUErxZ05iLbSU+itPYuOsqNoLtyP1qK9aMzfjqr0NWKrUJH8Lkrj"
    L"3kZh1CKV+Lkhs5AZ+CLS/Z9Gmu9jqkoYT0i6fO/UaI6JmVhXfLETX7NAIuog+rjAxOMLtjyTf7HcHFEmChNRJmbm8J0K5RPw"
    L"2/d3ujXBVo7SECI0ukgM4pqRGqoSjhYRIlQjChKO0gjcqERodG9McFhHbM79AEnnf4Kki6JILt+DFJ+HkBX8NLJCXxJFNg9F"
    L"4uKVJKxGZcpmNObsFfC6o63iInqbQjHak4jro3lyIpTLD2SLjxiYiGtz6wYDr3LsaiNuXanFjZFKXBssxuRQNoa74tHXGIKe"
    L"Ol901lxES9UpNFV4oLnKHU2iEjsqj6KpYBfSg9+F/4nVHbYu+fvdCgsLP0aVwmQ3Lx9vRAssUtJS1f2hFZQUo6isVFVKQFAg"
    L"du7epfN8OA+oo6cXLR1MuZeO3tiiWaZcDJz5KQSKAYmBBW8TIPaq5P2AYg8TYwYoRp0YhTI9I5ZmgDITTOxjJ/bqxACFiuT2"
    L"Mhd3KpPfFVCMMT+EkKBdmZBXtt1hzQ5Wsky1a3Rv6NLcYnRnEBNDAujsi8iIPYC0yG3IidmgV7zBhmOWa1N5Em1lx1CffxA1"
    L"WbtQlbkZNZnrUJ0pwEl+G6Xxb6KYMAmdj9zgV5EV8Dwy/Z5UkHBY2MAk/sLdChMTM6E6IUyiTnHI1UqjJ0gi3EUxHP3cHepE"
    L"3ZxpQHk/4/0myBrm8mk1woTKhLkrdKGoSDhqFH3sqwoSGod6405ZKiT+1PcRc+p7SDzzQ3XLEjx/hORLdyPd+37Nm8kURZId"
    L"/Ly4NqJIohehOF4USdoW1GXtRXPBUfRUnMdAvSiS7mSBQh5uTJTJF18j3znrxzBO0ipgb1GQ3Lopt2/Icd4/Xo6bQwWY7M7C"
    L"WEciBtpC0dPgo64Sf4fW0iNoKNyHuiInNJQ4ozZvj7iczmjM3oj0oHfgf2xVj61L/v638+fP+7H045kzZxAUFISomGikZ+cg"
    L"JcvKpKVSKSwuQFZOJs5fPIcVq5bjwqWLOhrU3m3VQuEcHXZiLvLV0js0BRMGZgkL5qRwO5NKmQ4TM7Jjr0wMVAgTKhPj6pjJ"
    L"fwYmZq6OcXdmUib2sRN7mNDdMWn1ChJRASbKQfutQDL1ACoIY9Pa9MdOM2a2mqdyqZzr16hEOOTLJS+a0d4ag+T4A8hOOYCk"
    L"8M0oS9+HkZbLmGj2wnDtOfSWn0RX1XE0Fh9CXf4+PXlrc3aiPG0DypJWozxlJUri30ZR9BvID5uPnKBXkE2YiIuT4fNL6XwP"
    L"aO5FyqV7dfg03vNn0jl/pB01+tS3VZ1QEejIiQfdDVELAhR2eCsAy/iGpSgIAzOycxsq1miNvfE+E3C9rUYs09wReW2qoOmK"
    L"ZMqlOf19xJ79gSgoK8bDrFYrj+ReBUlW4FPIDX1RQVIU+ybKklcKSDajPleUW8lx9FRdwlBjMCY643BjIFNOgCr5/gkRxkio"
    L"SGiWOrl5o0F+o1rcnKzCrYkSzSW52peCsfYYeY1Q9IkiYYJbW4UonRJXUYb70JCzHZXpa1GevhpVWetQlrJGXM+NqE55FxmB"
    L"S+BzdNmorTv+/jfOQt6zZw+OHj3Kha+1ahsnDxIoVCmZuTkKE0IlKSURLF7M3JTI6Cg0t7TpYutcprS0tkFnH3PFQfuRHULC"
    L"gIPqhGCwh4k9UPgce6CY+IkBismGNXETwsRenZj5OhostsVN7EFCZWLv6tjDxLg7BIo1e/g2UAiT3wlQ7B7DOTY35P25rzVd"
    L"dYhGVIm8782rQhX1fRhHGdarYWb6GaQmuyA1aQ+Kc13Q23wBg2LDzRcx2nQR3aXH0V14EgNVZ+QkPoC63J2oztom2+2oy9mm"
    L"J3FR3HIUxy4VmLyFwvBF4ubMRnbgS5pKnuH3qADlQb2aW0C5R9yFu6Sj/kg7LDuuBRQr+5VxE6uTW0Cx3ByqE3F5NGhqyzuZ"
    L"BpU77OCn9T4an6PwsDPGSqhKNNh67CsKEiqS2yD5IWJEjcSe+YmCjwBMvHifKiwOcTMZLSfkBRQKSIrj3hCYrhKFthkNefvQ"
    L"XHwU3dUX5TsMwUR3PG4MZclJUCI/PId/m+R7bxNrl99DQMLbokYYiL15tQLXx4txdTgHEz1JGGmLxGBjAPpqL6O7XFzM0mNo"
    L"LT6IxgJRIwKS+kwrSZAgy4uZj/z4RciLFqglLkV53CIByhvwcXvrg7UCo7+//0YnJyeus6KuD8tFxqemIiUjXaFCoOTm5yAv"
    L"PwshoQFYv34t9h/ch8zsLI2ncNSnUiBApaKxDenwDL4m5uQhq7RcIWGAwZGemYBiDxOaujjTgGKvTt4PKFqBzZZzMj1mMuMQ"
    L"8TSYsAuzKxMkJtYx3d7T3vMAAxOarU1/jBrvF3jcFBVyc0T2qUSYms2AXytaGxKQGn8CuRlnkZ7ghpbqAIx0RmO0MxyjrYFy"
    L"El/EQO15DNedx2DtWVEnJ9Cctx/tcjI354qUz1iHqtQVKE9apnNxCBK1iMXID30d2UGzRJk8h3S/J5Dm9yvpiA8h2es+vbIz"
    L"nT5BOik7LWfcTsUlCBSPr2kcYwoo7iYT1gKBBRRRKxpEvQ0VY7dBYgVaLZh8TgESIQBhwHVKkYh7Y1Li9TOIUqJiohLhzF+O"
    L"PiVcuBdJlx6Uz/8r/VuYO5MXNgsFUQv0765IWilu3xY0FRxAS4m4NjWXMNAUiLHuOFwbFpBcEZBcF5AwIY1KhK4ME9MIkmv1"
    L"oliqcWuyAjdtILkymIyxrij0N/mht+aCzijuLPVAS8FBNGTvEhdqM6oEXpXxAo2YxSiOnKfJcVmhLyA7/BUNAhdGzBfQvYaM"
    L"gPm47LLog7ek6969e9u4HOnps2fAcpERcXE68kOoZOdmCTwyFCg5uRm4dOkCli9fhpOnT6G0skInp7H8I3NTGDRlhyccUvIL"
    L"kVNeqfCg28OtuW2AYq9ODEwID4LEmLk93d15T+zEVtJx+jCxAYp9ENZ+VIcwmVInYgYod/R9O3tPe88Dfj1MdNVPqhJ5t5tX"
    L"h2RrRmro1rQhP/00inNOIjv5MMryTuP6UCrGu+LEP4+WKyJhEoKBOh+ByCX0Vp1FU/5hneHaWuiKxpw9aMjcKlfG9ahNW4Xy"
    L"BHFvYhaiMJrJagv0RM4LnaNxk0z/F6QDPqVXc17VmWfCpDW6DAYmnOjHERNrfs5/qMthEtcIEhpVhBaUZoaqbM1ozG0zozTG"
    L"rON3gMRNXuuIuFBHvyqv+VVr1Oa4lQ5v79rosPXZn+rM34QL8nltIMnwFZAEPacdl4FmJqMxq7U6YysacvegtdQd3TXn0dvo"
    L"r4pkcjBDXJZi+aFtma3GtbllA8kNG0wmKnFztAhXB3NxpTdVgT7QEoSe+kvorDitQe+2Ihf5DfaIG7NV3Upm1HKuT0GwgCPo"
    L"VeQHPm+pwMAnkR3yrAa/VRkGPotUv9m4cHDuBw8o8fHxP96/fz9c3Q7D8+Il+AYHa9IboZKWkapGmBQW5SItPQn79+/F1u3b"
    L"dEi5oqZeh5FZtoBAoXrgYuMN0uHp/hAcVCwGHNOBMpM6mQ4TqpXp7o49TKbUCcs5zhA7MUAhTMyojomb2AOF9V4smFCfGLNY"
    L"8J5m7phuvw4obLoltiZw4yoh0oeuliwU5vghJ+U08pOPydUvDBjJxPX+JFztisGVzghMdoRhpNkXgw2X0CVqxLLjIrM99MRu"
    L"KXIV98ZZTuj1KE1co8OgPLmZQl8QNRcFEa8gL/wF5AQ/i+yAp5Hl+zgyvMXVufwwUi88qLOFk87chXjpsKz/EXuCMPmedG5m"
    L"wgpMbECxOjxjKF/RICmhoBPupqAyHSgWPG7bv94BkqijX9aZvjrb10PUyDHLtYk//V3LTglIRJUkigumwGOBo8sPIc37MQ0k"
    L"EyT5YS+hJGqOuBFv6nB4bdZ2a3Sr9Ai6BboDzf4C5VhM9KfJVYSlBEycxAq2EuQ3aNfl2FUByaSoktFSTA7kYrxbQNIei/7G"
    L"EHSJa9NeIW6lAKq50EVgtRs1mTaXUiBWKAowJ3SexqZyfEWReD8j3++jSLn4Sx2C5whaiteTSLn8BJIvPSKK8AWc3ffSBw8o"
    L"bCdOnDi678B+HD1+AqcvXNBykBwmTkwW6Z2egvTMNGRlpyE9XQCTloJtO7ZrPCUuKRn1ApQaAQqzaVm9nm4PV98jEKhOCIyZ"
    L"RnhmUifTXR3eJpimj+wYkEzFTuT9rDonBMo0hTIm6uTKTQxP3lIbFSlir07sgaL93da4b+x9m7nzjgcbqNiM7o0mqtGpYqB1"
    L"CLeutqMwOxBl+UHISDqJujJ/3BzLlQ+Sjytd8bjam4Br3bEYojwXF4cw6a8R16bqFLorj6GrQq6OJaJK8veiKmMHSpI2oSJ9"
    L"i1yZN8oVeoUoE5YbWCwn+XzkRryK3LDnxRV4Rk72J5EV8KgA5VFkev8SmXKVT5erffLZu5F45qdiP1Y1QFdH3Rw1DhNbIyuE"
    L"iBWQ/YqVDSuAYIIZIWEmAVpzd6wtIWIFa+U+sfDDzKLlcPNXFE6cRBitIPkPNcZqNHeEEw4FJAy2MkZCmChIRFFl+T8jKusV"
    L"FIbPQXHU6zqRkUHOuoz1GiNpEchSkfQ1+mK4PRxX+1Play8UkHDinkmRJ0isERvadYHJdQHJ9ZFi3BjMw2RPuoJkoMkKtnZW"
    L"nEN7yXEFSXMBg607UZ26DiVxS5EftVC+4wXIi5gjbswLopieQqbXY8i48DCST/9c1Bbzd8RVO3+XuJMPiAK8X/4mcSsvPokz"
    L"zs9+MIHCtmev85CzQuWYTiCkRcXFIj45CUlpqQqV1PQ0jZ8Ei4rZuHEjtm/fjsREgYrAoUlAUCqwqGppRSFViUCjtKEF+VV1"
    L"KKxvQk5VLQrqGpFfawVpCZWyhkaUNwpUZEsjQAxUuJ3KObFVfeNsYrVpsZN2Fk4aFaCMcV2dEfSMWcWTWCN2QOgxIv14TPrz"
    L"hMCEZtbRMbETGl0ebQIFdU3sij2TE/ZxFZYG4JbDvtxa/0hThsiNa/LKHOq9OihbujV9YqLeKmNQWeCPwsxzKM+7iJ7mCPHP"
    L"M+UD5Kh7c30gBVf7kvSKOtoegaGWYAxKx+itk8fWnEF39QnNYzDBv9qcbRpsrEhbp4HHiqQV4uosRXHMmxpHKJBOx5hCboil"
    L"TrKY0GWrZaIB2AuiTGxuDmGSePb7ogykU3P2LUdU1KxFuJgJq3b8i5qDwqLOQRwi5lCxwCLI1RqVCT3K7b/IVtSI3A47agVX"
    L"mVFLkESIGjHZrFZG67d0tjIhEu35A0SJMTU+9txPpNP9XGM7HNLO8HscecEvKUhKo5egLGG5AHS9qoQm6eSMkbRVXUB3c4hm"
    L"tl4ZysCNMQEJc0lu1MnvQlUirg3dGjUqkjpcHy/HteEiXB/O10S2wdZQzSHpqbmMnurz6Cw7KS6lG5py96E+YxtqU9ejIk5c"
    L"m/D5yA+V71Y+E4PAyV6iOgTS8RfvRfyZnyH+xA8Q6yF/o/y9DCxr7s5R+S49vqUFnCJP/gJn9z71wQWKuD530ZVxOeKGY6dO"
    L"4pKPNwJDQ9T9SUxNUagw0Y2WlJSkI0ObNmzEPue9yMvKRk1NjabnF9fWTS3cVdFopeMnZOcjPrdAgUK4UKVokLamVoFS2Syu"
    L"T2PjVFKcPVBozHfh8LSZTTzd3bkNlDF0j4tCsasRS6CMSv+eAorAYCag6KgOCzkTDgYQ0ji7l0lmNFM9fqrKonmsMQWJvNot"
    L"Idj1ATnQJ8AQNZbpi9LsSyhIOYn26mDcHBaI3OBVM1/kUpbCROd4iI8/1hmDodYI9DcH61W2u+4yuqrPCkisvIbmEsJkj47i"
    L"1GRt0mFJptFXpkgHS3xHrppvaLwkP2w28kJfQ27oq1oUKMP/Se2U6b6/Qrr3Q1rTJOUi4yY/E6D8RJPXCJO40xZM2AF0dq4N"
    L"JhHHvjCVEcstgcHiSaFHPq/GfYImzF0UizGBiZVNy7yVryLSQxQJQcLaKae+N2Um2Bp9jnNsmNX6c02PT/N7GFlBT8jf8byW"
    L"ECiJXiiKZJkoEs6z2SEK7aAqks5aT81CHeqMw3h/Fq6NF+LmlVL5fmvke2acRAAiMLkpAFGQCGBujZcJRApwfTBbXZuB5nD0"
    L"NvgovPX7rjhhuZJZe1CbthV1aZs0qF0a9QZKIuYK3F5Aps9jAuYHp+rD8PNHnvkeIo5/04Knq3w/DEYf+kctxs1q+oEHPo/A"
    L"g59D0JEf4/TuJ2/Zut8Hs504der0xs2bcOzEcTVWeCNUwqOjVK0kJCUqUFJT6fqk4ezpM1jx7nK4HXJBdmaWuD8tKKmtQY3A"
    L"gGApEGgwnlLd1oWs0koUiWtkgKJuT4OolKYmlDeLyyNGN6hclEiluE/1ApBOkRZtg6MaO6FSsVcmxt2Zip+wrOPIVfSIT9M7"
    L"dh194zesko5CD3V1bLEThclUAptAQkhAu64DxFZYVs465QP/1SJnU403rMfcvEFwyG0qEh66wVfkaA3VSDcqiyNRlO2P4lxf"
    L"5CSeEZ88R95cfHjaaJ7mPVzrS8d16QCTfRkY60rCaEeMBl8HmoLRW++H3lovTZLiyU3fnaMVDflOqMveocqkKk2ulqmrUZa8"
    L"whqOTHhbO11+mJzwApN8sbyQlzUIyFEQBjFTvH4pHeAB6QD3TBVIMkBhEFTVic3NMXN1LPuSbhUusiU8dAjZdj/3zURBc8wy"
    L"AQmPC0wiT1huDSGirhVHksQYbKVrw2S6xMv3alZrZuDjoqye0xER1iJR1yZ1jcZI+D20lh1Dp7g2/c3+GOuJ1BEYjZFoLgnT"
    L"5AUkagITmg0kV0eZ0ZqLG/IbXBtIE5CEoavGX79rwqRVAFWbe1CLJ7FcQXX6JpTEiGsT+roV0PZ7Vr7DR5By6X5kMHHOizk7"
    L"30fY8X9HiAA42P1LqtZYGY7lJrkmkJfT3+Dyro/j3JZP4ML2T8n+P8Nn3/dxYvvTH1yFYtqOXTtbWAKSqfmnz3lOuT+hkRGa"
    L"/JaUkozk5GSkpKQgPTUNHkfdseydpXBzc0NmVg7Ka2tRKUqkggFXcW9Y7qBzaFxUxBjKGlvV5SFQND7S1qZQoZW3tmnRJgKF"
    L"RheJdVcCYxLQ1j+iMRR7oBiYTMVPhifQM3JTrW/8lsDkpq0+7DUMi28ydvOGztOhWUCxYEJlQpgwCEsyEC/Xb15TRUKgkBem"
    L"6aQ8ujLUNDfHZN9mnJh3awCTY9XIy/ZCcZ4fMlLOoLE6XCX3jZECPYExUSQnvSiT4TwFydVegUlPpvjrKQKRaHFzotTN6Wuw"
    L"TvAukfBt5SdVzrMOR32ulazGodDKdAsmpUnLUZywTJPWrCxYS51QmeSFvKqSnNXWONmPMGFNEw4PMwHMXp2oy3HMNkSsIzvf"
    L"UAhMqQstMm1tLXBM34qi0Rm+dGWsmb7GCCgrbf82TKwYiQUSDbhKB9WsVgEJizkxkFwUs0AhWZO2FrXi2jTm75fvwl1T2jnX"
    L"ZpRBawHJjXHGn2y5JBz6tcshwXWBytVqUSzlmgXLlPrxvmT0Nlnp8f1N4lLWXEBT0RFVPIxLMeDKBLiimGXICp4n3x+/y5eR"
    L"dPFhVXeZPvIdXvwZYk5/S78TwjTs2Jfg7/ZZ+Iga8XX+ewWJn9PH4LXjL3F+y0dxdvNf4PSGv8WZDX+P81v/CRd2fkeA8gGO"
    L"oZgmsPjKytWrwNR8zuPhEDEzZDnnJyIqUqHCuT9xCfEKloSEBLi6uupK/Xv379OEuKq6egVLlaiRYk4OFKiw6BKDr1QmJkhb"
    L"II+jFYvbQ6BUiDIpEjepcWAUF0KjcM/jz2LFtp1oHRjR4k1N3eL29AzednVsMNGC06JMekaAXhEJBIqWdBQADN4Yx/CtKxgV"
    L"V2RMgDEhkGAA1hrVkXNPUcEm+BDQECI8Zo7LIdwUdUNT5tDfuU6w2LJY5SrYUhMuEDmHovwLApSzmBzNxa2rxSK/M3RyGW5W"
    L"yBsWiauTrXZjIBtXutMw1pEsEEnS7XhXgiqT/npvzZforDiLdvHfmYjVVHBIYLIX9Tm79ArNqyYrh1Umr0J54nIdKi2JXSyu"
    L"Dkd0xNUJm4WcEAFJwPPi6ghMfOjqcJThTqBQnWiWKWcRSycnULh0BI1B0kgPCyo0JrVN1T1hVTbb7amtmI4KHfuuBSbbkLMZ"
    L"+jXDv5rZyviIbeSGn4fJddmBT6Ew7GUtN1kaa4GEioRqjLOkOZ2AIOlpClTXZoIKT+DMPBGtScI4yTWqEOaTiClI5NhEpQZb"
    L"J+Q750hPX3MoOmq8dPiXcakmcWsa85w1s7Ve3qs0cRXyxLXJCX9dlR6BzGAwP2umz306R4h/s8ZFTv6b/N1f0ImQXs4fh9fe"
    L"v8Mlgcil7X+Fi1v/DJc2C0g2/DFOrftjHF/7EZxc+7c4seYT8Nz8aZzd+k2c2vYhAAqbX2Dgi6wryzk87sc8dBKht6+P1pkl"
    L"WAiVxOQkxMbGqlIhVDj0vHjxYnC0KCMzG9X1DagUNVLT2CJQsdwfVr2nMiFUMopLkVZSKi5QAyra2lEq6qSwuR21Ao7ipnYs"
    L"XLUed//qKZzxCxGYWKUSDFDMyM6Uq8OFusYsd8e4OlQmg9evYkhckxFRFKOiScZFfUyIGqFzoo6NIYfNzEQ90wiY2/cLSHRO"
    L"jRUfuTbWgKrCIFQX+6A4+ziaanzkRC7E1QmR01fyBShysk8U4sZYvpzUhXJVTBVXR+4bojJJ07Tta/2pGjfhUHFHlTf66rx1"
    L"hMLAhPK7pdBNr8zMqWAA0sCkKmX1VBCWk/0IFCqT/HC5moa9opmimbZqa4QJYyfsvAxy0t9n1TWrhKOVXq/K4dj3EHdcOr0N"
    L"KgTD7QCqpTS4VXjYKRDrvm8hyoPPsYacOfTMYKulRgRYAhLNJTl/FxIYuxFFQheMn5FX/6KIOSiLfUP/nmpRXvXZVrC1vfy4"
    L"xpEYU+KkPQXJWJF8v4SIQINKxCiS6zaz5ZHcGC3B1cE8eV4qhtpi0FXnh75GH/TUntP4CHNIWgud0Ji9RcGcH7FI7HUNZjNJ"
    L"jmUctLDUxbtEmdyF2FPf1KVUY0+KK+f+OZ0+wDWVaZzseHHnX8Nz65/j/IY/xbn1H8GZNX+EE6v+D46v/iMBykctoKz9v7i4"
    L"/Z8FKN8QoHwIXB7TPDw8jm3ZsgWc73PUw10nEl68fEknC9IIFaqU2Pg4nVhIuFCpvP3223BxcdHZyxWV1aioqUUFwSIg4UgO"
    L"R4EYdDUKpbS5RdVJSbPc39GDqt5h7DnuiR899DjWOx3Syvosb9gkxwkUzhlql9sdApmpvBMNwk4ITEYEJsOiTq4oUIau38Sw"
    L"QGJIgDAq8oJAuSI4uSo4oXOjoKDqIEiMqQqR/4U4HO2xQHLFyhu52YGG2jiUFfmK+aCi4DJGBQ7qu0OuhtfLZVsjV0ZOLpN9"
    L"2TKJiiMIN4dzNWZCkHBYeKIrGsNMVGv2F6B4Y6DFS4OBnFTGNO42USatxUxcO6Awqc/aiaq0jTpcSWVSIR2gLP4d6YSs/fqG"
    L"Jq8VhL0mUHkJeaEv6qgORx+suiZWAhv9fsZObs8itoaJGSSlESYKFAGLqhSbyrBcIE7Gsw0j20ZnTBYttzpBT0HyQ4GTGfb9"
    L"oZY9YNHnOJZYvCQg8XpAczJYWpL1ahlALo9jLZLlqEndqH9na+EhdFQQJBflewnBSLd8XwISTtrjXBpVH5rRKnajRb5rgchV"
    L"USeTtfJbiCIZKtISAiPdKQKSOIFIKDqrvdBReVbrnBDQzflOaMjaqEqIw8+l0fMVbAxea4xEg633yvf1YyScFeV24qvyt34J"
    L"YW7/NDVbmtXlfPf8tQDio/Dc9BGc3fhnOL3+T3F29Z/g9Mo/wqkVf4iTYqfXiVLZ9DGc2/opdXeCDn4ZF3Z9Aye3P/rhAQrb"
    L"9u3bM9atW6dFq1mUyUCFSoUV3ULCQrUQE+f3cEuonDvrieXLl2Pnzp1ITEpBjYCjVlQKoVJebw0PMx7CkRnmq1S2d6jLQ4VS"
    L"J65NbEEZnpz7Bh6ftQChSVmo6x5GAxcWE6CoOukbVqBwfSADFGuNnVH0TQyg/8ogBq6OWwWnBQoEyqiAY0y6OrNArggtGDNh"
    L"ffopoFCuGFPAyB26DgXjJWMYHqhFbU0CaqsikZN1Dr1diXK/QOOWnNyQE5lXR14VJy1fnVdGTIjPPlggJ3gJbgzmizLJUaBM"
    L"9spVVmAy2hGCoVY/9DaKIqk7LSf8cZ0t3FbmIfL7iOY8cMGo+hwn1GbQzdmCmrT1qkwqRZkQJqUxb2pmJif7sQwBO4QODwc+"
    L"rXVNFCZ+ViCWc12s5DAbTFh9TZQDlQlhwuUv4k6KkhCYxHh8V1wey20xOSmECEeBCBQDEZ3tS5ic/q6oHQHHadaZFXdGlA/f"
    L"Q1WQXN0JkoRL9+D/a+8/4KpMk7Vf2NBhprtnpif0dE/qnHOO5pxzzlkMKDlLzjnnnHNGQBFEEJEoKIhEJQgooiLmcH1VtVi2"
    L"M3uf95zzve/eZ+/d657fPc8iCvR6/uuququuOp4yHyfSl1I4sU6OtTnvw+XxLaWmaD1hLVW/F08H4XJLnOSSuET+1tXjTygS"
    L"/lvz0S9tBgk7yzNIbhLEh5vwYKhWTm04wT3YmYfL7RmkShIJTlHorAtAexXD2U4a9BoKtQQkbPdYnbkaVZxnSpiFUj5dYt+U"
    L"6HEoDPuKwpsPpZv6oM8bYIsFJUQS7F5ArOVzCOOQxnA0qY+xCNAbgwACSJDmaARpjEIgb52nEWZKIZDNK0i0f5tgxC0FXyHO"
    L"+QsEWM3+ZQGFl4WFxRldXV04OzsLVFitcN8Pq5SUjHQ5Vs6gMIiTtXycXFhwRDqYDQ0NQV8rIdLZllaZVHiuowNnz9PuuCBK"
    L"hY+EubiNT3f4ZKeV1IeZux9+nL0Itj7BaL98Ax0DwwKTjkvXBCYMFQZK39Wb/9QEeIlCnyu3buL6vdsElVuiUpSjRBkkQwQL"
    L"NnPm42IWHnx685DNWTnOYXjwfsBX/mxF7Uhz42GcPpWBpoZsNJ7OpI/xKyNvflLzKyQb7NDm2Sv85OZXyNsEGXqVlD3ciAfX"
    L"6yi0qcTtSydwh6S3HA1fzBGYDHZE41JrCPqaAyjU8UNvkx8unvGWV+jzkhxUnDRwQlJCHVImDBPuFVEqE36Fr8naoqiJoDDn"
    L"JN2w3K7PNy+HOUpLAg51uGxduogp9ODmuidhwlt6d2hLXQiHK/x4xCKgIPwb8LwdzoXwqQbnEkSBKDebMElpPOdGuHBrkoCk"
    L"mE9CUuaKF0lFzhpU52/FqcK90vnbKKZGdtI6wNW/nIi+2pmDW1zYd72c1AiBWTxb6W/OqoQL0p7wIQGPq6C/8SP6Gz8Y5L9x"
    L"ISmSbPS3JaGnKVpR2XomgP6Wngowc2VroTZqCMJcz1KTRaFhMqm42LkoiZmKktgJpEjodwn7QnIkeQHvIJ8bIN3/ikS73yHJ"
    L"/kUk2f0GcVbPI8x4LEL1xyCEwppQvacIJKPhsXcUvGkzSEJ1nqL3P0uweR5RVn9CousbOEiw5hqV6vQ5SPcbT0CZ82DkNvtl"
    L"LQMDgw4uZOOcCidpOa/CaoUd37iqloGSk3sQ+fkUBh06jJycHISGhsLMzAwmJiaIiY2XkRzNrE4uEEza+ASoTUr2GSpcTs/H"
    L"w1GZuZi1ehPW7tZE8amz6Lp2Bw2dfWjrG6SP35Rwh6HC4Q5vpTMbV8Zyef2l6xTq3HogCVmZqUNPQXZh45zJHdosQO4SP7ip"
    L"V7EUCkSGMPG4A4LI8MBZtJw5hLamQ3Ja09dDIc0DrrAkaMgrI7t1se3fyBbHLnqis+xWAoWf5Dca8HCoXuT33QHuUC3FzV4+"
    L"GlaEOhzmDLRFEkyCFCA564veBh9017NNows6quwfJ2Gby8xEnZzjxrOjWjhLMp3zDfwKK8VrpE44eSil9RmL5aSEgfKkWZK4"
    L"1XMRG93wDBRFN7ECKspdGPqdbCl5Z6gQTCSZyp4jBBF+zKqG1QiHMkrbAIYUf98jfHJE/1ZJIoU1yXMorJk3ApJVBBK2WVRD"
    L"Hf/8xw+I8XNbtYfYCHDH7vUL6bjZX4TbHNqQImGl99iT5NF5ijzpOpIfkbk3w6QQSZE8GCjHnYv0d+08OFIEGCU1O51nfOR4"
    L"mU/FuACOQXIqf4+EhtUMkrRlOEGhTWnUNByLmIRjkQTa4M+kEpgVSQ7bWHr8FSmOfyKI/E5ObCIprOEdYfQ0gnVHIUBzFPz2"
    L"j4KP+ii47yGY8GN6X7D2aEQY/gqRJr+jEOclgUlO4Bc4Qv9WBQG/OnsRDobOQoj90usjt9gva1VVVT1Hoc8Aw8HOzg5uHu4I"
    L"iwhHeHSUbAYKhz88JCwrKwsFBQWSqI2IiBCgMIxY2fA0wnNtrdKp3NbVJX1AnFuRsaVdfdC1tMekhctk3k/fzfvoorBGwp3+"
    L"a3L0zDOVGSSsTJR9OgwT3jyTmHiCISLHVRIZQ0SRIVYot29L7cldUiS3+AGHObwknOGTmn7cJACcbUhDY10C2s9loKfjCD1x"
    L"OSdCoJDmvav0mK4PB2gzePh0h5SJlG/zE51ePfnJz1WZt+rxaJheMfnJfrWK1IkCJoo6kwJF0VpHKq60JcpxJfeb8Kuz9OXU"
    L"e4kFgUKd0A1XrlAnLcdN5MSjif086IZsPKJO4Q7dnLlbJW/Cx8MVGSvlycql9WXJdKMkziZ1ohh9ocibTH4MFAYAA0UsAOgq"
    L"hWXKnMfIVp788JXf5jyIIh+iCGV45KgCIvQ9OcnK+YaEmSihEOtE+iKUZy+Xub/V+ZtIkexGfbGmHHXzlD0+WelpDEF/KymS"
    L"ERuB+1dPEIxP09+QFMl9AjMnXMXgiCAiR8BtCi+Sm2cUx/AEnvucpO0+iOukSK40UejY4IeeM264cMpRCv84ec1H6tw0yODl"
    L"4jhu1uMpgDz0vCx2qgx3Lwr5QQyuuVUgP4Bg4vMqqZE/IJaUSKLtbxFv9VuBCG9WJZwXCdQaDX9SIr77RsGLgMIw8SPI+OuR"
    L"OjEag2izFxBv/WckOb+OLAoRj9DfiOuBGg7zsf4aHIpaiVCHDWUjt9gvbx0+fPhVTU3N+xzK8PEwn+ZwVa0SKlxZm56ZIWZN"
    L"2dnZj6GSnJwMR0dH7N+/X3IxnG9ppRCn9TypE1IqbQST8z394lEbkZwOj9Bo1LZ2on/4niiT9t4B1Jxtlarb/mu3JNRROrMx"
    L"SBgsHPawMukbvAfiDa4MkzohoHB4wx4nzBCuLXn4SKFV7t69QuHOAAZJjTScycPZxlw01Kdh6GoZfcEZRbzOVgIPSIEwSO5z"
    L"Cf0QPbFZyVymj/XRJtjQE/7Ro1Y8eEBPdE7Csvcon+zcqMLdaydlJCV3qg73FeJ6z2Fc786jGygbgyN1JpebYwkopFIaQtBT"
    L"7y+hDveKKNVJ6xPqhNviedAUJxG5EY6HcPHJCN8knDvhcIfL6v8VJnya8u8BhYvJBCakMDjnwUfIP0OF30fweCIXwicd8vUS"
    L"ypDa4c15GU6ykhLifE15xjKczF6NqtwNqKUbp/4Imxrx3F8TUgrW4FqazsZAXGqLl9BEPFsHy2S0hMBYCRIpQqMr7/ukSOi/"
    L"Bydk7w1V09/0OIZ7D0vn9VB7Eq5yJzE3S572xoVqW/q7mYrdZf3Rfag9tBNVBzfJiRfXthTFzBDvFz7+PUYhmhyNs1M+be54"
    L"Puj7GhIdf68ACamSRJsXEW78jAAkTP9pgYgAhNTIk9uHYBKgOxbBJqMRajoWMZYvINGBZwdxxfBnKCBolaYvFutJPo3jWT+H"
    L"ozfB23ytxsjt9ctcmZmZXzJQdPR0ZbIgg4WVRyCFQVwEFxUXj7T0TGRkZiM1LUMUC0MlLS0Fvr7e0NXVhqmpCUJCQlBZXYW+"
    L"/svo6u0juHSho7tP8iNcVct1Jx19V+RUhweyl9c2yAD2SxQWXb42/HgrO4nZ74T39bv3pfHvBlfGklphT2c+pGGi3L2rtAro"
    L"QmdXCRoastDYcBA93eX0PlYcDAqucmUlwiqE3+b3c2jDAOFh1/RYrqxO+KiSVAzF+A/vM4TqBCaPj4avlBBMiuWmkTCnOxfX"
    L"OtnxPE2Oh7nWhHtFLjVG0M0Qip5T3MWqKKwSmIzkThgoipMdXUUitmCvQp3kbVMkFEm+K8MdzpkwTCRnQjCR8np6FRafk5HT"
    L"HYYCA0W5GTDcz8NVs1KxOlIjwhDhz+Wvka+LJSAxROKmS/cs960cS6B/L+Vn+4CqvC2o5lCscD/OEACbykxJKdjiQp0XKZIg"
    L"AkkCrhAIBCScI7l5SlFHwp4k3LhH8FBChBUff4zd0RgktweP40bfYfkbDpCyYS+SvjOkSE65oavKAedPWuFcKf2NivZKU2RF"
    L"9gaxfZR8UhL3Lo0kWyO/l2NuhT/tu8inEIdnHcfb/J5A8BxdFTUkfNwbQpCINPoVXZ+Cxy5FaOO5e5Q8FpgQXFiZBNLn8QlP"
    L"pPULiLP/I1Jd/0Eh0wfgo3P2bDnOiej8zfR30SLQUshH/y3zo7fDXGfxn0durV/uSktL+3yfxn6BCncdS1VtgD+CwkIJLGGI"
    L"jk9ASmq6AIXVCudT8vNzZcfERMHa2lJOgdzd3aWxkI+W2zu6ZE4v50MYJqxK2AHuAikUDnO4f4dPdC5eJlVCULk6TKEOXZXv"
    L"Z6+TwVt30D90VZKyN4gkdzjXSjDh6ta7t65g8HIT2s4V4uyZDAlvrl+vJih00eewOxcpkIfXcf8OmxzdwIN7BJVHDBfeDBcG"
    L"Cm9FmCOGxSLD6RX1fhMe3j1Ncpzi/mE+zSmXV12GCScXh/sPY+jiQXpF5rxJMi63J8gphsDkbJg0n/WcCkBXjSLmb68iuc4w"
    L"EXViIeX1Z4vZkkBxTMzVsKxO2I1McgFy0yyT487HCVgOc0aA8s9QUZzyKJUKX3lzTw9vJUBkbrFshRLhcEaUDkMkcY644nMp"
    L"f1kqD09fo3BGO6wm3bf1R9kM21wBElIk3DbAxXoMAnY6u3OVQULwZRsBadpjRTKiRjjM5NDxNv1Nh0/j3jUGyQnc6D0iyVqu"
    L"IL7UEi3WDT31pOa4DUFMjfSlfoXbDriwT4r6+KQrdQFBb7rAkfNAXD9zOOQTcC0JNzAmO7+EOLsXCSK/QwKFNtEEkmDD0QKT"
    L"MKNnEaQ9Bu5qo+C6g0Ib9dHwJojw5rwJq5UgvacQavgMokx/jTjrF0nd/AVpHm9JA+DR8PE4Hj9HlGNNwQ7Ul2hJHVHDMUUd"
    L"UWbo5l/eCc//1UpNTf1AQ0NDhqqbmpvJ7B4XD3d4+vvDJzBExm7EJ6eIrSQBiMCSipzsTOQezMbBnCz4+/lAS3M/9HR0ERkZ"
    L"iZqaU+jouihJWimvZ+MmAgwbN3HidZDkRt+V6wIRvvYOXENT2wWUnzotXchsmqTwOyGF8ug+bj28iQcYws3bnejpOYm2Zj6t"
    L"ScPli2V049MTVpzRGBSsWFi5cGKWK1X4MedV+HqdvscA7t6/SO/vxT1OyD4GCsf1ihvgEcX1D241yg3Ax8P3r1Tg7mUCSv8x"
    L"STJyeTjPYrnaRf8+weRSe7QkDvvPhSpujNM+csrRWeMh7fetfBpRbi0GQXxjNpQYSvUmV8PWHdorpklPTvfjeg4xSxLH+p+9"
    L"YZUw+VeoKNWKYhNICBi8uebi8ce5+I23EkwEqpKkufJqzzcqv/LLmE7u/OWZv0fVxSGuqdRMmun49+lvDMYVUhIcmtzpL1R4"
    L"trLV4h3OkTBEFLkRzkGxX+uje214ePvsY5DcvMwgKca1rnxcbktH79loRcFfvZ/UkTBwGbb1RVqoyd+NypwtqEj/uW+Jj8o5"
    L"zOPwTSwspbr1PXHkZ5BwaBNvT2rE9teIMBuLUApXgvRHyalNkN5YUR6sRDwIKJ5PXBksgVpjRbWEGT0tR8fJjr9DivMryPH9"
    L"EIdCfhCPmark5aijvw+HqOfKjdBSbU2ApZ/7pB2Opash3HVZ38jtpFq8jh079pKhsdHQPg11mJgaw9LOCmx/wHkVv6BAOQ0K"
    L"pzAoISGBoJKBrAwCSk6e5FeyM7OQmJgoncpcCGdpaSnwqTt7Fq093RT6EEguXx6ZVnhZAZMrQ7SH5Xrtxh30Xh5EU3s7Ll0b"
    L"BPflDFy/gjsP7+LyYC/OtdWgsbkEZ5vy0dychxv0BFX4YDAUKGRhkDy4gkcP2DVtmFQKqRJ6zEB5gKsEjwHcpxDoIX3ew5HQ"
    L"5wHXPTBM+PtwspBfXTnXIhWZZ+R4+P5gFe4NKI6Hb9OrsYQ7BJShi9m42p1KQInDlfYYkuwRFPIQUBr9FYbGte5iI/i4V+cE"
    L"j73gfp0DaCB1Uk/qpO7wfjnurM7dJjARa0EKNRTjLxaKapDcCfeZ0KvyvwcUbgpUvP2zeuHNAOL9s7rh/ItiK+0VlRWtnATm"
    L"o2o2u+YKXa7WbTluJCXyF2ootDkTKiEJg+R2fwFB9jiBhG0EOLThEJGVnQImDBbuAH5wt1kSrhzasLq7RSC+2nmQFEkGqblE"
    L"XGyIQNcpXwFVR4UdvcKbCGDZwZ5nMPOMHVZLJfH0N4hjkEwlRfITcgO/FLsECW/830aqy18UILF9HrHWCpAEGY5CAIEkyICU"
    L"hw6BgxSIMykS950U0nBoQ9ubwhzOnfCpTrD+0wg35kK25wgmLyLD5W847PcO2GWuKIrAnLgI1RnrcYZ+tpZiPZyvsEB3nTPB"
    L"0APdDd5ornBGjO9yuByYvWHkVlIt5aK7a4ytnWXnjp1bYGyqjwPmhrB3soaru5PkTEJDwxEcFI6w0GikphBEEjKQlp6D7Kw8"
    L"ZGblSEgUGh4CMwsTaOqow97FFpn5FJK0N6K54xz6rwzI2I6ePgpZBm/RvoPLl2/g0uVrGLpxk9TDLdy6O4gbN3tx5y6BpLEE"
    L"jWeK0NRYjL6LtXjI9SGiRAbw6D6HLBzSsPpQ+rjywfJNPHo0JJvf/4gNkAgkin2ZNn89Kxr+XnwjkCq5d5aUyRnFqQTL9yH6"
    L"t65XSsKQcyd3B4txe6BIrAjY5OfqhXRpQhtoixV1wj0kXAnK1bBSwFbjriivZwlfyUCxFJiwleFpumEVvrC7FIOp6FWPcycK"
    L"Hw4OdbiIbbFCoSSzkphJYYkCKj+HPiObXjlLY0nBxMyS6+P30+ZXdM6J8C6O5yuFNYmLcTJ1OaoJJLVZm1Cb/fOozqZiY5wr"
    L"tRiZssc5khCxWbzWk0dhSokkpsVqkf9OnGxlgHDxHzujiTsawYVtFlmRDNVIAdutgcO43s05pnhFl3VDiKLIr8oFnZX2aDpq"
    L"hMYCTUUxX9YGCff41KaUYKqsAuZEMtfUcP8Rg4S9bnmAGNeQJNv/FvEEkigzgoIJV7COkfDGT4vUBwHDleDhums0PNXGwHPn"
    L"aHgRVPzpfVykFqJLMCH4xNk8iwirZ5FI31MaA70IJMETcJJAcpJrgdhioVCLQGKGnhpHdNe64kpTAAabg9B/1g/x/uvgZbvw"
    L"v/8I0v/I5e3jVrJXfSd0dNVhbKIDG1sLODk5SCl+SEgYASUKPt6BiItPQWJSOhISUwUoXPSWkUVvJ8UjOMwfhiba2Lt/h0Dp"
    L"8JFDMrj9Ym8/ei9extXBG7KvX7+BGzeuY/jmFYJKD7q6eXRqEerq8tHfU4Xb1/mEhpOnA6QkLuPBPVIYYnKkrJVVQOTnfUMB"
    L"FALNIwLOv8Lk0ePciVKZ0Pe/SzcCF13xEfHNGmJUBR7Jqc7PMLl5qYAke67kThgogxeSJZcw0EYhT7NibgvLd+kpIaAorQma"
    L"T/LoBVImFOqwaRI/OTnUEWWSvUlyBJI7SV8t8p63Eih8ZFuSwlDhXAeri1kCCn7V5s2Wj8qtfB9vjvX5pmQlIlP1CFTcWKh0"
    L"RjudtxMNh9RxlkFCN3VrqTWBxJkUiTcung0VWA715Urz3V3upGbI0t/on05tOEzkx6xIKESUWpMbtZJz4lOwy/Q3Yl+TS61h"
    L"9LcJkAQ1F/axImEvkobDBBIe5k5Q5crWCv6dWVFFTRaLyKJIDm3Yy0XRuMcu/Gmuf5Xj3wSb3xBInkP0gacQYTQaIXqkNggi"
    L"rDq4EM2NQUIhjdvuMXDdOQZu20mZ0NsB+8ciTO8pRNLXRFJIFGE6Chluf0Sq+yvI8H0HXBV8LHoOatLW08+mhhpSkW2VFtIN"
    L"3k+KpLfeDddaggkmIaRAvRDvvRHmWhNDRm4b1fpfrbi4CJtde3ZAS2c/dPW1YGisBysbc9g72sDd0w1hEaHiscIFcbw5cZuY"
    L"lIas7DzkHyqUkyHOt/BYD0NDY2js14G5mQ0yMw7iVE0tLvZcwLVrF3FjuBuXrzThXEsJamtzceZMAQYuk5xWntA84qNdNjdS"
    L"5kN4K8KafwsUggl/HgPnEcPkn4GiUDcjpzpyQ7SR0mmS0weZvE83jhIoD68qTnbEKOly4UiocxBXuzL+CSiiUM5FjPTs+AtQ"
    L"uOScO1659oRzJ2eOGaH+qJ6oE5b2tXmKHIGiloLCHVInbJrEpyu8uWeHwx5RKbQVzYALFL6xCXMFGP+6FTDhI2auW1ko+ZGT"
    L"6cskicjAYhVUn6fos+Hq3HPFhminUIx/TlZW3HfEjXbDfQdx//pRUmsnSYFwMRr32oyc2jwayZWM1OncGyYAE4T578ZqjtXM"
    L"5Y40qWzlvh1WbazUeEAZ/y1aTpjL+A/2ImGHNK5sLSclwCENn0wdi5lIqutH6THK9mXzp7eR7vkKEpw4P/I7CW8S7X4vSVNW"
    L"IlzRyrkPzoFwLkSZG1HmR1y2EUgovPHTHAN/3dG0RyHQaJQcI6c5voRMCm8K/T/BseDvUBI5GScS2LNlg9S61BXpK8K+KgdS"
    L"JH7oP+OBgWZP9Da4oLHUEkGOy67Z6Sx2HLldVOv/yTp8+PA4PX1DbN6yDcYHTGBhZQ4TU0PY2lvBxc0RHl6uBBVfBIUEIiom"
    L"GsGkXriKNiY2kcKhVGSkZSM38yDSCTRBfv6wt7bC/n27YGOlD/8AB5yuP4LGxkI0NRWip6cS9++xclAkVh+KQxpB4yGpjXtX"
    L"6W0Kbx7Q2zJx79/bDBhWJlxjwqHQk8pEGepwvoVh0inJQ+V8Fq7i5BtD6iduVJM6qZSk452BYz8fFTNQ2N9kRKGI7wZbObZE"
    L"oq8pRBHy1HsrXokrnKQEnZOxrE5O081bX6hLMNGSfhdFYRYXsbE6GenZIYXCkp+BoujdYZWyWMIU3mVJi+TmY1cxJViUakQg"
    L"w8Dh2hH6WjYy4mIrrmnh2hZulONKXK57EQuBcitRC9z5zGEI/y4MkjvX2IuEQHK3lv7WFNo8aKbNeZInNp/a3GoQRcIg4eNf"
    L"zo9cald0/l7uIJAQXNvZoqHKjv4eDBJTVB/aJxYC3PPDvzMbUx+LoZCNK3F54HnEN2ICxSCRece+byDe6TdIclbseNvnEH7g"
    L"aQEJJ1lDDJ4RiPiQImF4uO1QAMSV1AjnS/h9AQQSPsHx1WSQjEaUzfNIcv0z0tz/LjUlR0O+w7HQCaiIm48zpBQbZXiYjhTR"
    L"nec8yRlPgcnFOgIJPea50iFOc+FgOCn+wN5pn4zcJqr1/2aVljb9xsbG7eb27erYt18bRsYHcMDMRPIkdgQWN3dHUIgEXz8P"
    L"BBJcYmIjEBoWiKTEeKRxKJScjsyUBOTnJCA50QfR0fZoOJODhsaDaDqbj8v9p3D/Dt3kPCxc7Bbv034o++E9AsVDLrLnKlja"
    L"j+7i4YPb9CF+H11l/wwUpSJRQImTsJdoc7jEMFHUnjx61E1ff0FOInhy3IM7dHNw1yurkxGgPLxa8U9A4cpP8YW9mC9AkdoT"
    L"aZuPFnXCJyCsTribmEMdzkNwIpZNfRpLTdBQTAqFgFJfoDjZ4Z4dKbPnkIcUCr9Sc16jMn0lKtJIpaQulyu7iZ1MJMgk0TVp"
    L"6T8B5Xg816hwf88C8ZZlNcJdydU5a3Eqj9TIIYIIn9jQTdJEr7jNbCFQZi5Vu9yw2NsUJmM2r/Zk49bgUYWp0T2GCIU2HAZy"
    L"eCM5ElIjokjo7dtN0ijJFgLsSnetK1dUG88WutTCnb9eaK0mZUahHs/7bT15QABalUMQyd0ix79HSUUVx0xDeRLDZKL02bC1"
    L"JPvV5vq/hYP+r8tUwhib55Di/DxibcYg1GQUQgxHSb9NOIHEX2O0QoUQODxGQMKbocIg8d6jKFjz0x6FcApt+Ag50YFPgv6G"
    L"FNc3Sfl8LaqIPVvK6G/MSpFnRCuMsa0FHP2N3rjaHCTes5w34WPhA3u/bXc5sNhp5NZQrf+dFRoak7J9225SKzugq6tPW5dC"
    L"GX2YmRsLWBwcrQUsAYGeFOa4IpCuwX5eiAsPRGK0L86dOYr6uhyUlkSjr5eLz7j2gxOjfPOz8lDChN5kYxO+Kj1L2I5RSuu5"
    L"e0cBl0eP2FpJCRSlOlGGNorwhmHCR8SPlQnthyMwYXXCFZs/qxMKeW6comipRoByjzuJB0r/XaBw/QknGnlAFydklcfFbBzE"
    L"hWysTs6VcY3CSO6k2EB6T+opJlcChY9nGSgcipzK2YBarpDNWCUg4c0GzlUpa1CZvBqVKQSa5GUElcWPocIw4bGjCkWySjxT"
    L"uH6Ei8DYh5YL5rgDl0MbBglLd1ZQHNqwxcL1i3m4c/047g5X0d/gtAIiXNjHeSVlHYmAhKDCoyhGPFv5+HfoYuHjloP+5gj5"
    L"3S+cchI10l5lLp64fGJTQbA8lbcFPD6Uk8Nc2coFacdifxIvEilEC3wXhwLfIlXyNyQ7/x6xts8jjmCSYPc8KZJRiDQbjUjj"
    L"MdJrI/kRgogSHAwTfsybAaM8ApYmPoOxiLZ8BlHWz1KY9EekE0i467o4ihRR/HyUEKBrD2/HqSJ1NJUbo/mk4vTmSrMPLp/1"
    L"xGUKcTh5nBq4EZb7v7nnbDSTcyWjFXeDav0fWeVF5X+3tXG8uX2rGtT3aEBX2wCG+kYwO2AKK0tzWFuZwYIAEx7iC39fRxQW"
    L"xKGhLg8Vx9PQ1lRKMTcDZCShyuGLtPjdoZt7mNQCQ0VhM82wYDXy6CFtUSb38IBUyb37N3H/AedNGCoKlaJQJyOhzogy+Rko"
    L"vbQVIFEWsEmowzUS9IrLQFFWbz4JFHZgY6BwXkBCnr4CgYnS0nGwXRHuDLQo6k96zwSIOuGjYs5JtPBQcz4mLjGRk53Go/qK"
    L"cGcEKGxTwMVs3LujBApvViqVI6GPEigCFQJKZcpyOQGRLYnWxRLaVLFfCsn1Gh4AxpKdICJdzCVG0jvUWe2KPvr5+Ge92pkq"
    L"oRsPx2J3NBmOxY2SbCOg3I8VyQhIbpwWRTJ8qQw3ehXHv5daU9B7Ngrna/3o9/ZAVy3/zqTEjmqKzaPCZY6L0ZZIzYs43Sew"
    L"5+04AQk37LEXCffYZHi8glQCCdd+JNq/QDD5NSJNn5ITm3BDzo9QWLOHAMKKZGRzWOO05QlVsptgw+XyOoq+myizZ6WeJMHh"
    L"DzL+oyDwMxwLn4BjUbModFwmaqm+eD/qj+ujsdoSHQ3uuHDGHQPN/hho8kJLsT7qczVgvf9TeJjPDtXd+NVbI7eAav1HrIS4"
    L"hC0GevrYsW079uxWg5amOsxMdGFitA8WZlo4VVOE6sqDqK3JxMXO43JzSwcwJ1J5aDiXvLICEcdo3krlQTBhhxMGijxmoCj2"
    L"zxC5Ix+/LyM/lepEcTT8JEx488Cnh1Jroqw34cKrn9WJnFzcbgRujRwXPwEUdmFTVshyrwnnT67Tq7IkZEeAojjdCVK8Qte6"
    L"STjBZfasTjjcYQOlx+qkgJ3YNAgq6o9DntMj/Tu1WfRKPgIVDn0kn8KnPimkVpJXCEwUYdAyKcln13h+1a/JJ4VDiqSuYI8k"
    L"EvnfkqrWkzboIKnOVbvcXzREELxziRv2jkt5vKgR+ns8fEB/jwf0t2GlqLRZvNOKh7eaSZE0yqkNqzR2R7t8PotCpFT00vfr"
    L"Ph0iFgKKmTZW0jXNqqiObtTag9w6sIzU0zxwcR2HNUejf8BhCm3Yi4TrRxgk2R5/IYD8Ro5+ecdZ/RoRB8Yi2GAUAvX4WHcs"
    L"/Ehx+O4ZKyc07gQRF4KI81YFUFipcKGaolR+jNgysilSnPULSHF6UZzpeYgZd1qzlQGfIvHfmZUb9wa1VFL4d4Z+/jMu6CSI"
    L"MFC4qvlYyl74mY6Dg8bX8brbP/985CmvWv/Rq6mp9DdhoV7hOlpbCSorEBZii+qqVNTWpqCpKRv3+HRA6j34Rh9RJaQ4FGEM"
    L"vckc4Ss9EBUiUBmxmCZ1wvvew2GCwjC9j8Mb3jfps5U1J7z/OQkrMGHbAtn9ki9RwISvPARKkYh9dLeF+NT8M1Bu1svNIzC5"
    L"VjFiOF0q+84TCoWBwjJfene4ZPxsGHoaAuRkh08zWJ0wUB4Xso0oFHFkO6rz2JWNT1q4f4dPXerztipuRFIqigQtNweul5MZ"
    L"DoEUm8DCVgbpXBpPb+eSmjm0FdWHd0qfDYdVZymsYWd3ZXk89xZx4yIf4fLvxEfiihMbzosoQPLgHkFEYEKbQPLoZpPCpoEt"
    L"Gq6w2XY+Bs5TWNOWIP6vnacD0V7Dp1iuOF9lh4YiPbEQUCSX16Gaj72T5qM0lsKamIkojvwWR8I+oxubLRZ53vEbBJK/ialR"
    L"gs3zsuMtfoUoE0XnLysSAQlBQhnSeGwdCzeGCG2lIuG+GwaJpzqpEoJPtMXzYtXIZfcMEp63fDT8C5yMn4njcfMFwI3FauLb"
    L"0kxKitsI2FricpM3uihU66AwjUePuhp+CyuNbxNMtnw3Y+Rprlr/metGX+ufzzWV9Zwsy8LpukMYvEKv8qwEpIKVwwy+0ZX1"
    L"Ik/kSEb2vbuED+7RoceKd7HvGjuw3aP/sUJhmNyg912n9ykqXx89BgqrkieTsP8roPDPwzfPv6iTOwyU05KM5cItLmZjhzAG"
    L"ioQ8nJS9VChAYYXCrflKB3v2ipVCrdOK2hPOnSiBIsbTMhbjgIQeCr9YXQEK96gwUNhQqeHQLkmcslJhqIjlI0FFuSXJyqc1"
    L"OWvFNZ5Bwj4kdUd2Sux/+pgOGkpNcK7CTm70rjMKd7Sh7iz6uUmRDPF4CQrn2GaRQxgxkiLVdr/zZ5jc7cCDm+fwcOgMAVXh"
    L"R3KruxCDbRnoa4lFb0sYes8F43ydO5pPWktNBjc3CkhytorCYuDJqROBhC0E2IuEx5AySNjQiEHCpkapTi+JfQAbGsUzAMwp"
    L"vDF+RiDypBcJw4RPahgkblvHKK6kSiQRu5c+T4tBQqGN1QsIM/sVEhxfkpObDK+3ZAYRnxhVps6R+htWTq0n9OhvpYULdbak"
    L"rlxw8bSHeNR0Vzug44QFQm0nwXLPhw2W6t/vG3lqq9Z/9Gpvb385JTk5ICkxvjYzI6UuMSG6PTExEidOFGGgv1ORA3lwm56k"
    L"I+BgxzTCAp/K3LtLSuM+50X4fbSIHjzITzyjR7YYJRFZHrBCIQDdI3jce3QF92k/FDCxyhnZDwfp+/HpzQhEntwEE2Uy9hE3"
    L"/QngaD+uO/l3wp2Ryljl6c59drcXe0dFQpbn6yiSkDxcOwH952Kk8pONp3kkBqsCJVDE94RuPK65YFc2aQakG/DxJMCjmqRS"
    L"CAYCFUVyVqwLuJ+HXull/g6/4udsFLd7sQ5gV7SCHag9QmHF0X04U6KL08eNxIukrdYdXWdD5OdiC4E7lwuJtycJlqRI+JhX"
    L"mRPhkEa6qkk1MkhIpTy43Sz9NmJrSWHeze58DLam4sq5OAy2RNHv6I/WGjtJWLZXcRinL8nWqixFI2NN1ioBybHoaTjBJf/c"
    L"tBf4hVgIHAr8CHmkSjLdXpZTFvYgSbL9PZJtXkSM6a+lwIx3sNYY+HNBGnf9coJ15MSGQxsOcfhtfj/Xk3jT5/mzJaPRMwg1"
    L"ew7xTn9GtO2fkOb1jsJDN3wcypPnoi57Ff1NN+JMqTqaq03RQP8t2Dmv97Qv+utIwdXYooOUSoTdOFipvdPjojXebeRprlr/"
    L"0SslJS/I3t4Tbu4+SEhMRnRsFOLio5B/KBvHywqRmBQDfz9P2l6Ij43DyRMVGBygm14hOf5503rwQDEbh9+8TfxhmBBLZN8l"
    L"4Ch87DnUYRXyM0S4hP7hIwLJQwYHg4Qh829hwsrk59OdkdzJE0BRtNHTK/Y/5U6q8WhIUR3LQFGqE4aJlNt35yom/5E6YRMh"
    L"BVDCxcFdqVD4uFhppNR+UuF9wlBhv1MZ4PVE6MMqRaAiyVkKW/J2CFCkNoVL8dkGMm87gYQTrRQaHdmL+iL6GoLS2TJTNJNk"
    L"b61xIZAEoa8tDtd7D0pFr8xTvkO/j/TZcA0Jw4QL0fh3p7+BbAr7KOQTkNw5g2EK666cP4ir7em40ZEsfiT9pwPkqLSt0gpt"
    L"VexHwqc2O+TUpi6ff741kmzlZr2yhKkCEvEi8fsQBcGfgGclp7r8VYrQkh1o27+IOMvfSFgjJs86TyFC/1lFjoQUh/dIeCNh"
    L"DedJRnIlSpDwEbC/1lgE6bOf6/OIsf0DEpz+ghQPCqOCPsfRuClgqwcuEGwq3I32Eh1SUfoEWwJ7nQO6Gii0IZBcOOmE7koH"
    L"pPssgOXON+Gk85XvyNNctf4zlqdXeEtCSh76Bm6KaVJLRzsuXu5Ba2cTzrbVorvvHH2sDbfuXSZdcR39l8+jorKEgBMJBwcH"
    L"+HoFIzE2GxXlDRi4xOqF7u3HYFHO+LtF774hX89XVicPKNy582gY9x8yRH7eFNzT1zNIuOiNrux5Ir4nBBLeI/4mP5/udClU"
    L"iiRj+RTjCaDcapBXZk7E4kYNqZRKBVCu/hzqMEzEqqAzS3InPP3vSaBwroLrT3rqFL6xSjMlBoqoFHplfzwR8J9yKQpjJTbn"
    L"4bnFvBkeXPDGV36bP8ZDv06zV2upkeRImivt0VHnie6m0McgGR4oxD1WJOzM/4BzJKxKRpQJA0W57ysc0njSHs+04TEgVwmU"
    L"g53pCqvFFgIk/R5t5QqF1VVNP3+ZPqoJbCez1svEPw7BuLmQe4V4yl5J1DgJbXhM5+HAD0iZvIVkp5cRb/dHpLj8mYDyO0Sb"
    L"/0rh1coeIwbPypGu/74x8BipIeFkq6gSggif3DBIlMfACluBUWKGFHngOURbvYgkp78ig82TCCSHIkiRpC+SBDXPMOIK4FYK"
    L"Ac+fsCYgknI7QyEpqbfztVwXZIyTmdtgs+cNuOr8cGz/qg+/G3maq9Z/xrK1dT3b0jmA/ut8e0M8TfzCIpCaexDX790Ej67o"
    L"uNiOivoTOHQsF4dL8lBRW472rlZcu3kV9x7QZ1CEM9A/hIqTdYgIj4eDvSvc3byRmpqMkxWlGBjsws273F/DRWkjgzEeMmCU"
    L"k//4X+b8i/I0R2FLwCHPw3sEj38Cys/1JgwT5ekOA0WSsQyUEbcwLtCSknG6uTDMMOHq2JPEKwp12EhJ8iZK35NsRWUsA6U9"
    L"UdzJOCGrtHrkDmOW0t2nFJP8JexRAuVxLuVJoOhKfQgnZ+XEZwQq3CzIm99m9cJ+KVxl21RmjdYKR3FH46mD/O9zgyK3A7Bf"
    L"izTscfgmlaz0O0pp/MgRMHvijiSfudaE+3JuECyvX6TQpjMTVztTBIxs4cit+F30at5eaSbQq87bKYpE0QlNIElcMFJHMl0S"
    L"rjzfhxUJj6PI82P39z/LPJskpz8gzu4FcTrjI9wIk2cRZfxrKZHn/AjDgiHCRWmiRjaPguNGxWagcO7EV32swlJAbzR4sFas"
    L"xQukctgx7XVpEmTXOe5r4mI+rr3hrm3OU3WSOrxQ5SRub9eawwkqvvR3t0NFjhYctT+BtfoHJ9QWv/zxyFNctf6zVmxsknNK"
    L"ei5u3Ac8QxMxbf46zFuxBVPnr8acZZuwXd0Im9S0YWrrAa/gGBwprUHfVZ6QQxH6lRuoP9uGqlN1KD5WgLq6E+jubqbwhsMU"
    L"Uh8PhtDV1SJACQsPgqenu5gzRUdH48SJE7h4sQ937/KJD90T929LDkac6x8QVO4P0SaVIjYFrFboeyqB8khZvNYlMPlXoHAx"
    L"22OgcFeshDoj4c61SsmbPJmE5VMdpZGSHBVLdWzSvwEK+54yUHpIOSjMqP8tULha9swxA3GG534erhU5XagAigz14rGj7IpG"
    L"YQ0PSW+mEIPrOjgvw5MGextDpW+Ik62cI7k/dFIBEh7VKdWsDBBSYuzUz+ENj6Kg99+/eRr3+eiXfr+bFMaxsdFgV44oLZ4j"
    L"3HmGwrXTFK7VOoAHtXPZee2RPajI24by7PUjw7EWojiW+4fmSTt/XtB3dFN/KjDhEnlu2ONTG64libN/HmHmFJpwZavpaBlH"
    L"4a+tSLSKIxrnSbiOhFSI4waCySaCCIOEoMJHwfzxAI2nEKL/KwIR2wk8J+ESDyhnJ7bCsG+kVL8ifTGFiOsJ0PsIJHpoO2FK"
    L"/w1c0Vvvgf4GX9r+pAJN0JCjhkCDT2G/++Pruqs+1Bl5eqvWf/aysXHB2ZaLSC+owPg5a7BN0xLGtv6Yu0INb306BS+9/jW+"
    L"GLcI46evwYIVe7B+uwHU6HO0DB1gYuUFz8A4pOceweXrV3D74Q3cIohc6G1DzZkKnDpdgfbzLei71Is79+7i7j0Kcm7fRzeF"
    L"VMeOlYqdJHvWOjjaICExGiWlBQSZdgIRqxNO+BJYeA4xqxVpAhyBioQ7SqBcIIVDNxZDhYAiMOFw5zFQGhT1GFxuP6QYdK7M"
    L"mShgcugxUNhMSYrZLvAALwZKrFSH8pFx75lAKWp7rFCqXUeAwgPQbR6HPEqgMEzYC4UbBJUA4SSthEElejhXZoT2Cgt01lDc"
    L"z8nDs+Eyr5cVyb0rxQQ/nvl7mn4PUh3cqHePIcl5ET69od91ZK4NJ1vv3+TfsQZ3rpaCrRbZIU1UVkuc4nSqzktK5Pln5NCK"
    L"FUklvdpzEvhE5kocS5ovbm7su3I0cpI46h/0+0QxG9n3PWR6vCqFY2wjwH4kURbPIJxUCfuwsicJN+LxGAouOmPVwYlWRwKI"
    L"AwOEtsCENoc4/HEObzgcijT5DWLMXkSMxe9lQHmOz9symIvrWSpS56Mmaw3OHCZFUqwtg73OV9ngUqObFKRxIR9XtzYUWSHA"
    L"Yiqsd7x+xWbnu7YjT2vV+v9i5eSXzAuKTJYgY8bSbTB1iYS6iQdeeuM7fPTdfExfvFP27OV7MG7mWnz6w3x88PUsfDVhEX6a"
    L"ugKTZq3FkrV7sVPbAnsMraFh6gB9G1c4+IUjo7BMXO9vPlIEM71Xr6OjpwdN7a3o6G7H4PU+3L53FXfvDxJsBsRY6fjJQwgN"
    L"94KZhR78/N2Rlp6IqsoydF5oxcOHN/GIVQuGCBqXaLNK4Z4dVid8wsNXggzJfzb94cpQNp9+MFwreQeuGOUb7t7VY4qu4v4C"
    L"yZvcohuQC8E43OHuWR7kzQ5trE64i5ZNlZQ5FK5DYagonNoU83dkZky5tZyONJYZ43QJwaSYB6ErgMIKhXtHeJ8t1RU1wglQ"
    L"ViTc+XvpXAwG2nnmbyGGBxSjOiWsUdoHiBIZ2Y+n7FHIw6c2ww30+9SCrRav9eQSjDJwrStVvFs4iSxzlcvtpNP4TIEW6nJ3"
    L"y6lSbQ7PAVohyU2F5wrPS/4OR8J4xrFiVCebPmd6/kXUCJfG/1zVyr02oxGs/xT8tEfL0S5bCLAa4fCGwxpRIgKS0aJIGC7S"
    L"d0MgCSD4hBmPkpEW8VYvItP5DRzy+wwHA77AidTZMtxM+pMoHOSwsfW4Bc5X2uNiHVcBu9Hf3RZd1ZZoPmaCUJupsNjx2T1z"
    L"tR9VCdf/CissNj06MfMIyurPY52aKczdYvHyu+OxcIMuFq/XxvpdZtA284GhTRAsXCNg7hIOfUtfbN1nifmr9mLC7PX4bsoy"
    L"fD15Gb4lBTNu3lbMXaeBtXvNsV3HDrv17aF5wAUmtp6w9whEUna+jN5glzauQbk6PIiL/Z3o7GlFN117B3pwh0Ierku5fmMQ"
    L"9WdqkZufjaBgX5iZG8HXzw2ZmfE4XpaHjvOncO++4jj5wX2ly30vvWJfwP3bnKhsk+7iOxTm3Ll+gkIBUiWDR3F7oJCAUkQq"
    L"4KiEFKxQ+BWd605u9udKyMPJWL4xr7THSbKPVQqX3TNQZMRorZuU3jNQuPT+bKk5Go4bof6YLuroJhCg0OYCqsYSfTQdNyTg"
    L"GBNIzCjksJPQ4+LZUDlJGqbQhMd13Bmi3+fmWTnqVuRHCJJc2cqbR4Jwnc3dNoJjI4VBp/HwxikpYhvuO0aKihQJK6qWcGkG"
    L"FFuFGsUA8aajBqjPUxeDpVMEk8r05YrjX64hiZkspkY8UZCrWrlZL8fn70h3fwkpTr8jNfIrxFo9jRjLpwgAnCcZJX02MoZi"
    L"/1iCyWi4kuJwZpBsUYDDnsIbu3X0eD0plc1jJBnLMOHaE7Zp5ApZVjmch8nyfEPc0o5GjZeE69GkRThdxLUkPHbEWAysL9a5"
    L"C0wu1vMIUltRdYm+i2G+52OYq32ZOvJUVq3/CsvVO6Su+OQZeIQkQU3HEdqWAZgwfxvmrNbA/HXa2KJpr4CJSxSs3GLg4JME"
    L"J99E2Z4hGQiIzoNHcDosXSKgZeqN5duMMWnhDkxduBML1mhh5WYjrCdQ7dhnAzP6PvqmHtAxdIY+Q8bMFY4uwUjLKMSZxi6p"
    L"TblPaubWPZCCuYTm9gto7mjB5Wv9cjp079E1XLzUippTJThUkIaAIHeYWxjC08sFWZmpKDtWhK52kv/Dl/DoTh/u3zpPMCGg"
    L"DNXQjVqJmwSTW9doDxaREmCLgsOy2XyZQwwGyVBPOm5dzMJQZ5oUtA00x2OwLQ5XWqMxQEBh164eLpSqY4MirkNxoNDFWsyL"
    L"uYKVj3oZJA0lxopy/OPmklthJzduke9p9JJpgzwL+UZvjniwSNcz50f4pIaTrDI8nEFCQLk3skmVcIm8DCKjMOjedQZJIUEk"
    L"i5RUuiSQeeiW0lKBQ7AzRw1Rm78PtTxAnFVJ9loCyXxxRmMH+aNRP0nSM8v7/ccjO7nzN8H2RSlpT7Tl2TSkSiisCdQmVTJi"
    L"bOTHORLOj+wcDZetpEA2jYEDgcOOlAhvhy1j4bT1KbjveEpK6UM0n0KE3lhEGT2DWNMXkGT3FwLJezKcrChuPMrSpqM8ZwFO"
    L"HdmM5nItOW1qLTuA/lMuuEQg6a+xQ0+NJSkVXeRGLIfRzndgrfFT/s7lH3448jRWrf8qy8bZt6XuXLeoiH1G7gIUhsncNZpY"
    L"tFEPe4w8YGAdiAMOYQIVG484OPslCUR8I3IQGJOPoNhDssMSC5F0sAKZR+qQkF1BwMmCsWUwdmk4YtNOC2xRs4Smvgd0jb3o"
    L"6oYDFoFw9UqAnXMkNHSdsF/bGRY2oXB1j0F0Yh5aOi5BUaBPkHlwBz2kXjq629By/hwGb5AqIYVz9+EDGeVRXnFSTLTDg/zg"
    L"bG8GHw8bpCUHoKw4CR3ninB7qB43rlRiaKAcNy6XSWhxix5zN+1gdzEutedJ6HO1I1Xm7lzhI+OzMbjKLf/t8bhwyl+6jOWU"
    L"p8GLVAo3BtqPlN0rDKm5BoWPK2UWD72fu5DZuPr8KW9JiHIV6sCFBNzozx7xIqlUdP7yyQwnWp8cIC4wIbjcaZexnQ9uKo6+"
    L"7wxWUFijSLaKeXZnCnqbItBWwz+Th/y7dUcMUHNIU/qJ2BSb+4SK4+aIjQDXkXD3L5s+5/i9R1cKb3zfQbLLX+T4N9nhpZEh"
    L"Wb9BqCGP6CQ1wjDRHSvJ1icL0jgvYk8qxH79aNhvHAtLUia2W0bDcedYuO9+Wk5vuJAt0vBpxJn+Cml2LyHD5TXken9OodVE"
    L"6Uguz1qBqoL1OFW8Qxr4WsqN0Vtni756B3RWmOMyPWZv19LEjbDT+BCuBuOadix8U+VN8l91EVA66pt7sVPbCmr6pBzswvHj"
    L"nK1YtMkQa3dbQMvCH4b2YRIKOfomy3b2T4V7cKYoFK/QTPhH5SI04TBiUouQnFOK9PxyZB+pQXFFC063DKKl6xaFOTdR3XAJ"
    L"2QV18AxMg4l1AKkULxhZeOOAtR9MbfxhZusLB/dw+AYnw94tDLv3W2KvpjUs7QLh7BmJqIRcnD5LSuahAjLXb93FxYEBdPZd"
    L"RFtnC4ZvX8S9u90YvtGB7q5a1NYUIDsjEsH+zvBysUB8pDfyMqNRW56HC83luNJDkLncSPsM3eQ16GvOxXBPAa5R+MMnI0Od"
    L"BJeOeMmfsFMbV8p2sUtbrQ+BwhXsgH6unGL5cu6rYcd4M2meY9tDHpTOn3exIUzCp4Hz6bjefwjDFGrdHa7AI/YikRwJgUQZ"
    L"1vBIEPbRvUePJU9CMBk+J+ENdwBf6y6Ufhs+/WHDbJ7720phF3uSdNQ4oL6QlFGRopCOC+XKUleJvyzXkfBwLG7aY1OjHL93"
    L"KMR5T/IkSS6KURRJTn/Cz+5oY6UBL1TvaQLJM1KQpqwfcSUF4jKSH3FgmKzl0GYMnDc/DaftT8Fl9xh4UmjDPTchxqMQbf60"
    L"nNxkOP8VhwM+RVHYjzgeO1vK5Nljt5ag11hCAC63x5VmgvZpd3TX2KC71hxd1fqozNkIe823YKP+WafxlvG/7MFa/x2Wg1tA"
    L"Z+3ZbuzQtMIOXWfo2YZhwQZ9zFixH2t2mUPdxAs6VoGw9IiHHakJe+9EgYpLQJoolNCEI6JMQuIPIT6zEKkHi5FzpAIFpXU4"
    L"VnEWVWcuoPH8ADr6b6L/hqLChE0IeA/cAs5euIyDRyvgH5kMSydPmNg4y/G0jUugQMbGKQJeARkIjS6ArWMUtPRdoG3gBAs7"
    L"H3j4RyI2JQcV9Q2Sk3kE7ge6IfUurW0NBJV23BgaxPC1q7jcexGna2twouQo0rna19sB7s7mCA1yRG5mGE6WJKHzbD4GOjg5"
    L"W0iv/HkSSnQ3xaOrMRIXz0VLA153Qyjaq33QXOEq9SLsUsYJ1tYKS8lXiA/JKS/p/O0/xw17nJcpws0rZeLVygZPj+43g7ug"
    L"ZT6QlMf30pU2qxPuubmtAMnDoUbcHazBrf4y8SQZ6s6lnylBQhuuJeGwq6PGFjwi9NQRDZwuVJeCrxNpKxVzd5LnoyxxhoCE"
    L"bQRYiRwOJlXi/wZSXP4kdSTJzn+UUvlQYx4/MVp8RUIMeMreWMl7iCPaNjZ8fgbOpEAcOTfCEOErvc1g4f4bD1Ilvvufgb/O"
    L"UwgyHoNwi6cQa/ccUt3/JEPLC8O+kpwNWzFw0Zx0AR8zRBuP7agh9VYfhPZyZ1ysdRagsDmVk8FHMFF747aF+tcq68X/LsvO"
    L"1f9iTWMXNu81xW4ObxyiMH7+DizbdgBbtJywz9QPB5yjYO2VKOGOW1AGfCPz4B1+UBSKT3i2hD0MlJi0w0jIOoKMw8dxqJQV"
    L"ymmU1zWjob1HpgoO3LxHULmJwTt35dRHUcb2EMOkN/gxTw3kaYI1DS1Izy2Cu280zEm1GJp6wsYxFI7usTAw9YaDWwx8gtPg"
    L"FZgCS4cgCqnMoWfiDCfXCLi6RSEq+iDq6rtx+y698HNOhsA1TP92x/lOdPWcR9+lDtqt6O6tx9mWUhwrTUZ6ujfCg83g66aN"
    L"iEBTHEr3wcmiaLTUZ6G37RA6G5PRXBOK5lpSBRTCtJ/ykAY63pyz4P4erjxlNzfOubCFANsg8EhTNiviqlVWI+weJ701T+xH"
    L"D/hkit7PTvLDjbh/rR63B6oEJDd6jpJKykZ/SzJ6CWx8csNH1m0VNpLs5Y5aLtc/dWib+KrwfGS2EmBjIzZ+Zr/WQ4EfID/w"
    L"HWR7/wNpbn9GiuMfpESep+yxBQAXpHF4w0Dheb9cQ8KFaOwc70Gqw5XCGce1o+G07im5OhBMONxhxcKfx7aMnHANNRglJkk8"
    L"4iLR+U9I834DuQSyI7GTcDR5LspzVsvQLO4R6iBl103hYB+Fav0EFL62HTdDW5kRfM2/gbHau7BUn5g08jRVrf8uy8rR+woD"
    L"ZdVWXQKIA3RtwzF3rQ4WbzbCur020LQIhI13EkxdokWZKBUKhzx+kQcld8IqJTzpCGIyiiXkySqoRH5JLTjZW3G6Fadbu2SY"
    L"eufAIK7dvSMFcbcJIrcII3dxb2TflzCG2wl5pjE/JgbI514avItj5WcQl3IYFvb+EgIZW/rAyNyXtj+0DT3g4ZcBb7+D8A3I"
    L"g41DLDR13aFv4gkXz1gEhiTDLygWlacaJUTi3AvXA1+7RT9T/zm0ddejo6seF3vP4kLHKdRWF6LwcDLSkkMQHGCLAC9jRIea"
    L"Ii/dASeKPNBUE4QLZ0Kkvb+z3k9uci5848FYCi+Sw7g/WCoWCTL7RxKtPQIOgcfDPtp8KtVFoU8HqZYWPLhNiuTGKel+vtVX"
    L"QmqElNIFnm2Thr6mGHSfDgLPteHeIZ6VzEfR3FlbM+KQxgZMFansO8vDsb7HodAvUBD6ucAk2+s1KUb7eRTFC4g48LRAhCfn"
    L"cYjDPqzskKaECcNCGdY4rGFVMhouGwgo9Da/35U+zolZtmfkfh0+Rk6weoZg9YL8e/xvS89NKg9eX4vyvK0yfe/cSVOcr3aQ"
    L"EaR9pLL6q13RddwGLUeNkeQ1F4bbX4flvi+SJk0a9auRp6hq/XdaZrae16obOrF8kxa26ThBx0YR8qzYYYaN++2x34xuYA53"
    L"fFNEoTBMOG/C4Y5SnUQmHhGoxGacQHzWSUnM5h5rRGl1O+1W1DRewLnOy7jQd02mBl4jFcKgUDQJPpTbmzdDRNk4qNzc/cNX"
    L"/hiDhr9uiBTHqcZuJKQeg5NbPIzNAqFj5A0tA3fs1aKfWddJ8jLOXrEUFiXBxTtWcjL79WxhbOUOR69geARGICg6AYePn0DX"
    L"lavyfZX/Rt/goMwVau04Q6A5g6ZzJ1BZkYPjxfHITvOEt9NeFGW6oe1UrORabnSnSL7l7kCRjKLAvdOKUn9wovU8Ht6lzbkR"
    L"BgkX5D0gmPDJzR3FSIp7109JHQmP77h16RCGL+ZKpa6EWAQSNpfm4+mm4gNi3sRNhhzacHMcm1uzz+yJpFkoCP9GbmRu2GOQ"
    L"HPR5E5lu7EfyElJJlbCNQKzZr6Rpjx3keVQnA4HHc4pVACkStg7gUMaeVAhvAcrIdtmsAAmHQr4aYxHAIOGTG3aVd/6zDOIq"
    L"ifoahaHfEdhmg8242V+Wj8zPlBzAeW5VqHYTJce9N21lljhfZoziuKUw3f4SKZKPm7YueGPCyFNTtf47LnNbz+sMlGUbNQUo"
    L"enYRmLJkD1buNMcOPTdSKP4S7nDBm1dYDpwDkuHir4AKA8UvMhshMXkIj+dcylEk551Can4tsotOI7/4DI6dbEH1mW40tPSi"
    L"q3cIlwdvY+gm6YO7j3D33iM8ZFooF4Un7Jdyi2KVu/cZJQrU8JWnCvJjUTB05/PsYwbAPQLApSvA2darSM87Ds9gUifGdti+"
    L"7wB2a1tjn54D1DRssHGnCUxtQ+BMassvLFuOvQ3MveR064CtH2xcw2DpFAzPoERkF5ajtfeydBXdoX916O51DA734XRDuSiY"
    L"3s4anDgai0g/Q5TmeeLR9eN4NFROn8wnNk24T5DggjRWHqxAFL1HXC/DdTIMEq56bSMJ1iCzgjnHwn4mPCSLi+i49J67mrne"
    L"hU+Szh03l2Qre5KwBcKpgxtxInmx3LQ824a7gA+FfCcgyfd/R/xIcjxfVQzIsv299MbEmj0nXiQMEuUYCjMA7"
    L"OAM+IekIjMxBZFIBoincCY7ORUTyUVEnqQdPIreoHgUlDeDvXVV/Hg1NvWhq7UPHhSvo77+JG9cf4T7LAgICSxD2pf7XJcPS"
    L"2f6Arty0zDYIzB9+zNcnt6gL2pzsHbx7H01d/Th4rAaeIWnQNvXGpt02WLvNAmu2WmL1FjOs2HwA63ZYQJ3CJSvXaDiRkrH1"
    L"ioaGuTvUCTIWBBhHvzj6vSPgHRqH5IN5aO1sJ5hdw/WhTrQ0HcfD2xdQfTwVmfFueHS7DfduteEBH//iIu5yJetIKwAedUmh"
    L"HZfIC0gIOHcHKLQhkNy9XICbPZlyisSjOXqb/ORY+ny1vRg2cdk+9wA1FLDj22acTF0qtSQnkng2z1TkB3+LbL8vkB/4qTTt"
    L"5Xj+XdRCCt3oCda/Ey8Sdo5nC4FADUUNCdsHcFjDyVbXLYpKVgeCh93qsbBf8zQc1img4sgnOttJvfDn71Oc3PB4ilDzXyGW"
    L"O4zdXkO2/4cojPwOxxKm4WjCXBkr2lSsicZCPfSUO6OvwhuXqnxxuc4XXZU2OFeqi8r8rXAweBfGam/d0Fz3rtHIU1G1/ics"
    L"MxuPm5X1nViyfr+c8hg5RmH+ej0s22aMrVoOOOAcIQVtthQ+uAelwS0wGZ6BKfANS0dAZBYCI7IQGpOLuNRCJGQWIyb1CArL"
    L"TuPQsVqkZh/F4eJqhEQkI//wCZyqb0dz80Vc7L6Oa4P3MXztEW4N0f3GJGB4kPxg/xT2TJHF15GtdJJU7vsP79HmU517BJQ7"
    L"BJQ79D+ur1WAZYg+f5AeXB7mkn/g/GXg8LF2hMSWwNAyDOvVrLFotR7mr9LBrGX7MHHuVoyftxmLt+hAzdAF2laBclxuYBcM"
    L"LTN37NK1gIGVHTSNDOHgao/yymO4caMfgwPn0XOhEZmpkfSvXsWd2wQQ6YLmloALuHXjDP3AfKJDgLlzRlH70nuUQFKIO305"
    L"4qTPIBloDkTfWfZZsRWDJm7c48bBxqN75FSE3e5LE+dKnoStBNg79VDQF3J6ku//CVJcXpXwRpKt1i8gxvx5RJs8J1YAQZoU"
    L"nuxXeLUySMRGQGAyVvIhXNVqs4ZCHIKJ47pn4LLpabgobQX2EoS0R0m/TpjZU4i2fQHJbn9BJhtPh32LowS1E2kLUZGzBlWH"
    L"dqC18gBayy1k9Og1blUod8elSk80HjIUM2h7rXdhtve12ya733FXPANV63/UMrV2v1VRdwGL1qpjO4U8rFC4QnbjPhs5Mta3"
    L"DRKgcKjj4B1P6iQNvqROfIJTBSbhsQcRGZ+H2ORDSDt4FLmFpQgIj0VtYyuu3+KbXaEgrl2/heaW8zhSWIL4uDSkp+Wi4kQ9"
    L"zrf14dqVO7jL8cUTxGCAPCDZwVu5HhFpHj1S5FvYi1bhr6LYt+7fpP+nKIL+sau3HsiJ0tU7D3Gd3nnpxj2cv3QDXZfvoO3i"
    L"TZztGEJFw2UcPHIWfuFHYGARToCxxfTFezBlsRomzN+Cb6evwpeTl+GnuWsJsOpYr24CXStSL/pW0DCwwl5tY1g7uEhRXffF"
    L"HvRcPI+UlAiCYh9u3TyPoesN9AOzWmmR0RXsR8KhDZf73+49KE17PDDsSkuIKJKWCmuF5UGpvoCEE6481U5mHyfTDZtKqiRx"
    L"hoAkL+ATFIZ+JlYC6R7/QLLTK9IFzBWuDBLOkTBI2EKAh1/57B4tMOFaElcOX0h5MEi4KE0JE+vVHN6MgevWp+QImMMhDnE4"
    L"YRtlrvBvFZCQCiqI/JrUyFT62dibZK38rA3HdMEzbjjh2l1LyqTOBee5PqfUAM1HtRFm8z2Mtv4DZrs+jRh56qnW/8R1wMrt"
    L"7slT57FwzV6BiK1vGuat1cKyrUbYbegKHSt/uBNA7L3i4E5hQUBEtoDEPyxjBCQFiEs5grSsIiQkZ+F0YwuuD3O+g9Q9weDe"
    L"/YfSYcyckJCFQMFvcO6kr/cKyo5XIiU5E1GRicjJLkBV5Wlc6n/CAY4v/PkU9zBQWMpwTkWSuKJSFLkW/shtggfnVjixSj8C"
    L"rg7fQf/1YVwcvI7ugato674kyeGm81dwuvUKKuq6caziAoVmHcg60oSknBo4+6Vgr6EzVu/Qx7Rl2/DF5MX4aNw8fDl1OT6f"
    L"uAQzl+3ELj0H7Ddyxm5tS+zXtxRgNba2oq6+AlUVh+lHJHVy/zxucDEa20oOHsWN/jzcpj3Ynoj+pkjFfJ9GXzFQbjx+AOdO"
    L"GKOphPMku1CeSYokfY3M3+FjYPYkKQj7AQUhX+Fo+FcjIPm7lMmnu78sniRRFIZEm5EiMXpWTl2418Zv3xg5iZGiNFIcSpDY"
    L"rSWIEECsVo0oE86XbFLUnHBORYyO2CDJ+NeIs/o9wepvyPF5V2YPH42ZgLLUeTI2g0doMPzaTpqj85Q9OmsdwSM22tmqssxc"
    L"EshJPrNhsuNvfHpzaNKkUU+NPO1U63/qIqDcL6/twILVe7BH30VqTr6fuQF//WASxs3agC37rCWByeGOV1CqhDusTsLj8pGQ"
    L"ViRQSc86hrrTJP17rmLwyk1cv3YHVweHMXyD7mq608UGkkEysvjmV24GjNIikmtF6urPIiY2SSwoA4PCUHCkCBc6uyVR++TX"
    L"PbnZr/Yew4RCnOHhB7g+dBtXZd/FlWu30T9wAxcvE1T6rqOl8xIaWi+itukCTta2EFAacOR4PQ4WVpLCKkNGXjni0ovhF5UF"
    L"TwrrnOj31bDwwcJNOvhq2ip8PX01vp+9nlScJvYYuWC/iRsBxk7CrHOtDchID0Vn2wlcH6jBzSvlApOhi9nSI8RjOBgkPGSd"
    L"i+KaTljSNsOpEl1UEEiqcneILSQrkxMpK2RAFU+8OxL+o3iDcNI1x/sNcXlnkPD4CB5uFW46BpHmTyNYf4ycvEjylK0ECA6S"
    L"bCVYiCIhNWI3okZ423KBGiddt4+FB6kYPw1SJLo8fW+MhE3ppHzyvN9FYcjXKImaRAppnswRYgtLdqHjVoOOahspsGOzps5q"
    L"U1IkOrT1cCRuA0x2vg2DbR+W7lr6xqcjTzfV+p++GChlte2Yv2o3tmnYwsw1CsaOYZi3ap90EP/1nR/wqxffwjufTsTMBZtg"
    L"auMjYU5EXA4ysktwsqoFZ89dREfHZXRdGEAPKYBLvddwue8qrg/exNA1BsxNMVG6R0qFl0CAR4/SIyUUWMkoH/P/8ZUhcq65"
    L"VQaxe/v4ISg4VB7X1Tdi4MqQQOQ+qaC7xC32q2Wg3Lx1X8B049ZDDA3dE5h008/Du6OLLRL6cLq5G9Vn2lFSUY8jpVXIP3YS"
    L"2UfKkJZTjPj0I4hKPITwhEPwj8qBK0GUT7nM3ePkxGvBJj18M3M9fpq/FXNJye3Qc8FeY0/oWbpL8NVx4QyiY7xwpb8Kgz3c"
    L"xZyqKN1vCcfFM77iRdt8wkbRnVxiiFN0Y1bk78TJ3M2PQXIsjt3SZsnpDcOEQ5wsrzcFJBkeCoMjnrYXaf4Uwg6MRrARKQo9"
    L"hRcrHwEr8yTc/SthDUFEQEKKREDCx8EEGM6TyBGwuiI04ol9MRbPIMXh98jxeA0FgZ8SSCbgZNJcGZvKo0DYyb+l1BQdpKy6"
    L"Tzk97gDmHiY2P8qLXAHLfe9xLUnrunl/+XLkaaZav5RlbOV6W4CyehfUdO1h5hyGuOwTOFrZhqLyc0jOOg6vgASYWnvCNzAW"
    L"kdHpyMwsRGFhOU4cP4XKk2dQW92E+lMtaGpoR+u58zjf1k1wuYierj70XbyMK5ev4RqFHTdu3MStW3cIAhy+MDn+ef1TMvbf"
    L"Waxy2OGtpOQ44kjFeHj6IjUtC1XVdQSeDgxev4VLV4YFJv1XbuFi/xCGbj7C+c5BdPVcRxsBr7n9EmobLqC8plnUSX5xFamT"
    L"k0jPK0FydrEklyNIdQVGEcQorHMhhWLrmUigjYGhXYT0Oq3ZbSVH6z/O2YzZqzWwU9+Vth0c/AJxC/eRkpuG4ycz0XWOu5WD"
    L"MXDWn169ndFCoYD0/pSYoeGILmrzdqMyZ5PkIXiWTFnSIhkozgZHXOXKc4C5XD7T82/I8vgzkh1/K3YCfNNHmRFMjEcLSHxI"
    L"WXBRWgCBwZsUiRNBhIvR7FaPhvWKUTBdMgrGi+htAglbC7D1oisBx4tgEkhqJsrgWcSb/hbp9i8j0/k1HPH/HCURE1FGUOOB"
    L"YzxcjMdSnCkzRkuNPVrpd2ELSQZKZ6UZWo/tw8n0DXDU/wa6Oz67smf956qem1/q0ja2HS6vb8e0RRukn8fWI0pqUvbo2MDK"
    L"MRCB4amIiMlAWEQKYmLSkJSUheyswziUX4TCw6U4VlSGE6XVqCyvRU1lPeprG9F45hzOnW1Fy7l2tLdeELj09lzCpUsDuHLl"
    L"KimHYQELqxaGixwLM0T+ZdALPi8AAD1tSURBVCuSsBwu8ec8ETONrOGbt9HXfxkVldUCFidXLzi5+yIiNhl5Bcdx6kwb2kkx"
    L"MVD4uLqj6yrqCCZlVc0U7rQRTGpwuOSU9B5lHjqBxIxixCQXIDQuD/7hFPKEZMDJLwXW7vESCupahcjm+hwu/Ju2bA8mLdpJ"
    L"jw9gl5EztMwd0NjTh/wTJ+j7BKLrbJx4nnZy1/FxS1EkdUUEklx11OaoybAvBsnJ5EUojZ0h3iTsKM/hTa7f+8j0ek2hSJx/"
    L"LyBJoB1tMVbMiXjiXoDOaKkL8eDCtJ2j4bppDNw2jIHL2jGwZ5gsHwXLZaRIJOE6GvabSbXwMTB9PlfGck0K2wlwF3C2K4Ek"
    L"8EsUh41HVdI8lMUvkJ+PLQO4xL+10krsF9prncW+gTuszxYbgMetOmu+CZNtf7+3f8MHqp6bX/qydPQZPny8DgtW7sCO/aZw"
    L"84/DRjV92LkFIzgyFeHRaQgMiUVoKKmTyETExiQhOSEdaSmZyEjNwcHsfBTkH8XRwhKUHjtBqqWCVEsNaqrqUFd7Bg2nm9Dc"
    L"1Ia2FlIu3EvT1YO+vksCluvXb2B4+BaFKvcU4ZAymfIvi2HDm5nC+8nwSLk5AXzzzkMM335AYVgdfPzDYG7liL37DWBu6SpA"
    L"zM4rRXlVEymULhSW1gtQik82IjW3DIlZxxCfVkzhTgGCog7CNzQDboGp0mpg4RIDI/tI6FgGQ/2AL/aa+GCbjou0J/w0dxup"
    L"lV3Yoe+EbZoWiM48gobufrh4kRqpCsO5Ygu0FpvibJE+KnJJkeTtwqncnajiExxSJOyWxsVgJbGTUBL9vYzTzPR8A1neryPN"
    L"7RXEWD6nGLlp9WuZuBdmwIbOipJ3n71jpShN4UnyFCmTX8Fm+ViYLSSQLCWVslaRiFVO4XNVIyWzbzSC9J5HuOkfEGv7F6S6"
    L"voE8/4+kyrYwejoqs1eiImcV6o5sQ3O5jrjL8RFwX60beisdcYFDtby9OFugDn/zb6G76R84sOcrVc+NaimWhr7FnYr6NsxY"
    L"sBr79a3hF5aMiLgsBIQlwdk9CK4egfDzC4OfbzD8/UIQHBSOiLBoxETFI57CjuTENGSkZSMnKw+H8o7gyOGjKC4qxfGScpSX"
    L"VaK68tRjsJw9ew7Nza1obz+Pzs5uCV9YtVy9el1Uy+2bt3Dvzl08ePBAlMn/1fpXmHBiV65PbDZq4oRs+wUKkcpq4e0biV17"
    L"9KFraIPSiibZ3BGdkV8mdgvcgxSTXCi5Ez7JYr8XVifcbmDiGA19mzDoWIdQaOMumxsnV6lZYOrSvfh+1kas3mWOXQYuUDdy"
    L"kslC1o7mKDnogI5yG9Qd3I+Gw/ukia8scy1BhEKb6DkoS5yJ44nsmPaD+JMwRNgxLcPrb4ix+a2c3ChrStiPJFhnrJg6s8+I"
    L"9+6npHGPj3rl6Hf1GFitGCubH3MDHzvLsx0jDxzn/EqgztMIN34RcTZ/RZLT20jz/gQHQ75FUfwUlKbOxcm8Dag5qoZTxXtx"
    L"rsKQwhor9NQ54nK9G3rY4jJfCy35++lrJ8Bs619hsuP9DFXPjWr909I1tERc6kF4BkQhMiELti5+BJIAuND29AmGt08gAvxD"
    L"ERwYhgC/YAQFBCM8KAxRoZGIDotCbEQMgSUBifFJoloy0xWqheFSWFCMY0ePi2qpKK9GNSmH2prTOF1/FmcbW9DS0oaOjgvo"
    L"7qaQqLcfly9fEbhwruU2l98/ERI9uVjIcAWtEij8tlLc8JVNsPnKCdrz5/vg4uKDuQtW4r0PvsGqdXRTU8iTlX8Ch4/VI7ew"
    L"BknZx5GQWYKo5CMS7viFZcqpFvctWbvHwtghCnrWodAihbJDzx3bdd2wZre1AGXeOl18N3MDZq3SxHZtd6gb+6C59zaFXg4o"
    L"yrDGyXR1tBXr40TKahxLWYSStAVyWlISOwWHw75GboDCBDrX/y2xXYy1+xUirUiR2DxH4Q17k4yWI+Aw3V/Dn3Mku8YKSNy2"
    L"PA3n9WMlT2KzksKbVaNhtYbAwvmTzaPhtn209OfwsXGg5tOIMHgB8VavIMXpdWR7f4bcoO9xOGoqjqUuJOW0GlVHtuBstR4q"
    L"S7TQccoJfWd8ZFDWhTIrnCvYhzMHN+FI5EzokwoyV3uzc/3sP00ceQqplmopVkpaVmQshS9+wZFw9vCDqZUDHJy94OjoCSdn"
    L"D7i5esPL0w++3gHwcPMmoAQi2C8IIf7BskMDgxAeHEKKheASyWBJIrCkICUp/bFqyc8tQMGhIhQdOYYSComOl57EyRG41NWd"
    L"RmNjk4DlSdWiBMu/hkQMF6mkJXBIST49UKoT3sqaF35fZ1cvrG0csWnzDkyZMgfffjsBP46fgYlTF6DkxGmcqG5B9uFKOSaO"
    L"z6Bwh3ZE4mEExuTCOyxLkrFsd2nhHifhDudO1A/4Qc3AU/bqXVZYutVEGil/mL0J4+fvxG79AKjpeaG1/wGsHSxxKPkA6nM1"
    L"UBpHYUTaMpSkzEJB3DgcDv0WR4K/QGHoJzgU9C7S3f4mniTRlk8hwf5ZxFo/o8iRsJmz/tMI1npGTm681Z4W7xHn9aMVSddV"
    L"o2C7kpOvY+GwntQKhT52fLJDqoRNowO0n0KE8QuIs/gTEkmVZHm8SyHV1yiMnIxjXL6fuRq1R3aioVQLDSf10VRtip5zbmir"
    L"JGVV4YTWoxYU3uigLGE9rNRehqX6q/0b5j87e+Tpo1qqNWrUuXPn/hYZGZUcn5ByztnFA8amVjCztJOrlbUDnF08YW5mAxcC"
    L"iqe7D1yc3OFFYAn0CYCfpy/8vfwQ4O0vbwf5BgpYwgJDERkSgaiIWAJLHOJiEpEQlyxgSU/NQlbGQWRn5iKfVEsBhURFhSUC"
    L"lxMUElVW1OIUhURnRkIihsuFC12PVQuHRINXhjB0/RZuDN/B8E1FPQo3Ft65+1A2v82qhFVNc3MzfH39oampjQXzl2DK5Bn4"
    L"+psf8eOPU/DlNxPw/biZKCqpRfahcqk7ySmoEpjEph0VTxc+KuZkLFcGc/6Ener4dEfbKhh7TXwp3PEQlbJKzVJgwsrkp7lb"
    L"aG+Hmr4/9hoGoKXvroQ8uYl6qEjfiuKoueJPkhvxJfIjP5cqV27i44Qrl8rzQHH2b421fBbhJjzfZhRC9UdJN7APJ1x3UNiy"
    L"bQxcuZ6EFchagsZqxWawcFOf2/axcNpGn6vOR8hPIcyElI7Z84i1+b0YQReGfoGj4T/geMIsVGWsklGoXEvSevKAVLiyv0pL"
    L"mQ16qhxxrkiHlIkuiqOXwWbXazDa8ib2r33fYuQppFqqNWpUcVGRVUhQ8PnU5BTY2tpDW1sXJgcsoKdvLJsfmxibw8bKHk4O"
    L"rrC3dYKTrTPcnTzg7eYDN0dX+LkTTAgoSpjwZsWiVC1hIZEID6XQKTxGwBIbnSBgSUpIlVxLOqmWLALLwZxDOJRfiCMUEhVT"
    L"SMSqpfxE1T+pFs61tLa2SyK3p7sf/X2kWq4No6//Cq5dv4nBqzcwdOO2bH7cQF/j7e0LDw8PrF27HjNmzMKE8VPww/fjMX7C"
    L"VNk/jJuOdz/8CkfL6pFXWCUwkWQsAYVzJ8Fx+dI9zZXB3CzIPjAMFAN7BVD2mwVgm44rNu53kObJOWu0MXXpbkxYsJ32TuzQ"
    L"JeCQQmnpuwNHV3NkRavjSNRCqeNgV/n88A+RHfymHANzLUmi/W8QZ/UrxJk/ixjTZxFmMFphdKQ9Wo6BOffBysSdLQX4yFeO"
    L"ghVAYfc0BgwnXGUy34gi4XnAUeYvINHxj8jw/AdyA99BQfiXKI6dSHBbLJ3KZwv3SUl8Z4Uleuvs0XvKEd1VtugqtcbZHF1U"
    L"pW6Hi9Y7BJJXYLjtnayRp5BqqdaoUTEx8QdjY2MRHBwMMzMzbN26VV69d+/ei/37NWFsfAAGBkYwNjKD2QFLWB6wgp2lPZzt"
    L"XOBs4wxXO1e4O7jDx9UHHhQK+br5ItArULa/V4Bszq1wjoU3A4WTtgwXBosyDGLVkkhgSaDHyaRcGCw52YeQnZWPvNwjOFpU"
    L"KnApO16Bmup6US6NDc2ym1s60NJ6Hm3tnRLKdHX3YeDKdfT2DaDsRCVCKdyKjomT32fBggUYP34ipk2bgekElfETpmAcgWXK"
    L"1FkYN3EW3nrvc+QXUaiTexypOceRnFOGhIxS8cRlmIhCCcuEvU+8hDtKoLBz3W4jb2zVdqFwx1JOeDh/wic840ihjBeF4i1H"
    L"yucH78LW0RDJoRtREDUThwK/EhvEFK9XkOz+ByQ7/k4UCcMk2vwZRBqTouAmPgIC2woIRAgSYgRN4QtbCXCVK5/YyMwbAglf"
    L"eV4wJ1sZQDwjJ8byt4gwfU7sCw76vYW8kI9xPIXCm8RZqM7b+LgL+EK5DS5WO6KnwgrdFWZoO6aJloLdOJ26AaFGn8Ng9V9g"
    L"suXzjJGnkGr90tfg4ODv/PwDj+Xk5MLT0xv79u3Dhg0bsGPHDmzfvh1btmyDlpaO3ID79mlAT88A2po60NU2gL2VA8yNLWBN"
    L"YY+jlaMAxcWWoeIBf48AeDl7E1g84OvhRwolmMKfQHh7+sHLwxd+PgQa/xCBCUOF4aJULHwqxMVosRwOcZ4lOQOpKVmyM9IP"
    L"ClwOHyqSsKhwJN/CqoUB09DYjNNnmtDe0YVzze2yC4tKEEChVvGxMpiYW2Hh0hVYsHAxZs6ag5kzZ8uVgTJt+mxMnTYb4ydN"
    L"x9ffT8YnX4wToKTlliLzcIXkTaJSjyI6rRhhiQVid+Dol/g4h8Idx5qkTNi5jpOxW7Wdxclu6VYjKWobN38zvpuxFj/O3iQf"
    L"5+Pj1ivDsHXSQVzgCmQH/oAsr3cV4Y3Hi0h0eV5GU8SaPYMok7EINxyDUN3R0hHMFaueahS+jPTdcMn8v3qTSFEaK5LdiloS"
    L"9oCNNHtWTI6yPF9VzLkJ+lQsBY4nz0Jt3lrUHt6OMzxo7Jgh+hvccf6kDfpIlXD377lCTdTnbEeMzfcwXv17mG9+o2z5pJde"
    L"GHkqqdYveVVV1RgEBYeez8zKgbWNHVavXi17/fr1iIqKelwg1tx0DumpafBwc4fGvv3Yt1cdpiZm0NMxhK21A4z1TESp2Jjb"
    L"wcHaCQ6WjvB08oK9hYNcfT384UWKxZvUSoBviEDEx8v/8fb3DUJocISAhVVLWHAUoiPiERWp2DHRinqW+LgUgksqKZc0JCWm"
    L"Iy01WwrmMrPykJWdT+qjisBRiqPFZfKYr7kEnMDQCNQ3noOzuxcWLVuJGXPmY/7iZZg2ay5mz1+E+QuWyPsmTyeg0PtmzF6A"
    L"iVNmYtK0+Xjz3S+QlX8cSVlH5aiYcycMlOA4RUKWVQp7vlh7RMPMKQJGDuHQtgjCHkMv7CRgbFS3w5JNRjJmZNbKfZi0cJv0"
    L"PI2bu0nUCRe4tZB6snPRRrTPIqR4fIoUp39I302sw68RZfuUVLkySHg4ONeUKD1K3Hewe/wYKYvnilbe/Jj7bbiMXgyjGSQa"
    L"BCBSNGGkRmJtf4cUt5eQ6fWqFMNxYVx54hzUZa1H4+G9OFuki45Ka3TXO+J8jTXaq8zRUWUmPisV6duRG7QEOsv/AOudH53Z"
    L"Mu+1H0aeSqr1S14ZGZm58fGJiIiMhj6FL3PnLcDCRUuwYsUKAcn9+/elruP27dsIDQ3FtcGrPx+N0G5vbUPewVw5xVHfowEL"
    L"M2scMDAllWJL4Y8DhT4uAhVfNz+4kTpxsXeDh7MXPAko7q5eok447FGChRULvx0SFC47NCgS4SGkWMJiEBEei8iIONnRUQmk"
    L"WpJlM2DCQqMpHMoQoCSnZApUCkit5Bw8jEQKk9LSc0ildCMgJBzrNm0VeLAyWbZqLZauXCPXBUuWY96ipZizgNTK3AUCFgbK"
    L"1FnzMHHqPLz90Tf449/exTfj5mHNVh2Y2gaIMVRIfIGidycgCTbu0TK47IB9qExN3G/sJcfB27ScsVnDDqt2mkpz4NQlO/HD"
    L"jNX4Zspy/DBrPdQMPLDb2AUNvf2wddZCuOdcxDm9j3i7P8ukvFCL0Qg1G/VYlfBxMM+64aSrFJ9tHS21I0qIsPGRst/Gm9SL"
    L"n+YzFNq8gAiz3yOOvmey89+Q6f0mhTZscvQNqtPm43TOepzK2orGQ5roKLWQOhge48lNfK2Vxmgs08SJg9uQ6DcH5rvegen2"
    L"j/vUl7w5beSppFq/1DU0NPTH8PDIyry8Q3B398S2bTskfzBz5kzJkwQEBBAtgHv37qG4uBi//e1vQV8m+5lnnsGYMWPw/bff"
    L"wdLcAj1d7HfKdIEUmO3Zow6zA+Y4YGwOKwtbWJnaCFg4t+JgTWGQAwHFjUIf2u6uHvLve3n5iDLhsIePmfnKbzNkgvzDEOgX"
    L"itCQKIGGckeExxNUkkixJIt9AZf1R9PjhIQMxMSkIDY2FXFxaQKW/ktXEUyKZ/WajZg9bzEWL1stUNm8XU1gsmTFaixfswEL"
    L"lq6UvWTlWixctgpzFi7FrPmLMWPuQlIsC/DOx9/CwNwJ85dtwwdfTsWr7/6A5//8Id7+bComz16HbeqmsHAIhpVrJMwcw2XA"
    L"mbaZH7Zp2GOzug3W77LA8s2GWLROSxoop1DI8/3Ulfhm8kpRKBzyVJxrh4XNPgQ7zUC45ZuIMX8JkQd+hSDjUQgwVOQ8/Pcp"
    L"akR4cDh7lEhHMOdHONSht9ncyJ1Bsm8sAnWeRajRC1LdGmPzCpJd3kCGz4fIDfwSRyJ/QmnSTLE5qM7cioZDuug6YYu+Khf0"
    L"1bqgt4ZCsDIjnCvVxPHsNTiSuhRW2q9Cb/s/Hu5b956lPJlU65e7Ojo6VhFIWouLS2BDYc2qVWswjxQJ5w2WLVtGN2WShDV3"
    L"795FeXk5nnvuOfzmN795DJMn95hRozGarryf//Vz0NLSknwK51ZMTEwlBDIyOABbCzsKf2zhaEPhDwGFT4CcHV3g5uJOQHEj"
    L"oLjL6Qq/zWqFgcKb1QpfJSwioAQHRfzTDgmOJsjEEFxiSb3EISgoikBIH6P3hxNsjh+vRnf3AKJjErF+w1YsWboK23fswY5d"
    L"+6BGSmrNhs0ClfWbt2HV+k1Ysma97BVrN2LpqnUClvlLVsievWAZ5ixejb++8RGOHD8F9oDJPVqHqKRCmDqEYMVmbRn8/saH"
    L"P+GFP72HV17/Cp/9SMpmwTas2GIENR1n2Rt2W2LhWk0ZID953ib8MG0Fvpm0BN9NY6Bw0ZszjtacxQHzPfC3nYEgk9cQYfIH"
    L"SZr6G1B4ozNKjoK5ZJ4TqxLObBqt8CphmPCpDUHGi5SLn+YYCW0iTV9AnO1LSHJ6FRme7yIv6BsURU1CScI8GZ1Rn7dVTm44"
    L"N9JeZosegkhXhR1aS03QXmooptblGZvhYvAejHb+DVob/hFC//1V65e6SDiMOXq0pDAhIelBZma23PQLFiySo9Hvv/8RkydP"
    L"RVpahiKEwUOEhAThqafG/BuAjB49mt7/lAImpFAYJH//69+waMFCyaVoa2tjv6YGdPR0ZevrG8LU1FzUCm8TowOkOgJwtLAY"
    L"NVXVSE9Ph4ODHaytLeHr602qRBHucP0KX5U5FT9vUioEFK645RJ+3vzYX0r6QxFIEGGwhNDmnqHTp1tw/fpdREQkYNMmNaxd"
    L"txlbt+3Czt37sV1NHVsJKvyYYbJjt7pc1xFU1mzZjtVbdmL1ph1YumYT5i9fhYWkXhatXI+5S1Zh+rxl+ODzH7F5l450FPtH"
    L"ZIr/7cHiBhRXteNwSSNSssvg5pcINQ0rTJq7UZTLH1/9Gi+8/Ane+mw6vpywjFQJhVvLdmPawq0yKP6rnxbg28nL5ciYVcqR"
    L"ykaYWuyDn81M+Or/AyH6fxA4eGkQLAgU0n9DKkSSrAQRrnr12P6MgMSTPu6vPUqOj3mKX7zN7xRDxb1fx6HgT6ULmf1kqzNX"
    L"oz5/p9gJtLKdQLk1eupd0FLJs5QNcaHKEA1HdqIoZhGFWt9g3+LfQXv5m6oj4F/yIjq8n5GR1VVUVIyoqBjs3UsSe8o0Acik"
    L"SVPkypvft3HjRhga6OHdd94SUPCXCzQIIvx47Nixj8HC+5VXXsGSRYuxdfMW7FbbhV071SjU2YPde/dAQ0tTwMLHybq6+tDX"
    L"NYCDnT1qq2tw88awgOve3du4c4d9Gx/ixvBV1NRUITU5TZKwnFdhqHDYw0DxcveDtwepFZ8geHkSdLwoJCKo+PqEUKgURDAK"
    L"FZgwSAauDsvjtRu2Y+3ardi1VxsaGgZQ19DHbnUd7NmnS+pECzv3amHHHk3a+7Fp+x5s3a2BjWr7CSi7sGqzGtZs3S1gWbFx"
    L"G8FlC+YtX4f5KzZSmPMTgWc/wuJzEZ1S+LirmHMn7EgXGpuPtEM1KDjRgsKTbUjJr4VXaDaM7UJElXz64yIBzPN//hh/fuMb"
    L"vPnJRHz01Qx8Nm4hhTse2GHgjkMV52Bqrgsfyznw0HwVAdoviuGRI4GEN5seuXFYQ1epIdnxtJTUc06FT23YeS3Z7g9Id/mL"
    L"NAry+FA+tSlNmoGTGYtxKm8TmorUpZak/YQFLpAa6aqylyK15nI91B7aidL0NQi0/gJGW1+G7tpXC5b/8Ns/0H931folrra2"
    L"NoOcnJxbpaWldMN5YcmSZY8hogTKxImTaU/EhAkT5Dpt2jRMnTIJM6ZPxayZ0zFl8kR8+vEneOmPf3oME4bLn/70J/n45k0b"
    L"sHXjBmxatxb7du/Cju1b5Qh5xw41uoG15DErFiMjI5SVlQk4eJ88eQLPP/9r+l4KML322j9gZnaAwquyx13BXCLPFa95B/MJ"
    L"KIrELSsWD4IMbx/fIHjQ2y70/oTkDJxrvSBdwuFR8di4bSeWrVyH/ToG0NA2wl5NfegamEFD5wD2a5tgn5Yx1DVNsEfLBLs1"
    L"jaG23xDb9upj6x49bFXXx+Y9uli/k8KhbeoElb1YtWUPQUUNS9Ztx/KNu/Du5+OwdrsmQhPz4BuZKdvZL0G2hWsETJxCYWgT"
    L"BF0LPxjZBsPeLxWRqWXIPtqEo1XdSD1Uj6C4I5JbWbpJG19OXIS3Pp2El975BtuMnbDZwB75Fa0wszCFu/EiuO5+Hc7bnpZu"
    L"X5ddFNKoEUgIIh60efB4wP5RCNd8GuG6tEmVsFt9ttvfken6OvJ9P0VJzFQZmcFziisPbkclhS9sysSNe+2VZrh4ykZGdzaX"
    L"6OJsAX08fTUyfGdCa82fYLb782tbFryxQJ5UqvXLXENDQ3/q7u68e/36VZSUlNDNrSGhzY8/jhOgfPvt9xLmKGHCe8oUAg0B"
    L"ZOrECZg7cwbmzpmlAMuMmVg4f8HjZO2cOXMITkuweeMmrFu7GutWrcT61augtm0rtm3dTO/fAvW9mpKUZWXCQOPFxtBlZaV4"
    L"5lkKowgko8cQTOj6q189I2AZQ28zrJ5++lkKqZ7BsmUrcPz4CRQXHYOTg7PkUZwd3eBg70KqJFD6hGLjksWmkX1go+KTKWTZ"
    L"jm279pLy2A9dCq2Mza2hY2hGj81hZGoHHQML6BrS+wysoKmv2Pv0rLBXxwK7tM2wU/MAdmiYYNs+I4HKpt062LhLGxvUtAgw"
    L"mrKXb1bHe19NxOrt2mLx6BKQIC7+9l4xckxs4hQOQ8dQGR2y39QHuwzcsE3bEVs1neSEZ6e2E0GGQrXoQ8g4chqHy1uQW3pW"
    L"jpt5/MYWEwdsMrLDoco2UigH4GawAC47X6Xw5ik5xeH+GleCCE/n46PiIJ1RiDAajXiz55Fk+SKB5B9SR5Lv9zFKIsbjZOJ8"
    L"gslyVPGpzVECxnFjnK+1Q/dpJ/Q1ueB8lZWEPFwqX5WxBYeD58By+8uw2fX+8Pb5f1k68pRSrV/yamysn3DggDHS01PR28vj"
    L"GNja8A7Onz+PiIgI7N+/H5MnTsIP33+LyZMIKON/wriffhCATJkwHpPG/YRpkyZi3iwCyKzZmDFtOoFkHpYvXyk3Ol+XL6Xr"
    L"0mUS7mzZRCBZvwEb1q2nsGc3tLX0wQlfbsLjVVFRQaAYKzkZvtKP+Pj6b/cY/OEPf8KmTVvk5MfBzvFxolZCIFImXH5/Y/iu"
    L"+JeEhMdg01Y17N6vDT1jMxhw+b+FjVz1jPmxnTQsMlAMTGxgYu4EYzNHGJg6Qv+AA3QPOELb2B4aRrbYZ2AtjvQMF957dC2g"
    L"pmVKUNElgGhi7U5tAck7X0zEkg374BmaKjDhmhN282ebAh6ZoWMdJFaP6ge85Rh4s6Yj1qhZYvnWA2LcvWC9DmYsUcOEOZsw"
    L"a7katmpYQcvMU+pPttLebOyIw9XnSKHowVV/Mpx3/BHu28bCm5QJGxt5kSLhUx6eJ8wWjrEOz4tjfZbX2zgU8DkKCSTFcbNQ"
    L"nrYcNTy+88heMa/m2pHuOhu0sUNahSUaig3QdNQAp3P2ojR8GVz2vAWjDa9h54K/WdF/C9VSLcU6c6Z++2RSG1988RmFE6/J"
    L"Cc3kyZOldL6zs1PUAoceLc2NiIkOh6bGXgphpuLbb77AzKkU8kyZLHvWtKkClPlz52Hu3PlyCsRgWbx4KVYsU0Bl9co1WLNq"
    L"NTasWSvqZOPGzdilpk4QqUJtbZ2ER/QjyWYVwpsfsyoZO1aRm3nmGUWS9+WXX5avd3Fxk14hZ2dXCnN8BCoMk8P5hbh39xFu"
    L"37ovNSacZN2trgUrWyeYWdvLtrJ3hqWdi0DEwtYZtk4eMLd2IZA4wMbBExY27jC1cYOJpSuMLF1gaOEKA3MX6Jo5QZsAo2Vi"
    L"L1eGCysW3gyWnQSYjXsMsU5ND+9/PQ2L16vDIzhVcif2Xgkwd4qAsV2YVMXyPB6GyS5Dd0mwcmUsF7Ot222NRRv1ZCLArGV7"
    L"MXn+Vnw3dTU+H78IX0xYjC+mrMAWAxeBypHqZlIo2nDRHw/XXS/Cb88YBGtSeEOKhEd3Rpv9Bgk2f0Siy0tI9XkVOaEf43DU"
    L"DzgWP5vUyGqxXKwr2IPG4/pSiNZZa4muUxZoK9NDf70lGgs0cTp/P04m74Cr5icwWvs36K1+M43+O6iWav3zKq88vn7a9En4"
    L"5tsv8Nlnn+D999+VROvbb72Bl//8Jzw1dhS+/fIzBPl7obeb575QzPDgNjo7mpEUGwldDXUCykRMHv8jZs+aIVCZPXMOFsxb"
    L"iPnzF0o+Zvmy1ViyeAUWL1yGtavXYeP6TQKX9es3Slk+X7nfh8v058+fL/mZP/zhDxLWcKKXfkzZfGL09NNPiyIxMjKBlZUN"
    L"zMwsYG5uCScnFznWLiw8KklcHuLFvTz6esbYr6ErYY8LgcbWwRV2zp6wcWRYOMHG2Vv8WBxd/QQito4+sHf2k6uVgw8s7b1h"
    L"YecDMztvmNp6wcTWE0bW7jC0chOY6Jk7Q9/CBTqkYvYa2EJNxxK79Ui9GNphxRYtfPrDXMxbvRsmDsEyDdHOKwmWrrHid8IG"
    L"ShqWgdhzwEeSq6xO1u6xlkZA7t1ZutkYM1eoE0y2Y+Lczfh+2hpJ0jJUfpi1FlsNKDQydEZBVStMTUmhGEwgRfJ7hGqPQrTx"
    L"KMRYPiXzbdKd3kKWx2fIDPgSeVE/oih5Osqyl8igrFNHNNFwTA8tJw/gAoU3HbXWokrYUf58qSZOpaxGXdIq+Gq+BY3FL8Jo"
    L"84ep8sRRLdX691bpiaOzp8+YjC++/ARvv/0m3n33bXz80Qd45+3X8fprf8OHH7yNt17/G9549WV88O5reO/tf2D2jIlwdbLG"
    L"pR4CzAMe6HAPPRfakRwfQzewDlYuX4FZM2Zj4cLFmDVrjtStrF69llTKKlEqK5evwvq1GwQMa9asI5DsxLp1G6RAbufOnbL3"
    L"7t0LNTU1bNu2TXIxH374oZwKGRgYQEdHD9bWto+h4urqjvz8wxI2saVASHCE5GbMTK1gb+cMB0c32BNIHAgqdvSYlYibdyDc"
    L"fEPh5BkIV88QcYtzcPGHu3cYPH0jBCqO7sFwcAsSe0obl0BYuwbC0tkfFk5+MHf0xQE7L3nblKDDYDG08YKBtSdWbdPCZz/N"
    L"xXtfTpOjYCO7QHiEZFGYkwAjWy5eixKLAg3zAOw19cUuYy9s03PFJg2Hx0BZssUY89boSLn9lAU7MH72ZlEoDJRPfpyPr6eu"
    L"EgjtMHBFQXk7TA7ow91kMgL0/wweUJ5i+zRSXH+vOLXx/R5HQmagKGYBStJWoCJ/M2qP7sHpEgM0V9lQeGMnEwTZLU2RcNXH"
    L"mfzdqEvfiEizT2Gy6rew2PJG/foZo54fedqolmr9+6u8qmTcpMnj8O13X+LLLz7BB+8TQN58FW++8Q98/P47ePO1v2LujCkw"
    L"M9KBrYUxzE20YXFAB0Z66tixZTWmjP8OUyd8DwdbC5xvayJ1cB9379ySatisLLqJbGywdOlSSdQuXbocK1euxNLFS7COwp6l"
    L"i5dh04bNtDc+3nt378G2LVuhvmev1Krw2xr7NKGjpQvN/VrSA8Sbj5YZJqWlZWKKNDAwiMiIWOzfpw1Hgoezkzsc7PlKysTV"
    L"C470tr2TJxxcvcVYmhWJnas/HNzpZveLgn9oIvyD4+HtHw2fgBh57EVX/pirbxScvSPg6B0Oe89QAkMYrNxCYOsWChMbbwKE"
    L"HyxcgrBDyxI/zVyJj7+fJQVoi9ZrYP0uMznBMbAOFic2Y4doGNlHEUwCoWERhF0HfLFN1w0b9jtgNYU5y3eYC0wWbjQQdcJ7"
    L"4oLt+Hb6Gnw1cTl97wX49KcFpFA2YpeeL9R0fQQopmb68LaegQibt0cGZf0dB4M+xGH2JIlZhMqkDajJ3on6Am2cPWaG5nJb"
    L"tFfbo+esOy6edsX5k1akSIzRengfTkYuRbbLJBis+C3Mtr0zsHjyi5+NPF1US7X+16uururL73/4Gl99/Rk++vBdvPfum3j/"
    L"vbcELCuWLoCZiT68XLkE3hS2loZwsjOlbQIHGyN6W1/eZ2mqiy0bV+Hdt/6OXz07BhPG/wBXFwecPdsg+Rfe3d2dOHgwG/b2"
    L"tli/bg1WLFsuUFm7eiSvsm69JG1579i2Xbbajp1Ss8Ig4e7kfXv3y9XKwlqK3fi4uKenF4mJyVJp6+8XLB3G4WHR4kXrQyrE"
    L"yztAjoyd3bxh7+Ilm6HCQHH2ChFguPlEwN03EgEhCQISHt8hjwkyvgyWoHi4+8fAhT7XiT7X3jsStp7hAhRHr0homTqLi/+H"
    L"387A1xMXY8birZi7Yjfmr96PNTtMoWFCCsYyGGYu8RTqhEHLPEiAsseEYGLggU2aTlinbqcAyk6zEasCbcxetR9Tl+zC9zPX"
    L"CVBYoTBQPvh2Nr6dsgZ7dAOwW8ePgHIe5lYm8HdehHiPrwkkX+JwxFcK79aURajN3IGmQzpoLTZC+wkrdFW6kRLxFJC0V1mi"
    L"iUeSHtHGqdQdyHKdCa+978Fi/Wv3d8x7eY3iWaJaqvX/cJ08efLdKVMnCFA+/OAdfP7ZR9i+bRO9ulvD28OZQhtbeDjbwtvN"
    L"Ae7OVvBwsYanq5VAhfeenesovPkRE376EvNmT6IQZgmFMquwYeNqTJ4yHm++9SomTvqJbmw3nGs6g0cP7xJg7mPgci/y8w7S"
    L"Te8pNSoMGVYoDBa+bt+6jWCyhwCz7TFMtDS0UVhQhHt37uNy/wBCQ8NhZm4p8GDTJDZQyj14GHm5BYiKjEME7ZDQKDG9dvcK"
    L"FNtJRzcfgYqdkzdsKVyxdQ6Ek8c/gyUwIkl2UESKQIWNtH1Dk+DqFwvPoESCSSSc/eMlvJm+eKOA5NspSzBh9lqMm7kaUxdt"
    L"ld4b3qu3H8B2bRdoWwTDwDYCulbh0DDjY2JSNPpe2KLngY0ajli1ywrLCD6LtxqLMxtbFXC4w3OPx8/eSN9/FT7/aSE++X6e"
    L"7O8mr8F+gwCoaXrieN1l6BobItBrI9LDFqIwYR6KUxag8tAWnMjZiq4yS3Qft0ZPuSUuVlKoWsN9N84S2pw7qoHa9E0oi1wB"
    L"551vwGHXx1Bb/FbsyNNDtVTr/90qrSn96/gJP+DzLz6GttY+uDjbw8/XEyHB/gjy80agnyeC/bzg7+UCTxc7gomNqBQjvT1Y"
    L"OHcSZs/8EYsWTKEbfznU1bdi166N2LJ1LTZvWYP1G1Zg1erFWLFysVwnTfoer776Z0yc+B39O3ZoOntawiNWMEPXr5LqKIKX"
    L"hyf0dHRHQqDNcrSsp6OPQ3mHJdl6/eqQVMhyVW1ISBgO5uaLZyzbOx4rLhPTJN5soJSdcwipadmIjktFcFgsfALD4e4TpAAL"
    L"KRROutpRqGLnEiBw4XwJhzku3qFw8w0XmLAbv09IIgIj0+ARmCBQcfCJwaR56yS0+WL8XIHJd1OX4sfpK6U8npXJgjUaWLhW"
    L"W45/t2g4YZ+JH0ElVECyx8gPaoY+FOoQTLRdRZ2s2GmBJdtMsGiLQp3MWq2OyYu2izr5YfpafDt1Bb4YtwgffzcXH307h0Kq"
    L"NVDXccceHQ8UV/dCw8AYvh67UJihLsVodcV7capUC22VFtK8d6nSEdfOOOLyKSv0Vpmh6cg+1OfuwMnkdQSRV2Gx5TVorXhd"
    L"ZXKkWv97q7y8/GU+5eGwp7i4CLW11RQ2RMPD3VmgEuDvhQBfD/h5u5BicZTt5mwDawsDGOjxza4GQ/09FHKwcdJeAQrDhKGy"
    L"YeNKrFqzUFTLqlULsHjBdNmrls/D6hXzKTT6Hn/8w28x7qfvSGX44WzjGdwYuibg4JL7utp6nDhejhvXh2USYGx0nORPuOM4"
    L"KytHbB1LSsvFRY2vbJBUVXmKrtUElxMoPV6h8DQ5VIy0zHzEJWciPCYJfqHR8AqMFHDYuwbC3NZTTnM4p+LqEyZO/P5hCXD0"
    L"CEJ4bCY8AigEikgVmCxeswuf/jgH30xejNc++gmfj5uHn2askj1+znpMWbgFM5fuwpwV+7BogwFWbjfDVk1n8YflMEd8YvW8"
    L"sJVgsEnTBRv2O0mos3Sb4mSH1Ql7n8xYvhtTFmwTuwLpLp60DF/+uBAffjkdH3wxDT9OWU4KxQF79OxRXHsJOiZ2iAmzwYlD"
    L"NqguMkDjSSM015qjs84Bl+p4LIUtOkp1cb5sPxoKNuIEhUJe2q/CZMNL0NrwRunI00G1VOt/b9XUHHtpwsQf5dj42rVBUQuP"
    L"Ht6X6+CVy/TKfwwxEaEIDvBGaJCvHB9zKMRQcXG0gJ3dARw4oA0dnd3Yv38n9u3bAbVdmwUqDJQNm5Zj+fK5stV2rIHm/q3Y"
    L"u3sDKY+12Lt3G+bOmY7PPv1AksCKZPBrkjcZHLgCLoG5c+uujM2wtrSRa37uIRw5XCjJWHZZO1xwVNzUSo+fRGlJuaiU46UV"
    L"KDteiROkXNgo6djxKhwtqcChouPIzi9CUmYeYlNyEBKdKgrEOzBWQGLt5CNJWs6t2LsFwDckDs4+oZKQ5fL5T76dji9+nItR"
    L"o/+I3/31Y/z13W/x5qcT8P5X0/Hl+IX4adZaCU/4mHcOhSx8SsMKZbOmk9g5Mkx2GXpJEpaBwjBZu9dWErEc6nAilucXz1yx"
    L"B1OXbBc4/ThzjTQCcrjz2XdzBCbvfTYF309eIgV2u3RtcLSmD7s0TZAY7Yr6Ui+0UUjT3eiC3nPu6Kp3xMVqJ3SfsEJzsSaK"
    L"k5cizPEzaK59Gpa73jw155tRr4w8FVRLtf73V1lZ2R+mTJmEr7/+EpcvX0ZPTw8GBgYwOMhw4aVIqnLeo7+vG0cK8hAU6As3"
    L"VwfYWJvB1sYcVpYHYGyiBx3d/divsRu792zHjp0bsZGAwltt+zrs27MF+/dsgqb6Zmjt24J9ezdh5szx+Pzz9/HGG3/BJx+/"
    L"h/nzZolamT9vDioryhESFAw7G1sCSZyEQ7zLy0h5HCsBNzByaFNZdQoVlbU4XlaB8pPVshkktaca6HGN7LKTp2SXnqzFUQJN"
    L"/tFy5BSUIi2vGAmZhxEak4aA8ERSLvHw8A+nx4orw2SnhiEmzVqO+Su34suf5uDrSYso5JiFv7/3A37zyod49YOf8M7ndJN/"
    L"OQOf/bRYakUmzN+GGcv2YfpSdVEdbDIt7vW0t+q4You2i+z1++wld7KUoMMwWbBOTxKxYlWwYDPGzVwrp0XfTliMT7+djc9o"
    L"f0QK5X0CyncTl2CnthX2GjqitKEfu7UNkBhjj+4zEeg97YvuOheFs/wJczQe1sOJpC2IcZ8M/R2vQG/7a3eWz/zttyNPAdVS"
    L"rf9zq6qq6nfTp0/FN9989RgoQ0ND6O/vl1L8vr4+9Pf24drVKxSKKKpmlSqmv+8iCg7nw9nZEUZGFAIZ6EFTS50UyjZs274R"
    L"O3dsxsqVC7Fx3XJsWr8MK5fOwvbNy7FxLb3afvQa3n7zzwSUdzF37lTMmTMF06dNxLSpE6XpkJO1hQVHBCDHS0pRVVEpm99u"
    L"OH0G9fX1qK9rxKm6BtSfPou6esVj9oNtPNuK6prT9L6zApbKmgacrDqNE1X1OF5xCkdP1KLweDXyiisEKpn5Jcg+XIKE9DzE"
    L"JGcjLDYVpjYumDR7Cdbv2A91fQvs1bOUY+G1O/XlFIcTsHyzc6jz1YQlpFJm4r2vZuOL8cvxw8zNmDh/J2au1MCybQfEaJqV"
    L"CQNls7YTNmo6YIOGPdbssRF1smiTIeav1/s51CFlMn7OWgp1lst+EigMk3c+nogfJi/DHgp5duo54OipXuiZWSMlzg69jeG4"
    L"cNIB3VXWaDluhMqsPcgNXAF79Q9htP3thyvn/Hb/yH961VKt//OLgPLcnDnsc/KtKBOGCtd0PDkAi2fVMGguXeIZwVfk865d"
    L"uyZWj2ysxIstH9va2hAdHQ09PR1s3bYRWzavx8YNq7F6xUKsXbUQWzYsxerlc7Bk3iSsWDIDy2gvWjQNs2dPxOTJP2DWTIbK"
    L"JEycMI7UR6FYRTJEqiurUH+qDmcbGnHubJM8bmxsREtzh8Ckte2CuNMzSBoaW8S28Rx9rLW1G83NnWho6sDpxjbUNbai5vQ5"
    L"nDzVhLLqBhwl1XKssh6pOUcEKLzN7Vzw1Y+TLi1eteGyppEFvIJjEBybDgevMOhbeYkq4LzFHj1HqGnbiFHSzCU7pIDtp5nr"
    L"8eOMDfhx1hb8NHurjA1l5bFW3QZbdV2keI2Bsna/HdbstcZKNUss2WIibvY8IoPVybSlapg4j77XLM6dEEymLMFX4+YTTGbh"
    L"k69n4K0Px+H9TyZhztJt0DR1o/DJAWVnr2O/gQWSo2zRVu6GlmI9nMnbhuK4JbDZ/TfY7nwP+xa96jnyn1y1VOs/bhUVFT01"
    L"d+5sAQoDg4HCQ68YJDxknKHS339ZoMIwYdXCcGGgMHiUn8MVqkqwsIMb9wCda2pEgL83NhNUli6aK2BZtWwuli2aieWLZ2H+"
    L"/KmYN2+K7NmzJ2PmjMmYM3s6Jk0cj6LCAlEibHDdeKZBNj9moLQ2K0aKNp9rlxEXHe1dMvair/cKLvYOoON8D7p7LhFk+nCh"
    L"s1eu52m3X7iIFvrcM+facKrhHMqr62QXl56EubVDwDffTHpl48Ydb3388Xcv05/mT6NG/er1Mc++sPuZF16qnrlgFXxC4pFd"
    L"cBLhcbnwDkoiVeAuiVHe6rr22LjbFEs36EjI8tWU1fhi0kqMm7cVc9fqYIWaOdYTSDZpOQpQVu22xIodZoq8yWoFTGat3Isp"
    L"i3dIcvfHmavwzZSl+Pyn+aJOPv56puw3PhiHL76fiy27D2C3viN0LLxx/RGga2iEnCR71BdaoDRxI5w0XoPxtldguPn1I4r/"
    L"0qqlWv8JixgwmitXx/80Dpf7L6GjrV2ul/r6MXCJ1Artvou98r6rVwblbeXH+f2cPFV+nN93/eo1+bzhoRt4eF8RIj18cE/C"
    L"peqqCni6OlHoswiLF82jPRcLFs7ELILJ9BmTMHPWFHDV7tSpk3GUFMqZM2dE9bDXiXIIF4Okk8BwgaDBA7kuEjgu9V3B5UtX"
    L"0UuP+Xpj6PYIAK9icPCaAG+AHndfpK/ppa/t6sT5zgv081QPebq7/99OqHv55Zf/PGrUC7RHvT9q1FM6n33+bYueoTmKSmpQ"
    L"dLxOXO09/WOha+IMDX0HaBo6i4JZq2aMJZv1MHnRTnw6YSW+JvUyYeEuTFq8R46IF67Xl7zJwrWkUFZriAk1151wn87XU1fg"
    L"0/GLpCqWzZTe+XI63v5sOoVWs/HjtPXQMPKClqEPAsIzcffhI1ia70FWggFczCZAb/vr0Nr0et5XX416WvEbqJZq/ScuJVB4"
    L"yt+J42U4nH9I3NJYITBgblwfEpB0nr8g4LhyeUDe5ivDg0HCjxks/Li356KAhj/Gjvf8sTu3bstxMG9+fP/eHRwtLoCeviZm"
    L"z5lGUJkKLrCbO48AM2sGDh/Ox7lz59DV1UWbFAePCr14Wfalfvo3aQ8QPK5cZoANEciG5To8dFNAcu3akCSWr1/nkO2S5ISu"
    L"Xr1CH7uMpqbGqwF+Ptojv/7/v+vFUaOeWfObF//SsGjpOiSm5uN89zU0tvYhJbtUKmj3Elz2GTqJkthv6oV1e61klOj05fvw"
    L"4Y9LKZxZJz06vKctUsP4eXSlkOcnUijfTl+FLycvw0c/zsebX8zAe9/OxQffUOjz01KoG3oIUDQNXTFw7S5ycpKgq7EUGtu/"
    L"g86Oz1smqU5uVOv/y8UGSNOmTMW4H3/CV198KZ6vnu4eyMrIlIRo0ZFCVJ6swPn2DunRYZhwopY3A0YJEn6bQcIf593d2SXv"
    L"58cXu3vkyh9nQN2+NUx04eNpxR68egkFR/KwX2OPdD2zQmF1ogjDOG8zKKDg6zWCB2+GyODAddpXMXTtxuN9sbtXlBL/O6KY"
    L"Bgl8tLs6z3cFBwT8Hy0n//DDb1556qnfjBs16lm/UWNfuPz+x9/A1tEX7QQXbpusbbyIuPRimDkGQ4tCpH3GrtiuZQMT+1Cs"
    L"UzPFii0GcjL09aQVBJAV+HH2BlImS/DNtNX4ZNxiAcn7383D3z+aJB3HO7UdsEffBZoHXBAal4wH9LdT27V2QHPP8qaVC75c"
    L"MvJjqZZq/X+39PX0sufNmYuffvhRbBwZKu+89Tbee+ddvPn6G1i9cpW4zbOCOXa0WI5t2TS65VzzYzWiVCh8VT7mm1n5MQYP"
    L"P2bAKPfFi924MtiPywO9uHefK2YVcGGbx8LCAgEKJ4A5n8OhC28BCqmPq4M3RlTJNdwavk2Ph+h7MrCu4vbNW49VFf/bPd1d"
    L"/TGRketHft3/sLV8+fKxb731/t6xz/zmJMPlxT+9hpVrd6LgWI3A5fpdoLqxhwBTBF1TN1Evu/QcoGflC3OXcGzeZyHzdziP"
    L"wtMBefYOD/SauGArtmlaP86ZaBxwxwFHL9ylv9XOfTyhceXykR9BtVTrv8Y6mJk50UBPv3jyxIkXGCofvPe+AOWTjz7GG6+9"
    L"jo8//Agfvv+BPF61YiUC/QNkWFduzkGBDJ+8cHjEgOEbmYGi3Kxc+MZmiDBYHqubyz+HIf39BKOR6zvvvPUYKJwI5gTw1auc"
    L"D2GVonz7OoUzNx4DhkObW8M35d9goPDPcP/2rXs5mel2I7/if/patmzNwjff/MB71Njnyik0Gvj6u0mwtvdC2wUKASn0u0qU"
    L"KavvFPOlXTq2cnrEsGDvFEu3SKgbuUDX0kdUjZGNH9QNHKF1wA2BsQdxg77ewsEIm3YuVqkS1fqvv7zcvdatWb268PNPP+t5"
    L"9+13RLG8/eZbAhmGyltvvClXhg93CAcFBApc8nPzcPJEuZzKcM5FCRNRCiPhkjIsunSpDz09FBbRlcHCHckMlX/84284cuQw"
    L"2tvbBSRcF6PcDI7h4WF5zFaVDBe+ytu3buPBvfsCliNHCtxHfpX/Mmvu3CXTXn/zXYs//OGVYx9+8jWWrN6KxMwi3Cb1wvsy"
    L"RYGZh6tg6xEFTRNX7DdyFogYWXnD2NoHLr4x6Oi/iSs3gR37NKMWr5z7wci3Vi3V+u+zSktLf2NvY6O+aP6C6vfffe86A4YV"
    L"C6uYv77yFwENvV+uBCCZXRwVESnJ3UN5+QKYM/WncaHjvCgWTtSykmC1waqDi+f4KJpBcfHiRXz88ccyfZAtKBkUN2/eoI8N"
    L"yebHPE6DH7Ox9v37d3Hr1rA8vknvyzuY4zDyY/+XXjpGRp9Omznf+vX3Pst8/b0vmr/6YTrUdS1RVtMi4dGNe4p9oe+OAIT3"
    L"7YdAfFr+0KyFq3cuWLDgNyPfSrVU67/3yklMfMVQ39Bk7qw5VR9/+OEtpXph0LBqYbjwlfc3X30t3cMxUdESGhUcOiz5FwYM"
    L"n+AwSFh1cP3LhQsX5PGrr74qQOGP3bx5U4ChhAnv27dv4saN63Ll/qN79+4gN/dg3MiP999y6RiZfzpz3oo9n349we2Lbybn"
    L"zpi9qmPFajVs3amNNRt2PVy+ekvl3AXLVs2YsZiPr1VLtf7nrpiYmDf37dvnN3HixOoPPvjg0ttvE2Bov/fee3jzzTfl8bvv"
    L"vivXH3/8UebvxMXFUVhyBEVFReJ0z0fEfLTLqkSpUDhpyzBhcLACYYgoocKAYZBkZWUUqqurvzTyo/yPWYmJiWM1jIz+oqdn"
    L"/ndtbfM/jbxbtVTrl7fCwsJe3717d8Q333xz6v3337/71lukXggovN955x3Q+/DRRx/JY5734+DggIyMDOTl5ckxMb+fQcM5"
    L"FYbHEFsa4CEePLj3WJ0UFR3p3bFjx+SRf1K1VEu1fikrICBg7vbt2/N/+umnNqVSYWiwgmE1wsDhtz/55BOBDgOnqqpKjox5"
    L"KfMkrEjy83OvaWruM7CyslIlJFVLtX7pi5sRHR0dDTZu3Hjs888/b/3www/vfvXVV6BQSZztv/zyS4FMeXm5hED37t0TdVJX"
    L"VwsrKwtfS0vLr0a+lWqplmqp1r9dpqYm+qtWLc///PNPT3/33TdXGSqcV+EELdsUEIBSDxw4sIbzCiNfolqqpVqq9X+/wsLC"
    L"fuXk5GTc3t6eRDCJ8fLy+nrkQ6qlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqql"
    L"WqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqql"
    L"WqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWqqlWv/l16hR/z+lIl3BjoIlmwAAAABJRU5ErkJg"
    L"gg==";

static const WCHAR* WMP_PLAYER_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAA4w0lEQVR42u29e7Bm11Uf+Putvc/5nvfVt98PdaslS7JkyTZ2"
    L"LBsCtrExxg4VA5GHzKScIskUNRnCBJgpCKFoaSoFSYqEKTyVKRInsQNkwAKbDMbGEPBDtjGSJev97m69Wq1+3Pf9Huecvdea"
    L"P875Xvfebr2lNnV31VXrfvf7znfO3muv9Vu/9djA9tge22N7bI/tsT22x/bYHttje2yP7bE9tsf22B7bY3tsj+2xPbbH9tge"
    L"22N7bI/tsT22x/bYHttje2yP7bE9tsf22B7bY3tsj+2xPbbH9viOHPzr8xy2+VXb+s3Hbt76uR+87tZNr599YNfm974HuPrR"
    L"qS2vse+qtdG3vuc9egupf30FwIzHbr6ZwM2v+GQ+8uyavefm9+iDt05e59abblKQY0trBGiX6gSbGUmgvEfjJqHk63vvL0kA"
    L"jh0zueVm2Ot98+Pjbb/5rQR3jn6/c9+avXO6lsiMeiyMXn+46/nmPbLPmRMgR44UIY/qWUzXvZ83mgIeQEAIgK/JbqdoGkwB"
    L"B6UZjWnibA84mD+BJKn5xDPQPXdgR2PdU+qA3PN/f3Df7eU6EzDbpJiOHTPBzcDrpSn44hf/mNxyyy0KAH//sydn3frqniik"
    L"hqK6ViMW6E07kR3RbPispkoTt8simmT5uhkMihTCPTAtPy8CQCFw04DNAYhSXUNIIbEHQArCoCAFSmGT4E4aDVAAAgpAYifA"
    L"FlQBAgTRqgm9SH1wX46EEFZLPZ3zIA1CgiQcFIl3oDgIy8kigDTxqNXq8IMbo0AIiHfIguLptYBuBBgL0zz7j//y537hn+Le"
    L"3+4DNwHXHnN4X8rf/Ptv0PvPPiYf/9BVGQAcM5ObAdx666184KabDDcDuBl48Fbw1pugr9Zme3ECcMwEt1A/+p9u/xs+bd8M"
    L"jd8DjQ0TkgYREScgxAmcT8pJq76BpnAugXMC0wiYgQB84uF9Wi1QuVMcAQcDoCAIJ4AXgSCCFqOQJgIIS+1aT1zPe9dl+XEh"
    L"ASe01LkchLEUK5JE4lhQpDCYF4BSfsCcc71SyEwqYaEXBiPzUkbK6XIEFaJGKExJysAGiRq07XSOGvd+5ZmORKVLGk2ceey+"
    L"u5n3V1vz+9uuXm93VZLTHevXPFySpt+u2/qv/Or7r7j3ktYAN930aXfrrR+Nf/eT97+j3Wr82e4dM9MMPXjivHdyTshcRApP"
    L"xDSRHiFGR7jS6tEJhHApoDWKlKjN1CWJj0LEweY3NQ+STlhIJT1GiKp5UsygEMLMQFVLUF5LKy0jMPNGoQEGWiRIi0iNVu1X"
    L"xlIli8C0XGzSzExICsyElZomAYoTAIJK31faywFwBDD4KqGAZhKU8cgUGmtZ3n5mLbdmvW5Lp0+hu3huafehI0WjPR1m67Lj"
    L"2S6aD5zrod6aQrZ8/lyx+Nx/9/UWm9NTtloAy5kW7WadjPG+9x3e+xsfugq5WXmvr6QA+Bf6xk9/+ib96Ec/7Zo1/xtvPLJr"
    L"uiFZqPmps7tnm38500yerTnkJuVCqJaCZQCiwa2vhz1L3eLaPIR9Zs45h74AAfQwM2fljjMDFA40g88UCVHtPBKmJqASIEQG"
    L"1hRSWU6SVNKhXGiCBpIwkkaalKrIQJTqvRQsUqoJZfXCBJ4svxwECdIIKxfAwPKey985UF9mJOiXC8WuugdJNhsJs+Xa+hPd"
    L"uOT7FqaaCdcy37l6VneFkM0t9lbirt0zu2gLP1yX/Okde9r96YbfdbKXHHx8KUe93sAdz525ltj7EyXgfWWHf6Gqn6R+76/d"
    L"vv/yqfpbm76wIjdX9zxHYKWfQ/pA3aCbbjAr4uziav/GaHbUOWEtdQt179dFWJA0jOFiAdXMfB7jbAxaB5CIMIJQwJU70Kr3"
    L"S2lFWKp4EKSVHgFJlOhitLJWCZhNKD0OLjN4tfygbABkBGGkGWxCZZKw6o4IGChiEWKJi0mw0MqiUoIhRtOak24iLOppUqi4"
    L"sJDH/PKZdG6u4aXZrNvxs3r3tx9+8ktvnD6QF42Z/tt25D/mVd++0F2ia8jHPvvQyr8k+cgxM3klAeMLEoBjAG4BMO3TmcSX"
    L"EAlEdI6Fdy7IcBHFNvjh7GfZLiX2pklCofWbabJQS/0KNwmLwEzZL2zG1HaJk7oIc0/2hSwGC73J0SeHr9sILNqFWYAL/2Gr"
    L"18nRMw1kavyetyIkCEpWKJZ6ERkUnSKoKTIliwgUZtDC0FczOBpTR6bCZs0nszVasRaxsFjw+N6me2eMgtQJCug+AI9cd+ut"
    L"fF1MAABY4uJwmghjuYp2IXChAKNZS8CUpU6OjixoyvGtWP6PwtQSU52BSF0I84Is9b5nGisHwVkJDIFS6Bwq84+Bg0UAakaz"
    L"UnODG1bWhpodsEqpXoRDMlNeWJB0kyBYhWX60bCcRRSM6BUGSmlBBo8rBHuFYqEfkUtAp7AoYmpk9AASIToholMYihit7ZmX"
    L"WOwme90EoHT1OFKL4yp5S2SpFXDC8F+7AAIlxMyiUyCRCmwJJJbegzMhLESVYMIsqMuDOguZIyqILgQo8AL1gui9CxKjEdHo"
    L"HShJJbYKtdENcIxHnLgfGxOCiyBoq55zKAhaIt5+UCz1I3JEdAu1CQmpvrcfDCtZhPqIfogm4xqFQBYMq5mh7mEDoHzz64IB"
    L"LqIoKZNqf+Pil7tjZI55QfdDqwlVkg5OBKmjFVG5ttarr3eylL3FtJYv+Sld4h5dZtPW4WkgARHC6FG4NvrJHLq1XZpxKmrS"
    L"CjDLvcVCEhddo6Gu1jCQUNtqMcc0xGBHX0QYJhWIDl2erDAsdSMKi8jziPoWeqMXFMu9iMiAbj+AUgHnAX6KhvUsIkSYmYuv"
    L"hhvoX+oHSz/swjv/YgJEcJMmGHzWSbmj19a6PL2y1kLn7PS+/KS7Sp/CXjuNWV1EghzOCtDC+A0BIJQeygRRUukmu2W5dTRZ"
    L"rh9tLKWHtNNv5vnyWg5qkczMxHRqJtI7U+OWQjluozY6X3YRX9pB0Y+GpV6wApEI0Rq0saVXiAC9wrDcU0RGZEHNyzi6EPSD"
    L"Ya0UAFiUS0UARo9LN+ZCXXDx9XmvZGZ0zsUUhS2sdGVpYQnT64/xrfGB9EqcwKwtgYgwOERzMAoK1qCojdjY0cVAM0jMMVWc"
    L"xMz6YzgkKfqNvbK84y31s7PfVVuyXfn60kqWnT8fktm5WJ+bDfSpAZjQCuPaYIAVuEECthJkAdAPipUsIkpEmuuASRp+SAYa"
    L"oB8RJAKF2o7KhAyu1Y+GtdwQolkvz0oBuPmSMQE0U1x0saVyrLba6ePz0Uhdsby0MvXEqfOXTa091nqP3oGreRwJMig8CiQw"
    L"S6vP2JBFdJUmGjlwHFsYQllD9DXADLXuaexbfwq7al/l6p531s7ueXd6NtuZL505neWLC76+e3dIp2eCODdEubqVIGy8/63M"
    L"Q0X7RwWiDUIA3CRZWTSs5hHRBSSF2jyVWj0XBMiDYr2IiApzGFd3l4oGIM1UWd7xZufInodudCLqLOfjJ89etnbmyf3vtNv5"
    L"FtyHVPrILUWGBmg6sfCwAZEzQJ+sFoFj9ngkFEOBEI8gKVj0sOPJL2DqzB3cdeSDteeu/B5/6vRqf/XJJ6Q2N+frO3fmvtGK"
    L"lAEk20IQMKJkuEEIXpwZLTWOWiXI3LzVBn+Pr1Lg3r+sT5tRns8z5hjaBuAEcDQ676zorjYeOn7m8r3dB6Y+IrdhDotWoM4M"
    L"dQgMVBvb8YZyjgxQLRdAx91927D4HEEDjOIRoCD4NiRfw9xDv43Wznvd3NV/r/nUzOXZsydO5Nrt1+q7doZkdrYQ560iizZB"
    L"23FB4Aav4UJ7hs+3rV6H7IyXbAIIGMSgVLpqisYnSgSwCDqSDkovRCJgAkum656ry4tzjz15/vDb7dvJjY1vmZljzn0UKJyN"
    L"e0yl+0zTkmO2CFgEYgQsABaHwaXyRyfV8Ub/TgEyAvSwJEF6/j7sXvtVtm74B/X2DdfK8Ycf73eePZXW8r40du7NJPGDgP6A"
    L"rhoKwdALGFtgudACvyAhsO8EAcCIiNHR5hYMQiYQMYgjxHn4ZiKJgyHxJZ3errv43HPn5h89fvLIDxef5Rvig5YFIWK5mGrV"
    L"7h4oYPGAJDBXA1wK83UwqQO+BkgToIAagVgAGsp/LcIsAqpbGOnB7woYoL4JFh207vi/cOUNfy9t3PBeeeDeR7rZ2XMeRWB9"
    L"z97M1+s68l9KIdioDYZCIFvP17hwyIUEwZ7Pg7oEBIBb6HvvBVR1QogHfOrgnUMCBWHGbjDL8wh6lz108mT95EN3H/5J+yTf"
    L"oA+gqyk5NpW2YfsO1TqroBwcKAL4JlBrAekUWJsG6zOArwNpGzAFQwbEAjYQDNMtJ5emMPEgHfy3P4nLrl3xyZs/0rzn7kf6"
    L"+eKC0JTcd7Dvaqmyuh8OGY5RwGFDjOBi7NjY1NmW73EX0SSXDg8wkGoR8aZpIkhSh0QMkivy5T7OLfZ07cS5bCYr1JqtGpaf"
    L"O1N7+sH7j/wCf5NX4XF0OQVxOu7BbfKyOf66lTvaYgDiMtBbLH+nB8QDaRvS2AE0d4CNHUDaKpcmZEDMKmGIo+vYuOolkLbB"
    L"+z+DAxQvb/uR+p3fur+XLS0mMLCx/2AmtVTLuxm5v3KBPcuNoN+s9O55ccAo8kK4lNdbAAh4IWqOruml3hQVBWy9wOrZHhbO"
    L"9m2lH5FZNMkLhCRxCOsLePqRBxv/GJ/CVTiOjk3BI8CMsI0LPtxCNgHiJp1IVy76QEI0wnqL0M75Ulv4OtCYg7T3gO3dQG2m"
    L"xBGhB4QcFoty90NhOrK/TNvQ+z6DA81ZH976fbW7/urefrayInCSNPbuKySt6bhcErBxXEBuRRlaZTZ0+B5ezLzKJW0CiFQI"
    L"M7CRSJqYpos5zjzTtbPnM6xphNYcJBV4g0o/cZSig/sePYW/W3wab3d3o2NtOISKWJlcfjMrdywAuAQoOfxq59pYBGfcNUTF"
    L"AnjAVSxlzGGrzyKunAKSJqS9C5w+CLR2gr4Fhj6s6EE0QGOEDbQCCCYNhNs/hSPfvzvt3fBGvf/uBzNZWXFMfGzs2hvp/SBO"
    L"WCWewFz5rYgDIeCLB3nyAlzn11UA8sqNq/sy02KtH3tnc310KfizZGSNEHr4aoksAmj6aPc9/pz7nrXP4YPuq+hqq1x8cBSI"
    L"H6TfxKL0u+ffCEun4Ip1oL8MigPoYEUXCP1SQCxO5EcMWADTwS5kGQAywEIOXXwKWHoGrM+As4fBmYNgYw5W9MGiNzIPZYgZ"
    L"FIfw9f8H1/zgLfXlwwfiqSeeiVxc8S6thWRuvqAISxkwEQhtmHJw8YwdgUK3sPCv4aZ/GQKQl2FKA3FyqYidEBZ3zNdXG7Xo"
    L"qiwgM8AEgGlgq16L9z7+9I4dK/e5j/EPLdOUMhELMECrxMGQQ1t7zb33ZiYH/gYgAhe7iM/cbvrQH9EWHgdbu4AYYEUH6K+W"
    L"iwcd7pdyD3IE9M1GOF2SUkC6S7DOArjwOLjjKDh7GdCYA4suUPRhWgmCS2C9FfD2T+DN3/Pz9eWllU7W7Uu+uFCXel1dox0M"
    L"phBhHKQoGkTUyjSXi2Cn0hzYlhwqX2Mv4EUJXjsF1nPFw2cznFkP5kQoVG/mrApklWpMlT6pFWuLCzPnTz/xlo/F30WKDBF+"
    L"GDGbiMeawYwm7/sX8Fd9PygOvtaAtHchuebDTH/oX0Ou/RGzIisXtT4HtPaAjRmA5S4vzQmHVmLk8o34AZqVbqVLYVkHeupu"
    L"6PEvwxZPAq4G1GdB3yyFRUs8kD91D6ae+KJ703VX1ooQzLJcwvJSTWJwQgJqhmhRgWCwWCahcktPj2MZRdzCK5CLxl0vAQGY"
    L"b6dY7kV084haCWeNG7OAACiUs2kMdz767Pt+wG5rX6sPaB8NCrQyDgOes3KgQx+6+3q4gzfSuh1QZJRHrxGsTyP97v+VyQf+"
    L"BTB9ANZfBX0dbO4CWjuBpFHWXNjIm+AW/OxAK5RvcCNBeOYu6ImvwToLQG0KrE0BLi0XqtZCds9ncVm6UNt7cL/0+32Nax1X"
    L"rK8lhDmRyg6pmRmDqpWCMLmIFBphxucPsOPS1QAAqpx5QKEGoVFgtDikbqIGadZ89sATi5e3lh9714fyz1tu9UEpwORTD0BA"
    L"kcHvvIr0KarcugoqExBX7WSFO/g2pH/r18CrP2yWd4EYwdos0NwF1KcxyvCzgac3qRHGf9fBL64kmjrnoSdvg566G4CAjVkw"
    L"aYBJHdpfB+69Fddfta9G5zXP+pYvLzvN+uIAcY7kiOpTKCIG6UkwOBJOHIV+U5zkFVmU11IAxqjgC16o6REfefrM93+Qt/l5"
    L"PWsBCQkdTbpupGxZofALpYuU2T4wBWvTqH3fz9C/75dhrd2wbA30DbC5G2zvAl190oXeIAATsz8QQtXKpRTY+cehJ75SaYMZ"
    L"0NfhGnPITn4L873H0iOXH0qyLLe822G2tkZVI2FOaI5SJktunCYzkqLOOxO6spqBF8knwKUtAKOksGGQgyV7T1VO1WrZw8+u"
    L"HZxfe/gt35t/Bbk1SVOYWuni2ebHthc6DZThNvaXfzdqP/xvgKs/ZFb0AAtgfQc4vQ9szIB0mxfdNgYJxl6oIo2QBNZbQzzx"
    L"NdiZh4CkWZoFcdAH/gRXHZqrp7UUWhTsryy7LM8iyEgYHU0GazsKZxBmqmZUEs4BjrSN6QGvmzfw4r9vWJhgFcExHv5VtlKE"
    L"40+dfde7+S0/qwtawLHK0AS5Od7JSh3zxYgfWWqD5hzq7/4Z+u//RVh9DshWAV8H23uB1u6SDMKIVMJG9G2TdMI4PiAF8fT9"
    L"iE/+VQk8p/aiePZBzPZOJvsO7vN5Xljs9ny+tiZF0GhaRmydmNBxGJsmgEhqHi0zRRRCHCmUyUfmd4oJYAVkCJoMN09BqjJJ"
    L"fDi12JtJlp94+43ZX6Kw0u2bBH1b2OON2+CFSMNQGyj8Fd+H2t/+deDK98H660AsShve3g3Up0r1jjHwaTZ5L2Nx/ZFCsJJH"
    L"WHwa8fhtsKIH9Q3giW/gigM7auIcTAPj6mqisUCgFEERzUDB2A5nFTOIFvtB+0E1CCEOw6Dn2Pu+Q0zAkHnh5GaeSlz26KmV"
    L"a6+zR6YPxGesYI3j7Ne49rWxid4QAXoRuHgMG7R3ofa+X4B//y9CG3OG/hqYtEoGsDkHuEaJ/I0baHYO5GjkMUatXtPSU+gt"
    L"wx7/KiTmCGdOYKdbSXfsnBONarHfd6HbcwZDNARVBI7Xmoz5RgZooezHaMVklsTm1ZBLXQC4xQNmIbr15fPX3ah3Qiya6RDr"
    L"Vdpj4POPkjzwSvhFlKGv7698D2of+XXgivea9VfLDKLGPNieB9P2UBuU2oeA2kj9jxGxHOQimAJMYHkP8YlvIpw9jmT5BA/t"
    L"35Uo1BALCd01b2Y0AAEIQREvFAxUg2aKLCiKzVBXv3M0gJXF3XBSXiSlxHPrYbrVeeqaa/IHkaNGGYNCNszuGbuGDoRiLLz4"
    L"UiExR9pA2rtY+8Av0r3350yTtlm2DiRtoDkP1GfLXIJB4l6lfYb4xMbueXBTpqX2CDn0yTugD/059u5opc1GjYAhdroeRVEm"
    L"qqlBx/OlbTInoHwPVA3FRBTERqnJqpGXvgmwwc0qTZWNuuRPL3QPXx6faE+HJQvmOND1AzU/ULNbovOXuf5baYPkjT/E2kd+"
    L"DTz6vWb9dZACNneArZ1gOgUbiGhFIrGqMhzhkwGAHDR3KAmq7NufxVT+nJ/dscMJAQu5hLznOOkCDA2bEPRukPVvA3w4ISTi"
    L"RISU7wgvYDx/TQRmCibexdXV1cvfiOPw1jMbkjJbqPRxLfBq0GDj2mD2AGsf/GW67/9Z07RtyDulWze1G2zOgxXjVwrqFrzB"
    L"ICtzoLJcirhyBu6Jr3Pnnj3OAfRmYlnmRsZjwpJAAPFCL2KyNc41eMKn3nvBoATuUtcA5YSUD+hFV3OkSba097LiCSiSMvZ+"
    L"kd0+Fh7fYH9fSUGQIbJLr/0hph/51+Dhd8J666ABbM0D7V1g2gbhJmMIowDDZtpADXLqXngrJFLMOxHmmaNFljO6sUbWkDhK"
    L"zYlzzrihxp9mZbOLmnc1J5Y4xLJsyS5FARjYqurmVJXeYJ081tPe4qH9xTMoLBk94pbsGybs7wQGeFW0QWnH3dxlrP3wv4B/"
    L"909B05Yh5GBrHpw5CEztgvl0ZIjMthZeVcDXEE/dj+kkyuJqN5KkmDpocBiLcMPK3JZ+QJ5HC4mj96DbWOhMEut57FKIunc1"
    L"IZzJa9N/6aUTQRyUTgu8R1zp2tR8ODvViGuwof3fSLlu4e4NbO2rzYtWJgFmSN78t5H+6K8BR94JhAKstSCzh+DmjoDNHWU0"
    L"0GTEV4wD2IofKM4/jR1cT7p51PMLSyGhOdEgfkM2CKskl+Vu3jWD1Zx4N1EFChMhOiH0Fzu95dQxrTmXkLJV3e3rKwD5Brem"
    L"bJKhknjElfVsfqee816LKhd0pC5t0263MXBVZQFRXn1SvOocVWqDQ6x96Jfh3vUPYb5RBpNn9oE7rwBnD4K1dhVcmrRSg3u1"
    L"/jrcwgnO75x3p84sFJ31riUwB6jjhpJzIZirxZXMOo7q6p6epjJ+7YQiZ1a7q/089hoedaG6S1MDjIU0tfTtzVG0F3R6HmtM"
    L"NCtrecwujPBs0hSQBLIuqHHQkOXVDZJW2IBmSG74W0h+6JeA/dfDNECas3A7rwB3Xwk0Ziuym5NkFQWa9ZB0zmDH/JwUIeDZ"
    L"s4uFhsDUb5UQRnOEdLOQdwP6icB7txnwiwJnOnreVMU78ZcmBhh4AWW5hEEAL1CN2mqxB6DYVAnHLYHfiBQyCKy7DMSwOVPi"
    L"VROCMWyw8wjTD/wc3HfdBIUzo4BTeyEz+wGfDmMVw4QvI2LIgd4iW826I4BOp29nltZCIhQvW+myMtVsqa9dNVPnIBsaXkLE"
    L"ZD0rsuXClp1ZYmaXoACMY9dS1YmJsyR2mtO2DpgYx33oMQ9qmLqlE1RCNTdjOQCv5RhQyRQk130Q/m/+Q1jIgaIH1KcgSWtA"
    L"3pQFpxVnYBCgs4RUohhoApWFtSzvRwupbD2vBFgo4kqhPT9uX8aG88Jz61gOQF42KLsUBaAkAgKgsKgSATiBeQuTgG6sWosV"
    L"DzpEyZV/XSZuVHCHHAnCay0EIKAR/sCbKJe/y2x9qWT/0mblkW1M6HdAfxXNREScIwBEKBb62r/YVzkIV/vIMkVBkONmkhQj"
    L"hX1F0cl0la/BPLwkDDDwAGlSZl8p4Mo03Ak1PxH/rwCUqW0R+Kv+K3yVXYEXYBbMwENvNVUzK/olW1h5BKZjOYYkbH0JorFq"
    L"L2hwIFcKLdYKLZxs1dDPBl1kdD23CwqKURmMxSVuAiqwVvU0EFY01wAs6agp2wjTjWfLjIUTzUY0rLye+TGlCWJrliBo/S4s"
    L"FIO0njFvwCafZ0PEc6Ww/GL9hUhB1Cpp7QXschJwr5I6eMnFoeXu1lH5vpCkTDRE5pBDJ0bSvCETqEoWCVFjYhAOJMteB0VQ"
    L"3WNcOWPaW4PzKS0GjOcsTWxKjWXAiFUczwwOQIxQVXvZXR0FZQVW6gV1erlkBGBTTaPmtLSVdV0bUKUZQd1IY4zv/o1GwGBW"
    L"ZvXLC6mjfvUkoHTxHvsaGHKar5WeiQ16E4whV1VoYw5waVkewDLpW1+5O4FzgpoXpGXS6asiAC+ZCSz7JomJiBKk977TlyZ0"
    L"Athwi2RM2xQXGHgFJQB8HUDgIIVYHIpHbrP4wBeFtRYQMljeR1UGPxQCA4GoYGsHurlaKAIgYvReX8nZdgRST6QebNWL8tM3"
    L"XwoaYMP6BIrUPVc7MoPIWpUCzs2U70YiaFOLT2yZNPqq7vgBC6nR+rf/PsI3fgtCX/6ptw4U/RHrNZTh0mNgrYUslrCWTmzQ"
    L"X+jFQDch4YVQgDomPqqV+neC1BOu4V4VZvDFC4CVjnDJoymFsBDUNerJ4nptVxGklnBj96gLhYPtwnb4VV//ql0MSIRTD1r2"
    L"5f8Ae/oeSq1V3lZvDdZdBWKsbmkyk4XiYY05dPt5lIq/oHi70I1v6v1gZUu81JU2PjhXnXQwth+kMgFikiovEQEYro6KQECF"
    L"mUKadaysNA+tB6nP+WIdKg58viivDWrlMIq0bTK2rw7QAwWady3/xu9YuPMPSc0pjRlY0Qd6K9DeGhDyibayZYMqlO2/ak0U"
    L"0/tt6cyK0TnAJerqtagvUHpL7qRc/MSzaj4/shVa5ggg8az6KoGXkABU3ioUCiBEdTUfs35z55mV1mVzuxfuUaWX4QpvCI4Y"
    L"xvt2j3LybFDVY6/S06oOi++Lk9+y7Ev/Hjz7uEitDZM6rLME6y2Xaj/qZNcx4+jeLIJpC8X85bb40DPRewGTVCVNVdUujpgn"
    L"bHypAWpeUKDMI5soj+GrrwhfhgAMyBsxAKyJ9kJz/tnzU1dfs+/sHchdq2zoNAaesMGNGus4OPzjkGZ6JR99eLqAIK4vWvbV"
    L"/wx94E8hJNGYgWXrsO5ymSiiccDzDAV1lBJWEYYhoLZzN5alHdfXOwo6ulpN6RMjOOxqIQIkjvBOUKaATj5T6eIRNefQdw6v"
    L"Q07oixOAtPICNjfeEqhGsjnz5NnmFYiScESc2Jb7YORTVzWT49W8ryQPMLD1APIH/8Lyr/5ncPkUWZ8qW82snIH1VmGhwKC5"
    L"wIjnGO82Pmx0B8t6kKPvsOd6rsh6PWs063DtqYLOmxZGQ1kClDqHWlkLhE0QQAHniJp3SDzhBa/LqVEvvUcQaaRYBVwsRKbt"
    L"upw8PXt9N09nmxJ6m/0eGy38IJI00f5nmI2pr9DCl7mBYeEZy7/yCeijt5E+BWtTsO4KrLMEZL0JQDJOYw1zF8ZO/Br8zV12"
    L"Pc+s9EKIAZIkljSnQpkgE4fqvebLHW4kdawt1OAJhWVWNV9H8vNlpISNtqiVjTJc3RVL/bmjJxZ3vAmu6FV5AYPyr9Fut2qH"
    L"2UR0cEOpEF+JXU/L7viM9X7nn5aLX58qm0wuPQtdPgPLetV92GT0sqoXmCxhGqC3AGntQHb4xvjUk08XSZqQ9VZIWu0w6J07"
    L"AHg1X6J452SyQ90LAIiXrgCMKXEbdApVrTZ3LEJ7z/1P7/neUe+eYfh0PA2Qm0iiidq8l0XolAg/nH7UOr/zcyj+7ON0eZf0"
    L"ddjKWejCM9DOctVzaGDUN+CFifvgWIBLoFkX6eVvwVm3M5w5cz54nyCdmSkkTRVjiU+OJcKveUHi5UXmdrjXjAp5SSbAxtC7"
    L"VmjHTE2BNEntoWf33Ljea+5tJ9kKFG6icZ4N2i0OsYFVIPDlLLwNaVwLObJv/L8W7vgDMO9S0hYs65QIP++ONJfapO7dlMLO"
    L"LSwRgRDg3/phO36+m/ezHqbm5jSd3pFXR94MP+8rdO8dkW/dr2JLXk3hLnETMH7fHEEbA4zmvI/FufX5q+8/e+h9cHlHbZia"
    L"jTGtYJu8g5fs+OnI1hdP3G3dT/20hds+SWogCOjqWejKGVjW3YzobTznHxfvJQAARQHZcdD6b3ivPvrYiTxJE/j2dJFMTQcb"
    L"o/EGrtyLse36OhmBl1EaNkBwgjgQggAaQa23/vLJwx8MhW9TbJJJ44bKYBu0idGN28Gef9ebASLQ7gq6X/y49T/9z4BzJ8mk"
    L"DuuswJaeg3VWgFBsVu2GrauWMWn6bWBZ6BG7q2i87cN8Uqfz06fPhLTeQGN+dyY+1efXmJOLbRcRAn0NyfCXgwEGZwSOelsK"
    L"1MTVpOifWDpw4yOLR3+Arr9WFkxvmH8OcwUq1b1+nqzaxL1whE9kD99m3U/9tOmdn6GjI2IBWz4NW1+AFVnFLo4LzRYLvgU9"
    L"PbQqAy0VC7AxbfHGH9dv3fdYnwSS1nSRzszng90/+KoLwRiFbPnVr+d4eSFGVspKZeyCgiiIWdL6ytPX/U+hSKfAqFuq1SEy"
    L"pgNXz4hl6wTdheHyoLiQgrh8xrr/7Vet+Mwt5MpzpCTQlTPQpWdh/U7VTXzs+zZqGmyh7sdWpwzuDrSMg3ZW0fjum3Cidll+"
    L"/PjJvNZooj6/O3O1RkTFDpeXvFDP8PF6Gl4SHsBLEoAtyjom1FZUNVFXL/rdBxf2veOexTd9lNJfNYgbJYAOFkTLFDFKAls7"
    L"x3DiDqNPK5W9YaUq185I9O/6Y+t96qeh9/8Z6RNYtg5dPAXrLE8u/AUDTpt/bKwwcNw8GQgrcnB6t+H9/9j+6u6He2ZE2p4t"
    L"6nO7MlR0+PCR4Lb8Ut1o6V+ECiDLfJtLCwMM2TGtHmvs7DxVQhKsBf73p9/8D/rZjqOQvFfm/gz4/vEiERBURfHN36vq8QUW"
    L"stJV04hBI4ji7Enr/u4/s+Lz/wbSWyZB2NKzsJVzQJFvvbgY45YGeYkb9bTZMN17mKQ6MFV0iOuraH7op/hgNts/fuLJvN5u"
    L"W2PvgZ5rNONYv6uR+3ZBmRuvMXg+IDioLCaCmp7vxTIuffOlYgI4dgnZAGJEFEC63u+fPtU8ct+59/5imVo1oH51/AIsjWdt"
    L"CvrwV9n/k98Aai0wbZYt4sRB+x3rf/2/Wv+3fxZ49BsUcdT1Jejis9Bed1PrmY1taCaPnMHw9BEb1iUQw7S1CcLPQburSN74"
    L"Pei862PhL267veOSBPW5PVl9blempmOAbjSVahd3WtSeb0kcYvV/nTzimaVMTyx2+pcGD0BeEMHKWCfcCJhLEvfkuaXnZg59"
    L"QNtv+5jMfPMTiK356tSPjVhAIUkDxRd+HXr6Ecj1H4T5xGTpWYSHvgJ77lGKOFiMwNr5iru3scUdsbaTKtY2pRiYjWr2sJH6"
    L"Hf5KIBSwWtuaH/tX+MN7n14/t7yss7v3hvaBQ114KUsGVcZs/8UX18YCi3oRFaAAnAhW+wHPLmdIxdASXmrh4JFDpROwZtRB"
    L"XyCIkXjgufUw/30/nyZnH7HmE39Jrc+MVPuYKjYSTGrQO/8bwl2fA+jIkINpA/ApVLU6BQSbQ8zjyadbxpO4gfO3zSFqjAd+"
    L"iNhds9l/8gneVezt3vntP8umZud0av/hjm9NlbSvbgB3djHaDBicrbxVzoBtAIIiQC+PCCEg9XzVesi/lKva8JQYrQ4OuoAf"
    L"qwA8BatZwL1rCRY+8m/Rn7/CmHVK9b7RXxo0Y6hNQ9IGxCdgY6os0IgBpoqNZ0rYeDePsTwDm6DxOVT33FCxNMkrDGbFIa4u"
    L"YPpH/nc+dcUHs8/96VfW02YL7b2HevXZnXmZEC1Dm68bPZuLaIBYbYxRPGSsf9DGiKEZYoyIGlH0u3KpCMBgvsS2yHwe7P3B"
    L"w0SUYdFnV7q4x/bnvR//dxrqM4Z+p+ratZmMMY3DuoKJc39snEiyUefRQcMpnVS1GLR92YrhG3cPx5sx0CMun0P7/T+Bhff/"
    L"TPHpz391NQA2vf9wr7nnsp7RT5yXOCyDmFi8zf0QdagmZfIYOjOcW8uwtNZX791QwUEFpoYYFTEaiksuGkhY2SFUn9d/NdAS"
    L"n+Cxs6vrD8xc/0T/Y5+w2JqvhMCPInIDVW4begvqoBX82DFyNllbaJsaUHCsAHWDuzcUuPHkE6n4hbNo/8BPoPfj/yr81y98"
    L"fXV5bV3nDh3tTx24vEPnh/bHqhL4yRBCaTo2o/lR1quNWZkQFc8u9nBmuYc4NoEaq9vTWB5mEV89duAlpYUPMZVszWBrxXpN"
    L"uEAGJN77B053Tjy6/8b7ip/8PYb5o2qdReNYQwaO9wzYkD5kQ998vNkUh2cEYFOreGzh8mGLljUCaERcWcLsj/wMVv7Or+af"
    L"/NzXVs4urBQ7L7+qN33winW6RActngaXHD3vKNtHdbyTjJWnoBlg4kZAEQongqX1DIvr/eromq08BoNGhYZIDZcYDzAIB5cH"
    L"RbgtmayoG3ZIKQTJvad6j97fuurO8NN/oHjTD1KXz1aNp2TUtg3jfrttpmttdIS9mV2QA8B4mHkD/1+eCumh/Q40Gub+l4/j"
    L"6ff/H/l//P++tPrc4kqx5w3XdVsHjnZQLT427HqAiGPdDg2j1nKmVmKWqkBmAh8N0H5lxmIsGypuXAw1g4aAqBEaCl5SJsBU"
    L"OTglUJ+HzrQJfEBLkzS590z/xDeznd/o/6P/spb+6C8x5rlabx0mDsQodsDnbTRlkwAOF1hw2+DmiSsnf+U80kPXYvaffwZ3"
    L"Hf5Q9xO//8WVbnD5vmvevN7cd1lXXKJjUYuxZ+EE1tissGxYTBriIDdSCVPCwGigBmWIsdzlqowRokWQ0C+cWhBTRaz+nq8t"
    L"eQD43Od+cnCq3OBnvAXha8ID2IQr4AZC7SBjSx/HHMJNXWEAa9TS9MRS/+xy1932rg/8zJsOvPF7D+pnfgXFQ18zJjWy1qx8"
    L"zFjt9M0w27BFBRIv8vugfZxGxLUluNYM5v6HX8DKu/9R/P0Hz6x/696/6M3sOZTNHDzST1rT+UbnYCKSN54RNfFVVc6kGUwj"
    L"ziz1sbrcsz0wBlVqjBLMxJQ0Kxc4xggLweWaJVm/owV8UuQtXwqAQiPROft0A4A/efIvGtX02gv42YpgfJkCMGgWzepcZwVU"
    L"LnAQpm6oBCUY1Xww0KlZwyfJah6zLzzSu+OGg29/7i3/22eunrrrD2Z6f/LvUDxxn4n3YK1ZtlZWnUwZ28DhjDpXjnI5h8JR"
    L"NZbWPIP1O3Dtacy858eh7/+f7S7u7//5n969vtSP/X3XvjWrz8zn4pNope3m5BGPCh22+iPUSi04rlxCNEIpeVA8fb6DKCka"
    L"RRbUFdQQfIiqURWq4lQVGhQaIxCjxBh8luca0U+KPPFqhhgUQcB8bbEGoJllqw1s5j6xRWB74IQNHJTwigjAWDsDUTOnOsph"
    L"Kf8vDu1KFBiFmQWL5gwKpiHEqTRNFs1KheidEzgndz7Vf+rxhjv71ht+7MjR6z98dPq+P2rmt/0u8uPfhq6dNyQ1iE/Ls0ll"
    L"LODCscLNKgtjWM5tEVrksKwHike65zI03/EhFN/1w3goPZR9/eGnO089d3d3et+h7Mju/QUSp6aERh07zjIObDtHfIJOovpB"
    L"a/iyzatRPJc7OZbWMsxMJ7CoBhSiamYaxXSQCFVqCVOFmQoNFAXVSBppUaExwNQhL3IHwKkGj80xzQ1M17j/hICyv5dt5ay9"
    L"5JQwU/MknUGosVqTKiCiVZqIiGji/ek8Cwsa0aRprZBwIM+Ljk/8oii0OkbM2mmSZAHFlx/tPXRPK336uhtuOnj5m3/swNSp"
    L"e2aSh77kioe+hvDMI4idNVh/HQYanB82fBpoZMYAaISkDUqtgdqeQ6hf8y7oFe/A0uF3xHuLqey+J850Ty2e7NTnduYH33xV"
    L"ZJJqjBEM4Miw6JitH+QwygQdwUEgbMg/yEAYWJXPlyrebKhJhitngoGNjzFW3SInNbaaQmOBGA1a7isxM7fB5o//y2GErlz4"
    L"gOdJsH+J5eEGI5yqNWPQNPHoR42Tze+qf9PUn+5ncmcMsU7jjszCjhjs2sS7BXHSEVomwlwEhYgLdSe63suLrz6QH789dU8d"
    L"2f3WmUPve+vOfe/9JztbvXPt+lP3JMn5J5yuLtCWzsCH7hgvTHD+AKw9jzh3EPnuK3Cutd9OFQ17ci3EEw+fD72wmDen5m3u"
    L"iiOpiEhuhSLLlGSU8sB6I1klCnPYGcbUONIG3LLweRDm1sr+aYzDQykNgEYlolHNqKbUIdGjkGAMLJxqIXne9UEbMsAAFhVh"
    L"ba0GoK4a6hsWdRwIDm4p3/BTXAgLvCABuOVmGG4B/Grr2TAVukZOkWQRwpFet7sPIfSgRXlWvILBAi2ULTEVgEd8wAz9EPQw"
    L"wdmMsVXk1gJkWgjH8gSN6EQKEkGAQgRFL4v9+59Y6N6jdqZRS85ON+dl994fqu+4EvW2oNYwpNTgSNqAlVynL/pAsbCOeHqp"
    L"H5fOdXyvs5JQxKWt+bQF1A1Iu71+rernQJImIrG8e1VSgpDBiChkZNnJI5ZZUDSY0jhJZBtptEiLZSq0KioAp7AS/PmoGmMI"
    L"Tg0IUZyZwmKAxQjVwsVYJHk/05xWK7J2YqpQjYgRDP31OoCWWWxiqwYL5U8AkAHoAegC6FevXSgL7QVqANKOmckt5PJPfOq+"
    L"38LOfT+FuBwVurvT67835P3EWXiOppmFGIHqoU09TIV0RrOTRpxWSh2G1OBqJtYU8W2QTRjqRmnRSYuQJkWcEI4iTgTW7eZF"
    L"p5Plp85arqoZia4XWRUnOYhI0whYKIoQqQYvoHOOifOst1KqQaLl3tS8UZyppRFMCUkB+AKagExhSCGWUEhTc0IaHdXACEKF"
    L"iBBGGCKISFAHAkIzCwY1LRMLNMYBm0dzKhoDNBZOowEqYlERY4DGgKAqGtWHvKfqxUFNVA2hiIie0CJPAHgz8xvAXtyw8P3q"
    L"p9givPDSTcAtgOHYMTnXz36ep5/aW2+2/06t1QR8clW33znq6E8Ttkhxq7C4TInLjMUyVLsWgwo1WChWoFiBUEg6VU2MlAg6"
    L"L84rrE6fJKQkNNaD+FIgxNUMbFKk7ZyvCzlFg1ODIBoEiEYJJIp64gJpOcDcTPNoKDTGCDCYIRAsqsBjlfJViJUxbg+KI+A0"
    L"WmJREpCpkokpEzNLKUyisUYnhEFAkEKlSHUkuWlQhCKa6CCQEyKiqqhRtDpEQyEGtfLMyaiIqpBgolQxNTGLEi3STCuQKGCF"
    L"sqpG03Fs4ftjC5+NLby9sjxA5f9/Drd0Adx007/93IfDyvJNLk13pe2ZPWlz+kDanLquVq97l6RIai24ukPMekZiMVpcEGDR"
    L"VFeBsKyhWGXor5pZAVgeQxEJjaaFmKon6Ai6CDgT50ip0blUKYmJ1IzSFHEtcb5uxgacNIWuCSd1gi0BPauzPUkXhBYAFjQU"
    L"JHKD5QIWRhdIi2KIQNnciQZQQnlesRkVcFTzZs4R9GoxgaJGJ4kpPRBTgIkIPRSJqnpTZYwRIRQgpA2fHjT6PBp6IWo/V03N"
    L"SpSvUWFmBS1kiUfomYaoWuZPkQo1XXv20XMArCiKwUL3N+z24qXkm/oX7wCUtvbWn+UfA/hzAFM1YOrgFdfMpbv3zs4eOLqr"
    L"tfPIgfbuy442Wu3dSb21zzfb++utmcPN9tRVPknhkib8VAoNBTQU6x66oKqLUF0xCysai+UY8hWxoq8RQRCiRCyaGs3Mm4gj"
    L"6AzmAulgLoVLUiUTUtIIaUqSNElpklIXcQ04aVKkJnQNUTiIOCk7ckcBC9ACgAK03IQ54PJy+0mgaIRIBpiJmUGNEJbMnkLU"
    L"6AE4Cn0M5qgybWaXm0aaKjTkhkjnYC1n2vaA0OhNjTGaxSLAvN/Xx+w1Zzp2vj3tw/7Z9pEv3HuOzek5F5af+dPnvvWFh1Cr"
    L"CbJsYNsHAO951fwrnBBSaoJjZvJ/Ot8H0M9Mzx0//jBw/GEAXx6nmVMAM/Pt9tzc5VfNzuw5PD994MoDU/suPzo9t/NQrdHc"
    L"6RvtA8327IF6q3241mjB+SZckpaouejnproYVReixhXTsBJDsWQhX4kxdkyLApF9x7iMvABgDkJHiLPcnFFE4VIKU4pPIEzJ"
    L"pAZhy7m0SSd1EWmQ0hTnGiTbJSClK5uh+2ieAaYBhoJgboZCzDIFAtQiYEFohZKZQaGmUWi5qV4eQzQD2et1n1vuLN2HWtMZ"
    L"IYW5WmxN71LVG2DKosjtQKtx8Ord8wdnDx7Fvp1z+Opj5/HwM+eLlPkfPXvHZ/85ZmaW4Zwiy7LnA3avSUbQLeRkItBYGbiI"
    L"M5AKog+1/sL6+pmF++4C7rtrMvENaAOYu+yKa2bb8/tmd1153WWtXYcOzu+57HBzqrWvVm/uSduzB5qtqeva7SkkaRPiEsB7"
    L"FP0uzHQ5Rl3IY1yMMazEIiwXebZSFPmaQXNRLQRxXWJUWC7lWb/OqalX56lKD1qNPklEXErxNQob4pKWT1xD6OsUNinSdM7V"
    L"KWjTUUgnWh53XQAW1CwYkJtZcFF7AJMBBjAzhCJ0is75Z7O13dpstLK13PLa7vRo4t0NMaomaSL3ffOL/+UbJ771l1e95yM7"
    L"W7sONR4+tXhOlk99887fueXbYwtd4BUuKfAv8/M2kbuMicIebpVAVsZhnIJUGJZMdempgfa4/UvjAtIEMNXesXd275ErZnZd"
    L"dtXO2cvecGTH7kNH5nbM7W20WoeSWnNfc2rqwHR7+or61BTEedAnCCGiyLOuxrjUC/F8lherMYTlIoSlXpatFkXeF9VAs76n"
    L"LrEoSKETEQeYBygZvZgwhUnNJd5TXM0JG3RJM/Gu5b3Uxfm6ONf04uoibIjAOXGspd7BjKZqIc/Rqtd3N6cOXxvr7YUVdc/l"
    L"CNkb9s4cuOPJFUtb0xI6S0sLt//h5888ffzep7/9ldMAVoZz++53e7znPYpbbnlV2ge8XpXpFxMOGyb0meIi7VJnAMzsvvr6"
    L"mUMHj87vuOJNh+f2HjqyZ/feQ9NTU/vTVuNArT61rzU1vas91YZLElA8ghFZt6PRbCEr4sJali8XeVzNiny5yPOlXj/rQGMk"
    L"EZwxENHKA5/oWJkXUEQcEjWpOecS75PEOVf3TlqBrL/pwOz8k08/96bf/vpx1msprpoOeMfhGUztP4La1DwaDDi33MUnv/Iw"
    L"kHcfXPn253/lwc9+/KsHD17biXsa/Td/93fH3o4fjV/Bl1+1hX+9BeCF39cFOoyIcwbVC+nDBoAZtPfNXvPGa+f2XXX9oR0H"
    L"Lrts576DR3bNze5tTTUPJvXWgUZret/c7KxLazXQOUR49Ps9FEWxlsW4sN6LC+v9bC0rwnKW50u9LFvLC80cYhRodLTgSXEi"
    L"ZVNPJ35xLS/e96YDh86s9P7H//SlR3Tnrp3Seez2r7qT3/jjN/7ND8xf8YZrGmfX+vq1x84VLsRHn/qtX/rcuXNPngfeRuDO"
    L"gNe4auxSFYAXJSAbZ8w5b3Zh7eFK7VGbmb3qLVPXvfkte3YdvuLI/J4DB/bt3nPZ7HTrUK3ROFBrtA7OzMxMtdpNOPFQ55Dl"
    L"hm63mwWNC51+WFrq5svdPF/J89K09Pt5r5kg/OS7r/7BT3395HVfvO+Un24kOP3F//BLx7/8e58DcAbA+U2RuWMmuOV16RDz"
    L"HSsAL104vC81h13wRJ46gDamr5y55h3X77ryuu86PDO/57K9+/Yf3j+/40CjUTuUNpsHWq2pvTtmZ5CmKSAemQJra2t6ZDaR"
    L"s+uKX/2jexGzbqf75F0ff+jf//yn9l52zUqjsWP9hhtuDLj2XfHE/qN2dO6E3vrRjypex1rRv44C8AKf2YBjN/MYANx886Di"
    L"ypxzBtMLFXgIgGmgPjP7prfPXP+2Gw/tO/yGo7t27Tq4f8/84Vpz+sqeps/9yX3PrHbX1x8PT931+ft+51fuOXjwnXjmmW9m"
    L"uLQKg7fHRQSEMKOZ8ZiZHCv5HxHnILxoK4vWpleOHZPtKf3rJhyohOPYSDjMjEJCRHDsmMm7j33J4+V3jN82Ad+R2GNbzW+P"
    L"7bE9tsf22B7bY3tsj+2xPbbH9rg0x/8PgD1sZcQyiS0AAAAASUVORK5CYII=";

static inline int B64Val(WCHAR c) {
    if (c >= L'A' && c <= L'Z') return c - L'A';
    if (c >= L'a' && c <= L'z') return c - L'a' + 26;
    if (c >= L'0' && c <= L'9') return c - L'0' + 52;
    if (c == L'+') return 62;
    if (c == L'/') return 63;
    return -1; // padding / whitespace
}

static std::vector<BYTE> Base64Decode(const WCHAR* s) {
    std::vector<BYTE> out;
    out.reserve(4096);
    int acc = 0, bits = 0;
    for (; s && *s; ++s) {
        int v = B64Val(*s);
        if (v < 0) continue;
        acc = (acc << 6) | v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((BYTE)((acc >> bits) & 0xFF));
        }
    }
    return out;
}

// ============================================================================
// ============================================================================
static HRESULT WicScaleToBitmap(IWICImagingFactory* fac, IWICBitmapSource* src,
                                UINT tw, UINT th, IWICBitmap** outBmp) {
    if (!fac || !src || !outBmp || tw == 0 || th == 0) return E_INVALIDARG;
    *outBmp = nullptr;
    IWICBitmapScaler* sc = nullptr;
    HRESULT hr = fac->CreateBitmapScaler(&sc);
    if (FAILED(hr) || !sc) return E_FAIL;
    hr = sc->Initialize(src, tw, th, WICBitmapInterpolationModeHighQualityCubic);
    if (FAILED(hr))
        hr = sc->Initialize(src, tw, th, WICBitmapInterpolationModeFant);
    if (FAILED(hr))
        hr = sc->Initialize(src, tw, th, WICBitmapInterpolationModeCubic);
    if (FAILED(hr)) {
        sc->Release();
        return hr;
    }
    hr = fac->CreateBitmapFromSource(sc, WICBitmapCacheOnLoad, outBmp);
    sc->Release();
    return hr;
}

static bool DecodePngToPremul(const BYTE* png, size_t pngSize,
                              int w, int h, std::vector<BYTE>& outPixels) {
    if (!g_wic || !png || !pngSize || w <= 0 || h <= 0) return false;

    // All COM objects are wrapped in ApComPtr so they are released on every
    // exit path (including an outPixels.resize() -> bad_alloc exception),
    // instead of the previous hand-rolled Release() chains with early returns.
    try {
        ApComPtr<IWICStream> pStream;
        if (FAILED(g_wic->CreateStream(pStream.put())) || !pStream) return false;
        HRESULT hr = pStream->InitializeFromMemory(
            const_cast<BYTE*>(png), (DWORD)pngSize);
        if (FAILED(hr)) return false;

        ApComPtr<IWICBitmapDecoder> pDecoder;
        hr = g_wic->CreateDecoderFromStream(pStream.get(), nullptr,
                                            WICDecodeMetadataCacheOnLoad,
                                            pDecoder.put());
        if (FAILED(hr) || !pDecoder) return false;

        ApComPtr<IWICBitmapFrameDecode> pFrame;
        hr = pDecoder->GetFrame(0, pFrame.put());
        if (FAILED(hr) || !pFrame) return false;

        ApComPtr<IWICFormatConverter> pConv;
        hr = g_wic->CreateFormatConverter(pConv.put());
        if (SUCCEEDED(hr) && pConv) {
            hr = pConv->Initialize((IWICBitmapSource*)pFrame.get(),
                                   GUID_WICPixelFormat32bppBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0.0,
                                   WICBitmapPaletteTypeMedianCut);
        }
        if (FAILED(hr) || !pConv) return false;

        IWICBitmapSource* src = (IWICBitmapSource*)pConv.get();
        ApComPtr<IWICBitmap> owned;   // chain of scaled bitmaps
        UINT cw = 0, ch = 0;
        src->GetSize(&cw, &ch);
        while (cw != (UINT)w || ch != (UINT)h) {
            UINT nw = cw, nh = ch;
            if (cw > (UINT)w * 2) nw = (std::max)((UINT)w, (cw + 1) / 2);
            else nw = (UINT)w;
            if (ch > (UINT)h * 2) nh = (std::max)((UINT)h, (ch + 1) / 2);
            else nh = (UINT)h;
            ApComPtr<IWICBitmap> next;
            hr = WicScaleToBitmap(g_wic, src, nw, nh, next.put());
            if (FAILED(hr) || !next) return false;
            owned.reset(next.release());   // releases the previous one
            src = (IWICBitmapSource*)owned.get();
            src->GetSize(&cw, &ch);
        }

        outPixels.resize((size_t)w * h * 4);
        hr = src->CopyPixels(nullptr, (UINT)w * 4, (UINT)outPixels.size(),
                             outPixels.data());
        if (FAILED(hr)) return false;

        BYTE maxA = 0;
        for (size_t i = 3; i < outPixels.size(); i += 4)
            if (outPixels[i] > maxA) maxA = outPixels[i];
        if (maxA == 0) {
            for (size_t i = 3; i < outPixels.size(); i += 4)
                outPixels[i] = 255;
        } else {
            for (size_t i = 0; i + 3 < outPixels.size(); i += 4) {
                BYTE a = outPixels[i + 3];
                outPixels[i + 0] = (BYTE)((outPixels[i + 0] * a + 127) / 255);
                outPixels[i + 1] = (BYTE)((outPixels[i + 1] * a + 127) / 255);
                outPixels[i + 2] = (BYTE)((outPixels[i + 2] * a + 127) / 255);
            }
        }
        return true;
    } catch (...) {
        Wh_Log(L"DecodePngToPremul: exception");
        return false;
    }
}

static HBITMAP CreateDib32(int w, int h, const void* pixels) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h; // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(NULL);
    HBITMAP hb = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (screen) ReleaseDC(NULL, screen);
    if (hb && bits) memcpy(bits, pixels, (size_t)w * h * 4);
    return hb;
}

static HBITMAP CreateBitmapFromBase64PNG(const WCHAR* b64, int w, int h) {
    std::vector<BYTE> png = Base64Decode(b64);
    std::vector<BYTE> pixels;
    if (!DecodePngToPremul(png.data(), png.size(), w, h, pixels)) return NULL;
    return CreateDib32(w, h, pixels.data());
}

static HICON CreateIconFromBase64PNG(const WCHAR* b64, int w, int h) {
    std::vector<BYTE> png = Base64Decode(b64);
    std::vector<BYTE> pixels;
    if (!DecodePngToPremul(png.data(), png.size(), w, h, pixels)) return NULL;

    HBITMAP color = CreateDib32(w, h, pixels.data());
    if (!color) return NULL;

    // RAII: both bitmaps are owned here and freed on every exit path,
    // including if the mask vector allocation throws below.
    ApScopedGdiObj colorGuard((HGDIOBJ)color);
    ApScopedGdiObj maskGuard;

    int stride = ((w + 15) & ~15) / 8;
    std::vector<BYTE> mask((size_t)stride * h, 0xFF);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            BYTE a = pixels[((size_t)y * w + x) * 4 + 3];
            if (a >= 128)
                mask[(size_t)y * stride + x / 8] &= ~(0x80 >> (x % 8));
        }
    HBITMAP maskBmp = CreateBitmap(w, h, 1, 1, mask.data());
    if (maskBmp) maskGuard.reset((HGDIOBJ)maskBmp);

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmMask = (HBITMAP)maskGuard.get();
    ii.hbmColor = (HBITMAP)colorGuard.get();
    HICON hi = CreateIconIndirect(&ii);
    // Guards release maskBmp and color on scope exit.
    return hi;
}

static void DrawAlphaBitmap(HDC hdc, HBITMAP hb, int x, int y, int w, int h) {
    BITMAP bm = {};
    if (!hb || !GetObject(hb, sizeof(bm), &bm) || bm.bmWidth <= 0) return;
    HDC mem = CreateCompatibleDC(hdc);
    if (!mem) return;
    ApScopedDc memGuard(mem);               // Deletes the DC on scope exit
    HBITMAP old = (HBITMAP)SelectObject(mem, hb);
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = (bm.bmBitsPixel == 32) ? AC_SRC_ALPHA : 0;
    BOOL ok = AlphaBlend(hdc, x, y, w, h, mem, 0, 0, bm.bmWidth, bm.bmHeight, bf);
    if (!ok)
        StretchBlt(hdc, x, y, w, h, mem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    // Restore the caller's bitmap before the DC is destroyed. `old` is not
    // owned here, so it must not be deleted. The memGuard frees the DC.
    if (old) SelectObject(mem, old);
}

// ============================================================================
// ============================================================================
static void FreeDpiResources() {
    if (g_fontTitle) DeleteObject(g_fontTitle), g_fontTitle = nullptr;
    if (g_fontText) DeleteObject(g_fontText), g_fontText = nullptr;
    if (g_fontBold) DeleteObject(g_fontBold), g_fontBold = nullptr;
    if (g_fontSmall) DeleteObject(g_fontSmall), g_fontSmall = nullptr;
    if (g_fontLink) DeleteObject(g_fontLink), g_fontLink = nullptr;
    if (g_fontLinkUnder) DeleteObject(g_fontLinkUnder), g_fontLinkUnder = nullptr;
    if (g_bmpFolder) DeleteObject(g_bmpFolder), g_bmpFolder = nullptr;
    if (g_bmpSetup) DeleteObject(g_bmpSetup), g_bmpSetup = nullptr;
    if (g_bmpReadyBoost) DeleteObject(g_bmpReadyBoost), g_bmpReadyBoost = nullptr;
    if (g_bmpDrive48) DeleteObject(g_bmpDrive48), g_bmpDrive48 = nullptr;
    if (g_bmpDisc48) DeleteObject(g_bmpDisc48), g_bmpDisc48 = nullptr;
    if (g_hicoDrive48) DestroyIcon(g_hicoDrive48), g_hicoDrive48 = nullptr;
    if (g_hicoDisc48) DestroyIcon(g_hicoDisc48), g_hicoDisc48 = nullptr;
    if (g_bmpLocal48) DeleteObject(g_bmpLocal48), g_bmpLocal48 = nullptr;
    if (g_hicoLocal48) DestroyIcon(g_hicoLocal48), g_hicoLocal48 = nullptr;
    if (g_bmpPhone48) DeleteObject(g_bmpPhone48), g_bmpPhone48 = nullptr;
    if (g_hicoPhone48) DestroyIcon(g_hicoPhone48), g_hicoPhone48 = nullptr;
    if (g_bmpPlay) DeleteObject(g_bmpPlay), g_bmpPlay = nullptr;
    if (g_bmpAutoPlay16) DeleteObject(g_bmpAutoPlay16), g_bmpAutoPlay16 = nullptr;
    g_resDpi = 0;
}

static HFONT MkFont(int pt, int weight, BOOL underline = FALSE) {
    return CreateFontW(-MulDiv(pt, (int)g_dpi, 72), 0, 0, 0, weight, FALSE,
                       underline, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

static void RebuildHeaderIcon();   // forward decl, defined below (item 5 fix)
static void RebindSharedIcons();   // forward decl, defined below (item 5 fix)
static void EnsureDpiResources();  // forward decl, defined right below

// Item 5 fix: wraps EnsureDpiResources() so every call site rebinds the shared
// icon handles it may have just recreated, instead of relying on each caller to
// remember to do it (WM_CREATE and ComputeLayout() previously didn't).
static void EnsureDpiResourcesAndRebind() {
    bool recreated = (g_resDpi != g_dpi) || !g_fontText;
    EnsureDpiResources();
    if (recreated) {
        RebuildHeaderIcon();
        RebindSharedIcons();
    }
}

static void EnsureDpiResources() {
    if (g_resDpi == g_dpi && g_fontText) return;
    FreeDpiResources();
    g_resDpi = g_dpi;
    g_fontTitle     = MkFont(12, FW_NORMAL); // Win7 AutoPlay: Segoe UI 12 regular
    g_fontText      = MkFont(9, FW_NORMAL);
    g_fontBold      = MkFont(9, FW_SEMIBOLD);
    g_fontSmall     = MkFont(8, FW_NORMAL);
    g_fontLink      = MkFont(9, FW_NORMAL);
    g_fontLinkUnder = MkFont(9, FW_NORMAL, TRUE);

    int s32 = Scale(32), s48 = Scale(48), s16 = Scale(16);
    g_bmpFolder = CreateBitmapFromBase64PNG(USER_FOLDER_ICON_BASE64, s32, s32);
    if (!g_bmpFolder) g_bmpFolder = CreateBitmapFromBase64PNG(WIN7_NATIVE_FOLDER_BASE64, s32, s32);
    if (!g_bmpFolder) g_bmpFolder = CreateBitmapFromBase64PNG(FOLDER_ICON_BASE64, s32, s32);
    g_bmpSetup = CreateBitmapFromBase64PNG(WIN7_NATIVE_FLOPPY_BASE64, s32, s32);
    if (!g_bmpSetup) g_bmpSetup = CreateBitmapFromBase64PNG(SETUP_ICON_BASE64, s32, s32);
    int s32w = MulDiv(s32, 21, 20); // +5% larghezza ReadyBoost
    g_bmpReadyBoost = CreateBitmapFromBase64PNG(USER_READYBOOST_ICON_BASE64, s32w, s32);
    if (!g_bmpReadyBoost) g_bmpReadyBoost = CreateBitmapFromBase64PNG(WIN7_NATIVE_READYBOOST_BASE64, s32, s32);
    if (!g_bmpReadyBoost) g_bmpReadyBoost = CreateBitmapFromBase64PNG(READYBOOST_ICON_BASE64, s32, s32);
    g_bmpDrive48 = CreateBitmapFromBase64PNG(USER_REMOVABLE_ICON_BASE64, s48, s48);
    if (!g_bmpDrive48) g_bmpDrive48 = CreateBitmapFromBase64PNG(USER_DRIVE_ICON_BASE64, s48, s48);
    if (!g_bmpDrive48) g_bmpDrive48 = CreateBitmapFromBase64PNG(DRIVE_ICON_BASE64, s48, s48);
    if (!g_bmpDrive48) g_bmpDrive48 = CreateBitmapFromBase64PNG(WIN7_NATIVE_FLOPPY_BASE64, s48, s48);
    g_hicoDrive48 = CreateIconFromBase64PNG(USER_REMOVABLE_ICON_BASE64, s48, s48);
    if (!g_hicoDrive48) g_hicoDrive48 = CreateIconFromBase64PNG(USER_DRIVE_ICON_BASE64, s48, s48);
    // Local (fixed) disk icon: silver external/HDD image, independent of the
    // removable-drive icon so a fixed disk (e.g. Z: = host OS partition) is
    // shown distinctly while removable drives keep the current one.
    g_hicoLocal48 = CreateIconFromBase64PNG(USER_LOCALDISK_ICON_BASE64, s48, s48);
    if (!g_hicoLocal48)
        g_hicoLocal48 = CreateIconFromBase64PNG(USER_DRIVE_ICON_BASE64, s48, s48);
    g_bmpLocal48 = CreateBitmapFromBase64PNG(USER_LOCALDISK_ICON_BASE64, s48, s48);
    if (!g_bmpLocal48)
        g_bmpLocal48 = CreateBitmapFromBase64PNG(USER_DRIVE_ICON_BASE64, s48, s48);
    // Optional-list fix: the old fallback here called CreateIconIndirect with
    // hbmColor == hbmMask, passing the 32-bpp g_bmpDrive48 DIB as the mask.
    // ICONINFO::hbmMask must be a 1-bpp monochrome bitmap, so that call either
    // failed outright or produced a garbage icon. CreateIconFromBase64PNG
    // already builds a correct mask; if both calls above failed, just leave
    // g_hicoDrive48 null and let the g_bmpDrive48 bitmap path (drawn directly,
    // not via an HICON) handle rendering instead.
    g_bmpDisc48 = CreateBitmapFromBase64PNG(USER_CDROM_DRIVE_ICON_BASE64, s48, s48);
    if (!g_bmpDisc48) g_bmpDisc48 = CreateBitmapFromBase64PNG(USER_DISC_ICON_BASE64, s48, s48);
    if (!g_bmpDisc48) g_bmpDisc48 = CreateBitmapFromBase64PNG(WIN7_NATIVE_DISC_BASE64, s48, s48);
    if (!g_bmpDisc48) g_bmpDisc48 = CreateBitmapFromBase64PNG(DRIVE_ICON_BASE64, s48, s48);
    g_hicoDisc48 = CreateIconFromBase64PNG(USER_CDROM_DRIVE_ICON_BASE64, s48, s48);
    if (!g_hicoDisc48) g_hicoDisc48 = CreateIconFromBase64PNG(USER_DISC_ICON_BASE64, s48, s48);
    g_bmpPhone48 = CreateBitmapFromBase64PNG(USER_PHONE_ICON_BASE64, s48, s48);
    g_hicoPhone48 = CreateIconFromBase64PNG(USER_PHONE_ICON_BASE64, s48, s48);
    g_bmpPlay = CreateBitmapFromBase64PNG(WMP_PLAYER_ICON_BASE64, s32, s32);
    if (!g_bmpPlay) g_bmpPlay = CreateBitmapFromBase64PNG(AUTOPLAY_ICON_BASE64, s32, s32);
    g_bmpAutoPlay16 = CreateBitmapFromBase64PNG(AUTOPLAY_ICON_BASE64, s16, s16);
    if (!g_bmpAutoPlay16) g_bmpAutoPlay16 = CreateBitmapFromBase64PNG(WIN7_NATIVE_DISC_BASE64, s16, s16);
}

// ============================================================================
// ============================================================================
struct DialogIcon {
    HICON hIcon = nullptr;
    HBITMAP hBmp = nullptr;
    bool shared = false;

    void Free() {
        if (!shared) {
            if (hIcon) DestroyIcon(hIcon);
            if (hBmp) DeleteObject(hBmp);
        }
        hIcon = nullptr; hBmp = nullptr; shared = false;
    }
    void Draw(HDC hdc, int x, int y, int size) const {
        if (hIcon)
            DrawIconEx(hdc, x, y, hIcon, size, size, 0, NULL, DI_NORMAL);
        else if (hBmp)
            DrawAlphaBitmap(hdc, hBmp, x, y, size, size);
    }
};

enum class ActionType { RunProgram, OpenFolder, ReadyBoost, PlayMedia, ViewPictures,
                         BurnDisc, ImportPictures, ImportMusic, SyncDevice,
                         ViewSlideshow };
enum class OptionGroup { Program, Content, General };
enum class ContentKind { General, Empty, Pictures, Music, Mixed, Software, AudioCD, DataDisc, Portable, Video, DvdMovie, BlankDisc };

struct AutoPlayOption {
    ActionType type = ActionType::OpenFolder;
    OptionGroup group = OptionGroup::General;
    std::wstring line1, line2;
    std::wstring programPath, programArgs;
    std::wstring targetPath;
    DialogIcon icon;
};

static std::vector<AutoPlayOption> g_options;
static DialogIcon g_hdrIcon;
static int     g_firstGeneralIdx = 0;
static bool    g_hasProgramSection = false;
static bool    g_audioCd = false;
static ContentKind g_contentKind = ContentKind::General;
static std::wstring g_driveRoot;
static std::wstring g_driveTitle;
static std::wstring g_headerSub;
static std::wstring g_prompt;
static int     g_driveLetter = 0;
static UINT    g_driveType = DRIVE_UNKNOWN;
static bool    g_isWpd = false;
static std::wstring g_wpdPath;
static std::wstring g_wpdId;
static HDEVNOTIFY g_hDevNotify = nullptr;
static UINT    g_msgQueryCancelAP = 0;
// Reverted the tool-mod conversion (couldn't be verified without a real Windows
// build/test cycle) - back to explorer.exe injection with the original
// single-instance mutex.
static HANDLE  g_hOwnerMutex = nullptr;
static bool    g_ownsAutoPlay = false;   // only the owning instance writes/deletes CancelAutoplay values
static HANDLE  g_hActionWorker = nullptr;

struct HotRect { RECT rc; int kind; int idx; }; // 0=option 1=link 2=checkbox
static std::vector<HotRect> g_hotRects;
static int  g_hoverItem = -1;
static int  g_focusItem = -1;
static int  g_pressedItem = -1;
static bool g_trackingLeave = false;
static bool g_kbdFocus = false;
static bool g_alwaysChecked = false;
static RECT g_rcCheckHit = {};
static RECT g_rcLink = {};
static int  g_headerH = 0;
static int  g_checkBottom = 0;

struct PendingVolume {
    int letter = 0;
    int tries = 0;
    ULONGLONG firstTick = 0;
    bool isWpd = false;
    std::wstring wpdPath;
};
static std::vector<PendingVolume> g_pending;

#ifndef SHIL_JUMBO
#define SHIL_JUMBO 4
#endif
#ifndef SHIL_EXTRALARGE
#define SHIL_EXTRALARGE 2
#endif
// IImageList GUID: lld/clang di Windhawk non esporta IID_IImageList da libuuid.
static const IID IID_IImageListLocal =
    { 0x46EB5926, 0x582E, 0x4017, { 0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50 } };

// ============================================================================
// Windows 7 AutoPlay client metrics at 96 DPI. Values come from the original
// dialog proportions (icon 48, option 32, ~384-wide client).
// ============================================================================
static const int W_WIDTH      = 400;
static const int HDR_PAD_X    = 18;
static const int HDR_PAD_Y    = 12;
static const int HDR_ICON_SZ  = 48;
static const int HDR_TEXT_GAP = 12;
static const int BODY_GAP     = 8;
static const int ROW_H_MIN    = 42;
static const int ROW_ICON     = 32;
static const int ROW_PAD_X    = 16;
static const int ROW_TEXT_GAP = 10;
static const int SECT_H_MIN   = 24;
static const int LINK_H_MIN   = 22;
static const int END_PAD      = 18;
static const int CHECK_BOX    = 13;

static const COLORREF CLR_TITLE       = RGB(0, 51, 153); // Win7 Main Instruction
static const COLORREF CLR_SUB_TEXT    = RGB(89, 89, 89);
static const COLORREF CLR_LINE1       = RGB(20, 20, 20);
static const COLORREF CLR_LINE2       = RGB(96, 96, 96);
static const COLORREF CLR_SECTION     = RGB(56, 80, 134);
static const COLORREF CLR_LINK        = RGB(0, 102, 204);
static const COLORREF CLR_HAIRLINE    = RGB(205, 205, 205);
static const COLORREF CLR_HOVER_FILL  = RGB(229, 243, 252);
static const COLORREF CLR_HOVER_EDGE  = RGB(151, 196, 232);

// ============================================================================
// ============================================================================
static bool IsHighContrastOn() {
    HIGHCONTRASTW hc = { sizeof(hc) };
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) &&
           (hc.dwFlags & HCF_HIGHCONTRASTON);
}

static COLORREF GetWin7TitleColor() {
    if (IsHighContrastOn())
        return GetSysColor(COLOR_WINDOWTEXT);
    HTHEME th = OpenThemeData(NULL, L"TEXTSTYLE");
    if (th) {
        COLORREF c = CLR_TITLE;
        HRESULT hr = GetThemeColor(th, TEXT_MAININSTRUCTION, 0, TMT_TEXTCOLOR, &c);
        CloseThemeData(th);
        if (SUCCEEDED(hr)) return c;
    }
    return CLR_TITLE;
}

static std::wstring GetDriveRootForLetter(int letter) {
    std::wstring r;
    r.push_back((wchar_t)letter);
    r += L":\\";
    return r;
}

static bool IsSystemDriveLetter(int letter) {
    WCHAR sysDir[MAX_PATH] = {};
    GetWindowsDirectoryW(sysDir, MAX_PATH);
    return sysDir[0] && towupper(sysDir[0]) == towupper((wchar_t)letter);
}

static bool DrivePresent(int letter) {
    std::wstring root = GetDriveRootForLetter(letter);
    UINT dt = GetDriveTypeW(root.c_str());
    return dt != DRIVE_NO_ROOT_DIR && dt != DRIVE_UNKNOWN;
}

static bool VolumeReady(const std::wstring& root, std::wstring* outVolName = nullptr,
                        std::wstring* outFs = nullptr) {
    WCHAR vol[MAX_PATH] = {}, fs[MAX_PATH] = {};
    DWORD serial = 0, maxComp = 0, flags = 0;
    if (!GetVolumeInformationW(root.c_str(), vol, MAX_PATH, &serial,
                               &maxComp, &flags, fs, MAX_PATH))
        return false;
    if (!fs[0]) return false;
    if (outVolName) *outVolName = vol;
    if (outFs) *outFs = fs;
    return true;
}

static bool PathIsUnderRoot(const std::wstring& path, const std::wstring& root) {
    WCHAR full[MAX_PATH] = {}, rootFull[MAX_PATH] = {};
    if (!GetFullPathNameW(path.c_str(), MAX_PATH, full, nullptr)) return false;
    if (!GetFullPathNameW(root.c_str(), MAX_PATH, rootFull, nullptr)) return false;
    size_t n = wcslen(rootFull);
    if (n == 0 || wcslen(full) < n) return false;
    if (_wcsnicmp(full, rootFull, n) != 0) return false;
    return true;
}

static HICON GetShellIcon(PCWSTR path, int mode) {
    SHFILEINFOW sfi = {};
    if (mode == 2) {
        if (!SHGetFileInfoW(path, 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX) &&
            !SHGetFileInfoW(path, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
                            SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES))
            return NULL;
        int want = Scale(48);
        int shil = (want > 48) ? SHIL_JUMBO : SHIL_EXTRALARGE;
        IImageList* piml = nullptr;
        if (FAILED(SHGetImageList(shil, IID_IImageListLocal, (void**)&piml)) || !piml) {
            if (FAILED(SHGetImageList(SHIL_EXTRALARGE, IID_IImageListLocal, (void**)&piml)) || !piml)
                piml = nullptr;
        }
        if (piml) {
            HICON hi = nullptr;
            piml->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hi);
            piml->Release();
            if (hi) return hi;
        }
        if (SHGetFileInfoW(path, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) ||
            SHGetFileInfoW(path, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
                           SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES))
            return sfi.hIcon;
        return NULL;
    }

    UINT flags = SHGFI_ICON | ((mode == 0) ? SHGFI_LARGEICON : SHGFI_SMALLICON);
    if (!SHGetFileInfoW(path, 0, &sfi, sizeof(sfi), flags) &&
        !SHGetFileInfoW(path, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
                        flags | SHGFI_USEFILEATTRIBUTES))
        return NULL;
    return sfi.hIcon;
}

static std::wstring GetCompanyName(const std::wstring& exePath) {
    DWORD dummy = 0;
    DWORD size = GetFileVersionInfoSizeW(exePath.c_str(), &dummy);
    if (!size) return L"";
    std::vector<BYTE> buf(size);
    if (!GetFileVersionInfoW(exePath.c_str(), 0, size, buf.data())) return L"";

    struct LangCP { WORD lang, cp; } *trans = nullptr;
    UINT transLen = 0;
    std::wstring key;
    if (VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation",
                       (LPVOID*)&trans, &transLen) && trans &&
        transLen >= sizeof(LangCP)) {
        wchar_t tmp[64];
        swprintf_s(tmp, ARRAYSIZE(tmp), L"\\StringFileInfo\\%04x%04x\\CompanyName",
                   trans[0].lang, trans[0].cp);
        key = tmp;
    } else {
        key = L"\\StringFileInfo\\040904B0\\CompanyName";
    }
    LPVOID data = nullptr;
    UINT len = 0;
    if (!VerQueryValueW(buf.data(), key.c_str(), &data, &len) || !data || !len)
        return L"";
    // Bound the string by the length VerQueryValueW returned instead of scanning
    // for a NUL, so a crafted/truncated version resource cannot over-read (item 9).
    size_t chars = (size_t)len / sizeof(wchar_t);
    const wchar_t* p = (const wchar_t*)data;
    size_t n = 0;
    while (n < chars && p[n]) n++;
    return std::wstring(p, n);
}

// ============================================================================
// Policy di AutoPlay: sola lettura. Non scriviamo mai queste chiavi.
// ============================================================================
static const WCHAR kPolExplorer[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";
static const WCHAR kAutoplayHandlers[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers";
static const WCHAR kCancelClsidPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers\\CancelAutoplay\\CLSID";

static bool RegReadDword(HKEY root, PCWSTR subkey, PCWSTR value, DWORD* out) {
    ApRegKey key;
    if (RegOpenKeyExW(root, subkey, 0, KEY_READ, &key.h) != ERROR_SUCCESS)
        return false;
    DWORD data = 0, type = 0, cb = sizeof(data);
    LONG r = RegQueryValueExW(key.h, value, nullptr, &type, (LPBYTE)&data, &cb);
    if (r != ERROR_SUCCESS || type != REG_DWORD || cb != sizeof(DWORD))
        return false;
    *out = data;
    return true;
}

static DWORD ReadPolicyDword(PCWSTR subkey, PCWSTR value, bool* found) {
    DWORD v = 0;
    if (RegReadDword(HKEY_CURRENT_USER, subkey, value, &v)) {
        if (found) *found = true;
        return v;
    }
    if (RegReadDword(HKEY_LOCAL_MACHINE, subkey, value, &v)) {
        if (found) *found = true;
        return v;
    }
    if (found) *found = false;
    return 0;
}

static bool IsAutoPlayGloballyDisabled() {
    bool found = false;
    DWORD v = ReadPolicyDword(kAutoplayHandlers, L"DisableAutoplay", &found);
    if (found && v != 0) return true;
    v = ReadPolicyDword(kPolExplorer, L"DisableAutoplay", &found);
    return found && v != 0;
}

static bool IsNonVolumeAutoPlayBlocked() {
    bool found = false;
    DWORD v = ReadPolicyDword(kPolExplorer, L"NoAutoplayfornonVolume", &found);
    return found && v != 0;
}

static DWORD DriveTypePolicyBit(UINT driveType) {
    switch (driveType) {
        case DRIVE_UNKNOWN:   return 0x01;
        case DRIVE_NO_ROOT_DIR: return 0x02;
        case DRIVE_REMOVABLE: return 0x04;
        case DRIVE_FIXED:     return 0x08;
        case DRIVE_REMOTE:    return 0x10;
        case DRIVE_CDROM:     return 0x20;
        case DRIVE_RAMDISK:   return 0x40;
        default:              return 0x01;
    }
}

static bool IsDriveTypeBlockedByPolicy(UINT driveType) {
    bool found = false;
    DWORD mask = ReadPolicyDword(kPolExplorer, L"NoDriveTypeAutoRun", &found);
    if (!found) return false;
    return (mask & DriveTypePolicyBit(driveType)) != 0;
}

static bool IsDriveLetterBlockedByPolicy(int letter) {
    int idx = (int)towupper((wint_t)letter) - (int)L'A';
    if (idx < 0 || idx > 25) return false;
    bool found = false;
    DWORD mask = ReadPolicyDword(kPolExplorer, L"NoDriveAutoRun", &found);
    if (!found) return false;
    return (mask & (1u << idx)) != 0;
}

static bool VolumeAllowedByPolicy(int letter, UINT driveType) {
    if (IsAutoPlayGloballyDisabled()) return false;
    if (IsDriveLetterBlockedByPolicy(letter)) return false;
    if (IsDriveTypeBlockedByPolicy(driveType)) return false;
    return true;
}

// ============================================================================
// IQueryCancelAutoPlay + ROT + CLSID HKCU (non e' una policy).
// ============================================================================
// {A7E4C9B1-3D52-4F08-9E6A-7C1B2D4E8F01}
static const CLSID CLSID_Win7CancelAP =
    { 0xA7E4C9B1, 0x3D52, 0x4F08, { 0x9E, 0x6A, 0x7C, 0x1B, 0x2D, 0x4E, 0x8F, 0x01 } };
static const WCHAR kCancelClsidBraces[] = L"{A7E4C9B1-3D52-4F08-9E6A-7C1B2D4E8F01}";
static const WCHAR kCancelClsidBare[]   = L"A7E4C9B1-3D52-4F08-9E6A-7C1B2D4E8F01";
static const IID IID_IQueryCancelAutoPlayLocal =
    { 0xDDEFE873, 0x6997, 0x4e68, { 0xBE, 0x26, 0x39, 0xB6, 0x33, 0xAD, 0xBE, 0x12 } };

#ifndef __IQueryCancelAutoPlay_INTERFACE_DEFINED__
#define __IQueryCancelAutoPlay_INTERFACE_DEFINED__
MIDL_INTERFACE("DDEFE873-6997-4e68-BE26-39B633ADBE12")
IQueryCancelAutoPlay : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE AllowAutoPlay(
        LPCWSTR pszPath, DWORD dwContentType,
        LPCWSTR pszLabel, DWORD dwSerialNumber) = 0;
};
#endif

static const GUID GUID_DEVINTERFACE_WPD_ =
    { 0x6AC27878, 0xA6FA, 0x4155, { 0xBA, 0x85, 0xF9, 0x8F, 0x49, 0x1D, 0x4F, 0x33 } };
static const CLSID CLSID_PortableDeviceManager_ =
    { 0x0af10cec, 0x2ecd, 0x4b92, { 0x95, 0x81, 0x34, 0xf6, 0xae, 0x06, 0x37, 0xf3 } };
static const IID IID_IPortableDeviceManager_ =
    { 0xa1567595, 0x4c2f, 0x4574, { 0xa6, 0xfa, 0xec, 0xef, 0x91, 0x7b, 0x9a, 0x40 } };

struct IPortableDeviceManagerWH : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetDevices(LPWSTR* pPnPDeviceIDs, DWORD* pcPnPDeviceIDs) = 0;
    virtual HRESULT STDMETHODCALLTYPE RefreshDeviceList() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFriendlyName(LPCWSTR pszPnPDeviceID, WCHAR* pName, DWORD* pcch) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceDescription(LPCWSTR pszPnPDeviceID, WCHAR* pName, DWORD* pcch) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceManufacturer(LPCWSTR pszPnPDeviceID, WCHAR* pName, DWORD* pcch) = 0;
};

static bool IsHotplugOrCardReader(const wchar_t* root) {
    if (!root || !root[0] || root[1] != L':') return false;
    WCHAR dev[] = L"\\\\.\\X:";
    dev[4] = root[0];
    HANDLE h = CreateFileW(dev, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    STORAGE_PROPERTY_QUERY q = {};
    q.PropertyId = StorageDeviceProperty;
    q.QueryType = PropertyStandardQuery;
    BYTE buf[512] = {};
    DWORD ret = 0;
    bool hot = false;
    if (DeviceIoControl(h, IOCTL_STORAGE_QUERY_PROPERTY, &q, sizeof(q),
                        buf, sizeof(buf), &ret, nullptr) && ret >= sizeof(STORAGE_DEVICE_DESCRIPTOR)) {
        auto* desc = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buf);
        if (desc->RemovableMedia) hot = true;
        else if (desc->BusType == BusTypeUsb || desc->BusType == BusTypeSd ||
                 desc->BusType == BusTypeMmc)
            hot = true;
    }
    CloseHandle(h);
    return hot;
}

static bool DriveTypeWeHandle(UINT dt, const wchar_t* root = nullptr) {
    if (dt == DRIVE_REMOVABLE || dt == DRIVE_CDROM) return true;
    if (dt == DRIVE_FIXED && (g_settings.includeFixedDrives ||
                              (root && IsHotplugOrCardReader(root))))
        return true;
    if (dt == DRIVE_REMOTE && g_settings.includeNetworkDrives) return true;
    if (dt == DRIVE_RAMDISK && g_settings.includeRamDisks) return true;
    return false;
}

// USB flash/HDD already arrive as DRIVE_REMOVABLE. Windows also exposes them
// as WPD via WPDBUSENUM+USBSTOR#Disk — that must not open a second dialog.
static bool IsWpdMassStorageAlias(const wchar_t* path) {
    if (!path || !*path) return false;
    if (StrStrIW(path, L"USBSTOR#Disk") || StrStrIW(path, L"USBSTOR#disk"))
        return true;
    if (StrStrIW(path, L"WPDBUSENUM") &&
        (StrStrIW(path, L"USBSTOR") ||
         StrStrIW(path, L"{53f56307-b6bf-11d0-94f2-00a0c91efb8b}")))
        return true;
    return false;
}

static DWORD GetTrayOwnerPid() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) tray = FindWindowW(L"Progman", nullptr);
    DWORD pid = 0;
    if (tray) GetWindowThreadProcessId(tray, &pid);
    return pid;
}

static bool IsMainExplorerShell() {
    DWORD owner = GetTrayOwnerPid();
    if (!owner) return true;
    return owner == GetCurrentProcessId();
}

static bool TryBecomeAutoPlayOwner() {
    if (g_hOwnerMutex) return true;
    HANDLE h = CreateMutexW(nullptr, TRUE, L"Local\\Win7ClassicAutoPlay.Owner");
    if (!h) return false;
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(h);
        return false;
    }
    g_hOwnerMutex = h;
    g_ownsAutoPlay = true;
    return true;
}

static void ReleaseAutoPlayOwner() {
    if (g_hOwnerMutex) {
        ReleaseMutex(g_hOwnerMutex);
        CloseHandle(g_hOwnerMutex);
        g_hOwnerMutex = nullptr;
    }
    g_ownsAutoPlay = false;
}

class CancelAutoPlayObj : public IQueryCancelAutoPlay {
    LONG m_ref = 1;
    IUnknown* m_marshal = nullptr;
public:
    CancelAutoPlayObj() {
        CoCreateFreeThreadedMarshaler(static_cast<IQueryCancelAutoPlay*>(this), &m_marshal);
    }
    virtual ~CancelAutoPlayObj() {
        if (m_marshal) m_marshal->Release();
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IQueryCancelAutoPlayLocal) {
            *ppv = static_cast<IQueryCancelAutoPlay*>(this);
            AddRef();
            return S_OK;
        }
        if (m_marshal && riid == IID_IMarshal)
            return m_marshal->QueryInterface(riid, ppv);
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return (ULONG)InterlockedIncrement(&m_ref);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = (ULONG)InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE AllowAutoPlay(LPCWSTR pszPath, DWORD /*dwContentType*/,
                                            LPCWSTR /*pszLabel*/, DWORD /*dwSerialNumber*/) override {
        try {
            if (!g_settings.suppressNativeAutoPlay) return S_OK;
            if (!g_hUiThread) return S_OK;
            if (IsAutoPlayGloballyDisabled()) return S_OK;
            if (pszPath && pszPath[0] && pszPath[1] == L':') {
                int letter = (int)towupper((wint_t)pszPath[0]);
                if (IsSystemDriveLetter(letter)) return S_OK;
                std::wstring root = GetDriveRootForLetter(letter);
                UINT dt = GetDriveTypeW(root.c_str());
                if (!VolumeAllowedByPolicy(letter, dt)) return S_OK;
                if (DriveTypeWeHandle(dt, root.c_str())) {
                    Wh_Log(L"AllowAutoPlay: veto native %c: type=%u", letter, dt);
                    return S_FALSE;
                }
                return S_OK;
            }
            if (pszPath && IsWpdMassStorageAlias(pszPath))
                return S_OK;
            if (g_settings.includeMtpDevices && !IsNonVolumeAutoPlayBlocked()) {
                Wh_Log(L"AllowAutoPlay: veto native MTP/WPD");
                return S_FALSE;
            }
            return S_OK;
        } catch (...) {
            Wh_Log(L"AllowAutoPlay: exception, allow native");
            return S_OK;
        }
    }
};

class CancelClassFactory : public IClassFactory {
    LONG m_ref = 1;
public:
    virtual ~CancelClassFactory() = default;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return (ULONG)InterlockedIncrement(&m_ref);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = (ULONG)InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override {
        if (outer) return CLASS_E_NOAGGREGATION;
        CancelAutoPlayObj* o = new CancelAutoPlayObj();
        HRESULT hr = o->QueryInterface(riid, ppv);
        o->Release();
        return hr;
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) override { return S_OK; }
};

// Intercept only the shell verb used by Explorer's "Open AutoPlay..."
// context-menu command. Device-arrival handling remains unchanged.
static decltype(&ShellExecuteExW) g_ShellExecuteExW_Original = nullptr;
static decltype(&ShellExecuteW) g_ShellExecuteW_Original = nullptr;
static decltype(&ShellExecuteExA) g_ShellExecuteExA_Original = nullptr;
static decltype(&ShellExecuteA) g_ShellExecuteA_Original = nullptr;
static decltype(&CreateProcessW) g_CreateProcessW_Original = nullptr;
static decltype(&CreateProcessA) g_CreateProcessA_Original = nullptr;

static bool GetAutoPlayVerbDrive(PCWSTR verb, PCWSTR file, int* letter) {
    // lpVerb may legally be an ordinal resource (MAKEINTRESOURCE).
    if (!verb || (ULONG_PTR)verb <= 0xFFFF ||
        _wcsicmp(verb, L"autoplay") != 0 || !file || !file[0])
        return false;
    if (!(((file[0] >= L'A' && file[0] <= L'Z') ||
           (file[0] >= L'a' && file[0] <= L'z')) && file[1] == L':'))
        return false;
    if (file[2] != L'\0' && file[2] != L'\\') return false;
    *letter = (int)towupper((wint_t)file[0]);
    return true;
}

static bool QueueContextAutoPlay(PCWSTR verb, PCWSTR file) {
    int letter = 0;
    if (!GetAutoPlayVerbDrive(verb, file, &letter) || !g_hwndListener)
        return false;
    // Manual invocation must ignore a remembered automatic action.
    if (!PostMessageW(g_hwndListener, WMU_CONTEXT_AUTOPLAY,
                      (WPARAM)letter, 0))
        return false;
    Wh_Log(L"Context AutoPlay: intercepted %c:", letter);
    return true;
}

static int FindAutoplayDriveInText(PCWSTR text) {
    if (!text) return 0;
    for (const wchar_t* p = text; *p; ++p) {
        if (((*p >= L'A' && *p <= L'Z') || (*p >= L'a' && *p <= L'z')) &&
            p[1] == L':' && p[2] == L'\\')
            return (int)towupper((wint_t)*p);
    }
    return 0;
}

static bool IsAutoplayLaunchText(PCWSTR text) {
    if (!text) return false;
    // Only match the precise, unambiguous AutoPlay entry points. The bare
    // substring "autoplay" is deliberately NOT matched: it would block unrelated
    // launches (e.g. D:\AutoPlay\setup.exe, a folder named "AutoPlay something").
    // "Microsoft.AutoPlay" is also excluded so the mod does not block its own
    // "View more AutoPlay options in Control Panel" link (item 2).
    return StrStrIW(text, L"9C60DE1E-E5FC-40F4-A487-460851A8D915") ||
           StrStrIW(text, L"ms-settings:autoplay");
}

static bool RedirectAutoplayLaunch(PCWSTR file, PCWSTR parameters) {
    if (!IsAutoplayLaunchText(file) && !IsAutoplayLaunchText(parameters))
        return false;
    int letter = FindAutoplayDriveInText(file);
    if (!letter) letter = FindAutoplayDriveInText(parameters);
    if (!letter) letter = g_driveLetter;
    if (!letter || !g_hwndListener) return false;
    if (!PostMessageW(g_hwndListener, WMU_CONTEXT_AUTOPLAY,
                      (WPARAM)letter, 0)) return false;
    Wh_Log(L"AutoPlay launch redirected to classic dialog: %c:", letter);
    return true;
}

static BOOL WINAPI CreateProcessWHook(LPCWSTR app, LPWSTR cmd, LPSECURITY_ATTRIBUTES pa,
                                      LPSECURITY_ATTRIBUTES ta, BOOL inherit, DWORD flags,
                                      LPVOID env, LPCWSTR dir, LPSTARTUPINFOW si,
                                      LPPROCESS_INFORMATION pi) {
    if (RedirectAutoplayLaunch(app, cmd)) {
        SetLastError(ERROR_CANCELLED);
        return FALSE;
    }
    return g_CreateProcessW_Original(app, cmd, pa, ta, inherit, flags, env, dir, si, pi);
}

static BOOL WINAPI CreateProcessAHook(LPCSTR app, LPSTR cmd, LPSECURITY_ATTRIBUTES pa,
                                      LPSECURITY_ATTRIBUTES ta, BOOL inherit, DWORD flags,
                                      LPVOID env, LPCSTR dir, LPSTARTUPINFOA si,
                                      LPPROCESS_INFORMATION pi) {
    WCHAR wapp[MAX_PATH] = {}, wcmd[2048] = {};
    if (app) MultiByteToWideChar(CP_ACP, 0, app, -1, wapp, ARRAYSIZE(wapp));
    if (cmd) MultiByteToWideChar(CP_ACP, 0, cmd, -1, wcmd, ARRAYSIZE(wcmd));
    if (RedirectAutoplayLaunch(wapp, wcmd)) {
        SetLastError(ERROR_CANCELLED);
        return FALSE;
    }
    return g_CreateProcessA_Original(app, cmd, pa, ta, inherit, flags, env, dir, si, pi);
}

static bool QueueContextAutoPlayA(LPCSTR verb, LPCSTR file) {
    if (!verb || !file) return false;
    WCHAR wverb[64] = {}, wfile[MAX_PATH] = {};
    MultiByteToWideChar(CP_ACP, 0, verb, -1, wverb, ARRAYSIZE(wverb));
    MultiByteToWideChar(CP_ACP, 0, file, -1, wfile, ARRAYSIZE(wfile));
    return QueueContextAutoPlay(wverb, wfile);
}

static BOOL WINAPI ShellExecuteExAHook(SHELLEXECUTEINFOA* sei) {
    if (sei && QueueContextAutoPlayA(sei->lpVerb, sei->lpFile))
        return TRUE;
    return g_ShellExecuteExA_Original(sei);
}

static HINSTANCE WINAPI ShellExecuteAHook(HWND hwnd, LPCSTR verb,
                                           LPCSTR file, LPCSTR parameters,
                                           LPCSTR directory, INT show) {
    if (QueueContextAutoPlayA(verb, file))
        return (HINSTANCE)(INT_PTR)33;
    return g_ShellExecuteA_Original(hwnd, verb, file, parameters,
                                    directory, show);
}

static BOOL WINAPI ShellExecuteExWHook(SHELLEXECUTEINFOW* sei) {
    if (sei && QueueContextAutoPlay(sei->lpVerb,
                                    sei->lpFile ? sei->lpFile : sei->lpDirectory))
        return TRUE;
    return g_ShellExecuteExW_Original(sei);
}

static HINSTANCE WINAPI ShellExecuteWHook(HWND hwnd, LPCWSTR verb,
                                           LPCWSTR file, LPCWSTR parameters,
                                           LPCWSTR directory, INT show) {
    if (QueueContextAutoPlay(verb, file))
        return (HINSTANCE)(INT_PTR)33;
    return g_ShellExecuteW_Original(hwnd, verb, file, parameters,
                                    directory, show);
}

// Some Explorer builds invoke the drive context-menu verb directly through
// TrackPopupMenuEx, without calling ShellExecute(Ex) at all. Catch the command
// returned by that menu only when it contains an AutoPlay item.
static decltype(&TrackPopupMenuEx) g_TrackPopupMenuEx_Original = nullptr;
static decltype(&TrackPopupMenu) g_TrackPopupMenu_Original = nullptr;

static bool MenuTextIsAutoPlay(HMENU menu, UINT pos) {
    WCHAR text[256] = {};
    if (!GetMenuStringW(menu, pos, text, ARRAYSIZE(text), MF_BYPOSITION))
        return false;
    return StrStrIW(text, L"autoplay") != nullptr ||
           StrStrIW(text, L"auto play") != nullptr ||
           StrStrIW(text, L"riproduzione automatica") != nullptr;
}

static bool MenuContainsAutoPlay(HMENU menu, UINT* commandId, int depth = 0) {
    if (!menu || depth > 4) return false;
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        UINT id = GetMenuItemID(menu, i);
        HMENU sub = GetSubMenu(menu, i);
        if (sub) {
            if (MenuContainsAutoPlay(sub, commandId, depth + 1)) return true;
        } else if (id != (UINT)-1 && MenuTextIsAutoPlay(menu, (UINT)i)) {
            *commandId = id;
            return true;
        }
    }
    return false;
}

static bool ContextTargetIsEligible(int letter, bool allowFixed) {
    if (letter < L'A' || letter > L'Z' || IsSystemDriveLetter(letter))
        return false;
    std::wstring root = GetDriveRootForLetter(letter);
    UINT type = GetDriveTypeW(root.c_str());
    if (!DrivePresent(letter)) return false;
    if (!VolumeAllowedByPolicy(letter, type)) return false;
    // Normal AutoPlay targets follow DriveTypeWeHandle(). But the manual
    // "Open AutoPlay..." action was explicitly requested for the *clicked*
    // drive, so a fixed disk (e.g. Z: = host OS partition) must be honored
    // even when includeFixedDrives is off, otherwise the mod cannot show it.
    if (DriveTypeWeHandle(type, root.c_str())) return true;
    return allowFixed && type == DRIVE_FIXED;
}

static int FindContextAutoPlayDrive(int preferredLetter = 0) {
    // A drive supplied by the context-menu object always wins, even a fixed
    // one (the user explicitly asked to open AutoPlay for that drive).
    if (preferredLetter && ContextTargetIsEligible(preferredLetter, true))
        return preferredLetter;

    int found = 0;
    int autorun = 0;
    for (int i = 0; i < 26; ++i) {
        int letter = L'A' + i;
        if (letter == preferredLetter || !ContextTargetIsEligible(letter, false))
            continue;
        std::wstring root = GetDriveRootForLetter(letter);
        if (GetFileAttributesW((root + L"autorun.inf").c_str()) != INVALID_FILE_ATTRIBUTES)
            autorun = letter;
        if (found) {
            // The context entry is normally created by autorun.inf. Prefer
            // that volume over an unrelated removable drive.
            if (autorun) return autorun;
            return 0;
        }
        found = letter;
    }
    return autorun ? autorun : found;
}

static BOOL WINAPI TrackPopupMenuExHook(HMENU menu, UINT flags, int x, int y,
                                        HWND owner, LPTPMPARAMS params) {
    UINT autoplayId = 0;
    bool hasAutoPlay = MenuContainsAutoPlay(menu, &autoplayId);
    BOOL result = g_TrackPopupMenuEx_Original(menu, flags, x, y, owner, params);
    if (hasAutoPlay && (flags & TPM_RETURNCMD) &&
        (UINT)result == autoplayId) {
        int letter = FindContextAutoPlayDrive();
        if (letter && g_hwndListener) {
            PostMessageW(g_hwndListener, WMU_CONTEXT_AUTOPLAY,
                         (WPARAM)letter, 0);
            Wh_Log(L"Context AutoPlay: intercepted menu command %u for %c:",
                   autoplayId, letter);
            // TPM_RETURNCMD returns the command ID through the BOOL return
            // value. Return zero so Explorer does not execute the modern verb.
            return FALSE;
        }
    }
    return result;
}

static BOOL WINAPI TrackPopupMenuHook(HMENU menu, UINT flags, int x, int y,
                                      int reserved, HWND owner, const RECT* exclude) {
    UINT autoplayId = 0;
    bool hasAutoPlay = MenuContainsAutoPlay(menu, &autoplayId);
    BOOL result = g_TrackPopupMenu_Original(menu, flags, x, y, reserved,
                                            owner, exclude);
    if (hasAutoPlay && (flags & TPM_RETURNCMD) &&
        (UINT)result == autoplayId) {
        int letter = FindContextAutoPlayDrive();
        if (letter && g_hwndListener) {
            PostMessageW(g_hwndListener, WMU_CONTEXT_AUTOPLAY,
                         (WPARAM)letter, 0);
            Wh_Log(L"Context AutoPlay: intercepted menu command %u for %c:",
                   autoplayId, letter);
            return FALSE;
        }
    }
    return result;
}

// The drive menu is a CDefFolderMenu and recent Explorer versions invoke
// IContextMenu::InvokeCommand directly. Track its AutoPlay command ID instead
// of relying on ShellExecute or TrackPopupMenu.
struct ContextMenuInfo { IContextMenu* menu; int letter; UINT autoPlayOffset; };
static const UINT kNoAutoPlayCommand = UINT_MAX;
static std::vector<ContextMenuInfo> g_contextMenus;
// Menus obtained during the startup warm-up are kept alive for the whole
// session so the letter->menu association stays valid even if Explorer reuses
// a cached drive-menu object (created before the mod loaded).
static std::vector<IContextMenu*> g_warmupMenus;
static CRITICAL_SECTION g_csContextMenus;
static bool g_csContextMenusInit = false;
static void EnsureContextMenusCS() {
    if (!g_csContextMenusInit) { InitializeCriticalSection(&g_csContextMenus); g_csContextMenusInit = true; }
}
static HRESULT (STDMETHODCALLTYPE* g_ContextQueryOriginal)(IContextMenu*, HMENU, UINT, UINT, UINT, UINT) = nullptr;
static HRESULT (STDMETHODCALLTYPE* g_ContextInvokeOriginal)(IContextMenu*, LPCMINVOKECOMMANDINFO) = nullptr;
static HRESULT (STDMETHODCALLTYPE* g_GetUIObjectOf_Original)(IShellFolder*, HWND, UINT, PCUITEMID_CHILD_ARRAY, REFIID, UINT*, void**) = nullptr;

// Stored entries own a reference to their IContextMenu object (AddRef on
// insert) so the pointer can never dangle, and everything is accessed under the
// single lock. Callers always copy the fields out through SnapshotContextMenu
// instead of receiving a pointer into the vector (which could be invalidated by
// a concurrent reallocation). Growth is bounded to kMaxContextMenuEntries by
// dropping + releasing the oldest entries, so repeated right-clicks cannot grow
// the table without limit.
static const size_t kMaxContextMenuEntries = 64;

struct ContextMenuSnapshot { int letter = 0; UINT autoPlayOffset = kNoAutoPlayCommand; };

static bool SnapshotContextMenu(IContextMenu* menu, ContextMenuSnapshot* snap) {
    EnsureContextMenusCS();
    ApScopedCriticalSection lock(&g_csContextMenus);
    for (auto& i : g_contextMenus) {
        if (i.menu == menu) {
            if (snap) { snap->letter = i.letter; snap->autoPlayOffset = i.autoPlayOffset; }
            return true;
        }
    }
    return false;
}

static void SetContextMenuLetter(IContextMenu* menu, int letter) {
    if (!menu) return;
    EnsureContextMenusCS();
    ApScopedCriticalSection lock(&g_csContextMenus);
    for (auto& i : g_contextMenus) {
        if (i.menu == menu) { i.letter = letter; return; }
    }
    // Own the object so the stored pointer stays valid for the entry's life.
    menu->AddRef();
    g_contextMenus.push_back({ menu, letter, kNoAutoPlayCommand });
    while (g_contextMenus.size() > kMaxContextMenuEntries) {
        ContextMenuInfo& oldest = g_contextMenus.front();
        if (oldest.menu) oldest.menu->Release();
        g_contextMenus.erase(g_contextMenus.begin());
    }
}

static void SetContextMenuAutoPlayOffset(IContextMenu* menu, UINT offset) {
    if (!menu) return;
    EnsureContextMenusCS();
    ApScopedCriticalSection lock(&g_csContextMenus);
    for (auto& i : g_contextMenus) {
        if (i.menu == menu) { i.autoPlayOffset = offset; return; }
    }
}

static void ReleaseAllContextMenuEntries() {
    EnsureContextMenusCS();
    ApScopedCriticalSection lock(&g_csContextMenus);
    for (auto& i : g_contextMenus)
        if (i.menu) i.menu->Release();
    g_contextMenus.clear();
}

// Derive a "X:\" path from a shell folder child item. This is the reliable
// boundary at which a drive's identity is still known, unlike the vtable-only
// IContextMenu path (whose CDefFolderMenu_Create2 hook does not fire in some
// Win10/11 builds - which is exactly why "mapped=0" and the mod guessed the
// wrong drive / showed the native AutoPlay).
static std::wstring GetPathFromChild(IShellFolder* parent, PCUITEMID_CHILD child) {
    if (!parent || !child) return L"";
    STRRET sr = {};
    if (FAILED(parent->GetDisplayNameOf(child, SHGDN_FORPARSING, &sr)))
        return L"";
    WCHAR buf[MAX_PATH] = {};
    if (FAILED(StrRetToBufW(&sr, child, buf, ARRAYSIZE(buf))))
        return L"";
    return buf;
}

// IShellFolder::GetUIObjectOf is what Explorer calls on the enclosing folder to
// obtain each drive's IContextMenu. We capture the real drive here so the map is
// populated on every build, then InvokeCommand never has to guess.
static HRESULT STDMETHODCALLTYPE GetUIObjectOfHook(
    IShellFolder* parent, HWND hwnd, UINT cidl, PCUITEMID_CHILD_ARRAY items,
    REFIID riid, UINT* reserved, void** ppv) {
    HRESULT hr = S_OK;
    try {
        hr = g_GetUIObjectOf_Original(parent, hwnd, cidl, items, riid, reserved, ppv);
        if (SUCCEEDED(hr) && ppv && *ppv && riid == IID_IContextMenu &&
            cidl == 1 && items && items[0]) {
            std::wstring path = GetPathFromChild(parent, items[0]);
            if (path.size() >= 2 && path[1] == L':') {
                int letter = (int)towupper((wint_t)path[0]);
                // Takes the lock internally and owns the menu reference.
                SetContextMenuLetter((IContextMenu*)*ppv, letter);
                Wh_Log(L"GetUIObjectOfHook: menu=%p -> %s -> %c:",
                       *ppv, path.c_str(), letter);
            } else {
                Wh_Log(L"GetUIObjectOfHook: menu=%p no drive path (path='%s')",
                       *ppv, path.c_str());
            }
        }
    } catch (...) {
        // A shell hook must never let a C++ exception cross back into Explorer.
        // If the CSi section is left held by an exception, we guard the leak
        // message but the mapping just won't be added.
        Wh_Log(L"GetUIObjectOfHook: exception");
    }
    return hr;
}

// Startup warm-up: enumerate the drives that are already mounted and pre-build
// their context menus so every letter is associated before the user right-clicks.
// Nothing is shown; the menus are kept alive for the session. This makes the
// mapping robust even if Explorer had cached the drive menus before the mod
// loaded (otherwise the first right-click could hit an unmapped object and the
// mod would fall back to guessing or let native AutoPlay run).
static void WarmUpDriveContextMenus() {
    // RAII for COM: the CoUninitialize() runs on every exit path. Failure is
    // tolerated (RPC_E_CHANGED_MODE means COM is already initialised).
    ApScopedCoInit coInit;
    if (!coInit.available()) return;

    try {
        int associated = 0;
        WCHAR drives[512] = {};
        if (GetLogicalDriveStringsW(ARRAYSIZE(drives) - 1, drives)) {
            for (WCHAR* p = drives; *p; p += wcslen(p) + 1) {
                UINT dt = GetDriveTypeW(p);
                int letter = (int)towupper((wint_t)p[0]);
                if (p[1] != L':' || !DrivePresent(letter) || IsSystemDriveLetter(letter))
                    continue;
                if (!DriveTypeWeHandle(dt, p)) continue;

                PIDLIST_ABSOLUTE pidl = nullptr;
                ApComPtr<IShellFolder> parent;   // released automatically
                PCUITEMID_CHILD child = nullptr;
                ApComPtr<IContextMenu> menu;     // released, unless adopted
                HRESULT hr = SHParseDisplayName(p, nullptr, &pidl, 0, nullptr);
                if (SUCCEEDED(hr))
                    hr = SHBindToParent(pidl, IID_IShellFolder,
                                        (void**)parent.put(), &child);
                if (SUCCEEDED(hr) && parent)
                    hr = parent->GetUIObjectOf(nullptr, 1, &child, IID_IContextMenu,
                                               nullptr, (void**)menu.put());
                if (SUCCEEDED(hr) && menu) {
                    // Locked + owns a reference to the menu object.
                    SetContextMenuLetter(menu.get(), letter);
                    // Transfer ownership to the session-lifetime list so the menu
                    // stays alive and the letter mapping remains valid. Held on
                    // this (COM-initialised) thread and released on it too.
                    IContextMenu* rawMenu = menu.get();
                    g_warmupMenus.push_back(menu.release());
                    ++associated;
                    Wh_Log(L"WarmUpDriveContextMenus: associated %s menu=%p",
                           p, rawMenu);
                }
                if (pidl) CoTaskMemFree(pidl);
            }
        }
        Wh_Log(L"WarmUpDriveContextMenus: completed, associated=%d", associated);
    } catch (...) {
        Wh_Log(L"WarmUpDriveContextMenus: exception");
    }
}

// Release the session-lifetime menus acquired by the warm-up. Each menu already
// owns a reference in g_contextMenus (SetContextMenuLetter AddRef'd it), so this
// only drops the extra warm-up reference; the association stays valid. Must run
// on the same COM-initialised thread that created them (item 3).
static void ReleaseContextMenuWarmupMenus() {
    for (IContextMenu* m : g_warmupMenus)
        if (m) m->Release();
    g_warmupMenus.clear();
}

// Drop and release all entries belonging to a removed drive letter, so the table
// stays bounded and no reference outlives the volume it describes.
static void ReleaseContextMenuEntriesForLetter(int letter) {
    if (!letter) return;
    EnsureContextMenusCS();
    ApScopedCriticalSection lock(&g_csContextMenus);
    for (auto it = g_contextMenus.begin(); it != g_contextMenus.end(); ) {
        if (it->letter == letter) {
            if (it->menu) it->menu->Release();
            it = g_contextMenus.erase(it);
        } else {
            ++it;
        }
    }
}

static HRESULT STDMETHODCALLTYPE ContextQueryHook(IContextMenu* menu, HMENU hmenu,
                                                    UINT index, UINT first, UINT last,
                                                    UINT flags) {
    HRESULT hr = g_ContextQueryOriginal(menu, hmenu, index, first, last, flags);
    ContextMenuSnapshot snap;
    bool mapped = SnapshotContextMenu(menu, &snap);
    Wh_Log(L"Context menu QueryContextMenu self=%p first=%u last=%u mapped=%d",
           menu, first, last, mapped ? 1 : 0);
    if (SUCCEEDED(hr) && mapped) {
        int n = GetMenuItemCount(hmenu);
        for (int i = 0; i < n; ++i) {
            WCHAR text[256] = {};
            GetMenuStringW(hmenu, i, text, ARRAYSIZE(text), MF_BYPOSITION);
            if (StrStrIW(text, L"autoplay") || StrStrIW(text, L"auto play") ||
                StrStrIW(text, L"riproduzione automatica")) {
                UINT id = GetMenuItemID(hmenu, i);
                if (id != (UINT)-1 && id >= first && id <= last)
                    SetContextMenuAutoPlayOffset(menu, id - first);
            }
        }
    }
    return hr;
}

static bool ContextInvocationIsAutoPlay(IContextMenu* menu,
                                             LPCMINVOKECOMMANDINFO ci) {
    if (!menu || !ci) return false;
    if (!IS_INTRESOURCE(ci->lpVerb) && ci->lpVerb &&
        !_stricmp((LPCSTR)ci->lpVerb, "autoplay")) return true;
    if (IS_INTRESOURCE(ci->lpVerb)) {
        UINT_PTR offset = LOWORD((ULONG_PTR)ci->lpVerb);
        WCHAR wide[64] = {};
        if (SUCCEEDED(menu->GetCommandString(offset, GCS_VERBW, nullptr,
                                             (LPSTR)wide, ARRAYSIZE(wide))) &&
            !_wcsicmp(wide, L"autoplay")) return true;
        char narrow[64] = {};
        if (SUCCEEDED(menu->GetCommandString(offset, GCS_VERBA, nullptr,
                                             narrow, ARRAYSIZE(narrow))) &&
            !_stricmp(narrow, "autoplay")) return true;
    }
    return false;
}

static HRESULT STDMETHODCALLTYPE ContextInvokeHook(IContextMenu* menu,
                                                    LPCMINVOKECOMMANDINFO ci) {
    ContextMenuSnapshot snap;
    bool mapped = SnapshotContextMenu(menu, &snap);
    Wh_Log(L"Context menu InvokeCommand self=%p mapped=%d", menu, mapped ? 1 : 0);
    if (!ci || !g_hwndListener) return g_ContextInvokeOriginal(menu, ci);

    // Cheap checks first: is this an AutoPlay command at all? Only then do we
    // do the (expensive) eligible-drive scan. The canonical "autoplay" verb is
    // language-independent, so prefer it over localized menu text (item 4/6).
    bool known = mapped && snap.autoPlayOffset != kNoAutoPlayCommand;
    bool isAutoPlay =
        (known && IS_INTRESOURCE(ci->lpVerb) &&
         LOWORD((ULONG_PTR)ci->lpVerb) == snap.autoPlayOffset) ||
        ContextInvocationIsAutoPlay(menu, ci);
    if (!isAutoPlay)
        return g_ContextInvokeOriginal(menu, ci);

    // Use the drive letter captured for this menu object if available; only
    // fall back to scanning when we truly have no mapping for it.
    int letter = mapped ? FindContextAutoPlayDrive(snap.letter)
                        : FindContextAutoPlayDrive();
    if (letter) {
        PostMessageW(g_hwndListener, WMU_CONTEXT_AUTOPLAY,
                     (WPARAM)letter, 0);
        Wh_Log(L"Context AutoPlay: intercepted IContextMenu::InvokeCommand for %c:", letter);
        return S_OK;
    }
    return g_ContextInvokeOriginal(menu, ci);
}

static bool InstallAutoplayContextMenuHooks() {
    // Probe the shell's own AutoPlay COM class, like the Open With mod does.
    // This covers Explorer builds which don't use CDefFolderMenu_Create2.
    static const CLSID kClsidAutoPlay =
        { 0x9C60DE1E, 0xE5FC, 0x40F4, { 0xA4, 0x87, 0x46, 0x08, 0x51, 0xA8, 0xD9, 0x15 } };
    ApScopedCoInit coInit;
    if (!coInit.available()) return false;
    IContextMenu* menu = nullptr;
    HRESULT hr = CoCreateInstance(kClsidAutoPlay, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_IContextMenu, (void**)&menu);
    if (FAILED(hr) || !menu) {
        Wh_Log(L"AutoPlay context COM probe failed hr=0x%08X", (unsigned)hr);
        return false;
    }
    void** vt = *(void***)menu;
    auto query = (decltype(g_ContextQueryOriginal))vt[3];
    auto invoke = (decltype(g_ContextInvokeOriginal))vt[4];
    bool q = query && Wh_SetFunctionHook((void*)query, (void*)ContextQueryHook,
                                         (void**)&g_ContextQueryOriginal);
    bool i = invoke && Wh_SetFunctionHook((void*)invoke, (void*)ContextInvokeHook,
                                           (void**)&g_ContextInvokeOriginal);
    Wh_Log(L"AutoPlay context COM probe query=%p invoke=%p hooks=%d/%d",
           query, invoke, q, i);
    menu->Release();
    return q && i;
}

// The drive-letter association must be built regardless of which IContextMenu
// implementation gets hooked. Explorer obtains each drive's context menu from
// the "This PC" (CSIDL_DRIVES) shell folder via IShellFolder::GetUIObjectOf, so
// hooking that folder's GetUIObjectOf (vtable slot 10) captures every real drive
// menu. This is installed unconditionally in Wh_ModInit, independently of the
// AutoPlay COM probe path (item 5).
static bool g_getUIObjectOfHookInstalled = false;
static bool InstallDriveFolderGetUIObjectHook() {
    if (g_getUIObjectOfHookInstalled) return true;

    WCHAR drives[512] = {};
    if (!GetLogicalDriveStringsW(ARRAYSIZE(drives) - 1, drives)) return false;
    WCHAR root[4] = {};
    for (WCHAR* p = drives; *p; p += wcslen(p) + 1) {
        if (GetDriveTypeW(p) == DRIVE_NO_ROOT_DIR) continue;
        wcscpy_s(root, p);
        break;
    }
    if (!root[0]) return false;

    ApScopedCoInit coInit;
    if (!coInit.available()) return false;
    PIDLIST_ABSOLUTE pidl = nullptr;
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    HRESULT hr = SHParseDisplayName(root, nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr))
        hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&parent, &child);
    bool ok = false;
    if (SUCCEEDED(hr) && parent) {
        void** pvt = *(void***)parent;
        auto guiObj = pvt ? (decltype(g_GetUIObjectOf_Original))pvt[10] : nullptr;
        ok = guiObj && Wh_SetFunctionHook((void*)guiObj,
                                          (void*)GetUIObjectOfHook,
                                          (void**)&g_GetUIObjectOf_Original);
    }
    Wh_Log(L"Drive folder GetUIObjectOf hook installed=%d", ok ? 1 : 0);
    if (parent) parent->Release();
    if (pidl) CoTaskMemFree(pidl);
    if (ok) g_getUIObjectOfHookInstalled = true;
    return ok;
}

static bool InstallDriveContextMenuHooks() {
    // Obtain the same drive context-menu implementation Explorer uses, then
    // hook its real IContextMenu vtable. This is the boundary at which the
    // selected menu item is still identified by its canonical verb.
    WCHAR drives[512] = {};
    if (!GetLogicalDriveStringsW(ARRAYSIZE(drives) - 1, drives)) return false;
    WCHAR root[4] = {};
    for (WCHAR* p = drives; *p; p += wcslen(p) + 1) {
        if (GetDriveTypeW(p) == DRIVE_NO_ROOT_DIR) continue;
        wcscpy_s(root, p);
        break;
    }
    if (!root[0]) return false;

    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool uninit = SUCCEEDED(co);
    if (FAILED(co) && co != RPC_E_CHANGED_MODE) return false;
    PIDLIST_ABSOLUTE pidl = nullptr;
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    IContextMenu* menu = nullptr;
    HRESULT hr = SHParseDisplayName(root, nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr))
        hr = SHBindToParent(pidl, IID_IShellFolder,
                            (void**)&parent, &child);
    if (SUCCEEDED(hr))
        hr = parent->GetUIObjectOf(nullptr, 1, &child, IID_IContextMenu,
                                   nullptr, (void**)&menu);
    bool ok = false;
    if (SUCCEEDED(hr) && menu) {
        void** vt = *(void***)menu;
        auto query = (decltype(g_ContextQueryOriginal))vt[3];
        auto invoke = (decltype(g_ContextInvokeOriginal))vt[4];
        bool q = query && Wh_SetFunctionHook((void*)query,
                                             (void*)ContextQueryHook,
                                             (void**)&g_ContextQueryOriginal);
        bool i = invoke && Wh_SetFunctionHook((void*)invoke,
                                               (void*)ContextInvokeHook,
                                               (void**)&g_ContextInvokeOriginal);
        Wh_Log(L"Drive AutoPlay context probe root=%s query=%p invoke=%p hooks=%d/%d",
               root, query, invoke, q, i);
        ok = q && i;
    } else {
        Wh_Log(L"Drive AutoPlay context probe failed hr=0x%08X root=%s",
               (unsigned)hr, root);
    }
    if (menu) menu->Release();
    if (parent) parent->Release();
    if (pidl) CoTaskMemFree(pidl);
    if (uninit) CoUninitialize();
    EnsureContextMenusCS();
    return ok;
}

static HRESULT (WINAPI* g_CDefCreateOriginal)(LPCITEMIDLIST, HWND, UINT, PCUITEMID_CHILD_ARRAY, IShellFolder*, LPFNDFMCALLBACK, UINT, HKEY*, IContextMenu**) = nullptr;

static HRESULT WINAPI CDefFolderMenuCreate2Hook(
    LPCITEMIDLIST folder, HWND hwnd, UINT cidl, PCUITEMID_CHILD_ARRAY items,
    IShellFolder* sf, LPFNDFMCALLBACK callback, UINT keys, HKEY* hkeys,
    IContextMenu** outMenu) {
    HRESULT hr = g_CDefCreateOriginal(folder, hwnd, cidl, items, sf,
                                       callback, keys, hkeys, outMenu);
    // Diagnose whether this hook actually fires and can derive a drive letter.
    Wh_Log(L"CDefFolderMenuCreate2Hook fired hr=0x%08X cidl=%u menu=%p",
           (unsigned)hr, cidl, outMenu && *outMenu ? *outMenu : nullptr);
    if (SUCCEEDED(hr) && outMenu && *outMenu) {
        // A drive's own context menu is created with cidl == 1 and a single
        // volume child. Do not require cidl == 1 strictly: some Explorer builds
        // create the drive menu with additional items. Try every item until a
        // "X:\" path is found, and fall back to the folder itself.
        int foundLetter = 0;
        UINT itemsToTry = (cidl == 0) ? 1 : cidl;
        for (UINT k = 0; k < itemsToTry && !foundLetter; ++k) {
            LPCITEMIDLIST item = items ? items[k] : nullptr;
            if (!item) continue;
            WCHAR path[MAX_PATH] = {};
            if (folder) {
                PIDLIST_ABSOLUTE abs = ILCombine(folder, item);
                if (abs) { SHGetPathFromIDListW(abs, path); CoTaskMemFree(abs); }
            }
            if (!path[0]) SHGetPathFromIDListW(item, path);
            if (path[0] && path[1] == L':' && path[2] == L'\\') {
                foundLetter = (int)towupper((wint_t)path[0]);
                Wh_Log(L"CDefFolderMenuCreate2Hook: item %u -> %s -> %c:",
                       k, path, foundLetter);
                break;
            }
            // The path may be empty in the modern shell view; also check folder.
            if (!path[0] && folder && k == 0) {
                WCHAR folderPath[MAX_PATH] = {};
                if (SHGetPathFromIDListW(folder, folderPath) &&
                    folderPath[0] && folderPath[1] == L':' && folderPath[2] == L'\\') {
                    foundLetter = (int)towupper((wint_t)folderPath[0]);
                    Wh_Log(L"CDefFolderMenuCreate2Hook: folder -> %s -> %c:",
                           folderPath, foundLetter);
                    break;
                }
            }
        }
        if (foundLetter) {
            // Locked + owns a reference, so this is safe to call from any thread.
            SetContextMenuLetter(*outMenu, foundLetter);
        } else {
            Wh_Log(L"CDefFolderMenuCreate2Hook: could not derive a drive letter for menu=%p",
                   *outMenu);
        }
    }
    return hr;
}


static DWORD g_rotCookie = 0;
static IRunningObjectTable* g_rot = nullptr;
static DWORD g_classCookie = 0;
static bool g_cancelRegWritten = false;

static void WriteCancelAutoPlayClsid(bool add) {
    if (add) {
        ApRegKey key;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, kCancelClsidPath, 0, nullptr, 0,
                            KEY_SET_VALUE, nullptr, &key.h, nullptr) != ERROR_SUCCESS)
            return;
        const BYTE empty[] = { 0, 0 };
        RegSetValueExW(key.h, kCancelClsidBare, 0, REG_SZ, empty, sizeof(empty));
        RegSetValueExW(key.h, kCancelClsidBraces, 0, REG_SZ, empty, sizeof(empty));
        g_cancelRegWritten = true;
    } else {
        // Item 3 fix: delete unconditionally, not just when g_cancelRegWritten is
        // set. Wh_ModUninit does not run on Explorer restart / sign-out / reboot /
        // crash, so a leftover from a previous session must still be cleaned up
        // even though g_cancelRegWritten is false on a fresh load (it was only
        // ever overwritten before, never removed, in that case).
        ApRegKey key;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kCancelClsidPath, 0, KEY_SET_VALUE, &key.h) == ERROR_SUCCESS) {
            RegDeleteValueW(key.h, kCancelClsidBare);
            RegDeleteValueW(key.h, kCancelClsidBraces);
        }
        g_cancelRegWritten = false;
    }
}

static void UnregisterCancelAutoPlay() {
    if (g_rot && g_rotCookie) {
        g_rot->Revoke(g_rotCookie);
        g_rotCookie = 0;
    }
    if (g_rot) {
        g_rot->Release();
        g_rot = nullptr;
    }
    if (g_classCookie) {
        CoRevokeClassObject(g_classCookie);
        g_classCookie = 0;
    }
    WriteCancelAutoPlayClsid(false);
}

static void RegisterCancelAutoPlay() {
    UnregisterCancelAutoPlay();
    if (!g_settings.suppressNativeAutoPlay)
        return;

    WriteCancelAutoPlayClsid(true);

    CancelClassFactory* fac = new CancelClassFactory();
    HRESULT hr = CoRegisterClassObject(CLSID_Win7CancelAP, fac,
                                       CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
                                       REGCLS_MULTIPLEUSE, &g_classCookie);
    fac->Release();
    if (FAILED(hr)) {
        Wh_Log(L"RegisterCancelAutoPlay: CoRegisterClassObject failed 0x%08X", (unsigned)hr);
        g_classCookie = 0;
    }

    IMoniker* mon = nullptr;
    if (FAILED(CreateClassMoniker(CLSID_Win7CancelAP, &mon)) || !mon) {
        Wh_Log(L"RegisterCancelAutoPlay: CreateClassMoniker failed");
        return;
    }
    IRunningObjectTable* rot = nullptr;
    if (FAILED(GetRunningObjectTable(0, &rot)) || !rot) {
        mon->Release();
        return;
    }
    CancelAutoPlayObj* obj = new CancelAutoPlayObj();
    hr = rot->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, obj, mon, &g_rotCookie);
    obj->Release();
    mon->Release();
    if (FAILED(hr)) {
        Wh_Log(L"RegisterCancelAutoPlay: ROT failed 0x%08X", (unsigned)hr);
        rot->Release();
        g_rotCookie = 0;
        return;
    }
    g_rot = rot;
    Wh_Log(L"RegisterCancelAutoPlay: registered");
}

// ============================================================================
// MTP / WPD
// ============================================================================
static std::wstring WpdReadName(IPortableDeviceManagerWH* mgr, LPCWSTR id, int which) {
    WCHAR stack[256] = {};
    DWORD cch = ARRAYSIZE(stack);
    HRESULT hr = E_FAIL;
    if (which == 0) hr = mgr->GetDeviceFriendlyName(id, stack, &cch);
    else if (which == 1) hr = mgr->GetDeviceManufacturer(id, stack, &cch);
    else hr = mgr->GetDeviceDescription(id, stack, &cch);
    if (SUCCEEDED(hr) && stack[0])
        return stack;
    cch = 0;
    if (which == 0) hr = mgr->GetDeviceFriendlyName(id, nullptr, &cch);
    else if (which == 1) hr = mgr->GetDeviceManufacturer(id, nullptr, &cch);
    else hr = mgr->GetDeviceDescription(id, nullptr, &cch);
    if (FAILED(hr) || cch <= 1) return L"";
    std::wstring s(cch, L'\0');
    DWORD n = cch;
    if (which == 0) hr = mgr->GetDeviceFriendlyName(id, &s[0], &n);
    else if (which == 1) hr = mgr->GetDeviceManufacturer(id, &s[0], &n);
    else hr = mgr->GetDeviceDescription(id, &s[0], &n);
    if (FAILED(hr)) return L"";
    s.resize(wcslen(s.c_str()));
    return s;
}

static bool WpdIdsMatch(const std::wstring& a, const std::wstring& b) {
    if (a.empty() || b.empty()) return false;
    if (_wcsicmp(a.c_str(), b.c_str()) == 0) return true;
    auto tail = [](const std::wstring& s) {
        size_t pos = s.find(L"#{");
        return (pos == std::wstring::npos) ? s : s.substr(0, pos);
    };
    return _wcsicmp(tail(a).c_str(), tail(b).c_str()) == 0;
}

static bool CreatePdm(ApComPtr<IPortableDeviceManagerWH>& mgr) {
    HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager_, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IPortableDeviceManager_, (void**)mgr.put());
    return SUCCEEDED(hr) && mgr;
}

static bool ResolveWpdDevice(const std::wstring& devicePath,
                             std::wstring& outId, std::wstring& outName, std::wstring& outMfr) {
    outId.clear(); outName.clear(); outMfr.clear();
    try {
        ApComPtr<IPortableDeviceManagerWH> mgr;
        if (!CreatePdm(mgr)) return false;
        mgr->RefreshDeviceList();
        DWORD count = 0;
        mgr->GetDevices(nullptr, &count);
        bool found = false;
        if (count > 0) {
            ApCoTaskStrs bag;
            bag.ids.assign(count, nullptr);
            DWORD n = count;
            if (SUCCEEDED(mgr->GetDevices(bag.ids.data(), &n))) {
                for (DWORD i = 0; i < n; i++) {
                    if (!bag.ids[i] || found) continue;
                    if (devicePath.empty() || WpdIdsMatch(devicePath, bag.ids[i])) {
                        outId = bag.ids[i];
                        outName = WpdReadName(mgr.get(), bag.ids[i], 0);
                        outMfr  = WpdReadName(mgr.get(), bag.ids[i], 1);
                        if (outName.empty())
                            outName = WpdReadName(mgr.get(), bag.ids[i], 2);
                        found = !outId.empty();
                    }
                }
            }
        }
        if (!found && !devicePath.empty()) {
            outId = devicePath;
            found = true;
        }
        return found;
    } catch (...) {
        Wh_Log(L"ResolveWpdDevice: exception");
        return false;
    }
}

static bool WpdStillPresent(const std::wstring& idOrPath) {
    if (idOrPath.empty()) return false;
    try {
        ApComPtr<IPortableDeviceManagerWH> mgr;
        if (!CreatePdm(mgr)) return true;
        mgr->RefreshDeviceList();
        DWORD count = 0;
        mgr->GetDevices(nullptr, &count);
        bool found = false;
        if (count > 0) {
            ApCoTaskStrs bag;
            bag.ids.assign(count, nullptr);
            DWORD n = count;
            if (SUCCEEDED(mgr->GetDevices(bag.ids.data(), &n))) {
                for (DWORD i = 0; i < n; i++) {
                    if (bag.ids[i] && WpdIdsMatch(idOrPath, bag.ids[i])) found = true;
                }
            }
        }
        return found;
    } catch (...) {
        return true;
    }
}

static bool OpenWpdInExplorer(const std::wstring& friendly, const std::wstring& parseHint) {
    PIDLIST_ABSOLUTE pidlComputer = nullptr;
    if (FAILED(SHGetFolderLocation(NULL, CSIDL_DRIVES, NULL, 0, &pidlComputer)) || !pidlComputer)
        return false;
    IShellFolder* desktop = nullptr;
    if (FAILED(SHGetDesktopFolder(&desktop)) || !desktop) {
        CoTaskMemFree(pidlComputer);
        return false;
    }
    IShellFolder* computer = nullptr;
    HRESULT hr = desktop->BindToObject(pidlComputer, nullptr, IID_IShellFolder, (void**)&computer);
    desktop->Release();
    if (FAILED(hr) || !computer) {
        CoTaskMemFree(pidlComputer);
        return false;
    }
    IEnumIDList* en = nullptr;
    hr = computer->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_STORAGE | SHCONTF_NONFOLDERS, &en);
    bool opened = false;
    if (SUCCEEDED(hr) && en) {
        LPITEMIDLIST child = nullptr;
        ULONG fetched = 0;
        while (!opened && en->Next(1, &child, &fetched) == S_OK && child) {
            wchar_t display[MAX_PATH] = {}, parse[MAX_PATH * 2] = {};
            STRRET sr = {};
            if (SUCCEEDED(computer->GetDisplayNameOf(child, SHGDN_NORMAL, &sr)))
                StrRetToBufW(&sr, child, display, ARRAYSIZE(display));
            sr = {};
            if (SUCCEEDED(computer->GetDisplayNameOf(child, SHGDN_FORPARSING, &sr)))
                StrRetToBufW(&sr, child, parse, ARRAYSIZE(parse));
            bool match = false;
            if (friendly[0] && display[0] && _wcsicmp(display, friendly.c_str()) == 0)
                match = true;
            if (!match && !parseHint.empty() && parse[0] &&
                (WpdIdsMatch(parse, parseHint) || wcsstr(parse, parseHint.c_str())))
                match = true;
            if (match) {
                LPITEMIDLIST abs = ILCombine(pidlComputer, child);
                if (abs) {
                    SHELLEXECUTEINFOW sei = { sizeof(sei) };
                    sei.fMask = SEE_MASK_IDLIST | SEE_MASK_NOASYNC;
                    sei.lpIDList = abs;
                    sei.nShow = SW_SHOWNORMAL;
                    sei.lpVerb = L"explore";
                    opened = ShellExecuteExW(&sei) != FALSE;
                    if (!opened) {
                        sei.lpVerb = L"open";
                        opened = ShellExecuteExW(&sei) != FALSE;
                    }
                    CoTaskMemFree(abs);
                }
            }
            CoTaskMemFree(child);
            child = nullptr;
        }
        en->Release();
    }
    computer->Release();
    CoTaskMemFree(pidlComputer);
    if (!opened) {
        ShellExecuteW(NULL, L"open", L"explorer.exe", L"shell:MyComputerFolder",
                      nullptr, SW_SHOWNORMAL);
        opened = true;
    }
    return opened;
}

static bool CurrentTargetPresent() {
    if (g_isWpd)
        return WpdStillPresent(g_wpdId.empty() ? g_wpdPath : g_wpdId);
    return DrivePresent(g_driveLetter);
}

// ============================================================================
// ============================================================================
struct AutorunInfo {
    bool hasProgram = false;
    std::wstring programPath;
    std::wstring programArgs;
    std::wstring action;
    std::wstring label;
    std::wstring iconFile;
    int iconIndex = 0;
};

static std::wstring TrimQuotes(std::wstring s) {
    while (!s.empty() && (s.back() == L' ' || s.back() == L'\t' || s.back() == L'"'))
        s.pop_back();
    size_t start = 0;
    while (start < s.size() && (s[start] == L' ' || s[start] == L'\t' || s[start] == L'"'))
        start++;
    return s.substr(start);
}

static std::wstring MakeAbsolute(std::wstring path, const std::wstring& root) {
    WCHAR expanded[MAX_PATH * 2] = {};
    ExpandEnvironmentStringsW(path.c_str(), expanded, ARRAYSIZE(expanded));
    path = expanded;
    if (path.size() >= 2 && path[1] == L':') return path;
    if (path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\') return path;
    if (!path.empty() && (path[0] == L'\\' || path[0] == L'/'))
        return root.substr(0, 2) + path;
    return root + path;
}

static std::wstring InfGet(const std::wstring& inf, PCWSTR section, PCWSTR name) {
    WCHAR buf[1024] = {};
    GetPrivateProfileStringW(section, name, L"", buf, ARRAYSIZE(buf), inf.c_str());
    return TrimQuotes(buf);
}

static std::wstring PickAutorunOpenCommand(const std::wstring& inf) {
    auto useAp = [](const std::wstring& v) {
        return !v.empty() && _wtoi(v.c_str()) != 0;
    };
    if (useAp(InfGet(inf, L"autorun.amd64", L"UseAutoPlay")) ||
        useAp(InfGet(inf, L"autorun", L"UseAutoPlay")))
        return L"";

    const PCWSTR sections[] = {
#ifdef _WIN64
        L"autorun.amd64", L"AutoRun.Amd64",
#endif
        L"autorun", nullptr
    };
#ifdef _WIN64
    const PCWSTR archKeys[] = { L"open.amd64", L"open.Amd64", L"AutoRun.Amd64", nullptr };
    for (int s = 0; sections[s]; ++s) {
        for (int k = 0; archKeys[k]; ++k) {
            std::wstring v = InfGet(inf, sections[s], archKeys[k]);
            if (!v.empty()) return v;
        }
    }
#endif
    const PCWSTR openKeys[] = { L"open", L"AutoRun", L"shellexecute", nullptr };
    for (int s = 0; sections[s]; ++s) {
        for (int k = 0; openKeys[k]; ++k) {
            std::wstring v = InfGet(inf, sections[s], openKeys[k]);
            if (!v.empty()) return v;
        }
    }
    return L"";
}

static void ParseAutorunInf(const std::wstring& root, AutorunInfo& out) {
    std::wstring inf = root + L"autorun.inf";
    if (GetFileAttributesW(inf.c_str()) == INVALID_FILE_ATTRIBUTES) return;

    auto GetKey = [&](PCWSTR name) {
        std::wstring v;
#ifdef _WIN64
        v = InfGet(inf, L"autorun.amd64", name);
        if (v.empty()) v = InfGet(inf, L"AutoRun.Amd64", name);
#endif
        if (v.empty()) v = InfGet(inf, L"autorun", name);
        return v;
    };

    out.label = GetKey(L"label");

    std::wstring openCmd = PickAutorunOpenCommand(inf);

    if (!openCmd.empty()) {
        int argc = 0;
        LPWSTR* argv = CommandLineToArgvW(openCmd.c_str(), &argc);
        if (argv && argc > 0) {
            out.programPath = MakeAbsolute(TrimQuotes(argv[0]), root);
            std::wstring args;
            for (int i = 1; i < argc; i++) {
                if (!args.empty()) args += L' ';
                args += argv[i];
            }
            out.programArgs = args;
        }
        if (argv) LocalFree(argv);
    }
    out.hasProgram = !out.programPath.empty() &&
                     GetFileAttributesW(out.programPath.c_str()) != INVALID_FILE_ATTRIBUTES &&
                     PathIsUnderRoot(out.programPath, root);

    out.action = GetKey(L"action");

    std::wstring iconSpec = GetKey(L"icon");
    if (!iconSpec.empty()) {
        int idx = 0;
        size_t comma = iconSpec.find_last_of(L',');
        if (comma != std::wstring::npos && comma + 1 < iconSpec.size()) {
            std::wstring num = iconSpec.substr(comma + 1);
            bool numeric = !num.empty();
            for (wchar_t c : num) numeric = numeric && (iswdigit(c) != 0);
            if (numeric) { idx = _wtoi(num.c_str()); iconSpec = iconSpec.substr(0, comma); }
        }
        out.iconIndex = idx;
        out.iconFile = MakeAbsolute(iconSpec, root);
        if (GetFileAttributesW(out.iconFile.c_str()) == INVALID_FILE_ATTRIBUTES)
            out.iconFile.clear();
    }
}

// ============================================================================
// ============================================================================
static std::wstring ClassKey(ContentKind kind, bool hasProgram, UINT driveType) {
    if (hasProgram || kind == ContentKind::Software) return L"software";
    if (kind == ContentKind::AudioCD || kind == ContentKind::DvdMovie ||
        kind == ContentKind::BlankDisc || driveType == DRIVE_CDROM) return L"cd";
    if (kind == ContentKind::Pictures) return L"pictures";
    if (kind == ContentKind::Music) return L"music";
    if (kind == ContentKind::Video) return L"video";
    if (kind == ContentKind::Mixed) return L"mixed";
    if (kind == ContentKind::Portable || g_isWpd) return L"wpd";
    return L"usb";
}

static std::wstring ReadRemembered(const std::wstring& cls) {
    WCHAR name[64] = {};
    swprintf_s(name, ARRAYSIZE(name), L"remember_%s", cls.c_str());
    WCHAR value[512] = {};
    if (Wh_GetStringValue(name, value, ARRAYSIZE(value)) <= 0) return L"";
    return value;
}

static void WriteRemembered(const std::wstring& cls, const std::wstring& token) {
    WCHAR name[64] = {};
    swprintf_s(name, ARRAYSIZE(name), L"remember_%s", cls.c_str());
    Wh_SetStringValue(name, token.c_str());
}

// ============================================================================
// ============================================================================
static bool LooksLikeAudioCd(const std::wstring& root) {
    WIN32_FIND_DATAW fd = {};
    HANDLE h = FindFirstFileW((root + L"Track*.cda").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) { FindClose(h); return true; }
    h = FindFirstFileW((root + L"*.cda").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) { FindClose(h); return true; }
    return false;
}

static bool PathIsDirOrFile(const std::wstring& path) {
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

static bool LooksLikeDvdMovie(const std::wstring& root) {
    return PathIsDirOrFile(root + L"VIDEO_TS\\VIDEO_TS.IFO") ||
           PathIsDirOrFile(root + L"VIDEO_TS\\video_ts.ifo") ||
           PathIsDirOrFile(root + L"VIDEO_TS") ||
           PathIsDirOrFile(root + L"video_ts");
}

static bool LooksLikeBluray(const std::wstring& root) {
    return PathIsDirOrFile(root + L"BDMV") || PathIsDirOrFile(root + L"bdmv");
}

static bool LooksLikeBlankOptical(const std::wstring& root, UINT driveType, bool empty) {
    if (driveType != DRIVE_CDROM || !empty) return false;
    DWORD flags = 0;
    WCHAR fs[MAX_PATH] = {};
    if (!GetVolumeInformationW(root.c_str(), nullptr, 0, nullptr, nullptr, &flags, fs, MAX_PATH))
        return true;
    if (flags & FILE_READ_ONLY_VOLUME) return false;
    return true;
}

static bool IsSkipDirName(const wchar_t* name) {
    return !_wcsicmp(name, L"$RECYCLE.BIN") ||
           !_wcsicmp(name, L"System Volume Information") ||
           !_wcsicmp(name, L"RECYCLER") ||
           !_wcsicmp(name, L"RECYCLED"); // was a duplicate "System Volume Information" check
}

static bool ExtIs(const wchar_t* ext, const wchar_t* const* list) {
    if (!ext || !*ext) return false;
    for (; *list; ++list)
        if (_wcsicmp(ext, *list) == 0) return true;
    return false;
}

static const wchar_t* kPicExt[] = {
    L".jpg", L".jpeg", L".png", L".bmp", L".gif", L".tif", L".tiff",
    L".jfif", L".heic", L".webp", nullptr
};
static const wchar_t* kAudExt[] = {
    L".mp3", L".wav", L".wma", L".aac", L".flac", L".m4a", L".ogg", L".cda", nullptr
};
static const wchar_t* kVidExt[] = {
    L".mp4", L".avi", L".wmv", L".mkv", L".mov", L".mpg", L".mpeg", L".m4v", nullptr
};

struct MediaInventory {
    int files = 0;
    int pictures = 0;
    int audio = 0;
    int video = 0;
    bool empty = true;
    std::wstring firstPicture;
    std::wstring firstAudio;
    std::wstring firstVideo;
};

static void ScanDirLimited(const std::wstring& dir, int depth, MediaInventory& inv,
                           ULONGLONG deadline) {
    if (inv.files >= 64 || depth > 2) return;
    if (GetTickCount64() > deadline) return;

    WIN32_FIND_DATAW fd = {};
    HANDLE h = FindFirstFileW((dir + L"*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (GetTickCount64() > deadline || inv.files >= 64) break;
        if (fd.cFileName[0] == L'.' &&
            (fd.cFileName[1] == 0 || (fd.cFileName[1] == L'.' && fd.cFileName[2] == 0)))
            continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (depth < 2 && !IsSkipDirName(fd.cFileName))
                ScanDirLimited(dir + fd.cFileName + L"\\", depth + 1, inv, deadline);
            continue;
        }
        inv.files++;
        inv.empty = false;
        const wchar_t* ext = PathFindExtensionW(fd.cFileName);
        std::wstring full = dir + fd.cFileName;
        if (ExtIs(ext, kPicExt)) {
            inv.pictures++;
            if (inv.firstPicture.empty()) inv.firstPicture = full;
        } else if (ExtIs(ext, kAudExt)) {
            inv.audio++;
            if (inv.firstAudio.empty()) inv.firstAudio = full;
        } else if (ExtIs(ext, kVidExt)) {
            inv.video++;
            if (inv.firstVideo.empty()) inv.firstVideo = full;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static MediaInventory ScanVolumeShallow(const std::wstring& root) {
    MediaInventory inv;
    ScanDirLimited(root, 0, inv, GetTickCount64() + 180);
    return inv;
}

static bool IsReadyBoostEligible(const std::wstring& root, UINT driveType) {
    if (!g_settings.includeReadyBoost) return false;
    if (driveType != DRIVE_REMOVABLE) return false;
    ULARGE_INTEGER total = {}, freeB = {};
    if (!GetDiskFreeSpaceExW(root.c_str(), nullptr, &total, &freeB))
        return false;
    if (total.QuadPart < 256ULL * 1024 * 1024) return false;
    if (freeB.QuadPart < 64ULL * 1024 * 1024) return false;
    DWORD serial = 0, maxComp = 0, flags = 0;
    WCHAR fs[MAX_PATH] = {};
    if (!GetVolumeInformationW(root.c_str(), nullptr, 0, &serial, &maxComp, &flags, fs, MAX_PATH))
        return false;
    if (flags & FILE_READ_ONLY_VOLUME) return false;
    if (fs[0] && (!_wcsicmp(fs, L"CDFS") || !_wcsicmp(fs, L"UDF"))) return false;
    return true;
}

static ContentKind ClassifyContent(bool audioCd, bool hasProgram, UINT driveType,
                                   const MediaInventory& inv) {
    if (audioCd) return ContentKind::AudioCD;
    if (LooksLikeDvdMovie(g_driveRoot) || LooksLikeBluray(g_driveRoot))
        return ContentKind::DvdMovie;
    if (hasProgram) return ContentKind::Software;
    if (inv.empty) {
        if (LooksLikeBlankOptical(g_driveRoot, driveType, true))
            return ContentKind::BlankDisc;
        return ContentKind::Empty;
    }
    int kinds = (inv.pictures > 0) + (inv.audio > 0) + (inv.video > 0);
    if (kinds >= 2) return ContentKind::Mixed;
    if (inv.pictures > 0 && inv.audio == 0 && inv.video == 0 &&
        inv.pictures * 2 >= inv.files)
        return ContentKind::Pictures;
    if (inv.audio > 0 && inv.pictures == 0 && inv.video == 0)
        return ContentKind::Music;
    if (inv.video > 0 && inv.pictures == 0 && inv.audio == 0)
        return ContentKind::Video;
    if (driveType == DRIVE_CDROM) return ContentKind::DataDisc;
    return ContentKind::General;
}

static PCWSTR AlwaysText(const LangPack* lp) {
    switch (g_contentKind) {
        case ContentKind::Software: return lp->alwaysSoftware;
        case ContentKind::AudioCD:
        case ContentKind::DataDisc:
        case ContentKind::DvdMovie:
        case ContentKind::BlankDisc: return lp->alwaysCd;
        case ContentKind::Pictures: return lp->alwaysPictures;
        case ContentKind::Music:    return lp->alwaysMusic;
        case ContentKind::Video:    return lp->alwaysGeneral;
        case ContentKind::Mixed:    return lp->alwaysMixed;
        case ContentKind::Portable: return lp->alwaysUsb;
        case ContentKind::Empty:
        case ContentKind::General:
            return (g_driveType == DRIVE_REMOVABLE || g_isWpd) ? lp->alwaysUsb
                                                               : lp->alwaysGeneral;
    }
    return lp->alwaysUsb;
}

static void ExecuteOpenFolder() {
    if (g_isWpd) {
        OpenWpdInExplorer(g_driveTitle, g_wpdId.empty() ? g_wpdPath : g_wpdId);
        return;
    }
    if (!DrivePresent(g_driveLetter)) return;
    HINSTANCE h = ShellExecuteW(NULL, L"explore", g_driveRoot.c_str(),
                                nullptr, nullptr, SW_SHOWNORMAL);
    if ((INT_PTR)h <= 32)
        ShellExecuteW(NULL, L"open", g_driveRoot.c_str(), nullptr, nullptr,
                      SW_SHOWNORMAL);
}

static void ExecuteReadyBoost() {
    if (!DrivePresent(g_driveLetter)) return;
    HWND owner = g_hwndDialog ? g_hwndDialog : GetForegroundWindow();
    if (!SHObjectProperties(owner, SHOP_FILEPATH, g_driveRoot.c_str(), L"ReadyBoost")) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.hwnd = owner;
        sei.lpVerb = L"properties";
        sei.lpFile = g_driveRoot.c_str();
        sei.nShow = SW_SHOWNORMAL;
        sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_NOASYNC;
        ShellExecuteExW(&sei);
    }
}

static void ExecuteProgram(const AutoPlayOption& opt) {
    if (!DrivePresent(g_driveLetter)) return;
    if (opt.programPath.empty() || !PathIsUnderRoot(opt.programPath, g_driveRoot)) {
        Wh_Log(L"ExecuteProgram: rejected, path not on volume");
        return;
    }
    if (GetFileAttributesW(opt.programPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        return;
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    sei.lpVerb = L"open";
    sei.lpFile = opt.programPath.c_str();
    sei.lpParameters = opt.programArgs.empty() ? nullptr : opt.programArgs.c_str();
    sei.lpDirectory = g_driveRoot.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&sei)) return;
    DWORD err = GetLastError();
    if (err == ERROR_ELEVATION_REQUIRED) {
        sei.lpVerb = L"runas";
        if (ShellExecuteExW(&sei))
            Wh_Log(L"ExecuteProgram: elevated via runas");
        else
            Wh_Log(L"ExecuteProgram: runas failed %lu", GetLastError());
        return;
    }
    Wh_Log(L"ExecuteProgram: ShellExecuteEx failed %lu", err);
}

static bool FileExistsOnDisk(const wchar_t* path) {
    return path && path[0] && GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
}

static std::wstring FindWindowsMediaPlayer() {
    WCHAR buf[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windows Media Player\\wmplayer.exe",
                                  buf, MAX_PATH) && FileExistsOnDisk(buf))
        return buf;
    if (ExpandEnvironmentStringsW(L"%ProgramFiles(x86)%\\Windows Media Player\\wmplayer.exe",
                                  buf, MAX_PATH) && FileExistsOnDisk(buf))
        return buf;
    WCHAR sys[MAX_PATH] = {};
    if (GetSystemDirectoryW(sys, MAX_PATH)) {
        wcscat_s(sys, L"\\wmplayer.exe");
        if (FileExistsOnDisk(sys)) return sys;
    }
    return L"";
}

static bool HasWindowsMediaPlayer() {
    return !FindWindowsMediaPlayer().empty();
}

static std::wstring FindPhotoViewerDll() {
    WCHAR buf[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windows Photo Viewer\\PhotoViewer.dll",
                                  buf, MAX_PATH) && FileExistsOnDisk(buf))
        return buf;
    if (ExpandEnvironmentStringsW(L"%ProgramFiles(x86)%\\Windows Photo Viewer\\PhotoViewer.dll",
                                  buf, MAX_PATH) && FileExistsOnDisk(buf))
        return buf;
    WCHAR sys[MAX_PATH] = {};
    if (GetSystemDirectoryW(sys, MAX_PATH)) {
        wcscat_s(sys, L"\\PhotoViewer.dll");
        if (FileExistsOnDisk(sys)) return sys;
    }
    return L"";
}

static bool HasWindowsPhotoViewer() {
    return !FindPhotoViewerDll().empty();
}

// Open a media/picture file with the shell's default handler. On Windows 10/11
// the legacy "Windows Media Player" (wmplayer.exe) and Windows 7 Photo Viewer
// (PhotoViewer.dll) are often stubs/unavailable, so opening the file with the
// user's real default app is the reliable way to show something. Returns true
// if the default handler accepted the launch.
static bool OpenFileWithDefaultHandler(const std::wstring& path) {
    if (path.empty() || !FileExistsOnDisk(path.c_str())) return false;
    Wh_Log(L"OpenFileWithDefaultHandler: %s", path.c_str());
    return (INT_PTR)ShellExecuteW(NULL, L"open", path.c_str(),
                                  nullptr, nullptr, SW_SHOWNORMAL) > 32;
}

static void ExecutePlay(const AutoPlayOption& opt) {
    if (!DrivePresent(g_driveLetter)) return;
    const wchar_t* target = !opt.targetPath.empty() && FileExistsOnDisk(opt.targetPath.c_str())
                          ? opt.targetPath.c_str() : g_driveRoot.c_str();

    // Prefer the shell default handler (the app the user actually uses); only if
    // that fails (e.g. no association) fall back to the legacy wmplayer.exe.
    if (OpenFileWithDefaultHandler(target)) {
        Wh_Log(L"ExecutePlay: opened via default handler");
        return;
    }
    std::wstring wmp = FindWindowsMediaPlayer();
    if (wmp.empty()) {
        Wh_Log(L"ExecutePlay: no default handler and WMP not installed, skip");
        return;
    }
    Wh_Log(L"ExecutePlay: launching %s %s", wmp.c_str(), target);
    if ((INT_PTR)ShellExecuteW(NULL, L"open", wmp.c_str(), target,
                               nullptr, SW_SHOWNORMAL) <= 32)
        Wh_Log(L"ExecutePlay: ShellExecute WMP failed");
}

static void ExecuteViewPictures(const AutoPlayOption& opt) {
    if (!DrivePresent(g_driveLetter)) return;
    if (opt.targetPath.empty() || !FileExistsOnDisk(opt.targetPath.c_str())) {
        ExecuteOpenFolder();
        return;
    }

    // Prefer the shell default handler (Photos on Win10/11) so it always opens;
    // fall back to the legacy rundll32 + PhotoViewer.dll only if that fails.
    if (OpenFileWithDefaultHandler(opt.targetPath)) {
        Wh_Log(L"ExecuteViewPictures: opened via default handler");
        return;
    }
    std::wstring dll = FindPhotoViewerDll();
    if (dll.empty()) {
        Wh_Log(L"ExecuteViewPictures: no default handler and Photo Viewer not installed, skip");
        return;
    }
    WCHAR rundll[MAX_PATH] = {};
    if (!GetSystemDirectoryW(rundll, MAX_PATH)) { ExecuteOpenFolder(); return; }
    wcscat_s(rundll, L"\\rundll32.exe");
    std::wstring args = L"\"";
    args += dll;
    args += L"\", ImageView_Fullscreen \"";
    args += opt.targetPath;
    args += L"\"";
    Wh_Log(L"ExecuteViewPictures: launching rundll32 %s", args.c_str());
    if ((INT_PTR)ShellExecuteW(NULL, L"open", rundll, args.c_str(),
                               nullptr, SW_SHOWNORMAL) <= 32)
        Wh_Log(L"ExecuteViewPictures: rundll32 Photo Viewer failed");
}

// Optional component probes are defined below, after the option builder.
static bool FileExistsExpanded(PCWSTR pattern, std::wstring& out);
static bool HasWindowsPhotoGallery();
static bool HasWindowsMobileCenter();
static bool HasDiscImageBurner();

static void ExecuteBurnDisc() {
    if (!DrivePresent(g_driveLetter) || !HasDiscImageBurner()) return;
    std::wstring p;
    if (!FileExistsExpanded(L"%SystemRoot%\\System32\\isoburn.exe", p)) return;
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open"; sei.lpFile = p.c_str();
    std::wstring args = L"\"" + g_driveRoot + L"\"";
    sei.lpParameters = args.c_str(); sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}
static void ExecuteImportPictures() {
    if (!DrivePresent(g_driveLetter) || !HasWindowsPhotoGallery()) return;
    std::wstring p;
    if (!FileExistsExpanded(L"%ProgramFiles%\\Windows Photo Gallery\\PhotoGallery.exe", p) &&
        !FileExistsExpanded(L"%ProgramFiles(x86)%\\Windows Photo Gallery\\PhotoGallery.exe", p)) return;
    ShellExecuteW(nullptr, L"open", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}
static void ExecuteImportMusic() {
    if (!DrivePresent(g_driveLetter)) return;
    std::wstring p = FindWindowsMediaPlayer();
    if (!p.empty()) ShellExecuteW(nullptr, L"open", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}
static void ExecuteSyncDevice() {
    std::wstring p;
    if (FileExistsExpanded(L"%ProgramFiles%\\Windows Mobile\\wmdSync.exe", p) ||
        FileExistsExpanded(L"%ProgramFiles(x86)%\\Windows Mobile\\wmdSync.exe", p))
        ShellExecuteW(nullptr, L"open", p.c_str(), L"/sync", nullptr, SW_SHOWNORMAL);
}
static void ExecuteViewSlideshow(const AutoPlayOption& opt) {
    if (!DrivePresent(g_driveLetter)) return;
    // Open the first picture with the shell default handler (Photos on Win10/11),
    // which provides a slideshow; fall back to opening the folder if no picture.
    if (!opt.targetPath.empty() && FileExistsOnDisk(opt.targetPath.c_str())) {
        if (OpenFileWithDefaultHandler(opt.targetPath)) {
            Wh_Log(L"ExecuteViewSlideshow: opened first picture via default handler");
            return;
        }
    }
    ExecuteOpenFolder(); // safe fallback
}
static void ExecuteControlPanelLink() {
    ShellExecuteW(NULL, L"open", L"control.exe",
                  L"/name Microsoft.AutoPlay", nullptr, SW_SHOWNORMAL);
}

// ============================================================================
// ============================================================================
static void FreeOptions() {
    for (auto& o : g_options) o.icon.Free();
    g_options.clear();
    g_hdrIcon.Free();
}

static void RebuildHeaderIcon() {
    g_hdrIcon.Free();
    if (g_isWpd) {
        if (g_hicoPhone48) { g_hdrIcon.hIcon = g_hicoPhone48; g_hdrIcon.shared = true; return; }
        if (g_bmpPhone48) { g_hdrIcon.hBmp = g_bmpPhone48; g_hdrIcon.shared = true; return; }
    }
    if (g_driveType == DRIVE_CDROM) {
        if (g_hicoDisc48) { g_hdrIcon.hIcon = g_hicoDisc48; g_hdrIcon.shared = true; return; }
        if (g_bmpDisc48) { g_hdrIcon.hBmp = g_bmpDisc48; g_hdrIcon.shared = true; return; }
    }
    // Local (fixed) disks use the silver local-disk icon; removable drives keep
    // the current removable-drive icon. The local icon is also the fallback.
    if (g_driveType == DRIVE_FIXED) {
        if (g_hicoLocal48) { g_hdrIcon.hIcon = g_hicoLocal48; g_hdrIcon.shared = true; return; }
        if (g_bmpLocal48) { g_hdrIcon.hBmp = g_bmpLocal48; g_hdrIcon.shared = true; return; }
        if (g_hicoDrive48) { g_hdrIcon.hIcon = g_hicoDrive48; g_hdrIcon.shared = true; return; }
        if (g_bmpDrive48) { g_hdrIcon.hBmp = g_bmpDrive48; g_hdrIcon.shared = true; return; }
        if (!g_driveRoot.empty())
            g_hdrIcon.hIcon = GetShellIcon(g_driveRoot.c_str(), 2);
        return;
    }
    if (g_hicoDrive48) { g_hdrIcon.hIcon = g_hicoDrive48; g_hdrIcon.shared = true; return; }
    if (g_bmpDrive48) { g_hdrIcon.hBmp = g_bmpDrive48; g_hdrIcon.shared = true; return; }
    if (!g_driveRoot.empty())
        g_hdrIcon.hIcon = GetShellIcon(g_driveRoot.c_str(), 2);
}

static void RebindSharedIcons() {
    for (auto& o : g_options) {
        if (!o.icon.shared) continue;
        switch (o.type) {
            case ActionType::OpenFolder:   o.icon.hBmp = g_bmpFolder; break;
            case ActionType::ReadyBoost:   o.icon.hBmp = g_bmpReadyBoost; break;
            case ActionType::RunProgram:   if (!o.icon.hIcon) o.icon.hBmp = g_bmpSetup; break;
            case ActionType::PlayMedia:    if (!o.icon.hIcon) o.icon.hBmp = g_bmpPlay; break;
            case ActionType::ViewPictures: if (!o.icon.hIcon) o.icon.hBmp = g_bmpFolder; break;
            case ActionType::BurnDisc:       if (!o.icon.hIcon) o.icon.hBmp = g_bmpSetup; break;
            case ActionType::ImportPictures: if (!o.icon.hIcon) o.icon.hBmp = g_bmpFolder; break;
            case ActionType::ImportMusic:    if (!o.icon.hIcon) o.icon.hBmp = g_bmpPlay; break;
            case ActionType::SyncDevice:     if (!o.icon.hIcon) o.icon.hBmp = g_bmpSetup; break;
            case ActionType::ViewSlideshow:  if (!o.icon.hIcon) o.icon.hBmp = g_bmpFolder; break;
        }
    }
    if (g_hdrIcon.shared) {
        if (g_isWpd && g_hicoPhone48)
            g_hdrIcon.hIcon = g_hicoPhone48;
        else if (g_driveType == DRIVE_CDROM && g_hicoDisc48)
            g_hdrIcon.hIcon = g_hicoDisc48;
        else if (g_driveType == DRIVE_FIXED && g_hicoLocal48)
            g_hdrIcon.hIcon = g_hicoLocal48;
        else if (g_hicoDrive48)
            g_hdrIcon.hIcon = g_hicoDrive48;
        else if (!g_hdrIcon.hIcon) {
            g_hdrIcon.hBmp = (g_driveType == DRIVE_CDROM && g_bmpDisc48)
                           ? g_bmpDisc48
                           : (g_driveType == DRIVE_FIXED && g_bmpLocal48)
                           ? g_bmpLocal48 : g_bmpDrive48;
        }
    }
}

static DialogIcon MakeProgramIcon(const AutorunInfo& ar) {
    DialogIcon ic;
    if (!ar.iconFile.empty()) {
        HICON big = nullptr;
        if (ExtractIconExW(ar.iconFile.c_str(), ar.iconIndex, &big, nullptr, 1) > 0 && big)
            ic.hIcon = big;
    }
    if (!ic.hIcon)
        ic.hIcon = GetShellIcon(ar.programPath.c_str(), 0);
    if (!ic.hIcon) { ic.hBmp = g_bmpSetup; ic.shared = true; }
    return ic;
}

static DialogIcon MakePathIcon(const std::wstring& path, HBITMAP fallback) {
    DialogIcon ic;
    if (!path.empty())
        ic.hIcon = GetShellIcon(path.c_str(), 0);
    if (!ic.hIcon) { ic.hBmp = fallback; ic.shared = true; }
    return ic;
}

static bool FileExistsExpanded(PCWSTR pattern, std::wstring& out) {
    WCHAR path[MAX_PATH] = {};
    if (!ExpandEnvironmentStringsW(pattern, path, ARRAYSIZE(path))) return false;
    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES) return false;
    out = path;
    return true;
}

static bool HasWindowsPhotoGallery() {
    std::wstring p;
    return FileExistsExpanded(L"%ProgramFiles%\\Windows Photo Gallery\\PhotoGallery.exe", p) ||
           FileExistsExpanded(L"%ProgramFiles(x86)%\\Windows Photo Gallery\\PhotoGallery.exe", p);
}
static bool HasWindowsMobileCenter() {
    std::wstring p;
    return FileExistsExpanded(L"%ProgramFiles%\\Windows Mobile\\wmdSync.exe", p) ||
           FileExistsExpanded(L"%ProgramFiles(x86)%\\Windows Mobile\\wmdSync.exe", p);
}
static bool HasDiscImageBurner() {
    WCHAR p[MAX_PATH] = {};
    return GetSystemDirectoryW(p, ARRAYSIZE(p)) &&
           wcscat_s(p, L"\\isoburn.exe") == 0 &&
           GetFileAttributesW(p) != INVALID_FILE_ATTRIBUTES;
}

static DialogIcon MakeSafePathIcon(const std::wstring& path, HBITMAP fallback) {
    DialogIcon ic;
    if (!path.empty() && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES)
        ic.hIcon = GetShellIcon(path.c_str(), 0);
    if (!ic.hIcon) { ic.hBmp = fallback; ic.shared = true; }
    return ic;
}

static void BuildOptions(const AutorunInfo& ar, const MediaInventory& inv) {
    FreeOptions();
    const LangPack* lp = L();
    g_hasProgramSection = ar.hasProgram;
    g_firstGeneralIdx = 0;
    g_contentKind = ClassifyContent(g_audioCd, ar.hasProgram, g_driveType, inv);

    // Item 4 fix: since Windows 7 RTM (and Vista/XP post-KB971029), autorun.inf
    // "run program" entries are honored only for non-removable optical media -
    // this is the Conficker/USB-worm mitigation. Restrict the program row the
    // same way, so a removable USB stick can no longer present an
    // attacker-controlled label/icon that runs an arbitrary EXE with one click.
    if (ar.hasProgram && g_driveType == DRIVE_CDROM) {
        AutoPlayOption prog;
        prog.type = ActionType::RunProgram;
        prog.group = OptionGroup::Program;
        prog.programPath = ar.programPath;
        prog.programArgs = ar.programArgs;
        if (!ar.action.empty()) {
            prog.line1 = ar.action;
        } else {
            // Widen the buffer so a MAX_PATH filename cannot overflow swprintf_s
            // (which terminates the process on overflow) (item 9).
            wchar_t buf[1024];
            swprintf_s(buf, ARRAYSIZE(buf), lp->runFile,
                       PathFindFileNameW(ar.programPath.c_str()));
            prog.line1 = buf;
        }
        std::wstring company = GetCompanyName(ar.programPath);
        if (!company.empty()) {
            // Clamp the (attacker-controlled) company string so it can never
            // overflow swprintf_s (item 9).
            std::wstring companyClamped = company.substr(0, 256);
            wchar_t buf[1024];
            swprintf_s(buf, ARRAYSIZE(buf), lp->publishedBy, companyClamped.c_str());
            prog.line2 = buf;
        } else {
            prog.line2 = lp->publisherUnknown;
        }
        prog.icon = MakeProgramIcon(ar);
        g_options.push_back(std::move(prog));
    }

    const bool haveWmp = HasWindowsMediaPlayer();
    if (g_audioCd && haveWmp) {
        AutoPlayOption play;
        play.type = ActionType::PlayMedia;
        play.group = OptionGroup::Content;
        play.line1 = lp->playAudioCd;
        play.line2 = lp->usingPlayer;
        play.targetPath = g_driveRoot + L"Track01.cda";
        if (!FileExistsOnDisk(play.targetPath.c_str()))
            play.targetPath.clear();
        play.icon.hBmp = g_bmpPlay;
        play.icon.shared = true;
        g_options.push_back(std::move(play));
        Wh_Log(L"BuildOptions: added Play audio CD (WMP present)");
    } else if (g_audioCd) {
        Wh_Log(L"BuildOptions: skip Play audio CD, WMP missing");
    } else if (g_contentKind == ContentKind::Pictures &&
               !inv.firstPicture.empty() && FileExistsOnDisk(inv.firstPicture.c_str())) {
        // On Windows 10/11 the legacy Windows 7 Photo Viewer (PhotoViewer.dll) is
        // often absent, so we no longer require it to offer "View pictures": the
        // shell default handler (Photos) is used first (see ExecuteViewPictures).
        AutoPlayOption view;
        view.type = ActionType::ViewPictures;
        view.group = OptionGroup::Content;
        view.line1 = lp->viewPictures;
        view.line2 = lp->usingWindows;
        view.targetPath = inv.firstPicture;
        HBITMAP customIcon = CreateBitmapFromBase64PNG(PHOTO_VIEWER_BASE64, Scale(32), Scale(32));
if (customIcon) {
    view.icon.hBmp = customIcon;
    view.icon.shared = false;
} else {
    view.icon = MakePathIcon(inv.firstPicture, g_bmpFolder);
}       
 g_options.push_back(std::move(view));
        Wh_Log(L"BuildOptions: added View pictures (default handler first)");
    } else if (g_contentKind == ContentKind::Music && haveWmp &&
               !inv.firstAudio.empty() && FileExistsOnDisk(inv.firstAudio.c_str())) {
        AutoPlayOption play;
        play.type = ActionType::PlayMedia;
        play.group = OptionGroup::Content;
        play.line1 = lp->playMedia;
        play.line2 = lp->usingPlayer;
        play.targetPath = inv.firstAudio;
        play.icon.hBmp = g_bmpPlay;
        play.icon.shared = true;
        g_options.push_back(std::move(play));
    } else if (g_contentKind == ContentKind::Music && !haveWmp) {
        Wh_Log(L"BuildOptions: skip Play music, WMP missing");
    } else if (g_contentKind == ContentKind::Video && haveWmp &&
               !inv.firstVideo.empty() && FileExistsOnDisk(inv.firstVideo.c_str())) {
        AutoPlayOption play;
        play.type = ActionType::PlayMedia;
        play.group = OptionGroup::Content;
        play.line1 = lp->playMedia;
        play.line2 = lp->usingPlayer;
        play.targetPath = inv.firstVideo;
        play.icon.hBmp = g_bmpPlay;
        play.icon.shared = true;
        g_options.push_back(std::move(play));
        Wh_Log(L"BuildOptions: added Play video (WMP present)");
    } else if (g_contentKind == ContentKind::Video && !haveWmp) {
        Wh_Log(L"BuildOptions: skip Play video, WMP missing");
    } else if (g_contentKind == ContentKind::DvdMovie && haveWmp) {
        AutoPlayOption play;
        play.type = ActionType::PlayMedia;
        play.group = OptionGroup::Content;
        play.line1 = lp->playMedia;
        play.line2 = lp->usingPlayer;
        play.targetPath = g_driveRoot + L"VIDEO_TS\\VIDEO_TS.IFO";
        if (!FileExistsOnDisk(play.targetPath.c_str()))
            play.targetPath = g_driveRoot;
        play.icon.hBmp = g_bmpPlay;
        play.icon.shared = true;
        g_options.push_back(std::move(play));
        Wh_Log(L"BuildOptions: added Play DVD/Blu-ray (WMP present)");
    } else if (g_contentKind == ContentKind::DvdMovie) {
        Wh_Log(L"BuildOptions: skip Play DVD/Blu-ray, WMP missing");
    } else if (g_contentKind == ContentKind::Mixed) {
        std::wstring media = !inv.firstAudio.empty() ? inv.firstAudio : inv.firstVideo;
        if (haveWmp && !media.empty() && FileExistsOnDisk(media.c_str())) {
            AutoPlayOption play;
            play.type = ActionType::PlayMedia;
            play.group = OptionGroup::Content;
            play.line1 = lp->playMedia;
            play.line2 = lp->usingPlayer;
            play.targetPath = media;
            play.icon.hBmp = g_bmpPlay;
            play.icon.shared = true;
            g_options.push_back(std::move(play));
        } else if (!haveWmp && !media.empty()) {
            Wh_Log(L"BuildOptions: skip Play mixed, WMP missing");
        }
        if (HasWindowsPhotoViewer() && !inv.firstPicture.empty() &&
            FileExistsOnDisk(inv.firstPicture.c_str())) {
            AutoPlayOption view;
            view.type = ActionType::ViewPictures;
            view.group = OptionGroup::Content;
            view.line1 = lp->viewPictures;
            view.line2 = lp->usingWindows;
            view.targetPath = inv.firstPicture;
            HBITMAP customIcon = CreateBitmapFromBase64PNG(PHOTO_VIEWER_BASE64, Scale(32), Scale(32));
if (customIcon) {
    view.icon.hBmp = customIcon;
    view.icon.shared = false;
} else {
    view.icon = MakePathIcon(inv.firstPicture, g_bmpFolder);
}            
g_options.push_back(std::move(view));
        }
    }

    // Add optional handlers only when the actual component is installed.
    // Detection is deliberately conservative: no placeholder rows are shown.
    if (g_contentKind == ContentKind::AudioCD && HasWindowsMediaPlayer()) {
        AutoPlayOption o;
        o.type = ActionType::ImportMusic; o.group = OptionGroup::Content;
        o.line1 = L"Importa brani nel Windows Media Player";
        o.line2 = lp->usingPlayer; o.icon.hBmp = g_bmpPlay; o.icon.shared = true;
        g_options.push_back(std::move(o));
    }
    if (g_contentKind == ContentKind::BlankDisc && HasDiscImageBurner()) {
        AutoPlayOption o;
        o.type = ActionType::BurnDisc; o.group = OptionGroup::Content;
        o.line1 = L"Masterizza disco";
        o.line2 = L"utilizzando Windows Disc Image Burner";
        std::wstring p = L""; FileExistsExpanded(L"%SystemRoot%\\System32\\isoburn.exe", p);
        o.icon = MakeSafePathIcon(p, g_bmpSetup);
        g_options.push_back(std::move(o));
    }
    if (g_contentKind == ContentKind::Pictures) {
        if (HasWindowsPhotoGallery()) {
            AutoPlayOption o;
            o.type = ActionType::ImportPictures; o.group = OptionGroup::Content;
            o.line1 = L"Importa immagini e video";
            o.line2 = L"utilizzando Windows Photo Gallery";
            std::wstring p; FileExistsExpanded(L"%ProgramFiles%\\Windows Photo Gallery\\PhotoGallery.exe", p);
            o.icon = MakeSafePathIcon(p, g_bmpFolder);
            g_options.push_back(std::move(o));
        }
        if (!inv.firstPicture.empty()) {
            AutoPlayOption o;
            o.type = ActionType::ViewSlideshow; o.group = OptionGroup::Content;
            o.line1 = L"Avvia presentazione";
            o.line2 = lp->usingWindows; o.targetPath = inv.firstPicture;
            o.icon = MakeSafePathIcon(inv.firstPicture, g_bmpFolder);
            g_options.push_back(std::move(o));
        }
    }
    if (g_isWpd && HasWindowsMobileCenter()) {
        AutoPlayOption o;
        o.type = ActionType::SyncDevice; o.group = OptionGroup::Content;
        o.line1 = L"Sincronizza dispositivo";
        o.line2 = L"utilizzando Windows Mobile Center";
        std::wstring p; FileExistsExpanded(L"%ProgramFiles%\\Windows Mobile\\wmdSync.exe", p);
        o.icon = MakeSafePathIcon(p, g_bmpSetup);
        g_options.push_back(std::move(o));
    }

    g_firstGeneralIdx = (int)g_options.size();

    {
        AutoPlayOption folder;
        folder.type = ActionType::OpenFolder;
        folder.group = OptionGroup::General;
        folder.line1 = lp->openFolder;
        folder.line2 = lp->usingExplorer;
        folder.icon.hBmp = g_bmpFolder;
        folder.icon.shared = true;
        g_options.push_back(std::move(folder));
    }

    if (IsReadyBoostEligible(g_driveRoot, g_driveType)) {
        AutoPlayOption boost;
        boost.type = ActionType::ReadyBoost;
        boost.group = OptionGroup::General;
        boost.line1 = lp->speedUp;
        boost.line2 = lp->usingReadyBoost;
        boost.icon.hBmp = g_bmpReadyBoost;
        boost.icon.shared = true;
        g_options.push_back(std::move(boost));
    }


    g_prompt.clear();
    if (g_contentKind == ContentKind::Mixed)
        g_prompt = lp->chooseDisk;
}

static int DefaultOptionIndex() {
    for (int i = 0; i < (int)g_options.size(); i++)
        if (g_options[i].type == ActionType::OpenFolder) return i;
    return g_options.empty() ? -1 : 0;
}

static void ExecuteOptionByIndex(int idx, HWND hwndDlg) {
    Wh_Log(L"ExecuteOptionByIndex: idx=%d options=%d letter=%c drivePresent=%d",
           idx, (int)g_options.size(),
           g_driveLetter ? (wchar_t)g_driveLetter : L'?',
           g_driveLetter ? (DrivePresent(g_driveLetter) ? 1 : 0) : -1);
    if (idx < 0 || idx >= (int)g_options.size()) return;
    AutoPlayOption opt = g_options[idx];
    bool remember = g_alwaysChecked;
    int letter = g_driveLetter;

    // Functionality-notes fix: TryExecuteRemembered() always returns false for a
    // "Program|..." token (running a remembered program is deliberately refused
    // as a security decision), so persisting it here previously just clobbered
    // whatever choice (e.g. OpenFolder) was remembered for this content class
    // and silently turned "always do this" into a no-op from then on. Simply
    // don't persist a token for the program row.
    if (remember && opt.type != ActionType::RunProgram) {
        std::wstring token;
        switch (opt.type) {
            case ActionType::OpenFolder:   token = L"OpenFolder"; break;
            case ActionType::ReadyBoost:   token = L"ReadyBoost"; break;
            case ActionType::PlayMedia:    token = L"PlayMedia"; break;
            case ActionType::ViewPictures: token = L"ViewPictures"; break;
            case ActionType::RunProgram:   break; // unreachable, excluded above
            case ActionType::BurnDisc:      token = L"BurnDisc"; break;
            case ActionType::ImportPictures: token = L"ImportPictures"; break;
            case ActionType::ImportMusic:   token = L"ImportMusic"; break;
            case ActionType::SyncDevice:    token = L"SyncDevice"; break;
            case ActionType::ViewSlideshow: token = L"ViewSlideshow"; break;
        }
        WriteRemembered(ClassKey(g_contentKind, g_hasProgramSection, g_driveType), token);
    }

    if (!g_isWpd && !DrivePresent(letter)) {
        if (hwndDlg && IsWindow(hwndDlg)) DestroyWindow(hwndDlg);
        return;
    }
    if (g_isWpd && !CurrentTargetPresent()) {
        if (hwndDlg && IsWindow(hwndDlg)) DestroyWindow(hwndDlg);
        return;
    }

    // Item 2 fix: the actual action (ShellExecuteExW with lpVerb=L"runas" can pop
    // an unbounded UAC prompt, SHObjectProperties and IPortableDeviceManager calls
    // are routinely slow) must NOT run on this thread's message loop, because this
    // is the same thread that services WMU_SHUTDOWN in ListenerWndProc. Close the
    // dialog immediately and hand the blocking invocation to a short-lived,
    // detached worker thread so Wh_ModUninit's wait on g_hUiThread is never stuck
    // behind a UAC prompt or a slow shell call.
    if (hwndDlg && IsWindow(hwndDlg)) DestroyWindow(hwndDlg);

    // Keep the worker handle until it has finished. ProcessPendingQueue is held
    // while this handle exists, so the globals used by the Execute* routines
    // cannot be rewritten for another volume during the action.
    if (g_hActionWorker) return;
    auto* pOpt = new AutoPlayOption(opt);
    HANDLE hWorker = CreateThread(NULL, 0, [](LPVOID param) -> DWORD {
        std::unique_ptr<AutoPlayOption> opt(static_cast<AutoPlayOption*>(param));
        ApScopedCoInit coInit;
        switch (opt->type) {
            case ActionType::OpenFolder:   ExecuteOpenFolder(); break;
            case ActionType::ReadyBoost:   ExecuteReadyBoost(); break;
            case ActionType::RunProgram:   ExecuteProgram(*opt); break;
            case ActionType::PlayMedia:    ExecutePlay(*opt); break;
            case ActionType::ViewPictures: ExecuteViewPictures(*opt); break;
            case ActionType::BurnDisc: ExecuteBurnDisc(); break;
            case ActionType::ImportPictures: ExecuteImportPictures(); break;
            case ActionType::ImportMusic: ExecuteImportMusic(); break;
            case ActionType::SyncDevice: ExecuteSyncDevice(); break;
            case ActionType::ViewSlideshow: ExecuteViewSlideshow(*opt); break;
        }
        // ApScopedCoInit releases COM even when an action throws.
        if (g_hwndListener)
            PostMessageW(g_hwndListener, WMU_ACTION_DONE, 0, 0);
        return 0;
    }, pOpt, 0, nullptr);
    if (hWorker) {
        g_hActionWorker = hWorker;
    } else {
        delete pOpt;
    }
}

static bool TryExecuteRemembered() {
    std::wstring key = ClassKey(g_contentKind, g_hasProgramSection, g_driveType);
    std::wstring token = ReadRemembered(key);
    Wh_Log(L"TryExecuteRemembered: key=%s token=%s", key.c_str(),
           token.empty() ? L"(none)" : token.c_str());
    if (token.empty()) return false;

    if (token == L"OpenFolder") { ExecuteOpenFolder(); return true; }
    if (token == L"ReadyBoost") {
        if (IsReadyBoostEligible(g_driveRoot, g_driveType)) {
            ExecuteReadyBoost();
            return true;
        }
        return false;
    }
    if (token == L"PlayMedia") {
        if (!HasWindowsMediaPlayer()) return false;
        for (const auto& o : g_options)
            if (o.type == ActionType::PlayMedia) { ExecutePlay(o); return true; }
        return false;
    }
    if (token == L"ViewPictures") {
        if (!HasWindowsPhotoViewer()) return false;
        for (const auto& o : g_options)
            if (o.type == ActionType::ViewPictures) { ExecuteViewPictures(o); return true; }
        return false;
    }
    if (token.compare(0, 8, L"Program|") == 0)
        return false;
    return false;
}

// ============================================================================
// ============================================================================
struct LayoutItem {
    enum Type { OPTION, SECTION, LINK, CHECK, PROMPT } type;
    int optIndex;
    RECT rc;
};

static int MeasureMultiline(HDC hdc, HFONT font, PCWSTR text, int maxWidth, int maxLines) {
    if (!text || !*text || maxWidth <= 0) return 0;
    HFONT old = (HFONT)SelectObject(hdc, font);
    RECT rc = { 0, 0, maxWidth, 0 };
    DrawTextW(hdc, text, -1, &rc,
              DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX | DT_TOP | DT_LEFT);
    TEXTMETRICW tm = {};
    GetTextMetricsW(hdc, &tm);
    int lineH = tm.tmHeight > 0 ? tm.tmHeight : Scale(12);
    int h = rc.bottom - rc.top;
    if (maxLines > 0) h = (std::min)(h, lineH * maxLines);
    SelectObject(hdc, old);
    return (std::max)(h, lineH);
}

static int MeasureSingle(HDC hdc, HFONT font, PCWSTR text) {
    if (!text) return 0;
    HFONT old = (HFONT)SelectObject(hdc, font);
    SIZE sz = {};
    GetTextExtentPoint32W(hdc, text, lstrlenW(text), &sz);
    SelectObject(hdc, old);
    return sz.cx;
}

static PCWSTR SectionTitleForGroup(const LangPack* lp, OptionGroup g) {
    if (g == OptionGroup::Program) return lp->installRun;
    if (g == OptionGroup::Content) {
        if (g_contentKind == ContentKind::Pictures) return lp->picturesOptions;
        if (g_contentKind == ContentKind::Music || g_audioCd) return lp->musicOptions;
        if (g_contentKind == ContentKind::Video || g_contentKind == ContentKind::DvdMovie)
            return lp->musicOptions;
        if (g_contentKind == ContentKind::Mixed) return lp->mixedOptions;
        return lp->musicOptions;
    }
    return lp->generalOptions;
}

static int ComputeLayout(std::vector<LayoutItem>& items) {
    items.clear();
    EnsureDpiResourcesAndRebind(); // item 5 fix: same paint/hit-test hazard as WM_CREATE
    const LangPack* lp = L();
    int clientW = Scale(W_WIDTH);

    HDC hdc = GetDC(NULL);
    if (hdc && g_fontText) SelectObject(hdc, g_fontText);

    int iconSz = Scale(HDR_ICON_SZ);
    int headerH = Scale(HDR_PAD_Y) + iconSz + Scale(HDR_PAD_Y);
    g_headerH = headerH;

    int y = headerH;

    if (!g_prompt.empty() && hdc) {
        int tw = clientW - Scale(HDR_PAD_X) * 2;
        int th = MeasureMultiline(hdc, g_fontText, g_prompt.c_str(), tw, 3);
        RECT pr = { Scale(HDR_PAD_X), y, clientW - Scale(HDR_PAD_X), y + th + Scale(4) };
        items.push_back({ LayoutItem::PROMPT, -1, pr });
        y = pr.bottom + Scale(4);
    }

    int box = Scale(CHECK_BOX);
    if (box < 13) box = 13;
    int checkTextX = Scale(HDR_PAD_X) + box + Scale(6);
    int checkTextW = clientW - checkTextX - Scale(HDR_PAD_X);
    int checkTextH = hdc ? MeasureMultiline(hdc, g_fontText, AlwaysText(lp), checkTextW, 3)
                         : Scale(16);
    int checkH = (std::max)(box, checkTextH) + Scale(12);
    RECT chk = { Scale(HDR_PAD_X), y, clientW - Scale(HDR_PAD_X), y + checkH };
    items.push_back({ LayoutItem::CHECK, -1, chk });
    y = chk.bottom + Scale(BODY_GAP);
    g_checkBottom = y;

    OptionGroup prev = OptionGroup::Program;
    bool firstOpt = true;
    for (int i = 0; i < (int)g_options.size(); i++) {
        if (firstOpt || g_options[i].group != prev) {
            int sh = Scale(SECT_H_MIN);
            RECT sect = { 0, y, clientW, y + sh };
            items.push_back({ LayoutItem::SECTION, (int)g_options[i].group, sect });
            y += sh;
            prev = g_options[i].group;
            firstOpt = false;
        }

        int textX = Scale(ROW_PAD_X) + Scale(ROW_ICON) + Scale(ROW_TEXT_GAP);
        int textW = clientW - textX - Scale(ROW_PAD_X);
        int h1 = hdc ? MeasureMultiline(hdc, g_fontText, g_options[i].line1.c_str(), textW, 2)
                     : Scale(16);
        int h2 = 0;
        if (hdc && !g_options[i].line2.empty())
            h2 = MeasureMultiline(hdc, g_fontSmall, g_options[i].line2.c_str(), textW, 2);
        int rowH = Scale(6) + h1 + (h2 ? Scale(2) + h2 : 0) + Scale(6);
        rowH = (std::max)(rowH, Scale(ROW_H_MIN));
        RECT row = { Scale(6), y, clientW - Scale(6), y + rowH };
        items.push_back({ LayoutItem::OPTION, i, row });
        y += rowH;
    }

    y += Scale(6);
    int linkW = clientW - Scale(HDR_PAD_X) * 2;
    int linkH = hdc ? MeasureMultiline(hdc, g_fontLink, lp->viewMore, linkW, 2) : Scale(16);
    linkH = (std::max)(linkH + Scale(6), Scale(LINK_H_MIN));
    RECT link = { Scale(HDR_PAD_X), y, clientW - Scale(HDR_PAD_X), y + linkH };
    items.push_back({ LayoutItem::LINK, -1, link });
    y = link.bottom + Scale(END_PAD);

    if (hdc) ReleaseDC(NULL, hdc);
    return y;
}

static void PlaceNativeCheck(HWND hDlg) {
    if (!hDlg) return;
    std::vector<LayoutItem> items;
    ComputeLayout(items);
    RECT rc = {};
    bool found = false;
    for (const auto& it : items) {
        if (it.type == LayoutItem::CHECK) { rc = it.rc; found = true; break; }
    }
    if (!found) return;
    const LangPack* lp = L();
    if (!g_hwndCheck || !IsWindow(g_hwndCheck)) {
        g_hwndCheck = CreateWindowExW(0, L"BUTTON", AlwaysText(lp),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | BS_MULTILINE | BS_TOP,
            rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
            hDlg, (HMENU)(INT_PTR)IDC_ALWAYS, HINST_THISCOMPONENT, nullptr);
        if (g_fontText)
            SendMessageW(g_hwndCheck, WM_SETFONT, (WPARAM)g_fontText, TRUE);
    } else {
        SetWindowTextW(g_hwndCheck, AlwaysText(lp));
        SetWindowPos(g_hwndCheck, nullptr, rc.left, rc.top,
                     rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        if (g_fontText)
            SendMessageW(g_hwndCheck, WM_SETFONT, (WPARAM)g_fontText, FALSE);
    }
    SendMessageW(g_hwndCheck, BM_SETCHECK,
                 g_alwaysChecked ? BST_CHECKED : BST_UNCHECKED, 0);
}

// ============================================================================
// ============================================================================
static void FillSolidRect(HDC hdc, const RECT& rc, COLORREF clr) {
    HBRUSH br = CreateSolidBrush(clr);
    RECT r = rc;
    FillRect(hdc, &r, br);
    DeleteObject(br);
}

static void DrawTextWrapped(HDC hdc, HFONT font, COLORREF clr, PCWSTR text,
                            int x, int y, int w, int h) {
    if (!text || !font) return;
    HFONT old = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, clr);
    SetBkMode(hdc, TRANSPARENT);
    RECT tr = { x, y, x + w, y + h };
    DrawTextW(hdc, text, -1, &tr,
              DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX | DT_END_ELLIPSIS);
    SelectObject(hdc, old);
}

static void DrawTextLine(HDC hdc, HFONT font, COLORREF clr, PCWSTR text,
                         int x, int y, int w, int h) {
    if (!text || !font) return;
    HFONT old = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, clr);
    SetBkMode(hdc, TRANSPARENT);
    RECT tr = { x, y, x + w, y + h };
    DrawTextW(hdc, text, -1, &tr,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    SelectObject(hdc, old);
}

static void PaintDialog(HWND hWnd, HDC hdcPaint) {
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    int cw = rcClient.right, ch = rcClient.bottom;

    HDC mem = CreateCompatibleDC(hdcPaint);
    HBITMAP bmp = CreateCompatibleBitmap(hdcPaint, cw, ch);
    HBITMAP oldBmp = (HBITMAP)SelectObject(mem, bmp);

    const bool hc = IsHighContrastOn();
    const COLORREF bg       = hc ? GetSysColor(COLOR_WINDOW) : RGB(255, 255, 255);
    const COLORREF line1    = hc ? GetSysColor(COLOR_WINDOWTEXT) : CLR_LINE1;
    const COLORREF line2    = hc ? GetSysColor(COLOR_GRAYTEXT) : CLR_LINE2;
    const COLORREF titleClr = GetWin7TitleColor();
    const COLORREF subClr   = hc ? GetSysColor(COLOR_GRAYTEXT) : CLR_SUB_TEXT;
    const COLORREF sectClr  = hc ? GetSysColor(COLOR_WINDOWTEXT) : CLR_SECTION;
    const COLORREF linkClr  = hc ? GetSysColor(COLOR_HOTLIGHT) : CLR_LINK;
    const COLORREF hoverFill= hc ? GetSysColor(COLOR_HIGHLIGHT) : CLR_HOVER_FILL;
    const COLORREF hoverEdge= hc ? GetSysColor(COLOR_HIGHLIGHT) : CLR_HOVER_EDGE;
    const COLORREF hoverTx1 = hc ? GetSysColor(COLOR_HIGHLIGHTTEXT) : CLR_LINE1;
    const COLORREF hoverTx2 = hc ? GetSysColor(COLOR_HIGHLIGHTTEXT) : CLR_LINE2;
    const COLORREF hair     = hc ? GetSysColor(COLOR_GRAYTEXT) : CLR_HAIRLINE;

    FillSolidRect(mem, rcClient, bg);

    std::vector<LayoutItem> items;
    ComputeLayout(items);

    const LangPack* lp = L();
    int iconSz = Scale(HDR_ICON_SZ);
    int iconX = Scale(HDR_PAD_X);
    int iconY = Scale(HDR_PAD_Y);
    if (g_hdrIcon.hIcon || g_hdrIcon.hBmp)
        g_hdrIcon.Draw(mem, iconX, iconY, iconSz);
    else if (g_isWpd && g_hicoPhone48)
        DrawIconEx(mem, iconX, iconY, g_hicoPhone48, iconSz, iconSz, 0, NULL, DI_NORMAL);
    else if (g_driveType == DRIVE_CDROM && g_hicoDisc48)
        DrawIconEx(mem, iconX, iconY, g_hicoDisc48, iconSz, iconSz, 0, NULL, DI_NORMAL);
    else if (g_driveType == DRIVE_FIXED && g_hicoLocal48)
        DrawIconEx(mem, iconX, iconY, g_hicoLocal48, iconSz, iconSz, 0, NULL, DI_NORMAL);
    else if (g_driveType == DRIVE_FIXED && g_bmpLocal48)
        DrawAlphaBitmap(mem, g_bmpLocal48, iconX, iconY, iconSz, iconSz);
    else if (g_hicoDrive48)
        DrawIconEx(mem, iconX, iconY, g_hicoDrive48, iconSz, iconSz, 0, NULL, DI_NORMAL);
    else if (g_bmpDrive48)
        DrawAlphaBitmap(mem, g_bmpDrive48, iconX, iconY, iconSz, iconSz);

    int textX = iconX + iconSz + Scale(HDR_TEXT_GAP);
    int textW = cw - textX - Scale(HDR_PAD_X);
    bool hasSub = !g_headerSub.empty();
    if (hasSub) {
        DrawTextLine(mem, g_fontTitle, titleClr, g_driveTitle.c_str(),
                     textX, iconY + Scale(4), textW, Scale(22));
        DrawTextLine(mem, g_fontText, subClr, g_headerSub.c_str(),
                     textX, iconY + Scale(26), textW, Scale(18));
    } else {
        DrawTextLine(mem, g_fontTitle, titleClr, g_driveTitle.c_str(),
                     textX, iconY, textW, iconSz);
    }

    g_hotRects.clear();
    HPEN hoverPen = CreatePen(PS_SOLID, 1, hoverEdge);
    HBRUSH hoverBrush = CreateSolidBrush(hoverFill);

    for (size_t i = 0; i < items.size(); i++) {
        const LayoutItem& it = items[i];
        if (it.type == LayoutItem::PROMPT) {
            DrawTextWrapped(mem, g_fontText, line1, g_prompt.c_str(),
                            it.rc.left, it.rc.top, it.rc.right - it.rc.left,
                            it.rc.bottom - it.rc.top);
            continue;
        }
        if (it.type == LayoutItem::CHECK) {
            g_rcCheckHit = it.rc;
            continue;
        }
        if (it.type == LayoutItem::SECTION) {
            PCWSTR title = SectionTitleForGroup(lp, (OptionGroup)it.optIndex);
            int tx = it.rc.left + Scale(HDR_PAD_X);
            int ty = it.rc.top + Scale(6);
            int tw = MeasureSingle(mem, g_fontText, title);
            DrawTextLine(mem, g_fontText, sectClr, title,
                         tx, ty, tw + Scale(4), Scale(16));
            int lineY = ty + Scale(8);
            int lineX = tx + tw + Scale(8);
            if (lineX < it.rc.right - Scale(HDR_PAD_X)) {
                RECT lr = { lineX, lineY, it.rc.right - Scale(HDR_PAD_X), lineY + 1 };
                FillSolidRect(mem, lr, hair);
            }
            continue;
        }
        if (it.type == LayoutItem::LINK) {
            bool hover = (g_hoverItem == (int)i) || (g_kbdFocus && g_focusItem == (int)i);
            HFONT lf = hover ? g_fontLinkUnder : g_fontLink;
            DrawTextWrapped(mem, lf, linkClr, lp->viewMore,
                            it.rc.left, it.rc.top + Scale(2),
                            it.rc.right - it.rc.left, it.rc.bottom - it.rc.top);
            g_rcLink = it.rc;
            g_hotRects.push_back({ it.rc, 1, -1 });
            if (g_kbdFocus && g_focusItem == (int)i)
                DrawFocusRect(mem, const_cast<RECT*>(&it.rc));
            continue;
        }

        bool hover = (g_hoverItem == (int)i) || (g_kbdFocus && g_focusItem == (int)i);
        bool pressed = (g_pressedItem == (int)i);
        if (hover) {
            HPEN oldPen = (HPEN)SelectObject(mem, hoverPen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(mem, hoverBrush);
            RECT hr = it.rc;
            if (pressed) OffsetRect(&hr, 0, 1);
            Rectangle(mem, hr.left, hr.top, hr.right, hr.bottom);
            SelectObject(mem, oldBrush);
            SelectObject(mem, oldPen);
        }
        const AutoPlayOption& opt = g_options[it.optIndex];
        int iSz = Scale(ROW_ICON);
        int iW = (opt.type == ActionType::ReadyBoost) ? MulDiv(iSz, 21, 20) : iSz;
        int ix = it.rc.left + Scale(ROW_PAD_X) - Scale(6);
        int iy = it.rc.top + ((it.rc.bottom - it.rc.top) - iSz) / 2;
        if (opt.icon.hIcon)
            DrawIconEx(mem, ix, iy, opt.icon.hIcon, iW, iSz, 0, NULL, DI_NORMAL);
        else if (opt.icon.hBmp)
            DrawAlphaBitmap(mem, opt.icon.hBmp, ix, iy, iW, iSz);
        COLORREF c1 = hover ? hoverTx1 : line1;
        COLORREF c2 = hover ? hoverTx2 : line2;
        bool isProgram = (opt.type == ActionType::RunProgram);
        int tx = it.rc.left + Scale(ROW_PAD_X) + Scale(ROW_ICON) + Scale(ROW_TEXT_GAP) - Scale(6);
        int tw = it.rc.right - tx - Scale(10);
        int h1 = MeasureMultiline(mem, isProgram ? g_fontBold : g_fontText,
                                  opt.line1.c_str(), tw, 2);
        int h2 = opt.line2.empty() ? 0
               : MeasureMultiline(mem, g_fontSmall, opt.line2.c_str(), tw, 2);
        int block = h1 + (h2 ? Scale(2) + h2 : 0);
        int ty = it.rc.top + ((it.rc.bottom - it.rc.top) - block) / 2;
        DrawTextWrapped(mem, isProgram ? g_fontBold : g_fontText, c1, opt.line1.c_str(),
                        tx, ty, tw, h1);
        if (h2)
            DrawTextWrapped(mem, g_fontSmall, c2, opt.line2.c_str(),
                            tx, ty + h1 + Scale(2), tw, h2);
        g_hotRects.push_back({ it.rc, 0, it.optIndex });
        if (g_kbdFocus && g_focusItem == (int)i && !hover)
            DrawFocusRect(mem, const_cast<RECT*>(&it.rc));
    }
    DeleteObject(hoverPen);
    DeleteObject(hoverBrush);

    BitBlt(hdcPaint, 0, 0, cw, ch, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
}

// ============================================================================
// ============================================================================
static const WCHAR* kDialogClass = L"Win7ClassicAutoPlayDialog";
static bool g_dialogClassRegistered = false;

static UINT GetBestDpiForWindow(HWND hwnd) {
    UINT dpi = GetDpiForWindow(hwnd);
    if (dpi < 96) dpi = 96;
    return dpi;
}

static HMONITOR MonitorForDialog() {
    HWND fg = GetForegroundWindow();
    if (fg && IsWindowVisible(fg))
        return MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    POINT pt = {};
    GetCursorPos(&pt);
    return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
}

static void ClampToWorkArea(int& x, int& y, int w, int h, const RECT& work) {
    if (x + w > work.right) x = work.right - w;
    if (y + h > work.bottom) y = work.bottom - h;
    if (x < work.left) x = work.left;
    if (y < work.top) y = work.top;
}

static void EnsureOnScreen(HWND hWnd) {
    RECT wr = {};
    GetWindowRect(hWnd, &wr);
    HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) return;
    int w = wr.right - wr.left, h = wr.bottom - wr.top;
    int x = wr.left, y = wr.top;
    ClampToWorkArea(x, y, w, h, mi.rcWork);
    if (x != wr.left || y != wr.top)
        SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static void ApplyDialogSize(HWND hWnd, const RECT* suggested) {
    std::vector<LayoutItem> items;
    int clientH = ComputeLayout(items);
    int clientW = Scale(W_WIDTH);
    DWORD style = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
    DWORD ex = (DWORD)GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    RECT rc = { 0, 0, clientW, clientH };
    AdjustWindowRectExForDpi(&rc, style, FALSE, ex, g_dpi);
    int winW = rc.right - rc.left, winH = rc.bottom - rc.top;

    HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(mon, &mi);
    int x, y;
    if (suggested) {
        x = suggested->left;
        y = suggested->top;
    } else {
        RECT wr = {};
        GetWindowRect(hWnd, &wr);
        x = wr.left;
        y = wr.top;
    }
    ClampToWorkArea(x, y, winW, winH, mi.rcWork);
    if (!IsIconic(hWnd) && !IsZoomed(hWnd)) {
        g_lockWinW = winW;
        g_lockWinH = winH;
        SetWindowPos(hWnd, nullptr, x, y, winW, winH, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    PlaceNativeCheck(hWnd);
}

static void SetDefaultFocus() {
    std::vector<LayoutItem> items;
    ComputeLayout(items);
    int defOpt = DefaultOptionIndex();
    g_focusItem = -1;
    g_hoverItem = -1;
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].type == LayoutItem::OPTION && items[i].optIndex == defOpt) {
            g_focusItem = i;
            break;
        }
    }
    g_kbdFocus = false;
}

static int HitTestLayout(POINT pt) {
    std::vector<LayoutItem> items;
    ComputeLayout(items);
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].type == LayoutItem::OPTION ||
            items[i].type == LayoutItem::LINK) {
            if (PtInRect(&items[i].rc, pt)) return i;
        }
    }
    return -1;
}

static void ActivateLayoutItem(HWND hWnd, int item) {
    std::vector<LayoutItem> items;
    ComputeLayout(items);
    if (item < 0 || item >= (int)items.size()) return;
    const LayoutItem& it = items[item];
    if (it.type == LayoutItem::OPTION)
        ExecuteOptionByIndex(it.optIndex, hWnd);
    else if (it.type == LayoutItem::LINK)
        ExecuteControlPanelLink();
    else if (it.type == LayoutItem::CHECK && g_hwndCheck) {
        SendMessageW(g_hwndCheck, BM_CLICK, 0, 0);
    }
}

static void MoveFocus(HWND hWnd, int dir) {
    std::vector<LayoutItem> items;
    ComputeLayout(items);
    std::vector<int> tabs;
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].type == LayoutItem::CHECK ||
            items[i].type == LayoutItem::OPTION ||
            items[i].type == LayoutItem::LINK)
            tabs.push_back(i);
    }
    if (tabs.empty()) return;
    int cur = 0;
    for (int i = 0; i < (int)tabs.size(); i++)
        if (tabs[i] == g_focusItem) { cur = i; break; }
    cur = (cur + dir + (int)tabs.size()) % (int)tabs.size();
    g_focusItem = tabs[cur];
    g_hoverItem = (items[g_focusItem].type == LayoutItem::OPTION) ? g_focusItem : -1;
    g_kbdFocus = true;
    if (items[g_focusItem].type == LayoutItem::CHECK && g_hwndCheck)
        SetFocus(g_hwndCheck);
    else
        SetFocus(hWnd);
    InvalidateRect(hWnd, nullptr, FALSE);
    NotifyWinEvent(EVENT_OBJECT_FOCUS, hWnd, OBJID_CLIENT, g_focusItem + 1);
}

static void ShowAutoPlayDialog();
static void BuildDriveDialog(int letter, bool forceDialog);
static void ProcessPendingQueue();

static void ShowAutoPlayDialog() {
    if (g_hwndDialog) {
        DestroyWindow(g_hwndDialog);
        g_hwndDialog = nullptr;
    }

    if (!g_dialogClassRegistered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = AutoPlayDialogProc;
        wc.hInstance = HINST_THISCOMPONENT;
        wc.lpszClassName = kDialogClass;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        if (!RegisterClassW(&wc)) {
            Wh_Log(L"ShowAutoPlayDialog: RegisterClassW failed %lu", GetLastError());
            return;
        }
        g_dialogClassRegistered = true;
    }

    HMONITOR hMon = MonitorForDialog();
    UINT dpi = 96;
    typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
    HMODULE shcore = GetModuleHandleW(L"shcore.dll");
    if (shcore) {
        auto fn = (GetDpiForMonitor_t)GetProcAddress(shcore, "GetDpiForMonitor");
        if (fn) { UINT dx = 96, dy = 96; if (S_OK == fn(hMon, 0, &dx, &dy)) dpi = dx; }
    }
    g_dpi = dpi ? dpi : 96;
    EnsureDpiResources();
    RebindSharedIcons();

    std::vector<LayoutItem> items;
    int clientH = ComputeLayout(items);
    int clientW = Scale(W_WIDTH);

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                  WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    DWORD exStyle = WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME | WS_EX_APPWINDOW;
    RECT rc = { 0, 0, clientW, clientH };
    AdjustWindowRectExForDpi(&rc, style, FALSE, exStyle, g_dpi);
    int winW = rc.right - rc.left, winH = rc.bottom - rc.top;
    g_lockWinW = winW;
    g_lockWinH = winH;

    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);
    int x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - winW) / 2;
    int y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - winH) / 2;
    ClampToWorkArea(x, y, winW, winH, mi.rcWork);

    const LangPack* lp = L();
    g_alwaysChecked = false;
    g_pressedItem = -1;
    SetDefaultFocus();

    g_hwndDialog = CreateWindowExW(exStyle, kDialogClass,
                                   lp->windowTitle, style, x, y, winW, winH,
                                   NULL, NULL, HINST_THISCOMPONENT, nullptr);
    if (!g_hwndDialog) {
        Wh_Log(L"ShowAutoPlayDialog: CreateWindowExW failed %lu", GetLastError());
        return;
    }

    if (g_hwndIconBig) { DestroyIcon(g_hwndIconBig); g_hwndIconBig = nullptr; }
    if (g_hwndIconSmall) { DestroyIcon(g_hwndIconSmall); g_hwndIconSmall = nullptr; }
    // Title-bar icon: local (fixed) disks use the silver local-disk icon,
    // removable drives keep the current removable icon, with fallbacks.
    const WCHAR* bigB64 = (g_driveType == DRIVE_FIXED) ? USER_LOCALDISK_ICON_BASE64
                                                       : USER_REMOVABLE_ICON_BASE64;
    g_hwndIconBig = CreateIconFromBase64PNG(bigB64, GetSystemMetrics(SM_CXICON),
                                            GetSystemMetrics(SM_CYICON));
    if (!g_hwndIconBig)
        g_hwndIconBig = CreateIconFromBase64PNG(USER_DRIVE_ICON_BASE64, GetSystemMetrics(SM_CXICON),
                                                GetSystemMetrics(SM_CYICON));
    g_hwndIconSmall = CreateIconFromBase64PNG(bigB64, GetSystemMetrics(SM_CXSMICON),
                                              GetSystemMetrics(SM_CYSMICON));
    if (!g_hwndIconSmall)
        g_hwndIconSmall = CreateIconFromBase64PNG(USER_DRIVE_ICON_BASE64, GetSystemMetrics(SM_CXSMICON),
                                                  GetSystemMetrics(SM_CYSMICON));
    if (!g_hwndIconBig) g_hwndIconBig = GetShellIcon(g_driveRoot.c_str(), 0);
    if (!g_hwndIconSmall) g_hwndIconSmall = GetShellIcon(g_driveRoot.c_str(), 1);
    if (g_hwndIconBig) SendMessageW(g_hwndDialog, WM_SETICON, ICON_BIG, (LPARAM)g_hwndIconBig);
    if (g_hwndIconSmall) SendMessageW(g_hwndDialog, WM_SETICON, ICON_SMALL, (LPARAM)g_hwndIconSmall);

    PlaceNativeCheck(g_hwndDialog);
    ShowWindow(g_hwndDialog, SW_SHOWNORMAL);
    UpdateWindow(g_hwndDialog);
    SetForegroundWindow(g_hwndDialog);
    SetFocus(g_hwndDialog);
}

// ============================================================================
// ============================================================================
static void BuildWpdDialog(const std::wstring& devicePath, bool forceDialog);

static void BuildDriveDialog(int letter, bool forceDialog) {
    try {
    if (!DrivePresent(letter)) return;
    UINT dt0 = GetDriveTypeW(GetDriveRootForLetter(letter).c_str());
    if (!VolumeAllowedByPolicy(letter, dt0)) {
        Wh_Log(L"BuildDriveDialog: skip %c: reason=policy", letter);
        return;
    }

    g_isWpd = false;
    g_wpdPath.clear();
    g_wpdId.clear();
    g_driveRoot = GetDriveRootForLetter(letter);
    g_driveLetter = letter;
    g_driveType = GetDriveTypeW(g_driveRoot.c_str());

    std::wstring volName, fsName;
    bool ready = VolumeReady(g_driveRoot, &volName, &fsName);
    g_audioCd = (g_driveType == DRIVE_CDROM) && LooksLikeAudioCd(g_driveRoot);
    if (!ready && g_driveType != DRIVE_CDROM) return;
    if (!ready && g_driveType == DRIVE_CDROM)
        Wh_Log(L"BuildDriveDialog: optical not ready, audio=%d (show anyway)", g_audioCd ? 1 : 0);

    if (g_hwndDialog) {
        DestroyWindow(g_hwndDialog);
        g_hwndDialog = nullptr;
    }

    AutorunInfo ar;
    MediaInventory inv;
    if (ready && !g_audioCd) {
        ParseAutorunInf(g_driveRoot, ar);
        inv = ScanVolumeShallow(g_driveRoot);
    }

    const LangPack* lp = L();
    std::wstring generic = (g_driveType == DRIVE_CDROM) ? lp->cdDrive
                         : (g_driveType == DRIVE_REMOVABLE) ? lp->removableDisk
                         : lp->localDisk;
    wchar_t title[260];
    swprintf_s(title, ARRAYSIZE(title), L"%s (%c:)", generic.c_str(), letter);
    g_driveTitle = title;

    EnsureDpiResources();
    RebuildHeaderIcon();
    BuildOptions(ar, inv);

    std::wstring label = ar.label;
    if (label.empty()) label = volName;
    if (g_settings.hideDeviceNames) {
        g_headerSub.clear();
        if (g_driveType == DRIVE_CDROM)
            g_driveTitle = lp->cdDrive;
        else if (g_driveType == DRIVE_FIXED)
            g_driveTitle = lp->localDisk;
        else
            g_driveTitle = lp->removableDisk;
        Wh_Log(L"BuildDriveDialog: privacy, generic title");
    } else if (g_audioCd)
        g_headerSub = lp->audioCd;
    else if (g_contentKind == ContentKind::DvdMovie)
        g_headerSub = lp->cdDrive;
    else if (g_contentKind == ContentKind::BlankDisc)
        g_headerSub.clear();
    else if (!label.empty() && _wcsicmp(label.c_str(), generic.c_str()) != 0)
        g_headerSub = label;
    else
        g_headerSub.clear();

    if (!forceDialog && TryExecuteRemembered()) {
        Wh_Log(L"BuildDriveDialog: remembered action for %c:", letter);
        return;
    }
    ShowAutoPlayDialog();
    } catch (...) {
        Wh_Log(L"BuildDriveDialog: exception letter=%c", letter);
    }
}

static void BuildWpdDialog(const std::wstring& devicePath, bool forceDialog) {
    try {
    if (IsAutoPlayGloballyDisabled() || IsNonVolumeAutoPlayBlocked()) {
        Wh_Log(L"BuildWpdDialog: skip reason=policy");
        return;
    }
    if (!g_settings.includeMtpDevices) return;

    std::wstring id, name, mfr;
    ResolveWpdDevice(devicePath, id, name, mfr);
    if (id.empty() && devicePath.empty()) return;

    if (g_hwndDialog) {
        DestroyWindow(g_hwndDialog);
        g_hwndDialog = nullptr;
    }

    g_isWpd = true;
    g_wpdPath = devicePath;
    g_wpdId = id.empty() ? devicePath : id;
    g_driveLetter = 0;
    g_driveType = DRIVE_UNKNOWN;
    g_driveRoot.clear();
    g_audioCd = false;

    const LangPack* lp = L();
    if (g_settings.hideDeviceNames) {
        g_driveTitle = lp->portableDevice;
        g_headerSub.clear();
        Wh_Log(L"BuildWpdDialog: privacy, generic title");
    } else {
        g_driveTitle = !name.empty() ? name : lp->portableDevice;
        g_headerSub = !mfr.empty() ? mfr : lp->portableDevice;
    }

    AutorunInfo ar;
    MediaInventory inv;
    EnsureDpiResources();
    RebuildHeaderIcon();
    g_contentKind = ContentKind::Portable;
    g_hasProgramSection = false;
    BuildOptions(ar, inv);
    g_prompt.clear();

    if (!forceDialog && TryExecuteRemembered()) {
        Wh_Log(L"BuildWpdDialog: remembered action");
        return;
    }
    ShowAutoPlayDialog();
    } catch (...) {
        Wh_Log(L"BuildWpdDialog: exception");
    }
}

// ============================================================================
// ============================================================================
LRESULT CALLBACK AutoPlayDialogProc(HWND hWnd, UINT uMsg,
                                    WPARAM wParam, LPARAM lParam) {
    if (g_msgQueryCancelAP && uMsg == g_msgQueryCancelAP)
        return (g_settings.suppressNativeAutoPlay && g_hUiThread) ? TRUE : FALSE;
    switch (uMsg) {
    case WM_NCCREATE:
        // Optional-list fix: EnableNonClientDpiScaling only has an effect when
        // called during WM_NCCREATE; calling it after CreateWindowExW returns
        // (the old call site, right before ShowWindow) is too late and the
        // non-client area silently won't scale on a DPI change.
        EnableNonClientDpiScaling(hWnd);
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    case WM_CREATE:
        g_dpi = GetBestDpiForWindow(hWnd);
        // Item 5 fix: if GetDpiForWindow() here disagrees with the
        // GetDpiForMonitor() value ShowAutoPlayDialog() used to size this window,
        // EnsureDpiResources() below recreates every shared bitmap/icon out from
        // under g_options[i].icon / g_hdrIcon. Rebind afterwards, exactly like the
        // WM_DPICHANGED path does, so WM_PAINT never draws through a destroyed
        // (possibly recycled) GDI handle.
        EnsureDpiResourcesAndRebind();
        return 0;
    case WM_GETDLGCODE:
        return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;
    case WM_NCHITTEST: {
        LRESULT ht = DefWindowProcW(hWnd, uMsg, wParam, lParam);
        if (ht >= HTLEFT && ht <= HTBOTTOMRIGHT)
            return HTCAPTION;
        return ht;
    }
    case WM_GETMINMAXINFO: {
        if (g_lockWinW > 0 && g_lockWinH > 0) {
            MINMAXINFO* mm = (MINMAXINFO*)lParam;
            mm->ptMinTrackSize.x = g_lockWinW;
            mm->ptMinTrackSize.y = g_lockWinH;
            mm->ptMaxTrackSize.x = g_lockWinW;
            mm->ptMaxTrackSize.y = g_lockWinH;
        }
        return 0;
    }
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_RESTORE) {
            ShowWindow(hWnd, SW_RESTORE);
            return 0;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        PaintDialog(hWnd, hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEMOVE: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int hit = HitTestLayout(pt);
        if (hit != g_hoverItem) {
            g_hoverItem = hit;
            g_kbdFocus = false;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        if (!g_trackingLeave) {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
            TrackMouseEvent(&tme);
            g_trackingLeave = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        g_trackingLeave = false;
        if (g_hoverItem != -1) {
            g_hoverItem = -1;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT) {
            POINT pt = {};
            GetCursorPos(&pt);
            ScreenToClient(hWnd, &pt);
            std::vector<LayoutItem> items;
            ComputeLayout(items);
            int hit = HitTestLayout(pt);
            if (hit >= 0 && hit < (int)items.size() && items[hit].type == LayoutItem::LINK) {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                return TRUE;
            }
        }
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    case WM_LBUTTONDOWN: {
        SetFocus(hWnd);
        g_kbdFocus = false;
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_pressedItem = HitTestLayout(pt);
        if (g_pressedItem >= 0) {
            g_focusItem = g_pressedItem;
            g_hoverItem = g_pressedItem;
            SetCapture(hWnd);
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int hit = HitTestLayout(pt);
        int pressed = g_pressedItem;
        g_pressedItem = -1;
        if (GetCapture() == hWnd) ReleaseCapture();
        if (hit >= 0 && hit == pressed)
            ActivateLayoutItem(hWnd, hit);
        else
            InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }
    case WM_CAPTURECHANGED:
        if (g_pressedItem != -1) {
            g_pressedItem = -1;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    case WM_LBUTTONDBLCLK:
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            DestroyWindow(hWnd);
            return 0;
        }
        if (wParam == VK_TAB) {
            MoveFocus(hWnd, (GetKeyState(VK_SHIFT) & 0x8000) ? -1 : 1);
            return 0;
        }
        if (wParam == VK_DOWN || wParam == VK_RIGHT) {
            MoveFocus(hWnd, 1);
            return 0;
        }
        if (wParam == VK_UP || wParam == VK_LEFT) {
            MoveFocus(hWnd, -1);
            return 0;
        }
        if (wParam == VK_HOME || wParam == VK_END) {
            std::vector<LayoutItem> items;
            ComputeLayout(items);
            int first = -1, last = -1;
            for (int i = 0; i < (int)items.size(); i++) {
                if (items[i].type == LayoutItem::OPTION) {
                    if (first < 0) first = i;
                    last = i;
                }
            }
            int dest = (wParam == VK_HOME) ? first : last;
            if (dest >= 0) {
                g_focusItem = dest;
                g_hoverItem = dest;
                g_kbdFocus = true;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }
        if (wParam == VK_RETURN) {
            if (g_focusItem >= 0) ActivateLayoutItem(hWnd, g_focusItem);
            return 0;
        }
        if (wParam == VK_SPACE) {
            std::vector<LayoutItem> items;
            ComputeLayout(items);
            if (g_focusItem >= 0 && g_focusItem < (int)items.size() &&
                items[g_focusItem].type == LayoutItem::CHECK) {
                g_alwaysChecked = !g_alwaysChecked;
                InvalidateRect(hWnd, nullptr, FALSE);
            } else if (g_focusItem >= 0) {
                ActivateLayoutItem(hWnd, g_focusItem);
            }
            return 0;
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_ALWAYS && HIWORD(wParam) == BN_CLICKED) {
            if (g_hwndCheck)
                g_alwaysChecked = (SendMessageW(g_hwndCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
            Wh_Log(L"Always checkbox: %d", g_alwaysChecked ? 1 : 0);
            return 0;
        }
        break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
        if ((HWND)lParam == g_hwndCheck) {
            HDC hdc = (HDC)wParam;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, IsHighContrastOn() ? GetSysColor(COLOR_WINDOWTEXT) : RGB(20, 20, 20));
            return (LRESULT)GetStockObject(WHITE_BRUSH);
        }
        break;
    case WM_DPICHANGED: {
        g_dpi = HIWORD(wParam);
        if (g_dpi < 96) g_dpi = 96;
        EnsureDpiResources();
        RebuildHeaderIcon();
        RebindSharedIcons();
        ApplyDialogSize(hWnd, (RECT*)lParam);
        PlaceNativeCheck(hWnd);
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }
    case WM_DISPLAYCHANGE:
        EnsureOnScreen(hWnd);
        return 0;
    case WM_SETTINGCHANGE:
    case WM_THEMECHANGED:
    case WM_SYSCOLORCHANGE:
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    case WMU_SELF_REBUILD:
        if (g_hwndListener) PostMessageW(g_hwndListener, WMU_REBUILD, 0, 0);
        return 0;
    case WM_NCDESTROY:
        g_hwndCheck = nullptr;
        if (g_hwndDialog == hWnd) g_hwndDialog = nullptr;
        g_hoverItem = -1;
        g_focusItem = -1;
        g_pressedItem = -1;
        g_hotRects.clear();
        if (g_hwndIconBig) { DestroyIcon(g_hwndIconBig); g_hwndIconBig = nullptr; }
        if (g_hwndIconSmall) { DestroyIcon(g_hwndIconSmall); g_hwndIconSmall = nullptr; }
        FreeOptions();
        if (g_hwndListener) PostMessageW(g_hwndListener, WMU_PROCESS_QUEUE, 0, 0);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// ============================================================================
// ============================================================================
static const WCHAR* kListenerClass = L"Win7ClassicAutoPlayListener";
static bool g_listenerClassRegistered = false;

static bool PendingContains(int letter) {
    for (const auto& p : g_pending)
        if (!p.isWpd && p.letter == letter) return true;
    return false;
}

static bool PendingContainsWpd(const std::wstring& path) {
    for (const auto& p : g_pending)
        if (p.isWpd && WpdIdsMatch(p.wpdPath, path)) return true;
    return false;
}

static void RemovePending(int letter) {
    g_pending.erase(std::remove_if(g_pending.begin(), g_pending.end(),
        [letter](const PendingVolume& p) { return !p.isWpd && p.letter == letter; }),
        g_pending.end());
}

static void EnsureReadyTimer() {
    if (g_hwndListener && !g_pending.empty())
        SetTimer(g_hwndListener, IDT_READY, 250, nullptr);
}

static void DropPendingWpdAliases() {
    g_pending.erase(std::remove_if(g_pending.begin(), g_pending.end(),
        [](const PendingVolume& p) {
            return p.isWpd && IsWpdMassStorageAlias(p.wpdPath.c_str());
        }), g_pending.end());
}

static void HandleVolumeArrival(DWORD unitmask) {
    try {
    if (IsAutoPlayGloballyDisabled()) {
        Wh_Log(L"HandleVolumeArrival: skip mask=0x%X reason=DisableAutoplay", unitmask);
        return;
    }
    DropPendingWpdAliases();
    for (int i = 0; i < 26; i++) {
        if (!(unitmask & (1u << i))) continue;
        int letter = 'A' + i;
        if (IsSystemDriveLetter(letter)) {
            Wh_Log(L"HandleVolumeArrival: skip %c: reason=system-drive", letter);
            continue;
        }
        std::wstring root = GetDriveRootForLetter(letter);
        UINT dt = GetDriveTypeW(root.c_str());
        if (!VolumeAllowedByPolicy(letter, dt)) {
            Wh_Log(L"HandleVolumeArrival: skip %c: type=%u reason=policy", letter, dt);
            continue;
        }
        bool eligible = DriveTypeWeHandle(dt, root.c_str());
        if (!eligible) {
            Wh_Log(L"HandleVolumeArrival: skip %c: type=%u reason=not-eligible", letter, dt);
            continue;
        }
        if (g_hwndDialog && !g_isWpd && g_driveLetter == letter) {
            Wh_Log(L"HandleVolumeArrival: skip %c: reason=dialog-already-open", letter);
            continue;
        }
        if (PendingContains(letter)) {
            Wh_Log(L"HandleVolumeArrival: skip %c: reason=already-queued", letter);
            continue;
        }
        PendingVolume pv;
        pv.letter = letter;
        pv.tries = 0;
        pv.firstTick = GetTickCount64();
        g_pending.push_back(pv);
        Wh_Log(L"HandleVolumeArrival: queued %c: type=%u pending=%u",
               letter, dt, (unsigned)g_pending.size());
    }
    EnsureReadyTimer();
    } catch (...) {
        Wh_Log(L"HandleVolumeArrival: exception");
    }
}

static void ProcessPendingQueue() {
    try {
    // Only a blocking action worker shares the globals we need; a dialog being
    // open must NOT freeze the queue. Otherwise inserting a second drive (Z:)
    // while the dialog for an already-mounted drive (D:) is open would leave Z:
    // queued forever and you would keep seeing the D: dialog (or the native
    // AutoPlay if D: is not present). BuildDriveDialog() already destroys and
    // replaces a dialog belonging to a different drive.
    if (g_hActionWorker)
        return;

    for (size_t i = 0; i < g_pending.size(); ) {
        if (g_pending[i].isWpd) {
            if (IsWpdMassStorageAlias(g_pending[i].wpdPath.c_str())) {
                Wh_Log(L"ProcessPending: drop WPD USBSTOR alias");
                g_pending.erase(g_pending.begin() + i);
                continue;
            }
            std::wstring id, name, mfr;
            bool resolved = ResolveWpdDevice(g_pending[i].wpdPath, id, name, mfr);
            if (resolved && (!name.empty() || g_pending[i].tries >= 2)) {
                std::wstring path = g_pending[i].wpdPath;
                g_pending.erase(g_pending.begin() + i);
                // Same rule as volumes: if this device already has its dialog
                // open, do nothing; otherwise BuildWpdDialog replaces it.
                if (g_hwndDialog && g_isWpd &&
                    (WpdIdsMatch(g_wpdPath, path) || WpdIdsMatch(g_wpdId, path))) {
                    Wh_Log(L"ProcessPending: WPD dialog already open, drop");
                    if (g_pending.empty() && g_hwndListener)
                        KillTimer(g_hwndListener, IDT_READY);
                    return;
                }
                Wh_Log(L"ProcessPending: show WPD remaining=%u", (unsigned)g_pending.size());
                BuildWpdDialog(path, false);
                if (g_pending.empty() && g_hwndListener)
                    KillTimer(g_hwndListener, IDT_READY);
                return;
            }
            if (++g_pending[i].tries > 16) {
                Wh_Log(L"ProcessPending: WPD timeout, drop");
                g_pending.erase(g_pending.begin() + i);
                continue;
            }
            ++i;
            continue;
        }
        int letter = g_pending[i].letter;
        if (!DrivePresent(letter)) {
            g_pending.erase(g_pending.begin() + i);
            continue;
        }
        std::wstring root = GetDriveRootForLetter(letter);
        UINT dt = GetDriveTypeW(root.c_str());
        if (!VolumeAllowedByPolicy(letter, dt)) {
            g_pending.erase(g_pending.begin() + i);
            continue;
        }
        bool ready = VolumeReady(root);
        bool audioGuess = (dt == DRIVE_CDROM) && LooksLikeAudioCd(root);
        bool opticalGiveUp = (dt == DRIVE_CDROM) && g_pending[i].tries >= 4;
        if (ready || audioGuess || opticalGiveUp) {
            g_pending.erase(g_pending.begin() + i);
            // If the classic dialog is already open for THIS same drive, it is
            // already shown - nothing new to do. A dialog for a DIFFERENT drive
            // is replaced by BuildDriveDialog() below, so the newly inserted
            // volume always gets its own dialog instead of the old one's.
            if (g_hwndDialog && !g_isWpd && g_driveLetter == letter) {
                Wh_Log(L"ProcessPending: dialog already open for %c:, drop", letter);
                if (g_pending.empty() && g_hwndListener)
                    KillTimer(g_hwndListener, IDT_READY);
                return;
            }
            Wh_Log(L"ProcessPending: show %c: remaining=%u", letter, (unsigned)g_pending.size());
            BuildDriveDialog(letter, false);
            if (g_pending.empty() && g_hwndListener)
                KillTimer(g_hwndListener, IDT_READY);
            return;
        }
        if (++g_pending[i].tries > 32) {
            Wh_Log(L"ProcessPending: timeout %c:, drop", letter);
            g_pending.erase(g_pending.begin() + i);
            continue;
        }
        ++i;
    }
    if (g_pending.empty() && g_hwndListener)
        KillTimer(g_hwndListener, IDT_READY);
    } catch (...) {
        Wh_Log(L"ProcessPendingQueue: exception");
    }
}

static void OnReadyTimer() {
    ProcessPendingQueue();
}

static void HandleWpdArrival(const wchar_t* devicePath) {
    try {
    if (!g_settings.includeMtpDevices) {
        Wh_Log(L"HandleWpdArrival: skip reason=mtp-disabled");
        return;
    }
    if (IsAutoPlayGloballyDisabled() || IsNonVolumeAutoPlayBlocked()) {
        Wh_Log(L"HandleWpdArrival: skip reason=policy");
        return;
    }
    if (!devicePath || !*devicePath) return;
    if (IsWpdMassStorageAlias(devicePath)) {
        Wh_Log(L"HandleWpdArrival: skip USBSTOR volume alias");
        return;
    }
    if (PendingContainsWpd(devicePath)) {
        Wh_Log(L"HandleWpdArrival: skip already-queued");
        return;
    }
    if (g_hwndDialog && g_isWpd && WpdIdsMatch(g_wpdPath, devicePath)) {
        Wh_Log(L"HandleWpdArrival: skip dialog-already-open");
        return;
    }
    PendingVolume pv;
    pv.isWpd = true;
    pv.wpdPath = devicePath;
    pv.tries = 0;
    pv.firstTick = GetTickCount64();
    g_pending.push_back(pv);
    Wh_Log(L"HandleWpdArrival: queued MTP/WPD pending=%u", (unsigned)g_pending.size());
    EnsureReadyTimer();
    } catch (...) {
        Wh_Log(L"HandleWpdArrival: exception");
    }
}

static void HandleWpdRemoved(const wchar_t* devicePath) {
    if (!devicePath || !*devicePath) return;
    g_pending.erase(std::remove_if(g_pending.begin(), g_pending.end(),
        [devicePath](const PendingVolume& p) {
            return p.isWpd && WpdIdsMatch(p.wpdPath, devicePath);
        }), g_pending.end());
    if (g_hwndDialog && g_isWpd &&
        (WpdIdsMatch(g_wpdPath, devicePath) || WpdIdsMatch(g_wpdId, devicePath)))
        DestroyWindow(g_hwndDialog);
    if (g_pending.empty() && g_hwndListener)
        KillTimer(g_hwndListener, IDT_READY);
}

static void HandleVolumeRemoved(DWORD unitmask) {
    for (int i = 0; i < 26; i++) {
        if (!(unitmask & (1u << i))) continue;
        int letter = 'A' + i;
        RemovePending(letter);
        ReleaseContextMenuEntriesForLetter(letter);
        if (g_hwndDialog && !g_isWpd && g_driveLetter == letter)
            DestroyWindow(g_hwndDialog);
    }
    if (g_pending.empty() && g_hwndListener)
        KillTimer(g_hwndListener, IDT_READY);
}

LRESULT CALLBACK ListenerWndProc(HWND hWnd, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam) {
    if (g_msgQueryCancelAP && uMsg == g_msgQueryCancelAP)
        return (g_settings.suppressNativeAutoPlay && g_hUiThread) ? TRUE : FALSE;
    switch (uMsg) {
    case WM_DEVICECHANGE:
        if (wParam == DBT_DEVICEARRIVAL) {
            DEV_BROADCAST_HDR* hdr = (DEV_BROADCAST_HDR*)lParam;
            if (hdr && hdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                DEV_BROADCAST_VOLUME* vol = (DEV_BROADCAST_VOLUME*)hdr;
                HandleVolumeArrival(vol->dbcv_unitmask);
            } else if (hdr && hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                auto* di = (DEV_BROADCAST_DEVICEINTERFACE_W*)hdr;
                if (IsEqualGUID(di->dbcc_classguid, GUID_DEVINTERFACE_WPD_))
                    HandleWpdArrival(di->dbcc_name);
            }
        } else if (wParam == DBT_DEVICEREMOVECOMPLETE ||
                   wParam == DBT_DEVICEQUERYREMOVE ||
                   wParam == DBT_DEVICEREMOVEPENDING) {
            DEV_BROADCAST_HDR* hdr = (DEV_BROADCAST_HDR*)lParam;
            if (hdr && hdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
                HandleVolumeRemoved(((DEV_BROADCAST_VOLUME*)hdr)->dbcv_unitmask);
            else if (hdr && hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                auto* di = (DEV_BROADCAST_DEVICEINTERFACE_W*)hdr;
                if (IsEqualGUID(di->dbcc_classguid, GUID_DEVINTERFACE_WPD_))
                    HandleWpdRemoved(di->dbcc_name);
            }
        }
        return TRUE;
    case WM_TIMER:
        if (wParam == IDT_READY) OnReadyTimer();
        return 0;
    case WMU_PROCESS_QUEUE:
        ProcessPendingQueue();
        return 0;
    case WMU_REBUILD:
        // Skip while an action worker is running: the worker reads the shared
        // globals below (g_driveRoot/g_driveLetter/g_isWpd/...), and Rebuild
        // would rewrite them concurrently (item 7). The next settings change or
        // queue event will pick the work up again.
        if (g_hActionWorker) { Wh_Log(L"WMU_REBUILD: deferred, worker running"); return 0; }
        if (g_hwndDialog) {
            g_dpi = (int)GetDpiForWindow(g_hwndDialog);
            if (g_dpi < 96) g_dpi = 96;
            if (g_isWpd)
                BuildWpdDialog(g_wpdPath.empty() ? g_wpdId : g_wpdPath, true);
            else if (g_driveLetter)
                BuildDriveDialog(g_driveLetter, true);
        }
        return 0;
    case WMU_ACTION_DONE:
        if (g_hActionWorker) {
            WaitForSingleObject(g_hActionWorker, INFINITE);
            CloseHandle(g_hActionWorker);
            g_hActionWorker = nullptr;
        }
        ProcessPendingQueue();
        return 0;
    case WMU_CONTEXT_AUTOPLAY: {
        // Never fall back to the last inserted volume: this request belongs to
        // the drive represented by the context-menu object.
        // Defer while an action worker is running so we never rewrite the globals
        // the worker is reading (item 7).
        if (g_hActionWorker) { Wh_Log(L"WMU_CONTEXT_AUTOPLAY: deferred, worker running"); return 0; }
        int letter = (int)wParam;
        if (letter >= L'A' && letter <= L'Z' && DrivePresent(letter))
            BuildDriveDialog(letter, true);
        else
            Wh_Log(L"WMU_CONTEXT_AUTOPLAY: invalid or missing drive=%d", letter);
        return 0;
    }
    case WMU_APPLY_SUPPRESS:
        RegisterCancelAutoPlay();
        return 0;
    case WMU_SHUTDOWN:
        KillTimer(hWnd, IDT_READY);
        g_pending.clear();
        if (g_hwndDialog) {
            DestroyWindow(g_hwndDialog);
            g_hwndDialog = nullptr;
        }
        if (g_hDevNotify) {
            UnregisterDeviceNotification(g_hDevNotify);
            g_hDevNotify = nullptr;
        }
        UnregisterCancelAutoPlay();
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        KillTimer(hWnd, IDT_READY);
        if (g_hDevNotify) {
            UnregisterDeviceNotification(g_hDevNotify);
            g_hDevNotify = nullptr;
        }
        g_hwndListener = nullptr;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI AutoPlayUiThreadProc(LPVOID) {
    try {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&g_wic)))) {
        g_wic = nullptr;
        Wh_Log(L"UiThread: WIC unavailable, embedded icons disabled");
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = ListenerWndProc;
    wc.hInstance = HINST_THISCOMPONENT;
    wc.lpszClassName = kListenerClass;
    if (RegisterClassW(&wc)) g_listenerClassRegistered = true;

    g_hwndListener = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                     kListenerClass, L"", WS_POPUP,
                                     0, 0, 0, 0, NULL, NULL,
                                     HINST_THISCOMPONENT, nullptr);
    if (g_evtListenerReady) SetEvent(g_evtListenerReady);
    if (!g_hwndListener) {
        Wh_Log(L"UiThread: listener window failed %lu", GetLastError());
        if (g_wic) { g_wic->Release(); g_wic = nullptr; }
        CoUninitialize();
        return 1;
    }

    g_msgQueryCancelAP = RegisterWindowMessageW(L"QueryCancelAutoPlay");

    DEV_BROADCAST_DEVICEINTERFACE_W flt = {};
    flt.dbcc_size = sizeof(flt);
    flt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    flt.dbcc_classguid = GUID_DEVINTERFACE_WPD_;
    g_hDevNotify = RegisterDeviceNotificationW(g_hwndListener, &flt,
                                               DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!g_hDevNotify)
        Wh_Log(L"UiThread: RegisterDeviceNotification WPD failed %lu", GetLastError());

    RegisterCancelAutoPlay();

    // Warm up the drive-letter association on this COM-initialised thread
    // (item 3): shell extensions are built here, not on Explorer's startup
    // path, and the menus are (re)leased on this same thread before COM shuts
    // down, so there is no cross-apartment Release from an uninitialised thread.
    WarmUpDriveContextMenus();
    ReleaseContextMenuWarmupMenus();

    Wh_Log(L"UiThread: ready pid=%lu", GetCurrentProcessId());

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (g_hwndDialog && IsWindow(g_hwndDialog) &&
            (msg.hwnd == g_hwndDialog || IsChild(g_hwndDialog, msg.hwnd))) {
            if (msg.message == WM_KEYDOWN &&
                (msg.wParam == VK_TAB || msg.wParam == VK_ESCAPE ||
                 msg.wParam == VK_RETURN || msg.wParam == VK_SPACE ||
                 msg.wParam == VK_UP || msg.wParam == VK_DOWN ||
                 msg.wParam == VK_LEFT || msg.wParam == VK_RIGHT ||
                 msg.wParam == VK_HOME || msg.wParam == VK_END)) {
                SendMessageW(g_hwndDialog, msg.message, msg.wParam, msg.lParam);
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hDevNotify) {
        UnregisterDeviceNotification(g_hDevNotify);
        g_hDevNotify = nullptr;
    }
    UnregisterCancelAutoPlay();
    if (g_wic) { g_wic->Release(); g_wic = nullptr; }
    CoUninitialize();
    return 0;
    } catch (...) {
        Wh_Log(L"UiThread: exception");
        try { UnregisterCancelAutoPlay(); } catch (...) {}
        if (g_wic) { g_wic->Release(); g_wic = nullptr; }
        CoUninitialize();
        return 1;
    }
}

// ============================================================================
// ============================================================================
BOOL Wh_ModInit() {
    try {
        LoadSettings();
        // Do not touch CancelAutoplay\CLSID before ownership is acquired:
        // secondary Explorer instances are injected too and could erase the
        // values owned by the shell instance. RegisterCancelAutoPlay() performs
        // the cleanup/re-registration after ownership is established.
        const DWORD pid = GetCurrentProcessId();
        const DWORD tray = GetTrayOwnerPid();
        const bool mainShell = IsMainExplorerShell();
        Wh_Log(L"Wh_ModInit: pid=%lu trayOwner=%lu mainShell=%d", pid, tray, mainShell ? 1 : 0);

        if (!mainShell) {
            Wh_Log(L"Wh_ModInit: secondary explorer, listener not started");
            return TRUE;
        }
        if (!TryBecomeAutoPlayOwner()) {
            Wh_Log(L"Wh_ModInit: another instance already owns AutoPlay");
            return TRUE;
        }

        Wh_SetFunctionHook((void*)ShellExecuteExW, (void*)ShellExecuteExWHook,
                           (void**)&g_ShellExecuteExW_Original);
        Wh_SetFunctionHook((void*)ShellExecuteW, (void*)ShellExecuteWHook,
                           (void**)&g_ShellExecuteW_Original);
        Wh_SetFunctionHook((void*)ShellExecuteExA, (void*)ShellExecuteExAHook,
                           (void**)&g_ShellExecuteExA_Original);
        Wh_SetFunctionHook((void*)ShellExecuteA, (void*)ShellExecuteAHook,
                           (void**)&g_ShellExecuteA_Original);
        Wh_SetFunctionHook((void*)CreateProcessW, (void*)CreateProcessWHook,
                           (void**)&g_CreateProcessW_Original);
        Wh_SetFunctionHook((void*)CreateProcessA, (void*)CreateProcessAHook,
                           (void**)&g_CreateProcessA_Original);
        Wh_SetFunctionHook((void*)TrackPopupMenuEx,
                           (void*)TrackPopupMenuExHook,
                           (void**)&g_TrackPopupMenuEx_Original);
        Wh_SetFunctionHook((void*)TrackPopupMenu,
                           (void*)TrackPopupMenuHook,
                           (void**)&g_TrackPopupMenu_Original);
        // Initialise the menu-association critical section eagerly on the main
        // thread, before any hook callback can run. This removes the lazy-init
        // race (item 1).
        EnsureContextMenusCS();

        // Prefer the AutoPlay COM implementation; fall back to a real drive
        // context-menu object only if the probe is unavailable. Installing two
        // different vtable detours into the same global originals can recurse.
        if (!InstallAutoplayContextMenuHooks())
            InstallDriveContextMenuHooks();
        // The drive-letter association must be installed regardless of which
        // IContextMenu implementation was hooked above (item 5). Without this,
        // on the common AutoPlay-probe path the mod would fall back to guessing
        // the drive.
        InstallDriveFolderGetUIObjectHook();
        // Capture the path of each real Explorer drive-menu object. The vtable
        // hook above handles the command; this hook supplies its drive letter.
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (shell32) {
            void* create2 = (void*)GetProcAddress(shell32, "CDefFolderMenu_Create2");
            if (create2)
                Wh_SetFunctionHook(create2, (void*)CDefFolderMenuCreate2Hook,
                                   (void**)&g_CDefCreateOriginal);
        }

        // Pre-associate the already-mounted drives (no UI shown) so a first
        // right-click hits a mapped menu even if Explorer cached drive menus
        // before the mod loaded. Done lazily on the mod's own COM-initialised
        // UI thread (item 3), not on Explorer's startup path.
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES };
        InitCommonControlsEx(&icc);

        g_evtListenerReady = CreateEventW(NULL, FALSE, FALSE, NULL);
        g_hUiThread = CreateThread(NULL, 0, AutoPlayUiThreadProc, nullptr, 0, &g_dwUiThreadId);
        if (!g_hUiThread) {
            Wh_Log(L"Wh_ModInit: CreateThread failed %lu", GetLastError());
            ReleaseAutoPlayOwner();
            if (g_evtListenerReady) { CloseHandle(g_evtListenerReady); g_evtListenerReady = nullptr; }
            return FALSE;
        }
        return TRUE;
    } catch (...) {
        Wh_Log(L"Wh_ModInit: exception");
        ReleaseAutoPlayOwner();
        return FALSE;
    }
}

void Wh_ModUninit() {
    try {
    Wh_Log(L"Wh_ModUninit: pid=%lu", GetCurrentProcessId());

    // Item 2 fix: Windhawk FreeLibrarys the mod as soon as this function returns,
    // so no mod code may still be running afterwards. A finite timeout here means
    // the image can be unmapped while AutoPlayUiThreadProc is still executing.
    // Now that ExecuteOptionByIndex no longer runs blocking shell calls on this
    // thread (see item 2 fix there), the UI thread's message loop is always free
    // to service WMU_SHUTDOWN promptly, so it is safe (and required) to wait
    // without a timeout instead of racing an unload against it.
    if (g_evtListenerReady) WaitForSingleObject(g_evtListenerReady, INFINITE);
    if (g_hUiThread) {
        if (g_hwndListener) PostMessageW(g_hwndListener, WMU_SHUTDOWN, 0, 0);
        WaitForSingleObject(g_hUiThread, INFINITE);
        CloseHandle(g_hUiThread);
        g_hUiThread = nullptr;
        g_dwUiThreadId = 0;
    }
    if (g_hActionWorker) {
        WaitForSingleObject(g_hActionWorker, INFINITE);
        CloseHandle(g_hActionWorker);
        g_hActionWorker = nullptr;
    }
    if (g_evtListenerReady) { CloseHandle(g_evtListenerReady); g_evtListenerReady = nullptr; }

    if (g_dialogClassRegistered)
        UnregisterClassW(kDialogClass, HINST_THISCOMPONENT);
    if (g_listenerClassRegistered)
        UnregisterClassW(kListenerClass, HINST_THISCOMPONENT);

    g_pending.clear();
    g_rot = nullptr;
    g_rotCookie = 0;
    g_classCookie = 0;
    // Only the owning explorer.exe instance owns the CancelAutoplay values; a
    // secondary instance must not delete what the shell instance wrote (item 8).
    // The UI thread (already joined above) also removes them via UnregisterCancelAutoPlay,
    // so this is a belt-and-suspenders guard rather than the primary cleanup.
    if (g_ownsAutoPlay)
        WriteCancelAutoPlayClsid(false);
    // Release any drive menus kept alive by the startup warm-up (done on the
    // UI thread, but keep this as a harmless double-check).
    for (IContextMenu* m : g_warmupMenus)
        if (m) m->Release();
    g_warmupMenus.clear();
    // Release the owned references held for the drive-letter association.
    ReleaseAllContextMenuEntries();
    if (g_csContextMenusInit) { DeleteCriticalSection(&g_csContextMenus); g_csContextMenusInit = false; }
    FreeDpiResources();
    FreeOptions();
    ReleaseAutoPlayOwner();
    } catch (...) {
        Wh_Log(L"Wh_ModUninit: exception");
        ReleaseAutoPlayOwner();
    }
}

void Wh_ModSettingsChanged() {
    try {
        LoadSettings();
        if (g_hwndListener) {
            PostMessageW(g_hwndListener, WMU_APPLY_SUPPRESS, 0, 0);
            PostMessageW(g_hwndListener, WMU_REBUILD, 0, 0);
        }
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged: exception");
    }
}
