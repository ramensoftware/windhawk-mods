// ==WindhawkMod==
// @id              win7-default-programs-applet-restorer
// @name            Windows 7 Default Programs Restorer
// @description     This mod restores the classic Default Programs Control Panel pages on Windows 10 and 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinhttp -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -lcomctl32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Default Programs Restorer

## About
This mod restores the classic **Default Programs** pages of Control Panel: *Set your default programs*,
*Associate a file type or protocol with a program*, and *Set program access and computer defaults*.
The pages use Microsoft's original Windows 10 1607 `sud.dll`, not a recreation.

No system file is replaced or modified. Disabling this mod restores the Windows page, but does not undo
association changes made while it was enabled. See *Notes and limitations* below.

## Screenshots
![Default Programs page](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/default1.PNG)

![Set your default programs page](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/default2.PNG)

## What is downloaded, from where, and when
This mod contains no Microsoft binaries. It downloads files from Microsoft's public symbol server
(`https://msdl.microsoft.com/download/symbols/`) and keeps them in the mod's storage folder.

| File | Version | Used for |
|---|---|---|
| `sud.dll` | 10.0.14393.726 (Windows 10 1607, KB3216755) | The classic pages |
| `dui70.dll` | 10.0.15063.1155 (Windows 10 1703) | The DirectUI engine for the pages |
| `ComputerDefaults.exe` | Windows 10 1607 | The classic program access and computer defaults link |

Every file is pinned: the mod accepts a file only if its exact size and SHA-256 match the values built into the
mod (and its PE header matches the key on the server). This applies to the downloaded files, to the files already
in the `dl` folder and to a file placed there by hand, so a file that did not come from Microsoft is never processed
or loaded. `ComputerDefaults.exe` is checked in the same way, and again right before it is launched. If the
executable is unavailable or does not match, the original link is used instead.

**When the downloads happen.** The setup runs on a background thread when the mod loads (every `explorer.exe`
start) and every time the mod's settings change. A file is requested only if it is missing from `dl` or does not
match; up to 3 attempts are made per file. With *Keep downloaded files when the mod is disabled* turned off, `dl` is
deleted whenever the mod is unloaded, so the files are downloaded again after every disable, update or reload.
**No setting turns the downloads off.** The only alternative is to put the exact files in `dl` by hand. The mod itself makes only the
three requests to `msdl.microsoft.com` listed above; nothing user-specific is sent.

From the DLLs this mod generates private copies in its `bin` folder. English, Italian, German, Spanish, French and Brazilian Portuguese page strings are
embedded because the symbol server does not provide the `.mui` files. The original downloads are kept in `dl`;
they can be reused offline. If a required DLL cannot be downloaded, a matching file can be placed in `dl`
manually. Without the required files the Windows page stays in place; connect to the internet and restart
`explorer.exe` or toggle the mod.

## How it works
- **The page.** The first default-programs link uses the Windows 7-style icon embedded in this mod. This mod redirects the relevant Control Panel page definition to a private copy of `sud.dll`
  and creates the page's original UI from it. It does not replace the system copy.
- **Compatibility.** This mod applies hooks for changes in newer Windows components. On Windows 10 builds
  19041–19045 it also patches two interface IDs and three vtable offsets in the private page, based on the
  WinClassic 1607 fix. The hooks require symbols for Windows components and the private page. Windhawk may
  download matching symbols when they are not cached; an internet connection may be needed after installation
  or a Windows update. If symbols are unavailable, some compatibility fixes may not work.
- **Program access and computer defaults.** When available, the verified 1607 executable is used for this link.
- **Associations.** This mod attempts to use a matching Windows Shell handler to set file associations,
  then checks the observed result. Changes made by the Shell persist after the mod is disabled. The mod does
  not directly write `UserChoice` or an extension's `Software\Classes` key. If an association cannot be
  applied or confirmed, the mod offers to open Windows Default apps settings, where you can change it yourself.
- **Scope.** This mod runs in `explorer.exe`.

## Settings
- **Page language**: *Automatic* follows the Windows display language for English, Italian, German, Spanish, French and Brazilian Portuguese, or uses English
  for other languages. Restart `explorer.exe` if the page has already been used in this session.
- **Hide system UWP apps**, **Hide all Store/UWP apps**, **Additional apps to hide**, **Icon overrides**:
  These settings customize the applications and icons in the default programs list.
- **Keep downloaded files when the mod is disabled**: This setting is on by default, allowing the files to be
  reused without downloading them again.
- **Enable debug logging**: This setting records additional information for troubleshooting.

## Notes and limitations
- **Changing associations.** Windows may retain an existing protected `UserChoice`, and a Shell success result
  does not guarantee that a change persisted. Check the actual default after saving and after restarting Windows.
  If an operation fails or is only partly confirmed, a prompt offers to open Default apps settings. Opening
  Settings does not itself change any default; you must make the choice there. A prompt may be suppressed if
  another one is already open or was shown in the preceding 15 seconds.
- File types whose target program or matching Shell handler cannot be resolved may not be changed. URL protocol
  associations cannot be applied by the Shell-handler helper and must be changed in Windows Settings.
- If the generated DirectUI page cannot load, this mod may use an unmodified private copy instead. Its page
  strings may be missing or different. The log records the loading steps.
- This mod uses older pages with newer Windows components. Some features may not work or may stop working after
  a Windows update. The page and program access link have been tested on Windows 10 21H2 and Windows 11 24H2.
- Windows 10 1607 or later, or Windows 11; 64-bit (x64) only. A recent Windhawk version is required. DirectUI
  control accessibility names are in English. Private copies remain loaded until `explorer.exe` restarts.

## Credits
- **repensky** for the work on restoring the classic Default Programs pages.
- **AdministratoX** (AdmXP8) for suggesting the 1607 `sud.dll` with the Windows 10 1703 DirectUI and for
  the technique of using older Control Panel pages from Microsoft's symbol server.
- **RandomGuy5747** for the WinClassic restoration work, the 19041–19045 compatibility patches, the German translation and the Windows 7-style icon.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Page language
  $name:it-IT: Lingua della pagina
  $description: This setting chooses the language of the restored page. "Automatic" follows the Windows display language.
  $description:it-IT: Questa impostazione sceglie la lingua della pagina ripristinata. "Automatica" segue la lingua di visualizzazione di Windows.
  $options:
    - auto: Automatic (Windows display language)
    - en-US: English
    - it-IT: Italiano
    - de-DE: Deutsch
    - es-ES: Español
    - fr-FR: Français
    - pt-BR: Português (Brasil)
- hideSystemApps: true
  $name: Hide system UWP apps
  $name:it-IT: Nascondi le app UWP di sistema
  $description: This setting hides internal Windows components (like MS-SEARCH, MS-ACTIONCENTER) that only register URL protocol associations from the Default Programs list
  $description:it-IT: Questa impostazione nasconde dall'elenco Programmi predefiniti i componenti interni di Windows (come MS-SEARCH, MS-ACTIONCENTER) che registrano solo associazioni di protocolli URL
- hideStoreApps: true
  $name: Hide all Store/UWP apps
  $name:it-IT: Nascondi tutte le app Store/UWP
  $description: This setting hides all Microsoft Store and UWP applications from the Default Programs list, keeping only Win32 desktop apps
  $description:it-IT: Questa impostazione nasconde dall'elenco Programmi predefiniti tutte le applicazioni Microsoft Store e UWP, lasciando solo le app desktop Win32
- additionalBlacklist:
  - Insert an app name here
  $name: Additional apps to hide
  $name:it-IT: App aggiuntive da nascondere
  $description: This setting lists app display names, registry names, or capability path substrings to hide from the Default Programs list. Matches against the display name shown in the Programs list, the app registry name, or capabilities path (case-insensitive).
  $description:it-IT: Questa impostazione elenca i nomi visualizzati, i nomi di registro o le parti del percorso delle capacità delle app da nascondere dall'elenco Programmi predefiniti (senza distinzione tra maiuscole e minuscole).
- iconOverrides:
  - - appName: Windows Disc Image Burner
      $name: App name
      $name:it-IT: Nome app
      $description: This setting specifies the app display name as shown in the Default Programs list, or the app registry name. Case-insensitive match.
      $description:it-IT: Questa impostazione specifica il nome dell'app come appare nell'elenco Programmi predefiniti, o il nome di registro. Senza distinzione tra maiuscole e minuscole.
    - iconPath: C:\Windows\System32\imageres.dll
      $name: Icon DLL/EXE path
      $name:it-IT: Percorso DLL/EXE dell'icona
      $description: This setting specifies the full path to the DLL or EXE containing the icon resource.
      $description:it-IT: Questa impostazione specifica il percorso completo della DLL o dell'EXE che contiene l'icona.
    - iconIndex: -5205
      $name: Icon resource index
      $name:it-IT: Indice dell'icona
      $description: This setting specifies the icon resource ID (negative for resource ID, positive for ordinal).
      $description:it-IT: Questa impostazione specifica l'ID della risorsa icona (negativo per ID di risorsa, positivo per numero d'ordine).
  $name: Icon overrides
  $name:it-IT: Icone personalizzate
  $description: This setting overrides icons for specific apps in the Default Programs list.
  $description:it-IT: Questa impostazione sostituisce le icone di app specifiche nell'elenco Programmi predefiniti.
- keepFilesOnDisable: true
  $name: Keep downloaded files when the mod is disabled
  $name:it-IT: Conserva i file scaricati quando la mod viene disattivata
  $description: This setting decides whether the downloaded and generated files stay in the mod's storage folder. If on, they stay, so enabling the mod again works offline. If off, they are deleted when the mod is unloaded (files still in use are left).
  $description:it-IT: Questa impostazione decide se i file scaricati e generati restano nella cartella della mod. Se attiva, restano, così la riattivazione funziona senza connessione. Se disattiva, vengono eliminati allo scaricamento della mod (i file ancora in uso restano).
- enableLogging: false
  $name: Enable debug logging
  $name:it-IT: Abilita il log di debug
  $description: This setting logs hook activity for debugging purposes
  $description:it-IT: Questa impostazione registra l'attività degli hook a scopo di debug
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <winhttp.h>
#include <objbase.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <commctrl.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

using Bytes = std::vector<uint8_t>;

// =====================================================================================================================
// Constants
// =====================================================================================================================

// Bump when the generated files change, so that a new folder is used (a loaded file cannot be replaced).
constexpr wchar_t kGeneratorRevision[] = L"r9-win11-compat";
constexpr wchar_t kPrivateDuiName[] = L"dpdui.dll";        // same length as "DUI70.dll"
constexpr char kDuiNew[] = "dpdui.dll";
constexpr wchar_t kSymbolHost[] = L"msdl.microsoft.com";
constexpr wchar_t kSymbolPath[] = L"/download/symbols/";
constexpr DWORD kMinBuild = 14393;                         // Windows 10 1607

struct FileSpec {
    const wchar_t* name;      // file name in <storage>\dl
    const wchar_t* key;       // path below the symbol server's root: name/TimeDateStamp+SizeOfImage/name
    uint32_t maxSize;         // a download larger than this is not the file
    uint32_t timestamp;       // PE TimeDateStamp and SizeOfImage: what the key says the file is
    uint32_t imageSize;
    const wchar_t* altName;   // another name accepted in <storage>\dl (a pre-renamed DirectUI file)
    const char* sha256;       // pinned SHA-256 of the whole file (lowercase hex); a file without a pin is never accepted
    uint32_t fileSize;        // exact size in bytes (0 = not pinned; the SHA-256 still decides)
};

// Pinned values of the Microsoft files. Get them with: Get-FileHash <file> -Algorithm SHA256; (Get-Item <file>).Length
constexpr char kSudSha256[] = "c47fce44d5cab0d7c09790c252fd6f12f4c56bafc72e035a150d3955b3c60740";
constexpr uint32_t kSudFileSize = 655872;   // sud.dll 10.0.14393.726 (x64)
constexpr char kDuiSha256[] = "11e0fd294026eca9261d4fb6d39625ac5de84c85ec3d6fcf031bc72cfb9463d3"; // dui70.dll 10.0.15063.1155 (x64)
constexpr uint32_t kDuiFileSize = 1723392;
constexpr uint32_t kSpadFileSize = 50176;   // ComputerDefaults.exe, Windows 10 1607 (x64)
constexpr char kSpadSha256[] = "4978ad7650c44d4239ed6b77267dd21d50d33bbd3d875ace4131f2ded3a11804";

constexpr FileSpec kFiles[] = {
    {L"sud.dll", L"sud.dll/58785909a5000/sud.dll", 4u << 20, 0x58785909u, 0xA5000u, nullptr, kSudSha256, kSudFileSize},
    {L"dui70.dll", L"dui70.dll/9ECBA8E71ab000/dui70.dll", 4u << 20, 0x9ECBA8E7u, 0x1AB000u, L"dui17.dll", kDuiSha256, kDuiFileSize},
};
enum { kFileSud = 0, kFileDui = 1 };

// Classes of the page, as Windows registers them (served by System32\sud.dll).
constexpr GUID kClsidElementProvider = {0xD4EFF9CD, 0x16DE, 0x446e, {0x83, 0xC5, 0x95, 0x37, 0x54, 0x3C, 0xF4, 0xA1}};
constexpr GUID kClsidAssocRegUi = {0x1968106d, 0xf3b5, 0x44cf, {0x89, 0x0e, 0x11, 0x6f, 0xcb, 0x9e, 0xce, 0xf1}};
constexpr GUID kIidClassFactory = {0x00000001, 0x0000, 0x0000, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};

inline bool Same(const GUID& a, const GUID& b) { return memcmp(&a, &b, sizeof(GUID)) == 0; }

// =====================================================================================================================
// Strings of the pages (sud.dll.mui 10.0.14393.726 and dui70.dll.mui 10.0.15063.0; the strings are the same in the downloaded 14393.726 and 15063.1155)
// =====================================================================================================================

// <tables-begin>
struct StringEntry {
    uint16_t id;
    const wchar_t* text;
};
struct StringTable {
    const StringEntry* entries;
    size_t count;
    const wchar_t* tag;       // part of the generated file's name
    const wchar_t* find(uint32_t id) const {
        size_t lo = 0, hi = count;
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (entries[mid].id < id) lo = mid + 1; else hi = mid;
        }
        return lo < count && entries[lo].id == id ? entries[lo].text : nullptr;
    }
};
// <tables-end>

// Generated from the .mui files; the Italian text is the mod's own translation. Sorted by id.
static const StringEntry kSudEn[] = {
    {1, L"Default Programs"},
    {2, L"Set Associations"},
    {3, L"Choose a default"},
    {4, L"Set Default Programs"},
    {7, L"Set Program Associations"},
    {10, L"Choose which programs you want Windows to use for activities like web browsing, editing photos, sending e-mail, and playing music."},
    {200, L"Program Defaults Control Panel Error"},
    {201, L"There was a problem loading the 'Set a Default Program' page. (Error code: %1!lX!)"},
    {202, L"(Select a program for more information)"},
    {203, L"Name"},
    {204, L"Description"},
    {205, L"Current Default"},
    {206, L"Programs"},
    {210, L"Extensions"},
    {211, L"Protocols"},
    {212, L"MAPI"},
    {220, L"Send mail"},
    {221, L"Send mail command"},
    {222, L"Unknown application"},
    {223, L"This program has all its defaults"},
    {224, L"This program has %1!lu! out of %2!lu! defaults"},
    {225, L"Not selected"},
    {226, L"Use the selected program to open all file types and protocols it can open by default."},
    {227, L"Choose which file types and protocols the selected program opens by default."},
    {228, L"Programs and Features"},
    {229, L"Parental Controls"},
    {231, L"To change your default apps, go to Settings > System > Default apps."},
    {232, L"OK"},
    {240, L"Make a file type or protocol (such as .mp3 or http://) always open in a specific program."},
    {1169, L"Choose the programs that Windows uses by default"},
    {1170, L"Set your default programs"},
    {1171, L"Make a program the default for all file types and protocols it can open."},
    {1172, L"Associate a file type or protocol with a program"},
    {1173, L"Make a file type or protocol (such as .mp3 or http://) always open in a specific program."},
    {1174, L"Change AutoPlay settings"},
    {1175, L"Play CDs or other media automatically"},
    {1176, L"Set program access and computer defaults"},
    {1177, L"Control access to certain programs and set defaults for this computer."},
    {1180, L"OK"},
    {1183, L"Set your default programs"},
    {1184, L"Set associations for a program"},
    {1185, L"Select the extensions you want this program to open by default, and then click Save."},
    {1186, L"Save"},
    {1187, L"Cancel"},
    {1190, L"Programs List"},
    {1192, L"Select All"},
    {1193, L"To set a program as the default for all file types and protocols it can open, click the program and then click Set as default."},
    {1194, L"Associate a file type or protocol with a specific program"},
    {1195, L"Click on an extension to view the program that currently opens it by default. To change the default program, click Change program."},
    {1196, L"Change program..."},
    {1197, L"No extension or protocol selected"},
    {1198, L"Close"},
    {1199, L"List of available programs to change default settings."},
    {1200, L"Program description"},
    {1201, L"Program Association List"},
    {1202, L"Select the extensions you want this program to open by default, and then click Save."},
    {1203, L"No extension or protocol selected"},
    {1205, L"Extensions List"},
    {1206, L"Click on an extension to view the program that currently opens it by default. To change the default program, click Change program."},
    {1207, L"&Set this program as default"},
    {1208, L"&Choose defaults for this program"},
    {1209, L"Set your default programs"},
    {1210, L"Associate a file type or protocol with a program"},
    {1211, L"Change AutoPlay settings"},
    {1212, L"Set program access and computer defaults"},
    {1213, L"Default Programs"},
    {1214, L"Default Programs"},
    {1216, L"Set your default programs"},
    {1217, L"Set your default programs"},
    {1218, L"Set associations for a program"},
    {1219, L"Set associations for a program"},
    {1220, L"Associate a file type or protocol with a specific program"},
    {1221, L"Associate a file type or protocol with a specific program"},
    {1224, L"Help"},
    {1225, L"Help"},
    {1226, L"Help"},
    {1230, L"Loading..."},
    {1231, L"Loading..."},
};
static const StringEntry kSudIt[] = {
    {1, L"Programmi predefiniti"},
    {2, L"Imposta associazioni"},
    {3, L"Scegli un programma predefinito"},
    {4, L"Imposta programmi predefiniti"},
    {7, L"Imposta associazioni programma"},
    {10, L"Scegliere i programmi da utilizzare in Windows per attivit\u00e0 quali esplorazione Web, modifica di foto, invio di posta elettronica e riproduzione di musica."},
    {200, L"Errore del Pannello di controllo Programmi predefiniti"},
    {201, L"Si \u00e8 verificato un problema durante il caricamento della pagina 'Imposta programma predefinito'. (Codice errore: %1!lX!)"},
    {202, L"(Selezionare un programma per ulteriori informazioni)"},
    {203, L"Nome"},
    {204, L"Descrizione"},
    {205, L"Predefinito corrente"},
    {206, L"Programmi"},
    {210, L"Estensioni"},
    {211, L"Protocolli"},
    {212, L"MAPI"},
    {220, L"Invia posta"},
    {221, L"Comando Invia posta"},
    {222, L"Applicazione sconosciuta"},
    {223, L"Questo programma ha tutti i valori predefiniti"},
    {224, L"Questo programma ha %1!lu! valori predefiniti su %2!lu!"},
    {225, L"Non selezionato"},
    {226, L"Utilizzare il programma selezionato per aprire tutti i tipi di file e i protocolli che pu\u00f2 aprire per impostazione predefinita."},
    {227, L"Scegliere i tipi di file e i protocolli che il programma selezionato apre per impostazione predefinita."},
    {228, L"Programmi e funzionalit\u00e0"},
    {229, L"Controllo genitori"},
    {231, L"Per modificare le app predefinite, andare in Impostazioni > Sistema > App predefinite."},
    {232, L"OK"},
    {240, L"Fare in modo che un tipo di file o un protocollo (ad esempio .mp3 o http://) venga sempre aperto in un programma specifico."},
    {1169, L"Scegliere i programmi utilizzati da Windows per impostazione predefinita"},
    {1170, L"Imposta programmi predefiniti"},
    {1171, L"Impostare un programma come predefinito per tutti i tipi di file e i protocolli che pu\u00f2 aprire."},
    {1172, L"Associa un tipo di file o un protocollo a un programma"},
    {1173, L"Fare in modo che un tipo di file o un protocollo (ad esempio .mp3 o http://) venga sempre aperto in un programma specifico."},
    {1174, L"Modifica impostazioni AutoPlay"},
    {1175, L"Riproduci automaticamente CD o altri supporti"},
    {1176, L"Imposta accesso ai programmi e impostazioni predefinite del computer"},
    {1177, L"Controlla l'accesso a determinati programmi e imposta i valori predefiniti per questo computer."},
    {1180, L"OK"},
    {1183, L"Imposta programmi predefiniti"},
    {1184, L"Imposta associazioni per un programma"},
    {1185, L"Selezionare le estensioni che si desidera far aprire per impostazione predefinita a questo programma, quindi fare clic su Salva."},
    {1186, L"Salva"},
    {1187, L"Annulla"},
    {1190, L"Elenco programmi"},
    {1192, L"Seleziona tutto"},
    {1193, L"Per impostare un programma come predefinito per tutti i tipi di file e i protocolli che pu\u00f2 aprire, fare clic sul programma e quindi su Imposta come predefinito."},
    {1194, L"Associa un tipo di file o un protocollo a un programma specifico"},
    {1195, L"Fare clic su un'estensione per visualizzare il programma che attualmente la apre per impostazione predefinita. Per cambiare il programma predefinito, fare clic su Cambia programma."},
    {1196, L"Cambia programma..."},
    {1197, L"Nessuna estensione o protocollo selezionato"},
    {1198, L"Chiudi"},
    {1199, L"Elenco dei programmi disponibili per cambiare le impostazioni predefinite."},
    {1200, L"Descrizione del programma"},
    {1201, L"Elenco associazioni programma"},
    {1202, L"Selezionare le estensioni che si desidera far aprire per impostazione predefinita a questo programma, quindi fare clic su Salva."},
    {1203, L"Nessuna estensione o protocollo selezionato"},
    {1205, L"Elenco estensioni"},
    {1206, L"Fare clic su un'estensione per visualizzare il programma che attualmente la apre per impostazione predefinita. Per cambiare il programma predefinito, fare clic su Cambia programma."},
    {1207, L"&Imposta questo programma come predefinito"},
    {1208, L"&Scegli le impostazioni predefinite per questo programma"},
    {1209, L"Imposta programmi predefiniti"},
    {1210, L"Associa un tipo di file o un protocollo a un programma"},
    {1211, L"Modifica impostazioni AutoPlay"},
    {1212, L"Imposta accesso ai programmi e impostazioni predefinite del computer"},
    {1213, L"Programmi predefiniti"},
    {1214, L"Programmi predefiniti"},
    {1216, L"Imposta programmi predefiniti"},
    {1217, L"Imposta programmi predefiniti"},
    {1218, L"Imposta associazioni per un programma"},
    {1219, L"Imposta associazioni per un programma"},
    {1220, L"Associa un tipo di file o un protocollo a un programma specifico"},
    {1221, L"Associa un tipo di file o un protocollo a un programma specifico"},
    {1224, L"Guida"},
    {1225, L"Guida"},
    {1226, L"Guida"},
    {1230, L"Caricamento in corso..."},
    {1231, L"Caricamento in corso..."},
};
// German page strings provided by RandomGuy5747 on WinClassic.
static const StringEntry kSudDe[] = {
    {1, L"Standardprogramme"},
    {2, L"Zuordnungen festlegen"},
    {3, L"Standardprogramm ausw\u00e4hlen"},
    {4, L"Standardprogramme festlegen"},
    {7, L"Programmzuordnungen festlegen"},
    {10, L"W\u00e4hlen Sie die Programme aus, die Windows standardm\u00e4\u00dfig f\u00fcr Aktivit\u00e4ten wie Surfen im Internet, Bearbeiten von Fotos, Senden von E-Mails und Wiedergeben von Musik verwenden soll."},
    {200, L"Fehler in der Systemsteuerung f\u00fcr Standardprogramme"},
    {201, L"Beim Laden der Seite \u201eStandardprogramm festlegen\u201c ist ein Problem aufgetreten. (Fehlercode: %1!lX!)"},
    {202, L"(W\u00e4hlen Sie ein Programm aus, um weitere Informationen anzuzeigen)"},
    {203, L"Name"},
    {204, L"Beschreibung"},
    {205, L"Aktueller Standard"},
    {206, L"Programme"},
    {210, L"Erweiterungen"},
    {211, L"Protokolle"},
    {212, L"MAPI"},
    {220, L"E-Mail senden"},
    {221, L"Befehl zum Senden von E-Mails"},
    {222, L"Unbekannte Anwendung"},
    {223, L"F\u00fcr dieses Programm sind alle Standardwerte festgelegt"},
    {224, L"F\u00fcr dieses Programm sind %1!lu! von %2!lu! Standardwerten festgelegt"},
    {225, L"Nicht ausgew\u00e4hlt"},
    {226, L"Das ausgew\u00e4hlte Programm standardm\u00e4\u00dfig zum \u00d6ffnen aller Dateitypen und Protokolle verwenden, die es \u00f6ffnen kann."},
    {227, L"W\u00e4hlen Sie aus, welche Dateitypen und Protokolle das ausgew\u00e4hlte Programm standardm\u00e4\u00dfig \u00f6ffnet."},
    {228, L"Programme und Features"},
    {229, L"Jugendschutz"},
    {231, L"Um Ihre Standard-Apps zu \u00e4ndern, wechseln Sie zu Einstellungen > System > Standard-Apps."},
    {232, L"OK"},
    {240, L"Festlegen, dass ein Dateityp oder Protokoll (z. B. .mp3 oder http://) immer in einem bestimmten Programm ge\u00f6ffnet wird."},
    {1169, L"Programme ausw\u00e4hlen, die Windows standardm\u00e4\u00dfig verwendet"},
    {1170, L"Standardprogramme festlegen"},
    {1171, L"Ein Programm als Standard f\u00fcr alle Dateitypen und Protokolle festlegen, die es \u00f6ffnen kann."},
    {1172, L"Dateityp oder Protokoll einem Programm zuordnen"},
    {1173, L"Festlegen, dass ein Dateityp oder Protokoll (z. B. .mp3 oder http://) immer in einem bestimmten Programm ge\u00f6ffnet wird."},
    {1174, L"AutoPlay-Einstellungen \u00e4ndern"},
    {1175, L"CDs oder andere Medien automatisch wiedergeben"},
    {1176, L"Programmzugriff und Computerstandards festlegen"},
    {1177, L"Zugriff auf bestimmte Programme steuern und Standardwerte f\u00fcr diesen Computer festlegen."},
    {1180, L"OK"},
    {1183, L"Standardprogramme festlegen"},
    {1184, L"Zuordnungen f\u00fcr ein Programm festlegen"},
    {1185, L"W\u00e4hlen Sie die Erweiterungen aus, die dieses Programm standardm\u00e4\u00dfig \u00f6ffnen soll, und klicken Sie dann auf \u201eSpeichern\u201c."},
    {1186, L"Speichern"},
    {1187, L"Abbrechen"},
    {1190, L"Programmliste"},
    {1192, L"Alle ausw\u00e4hlen"},
    {1193, L"Um ein Programm als Standard f\u00fcr alle Dateitypen und Protokolle festzulegen, die es \u00f6ffnen kann, klicken Sie auf das Programm und dann auf \u201eProgramm als Standard festlegen\u201c."},
    {1194, L"Einen Dateityp oder ein Protokoll einem bestimmten Programm zuordnen"},
    {1195, L"Klicken Sie auf eine Erweiterung, um das Programm anzuzeigen, das sie derzeit standardm\u00e4\u00dfig \u00f6ffnet. Klicken Sie auf \u201eProgramm \u00e4ndern\u201c, um das Standardprogramm zu \u00e4ndern."},
    {1196, L"Programm \u00e4ndern..."},
    {1197, L"Keine Erweiterung oder kein Protokoll ausgew\u00e4hlt"},
    {1198, L"Schlie\u00dfen"},
    {1199, L"Liste der verf\u00fcgbaren Programme zum \u00c4ndern der Standardeinstellungen."},
    {1200, L"Programmbeschreibung"},
    {1201, L"Liste der Programmzuordnungen"},
    {1202, L"W\u00e4hlen Sie die Erweiterungen aus, die dieses Programm standardm\u00e4\u00dfig \u00f6ffnen soll, und klicken Sie dann auf \u201eSpeichern\u201c."},
    {1203, L"Keine Erweiterung oder kein Protokoll ausgew\u00e4hlt"},
    {1205, L"Liste der Erweiterungen"},
    {1206, L"Klicken Sie auf eine Erweiterung, um das Programm anzuzeigen, das sie derzeit standardm\u00e4\u00dfig \u00f6ffnet. Klicken Sie auf \u201eProgramm \u00e4ndern\u201c, um das Standardprogramm zu \u00e4ndern."},
    {1207, L"&Programm als Standard festlegen"},
    {1208, L"&Standardwerte f\u00fcr dieses Programm ausw\u00e4hlen"},
    {1209, L"Standardprogramme festlegen"},
    {1210, L"Dateityp oder Protokoll einem Programm zuordnen"},
    {1211, L"AutoPlay-Einstellungen \u00e4ndern"},
    {1212, L"Programmzugriff und Computerstandards festlegen"},
    {1213, L"Standardprogramme"},
    {1214, L"Standardprogramme"},
    {1216, L"Standardprogramme festlegen"},
    {1217, L"Standardprogramme festlegen"},
    {1218, L"Zuordnungen f\u00fcr ein Programm festlegen"},
    {1219, L"Zuordnungen f\u00fcr ein Programm festlegen"},
    {1220, L"Einen Dateityp oder ein Protokoll einem bestimmten Programm zuordnen"},
    {1221, L"Einen Dateityp oder ein Protokoll einem bestimmten Programm zuordnen"},
    {1224, L"Hilfe"},
    {1225, L"Hilfe"},
    {1226, L"Hilfe"},
    {1230, L"Wird geladen..."},
    {1231, L"Wird geladen..."},
};

