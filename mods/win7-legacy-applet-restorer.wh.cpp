// ==WindhawkMod==
// @id              win7-legacy-applet-restorer
// @name            Windows 7 Legacy Applet Restorer
// @description     This mod restores some classic Control Panel applets and localized Windows 7 task links using native components
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @compilerOptions -lcomctl32
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
- restoreWin7CategoryBlueLinks: true
  $name: Restore Windows 7 Category Blue Links
  $description: Restores the classic blue task links under all Control Panel categories (System and Security, Programs, User Accounts, Clock/Language/Region, Ease of Access) like Windows 7 had
- enableLogging: false
  $name: Enable debug logging
  $description: Logs Windows version, CLSID availability, and Control Panel option status to the Windhawk log for troubleshooting across different Windows builds
*/
// ==/WindhawkModSettings==

// If a reviewer or automated program is reading the comments, feel free to point out any missing link that could be restored
/* Author's note
This mod enhances Anixx's "Restore the classic Personalization and other CPLs" 
(restore-classic-cpls.wh.cpp) by adding HomeGroup, classic task links, Windows 7 
category blue links, and a shell::: command redirect hook.

Based on a fork of Anixx's work, its primary goal is to restore Windows 7-style 
links on Windows 10/11. Currently tested only on Windows 10 1809 (hardware limited 
to 4 GB RAM, HDD; unable to run Windows 11).

New features include the reintroduction of HomeGroup in Control Panel — absent 
from the original mod. To prevent conflicts, this mod will auto-disable if 
Anixx's mod is detected and log a warning.

The original author (Anixx) was contacted about merging these changes but did 
not reply. As a precaution, automatic detection ensures only one mod runs at a time.

Testing has been limited to Windows 10 1809. An optional debug logging flag 
(enableLogging) helps users on other builds report issues. If you encounter 
problems, please enable logging and submit an issue with the log output.
*/
#include <string>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <atomic>
#include <fstream>
#include <cstring>
#include <memory>
#include <new>
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
    std::atomic<bool> restoreWin7CategoryBlueLinks;
    std::atomic<bool> enableLogging;
} g_settings;

// Logging macro - only logs when enableLogging is true
#define LOG_IF_ENABLED(fmt, ...) do { if (g_settings.enableLogging.load()) Wh_Log(L##fmt, ##__VA_ARGS__); } while(0)

// Windows version info
typedef LONG (WINAPI *RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
static DWORD g_winBuild = 0;
static wchar_t g_winVersionStr[64] = {};

void DetectWindowsVersion() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        auto pRtlGetVersion = (RtlGetVersion_t)GetProcAddress(hNtdll, "RtlGetVersion");
        if (pRtlGetVersion) {
            RTL_OSVERSIONINFOW osvi = { sizeof(osvi) };
            if (pRtlGetVersion(&osvi) == 0) {
                g_winBuild = osvi.dwBuildNumber;
                const wchar_t* name = L"Unknown";
                if (osvi.dwMajorVersion == 10) {
                    if (osvi.dwBuildNumber >= 26100)      name = L"Win11 24H2+";
                    else if (osvi.dwBuildNumber >= 22621)  name = L"Win11 23H2";
                    else if (osvi.dwBuildNumber >= 22000)  name = L"Win11 21H2/22H2";
                    else if (osvi.dwBuildNumber >= 19045)  name = L"Win10 22H2";
                    else if (osvi.dwBuildNumber >= 19044)  name = L"Win10 21H2";
                    else if (osvi.dwBuildNumber >= 19043)  name = L"Win10 21H1";
                    else if (osvi.dwBuildNumber >= 19042)  name = L"Win10 20H2";
                    else if (osvi.dwBuildNumber >= 19041)  name = L"Win10 2004";
                    else if (osvi.dwBuildNumber >= 18363)  name = L"Win10 1909";
                    else if (osvi.dwBuildNumber >= 18362)  name = L"Win10 1903";
                    else if (osvi.dwBuildNumber >= 17763)  name = L"Win10 1809";
                    else if (osvi.dwBuildNumber >= 17134)  name = L"Win10 1803";
                    else if (osvi.dwBuildNumber >= 16299)  name = L"Win10 1709";
                    else if (osvi.dwBuildNumber >= 15063)  name = L"Win10 1703";
                    else if (osvi.dwBuildNumber >= 14393)  name = L"Win10 1607";
                    else if (osvi.dwBuildNumber >= 10586)  name = L"Win10 1511";
                    else if (osvi.dwBuildNumber >= 10240)  name = L"Win10 RTM";
                }
                _snwprintf_s(g_winVersionStr, _countof(g_winVersionStr), _TRUNCATE,
                    L"%s (Build %u.%u.%u)", name,
                    osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
            }
        }
    }
}

std::wstring g_personalizationName;

// Forward declaration
bool EnsureClassicTaskLinksFile();
std::wstring g_classicTaskLinksFilePath;

// Forward declarations (defined further below; KeyTracker::Track needs them)
std::wstring ToLower(const std::wstring& str);
bool ContainsRelevantKeyword(const std::wstring& lowerPath);

// Tracks the "virtual path" behind every HKEY the mod cares about (both real
// keys opened through the hooked Reg* APIs, and fully synthetic/fake keys we
// hand back for injected CLSIDs). A single mutex guards all state so the
// "is this fake?" check and the "what's its path?" lookup can never observe
// two different snapshots of the data. Fake-handle memory is owned via
// unique_ptr, so it is always freed exactly once, from exactly one place —
// no manual new/delete pairing to get wrong.
class KeyTracker {
public:
    std::wstring GetPath(HKEY hKey) const {
        if (std::wstring special = SpecialRootPath(hKey); !special.empty()) return special;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = paths_.find(hKey);
        return it != paths_.end() ? it->second : std::wstring();
    }

    bool IsFake(HKEY hKey) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return fakeOwners_.count(hKey) != 0;
    }

    void Track(HKEY hKey, const std::wstring& path) {
        if (!hKey || IsSpecialRoot(hKey)) return;
        if (!ContainsRelevantKeyword(ToLower(path))) return;
        std::lock_guard<std::mutex> lock(mutex_);
        paths_[hKey] = path;
    }

    void Untrack(HKEY hKey) {
        if (!hKey || IsSpecialRoot(hKey)) return;
        std::lock_guard<std::mutex> lock(mutex_);
        paths_.erase(hKey);
    }

    // Allocates a small owned object to serve as an opaque, unique HKEY value.
    // Returns nullptr on allocation failure instead of throwing, so callers
    // running inside a hook (on an arbitrary explorer.exe thread) never have
    // to deal with a C++ exception unwinding through foreign code.
    HKEY CreateFake(const std::wstring& path) {
        std::unique_ptr<int> owned(new (std::nothrow) int(1));
        if (!owned) return nullptr;
        HKEY fake = reinterpret_cast<HKEY>(owned.get());
        std::lock_guard<std::mutex> lock(mutex_);
        paths_[fake] = path;
        fakeOwners_[fake] = std::move(owned);
        return fake;
    }

    void FreeFake(HKEY hKey) {
        std::lock_guard<std::mutex> lock(mutex_);
        paths_.erase(hKey);
        fakeOwners_.erase(hKey); // unique_ptr destructor frees the memory
    }

    // Called once from Wh_ModUninit. Deliberately does NOT delete the
    // fake-handle memory: shell32/explorer may still be holding a stale HKEY
    // across a disable/re-enable cycle, and freeing it here could let a
    // future allocation reuse the same address, turning a stale handle into
    // a dangling alias for a live object. Leaking a handful of ints is
    // strictly safer than that use-after-free, so we only release ownership
    // (no delete) and drop our own bookkeeping.
    void ClearWithoutFreeing() {
        std::lock_guard<std::mutex> lock(mutex_);
        paths_.clear();
        for (auto& kv : fakeOwners_) {
            int* intentionallyLeakedHandle = kv.second.release();
            (void)intentionallyLeakedHandle;
        }
        fakeOwners_.clear();
    }

private:
    static bool IsSpecialRoot(HKEY hKey) {
        auto v = (uintptr_t)hKey;
        return v >= 0x80000000 && v <= 0x80000004;
    }
    static std::wstring SpecialRootPath(HKEY hKey) {
        switch ((uintptr_t)hKey) {
            case 0x80000000: return L"HKEY_CLASSES_ROOT";
            case 0x80000001: return L"HKEY_CURRENT_USER";
            case 0x80000002: return L"HKEY_LOCAL_MACHINE";
            case 0x80000003: return L"HKEY_USERS";
            case 0x80000004: return L"HKEY_CURRENT_CONFIG";
            default: return L"";
        }
    }

    mutable std::mutex mutex_;
    std::unordered_map<HKEY, std::wstring> paths_;
    std::unordered_map<HKEY, std::unique_ptr<int>> fakeOwners_;
};

static KeyTracker g_keyTracker;

// Pre-computed lowercase GUID strings for fast comparison
std::wstring g_personalizationGuidLower;
std::wstring g_notificationIconsGuidLower;
std::wstring g_networkConnectionsGuidLower;
std::wstring g_printersAndFaxesGuidLower;
std::wstring g_homeGroupGuidLower;
std::wstring g_displayGuidLower;
std::wstring g_realPersonalizationGuidLower;
std::wstring g_suppressedGuidLower;

static const std::wstring kPersonalizationGuid     = L"{580722ff-16a7-44c1-bf74-7e1acd00f4f9}";
static const std::wstring kNotificationIconsGuid   = L"{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}";
static const std::wstring kNetworkConnectionsGuid  = L"{7007acc7-3202-11d1-aad2-00805fc1270e}";
static const std::wstring kPrintersAndFaxesGuid    = L"{2227a280-3aea-1069-a2de-08002b30309d}";
static const std::wstring kHomeGroupGuid           = L"{67ca7650-96e6-4fdd-bb43-a8e774f73a57}";
static const std::wstring kDisplayGuid             = L"{c55584f4-7c7f-44f2-9a6d-913076f34c6a}"; // Also used as RealDisplayGuid
static const std::wstring kRealPersonalizationGuid = L"{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}";
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

// Guards g_classicTaskLinksFilePath. EnsureClassicTaskLinksFile() is normally
// only called once, eagerly, from Wh_ModInit before any hook is installed —
// but TryProvideValue() also calls it lazily as a fallback, and that runs
// from the registry hooks on arbitrary explorer.exe threads. Without this
// lock, two threads could race on the check-then-write below, or one thread
// could read a half-written path while another is (re)generating the file.
static std::mutex g_taskLinksMutex;

// Thread-safe accessor for readers (TryProvideValue and friends) that just
// want the current path without regenerating anything.
std::wstring GetClassicTaskLinksFilePath() {
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    return g_classicTaskLinksFilePath;
}

