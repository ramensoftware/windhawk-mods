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
A privacy setting can hide the volume or device name in this mod only.


## Screenshot 

![autoplay.png](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/autoplay.png)

## Notes  

The mod has been tested on Windows 10 21H2 and Windows 11 24H2.
This mod is a best-effort reimplementation using native Windows components.
Native AutoPlay is suppressed by intercepting the shell's enumeration of
`IQueryCancelAutoPlay` CLSIDs under
`HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\CancelAutoplay\CLSID`
and synthesizing this mod's handler. Nothing is written to that key. Leftover
values from earlier versions of the mod are deleted when the owner instance
starts.
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
    const wchar_t *importMusic;
    const wchar_t *burnDisc;
    const wchar_t *usingDiscBurner;
    const wchar_t *importPictures;
    const wchar_t *usingPhotoGallery;
    const wchar_t *startSlideshow;
    const wchar_t *syncDevice;
    const wchar_t *usingMobileCenter;
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
      L"AutoPlay",
      L"Import tracks into Windows Media Player",
      L"Burn disc",
      L"using Windows Disc Image Burner",
      L"Import pictures and videos",
      L"using Windows Photo Gallery",
      L"Start slideshow",
      L"Sync device",
      L"using Windows Mobile Center" },
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
      L"Riproduzione automatica",
      L"Importa brani nel Windows Media Player",
      L"Masterizza disco",
      L"utilizzando Windows Disc Image Burner",
      L"Importa immagini e video",
      L"utilizzando Windows Photo Gallery",
      L"Avvia presentazione",
      L"Sincronizza dispositivo",
      L"utilizzando Windows Mobile Center" },
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
      L"AutoPlay",
      L"Titel in den Windows Media Player importieren",
      L"Disc brennen",
      L"mit Windows Disc Image Burner",
      L"Bilder und Videos importieren",
      L"mit der Windows-Fotogalerie",
      L"Diashow starten",
      L"Gerät synchronisieren",
      L"mit Windows Mobile Center" },
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
      L"Reproducción automática",
      L"Importar pistas a Windows Media Player",
      L"Grabar disco",
      L"con Windows Disc Image Burner",
      L"Importar imágenes y vídeos",
      L"con la Galería de fotos de Windows",
      L"Iniciar presentación",
      L"Sincronizar dispositivo",
      L"con Windows Mobile Center" },
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
      L"Lecture automatique",
      L"Importer des titres dans Windows Media Player",
      L"Graver le disque",
      L"avec Windows Disc Image Burner",
      L"Importer des images et des vidéos",
      L"avec la Galerie de photos Windows",
      L"Démarrer le diaporama",
      L"Synchroniser l'appareil",
      L"avec Windows Mobile Center" },
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
      L"Reprodução automática",
      L"Importar faixas para o Windows Media Player",
      L"Gravar disco",
      L"com o Windows Disc Image Burner",
      L"Importar imagens e vídeos",
      L"com a Galeria de Fotos do Windows",
      L"Iniciar apresentação",
      L"Sincronizar dispositivo",
      L"com o Windows Mobile Center" },
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
      L"Otomatik Oynat",
      L"Parçaları Windows Media Player'a aktar",
      L"Disk yaz",
      L"Windows Disc Image Burner ile",
      L"Resimleri ve videoları içeri aktar",
      L"Windows Fotoğraf Galerisi ile",
      L"Slayt gösterisini başlat",
      L"Aygıtı eşitle",
      L"Windows Mobile Center ile" },
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
      L"Автозапуск",
      L"Импортировать композиции в Windows Media Player",
      L"Записать диск",
      L"с помощью Windows Disc Image Burner",
      L"Импортировать изображения и видео",
      L"с помощью Windows Фотоальбом",
      L"Запустить слайд-шоу",
      L"Синхронизировать устройство",
      L"с помощью Windows Mobile Center" },
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
      L"自动播放",
      L"将曲目导入 Windows Media Player",
      L"刻录光盘",
      L"使用 Windows 磁盘映像刻录程序",
      L"导入图片和视频",
      L"使用 Windows 照片库",
      L"开始幻灯片放映",
      L"同步设备",
      L"使用 Windows Mobile Center" },
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
      L"自動播放",
      L"將曲目匯入 Windows Media Player",
      L"燒錄光碟",
      L"使用 Windows 磁碟映像燒錄程式",
      L"匯入圖片和影片",
      L"使用 Windows 相片庫",
      L"開始投影片放映",
      L"同步裝置",
      L"使用 Windows Mobile Center" },
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
      L"自動再生",
      L"トラックを Windows Media Player にインポートする",
      L"ディスクを書き込む",
      L"Windows Disc Image Burner を使用",
      L"画像とビデオをインポートする",
      L"Windows フォト ギャラリーを使用",
      L"スライド ショーを開始する",
      L"デバイスを同期する",
      L"Windows Mobile Center を使用" },
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
      L"자동 실행",
      L"Windows Media Player로 트랙 가져오기",
      L"디스크 굽기",
      L"Windows Disc Image Burner 사용",
      L"이미지 및 비디오 가져오기",
      L"Windows 사진 갤러리 사용",
      L"슬라이드 쇼 시작",
      L"디바이스 동기화",
      L"Windows Mobile Center 사용" },
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
      L"التشغيل التلقائي",
      L"استيراد المقاطع إلى Windows Media Player",
      L"نسخ القرص",
      L"باستخدام Windows Disc Image Burner",
      L"استيراد الصور ومقاطع الفيديو",
      L"باستخدام معرض صور Windows",
      L"بدء عرض الشرائح",
      L"مزامنة الجهاز",
      L"باستخدام Windows Mobile Center" },
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
      L"Automatisch afspelen",
      L"Nummers importeren in Windows Media Player",
      L"Schijf branden",
      L"met Windows Disc Image Burner",
      L"Afbeeldingen en video's importeren",
      L"met Windows Fotogalerie",
      L"Diavoorstelling starten",
      L"Apparaat synchroniseren",
      L"met Windows Mobile Center" },
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
      L"Autoodtwarzanie",
      L"Importuj utwory do Windows Media Player",
      L"Nagraj płytę",
      L"za pomocą Windows Disc Image Burner",
      L"Importuj obrazy i wideo",
      L"za pomocą Galerii fotografii systemu Windows",
      L"Uruchom pokaz slajdów",
      L"Synchronizuj urządzenie",
      L"za pomocą Windows Mobile Center" },
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
      L"Automatisk uppspelning",
      L"Importera spår till Windows Media Player",
      L"Bränn skiva",
      L"med Windows Disc Image Burner",
      L"Importera bilder och video",
      L"med Windows Fotogalleri",
      L"Starta bildspel",
      L"Synkronisera enhet",
      L"med Windows Mobile Center" },
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
      L"AutoPlay",
      L"Importér numre til Windows Media Player",
      L"Brænd disk",
      L"med Windows Disc Image Burner",
      L"Importér billeder og video",
      L"med Windows Fotogalleri",
      L"Start diasshow",
      L"Synkroniser enhed",
      L"med Windows Mobile Center" },
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
      L"AutoPlay",
      L"Importer spor til Windows Media Player",
      L"Brenn plate",
      L"med Windows Disc Image Burner",
      L"Importer bilder og video",
      L"med Windows Fotogalleri",
      L"Start lysbildefremvisning",
      L"Synkroniser enhet",
      L"med Windows Mobile Center" },
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
      L"Automaattinen toisto",
      L"Tuo kappaleet Windows Media Playeriin",
      L"Polta levy",
      L"käyttäen Windows Disc Image Burneria",
      L"Tuo kuvia ja videoita",
      L"käyttäen Windowsin valokuvagalleriaa",
      L"Käynnistä diaesitys",
      L"Synkronoi laite",
      L"käyttäen Windows Mobile Centeriä" },
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
      L"Αυτόματη αναπαραγωγή",
      L"Εισαγωγή κομματιών στο Windows Media Player",
      L"Εγγραφή δίσκου",
      L"με το Windows Disc Image Burner",
      L"Εισαγωγή εικόνων και βίντεο",
      L"με τη Συλλογή φωτογραφιών των Windows",
      L"Έναρξη παρουσίασης",
      L"Συγχρονισμός συσκευής",
      L"με το Windows Mobile Center" },
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
static const WCHAR* PHOTO_VIEWER_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAHVklEQVR42u2XS6xeVRXHf2vvs8/5"
    L"zve67xZuWwqFWmyhGIhBhIFGEnEimkgwzohMHMnIoffeiYnRgQ6cGB2hjakxIUYlBh3UkBBEREoa"
    L"kNJ7aQv2cXsf3/M8914ObltaroAdmJjof7T3zspa/7P+a691NvyvQ64sVFUWF9/bvx9LSxJuxPHC"
    L"gpoP9LWIIqLXEfgoqKq5AfsglwN8FKIriyeffnN31Gm2ysHgOoNO0mG5l3kReetGMvDFnywfmO9C"
    L"WQAUV8/jzizVuL/506/dfgFUooUFNUtLEoyYp5PafcYQe0WsCGjw2h/lslsvrj137PlvTnWnxyGU"
    L"H5YFccaE1aycefZ09sNRnjZTgwaJRRRUQ50q0drm+lHg8aOKuZoBoxhBQBAngRACWVHK7taIpx7d"
    L"PzM9NfezG8nA4QOb/OCPF7k4tBJbQURQxHivVEXhtkmggiKqdR3C+QHi1VKWESq7+f6xjhR+6K8Y"
    L"iihBBUQRVcQIrqFMd5UDN0G/QIZFZO69e1b/fnqTsgysjwP1WIPkuVRVuVUfv7yGAArBB8nzXHr1"
    L"PhHXxDRgOYOTbwNgAcRAEkFQqDxEBj55B9x9h4KDZgtmHLxxKdCjJZO3TpJVgh2NWfnbSam9SlFk"
    L"V+K/R8DXNRUFVVkw21aiGKp6i6gIWCNYA2KUOBIMUAal4eDOPeCNkufQmYC2EyyG5Uuefi7cs8ew"
    L"msFgMCSOPWWebb8FVVn68bhEfMFs1xGnyt4dYIAQlPMbMMogcRBb8ALqhbSpjDzUQ2GqCamFzbHS"
    L"sso9Owyvr0I3UorU0YyV1fVN9XUVthHoD8cTh+ZjHvzYLZysHfOzykRTCUGYacLaSDmzKtQlJE7p"
    L"jaCsBJxAUA7uVFInFDWowm0TwloGTQdn1gWI+MYjBzjypxV56bV32lcJLC2hAKP107948rGH9k/c"
    L"PN9prtUaWyNnBoaAYETZ2RZ2TSjjHDZymJmAjYGSB8EmwrkR7GorsVE2c2gnMJnAA7uhVwYujGrd"
    L"O9cxTzwwtXHs+fPPiggHT6ByubspQJWP3rpE4/a/rocQNyKT51t6NyJomkBsDKNauZQJkSgNKxiU"
    L"GkUEnBUQmHJQBphrQFYr1ghZUYcdxptLy6+/cegT995ljPEhBLkqwc7Dh1vGOHPu3Xf41W9/w56b"
    L"b+LS+XeZnmvz8COPk5km67lS1IpzYBCMVXakEIlQKPRLKLywXoEzytkxeBUaVph1QipQBKKpqX3t"
    L"Xu/t3nU1cOH4cW+MUJUlZ986RbaxhpWKweAcx/5whPsf+hLtziyzTqgqCD5gjGAsFAh5gDSBpiqq"
    L"YES4nBCcAatCXSuiQTc2loMx5voiBAQVQqgIIafKx+RhSNIynD67wsVnjjDVneaO2w9w+M6DzLRb"
    L"VGGr4IpaiUSJDBiFNDaUVSDollSJMQQPQfWy2NMCm9sIeBDybMjF9TeoZZLh2ho2MXRnJ5ia6bG5"
    L"Mcfyyhle/suf+fIjX+B8v8nqoKDhpknimHFW4ivPbTc3uGtvg04MHqEOUGsAFI8C6/XWBb+GwL77"
    L"Hk4xAV8qG2cHpK4DpksxKlgdjxn1z9Ka3KQVt+mvwveOvIpLE4LPmejswtkW3fZeDDO8emqVF090"
    L"uHVuP/fcNseh+Sk0ceRVhZVgZu/7/MT6K8+NAaKFhQVZWlrS+7/+3R+/uRbmI61DlXsZnOsTDCCK"
    L"MYYqLyiLBv10gLWKjRxRI2CNYdBfoRj2sfYFmhMdEM/KOeHFZJLfHW+xZ7bLXHef+cqdD4R3md/7"
    L"6a9+6zu/fvn3Txw9qiaCRWCJZquz++cvj5NWr/SdnZ8ycbOJIAgQRRZjDVGIsLVDvCEiJsogSWOs"
    L"c0RTgrOGuJGQpI52K6WZOpppRDMxNOOI55cHeqI/H3Ub7SkAHrtGAieUw7Hn1FqHdHr/1hdGghgh"
    L"MkLkHC5OcLEjSRztxAGCdQYXOyJrSOIYZwwTqWMijYmt0G2lzE+1yGp45vgIl1aEuqj/xTg2goFG"
    L"nIKLqaMItQbnLOIi4kaMESF2ESJQ+hoXWXwdCL6mEiEKNZGzVKamsJ7SGMZ5Tl1l9CqhLi1qxpR5"
    L"IdvHsSgExYeaRrtJMBGGgAHGRU3lVZ21DLOS2BoasUWUrV4KOGsofKDdiIldxIV+Lt3EkQflfC8j"
    L"SVLy3GIDVGW5fRxrCKEqCg/i993ycaqqZJiN+OyhSR7cPyeDPBhjBFQRthqNCAQUDUoUGawYVJXI"
    L"CpY6vHQ21+V/gNFAOagpi9xT1eRFtn0a1pVvW2dtXasVE4FUWByvrGR87u4Wh3dpmRUeQSWoR4MQ"
    L"FAQFUXy40mREjQTz5rATnewlWFtSFDmlrynqyqqNqMqyeQ2BRQCy4foLg966z4Yb3leVrauC2nvd"
    L"WA/y1I9ODVsrzy5mo/HAWm8ogfh9P4ElaGQladiqMbd/R+Pwo99Wb5JQZ1qWpfi6pCwrj2vYsuy/"
    L"CnDwBPrf9TL6IAMFFhcX5UYcLy4u6ocG/TcfLv/Hfxz/BCfcnrQ3pCIIAAAAAElFTkSuQmCC";
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
static bool    g_ownsAutoPlay = false;   // only the owning instance synthesizes CancelAutoplay CLSIDs
static volatile LONG g_shuttingDown = 0;
static HANDLE  g_hActionWorker = nullptr;
static bool    g_hooksInstalled = false;

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
    // When the menu's own drive is known, "not eligible" means do nothing —
    // never fall through to an unrelated volume.
    if (preferredLetter) {
        if (ContextTargetIsEligible(preferredLetter, true))
            return preferredLetter;
        return 0;
    }

    int found = 0;
    int autorun = 0;
    for (int i = 0; i < 26; ++i) {
        int letter = L'A' + i;
        if (!ContextTargetIsEligible(letter, false))
            continue;
        std::wstring root = GetDriveRootForLetter(letter);
        if (GetFileAttributesW((root + L"autorun.inf").c_str()) != INVALID_FILE_ATTRIBUTES)
            autorun = letter;
        if (found) {
            if (autorun) return autorun;
            return 0;
        }
        found = letter;
    }
    return autorun ? autorun : found;
}