static const StringEntry kDuiStrings[] = {
    {101, L"Progress meter"},
    {102, L"Busy indicator"},
    {103, L"%d%%"},
    {104, L"Press"},
    {105, L"Check"},
    {106, L"Uncheck"},
    {107, L"11pt;Normal;None;Segoe UI"},
    {108, L"11pt;Normal;None;Segoe UI Symbol"},
    {109, L"\ue0a4"},
    {110, L"\ue052"},
    {112, L"11pt;Semibold;None;Segoe UI"},
    {113, L"11pt;Normal;None;Segoe UI"},
    {114, L"14pt;Normal;None;Segoe UI Symbol"},
    {115, L"\ue001"},
    {116, L"11pt;Normal;None;Segoe UI"},
    {117, L"11pt;Normal;None;Segoe UI Symbol"},
    {118, L"\ue019"},
    {120, L"11pt;Semilight;None;Segoe UI"},
    {121, L"11pt;Semibold;None;Segoe UI"},
    {122, L"Touchswitch"},
    {123, L"Touchslider"},
    {124, L"Buffering"},
    {125, L"Thumb"},
    {126, L"11pt;Normal;None;Segoe UI"},
    {130, L"16pt;Normal;None;Segoe UI Symbol"},
    {131, L"\ue010"},
    {132, L"\ue011"},
    {133, L"\ue00e"},
    {134, L"\ue00f"},
    {135, L"20pt;Light;None;Segoe UI"},
    {136, L"20pt;Light;None;Segoe UI"},
    {137, L"20pt;Light;None;Segoe UI Symbol"},
    {138, L"\ue099"},
    {140, L"Slider"},
    {150, L"Clear"},
    {151, L"Password reveal"},
    {160, L"11pt;Semilight;None;Segoe UI"},
    {170, L"11pt;Semibold;None;Segoe UI"},
    {171, L"11pt;Light;None;Segoe UI"},
    {300, L"11pt;Semilight;None;Segoe UI"},
    {301, L"11pt;semibold;None;Segoe UI"},
    {302, L"20pt;light;None;Segoe UI"},
    {303, L"11pt;Semilight;None;Segoe UI"},
    {304, L"11pt;Semilight;None;Segoe UI"},
    {305, L"11pt;normal;None;Segoe UI"},
    {306, L"11pt;Normal;None;Segoe UI"},
    {1000, L"9pt;Normal;None;Segoe UI"},
    {1001, L"11pt;Semibold;None;Segoe UI"},
};

static const StringEntry kSudEs[] = {
    {1, L"Programas predeterminados"},
    {2, L"Establecer asociaciones"},
    {3, L"Elegir un valor predeterminado"},
    {4, L"Establecer programas predeterminados"},
    {7, L"Establecer asociaciones de programas"},
    {10, L"Elige los programas que Windows usar\u00e1 para navegar por Internet, editar fotos, enviar correo electr\u00f3nico y reproducir m\u00fasica."},
    {200, L"Error del Panel de control de programas predeterminados"},
    {201, L"Se produjo un problema al cargar la p\u00e1gina \u00abEstablecer un programa predeterminado\u00bb. (C\u00f3digo de error: %1!lX!)"},
    {202, L"(Selecciona un programa para obtener m\u00e1s informaci\u00f3n)"},
    {203, L"Nombre"},
    {204, L"Descripci\u00f3n"},
    {205, L"Predeterminado actual"},
    {206, L"Programas"},
    {210, L"Extensiones"},
    {211, L"Protocolos"},
    {212, L"MAPI"},
    {220, L"Enviar correo"},
    {221, L"Comando para enviar correo"},
    {222, L"Aplicaci\u00f3n desconocida"},
    {223, L"Este programa tiene todos sus valores predeterminados"},
    {224, L"Este programa tiene %1!lu! de %2!lu! valores predeterminados"},
    {225, L"No seleccionado"},
    {226, L"Usar el programa seleccionado para abrir de forma predeterminada todos los tipos de archivo y protocolos que admite."},
    {227, L"Elige los tipos de archivo y protocolos que el programa seleccionado abrir\u00e1 de forma predeterminada."},
    {228, L"Programas y caracter\u00edsticas"},
    {229, L"Control parental"},
    {231, L"Para cambiar las aplicaciones predeterminadas, ve a Configuraci\u00f3n > Sistema > Aplicaciones predeterminadas."},
    {232, L"Aceptar"},
    {240, L"Hacer que un tipo de archivo o protocolo (por ejemplo, .mp3 o http://) se abra siempre con un programa espec\u00edfico."},
    {1169, L"Elegir los programas que Windows usa de forma predeterminada"},
    {1170, L"Establecer programas predeterminados"},
    {1171, L"Establecer un programa como predeterminado para todos los tipos de archivo y protocolos que puede abrir."},
    {1172, L"Asociar un tipo de archivo o protocolo a un programa"},
    {1173, L"Hacer que un tipo de archivo o protocolo (por ejemplo, .mp3 o http://) se abra siempre con un programa espec\u00edfico."},
    {1174, L"Cambiar la configuraci\u00f3n de reproducci\u00f3n autom\u00e1tica"},
    {1175, L"Reproducir CD u otros medios autom\u00e1ticamente"},
    {1176, L"Configurar el acceso y los programas predeterminados del equipo"},
    {1177, L"Controlar el acceso a determinados programas y establecer los valores predeterminados de este equipo."},
    {1180, L"Aceptar"},
    {1183, L"Establecer programas predeterminados"},
    {1184, L"Establecer asociaciones para un programa"},
    {1185, L"Selecciona las extensiones que quieres que este programa abra de forma predeterminada y haz clic en Guardar."},
    {1186, L"Guardar"},
    {1187, L"Cancelar"},
    {1190, L"Lista de programas"},
    {1192, L"Seleccionar todo"},
    {1193, L"Para establecer un programa como predeterminado para todos los tipos de archivo y protocolos que puede abrir, selecci\u00f3nalo y haz clic en Establecer como predeterminado."},
    {1194, L"Asociar un tipo de archivo o protocolo a un programa espec\u00edfico"},
    {1195, L"Haz clic en una extensi\u00f3n para ver el programa que la abre actualmente de forma predeterminada. Para cambiarlo, haz clic en Cambiar programa."},
    {1196, L"Cambiar programa..."},
    {1197, L"No se ha seleccionado ninguna extensi\u00f3n o protocolo"},
    {1198, L"Cerrar"},
    {1199, L"Lista de programas disponibles para cambiar la configuraci\u00f3n predeterminada."},
    {1200, L"Descripci\u00f3n del programa"},
    {1201, L"Lista de asociaciones del programa"},
    {1202, L"Selecciona las extensiones que quieres que este programa abra de forma predeterminada y haz clic en Guardar."},
    {1203, L"No se ha seleccionado ninguna extensi\u00f3n o protocolo"},
    {1205, L"Lista de extensiones"},
    {1206, L"Haz clic en una extensi\u00f3n para ver el programa que la abre actualmente de forma predeterminada. Para cambiarlo, haz clic en Cambiar programa."},
    {1207, L"&Establecer este programa como predeterminado"},
    {1208, L"&Elegir opciones predeterminadas para este programa"},
    {1209, L"Establecer programas predeterminados"},
    {1210, L"Asociar un tipo de archivo o protocolo a un programa"},
    {1211, L"Cambiar la configuraci\u00f3n de reproducci\u00f3n autom\u00e1tica"},
    {1212, L"Configurar el acceso y los programas predeterminados del equipo"},
    {1213, L"Programas predeterminados"},
    {1214, L"Programas predeterminados"},
    {1216, L"Establecer programas predeterminados"},
    {1217, L"Establecer programas predeterminados"},
    {1218, L"Establecer asociaciones para un programa"},
    {1219, L"Establecer asociaciones para un programa"},
    {1220, L"Asociar un tipo de archivo o protocolo a un programa espec\u00edfico"},
    {1221, L"Asociar un tipo de archivo o protocolo a un programa espec\u00edfico"},
    {1224, L"Ayuda"},
    {1225, L"Ayuda"},
    {1226, L"Ayuda"},
    {1230, L"Cargando..."},
    {1231, L"Cargando..."},
};

static const StringEntry kSudFr[] = {
    {1, L"Programmes par d\u00e9faut"},
    {2, L"D\u00e9finir les associations"},
    {3, L"Choisir une valeur par d\u00e9faut"},
    {4, L"D\u00e9finir les programmes par d\u00e9faut"},
    {7, L"D\u00e9finir les associations de programmes"},
    {10, L"Choisissez les programmes que Windows utilise pour naviguer sur le Web, modifier des photos, envoyer des e-mails et \u00e9couter de la musique."},
    {200, L"Erreur du Panneau de configuration des programmes par d\u00e9faut"},
    {201, L"Un probl\u00e8me s'est produit lors du chargement de la page \u00ab D\u00e9finir un programme par d\u00e9faut \u00bb. (Code d'erreur : %1!lX!)"},
    {202, L"(S\u00e9lectionnez un programme pour plus d'informations)"},
    {203, L"Nom"},
    {204, L"Description"},
    {205, L"Programme par d\u00e9faut actuel"},
    {206, L"Programmes"},
    {210, L"Extensions"},
    {211, L"Protocoles"},
    {212, L"MAPI"},
    {220, L"Envoyer un courrier"},
    {221, L"Commande d'envoi de courrier"},
    {222, L"Application inconnue"},
    {223, L"Ce programme poss\u00e8de toutes ses valeurs par d\u00e9faut"},
    {224, L"Ce programme poss\u00e8de %1!lu! valeurs par d\u00e9faut sur %2!lu!"},
    {225, L"Non s\u00e9lectionn\u00e9"},
    {226, L"Utiliser le programme s\u00e9lectionn\u00e9 pour ouvrir par d\u00e9faut tous les types de fichiers et protocoles qu'il prend en charge."},
    {227, L"Choisissez les types de fichiers et les protocoles que le programme s\u00e9lectionn\u00e9 ouvre par d\u00e9faut."},
    {228, L"Programmes et fonctionnalit\u00e9s"},
    {229, L"Contr\u00f4le parental"},
    {231, L"Pour modifier les applications par d\u00e9faut, acc\u00e9dez \u00e0 Param\u00e8tres > Syst\u00e8me > Applications par d\u00e9faut."},
    {232, L"OK"},
    {240, L"Toujours ouvrir un type de fichier ou un protocole (par exemple .mp3 ou http://) avec un programme sp\u00e9cifique."},
    {1169, L"Choisir les programmes utilis\u00e9s par d\u00e9faut par Windows"},
    {1170, L"D\u00e9finir les programmes par d\u00e9faut"},
    {1171, L"D\u00e9finir un programme par d\u00e9faut pour tous les types de fichiers et protocoles qu'il peut ouvrir."},
    {1172, L"Associer un type de fichier ou un protocole \u00e0 un programme"},
    {1173, L"Toujours ouvrir un type de fichier ou un protocole (par exemple .mp3 ou http://) avec un programme sp\u00e9cifique."},
    {1174, L"Modifier les param\u00e8tres de lecture automatique"},
    {1175, L"Lire automatiquement les CD ou autres m\u00e9dias"},
    {1176, L"Configurer les programmes par d\u00e9faut et l'acc\u00e8s aux programmes"},
    {1177, L"Contr\u00f4ler l'acc\u00e8s \u00e0 certains programmes et d\u00e9finir les valeurs par d\u00e9faut de cet ordinateur."},
    {1180, L"OK"},
    {1183, L"D\u00e9finir les programmes par d\u00e9faut"},
    {1184, L"D\u00e9finir les associations d'un programme"},
    {1185, L"S\u00e9lectionnez les extensions que ce programme doit ouvrir par d\u00e9faut, puis cliquez sur Enregistrer."},
    {1186, L"Enregistrer"},
    {1187, L"Annuler"},
    {1190, L"Liste des programmes"},
    {1192, L"Tout s\u00e9lectionner"},
    {1193, L"Pour d\u00e9finir un programme par d\u00e9faut pour tous les types de fichiers et protocoles qu'il peut ouvrir, s\u00e9lectionnez-le, puis cliquez sur D\u00e9finir par d\u00e9faut."},
    {1194, L"Associer un type de fichier ou un protocole \u00e0 un programme sp\u00e9cifique"},
    {1195, L"Cliquez sur une extension pour voir le programme qui l'ouvre actuellement par d\u00e9faut. Pour le modifier, cliquez sur Changer de programme."},
    {1196, L"Changer de programme..."},
    {1197, L"Aucune extension ni aucun protocole s\u00e9lectionn\u00e9"},
    {1198, L"Fermer"},
    {1199, L"Liste des programmes disponibles pour modifier les param\u00e8tres par d\u00e9faut."},
    {1200, L"Description du programme"},
    {1201, L"Liste des associations du programme"},
    {1202, L"S\u00e9lectionnez les extensions que ce programme doit ouvrir par d\u00e9faut, puis cliquez sur Enregistrer."},
    {1203, L"Aucune extension ni aucun protocole s\u00e9lectionn\u00e9"},
    {1205, L"Liste des extensions"},
    {1206, L"Cliquez sur une extension pour voir le programme qui l'ouvre actuellement par d\u00e9faut. Pour le modifier, cliquez sur Changer de programme."},
    {1207, L"&D\u00e9finir ce programme par d\u00e9faut"},
    {1208, L"&Choisir les param\u00e8tres par d\u00e9faut de ce programme"},
    {1209, L"D\u00e9finir les programmes par d\u00e9faut"},
    {1210, L"Associer un type de fichier ou un protocole \u00e0 un programme"},
    {1211, L"Modifier les param\u00e8tres de lecture automatique"},
    {1212, L"Configurer les programmes par d\u00e9faut et l'acc\u00e8s aux programmes"},
    {1213, L"Programmes par d\u00e9faut"},
    {1214, L"Programmes par d\u00e9faut"},
    {1216, L"D\u00e9finir les programmes par d\u00e9faut"},
    {1217, L"D\u00e9finir les programmes par d\u00e9faut"},
    {1218, L"D\u00e9finir les associations d'un programme"},
    {1219, L"D\u00e9finir les associations d'un programme"},
    {1220, L"Associer un type de fichier ou un protocole \u00e0 un programme sp\u00e9cifique"},
    {1221, L"Associer un type de fichier ou un protocole \u00e0 un programme sp\u00e9cifique"},
    {1224, L"Aide"},
    {1225, L"Aide"},
    {1226, L"Aide"},
    {1230, L"Chargement..."},
    {1231, L"Chargement..."},
};

static const StringEntry kSudPtBr[] = {
    {1, L"Programas padr\u00e3o"},
    {2, L"Definir associa\u00e7\u00f5es"},
    {3, L"Escolher um padr\u00e3o"},
    {4, L"Definir programas padr\u00e3o"},
    {7, L"Definir associa\u00e7\u00f5es de programas"},
    {10, L"Escolha os programas que o Windows usar\u00e1 para navegar na Web, editar fotos, enviar emails e reproduzir m\u00fasicas."},
    {200, L"Erro do Painel de Controle de programas padr\u00e3o"},
    {201, L"Ocorreu um problema ao carregar a p\u00e1gina \u00abDefinir um programa padr\u00e3o\u00bb. (C\u00f3digo de erro: %1!lX!)"},
    {202, L"(Selecione um programa para obter mais informa\u00e7\u00f5es)"},
    {203, L"Nome"},
    {204, L"Descri\u00e7\u00e3o"},
    {205, L"Padr\u00e3o atual"},
    {206, L"Programas"},
    {210, L"Extens\u00f5es"},
    {211, L"Protocolos"},
    {212, L"MAPI"},
    {220, L"Enviar email"},
    {221, L"Comando para enviar email"},
    {222, L"Aplicativo desconhecido"},
    {223, L"Este programa tem todos os seus padr\u00f5es"},
    {224, L"Este programa tem %1!lu! de %2!lu! padr\u00f5es"},
    {225, L"N\u00e3o selecionado"},
    {226, L"Usar o programa selecionado para abrir por padr\u00e3o todos os tipos de arquivo e protocolos que ele suporta."},
    {227, L"Escolha os tipos de arquivo e protocolos que o programa selecionado abre por padr\u00e3o."},
    {228, L"Programas e Recursos"},
    {229, L"Controle dos Pais"},
    {231, L"Para alterar os aplicativos padr\u00e3o, acesse Configura\u00e7\u00f5es > Sistema > Aplicativos padr\u00e3o."},
    {232, L"OK"},
    {240, L"Fazer com que um tipo de arquivo ou protocolo (como .mp3 ou http://) seja sempre aberto por um programa espec\u00edfico."},
    {1169, L"Escolher os programas que o Windows usa por padr\u00e3o"},
    {1170, L"Definir programas padr\u00e3o"},
    {1171, L"Definir um programa como padr\u00e3o para todos os tipos de arquivo e protocolos que ele pode abrir."},
    {1172, L"Associar um tipo de arquivo ou protocolo a um programa"},
    {1173, L"Fazer com que um tipo de arquivo ou protocolo (como .mp3 ou http://) seja sempre aberto por um programa espec\u00edfico."},
    {1174, L"Alterar configura\u00e7\u00f5es de Reprodu\u00e7\u00e3o Autom\u00e1tica"},
    {1175, L"Reproduzir CDs ou outras m\u00eddias automaticamente"},
    {1176, L"Definir acesso a programas e padr\u00f5es do computador"},
    {1177, L"Controlar o acesso a determinados programas e definir padr\u00f5es para este computador."},
    {1180, L"OK"},
    {1183, L"Definir programas padr\u00e3o"},
    {1184, L"Definir associa\u00e7\u00f5es de um programa"},
    {1185, L"Selecione as extens\u00f5es que este programa deve abrir por padr\u00e3o e clique em Salvar."},
    {1186, L"Salvar"},
    {1187, L"Cancelar"},
    {1190, L"Lista de programas"},
    {1192, L"Selecionar tudo"},
    {1193, L"Para definir um programa como padr\u00e3o para todos os tipos de arquivo e protocolos que ele pode abrir, selecione-o e clique em Definir como padr\u00e3o."},
    {1194, L"Associar um tipo de arquivo ou protocolo a um programa espec\u00edfico"},
    {1195, L"Clique em uma extens\u00e3o para ver o programa que a abre atualmente por padr\u00e3o. Para alter\u00e1-lo, clique em Alterar programa."},
    {1196, L"Alterar programa..."},
    {1197, L"Nenhuma extens\u00e3o ou protocolo selecionado"},
    {1198, L"Fechar"},
    {1199, L"Lista de programas dispon\u00edveis para alterar as configura\u00e7\u00f5es padr\u00e3o."},
    {1200, L"Descri\u00e7\u00e3o do programa"},
    {1201, L"Lista de associa\u00e7\u00f5es do programa"},
    {1202, L"Selecione as extens\u00f5es que este programa deve abrir por padr\u00e3o e clique em Salvar."},
    {1203, L"Nenhuma extens\u00e3o ou protocolo selecionado"},
    {1205, L"Lista de extens\u00f5es"},
    {1206, L"Clique em uma extens\u00e3o para ver o programa que a abre atualmente por padr\u00e3o. Para alter\u00e1-lo, clique em Alterar programa."},
    {1207, L"&Definir este programa como padr\u00e3o"},
    {1208, L"&Escolher padr\u00f5es para este programa"},
    {1209, L"Definir programas padr\u00e3o"},
    {1210, L"Associar um tipo de arquivo ou protocolo a um programa"},
    {1211, L"Alterar configura\u00e7\u00f5es de Reprodu\u00e7\u00e3o Autom\u00e1tica"},
    {1212, L"Definir acesso a programas e padr\u00f5es do computador"},
    {1213, L"Programas padr\u00e3o"},
    {1214, L"Programas padr\u00e3o"},
    {1216, L"Definir programas padr\u00e3o"},
    {1217, L"Definir programas padr\u00e3o"},
    {1218, L"Definir associa\u00e7\u00f5es de um programa"},
    {1219, L"Definir associa\u00e7\u00f5es de um programa"},
    {1220, L"Associar um tipo de arquivo ou protocolo a um programa espec\u00edfico"},
    {1221, L"Associar um tipo de arquivo ou protocolo a um programa espec\u00edfico"},
    {1224, L"Ajuda"},
    {1225, L"Ajuda"},
    {1226, L"Ajuda"},
    {1230, L"Carregando..."},
    {1231, L"Carregando..."},
};

enum class Language { English, Italian, German, Spanish, French, PortugueseBrazil };

StringTable SudTable(Language language) {
    if (language == Language::Italian) return {kSudIt, ARRAYSIZE(kSudIt), L"it-IT"};
    if (language == Language::German) return {kSudDe, ARRAYSIZE(kSudDe), L"de-DE"};
    if (language == Language::Spanish) return {kSudEs, ARRAYSIZE(kSudEs), L"es-ES"};
    if (language == Language::French) return {kSudFr, ARRAYSIZE(kSudFr), L"fr-FR"};
    if (language == Language::PortugueseBrazil) return {kSudPtBr, ARRAYSIZE(kSudPtBr), L"pt-BR"};
    return {kSudEn, ARRAYSIZE(kSudEn), L"en-US"};
}
// The strings of the DirectUI copy are language neutral: font specifications and the names read by screen readers.
StringTable DuiTable() { return {kDuiStrings, ARRAYSIZE(kDuiStrings), L"neutral"}; }

// =====================================================================================================================
// SHA-256 (FIPS 180-4). Own implementation: no dependency on a crypto provider being available in the host.
// =====================================================================================================================

struct Sha256 {
    uint32_t h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    uint8_t block[64];
    size_t fill = 0;
    uint64_t total = 0;

