// ==WindhawkMod==
// @id              win7-classic-autoplay-restorer
// @name            Windows 7 Classic AutoPlay Dialog
// @description     This mod restores the classic Windows 7 AutoPlay dialog for removable drives and optical media
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -lole32 -luuid -loleaut32 -lshell32 -lshlwapi -lgdi32 -luser32 -lcomctl32 -lmsimg32 -lwindowscodecs -lversion -ladvapi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Classic AutoPlay Dialog

This mod restores the classic Windows 7 AutoPlay dialog when you insert a USB
drive, a disc, or a phone.

What you see is a simple list of real actions (open folder, play with Windows
Media Player, view pictures with Windows Photo Viewer, ReadyBoost). A program
from the disc is offered only if you click it, and only if the file is on that
volume.

The "Always do this" choice is remembered by the mod, not by Windows policy.
A privacy setting can hide the volume or device name in this window only.

The mod has been tested on Windows 10 21H2.
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
    explicit operator bool() const { return p != nullptr; }
};

struct ApRegKey {
    HKEY h = nullptr;
    ~ApRegKey() { if (h) { RegCloseKey(h); h = nullptr; } }
    ApRegKey() = default;
    ApRegKey(const ApRegKey&) = delete;
};

struct ApCoTaskStrs {
    std::vector<LPWSTR> ids;
    ~ApCoTaskStrs() {
        for (LPWSTR s : ids)
            if (s) CoTaskMemFree(s);
    }
};

LRESULT CALLBACK AutoPlayDialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ListenerWndProc(HWND, UINT, WPARAM, LPARAM);

#define WMU_SHUTDOWN      (WM_USER + 0x601) // postato da Wh_ModUninit al listener
#define WMU_REBUILD       (WM_USER + 0x602) // ricostruisci dialogo (impostazioni)
#define WMU_SELF_REBUILD  (WM_APP  + 0x033) // dialogo -> posta WMU_REBUILD
#define WMU_PROCESS_QUEUE (WM_USER + 0x603) // processa coda volumi in attesa
#define WMU_APPLY_SUPPRESS (WM_USER + 0x604) // applica/rimuove IQueryCancelAutoPlay
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
    L"iVBORw0KGgoAAAANSUhEUgAAAQAAAAEACAYAAABccqhmAAAQAElEQVR4Aez9B7Qlx3Xfje5d1X3u"
    L"3MkJgzTIGSAIMAeJQSJly0vWs0RK/vz8Wcvr2Qpeb1mfbUnvW7JEW7QpktInShSjKGaKEkEiMYkE"
    L"SOREEExgEMEIYJDDDCaHm073+/+qTx30nDn3zp2ZOzMY8pzp/9m7du29a1d17erq7nvvBPsJ+9R1"
    L"7XQpU/hBzFU3qEv5QPWxaUP2AbRlC8W3/bb5+fqfy4Y6MF9fC6E33/bmq7dAMe13Ti1EO0fDxxFb"
    L"AHTC0iAe7k66e00bmcIPYq66QV3iPhD9bC+7/tjKvgK57kApvsAwu7bfNj+oOx/7QRv8gUH5Qpfb"
    L"sc23vfnqzRar2px1Pg7Wqa39zql2O4P27bpD4Q+H3/4kPZjADiSgPIgH087RtDnYuGVXLVTc+AIH"
    L"6k/np39+D8b+QNs7WH1ia8fa9jObvK2T+QPRVZspqbNtm85V19abjT9U+yPptz9BZmt0Lvnh6uhc"
    L"bR5InSZEf5Vv84M+htUNkw2xO6TxG/SXy2q777fN5/r5Up2fg1qE5mrzQOrm0h3sw2yxZvl8fGVd"
    L"fM+mL/k+cwIZwG4+OBDd+fhD50B8HoguvmcD8v5Eo3CksZAdGRa7JkR/lW/zg7rD6rJMMc46RtLZ"
    L"J8Hm0p+rrh1T22/m52vb9nOwfG5zmP1sdcQ3WDdYHubvYGS0hV2mmacMKM/WtuRpTkiPhcChyAB2"
    L"8wG62GVdeJDLB0Pxme3256utm20Ols46uQ/W4f6Cb/vdX0cOxFfb70LyinGfJB/mX7GmsZxLf666"
    L"YT6R4RdgC82yHmUCp3YpLyRyW3P5bOsQX1u3XYd8sIzsQND2n/lM8Z35+fqUfi2kcyt7FoO+6WC5"
    L"X9FiZJsWEkRtnvKhYn/+5hPffGNY8MkzLPiDDXiYr9yxg/WZ7ReaKtY0mebrV/HP680AfgF+h9D+"
    L"JKb+QEEMs9nQVru+zWcbdDI/SNt12LbLg7qHWs6+oQB/tCkU8HNB+oyhVOv+IoBsLpthdfO1UUP9"
    L"dob5mUuWbefb1ly+ct2CLwDZcZsuZMDZ74H6zIOXafZztKjin9ebAcV70OdomG1bRgxz9b9d3+bb"
    L"Ptr8bL7atrPpHIyctsEwW9oUZlS/30VgmP2BytROSuxM2/ZtmWJSsVlsxCSbtu4g39bBdrD+YMvZ"
    L"7qAnV3ZwtGl7gOaKJQ9epnPpzrdObc85ftRnZJ+D5Sw/HFR9PaBdyXxjyH7pS+bna7uQerQN9uOz"
    L"Is796Jj89Lf0+9MdrJd/me/7+hn5oC5lKR9SW/iYzTd1B4I5J/D+HC1UEPtrZ676Ax3MhYxZbc+Z"
    L"YNRntPpQI2uV52TRVcxDz9Ns8rkc4m+u+gOpm4+vg4nxQGJo69IWaMuIEbRlC8Grnf7VW/6HJnSW"
    L"ZzrY7mzytt6gTi5n2tY9GH7oxJqvowMJoj1g8/V/sHq0BYbZH0jMw+zld84x21+9fKaJI71EVd7v"
    L"oZhnW2j6E0/+eKYwb5/7bXSBFIi9F9te44ZsPk3sT69dT1tgPn4PVUft9Mf+UH217dWfvc4hZaE/"
    L"duL3qsd2mAz5fNB3PB/l+eoQEMj68IdrwHIbUNqB0hbIZWQHC/k4oDFSuylZZ7PL9YpnnxMpWTqw"
    L"FWatT0r6kq/+JBRPu7wV2K+dTPuH2jmg/g3qD5b7jlsMsYEswiaX4bMcOljOetQNw7D6QR/D7GaT"
    L"yXZe4zdfvdnamU2u/qRzmv1TFji3yUR8qk+F3tcwWa9qKGkLD+jktw1n4wmcgEDWafNZNhfFx1z1"
    L"s9UNtjNYns1uLrl89AcfvcEyslmgbtSzji9+pJDqM81+qBP2OdG5/kDooO9sixyonb36l+tnoweq"
    L"P8wPPmh7WN3ByhbSn3ztdxFQH9L5GdQdLB9sf7J/7Pfnc7C+XW7z+BpEmoCDwkMptwM/FD/Hum0e"
    L"B52AucY4TSL1NVOxB3+oTd4sDPraq5zj6ekOTf6sM59I8LM/vWH+sl2m2cdgOcv3R9t2bX5/doP1"
    L"suUZzV5jNqjTLqM/V7ldd7B8uw2N5T6LU7v+QNuYa3IeqK8F0z+UDi1YEAvgSP0gIYcmGe5VnyZa"
    L"psjmgk7+AZ+vIb7lZt+diYR937LZK+Z23VzxzVY36G82vQORH0pMB2Mrm30S70DiHdQ9WH8ayzRn"
    L"8DfMB7K2TpvHZhD9kz5YcSBlGj0Q/QPRzb4zPRDbo6lLvMKCjG+rH/2T35IdENubEMP8DJMl37Lp"
    L"Lwj0CaSKI/g12GY7pnYYg3rtuszPZpvrh1HZzDo+w/T3JzsQf+rT0MVnmI9hsnYsg/yCTNC5Gp0t"
    L"+MFAcnlQP/vONOsdaaq4eMq+13ghmy0O4hX6iTOoN5ftoG4uy9+CTMLshxiENLmyLLd1uKja22sM"
    L"2+3MVaf40ljOpYOvrAc/H+zP3zAfskljNqzucMjUpwU578Nim/VkDFM+GNmBBn+g+vuLab4nS3r9"
    L"sch8prShuPbZziOj7mAwH9t2+wfTxv5siEGYc3INxiD9fcZhrnaG2c+mj+/Z6rJ8PjpZdz70YPzJ"
    L"Zs4xm0+7zxSd/qQ/mIB0co/oSngwMc73ZEkvXWFoI/OZIjsaONrt0+dDjeFQ7YlhhMM3Aoe0AOjk"
    L"HhMr4bGwUB2+Uzzy/NM2Anm+Z5r7P4we0gIwzOFCyeYT/HzbOlILlWJ+xo7nfMdqpHfsj0Ce75nO"
    L"1aNn7ISdT/BzdexI1bWTXjH3byOOVPujdkYjcCgjcMQXACXMYX1ucDj8y+c+bwDyoI+SPo/EiB6L"
    L"I3BEFgAlUD/plTCH9bnB4fAvnwf05PtYnAijmH+yR2C23i3IAtBO8GENKYGGJv2g3WB5mK+FkB1o"
    L"O9KfdQewEPGMfIxG4GiNwIIsALMl+P46NWg3WN6f/cHWt9shuffnR/qjHcD+BmlUf0yOwIIsAM/E"
    L"niux+7cdg/G160juIfXzHhf5mrfuYDuj8mgEjvYIHLXJq8SZNUEZlP3VowNm01NiD73twGauul79"
    L"vJ/my9e8dfE9wmgEjvQI0N5seXLUFgAlzqwJSsD7q0cHzFcP3flAA3XUxmQ+8Y10RiNwMCMwW56M"
    L"JvvAaGqgRlf0gTE5FopauPfZUSID7fgHy+26Q+EPl98c00L5H/TzU70AaDB+qvufJ9exRHXO9kl0"
    L"4tfCvc+OEhmgPmOwnOWHSg+X3xzXoH/GAeT6QTpb3aCfo5YAswVIR+aqo36hoME45Ku9Yt1rDFXm"
    L"7/LtJVuoeEd+9h0Bxntf6bEjIX5woBFr7s7rLxftz+9Rm6h0YLbg5qqbzeZoyRXrXouIylyJwNEK"
    L"6Se63d749vs4WO5XHAHmYBJ3MCziB4PyQy1nnzlGKBj0e9QWgMFAjpWyBnG/Y5YH/1jp0yjOn9wR"
    L"yHMRCgZ7ut/JPGgwn7KSZOh92nxs0RlmP0yG7oHgUH3IPmgQ97ri0z5yaBuSjW4F2gPyE8hrLjzj"
    L"d3rMw7mG/rAsAIc6MMPs27L9dWq2Dg/4OOC+y36f5Ket2eTUjXB0R4C5AtpRUAZt2U8iT580N+dc"
    L"pA44CXA6G47UoO6vU7PF15bLx9BkbuscCi//PKQ5rG0cSnw/Lba98zBnEsw1FkdqTs8Vw+GsW9AF"
    L"gMH+SR+wuU6G+r6g4zlXW6O6gx8B5imYj4f56s3H1zNRZ8En7FwDpgThvviQng8c6iASAz5EU9+h"
    L"ANmhAB/q++iKfyiD+Ayx1bncZ44Ok8033IOxPRibwXiyj0wH6ymnJIBZCMzVEP6VIP2t2P500W9j"
    L"mD4y0NbbH59jEO0na5vfn/1s9QvhYzbfI/mhj8CBzBOdy/48zS0Pk+W6haCD8R1qe8SUfWSKbBAL"
    L"ugDM1VBuOOtkmuX7o8P0kYH92c5VL/v+QtDW0wlZ0LFp+x7xR34EdJ51Sut9ruwHG4mczdsXbe+v"
    L"nfno7M/HYD0xgkF5uzya5K3R0GD1x0MnZOjC0FIfscfYCOic7nNlfyZ0QfNu3ovJQsfbn/AL5fhA"
    L"O3Og+u04sQVt2aHwmiCV/C34mBxKTCPbZ+YIaK4s2GKykL7yaGkey63X+pozzgWf7HM1SFA5wEzn"
    L"0s86s1FswWz1ByOXv9GV/2AGbmQzrxEgB8C8lA9SCTPN4zkTHx2w4AsATmfDsKCOxGDMFs9IPhqB"
    L"Iz0C5AA4nO22c6rND2vziC4AwwI43IMxrM35yjR4oz8GOt/BGuk9Y0agnVNtngA1p/d63rBgCwCO"
    L"AY38pECDN/pjoD8pJ3PUjzQCmtN73Ros2AKAY5BaGX2NRmA0AkdlBNqNzueCvGALQLvh+fLzCXC+"
    L"vkZ6oxEYjcDeIzCfC/JRXQDmE+DeXdq7xAIC9pYevpLaOqrjdfh6NvL80zoCx/SEZgEB7ZOnJN3r"
    L"IUe77lB5tTV6RXiogziyf0aNwFFdAA42Wdt2bZ6RVZKmhxyDcurAbHLqDgXye1TH8lBiH9n+ZIzA"
    L"XL3Q/Bx6YTyqkzYnK4ETIIA/ELR9zMfuQPXn4xMd+R3tDhiIEY6pETiqC8DBjpSSLV3l57Kfj85c"
    L"9qO60Qj8JI0A+TDsAvuMWQAIELQHfVjA7fqD4efrU3r9sYEHB9PeyGY0As+UERjML+LqT3IKhwNK"
    L"nKH3HvNpa1jA87GbS2e+PqXX39LDg7n8jupGI3A0R+Bg2z7sC4ASZ7/b9YMN/mjYaUE77GN2NPo1"
    L"avMnawQ0T+d14R1N5gM871rQ+juDAzQdqY9G4IiNgObpvC68owXgiJ2SUUOjEXjmjcCCLgDz3XbM"
    L"NQwL4aPtf6H9tX2P+NEIPBNG4FBiWNAFYL7bjrkCXggfbf8L7a/te8SPRuBYH4EFXQCO9cEYxT8a"
    L"gZ+2ERgtAD9tZ3zU39EItEZgtAC0BmPEjkbgWBuBQ413tAAc6giO7EcjcAyPwGgBOIZP3ij00Qgc"
    L"6giMFoBDHcGR/WgEjuERGC0Ax/DJG4X+0z0CC9H70QKwEKM48jEagWN0BEYLwDF64kZhj0ZgIUZg"
    L"tAAsxCiOfIxG4BgdgdECcIyeuFHYP90jsFC9Hy0ACzWSIz+jETgGR2C0AByDJ20U8mgEFmoERgvA"
    L"Qo3kyM9oBI7BERgtAMfgSRuF/NM9AgvZ+9ECsJCjOfI1GoFjbARGC8AxdsJG4Y5GYCFHYLQALORo"
    L"jnyNRuAYG4HRAnCMnbBRuD/dI7DQvR8tAAs9oiN/x/QItP+IbJs/Ep06Uu3RDqBPowWAURhhNAK9"
    L"EWj/Edk2T3VOGvjDgcH2Dkcb+KQdAD9aABiFEX6qRmBYIg+TDQ4KSTOoN1getHkmltsxjxaAZ+IZ"
    L"GsV0WEeARB5soC1rJ8hcetS17SgfLGgTzGU/WHeg+tm+HfNoAcijMqLH7AgcbCLM1uF2gsymsz95"
    L"jinT/enTJtifXrse/bb/zENBW3c2frQAzDYyI/kxMwIkwuEOloQCg+0Mk6FzMDHhC2A/H6B7MO3g"
    L"G1swWgAYjRFGI7CfESDRwKDaMFlbZ3/1g7oHot+2hc+2vAm+gAAAEABJREFUUICMJIcOgnowWgAG"
    L"R2ZUHo3AHCOQEyrTtmpb1uYHdWara+sN8rnctiWBs3w2uj+d0QIw28iN5MfkCLQTZLADs9XNJh+0"
    L"p5wTKlNkGW1Z5gd9IwfIM7J9psgzP0ixRYZOG1kGPRCMFoADGa2R7jN+BHKCECgJAs1o12UZdJh8"
    L"0Ba9g8Ew3/hBnkG5DeSUiQHADwN6GdTDQ/cHfAL0RgsAozDCT+QIzDchhnUe25wkw+qpA8Pq5pJh"
    L"AwZ1kIEshycGkGUHQ/GT7eABPgHy0QLAKIwwGoEhI5CTZEiVUQeG1c0lwwbMpTNYN1uZZM518CCX"
    L"M223BQ9yHXS0ADAKI/zEjsCwpDjYzh6KL2zBYNtZRmIC6rMsU2QZWYYuyHJ4QFk6887reSvieITR"
    L"CDzTR0CT39sx5qRoyw6WPxRf2ILBtpERs9DPRWQAXckdwB8AZFL3x0GFPo8PygC+3yiFEUYjcKyP"
    L"QE6cY6kfvZiVk08nLfEjB/AZg+Usb1N0QFsGrwbSYkIdQDZaABiFEY7pEcgT+1A6sRA+2u3jj3Km"
    L"8LMh6+SkbOvBzyanTrb9HBafrvRQQD3I9pkiy+gbZ8GIjkbgWBsBJjY40LgHk2QuH+iCwTaGydBp"
    L"+0IHIIcC+Ax0AXKQ5W0+y9qUetlVbdn+eOnX6Mg2CD5aABiNEX4qRyAnw3w6jy5Al8SBgiyDHwbq"
    L"M6hv85TbGKyj3K7fH5/1oQD9dqyUM1RfCfVoAcgjMqKjEZjHCOSEyrRt0pa1+UGd2eraerPxbVsS"
    L"eDa9LN+fzmgByCM1oj8RI9BOkMEODatDBgZ1ZyuTUBmDOsizLPODvtty1aVteLbJVPJ0L5/LbTpg"
    L"nx7qZf1M2/r740cLwP5GaFR/TI1AThCCzgmRabuOeoAMwLeRbbJssJzl86GDtrTXQ9qGD/qgDhl2"
    L"AH4Y0MugHh66P+AToDdaABiFEX6iR2C+idEeBGxykiCnDM2gDuTyfCk2YFAfGchyeNoEWXYwFD/Z"
    L"Dh7gEyAfLQCMwgg/kSOQJ/nBdm4ue+pA9k1iZb5N2zrIKQP4Q0W7TXgw6LPdFjxo64wWgPZojPif"
    L"uBEYlhQH20n52idfJEv36+3EQgZyO9RRBsgybfPogCxrU/iMbIsuyHJ4QFk6+8SJfBjmrTjMeCQb"
    L"jcAzbQQ0+VNC5rhyUuTyoVD52uedu2Tpvfqg30E5ZYBeppknZqGfi9QD6iVPD/rgDwAye/qnClXY"
    L"a0woA/z1G6UwwmgEjvURyIlzNPtxoDH09JWTTyct8SMH8BmD5SxvU3RAWwavBtJiQh1ANloAGIUR"
    L"jvkRYHLvrxNz6VAH9udjIerb7cALKQ9zUg62MZscPdnudXVHNohsn2m7PjXcFoz40QgciyMwbHIP"
    L"9mNQp5081IFBm4Uo0w7AV6bwoNdm3aOILH8N6mZ5mw6za9cP8lk/+x4tAIMjNCr/1IxATobc4ZwU"
    L"uTwbzXqZzqaX5bkd9OEBfK6HDpaRoQc9HMi+RwvA4Rjdkc9jcgRyUgxLxmEdOhB9dEH20+Z7sv1u"
    L"5Xt6C0pGC8CCDufI2dEcARIXHEgMw/SHJOdeLgfrB8t7KQ8p5DYzxV7Y5w3DENMFF40WgAUf0pHD"
    L"ozUCSqLZ7qVnnefYzCfenKyDusjbmK2+LadNbNqyzB9pOuvAHOlARu2NRuBQR2AwqVrl9K6+VR7a"
    L"1GB9u0zSDjNCDqjLFH5/QBfsT+9w148WgMM9wiP/R2wEBhMqlwfpbAFlvVw/WM7yuWh70UDvYHxg"
    L"d6QwWgCO1EiP2jkiIzCYgION7q++rT+oO1hGN8tyokMl2+vXfJGhm6H6o/LAL7ffpqMFoD0aI/6Y"
    L"H4HBZDuUDg36Gizje5hM8qHPIiRPxyw2liqP8NdoATjCAz5q7uiOwJFIvtwGV3pwdHs8d+ujBWDu"
    L"8RnVHkMjsL9k21/9QneVhQAM+m3HAQ8GdY5UebQAHKmRHrVz2EdgWLId9kYPooF2nG3+IFwdsslo"
    L"ATjkIRw5OFZGYH/JNnglbpfbfO7vMFmuG6TS3evBoMp7PQgcLA/aH67yaAE4XCM78vuMG4H9JVle"
    L"IKS3T15QJ/leSYuMTg7KkQ1Cuvv8/b8Bu718D9ofrvI+HT1cDY38jkbgcI8ACQVma0dJmH4giPq5"
    L"9FSf9Nr6ks16zFev7QAbgAwqjH4UmMEYYTQCBzsCSqKhr9+GJTu6uZ3B+nZd1oEOk2PbBnoAWaaZ"
    L"p5yBDOTy0aKjHcDRGvlRuws+AvtLqNnqc2IP1g+WhwWMLaAu0zaPDCBrAxlAdjQxWgCO5uiP2l7Q"
    L"EZgtobI809kaHawfLM9mhzzrzmfRQP+ZgtEC8Ew5E6M4FmQE9peA+6tvB3EgutmOheBg7LL9kaaj"
    L"BeBIj/iovcM6AiTgQjVwsL4O1m6h4j4QP6MF4EBGa6R7zI/AkUxOdgJgrkE72nWjBeBon4FR+ws2"
    L"As+0ZGOxAQvWwcPgaLQAHIZBPVIuv/SlL62+8cZbX3XTTbf+11tuueWtN99862U333zzF2688cbb"
    L"Mm644YbP3nj9jW+8/vrr33jdddf9KfjiF7/4m8KrMo5UvIe7nf0l2zNtgTjc4zEf/6MFYD6j9AzS"
    L"uenOO09Xsv/Ozbfc/PnJqYkfjI2F61esWPrW49et+6+nnHLivznrrDP+2QXnn/uzZ51x+s+eeuop"
    L"P3vK+pP+5UknH/9Hx5+w7o+OW7vmj1etXPHHK5ctf9+S8cXXj3XGrjerr//CF79Qf+ELwrXX1tde"
    L"e+2t11xzDfiM+D/93DWfe8PnPve510r2qs985jPPfQYNxQGHsr8FYtBhe8Fo84N68ykfqv182jgY"
    L"ndECcDCjdoRtbrrpprN1Zf+jm2+55bthcvL+EOJ7Vq1c9S/OPO2MteeefbaddcappkQ3LQK2atVK"
    L"W7Z0qa1Zs8ZOWHecnXTiSbb+5JPt9FNPsbPOPNPOPfdcu+CC8+3iiy+y51x6sb3g+c+15z/3OXbp"
    L"xc+y81R37jlnv+yM00972YknnPDLx61d+8crly5/3dLF41eWRXF9DOHr//iP/1h/VhC99bOf/eyt"
    L"WhTe/tlPf/oNov9RZRaJM47w8My7ufkm4TA9Fo9h8vk2jn1bF1+gLTsa/GgBOBqjPo82b7vttnN1"
    L"pX/drbfcck8I4UeLFo29ce2a1ReeftppdvFF59vZZ5xua9aussWLF1mMhWky6WJem3XrxNd1ZVUt"
    L"vupaVYmv4KtUp29TKfHBo8UYbWx8ka1cudxWr1plxx+/zk7TgnGG2rjwQi0Wz77YnvucS+0lL36R"
    L"8EJ71oUX2EUXnK+F4tSXnXrK+t897vh1r1u+bNn7y7K8Xm3d96lPfar+1Kc+ufWTn/zkrcJnrrrq"
    L"qjdcffXV/+GTV1zxqssvv/wMO0qfdhLWdb3fn71v6y90yPgGC+33QP2FAzUY6R++Ebj99tvPU+L/"
    L"ya233vp9TdAfKOnfsO74dRdceMF5SvoL7YzTTtWVfbWVpRJeKcyhvG4CqpXSAKEksuc7lbQsaHFI"
    L"h76qhFqLAjqVFgp4Ja5VsodSpk4rhI6eX+lVAnVLly6xZcuWaqE43k4+6SQ784wz7EItCiwSL9Ui"
    L"8YIXPE8LxAUrzj/3vJedcsopv3z8uuNet2LZsg+Ui8a1i4j3XXnllfUVV16x4YorrrhVePsnPvGJ"
    L"Nwivveyyy16loI/I0U4+9XWvxaBd1w5mNnlbJ/PZJxRk+TONjhaAo3xGtL1fq4T/b0r8G6u6/n6n"
    L"03m9EuY83cenK+0pJ59ki8fHU5TKT1ESUiQfEmqCKVHNdu/ZY9/73vft8suvsD9+3f+wn33ZK+2k"
    L"U06zs88+35596aX2sy9/pf3Kr/ya/fbv/H/tf/7J6+1d7/4bu+rKq+ymm26xb979Tbvv3vvsySef"
    L"sO07d9jExB6bmZmySisMSd/QuilrRZHYan3VWhSMGLSgVOKBbhVsqRaIlatWaIE40c4666xmgXju"
    L"pfbiF7/Qnvvc59izLrrwtDNPP00LxPrf1e3K61asWH6le7j+45/4eP3xj39868c/ftmtWhD+7rLL"
    L"/uENH/vYx14r/Hzu8kLTA0ns+badfULBfO2OtF440g2O2jO744471inp/28l/W2xKDaWZfyrNWtW"
    L"/5we3NmztOVe30p6xkv5pRxT1ul6zrdIKu/Zvcd+8P0f2gc++CH7rd/5T/bP/vm/sF/65V9O/Dvf"
    L"9W77zne+Yzt37LCNm560Bx54UOVv20233GSfuPwT9vZ3vDMtEr/5279jr/m1X7Off/Uv2PNf+CI7"
    L"/8Jn2emnn2UnrT/V1p1wsq1dp6u8FpFnPfsSe8XP/bz96ZveZHpoaE8+8YSxAFQKruolfy5Da8lr"
    L"LQg1df3bEN18SN7RDmbZ0mV2/AnH23rtIM479xy78IIL7KUvfZG96PkvsIsuunDFWWed/bJT1q//"
    L"jVWr1rxu+fLlV9ZW3/D3//AP9d///d9vFW796D989B2ibxBe9dGPfvS5dox9ninhjhaAI3Qmvvzl"
    L"Lx+vLf5/v/X2W79U1dUTnbL8cyU9T+vt2c96lp12ynpbuWJ5ikaT3Wolig7lei2ZoGOm27UnHn/C"
    L"rrn2WvuD/9//bb/62l+zX/ylX7L/9nu/b5dfcYUS/Du2ceMmXaWbRMOHjNOhq1Ci8/1SfLrdWGPn"
    L"nnOO/Z//57+1P/+zN9vln7jM/uiP/rv9s1e/2tYet8ZqVztpF6CIFR/JbipXPVCutADUKnfVGeKp"
    L"06JQG3LQVX2lfmlMDL0Q3ZbpFuM4PcQ8WQshDyYv0u0FtxYvev7ztThctOJ0dg4nrf/Py5Yte934"
    L"+Pj1dVV9/SN/95H6Ix/+yLc+/OEP3yq84UMf+tB/+OAHP3jYdg32E/IZLQCH+UQq6f/zHXfcdtfM"
    L"zMzjRVG8ae3qtS856/TT7eJnXbhX0jdhKItM0GHGbakYDsG8NrbWPKD757/4z+0v/p8/t8999jP2"
    L"g+991558/DHb+MRjtnnjk/aUrvaPPfqwtvM/sn/67rftzjtut2uv+byS9xP2nr99jxL5z+0P//AP"
    L"7Xd++7ft3/ybf2P/8pd+2X5OV/aXvORn7CUveYm98hWvsF/SovJbv/1b9o63v80++9lP25+98Y32"
    L"i2pz7dq1ZkpmHaZ8NuWuwdSKOSW2hFXiFTA8xMwq8abEJ+ErGVfilbRWdSur5CTZIxdPuYKXTpXK"
    L"XenUCXlxOF67kvXrT7aLtFt6tt5eEPclFz/bLnrWhc/WruFlxx2nZw4rVnxAQ3iDFoFaeOD97//g"
    L"raLveN8HPvD773//+1/17ne/e5VC+6k/wk/9CByGAVDS/zdt82+9/Y479sj9O1YsX/nCM884zS6+"
    L"6AIl/cmm+12JW4cShVLKkx5vJJKEtWjikQs6JO0dwa3QdrrTKa3QmwCu8iwSi8YW2eqVq+zkE060"
    L"8887z170whfYq37+lfbrr32N/eZv/n/sD37/v9mb3vgGe+c73icfNhwAABAASURBVGYf/tD77Qpd"
    L"2T/zqavs05+6WgvFx+1DH3i//c//8TrZ/LxiXWFVikFtqvEmxoqQrFaBOqgKjZaSl5j7UCJTL1XV"
    L"S6pyJZ1KArlLflI5JXtlaUcA39oVVMlGdXrDUVGncpd6/CTM2Pj4mC1ZstTWrz/JeN15wfnn20tf"
    L"/GJ7np43XHThhaeeccZpLzvhhOP/88rly99SFPH6slNuft973/vA+973vlve+973vkH4D+J/Xr38"
    L"qTrCT1VvD2Nnv/ylL/3Bl++440tK/El3+ytNtJeddsrJi56tK/0Zp59qK5YvH9J6SoFeYjS8Cikp"
    L"0hdJInEtYUqiHtXlkHyTimrIRfRU109GV5XKNChz6UpPOojwkyl8rWSqVZeBpQxEZIMDQdUSURYo"
    L"SEZSSqmRqxHE2UempuSsQdOgaSWRvoylLE/i9d1v31K50alFalOVXFT9nUJVsxtQWc8UavloFouu"
    L"Fo1aqKQPX1leJNhhxKJMDySPP/54O/XU09KzhhfoVuK5z3muXXjRhaeuX3/yy9kxLF+2/ANV1b3h"
    L"b9/zt/Xf/u3ffks7hM+85z3vecN73vmeV73vHe87Q1Ev2PFMcjRaAA7hbOi+/g/vvPOOr9xxx51T"
    L"usf9i2XLl79E78U7F190oR6knWq8U496x75PE/2EUY0msr6bQ7ymvdKl1oQWpwyodcWrSSKhol7o"
    L"Slu1JpF0XckSrNaqY6bT6dHMRQM0mqtcw4dC4iCIslvoIcRSMumpHAQTXDG79EMIcqk6UZdPl3/X"
    L"rqMWrc1NoRiflHApzq5kikwVOsQrqaVAsqbYrVVXqU5KSS5aqxdJr6dD57IMR9SpikOGZpU6X6vN"
    L"vaj8VGmXoDhUxwJRa7FgtwCttWvo9sq8Sl2mB5EnnXRys2PQ7QS3Es/SLcU555zz7FNPPfWXecbQ"
    L"Ge9cPx2n73v3u9699V3vetctWhjeIPqad77znc9V1475Q2f4mO/DEe2Akv5fCbfceeed05qUb16+"
    L"bMULTjvlpJLt/em60q9etdKiEmgwKM3NJJKN5nPVQ21aOIRmQndTUrlSIaSkNCVnKMdEOhY7Y1aI"
    L"LzuLbGxs3MaERXo9CMYXj9v4osWmB2IGv3jxYlssGXWLRBePU1a95OPi0YHyenFcssVLFtsS0SWL"
    L"l6RXjkvE44O6JcjAkqW2aMkS+ZcO9SqPL5ZsfKnaWmrjKo8tWWadRePW0S1IUSrmsrSgBSWGwszV"
    L"Ly1olRKz0pW8Fl9rUGrTB6qEFmcaGCV5LagkGTppkZAEnqRPFBug0ZKazGotCpVoZejTTq02oBJq"
    L"jGuh6u8mWBSoAywKVbcyZIzhypUr7WQ9gOTNxHN0C/H8F7yA3cIKPV94+apVq163ePHiqxTd19/x"
    L"znfU73jHO2555zvf/ra3v/3trxF/zO0UgjoyOvYzAnfdddevCnd8+c4799RWf2rR2NjLTzrh+OKi"
    L"C86z0087Jf34bYyx76VmYmZwpTM301W0TjSaKymCtqah6FghdJQspZKmoyQvF43ZokUkORA/NmZj"
    L"qhtLdMw6nY6VZWkFkI+iCFbEYFE0hsLcgwVdpa1uaAjBHLgoCGbOP9c3Zah0a1PuCSZZ5W4m6NCz"
    L"R2pkZGZBAhBDsBiCFcEtxmChiFYIZSxsTPERa+qDFqjxJeNaOBbbEi0QS5eu0HYcrLSluvouWrxU"
    L"fR03+k5/zM1ISA2dcrZWateJSmrKZdtrXHsLgoiZKnNdWgz0lcvQvBOoJa+k27RRG3zNgtRDd6Yy"
    L"9KlnMcCONxTqpt5MLLWTTj7ZzjnnbLtYu4QX6/nCRRdcZKeddtrLV61c/X+pz1fJ331//fa3b33b"
    L"2952y9ve9rb//da3vvXntWNYZc/gT3Nmn8EBHq3QdJV/rfBlJf5EXdVXa2K/9MQTT1h0vl6LnXP2"
    L"mbZ27RoLQVtvzVYmjWn2usoeopKisKjkTEmuRIUvVS6V4B2ghE6007Gy0zEmv94QWIKSCL8JHmzo"
    L"R4uKKT2A5m7DSVZZ10z5iqx2seJrTfhaMVqf4rEydJjoCVyRZdjoYWS9epPHWjXaUstHjZ4MU2KI"
    L"qklLNkosVZsKVolhV6MWzKRTa0uOnFZrxWBkExFrMSm1aChxbNHiRbZEC8LSZctt2fIV2lEssUVa"
    L"PNhJlGXHPETDRy3fOow2Gl6x9dpGDvp6arDRw1R6dEsK2MnEUr8VX5vWio+dQJ3lumXg1Ss67BK6"
    L"3RnZ1dopVEJXi9hSO+GEE+ycc8+15zznOfbCF7zQnnXhBSvWn3LKy5cvX/4/tFDfMDk5uVkLwTf/"
    L"6q/+6m3Cr77jHX/xjNolzDLD7Kfy85WvfOWlSnrl/JcnNQBXLlq06EUnHL9u7LzzzjaSfs3qVbr6"
    L"FqpqDnc3V5KS4IUmaiw6SuLS4AGTt9QCQGKHGI2kdvfG+IC+SbXGgMlYKYE0RzUZJdPEVlEME11Q"
    L"ujYTvxYnqJ5yV0rJVoa6DVZdV7lCvSDrSpMfh+h0tR2uSXYlDLqmMnKZGsldI5d+JQpfa5noKllI"
    L"kgoDgcTvkvwyIqlYNNSg1ehpnaJuZmbGpqanbHpy2qanpmxmekYqtRbQaGPa8XDr0uwcltu4bkNC"
    L"jMbwMYKufql59YP4K3W8Sra14kJSK8tzvSqbIwn6htJHLG3JJVUZvrIKH8SNXBRflcagVpl+1r36"
    L"pr/SV12lPru7LV2yzPjx6AsvvNBe9KIX2qWXXmpnnnnmJWvXrPm/xhctunp6Ot6nhWDDW97ylg+L"
    L"/uqb33x0X0cG+yn/fPWrX/0ZJf1VutTvqqvqjrFO+cITjj++c965Z9vZZ55uKemLIk0OhirqCl10"
    L"Flmpe3DAPW+hq3uIsUlw7QLSLEX5UKF5rbw1zUERTTSYnk9N1RRTn2pypipoQteYyCSuYaekU7bI"
    L"ppn0pkksj6rSt5JSDSi5dcXHVskjaapjsjdJL4nq6h6kovtptSHfOnAt+1qoElK7LAxZX21ofJNP"
    L"2q61wFTUyVHjs/FFgs3MTNvU5JRNaFGYFk85anyXaYewctVqW7F6tS1dsSLdDrGwVPKhw+TuaaSI"
    L"apUFjWPTRm2NXt3IZdDINSbwsumXGcwkq2xGCxntdNWHSp2dEU28KIvYjHYG3ZmqWdwqFjH1RbaV"
    L"Glui5yYn60HjRRddZC980Yu1KLzIzjv//NPWHXfcv1efri7Lya/+xV/8xS/pzUNJk0ca4Ug3+Exo"
    L"76tK+q985ctX6VK/q6qq28c6ndesXb1q8dlnnaEr/Vn9pCdWru5Noi+2jh60xbLTJLpWe3euRdLK"
    L"VOyCHJqw+Kl0dioyU5cn5WuatPpimhpidJiwSaYJB5+STJW6KDU62FJHMmpCVrpSqc+meUy2y7SW"
    L"P7GazJV0ahl2lZy1Jn2dlOpGR37EyE4RIZeeaiTSN7aCKtUmZUFtdqXTtFWpjVoLQy2Vyip1plYb"
    L"FZAvrq6VYqtlA7Aj4Sol1sz0dNpuT2ohmJzcY7t377GpiSm1U9liJdeaNWtt9eo12o4v084rKh4N"
    L"nvyoEauTT0ttV5LRhabUfNcpDqQ9iCQ9dMXXxCYfymwNNQLZqazqp32qbBJU8tVNvJnMLM0Mmcgw"
    L"6SZW2xYeMp6o24ZnXXxxWgxe+cqXnaXdzj/u2LHjfegeaWiKHekmj057JP1dX/nK1V/5yl1K+vr2"
    L"sui8Zs3q1Yu5yrO9P+H4dWnb6R6sKDrGlZ2EL7SFz1t3dzd3t4X/aNJq1lQZSmB4XWKZW2kC9ZiG"
    L"l3oNp4lHLLrepFmXEkky5ZWhX4tnUuJLriWrsFJVraTCQ5UmK3aq0Dxnmgqyw1acZPIu465QkeTU"
    L"iabtby/BtVYkn8lGO42mvW5fVsnGlBzUyw29U/sKR7rIiISkxw6/MtSh1lWvJjQM6rCYSg3VSjR2"
    L"BJPaHezZs8t27dljExMT6bzwHGH5ipU2roUhlr1dm/RpW63pUEuKpRbUAENHt8XWUmmQhPqqWCgV"
    L"AgqJ6CuZJUFttfpjCDQdVDLXPw7TF/PF+DhfZu5i3Mx6n2ktah09+znvvPPsHD1TeulLX6qa+t//"
    L"+V/++c+JOaLHT/QC8I1vfOM0Jf77dW+/RSfs9k5R/urqVST9aXbuOWfZCccf10t6N15XFeWYtvZj"
    L"FopCJy0IOnEGbGE/ygJNaWPCN9SUFGpCkyxNKlhNLh22V1mTr9aEZsIlW/woMUh4kkx91DXc5Lcr"
    L"kNzyrkr0azlrkqsnV4tdTeKaetVJqqakqSSTdd++Uh22qjQ1JVSpDUux1ErkWrqyUCxsi9PuQTaV"
    L"fGOTbNOCUSXbmvin1VESLEFy1Te68qNGuvJViapGYskUK3EA/KtBHZUWBkUhPdqdmJq03bt29ReD"
    L"RXrdCTiv2GlI5aWWP7Wt+HSIl1RxwpviklMd8qu41D2rdcWWthpp7Gp5UMGSvpK6FnTIieYIh0fN"
    L"I55TuLkLeuAZRKWgtymueCvZ1sZvR5500kmIU/kcLQInnXiyhSr8SRIewa9wBNs6Ik21kv7RmZmZ"
    L"DSH4f1y5YvnK008/RUl/pp14QnOlr9NZNH3cIolfamuv+0z3YO5uC/lRGrbcVWkaBU3yJBQllgSr"
    L"9c80CaGCYqyVbtShW0lXR5pImjkcRtJLIFrJr1qSAvoyTVfvNK81iytNapGUsNTjq2byp4Sv5UL2"
    L"sq1kUEuGDjQlI3Ilad2rq8RzxaZODVutxmrqEq2NJJWJUV+p3fQMQT6lqL511aPKqp5uLapST9fU"
    L"/1pqQFLFh20l34YVTtVFZNhJyyrpVMi1EGQZDxQnJyc0ZHX62YiOrrZyrPGQsQYh6andmsUvjVrT"
    L"ViUl6kR0yLt8U5aJAlMZj7JjdiCDmrl5CFZo7sToFkJGsOCaSyEYnxndzpxyyimm14YUe32UT/kj"
    L"/ksuvcQmJyZf8frXv/65UnDhiBxNdEekqcPXyPCkX3HiqetPtvPPPUdPZU+0JYsX9wedSDixoejo"
    L"FdQSi1H3jhK6L9y4a6rJo2mCitME5SQ3kFizR3OrmWSaAJL0eHGpLAVN+qwPJZmoIu6ujFUt53Wa"
    L"vl2ZdfEghVqJhn5KPMkqKdZKQlVJX1NcDD4UlWKTSB6U/dKsm/HpJRpXVRIZ3cSnPpC8aknhVSqb"
    L"4qiSP2SVIQPEUCu5kp0SE1klv5TxV2GjAJCzK2n0ZZ9irS0luOKiXk4JUiE2cuqQg2Qrv/isFQu6"
    L"aeegNinrAmCTk5MWtaMbW7TIXMlYKa5asdfyr2GTbzojqKCw+O6NhVgdtUoaGL6V6hLwrWmiPLfg"
    L"bh7dSH6X7xBUVtKHEM3dTV82NTVt/BjyGWecYZF51jRi+TOlWxluCdBZsXIFzzf+S647EjQc7kbq"
    L"utZIHJ5Wetv71pW+nfQn2LJlS/sNK46neXPr6IGeXvPp3DYn333hwqw0uYLAJG01ampMB1MKVFZr"
    L"0tZqPsWmSVtR1uSsEtAR+vW1PHY1x5U5cEqWnKCVJnWa+NgpIUxOa4EYWsM4AAAQAElEQVRZW0uv"
    L"aae2mvqErnhBdrX0atoGqutqAUFmaqOJQ3GikwBvVssnUdRqq9LiIlMlKHJJtDvArotQNlLWlVdy"
    L"9QM5qKXTxC4v0qkFNZt8UK9OipeB5NTVirOr2JDXaq+R1abhUqMVkfagImbqODoqafhrYyGoFE+M"
    L"Oitqcka+umqwkgMWTzVj6pIlir2AbfIhXoc108MtpAR3lQsLMVoRREOwILhHUTd0SWq9+tN2/8x0"
    L"m2n6JH89ynMLUCmWjIv1YHC6O/2vpMLh+gIiC3OofQdtb6FdOBy8u26kFtCx7ufffNddd/1ItFJn"
    L"tL2fPelzs9JLLLS2YGP8uKq2hYotyQ/2S+dOE0/WFagsn0gmE3XNJNK3ZlalSQmYZSkOZpXMTJMw"
    L"2amsKS1/tVQEZqZQ5XpqxKsm1eOrUiOA9iTUUcmdNPClNmmHxEGHBODqiayWAWXkXSXG0/La5FKQ"
    L"H/WpTj4q+W36oDxUYnYFVWKnJEIHha4Sk7Zq9TPJtDgofEPWVXtyarXi10mTMx0937RPHHJqtfTq"
    L"JMdLJQ+VzECtugZyL5kJioE2iUF++3bZHqoYRZIu8e3ZM6FRrJIvdiPdma7VdFhK8m4G7cPMzcz1"
    L"1UBJLiYWriSPVhTBYoiq9z6C6llsli5dpsQ/yxYP7DpNH6747ErocxvEc+aZZ+oWrbviv/yX//Ia"
    L"qaZDtwTkqKfCIX65chG03eC8XV4QXidjQQLOwSjZ+0kv2R8uX77s7JNPOsGb7f3eV3rV73UollSG"
    L"1uZWdjpW6sl+Eh7El6Zdsqo0lfTUxiDwTHYmUKpsTaLUbi2pZJqnzF8VZKGJlyZAmvQmuZR0ZaxV"
    L"TnJN/3SVFGVydmUv1irVkzR10lUNcjmuRHWoXnfLJIb8k3zI8MPmPflFTtIoilqVlXSflpMQM/JR"
    L"G7Ku9LpKIpIzldU2fazVXt2zo41KPL6oEyv1ruzVp16MpkGq1G6tXhInyUgZXYUgM9XIN36QU99V"
    L"2zWQQq0YNGLyid9KXrqNTToZ2FpTlq44WpNuJdqMhQwQ2y49JNy9c7ft0ZsDknBK2/Ou3t8TkxxI"
    L"TedA46Ik4dvMXUcwrvru0aKu+NAQg7mrLqiuBxJ4bNGYkcSrVq2SO+Jq/Jk+MzMz6QElevQxo11m"
    L"Xp551lnsWF4hk3RoAagFVyFD7MIdYeFcPe3JtdI8XTo4bq6k595+5YoVOiFzh58mpZqHAg/RWKUZ"
    L"9AxVz+NoJhMziod3nDx4ErLWF751xjWBzNJkEleD/oSsLdnIqFJSNBO/lokgvUoJpfyQJ9n3J714"
    L"RVaRGEqAWgqV7GvVy1BWJiJ7lUmYinoBPeKpdLmuVFfRmBIFu1rxsBgomF6SVo0P2sCWetpUjOiY"
    L"5I1NbdjhKyW8fCsFJdMEh1d8tE8cprKkMlVs8ke73V49Oia+71O9qNRr5IQpdaNOYvW0Ugg9yCF+"
    L"cr3Ckp76L8VahYoK4lYfiFEi1dfUopRopXZJ/gntAlgIWAT2TO7RPfpU0k0+SDH5SYd4N/2LbiFG"
    L"i0G8EEh4z4nvKUZkp59+uq1bt8741L144AFb/Snd66d+KkbmHnybwoP1J5/MAnApdhlaADQCuaSg"
    L"bCjsYD7hYIwOl42S/v/I23u18YftK/18k152ex35ZEC1MGn+Vcb92aTexU5OTxonYi+DVFDGiGr6"
    L"pYko1qRolWZWUyOJTglTTLPLNIN6AoSSsh0Qq3NtyT8JpXKaxEmzm+TUpQkrv7USBz+qUZ017cou"
    L"2TChhEqgHzULhupIRvSTTHVMoEp+8Ku8spSMktfSRQaQs5vAV5Xa7cWrYFO9MlGhGjpdlLNci0mN"
    L"L4BMCaVVRFtWNSNZpZhSXOJrxVDLT4WeeoI/taLu6Vt6yOlkLR91z29aPFSHvJJtlqc25UOtyF7f"
    L"8m/EJeBHTaioMyUG3eZ8aJA5aFhy9PZM77GJqQnbsXOH7Z6YSFdjPXW3ickpNC2llPTdzUxfQYhF"
    L"tOjRPLg5C0G6gAQj4dUTqxXvqaeeaicrad3daB9Y70Pis9DQfgbnaC7wV5dU/zK58B5E5nUcqH5y"
    L"GtL3Ufwi6YVvCNMK4+Pj44v62/uDTXr5SScDCjgpgJMwPd1NC8C0ng7zE2UsBsjRSyDDhYpJVwVT"
    L"jjAnNfOorS350SSsNNPgE6SvHNOUNFlVgtKSSUwiSLdOEn33JmNXE0dzLfmSimqbOpKASUUCVdi1"
    L"oUSp1AixdhUU7cqBclC28tCVbyJU5uqoFYvSVzLk2RcUdOVXSrKtBOlhL1mtYNLVOrXTNWJBlwWk"
    L"K53UtpI2yegD6MVSiVdzGiv5ZDySv9qIk7gNOylUakNVhg9TlBUygfGoVEZOPX2p+2NsrGVmtJHi"
    L"EKs20Kv1lfVUZWpGlfImn8m3HE3qXG95aotN7N6tp+xKftHduyfSDxHt2r0zxWh6Vk0GBVeCK5kL"
    L"vTkg0YN4ZFHJ7y4NYUbPDk468SQ79bRTDZ0Ug+JQU+ngak/yp/Z7cSiptVhqrDW2s/Hor1+/Pun9"
    L"xm/8Bq8D8adGIfMG+mCogWLdq+6wLQC5oUzb0SjZ/w+hn/SLFo0954Tj1xXnnn2mnXXG6Taf7X3b"
    L"32y82k4nN1MmAj9OOjExma4GExPTui/cbUyGvBBUmkWVaUrpKk5CyIEOlZH1IIHmriZZ0q30TQSV"
    L"1SR9sqNaM1RivjUHVKd6mRCL5rkUulapQt+JT3KU+zp1mszoAPTQAZXaqdQqcjKj1uJgmoBQEp6E"
    L"hUe3VoJW1AkmPWxr2k2JSL/UqCYlvrqqR63WV42dsgl/Xfqd6qQreU29ZN1eDJX0UhzJJ5NcnZBq"
    L"47OSZq2mayMO/FVKZFMMOizFLX+JIteYJf/INJYqSrWiJaFrat6aemrUjoiaUp1ZFRqu0VaFjgld"
    L"7Z948kl75NFHdZ53p9eCzAPkEzwLmNhjHtyC4IHkjxa15Xd3bQQEZIKKbM3tuLVr7AzNUe7Xmzia"
    L"GEwfEhu/0Eqdy8hlaMaMngm0gZwylFsJ8ate+cpXBrnlcL7a0G2Bg7ZsgN/HRvHuI8sNDNgeetF7"
    L"zwEyVcLPmfRr0m/alYfesDyoo/rWoXNTM/10MphPu7Xyp9V5csKmdk+nybBH/Pat2237th02qYWB"
    L"k4auyU4eRORBAnx20+wzTUhBU67ShK1BmvipMdV1BU1EybtKjFrJhS0JUpG4ioUJqnzqTWT5V1vo"
    L"1FoZavmqpCMvar6bdLpSrpDLHwmkcORO2pJRriSv1A736bUqdSS7WjKjTv5oW6mZ5Poi+hRnJd9d"
    L"GdCGBDq6QiWzWmr0SX2BSEfNKcIq1RNjrT6m9oifNvCKDC0p49dUJ2fJBkqxVn2yw6d6qYZ0qD3x"
    L"xFElP/rWeNO/5ANdxSFF+ZKiHCmyhlfbiUFAlUAibdmyxR544AF78KGHUvJPazymBNquutNWyy6G"
    L"wrjCu3lK/Kin+1zVXUkfWBTcU+Kv0DMnHvCND3myb/qQ+MwtxgV01U6m8Ero5CdTZCCXoZQBi4ts"
    L"z7j55pvr3iLgaqINU/LXQPJ5H66cBG2Dw7IAaIAJ1r72ta/9rN7Vf1LJv1uNfnzwSr+QSS//ex1p"
    L"rhCFGB1WazJN6anvrl2TNqkHMpPTUzap5OcqsGvPbtuydYdt3PSUbd++M90i6ATIppmU1puQIprD"
    L"XckrQXVyrPnezD1NJpJQ87KpS1evruqqHhRemsS1VYqFeGqVjQTMSZr4WiI5TnxlTQJUTcLLeaXE"
    L"qnoLCfHU8lHLX5VoN+lRj51KRhKSVF10UjtaBmRfJ/+1SWzo13LWTT5qw1+tjtUqax4nH4xHJR3s"
    L"kNfEkfxJqr7TXerkVrcV2tRIJsdGHHIln/Kr+JOtBInKP7Qrv406GSzkeunLkEOc2kl6qs8UI/Ea"
    L"2f5BnCz0jzzyqP3oRz+2h3Xl37Nrj6VfKpI+O72ukp+43IONLx43JYVF7vdDsCbxg+79g7biVXqV"
    L"R+IvX75CcagPirnfmBgSf677fBI6gyQfBurb8mXLltHW6XLvWgRMiwAzGUiUDniQCloI+nwSHMBX"
    L"OADdeamS9MLVJL1Oxm26l/oVJfo4W3sg/pBew80ZhPJGhyaLvnVoFK3ihMHLsNMpbHpmUq+BJnUL"
    L"ACaMHxvld9KnZqZs1+499viTG+3Rx5/UgrDVpvSgUH2wigmJ176vWjJNSE0oZnutZGAi0wz38ZXk"
    L"qtUEqhVCbZXsKmVaJUqSyZUlHfSyrTKHSYlcWaNpXfVtSeAk7yUubVXJHzpdXU+rnj8lXs+fyZ8q"
    L"klwRpIWhlqwmaYHarqWbdg3KKXwqPJnVQpXsiAedWqtAUy9PsiMWYkKmIHV0rZIvU53Jt7ToouQa"
    L"kRynJLUa6Iqihw9AG8b4yp76Biy5guLCJXpQDYrO4uzHrl277cEHH7Tv//CHdu+999mWLVutkm9l"
    L"M60aLMyMXLi2+VzVV65caUXvft90xTelEjGUndJOP/1046EcZSCz/jGliwjJXymwDBK5jXZSD/Is"
    L"RCDL23bLly/X3OkqEqKxtAio4X55kNcCINHsh2LHdqjCgiwASvj+lV6DsVfSc1+v+3vT1X9oAAsn"
    L"1GTDmSYZJ9mar0T0fCdNxkVjHVu6uGNezdikkn1qclpPhCdtUm8DpnSvyO3AxMRu27Ztpz2xcbM9"
    L"/MjjtvGpzek5gXLP1Dfle1eo5E+u1ZbmralCR8VJk7xOqFQnRkdtFUoKD3tjwuSEEiUBKjnvKlFM"
    L"n1p2LCJMiEa/Kx+gNlSor5XIlRKtgfXlXdlik3wqW1JyS2bo064cKHJLPiSXx4ZXZlSKhThq6aVw"
    L"VV9hJz/4bKA4NK7YS6yuVJbaUqLKtckNtQpInZU9eiJm8pkWIxkFgd2CumpqPCHpSI5agngOS19y"
    L"npSHfzFOm7XVv//+DfaDH/3QHnz4Edu8ebPNaEytMGOCx6ArupLcCFBX/bFyTPfya5Xca2x8ybgF"
    L"Jb47mmauf/zMPn/px90VovpiT39oj8SHNmNSGTSXoRk5uaHtZJ/UQ0kWEGSZopOBvXsKKJx99tkE"
    L"BlxRZAqvomWaeC0E7XKS6UuuZv9hPBxK58CP1s/f79YAzCvpt23bbv/03Xvs9ju+ZJy0qz/1abvi"
    L"qqvthhtvSvJt27YdWCD53DCDxNeafsmBeJ05HbU5Q8Ks6tXxN/OXr1hmnbFCJ27GZqZmbHLPdNoR"
    L"zOgNwcxMnf5CzeTuKdu+c6c9+dRT9uiTm+yRJx63Tbqi7NJ75OmZrmxBZV3N/Eq+K1ERU6M6atGu"
    L"KUelp1pNPE5qkxC15FWSkwiNzFSu0wKisZS9CbXVsqsEdLjisjBU6gttqgVd1Wmj1oLUxCInhq4q"
    L"dDQyEjsvBNK0SmOFTk2WKwD8VWRnT14hV5vITQlIt+RURy2fkqhT1FWqMzrcs5OCJJWI9CSjzaSn"
    L"eEnhSrW41jrTnCLp0AcKSS+laqMpVYnhRQaOWnaTenW3des2LdCP2o/vu9/ugHz57QAAEABJREFU"
    L"u+8+e+DBh+wpLdaTWtSDfMUYjJ/Rj7GwIiBxXYQW2Zo1q4wn+MevW2f8sY7oUXNEy5IWDP5wB6/1"
    L"gvRpB1jrQ+KTsCle9YtzCg8FOYEzzbKc6CQ+oD7LMk8ZffzRruBqOvz4xz+GgrwYwA8DzwVkYtRB"
    L"2+jLen77dQe0ALSSvv3z92l7P58r/X3332/f+c4/2Q+0RbvuuhvsZ176Ej2c2WMb9KAG+b06mf3I"
    L"5mI0c9QRS7+uqQmhoiXoS9PPdP41EeWAsuotrQIugaCjUxS2ZvVKW7l8mZV66FPZtHWV/OwC0rOB"
    L"atomq0mb1DODCT0Y3KGt5eZN2+3JJzbpFmGTPandwVObt9k2PS/Yo53DtGxJ1lqJpLRTQlZK8lox"
    L"dI1EILhKs7rSJMsnuKu4SE7KlZKqli19qpR8tbKuVn3d08cu1yOnnGzVIxIZWa3Moq1Kvir5qNRo"
    L"JR9ilaeKRVfzWn7l0ipN3kR7bUrNZJbkCtO60qslrMlY0S76QHI5k2f5kxzftKOS+qrB1oIiJzqU"
    L"vDoUnuTpu0/RZyWRO5oSeooIUG1BTRhtTyrhd+zcZRs3bbYHdW+/4cGH7d77HrAf33uvPfLYo7Zz"
    L"14SZLpgxliJuJLDpE0NpLAT8bUG21evWrbUTTzxRV/81ln5DUDokHdt8tvvcCjT9Vl9Ul48JneOD"
    L"uc8nqVkwQE50eEBdlmWeWDg3xKC2/bjjjouigDx1LQYqMrvNL7zwwgAk0IzWt5nrWQF6pg8yILZ/"
    L"pLLrQWBfIiYbiB1+7Jv04T+uXLn3z9/Pd3u/detW275jhz3vec+3V7361bZofLH9+9/4d+m/qHrt"
    L"r/2aVZzx4WE00n597wSJ6OjVQVTS0cy2WhOV6dqAWm3oVNA4pCPYEj0AWnfcajtu9RpbsmSRWXAl"
    L"r+4Qufefnkn8zExlXV3xu1okppUlO/UQcbP68MTmHfakFoGnNm+3rSrv2LFbE3HKpiZnbEoLCEna"
    L"lb4p8bhy16KEn5JU2VerjhNdKYtqpUEt2pVOpaSslAxVSmiFKyMSMiW2bJBjV0s3TRjZJn11EFmt"
    L"XsuV4c/ko1JbJFElva4qaiV1F59qp4u/XlvI8UtdJT1DrtxEBzmo8C1ZrXrKJj+mRUFNSL1SC5W8"
    L"KmbFYqpLOrKp0UFJGs1PUqIgR2pDIgrSsuSjKxnPXlhYt+/cYZs3b7XHn9hoDz70sH3/xz82tvhP"
    L"ake2Ww9uI1d3L5MLebMiBmNBDzFaqcWgU3Rsme6n+U07En/duuOM3woNgd1fZWvWrEk/ujs+Pm58"
    L"asUMzSBRSf5KMWUw5m2QxMOALSC5oRmU28AW37SNXyiQLGzcuDEolgwWApDK99xzjwuqTosB8vaz"
    L"AuTA+WphsGw4a9U37LCkX0XSn3KyXXDeObb+pBM1sMsa5QP4vvSSZ9srXv4yW7VypfGqY/HiJTaj"
    L"SbhID1yiku+5l14yxBsZjbhOk4TBQZKQvkxzjTohlaEumWzUXQdizWAEM3HBXO25e4pjme4DV69a"
    L"YcevXWkrly2xsU5HSrUWAF3S6rwQzFiXic9004QwLQgsDEzULXqF+CTQorBxyw7bsX3CduoWYg8P"
    L"inSLwQ5hRsk4o8WkUuIyz1I/YJQYXY2BTrj619XFsVbstdqqlBBdxVA9LVN61Wob3UpxJB/yl8ry"
    L"1ZRloyRHT9HLVzM+aeGRAF057fns1ZnJmzzKtxqVTWVyK1orJgbVEq8v6ck/2SZdfFUaE5M0tY2q"
    L"4qgEWLmlSi5Dw8pOIciN/EoBWxbVSY0TD2C37dhl7Kwef2Kz/ei+h3SVf9A2PPSoPaEFYMeOnbql"
    L"dwuyM973Ny7NRUud1kKIPMkvO+n/NjjuuLV28voTtd0/0fitvOUrluuqP2Z8lukc82R/6dKlvVhw"
    L"Sk2Drs4ViQ+t+v2stJ7S94ZSB0jgDBIbPid7m7L1pz7L4NHFP2PXprQtmXpmURGBokeRJZx00knI"
    L"Ey+f6r2mte0D04c6kf6xVxkH/RoYPb3/hgK7P4TwH5ukX6+kP9dOVtIv1+sJdABe3N3cG4ixBHOb"
    L"7cMvSZx/3rl22qnrbcWyJVqlXau2W9SKHZWQQfde1v5wXvQET4Ohifh0BWKluARwdXMSVTJpNRLN"
    L"NMWlCkmbeNzdgtDsgDxFSXu1OGinLGzpkiW2ZuUKXRmW22o9J1imHUIndizwrxutq4eHFbuBKSWL"
    L"NgozmvwzmuzKSwsqmz4z07XeMkwZzwq27Z6w7btnbPueKdu9p2t7ZDel+kmeMyjDZoD6p2ity1VS"
    L"D34r+QS1fNHvrvqk/FISSYtDFbV0aunXahtUUqg0aStN1pRg8ttVHYlYaYFBLnUNj7xJXqsgV/LZ"
    L"FSotBrLSoSoNmRpQm7V8qiCbWrGpJN86DDFfXXRkoENq2OCxtlpKdVrQZCfapS3FwOI5NV3ZhO7R"
    L"2UVt0e3TRj1TeVy3VQ8/tklb+Y260nN7tUkPYbfrbc2MzWicTONjcq/DqmDGORSx6JGCxaKwzqIx"
    L"I5lX6cJy4nHrtM0/wbjar9XObvnypdbRBSbo3PPr32eccYbO7xqNbnO4u7l7Kri7kXxKqDQujFs3"
    L"j6sovHJDcRFbgywjobGbHHjAR5k6kHl8YId/AF9rIDOefPJJ2t+poHLiZ6pOW4LagjIU6TmBbmEo"
    L"p1sD2TUdMkv013/919GzYZ+9Knhnr/ug55x04ol+yvqTbaUGFBdb9HBuix7gbdGDlz26J57RiR10"
    L"RksJ+nJ3c98bEliCue3vo7FoVPTwUlPKmABMvCTnS9CRdDI1kkKSVNZsSYOpsiFQud+sLhkKjRoz"
    L"MTG4SBBc5dqCriJMlJUrltlxa1bY2tXLbNXycU2wji3Rg0OtExZ1GXKfMdfkthmzSm1XBMhKIHTF"
    L"d9VuLXlaJKQzReLrWcHUVK0JVNt0FYXSpuuOTXdLWXWsjoV50TGPgqiFjkXJTAukh6gYg2J0cwvq"
    L"lmtYGpgk5m5mKouqF6pTSe2byuLM9Y+iwpJtSLEjM33cXRGbbNJoi0qoxCN++mYYyj49c4EyntRL"
    L"s6pkW5npzscmlOB71M8J9ZEd0K6Jrm3Zscee2rbbNm/ZaRu3bNcVfodt0VzatnWnbdsxoVvCPel1"
    L"K3/nLzUj/zNdfj6/Mo9uCtQsKl6NQfDCYllap1xki8bGbcXS5bZm1ap0T0/C87bphBPW2erVK218"
    L"fLEVOlmu893Rju60006z/GTf9HF3c2+gopGch3qfT5JPaTcD8AcF8ID6weRP81QnJVNiATt37rxH"
    L"tFy8eHFHNApFD/Bx06ZNrnLI2LBhg2sRCPm2QHLqRcyuuOIKzljie1/9OhwkGdt+Mf+KldRCMJJ8"
    L"SvfBtU502enY2NgiDWiZHozt3LXLNunebJu2ZZu3bk8ndLse0uwUJvTAxqzv3/IHSYK+3N3cn4YK"
    L"lmCuySlWE6sZECYl0MT0HlVdOmrJMiRIPVQZLRUNKhP5U03DSGZqoQe1j55qzYObO4imbwvi0Qoh"
    L"GpNn2dIltmrFEluzYqlxi7BkqcZDk6tM+87Kut61qqqs1ha/q6sdYWizYGRVUNkRBDM5R6Qrqgt1"
    L"4kmqyoL4aBY6WgTGzMtxC+ViK8aWWuysEF1hY4tX2djS1Ta+bLUtWrHalqxYa0tXCvDLj0vlxeKX"
    L"LV9ly1assuUr1tjSVWts2crVtnTlGlsiugx9+OWrbfGyNbZo2UpbtHSVdcaXWzm+zDqLllooxlP7"
    L"HhdZVQiKqfLSZkzoFpoX0fbMRNs1HWzntNuOycp27Kls1+5p27lzKt367NTOZ4d2Pdv18HRiYko7"
    L"nyntgCZtShePST1fmZqZsWlBJ8c0xBbSeAerNYYaCNNAGLlfakg6GuMoplOWtky3aiuWLdUVfJVe"
    L"4a20E45fKxxn69YeZytXrrBFixZZjIUFOY0xiAY7tffLOjFG+W0Odzf3BiQoV33OX0a3d7XPlKQd"
    L"BmwBiQ0dxKR2AwA59vjHJxQ0c7xugup9U//QQw9xu1FIVO7evZvAi8WLFxdLlixBRjksX768UN9K"
    L"6aSynnMEFgGVPeN5z3te6PEihhy6F1BIAgX4SQ2gj42N9TVDiGkQgyTubp1yzBSIsbqy3Vq6dJnx"
    L"U0vQxbqfH1+ieyoPen22y57ass0e37jJntq8RQvEDmuemE/q/ezeHTZlIZEBAlEzZnwJbp4mh7uj"
    L"JjQLRC0bcip5UpWKqpNUwrTtFVXJ0hVFtvCmj1uQR/mQzF2lYConUzHBgnx5+qrFN5VBeii5B3ON"
    L"x9hYaYsXdWz5kkXCmK1eOmYrxjs23nGLRW2arxbUvmkmd43vSuFVqUQ8lXYNtRYFY7In3szNTK3r"
    L"m8PNXJDEQzTXWwovSyVmxwolZTG2xMaUqGOLV9jYklVaFJTcSugly9fY0hXH2bJV62z52nW2Aqw+"
    L"XvQEW7HmeFu+6gRbvvoEW7b6RFu28nhbokVjXIvAuBaUsWUrDX9eLrUgWFxs0ynpo814sAntvSe1"
    L"uO2eqmy3rvDayFh6ZSp+Rn2YnNF5FZ2hT/SW/gtubtSbZB7NTP2J6ptr1xBCMNMqSdmkq9rmKKLm"
    L"WWmLdIVfsnhJml9r9XzmhLWr7Xjd1x+vJ/k8r1m5Yrnm4rjFohCiFUrwQnwIbu5uPPQ7TVf9KLn1"
    L"PrQJ3F3DX6XtPglHIlaKHVDOFB4oNyyDZIcnqQeRkx0KqEcXZD/4zqBdQnN3iIahth16oEzhlltu"
    L"uUu07KFgIdi1a1dcvHgxsrh9+3Z/8MEHMdRAWnjiiSecRUD6lJHb17/+9ZQiks16oGxc/TVQl2oB"
    L"SEFs374jvVu99tpr7D3veY/99u/8J/tP/+k/2R+/7o/sT//0jfY37/kbe+/73meXX365XXfddXb3"
    L"3Xfbo48+apuV7BrZdGK0Whn3/Eu0SHAV1VmxnboibNV93xObnrLHnnjSHnv8SXtcT3Q3S0YdD4PM"
    L"iN3Tzo/ErZU1UGcSea1aN+M7RW6KV7AGImLqRKQuKl6Ty6XPYONHGxqVpMa3Cu7Bgk5AukqbNFX2"
    L"BDN3Nz7UJ9prk3IM0Zi8TKiyjJq00RYpWcdKF2/WCV3rVNNW2LSF7rTVM9Nm2tpWKqegmXDqT2XE"
    L"YlogeowIy4WlDlRGBCmMYFYVqkwSS7XqHYI+gnQoKBeNxafulZEl4CwxT38xGdm5VLqt03AYC5S+"
    L"kkKlZwnNfXgTB8Ku4sa/7mpMLKIGlVluzjV+jAsVZSwtkP2MVwzmAlf0Usk6PrbIxrS7HNc9/PJl"
    L"i23l8iW2avliW71qma3SbmvF0sW2XLuvZYsXpzk1NtaxoE6WWhCj7KN8lumKHywq0TnHxx13nGkr"
    L"nHYDtO/uycbdzd2NT05O9AHJyThAwYx2J21kGclPUmf7wfJgXfaBPaAN2gPEkZHL0I0bN2pcqydU"
    L"VwqF0BEyH7UQBJWRF0uXLo3iw4oVKxLVIqBiOuho+3kAZWv/oJDaSjKcsd34HxpUJ+xuNJgAABAA"
    L"SURBVFH5s0lf+9pX7a1//df28Y9/3L5x9ze0DS511R/Xw60JJflT9v3vfd/u/sY3TKuUXX3VVfau"
    L"d77Tfv/3f9/+4A9+3/7oj//I3vbXb7MvfOEL9sUvfDEtED/4/vdtm14BKhdtbFwnnROvnUbZ6eik"
    L"BN0DztiOnbv0EGij/fi+DXr6e7/d98AD9uhjT9jGpzbbVt1m8FCtmxYDVw+VyuLJAtcXEgl7h0o6"
    L"NB+VZ9JTltCuGmrqKaheh1n6Mn2kFCiIyp+RCSq6uzlyb6hbMHf4YPJs1Lmbuf6FGCxINwqFEruA"
    L"6jQFryzoFiHU0+bVlPnMlC58E7qC7rHuxG6rJneqvFtrwy6bnpJ8co91tU3uasHodmd0bmZU39Wk"
    L"6GoRqazmCT8ZqAhsrg8DMLR+oKJGKfBlct7Q3jf9oR/m1AOzEIJ6K2r0F1or+UKDQnXBVF+bum9F"
    L"DILbmOSLdGVfpMVxcSfa4rFoS8ZLGxsLmleljS8Sxoq0s1o8VurqX6quY2PaaZVKdu7lSfCgtguV"
    L"oaX8FWVhjLvps3LlyvRKT0lh7i6JGXrunsrunu7z83ZfCaAxrTS+3aHIyQvNiT6l+/s2nxcC5PAA"
    L"Hh3sur1bCRIf0CYgOHdP8Zk+xJnlDz/8MHHdK7Fmj+67zKL4sgdkgJ1A3LlzZ5Dc+QE6XWwdvgcR"
    L"Mz0P4OyCVB72hQMG6F+OKSEZ5Ace2GCfvPpqm56eMm0p7MwzzrTzzz/fLr74WXbJJZckPOc5z7Fn"
    L"i+fvmZ919tl22umn21lnnaUr/mptqybtu/fck3YHH/3oR+0f/uEf7N1/8zf2zne9y/7sz95sf/zf"
    L"/9je/9732k033Wz33nuv7dLzBAYHFEWRTnwnrfSFMZjbtm+zJ3Ur8aDui37wwx/ZD3/0I3tA74SR"
    L"sV2a1lP52lgGNH9TD2sxtSSWYHwYmkw9JHmdEshN58H4EmeciIa6fAgmuPQT3NyByuaa4MH4Z0lm"
    L"ppyHNTd90pdJpji0mLgqo9oLus67tr3KevPupMgum9ECML1nu01P7LDJXVtsYtdWm9i+xfbs3GK7"
    L"d2yxXeJ379hsu7cK2zer/JTtlGyX+N07nmr0dm6zPbt22O7dO21izy6bzNCOqynvtunJ3ZILWnQm"
    L"hempPZLtse7UpE2z8ExPWK2dSlcLVFfU0m3KjPrQVZ+66kylmdgVZiz6tGbmtEUtRqBQ36L2HEGg"
    L"3NGtUBm7VuhBaaGdUNQCWIoq97UgmJXRjQepRQxWaKWI0BjNxQNTiwxhiNGoiyEYcwOkcpKTB55u"
    L"Ec7Qk30lgPFxd3N3C7Jxb/hpLagkJ3OMRAM5OTMlYYdhSkkP8AEdBH4BcnSyD/zC0yagzQz3Ji7T"
    L"hzizHBtyQon9LVV1BBKfjmZQjpJH2QRRFxJVG4kqZ339+vXIgaqb49d//dddO4C6KRljlHiMGKzj"
    L"GVw6otsB27Z9u/3Td/7JPve5z9n73vd+e8tb3mJvetObtf3/U3vzm99sf/VXf2Uf/MAH7Ytf/KJ9"
    L"5zvfMX7tkgWEJ7Fna0HIuPCCC+3ss862lStXafewJyU7CX33N79pV155pf3lX/6l/d7v/Z795m/+"
    L"pr3+9a/XjuMy+/a3v23b9E59ShNzQsiDGkK0GIKmmmmi77FN2hmwEPzgBz9Kv/yxYYN2DI8+rucN"
    L"29PCkW4d6KKGgYs+rAbNlNmCMQAJpk/QCTFjEZGyPf1pJMlSwqbOXVSHBTcPZu6iYpwyvGKUNMn1"
    L"Basm8WSi8iUdcepHrWpsVae9NMkHZqYntSjstqndO2xipxYDLQIk+o5tm2zHlidt21MbBfVz4yO2"
    L"5YlH7KnHH0rY9PiDtumRB23jIw/Yk48+jY2SpfJjDxo6Tz32sD21UXjykeRn6+bHbOe2J22X/O/a"
    L"9pRNauGZ2LnZJtXupBamyd3bbEaoJrZbPb3T6pndNjO1S+vD7sTXuv/XVsZq3d5oRTMWuFq3Pq7F"
    L"wdmpqG/K99RX+q0emwqCmbkOjYeI8QkezHWemYtc+aNHi7rSR10Yorb6UYlfFqVBmW+n6R6fn+IL"
    L"aczNoO5u7g2UFOmqT2Jx7gEyypnCA5I1I885knoQ5AgyKIBHH2Q/mdIGoF36597EBZ9jpQ4d2n78"
    L"8cfTMwAtArdKhwUAjIkvegiiLABhz549YXx8PPGSuXYBlag98cQTtXYRtXggko76ir3fBPTrgl79"
    L"/VcGHMiJPfroY/bDH/5Q9FFdzSc0qG4hBiNIOqmGbbsWiMcefywl680332xXX32VffCDH7QPf/jD"
    L"6Yr/yas/abfeeqv98Ec/1NZ+R7qqs0KfdPLJxq7hwgsvTJR7NU6iVqzU1l1f/rK9971/q9uJ37Pf"
    L"/d3f1WLzZ/Z+LUDXXHMNv1psPB1lsdm1a6dN6w2FmcbDzaanpm2nbiH4/QJ+DfSHP77Pvv+DH9r9"
    L"WhQee+xx27Z1m3SmjIVA6qYZYlBGAVj61Enm7ubB0yRzD+Zm5u7mpk+PukoAi4AMqDpNYHjZNXIz"
    L"d2kGtyCqQrMImBtHbfrwRZ0FM03kRg+hoWJ8WMwInu1/pduIGe3OpvWEeUpX8sk9u22CK/+ubb0d"
    L"w1bbsW2L7di6xbZvfcq2belh80bbBlhEtmxU/SbVb1LyPyVslu1W283uY88Om5rYadPTu6ybbkem"
    L"rGJX0LslqdR2Xc9Yre1trQRvYJa6YC4mHeqnWSpKVDPwACXthiCmryJEdTlorIMVJHgZRQUlfYiq"
    L"E5CzGBSSMUejZPyyDg/54N1drvYFW32Sk7YBSckchgISro0sY45j107udrnNo5uR7aGAtmgXmD7u"
    L"TYxB55h+SJQO6omD9n6k3a34B7UAbFQlV3sQxRcaAxYB+KBy0PM61wLg4tOh5wDIE5+/9BYgs4O0"
    L"b4fRKwiIwLRy2MMPP6SE2do7MdHQ9PRt5i5uAKYP55ZO79y5kxXIHnn0EWMRYRG4WrcTf/d3f5cW"
    L"h8s/cXl6NvDtb38nrXTqVdod6HWGXXTRRXbpc3RrcfElxn+cePY5Z2urGO2JJ5+0L2th+NhlH7O/"
    L"ftvb7F26lfjYxy4zHlDefffXbYOeFYD7N9yffHJCiGVGE3SXkmOLkv+Rxx6zH997v3ELcZ8WhUfZ"
    L"KUjOoOtSzGGuE6MOqt+6Iiv/ODHm6pwHcxcjuOmfyibUyUr1kqVv2bs7rLmLBhdtbCmqYMCT3O3p"
    L"xcIkRtdS0lQMpm4bzBSHB4i5bCx/qHP0BcmD4C7edK6UHC6TEKL8e/JHmOLkrXHg4pSDKtTiRPh2"
    L"UcHdTUeCmetwvgVvfJk+UnBJOFQyFc3c0sfdTYe5SiGEFIO7m4t3BdbIghI+WlSM7m5JBq9+ICvU"
    L"hxAK04RPz56YmzHJQnqyT/Ijs97HXf57QETiA84fIBGZD8OgZNvr6T7JDZhDUMAcgQJ4AI8OGPRL"
    L"e7RLLMC9ia/pZ9P3LEcXH/jkonqvbol1gbtF9SQ+COJBlJ6Lj3pOF0VdffTNmzfX4tOhizc7AMp9"
    L"DLwFQJ509dXncX4xwbm7HvBtNr1aMFNTZVmYuxhge3/c3dI/aBs6icgbf7jWvNGEZkDAtm1b7f77"
    L"77c77rg93QL83Uc+op3DB+xjH/uY3XDDDekKP60rzVJ+Im/1GjtVWzz+A8WLL77Ynv3sS+zcc8+1"
    L"Jap76qmnjDcPV111dboVuUzPGS6//BPGMwcWnNu0+/inf/one+ThR7Qz2JlOMieKpOW99Ha9anni"
    L"yY1aPB6yH/74XuOXS9JOYds226P31k53Q2j6CC84ZfVPQnNlUMj9Njd4+gcklsTUcbPEu/wAs6QX"
    L"jLKrDpjp2xo9t+AmuMqCCkluZm766IuzJpLK1Jm4WjAKqtAhUTDTswYTccUckx+XipB5qOpMcanC"
    L"WIyADM35EvpUOu5uIhZkF1w8kKY7vHokin1TF8yDm6pNjs3d9NGXDuw9qN5CWgBi76pOQnMx4GoP"
    L"Ze4F6QEZ653/WuMioclPMcHd5dv7vBIkbfc5ByQW4Jy30U74Nk9CA3xAB0GCAuToYAvFN3xuC0r7"
    L"gMDc3egDC1iGuxZ25QR2+GNHzbOsB3QhU1Lv0by+zsyKATQdlVA2tQhHpR1AovraJ/klq1/5yleK"
    L"cAmAmL3+9a/v+1GMiQ+qWktwdIZVSCuQgo5CMNeJdJ0Idzc64i4qGB+Zu3s6CdQBlcxch+Smdt3d"
    L"3BuYPu4N7+4q6egRBuD7elPwuc9/Pi0GH/7whxO9WrcSN998s/FbUBN79qQHPuvXn5IeSj7rWc/S"
    L"onCxbiXOsnXHH29Lly23iT0TxjbqpltusU9/+tPJB750/2M333STfeub39Ii80BaFFhoTFtY4uZk"
    L"TuyZTD+z8PAjj9i9WqQe0oPGJ/RKZuv2HZpYU+Y6ae46eSlsV7+CxkRUY9RclN0kNDM3D2YkhEw0"
    L"DJVpsCV2QUWNi/U+wSULnnTd5Vtwb2QuP0G8a/xNn6YczIM4N3N3C4IO8WYmWZ4ZbsH48M3MSHWq"
    L"R1bn9lGWzD19mYkGg9e3eHe37N/0ceqA5K6yQWHopHgNUOMZ3qgw2eMrWIwgShosBkGJT/JHXdkz"
    L"uPIjC6oHNMFtI7eJLPqU23B3c/d0a6rESU/yScAM5jM8NIOky+Ccwyuh0jOjNs3JDgXUoQ+wAfjM"
    L"FD6d416AxO/uBs39gzd90AP4Im691ks71x/84AfQL2/dunW31ILgAjSKAnjOGuC01lo8uqqjXOtZ"
    L"SKK5LGo333wzMthEdeuNTzXPjEVsOkfuS92bgVTjRmKwIocQ08liEoTgabDdRUOQUXi6LFnjyvoy"
    L"84a3OT7u3vdjvY/MEqcIdQ86rdd/W9LPI+RbCXYKn/zk1ekNwve+9z3dHjyhh4sTNj6+2E484QTj"
    L"toGF4QK9teCKsXzFciXvpN17331262232Wc+82m77LLL0u3I1VfKz8232Le+9S17+MEHbYfuo7t6"
    L"/+vu1inHzN0VQzctFvwyyv3SefiRR23Tpk3GDmKSn3jUsLp7vx8kvaexCkqG2oLGyt2TLxHj4+4i"
    L"wIw3mVJMVK56eoyvm8uPFNPh7hYoi5oH6TUQw2FuJrg5deZmQQd8DBaVZEGyKKFLRozB3TwEcxNc"
    L"vJl4M75UNHdPvIkYH5U9uLn3INtAOcgeSMflK4gP0glBJVcdvChlF3XkxBOCJd0QjaSPkmUg51Ue"
    L"z4aWL1/eLJ5m5t5ru0dNHxKI5CTRAYmYKTwJ2kaWkXzYtZO7XW7z6GZkX/jJ7UCZr0AhpX65u5FD"
    L"uU9Q6tDBB3EredNDcS5+3Horlgnd6n5MukG6wEWZFqASD9r8XmXNS8qg7t37o5uRrv7/+l//axYM"
    L"xrJ2trFyGhSo2ozNKtib1GzFOBFmblIUgpB5txDF6yRSZ/pA3e1pHXNz3xf4TOjVBVHTB4rc3Q0a"
    L"PBgfFdMESDK119UTZRape+/9senhpd104016FnCtrvafsk998pPprYS2UL23Eots/cnr025qKobo"
    L"AAAQAElEQVThYu0WtPrZKetP1S5ieVo0fvDDH2iFvEkPMK+2j/7939sH3v9++/jll6dbka99/WvG"
    L"3y7Yvm1run0odDvEdqssS+Wr6zZh0jbpNoRfTWXHwE6B31ab0gISrNfv4CZWcHN3MyWICypYqpLM"
    L"1Sdz01GbyTMTpM5UqwNS3W2oTkp8q5C49CWBDtdYubu5nAbvUQtmHsxdZXMLMZqrrRhMvJsKFt0t"
    L"hAbUuYv3YO7eA3yGm5mbqkzfgvU/WUb8SagdgQ4zKuhAEGuuhShY1NP8EIKFUJjmXUIMUXXR+PBk"
    L"n3t8Xc3M3RH1aSr0vnICkXwZJOUwkHAZJDLJDdq8kq+Z/3rlBw+yDnrYD/PdCycRdzf6Bo0a7wzK"
    L"6bxqUPBD7Fz1eV4GmM/cdm/cuPHabdu27aIdOQwCySyid6s2HMuWLSOh0ctgxGvd+1OGxz7h9a9/"
    L"PbLEt78CAQK9PrBHH3vUOElM+BiCTlQDD55ORO6gmVtwoa3jwdwHZfuW3eWrZycDSz5Do+euOiHE"
    L"XO61jwwbUfdGJw+q6cPA7tq92x7Twz7u/W+88Ub71Kc+mZ4zXHvttXpe8M3012LGFy82JthFF12o"
    L"24dn2wUXXGDsFDSQun3YYz/Sm4MbbrhRi8JVxvMJXoGy6+CHmr5y11fT7cW2rVvTdpPFYNnyZbZ4"
    L"yVLjlnuHHoDS/oYHHrLH9DpnGw8ZJyZ7/aMfLl5Q/Fp+jY/rC5i5uXsaU3GWPm5JZqLGomD61OI0"
    L"kXQ0EspaKNKZlZAx0YqZ6mrd3tQ9WaIKUiITMXS6aFHfGKuEGIdQtcVBvWqaRUlyyWo5kZY4U2gp"
    L"OBghGB/XeQKB/sRgMUQLMVoUgs4rW/2ybMoRWQjpgR/n5QTt4txd4eUWbK8PSUkC5aSHkjBtMBeG"
    L"AVtAQkMHQdID5OjgE5p9Uaa9jJqxEdzd3N2C+kF/8sJG2d2ND7b4JnaSf9euXenqzwJwv243Jfv+"
    L"Lbfc8inpNgZmM+I5M9OiJDnlQTqj3UOWU4c+aA9e5jOVu70PnSdPHZjS1f/RRx9NT97pSIgxdSrE"
    L"aDHEZrWG7yHEkOyiaAg60TFaFNy9sZMMeUJP7u7Jxt2Nj3swdxcCRVHxsjMTlRxbDw1vvY+79zgz"
    L"94Z398S7u/GplBScOAaa7dU3v3m3XXfdF1NiX3nVleKv56ekdIuxzRYvXmJsNy/Qq8lLLr3ULnrW"
    L"hVoUTjNuH/boLcKP9Er0Jj0/uFqvOv9eO4X3vvd9dtnHL7PP63nFt+7+ZvPGJAZju8qPoa5avco6"
    L"Y2PpVoo/ZvHggw/Zo3q1yl8c3qkTX2kXEzyY06+EYAFqwSQ0Pu6Sufqi06Y5phSUNPMSayPQZGNi"
    L"jNvvRsf4SEHErfknYgn4VxPMkKZs6TM1PWkbn9xkGzZsMOLjh7i+8Y2v23e/+129Rt2gnc4mm9Er"
    L"V8Lx2s1gTB94SC8uVwRO3OpLDIpf55z5ANIFReUylsYFJqje3c3djdd5J510Upo7JJVc7nOQQCQP"
    L"lPHLyGVoBuc9Y3p6Ou3gSOpBkJDIoAAefYA9ZXzCQzPaMbo3fYjqG4kPBbl/xIk/fO3RMywlurXB"
    L"mKv85O233/436nRO4hnxgDKY6pWRsSAA5G1wWoHOhk5EMzvgZZrK0KHQlGjkrPJ0lhPE1S3qQQ2d"
    L"KnqdK1TOnYPG0Gzjot7fhqgTrpMaRWOMBkIqR0vUQ5IhbyNoslCfoMGEusJx9zQ5xFpKFtmnOsmh"
    L"AD8ue3Qy3N2ow6/1Pk1Z9+TKJA22bdq4Sa8of6A3EXekJOYB4eWXX2H8rAE/BPXUpqd0Reqk2wcW"
    L"BX7ikZ+EXL9+vRaLxVq5d9q3v/VtLSLX2Yc/8pH0TIEHjjqJKWlYcHbt2KX+lrZi5Qpbe9xxxj0t"
    L"cezcuVuvWR+xBx960B5//Ak99Nmp5Jpu+qrYmyTyXuSW5PTF3RPv6m+AD2bmpo++dIjRIUZ1nHWn"
    L"UryE/cN7XJBtUH0q64vF6f4N95u2oBqXHxrJWJYd3Sbt0a7pKXtAr03Z2UjVZGbubm5mIsYnBDf6"
    L"FkIUdVMp0SCFmOeCBwshmLvqBdOHxZKrPnONpAIS73OQ+CQQyQRIxEzhmbNtZFlOvHZy4yeX2zy6"
    L"GfiCxw+grQxiBO5NP+hfyhHd2sDnPqKT/dAe8w5wQYLysJ2/Y6gr+MM33XTTm3UbsE0dJ8EzplUm"
    L"8duYlAw5yHrd008/ncQHterbUHH/R8gqixYtsk6nY50xQZSfuEqd00mERiU6neSEUWZBAMiiFgNo"
    L"6Om0Tz5yD24MDnBvJgM8oD5RBlGTBD7EaFnujm0z4O5PU9NHJQshCiHpu0siqMpcvtx9rzpODCcz"
    L"06o7oxV5l23atNE4IV/96leU2NenZwpXX311+pmFu7V7YFHo6KpOcvBa8tnPfrZB1x23LiU0yf8R"
    L"LQZ/8zd/Yx/+8IftHz72D/bZz37W7rzzTuNh5aPaWbHd4/Xm6jVrbOXKVcZCu3PnDnv0scdtQy/J"
    L"+EGmCf6nGsVtLbAImps+6Ss9NFQhrfOuinTWtcBp7yxxLVIJolr8G7HK2nkwSyrtjvhZA+Sqln5z"
    L"vPCFL0w/kfniF7/YXvGKV9gv/dIv2W/91m/Zr/zKr9iKFcubOwcZ9LzKSC0rRn2bu5vpHLvGPAiR"
    L"eaDzCd+GjIwn+9x28UyF8wCQD4LE56pZEXcPJOQwkGwZJC/JDdo8iYgMwAN4dEC2H/RP+zlGd7eo"
    L"uenu1uRAkcrI6Ke7G/r4xT99IOFJfPoCHtFbJmH6ySefvO4zn/nM/1LybzHjN8TStn9SPCDxM23z"
    L"OfmhaQewYcMGaJoCsq11r58e+InPh2dG/dDLrDohywIBqyJd3ZYtW5qufiR5p+yok9GKsrTc2TLJ"
    L"mk6jE3WiqeOejnJ/t9CzYWAAOtQ1fDQoCCGmyRN7g6pCStiggXR38SBI7AlBkyvXuTcyskBcmvDu"
    L"bnzQazhLD3dIPgafk0tfmz6bNYnA2Fmyx5Y/TMFJ2/TUpvQzC9/4+jfshhtvsM9+5jP2GSX1Lbfc"
    L"qiv9PdoJ7NLVfa096+KL7DnPea5x+3DyySenLeeDDz5oWtntAx/4QPptyg996ENpp3DLrbem33/g"
    L"GcGEXll2tNBy67BMT7vh9+hVJD+fwFsLdhIbNz5pO/QasquFyt3Ng6Ag6Z+ZW/okUpv3qJkrTU0f"
    L"pWotkpKnVgKroOTXd1MvhrHIixpjskG3Aci4Om/bvi31n2cbq1atbrzKhmZcJWsaNCjxRMUWPFrU"
    L"TjFwnlpwdz18XWYk/rJly9JY044N+ZA8JA7xZHS73fTcJdOcrIMUW0BCQwdBUgLk6GDfptl/bhea"
    L"43T3FK27G/M5as7STygV6OEP34A+kPjMJXhd7dN4KuGf0NP+/+fmm2++QnYkL8lMsrcxoXya0pxA"
    L"xgKQgS7ADrCm66ykU5qoFgAWAXi5T0ef996T/0ypDQROR9VgujJ1lLyLdLUrO6WVRceQl50eLQsr"
    L"tbIjK/q01IAI1MkW+TBwxctyBq0ZvGCZp0w95RiDhTSBouaX93UUuEmgw1M9ZfSsOTfGBxkngsHX"
    L"U1Vd4Xcb/cv9rJUQ8Er5JMcmo5E3JZdTykwKTij+NuvJP28g7r77G3b99dfrDcQX7K4v32Vc4Ttl"
    L"x9avP0U7g/ONV5G8iuGXp44//ni9TpxOJ/8Tn/i4velNb7K/efe70wPKG2+8OT2LeGDDBm23N6nh"
    L"2tiJLdbDSlP7E3qIyK6AP3n9wAMP2uPaLWzTgsCkDUo4rricevdgZq4DaulTq+xKe137VXYlnXpc"
    L"N6BQY6htAOeTpJSSPaVXnPw8xg++/z2N2x7bvGWzbRUqJaBUpdL4MXcOY+xjaM5REC2KkGQhNFQG"
    L"ekU7rnFZn678lBlT6CAY54mJiZTonK8M5PDQDM5vBmMBzzkaRE52KKAefYBNpvDZN5QYATG6e79P"
    L"T8/P2J+Tpg/x4Qv/tEPSM1+g9ElXfBNmNm7ceKtuF//XQw89tEFmJPWE6B4BCvgZALBH/nbLHzKA"
    L"7rT0wIxoTn4WAKAzywlVzdPHMJnOmyPXFGge4qQFgM4y8diedZTsbHehaRFQmUlC5xPK0lgAyqKh"
    L"nY4WBWQqF71FgXr4jKjVss9LJ5ef1ovJJxOHuhBif9CjbN29Xw69yeXudCYhyzRgxv0VgKdf7m58"
    L"3D3p6osiA5AoX+hBQeb7VOPa55UFnOzuTDdd6bdt22q8dbj+huvtE5d/wq644vLebcM3049EL9YD"
    L"xjPOOMN4lsBryEsvvTT9yDNjyy3HF75wrXHb8Gd//uf2wQ9+SLcf19k3v/lNYwfBTzsygdw9LQqM"
    L"A7//sG3btrTgbLh/Q6Jbtm6xfNsgVVMvLXgQVbKLWsOlb0l0sCTUqf/MBHHiJVbfKJs+iaqsrqsO"
    L"DQnlAf95rPs0hpQMxBda54Zzu17PTdhNuLNwJK842gckCeeLsQUkYabwJGgbWaYkSTs8kg77wTKy"
    L"dh31AF+Z4gvQHuBcQwnSXaMp0C/mL32CAvemT9kPfeB8AXablFkEeMqv+fjUXXfd9VfaFX5cfkli"
    L"ruwkNhSwCOxWHbQNdHLyz6h+tsQfHNzBskz3Pry3G+jfAtApHlbxLpaOkvxM1LGyYx0tAm2UpZKe"
    L"HYISvyiahQBZp+xoN6C6sky009Mpe+VCyc+tAGUmDEAWQrTQmzzQqKQH8Ao01UEBMuA9fU4YA03C"
    L"aKDThEAG6LLrC77O218mNjLNbvxRJ7aZ6LlOVIe0mgM9uKQrhpSAZyJ19e4fzOiJMz9F+ZC2/9/+"
    L"1rfsBu0Q+PFkHjBe/8Uvpgds2Jxzzrl2ySWX2qVaDJ773Oem15E8ZMTXV77yFb2puNp4jsAPLOlq"
    L"kZ4jbNiwwUj86emplGycp8VLllhw18O6CT3D2KSHdRvSa9CtWpSmFRN1rqSNQWMraiAt+kxc00dz"
    L"RJ3U0fB8q0CMIio1h7u8CCG42gvmIaTzEXvnKNN0TtxTfCeeeGJ6uk8d/myWD0lCspBwGSTjMDA+"
    L"GdMaa5IbtPmc7MjhATw6INtD223QNnFmtPvC/AT0BTlAD3t84p+kpy9Q5iI8u0Jd9bual3d+8pOf"
    L"/BMt6vdpGEh2kjrTXZLlxM+UBQCdadXl5IfPC0BeBHQCmbnS2vtAvrdkSEl9cMT9HYC7p6TNCTrW"
    L"GbO0GCiJO1oA4KEk79hY88AQPsvKsmNl5+nkpw5ZQdILlOGj7hGhIA8qMgYWihxd6qKeMUCRQdFx"
    L"9zQBc8LpYYpxj88JUaf6icwocGL54SF4klaVxgc9V0JAKYttiHwnJn3JgkyQMXoAMQsJFLjLixLC"
    L"3S35EKn0npxYZqpu2vpv1RX6wYcesm/efbd96lOf0nOB99tnPv3p9GvUTJgTTzzJ+HkEFgX+zgI8"
    L"zxLcXQ8YHzYeMPJAUQ+M0u6CMj/uzFV/TA9uV6xcaWvWrNXziONsfNFiLYDTeqK/Kf0sgu43bXJK"
    L"p1ly8QAAEABJREFUcy1EMw4lsbub9YJNfVL/0rjQV9NOQHBHx8zdhSbpSXyPbsyPqOTnXAB3dBqs"
    L"W7fOiJ3zh29gQz4kDUnC+clgzNogSYcBW5CTD74Nkh4gQwcfgzS3k9smTkCouU95zmVKn6nHJvuk"
    L"HRYwziOgT9DeVX+bFvS33nDDDX8vOxKYpAYkOMmewSIAkAOdMMvIC0BO/kq+0hnrUZH+keV9wVyM"
    L"5x1AVpKgudLr/r8zxlW/tP4ioCs4JxV0tBgA+Az04QvtBgole1TiUmbQKPfBzqHsGHLq+3L0sRXl"
    L"BAR3CzFID0RNQhcfLSjZGPDHH3/cSHz4fOKs9alZGHsTOonFowfMk0RzXloMmYpJ3qMkuNRTW+5u"
    L"7k9DKvKMXQMmA5Mp2eMLSMndm0wSz5H09OyBicMEefiRh9NW//Ofv8Y++vcftY/+3UfTjoF+cT/e"
    L"3iXw69Nc8XmIxP35bfxI82c/ozcVn9EziGvszi99KT1fYCxW6LXjiSeeYKeddmq6Ai9aNK4FYcq2"
    L"bH7Ktm7eart1j008jK+7YkyQRGwNn1g3F2Ws3d3cg0VREj//hCh1Ge5uq1evNl7pLdKCxFgAudjn"
    L"YKxIEihjkpHL0AzGKiMnMEk9CJIQGRTAow+wb9PsG0rbxAkI1J2+utEv5idgfkLd2TXV6ZkRPmmH"
    L"fnAuM5DxulRX/ko7wa/pqv+6Bx544F4zy0kPzUmfKQmPHArgAYkPBhO/vQDIdf/ozbx+ed5MkCYN"
    L"Gp3lnpV7e050RzuAzliZFgWuNB0tAmO68jc8C0THxrRYAGxK1QMWg44WCfxRjkrqxGthQI+JxKAC"
    L"5Bkx7wxiNNOAxxAtiueEVNq+aytlGtC03eWkmj7urm/lmjKWEwmUoRIkcfpCpurEpy8NFbLEo6xK"
    L"ysBM/jj0nKRSwlrrQz2xuEtBcnc3dzd9GR/q5SotLJTRhYIQGj13l7oj0u5gRpiyPbt3p7+ZsOGB"
    L"DcbvPLDtv/LKK/Q84IvGL4hMTE6mh2i8emSHwG9ELl2yNN3/f1kPIK/+5NX20Y9+1D7xiU+ktxR3"
    L"3HFH+uUp/C5fvkxP3k+z008/w3gYyflgi7pFuxJ2Te6ueMz0LYiqTKyueN3d6APlqHPX8KEnC+bu"
    L"6YefSHwWqKb/tc32IWFITsYV5CSEAhKrjSzjXGNHgkEHy8jaddQDfGUKjz9A2yDH6+79PjE+zMdM"
    L"6bN7k/zY5rZYbBlHKP2C56qvRXqHrvpvu+666z5segElkFtczfeI3z0ArvoAOXqApGe3AHLyd2VH"
    L"4gOxTFpIAgMOUuFgvrgFmGAw3FnJV9mi9Df7xmwRya1FYEzJnJO/1BWcBC5Ey17C5wEry06zWMim"
    L"0ykTX2rilNLFvpB+iDEldSmeRYYEL5T4DHTsLRTo4ZM6BlhPTPWO/l5jO0ucdBIK2icSufe+3D1N"
    L"0FRMPJw1yaksdfckcHMz8e6iRn0eYxV00IbUxVmaJLTn7jLxJKMezp1vRHWqQ86EgSI1c/2zvT6h"
    L"b2Pm7mpcM2ZqKj3E5BUk/eYXle647XbTNtK++tWv6ZbgET0lr4wHaxdedGF6jsAtw/KVy+zRxx+x"
    L"m2++0T704Q/ae9/3XvvYZZelNw3sGJ7U60TOyXq9pTj3vHPtjDPPNLbrJAbjvmh83DgfwYN5cINy"
    L"TjgPAD7D3dMr4/V6wLdy5Urj83Q/Ke0NEoTzyNhlMDbDQDwZJC8JB9p8Tnbk8AAeHZDtM83t5LaJ"
    L"Fbh7Gnd3N/rGfGvD3dN8wQ9+aQeQ7Fz16RNlzpN2brWe8N9x9dVX/5Ge1/zQmqs+CQ1I8DZIerBb"
    L"OybqM0h+0E5+JmQtf328/vWvd5U5kEEPGOp/9sFfsK430UkG6Li1uo8cX5wmA1f2jhaBqCTusAhw"
    L"W6AyyVt2yvTUHp66stNJ9/9MFhYIEj8NZs+2IOGV6MjRb+qioR9CtKiFIS8E9IarPVc/DaZeRzF2"
    L"SJUjykbiVAcaQf72hmlGpE4njq08a6VMmkp9u/cUxfePQVkyaOs1XkMIacL07cS4Nf/Emrt4wdzM"
    L"XV/WfLAGTYnvpk5RmivZ3Juyibi7pXZUYOJyXnbt3mWaYPa9e+7RIvAVu+22W9NPMX7vnu8Zv4C0"
    L"fNly4wp/UfqDKpfYsy99ti1bvtTuf+A+u/mWm+zvPvoR+8AHP2CXXfZxu+aaz9vDeh7Bgsw7+edc"
    L"eqmxIKxZs8ZWrFpp/Bgzu4uy0+mdm5DiSTEpNuYE9/j8sg6yfc6DPf0hKUl+zlcGfWqD/g0DtoDE"
    L"gw6CxAPI0cHHIEWW28rt5+jc3dw99a2ZgyH1l3kITB/6hg/80g+Svp38PHDuzc+tN9544//WE/7L"
    L"ZEYCc8UnqYdd9ZnMyMGExhA9gN2wxK9+/dd/XW6ZyRAzLQB5UWgEB/Htvft/TIMKj9BRBmn5yuW2"
    L"cuVKJXNpRVlYUZTa5jeTgStER4k8tmhMyd+xMV3pWQAKJTl/2on6qKs4C0aIUTryobqyKI1BRY5+"
    L"4lUPDTGkOnww0AwoVz09PU33rpwEggTEB82gDijbWRlElGYkrwg6qc4Tx1cfJJ4MUjmpykZjkMp8"
    L"JT7ZWZokqWymK692YsnAn5Y7npKwL3NzaVuaXDCuLyDSxPj0ucQYcQLtZITeYqOTY8joS1cPFZmI"
    L"XHl0tUmvH2+95Ra79ppr7YbrbrB/+vZ3tSDssjWr1qQ/5MofUbnkkmfb+lNOti1bn7Ivf/VLduVV"
    L"V+i143vsne98p+ke1W67/Xa9OXg8beXP0uvK9etPtjVr12h3cHySEQfjzo6DBYNf1uG8ETAxQQdB"
    L"0hEnFNuMXIZmMO8yOP/wJPUgcrJDAfXoA2wGafaf227HyngC+kZfmHvMSygydLHPfhnvNmiL+alz"
    L"UOue/3qN4//ctGnTExqHlPjySUKT4CT7bvmFcsXPoEz9hBYVdEl8MCMfmmTpN/9IclBfccUVUFWl"
    L"o5lsiW2+FG+eXo1gHt/YAFSDvu6kwwwWzwBWrlxhJGuhxC17i0BHiV90Ci0IhZVK8lSmDrmSvDOm"
    L"JId2ypT4ZU9eSObBLS0OIabJHGNs/KgOOSsp79Lv1lNyDWR60KKY+ocC7SWO8kXJ2q/oMYyIezBO"
    L"qitxxJi5mQWBwz0V3V0lN3fv08SqlA4VmABSMLbA7uilGolcMk/U3MydLzMxOtSQisSZcpuArInV"
    L"XRXi3YdTVe11uDd6yZdq3L3frrt49c/d03hwzvZM7EnPD3ggyo8c33bLbfb5z11j119/g333O/fY"
    L"rl27bYmeF5x11tnGgsD9Olcxxvv973+/vfnNb7Y3v+lN9u53v9t4IPnII4/aiuUr7KSTTtCDxNOM"
    L"qz1/7Zk/yOHetGtzfEh8kpO5BIgxU3iSqo0sI6mwayd3u9zm0c3AFzw0A5+AdkEO1911rtw4x3l+"
    L"QpmPyBhz9LGlPSWnxm9X2oHC0zfm6oYNG2zPnj3bdNX/X7fddtunzNITexKZpAYkeE72XYprp3SQ"
    L"AerRZbEAJD7IiQ9lBiXoau+y5UhlmEF462pOnfqx14/6ImuD+nZZfQ+fo+NgyeJxW6vbAAaG5GW7"
    L"Do1KVq7kDV+mBJahxRATH7UoYJNkSvAoNLpFehYQYrCoW4AYYzoBCsJ4NXbXXXeln4RjUhJUCNLr"
    L"6ahjiIQgWHPyVGe9D7oZiFxfjJKIee8fPEnZl7uZe7CnfZs+CG2vRCM+VSQ9d3EJ+tIRksDM9S+K"
    L"D3pg6K5SRmh4+ure8Phzd2vL3Ju6dh+s93F3cUo4fRO7u5t7A4l6fDBsXe1JkBbOST3lZ5I2C8I9"
    L"9qU77rTbb73dvv61b9gTjz2pxbmjh4Knpp9U5KEi9/Esuvz157f+9VvtL/7iL4xXjlzhxnS7x5Wf"
    L"mGkTuDtkH5AcSooUA/MIkEjDoIRIP0QFJXlJNtDm80KAHB7AowOwHQRt0W5GHnOCTeOk2OkL8zKD"
    L"MvXZBt+5L/Qng7abH83eWHPV173+6zRuT8iWZAYkNtilOEh+kh0K4AF6gMRny88VH5D0gCs9pzvR"
    L"geRXU/M7XAsCmENb1a6XPk5bFp7//OffrgGoFbgFJfKpp55inHwSmgFisOBLLQLwhRKZAS3Lwooe"
    L"TzmGoHJhUTL01IrxPMDMNfGlK98MLq+yeFLNE31OYlRSY59t8olDBmJ0g+KP6RdCtBAjl1gz+TZ3"
    L"HS7WE3V3yx93t6AEcQ/mifbq3M3dTV8c5vxT2d0tfUTdxafDqTV3N32Z80+8uxLUzczlW7IQRHtt"
    L"uKuutVtxR9H2+qAfpK+OmLsbZfeGmj7UeZBPydwbe3fv64k1PsGDBRWSvfQTL1m3WxnjvWP7dnvw"
    L"gQfTInDnl+7Uc4Sv8mOpmNoZZ5xu/FITP6W4YsWKtBizK/jQhz5sW7Zs0XmLpgdVKT73JoZk2Psi"
    L"MWhD86ef/MyjNjjHw4AtIOmggyDpAXJ08DFIkYHcHnEwf3rhJeLuKX7mV0YaK8mTgr6wpy36sqf1"
    L"a7uU9WQ/vX3SLmC7rvr/u3fVJ4FJZkDik+A52XeVZUk5g3r0AHaAq/5eyc+9vpJe0TSH+Ao0pYX7"
    L"dncWGHaRaacQcK1Be5yBrnWfydPh5cuWWR4sdcbSgJF4mmByoImhSdcrRyU89cA9mNcacOlhnyZj"
    L"cNuop9Bc7b/2ta+l13j4pB5EJTMUe0CZmADlNtzN3Gtz/Qsxmst3cE8J8DQfDBv8uEszwWQhnm+H"
    L"t/SJodF1d3MXJMU2BPHAGkqSqSq1I5G56kIwsar3BkHUeh93l658uyBF90aH3Yh7jzc+ri8396ch"
    L"gRGDOzJr8Z70qHdveH3vVZ/sWu25q32NEzacX1796QqW/gwai/ANN9yY/rQ7P2XIj4Fzi8A7fX4K"
    L"kR9RZvJzbqqBV6IkDMkBpS4jl6EZJGhGTmBiGQQJiAwK4NEH2A9SZLSR24ZqHtPVBMYCMA+Yb4C+"
    L"IAPunp7r0A7IiQ8FtMdOaOPGjbXG7Abd6/+xrvqPyzmJnEGSk/h7wd0pU0fyc8UHJD7YK/Hlrwbc"
    L"6yvh4VVkpkAWFhofb3tUnM3/Hle73zUzM20zumowAXjYw9WbwWOwoDE2k4mBDCGap8kVLIqPmmQg"
    L"RLcQkQXjBPHHNZloXPUZZGw7nY6pYYsxJoQgffGcHOqzPJbRkEVvKHwpveDBeOiIXohNnbmZSx5D"
    L"VOK5EAy/wVVh1vAhmLkOyVwM9e7e6MZg7pJmWE8umyCftZu5ex+GwBob1ViIwXi46EE6wCUVjUVM"
    L"NiH5CYYe/YjaDQXVq9Kok2uxbvQNuOqQJ16V7m4xxqST6lR2cwsxmIiFEMzdjQ88djFEyT3J3Xs0"
    L"BDM3m+nOcB+bFuMHtDvgj3/w8JVbB35Nd92649OPJCMnXp4BWO+Tk1OTKV31u91un8Jz3tvIMhKK"
    L"OZDtB8uDddQDfGUKn4Ff0E58dzd3V79DArG355R7U0/s+CQWFjJuQXWFT/f7e7QDYKFkhypZvup/"
    L"0sxI3vJG+oEAABAASURBVMHEJ8kBCQ/g+SUeKLokPsCWqz5bfcBVGJDwQO7TAQ9SYaG/nKvngFPN"
    L"CLPo3b9MAzs9nbb/PPEt9CAvxpgmXogNLUTlxEIMkoc0YfEXmFhiYojpwcm3v/1t+9KXvmSPPvII"
    L"Ww3jRKADsOekIAPwyGKMRj0UWRn1YDEEi73fMHPTPyVO1K2IR/FuitslNeNhYiHdJI/B3BWMDpcs"
    L"yq97lO9oMSVktJB9StGVbFK1EIKZyiEGCx7Eurnq3N1ikCzDcx0ytyAdZb/0kUvmQogmgfEqMgTk"
    L"bpm6wwseU/wxBNVFIZgathCpCzIXTXXUt8ouHnmEWtJzZPLr7sbHFVOtnVKQnrtbn5pbDFHfnmQh"
    L"RuM2jiTatbN53fj4Y4/b7j27zN2NH0pid8D5cPd0S4FuRlfJPwxpLs3MGJREI7lBmyf5kAF4AI8O"
    L"wHYYctuZWu/j7k2fQrCofhEzgAfubnzwSTu0t0fJDlgEAD51tdeOddarPld0knu3fEF3iQL4DHQm"
    L"JG8n/uBVf7bkl9mROby3GASae/7zX3z7xMTkzIQeInHreuopp6bf3yZBpWjN0KHZIHgySwUmFwPH"
    L"1YMtPsnPgyhsAVd8TkSm8JwQ6gA8dVDK1BdKUHgWobITLYZonbGOFUHPGLQwBJVjhI/m4tErymgh"
    L"BsM2yB6gg5+ohDD1grgLLQL4CzEa1FUHNQvmkrn0kHkIhr67JEJQ2d0tRMmBeHfZCDFGC6qPIRh8"
    L"DMEKyeCRA3f8BHN34+MxWIixgfSDgI275OIpu7tB94JTLzt0HBotxmDuLhothmhBfQohmPcofIyy"
    L"6yGqXXf0JZOOu3iNi7sbbxa2b9tu6Fx33XXG2xnGEHDlBJzvduKTVMNAogESGjoIkhAgRwcfgxRZ"
    L"Rm6TGID1Pu6e+u/uOv+FMYeIPwM1bPFDW8xzXd3TLghKmat+7wl/vupfLTuu3CQzILEBCb9L40HC"
    L"J1568NShlxMfW676OfnbSd++ysMDuTnyR8hNdrvTd7AVmp6eSu+C+dFRBtC9GdysB2Xw3d2mtWNg"
    L"q/T1r389PShhYmhgjITmJPAwET7LMk8dMgAfNFnhAXyMnTQBaT+E0spOqXKwosPJjdZRuShKK8rS"
    L"WBhI1BhLK6UbY2Fl0bG0g4jBglCU0UohhmjOvxDMNfGDKLYgxmDsIgIyIYZgMUaj7NJ1d4MHMQRr"
    L"bORNvLs/rZv8u3RVF6Wn+siuxYPFECyAnt/EUxZyW5lSF2OjH1v6rlhCQrBCfqN03MwK6XiQvsrB"
    L"o8UQLXivHIJFjUsQDYoVGqVPvaOT5MHc3ZB3yjL9HAY7G/6qEUmKDUkEOM/QDBIrgzkBT6INAj/I"
    L"oAAefYDNIEUGaK8N5p/p4+5GXICxYC5B6QNwd0MXW3xztQckPZTER/7oo4+mq77oDVdddRVP+LnX"
    L"J5FJaECCt5N9t+KijJzEB+hhM5j4+9vy29H8hNz41NTM63bs3GEMTojBLr300vQEmEFFh4EE8Dwc"
    L"4r6e+0Y9JEkngQFn8EnyDMrYU4YHlFkYoBmU0eFNw1jZ0cSONtbpWEcTMWiSB13Ri6KUPCiRoVEJ"
    L"HjTBXZPWjHiZzEET2T2YJ7gh9yAaCotB+jFajIWFEGUbzCWLkrl0oEHlolNYlE5RFlboqogshmhR"
    L"eoB23N1iqkPXLcvcxeMrRMkEeOm5mexp3y2G0EMUjRZisCAZcOmH5KORRcWBvI1C40Hb6JqbUefJ"
    L"PloM2EVr+0z18mn6wAfF5u7mHixG6comhmAhBnNH7uKj8UdiqWcB4DxzjngWQDJp8qftPQsAfKYk"
    L"E0ndTu52uc2jm4EPeOgg8A2Ye8C9FyMx95DnURqbGFO/TB9ixS+JTsID5jeUWLi9efDBB/nx60d6"
    L"T/i518+JDCXBc6IPveovXrwYvZz4g8k/eNXPV3qoupMeJinS+R8y8vlrD9fMPqAhq/zCL/zC7d2p"
    L"6a160mn8Z5s8EeY9MCeeicBgkuz8EAn/Uw8DyYAD6jkJ8G0gA51OJyUuPP6g6CEHlLmSw4+NL0q/"
    L"hxA10UOMWgQK6xQdaxKyo5NLYpZWdEotEqAj30Knk3TYLXQWdQxf7AKKUFoMmuDy58EtKiELTfYi"
    L"RIvyH8TTfik5ddK0EN2CBzMLFkOwIBAP9UVZyLd89vTxg49YRmMBo1/ou2ygXEVjCBZjsBBjA8rE"
    L"ItBOTPJgRaLSUX3IkMzdjXKMRaKJD9FiD7QZo/wHZMEivBCD+CCZfEQhqOzuqpdMi6p7wyMHUf6z"
    L"Hn3lV5v5c/Ff/vKXjTHilSDJyFyA5mQlyUgo0ObzQoAcHsCjA7L9IMU3bWSYPpqs/QXK3dM40G/i"
    L"gjbxx6SDLj5pr538JD5lYuAJ/1NPPVU99NBDn/7sZz/7Z5r3T5ilH+ohmbmik/wAPi0C8pmo9JAB"
    L"fpoP/cHEX7Crvvriaq9/eO/evS84CCb7gAbZ00DC7omJv928ebPt2LE9DfBLXvKStAtggLlHYsXk"
    L"GQGTJA88NCWbrtbISXD0szzzlOEz0EOGTbLnCj82ZjEGTbaOUZ/QWWQdyZOO2sAGH2XZaeTlmBUk"
    L"o+wKyQotFoHECsFiJ1qnU1pH9SRXZ6xMumVRGAsFdgXtqkzCkswxRCVikfofZFeqzY78xhCSrChL"
    L"TbJowaXnhYWoxaAsLKocYqGy6mK0KBQq05Yr2UKIsml8eHAL+FO7EXkMlqj4KLtAWbYuvRiCYi4s"
    L"KpYgPsbGDzQG8YH6aB6DRSHIxkO0GAoLUXQWhIA+9cFcfMAm4quwmGyKtH3WBOz/7QLGwvRRIqQd"
    L"AJRkAiQ0dBAkIUCODjaDFFkbeQGg7Qx317g3ID5iAQVjmOKNiszSGwnaAiQ8FypA4tMuz6eU9Omq"
    L"f9NNN71BO5wvhhC4V+dKTlKDnPjQnPSZUo9uTvy9kv95z3teurormDZVkUtBAnyCzyOZ56OTnB3A"
    L"l8Y0/QwAJoGvjNe85jX/fdfOnTP33nu/HpDsNh4G/ot//ou2eHyx8ccj+c2zM888w9atW5ceEnIC"
    L"NHi9CROVbB2jjJyTlE8OFHS4SuuEwaOTUfTkpU5kp+xY0Sm1CJTJX0pgycoialHoaEHqiI6lNkMs"
    L"LMRgY+PjVsqmo0Qk4TpFJ9mnHUCMFoqiKRel0XZRlhZ7suDBUqIik25RRukW0pOufEfpJRu1zwJh"
    L"enMaJS9U9ujSCxYD+oUhDzGKFrpFKayAlz1xRPgEySWjHCK60UIAQTRYlF/6QJul+gcNQfIQLfYW"
    L"kuQXW/lp6qPa6vktC8M+RvVL9UE0yD4jxpgSKYaY2guiUbJC7UJDkJ3KMYa0AHQ6HWObzK+7BtVx"
    L"znKykmSDyMkOBdSTeAC7QYoM5KTPV31N0tS+EiDFC42Ki/YBPH2HEhf6+KFNkJMfSpl281Vfz60+"
    L"pav+n2/atIl7/Sm1SVJnkPSAhAfwINe3k5+FIz/kS1f9M888s73tt96HxaDHzp+oTz5/7QPS7PsN"
    L"LTOEvmv37r984IEN9uijj6SBP+Gkk+y1r32tXXDRhcaDQf581Qte8IL0t+14Xchvki1dulTJOpYm"
    L"VD4hTBz4NuWEAU4gtFBClmVHyRKVwB356FihCdcpy8THWFrjo1TSj4tXUhagUHnMxtAVSk2MsfTL"
    L"SYXs0FWddg1sWTuqB4WSgUSHp/2yLKwoYu92I4pXWe2iF6N4+UQ/JZuuqmNji1JilUpCdEAp/QT5"
    L"Ic4YonSiFR1BerEoLMoPcvRBCFE6pdoTqJNtakN8qo+hsRENMSYeeUx8z5/4kOqlG4QY09gH8SCG"
    L"aH2fsbEhzuwHSjlmu9j4yOWcbCGENAfY/XHrR1n3vCkxSWxAYkFJMijIPHUZJCc8dBjyAqBJ35+S"
    L"tJdjIt4M4gfEiTL+8E27XOm54gN45Fz1WcREH+Gqf9ddd10nu8keSGySGkqiAxIfwGeggw1XfNBO"
    L"/n7S8wM98ttO+Davqvkf6t9B287WisZXbr3SV/IdpOht/PZv//br9DZg6+23f8me2Pikjeue/LTT"
    L"T7NXvuIVpucExi+HkFj8wBB/0+7lL3+5QfmxUhYDHhSNKfk4cZwkTiJUDWrSK7GUFJSbk1lYWRZK"
    L"2k6asB0lKxOX+kITd2ysTHXNFS0atNQCgF6nQ11pY7Ip5bPsdJQshZVlVDtBdqWoJr/qOoqnVLIW"
    L"5Zh0YpLjvyMfpdBRXVDClmVH9qWVhSBZRz5TLOJT/GXHCsXLjoL+gdy/Ujr4SvoklpDrQwyGPOko"
    L"PuRRiQ9FRh2IsbAiKGao4i4UU5RtjNFAIVrIDl1ihI/Soy4DnYCN+tCXSScombHLMsrwyPCV+STv"
    L"6SPT1dFIIp4D7Nixwzi/pg+JjhyaQQICytQBknMYSHiAf6CJmRYWuU6LDuMNiKfTOw/EQ7wAOTb4"
    L"pj2SnaTPoEydnuwb9/oPPPAAV/18r08Ck9AkPcjJDgW7FUcG9SQ+mJK8nfjpqi9ZfwEQnw8SDOTy"
    L"nFR96W/L51Q8xEqN6V4xhZ4/FwWUg7ZGr9X9fn3tNZ9Pf3aKh37jY+N2/nnn26te9Srj55ZfoQWB"
    L"h4SccHYA/Ew5bw74k9j8kgkThYWCCZ5PHDQDeTqxMabJXZaddBXrKFk7nPBSyaCJiF6h5CJZO2Md"
    L"S1B9TBO8sIiOyh10ZFuWY1aknUOR/DJZotrAz1insI504aFRekVqp7RF2kEUSi6SoZReFI9dlP9C"
    L"vrHhlqOMHT1TKI1yRoGO0OiXFkNhZdGxIEo7nY5kioFJW/T0CiU39m1ZWWInqD18FUVpAB3iaGTB"
    L"QmzGrNBCgQzAF/Jdlh3ZFIYsQf0gjmxfyjd61EEpwwNvLRLIqdeEsY7GjL/PsGXLFiMWTdb0mpDE"
    L"A8wBAJ+TfpCSjIMg8QH+gOZgSn7aAMQAiC1T5Njgi/bY4gMSH5oTn8Wqfa/fu+rnxM/Jn5OcpG8D"
    L"OYmPHhiW/O2kJ6kAXYAC+HlD41yDeRschKLG2LNZ5oMEWQgF4S1vecuXdbL/+VNPbb79nu99f+bm"
    L"m2+2L3zxC3b3N+/Ws4E9xpWeZP/FX/xF+3f/7t/Zz/38z5vufYxfKGFRoO5lL3uZXXLJJelXSlkM"
    L"ABOJSVVoogdtq0HZaSYsJxgUTFgPVpYxTWTKhSZ8UZAMhYUYjMnc0UTGHzZlpyP90sbKMtFOWghU"
    L"FkWnAXWFEkO0p4+8SXjJCkH2RQ/4pb4z1rFO2UmxlKVi6kSVe7pKOGJBN4QgHdWXhWhpoVMocQq1"
    L"F4UixVWo3U5Zqh55Kbn08REbWmhRKFIZX4X0Gl38I2/QSXYxahy0ABRANkWn8Rfl62kEKzWeJXFL"
    L"jn3wYCHGhH5ZsWNTSI4MUAYhRFuk2x+SSxeFlKDUk+AkPSARKQMSc5AiA1zxAQmcoYlofDT5k+/Q"
    L"i6XT6aj/hdF3EBUbddjhi3aJCZD8ICd/66qfn/Cne321QzKT2CR4BokPnyn1AN1hib+/q76aeWYe"
    L"GuP+wpT50AvcmZmhAAAQAElEQVTVRdsI73rXu+5605ve9P9645/+6Uka0Hf+6Ec/evK2226zK6+8"
    L"0vj10R/+8IdpIq5du9aeddFF9opXvNxe9epX2Qtf8EJbu3ZNekrML5jwd+xe/epXG88NzjjjjLRI"
    L"jC8at44mVVGUxkltTq6nE86ELjWZ09VW9Z2yY2WnY8jRYzKUxdMTA91I4mjiIC/QVT23IYVopywt"
    L"yeUnKlnGOoV8RStUtxek15FtVMLQFu2ketnkMvVs0Zu60oqitLJUMssm1anMg0Jkpfz14xVfCJ2i"
    L"sBDVtvgSXfFZp1BdRKa6olB/KVMfpC++FGKvjO8mpkJjUyqOwkhe6oMWVvoZU1ylhaA6/KofjHXR"
    L"UVl+eMAYQ7So2JM/+S+EqLq8SFAGIeqBoCaIttFGPYs5CdhO/NmSnmRtgwQGcpcOTUTFGBLwTSwA"
    L"nrah6GADaJO2SfyMnPg8qxi46n9RjXDVJ5FJarBbsjZIfICMejBb8leyJYnakCg93UcGf0whKFrv"
    L"QcQyjxxECcNb3/rWP3vjG9/4wi984Qs/98CDD1z1T9/5zg7x9pGPfCT9vDj/Web01Iydftrpdulz"
    L"LrV/9s9+0X71V19j/D9zLAJcOUhIHiCyGDz/+c+388491/ilI/5rrI4mfUdb8BBCmshMgLLQxNYk"
    L"dsmQx05UskmmicykiIlqMkuv0e1YlJ8yotdJE6rUhCbxQEd1xFB2OtYBZSf564hnogHa7ZSNHD6j"
    L"6NBOYRHfY2XyTUyl4is7pZWFUJaWdwul+EKJ2JEvbCJxyBa+6PFFsu1Yp9MxZNiQlPiNMViI0XjW"
    L"kPTK0pKPRKMVncLKsmNFLBSTxqwAkqs++2FBAJSj/MVC9Wo7hijb0ihHlQvJo9rKwKYo5TcE+Y6G"
    L"HgmoG/T0V4iidFkASESSHrQTvM1ztc8gebnaQ633wS8IaqtQLJ1Ox6DEDE9bqGJDOyQ6V3pA8lNm"
    L"MaANfoZ/4F6fq/6k7ElmkhqQ5ICEz6BMHUAXGxaNfe715au9AKiYjmMu8XUePEWuryDkA2EGnYKn"
    L"DopeuP322x99+9ve/j//9I1vfNltX7rttzZs2HDrPffcM3XLrbfY5675nPGz4/zXWZ1OYfx5Kf4K"
    L"zat/4dX2b//f/9Z4WHjiiSfazl0706TnB43YHXCrcNbZZ9tx69baypUrjT9QSVIxKZgApSYj4Mpb"
    L"aJIgy5MUeakJSQLGIqaJjU4hPun1EiLxsqWulIwybeRyWhhiaSGQWGWa+Oh1pIsOoDymCYoMW5Kd"
    L"ROkUTfKTjOghg46VY5b0ejZMaHyUZUf9p40itRNjMPSj+hGL0jpqsyyKHo39uqKMFkNQHzvWibIv"
    L"ogXZRhaBomNRtChL6QvSLaJsy8KSX9VF+cz1IdmpXj4idVG+5LtAX3rIckzJj2Smzw9+0Oz6eBNA"
    L"0pGUJHym8INAjwQGmnjy0hw58YmvGRf1SXHQblAs1KOPPxYbEj0n/e7du9MvJuF7yFWfJ/wkMIlM"
    L"Qu9RiyR5Tvg2RU49QBcbEh/k13vtpCcvgFweu1d9jW3ugwV60gNC0CvuRZBnUFF/4fNf+OY73/nO"
    L"P9Rtwi/oGcF/vee799z5rW99a9e1134h/QFKdgj891dLxhfb6tWr7Jxzzraf+Zmfsde+5tfsZ176"
    L"Ujv11NP0hmE83Sqceuqp9pxLLk1vE3hucPZZZ8tmtZGYhSY1E5JJwWRhIoeoycskAfCaoEycGCQX"
    L"z4TqKPFir67hczJEJVEz2cqioejFMioxCyORsU/+8NUpjXLSVTnEoCSTnpKnUMIUuvrin2RHBzv4"
    L"WKLTAP8hBINSTxudXK+dT/KvfpbJp2IqyqaNXnJ2VMcCOCbdQjqu3UVZNL7LqP7Id/IBX0pedrSA"
    L"dKzUQkF7HcnKorQYiCFIXjR9kt9kJ1+ZFvJRRPSiBeljD9WkMe3+DNrR2Oakz5REBSRlBkmfYb0P"
    L"9vjLY5HaVRy0A48cHXzgj+Qn8Ul6vZ1Kz6BYDKjTBYgn/NO6NclP+B9XMyQxyUxSA5J8GKgD6GLD"
    L"okHiA+7zAckP2nNfTaTkhx7zCOrBsM4ho+MgDwQU5JWRgUqDdsMNN3zjHe94x+t0m/Daj370o//x"
    L"29/+9vXf+c53tn7xi1+0D37oQ/aZf/ysfevb30mvk5YsWWxnn3OOPfd5z7GX/ezP2itf/sr08wWx"
    L"NwmXLFlip512Wrp9YIdw6vpTbeWKlbZ4fFyTuhQ6afKGqImfUJhmqmEfi2hRsvYEc5WRdTqlMckK"
    L"JUKhtvL2Gr5QgjCpmYBlp5D/oqcrqiQs0I9B8lKLxJiRTB2u8LEjvbHUJjq0g69CbRa9WKBFWVo5"
    L"VlrRKQz/xIqMNgslWRAKtYE8xsI6vRipL2WLnPqgxabxXVqIMcWBPr5oGz/KUEs61JeF4issFkIM"
    L"VqicxkE+i6IjeW+8pJv8B5XRFQrFT/vIWbCidHbu3GF8Fi1alBbunKQkYxs56bmCo09Cg6B+4gfQ"
    L"L9BvQ22igw2+SHy2+CQ+CwAg8WmTW0r+RJcWhHv13v0P5njCv0vtZ7AIwENJfJCTn7nMvAbMceY9"
    L"OdCGXKXERwZ/TELj6+3AQ6tAx0DuPDSDgclgsEh8ypmHsopO6pXhg+9///vf8uY3v/k3/uRP/uRX"
    L"v/Wdb33qe/d874k7br+95i/NXHPNNXb3N+7mt680ITu27vh19qIXvtBe8fKX2/Of9zw7//zz0v8k"
    L"y8ln8p1x5hn24pe82F6u1478F9ynnLLeVq5YYUweJnaIwUiiGIIxsZ4G5aCrXUxy79UzsWNUYqhM"
    L"okTsYzTaCuKpA5SjJiUU0F6QTYyNbozyXcpPT4d6JnNHsrLTkb8yxdjRVXtMZa7GnaKRd5SAZVFa"
    L"lC/syrSYxKRflvLZEeSX+qJFO6orOx0rsC87FpWkRUfJ3dMpJR/rIC+MhS/3OaoddKEhNONRaDGJ"
    L"kheyLYXQ7xtxRCuKUvE9HQe+Nz+12fig+/9n71zA7aqqez/mnGvtE/KC9hZ5CUUqoU2ET4JXLljl"
    L"US22xMvFqzxauPb7rq1SH59g249aMdhS29jKw2DQRMAmIeQBCeHhBwmQV8mDvAhJSXjY8LBIMEge"
    L"5Jy8T/+/uc84LJcnx1B5pGGf7P8eY4455lxrjTnGmHPNtfcOQQoIyCrkYPnjuFB0QQjBaNN9PJ1n"
    L"oWPSJzKAPv2womDsCXgFeJ7xKXvwa7Y3JYAda9asGavN6GvN8g914I8EMyDAewLBT9AD9PBX/NaB"
    L"P3vw4/fEAtAh8qvKZ8F/x7dQ+/hx1EVwYQ6/cDcEFLhxMDQGg2JAgDHdqFCAjEHYMmH8hNFXX331"
    L"n1511VX/d8GCBdc8unLlmmXLl+6Y9eAs++E999jKVStt/Usv2e7OTnvHoYfYse/W6mDoScbtAr91"
    L"zyYhswDPdZl5eNz4v045RU8cPmz5t+wGKhk0cNZkBGxMSZNgkMOljKQyCGaWYswJIWoJjSymaEmz"
    L"eIzJ1MhKOT2OmVJSABQqF5kia8hpcVj4oiwslclymWQiZy4UjEl8iqWxXEeP+qS+AGWQYrRCfSXp"
    L"U99EsoLAlW5RlBbRyX0WRn1RQhtWpNK8r5Ris65Lhh6gfaG+myisUD+l0FDCKNU35ZSPo2uIKfeX"
    L"VF+UOm/RmHVK9V1YqX6iziWJJrXhB06s649ABZ4EfNaHEsxytKyZ26stx+X8GrpOAE+f6NGGfghy"
    L"gh0w5iQBQGJQ0Ft11tcq82EdAD/E1/A5gM9BCfYqXI4uPgvwY4Bv4/fA48CpDpFflDOzv73Frgvi"
    L"Ah1uCAIfYCCAsQBGh2JEgFEBhgcY20G5Q4MP1f7AvTOvHznyr/7u764674c//OEVj61+bJaSwsZZ"
    L"s2bZnDlzbOH8hfb4E4/bli2v5N+lP/roo+0krQpOP/30vAqgfIBuBTZt2mw/U9IgOZw49ET7vTN/"
    L"z9hkZO9g4ID+puMZqwKcr5DzQaNmvJgU6HJo6hNOHZOCzYxymQrx0WIIuZzbSDepDaCMXkxJQVNY"
    L"Ckk5I1hM0agvFDhJfNkgeISuwCyKIvdX5PrCcPxCsmowpqQEgCwHYdLeR8OKsjAC5VVdjpuMdqV0"
    L"qS8UTGWjlF5p6AH6AvB+rJiabZED+k30IcTYrKPPJBuU0g0hWgjJgq6ffkAIwaLK1vVH0BKYAB50"
    L"VWUSQrCkvgDtOReOCw+QkyhoRyLx4N+yZUv+SjpJABk6e5j18TkH/uU+R+A7D6UOoIu/uv/i0wB/"
    L"x8+hHgPW9Vcvd4n3HxJ1KVykSH7BA4zhwDgAYwGCHyNiTIBh3cBQ4IaH8tvo9UFpX7hw4dKbb775"
    L"6hEjRvzJlVde+Yn58+ePXbny0WceXrx4h/YU8v9+s3btv9srmzdb0D8+ZMTsz4eMPvShD9r7ddvA"
    L"x5FxGL6yCuXzB0O1ejjzzDPtg7plOFZ7Df369286YiiyA4eg3mKwFJPFlCwJIahcJMPBY2pS5CDL"
    L"YrSkYEmqaxCkMSnoGhl5o1J16CXVQ2MKFpVwcHSQkEtWlEmBmvIxCwU4ddXAgEeWUmGFQF+0dRSl"
    L"Aj1F9VHkRFAE8erb9Wnjq4+2ttJi1Hmrvtln87iljptStCibot9olFaq3wzZAN1COqUAn2K0JHkI"
    L"0Ux2sq4/D3wCFCAOIUglWNIxAe1L9Y2NoJSRm/4IfPog0JntAbM9ZQKf+k2bNtlzzz2nCWEL9/p/"
    L"qVl/kZpWfQ9fA/hZT6AO/8RPaQfwX/wYuI/j8w4dIr8oZ2Z/ftOo5svjYjEGFMADAh9gLIDxgBsS"
    L"inEBhsbgDgbEAx8KXMaPK6DXXZ46derkb48c+cVvjvjm+ePGjfvc8uXL58yfv+DFGTNmds6YOcOW"
    L"Ll5izz77DM5gUU7J7D9kMB9AOs0IeL9dwGlICNwy/Nqv/Xr+DXxuJ373dz9g79bjRv733RiiRTkr"
    L"/YQQLMbCMEQIrzowdTGlpjPHJB210XGzvIwWYlPX9BdTbOp1BW2QftIKwB0+paSALpsoSktKGEmy"
    L"qP6gGQqyQu0zn5r9eZl+cgApmBJQ26ynII2JvqWv5BLVLirxoBtCskZbWw5ub9+cgUudu+q0eiDQ"
    L"Y6R9ssi56LygzeOqT66hbFiRqDfTJRt/gOAlSD34kYUQLEkX+DE5F3hA36Y/2hLkBHwdyKmvzPrj"
    L"uu718THgfobvOPAth8vwL3Rpg58CfBjg2w78HejM8gse5ML+/hZrF8iFV+FGgpIIAAYEGBR4QsDQ"
    L"AKMDHwAoYGC6B0n3fK/o2F52ik67NhKf1kbit7R38Jl/HPGPn3zooYeuW/7II0sXLlzUPnPGTGOF"
    L"8OSTT+alIk6Fc/EVZT53QDLgS0t8J2HgwAHGc2L2GEgIBw4cmPcNTtVjyPeffHL+YhMfVCoUNIoA"
    L"OW/R5ezRgrw9xWj0H5PKITR50RSTgRib9QQM51DmIE65j5S69KWTkmSqi/SZotoC9ZEKoy7GqDaF"
    L"BdWXWbdU4OpcysIahJt7zQAAEABJREFUBGpRGDopql1XPe1SpN/Ckuqdbwac5PQdOYZ41dM+qsx5"
    L"ttFnozCW/ejn9uo3CYV0kRVKVKXsUhTJKJsFnUMy/+st8Dlnn/VpG2O0EEL+Ge7qcp9ZH5AECHz6"
    L"JIFX7vWZ9RfqmPhZ3beyr6gO33Eeiq/hf+jTDuCj+C7Al0HVz9VNfiHLzP76JhuH6rVFFeoXTRm4"
    L"kaAA4wF4EoAD42JkB4ZnAACDUQeDxKA56mWXd9P77rtv5pgxY/72mmuuueif/vmfPrls2bJxS5Ys"
    L"efa+GfftuOuuu+yhf33IfvSjH+UdY11Pdja+k8D/fvPhj3zEfv/DH8krgQFKAGwm/XT9+pwYuD3g"
    L"tmLoSUPtZBLCMcdo7+FASzEKSf1EIcjxY0YI4hUkBEpMyaBFUXTXBbWLKVlM0ZBTHyWr0hBibodO"
    L"Sk0+JbWJKbeJ6i9V5CGELKc/kLJuUFA2hK4gJljLIveLTkZJf8mS6pp9lkbAJ7XPiIXFpHodC/1S"
    L"7UutLkrRGJOV4pPOJer8gemPj32L5BezPwx1KffTbEPwA9oD6tFTws9fIGKJT8ADeEDw019l1meH"
    L"/zq1w5fwK3wJ4Ev4BT4D4B2UXcfb0db9FN8F+C/+DXSI/IIHubA/v4UengJwvVx8FcgAMqfwwI2I"
    L"IeEdGJpk4MD4gMFgYAAD6BQeMHAOH8xe6R133DH5O9/5zhe1Qrhg0qRJn1u2fNmdWiU8c/fd9+ye"
    L"Nm2azZ07NycEHIuPsEbNxvyWwUknnmTDPjbMPnrWWXbCCcfn7yWwMuDx1oYNG6xPnzb7zaOPsuNP"
    L"OEH1J9jR4tl7IEBw5CRHJ4AxiAxpVVDvSDFZDCHXxxQtxiYKzaYppRyoTmNKFlOULFrs0gsBPlmI"
    L"QfKU4ecAZeMvpZjlMSWLtNOMXyjYk2iMyeALBXCRopXSKctkqRDEI4/qu0mjhRAsqq55fqUhDyGI"
    L"pmYiSM1fB2q0NYw/VlVJ/TjQrwY95RijTN/ZPesT6AQ9ezU+6yPTjGSMQWXW/wvd67PDj98AfAS4"
    L"j0DdP+CpA/gVwOcAfui+ia8C/NfBpQDK0Lcl4h6uGqP0BozpcCOTABwYHx66XcvsTHUsBpRBcsrA"
    L"MYh1+AB3UzkVOt1l9ZV5zRpPT548+fujR4/+0re/fd15eprwtVWrVs1etGDRujunT+9KCPNs7b+v"
    L"ta3bthr/y676yr9w9P73/U8755z/Y8OGDcsBP/DAA7XH0G4bNm7Iq4k+ffrmbzPyHQb2GPia84AB"
    L"zU3FEEIOnBBEFUwhiApR0Mv0mMBygMSkAE0KJs244pMCJ4SgBBElF8SniI5kUWWgwE3SK4sy9xFC"
    L"sw4ZgRVDzHJoLqOvds0AjgpajhWNOq41y2OyBNRvCCGfDzN0o2hYQ7N9qfv9pEBPKapdyPWuTx9c"
    L"F09hCFhm6xijJfVF4DtY9qNr+kPHZ30Cvw5uBbjX54c6tCqrP9d3H8E/AGMP8pireyhlQL3rE/gA"
    L"3wPum/iq+7Oa51e9nIVvt7e4FxfshuqJYtg63OgMQMbLL7/MoIA9JQIGEDCYDGodPEl4RefKwPeK"
    L"pUuXLrv11luvHvXdUZ/VpiIJYfjKlStnzZk7d92U26bsmjL5NuOx45NPPmVbt3bY7t27siPzceRT"
    L"TznFzv34uTbs7GH5F4/69++XbxU2btqY9xtw8EMPPSTvHfz2ccfl7zvwWwgEYgghJ4SYgqWYFP/J"
    L"+Ash5IBSpYEQgiUFTkwx08zHaEFlgidD5RQlC0Ftm3pRZdftpnnWjhZFCeaso5k+91UWapssRPUj"
    L"JOmw6UdbAhZ9+CT9lHWj9KOlmIw9jSgaU2rKRBn8gQMPzDP6Bq2WOE/6AfRFOYQge+7OIMCZ4Znt"
    L"AQmAMquyEAIf4bXnn3++U8G/ik/zadav7vDjC/hA1R8Yd2RVoAfwLYB/4X87zcz9klN3SJxflDPz"
    L"dn+L/0UDYMCegNGRQwGDUQUDAxgowKA5WBUABtThDuDUBx9ncJAYgJd/jiohLFVCuHbMmNGXjBo1"
    L"6vx5c+cOX7FixazZs2evk3zXLRNuyQlBG4/WroSwa+dOKzWj8tXl0/Qo8bzzzrNz/vc5efNwwIAB"
    L"SgQdeXWAQ5eaPbkvfte7jrFjtH/Al5369u1vMSXFerAYY6YhBAtmxiwaw6syUz06Sfo8vgvSCSFY"
    L"CEKuS0ZdCEGqMctdHzmIsbAyNo+X1E8KyYB6sIKgl4yAznXwujZ4+gHwKaqN6nK5iJZol6G+i9L4"
    L"I8iHDBmcEwAf0yUZevAntQ0hdC/5CXLsUwXBT1JgZaBVG8v+jsWLF1+v/Z3vqf/tgo89Y+34ubGU"
    L"jo+/+wdtaAvwK4C/7ZYuwBeBivkFD3Kh9WYWX6MRMB6oNqNchRseWgUD42CgAEnAKYNYBYMLGGwc"
    L"wqk7QZ3WnaWncrsSwjLtG1yrpwyX3PDd716gDcThS5YsnTVjxox148eN3zVhwq02a/Yc44tMOPKu"
    L"XTsN5x806Fg744wz7PwLLrCzzz7buC1o69PH2vMHV7YYuqWChc8mHH7E4XbkO480NiL5bkOKMQdv"
    L"NlqIMrogWQyW5SGETGNKRhDGrnKIzfosQ18gqGPs0o/NfkII5johNOsKBbrLCNAqQmjqhyDdrj5i"
    L"TFbo/KERmc4lCfC7du2yUv3xuw8E8UsvvWTUgRCC8efLfQK9Gvjw2AYd2jHra6d/5e2333752rVr"
    L"10jOmFfHmbFmbBk/qAM52Ko2gHbAfQg/2q060CnqEJtflDPTenvVAnKxVwuvkcOgoKdmyKtgUBwk"
    L"AXgoYOAcDCZgYKtwB2HgHThDFTgKTgPg60DuoC7zJIQpUyZfd9NNN13yve997wKtDIY//PCiWXr2"
    L"vO7GG2/cNW7ceJPMnnqq+ZRh+7atVjZK4xeSP/rRs+yP/viPjV9GGjToOGNWZLm7tWNr3vEmQNhE"
    L"POTQQ/NewsEH/w8bMKCf9RNIKimmHPghhJ+nKeaALkJhQUGYQGzKgtIHiCqHoHZQgXJUYkA3ahan"
    L"DF8ocGOKlmI0X33kckoK+ELHUW+qS9KJXe1pk6L0haIsbPu2bdbngAPssMMPz18C4qfjk9qHELqX"
    L"+wQ5wc71O5CRGOqz/r333jvaLH+G38e1p3FkjBzVetps72rvfoM/OdzvpJJf9XIWtt6aFvhVEkCz"
    L"h+a7GxnalLz6jszBIMFDHbukWgcDSyKogkFn8KvoLRngPDnI1f8ebxFU5zroty9fvnzZlClTrrv5"
    L"5pv/XBuLFyr4hz/wwAPaT5z+9JgxY7aPH3+LzZs7L/9XaDg4y2G+Hz9Yy+OzlBAuvPACO/OMM+23"
    L"3v1bxtKbXe+O9natELYq2MqcJFgV8LNqgATBdxyKPPs2g45AVeyZaXQUk83kICYKijkLmQaLUfpB"
    L"NMSsEyVHlgKBLZmpC9VJy0JUWfUEbrN/lUOwIiZLCmbaOVJIOTkgDzHYtu3b7YgjjrD+/fvnb3QS"
    L"4Oo6JwOu32d95CQBysjRIVn0MOtvUx1g/DrEA+xfHQt4ZNQBdPEBgF/sVDv8xv0Iv3KoKr8oZ6b1"
    L"1rMFYs/iX0mK0auod1atq/IMJANaBYNcB4MPcAScCOAcDncWKMCJqsCx9hrL9Xf33XffNHbs2C/r"
    L"tuEiJYcvzJw5884JEyasGfWdUe233KKEMG+ePfvMM9obaLeOrdtswMABRkL46B/+gZ33yU/aaaef"
    L"rkeK77LDDjs0JwCW0Vs1q0IxDt9vOOigA/NjSZJDEwOs0dZmDRJD0DBlBCOQCdQQAk0t8E91zWBN"
    L"ZsgDJFhMlM2igjiEoKomaI+MNjEm1ceMUhuCMYpnBSH9zIeoWb4zb3ySqLR8VzLbllc4JEACnsAH"
    L"8AT/Xsz6jBljAxib6nhQBtQ50Ge8Af6Aj7i/QDvN8ld1RfKrXs7C1tsvWiD+ouh1l/hgOK0ewGXV"
    L"QXQeChhsB4MPSACgylPGQQAOAzwpOHWHqjoYvANHdL5HqufVzygh3KwNxK/edPNNf/Ktb33rwjvv"
    L"vHOcEgEJYcNtU26zBfMX2H/8+D+0YbjF2js6rH/f/jmA+HTiGWecbqfoacNvHzfI2EBkI41EQPDk"
    L"wOncnQOVFQG/gdC3bz/rp6cRB2i/oa2tj5JCw1KKWScqWIOCOyUFtmgM0VKMFhW8IXTJVJayoQtC"
    L"CJk3C8ZfiF00JAtCStBgMSVDn7ji8xFDhgwxblvWd32IivMF1cAnIYQQbA+zvo8LY1Edh6qdkVOG"
    L"ogdoBxhf9wPobuPkXoWK+YVPZab19sstEH+5yuuuwQBV4QeoynxwoQ4fdKiDBODAQQDOAkgATuFx"
    L"pipwMnc2aDX44fcEdAH10Pb7779/6sSJE7/6L2PH/tk3vzniwlsn3jpSq4XZN4y6YZ2Sw86FDy+0"
    L"FY88kr/Ysn79S0bIsS/AR5dPPfVUGzr0xJwgDjvsMDvkHYcYAbdz53ZtMLbnx5Asp3n+nhSs/EJv"
    L"fy3FBwisHNralBAUdCH32jRlCCEzoUsWlQRCUCmYhaA3sxzcyFMKXcv9IKkpecSsE0KwoON1bN1q"
    L"Rx55pA0aNMgIdr6co8e6madM0HN+bBRyjjzX37x5c97h7+Vev10HA9027CojY1x8nBg/xpQx9jF3"
    L"f3B/UdP8qpezsPXWuwVi79VvSq0PHLR6QMoOBh2+SuHdKaA4icOdBgpwJEAiAO5gUBwOOI8TVlF3"
    L"UsoAHSiA78b8+fNnTZ8+/XqtCr4wYsSIi7VK+OLtU6feMnny5DW6fdi2cNEiW/3YaiWEH+fZMmjm"
    L"5YtLR7zziPw4cfDgIdpkHKx9hHfbIYceoo3DAcYThx07dtqmTZttw4aNtqW9Q8vwHRZC1KqgLf+H"
    L"Hf2UFPoc0GZFI1mh5XzBUj7FbNOg9xCChQBipinzQTWWAz+SKBT03asA9d2h4/iXqPiSFQHOzM9S"
    L"30EHe5j1q7bGxsDtVLcbdYwBbcAO9QsYU8YXMOYAXwBSya8qnwWtt72zQNw7tTdNi4Gsonrgqhwe"
    L"R6gDJwE4TR0kABwK6sDRHDhgHe6sTnHaPaG60eg87bbotuHZWbNmTdX+wRXXXnvtp772ta/90bhx"
    L"466fNGni3Llz5mx45JHlux59dIWtWrnKnn76aQX4BiMI33Hwb9ixxw6y9wx5j73vpKF2ilYLxx//"
    L"HnvnkUfmDblO3S6wyUhSYFbetHGjbd+tGGrssqJfsEb/wsoDCms7oLS2vg0r20or9Hw/pWgRy3a9"
    L"xa7ARxRCgOQEQYBzm8J3KjSrG8HPcUgAzPzcsnR2dma56nua9bGtTsiwK7bYE6hHD33AOAHGEjDO"
    L"nToxh9j8qpezsPW29xbILrD36m+6pg+wU07AeaiXcZA6cBzgiQAepwLIoMCTARTnc+CQ7phQd17n"
    L"SQQucx4KkEMdXoZmLFy48MG77rpr5LXXXfeZ4cOHX3zFFVdcpCXzLQsWLPiRHk1u0d5j56pVq/T4"
    L"8Ul77sfPaqXwsu3W8/jfOHXCEHcAAAjvSURBVPhgGzJ4sJ32wQ/ljzAP+9jH7KT3nWR8TPmggw6y"
    L"Axr9rHNHsC2b223j5k3Wvu0V69i9xbZ1ttuupEts7FYSKKyhPYWybLNGo2E5ASjwU0r5wzx8OlIB"
    L"bS+88ILx+wvHHnts/jWep556ShudrDzUj5mRDHrY4acS2wFsBfI1q0nVHsioQw9gd9oyJjuly3gB"
    L"H1fGG6gqv6p8FrTeXrsF9vUEUL8iBn1vgeO4A0GBy3CwKnA6B06IMwJ4nLMKnNaBE7tT90R9JdBT"
    L"ncu8j/bZs2dPGzt27F9rlfDpq6666iIlh0tnzJhx+4MPPvjk/Q888IpuLTpXLF+upPCUPf/C8zkA"
    L"2UA86qjftA984NT8eYSz/2CYfejU0+34495rJ/7O++x3jh5iv97nYGvr7G/bX9llmze9Yj97+Wf5"
    L"NxkJ4I1aNXAv39HRbgQ95RfXvZgf95188sn5F5m4v1+9enVencBzTPYCpLtp8eLFI5W4RmugsFfd"
    L"TlzbnoANXR87u/0ZJ8BY1cdah8m7/cjhW/gVLRB/xfZvdXMcweHn4uU6xaHcsaCOaiKAxxGd4pgO"
    L"HBy40zrFkQGO7rQe3NVEAA9cp8q7LNOf/OQnz+qJg/YXJ311zOjRf/qNb3zj4quvueZSJYkJ48eP"
    L"f1wVm+bMnt25fPkyW716jT3xxBOmNtZPTw0Ga5XANx5POfUD9vtnnWUfP/fjdvFFn7Jz/vBc+6BW"
    L"D3ySkT2DfnrKUBSF9hN2GvfxwLQPQD2/rXD44YfbsmXL7LHHHsu3BT/96U9JHp06zv3Tpk37m7Vr"
    L"166W4bGLAxsA7AHytUgHSpk6gP1og70BNvcxYayAj6Ga5xflzLTeXh8L/HdPAFUr4Bx1UF+XeRkH"
    L"q8KdD4ozVoGDAk8GUJzXndh5HLsKHL4KggAgg1ZRTwRVHXiwRYH33Ny5c+9Q8A3/wQ9+8Nkrr7zy"
    L"4q9//euX3XDDDbdMmDhxhVYLmx5e9PAuPZmw2bPn2MpHH7V1Wsp3bO2wFKMdedRRxv/LcP7559uX"
    L"L7vMPnPJZw3+E5/4RP5PX88999z83Qd+cq1f3772qNovXLDQ2ABk1tdqYZOOMXzevHlTZVyuG3DN"
    L"2KJdMsB1VSk8OgA92mBDbAqwOWA8fHycqsvWrI8R3gjsTwmgJ/u4EzlFx3moOxy0DhwSVBOB8zit"
    L"A0d2h4YCnBy4w0MJAkc1QOAdngSgwOW9UiWFZxWQd9w+Zco/6O+zn//85z912WWX/b+RI0f+w113"
    L"33nvlNtue2bMmDEvT5o0aYcyhxLD7Px7CWvWrLEXnn/e2vQo8Zhj3mX8wOppp51mQ9/7Xuuv4GfZ"
    L"r35thTYo169f36k9gfuUeP5K/E9kSK7JrxGec+T6oA7KgHqAPvbBZtgPe2Jj4PZnXIAOkV9VPgta"
    L"b6+fBfb3BNCbpdyxoA6cEL5K4QFOCnDanoBDA5wbuKNDHQSAg4BwECQOggce2hPqicF1ndIGvl2b"
    L"iMu///0bfzBq1Ki/vv7667/wla985f+THC6//PJvXHLJJRO11/BvU6dNe2H69Du3KrB3Tp06Lf9f"
    L"jzMfeMAW6FHl4iVLbMmSJXr0uGnj7NmzL9fKY5IMyvl3iIJ8HPF+TKcuRwfQBhtgF2y0U22wJcC2"
    L"2LwKVbdmfYzwRuPtlgDqTtaTfV0HxwSUqxQe4Lw4ch04uAOHdxAADgLCQYDU4QEEJah6Qz0h9KZL"
    L"XfsTTzyxQhuK07WX8Pd6+vDlL33pS5/mswrTpk79i0svvfTTShB/uWjRotsef/zxicINU6dO/dyL"
    L"L774nIzF+ezpeFU5en5NXCfXjR2wC/bCdgA7AmwMdIj8qvJZ0Hp7YyzwdksAdSviaFXU6yl7PY66"
    L"J+DMdeDoAKevgkAABAVwnkCpwwMJWgWB7EAOXw1AeAd1ewJtwRYt99fdc889a7XJt0Ebe2t1+3Dr"
    L"jTfeOEWPK2fKCNX2Wb9LxjHqdR74UL8+rh9bVG3kdnWqLluzPkZ4M/F2TwB1W7szOq3Xe7la73w1"
    L"OeDolKFVEARVEBgkgCoImnoiqJYJLEAgAgIQCqo8ZYAMwAP4nlAPZnSQ1bFZRnCZ60A5JwfX4NfE"
    L"NXLNbgfsgs2cqrv8QpaZ1tubZ4FWAujd1jhlFXXtal2ddweHAgLAKTxBUQfB4vAAIphAPQl4maAj"
    L"sKEAvgqCkzKUwIUCeAdl4OVMdbE9UfSqoG+OCzgnzhVw/lwf1wq49qqN1H33C3l3ocW8eRZoJYDX"
    L"ZmsctSd4L/U6d3poFR4QUIIEVHnKVdSTAsFFkFVB8AECsQ6CFLgcvopqQPfGkxC8nvb0xzGBnwvn"
    L"xrlzPQ6/drdP3V5ebtE32QKtBPD6GNwd22m1V5dBCQQoqPKUCRZkUALI4cHfEyXYgAcflGB0eIBC"
    L"AUFLAEMBPHDeKbKeQD2gL+DH4bicB+fo5811AK6J63NgmypPuYW3yAKtBPDGGN4d3Gn1KC6DenBA"
    L"gQeM85QdHlhOCbY6PAihBGUVHqzQavDCOwjuKlzulLYO75tj+Xn4uXL+gGt0uA0oO9+ib7EFWgng"
    L"zRkAnL4OP3JdXi17EEEdHmROPSFAPRCrlEAlSIHzUA/knmiHTg45gQ8FtHHQF+A4HBf4+fh5+nWo"
    L"q/yql7Ow9fbWWqCVAN46+3tAOPUz8XJP1IPLKUEHD62CgKyCQPWAhdbhge20Xl8t0xegfz8m5+Co"
    L"nnf1mpxv0X3IAq0EsO8MRjVwXgvvgVenHpxVStBWQSD3BnSph4JqXxyPMhRUz9mt6jIvt+g+ZoFW"
    L"AtjHBqSH0/Eg2ltKMNZRDVR4QEAD+L0BfaIH5VzqFJmfPjzwcovuoxZoJYB9dGB+yWkRXHsDD1J0"
    L"4evoUa5jV/WqOvCAeiiQeveLMugWtJh92wKtBLBvj89/9ewIQkB7aE+grhrI6FTL8A7qqqCtY09y"
    L"r2/RfdgCrQSwDw/Oazy11xqI6HMIqKNahu8N1Ta96bXq9mELtBLAPjw4r8OpeZD2RPe2+57aItvb"
    L"9i29fdgCrQSwDw/OG3xqBPEvQMesyyRqvfZXC/wnAAAA//8ue+udAAAABklEQVQDAL7ct/o5sL3u"
    L"AAAAAElFTkSuQmCC";

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

static const WCHAR* USER_PRINTER_ICON_BASE64 [[maybe_unused]] =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACx"
    L"jwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAd3SURBVFhH7VZ5UNTnGV6phDRVkxmVaVKgY6RV"
    L"OqYhscEYG0ElXpFpiREF4kQxUwIZEAin5UYEFPDiFhY5l93lZpdddllgAwws9+kiKWJQoGDllEOu"
    L"p+9vszpqTKYN+aedPDPP7P5+833v87zv973f92P9jP8rSGQV/xBLy+bEUtloSamsvURamisqkQYV"
    L"FAnNisUl67OysjTVQ396kOj7lTUKTD54gLGxMfxzaAi3em+jvfMGahT1DwViyWShQDSYVyhQcrNz"
    L"JdfT0pN8fH2N1NOXj4qvqgW9t/swOzuLmZkZPCAj4+PjGB0dxcjIyGPevz+C4eF7aGppxRdfODSr"
    L"py8PEql0FWU/Mz09jYcPHz42MTU1hYmJCVVFnjTBmBKJSnH06McH1SGWB1m53La5tR0LCwuYn5/H"
    L"3NycyghjgjE1OTn5jIkxuLp6dpiZmb6gDrE8UPY3mdIuLi5+x8TzlqShoQ0ffrjLST19eQgKCtqY"
    L"lMSeZ4SXlpZ+0ASzJNPTM4iNSVgyMXnrJXWIHw+JRLIiJzv7YkJcHB6BMfE8I49MDA6OwOakpVgd"
    L"4r9DdXOXRnVr96/Vj6ymhoZXZKWld5RKpVr+W3yfifHxCbi7e2LnToMD6hD/GQqEpasyuTnnUjnZ"
    L"/2KnZS2lc/PvZ3BzSwsFwvyKigrKalAt/TSeNVFZWYOPPjrU/c47OmvUoX8Ysso6LX5ukX9aJnck"
    L"O0+ICkUr5NWNkMrKkV9YBA43G2kZXKSkc8HPLYBEKkNjUwv6+wdoradU4o9MMOufdC0ZRn96PU4d"
    L"/vshKZO/lFsgdMjg8EezeLmQyRWQVVShSCBEUUEeCvPzkZeXgywOB3w+H1wuF2np6cjI5CCLy0du"
    L"XiEExSXU6xKVqfJyOSrKKxAWFoa/+7od7b47/ku11HfBzyv8C4eXM5SeyYWotBxSeRVS0tIQHxtL"
    L"GSSAzU5CMpuNxMRriI6OQlRUFJKT2Yijjcj8v3TpEiIjIxEeHo5LkRG4evkKMjOzUCIth7i0DMrb"
    L"Q9MNHT2VqZm81WrJbyEUSH7H4fLbUlIzUCgUQULi0TEx8A8IQIC/H0LOBSMsNFSVxfnzYQgJCcHZ"
    L"s0Hw8fFGYGAggoOD4e/vDz8/P3rnA1cXF6Iz2MmpqKhtRvPNb9Bxqx/5RSIE0fgznp56amkWKyQ8"
    L"yCU6Nm4hLZ0DWZmcMovG57afw8bGBvb29jh92hFOTk5woaCPyDw7ODjA1tZWRWdnZxrnADs7O3xi"
    L"bQV3N3eVWG1jB1q6+lDf2oWY6Di4uX4JDw8PxqiOSry5qfmbdF4KvAO94OsbAE/PM9i//yDe3/Fn"
    L"HDp0CEeOWMDK+hNYE48fP06/1rCysoKlpSWOHbPE4cOHVeOs6dnc/K/Yt28vgs+Folxeg4bmG2jt"
    L"ugUxlZ/J2sXpNNzcXOHj7Y3du3cbkvwLrP7+fty9048m2r3VdJ1+VVkNKZWfy81B4NkQHLE4hjff"
    L"NITRu+/CxGQXTTQl7mECUD/vxLZt27B161bo6+vjA1NTJLFToWhspczb0KbsQQ51h4eHG5ypYl9S"
    L"5cJo6bi8LLz26qvGZECTVVNTg7t371B/foy1a9fDyOg9nLJzptbKRmWVAlXVdSiVyREbl4C/2drD"
    L"0PBtrFnzMtatWw9dXT3o6ehBQ0MDxiYmyM8vgELRgOq6RrTc6KLOSIOnuxsJU9k93VUbOJu6Zu/e"
    L"/T0kvo/4IsvAwCBhx44dY+9t34PE6xm4LrqJo04R2LlnH/S3vAWTD/biwsUrEJXIyFCtypRILIXX"
    L"GW9s3mwACoIDBw6iTtEOdtJ1nA0ORW19I1JSU+Dl6UEld8OF8+chFosRGREBbW3tRJpjSvwDUYt1"
    L"b3j4xatXLncrld3IVCziVCYQKJ0CTzkIsbID8TnpMDthAe2Nm6Grb4jP7FzA4+eRCQkEwmKcDwun"
    L"fheDX5cInoCHuvpmcOh88D7jhQDqoFRq4QI6N2iP9GlqarqS6Fbia8SVRBZrYGCAPTs7h9beeVws"
    L"HMXl8mG0TC2i+t4AYquKcZodjP3+XtjiHIkNNjF4wyoM5jYu+OOWN6Cjo6M6ZicnJ8DJS4OirgZC"
    L"QTG1YyCi6IyQy8vpXLi4ROOSSIrJegvxFUb3MW739Jh2dnZW1SrqHg4Oj2F6Abg/M4nKznpc4l2D"
    L"hZ8PdE9eBstCgJcdm+DA6YWkrhuhdCZoa+uo7vjZ+TmMjY7T+itUh1FxsRCSEjEsLI50rly50pZk"
    L"ns76SdCdsWL79u0bjYyM7M3NzXn2dnbdcbExU+IyyVJ1axOENW2IzOuGQ8othBT2QdHZRyUXwdHR"
    L"kTbtb0sCAwP6OzraMUeXDbU06mprcCEsZFFPT+cahd9FNCA+feo9ByuIjLtVxA2rV6/e9emJT6ND"
    L"L0QoObyCSdp4i53Kr/F1bx+1aSUiIiIWaNxa4jri2zTextjYmP3ZKZtGSkRK704w74lM1r8g/iho"
    L"EDdoaWmZbdq0KejUyZNlUVevDsXHx0/Se8bok2DMM1n+hvh74uvEXxF/MjCfT8yHiC7xWfGf8b8M"
    L"FuvfH5HOW7bIygkAAAAASUVORK5CYII=";

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

static const WCHAR* WIN7_NATIVE_WARNING_BASE64 [[maybe_unused]] =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAMAAABg3Am1AAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAX"
    L"cJy6UTwAAAK7UExURQAAAMbJzsHBw8HAu8/R1rOzs7u7vK+vsKqqq77AxcPFypycnZ+flsK6f7u8w7i3uLy+ybWydujiCrSv"
    L"c6KioubZNPz1Bu/rLK2rorOzu7q2Zc3Q2aenqMrL0vvyFKiklre5wcrHXL63UaakjPfpGLWxmp2doNTV2tDMT8GsQdrc4szJ"
    L"xq2rffPrDLeqge7mPJCOTVVWWlNTVF5eaJ6ZQvLnNsPE0cjDdHBwZXp6e3Z2eHJzgYmERe3UG/blJ+Tl7Ozs7mxtcezSJPLa"
    L"J7axpebp7dPLYnh2WGtra2VmcranXO/x+dHS1Ovs8mRkaltdZIuCN+zOLfL0+fDx9dfNVmNjY1paWk1QXO3LGrehRMrGkoB7"
    L"RUNGU+3PIurEFPb4/cG8t/DkREtLUkRERMOlK+Li5dLU4NDJd/HVHEpKSpOGMOvEBreoaIN5Njo7Qzw8PU5MQpyKK/HPF+fL"
    L"G+nBHurJCtyxETY3O+zMEum8Cua7Cq2TQbu4iH5wIxQXIx0dHSkpKjMzNZqCGum5A+OrAbSiiuvcU3xsFgAAABAREiQkJeWz"
    L"AbyXMezKBOfICubCCoZzFQsLC6WFE+ClBa6Yb+Xp8ZJ7E+q3AbqMFsS2W5+DDwwNE6uIBqmPW93g6rmUBR4hI9mcBMm1P9al"
    L"AeOrDaeFO+fBE7OPC+ClCqWWd9/CKOe3CpR2Ci0rIi4vNLWMMMC/xK+wucKWDaSMYZ2YgObFIua9EIpwFAAACqeHJuKoELmK"
    L"G+nEIOq0Gum7GrOQGbCMJOSxHuOvIKSDQ6GYbuzKNe3CL3lnK4t7SNKnKem6NOe5NsKYKrSpmMy5UfXYSvDRTfHNSu/ISbid"
    L"Q5aEQ9eyQe3FTfDJVerDWKaKSfPWU+7RWO3LVu7QV+7MWOe9TbCehaeVfamWf7qoj8CulpGRkoGBgX19gP///woJFMMAAAAB"
    L"dFJOUwBA5thmAAAAAWJLR0ToJtR3AgAAAAd0SU1FB+oIGAwpKkgxCawAAAAldEVYdGRhdGU6Y3JlYXRlADIwMjYtMDgtMjRU"
    L"MTI6NDE6NDIrMDA6MDDGXK7lAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDI2LTA4LTI0VDEyOjQxOjQyKzAwOjAwtwEWWQAAACh0"
    L"RVh0ZGF0ZTp0aW1lc3RhbXAAMjAyNi0wOC0yNFQxMjo0MTo0MiswMDowMOAUN4YAAAPJSURBVEjHY2AYtoCRiZkk9SysbOys"
    L"pGjgYOLk4iZBPQ8XKy8zH/E6+FkFBIWEGUVEiNXAzcgtKiYuwUmsFRxMkoJiYmJS0uxE6pCR5REHapCTlyXOTSKyCopiIKAk"
    L"zUaMFUyssspyQPPF5FRUWYjxt5q6pIYYWIOYppY2YStY2dR15HSFQEBORU+LQ42gD7Qk9HUNDA2NDI1NVEzZ1QlpkGExMxeT"
    L"s7C0tLKytrG1E7bnJ+AoDgdVcTldCysgcLRxsnN2ccVvhZqDrJucnJy7lZWHhydIg5e3Dz4r2Jh9Ve3k7OTcHT08/PwDnJ2d"
    L"A1WD8PlbLdgnRE4FqMHPLzQsHKjBKSIyyB63Bg4frSg7IFCJ9g8zNIoBaoh1jtOLx+1vmfiERBWwBkOjpKTkAOfY2IiIFPUg"
    L"XNGtlpqWDlJvlxGdlJmZnJzlHAHUkJ0TxITdCibWIBdTOztTZ1OV3Ly8/IKYwqLiiLiS0jKXeOxWqDkwutnZOdsBwya3PD85"
    L"M6mwKK4irrSyqjqYBZsVrEwONUDlpkD1zrV19Q2NeU3NpVVA0NzSijVoZRxY24AW2NpWVJS2d3R0duU3dVdVdQNBS4+6FqYG"
    L"EUZZ3gzbit6+/v7+7AkdE+sbkyZBlHd3T54ylRXDURypMk69vX0V/SAN00AWhE2aDlbf0tIyQxvDChEWzpm9vWDlQA2zOmZ3"
    L"JfvN6YZpmDx3HlpuZWNVVy6ugKqv6p8/e0F5Utic6SDFIPUtC+W11DiQNXCrsy7KhioHgu7FINDSAtEwecnkyUvVUYpODiY+"
    L"4dLS/mVQ9VXTqxYvXwwxf3LL5MmTV0xeuFKdgxvZBzyrwBasBmno7m5es3ZdwaRuhPoVk9dv4EL4W0R7oxdI/erV/atB6qvm"
    L"d85uSA7bBNSwYsnkFWCwcDPCCkYZxi1bK+KWLdu2bTUYNG/fUd9o5L+ze8mSJbugYMluF1mYBm5GEU3brVv37CkBgr0le/dV"
    L"7u/sSvb3OLD34MFDcLDkMBc09vhZOY8EHoWDY0ePHTt2vDHJ48TJU0BwGoRPg9CZs1xqMmAfMHGfO38BAi5eBCIguHT5SvXV"
    L"a5euX7sBhteuXbp46dJNNnDssbHx6d26ffvO3bt3gABM3Ltz/d51ILiBAu7df8ApAtTBxsTG/ODhowePHoBAa+tjEHgCASBm"
    L"a2srWObRo4cJbGxPQS5i4+Nk4mLiYpRlUdeal+rqGxwUDwPBvq7287TUWWQZuYAq+Jg4ngHd9BRULlpZWVpZQsBzy2fPnwEB"
    L"kLaEASsYeMYwCAEARIO4BwsE1tQAAAAASUVORK5CYII=";

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
    L"iVBORw0KGgoAAAANSUhEUgAAAQAAAAEACAYAAABccqhmAAAQAElEQVR4Aez9CaBlx1UeCn+r9j7n"
    L"3HPv7XmSWvNoecKDsAEjGzODDQlmdIIZDAGSkMeY5H8/Lz9TIMMLITGP5IcEwmADAeyYyWMMHiXL"
    L"GFuyLNmapZZa6lbPwx3OtKve96299xlu33v7tix1q7vv7lq1Vq2pqlbVqj2cVitg/VqPwHoELtoI"
    L"rB8AF+3Sr098PQLA+gGwvgvWI3ARR2D9ALiIF3996hd3BDT79QNAUViH9QhcpBFYPwAu0oVfn/Z6"
    L"BBSB9QNAUViH9QhcpBFYPwAu0oVfn/bFHYF69usHQB2JdbwegYswAusHwEW46OtTXo9AHYH1A6CO"
    L"xDpej8BFGIH1A+AiXPT1KV/cERif/foBMB6NdXo9AhdZBNYPgItswdenux6B8QisHwDj0Vin1yNw"
    L"kUVg/QC4yBZ8fboXdwSWzn79AFgakfX2egQuogisHwAX0WKvT3U9AksjsH4ALI3Iens9AhdRBNYP"
    L"gItosdenenFHYLnZrx8Ay0VlnbcegYskAusHwEWy0OvTXI/AchFYPwCWi8o6bz0CF0kE1g+Ai2Sh"
    L"16d5cUdgpdmvHwArRWadvx6BiyAC6wfARbDI61Ncj8BKEVg/AFaKzDp/PQIXQQTWD4CLYJHXp3hx"
    L"R2C12a8fAKtFZ122HoELPALrB8AFvsDr01uPwGoRWD8AVovOumw9Ahd4BNYPgAt8gdend3FH4HSz"
    L"Xz8AThehdfl6BC7gCKwfABfw4q5PbT0Cp4vA+gFwugity9cjcAFHYP0AuIAXd31qF3cE1jL79QNg"
    L"LVFa11mPwAUagfUD4AJd2PVprUdgLRFYPwDWEqV1nfUIXKARWD8ALtCFXZ/WxR2Btc5+/QBYa6TW"
    L"9dYjcAFGYP0AuAAXdX1K6xFYawTWD4C1Rmpdbz0CF2AE1g+AC3BR16d0cUfgTGa/fgCcSbTWddcj"
    L"cIFFYP0AuMAWdH066xE4kwisHwBnEq113fUIXGARWD8ALrAFXZ/OxR2BM539+gFwphFb11+PwAUU"
    L"gfUD4AJazPWprEfgTCOwfgCcacTW9dcjcAFFYP0AuIAWc30qF3cEns7s1w+ApxO1dZv1CFwgEVg/"
    L"AC6QhVyfxnoEnk4E1g+ApxO1dZv1CFwgEVg/AC6QhTzdNL7n9+/6irXCm976mR8n/Nxa4Lvfdufv"
    L"vOmtd31wbfCZO+gzLQtv+2x60x/ek970R59Lb/rj+9L3/sl96c1vvz/9o3c+uP9H/vLRv/qx9+39"
    L"o3/2nr0fE/zoe/a+65+964l/+0/e9+TNp5v3xSJ/uvNcPwCeRuTWmkjS42Y/+8n01s+ckmTJ0ofW"
    L"CgzJfyb8/FrAkn0/kF67NsBLwWsqD5huZpPQMExnCdMhYtb6uGI2hzZnD9muPM9f3wj2xlZuXy6Y"
    L"yu11rSb+z+mQ/u4n37v3j3/4D/9uO91mBJkIjPR6WUMEFKw1qD0LKikZCG966x2vLRPljteW9Ge+"
    L"SrgGyWpYT6bxRIMnE1a5MjNsn2niyi1th6u2TOOqrZNw7bYZ3LRzdgi3XL8Nr67gNcSvuWEbvoLw"
    L"NTdtx9cKnr8DX/+CXfiGF+7Cm155xYk3v+rK+Te/6iqHf/SqqxZ+6MuvWvjhWwRXL/zIq69e+Mev"
    L"vmZR8M+/+rrFn/na6/EzX3cDfvqrrsNPvfZa/NRXXoufJvxzAXn/ooJ/TvzmV+7Gz7z2Cvzcay7B"
    L"m1+8Ze6yDY2kg6OGNg8RwUwr+85tO3e99/rrr595wY4d05dffnnr5ptvzhkW7e31g4CBWK0oSKvJ"
    L"n3GZkvlNb73znW96210DARD+N+9M7yN+F+G9PBbeTfx+wt8IKBveuTiYZ/XORP+rlqk8w0wzx2wN"
    L"LdKtBjZUsG26iR2zLYfLNrVxzdZZXLuthOuIr9s+ixt3bMDzd23ECyp4IfELL9mIL71qG77s6u1M"
    L"vh2LX3HjzrmvfN7O+a+6adf8VxO+5vm75gVf+/xL5r+O8M0vvrT4lpfsxhtechm+9aWX4dsI3/6y"
    L"y/AdL78c30n4rpsvxxsJ30n4uufvos/teM31hBu2M5m347XEX3njdnzVjTvwWuIvv24bbrlesB03"
    L"7dp45HmXbHrqJgLxAWKH63ZuPOCwY+OB67fPHrh+x+yB2XbrSN5o7skbjccEodHYE/LGHiM28pE3"
    L"92R549Esa+w5WWQPPdEJH39i0W7fS3icsHfBbn+c8Bhhzzw+sWcBf0v8d4/M26fvOoYPEt/52ALu"
    L"nuHTwrfcsNFetmuqmGbizxCEpxt8kiBsmGre/Lp/8/b33/BT//UXv+Qn/+svXvuP3/K7b/y9T//u"
    L"P/yjz73zu//0gQ9+7/96+M5/9OeP3vdP373343x9uHUcfur9T/z7X/3bwy9MKWW4CK9wtub8HX/y"
    L"d5v4nvi/lNCA/f1gIRAsIyY0sxAahJSFLOUhKwiDPATSIbWyHFN5A+0aGg1s5DPgpqkWBJuJN7db"
    L"EGybbmPHzDR2zk5j1+wMLt04g92EyzdvKK7csmnuKsLVY3Dttk24jnD99k24Yftm3LhjM55HuGnn"
    L"Zjx/5xa8YNcWvJDwoku2UC7djbhu+0ZcT7iBcKNgx0babGSyb8Q1Wzbg2q0beMed7V++febg5dtm"
    L"Dl6xbfbQlTs2PHXVjo1PkX7qsq0zT+3eOnPgsm2zDpcTb9s8c//2zdN3b55tP7RxeurRDe2pR5hg"
    L"e2baU4/NVjDTbj02TWi0WndmramPhlbro0ZszdatqdG6LeXN2yOhyJq3Dwj9rHF7P2t9rBPa717I"
    L"2u+ZC1PvPRaaHziaWh8kfOhIan7ocGx+6BDhYNH80MGi8aGDvfyzB3rZfU/1woOEh/cvhkf2dQmd"
    L"8MiTDvbI4x175LF5e/TJRRzcv4AOoffUAvrE/X3E++bQ3zcXHfbOx/7ji3Gwb77IjnSKbUe6cfuR"
    L"TtwhOEwsqHjbj3XitmO9tPVEt9iainTt8cW49ch82vi54/FIZ4Dezbva2Xa+GsxMBQhmWwEObF99"
    L"xc4vedkVW37y5Vdu+edfsmvmH75q9/T3vHp3+++/5vKp195y+dRLvmhn60Y+EH3pIKZXZWZD6Mf0"
    L"Lx85snj3z/z1k7/6B3/wB1v49NDg+0wOIBCM8JwvX8gANckvxH5Ntkr+qW7ztkbI39DOp9DO2zaV"
    L"t5jULeFA2tp5q0GYms5bU9MNh8ZMYyrMNqdsU3tqbseG9v7tG6YddhJvmWnvJxzYOju9j/AEZSVs"
    L"bD9yxY5Nn75i+6Y7L9++4a7d2zZ89tKtG+6+ZPPM53dtnn50Zw1b23t2bm4/duUlm2+/eveWjwqu"
    L"2r35o1deuvmjVxDI/+hll2z66BW7Nty6e+eG2y7dufH2Ky/f8jc3XLn93ddfsf3d1xGuvXz7u6+5"
    L"fNu7r7h823su273tvbt3b/7fuy7d8je7LtnywR27Nt+6ZcvGezYTNm3ZcPeGjbOfm9048/npDdP3"
    L"Ts9O39+aaT/UnG4/0iDk7fYjaDSOxrwxX0ORNRb6IVvsW9btpNBbTKFfw3xhjbkibXIYxE0Lg7Rp"
    L"rp+2nejF7YQdQ+jGHce7g0vmev0bF7r9GxZ6/Rt6nXR9tze4ttPtl9DrX9utoT+4plcMru4PiqsH"
    L"Rbxy0I9XDGK8ohhUUMQryL+iKNLlMabLIrDbUroEli7hwb4LZg5GPATYzgy2I8vCVh7oW3OzzY0w"
    L"gmZW0s3cNuVZ2ETZxjzLZvOGzTaa2YygmWUzjy4kThfpBduaaUMzwKGVYYOgmWHLzBR2T+HoFRvy"
    L"I9dsbhy+cefUoedfOnvo6h2bHnj5ZZsPf9U1m+Z/8hXb8IpLpwpt2qncIGjngThgAPuxz21/1duu"
    L"+/5//frN/8/7vvZbfvPWr/mO37/nK7/zj+/9yh/5yz1f/ZPvf/Lr/tXf7Hvdz39w/zf90kf3f7Pg"
    L"Z/9m3+t/5fbDL+DTQy6f5yOEszHodq/1zlbWekGrkeOKHTO4bvcGvPCazfGl129dfMXzdzz2mhdf"
    L"euvXvvzyv3zdK6/642/60qve+ve/7NrffsMt1/7Gt776+l9/wy3X//rrv/Ta3/2am69++9fcfJXD"
    L"V738qrcLvvJlV/7Ja196xTu+4qVXvPPVL77sL77sBZe9+2U37Lr16ks37rl8x8yBXds3HNu1dXZu"
    L"x7aZecLCjq3T8zu3tee2E2/bMrOwdct0p93KpvI8bMwy2yTIs8ANGDYG8vJm2IiQbQp5tsWysLVX"
    L"xMuPd3s3Ck4Q1zDf6d2w2OvfsNgdXLfY719L+tqFTn8Ii93+NQv9/jXkX90ZFFct9Iqruv10Rbc/"
    L"uLzTK67o9AvSBNK9weDyfhEv68e4ezCwSwdF3DlIuKRI2DUOqUg748C2gxATcbKtCbYZBMdMNJht"
    L"MgubjTzCpgC102wWwkzIsplMELKZUIORTzoPYTqz0M7y0Mqy0AwEYUFOmiB+KzdrMjaNnJWAfqkS"
    L"SshCRr1AYZZlAXkIEkA0ITnkIYXMAVkeKMuTZYIMFrKEYEMoLIvHe+jsaOfY1MocNjYD8Qj2ptnF"
    L"e/bP7/vswcUnPvNU9/G7Dw/2PLqA458+ir23HbKHH5pL+199+Sy+7PLpQf36oANgmn0L8lb7dTdc"
    L"sfudl2/f/u6rtm9+31VbWn99zeb232xr5x/oDuL7HjvZf9eBxcFfPjU/+AvB0V7xV3uOLd7zLz7w"
    L"5B+86+GjV+E8vMKzPebve+tnv7MRml+5eaaB51+1GVs3tTA7nYObo99sZAenm41HZ9rNfVPN5uF2"
    L"Iz8+1cjmGiF0goUB1njFFLNBgWavP9g83x3snmfCzfeL6zq93nWLvQHvcMXVvX7hidUbYHe/SJcO"
    L"BmlXEbGT9M4iph2ktwsGETuYaNtjwrZYYBuTaQu/VW6OyTalhC1gMgnI3yxsCJsEgG1MCBsthVnS"
    L"s2Y2U0KYAZhYCLOhxDNZCNx3NpWFrMU4NPMQmhlBdGZZKwMhZI1GI2R5njcaZDaykI0gI7+RtVpZ"
    L"1iDkVKASmnmGRiNPzUaWGnmW1BY0yGs0c8oaiTSoD8lz6ghEV4A8CwSOIMstG7sC6cBKYCELxvEg"
    L"hBxmzFhBaJAmhEYyazA+hNAoEBoRoUXcJHYQLYjImqmERowZdUE5WlwHgrViLKEgLpI1j/TB10Ge"
    L"bFMZtrRL2CzaIeedPJttNvMtjSxsa+a2vRFsezvPdkzn2bZ2ZlsPLIb00Hw49iWXtvNNfG3YIGgH"
    L"zBLX8ELenL7uUsM3X53hO26YwjddN3P8u1+4ua+nh3/68i2dLdRtZYF9lSAasO/86COL73nfo8eu"
    L"wXl2hWd7vNwj/6GRGa64ZBZZsLq7SHqxlWUHWo1wrJnZfKNhXVPSWyhSQKoVT4u5g5ioTSb65vne"
    L"YPdir7iKSX0FeZfAwjYLtinLQjtknmShkYeQ55knC3FkQgyG0MwHrWY24CHU50bqTTUb3VYr70wJ"
    L"Gnmn3Wh0php5r9XMiyaTqgT6auaRNFrktRpZhfNUtYmdHNRa/AAAEABJREFUh2YjM0GDOG9kodHM"
    L"suYYNChscIB5M3CcOYeXMX+zRp4JyKJJ7mANTqMRLLRGYK2M7rIs0GPmEGhn1A/BmkZg+JvBSJu1"
    L"rIJSFpqSC7JQ0nysbwJoOBgaZsjNrIRgbFe0BdKlHFYeCGYhsxAyBMtBO5R2mZmNgBqw+o8FM2qD"
    L"GMbKGzCjAiHwScAVQvB90coDmtkIGEo0MnaUh6Jh6DdC6GbJurmwhQ6DNc9Azjcb2cmTRTi0WKBz"
    L"3ZYWNrYyDJ8mSG9sBXStdeQjdz/0Ox/9/OO/8bGHj/znzx8d/Madx/N/f+9C8y3NRvPOH/iiLdi9"
    L"Me9O5wEODcM0oZnZ8+850Ps3KaUMZ/H6QrsKX6iD09kHC1fu3jaNLNhQ1WAxwBayLCw0smwxhKzP"
    L"qEULiEOlNRAWYQVi6A2Kmc6g2Nnrp91cgB1mtjFk1uYGyLnwBRO6OzPVmJsmtFuNk6RPtqeI240T"
    L"M+Mw1ThB/ol2q3Vyutmcazcbc+1Gc2GqkS9MNfNuzkfWLMszzomJFKaIp0IgDtbmJm2FzFpsO2RZ"
    L"aBGaNVDepKxsB8pCaGTmkBNXgDwL1sjMmpklArh3BRm7dshyZrUgC6ANIVieCXLLsoxgFmgf8mBG"
    L"CLmFkIVAXqgvYxsCGibHIfBja5byLItUEjjNduEQsgExIZQQbJAH0g7WJ91rBCshsx4HRgjdPAvd"
    L"RpYRgtNsd4ZgJY/9dzPqcZCOs8wqnFU4dDmmnsCAAXj1ioguH9WWAr9LdJLZPJDmzTAPS3OwcMIs"
    L"O25ZdsyCwI6d6NuhXe0MW/zJgXhI59g43WylYhAGvUHodrvZYL4TimJg84Nw/P755vuP9rMPfdO1"
    L"G2zLdOZPDhvoY5Yw08rAYX0zh7eNcN6U8GyO9M1/+MBXyP/sTENoHFII1s+C8a5vvcDYcaH848y4"
    L"0unoZDEUg9TiRtjUG0Q+usetxgOZfvMsC8h0R8izRd7FTzQbjZNTjcZcK88XeSh0HLKs21wGGrlx"
    L"01qPm3GQ5zYwukpI7ZTAx/uox/tpjp/Jb0x2a2QhZISQW7A8M+QhoBFCItRYdGxkIVKnpENQe9DI"
    L"skEjzwYczxAaWSA/DDiJPmW9Rh6G0GyEbgkce4OQsy3Isk4zC5xX3SbOCJwLTxLO0yogj3z67HJ8"
    L"PfbVawTrcc69Rmbsz6Fs5yU/z0M/z4KS2kE2K7ZDYNwI9JkRwmpgxj1AXbNeRgiGXhhi0QKreNYz"
    L"C33w6gwSusWpwAOgSDH2uU79hNDlYdCNZiWA2CHrnRyEE1kw1AfAVibwVh4Cgm3tRojgg0YqeN6Q"
    L"MhgQ2WtZHlxofIR+F6/c2PAnCD01bGwGPkkEzDbDzJ987tg3lZrnRx3OxjCb+ZJuDJFRTcas4joo"
    L"wsmi0JmNhiduKIrYGhRxEz+KbbQAJmXI6NNCQOKmYhJZNw9ZJzfrU16QX1AeVwMeIlxjJI2aTxQN"
    L"fiNoFynNcltMAzZFW+aAqaPUCKHPRo9z7DY92ZhgxHWCSFZBf6jLZGKy9vhIwIQLTLzgiTW0ybNe"
    L"TmCSjviNwKQkn7a13vK41MmyjIm4HDDh5COUOCPmRPrBwoqQMXbBjHErwcZo8dUuAYUZCsYtEpKv"
    L"MbACNvKNIUV1Udu3wFJcicdQVwcAocOngHGICbqJRMAiD4EEM+0zsjEBPB34NQHgGTIEKdAGE1cB"
    L"DWaCpcZCEe7dzgNjAxN/QzMrf4loZX4gTDdDWzrnC3CdzvpQufDg4iDCeNgiaMMkZls6k5FYjMbV"
    L"MSbnVExxhkvVDgjcqzBecpXMuBkt48bGIAQU1Bd/TaDA6HwaxNgsIma4Odr017RgWQimTmKWhX6W"
    L"ByYrIVNClQmXM/lytgWuk1FvDDzh2HZsVoS1QKIewMQyhmo1QMpgBCDjTMdBc1oOjHrLAdlfUKl9"
    L"Lu9Eyy1YXroat8MFEfhTAA+C+kDgPljNbELGtYUOjy4PkRrc5yCedlDU6Oj9X6oC7kHUEJHiREfP"
    L"YuOZcK398Ez4ORMfHmBWLGAiwTHO4KoHzRM8MKuzGMGvychhCNp0qC/eAcgojLhmrYblt4bCmEWJ"
    L"PlOa4lNAy0DajLlPik4CD5Qs6GCxQUigOpIkzzwkGATslMX9s7JVgAaYAJQXTYbsklPWp+OXWk+/"
    L"Xsl/6TERCYjWWLpjSa9DoMONIOAhvUYPgHQ79CO7GuS3o4xeixcOucd+h8BToUfgXizWYv5c0Qnn"
    L"YCDepyVL4GP6mfbvxmNGatNTxk2mnBhJmDXkJ+549ZRGglMp+RCMSyzqAIgZk79B3zlggYcJEYuZ"
    L"kl134oLJP/RNPXaHZ+hK9CUAsYC0JRIEHgjcwpgENlcq9cDGsJEWVM7hGHBE0QRWbJby6jbO8FrZ"
    L"TvNam7OungB452ay+l28xrwLJx/4Gtwk6oz7KZOf3xXWeADoCaLLA0DghwAPk14J/qGS7s+LorU9"
    L"2wON7DAxg0zJw0dUtck6fVl2sLLWroJvZ4A0i5DTWOWSP8GyKsldBA40h+kCSr9JRDLjK0yZgbHk"
    L"Y01XrbsyTuxHMOauVh5jnUomshQMYQGbayi1a2F2jFMA5SV5DSWnrJfjlZLV6+XtNG7B6raLTLT6"
    L"rt0hreQVJJkK5Hx1F/4EIJsaOjpUBH3exmvbVfyoL/7cDIEfADTTE0CfT4O1+fmAV9z/z+Lgh2Fl"
    L"5gzp1frTIAXjOjI85QGCTJZxtWVp+RIsK5xkyp1xt5S4kqlRkcwX7bi6VWLJlwJ/UsoXFhebh48e"
    L"nT0iePLBbYefeHi74OiTD28/9uQj24/vI+x/dPuJpx7ffvTQgW3HDj+1de7ooY3zRw9v7J441u6d"
    L"PD7Fn6jKTrQD+bqZBJGRJOY4eSSV4+GgfWw1Lo1Wr+sxT2jVzDFck+N6y/HG5SvRsjtVpjkIJiW1"
    L"bpd3f0GHuE7cxV7klz4FhTaOdBiSXqHIe4d38Bq61UHS4c9KQxMpDRuThERd2guU+DoE+mrHdFae"
    L"ACZH8/Rba8yDp9/BMpaKXb2Wepgd0svoYvkBrr64tZ+ljuVLUMtXwxO23tCwRxZiCcQRHoeFTic/"
    L"cvjA7ME992994oHP7rr/vgd2793z2M79e/duP3Ho0MYThw9tnFtAa6Gbmp1eai524Vh0b4BmbxCb"
    L"sbfYSr3u1KC7MFsQuovzW7oLc1sXjhy4dG7f47vm9jyybWHvY5sW9u6Z6Rx4stk9fLA5mDuRpzgI"
    L"4O/WPCiMP4nxTIgOSHF4IGjMq0E9l3GdCV7V4FMQfY5r4ZQ21nBV7pbRTMvwAMYJi73kWLRAiRjT"
    L"0tfv5e3ltODO6/IAqUGHiehO108PqawK0urz0BD0eNsXdIUHxdJBrOrnXAvXmg/P5DgDzIZB4uLz"
    L"HV2/BpzaRTiVRU4kjJXllcYUAN0F16CG5S/1N7mROOahqugiFnbi+KH24Yc+s+3+z91z6Z6H9+x8"
    L"6sCRjUcW09R80couGzyCG/t34ubOh/GNc2/DNxDeeOw/4k1HfgnfffiX8A8P/2t816FfxHcd/Nf4"
    L"9v2/gG/f9wv4tid+Ad/0xL/Fqw/8Dr78qf+BFxz9AK6e+xS2dx60RsOy5oZWqzEzNdOand6YN7It"
    L"6C5u6OzfP33i/vun5/c8MrV4cH+zf/xoY7Awn8V+P4wfCGs9DDS3GuoJL20zuPCDgAIWCBRrQW2z"
    L"VixbwaS+Yi8YcTuDhC6hwwQeBx518AFg/Dp1/SRVAuvgqKGrA4UJ3OWHBMkdTh2Ms1VpRNRFj68N"
    L"49Av0voTgAK0CjDhk4dW3wAYSKeX6p+6gbSQgpFmOFVpJPwCKA1o6euFeO6SA3bMavHIk1P77r9z"
    L"xz2fe+DSx/Ye3LKv02plaWDXxPvw6v5f4zsW/gd+eO7f4vWL/xOvWXwPXtq9FbuKPbhk8Bia6Izt"
    L"1US67MEUmkTnLHnqYmv3UcIeXH/iw3jx4T/Dl+z7XXz9wz+LVz7x27jpwF9g++IDxp+iw9SGqdbG"
    L"XdtmNl+6bbY13Zgujp9oLuzb31rcu3eqc2Bfs3v0cGMwfyKPvW42OgwS+y2B3Z1RKUe7xERMQcXW"
    L"8giq5prRmIsxmzSk66TXITAOsWBaj9SG+iVxqqDD5B0H+aLvkeKIKl2M1XzaR4+HUI/EOCz20/oB"
    L"MBan5UiuEvddJTHjkypGbW0YASauycSfEK3QWH4TraBcsWUjgH/fK5lqC8oW0O/30dlzV3jw7s/N"
    L"PPTE8dmD3anGxnQcLxl8Em/s/A/84OJb8A2dd+JFvb/D1niQZrLWTkp86EycqJ5HSpoMKBhI0gHp"
    L"il82qQuCETB5JWDT4qO44vjteMnjv41XPfCLeNFjb8Xu45+wKeuG6Q0zre1X7Jrdunt7uz3bagxO"
    L"zmX9I0cb3QMHGnxVyPsn+WTQXczSoBfq1wSNSjDZ0eoto1hANFnErIESraeA5JrLmPmYDSfOVodJ"
    L"t8i7/1KICqYMqbN8kb1AsYY/RSjpBR0mc0c+eQtf3naSq6cNvft3OZbxA4BPFMOn20mLZ671THo6"
    L"03V5Jvo2rlG5CpU3M37EIr38YFZPfvqi5dIycr+8fFJfOgJ44i/fX+rOYe6hT+HhBx7Bo3NNPnwM"
    L"8IL0WXxb/4/w/d3f5B3/g9ieDiHxj7yXWOMQiAMmMnthU3d5Y2uoY/DkN/IEiV+UtZeTKkgmoDZt"
    L"ax7ZVU+GRuxi29zncd0T78DN9/873PD4H2PLwv023QqNLds2ty+56hK+KEznqSiywYm5vHvoaN47"
    L"dKDZO3okHyzOZWnAV4TILc3+dAgI5H+tYFSsgeRkGRNofQWTCqu3ZL5UQ4mqxBuHDhOYDwCM0FLt"
    L"5dqMJdmyEegAEOhgIV6TD4YKSnydF8I1dOIpHyLY03O3nOl6PBMzUeyCcQ2Y9txrJFLgb+rLuV4+"
    L"GZfTXI633OYZ15NcUPJO7Uuy1JnLTj74d437H3rSnujOYhOO4iuLv8EP9H8TX9N/Ly6Lj5fmXiem"
    L"sBPEbs1GYvayaNZsUQBBUpsiY0MJ7xjSA8zUAhss1CHFUvJUy9bI4anhCmqXaoa86GL7sU/jeQ/+"
    L"dzz//v+EHUdvt6kM2c7tG2Yuv2rXzIYtszkGfSvmFkLv2LG8e+Bgs3P4UM7XgywVPAj4sZArwjEk"
    L"B6ijCjgsjLeXo6UjwNKr8iF2YCUgWlMZM3X9OtnGcZ+HpgsTaxkQrVak1ikixqHLJ4DO0NFq1h52"
    L"PwB0CI2PgwfK+ivA6qGD9kfkU6//PQC1lt8Mpybkan7N796raYxk2h+CEWeyL8n6i8faBz9/+1Wf"
    L"e2jflie6m8LGdAzfULwbP9j/bby0uAMtvp9Lr/ShlEklCWIVJnjiJEGQtBJCyS4w8I8iQYGZIdGm"
    L"qri7vADkO8+F5LkOeBlBZYRrquxLdUJ7cR+u2POnuOmef4OtRz6BZpaynTs2zlx9w+Wzs5tncyti"
    L"SJ0OnwpONLoHDzd7R3gQ8KOhDgLwiUB9B8aVo0PtX1ig3lcDDT+cHcgAABAASURBVF3ghjIQyEBY"
    L"QHr5dafg6RbFR7Y1Fr0CKKRdPjUImLT+OiDcGcSRdTXO5Vxox9SJP34InBzE9QNguYCN85Lx/j/O"
    L"OIVWeE9hDhnjGydA6yUoxUvXbLwtWlBq1vWoL8kEBx74zDV33/voTXt7m6Z0x39dfA9+qPgtvCDe"
    L"QyMl18hGLTJhiWnCXcWiJswMRkptE8UheuKLT6CIGc0S6YFKRoZJzwFeG3RVNe1FURsyp4mEMP6h"
    L"F0ZBY6KSc1lJoYJG7yh2P/InuP6uX8aWQ7ejGVK2+9It01dee2m70cphxcBSt5P1jh3PuwcPNPVq"
    L"0Gc7ic/xgU8FAs5QjgnwXrGGy6gjIIIbqSGoFlFIgHNwKddr0I1f0Cs04WowY+GsOCPE2Na2wrIV"
    L"zHee3VeA0QCeGepcxN7DatrJozk4r2xqI5fU0lqDFSzl123tK6eHhLdWqcq+pC7oHXty032fvvXl"
    L"D51obA9W2Kvibfi+4g/wgvQ5+pBGedyY72SyVDRygWgHpgk3h5K9ZHOiJMxoRRCf2Yrysd0FtCpt"
    L"SDCXpV+Dc9gb5TSiNhmklZC8M4NATfLgOtDFvoUc2B8ooSkEze5RXPrQH+OKz/0XtOcft5mprHHd"
    L"dZfObt21NQevNBiEtMiD4OjRfHD4QKN/8kRe9DpZKgp16mM296+RJHoW0HANxagjIBoVLWbFFDkS"
    L"fIFU5fML9HJRmD+jcT+TiAVD5E2zXqoKlwm5nJ/VBloZL2dG3nI+Izdv9L9kJL+yP3rfbTd+6qFj"
    L"Nx7F1uwqewzfH9+GL8cn0LIuoESiUoIuErWl0ZqyJKDI2DYzGD8RwgxKOgN1aFgnvpmRLQZg/geu"
    L"B9IUAMLQJSm8VSd5qUhb5xrgGH7pQHEO/Q/9SNUbTgyp9smHcMVn/gO2P/IONIrFsHvnpvZ1N+xu"
    L"t9pNLkpE6vZC/8Rc1jtyJO8dP5YNOosh9vWLQcH8T3zaIXivqkQnEWsCH+O45hiDkRqb0bjS2mm5"
    L"84mu3eSi1lTMz0kAeCPxvrlxLfmvAHHFcbjiitJK4Ctf0WNI7Ixtz0k6shChPGXbhAeLJ6bvu/OT"
    L"L3tgfsum2dDDt+Ev8Ub8BTaERSBrIAVB7hhZE0ZI4ht5yGCUG2nhBD0hlCAehdDhYOwMPPFgTH4D"
    L"EQcC6iW2h7lTp/mQASV8yRUvgaanAJWolohKX4wnabWJIFxbsa0mA1+yDRuf/Ah23/nv0T5+n81O"
    L"5Y2bbrhsdsu2jVngWHXXT53FbHD8RD44drRRzM9lsdcLiOUhwMeBibEEOuUI2MnaytioMO5ogo8z"
    L"vGRcmSjCFXlGaMzFGdmdr8pPN07P5HwtJD7LruBxTQPkqrEM9xH3L2pgzhkCmOsIATHwWTfkZiHL"
    L"LCweeHjX5+579IXz2JBfE/bie8Of4vrsCSjJQ96ChSZC1oLTTPxAcBlxaLRg1EFoAcRGbNQ1ygTg"
    L"wQCnG3DadGDkMAhngBmLOSZBHFBe5JFQMiUmFUkmuGpxSCqB1WQyO0kMlDbwS7SAAh4w7kKKbLpY"
    L"lcSxJPLOUey88//BpsfehdxSuPbKHdNXXn1JMwRD5IfA2O2Gvv4ewfHj+WDuRMZXgpDGDgF5qUFu"
    L"AxJqqPmrYdm4vCaIWXxGwi5ba1XPsTIUEqzVvNarV8IHUTOfA/jZGMJwrs+G8+V81gvC/R8l1wD4"
    L"c6CQmhOwLLPSkEw3cy0SfRn3K4gJxg1oyNjIDdawEHIDIYWmWciCZVlAOPLAndfeu69/RcqncEv+"
    L"aXx79n7e9bu80zeBUILu9GASq21MbqfVJiRTYlOPNHuBdI2+/DCodM39tGDUCdkUMW1IW8iBjE8W"
    L"GXHIAONszIgBODYkYZCvTS3akzjB+EckJq400SobVqLx2g3JlzrRSGTY8Mj7sP2Ot6AxmLedW2db"
    L"z3/B1e1Gk+NFQhz0rVhYyHpHj/OVgAdBt+OHgJ4CHEaOJqi1HgQaioBTK+3Z0JTVIDlkq70aBClL"
    L"QfMTroBRrKi1I67A2pXPY82nE5unPd16fYgjuMLGG38Eln0CWGVgxrwIWVCNwEUnzaw2s9wMOQ0r"
    L"MOqAYI2AIMgzx3bwnluf/+h8e/NMGOCb7QO42e4ClJS8axtxCoHtDKLBBE1WthO7FC0MJq8RkFEv"
    L"Z6JkTGjaggeDXhug5GeySw95E4lQHhDUJR14SATKQ4WNtNsYDwWHDByAg4F/NAb1D11jO1xJreay"
    L"QGYtl5mZ6hJqPnFikguaxx/Ettt+Fs25x23jdN540QuvbM9umgmyirGP2OuE/okT+UB/rZg/FxaD"
    L"fvADgD6kUzo+tabSqcxlOO7Dq1I4PtwxdilcoQ7jRmM6XMGx1tpI84+ta9M9X7WeTlye1lzrBTR+"
    L"Qiod8G01cmezEY07G6NryaCM0iAwgOkGpgwyJnnWMOR5sLxhMTDBlejIaSzIDHykhQWLBARuDENv"
    L"vnXvXXe98PFiR2tXOIxvCe/B1WEvzLRFDYgFYn8BqXMCcfE44vxRwiGk+UMlXjyMtHiM8uMOsUPc"
    L"W6RdBEB7AX0ZASEAIeMAeDBYSadMWE8NPASY8IkzSXx90FMIiK3RRsjbMD5JWGjBKIf0dJgYD4aQ"
    L"sRv6oL9kBuYtAGIHJrtIjF9kSE8siutkdSxe6cCtqemcMFjEpr/7z2g/eSumG1njxTddMa1DANJi"
    L"fFKvb4OT83n/2GH/LlAfAonJYvRX+8GSq4ywBrFEsKTp9l6Vgnr4ZWstdUKgPcspyozcKbxVGe4k"
    L"rqpyvgvPOCZPZ8Iex8qQdxquaRJL4NwhwdbYgAwBWkvmMJgOTIWAvJkhz4izDI0mcTNYlmXBAjUD"
    L"PQduVCME8DKQAtOFudKba9x555037pkz/sT1IF6/7z9i8+PvRvHYbRg88B4U9/8V4kPvQ3rkQ4h7"
    L"Pob02K2Iez+OtPdvkZ74JOm/dUiPfxxRsj2UUy9SX/bxgXfRx7upczsK6Tx1F+Kh+xCP7eGhcRTW"
    L"7wLICEbIMHrKIC8wuY0HA59AUkaaSQ8eCMibMGLLywPDKEdGOlDHZMdZanZGnwrpcvlFHgv7HK8Z"
    L"D3LqUkpGvFAsYvruP0Br323gF4vw0psun97IJ4HAflIqLA56KOYWsj5/JYj8OFgUvWB0kvgkIC/l"
    L"QUBG3UGFOUqs5SCQnhauMgO7rckvGDNiHMMa3Zw6hTUaPvNqz5ZHxePZ8r2iX4PpWD0lvByMcXWC"
    L"IAMCIWsGZFMZk94IxLy7N3JDg2mQk85oIz0Eg28U4UjP84OUDi7G4sn5ePLB/Qfn/ujP3/u8v360"
    L"CHHvJ/DmQ7+A1okHgAXe0ReOQHd+WvPGSEPwoi9tZiLe01DytbkpFjIYldgQJsOGmD+hLRxCIuD4"
    L"Y0iHH0DcfxcPk49j8NAHEO/7KxR7Poq47w6kQw8inXwKyQ8GzkK73Ig1G0GmJOcsg6AF6GlAhwGP"
    L"QnNoAnzdwPhhoHEIeBho/Bw5BMa6LjVd45pfzcabnBI0nJm734aZz/4eGOTwkhdcNb1hy2wIEqQU"
    L"kr4L8FeCLg+BNDeXxaLvh8C433HaHVeV+OUhUTGWQdKBV6VQ3ZbUGuvKtkITRuLxRjTBm2hwGbwt"
    L"RSeAAG3ZqnEBoXqqZ21KBt0rvDvtgZAQLZAKgTEOMB4N3N7ImNx5yzzpG9yAOdOhQSCJLAOoDppR"
    L"n64GzLvjnRj3zRV46GgfDxAeOzFIhxaLwckTR+yTt37kFU+mHeHVuB3/X/waptMik5qpEbntWQBD"
    L"eVWYPDMOitgVXS6ZGLQTSkYuCVObmA6UOH5iSFKyyKXc68qfXhtOPMmng3sR+WRR8GAoHnwvop40"
    L"dCgsHAWKgkYBPkvTbAk6EPww4IHAJwPwScD0qsCnBav5gXpGOwOMOHEOpHDai2OlyVBNZvVcGvtu"
    L"R/uu34Ves176/KumN26eZdxN0wwoBha7i3nn8JHmYO5krkMgxqJ0xWFwEnDNoeelRFpVXjoaszmF"
    L"MSZbhfShLJH7/Jbwhs0Vcv1CPASWi80wDs8GkZB4AiQtpZ6CRRirLIvQNs5bGZPe0GCW5xmgxCeJ"
    L"jPtZ2zuEalALRZp7coH310P9xz67v3P08WODwbFOkXpcWW06o14xKOzuv/34TU+m7Z78P5TehsSk"
    L"T5RJzt0HM1JuQyYxKDSjB2JyYPwj7HzRfNctz7Bql9Cf5DI1EQI2nHZMyn15RSnbcua+SHPyGAyQ"
    L"5g6WhwJfH4oH3ofikQ8jPnUP0vxBlE8onDmDAEXDeBQGflgMxDwIwAPB8haMTwmmtsAYPQf2YZyP"
    L"+mPv44XDg4BiZ4vW0ByT4yNm1eBTU/PO30NuMdz8wqvaG7fM0sT4IyFCHAwCD4GMTwJ+CKRBwTwR"
    L"gO9l7DsY9JUXHD5dDosNKXBk7ATLXzamqFksr1Vyl5VX9ku6Lw3WULu5V6UyJ1cSF0j9dOPyBU1f"
    L"8eReNu4NZAHGR/rQyJDrox7bw8RntmfcxkTcI+xxocDCowt47JNHcNenDsb7Hj6e9i/2Y4+iUsFE"
    L"jeDBT31k2764JTwfD+KHmfwu5o4S9i3HSpvdjFuHhNELWVASeGZUDUqhpE9KfvByvpV6tCVFJpvi"
    L"C+SH/pw5UbmQimQ66RWoXoKIioXeSaSjjyI+/gl+W+CBwG8O6fCDSN2TgPTUb8iQAg8BHZ0CPg3A"
    L"D4Img9pEEE9yy2DGUQpADA6h7qem2ZaEqNQQQain0XjidjQ+/8c8iVO4+UVXtzdsaFsAeLuPKRYF"
    L"Ymcx9I4cbQ7mT+axP+DA+Gk3Rr4WJDMW6kIG7A5Qg4QR6mIe9Lq1BI8rLhEtbS6rWjGrbpeanNp2"
    L"xcqIUm96xcY5KM9ml2d1WgqpNoGFkLQl+ZiftYzfvTPonb6RBfgdnxnPrQ0ioA/09i1i/2eO4p5P"
    L"H8bn987hYHeA/vID545FeR27872Ng/3ZcBWewI/H3+T2SgTKuKNZPAPMtzr5bsYtKIHTrIy6tCAF"
    L"sUmSIaaAZF2oMJKzUfPdN7spDUVQQlspG7HzrdSSmYMqqrEk6bHJ0akFLB5BPMjXhkc+guLB96PY"
    L"9xl/OjDqgAkORSwrD4OklygeBGi0KBI0AR4OyXIk6iYEmBn9Gox1XdTleFu0QHxB/uAHke29FQ1L"
    L"4SUvvqaV5cxzfg7g5BKKIhadBfSPHsuKhXnT3xrkHBTU4DgmHgYwC4B8cgjQ5bQIApVZn1pcx6tT"
    L"ZZMcBQN0XeIJmZUtdl8Sq9VRwkQ/wiW4nVdl+0Kpz9qUtN/MjI+RAlgjIOTBcm4I7ic0QkDGwbDA"
    L"l4qv8SfuP4mHP34An33gJJ6Y76OzetCTG8p44f6P4fOLO2ynHcXPpLdgWqbJdBOnCyNIVxi0IU7J"
    L"9zArANET3siipLIhuy7kowKZSacWgd7gQvhl3janvZKBbP21oeSr6TKvyBNDeu4n0QN5lDGJRq4H"
    L"fOg5vte/G+gwiPt1GByiBcNnGSxjYlp5GICvB8haEATikPEbQqDMqEfxq0grAAAQAElEQVTvMOMc"
    L"mXqJnaiMYR8GeVSRmkP+6d9H2PsxzDSt8aVf8vx2oKlCFqmsJwH9dwP9ueOh6CyCTwKMZYLRB4s0"
    L"dQg4cLBWZ1glhy4pjSYqTgnjOiVnpTq6ILADJ55WVfpYasroLmWd9+2zMidtoMBtkHEVleU5G3kw"
    L"y8wsN2TBYNDFVTvYwaG7j+Hezx7FA/sXcbRkq14BwiR/8OT9uPPIDGZsET+RfgP64Fc6T0iVqrE7"
    L"tZRUAjaHErBhgOvW+sPGkEEFZ0qvYvKAERe0hy4mBIsoKZFLr65KzJYE3rd4UiQkZVIqN5/Jn2RU"
    L"dL3KpuoWavr3DH0/OPa4vyrEh/8G6eC9SH2elTxZwQzzO36WwwITX08B/HYQeCgYDwJ/fUBGNQPc"
    L"IdOPffoQxAKHzrb65PDYIIMlu+tPEXgAbWhm+UtfekMTRmXX4wz4OtCf4+sAnwKK3qKlYkBmaU21"
    L"RF/sBPoswOdABK29lpAeMAlyyM7GiklhrL0yWcaQ22lSZc32k2bjraDBjjPOc/qsTCfjyuXsKedq"
    L"65YvnBnMDCHTlgBwkIn/qUP47H0nsOdYD/Nknb7QZ60kMi0cxz2PHabjhB9Kb8UV6UkX+1aqKu0+"
    L"JZRRMgTJuEfrtpqmikA2NQHJMHGpR4IpgXIgI2ZSQXdXJh6MbU9iY+Ik7nsaG2kiL3SsVmLSl234"
    L"nRi61G9pQVuw71IT4xd1zAwsJZft1F1AwV8Sigf/Gumx25H4awM0Bo2Hrwie8Bxj4iEAQuCBYGzD"
    L"chjl7sxs6K8kfAhOsgvH6C8ifOQ/wPpzdunWmdY111ySBbejLQ+AOOhZof9+YI7j6fUwSAVDnmJk"
    L"QUgFDJHqCYlUQmAdAhybOvCKTKOC2k8PkptxFRwPq9L5sLkswcEsyz8HzGe7y7My1Zy9cIcgc8wV"
    L"rxaBKB3p48SnDuJuJf7iAHzln5wyTSYZbPmiUsDCFhCMKAf2fP7ObBEtfGt8D744fZY7N3IbcSPo"
    L"liYdJl29p8bIisXtJtVKoN1Jr8NCEWDssTEDa2+DbboMtuFS2OwlsOkdhO2w1mZYcyNhFshaQKMJ"
    L"8EBIslMiyrf35t44Prp0PoaXmTltPvJSb2hGidOOKVOpgTwTsNLhleYOIT55B+KD7wMOPQAUA5gZ"
    L"gdFjwoPJnyoQbXpC8EMggEoADLrKWtQInDdYhN326wj8DnjTtbvam7bq7whQh30gJn0DsII/DRYL"
    L"C4ZeHzFGngJIkYHllFM0aHEE5Hhn/lRA34EteuHE5A4lJvm0S2BXZ2JscVx7ojEuuCDocDZmkXM5"
    L"M/aUERtXl33aQj/17znUe/Kzh/t7FiL4UkvukkKTJRzogdV5tUzYyDl5/20zB4qNeD4ewBvSX4G3"
    L"HID9acf5HuLOEx5uJ2aSJ4oYAkxeBhsxGm1g5lJ0rnkdBpd9OTpbnpdSmAJy8vNpoCFg0jcJUxuB"
    L"qU2w9hagRbopGfWyBqCDQH6X9ld3JTwuI615GPlmrNjWqcGh0wvbADH8KluURuUTsXOJ+wMUB+7H"
    L"4D5+OHzyLr4eLAIaB8F4OEGJXx0EgdjU5gFhRo+ExB5IsaYZ4DiBl6rDPFg+8z/Bjzjh5fwoSHXo"
    L"MA4UWypC0euG/sJcVnQXQooD41yiTgKLfCTg40AShwdBCtBTgTzKP381KN3Qn4p49Ph0irt0wzUf"
    L"Ahq8W1wc1VmZbmAvQcvInVRwTR472i/u2b944kQn+h0/lHt2IuJholU2shKxLghlkV5x+LEt956Y"
    L"zWf43v+T6b9x11OmLGFf0BaqQLQOgRrKbqUEcGhwQ96eSLiKeINLXhnD1/x75N/9F5j9mp/D1Nf/"
    L"Mma++des8cY/Qc52uupVMWXt8jahO6iSyu/+bZieFvhEEFqbgdYGQIcBkwxMvlR2y35IqIhBrL5r"
    L"MDOOy3xYVGRJ3iYHrp6oSRi12aAGFYc2aqphuq0df4y/IPw1nwzuRNJ/88BxwDKYj7kJ/XcJ4K8H"
    L"xm8EeioADwKYIQlAXHYnd6SqwtcNHLyPHwWz/Iu+6LoGNASJGBErBiEuLOaD+fk8djsZYqFx60yW"
    L"Fo8AFGxEzsWfCPwgoHfNhy6MBzSXNwV2z5HKhNw1lMq+0hzZrfkQqCzZeUVxMhV1oaHRHJ/FmRk3"
    L"jwoTHp9/qhMPzg/KDE4hanGXdr3coLKhUmlaN7u9QfORx/derd3yQ/GtaCfd4awUc2f5juMeECma"
    L"+4syMioVaGAUsgCixS8bKK59fZz6e/8lhCu+DONXyDJkjSay3S/H1Ff8f8LUt/92sBe+QXc3uqdv"
    L"V6ajkANKJiUVnw6sxScD4tTg04PfaQN7DKU2AyEiqW+6GOIqo8iibyZhJad3yMT5paHLxVdT8SAD"
    L"4BOBXCTuYZk6ffxxvhr8DX9F+DTQn0diesFPaY5XTyo6pAjmtJ5cFH15Npg7V0Vg5/IZP8ZXgf68"
    L"Xblz09SOS7YoYSlkUZ+DIsTFxaxYXMj502DGmz7zWoIIN08cYUoxRmFomBpxwX4ohvrjpInMyCLG"
    L"F3YFvQ6UIf/CHJ0F67PRxVkJhVZyL3/Xe/hwF/3I+4lxFaxcTG4gPg9i1XFo+60UjKP33vqVx9NM"
    L"+OJ0F24mgA61syr3NDOCRgB2aA7wy1gTJNLeIum23JuyH2x/6aD5NT8/OS7qBCa15UoKNujBS2sW"
    L"zZu/PzRf/6uIs5fzNicnkkiHLgJnoITPppCaGxD4amB6rVCCKfGSsWsBYCYbYpQYLuMgWSQyox4S"
    L"/wCqpKWsAflgaiXNn4IKORtSEtBEahSzP5qfeIJPBB9EfOIOoKeDM8D4RADOET7eJowfC81pzoFj"
    L"AegoEsCLY5Iv8KNg/MRvI6Cwm1941VSeU1cqAg4u9vpBh0DqLWap6GeSSiSgFw4EehuIiX8Yucix"
    L"04p7BNBpz15VEKgfXH+FivIJydL2SKiBj1oTFAegdt1RjcWrYWW/tcb5g5eb3zM++nsP8BP/wqDy"
    L"y+AnlP1aDGYwCyCzFJeCkj611n5AZQzMPfnIlQ/NTV+tR/8fKt6qjUQTI4iUS4N5q6zFKROkbHuv"
    L"IiWIqtSgLcnW1/0ib4duPKyMAzVu31JryB4Stv0GtL/1v2b2Ij4NyDl3cimkBQtkr+RS8uuVgK8I"
    L"ia8LKcthZtCfoRnHMDSnE4o5MBLkg/lgIl2BDGLeWUs5+SqSCwtMnqkGAlXhviSoIB3bi/jwR5AO"
    L"3IcUuU4cJ3Ro6YBy4EFgTYAHX9IB4Q6ssi6RDpG09w60cstuvOnKjMnMU6rqsCgsdnqhWOxkqd/N"
    L"YiwyWvOjXzQj4R5KzJnxSYCj4HKwQMMV5sInyVx1pYouLKwkHONTb6x1enI5n5zZ6Q3PA43l5vaM"
    L"D1v/9vpSp7pZIYUoPu8IQsPE9kZV1bLyZoChjvh7H7r3O7SBvju+HdPgHayyGc8ET/jE5RJILgNh"
    L"spQQDmrXfDKKzdf1bfZScUdAuQWGi/iUDBppOdW8+QdC83X/EXHjFYW2szN9+xqgLcp3a8v5GtCc"
    L"QeATgeljIhMtSQaDGQACSzkV2uqWWE4hQY/3SWkBihMrKlaIDLZZ1JZ+CWqRWRf64zTLFkXqL8U+"
    L"4sH7UTz0YaRjT1BGp9B8c4BjU/IbnwQECIq+gQOBLo8xieKOP4T15u363Vun27PTun1zmJLy+X7Q"
    L"5yHQyWKnEzDoB3ID7S1ANUaXBuyzkCkip6k9wlGSBkSPdJeheEfRdIYSjnJIPy1iBQca0NPy9xwz"
    L"YvzP5ogYtoSJkBrvEyuNQNuslPEGQGJ8sHvu/vRr9seNW/XV/9XxE5QmAouQNnjdzXA7sFuX1Tps"
    L"kKV7DDcjPCHKzYdwzWvHu6IB4G5UyQanv/xp4A2/noUXf2tUH2UHtZ2cBBhfCTD2NGANHgohpzof"
    L"ibjVNRwle2nL8WqQjqwcD91ZBUS0Q8l3HdIAarl8QRdlPl/RFbiMfG/2F/mRkD8f7rkN6M0DRg9K"
    L"UyY/+F0AelrhGCHgAZFgMMlhSPOHUdz9TlKFvfLl17dIKIFTUaQUiwJFt2eDTjfEbtcQC03NODdZ"
    L"ey+q2CF0GY2ThIBeCaIOAoMCIOkKYDSCXoTMjBjVRbqi1ogCOLNJXbImGc9y62y5P/vz4tJzdbi9"
    L"OUVmeAhBNzdtAjJGhaJRYwk13+lN7T1w5OuN6/2txbu5LaqtIj1jJSDy7UKRY7VrIK/e9Dp/pJ7q"
    L"mwtl+ZWvmOxeCuwLBCPgDK7Gzd8Xmq//FRStbYVnqHdMBwwCBJ5YTHx+HEQ+A9OTQZ7DsnJpanVa"
    L"wFQRUj0hjtXlxEOWaOqoSKamsNqnQLkKjN+YxA3Imj+C4kF+HzhwbynUCunxn98EwI+aOgwsb3Cc"
    L"OTQwN2NV3PsBvpsdxJbpRmvnzq2Ko7pPkddg0E9xsWNFt8tXAf0jIjwEaG0GCwKn6cQMHIGD5koO"
    L"Cw+CyGOA3JWK8VLmypc8BJjQSuor8xWXcKp4GdapSucZ5yzOSWs4Fh32zNVxZuDajkmWkMvc/T9/"
    L"59fNod1+dbodz0sPUJ+elAHypj0iLKDEi/aB2g5eVVsjwfgncYsKu4vkFqdWBpixwplfehqYfsOv"
    L"Zbj+a8vJaIxDNwYLgcBk4s+E/pGQB0H5t/Uy9hlKTY6Lxc+QpeMU36QlgjCka54z2NDGJpK90ATQ"
    L"bsSvG0y/gw9g8NBHgM5xIGRARuDdX8mPnN8FSCfLynGa0aVh8PHfgqGwL37x1fwtlCyOWjFOnPeg"
    L"P7BBt5MGfBqIsaAe5SmZz9IgbKaBsGaTQhWOQzwEDUyMZcHIJVjgF4agU6BqE5X9iFgz0NOadc9f"
    L"RY/72Rx+FdbhQsYYzfgUMD6GbNgonBof5Dzv/k8dW7yF+wPfWvwVfF8kuas8o8Y0FdvBK2doK5U2"
    L"3tTWJMEiFaKVy5jflZVWlvCXgtYt/yxrfN3PI7arp4HhQGimra+nAb4GpKkNsNZG6GlAf33XoIiw"
    L"/4SJi5yyTb6HQC0y2cR4GLwbLLlcibwau5IaAvLpwH0y0NY9gUIfCQ/WTwMZEr9jQB8G+VpgBPAg"
    L"MB4EoH48+CDSU/ehnVl+/XWXZuKBblOMKAZ99Bc61u92UOjXgVgYh8ziWgb+4XqXPIOaBGEHesGK"
    L"l8E8ipZSyNQgyBcNSAGG8qpx2VpS08A5cfmuarHrXADVWZrPKcGs18AxN4bjtcRzD+/+A+SNW+Lt"
    L"2Jb8vxUCtF3AsyCV/SS+VNCnOBWUsiR5qUI+C3s1lH+IyCj1nHiWqrD7ZWi/4S1Zuo5PA7HuxEqC"
    L"yJjspsdsfiDE1GYEYT1qa2tzkNVUoUvTUWKZ0VAMgXxqjgK1V8KSyVhOBPStpkMlG7rVNxX6iQfu"
    L"R3zkNiT/NsCtEwzGD4QlNGFBrwPkm6H47J/zZbywm665pO3uqCscY4HY71tc6AR+D0jFIHK1yl5p"
    L"plnSKYiTHycaFlgZ1nL5RKTNMyAFdhksM4RAViLAAateit+qCs++8Gz2EM5m6CeFBAAAEABJREFU"
    L"Z+Uirv4Ylw0HdOrdX6KnjnZukZ9vLd4F3r4J5CZBqlZX2EhbKaNI28ucU9YVy+U6FHzbJHEFpurZ"
    L"Bf4EOPVqPg18/S+gmNpa+DzUbWK3wmYw3mFNHwh5CKC1Cfpbeko08IBAlZAw6rOk6m5lYpgYY0AS"
    L"8ivsoIZADSkTsz8fA0m5gKqqj9L3SD/q2wB/KbDj+qWA28cIfAJIPAigJxgHvtTpsODPilNZKJ8C"
    L"oIt+WPhBkK8BXev1F8Ng0AOfAjkDrkLiSA0aDWvqs+Z+YK2lMjJWLzEOVxMpGC/5skBLE8i6xqJX"
    L"As4I8KrUGCNLxgVUn4O5RWP8uA1Y88ObWbR6EFxsMQnLJ/+D9977ZQNmwS386r8tHqYeN4buH9w3"
    L"3mBVbgHtpLILtcEexaHYC7eaWE6bGViclhsTVZqKqsC5Ff3MobD7pXwa+LUs6duA7jzDgVR9qM2E"
    L"sim+EsxsB/SNgG2EDDBGTaEcG6vPdazN6GAEJCcKIzIMhOhKKHvxFQyyTFNPrMiXf1IwPcrv/TTi"
    L"Pv0HV1SCwe/+PASM3wVMTzAh51PAX7D7wp53zaVtqrhHf79P0WIxsGK+y18GuqngzwRF5FGTENm1"
    L"epO6UZeOYZqtGV1BbKx48SSlG9PfNo+MDmBmzH4WWAgwI4BXiqA3EquVuJrwwpGFszWVsYiLFHjX"
    L"KYXE1eH28uaq1YH9+16v1fz64q9dL3FLmdZS1oRyg4pDMHMdM2LKTHrOAcSCLvK54cqbn2jyiADD"
    L"Wbts+DTw84ihlYYda5Bq+VgCLGsBU1tg+g+N9GTgB0Gguit4dsAHLiNGxicG54skVRaJpSc8BkY3"
    L"3lQ1lIvJNGScQQW26KOsAfKP7EHxyMeAQv9JB8eS5QDHlQjGgyAe4rcAPglMZZZfecXOjCbyxHgn"
    L"xEERUq+XFd1OKAYFMx88AXQP513BRw2/DLQylIcAu8AqV3S39FzEgh0RUn2YBJoKeAaA4JNc2ROV"
    L"lwqXYS1VOS/b52pevjCKmAaQYtQCq0lY/u7/+BMHr51Premb4gO4vNjrW8SScZ2rxTT4TkF9VWwq"
    L"1Bwk8gTOI00DFcqTy1TJIxlnvYTLXoap7/oti7tewo3L7rmbodFpnOBlBmNiQT8X+iEwDfjPcUw6"
    L"bmlIF9VFGzODWCRhRlqT9smDsRMX5GN0kWXQH7JqPdkQxE31eFxMZerowE3zx1DczwN58RjDFwCl"
    L"WZYj8RUGfDWQzPiLwA1XX9I02tbFmOcFvwUMFjpZGnQt8ikg8St/TDGyq0g9dgKOKBFYjIBoOM3F"
    L"3RMHSINekQre6Qs64i5xI9PoWBmCfGHFK6woefYFZ7uHszLXJauWykka1wYVXXJWq4/tueubePe3"
    L"Ly8+7qtXGxpbohN3jTbkUh/cp9yY1GAxF5JwDHCDOKWDhC3S0hCQXK6sIlpO/Ux5ehpov+4Xs/wr"
    L"fwqxOc2BssiJkIC0Bd5Ilfh8LUBjGsYnA91xocTz8XFJhX3iYHTAqdGYacCaDQkrlhg1SJM2alIJ"
    L"JKkkykgnmJV2bMD4R3JhaSQ+AQweuRU48ijFBvA4N74OgAdW3Pc5YP4wNrcbrY2z0xTCLx659M8n"
    L"dT0FLOq/ESj47M6cTcEPgBjBFaU7apuBBTpSjM1VCkfFRS2iaTv0BwkDOtMrQaqMjNEJhJX9UOi6"
    L"NfZGWS3DKgXncX3W52Q85bmlwCXVIggwunh+s7F0UJ1OZ2p/b+aqGSzglsHtPDUSZMjlpjacNhOH"
    L"nsnUQSCgYilj7RsOICU97r0Ev6ju2GpJxXfmOaqya74CU9/53yxe+tLyaWDpODzB+NWdPy1iahah"
    L"2QZ0KDDhEucBBFoYYVQUD+dowgkw8xaGl/hsGIw1YEYsUBCp76lILLXEJGMEWRKoBV3GjC323YWk"
    L"f3yENi4JOcCngPjgh6iX8KIXXt0MwUjTIsk8IupbQLebxUGfXw5dpORVN1GXqyU14UKc5uJhnjj7"
    L"FHl8MPv5TUCHALSx6AryYazNG1j5otLKwgtIwlid3dkk8NQH1yFFba+ULJ421o899NCXZRbDLbz7"
    L"g5fRnAhyIOzAFTUSBqtqHgak6mK6AyYqkVEh7kC4trgCZ3DzlhhjVykdYzzrpD8NfOMv8GngpxHz"
    L"dvJhqddqKIlJbrrL8ikAPAiMWE8DIE8yKDg8KKB0gPEPjStbUpxiIs+I1SJmEaVcS5FsJlAigHEb"
    L"8iko1cpag5K+0ZN0kIC4/16kJ+4Eqv4ty1Hs+TQsJdu1abrlbB4CMPBKPACKEPkdIPa6IfLVna8A"
    L"kiR2zSOFLJ4RiWqQc1qsVurNnHRasUMOJ+kpoDD06YwkODP3UKt6Y2lVKzHEKhPiVQ0nNM+Pxlmf"
    L"D1eX6wIPLAmzFIgULB3SJV+tGjTA40cOfbmRobs/Km3HpLUBHZxBJRYjn6jcYyLUlgNxRE/wyOAO"
    L"0wOnkEQrwmkVVrR82oLs2tdg6o3/3eJlfBqQF85DwyBiThj850L99eHmDBJ/JQg6CPQ0wDuv1GGs"
    L"NUUh4jpM8kEWXA664q5nfpNgw0upnGpm2aR5Kk0SlQREKrU/Mx68R59AevxT1GMqqi76/o+WMu/D"
    L"dddc2qAKgioZKif7hcVON4BPA7RmHy5QGjNvEfViwHHwTCj5a64jdHylokChQ4BT0SbTHWds5Eu8"
    L"MQ7ac+JyQEJnDc5FR/Vcz2bf2pLeL4lyIby1/BCOzp3ccjK2tl6Z9kIf/7SltNlKQ9kYt5iNbVzy"
    L"2AQVEhW56NpIEHY2+dpkPHloQ29s0wJ0ooKVr1pxZY1nS1I/DYTX/lTk04A28JKuAowJr6cA/VQI"
    L"/kqgNvQ0wKeAxJkZU8FMEZCpwczK+XPDg3FSkwwKE1GCLflDAcyMMgIbBmMN1owm7SkA2PKYJyAe"
    L"e5J3/k9AvxAYxxD3fx6GZJdfuj0LgZo0N2NF2xgHVgz4BDAYBNpTjS/xKC+JE4Ke6CNf5um55K9U"
    L"0yP7GUl5gtAF0iDqIEG/FyE3caRx5pT6OHOr56YFl+LcDYyradwM/pCqUSwdjNqPP3H4RYG791WD"
    L"j4P6XFwjUJsNrayYwmYG0ZQMsVFTbE92S9znNJISkWxcd6wiu7QVYSPBGDlingOqcd1XBD4NhLj9"
    L"Rt3JhiPQcMG5Qu/cvPsbnwT0gRBZG6aDgfykz1/c9okHQeK91IGGzDaPS3K6nKmZc50PXpIRlbER"
    L"QV11V5JssJgYUqR/mkuENHcIxcO3Mft6KJ64F/qfsW6daU1N5f4pAMFAK/ZVJEvdXoj9Tkh8DQAv"
    L"ilhDzwd6gWceB54NgTNw9uoVjVlcxzGXnsue+A4QifsRFl24TKUwia29J3yhw7mYJ7eLr6v/nTau"
    L"hK/RsoHm6BYPPv5qKdw84HulLAVUFk9AkpuINfks0J1eGFxpB25KFiep5UVy2dZQq/KMcPmwGioM"
    L"Oeec8KeBb/kPWfjifxhjo81U5pA0ISJ4JDKAvwzoKQA8CIxPA97WB8JAmVHLMzS5tmJjokgoTxQL"
    L"ZtpEvMYbBl6sFGdzOyNjsri9yxLQOYni0dthaYC07x5yE664ckfGywLX14di0TAoLPX6IRX6RkQ7"
    L"upRnqkBjKhIinwtKAWXLFQo9f+uXygmdJC88ixL9JI1+QjrRSPBRjXg+iFHzQqLO2dSSTS7T0oGo"
    L"vdDpTc3F1rbt6TC2xiPQhkiMPvcq66qIQTJxfXkvccqowAIts4u9oqgubLu8wlru2jfdeD9SFU/4"
    L"uQiNl35XmHrDfwqJTwMav8atcZpuqwgw3vWNTwOJ3wTAXwms0QT4QU58zR0+S4MZo1YyPNHIcZ6Z"
    L"QVeqZSjbzvNjh8Fjo1LDUK/WJza3MWDxOAYP3YZi7120SHbprq15YPZnIbMswLViKkLR13eAHpeP"
    L"jxFaPJQXVUriNHVuAXyWMHdIXSvvM6TKwiFp0ImHycQTVCmt6gTL6YEjsIpzVtC56mStsX2mx5fk"
    L"kJWZTn81loHHHj/Ax/9oLxt8xjcn9cu1rVcS4jD1tSFJku1ykqU3LWHVMElEC0rpsNZec7ZXJdtK"
    L"JKuKeu4h23gp2tXTQJG3o4+5mkPSyEOGkDdh/EBozQ0IxMjZJkDJwoCxUNMcNMPESjwByFWIJWWG"
    L"q8AvMyLztvSs0oOwQE7AJXPsFZ8EeAjc8Xagt4gtM61WFsxCCAghMzPA+CgY/TVgYLxHl90CoAi6"
    L"gioutdDKkEJGv7SRS8CNRtpGkuNlbrMmvUIxMw5LylZ2v8QNLqTrXMzNGEABGOhE+pTig2I1f+Lo"
    L"jUR2U3HfUCdp7WRN7I/s8sA2i+uQzZ3n5BCzH27WhFpnuLuoZhV3KabovCl6Gmh/238KcceN+mDu"
    L"M9J8jBmQCAj8PZ5PA+DPhWjOwvSKwA+ECBlggeEwAklZegBFlwAzjF8uTuJ4JcJjC1DPWV6hvCra"
    L"jbgcJ56C/pERGmD3pdv4FGAWQkAeMhlzEMkinwIS4pJesaYrMzNC4JRIGX3KrBqDyDVAoFUW9GAS"
    L"sgxGV5URN2JFXVDoXExruCLcFwz3yvHsLczdqGV8ce9O7hma0UA7VYlv4H6Rqani5hKiipDDGC0z"
    L"8ZzlVaVPph8o5DlWWx0QLy1VN0vZz5n28GnglW+Kkd8GNLDkc9HICToEgv7REf5cOLUJehowHQq+"
    L"xSmvYmAmmpbephdhPWEx3lwEMlTElB7pOriIapSBlXjIJ1vF24bivg/B+Mi1besG5pqBue+HQJYZ"
    L"Hwbpoyj4HYA/2uHML+0IOrPcWHNnG4fI8+VMHTH78yxr0EeWmxmMrs7Ux3mjf1bnZmVYKlQ1uNfI"
    L"4JYp215zVEdPnNzSTc0Nzxu7+1MPYKW9RDP4RUuy4HudDPMGCRWXkUHscmE2JfI295vTVWV0Lt9V"
    L"87xEzZd9Z5ji00DBpwHwYdeqWZiR4lY2HQL6R0f4SqAPikHfB3IeDFkGBIJixDhYZafAmrHlgRGm"
    L"wHVUCap2RbIFmrNQF7ycL1oAPwCQom2caWchBARqBh4FzH/SKUuDPn/04QHAU760ADWwpsvM6Co5"
    L"4jMPscn9hG3tc4I51pCcoaCfLONAAp0E9+gH3JjiM0ieS1fhXHXOu4Bi7d1zjwzpekBHj8ztDjaw"
    L"K4q9cCGVWEpaVmoISPveJAbbSUlNLFosB3dAajm+eBRJX08BruqVmBUsbVfs5yrS08D0G/hLwUvf"
    L"4PdujVM/+2mOosEohkYL4DeB1NwI5NNIei2wjCoBoNwPWMUmGRQX8XQYjAAlSUR11gwSF4JlxJc9"
    L"JXSq2vmJvwroXwuanWo0Fha7fNY3MMnMArG0BkVAtx/4q72peUZAC2Zr4KMETwIwj2H8E5b8g1On"
    L"dZkB1mBEeARkmfmv1PJjUGhwYV3nYkr1tvBIcn+ZE0uqucXObg7Obhzcxw1YCqXoG6xscm1J1N6I"
    L"TQpklQIR3HMyoMxbtVztGlygqmYIq33+Q/NLvj803/ArSJuv5LFoMDPGUvMT5vxCzuNwGJUAABAA"
    L"SURBVJ0+BUxthPH7gPGVwKoPhADTAEalutCOpWyRr7gSeXuMzy6c5ZX4AjUqbGYoHvkEOBA021OJ"
    L"v/QkM0Mw8AwwC0iWEg8AP8mj4QyuQUwFQuLILRhdZcY/tA8G414idfriPswsC8EyBL4LMCIWMwOG"
    L"PiLAggviCmd7Fgykd8n94KT5rcZZZVWNqDM3d4PIKwaPl3zW2nOgoYPaBLgXEsQuJ1nLRQ6hthOe"
    L"YKpBJgvkzDEumCvsvAHt7/r1EPQ0oLlpGzNW2s4WckBJ35yCTW0GprfBPxSSh6BUrMKQZCBaDgA9"
    L"TXis1RRIVIFkHv+Kv3R5pab/XoBHEC7ZtSU8vu9gb1AMYGYCC6z5KwA755ngDVkAhtNfJ3tFh+Py"
    L"U4MZa3lAyMwy2hoC1nTpJ8JOf9DhfZ+2KTSykAUjCR4tFsMa3aypr+eC0lmbj41mm8AGS+KapxEb"
    L"GB/MoNe/rI1FbCkOc8dRK1VAJHshB53F4zJnsqp5FRYaOZKcHBZSJdtIcfewviBL80v5NPCtv4Ji"
    L"y1UR/A4AJj9/NIMw9PjPpDe+EmBmC/R/NraG/olyviZIlwtFRcaFecmXCjODmbFdlURcgdkYn2yD"
    L"lfGlPCm+xHHP35EXbffOzc1BjOnJ/YcGibuB79y0YJKlqI4ISdZ0QPZaSgKOdnoLpaqbKftDbjA+"
    L"BYQwvsFKpWXrfUfnjxZF0aeBZUDIYFmWCfP7JRvLGj1N5rk2C+dgAFb3mZLWF6oSxq5OpzdVAK3L"
    L"efd3wdCiUhKTYIkClorLTUWKfL8DkSxLyXA1keqNCiIdOAjXky+XeeuCrPQ0MPOdbwl4yd8vLOPd"
    L"P2SAGUsGyxsAD4GQM/GnN8M27IAOAuhvEpKPQH2mgxnzkocAQziKN0jWcaxoNZXwArK8sCfH8dg+"
    L"GWDr7FTLyJlb7BYHDh/RYwD8K0SKlsVB4FOChSxCOlRbU+kPUnGyXyxoWllAoBP6UEkWsDZXMcX4"
    L"1PGFoxx7MqNthixHyDMgADBcQJcmdFanw40x7FORTMaARg/scBxHjpzcnTHyV8THSl4qkW86kVXb"
    L"Hy9F1yCZK5HBjrjLvDhblZlq55kZOwbBANaortrMWZOiSuP8R1Ov/J6s8c2/hLjx8sJ0h8+4tZXc"
    L"GT995U1YYxrW4neB6a0Is1thUxsA/ReHQemZwairKHj8RRDMmK4MHpMG5n/IdGwiIL4vDVtURdzz"
    L"CTSyEFrNhnG1cPjYycHcyU7pMoEqhWVJ3wMgBwJarq3Mdfu9bhF7MgqA/s5pCGYspMnAGq75/qB7"
    L"dLFz0iyFLCFkhizLQpZhcq/iPL/WGI5nbpbGfWJyR8wHKj31pcSbilh1aBc63a0B0dpx0ZMVSVJa"
    L"sSTSLGN8ycTh3qGQhZuNPOqWdlSl2Dcg2SlKTwSZRCouo6FjMmRKdEGXsON6TH/7r2TpRd9YwHh3"
    L"1x0+ZLAsRxLNXwmsxcTXdwFCaG+Ct8lnKoCKDCwjpTASGD6MX2ob+TXPTAteMYjik/fSPmHX1s0N"
    L"40YgC08cPNzvDwqkWFgqEi2iBQoFBrBgzdfh+WJhELmJDJZlCLkhZAEhoN5lWPayMe6Rk73jvX7q"
    L"GO0ycNZGyMDhnNlY8By+wtkYm012orUGN0hAspgMZhGM8Uip3+ttMTPc0L+Pm0R8KzEtSUEgrpyU"
    L"wFYto9CMFdtDRdKmBjuVyMxowMLOWVNStstanElYiT+pdX62pl7x3Vnjq38Ssb05IvAgsAzGJwFk"
    L"Tfg/Ra6ngektSLN8JeDTQOAvBmi0wfsiwFVLjF4CWINrxEgppmKAlemwHYFJi2uggzYde1IG2LV7"
    L"m5Hwwo+B6cmnDve1HywW2puB2WZBFDWGiqSXK5UavBseKse7A/8eIH/uB4EXzFRw+ivxFHryRPeQ"
    L"hpwF6KMij0fkFk5vuxaN54LOuZmKYdgvH68SWxFjV79X8AkAXEJUFzeTNhRbvnm4IiQBg1+SihZ2"
    L"kVfcj5VXrjj1KGWp3FBIFov8EZVtyk2OyLAxIImKjQvxCpe/BFN/7xdDuvrLCuiVQAdB9TQA3vGt"
    L"2UbgEwBmtvu3gcADAfovDbMGTNmgpGdghnFm4rNZxtSJUVUtjf/LQXomaLeaCvVQ4fj8Yjx6cn7g"
    L"xikiGCwjGAGCoebpiIhOz3oLvdQ1qnLDhSxEzooujZ7IW61Q38Wdfuof76fjtOBQkHEsGSJbLj3/"
    L"q3qeZ3cmCdG0wtwBgb+uhGEWlsMYxAGfAGC7+RFQaiU38WafhpF3k0QJHSmpuVdKmXglBTN2oHYN"
    L"bAM0qHahfBjbwpBMAHBz3s96vBgbAiJ1JnSBgR7vp275gSx/zT9GnN7CLZ4BTBcz4RzGD4FBf1dg"
    L"Zps/DYT2FugQSKHcQh5DxVlxITZiItYsIhxUsa3SmVONyy7Z0qjC7u1Aw31PHenzAkmrzxLxqTfm"
    L"wNVPWx3rp4WCSypF3cWzwEuNM4B9c4OjfDPp0UTRyDgwDo2tC6CUq3d2JzIRPL2mFWYT46gWG1PF"
    L"PEemJFbyA2akAWKgzEO64pYws5JHNknKyIQ5Zg04E97mKULMQhVn8zCQTorsgyB56p6k8liRwljz"
    L"Qiazy78IU9/wf4Z0zSu5LDkQCMaPg9z6iU8Hxo+Beg2w6c3QvzVgfFVA0PJpbRhUFoC0MMGSsSUO"
    L"Yw5e4hHFp3jIJt0H2KCGCREi16PgN4B9B+d6vhbBLGTeA1UsUGXlQo2lQi5pPNrjRjKYXgVM9x5b"
    L"qrV6O/F6qhMPqfMs4AytV/d9rqXhXA9g2P/YSAa9we66ydjDE1WK3BwGQ4rAkAde5OtAEAI3HPcY"
    L"dxtrKonnyc0mQFuMLvlRSzrCNHJkXl+8lb76T33J92TZl78ZRXOGZ3QfiHwqJ6So4HN1Gm3oqQE8"
    L"AJShSnQwxh474lH0eLCy7TEm5pKASYnCGcCWmanWSLekUhHt5EK3ONEtBjm7y8jOMuhTJalVCv1z"
    L"ZKcozPdDr1OkrgQ8TCwYN4IaK4BN8DkAtk900sJiP3bYMvYxqUL5mZbnij7ncvaHwuglLgF3Bl/z"
    L"DTyZCxsfRUqxfdlgD1kGYz3cWN6iKZm+f1QlKRC4s8gmkajFRE8iE2k4QFeC0zIzUmbSSyCCAQTW"
    L"CeoOYAsuIA8GM8PFdmWXvZhPA/8yxOltPAT4IF0MkIo+UipgfDJI/DZgeQuJKcGwQRES9giKIHis"
    L"jXGmlE3o/ABpHRjdJz5fNHJ/fIBkqC8axWJgT833O30/cSLYhcl/rbIsrhSW29RHetDjJAJdLWu7"
    L"BuZTizhI30nDX4P6eaHC+ZzFcZYLxOXlevueSMv2nxn4E6B/wJ0cnCwZfVrDTA4qceKW020F5Imm"
    L"nplRWAKbVGAhkQikuImjg3wJSj5NWIx+VEiyGEY0LrrL+N4/9fU/HaKFiMjEVwYXBUOWYHol4JMA"
    L"+HpgzFAPTmJ0CaKTcT1gTPqS4aFnWzKhfLA4XH9GuWInk8wAizGlw93oH/Ey8CKT9cql7GZZOd/h"
    L"49wgdZYVjjEnXPBsMsv5mSM4e3GQesf70Q+SMZPzmhwuwFmcRWRf+sfAE3cRdwdOGQP3DcyMCUoV"
    L"hb4GjHZAYsJ7SzLqlnRyjZJWLwQv5JPJQrlqjDBtwcvISdyhDkYG2xCQNmHwMsJFWHQIZC/7e7z7"
    L"l68B4EEAHgJQ0jemGBH+osvYRcaJkfZlFSZBGYsxcJSTKksiIlj5d3WwY+sGfmggryrU9lMkpWjH"
    L"u+guDhAlohsXiV4Rao0ajyke62HBHY3xlpLLmE2oHFrEEU6Fo59gn7eNU5Lv7MzECkaQZfnezAxm"
    L"CUaxgIiHgdfcGCxJ9BjmYcBFIbPWlkxLnZB4xzJ5ok0qlShkIS2+eCS5V6kgPfdicNIRK+iqseiL"
    L"D5o3fW3gR8BYJr8OgoJBjEi8+4N3ymTcSgqhglmHp24LM6BGKONNBvXiUf1dAKDVbNG4NqJbytTi"
    L"LYAo4nA3LRopcDmFVgW6XkmubXKin76gO3hnkPrcvPpFYKVuTst/LilMBP4sDYyZaXzO43Kqw3Tq"
    L"EwA/0sCMy++L6UuPutYGMmNLMkK1V+iJuyNF7h4WrrTRIglTl5KqM6qNFflS01gZ9WnJYmyBLXOA"
    L"Lvpgg5QRLt6SLn1RjPoGwEM18SkgESsaSYcAI+zxVKy0Loy9t6VQAxdLYgFVkI48ybAuE1MpCCo7"
    L"Prr3T/ZifxnNSmMlxP2wRHRygF4RyyeKJaI1Nwcx8avomtWf04rn4gAA7+5a/yowETwNJtbWwD/U"
    L"sEoDpLl3mJyQhHd1MqArQfuk1jOYmKgRePkmTBiyvI2xizK1nK+B0IV8ugFpeEMaaghfxHDJ9dAT"
    L"gH8ILPgU4IdBgTIyhvpY91gyTGY6xBXgGsisSCM5KmKWrZpK9AaClgS8+BTQScRfaOGZFfll8QtK"
    L"4Mmxf6EjOrf25+QASIk7o5p3woiuWJh4AuCdpOZDJwGhXIBUN0sxmyISsTaisNpm0tZG9Barsm0o"
    L"/5AxVmiskoy8GkhSV3UJ4pfUxVa3tl6W8+s8wJ8DwW8AkbdS6CDwJy8GjrFW8hNBIBpgvCjiskFX"
    L"ySMlHtGpxVySUtSi0bjU4O9vxWLBx46yuXw91F5eXHO5pYqafqZxZprwM+312fN3Lg6AMHoC4CJz"
    L"bhHaLiTqEkKZgkaGoEJJWc3tIWwwqEAXecZGyQf3WmJLAtKyIQfCCSWfmAXlpX0G8uXBcMqloYkt"
    L"OEV4cTFiZFYqB3kb1SFgfAIQHj1QcyUVEgWXoJAJxBJoCcBIlxgrXL70oBqXjE4wuub6p3n0nlQf"
    L"GT7LlObITY0QDK3c34dW7PG5JgjnYEBl1q/SsZnBzJi9LDyuPX9ZGWzCyrRXuOgszjdjMld6OgxQ"
    L"yYXNRrZOVX65y9hJ+ZVAG1N2gxj7kL6g9MzaYKwv5tLd9/kCfgDw8T/pJhqhdkrECgzjXa+FN9lI"
    L"CiobFSIF1GEVT5HHCpeZ8SWA3x4reT+OjpqKdSqyU1njnNOIx1VXpDktl3F4nvQZE9+BjDzYucgp"
    L"H8/Tqc7qYBl8jx0rlpWHGxhIPiVASma0kiob2kzaNAATXW1qaAO5BqvEpDaXUUgMys0MfolFQjoi"
    L"zeiDcrLKws1bEuCeru80shXUkosbFwcfY6YzeiweOseqyGZoRIlfrhHAEMP4x9sUGsAWUOuozdap"
    L"RQbULwWl75I+TS2HQ7vldU8jXt5oGa66ypnrGSepO3/GTMoJjSaZy+g/V1kc8tkbGoOvuHEDrB4k"
    L"xhQHp66mHi04PO0HoqqkchNxF5moJDUeA9U+GepWRIV4o3dFyISmbIOGxqoulLugblcyIQ5IqJZc"
    L"jDh155Dt+WSjjmcZA8U9wfi7mA5WxYihKkWqqzWxBNT82l66MWu6BsWKPMqL2lIm6MAveWdan51t"
    L"nWWAkj7jZHTTyoKhmdZfAVZcLcZJaz222KWq/hpJSZV1s5E/2c03Upd3aa/FN+iPjH1jkC8qu03K"
    L"AAAQAElEQVRuCQYKmdAl8pobSJgS8qXsFOkK+9Mk+Sx+GjhGeUnFwSt3jcofxCq1Lqp6/hNv74f+"
    L"Irc4Q8hYeSKr4gfBROBKMR46EOAh0lsB1aBL2NskFD6ZkQRufLUj6YwgmakXMxL0ajA/JUYKK1ND"
    L"b2u2WNnXGiSBe0JJPwENCyuZPhf552KwjJunsAHBlyyCkRyLTrCwaGbkSMxN5TumwuSaSVZuRB0I"
    L"dQKbccNQ7sVtKh1IX74kESYk8ohqjquwITP5NDKMbQFIQ1fZEHVRQXzqQdin3867v9agnLpHWt8D"
    L"+j0k/yjIYLJwCVxB2MMlnnPgUVR8jTzFGLpIHzh8nB8V4EdyybJkyiMdAvo7o2I+HQjP7vZW4odg"
    L"fAowZJxwECRkOI+uZzdCSwLBtS73BJeXBJsAN1Ja+gQQKDQu/sGpqyhnA7yswuRoE4EY3EVmNZ8t"
    L"eZRQUMnJhYNkpCCZ6JEZNdkgT8dS6Y5t6VICMeqm8y6uKh3fj/m/+OXCzBgKAqfPNWNIIzDoIw26"
    L"MP8gSAFjyCWhjIViNckdFoXe2FrK7/T4eY/8sij5DRaEJ7cnXZYqz5FaSS/wQ4Bpn3G4+foTwJmt"
    L"TuK2WmrRmpp6yMzQy2fgSQnjjkqEMU3tJvIdiT0k1CCMt2lKVbcfspfuJuqYlIhdkf2rSU9Lii1p"
    L"X7jN/qN3xIW3/VjMFg5lfnv22HC+CuJgwOTvAPp7APz4yoWiYEkZ6pd8RU4sYUHYdoWaDHNSc6TE"
    L"wz+EjG/T5vJSsLY6Dj0xG9dmwv7hf/dEiUxYsdPxLWO08vd/dpMZaG/IeOdqNIxHAc6bi8M/62Md"
    L"j+Owc24BqxsalFmWTuQ7yDK/qQAUs/hGq7eFNmIlhV8UsDjpuqTqNrEOE3LKIrko8msXQ7mSX7KL"
    L"FFLnJObf/+uD/p/9bEBvMSQluOLFWHmM9JeA+l2g24He/8F1oEgIdSylXodRMgndVroVNKY3ZodP"
    L"LPQVZtcRwXVOMuQBAGgnOPPMK310OI2Vd8OkZdIjs0AwB9TXsju1EhpQ2hmU+DVY0qdBnHI9Vxlf"
    L"QITPfEo23B5OsAkt92jtUV7NjW0+ASAdb/IAkNSBVZKJgLRUKyQSpH2DuVgNcolYD4uZsT/zzkv9"
    L"oYj7k8qUO4dkiUlws1JIG9LOvLCr3n0fjnO//2MF7nlfrhgpJMaolWFIML73p34H1l+A/kow9LcB"
    L"SyG1GCaGR00i6OAQgAwzEwtmFZ7aQPeGfn/gaZZcWlbGJwDLeCMmPuMzYNxR6W7ZWqPQP0WQmUHJ"
    L"GzIQQ0ltWOMluywz2hHkh5AHoyecN9dZPQCWi4rxQyCXOglQXY2QdSwEHJrmT4HkaR8QsXCvcDNx"
    L"55AeK9WiG8x3oFRcyqZjyWtwRl2JWdJGW46hbtDPSFYyL+w6deZw8u0/W/Tf/SshzB/KjNP1CLBi"
    L"gemOyq/9qbsAEGKvi8TH//r5DIyfH8AKPsFpAGbm4G05IpCDcOnzoOvY8Tk9X4gswQBjNlrWTCQS"
    L"/CLT8doq7pI1KWbc/TlBk83M/O4fUPa1Fh+1TRZoW0OOgPPoOquD5b5QdLmocdVTsr1hw5NmhmPt"
    L"y3uJG4+FCcmo6glAHgR0Rk7Jd2JUmRaRvfhBITwSDakke/mj3ElXpju2dccqFb1BUliowiQvpLL4"
    L"iT8uFn7rB2L2+B1cl6To+fSMdSIYv/KnAb/2d+aB3jyingCKPpxPGVWGNk5z7YwcgeKspwDRknmb"
    L"AQ+bdgMWcGx+cSLXklEzCynLs2S8CaC+9BcPa/oLxHWHnsB14gpnBt7RDateDFEln0h8ZhJdIERO"
    L"qpKfD4jDPuvDHEWQXXODMeATLHIBboAnj7avys3M29xPzE4W3S9oBGegvNQeA5GlgPrcbDWtHNcG"
    L"FDbaGwXSFTa2S37FVEMkFUoXJNimR68vhCoe34eTf/gvBum2t2Xgb/zlnIwz5899bChWxrt86i8C"
    L"nROw7hxSrwPoMOB3AMXCzKg5WZTwslXETHE1ytkw0SSF4yU36a8P4eChEwOKyPWSKEvQ3b/ZiDwA"
    L"xkQuf0arwIwVeCJzHhnBeNWdxJpYAWfMHrqoZrWCEoDnsoRTOHvDM+PeQhkvG+FykZdEO88bT3Jx"
    L"0r7ZF0KXNpUszWgpBsETk9babGyOCgUjHvWpUwvN2K4bxGpRvbzp+ztApSwkgQ+ZCaE29S+UsvCh"
    L"/1EsvPXHY/bUvXk9p+EUSRgT3Hinj4vHkeaOIHUW/M4PHgjgXV9h9BhT19eGWH6sXCSdDQTFjQIW"
    L"yUooNwGueAHlwLGT86OVp9PEu7/leeKtOIHt0sZVa3JFHFeUlIJUomGdMXuV9I6ZCRmBe24oHyeW"
    L"+rZx4XlMc8pnffRaB/3troRqLyw3gmYr52tASIeH3wEY8kTNMSCHiZtgJooylQQY/6hW7hrKzaNc"
    L"1obVZh3SJMSrNByRRQuR3LxOldVwqPRfcs7POj71AE7+7j8t0h3vzPQ3++rpKE4+R37ks4KP+7rb"
    L"LxwDFucAtQmmdzElv6ZOQ2OcPX7GeDFwlszXgxUoAryiEJRrMYgdUTff/QJ6AA4cOe5PAGoo4iEE"
    L"hLwRiZPp4w9dymw1UA/GKjChR3qrb23j2Pz9n2qZARltMzoRjHxc+BSnf04mqfXWrrCVes+npp4A"
    L"X6ee2PJFhTaNEnc5XTPuEHkjcF8NVeRYbYHstSnNxJVK4vLDAboSSJeyskZ1UVAae60BV4LzDvlP"
    L"e+/7tUHnD38K2dHHs1MmoEDprt9bRJw/hrRwHJEf+3jbBwZ8AdcDu77409CTXriKSomsROQbo1k2"
    L"GD/5JVK7tEvQPzueT2/KDh+f61Pdi1W18ee/rMEDIG8kQNuTwO5dvKTScirpBZ64tHCVGntjhcrg"
    L"H/088ZX8FdCXjVssvfOPyy4EmtE9J9Mw7pFA0D1l2QG0N2x8mGuCQxtu8r8mamYjPS6w9pU2lQMl"
    L"ZNEdCZZyo5Fg8ZuIJch8nJ8oExCVhQ5ZRjmuhhRqqDsqtc+rWn+hZ+73fjTa5/43n60nh+5R5V0d"
    L"A37V5x0/EvxLP9/zwaeBxEMhaZWoaMYkVzzoIo3Hh+0ycAnGPy4jFg1i8DIz1iqGTL8AsL13/+F+"
    L"Qnk5Js94AIRmK1rGoYZStrRmkk7cseskBm8Y47rus2KM0xULQz/caDpASj9DaU2M4VPPzTHhsuRz"
    L"nblCiJ/ZYdfB557RLij7TIjMKbW9s+VOWt4JHlpsbs3npnbwBp4IVKUzFhL1lkO5xegcvITktNyE"
    L"tU25ccWniuvzTBg6MHGMNYFjKvlSHDbYI4uzzqNKd/2Tf/7vBv13/mwIC0c97sbxe2wUKCZ+4jt9"
    L"7JxEPHkIiY/7+o0f/MKfmPgmHeob46N4uR3bE8WqViJBkIkZacbO9cWgvT/BKYZsFy/8hgERnjp8"
    L"0g9390ATs5Cs0Yqh3S4sy1LQ6U2hzIiGZZismSHnrIj8QKA6vQzVliXG91nGcToEc3sdAMHYwOS1"
    L"wgPIpNJ52mL4zvrIuZ7VOjEvJ3ofXx0K8rz1UAghPb71S2D6Y8rNRArQhtSJQBZRgi5tOLksWyj1"
    L"vCaHBdygcCw/GF6ym+DrjueMoQoJGrKQOC9K794Pxvn//oMxPPSxXPNTnDRw0QwY9CEPfd71TxwE"
    L"+MgvGvyd3/iYb/qlhfMvk5aHqGiCSu0HiqvHkxxi5hLqS3YmuQO5yvaaZtOed4uv9J79h4avANCK"
    L"8qe/rDVV5I1GMssYbQOWyb5h0rPT8bu48QQAr4S1besss2HiZzRR6pvBsMq1zHBW0X7uizjtsz5I"
    L"Lqy2EvtNXHXjdgxVm6zx0tgwdQ+4Ik9su7nr/AQY/0AXTwCKaJxqjriTjhJc7vuPUm1l2ZNNG6PQ"
    L"mTAzaM9HNSUUkKYxddioHUx6l8ZzDuIx/rT3tp8q+u/+j8H6C8P1VeILoMONd/20cBTxxAGk7jz0"
    L"s57e83XXT5QrTpqYGWNEwlD+IVkWhgR1TIws0kp655Emp4yUaIEYJcfj2b7kmsbJhcXiePULgNyx"
    L"CwTe9fPWVESWReWwr4fbTlYZM3UccrU5Vg5lNN/KJBELiCYKdcH8B03V9YRs+Ua2PPs85w4Ddhbn"
    L"odgLkAy+Ntxz3l46hrw9q58CF/dve0WjlEmd21ObiiTtuKG8lItIntGpeyUNcs0BpZLzSjrRR6JM"
    L"UCj7yeZLCWsjStQnsDUqS9sjyXOF6tz+P/nT3o/F7OADvlvNzIdmnKdx+PppD515pJNM/MUTwKAP"
    L"02uAgME01x5VipFiyVAxt+lgJJLHMkZV7MxoXYP06FPIjHwRpRPkL/wqgLwn9x/tu29UF3lZ3ohZ"
    L"q1UEvv+H6m5eSSeQkr9O+kx2wZBnUqn6ErkKTM5kOcVnJi2W8/xc4521mY4FXaQO91NisRwzazTv"
    L"4a0hPrHtldyENKG1lplI+4gMFja0vwzGPcUGWXXRJpZMbUkKNpLrWbmBKSDFWiWpQhkU0WMg0qXP"
    L"vSruvx8n3vbTg+LWt2bWXwyas49SY9Z8dcfv8Xf8E3zPnzvKu34HetRPTNLExGcgYMbYUZcBVLME"
    L"d8LosFCBLekQocaiCbRLBF8gYbJAf2p7U+OAO8HgJa/vi/fo/iPD/7mGzmyzLFmzHbPpmWgh56io"
    L"P/a87S7klxDoOwvmSZ9xsfwQII8khx94gFOpKuN2y+2vSm1NaGw4a9I/H5QUs3M0TgPvSiaYGMCS"
    L"Vcqmp+42Mzy6+7Udo6IWVBuIK639RQ5g/AO/KGXxNrFYiTK5FAZp9sfNXT5FJIxdahC0kfV1ckzi"
    L"+qwmWM+VxvwHf3uw+Ac/Af8LPYyT4uJjY5CS/jt9Jb++7J88DPQXkfh7Ppj44Bf+4ZyqeUP2MDf3"
    L"inwF2WOmOz19qu0w7IiasqtlbLpItmO+FFfxm89/LTtIuP+xfeVrHVumnZDnKZ9qF6HRjNUJLHV5"
    L"G2JvsBpP+owHQZYZhBEmtzNPETwzV/bMuHkOepmM2LM9QN44qi5ShR1ZOGWNna8qTG+6J4SwuH/n"
    L"q0Ivm/E9axLUQE/15tIeFNv3KjdfFEGGkfZNKwVtVm+Dtbk/saGLMhYE+hzqD4VSeO5A79FPx5P/"
    L"9U2Fffp/5fCMMZ8LJwVo3vyKj0U+7usjn3/d5wc/f+T3yXF6xJob0XBWanuj8iWaAXEVYjUdhnre"
    L"Qrl6tFHnrix+RQgRjLJw6Y3QfwL8yN5D3W6352ZyZWbIGo2Yt2cGlvP9n7r1xz+awq8hAU/2OumH"
    L"rwI8CKQ3pqbmswf27Lk+m57P7gGQtLIOfqSm6kDgST0Mp+7WSwMQmo17Yj4d9u/6UhrTKFGjBifJ"
    L"oyQJEvejdhX54MYS8p0GdUGgXDwz0iIIJUkB7VgmHiEpfk4V/bQ39563DPrv+FeBP+3xudk4MwKH"
    L"bxr8gL+s9RaBuSOIhNRdwPCuT3lisJn9PiczIza3H8aIOmQCLoOCOQI2vUjG/mSTan0XVJXLuCbC"
    L"8QWgcQAAEABJREFUFcvRV7zZ/9bf/Y891Z0wy7KUtaaLbHamCHz8Z98TYvXj9lU1nvQZE78GCxpY"
    L"pbSk7+X2VaV5ekTjpY//p0uc0zt9bmic1XlUayLkT9mMq0eBy2ZO1FUtqNqpNf1J45Hx4DXfslBv"
    L"uARuMIJudvWGdnU6c6wqce+6ghoV+NaSgMCdVb4HU8amfE8OhPznUNFPe3O/86PR/L/V50g1V84v"
    L"8XE+8Sc8/ZSX+HEvzR2Gfts3/p4Pf9xXQDlBzteMdowbCB4KMEYixBdm24QFpIexLc2pTCZlfIki"
    L"AXpBedVytpbKFFdB/uKvp1bCfY8+2ZMhbwBAsBTyPGb87T/Lm4ltTNz9aUGXE6VO+BGWG1cxr1kt"
    L"TViyzqjEofZZTZFhr2eLOGez47r6+z83gfYGm6tMeWrmYQvh6IktN7Xm2zuRtHvG1FNNiyD4Oytx"
    L"zdauVSfCzqtlxGbm3kjCzIb73fWeI5Xu+if/+P8qBn/1KyGbPxJgLLByOkx+DPg4zZ/z/G/x6XGf"
    L"bU983e0VLaNupZ94YPi0OGFxhxNmUnubQoqkXfoXJQb5ZVHD+NZupZwLyC5QqlHGzLGaB15kGYWN"
    L"F30VH/9nc3/87w//AiBMc2k0YqN+/OeuoAkNJ8s4r0x8IBjAglOuceVThGfO4JTO3Og8sQjnaJza"
    L"kspJ/UMgxj0wsWQecK/GRtdoftJCSI/c+A8XZaiNK+wbQNaCSn1IalNTgaiUUMBS0qxrvmMJCBoY"
    L"RcsUOlqG+2yzurf/zzj/G2+O2eN3Zgk8oDRADjgp8Qt+R+vxw15nzv+jHf9PdfWRT+//rpM8ST3p"
    L"2R6O1Wnep4mHsyLtcpoooWN9UDiTFfmKt6uRJmeykGcan4yNIgF5rs9m75Yf8Md+Pf6zKS0iS+Dj"
    L"fz61ocg3zJaP/+O3btpT6ZQi16cwl2WUmnGJbAW3Q62R/rlKj+FQnnXi7M6Q+5fB91WpcSFe5M45"
    L"zVSL9saPmhn2Xv11YZDzY6D06UQbzMxd8kyI3PDJnWmzSkU7TVJBEsMrV1OLBCBzAXRJLhB9DiEd"
    L"24cTv/+Tg+Jjbw024E97PhYOTMkt6Hfgf3VXv+vzJ7406AD84o+ijIGra1ICbyxfKX6KEWCMH/2j"
    L"vMwYsbpJzOOCGgYzKxVUk1/aqlGCwTym4rtvssPmS7Hhea9s9vr9eNcDe7oyIxuJt/Cs2SwasxsG"
    L"IW9FsJ0kINSYpFyt7bsMpy791WDc72p6tWypy6XtWm8cn0/02T0AknbHKDzcS3zgg+kJYGlgT2lb"
    L"3uGn4r9LeTs7ePlX8Jm39EMfvnGV8AZu4pJNqiKEEvckQTtJ+sJi01AF/qTMDrVhBS5bsZKjFYXP"
    L"iGDhb367mP+9/yNmB+7PE4MDzUYDU+LrP9rpzjH5TwB87NdHPvDrPvRP6+mpANWld3/Z1FCxjbh8"
    L"IhBBmCiUano1UObmDJjVj/WME9lloboPjTyPoewokQ2RREKIr/lBfvwzfOq+xxYqFfIthcw//sXG"
    L"xo0DyxopRRuJqXGmJdW7WeOqjDm0iipRGo6qbF/sdR2ysxkH7Q/fTiSSOk5xhVWJko6g29rwft4k"
    L"8MBLf8RfIn2d6UFYQPIUR+zDHXjii6JP8aRvplrMEtQSQI5KVlk7sySfzdr/Qs9v/0iRPvWOzPqd"
    L"ACa/d61k1gc9/Y7POz74yJ+6fPRX4jN40MEwzJ1q8OaW5XBFi03wuatdSpDEIN+bNfYGK7blxRhV"
    L"kiByUJc1jGLlGvTnKn7ilk8NCe1b3mhk4K77HuuhuhLHEPibf3Nmtp83W5GTLR1U8tqvmFwy5yYf"
    L"gJPLVyPFZeVpWe4KTPpimRAubU8Iz9NGOMvjVgwJ+vdAuCWM35IiLBJrHFHVGCxtF1njaAqNhwat"
    L"za2nrn3daDPRRhvZHNPv+EqT1h73DUVae0h6VB1u1mFbTILxzks0KrIbtZ5xSh/55vnTXuetP4Hs"
    L"6N4MTHzvhANPeqznRz1/3F84idTlb/v9Lvwv8ijxJfdsHBuk7AjDOY+JSr+sxSOYAsKm69aYfJEu"
    L"Ek0wMYiHemybM+Fqxrpcg5GS8778+wbWaIbPP7Jv8fjcwvAN33Ld/WeKxsbNuvtHHggyhK6aqLF4"
    L"gqVt8VaD8f0j28QxSn+cr/ZSOJ18qf753D7bB4D6EyDxad0AM4Nx/xpWupasRq8x5U8BD738n5R/"
    L"l5wbXcYGqzwkQEV2xGKyjzLZqUJ1sVzH9diSmvNJCBcLx4cbleKqUFhRzyTqPfLpePK3/nGBz76v"
    L"etyvvPOub7zDJ97x08Kx8nFf7/k8DPyOz6D5JDQ5gcw0eB+mMRpGju7BZLBorg7kTpRaVjNlVtHq"
    L"wl2qqnnSJy01Jfy4T2OvUh1BQuubfoIWCR/+9H2LNPN3+WQhhbwZW7r7T80UsIw6klYw2XJmgm8b"
    L"p1eshipDYkXVMxXENRqcb2rPfKROGwHzWHKTmFTV4NMrlzxT0zeIE1UleUU6WszaD8eQPVS0NrX2"
    L"X//6PrjptAnpD9qwxrYZ/BJPpLAYwnWbHUJ6arN/CEtHkB14sCm8FJROJU/WJfV0a9315/7XLxf9"
    L"t/+rkC0ezaC7vgYY6ZuJD37k0896+k91/a6vxNdf8tEkNWF1TNX6fV5Y5mI7SMZTtlZ1nirnsxIt"
    L"qCdO1rhr2VklMyNBufNYscU6TcRMrgRmRr4AsFu+b5C3Z/PPPbyvc2x+QZ8noSvx3T9vTfvdP+SN"
    L"SJMkvqAmarx0/aVzpiBfiaM6E7t0Jsrnse45OADKaGnRGWQW3sdW+avArr1kF3TyaT4FGB59xY/2"
    L"YrP8RYD+fImVBNrIRkNB2QFcpjZ3bklLIAD7F9CwlJPZncvisf3kLinUgWAJ+0ybvc9/0H/aswdv"
    L"zaDRKPk1MN71wcf78i/zHEHq8GOfEl+P+pJJh8MTcgBgxkcpNswM+kMSfhnYFlCuMcsOY5faBMWq"
    L"5tKFk1IvCbgPv9NDFw3EEapAMoGkAtF+UE5tQLO6+3/krvvnfVyyQUg5k741u6Gft3n3D7r7c7Ay"
    L"FlCHxW8E9bKntdz9ZUuIhPEiX+PttdJjI/KxrNXufNM7pwdAHSze9KpxMB/IXLqI3vaKQpZ5az3c"
    L"t/xTsbmxtffF3+O/L2vT+mKzqjcyVfl+oZpJTn5Jka4IbVaIT2OzKlEqWf/uDxQVOULUc2vZjLhr"
    L"pvy/1f/dHy/6f/kfQv2Rz42V3Ep8vd/zcd8Tn08A0Ps9v+wnydW3wA2qqhqHVXf6qulD1Lx8flQ1"
    L"Ja2RUJES9WtXnLa4DuIJpFraShm0rmLjduIl8spuUF2un9ggGKX2DT8xqO7+i8eOlf/yL80Bvvvn"
    L"U7NFY3rTIGT5KXd/mk8kXDqD5K8N5YMjUQiEiM2xV6euqrMv1iqcjYnXCzLqyyZYbE20R3orUyez"
    L"9vu5eTsHXvQP0J/dxS1HXXohz3emNjJXXgXOo9gbjnmPcgWqVthltJfYYOh+6s+i6CG4HhX8lkk8"
    L"FKyNWPjYHxXzv/N/xHDwwQy64xvtlNhK8u4C7/YnASW/aL/rs3uejNSCmZSd4oCJ1b1AbGGyvFR0"
    L"PVQzKdBE/AokE1swtKlklTZDkWDK1kQNGQiTBBVYRBFIiU8wCYj9EJCEv/s3X/0PdGrgQ9W7v9yA"
    L"7/55Yyq2NmzqZxtmCyy5+0uHs6aHsqQzSf7SpKytRKfU8RTOigxOx2VnYAI3OM+qs3IAjMVEcfU+"
    L"Y7KJZRodzJmrLw28t71yMboxO7qI5seR5fmDr/k5/0Vg3KNodeaP7CK0u2g63KSkudOZHSJKMDNu"
    L"Ze3bhPDUA43i0U+Wgrqmj6QDgHjCsJYvg+O++3H8935ikG57axYGi/oVk6YR0GN90WPi8zF/4Sji"
    L"Ir/w9zkNHgoWfcD0VuEK+XiNbBYeYfTDQlnSeIjJJgPgNOAXeanyJRXx3Jx891Ux3ZeEalNmBFBR"
    L"CHQmOZsMJamS6bRMBEv7x/f86iDLG+HWex6eO8F3/9LEkt73G9MbBo0tW/tZ3ooWgovkgxGpb+Bq"
    L"ngKuONokp8idwZ3leqxYnAWQKUodjJjinBZkMrQ/rfb5qVBF59kffBV77a0yrmNdWkAlHmOugdw/"
    L"mPpQRDjZuexl+eFrvtr3Ye1JG9/ow0w1CRYpGMo/iW0zA5vQJZnbUmAVc+69/4UfGSUdA08S7kTq"
    L"jXGXJec+8FuDxd//MeRP3Z9DfdE2McGh3/R1p9c/zjF/HKnXhelAkIw6nsXyyD5SsmpYbIin6JE0"
    L"8tU0VmaqSZCfCG6gpmhig/6QoJApTHFZg3zoop7PX7SA/up24gEia5oOhyUVB9pJRofedHzTLXH6"
    L"+puzE/OLxd/e9XCHKoBZQgjI+Ht/c9OWXt6c4t2fPMCTPgq7IomqpDpxh+2KWA3FUrjEVcms6kql"
    L"aj09tJKP+V6Bzx+c3/v0vJ4bq7N2ACw/Pe40CrjHfBxMK7ZUMlW+OZyoKg+8VxWD6IlO48Myfvyr"
    L"fg6xUX4Q1OY1M/jF3VBvYm+z0vbXZpUelEgEg1ECv7OJb2xne+5oLP7v/z89uKismAmJ7+XQP7aB"
    L"SVGpABSPfArH/9sPF/apdzDxNTrwkZq6etzvLaL8Tf8YE5+/jOkw0FMFg8DeWajHUrs2gLYgn1AX"
    L"Z7IhPaKhrvhsc4iVfuIsRCZyhYlEVqB5Csy1KGPMnKZcWECux0TYgTLvT9gZrAz0YNC/999681s4"
    L"k4R3feyzJzv9ntxT3ZDpL/1s3NZrbdrcD/wIKPN6KWuM6krLJb8MKvlKKJ4isHIPSbAG+1PMh4xy"
    L"DYfNJQQnjANzPTx1oodOxzfGEo3nbnP1mT3D4+YasGgjMuNK394uybJe6yGgNZXFkYHt39+1g3ne"
    L"tIe+8VfF8s2onafEFzazchOzW28Tc1e6LgXEY8OgLujB9UgXH/xN63zyz8cUwAnwCFEy6449dATE"
    L"o/tw8h2/NOj88f+F/Jj+Qo+RSVP+rJd0x+/ocf8ED4ATiD393f0CiT4EkB+q0ntZRNdAN8YxlQJq"
    L"asdR5olOpsbqNHmUUpPjkz/JCE5SwThvY9uoISBJShY0ZAF1Sl+0r2jZuq7kNDAz1lUhL2ksXAxa"
    L"IH3nz/fz6Y3ZQ3sPdR/Zf9D/tqY6CHme8vbGQXvrtp7+9l+SDz0V0A1NNQBSdRltSbpX9xqWC9V2"
    L"Yo2V+1ZVGYosTcfmUDLWXEeMxiej3iDhyeNdzHVGO1f88wUmZ/OsjXoY8MQunrE+65A/Mod93Wgx"
    L"Xv5SPPXiN7ILFvXk3Sr52a6KaUeKNla1Tip11Kx3mxl52txkDt7xs3b0d34CceKnQW55vsODTwNx"
    L"3304/vZfGiz+1j9CePC2PNHW3/H5aO+/4S8y4fmoD/2nunwCQH148M6vYVg9JvbFUXlC0Ds3f8mg"
    L"mrPJqIZn3lbNPK2sS10XiCQo4WXjPGqVPuWelAypIzSuY2bUNCmVQGMdCkTeVsIP22SaUZc4vOwb"
    L"4+wr/nqZMFgAABAASURBVF7e7fXTn3/kTn7RJJPF+KFP/9T31JZt3aw9XVim8Bh7pvCUMnoXHCoM"
    L"iVOUT2WM7aylZnXyL+Wf6mSMUxtVrIixDsg72RngyRNd9PTfYbB9PpbJGZ2dGRRWPtQOe+N2SMMG"
    L"iTqxgVVeBahXl14fxWeOxAUzw9Fbfhzdrdf5Zi03tlwTWIwG3PpMIjZYzMhJBFckQ9lARAUVgHJJ"
    L"hRv3fRCLv/otOPKbP4Ijf/KL6di73hKP/dHPFnO/9g/i4lt/GtlDt+XQe7z+y7z5Y0jz/B1//mj5"
    L"ZZ93fvBnvaSv+zwwwIPF/YIX+1O3PoSKqbbBOAcCVTiMcjyiK1ASai48psiBR1Q894PRVXpgWwT7"
    L"MvoVkDNZKFNyD+1tTEyZ84XFF6YfaSSeTmHLbjS/91c47IR3fPCO451+zzWSZSnwfb+1yR/9+WGw"
    L"GY13fhfS2PPLG3I6Sn6KhsXFw9ZpCHcIRLmrVccclOQzs+X7XMMDJ3tc8si14Upo9gRgUPd8XuBn"
    L"Jhprmur4qpQGWhDGLKWxr8GlZLxe/hDAktOYB3Fx74kU8zzHI2/4DcTWbOXEKgzfw1DCm8HMoP65"
    L"eqVADQEMoAy8zAdHJjGb/s7efOR2ND/1dss//Nshv+PPsrTnzqAngPTUg4gHHkY8uh/Qxz3d7fXY"
    L"ry/7gwH87/TzcV/Jr6RJ8knXGHWHUQJqQwEaxiRPBuBVYoOR5jzYZCENcsijIWs1AHI0UfkRT/0K"
    L"UF80NOlUbZeRJxuxhjL69EBJJgET34Rpm//Y25Tc4dP3PbbwKB/9ZZsQUpbnsblhc7+9bUcvNFsF"
    L"6KM2j7LVWkBeBGKUUOsoRCXHe67JVbHbekU17wQQqlnknrZIf6lSXLLffI6R67QUOMults/l9lk8"
    L"ADwMWmne8J0GCqBO/qVBp6hSWg1x+GOGj55IaT+/O7c2bMHjb/h1rk5tq27hWw26tGiJhIBoWLhB"
    L"OSIWCrj7WA9FWvAaKBrxSXnS0KeSm5kOvRbw1kDMu8PwcV/eCByKmXEsBvdDlvulHxXxSilbkjka"
    L"ErQRbZCN+hWYGf0BZoZSQAT615hEEszYpqmRL9sazMSngDqoZB44sqSbmOjQpYFJLtqNS7vsB3+t"
    L"aOy4Ijtw9MTgvbd/bl5qNEXIstSY3jiY5t0/a7WLMHz0N3qogWTlUzZq1di7EIMw5JFetXA7oL79"
    L"W+5mqgSlnRRKatW62lMVQo0nbRglxkbxGYcYGNBJxed0a40Reabm4MGZ6DPFesVO7WN0CKz0FAAu"
    L"DjcTV5jFHXz2aMJxfn7KLv8i2/+Nv1wwY1BCiZzWMHynpmr7uWmlUNFCiVUNJOtiVvapTSo3Vnsx"
    L"akifiM5YJ0qYKKTMKEwkaiBZ6pAgL8kRtUseNxedq6bUWUYd0UadUhekUF6VjCY+PVW1jniCul1j"
    L"+ZGxt2lftkWY9zeUjXoRq5Tpzs3xZq96Y5x62TeGbq+f3vqevz2WlBD8uc+Y/Hm7XbS3bO82Nm3i"
    L"00GDR5HiUPpmo/RT+Wavpe+6JoOlbg3xcryhUMQymTpijbbdiCejZYCqp9Ph9BE5kaWQFQNOchmf"
    L"z1EWp/rsj+zUhYveL3OCWylw25RjWC7oKx0CI35p65ucpL7H3HUkgR9nkV78unDkFd/fk4wiL2Zc"
    L"H62et6p9WNFKHNfVgAU1X1htAelE+zo56Y2cqlRy+XAdToiq5TavZZWqdEwSKtS+NJqSZyIpJaa+"
    L"1xy3mZKIjlhkT1PX8b7IE3Y+bbyQJ2xGD6IJRgs4qFbLoM5KWyoMV4NsFbLkU+Q4hJe9Pra/+19b"
    L"t9+Pb3vv3x7r9LoaDozv+PnU9KC95ZJua/PWfsj16B+S6SsFfTEklRur8AhRrKFMdOe8kcoZUaM9"
    L"4tvNbev+FXPNeSlMKLEx8sHGkrLU1tuFnTqxJXbPpeYoMmdpVFxQfe3JuFtGgapXhWMYI9laWiaf"
    L"BJbTpX8c5zeojz0V+1qKwVf/RL7wkjeU6xgjc5wa7HzomU3fccIoh6SFdLnzSBGzkGAhYdQTsOUb"
    L"1rEqyuRL7mu5xuCiMRkHQbWKQV9suB/d5dW3NqdsnE9CmqIT7zjCZHkxM8fiGf0InEED0QL5G8qp"
    L"L99GJfFrAG19AKgu2muMspvgVwHPdj8Pre/7vzVNfOBv753bf+SYzlsYv+XkjaliatP2XmvLtl6W"
    L"tyOZdKoewac1kl7Ktkh1NcR1QwzCqKltKhjZUbxqGdmObOISC+mMw2I/4omj871aLUJ91q0xnEQz"
    L"klwPrck4DNY/Aio4p4LHDPA7QdLdoFIx3dQqeiVUZu/y0olFTaWOEA+BdOfRdFzLP/+N/7+id+mL"
    L"B2BnpQZrKQlIDovaBAMH5dt7KOHIKVAhQDAm8vYYT93I3KQjPhv0yFaiZyKVVLmkTE0bSYaUElS+"
    L"JXOaimZGQxKy5waUnK1RIV886QucVkWNYbvqk6yyL7Xdjr5LDkWkWaC2ZFWglfzNf/mOgr/nh3fd"
    L"9rkTdzzwWPlv/OnnvkYrTm3Z2Wtv39XLmu0CeUjGJwIMLzkUlIxUonJ0VaNClURohSSUaDmgA5ZS"
    L"wtcRVElcDb/kq5aSz5sE8ZG5Hp44sohCj5CSrwBF5YgmPCMjEp+YxiFGsxVMn5PsM4zuFzYHhpoO"
    L"Jv+mFAN5SsCqGFN3VEaHQPkUMJKU+VC26apSVF97TqbFu0+Gh7Msy+fe/PtRh0Bih9CmRnVR0VnC"
    L"3Io81ysBkXhMstKGbbofLi9lVCeT/ZOWj3GgKmUUUEk1ETeMWOzBGfBRmNf0QQXZE1V68MvMHHtF"
    L"O+mYjXg+NueP/Na6jlXRyPVEO9BAmKjky5+ATAWf+qQ0KEfVEJFd9jzUyf/Rux+ev+P+R7su450/"
    L"azSZ/Jd02zt2drPW9EB/+cfGkj+6YulONbvWVCfmKp5kguSJO7k9x+XSWQ7iBLOc0yRvpKBpFkXy"
    L"xNcBoFgI4AfHSK+mqq1VNzl2xXwJ2Po3gGGAViAC9wLTqpQm3hpNLwVlc1gvt2ijBTj1EKgNxzeJ"
    L"fDw6h8P3zmWf5VfpfPEH3xr7175q4LpSFLChbSLSOLByV5JZMsgxNljUrkFN7Z4xXGol6pNZOXGV"
    L"BPI4SVSXlZpGLkW+iSr1SgEwM9SXP17SkcFQFbcxNQDWxspKTD0ML3mvG8ZkrtpCNbBjM8lqPaqR"
    L"h4rnyUBdjWE8+e9+eF/nw5+6d4FrB1jgz33N2N6yszuzc1c3XyX56Ureh+AEll7aDGEp018fZC+B"
    L"xrUsSOjAORFLX0ByWOo20xaL3T4ePTyPhR5/pq3v5BHmnQ0tSsL3XhyjOXjFZSlk608AZZBWqPWa"
    L"q13mGczDV/eIcrWWMajiPSHxhSBnORnZVRl3aXhk3p64by6/M+M1+N7/Fha+9M2LUtQm0EbiWnLV"
    L"yeHusJKCmWmcZALGP6rr/BJ2nutLgtFFoSZJRCuTa0/YoYIEtFO/7oMCM1KEUpnd6nikjreNCqS1"
    L"0XxjkhZfyDE3LgjyR82qSEpDoRokEU22kz4Ojk+BFE8y8oyjll/pmGc4kF//xcM7/6fue3zhzz56"
    L"x0mqUrNO/l3dmV27O2XyN5JpVelAriO1SNYuRZa0+vMW58u7fSKAMMaupCinHYfNFYnS1lyeqn69"
    L"UVWpwkL6er+3euRXbIdQjk4qk7Ckf80/Mu5Loby7TJo+l1vh7A/OyhVixxn3XxLEZVaLcpWoagn4"
    L"IUABi0vGF9YZ4xWF0ntkLjz+uZPhTnBz2ut+urX4qjcvaoMbDCrgpUUlgmPa1bS31SAo0ThkUuBW"
    L"oRIn4A1VahITsa7kosQgGDta6otOpMg+qSDdMZC+y2ueSbXSqxwZfcIB5SW+VITdWA2JjMakFQzq"
    L"m7EttoBsqQ5Nah5x4xV/D+2f/sNUv/O/5/bPzkvd9M7fmiraWy/tzlyyu6t3fsvzZIyvuhCA/YCX"
    L"9Im8jPeRMHm3H9dzZVYxctgEksOiNVgOaoWi6rduCy/nezkfPr4gi9PD8NDggT2ii7HAnt7HudZY"
    L"41SfvWEy4E8/YGMbQwss0EhrDG4Ep6nHgofmw+OfOR4+GWG97PX/vDX/5t+eY//gQc5dRu1qJCZM"
    L"Q8lElglPhrIEvBI5FJr753MEaXInS60uPC5RmyBbsc1MiGD0Zo5Z+Xjo2TEogS7a1UNQcwjqf1zm"
    L"PjVGanCsrOmHCs5ni6Q2LCkoCdynuhaDUMua3/GvUuvN+iu+hr+67e4Tn37w0S6fcJNZlrLWdDG7"
    L"4/LOzK5LO0x+f+enwENJFyylQ3ZFuiwaZklxOEz+mhYe11Nbt30lv9NLKukuB1KLqpaAdMdZamss"
    L"mvtSgAR0wlKaDImyOaq5OtRdap+KOsgjzecyda4OAK3BMC5slLtlyJkkVlwDqS0R0pe4vqed8Kqc"
    L"pmSPLNj+jx0OtxXJeq0bv6zd/ZkPL1p7g68711M704HLi4lB0dgE4tYCGpiZOPCkoVwdm/c5VonP"
    L"pnTMlkgp0yaC7iIydqCyMGU+mDoT2J8kEtVYLLPSp/uRjYAKxpFpHqW+QUlV0oAZ28AQOz/p0EgI"
    L"0xv5yP+nsfFV34tOb5D+21/eevTT/NpvCCkw+RvTGwYbdl6+OL19R5e/+Re68yfe+TG8St/VMCDf"
    L"GmctZg81KZHDkBFJEVhIuKnjiYqxUryWQpHKdR7Xrf2M82pa67EUYko++FM91VYV5uQS12UpRPRk"
    L"Kh+CSvm5izTYszo6xk39MTgpFMn3nyLOO8fqQ1lpIZO8OdCl4+U3zUgPONzFiffssw8sFDje3LSz"
    L"GX/21l66QR8HpSWA/1CphCoBfklSthOMf1JiX9qMpF2hqsT3XU259MyMihQKuZA0ZdDMSUK0YxIs"
    L"ZZvK4jmQFl+07NmEsNoEvc8O23VfQ30qkPZxy7HbcjgKqPjCVFGRaX7DK9D6ufcVjWteYgeOnhz8"
    L"5rtvPbLvyPEB+MiPLE/Njdv6s7suX2hv3d5Dsx2j+PwQCI+BnAvoH9WlPipSKI3d+SkSC34wRZKC"
    L"EdJo2SrLUJfNRMlyANQBJUU6RbWjMVRD0J5LfIyRRkyRZ8gkJBTg4luReKJZsohB6BcxxMEIOBYj"
    L"MOTpFIj9qI2sIAhE16D2aoBzcWlwZ7NfRkyhV5dBMfS1T3oVJCuObQ42TynV/nD+OJ2cs5ZK8S/1"
    L"OhGDdz1pH3l8AQ9neTPLf/h/hP53/2d/JUDl0HxTA2Yl5bVkBCUU2aivcZnTFJiVd1Tpuk/aCUue"
    L"SBh1XEaaJIs4RCqVLkAeSx01b7vMmWQnmJEmWCJWYBKcz7qSAVb98YADoDp0OaY+kwWNb/rx2P6p"
    L"P0jZpu3h7kf2L/7u+z5x9OiJRaZLlvLGVDG77bLOhp2XLTY3b+uD7/86FALzRH5qoKtyNhUhNJKN"
    L"thtzDz4WjbdSEClQc9JOHELFZEIz8TABnX6BwaCgOWeUmLiVLjOchmSzM9VseEmUa85LgWooBjGk"
    L"WIQ0KEIxYFBpi7TrAAAQAElEQVR7A+KuQ3/QpSyZOl9q6+3ufM4OBPrQvRIoEONgtFnaFm8pUO2Z"
    L"Ler0mfW4ujdOiAGFgPuK+bFUPZ7BITBuWy9uGmMuxwM4BAduIAC3HsI9nziUPlkk9KZf/rp2/IW/"
    L"nYs3vmogP2WSUp8N3zCqaGNGHrE3JYvU9AaZLGyxB06OfFDu4Da1HZnUL5OfBtT20ZCtjeX6YhPc"
    L"wvmkiN2GmCKakFAoiWSjft0VVSGi4ksGXWpTpuRj97Rn4RizG74YU7/w/th8/Y8aEym9/UOfOf6O"
    L"j356TnSW5bE1u6W/YdeVi+2dl3QaGzcN+EEwwgLGk1+uBd6XE/StPisoWZFiAh+doUyrZEJRVQWl"
    L"btlYQnP05Mt+DE4s9LD38AJ6/T7V6YmFceQCJN75+QQQk0UCYmGCFKP7USxPAeqlFMs7flGEVAxC"
    L"rGBQ4ZToR8NgEOMS6Pe6GUU6ABrEAtGCcVptgXQF47RyUjyBaIHGKxin1Rawm6df5PDpWz9tS18X"
    L"3vdDDAHRonbryFnE6sOKI9UJKlWtxKe4JJdcnASYWbngmLhGsXtwDvvf+Xj6wMEF7G9v3Nxu/5Pf"
    L"DfF73jIXpjaCG8mt6MQxdzBZCcY/omu+2gJU/MS+octUEdQWkCwLBYnUENiWQ9kLiy+xJitaYrZN"
    L"crZL/wY1UV02fhhUfUlP4C7Foy0fcd1M3z5a3/mvku76+SXX2INPHOq+5X99+NDnHtvXtdCIWXOa"
    L"X/kv68zuumKhuWVrL2u1C8sbXLOQzBgGOtXwIn26a2GOhYgSElWp28KQgfNHazxkkZ9cQZxIKrof"
    L"2dVAFXacHAbseP/xDvYfW0RRULeIliKTHcSpsIIJz7nbIBaBdAUxiEcHoO4p4D0PBqFADIOilw36"
    L"vbzbW8h7iwv5oII4oA8OJEX2uQQGvY6SWf9jGYGSXlhQ08th8cZBPpaCDoRxUAC5ATxZhAUc1ZkV"
    L"OTkziy9Y27iWKXAHinBvbJHn5BlUisUq6u7RuFPBxYRFtrW4kxaKmQDQK8F79sdP3nYgfZK3/97M"
    L"za+bbvzy7b34DT/ZSzQWDG3py2libiYeEt4qK8+EkvSaOsJlL6TUroFNL0NhTVRYviqyzISqUSHx"
    L"fFy1vzF+KYPCDF1MGZhJIcH4p/na78b0v/1IbHz190F3et31/+Cv/+54pz/gT36tOLVxW2/jJVcs"
    L"zuza1Wlu3DDIm0r+0c98iU5TsuHcvS1eDcPEYM+Mnz+KC0ueYClFG1QQiR3giRW4Tg5U5e0hDiF5"
    L"h+oyoTMosPfIPI7z7k8+BFl/fp4p6b5TpB31lfgoeAOoIBUF9wJldJ4StZdC7LNvjmNAkH0a8CBJ"
    L"7rN+EuDxUh5/iXNbAkW/q0Suk1e0QG3htUJ9YEh/3Fa0QJtfWCBaEDglAReF1BqLDNao+kypcddw"
    L"A7i3FBlswCJ3JCav6AfbJG9pK9aMFMqVqNsl5qIlPr6lnMnAz9fsgwYspXSiVswEwP0n0/4/fbT4"
    L"wH3H03153sw3vP5H88a/v6Mfb/le3hRAJ9RL3IRceDbgl1i+kShwBoYi6CI7SZ/Yc1A8AdtCqAcl"
    L"HTJclxgMFajjbWFvUEB9q3ow871IJot0CKS8GI9Y2da6DBIar/z7aP+bj6Tmd/1cQmvGPnL3I3P/"
    L"6R0f4l1/fzfLm7G5YWuPd/zFjZddudjavK2Xt9pFyJoRweST7pImjxTLpEiJ6UAggwNhonFNSQcO"
    L"T2tLSFocYjgUKWVMOx7KPPYxCZw+kxNctxL02E5dbo9kA9AjFcDr+MIAjx9aQKdbsCvOKpaQpYLn"
    L"CPuLkY/uib5EE9gPNapxsM8UjbNAor9TAezcGDoGv0CWBsgwGGSpnzIUfO8pLHC9ymBU/TIWGMEg"
    L"ALSZBCWqQIkqPA5Lk1ztGnQQCJa2x3nyJb8C9W3sX0B0+iKD02t9gRoK9riLhJAxdhowrOBaBe3s"
    L"cY2Sjlh9eBZCShn3EtKAMy7XsjSlw2Rc3wYXphWjNcjgBuAmiuD6VUqnIHohT08Dtx6M971jT/HX"
    L"j51Me1szG7INb/xZy//vOwbFLd8T6YVaLGkMnFkx2LEGwz7h4JXBDOCmhjelSvO6cJygmcuMe6/k"
    L"V0pE8mewkj1Wy05cybnJqUFlys2Yj8Qq/q/1vvZNmP43H/Xf9cPWS/BZfuR7yzs/fPiDn7pvYRCt"
    L"aMxu4Nf9Kxc3X37NwvS2Xd0w1Y6h2UjIMg1HK2SITKaIkAglrfYIOLdAsEGEQ0xgEhLY5jhNMh9n"
    L"SpwnR1si0tRJ4Hpx4ik5lj7jZAyKty150mqq2H90Af7In+RjDLjQSJFDK/zQoFjT9/WOpAREZaGQ"
    L"fVCd9hxoTZNBfY2Dh1mpuWJdz2Ucx6LQpq1B4x2Hmi+s/S8Yp9WuE7pOemElvED0SiBb+ar7W3Hc"
    L"4wIZjLefNTqNPLPPxO2JwN3KzRWNq2XFSD5BRapNMKqGznGtIXdI3zJbAKwDrkRCfakLNAYxzsQ4"
    L"mIkFGsm0ydgrdwJLrbgEG9sC4Ggvzb/vieKOP3m4/8E9J+I+HgRBB8HUr94d+9/2rwdhy27qViUR"
    L"swPQPbxrlBcHCfEpj9xopWc2SCSXlWqqzTgb8STzAbLtNkaXpCWjqXQhTDAzNhNUi+cqtBGt/3Kv"
    L"/aZfRvvf6Y7/sylsuQR3PbKv85/f8eEjb//onXMnF/tFa8PWwcwlV3c2X3XTQnvbrn42NROzvIFg"
    L"DBeDDI0d7JsEh2Qccwn/LztfAmBZUZ19qu7b+nW/3mdjWARRIBoJon80ERWEuAtGFAXEIL9GURMU"
    L"JYogIYKSKHFDJAiC7OuAoDCAIvsiDMPAMGv30N3TPT09vfd7992lqs75v3rdb5hhWoL+JC7Tzf1e"
    L"1T213KpTdb57Tt1RPLV+CTLTgPGiniKsKQCxIkFjgt7Jd1THtAyFGL1g+WGEBGDw6B+EIISfGnwO"
    L"me0uX2c2YHy1eoo0ER6hMAbUw/6S7YBHkf9DGe0INERhvS9kZ71q7diSPA+ER23TwC/L8+EHty28"
    L"4XrUZT6fQR8+9ajnfeqNf9vU5z18PY96H2g+PQ+feSH4Bi9U/pKWzSwm3iZ4a0BTxNsMkol+VxLQ"
    L"SrtAB9UgCCbQU0VIGRK8APAjjjSL5ES4ZB01OwcSMLgn0thnCi+Krdh2ks/J1XQ5CidTVbljwD5x"
    L"bXdyf0+ZByVTsK0Hf0Q3fP1eUafekcqbPmZVQwk1cQmAuZCo6T2MLQ4JLl/gEy/3IFJqOiX8KVK1"
    L"+gqpeAOm6fpKKZTiwu1MjgQT8JUFdabhyxVaKgraF1P+kI9Sw+m3ShEI3vQhSXReHl67MYKrP3bj"
    L"/SvKU6mYho5d4tLiV1ZaXvbKsNiJNz5cfZXNQmuYtvK9EwyTFJ41A3QvgrzbDizb3qMd5o1qGCM6"
    L"wTwE4/YgyJ8DoeI03NZy3G9zQYztgefTdiD/5/UzG8gPsfYQelF/goHNhhfVGJWwv7DZZDuI90II"
    L"A3kO3sa2BRRTY6htZQEReUP+bagbfr2eRn2f92kdvl8PFL34yzd+8bVfgppYWMFe8BMNRCmFPZLx"
    L"RoeRK7/iL0QCXNPbc4MINLHWOoKXOqpIjYhwhZks+hEhVuwkQFTYYJ1tN5Y7reMW57gBcWgOi4fn"
    L"1soDh3pWQEpAvXcRUSS1hURCpJTmqVRN3rnRPHF1V3LPk8N2zVRCU42LX55p+sjpuvHby0S+eHOk"
    L"/u7T1v8v50j5PS7+B8aAFAK4O+hLpiGQ4SL/hxTPq9XzKdTia9faonJNXquGej6tAcPzZR6ZvV9P"
    L"+Q+cQvkzfiHFs++m7FFfE7V4H1rdP2JuenBl+I2r7hpf+vi6MMm2xi277xt27vtX5ZbFe8aFjg6r"
    L"s3kmvPFJaYUhKyHt4bt9DniuYJE8/LNxO1Pmt880pLZ/fR69YPSC8QnS6fogDhLFzwMieFRGRcgx"
    L"2e3yQjONacc/P47Z4GsKGtaAG54BRLQVMxlBJcZmeT4gBuMIdCDKgdxIpUpYFG/zHxTl9wZBPhu8"
    L"Ejy8gXpsm/f3Hl7m4fPbomYXGLZPPbzhe/i8h2/jgSrPTQk3fqp+ZvUUolq5T18Q9c5esNJLXOiV"
    L"p6DorAeGqa3gsAU7oPYcpt/qCfhypueGzEKcyegEXsCY1pkBTWqIRMp4qxhhRBUiCp9sfBhQMtZ0"
    L"piadn6Z2vklNh0lta2Jss3Wu0VpXZOsKSPPGWpwZ2JwQ47CKA5RnxHEgDocV6A/jlchw+pvNZsM1"
    L"3fH9P+tJHuyacD2TRk2W9np1vvGIk3XDaT/n4neetsHnr42D933JZvZ+HdXCBWxxhX3tQdj3In5G"
    L"dXjpNAhy8T+AT7HhapV8abD3gZT768Op4A3+i1dT6UdrqHDyFRQcdgIFi16hurdM8C2PrLHnXHdP"
    L"fNVD6+JVEyop7fFq2/6q16eNu+3F2ZZOEG82K0GQceL/nzNVhpgDEsa5jNIwDI2NrbZCREEGovZk"
    L"PQ2IIFOAL/P2gkkJK/QBGU8DBCwz8EuLKdXm4NM6aoL6z4wQa0oeJBDgggp889ptLY/6GA/uZQcQ"
    L"hkH+TwSD8+OB5qwfjx8nwADKfBWPWftBHYSNeKYoX4fZD4KJrVXOpLoGfEnA0/F83gEsxhu0N9Z6"
    L"6vN1eGP28PH8C8HX8ai386nvT2NMfkAesJSaqVjI3Ay8zJd5QPTfX77D/77W/2cNr+htu4DysiLU"
    L"4Ng2WGsLytmAHfuNqKFozanVMETtrN0KmyZBHWlqkDeBs0YbY0SThEFOBpVSPUQ8QMxjzC5yLDGz"
    L"ODYusIltTBPTmZh0YZyki4FdUmMWIV2UpGZ+kgBp2onntuO+NU1tC4iihDE0ps4VU2eLKCuAEHLO"
    L"SdY5l/VjHpi0lV/1Vtdevbry8NXrovuWD6VrNlV4i8sWXPGVr8s1vPMfdcMXr+Hi2fdw0w/Xij7j"
    L"l9XsKTcaOu7cOPfef+b8O0+k3Ds+Tfl3fZpy7wRm0sK7IEdZ0f/LvFOWcPHcx6nJG/sXrqTc8d8i"
    L"DYP3xDIwEfED64bclQ+sN+fctiq+asVk8kxcMtk9D+DSnq8O8vMX51VjqTE1VEqZmg1LyThuSi2X"
    L"rOFSah3mx0XIGoxztfWwjvOWXc6xn6dknHAgAAMwQkVSszTFSBgL6+GQOoH2AQFQj+rw94IfD1gM"
    L"5KiA+3qekYc3gN2MnYF+UDrT1N9PA40gmzZKgVHODk9GCkRE5MeEBtgKyKNDNKnJvBxmq/AYmq0P"
    L"QkV0ABLDs5zUiMRZF3iwE+2BeUy3x7iRp23BifHG+nzj9sb8YuDbbluvbvS14eJnevgEVRE8XSID"
    L"mSeAOurlmDFKXsT1P0oA2Yx+0o/BpH57+NxzEOFGZ2SeM6459USQRFkYJBRttDVRYJMoSH06A+fA"
    L"voBNUpSlQVpL44CtVRbWrcWOB4p7A6XWC3E3sesT5s3s3KhjngRi6xyZxOaMcUXjPYDItKex6Uyi"
    L"ZGESpbvEVQtiMIBdjYt4QwAAEABJREFUnKQpiMEuShO7ME3TeRYhRJra9iRNO1KQQypcMtY1WbaN"
    L"xlHBMOVHQ+Me6I8Gb1pfWflfT03dd+Xq8oMgh6eeHjbdgyEPD8c01rDoZYXcXvsHTX97eC73ns9R"
    L"5vAvcPaIkzl7OLBNmpm51/u8QQov35+iXHM0FNPkM2Nu+MFN6eCNa8Pecx8a67pyrel7eDQ/OBjM"
    L"G8m0LZoqNLdWMrnGSJykZAXDgwE4Uti4Geg765hz1kkDxl5MmBsT60qJ8eBSkrrmxDLIwZWM8amf"
    L"myc+KRorDc5R3jhbSJlzaO/7yjJzxkNYApFpeAODUYMgBCBvhzASglFR7Z6xDTwcUo/6bvXpdhCa"
    L"boOUAXTyWw0Pz8ZzcAIveKaIFue0Y9wTa+yF7SDCWgTjwo88Dxg7OSeBszYwHGcSYzLGRbh/Dpir"
    L"wsMI6Q5gY7Y14G0N2pPCtmU+Xy/3aR3e6GvzhHr85VXk4Y29jhQF9bxPPQF4lXqg6MVf+sVX/d1r"
    L"Xvihl09Cz0Mwruc3DrBBmrA/dzdJuiCtRm0mNQUbV3MmrmY5STM1RGnGzsCE1WxSBqqVrJlBUsV9"
    L"BfdRNXBRzNZWp8QkA4E167DjV5Eza4jtWmVtNzvuFXb97OxmVByF5zFhrI2sxVI7RhXOIl+wCTZ9"
    L"bEtJ5NqT2MLgXWeSmEVxZBalqdvFJLwwdQ7kYBYk1s0zhjtTZzuMAaxtxaZrsZZrBjRaserp4WTq"
    L"7t5w4Ia15VXXrJpccd7jo/f88PGxXy9ZM/XYbevLy3fE1FbZL7oqT/7gsdF7zvvNyL0XPzX+6LWr"
    L"J5ff3VNZtaw/7BqYSvp1VkYygQxnFA1llAwFSjZjpw8pxVuwM3Em4kZJ3IQIT5JIhYRD51wEXWDI"
    L"CI4MRmswd8MZmzrM35MDN0AMSKPBPFIrzQYkkQK11EkT0iaDFDIQAxet4wYL78E5l3eOc84CjP5Y"
    L"MtaxJ4oMC2PNEWqwQ7ghgAuwHtuBnCjCWtQhDHMEaAYCY/UbyaezQWE3Q64ZRu/IhzOsmd0OEIdd"
    L"CQsWcbQjGOTg8MmYtXLo0XGg4GQ6O+0F+JTFE4ugLc8C6w24bsx1I/f3Xl4HRkp1eGP38FPz8Ebs"
    L"4Y3aG7dHgoJt4QnAw9dxKPP1PaYnBsGLvfwgXmzd36eeZhufw1jAKPTj37YL8W+PtsTZVyXG7JbG"
    L"yQKTpI0mMfk0SUAEUdbEUTaNwul8kmSdSWqyBHJf5lErj3y9quY4FjbVUEw4SjbqpzTqxfO72MRr"
    L"yUTPUBI9IyZdxWm0Rky8XmzSbU3S66zpdzYdZGs2O2NHrLMT1prQ+D9rxBgOjLG51Liica4pTWyz"
    L"Sbgd3kQn3poIH3iRMQxSEI+FqXHzLfO8NOV5zrE/fOww1rUbx62GCW44lzZOJtI1nsTrR5Pq+vEk"
    L"7JpIKl3jaRmYgnyqezSa7B6rwngdsSIWWK3AmsQ5bEs2SkmCXRXBksIgkLLOyBTyk1jQiQwJvCEZ"
    L"zWgazSj25LAlULw5ULRZiwwr5hESHkWv48jPkIOEJFIVKwlgnGHnrJCzrK3hrLWSNczewAuYk9eD"
    L"PzuZIQdustY1Gcel1LlG1Gs0IAjIio65AfcNSAtADnqBB8IgBgmYlWZRGNIMSAKY1VYQDNlDkE4D"
    L"MQe2kLDQbGALwxWnFDtSjhV0pWpkYlnRNmBC1FjrBzl+HmBCIiABJxgba2anjYs9UWmsQQ0kolCN"
    L"GON4PsSJN2YsQ83Aff6FgFHUrlp3yHmD9gbvjTvGfQTgEzf51N8nuPfw9Rzy3ujr8H1A9LtdfqC/"
    L"W4vfsfalx+3/fSHeYhJD1UrtU/22PRSEZIF1ckDCvE/KdrfUGhiQbYUrirjVFpwx2TRNctaajE0T"
    L"nNy7gB3ygEnirE3hJQC1fJJoG8cEmbVJmLo0LlOcTLAJx1w6NeySypAk5U2chP0SV3slCTdo2B/y"
    L"a11SBTGEa1xaWcOQcRp1ORP3OHgUbNNNbO1mZ9MRa+y4TU3tTWqsY2NADtZlE+PyqXXY+AwXWlrT"
    L"hDtShneQynxraUECGMBa3BvqNEZ1WkMdlqndWWp1Qv4LRbOzUoK1w4ikKEJ5sZwTAxBlhAnxOIzE"
    L"Ks2AYAsA4oFo0JGgJ2IDM0k1UazIRVqpUAsIQmRSiZsKRCaQjinhsYB4BNgSkAMxuCFiN4zOtrDY"
    L"USE3LuxAQFwWYXgPnhw4YYvRWHFihciJYssZIId55RxLHoA3wI3WcSOIomRBDM5yk3UCmUA/hNSX"
    L"SaNjV3ROcK7CBbTzbXPMAo9BAsfe+JR2grmy0gIQY2bYPRgPzQpCyEAC41QwOhAA8pAgL1vBXkb4"
    L"w/AFCp4FigStACIHFQKovsMlhDHIjqCaB4Dn1cY6W+q7Qms8hTAjPAQCA3jD9oiQ90Zfh7+PIfPw"
    L"xGCRx8rX2tb7gej3u7BPfr+GL7IV+3q9Pb/5C6hqwsICwskqJdWUnPVzQKlQA4vMhyEckLI+yFJw"
    L"oFXB/o6CfZ0KXm4l6GSlW61IEwi3wOwyjkXB8rA5sMjYGQKgJ78YihzeAhbnAqkja2NytiqcJlbw"
    L"Nmfm2Jo0ZpuEzsQVm8STNo3G2IZbJAk3uyQc4CQaIBP2uqjaI0mlm5NwLcfhGk7LayWuruakus6Z"
    L"aL1N4x6bJH1s436xKcKKdJhNOmqNmQIiYy0uR8a6jCcHY/EmtNIIwmhOnbQZkY6EZR7m7ee+ME1p"
    L"gXHk852p1R0sus1Y1Q59tFhRLc6qZkeqEdum0QoXhRjf7zjH5OBmSkYUiEHpQBRpYYeNB9WL8j+O"
    L"yFOEQChGKUm0pihQqgoHNwTKWvGUVjKZIZ7QwhOB8JhiN6KJhzXzUEC8WbEdJkFo4ewoO/aHrOPO"
    L"8SQLl0ngPTAnYiUhi18nAnIAn4gGVWRFJCuM8TI3IPXr3ehYmhwTwE3M0shOvKwI+Uw5DomFG9hx"
    L"QQRt0Qcm411oPE5mBRG2s7d92vEPbWsWUy9BDySM3+cDQ/cVBf4/iyhRShF+HNRXB5PUupmtfa1g"
    L"9p/6EHxjn7eoZgBv9B4x8lXAG3wdXubLPHx9D9/Ww/fjgSa//wWN/f6NX2zLX536sfHNE117WTZP"
    L"eV5ME0MRvIHKREghEFfiXFyNmpNqsmsS2/1Tw29MmQ4xFPxdGuSPTFXuKKsb3mGyxbfbbNMbONd4"
    L"IBcad1XZQklns4yXBJZIA0ooCNgDG1wyFLBgD3oopMQWj2ch7CgRB8NwVsTCU0hTZ20iJo3FWRBD"
    L"EpKLJ8kkY5RGw5JEm4EBTisDlIZ9klR6VFzpFlNdx3F1LUeV1WzC1ZziPonWuyTewGnc50wy4CxC"
    L"C5NuqZGDNVPG2tBamxjrnLNOp5ZzieW8c1y0TpoMUwvSdqSdlmkeSGGhEZoPgCxknhHd7iRotyAI"
    L"lqCVSbdYkmYn1MQijeJ0EfrIixA+ZRIMT7JECsSgAlZ4k+IG81fY1Qw4JQwoizzIgRIUxJpUFASq"
    L"CqIoB0qmlMiUJwbFblyRG1NkR5XwCGRblLghZgevgYfY2VFmC0K1E865KWGuMHHIhiOxNXIw5PAf"
    L"JoZfPJIzJIxxCoiM8iTSIJgDSKARaYlZmjCPJmZqRB7gAuEPeZoVRDlyKitE3ovAuQM8JicZh10B"
    L"mSYhPNMDU0c/eAYe6bfDcyBsDxS9iEtQZxYINIiSmctXYOQ9fB57jlLc1+EN3Bt7CJlPPQF4eHkC"
    L"ma9nkfp29T58PxC9NJd+abp5wV78wPkXJ75n8tJjXn3AxOTAYZXyluvSNOwRclWQNuNP2DHszQRp"
    L"nGZBBIW4EjVFU9XWJKx2pNWkwyRmnzSxr0qtvNk4dZhR+Y+mmeJno0zpq7bQ+vG00HyMaygd4hqa"
    L"/1ZyDbuqQlO7ymRdJpuzHiqTcUpnXQBoEEMN2tuEEg3S0IFi0VrE7wAtVhTBBinBDomFTczWRAob"
    L"2hqQgzGTnMYTlESjlCKsSKuDEsUDkkR9nFT62FSe5RoZwHtIwtUurXjPYS0n4Tq063ZptYdN4gli"
    L"kE28mY0ZMSadMMZWgNg6a800OWSNkxzQYC3Iwalm66gNrOVDB5ABiIHVAsfBPAx4vgU5MKk2JwBp"
    L"pLrZqaAEGSBFbPgiEyHsUjnMD4biPQddIwfMHYdnqrYfEDbAQvDKI3FCYpUSg3VKUJqQ0pHWKgwU"
    L"lZV28B5oMlA8rsVNKMVjIm6UiEEMNWwmy0PEbpjhUThxI84ZkANPsHBZLFfEUUhWEmVdKoI7K3jt"
    L"Yo+LaEUM8iKPPHZYA6Q+xZK4WUGYq9VBkyPVJEqDPAR5gVfBReekwCJ5FpAD+UNIjFKY+HnAfPGo"
    L"3/8y4eQEWkPN3o8gDLmWeiM2kCcziJCGgDd2D3/v03q5Tx3KPXxfvh8PiF7aq7bgL22Xv7U3PxHe"
    L"8smD73n0nOO+dO9ph3/yri/93cfv+tJhxz153b+f9szPLzq7f/XDN2zqWnZztTrZkyTRECtJWcSx"
    L"3xnWKk6tdjE+B8aJdmFErhqRJAgnErOQLe9hrf5ry/rNRhc+mmQaPpEUWk9NCi1f8ORgG1rexcWW"
    L"v3GF0qu4obQ4yOUCHYAcPEASwQxZaB2w1lnAk4NM70bscMH7wxG2j8b3V60saZXAbY6xsT0idjZk"
    L"NtjMZpKsGVcmHhUTD1ESD1KSbJKkuhFE0OfScAMIoNvFlfUuhtdgIpw7hDh3iNbaNOpyNnrWJnGf"
    L"q4UWZtBaM8zGjFrnJpGvWvyAHPA4DoyTnHOUR9pomeBOq1Yj1GGFOq2oeUIgB1HzHal5LAilRLcL"
    L"6XZWuo0pgPcQNDOpZkFoIaKKRKqBlMqLgucgACwK9wHKApRpEaUCJRwQrEbBR6bAalIeBvpJSEkU"
    L"ILTQikLlPYdpgBhknOA5kONxqNCHEjiItEMivFnYbnGAZTvqUjtmnZ3AOk+JtRU4bCGJiwjkAHM1"
    L"SlgIfyIw01mQyQSlIAjasIYdpHQHQoIOEqQgRBHX7lhAntzCTCAT9IHufJfbAiI84YWvfBDkw8Tt"
    L"UEkRJauu/PojKPDj9BW80ae4TwBv5B4h8lWgnnpZjHtfz8O3Y9z71Pfjgdv/mUv/z3T723u9Bzuh"
    L"UC6PURCsyyi1UomsGX7srnt6f3nN3asv/beLn7nkjEsfOPPDp9x71odP/tXXjvzEIz857ZSnf/Hj"
    L"c9Y++PP/2rDywRsmJkbWlqtTvSn2oKdVxooJVhTBNpExpNKUVJKSThIKkNfWFpTjPcDD+wurN7PK"
    L"vNcG+WPjfMvJIIdT04bWo9N869GuAHLIl14rhaZdglyhMVMjhgK8h4INMnmXDTIuA2LwwAZj/5py"
    L"GDzmwXgz4oUDEeENpsUPK8WqJeJMwmxidmnIYkNl3BSZdFKbdIxsMox8LbSQFIeSCC3gTTzLUbhB"
    L"0up6eBRrOa6sJoDTcJ2Nwi6H0MKmca9LkwHrQ4s02ZLi3MFZM+msDa2zCVKLja6dk6xlyTuhonXS"
    L"xEItDO+ABQRAqhP5+Ux6AW6pbgoAABAASURBVAvIQalOpxTIQbURaRCDTzOeGJoV6UaQXVFINWCz"
    L"5ByBIEjPeA8ClgRgZZpQSmitFAcK5ABAYgNFBkgUqVhrqkJXVa2kTIqmlPCUCE8Q8zixjAnzKAlv"
    L"EaEtWNchYZBDaocd2xGbWIQXXMEjUGV24yU0zJIEGUXZrFaFjFbFjKbmjJbWjNZtWpH/Z5DzRQTz"
    L"JFRHP2iD+615ZJR/xgthYXNxn9X9/kW/fS3lkgch8etvkHqj9/AG7g2+jhhlXuZTX57i3td3SHkG"
    L"gvR/5cIa/a88Z9uHyKrh4cqzzz67sauvb03rvHlPqWx2mRHzaKaSeTCOw/sdufukPHGfK489NLzy"
    L"4aUDv7rmxu6fnXfZyovO+OF9Z37wn3/9tQ/809KTDvvAL0465KjlN53/9Sdvv+w76558YElf18p7"
    L"x6cmNlaSOExAChYQFoI1UIBXZQbIOksFa6jBGcqzw2uO9whE9tCk3kwq83cSNBybZEufiQstp6aF"
    L"tk9bEIQrNB9sC21vlELzLp4gdJCxuWyuhgCeg0Zea+81TEMpEQ8fWmDDgxxIFE2Tg4JBCNEMOSQx"
    L"WxMJm6qzpkLGTCqTjEuajDhTGWITb5I0ng4t0rCXEVpIUl3PaXUNw3sAr4AgorUuidbZJOoGOfSY"
    L"NN5o02STtclma+wIm2QC7kLZGhc564yFDpyTDDPibpECEzWCGJuJVZuQ6hDR81jp+XijLxDSIAk1"
    L"H/JOUiAOHbSRClpFUYtSuplIlVC/SKKLyBc0aYQWKstEOJQkqJUC5DULacKfUoq1QpQOEIEoFRnc"
    L"p0oRDEEipQVvRUAcQgueVCLjAoLAWMeY3Zi/RzewUZ4VSbU8EsXVEZMko9ak42xtmdiG2kmK4A7x"
    L"hKicpmxGqRwJRoCByfMAT6o0mmk+ZFRa3jJqCweMpJnXTCSyW2L9fInesu+urw00Ndy3dpC2/cMc"
    L"Jrt+fsHXIMNcqG7gdaP3qZd5xKjjjd4ThQfjvg6MCnf/i1dtYf4Xn7fto/yk3bJly0xXV1eyadOm"
    L"6lq83Pv7+8dBDsPdEOzW39/T1NTUFTu3Oj819RRl6MmMMU8YY5a5gJaL1k/13X/jjT23XXTNyp+c"
    L"9r3Hf/C5s395+t9/+vYvvv3YW/75kA89+NNzvvzw9eed9eTDd1z+9LJ7bxwaHekbLZdHQwTSEVaa"
    L"QRAKGyAnTB4NSBvJUTNQUo5w5N6aF9ojp4K/DoLMmymTP9aAIOJC26lRvuPzaUM7PIeOI1yu5W+k"
    L"oXk/VSjCe8iBExBagBQUyAHtnNZZrkNhF3tMkwMOLjXBnhTqkCU9HVqQmESxi8WZmJ0NmU2orZmE"
    L"1zBONh4VE20BMQxyEm8yaQUGP9XHcbjBRWX/1WI9J+U1LqmusXF5jUmitSaO1tu0usHEMcKQZMCa"
    L"dLNN02E2ZhRG4r2HKp6TOnaOhTUxZ0XIx9swbGoi8ptftSt4CqT0PFLBfAJJkArmaa07KdDtEqh2"
    L"URokQiAJBc9BNbNSjYLwQinVoEjnMdOciIL3oDLIB8QUkGDWRKQUixZipcmpgCwxIDh7UJQoRZFW"
    L"VFVKvHGRiMyKcGqivxpO9lYrk8/GlbHuarXcFYaV7qlqpQfpxjCKNiUILcXBEyMQwCz9lPIBHb7/"
    L"7vNev+f83V6316LXdrS0vKmxsfnIINvwiT0XzD95/13bDr7kvm6qphY9TF+Bos3V/pWf2/L47f2Q"
    L"VGfgjd3nfRpD5pEi9fCNPRj3Hkj+MJf+wzz2BZ8qKPVKcQgX7KpVq1KQQrQKXkNPT89E1+bNI7gf"
    L"3LO3d2NHR0c3NsPaqjHPBPn8k1lrl6PtEyqTWa6YnxxZdsfPB++9/ub1V51zwdOXnHH+L0878lNL"
    L"T3n3CT/77NuOvO3Mjx5//7XnnfXIXddd+MSj9y7p6tmwcnB0dON4lKRTWNzUCeyAKa+EGoASdmcL"
    L"MC8jNB9buC0j+ARBezQEat9CNvtmncm/12Yaj42yLSeDHL4S5zs+7oodH/GeAxeaD/DkQPliRwbE"
    L"4KFmIQcfVngQDiVFaxGNfY73p5A41mKFdOrY4tDDJMImJpeGEIdk3JQ4MykuHRObDLsk3cxJdUAQ"
    L"Wri40meTco9Nog1pUllnYoQT1XC1jcPVJgrXpVG1K02q3UmS9LrUk0M8aNN0izOpJ4cJti5khBaC"
    L"H7wttRLOQh15qKIhCKhJZ1SLDlRbIKozkw06A5BCkMks0D5VmflaB+1EQZvSyqNVKd0MlIh0ibQu"
    L"aqUbiFRBkc6BCrKKVI0cWIEcFPn9qWCqpEixUooJfxgHzQZnogmuhqM2CofisDqYhpMb42rYm0RR"
    L"d1SN18dxdW1UjVa7JOnzfc6GMLG0dtMEtRYC2r21SB99w270uYP3opMO2YvesGcr/efS1XLfmk0B"
    L"hkEYT6w5vbH/zkvfteKCL94FmTf4OuDNUAyZJy2DdFuDr80Dsj/45RX8Bx/E7ziAGkF4cli2bJnp"
    L"6emJh4aGQngRU+s2bRrt7e0dggfR37ZgQQ/lcl2RtaszYeFp59yTOqufCJx7wmpZHo4OPLrxvhuW"
    L"dF1//iVPXXTqeQ9+42Mn33nqEScu+dxhH77hswcfdc/VPzjrvpsu+c7DD/56ySO/eXhpz9Dwxv6J"
    L"cmUsNjSJTeI3YAaWUMJWaMkQdWaJFuUV7YatvAswP68WtGT0HqV87s35XOEwnWs8Ns00nhDlO74S"
    L"Fzo+XSOHYvs7XaHljZJv2ssTRA7k4BGAHLJBxmXgOdShFExMKdFBlkVnQA6eIACCq6KVJSVGmFN2"
    L"LvHkAMONmF2Vra2IcZOSRhPKpiNios04hNxkkupAGod9Ji732qiywcZT65OosjapVtckcXV1ElaR"
    L"D9cj35XE0bNJGm+0Job3YDY7a6ZP81NXlsRGyjqj4DqQZYTblANvFYKMasxkqSmTUa2ZnOoIPDlk"
    L"9fwgpxdgHeYH8Bw8SOl2HehWpYM2oEUpatakS1rh7IF0EYYPggAPK8qSUOD3CkifZoNiMsw2ZU9Y"
    L"YiOxNhRnJ5Wk48TpCBl4PqY62BDIlO9nNqRTI0/f8OPv/et/fPvcM79w7kUXvv9rF1/94e8tffqY"
    L"8+/u/8IVD29c/uzmR7zRU3X89IdPe/vLHzrjfSf23ncNCIUi9OdRN3r/pvdwkFuAZ4Dkj+fSfzxD"
    L"eUlGUiMH9OQ8OYAUkk2bNlWfH1rs1dfX19jY2O2cW1NMw5Uqm10eGLPciMB7ULXQYuDuJTfUQ4vl"
    L"F37p27d/+e//8dYvvP2j15741g/ddfHZX7n98u+efe9dP7/i1/f+asnKnv6erqGx4cFKTMPVlFI3"
    L"HaM2ZxS15RQtACHs0qBp75aAXgEsbtCtCwvBHm353GtaioWDsoXikZyF9wBy8AThySEtIrQotL4R"
    L"nzX3k4I/mGyEQeWsJ4cgyCBkCFjraSilxEPDc1BaiyIPmAsrBwOyisTCPr1hpM7ahJ2LxXGonAvJ"
    L"mkk2dtIlyZgx8RaLT5o2CkEQlY1pXOlNq+GzaRR2p2FlfY0cqlNroqi8OqqGa5OqJ4hoQ1yNNoIg"
    L"BtI02WxdOuxSO8qpKYuxVYJbooQRsbEOFCH+pkJWqWI2p0rZnG7O5XR7Lht05nJBZzarF2SyWZCE"
    L"mp+FLJMN2jO5oC0IAhCEbiNNRcLfbMbvZSTexvwWsCTihDU7rZyFJAVBJkyckKhofnOhBd3MepnJ"
    L"ob5gZF0PdT/4pDx6+TXln5317afO+eBRvznj3W996LR3HvTQ6e/6IIz+M4988yMXoINoG9QN/4/2"
    L"bY+x7nD9uRHADhPcRoB9gMiSyHnvYbbQYuPGjZt7enqmQ4usbA0tmHm5yiggUwstRh+789bBe2+4"
    L"aeVl51zgvYdfnn7UiT//0rs/cd1nDjnyxq8efcKtl373rDtuufrCpb+6a8m9K55+8pmNmzeuHymn"
    L"g1MJjYYGm1P8a5I6i5oWAC9rDuiVbRl6dUeW9mnP0B6l7B6LGnP7zCsVDmoqNLw3Xygem+aaTkzy"
    L"bV8xhfbjbbHjw1xsPVhAEPAedqF8Y0cmADnorAvUc+RAIAgJlHioTMCktEyDhIhhLrASUvjyjlNR"
    L"4VRZm5CVWJjx5nQh27TMLpm0aTJubTycxOFQEsUDIIQBEANc60pPXK1sqFYr65Kwshb51dUwBDlU"
    L"1iFdF8bV7qhc7U3CuD+N40EbRVtcakYkNRNkTEWhc03WBsQarkM2qwk8QA35TNCYy6jmfEa35QLd"
    L"Uchl5uVzwbwAXoJfTxEERbPAl80OhngajKOFBa0Ni4cnvWcO8XaXmKH7rv0V+q+APcpY9ymtdQVV"
    L"vDvvUXfvtzV831GKOgZwgAWmH4bMH/u1MxHAf7cWMIppgvDeA4hga2iBM4cx3G9GaNG3ob+/u/71"
    L"IlfMPQ6v4dFsIfsgrOh+0fqBZKj3rr57r79k3ZLzvv/MT0475+FvfOKzt37pvR9b8rlD3/vT49/8"
    L"vqWX/+CMW66+6Nzbfnnnjbf86t6lj67p6Xmqf3Tq2dGINk7EFOM9BRea5hU1LWwKaK/WDL2yPUsH"
    L"LsjR6xbmQRDZBXu35/ZYVCq83hNEY7Hx2Eyh9PG4oe0rSbH9JNPU8WHX2PYOKba+UTc076XzpV20"
    L"zmcCTw41zyHDWk9DgRxqmCEHAUEIfHkh9uaFzawss7MkNoU7nZAz8BzwXd5wxdokNKmdTJNowqXp"
    L"SJxUh0xU3ZRGlYG4Wu2LquXeuFLeEMNLqIbldeFUZU1Yra4Ow/LasFpeH0XR+mq58mw1jPriatSf"
    L"RulmG6cjLrETeE5ZsYs03BMcOHBWS5DVKgfPIecXUZhpNhQXv+LQhoUvf1uubdf9cy0LX5vJlxZT"
    L"rnGeb1PHXyzq3LW9sbDrfWs310VbU54cejQefLaXlRpVBC8GnyeTJCmjgjd8b/Q+9W96D2/43ugt"
    L"yqGr2t7xho/bP51rjgB+t7XajiTqIQbSKZDEOAii9vXChxhNTU3rU5GVTSZaUQ8xbCDLeu657vKu"
    L"2y7+CQ4lz17xwy+ceedpH/zYz0565wcuP+Et773xP0769HUXf/fMm265+fJrb1m65Be/WbnusfUD"
    L"/c8MlqlnLKbNUyl50yxlFe3anKGXtWbpVfNydCCI4U27Fuj1uxQKfzm/sMeenQ2v2bWj4aCWUvHI"
    L"pqbSMbbY9nlT7Piya4LnUOo4Soo4dyg2H1Ajh1xjowI5+E+ZAYihDgI5eCh4EQrE8BxYHJwKUhqu"
    L"tcNRKaUkNlXMCTuOLQ4NPTlYY+A5pJ4gxpKkugUYTMJwE0hhY1QJe8NKuSeshN0ggfWVMFxbKU+t"
    L"DsvlGkGUy+H6SiXcEFbivmpYHUiq8aBJkmFmW4vdxdPTLHjD7s2t+y5snr/PrvP+auH8jgM7Fix6"
    L"v27qODZoXXxy0LL4M3ss3v24t+y78Mgn+sbs6k3j26+8q67ovuTkMymgfufcpoR5OJvNTqJS3QN4"
    L"vtF7Y68bvt8XqPqnd80RwEu3Zn4e1LjOAAALJElEQVQT1DbFC4UY/utFEz5tKqVW44Dy6QBfLxLn"
    L"lruMWja+fvnSjXdecfXKy8/+lvceHvnGP5yw5JQjPnb5J9/y7stOOfqYGy/6zr9ed80VF1x18+1L"
    L"rlz6wP13P/1s78MbRpP1w1XqGo4oSh0FimjXJpBDc5b+cn6eXrswT297WQMdChy4qGGP/Rc27LHn"
    L"vMaDdutoPKxUaj4mU2w+ESHFl11D26ddYzu8h5aDGQShs/AcAIUDSR1knNIZrkODEDzgkuNlqURU"
    L"HTNZhFlCMCNhg0g8FZ4hB+dCC6QmLqdJPGnSeDxOopE4CofianVTNQoH4C30huWpnrBc3lCZmlpX"
    L"KZfXlsuVGjmYJPKf2X7rio2HhjoKinYvZenQvUp0xD4l+scD2+hTB7bTp1/XXnjffi3zVg5OqvPv"
    L"XIVj2+luYABTPN7/72u/d8JHVar6qhgHSkajKJos48HIe8P3b3oPv74WMp96IPunfWH+f9oT+BMa"
    L"fY0g6uSAkCIemvl6MTAwMOa/XkC2sX3Rog2FQmFd7Nwz2WrDCpgRvl5klyWDG+7fcPc1V6698fvn"
    L"rbjk9G888d2TvvzzU444/vrPHnr4jz72t4ffeun3z7j4B9/+t2tuvm3Jj2+6646bHnmmC27u1GN9"
    L"k7RmqEqDlbTmPbQVNO0Nz2G/jhy9YXGBDnlZkd6zdyMdsldTyxt3b9r9VQtLr3/FgqaD5re3HNPW"
    L"2nwMw3OwTfP+2TV2fJgbOw6veQ8NLfupfNMuOshmcdmal+A9BQ+Qg/cWMjrLAYjBA2T3HEeAHDS8"
    L"ByaxnhwI5CDOYbouYmMrOEgMrUFoEUcTcL9Ho6g6FEeVwTCsDNg43fJC633d5Rd99yfXLfnxxTf/"
    L"8tpv/ezRh87/9eqVlz82GF9wfw99/+710eeveNR9+9YVOjZuAl9wnlam8p1nzj7qr7p+9LnzjDFD"
    L"RpvhKIrGn2f4Ds/8szJ6zGfrNUcAW1XxB80Inu7fKC/49WLv/n7/D6PWM/OqRlP1/4Jyee0fRoks"
    L"2wBy6LvnumuevujUrz91/sn/evcZx5xw3efe/sFLj3/L+y4/66RPXXLef5552bU3XH7+VTffdOGd"
    L"j6+5Y/mzG+9aO0IrB0NaNxJRmDgK4D3sBXLYtz1Hf71rgQ7avUhH7luiD+1bKrx1z6bd/2aPxn32"
    L"W1Q66BULS+/paCsdk2/tOMmBIKip8x8EBKHgOVCxBaFF4y4q19Cpgyw8h4C3EsQMOXiCCLYjByW4"
    L"hfsgokEOTpyD1aVObM17sHY6tGgpBnnoadZLiauOPv3Amg23XXbPssvPuuLes4//ymWfOuy4b7z3"
    L"lQf859/vt/cPjnrNfree9NaXPXL6OxY9dNrb933ytHe+beV3jj9bx3psDEZfqVSmJicnvbtff+Nj"
    L"CH+acf2sCvotwjkC+C2K+SMSbyWHuveA84bo+f8w6uUbN/Z2dnZ2ZRoatoYWKfMyyenHxruWLe2+"
    L"68qrViK0WH3JGd9c9h8nfOLazx9x/NWfeuu7/+vzRx195fnn/uuFl1x6wY+vvfmm795w9/2X3bOi"
    L"+/ZntiT3bxin5YMV6sWJOQvTbggt/rIzjwPJAh26ZyMd8comOvbVzfS+fZvnH7p3affX7tZ80F8u"
    L"bj50lwWtx8zraDuemzr+RZo6/9GTAxXbDpZi6xtUQ9NeqtC42JODxwuRQwBWqHsPpXwmt/9unf9n"
    L"9eCUN8wdlsdNDj2Fs4GIJCgjnZIgmNJal1HRn957VGfyPq3A0hFllCvjNO7vE5SlgO/bg5H3ekfy"
    L"533NEcCf/vr6jcr3EFn/9QIHksk2ocUoworNOJzsQ5jRBeJY1TZ//pM6l3vMin24EDfeP75pw129"
    L"915/Ydc1556z+uLTv/z4D/7pxDvOOObDV3zyLW/70XEHHXrNRed9+YffPfe0i67/xfVnXbX09m/d"
    L"+ujqJU9snLpt9Qg9NlCmp4ZCKsN7aMwq+ouOPB0wv0Bvg+fwzpc30fH7t9LH/6qt5R2vbN79kFe0"
    L"vP4AEMR+i9s+gPDi6GL7vH+pEQQ8B2psez8I4o3kQwuQQ5BvaFI+nJhBqSGXO+w1ux5cyAbNv+ka"
    L"DnZcMkkHl/7XFSCLcUt2DOVj8CF8CFE3fH+C7+Hf7h7e2D18XO8N3qINzwDJznPNEcDOsdY1ksBU"
    L"dwgxNtX/9eTgYP/uAwMbSqXSOrxBn2kyZoUj92TfLVf9dOCOyy5bcdFXv7Ligi+d/Juzjzvmqs8c"
    L"9o6Lj3/TIRed+fmPXXL+d089//LrLvnWT2++4d+uv2/Zj3715IbrVgzRPd3j9Gh/mbrHE4T5Qi/D"
    L"oeSBCwr0xsVFeu8rSnT0q1rok69to6Nf07b7u/dte8Ub9mx502v3aH3P7gvajl7Q2fEpKc07palj"
    L"/gn77bXLCYcfuOcJi1uLrx6vpva+NYOYxnMXopa0vOL2s8LB9evFuc0ggS0IkUbjOJ5CLbzoKULq"
    L"jd6/5b3Be2PfqY0e+th6zRHAVlXstJmt5OC9iG3/gdTAwMBoz3DPUPemTQP1ECNbLK7Ct76n4GI/"
    L"Of7EPUu777jislUXf/PfVv341K8+8a1P/N+lp3746J9+/K1v/tHJH3n/ZRd8/8vf++EP//28q352"
    L"w2mX3Xn/GTc8tObqx/uTJU9voQf7pmjllipNxJaa4T3sD8/h/yxqoHfu1URH7NtMJ76unY57TVvH"
    L"wS8rdbQWMo2xYTrn1hUZcMDWhdLk+qrrHzx56J6rbofh94nJDIK8xnCS711/b/je6Ofe9Fs1tmNm"
    L"jgB21MmcZHsN1AjCk8O2IUZfX99499DQMMKKwZ7NPRu99+DPH1Lmp7Nx9qnJ/u4HvPfQc9OPzvfk"
    L"8Nj3P/eZR/794x+97BNvPeTCj/3t2y469+uf/f653zr9u1fecv3Xf3rb0q9e+8CaC369dvMVywbp"
    L"rq5xeqB3iu7rmaRfrhulqx/tNZ/96UPUN+o9eqJMIE9TOHzmym995NC+W8+7harVAZzibwmagolt"
    L"TvC98dff9jule7/9Ms5+p2cXz0nnNPCiNFAjB9R0niD8+cOmTZuqG8Y3TCId9d7DBngPnhyam5vX"
    L"4u38TDmOV8J7WL757utvGLjj8p/60GL5eV86+bFv/sNHbvz8u99/6QkHHXIxQotLr735ostuvvun"
    L"V//yN6fd9sT6T6QmOU5z/PHV3z6qfcU3P3TwqvM/830c8g0HQTA+Uq1OTE1NVYaHh/1b3/jxAHWj"
    L"92PE7dw1mwbmCGA2rczJXgoNeMPzcJ4c6qHF2NjYVB+8h2e3bNlSDy3mLZ63nrLZNZUkecb/q8la"
    L"aHH1WWf0XHjSv6z70We/8+Bp77nygdPeffVDZxxx5eTkZO30HgZfnpiYCEdGRrxb8Py3vX/uSzGH"
    L"P/s+5gjgz36Jf88J/s8380bKnhx8aIGvFTGMuYx0ontoaMvGjRs3gSSG4En4z3TelWcMyace9be8"
    L"T728DlSZu34XDcwRwO+irbm6f0gN1I182/QPOZ4/i2fPEcCfxTLOTWJOA7+fBuYI4PfT21yrOQ38"
    L"WWhgjgD+LJbxJZ7EXHc7jQbmCGCnWeq5ic5pYEcNzBHAjjqZk8xpYKfRwBwB7DRLPTfROQ3sqIE5"
    L"AthRJzu3ZG72O5UG5ghgp1ruucnOaWB7DcwRwPb6mLub08BOpYE5AtiplntusnMa2F4DcwSwvT52"
    L"7ru52e90GpgjgJ1uyecmPKeB5zQwRwDP6WIuN6eBnU4DcwSw0y353ITnNPCcBuYI4Dld7Ny5udnv"
    L"lBqYI4CdctnnJj2ngWkNzBHAtB7mfuc0sFNq4P8BAAD//3ZQFgcAAAAGSURBVAMAEwpwgtXQjTMA"
    L"AAAASUVORK5CYII=";

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

    IWICStream* pStream = nullptr;
    if (FAILED(g_wic->CreateStream(&pStream)) || !pStream) return false;
    HRESULT hr = pStream->InitializeFromMemory(
        const_cast<BYTE*>(png), (DWORD)pngSize);
    if (FAILED(hr)) { pStream->Release(); return false; }

    IWICBitmapDecoder* pDecoder = nullptr;
    hr = g_wic->CreateDecoderFromStream(pStream, nullptr,
                                        WICDecodeMetadataCacheOnLoad, &pDecoder);
    pStream->Release();
    if (FAILED(hr) || !pDecoder) return false;

    IWICBitmapFrameDecode* pFrame = nullptr;
    hr = pDecoder->GetFrame(0, &pFrame);
    pDecoder->Release();
    if (FAILED(hr) || !pFrame) return false;

    IWICFormatConverter* pConv = nullptr;
    hr = g_wic->CreateFormatConverter(&pConv);
    if (SUCCEEDED(hr) && pConv) {
        hr = pConv->Initialize((IWICBitmapSource*)pFrame,
                               GUID_WICPixelFormat32bppBGRA,
                               WICBitmapDitherTypeNone, nullptr, 0.0,
                               WICBitmapPaletteTypeMedianCut);
    }
    pFrame->Release();
    if (FAILED(hr) || !pConv) { if (pConv) pConv->Release(); return false; }

    IWICBitmapSource* src = (IWICBitmapSource*)pConv;
    IWICBitmap* owned = nullptr;
    UINT cw = 0, ch = 0;
    src->GetSize(&cw, &ch);
    while (cw != (UINT)w || ch != (UINT)h) {
        UINT nw = cw, nh = ch;
        if (cw > (UINT)w * 2) nw = (std::max)((UINT)w, (cw + 1) / 2);
        else nw = (UINT)w;
        if (ch > (UINT)h * 2) nh = (std::max)((UINT)h, (ch + 1) / 2);
        else nh = (UINT)h;
        IWICBitmap* next = nullptr;
        hr = WicScaleToBitmap(g_wic, src, nw, nh, &next);
        if (FAILED(hr) || !next) {
            if (owned) owned->Release();
            pConv->Release();
            return false;
        }
        if (owned) owned->Release();
        owned = next;
        src = (IWICBitmapSource*)owned;
        src->GetSize(&cw, &ch);
    }

    outPixels.resize((size_t)w * h * 4);
    hr = src->CopyPixels(nullptr, (UINT)w * 4, (UINT)outPixels.size(),
                         outPixels.data());
    if (owned) owned->Release();
    pConv->Release();
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

    int stride = ((w + 15) & ~15) / 8;
    std::vector<BYTE> mask((size_t)stride * h, 0xFF);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            BYTE a = pixels[((size_t)y * w + x) * 4 + 3];
            if (a >= 128)
                mask[(size_t)y * stride + x / 8] &= ~(0x80 >> (x % 8));
        }
    HBITMAP maskBmp = CreateBitmap(w, h, 1, 1, mask.data());

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmMask = maskBmp;
    ii.hbmColor = color;
    HICON hi = CreateIconIndirect(&ii);
    if (maskBmp) DeleteObject(maskBmp);
    DeleteObject(color);
    return hi;
}

static void DrawAlphaBitmap(HDC hdc, HBITMAP hb, int x, int y, int w, int h) {
    BITMAP bm = {};
    if (!hb || !GetObject(hb, sizeof(bm), &bm) || bm.bmWidth <= 0) return;
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP old = (HBITMAP)SelectObject(mem, hb);
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = (bm.bmBitsPixel == 32) ? AC_SRC_ALPHA : 0;
    BOOL ok = AlphaBlend(hdc, x, y, w, h, mem, 0, 0, bm.bmWidth, bm.bmHeight, bf);
    if (!ok)
        StretchBlt(hdc, x, y, w, h, mem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    SelectObject(mem, old);
    DeleteDC(mem);
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
    if (!g_hicoDrive48 && g_bmpDrive48) {
        ICONINFO ii = {};
        ii.fIcon = TRUE;
        ii.hbmColor = g_bmpDrive48;
        ii.hbmMask = g_bmpDrive48;
        g_hicoDrive48 = CreateIconIndirect(&ii);
    }
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

enum class ActionType { RunProgram, OpenFolder, ReadyBoost, PlayMedia, ViewPictures };
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
static HANDLE  g_hOwnerMutex = nullptr;

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
    return std::wstring((const wchar_t*)data);
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
    g_hOwnerMutex = CreateMutexW(nullptr, TRUE, L"Local\\Win7ClassicAutoPlay.Owner");
    if (!g_hOwnerMutex) return false;
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_hOwnerMutex);
        g_hOwnerMutex = nullptr;
        return false;
    }
    return true;
}

static void ReleaseAutoPlayOwner() {
    if (g_hOwnerMutex) {
        ReleaseMutex(g_hOwnerMutex);
        CloseHandle(g_hOwnerMutex);
        g_hOwnerMutex = nullptr;
    }
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
    } else if (g_cancelRegWritten) {
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
                    sei.fMask = SEE_MASK_IDLIST;
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
           !_wcsicmp(name, L"System Volume Information");
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
        sei.fMask = SEE_MASK_INVOKEIDLIST;
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
    sei.fMask = SEE_MASK_FLAG_NO_UI;
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

static void ExecutePlay(const AutoPlayOption& opt) {
    if (!DrivePresent(g_driveLetter)) return;
    std::wstring wmp = FindWindowsMediaPlayer();
    if (wmp.empty()) {
        Wh_Log(L"ExecutePlay: Windows Media Player not installed, skip");
        return;
    }
    const wchar_t* target = !opt.targetPath.empty() && FileExistsOnDisk(opt.targetPath.c_str())
                          ? opt.targetPath.c_str() : g_driveRoot.c_str();
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
    std::wstring dll = FindPhotoViewerDll();
    if (dll.empty()) {
        Wh_Log(L"ExecuteViewPictures: Photo Viewer not installed, skip");
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
    if ((INT_PTR)ShellExecuteW(NULL, L"open", rundll, args.c_str(),
                               nullptr, SW_SHOWNORMAL) <= 32)
        Wh_Log(L"ExecuteViewPictures: rundll32 Photo Viewer failed");
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
        }
    }
    if (g_hdrIcon.shared) {
        if (g_isWpd && g_hicoPhone48)
            g_hdrIcon.hIcon = g_hicoPhone48;
        else if (g_driveType == DRIVE_CDROM && g_hicoDisc48)
            g_hdrIcon.hIcon = g_hicoDisc48;
        else if (g_hicoDrive48)
            g_hdrIcon.hIcon = g_hicoDrive48;
        else if (!g_hdrIcon.hIcon) {
            g_hdrIcon.hBmp = (g_driveType == DRIVE_CDROM && g_bmpDisc48)
                           ? g_bmpDisc48 : g_bmpDrive48;
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

static void BuildOptions(const AutorunInfo& ar, const MediaInventory& inv) {
    FreeOptions();
    const LangPack* lp = L();
    g_hasProgramSection = ar.hasProgram;
    g_firstGeneralIdx = 0;
    g_contentKind = ClassifyContent(g_audioCd, ar.hasProgram, g_driveType, inv);

    if (ar.hasProgram) {
        AutoPlayOption prog;
        prog.type = ActionType::RunProgram;
        prog.group = OptionGroup::Program;
        prog.programPath = ar.programPath;
        prog.programArgs = ar.programArgs;
        if (!ar.action.empty()) {
            prog.line1 = ar.action;
        } else {
            wchar_t buf[280];
            swprintf_s(buf, ARRAYSIZE(buf), lp->runFile,
                       PathFindFileNameW(ar.programPath.c_str()));
            prog.line1 = buf;
        }
        std::wstring company = GetCompanyName(ar.programPath);
        if (!company.empty()) {
            wchar_t buf[512];
            swprintf_s(buf, ARRAYSIZE(buf), lp->publishedBy, company.c_str());
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
    } else if (g_contentKind == ContentKind::Pictures && HasWindowsPhotoViewer() &&
               !inv.firstPicture.empty() && FileExistsOnDisk(inv.firstPicture.c_str())) {
        AutoPlayOption view;
        view.type = ActionType::ViewPictures;
        view.group = OptionGroup::Content;
        view.line1 = lp->viewPictures;
        view.line2 = lp->usingWindows;
        view.targetPath = inv.firstPicture;
        view.icon = MakePathIcon(inv.firstPicture, g_bmpFolder);
        g_options.push_back(std::move(view));
        Wh_Log(L"BuildOptions: added View pictures (Photo Viewer present)");
    } else if (g_contentKind == ContentKind::Pictures && !HasWindowsPhotoViewer()) {
        Wh_Log(L"BuildOptions: skip View pictures, Photo Viewer missing");
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
            view.icon = MakePathIcon(inv.firstPicture, g_bmpFolder);
            g_options.push_back(std::move(view));
        }
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
    if (idx < 0 || idx >= (int)g_options.size()) return;
    AutoPlayOption opt = g_options[idx];
    bool remember = g_alwaysChecked;
    int letter = g_driveLetter;

    if (remember) {
        std::wstring token;
        switch (opt.type) {
            case ActionType::OpenFolder:   token = L"OpenFolder"; break;
            case ActionType::ReadyBoost:   token = L"ReadyBoost"; break;
            case ActionType::PlayMedia:    token = L"PlayMedia"; break;
            case ActionType::ViewPictures: token = L"ViewPictures"; break;
            case ActionType::RunProgram:
                token = L"Program|" + std::wstring(PathFindFileNameW(opt.programPath.c_str()));
                break;
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

    switch (opt.type) {
        case ActionType::OpenFolder:   ExecuteOpenFolder(); break;
        case ActionType::ReadyBoost:   ExecuteReadyBoost(); break;
        case ActionType::RunProgram:   ExecuteProgram(opt); break;
        case ActionType::PlayMedia:    ExecutePlay(opt); break;
        case ActionType::ViewPictures: ExecuteViewPictures(opt); break;
    }
    if (hwndDlg && IsWindow(hwndDlg)) DestroyWindow(hwndDlg);
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
    EnsureDpiResources();
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
    g_hwndIconBig = CreateIconFromBase64PNG(USER_REMOVABLE_ICON_BASE64, GetSystemMetrics(SM_CXICON),
                                            GetSystemMetrics(SM_CYICON));
    if (!g_hwndIconBig)
        g_hwndIconBig = CreateIconFromBase64PNG(USER_DRIVE_ICON_BASE64, GetSystemMetrics(SM_CXICON),
                                                GetSystemMetrics(SM_CYICON));
    g_hwndIconSmall = CreateIconFromBase64PNG(USER_REMOVABLE_ICON_BASE64, GetSystemMetrics(SM_CXSMICON),
                                              GetSystemMetrics(SM_CYSMICON));
    if (!g_hwndIconSmall)
        g_hwndIconSmall = CreateIconFromBase64PNG(USER_DRIVE_ICON_BASE64, GetSystemMetrics(SM_CXSMICON),
                                                  GetSystemMetrics(SM_CYSMICON));
    if (!g_hwndIconBig) g_hwndIconBig = GetShellIcon(g_driveRoot.c_str(), 0);
    if (!g_hwndIconSmall) g_hwndIconSmall = GetShellIcon(g_driveRoot.c_str(), 1);
    if (g_hwndIconBig) SendMessageW(g_hwndDialog, WM_SETICON, ICON_BIG, (LPARAM)g_hwndIconBig);
    if (g_hwndIconSmall) SendMessageW(g_hwndDialog, WM_SETICON, ICON_SMALL, (LPARAM)g_hwndIconSmall);

    PlaceNativeCheck(g_hwndDialog);
    EnableNonClientDpiScaling(g_hwndDialog);
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
    BuildOptions(ar, inv);
    g_contentKind = ContentKind::Portable;
    g_hasProgramSection = false;
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
    case WM_CREATE:
        g_dpi = GetBestDpiForWindow(hWnd);
        EnsureDpiResources();
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
    if (g_hwndDialog) {
        if (g_pending.empty() && g_hwndListener)
            KillTimer(g_hwndListener, IDT_READY);
        return;
    }

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
        if (g_hwndDialog) {
            g_dpi = (int)GetDpiForWindow(g_hwndDialog);
            if (g_dpi < 96) g_dpi = 96;
            if (g_isWpd)
                BuildWpdDialog(g_wpdPath.empty() ? g_wpdId : g_wpdPath, true);
            else if (g_driveLetter)
                BuildDriveDialog(g_driveLetter, true);
        }
        return 0;
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

    if (g_evtListenerReady) WaitForSingleObject(g_evtListenerReady, 5000);
    if (g_hUiThread) {
        if (g_hwndListener) PostMessageW(g_hwndListener, WMU_SHUTDOWN, 0, 0);
        WaitForSingleObject(g_hUiThread, 10000);
        CloseHandle(g_hUiThread);
        g_hUiThread = nullptr;
        g_dwUiThreadId = 0;
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
        if (g_hwndListener) {
            PostMessageW(g_hwndListener, WMU_APPLY_SUPPRESS, 0, 0);
            PostMessageW(g_hwndListener, WMU_REBUILD, 0, 0);
        }
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged: exception");
    }
}