// Per-thread association only: GetUIObjectOf / QueryContextMenu / InvokeCommand
// for a given menu all run on the same Explorer window thread. Nothing is
// AddRef'd, so there is no cross-apartment Release, no lock, no eviction and
// no warm-up that would instantiate every Drive shell extension.
static const UINT kNoAutoPlayCommand = UINT_MAX;

struct DriveMenuTls {
    IContextMenu* menu = nullptr; // not owned
    int letter = 0;
    UINT autoPlayOffset = kNoAutoPlayCommand;
};
static thread_local DriveMenuTls t_lastDriveMenu;

struct ContextMenuSnapshot { int letter = 0; UINT autoPlayOffset = kNoAutoPlayCommand; };

static void RememberDriveMenu(IContextMenu* menu, int letter) {
    if (!menu || !letter) return;
    t_lastDriveMenu.menu = menu;
    t_lastDriveMenu.letter = letter;
    t_lastDriveMenu.autoPlayOffset = kNoAutoPlayCommand;
}

static bool SnapshotContextMenu(IContextMenu* menu, ContextMenuSnapshot* snap) {
    if (!menu || t_lastDriveMenu.menu != menu) return false;
    if (snap) {
        snap->letter = t_lastDriveMenu.letter;
        snap->autoPlayOffset = t_lastDriveMenu.autoPlayOffset;
    }
    return true;
}