    static uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
    void compress(const uint8_t* p) {
        static const uint32_t k[64] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98,
            0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
            0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8,
            0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
            0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
            0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
            0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
            0xc67178f2};
        uint32_t w[64];
        for (int i = 0; i < 16; i++)
            w[i] = uint32_t(p[4 * i]) << 24 | uint32_t(p[4 * i + 1]) << 16 | uint32_t(p[4 * i + 2]) << 8 | p[4 * i + 3];
        for (int i = 16; i < 64; i++) {
            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
        for (int i = 0; i < 64; i++) {
            uint32_t t1 = hh + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ (~e & g)) + k[i] + w[i];
            uint32_t t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
            hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }
    void update(const uint8_t* p, size_t n) {
        total += n;
        while (n) {
            size_t take = std::min(n, sizeof(block) - fill);
            memcpy(block + fill, p, take);
            fill += take; p += take; n -= take;
            if (fill == sizeof(block)) { compress(block); fill = 0; }
        }
    }
    std::string hex() {
        uint64_t bits = total * 8;
        uint8_t pad[72] = {0x80};
        size_t padLen = (fill < 56 ? 56 : 120) - fill;
        update(pad, padLen);
        uint8_t len[8];
        for (int i = 0; i < 8; i++) len[i] = uint8_t(bits >> (56 - 8 * i));
        update(len, 8);
        static const char digits[] = "0123456789abcdef";
        std::string out;
        for (uint32_t v : h)
            for (int s = 28; s >= 0; s -= 4) out.push_back(digits[(v >> s) & 15]);
        return out;
    }
};

std::string Sha256Hex(const uint8_t* p, size_t n) {
    Sha256 s;
    s.update(p, n);
    return s.hex();
}
std::string Sha256Hex(const Bytes& b) { return Sha256Hex(b.data(), b.size()); }

// =====================================================================================================================
// The generator. Pure: bytes in, bytes out; the same inputs always give the same bytes.
//
// Output = a Windows 10 module (sud.dll 1607 or dui70.dll 1703) with
//   * for sud.dll, the import name DUI70.dll replaced by the private name of the DirectUI copy,
//   * a new resource directory in an added section: all original resources (their data stays where it is) plus the
//     strings (one language, stored language-neutral), without the "MUI" resource (the strings are inside now),
//   * for sud.dll, a page definition with literal page titles and the copy's own icons (the class ids stay the
//     registered ones).
// The generator itself does not change code; a build-specific compatibility patch may precede it.
// =====================================================================================================================

// <gen-begin>
namespace gen {

constexpr uint32_t kRtString = 6;

uint16_t R16(const Bytes& b, size_t o) { uint16_t v = 0; if (o + 2 <= b.size()) memcpy(&v, &b[o], 2); return v; }
uint32_t R32(const Bytes& b, size_t o) { uint32_t v = 0; if (o + 4 <= b.size()) memcpy(&v, &b[o], 4); return v; }
void W16(Bytes& b, size_t o, uint16_t v) { memcpy(&b[o], &v, 2); }
void W32(Bytes& b, size_t o, uint32_t v) { memcpy(&b[o], &v, 4); }
void P16(Bytes& b, uint16_t v) { b.push_back(uint8_t(v)); b.push_back(uint8_t(v >> 8)); }
void P32(Bytes& b, uint32_t v) { P16(b, uint16_t(v)); P16(b, uint16_t(v >> 16)); }

struct Section { uint32_t va, vsize, rawPtr, rawSize; };

struct Pe {
    const Bytes* d = nullptr;
    size_t nt = 0, fh = 0, opt = 0, dd = 0, sec = 0;
    unsigned nsec = 0;
    std::vector<Section> sections;