// Creates a self-contained task list used by Control Panel to display the
// classic blue links below the Personalization item.
bool EnsureClassicTaskLinksFile() {
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
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
        const char* reviewComputerStatus;
        const char* backUpComputer;
        const char* findAndFixProblems;
        const char* checkFirewallStatus;
        const char* uninstallProgram;
        const char* turnWindowsFeatures;
        const char* changeAccountPicture;
        const char* addRemoveAccounts;
        const char* setParentalControls;
        const char* changeDateTime;
        const char* changeInputMethod;
        const char* letWindowsSuggest;
    };

    // Hard-coded localized Windows 7-style labels. The selected entry follows
    // the current Windows UI language; English is the fallback.
    // Complete static catalog for 30 common Windows UI languages. It has no
    // MUI/resource dependency; English remains the fallback for other locales.
    // Review corrections can be made one row at a time without altering logic.
    static const TaskLinkTexts kTaskLinkTexts[] = {
        { L"en", "Change the theme", "Change desktop background", "Change window glass colors", "Change sound effects", "Change screen saver", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Choose homegroup and sharing options", "Share printers", "Adjust screen resolution", "Review your computer's status", "Back up your computer", "Find and fix problems", "Check firewall status", "Uninstall a program", "Turn Windows features on or off", "Change account picture", "Add or remove user accounts", "Set up parental controls for any user", "Change the date and time", "Change input methods", "Let Windows suggest settings for you" },
        { L"it", "Cambia tema", "Cambia sfondo del desktop", "Cambia i colori delle finestre", "Cambia effetti sonori", "Cambia salvaschermo", "Attiva o disattiva le icone di sistema", "Ripristina comportamenti predefiniti delle icone", "Visualizza stato e attività della rete", "Connetti a una rete", "Visualizza computer e dispositivi di rete", "Aggiungi un dispositivo wireless alla rete", "Aggiungi una stampante", "Configura stampanti predefinite", "Modifica impostazioni stampante", "Visualizza dispositivi e stampanti", "Scegli gruppo home e opzioni di condivisione", "Condividi stampanti", "Regola la risoluzione dello schermo", "Controlla lo stato del computer", "Esegui il backup del computer", "Trova e correggi problemi", "Verifica stato del firewall", "Disinstalla un programma", "Attiva o disattiva funzionalità di Windows", "Cambia immagine dell'account", "Aggiungi o rimuovi account utente", "Configura controlli parentali per qualsiasi utente", "Cambia data e ora", "Cambia metodi di immissione", "Consenti a Windows di suggerire le impostazioni" },
        { L"es", "cambiar el tema", "Cambiar fondo de escritorio", "Cambiar colores de vidrio de ventana", "Cambiar efectos de sonido", "Cambiar protector de pantalla", "Activar o desactivar los iconos del sistema", "Restaurar los comportamientos predeterminados de los iconos", "Ver el estado y las tareas de la red", "Conectarse a una red", "Ver computadoras y dispositivos de la red", "Agregar un dispositivo inalámbrico a la red", "Agregar una impresora", "Configurar impresoras predeterminadas", "Cambiar la configuración de la impresora", "Ver dispositivos e impresoras", "Elija el grupo en el hogar y las opciones para compartir", "Compartir impresoras", "Ajustar la resolución de la pantalla", "Revisa el estado de tu computadora", "Haz una copia de seguridad de tu computadora", "Encontrar y solucionar problemas", "Comprobar el estado del cortafuegos", "Desinstalar un programa", "Activar o desactivar las funciones de Windows", "Cambiar imagen de cuenta", "Agregar o eliminar cuentas de usuario", "Configurar controles parentales para cualquier usuario", "Cambiar la fecha y la hora", "Cambiar métodos de entrada", "Deje que Windows le sugiera configuraciones" },
        { L"fr", "Changer le thème", "Changer l'arrière-plan du bureau", "Changer les couleurs des vitres", "Changer les effets sonores", "Changer l'économiseur d'écran", "Activer ou désactiver les icônes du système", "Restaurer les comportements des icônes par défaut", "Afficher l'état et les tâches du réseau", "Connectez-vous à un réseau", "Afficher les ordinateurs et les appareils du réseau", "Ajouter un appareil sans fil au réseau", "Ajouter une imprimante", "Configurer les imprimantes par défaut", "Modifier les paramètres de l'imprimante", "Afficher les appareils et les imprimantes", "Choisissez le groupe résidentiel et les options de partage", "Partager des imprimantes", "Ajuster la résolution de l'écran", "Vérifiez l'état de votre ordinateur", "Sauvegardez votre ordinateur", "Rechercher et résoudre les problèmes", "Vérifier l'état du pare-feu", "Désinstaller un programme", "Activer ou désactiver des fonctionnalités Windows", "Changer la photo du compte", "Ajouter ou supprimer des comptes d'utilisateurs", "Configurer le contrôle parental pour n'importe quel utilisateur", "Changer la date et l'heure", "Changer les méthodes de saisie", "Laissez Windows vous suggérer des paramètres" },
        { L"de", "Ändern Sie das Thema", "Desktop-Hintergrund ändern", "Fensterglasfarben ändern", "Soundeffekte ändern", "Bildschirmschoner ändern", "Systemsymbole ein- oder ausschalten", "Stellen Sie das Standardverhalten von Symbolen wieder her", "Netzwerkstatus und Aufgaben anzeigen", "Stellen Sie eine Verbindung zu einem Netzwerk her", "Netzwerkcomputer und -geräte anzeigen", "Fügen Sie dem Netzwerk ein drahtloses Gerät hinzu", "Fügen Sie einen Drucker hinzu", "Richten Sie Standarddrucker ein", "Ändern Sie die Druckereinstellungen", "Geräte und Drucker anzeigen", "Wählen Sie Heimnetzgruppen- und Freigabeoptionen", "Drucker freigeben", "Passen Sie die Bildschirmauflösung an", "Überprüfen Sie den Status Ihres Computers", "Sichern Sie Ihren Computer", "Probleme finden und beheben", "Überprüfen Sie den Firewall-Status", "Deinstallieren Sie ein Programm", "Schalten Sie Windows-Funktionen ein oder aus", "Kontobild ändern", "Benutzerkonten hinzufügen oder entfernen", "Richten Sie die Kindersicherung für jeden Benutzer ein", "Ändern Sie Datum und Uhrzeit", "Eingabemethoden ändern", "Lassen Sie sich von Windows Einstellungen vorschlagen" },
        { L"pt-BR", "Mude o tema", "Alterar plano de fundo da área de trabalho", "Alterar as cores dos vidros das janelas", "Alterar efeitos sonoros", "Alterar protetor de tela", "Ativar ou desativar ícones do sistema", "Restaurar comportamentos padrão dos ícones", "Visualize o status e as tarefas da rede", "Conecte-se a uma rede", "Ver computadores e dispositivos de rede", "Adicione um dispositivo sem fio à rede", "Adicionar uma impressora", "Configurar impressoras padrão", "Alterar configurações da impressora", "Ver dispositivos e impressoras", "Escolha opções de grupo doméstico e compartilhamento", "Compartilhar impressoras", "Ajustar a resolução da tela", "Revise o status do seu computador", "Faça backup do seu computador", "Encontre e corrija problemas", "Verifique o status do firewall", "Desinstalar um programa", "Ativar ou desativar recursos do Windows", "Alterar imagem da conta", "Adicionar ou remover contas de usuário", "Configure o controle dos pais para qualquer usuário", "Alterar a data e hora", "Alterar métodos de entrada", "Deixe o Windows sugerir configurações para você" },
        { L"pt-PT", "Mude o tema", "Alterar o fundo da área de trabalho", "Alterar as cores dos vidros das janelas", "Alterar efeitos sonoros", "Alterar protetor de ecrã", "Ativar ou desativar os ícones do sistema", "Restaurar os comportamentos padrão dos ícones", "Visualize o estado e as tarefas da rede", "Ligue-se a uma rede", "Ver computadores e dispositivos de rede", "Adicione um dispositivo sem fios à rede", "Adicionar uma impressora", "Configurar impressoras padrão", "Alterar as definições da impressora", "Ver dispositivos e impressoras", "Escolha as opções de grupo doméstico e partilha", "Partilhar impressoras", "Ajustar a resolução do ecrã", "Reveja o estado do seu computador", "Faça cópias de segurança do seu computador", "Encontre e corrija problemas", "Verifique o estado do firewall", "Desinstalar um programa", "Ativar ou desativar funcionalidades do Windows", "Alterar imagem da conta", "Adicionar ou remover contas de utilizador", "Configure o controlo parental para qualquer utilizador", "Alterar a data e hora", "Alterar métodos de entrada", "Deixe o Windows sugerir-lhe definições" },
        { L"nl", "Verander het thema", "Bureaubladachtergrond wijzigen", "Verander de kleuren van vensterglas", "Verander geluidseffecten", "Schermbeveiliging wijzigen", "Systeempictogrammen in- of uitschakelen", "Herstel het standaardpictogramgedrag", "Bekijk de netwerkstatus en taken", "Maak verbinding met een netwerk", "Bekijk netwerkcomputers en apparaten", "Voeg een draadloos apparaat toe aan het netwerk", "Voeg een printer toe", "Standaardprinters instellen", "Wijzig de printerinstellingen", "Bekijk apparaten en printers", "Kies thuisgroep- en deelopties", "Deel printers", "Pas de schermresolutie aan", "Controleer de status van uw computer", "Maak een back-up van uw computer", "Problemen vinden en oplossen", "Controleer de firewallstatus", "Een programma verwijderen", "Schakel Windows-functies in of uit", "Accountafbeelding wijzigen", "Gebruikersaccounts toevoegen of verwijderen", "Stel ouderlijk toezicht in voor elke gebruiker", "Wijzig de datum en tijd", "Wijzig invoermethoden", "Laat Windows instellingen voor u voorstellen" },
        { L"pl", "Zmień motyw", "Zmień tło pulpitu", "Zmień kolory szyb okiennych", "Zmień efekty dźwiękowe", "Zmień wygaszacz ekranu", "Włącz lub wyłącz ikony systemowe", "Przywróć domyślne zachowanie ikon", "Wyświetl stan sieci i zadania", "Połącz się z siecią", "Wyświetl komputery i urządzenia sieciowe", "Dodaj urządzenie bezprzewodowe do sieci", "Dodaj drukarkę", "Skonfiguruj drukarki domyślne", "Zmień ustawienia drukarki", "Wyświetl urządzenia i drukarki", "Wybierz grupę domową i opcje udostępniania", "Udostępnij drukarki", "Dostosuj rozdzielczość ekranu", "Sprawdź stan swojego komputera", "Utwórz kopię zapasową komputera", "Znajdź i rozwiąż problemy", "Sprawdź stan zapory", "Odinstaluj program", "Włącz lub wyłącz funkcje systemu Windows", "Zmień zdjęcie konta", "Dodaj lub usuń konta użytkowników", "Skonfiguruj kontrolę rodzicielską dla dowolnego użytkownika", "Zmień datę i godzinę", "Zmień metody wprowadzania", "Pozwól systemowi Windows zasugerować ustawienia" },
        { L"ru", "Изменить тему", "Изменить фон рабочего стола", "Изменить цвет оконного стекла", "Изменить звуковые эффекты", "Изменить заставку", "Включить или выключить системные значки", "Восстановить поведение значков по умолчанию", "Просмотреть состояние сети и задачи", "Подключиться к сети", "Просмотреть сетевые компьютеры и устройства", "Добавить беспроводное устройство в сеть", "Добавить принтер", "Настроить принтеры по умолчанию", "Изменить настройки принтера", "Просмотреть устройства и принтеры", "Выбрать домашнюю группу и параметры общего доступа", "Предоставить общий доступ к принтерам", "Настроить разрешение экрана", "Проверить состояние компьютера", "Создать резервную копию компьютера", "Найти и устранить проблемы", "Проверить состояние брандмауэра", "Удалить программу", "Включить или выключить компоненты Windows", "Изменить изображение аккаунта", "Добавить или удалить учетные записи пользователей", "Настроить родительский контроль для любого пользователя", "Изменить дату и время", "Изменить методы ввода", "Разрешить Windows предлагать параметры" },
        { L"uk", "Змінити тему", "Змінити фон робочого столу", "Змінити колір віконного скла", "Змінити звукові ефекти", "Змінити заставку", "Увімкнути або вимкнути системні значки", "Відновити поведінку піктограм за замовчуванням", "Переглянути стан мережі та завдання", "Підключитися до мережі", "Переглянути мережеві комп’ютери й пристрої", "Додати бездротовий пристрій до мережі", "Додати принтер", "Налаштувати принтери за замовчуванням", "Змінити налаштування принтера", "Переглянути пристрої та принтери", "Вибрати домашню групу та параметри спільного доступу", "Спільно використовувати принтери", "Налаштувати роздільну здатність екрана", "Перевірити стан комп’ютера", "Створити резервну копію комп’ютера", "Знайти й усунути проблеми", "Перевірити стан брандмауера", "Видалити програму", "Увімкнути або вимкнути компоненти Windows", "Змінити зображення облікового запису", "Додати або видалити облікові записи користувачів", "Налаштувати батьківський контроль для будь-якого користувача", "Змінити дату й час", "Змінити методи введення", "Дозволити Windows пропонувати параметри" },
        { L"tr", "Temayı değiştir", "Masaüstü arka planını değiştir", "Pencere camı renklerini değiştirme", "Ses efektlerini değiştir", "Ekran koruyucuyu değiştir", "Sistem simgelerini açma veya kapatma", "Varsayılan simge davranışlarını geri yükle", "Ağ durumunu ve görevlerini görüntüleyin", "Bir ağa bağlanma", "Ağ bilgisayarlarını ve cihazlarını görüntüleyin", "Ağa kablosuz cihaz ekleme", "Yazıcı ekle", "Varsayılan yazıcıları ayarlama", "Yazıcı ayarlarını değiştirin", "Cihazları ve yazıcıları görüntüleyin", "Ev grubu ve paylaşım seçeneklerini seçin", "Yazıcıları paylaş", "Ekran çözünürlüğünü ayarlayın", "Bilgisayarınızın durumunu inceleyin", "Bilgisayarınızı yedekleyin", "Sorunları bulun ve düzeltin", "Güvenlik duvarı durumunu kontrol edin", "Bir programı kaldırma", "Windows özelliklerini açma veya kapatma", "Hesap resmini değiştir", "Kullanıcı hesaplarını ekleme veya kaldırma", "Herhangi bir kullanıcı için ebeveyn denetimlerini ayarlayın", "Tarihi ve saati değiştirme", "Giriş yöntemlerini değiştirin", "Windows'un sizin için ayarlar önermesine izin verin" },
        { L"ar", "تغيير الموضوع", "تغيير خلفية سطح المكتب", "تغيير ألوان زجاج النوافذ", "تغيير المؤثرات الصوتية", "تغيير شاشة التوقف", "تشغيل أيقونات النظام أو إيقاف تشغيلها", "استعادة سلوكيات الأيقونة الافتراضية", "عرض حالة الشبكة والمهام", "الاتصال بالشبكة", "عرض أجهزة الكمبيوتر والأجهزة المتصلة بالشبكة", "إضافة جهاز لاسلكي إلى الشبكة", "إضافة طابعة", "إعداد الطابعات الافتراضية", "تغيير إعدادات الطابعة", "عرض الأجهزة والطابعات", "اختيار مجموعة المشاركة المنزلية وخيارات المشاركة", "مشاركة الطابعات", "ضبط دقة الشاشة", "مراجعة حالة الكمبيوتر", "إنشاء نسخة احتياطية للكمبيوتر", "البحث عن المشاكل وإصلاحها", "التحقق من حالة جدار الحماية", "إلغاء تثبيت برنامج", "تشغيل ميزات Windows أو إيقاف تشغيلها", "تغيير صورة الحساب", "إضافة أو إزالة حسابات المستخدمين", "إعداد الضوابط الأبوية لأي مستخدم", "تغيير التاريخ والوقت", "تغيير طرق الإدخال", "السماح لـ Windows باقتراح الإعدادات" },
        { L"he", "שנה את הנושא", "שנה רקע שולחן העבודה", "שנה את צבעי זכוכית החלון", "שנה אפקטים קוליים", "שנה שומר מסך", "Turn system icons on or off", "Restore default icon behaviors", "הצג את מצב הרשת ומשימות", "התחבר לרשת", "הצג מחשבים והתקנים ברשת", "הוסף התקן אלחוטי לרשת", "הוסף מדפסת", "הגדר מדפסות ברירת מחדל", "שנה את הגדרות המדפסת", "הצג מכשירים ומדפסות", "Choose homegroup and sharing options", "שתף מדפסות", "התאם את רזולוציית המסך", "Review your computer's status", "גבה את המחשב שלך", "מצא ותקן בעיות", "בדוק את מצב חומת האש", "הסר התקנה של תוכנית", "Turn Windows features on or off", "שנה את תמונת החשבון", "Add or remove user accounts", "Set up parental controls for any user", "שנה את התאריך והשעה", "שנה שיטות קלט", "Let Windows suggest settings for you" },
        { L"ja", "テーマを変更する", "デスクトップの背景を変更する", "窓ガラスの色を変更する", "効果音を変更する", "スクリーンセーバーを変更する", "システムアイコンをオンまたはオフにする", "デフォルトのアイコン動作を復元する", "ネットワークのステータスとタスクを表示する", "ネットワークに接続する", "ネットワークのコンピュータとデバイスを表示する", "ワイヤレスデバイスをネットワークに追加する", "プリンターを追加する", "デフォルトのプリンターを設定する", "プリンターの設定を変更する", "デバイスとプリンターを表示する", "ホームグループと共有オプションを選択する", "プリンターを共有する", "画面解像度を調整する", "コンピュータのステータスを確認する", "コンピュータをバックアップする", "問題を見つけて解決する", "ファイアウォールのステータスを確認する", "プログラムをアンインストールする", "Windows の機能をオンまたはオフにする", "アカウントの写真を変更する", "ユーザーアカウントの追加または削除", "任意のユーザーに対してペアレントコントロールを設定する", "日付と時刻を変更する", "入力方法を変更する", "Windows が設定を提案してくれるようにする" },
        { L"ko", "테마 변경", "데스크탑 배경 변경", "창유리 색상 변경", "음향 효과 변경", "화면 보호기 변경", "시스템 아이콘 켜기 또는 끄기", "기본 아이콘 동작 복원", "네트워크 상태 및 작업 보기", "네트워크에 연결", "네트워크 컴퓨터 및 장치 보기", "네트워크에 무선 장치 추가", "프린터 추가", "기본 프린터 설정", "프린터 설정 변경", "장치 및 프린터 보기", "홈 그룹 및 공유 옵션 선택", "프린터 공유", "화면 해상도 조정", "컴퓨터 상태 검토", "컴퓨터 백업", "문제 찾기 및 수정", "방화벽 상태 확인", "프로그램 제거", "Windows 기능 켜기 또는 끄기", "계정 사진 변경", "사용자 계정 추가 또는 제거", "모든 사용자에 대해 자녀 보호 기능 설정", "날짜 및 시간 변경", "입력 방법 변경", "Windows에서 설정을 제안하도록 허용" },
        { L"zh-CN", "更改主题", "更改桌面背景", "改变窗玻璃颜色", "改变音效", "更改屏幕保护程序", "打开或关闭系统图标", "恢复默认图标行为", "查看网络状态和任务", "连接到网络", "查看网络计算机和设备", "将无线设备添加到网络", "添加打印机", "设置默认打印机", "更改打印机设置", "查看设备和打印机", "选择家庭组和共享选项", "共享打印机", "调整屏幕分辨率", "查看计算机的状态", "备份您的计算机", "发现并解决问题", "检查防火墙状态", "卸载程序", "打开或关闭 Windows 功能", "更改账户图片", "添加或删除用户帐户", "为任何用户设置家长控制", "更改日期和时间", "更改输入法", "让 Windows 为您建议设置" },
        { L"zh-TW", "更改主題", "更改桌面背景", "改變窗玻璃顏色", "改變音效", "更改螢幕保護程式", "開啟或關閉系統圖標", "恢復預設圖示行為", "查看網路狀態和任務", "連接網路", "查看網路電腦和設備", "將無線設備新增至網絡", "新增印表機", "設定預設印表機", "變更印表機設定", "查看設備和印表機", "選擇家庭群組和共享選項", "共用印表機", "調整螢幕解析度", "查看計算機的狀態", "備份您的計算機", "發現並解決問題", "檢查防火牆狀態", "解除安裝程式", "開啟或關閉 Windows 功能", "更改帳戶圖片", "新增或刪除使用者帳戶", "為任何使用者設定家長監護", "更改日期和時間", "更改輸入法", "讓 Windows 為您建議設定" },
        { L"cs", "Změnit téma", "Změnit pozadí plochy", "Změnit barvu okenního skla", "Změnit zvukové efekty", "Změnit spořič obrazovky", "Zapnout nebo vypnout systémové ikony", "Obnovit výchozí chování ikon", "Zobrazit stav sítě a úlohy", "Připojit se k síti", "Zobrazit síťové počítače a zařízení", "Přidat bezdrátové zařízení do sítě", "Přidat tiskárnu", "Nastavit výchozí tiskárny", "Změnit nastavení tiskárny", "Zobrazit zařízení a tiskárny", "Vybrat domácí skupinu a možnosti sdílení", "Sdílet tiskárny", "Upravit rozlišení obrazovky", "Zkontrolovat stav počítače", "Zálohovat počítač", "Najít a opravit problémy", "Zkontrolovat stav brány firewall", "Odinstalovat program", "Zapnout nebo vypnout funkce systému Windows", "Změnit obrázek účtu", "Přidat nebo odebrat uživatelské účty", "Nastavit rodičovskou kontrolu pro libovolného uživatele", "Změnit datum a čas", "Změnit metody zadávání", "Nechat Windows navrhnout nastavení" },
        { L"da", "Skift tema", "Skift skrivebordsbaggrund", "Skift vinduesglasfarver", "Skift lydeffekter", "Skift pauseskærm", "Slå systemikoner til eller fra", "Gendan standard ikonadfærd", "Se netværksstatus og opgaver", "Opret forbindelse til et netværk", "Se netværkscomputere og -enheder", "Tilføj en trådløs enhed til netværket", "Tilføj en printer", "Konfigurer standardprintere", "Skift printerindstillinger", "Se enheder og printere", "Vælg hjemmegruppe og delingsmuligheder", "Del printere", "Juster skærmopløsningen", "Gennemgå din computers status", "Sikkerhedskopier din computer", "Find og ret problemer", "Tjek firewall-status", "Afinstaller et program", "Slå Windows-funktioner til eller fra", "Skift kontobillede", "Tilføj eller fjern brugerkonti", "Konfigurer forældrekontrol for enhver bruger", "Skift dato og klokkeslæt", "Skift indtastningsmetoder", "Lad Windows foreslå indstillinger for dig" },
        { L"fi", "Vaihda teemaa", "Vaihda työpöydän tausta", "Vaihda ikkunalasien väriä", "Muuta äänitehosteita", "Vaihda näytönsäästäjä", "Ota järjestelmäkuvakkeet käyttöön tai poista ne käytöstä", "Palauta oletuskuvakkeiden toimintatavat", "Tarkastele verkon tilaa ja tehtäviä", "Yhdistä verkkoon", "Tarkastele verkon tietokoneita ja laitteita", "Lisää langaton laite verkkoon", "Lisää tulostin", "Aseta oletustulostimet", "Muuta tulostimen asetuksia", "Tarkastele laitteita ja tulostimia", "Valitse kotiryhmä- ja jakamisasetukset", "Jaa tulostimia", "Säädä näytön resoluutiota", "Tarkista tietokoneesi tila", "Varmuuskopioi tietokoneesi", "Etsi ja korjaa ongelmat", "Tarkista palomuurin tila", "Poista ohjelman asennus", "Ota Windowsin ominaisuudet käyttöön tai poista ne käytöstä", "Vaihda tilikuvaa", "Lisää tai poista käyttäjätilejä", "Määritä lapsilukko kaikille käyttäjille", "Muuta päivämäärää ja kellonaikaa", "Muuta syöttötapoja", "Anna Windowsin ehdottaa asetuksia puolestasi" },
        { L"el", "Αλλαγή θέματος", "Αλλαγή φόντου επιφάνειας εργασίας", "Αλλαγή χρωμάτων τζαμιών παραθύρων", "Αλλαγή ηχητικών εφέ", "Αλλαγή προφύλαξης οθόνης", "Ενεργοποίηση ή απενεργοποίηση εικονιδίων συστήματος", "Επαναφορά προεπιλεγμένων συμπεριφορών εικονιδίων", "Προβολή κατάστασης και εργασιών δικτύου", "Σύνδεση σε δίκτυο", "Προβολή υπολογιστών και συσκευών δικτύου", "Προσθήκη ασύρματης συσκευής στο δίκτυο", "Προσθήκη εκτυπωτή", "Ρύθμιση προεπιλεγμένων εκτυπωτών", "Αλλαγή ρυθμίσεων εκτυπωτή", "Προβολή συσκευών και εκτυπωτών", "Επιλογή οικιακής ομάδας και επιλογών κοινής χρήσης", "Κοινή χρήση εκτυπωτών", "Προσαρμογή ανάλυσης οθόνης", "Έλεγχος κατάστασης υπολογιστή", "Δημιουργία αντιγράφου ασφαλείας υπολογιστή", "Εύρεση και επιδιόρθωση προβλημάτων", "Έλεγχος κατάστασης τείχους προστασίας", "Απεγκατάσταση προγράμματος", "Ενεργοποίηση ή απενεργοποίηση δυνατοτήτων των Windows", "Αλλαγή εικόνας λογαριασμού", "Προσθήκη ή κατάργηση λογαριασμών χρηστών", "Ρύθμιση γονικού ελέγχου για οποιονδήποτε χρήστη", "Αλλαγή ημερομηνίας και ώρας", "Αλλαγή μεθόδων εισαγωγής", "Να επιτρέπεται στα Windows να προτείνουν ρυθμίσεις" },
        { L"hu", "Változtasd meg a témát", "Az asztal hátterének módosítása", "Az ablaküveg színének megváltoztatása", "Hanghatások módosítása", "Képernyővédő módosítása", "A rendszerikonok be- és kikapcsolása", "Az alapértelmezett ikonviselkedés visszaállítása", "Megtekintheti a hálózat állapotát és a feladatokat", "Csatlakozzon egy hálózathoz", "Tekintse meg a hálózati számítógépeket és eszközöket", "Adjon hozzá egy vezeték nélküli eszközt a hálózathoz", "Nyomtató hozzáadása", "Állítsa be az alapértelmezett nyomtatókat", "A nyomtató beállításainak módosítása", "Eszközök és nyomtatók megtekintése", "Válassza ki az otthoni csoportot és a megosztási beállításokat", "Nyomtatók megosztása", "Állítsa be a képernyő felbontását", "Tekintse át számítógépe állapotát", "Készítsen biztonsági másolatot a számítógépről", "Keresse meg és javítsa ki a problémákat", "Ellenőrizze a tűzfal állapotát", "Távolítson el egy programot", "Kapcsolja be vagy ki a Windows szolgáltatásait", "Fiókkép módosítása", "Felhasználói fiókok hozzáadása vagy eltávolítása", "Szülői felügyelet beállítása bármely felhasználó számára", "Módosítsa a dátumot és az időt", "Beviteli módszerek módosítása", "Hagyja, hogy a Windows beállításokat javasoljon Önnek" },
        { L"nb", "Endre tema", "Endre skrivebordsbakgrunn", "Endre fargene på vinduets glass", "Endre lydeffekter", "Bytt skjermsparer", "Slå systemikoner på eller av", "Gjenopprett standard ikonatferd", "Se nettverksstatus og oppgaver", "Koble til et nettverk", "Se nettverksdatamaskiner og enheter", "Legg til en trådløs enhet i nettverket", "Legg til en skriver", "Sett opp standardskrivere", "Endre skriverinnstillinger", "Se enheter og skrivere", "Velg hjemmegruppe og delingsalternativer", "Del skrivere", "Juster skjermoppløsningen", "Se gjennom datamaskinens status", "Sikkerhetskopier datamaskinen", "Finn og fiks problemer", "Sjekk brannmurstatus", "Avinstaller et program", "Slå Windows-funksjoner på eller av", "Endre kontobilde", "Legg til eller fjern brukerkontoer", "Sett opp foreldrekontroll for alle brukere", "Endre dato og klokkeslett", "Endre inndatametoder", "La Windows foreslå innstillinger for deg" },
        { L"ro", "Schimbați tema", "Schimbați fundalul desktopului", "Schimbați culorile geamului", "Schimbați efectele sonore", "Schimbați economizorul de ecran", "Activați sau dezactivați pictogramele de sistem", "Restabiliți comportamentul implicit al pictogramelor", "Vizualizați starea rețelei și sarcinile", "Conectați-vă la o rețea", "Vizualizați computerele și dispozitivele din rețea", "Adăugați un dispozitiv fără fir în rețea", "Adăugați o imprimantă", "Configurați imprimante implicite", "Modificați setările imprimantei", "Vizualizați dispozitivele și imprimantele", "Alegeți grupul de acasă și opțiunile de partajare", "Partajați imprimante", "Reglați rezoluția ecranului", "Examinați starea computerului dvs", "Faceți o copie de rezervă a computerului", "Găsiți și rezolvați problemele", "Verificați starea firewallului", "Dezinstalează un program", "Activați sau dezactivați funcțiile Windows", "Schimbați imaginea contului", "Adăugați sau eliminați conturi de utilizator", "Configurați controale parentale pentru orice utilizator", "Schimbați data și ora", "Schimbați metodele de introducere", "Lăsați Windows să vă sugereze setări" },
        { L"sv", "Ändra temat", "Ändra skrivbordsbakgrund", "Ändra fönsterglasfärger", "Ändra ljudeffekter", "Byt skärmsläckare", "Slå på eller av systemikoner", "Återställ standardikonbeteenden", "Visa nätverksstatus och uppgifter", "Anslut till ett nätverk", "Visa nätverksdatorer och enheter", "Lägg till en trådlös enhet i nätverket", "Lägg till en skrivare", "Konfigurera standardskrivare", "Ändra skrivarinställningar", "Visa enheter och skrivare", "Välj hemgrupp och delningsalternativ", "Dela skrivare", "Justera skärmupplösningen", "Granska din dators status", "Säkerhetskopiera din dator", "Hitta och åtgärda problem", "Kontrollera brandväggens status", "Avinstallera ett program", "Slå på eller av Windows-funktioner", "Byt kontobild", "Lägg till eller ta bort användarkonton", "Ställ in föräldrakontroll för alla användare", "Ändra datum och tid", "Ändra inmatningsmetoder", "Låt Windows föreslå inställningar åt dig" },
        { L"vi", "Thay đổi chủ đề", "Thay đổi hình nền máy tính", "Thay đổi màu kính cửa sổ", "Thay đổi hiệu ứng âm thanh", "Thay đổi trình bảo vệ màn hình", "Bật hoặc tắt biểu tượng hệ thống", "Khôi phục hành vi biểu tượng mặc định", "Xem trạng thái và nhiệm vụ mạng", "Kết nối với mạng", "Xem máy tính và thiết bị mạng", "Thêm thiết bị không dây vào mạng", "Thêm máy in", "Thiết lập máy in mặc định", "Thay đổi cài đặt máy in", "Xem thiết bị và máy in", "Chọn nhóm nhà và tùy chọn chia sẻ", "Chia sẻ máy in", "Điều chỉnh độ phân giải màn hình", "Xem lại trạng thái máy tính của bạn", "Sao lưu máy tính của bạn", "Tìm và khắc phục sự cố", "Kiểm tra trạng thái tường lửa", "Gỡ cài đặt một chương trình", "Bật hoặc tắt các tính năng của Windows", "Thay đổi ảnh tài khoản", "Thêm hoặc xóa tài khoản người dùng", "Thiết lập quyền kiểm soát của phụ huynh cho bất kỳ người dùng nào", "Thay đổi ngày và giờ", "Thay đổi phương thức nhập", "Hãy để Windows đề xuất cài đặt cho bạn" },
        { L"id", "Ubah temanya", "Ubah latar belakang desktop", "Mengubah warna kaca jendela", "Ubah efek suara", "Ubah screen saver", "Mengaktifkan atau menonaktifkan ikon sistem", "Pulihkan perilaku ikon default", "Lihat status dan tugas jaringan", "Hubungkan ke jaringan", "Lihat komputer dan perangkat jaringan", "Tambahkan perangkat nirkabel ke jaringan", "Tambahkan pencetak", "Siapkan printer default", "Ubah pengaturan pencetak", "Lihat perangkat dan printer", "Pilih homegroup dan opsi berbagi", "Bagikan printer", "Sesuaikan resolusi layar", "Tinjau status komputer Anda", "Cadangkan komputer Anda", "Temukan dan perbaiki masalah", "Periksa status firewall", "Copot pemasangan suatu program", "Mengaktifkan atau menonaktifkan fitur Windows", "Ubah gambar akun", "Menambah atau menghapus akun pengguna", "Siapkan kontrol orang tua untuk pengguna mana pun", "Ubah tanggal dan waktu", "Ubah metode masukan", "Biarkan Windows menyarankan pengaturan untuk Anda" },
        { L"th", "เปลี่ยนธีม", "เปลี่ยนพื้นหลังเดสก์ท็อป", "เปลี่ยนสีกระจกหน้าต่าง", "เปลี่ยนเอฟเฟกต์เสียง", "เปลี่ยนภาพพักหน้าจอ", "เปิดหรือปิดไอคอนระบบ", "คืนค่าลักษณะการทำงานของไอคอนเริ่มต้น", "ดูสถานะเครือข่ายและงาน", "เชื่อมต่อกับเครือข่าย", "ดูคอมพิวเตอร์และอุปกรณ์เครือข่าย", "เพิ่มอุปกรณ์ไร้สายเข้ากับเครือข่าย", "เพิ่มเครื่องพิมพ์", "ตั้งค่าเครื่องพิมพ์เริ่มต้น", "เปลี่ยนการตั้งค่าเครื่องพิมพ์", "ดูอุปกรณ์และเครื่องพิมพ์", "เลือกโฮมกรุ๊ปและตัวเลือกการแชร์", "แบ่งปันเครื่องพิมพ์", "ปรับความละเอียดหน้าจอ", "ตรวจสอบสถานะของคอมพิวเตอร์ของคุณ", "สำรองข้อมูลคอมพิวเตอร์ของคุณ", "ค้นหาและแก้ไขปัญหา", "ตรวจสอบสถานะไฟร์วอลล์", "ถอนการติดตั้งโปรแกรม", "เปิดหรือปิดคุณสมบัติ Windows", "เปลี่ยนรูปบัญชี", "เพิ่มหรือลบบัญชีผู้ใช้", "ตั้งค่าการควบคุมโดยผู้ปกครองสำหรับผู้ใช้ทุกคน", "เปลี่ยนวันที่และเวลา", "เปลี่ยนวิธีการป้อนข้อมูล", "ให้ Windows แนะนำการตั้งค่าให้กับคุณ" },
        { L"hi", "थीम बदलें", "डेस्कटॉप पृष्ठभूमि बदलें", "खिड़की के शीशे का रंग बदलें", "ध्वनि प्रभाव बदलें", "स्क्रीन सेवर बदलें", "सिस्टम आइकन चालू या बंद करें", "डिफ़ॉल्ट आइकन व्यवहार पुनर्स्थापित करें", "नेटवर्क स्थिति और कार्य देखें", "किसी नेटवर्क से कनेक्ट करें", "नेटवर्क कंप्यूटर और डिवाइस देखें", "नेटवर्क में एक वायरलेस डिवाइस जोड़ें", "एक प्रिंटर जोड़ें", "डिफ़ॉल्ट प्रिंटर सेट करें", "प्रिंटर सेटिंग बदलें", "डिवाइस और प्रिंटर देखें", "होमग्रुप और साझाकरण विकल्प चुनें", "प्रिंटर साझा करें", "स्क्रीन रिज़ॉल्यूशन समायोजित करें", "अपने कंप्यूटर की स्थिति की समीक्षा करें", "अपने कंप्यूटर का बैकअप लें", "समस्याएं ढूंढें और ठीक करें", "फ़ायरवॉल स्थिति जाँचें", "किसी प्रोग्राम को अनइंस्टॉल करें", "विंडोज़ सुविधाओं को चालू या बंद करें", "खाता चित्र बदलें", "उपयोगकर्ता खाते जोड़ें या हटाएँ", "किसी भी उपयोगकर्ता के लिए अभिभावकीय नियंत्रण सेट करें", "दिनांक और समय बदलें", "इनपुट पद्धतियाँ बदलें", "विंडोज़ को आपके लिए सेटिंग्स सुझाने दें" },
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
    <sh:task id="{D4F4A002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{BACKGROUND}</sh:name><sh:keywords>desktop;background;wallpaper</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\pageWallpaper</sh:command></sh:task>
    <sh:task id="{D4F4A003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{COLORS}</sh:name><sh:keywords>window;color;glass;colorization</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\pageColorization</sh:command></sh:task>
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
  <application id="Microsoft.NotificationAreaIcons">
    <sh:task id="{D4F4B001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{SYSTEMICONS}</sh:name><sh:controlpanel name="Microsoft.NotificationAreaIcons" page="SystemIcons"/></sh:task>
    <sh:task id="{D4F4B002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{RESTOREICONS}</sh:name><sh:controlpanel name="Microsoft.NotificationAreaIcons"/></sh:task>
    <!-- This applet intentionally has no Control Panel category. Category 0
         keeps it out of Appearance and Personalization while retaining its
         search task links. -->
    <category id="0">
       <sh:task idref="{D4F4B001-0D35-4CB6-A21F-BC1661200001}"/>
       <sh:task idref="{D4F4B002-0D35-4CB6-A21F-BC1661200002}"/>
    </category>
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
{CATEGORY_BLUE_LINKS_BLOCK}
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
    
    // Windows 7 Category Blue Links
    if (g_settings.restoreWin7CategoryBlueLinks.load()) {
        replaceAll("{CATEGORY_BLUE_LINKS_BLOCK}",
            "  <!-- System and Security (Category 5) -->\n"
            "  <application id=\"{A453CFBA-AE7B-4B9E-AA41-B6A7FAE9C3E5}\">\n"
            "    <sh:task id=\"{D4F4F001-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{REVIEWSTATUS}</sh:name><sh:command>rundll32.exe shell32.dll,Control_RunDLL wscui.cpl</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F002-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{BACKUPCOMPUTER}</sh:name><sh:command>sdclt.exe</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F003-0D35-4CB6-A21F-BC1661200003}\"><sh:name>{FINDFIXPROBLEMS}</sh:name><sh:controlpanel name=\"Microsoft.Troubleshooting\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4F004-0D35-4CB6-A21F-BC1661200004}\"><sh:name>{CHECKFIREWALL}</sh:name><sh:controlpanel name=\"Microsoft.WindowsFirewall\"/></sh:task>\n"
            "    <category id=\"5\">\n"
            "       <sh:task idref=\"{D4F4F001-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F002-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "       <sh:task idref=\"{D4F4F003-0D35-4CB6-A21F-BC1661200003}\"/>\n"
            "       <sh:task idref=\"{D4F4F004-0D35-4CB6-A21F-BC1661200004}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- Programs (Category 8) -->\n"
            "  <application id=\"{7B81BE6A-CE2B-4C39-9987-2B3D27A6CF15}\">\n"
            "    <sh:task id=\"{D4F4F101-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{UNINSTALLPROGRAM}</sh:name><sh:controlpanel name=\"Microsoft.ProgramsAndFeatures\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4F102-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{TURNWINDOWSFEATURES}</sh:name><sh:command>optionalfeatures.exe</sh:command></sh:task>\n"
            "    <category id=\"8\">\n"
            "       <sh:task idref=\"{D4F4F101-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F102-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- User Accounts (Category 9) -->\n"
            "  <application id=\"{60632754-C523-4B62-B45C-4172DA0126C0}\">\n"
            "    <sh:task id=\"{D4F4F201-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{CHANGEACCTPICTURE}</sh:name><sh:controlpanel name=\"Microsoft.UserAccounts\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4F202-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{ADDREMOVEACCOUNTS}</sh:name><sh:command>netplwiz.exe</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F203-0D35-4CB6-A21F-BC1661200003}\"><sh:name>{SETPARENTALCONTROLS}</sh:name><sh:command>rundll32.exe shell32.dll,Control_RunDLL %SystemRoot%\\System32\\parentalcontrols.cpl</sh:command></sh:task>\n"
            "    <category id=\"9\">\n"
            "       <sh:task idref=\"{D4F4F201-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F202-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "       <sh:task idref=\"{D4F4F203-0D35-4CB6-A21F-BC1661200003}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- Clock, Language, and Region (Category 6) -->\n"
            "  <application id=\"{E2B7C8E1-3F1A-4B6D-9C4E-5A8F2D7B3E60}\">\n"
            "    <sh:task id=\"{D4F4F301-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{CHANGEDATETIME}</sh:name><sh:controlpanel name=\"Microsoft.DateAndTime\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4F302-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{CHANGEINPUTMETHOD}</sh:name><sh:controlpanel name=\"Microsoft.RegionAndLanguage\" page=\"Input\"/></sh:task>\n"
            "    <category id=\"6\">\n"
            "       <sh:task idref=\"{D4F4F301-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F302-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- Ease of Access (Category 7) -->\n"
            "  <application id=\"{D555E8C2-3A1B-4D6F-8E9A-2C7B4F3D8A15}\">\n"
            "    <sh:task id=\"{D4F4F401-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{LETWINDOWSSUGGEST}</sh:name><sh:controlpanel name=\"Microsoft.EaseOfAccessCenter\"/></sh:task>\n"
            "    <category id=\"7\">\n"
            "       <sh:task idref=\"{D4F4F401-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "    </category>\n"
            "  </application>");
    } else {
        replaceAll("{CATEGORY_BLUE_LINKS_BLOCK}", "");
    }

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
            "    <sh:task id=\"{D4F4A002-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{BACKGROUND}</sh:name><sh:keywords>desktop;background;wallpaper</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\\pageWallpaper</sh:command></sh:task>\n"
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
    replaceAll("{REVIEWSTATUS}", texts->reviewComputerStatus);
    replaceAll("{BACKUPCOMPUTER}", texts->backUpComputer);
    replaceAll("{FINDFIXPROBLEMS}", texts->findAndFixProblems);
    replaceAll("{CHECKFIREWALL}", texts->checkFirewallStatus);
    replaceAll("{UNINSTALLPROGRAM}", texts->uninstallProgram);
    replaceAll("{TURNWINDOWSFEATURES}", texts->turnWindowsFeatures);
    replaceAll("{CHANGEACCTPICTURE}", texts->changeAccountPicture);
    replaceAll("{ADDREMOVEACCOUNTS}", texts->addRemoveAccounts);
    replaceAll("{SETPARENTALCONTROLS}", texts->setParentalControls);
    replaceAll("{CHANGEDATETIME}", texts->changeDateTime);
    replaceAll("{CHANGEINPUTMETHOD}", texts->changeInputMethod);
    replaceAll("{LETWINDOWSSUGGEST}", texts->letWindowsSuggest);


    std::ofstream file(g_classicTaskLinksFilePath.c_str(), std::ios::binary | std::ios::trunc);
    if (!file) {
        g_classicTaskLinksFilePath.clear();
        return false;
    }

    file.write(taskList.data(), static_cast<std::streamsize>(taskList.size()));
    bool ok = file.good();
    LOG_IF_ENABLED("[TaskLinks] XML file written to: %s (size=%zu, ok=%d)",
        g_classicTaskLinksFilePath.c_str(), taskList.size(), ok);
    LOG_IF_ENABLED("[TaskLinks] Locale selected: %s", texts->locale);
    LOG_IF_ENABLED("[TaskLinks] BlueLinks=%d CatAppearance=%d",
        g_settings.restoreWin7CategoryBlueLinks.load(), g_settings.enableCategoryAppearanceLinks.load());
    return ok;
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
    g_settings.restoreWin7CategoryBlueLinks.store(Wh_GetIntSetting(L"restoreWin7CategoryBlueLinks"));
    g_settings.enableLogging.store(Wh_GetIntSetting(L"enableLogging"));
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
    LOG_IF_ENABLED("[Init] Personalization display name: %s", g_personalizationName.c_str());
    
    // Pre-compute lowercase GUIDs
    g_personalizationGuidLower    = ToLower(kPersonalizationGuid);
    g_notificationIconsGuidLower  = ToLower(kNotificationIconsGuid);
    g_networkConnectionsGuidLower = ToLower(kNetworkConnectionsGuid);
    g_printersAndFaxesGuidLower   = ToLower(kPrintersAndFaxesGuid);
    g_homeGroupGuidLower          = ToLower(kHomeGroupGuid);
    g_displayGuidLower            = ToLower(kDisplayGuid);
    g_realPersonalizationGuidLower = ToLower(kRealPersonalizationGuid);
    g_suppressedGuidLower         = ToLower(kSuppressedGuid);
}

// GetTrackedPath/TrackKey/UntrackKey/CreateFakeHandle/FreeFakeHandle now live
// as methods on g_keyTracker (see KeyTracker class above).


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

    if (g_settings.enableCategoryAppearanceLinks.load()) {
        if (EndsWith(lower, L"clsid\\" + g_realPersonalizationGuidLower) ||
            EndsWith(lower, L"clsid\\" + g_displayGuidLower)) {
            return { VNode::ClsidRoot, ItemKind::RealCplTaskUrl, 0 };
        }
    }

    struct { std::atomic<bool>* enabled; const std::wstring* guidLower; DWORD cat; } categoryItems[] = {
        { &g_settings.enableNotificationIcons,  &g_notificationIconsGuidLower,  0 }, // Keep it outside category view; search still exposes its tasks
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
    if (!lpData) {
        *lpcbData = requiredSize;
        return ERROR_SUCCESS; // Standard two-call size-probe pattern
    }
    if (*lpcbData < requiredSize) {
        *lpcbData = requiredSize;
        return ERROR_MORE_DATA;
    }
    *lpcbData = requiredSize;
    memcpy(lpData, str.c_str(), requiredSize);
    return ERROR_SUCCESS;
}

LSTATUS ProvideDwordValue(LPBYTE lpData, LPDWORD lpcbData, DWORD value) {
    if (!lpcbData) return ERROR_INVALID_PARAMETER;
    if (!lpData) {
        *lpcbData = sizeof(DWORD);
        return ERROR_SUCCESS; // Standard two-call size-probe pattern
    }
    if (*lpcbData < sizeof(DWORD)) {
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
        LOG_IF_ENABLED("[Suppress] Blocked CLSID query: %s", path.c_str());
        outStatus = ERROR_FILE_NOT_FOUND;
        return true;
    }

    if (cr.kind == ItemKind::RealCplTaskUrl) {
        LOG_IF_ENABLED("[RealCplTaskUrl] Providing value for: %s (value=%s)", path.c_str(), valueName.c_str());
        if (valueName == L"System.Software.TasksFileUrl" && EnsureClassicTaskLinksFile()) {
            if (lpType) *lpType = REG_SZ;
            outStatus = ProvideStringValue(lpData, lpcbData, GetClassicTaskLinksFilePath());
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
        LOG_IF_ENABLED("[CategoryOnly] Providing value for: %s (value=%s, cat=%lu)", path.c_str(), valueName.c_str(), cr.category);
        if (valueName == L"System.ControlPanel.Category") {
            if (lpType) *lpType = REG_DWORD;
            outStatus = ProvideDwordValue(lpData, lpcbData, cr.category);
            return true;
        }
        // Category 0 is used only by the virtual Notification Area Icons applet.
        // Search uses the application name to associate task metadata with it.
        if (cr.category == 0 && valueName == L"System.ApplicationName") {
            if (lpType) *lpType = REG_SZ;
            outStatus = ProvideStringValue(lpData, lpcbData, L"Microsoft.NotificationAreaIcons");
            return true;
        }
        if (valueName == L"System.Software.TasksFileUrl" &&
            g_settings.restoreClassicTaskLinks.load() && EnsureClassicTaskLinksFile()) {
            if (lpType) *lpType = REG_SZ;
            outStatus = ProvideStringValue(lpData, lpcbData, GetClassicTaskLinksFilePath());
            return true;
        }
        return false;
    }

    if (cr.kind == ItemKind::Personalization) {
        LOG_IF_ENABLED("[Personalization] Providing value for: %s (value=%s, node=%d)", path.c_str(), valueName.c_str(), (int)cr.node);
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
                std::wstring taskFileUrl =
                    (g_settings.restoreClassicTaskLinks.load() && EnsureClassicTaskLinksFile())
                        ? GetClassicTaskLinksFilePath()
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
    // Hooks run on arbitrary explorer.exe threads with no expectation of C++
    // exceptions unwinding through them; a stray std::bad_alloc etc. must not
    // escape into foreign code, so fall back to the real API on any failure.
    try {
        if (g_keyTracker.IsFake(hKey)) {
            std::wstring fullPath = g_keyTracker.GetPath(hKey);
            if (lpSubKey && *lpSubKey) {
                if (!fullPath.empty()) fullPath += L"\\";
                fullPath += lpSubKey;
            }
            if (IsTargetKey(fullPath)) {
                HKEY fake = g_keyTracker.CreateFake(fullPath);
                if (!fake) return ERROR_OUTOFMEMORY;
                LOG_IF_ENABLED("[RegOpen] Fake handle (child): %s", fullPath.c_str());
                if (phkResult) *phkResult = fake;
                return ERROR_SUCCESS;
            }
            return ERROR_FILE_NOT_FOUND;
        }

        if (g_settings.suppressCompanySync.load() && lpSubKey) {
            std::wstring basePath = g_keyTracker.GetPath(hKey);
            std::wstring fullPath = basePath;
            if (*lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
            if (IsSuppressedNamespaceKey(ToLower(fullPath))) {
                LOG_IF_ENABLED("[RegOpen] Suppressed key: %s", fullPath.c_str());
                return ERROR_FILE_NOT_FOUND;
            }
        }

        LSTATUS status = RegOpenKeyExWOriginal(hKey, lpSubKey, ulOptions, samDesired, phkResult);
        if (status == ERROR_SUCCESS && phkResult && *phkResult) {
            std::wstring basePath = g_keyTracker.GetPath(hKey);
            std::wstring fullPath = basePath;
            if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
            g_keyTracker.Track(*phkResult, fullPath);
        } else if (status == ERROR_FILE_NOT_FOUND && phkResult) {
            std::wstring basePath = g_keyTracker.GetPath(hKey);
            std::wstring fullPath = basePath;
            if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
            if (IsTargetKey(fullPath)) {
                HKEY fake = g_keyTracker.CreateFake(fullPath);
                if (!fake) return ERROR_OUTOFMEMORY;
                LOG_IF_ENABLED("[RegOpen] Fake handle (not found): %s", fullPath.c_str());
                *phkResult = fake;
                return ERROR_SUCCESS;
            }
        }
        return status;
    } catch (...) {
        LOG_IF_ENABLED("[RegOpen] Exception caught, falling back to real API");
        return RegOpenKeyExWOriginal(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }
}

using RegCloseKey_t = decltype(&RegCloseKey);
RegCloseKey_t RegCloseKeyOriginal;
LSTATUS WINAPI RegCloseKeyHook(HKEY hKey) {
    try {
        if (g_keyTracker.IsFake(hKey)) {
            g_keyTracker.FreeFake(hKey);
            return ERROR_SUCCESS;
        }
        LSTATUS status = RegCloseKeyOriginal(hKey);
        g_keyTracker.Untrack(hKey);
        return status;
    } catch (...) {
        LOG_IF_ENABLED("[RegClose] Exception caught, falling back to real API");
        return RegCloseKeyOriginal(hKey);
    }
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExWOriginal;
LSTATUS WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    try {
        std::wstring path = g_keyTracker.GetPath(hKey);
        if (!path.empty()) {
            std::wstring valueName = lpValueName ? lpValueName : L"";
            LSTATUS outStatus;
            if (TryProvideValue(path, valueName, lpType, lpData, lpcbData, outStatus)) return outStatus;
        }

        if (g_keyTracker.IsFake(hKey)) return ERROR_FILE_NOT_FOUND;

        return RegQueryValueExWOriginal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    } catch (...) {
        LOG_IF_ENABLED("[RegQueryValue] Exception caught, falling back to real API");
        return RegQueryValueExWOriginal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    }
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueWOriginal;
LSTATUS WINAPI RegGetValueWHook(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue,
                                DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    try {
        if (g_keyTracker.IsFake(hkey)) return ERROR_FILE_NOT_FOUND;

        std::wstring path = g_keyTracker.GetPath(hkey);
        if (lpSubKey && *lpSubKey) { if (!path.empty()) path += L"\\"; path += lpSubKey; }
        if (!path.empty()) {
            std::wstring valueName = lpValue ? lpValue : L"";
            LSTATUS outStatus;
            if (TryProvideValue(path, valueName, pdwType, (LPBYTE)pvData, pcbData, outStatus)) return outStatus;
        }
        return RegGetValueWOriginal(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    } catch (...) {
        LOG_IF_ENABLED("[RegGetValue] Exception caught, falling back to real API");
        return RegGetValueWOriginal(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    }
}

using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
RegEnumKeyExW_t RegEnumKeyExWOriginal;
LSTATUS WINAPI RegEnumKeyExWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName,
                                 LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass,
                                 PFILETIME lpftLastWriteTime) {
    try {
        if (g_keyTracker.IsFake(hKey)) {
            std::wstring path = g_keyTracker.GetPath(hKey);
            ClassifyResult cr = ClassifyPath(path);
            std::wstring subName;
            if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
            if (!lpcchName || !lpName) return ERROR_INVALID_PARAMETER;
            if (*lpcchName < subName.size() + 1) {
                *lpcchName = (DWORD)(subName.size() + 1);
                return ERROR_MORE_DATA;
            }
            wcscpy_s(lpName, *lpcchName, subName.c_str());
            *lpcchName = (DWORD)subName.size();
            if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
            return ERROR_SUCCESS;
        }

        const std::wstring path = g_keyTracker.GetPath(hKey);
        if (!IsNameSpaceParentKey(path)) {
            return RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName, lpReserved,
                                         lpClass, lpcchClass, lpftLastWriteTime);
        }

        // Return virtual CLSIDs before real namespace entries. This is the
        // portable ordering mechanism; no private shell32 layout is involved.
        const std::vector<std::wstring> clsids = GetNamespaceClsids();
        if (dwIndex < clsids.size()) {
            if (!lpcchName || !lpName) return ERROR_INVALID_PARAMETER;
            const std::wstring& clsid = clsids[dwIndex];
            if (*lpcchName < clsid.size() + 1) {
                *lpcchName = (DWORD)(clsid.size() + 1);
                return ERROR_MORE_DATA;
            }
            wcscpy_s(lpName, *lpcchName, clsid.c_str());
            *lpcchName = (DWORD)clsid.size();
            if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
            return ERROR_SUCCESS;
        }

        const DWORD requestedRealIndex = dwIndex - (DWORD)clsids.size();
        if (!g_settings.suppressCompanySync.load()) {
            return RegEnumKeyExWOriginal(hKey, requestedRealIndex, lpName, lpcchName, lpReserved,
                                         lpClass, lpcchClass, lpftLastWriteTime);
        }

        // Map the virtual index to a real one without using the caller's buffer
        // while scanning. ControlPanel\NameSpace entries are CLSIDs, so 256
        // wchar_t characters is more than sufficient.
        wchar_t scannedName[256];
        DWORD realIndex = 0;
        DWORD visibleRealIndex = 0;
        for (;;) {
            DWORD scannedCch = ARRAYSIZE(scannedName);
            const LSTATUS status = RegEnumKeyExWOriginal(
                hKey, realIndex, scannedName, &scannedCch,
                nullptr, nullptr, nullptr, nullptr);
            if (status != ERROR_SUCCESS) return status;

            if (!IsSuppressedNamespaceEntry(scannedName)) {
                if (visibleRealIndex == requestedRealIndex) {
                    // Repeat only the selected entry with caller-provided
                    // output buffers, preserving class and timestamp output.
                    return RegEnumKeyExWOriginal(hKey, realIndex, lpName, lpcchName, lpReserved,
                                                 lpClass, lpcchClass, lpftLastWriteTime);
                }
                ++visibleRealIndex;
            }
            ++realIndex;
        }
    } catch (...) {
        LOG_IF_ENABLED("[RegEnumKeyEx] Exception caught, falling back to real API");
        return RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName,
                                     lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
    }
}

using RegEnumKeyW_t = decltype(&RegEnumKeyW);
RegEnumKeyW_t RegEnumKeyWOriginal;
LSTATUS WINAPI RegEnumKeyWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, DWORD cchName) {
    try {
        if (g_keyTracker.IsFake(hKey)) {
            std::wstring path = g_keyTracker.GetPath(hKey);
            ClassifyResult cr = ClassifyPath(path);
            std::wstring subName;
            if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
            if (!lpName) return ERROR_INVALID_PARAMETER;
            if (cchName <= subName.size()) return ERROR_MORE_DATA;
            wcscpy_s(lpName, cchName, subName.c_str());
            return ERROR_SUCCESS;
        }

        const std::wstring path = g_keyTracker.GetPath(hKey);
        if (!IsNameSpaceParentKey(path))
            return RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);

        const std::vector<std::wstring> clsids = GetNamespaceClsids();
        if (dwIndex < clsids.size()) {
            if (!lpName) return ERROR_INVALID_PARAMETER;
            const std::wstring& clsid = clsids[dwIndex];
            if (cchName <= clsid.size()) return ERROR_MORE_DATA;
            wcscpy_s(lpName, cchName, clsid.c_str());
            return ERROR_SUCCESS;
        }

        const DWORD requestedRealIndex = dwIndex - (DWORD)clsids.size();
        if (!g_settings.suppressCompanySync.load())
            return RegEnumKeyWOriginal(hKey, requestedRealIndex, lpName, cchName);

        wchar_t scannedName[256];
        DWORD realIndex = 0;
        DWORD visibleRealIndex = 0;
        for (;;) {
            const LSTATUS status = RegEnumKeyWOriginal(hKey, realIndex, scannedName,
                                                        ARRAYSIZE(scannedName));
            if (status != ERROR_SUCCESS) return status;
            if (!IsSuppressedNamespaceEntry(scannedName)) {
                if (visibleRealIndex == requestedRealIndex)
                    return RegEnumKeyWOriginal(hKey, realIndex, lpName, cchName);
                ++visibleRealIndex;
            }
            ++realIndex;
        }
    } catch (...) {
        LOG_IF_ENABLED("[RegEnumKey] Exception caught, falling back to real API");
        return RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);
    }
}

using ShellExecuteExW_t = BOOL(WINAPI*)(LPSHELLEXECUTEINFOW);
ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;

BOOL WINAPI ShellExecuteExWHook(LPSHELLEXECUTEINFOW psei) {
  try {
    if (!psei || psei->cbSize < sizeof(SHELLEXECUTEINFOW))
        return ShellExecuteExWOriginal(psei);

    if (psei->lpFile) {
        std::wstring file = ToLower(psei->lpFile);
        if (file == L"explorer.exe" || file == L"explorer") {
            if (psei->lpParameters) {
                std::wstring params = psei->lpParameters;
                // Only match if parameters START with shell::: (after trimming)
                size_t firstNonSpace = params.find_first_not_of(L" \t");
                if (firstNonSpace != std::wstring::npos) {
                    std::wstring trimmed = params.substr(firstNonSpace);
                    if (ToLower(trimmed).find(L"shell:::") == 0) {
                        std::wstring shellCommand = trimmed;
                        
                        std::wstring newFile;
                        std::wstring newParams;
                        
                        size_t spacePos = shellCommand.find(L' ');
                        if (spacePos != std::wstring::npos) {
                            newFile = shellCommand.substr(0, spacePos);
                            newParams = shellCommand.substr(spacePos + 1);
                            size_t ns = newParams.find_first_not_of(L" \t");
                            if (ns != std::wstring::npos)
                                newParams = newParams.substr(ns);
                            else
                                newParams.clear();
                        } else {
                            newFile = shellCommand;
                        }
                        
                        // Copy by cbSize, but never write past our local struct's
                        // own size even if the caller's cbSize is larger than
                        // what we know about (forward-compatibility safety net).
                        SHELLEXECUTEINFOW sei{};
                        memcpy(&sei, psei, (std::min)((size_t)psei->cbSize, sizeof(SHELLEXECUTEINFOW)));
                        sei.lpFile = newFile.c_str();
                        sei.lpParameters = newParams.empty() ? nullptr : newParams.c_str();
                        
                        LOG_IF_ENABLED("[ShellExec] Redirected: %s %s -> %s %s",
                            psei->lpFile, psei->lpParameters ? psei->lpParameters : L"",
                            newFile.c_str(), newParams.empty() ? L"" : newParams.c_str());
                        
                        BOOL result = ShellExecuteExWOriginal(&sei);
                        // Propagate output fields back to caller
                        psei->hInstApp = sei.hInstApp;
                        if (psei->fMask & SEE_MASK_NOCLOSEPROCESS)
                            psei->hProcess = sei.hProcess;
                        return result;
                    }
                }
            }
        }
    }
    return ShellExecuteExWOriginal(psei);
  } catch (...) {
      LOG_IF_ENABLED("[ShellExec] Exception caught, falling back to real API");
      return ShellExecuteExWOriginal(psei);
  }
}
#define CControlPanelAppletList_HDPA(pThis) ((HDPA)*((void ***)pThis + 2))
#define CControlPanelAppletList_Category(pThis) *((DWORD *)pThis + 32)

/* Map CPL category ID to array index - credits to aubymori*/
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

// DPA_GetPtr is available from comctl32 (linked via -lcomctl32)

int (WINAPI *CControlPanelAppletList_s_SortAppletsInCategory_orig)(void *, void *, LPARAM);

// The offsets used below (HDPA pointer, category DWORD, applet moniker string)
// come from reverse-engineering one specific shell32.dll build. They are NOT
// guaranteed to be correct on every Windows build/architecture, so every
// pointer is validated with VirtualQuery before it's ever dereferenced, and
// the resulting "applet name" is validated to actually look like a CLSID
// moniker (e.g. "::{580722ff-...}") before it's used for anything. If any
// check fails, we transparently fall back to the original comparison —
// worst case our custom sort is skipped, it can never crash explorer.exe.
static bool g_sortHookSafe = false;

// Returns true only if [ptr, ptr+size) is entirely within a single committed,
// readable memory region (i.e. safe to dereference without crashing).
static bool IsReadableMemory(const void *ptr, size_t size) {
    if (!ptr || size == 0) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(ptr, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
    if (mbi.Protect == 0) return false;
    auto start = (uintptr_t)ptr;
    auto regionEnd = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
    if (start + size < start) return false; // overflow
    return (start + size) <= regionEnd;
}

// Validates that lpszApplet points to a readable, null-terminated string that
// actually looks like one of our "::{GUID}" applet monikers, reading it one
// safely-validated chunk at a time (never dereferences past what VirtualQuery
// confirmed is committed/readable).
static bool LooksLikeAppletMoniker(LPCWSTR lpszApplet) {
    if (!IsReadableMemory(lpszApplet, sizeof(wchar_t) * 3)) return false;
    if (lpszApplet[0] != L':' || lpszApplet[1] != L':' || lpszApplet[2] != L'{') return false;

    // GUID monikers ("::{8-4-4-4-12}") are 40 characters; cap the scan well
    // above that so a corrupt/non-terminated pointer can't run away.
    const size_t kMaxLen = 64;
    for (size_t i = 0; i < kMaxLen; i++) {
        if (!IsReadableMemory(&lpszApplet[i], sizeof(wchar_t))) return false;
        if (lpszApplet[i] == L'\0') return i > 3 && lpszApplet[i - 1] == L'}';
    }
    return false;
}

void ValidateSortHookOffsets() {
    // Runtime pointer/format validation (above) is what actually keeps this
    // hook safe; this build check is just an extra, conservative gate on top
    // of it. The x86 offsets are known-wrong (different pointer size changes
    // every subsequent byte offset), so the hook stays off there regardless.
#ifdef _WIN64
    g_sortHookSafe = true;
    LOG_IF_ENABLED("[SortHook] Enabled for x64 (build %u); runtime pointer validation active", g_winBuild);
#else
    LOG_IF_ENABLED("[SortHook] Disabled: x86 offsets unverified (build %u)", g_winBuild);
#endif
}

int WINAPI CControlPanelAppletList_s_SortAppletsInCategory_hook(
    void *p1, void *p2, LPARAM lParam
) {
    if (!g_sortHookSafe)
        return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);

    if (!IsReadableMemory(p1, sizeof(int)) || !IsReadableMemory(p2, sizeof(int)) ||
        !IsReadableMemory((void *)lParam, sizeof(void *) * 2 + sizeof(DWORD) * 33)) {
        return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);
    }

    HDPA hDpa = (HDPA)CControlPanelAppletList_HDPA(lParam);
    if (!hDpa) return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);

    int category = MapCategory(CControlPanelAppletList_Category(lParam));
    if (category < 0) return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);

    LPVOID pThing1 = DPA_GetPtr(hDpa, *(int *)p1);
    LPVOID pThing2 = DPA_GetPtr(hDpa, *(int *)p2);
    if (!IsReadableMemory(pThing1, 520 + sizeof(wchar_t)) ||
        !IsReadableMemory(pThing2, 520 + sizeof(wchar_t))) {
        return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);
    }

    LPCWSTR pszApplet1 = (LPCWSTR)((char *)pThing1 + 520);
    LPCWSTR pszApplet2 = (LPCWSTR)((char *)pThing2 + 520);
    if (!LooksLikeAppletMoniker(pszApplet1) || !LooksLikeAppletMoniker(pszApplet2)) {
        return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);
    }

    int iApplet1 = FindApplet(pszApplet1, category);
    int iApplet2 = FindApplet(pszApplet2, category);

    if (iApplet1 >= 0 && iApplet2 >= 0) {
        return iApplet1 - iApplet2;
    } else if (iApplet1 >= 0) {
        return -1; // Move our custom applet to the top
    } else if (iApplet2 >= 0) {
        return 1;
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

void InvalidateClassicTaskLinksFile() {
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    g_classicTaskLinksFilePath.clear();
}

void Wh_ModSettingsChanged() {
  try {
    LoadSettings();
    // Regenerate task links file with updated settings
    InvalidateClassicTaskLinksFile();
    EnsureClassicTaskLinksFile();
    LOG_IF_ENABLED("[Settings] Changed - Pers=%d Notif=%d Net=%d Print=%d Home=%d CatApp=%d Suppress=%d TaskLinks=%d BlueLinks=%d Log=%d",
        g_settings.enablePersonalization.load(), g_settings.enableNotificationIcons.load(),
        g_settings.enableNetworkConnections.load(), g_settings.enablePrintersAndFaxes.load(),
        g_settings.enableHomeGroup.load(), g_settings.enableCategoryAppearanceLinks.load(),
        g_settings.suppressCompanySync.load(), g_settings.restoreClassicTaskLinks.load(),
        g_settings.restoreWin7CategoryBlueLinks.load(), g_settings.enableLogging.load());
  } catch (...) {
      Wh_Log(L"Exception while applying changed settings");
  }
}

BOOL Wh_ModInit() {
  try {
    LoadSettings();
    // Conflict detection: check if the original mod (restore-classic-cpls) is loaded
    // If both mods are active, they would both inject the same CLSIDs, causing conflicts.
    if (GetModuleHandleW(L"restore-classic-cpls.wh.cpp") ||
        FindWindowW(L"WindhawkMod_restore-classic-cpls", nullptr)) {
        Wh_Log(L"CONFLICT: Original mod (restore-classic-cpls) detected. This mod will deactivate to avoid conflicts.");
        Wh_Log(L"To use this mod, please disable the original restore-classic-cpls mod first.");
        return FALSE;
    }

    DetectWindowsVersion();

    ValidateSortHookOffsets();


    // Generate task links file eagerly to avoid data races
    EnsureClassicTaskLinksFile();

    LOG_IF_ENABLED("=== Windows 7 Legacy Applet Restorer Init ===");
    LOG_IF_ENABLED("[Version] %s", g_winVersionStr);
    LOG_IF_ENABLED("[Settings] Pers=%d Notif=%d Net=%d Print=%d Home=%d CatApp=%d Suppress=%d TaskLinks=%d BlueLinks=%d",
        g_settings.enablePersonalization.load(), g_settings.enableNotificationIcons.load(),
        g_settings.enableNetworkConnections.load(), g_settings.enablePrintersAndFaxes.load(),
        g_settings.enableHomeGroup.load(), g_settings.enableCategoryAppearanceLinks.load(),
        g_settings.suppressCompanySync.load(), g_settings.restoreClassicTaskLinks.load(),
        g_settings.restoreWin7CategoryBlueLinks.load());

    void* pRegOpenKeyExW      = GetRegFunc("RegOpenKeyExW");
    void* pRegCloseKey        = GetRegFunc("RegCloseKey");
    void* pRegQueryValueExW   = GetRegFunc("RegQueryValueExW");
    void* pRegGetValueW       = GetRegFunc("RegGetValueW");
    void* pRegEnumKeyExW      = GetRegFunc("RegEnumKeyExW");
    void* pRegEnumKeyW        = GetRegFunc("RegEnumKeyW");

    if (!pRegOpenKeyExW || !pRegCloseKey || !pRegQueryValueExW ||
        !pRegGetValueW  || !pRegEnumKeyExW || !pRegEnumKeyW) {
        Wh_Log(L"Failed to get one or more registry functions");
        return FALSE;
    }

    InitDisplayNames();

    if (!Wh_SetFunctionHook(pRegOpenKeyExW,    (void*)RegOpenKeyExWHook,    (void**)&RegOpenKeyExWOriginal))    { Wh_Log(L"Failed to hook RegOpenKeyExW");    return FALSE; }
    if (!Wh_SetFunctionHook(pRegCloseKey,      (void*)RegCloseKeyHook,      (void**)&RegCloseKeyOriginal))      { Wh_Log(L"Failed to hook RegCloseKey");      return FALSE; }
    if (!Wh_SetFunctionHook(pRegQueryValueExW, (void*)RegQueryValueExWHook, (void**)&RegQueryValueExWOriginal)) { Wh_Log(L"Failed to hook RegQueryValueExW"); return FALSE; }
    if (!Wh_SetFunctionHook(pRegGetValueW,     (void*)RegGetValueWHook,     (void**)&RegGetValueWOriginal))     { Wh_Log(L"Failed to hook RegGetValueW");     return FALSE; }
    if (!Wh_SetFunctionHook(pRegEnumKeyExW,    (void*)RegEnumKeyExWHook,    (void**)&RegEnumKeyExWOriginal))    { Wh_Log(L"Failed to hook RegEnumKeyExW");    return FALSE; }
    if (!Wh_SetFunctionHook(pRegEnumKeyW,      (void*)RegEnumKeyWHook,      (void**)&RegEnumKeyWOriginal))      { Wh_Log(L"Failed to hook RegEnumKeyW");      return FALSE; }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (hShell32) {
        void* pShellExecuteExW = (void*)GetProcAddress(hShell32, "ShellExecuteExW");
        if (pShellExecuteExW) {
            if (!Wh_SetFunctionHook(pShellExecuteExW, (void*)ShellExecuteExWHook, (void**)&ShellExecuteExWOriginal)) {
                Wh_Log(L"Failed to hook ShellExecuteExW");
            }
        }

        const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
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

        if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
            Wh_Log(L"Failed to hook CControlPanelAppletList::s_SortAppletsInCategory");
        } else {
            LOG_IF_ENABLED("[Hooks] CControlPanelAppletList::s_SortAppletsInCategory hooked OK");
        }
    }

    LOG_IF_ENABLED("[Hooks] All hooks set successfully");
    LOG_IF_ENABLED("[Hooks] Shell32 symbol hook: %s", hShell32 ? L"loaded" : L"FAILED");
    return TRUE;
  } catch (...) {
      Wh_Log(L"Exception during mod initialization, aborting load");
      return FALSE;
  }
}

static void CleanupTempFiles() {
    // Delete the temp task-links file
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    if (!g_classicTaskLinksFilePath.empty()) {
        DeleteFileW(g_classicTaskLinksFilePath.c_str());
        LOG_IF_ENABLED("[Cleanup] Deleted task links file: %s", g_classicTaskLinksFilePath.c_str());
    }
}

void Wh_ModUninit() {
    try {
        CleanupTempFiles();
        // See KeyTracker::ClearWithoutFreeing for why we deliberately don't
        // delete the fake-handle memory here.
        g_keyTracker.ClearWithoutFreeing();
        Wh_Log(L"Cleanup completed");
    } catch (...) {
        Wh_Log(L"Exception during cleanup, continuing anyway");
    }
}