static void RememberAutoPlayOffset(IContextMenu* menu, UINT offset) {
    if (menu && t_lastDriveMenu.menu == menu)
        t_lastDriveMenu.autoPlayOffset = offset;
}

static HRESULT STDMETHODCALLTYPE ContextQueryHook(IContextMenu*, HMENU, UINT, UINT, UINT, UINT);
static HRESULT STDMETHODCALLTYPE ContextInvokeHook(IContextMenu*, LPCMINVOKECOMMANDINFO);

// Originals are keyed by vtable so both the AutoPlay CPL class and
// CDefFolderMenu can be hooked without clobbering each other. Addresses are
// stable (fixed array) because Wh_SetFunctionHook writes the trampoline
// pointer later, at apply time.
struct ContextMenuOrigs {
    void** vtable = nullptr;
    HRESULT (STDMETHODCALLTYPE* query)(IContextMenu*, HMENU, UINT, UINT, UINT, UINT) = nullptr;
    HRESULT (STDMETHODCALLTYPE* invoke)(IContextMenu*, LPCMINVOKECOMMANDINFO) = nullptr;
    HMODULE pinned = nullptr;
};

static const int kMaxContextMenuVtables = 8;
static ContextMenuOrigs g_contextMenuOrigs[kMaxContextMenuVtables];
static int g_contextMenuOrigsCount = 0;
static HMODULE g_pinnedGetUIObjectOfModule = nullptr;
static HRESULT (STDMETHODCALLTYPE* g_GetUIObjectOf_Original)(IShellFolder*, HWND, UINT, PCUITEMID_CHILD_ARRAY, REFIID, UINT*, void**) = nullptr;