    bool parse(const Bytes& b) {
        d = &b;
        if (b.size() < 0x200 || b[0] != 'M' || b[1] != 'Z') return false;
        nt = R32(b, 0x3C);
        if (nt + 24 + 240 > b.size() || R32(b, nt) != 0x00004550) return false;
        fh = nt + 4;
        nsec = R16(b, fh + 2);
        opt = fh + 20;
        if (R16(b, opt) != 0x20B) return false;                       // PE32+
        dd = opt + 112;
        sec = opt + R16(b, fh + 16);
        if (nsec == 0 || nsec > 32 || sec + 40ull * nsec > b.size()) return false;
        sections.clear();
        for (unsigned i = 0; i < nsec; i++) {
            size_t o = sec + 40 * i;
            sections.push_back({R32(b, o + 12), R32(b, o + 8), R32(b, o + 20), R32(b, o + 16)});
        }
        return true;
    }
    // File offset of an RVA, or SIZE_MAX. `need` bytes must be inside the file.
    size_t off(uint32_t rva, size_t need = 1) const {
        for (const Section& s : sections) {
            uint32_t span = std::max(s.vsize, s.rawSize);
            if (rva >= s.va && rva - s.va < span) {
                size_t o = size_t(s.rawPtr) + (rva - s.va);
                if (rva - s.va + need > s.rawSize || o + need > d->size()) return SIZE_MAX;
                return o;
            }
        }
        return SIZE_MAX;
    }
    uint32_t dirRva(int i) const { return R32(*d, dd + 8 * i); }
    uint32_t dirSize(int i) const { return R32(*d, dd + 8 * i + 4); }
};

// A resource type or name: a string (sorted first, case-insensitively) or a number.
struct ResId {
    bool isName = false;
    uint32_t id = 0;
    std::wstring name;
    std::wstring upper() const {
        std::wstring u = name;
        for (wchar_t& c : u)
            if (c >= L'a' && c <= L'z') c = wchar_t(c - 32);
        return u;
    }
    bool operator<(const ResId& o) const {
        if (isName != o.isName) return isName;
        return isName ? upper() < o.upper() : id < o.id;
    }
    bool operator==(const ResId& o) const { return isName == o.isName && (isName ? name == o.name : id == o.id); }
    bool is(const wchar_t* s) const { return isName && name == s; }
    bool is(uint32_t v) const { return !isName && id == v; }
};

struct ResItem {
    ResId type, name;
    uint32_t lang = 0;
    bool isNew = false;        // true: `data` goes into the new section; false: stays at rva/size in the image
    uint32_t rva = 0, size = 0, codepage = 0;
    Bytes data;
};

bool ReadResources(const Pe& pe, std::vector<ResItem>& out) {
    const Bytes& d = *pe.d;
    const uint32_t rootRva = pe.dirRva(2), rootSize = pe.dirSize(2);
    const size_t base = pe.off(rootRva, rootSize);
    if (base == SIZE_MAX || rootSize < 16) return false;
    auto ident = [&](uint32_t v, ResId& id) -> bool {
        if (v & 0x80000000u) {
            const size_t o = base + (v & 0x7FFFFFFFu);
            if (o + 2 > base + rootSize) return false;
            const size_t n = R16(d, o);
            if (o + 2 + 2 * n > base + rootSize) return false;
            id.isName = true;
            id.name.assign(n, L'\0');
            for (size_t i = 0; i < n; i++) id.name[i] = wchar_t(R16(d, o + 2 + 2 * i));
        } else {
            id.isName = false;
            id.id = v;
        }
        return true;
    };
    struct Entry { uint32_t key, value; };
    auto entries = [&](uint32_t dirOff, std::vector<Entry>& e) -> bool {
        e.clear();
        if (dirOff + 16ull > rootSize) return false;
        const size_t n = size_t(R16(d, base + dirOff + 12)) + R16(d, base + dirOff + 14);
        if (dirOff + 16ull + 8 * n > rootSize) return false;
        for (size_t i = 0; i < n; i++)
            e.push_back({R32(d, base + dirOff + 16 + 8 * i), R32(d, base + dirOff + 20 + 8 * i)});
        return true;
    };
    std::vector<Entry> types, names, langs;
    if (!entries(0, types)) return false;
    for (const Entry& t : types) {
        if (!(t.value & 0x80000000u) || !entries(t.value & 0x7FFFFFFFu, names)) return false;
        for (const Entry& n : names) {
            if (!(n.value & 0x80000000u) || !entries(n.value & 0x7FFFFFFFu, langs)) return false;
            for (const Entry& l : langs) {
                if ((l.value & 0x80000000u) || l.value + 16ull > rootSize) return false;
                ResItem item;
                if (!ident(t.key, item.type) || !ident(n.key, item.name)) return false;
                item.lang = l.key;
                item.rva = R32(d, base + l.value);
                item.size = R32(d, base + l.value + 4);
                item.codepage = R32(d, base + l.value + 8);
                if (pe.off(item.rva, item.size ? item.size : 1) == SIZE_MAX) return false;
                out.push_back(std::move(item));
            }
        }
    }
    return true;
}

Bytes ResourceBytes(const Pe& pe, const ResItem& item) {
    const size_t o = pe.off(item.rva, item.size ? item.size : 1);
    return Bytes(pe.d->begin() + o, pe.d->begin() + o + item.size);
}

// Replaces every occurrence of `from` by `to`; returns the number of replacements.
int ReplaceAll(Bytes& b, const std::string& from, const std::string& to) {
    int count = 0;
    Bytes out;
    size_t i = 0;
    while (i < b.size()) {
        if (i + from.size() <= b.size() && memcmp(&b[i], from.data(), from.size()) == 0) {
            out.insert(out.end(), to.begin(), to.end());
            i += from.size();
            count++;
        } else {
            out.push_back(b[i++]);
        }
    }
    if (count) b.swap(out);
    return count;
}

// Finds "dui70.dll", or "dui17.dll" (a pre-renamed DirectUI file), in any case, as a
// whole zero-terminated string; `count` is how many there are.
size_t FindDuiImport(const Bytes& b, size_t& count) {
    static const char* const wants[] = {"dui70.dll", "dui17.dll"};
    const size_t n = 10;                // 9 characters and the terminating zero
    size_t found = SIZE_MAX;
    count = 0;
    for (size_t i = 0; i + n <= b.size(); i++) {
        if (b[i + n - 1] != 0 || (i > 0 && b[i - 1] != 0)) continue;
        for (const char* want : wants) {
            bool same = true;
            for (size_t k = 0; k + 1 < n && same; k++) {
                char c = char(b[i + k]);
                if (c >= 'A' && c <= 'Z') c = char(c + 32);
                same = c == want[k];
            }
            if (same) { found = i; count++; break; }
        }
    }
    return found;
}

// Attribute text, pure ASCII: characters outside 32..126 become numeric character references.
std::string XmlEscape(const std::wstring& s) {
    static const char digits[] = "0123456789ABCDEF";
    std::string out;
    for (wchar_t ch : s) {
        const unsigned c = unsigned(uint16_t(ch));
        if (c == '&') out += "&amp;";
        else if (c == '<') out += "&lt;";
        else if (c == '>') out += "&gt;";
        else if (c == '"') out += "&quot;";
        else if (c < 32 || c > 126) {
            out += "&#x";
            bool started = false;
            for (int shift = 12; shift >= 0; shift -= 4) {
                const unsigned nibble = (c >> shift) & 15;
                if (nibble || started || shift == 0) { out.push_back(digits[nibble]); started = true; }
            }
            out.push_back(';');
        } else {
            out.push_back(char(c));
        }
    }
    return out;
}

// Page definition (XMLFILE 100): literal titles (they would otherwise be read from System32\sud.dll), own icons. The
// class ids are left as they are: Control Panel refuses a class that is not registered.
void PatchSudDefinition(Bytes& xml, const StringTable& table, const std::wstring& selfPath) {
    static const char prefix[] = "displayname=\"@sud.dll,-";
    const size_t plen = sizeof(prefix) - 1;
    Bytes out;
    size_t i = 0;
    while (i < xml.size()) {
        if (i + plen <= xml.size() && memcmp(&xml[i], prefix, plen) == 0) {
            size_t j = i + plen;
            uint32_t id = 0;
            size_t digits = 0;
            while (j < xml.size() && xml[j] >= '0' && xml[j] <= '9' && digits < 6) { id = id * 10 + (xml[j] - '0'); j++; digits++; }
            const wchar_t* text = (digits && j < xml.size() && xml[j] == '"') ? table.find(id) : nullptr;
            if (text) {
                const std::string repl = "displayname=\"" + XmlEscape(text) + "\"";
                out.insert(out.end(), repl.begin(), repl.end());
                i = j + 1;
                continue;
            }
        }
        out.push_back(xml[i++]);
    }
    xml.swap(out);
    if (!selfPath.empty())
        ReplaceAll(xml, "icon=\"sud.dll,-", "icon=\"" + XmlEscape(selfPath) + ",-");
}

// String blocks (RT_STRING): block id -> 16 counted UTF-16 strings.
void BuildStringBlocks(const StringTable& table, std::vector<std::pair<uint32_t, Bytes>>& out) {
    size_t i = 0;
    while (i < table.count) {
        const uint32_t block = (table.entries[i].id >> 4) + 1;
        Bytes blob;
        for (uint32_t slot = 0; slot < 16; slot++) {
            const wchar_t* s = L"";
            if (i < table.count && (table.entries[i].id >> 4) + 1u == block && (table.entries[i].id & 15u) == slot)
                s = table.entries[i++].text;
            const size_t n = wcslen(s);
            P16(blob, uint16_t(n));
            for (size_t k = 0; k < n; k++) P16(blob, uint16_t(s[k]));
        }
        out.emplace_back(block, std::move(blob));
    }
}

// Windows 7-style icon supplied for the first hub link. The ICO is embedded,
// so setup does not download another file. All original image sizes are kept.
static const char kFirstLinkIconBase64[] =
    "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"
    "/wBMcv4Af5r/ALLC/wAAIEwAADZ/AABMsgAAYuUAGXv/AEyZ/gB/tv8AstP/AAAxTAAAUX8AAHKyAACT5QAZrf8ATL/+AH/R/wCy"
    "4/8AAEFMAABtfwAAmbIAAMTlABne/wBM5f4Af+z/ALL0/wAATEcAAH92AACypQAA5dUAGf/uAEz+8gB///UAsv/5AABMNgAAf1sA"
    "ALJ/AADlowAZ/70ATP7MAH//2gCy/+kAAEwmAAB/PwAAslkAAOVyABn/jABM/qUAf/+/ALL/2AAATBUAAH8kAACyMwAA5UEAGf9b"
    "AEz+fwB//6MAsv/IAABMBQAAfwkAALIMAADlEAAZ/ykATP5ZAH//iACy/7cACkwAABJ/AAAZsgAAIOUAADr/GQBm/kwAkf9/AL3/"
    "sgAbTAAALX8AAD+yAABR5QAAa/8ZAIz+TACt/38Azf+yACtMAABIfwAAZbIAAIPlAACc/xkAsv5MAMj/fwDe/7IAPEwAAGR/AACM"
    "sgAAtOUAAM3/GQDY/kwA4/9/AO7/sgBMTAAAf38AALKyAADl5QAA//8ZAP7+TAD//38A//+yAEw8AAB/ZAAAsowAAOW0AAD/zRkA"
    "/thMAP/jfwD/7rIATCsAAH9IAACyZgAA5YMAAP+cGQD+skwA/8h/AP/esgBMGwAAfy0AALI/AADlUQAA/2sZAP6MTAD/rX8A/82y"
    "AEwKAAB/EgAAshkAAOUgAAD/OhkA/mVMAP+RfwD/vbIATAAFAH8ACQCyAAwA5QAQAP8ZKQD+TFkA/3+IAP+ytwBMABUAfwAkALIA"
    "MgDlAEEA/xlbAP5MfwD/f6MA/7LIAEwAJgB/AD8AsgBZAOUAcgD/GYwA/kylAP9/vwD/stgATAA2AH8AWwCyAH8A5QCjAP8ZvQD+"
    "TMwA/3/aAP+y6QBMAEcAfwB2ALIApQDlANUA/xnuAP5M8gD/f/UA/7L5AEEATABtAH8AmQCyAMQA5QDeGf8A5Uz+AOx//wD0sv8A"
    "MQBMAFEAfwByALIAkwDlAK0Z/wC/TP4A0X//AOOy/wAgAEwANgB/AEwAsgBiAOUAexn/AJlM/gC2f/8A07L/ABAATAAbAH8AJgCy"
    "ADEA5QBKGf8Ackz+AJp//wDCsv8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFkXAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "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"
    "/wAxAEwAUQB/AHIAsgCTAOUArRn/AL9M/gDRf/8A47L/ACAATAA2AH8ATACyAGIA5QB7Gf8AmUz+ALZ//wDTsv8AEABMABsAfwAm"
    "ALIAMQDlAEoZ/wByTP4Amn//AMKy/wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "FRYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGGFrYQgAAAAAAAAAAAAAAAAAAAAAAAAAABoZCBcXCBZZcnJqYQAAAAAAAAAA"
    "AAAAAAAAAAAAGxocHR4eHh4dFWJibXJiFQAAAAAAAAAAAAAAAAAAGxsdHa8HthmtA1lyY21ibHJZGQAAAAAAAAAAAAAAABodHa8a"
    "tq6uthh5WXJtCGxibWKBAAAAAAAAAAAAAAAaHh2utq6urq4XeWFybRhtUGptY1oWAAAAAAAAAAAAGh4dthqurq6trXlhYnpmFgNS"
    "WXZleloZAAAAAAAAABodHbaurq6lGa2tFoFaGVJSNj0WGWZtWoEAAAAAAAAAGx6uGq6urRmkra2aF1JSGT09RV1ZGm5tFloAAAAA"
    "ABkeHbaupRmtraIYorWjPVVDRT1FPVoWZm5tFhkAAAAAGh62rratra2ata2iorVDRD1ERUU+VloWZmVaFwAAAAAcHrauGa0ItZoX"
    "orUGokRERERFRT5GZlkZb20XbQAAGR4HtqW2raO1orWioherRERERD1FTT4/ZVkZZm4WCAAIH66urq2togiioqKioqpDRENEREVF"
    "Rj8bF2IbZhliABYPGbcatgi1orUWohaioURDRENEPVU+V05mFm0abhYAFg8ZHBsZGQglFRbyFvLpSlJSSlJKZhpWB14YYhZtFQAX"
    "HyYnJyYlJSUrJPsrIythYXlZgRdabhpmGm4XdhYAAAgfLicnLiYtJSv7K/sj+2F5WWJaF20YZhpmGBkdCAAAGR4vNi8nLi0lJTIk"
    "Kyv7Yll6FmJtF2UYbhltGh4ZAAAABx4uHScuJi39JSX6KyRiFlpaF1oYZRhlbRZnHQAAAAAZHy4vHScuGi0tJSUlJVltF21tGGVl"
    "F1oXbdcaAAAAABkdHS4eNycnLiYYLSUtCGVlCGVlF2UWbRYaHhoAAAAAABofJyceHi8cJy4uLjUZZhleGV0ZZVp6bh0bAAAAAAAA"
    "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"
    "Hi4vJy4mLSUlF21aF1plFm0WHgcAAAAAHB0uHS8bLi4tGF0ZZRhtUYFnHQcAAAAAGx4HNh439y82XmZeZhllFm4dGwAAAAAAABse"
    "Jy4eHx4vHDcHXhlaGh4HAAAAAAAAAAAHHh0nLi83N2YZbQh3HhsAAAAAAAAAAAAAGxweHS8nNhkaGx4cGwAAAAAAAAAAAAAAAAAH"
    "HB0eHo8dHQccAAAAAAAAAAAAAAAAAAAAGxoZGRoZGgAAAAAAAAAAAP///wD/AH8A/AB/APgAPwDwAB8A4AAPAMAABwCAAAMAgAAD"
    "AAAAAQAAAAEAAAAAAAAAAQAAAAEAAAABAAAAAQCAAAMAwAADAMAABwDgAA8A8AAfAPgAPwD+AH8A/wH/ACgAAAAQAAAAIAAAAAEA"
    "CAAAAAAAAAEAAAAAAAAAAAAAAAEAAAABAAAAAAAAO2k8ADp5MABNbDsAFXtIABh/UQA+fkYAW21KAEZ/UABLf1AAdnZ4AHh4egB8"
    "fH4AtWcXAIxIMwCmVyAAplwvALxsKgC4dikAyWwiAIxWQQCaXkwAoWhQAKVqUQCpbFAAqGxVAK9wUgDTekAA2X5FAMB9dgAXjiQA"
    "HJIjACuYQgA3qkwAPqtWAC+PawBEg0oAS4BQAEqFUgBJvV4AY4hjAG2EbgBklmoAeY96AHuRfAB2uX4AJuBJAOaARgDMiGgA35Bt"
    "AO+daAD2m2kA/6R4APWpfQD+qn8Af3+CABUV5AAXHfAAGiTgAB0q7wAiLukAKUHpAE1XxwBGVssAUFvLAFFczABBVNIAR1nQAE1a"
    "0QBJT+sARFPuAER0+wB0ffcARZ+bAFyukgBxt4kAdr6tAFnFgQBcyoAAbMeIAG3UiwB404gAds+SAHrSlwAjuMcARL7PAHWB8QAx"
    "wtgAEMzjABbe/wAhyuIAItv/ADHb/wBG2P4AYtj2AHre/gCEhoYAgomEAIiIigCLi40AlZWXALyWggC5nI0Ai6OPAI2jjgCrq6wA"
    "rq6vAKWrvwCzs7QAtbW2AP6vggDnrKcAiNKeAITbpgC63b0AjOSzANLCuACRnsYAp63AAKe03QCAjPYAkaH6AKm5/wCyv/4At/bT"
    "AJft/gDFxcUA0M/QANDQ0QDU1NUA2dHQANjZ3wDm184Ax/bNAM762gDl4d8A6eHdAM/Y/gDe4PsAzvPyAOrr7gDs7e0A6vLrAOjw"
    "/wDs8P8A8vLyAPPz9AD09PQA+Pn5AP///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAABpYwoLYQEoAAAAAAAAAG1pgYeCfgEuASkAAAAAAG1/hGUYMAEuhS4BKwAAAG1/dBc1NgEulQGGLgEsAABpiBduMhwSAQFJ"
    "AYYuAWhpgWYxMxsREFVbXUoChi4BY5IaNC8TDQ9XWVx9SwGGB2OSGG8dFRQOVFhaXl8tAQNjkkJ6Rz06OAQeIE9TKnJgY5I/e3lG"
    "PDkFHyJSUSSOZGmBa0SJVkU7IyFNTicJlWkAaY0+d4p4SExxc1AGjWkAAG2Bg0B1kJCLfHAmjYFtAAAAbX+NdkBDCCZolH9tAAAA"
    "AABtaYGSkpKSgWltAAAAAAAAAABpYwtgYGkAAAAAAPgPAADgBwAAwAMAAIABAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "gAEAAIABAADAAwAA4AcAAPgfAACJUE5HDQoaCgAAAA1JSERSAAABAAAAAQAIBgAAAFxyqGYAAAABc1JHQgCuzhzpAAAABGdBTUEA"
    "ALGPC/xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdTAAAOpgAAA6mAAAF3CculE8AAD/jklEQVR4Xuy9B1he15U1/DlTv8m0"
    "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"
    "N/8TyjT/Fnck7xVWIXYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHNzdCpzc3S84eDfoe3YzaK6"
    "e1zO5JJw+fOccfzlmm724Zpr+dyVZPnXkFz60opT+c+DTPnMfkX6yXk/+tRzPfmhcS37UYYO/hmtGv8Ivhb/EMcw/ynQSv8s0kn/"
    "TthZ/4DQov5JpL37NaLS+Tyx7PYxqqv5D54c/yK6OP8y01D/IM5A/xjLOf8d0ED/HsA5+BpqKPwaaykVAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAc3N0CHNzdJi2tbaX9+/rr79/YMfOg2Tt9Z50/+SZbfXhmWr525Nh+taNWPnRhk75zH9G+sh6"
    "P/rFdjr6w3E1+sVuMfq3ay37qqRq/my/cv8UyT//FctE/zTWVv9i2XL/hdGp/kqlvfw6p9f5Prfm9z+36fZBtu33KrBn/Q+cGv8q"
    "zEf/HM0+/x3OP/8hz0L/L9dP/yCRMPAWXiO6GmspCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAc3NzSoCAga3+/fzN"
    "1KaPsb57XuH3oXj/5Zhs9eKZavjck2L51YtX+c+ETPnKfEP6x3c8+sNzNvrAbzL7vm0u+7xrK/u9aCj7vWkq/JGkcP2I3K7+feat"
    "/5Hkuv580L/+SarF/UCx3ftIwuz5SMHs+UnA6/hMwe74SLvZ+BiiKv8Xryj/Hs4//yDOQf8r0kr/Rdth/0/haf4abyjqF14kZwAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABzc3QPc3N0rs7NzKDt2tGvuHlc1PideP/rl2v345lr992TY/nWjFj5z4NN+ct8QvrG"
    "dTn6wnE0+r9vMPu8bCz7umop/LdoJ/y2aCT8u2Ec/KV7R/tVs6H9Rbe7/iiZsf8wncH+P7jf/UrN8/pRzPL6VMvw+lfK7/lZyu75"
    "Xcv4+UK9kvsMmhj/HMQ4/ynRSP8/2Fz/U91t/1rjdf86t070FWEi0xloKCEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABzc3RMgYGC"
    "q/79/NbNmH7F2Yxs9vSYbvzklmv23pRl+NiNWfnRg0z5ynpB+sV0OPrBbzH6u2wt+7lqKvu2aCf8tGck/LNmIv2zZSH9t2AZ/aB+"
    "Sv1Uwcj9MtTz/jLW9/481/n+Qtf5/UnW+PxT1fb7XNX0+2LU8vpn1fL6atXz+mjP5/ohqTz+EaYg/zvWV/9S3Wz/WN5y/1jgc/9W"
    "5XH/IYcw6xdgJbMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABzc3SSt7a3pPDg2La6e13W+J13/+aSaPTilWj43I5d+dOET/nMe0P5"
    "xnM5+cBtMPq8aiz6uGgn+7VnJPyzZiL9sWQg/bBkHv6vYx3+tF4V/pR/Tv4/ytn+Ktj5/yzZ/f8z2fv+Otr7/kTa+v5O2vn9W9v4"
    "/Wjc9fxy3fT7et70+37g+vtl0Lr8Ep4e/zjCTP9Y3nP/V95x/1fecf9a43X/StRi+xdoJeUZZideAAAAAAAAAAAAAAAAAAAAAHNz"
    "dCJzc3TE9fX0wditmMPOg2Tn9pRt/uWUafbgkGH414dU+M58RfnJdDv5wW0x+rxqK/q3Zyb7tGUj/LFkIf2vYx/9rWId/q1iG/6s"
    "YRr+sV0S/ouAUf4uzuX+INj7/yPZ/f8q2fz/Mtr7/jvc+/5H3fv+WN/6/Wjh+f144/j8hOX3/Irm9vyP6Pv8SMFv/hWfIP9R12n/"
    "V95x/1becP9Y33L/W+Z1/zOoROsYZSbcGWcoHQAAAAAAAAAAAAAAAHNzc1aHh4ip/Pr40MWDZOb2m3b/545l8+OSZvfci1v404BM"
    "+Mt2PfnEbjL6vmgr+rhmJvu0ZCL8sWQg/a9iHv2sYRz+q2Aa/qlgGf6pXxj+r1sP/oOAVP8e0e7/Ftb8/xrX/f8h2fz/KNr8/zLc"
    "/P8+3vv+UOH7/mTk+v535/n9ien4/ZTs9/2b7vr9j+Xi/R6kLf8vtz7/Vt5w/1jecv9d4Hf/aOeA/2Xqfv8fdSzZGGUmnwAAAAAA"
    "AAAAAAAAAHNzc3umpqar8uTdvr59X9z3lnL/5ZNq9eCRZPfYhFP4z3hD+chwN/m/aS35umYn+7VkI/uxYiD8rmId/atgG/6pXxn+"
    "p14Y/qZeF/+mXhb/rFgO/3yAV/8T0/X/D9X8/xPW/f8Z1/3/H9n9/ynb/P823vz/RuH8/lvk+/5w5/v+iOv6/Znv+f2j8fn9qPT+"
    "/W7Rl/4RnBz/T9Zm/2Xjfv9u5oT/deiK/3/ylP9c0W7zF2Ij1BlnJ08AAAAAAAAAAHNzdJ7IyMe248S1wsiBY+L1l3T+5Jhx9t6R"
    "ZfjVglD4zHQ8+MRrMfm9Zin6t2Mk+7JiIPyuYR38q2Ab/aheGf6mXRf+pV0W/6RcFf+jXBX/qVYM/3V/Wf8M1Pn/C9T8/w3U/P8T"
    "1fz/Gdf9/yLZ/P8u3Pz/PuD8/lTk/P5q5/v+g+v7/prv+v6q8vn+sPX7/qbt6P45sEv/KK41/3fpjP9/65L/g+yW/4nwnP+L+5//"
    "PZxL4RlmJ9YaaykIAAAAAHNzdL3i4eLJ1aiR1NyNbuzvmnf645hx99yPYvjTglH4ynI8+cBnK/m6YyX6tGIh+7BgHvysXxz9qV4a"
    "/qZdF/6kXBb/olsV/6BaFP+fWhT/plQK/3B/XP8H1Pz/B9P9/wrT/P8O1Pz/E9b8/xvY/P8m2vz/N978/0vi/P5j5vz+fuv7/pjv"
    "+/6t8vr+t/T6/rv3/f6P3L3/GJ8k/2nZd/+N75//j/Gh/5Hxov+Y+ar/gu+V/h1rKdYZZiehAAAAAHNzdNf29fXgz5Z75++aefnq"
    "mnj445lx99uNYfjSgE/4yXQ++cBmK/q5YCH6s18f+65eG/yqXhr9p10Y/qRcFv6hWhX/n1oU/51YE/+cWBL/o1EI/2t+Xf8E1P3/"
    "BNL9/wfS/P8K0/z/D9T8/xbW/P8g2f3/L9z9/0Ph/P5a5fz+d+r7/pPv+/6q8fr+uPT6/r32+f6u7er+Vrtv/yqqNf+R8KH/mvSr"
    "/5r0q/+Z86n/mv6s/13JbPEYZibikE4LAXNzdOX+/v7y0I5v/fejf//rnXr35Jpw+NuNX/jTf0v5zXY8+cNsLvq7Yh77tF8Y+7Bf"
    "Fv2sXhX+qF0T/qRcEv6hWxH/n1kP/51YDv+cVw3/o1AG/2h/Xf8C1f7/A9P//wXS//8I0///DNT//xLW//8b2P//Kdv//z7g//9T"
    "5P7+b+n+/o7u/P6o8fv+uvT7/r/0+/6/9f3+nuHV/iemNf9dyWj/oPav/5/2sP+b9Kz/m/qt/3Xeg/8mdyy2AAAAAHNzdOX+/v7+"
    "zIdo+9aksP/PoqP3yZyY+L+Oh/i0f3b4qnJn+aJqWfqcYkr7lVk8/JJXNv2RVjH9kVYt/pBUKf6PUyb/j1Ii/45QIP+NTx7/k0gT"
    "/154Y/8Bz/H/A8zv/wXM7v8Ize7/C87u/xHQ7v8Z0u7/Jtbt/zja7f9N3+z+Z+Ps/oTo6v2d6+r9r+3n/bfr5Py36d/8sOfc+2fA"
    "gfwcoCb/iueY/6P5sv+e9q7/o/22/33aiv0ocyyiAAAAAHNzdOX+/v7yTHX7/4ul//+drP31laT593+Q+PZhc/L3RVXr+DRB5fon"
    "MN/7HCPa/RQY1v4TE9T+EhHS/xEO0f8QDdH/EA3Q/xAM0P8QDND/EQbZ/w82lf8LhCP/DIMn/w2EKv8Ohiz/EIkw/xSNNf8Ykjv/"
    "H5lE/ymhT/82qVr/RbJn/lm7d/1sw4X8f8qT+4/OmvqX0KD6mtGg+IbFjvY8qUT9X8Rq/rb9w//m6ef/nemq81OrWbwVZhVKAAAA"
    "AHNzdNr39/ffXYD264yi//6osvn2oKz39omW9vZqePX3S1ry+DVC7vomMev7GiLn/Q8V5f4KDuP+CAvi/wcJ4f8GB+H/Bgfh/wYH"
    "4f8GB+H/BQHr/wkwmf8NfxP/DX4X/w5/Gv8QgRz/EoUh/xWJJ/8ajy7/IZU3/yqdQ/82pVD+Ra5e/lm4cP1qwH38fceM+4zNmPqV"
    "z5/6l9Ch+JLPm/ZvvXj5YLNn+WOladdRoVStP59BkByCHUAVfBUFAAAAAHNzdMLm5ubHeJb02nyU+fKwufr7p7H19pKd9Pd0gfP3"
    "VmPw+D9L7PkuN+j7Iirl/Rge4v4PE+D+Cw7f/woL3v8JCd3/CQnd/wgI3f8ICN3/CALn/woxmf8OgBj/DoAc/w+BH/8RhCL/FIgn"
    "/xeNLv8dkzb/I5k+/y2hSv84qFX+SLJk/lm6dP1rwoH8e8eN+ofNlPmNzJr4kc2b947Ml/WKzZP5aaxz7nGacqjFzMXda3RrshV8"
    "FQUAAAAAAAAAAHNzdKHJycq0orb3yGN/6+W7w/7/r7f2952m9feAjPT3Ym7x+EdT7fk3Qur7KTHm/B0k4/4VGuH+DxLg/wsO3v8K"
    "C97/CQnd/wgI3f8ICN3/CALn/wsymf8Pghv/EIMg/xGFI/8TiCf/Fows/xqRM/8hlzz/J51E/zGlT/48rFr+SbNm/Vm7c/xpwoD7"
    "dcWI+n/JjviEyZD2hciQ9oLGi/N6xIP5QJhL8oi8jdy4urjBcHNxjwAAAAAAAAAAAAAAAHNzdHunp6ep0dz8w0tv5t69xv//t7/3"
    "+Kmx9/iOmfX3b3zz+FRg7/lCTez7MTvo/CUt5f0cIuP+FBjh/w8T4P8MDt//Cgze/woL3v8JCd3/CQPn/wwznP8ShCL/E4cm/xWJ"
    "KP8XjC3/GpEz/x+WOv8lnEL+LKJK/jWoVP5AsF/9TLdq/Fq9dfxmwn77cMWD+HXFhfd3w4T0d8KC9HbBgPJsvnf5NZBA7cXcx8ec"
    "nJ21c3N0ZgAAAAAAAAAAAAAAAHNzdFSHh4iq9Pb+00Jr7uWwv///wMf497O7+Picpvb5gIz1+GRw8fpPW+/6PEfq/C435/0jKuX+"
    "GyHi/hUa4f8QE+D/DRDf/wwP3/8LDd7/Cgbo/w81n/8Whir/GI0t/xqQMv8dkzb/IZc8/iWcQ/4roUn+M6hT/j2vXf5Fs2T9ULpu"
    "+1q/dvthwHj5Z8F69mnAfPZpvXf0abx18Wm8dPJbs2b5KYgz6u/079SAgIG+c3N0OgAAAAAAAAAAAAAAAHNzdCZzc3TA9fb4wnqX"
    "9NiDnv76y9H8/LzE+Piqsvj4kJr393OA8/ldafD5Slfu+zxH6vwxOuj9Ji7l/h4k4/4ZHuL/FBjh/xEV4P8PE+D/Dg3p/xM6pf8d"
    "izr/H5Q3/yGXO/8lmkH/KZ9H/i2jTf4zqVX+O69d/kK0ZP1KuWv8U71x+1e/dPlcwHb3X7509l67cfNct2vyWrVo71i1ZfNEoU/1"
    "aKZu0+Xm5cZzc3TDc3N0EgAAAAAAAAAAAAAAAAAAAABzc3SZu7u8n8nV+79Lb+feytP//8bN+Pi4wPn5o6z4+YmV9vhzgPP6YGzx"
    "+09b7ftCTev9OELp/is05v4kK+T/HiXj/xsh4/8XHeL/Fxjr/xpCq/8lkEj/KZ1F/yugSP8vpE//NKhW/zitW/49sWH+Q7dp/Ui6"
    "bvxPvnP8VMB0+VbAdPdYvnL1Vrpt9FK1ZfFPsV/wTq9c706xW/UujjnszOHPsKmpqrBzc3R6AAAAAAAAAAAAAAAAAAAAAAAAAABz"
    "c3RSgoKDpvr7/9lhg+/Tm7D+/9PX/P3Dyvn5tb35+aGr+PqNmff6eYb0+2l28fxaZ+/9TFjs/j9J6v40Pef+Lzjn/ys05v8nMeX/"
    "JSrt/yhPtf82mWD/Oqpb/zqsW/89sGL/QLRn/0O4a/5Iu3H9TL91/E/BePtTwnn6VMJ4+FPAdPZSvW31TLZl8kmxXfBFrVfuQ6tQ"
    "70SoUfhKl1LZ/P381X9/f7dzc3Q+AAAAAAAAAAAAAAAAAAAAAAAAAABzc3QSc3N0sM/Pz528yvm/TXDp4c7X///S2Pr5xMv6+rjA"
    "+vqosvn7l6L4+4aS9fx0f/L8Z3Lw/lpl7v5PWuz+RVDq/z9K6v86Run/N0Hv/zpfwP9JpHn/T7Zx/1C6cv5Tvnn+VsF9/lfEgP5X"
    "xoP8V8eC+1bIgfpUxn34UsN49k6/cfVKumfzRbRf8UGuV+48qU7uO6hK9C6POe292MCvx8fHo3NzdKVzc3QMAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAc3N0VIKCg6T6+//QcY7uyYGb+/fc4f7+09n7+svS/PvCyvv7tb36+6Ww+PyYofb9iJL1/nqF8/5sefD+Ym7v"
    "/lto7v5UYu3/TVry/1B0zf5dr5L/ZcWJ/2jJjP5qy5H+bM6V/m3Qlv5v0pj9bdKX/GjQkvplzov4XsiC9lXCdvRJuWfxP7FZ7jur"
    "Uu03p0jvOqFH+FyfZMj8/fzMf3+AsHNzdEUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAc3N0C3NzdJu4uLiT3eT9u0Fo5NKu"
    "wP//5un+/trg/PvW2/z8ztT8/MXL+/y5wPr9q7T4/Z6o9/2Snfb+hZHz/nuK8/5ygvL+bHz1/mqM2v51vK7+fNSi/nzYpP5/2aj+"
    "gNur/YDbqf2A2qn8ftun+3nXoPp105j4bM2O9WPGgfNYv3TyQ7Fc7zOmSe49q0z7KIcy3efx6KyysrKYc3N0knNzdAcAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHNzdDNzc3Sz3d7jo7HC+LpIa+DZ1eH//+vt//7j5/394Of9/d3i/f7V2vz+zNH7/cDI"
    "+v60vfn+qbP4/p6r+P6Upff+jJ75/oek5/6LycL+kN+3/pDjuP6R4rr+kOS6/I/jufyN4bX6iN+v+YPbqPh71Z/2cs6S9WnHhfNh"
    "wXryTbRl7z2wUf4qjDXpttS5peLi4p5zc3S6c3N0LAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABzc3RT"
    "f3+AofDy+LWYrvW6VHXg3Ojx///2+P//6+7+/ezv/f7r7v7+5On9/tzh/f7S2Pz+x8/7/rzH+/6zwfv+qLr8/p+58f2h09b+pOfH"
    "/qLrx/2h6sf9nujF/JzowvuX5b36kuG2+Yrdq/iC16L2eM+W9W7HifJmwn3yUr5n/y6RO+mSv5al9ff2r35+f6hzc3RLAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAc3N0aIqKi5f2+P66ma/2uk5x4NzY5P///v7///b3///2"
    "9///9vb+//Dz/v/o7f7/3+b9/tXe/f7K1v39wdD9/bbJ+P2z2+T9tO3T/bDw0P2s7tD8quzN/KbqyPqg5sH6mOK5+ZHcr/eI16P2"
    "f8+Y83nNkPhjwnX/KIk055K+l6T7/Pu1ioqLmHNzdGcAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAHNzdGiKiouY8PL4tbLC+LtGbOLXuMv+///////+/v///f7///r7///09///7vL+/uXr/v7b5f790d/+/cbW"
    "/P3B4+z9v/Db/brv1fy17tL8su3Q+6vpyful5sL5nuK6+Jbcr/aP1qf2jNul/1m4a/8ohzPgt9i7pvX29bCKiouXc3N0aAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABzc3Rnf39/od3e46Pc4/y8"
    "cY7rzmmJ7ezm7P7///////7+///6+///8/b//uvx//7i6v792eX+/c7d/v3K5fD8x/Hd/MDw1/y67tT7tuzP+7Dpyfqr5cP5qOPA"
    "+qXnu/950Y//OJpG8l6hZMTh7+Ox4OHgoH9/gKFzc3RoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAc3N0UnNzdLS3t7mV+Pr+0bzL+sJkhOzfbIzv78nZ/v/+/v////////T4///r"
    "8///4+r+/dfh//3R5/H8zfLe/Mfy2fzG8Nn9wO7U/b3v0f+s7sP/d8uK/zubSfJLmlTcuda8tPv9/M23uLeUc3N0tHNzdFMAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AHNzdDJzc3SbgoKDpM/Q0p76+/7ZydX8wXyZ9dxJcvLraovy85ey/f+1yP7/x9v//9Tk///T8Pz/y/zg/6/vw/+Q2KT/bcF//0Si"
    "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"
    "hPuD0JX5i86Z85LYnvl0vX//YpJnxODp4NNcX17MG3ocCAAAAABcXF6i397ft52v68OZq/v8wcn7/Jyn9/Z6ivf2XnH0+Exd8Ps6"
    "SOv8JjPn/hol5P4VHeP/Ehji/xIT6f8WjTb/GpIt/x2TNf8kmD/+L6JM/j+uXf1UunD8acWB+XbKivd2xIbxf9ON/FiwZPuFn4qs"
    "3N7bxF1eX6QAAAAAAAAAAFxcXnWwr7C21972zGaD6ObU3P7+rrb49o2b+PdzhPb4Xm/y+0tc7/w7Suv9Kzno/iEt5v4cJuX/GiHq"
    "/yCVRv8noED/K6JJ/zGmUf47rlv9R7dp/FfAdfphxHz3ZMJ682C6cfFkyXL+Qp9O78DHwa7HxsfIXl5gdQAAAAAAAAAAXFxeQXd3"
    "eMTw8ffFbIjiycDO/v/Iz/v6prD5+I+c+fl7ifb7Z3jz/VZm8P1GVu3+Okrr/jA/6v8sOO3/MaFe/zuyW/8/tWX+Q7dr/km9cPxP"
    "wXb6VcR691XCdfVSuGruT7hg9U66XP9IlVLY5OXktJ2cntJeXmBBAAAAAAAAAABcXF4VX19hmsvKzbLBy/HCcovm5d7l/v7FzPr5"
    "sbz6+qGs+fuPnvj9e4v1/Wl68v5cbvD+UWPw/kxe8P5PtIH/XcmC/l3KiP5ey4v9XsyK/FvLhflXyH72Tb9w8kKyXe9Du1f9OqBG"
    "7423ksbY19e9fX1+pV5eYBUAAAAAAAAAAFxcXgVcXF5SgH+BsOnr9LaFnOnCoLLv8+vv/v3R2Pz7xMz8+7nD+/2otPr9lqT3/oeY"
    "9v56jfb+cYn1/nPJp/6C4Kr+f96r/n7dq/x626X7cdaZ+WfMivVWvnXvRLtf+juvTfxQmVnOz93QvaGgocFnZ2lVXl5gBQAAAAAA"
    "AAAAAAAAAFxcXhRgYGKFq6uuqNrg9sBzjePKxtT9//X2/v7m6P393eL9/dDY/P3Cy/z+ssP7/qO0+/6XsPj9ldrI/p3txf2b7MT9"
    "lue++4vitPqB2aX3dc6V9GnMhPpLumH+PZVI17bQub/CxMK8cHBxi15eYBQAAAAAAAAAAAAAAAAAAAAA4ODgAVxcXjRtbW+iwcDE"
    "ptXb9ceAmevOuMbw9/r6/v729/7+7vL9/uTo/f7W4f3+xdL9/bjM+f2x5tz9sfTT/arvz/uj6sb7l+K5+Yzbq/eI3KP8ZcV7/kOY"
    "T9i41LrB0dPQvX5+gKteXmA14ODhAQAAAAAAAAAAAAAAAAAAAAAAAAAA5eXlBVtbXUpwb3Gmv77Cptrg98KUqefKjaHj6PH0///8"
    "/P7/+vr+/uzx/v7b4/79y9v7/cPr5f2/8tf9tu7T+67ryvyq7cX+meiy/V21cPdkq23TyN3Jwc3PzbiAgIKvX19hS+Xl5QUAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA6urqB1tbXUdsbG6gpKSmqd/j77TO1/PMj6bt2JSq7+7T3fz/8vj///L3//7k8v7+2vr0"
    "/tL95v7G+93+n+S1/ma1dvZapGbdsNK0zNvm3MK1tbW0d3Z4pl5eYEjq6uoHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAA7+/vBVpaXDFiYmR9d3d5sb29wrHm6PTD3uT42bLC89aSqO/edZLu62CD7PBLnFvvWqdm62+veeKTwZnYzeLP1OLr4s/I"
    "ysi8hoWHt2hoan9dXV8x7+/vBQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA9PT0AltbXRRbW11H"
    "Y2Nki21tb8ihoaO60tLWs+vr8MPy8/rc8vP69fD18frx9fDi6+7qzNXX1bqqqqq/eXl5zGZmaIxgYGFIXFxeFPT09AIAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFpaXAJaWlwSWlpcNV1dX2deXmCUX19huWFg"
    "YtdhYWLsYWFi7GFgYtdgYGK5X19hlV5eYGdcXF41XFxeElxcXgMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/"
    "//P////B//+AAP/8AAD/+AAAf/AAAD/gAAA/wAAAH4AAAA+AAAAHAAAABwAAAAMAAAADAAAAAQAAAAEAAAABAAAAAQAAAAAAAAAA"
    "AAAAAQAAAAMAAAADAAAAAwAAAAMAAAADgAAAB4AAAAfAAAAP4AAAH/AAAD/4AAB//gAB/ygAAAAYAAAAMAAAAAEAIAAAAAAAYAkA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBc"
    "ECsUWBV7EUQSKwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABoaGoLaGhqQWpqbIB+"
    "foCxjIyP1IyMj+iJiYzXd3h6tipuLcwcgB/vF1gXiwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAG1tbx6NjY+Gs7O04MjIyfja2tv/5ubm/+bm5v/l5uX/lLiV/02dUf83mDr/K4Us+C1VL2IAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAcHByS6OjpM7W1tb97Ono/9jEuP/Vq5L/3Zpz/9GOYv9Rg17/QJdF/0mlT/+BwoT/SphN/02G"
    "UPZrcG1gaGhqAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGhoagF0dHZbubm56ujm5v/cx7v/zI1o/9J+T//mj2D/6JJp/1yGTv8W"
    "jiv/HKMr/3XZgv/D9s3/Wb9n/1iZW/+LoYz3en1+bmhoagMAAAAAAAAAAAAAAAAAAAAAAAAAAHNzdUe3t7jp7evq/9Ksl//NflP/"
    "9qV3//+zg//onnP/bYU8/yOTIf8qoTT/iuCZ/4vLmP9Kq2D/nOus/z+kSv9flWH/rrWv8nZ5e1kAAAAAAAAAAAAAAAAAAAAAaGhq"
    "GJ2dnsjn5eX/066Y/9KCVv//s4T/8Kd3/+mZZv+9fkz/QIMp/ymzOv+N2Zn/c7l+/y+IWv8tjFf/abR0/5LppP8xjDn/cJty/6Wn"
    "qNlnbG4lAAAAAAAAAABoaGoOl5eZgtPT0/7cy8L/1IFU//6vgv/rn3D/4pJc/+GKTv/PfT//YXYu/0aeUv9FmVj/LYhh/zesyP86"
    "qLb/MJBH/5bVoP9w24b/NXo5/5+2oP+RlJaUZXJ0DwAAAABoaGpJubm63Ovn5f/SlHD//KFy/+6fb//hjVf/3YNF/9qAPf/Ufjf/"
    "t24o/1GTcv8sqqb/Ocbe/0rY9f9V1PH/M5tz/0GjVf+o7bb/XL9t/0mES/+grKPpZ2psSgAAAABwcHOHzc3O993Jvf/aglD/+qN2"
    "/+KMWf/afEL/1Hg3/854MP/IdSz/z3Ae/1u/vv8t4v7/N9/+/0bg+v9Z5Pr/YNTb/zKLUv9dtmv/s/TC/0WRS/9Rh1P+ZmpriD34"
    "/wGSkpa14eHi/t+2n//pjF//8Jls/91+Rv/UdDX/y3Mt/8NwJv+9bSL/wWcX/0HCy/8d3///Jtz+/zff/f9Q5Pv/cOr+/1i1ov81"
    "jkn/gc2N/63tu/+ArYT/QV1F1B9gIh2UlJjX6urq/+iqhv/vl2n/8Jhk/+F6OP/VcCL/y3Ec/8BuGP+4axb/u2IP/zDE1/8Q3///"
    "F9r//yjd//9D4v//Zun+/4js9v9HmW7/Rp9V/6Dbqf+m27H/LWsy+x1dH4GRkZXo6urr/+Kif//Zopv/xpec/7F5fP+hZ2D/l2FT"
    "/5FdSf+MWEL/j0w1/yK4yP8Lzt//Ecra/x/N2/81093/Wdzh/4Ll5P93xbL/QoxW/1WlXv9npW//N3c+/R5dIGyNjZHU6+vr/5mt"
    "7/+MoPT/hJT3/1Vr8/83Tu7/IDXp/xoo5P8WH+D/ExHk/xN6SP8TkST/Fo8w/x+WPf8xo1D/TrVt/3DGi/+Hzp3/Y61x/0KJR/92"
    "o3n/UnFW5BteHQJ1dXex3t7e/q265v+KnvP/mqf6/2d68/9LYPP/METu/x4w6/8WI+n/ERbw/xV9UP8XkR//G5At/yKWOf8zokv/"
    "TLJl/2bAff94xor/XrNo/3iqe/+9yL7/Z2lpsgAAAABoaGqBysrK9cbM4v96kvD/t7/+/3+Q8/9gc/H/S17w/zRG7P8kNOn/HSXv"
    "/yGFZP8joDj/JZxB/y2iTP87rFn/SrRn/1i5cf9funD/PJ1D/7DFsv/IyMn6aGhqgQAAAABoaGpAt7e41ejp7P+NoeP/tsP+/6Wv"
    "+P+Dk/X/a3/1/1dq8v9EWO//NkXz/zeWgv88t13/PLRj/z+1Zv9Etmj/R7dp/0mvYv9DsVP/WZ9c/+fq5/+zs7TfaGhqQAAAAABo"
    "aGoKk5OVdMvLzPzM0OH/gprv/8/W/v+1vvr/oa/5/4qa+P90h/X/YXX3/1+vqf9m0o//ZM6P/17Ki/9YxYD/Trlx/z+xWf83mT3/"
    "s8i0/9bW1v6NjY+FaGhqCgAAAAAAAAAAaGhqEpmZmrvj4+X/rrne/56y+P/h6P7/19z9/8LL+/+ruvr/laj7/4rJzf+P57j/jOG1"
    "/4Laq/92z5j/ZcaC/zyoSf+Jsov/5ebl/6Kio85oaGocAAAAAAAAAAAAAAAAAAAAAG1tbzuwsLHg6+zt/7C73/+Qpe//197+/+3w"
    "/v/d5/7/xdH+/7Tc5f+x8dL/qO3L/5vmvf990pf/TKdW/463kf/q6+r/ubm56nBwckoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABs"
    "bG5JsLCx3+Pj5f/N0uL/m63r/5ar9v+zwvn/vMz8/7nX9P+c2q7/fMaM/16uZ/9qqm7/vs++/+Pl5P+3t7jpdHR2WwAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaGhqOZqanLzMzM766Ort/8nP5f+1wu3/orb1/5St9f+Eu4b/msOc/7rPu//j5+T/09TU"
    "/J6eoMhvb3BGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHBwchSTk5R1t7e41crKyvTb29v+5+fn/+jo"
    "6f/o6Oj/3d3e/szMzfa5ubrcmZmag3NzdRsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABo"
    "aGoLaGhqQWhoaoB7e32xgoKE1JCQkuiNjZDXcHBytWhoaodoaGpIaGhqDwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD//H8A"
    "/AB/APgAPwDwAA8AwAAHAMAABwCAAAMAAAABAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAEAAAABAAAAAQCAAAMAwAAH"
    "AOAADwDwAB8A+AA/APwAfwAoAAAAEAAAACAAAAABACAAAAAAAEAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGho"
    "akZoaGqNa2ttx25ucPBwcHLwZG1mzTtpPP8yYzPCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaGhqIISEhpmsrK341NTV/+Xh3//Z"
    "0dD/xcXF/ztpPP8m4En/O2k8/01pT9FoaGogAAAAAAAAAAAAAAAAaGhqIJeXmLjQz9D/5tfO/7yWgv+pbFD/zIho/ztpPP8m4En/"
    "x/bN/ybgSf87aTz/Y3xk2mhoaiAAAAAAAAAAAISEhpnQz9D/0sK4/6VqUf/1qX3//qp//ztpPP8m4En//////ztpPP/O+tr/JuBJ"
    "/ztpPP9YdFrJAAAAAGhoakasrK346eHd/6FoUP/+r4L/751o/9l+Rf+4din/O2k8/ztpPP9Fn5v/O2k8/8762v8m4En/O2k8/0dq"
    "SJ1oaGqN1NTV/7mcjf/fkG3/9ptp/9N6QP+8bCr/plwv/0S+z/8i2///Rtj+/1yukv86eTD/zvra/ybgSf87aTz/a2ttx/Pz8/+v"
    "cFL//6R4/+aARv/JbCL/tWcX/6ZXIP8xwtj/Ft7//zHb//+X7f7/cbeJ/ztpPP/O+tr/Sl4454SEhvD09PT/qGxV/+esp//AfXb/"
    "ml5M/4xWQf+MSDP/I7jH/xDM4/8hyuL/Ytj2/3re/v92uX7/O2k8/0pqOPqBgYPw9PT0/0FU0v+puf//RHT7/ylB6f8aJOD/FRXk"
    "/xV7SP8XjiT/K5hC/2zHiP960pf/ZJZq/7rdvf99gIDxaWlrx/Pz8/9GVsv/sr/+/5Gh+v9EU+7/Ii7p/xcd8P8Yf1H/HJIj/z6r"
    "Vv92z5L/eNOI/0SDSv/q8uv/eHh6x2hoao3U1NX/pau//01a0f/P2P7/dYHx/0lP6/8dKu//L49r/zeqTP9ZxYH/XMqA/0m9Xv9L"
    "f1D/+fr6/2hoao1oaGpGrKyt+Ozs7v9NV8f/p7Td/97g+/+AjPb/dH33/3a+rf+E26b/jOSz/23Ui/8+fkb/8PDw/6ysrfhoaGpG"
    "AAAAAISEhpnQ0NH/2Nnf/1FczP+Rnsb/7PD//+jw///O8/L/t/bT/4jSnv9KhVL/7O7s/9DQ0f+EhIaZAAAAAAAAAABoaGogl5eY"
    "uNDQ0f/q6+7/p63A/1Bby/9HWdD/Rn9Q/0uAUP+Lo4//+Pn4/9DQ0f+Xl5i4aGhqIAAAAAAAAAAAAAAAAGhoaiCEhIaZrKyt+NTU"
    "1f/z8/P/8/Pz//Pz9P/z8/P/1NTV/6ysrfiEhIaZaGhqIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGhoakZoaGqNaGhqx3R0dvB4"
    "eHvwaGhqx2hoao1oaGpGAAAAAAAAAAAAAAAAAAAAAPAPAADAAwAAgAEAAIABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAIABAACAAQAAwAMAAPAPAAA=";

static Bytes DecodeFirstLinkIcon() {
    Bytes result;
    unsigned acc = 0, bits = 0;
    for (const char* p = kFirstLinkIconBase64; *p; ++p) {
        const unsigned char c = static_cast<unsigned char>(*p);
        if (c == '=') break;
        int v = c >= 'A' && c <= 'Z' ? c - 'A' :
                c >= 'a' && c <= 'z' ? c - 'a' + 26 :
                c >= '0' && c <= '9' ? c - '0' + 52 :
                c == '+' ? 62 : c == '/' ? 63 : -1;
        if (v < 0) return {};
        acc = (acc << 6) | static_cast<unsigned>(v);
        bits += 6;
        if (bits >= 8) { bits -= 8; result.push_back(uint8_t(acc >> bits)); }
    }
    return result;
}

// The hub's Link_SetDefaultProgram_Icon uses icon(4,24rp,24rp).
// Replace group 4, retaining all other icon groups and their images. Allocate
// unused RT_ICON names so images shared by other groups cannot be changed.
static bool ReplaceFirstLinkIcon(std::vector<ResItem>& items) {
    const Bytes ico = DecodeFirstLinkIcon();
    if (ico.size() < 6 || R16(ico, 0) != 0 || R16(ico, 2) != 1) return false;
    const unsigned count = R16(ico, 4);
    if (!count || count > 64 || 6u + size_t(count) * 16u > ico.size()) return false;
    std::vector<uint32_t> langs;
    for (const ResItem& r : items) {
        if (r.type.is(14u) && r.name.is(4u)) langs.push_back(r.lang);
    }
    if (langs.empty()) return false;
    std::vector<uint16_t> ids;
    for (unsigned candidate = 65535; candidate && ids.size() < count; --candidate) {
        bool used = false;
        for (const ResItem& r : items) {
            if (r.type.is(3u) && r.name.is(candidate)) { used = true; break; }
        }
        if (!used) ids.push_back(uint16_t(candidate));
    }
    if (ids.size() != count) return false;
    Bytes group;
    P16(group, 0); P16(group, 1); P16(group, uint16_t(count));
    std::vector<ResItem> images;
    for (unsigned i = 0; i < count; ++i) {
        const size_t entry = 6u + size_t(i) * 16u;
        const uint32_t size = R32(ico, entry + 8), offset = R32(ico, entry + 12);
        if (!size || offset > ico.size() || size > ico.size() - offset) return false;
        group.insert(group.end(), ico.begin() + entry, ico.begin() + entry + 12);
        P16(group, ids[i]);
        for (uint32_t lang : langs) {
            ResItem image;
            image.type.id = 3; image.name.id = ids[i]; image.lang = lang;
            image.isNew = true;
            image.data.assign(ico.begin() + offset, ico.begin() + offset + size);
            images.push_back(std::move(image));
        }
    }
    for (ResItem& r : items) {
        if (r.type.is(14u) && r.name.is(4u)) { r.isNew = true; r.data = group; }
    }
    for (ResItem& r : images) items.push_back(std::move(r));
    return true;
}

// The new resource section: directory, names, data entries, then the new data. Old data keeps its RVA.
Bytes BuildDirectory(std::vector<ResItem>& items, uint32_t sectionRva) {
    std::stable_sort(items.begin(), items.end(), [](const ResItem& a, const ResItem& b) {
        if (!(a.type == b.type)) return a.type < b.type;
        if (!(a.name == b.name)) return a.name < b.name;
        return a.lang < b.lang;
    });
    // Group boundaries.
    struct NameGroup { size_t first, count; };
    struct TypeGroup { size_t first; std::vector<NameGroup> names; };
    std::vector<TypeGroup> types;
    for (size_t i = 0; i < items.size(); i++) {
        if (types.empty() || !(items[types.back().first].type == items[i].type)) types.push_back({i, {}});
        std::vector<NameGroup>& ng = types.back().names;
        if (ng.empty() || !(items[ng.back().first].name == items[i].name)) ng.push_back({i, 0});
        ng.back().count++;
    }
    size_t dirs = 1, entries = types.size();
    for (const TypeGroup& t : types) {
        dirs += 1 + t.names.size();
        entries += t.names.size();
        for (const NameGroup& n : t.names) entries += n.count;
    }
    const size_t dirSize = dirs * 16 + entries * 8;
    // Names, in directory order, each once.
    std::vector<std::pair<std::wstring, uint32_t>> nameOffsets;
    Bytes nameBlob;
    auto nameOffset = [&](const std::wstring& s) -> uint32_t {
        for (const auto& p : nameOffsets)
            if (p.first == s) return p.second;
        const uint32_t o = uint32_t(dirSize + nameBlob.size());
        nameOffsets.emplace_back(s, o);
        P16(nameBlob, uint16_t(s.size()));
        for (wchar_t c : s) P16(nameBlob, uint16_t(c));
        return o;
    };
    for (const TypeGroup& t : types) {
        if (items[t.first].type.isName) nameOffset(items[t.first].type.name);
        for (const NameGroup& n : t.names)
            if (items[n.first].name.isName) nameOffset(items[n.first].name.name);
    }
    while ((dirSize + nameBlob.size()) % 4) nameBlob.push_back(0);
    const size_t leavesOff = dirSize + nameBlob.size();
    const size_t dataOff = leavesOff + items.size() * 16;

    Bytes dir(dirSize, 0), leaves, data;
    auto key = [&](const ResId& id) -> uint32_t { return id.isName ? (0x80000000u | nameOffset(id.name)) : id.id; };
    auto header = [&](size_t at, size_t named, size_t total) {
        W16(dir, at + 12, uint16_t(named));
        W16(dir, at + 14, uint16_t(total - named));
    };
    size_t pos = 16 + 8 * types.size();
    size_t namedTypes = 0;
    for (const TypeGroup& t : types) namedTypes += items[t.first].type.isName;
    header(0, namedTypes, types.size());
    for (size_t ti = 0; ti < types.size(); ti++) {
        const TypeGroup& t = types[ti];
        const size_t tdir = pos;
        pos += 16 + 8 * t.names.size();
        W32(dir, 16 + 8 * ti, key(items[t.first].type));
        W32(dir, 20 + 8 * ti, 0x80000000u | uint32_t(tdir));
        size_t named = 0;
        for (const NameGroup& n : t.names) named += items[n.first].name.isName;
        header(tdir, named, t.names.size());
        for (size_t ni = 0; ni < t.names.size(); ni++) {
            const NameGroup& n = t.names[ni];
            const size_t ndir = pos;
            pos += 16 + 8 * n.count;
            W32(dir, tdir + 16 + 8 * ni, key(items[n.first].name));
            W32(dir, tdir + 20 + 8 * ni, 0x80000000u | uint32_t(ndir));
            header(ndir, 0, n.count);
            for (size_t li = 0; li < n.count; li++) {
                const ResItem& item = items[n.first + li];
                uint32_t rva = item.rva, size = item.size;
                if (item.isNew) {
                    while (data.size() % 8) data.push_back(0);
                    rva = sectionRva + uint32_t(dataOff + data.size());
                    size = uint32_t(item.data.size());
                    data.insert(data.end(), item.data.begin(), item.data.end());
                }
                W32(dir, ndir + 16 + 8 * li, item.lang);
                W32(dir, ndir + 20 + 8 * li, uint32_t(leavesOff + leaves.size()));
                P32(leaves, rva); P32(leaves, size); P32(leaves, item.codepage); P32(leaves, 0);
            }
        }
    }
    Bytes out;
    out.reserve(dirSize + nameBlob.size() + leaves.size() + data.size());
    out.insert(out.end(), dir.begin(), dir.end());
    out.insert(out.end(), nameBlob.begin(), nameBlob.end());
    out.insert(out.end(), leaves.begin(), leaves.end());
    out.insert(out.end(), data.begin(), data.end());
    return out;
}

// PE checksum; the field itself counts as zero.
uint32_t Checksum(const Bytes& d, size_t at) {
    uint64_t s = 0;
    const size_t n = d.size();
    for (size_t i = 0; i + 1 < n; i += 2) {
        if (i >= at && i < at + 4) continue;
        s += uint32_t(d[i]) | uint32_t(d[i + 1]) << 8;
        s = (s & 0xFFFF) + (s >> 16);
    }
    if (n & 1) { s += d[n - 1]; s = (s & 0xFFFF) + (s >> 16); }
    s = (s & 0xFFFF) + (s >> 16);
    return uint32_t(s + n);
}

// `isPage`: the module is sud.dll (rename of the DirectUI import, page definition); otherwise it is the DirectUI copy.
bool Generate(const Bytes& src, const StringTable& table, bool isPage, const std::wstring& selfPath, Bytes& out,
              std::string& error) {
    const char* what = isPage ? "sud.dll" : "dui70.dll";
    out = src;
    // 1. The private DirectUI: exactly one name to replace (the import descriptor's).
    if (isPage) {
        size_t count = 0;
        const size_t at = FindDuiImport(out, count);
        if (count != 1) { error = "sud.dll: the DirectUI import name (dui70.dll or dui17.dll) was not found exactly once"; return false; }
        memcpy(&out[at], kDuiNew, sizeof(kDuiNew));
    }
    // 2. Resources.
    Pe pe;
    if (!pe.parse(out)) { error = std::string(what) + ": not a 64-bit PE file"; return false; }
    std::vector<ResItem> res;
    if (!ReadResources(pe, res)) { error = std::string(what) + ": unreadable resources"; return false; }
    std::vector<ResItem> items;
    bool sawDefinition = false;
    for (ResItem& r : res) {
        if (r.type.is(L"MUI") || r.type.is(kRtString)) continue;
        if (isPage && r.type.is(L"XMLFILE") && r.name.is(100u)) {
            r.data = ResourceBytes(pe, r);
            PatchSudDefinition(r.data, table, selfPath);
            r.isNew = true;
            sawDefinition = true;
        }
        items.push_back(std::move(r));
    }
    if (isPage && !sawDefinition) { error = "sud.dll: page definition not found"; return false; }
    if (isPage && !ReplaceFirstLinkIcon(items)) { error = "sud.dll: cannot replace first link icon"; return false; }
    std::vector<std::pair<uint32_t, Bytes>> blocks;
    BuildStringBlocks(table, blocks);
    for (auto& b : blocks) {
        ResItem s;
        s.type.id = kRtString; s.name.id = b.first; s.lang = 0; s.isNew = true; s.data = std::move(b.second);
        items.push_back(std::move(s));
    }
    // 3. A new section after the last one.
    const uint32_t salign = R32(out, pe.opt + 32), falign = R32(out, pe.opt + 36);
    if (!salign || !falign) { error = std::string(what) + ": bad alignment"; return false; }
    uint32_t lastVa = 0, lastSize = 0, firstRaw = 0xFFFFFFFFu;
    for (const Section& s : pe.sections) {
        if (s.va >= lastVa) { lastVa = s.va; lastSize = std::max<uint32_t>(s.vsize, 1); }
        if (s.rawPtr) firstRaw = std::min(firstRaw, s.rawPtr);
    }
    const uint32_t newRva = (lastVa + lastSize + salign - 1) / salign * salign;
    const Bytes blob = BuildDirectory(items, newRva);
    const uint32_t rawSize = uint32_t((blob.size() + falign - 1) / falign * falign);
    while (out.size() % falign) out.push_back(0);
    const uint32_t rawPtr = uint32_t(out.size());
    out.insert(out.end(), blob.begin(), blob.end());
    out.resize(out.size() + (rawSize - blob.size()), 0);
    // The header of the new section takes the place of the bound-import table, which is stale on any other Windows
    // than the one the file was built for and is ignored then.
    const size_t hdr = pe.sec + 40 * pe.nsec;
    if (hdr + 40 > firstRaw) { error = std::string(what) + ": no room for a section header"; return false; }
    W32(out, pe.dd + 8 * 11, 0);
    W32(out, pe.dd + 8 * 11 + 4, 0);
    W32(out, pe.dd + 8 * 4, 0);                                         // no certificate table: the file is changed
    W32(out, pe.dd + 8 * 4 + 4, 0);
    for (size_t k = hdr; k < firstRaw; k++) out[k] = 0;
    memcpy(&out[hdr], ".rsrc7\0\0", 8);
    W32(out, hdr + 8, uint32_t(blob.size()));
    W32(out, hdr + 12, newRva);
    W32(out, hdr + 16, rawSize);
    W32(out, hdr + 20, rawPtr);
    W32(out, hdr + 36, 0x40000040);                                    // initialized data, readable
    W16(out, pe.fh + 2, uint16_t(pe.nsec + 1));
    W32(out, pe.dd + 8 * 2, newRva);
    W32(out, pe.dd + 8 * 2 + 4, uint32_t(blob.size()));
    W32(out, pe.opt + 56, uint32_t((newRva + blob.size() + salign - 1) / salign * salign));   // SizeOfImage
    W32(out, pe.opt + 64, 0);
    W32(out, pe.opt + 64, Checksum(out, pe.opt + 64));
    return true;
}

}  // namespace gen
// <gen-end>

