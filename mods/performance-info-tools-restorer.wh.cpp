// ==WindhawkMod==
// @id             performance-info-tools-restorer
// @name           Performance Information and Tools Restorer
// @description    This mod restores the classic Windows Performance Information and Tools (Windows Experience Index) page for Windows 10 and 11
// @version        1.0.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @include        control.exe
// @architecture   x86-64
// @compilerOptions -lwininet -ladvapi32 -lole32 -luser32
// ==/WindhawkMod==

// Note on @architecture x86-64:
// "x86-64" is the canonical specifier used by Windhawk mods and passes the
// repository's PR validation cleanly. "amd64" is also accepted by
// .github/pr_validation.py (the only hard rejections are architectures outside
// {"x86", "x86-64", "amd64", "arm64"}), but it produces a "manual verification
// required" warning for the maintainers, so x86-64 is used here. The payload
// DLL is x64-only, and on ARM64 devices Windhawk may still inject this mod into
// the native ARM64 explorer.exe; Wh_ModInit() refuses to start there before any
// hook is installed (see IsRunningAsAmd64).

// ==WindhawkModReadme==
/*
# Performance Information and Tools Restorer
## About
This mod restores the classic Windows "Performance Information and Tools" (Windows Experience Index) page on Windows 10 and 11, bringing back the CPU, RAM, graphics and disk scores in Control Panel.

The mod has been tested on Windows 10 21H2, Windows 10 22H2 and Windows 11 25H2.
## Screenshots

The mod includes **3 skins**: Windows 7, Windows 8, and Windows Vista.

### Windows 7 skin

![Windows 7 skin](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/windows7perf.png)

### Windows 8 skin

![Windows 8 skin](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/windows8perf.png)

### Windows Vista skin

![Windows Vista skin](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/windowsvistaperf.png)

## Features

- **Classic WEI page**: The mod restores "Performance Information and Tools" in Control Panel (Large/Small icons view)
- **Automatic setup**: The required PerfCenterCPL.dll is downloaded and verified automatically, nothing to install manually
- **Right DLL for the system**: Uses the Windows 8 DLL on Windows 8.1/10/11
- **Built-in translations and language selector**: All 147 page strings are embedded for Italian, Spanish, French, English, Turkish, Russian, Chinese simplified, German, Portuguese and Polish. The language can follow Windows automatically or be selected manually. 
- **Conservative resource handling**: The mod implements a conservative approach to be more stable
- **100% reversible**: Disabling the mod makes the Control Panel entry disappear immediately. The files it created (private resource module, downloaded DLL, variant marker, and any stale copies) are removed on the next unload of the mod in a process where they are no longer mapped; because explorer.exe keeps the DLL loaded in memory for the rest of its session, a file that is still in use at that moment is retried on a later unload (e.g. after explorer.exe restarts). By default, downloaded files are preserved for faster offline re-enabling and registry keys are not written.

## Requirements

- **Windows 10 or Windows 11** (64-bit) with the native Control Panel
- An internet connection the first time the mod runs (to download the DLL)

## Design and safety notes

- The setup step (download + verification) runs on a background thread so it never blocks explorer.exe startup, and it is serialized across processes with a named mutex so two explorer.exe/Control Panel instances can't write the same file at once. The download always goes to a temporary file that is SHA-256 checked against a pinned digest and then atomically moved into place.
- The download is time-limited (connect/send/receive and an overall deadline), retried a bounded number of times, and aborts immediately on shutdown, so a slow or captive-portal network can never hang explorer.exe or block sign-out. If a valid DLL was already downloaded previously, it is reused with no network access at all, so the page keeps working offline.
- If the DLL cannot be obtained, the mod fails gracefully: nothing crashes or blocks, and a clear message is written to the mod's log explaining that an internet connection is required (restart explorer.exe to retry).
- The mod only stores files in its dedicated Windhawk mod-storage folder and never touches files it did not create. This location is obtained through `Wh_GetModStoragePath`, so it also works with portable Windhawk installations.
- All registry values are provided through an in-memory virtualization layer and nothing is persisted to the registry, so uninstalling the mod does not leave traces.

## Known limitations

- **"(unrated)" scores**: If your PC has never been rated, plug in power and run `winsat formal` once as admin
- **On battery**: The assessment is not applicable while running on battery
- **Windows 7 page on modern systems**: The Windows 7 version of the page is not compatible with Windows 8.1/10/11 (It displays the "cannot load page" error), so the Windows 8 DLL is always used there
- **Windows 7 skin score scale**: The optional Windows 7 skin is only a visual/layout recreation. The restored page still uses the Windows 8 scoring range, so ratings can go up to 9.9 rather than Windows 7's original 7.9 scale.

## Credits

- **Cips** — Testing the mod on Windows 11 25H2
- **susiked** — Testing the mod on Windows 10 22H2 and providing the Windows 8 skin screenshot
- **MzKo** — Testing the mod on Windows 10 22H2, providing feedback, and providing the Windows Vista skin screenshot

If any problems are encountered, please report them to the mod's author.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- language: auto
  $name: Page language
  $description: This setting is used to select the language used by the restored page and its Control Panel tooltip. "Automatic" follows the current Windows UI language.
  $options:
    - auto: Automatic (Windows UI language)
    - en-US: English (United States)
    - it-IT: Italian
    - es-ES: Spanish
    - fr-FR: French
    - tr-TR: Turkish
    - ru-RU: Russian
    - zh-CN: Simplified Chinese
    - de-DE: German
    - pt-BR: Portuguese (Brazil)
    - pl-PL: Polish

- forceTranslations: true
  $name: Force translations
  $description: If this setting is enabled, the mod forces the use of the embedded translations within the page for all 10 available languages (including DirectUI page text). If disabled, uses the DLL's own native resources.

- pageSkin: win8
  $name: Page skin
  $description: This setting is used to change the page skin/layout. Choose Windows 8 (default), Windows 7, or Windows Vista. The skin is applied in memory only and does not write or modify DLL resources on disk.
  $options:
    - win8: Windows 8 layout (default)
    - win7: Windows 7 layout (more complete)
    - vista: Windows Vista (more compact)

- keepFilesOnDisable: true
  $name: Keep downloaded files if the mod is disabled
  $description: If this setting is enabled (which it is by default), the mod keeps downloaded files for faster re-enabling. If disabled, when the mod is disabled or uninstalled, the downloaded DLL, variant marker, and any stale copies are removed from the mod's folder on the next unload in which they are no longer mapped. The DLL stays loaded in memory until the process (explorer.exe) restarts, so files that are still in use at that moment are deleted on a later unload.
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <wininet.h>
#include <wincrypt.h>
#include <combaseapi.h>
#include <shellapi.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>
#include <thread>
#include <atomic>
#include <optional>
#include <windhawk_utils.h>

// =============================================================================
// Constants - the mod keeps all of its files in its own folder and never
// shares (or deletes) files created by other mods.
// =============================================================================
static const wchar_t* kDllRelativeName = L"PerfCenterCPL.dll";

// =============================================================================
// EMBEDDED STRING CATALOG - No MUI file is generated or required
// =============================================================================
enum class MuiLanguage {
    EN_US, IT_IT, ES_ES, FR_FR, TR_TR, RU_RU, ZH_CN, DE_DE, PT_BR, PL_PL
};

struct MuiStringTable {
    UINT id;
    const wchar_t* en;
    const wchar_t* it;
    const wchar_t* es;
    const wchar_t* fr;
    const wchar_t* tr;
    const wchar_t* ru;
    const wchar_t* zh;
    const wchar_t* de;
    const wchar_t* pt;
    const wchar_t* pl;
};

// All 147 strings and ten translations are compiled directly into the mod.
// The IDs are the exact IDs extracted from the Windows 8 PerfCenterCPL.dll.
static const MuiStringTable kMuiStrings[] = {
    {1,   L"Performance Information and Tools",        L"Informazioni e strumenti sulle prestazioni",        L"Información y herramientas de rendimiento",        L"Informations et outils de performance",        L"Performans Bilgileri ve Araçları",        L"Счетчики и средства производительности",        L"性能信息和工具",        L"Leistungsinformationen und -tools",        L"Informações e Ferramentas de Desempenho",        L"Informacje i narzędzia dotyczące wydajności"},
    {2,   L"Get information about your computer's speed and performance. If solutions to performance problems are available, Windows lets you know.", L"Ottieni informazioni sulla velocità e le prestazioni del computer. Se sono disponibili soluzioni ai problemi di prestazioni, Windows ti avvisa.", L"Obtén información sobre la velocidad y el rendimiento de tu equipo. Si hay soluciones disponibles, Windows te lo indica.", L"Obtenez des informations sur la vitesse et les performances de votre ordinateur. Si des solutions sont disponibles, Windows vous en informe.", L"Bilgisayarınızın hızı ve performansı hakkında bilgi alın. Performans sorunlarına çözümler mevcutsa, Windows sizi bilgilendirir.", L"Получите информацию о скорости и производительности компьютера. Если доступны решения проблем с производительностью, Windows сообщит вам.", L"获取有关计算机速度和性能的信息。如果有可用的性能问题解决方案，Windows会通知您。", L"Erhalten Sie Informationen über die Geschwindigkeit und Leistung Ihres Computers. Wenn Lösungen verfügbar sind, informiert Windows Sie.", L"Obtenha informações sobre a velocidade e o desempenho do seu computador. Se houver soluções disponíveis, o Windows informará você.", L"Uzyskaj informacje o szybkości i wydajności komputera. Jeśli dostępne są rozwiązania problemów z wydajnością, system Windows Cię o tym powiadomi."},
    {3,   L"Advanced Tools",                           L"Strumenti avanzati",                               L"Herramientas avanzadas",                           L"Outils avancés",                               L"Gelişmiş Araçlar",                           L"Дополнительные средства",                        L"高级工具",                                        L"Erweiterte Tools",                              L"Ferramentas Avançadas",                           L"Zaawansowane narzędzia"},
    {4,   L"Advanced Tools",                           L"Strumenti avanzati",                               L"Herramientas avanzadas",                           L"Outils avancés",                               L"Gelişmiş Araçlar",                           L"Дополнительные средства",                        L"高级工具",                                        L"Erweiterte Tools",                              L"Ferramentas Avançadas",                           L"Zaawansowane narzędzia"},
    {10,   L"Manage startup programs",                  L"Gestisci programmi di avvio",                       L"Administrar programas de inicio",                  L"Gérer les programmes de démarrage",             L"Başlangıç programlarını yönet",               L"Управление программами автозагрузки",           L"管理启动程序",                                     L"Startprogramme verwalten",                        L"Gerenciar programas de inicialização",           L"Zarządzaj programami uruchamianymi przy starcie"},
    {11,   L"Adjust visual effects",                    L"Modifica effetti visivi",                          L"Ajustar efectos visuales",                         L"Modifier les effets visuels",                 L"Görsel efektleri ayarla",                     L"Настройка визуальных эффектов",                 L"调整视觉效果",                                     L"Visuelle Effekte anpassen",                        L"Ajustar efeitos visuais",                         L"Modyfikuj efekty wizualne"},
    {12,   L"Adjust indexing options",                  L"Modifica opzioni di indicizzazione",               L"Ajustar opciones de indexación",                   L"Modifier les options d'indexation",           L"Dizin oluşturma seçeneklerini ayarla",         L"Настройка параметров индексации",               L"调整索引选项",                                     L"Indexierungsoptionen anpassen",                   L"Ajustar opções de indexação",                     L"Modyfikuj opcje indeksowania"},
    {13,   L"Adjust power settings",                    L"Modifica impostazioni risparmio energia",          L"Ajustar la configuración de energía",             L"Modifier les paramètres d'alimentation",      L"Güç ayarlarını değiştir",                       L"Настройка параметров электропитания",            L"调整电源设置",                                     L"Energieeinstellungen anpassen",                   L"Ajustar configurações de energia",                L"Modyfikuj ustawienia zasilania"},
    {14,   L"Open disk cleanup",                        L"Apri Pulizia disco",                               L"Abrir la limpieza de disco",                      L"Ouverture du nettoyage de disque",            L"Disk temizlemeyi aç",                           L"Открыть очистку диска",                         L"打开磁盘清理",                                     L"Datenträgerbereinigung öffnen",                  L"Abrir a Limpeza de Disco",                        L"Otwórz oczyszczanie dysku"},
    {15,   L"Advanced tools",                           L"Strumenti avanzati",                               L"Herramientas avanzadas",                           L"Outils avancés",                               L"Gelişmiş Araçlar",                           L"Дополнительные средства",                        L"高级工具",                                        L"Erweiterte Tools",                              L"Ferramentas Avançadas",                           L"Zaawansowane narzędzia"},
    {16,   L"Security center",                          L"Centro sicurezza",                                 L"Centro de seguridad",                             L"Centre de sécurité",                           L"Güvenlik Merkezi",                             L"Центр безопасности",                             L"安全中心",                                         L"Sicherheitscenter",                               L"Central de Segurança",                            L"Centrum zabezpieczeń"},
    {17,   L"Problem reports and solutions",            L"Segnalazione e soluzioni problemi",                L"Informes de problemas y soluciones",               L"Rapports de problèmes et solutions",           L"Sorun raporları ve çözümler",                   L"Отчеты о проблемах и решения",                   L"问题报告和解决方案",                               L"Problemberichte und Lösungen",                    L"Relatórios de problemas e soluções",              L"Raporty o problemach i rozwiązania"},
    {36,   L"The performance settings on this computer are managed by your network administrator through Group Policy.  Your administrator has turned off the features on this page.", L"Le impostazioni delle prestazioni di questo computer sono gestite dall'amministratore di rete tramite Criteri di gruppo. L'amministratore ha disattivato le funzionalità di questa pagina.", L"La configuración de rendimiento de este equipo está administrada por el administrador de red mediante directivas de grupo. Su administrador ha desactivado las características de esta página.", L"Les paramètres de performances de cet ordinateur sont gérés par votre administrateur réseau via la stratégie de groupe. Votre administrateur a désactivé les fonctionnalités de cette page.", L"Bu bilgisayardaki performans ayarları, ağ yöneticiniz tarafından Grup İlkesi aracılığıyla yönetiliyor. Yöneticiniz bu sayfadaki özellikleri kapatmış.", L"Параметры производительности этого компьютера управляются сетевым администратором через групповую политику. Администратор отключил функции на этой странице.", L"此计算机上的性能设置由网络管理员通过组策略进行管理。管理员已关闭此页面上的功能。", L"Die Leistungseinstellungen auf diesem Computer werden von Ihrem Netzwerkadministrator über Gruppenrichtlinien verwaltet. Ihr Administrator hat die Funktionen auf dieser Seite deaktiviert.", L"As configurações de desempenho neste computador são gerenciadas pelo administrador de rede por meio da Diretiva de Grupo. Seu administrador desligou os recursos desta página.", L"Ustawienia wydajności na tym komputerze są zarządzane przez administratora sieci za pomocą zasad grupy. Administrator wyłączył funkcje na tej stronie."},
    {37,   L"Windows is currently running in safe mode. Performance details are currently unavailable.", L"Windows è attualmente in esecuzione in modalità provvisoria. I dettagli sulle prestazioni non sono disponibili.", L"Windows se está ejecutando en modo seguro. Los detalles de rendimiento no están disponibles actualmente.", L"Windows est actuellement en mode sans échec. Les détails de performance ne sont actuellement pas disponibles.", L"Windows şu anda güvenli modda çalışıyor. Performans ayrıntıları şu anda kullanılamıyor.", L"Windows в настоящее время работает в безопасном режиме. Сведения о производительности в настоящее время недоступны.", L"Windows 当前正在安全模式下运行。性能详细信息目前不可用。", L"Windows wird derzeit im abgesicherten Modus ausgeführt. Leistungsdetails sind derzeit nicht verfügbar.", L"O Windows está sendo executado no modo de segurança. Os detalhes de desempenho não estão disponíveis no momento.", L"System Windows jest obecnie uruchomiony w trybie awaryjnym. Szczegóły dotyczące wydajności są obecnie niedostępne."},
    {39,   L"Windows Experience Index",                L"Indice prestazioni Windows",                       L"Índice de experiencia de Windows",                 L"Índice de performance Windows",                L"Windows Deneyim Dizini",                       L"Индекс производительности Windows",             L"Windows 体验指数",                                   L"Windows-Leistungsindex",                          L"Índice de Experiência do Windows",               L"Indeks wydajności systemu Windows"},
    {40,   L"Rating this computer ...",                 L"Valutazione del computer ...",                     L"Evaluando este equipo ...",                       L"Évaluation de cet ordinateur ...",            L"Bu bilgisayarı değerlendirme ...",             L"Оценка этого компьютера ...",                    L"正在评估此计算机 ...",                             L"Diesen Computer bewerten ...",                    L"Avaliando este computador ...",                   L"Ocena tego komputera ..."},
    {41,   L"This might take a few minutes. Your screen might flash", L"Può richiedere alcuni minuti. Lo schermo potrebbe lampeggiare", L"Esto puede tardar unos minutos. La pantalla podría parpadear", L"Cela peut prendre quelques minutes. Votre écran risque de clignoter", L"Bu işlem birkaç dakika sürebilir. Ekranınız yanıp sönebilir", L"Это может занять несколько минут. Ваш экран может мигать", L"这可能需要几分钟。您的屏幕可能会闪烁", L"Dies kann einige Minuten dauern. Ihr Bildschirm könnte flackern", L"Isso pode levar alguns minutos. Sua tela podepiscar", L"Może to potrwać kilka minut. Ekran może migać"},
    {42,   L"during the rating.",                       L"durante la valutazione.",                           L"durante la evaluación.",                           L"pendant l'évaluation.",                        L"değerlendirme sırasında.",                     L"во время оценки.",                                L"评估期间。",                                         L"während der Bewertung.",                           L"durante a avaliação.",                             L"podczas oceny."},
    {48,   L"Your Windows Experience Index has not yet been established.", L"L'indice prestazioni Windows non è ancora stato definito.", L"El Índice de experiencia aún no se ha establecido.", L"L'indice de performance n'a pas encore été établi.", L"Windows Deneyim Dizini henüz oluşturulmadı.", L"Индекс производительности ещё не определён.", L"尚未建立 Windows 体验指数。", L"Windows-Leistungsindex wurde noch nicht erstellt.", L"O Índice de Experiência ainda não foi estabelecido.", L"Indeks wydajności nie został jeszcze ustalony."},
    {49,   L"Your Windows Experience Index needs to be refreshed", L"L'indice prestazioni Windows deve essere aggiornato", L"El Índice de experiencia necesita actualizarse", L"L'indice de performance doit être actualisé", L"Windows Deneyim Dizini yenilenmeli", L"Индекс производительности нуждается в обновлении", L"需要刷新您的 Windows 体验指数", L"Der Windows-Leistungsindex muss aktualisiert werden", L"O Índice de Experiência precisa ser atualizado", L" Indeks wydajności wymaga odświeżenia"},
    {52,   L"New hardware detected",                    L"Nuovo hardware rilevato",                           L"Nuevo hardware detectado",                        L"Nouveau matériel détecté",                    L"Yeni donanım algılandı",                       L"Обнаружено новое оборудование",                 L"检测到新硬件",                                     L"Neue Hardware erkannt",                            L"Novo hardware detectado",                        L"Wykryto nowy sprzęt"},
    {53,   L"Refresh Now",                              L"Aggiorna ora",                                      L"Actualizar ahora",                                 L"Actualiser maintenant",                       L"Şimdi yenile",                                 L"Обновить сейчас",                                 L"立即刷新",                                         L"Jetzt aktualisieren",                             L"Atualizar agora",                                 L"Odśwież teraz"},
    {54,   L"(unrated)",                               L"(senza classificazione)",                                   L"(sin evaluar)",                                    L"(non évalué)",                                 L"(derecelendirilmedi)",                         L"(не оценено)",                                    L"(未评分)",                                         L"(nicht bewertet)",                                 L"(não avaliado)",                                   L"(nie oceniono)"},
    {55,   L"Unable to show performance details",      L"Impossibile visualizzare i dettagli sulle prestazioni", L"No se pueden mostrar los detalles de rendimiento", L"Impossible d'afficher les détails de performance", L"Performans ayrıntıları gösterilemiyor", L"Не удается отобразить сведения о производительности", L"无法显示性能详细信息", L"Details zur Leistung können nicht angezeigt werden", L"Não é possível exibir detalhes de desempenho", L"Nie można wyświetlić szczegółów wydajności"},
    {56,   L"The Windows Experience Index is not applicable while on batteries.", L"L'indice prestazioni Windows non è applicabile durante l'uso della batteria.", L"El Índice de experiencia no es aplicable mientras está con batería.", L"L'indice de performance n'est pas applicable sur batterie.", L"Dizin pildeyken kullanılamaz.", L"Индекс недоступен от батареи.", L"使用电池时不可用。", L"Index bei Akkubetrieb nicht verfügbar.", L"Índice indisponível na bateria.", L"Indeks niedostępny na baterii."},
    {210,  L"An error prevented the solution from being displayed.", L"Un errore ha impedito la visualizzazione della soluzione.", L"Un error impidió que se mostrara la solución.", L"Une erreur a empêché l'affichage de la solution.", L"Bir hata, çözümün görüntülenmesini engelledi.", L"Ошибка не позволила отобразить решение.", L"错误导致无法显示解决方案。", L"Ein Fehler verhinderte die Anzeige der Lösung.", L"Um erro impediu a exibição da solução.", L"Błąd uniemożliwił wyświetlenie rozwiązania."},
    {211,  L"Performance",                              L"Prestazioni",                                      L"Rendimiento",                                      L"Performances",                                 L"Performans",                                   L"Производительность",                              L"性能",                                             L"Leistung",                                        L"Desempenho",                                      L"Wydajność"},
    {402,  L"Administrator privileges required",       L"Richiesti privilegi di amministratore",             L"Se requieren privilegios de administrador",        L"Droits d'administrateur requis",              L"Yönetici ayrıcalıkları gerekli",               L"Требуются права администратора",                 L"需要管理员权限",                                   L"Administratorrechte erforderlich",                 L"Privilégios de administrador necessários",        L"Wymagane uprawnienia administratora"},
    {403,  L"Windows Experience Index score",          L"Punteggio Indice esperienza Windows",               L"Puntuación del Índice de experiencia de Windows", L"Score de l'Índice de performance Windows",     L"Windows Deneyim Dizini puanı",                   L"Оценка индекса производительности Windows",     L"Windows 体验指数分数",                              L"Windows-Leistungsindex-Bewertung",               L"Pontuação do Índice de Experiência do Windows",   L"Wynik indeksu wydajności systemu Windows"},
    {404,  L"Exclamation image",                       L"Immagine punto esclamativo",                        L"Imagen de exclamación",                            L"Image d'exclamation",                          L"Ünlem görseli",                                 L"Изображение восклицания",                         L"感叹号图片",                                         L"Ausrufezeichenbild",                              L"Imagem de exclamação",                             L"Obraz wykrzyknika"},
    {405,  L"Lightbulb image",                          L"Immagine lampadina",                                L"Imagen de bombilla",                               L"Image d'ampoule",                              L"Ampul görseli",                                 L"Изображение лампочки",                           L"灯泡图片",                                           L"Glühbirnenbild",                                  L"Imagem de lâmpada",                                 L"Obraz żarówki"},
    {406,  L"Manufacturer's link image",                L"Immagine collegamento produttore",                  L"Imagen de enlace del fabricante",                  L"Image du lien du fabricant",                   L"Üretici bağlantı görseli",                      L"Изображение ссылки производителя",                L"制造商链接图片",                                     L"Herstellerlinkbild",                               L"Imagem de link do fabricante",                    L"Obraz łącza producenta"},
    {407,  L"Windows Experience Index score (small)",   L"Punteggio Indice esperienza Windows (piccolo)",     L"Puntuación del Índice de experiencia de Windows (pequeño)", L"Score de l'Índice de performance Windows (petit)", L"Windows Deneyim Dizini puanı (küçük)",         L"Оценка индекса производительности Windows (маленькая)", L"Windows 体验指数分数（小）",                              L"Windows-Leistungsindex-Bewertung (klein)",       L"Pontuação do Índice de Experiência do Windows (pequeno)", L"Wynik indeksu wydajności systemu Windows (mały)"},
    {408,  L"score",                                    L"punteggio",                                        L"puntuación",                                      L"score",                                        L"puan",                                         L"оценка",                                          L"分数",                                             L"Bewertung",                                       L"pontuação",                                       L"wynik"},
    {409,  L"Help",                                     L"Guida",                                             L"Ayuda",                                           L"Aide",                                         L"Yardım",                                        L"Справка",                                         L"帮助",                                             L"Hilfe",                                           L"Ajuda",                                          L"Pomoc"},
    {410,  L"Main Window",                              L"Finestra principale",                               L"Ventana principal",                                L"Fenêtre principale",                           L"Ana Pencere",                                   L"Главное окно",                                    L"主窗口",                                            L"Hauptfenster",                                     L"Janela Principal",                                 L"Główne okno"},
    {501,  L"Component",                                L"Componente",                                        L"Componente",                                       L"Composant",                                     L"Bileşen",                                       L"Компонент",                                        L"组件",                                             L"Komponente",                                       L"Componente",                                      L"Składnik"},
    {502,  L"Details",                                  L"Dettagli",                                          L"Detalles",                                         L"Détails",                                        L"Ayrıntılar",                                    L"Сведения",                                         L"详细信息",                                         L"Details",                                         L"Detalhes",                                        L"Szczegóły"},
    {503,  L"Subscore",                                 L"Sottopunteggio",                                    L"Subpuntuación",                                    L"Sous-score",                                    L"Alt puan",                                      L"Подоценка",                                       L"子分数",                                           L"Teilbewertung",                                    L"Subpontuação",                                     L"Podwynik"},
    {504,  L"Base score",                               L"Punteggio di base",                                 L"Puntuación base",                                  L"Score de base",                                 L"Temel puan",                                    L"Базовая оценка",                                   L"基本分数",                                          L"Basisbewertung",                                   L"Pontuação base",                                   L"Wynik podstawowy"},
    {505,  L"Determined by lowest subscore",           L"Determinato dal sottopunteggio più basso",          L"Determinado por la subpuntuación más baja",        L"Déterminé par le sous-score le plus bas",     L"En düşük alt puana göre belirlenir",              L"Определяется наименьшей подоценкой",              L"由最低子分数决定",                                   L"Wird durch die niedrigste Teilbewertung bestimmt",L"Determinado pela subpontuação mais baixa",         L"Określone przez najniższy podwynik"},
    {506,  L"System",                                   L"Sistema",                                           L"Sistema",                                          L"Système",                                       L"Sistem",                                        L"Система",                                          L"系统",                                             L"System",                                          L"Sistema",                                         L"System"},
    {509,  L"Total amount of system memory",            L"Quantità totale di memoria di sistema",            L"Cantidad total de memoria del sistema",           L"Quantité totale de mémoire système",           L"Toplam sistem belleği miktarı",                   L"Общий объем памяти системы",                     L"系统内存总量",                                       L"Gesamtmenge des Systemspeichers",                 L"Quantidade total de memória do sistema",          L"Całkowita ilość pamięci systemowej"},
    {510,  L"Storage",                                  L"Archiviazione",                                     L"Almacenamiento",                                   L"Stockage",                                      L"Depolama",                                      L"Хранилище",                                        L"存储",                                             L"Speicher",                                        L"Armazenamento",                                   L"Magazyn"},
    {511,  L"Total size of hard disk(s)",              L"Dimensione totale del disco rigido",              L"Tamaño total del disco duro",                      L"Taille totale du disque dur",                  L"Toplam sabit disk boyutu",                       L"Общий размер жесткого диска",                    L"硬盘总大小",                                         L"Gesamtgröße der Festplatte(n)",                   L"Tamanho total do(s) disco(s) rígido(s)",          L"Całkowity rozmiar dysku(twardych)"},
    {512,  L"Graphics",                                 L"Grafica",                                           L"Gráficos",                                         L"Graphiques",                                    L"Grafik",                                        L"Графика",                                          L"图形",                                             L"Grafik",                                          L"Elementos gráficos",                               L"Grafika"},
    {513,  L"Display adapter type",                     L"Tipo di scheda video",                              L"Tipo de adaptador de pantalla",                   L"Type de carte d'affichage",                    L"Görüntü bağdaştırıcı türü",                    L"Тип видеоадаптера",                               L"显示适配器类型",                                      L"Displayadaptertyp",                                L"Tipo de adaptador de vídeo",                      L"Typ karty graficznej"},
    {515,  L"Primary monitor resolution",               L"Risoluzione monitor principale",                    L"Resolución del monitor principal",                L"Résolution du moniteur principal",             L"Birincil monitör çözünürlüğü",                   L"Разрешение основного монитора",                  L"主显示器分辨率",                                     L"Auflösung des primären Monitors",                L"Resolução do monitor primário",                   L"Rozdzielczość głównego monitora"},
    {516,  L"Secondary monitor resolution",             L"Risoluzione monitor secondario",                   L"Resolución del monitor secundario",               L"Résolution du moniteur secondaire",           L"İkincil monitör çözünürlüğü",                   L"Разрешение дополнительного монитора",             L"辅助显示器分辨率",                                     L"Auflösung des sekundären Monitors",               L"Resolução do monitor secundário",                 L"Rozdzielczość dodatkowego monitora"},
    {517,  L"%ix%i",                                    L"%ix%i",                                            L"%ix%i",                                           L"%ix%i",                                        L"%ix%i",                                        L"%ix%i",                                            L"%ix%i",                                           L"%ix%i",                                           L"%ix%i",                                          L"%ix%i"},
    {518,  L"DirectX version",                          L"Versione DirectX",                                  L"Versión de DirectX",                               L"Version de DirectX",                            L"DirectX sürümü",                                 L"Версия DirectX",                                  L"DirectX 版本",                                      L"DirectX-Version",                                   L"Versão do DirectX",                                L"Wersja DirectX"},
    {519,  L"Print this page",                         L"Stampa questa pagina",                             L"Imprimir esta página",                             L"Imprimer cette page",                          L"Bu sayfayı yazdır",                             L"Печать этой страницы",                            L"打印此页",                                          L"Diese Seite drucken",                              L"Imprimir esta página",                             L"Drukuj tę stronę"},
    {520,  L"Network",                                  L"Rete",                                              L"Red",                                              L"Réseau",                                        L"Ağ",                                            L"Сеть",                                             L"网络",                                             L"Netzwerk",                                         L"Rede",                                           L"Sieć"},
    {521,  L"Model",                                    L"Modello",                                           L"Modelo",                                           L"Modèle",                                        L"Model",                                         L"Модель",                                           L"型号",                                             L"Modell",                                          L"Modelo",                                         L"Model"},
    {522,  L"Manufacturer",                             L"Produttore",                                        L"Fabricante",                                       L"Fabricant",                                     L"Üretici",                                       L"Производитель",                                    L"制造商",                                           L"Hersteller",                                      L"Fabricante",                                      L"Producent"},
    {523,  L"Network Adapter",                          L"Scheda di rete",                                    L"Adaptador de red",                                  L"Adaptateur réseau",                             L"Ağ Bağdaştırıcısı",                              L"Сетевой адаптер",                                  L"网络适配器",                                        L"Netzwerkadapter",                                  L"Adaptador de Rede",                                L"Karta sieciowa"},
    {524,  L"More details about my computer",           L"Ulteriori informazioni sul computer",               L"Más detalles sobre mi equipo",                     L"Plus de détails sur mon ordinateur",            L"Bilgisayarım hakkında daha fazla ayrıntı",        L"Подробнее о моем компьютере",                    L"关于我的电脑的更多信息",                                L"Weitere Details zu meinem Computer",               L"Mais detalhes sobre meu computador",                L"Więcej informacji o moim komputerze"},
    {525,  L"%s GB",                                    L"%s GB",                                            L"%s GB",                                           L"%s Go",                                         L"%s GB",                                         L"%s ГБ",                                            L"%s GB",                                           L"%s GB",                                           L"%s GB",                                          L"%s GB"},
    {526,  L"%s MB",                                    L"%s MB",                                            L"%s MB",                                           L"%s Mo",                                         L"%s MB",                                         L"%s МБ",                                            L"%s MB",                                           L"%s MB",                                           L"%s MB",                                          L"%s MB"},
    {527,  L"%s KB",                                    L"%s KB",                                            L"%s KB",                                           L"%s Ko",                                         L"%s KB",                                         L"%s КБ",                                            L"%s KB",                                           L"%s KB",                                           L"%s KB",                                          L"%s KB"},
    {528,  L"%s B",                                     L"%s B",                                             L"%s B",                                            L"%s o",                                          L"%s B",                                          L"%s Б",                                             L"%s B",                                            L"%s B",                                            L"%s B",                                           L"%s B"},
    {529,  L"%s B",                                     L"%s B",                                             L"%s B",                                            L"%s o",                                          L"%s B",                                          L"%s Б",                                             L"%s B",                                            L"%s B",                                            L"%s B",                                           L"%s B"},
    {530,  L"%s RAM",                                   L"%s RAM",                                           L"%s RAM",                                          L"%s RAM",                                        L"%s RAM",                                        L"%s ОЗУ",                                           L"%s RAM",                                          L"%s RAM",                                          L"%s RAM",                                         L"%s RAM"},
    {531,  L"DirectX 9",                                L"DirectX 9",                                        L"DirectX 9",                                        L"DirectX 9",                                     L"DirectX 9",                                     L"DirectX 9",                                        L"DirectX 9",                                        L"DirectX 9",                                        L"DirectX 9",                                       L"DirectX 9"},
    {532,  L"DirectX 8 or earlier",                     L"DirectX 8 o precedente",                             L"DirectX 8 o anterior",                              L"DirectX 8 ou antérieur",                        L"DirectX 8 veya öncesi",                          L"DirectX 8 или более ранняя версия",              L"DirectX 8 或更早版本",                                L"DirectX 8 oder früher",                            L"DirectX 8 ou anterior",                            L"DirectX 8 lub wcześniejsza"},
    {533,  L"Disk partition (%c:)",                     L"Partizione disco (%c:)",                           L"Partición de disco (%c:)",                         L"Partition de disque (%c:)",                    L"Disk bölümü (%c:)",                              L"Раздел диска (%c:)",                               L"磁盘分区 (%c:)",                                     L"Datenträgerpartition (%c:)",                       L"Partição do disco (%c:)",                           L"Partycja dysku (%c:)"},
    {534,  L"%s Free (%s Total)",                       L"%s disponibili (%s totali)",                       L"%s libres (%s totales)",                           L"%s libres (%s totaux)",                         L"%s boş (%s toplam)",                             L"%s свободно (%s всего)",                          L"%s 可用（共 %s）",                                     L"%s frei (%s gesamt)",                              L"%s livre (%s total)",                             L"%s wolne (%s łącznie)"},
    {535,  L"Media drive (%c:)",                        L"Unità multimediale (%c:)",                         L"Unidad multimedia (%c:)",                          L"Lecteur média (%c:)",                           L"Ortam sürücüsü (%c:)",                           L"Привод для носителей (%c:)",                      L"媒体驱动器 (%c:)",                                    L"Medienlaufwerk (%c:)",                             L"Unidade de mídia (%c:)",                           L"Napęd multimedialny (%c:)"},
    {536,  L"DVD",                                      L"DVD",                                               L"DVD",                                              L"DVD",                                           L"DVD",                                           L"DVD",                                              L"DVD",                                              L"DVD",                                             L"DVD",                                            L"DVD"},
    {537,  L"CD",                                       L"CD",                                                L"CD",                                               L"CD",                                            L"CD",                                            L"CD",                                               L"CD",                                               L"CD",                                              L"CD",                                             L"CD"},
    {538,  L"%s MB",                                    L"%s MB",                                            L"%s MB",                                           L"%s Mo",                                         L"%s MB",                                         L"%s МБ",                                            L"%s MB",                                           L"%s MB",                                           L"%s MB",                                          L"%s MB"},
    {539,  L"Total available graphics memory",          L"Memoria grafica totale disponibile",               L"Memoria gráfica total disponible",                L"Mémoire graphique totale disponible",          L"Toplam kullanılabilir grafik belleği",            L"Всего доступной памяти графики",                  L"总可用图形内存",                                        L"Gesamte verfügbare Grafikspeicher",               L"Memória gráfica total disponível",                 L"Całkowita dostępna pamięć graficzna"},
    {540,  L"Shared system memory",                    L"Memoria di sistema condivisa",                      L"Memoria del sistema compartida",                   L"Mémoire système partagée",                      L"Paylaşılan sistem belleği",                      L"Общая системная память",                          L"共享系统内存",                                         L"Freigegebener Systemspeicher",                    L"Memória do sistema compartilhada",                 L"Współdzielona pamięć systemowa"},
    {541,  L"Dedicated graphics memory",               L"Memoria grafica dedicata",                          L"Memoria gráfica dedicada",                         L"Mémoire graphique dédiée",                     L"Ayrılmış grafik belleği",                         L"Выделенная память графики",                       L"专用图形内存",                                         L"Dedizierter Grafikspeicher",                       L"Memória gráfica dedicada",                         L"Dedykowana pamięć graficzna"},
    {542,  L"Dedicated system memory",                 L"Memoria di sistema dedicata",                      L"Memoria del sistema dedicada",                     L"Mémoire système dédiée",                        L"Ayrılmış sistem belleği",                        L"Выделенная системная память",                    L"专用系统内存",                                         L"Dedizierter Systemspeicher",                       L"Memória do sistema dedicada",                      L"Dedykowana pamięć systemowa"},
    {543,  L"Not detected",                             L"Non rilevato",                                      L"No detectado",                                     L"Non détecté",                                   L"Algılanmadı",                                    L"Не обнаружено",                                    L"未检测到",                                          L"Nicht erkannt",                                    L"Não detectado",                                    L"Nie wykryto"},
    {544,  L"System type",                              L"Tipo di sistema",                                   L"Tipo de sistema",                                   L"Type de système",                               L"Sistem türü",                                    L"Тип системы",                                      L"系统类型",                                          L"Systemtyp",                                        L"Tipo de sistema",                                  L"Typ systemu"},
    {545,  L"Number of processor cores",               L"Numero di core del processore",                    L"Número de núcleos del procesador",                 L"Nombre de cœurs de processeur",                L"İşlemci çekirdek sayısı",                        L"Количество ядер процессора",                     L"处理器内核数",                                         L"Anzahl der Prozessorkerne",                        L"Número de núcleos do processador",                 L"Liczba rdzeni procesora"},
    {546,  L"64-bit capable",                           L"Capace 64 bit",                                    L"Capacidad de 64 bits",                             L"Compatible 64 bits",                            L"64 bit destekli",                                 L"64-разрядный",                                     L"64 位",                                             L"64-Bit-fähig",                                     L"Capaz de 64 bits",                                 L"64-bitowy"},
    {547,  L"Yes",                                      L"Sì",                                                L"Sí",                                              L"Oui",                                           L"Evet",                                           L"Да",                                              L"是",                                               L"Ja",                                              L"Sim",                                            L"Tak"},
    {548,  L"No",                                       L"No",                                                L"No",                                               L"Non",                                           L"Hayır",                                          L"Нет",                                              L"否",                                               L"Nein",                                             L"Não",                                            L"Nie"},
    {549,  L"32-bit operating system",                 L"Sistema operativo a 32 bit",                       L"Sistema operativo de 32 bits",                     L"Système d'exploitation 32 bits",               L"32 bit işletim sistemi",                          L"32-разрядная операционная система",               L"32 位操作系统",                                        L"32-Bit-Betriebssystem",                            L"Sistema operacional de 32 bits",                    L"32-bitowy system operacyjny"},
    {550,  L"64-bit operating system",                 L"Sistema operativo a 64 bit",                       L"Sistema operativo de 64 bits",                     L"Système d'exploitation 64 bits",               L"64 bit işletim sistemi",                          L"64-разрядная операционная система",               L"64 位操作系统",                                        L"64-Bit-Betriebssystem",                            L"Sistema operacional de 64 bits",                    L"64-bitowy system operacyjny"},
    {552,  L"Display adapter driver version",          L"Versione driver scheda video",                     L"Versión del controlador del adaptador de pantalla", L"Version du pilote de la carte d'affichage",    L"Ekran bağdaştırıcı sürücü sürümü",               L"Версия драйвера видеоадаптера",                  L"显示适配器驱动程序版本",                                  L"Version des Displayadaptertreibers",               L"Versão do driver do adaptador de vídeo",           L"Wersja sterownika karty graficznej"},
    {553,  L"Notes",                                    L"Note",                                              L"Notas",                                            L"Notes",                                         L"Notlar",                                         L"Заметки",                                         L"备注",                                             L"Notizen",                                          L"Notas",                                          L"Uwagi"},
    {554,  L"The gaming graphics score is based on the primary graphics adapter.  If this system has linked or multiple graphics adapters, some software applications may see additional performance benefits.", L"Il punteggio della grafica dei giochi è basato sulla scheda grafica principale. Se questo sistema ha schede grafiche collegate o multiple, alcune applicazioni software potrebbero trarre ulteriori benefici in termini di prestazioni.", L"La puntuación de los gráficos de juegos se basa en el adaptador de gráficos principal. Si este sistema tiene adaptadores de gráficos vinculados o múltiples, algunas aplicaciones de software pueden ver beneficios de rendimiento adicionales.", L"Le score des graphiques de jeu est basé sur l'adaptateur graphique principal. Si ce système dispose d'adaptateurs graphiques liés ou multiples, certaines applications logicielles peuvent bénéficier de performances supplémentaires.", L"Oyun grafik puanı birincil grafik bağdaştırıcıya dayalıdır. Bu sistemde bağlantılı veya birden fazla grafik bağdaştırıcı varsa, bazı yazılım uygulamaları ek performans avantajları görebilir.", L"Оценка игровой графики основана на основном графическом адаптере. Если в этой системе есть связанные или множественные графические адаптеры, некоторые программные приложения могут получить дополнительные преимущества в производительности.", L"游戏图形分数基于主图形适配器。如果此系统有链接或多个图形适配器，某些软件应用程序可能会看到额外的性能优势。", L"Die Spiele-Grafikbewertung basiert auf dem primären Grafikadapter. Wenn dieses System über verknüpfte oder mehrere Grafikadapter verfügt, können einige Softwareanwendungen zusätzliche Leistungsvorteile sehen.", L"A pontuação dos gráficos de jogos é baseada no adaptador gráfico primário. Se este sistema tiver adaptadores de gráficos vinculados ou múltiplos, alguns aplicativos de software podem ver benefícios de desempenho adicionais.", L"Wynik grafiki gier jest oparty na podstawowym adapterze graficznym. Jeśli ten system ma połączone lub wiele kart graficznych, niektóre aplikacje mogą odnieść dodatkowe korzyści w zakresie wydajności."},
    {555,  L"DirectX 11 or better",                    L"DirectX 11 o superiore",                            L"DirectX 11 o superior",                            L"DirectX 11 ou supérieur",                        L"DirectX 11 veya üstü",                           L"DirectX 11 или выше",                             L"DirectX 11 或更高版本",                               L"DirectX 11 oder besser",                           L"DirectX 11 ou melhor",                              L"DirectX 11 lub lepszy"},
    {556,  L"DirectX 10.1",                             L"DirectX 10.1",                                      L"DirectX 10.1",                                     L"DirectX 10.1",                                   L"DirectX 10.1",                                   L"DirectX 10.1",                                    L"DirectX 10.1",                                     L"DirectX 10.1",                                    L"DirectX 10.1",                                    L"DirectX 10.1"},
    {557,  L"DirectX 10",                               L"DirectX 10",                                        L"DirectX 10",                                      L"DirectX 10",                                    L"DirectX 10",                                     L"DirectX 10",                                      L"DirectX 10",                                      L"DirectX 10",                                      L"DirectX 10",                                      L"DirectX 10"},
    {600,  L"View Details Error",                       L"Errore visualizzazione dettagli",                   L"Error al ver los detalles",                        L"Erreur d'affichage des détails",               L"Ayrıntıları Görüntüleme Hatası",                 L"Ошибка просмотра сведений",                       L"查看详细信息错误",                                        L"Fehler beim Anzeigen der Details",                L"Erro ao Exibir Detalhes",                           L"Błąd wyświetlania szczegółów"},
    {601,  L"Could not display details page.",          L"Impossibile visualizzare la pagina dei dettagli.", L"No se pudo mostrar la página de detalles.",        L"Impossible d'afficher la page de détails.",     L"Ayrıntılar sayfası görüntülenemedi.",             L"Не удалось отобразить страницу сведений.",       L"无法显示详细信息页。",                                   L"Seite konnte nicht angezeigt werden.",             L"Não foi possível exibir a página de detalhes.",    L"Nie można wyświetlić strony szczegółów."},
    {602,  L"Create Details Error",                     L"Errore creazione dettagli",                         L"Error al crear los detalles",                      L"Erreur de création des détails",               L"Ayrıntılar Oluşturma Hatası",                    L"Ошибка создания сведений",                        L"创建详细信息错误",                                        L"Fehler beim Erstellen der Details",                L"Erro ao Criar Detalhes",                           L"Błąd tworzenia szczegółów"},
    {603,  L"Could not create details page.",           L"Impossibile creare la pagina dei dettagli.",        L"No se pudo crear la página de detalles.",          L"Impossible de créer la page de détails.",       L"Ayrıntılar sayfası oluşturulamadı.",               L"Не удалось создать страницу сведений.",           L"无法创建详细信息页。",                                   L"Seite konnte nicht erstellt werden.",              L"Não foi possível criar a página de detalhes.",     L"Nie można utworzyć strony szczegółów."},
    {650,  L"Performance Information and Tools",        L"Informazioni e strumenti sulle prestazioni",        L"Información y herramientas de rendimiento",        L"Informations et outils de performance",        L"Performans Bilgileri ve Araçları",        L"Счетчики и средства производительности",        L"性能信息和工具",        L"Leistungsinformationen und -tools",        L"Informações e Ferramentas de Desempenho",        L"Informacje i narzędzia dotyczące wydajności"},
    {652,  L"The Windows Experience Index for your system could not be computed.", L"Non è stato possibile calcolare l'Indice esperienza Windows per il sistema.", L"No se pudo calcular el Índice de experiencia de Windows para su sistema.", L"L'indice de performance Windows pour votre système n'a pas pu être calculé.", L"Sisteminiz için Windows Deneyim Dizini hesaplanamadı.", L"Не удалось вычислить индекс производительности Windows для вашей системы.", L"无法计算系统的 Windows 体验指数。", L"Der Windows-Leistungsindex für Ihr System konnte nicht berechnet werden.", L"O Índice de Experiência do Windows para seu sistema não pôde ser calculado.", L"Nie można było obliczyć indeksu wydajności systemu Windows dla Twojego systemu."},
    {653,  L"Windows was unable to calculate the Windows Experience Index.", L"Windows non è riuscito a calcolare l'Indice esperienza Windows.", L"Windows no pudo calcular el Índice de experiencia de Windows.", L"Windows n'a pas pu calculer l'indice de performance Windows.", L"Windows, Windows Deneyim Dizinini hesaplayamadı.", L"Windows не удалось вычислить индекс производительности Windows.", L"Windows 无法计算 Windows 体验指数。", L"Windows konnte den Windows-Leistungsindex nicht berechnen.", L"O Windows não pôde calcular o Índice de Experiência do Windows.", L"System Windows nie mógł obliczyć indeksu wydajności systemu Windows."},
    {654,  L"Cannot read data from the registry",      L"Impossibile leggere i dati dal registro",          L"No se pueden leer los datos del registro",         L"Impossible de lire les données du registre",   L"Kayıt defterinden veri okunamıyor",               L"Не удается прочитать данные из реестра",          L"无法从注册表读取数据",                                   L"Daten können nicht aus der Registrierung gelesen werden", L"Não é possível ler dados do registro",              L"Nie można odczytać danych z rejestru"},
    {655,  L"Close",                                    L"Chiudi",                                            L"Cerrar",                                           L"Fermer",                                        L"Kapat",                                          L"Закрыть",                                         L"关闭",                                             L"Schließen",                                        L"Fechar",                                          L"Zamknij"},
    {710,  L"Standard VGA Graphics Adaptor",             L"Scheda grafica VGA standard",                       L"Adaptador de gráficos VGA estándar",                L"Adaptateur graphique VGA standard",            L"Standart VGA Grafik Bağdaştırıcısı",             L"Стандартный графический адаптер VGA",            L"标准 VGA 图形适配器",                                  L"Standard-VGA-Grafikadapter",                       L"Adaptador Gráfico VGA Padrão",                    L"Standardowy adapter graficzny VGA"},
    {711,  L"Desktop graphics performance",             L"Prestazioni desktop per Windows Aero",                       L"Rendimiento de gráficos de escritorio",            L"Performances graphiques du Bureau",            L"Masaüstü grafik performansı",                   L"Производительность графики рабочего стола",      L"桌面图形性能",                                        L"Desktopgrafikleistung",                             L"Desempenho gráfico da área de trabalho",          L"Wydajność grafiki pulpitu"},
    {1100, L"Modify the schedule used to automatically defragment your hard disk.", L"Modifica la pianificazione utilizzata per la deframmentazione automatica del disco rigido.", L"Modificar la programación usada para desfragmentar automáticamente el disco duro.", L"Modifier la planification utilisée pour défragmenter automatiquement votre disque dur.", L"Sabit diskinizi otomatik olarak birleştirmek için kullanılan zamanlamayı değiştirin.", L"Измените расписание, используемое для автоматической дефрагментации жесткого диска.", L"修改用于自动整理硬盘碎片的计划。", L"Ändern Sie den Zeitplan, der zur automatischen Defragmentierung Ihrer Festplatte verwendet wird.", L"Modificar o agendamento usado para desfragmentar automaticamente o disco rígido.", L"Modyfikuj harmonogram używany do automatycznego defragmentowania dysku twardego."},
    {1101, L"Component",                                L"Componente",                                        L"Componente",                                       L"Composant",                                     L"Bileşen",                                       L"Компонент",                                        L"组件",                                             L"Komponente",                                       L"Componente",                                      L"Składnik"},
    {1102, L"What is rated",                           L"Elementi classificati",                               L"Qué se evalúa",                                    L"Élément évalué",                                L"Ne değerlendirilir",                              L"Что оценивается",                                  L"评估内容",                                           L"Was wird bewertet",                                 L"O que é avaliado",                                 L"Co jest oceniane"},
    {1103, L"Subscore",                                 L"Sottopunteggio",                                    L"Subpuntuación",                                    L"Sous-score",                                    L"Alt puan",                                      L"Подоценка",                                       L"子分数",                                           L"Teilbewertung",                                    L"Subpontuação",                                     L"Podwynik"},
    {1104, L"Base score",                               L"Punteggio di base",                                 L"Puntuación base",                                  L"Score de base",                                 L"Temel puan",                                    L"Базовая оценка",                                   L"基本分数",                                          L"Basisbewertung",                                   L"Pontuação base",                                   L"Wynik podstawowy"},
    {1110, L"View advanced information about your computer's performance", L"Visualizza informazioni avanzate sulle prestazioni del computer", L"Ver información avanzada sobre el rendimiento de su equipo", L"Afficher des informations avancées sur les performances de votre ordinateur", L"Bilgisayarınızın performansı hakkında gelişmiş bilgileri görüntüle", L"Просмотр дополнительных сведений о производительности компьютера", L"查看有关计算机性能的高级信息", L"Erweiterte Informationen zur Leistung Ihres Computers anzeigen", L"Exibir informações avançadas sobre o desempenho do seu computador", L"Wyświetl zaawansowane informacje o wydajności komputera"},
    {1111, L"Determined by lowest subscore",           L"Determinato dal sottopunteggio più basso",          L"Determinado por la subpuntuación más baja",        L"Déterminé par le sous-score le plus bas",     L"En düşük alt puana göre belirlenir",              L"Определяется наименьшей подоценкой",              L"由最低子分数决定",                                   L"Wird durch die niedrigste Teilbewertung bestimmt",L"Determinado pela subpontuação mais baixa",         L"Określone przez najniższy podwynik"},
    {1112, L"View and print detailed performance and system information", L"Visualizza e stampa informazioni dettagliate sulle prestazioni e sul sistema", L"Ver e imprimir información detallada del rendimiento y del sistema", L"Afficher et imprimer les informations détaillées sur les performances et le système", L"Ayrıntılı performans ve sistem bilgilerini görüntüle ve yazdır", L"Просмотр и печать подробных сведений о производительности и системе", L"查看和打印详细的性能和系统信息", L"Detaillierte Leistungs- und Systeminformationen anzeigen und drucken", L"Exibir e imprimir informações detalhadas de desempenho e sistema", L"Wyświetl i wydrukuj szczegółowe informacje o wydajności i systemie"},
    {1115, L"Generate a system health report",          L"Genera un report sullo stato del sistema",          L"Generar un informe de estado del sistema",        L"Générer un rapport d'état du système",         L"Sistem sağlığı raporu oluştur",                   L"Создать отчет о работоспособности системы",      L"生成系统运行状况报告",                                      L"Systemzustandsbericht erstellen",                  L"Gerar um relatório de integridade do sistema",     L"Utwórz raport o kondycji systemu"},
    {1116, L"View details about system health and performance.", L"Visualizza dettagli su stato e prestazioni del sistema.", L"Ver detalles sobre el estado y el rendimiento del sistema.", L"Voir les détails sur l'état et les performances du système.", L"Sistem durumu ve performansı hakkında ayrıntıları görüntüle.", L"Просмотр сведений о работоспособности и производительности системы.", L"查看系统运行状况和性能详细信息。", L"Details zum Systemzustand und zur Leistung anzeigen.", L"Exibir detalhes sobre a integridade e o desempenho do sistema.", L"Wyświetl szczegóły dotyczące kondycji i wydajności systemu."},
    {1117, L"Enter Your Title",                         L"Immetti il titolo",                                 L"Introduzca su título",                              L"Entrez votre titre",                            L"Başlığınızı girin",                               L"Введите название",                                  L"输入标题",                                           L"Titel eingeben",                                   L"Digite seu título",                                L"Wpisz tytuł"},
    {1118, L"Default solutions text",                   L"Testo predefinito delle soluzioni",                L"Texto de soluciones predeterminado",               L"Texte des solutions par défaut",               L"Varsayılan çözümler metni",                       L"Текст решений по умолчанию",                      L"默认解决方案文本",                                      L"Standardlösungs-text",                              L"Texto de soluções padrão",                         L"Domyślny tekst rozwiązań"},
    {1119, L"The Windows Experience Index assesses key system components on a scale of 1.0 to 9.9.", L"L'indice prestazioni Windows valuta i componenti chiave del sistema in base a una scala da 1,0 a 9,9.", L"El Índice de experiencia de Windows evalúa los componentes principales del sistema en una escala de 1,0 a 9,9.", L"L'indice de performance Windows évalue les composants principaux du système sur une échelle de 1,0 à 9,9.", L"Windows Deneyim Dizini, temel sistem bileşenlerini 1,0 ile 9,9 arasında bir ölçekte değerlendirir.", L"Индекс производительности Windows оценивает основные компоненты системы по шкале от 1,0 до 9,9.", L"Windows 体验指数按 1.0 到 9.9 的等级评估关键系统组件。", L"Der Windows-Leistungsindex bewertet wichtige Systemkomponenten auf einer Skala von 1,0 bis 9,9.", L"O Índice de Experiência do Windows avalia os principais componentes do sistema em uma escala de 1,0 a 9,9.", L"Indeks wydajności systemu Windows ocenia główne składniki systemu w skali od 1,0 do 9,9."},
    {1120, L"Your scores are current",                 L"I punteggi sono aggiornati",                       L"Las puntuaciones están actualizadas",            L"Vos scores sont à jour",                       L"Puanlarınız güncel",                              L"Ваши оценки актуальны",                           L"您的分数是当前的",                                      L"Ihre Bewertungen sind aktuell",                    L"Seus pontuações estão atualizadas",                L"Twoje wyniki są aktualne"},
    {1121, L"Clear all Windows Experience Index scores and re-rate the system", L"Cancella tutti i punteggi dell'Indice esperienza Windows e ricalcola il sistema", L"Borrar todas las puntuaciones del Índice de experiencia de Windows y recalificar el sistema", L"Effacer tous les scores de l'Índice de performance Windows et re-évaluer le système", L"Tüm Windows Deneyim Dizini puanlarını temizle ve sistemi yeniden değerlendir", L"Очистить все оценки индекса производительности Windows и переоценить систему", L"清除所有 Windows 体验指数分数并重新评估系统", L"Alle Windows-Leistungsindex-Bewertungen löschen und das System neu bewerten", L"Limpar todas as pontuações do Índice de Experiência do Windows e reavaliar o sistema", L"Wyczyść wszystkie wyniki indeksu wydajności systemu Windows i ponownie oceń system"},
    {1122, L"Force a complete re-run of all Windows Experience Index tests.", L"Forza una nuova esecuzione completa di tutti i test dell'Indice esperienza Windows.", L"Forzar una nueva ejecución completa de todas las pruebas del Índice de experiencia de Windows.", L"Forcer une nouvelle exécution complète de tous les tests de l'indice de performance Windows.", L"Tüm Windows Deneyim Dizini testlerinin tam yeniden çalıştırılmasını zorla", L"Принудительно выполнить полный повторный запуск всех тестов индекса производительности Windows.", L"强制重新运行所有 Windows 体验指数测试。", L"Ein vollständiges erneutes Ausführen aller Windows-Leistungsindex-Tests erzwingen.", L"Forçar uma nova execução completa de todos os testes do Índice de Experiência do Windows.", L"Wymuś pełne ponowne uruchomienie wszystkich testów indeksu wydajności systemu Windows."},
    {1123, L"Open Resource Monitor",                   L"Apri Monitor risorse",                              L"Abrir el Monitor de recursos",                     L"Ouvrir le Moniteur de ressources",             L"Kaynak İzleyicisini Aç",                          L"Открыть Монитор ресурсов",                        L"打开资源监视器",                                        L"ressourcenmonitor öffnen",                         L"Abrir o Monitor de Recursos",                      L"Otwórz Monitor zasobów"},
    {1124, L"View real-time system resource usage and manage active services and applications.", L"Visualizza l'utilizzo delle risorse di sistema in tempo reale e gestisci servizi e applicazioni attivi.", L"Ver el uso de recursos del sistema en tiempo real y administrar servicios y aplicaciones activos.", L"Voir l'utilisation des ressources système en temps réel et gérer les services et applications actifs.", L"Gerçek zamanlı sistem kaynak kullanımını görüntüle ve etkin hizmetleri ve uygulamaları yönet.", L"Просмотр использования системных ресурсов в реальном времени и управление активными службами и приложениями.", L"查看实时系统资源使用情况并管理活动的服务应用程序。", L"Echtzeit-Systemressourcennutzung anzeigen und aktive Dienste und Anwendungen verwalten.", L"Exibir uso de recursos do sistema em tempo real e gerenciar serviços e aplicativos ativos.", L"Wyświetl użycie zasobów systemowych w czasie rzeczywistym i zarządzaj aktywnymi usługami i aplikacjami."},
    {1173, L"Open Disk Defragmenter",                  L"Apri Deframmentazione disco",                      L"Abrir el Desfragmentador de disco",               L"Ouvrir le Défragmenteur de disque",            L"Disk Birleştiriciyi Aç",                         L"Открыть Дефрагментацию диска",                    L"打开磁盘碎片整理程序",                                      L"Datenträgeroptimierung öffnen",                   L"Abrir o Desfragmentador de Disco",                L"Otwórz defragmentator dysku"},
    {1182, L"Rate and improve your computer's performance", L"Valuta e migliora le prestazioni del computer", L"Califica y mejora el rendimiento del equipo", L"Évaluer et améliorer les performances de l'ordinateur", L"Bilgisayarınızın performansını derecelendirin ve geliştirin", L"Оценить и повысить производительность компьютера", L"为计算机的性能评分并进行改进", L"Bewerten und Verbessern der Computerleistung", L"Classifique e melhore o desempenho do computador", L"Oceń i popraw wydajność komputera"},
    {1183, L"Performance issues",                       L"Problemi di prestazioni",                           L"Problemas de rendimiento",                         L"Problèmes de performances",                     L"Performans sorunları",                           L"Проблемы с производительностью",                  L"性能问题",                                           L"Leistungsprobleme",                                L"Problemas de desempenho",                          L"Problemy z wydajnością"},
    {1184, L"No issues reported.  ",                    L"Nessun problema segnalato.  ",                       L"No se han notado problemas.  ",                     L"Aucun problème signalé.  ",                      L"Hiçbir sorun bildirilmedi.  ",                     L"Проблем не обнаружено.  ",                          L"未报告任何问题。  ",                                    L"Keine Probleme gemeldet.  ",                         L"Nenhum problema relatado.  ",                        L"Nie zgłoszono żadnych problemów.  "},
    {1185, L"Your computer has a Windows Experience Index base score of", L"Il computer ha un punteggio di base dell'Indice esperienza Windows di", L"Su equipo tiene una puntuación base del Índice de experiencia de Windows de", L"Votre ordinateur a un score de base de l'indice de performance Windows de", L"Bilgisayarınız Windows Deneyim Dizini temel puanına sahip", L"Ваш компьютер имеет базовую оценку индекса производительности Windows", L"您的计算机的 Windows 体验指数基础分数为", L"Ihr Computer hat eine Windows-Leistungsindex-Basisbewertung von", L"Seu computador tem uma pontuação base do Índice de Experiência do Windows de", L"Twój komputer ma podstawowy wynik indeksu wydajności systemu Windows równy"},
    {1186, L"Rate this computer",                       L"Valuta il computer",                                L"Evaluar este equipo",                              L"Évaluer cet ordinateur",                        L"Bu bilgisayarı derecelendir",                    L"Оценить компьютер",                                L"评估此计算机",                                        L"Diesen Computer bewerten",                         L"Avaliar este computador",                         L"Oceń ten komputer"},
    {1188, L"Re-run the assessment",                   L"Esegui di nuovo la valutazione",                   L"Repetir la evaluación",                             L"Réexécuter l'évaluation",                        L"Değerlendirmeyi yeniden çalıştır",               L"Повторить оценку",                                  L"重新运行评估",                                         L"Bewertung wiederholen",                             L"Repetir a avaliação",                               L"Uruchom ponownie ocenę"},
    {1189, L"View performance details in Event log",    L"Visualizza i dettagli sulle prestazioni nel log eventi", L"Ver los detalles de rendimiento en el registro de eventos", L"Afficher les détails de performance dans le journal des événements", L"Olay günlüğünde performans ayrıntılarını görüntüle", L"Просмотр сведений о производительности в журнале событий", L"在事件日志中查看性能详细信息", L"Leistungsdetails in der Ereignisanzeige anzeigen", L"Exibir detalhes de desempenho no log de eventos", L"Wyświetl szczegóły wydajności w dzienniku zdarzeń"},
    {1190, L"View details of problems affecting Windows performance.", L"Visualizza i dettagli dei problemi che influenzano le prestazioni di Windows.", L"Ver detalles de los problemas que afectan el rendimiento de Windows.", L"Voir les détails des problèmes affectant les performances de Windows.", L"Windows performansını etkileyen sorunların ayrıntılarını görüntüle.", L"Просмотр сведений о проблемах, влияющих на производительность Windows.", L"查看影响 Windows 性能的问题的详细信息。", L"Details zu Problemen anzeigen, die die Windows-Leistung beeinträchtigen.", L"Exibir detalhes de problemas que afetam o desempenho do Windows.", L"Wyświetl szczegóły problemów wpływających na wydajność systemu Windows."},
    {1191, L"View advanced system details in System Information", L"Visualizza i dettagli avanzati del sistema in Informazioni di sistema", L"Ver detalles avanzados del sistema en Información del sistema", L"Afficher les détails système avancés dans Informations système", L"Sistem Bilgilerinde gelişmiş sistem ayrıntılarını görüntüle", L"Просмотр дополнительных сведений о системе в разделе Сведения о системе", L"在系统信息中查看高级系统详细信息", L"Erweiterte Systemdetails in Systeminformationen anzeigen", L"Exibir detalhes avançados do sistema em Informações do Sistema", L"Wyświetl zaawansowane szczegóły systemu w Informacjach o systemie"},
    {1192, L"View details about the hardware and software components on your computer.", L"Visualizza i dettagli sui componenti hardware e software del computer.", L"Ver detalles sobre los componentes de hardware y software de su equipo.", L"Voir les détails sur les composants matériels et logiciels de votre ordinateur.", L"Bilgisayarınızdaki donanım ve yazılım bileşenleri hakkında ayrıntıları görüntüle.", L"Просмотр сведений о компонентах оборудования и программного обеспечения компьютера.", L"查看有关计算机硬件和软件组件的详细信息。", L"Details zu den Hardware- und Softwarekomponenten auf Ihrem Computer anzeigen.", L"Exibir detalhes sobre os componentes de hardware e software do seu computador.", L"Wyświetl szczegóły dotyczące składników sprzętowych i programowych komputera."},
    {1193, L"Adjust the appearance and performance of Windows", L"Modifica l'aspetto e le prestazioni di Windows", L"Ajustar la apariencia y el rendimiento de Windows", L"Modifier l'apparence et les performances de Windows", L"Windows'un görünümünü ve performansını ayarla", L"Настройка внешнего вида и производительности Windows", L"调整 Windows 的外观和性能", L"Darstellung und Leistung von Windows anpassen", L"Ajustar a aparência e o desempenho do Windows", L"Dostosuj wygląd i wydajność systemu Windows"},
    {1194, L"Select settings to change visual effects, processor and memory usage, and virtual memory.", L"Seleziona le impostazioni per modificare gli effetti visivi, l'utilizzo del processore e della memoria e la memoria virtuale.", L"Seleccionar la configuración para cambiar los efectos visuales, el uso del procesador y la memoria, y la memoria virtual.", L"Sélectionner les paramètres pour modifier les effets visuels, l'utilisation du processeur et de la mémoire, et la mémoire virtuelle.", L"Görsel efektleri, işlemci ve bellek kullanımını ve sanal belleği değiştirmek için ayarları seçin.", L"Выберите параметры для изменения визуальных эффектов, использования процессора и памяти и виртуальной памяти.", L"选择设置以更改视觉效果、处理器和内存使用情况以及虚拟内存。", L"Einstellungen auswählen, um visuelle Effekte, Prozessor- und Speichernutzung sowie virtuellen Speicher zu ändern.", L"Selecionar configurações para alterar efeitos visuais, uso do processador e memória e memória virtual.", L"Wybierz ustawienia, aby zmienić efekty wizualne, użycie procesora i pamięci oraz pamięć wirtualną."},
    {1195, L"Open Performance Monitor",                 L"Apri Monitor prestazioni",                          L"Abrir el Monitor de rendimiento",                  L"Ouvrir le Moniteur de performances",            L"Performans İzleyicisini Aç",                      L"Открыть Монитор производительности",             L"打开性能监视器",                                        L"Die Leistungüberwachung öffnen",                  L"Abrir o Monitor de Desempenho",                    L"Otwórz Monitor wydajności"},
    {1196, L"View graphs of system performance and collect data logs.", L"Visualizza grafici delle prestazioni del sistema e raccogli registri dati.", L"Ver gráficos del rendimiento del sistema y recopilar registros de datos.", L"Afficher les graphiques des performances du système et collecter les journaux de données.", L"Sistem performansı grafiklerini görüntüle ve veri günlüklerini topla.", L"Просмотр графиков производительности системы и сбор журналов данных.", L"查看系统性能图表并收集数据日志。", L"Graphiken zur Systemleistung anzeigen und Datenprotokolle sammeln.", L"Exibir gráficos de desempenho do sistema e coletar logs de dados.", L"Wyświetl wykresy wydajności systemu i zbieraj dzienniki danych."},
    {1200, L"Last update: %LastChecked%",              L"Ultimo aggiornamento: %LastChecked%",              L"Última actualización: %LastChecked%",             L"Dernière mise à jour : %LastChecked%",          L"Son güncelleme: %LastChecked%",                   L"Последнее обновление: %LastChecked%",             L"上次更新：%LastChecked%",                                  L"Letzte Aktualisierung: %LastChecked%",              L"Última atualização: %LastChecked%",                 L"Ostatnia aktualizacja: %LastChecked%"},
    {1202, L"Open Task Manager",                        L"Apri Gestione attività",                            L"Abrir el Administrador de tareas",                 L"Ouvrir le Gestionnaire de tâches",              L"Görev Yöneticisini Aç",                           L"Открыть Диспетчер задач",                          L"打开任务管理器",                                        L"Task-Manager öffnen",                               L"Abrir o Gerenciador de Tarefas",                    L"Otwórz Menedżer zadań"},
    {1203, L"Get information about the programs and processes that are currently running on your computer.", L"Ottieni informazioni sui programmi e sui processi attualmente in esecuzione sul computer.", L"Obtener información sobre los programas y procesos que se están ejecutando actualmente en su equipo.", L"Obtenir des informations sur les programmes et les processus en cours d'exécution sur votre ordinateur.", L"Bilgisayarınızda şu anda çalışan programlar ve işlemler hakkında bilgi alın.", L"Получить сведения о программах и процессах, которые сейчас выполняются на компьютере.", L"获取有关当前在计算机上运行的程序和进程的信息。", L"Informationen zu den derzeit auf Ihrem Computer ausgeführten Programmen und Prozessen erhalten.", L"Obter informações sobre os programas e processos em execução no momento no seu computador.", L"Uzyskaj informacje o programach i procesach aktualnie uruchomionych na komputerze."},
    {1207, L"View ways to increase your score",        L"Visualizza i modi per migliorare il punteggio",    L"Ver formas de aumentar su puntuación",             L"Voir les moyens d'améliorer votre score",      L"Puanınızı artırma yollarını görüntüle",           L"Просмотр способов повышения оценки",              L"查看提高分数的方法",                                       L"Möglichkeiten zum Erhöhen Ihrer Bewertung anzeigen", L"Exibir maneiras de aumentar sua pontuação",        L"Wyświetl sposoby zwiększenia wyniku"},
    {1209, L"Processor:",                               L"Processore:",                                       L"Procesador:",                                      L"Processeur :",                                   L"İşlemci:",                                       L"Процессор:",                                       L"处理器：",                                         L"Prozessor:",                                       L"Processador:",                                    L"Procesor:"},
    {1210, L"Calculations per second",                  L"Calcoli al secondo",                                 L"Cálculos por segundo",                             L"Calculs par seconde",                           L"Saniyede hesaplamalar",                           L"Вычислений в секунду",                              L"每秒计算次数",                                        L"Berechnungen pro Sekunde",                           L"Cálculos por segundo",                              L"Obliczenia na sekundę"},
    {1212, L"Memory (RAM):",                           L"Memoria (RAM):",                                   L"Memoria (RAM):",                                   L"Mémoire (RAM) :",                                L"Bellek (RAM):",                                   L"Память (RAM):",                                     L"内存 (RAM)：",                                       L"Arbeitsspeicher (RAM):",                            L"Memória (RAM):",                                   L"Pamięć (RAM):"},
    {1213, L"Memory operations per second",            L"Operazioni di memoria al secondo",                  L"Operaciones de memoria por segundo",               L"Opérations mémoire par seconde",                L"Saniyede bellek işlemleri",                       L"Операций памяти в секунду",                       L"每秒内存操作数",                                        L"Speicheroperationen pro Sekunde",                   L"Operações de memória por segundo",                 L"Operacje pamięci na sekundę"},
    {1215, L"Primary hard disk:",                      L"Disco rigido primario:",                           L"Disco duro principal:",                            L"Disque dur principal :",                        L"Birincil sabit disk:",                             L"Основной жесткий диск:",                           L"主硬盘：",                                           L"Primäre Festplatte:",                               L"Disco rígido principal:",                           L"Podstawowy dysk twardy:"},
    {1216, L"Disk data transfer rate",                  L"Velocità di trasferimento dati del disco",         L"Velocidad de transferencia del disco",            L"Taux de transfert du disque",                     L"Disk veri aktarım hızı",                           L"Скорость передачи данных диска",                   L"磁盘数据传输速率",                                      L"Datentransferrate des Datenträgers",                L"Taxa de transferência do disco",                    L"Szybkość transferu dysku"},
    {1218, L"Graphics:",                                L"Scheda video:",                                    L"Gráficos:",                                        L"Graphiques :",                                   L"Grafik:",                                        L"Графика:",                                          L"图形：",                                            L"Grafik:",                                          L"Elementos gráficos:",                              L"Grafika:"},
    {1221, L"Gaming graphics:",                        L"Grafica dei giochi:",                               L"Gráficos de juego:",                               L"Graphiques de jeu :",                            L"Oyun grafikleri:",                               L"Графика для игр:",                                  L"游戏图形：",                                          L"Gaminggrafik:",                                    L"Gráficos de jogos:",                               L"Grafika w grach:"},
    {1222, L"3D business and gaming graphics performance", L"Prestazioni grafica 3D per applicazioni business e giochi", L"Rendimiento gráfico 3D para juegos y negocios", L"Performances graphiques 3D jeux et pro", L"3B iş ve oyun grafik performansı", L"3D-графика для игр и бизнеса", L"3D 商业和游戏图形性能", L"3D-Grafikleistung für Spiele und Business", L"Desempenho gráfico 3D para jogos e negócios", L"Wydajność grafiki 3D w grach i biznesie"},
    {1223, L"Use these tools to get additional performance information", L"Usa questi strumenti per ottenere ulteriori informazioni sulle prestazioni", L"Use estas herramientas para obtener información adicional sobre el rendimiento", L"Utilisez ces outils pour obtenir des informations supplémentaires sur les performances", L"Ek performans bilgileri almak için bu araçları kullanın", L"Используйте эти средства для получения дополнительных сведений о производительности", L"使用这些工具获取额外的性能信息", L"Verwenden Sie diese Tools, um zusätzliche Leistungsinformationen zu erhalten", L"Use estas ferramentas para obter informações adicionais de desempenho", L"Użyj tych narzędzi, aby uzyskać dodatkowe informacje o wydajności"},
    // Terminator
    {0}
};

// Get an embedded string for a specific language.
static const wchar_t* GetMuiString(UINT id, MuiLanguage lang) {
    for (const MuiStringTable* t = kMuiStrings; t->en != nullptr; ++t) {
        if (t->id != id) {
            continue;
        }

        switch (lang) {
            case MuiLanguage::IT_IT: return t->it;
            case MuiLanguage::ES_ES: return t->es;
            case MuiLanguage::FR_FR: return t->fr;
            case MuiLanguage::TR_TR: return t->tr;
            case MuiLanguage::RU_RU: return t->ru;
            case MuiLanguage::ZH_CN: return t->zh;
            case MuiLanguage::DE_DE: return t->de;
            case MuiLanguage::PT_BR: return t->pt;
            case MuiLanguage::PL_PL: return t->pl;
            default: return t->en;
        }
    }

    return nullptr;
}

static MuiLanguage DetectMuiLanguage(const wchar_t* locale) {
    if (!locale || !*locale) return MuiLanguage::EN_US;

    std::wstring normalized(locale);
    for (auto& c : normalized) c = towlower(c);

    if (normalized.rfind(L"it", 0) == 0) return MuiLanguage::IT_IT;
    if (normalized.rfind(L"es", 0) == 0) return MuiLanguage::ES_ES;
    if (normalized.rfind(L"fr", 0) == 0) return MuiLanguage::FR_FR;
    if (normalized.rfind(L"tr", 0) == 0) return MuiLanguage::TR_TR;
    if (normalized.rfind(L"ru", 0) == 0) return MuiLanguage::RU_RU;
    if (normalized.rfind(L"zh", 0) == 0) return MuiLanguage::ZH_CN;
    if (normalized.rfind(L"de", 0) == 0) return MuiLanguage::DE_DE;
    if (normalized.rfind(L"pt", 0) == 0) return MuiLanguage::PT_BR;
    if (normalized.rfind(L"pl", 0) == 0) return MuiLanguage::PL_PL;
    return MuiLanguage::EN_US;
}

// END OF EMBEDDED STRING CATALOG

static const wchar_t* kAppletClsidEnglish = L"{78f3955e-3b90-4184-bd14-5397c15f1efc}";
static const wchar_t* kAppletDisplayNameEN = L"Performance Information and Tools";
static const wchar_t* kLayoutFolderClsid = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";
static const wchar_t* kProviderClsid = L"{9cb535dd-4354-42a1-8281-bbb58defa741}";
static const DWORD kShellFolderAttributes = 0xa80001a0; // + SFGAO_BROWSABLE (0x08000000): required for the Control Panel to navigate the item in-place on double click
static const DWORD kInitResourceId = 100;
// Windows 8 SP-less RTM x64 (6.2.9200.16384, win8_rtm.120725-1247). Verified URL.
static const wchar_t* kDownloadUrlWin8 = L"https://msdl.microsoft.com/download/symbols/PerfCenterCPL.dll/501093259D000/PerfCenterCPL.dll";
// Windows 7 SP1 x64, 6.1.7601.23403 (win7sp1_ldr.160325-0600).
// GUID 56F58B33A5000 = TimeDateStamp(0x56F58B33) + SizeOfImage(0xA5000), verified
// byte-identical to the official Microsoft symbol server file (SHA-1 1b7d13d3909e19fe5083dd42fa67337cc12e678a).
// KEPT ONLY AS FALLBACK FOR WINDOWS 7 ITSELF: on Windows 8.1/10/11 the Win7 page host
// contract is not compatible anymore ("cannot load page"), so Win8+ systems always use the Win8 DLL.
static const wchar_t* kDownloadUrlWin7 = L"https://msdl.microsoft.com/download/symbols/PerfCenterCPL.dll/56F58B33A5000/PerfCenterCPL.dll";
static const wchar_t* kVariantMarkerName = L"PerfCenterCPL.dll.variant";
static const wchar_t* kLocalizedResourcePrefix = L"PerfCenterCPL.resources-";
// The real files are ~610-660 KB; a 64 KB floor still catches a truncated download
// cheaply while tolerating any legitimate build. (The SHA-256 check below is the
// authoritative integrity test.)
static const DWORD kMinPlausibleDllSize = 65536;

enum class DllVariant { Win8, Win7 };

// Pinned SHA-256 digests of the exact files served from msdl.microsoft.com at
// the URLs above (computed from the symbol-server payloads). A downloaded
// binary is only ever accepted when its SHA-256 matches the digest for the
// requested variant.
static const wchar_t* kExpectedSha256[2] = {
    L"425820ABCA72EFF806C9F41809A619C97ACB642B06B988001EA50090D07D1B98", // DllVariant::Win8
    L"CE15883E0B681DB4CF00FE08A021777568213D6254615E3693DF3C28EB44C4D1", // DllVariant::Win7
};

static const GUID kAppletFolderGuid = {0x78f3955e, 0x3b90, 0x4184,
                                       {0xBD, 0x14, 0x53, 0x97, 0xC1, 0x5F, 0x1E, 0xFC}};
static GUID kProviderGuid = {0x9CB535DD, 0x4354, 0x42A1,
                             {0x82, 0x81, 0xBB, 0xB5, 0x8D, 0xEF, 0xA7, 0x41}};
static const IID IID_IClassFactory_GUID = {0x00000001, 0x0000, 0x0000,
                                           {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

// -----------------------------------------------------------------------------
// Shared state. Most of this is read on hot paths by hooks, so it is published
// atomically. The setup thread populates it in one direction; hooks only read.
// -----------------------------------------------------------------------------
std::atomic<HMODULE> g_hPerfCenter{nullptr};
// The DLL path is written exactly once by the setup thread and never mutated
// afterwards, so an atomic raw pointer to a heap-allocated string is sufficient
// (std::atomic<shared_ptr> is not available on the Windhawk compiler).
std::atomic<const std::wstring*> g_dllPath{nullptr};
std::atomic<bool> g_dllVerifiedOk{false};
std::atomic<int> g_activeVariant{static_cast<int>(DllVariant::Win8)};
std::atomic<bool> g_forceTranslations{true};
std::atomic<bool> g_languageAutomatic{true};
std::atomic<bool> g_useWindows7Skin{false};
std::atomic<bool> g_useWindowsVistaSkin{false};
// Kept only for the initial "Wh_ModInit -> LoadPageSkinSetting" log line.
// These are no longer read by ResolvePageSkinForFreshPage: Wh_GetStringSetting
// (via WindhawkUtils::StringSetting) already returns the configured value from
// Wh_ModInit onwards, so there is no transient/default read to latch against,
// and a single "which skin" value read on each fresh page is simpler and correct.
std::atomic<bool> g_startupRequestedWindows7Skin{false};
std::atomic<bool> g_startupRequestedWindowsVistaSkin{false};
std::atomic<bool> g_perfCenterXmlPageSeen{false};
std::atomic<int> g_forcedLanguage{static_cast<int>(MuiLanguage::EN_US)};

// Manual-reset stop event set by Wh_ModUninit so that setup waits and retry
// delays can be interrupted immediately.
static HANDLE g_stopEvent = nullptr;

// Set by Wh_ModUninit so the background setup (and any in-flight download) can
// abort promptly instead of blocking shutdown.
std::atomic<bool> g_shuttingDown{false};
static const DWORD kDownloadTimeoutMs = 20000;
static const int kMaxDownloadAttempts = 3;
static const DWORD kRetryDelayMs = 3000;

// The private DirectUI resource module is built once and loaded lazily; it is
// guarded by a mutex because both the setup thread and settings changes can
// (re)build it.
static std::mutex g_localizedResourceMutex;
// Cache of the (forceTranslations, languageAutomatic, forcedLanguage) tuple the
// private resource module was last built with, guarded by g_localizedResourceMutex.
// Wh_ModSettingsChanged used to rebuild unconditionally on every settings save,
// which copies and patches a ~650 KB DLL and maps it as a new never-freed
// section each time; skip the rebuild when none of the three inputs changed.
static bool g_lastBuiltValid = false;
static bool g_lastBuiltForceTranslations = false;
static bool g_lastBuiltLanguageAutomatic = true;
static int g_lastBuiltForcedLanguage = static_cast<int>(MuiLanguage::EN_US);
static std::wstring g_localizedResourcePath;
// Read lock-free on the hot path, written under g_localizedResourceMutex.
static std::atomic<HMODULE> g_hLocalizedResources{nullptr};
// Version tag embedded in the private resource module's file name. The module
// content is fully determined by the translation inputs plus the mod's
// embedded string catalog, so bumping this invalidates files built by older
// versions of the mod whenever the catalog changes.
static constexpr DWORD kLocalizedResourceFormatVersion = 1;

// Setup is performed on a background worker thread that is joined on unload.
// Wrapped in std::optional with [[clang::no_destroy]] so that on process shutdown
// (when Wh_ModUninit is not called), ~thread() is not invoked on a joinable thread,
// avoiding std::terminate() and process crashes.
[[clang::no_destroy]] static std::optional<std::thread> g_setupThread;

const std::wstring* CurrentDllPath() {
    return g_dllPath.load(std::memory_order_acquire);
}

std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    for (auto& c : r) c = towlower(c);
    return r;
}
bool EndsWith(const std::wstring& s, const std::wstring& suf) {
    if (s.size() < suf.size()) return false;
    return s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}
std::wstring ExpandEnv(const wchar_t* p) {
    wchar_t b[MAX_PATH]{};
    ExpandEnvironmentStringsW(p, b, MAX_PATH);
    return std::wstring(b);
}

// Cheap, non-allocating ASCII case-insensitive substring test. Used to gate the
// expensive registry hooks so they do no allocation for the vast majority of
// registry accesses that are irrelevant to this mod.
static bool AsciiCaseInsensitiveContains(const wchar_t* s, const char* needle) {
    if (!s || !*s || !needle) return false;
    wchar_t needleW[32] = {};
    size_t nlen = 0;
    for (; needle[nlen] && nlen < 31; ++nlen) {
        char c = needle[nlen];
        if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
        needleW[nlen] = static_cast<wchar_t>(c);
    }
    needleW[nlen] = 0;
    if (!nlen) return false;
    for (const wchar_t* p = s; *p; ++p) {
        const wchar_t* a = p;
        const wchar_t* b = needleW;
        while (*b) {
            wchar_t ca = *a;
            if (ca >= L'a' && ca <= L'z') ca = static_cast<wchar_t>(ca - L'a' + L'A');
            if (ca != *b) break;
            ++a;
            ++b;
        }
        if (!*b) return true;
    }
    return false;
}

static bool ContainsRelevantKeywordCheap(const wchar_t* s) {
    return s && (AsciiCaseInsensitiveContains(s, "clsid") ||
                 AsciiCaseInsensitiveContains(s, "controlpanel") ||
                 AsciiCaseInsensitiveContains(s, "shell extensions"));
}

// -----------------------------------------------------------------------------
// File integrity - SHA-256 via CryptoAPI (advapi32, already linked).
// -----------------------------------------------------------------------------
static wchar_t HexUpper(BYTE nibble) {
    return nibble < 10 ? static_cast<wchar_t>(L'0' + nibble)
                       : static_cast<wchar_t>(L'A' + nibble - 10);
}

static bool ComputeFileSha256(const std::wstring& path, BYTE digest[32]) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;

    HCRYPTPROV prov = 0;
    HCRYPTHASH hash = 0;
    bool ok = false;
    if (CryptAcquireContextW(&prov, nullptr, nullptr, PROV_RSA_AES,
                             CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(prov, CALG_SHA_256, 0, 0, &hash)) {
            BYTE buf[65536];
            DWORD rd = 0;
            bool readOk = true;
            while (ReadFile(h, buf, sizeof(buf), &rd, nullptr) && rd > 0) {
                if (!CryptHashData(hash, buf, rd, 0)) {
                    readOk = false;
                    break;
                }
            }
            if (readOk) {
                DWORD cb = 32;
                ok = CryptGetHashParam(hash, HP_HASHVAL, digest, &cb, 0) != FALSE &&
                     cb == 32;
            }
            CryptDestroyHash(hash);
        }
        CryptReleaseContext(prov, 0);
    }
    CloseHandle(h);
    return ok;
}

static bool FileSha256Matches(const std::wstring& path, const wchar_t* expectedHex) {
    BYTE digest[32];
    if (!ComputeFileSha256(path, digest)) return false;
    for (int i = 0; i < 32; ++i) {
        if (expectedHex[i * 2] != HexUpper(digest[i] >> 4) ||
            expectedHex[i * 2 + 1] != HexUpper(digest[i] & 0xF)) {
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------------
// PE validation helpers
// -----------------------------------------------------------------------------
bool VerifyDownloadedDllLooksValid(const std::wstring& p) {
    HANDLE h = CreateFileW(p.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{};
    GetFileSizeEx(h, &sz);
    if (sz.QuadPart < kMinPlausibleDllSize) {
        CloseHandle(h);
        return false;
    }
    IMAGE_DOS_HEADER dos{};
    DWORD br = 0;
    ReadFile(h, &dos, sizeof(dos), &br, nullptr);
    if (br != sizeof(dos) || dos.e_magic != IMAGE_DOS_SIGNATURE) {
        CloseHandle(h);
        return false;
    }
    LARGE_INTEGER off{};
    off.QuadPart = dos.e_lfanew;
    SetFilePointerEx(h, off, nullptr, FILE_BEGIN);
    DWORD sig = 0;
    ReadFile(h, &sig, sizeof(sig), &br, nullptr);
    CloseHandle(h);
    return br == sizeof(sig) && sig == IMAGE_NT_SIGNATURE;
}

bool GetRealOsVersion(DWORD& major, DWORD& minor, DWORD& build) {
    struct OsVersionInfoW {
        DWORD size, major, minor, build, platform;
        WCHAR csd[128];
    };
    typedef LONG(WINAPI* RtlGetVersionT)(OsVersionInfoW*);
    major = 6;
    minor = 2;
    build = 9200;
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) return false;
    RtlGetVersionT p = reinterpret_cast<RtlGetVersionT>(
        reinterpret_cast<void*>(GetProcAddress(ntdll, "RtlGetVersion")));
    if (!p) return false;
    OsVersionInfoW ovi{};
    ovi.size = sizeof(ovi);
    if (p(&ovi) != 0) return false;
    major = ovi.major;
    minor = ovi.minor;
    build = ovi.build;
    return true;
}

bool IsOsWindows8OrNewer() {
    DWORD M, m, b;
    GetRealOsVersion(M, m, b);
    return (M > 6) || (M == 6 && m >= 2);
}

bool GetPeMachineType(const std::wstring& p, WORD& machine) {
    HANDLE h = CreateFileW(p.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    IMAGE_DOS_HEADER dos{};
    DWORD br = 0;
    bool ok = ReadFile(h, &dos, sizeof(dos), &br, nullptr) && br == sizeof(dos) &&
              dos.e_magic == IMAGE_DOS_SIGNATURE;
    if (ok) {
        LARGE_INTEGER off{};
        off.QuadPart = dos.e_lfanew;
        ok = SetFilePointerEx(h, off, nullptr, FILE_BEGIN) != FALSE;
        DWORD sig = 0;
        ok = ok && ReadFile(h, &sig, sizeof(sig), &br, nullptr) &&
             br == sizeof(sig) && sig == IMAGE_NT_SIGNATURE;
        ok = ok && ReadFile(h, &machine, sizeof(machine), &br, nullptr) &&
             br == sizeof(machine);
    }
    CloseHandle(h);
    return ok;
}

std::wstring VariantMarkerName(DllVariant v) {
    return v == DllVariant::Win7 ? L"win7" : L"win8";
}

bool ReadVariantMarker(const std::wstring& dir, std::wstring& out) {
    std::wstring mp = dir + L"\\" + kVariantMarkerName;
    HANDLE h = CreateFileW(mp.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    char buf[16] = {};
    DWORD br = 0;
    ReadFile(h, buf, sizeof(buf) - 1, &br, nullptr);
    CloseHandle(h);
    if (br == 0) return false;
    out.clear();
    for (DWORD i = 0; i < br; i++) out.push_back(static_cast<wchar_t>(
                                                     static_cast<unsigned char>(buf[i])));
    while (!out.empty() && (out.back() == L'\r' || out.back() == L'\n' ||
                            out.back() == L' '))
        out.pop_back();
    return !out.empty();
}

void WriteVariantMarker(const std::wstring& dir, DllVariant v) {
    std::wstring mp = dir + L"\\" + kVariantMarkerName;
    std::wstring name = VariantMarkerName(v);
    std::string narrow(name.begin(), name.end());
    HANDLE h = CreateFileW(mp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD bw = 0;
    WriteFile(h, narrow.c_str(), static_cast<DWORD>(narrow.size()), &bw, nullptr);
    CloseHandle(h);
}

DllVariant ResolveSelectedVariant() {
    if (IsOsWindows8OrNewer()) return DllVariant::Win8;
    Wh_Log(L"Windows 7 detected: the Windows 8 DLL requires NT 6.2+, using the "
           L"Windows 7 DLL as fallback");
    return DllVariant::Win7;
}

// The mod and the downloaded PerfCenterCPL.dll are x64 only, which is why the
// metadata declares `@architecture x86-64`. On an ARM64 device Windhawk can
// still inject the mod into the native ARM64 explorer.exe (a predefined shell
// process), where the x64 binary runs under emulation. Wh_ModInit() calls
// IsRunningAsAmd64() first and returns FALSE before installing any of the
// process-wide hooks, so an ARM64 shell never pays the registry/COM/LoadString
// hook cost for a mod that can never do anything there. RunSetup keeps the
// same guard as a second line of defense.
static bool IsRunningAsAmd64() {
#if defined(_M_ARM64) || defined(__aarch64__)
    return false;
#else
    // IsWow64Process2 reports the machine of the current process (even for
    // 64-bit processes) and the native machine of the system.
    // Note: When running natively without WOW64 translation, processMachine is
    // set to IMAGE_FILE_MACHINE_UNKNOWN (0), and the process architecture is
    // equal to nativeMachine.
    typedef BOOL(WINAPI* IsWow64Process2T)(HANDLE, USHORT*, USHORT*);
    USHORT processMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    USHORT nativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
    if (k32) {
        auto pIsWow64Process2 = reinterpret_cast<IsWow64Process2T>(
            reinterpret_cast<void*>(GetProcAddress(k32, "IsWow64Process2")));
        if (pIsWow64Process2 &&
            pIsWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine)) {
            const USHORT actualMachine =
                (processMachine == IMAGE_FILE_MACHINE_UNKNOWN) ? nativeMachine
                                                               : processMachine;
            return actualMachine == IMAGE_FILE_MACHINE_AMD64;
        }
    }
    // Fallback for older Windows where IsWow64Process2 is not available:
    SYSTEM_INFO si{};
    GetNativeSystemInfo(&si);
    return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
#endif
}

// Why this mod downloads PerfCenterCPL.dll instead of asking the user to
// supply it:
//
// The original "Informazioni e strumenti prestazioni" applet was removed by
// Microsoft starting with Windows 8.1, so no genuine copy of PerfCenterCPL.dll
// ships with the versions of Windows this mod targets - there is no local file
// to point the mod at. Asking users to hunt down and paste in a binary
// themselves is fundamental to why this mod stays easy to use, but it is also
// exactly the kind of step that quietly breaks a mod and gets blamed on the
// author: users would inevitably supply files of mismatched bitness (x86
// instead of x64), the wrong Windows release, a version already patched or
// bundled with unrelated tools, or a corrupted/incomplete download - none of
// which VerifyDllIsCompatible() can fully tell apart from "the mod itself
// doesn't work". A wrong file silently produces broken/crashing behavior that
// looks exactly like a bug in the mod, generates support requests and bad
// reviews the author has no way to reproduce or diagnose, and defeats the
// entire point of a one-click Windhawk mod.
//
// Downloading a specific, known-good build directly (see GetVariantUrl below)
// keeps the whole install path deterministic: the same bytes, verified with
// VerifyDllIsCompatible()/VerifyDownloadedDllLooksValid() before ever being
// registered, on every machine. It never touches or replaces a system file
// (see the registry-virtualization comments above), only a private,
// mod-owned copy is created, so this is not different in kind from a mod that
// draws its own resources - it just fetches them from the network instead of
// embedding megabytes of data in the mod script.
std::wstring GetVariantUrl(DllVariant v) {
    return v == DllVariant::Win7 ? kDownloadUrlWin7 : kDownloadUrlWin8;
}

// Structural + SHA-256 verification. Anything that is not the exact expected
// binary is rejected and never loaded.
bool VerifyDllIsCompatible(const std::wstring& p, DllVariant variant) {
    if (!VerifyDownloadedDllLooksValid(p)) return false;
    WORD machine = 0;
    if (!GetPeMachineType(p, machine) || machine != IMAGE_FILE_MACHINE_AMD64) {
        Wh_Log(L"DLL at %s is not x64 (machine=%04X) - incompatible with this "
               L"x64 mod",
               p.c_str(), machine);
        return false;
    }
    if (!FileSha256Matches(p, kExpectedSha256[static_cast<int>(variant)])) {
        Wh_Log(L"DLL at %s failed SHA-256 verification; refusing to use it",
               p.c_str());
        return false;
    }
    return true;
}

// Delete the mod's own stale "*.old-*" files (renamed-out previous variants).
void CleanupOldDlls(const std::wstring& dir) {
    WIN32_FIND_DATAW fd{};
    std::wstring pattern = dir + L"\\" + kDllRelativeName + L".old*";
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

// -----------------------------------------------------------------------------
// Verified load of the downloaded DLL
// -----------------------------------------------------------------------------
class UniqueWinHandle {
public:
    UniqueWinHandle() = default;
    explicit UniqueWinHandle(HANDLE handle) : handle_(handle) {}
    ~UniqueWinHandle() { Reset(); }

    UniqueWinHandle(const UniqueWinHandle&) = delete;
    UniqueWinHandle& operator=(const UniqueWinHandle&) = delete;

    UniqueWinHandle(UniqueWinHandle&& other) noexcept
        : handle_(other.Release()) {}
    UniqueWinHandle& operator=(UniqueWinHandle&& other) noexcept {
        if (this != &other) Reset(other.Release());
        return *this;
    }

    bool IsValid() const { return handle_ && handle_ != INVALID_HANDLE_VALUE; }
    HANDLE Get() const { return handle_; }
    HANDLE Release() {
        HANDLE result = handle_;
        handle_ = INVALID_HANDLE_VALUE;
        return result;
    }
    void Reset(HANDLE handle = INVALID_HANDLE_VALUE) {
        if (IsValid()) CloseHandle(handle_);
        handle_ = handle;
    }

private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};

// Open the DLL denying write sharing for the whole verify -> load window.
// FILE_SHARE_READ | FILE_SHARE_DELETE is required (not FILE_SHARE_READ only)
// because the loader opens the file requesting delete sharing; with read-only
// sharing LoadLibraryExW would fail with ERROR_SHARING_VIOLATION. What the pin
// prevents is in-place modification: the bytes cannot be rewritten between
// verification and load. The caller must keep the returned handle alive until
// after LoadLibraryExW returns.
static UniqueWinHandle PinDllForLoad(const std::wstring& path) {
    return UniqueWinHandle(
        CreateFileW(path.c_str(), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, nullptr));
}

// Handles for the WinInet call currently in flight (if any), published under
// g_downloadHandlesMutex so Wh_ModUninit can close them from the outside.
// InternetOpenUrlW/InternetQueryDataAvailable/InternetReadFile each block for
// up to kDownloadTimeoutMs and do not otherwise notice g_shuttingDown, so on a
// captive-portal/dead network the only way to make them return immediately is
// to close the handle out from under them - WinInet fails the blocked call
// with ERROR_INTERNET_OPERATION_CANCELLED instead of waiting out the timeout.
static std::mutex g_downloadHandlesMutex;
static HINTERNET g_downloadHNet = nullptr;
static HINTERNET g_downloadHUrl = nullptr;

static void CancelInFlightDownload() {
    std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
    if (g_downloadHUrl) {
        InternetCloseHandle(g_downloadHUrl);
        g_downloadHUrl = nullptr;
    }
    if (g_downloadHNet) {
        InternetCloseHandle(g_downloadHNet);
        g_downloadHNet = nullptr;
    }
}

// Download a file to `dest` using WinInet. Unlike URLDownloadToFileW this gives
// us real control over timeouts and lets us abort on shutdown. connect/send/
// receive are each bounded to kDownloadTimeoutMs at the WinInet level, and we
// additionally enforce an overall deadline so a stuck/captive-portal connection
// can never block for minutes. Returns true only when the full file was written.
static bool DownloadWithTimeout(const std::wstring& url, const std::wstring& dest) {
    HINTERNET hNet = InternetOpenW(L"Windhawk Performance Info Tools",
                                   INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr,
                                   0);
    if (!hNet) return false;

    {
        std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
        g_downloadHNet = hNet;
    }

    // If shutdown was requested while we were setting hNet up, cancel right
    // away instead of starting a request we'd only have to unwind.
    if (g_shuttingDown.load(std::memory_order_relaxed)) {
        CancelInFlightDownload();
        return false;
    }

    DWORD timeoutMs = kDownloadTimeoutMs;
    InternetSetOptionW(hNet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs,
                       sizeof(timeoutMs));
    InternetSetOptionW(hNet, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs,
                       sizeof(timeoutMs));
    InternetSetOptionW(hNet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs,
                       sizeof(timeoutMs));

    HINTERNET hUrl = InternetOpenUrlW(
        hNet, url.c_str(), nullptr, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI,
        0);
    if (!hUrl) {
        CancelInFlightDownload();
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
        // CancelInFlightDownload() may have already closed g_downloadHNet from
        // under us (shutdown raced this open); if so, hUrl is bogus, back out.
        if (!g_downloadHNet) {
            InternetCloseHandle(hUrl);
            return false;
        }
        g_downloadHUrl = hUrl;
    }

    bool ok = false;
    do {
        // Redirects are followed automatically (INTERNET_FLAG_NO_AUTO_REDIRECT
        // is not set). Confirm we actually got a 200 OK response.
        DWORD status = 0;
        DWORD statusLen = sizeof(status);
        DWORD headerIndex = 0;
        if (!HttpQueryInfoW(hUrl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                            &status, &statusLen, &headerIndex) ||
            status != HTTP_STATUS_OK) {
            break;
        }

        HANDLE hFile = CreateFileW(dest.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                                   nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                   nullptr);
        if (hFile == INVALID_HANDLE_VALUE) break;

        const ULONGLONG start = GetTickCount64();
        BYTE buf[65536];
        bool readOk = true;
        DWORD rd = 0;
        for (;;) {
            if (g_shuttingDown.load(std::memory_order_relaxed)) {
                readOk = false;
                break;
            }
            DWORD avail = 0;
            if (!InternetQueryDataAvailable(hUrl, &avail, 0, 0)) {
                readOk = false;
                break;
            }
            if (avail == 0) break;  // normal EOF
            if (avail > sizeof(buf)) avail = static_cast<DWORD>(sizeof(buf));
            if (!InternetReadFile(hUrl, buf, avail, &rd) || rd == 0) {
                readOk = false;
                break;
            }
            DWORD wr = 0;
            if (!WriteFile(hFile, buf, rd, &wr, nullptr) || wr != rd) {
                readOk = false;
                break;
            }
            if (GetTickCount64() - start > kDownloadTimeoutMs) {
                readOk = false;  // overall deadline exceeded
                break;
            }
        }
        CloseHandle(hFile);
        ok = readOk;
    } while (false);

    // Normal completion path. Only close a handle here if a concurrent
    // CancelInFlightDownload() hasn't already closed it out from under us
    // (checked-and-cleared under the same lock, to avoid a double-close race
    // with the shutdown path).
    {
        std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
        if (g_downloadHUrl == hUrl) {
            g_downloadHUrl = nullptr;
        } else {
            hUrl = nullptr;  // already closed by CancelInFlightDownload
        }
        if (g_downloadHNet == hNet) {
            g_downloadHNet = nullptr;
        } else {
            hNet = nullptr;  // already closed by CancelInFlightDownload
        }
    }
    if (hUrl) InternetCloseHandle(hUrl);
    if (hNet) InternetCloseHandle(hNet);
    return ok;
}

// Download the required DLL (if needed), verify it, and make it visible only
// once valid. Returns true and sets outPath when a valid DLL is available.
// The cross-process serialization is done by the caller (the setup thread).
// Download the required DLL (if needed), verify it, and make it visible only
// once valid. On success, `outPin` holds an open handle to the verified file
// (opened denying write sharing) and `outPath` its path; the caller must keep
// `outPin` alive until after LoadLibraryExW so the executed bytes are the
// verified bytes. The cross-process serialization is done by the caller (the
// setup thread).
bool DownloadDllToPath(const std::wstring& dir, DllVariant variant,
                       std::wstring& outPath, UniqueWinHandle& outPin) {
    const std::wstring wanted = VariantMarkerName(variant);
    const std::wstring out = dir + L"\\" + kDllRelativeName;
    CleanupOldDlls(dir);

    // Reuse an existing, valid DLL of the requested variant.
    if (GetFileAttributesW(out.c_str()) != INVALID_FILE_ATTRIBUTES) {
        std::wstring marker;
        bool markerOk = ReadVariantMarker(dir, marker);
        if (markerOk && marker == wanted) {
            UniqueWinHandle pin = PinDllForLoad(out);
            if (pin.IsValid() && VerifyDllIsCompatible(out, variant)) {
                outPath = out;
                outPin = std::move(pin);
                return true;
            }
        }
        // Marker mismatch or corrupt file: replace it.
        DeleteFileW(out.c_str());
        // If the file is locked (still loaded), renaming a loaded module is
        // allowed, so move it out of the way to free the name immediately.
        FILETIME ft{};
        SYSTEMTIME st{};
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);
        wchar_t suffix[48];
        swprintf_s(suffix, 48, L".old-%08X%08X", (unsigned)ft.dwHighDateTime,
                   (unsigned)ft.dwLowDateTime);
        std::wstring oldPath = out + suffix;
        MoveFileW(out.c_str(), oldPath.c_str());
        if (GetFileAttributesW(out.c_str()) != INVALID_FILE_ATTRIBUTES) {
            // Could not remove or rename the old file. Keep it only if it is
            // actually a valid DLL of the requested variant.
            UniqueWinHandle pin = PinDllForLoad(out);
            if (pin.IsValid() && VerifyDllIsCompatible(out, variant)) {
                outPath = out;
                outPin = std::move(pin);
                return true;
            }
            return false;
        }
    }

    // Download to a temp file first, verify, then atomically move it into
    // place so a partial file is never visible under the final name.
    const std::wstring url = GetVariantUrl(variant);
    const std::wstring tmp = out + L".tmp";
    DeleteFileW(tmp.c_str());
    Wh_Log(L"Downloading %s variant from %s", wanted.c_str(), url.c_str());
    const bool downloaded = DownloadWithTimeout(url, tmp);
    bool ok = false;
    UniqueWinHandle pin;
    if (downloaded) {
        pin = PinDllForLoad(tmp);
        ok = pin.IsValid() && VerifyDllIsCompatible(tmp, variant);
    }
    if (ok) {
        // The pin handle stays open across the move: it was opened on the
        // temp file's file object, which is the same object that now appears
        // at `out`, so the verified bytes are the bytes the caller will load.
        if (MoveFileExW(tmp.c_str(), out.c_str(),
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            WriteVariantMarker(dir, variant);
            outPath = out;
            outPin = std::move(pin);
            return true;
        }
        ok = false;
    }
    if (!ok) {
        Wh_Log(L"Download/verification failed; the DLL was not installed");
        DeleteFileW(tmp.c_str());
    }
    return false;
}

// Remove only files this mod created. keepBase=true leaves the base DLL and the
// variant marker behind (used when the "keep files" setting is ON), but still
// removes per-process resource modules and stale copies. If a file is still in
// use it is left alone and retried on the next unload.
void RemoveOwnFiles(const std::wstring& dir, bool keepBase) {
    WIN32_FIND_DATAW fd{};
    std::wstring pattern = dir + L"\\*";
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    std::vector<std::wstring> folders;
    do {
        std::wstring name = fd.cFileName;
        if (name == L"." || name == L"..") continue;
        const bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        const std::wstring p = dir + L"\\" + name;
        if (isDir) {
            folders.push_back(p);
            continue;
        }
        const bool ownFile =
            name == kDllRelativeName || name == kVariantMarkerName ||
            name.rfind(kLocalizedResourcePrefix, 0) == 0 ||
            (name.rfind(kDllRelativeName, 0) == 0 &&
             name.find(L".old") != std::wstring::npos) ||
            (name.size() >= 4 &&
             name.compare(name.size() - 4, 4, L".tmp") == 0);
        if (!ownFile) continue;
        if (keepBase && (name == kDllRelativeName || name == kVariantMarkerName))
            continue;
        if (!DeleteFileW(p.c_str())) {
            // No MOVEFILE_DELAY_UNTIL_REBOOT here: a machine-wide persistent
            // operation is not appropriate. Just leave the file; it will be
            // retried on a later unload.
            Wh_Log(L"Could not delete %s (err=%u, in use); will retry on next "
                   L"unload",
                   p.c_str(), GetLastError());
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    for (const auto& f : folders) RemoveDirectoryW(f.c_str());
    RemoveDirectoryW(dir.c_str());
}

// -----------------------------------------------------------------------------
// Conservative DirectUI resource module
// -----------------------------------------------------------------------------
class ScopedTemporaryFile {
public:
    explicit ScopedTemporaryFile(std::wstring path) : path_(std::move(path)) {}
    ~ScopedTemporaryFile() {
        if (!committed_ && !path_.empty()) DeleteFileW(path_.c_str());
    }
    ScopedTemporaryFile(const ScopedTemporaryFile&) = delete;
    ScopedTemporaryFile& operator=(const ScopedTemporaryFile&) = delete;
    void Commit() { committed_ = true; }

private:
    std::wstring path_;
    bool committed_ = false;
};

class ResourceUpdateTransaction {
public:
    explicit ResourceUpdateTransaction(const std::wstring& path)
        : update_(BeginUpdateResourceW(path.c_str(), FALSE)) {}
    ~ResourceUpdateTransaction() {
        if (update_) EndUpdateResourceW(update_, TRUE);
    }
    ResourceUpdateTransaction(const ResourceUpdateTransaction&) = delete;
    ResourceUpdateTransaction& operator=(const ResourceUpdateTransaction&) = delete;

    bool IsValid() const { return update_ != nullptr; }
    HANDLE Get() const { return update_; }
    bool Commit() {
        if (!update_) return false;
        HANDLE update = update_;
        update_ = nullptr;
        return EndUpdateResourceW(update, FALSE) != FALSE;
    }

private:
    HANDLE update_ = nullptr;
};

template <typename T>
static bool ReadPeValue(const std::vector<BYTE>& file, size_t offset, T& value) {
    if (offset > file.size() || file.size() - offset < sizeof(T)) return false;
    memcpy(&value, file.data() + offset, sizeof(T));
    return true;
}

// UpdateResource intentionally restricts LN/MUI binaries. Rename the private
// copy's named "MUI" RC-config resource to the unused name "CUI" first. This
// changes only the copy and makes it a normal resource PE. Note: this depends on
// undocumented layout details of the specific Microsoft binary at the symbol
// server URL; it may need updating if that binary ever changes. It is confined
// to a private copy, so the blast radius is small.
static bool DisableMuiConfigInPrivateCopy(const std::wstring& path) {
    UniqueWinHandle file(CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid()) return false;

    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file.Get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > 64 * 1024 * 1024) {
        return false;
    }

    std::vector<BYTE> bytes(static_cast<size_t>(size.QuadPart));
    DWORD bytesRead = 0;
    if (!ReadFile(file.Get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &bytesRead, nullptr) ||
        bytesRead != bytes.size()) {
        return false;
    }

    IMAGE_DOS_HEADER dos = {};
    if (!ReadPeValue(bytes, 0, dos) || dos.e_magic != IMAGE_DOS_SIGNATURE ||
        dos.e_lfanew < 0) {
        return false;
    }

    const size_t ntOffset = static_cast<size_t>(dos.e_lfanew);
    DWORD signature = 0;
    IMAGE_FILE_HEADER fileHeader = {};
    if (!ReadPeValue(bytes, ntOffset, signature) ||
        signature != IMAGE_NT_SIGNATURE ||
        !ReadPeValue(bytes, ntOffset + sizeof(DWORD), fileHeader)) {
        return false;
    }

    const size_t optionalOffset = ntOffset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER);
    WORD optionalMagic = 0;
    if (!ReadPeValue(bytes, optionalOffset, optionalMagic)) return false;

    DWORD resourceRva = 0;
    if (optionalMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        IMAGE_OPTIONAL_HEADER64 optional = {};
        if (!ReadPeValue(bytes, optionalOffset, optional) ||
            optional.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE) {
            return false;
        }
        resourceRva =
            optional.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    } else if (optionalMagic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        IMAGE_OPTIONAL_HEADER32 optional = {};
        if (!ReadPeValue(bytes, optionalOffset, optional) ||
            optional.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE) {
            return false;
        }
        resourceRva =
            optional.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    } else {
        return false;
    }
    if (!resourceRva) return false;

    const size_t sectionOffset = optionalOffset + fileHeader.SizeOfOptionalHeader;
    DWORD resourceRaw = 0;
    for (WORD i = 0; i < fileHeader.NumberOfSections; ++i) {
        IMAGE_SECTION_HEADER section = {};
        if (!ReadPeValue(bytes, sectionOffset + i * sizeof(IMAGE_SECTION_HEADER),
                         section)) {
            return false;
        }
        const DWORD virtualSize = section.Misc.VirtualSize;
        const DWORD span = virtualSize > section.SizeOfRawData ? virtualSize
                                                               : section.SizeOfRawData;
        if (resourceRva >= section.VirtualAddress &&
            resourceRva - section.VirtualAddress < span) {
            resourceRaw = section.PointerToRawData +
                          (resourceRva - section.VirtualAddress);
            break;
        }
    }
    if (!resourceRaw || resourceRaw >= bytes.size()) return false;

    IMAGE_RESOURCE_DIRECTORY root = {};
    if (!ReadPeValue(bytes, resourceRaw, root)) return false;
    const DWORD entryCount = static_cast<DWORD>(root.NumberOfNamedEntries) +
                             root.NumberOfIdEntries;
    const size_t entriesOffset = resourceRaw + sizeof(root);

    size_t muiFirstCharacterOffset = 0;
    for (DWORD i = 0; i < entryCount; ++i) {
        IMAGE_RESOURCE_DIRECTORY_ENTRY entry = {};
        if (!ReadPeValue(bytes, entriesOffset + i * sizeof(entry), entry)) {
            return false;
        }
        DWORD nameField = 0;
        memcpy(&nameField, &entry, sizeof(nameField));
        if (!(nameField & 0x80000000u)) continue;
        const size_t stringOffset = resourceRaw + (nameField & 0x7FFFFFFFu);
        WORD length = 0;
        if (!ReadPeValue(bytes, stringOffset, length) || length != 3) continue;
        WCHAR name[3] = {};
        if (stringOffset + sizeof(WORD) > bytes.size() ||
            bytes.size() - (stringOffset + sizeof(WORD)) < sizeof(name)) {
            return false;
        }
        memcpy(name, bytes.data() + stringOffset + sizeof(WORD), sizeof(name));
        if (name[0] == L'M' && name[1] == L'U' && name[2] == L'I') {
            muiFirstCharacterOffset = stringOffset + sizeof(WORD);
            break;
        }
    }
    if (!muiFirstCharacterOffset) return false;

    LARGE_INTEGER position = {};
    position.QuadPart = static_cast<LONGLONG>(muiFirstCharacterOffset);
    if (!SetFilePointerEx(file.Get(), position, nullptr, FILE_BEGIN)) return false;
    const WCHAR replacement = L'C';
    DWORD written = 0;
    return WriteFile(file.Get(), &replacement, sizeof(replacement), &written,
                     nullptr) &&
           written == sizeof(replacement);
}

static WORD GetEmbeddedLanguageId(MuiLanguage language) {
    switch (language) {
        case MuiLanguage::IT_IT: return 0x0410;
        case MuiLanguage::ES_ES: return 0x0C0A;
        case MuiLanguage::FR_FR: return 0x040C;
        case MuiLanguage::TR_TR: return 0x041F;
        case MuiLanguage::RU_RU: return 0x0419;
        case MuiLanguage::ZH_CN: return 0x0804;
        case MuiLanguage::DE_DE: return 0x0407;
        case MuiLanguage::PT_BR: return 0x0416;
        case MuiLanguage::PL_PL: return 0x0415;
        default: return 0x0409;
    }
}

static bool BuildEmbeddedStringBlock(UINT blockId, MuiLanguage language,
                                     std::vector<BYTE>& output) {
    output.clear();
    bool hasString = false;
    for (UINT index = 0; index < 16; ++index) {
        const UINT id = (blockId - 1) * 16 + index;
        const wchar_t* text = GetMuiString(id, language);
        const size_t length = text ? wcslen(text) : 0;
        if (length > 0xFFFF) return false;
        if (text) hasString = true;

        const WORD wordLength = static_cast<WORD>(length);
        const BYTE* lengthBytes = reinterpret_cast<const BYTE*>(&wordLength);
        output.insert(output.end(), lengthBytes, lengthBytes + sizeof(wordLength));
        if (length) {
            const BYTE* textBytes = reinterpret_cast<const BYTE*>(text);
            output.insert(output.end(), textBytes,
                          textBytes + length * sizeof(wchar_t));
        }
    }
    return hasString;
}

// Reads the translation settings at call time, so both the setup thread and a
// settings change can rebuild the module with the current values.
// The private resource module's content is fully determined by the translation
// inputs (forceTranslations / languageAutomatic / forcedLanguage) plus this
// mod's embedded string catalog - not by the process or the current UI
// language (the automatic build embeds all ten languages, and a forced build
// embeds the forced language under every language ID). The file name is
// therefore derived from those inputs (plus a format version), so a given
// configuration maps to exactly one reusable file that is shared across
// processes and explorer restarts, instead of accumulating a new ~650 KB file
// per process load. Only the module for the configuration that is currently
// in use is ever loaded, so this stays compatible with the conservative
// never-unload approach (files still mapped are simply left alone).
static std::wstring LocalizedResourceModuleFileName() {
    const bool automatic = g_languageAutomatic.load();
    const MuiLanguage forced = static_cast<MuiLanguage>(g_forcedLanguage.load());
    wchar_t name[96] = {};
    if (automatic) {
        swprintf_s(name, ARRAYSIZE(name), L"%sv%lu-auto.dll",
                   kLocalizedResourcePrefix, kLocalizedResourceFormatVersion);
    } else {
        const wchar_t* tag = L"en";
        switch (forced) {
            case MuiLanguage::IT_IT: tag = L"it"; break;
            case MuiLanguage::ES_ES: tag = L"es"; break;
            case MuiLanguage::FR_FR: tag = L"fr"; break;
            case MuiLanguage::TR_TR: tag = L"tr"; break;
            case MuiLanguage::RU_RU: tag = L"ru"; break;
            case MuiLanguage::ZH_CN: tag = L"zh"; break;
            case MuiLanguage::DE_DE: tag = L"de"; break;
            case MuiLanguage::PT_BR: tag = L"pt"; break;
            case MuiLanguage::PL_PL: tag = L"pl"; break;
            default: tag = L"en"; break;
        }
        swprintf_s(name, ARRAYSIZE(name), L"%sv%lu-%s.dll",
                   kLocalizedResourcePrefix, kLocalizedResourceFormatVersion,
                   tag);
    }
    return name;
}

// Remove resource-module files that are no longer needed: anything that does
// not match the current configuration's file name. This also cleans up the
// legacy per-process "PerfCenterCPL.resources-<pid>-<seq>.dll" files created
// by older versions of the mod. Files still mapped (by this process or
// another one) fail to delete and are simply left for a later sweep/unload.
// Intentionally called only at setup time (before this process has loaded any
// resource module), never while modules may be referenced.
static void SweepStaleResourceModules(const std::wstring& directory) {
    if (directory.empty()) return;
    const std::wstring currentName = LocalizedResourceModuleFileName();
    WIN32_FIND_DATAW fd{};
    const std::wstring pattern =
        directory + L"\\" + kLocalizedResourcePrefix + L"*";
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const std::wstring name = fd.cFileName;
        if (name == currentName) continue;
        const std::wstring p = directory + L"\\" + name;
        if (!DeleteFileW(p.c_str())) {
            // Still mapped (this or another process) or transiently locked;
            // retried on a later sweep/unload.
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static bool BuildLocalizedResourceModule(const std::wstring& sourceDll,
                                         const std::wstring& directory) {
    g_localizedResourcePath.clear();
    if (!g_forceTranslations.load() ||
        static_cast<DllVariant>(g_activeVariant.load()) != DllVariant::Win8 ||
        sourceDll.empty()) {
        return false;
    }

    const std::wstring destination =
        directory + L"\\" + LocalizedResourceModuleFileName();

    // Reuse an existing valid module for this exact configuration: the bytes
    // are deterministic, so a file built earlier (by this process, a previous
    // session, or control.exe) is byte-identical to a fresh build. Skipping
    // the rebuild avoids the ~650 KB copy + PE patch + resource rewrite on
    // every process load. If the file is valid it has not been modified in
    // place (it was built by this mod from a pinned-verified source), so
    // loading it is safe.
    if (GetFileAttributesW(destination.c_str()) != INVALID_FILE_ATTRIBUTES &&
        VerifyDownloadedDllLooksValid(destination)) {
        g_localizedResourcePath = destination;
        g_lastBuiltForceTranslations = g_forceTranslations.load();
        g_lastBuiltLanguageAutomatic = g_languageAutomatic.load();
        g_lastBuiltForcedLanguage = g_forcedLanguage.load();
        g_lastBuiltValid = true;
        Wh_Log(L"Reusing existing private resource module: %s",
               destination.c_str());
        return true;
    }
    // Invalid leftover from an interrupted build: remove it and rebuild.
    DeleteFileW(destination.c_str());

    // Unique per-process temp file: the destination name is now shared across
    // processes, so a bare ".tmp" suffix would collide with a concurrent build
    // in control.exe or another explorer.exe.
    wchar_t tmpSuffix[32] = {};
    swprintf_s(tmpSuffix, ARRAYSIZE(tmpSuffix), L".tmp-%lu",
               GetCurrentProcessId());
    const std::wstring temporary = destination + tmpSuffix;
    ScopedTemporaryFile temporaryGuard(temporary);

    DeleteFileW(temporary.c_str());
    if (!CopyFileW(sourceDll.c_str(), temporary.c_str(), FALSE)) {
        Wh_Log(L"Copying private resource module failed: %u", GetLastError());
        return false;
    }

    if (!DisableMuiConfigInPrivateCopy(temporary)) {
        Wh_Log(L"Could not neutralize RC Config in private copy");
        return false;
    }

    ResourceUpdateTransaction update(temporary);
    if (!update.IsValid()) {
        Wh_Log(L"BeginUpdateResource failed: %u", GetLastError());
        return false;
    }

    static const MuiLanguage languages[] = {
        MuiLanguage::EN_US, MuiLanguage::IT_IT, MuiLanguage::ES_ES,
        MuiLanguage::FR_FR, MuiLanguage::TR_TR, MuiLanguage::RU_RU,
        MuiLanguage::ZH_CN, MuiLanguage::DE_DE, MuiLanguage::PT_BR,
        MuiLanguage::PL_PL,
    };

    UINT maxStringId = 0;
    for (const MuiStringTable* row = kMuiStrings; row->en; ++row) {
        if (row->id > maxStringId) maxStringId = row->id;
    }
    const UINT maxBlockId = maxStringId / 16 + 1;

    std::vector<BYTE> block;
    for (MuiLanguage resourceLanguage : languages) {
        const MuiLanguage textLanguage =
            g_languageAutomatic.load() ? resourceLanguage
                                       : static_cast<MuiLanguage>(g_forcedLanguage.load());
        for (UINT blockId = 1; blockId <= maxBlockId; ++blockId) {
            if (!BuildEmbeddedStringBlock(blockId, textLanguage, block)) continue;
            if (!UpdateResourceW(update.Get(), RT_STRING, MAKEINTRESOURCEW(blockId),
                                 GetEmbeddedLanguageId(resourceLanguage),
                                 block.data(), static_cast<DWORD>(block.size()))) {
                Wh_Log(L"UpdateResource failed: lang=%04X block=%u err=%u",
                       GetEmbeddedLanguageId(resourceLanguage), blockId,
                       GetLastError());
                return false;
            }
        }
    }

    if (!update.Commit()) {
        Wh_Log(L"EndUpdateResource failed: %u", GetLastError());
        return false;
    }

    DeleteFileW(destination.c_str());
    if (!MoveFileExW(temporary.c_str(), destination.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        // Another process may have just built (or mapped) the same
        // deterministic file; if the destination is already valid, use it.
        if (VerifyDownloadedDllLooksValid(destination)) {
            temporaryGuard.Commit();
            g_localizedResourcePath = destination;
            g_lastBuiltForceTranslations = g_forceTranslations.load();
            g_lastBuiltLanguageAutomatic = g_languageAutomatic.load();
            g_lastBuiltForcedLanguage = g_forcedLanguage.load();
            g_lastBuiltValid = true;
            Wh_Log(L"Private resource module already present (built by another "
                   L"process): %s",
                   destination.c_str());
            return true;
        }
        Wh_Log(L"Activating private resource module failed: %u", GetLastError());
        return false;
    }
    temporaryGuard.Commit();

    if (!VerifyDownloadedDllLooksValid(destination)) {
        DeleteFileW(destination.c_str());
        Wh_Log(L"Private resource module failed PE validation");
        return false;
    }

    g_localizedResourcePath = destination;
    g_lastBuiltForceTranslations = g_forceTranslations.load();
    g_lastBuiltLanguageAutomatic = g_languageAutomatic.load();
    g_lastBuiltForcedLanguage = g_forcedLanguage.load();
    g_lastBuiltValid = true;
    Wh_Log(L"Private resource module ready with 147 strings in 10 languages: %s",
           destination.c_str());
    return true;
}

// True when the cached build already matches the current translation inputs,
// so callers can skip ReleaseLocalizedResourceModuleLocked()+BuildLocalizedResourceModule()
// entirely. Assumes g_localizedResourceMutex is already held.
static bool LocalizedResourceModuleUpToDateLocked() {
    return g_lastBuiltValid &&
           !g_localizedResourcePath.empty() &&
           g_lastBuiltForceTranslations == g_forceTranslations.load() &&
           g_lastBuiltLanguageAutomatic == g_languageAutomatic.load() &&
           (g_lastBuiltLanguageAutomatic ||
            g_lastBuiltForcedLanguage == g_forcedLanguage.load());
}

static HMODULE EnsureLocalizedResourceModuleLoaded() {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    if (HMODULE h = g_hLocalizedResources.load()) return h;
    if (g_localizedResourcePath.empty()) return nullptr;

    HMODULE h = LoadLibraryExW(g_localizedResourcePath.c_str(), nullptr,
                               LOAD_LIBRARY_AS_DATAFILE |
                                   LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    g_hLocalizedResources.store(h);
    if (!h) {
        Wh_Log(L"Loading private resource module failed: %u", GetLastError());
    }
    return h;
}

// Assumes g_localizedResourceMutex is already held.
// Note: We deliberately never call FreeLibrary or DeleteFileW here (nor on
// teardown), because DirectUI::XResourceProvider::Create caches this HINSTANCE
// and loads resources from it lazily. Unloading or deleting the file while a
// page is open can crash explorer.exe. Dropping the handle and clearing the path
// only means the next page that opens will use a freshly built module; pages
// already open keep using the old (still-mapped) module, and RemoveOwnFiles
// cleans up whichever copies are no longer mapped on a later unload.
static void ReleaseLocalizedResourceModuleLocked() {
    g_hLocalizedResources.store(nullptr);
    g_localizedResourcePath.clear();
}

static void ReleaseLocalizedResourceModule() {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    ReleaseLocalizedResourceModuleLocked();
}

// The storage directory is scoped to this mod only. Windhawk resolves it for
// both installed and portable deployments, so do not construct a ProgramData path.
static std::wstring StoreDir() {
    // Windows extended-length paths may contain up to 32,767 characters.
    wchar_t path[32768] = {};
    const size_t pathLength = Wh_GetModStoragePath(path, ARRAYSIZE(path));
    if (pathLength == 0 || pathLength >= ARRAYSIZE(path)) {
        Wh_Log(L"Could not retrieve the mod storage path");
        return {};
    }
    return path;
}

// -----------------------------------------------------------------------------
// Async setup - runs on a worker thread so explorer.exe startup is never blocked.
// -----------------------------------------------------------------------------
static void RunSetup() {
    Wh_Log(L"PerfCenterCPL background setup started...");
    // x64 only: bail out cleanly if we ended up in a non-AMD64 process (see
    // IsRunningAsAmd64). This avoids any download/load attempt on ARM64.
    if (!IsRunningAsAmd64()) {
        Wh_Log(L"PerfCenterCPL setup aborted: system architecture is not AMD64 (x64)");
        g_dllVerifiedOk.store(false, std::memory_order_release);
        return;
    }

    // Serialize with other processes (other explorer.exe instances, control.exe)
    // that may be performing the same setup at the same time.
    HANDLE setupMutex =
        CreateMutexW(nullptr, FALSE, L"Windhawk.PerformanceInfoToolsRestorer.Setup");
    if (setupMutex) {
        HANDLE handles[] = {setupMutex, g_stopEvent};
        DWORD numHandles = g_stopEvent ? 2 : 1;
        DWORD wait = WaitForMultipleObjects(numHandles, handles, FALSE, 60000);
        if (wait == WAIT_OBJECT_0 + 1) {
            CloseHandle(setupMutex);
            return;
        }
        if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) {
            // Could not get the lock in time. Proceed anyway; the
            // temp-file + atomic-move pattern keeps a partial file invisible.
        }
    }

    DllVariant variant = ResolveSelectedVariant();
    std::wstring dir = StoreDir();
    if (dir.empty()) {
        g_dllVerifiedOk.store(false, std::memory_order_release);
        Wh_Log(L"Performance Information and Tools is unavailable: Windhawk did not provide a mod storage path");
        if (setupMutex) {
            ReleaseMutex(setupMutex);
            CloseHandle(setupMutex);
        }
        return;
    }

    // Remove private resource modules left behind by other configurations,
    // sessions or processes (and by older per-process naming). Runs only at
    // startup, before this process has loaded any resource module, so nothing
    // referenced by this process can be deleted. The current configuration's
    // file - if any - is kept, and files still mapped elsewhere are skipped
    // and retried on a later sweep/unload.
    SweepStaleResourceModules(dir);

    std::wstring outPath;
    UniqueWinHandle pinnedDll;
    bool ok = false;

    // A few bounded retries with backoff cover transient network failures.
    // Each attempt is individually time-limited and aborts on shutdown, so this
    // thread never blocks indefinitely. A persisted valid file is reused without
    // any network access (see DownloadDllToPath), so an offline restart with an
    // existing copy still works.
    for (int attempt = 1; attempt <= kMaxDownloadAttempts; ++attempt) {
        if (g_shuttingDown.load(std::memory_order_relaxed)) break;
        if (DownloadDllToPath(dir, variant, outPath, pinnedDll)) {
            ok = true;
            break;
        }
        if (g_shuttingDown.load(std::memory_order_relaxed)) break;
        if (attempt < kMaxDownloadAttempts) {
            Wh_Log(L"Download attempt %d/%d failed; retrying in a few seconds",
                   attempt, kMaxDownloadAttempts);
            if (g_stopEvent &&
                WaitForSingleObject(g_stopEvent, kRetryDelayMs) == WAIT_OBJECT_0) {
                break;
            } else if (!g_stopEvent) {
                Sleep(kRetryDelayMs);
            }
        }
    }

    if (setupMutex) {
        ReleaseMutex(setupMutex);
        CloseHandle(setupMutex);
    }

    if (ok && !outPath.empty()) {
        auto* pathPtr = new std::wstring(outPath);
        g_dllPath.store(pathPtr, std::memory_order_release);
        g_activeVariant.store(static_cast<int>(variant), std::memory_order_release);
        // Take exactly one reference on the DLL; it is released in Wh_ModUninit.
        // `pinnedDll` is still held here (write sharing denied), so the bytes
        // mapped as executable code are the bytes that passed verification.
        HMODULE h = LoadLibraryExW(outPath.c_str(), nullptr, 0);
        g_hPerfCenter.store(h, std::memory_order_release);
        {
            std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
            BuildLocalizedResourceModule(outPath, dir);
        }
        g_dllVerifiedOk.store(true, std::memory_order_release);
        Wh_Log(L"PerfCenterCPL setup complete (variant %s)",
               VariantMarkerName(variant).c_str());
    } else {
        g_dllVerifiedOk.store(false, std::memory_order_release);
        Wh_Log(L"Performance Information and Tools is unavailable: the required "
               L"PerfCenterCPL.dll could not be downloaded or verified. Check "
               L"your internet connection and restart explorer.exe to retry.");
    }
}

// -----------------------------------------------------------------------------
// Language selection
// -----------------------------------------------------------------------------
static MuiLanguage GetCurrentEmbeddedLanguage() {
    if (!g_languageAutomatic.load())
        return static_cast<MuiLanguage>(g_forcedLanguage.load());
    LANGID languageId = GetThreadUILanguage();
    if (!languageId) languageId = GetUserDefaultUILanguage();

    switch (PRIMARYLANGID(languageId)) {
        case LANG_ITALIAN: return MuiLanguage::IT_IT;
        case LANG_SPANISH: return MuiLanguage::ES_ES;
        case LANG_FRENCH: return MuiLanguage::FR_FR;
        case LANG_TURKISH: return MuiLanguage::TR_TR;
        case LANG_RUSSIAN: return MuiLanguage::RU_RU;
        case LANG_GERMAN: return MuiLanguage::DE_DE;
        case LANG_POLISH: return MuiLanguage::PL_PL;
        // Only Simplified Chinese (zh-CN) is embedded. Traditional Chinese
        // (zh-TW/zh-HK/zh-MO) falls back to English rather than showing the
        // wrong (Simplified) variant.
        case LANG_CHINESE:
            return (SUBLANGID(languageId) == SUBLANG_CHINESE_SIMPLIFIED ||
                    SUBLANGID(languageId) == SUBLANG_CHINESE_SINGAPORE)
                       ? MuiLanguage::ZH_CN
                       : MuiLanguage::EN_US;
        // Only Brazilian Portuguese is embedded; European Portuguese falls back
        // to English rather than showing the wrong variant.
        case LANG_PORTUGUESE:
            return SUBLANGID(languageId) == SUBLANG_PORTUGUESE_BRAZILIAN
                       ? MuiLanguage::PT_BR
                       : MuiLanguage::EN_US;
        default: break;
    }

    // Locale fallback for unusual/custom UI-language configurations.
    wchar_t locale[LOCALE_NAME_MAX_LENGTH] = {};
    if (GetUserDefaultLocaleName(locale, ARRAYSIZE(locale))) {
        return DetectMuiLanguage(locale);
    }
    return MuiLanguage::EN_US;
}


static void LoadLanguageSetting() {
    g_languageAutomatic.store(true);
    g_forcedLanguage.store(static_cast<int>(MuiLanguage::EN_US));

    // WindhawkUtils::StringSetting frees the underlying pointer automatically.
    auto raw = WindhawkUtils::StringSetting::make(L"language");
    PCWSTR r = raw.get();
    std::wstring value = (r && *r) ? r : L"auto";
    for (auto& character : value) character = towlower(character);

    if (value.empty() || value == L"auto" || value == L"system") return;
    g_languageAutomatic.store(false);
    if (value == L"it" || value == L"it-it")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::IT_IT));
    else if (value == L"es" || value == L"es-es")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::ES_ES));
    else if (value == L"fr" || value == L"fr-fr")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::FR_FR));
    else if (value == L"tr" || value == L"tr-tr")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::TR_TR));
    else if (value == L"ru" || value == L"ru-ru")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::RU_RU));
    else if (value == L"zh" || value == L"zh-cn")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::ZH_CN));
    else if (value == L"de" || value == L"de-de")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::DE_DE));
    else if (value == L"pt" || value == L"pt-br")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::PT_BR));
    else if (value == L"pl" || value == L"pl-pl")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::PL_PL));
    else if (value == L"en" || value == L"en-us")
        g_forcedLanguage.store(static_cast<int>(MuiLanguage::EN_US));
    else {
        g_languageAutomatic.store(true);
        Wh_Log(L"Unknown language setting '%s'; using Automatic", value.c_str());
    }
}


static std::wstring ReadPageSkinSettingValue() {
    auto raw = WindhawkUtils::StringSetting::make(L"pageSkin");
    PCWSTR r = raw.get();
    std::wstring value = (r && *r) ? r : L"win8";
    for (auto& character : value) character = towlower(character);
    return value;
}

static bool ReadPageSkinVistaSetting() {
    std::wstring value = ReadPageSkinSettingValue();
    return value.find(L"vista") != std::wstring::npos ||
           value.find(L"windows vista") != std::wstring::npos;
}

static bool ReadPageSkinSetting() {
    std::wstring value = ReadPageSkinSettingValue();

    // Be intentionally permissive. Windhawk normally stores the option id
    // ("win7"/"win8"/"vista"), but during local testing/imports it can be useful
    // to tolerate labels such as "Windows 7 classic links" too.
    const bool mentionsVista = value.find(L"vista") != std::wstring::npos;
    const bool mentions8 = value.find(L"8") != std::wstring::npos ||
                           value.find(L"win8") != std::wstring::npos ||
                           value.find(L"windows 8") != std::wstring::npos ||
                           value.find(L"windows8") != std::wstring::npos;
    const bool mentions7 = value.find(L"7") != std::wstring::npos ||
                           value.find(L"win7") != std::wstring::npos ||
                           value.find(L"windows 7") != std::wstring::npos ||
                           value.find(L"windows7") != std::wstring::npos ||
                           value.find(L"classic") != std::wstring::npos;
    return mentions7 && !mentions8 && !mentionsVista;
}

static const wchar_t* CurrentPageSkinName() {
    if (g_useWindowsVistaSkin.load()) return L"Windows Vista";
    if (g_useWindows7Skin.load()) return L"Windows 7";
    return L"Windows 8";
}

static void LoadPageSkinSetting() {
    const bool useVista = ReadPageSkinVistaSetting();
    const bool useWin7 = !useVista && ReadPageSkinSetting();
    g_useWindowsVistaSkin.store(useVista);
    g_useWindows7Skin.store(useWin7);
    g_startupRequestedWindowsVistaSkin.store(useVista);
    g_startupRequestedWindows7Skin.store(useWin7);
    Wh_Log(L"Initial page skin setting: %s", CurrentPageSkinName());
}

static void ResolvePageSkinForFreshPage() {
    // Read the setting and use it verbatim. Earlier versions OR'd the fresh
    // read with latched g_useWindowsVistaSkin/g_startupRequestedWindowsVistaSkin
    // globals, which meant a skin could only ever be turned on through this
    // path, never off: Wh_ModSettingsChanged deliberately skips re-resolving
    // while the page is open ("close and reopen to apply"), so switching e.g.
    // Vista -> Windows 8 while the page is open, then closing and reopening
    // it, left freshVista false but the latched flag still true, and the
    // Vista skin stuck until the mod was reloaded. Wh_GetStringSetting (via
    // WindhawkUtils::StringSetting) returns the configured value from
    // Wh_ModInit onwards, so there is no "transient/default read on the very
    // first parse" to guard against, and no latch is needed.
    const bool useVista = ReadPageSkinVistaSetting();
    g_useWindowsVistaSkin.store(useVista);
    g_useWindows7Skin.store(!useVista && ReadPageSkinSetting());
}


static bool ContainsInsensitive(const std::wstring& text,
                                const std::wstring& needle) {
    if (needle.empty()) return true;
    std::wstring a = text;
    std::wstring b = needle;
    for (auto& c : a) c = towlower(c);
    for (auto& c : b) c = towlower(c);
    return a.find(b) != std::wstring::npos;
}

static BOOL CALLBACK FindPerfCenterWindowProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;

    wchar_t title[256] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    if (!title[0]) return TRUE;

    std::wstring windowTitle = title;
    // Compare against every language in the mod's own MUI catalog instead of a
    // hand-maintained list of titles: the previous list only covered English,
    // Italian, Spanish, French, German, Portuguese and Polish, so on Turkish,
    // Russian or Simplified Chinese UI (missing from that list) this always
    // returned false and Wh_ModSettingsChanged would swap the skin under an
    // open page, which can leave a half-patched DirectUI tree.
    bool isPerfCenterTitle = false;
    for (int i = 0; i <= static_cast<int>(MuiLanguage::PL_PL); ++i) {
        const wchar_t* candidate = GetMuiString(1, static_cast<MuiLanguage>(i));
        if (candidate && ContainsInsensitive(windowTitle, candidate)) {
            isPerfCenterTitle = true;
            break;
        }
    }

    if (isPerfCenterTitle) {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;
    }
    return TRUE;
}

static bool IsPerfCenterControlPanelWindowOpen() {
    bool found = false;
    EnumWindows(FindPerfCenterWindowProc, reinterpret_cast<LPARAM>(&found));
    return found;
}


static const wchar_t* GetEmbeddedTranslation(UINT id) {
    return GetMuiString(id, GetCurrentEmbeddedLanguage());
}

std::wstring GetLocalizedDisplayName() {
    const wchar_t* text = GetMuiString(1, GetCurrentEmbeddedLanguage());
    return text ? std::wstring(text) : std::wstring(kAppletDisplayNameEN);
}

static std::wstring GetLocalizedInfoTip() {
    const wchar_t* text = GetEmbeddedTranslation(2);
    return text
               ? std::wstring(text)
               : std::wstring(
                     L"Get information about your computer's speed and performance. "
                     L"If solutions to performance problems are available, Windows "
                     L"lets you know.");
}

// -----------------------------------------------------------------------------
// Registry virtualization
// -----------------------------------------------------------------------------
std::wstring g_clsidLower, g_clsidSuffix, g_defaultIconSuffix, g_inprocSuffix,
    g_shellFolderSuffix, g_instanceSuffix, g_initPropBagSuffix, g_namespaceSuffix;
std::wstring g_providerClsidLower, g_providerSuffix, g_providerInprocSuffix,
    g_namespaceHkcuPath;

void InitClsidStrings() {
    g_clsidLower = kAppletClsidEnglish;
    g_clsidSuffix = L"clsid\\" + g_clsidLower;
    g_defaultIconSuffix = g_clsidSuffix + L"\\defaulticon";
    g_inprocSuffix = g_clsidSuffix + L"\\inprocserver32";
    g_shellFolderSuffix = g_clsidSuffix + L"\\shellfolder";
    g_instanceSuffix = g_clsidSuffix + L"\\instance";
    g_initPropBagSuffix = g_instanceSuffix + L"\\initpropertybag";
    g_namespaceSuffix = L"controlpanel\\namespace\\" + g_clsidLower;
    g_providerClsidLower = kProviderClsid;
    g_providerSuffix = L"clsid\\" + g_providerClsidLower;
    g_providerInprocSuffix = g_providerSuffix + L"\\inprocserver32";
    g_namespaceHkcuPath =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\" +
        g_clsidLower;
}

// KeyTracker maps HKEY handles to registry paths and tracks the fake handles the
// virtualization layer hands out. Reads use a shared lock (so concurrent
// registry reads are not serialized against each other), writes use an
// exclusive lock.
class KeyTracker {
public:
    std::wstring GetPath(HKEY k) const {
        if (const wchar_t* s = SpecialRootPath(k)) return s;
        std::shared_lock<std::shared_mutex> l(mutex_);
        auto it = paths_.find(k);
        return it != paths_.end() ? it->second : std::wstring();
    }
    bool IsFakeAndGetPath(HKEY k, std::wstring& o) const {
        if (const wchar_t* s = SpecialRootPath(k)) {
            o = s;
            return false;
        }
        std::shared_lock<std::shared_mutex> l(mutex_);
        bool f = fakeOwners_.count(k) != 0;
        auto it = paths_.find(k);
        o = it != paths_.end() ? it->second : std::wstring();
        return f;
    }
    bool IsFake(HKEY k) const {
        std::shared_lock<std::shared_mutex> l(mutex_);
        return fakeOwners_.count(k) != 0;
    }
    // True if hk is either a fake (virtualized) key, or a real key whose path
    // is already tracked (i.e. its path contained one of our keywords) - the
    // two cases where a subkey open could still land inside the virtualized
    // tree even if the subkey name itself doesn't contain a keyword (e.g.
    // opening "CLSID" is tracked because "clsid" matches; opening the GUID
    // subkey underneath it doesn't repeat that keyword but still needs the
    // combined-path check). No string copy - just a presence check.
    bool IsTrackedOrFake(HKEY k) const {
        if (SpecialRootPath(k)) return false;
        std::shared_lock<std::shared_mutex> l(mutex_);
        return fakeOwners_.count(k) != 0 || paths_.count(k) != 0;
    }
    void Track(HKEY k, const std::wstring& p) {
        if (!k || IsSpecialRoot(k)) return;
        if (!ContainsRelevantKeywordCheap(p.c_str())) return;
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_[k] = p;
    }
    void Untrack(HKEY k) {
        if (!k || IsSpecialRoot(k)) return;
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_.erase(k);
    }
    HKEY CreateFake(const std::wstring& p) {
        std::unique_ptr<int> o(new (std::nothrow) int(1));
        if (!o) return nullptr;
        HKEY f = reinterpret_cast<HKEY>(o.get());
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_[f] = p;
        fakeOwners_[f] = std::move(o);
        return f;
    }
    void FreeFake(HKEY k) {
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_.erase(k);
        fakeOwners_.erase(k);
    }
    // On unload we deliberately abandon (leak) the backing int of each
    // outstanding fake key. This keeps the addresses reserved so they can never
    // be handed back by a subsequent allocation and mistaken for a valid OS
    // handle. This is intentional, not a leak bug.
    void ClearWithoutFreeing() {
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_.clear();
        for (auto& kv : fakeOwners_) {
            [[maybe_unused]] int* abandoned = kv.second.release();
        }
        fakeOwners_.clear();
    }

private:
    static bool IsSpecialRoot(HKEY k) {
        auto v = reinterpret_cast<uintptr_t>(k);
        return v >= 0x80000000 && v <= 0x80000004;
    }
    // Returns an interned literal (no heap allocation) instead of a fresh
    // std::wstring. Every registry hook in the process (RegOpenKeyExW,
    // RegQueryValueExW, RegGetValueW, ...) calls this on every invocation via
    // GetPath()/IsFakeAndGetPath(), and names like L"HKEY_LOCAL_MACHINE" are
    // past the small-string-optimization limit, so returning by value used to
    // cost a heap allocation on every single registry read/open in explorer.exe.
    static const wchar_t* SpecialRootPath(HKEY k) {
        switch (reinterpret_cast<uintptr_t>(k)) {
            case 0x80000000: return L"HKEY_CLASSES_ROOT";
            case 0x80000001: return L"HKEY_CURRENT_USER";
            case 0x80000002: return L"HKEY_LOCAL_MACHINE";
            case 0x80000003: return L"HKEY_USERS";
            case 0x80000004: return L"HKEY_CURRENT_CONFIG";
            default: return nullptr;
        }
    }
    mutable std::shared_mutex mutex_;
    std::unordered_map<HKEY, std::wstring> paths_;
    std::unordered_map<HKEY, std::unique_ptr<int>> fakeOwners_;
};

static KeyTracker g_keyTracker;
static std::mutex g_injectedMutex;
static std::unordered_map<HKEY, bool> g_injectedForHandle;

// Inject the namespace entry once per enumeration pass. Resetting when a new
// pass starts (idx==0) means a caller that enumerates twice on the same handle
// (e.g. once to size buffers) still sees the entry on each pass.
static bool ShouldInjectNow(HKEY k, DWORD idx) {
    std::lock_guard<std::mutex> l(g_injectedMutex);
    if (idx == 0) g_injectedForHandle[k] = false;
    bool& a = g_injectedForHandle[k];
    if (a) return false;
    a = true;
    return true;
}
void ClearInjectedState(HKEY k) {
    std::lock_guard<std::mutex> l(g_injectedMutex);
    g_injectedForHandle.erase(k);
}

enum class VNode {
    None, ClsidRoot, DefaultIcon, InProcServer32, ShellFolder, Instance,
    InitPropertyBag, NamespaceEntry, ProviderRoot, ProviderInProc
};

VNode ClassifyPath(const std::wstring& p) {
    if (!ContainsRelevantKeywordCheap(p.c_str())) return VNode::None;
    std::wstring l = ToLower(p);
    if (EndsWith(l, g_namespaceSuffix)) return VNode::NamespaceEntry;
    if (EndsWith(l, g_initPropBagSuffix)) return VNode::InitPropertyBag;
    if (EndsWith(l, g_instanceSuffix)) return VNode::Instance;
    if (EndsWith(l, g_shellFolderSuffix)) return VNode::ShellFolder;
    if (EndsWith(l, g_inprocSuffix)) return VNode::InProcServer32;
    if (EndsWith(l, g_defaultIconSuffix)) return VNode::DefaultIcon;
    if (EndsWith(l, g_clsidSuffix)) return VNode::ClsidRoot;
    if (EndsWith(l, g_providerInprocSuffix)) return VNode::ProviderInProc;
    if (EndsWith(l, g_providerSuffix)) return VNode::ProviderRoot;
    return VNode::None;
}

bool IsApprovedKey(const std::wstring& p) {
    return EndsWith(ToLower(p), L"shell extensions\\approved");
}
bool IsTargetKey(const std::wstring& p) { return ClassifyPath(p) != VNode::None; }
bool IsNamespaceParentKey(const std::wstring& p) {
    return EndsWith(ToLower(p), L"controlpanel\\namespace");
}

LSTATUS ProvideStringValue(LPBYTE d, LPDWORD cb, const std::wstring& s) {
    if (!cb) return ERROR_INVALID_PARAMETER;
    DWORD need = static_cast<DWORD>((s.length() + 1) * sizeof(wchar_t));
    if (!d) {
        *cb = need;
        return ERROR_SUCCESS;
    }
    if (*cb < need) {
        *cb = need;
        return ERROR_MORE_DATA;
    }
    *cb = need;
    memcpy(d, s.c_str(), need);
    return ERROR_SUCCESS;
}

LSTATUS ProvideDwordValue(LPBYTE d, LPDWORD cb, DWORD v) {
    if (!cb) return ERROR_INVALID_PARAMETER;
    if (!d) {
        *cb = sizeof(DWORD);
        return ERROR_SUCCESS;
    }
    if (*cb < sizeof(DWORD)) {
        *cb = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    *cb = sizeof(DWORD);
    *reinterpret_cast<DWORD*>(d) = v;
    return ERROR_SUCCESS;
}

std::wstring GetShdocvwPath() {
    wchar_t b[MAX_PATH]{};
    GetSystemDirectoryW(b, MAX_PATH);
    return std::wstring(b) + L"\\shdocvw.dll";
}

// Compute the virtual value for (path, valueName). Shared by the W and A value
// query hooks and the value enumerators so they always agree.
static bool TryProvideValueData(const std::wstring& path, const std::wstring& vn,
                                DWORD* type, std::wstring& strOut,
                                DWORD& dwordOut, bool& isStr, LSTATUS& status) {
    const std::wstring* dllPath = CurrentDllPath();
    if (!g_dllVerifiedOk.load() || !dllPath || dllPath->empty()) return false;

    if (IsApprovedKey(path)) {
        std::wstring low = ToLower(vn);
        if (low == g_clsidLower || low == g_providerClsidLower) {
            if (type) *type = REG_SZ;
            strOut.clear();
            isStr = true;
            status = ERROR_SUCCESS;
            return true;
        }
        return false;
    }

    VNode node = ClassifyPath(path);
    if (node == VNode::None) return false;
    switch (node) {
        case VNode::NamespaceEntry:
            if (vn.empty()) {
                if (type) *type = REG_SZ;
                strOut = GetLocalizedDisplayName();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::ClsidRoot:
            if (vn.empty()) {
                if (type) *type = REG_SZ;
                strOut = GetLocalizedDisplayName();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"LocalizedString") {
                if (type) *type = REG_EXPAND_SZ;
                strOut = L"@" + *dllPath + L",-1";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"InfoTip") {
                if (type) *type = REG_SZ;
                strOut = GetLocalizedInfoTip();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"{305CA226-D286-468e-B848-2B2E8E697B74} 2") {
                if (type) *type = REG_EXPAND_SZ;
                strOut = L"5";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::InProcServer32:
            if (vn.empty()) {
                if (type) *type = REG_EXPAND_SZ;
                strOut = GetShdocvwPath();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"ThreadingModel") {
                if (type) *type = REG_SZ;
                strOut = L"Apartment";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::ShellFolder:
            if (vn == L"Attributes") {
                if (type) *type = REG_DWORD;
                dwordOut = kShellFolderAttributes;
                isStr = false;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"WantsParseDisplayName") {
                if (type) *type = REG_SZ;
                strOut.clear();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::Instance:
            if (vn == L"CLSID") {
                if (type) *type = REG_SZ;
                strOut = kLayoutFolderClsid;
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::InitPropertyBag:
            if (vn == L"ResourceDLL") {
                if (type) *type = REG_EXPAND_SZ;
                strOut = *dllPath;
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"ResourceID") {
                if (type) *type = REG_DWORD;
                dwordOut = kInitResourceId;
                isStr = false;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::DefaultIcon:
            if (vn.empty()) {
                if (type) *type = REG_EXPAND_SZ;
                strOut = *dllPath + L",-1";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::ProviderRoot:
            if (vn.empty()) {
                if (type) *type = REG_SZ;
                strOut.clear();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        case VNode::ProviderInProc:
            if (vn.empty()) {
                if (type) *type = REG_EXPAND_SZ;
                strOut = *dllPath;
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"ThreadingModel") {
                if (type) *type = REG_SZ;
                strOut = L"Apartment";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;
        default: break;
    }
    return false;
}

static bool TryProvideValue(const std::wstring& path, const std::wstring& vn,
                            LPDWORD tp, LPBYTE d, LPDWORD cb, LSTATUS& out) {
    DWORD vtype = 0;
    std::wstring strOut;
    DWORD dwOut = 0;
    bool isStr = true;
    if (!TryProvideValueData(path, vn, &vtype, strOut, dwOut, isStr, out))
        return false;
    if (tp) *tp = vtype;
    if (isStr) {
        out = ProvideStringValue(d, cb, strOut);
        return true;
    }
    out = ProvideDwordValue(d, cb, dwOut);
    return true;
}

bool GetVirtualSubKeyName(VNode n, DWORD idx, std::wstring& o) {
    switch (n) {
        case VNode::ClsidRoot:
            if (idx == 0) { o = L"DefaultIcon"; return true; }
            if (idx == 1) { o = L"InProcServer32"; return true; }
            if (idx == 2) { o = L"ShellFolder"; return true; }
            if (idx == 3) { o = L"Instance"; return true; }
            return false;
        case VNode::Instance:
            if (idx == 0) { o = L"InitPropertyBag"; return true; }
            return false;
        case VNode::ProviderRoot:
            if (idx == 0) { o = L"InProcServer32"; return true; }
            return false;
        default: return false;
    }
}

DWORD GetVirtualSubKeyCount(VNode n) {
    switch (n) {
        case VNode::ClsidRoot: return 4;
        case VNode::Instance: return 1;
        case VNode::ProviderRoot: return 1;
        default: return 0;
    }
}

// -----------------------------------------------------------------------------
// Conservative Registry Virtualization (Unicode *W only)
// -----------------------------------------------------------------------------
using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegOpenKeyW_t = decltype(&RegOpenKeyW);
using RegCreateKeyExW_t = decltype(&RegCreateKeyExW);
using RegCloseKey_t = decltype(&RegCloseKey);
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
using RegGetValueW_t = decltype(&RegGetValueW);
using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
using RegEnumKeyW_t = decltype(&RegEnumKeyW);
using RegQueryInfoKeyW_t = decltype(&RegQueryInfoKeyW);

RegOpenKeyExW_t RegOpenKeyExWOriginal = nullptr;
RegOpenKeyW_t RegOpenKeyWOriginal = nullptr;
RegCreateKeyExW_t RegCreateKeyExWOriginal = nullptr;
RegCloseKey_t RegCloseKeyOriginal = nullptr;
RegQueryValueExW_t RegQueryValueExWOriginal = nullptr;
RegGetValueW_t RegGetValueWOriginal = nullptr;
RegEnumKeyExW_t RegEnumKeyExWOriginal = nullptr;
RegEnumKeyW_t RegEnumKeyWOriginal = nullptr;
RegQueryInfoKeyW_t RegQueryInfoKeyWOriginal = nullptr;

static bool IsWriteAccess(REGSAM sam) {
    return (sam & (KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_CREATE_LINK)) != 0;
}

// Shared "open" logic used by the W open hooks (and the create hooks, to
// refuse persisting writes to the virtualized tree). Never returns a fake
// handle to a caller that asked for write/create access, which keeps synthetic
// handles out of write paths. Preserves caller's opt parameter (e.g. REG_OPTION_OPEN_LINK).
static LSTATUS RegOpenKeyVirtual(HKEY hk, const std::wstring& sub, bool hasSub,
                                 DWORD opt, REGSAM sam, PHKEY out) {
    std::wstring full;
    const bool fakeParent = g_keyTracker.IsFakeAndGetPath(hk, full);
    if (fakeParent) {
        if (hasSub) {
            if (!full.empty()) full += L"\\";
            full += sub;
        }
        if (IsTargetKey(full)) {
            if (IsWriteAccess(sam)) return ERROR_ACCESS_DENIED;
            HKEY f = g_keyTracker.CreateFake(full);
            if (!f) return ERROR_OUTOFMEMORY;
            if (out) *out = f;
            return ERROR_SUCCESS;
        }
        return ERROR_FILE_NOT_FOUND;
    }

    LSTATUS st = RegOpenKeyExWOriginal(hk, hasSub ? sub.c_str() : nullptr, opt, sam, out);
    if (st == ERROR_SUCCESS && out && *out) {
        std::wstring fp = full;
        if (hasSub) {
            if (!fp.empty()) fp += L"\\";
            fp += sub;
        }
        g_keyTracker.Track(*out, fp);
    } else if (st == ERROR_FILE_NOT_FOUND && out) {
        std::wstring fp = full;
        if (hasSub) {
            if (!fp.empty()) fp += L"\\";
            fp += sub;
        }
        if (IsTargetKey(fp)) {
            if (IsWriteAccess(sam)) return ERROR_ACCESS_DENIED;
            HKEY f = g_keyTracker.CreateFake(fp);
            if (!f) return ERROR_OUTOFMEMORY;
            if (out) *out = f;
            return ERROR_SUCCESS;
        }
    }
    return st;
}

// Fast pre-check shared by both open hooks: these run on every RegOpenKeyExW/
// RegOpenKeyW in the process, so avoid constructing/copying std::wstring at
// all for the overwhelming majority of calls that can't possibly touch the
// virtualized tree - i.e. hk isn't one of our fake keys and sub doesn't even
// contain one of our cheap keywords. g_keyTracker.IsFake() is a shared-lock
// map lookup with no allocation, matching what RegOpenKeyVirtual would have
// concluded anyway.
static bool MightNeedVirtualization(HKEY hk, LPCWSTR sub) {
    return g_keyTracker.IsTrackedOrFake(hk) || ContainsRelevantKeywordCheap(sub);
}

LSTATUS WINAPI RegOpenKeyExWHook(HKEY hk, LPCWSTR sub, DWORD opt, REGSAM sam,
                                 PHKEY out) {
    if (!MightNeedVirtualization(hk, sub)) {
        return RegOpenKeyExWOriginal(hk, sub, opt, sam, out);
    }
    std::wstring s = sub ? sub : L"";
    return RegOpenKeyVirtual(hk, s, sub && *sub, opt, sam, out);
}

LSTATUS WINAPI RegOpenKeyWHook(HKEY hk, LPCWSTR sub, PHKEY out) {
    // RegOpenKey uses MAXIMUM_ALLOWED as default access mask in modern Windows.
    if (!MightNeedVirtualization(hk, sub)) {
        return RegOpenKeyWOriginal(hk, sub, out);
    }
    std::wstring s = sub ? sub : L"";
    return RegOpenKeyVirtual(hk, s, sub && *sub, 0, MAXIMUM_ALLOWED, out);
}

// RegCreateKeyEx: creating/opening the virtualized tree for write would persist
// to the real registry, which the mod must never do. Refuse writes there.
template <typename CreateFn>
static LSTATUS CreateKeyVirtual(HKEY hk, const std::wstring& sub, bool hasSub,
                                REGSAM sam, PHKEY out, LPDWORD disposition,
                                CreateFn original) {
    std::wstring full;
    const bool fakeParent = g_keyTracker.IsFakeAndGetPath(hk, full);
    if (fakeParent) {
        if (hasSub) {
            if (!full.empty()) full += L"\\";
            full += sub;
        }
        if (IsTargetKey(full)) {
            if (out) *out = nullptr;
            return ERROR_ACCESS_DENIED;
        }
        return ERROR_FILE_NOT_FOUND;
    }
    if (hasSub) {
        if (!full.empty()) full += L"\\";
        full += sub;
    }
    if (IsTargetKey(full)) {
        if (out) *out = nullptr;
        return ERROR_ACCESS_DENIED;
    }
    return original();
}

LSTATUS WINAPI RegCreateKeyExWHook(HKEY hk, LPCWSTR sub, DWORD reserved,
                                   LPWSTR cls, DWORD opt, REGSAM sam,
                                   LPSECURITY_ATTRIBUTES sa, PHKEY out,
                                   LPDWORD disposition) {
    std::wstring s = sub ? sub : L"";
    return CreateKeyVirtual(
        hk, s, sub && *sub, sam, out, disposition,
        [&]() {
            return RegCreateKeyExWOriginal(hk, sub, reserved, cls, opt, sam, sa,
                                           out, disposition);
        });
}

LSTATUS WINAPI RegCloseKeyHook(HKEY k) {
    if (g_keyTracker.IsFake(k)) {
        g_keyTracker.FreeFake(k);
        return ERROR_SUCCESS;
    }
    LSTATUS s = RegCloseKeyOriginal(k);
    g_keyTracker.Untrack(k);
    ClearInjectedState(k);
    return s;
}

LSTATUS WINAPI RegQueryValueExWHook(HKEY k, LPCWSTR vn, LPDWORD r, LPDWORD t,
                                    LPBYTE d, LPDWORD cb) {
    try {
        std::wstring p = g_keyTracker.GetPath(k);
        if (!p.empty()) {
            std::wstring v = vn ? vn : L"";
            LSTATUS o;
            if (TryProvideValue(p, v, t, d, cb, o)) return o;
        }
        if (g_keyTracker.IsFake(k)) return ERROR_FILE_NOT_FOUND;
        return RegQueryValueExWOriginal(k, vn, r, t, d, cb);
    } catch (...) {
        return RegQueryValueExWOriginal(k, vn, r, t, d, cb);
    }
}

LSTATUS WINAPI RegGetValueWHook(HKEY hk, LPCWSTR sub, LPCWSTR val, DWORD fl,
                                LPDWORD tp, PVOID d, LPDWORD cb) {
    try {
        std::wstring p = g_keyTracker.GetPath(hk);
        if (sub && *sub) {
            if (!p.empty()) p += L"\\";
            p += sub;
        }
        if (!p.empty()) {
            std::wstring v = val ? val : L"";
            LSTATUS o;
            if (TryProvideValue(p, v, tp, static_cast<LPBYTE>(d), cb, o)) return o;
        }
        if (g_keyTracker.IsFake(hk)) return ERROR_FILE_NOT_FOUND;
        return RegGetValueWOriginal(hk, sub, val, fl, tp, d, cb);
    } catch (...) {
        return RegGetValueWOriginal(hk, sub, val, fl, tp, d, cb);
    }
}

LSTATUS WINAPI RegEnumKeyExWHook(HKEY k, DWORD idx, LPWSTR name, LPDWORD lpcch,
                                 LPDWORD r, LPWSTR cls, LPDWORD lpcCls,
                                 PFILETIME ft) {
    try {
        if (g_keyTracker.IsFake(k)) {
            std::wstring p = g_keyTracker.GetPath(k);
            VNode n = ClassifyPath(p);
            std::wstring s;
            if (!GetVirtualSubKeyName(n, idx, s)) return ERROR_NO_MORE_ITEMS;
            if (!lpcch || !name) return ERROR_INVALID_PARAMETER;
            if (*lpcch < s.size() + 1) {
                *lpcch = static_cast<DWORD>(s.size() + 1);
                return ERROR_MORE_DATA;
            }
            wcscpy_s(name, *lpcch, s.c_str());
            *lpcch = static_cast<DWORD>(s.size());
            if (ft) GetSystemTimeAsFileTime(ft);
            return ERROR_SUCCESS;
        }
        const std::wstring path = g_keyTracker.GetPath(k);
        if (!IsNamespaceParentKey(path) || !g_dllVerifiedOk.load())
            return RegEnumKeyExWOriginal(k, idx, name, lpcch, r, cls, lpcCls, ft);
        const LSTATUS st = RegEnumKeyExWOriginal(k, idx, name, lpcch, r, cls, lpcCls, ft);
        if (st != ERROR_NO_MORE_ITEMS) return st;
        {
            HKEY hTest = nullptr;
            if (RegOpenKeyExWOriginal(k, g_clsidLower.c_str(), 0, KEY_READ, &hTest) ==
                ERROR_SUCCESS) {
                RegCloseKeyOriginal(hTest);
                return ERROR_NO_MORE_ITEMS;
            }
        }
        // Validate the output buffer BEFORE latching the one-shot injection
        // state: a rejected attempt (ERROR_MORE_DATA / ERROR_INVALID_PARAMETER)
        // must not consume the injection, or the caller's retry at the same
        // index would hit the latched flag and silently lose the applet entry.
        if (!lpcch || !name) return ERROR_INVALID_PARAMETER;
        if (*lpcch < g_clsidLower.size() + 1) {
            *lpcch = static_cast<DWORD>(g_clsidLower.size() + 1);
            return ERROR_MORE_DATA;
        }
        if (!ShouldInjectNow(k, idx)) return ERROR_NO_MORE_ITEMS;
        wcscpy_s(name, *lpcch, g_clsidLower.c_str());
        *lpcch = static_cast<DWORD>(g_clsidLower.size());
        if (ft) GetSystemTimeAsFileTime(ft);
        return ERROR_SUCCESS;
    } catch (...) {
        return RegEnumKeyExWOriginal(k, idx, name, lpcch, r, cls, lpcCls, ft);
    }
}

LSTATUS WINAPI RegEnumKeyWHook(HKEY k, DWORD idx, LPWSTR name, DWORD cch) {
    try {
        if (g_keyTracker.IsFake(k)) {
            std::wstring p = g_keyTracker.GetPath(k);
            VNode n = ClassifyPath(p);
            std::wstring s;
            if (!GetVirtualSubKeyName(n, idx, s)) return ERROR_NO_MORE_ITEMS;
            if (!name) return ERROR_INVALID_PARAMETER;
            if (cch <= s.size()) return ERROR_MORE_DATA;
            wcscpy_s(name, cch, s.c_str());
            return ERROR_SUCCESS;
        }
        const std::wstring path = g_keyTracker.GetPath(k);
        if (!IsNamespaceParentKey(path) || !g_dllVerifiedOk.load())
            return RegEnumKeyWOriginal(k, idx, name, cch);
        const LSTATUS st = RegEnumKeyWOriginal(k, idx, name, cch);
        if (st != ERROR_NO_MORE_ITEMS) return st;
        {
            HKEY hTest = nullptr;
            if (RegOpenKeyExWOriginal(k, g_clsidLower.c_str(), 0, KEY_READ, &hTest) ==
                ERROR_SUCCESS) {
                RegCloseKeyOriginal(hTest);
                return ERROR_NO_MORE_ITEMS;
            }
        }
        // Same ordering as RegEnumKeyExWHook: validate the buffer before
        // latching the injection state so a retry after ERROR_MORE_DATA still
        // sees the injected entry.
        if (!name) return ERROR_INVALID_PARAMETER;
        if (cch <= g_clsidLower.size()) return ERROR_MORE_DATA;
        if (!ShouldInjectNow(k, idx)) return ERROR_NO_MORE_ITEMS;
        wcscpy_s(name, cch, g_clsidLower.c_str());
        return ERROR_SUCCESS;
    } catch (...) {
        return RegEnumKeyWOriginal(k, idx, name, cch);
    }
}

LSTATUS WINAPI RegQueryInfoKeyWHook(HKEY k, LPWSTR cls, LPDWORD lpcCls, LPDWORD r,
                                    LPDWORD cSubKeys, LPDWORD lpcMaxSub,
                                    LPDWORD lpcMaxCls, LPDWORD cValues,
                                    LPDWORD lpcMaxValName, LPDWORD lpcMaxValData,
                                    LPDWORD sec, PFILETIME ft) {
    if (g_keyTracker.IsFake(k)) {
        std::wstring p = g_keyTracker.GetPath(k);
        VNode n = ClassifyPath(p);
        if (cSubKeys) *cSubKeys = GetVirtualSubKeyCount(n);
        // Report 0 values on synthetic keys: the virtualization layer has no
        // enumerable value store (RegEnumValueW is not hooked), so a caller
        // that sizes its enumeration from RegQueryInfoKeyW must not attempt to
        // enumerate values on a fake handle (which would be interpreted as a
        // kernel handle by the real RegEnumValueW and fail).
        if (cValues) *cValues = 0;
        if (lpcMaxSub) *lpcMaxSub = 32;
        if (lpcMaxCls) *lpcMaxCls = 0;
        if (lpcMaxValName) *lpcMaxValName = 64;
        if (lpcMaxValData) *lpcMaxValData = 512;
        if (cls && lpcCls) {
            if (*lpcCls > 0) cls[0] = 0;
            *lpcCls = 0;
        }
        if (ft) GetSystemTimeAsFileTime(ft);
        return ERROR_SUCCESS;
    }
    std::wstring path = g_keyTracker.GetPath(k);
    if (IsNamespaceParentKey(path) && g_dllVerifiedOk.load()) {
        LSTATUS st =
            RegQueryInfoKeyWOriginal(k, cls, lpcCls, r, cSubKeys, lpcMaxSub,
                                     lpcMaxCls, cValues, lpcMaxValName, lpcMaxValData,
                                     sec, ft);
        if (st == ERROR_SUCCESS && cSubKeys) {
            HKEY hTest = nullptr;
            if (RegOpenKeyExWOriginal(k, g_clsidLower.c_str(), 0, KEY_READ, &hTest) !=
                ERROR_SUCCESS) {
                (*cSubKeys)++;
                if (lpcMaxSub) {
                    DWORD need = static_cast<DWORD>(g_clsidLower.size() + 1);
                    if (*lpcMaxSub < need) *lpcMaxSub = need;
                }
            } else {
                RegCloseKeyOriginal(hTest);
            }
        }
        return st;
    }
    return RegQueryInfoKeyWOriginal(k, cls, lpcCls, r, cSubKeys, lpcMaxSub, lpcMaxCls,
                                    cValues, lpcMaxValName, lpcMaxValData, sec, ft);
}

void* GetRegFunc(const char* n) {
    HMODULE h = GetModuleHandleW(L"kernelbase.dll");
    if (h) {
        void* p = reinterpret_cast<void*>(GetProcAddress(h, n));
        if (p) return p;
    }
    HMODULE a = GetModuleHandleW(L"advapi32.dll");
    if (!a) a = LoadLibraryW(L"advapi32.dll");
    if (a) {
        void* p = reinterpret_cast<void*>(GetProcAddress(a, n));
        if (p) return p;
    }
    return nullptr;
}

// -----------------------------------------------------------------------------
// Translation hooks (LoadStringW/A + DirectUI XResourceProvider)
// -----------------------------------------------------------------------------
using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
using LoadStringA_t = int(WINAPI*)(HINSTANCE, UINT, LPSTR, int);
LoadStringW_t LoadStringWOriginal = nullptr;
LoadStringA_t LoadStringAOriginal = nullptr;

// The PerfCenter module handle is resolved once and we compare pointers only,
// which avoids a per-call GetModuleFileNameW on the hot path.
static bool IsPerfCenterResourceModule(HINSTANCE instance) {
    if (!instance) return false;
    const ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(instance);
    HMODULE module = reinterpret_cast<HMODULE>(raw & ~static_cast<ULONG_PTR>(3));
    HMODULE h = g_hPerfCenter.load();
    if (h && module == h) return true;
    HMODULE lr = g_hLocalizedResources.load();
    if (lr && module == lr) return true;
    return false;
}

static int CopyEmbeddedStringW(const wchar_t* text, LPWSTR buffer, int bufferChars) {
    if (!text) return 0;
    const int length = static_cast<int>(wcslen(text));
    // Note: the cchBufferMax == 0 (return-a-pointer) form of LoadStringW is
    // deliberately not emulated here - see LoadStringWHook. Callers of this
    // function always pass bufferChars > 0.
    if (!buffer || bufferChars < 1) return 0;
    const int copied = length < bufferChars - 1 ? length : bufferChars - 1;
    if (copied > 0)
        memcpy(buffer, text, static_cast<size_t>(copied) * sizeof(wchar_t));
    buffer[copied] = L'\0';
    return copied;
}

int WINAPI LoadStringWHook(HINSTANCE instance, UINT id, LPWSTR buffer,
                           int bufferChars) {
    // The cchBufferMax == 0 form of LoadStringW returns a pointer into the
    // string's home resource module, which stays mapped for the module's
    // lifetime. Our embedded strings live in the mod's own image, which is
    // unmapped when the mod is disabled/unloaded - handing out a pointer to
    // them would leave the caller (DirectUI's resstr resolution) with a
    // dangling pointer. Fall through to the original for that form: the
    // private resource module already has the translated strings baked in, so
    // the pointer it returns is both correct and safe.
    if (bufferChars != 0 && g_forceTranslations.load() &&
        static_cast<DllVariant>(g_activeVariant.load()) == DllVariant::Win8 &&
        IsPerfCenterResourceModule(instance)) {
        if (const wchar_t* text = GetEmbeddedTranslation(id)) {
            return CopyEmbeddedStringW(text, buffer, bufferChars);
        }
    }
    return LoadStringWOriginal(instance, id, buffer, bufferChars);
}

int WINAPI LoadStringAHook(HINSTANCE instance, UINT id, LPSTR buffer,
                           int bufferChars) {
    // bufferChars == 0 is also passed through to the original: the A variant
    // must not fabricate results for the pointer-returning form either.
    if (bufferChars > 0 && g_forceTranslations.load() &&
        static_cast<DllVariant>(g_activeVariant.load()) == DllVariant::Win8 &&
        IsPerfCenterResourceModule(instance)) {
        if (const wchar_t* text = GetEmbeddedTranslation(id)) {
            if (!buffer || bufferChars < 1) return 0;
            const int wideLength = static_cast<int>(wcslen(text));
            // Note: converting to CP_ACP is lossy for non-ANSI characters on a
            // mismatched codepage; unavoidable for the A variant.
            int byteLength = WideCharToMultiByte(CP_ACP, 0, text, wideLength,
                                                 nullptr, 0, nullptr, nullptr);
            if (byteLength <= 0) return 0;
            std::string converted(static_cast<size_t>(byteLength), '\0');
            if (byteLength > 0)
                WideCharToMultiByte(CP_ACP, 0, text, wideLength, &converted[0],
                                    byteLength, nullptr, nullptr);
            const int copied =
                byteLength < bufferChars - 1 ? byteLength : bufferChars - 1;
            if (copied > 0)
                memcpy(buffer, converted.data(), static_cast<size_t>(copied));
            buffer[copied] = '\0';
            return copied;
        }
    }
    return LoadStringAOriginal(instance, id, buffer, bufferChars);
}

using XResourceProviderCreate_t = HRESULT(*)(HINSTANCE, LPCWSTR, LPCWSTR, LPCWSTR,
                                             void**);
static XResourceProviderCreate_t XResourceProviderCreateOriginal = nullptr;

HRESULT XResourceProviderCreateHook(HINSTANCE instance, LPCWSTR resourceName,
                                    LPCWSTR resourceType, LPCWSTR stylesheetName,
                                    void** provider) {
    HINSTANCE resourceInstance = instance;
    if (g_forceTranslations.load() &&
        static_cast<DllVariant>(g_activeVariant.load()) == DllVariant::Win8 &&
        IsPerfCenterResourceModule(instance)) {
        if (HMODULE localized = EnsureLocalizedResourceModuleLoaded()) {
            resourceInstance = reinterpret_cast<HINSTANCE>(localized);
        }
    }
    return XResourceProviderCreateOriginal(resourceInstance, resourceName,
                                           resourceType, stylesheetName, provider);
}


// -----------------------------------------------------------------------------
// DirectUI XML patch - optional Windows 7 link layout, in memory only
// -----------------------------------------------------------------------------
#ifdef _WIN64
#define PERF_DUI_THISCALL __cdecl
#else
#define PERF_DUI_THISCALL __thiscall
#endif

using DUISetXML_t = HRESULT(PERF_DUI_THISCALL*)(void*, const WCHAR*, HINSTANCE,
                                                HINSTANCE);
using DUISetXMLFromResource_t = HRESULT(PERF_DUI_THISCALL*)(
    void*, PCWSTR, PCWSTR, HMODULE, HINSTANCE, HINSTANCE);

static DUISetXML_t DUISetXML = nullptr;
static DUISetXMLFromResource_t DUISetXMLFromResourceOriginal = nullptr;
static thread_local int g_inPerfCenterXmlPatch = 0;


static std::wstring LoadUifileXml(HMODULE module, PCWSTR resourceName,
                                  PCWSTR resourceType) {
    HRSRC resource = FindResourceW(module, resourceName, resourceType);
    if (!resource) return {};

    HGLOBAL loaded = LoadResource(module, resource);
    if (!loaded) return {};

    DWORD size = SizeofResource(module, resource);
    const char* data = static_cast<const char*>(LockResource(loaded));
    if (!data || !size) return {};

    int wideLength = MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(size),
                                         nullptr, 0);
    UINT codePage = CP_UTF8;
    if (wideLength <= 0) {
        wideLength = MultiByteToWideChar(CP_ACP, 0, data, static_cast<int>(size),
                                         nullptr, 0);
        codePage = CP_ACP;
    }
    if (wideLength <= 0) return {};

    std::wstring xml(static_cast<size_t>(wideLength), L'\0');
    MultiByteToWideChar(codePage, 0, data, static_cast<int>(size), &xml[0],
                        wideLength);
    while (!xml.empty() &&
           (xml.back() == L'\0' || xml.back() == L'\r' || xml.back() == L'\n')) {
        xml.pop_back();
    }
    return xml;
}

static bool FindOuterElement(const std::wstring& xml, size_t markerPos,
                             size_t& outStart, size_t& outEnd) {
    size_t start = xml.rfind(L"<element", markerPos);
    if (start == std::wstring::npos) return false;

    int depth = 0;
    size_t i = start;
    size_t steps = 0;
    while (i < xml.size() && ++steps < 250000) {
        if (xml.compare(i, 8, L"<element") == 0) {
            size_t gt = xml.find(L'>', i);
            if (gt == std::wstring::npos) return false;
            if (gt > i && xml[gt - 1] == L'/') {
                i = gt + 1;
                continue;
            }
            ++depth;
            i = gt + 1;
            continue;
        }
        if (xml.compare(i, 10, L"</element>") == 0) {
            --depth;
            i += 10;
            if (depth == 0) {
                outStart = start;
                outEnd = i;
                return true;
            }
            continue;
        }
        ++i;
    }
    return false;
}

static std::wstring XmlAttrEscape(const wchar_t* text) {
    std::wstring out;
    if (!text) return out;
    for (const wchar_t* p = text; *p; ++p) {
        switch (*p) {
            case L'&': out += L"&amp;"; break;
            case L'\"': out += L"&quot;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            default: out.push_back(*p); break;
        }
    }
    return out;
}

static void ReplaceAll(std::wstring& text, const std::wstring& from,
                       const std::wstring& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::wstring::npos) {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }
}

static const wchar_t* PerfTitleText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Classificare e migliorare le prestazioni del computer";
        case MuiLanguage::ES_ES: return L"Califica y mejora el rendimiento del equipo";
        case MuiLanguage::FR_FR: return L"Évaluer et améliorer les performances de l'ordinateur";
        case MuiLanguage::TR_TR: return L"Bilgisayarınızın performansını derecelendirin ve geliştirin";
        case MuiLanguage::RU_RU: return L"Оценить и повысить производительность компьютера";
        case MuiLanguage::ZH_CN: return L"为计算机的性能评分并进行改进";
        case MuiLanguage::DE_DE: return L"Bewerten und Verbessern der Computerleistung";
        case MuiLanguage::PT_BR: return L"Classifique e melhore o desempenho do computador";
        case MuiLanguage::PL_PL: return L"Oceń i popraw wydajność komputera";
        default: return L"Rate and improve your computer's performance";
    }
}

static const wchar_t* PerfDescriptionText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"L'indice prestazioni Windows valuta i componenti chiave del sistema in base a una scala da 1,0 a 9,9.";
        case MuiLanguage::ES_ES: return L"El Índice de experiencia de Windows evalúa los componentes principales del sistema en una escala de 1,0 a 9,9.";
        case MuiLanguage::FR_FR: return L"L'indice de performance Windows évalue les composants principaux du système sur une échelle de 1,0 à 9,9.";
        case MuiLanguage::TR_TR: return L"Windows Deneyim Dizini, temel sistem bileşenlerini 1,0 ile 9,9 arasında bir ölçekte değerlendirir.";
        case MuiLanguage::RU_RU: return L"Индекс производительности Windows оценивает основные компоненты системы по шкале от 1,0 до 9,9.";
        case MuiLanguage::ZH_CN: return L"Windows 体验指数按 1.0 到 9.9 的等级评估关键系统组件。";
        case MuiLanguage::DE_DE: return L"Der Windows-Leistungsindex bewertet wichtige Systemkomponenten auf einer Skala von 1,0 bis 9,9.";
        case MuiLanguage::PT_BR: return L"O Índice de Experiência do Windows avalia os principais componentes do sistema em uma escala de 1,0 a 9,9.";
        case MuiLanguage::PL_PL: return L"Indeks wydajności systemu Windows ocenia główne składniki systemu w skali od 1,0 do 9,9.";
        default: return L"The Windows Experience Index assesses key system components on a scale of 1.0 to 9.9.";
    }
}

static const wchar_t* RateThisComputerText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Classifica computer";
        case MuiLanguage::ES_ES: return L"Evaluar este equipo";
        case MuiLanguage::FR_FR: return L"Évaluer cet ordinateur";
        case MuiLanguage::TR_TR: return L"Bu bilgisayarı derecelendir";
        case MuiLanguage::RU_RU: return L"Оценить компьютер";
        case MuiLanguage::ZH_CN: return L"评估此计算机";
        case MuiLanguage::DE_DE: return L"Diesen Computer bewerten";
        case MuiLanguage::PT_BR: return L"Avaliar este computador";
        case MuiLanguage::PL_PL: return L"Oceń ten komputer";
        default: return L"Rate this computer";
    }
}

static const wchar_t* RerunAssessmentText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Esegui di nuovo la valutazione";
        case MuiLanguage::ES_ES: return L"Repetir la evaluación";
        case MuiLanguage::FR_FR: return L"Réexécuter l'évaluation";
        case MuiLanguage::TR_TR: return L"Değerlendirmeyi yeniden çalıştır";
        case MuiLanguage::RU_RU: return L"Повторить оценку";
        case MuiLanguage::ZH_CN: return L"重新运行评估";
        case MuiLanguage::DE_DE: return L"Bewertung wiederholen";
        case MuiLanguage::PT_BR: return L"Repetir a avaliação";
        case MuiLanguage::PL_PL: return L"Uruchom ponownie ocenę";
        default: return L"Re-run the assessment";
    }
}

static const wchar_t* Win7NumbersMeanText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Significato dei numeri visualizzati";
        case MuiLanguage::ES_ES: return L"¿Qué significan estos números?";
        case MuiLanguage::FR_FR: return L"Que signifient ces nombres ?";
        case MuiLanguage::TR_TR: return L"Bu sayılar ne anlama geliyor?";
        case MuiLanguage::RU_RU: return L"Что означают эти числа?";
        case MuiLanguage::ZH_CN: return L"这些数字是什么意思？";
        case MuiLanguage::DE_DE: return L"Was bedeuten diese Zahlen?";
        case MuiLanguage::PT_BR: return L"O que significam estes números?";
        case MuiLanguage::PL_PL: return L"Co oznaczają te liczby?";
        default: return L"What do these numbers mean?";
    }
}

static const wchar_t* Win7ImproveText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Suggerimenti per migliorare le prestazioni del computer.";
        case MuiLanguage::ES_ES: return L"Sugerencias para mejorar el rendimiento del equipo.";
        case MuiLanguage::FR_FR: return L"Conseils pour améliorer les performances de l'ordinateur.";
        case MuiLanguage::TR_TR: return L"Bilgisayarınızın performansını artırmaya yönelik ipuçları.";
        case MuiLanguage::RU_RU: return L"Советы по повышению производительности компьютера.";
        case MuiLanguage::ZH_CN: return L"提高计算机性能的提示。";
        case MuiLanguage::DE_DE: return L"Tipps zum Verbessern der Computerleistung.";
        case MuiLanguage::PT_BR: return L"Dicas para melhorar o desempenho do computador.";
        case MuiLanguage::PL_PL: return L"Wskazówki dotyczące poprawy wydajności komputera.";
        default: return L"Tips for improving your computer's performance.";
    }
}

static const wchar_t* Win7MicrosoftLinkText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Ulteriori informazioni su punteggi e software online";
        case MuiLanguage::ES_ES: return L"Más información sobre puntuaciones y software en línea";
        case MuiLanguage::FR_FR: return L"En savoir plus sur les scores et les logiciels en ligne";
        case MuiLanguage::TR_TR: return L"Puanlar ve yazılımlar hakkında çevrimiçi daha fazla bilgi";
        case MuiLanguage::RU_RU: return L"Дополнительные сведения об оценках и программном обеспечении в Интернете";
        case MuiLanguage::ZH_CN: return L"联机了解有关分数和软件的详细信息";
        case MuiLanguage::DE_DE: return L"Weitere Informationen zu Bewertungen und Software online";
        case MuiLanguage::PT_BR: return L"Saiba mais online sobre pontuações e software";
        case MuiLanguage::PL_PL: return L"Dowiedz się więcej online o wynikach i oprogramowaniu";
        default: return L"Learn more about scores and software online";
    }
}

static const wchar_t* Win7NumbersDialogText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT:
            return L"L'Indice prestazioni Windows valuta i componenti principali del computer, come processore, memoria, grafica, grafica dei giochi e disco rigido primario. Il punteggio di base è determinato dal sottopunteggio più basso, perché quel componente rappresenta il principale collo di bottiglia delle prestazioni.";
        case MuiLanguage::ES_ES:
            return L"El Índice de experiencia de Windows evalúa los componentes principales del equipo, como el procesador, la memoria, los gráficos, los gráficos de juego y el disco duro principal. La puntuación base viene determinada por la subpuntuación más baja, porque ese componente es el principal cuello de botella del rendimiento.";
        case MuiLanguage::FR_FR:
            return L"L'indice de performance Windows évalue les principaux composants de l'ordinateur, comme le processeur, la mémoire, les graphiques, les graphiques de jeu et le disque dur principal. Le score de base est déterminé par le sous-score le plus bas, car ce composant représente le principal goulot d'étranglement des performances.";
        case MuiLanguage::TR_TR:
            return L"Windows Deneyim Dizini işlemci, bellek, grafikler, oyun grafikleri ve birincil sabit disk gibi bilgisayarın temel bileşenlerini değerlendirir. Temel puan en düşük alt puana göre belirlenir, çünkü bu bileşen performansın ana darboğazıdır.";
        case MuiLanguage::RU_RU:
            return L"Индекс производительности Windows оценивает основные компоненты компьютера, такие как процессор, память, графика, игровая графика и основной жесткий диск. Базовая оценка определяется наименьшей подоценкой, поскольку этот компонент является главным ограничением производительности.";
        case MuiLanguage::ZH_CN:
            return L"Windows 体验指数会评估计算机的关键组件，例如处理器、内存、图形、游戏图形和主硬盘。基本分数由最低的子分数决定，因为该组件是主要的性能瓶颈。";
        case MuiLanguage::DE_DE:
            return L"Der Windows-Leistungsindex bewertet wichtige Komponenten des Computers, z. B. Prozessor, Arbeitsspeicher, Grafik, Gaminggrafik und primäre Festplatte. Die Basisbewertung wird durch die niedrigste Teilbewertung bestimmt, da diese Komponente den wichtigsten Leistungsengpass darstellt.";
        case MuiLanguage::PT_BR:
            return L"O Índice de Experiência do Windows avalia os principais componentes do computador, como processador, memória, elementos gráficos, gráficos de jogos e disco rígido principal. A pontuação base é determinada pela menor subpontuação, pois esse componente é o principal gargalo de desempenho.";
        case MuiLanguage::PL_PL:
            return L"Indeks wydajności systemu Windows ocenia główne składniki komputera, takie jak procesor, pamięć, grafika, grafika w grach i podstawowy dysk twardy. Wynik podstawowy jest określany przez najniższy podwynik, ponieważ ten składnik stanowi główne wąskie gardło wydajności.";
        default:
            return L"The Windows Experience Index rates key components of your computer, such as the processor, memory, graphics, gaming graphics and primary hard disk. The base score is determined by the lowest subscore, because that component is the main performance bottleneck.";
    }
}

static const wchar_t* Win7TipsDialogText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT:
            return L"Per migliorare il punteggio, aggiorna i driver dei dispositivi, chiudi i programmi di avvio non necessari, usa un piano di alimentazione ad alte prestazioni, mantieni spazio libero sul disco ed esegui di nuovo la valutazione dopo modifiche hardware o driver.";
        case MuiLanguage::ES_ES:
            return L"Para mejorar la puntuación, actualiza los controladores de dispositivo, cierra programas de inicio innecesarios, usa un plan de energía de alto rendimiento, mantén suficiente espacio libre en disco y vuelve a ejecutar la evaluación después de cambios de hardware o controladores.";
        case MuiLanguage::FR_FR:
            return L"Pour améliorer le score, mettez à jour les pilotes de périphériques, fermez les programmes de démarrage inutiles, utilisez un mode de gestion de l'alimentation hautes performances, gardez suffisamment d'espace disque libre et relancez l'évaluation après des changements matériels ou de pilotes.";
        case MuiLanguage::TR_TR:
            return L"Puanınızı artırmak için aygıt sürücülerini güncelleyin, gereksiz başlangıç programlarını kapatın, yüksek performanslı bir güç planı kullanın, yeterli boş disk alanı bırakın ve donanım ya da sürücü değişikliklerinden sonra değerlendirmeyi yeniden çalıştırın.";
        case MuiLanguage::RU_RU:
            return L"Чтобы повысить оценку, обновите драйверы устройств, отключите ненужные программы автозагрузки, используйте план питания высокой производительности, оставляйте достаточно свободного места на диске и повторно запускайте оценку после изменений оборудования или драйверов.";
        case MuiLanguage::ZH_CN:
            return L"若要提高分数，请更新设备驱动程序，关闭不必要的启动程序，使用高性能电源计划，保留足够的可用磁盘空间，并在更改硬件或驱动程序后重新运行评估。";
        case MuiLanguage::DE_DE:
            return L"Um die Bewertung zu verbessern, aktualisieren Sie Gerätetreiber, schließen Sie unnötige Autostartprogramme, verwenden Sie einen Energiesparplan mit hoher Leistung, halten Sie genügend freien Speicherplatz bereit und führen Sie die Bewertung nach Hardware- oder Treiberänderungen erneut aus.";
        case MuiLanguage::PT_BR:
            return L"Para melhorar sua pontuação, atualize os drivers de dispositivo, feche programas de inicialização desnecessários, use um plano de energia de alto desempenho, mantenha espaço livre suficiente em disco e execute a avaliação novamente após alterações de hardware ou driver.";
        case MuiLanguage::PL_PL:
            return L"Aby poprawić wynik, zaktualizuj sterowniki urządzeń, zamknij niepotrzebne programy startowe, użyj planu zasilania o wysokiej wydajności, zachowaj wystarczającą ilość wolnego miejsca na dysku i ponownie uruchom ocenę po zmianach sprzętu lub sterowników.";
        default:
            return L"To improve your score, update device drivers, close unnecessary startup programs, use a high performance power plan, keep enough free disk space and re-run the assessment after hardware or driver changes.";
    }
}

static const wchar_t* Win7ViewPrintText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Visualizza e stampa dettagli";
        case MuiLanguage::ES_ES: return L"Ver e imprimir detalles";
        case MuiLanguage::FR_FR: return L"Afficher et imprimer les détails";
        case MuiLanguage::TR_TR: return L"Ayrıntıları görüntüle ve yazdır";
        case MuiLanguage::RU_RU: return L"Просмотр и печать сведений";
        case MuiLanguage::ZH_CN: return L"查看并打印详细信息";
        case MuiLanguage::DE_DE: return L"Details anzeigen und drucken";
        case MuiLanguage::PT_BR: return L"Exibir e imprimir detalhes";
        case MuiLanguage::PL_PL: return L"Wyświetl i wydrukuj szczegóły";
        default: return L"View and print details";
    }
}

static const wchar_t* VistaNotSureText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Non sai da dove iniziare?";
        default: return L"Not sure where to start?";
    }
}

static const wchar_t* VistaImproveLinkText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Informazioni su come migliorare le prestazioni del computer.";
        default: return L"Learn how you can improve your computer's performance.";
    }
}

static const wchar_t* VistaLearnScoresText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Ulteriori informazioni sui punteggi online";
        default: return L"Learn more about the scores online";
    }
}

static const wchar_t* VistaSoftwareForScoreText(MuiLanguage lang) {
    switch (lang) {
        case MuiLanguage::IT_IT: return L"Visualizza software per il punteggio di base online";
        default: return L"View software for my base score online";
    }
}

static std::wstring Win7PerfLinksXml() {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();
    std::wstring xml = LR"PC(<element id="atom(perfhub_details_wincrslinks_1)" layoutpos="top" layout="tablelayout(0,0, 0,2,-50, 1,2,-50)"><NavigateButton layoutpos="top" layout="flowlayout(0,2,0,2)" padding="rect(0rp,7rp,0rp,7rp)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:numbers"><element sheet="perfhub_style" class="perfhub_nav_img" id="atom(perfhub_details_numbersmeanimg)" layoutpos="left" padding="rect(15rp, 0rp, 15rp, 0rp)" content="icon(99, 32rp, 32rp, library(imageres.dll))" accname="Question mark image"/><element layout="filllayout()" layoutpos="top"><button class="cp_content_link" active="mouse|keyboard" layoutpos="left" content="%NUMBERS%"/></element></NavigateButton></element><element id="atom(perfhub_details_wincrslinks_2)" layoutpos="top" layout="tablelayout(0,0, 0,2,-50, 1,2,-50)"><NavigateButton id="atom(perfhub_page_description_container)" layout="borderlayout()" layoutpos="top" padding="rect(0rp, 0rp, 7rp, 0rp)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:tips"><element sheet="perfhub_style" class="perfhub_nav_img" id="atom(perfhub_details_improvesystemimg)" layoutpos="left" padding="rect(15rp, 0rp, 15rp, 0rp)" content="icon(99, 32rp, 32rp, library(imageres.dll))" accname="Question mark image"/><button class="cp_content_link" active="mouse|keyboard" layoutpos="top" content="%IMPROVE%"/></NavigateButton></element>)PC";
    ReplaceAll(xml, L"%NUMBERS%", XmlAttrEscape(Win7NumbersMeanText(lang)));
    ReplaceAll(xml, L"%IMPROVE%", XmlAttrEscape(Win7ImproveText(lang)));
    return xml;
}

static std::wstring Win7MicrosoftUpsellXml() {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();
    std::wstring xml = LR"PC(<element id="atom(perfhub_upsell_mscontainer)" layoutpos="top" layout="borderlayout()" bordercolor="gtc(CONTROLPANELSTYLE,17,0,3821)" borderthickness="rect(1rp,1rp,1rp,1rp)" padding="rect(7rp,7rp,7rp,7rp)"><element sheet="perfhub_style" class="perfhub_nav_img" id="atom(perfhub_upsell_mslogo)" layoutpos="left" padding="rect(0rp, 0rp, 15rp, 0rp)" content="icon(271, 40rp, 40rp, library(shell32.dll))" accname="Microsoft upsell image"/><NavigateButton layoutpos="client" layout="flowlayout(0,2,0,2)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:learnmore"><element layout="filllayout()" layoutpos="top"><button sheet="cp_style" class="cp_content_link" cursor="hand" active="mouse|keyboard" content="%LEARN_MORE%"/></element></NavigateButton></element>)PC";
    ReplaceAll(xml, L"%LEARN_MORE%", XmlAttrEscape(Win7MicrosoftLinkText(lang)));
    return xml;
}

static bool HasWinsatAssessmentData() {
    wchar_t windowsDir[MAX_PATH] = {};
    if (!GetWindowsDirectoryW(windowsDir, ARRAYSIZE(windowsDir))) return false;

    std::wstring pattern = std::wstring(windowsDir) +
        L"\\Performance\\WinSAT\\DataStore\\*Formal.Assessment*.xml";
    WIN32_FIND_DATAW fd{};
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;
    FindClose(h);
    return true;
}

static std::wstring VistaStartHelpXml() {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();
    std::wstring prefix = XmlAttrEscape(VistaNotSureText(lang));
    std::wstring link = XmlAttrEscape(VistaImproveLinkText(lang));

    std::wstring xml;
    xml += LR"PC(<element layoutpos="top" layout="flowlayout(0,2,0,2)" padding="rect(0rp,0rp,0rp,0rp)">)PC";
    xml += LR"PC(<button layoutpos="left" accessible="true" accrole="graphic" padding="rect(0rp,0rp,8rp,0rp)" content="icon(61001,16rp,16rp,library(shell32.dll))" accname="Help image"/>)PC";
    xml += LR"PC(<element sheet="cp_style" class="cp_content_text" layoutpos="left" content=")PC" + prefix + L"\"/>";
    xml += LR"PC(<element layoutpos="left" width="4rp"/>)PC";
    xml += LR"PC(<NavigateButton layoutpos="left" layout="flowlayout()" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:tips">)PC";
    xml += LR"PC(<button sheet="cp_style" class="cp_content_link" cursor="hand" active="mouse|keyboard" content=")PC" + link + L"\"/>";
    xml += LR"PC(</NavigateButton></element>)PC";
    return xml;
}

static std::wstring VistaPerfLinksXml() {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();
    const bool rated = HasWinsatAssessmentData();

    std::wstring xml = LR"PC(<element id="atom(perfhub_details_wincrslinks_1)" layoutpos="top" layout="tablelayout(0,0, 0,2,-50, 1,2,-50)">%RATED_SECTION%</element><element id="atom(perfhub_details_wincrslinks_2)" layoutpos="top" layout="tablelayout(0,0, 0,2,-50, 1,2,-50)"><NavigateButton layoutpos="top" layout="flowlayout(0,2,0,2)" padding="rect(0rp,7rp,0rp,7rp)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:learnmore"><button sheet="cp_style" class="cp_content_link" cursor="hand" active="mouse|keyboard" layoutpos="left" content="%SCORES%"/></NavigateButton><element layout="filllayout()"/></element>)PC";

    std::wstring ratedSection;
    if (rated) {
        ratedSection = LR"PC(<NavigateButton layoutpos="top" layout="flowlayout(0,2,0,2)" padding="rect(0rp,7rp,0rp,7rp)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:viewprint"><button sheet="cp_style" class="cp_content_link" cursor="hand" active="mouse|keyboard" layoutpos="left" content="%VIEWPRINT%"/></NavigateButton><NavigateButton layoutpos="client" layout="flowlayout(0,2,0,2)" padding="rect(0rp,7rp,0rp,7rp)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:numbers"><button sheet="cp_style" class="cp_content_link" cursor="hand" active="mouse|keyboard" layoutpos="left" content="%NUMBERS%"/></NavigateButton>)PC";
    } else {
        ratedSection = LR"PC(<element layout="filllayout()"/><NavigateButton layoutpos="client" layout="flowlayout(0,2,0,2)" padding="rect(0rp,7rp,0rp,7rp)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:numbers"><button sheet="cp_style" class="cp_content_link" cursor="hand" active="mouse|keyboard" layoutpos="left" content="%NUMBERS%"/></NavigateButton>)PC";
    }
    ReplaceAll(ratedSection, L"%NUMBERS%", XmlAttrEscape(Win7NumbersMeanText(lang)));
    if (rated) {
        ReplaceAll(ratedSection, L"%VIEWPRINT%", XmlAttrEscape(Win7ViewPrintText(lang)));
    }

    ReplaceAll(xml, L"%RATED_SECTION%", ratedSection);
    ReplaceAll(xml, L"%SCORES%", XmlAttrEscape(VistaLearnScoresText(lang)));
    return xml;
}

static std::wstring VistaSoftwareUpsellXml() {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();
    std::wstring text = XmlAttrEscape(VistaSoftwareForScoreText(lang));

    std::wstring xml;
    xml += LR"PC(<element id="atom(perfhub_vista_software_container)" layoutpos="top" layout="borderlayout()" bordercolor="gtc(CONTROLPANELSTYLE,17,0,3821)" borderthickness="rect(1rp,1rp,1rp,1rp)" padding="rect(12rp,7rp,7rp,7rp)">)PC";
    xml += LR"PC(<element sheet="perfhub_style" class="perfhub_nav_img" layoutpos="left" padding="rect(0rp,0rp,15rp,0rp)" content="icon(271,40rp,40rp,library(shell32.dll))" accname="Software image"/>)PC";
    xml += LR"PC(<NavigateButton layoutpos="client" layout="flowlayout(0,2,0,2)" shellexecute="%SystemRoot%\explorer.exe" shellexecuteparams="perfcenter-restorer:learnmore"><button sheet="cp_style" class="cp_content_link" cursor="hand" active="mouse|keyboard" content=")PC" + text + L"\"/></NavigateButton>";
    xml += LR"PC(</element>)PC";
    return xml;
}

static std::wstring PatchPerfCenterVistaXml(std::wstring xml) {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();

    std::wstring descriptionMarker = L"content=\"" + XmlAttrEscape(PerfDescriptionText(lang)) + L"\"";
    size_t descriptionPos = xml.find(descriptionMarker);
    if (descriptionPos != std::wstring::npos) {
        size_t blockStart = 0;
        size_t blockEnd = 0;

        // The description line in the stock UIFILE is usually a self-closing
        // <element .../>. FindOuterElement intentionally ignores self-closing
        // elements, so handle that case here; otherwise the Vista "Not sure..."
        // row won't be inserted.
        size_t elementStart = xml.rfind(L"<element", descriptionPos);
        size_t tagEnd = (elementStart != std::wstring::npos) ? xml.find(L'>', elementStart)
                                                             : std::wstring::npos;
        if (elementStart != std::wstring::npos && tagEnd != std::wstring::npos &&
            tagEnd > elementStart && xml[tagEnd - 1] == L'/') {
            blockStart = elementStart;
            blockEnd = tagEnd + 1;
            xml.replace(blockStart, blockEnd - blockStart, VistaStartHelpXml());
        } else if (FindOuterElement(xml, descriptionPos, blockStart, blockEnd)) {
            xml.replace(blockStart, blockEnd - blockStart, VistaStartHelpXml());
        }
    }

    size_t linksMarker = xml.find(L"atom(perfhub_details_wincrslinks_1)");
    if (linksMarker != std::wstring::npos) {
        size_t blockStart = 0;
        size_t blockEnd = 0;
        if (FindOuterElement(xml, linksMarker, blockStart, blockEnd)) {
            xml.replace(blockStart, blockEnd - blockStart, VistaPerfLinksXml());
        }
    }

    if (xml.find(L"atom(perfhub_vista_software_container)") == std::wstring::npos) {
        size_t upsellContainer =
            xml.find(L"<element id=\"atom(perfhub_upsell_container)\"");
        if (upsellContainer != std::wstring::npos) {
            xml.insert(upsellContainer, VistaSoftwareUpsellXml());
        }
    }
    return xml;
}

static void InlineResString(std::wstring& xml, UINT id, MuiLanguage lang) {
    const wchar_t* text = GetMuiString(id, lang);
    if (!text || !*text) return;

    std::wstring from = L"content=\"resstr(" + std::to_wstring(id) + L")\"";
    std::wstring to = L"content=\"" + XmlAttrEscape(text) + L"\"";
    ReplaceAll(xml, from, to);
}

static std::wstring PatchPerfCenterCommonTextXml(const std::wstring& inputXml) {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();
    std::wstring xml = inputXml;

    // Inline the core table labels/descriptions before any skin-specific patch.
    // When SetXML is used with an XML string, some DirectUI resstr(...) lookups
    // can fail depending on the resource module/MUI state. These replacements
    // keep the original layout while ensuring the visible component names don't
    // disappear (Processor, Memory, Graphics, Gaming graphics, Primary hard disk,
    // table headers, unrated text, etc.).
    static const UINT idsToInline[] = {
        54, 1101, 1102, 1103, 1104, 1111,
        1185, 1186, 1188,
        1200, 1209, 1210, 1212, 1213, 1215, 1216,
        1218, 711, 1221, 1222,
    };
    for (UINT id : idsToInline) {
        InlineResString(xml, id, lang);
    }

    // Defensive fallback for DirectUI text that can be blank when the page is
    // parsed from the downloaded DLL without its original MUI companion. This
    // doesn't change the embedded string catalog and preserves the layout.
    ReplaceAll(xml, L"content=\"resstr(1182)\"",
               L"content=\"" + XmlAttrEscape(PerfTitleText(lang)) + L"\"");
    ReplaceAll(xml, L"content=\"resstr(1119)\"",
               L"content=\"" + XmlAttrEscape(PerfDescriptionText(lang)) + L"\"");
    ReplaceAll(xml, L"content=\"resstr(1186)\"",
               L"content=\"" + XmlAttrEscape(RateThisComputerText(lang)) + L"\"");
    ReplaceAll(xml, L"content=\"resstr(1188)\"",
               L"content=\"" + XmlAttrEscape(RerunAssessmentText(lang)) + L"\"");

    return xml;
}

static std::wstring PatchPerfCenterXml(const std::wstring& inputXml) {
    if (inputXml.find(L"atom(perfcentertoplevel)") == std::wstring::npos) {
        return inputXml;
    }

    std::wstring xml = PatchPerfCenterCommonTextXml(inputXml);

    if (g_useWindowsVistaSkin.load()) {
        return PatchPerfCenterVistaXml(xml);
    }

    if (!g_useWindows7Skin.load()) {
        return xml;
    }

    // Minimal Windows 7 skin patch: only restore the Windows 7 link blocks.
    // All other UIFILE content and all original string resources are left untouched.
    // content and all original string resources are left untouched.
    size_t linksMarker = xml.find(L"atom(perfhub_details_wincrslinks_1)");
    if (linksMarker != std::wstring::npos) {
        size_t blockStart = 0;
        size_t blockEnd = 0;
        if (FindOuterElement(xml, linksMarker, blockStart, blockEnd)) {
            xml.replace(blockStart, blockEnd - blockStart, Win7PerfLinksXml());
        }
    }

    if (xml.find(L"atom(perfhub_upsell_mscontainer)") == std::wstring::npos) {
        size_t upsellContainer =
            xml.find(L"<element id=\"atom(perfhub_upsell_container)\"");
        if (upsellContainer != std::wstring::npos) {
            xml.insert(upsellContainer, Win7MicrosoftUpsellXml());
        } else {
            size_t oemContainer =
                xml.find(L"<element id=\"atom(perfhub_upsell_oem_container)\"");
            if (oemContainer != std::wstring::npos) {
                xml.insert(oemContainer, Win7MicrosoftUpsellXml());
            }
        }
    }

    return xml;
}

static HRESULT PERF_DUI_THISCALL DUISetXMLFromResourceHook(
    void* parser, PCWSTR resourceName, PCWSTR resourceType, HMODULE resourceModule,
    HINSTANCE hInstance1, HINSTANCE hInstance2) {
    if (!DUISetXMLFromResourceOriginal) return E_FAIL;
    if (!DUISetXML || g_inPerfCenterXmlPatch) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType,
                                             resourceModule, hInstance1,
                                             hInstance2);
    }

    const bool isUifile101 =
        resourceType && !IS_INTRESOURCE(resourceType) &&
        !_wcsicmp(resourceType, L"UIFILE") && IS_INTRESOURCE(resourceName) &&
        static_cast<UINT>(reinterpret_cast<UINT_PTR>(resourceName)) == 101;

    if (!isUifile101) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType,
                                             resourceModule, hInstance1,
                                             hInstance2);
    }

    // Gate on g_dllVerifiedOk and the two resource modules we actually own.
    // The page can't be parsed before setup has completed: the registry entry
    // that lets the Control Panel offer "Performance Information and Tools" at
    // all comes from TryProvideValueData(), which bails out (returns false,
    // so nothing is exposed) unless g_dllVerifiedOk is set - so by the time any
    // UIFILE 101 for this applet is loaded, g_dllVerifiedOk is true and
    // g_hPerfCenter/g_hLocalizedResources have already been published. Without
    // this check, every DirectUI UIFILE 101 in the process (a very common
    // resource id across the shell's DUI modules) paid for a FindResource +
    // LoadResource + full XML conversion + substring search on every page
    // creation, just to find out it wasn't ours.
    if (!g_dllVerifiedOk.load() ||
        (resourceModule != g_hPerfCenter.load() &&
         resourceModule != g_hLocalizedResources.load())) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType,
                                             resourceModule, hInstance1,
                                             hInstance2);
    }

    std::wstring xml = LoadUifileXml(resourceModule, resourceName, resourceType);
    if (xml.empty() || xml.find(L"atom(perfcentertoplevel)") == std::wstring::npos) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType,
                                             resourceModule, hInstance1,
                                             hInstance2);
    }

    // Re-read the setting when a fresh page is being created. This makes a skin
    // choice made while the page was open take effect after the user closes and
    // opens the page again, without forcing a mod reload. It also makes startup
    // robust if Windhawk exposes string settings slightly later than Wh_ModInit.
    ResolvePageSkinForFreshPage();
    Wh_Log(L"PerfCenter UIFILE 101 load: page skin is %s (fresh7=%d freshVista=%d latched7=%d latchedVista=%d)",
           CurrentPageSkinName(),
           ReadPageSkinSetting() ? 1 : 0,
           ReadPageSkinVistaSetting() ? 1 : 0,
           g_useWindows7Skin.load() ? 1 : 0,
           g_useWindowsVistaSkin.load() ? 1 : 0);

    g_perfCenterXmlPageSeen.store(true);

    std::wstring patchedXml = PatchPerfCenterXml(xml);
    if (patchedXml == xml) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType,
                                             resourceModule, hInstance1,
                                             hInstance2);
    }

    Wh_Log(L"PerfCenter UIFILE 101 patched with Windows 7 link layout");
    ++g_inPerfCenterXmlPatch;
    HRESULT hr = DUISetXML(parser, patchedXml.c_str(),
                           reinterpret_cast<HINSTANCE>(resourceModule), hInstance1);
    --g_inPerfCenterXmlPatch;
    return hr;
}


static const int kVistaLampIconId = 61001;
static const char kVistaLampIconRtBase64[] = R"B64(KAAAABAAAAAgAAAAAQAgAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAACQgIEY5NTDN
LykjwwYDA1AAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///8BAAAAAE46OhqgmZDmzsO2/3pvYf8vKybw
AAAAUAAAACYAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAf39/AgAAAACQg3I8y7yt/sWzpv5xYFD+Qzsw8gAAADcAAAAh
AAAAGQAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAH9/fwIAAAAAenpxNqutp/mos7D/gY+M/2BlYO1VRjgSAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAB/f/8CAAAAAHu90EKu6Pb9yvz//6Lt/f+U0+Ty3eXlHgAAAAH///8BAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAqv//AwAAAACK3u9ltfT//6zt//6L2/H/j8/h/arW5UUA//8Bf///AgAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAKqq/wMAAAAAltvupqT1//+K4vj8kNnt/ZLY7P+Ry9+QAAAAAFWqqgMAAAAAAAAAAAAAAAAAAAAAAAAAAP///wIAAAAA
fMjeL6Di9POM5Pr+ht30/YLT6P2J0+f/ks/f5Wa7zB7///8BVaqqAwAAAAAAAAAAAAAAAAAAAAD///8BAAAAAJzY6q6t9f//
juT8/Y/j+f+E2fD/fM7h/aPg8P+RzN6TAAAAAH9/fwIAAAAAAAAAAAAAAAB///8CAAAAAHvK3US16vn+tvD//rvw//+68f//
nen8/4DV6/+U1uf+qtjm+HG90y8AAAAA////AQAAAAAAAAAAf///BAAAAACW3/Ga1P///973//za+P//zvT//8Pz//+T4vf/
g9Dl/b/o9f+KzNuIAAAAAH+/vwQAAAAAAAAAAJnM/wUAAAAAouj4qOL///72/f/74Pn//8z0///L9v//o+n8/4DS5/y86fX+
kc/dpQAAAACqqqoDAAAAAAAAAAC///8EAAAAAJfi9pnW///+9vz9/Oj7///O9P//xPT//5zn+/+G1Of8ruHw/4zM34gAAAAA
f7+/BAAAAAAAAAAAf///AgAAAACH2fBEsPD9/937//7m+P/9xvT//8L0//+i4/X9idTm/43M3vh5wtguAAAAAH9//wIAAAAA
AAAAAAAAAAD///8BAAAAAJLe9H+l8P3/sPL//6ft//+f6Pr/k9zw/37N4f17xNdtAAAAAP///wEAAAAAAAAAAAAAAAAAAAAA
AP//Af///wEAAAAAg9XsY4Tc8daF2e/5ftHn+HvL4Mt3wNJRAAAAAH9//wIAAAABAAAAAAAAAAD+fwAA/D8AAPw/AAD8PwAA
/D8AAPw/AAD4HwAA+B8AAPAPAADwDwAA4AcAAOAHAADgBwAA8A8AAPgfAAD8PwAA)B64";

using LoadImageW_t = decltype(&LoadImageW);
static LoadImageW_t LoadImageWOriginalForVistaLamp = nullptr;
// Reachable from any thread that hits any of the five icon hooks below, so
// this must not be a plain HICON: two concurrent first-callers could each
// create an icon and one would leak. std::atomic<HICON> with compare-exchange
// makes creation race-safe, and DestroyIcon in Wh_ModUninit reclaims it - the
// CopyIcon() results handed out to callers are independent handles, so
// destroying this master copy on unload is safe.
static std::atomic<HICON> g_vistaLampIcon{nullptr};

static int Base64Value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

static std::vector<BYTE> DecodeBase64(const char* input) {
    std::vector<BYTE> output;
    int val = 0;
    int valb = -8;
    for (const char* p = input; p && *p; ++p) {
        if (*p == '=') break;
        int decoded = Base64Value(*p);
        if (decoded < 0) continue;
        val = (val << 6) + decoded;
        valb += 6;
        if (valb >= 0) {
            output.push_back(static_cast<BYTE>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}

static HICON GetVistaLampIcon() {
    if (HICON existing = g_vistaLampIcon.load(std::memory_order_acquire)) {
        return existing;
    }
    std::vector<BYTE> iconData = DecodeBase64(kVistaLampIconRtBase64);
    if (iconData.empty()) return nullptr;
    HICON created = CreateIconFromResourceEx(iconData.data(),
                                             static_cast<DWORD>(iconData.size()),
                                             TRUE, 0x00030000, 16, 16,
                                             LR_DEFAULTCOLOR);
    if (!created) return nullptr;

    HICON expected = nullptr;
    if (!g_vistaLampIcon.compare_exchange_strong(expected, created,
                                                 std::memory_order_acq_rel)) {
        // Another thread won the race and published its icon first; drop ours.
        DestroyIcon(created);
        return expected;
    }
    return created;
}

HANDLE WINAPI LoadImageWHookForVistaLamp(HINSTANCE instance, LPCWSTR name, UINT type,
                                         int cx, int cy, UINT flags) {
    // Only intercept while the Vista skin is actually selected, and only for
    // requests that actually name shell32.dll (icon 61001 there is the "rate
    // this computer" lamp glyph we're overriding). Without both checks this
    // hijacked resource id 61001 process-wide for any module/skin.
    if (g_useWindowsVistaSkin.load() && type == IMAGE_ICON && IS_INTRESOURCE(name) &&
        static_cast<UINT>(reinterpret_cast<UINT_PTR>(name)) == kVistaLampIconId &&
        instance == GetModuleHandleW(L"shell32.dll")) {
        if (HICON icon = GetVistaLampIcon()) {
            return CopyIcon(icon);
        }
    }
    return LoadImageWOriginalForVistaLamp(instance, name, type, cx, cy, flags);
}


using LoadIconW_t = decltype(&LoadIconW);
using ExtractIconExW_t = decltype(&ExtractIconExW);
using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
using SHDefExtractIconW_t = HRESULT(WINAPI*)(LPCWSTR, int, UINT, HICON*, HICON*, UINT);
static LoadIconW_t LoadIconWOriginalForVistaLamp = nullptr;
static ExtractIconExW_t ExtractIconExWOriginalForVistaLamp = nullptr;
static PrivateExtractIconsW_t PrivateExtractIconsWOriginalForVistaLamp = nullptr;
static SHDefExtractIconW_t SHDefExtractIconWOriginalForVistaLamp = nullptr;

static bool IsVistaLampIconRequest(LPCWSTR file, int index) {
    if (!g_useWindowsVistaSkin.load()) return false;
    if (index != kVistaLampIconId && index != -kVistaLampIconId) return false;
    // Require the caller to actually name shell32.dll. Treating an empty/NULL
    // file as a match let any module in the process asking for icon 61001 by
    // ordinal (with no file specified) get the lamp bitmap instead of its own
    // icon.
    if (!file || !*file) return false;
    std::wstring s = file;
    for (auto& c : s) c = towlower(c);
    return s.find(L"shell32.dll") != std::wstring::npos;
}

HICON WINAPI LoadIconWHookForVistaLamp(HINSTANCE instance, LPCWSTR name) {
    if (g_useWindowsVistaSkin.load() && IS_INTRESOURCE(name) &&
        static_cast<UINT>(reinterpret_cast<UINT_PTR>(name)) == kVistaLampIconId &&
        instance == GetModuleHandleW(L"shell32.dll")) {
        if (HICON icon = GetVistaLampIcon()) return CopyIcon(icon);
    }
    return LoadIconWOriginalForVistaLamp(instance, name);
}

UINT WINAPI ExtractIconExWHookForVistaLamp(LPCWSTR file, int iconIndex,
                                           HICON* large, HICON* small,
                                           UINT icons) {
    if (IsVistaLampIconRequest(file, iconIndex) && icons > 0) {
        HICON icon = GetVistaLampIcon();
        if (!icon) return 0;
        if (large) large[0] = CopyIcon(icon);
        if (small) small[0] = CopyIcon(icon);
        return 1;
    }
    return ExtractIconExWOriginalForVistaLamp(file, iconIndex, large, small, icons);
}

UINT WINAPI PrivateExtractIconsWHookForVistaLamp(LPCWSTR file, int iconIndex,
                                                 int cx, int cy, HICON* icons,
                                                 UINT* ids, UINT iconCount,
                                                 UINT flags) {
    if (IsVistaLampIconRequest(file, iconIndex) && iconCount > 0 && icons) {
        HICON icon = GetVistaLampIcon();
        if (!icon) return 0;
        icons[0] = CopyIcon(icon);
        if (ids) ids[0] = kVistaLampIconId;
        return 1;
    }
    return PrivateExtractIconsWOriginalForVistaLamp(file, iconIndex, cx, cy, icons,
                                                    ids, iconCount, flags);
}

HRESULT WINAPI SHDefExtractIconWHookForVistaLamp(LPCWSTR file, int iconIndex,
                                                 UINT flags, HICON* large,
                                                 HICON* small, UINT size) {
    if (IsVistaLampIconRequest(file, iconIndex)) {
        HICON icon = GetVistaLampIcon();
        if (!icon) return E_FAIL;
        if (large) *large = CopyIcon(icon);
        if (small) *small = CopyIcon(icon);
        return S_OK;
    }
    return SHDefExtractIconWOriginalForVistaLamp(file, iconIndex, flags, large, small, size);
}

using ShellExecuteW_t = decltype(&ShellExecuteW);
using ShellExecuteExW_t = decltype(&ShellExecuteExW);
static ShellExecuteW_t ShellExecuteWOriginal = nullptr;
static ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;

static bool IsPerfCenterAction(PCWSTR params, const wchar_t* action) {
    if (!params || !action) return false;
    std::wstring value = params;
    for (auto& c : value) c = towlower(c);
    std::wstring expected = L"perfcenter-restorer:";
    expected += action;
    for (auto& c : expected) c = towlower(c);
    return value == expected;
}

static bool HandlePerfCenterAction(HWND hwnd, PCWSTR params) {
    MuiLanguage lang = GetCurrentEmbeddedLanguage();

    if (IsPerfCenterAction(params, L"numbers")) {
        MessageBoxW(hwnd, Win7NumbersDialogText(lang), Win7NumbersMeanText(lang),
                    MB_OK | MB_ICONINFORMATION);
        return true;
    }
    if (IsPerfCenterAction(params, L"tips")) {
        MessageBoxW(hwnd, Win7TipsDialogText(lang), Win7ImproveText(lang),
                    MB_OK | MB_ICONINFORMATION);
        return true;
    }
    if (IsPerfCenterAction(params, L"viewprint")) {
        if (ShellExecuteWOriginal) {
            ShellExecuteWOriginal(hwnd, L"open", L"msinfo32.exe", nullptr, nullptr, SW_SHOWNORMAL);
        }
        return true;
    }
    if (IsPerfCenterAction(params, L"learnmore")) {
        if (ShellExecuteWOriginal) {
            ShellExecuteWOriginal(hwnd, L"open",
                                  L"https://support.microsoft.com/en-us/windows/experience/performance-optimization/tips-to-improve-pc-performance-in-windows",
                                  nullptr, nullptr, SW_SHOWNORMAL);
        }
        return true;
    }
    return false;
}

HINSTANCE WINAPI ShellExecuteWHook(HWND hwnd, LPCWSTR operation, LPCWSTR file,
                                   LPCWSTR parameters, LPCWSTR directory,
                                   INT showCmd) {
    if (HandlePerfCenterAction(hwnd, parameters)) {
        return reinterpret_cast<HINSTANCE>(static_cast<INT_PTR>(33));
    }
    return ShellExecuteWOriginal(hwnd, operation, file, parameters, directory, showCmd);
}

BOOL WINAPI ShellExecuteExWHook(SHELLEXECUTEINFOW* execInfo) {
    if (execInfo && HandlePerfCenterAction(execInfo->hwnd, execInfo->lpParameters)) {
        execInfo->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<INT_PTR>(33));
        return TRUE;
    }
    return ShellExecuteExWOriginal(execInfo);
}

// Installs both the Vista-lamp icon hooks and the perf-center-action
// ShellExecute hooks. The icon hooks stay installed process-wide even when
// the Windows 8 (default) or Windows 7 skin is active, because the selected
// skin can change at runtime via Wh_ModSettingsChanged; each hook checks
// g_useWindowsVistaSkin (and, for icon 61001, that the caller actually named
// shell32.dll) before doing anything, so with the default skin they fall
// straight through to the original function.
static void InstallPerfCenterActionHooks() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (user32) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(user32, "LoadImageW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadImageW_t>(p),
                                           LoadImageWHookForVistaLamp,
                                           &LoadImageWOriginalForVistaLamp);
        }
        if (void* p = reinterpret_cast<void*>(GetProcAddress(user32, "LoadIconW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadIconW_t>(p),
                                           LoadIconWHookForVistaLamp,
                                           &LoadIconWOriginalForVistaLamp);
        }
        if (void* p = reinterpret_cast<void*>(GetProcAddress(user32, "PrivateExtractIconsW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<PrivateExtractIconsW_t>(p),
                                           PrivateExtractIconsWHookForVistaLamp,
                                           &PrivateExtractIconsWOriginalForVistaLamp);
        }
    }

    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) {
        shell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!shell32) return;
    if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ExtractIconExW"))) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<ExtractIconExW_t>(p),
                                       ExtractIconExWHookForVistaLamp,
                                       &ExtractIconExWOriginalForVistaLamp);
    }
    if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "SHDefExtractIconW"))) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<SHDefExtractIconW_t>(p),
                                       SHDefExtractIconWHookForVistaLamp,
                                       &SHDefExtractIconWOriginalForVistaLamp);
    }
    if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteW"))) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<ShellExecuteW_t>(p),
                                       ShellExecuteWHook, &ShellExecuteWOriginal);
    }
    if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteExW"))) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<ShellExecuteExW_t>(p),
                                       ShellExecuteExWHook, &ShellExecuteExWOriginal);
    }
}

static void InstallPerfCenterXmlPatchHook() {
    HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
    if (!dui70) {
        dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!dui70) {
        Wh_Log(L"PerfCenter XML patch: dui70.dll could not be loaded");
        return;
    }

    for (const char* name : {
#ifdef _WIN64
             "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",
#endif
             "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z"}) {
        if (FARPROC proc = GetProcAddress(dui70, name)) {
            DUISetXML = reinterpret_cast<DUISetXML_t>(proc);
            break;
        }
    }
    if (!DUISetXML) {
        Wh_Log(L"PerfCenter XML patch: DirectUI::DUIXmlParser::SetXML not found");
        return;
    }

    for (const char* name : {
#ifdef _WIN64
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z",
#endif
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z"}) {
        if (FARPROC proc = GetProcAddress(dui70, name)) {
            WindhawkUtils::SetFunctionHook(
                reinterpret_cast<DUISetXMLFromResource_t>(proc),
                DUISetXMLFromResourceHook, &DUISetXMLFromResourceOriginal);
            break;
        }
    }

    if (!DUISetXMLFromResourceOriginal) {
        Wh_Log(L"PerfCenter XML patch: DirectUI::_SetXMLFromResource hook failed");
    }
}

void InstallTranslationHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32)
        user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    void* loadStringW =
        user32 ? reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringW")) : nullptr;
    void* loadStringA =
        user32 ? reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringA")) : nullptr;
    if (loadStringW)
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(loadStringW),
                                       LoadStringWHook, &LoadStringWOriginal);
    if (loadStringA)
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringA_t>(loadStringA),
                                       LoadStringAHook, &LoadStringAOriginal);

    // dui70.dll is not a KnownDLL, so restrict the search to System32.
    HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
    if (!dui70)
        dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    void* xResourceProviderCreate =
        dui70 ? reinterpret_cast<void*>(
                    GetProcAddress(
                        dui70,
                        "?Create@XResourceProvider@DirectUI@@SAJPEAUHINSTANCE__@@PEBG11PEAPEAV12@@Z"))
              : nullptr;
    if (xResourceProviderCreate)
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<XResourceProviderCreate_t>(xResourceProviderCreate),
            XResourceProviderCreateHook, &XResourceProviderCreateOriginal);

    InstallPerfCenterXmlPatchHook();
    InstallPerfCenterActionHooks();
}

// -----------------------------------------------------------------------------
// COM hooks
// -----------------------------------------------------------------------------
using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID,
                                            LPVOID*);
CoCreateInstance_t CoCreateInstanceOriginalCombase = nullptr;
CoCreateInstance_t CoCreateInstanceOriginalOle32 = nullptr;

static HRESULT HandleCoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                      DWORD dwClsCtx, REFIID riid, LPVOID* ppv,
                                      CoCreateInstance_t original) {
    if (ppv) *ppv = nullptr;
    const bool isProvider = IsEqualGUID(rclsid, kProviderGuid);
    const bool isFolder = IsEqualGUID(rclsid, kAppletFolderGuid);
    if (isProvider || isFolder) {
        if (!g_dllVerifiedOk.load()) return REGDB_E_CLASSNOTREG;
        HMODULE h = nullptr;
        if (isProvider) {
            // Reuse the reference the setup thread took; do not add another one.
            h = g_hPerfCenter.load();
        } else {
            // The applet's shell folder comes from shdocvw.dll. Resolve it
            // without creating new references when it is already mapped.
            h = GetModuleHandleW(L"shdocvw.dll");
            if (!h)
                h = LoadLibraryExW(L"shdocvw.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        if (!h) return REGDB_E_CLASSNOTREG;
        auto pDllGetClassObject = reinterpret_cast<HRESULT(WINAPI*)(REFCLSID, REFIID,
                                                                   LPVOID*)>(
            GetProcAddress(h, "DllGetClassObject"));
        if (!pDllGetClassObject) return REGDB_E_CLASSNOTREG;
        IClassFactory* cf = nullptr;
        HRESULT hr = pDllGetClassObject(rclsid, IID_IClassFactory_GUID,
                                        reinterpret_cast<LPVOID*>(&cf));
        if (FAILED(hr)) return hr;
        hr = cf->CreateInstance(pUnkOuter, riid, ppv);
        cf->Release();
        return hr;
    }
    return original(rclsid, pUnkOuter, dwClsCtx, riid, ppv);
}

HRESULT WINAPI CoCreateInstanceHookCombase(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                           DWORD dwClsCtx, REFIID riid, LPVOID* ppv) {
    return HandleCoCreateInstance(rclsid, pUnkOuter, dwClsCtx, riid, ppv,
                                  CoCreateInstanceOriginalCombase);
}

HRESULT WINAPI CoCreateInstanceHookOle32(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                         DWORD dwClsCtx, REFIID riid, LPVOID* ppv) {
    return HandleCoCreateInstance(rclsid, pUnkOuter, dwClsCtx, riid, ppv,
                                  CoCreateInstanceOriginalOle32);
}

void InstallComHook() {
    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase)
        combase = LoadLibraryExW(L"combase.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
    if (!ole32)
        ole32 = LoadLibraryExW(L"ole32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    void* pCombase =
        combase ? reinterpret_cast<void*>(GetProcAddress(combase, "CoCreateInstance"))
                : nullptr;
    void* pOle32 =
        ole32 ? reinterpret_cast<void*>(GetProcAddress(ole32, "CoCreateInstance"))
              : nullptr;
    // Hook the real implementation (combase). Each target gets its own hook
    // function and its own original pointer so calls never cross between them.
    if (pCombase) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<CoCreateInstance_t>(pCombase),
                                       CoCreateInstanceHookCombase,
                                       &CoCreateInstanceOriginalCombase);
    }
    if (pOle32 && pOle32 != pCombase) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<CoCreateInstance_t>(pOle32),
                                       CoCreateInstanceHookOle32,
                                       &CoCreateInstanceOriginalOle32);
    }
}

// -----------------------------------------------------------------------------
// Windhawk entry points
// -----------------------------------------------------------------------------
BOOL Wh_ModInit(void) {
    try {
        // The payload DLL is x64-only. On ARM64, Windhawk may inject this mod
        // into the native ARM64 explorer.exe (a predefined shell process);
        // refuse to start BEFORE installing any hook so an ARM64 shell never
        // pays the registry/COM/LoadString hook cost for a mod that can never
        // do anything there.
        if (!IsRunningAsAmd64()) {
            Wh_Log(L"PerfCenterCPL mod not started: system architecture is not "
                   L"AMD64 (x64)");
            return FALSE;
        }

        InitClsidStrings();
        g_forceTranslations.store(Wh_GetIntSetting(L"forceTranslations") != 0);
        LoadLanguageSetting();
        // Load the skin setting during startup before DirectUI hooks are installed,
        // so pageSkin=win7 is active for the very first page load after enabling
        // the mod. The XML hook also re-reads the setting when a fresh page is
        // created, which covers settings changed while the page was open.
        LoadPageSkinSetting();

        // --- Install conservative registry hooks (Unicode *W only: 9 hooks) ---
        void* pOpen = GetRegFunc("RegOpenKeyExW");
        void* pOpenOldW = GetRegFunc("RegOpenKeyW");
        void* pCreateW = GetRegFunc("RegCreateKeyExW");
        void* pClose = GetRegFunc("RegCloseKey");
        void* pQV = GetRegFunc("RegQueryValueExW");
        void* pGV = GetRegFunc("RegGetValueW");
        void* pEnumEx = GetRegFunc("RegEnumKeyExW");
        void* pEnum = GetRegFunc("RegEnumKeyW");
        void* pQInfo = GetRegFunc("RegQueryInfoKeyW");

        if (!pOpen || !pClose || !pQV || !pGV || !pEnumEx || !pEnum || !pQInfo) {
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyExW_t>(pOpen),
                                       RegOpenKeyExWHook, &RegOpenKeyExWOriginal);
        if (pOpenOldW)
            WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyW_t>(pOpenOldW),
                                           RegOpenKeyWHook, &RegOpenKeyWOriginal);
        if (pCreateW)
            WindhawkUtils::SetFunctionHook(reinterpret_cast<RegCreateKeyExW_t>(pCreateW),
                                           RegCreateKeyExWHook,
                                           &RegCreateKeyExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegCloseKey_t>(pClose),
                                       RegCloseKeyHook, &RegCloseKeyOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegQueryValueExW_t>(pQV),
                                       RegQueryValueExWHook, &RegQueryValueExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegGetValueW_t>(pGV),
                                       RegGetValueWHook, &RegGetValueWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegEnumKeyExW_t>(pEnumEx),
                                       RegEnumKeyExWHook, &RegEnumKeyExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegEnumKeyW_t>(pEnum),
                                       RegEnumKeyWHook, &RegEnumKeyWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegQueryInfoKeyW_t>(pQInfo),
                                       RegQueryInfoKeyWHook, &RegQueryInfoKeyWOriginal);

        InstallComHook();
        InstallTranslationHook();

        if (!g_stopEvent) {
            g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        } else {
            ResetEvent(g_stopEvent);
        }
        g_shuttingDown.store(false, std::memory_order_release);

        // --- Start async setup on a worker thread (never blocks startup) ---
        try {
            g_setupThread.emplace(RunSetup);
        } catch (...) {
            // Thread creation failed; the mod just runs without the DLL.
        }
        return TRUE;
    } catch (...) {
        return FALSE;
    }
}

void Wh_ModAfterInit(void) {
    Wh_Log(L"Conservative string hooks are active; PerfCenter DLL loads in the "
           L"background; startup page skin: %s",
           CurrentPageSkinName());
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    try {
        const bool oldWindows7Skin = g_useWindows7Skin.load();
        const bool oldWindowsVistaSkin = g_useWindowsVistaSkin.load();
        const bool requestedVistaSkin = ReadPageSkinVistaSetting();
        const bool requestedWindows7Skin = !requestedVistaSkin && ReadPageSkinSetting();

        // Language/translation settings are handled in place - no reload is
        // needed, because GetLocalizedDisplayName, GetLocalizedInfoTip and
        // LoadStringWHook all read the language live. Rebuild the private
        // resource module so pages opened from now on get the new language:
        // the old module is released (kept mapped for pages that are already
        // open) and the new build gets a fresh, never-before-used file name.
        g_forceTranslations.store(Wh_GetIntSetting(L"forceTranslations") != 0);
        LoadLanguageSetting();
        const std::wstring* dll = CurrentDllPath();
        if (dll && !dll->empty()) {
            std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
            if (!LocalizedResourceModuleUpToDateLocked()) {
                ReleaseLocalizedResourceModuleLocked();
                BuildLocalizedResourceModule(*dll, StoreDir());
            }
        }

        if (oldWindows7Skin != requestedWindows7Skin ||
            oldWindowsVistaSkin != requestedVistaSkin) {
            // Defensive runtime behavior: changing the DirectUI XML while the
            // Control Panel page is already open can leave a half-patched tree.
            // Apply the skin switch immediately only when the page isn't open;
            // otherwise keep the active skin until the user closes/reopens the page.
            if (IsPerfCenterControlPanelWindowOpen()) {
                Wh_Log(L"Page skin setting changed, but Performance Information "
                       L"and Tools is open; close and reopen the page to apply it");
                if (reload) *reload = FALSE;
                return TRUE;
            }

            g_useWindowsVistaSkin.store(requestedVistaSkin);
            g_useWindows7Skin.store(requestedWindows7Skin);
            g_startupRequestedWindowsVistaSkin.store(requestedVistaSkin);
            g_startupRequestedWindows7Skin.store(requestedWindows7Skin);
            Wh_Log(L"Page skin changed at runtime to %s", CurrentPageSkinName());
            if (reload) *reload = FALSE;
            return TRUE;
        }

        // Every setting is now applied in place, so the mod never needs to
        // reload (a reload would tear down and re-run setup, and would fail to
        // rebuild the resource module anyway while the old one is still mapped).
        if (reload) *reload = FALSE;
        return TRUE;
    } catch (...) {
        if (reload) *reload = TRUE;
        return TRUE;
    }
}


void Wh_ModUninit(void) {
    try {
        // Ask the background setup (and any in-flight download) to stop, then
        // wait for it to finish before tearing anything down. The download loop
        // checks this flag and the WinInet timeouts bound each blocking read, so
        // the join returns promptly instead of hanging on a stuck connection.
        g_shuttingDown.store(true, std::memory_order_release);
        if (g_stopEvent) {
            SetEvent(g_stopEvent);
        }
        // InternetOpenUrlW/InternetQueryDataAvailable/InternetReadFile are not
        // interruptible and only bound each individual call to
        // kDownloadTimeoutMs (20s), across up to kMaxDownloadAttempts retries -
        // g_shuttingDown is only checked between calls, so a stuck/captive-
        // portal connection could otherwise stall this join for a long time.
        // Closing the handles from here makes any currently-blocked WinInet
        // call fail immediately instead of waiting out its timeout.
        CancelInFlightDownload();
        if (g_setupThread && g_setupThread->joinable()) {
            g_setupThread->join();
        }
        g_setupThread.reset();
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }

        // The master Vista-lamp icon is safe to destroy here even though
        // CopyIcon() results already handed out to callers are independent
        // handles unaffected by this. Without it, one HICON leaked in
        // explorer.exe on every enable/disable cycle.
        if (HICON lamp = g_vistaLampIcon.exchange(nullptr)) {
            DestroyIcon(lamp);
        }

        // Do not force-unload g_hPerfCenter or g_hLocalizedResources on teardown,
        // because live COM objects or DirectUI::XResourceProvider may still reference
        // their vtables and resources.
        ReleaseLocalizedResourceModule();
        g_hPerfCenter.store(nullptr);
        g_dllVerifiedOk.store(false);
        const std::wstring* path = g_dllPath.exchange(nullptr);
        delete path;

        if (Wh_GetIntSetting(L"keepFilesOnDisable") == 0) {
            std::wstring dir = StoreDir();
            if (!dir.empty()) RemoveOwnFiles(dir, false);
            Wh_Log(L"Mod disabled: mod-owned files removed (files still mapped "
                   L"are retried on a later unload)");
        } else {
            std::wstring dir = StoreDir();
            if (!dir.empty()) RemoveOwnFiles(dir, true);
            Wh_Log(L"Mod disabled: base files kept; private resource module and "
                   L"stale copies removed (files still mapped are retried on a "
                   L"later unload)");
        }
        g_keyTracker.ClearWithoutFreeing();
    } catch (...) {
    }
}