static HMODULE PinForeignModule(void* addr) {
    if (!addr) return nullptr;
    HMODULE mod = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            reinterpret_cast<LPCWSTR>(addr), &mod) || !mod)
        return nullptr;
    if (mod == HINST_THISCOMPONENT) {
        FreeLibrary(mod);
        return nullptr;
    }
    return mod;
}

static ContextMenuOrigs* OrigsForMenu(IContextMenu* menu) {
    if (!menu) return nullptr;
    void** vt = *reinterpret_cast<void***>(menu);
    for (int i = 0; i < g_contextMenuOrigsCount; ++i) {
        if (g_contextMenuOrigs[i].vtable == vt)
            return &g_contextMenuOrigs[i];
    }
    return nullptr;
}

static bool HookContextMenuVtable(IContextMenu* menu) {
    if (!menu) return false;
    void** vt = *reinterpret_cast<void***>(menu);
    if (!vt) return false;
    if (OrigsForMenu(menu)) return true;
    if (g_contextMenuOrigsCount >= kMaxContextMenuVtables) return false;

    auto query = reinterpret_cast<decltype(ContextMenuOrigs::query)>(vt[3]);
    auto invoke = reinterpret_cast<decltype(ContextMenuOrigs::invoke)>(vt[4]);
    if (!query || !invoke) return false;

    ContextMenuOrigs& slot = g_contextMenuOrigs[g_contextMenuOrigsCount];
    slot.vtable = vt;
    bool q = Wh_SetFunctionHook((void*)query, (void*)ContextQueryHook, (void**)&slot.query);
    bool i = Wh_SetFunctionHook((void*)invoke, (void*)ContextInvokeHook, (void**)&slot.invoke);
    if (!q || !i) {
        slot = ContextMenuOrigs{};
        Wh_Log(L"HookContextMenuVtable: failed vt=%p query=%d invoke=%d", vt, q, i);
        return false;
    }
    slot.pinned = PinForeignModule((void*)query);
    ++g_contextMenuOrigsCount;
    Wh_Log(L"HookContextMenuVtable: vt=%p query=%p invoke=%p pinned=%p",
           vt, query, invoke, slot.pinned);
    return true;
}

// Derive a "X:\" path from a shell folder child item. This is the reliable
// boundary at which a drive's identity is still known.
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