// =====================================================================================================================
// Runtime
// =====================================================================================================================

std::atomic<bool> g_stopping{false};
HANDLE g_stopEvent = nullptr;
HANDLE g_setupThread = nullptr;
std::atomic<HINTERNET> g_request{nullptr};
std::atomic<bool> g_keepFiles{true};
std::atomic<int> g_languageSetting{0};      // 0 automatic, 1 English, 2 Italian, 3 German, 4 Spanish, 5 French, 6 Brazilian Portuguese

SRWLOCK g_stateLock = SRWLOCK_INIT;         // guards the block below
std::wstring g_spadPath;                    // optional, unmodified Microsoft 1607 executable
std::wstring g_pagePath;                    // the private copy of sud.dll, once it is loaded
std::wstring g_storage;
HMODULE g_pageModule = nullptr;             // loaded by the setup thread; never unloaded
HMODULE g_duiModule = nullptr;
using DllGetClassObject_t = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, LPVOID*);
std::atomic<DllGetClassObject_t> g_getClassObject{nullptr};
std::atomic<bool> g_pageReady{false};       // the hooks hand out the private copy only when this is true

// ---- RAII helpers ---------------------------------------------------------------------------------------------------
// Every handle, key, lock and COM reference taken in this file is owned by one of these, so that it is released on every
// path, including the one of an exception (std::bad_alloc from a string or a vector).

struct FileHandle {
    HANDLE h;
    explicit FileHandle(HANDLE handle = INVALID_HANDLE_VALUE) : h(handle) {}
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    ~FileHandle() { close(); }
    bool valid() const { return h != INVALID_HANDLE_VALUE && h != nullptr; }
    HANDLE get() const { return h; }
    void close() {
        if (valid()) CloseHandle(h);
        h = INVALID_HANDLE_VALUE;
    }
};

struct FindHandle {
    HANDLE h;
    explicit FindHandle(HANDLE handle = INVALID_HANDLE_VALUE) : h(handle) {}
    FindHandle(const FindHandle&) = delete;
    FindHandle& operator=(const FindHandle&) = delete;
    ~FindHandle() {
        if (valid()) FindClose(h);
    }
    bool valid() const { return h != INVALID_HANDLE_VALUE && h != nullptr; }
    HANDLE get() const { return h; }
};

struct InetHandle {
    HINTERNET h;
    explicit InetHandle(HINTERNET handle = nullptr) : h(handle) {}
    InetHandle(const InetHandle&) = delete;
    InetHandle& operator=(const InetHandle&) = delete;
    ~InetHandle() {
        if (h) WinHttpCloseHandle(h);
    }
    explicit operator bool() const { return h != nullptr; }
    HINTERNET get() const { return h; }
    void reset(HINTERNET handle) {
        if (h) WinHttpCloseHandle(h);
        h = handle;
    }
};

struct RegKey {
    HKEY k = nullptr;
    RegKey() = default;
    RegKey(const RegKey&) = delete;
    RegKey& operator=(const RegKey&) = delete;
    ~RegKey() {
        if (k) RegCloseKey(k);
    }
};

// A named mutex that is closed, and released if it was acquired, when the object goes away.
struct NamedMutex {
    HANDLE h;
    bool owned = false;
    explicit NamedMutex(HANDLE handle) : h(handle) {}
    NamedMutex(const NamedMutex&) = delete;
    NamedMutex& operator=(const NamedMutex&) = delete;
    ~NamedMutex() {
        if (!h) return;
        if (owned) ReleaseMutex(h);
        CloseHandle(h);
    }
};

struct SharedLock {
    SRWLOCK* lock;
    explicit SharedLock(SRWLOCK& l) : lock(&l) { AcquireSRWLockShared(lock); }
    SharedLock(const SharedLock&) = delete;
    SharedLock& operator=(const SharedLock&) = delete;
    ~SharedLock() { ReleaseSRWLockShared(lock); }
};

struct ExclusiveLock {
    SRWLOCK* lock;
    explicit ExclusiveLock(SRWLOCK& l) : lock(&l) { AcquireSRWLockExclusive(lock); }
    ExclusiveLock(const ExclusiveLock&) = delete;
    ExclusiveLock& operator=(const ExclusiveLock&) = delete;
    ~ExclusiveLock() { ReleaseSRWLockExclusive(lock); }
};

// The string returned by Wh_GetStringSetting, freed with Wh_FreeStringSetting.
struct StringSetting {
    PCWSTR value;
    explicit StringSetting(PCWSTR v) : value(v) {}
    StringSetting(const StringSetting&) = delete;
    StringSetting& operator=(const StringSetting&) = delete;
    ~StringSetting() {
        if (value) Wh_FreeStringSetting(value);
    }
};

struct ComDeleter {
    void operator()(IUnknown* p) const {
        if (p) p->Release();
    }
};

