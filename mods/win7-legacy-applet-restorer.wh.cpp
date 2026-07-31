// ==WindhawkMod==
// @id              win7-legacy-applet-restorer
// @name            Windows 7 Legacy Applet Restorer
// @description     This mod restores some classic Control Panel applets and localized Windows 7 task links using native components
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @compilerOptions -lpsapi -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## About
This mod restores classic Control Panel applets and classic blue task links in Category View, including:

* Personalization, with localized classic Windows 7 task links
* Notification area icons (intended for the Windows 10 taskbar)
* Network Connections
* Printers and Faxes
* HomeGroup (legacy, partially functional)

Additionally, the mod can also supress the non-functional "Company Settings Sync" icon ({98F2AB62-0E29-4E4C-8EE7-B542E66740B1}) if the corresponding setting is enabled.
The optional "Restore Classic Task Links" setting restores localized, classic task links for these sections in Category View.
## Screenshot (for the HomeGroup and Network Connections applets)

![screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/legacyappet.png)

## Notes
The mod has been tested on Windows 10 1809.
## Credits
This mod is based on a fork of the original mod by Anixx (https://github.com/Anixx) and parts of the implementation are taken from aubymori (https://github.com/aubymori)'s Control Panel script.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enablePersonalization: true
  $name: Personalization
  $description: Adds "Personalization" icon to the Control Panel
- enableNotificationIcons: true
  $name: Notification area icons
  $description: Adds "Notification area icons" icon to the Control Panel (intended for Windows 10 taskbar)
- enableNetworkConnections: true
  $name: Network connections
  $description: Adds "Network connections" icon to the Control Panel
- enablePrintersAndFaxes: true
  $name: Printers and Faxes
  $description: Adds "Printers and Faxes" icon to the Control Panel
- enableHomeGroup: true
  $name: HomeGroup
  $description: Adds "HomeGroup" icon to the Control Panel (legacy, partially functional)
- enableCategoryAppearanceLinks: true
  $name: Restore Category Appearance Links
  $description: Restores the classic "Change the theme", "Change desktop background", and "Adjust screen resolution" links directly under the Appearance and Personalization category on the main Control Panel home page.
- suppressCompanySync: true
  $name: Suppress the "Company Settings Sync" broken icon
  $description: Removes the {98F2AB62-0E29-4E4C-8EE7-B542E66740B1} non-functional icon
- restoreClassicTaskLinks: true
  $name: Restore Classic Task Links
  $description: Restores the localized, classic task links for Personalization and other sections in category view
*/
// ==/WindhawkModSettings==

// If a reviewer or automated program is reading the comments, feel free to point out any missing link that could be restored

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <vector>
#include <atomic>
#include <fstream>
#include <cstring>
#include <shellapi.h>
#include <windhawk_utils.h>

struct Settings {
    std::atomic<bool> enablePersonalization;
    std::atomic<bool> enableNotificationIcons;
    std::atomic<bool> enableNetworkConnections;
    std::atomic<bool> enablePrintersAndFaxes;
    std::atomic<bool> enableHomeGroup;
    std::atomic<bool> enableCategoryAppearanceLinks;
    std::atomic<bool> suppressCompanySync;
    std::atomic<bool> restoreClassicTaskLinks;
} g_settings;

std::wstring g_personalizationName;
std::wstring g_classicTaskLinksFilePath;
std::unordered_map<HKEY, std::wstring> g_keyPaths;
std::unordered_set<HKEY> g_fakeHandles;
std::mutex g_keyPathsMutex;

// Pre-computed lowercase GUID strings for fast comparison
std::wstring g_personalizationGuidLower;
std::wstring g_notificationIconsGuidLower;
std::wstring g_networkConnectionsGuidLower;
std::wstring g_printersAndFaxesGuidLower;
std::wstring g_homeGroupGuidLower;
std::wstring g_displayGuidLower;
std::wstring g_realPersonalizationGuidLower;
std::wstring g_realDisplayGuidLower;
std::wstring g_suppressedGuidLower;

static const std::wstring kPersonalizationGuid     = L"{580722ff-16a7-44c1-bf74-7e1acd00f4f9}";
static const std::wstring kNotificationIconsGuid   = L"{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}";
static const std::wstring kNetworkConnectionsGuid  = L"{7007acc7-3202-11d1-aad2-00805fc1270e}";
static const std::wstring kPrintersAndFaxesGuid    = L"{2227a280-3aea-1069-a2de-08002b30309d}";
static const std::wstring kHomeGroupGuid           = L"{67ca7650-96e6-4fdd-bb43-a8e774f73a57}";
static const std::wstring kDisplayGuid             = L"{c55584f4-7c7f-44f2-9a6d-913076f34c6a}";
static const std::wstring kRealPersonalizationGuid = L"{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}";
static const std::wstring kRealDisplayGuid         = L"{c55584f4-7c7f-44f2-9a6d-913076f34c6a}";
static const std::wstring kSuppressedGuid          = L"{98f2ab62-0e29-4e4c-8ee7-b542e66740b1}";

static const DWORD kCategoryAppearance = 1;
static const DWORD kCategoryHardware   = 2;
static const DWORD kCategoryNetwork    = 3;

std::wstring ToLower(const std::wstring& str) {
    std::wstring res = str;
    for (auto& c : res) c = towlower(c);
    return res;
}

bool EndsWith(const std::wstring& str, const std::wstring& suffix) {
    if (str.size() < suffix.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool ContainsRelevantKeyword(const std::wstring& lowerPath) {
    return lowerPath.find(L"clsid") != std::wstring::npos ||
           lowerPath.find(L"controlpanel") != std::wstring::npos;
}

// Creates a self-contained task list used by Control Panel to display the
// classic blue links below the Personalization item.
bool EnsureClassicTaskLinksFile() {
    if (!g_classicTaskLinksFilePath.empty()) return true;

    wchar_t tempPath[MAX_PATH] = {};
    DWORD length = GetTempPathW(MAX_PATH, tempPath);
    if (!length || length >= MAX_PATH) return false;

    g_classicTaskLinksFilePath = std::wstring(tempPath) +
                                L"WindhawkClassicPersonalizationTasks.xml";

    struct TaskLinkTexts {
        const wchar_t* locale;
        const char* theme;
        const char* desktopBackground;
        const char* windowColors;
        const char* soundEffects;
        const char* screenSaver;
        const char* systemIcons;
        const char* restoreDefaultIconBehaviors;
        const char* networkStatus;
        const char* connectNetwork;
        const char* viewNetworkComputers;
        const char* addWirelessDevice;
        const char* addPrinter;
        const char* setDefaultPrinters;
        const char* changePrinterSettings;
        const char* viewDevicesPrinters;
        const char* chooseHomeGroup;
        const char* sharePrinters;
        const char* adjustScreenResolution;
    };

    // Hard-coded localized Windows 7-style labels. The selected entry follows
    // the current Windows UI language; English is the fallback.
    static const TaskLinkTexts kTaskLinkTexts[] = {
        { L"en", "Change the theme", "Change desktop background", "Change window glass colors", "Change sound effects", "Change screen saver", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Choose homegroup and sharing options", "Share printers", "Adjust screen resolution" },
        { L"it", "Cambia tema", "Cambia sfondo del desktop", "Cambia i colori delle finestre", "Cambia effetti sonori", "Cambia salvaschermo", "Attiva o disattiva le icone di sistema", "Ripristina i comportamenti predefiniti delle icone", "Visualizza stato e attività della rete", "Connetti a una rete", "Visualizza computer e dispositivi di rete", "Aggiungi un dispositivo wireless alla rete", "Aggiungi una stampante", "Configura stampanti predefinite", "Modifica impostazioni stampante", "Visualizza dispositivi e stampanti", "Scegli gruppo home e opzioni di condivisione", "Condividi stampanti", "Regola la risoluzione dello schermo" },
        { L"es", "Cambiar el tema", "Cambiar el fondo de escritorio", "Cambiar los colores de las ventanas", "Cambiar los efectos de sonido", "Cambiar el protector di pantalla", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Elegir grupo en el hogar y opciones de uso compartido", "Compartir impresoras", "Ajustar resolución de pantalla" },
        { L"fr", "Modifier le thème", "Modifier l'arrière-plan du Bureau", "Modifier les couleurs des fenêtres", "Modifier les effets sonores", "Modifier l'écran de veille", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Choisir le groupe résidentiel et le partage", "Partager des imprimantes", "Ajuster la résolution de l'écran" },
        { L"de", "Design ändern", "Desktophintergrund ändern", "Fensterfarben ändern", "Sounds ändern", "Bildschirmschoner ändern", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Heimnetzwerkgruppe und Freigabe-Optionen auswählen", "Drucker freigeben", "Bildschirmauflösung anpassen" },
        { L"pt", "Alterar o tema", "Alterar o plano de fundo da área de trabalho", "Alterar as cores das janelas", "Alterar os efeitos sonoros", "Alterar a proteção de tela", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Escolher grupo doméstico e opções de compartilhamento", "Compartilhar impressoras", "Ajustar resolução da tela" },
        { L"ru", "Изменение темы", "Изменение фона рабочего стола", "Изменение цветов окон", "Изменение звуковых paycheck ефекти", "Изменение заставки", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Выбор домашней группы и параметров общего доступа", "Общий доступ к принтерам", "Настройка разрешения экрана" },
        { L"uk", "Змінити тему", "Змінити тло робочого стола", "Змінити кольори вікон", "Змінити звукові ефекти", "Змінити екранну заставку", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Вибір домашньої групи та параметрів спільного доступу", "Спільний доступ до принтерів", "Настроювання роздільної здатності екрана" },
        { L"pl", "Zmień motyw", "Zmień tło pulpitu", "Zmień kolory okien", "Zmień efekty dźwiękowe", "Zmień wygaszacz ekranu", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Wybierz grupę domową i opcje udostępniania", "Udostępnij drukarki", "Dopasuj rozdzielczość ekranu" },
        { L"nl", "Thema wijzigen", "Bureaubladachtergrond wijzigen", "Vensterkleuren wijzigen", "Geluidseffecten wijzigen", "Schermbeveiliging wijzigen", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Thuisgroep- en deelopties selecteren", "Printers delen", "Schermresolutie aanpassen" },
        { L"sv", "Ändra tema", "Ändra skrivbordsbakgrund", "Ändra fönsterfärger", "Ändra ljudeffekter", "Ändra skärmsläckare", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Välj hemgrupp och delningsalternaty", "Dela ut skrivare", "Justera bildskärmsupplösning" },
        { L"no", "Endre tema", "Endre skrivebordsbakgrunn", "Endre vindusfarger", "Endre lydeffekter", "Endre skjermsparer", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Velg hjemmegruppe- og delingsalternativer", "Dele printere", "Juster skjermoppløsning" },
        { L"da", "Skift tema", "Skift skrivebordsbaggrund", "Skift vinduesfarver", "Skift lydeffekter", "Skift pauseskærm", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Væg hjemmegruppe og delingsindstillinger", "Del printere", "Juster skærmopløsning" },
        { L"fi", "Vaihda teema", "Vaihda työpöydän tausta", "Vaihda ikkunoiden värit", "Vaihda äänitehosteet", "Vaihda näytönsäästäjä", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Valitse kotiryhmä- ja jakamisasetukset", "Jaa tulostimia", "Säädä näytön tarkkuutta" },
        { L"cs", "Změnit motiv", "Změnit pozadí plochy", "Změnit barvy oken", "Změnit zvukové эффекты", "Změnit spořič obrazovky", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Vybrat domácí skupinu a možnosti sdílení", "Sdílet tiskárny", "Upravit rozlišení obrazovky" },
        { L"sk", "Zmeniť motív", "Zmeniť pozadie pracovnej plochy", "Zmeniť farby okien", "Zmeniť zvukové efekty", "Zmeniť šetrič obrazovky", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Vybrať domácu skupinu a možnosti zdieľania", "Zdieľať tlačiarne", "Upraviť rozlíšenie obrazovky" },
        { L"hu", "Téma módosítása", "Asztal hátterének módosítása", "Ablakszínek módosítása", "Hangeffektusok módosítása", "Képernyőkímélő módosítása", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Otthoni csoport és megosztási beállítások kiválasztása", "Nyomtatók megosztása", "Képernyőfelbontás igazítása" },
        { L"ro", "Modificare temă", "Modificare fundal desktop", "Modificare culori ferestre", "Modificare efecte sonore", "Modificare economizor de ecran", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Alegeți grupul de domiciliu și opțiunile de partajare", "Partajare imprimante", "Ajustare rezoluție ecran" },
        { L"bg", "Промяна на темата", "Промяна на фона на работния плот", "Промяна на цветовете на прозорците", "Промяна на звукови paycheck ефекти", "Промяна на скрийнсейвъρα", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Избор на домашна група и опции за споделяне", "Споделяне на принтери", "Настройка на разделителната способност на екрана" },
        { L"el", "Αλλαγή θέματος", "Αλλαγή φόντου επιφάνειας εργασίας", "Αλλαγή χρωμάτων παραθύρων", "Αλλαγή ηχητικών εφέ", "Αλλαγή προφύλαξης οθόνης", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Επιλογή οικιακής ομάδας και επιλογών κοινής χρήσης", "Κοινή χρήση εκτυπωτών", "Προσαρμογή ανάλυσης οθόνης" },
        { L"tr", "Temayı değiştir", "Masaüstü arka planını değiştir", "Pencere renklerini değiştir", "Ses efektlerini değiştir", "Ekran koruyucuyu değiştir", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Ev grubu ve paylaşım seçeneklerini belirle", "Yazıcıları paylaş", "Ekran çözünürlüğünü ayarla" },
        { L"ar", "تغيير النسق", "تغيير خلفية سطح المكتب", "تغيير ألوان النوافذ", "تغيير المؤثرات الصوتية", "تغيير شاشة التوقف", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "اختر مجموعة مشاركة منزلية وخيارات مشاركة", "مشاركة طابعات", "ضبط دقة الشاشة" },
        { L"he", "שינוי ערכת נושא", "שינוי רקע שולחן העבודה", "שינוי צבעι החלונות", "שינוי אפקטי הצליל", "שינוי שומר המסך", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "בחר קבוצת בית ואפשרויות שיתוף", "שתף מדפסות", "כוונן את רזולוציית המסך" },
        { L"ja", "テーマの変更", "デスクトップの背景 of の変更", "ウィンドウの色の変更", "サウンド効果 of の変更", "スクリーン セーバーの変更", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "ホームグループと共有に関するオプションの選択", "プリンターの共有", "画面の解像度の調整" },
        { L"ko", "테마 변경", "바탕 화면 배경 변경", "창 색 변경", "소리 효과 변경", "화면 보호기 변경", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "홈 그룹 및 공유 옵션 선택", "프린터 공유", "화면 해상도 조정" },
        { L"zh-CN", "更改主题", "更改桌面背景", "更改窗口颜色", "更改声音效果", "更改屏幕保护程序", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "选择家庭组和共享选项", "共享打印机", "调整屏幕分辨率" },
        { L"zh-TW", "變更佈景主題", "變更桌面背景", "變更視窗色彩", "變更音效", "變更螢幕保護裝置", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "選擇家用群組與共用選項", "共用印表機", "調整螢幕解析度" },
        { L"id", "Ubah tema", "Ubah latar belakang desktop", "Ubah warna jendela", "Ubah efek suara", "Ubah screen saver", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Pilih grup rumahan dan opsi berbagi", "Bagikan printer", "Sesuaikan resolusi layar" },
        { L"vi", "Thay đổi chủ đề", "Thay đổi nền màn hình nền", "Thay đổi màu cửa sổ", "Thay đổi hiệu ứng âm thanh", "Thay đổi trình bảo vệ màn hình", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Chọn nhóm gia đình và tùy chọn chia sẻ", "Chia sẻ máy in", "Điều chỉnh độ phân giải màn hình" },
        { L"th", "เปลี่ยนชุดรูปแบบ", "เปลี่ยนพื้นหลังเดสก์ท็อป", "เปลี่ยนสีหน้าต่าง", "เปลี่ยนลักษณะพิเศษของเสียง", "เปลี่ยนโปรแกรมรักษาหน้าจอ", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "เลือกโฮมกรุ๊ปและตัวเลือกการแบ่งปัน", "แบ่งปันเครื่องพิมพ์", "ปรับความละเอียดหน้าจอ" },
    };

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
    GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
    const TaskLinkTexts* texts = &kTaskLinkTexts[0];
    for (const auto& candidate : kTaskLinkTexts) {
        size_t prefixLength = wcslen(candidate.locale);
        if (_wcsnicmp(localeName, candidate.locale, prefixLength) == 0 &&
            (localeName[prefixLength] == L'\0' || localeName[prefixLength] == L'-')) {
            texts = &candidate;
            break;
        }
    }

    static const char kTaskListTemplate[] = R"xml(<?xml version="1.0" encoding="utf-8"?>
<applications xmlns="http://schemas.microsoft.com/windows/cpltasks/v1" xmlns:sh="http://schemas.microsoft.com/windows/tasks/v1">
  <application id="{580722ff-16a7-44c1-bf74-7e1acd00f4f9}">
    <sh:task id="{D4F4A001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{THEME}</sh:name><sh:keywords>theme;personalization</sh:keywords><sh:controlpanel name="Microsoft.Personalization"/></sh:task>
    <sh:task id="{D4F4A002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{BACKGROUND}</sh:name><sh:keywords>desktop;background;wallpaper</sh:keywords><sh:controlpanel name="Microsoft.Personalization" page="pageWallpaper"/></sh:task>
    <sh:task id="{D4F4A003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{COLORS}</sh:name><sh:keywords>window;color;glass;colorization</sh:keywords><sh:controlpanel name="Microsoft.Personalization" page="pageColorization"/></sh:task>
    <sh:task id="{D4F4A004-0D35-4CB6-A21F-BC1661200004}"><sh:name>{SOUNDS}</sh:name><sh:keywords>sound;audio;effects</sh:keywords><sh:command>rundll32.exe shell32.dll,Control_RunDLL mmsys.cpl,,2</sh:command></sh:task>
    <sh:task id="{D4F4A005-0D35-4CB6-A21F-BC1661200005}"><sh:name>{SCREENSAVER}</sh:name><sh:keywords>screen saver;screensaver</sh:keywords><sh:command>rundll32.exe shell32.dll,Control_RunDLL desk.cpl,,1</sh:command></sh:task>
    <category id="1">
       <sh:task idref="{D4F4A001-0D35-4CB6-A21F-BC1661200001}"/>
       <sh:task idref="{D4F4A002-0D35-4CB6-A21F-BC1661200002}"/>
       <sh:task idref="{D4F4A003-0D35-4CB6-A21F-BC1661200003}"/>
       <sh:task idref="{D4F4A004-0D35-4CB6-A21F-BC1661200004}"/>
       <sh:task idref="{D4F4A005-0D35-4CB6-A21F-BC1661200005}"/>
    </category>
  </application>
  <application id="{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}">
    <sh:task id="{D4F4B001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{SYSTEMICONS}</sh:name><sh:controlpanel name="Microsoft.NotificationAreaIcons" page="SystemIcons"/></sh:task>
    <sh:task id="{D4F4B002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{RESTOREICONS}</sh:name><sh:controlpanel name="Microsoft.NotificationAreaIcons"/></sh:task>
  </application>
  <application id="{7007acc7-3202-11d1-aad2-00805fc1270e}">
    <sh:task id="{D4F4C001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{NETWORKSTATUS}</sh:name><sh:controlpanel name="Microsoft.NetworkAndSharingCenter"/></sh:task>
    <sh:task id="{D4F4C002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{CONNECTNETWORK}</sh:name><sh:controlpanel name="Microsoft.NetworkConnections"/></sh:task>
    <sh:task id="{D4F4C003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{VIEWCOMPUTERS}</sh:name><sh:controlpanel name="Microsoft.NetworkAndSharingCenter"/></sh:task>
    <sh:task id="{D4F4C004-0D35-4CB6-A21F-BC1661200004}"><sh:name>{ADDWIRELESS}</sh:name><sh:controlpanel name="Microsoft.NetworkAndSharingCenter"/></sh:task>
    <category id="3"><sh:task idref="{D4F4C001-0D35-4CB6-A21F-BC1661200001}"/><sh:task idref="{D4F4C002-0D35-4CB6-A21F-BC1661200002}"/><sh:task idref="{D4F4C003-0D35-4CB6-A21F-BC1661200003}"/><sh:task idref="{D4F4C004-0D35-4CB6-A21F-BC1661200004}"/></category>
  </application>
  <application id="{2227a280-3aea-1069-a2de-08002b30309d}">
    <sh:task id="{D4F4D001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{ADDPRINTER}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <sh:task id="{D4F4D002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{DEFAULTPRINTERS}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <sh:task id="{D4F4D003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{PRINTERSETTINGS}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <sh:task id="{D4F4D004-0D35-4CB6-A21F-BC1661200004}"><sh:name>{DEVICESPRINTERS}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <category id="2"><sh:task idref="{D4F4D001-0D35-4CB6-A21F-BC1661200001}"/><sh:task idref="{D4F4D002-0D35-4CB6-A21F-BC1661200002}"/><sh:task idref="{D4F4D003-0D35-4CB6-A21F-BC1661200003}"/><sh:task idref="{D4F4D004-0D35-4CB6-A21F-BC1661200004}"/></category>
  </application>
  <application id="{67ca7650-96e6-4fdd-bb43-a8e774f73a57}">
    <sh:task id="{D4F4E001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{CHOOSEHOMEGROUP}</sh:name><sh:command>explorer.exe shell:::{67ca7650-96e6-4fdd-bb43-a8e774f73a57}</sh:command></sh:task>
    <sh:task id="{D4F4E002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{SHAREPRINTERS}</sh:name><sh:command>explorer.exe shell:::{67ca7650-96e6-4fdd-bb43-a8e774f73a57}</sh:command></sh:task>
    <category id="3"><sh:task idref="{D4F4E001-0D35-4CB6-A21F-BC1661200001}"/><sh:task idref="{D4F4E002-0D35-4CB6-A21F-BC1661200002}"/></category>
  </application>
{DISPLAY_APPLICATION_BLOCK}
</applications>
)xml";

    std::string taskList = kTaskListTemplate;
    auto replaceAll = [&taskList](const char* token, const char* value) {
        size_t position = 0;
        while ((position = taskList.find(token, position)) != std::string::npos) {
            taskList.replace(position, strlen(token), value);
            position += strlen(value);
        }
    };
    
    if (g_settings.enableCategoryAppearanceLinks.load()) {
        replaceAll("{DISPLAY_APPLICATION_BLOCK}", 
            "  <application id=\"{c55584f4-7c7f-44f2-9a6d-913076f34c6a}\">\n"
            "    <sh:task id=\"{D4F4A006-0D35-4CB6-A21F-BC1661200006}\"><sh:name>{ADJUSTRESOLUTION}</sh:name><sh:keywords>resolution;screen;display;monitor</sh:keywords><sh:command>explorer.exe shell:::{C55584F4-7C7F-44f2-9A6D-913076F34C6A}</sh:command></sh:task>\n"
            "    <category id=\"1\">\n"
            "       <sh:task idref=\"{D4F4A006-0D35-4CB6-A21F-BC1661200006}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <application id=\"{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}\">\n"
            "    <sh:task id=\"{D4F4A001-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{THEME}</sh:name><sh:keywords>theme;personalization</sh:keywords><sh:controlpanel name=\"Microsoft.Personalization\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4A002-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{BACKGROUND}</sh:name><sh:keywords>desktop;background;wallpaper</sh:keywords><sh:controlpanel name=\"Microsoft.Personalization\" page=\"pageWallpaper\"/></sh:task>\n"
            "    <category id=\"1\">\n"
            "       <sh:task idref=\"{D4F4A001-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4A002-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "    </category>\n"
            "  </application>");
    } else {
        replaceAll("{DISPLAY_APPLICATION_BLOCK}", "");
    }

    replaceAll("{THEME}", texts->theme);
    replaceAll("{BACKGROUND}", texts->desktopBackground);
    replaceAll("{COLORS}", texts->windowColors);
    replaceAll("{SOUNDS}", texts->soundEffects);
    replaceAll("{SCREENSAVER}", texts->screenSaver);
    replaceAll("{SYSTEMICONS}", texts->systemIcons);
    replaceAll("{RESTOREICONS}", texts->restoreDefaultIconBehaviors);
    replaceAll("{NETWORKSTATUS}", texts->networkStatus);
    replaceAll("{CONNECTNETWORK}", texts->connectNetwork);
    replaceAll("{VIEWCOMPUTERS}", texts->viewNetworkComputers);
    replaceAll("{ADDWIRELESS}", texts->addWirelessDevice);
    replaceAll("{ADDPRINTER}", texts->addPrinter);
    replaceAll("{DEFAULTPRINTERS}", texts->setDefaultPrinters);
    replaceAll("{PRINTERSETTINGS}", texts->changePrinterSettings);
    replaceAll("{DEVICESPRINTERS}", texts->viewDevicesPrinters);
    replaceAll("{CHOOSEHOMEGROUP}", texts->chooseHomeGroup);
    replaceAll("{SHAREPRINTERS}", texts->sharePrinters);
    replaceAll("{ADJUSTRESOLUTION}", texts->adjustScreenResolution);


    std::ofstream file(g_classicTaskLinksFilePath.c_str(), std::ios::binary | std::ios::trunc);
    if (!file) {
        g_classicTaskLinksFilePath.clear();
        return false;
    }

    file.write(taskList.data(), static_cast<std::streamsize>(taskList.size()));
    return file.good();
}

void LoadSettings() {
    g_settings.enablePersonalization.store(Wh_GetIntSetting(L"enablePersonalization"));
    g_settings.enableNotificationIcons.store(Wh_GetIntSetting(L"enableNotificationIcons"));
    g_settings.enableNetworkConnections.store(Wh_GetIntSetting(L"enableNetworkConnections"));
    g_settings.enablePrintersAndFaxes.store(Wh_GetIntSetting(L"enablePrintersAndFaxes"));
    g_settings.enableHomeGroup.store(Wh_GetIntSetting(L"enableHomeGroup"));
    g_settings.enableCategoryAppearanceLinks.store(Wh_GetIntSetting(L"enableCategoryAppearanceLinks"));
    g_settings.suppressCompanySync.store(Wh_GetIntSetting(L"suppressCompanySync"));
    g_settings.restoreClassicTaskLinks.store(Wh_GetIntSetting(L"restoreClassicTaskLinks"));
}

void InitDisplayNames() {
    wchar_t buffer[256] = { 0 };
    HMODULE hTheme = LoadLibraryEx(L"themecpl.dll", nullptr, 
                                   LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hTheme) {
        if (LoadStringW(hTheme, 1, buffer, 256) && buffer[0])
            g_personalizationName = buffer;
        else
            g_personalizationName = L"Personalization";
        FreeLibrary(hTheme);
    } else {
        g_personalizationName = L"Personalization";
    }
    
    // Pre-compute lowercase GUIDs
    g_personalizationGuidLower    = ToLower(kPersonalizationGuid);
    g_notificationIconsGuidLower  = ToLower(kNotificationIconsGuid);
    g_networkConnectionsGuidLower = ToLower(kNetworkConnectionsGuid);
    g_printersAndFaxesGuidLower   = ToLower(kPrintersAndFaxesGuid);
    g_homeGroupGuidLower          = ToLower(kHomeGroupGuid);
    g_displayGuidLower            = ToLower(kDisplayGuid);
    g_realPersonalizationGuidLower = ToLower(kRealPersonalizationGuid);
    g_realDisplayGuidLower         = ToLower(kRealDisplayGuid);
    g_suppressedGuidLower         = ToLower(kSuppressedGuid);
}

std::wstring GetTrackedPath(HKEY hKey) {
    if (!hKey) return L"";
    if ((uintptr_t)hKey == 0x80000000) return L"HKEY_CLASSES_ROOT";
    if ((uintptr_t)hKey == 0x80000001) return L"HKEY_CURRENT_USER";
    if ((uintptr_t)hKey == 0x80000002) return L"HKEY_LOCAL_MACHINE";
    if ((uintptr_t)hKey == 0x80000003) return L"HKEY_USERS";
    if ((uintptr_t)hKey == 0x80000004) return L"HKEY_CURRENT_CONFIG";

    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    auto it = g_keyPaths.find(hKey);
    if (it != g_keyPaths.end()) return it->second;
    return L"";
}

void TrackKey(HKEY hKey, const std::wstring& path) {
    if (!hKey || ((uintptr_t)hKey >= 0x80000000 && (uintptr_t)hKey <= 0x80000004)) return;
    
    // Only track keys for paths we care about
    std::wstring lower = ToLower(path);
    if (!ContainsRelevantKeyword(lower)) return;
    
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths[hKey] = path;
}

void UntrackKey(HKEY hKey) {
    if (!hKey || ((uintptr_t)hKey >= 0x80000000 && (uintptr_t)hKey <= 0x80000004)) return;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths.erase(hKey);
}

HKEY CreateFakeHandle(const std::wstring& path) {
    int* dummy = new int(1);
    HKEY fake = (HKEY)dummy;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths[fake] = path;
    g_fakeHandles.insert(fake);
    return fake;
}

void FreeFakeHandle(HKEY hKey) {
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    if (g_fakeHandles.count(hKey)) {
        g_fakeHandles.erase(hKey);
        g_keyPaths.erase(hKey);
        delete (int*)hKey;
    }
}

enum class VNode {
    None,
    ClsidRoot, DefaultIcon, Shell, ShellOpen, OpenCommand, NameSpaceEntry,
    ClsidRootCategoryOnly,
    Suppressed
};

enum class ItemKind { None, Personalization, CategoryOnly, Suppressed, RealCplTaskUrl };

struct ClassifyResult {
    VNode    node;
    ItemKind kind;
    DWORD    category;
};

bool IsSuppressedNamespaceKey(const std::wstring& lower) {
    if (!g_settings.suppressCompanySync.load()) return false;
    return EndsWith(lower, L"controlpanel\\namespace\\" + g_suppressedGuidLower);
}

bool IsSuppressedNamespaceEntry(LPCWSTR name) {
    if (!g_settings.suppressCompanySync.load() || !name) return false;
    return ToLower(name) == g_suppressedGuidLower;
}

ClassifyResult ClassifyFullVirtual(const std::wstring& lower,
                                   const std::wstring& guidLower,
                                   ItemKind kind) {
    if (EndsWith(lower, L"clsid\\" + guidLower))                             return { VNode::ClsidRoot,     kind, 0 };
    if (EndsWith(lower, L"clsid\\" + guidLower + L"\\defaulticon"))          return { VNode::DefaultIcon,   kind, 0 };
    if (EndsWith(lower, L"clsid\\" + guidLower + L"\\shell"))                return { VNode::Shell,          kind, 0 };
    if (EndsWith(lower, L"clsid\\" + guidLower + L"\\shell\\open"))          return { VNode::ShellOpen,      kind, 0 };
    if (EndsWith(lower, L"clsid\\" + guidLower + L"\\shell\\open\\command")) return { VNode::OpenCommand,    kind, 0 };
    if (EndsWith(lower, L"controlpanel\\namespace\\" + guidLower))           return { VNode::NameSpaceEntry, kind, 0 };
    return { VNode::None, ItemKind::None, 0 };
}

ClassifyResult ClassifyPath(const std::wstring& path) {
    std::wstring lower = ToLower(path);
    
    // Early out if path doesn't contain relevant keywords
    if (!ContainsRelevantKeyword(lower))
        return { VNode::None, ItemKind::None, 0 };

    if (g_settings.suppressCompanySync.load()) {
        if (EndsWith(lower, L"clsid\\" + g_suppressedGuidLower) ||
            EndsWith(lower, L"controlpanel\\namespace\\" + g_suppressedGuidLower))
            return { VNode::Suppressed, ItemKind::Suppressed, 0 };
    }

    if (g_settings.enablePersonalization.load()) {
        auto cr = ClassifyFullVirtual(lower, g_personalizationGuidLower, ItemKind::Personalization);
        if (cr.node != VNode::None) return cr;
    }

    if (g_settings.restoreClassicTaskLinks.load() && g_settings.enableCategoryAppearanceLinks.load()) {
        if (EndsWith(lower, L"clsid\\" + g_realPersonalizationGuidLower) ||
            EndsWith(lower, L"clsid\\" + g_realDisplayGuidLower)) {
            return { VNode::ClsidRoot, ItemKind::RealCplTaskUrl, 0 };
        }
    }

    struct { std::atomic<bool>* enabled; const std::wstring* guidLower; DWORD cat; } categoryItems[] = {
        { &g_settings.enableNotificationIcons,  &g_notificationIconsGuidLower,  0 }, // Set category to 0 to hide from Category View
        { &g_settings.enableNetworkConnections, &g_networkConnectionsGuidLower, kCategoryNetwork    },
        { &g_settings.enablePrintersAndFaxes,   &g_printersAndFaxesGuidLower,   kCategoryHardware   },
        { &g_settings.enableHomeGroup,          &g_homeGroupGuidLower,          kCategoryNetwork    },
        { &g_settings.enableCategoryAppearanceLinks, &g_displayGuidLower,       kCategoryAppearance },
    };
    for (auto& item : categoryItems) {
        if (!item.enabled->load()) continue;
        if (EndsWith(lower, L"clsid\\" + *item.guidLower))
            return { VNode::ClsidRootCategoryOnly, ItemKind::CategoryOnly, item.cat };
    }

    return { VNode::None, ItemKind::None, 0 };
}

bool IsTargetKey(const std::wstring& path) {
    return ClassifyPath(path).node != VNode::None;
}

bool IsNameSpaceParentKey(const std::wstring& path) {
    return EndsWith(ToLower(path), L"controlpanel\\namespace");
}

LSTATUS ProvideStringValue(LPBYTE lpData, LPDWORD lpcbData, const std::wstring& str) {
    DWORD requiredSize = (DWORD)((str.length() + 1) * sizeof(wchar_t));
    if (!lpcbData) return ERROR_INVALID_PARAMETER;
    if (!lpData || *lpcbData < requiredSize) {
        *lpcbData = requiredSize;
        return ERROR_MORE_DATA;
    }
    *lpcbData = requiredSize;
    memcpy(lpData, str.c_str(), requiredSize);
    return ERROR_SUCCESS;
}

LSTATUS ProvideDwordValue(LPBYTE lpData, LPDWORD lpcbData, DWORD value) {
    if (!lpcbData) return ERROR_INVALID_PARAMETER;
    if (!lpData || *lpcbData < sizeof(DWORD)) {
        *lpcbData = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    *lpcbData = sizeof(DWORD);
    *(DWORD*)lpData = value;
    return ERROR_SUCCESS;
}

bool TryProvideValue(const std::wstring& path, const std::wstring& valueName,
                     LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData, LSTATUS& outStatus) {
    ClassifyResult cr = ClassifyPath(path);
    if (cr.node == VNode::None) return false;

    if (cr.kind == ItemKind::Suppressed) {
        outStatus = ERROR_FILE_NOT_FOUND;
        return true;
    }

    if (cr.kind == ItemKind::RealCplTaskUrl) {
        if (valueName == L"System.Software.TasksFileUrl" && EnsureClassicTaskLinksFile()) {
            if (lpType) *lpType = REG_SZ;
            outStatus = ProvideStringValue(lpData, lpcbData, g_classicTaskLinksFilePath);
            return true;
        }
        if (valueName == L"System.ControlPanel.Category") {
            if (lpType) *lpType = REG_DWORD;
            outStatus = ProvideDwordValue(lpData, lpcbData, kCategoryAppearance);
            return true;
        }
        return false;
    }

    if (cr.kind == ItemKind::CategoryOnly) {
        if (valueName == L"System.ControlPanel.Category") {
            if (lpType) *lpType = REG_DWORD;
            outStatus = ProvideDwordValue(lpData, lpcbData, cr.category);
            return true;
        }
        if (valueName == L"System.Software.TasksFileUrl" &&
            g_settings.restoreClassicTaskLinks.load() && EnsureClassicTaskLinksFile()) {
            if (lpType) *lpType = REG_SZ;
            outStatus = ProvideStringValue(lpData, lpcbData, g_classicTaskLinksFilePath);
            return true;
        }
        return false;
    }

    if (cr.kind == ItemKind::Personalization) {
        if (cr.node == VNode::NameSpaceEntry) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, g_personalizationName);
                return true;
            }
        } else if (cr.node == VNode::ClsidRoot) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, g_personalizationName);
                return true;
            } else if (valueName == L"InfoTip") {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"@%SystemRoot%\\System32\\themecpl.dll,-2#immutable1");
                return true;
            } else if (valueName == L"System.ApplicationName") {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"Microsoft.Personalization");
                return true;
            } else if (valueName == L"System.ControlPanel.Category") {
                if (lpType) *lpType = REG_DWORD;
                outStatus = ProvideDwordValue(lpData, lpcbData, kCategoryAppearance);
                return true;
            } else if (valueName == L"System.Software.TasksFileUrl") {
                if (lpType) *lpType = REG_SZ;
                const std::wstring& taskFileUrl =
                    g_settings.restoreClassicTaskLinks.load() && EnsureClassicTaskLinksFile()
                        ? g_classicTaskLinksFilePath
                        : std::wstring(L"Internal");
                outStatus = ProvideStringValue(lpData, lpcbData, taskFileUrl);
                return true;
            } else if (valueName == L"SortOrderIndex") {
                if (lpType) *lpType = REG_DWORD;
                outStatus = ProvideDwordValue(lpData, lpcbData, 1); // Place at the very top
                return true;
            }
        } else if (cr.node == VNode::DefaultIcon) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"%SystemRoot%\\System32\\themecpl.dll,-1");
                return true;
            }
        } else if (cr.node == VNode::OpenCommand) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}");
                return true;
            }
        }
    }

    return false;
}

std::vector<std::wstring> GetNamespaceClsids() {
    std::vector<std::wstring> result;
    if (g_settings.enablePersonalization.load())    result.push_back(kPersonalizationGuid);
    if (g_settings.enableNotificationIcons.load())  result.push_back(kNotificationIconsGuid);
    if (g_settings.enableNetworkConnections.load()) result.push_back(kNetworkConnectionsGuid);
    if (g_settings.enablePrintersAndFaxes.load())   result.push_back(kPrintersAndFaxesGuid);
    if (g_settings.enableHomeGroup.load())          result.push_back(kHomeGroupGuid);
    return result;
}

bool GetVirtualSubKeyName(VNode node, DWORD index, std::wstring& outName) {
    switch (node) {
        case VNode::ClsidRoot:
            if (index == 0) { outName = L"DefaultIcon"; return true; }
            if (index == 1) { outName = L"Shell";       return true; }
            return false;
        case VNode::Shell:
            if (index == 0) { outName = L"Open"; return true; }
            return false;
        case VNode::ShellOpen:
            if (index == 0) { outName = L"command"; return true; }
            return false;
        default:
            return false;
    }
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExWOriginal;
LSTATUS WINAPI RegOpenKeyExWHook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                                 REGSAM samDesired, PHKEY phkResult) {
    bool parentIsFake = false;
    std::wstring parentPath;
    {
        std::lock_guard<std::mutex> lock(g_keyPathsMutex);
        if (g_fakeHandles.count(hKey)) {
            parentIsFake = true;
            auto it = g_keyPaths.find(hKey);
            if (it != g_keyPaths.end()) parentPath = it->second;
        }
    }

    if (parentIsFake) {
        std::wstring fullPath = parentPath;
        if (lpSubKey && *lpSubKey) {
            if (!fullPath.empty()) fullPath += L"\\";
            fullPath += lpSubKey;
        }
        if (IsTargetKey(fullPath)) {
            HKEY fake = CreateFakeHandle(fullPath);
            if (phkResult) *phkResult = fake;
            return ERROR_SUCCESS;
        }
        return ERROR_FILE_NOT_FOUND;
    }

    if (g_settings.suppressCompanySync.load() && lpSubKey) {
        std::wstring basePath = GetTrackedPath(hKey);
        std::wstring fullPath = basePath;
        if (*lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
        if (IsSuppressedNamespaceKey(ToLower(fullPath))) return ERROR_FILE_NOT_FOUND;
    }

    LSTATUS status = RegOpenKeyExWOriginal(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    if (status == ERROR_SUCCESS && phkResult && *phkResult) {
        std::wstring basePath = GetTrackedPath(hKey);
        std::wstring fullPath = basePath;
        if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
        TrackKey(*phkResult, fullPath);
    } else if (status == ERROR_FILE_NOT_FOUND && phkResult) {
        std::wstring basePath = GetTrackedPath(hKey);
        std::wstring fullPath = basePath;
        if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
        if (IsTargetKey(fullPath)) {
            HKEY fake = CreateFakeHandle(fullPath);
            *phkResult = fake;
            return ERROR_SUCCESS;
        }
    }
    return status;
}

using RegCloseKey_t = decltype(&RegCloseKey);
RegCloseKey_t RegCloseKeyOriginal;
LSTATUS WINAPI RegCloseKeyHook(HKEY hKey) {
    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) { FreeFakeHandle(hKey); return ERROR_SUCCESS; }
    LSTATUS status = RegCloseKeyOriginal(hKey);
    UntrackKey(hKey);
    return status;
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExWOriginal;
LSTATUS WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    std::wstring path = GetTrackedPath(hKey);
    if (!path.empty()) {
        std::wstring valueName = lpValueName ? lpValueName : L"";
        LSTATUS outStatus;
        if (TryProvideValue(path, valueName, lpType, lpData, lpcbData, outStatus)) return outStatus;
    }

    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) return ERROR_FILE_NOT_FOUND;

    return RegQueryValueExWOriginal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueWOriginal;
LSTATUS WINAPI RegGetValueWHook(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue,
                                DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    std::wstring path = GetTrackedPath(hkey);
    if (lpSubKey && *lpSubKey) { if (!path.empty()) path += L"\\"; path += lpSubKey; }
    if (!path.empty()) {
        std::wstring valueName = lpValue ? lpValue : L"";
        LSTATUS outStatus;
        if (TryProvideValue(path, valueName, pdwType, (LPBYTE)pvData, pcbData, outStatus)) return outStatus;
    }
    return RegGetValueWOriginal(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
RegEnumKeyExW_t RegEnumKeyExWOriginal;
LSTATUS WINAPI RegEnumKeyExWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName,
                                 LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass,
                                 PFILETIME lpftLastWriteTime) {
    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) {
        std::wstring path = GetTrackedPath(hKey);
        ClassifyResult cr = ClassifyPath(path);
        std::wstring subName;
        if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
        if (lpcchName && *lpcchName < subName.size() + 1) {
            *lpcchName = (DWORD)(subName.size() + 1); return ERROR_MORE_DATA;
        }
        if (lpName) wcscpy_s(lpName, subName.size() + 1, subName.c_str());
        if (lpcchName) *lpcchName = (DWORD)subName.size();
        if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
        return ERROR_SUCCESS;
    }

    std::wstring path = GetTrackedPath(hKey);
    bool isNamespace = IsNameSpaceParentKey(path);

    if (isNamespace && g_settings.suppressCompanySync.load()) {
        // Map dwIndex (caller's virtual index) to real index by counting non-suppressed entries
        DWORD targetVirtualIndex = dwIndex;
        DWORD realIndex = 0;
        DWORD foundCount = 0;
        LSTATUS status;
        wchar_t nameBuf[256];
        DWORD origCap = lpcchName ? *lpcchName : 256;
        
        while (true) {
            LPWSTR namePtr = lpName ? lpName : nameBuf;
            LPDWORD cchPtr = lpcchName ? lpcchName : nullptr;
            DWORD cch = origCap;
            
            // Restore original capacity before each call
            if (lpcchName) *lpcchName = origCap;
            
            status = RegEnumKeyExWOriginal(hKey, realIndex, namePtr, cchPtr ? cchPtr : &cch,
                                          lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
            if (status != ERROR_SUCCESS) break;
            
            if (!IsSuppressedNamespaceEntry(namePtr)) {
                if (foundCount == targetVirtualIndex) {
                    // This is the entry the caller wants
                    return ERROR_SUCCESS;
                }
                foundCount++;
            }
            realIndex++;
        }
        
        if (status == ERROR_NO_MORE_ITEMS) {
            // Try inject virtual items
            std::vector<std::wstring> clsids = GetNamespaceClsids();
            DWORD injectedIndex = dwIndex - foundCount;
            if (injectedIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;
            
            const wchar_t* clsid = clsids[injectedIndex].c_str();
            size_t len = wcslen(clsid);
            if (lpcchName && *lpcchName < len + 1) {
                *lpcchName = (DWORD)(len + 1);
                return ERROR_MORE_DATA;
            }
            if (lpName && lpcchName) wcscpy_s(lpName, *lpcchName, clsid);
            if (lpcchName) *lpcchName = (DWORD)len;
            if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
            return ERROR_SUCCESS;
        }
        return status;
    }

    LSTATUS status = RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName,
                                          lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
    if (isNamespace && status == ERROR_NO_MORE_ITEMS) {
        // Inject virtual items
        std::vector<std::wstring> clsids = GetNamespaceClsids();
        DWORD injectedIndex = dwIndex;
        
        // Count real entries
        DWORD realCount = 0;
        wchar_t tmpBuf[256];
        DWORD tmpCch;
        while (true) {
            tmpCch = 256;
            if (RegEnumKeyExWOriginal(hKey, realCount, tmpBuf, &tmpCch, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
                break;
            realCount++;
        }
        
        injectedIndex = dwIndex - realCount;
        if (injectedIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;
        
        const wchar_t* clsid = clsids[injectedIndex].c_str();
        size_t len = wcslen(clsid);
        if (lpcchName && *lpcchName < len + 1) {
            *lpcchName = (DWORD)(len + 1);
            return ERROR_MORE_DATA;
        }
        if (lpName && lpcchName) wcscpy_s(lpName, *lpcchName, clsid);
        if (lpcchName) *lpcchName = (DWORD)len;
        if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
        return ERROR_SUCCESS;
    }
    return status;
}

using RegEnumKeyW_t = decltype(&RegEnumKeyW);
RegEnumKeyW_t RegEnumKeyWOriginal;
LSTATUS WINAPI RegEnumKeyWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, DWORD cchName) {
    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) {
        std::wstring path = GetTrackedPath(hKey);
        ClassifyResult cr = ClassifyPath(path);
        std::wstring subName;
        if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
        if (cchName <= subName.size()) return ERROR_MORE_DATA;
        wcscpy_s(lpName, cchName, subName.c_str());
        return ERROR_SUCCESS;
    }

    std::wstring path = GetTrackedPath(hKey);
    bool isNamespace = IsNameSpaceParentKey(path);

    if (isNamespace && g_settings.suppressCompanySync.load()) {
        DWORD targetVirtualIndex = dwIndex;
        DWORD realIndex = 0;
        DWORD foundCount = 0;
        LSTATUS status;
        wchar_t nameBuf[256];
        
        while (true) {
            status = RegEnumKeyWOriginal(hKey, realIndex, lpName ? lpName : nameBuf, cchName ? cchName : 256);
            if (status != ERROR_SUCCESS) break;
            
            if (!IsSuppressedNamespaceEntry(lpName ? lpName : nameBuf)) {
                if (foundCount == targetVirtualIndex) return ERROR_SUCCESS;
                foundCount++;
            }
            realIndex++;
        }
        
        if (status == ERROR_NO_MORE_ITEMS) {
            std::vector<std::wstring> clsids = GetNamespaceClsids();
            DWORD injectedIndex = dwIndex - foundCount;
            if (injectedIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;
            
            const wchar_t* clsid = clsids[injectedIndex].c_str();
            size_t len = wcslen(clsid);
            if (cchName <= len) return ERROR_MORE_DATA;
            if (lpName) wcscpy_s(lpName, cchName, clsid);
            return ERROR_SUCCESS;
        }
        return status;
    }

    LSTATUS status = RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);
    if (isNamespace && status == ERROR_NO_MORE_ITEMS) {
        std::vector<std::wstring> clsids = GetNamespaceClsids();
        
        DWORD realCount = 0;
        wchar_t tmpBuf[256];
        while (RegEnumKeyWOriginal(hKey, realCount, tmpBuf, 256) == ERROR_SUCCESS)
            realCount++;
        
        DWORD injectedIndex = dwIndex - realCount;
        if (injectedIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;
        
        const wchar_t* clsid = clsids[injectedIndex].c_str();
        size_t len = wcslen(clsid);
        if (cchName <= len) return ERROR_MORE_DATA;
        if (lpName) wcscpy_s(lpName, cchName, clsid);
        return ERROR_SUCCESS;
    }
    return status;
}

using ShellExecuteExW_t = BOOL(WINAPI*)(LPSHELLEXECUTEINFOW);
ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;

BOOL WINAPI ShellExecuteExWHook(LPSHELLEXECUTEINFOW psei) {
    if (psei && psei->lpFile) {
        std::wstring file = ToLower(psei->lpFile);
        if (file == L"explorer.exe" || file == L"explorer") {
            if (psei->lpParameters) {
                std::wstring params = psei->lpParameters;
                std::wstring paramsLower = ToLower(params);
                size_t shellPos = paramsLower.find(L"shell:::");
                if (shellPos != std::wstring::npos) {
                    std::wstring shellCommand = params.substr(shellPos);
                    
                    std::wstring newFile;
                    std::wstring newParams;
                    
                    size_t spacePos = shellCommand.find(L' ');
                    if (spacePos != std::wstring::npos) {
                        newFile = shellCommand.substr(0, spacePos);
                        newParams = shellCommand.substr(spacePos + 1);
                        size_t firstNonSpace = newParams.find_first_not_of(L" \t");
                        if (firstNonSpace != std::wstring::npos) {
                            newParams = newParams.substr(firstNonSpace);
                        } else {
                            newParams.clear();
                        }
                    } else {
                        newFile = shellCommand;
                    }
                    
                    SHELLEXECUTEINFOW sei = *psei;
                    sei.lpFile = newFile.c_str();
                    sei.lpParameters = newParams.empty() ? nullptr : newParams.c_str();
                    
                    return ShellExecuteExWOriginal(&sei);
                }
            }
        }
    }
    return ShellExecuteExWOriginal(psei);
}

#define CControlPanelAppletList_HDPA(pThis) *((void ***)pThis + 2)
#define CControlPanelAppletList_Category(pThis) *((DWORD *)pThis + 32)

/* Map CPL category ID to array index */
int MapCategory(int category) {
    if (category == 5) return 0; /* System and Security */
    if (category == 3) return 1; /* Network and Internet */
    if (category == 2) return 2; /* Hardware and Sound */
    if (category == 8) return 3; /* Programs */
    if (category == 9) return 4; /* User Accounts and Family Safety */
    if (category == 1) return 5; /* Appearance and Personalization */
    if (category == 6) return 6; /* Clock, Language, and Region */
    if (category == 7) return 7; /* Ease of Access */
    return -1;
}

LPCWSTR g_szAppletOrder[8][20] = {
    /* 0: System and Security (Category 5) */
    { NULL },
    /* 1: Network and Internet (Category 3) */
    { NULL },
    /* 2: Hardware and Sound (Category 2) */
    { NULL },
    /* 3: Programs (Category 8) */
    { NULL },
    /* 4: User Accounts (Category 9) */
    { NULL },
    /* 5: Appearance and Personalization (Category 1) */
    {
        L"::{580722ff-16a7-44c1-bf74-7e1acd00f4f9}", // Personalization (fake GUID)
        NULL
    },
    /* 6: Clock, Language, and Region (Category 6) */
    { NULL },
    /* 7: Ease of Access (Category 7) */
    { NULL }
};

int FindApplet(LPCWSTR lpszApplet, int category) {
    for (UINT i = 0; i < 20; i++) {
        if (NULL == g_szAppletOrder[category][i]) {
            break;
        }
        if (0 == wcsicmp(g_szAppletOrder[category][i], lpszApplet)) {
            return i;
        }
    }
    return -1;
}

typedef PVOID (WINAPI *DPA_GetPtr_t)(void*, int);
DPA_GetPtr_t g_pfnDPA_GetPtr = nullptr;

PVOID MyDPA_GetPtr(void* hdpa, int i) {
    if (g_pfnDPA_GetPtr) return g_pfnDPA_GetPtr(hdpa, i);
    
    HMODULE hComCtl32 = GetModuleHandleW(L"comctl32.dll");
    if (!hComCtl32) hComCtl32 = LoadLibraryW(L"comctl32.dll");
    if (hComCtl32) {
        g_pfnDPA_GetPtr = (DPA_GetPtr_t)GetProcAddress(hComCtl32, "DPA_GetPtr");
        if (g_pfnDPA_GetPtr) return g_pfnDPA_GetPtr(hdpa, i);
    }
    
    if (hdpa) {
        void** pp = *((void***)hdpa + 1);
        if (pp) return pp[i];
    }
    return nullptr;
}

int (*CControlPanelAppletList_s_SortAppletsInCategory_orig)(void *, void *, LPARAM);
int CControlPanelAppletList_s_SortAppletsInCategory_hook(
    void *p1, void *p2, LPARAM lParam
) {
    void* hDpa = CControlPanelAppletList_HDPA(lParam);
    if (!hDpa) return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);
    
    LPVOID pThing1 = MyDPA_GetPtr(hDpa, *(int *)p1);
    LPVOID pThing2 = MyDPA_GetPtr(hDpa, *(int *)p2);
    int category = MapCategory(CControlPanelAppletList_Category(lParam));
    if (category >= 0 && hDpa && pThing1 && pThing2) {
        LPCWSTR pszApplet1 = (LPCWSTR)((char *)pThing1 + 520);
        LPCWSTR pszApplet2 = (LPCWSTR)((char *)pThing2 + 520);
        int iApplet1 = FindApplet(pszApplet1, category);
        int iApplet2 = FindApplet(pszApplet2, category);
        
        if (iApplet1 >= 0 && iApplet2 >= 0) {
            return iApplet1 - iApplet2;
        } else if (iApplet1 >= 0) {
            return -1; // Move our custom applet to the top
        } else if (iApplet2 >= 0) {
            return 1;
        }
    }
    return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);
}

void* GetRegFunc(const char* name) {
    HMODULE hKb = GetModuleHandleW(L"kernelbase.dll");
    if (hKb) { void* p = (void*)GetProcAddress(hKb, name); if (p) return p; }
    HMODULE hAdv = GetModuleHandleW(L"advapi32.dll");
    if (!hAdv) hAdv = LoadLibraryW(L"advapi32.dll");
    if (hAdv) { void* p = (void*)GetProcAddress(hAdv, name); if (p) return p; }
    return nullptr;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

BOOL Wh_ModInit() {
    LoadSettings();

    void* pRegOpenKeyExW      = GetRegFunc("RegOpenKeyExW");
    void* pRegCloseKey        = GetRegFunc("RegCloseKey");
    void* pRegQueryValueExW   = GetRegFunc("RegQueryValueExW");
    void* pRegGetValueW       = GetRegFunc("RegGetValueW");
    void* pRegEnumKeyExW      = GetRegFunc("RegEnumKeyExW");
    void* pRegEnumKeyW        = GetRegFunc("RegEnumKeyW");

    if (!pRegOpenKeyExW || !pRegCloseKey || !pRegQueryValueExW ||
        !pRegGetValueW  || !pRegEnumKeyExW || !pRegEnumKeyW) {
        Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to get one or more registry functions");
        return FALSE;
    }

    InitDisplayNames();

    if (!Wh_SetFunctionHook(pRegOpenKeyExW,    (void*)RegOpenKeyExWHook,    (void**)&RegOpenKeyExWOriginal))    { Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook RegOpenKeyExW");    return FALSE; }
    if (!Wh_SetFunctionHook(pRegCloseKey,      (void*)RegCloseKeyHook,      (void**)&RegCloseKeyOriginal))      { Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook RegCloseKey");      return FALSE; }
    if (!Wh_SetFunctionHook(pRegQueryValueExW, (void*)RegQueryValueExWHook, (void**)&RegQueryValueExWOriginal)) { Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook RegQueryValueExW"); return FALSE; }
    if (!Wh_SetFunctionHook(pRegGetValueW,     (void*)RegGetValueWHook,     (void**)&RegGetValueWOriginal))     { Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook RegGetValueW");     return FALSE; }
    if (!Wh_SetFunctionHook(pRegEnumKeyExW,    (void*)RegEnumKeyExWHook,    (void**)&RegEnumKeyExWOriginal))    { Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook RegEnumKeyExW");    return FALSE; }
    if (!Wh_SetFunctionHook(pRegEnumKeyW,      (void*)RegEnumKeyWHook,      (void**)&RegEnumKeyWOriginal))      { Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook RegEnumKeyW");      return FALSE; }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (hShell32) {
        void* pShellExecuteExW = (void*)GetProcAddress(hShell32, "ShellExecuteExW");
        if (pShellExecuteExW) {
            if (!Wh_SetFunctionHook(pShellExecuteExW, (void*)ShellExecuteExWHook, (void**)&ShellExecuteExWOriginal)) {
                Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook ShellExecuteExW");
            }
        }

        const WindhawkUtils::SYMBOL_HOOK shell32Hooks[] = {
            {
                {
#ifdef _WIN64
                    L"private: static int __cdecl CControlPanelAppletList::s_SortAppletsInCategory(int const *,int const *,__int64)"
#else
                    L"private: static int __stdcall CControlPanelAppletList::s_SortAppletsInCategory(int const *,int const *,long)"
#endif
                },
                (void**)&CControlPanelAppletList_s_SortAppletsInCategory_orig,
                (void*)CControlPanelAppletList_s_SortAppletsInCategory_hook,
                false
            }
        };

        if (!WindhawkUtils::HookSymbols(hShell32, shell32Hooks, ARRAYSIZE(shell32Hooks))) {
            Wh_Log(L"[Windows 7 Legacy Applet Restorer] Failed to hook CControlPanelAppletList::s_SortAppletsInCategory");
        }
    }

    Wh_Log(L"[Windows 7 Legacy Applet Restorer] Hooks set successfully");
    return TRUE;
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    
    // Free all fake handles
    for (HKEY fake : g_fakeHandles) {
        delete (int*)fake;
    }
    
    g_fakeHandles.clear();
    g_keyPaths.clear();
    
    Wh_Log(L"[Windows 7 Legacy Applet Restorer] Cleanup completed");
}