static HRESULT STDMETHODCALLTYPE GetUIObjectOfHook(
    IShellFolder* parent, HWND hwnd, UINT cidl, PCUITEMID_CHILD_ARRAY items,
    REFIID riid, UINT* reserved, void** ppv) {
    HRESULT hr = S_OK;
    try {
        if (!g_GetUIObjectOf_Original) return E_UNEXPECTED;
        hr = g_GetUIObjectOf_Original(parent, hwnd, cidl, items, riid, reserved, ppv);
        if (SUCCEEDED(hr) && ppv && *ppv && riid == IID_IContextMenu &&
            cidl == 1 && items && items[0]) {
            std::wstring path = GetPathFromChild(parent, items[0]);
            if (path.size() >= 2 && path[1] == L':') {
                int letter = (int)towupper((wint_t)path[0]);
                RememberDriveMenu((IContextMenu*)*ppv, letter);
                Wh_Log(L"GetUIObjectOfHook: menu=%p -> %s -> %c:",
                       *ppv, path.c_str(), letter);
            }
        }
    } catch (...) {
        Wh_Log(L"GetUIObjectOfHook: exception");
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE ContextQueryHook(IContextMenu* menu, HMENU hmenu,
                                                    UINT index, UINT first, UINT last,
                                                    UINT flags) {
    ContextMenuOrigs* origs = OrigsForMenu(menu);
    if (!origs || !origs->query) return E_UNEXPECTED;
    HRESULT hr = origs->query(menu, hmenu, index, first, last, flags);
    ContextMenuSnapshot snap;
    bool mapped = SnapshotContextMenu(menu, &snap);
    Wh_Log(L"Context menu QueryContextMenu self=%p first=%u last=%u mapped=%d",
           menu, first, last, mapped ? 1 : 0);
    if (FAILED(hr) || !menu || !hmenu || first > last) return hr;

    // Canonical "autoplay" verb is language-independent; walk real items only.
    int n = GetMenuItemCount(hmenu);
    for (int i = 0; i < n; ++i) {
        UINT id = GetMenuItemID(hmenu, i);
        if (id == (UINT)-1 || id < first || id > last) continue;
        UINT offset = id - first;
        WCHAR wide[64] = {};
        if (SUCCEEDED(menu->GetCommandString(offset, GCS_VERBW, nullptr,
                                             (LPSTR)wide, ARRAYSIZE(wide))) &&
            !_wcsicmp(wide, L"autoplay")) {
            RememberAutoPlayOffset(menu, offset);
            break;
        }
        char narrow[64] = {};
        if (SUCCEEDED(menu->GetCommandString(offset, GCS_VERBA, nullptr,
                                             narrow, ARRAYSIZE(narrow))) &&
            !_stricmp(narrow, "autoplay")) {
            RememberAutoPlayOffset(menu, offset);
            break;
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
    ContextMenuOrigs* origs = OrigsForMenu(menu);
    if (!origs || !origs->invoke) return E_UNEXPECTED;
    ContextMenuSnapshot snap;
    bool mapped = SnapshotContextMenu(menu, &snap);
    Wh_Log(L"Context menu InvokeCommand self=%p mapped=%d", menu, mapped ? 1 : 0);
    if (!ci || !g_hwndListener) return origs->invoke(menu, ci);

    bool known = mapped && snap.autoPlayOffset != kNoAutoPlayCommand;
    bool isAutoPlay =
        (known && IS_INTRESOURCE(ci->lpVerb) &&
         LOWORD((ULONG_PTR)ci->lpVerb) == snap.autoPlayOffset) ||
        ContextInvocationIsAutoPlay(menu, ci);
    if (!isAutoPlay)
        return origs->invoke(menu, ci);

    int letter = mapped ? FindContextAutoPlayDrive(snap.letter)
                        : FindContextAutoPlayDrive();
    if (letter) {
        PostMessageW(g_hwndListener, WMU_CONTEXT_AUTOPLAY,
                     (WPARAM)letter, 0);
        Wh_Log(L"Context AutoPlay: intercepted IContextMenu::InvokeCommand for %c:", letter);
        return S_OK;
    }
    return origs->invoke(menu, ci);
}

static bool InstallAutoplayContextMenuHooks() {
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
    bool ok = HookContextMenuVtable(menu);
    menu->Release();
    return ok;
}

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
        void** pvt = *reinterpret_cast<void***>(parent);
        auto guiObj = pvt ? (decltype(g_GetUIObjectOf_Original))pvt[10] : nullptr;
        ok = guiObj && Wh_SetFunctionHook((void*)guiObj,
                                          (void*)GetUIObjectOfHook,
                                          (void**)&g_GetUIObjectOf_Original);
        if (ok && !g_pinnedGetUIObjectOfModule)
            g_pinnedGetUIObjectOfModule = PinForeignModule((void*)guiObj);
    }
    Wh_Log(L"Drive folder GetUIObjectOf hook installed=%d", ok ? 1 : 0);
    if (parent) parent->Release();
    if (pidl) CoTaskMemFree(pidl);
    if (ok) g_getUIObjectOfHookInstalled = true;
    return ok;
}

static bool InstallDriveContextMenuHooks() {
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
    IContextMenu* menu = nullptr;
    HRESULT hr = SHParseDisplayName(root, nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr))
        hr = SHBindToParent(pidl, IID_IShellFolder,
                            (void**)&parent, &child);
    if (SUCCEEDED(hr) && parent)
        hr = parent->GetUIObjectOf(nullptr, 1, &child, IID_IContextMenu,
                                   nullptr, (void**)&menu);
    bool ok = false;
    if (SUCCEEDED(hr) && menu) {
        ok = HookContextMenuVtable(menu);
        Wh_Log(L"Drive AutoPlay context probe root=%s ok=%d", root, ok ? 1 : 0);
    } else {
        Wh_Log(L"Drive AutoPlay context probe failed hr=0x%08X root=%s",
               (unsigned)hr, root);
    }
    if (menu) menu->Release();
    if (parent) parent->Release();
    if (pidl) CoTaskMemFree(pidl);
    return ok;
}

static HRESULT (WINAPI* g_CDefCreateOriginal)(LPCITEMIDLIST, HWND, UINT, PCUITEMID_CHILD_ARRAY, IShellFolder*, LPFNDFMCALLBACK, UINT, HKEY*, IContextMenu**) = nullptr;

static HRESULT WINAPI CDefFolderMenuCreate2Hook(
    LPCITEMIDLIST folder, HWND hwnd, UINT cidl, PCUITEMID_CHILD_ARRAY items,
    IShellFolder* sf, LPFNDFMCALLBACK callback, UINT keys, HKEY* hkeys,
    IContextMenu** outMenu) {
    HRESULT hr = g_CDefCreateOriginal(folder, hwnd, cidl, items, sf,
                                       callback, keys, hkeys, outMenu);
    Wh_Log(L"CDefFolderMenuCreate2Hook fired hr=0x%08X cidl=%u menu=%p",
           (unsigned)hr, cidl, outMenu && *outMenu ? *outMenu : nullptr);
    if (SUCCEEDED(hr) && outMenu && *outMenu) {
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
        if (foundLetter)
            RememberDriveMenu(*outMenu, foundLetter);
        else
            Wh_Log(L"CDefFolderMenuCreate2Hook: could not derive a drive letter for menu=%p",
                   *outMenu);
    }
    return hr;
}

// Synthesize our IQueryCancelAutoPlay CLSID when the shell enumerates
// CancelAutoplay\CLSID. Nothing is persisted; leftover values from earlier
// versions of the mod are deleted on startup.
static decltype(&RegQueryInfoKeyW) g_RegQueryInfoKeyW_Original = nullptr;
static decltype(&RegEnumValueW) g_RegEnumValueW_Original = nullptr;

typedef struct _KEY_NAME_INFORMATION_WH {
    ULONG NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION_WH;

static LONG (NTAPI* g_NtQueryKey)(HANDLE, int, PVOID, ULONG, PULONG) = nullptr;

static void EnsureNtQueryKey() {
    if (g_NtQueryKey) return;
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll)
        g_NtQueryKey = (decltype(g_NtQueryKey))GetProcAddress(ntdll, "NtQueryKey");
}

static bool IsCancelAutoplayClsidKey(HKEY hKey) {
    if (!hKey || hKey == INVALID_HANDLE_VALUE) return false;
    EnsureNtQueryKey();
    if (!g_NtQueryKey) return false;
    BYTE buf[1024];
    ULONG len = 0;
    LONG st = g_NtQueryKey(hKey, 3 /* KeyNameInformation */, buf, sizeof(buf), &len);
    if (st < 0) return false;
    auto* info = reinterpret_cast<KEY_NAME_INFORMATION_WH*>(buf);
    size_t maxChars = (sizeof(buf) - sizeof(ULONG)) / sizeof(WCHAR);
    size_t nchars = info->NameLength / sizeof(WCHAR);
    if (nchars > maxChars) nchars = maxChars;
    std::wstring name(info->Name, nchars);
    const wchar_t kSuffix[] = L"\\AutoplayHandlers\\CancelAutoplay\\CLSID";
    size_t suffixLen = wcslen(kSuffix);
    if (name.size() < suffixLen) return false;
    return _wcsicmp(name.c_str() + (name.size() - suffixLen), kSuffix) == 0;
}

static bool ShouldSpoofCancelClsid(HKEY hKey) {
    if (g_shuttingDown || !g_ownsAutoPlay || !g_settings.suppressNativeAutoPlay)
        return false;
    return IsCancelAutoplayClsidKey(hKey);
}

static LSTATUS SynthesizeCancelClsidValue(
    PCWSTR name, LPWSTR lpValueName, LPDWORD lpcchValueName,
    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (!lpcchValueName) return ERROR_INVALID_PARAMETER;
    DWORD nameLen = (DWORD)wcslen(name);
    if (!lpValueName || *lpcchValueName < nameLen + 1) {
        *lpcchValueName = nameLen + 1;
        return ERROR_MORE_DATA;
    }
    memcpy(lpValueName, name, (nameLen + 1) * sizeof(WCHAR));
    *lpcchValueName = nameLen;
    if (lpType) *lpType = REG_SZ;
    const BYTE empty[] = { 0, 0 };
    if (lpcbData) {
        if (lpData && *lpcbData < sizeof(empty)) {
            *lpcbData = sizeof(empty);
            return ERROR_MORE_DATA;
        }
        if (lpData) memcpy(lpData, empty, sizeof(empty));
        *lpcbData = sizeof(empty);
    }
    return ERROR_SUCCESS;
}

static LSTATUS WINAPI RegQueryInfoKeyWHook(
    HKEY hKey, LPWSTR lpClass, LPDWORD lpcchClass, LPDWORD lpReserved,
    LPDWORD lpcSubKeys, LPDWORD lpcbMaxSubKeyLen, LPDWORD lpcbMaxClassLen,
    LPDWORD lpcValues, LPDWORD lpcbMaxValueNameLen, LPDWORD lpcbMaxValueLen,
    LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime) {
    if (!g_RegQueryInfoKeyW_Original) return ERROR_INVALID_FUNCTION;
    LSTATUS st = g_RegQueryInfoKeyW_Original(
        hKey, lpClass, lpcchClass, lpReserved, lpcSubKeys, lpcbMaxSubKeyLen,
        lpcbMaxClassLen, lpcValues, lpcbMaxValueNameLen, lpcbMaxValueLen,
        lpcbSecurityDescriptor, lpftLastWriteTime);
    if (st == ERROR_SUCCESS && ShouldSpoofCancelClsid(hKey)) {
        if (lpcValues) *lpcValues += 2;
        DWORD needName = (DWORD)wcslen(kCancelClsidBraces);
        if (lpcbMaxValueNameLen && *lpcbMaxValueNameLen < needName)
            *lpcbMaxValueNameLen = needName;
        if (lpcbMaxValueLen && *lpcbMaxValueLen < sizeof(WCHAR))
            *lpcbMaxValueLen = sizeof(WCHAR);
    }
    return st;
}

static LSTATUS WINAPI RegEnumValueWHook(
    HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, LPDWORD lpcchValueName,
    LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (ShouldSpoofCancelClsid(hKey) && g_RegQueryInfoKeyW_Original) {
        DWORD realCount = 0;
        LSTATUS qi = g_RegQueryInfoKeyW_Original(
            hKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
            &realCount, nullptr, nullptr, nullptr, nullptr);
        if (qi == ERROR_SUCCESS) {
            if (dwIndex == realCount)
                return SynthesizeCancelClsidValue(kCancelClsidBraces, lpValueName,
                    lpcchValueName, lpType, lpData, lpcbData);
            if (dwIndex == realCount + 1)
                return SynthesizeCancelClsidValue(kCancelClsidBare, lpValueName,
                    lpcchValueName, lpType, lpData, lpcbData);
        }
    }
    return g_RegEnumValueW_Original(hKey, dwIndex, lpValueName, lpcchValueName,
                                    lpReserved, lpType, lpData, lpcbData);
}

static DWORD g_rotCookie = 0;
static IRunningObjectTable* g_rot = nullptr;
static DWORD g_classCookie = 0;
static bool g_cancelRegWritten = false;

static void WriteCancelAutoPlayClsid(bool add) {
    // add is ignored: the mod no longer persists CancelAutoplay CLSID values.
    // The RegQueryInfoKeyW / RegEnumValueW hooks synthesize our entry instead.
    // The delete path remains so leftovers from earlier versions disappear.
    (void)add;
    ApRegKey key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kCancelClsidPath, 0, KEY_SET_VALUE, &key.h) == ERROR_SUCCESS) {
        RegDeleteValueW(key.h, kCancelClsidBare);
        RegDeleteValueW(key.h, kCancelClsidBraces);
    }
    g_cancelRegWritten = false;
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

    WriteCancelAutoPlayClsid(false); // drop leftovers from earlier versions

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
        if (!PathIsUnderRoot(out.iconFile, root) ||
            GetFileAttributesW(out.iconFile.c_str()) == INVALID_FILE_ATTRIBUTES)
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
        o.line1 = lp->importMusic;
        o.line2 = lp->usingPlayer; o.icon.hBmp = g_bmpPlay; o.icon.shared = true;
        g_options.push_back(std::move(o));
    }
    if (g_contentKind == ContentKind::BlankDisc && HasDiscImageBurner()) {
        AutoPlayOption o;
        o.type = ActionType::BurnDisc; o.group = OptionGroup::Content;
        o.line1 = lp->burnDisc;
        o.line2 = lp->usingDiscBurner;
        std::wstring p = L""; FileExistsExpanded(L"%SystemRoot%\\System32\\isoburn.exe", p);
        o.icon = MakeSafePathIcon(p, g_bmpSetup);
        g_options.push_back(std::move(o));
    }
    if (g_contentKind == ContentKind::Pictures) {
        if (HasWindowsPhotoGallery()) {
            AutoPlayOption o;
            o.type = ActionType::ImportPictures; o.group = OptionGroup::Content;
            o.line1 = lp->importPictures;
            o.line2 = lp->usingPhotoGallery;
            std::wstring p; FileExistsExpanded(L"%ProgramFiles%\\Windows Photo Gallery\\PhotoGallery.exe", p);
            o.icon = MakeSafePathIcon(p, g_bmpFolder);
            g_options.push_back(std::move(o));
        }
        if (!inv.firstPicture.empty()) {
            AutoPlayOption o;
            o.type = ActionType::ViewSlideshow; o.group = OptionGroup::Content;
            o.line1 = lp->startSlideshow;
            o.line2 = lp->usingWindows; o.targetPath = inv.firstPicture;
            o.icon = MakeSafePathIcon(inv.firstPicture, g_bmpFolder);
            g_options.push_back(std::move(o));
        }
    }
    if (g_isWpd && HasWindowsMobileCenter()) {
        AutoPlayOption o;
        o.type = ActionType::SyncDevice; o.group = OptionGroup::Content;
        o.line1 = lp->syncDevice;
        o.line2 = lp->usingMobileCenter;
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
    if (g_shuttingDown) return;
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
    if (g_shuttingDown) {
        g_pending.clear();
        return;
    }
    // Only a blocking action worker shares the globals we need; a dialog being
    // open must NOT freeze the queue. Otherwise inserting a second drive (Z:)
    // while the dialog for an already-mounted drive (D:) is open would leave Z:
    // queued forever and you would keep seeing the D: dialog (or the native
    // AutoPlay if D: is not present). BuildDriveDialog() already destroys and
    // replaces a dialog belonging to a different drive.
    if (g_hActionWorker)
        return;

    for (size_t i = 0; i < g_pending.size(); ) {
        if (g_shuttingDown) {
            g_pending.clear();
            return;
        }
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
    if (g_shuttingDown) return;
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
        if (g_evtListenerReady) SetEvent(g_evtListenerReady);
        try { UnregisterCancelAutoPlay(); } catch (...) {}
        if (g_wic) { g_wic->Release(); g_wic = nullptr; }
        CoUninitialize();
        return 1;
    }
}