bool ReadWholeFile(const std::wstring& path, Bytes& out, DWORD share = FILE_SHARE_READ | FILE_SHARE_DELETE) {
    out.clear();
    try {
        FileHandle f(CreateFileW(path.c_str(), GENERIC_READ, share, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!f.valid()) return false;
        LARGE_INTEGER size;
        bool ok = GetFileSizeEx(f.get(), &size) && size.QuadPart > 0 && size.QuadPart < 16 * 1024 * 1024;
        if (ok) {
            out.resize(size_t(size.QuadPart));
            size_t done = 0;
            while (ok && done < out.size()) {
                DWORD got = 0;
                ok = ReadFile(f.get(), out.data() + done, DWORD(out.size() - done), &got, nullptr) && got;
                done += got;
            }
        }
        if (!ok) out.clear();
        return ok;
    } catch (...) {
        out.clear();
        return false;
    }
}

void EnsureDirs(const std::wstring& path) {     // creates every folder level of `path`
    try {
        for (size_t i = 3; i <= path.size(); i++)
            if (i == path.size() || path[i] == L'\\') CreateDirectoryW(path.substr(0, i).c_str(), nullptr);
    } catch (...) {
    }
}

// Writes through a temporary file and renames it into place, so a reader never sees a partial file.
bool WriteFileAtomic(const std::wstring& path, const Bytes& data) {
    try {
        wchar_t suffix[32];
        swprintf(suffix, ARRAYSIZE(suffix), L".%lu.tmp", GetCurrentProcessId());
        const std::wstring tmp = path + suffix;
        bool ok = true;
        {
            FileHandle f(CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
            if (!f.valid()) return false;
            size_t done = 0;
            while (ok && done < data.size()) {
                DWORD put = 0;
                ok = WriteFile(f.get(), data.data() + done, DWORD(std::min<size_t>(data.size() - done, 1 << 20)), &put,
                               nullptr) && put;
                done += put;
            }
            ok = ok && FlushFileBuffers(f.get());
        }      // the file is closed here, before it is renamed
        if (ok) ok = MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
        if (!ok) DeleteFileW(tmp.c_str());
        return ok;
    } catch (...) {
        return false;
    }
}

// Makes `path` hold exactly `data`. A file that already does is left alone (it may be in use by another Explorer).
bool EnsureFile(const std::wstring& path, const Bytes& data) {
    try {
        Bytes have;
        if (ReadWholeFile(path, have, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE) && have == data) return true;
        return WriteFileAtomic(path, data);
    } catch (...) {
        return false;
    }
}

void CancelDownload() {
    if (HINTERNET r = g_request.exchange(nullptr)) WinHttpCloseHandle(r);
}

bool Download(const std::wstring& path, uint32_t expectedSize, Bytes& out) {
    out.clear();
    try {
        InetHandle session(WinHttpOpen(L"Microsoft-Symbol-Server/10.0.0.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                       WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
        if (!session)
            session.reset(WinHttpOpen(L"Microsoft-Symbol-Server/10.0.0.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
        if (!session) return false;
        WinHttpSetTimeouts(session.get(), 15000, 15000, 20000, 20000);
        InetHandle connect(WinHttpConnect(session.get(), kSymbolHost, INTERNET_DEFAULT_HTTPS_PORT, 0));
        if (!connect) return false;
        HINTERNET request = WinHttpOpenRequest(connect.get(), L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                               WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!request) return false;
        g_request.store(request);
        // The request handle is shared with the canceller: whoever takes it out of g_request closes it. Declared after
        // `session` and `connect`, so it is closed first.
        struct RequestRelease {
            ~RequestRelease() {
                if (HINTERNET r = g_request.exchange(nullptr)) WinHttpCloseHandle(r);
            }
        } requestRelease;

        bool ok = false;
        const ULONGLONG deadline = GetTickCount64() + 120000;
        DWORD status = 0, len = sizeof(status);
        if (!g_stopping.load() &&
            WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
            WinHttpReceiveResponse(request, nullptr) &&
            WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
                                &status, &len, WINHTTP_NO_HEADER_INDEX) &&
            status == 200) {
            out.reserve(expectedSize);
            ok = true;
            for (;;) {
                uint8_t buf[16384];
                DWORD got = 0;
                if (g_stopping.load() || GetTickCount64() > deadline || !WinHttpReadData(request, buf, sizeof(buf), &got)) {
                    ok = false;
                    break;
                }
                if (!got) break;
                if (out.size() + got > expectedSize) { ok = false; break; }      // more than the pinned size: not our file
                out.insert(out.end(), buf, buf + got);
            }
        } else if (status) {
            Wh_Log(L"Download: HTTP status %lu", status);
        }
        if (!ok) out.clear();
        return ok;
    } catch (...) {
        out.clear();
        return false;
    }
}


// =====================================================================================================================
// Compatibility code adapted from work by repensky (https://github.com/repensky): the hooks of the private copy of
// sud.dll are set when the copy is loaded, the lists of the settings are guarded by a lock, a missing symbol of
// shell32.dll is no longer fatal.
// =====================================================================================================================

namespace compat {

static SRWLOCK g_listLock = SRWLOCK_INIT;      // guards g_additionalBlacklist and g_iconOverrides

static bool g_enableLogging = false;
static bool g_hideSystemApps = false;
static bool g_hideStoreApps = false;
static std::vector<std::wstring> g_additionalBlacklist;

// Thread-local flag: set to true when sud.dll code is active
static thread_local bool g_inSudContext = false;

struct IconOverride {
    std::wstring appName;
    std::wstring iconPath;
    int iconIndex;
};
static std::vector<IconOverride> g_iconOverrides;

// ============================================================================
// Registry queries and Shell handler operations do not write directly.
// ============================================================================

static void LoadBlacklistImpl()
{
    g_additionalBlacklist.clear();
    for (int i = 0; ; i++) {
        LPCWSTR value = Wh_GetStringSetting(L"additionalBlacklist[%d]", i);
        if (!value || !value[0]) {
            Wh_FreeStringSetting(value);
            break;
        }
        g_additionalBlacklist.push_back(value);
        Wh_FreeStringSetting(value);
    }
}

static void LoadIconOverridesImpl()
{
    g_iconOverrides.clear();
    for (int i = 0; ; i++) {
        LPCWSTR appName = Wh_GetStringSetting(L"iconOverrides[%d].appName", i);
        if (!appName || !appName[0]) {
            Wh_FreeStringSetting(appName);
            break;
        }
        LPCWSTR iconPath = Wh_GetStringSetting(L"iconOverrides[%d].iconPath", i);
        int iconIndex = Wh_GetIntSetting(L"iconOverrides[%d].iconIndex", i);

        IconOverride ov;
        ov.appName = appName;
        ov.iconPath = iconPath ? iconPath : L"";
        ov.iconIndex = iconIndex;

        if (!ov.appName.empty() && !ov.iconPath.empty()) {
            g_iconOverrides.push_back(ov);
        }

        Wh_FreeStringSetting(appName);
        if (iconPath) Wh_FreeStringSetting(iconPath);
    }
}

// Check if an app has an icon override; matches against registry name, display name, or capabilities path
static bool FindIconOverride(LPCWSTR pszAppName, LPCWSTR pszDisplayName, IconOverride& out)
{
    SharedLock lock(g_listLock);
    for (const auto& ov : g_iconOverrides) {
        if ((pszAppName && _wcsicmp(pszAppName, ov.appName.c_str()) == 0) ||
            (pszDisplayName && _wcsicmp(pszDisplayName, ov.appName.c_str()) == 0)) {
            out = ov;
            return true;
        }
    }
    return false;
}

// ============================================================================
// Shell association handling
// ============================================================================

using GenerateUserAssocChangeNotification_t = long(__cdecl*)(const WCHAR*);
static GenerateUserAssocChangeNotification_t p_GenerateUserAssocChangeNotification = nullptr;

// Shell handler route. No direct UserChoice writes.
// A handler must identify the exact registered ProgID, and the effective
// executable must agree after the operation; otherwise report failure.
static bool AssocExecutable(LPCWSTR name, std::wstring& out) {
    DWORD count = 0;
    HRESULT hr = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE, name, L"open", nullptr, &count);
    if ((hr != S_FALSE && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) || !count || count > 32768) return false;
    std::vector<wchar_t> buf(count);
    hr = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE, name, L"open", buf.data(), &count);
    if (FAILED(hr) || !buf[0]) return false;
    out = buf.data();
    return true;
}
// Adapted approach inspired by win7-open-with-dialog (babamohammed, MIT):
// find a Shell handler, call MakeDefault and check the observed effective default.
// Never write the class key or UserChoice directly.
static LSTATUS ReadChoice(const WCHAR* ext, std::wstring& choice) {
    std::wstring path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\";
    path += ext; path += L"\\UserChoice";
    HKEY key = nullptr;
    LSTATUS st = RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, KEY_READ, &key);
    if (st != ERROR_SUCCESS) return st;
    wchar_t buf[512] = {}; DWORD size = sizeof(buf);
    st = RegGetValueW(key, nullptr, L"ProgId", RRF_RT_REG_SZ, nullptr, buf, &size);
    RegCloseKey(key);
    if (st == ERROR_SUCCESS) choice = buf;
    // A key without a readable ProgId is NOT equivalent to no key.
    return ERROR_SUCCESS;
}
static long SafeSetUserChoiceAndHash(const WCHAR* extension, const WCHAR* progId) {
    if (!extension || extension[0] != L'.' || !extension[1] || !progId || !*progId ||
        wcschr(extension, L'\\') || wcschr(extension, L'/')) return E_INVALIDARG;
    static thread_local bool active = false;
    if (active) return E_FAIL;
    struct Guard { bool& b; Guard(bool& x):b(x){b=true;} ~Guard(){b=false;} } guard(active);
    try {
        std::wstring intended;
        if (!AssocExecutable(progId, intended) || intended.empty() || intended[0] == L'%') {
            Wh_Log(L"[DP-OPENWITH] %s -> %s: executable unresolved; refused", extension, progId);
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
        HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(init) && init != RPC_E_CHANGED_MODE) return init;
        struct ComGuard { bool own; ~ComGuard(){if(own)CoUninitialize();} } cg{SUCCEEDED(init)};
        IEnumAssocHandlers* e = nullptr;
        HRESULT hr = SHAssocEnumHandlers(extension, ASSOC_FILTER_NONE, &e);
        if (FAILED(hr) || !e) return FAILED(hr) ? hr : E_FAIL;
        struct EnumGuard { IEnumAssocHandlers* p; ~EnumGuard(){p->Release();} } eg{e};
        IAssocHandler* selected = nullptr; unsigned matches = 0;
        IAssocHandler* h = nullptr; ULONG fetched = 0;
        while (e->Next(1, &h, &fetched) == S_OK && fetched == 1 && h) {
            PWSTR name = nullptr; HRESULT nh = h->GetName(&name);
            bool match = SUCCEEDED(nh) && name &&
                (!_wcsicmp(name, progId) || !_wcsicmp(name, intended.c_str()));
            if (match) { ++matches; if (!selected) { selected = h; h->AddRef(); } }
            if (name) CoTaskMemFree(name);
            h->Release(); h = nullptr; fetched = 0;
        }
        struct HandlerGuard { IAssocHandler* p; ~HandlerGuard(){if(p)p->Release();} } hg{selected};
        if (matches != 1 || !selected) {
            Wh_Log(L"[DP-OPENWITH] %s -> %s: %u matches, refused", extension, progId, matches);
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
        std::wstring before, after, actual;
        LSTATUS beforeStatus = ReadChoice(extension, before);
        // MakeDefault can call _SetAssociation internally. Do not re-enter our
        // own page-only hook while the Shell is handling the operation.
        {
            struct ContextGuard {
                bool previous;
                ContextGuard() : previous(g_inSudContext) { g_inSudContext = false; }
                ~ContextGuard() { g_inSudContext = previous; }
            } contextGuard;
            hr = selected->MakeDefault(extension);
        }
        LSTATUS afterStatus = ReadChoice(extension, after);
        Wh_Log(L"[DP-OPENWITH] %s target=%s exe=%s MakeDefault=0x%08lX before=%s beforeStatus=%ld after=%s afterStatus=%ld",
            extension, progId, intended.c_str(), (unsigned long)hr,
            before.empty()?L"(none/unreadable)":before.c_str(), (long)beforeStatus,
            after.empty()?L"(none/unreadable)":after.c_str(), (long)afterStatus);
        if (FAILED(hr)) return hr;
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSH, nullptr, nullptr);
        std::wstring currentChoice;
        LSTATUS currentStatus = ReadChoice(extension, currentChoice);
        bool exeOk = AssocExecutable(extension, actual) && !_wcsicmp(actual.c_str(), intended.c_str());
        Wh_Log(L"[DP-OPENWITH] %s effectiveExe=%s choice=%s status=%ld; exeMatch=%d, persistence unverified",
            extension, actual.empty()?L"(unresolved)":actual.c_str(),
            currentChoice.empty()?L"(none/unreadable)":currentChoice.c_str(), (long)currentStatus, exeOk);
        // Exact ProgID or no protected choice plus matching executable: observed,
        // not a guarantee of persistence. Other identities remain unconfirmed.
        if (exeOk && ((currentStatus == ERROR_SUCCESS && !_wcsicmp(currentChoice.c_str(), progId)) ||
                      currentStatus == ERROR_FILE_NOT_FOUND)) return S_OK;
        return HRESULT_FROM_WIN32(ERROR_RETRY);
    } catch (...) { Wh_Log(L"[DP-OPENWITH] exception; outcome unknown"); return E_FAIL; }
}

static void NotifyAssocChanged(const WCHAR* pszExtension)
{
    if (p_GenerateUserAssocChangeNotification && pszExtension) {
        p_GenerateUserAssocChangeNotification(pszExtension);
    } else {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
}

// ============================================================================
// Server-side hook: _SetAssociation
// ============================================================================

using SetAssociation_t = long(__cdecl*)(const WCHAR*, const WCHAR*,
                                         ASSOCIATIONTYPE, const WCHAR*);
static SetAssociation_t SetAssociation_Original = nullptr;

long __cdecl SetAssociation_Hook(const WCHAR* param_1, const WCHAR* param_2,
                                  ASSOCIATIONTYPE param_3, const WCHAR* param_4)
{
    // Do not replace association operations originating outside the private page.
    if (!g_inSudContext) return SetAssociation_Original(param_1, param_2, param_3, param_4);
    // Types 2 (StartMenuClient) and 3 (MimeType) still work in the stub, pass them through to the original.
    if (param_3 >= AT_STARTMENUCLIENT) {
        return SetAssociation_Original(param_1, param_2, param_3, param_4);
    }

    if (!param_2 || !param_4) {
        return E_INVALIDARG;
    }

    if (g_enableLogging) {
        Wh_Log(L"_SetAssociation: type=%d, ext=\"%s\", progId=\"%s\"",
               (int)param_3, param_2, param_4);
    }

    long hr = SafeSetUserChoiceAndHash(param_2, param_4);

    if (SUCCEEDED(hr)) {
        NotifyAssocChanged(param_2);
    }

    return hr;
}

// ============================================================================
// Server-side hook: QueryAppIsDefault (public interface)
// ============================================================================

using QueryAppIsDefault_Public_t = long(__thiscall*)(void*, const WCHAR*,
                                                      int, int,
                                                      const WCHAR*, int*);
static QueryAppIsDefault_Public_t QueryAppIsDefault_Public_Original = nullptr;

using QueryAppIsDefault_Private_t = long(__thiscall*)(void*, const WCHAR*,
                                                       int, int,
                                                       const WCHAR*, int*);
static QueryAppIsDefault_Private_t p_QueryAppIsDefault_Private = nullptr;

// True only for calls made by code of the private page (sud7.*.dll). Every other caller in explorer.exe (shell
// extensions, other Explorer code) must reach the unchanged system implementation.
static bool CalledFromPage(void* returnAddress)
{
    HMODULE from = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(returnAddress), &from)) return false;
    SharedLock lock(g_stateLock);
    return from && from == g_pageModule;
}

long __thiscall QueryAppIsDefault_Public_Hook(void* pThis, const WCHAR* pszQuery,
                                               int atType, int alLevel,
                                               const WCHAR* pszAppName, int* pfDefault)
{
    if (!CalledFromPage(__builtin_return_address(0)))
        return QueryAppIsDefault_Public_Original(pThis, pszQuery, atType, alLevel, pszAppName, pfDefault);

    if (!pfDefault) return E_POINTER;

    void* pFullObject = (char*)pThis - 8;

    if (p_QueryAppIsDefault_Private) {
        long hr = p_QueryAppIsDefault_Private(pFullObject, pszQuery, atType,
                                               alLevel, pszAppName, pfDefault);
        if (g_enableLogging) {
            Wh_Log(L"QueryAppIsDefault: \"%s\" type=%d -> default=%d (0x%08X)",
                   pszQuery ? pszQuery : L"(null)", atType, *pfDefault, hr);
        }
        return hr;
    }

    return QueryAppIsDefault_Public_Original(pThis, pszQuery, atType, alLevel,
                                              pszAppName, pfDefault);
}

// ============================================================================
// Server-side hook: _QueryAppIsDefaultAll (fix invalid pointer from sud.dll)
//
// The 1607 sud.dll's CAdvancedSettingsPage::FillListViewWithMapiHandler passes an invalid pointer (value 0x1) as the output parameter. This hook validates the pointer and substitutes a local variable if it's invalid.
// ============================================================================

using QueryAppIsDefaultAll_t = long(__thiscall*)(void*, int, const WCHAR*, int*);
static QueryAppIsDefaultAll_t QueryAppIsDefaultAll_Original = nullptr;

long __thiscall QueryAppIsDefaultAll_Hook(void* pThis, int alLevel,
                                           const WCHAR* pszAppName, int* pfDefault)
{
    // Validate the output pointer - if it looks like a small integer
    // (not a real pointer), substitute a local variable to prevent crash.
    // Only for calls from the private page; all other callers are passed through untouched.
    int localDefault = 0;
    if ((uintptr_t)pfDefault < 0x10000 && CalledFromPage(__builtin_return_address(0))) {
        if (g_enableLogging)
            Wh_Log(L"_QueryAppIsDefaultAll: fixing invalid pfDefault=%p for app=\"%s\"",
                   pfDefault, pszAppName ? pszAppName : L"(null)");
        pfDefault = &localDefault;
    }

    return QueryAppIsDefaultAll_Original(pThis, alLevel, pszAppName, pfDefault);
}

// Offer the Windows UI when a page action could not be fully confirmed. Several
// asynchronous per-extension jobs can finish together, so allow only one prompt
// at a time and suppress repeats for 15 seconds. No association is changed here.
static std::atomic<bool> g_settingsPromptOpen{false};
static std::atomic<ULONGLONG> g_lastSettingsPrompt{0};

static void OfferDefaultAppsSettings()
{
    if (g_stopping.load()) return;
    bool expected = false;
    if (!g_settingsPromptOpen.compare_exchange_strong(expected, true)) return;
    struct PromptGuard {
        ~PromptGuard() { g_settingsPromptOpen.store(false); }
    } guard;
    ULONGLONG now = GetTickCount64();
    ULONGLONG last = g_lastSettingsPrompt.load();
    if (last && now - last < 15000) return;
    if (g_stopping.load()) return;
    g_lastSettingsPrompt.store(now ? now : 1);
    bool italian = g_languageSetting.load() == 2 ||
        (g_languageSetting.load() == 0 && PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_ITALIAN);
    const wchar_t* text = italian
        ? L"Non e stato possibile applicare o verificare tutte le associazioni. "
          L"Alcune potrebbero essere gia cambiate.\n\n"
          L"Vuoi aprire le impostazioni di Windows per completare la scelta?"
        : L"Some associations could not be applied or confirmed. "
          L"Others may already have changed.\n\n"
          L"Would you like to open Windows Settings to complete your selection?";
    const wchar_t* title = italian ? L"Programmi predefiniti" : L"Default Programs";
    if (MessageBoxW(nullptr, text, title, MB_YESNO | MB_ICONWARNING | MB_SETFOREGROUND) != IDYES ||
        g_stopping.load()) return;
    HINSTANCE result = ShellExecuteW(nullptr, L"open", L"ms-settings:defaultapps",
                                     nullptr, nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        Wh_Log(L"Could not open Default apps settings, ShellExecute error=%Id", reinterpret_cast<INT_PTR>(result));
        MessageBoxW(nullptr,
            italian ? L"Impossibile aprire Impostazioni. Apri manualmente App predefinite nelle Impostazioni di Windows."
                    : L"Could not open Settings. Open Default apps manually in Windows Settings.",
            title, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
    }
}

// ============================================================================
// Client-side hook: _AsyncRestoreAppDefaults in sud.dll
// ============================================================================

using AsyncRestoreAppDefaults_t = unsigned long(__cdecl*)(void*);
static AsyncRestoreAppDefaults_t AsyncRestoreAppDefaults_Original = nullptr;

static HRESULT SetAllDefaultsForApp(LPCWSTR pszAppName)
{
    if (g_enableLogging)
        Wh_Log(L"SetAllDefaultsForApp: \"%s\"", pszAppName);

    HKEY hRegApps = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\RegisteredApplications", 0, KEY_READ, &hRegApps) != ERROR_SUCCESS)
        return E_FAIL;

    WCHAR szCapPath[1024] = {};
    DWORD cbPath = sizeof(szCapPath);
    DWORD dwType = 0;
    LSTATUS ls = RegQueryValueExW(hRegApps, pszAppName, nullptr, &dwType,
                                   (LPBYTE)szCapPath, &cbPath);
    RegCloseKey(hRegApps);

    if (ls != ERROR_SUCCESS || dwType != REG_SZ)
        return E_FAIL;

    HKEY hCap = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, szCapPath, 0, KEY_READ, &hCap) != ERROR_SUCCESS)
        return E_FAIL;

    DWORD totalSet = 0, totalFailed = 0;

    auto processCategory = [&](LPCWSTR subkey) {
        HKEY hAssoc = nullptr;
        if (RegOpenKeyExW(hCap, subkey, 0, KEY_READ, &hAssoc) != ERROR_SUCCESS)
            return;

        DWORD cValues = 0;
        RegQueryInfoKeyW(hAssoc, nullptr, nullptr, nullptr, nullptr, nullptr,
                         nullptr, &cValues, nullptr, nullptr, nullptr, nullptr);

        for (DWORD i = 0; i < cValues; i++) {
            WCHAR szExt[256] = {};
            DWORD cchExt = ARRAYSIZE(szExt);
            WCHAR szProgId[256] = {};
            DWORD cbProgId = sizeof(szProgId);
            DWORD type = 0;

            if (RegEnumValueW(hAssoc, i, szExt, &cchExt, nullptr,
                              &type, (LPBYTE)szProgId, &cbProgId) == ERROR_SUCCESS &&
                type == REG_SZ && szProgId[0])
            {
                long hr = SafeSetUserChoiceAndHash(szExt, szProgId);
                if (SUCCEEDED(hr))
                    totalSet++;
                else {
                    totalFailed++;
                    if (g_enableLogging)
                        Wh_Log(L"  FAILED: %s -> %s (0x%08X)",
                               szExt, szProgId, hr);
                }
            }
        }
        RegCloseKey(hAssoc);
    };

    processCategory(L"FileAssociations");
    processCategory(L"URLAssociations");
    RegCloseKey(hCap);

    if (g_enableLogging)
        Wh_Log(L"SetAllDefaultsForApp: %u set, %u failed", totalSet, totalFailed);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    return (totalFailed == 0 && totalSet > 0) ? S_OK : (totalSet > 0) ? S_FALSE : E_FAIL;
}

unsigned long __cdecl AsyncRestoreAppDefaults_Hook(void* param_1)
{
    g_inSudContext = true;
    LPCWSTR pszAppName = (LPCWSTR)param_1;

    if (g_enableLogging)
        Wh_Log(L"_AsyncRestoreAppDefaults: \"%s\"", pszAppName ? pszAppName : L"(null)");

    HRESULT hr = E_INVALIDARG;
    if (pszAppName && pszAppName[0]) {
        hr = SetAllDefaultsForApp(pszAppName);
    }

    CoTaskMemFree(param_1);
    g_inSudContext = false;
    // S_FALSE indicates partial success, not a fully applied selection.
    if (hr != S_OK) OfferDefaultAppsSettings();
    if (hr == S_FALSE) hr = HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY);
    return (unsigned long)hr;
}

// ============================================================================
// Client-side hook: _AsyncSetAppAssoc in sud.dll
// ============================================================================

using AsyncSetAppAssoc_t = unsigned long(__cdecl*)(void*);
static AsyncSetAppAssoc_t AsyncSetAppAssoc_Original = nullptr;

// Helper: look up ProgId for a specific extension from app capabilities
static bool GetAppProgIdForExtension(LPCWSTR pszAppName, LPCWSTR pszExtension,
                                      WCHAR* pszProgIdOut, DWORD cchProgId)
{
    HKEY hRegApps = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\RegisteredApplications", 0, KEY_READ, &hRegApps) != ERROR_SUCCESS)
        return false;

    WCHAR szCapPath[1024] = {};
    DWORD cbPath = sizeof(szCapPath);
    DWORD dwType = 0;
    LSTATUS ls = RegQueryValueExW(hRegApps, pszAppName, nullptr, &dwType, (LPBYTE)szCapPath, &cbPath);
    RegCloseKey(hRegApps);

    if (ls != ERROR_SUCCESS || dwType != REG_SZ)
        return false;

    HKEY hCap = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, szCapPath, 0, KEY_READ, &hCap) != ERROR_SUCCESS)
        return false;

    bool found = false;
    auto trySubkey = [&](LPCWSTR subkey) {
        if (found) return;
        HKEY hAssoc = nullptr;
        if (RegOpenKeyExW(hCap, subkey, 0, KEY_READ, &hAssoc) != ERROR_SUCCESS)
            return;

        DWORD cbProgId = cchProgId * sizeof(WCHAR);
        DWORD type = 0;
        if (RegQueryValueExW(hAssoc, pszExtension, nullptr, &type, (LPBYTE)pszProgIdOut, &cbProgId) == ERROR_SUCCESS &&
            type == REG_SZ && pszProgIdOut[0])
        {
            found = true;
        }
        RegCloseKey(hAssoc);
    };

    trySubkey(L"FileAssociations");
    trySubkey(L"URLAssociations");
    RegCloseKey(hCap);

    return found;
}