// ============================================================================
// ============================================================================
static bool StartUiThread() {
    if (g_hUiThread) return true;
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);
    if (!g_evtListenerReady)
        g_evtListenerReady = CreateEventW(NULL, FALSE, FALSE, NULL);
    g_hUiThread = CreateThread(NULL, 0, AutoPlayUiThreadProc, nullptr, 0, &g_dwUiThreadId);
    if (!g_hUiThread) {
        Wh_Log(L"StartUiThread: CreateThread failed %lu", GetLastError());
        if (g_evtListenerReady) { CloseHandle(g_evtListenerReady); g_evtListenerReady = nullptr; }
        return false;
    }
    return true;
}

static bool InstallOwnerHooks() {
    if (g_hooksInstalled) return true;

    Wh_SetFunctionHook((void*)ShellExecuteExW, (void*)ShellExecuteExWHook,
                       (void**)&g_ShellExecuteExW_Original);
    Wh_SetFunctionHook((void*)ShellExecuteW, (void*)ShellExecuteWHook,
                       (void**)&g_ShellExecuteW_Original);
    Wh_SetFunctionHook((void*)ShellExecuteExA, (void*)ShellExecuteExAHook,
                       (void**)&g_ShellExecuteExA_Original);
    Wh_SetFunctionHook((void*)ShellExecuteA, (void*)ShellExecuteAHook,
                       (void**)&g_ShellExecuteA_Original);
    Wh_SetFunctionHook((void*)RegQueryInfoKeyW, (void*)RegQueryInfoKeyWHook,
                       (void**)&g_RegQueryInfoKeyW_Original);
    Wh_SetFunctionHook((void*)RegEnumValueW, (void*)RegEnumValueWHook,
                       (void**)&g_RegEnumValueW_Original);

    // Hook both IContextMenu implementations. Originals are keyed by vtable so
    // one cannot clobber the other.
    InstallAutoplayContextMenuHooks();
    InstallDriveContextMenuHooks();
    InstallDriveFolderGetUIObjectHook();
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (shell32) {
        void* create2 = (void*)GetProcAddress(shell32, "CDefFolderMenu_Create2");
        if (create2)
            Wh_SetFunctionHook(create2, (void*)CDefFolderMenuCreate2Hook,
                               (void**)&g_CDefCreateOriginal);
    }

    g_hooksInstalled = true;
    return true;
}