unsigned long __cdecl AsyncSetAppAssoc_Hook(void* param_1)
{
    g_inSudContext = true;
    if (!param_1) { g_inSudContext = false; return (unsigned long)E_INVALIDARG; }

    // Extract fields from the struct (validated against sud.dll decompilation)
    LPCWSTR pszAppName   = *(LPCWSTR*)((char*)param_1 + 0x08);
    LPCWSTR pszExtension = *(LPCWSTR*)((char*)param_1 + 0x10);
    int assocType        = *(int*)((char*)param_1 + 0x18);

    if (g_enableLogging)
        Wh_Log(L"_AsyncSetAppAssoc: app=\"%s\", ext=\"%s\", type=%d",
               pszAppName ? pszAppName : L"(null)",
               pszExtension ? pszExtension : L"(null)",
               assocType);

    HRESULT hr = E_FAIL;

    // Only intercept file extensions (0) and URL protocols (1)
    if (assocType < 2 && pszAppName && pszExtension) {
        // Look up the ProgId for this extension from the app's capabilities
        WCHAR szProgId[256] = {};
        if (GetAppProgIdForExtension(pszAppName, pszExtension, szProgId, ARRAYSIZE(szProgId))) {
            if (g_enableLogging)
                Wh_Log(L"  ProgId: %s", szProgId);

            hr = SafeSetUserChoiceAndHash(pszExtension, szProgId);

            if (SUCCEEDED(hr)) {
                NotifyAssocChanged(pszExtension);
            }
        } else {
            if (g_enableLogging)
                Wh_Log(L"  No ProgId found for \"%s\" under \"%s\", refusing unsafe fallback",
                       pszExtension, pszAppName);
            hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
    } else {
        // Types 2/3 (StartMenu/MimeType) or missing function - use original
        g_inSudContext = false;
        return AsyncSetAppAssoc_Original(param_1);
    }

    // Clean up the param struct.
    {
        typedef void(__cdecl* DestroyFunc)(void*, int);
        void** vtable = *(void***)param_1;
        if (vtable) {
            DestroyFunc destroy = (DestroyFunc)vtable[0];
            if (destroy) {
                destroy(param_1, 1);
            }
        }
    }

    g_inSudContext = false;
    if (FAILED(hr)) OfferDefaultAppsSettings();
    return (unsigned long)hr;
}

// ============================================================================
// Icon override helper: apply override to CAppConfig object after creation
// ============================================================================

static void ApplyIconOverride(void* pAppConfig, const IconOverride* ov)
{
    if (!pAppConfig || !ov) return;

    char* obj = (char*)pAppConfig;
    LPCWSTR iconPath = ov->iconPath.c_str();
    int iconIndex = ov->iconIndex;

    // Get the system image list cached index for this icon
    int cachedIdx = Shell_GetCachedImageIndexW(iconPath, iconIndex, 0);
    if (cachedIdx == -1) {
        cachedIdx = Shell_GetCachedImageIndexW(iconPath, iconIndex, GIL_FORSHELL);
    }

    if (cachedIdx != -1) {
        *(int*)(obj + 0x30) = cachedIdx;
        *(int*)(obj + 0x68) = cachedIdx;
    }

    HIMAGELIST hilLarge = nullptr, hilSmall = nullptr;
    if (cachedIdx != -1 && Shell_GetImageLists(&hilLarge, &hilSmall)) {
        HICON hOldSmall = *(HICON*)(obj + 0x38);
        HICON hOldLarge = *(HICON*)(obj + 0x40);

        // Null first so a partial failure cannot leave a stale handle for the destructor to double-free
        *(HICON*)(obj + 0x38) = nullptr;
        *(HICON*)(obj + 0x40) = nullptr;

        if (hilSmall)
            *(HICON*)(obj + 0x38) = ImageList_GetIcon(hilSmall, cachedIdx, ILD_NORMAL);
        if (hilLarge)
            *(HICON*)(obj + 0x40) = ImageList_GetIcon(hilLarge, cachedIdx, ILD_NORMAL);

        if (hOldSmall) DestroyIcon(hOldSmall);
        if (hOldLarge) DestroyIcon(hOldLarge);
    }

    if (g_enableLogging) {
        LPCWSTR appName = *(LPCWSTR*)(obj + 0x08);
        Wh_Log(L"Icon override applied: app=\"%s\", path=\"%s\", index=%d, cached=%d",
               appName ? appName : L"(null)", iconPath, iconIndex, cachedIdx);
    }
}

// ============================================================================
// Client-side hook: BuildAppConfig in sud.dll (app list filter + icon override)
// ============================================================================

using BuildAppConfig_t = long(__cdecl*)(const WCHAR*, const WCHAR*, void**);
static BuildAppConfig_t BuildAppConfig_Original = nullptr;

static bool ShouldFilterApp(LPCWSTR pszCapabilitiesPath)
{
    // Only filter system UWP components (cw5n1h2txyewy publisher).
    // Store apps use 8wekyb3d8bbwe or other publisher IDs and should not be filtered here.
    if (pszCapabilitiesPath && StrStrIW(pszCapabilitiesPath, L"AppModel\\Repository\\Packages")) {
        if (!StrStrIW(pszCapabilitiesPath, L"_cw5n1h2txyewy\\")) {
            return false; // Not a system component, don't filter
        }
    }

    // Try opening under both HKCU and HKLM
    HKEY hCap = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, pszCapabilitiesPath, 0, KEY_READ, &hCap) != ERROR_SUCCESS) {
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, pszCapabilitiesPath, 0, KEY_READ, &hCap) != ERROR_SUCCESS) {
            return false;
        }
    }

    // Check if FileAssociations subkey exists and has real (non-GUID) values
    HKEY hFileAssoc = nullptr;
    bool hasRealFileAssocs = false;
    if (RegOpenKeyExW(hCap, L"FileAssociations", 0, KEY_READ, &hFileAssoc) == ERROR_SUCCESS) {
        DWORD cValues = 0;
        RegQueryInfoKeyW(hFileAssoc, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &cValues, nullptr, nullptr, nullptr, nullptr);

        for (DWORD i = 0; i < cValues; i++) {
            WCHAR szExt[256] = {};
            DWORD cchExt = ARRAYSIZE(szExt);

            if (RegEnumValueW(hFileAssoc, i, szExt, &cchExt, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                // Skip GUID-style extensions like .c5e2524a-ea46-4f67-841f-6a9465d9d515
                // Real extensions are short (e.g. .txt, .html, .mp3)
                if (cchExt <= 20 && szExt[0] == L'.') {
                    hasRealFileAssocs = true;
                    break;
                }
            }
        }
        RegCloseKey(hFileAssoc);
    }

    if (hasRealFileAssocs) {
        RegCloseKey(hCap);
        return false;  // has real file associations, keep it
    }

    // No file associations - check URL associations
    HKEY hUrlAssoc = nullptr;
    bool hasNonMsProtocol = false;
    bool hasAnyProtocol = false;

    if (RegOpenKeyExW(hCap, L"URLAssociations", 0, KEY_READ, &hUrlAssoc) == ERROR_SUCCESS) {
        DWORD cValues = 0;
        RegQueryInfoKeyW(hUrlAssoc, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &cValues, nullptr, nullptr, nullptr, nullptr);

        for (DWORD i = 0; i < cValues; i++) {
            WCHAR szProtocol[256] = {};
            DWORD cchProtocol = ARRAYSIZE(szProtocol);

            if (RegEnumValueW(hUrlAssoc, i, szProtocol, &cchProtocol, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                hasAnyProtocol = true;

                // Check if this protocol does NOT start with "ms-" (case-insensitive)
                if (_wcsnicmp(szProtocol, L"ms-", 3) != 0 &&
                    _wcsnicmp(szProtocol, L"microsoft", 9) != 0 &&
                    _wcsnicmp(szProtocol, L"xbox", 4) != 0 &&
                    _wcsnicmp(szProtocol, L"windowsdefender", 15) != 0 &&
                    _wcsnicmp(szProtocol, L"zune", 4) != 0 &&
                    _wcsnicmp(szProtocol, L"microsoftvideo", 14) != 0 &&
                    _wcsnicmp(szProtocol, L"xboxidp", 7) != 0) {
                    hasNonMsProtocol = true;
                    break;
                }
            }
        }
        RegCloseKey(hUrlAssoc);
    }

    // Check StartMenu subkey - if it exists with values, the app most likely has real UI
    HKEY hStartMenu = nullptr;
    bool hasStartMenu = false;
    if (RegOpenKeyExW(hCap, L"StartMenu", 0, KEY_READ, &hStartMenu) == ERROR_SUCCESS) {
        DWORD cValues = 0;
        RegQueryInfoKeyW(hStartMenu, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &cValues, nullptr, nullptr, nullptr, nullptr);
        if (cValues > 0)
            hasStartMenu = true;
        RegCloseKey(hStartMenu);
    }

    RegCloseKey(hCap);

    // Filter if: no file associations, no start menu entries, and either no protocols at all or only internal MS/Xbox/system protocols
    if (!hasStartMenu && (!hasAnyProtocol || !hasNonMsProtocol)) {
        return true; // filter it out
    }

    return false;
}

static bool IsInBlacklist(LPCWSTR pszAppName, LPCWSTR pszPath, LPCWSTR pszDisplayName = nullptr)
{
    SharedLock lock(g_listLock);
    for (const auto& entry : g_additionalBlacklist) {
        if (pszAppName && _wcsicmp(pszAppName, entry.c_str()) == 0)
            return true;
        if (pszDisplayName && _wcsicmp(pszDisplayName, entry.c_str()) == 0)
            return true;
        if (pszPath) {
            std::wstring pathLower(pszPath);
            std::wstring entryLower(entry);
            for (auto& c : pathLower) c = towlower(c);
            for (auto& c : entryLower) c = towlower(c);
            if (pathLower.find(entryLower) != std::wstring::npos)
                return true;
        }
    }
    return false;
}

long __cdecl BuildAppConfig_Hook(const WCHAR* param_1, const WCHAR* param_2, void** param_3)
{
    // Preserve outer context on nested calls and every C++ exit path.
    struct ContextGuard {
        bool previous;
        ContextGuard() : previous(g_inSudContext) { g_inSudContext = true; }
        ~ContextGuard() { g_inSudContext = previous; }
    } context;

    if (g_enableLogging)
        Wh_Log(L"BuildAppConfig: app=\"%s\", path=\"%s\"",
               param_1 ? param_1 : L"(null)",
               param_2 ? param_2 : L"(null)");

    bool filtered = false;

    if (g_hideSystemApps) {
        // Filter by capabilities analysis (only MS-protocol apps)
        if (param_2 && ShouldFilterApp(param_2)) {
            filtered = true;
        }

        // Filter by built-in path patterns for known system apps
        if (!filtered && param_2) {
            static const LPCWSTR s_filteredPathSubstrings[] = {
                L"CurrentVersion\\Explorer\\Capabilities", // File Explorer
                L"SecHealthUI",                            // Windows Security
                L"DesktopAppInstaller",                    // App Installer & WinGet
            };
            for (size_t i = 0; i < ARRAYSIZE(s_filteredPathSubstrings); i++) {
                if (StrStrIW(param_2, s_filteredPathSubstrings[i]) != nullptr) {
                    filtered = true;
                    break;
                }
            }
        }

        // Filter by built-in name patterns (for HKLM desktop apps with real names)
        if (!filtered && param_1) {
            static const LPCWSTR s_filteredNames[] = {
                L"File Explorer",
            };
            for (size_t i = 0; i < ARRAYSIZE(s_filteredNames); i++) {
                if (_wcsicmp(param_1, s_filteredNames[i]) == 0) {
                    filtered = true;
                    break;
                }
            }
        }
    }

    // Pre-build blacklist check (registry name and path)
    if (!filtered) {
        if (IsInBlacklist(param_1, param_2)) {
            filtered = true;
        }
    }

    // Hide Store apps (8wekyb3d8bbwe publisher), not system components (cw5n1h2txyewy)
    if (!filtered && g_hideStoreApps && param_2) {
        if (StrStrIW(param_2, L"_8wekyb3d8bbwe\\") != nullptr) {
            filtered = true;
        }
    }

    if (filtered) {
        if (g_enableLogging)
            Wh_Log(L"^ FILTERED ^");
        *param_3 = nullptr;
        return E_FAIL;
    }

    long result = BuildAppConfig_Original(param_1, param_2, param_3);
    if (g_enableLogging)
        Wh_Log(L"[DP-APP] BuildAppConfig result app=\"%s\" hr=0x%08lX object=%d",
               param_1 ? param_1 : L"(null)", (unsigned long)result,
               param_3 && *param_3 ? 1 : 0);

    if (SUCCEEDED(result) && param_3 && *param_3) {
        char* obj = (char*)*param_3;
        LPCWSTR pszDisplayName = *(LPCWSTR*)(obj + 0x10);

        if (g_enableLogging && pszDisplayName)
            Wh_Log(L"  Display name: \"%s\"", pszDisplayName);

        // Post-build blacklist check using display name
        if (pszDisplayName && IsInBlacklist(nullptr, nullptr, pszDisplayName)) {
            if (g_enableLogging)
                Wh_Log(L"^ FILTERED (by display name) ^");

            // Destroy the object via its destructor (vtable slot 0)
            typedef void(__cdecl* DestroyFunc)(void*, int);
            void** vtable = *(void***)obj;
            if (vtable && vtable[0]) {
                ((DestroyFunc)vtable[0])(*param_3, 1);
            }
            *param_3 = nullptr;
            return E_FAIL;
        }

        // Apply icon overrides (match registry name or display name)
        IconOverride ov;
        if (FindIconOverride(param_1, pszDisplayName, ov)) {
            ApplyIconOverride(*param_3, &ov);
        }
    }

    return result;
}

// ============================================================================
// CAssocArray::QueryInterface - RS2 -> VB IID translation
// ============================================================================

static constexpr GUID IID_IAssociationArrayRS2 =
    {0xC35F777C, 0x5FA8, 0x4293, {0xBD, 0x69, 0x46, 0x36, 0xFF, 0x36, 0x6F, 0x22}};
static constexpr GUID IID_IAssociationArrayVB =
    {0xD7D31033, 0xCCB5, 0x40E3, {0x8E, 0xFA, 0xA6, 0x68, 0xF2, 0x31, 0x00, 0x3F}};

HRESULT(STDMETHODCALLTYPE* CAssocArray_QueryInterface_orig)(void*, REFIID, LPVOID*);
HRESULT STDMETHODCALLTYPE CAssocArray_QueryInterface_hook(void* pThis, REFIID riid, LPVOID* ppv)
{
    if (g_inSudContext && IsEqualIID(riid, IID_IAssociationArrayRS2)) {
        HRESULT hr = CAssocArray_QueryInterface_orig(pThis, IID_IAssociationArrayVB, ppv);
        if (g_enableLogging) Wh_Log(L"[DP-APP] association array translated QueryInterface hr=0x%08lX", (unsigned long)hr);
        return hr;
    }
    return CAssocArray_QueryInterface_orig(pThis, riid, ppv);
}

// ============================================================================
// Symbol hook tables
// ============================================================================

// Windhawk's default symbol enumeration uses unannotated names. Decorated
// alternatives are retained; symbol resolution is still required for these hooks.

// One table per module, one HookSymbols call per module: Windhawk keeps a single resolved-symbol set per module,
// so every extra call with a different list would load the (large) symbols of that module again.
// Every entry is optional: the class that holds the association registration lives in shell32.dll up to Windows 10
// and in windows.storage.dll later, so what is not found in one module is looked for in the other.

// windows.storage.dll
const WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
    {
        {
            L"long __cdecl _SetAssociation(unsigned short const *,unsigned short const *,enum ASSOCIATIONTYPE,unsigned short const *)",
            L"?_SetAssociation@@YAJPEBG0W4ASSOCIATIONTYPE@@0@Z"
        },
        (void**)&SetAssociation_Original,
        (void*)SetAssociation_Hook,
        true
    },
    {
        {
            L"public: virtual long __cdecl CApplicationAssociationRegistration::[IApplicationAssociationRegistration]::QueryAppIsDefault(unsigned short const *,enum ASSOCIATIONTYPE,enum ASSOCIATIONLEVEL,unsigned short const *,int *)",
            L"?QueryAppIsDefault@?QIApplicationAssociationRegistration@@CApplicationAssociationRegistration@@UEAAJPEBGW4ASSOCIATIONTYPE@@W4ASSOCIATIONLEVEL@@0PEAH@Z"
        },
        (void**)&QueryAppIsDefault_Public_Original,
        (void*)QueryAppIsDefault_Public_Hook,
        true
    },
    {
        {
            L"public: virtual long __cdecl CApplicationAssociationRegistration::[IApplicationAssociationRegistrationInternal]::QueryAppIsDefaultAll(enum ASSOCIATIONLEVEL,unsigned short const *,int *)",
            L"?QueryAppIsDefaultAll@?QIApplicationAssociationRegistrationInternal@@CApplicationAssociationRegistration@@UEAAJW4ASSOCIATIONLEVEL@@PEBGPEAH@Z"
        },
        (void**)&QueryAppIsDefaultAll_Original,
        (void*)QueryAppIsDefaultAll_Hook,
        true
    },
    {
        {
            L"private: long __cdecl CApplicationAssociationRegistration::_QueryAppIsDefault(unsigned short const *,enum ASSOCIATIONTYPE,enum ASSOCIATIONLEVEL,unsigned short const *,int *)",
            L"?_QueryAppIsDefault@CApplicationAssociationRegistration@@AEAAJPEBGW4ASSOCIATIONTYPE@@W4ASSOCIATIONLEVEL@@0PEAH@Z"
        },
        (void**)&p_QueryAppIsDefault_Private,
        nullptr,
        true
    },
    {
        {
            L"long __cdecl GenerateUserAssocChangeNotification(unsigned short const *)",
            L"?GenerateUserAssocChangeNotification@@YAJPEBG@Z"
        },
        (void**)&p_GenerateUserAssocChangeNotification,
        nullptr,
        true
    },
    {
        {
            L"public: virtual long __cdecl CAssocArray::QueryInterface"
            L"(struct _GUID const &,void * *)"
        },
        (void**)&CAssocArray_QueryInterface_orig,
        (void*)CAssocArray_QueryInterface_hook,
        true
    },
};

// shell32.dll (used only for what windows.storage.dll lacked)
const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"long __cdecl _SetAssociation(unsigned short const *,unsigned short const *,enum ASSOCIATIONTYPE,unsigned short const *)",
            L"?_SetAssociation@@YAJPEBG0W4ASSOCIATIONTYPE@@0@Z"
        },
        (void**)&SetAssociation_Original,
        (void*)SetAssociation_Hook,
        true
    },
    {
        {
            L"public: virtual long __cdecl CApplicationAssociationRegistration::[IApplicationAssociationRegistration]::QueryAppIsDefault(unsigned short const *,enum ASSOCIATIONTYPE,enum ASSOCIATIONLEVEL,unsigned short const *,int *)",
            L"?QueryAppIsDefault@?QIApplicationAssociationRegistration@@CApplicationAssociationRegistration@@UEAAJPEBGW4ASSOCIATIONTYPE@@W4ASSOCIATIONLEVEL@@0PEAH@Z"
        },
        (void**)&QueryAppIsDefault_Public_Original,
        (void*)QueryAppIsDefault_Public_Hook,
        true
    },
    {
        {
            L"public: virtual long __cdecl CApplicationAssociationRegistration::[IApplicationAssociationRegistrationInternal]::QueryAppIsDefaultAll(enum ASSOCIATIONLEVEL,unsigned short const *,int *)",
            L"?QueryAppIsDefaultAll@?QIApplicationAssociationRegistrationInternal@@CApplicationAssociationRegistration@@UEAAJW4ASSOCIATIONLEVEL@@PEBGPEAH@Z"
        },
        (void**)&QueryAppIsDefaultAll_Original,
        (void*)QueryAppIsDefaultAll_Hook,
        true
    },
    {
        {
            L"private: long __cdecl CApplicationAssociationRegistration::_QueryAppIsDefault(unsigned short const *,enum ASSOCIATIONTYPE,enum ASSOCIATIONLEVEL,unsigned short const *,int *)",
            L"?_QueryAppIsDefault@CApplicationAssociationRegistration@@AEAAJPEBGW4ASSOCIATIONTYPE@@W4ASSOCIATIONLEVEL@@0PEAH@Z"
        },
        (void**)&p_QueryAppIsDefault_Private,
        nullptr,
        true
    },
    {
        {
            L"long __cdecl GenerateUserAssocChangeNotification(unsigned short const *)",
            L"?GenerateUserAssocChangeNotification@@YAJPEBG@Z"
        },
        (void**)&p_GenerateUserAssocChangeNotification,
        nullptr,
        true
    },
};

// ============================================================================
// Client-side hook: FillListViewWithMapiHandler in sud.dll
// ============================================================================

using FillListViewWithMapiHandler_t = long(__thiscall*)(void*, void*, void*);
static FillListViewWithMapiHandler_t FillListViewWithMapiHandler_Original = nullptr;

long __thiscall FillListViewWithMapiHandler_Hook(void* pThis, void* pAAR, void* pDPA)
{
    Wh_Log(L"FillListViewWithMapiHandler: skipping buggy MAPI handler to prevent crash");
    return 0; // S_OK
}

// Scope association-array IID adaptation to protocol enumeration by the page.
using AddProtocolsForApp_t = long(__cdecl*)(const WCHAR*, void*);
static AddProtocolsForApp_t AddProtocolsForApp_Original = nullptr;
long __cdecl AddProtocolsForApp_Hook(const WCHAR* name, void* associations)
{
    bool previous = g_inSudContext;
    g_inSudContext = true;
    long result = AddProtocolsForApp_Original(name, associations);
    g_inSudContext = previous;
    return result;
}

// The target is the private, language-named copy of the Microsoft 1607 page.
const WindhawkUtils::SYMBOL_HOOK sudDllHooks[] = {
    {
        {
            L"long __cdecl _AddProtocolsForApp(unsigned short const *,class CDPACoTaskMem<unsigned short> &)",
            L"?_AddProtocolsForApp@@YAJPEBGAEAV?$CDPACoTaskMem@G@@@Z"
        },
        (void**)&AddProtocolsForApp_Original,
        (void*)AddProtocolsForApp_Hook,
        true
    },
    {
        {
            L"unsigned long __cdecl _AsyncRestoreAppDefaults(void *)",
            L"?_AsyncRestoreAppDefaults@@YAKPEAX@Z"
        },
        (void**)&AsyncRestoreAppDefaults_Original,
        (void*)AsyncRestoreAppDefaults_Hook,
        false
    },
    {
        {
            L"unsigned long __cdecl _AsyncSetAppAssoc(void *)",
            L"?_AsyncSetAppAssoc@@YAKPEAX@Z"
        },
        (void**)&AsyncSetAppAssoc_Original,
        (void*)AsyncSetAppAssoc_Hook,
        false
    },
    {
        {
            L"long __cdecl BuildAppConfig(unsigned short const *,unsigned short const *,class CAppConfig * *)",
            L"?BuildAppConfig@@YAJPEBG0PEAPEAVCAppConfig@@@Z"
        },
        (void**)&BuildAppConfig_Original,
        (void*)BuildAppConfig_Hook,
        false
    },
    {
        {
            L"private: long __cdecl CAdvancedSettingsPage::FillListViewWithMapiHandler(struct IApplicationAssociationRegistrationInternal *,class CDPACoTaskMem<unsigned short> const &)",
            L"?FillListViewWithMapiHandler@CAdvancedSettingsPage@@AEAAJPEAUIApplicationAssociationRegistrationInternal@@AEBV?$CDPACoTaskMem@G@@@Z"
        },
        (void**)&FillListViewWithMapiHandler_Original,
        (void*)FillListViewWithMapiHandler_Hook,
        false
    },
};


static void LoadBlacklist()
{
    ExclusiveLock lock(g_listLock);
    LoadBlacklistImpl();
}

static void LoadIconOverrides()
{
    ExclusiveLock lock(g_listLock);
    LoadIconOverridesImpl();
}

void LoadSettings()
{
    g_enableLogging = Wh_GetIntSetting(L"enableLogging") != 0;
    g_hideSystemApps = Wh_GetIntSetting(L"hideSystemApps") != 0;
    g_hideStoreApps = Wh_GetIntSetting(L"hideStoreApps") != 0;
    LoadBlacklist();
    LoadIconOverrides();
}

// The hooks of shell32.dll and windows.storage.dll (symbols: Windhawk downloads them the first time): one
// HookSymbols call per module. windows.storage.dll first; shell32.dll only if something was not found there.
void InstallSystemHooks()
{
    HMODULE hStorage = LoadLibraryW(L"windows.storage.dll");
    if (hStorage &&
        WindhawkUtils::HookSymbols(hStorage, windowsStorageHooks, ARRAYSIZE(windowsStorageHooks)))
        Wh_Log(L"Resolved the symbols of windows.storage.dll");

    if (!SetAssociation_Original || !p_QueryAppIsDefault_Private) {
        HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
        if (hShell32 &&
            WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks)))
            Wh_Log(L"Resolved the symbols of shell32.dll");
    }

    if (SetAssociation_Original) Wh_Log(L"Hooked _SetAssociation");
    else Wh_Log(L"WARNING: Could not hook _SetAssociation");
    if (QueryAppIsDefault_Public_Original) Wh_Log(L"Hooked QueryAppIsDefault (public)");
    else Wh_Log(L"WARNING: Could not hook QueryAppIsDefault (public)");
    if (QueryAppIsDefaultAll_Original) Wh_Log(L"Hooked _QueryAppIsDefaultAll");
    else Wh_Log(L"WARNING: Could not hook _QueryAppIsDefaultAll");
    if (p_QueryAppIsDefault_Private) Wh_Log(L"Resolved _QueryAppIsDefault");
    else Wh_Log(L"WARNING: Could not resolve _QueryAppIsDefault");
    if (CAssocArray_QueryInterface_orig) Wh_Log(L"Hooked CAssocArray::QueryInterface");
    else Wh_Log(L"WARNING: Could not hook CAssocArray::QueryInterface");
    if (!p_GenerateUserAssocChangeNotification) Wh_Log(L"Using SHChangeNotify fallback");

    if (!SetAssociation_Original || !QueryAppIsDefault_Public_Original || !QueryAppIsDefaultAll_Original ||
        !p_QueryAppIsDefault_Private) {
        Wh_Log(L"Some compatibility symbols or hooks are missing; check the preceding warnings");
    }
}

// The hooks inside the private copy of sud.dll.
void HookPrivateSud(HMODULE page)
{
    if (WindhawkUtils::HookSymbols(page, sudDllHooks, ARRAYSIZE(sudDllHooks))) {
        Wh_ApplyHookOperations();
        Wh_Log(L"Hooked the page (_AsyncRestoreAppDefaults, _AsyncSetAppAssoc, BuildAppConfig, FillListViewWithMapiHandler)");
    } else {
        Wh_Log(L"WARNING: Could not hook the page: saving associations and the programs list may not work");
    }
}

}  // namespace compat


// A downloaded, cached or hand-placed file is accepted only if it is exactly the pinned Microsoft file: pinned SHA-256
// and size, and a PE header that agrees with the key (time stamp, image size, x64). The PE fields alone can be forged,
// so the hash is what decides; a spec without a pinned hash accepts nothing.
bool Matches(const FileSpec& f, const Bytes& b) {
    if (!f.sha256 || !*f.sha256) return false;
    if (b.size() > f.maxSize || (f.fileSize && b.size() != f.fileSize)) return false;
    gen::Pe pe;
    if (!pe.parse(b)) return false;
    if (gen::R16(b, pe.fh) != 0x8664 || gen::R32(b, pe.fh + 4) != f.timestamp ||
        gen::R32(b, pe.opt + 56) != f.imageSize) return false;
    return Sha256Hex(b) == f.sha256;
}

// The verified bytes of one of the Microsoft files: from <storage>\dl if present and intact, else downloaded.
bool GetVerifiedFile(const std::wstring& dlDir, const FileSpec& f, Bytes& data) {
    try {
        const std::wstring path = dlDir + L"\\" + f.name;
        if (ReadWholeFile(path, data) && Matches(f, data)) return true;
        if (f.altName && ReadWholeFile(dlDir + L"\\" + f.altName, data) && Matches(f, data)) return true;
        const std::wstring url = std::wstring(kSymbolPath) + f.key;
        for (int attempt = 1; attempt <= 3 && !g_stopping.load(); attempt++) {
            Wh_Log(L"Downloading %s from %s (attempt %d)", f.name, kSymbolHost, attempt);
            if (Download(url, f.maxSize, data)) {
                if (Matches(f, data)) {
                    Wh_Log(L"%s downloaded and verified (SHA-256 %S)", f.name, Sha256Hex(data).c_str());
                    if (!WriteFileAtomic(path, data)) Wh_Log(L"Could not save %s (continuing in memory)", f.name);
                    return true;
                }
                Wh_Log(L"%s: the downloaded file is not the pinned Microsoft file (size, SHA-256 or PE header differs); discarded", f.name);
            }
            if (attempt < 3 && WaitForSingleObject(g_stopEvent, 2000 * attempt) == WAIT_OBJECT_0) break;
        }
        Wh_Log(L"%s is not available at https://%s%s", f.name, kSymbolHost, url.c_str());
    } catch (...) {
        Wh_Log(L"GetVerifiedFile: unexpected exception for %s", f.name);
    }
    data.clear();
    return false;
}

// ---- language -------------------------------------------------------------------------------------------------------

Language CurrentLanguage() {
    const int setting = g_languageSetting.load();
    if (setting == 1) return Language::English;
    if (setting == 2) return Language::Italian;
    if (setting == 3) return Language::German;
    if (setting == 4) return Language::Spanish;
    if (setting == 5) return Language::French;
    if (setting == 6) return Language::PortugueseBrazil;
    const LANGID uiLanguage = GetUserDefaultUILanguage();
    const WORD primary = PRIMARYLANGID(uiLanguage);
    if (primary == LANG_ITALIAN) return Language::Italian;
    if (primary == LANG_GERMAN) return Language::German;
    if (primary == LANG_SPANISH) return Language::Spanish;
    if (primary == LANG_FRENCH) return Language::French;
    if (primary == LANG_PORTUGUESE && SUBLANGID(uiLanguage) == SUBLANG_PORTUGUESE_BRAZILIAN)
        return Language::PortugueseBrazil;
    return Language::English;
}

void LoadSettings() {
    int language = 0;
    {
        StringSetting s(Wh_GetStringSetting(L"language"));
        if (s.value) {
            if (!wcscmp(s.value, L"en-US")) language = 1;
            else if (!wcscmp(s.value, L"it-IT")) language = 2;
            else if (!wcscmp(s.value, L"de-DE")) language = 3;
            else if (!wcscmp(s.value, L"es-ES")) language = 4;
            else if (!wcscmp(s.value, L"fr-FR")) language = 5;
            else if (!wcscmp(s.value, L"pt-BR")) language = 6;
        }
    }
    g_languageSetting.store(language);
    g_keepFiles.store(Wh_GetIntSetting(L"keepFilesOnDisable") != 0);
}

// ---- loading --------------------------------------------------------------------------------------------------------

// Loads `path` only if its content has the expected SHA-256. The file is held open without write or delete sharing
// from before the check until after the load, so what is checked is what is loaded.
HMODULE LoadVerified(const std::wstring& path, const std::string& sha256, const wchar_t* stage, DWORD* failure = nullptr) {
    if (failure) *failure = ERROR_SUCCESS;
    try {
        FileHandle f(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!f.valid()) { DWORD e = GetLastError(); if (failure) *failure = e; Wh_Log(L"[DP-LOAD] %s open failed error=%lu", stage,e); SetLastError(e); return nullptr; }
        Sha256 hash;
        for (;;) {
            uint8_t buf[65536]; DWORD got = 0;
            if (!ReadFile(f.get(), buf, sizeof(buf), &got, nullptr)) {
                DWORD e = GetLastError(); if (failure) *failure = e; Wh_Log(L"[DP-LOAD] %s read failed error=%lu",stage,e); SetLastError(e); return nullptr;
            }
            if (!got) break;
            hash.update(buf, got);
        }
        if (hash.hex() != sha256) {
            Wh_Log(L"[DP-LOAD] %s content mismatch; not loaded", stage);
            if (failure) *failure = ERROR_CRC; SetLastError(ERROR_CRC); return nullptr;
        }
        SetLastError(ERROR_SUCCESS);
        HMODULE result = LoadLibraryExW(path.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        DWORD loadError = result ? ERROR_SUCCESS : GetLastError();
        if (failure) *failure = loadError;
        if (!result) Wh_Log(L"[DP-LOAD] %s LoadLibraryEx failed error=%lu", stage,loadError);
        else Wh_Log(L"[DP-LOAD] %s loaded", stage);
        SetLastError(loadError);
        return result;
    } catch (...) {
        Wh_Log(L"[DP-LOAD] %s unexpected C++ exception", stage);
        if (failure) *failure = ERROR_UNHANDLED_EXCEPTION;
        SetLastError(ERROR_UNHANDLED_EXCEPTION);
        return nullptr;
    }
}

// ---- setup (worker thread) ------------------------------------------------------------------------------------------

std::wstring StorageDir() {
    try {
        wchar_t buf[MAX_PATH];
        const size_t n = Wh_GetModStoragePath(buf, ARRAYSIZE(buf));
        std::wstring s = (n && n < ARRAYSIZE(buf)) ? std::wstring(buf, n) : std::wstring();
        while (!s.empty() && s.back() == L'\\') s.pop_back();
        return s;
    } catch (...) {
        return std::wstring();
    }
}

// 19041-19045 fix derived by comparing the 1607 image with the WinClassic
// fix. The same patch is an unverified Windows 11 test on the listed builds.
// Patch only the exact Microsoft image;
// offsets are file offsets, not RVAs. All-or-nothing, on a private copy.
DWORD WindowsBuild();
bool ApplyWinClassicCompat(Bytes& image) {
    if (Sha256Hex(image) != "c47fce44d5cab0d7c09790c252fd6f12f4c56bafc72e035a150d3955b3c60740") {
        Wh_Log(L"[DP-COMPAT] unknown sud.dll SHA-256; compatibility patch skipped");
        return false;
    }
    static const uint8_t oldInternal[] = {0x10,0x9d,0x5c,0x1c,0x25,0x12,0x97,0x4c,0x8c,0x51,0x98,0xe1,0xb6,0xf0,0xd4,0xe0};
    static const uint8_t newInternal[] = {0x4e,0x13,0x57,0xa3,0x1e,0x8c,0xdd,0x4e,0x84,0x74,0x40,0xa8,0x05,0x46,0xf5,0x4f};
    static const uint8_t oldArray[] = {0x7c,0x77,0x5f,0xc3,0xa8,0x5f,0x93,0x42,0xbd,0x69,0x46,0x36,0xff,0x36,0x6f,0x22};
    static const uint8_t newArray[] = {0x33,0x10,0xd3,0xd7,0xb5,0xcc,0xe3,0x40,0x8e,0xfa,0xa6,0x68,0xf2,0x31,0x00,0x3f};
    if (image.size() <= 0x1bb8f || image[0x54d8] != 0x78 || image[0x556b] != 0x38 || image[0x953b] != 0x38 ||
        memcmp(image.data()+0x1b9c8, oldInternal, 16) || memcmp(image.data()+0x1bb80, oldArray, 16)) {
        Wh_Log(L"[DP-COMPAT] expected bytes do not match; patch skipped"); return false;
    }
    image[0x54d8] = 0x70; image[0x556b] = 0x30; image[0x953b] = 0x30;
    memcpy(image.data()+0x1b9c8, newInternal, 16);
    memcpy(image.data()+0x1bb80, newArray, 16);
    return true;
}

void RunSetup() {
    {
        SharedLock lock(g_stateLock);
        if (g_pageModule) {
            Wh_Log(L"The page is already in use: a changed language applies after explorer.exe restarts");
            return;
        }
    }
    const std::wstring storage = StorageDir();
    if (storage.empty()) { Wh_Log(L"Setup: no storage folder"); return; }
    const std::wstring dlDir = storage + L"\\dl", binDir = storage + L"\\bin\\" + kGeneratorRevision;
    EnsureDirs(dlDir);
    EnsureDirs(binDir);

    // One Explorer at a time (there can be several explorer.exe processes writing the same files).
    NamedMutex mutex(CreateMutexW(nullptr, FALSE, L"Local\\Win7DefaultProgramsRestorer.Setup"));
    if (!mutex.h) { Wh_Log(L"[DP-DIAG] CreateMutex failed error=%lu; setup cancelled", GetLastError()); return; }
    if (mutex.h) {
        HANDLE handles[2] = {g_stopEvent, mutex.h};
        const DWORD w = WaitForMultipleObjects(2, handles, FALSE, 300000);
        if (w != WAIT_OBJECT_0 + 1 && w != WAIT_ABANDONED_0 + 1) {
            Wh_Log(L"[DP-DIAG] setup mutex wait result=%lu error=%lu", w, w == WAIT_FAILED ? GetLastError() : 0);
            return;
        }
        mutex.owned = true;
    }

    Bytes files[ARRAYSIZE(kFiles)];
    bool missing = false;
    for (size_t i = 0; i < ARRAYSIZE(kFiles) && !g_stopping.load(); i++)
        if (!GetVerifiedFile(dlDir, kFiles[i], files[i])) missing = true;
    if (missing || g_stopping.load()) {
        if (!g_stopping.load())
            Wh_Log(L"Setup stopped: a file is not available. The Windows page stays in place. Check the internet "
                   L"connection, or put the files (sud.dll, dui70.dll) in %s, and restart explorer.exe.", dlDir.c_str());
        return;
    }

    // The page and its existing fixes remain usable
    // even if this independent download fails. No system file is replaced.
    std::wstring spadPath;
    const FileSpec spadSpec = {L"ComputerDefaults-1607.exe",
        L"ComputerDefaults.exe/57899B3C10000/ComputerDefaults.exe",
        1u << 20, 0x57899B3Cu, 0x10000u, nullptr, kSpadSha256, kSpadFileSize};
    Bytes spad;
    if (GetVerifiedFile(dlDir, spadSpec, spad)) {
        const std::wstring candidate = binDir + L"\\ComputerDefaults-1607.exe";
        if (EnsureFile(candidate, spad)) {
            spadPath = candidate;
            Wh_Log(L"[DP-SPAD] verified 1607 executable ready: %s", candidate.c_str());
        }
    } else Wh_Log(L"[DP-SPAD] 1607 executable unavailable or SHA-256 mismatch; using original link");

    const StringTable table = SudTable(CurrentLanguage());
    const std::wstring duiPath = binDir + L"\\" + kPrivateDuiName;
    const std::wstring pagePath = binDir + L"\\sud7." + table.tag + L".dll";
    if (pagePath.size() >= MAX_PATH - 1) { Wh_Log(L"Setup: the storage path is too long"); return; }
    Bytes page, dui;
    std::string error;
    Bytes pageInput = files[kFileSud];
    const DWORD compatBuild = WindowsBuild();
    // Windows 10 remains restricted to 19041-19045. For Windows 11 and
    // later builds, apply the same private-image fix, including future builds.
    // A matching input hash protects the page image, not the system ABI:
    // future Windows builds may change undocumented interfaces or vtables.
    const bool win10Compat = compatBuild >= 19041 && compatBuild <= 19045;
    const bool win11Compat = compatBuild >= 22000;
    if (win10Compat || win11Compat) {
        if (!ApplyWinClassicCompat(pageInput)) {
            Wh_Log(L"[DP-COMPAT] exact 1607 input required; setup stopped safely");
            return;
        }
        Wh_Log(L"[DP-COMPAT] private-page interface and vtable fixes applied build=%lu win11=%d",
               compatBuild, win11Compat);
        if (compatBuild > 26200)
            Wh_Log(L"[DP-COMPAT] Windows build=%lu has not been validated; private ABI may have changed", compatBuild);
    }
    if (!gen::Generate(pageInput, table, true, pagePath, page, error) ||
        !gen::Generate(files[kFileDui], DuiTable(), false, std::wstring(), dui, error)) {
        Wh_Log(L"Setup: could not build the files: %S", error.c_str());
        return;
    }
    if (!EnsureFile(duiPath, dui) || !EnsureFile(pagePath, page)) {
        Wh_Log(L"Setup: could not write the files in %s", binDir.c_str());
        return;
    }
    if (g_stopping.load()) return;

    // The DirectUI first: the page finds it by name when it is loaded.
    DWORD duiError = ERROR_SUCCESS;
    HMODULE duiModule = LoadVerified(duiPath, Sha256Hex(dui), L"private DirectUI (dpdui.dll)", &duiError);
    if (!duiModule && duiError == ERROR_BAD_EXE_FORMAT) {
        // Some Windows loaders reject the rebuilt resource image. Try the
        // verified original bytes without altering their PE structure. The
        // separate directory avoids replacing a DLL in use by another process.
        const std::wstring fallbackDir = binDir + L"\\original-directui";
        const std::wstring fallbackPath = fallbackDir + L"\\" + kPrivateDuiName;
        EnsureDirs(fallbackDir);
        Wh_Log(L"[DP-LOAD] generated DirectUI rejected; trying unmodified DirectUI in a separate directory");
        if (EnsureFile(fallbackPath, files[kFileDui]))
            duiModule = LoadVerified(fallbackPath, Sha256Hex(files[kFileDui]), L"unmodified DirectUI fallback");
        else Wh_Log(L"[DP-LOAD] could not write unmodified DirectUI fallback");
        if (duiModule) Wh_Log(L"[DP-LOAD] fallback active: embedded DirectUI localization is unavailable");
    }
    if (!duiModule) {
        Wh_Log(L"[DP-LOAD] setup stopped before loading sud7; Windows page stays in place");
        return;
    }
    HMODULE pageModule = LoadVerified(pagePath, Sha256Hex(page), L"private page (sud7)");
    if (!pageModule) {
        Wh_Log(L"[DP-LOAD] private page was not loaded; Windows page stays in place");
        // Keep the already loaded DirectUI until Explorer exits, as in the original code.
        return;
    }
    auto getClassObject = reinterpret_cast<DllGetClassObject_t>(
        reinterpret_cast<void*>(GetProcAddress(pageModule, "DllGetClassObject")));
    if (!getClassObject) { Wh_Log(L"Setup: the page has no DllGetClassObject"); return; }
    {
        ExclusiveLock lock(g_stateLock);
        g_storage = storage;
        g_spadPath = spadPath;
        g_pagePath = pagePath;
        g_duiModule = duiModule;
        g_pageModule = pageModule;
    }
    g_getClassObject.store(getClassObject);
    // The fixes inside the private copy (symbols: Windhawk downloads them the first time).
    compat::HookPrivateSud(pageModule);
    g_pageReady.store(true);
    Wh_Log(L"Ready: %s", pagePath.c_str());
}

DWORD WINAPI SetupThread(LPVOID) {
    // An exception must never leave a thread procedure: the process would be terminated.
    try {
        RunSetup();
    } catch (...) {
        Wh_Log(L"Setup stopped by an unexpected exception. The Windows page stays in place.");
    }
    return 0;
}

void StartSetup() {
    if (g_setupThread) {
        if (WaitForSingleObject(g_setupThread, 0) != WAIT_OBJECT_0) return;      // still running
        CloseHandle(g_setupThread);
    }
    g_setupThread = CreateThread(nullptr, 0, SetupThread, nullptr, 0, nullptr);
    if (!g_setupThread) Wh_Log(L"[DP-DIAG] setup thread creation failed error=%lu", GetLastError());
    else Wh_Log(L"[DP-DIAG] setup worker started pid=%lu", GetCurrentProcessId());
}

// =====================================================================================================================
// Redirection (explorer.exe)
// =====================================================================================================================

using PSPropertyBag_ReadStr_t = HRESULT(WINAPI*)(IPropertyBag*, LPCWSTR, LPWSTR, int);
PSPropertyBag_ReadStr_t PSPropertyBag_ReadStr_Orig;
std::atomic<bool> g_diagEnabled{false};
std::atomic<unsigned> g_diagResourceReads{0};

// True if `value` (as read from the registry, possibly with environment variables) names System32\sud.dll.
bool IsSystemSud(LPCWSTR value) {
    wchar_t expanded[MAX_PATH], system[MAX_PATH];
    const DWORD n = ExpandEnvironmentStringsW(value, expanded, ARRAYSIZE(expanded));
    if (!n || n > ARRAYSIZE(expanded)) return false;
    const UINT s = GetSystemDirectoryW(system, ARRAYSIZE(system));
    if (!s || s + 9 > ARRAYSIZE(system)) return false;
    wcscat(system, L"\\sud.dll");
    return _wcsicmp(expanded, system) == 0;
}

// Control Panel builds a page from the "ResourceDLL" and "ResourceID" of its registration (CLayoutFolder::Load in
// shell32). For the Default Programs page the DLL becomes the private copy; everything else is left as read.
HRESULT WINAPI PSPropertyBag_ReadStr_Hook(IPropertyBag* bag, LPCWSTR name, LPWSTR value, int count) {
    const HRESULT hr = PSPropertyBag_ReadStr_Orig(bag, name, value, count);
    if (name && _wcsicmp(name, L"ResourceDLL") == 0 && g_diagEnabled.load()) {
        unsigned number = g_diagResourceReads.fetch_add(1);
        if (number < 30) {
            bool match = SUCCEEDED(hr) && value && count > 0 && IsSystemSud(value);
            Wh_Log(L"[DP-DIAG] ResourceDLL read #%u hr=0x%08lX system-sud=%d ready=%d", number+1, (unsigned long)hr, match, g_pageReady.load());
        }
    }
    // A hook runs inside Explorer: whatever goes wrong here, the value as Windows read it is the answer.
    try {
        if (SUCCEEDED(hr) && name && value && count > 0 && !g_stopping.load() && g_pageReady.load() &&
            _wcsicmp(name, L"ResourceDLL") == 0 && IsSystemSud(value)) {
            std::wstring page;
            {
                SharedLock lock(g_stateLock);
                page = g_pagePath;
            }
            if (!page.empty() && page.size() < size_t(count) && GetFileAttributesW(page.c_str()) != INVALID_FILE_ATTRIBUTES)
                { wcscpy(value, page.c_str()); if (g_diagEnabled.load() && g_diagResourceReads.load() <= 30) Wh_Log(L"[DP-DIAG] ResourceDLL redirected to private page"); }
        }
    } catch (...) {
    }
    return hr;
}

// Restrict the interception to the loaded private page AND the exact system
// ComputerDefaults target. Do not affect unrelated shell launches.
using ShellExecCmdLine_t = HRESULT (WINAPI*)(HWND, LPCWSTR, LPCWSTR, INT, void*, DWORD);
ShellExecCmdLine_t ShellExecCmdLine_Orig = nullptr;
bool PrivateSpadTarget(void* caller, LPCWSTR target, std::wstring& replacement) {
    if (!target || g_stopping.load() || !g_pageReady.load()) return false;
    HMODULE from = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(caller), &from)) return false;
    wchar_t expanded[32768], sys[MAX_PATH];
    DWORD n = ExpandEnvironmentStringsW(target, expanded, ARRAYSIZE(expanded));
    UINT m = GetSystemDirectoryW(sys, ARRAYSIZE(sys));
    if (!n || n > ARRAYSIZE(expanded) || !m || m + 22 >= ARRAYSIZE(sys)) return false;
    wcscat(sys, L"\\ComputerDefaults.exe");
    if (_wcsicmp(expanded, sys)) return false;
    SharedLock lock(g_stateLock);
    if (from != g_pageModule || g_spadPath.empty()) return false;
    replacement = g_spadPath;
    return true;
}
// True if the open file has the pinned SHA-256. The handle was opened without write or delete sharing, so the file
// cannot change between this check and the launch while the caller keeps the handle open.
bool HandleHasSha256(HANDLE file, const char* expected) {
    Sha256 hash;
    for (;;) {
        uint8_t buf[65536]; DWORD got = 0;
        if (!ReadFile(file, buf, sizeof(buf), &got, nullptr)) return false;
        if (!got) break;
        hash.update(buf, got);
    }
    return hash.hex() == expected;
}
HRESULT WINAPI ShellExecCmdLine_Hook(HWND wnd, LPCWSTR command, LPCWSTR dir, INT show, void* unknown, DWORD flags) {
    try {
        std::wstring path;
        if (PrivateSpadTarget(__builtin_return_address(0), command, path)) {
            // Check the executable again right before it is started (it can be elevated), and keep it open without
            // write or delete sharing until the launch call returns.
            FileHandle held(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, nullptr));
            if (held.valid() && HandleHasSha256(held.get(), kSpadSha256)) {
                std::wstring quoted = L"\"" + path + L"\"";
                Wh_Log(L"[DP-SPAD] ShellExecCmdLine redirected to 1607 executable (re-verified)");
                HRESULT hr = ShellExecCmdLine_Orig(wnd, quoted.c_str(), dir, show, unknown, flags);
                Wh_Log(L"[DP-SPAD] ShellExecCmdLine hr=0x%08lX", (unsigned long)hr);
                return hr;
            }
            Wh_Log(L"[DP-SPAD] 1607 executable missing or changed since setup; using original link");
        }
    } catch (...) { Wh_Log(L"[DP-SPAD] redirect failed; using original"); }
    return ShellExecCmdLine_Orig(wnd, command, dir, show, unknown, flags);
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
CoCreateInstance_t CoCreateInstance_Orig;

HRESULT CreatePageObject(const GUID& systemClass, LPUNKNOWN outer, REFIID iid, LPVOID* object) {
    const DllGetClassObject_t getClassObject = g_getClassObject.load();
    if (!getClassObject) return CLASS_E_CLASSNOTAVAILABLE;
    IClassFactory* raw = nullptr;
    HRESULT hr = getClassObject(systemClass, kIidClassFactory, reinterpret_cast<void**>(&raw));
    std::unique_ptr<IClassFactory, ComDeleter> factory(raw);
    if (FAILED(hr) || !factory) return FAILED(hr) ? hr : E_NOINTERFACE;
    return factory->CreateInstance(outer, iid, object);
}

HRESULT CoCreateInstance_Impl(REFCLSID clsid, LPUNKNOWN outer, DWORD context, REFIID iid, LPVOID* object) {
    // This function is called very often: one comparison of the first field decides for almost every call.
    const DWORD first = clsid.Data1;
    if ((first == kClsidElementProvider.Data1 || first == kClsidAssocRegUi.Data1) && object && !g_stopping.load() &&
        g_pageReady.load() && (Same(clsid, kClsidElementProvider) || Same(clsid, kClsidAssocRegUi))) {
        // The classes of the page: from the private copy, once it is ready. If the class cannot be created from it,
        // Windows' own class (the same CLSID) is used.
        const HRESULT hr = CreatePageObject(clsid, outer, iid, object);
        if (SUCCEEDED(hr)) return hr;
        Wh_Log(L"The page class could not be created (0x%08lX); Windows' own class is used", hr);
    }
    return CoCreateInstance_Orig(clsid, outer, context, iid, object);
}

// CoCreateInstance is called by all of Explorer: an exception must not cross it. If the mod's own work fails, the call
// goes to Windows as if the mod were not there.
HRESULT WINAPI CoCreateInstance_Hook(REFCLSID clsid, LPUNKNOWN outer, DWORD context, REFIID iid, LPVOID* object) {
    try {
        return CoCreateInstance_Impl(clsid, outer, context, iid, object);
    } catch (...) {
        Wh_Log(L"CoCreateInstance: unexpected exception; passed to Windows");
    }
    return CoCreateInstance_Orig(clsid, outer, context, iid, object);
}

// ---- environment ----------------------------------------------------------------------------------------------------

bool IsNativeAmd64() {
    using IsWow64Process2_t = BOOL(WINAPI*)(HANDLE, USHORT*, USHORT*);
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    auto isWow64Process2 = kernel32 ? reinterpret_cast<IsWow64Process2_t>(reinterpret_cast<void*>(
                                          GetProcAddress(kernel32, "IsWow64Process2")))
                                    : nullptr;
    USHORT process = 0, native = 0;
    if (isWow64Process2 && isWow64Process2(GetCurrentProcess(), &process, &native))
        return native == IMAGE_FILE_MACHINE_AMD64;
    SYSTEM_INFO si{};
    GetNativeSystemInfo(&si);
    return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
}

DWORD WindowsBuild() {
    using RtlGetNtVersionNumbers_t = void(WINAPI*)(DWORD*, DWORD*, DWORD*);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto get = ntdll ? reinterpret_cast<RtlGetNtVersionNumbers_t>(reinterpret_cast<void*>(
                           GetProcAddress(ntdll, "RtlGetNtVersionNumbers")))
                     : nullptr;
    if (!get) return 0;
    DWORD major = 0, minor = 0, build = 0;
    get(&major, &minor, &build);
    return major >= 10 ? (build & 0xFFFF) : 0;
}

void DeleteFilesIn(const std::wstring& dir) {
    try {
        WIN32_FIND_DATAW fd;
        {
            FindHandle h(FindFirstFileW((dir + L"\\*").c_str(), &fd));
            if (!h.valid()) return;
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
            } while (FindNextFileW(h.get(), &fd));
        }      // the search handle is closed before the folder is removed
        RemoveDirectoryW(dir.c_str());
    } catch (...) {
    }
}

}  // namespace

// =====================================================================================================================
// Windhawk entry points
// =====================================================================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"[DP-DIAG] Wh_ModInit pid=%lu", GetCurrentProcessId());
    if (!IsNativeAmd64()) {
        Wh_Log(L"Not an x64 Windows: the mod does nothing here");
        return FALSE;
    }
    const DWORD build = WindowsBuild();
    if (build < kMinBuild) {
        Wh_Log(L"Windows build %lu is older than Windows 10 1607: the mod does nothing here", build);
        return FALSE;
    }
    g_stopping.store(false);
    LoadSettings();
    compat::LoadSettings();
    g_diagEnabled.store(Wh_GetIntSetting(L"enableLogging") != 0);
    Wh_Log(L"[DP-DIAG] Windows build=%lu debug=%d", build, g_diagEnabled.load());

    HMODULE propsys = LoadLibraryExW(L"propsys.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE combase = GetModuleHandleW(L"combase.dll");
    void* readStr = propsys ? reinterpret_cast<void*>(GetProcAddress(propsys, "PSPropertyBag_ReadStr")) : nullptr;
    void* coCreate = combase ? reinterpret_cast<void*>(GetProcAddress(combase, "CoCreateInstance")) : nullptr;
    if (!readStr || !coCreate) {
        Wh_Log(L"PSPropertyBag_ReadStr or CoCreateInstance not found: the mod does nothing here");
        return FALSE;
    }
    if (!Wh_SetFunctionHook(readStr, reinterpret_cast<void*>(PSPropertyBag_ReadStr_Hook),
                            reinterpret_cast<void**>(&PSPropertyBag_ReadStr_Orig)) ||
        !Wh_SetFunctionHook(coCreate, reinterpret_cast<void*>(CoCreateInstance_Hook),
                            reinterpret_cast<void**>(&CoCreateInstance_Orig))) {
        Wh_Log(L"Could not set the hooks");
        return FALSE;
    }
    // The 1607 page imports ShellExecCmdLine by shell32 ordinal 265.
    HMODULE shell = GetModuleHandleW(L"shell32.dll");
    void* exec = shell ? reinterpret_cast<void*>(GetProcAddress(shell, MAKEINTRESOURCEA(265))) : nullptr;
    bool spadHook = exec && Wh_SetFunctionHook(exec, reinterpret_cast<void*>(ShellExecCmdLine_Hook), reinterpret_cast<void**>(&ShellExecCmdLine_Orig));
    Wh_Log(L"[DP-SPAD] optional ShellExecCmdLine hook=%d", spadHook);
    compat::InstallSystemHooks();
    if (!g_stopEvent) g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) ResetEvent(g_stopEvent);
    else { Wh_Log(L"[DP-DIAG] stop event creation failed error=%lu", GetLastError()); return FALSE; }
    // Download, verification, generation and loading happen on a worker thread: Explorer never waits for them. Until
    // they are done (or if they fail) the redirection hooks change nothing.
    StartSetup();
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = FALSE;
    LoadSettings();
    compat::LoadSettings();
    g_diagEnabled.store(Wh_GetIntSetting(L"enableLogging") != 0);
    StartSetup();
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_stopping.store(true);
    if (g_stopEvent) SetEvent(g_stopEvent);
    CancelDownload();
    if (g_setupThread) WaitForSingleObject(g_setupThread, INFINITE);
}

void Wh_ModUninit() {
    if (g_setupThread) {
        CancelDownload();
        // No time limit: every wait in the thread is on g_stopEvent, a download is cancelled and the loops check
        // g_stopping, so it ends promptly, and the mod's code must not be unmapped under it.
        WaitForSingleObject(g_setupThread, INFINITE);
        CloseHandle(g_setupThread);
        g_setupThread = nullptr;
    }
    g_pageReady.store(false);
    try {
        std::wstring storage;
        {
            ExclusiveLock lock(g_stateLock);
            storage = g_storage;
        }
        if (!g_keepFiles.load()) {
            if (storage.empty()) storage = StorageDir();
            if (!storage.empty()) {
                // Files that are loaded (here or in another Explorer) cannot be deleted and stay until a later unload.
                DeleteFilesIn(storage + L"\\bin\\" + kGeneratorRevision);
                DeleteFilesIn(storage + L"\\dl");
            }
        }
    } catch (...) {
        Wh_Log(L"Wh_ModUninit: unexpected exception");
    }
}