BOOL Wh_ModInit() {
    try {
        LoadSettings();
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

        InstallOwnerHooks();

        if (!StartUiThread()) {
            ReleaseAutoPlayOwner();
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
    InterlockedExchange(&g_shuttingDown, 1);

    // Wait for the UI thread to either signal readiness or exit (the catch
    // path used to skip SetEvent, which made an INFINITE wait on the event
    // hang Explorer). Then post WMU_SHUTDOWN and join.
    if (g_evtListenerReady && g_hUiThread) {
        HANDLE waitOn[2] = { g_evtListenerReady, g_hUiThread };
        WaitForMultipleObjects(2, waitOn, FALSE, INFINITE);
    } else if (g_evtListenerReady) {
        WaitForSingleObject(g_evtListenerReady, INFINITE);
    }
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
    if (g_ownsAutoPlay)
        WriteCancelAutoPlayClsid(false);
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
        if (!g_ownsAutoPlay && IsMainExplorerShell() && TryBecomeAutoPlayOwner()) {
            Wh_Log(L"Wh_ModSettingsChanged: acquired AutoPlay ownership, installing hooks");
            InstallOwnerHooks();
            if (!Wh_ApplyHookOperations())
                Wh_Log(L"Wh_ModSettingsChanged: Wh_ApplyHookOperations failed");
            if (!StartUiThread())
                ReleaseAutoPlayOwner();
        }
        if (g_hwndListener) {
            PostMessageW(g_hwndListener, WMU_APPLY_SUPPRESS, 0, 0);
            PostMessageW(g_hwndListener, WMU_REBUILD, 0, 0);
        }
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged: exception");
    }
}
