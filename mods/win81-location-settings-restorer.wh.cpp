// ==WindhawkMod==
// @id              win81-location-settings-restorer
// @name            Windows 7/8.1 Location Settings Applet Restorer
// @description     This mod restores the Windows 7/8.1 Location Settings Control Panel applet on Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @include         dllhost.exe
// @include         svchost.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lwininet -ladvapi32 -lole32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7/8.1 Location Settings Applet Restorer

This mod restores the classic Windows 8.1 **Location Settings** Control Panel applet (along with optional Windows 7-era left-pane navigation links) that was removed from Windows 10 and Windows 11.

**IMPORTANT:** `svchost.exe` must be included in the mod's target processes for this mod to start and work properly.

## Screenshot

![Location Settings](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/locationsettings.png)

## About

The mod downloads the original Windows 8.1 binaries (`sensorscpl.dll` and `dui70.dll`) plus the Windows 7 `duser.dll` stylesheet from Microsoft's public symbol server on first use, verifies every download with pinned SHA-256 hashes, applies the required compatibility patches (including DirectUI side-by-side isolation, an inlined stylesheet, the Group Policy banner layout fix, and Windows 10/11 Location setting synchronization), and registers the applet virtually so that no permanent system files or registry keys are modified.

It supports 6 languages using authentic Windows 8.1 Microsoft translations: English, Italian, French, Spanish, Portuguese, and German. The language can follow the Windows UI language or be set to a specific locale.
This mod has been tested on Windows 11 24H2.
You can also optionally enable the **Show Windows 7 navigation links** setting to add the classic Windows 7 left-pane links (**View location activity** to open the Windows Location Event Viewer log, and **Default Location** under *See also* to open Windows Location settings).

The applet appears under **Hardware and Sound** in Control Panel Category view, in Large / Small Icons view (with hover InfoTip), and in Control Panel Search. You can also open it directly using either:

- Canonical name: `control /name Microsoft.LocationSettings`
- CLSID (Run dialog / Explorer address bar): `shell:::{E9950154-C418-419e-A90A-20C5287AE24B}` (or `explorer shell:::{E9950154-C418-419e-A90A-20C5287AE24B}`)

Note: an internet connection is required on first use, and it might take a moment to load the applet while the mod downloads the required dependencies. After the first-time download completes, refresh or reopen Control Panel.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $description: This setting selects the display language for the Location Settings applet using authentic Windows 8.1 translations. "Automatic" follows the Windows UI language.
  $options:
  - auto: Automatic (match Windows UI language)
  - en: English
  - it: Italiano
  - fr: Français
  - es: Español
  - pt: Português
  - de: Deutsch
- showControlPanelTasks: true
  $name: Show Control Panel category task links
  $description: This setting controls whether the classic blue "Change location settings" task link is displayed under Location Settings in Category view and Control Panel Search.
- showWin7NavLinks: false
  $name: Show Windows 7 navigation links
  $description: This setting adds the classic Windows 7 left-pane navigation links ("View location activity" and "Default Location" under "See also") to the Location Settings page. "View location activity" opens the Windows Location Event Viewer log, and "Default Location" opens Windows Location settings.
- keepDownloadedFiles: true
  $name: Keep cached Microsoft binaries when disabled
  $description: This setting keeps the downloaded Microsoft binaries in mod storage when the mod is disabled so they do not need to be downloaded again when re-enabling the mod.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#ifndef LOC_GENERATOR_ONLY
#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <sddl.h>
#include <unknwn.h>
#include <ocidl.h>
#endif

namespace {

using Bytes = std::vector<uint8_t>;

// ============================================================================
// 1. Supported Languages & Authentic Windows 8.1 Microsoft String Tables
//    Extracted verbatim from official KB3012997 Win8.1 Language Packs
//    (SensorsCpl.dll.mui: 79 strings; duser.dll.mui: 4 strings remapped 1100->2000)
// ============================================================================

enum class Lang : int {
    En = 0,
    It = 1,
    Fr = 2,
    Es = 3,
    Pt = 4,
    De = 5,
};

struct StrEntry {
    uint16_t id;
    const wchar_t* text;
};

static const StrEntry kStrings_en[] = {
    {1u, L"Location Settings"},
    {20u, L"Home page"},
    {21u, L"Sensor Properties"},
    {22u, L"User Settings"},
    {50u, L"Connected"},
    {51u, L"Not connected"},
    {201u, L"%1 (%2)"},
    {202u, L"Sensor"},
    {203u, L"User Accounts"},
    {205u, L"Limited access"},
    {207u, L"Error"},
    {208u, L"Administrator credentials are required to change this setting"},
    {209u, L"You do not have the correct permissions to change this setting. For more information, contact your system administrator."},
    {210u, L"Change user settings"},
    {211u, L"View sensors by name"},
    {212u, L"View sensors by date"},
    {213u, L"%1"},
    {214u, L"Enabled"},
    {215u, L"Not enabled"},
    {216u, L"Limited access"},
    {217u, L"%1 %2"},
    {218u, L"%1"},
    {219u, L"Everyone"},
    {220u, L"Services"},
    {223u, L"Always enabled"},
    {224u, L"Default Location"},
    {225u, L"View location activity"},
    {228u, L"Proximity settings"},
    {450u, L"Confirm Sensor Uninstallation"},
    {451u, L"Do you want to uninstall this sensor?"},
    {452u, L"To finish uninstalling the sensor, you must restart your computer."},
    {453u, L"Restart now"},
    {454u, L"Restart later"},
    {701u, L"Configure your sensor settings."},
    {702u, L"Move up"},
    {703u, L"Move down"},
    {704u, L"Click to change settings for "},
    {801u, L"%s settings"},
    {900u, L"One or more gadgets could not be turned on or off"},
    {907u, L"Windows was unable to save the changes to the device. Make sure the device is connected to the computer, and then try again."},
    {1000u, L"Enable sensors"},
    {1001u, L"Sensors can detect information about your computer's current location, surroundings, and more. Once a sensor is enabled, all programs and users can access it. \n\n<A HREF=\"HELP\">How is my privacy affected?</A>\n\nEnable these sensors:"},
    {1002u, L"Sensors can detect information about your computer's current location, surroundings, and more."},
    {1003u, L"Enable these sensors"},
    {1004u, L"Don't enable these sensors\nYou can enable these sensors later in Location and Other Sensors in Control Panel."},
    {1005u, L"Open Location and Other Sensors for more options"},
    {1006u, L"Open Location and Other Sensors to enable sensors"},
    {1010u, L"Enable sensors"},
    {1011u, L"Sensors can detect information about your computer's current location, surroundings, and more. Once a sensor is enabled, all programs and users can access it. \n\n<A HREF=\"HELP\">How is my privacy affected?</A>\n\nEnable this sensor:"},
    {1013u, L"Enable this sensor"},
    {1014u, L"Don't enable this sensor\nYou can enable this sensor later in Location and Other Sensors in Control Panel."},
    {1015u, L"Open Location and Other Sensors for more options"},
    {1016u, L"Open Location and Other Sensors to enable sensors"},
    {1030u, L"Enable location sensors"},
    {1031u, L"Location sensors can detect your computer's current location. Once a location sensor is enabled, all programs and users on this computer can use your location information.\n\n<A HREF=\"HELP\">How is my privacy affected?</A>\n\nEnable these location sensors:"},
    {1032u, L"Location sensors can detect your computer's current location."},
    {1033u, L"Enable these location sensors"},
    {1034u, L"Don't enable these location sensors\nYou can enable these sensors later in Location and Other Sensors in Control Panel."},
    {1035u, L"Open Location and Other Sensors for more options"},
    {1036u, L"Open Location and Other Sensors to enable location sensors"},
    {1040u, L"Enable location sensors"},
    {1041u, L"Location sensors can detect your computer's current location. Once a location sensor is enabled, all programs and users on this computer can use your location information.\n\n<A HREF=\"HELP\">How is my privacy affected?</A>\n\nEnable this location sensor:"},
    {1043u, L"Enable this location sensor"},
    {1044u, L"Don't enable this location sensor\nYou can enable this sensor later in Location and Other Sensors in Control Panel."},
    {1045u, L"Open Location and Other Sensors for more options"},
    {1046u, L"Open Location and Other Sensors to enable this location sensor"},
    {1100u, L"Cancel"},
    {1101u, L"OK"},
    {1102u, L"These settings are managed by your system administrator."},
    {1103u, L"Why can't I change these settings?"},
    {1104u, L"Change location settings"},
    {1105u, L"When signed in as administrator, you can let users control how apps use their location and help Microsoft improve its location services."},
    {1106u, L"Privacy statement"},
    {1107u, L"Turn on the Windows Location platform"},
    {1108u, L"Let users choose their own location settings."},
    {1109u, L"Help improve Microsoft location services"},
    {1110u, L"Let Windows periodically send GPS and other location information to Microsoft when you use location-aware apps. We won't use this information to identify or contact you."},
    {1111u, L"Apply"},
    {2000u, L"Used to change vertical viewing area"},
    {2001u, L"Used to change horizontal viewing area"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
    {10000u, L"Sensors"},
};

static const StrEntry kStrings_it[] = {
    {1u, L"Impostazioni posizione"},
    {20u, L"Home page"},
    {21u, L"Propriet\u00E0 sensore"},
    {22u, L"Impostazioni utente"},
    {50u, L"Connesso"},
    {51u, L"Non connesso"},
    {201u, L"%1 (%2)"},
    {202u, L"Sensore"},
    {203u, L"Account utente"},
    {205u, L"Accesso limitato"},
    {207u, L"Errore"},
    {208u, L"Per modificare questa impostazione sono necessarie le credenziali di amministratore"},
    {209u, L"Non si dispone delle autorizzazioni corrette per modificare questa impostazione. Per ulteriori informazioni, contattare l'amministratore del sistema."},
    {210u, L"Cambia impostazioni utente"},
    {211u, L"Visualizza sensori per nome"},
    {212u, L"Visualizza sensori per data"},
    {213u, L"%1"},
    {214u, L"Attivato"},
    {215u, L"Non abilitato"},
    {216u, L"Accesso limitato"},
    {217u, L"%1 %2"},
    {218u, L"%1"},
    {219u, L"Tutti"},
    {220u, L"Servizi"},
    {223u, L"Sempre attivato"},
    {224u, L"Posizione predefinita"},
    {225u, L"Visualizza attivit\u00E0 posizione"},
    {228u, L"Impostazioni prossimit\u00E0"},
    {450u, L"Conferma disinstallazione sensore"},
    {451u, L"Disinstallare il sensore?"},
    {452u, L"Per completare la disinstallazione del sensore, \u00E8 necessario riavviare il computer."},
    {453u, L"Riavvia ora"},
    {454u, L"Riavvia in seguito"},
    {701u, L"Configura le impostazioni dei sensori."},
    {702u, L"Sposta su"},
    {703u, L"Sposta gi\u00F9"},
    {704u, L"Fare clic per modificare le impostazioni per "},
    {801u, L"Impostazioni di %s"},
    {900u, L"Impossibile attivare o disattivare uno o pi\u00F9 gadget"},
    {907u, L"Impossibile salvare le modifiche nel dispositivo. Controllare che il dispositivo sia collegato al computer e riprovare."},
    {1000u, L"Attiva sensori"},
    {1001u, L"I sensori possono rilevare informazioni sulla posizione corrente del computer, sull'ambiente circostante e altro ancora. Se \u00E8 attivato un sensore, tutti i programmi e gli utenti possono accedervi.\n\n<A HREF=\"HELP\">Effetti sulla privacy</A>\n\nAttiva questi sensori:"},
    {1002u, L"I sensori possono rilevare informazioni sulla posizione corrente del computer, sull'ambiente circostante e altro ancora."},
    {1003u, L"Attiva questi sensori"},
    {1004u, L"Non attivare questi sensori\nSar\u00E0 possibile attivarli in un secondo momento in Sensore di posizione e altri sensori nel Pannello di controllo."},
    {1005u, L"Apri Sensore di posizione e altri sensori per ulteriori opzioni"},
    {1006u, L"Apri Sensore di posizione e altri sensori per attivare i sensori"},
    {1010u, L"Attiva sensori"},
    {1011u, L"I sensori possono rilevare informazioni sulla posizione corrente del computer, sull'ambiente circostante e altro ancora. Se \u00E8 attivato un sensore, tutti i programmi e gli utenti possono accedervi.\n\n<A HREF=\"HELP\">Effetti sulla privacy</A>\n\nAttiva questo sensore:"},
    {1013u, L"Attiva questo sensore"},
    {1014u, L"Non attivare questo sensore\nSar\u00E0 possibile attivarlo in un secondo momento in Sensore di posizione e altri sensori nel Pannello di controllo."},
    {1015u, L"Apri Sensore di posizione e altri sensori per ulteriori opzioni"},
    {1016u, L"Apri Sensore di posizione e altri sensori per attivare i sensori"},
    {1030u, L"Attiva sensori di posizione"},
    {1031u, L"I sensori di posizione possono rilevare la posizione corrente del computer. Se \u00E8 attivato un sensore di posizione, tutti i programmi e gli utenti del computer possono utilizzare le informazioni relative alla posizione.\n\n<A HREF=\"HELP\">Effetti sulla privacy</A>\n\nAttiva questi sensori di posizione:"},
    {1032u, L"I sensori di posizione possono rilevare la posizione corrente del computer."},
    {1033u, L"Attiva questi sensori di posizione"},
    {1034u, L"Non attivare questi sensori di posizione\nSar\u00E0 possibile attivarli in un secondo momento in Sensore di posizione e altri sensori nel Pannello di controllo."},
    {1035u, L"Apri Sensore di posizione e altri sensori per ulteriori opzioni"},
    {1036u, L"Apri Sensore di posizione e altri sensori per attivare i sensori di posizione"},
    {1040u, L"Attiva sensori di posizione"},
    {1041u, L"I sensori di posizione possono rilevare la posizione corrente del computer. Se \u00E8 attivato un sensore di posizione, tutti i programmi e gli utenti del computer possono utilizzare le informazioni relative alla posizione.\n\n<A HREF=\"HELP\">Effetti sulla privacy</A>\n\nAttiva questo sensore di posizione:"},
    {1043u, L"Attiva questo sensore di posizione"},
    {1044u, L"Non attivare questo sensore di posizione\nSar\u00E0 possibile attivarlo in un secondo momento in Sensore di posizione e altri sensori nel Pannello di controllo."},
    {1045u, L"Apri Sensore di posizione e altri sensori per ulteriori opzioni"},
    {1046u, L"Apri Sensore di posizione e altri sensori per attivare il sensore di posizione"},
    {1100u, L"Annulla"},
    {1101u, L"OK"},
    {1102u, L"Queste impostazioni sono gestite dall'amministratore del sistema."},
    {1103u, L"Ragioni che impediscono la modifica di queste impostazioni"},
    {1104u, L"Modifica impostazioni posizione"},
    {1105u, L"Effettuando l'accesso come amministratore, \u00E8 possibile consentire agli utenti di controllare il modo in cui le app utilizzano le informazioni sulla propria posizione e contribuire al miglioramento dei servizi di localizzazione geografica di Microsoft."},
    {1106u, L"Informativa sulla privacy"},
    {1107u, L"Attiva la piattaforma di localizzazione geografica di Windows"},
    {1108u, L"Consenti agli utenti di scegliere le proprie impostazioni sulla posizione."},
    {1109u, L"Contribuisci al miglioramento dei servizi di localizzazione geografica di Microsoft"},
    {1110u, L"Consenti l'invio delle informazioni GPS e di altre informazioni sulla posizione a Microsoft durante l'utilizzo di app in grado di rilevare la posizione geografica. Le informazioni non verranno utilizzate per identificare o contattare l'utente."},
    {1111u, L"Applica"},
    {2000u, L"Per modificare l'area di visualizzazione verticale"},
    {2001u, L"Per modificare l'area di visualizzazione orizzontale"},
    {2002u, L"Orizzontale"},
    {2003u, L"Verticale"},
    {10000u, L"Sensori"},
};

static const StrEntry kStrings_fr[] = {
    {1u, L"Param\u00E8tres de localisation"},
    {20u, L"Page d\u2019accueil"},
    {21u, L"Propri\u00E9t\u00E9s du capteur"},
    {22u, L"Param\u00E8tres utilisateurs"},
    {50u, L"Connect\u00E9"},
    {51u, L"Non connect\u00E9"},
    {201u, L"%1 (%2)"},
    {202u, L"Capteur"},
    {203u, L"Comptes d\u2019utilisateurs"},
    {205u, L"Acc\u00E8s limit\u00E9"},
    {207u, L"Erreur"},
    {208u, L"Les informations d\u2019identification de l\u2019administrateur sont requises pour modifier ce param\u00E8tre."},
    {209u, L"Vous ne poss\u00E9dez pas les autorisations requises pour modifier ce param\u00E8tre. Pour plus d\u2019informations, contactez votre administrateur syst\u00E8me."},
    {210u, L"Changer les param\u00E8tres utilisateurs"},
    {211u, L"Afficher les capteurs par nom"},
    {212u, L"Afficher les capteurs par date"},
    {213u, L"%1"},
    {214u, L"Activ\u00E9"},
    {215u, L"Non activ\u00E9"},
    {216u, L"Acc\u00E8s limit\u00E9"},
    {217u, L"%1 %2"},
    {218u, L"%1"},
    {219u, L"Tout le monde"},
    {220u, L"Services"},
    {223u, L"Toujours activ\u00E9"},
    {224u, L"Localisation par d\u00E9faut"},
    {225u, L"Afficher l\u2019activit\u00E9 de la localisation"},
    {228u, L"Param\u00E8tres de proximit\u00E9"},
    {450u, L"Confirmer la d\u00E9sinstallation du capteur"},
    {451u, L"Voulez-vous d\u00E9sinstaller ce capteur\u00A0?"},
    {452u, L"Pour terminer la d\u00E9sinstallation du capteur, vous devez red\u00E9marrer l\u2019ordinateur."},
    {453u, L"Red\u00E9marrer maintenant"},
    {454u, L"Red\u00E9marrer ult\u00E9rieurement"},
    {701u, L"Configurez les param\u00E8tres des capteurs."},
    {702u, L"Monter"},
    {703u, L"Descendre"},
    {704u, L"Cliquez pour modifier les param\u00E8tres pour "},
    {801u, L"Param\u00E8tres de %s"},
    {900u, L"Certains gadgets n\u2019ont pas pu \u00EAtre activ\u00E9s ou d\u00E9sactiv\u00E9s."},
    {907u, L"Windows n\u2019a pas pu enregistrer les modifications sur le p\u00E9riph\u00E9rique. V\u00E9rifiez que le p\u00E9riph\u00E9rique est connect\u00E9 \u00E0 l\u2019ordinateur, puis recommencez."},
    {1000u, L"Activer les capteurs"},
    {1001u, L"Les capteurs peuvent d\u00E9tecter des informations sur la localisation actuelle, l\u2019environnement, etc. de votre ordinateur. Une fois qu\u2019un capteur est activ\u00E9, tous les programmes et utilisateurs peuvent y acc\u00E9der. <A HREF=\"HELP\">En quoi ma confidentialit\u00E9 est-elle affect\u00E9e\u00A0?</A> Activer ces capteurs\u00A0:"},
    {1002u, L"Les capteurs peuvent d\u00E9tecter des informations sur la localisation actuelle, l\u2019environnement, etc. de votre ordinateur."},
    {1003u, L"Activer ces capteurs"},
    {1004u, L"Ne pas activer ces capteurs Vous pouvez activer ces capteurs ult\u00E9rieurement dans le Panneau de configuration Localisation et autres capteurs."},
    {1005u, L"Ouvrir Localisation et autres capteurs pour plus d\u2019options"},
    {1006u, L"Ouvrir Localisation et autres capteurs pour activer les capteurs"},
    {1010u, L"Activer les capteurs"},
    {1011u, L"Les capteurs peuvent d\u00E9tecter des informations sur la localisation actuelle, l\u2019environnement, etc. de votre ordinateur. Une fois qu\u2019un capteur est activ\u00E9, tous les programmes et utilisateurs peuvent y acc\u00E9der. <A HREF=\"HELP\">En quoi ma confidentialit\u00E9 est-elle affect\u00E9e\u00A0?</A> Activer ce capteur\u00A0:"},
    {1013u, L"Activer ce capteur"},
    {1014u, L"Ne pas activer ce capteur Vous pouvez activer ce capteur ult\u00E9rieurement dans le Panneau de configuration Localisation et autres capteurs."},
    {1015u, L"Ouvrir Localisation et autres capteurs pour plus d\u2019options"},
    {1016u, L"Ouvrir Localisation et autres capteurs pour activer les capteurs"},
    {1030u, L"Activer les capteurs de localisation"},
    {1031u, L"Les capteurs de localisation peuvent d\u00E9tecter la localisation actuelle de votre ordinateur. Une fois qu\u2019un capteur de localisation est activ\u00E9, tous les programmes et utilisateurs existant sur cet ordinateur peuvent utiliser les informations concernant votre localisation. <A HREF=\"HELP\">En quoi ma confidentialit\u00E9 est-elle affect\u00E9e\u00A0?</A> Activer ces capteurs de localisation\u00A0:"},
    {1032u, L"Les capteurs de localisation peuvent d\u00E9tecter la localisation actuelle de votre ordinateur."},
    {1033u, L"Activer ces capteurs de localisation"},
    {1034u, L"Ne pas activer ces capteurs de localisation Vous pouvez activer ces capteurs ult\u00E9rieurement dans le Panneau de configuration Localisation et autres capteurs."},
    {1035u, L"Ouvrir Localisation et autres capteurs pour plus d\u2019options"},
    {1036u, L"Ouvrir Localisation et autres capteurs pour activer les capteurs de localisation"},
    {1040u, L"Activer les capteurs de localisation"},
    {1041u, L"Les capteurs de localisation peuvent d\u00E9tecter la localisation actuelle de votre ordinateur. Une fois qu\u2019un capteur de localisation est activ\u00E9, tous les programmes et utilisateurs existant sur cet ordinateur peuvent utiliser les informations concernant votre localisation. <A HREF=\"HELP\">En quoi ma confidentialit\u00E9 est-elle affect\u00E9e\u00A0?</A> Activer ce capteur de localisation\u00A0:"},
    {1043u, L"Activer ce capteur de localisation"},
    {1044u, L"Ne pas activer ce capteur de localisation Vous pouvez activer ce capteur ult\u00E9rieurement dans le Panneau de configuration Localisation et autres capteurs."},
    {1045u, L"Ouvrir Localisation et autres capteurs pour plus d\u2019options"},
    {1046u, L"Ouvrir Localisation et autres capteurs pour activer ce capteur de localisation"},
    {1100u, L"Annuler"},
    {1101u, L"OK"},
    {1102u, L"Ces param\u00E8tres sont g\u00E9r\u00E9s par votre administrateur syst\u00E8me."},
    {1103u, L"Pourquoi ne puis-je pas modifier ces param\u00E8tres\u00A0?"},
    {1104u, L"Modifier les param\u00E8tres de localisation"},
    {1105u, L"Vous pouvez contr\u00F4ler la fa\u00E7on dont les applications utilisent votre localisation et aider Microsoft \u00E0 am\u00E9liorer ses services de localisation."},
    {1106u, L"D\u00E9claration de confidentialit\u00E9"},
    {1107u, L"Activer la plateforme de localisation Windows"},
    {1108u, L"Laisser les utilisateurs choisir leurs propres param\u00E8tres de localisation."},
    {1109u, L"Participer \u00E0 l\u2019am\u00E9lioration des services de localisation Microsoft"},
    {1110u, L"Laisser Windows collecter p\u00E9riodiquement des informations GPS et d\u2019autres informations sur votre localisation et les envoyer \u00E0 Microsoft lorsque vous utilisez des applications prenant en charge les localisations. Nous n\u2019utiliserons pas ces informations pour vous identifier ou vous contacter."},
    {1111u, L"Appliquer"},
    {2000u, L"Utilis\u00E9 pour modifier la zone d\u2019affichage verticale"},
    {2001u, L"Utilis\u00E9 pour modifier la zone d\u2019affichage horizontale"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
    {10000u, L"Capteurs"},
};

static const StrEntry kStrings_es[] = {
    {1u, L"Configuraci\u00F3n de ubicaci\u00F3n"},
    {20u, L"P\u00E1gina principal"},
    {21u, L"Propiedades del sensor"},
    {22u, L"Configuraci\u00F3n de usuario"},
    {50u, L"Conectado"},
    {51u, L"No conectado"},
    {201u, L"%1 (%2)"},
    {202u, L"Sensor"},
    {203u, L"Cuentas de usuario"},
    {205u, L"Acceso limitado"},
    {207u, L"Error"},
    {208u, L"Son necesarias credenciales de administrador para cambiar esta configuraci\u00F3n"},
    {209u, L"No tiene los permisos suficientes para cambiar esta configuraci\u00F3n. P\u00F3ngase en contacto con el administrador del sistema para obtener m\u00E1s informaci\u00F3n."},
    {210u, L"Cambiar configuraci\u00F3n de usuario"},
    {211u, L"Ver sensores por nombre"},
    {212u, L"Ver sensores por fecha"},
    {213u, L"%1"},
    {214u, L"Habilitadas"},
    {215u, L"No habilitado"},
    {216u, L"Acceso limitado"},
    {217u, L"%1 %2"},
    {218u, L"%1"},
    {219u, L"Todos"},
    {220u, L"Servicios"},
    {223u, L"Siempre habilitado"},
    {224u, L"Ubicaci\u00F3n predeterminada"},
    {225u, L"Ver actividad de ubicaci\u00F3n"},
    {228u, L"Configuraci\u00F3n de proximidad"},
    {450u, L"Confirmar desinstalaci\u00F3n de sensor"},
    {451u, L"\u00BFDesea desinstalar este sensor?"},
    {452u, L"Para finalizar la desinstalaci\u00F3n del sensor, debe reiniciar el equipo."},
    {453u, L"Reiniciar ahora"},
    {454u, L"Reiniciar m\u00E1s tarde"},
    {701u, L"Establezca la configuraci\u00F3n del sensor."},
    {702u, L"Subir"},
    {703u, L"Bajar"},
    {704u, L"Haga clic para cambiar la configuraci\u00F3n de "},
    {801u, L"Configuraci\u00F3n de %s"},
    {900u, L"No se puede activar o desactivar por lo menos un gadget"},
    {907u, L"Windows no pudo guardar los cambios al dispositivo. Aseg\u00FArese de que el dispositivo est\u00E9 conectado al equipo e int\u00E9ntelo de nuevo."},
    {1000u, L"Habilitar sensores"},
    {1001u, L"Los sensores pueden detectar informaci\u00F3n acerca de la ubicaci\u00F3n actual del equipo, sus alrededores y otros datos. Una vez que se habilita un sensor, todos los programas y usuarios pueden obtener acceso al mismo.\n\n<A HREF=\"HELP\">\u00BFC\u00F3mo se ve afectada mi privacidad?</A>\n\nHabilitar estos sensores:"},
    {1002u, L"Los sensores pueden detectar informaci\u00F3n acerca de la ubicaci\u00F3n actual del equipo, sus alrededores y otros datos."},
    {1003u, L"Habilitar estos sensores"},
    {1004u, L"No habilitar estos sensores\nPuede habilitar estos sensores m\u00E1s adelante en Ubicaci\u00F3n y otros sensores en el Panel de control."},
    {1005u, L"Abrir Ubicaci\u00F3n y otros sensores para ver m\u00E1s opciones"},
    {1006u, L"Abrir Ubicaci\u00F3n y otros sensores para habilitar los sensores"},
    {1010u, L"Habilitar sensores"},
    {1011u, L"Los sensores pueden detectar informaci\u00F3n acerca de la ubicaci\u00F3n actual del equipo, sus alrededores y otros datos. Una vez que se habilita un sensor, todos los programas y usuarios pueden obtener acceso al mismo.\n\n<A HREF=\"HELP\">\u00BFC\u00F3mo se ve afectada mi privacidad?</A>\n\nHabilitar este sensor:"},
    {1013u, L"Habilitar este sensor"},
    {1014u, L"No habilitar este sensor\nPuede habilitar este sensor m\u00E1s adelante en Ubicaci\u00F3n y otros sensores en el Panel de control."},
    {1015u, L"Abrir Ubicaci\u00F3n y otros sensores para ver m\u00E1s opciones"},
    {1016u, L"Abrir Ubicaci\u00F3n y otros sensores para habilitar los sensores"},
    {1030u, L"Habilitar sensores de ubicaci\u00F3n"},
    {1031u, L"Los sensores de ubicaci\u00F3n pueden detectar la ubicaci\u00F3n actual del equipo. Una vez que se habilita un sensor de ubicaci\u00F3n, todos los programas y usuarios de este equipo pueden usar su informaci\u00F3n de ubicaci\u00F3n.\n\n<A HREF=\"HELP\">\u00BFC\u00F3mo se ve afectada mi privacidad?</A>\n\nHabilitar estos sensores de ubicaci\u00F3n:"},
    {1032u, L"Los sensores de ubicaci\u00F3n pueden detectar la ubicaci\u00F3n actual del equipo."},
    {1033u, L"Habilitar estos sensores de ubicaci\u00F3n"},
    {1034u, L"No habilitar estos sensores de ubicaci\u00F3n\nPuede habilitar estos sensores m\u00E1s adelante en Ubicaci\u00F3n y otros sensores en el Panel de control."},
    {1035u, L"Abrir Ubicaci\u00F3n y otros sensores para ver m\u00E1s opciones"},
    {1036u, L"Abrir Ubicaci\u00F3n y otros sensores para habilitar los sensores de ubicaci\u00F3n"},
    {1040u, L"Habilitar sensores de ubicaci\u00F3n"},
    {1041u, L"Los sensores de ubicaci\u00F3n pueden detectar la ubicaci\u00F3n actual del equipo. Una vez que se habilita un sensor de ubicaci\u00F3n, todos los programas y usuarios de este equipo pueden usar su informaci\u00F3n de ubicaci\u00F3n.\n\n<A HREF=\"HELP\">\u00BFC\u00F3mo se ve afectada mi privacidad?</A>\n\nHabilitar este sensor de ubicaci\u00F3n:"},
    {1043u, L"Habilitar este sensor de ubicaci\u00F3n"},
    {1044u, L"No habilitar este sensor de ubicaci\u00F3n\nPuede habilitar este sensor m\u00E1s adelante en Ubicaci\u00F3n y otros sensores en el Panel de control."},
    {1045u, L"Abrir Ubicaci\u00F3n y otros sensores para ver m\u00E1s opciones"},
    {1046u, L"Abrir Ubicaci\u00F3n y otros sensores para habilitar este sensor de ubicaci\u00F3n"},
    {1100u, L"Cancelar"},
    {1101u, L"Aceptar"},
    {1102u, L"El administrador del sistema administra estas opciones de configuraci\u00F3n."},
    {1103u, L"\u00BFPor qu\u00E9 no puedo cambiar estas opciones de configuraci\u00F3n?"},
    {1104u, L"Cambiar configuraci\u00F3n de ubicaci\u00F3n"},
    {1105u, L"Cuando inicie sesi\u00F3n como administrador, puede permitir a los usuarios controlar el modo en que las aplicaciones usan la ubicaci\u00F3n y ayudar a Microsoft a mejorar sus servicios de localizaci\u00F3n."},
    {1106u, L"Declaraci\u00F3n de privacidad"},
    {1107u, L"Activar la plataforma de ubicaci\u00F3n de Windows"},
    {1108u, L"Permita que los usuarios elijan su propia configuraci\u00F3n de ubicaci\u00F3n."},
    {1109u, L"Ayudar a mejorar los servicios de localizaci\u00F3n de Microsoft"},
    {1110u, L"Permita que Windows env\u00EDe peri\u00F3dicamente informaci\u00F3n de GPS y otros datos de ubicaci\u00F3n a Microsoft cuando utilice aplicaciones con reconocimiento de ubicaci\u00F3n de red. No usaremos esta informaci\u00F3n para identificarlo ni para ponernos en contacto con usted."},
    {1111u, L"Aplicar"},
    {2000u, L"Se usa para cambiar el \u00E1rea de observaci\u00F3n vertical"},
    {2001u, L"Se usa para cambiar el \u00E1rea de observaci\u00F3n horizontal"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
    {10000u, L"Sensores"},
};

static const StrEntry kStrings_pt[] = {
    {1u, L"Configura\u00E7\u00F5es de Local"},
    {20u, L"Home page"},
    {21u, L"Propriedades do Sensor"},
    {22u, L"Configura\u00E7\u00F5es de Usu\u00E1rio"},
    {50u, L"Conectado"},
    {51u, L"Desconectado"},
    {201u, L"%1 (%2)"},
    {202u, L"Sensor"},
    {203u, L"Contas de Usu\u00E1rio"},
    {205u, L"Acesso limitado"},
    {207u, L"Erro"},
    {208u, L"\u00C9 necess\u00E1rio ter credenciais de administrador para alterar esta configura\u00E7\u00E3o"},
    {209u, L"Voc\u00EA n\u00E3o tem as permiss\u00F5es corretas para alterar esta configura\u00E7\u00E3o. Para obter mais informa\u00E7\u00F5es, contate o administrador do sistema."},
    {210u, L"Alterar configura\u00E7\u00F5es do usu\u00E1rio"},
    {211u, L"Exibir sensores por nome"},
    {212u, L"Exibir sensores por data"},
    {213u, L"%1"},
    {214u, L"Ativadas"},
    {215u, L"N\u00E3o habilitado"},
    {216u, L"Acesso limitado"},
    {217u, L"%1 %2"},
    {218u, L"%1"},
    {219u, L"Todos"},
    {220u, L"Servi\u00E7os"},
    {223u, L"Sempre habilitado"},
    {224u, L"Localiza\u00E7\u00E3o Padr\u00E3o"},
    {225u, L"Exibir atividade do local"},
    {228u, L"Configura\u00E7\u00F5es de proximidade"},
    {450u, L"Confirmar Desinstala\u00E7\u00E3o de Sensor"},
    {451u, L"Deseja desinstalar o sensor?"},
    {452u, L"Para finalizar a desinstala\u00E7\u00E3o do sensor, voc\u00EA deve reiniciar o computador."},
    {453u, L"Reinicializar agora"},
    {454u, L"Reiniciar depois"},
    {701u, L"Definir as configura\u00E7\u00F5es dos sensores."},
    {702u, L"Mover para cima"},
    {703u, L"Mover para baixo"},
    {704u, L"Clique para alterar as configura\u00E7\u00F5es de "},
    {801u, L"%s configura\u00E7\u00F5es"},
    {900u, L"N\u00E3o foi poss\u00EDvel ativar ou desativar um ou mais gadgets"},
    {907u, L"O Windows n\u00E3o conseguiu salvar as altera\u00E7\u00F5es no dispositivo. Verifique se o dispositivo est\u00E1 conectado ao computador e tente de novo."},
    {1000u, L"Habilitar sensores"},
    {1001u, L"Os sensores pode detectar informa\u00E7\u00F5es sobre o local atual do computador, seus arredores e mais. Depois que um sensor for habilitado, todos os programas e usu\u00E1rios poder\u00E3o acess\u00E1-lo.\n\n<A HREF=\"HELP\"> Como minha privacidade \u00E9 afetada </A>?\n\nHabilitar esses sensores:"},
    {1002u, L"Os sensores pode detectar informa\u00E7\u00F5es sobre o local atual do computador, seus arredores e mais."},
    {1003u, L"Habilitar estes sensores"},
    {1004u, L"N\u00E3o habilitar esses sensores\nVoc\u00EA pode habilitar esses sensores mais tarde em Sensor de Localiza\u00E7\u00E3o e Outros Sensores no Painel de Controle."},
    {1005u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para obter mais op\u00E7\u00F5es"},
    {1006u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para habilitar os sensores"},
    {1010u, L"Habilitar sensores"},
    {1011u, L"Os sensores pode detectar informa\u00E7\u00F5es sobre o local atual do computador, seus arredores e mais. Depois que um sensor for habilitado, todos os programas e usu\u00E1rios poder\u00E3o acess\u00E1-lo.\n\n<A HREF=\"HELP\"> Como minha privacidade \u00E9 afetada </A>?\n\nHabilitar esse sensor:"},
    {1013u, L"Habilitar este sensor"},
    {1014u, L"N\u00E3o habilitar esse sensor\nVoc\u00EA pode habilitar esse sensor mais tarde em Sensor de Localiza\u00E7\u00E3o e Outros Sensores no Painel de Controle."},
    {1015u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para obter mais op\u00E7\u00F5es"},
    {1016u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para habilitar os sensores"},
    {1030u, L"Habilitar sensores de localiza\u00E7\u00E3o"},
    {1031u, L"Os sensores de localiza\u00E7\u00E3o podem detectar o local atual de seu computador. Depois que um sensor for habilitado, todos os programas e usu\u00E1rios deste computador poder\u00E3o usar as informa\u00E7\u00F5es de local.\n\n<A HREF=\"HELP\"> Como minha privacidade \u00E9 afetada </A>?\n\nHabilitar esses sensores de localiza\u00E7\u00E3o:"},
    {1032u, L"Os sensores de localiza\u00E7\u00E3o podem detectar o local atual do computador."},
    {1033u, L"Habilitar estes sensores de localiza\u00E7\u00E3o"},
    {1034u, L"N\u00E3o habilitar esses sensores de localiza\u00E7\u00E3o\nVoc\u00EA pode habilitar esses sensores mais tarde em Sensor de Localiza\u00E7\u00E3o e Outros Sensores no Painel de Controle."},
    {1035u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para obter mais op\u00E7\u00F5es"},
    {1036u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para habilitar os sensores de localiza\u00E7\u00E3o"},
    {1040u, L"Habilitar sensores de localiza\u00E7\u00E3o"},
    {1041u, L"Os sensores de localiza\u00E7\u00E3o podem detectar o local atual de seu computador. Depois que um sensor for habilitado, todos os programas e usu\u00E1rios deste computador poder\u00E3o usar as informa\u00E7\u00F5es de local.\n\n<A HREF=\"HELP\"> Como minha privacidade \u00E9 afetada </A>?\n\nHabilitar esse sensor de localiza\u00E7\u00E3o:"},
    {1043u, L"Habilitar este sensor de localiza\u00E7\u00E3o"},
    {1044u, L"N\u00E3o habilitar esse sensor de localiza\u00E7\u00E3o\nVoc\u00EA pode habilitar esse sensor mais tarde em Sensor de Localiza\u00E7\u00E3o e Outros Sensores no Painel de Controle."},
    {1045u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para obter mais op\u00E7\u00F5es"},
    {1046u, L"Abra o Sensor de Localiza\u00E7\u00E3o e Outros Sensores para habilitar esse sensor de localiza\u00E7\u00E3o"},
    {1100u, L"Cancelar"},
    {1101u, L"OK"},
    {1102u, L"Estas configura\u00E7\u00F5es s\u00E3o gerenciadas pelo administrador do sistema."},
    {1103u, L"Por que n\u00E3o posso alterar as configura\u00E7\u00F5es?"},
    {1104u, L"Alterar configura\u00E7\u00F5es de local"},
    {1105u, L"Quando conectado como administrador, voc\u00EA pode permitir que os usu\u00E1rios controlem como os aplicativos usam sua localiza\u00E7\u00E3o e ajudar a Microsoft a aperfei\u00E7oar os servi\u00E7os de localiza\u00E7\u00E3o."},
    {1106u, L"Pol\u00EDtica de privacidade"},
    {1107u, L"Ativar a plataforma do Windows Location"},
    {1108u, L"Permita que os usu\u00E1rios escolham suas pr\u00F3prias configura\u00E7\u00F5es de localiza\u00E7\u00E3o."},
    {1109u, L"Ajude a aperfei\u00E7oar os servi\u00E7os de localiza\u00E7\u00E3o da Microsoft"},
    {1110u, L"Permita que o Windows envie periodicamente informa\u00E7\u00F5es de GPS e outras informa\u00E7\u00F5es de localiza\u00E7\u00E3o \u00E0 Microsoft quando voc\u00EA usar aplicativos com reconhecimento de local. N\u00E3o usaremos essas informa\u00E7\u00F5es para identific\u00E1-lo ou contat\u00E1-lo."},
    {1111u, L"Aplicar"},
    {2000u, L"Usado para alterar a \u00E1rea de exibi\u00E7\u00E3o vertical"},
    {2001u, L"Usado para alterar a \u00E1rea de exibi\u00E7\u00E3o horizontal"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
    {10000u, L"Sensores"},
};

static const StrEntry kStrings_de[] = {
    {1u, L"Standorteinstellungen"},
    {20u, L"Startseite"},
    {21u, L"Sensoreigenschaften"},
    {22u, L"Benutzereinstellungen"},
    {50u, L"Verbunden"},
    {51u, L"Nicht verbunden"},
    {201u, L"%1 (%2)"},
    {202u, L"Sensor"},
    {203u, L"Benutzerkonten"},
    {205u, L"Eingeschr\u00E4nkter Zugriff"},
    {207u, L"Fehler"},
    {208u, L"Zum \u00C4ndern dieser Einstellung sind Administratorrechte erforderlich."},
    {209u, L"Sie sind nicht berechtigt, diese Einstellung zu \u00E4ndern. Wenden Sie sich an den Systemadministrator, um weitere Informationen zu erhalten."},
    {210u, L"Benutzereinstellungen \u00E4ndern"},
    {211u, L"Sensoren nach Name anzeigen"},
    {212u, L"Sensoren nach Datum anzeigen"},
    {213u, L"%1"},
    {214u, L"Aktiviert"},
    {215u, L"Nicht aktiviert"},
    {216u, L"Eingeschr\u00E4nkter Zugriff"},
    {217u, L"%1 %2"},
    {218u, L"%1"},
    {219u, L"Jeder"},
    {220u, L"Dienste"},
    {223u, L"Immer aktiviert"},
    {224u, L"Aufenthaltsort"},
    {225u, L"Standortaktivit\u00E4t anzeigen"},
    {228u, L"Abstandseinstellungen"},
    {450u, L"Sensordeinstallation best\u00E4tigen"},
    {451u, L"M\u00F6chten Sie den Sensor deinstallieren?"},
    {452u, L"Zur Deinstallation des Sensors m\u00FCssen Sie den Computer neu starten."},
    {453u, L"Jetzt neu starten"},
    {454u, L"Sp\u00E4ter neu starten"},
    {701u, L"Konfigurieren Sie die Sensoreinstellungen."},
    {702u, L"Nach oben"},
    {703u, L"Nach unten"},
    {704u, L"Klicken Sie hier, um folgende Einstellungen zu \u00E4ndern: "},
    {801u, L"Einstellungen von %s"},
    {900u, L"Mindestens eines der Gadgets konnte nicht aktiviert bzw. deaktiviert werden."},
    {907u, L"Die an diesem Ger\u00E4t vorgenommenen \u00C4nderungen konnten nicht gespeichert werden. Vergewissern Sie sich, dass das Ger\u00E4t an den Computer angeschlossen ist, und wiederholen Sie den Vorgang."},
    {1000u, L"Sensoren aktivieren"},
    {1001u, L"Mithilfe von Sensoren lassen sich unter Anderem Informationen zum aktuellen Standort und zur Umgebung des Computers ermitteln. Ist ein Sensor aktiviert, steht dieser allen Programmen auf dem Computer sowie allen Benutzern des Computers zur Verf\u00FCgung.\n\n<A HREF=\"HELP\">Inwiefern beeinflusst dies die Privatsph\u00E4re?</A>\n\nFolgende Sensoren aktivieren:"},
    {1002u, L"Mithilfe von Sensoren lassen sich unter Anderem Informationen zum aktuellen Standort und zur Umgebung des Computers ermitteln."},
    {1003u, L"Diese Sensoren aktivieren"},
    {1004u, L"Diese Sensoren nicht aktivieren\nDie Sensoren k\u00F6nnen auch sp\u00E4ter in der Systemsteuerung unter \"Positions- und andere Sensoren\" aktiviert werden."},
    {1005u, L"\"Positions- und andere Sensoren\" zum Anzeigen weiterer Optionen \u00F6ffnen"},
    {1006u, L"\"Positions- und andere Sensoren\" zum Aktivieren von Sensoren \u00F6ffnen"},
    {1010u, L"Sensoren aktivieren"},
    {1011u, L"Mithilfe von Sensoren lassen sich unter Anderem Informationen zum aktuellen Standort und zur Umgebung des Computers ermitteln. Ist ein Sensor aktiviert, steht dieser allen Programmen auf dem Computer sowie allen Benutzern des Computers zur Verf\u00FCgung.\n\n<A HREF=\"HELP\">Inwiefern beeinflusst dies die Privatsph\u00E4re?</A>\n\nFolgenden Sensor aktivieren:"},
    {1013u, L"Diesen Sensor aktivieren"},
    {1014u, L"Diesen Sensor nicht aktivieren\nDer Sensor kann auch sp\u00E4ter in der Systemsteuerung unter \"Positions- und andere Sensoren\" aktiviert werden."},
    {1015u, L"\"Positions- und andere Sensoren\" zum Anzeigen weiterer Optionen \u00F6ffnen"},
    {1016u, L"\"Positions- und andere Sensoren\" zum Aktivieren von Sensoren \u00F6ffnen"},
    {1030u, L"Positionssensoren aktivieren"},
    {1031u, L"Mithilfe von Positionssensoren l\u00E4sst sich der aktuelle Standort des Computers ermitteln. Ist ein Positionssensor aktiviert, stehen die Standortinformationen s\u00E4mtlichen Programmen und Benutzern des Computers zur Verf\u00FCgung.\n\n<A HREF=\"HELP\">Inwiefern beeinflusst dies die Privatsph\u00E4re?</A>\n\nFolgende Positionssensoren aktivieren:"},
    {1032u, L"Mithilfe von Positionssensoren l\u00E4sst sich der aktuelle Standort des Computers ermitteln."},
    {1033u, L"Diese Positionssensoren aktivieren"},
    {1034u, L"Diese Positionssensoren nicht aktivieren\nDie Sensoren k\u00F6nnen auch sp\u00E4ter in der Systemsteuerung unter \"Positions- und andere Sensoren\" aktiviert werden."},
    {1035u, L"\"Positions- und andere Sensoren\" zum Anzeigen weiterer Optionen \u00F6ffnen"},
    {1036u, L"\"Positions- und andere Sensoren\" zum Aktivieren von Positionssensoren \u00F6ffnen"},
    {1040u, L"Positionssensoren aktivieren"},
    {1041u, L"Mithilfe von Positionssensoren l\u00E4sst sich der aktuelle Standort des Computers ermitteln. Ist ein Positionssensor aktiviert, stehen die Standortinformationen s\u00E4mtlichen Programmen und Benutzern des Computers zur Verf\u00FCgung.\n\n<A HREF=\"HELP\">Inwiefern beeinflusst dies die Privatsph\u00E4re?</A>\n\nFolgende Positionssensoren aktivieren:"},
    {1043u, L"Diesen Positionssensor aktivieren"},
    {1044u, L"Diesen Positionssensor nicht aktivieren\nDer Sensor kann auch sp\u00E4ter in der Systemsteuerung unter \"Positions- und andere Sensoren\" aktiviert werden."},
    {1045u, L"\"Positions- und andere Sensoren\" zum Anzeigen weiterer Optionen \u00F6ffnen"},
    {1046u, L"\"Positions- und andere Sensoren\" zum Aktivieren des Positionssensors \u00F6ffnen"},
    {1100u, L"Abbrechen"},
    {1101u, L"OK"},
    {1102u, L"Diese Einstellungen werden vom Systemadministrator verwaltet."},
    {1103u, L"Warum k\u00F6nnen Sie diese Einstellungen nicht \u00E4ndern?"},
    {1104u, L"Standorteinstellungen \u00E4ndern"},
    {1105u, L"Wenn Sie als Administrator angemeldet sind, k\u00F6nnen Sie Benutzern erlauben, selbst zu steuern, wie Apps ihre Position verwenden. Zudem k\u00F6nnen Sie Microsoft dabei unterst\u00FCtzen, die Positionsdienste zu verbessern."},
    {1106u, L"Datenschutzbestimmungen"},
    {1107u, L"Plattform f\u00FCr Windows-Position aktivieren"},
    {1108u, L"Hiermit k\u00F6nnen Benutzer ihre eigenen Standorteinstellungen ausw\u00E4hlen."},
    {1109u, L"Verbesserung des Positionsdiensts von Microsoft unterst\u00FCtzen"},
    {1110u, L"Gestatten Sie Windows, regelm\u00E4\u00DFig GPS- und andere Standortinformationen an Microsoft zu senden, wenn Sie standortbezogene Apps verwenden. Microsoft verwendet die Informationen nicht dazu, Sie zu identifizieren oder Kontakt mit Ihnen aufzunehmen."},
    {1111u, L"Anwenden"},
    {2000u, L"Wird zum \u00C4ndern des vertikalen Anzeigebereichs verwendet"},
    {2001u, L"Wird zum \u00C4ndern des horizontalen Anzeigebereichs verwendet"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertikal"},
    {10000u, L"Sensoren"},
};

struct LangTable {
    Lang lang;
    const wchar_t* code;
    const StrEntry* entries;
    size_t count;
};

static const LangTable kLangTables[] = {
    {Lang::En, L"en", kStrings_en, sizeof(kStrings_en) / sizeof(kStrings_en[0])},
    {Lang::It, L"it", kStrings_it, sizeof(kStrings_it) / sizeof(kStrings_it[0])},
    {Lang::Fr, L"fr", kStrings_fr, sizeof(kStrings_fr) / sizeof(kStrings_fr[0])},
    {Lang::Es, L"es", kStrings_es, sizeof(kStrings_es) / sizeof(kStrings_es[0])},
    {Lang::Pt, L"pt", kStrings_pt, sizeof(kStrings_pt) / sizeof(kStrings_pt[0])},
    {Lang::De, L"de", kStrings_de, sizeof(kStrings_de) / sizeof(kStrings_de[0])},
};

static const LangTable& GetLangTable(Lang lang) {
    const int idx = static_cast<int>(lang);
    if (idx >= 0 && idx < static_cast<int>(sizeof(kLangTables) / sizeof(kLangTables[0]))) {
        return kLangTables[idx];
    }
    return kLangTables[0];
}

static std::wstring LookupString(Lang lang, uint16_t id) {
    const LangTable& t = GetLangTable(lang);
    for (size_t i = 0; i < t.count; ++i) {
        if (t.entries[i].id == id) return t.entries[i].text;
    }
    const LangTable& en = kLangTables[0];
    for (size_t i = 0; i < en.count; ++i) {
        if (en.entries[i].id == id) return en.entries[i].text;
    }
    return L"";
}

// ============================================================================
// 2. Portable SHA-256 (self-contained, no external crypto runtime required)
// ============================================================================

namespace sha256 {

static inline uint32_t Rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32u - n));
}

static std::string Hex(const uint8_t* data, size_t len) {
    static const uint32_t k[64] = {
        0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
        0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
        0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
        0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
        0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
        0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
        0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
        0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u,
    };
    uint32_t h[8] = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u,
    };
    const uint64_t bitLen = static_cast<uint64_t>(len) * 8ull;
    const size_t padLen = ((len + 9u + 63u) / 64u) * 64u;
    Bytes buf(padLen, 0);
    if (len) std::memcpy(buf.data(), data, len);
    buf[len] = 0x80u;
    for (int i = 0; i < 8; ++i) {
        buf[padLen - 1 - i] = static_cast<uint8_t>((bitLen >> (i * 8)) & 0xFFu);
    }
    for (size_t off = 0; off < padLen; off += 64) {
        uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            const uint8_t* p = buf.data() + off + i * 4;
            w[i] = (static_cast<uint32_t>(p[0]) << 24) | (static_cast<uint32_t>(p[1]) << 16) |
                   (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
        }
        for (int i = 16; i < 64; ++i) {
            const uint32_t s0 = Rotr(w[i - 15], 7) ^ Rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            const uint32_t s1 = Rotr(w[i - 2], 17) ^ Rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
        for (int i = 0; i < 64; ++i) {
            const uint32_t S1 = Rotr(e, 6) ^ Rotr(e, 11) ^ Rotr(e, 25);
            const uint32_t ch = (e & f) ^ ((~e) & g);
            const uint32_t t1 = hh + S1 + ch + k[i] + w[i];
            const uint32_t S0 = Rotr(a, 2) ^ Rotr(a, 13) ^ Rotr(a, 22);
            const uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            const uint32_t t2 = S0 + maj;
            hh = g; g = f; f = e; e = d + t1;
            d = c; c = b; b = a; a = t1 + t2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }
    static const char hexDigits[] = "0123456789abcdef";
    std::string out(64, '0');
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            out[i * 8 + j] = hexDigits[(h[i] >> (28 - j * 4)) & 0xFu];
        }
    }
    return out;
}

static std::string Hex(const Bytes& b) {
    return Hex(b.data(), b.size());
}

}  // namespace sha256

// ============================================================================
// 3. PE & Resource Builder (portable C++ -- testable standalone via -DLOC_GENERATOR_ONLY)
// ============================================================================

namespace gen {

static uint16_t Rd16(const Bytes& b, size_t off) {
    return static_cast<uint16_t>(b[off]) | (static_cast<uint16_t>(b[off + 1]) << 8);
}
static uint32_t Rd32(const Bytes& b, size_t off) {
    return static_cast<uint32_t>(b[off]) | (static_cast<uint32_t>(b[off + 1]) << 8) |
           (static_cast<uint32_t>(b[off + 2]) << 16) | (static_cast<uint32_t>(b[off + 3]) << 24);
}
static void Wr16(Bytes& b, size_t off, uint16_t v) {
    b[off] = static_cast<uint8_t>(v & 0xFFu);
    b[off + 1] = static_cast<uint8_t>((v >> 8) & 0xFFu);
}
static void Wr32(Bytes& b, size_t off, uint32_t v) {
    b[off] = static_cast<uint8_t>(v & 0xFFu);
    b[off + 1] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    b[off + 2] = static_cast<uint8_t>((v >> 16) & 0xFFu);
    b[off + 3] = static_cast<uint8_t>((v >> 24) & 0xFFu);
}
static void Append16(Bytes& b, uint16_t v) {
    b.push_back(static_cast<uint8_t>(v & 0xFFu));
    b.push_back(static_cast<uint8_t>((v >> 8) & 0xFFu));
}
static void Append32(Bytes& b, uint32_t v) {
    b.push_back(static_cast<uint8_t>(v & 0xFFu));
    b.push_back(static_cast<uint8_t>((v >> 8) & 0xFFu));
    b.push_back(static_cast<uint8_t>((v >> 16) & 0xFFu));
    b.push_back(static_cast<uint8_t>((v >> 24) & 0xFFu));
}
static uint32_t AlignUp(uint32_t v, uint32_t a) {
    return a ? ((v + a - 1u) / a) * a : v;
}

static std::string WideToUtf8(const std::wstring& ws) {
    std::string out;
    out.reserve(ws.size() * 2);
    for (size_t i = 0; i < ws.size(); ++i) {
        uint32_t cp = static_cast<uint32_t>(ws[i]);
        if (cp >= 0xD800u && cp <= 0xDBFFu && i + 1 < ws.size()) {
            const uint32_t lo = static_cast<uint32_t>(ws[i + 1]);
            if (lo >= 0xDC00u && lo <= 0xDFFFu) {
                cp = 0x10000u + (((cp - 0xD800u) << 10) | (lo - 0xDC00u));
                ++i;
            }
        }
        if (cp < 0x80u) {
            out.push_back(static_cast<char>(cp));
        } else if (cp < 0x800u) {
            out.push_back(static_cast<char>(0xC0u | (cp >> 6)));
            out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
        } else if (cp < 0x10000u) {
            out.push_back(static_cast<char>(0xE0u | (cp >> 12)));
            out.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
            out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
        } else {
            out.push_back(static_cast<char>(0xF0u | (cp >> 18)));
            out.push_back(static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu)));
            out.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
            out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
        }
    }
    return out;
}

static std::string XmlEscapeUtf8(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 16);
    for (char c : s) {
        switch (c) {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default: out.push_back(c); break;
        }
    }
    return out;
}

static std::wstring XmlEscapeWide(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size() + 16);
    for (wchar_t c : s) {
        switch (c) {
            case L'&': out += L"&amp;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            case L'"': out += L"&quot;"; break;
            case L'\'': out += L"&apos;"; break;
            default: out.push_back(c); break;
        }
    }
    return out;
}

struct PeSection {
    uint32_t vsize;
    uint32_t vrva;
    uint32_t rsize;
    uint32_t rptr;
};

struct PeLayout {
    uint32_t peOff = 0;
    uint16_t numSecs = 0;
    uint16_t optSize = 0;
    uint32_t optOff = 0;
    uint32_t ddOff = 0;
    uint32_t secOff = 0;
    uint32_t secAlign = 0;
    uint32_t fileAlign = 0;
    uint32_t sizeOfHeaders = 0;
    std::vector<PeSection> secs;

    bool Parse(const Bytes& b) {
        if (b.size() < 0x100 || b[0] != 'M' || b[1] != 'Z') return false;
        peOff = Rd32(b, 0x3C);
        if (peOff + 0x108 > b.size()) return false;
        if (b[peOff] != 'P' || b[peOff + 1] != 'E' || b[peOff + 2] != 0 || b[peOff + 3] != 0) return false;
        numSecs = Rd16(b, peOff + 6);
        optSize = Rd16(b, peOff + 20);
        optOff = peOff + 24;
        if (optOff + optSize > b.size()) return false;
        const uint16_t magic = Rd16(b, optOff);
        if (magic != 0x20Bu) return false;  // PE32+ (x64) only
        secAlign = Rd32(b, optOff + 32);
        fileAlign = Rd32(b, optOff + 36);
        sizeOfHeaders = Rd32(b, optOff + 60);
        ddOff = optOff + 112;
        secOff = optOff + optSize;
        if (secOff + static_cast<size_t>(numSecs) * 40u > b.size()) return false;
        secs.clear();
        secs.reserve(numSecs);
        for (uint16_t i = 0; i < numSecs; ++i) {
            const size_t o = secOff + static_cast<size_t>(i) * 40u;
            secs.push_back({Rd32(b, o + 8), Rd32(b, o + 12), Rd32(b, o + 16), Rd32(b, o + 20)});
        }
        return true;
    }

    bool RvaToOff(uint32_t rva, size_t& out) const {
        for (const PeSection& s : secs) {
            const uint32_t span = std::max(s.vsize, s.rsize);
            if (rva >= s.vrva && rva < s.vrva + span) {
                const uint32_t delta = rva - s.vrva;
                if (delta < s.rsize) {
                    out = static_cast<size_t>(s.rptr) + delta;
                    return true;
                }
                return false;
            }
        }
        return false;
    }
};

struct RsrcKey {
    bool isStr = false;
    uint32_t id = 0;
    std::wstring name;
    bool operator<(const RsrcKey& o) const {
        if (isStr != o.isStr) return isStr > o.isStr;
        return isStr ? (name < o.name) : (id < o.id);
    }
};

struct RsrcItem {
    RsrcKey type;
    RsrcKey name;
    uint32_t lang = 0;
    Bytes data;
};

static bool ExtractResources(const Bytes& b, const PeLayout& pe, std::vector<RsrcItem>& out) {
    const uint32_t rsrcRva = Rd32(b, pe.ddOff + 2 * 8);
    const uint32_t rsrcSize = Rd32(b, pe.ddOff + 2 * 8 + 4);
    if (!rsrcRva || !rsrcSize) return false;
    size_t base = 0;
    if (!pe.RvaToOff(rsrcRva, base)) return false;

    auto readKey = [&](uint32_t raw, RsrcKey& k) -> bool {
        if (raw & 0x80000000u) {
            const size_t noff = base + (raw & 0x7FFFFFFFu);
            if (noff + 2 > b.size()) return false;
            const uint16_t len = Rd16(b, noff);
            if (noff + 2 + static_cast<size_t>(len) * 2u > b.size()) return false;
            k.isStr = true;
            k.id = 0;
            k.name.resize(len);
            for (uint16_t i = 0; i < len; ++i) {
                k.name[i] = static_cast<wchar_t>(Rd16(b, noff + 2 + static_cast<size_t>(i) * 2u));
            }
        } else {
            k.isStr = false;
            k.id = raw;
            k.name.clear();
        }
        return true;
    };

    auto walk = [&](auto&& self, uint32_t dirOff, int depth, RsrcKey curType, RsrcKey curName) -> bool {
        const size_t off = base + dirOff;
        if (off + 16 > b.size()) return false;
        const uint16_t nNamed = Rd16(b, off + 12);
        const uint16_t nId = Rd16(b, off + 14);
        const size_t total = static_cast<size_t>(nNamed) + nId;
        if (off + 16 + total * 8u > b.size()) return false;
        for (size_t i = 0; i < total; ++i) {
            const uint32_t rawId = Rd32(b, off + 16 + i * 8u);
            const uint32_t sub = Rd32(b, off + 16 + i * 8u + 4);
            RsrcKey k;
            if (!readKey(rawId, k)) return false;
            if (sub & 0x80000000u) {
                if (depth == 0) {
                    if (!self(self, sub & 0x7FFFFFFFu, 1, k, {})) return false;
                } else if (depth == 1) {
                    if (!self(self, sub & 0x7FFFFFFFu, 2, curType, k)) return false;
                }
            } else if (depth == 2) {
                const size_t doff = base + sub;
                if (doff + 16 > b.size()) return false;
                const uint32_t dataRva = Rd32(b, doff);
                const uint32_t dataSz = Rd32(b, doff + 4);
                size_t fileOff = 0;
                if (!pe.RvaToOff(dataRva, fileOff) || fileOff + dataSz > b.size()) return false;
                RsrcItem item;
                item.type = curType;
                item.name = curName;
                item.lang = k.id;
                item.data.assign(b.begin() + static_cast<ptrdiff_t>(fileOff),
                                 b.begin() + static_cast<ptrdiff_t>(fileOff + dataSz));
                out.push_back(std::move(item));
            }
        }
        return true;
    };
    return walk(walk, 0, 0, {}, {});
}

static Bytes BuildResourceSection(const std::vector<RsrcItem>& items, uint32_t baseRva) {
    using LangMap = std::map<uint32_t, const Bytes*>;
    using NameMap = std::map<RsrcKey, LangMap>;
    using TypeMap = std::map<RsrcKey, NameMap>;
    TypeMap tree;
    for (const RsrcItem& it : items) {
        tree[it.type][it.name][it.lang] = &it.data;
    }

    auto countEntries = [](const auto& m) -> std::pair<uint16_t, uint16_t> {
        uint16_t nn = 0, ni = 0;
        for (const auto& kv : m) {
            if (kv.first.isStr) ++nn;
            else ++ni;
        }
        return {nn, ni};
    };

    uint32_t dirPos = static_cast<uint32_t>(16u + tree.size() * 8u);
    std::map<RsrcKey, uint32_t> typeDirOff;
    std::map<std::pair<RsrcKey, RsrcKey>, uint32_t> nameDirOff;
    for (const auto& tkv : tree) {
        typeDirOff[tkv.first] = dirPos;
        dirPos += static_cast<uint32_t>(16u + tkv.second.size() * 8u);
        for (const auto& nkv : tkv.second) {
            nameDirOff[{tkv.first, nkv.first}] = dirPos;
            dirPos += static_cast<uint32_t>(16u + nkv.second.size() * 8u);
        }
    }

    std::map<std::wstring, uint32_t> strOff;
    uint32_t strPos = dirPos;
    auto regStr = [&](const RsrcKey& k) {
        if (k.isStr && strOff.find(k.name) == strOff.end()) {
            strOff[k.name] = strPos;
            strPos += static_cast<uint32_t>(2u + k.name.size() * 2u);
        }
    };
    for (const auto& tkv : tree) {
        regStr(tkv.first);
        for (const auto& nkv : tkv.second) regStr(nkv.first);
    }
    strPos = AlignUp(strPos, 4u);

    size_t leafCount = 0;
    for (const auto& tkv : tree)
        for (const auto& nkv : tkv.second) leafCount += nkv.second.size();
    const uint32_t dataDescStart = strPos;
    uint32_t payloadPos = AlignUp(dataDescStart + static_cast<uint32_t>(leafCount * 16u), 4u);

    struct LeafInfo {
        uint32_t descOff;
        uint32_t dataOff;
        const Bytes* data;
    };
    std::vector<LeafInfo> leaves;
    leaves.reserve(leafCount);

    Bytes out;
    out.reserve(payloadPos + 131072);

    auto writeDirHeader = [&](uint16_t nNamed, uint16_t nId) {
        Append32(out, 0);
        Append32(out, 0);
        Append16(out, 0);
        Append16(out, 0);
        Append16(out, nNamed);
        Append16(out, nId);
    };

    {
        auto [nn, ni] = countEntries(tree);
        writeDirHeader(nn, ni);
        for (const auto& tkv : tree) {
            const uint32_t idField = tkv.first.isStr ? (0x80000000u | strOff[tkv.first.name]) : tkv.first.id;
            Append32(out, idField);
            Append32(out, 0x80000000u | typeDirOff[tkv.first]);
        }
    }
    uint32_t curDesc = dataDescStart;
    for (const auto& tkv : tree) {
        auto [nn, ni] = countEntries(tkv.second);
        writeDirHeader(nn, ni);
        for (const auto& nkv : tkv.second) {
            const uint32_t idField = nkv.first.isStr ? (0x80000000u | strOff[nkv.first.name]) : nkv.first.id;
            Append32(out, idField);
            Append32(out, 0x80000000u | nameDirOff[{tkv.first, nkv.first}]);
        }
        for (const auto& nkv : tkv.second) {
            writeDirHeader(0, static_cast<uint16_t>(nkv.second.size()));
            for (const auto& lkv : nkv.second) {
                Append32(out, lkv.first);
                Append32(out, curDesc);
                leaves.push_back({curDesc, payloadPos, lkv.second});
                curDesc += 16u;
                payloadPos = AlignUp(payloadPos + static_cast<uint32_t>(lkv.second->size()), 4u);
            }
        }
    }

    std::vector<std::pair<uint32_t, std::wstring>> orderedNames;
    orderedNames.reserve(strOff.size());
    for (const auto& kv : strOff) orderedNames.push_back({kv.second, kv.first});
    std::sort(orderedNames.begin(), orderedNames.end());
    for (const auto& p : orderedNames) {
        while (out.size() < p.first) out.push_back(0);
        Append16(out, static_cast<uint16_t>(p.second.size()));
        for (wchar_t c : p.second) Append16(out, static_cast<uint16_t>(c));
    }
    while (out.size() < dataDescStart) out.push_back(0);

    for (const LeafInfo& lf : leaves) {
        Append32(out, baseRva + lf.dataOff);
        Append32(out, static_cast<uint32_t>(lf.data->size()));
        Append32(out, 0);
        Append32(out, 0);
    }

    for (const LeafInfo& lf : leaves) {
        while (out.size() < lf.dataOff) out.push_back(0);
        out.insert(out.end(), lf.data->begin(), lf.data->end());
    }
    while (out.size() % 4u != 0) out.push_back(0);
    return out;
}

static uint32_t ComputePeChecksum(const Bytes& peBytes, size_t checksumFieldOff) {
    uint64_t sum = 0;
    const size_t n = peBytes.size();
    for (size_t i = 0; i < n; i += 2) {
        if (i == checksumFieldOff || i == checksumFieldOff + 2) continue;
        uint16_t word = static_cast<uint16_t>(peBytes[i]);
        if (i + 1 < n) word |= static_cast<uint16_t>(peBytes[i + 1]) << 8;
        sum += word;
        sum = (sum & 0xFFFFu) + (sum >> 16);
    }
    sum = (sum & 0xFFFFu) + (sum >> 16);
    return static_cast<uint32_t>(sum + n);
}

// Patch CSensorsLicense::GetPermission (RVA 0xBAD0 / file off 0xAED0) -> xor eax, eax; ret (S_OK)
static bool PatchSensorsLicenseCheck(Bytes& peBytes, const PeLayout& pe, std::string& err) {
    size_t off = 0;
    if (!pe.RvaToOff(0xBAD0u, off) || off + 16u > peBytes.size()) {
        err = "unable to locate CSensorsLicense::GetPermission at RVA 0xBAD0";
        return false;
    }
    static const uint8_t kExpected[16] = {
        0x48, 0x8B, 0xC4, 0x48, 0x89, 0x58, 0x08, 0x48,
        0x89, 0x70, 0x10, 0x48, 0x89, 0x78, 0x18, 0x55,
    };
    if (std::memcmp(peBytes.data() + off, kExpected, sizeof(kExpected)) != 0) {
        err = "CSensorsLicense::GetPermission prologue mismatch";
        return false;
    }
    peBytes[off + 0] = 0x31u;  // xor eax, eax
    peBytes[off + 1] = 0xC0u;
    peBytes[off + 2] = 0xC3u;  // ret (S_OK)
    return true;
}

static bool PatchImportDui70ToS8dui(Bytes& peBytes, const PeLayout& pe, std::string& err) {
    const uint32_t impRva = Rd32(peBytes, pe.ddOff + 1 * 8);
    if (!impRva) {
        err = "missing Import Directory";
        return false;
    }
    size_t impOff = 0;
    if (!pe.RvaToOff(impRva, impOff)) {
        err = "unmappable Import Directory RVA";
        return false;
    }
    int patched = 0;
    for (size_t off = impOff; off + 20 <= peBytes.size(); off += 20) {
        const uint32_t oft = Rd32(peBytes, off);
        const uint32_t nameRva = Rd32(peBytes, off + 12);
        const uint32_t ft = Rd32(peBytes, off + 16);
        if (!oft && !nameRva && !ft) break;
        size_t nameOff = 0;
        if (!pe.RvaToOff(nameRva, nameOff) || nameOff + 10 > peBytes.size()) continue;
        std::string dllName;
        for (size_t k = nameOff; k < peBytes.size() && peBytes[k] != 0 && dllName.size() < 64; ++k) {
            dllName.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(peBytes[k]))));
        }
        if (dllName == "dui70.dll") {
            static const char kRepl[10] = "s8dui.dll";
            std::memcpy(peBytes.data() + nameOff, kRepl, 10);
            ++patched;
        }
    }
    if (patched != 1) {
        err = "expected 1 DUI70.dll import descriptor, found " + std::to_string(patched);
        return false;
    }
    return true;
}

static bool ExtractCommonSheet(const Bytes& duserRaw, std::string& outSheet, std::string& err) {
    PeLayout dpe;
    if (!dpe.Parse(duserRaw)) {
        err = "failed to parse duser.dll PE";
        return false;
    }
    std::vector<RsrcItem> items;
    if (!ExtractResources(duserRaw, dpe, items)) {
        err = "failed to extract duser.dll resources";
        return false;
    }
    std::string raw;
    for (const RsrcItem& it : items) {
        if (it.type.isStr && it.type.name == L"UIFILE" && !it.name.isStr && it.name.id == 1010) {
            raw.assign(reinterpret_cast<const char*>(it.data.data()), it.data.size());
            break;
        }
    }
    if (raw.empty()) {
        err = "duser.dll UIFILE/1010 not found";
        return false;
    }
    const size_t s0 = raw.find("<style resid=\"common\">");
    const size_t s1 = (s0 != std::string::npos) ? raw.find("</style>", s0) : std::string::npos;
    if (s0 == std::string::npos || s1 == std::string::npos) {
        err = "common style block not found in UIFILE/1010";
        return false;
    }
    std::string block = raw.substr(s0, (s1 + 8) - s0);
    // Remap duser.dll.mui strings resstr(1100..1103) -> resstr(2000..2003)
    // so they never collide with SensorsCpl.dll's own strings 1100..1111.
    for (int i = 0; i < 4; ++i) {
        const std::string from = "resstr(" + std::to_string(1100 + i) + ")";
        const std::string to = "resstr(" + std::to_string(2000 + i) + ")";
        size_t pos = 0;
        while ((pos = block.find(from, pos)) != std::string::npos) {
            block.replace(pos, from.size(), to);
            pos += to.size();
        }
    }
    outSheet = std::move(block);
    return true;
}

static void PatchXml100(Bytes& data, Lang lang) {
    std::string xml(reinterpret_cast<const char*>(data.data()), data.size());
    // Replace displayname="@sensorscpl.dll,-<id>" -> literal localized UTF-8 string
    // so shdocvw.dll never tries to read Win10/11's 12 KB stub System32\sensorscpl.dll.
    const std::string dispPrefix = "displayname=\"@sensorscpl.dll,-";
    size_t pos = 0;
    while ((pos = xml.find(dispPrefix, pos)) != std::string::npos) {
        const size_t numStart = pos + dispPrefix.size();
        const size_t quoteEnd = xml.find('"', numStart);
        if (quoteEnd == std::string::npos) break;
        const std::string numStr = xml.substr(numStart, quoteEnd - numStart);
        uint32_t sid = 0;
        bool valid = !numStr.empty();
        for (char c : numStr) {
            if (c < '0' || c > '9') { valid = false; break; }
            sid = sid * 10u + static_cast<uint32_t>(c - '0');
        }
        if (valid && sid <= 65535u) {
            const std::wstring wtxt = LookupString(lang, static_cast<uint16_t>(sid));
            const std::string repl = "displayname=\"" + XmlEscapeUtf8(WideToUtf8(wtxt)) + "\"";
            xml.replace(pos, (quoteEnd + 1) - pos, repl);
            pos += repl.size();
        } else {
            pos = quoteEnd + 1;
        }
    }
    data.assign(xml.begin(), xml.end());
}

static void BuildStringTableResources(Lang lang, std::vector<RsrcItem>& outItems) {
    const LangTable& t = GetLangTable(lang);
    std::map<uint32_t, std::array<std::wstring, 16>> blocks;
    for (size_t i = 0; i < t.count; ++i) {
        const uint32_t blockId = static_cast<uint32_t>(t.entries[i].id / 16u) + 1u;
        const size_t slot = t.entries[i].id % 16u;
        blocks[blockId][slot] = t.entries[i].text;
    }
    for (const auto& kv : blocks) {
        RsrcItem item;
        item.type = {false, 6u, L""};          // RT_STRING
        item.name = {false, kv.first, L""};    // 1-based block ID
        item.lang = 0u;                        // Language-neutral so LoadStringW / DirectUI always matches
        for (size_t slot = 0; slot < 16; ++slot) {
            const std::wstring& s = kv.second[slot];
            Append16(item.data, static_cast<uint16_t>(s.size()));
            for (wchar_t ch : s) Append16(item.data, static_cast<uint16_t>(ch));
        }
        outItems.push_back(std::move(item));
    }
}

static bool PatchUifile110(Bytes& data, const std::string& commonSheet, std::string& err) {
    std::string ui(reinterpret_cast<const char*>(data.data()), data.size());

    // 1. Inline <style resid="common"> from Win7 duser.dll so we never touch System32\duser.dll.
    const size_t closePos = ui.find("</stylesheets>");
    if (closePos == std::string::npos || ui.find("resid=\"common\"") != std::string::npos) {
        err = "UIFILE/110 missing </stylesheets> or already contains common stylesheet";
        return false;
    }
    ui.insert(closePos, "\r\n" + commonSheet + "\r\n");

    // 2. Fix Windows 8.1 SensorsCpl.dll UIFILE/110 Group Policy banner layout defect:
    //    In Win8.1, groupPolicyBox and wss_hub_content were placed as siblings inside
    //    <element layout="flowlayout()" class="cp_hub_frame">, so in wide/maximized Control Panel
    //    windows the yellow Group Policy banner renders side-by-side to the LEFT of the content
    //    instead of stacked directly above the heading. Moving groupPolicyBox inside wss_hub_content
    //    (class="cp_content_pane", width="600rp", layout="borderlayout()") ensures it always docks
    //    cleanly across the full 580rp content width directly above "Change location settings" when
    //    Group Policy is active, and collapses to 0x0 with exact authentic Win8.1 margins when inactive.
    static const char kOrigGpBlock[] =
        "<element layout=\"flowlayout()\" layoutpos=\"client\" sheet=\"cp_style\" class=\"cp_hub_frame\">\r\n"
        "<element id=\"atom(groupPolicyBox)\" layoutpos=\"none\" layout=\"borderlayout()\" class=\"cp_content_banner_box\">\r\n"
        "<element layoutpos=\"left\" content=\"icon(81,sysmetric(49),sysmetric(50),library(imageres.dll))\" contentalign=\"topleft\"/>\r\n"
        "<element layout=\"verticalflowlayout(1,0,0,0)\" padding=\"rect(6rp,0,0rp,0)\">\r\n"
        "<element class=\"cp_content_text\" content=\"resstr(1102)\" padding=\"rect(0rp,0,6rp,0)\"/>\r\n"
        "<button id=\"atom(helpGP)\" layoutpos=\"left\" class=\"cp_content_link\" content=\"resstr(1103)\"/>\r\n"
        "</element>\r\n"
        "</element>\r\n"
        "<element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
        "<element id=\"atom(wss_hub_content)\" class=\"cp_content_pane\" layoutpos=\"top\" layout=\"borderlayout()\" background=\"argb(0,0,0,0)\" accessible=\"false\">\r\n";

    static const char kFixedGpBlock[] =
        "<element layout=\"flowlayout()\" layoutpos=\"client\" sheet=\"cp_style\" class=\"cp_hub_frame\">\r\n"
        "<element id=\"atom(wss_hub_content)\" class=\"cp_content_pane\" layoutpos=\"top\" layout=\"borderlayout()\" background=\"argb(0,0,0,0)\" accessible=\"false\">\r\n"
        "<element id=\"atom(groupPolicyBox)\" layoutpos=\"none\" layout=\"borderlayout()\" class=\"cp_content_banner_box\" margin=\"rect(0rp,0rp,0rp,12rp)\">\r\n"
        "<element layoutpos=\"left\" content=\"icon(81,sysmetric(49),sysmetric(50),library(imageres.dll))\" contentalign=\"topleft\"/>\r\n"
        "<element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(6rp,0rp,0rp,0rp)\">\r\n"
        "<element layoutpos=\"top\" class=\"cp_content_text\" content=\"resstr(1102)\" padding=\"rect(0rp,0rp,6rp,2rp)\"/>\r\n"
        "<element layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
        "<button id=\"atom(helpGP)\" layoutpos=\"left\" class=\"cp_content_link\" content=\"resstr(1103)\"/>\r\n"
        "</element>\r\n"
        "</element>\r\n"
        "</element>\r\n";

    const size_t gpPos = ui.find(kOrigGpBlock);
    if (gpPos == std::string::npos) {
        err = "UIFILE/110 Group Policy layout block not found";
        return false;
    }
    ui.replace(gpPos, sizeof(kOrigGpBlock) - 1u, kFixedGpBlock);

    data.assign(ui.begin(), ui.end());
    return true;
}

static bool BuildPatchedSensorsCpl(const Bytes& rawSensorsCpl, const Bytes& rawDuser, Lang lang,
                                   Bytes& outDll, std::string& err) {
    Bytes peBytes = rawSensorsCpl;
    PeLayout pe;
    if (!pe.Parse(peBytes)) {
        err = "failed to parse sensorscpl.dll PE";
        return false;
    }

    std::string commonSheet;
    if (!ExtractCommonSheet(rawDuser, commonSheet, err)) return false;

    if (!PatchSensorsLicenseCheck(peBytes, pe, err)) return false;
    if (!PatchImportDui70ToS8dui(peBytes, pe, err)) return false;

    std::vector<RsrcItem> origItems;
    if (!ExtractResources(peBytes, pe, origItems)) {
        err = "failed to extract sensorscpl.dll resources";
        return false;
    }

    std::vector<RsrcItem> newItems;
    newItems.reserve(origItems.size() + 32);
    int xmlPatched = 0;
    int uiPatched = 0;
    for (RsrcItem& it : origItems) {
        if (it.type.isStr && it.type.name == L"MUI") continue;
        if (!it.type.isStr && it.type.id == 6u) continue;
        if (it.type.isStr && it.type.name == L"XMLFILE" && !it.name.isStr && it.name.id == 100u) {
            PatchXml100(it.data, lang);
            it.lang = 0u;
            ++xmlPatched;
        } else if (it.type.isStr && it.type.name == L"UIFILE") {
            if (!PatchUifile110(it.data, commonSheet, err)) return false;
            it.lang = 0u;
            ++uiPatched;
        }
        newItems.push_back(std::move(it));
    }
    if (xmlPatched != 1 || uiPatched != 1) {
        err = "unexpected resource patch counts: xml=" + std::to_string(xmlPatched) +
              " ui=" + std::to_string(uiPatched);
        return false;
    }

    BuildStringTableResources(lang, newItems);

    const size_t newSecHdrOff = pe.secOff + static_cast<size_t>(pe.numSecs) * 40u;
    uint32_t firstRaw = pe.sizeOfHeaders;
    uint32_t maxVEnd = 0;
    for (const PeSection& s : pe.secs) {
        if (s.rptr && s.rptr < firstRaw) firstRaw = s.rptr;
        const uint32_t vend = AlignUp(s.vrva + std::max(s.vsize, s.rsize), pe.secAlign);
        if (vend > maxVEnd) maxVEnd = vend;
    }
    if (newSecHdrOff + 40u > firstRaw) {
        err = "insufficient PE header slack for .rsrc8 section";
        return false;
    }

    // Zero BoundImport (DD[11]), Security (DD[4]), and stale header slack.
    Wr32(peBytes, pe.ddOff + 11 * 8, 0);
    Wr32(peBytes, pe.ddOff + 11 * 8 + 4, 0);
    Wr32(peBytes, pe.ddOff + 4 * 8, 0);
    Wr32(peBytes, pe.ddOff + 4 * 8 + 4, 0);
    std::memset(peBytes.data() + newSecHdrOff, 0, firstRaw - newSecHdrOff);

    const uint32_t newRawOff = AlignUp(static_cast<uint32_t>(peBytes.size()), pe.fileAlign);
    peBytes.resize(newRawOff, 0);
    const Bytes rsrcBlob = BuildResourceSection(newItems, maxVEnd);
    const uint32_t rsrcVSize = static_cast<uint32_t>(rsrcBlob.size());
    const uint32_t rsrcRSize = AlignUp(rsrcVSize, pe.fileAlign);
    peBytes.insert(peBytes.end(), rsrcBlob.begin(), rsrcBlob.end());
    peBytes.resize(static_cast<size_t>(newRawOff) + rsrcRSize, 0);

    static const char kSecName[8] = {'.', 'r', 's', 'r', 'c', '8', 0, 0};
    std::memcpy(peBytes.data() + newSecHdrOff, kSecName, 8);
    Wr32(peBytes, newSecHdrOff + 8, rsrcVSize);
    Wr32(peBytes, newSecHdrOff + 12, maxVEnd);
    Wr32(peBytes, newSecHdrOff + 16, rsrcRSize);
    Wr32(peBytes, newSecHdrOff + 20, newRawOff);
    Wr32(peBytes, newSecHdrOff + 36, 0x40000040u);  // IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ

    Wr16(peBytes, pe.peOff + 6, static_cast<uint16_t>(pe.numSecs + 1u));
    Wr32(peBytes, pe.optOff + 56, AlignUp(maxVEnd + rsrcVSize, pe.secAlign));
    Wr32(peBytes, pe.ddOff + 2 * 8, maxVEnd);
    Wr32(peBytes, pe.ddOff + 2 * 8 + 4, rsrcVSize);

    const size_t csumOff = pe.optOff + 64;
    Wr32(peBytes, csumOff, 0);
    Wr32(peBytes, csumOff, ComputePeChecksum(peBytes, csumOff));

    outDll = std::move(peBytes);
    return true;
}

static Bytes BuildTasksXmlUtf16(Lang lang) {
    const std::wstring tChange = XmlEscapeWide(LookupString(lang, 1104u));
    const std::wstring xml =
        L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n"
        L"<applications xmlns=\"http://schemas.microsoft.com/windows/cpltasks/v1\" "
        L"xmlns:sh=\"http://schemas.microsoft.com/windows/tasks/v1\">\r\n"
        L"  <application id=\"{E9950154-C418-419e-A90A-20C5287AE24B}\">\r\n"
        L"    <sh:task id=\"{B54A4F85-E59B-42E4-AE66-281D06D04C3B}\">\r\n"
        L"      <sh:name>" + tChange + L"</sh:name>\r\n"
        L"      <sh:keywords>location;sensors;gps;posizione;sensori;localisation;capteurs;ubicaci\u00F3n;sensores;local;standort;sensoren</sh:keywords>\r\n"
        L"      <sh:command>explorer.exe shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\2\\::{E9950154-C418-419e-A90A-20C5287AE24B}</sh:command>\r\n"
        L"    </sh:task>\r\n"
        L"    <category id=\"2\">\r\n"
        L"      <sh:task idref=\"{B54A4F85-E59B-42E4-AE66-281D06D04C3B}\"/>\r\n"
        L"    </category>\r\n"
        L"  </application>\r\n"
        L"</applications>\r\n";

    Bytes out;
    out.reserve(2 + xml.size() * 2);
    out.push_back(0xFFu);
    out.push_back(0xFEu);
    for (wchar_t c : xml) {
        out.push_back(static_cast<uint8_t>(c & 0xFFu));
        out.push_back(static_cast<uint8_t>((c >> 8) & 0xFFu));
    }
    return out;
}

}  // namespace gen

#ifndef LOC_GENERATOR_ONLY

// ============================================================================
// 4. Pinned Official Microsoft Symbol Server Files (Windows 8.1 / Win7 x64)
// ============================================================================

struct FileSpec {
    const wchar_t* name;
    const wchar_t* msdlPath;
    size_t size;
    const char* sha256;
};

static const FileSpec kFiles[] = {
    {
        L"sensorscpl.dll",
        L"sensorscpl.dll/54504BB220e000/sensorscpl.dll",
        2134528u,
        "565bf8c1261aa582518dd9318a2f5d81395fad73aab3b75e330e0fe03218c357",
    },
    {
        L"dui70.dll",
        L"dui70.dll/54503E181c1000/dui70.dll",
        1822720u,
        "a8190828346071074ae1c00a4c8a817d386c94b20fc4adf69e825378f68df894",
    },
    {
        L"duser.dll",
        L"duser.dll/4A5BDF2643000/duser.dll",
        260608u,
        "dd663029b2eb7b12fdb00fce403d8326141e540e3b9ce84cd5871473d3e2e2cf",
    },
};

// Official Windows 8.1 Location Settings COM & Shell GUIDs
static const wchar_t* kAppletClsid          = L"{E9950154-C418-419e-A90A-20C5287AE24B}";
static const wchar_t* kLayoutFolderClsid    = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";
static const wchar_t* kFrameProviderClsid   = L"{E4E54B1D-1AC1-405c-B62A-E11CD87F2261}";
static const wchar_t* kInitializerClsid     = L"{EACFAA1F-A6CB-4d4a-83D9-810F84C11803}";
static const wchar_t* kPermLuaClsid         = L"{6CE51F75-0448-438e-B9CA-69C352A248A7}";
static const wchar_t* kPermIidAndPsClsid    = L"{673F14E0-1875-42a1-90F9-647F4AD9DB92}";
static const wchar_t* kLocSensorCatGuid     = L"{BFA794E4-F964-4FDB-90F6-51056BFE4B44}";

static constexpr GUID kGuidApplet           = {0xE9950154, 0xC418, 0x419E, {0xA9, 0x0A, 0x20, 0xC5, 0x28, 0x7A, 0xE2, 0x4B}};
static constexpr GUID kGuidFrameProvider    = {0xE4E54B1D, 0x1AC1, 0x405C, {0xB6, 0x2A, 0xE1, 0x1C, 0xD8, 0x7F, 0x22, 0x61}};
static constexpr GUID kGuidInitializer      = {0xEACFAA1F, 0xA6CB, 0x4D4A, {0x83, 0xD9, 0x81, 0x0F, 0x84, 0xC1, 0x18, 0x03}};
static constexpr GUID kGuidPermLua          = {0x6CE51F75, 0x0448, 0x438E, {0xB9, 0xCA, 0x69, 0xC3, 0x52, 0xA2, 0x48, 0xA7}};
static constexpr GUID kGuidPermIidAndPs     = {0x673F14E0, 0x1875, 0x42A1, {0x90, 0xF9, 0x64, 0x7F, 0x4A, 0xD9, 0xDB, 0x92}};
static constexpr GUID kIidUnknown           = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static constexpr GUID kIidClassFactory      = {0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

// Verified vtable layout of ISensorsCPLChangeDevicePermission ({673F14E0-...}):
//   +0x00 QueryInterface
//   +0x08 AddRef
//   +0x10 Release
//   +0x18 SetLocationOverridePermission(BOOL bEnable)  [checkboxLocationSettings]
//   +0x20 SetLocationProviderCSFlag(BOOL bEnable)      [checkboxLocationProvider]
struct ISensorsCPLChangeDevicePermission : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE SetLocationOverridePermission(BOOL bEnable) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetLocationProviderCSFlag(BOOL bEnable) = 0;
};

enum class ProcessRole { Other, Explorer, Control, DllHost, ComService, ElevatedHelper };

static ProcessRole g_role = ProcessRole::Other;
static std::atomic<int> g_langSetting{0};
static std::atomic<bool> g_showTasks{true};
static std::atomic<bool> g_showWin7NavLinks{false};
static std::atomic<bool> g_shuttingDown{false};
static HANDLE g_stopEvent = nullptr;
static std::optional<std::thread> g_setupThread;
static std::atomic<HMODULE> g_hDui{nullptr};
static std::atomic<HMODULE> g_hSensorsCpl{nullptr};

static std::mutex g_inetMutex;
static HINTERNET g_activeInet = nullptr;
static HINTERNET g_activeUrl = nullptr;

// ============================================================================
// 5. Utilities: Settings, Paths, Atomic File I/O, MSDL Download, Location Sync
// ============================================================================

static std::wstring LowerStr(std::wstring s) {
    for (wchar_t& c : s) c = towlower(c);
    return s;
}

static bool EndsWithI(const std::wstring& s, const wchar_t* suffix) {
    const size_t slen = wcslen(suffix);
    if (s.size() < slen) return false;
    return _wcsicmp(s.c_str() + (s.size() - slen), suffix) == 0;
}

static Lang DetectSystemLang() {
    const LANGID lid = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(lid)) {
        case LANG_ITALIAN:    return Lang::It;
        case LANG_FRENCH:     return Lang::Fr;
        case LANG_SPANISH:    return Lang::Es;
        case LANG_PORTUGUESE: return Lang::Pt;
        case LANG_GERMAN:     return Lang::De;
        default:              return Lang::En;
    }
}

static void LoadSettings() {
    Lang resolved = DetectSystemLang();
    PCWSTR raw = Wh_GetStringSetting(L"language");
    if (raw) {
        if (_wcsicmp(raw, L"en") == 0) resolved = Lang::En;
        else if (_wcsicmp(raw, L"it") == 0) resolved = Lang::It;
        else if (_wcsicmp(raw, L"fr") == 0) resolved = Lang::Fr;
        else if (_wcsicmp(raw, L"es") == 0) resolved = Lang::Es;
        else if (_wcsicmp(raw, L"pt") == 0) resolved = Lang::Pt;
        else if (_wcsicmp(raw, L"de") == 0) resolved = Lang::De;
        Wh_FreeStringSetting(raw);
    }
    g_langSetting.store(static_cast<int>(resolved), std::memory_order_release);
    g_showTasks.store(Wh_GetIntSetting(L"showControlPanelTasks") != 0, std::memory_order_release);
    g_showWin7NavLinks.store(Wh_GetIntSetting(L"showWin7NavLinks") != 0, std::memory_order_release);
}

static Lang CurrentLang() {
    return static_cast<Lang>(g_langSetting.load(std::memory_order_acquire));
}

static std::wstring CplDllFileName(Lang lang) {
    return std::wstring(L"s8cpl2-") + GetLangTable(lang).code + L".dll";
}

static std::wstring LegacyCplDllFileName(Lang lang) {
    return std::wstring(L"s8cpl-") + GetLangTable(lang).code + L".dll";
}

static std::wstring TasksXmlFileName(Lang lang) {
    return std::wstring(L"LocationSettings-") + GetLangTable(lang).code + L".tasks.xml";
}

static std::wstring StorageDir() {
    wchar_t buf[MAX_PATH] = {};
    if (Wh_GetModStoragePath(buf, MAX_PATH) == 0 || !buf[0]) return {};
    std::wstring s(buf);
    while (!s.empty() && (s.back() == L'\\' || s.back() == L'/')) s.pop_back();
    return s;
}

static void EnsureDir(const std::wstring& path) {
    if (path.empty()) return;
    if (CreateDirectoryW(path.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS) return;
    const size_t sep = path.find_last_of(L"\\/");
    if (sep != std::wstring::npos) {
        EnsureDir(path.substr(0, sep));
        CreateDirectoryW(path.c_str(), nullptr);
    }
}

static bool ReadWholeFile(const std::wstring& path, Bytes& out) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{};
    if (!GetFileSizeEx(h, &sz) || sz.QuadPart <= 0 || sz.QuadPart > 32LL * 1024 * 1024) {
        CloseHandle(h);
        return false;
    }
    Bytes buf(static_cast<size_t>(sz.QuadPart));
    size_t got = 0;
    while (got < buf.size()) {
        DWORD rd = 0;
        const DWORD chunk = static_cast<DWORD>(std::min<size_t>(buf.size() - got, 1u << 20));
        if (!ReadFile(h, buf.data() + got, chunk, &rd, nullptr) || rd == 0) {
            CloseHandle(h);
            return false;
        }
        got += rd;
    }
    CloseHandle(h);
    out = std::move(buf);
    return true;
}

static bool WriteFileAtomic(const std::wstring& path, const Bytes& data) {
    {
        Bytes existing;
        if (ReadWholeFile(path, existing) && existing == data) return true;
    }
    const std::wstring tmp = path + L".tmp." + std::to_wstring(GetCurrentProcessId());
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    size_t written = 0;
    while (written < data.size()) {
        DWORD wr = 0;
        const DWORD chunk = static_cast<DWORD>(std::min<size_t>(data.size() - written, 1u << 20));
        if (!WriteFile(h, data.data() + written, chunk, &wr, nullptr) || wr == 0) {
            CloseHandle(h);
            DeleteFileW(tmp.c_str());
            return false;
        }
        written += wr;
    }
    FlushFileBuffers(h);
    CloseHandle(h);
    if (!MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(tmp.c_str());
        Bytes existing;
        if (ReadWholeFile(path, existing) && existing == data) return true;
        return false;
    }
    return true;
}

static bool ReadPinnedFile(const std::wstring& path, const FileSpec& spec, Bytes& out) {
    Bytes b;
    if (!ReadWholeFile(path, b)) return false;
    if (b.size() != spec.size) return false;
    if (sha256::Hex(b) != spec.sha256) return false;
    out = std::move(b);
    return true;
}

static void CancelInFlightDownload() {
    HINTERNET url = nullptr;
    HINTERNET inet = nullptr;
    {
        std::lock_guard<std::mutex> lk(g_inetMutex);
        url = g_activeUrl; g_activeUrl = nullptr;
        inet = g_activeInet; g_activeInet = nullptr;
    }
    if (url) InternetCloseHandle(url);
    if (inet) InternetCloseHandle(inet);
}

static bool DownloadFromMsdl(const FileSpec& spec, Bytes& out) {
    const std::wstring url = std::wstring(L"https://msdl.microsoft.com/download/symbols/") + spec.msdlPath;
    for (int attempt = 0; attempt < 4 && !g_shuttingDown.load(); ++attempt) {
        if (attempt > 0 && g_stopEvent) {
            if (WaitForSingleObject(g_stopEvent, static_cast<DWORD>(attempt * 1500)) == WAIT_OBJECT_0) return false;
        }
        HINTERNET hInet = InternetOpenW(L"Microsoft-Symbol-Server/10.0.0.0",
                                        INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
        if (!hInet) continue;
        DWORD timeoutMs = 20000;
        InternetSetOptionW(hInet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
        InternetSetOptionW(hInet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
        {
            std::lock_guard<std::mutex> lk(g_inetMutex);
            g_activeInet = hInet;
        }

        const DWORD flags = INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI;
        HINTERNET hUrl = InternetOpenUrlW(hInet, url.c_str(), nullptr, 0, flags, 0);
        {
            std::lock_guard<std::mutex> lk(g_inetMutex);
            g_activeUrl = hUrl;
        }
        bool ok = false;
        Bytes buf;
        if (hUrl) {
            DWORD status = 0;
            DWORD statusLen = sizeof(status);
            if (HttpQueryInfoW(hUrl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &statusLen, nullptr) &&
                status == 200u) {
                buf.reserve(spec.size);
                uint8_t chunk[32768];
                DWORD rd = 0;
                ok = true;
                while (!g_shuttingDown.load()) {
                    if (!InternetReadFile(hUrl, chunk, sizeof(chunk), &rd)) { ok = false; break; }
                    if (rd == 0) break;
                    if (buf.size() + rd > spec.size + 4096u) { ok = false; break; }
                    buf.insert(buf.end(), chunk, chunk + rd);
                }
            }
        }
        {
            std::lock_guard<std::mutex> lk(g_inetMutex);
            if (g_activeUrl == hUrl) g_activeUrl = nullptr;
            if (g_activeInet == hInet) g_activeInet = nullptr;
        }
        if (hUrl) InternetCloseHandle(hUrl);
        InternetCloseHandle(hInet);

        if (ok && !g_shuttingDown.load() && buf.size() == spec.size &&
            buf.size() >= 2 && buf[0] == 'M' && buf[1] == 'Z' && sha256::Hex(buf) == spec.sha256) {
            out = std::move(buf);
            return true;
        }
        Wh_Log(L"MSDL attempt %d failed for %s (got %zu bytes)", attempt + 1, spec.name, buf.size());
    }
    return false;
}

static bool GetVerifiedFile(const std::wstring& dlDir, const FileSpec& spec, Bytes& out) {
    const std::wstring path = dlDir + L"\\" + spec.name;
    if (ReadPinnedFile(path, spec, out)) return true;
    if (!DownloadFromMsdl(spec, out)) return false;
    WriteFileAtomic(path, out);
    return true;
}

static HMODULE LoadVerifiedModule(const std::wstring& path, const Bytes& expected) {
    Bytes disk;
    if (!ReadWholeFile(path, disk) || disk != expected) {
        Wh_Log(L"Verification mismatch before LoadLibrary: %s", path.c_str());
        return nullptr;
    }
    return LoadLibraryExW(path.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
}

static bool IsCurrentProcessElevated() {
    HANDLE tok = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok)) return false;
    TOKEN_ELEVATION te{};
    DWORD cb = sizeof(te);
    const BOOL ok = GetTokenInformation(tok, TokenElevation, &te, sizeof(te), &cb);
    CloseHandle(tok);
    return ok && te.TokenIsElevated != 0;
}

// ---- Windows 10/11 Modern Location ConsentStore Synchronization ----
static const wchar_t* kConsentLocationSubKey =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\location";
static const wchar_t* kSensorLocOverrideSubKey =
    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Sensor\\Overrides\\{BFA794E4-F964-4FDB-90F6-51056BFE4B44}";

static bool ReadWin10LocationConsent(bool& outEnabled) {
    wchar_t buf[32] = {};
    DWORD cb = sizeof(buf);
    const LSTATUS st = RegGetValueW(
        HKEY_LOCAL_MACHINE, kConsentLocationSubKey, L"Value",
        RRF_RT_REG_SZ | RRF_SUBKEY_WOW6464KEY, nullptr, buf, &cb);
    if (st == ERROR_SUCCESS) {
        if (_wcsicmp(buf, L"Allow") == 0) { outEnabled = true; return true; }
        if (_wcsicmp(buf, L"Deny") == 0)  { outEnabled = false; return true; }
    }
    return false;
}

static bool WriteConsentStoreValue(HKEY root, bool enable) {
    HKEY hk = nullptr;
    if (RegCreateKeyExW(root, kConsentLocationSubKey, 0, nullptr, 0,
                        KEY_SET_VALUE | KEY_WOW64_64KEY, nullptr, &hk, nullptr) != ERROR_SUCCESS || !hk) {
        return false;
    }
    const wchar_t* val = enable ? L"Allow" : L"Deny";
    const DWORD cb = static_cast<DWORD>((wcslen(val) + 1u) * sizeof(wchar_t));
    const LSTATUS st = RegSetValueExW(hk, L"Value", 0, REG_SZ, reinterpret_cast<const BYTE*>(val), cb);
    RegCloseKey(hk);
    return st == ERROR_SUCCESS;
}

static bool SyncWin10LocationMachine(bool enable) {
    bool anyOk = WriteConsentStoreValue(HKEY_LOCAL_MACHINE, enable);

    // Ensure Sensor\Overrides\{BFA794E4-...}\SensorPermissionState matches too.
    HKEY hOv = nullptr;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, kSensorLocOverrideSubKey, 0, nullptr, 0,
                        KEY_SET_VALUE | KEY_WOW64_64KEY, nullptr, &hOv, nullptr) == ERROR_SUCCESS && hOv) {
        const DWORD state = enable ? 1u : 0u;
        if (RegSetValueExW(hOv, L"SensorPermissionState", 0, REG_DWORD,
                           reinterpret_cast<const BYTE*>(&state), sizeof(state)) == ERROR_SUCCESS) {
            anyOk = true;
        }
        RegCloseKey(hOv);
    }

    // Invoke Microsoft's SystemSettingsAdminFlows.exe (if present) so WinRT / Settings UI updates instantly.
    wchar_t sysDir[MAX_PATH] = {};
    if (GetSystemDirectoryW(sysDir, MAX_PATH) > 0) {
        const std::wstring flowsExe = std::wstring(sysDir) + L"\\SystemSettingsAdminFlows.exe";
        if (GetFileAttributesW(flowsExe.c_str()) != INVALID_FILE_ATTRIBUTES) {
            std::wstring cmd = L"\"" + flowsExe + L"\" SetCamSystemGlobal location " + (enable ? L"1" : L"0");
            STARTUPINFOW si{};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
            PROCESS_INFORMATION pi{};
            if (CreateProcessW(flowsExe.c_str(), cmd.data(), nullptr, nullptr, FALSE,
                               CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, 3500u);
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
                anyOk = true;
            }
        }
    }
    return anyOk;
}

static void SyncWin10LocationCurrentUser(bool enable) {
    WriteConsentStoreValue(HKEY_CURRENT_USER, enable);
}

// Forward declaration for elevated COM wrapper used by both dllhost & fallback paths
namespace loc_shim {
static ISensorsCPLChangeDevicePermission* WrapElevatedPerm(ISensorsCPLChangeDevicePermission* inner);
}

// ============================================================================
// 6. Virtual Registry & COM Activation (Zero HKLM/HKCU Writes)
// ============================================================================

namespace vreg {

struct Val {
    std::wstring name;
    DWORD type;
    std::wstring str;
    DWORD dw;
};

struct Key {
    std::wstring rel;  // lowercase relative key path
    std::vector<Val> values;
    std::vector<std::wstring> subkeys;
};

struct Table {
    std::vector<Key> keys;
};

static std::mutex g_tableMutex;
static std::shared_ptr<const Table> g_table;

static std::shared_ptr<const Table> GetTable() {
    std::lock_guard<std::mutex> lk(g_tableMutex);
    return g_table;
}

static void SetTable(std::shared_ptr<const Table> t) {
    std::lock_guard<std::mutex> lk(g_tableMutex);
    g_table = std::move(t);
}

static bool ContainsKeywordI(LPCWSTR s) {
    if (!s) return false;
    for (; *s; ++s) {
        if (_wcsnicmp(s, L"clsid", 5) == 0 ||
            _wcsnicmp(s, L"appid", 5) == 0 ||
            _wcsnicmp(s, L"interface", 9) == 0 ||
            _wcsnicmp(s, L"controlpanel", 12) == 0 ||
            _wcsnicmp(s, L"shell extensions", 16) == 0 ||
            _wcsnicmp(s, L"e9950154-c418-419e-a90a-20c5287ae24b", 36) == 0 ||
            _wcsnicmp(s, L"6ce51f75-0448-438e-b9ca-69c352a248a7", 36) == 0 ||
            _wcsnicmp(s, L"673f14e0-1875-42a1-90f9-647f4ad9db92", 36) == 0) {
            return true;
        }
    }
    return false;
}

static const wchar_t* PredefinedName(HKEY h) {
    if (h == HKEY_CLASSES_ROOT) return L"hkey_classes_root";
    if (h == HKEY_CURRENT_USER) return L"hkey_current_user";
    if (h == HKEY_LOCAL_MACHINE) return L"hkey_local_machine";
    if (h == HKEY_USERS) return L"hkey_users";
    if (h == HKEY_CURRENT_CONFIG) return L"hkey_current_config";
    return L"";
}

static std::wstring JoinPath(const std::wstring& base, LPCWSTR sub) {
    std::wstring p = base;
    if (sub && *sub) {
        std::wstring s = LowerStr(sub);
        while (!s.empty() && s.front() == L'\\') s.erase(s.begin());
        while (!s.empty() && s.back() == L'\\') s.pop_back();
        if (!s.empty()) {
            if (!p.empty()) p += L'\\';
            p += s;
        }
    }
    return p;
}

static bool EndsWithKey(const std::wstring& path, const std::wstring& rel) {
    if (path.size() < rel.size()) return false;
    if (path.compare(path.size() - rel.size(), rel.size(), rel) != 0) return false;
    return path.size() == rel.size() || path[path.size() - rel.size() - 1] == L'\\';
}

static const Key* FindKey(const Table& t, const std::wstring& lowerPath) {
    if (lowerPath.find(L"wow6432node") != std::wstring::npos) return nullptr;
    for (const Key& k : t.keys) {
        if (EndsWithKey(lowerPath, k.rel)) return &k;
    }
    return nullptr;
}

static const Val* FindValue(const Key& k, LPCWSTR name) {
    const wchar_t* n = name ? name : L"";
    for (const Val& v : k.values) {
        if (_wcsicmp(v.name.c_str(), n) == 0) return &v;
    }
    return nullptr;
}

static bool IsNamespaceParent(const std::wstring& lowerPath) {
    if (lowerPath.find(L"wow6432node") != std::wstring::npos) return false;
    if (lowerPath.find(L"hkey_current_user") != std::wstring::npos) return false;
    if (lowerPath.find(L"hkey_users") != std::wstring::npos) return false;
    return EndsWithKey(lowerPath, L"controlpanel\\namespace");
}

struct TrackedHandle {
    std::wstring path;
    bool fake = false;
};

static std::mutex g_handleMutex;
static std::unordered_map<HKEY, TrackedHandle> g_handles;
static HKEY g_scratchRoot = nullptr;
static std::wstring g_scratchPath;

static void CleanupStaleHives() {
    const std::wstring dir = StorageDir();
    if (dir.empty()) return;
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileW((dir + L"\\vreg-*.hiv*").c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
        }
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);
}

static HKEY EnsureScratchRootLocked() {
    if (g_scratchRoot) return g_scratchRoot;
    const std::wstring dir = StorageDir();
    if (dir.empty()) return nullptr;
    EnsureDir(dir);
    g_scratchPath = dir + L"\\vreg-" + std::to_wstring(GetCurrentProcessId()) + L".hiv";
    DeleteFileW(g_scratchPath.c_str());
    HKEY root = nullptr;
    const LSTATUS st = RegLoadAppKeyW(g_scratchPath.c_str(), &root, KEY_ALL_ACCESS, REG_PROCESS_APPKEY, 0);
    if (st != ERROR_SUCCESS || !root) {
        Wh_Log(L"RegLoadAppKeyW failed (%ld)", static_cast<long>(st));
        return nullptr;
    }
    g_scratchRoot = root;
    return g_scratchRoot;
}

#ifndef REG_MUI_STRING_TRUNCATE
#define REG_MUI_STRING_TRUNCATE 0x00000001u
#endif

using RegOpenKeyExW_t     = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
using RegOpenKeyW_t       = LSTATUS(WINAPI*)(HKEY, LPCWSTR, PHKEY);
using RegCreateKeyExW_t   = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
using RegCloseKey_t       = LSTATUS(WINAPI*)(HKEY);
using RegQueryValueExW_t  = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegQueryValueW_t    = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPWSTR, PLONG);
using RegGetValueW_t      = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
using RegLoadMUIStringW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPWSTR, DWORD, LPDWORD, DWORD, LPCWSTR);
using RegEnumKeyExW_t     = LSTATUS(WINAPI*)(HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPWSTR, LPDWORD, PFILETIME);
using RegEnumKeyW_t       = LSTATUS(WINAPI*)(HKEY, DWORD, LPWSTR, DWORD);
using RegEnumValueW_t     = LSTATUS(WINAPI*)(HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegQueryInfoKeyW_t  = LSTATUS(WINAPI*)(HKEY, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);

static RegOpenKeyExW_t     RegOpenKeyExW_Orig;
static RegOpenKeyW_t       RegOpenKeyW_Orig;
static RegCreateKeyExW_t   RegCreateKeyExW_Orig;
static RegCloseKey_t       RegCloseKey_Orig;
static RegQueryValueExW_t  RegQueryValueExW_Orig;
static RegQueryValueW_t    RegQueryValueW_Orig;
static RegGetValueW_t      RegGetValueW_Orig;
static RegLoadMUIStringW_t RegLoadMUIStringW_Orig;
static RegEnumKeyExW_t     RegEnumKeyExW_Orig;
static RegEnumKeyW_t       RegEnumKeyW_Orig;
static RegEnumValueW_t     RegEnumValueW_Orig;
static RegQueryInfoKeyW_t  RegQueryInfoKeyW_Orig;

static HKEY NewFakeHandle(const std::wstring& lowerPath) {
    std::lock_guard<std::mutex> lk(g_handleMutex);
    HKEY root = EnsureScratchRootLocked();
    if (!root) return nullptr;
    HKEY dup = nullptr;
    if (RegOpenKeyExW_Orig(root, nullptr, 0, KEY_READ, &dup) != ERROR_SUCCESS || !dup) return nullptr;
    g_handles[dup] = {lowerPath, true};
    return dup;
}

static void TrackRealHandle(HKEY h, const std::wstring& lowerPath) {
    if (!h) return;
    std::lock_guard<std::mutex> lk(g_handleMutex);
    g_handles[h] = {lowerPath, false};
}

static bool LookupHandle(HKEY h, std::wstring& path, bool& fake) {
    if (!h) return false;
    std::lock_guard<std::mutex> lk(g_handleMutex);
    auto it = g_handles.find(h);
    if (it == g_handles.end()) return false;
    path = it->second.path;
    fake = it->second.fake;
    return true;
}

static void UntrackHandle(HKEY h) {
    if (!h) return;
    std::lock_guard<std::mutex> lk(g_handleMutex);
    g_handles.erase(h);
}

static void ForgetAllHandles() {
    std::lock_guard<std::mutex> lk(g_handleMutex);
    g_handles.clear();
}

static void ReleaseScratchRoot() {
    HKEY root = nullptr;
    std::wstring path;
    {
        std::lock_guard<std::mutex> lk(g_handleMutex);
        root = g_scratchRoot;
        g_scratchRoot = nullptr;
        path = std::move(g_scratchPath);
    }
    if (root && RegCloseKey_Orig) RegCloseKey_Orig(root);
    if (!path.empty()) {
        DeleteFileW(path.c_str());
        DeleteFileW((path + L".LOG1").c_str());
        DeleteFileW((path + L".LOG2").c_str());
    }
}

static std::shared_ptr<const Table> BuildTable(const std::wstring& binDir, Lang lang, bool showTasks, bool nameOnly) {
    auto t = std::make_shared<Table>();
    auto S = [](const wchar_t* n, DWORD ty, std::wstring s) -> Val { return {n, ty, std::move(s), 0}; };
    auto D = [](const wchar_t* n, DWORD d) -> Val { return {n, REG_DWORD, {}, d}; };
    auto add = [&](std::wstring rel, std::vector<Val> vals, std::vector<std::wstring> subs) {
        t->keys.push_back({LowerStr(std::move(rel)), std::move(vals), std::move(subs)});
    };

    const std::wstring cplPath = binDir + L"\\" + CplDllFileName(lang);
    const std::wstring tasksPath = binDir + L"\\" + TasksXmlFileName(lang);
    const std::wstring dispName = LookupString(lang, 1u);
    const std::wstring infoTip = LookupString(lang, 701u);
    const std::wstring applet = L"clsid\\" + LowerStr(kAppletClsid);

    std::vector<Val> appletVals = {
        S(L"", REG_SZ, dispName),
        S(L"LocalizedString", REG_EXPAND_SZ, L"@" + cplPath + L",-1"),
        S(L"InfoTip", REG_SZ, infoTip),
        S(L"System.ApplicationName", REG_SZ, L"Microsoft.LocationSettings"),
        S(L"System.ControlPanel.Category", REG_SZ, L"2"),
        D(L"{305CA226-D286-468e-B848-2B2E8E697B74} 2", 2u),
        D(L"System.ControlPanel.EnableInSafeMode", 3u),
        S(L"System.Software.TasksFileUrl", REG_SZ, showTasks ? tasksPath : L"Internal"),
    };
    add(applet, std::move(appletVals),
        nameOnly ? std::vector<std::wstring>{}
                 : std::vector<std::wstring>{L"DefaultIcon", L"InProcServer32", L"Instance", L"ShellFolder"});

    // InfoTip is exposed as a pre-localized literal REG_SZ on both CLSID and
    // ControlPanel\NameSpace (same pattern as win7-legacy-applet-restorer) so
    // Explorer always displays the hover tooltip regardless of which path it queries.
    add(L"controlpanel\\namespace\\" + LowerStr(kAppletClsid),
        {S(L"", REG_SZ, dispName),
         S(L"InfoTip", REG_SZ, infoTip)}, {});
    if (nameOnly) return t;

    // LayoutFolder shell applet registration (shdocvw.dll)
    add(applet + L"\\defaulticon", {S(L"", REG_EXPAND_SZ, cplPath + L",-1")}, {});
    add(applet + L"\\inprocserver32",
        {S(L"", REG_EXPAND_SZ, L"%SystemRoot%\\System32\\shdocvw.dll"),
         S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
    add(applet + L"\\instance",
        {S(L"CLSID", REG_SZ, kLayoutFolderClsid)}, {L"InitPropertyBag"});
    add(applet + L"\\instance\\initpropertybag",
        {S(L"ResourceDLL", REG_EXPAND_SZ, cplPath),
         D(L"ResourceID", 100u)}, {});
    add(applet + L"\\shellfolder",
        {D(L"Attributes", 0xA0000000u),
         S(L"WantsParseDisplayName", REG_SZ, L"")}, {});

    // FrameProvider & CSensorsCplInitializer
    for (const wchar_t* cls : {kFrameProviderClsid, kInitializerClsid}) {
        const std::wstring root = L"clsid\\" + LowerStr(cls);
        add(root, {S(L"", REG_SZ, L"Location Settings Control Panel")}, {L"InProcServer32"});
        add(root + L"\\inprocserver32",
            {S(L"", REG_EXPAND_SZ, cplPath),
             S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
    }

    // Elevated COM LUA Helper: CSensorsCPLChangeDevicePermission ({6CE51F75-...}) + AppID surrogate
    const std::wstring permRoot = L"clsid\\" + LowerStr(kPermLuaClsid);
    add(permRoot,
        {S(L"", REG_SZ, L"Sensors CPL Change Device Permission LUA Helper"),
         S(L"AppId", REG_SZ, kPermLuaClsid),
         S(L"LocalizedString", REG_EXPAND_SZ, L"@" + cplPath + L",-1")},
        {L"Elevation", L"InProcServer32"});
    add(permRoot + L"\\elevation",
        {D(L"Enabled", 1u),
         S(L"IconReference", REG_EXPAND_SZ, L"@" + cplPath + L",-1")}, {});
    add(permRoot + L"\\inprocserver32",
        {S(L"", REG_EXPAND_SZ, cplPath),
         S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});

    add(L"appid\\" + LowerStr(kPermLuaClsid),
        {S(L"", REG_SZ, L"Sensors CPL Change Device Permission LUA Helper"),
         S(L"DllSurrogate", REG_SZ, L"")}, {});

    // Embedded MIDL Proxy/Stub & Interface registration for ISensorsCPLChangeDevicePermission ({673F14E0-...})
    const std::wstring psRoot = L"clsid\\" + LowerStr(kPermIidAndPsClsid);
    add(psRoot, {S(L"", REG_SZ, L"ISensorsCPLChangeDevicePermission ProxyStub")}, {L"InProcServer32"});
    add(psRoot + L"\\inprocserver32",
        {S(L"", REG_EXPAND_SZ, cplPath),
         S(L"ThreadingModel", REG_SZ, L"Both")}, {});

    const std::wstring ifaceRoot = L"interface\\" + LowerStr(kPermIidAndPsClsid);
    add(ifaceRoot, {S(L"", REG_SZ, L"ISensorsCPLChangeDevicePermission")}, {L"NumMethods", L"ProxyStubClsid32"});
    add(ifaceRoot + L"\\nummethods", {S(L"", REG_SZ, L"5")}, {});
    add(ifaceRoot + L"\\proxystubclsid32", {S(L"", REG_SZ, kPermIidAndPsClsid)}, {});

    return t;
}

static LSTATUS WriteVal(const Val& v, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData, bool expandEnv) {
    if (v.type == REG_DWORD) {
        if (lpType) *lpType = REG_DWORD;
        const DWORD need = sizeof(DWORD);
        if (!lpData) {
            if (lpcbData) *lpcbData = need;
            return ERROR_SUCCESS;
        }
        if (!lpcbData || *lpcbData < need) {
            if (lpcbData) *lpcbData = need;
            return ERROR_MORE_DATA;
        }
        std::memcpy(lpData, &v.dw, need);
        *lpcbData = need;
        return ERROR_SUCCESS;
    }

    std::wstring s = v.str;
    DWORD outType = v.type;
    if (expandEnv && v.type == REG_EXPAND_SZ) {
        const DWORD req = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
        if (req > 0) {
            std::wstring exp(req, L'\0');
            const DWORD got = ExpandEnvironmentStringsW(s.c_str(), exp.data(), req);
            if (got > 0) {
                if (!exp.empty() && exp.back() == L'\0') exp.pop_back();
                s = std::move(exp);
                outType = REG_SZ;
            }
        }
    }
    if (lpType) *lpType = outType;
    const DWORD need = static_cast<DWORD>((s.size() + 1u) * sizeof(wchar_t));
    if (!lpData) {
        if (lpcbData) *lpcbData = need;
        return ERROR_SUCCESS;
    }
    if (!lpcbData || *lpcbData < need) {
        if (lpcbData) *lpcbData = need;
        return ERROR_MORE_DATA;
    }
    std::memcpy(lpData, s.c_str(), need);
    *lpcbData = need;
    return ERROR_SUCCESS;
}

static bool NamespaceChildExistsReal(HKEY hKey) {
    HKEY sub = nullptr;
    if (RegOpenKeyExW_Orig(hKey, kAppletClsid, 0, KEY_READ, &sub) == ERROR_SUCCESS && sub) {
        RegCloseKey_Orig(sub);
        return true;
    }
    return false;
}

static std::wstring QueryRealKeyPath(HKEY hKey) {
    using NtQueryKey_t = LONG(NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    static NtQueryKey_t pNtQueryKey = []() -> NtQueryKey_t {
        HMODULE nt = GetModuleHandleW(L"ntdll.dll");
        return nt ? reinterpret_cast<NtQueryKey_t>(reinterpret_cast<void*>(GetProcAddress(nt, "NtQueryKey"))) : nullptr;
    }();
    if (!pNtQueryKey || !hKey) return {};
    alignas(8) uint8_t buf[1024];
    ULONG retLen = 0;
    if (pNtQueryKey(hKey, 3u /* KeyNameInformation */, buf, sizeof(buf), &retLen) != 0) return {};
    const ULONG nameBytes = *reinterpret_cast<const ULONG*>(buf);
    if (nameBytes < 2u || sizeof(ULONG) + nameBytes > sizeof(buf)) return {};
    std::wstring ntPath(reinterpret_cast<const wchar_t*>(buf + sizeof(ULONG)), nameBytes / sizeof(wchar_t));
    ntPath = LowerStr(std::move(ntPath));
    if (ntPath.rfind(L"\\registry\\machine\\", 0) == 0) {
        return L"hkey_local_machine\\" + ntPath.substr(18);
    }
    if (ntPath.rfind(L"\\registry\\user\\", 0) == 0) {
        return L"hkey_users\\" + ntPath.substr(15);
    }
    return ntPath;
}

static LSTATUS HandleOpen(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    if (samDesired & KEY_WOW64_32KEY) {
        return RegOpenKeyExW_Orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }
    std::wstring basePath;
    bool baseFake = false;
    const bool tracked = LookupHandle(hKey, basePath, baseFake);
    if (!tracked) basePath = PredefinedName(hKey);

    if (tracked || ContainsKeywordI(lpSubKey)) {
        if (!tracked && basePath.empty() && hKey) {
            basePath = QueryRealKeyPath(hKey);
            if (!basePath.empty()) TrackRealHandle(hKey, basePath);
        }
        const std::wstring full = JoinPath(basePath, lpSubKey);
        const auto table = GetTable();
        if (table && FindKey(*table, full)) {
            if (!phkResult) return ERROR_INVALID_PARAMETER;
            HKEY fakeH = NewFakeHandle(full);
            if (fakeH) {
                *phkResult = fakeH;
                return ERROR_SUCCESS;
            }
        }
        if (baseFake) return ERROR_FILE_NOT_FOUND;

        HKEY realOut = nullptr;
        const LSTATUS st = RegOpenKeyExW_Orig(hKey, lpSubKey, ulOptions, samDesired, &realOut);
        if (st == ERROR_SUCCESS && realOut) {
            std::wstring trackPath = full;
            if (!tracked && basePath.empty()) {
                const std::wstring canonical = QueryRealKeyPath(realOut);
                if (!canonical.empty()) trackPath = canonical;
            }
            TrackRealHandle(realOut, trackPath);
            if (phkResult) *phkResult = realOut;
        }
        return st;
    }
    return RegOpenKeyExW_Orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

static LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    try {
        return HandleOpen(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    } catch (...) {
        return RegOpenKeyExW_Orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }
}

static LSTATUS WINAPI RegOpenKeyW_Hook(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult) {
    try {
        return HandleOpen(hKey, lpSubKey, 0, KEY_READ, phkResult);
    } catch (...) {
        return RegOpenKeyW_Orig(hKey, lpSubKey, phkResult);
    }
}

static LSTATUS WINAPI RegCreateKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD reserved, LPWSTR lpClass,
                                           DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSec,
                                           PHKEY phkResult, LPDWORD lpdwDisp) {
    try {
        if (!(samDesired & KEY_WOW64_32KEY)) {
            std::wstring basePath;
            bool baseFake = false;
            const bool tracked = LookupHandle(hKey, basePath, baseFake);
            if (!tracked) basePath = PredefinedName(hKey);
            if (tracked || ContainsKeywordI(lpSubKey)) {
                if (!tracked && basePath.empty() && hKey) {
                    basePath = QueryRealKeyPath(hKey);
                    if (!basePath.empty()) TrackRealHandle(hKey, basePath);
                }
                const std::wstring full = JoinPath(basePath, lpSubKey);
                const auto table = GetTable();
                if (table && FindKey(*table, full)) {
                    if (!phkResult) return ERROR_INVALID_PARAMETER;
                    HKEY fakeH = NewFakeHandle(full);
                    if (fakeH) {
                        *phkResult = fakeH;
                        if (lpdwDisp) *lpdwDisp = REG_OPENED_EXISTING_KEY;
                        return ERROR_SUCCESS;
                    }
                }
                if (baseFake) return ERROR_ACCESS_DENIED;
            }
        }
    } catch (...) {
    }
    return RegCreateKeyExW_Orig(hKey, lpSubKey, reserved, lpClass, dwOptions, samDesired, lpSec, phkResult, lpdwDisp);
}

static LSTATUS WINAPI RegCloseKey_Hook(HKEY hKey) {
    try {
        UntrackHandle(hKey);
    } catch (...) {
    }
    return RegCloseKey_Orig(hKey);
}

static LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                            LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    try {
        std::wstring path;
        bool fake = false;
        if (LookupHandle(hKey, path, fake) && fake) {
            const auto table = GetTable();
            const Key* k = table ? FindKey(*table, path) : nullptr;
            if (!k) return ERROR_FILE_NOT_FOUND;
            const Val* v = FindValue(*k, lpValueName);
            if (!v) return ERROR_FILE_NOT_FOUND;
            return WriteVal(*v, lpType, lpData, lpcbData, false);
        }
    } catch (...) {
    }
    return RegQueryValueExW_Orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegQueryValueW_Hook(HKEY hKey, LPCWSTR lpSubKey, LPWSTR lpData, PLONG lpcbData) {
    try {
        std::wstring basePath;
        bool baseFake = false;
        const bool tracked = LookupHandle(hKey, basePath, baseFake);
        if (!tracked) basePath = PredefinedName(hKey);
        if (tracked || ContainsKeywordI(lpSubKey)) {
            if (!tracked && basePath.empty() && hKey) {
                basePath = QueryRealKeyPath(hKey);
                if (!basePath.empty()) TrackRealHandle(hKey, basePath);
            }
            const std::wstring full = JoinPath(basePath, lpSubKey);
            const auto table = GetTable();
            const Key* k = table ? FindKey(*table, full) : nullptr;
            if (k) {
                const Val* v = FindValue(*k, L"");
                if (!v) return ERROR_FILE_NOT_FOUND;
                DWORD cb = (lpcbData && *lpcbData > 0) ? static_cast<DWORD>(*lpcbData) : 0u;
                const LSTATUS st = WriteVal(*v, nullptr, reinterpret_cast<LPBYTE>(lpData),
                                            lpcbData ? &cb : nullptr, false);
                if (lpcbData) *lpcbData = static_cast<LONG>(cb);
                return st;
            }
            if (baseFake) return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
    }
    return RegQueryValueW_Orig(hKey, lpSubKey, lpData, lpcbData);
}

static LSTATUS WINAPI RegGetValueW_Hook(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags,
                                        LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    try {
        std::wstring basePath;
        bool baseFake = false;
        const bool tracked = LookupHandle(hKey, basePath, baseFake);
        if (!tracked) basePath = PredefinedName(hKey);
        if (tracked || ContainsKeywordI(lpSubKey)) {
            if (!tracked && basePath.empty() && hKey) {
                basePath = QueryRealKeyPath(hKey);
                if (!basePath.empty()) TrackRealHandle(hKey, basePath);
            }
            const std::wstring full = JoinPath(basePath, lpSubKey);
            const auto table = GetTable();
            const Key* k = table ? FindKey(*table, full) : nullptr;
            if (k) {
                const Val* v = FindValue(*k, lpValue);
                if (!v) return ERROR_FILE_NOT_FOUND;
                const bool expand = (dwFlags & RRF_NOEXPAND) == 0;
                return WriteVal(*v, pdwType, reinterpret_cast<LPBYTE>(pvData), pcbData, expand);
            }
            if (baseFake) return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
    }
    return RegGetValueW_Orig(hKey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

static LSTATUS WINAPI RegLoadMUIStringW_Hook(HKEY hKey, LPCWSTR pszValue, LPWSTR pszOutBuf,
                                             DWORD cbOutBuf, LPDWORD pcbData, DWORD Flags,
                                             LPCWSTR pszDirectory) {
    try {
        std::wstring path;
        bool fake = false;
        if (LookupHandle(hKey, path, fake) && fake) {
            const auto table = GetTable();
            const Key* k = table ? FindKey(*table, path) : nullptr;
            if (!k) return ERROR_FILE_NOT_FOUND;
            const Val* v = FindValue(*k, pszValue);
            if (!v) return ERROR_FILE_NOT_FOUND;
            if (v->type != REG_SZ && v->type != REG_EXPAND_SZ) return ERROR_INVALID_DATA;

            std::wstring resolved;
            if (!v->str.empty() && v->str[0] == L'@') {
                const size_t sep = v->str.rfind(L",-");
                if (sep != std::wstring::npos) {
                    wchar_t* endPtr = nullptr;
                    const unsigned long id = wcstoul(v->str.c_str() + sep + 2u, &endPtr, 10);
                                if (endPtr && *endPtr == L'\0' && id > 0ul && id <= 0xFFFFul) {
                                    resolved = LookupString(CurrentLang(), static_cast<uint16_t>(id));
                                }
                }
            }
            if (resolved.empty()) {
                resolved = v->str;
                if (v->type == REG_EXPAND_SZ) {
                    const DWORD req = ExpandEnvironmentStringsW(resolved.c_str(), nullptr, 0);
                    if (req > 0) {
                        std::wstring exp(req, L'\0');
                        const DWORD got = ExpandEnvironmentStringsW(resolved.c_str(), exp.data(), req);
                        if (got > 0) {
                            if (!exp.empty() && exp.back() == L'\0') exp.pop_back();
                            resolved = std::move(exp);
                        }
                    }
                }
            }

            const DWORD need = static_cast<DWORD>((resolved.size() + 1u) * sizeof(wchar_t));
            if (pcbData) *pcbData = need;
            if (!pszOutBuf || cbOutBuf < need) {
                if ((Flags & REG_MUI_STRING_TRUNCATE) != 0 && pszOutBuf && cbOutBuf >= sizeof(wchar_t)) {
                    const size_t maxChars = (cbOutBuf / sizeof(wchar_t)) - 1u;
                    if (maxChars > 0) std::memcpy(pszOutBuf, resolved.c_str(), maxChars * sizeof(wchar_t));
                    pszOutBuf[maxChars] = L'\0';
                    return ERROR_SUCCESS;
                }
                return ERROR_MORE_DATA;
            }
            std::memcpy(pszOutBuf, resolved.c_str(), need);
            return ERROR_SUCCESS;
        }
    } catch (...) {
    }
    return RegLoadMUIStringW_Orig(hKey, pszValue, pszOutBuf, cbOutBuf, pcbData, Flags, pszDirectory);
}

static LSTATUS WINAPI RegEnumValueW_Hook(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, LPDWORD lpcchValueName,
                                         LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    try {
        std::wstring path;
        bool fake = false;
        if (LookupHandle(hKey, path, fake) && fake) {
            const auto table = GetTable();
            const Key* k = table ? FindKey(*table, path) : nullptr;
            if (!k || dwIndex >= k->values.size()) return ERROR_NO_MORE_ITEMS;
            const Val& v = k->values[dwIndex];
            if (!lpValueName || !lpcchValueName) return ERROR_INVALID_PARAMETER;
            if (*lpcchValueName <= v.name.size()) {
                *lpcchValueName = static_cast<DWORD>(v.name.size() + 1u);
                return ERROR_MORE_DATA;
            }
            std::memcpy(lpValueName, v.name.c_str(), (v.name.size() + 1u) * sizeof(wchar_t));
            *lpcchValueName = static_cast<DWORD>(v.name.size());
            if (lpType || lpData || lpcbData) {
                return WriteVal(v, lpType, lpData, lpcbData, false);
            }
            return ERROR_SUCCESS;
        }
    } catch (...) {
    }
    return RegEnumValueW_Orig(hKey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegEnumKeyExW_Hook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName,
                                         LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass,
                                         PFILETIME lpftLastWriteTime) {
    try {
        std::wstring path;
        bool fake = false;
        if (LookupHandle(hKey, path, fake)) {
            const auto table = GetTable();
            if (fake) {
                const Key* k = table ? FindKey(*table, path) : nullptr;
                if (!k || dwIndex >= k->subkeys.size()) return ERROR_NO_MORE_ITEMS;
                const std::wstring& s = k->subkeys[dwIndex];
                if (!lpName || !lpcchName) return ERROR_INVALID_PARAMETER;
                if (*lpcchName <= s.size()) {
                    *lpcchName = static_cast<DWORD>(s.size() + 1u);
                    return ERROR_MORE_DATA;
                }
                std::memcpy(lpName, s.c_str(), (s.size() + 1u) * sizeof(wchar_t));
                *lpcchName = static_cast<DWORD>(s.size());
                if (lpClass && lpcchClass) { if (*lpcchClass > 0) lpClass[0] = 0; *lpcchClass = 0; }
                if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
                return ERROR_SUCCESS;
            }
            if (table && IsNamespaceParent(path)) {
                DWORD realCount = 0;
                if (RegQueryInfoKeyW_Orig(hKey, nullptr, nullptr, nullptr, &realCount,
                                          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS &&
                    dwIndex == realCount && !NamespaceChildExistsReal(hKey)) {
                    const size_t len = wcslen(kAppletClsid);
                    if (!lpName || !lpcchName) return ERROR_INVALID_PARAMETER;
                    if (*lpcchName <= len) {
                        *lpcchName = static_cast<DWORD>(len + 1u);
                        return ERROR_MORE_DATA;
                    }
                    std::memcpy(lpName, kAppletClsid, (len + 1u) * sizeof(wchar_t));
                    *lpcchName = static_cast<DWORD>(len);
                    if (lpClass && lpcchClass) { if (*lpcchClass > 0) lpClass[0] = 0; *lpcchClass = 0; }
                    if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
                    return ERROR_SUCCESS;
                }
            }
        }
    } catch (...) {
    }
    return RegEnumKeyExW_Orig(hKey, dwIndex, lpName, lpcchName, lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
}

static LSTATUS WINAPI RegEnumKeyW_Hook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, DWORD cchName) {
    try {
        std::wstring path;
        bool fake = false;
        if (LookupHandle(hKey, path, fake)) {
            const auto table = GetTable();
            if (fake) {
                const Key* k = table ? FindKey(*table, path) : nullptr;
                if (!k || dwIndex >= k->subkeys.size()) return ERROR_NO_MORE_ITEMS;
                const std::wstring& s = k->subkeys[dwIndex];
                if (!lpName || cchName <= s.size()) return ERROR_MORE_DATA;
                std::memcpy(lpName, s.c_str(), (s.size() + 1u) * sizeof(wchar_t));
                return ERROR_SUCCESS;
            }
            if (table && IsNamespaceParent(path)) {
                DWORD realCount = 0;
                if (RegQueryInfoKeyW_Orig(hKey, nullptr, nullptr, nullptr, &realCount,
                                          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS &&
                    dwIndex == realCount && !NamespaceChildExistsReal(hKey)) {
                    const size_t len = wcslen(kAppletClsid);
                    if (!lpName || cchName <= len) return ERROR_MORE_DATA;
                    std::memcpy(lpName, kAppletClsid, (len + 1u) * sizeof(wchar_t));
                    return ERROR_SUCCESS;
                }
            }
        }
    } catch (...) {
    }
    return RegEnumKeyW_Orig(hKey, dwIndex, lpName, cchName);
}

static LSTATUS WINAPI RegQueryInfoKeyW_Hook(HKEY hKey, LPWSTR cls, LPDWORD cchCls, LPDWORD reserved,
                                            LPDWORD cSubKeys, LPDWORD maxSubKeyLen, LPDWORD maxClassLen,
                                            LPDWORD cValues, LPDWORD maxValueNameLen, LPDWORD maxValueLen,
                                            LPDWORD secDescLen, PFILETIME ft) {
    try {
        std::wstring path;
        bool fake = false;
        if (LookupHandle(hKey, path, fake)) {
            const auto table = GetTable();
            if (fake) {
                const Key* k = table ? FindKey(*table, path) : nullptr;
                if (cSubKeys) *cSubKeys = k ? static_cast<DWORD>(k->subkeys.size()) : 0u;
                if (cValues) *cValues = k ? static_cast<DWORD>(k->values.size()) : 0u;
                if (maxSubKeyLen) *maxSubKeyLen = 64u;
                if (maxClassLen) *maxClassLen = 0u;
                if (maxValueNameLen) *maxValueNameLen = 64u;
                if (maxValueLen) *maxValueLen = 512u;
                if (secDescLen) *secDescLen = 0u;
                if (cls && cchCls) { if (*cchCls > 0) cls[0] = 0; *cchCls = 0; }
                if (ft) GetSystemTimeAsFileTime(ft);
                return ERROR_SUCCESS;
            }
            if (table && IsNamespaceParent(path)) {
                const LSTATUS st = RegQueryInfoKeyW_Orig(hKey, cls, cchCls, reserved, cSubKeys, maxSubKeyLen,
                                                         maxClassLen, cValues, maxValueNameLen, maxValueLen,
                                                         secDescLen, ft);
                if (st == ERROR_SUCCESS && cSubKeys && !NamespaceChildExistsReal(hKey)) {
                    ++*cSubKeys;
                    const DWORD need = static_cast<DWORD>(wcslen(kAppletClsid) + 1u);
                    if (maxSubKeyLen && *maxSubKeyLen < need) *maxSubKeyLen = need;
                }
                return st;
            }
        }
    } catch (...) {
    }
    return RegQueryInfoKeyW_Orig(hKey, cls, cchCls, reserved, cSubKeys, maxSubKeyLen, maxClassLen,
                                 cValues, maxValueNameLen, maxValueLen, secDescLen, ft);
}

// ---- Direct COM Activation (CoCreateInstance + CoGetClassObject) ----
// Intercepting BOTH CoCreateInstance and CoGetClassObject guarantees:
//   - shdocvw.dll instantiates FrameProvider & Initializer from our pinned s8cpl DLL
//   - dllhost.exe activates CSensorsCPLChangeDevicePermission wrapped with Win10/11 sync
//   - combase.dll loads our embedded MIDL IPSFactoryBuffer ({673F14E0-...}) cleanly

class CElevatedPermFactoryWrapper final : public IClassFactory {
    std::atomic<ULONG> m_ref{1};
    IClassFactory* m_inner;

public:
    explicit CElevatedPermFactoryWrapper(IClassFactory* inner) : m_inner(inner) {
        if (m_inner) m_inner->AddRef();
    }
    ~CElevatedPermFactoryWrapper() {
        if (m_inner) m_inner->Release();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, kIidUnknown) || IsEqualGUID(riid, kIidClassFactory)) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return m_ref.fetch_add(1, std::memory_order_relaxed) + 1; }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG n = m_ref.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (n == 0) delete this;
        return n;
    }
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override {
        try {
            if (!ppvObject) return E_POINTER;
            *ppvObject = nullptr;
            if (!m_inner) return E_UNEXPECTED;
            if (pUnkOuter) return CLASS_E_NOAGGREGATION;
            if (IsEqualGUID(riid, kGuidPermIidAndPs) || IsEqualGUID(riid, kIidUnknown)) {
                ISensorsCPLChangeDevicePermission* raw = nullptr;
                const HRESULT hr = m_inner->CreateInstance(nullptr, kGuidPermIidAndPs, reinterpret_cast<void**>(&raw));
                if (FAILED(hr) || !raw) return hr;
                ISensorsCPLChangeDevicePermission* wrapped = loc_shim::WrapElevatedPerm(raw);
                raw->Release();
                if (!wrapped) return E_OUTOFMEMORY;
                *ppvObject = wrapped;
                return S_OK;
            }
            return m_inner->CreateInstance(pUnkOuter, riid, ppvObject);
        } catch (...) {
            if (ppvObject) *ppvObject = nullptr;
            return E_FAIL;
        }
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) override {
        return m_inner ? m_inner->LockServer(fLock) : S_OK;
    }
};

struct ComClassEntry {
    GUID clsid;
    int target;  // 0 = shdocvw.dll, 1 = private s8cpl-<lang>.dll
};

static const ComClassEntry kComClasses[] = {
    {kGuidApplet,        0},
    {kGuidFrameProvider, 1},
    {kGuidInitializer,   1},
    {kGuidPermLua,       1},
    {kGuidPermIidAndPs,  1},
};

static HRESULT GetOurClassObject(REFCLSID clsid, REFIID riid, void** ppv) {
    int target = -1;
    for (const ComClassEntry& e : kComClasses) {
        if (IsEqualGUID(clsid, e.clsid)) { target = e.target; break; }
    }
    if (target < 0) return REGDB_E_CLASSNOTREG;

    HMODULE mod = nullptr;
    if (target == 0) {
        mod = GetModuleHandleW(L"shdocvw.dll");
        if (!mod) mod = LoadLibraryExW(L"shdocvw.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    } else {
        mod = g_hSensorsCpl.load(std::memory_order_acquire);
    }
    if (!mod) return REGDB_E_CLASSNOTREG;

    using DllGetClassObject_t = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, LPVOID*);
    auto fn = reinterpret_cast<DllGetClassObject_t>(reinterpret_cast<void*>(GetProcAddress(mod, "DllGetClassObject")));
    if (!fn) return REGDB_E_CLASSNOTREG;

    if (g_role == ProcessRole::DllHost && IsEqualGUID(clsid, kGuidPermLua) &&
        (IsEqualGUID(riid, kIidClassFactory) || IsEqualGUID(riid, kIidUnknown))) {
        IClassFactory* inner = nullptr;
        const HRESULT hr = fn(clsid, kIidClassFactory, reinterpret_cast<void**>(&inner));
        if (FAILED(hr) || !inner) return hr;
        *ppv = new (std::nothrow) CElevatedPermFactoryWrapper(inner);
        inner->Release();
        return *ppv ? S_OK : E_OUTOFMEMORY;
    }

    return fn(clsid, riid, ppv);
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
using CoGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstanceCombase_Orig;
static CoCreateInstance_t CoCreateInstanceOle32_Orig;
static CoGetClassObject_t CoGetClassObjectCombase_Orig;
static CoGetClassObject_t CoGetClassObjectOle32_Orig;

static HRESULT HandleCoCreateInstance(REFCLSID rclsid, LPUNKNOWN outer, DWORD ctx, REFIID riid, LPVOID* ppv,
                                      CoCreateInstance_t orig) {
    try {
        if (ppv && (ctx & CLSCTX_INPROC_SERVER) && GetTable()) {
            IClassFactory* factory = nullptr;
            HRESULT hr = GetOurClassObject(rclsid, kIidClassFactory, reinterpret_cast<void**>(&factory));
            if (SUCCEEDED(hr) && factory) {
                *ppv = nullptr;
                hr = factory->CreateInstance(outer, riid, ppv);
                factory->Release();
                return hr;
            }
        }
    } catch (...) {
    }
    return orig(rclsid, outer, ctx, riid, ppv);
}

static HRESULT HandleCoGetClassObject(REFCLSID rclsid, DWORD ctx, LPVOID serverInfo, REFIID riid, LPVOID* ppv,
                                      CoGetClassObject_t orig) {
    try {
        if (ppv && (ctx & CLSCTX_INPROC_SERVER) && !serverInfo && GetTable()) {
            void* obj = nullptr;
            const HRESULT hr = GetOurClassObject(rclsid, riid, &obj);
            if (SUCCEEDED(hr) && obj) {
                *ppv = obj;
                return hr;
            }
        }
    } catch (...) {
    }
    return orig(rclsid, ctx, serverInfo, riid, ppv);
}

static HRESULT WINAPI CoCreateInstanceCombase_Hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID r, LPVOID* p) {
    return HandleCoCreateInstance(c, o, ctx, r, p, CoCreateInstanceCombase_Orig);
}
static HRESULT WINAPI CoCreateInstanceOle32_Hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID r, LPVOID* p) {
    return HandleCoCreateInstance(c, o, ctx, r, p, CoCreateInstanceOle32_Orig);
}
static HRESULT WINAPI CoGetClassObjectCombase_Hook(REFCLSID c, DWORD ctx, LPVOID si, REFIID r, LPVOID* p) {
    return HandleCoGetClassObject(c, ctx, si, r, p, CoGetClassObjectCombase_Orig);
}
static HRESULT WINAPI CoGetClassObjectOle32_Hook(REFCLSID c, DWORD ctx, LPVOID si, REFIID r, LPVOID* p) {
    return HandleCoGetClassObject(c, ctx, si, r, p, CoGetClassObjectOle32_Orig);
}

static void* FindRegExport(const char* name) {
    if (HMODULE kb = GetModuleHandleW(L"kernelbase.dll")) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(kb, name))) return p;
    }
    HMODULE adv = GetModuleHandleW(L"advapi32.dll");
    if (!adv) adv = LoadLibraryExW(L"advapi32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    return adv ? reinterpret_cast<void*>(GetProcAddress(adv, name)) : nullptr;
}

static bool InstallHooks(bool withCom) {
    struct HookDef { const char* name; void* hook; void** orig; bool required; };
    const HookDef hooks[] = {
        {"RegOpenKeyExW",     (void*)RegOpenKeyExW_Hook,     (void**)&RegOpenKeyExW_Orig,     true},
        {"RegOpenKeyW",       (void*)RegOpenKeyW_Hook,       (void**)&RegOpenKeyW_Orig,       false},
        {"RegCreateKeyExW",   (void*)RegCreateKeyExW_Hook,   (void**)&RegCreateKeyExW_Orig,   true},
        {"RegCloseKey",       (void*)RegCloseKey_Hook,       (void**)&RegCloseKey_Orig,       true},
        {"RegQueryValueExW",  (void*)RegQueryValueExW_Hook,  (void**)&RegQueryValueExW_Orig,  true},
        {"RegQueryValueW",    (void*)RegQueryValueW_Hook,    (void**)&RegQueryValueW_Orig,    false},
        {"RegGetValueW",      (void*)RegGetValueW_Hook,      (void**)&RegGetValueW_Orig,      true},
        {"RegLoadMUIStringW", (void*)RegLoadMUIStringW_Hook, (void**)&RegLoadMUIStringW_Orig, false},
        {"RegEnumValueW",     (void*)RegEnumValueW_Hook,     (void**)&RegEnumValueW_Orig,     false},
        {"RegEnumKeyExW",     (void*)RegEnumKeyExW_Hook,     (void**)&RegEnumKeyExW_Orig,     true},
        {"RegEnumKeyW",       (void*)RegEnumKeyW_Hook,       (void**)&RegEnumKeyW_Orig,       true},
        {"RegQueryInfoKeyW",  (void*)RegQueryInfoKeyW_Hook,  (void**)&RegQueryInfoKeyW_Orig,  true},
    };
    for (const HookDef& h : hooks) {
        void* target = FindRegExport(h.name);
        if (!target || !Wh_SetFunctionHook(target, h.hook, h.orig)) {
            if (!h.required) continue;
            Wh_Log(L"Failed to hook %S", h.name);
            return false;
        }
    }
    if (!withCom) return true;
    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase) combase = LoadLibraryExW(L"combase.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
    if (!ole32) ole32 = LoadLibraryExW(L"ole32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    void* pCciCombase = combase ? reinterpret_cast<void*>(GetProcAddress(combase, "CoCreateInstance")) : nullptr;
    void* pCciOle32 = ole32 ? reinterpret_cast<void*>(GetProcAddress(ole32, "CoCreateInstance")) : nullptr;
    if (pCciCombase) Wh_SetFunctionHook(pCciCombase, (void*)CoCreateInstanceCombase_Hook, (void**)&CoCreateInstanceCombase_Orig);
    if (pCciOle32 && pCciOle32 != pCciCombase) {
        Wh_SetFunctionHook(pCciOle32, (void*)CoCreateInstanceOle32_Hook, (void**)&CoCreateInstanceOle32_Orig);
    }

    void* pGcoCombase = combase ? reinterpret_cast<void*>(GetProcAddress(combase, "CoGetClassObject")) : nullptr;
    void* pGcoOle32 = ole32 ? reinterpret_cast<void*>(GetProcAddress(ole32, "CoGetClassObject")) : nullptr;
    if (pGcoCombase) Wh_SetFunctionHook(pGcoCombase, (void*)CoGetClassObjectCombase_Hook, (void**)&CoGetClassObjectCombase_Orig);
    if (pGcoOle32 && pGcoOle32 != pGcoCombase) {
        Wh_SetFunctionHook(pGcoOle32, (void*)CoGetClassObjectOle32_Hook, (void**)&CoGetClassObjectOle32_Orig);
    }
    return true;
}

}  // namespace vreg

// ============================================================================
// 6b. Private s8cpl2-<lang>.dll IAT Shims, Win7 Nav Links & Elevated Sync
//
// Patches ONLY our private s8cpl2-<lang>.dll's IAT (leaving system DLLs
// untouched) and restores every slot cleanly in Wh_ModBeforeUninit / Uninit:
//   - KERNEL32!RegOpenKeyExW / RegCreateKeyExW: 0xF013F -> 0x2011F fallback
//     on Sensor/LocationProvider keys, plus ConsentStore initial-state sync
//     via RegEnumKeyExW / RegQueryValueExW in Explorer.
//   - KERNEL32!RegSetKeySecurity: tolerates ERROR_ACCESS_DENIED on modern
//     TrustedInstaller-owned Sensor\Overrides keys (lfsvc already has access).
//   - ole32!CoGetObject (Explorer): wraps the elevated COM helper to mirror
//     HKCU\...\ConsentStore\location, and provides a single-prompt elevated
//     rundll32.exe pipe fallback if svchost.exe is not injected or is PPL.
//   - SHLWAPI!#176 (IUnknown_QueryService, Explorer): optionally publishes
//     the Windows 7-era left-pane navigation links ("View location activity"
//     and "See also: Default Location") during CSensorsCplPage::LayoutInitialized
//     with immediate VT_EMPTY bag cleanup for unload-safe lifetime.
// ============================================================================

namespace loc_shim {

using RegOpenKeyExW_t         = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
using RegCreateKeyExW_t       = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
using RegQueryInfoKeyW_t      = LSTATUS(WINAPI*)(HKEY, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);
using RegEnumKeyExW_t         = LSTATUS(WINAPI*)(HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPWSTR, LPDWORD, PFILETIME);
using RegQueryValueExW_t      = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegSetKeySecurity_t     = LSTATUS(WINAPI*)(HKEY, SECURITY_INFORMATION, PSECURITY_DESCRIPTOR);
using RegCloseKey_t           = LSTATUS(WINAPI*)(HKEY);
using CoGetObject_t           = HRESULT(WINAPI*)(LPCWSTR, BIND_OPTS*, REFIID, void**);
using IUnknown_QueryService_t = HRESULT(WINAPI*)(IUnknown*, REFGUID, REFIID, void**);

struct PatchedIatSlot {
    void** slot;
    void* origFn;
};

static std::mutex g_iatMutex;
static std::vector<PatchedIatSlot> g_patchedSlots;

static std::atomic<RegOpenKeyExW_t>         g_regOpen_Orig{nullptr};
static std::atomic<RegCreateKeyExW_t>       g_regCreate_Orig{nullptr};
static std::atomic<RegQueryInfoKeyW_t>      g_regQueryInfo_Orig{nullptr};
static std::atomic<RegEnumKeyExW_t>         g_regEnum_Orig{nullptr};
static std::atomic<RegQueryValueExW_t>      g_regQuery_Orig{nullptr};
static std::atomic<RegSetKeySecurity_t>     g_regSetSec_Orig{nullptr};
static std::atomic<RegCloseKey_t>           g_regClose_Orig{nullptr};
static std::atomic<CoGetObject_t>           g_coGetObject_Orig{nullptr};
static std::atomic<IUnknown_QueryService_t> g_queryService_Orig{nullptr};

// Tracked registry handles opened by s8cpl-<lang>.dll for initial-state sync
struct OverrideKeyInfo {
    bool isSensorRoot = false;
    bool isOverridesParent = false;
    bool isLocGuidKey = false;
    bool synthetic = false;
    bool injectLocGuid = false;
    DWORD realSubKeyCount = 0;
};

static std::mutex g_regTrackMutex;
static std::unordered_map<HKEY, OverrideKeyInfo> g_regTrack;

static bool IsSensorRelatedSubkey(LPCWSTR lpSubKey) {
    if (!lpSubKey) return false;
    const std::wstring lo = LowerStr(lpSubKey);
    return lo.find(L"sensor") != std::wstring::npos ||
           lo.find(L"overrides") != std::wstring::npos ||
           lo.find(L"locationprovider") != std::wstring::npos ||
           lo.find(L"bfa794e4-f964-4fdb-90f6-51056bfe4b44") != std::wstring::npos;
}

static bool IsSensorRootSubkey(LPCWSTR lpSubKey) {
    if (!lpSubKey) return false;
    std::wstring s(lpSubKey);
    while (!s.empty() && s.back() == L'\\') s.pop_back();
    return EndsWithI(s, L"\\sensor");
}

static bool IsOverridesSubkey(LPCWSTR lpSubKey) {
    if (!lpSubKey) return false;
    std::wstring s(lpSubKey);
    while (!s.empty() && s.back() == L'\\') s.pop_back();
    return EndsWithI(s, L"\\overrides") || _wcsicmp(s.c_str(), L"overrides") == 0;
}

static bool IsLocGuidSubkey(LPCWSTR lpSubKey) {
    if (!lpSubKey) return false;
    std::wstring s(lpSubKey);
    while (!s.empty() && s.back() == L'\\') s.pop_back();
    return EndsWithI(s, kLocSensorCatGuid);
}

static HKEY OpenHarmlessSyntheticKey(RegOpenKeyExW_t orig) {
    HKEY h = nullptr;
    if (orig(HKEY_LOCAL_MACHINE, kConsentLocationSubKey, 0, KEY_READ | KEY_WOW64_64KEY, &h) == ERROR_SUCCESS && h) {
        return h;
    }
    return nullptr;
}

static LSTATUS WINAPI RegOpenKeyExW_Shim(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    const RegOpenKeyExW_t orig = g_regOpen_Orig.load(std::memory_order_acquire);
    if (!orig) return ERROR_PROC_NOT_FOUND;
    try {
        bool parentIsSensorRoot = false;
        bool parentIsOverrides = false;
        bool parentSynthetic = false;
        {
            std::lock_guard<std::mutex> lk(g_regTrackMutex);
            auto it = g_regTrack.find(hKey);
            if (it != g_regTrack.end()) {
                parentIsSensorRoot = it->second.isSensorRoot;
                parentIsOverrides = it->second.isOverridesParent;
                parentSynthetic = it->second.synthetic;
            }
        }

        if (parentSynthetic) {
            if (parentIsSensorRoot && IsOverridesSubkey(lpSubKey)) {
                if (!phkResult) return ERROR_INVALID_PARAMETER;
                HKEY hFake = OpenHarmlessSyntheticKey(orig);
                if (!hFake) return ERROR_FILE_NOT_FOUND;
                {
                    std::lock_guard<std::mutex> lk(g_regTrackMutex);
                    g_regTrack[hFake] = {false, true, false, true, false, 0u};
                }
                *phkResult = hFake;
                return ERROR_SUCCESS;
            }
            if (parentIsOverrides && IsLocGuidSubkey(lpSubKey)) {
                if (!phkResult) return ERROR_INVALID_PARAMETER;
                HKEY hFake = OpenHarmlessSyntheticKey(orig);
                if (!hFake) return ERROR_FILE_NOT_FOUND;
                {
                    std::lock_guard<std::mutex> lk(g_regTrackMutex);
                    g_regTrack[hFake] = {false, false, true, true, false, 0u};
                }
                *phkResult = hFake;
                return ERROR_SUCCESS;
            }
            return ERROR_FILE_NOT_FOUND;
        }

        LSTATUS st = orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);

        // On Win10/11, SensorsCpl requests 0xF013F (KEY_ALL_ACCESS | KEY_WOW64_64KEY) on Sensor\Overrides;
        // downgrade WRITE_DAC/WRITE_OWNER/DELETE when TrustedInstaller owns the key.
        if (st == ERROR_ACCESS_DENIED && (samDesired & (WRITE_DAC | WRITE_OWNER | DELETE)) != 0 &&
            (parentIsSensorRoot || parentIsOverrides || IsSensorRelatedSubkey(lpSubKey))) {
            const REGSAM fallbackSam = (samDesired & ~(WRITE_DAC | WRITE_OWNER | DELETE)) | KEY_READ | KEY_WRITE;
            st = orig(hKey, lpSubKey, ulOptions, fallbackSam, phkResult);
        }

        if (g_role == ProcessRole::Explorer) {
            bool consentEnabled = false;
            if (st == ERROR_SUCCESS && phkResult && *phkResult) {
                if (IsSensorRootSubkey(lpSubKey)) {
                    std::lock_guard<std::mutex> lk(g_regTrackMutex);
                    g_regTrack[*phkResult] = {true, false, false, false, false, 0u};
                } else if (IsOverridesSubkey(lpSubKey)) {
                    bool inject = false;
                    if (ReadWin10LocationConsent(consentEnabled)) {
                        HKEY hCheck = nullptr;
                        if (orig(*phkResult, kLocSensorCatGuid, 0, KEY_READ | KEY_WOW64_64KEY, &hCheck) == ERROR_SUCCESS && hCheck) {
                            const RegCloseKey_t origClose = g_regClose_Orig.load(std::memory_order_acquire);
                            if (origClose) origClose(hCheck);
                            else RegCloseKey(hCheck);
                        } else {
                            inject = true;
                        }
                    }
                    std::lock_guard<std::mutex> lk(g_regTrackMutex);
                    g_regTrack[*phkResult] = {false, true, false, false, inject, 0u};
                } else if ((parentIsOverrides || IsSensorRelatedSubkey(lpSubKey)) && IsLocGuidSubkey(lpSubKey)) {
                    std::lock_guard<std::mutex> lk(g_regTrackMutex);
                    g_regTrack[*phkResult] = {false, false, true, false, false, 0u};
                }
            } else if (st == ERROR_FILE_NOT_FOUND && phkResult && ReadWin10LocationConsent(consentEnabled)) {
                if (IsSensorRootSubkey(lpSubKey)) {
                    if (HKEY hFake = OpenHarmlessSyntheticKey(orig)) {
                        std::lock_guard<std::mutex> lk(g_regTrackMutex);
                        g_regTrack[hFake] = {true, false, false, true, false, 0u};
                        *phkResult = hFake;
                        return ERROR_SUCCESS;
                    }
                } else if (IsOverridesSubkey(lpSubKey)) {
                    if (HKEY hFake = OpenHarmlessSyntheticKey(orig)) {
                        std::lock_guard<std::mutex> lk(g_regTrackMutex);
                        g_regTrack[hFake] = {false, true, false, true, false, 0u};
                        *phkResult = hFake;
                        return ERROR_SUCCESS;
                    }
                } else if (parentIsOverrides && IsLocGuidSubkey(lpSubKey)) {
                    if (HKEY hFake = OpenHarmlessSyntheticKey(orig)) {
                        std::lock_guard<std::mutex> lk(g_regTrackMutex);
                        g_regTrack[hFake] = {false, false, true, true, false, 0u};
                        *phkResult = hFake;
                        return ERROR_SUCCESS;
                    }
                }
            }
        }
        return st;
    } catch (...) {
        return orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }
}

static LSTATUS WINAPI RegCreateKeyExW_Shim(HKEY hKey, LPCWSTR lpSubKey, DWORD reserved, LPWSTR lpClass,
                                           DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSec,
                                           PHKEY phkResult, LPDWORD lpdwDisp) {
    const RegCreateKeyExW_t orig = g_regCreate_Orig.load(std::memory_order_acquire);
    if (!orig) return ERROR_PROC_NOT_FOUND;
    try {
        LSTATUS st = orig(hKey, lpSubKey, reserved, lpClass, dwOptions, samDesired, lpSec, phkResult, lpdwDisp);
        if (st == ERROR_ACCESS_DENIED && (samDesired & (WRITE_DAC | WRITE_OWNER | DELETE)) != 0 &&
            IsSensorRelatedSubkey(lpSubKey)) {
            const REGSAM fallbackSam = (samDesired & ~(WRITE_DAC | WRITE_OWNER | DELETE)) | KEY_READ | KEY_WRITE;
            st = orig(hKey, lpSubKey, reserved, lpClass, dwOptions, fallbackSam, lpSec, phkResult, lpdwDisp);
        }
        return st;
    } catch (...) {
        return orig(hKey, lpSubKey, reserved, lpClass, dwOptions, samDesired, lpSec, phkResult, lpdwDisp);
    }
}

static LSTATUS WINAPI RegQueryInfoKeyW_Shim(HKEY hKey, LPWSTR cls, LPDWORD cchCls, LPDWORD reserved,
                                            LPDWORD cSubKeys, LPDWORD maxSubKeyLen, LPDWORD maxClassLen,
                                            LPDWORD cValues, LPDWORD maxValueNameLen, LPDWORD maxValueLen,
                                            LPDWORD secDescLen, PFILETIME ft) {
    const RegQueryInfoKeyW_t orig = g_regQueryInfo_Orig.load(std::memory_order_acquire);
    if (!orig) return ERROR_PROC_NOT_FOUND;
    try {
        OverrideKeyInfo info{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lk(g_regTrackMutex);
            auto it = g_regTrack.find(hKey);
            if (it != g_regTrack.end()) {
                info = it->second;
                found = true;
            }
        }

        if (found) {
            if (info.isOverridesParent && info.synthetic) {
                if (cSubKeys) *cSubKeys = 1u;
                if (maxSubKeyLen) *maxSubKeyLen = 64u;
                if (maxClassLen) *maxClassLen = 0u;
                if (cValues) *cValues = 0u;
                if (maxValueNameLen) *maxValueNameLen = 0u;
                if (maxValueLen) *maxValueLen = 0u;
                if (secDescLen) *secDescLen = 0u;
                if (cls && cchCls) { if (*cchCls > 0) cls[0] = 0; *cchCls = 0; }
                if (ft) GetSystemTimeAsFileTime(ft);
                return ERROR_SUCCESS;
            }
            if (info.isLocGuidKey) {
                bool consentEnabled = false;
                if (info.synthetic || ReadWin10LocationConsent(consentEnabled)) {
                    if (cSubKeys) *cSubKeys = 0u;
                    if (maxSubKeyLen) *maxSubKeyLen = 0u;
                    if (maxClassLen) *maxClassLen = 0u;
                    if (cValues) *cValues = 1u;
                    if (maxValueNameLen) *maxValueNameLen = 32u;
                    if (maxValueLen) *maxValueLen = sizeof(DWORD);
                    if (secDescLen) *secDescLen = 0u;
                    if (cls && cchCls) { if (*cchCls > 0) cls[0] = 0; *cchCls = 0; }
                    if (ft) GetSystemTimeAsFileTime(ft);
                    return ERROR_SUCCESS;
                }
            }
            if (info.isOverridesParent && info.injectLocGuid) {
                DWORD realSub = 0;
                const LSTATUS st = orig(hKey, cls, cchCls, reserved, &realSub, maxSubKeyLen, maxClassLen,
                                        cValues, maxValueNameLen, maxValueLen, secDescLen, ft);
                if (st == ERROR_SUCCESS) {
                    {
                        std::lock_guard<std::mutex> lk(g_regTrackMutex);
                        auto it = g_regTrack.find(hKey);
                        if (it != g_regTrack.end()) it->second.realSubKeyCount = realSub;
                    }
                    if (cSubKeys) *cSubKeys = realSub + 1u;
                    const DWORD need = static_cast<DWORD>(wcslen(kLocSensorCatGuid) + 1u);
                    if (maxSubKeyLen && *maxSubKeyLen < need) *maxSubKeyLen = need;
                }
                return st;
            }
        }
    } catch (...) {
    }
    return orig(hKey, cls, cchCls, reserved, cSubKeys, maxSubKeyLen, maxClassLen,
                cValues, maxValueNameLen, maxValueLen, secDescLen, ft);
}

static LSTATUS WINAPI RegEnumKeyExW_Shim(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName,
                                         LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass,
                                         PFILETIME lpftLastWriteTime) {
    const RegEnumKeyExW_t orig = g_regEnum_Orig.load(std::memory_order_acquire);
    if (!orig) return ERROR_PROC_NOT_FOUND;
    try {
        OverrideKeyInfo info{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lk(g_regTrackMutex);
            auto it = g_regTrack.find(hKey);
            if (it != g_regTrack.end()) {
                info = it->second;
                found = true;
            }
        }

        auto emitLocGuid = [&]() -> LSTATUS {
            const size_t len = wcslen(kLocSensorCatGuid);
            if (!lpName || !lpcchName) return ERROR_INVALID_PARAMETER;
            if (*lpcchName <= len) {
                *lpcchName = static_cast<DWORD>(len + 1u);
                return ERROR_MORE_DATA;
            }
            std::memcpy(lpName, kLocSensorCatGuid, (len + 1u) * sizeof(wchar_t));
            *lpcchName = static_cast<DWORD>(len);
            if (lpClass && lpcchClass) { if (*lpcchClass > 0) lpClass[0] = 0; *lpcchClass = 0; }
            if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
            return ERROR_SUCCESS;
        };

        if (found) {
            if (info.isOverridesParent && info.synthetic) {
                return (dwIndex == 0u) ? emitLocGuid() : ERROR_NO_MORE_ITEMS;
            }
            if (info.isLocGuidKey && info.synthetic) {
                return ERROR_NO_MORE_ITEMS;
            }
            if (info.isOverridesParent && info.injectLocGuid && dwIndex == info.realSubKeyCount) {
                return emitLocGuid();
            }
        }
    } catch (...) {
    }
    return orig(hKey, dwIndex, lpName, lpcchName, lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
}

static LSTATUS WINAPI RegQueryValueExW_Shim(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                            LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    const RegQueryValueExW_t orig = g_regQuery_Orig.load(std::memory_order_acquire);
    if (!orig) return ERROR_PROC_NOT_FOUND;
    try {
        bool isLocKey = false;
        bool synthetic = false;
        {
            std::lock_guard<std::mutex> lk(g_regTrackMutex);
            auto it = g_regTrack.find(hKey);
            if (it != g_regTrack.end()) {
                isLocKey = it->second.isLocGuidKey;
                synthetic = it->second.synthetic;
            }
        }

        if (isLocKey && lpValueName && _wcsicmp(lpValueName, L"SensorPermissionState") == 0) {
            bool consentEnabled = false;
            if (ReadWin10LocationConsent(consentEnabled)) {
                if (lpType) *lpType = REG_DWORD;
                const DWORD need = sizeof(DWORD);
                if (!lpData) {
                    if (lpcbData) *lpcbData = need;
                    return ERROR_SUCCESS;
                }
                if (!lpcbData || *lpcbData < need) {
                    if (lpcbData) *lpcbData = need;
                    return ERROR_MORE_DATA;
                }
                const DWORD val = consentEnabled ? 1u : 0u;
                std::memcpy(lpData, &val, need);
                *lpcbData = need;
                return ERROR_SUCCESS;
            }
            if (synthetic) return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
    }
    return orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegSetKeySecurity_Shim(HKEY hKey, SECURITY_INFORMATION secInfo, PSECURITY_DESCRIPTOR pSd) {
    const RegSetKeySecurity_t orig = g_regSetSec_Orig.load(std::memory_order_acquire);
    if (!orig) return ERROR_PROC_NOT_FOUND;
    const LSTATUS st = orig(hKey, secInfo, pSd);
    // Win10/11 lfsvc already has access to Sensor\Overrides; do not fail Apply if WRITE_DAC is denied.
    if (st == ERROR_ACCESS_DENIED) return ERROR_SUCCESS;
    return st;
}

static LSTATUS WINAPI RegCloseKey_Shim(HKEY hKey) {
    try {
        std::lock_guard<std::mutex> lk(g_regTrackMutex);
        g_regTrack.erase(hKey);
    } catch (...) {
    }
    const RegCloseKey_t orig = g_regClose_Orig.load(std::memory_order_acquire);
    return orig ? orig(hKey) : RegCloseKey(hKey);
}

// ---- Elevated ISensorsCPLChangeDevicePermission Wrapper (runs inside elevated process) ----
class CElevatedLocationPerm final : public ISensorsCPLChangeDevicePermission {
    std::atomic<ULONG> m_ref{1};
    ISensorsCPLChangeDevicePermission* m_inner;

public:
    explicit CElevatedLocationPerm(ISensorsCPLChangeDevicePermission* inner) : m_inner(inner) {
        if (m_inner) m_inner->AddRef();
    }
    ~CElevatedLocationPerm() {
        if (m_inner) m_inner->Release();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, kIidUnknown) || IsEqualGUID(riid, kGuidPermIidAndPs)) {
            *ppv = static_cast<ISensorsCPLChangeDevicePermission*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return m_ref.fetch_add(1, std::memory_order_relaxed) + 1; }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG n = m_ref.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (n == 0) delete this;
        return n;
    }
    HRESULT STDMETHODCALLTYPE SetLocationOverridePermission(BOOL bEnable) override {
        try {
            const HRESULT hrOrig = m_inner ? m_inner->SetLocationOverridePermission(bEnable) : E_UNEXPECTED;
            const bool synced = SyncWin10LocationMachine(bEnable != FALSE);
            Wh_Log(L"[loc-elev] SetLocationOverridePermission(%d): orig=0x%08X sync=%d",
                   bEnable ? 1 : 0, static_cast<unsigned>(hrOrig), synced ? 1 : 0);
            return (SUCCEEDED(hrOrig) || synced) ? S_OK : hrOrig;
        } catch (...) {
            return E_FAIL;
        }
    }
    HRESULT STDMETHODCALLTYPE SetLocationProviderCSFlag(BOOL bEnable) override {
        try {
            const HRESULT hrOrig = m_inner ? m_inner->SetLocationProviderCSFlag(bEnable) : E_UNEXPECTED;
            Wh_Log(L"[loc-elev] SetLocationProviderCSFlag(%d): hr=0x%08X",
                   bEnable ? 1 : 0, static_cast<unsigned>(hrOrig));
            return hrOrig;
        } catch (...) {
            return E_FAIL;
        }
    }
};

static ISensorsCPLChangeDevicePermission* WrapElevatedPerm(ISensorsCPLChangeDevicePermission* inner) {
    return new (std::nothrow) CElevatedLocationPerm(inner);
}

// ---- Explorer-side Wrapper (mirrors HKCU ConsentStore after elevated Apply succeeds) ----
class CExplorerLocationPermProxy final : public ISensorsCPLChangeDevicePermission {
    std::atomic<ULONG> m_ref{1};
    ISensorsCPLChangeDevicePermission* m_inner;

public:
    explicit CExplorerLocationPermProxy(ISensorsCPLChangeDevicePermission* inner) : m_inner(inner) {
        if (m_inner) m_inner->AddRef();
    }
    ~CExplorerLocationPermProxy() {
        if (m_inner) m_inner->Release();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, kIidUnknown) || IsEqualGUID(riid, kGuidPermIidAndPs)) {
            *ppv = static_cast<ISensorsCPLChangeDevicePermission*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return m_ref.fetch_add(1, std::memory_order_relaxed) + 1; }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG n = m_ref.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (n == 0) delete this;
        return n;
    }
    HRESULT STDMETHODCALLTYPE SetLocationOverridePermission(BOOL bEnable) override {
        try {
            const HRESULT hr = m_inner ? m_inner->SetLocationOverridePermission(bEnable) : E_UNEXPECTED;
            if (SUCCEEDED(hr)) {
                SyncWin10LocationCurrentUser(bEnable != FALSE);
            }
            return hr;
        } catch (...) {
            return E_FAIL;
        }
    }
    HRESULT STDMETHODCALLTYPE SetLocationProviderCSFlag(BOOL bEnable) override {
        try {
            return m_inner ? m_inner->SetLocationProviderCSFlag(bEnable) : E_UNEXPECTED;
        } catch (...) {
            return E_FAIL;
        }
    }
};

// ---- One-Shot UAC Elevated Pipe Fallback (used ONLY if svchost.exe COM Elevation fails) ----
enum : uint32_t {
    kPipeOpQuit = 0u,
    kPipeOpSetOverride = 1u,
    kPipeOpSetProvider = 2u,
};

struct PipeReq {
    uint32_t op;
    int32_t val;
};

struct PipeResp {
    HRESULT hr;
};

static bool PipeWriteExact(HANDLE hPipe, const void* buf, DWORD cb, DWORD timeoutMs) {
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) return false;
    DWORD written = 0;
    BOOL ok = WriteFile(hPipe, buf, cb, &written, &ov);
    if (!ok && GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(ov.hEvent, timeoutMs) == WAIT_OBJECT_0) {
            ok = GetOverlappedResult(hPipe, &ov, &written, FALSE);
        } else {
            CancelIoEx(hPipe, &ov);
            ok = FALSE;
        }
    }
    CloseHandle(ov.hEvent);
    return ok && written == cb;
}

static bool PipeReadExact(HANDLE hPipe, void* buf, DWORD cb, DWORD timeoutMs) {
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) return false;
    DWORD got = 0;
    BOOL ok = ReadFile(hPipe, buf, cb, &got, &ov);
    if (!ok && GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(ov.hEvent, timeoutMs) == WAIT_OBJECT_0) {
            ok = GetOverlappedResult(hPipe, &ov, &got, FALSE);
        } else {
            CancelIoEx(hPipe, &ov);
            ok = FALSE;
        }
    }
    CloseHandle(ov.hEvent);
    return ok && got == cb;
}

class CPipeLocationPermProxy final : public ISensorsCPLChangeDevicePermission {
    std::atomic<ULONG> m_ref{1};
    HANDLE m_pipe;
    HANDLE m_proc;

    HRESULT SendOp(uint32_t op, BOOL val) {
        if (m_pipe == INVALID_HANDLE_VALUE) return E_UNEXPECTED;
        const PipeReq req{op, val ? 1 : 0};
        PipeResp resp{E_FAIL};
        if (!PipeWriteExact(m_pipe, &req, sizeof(req), 15000u)) return E_FAIL;
        if (!PipeReadExact(m_pipe, &resp, sizeof(resp), 15000u)) return E_FAIL;
        return resp.hr;
    }

public:
    CPipeLocationPermProxy(HANDLE pipe, HANDLE proc) : m_pipe(pipe), m_proc(proc) {}
    ~CPipeLocationPermProxy() {
        if (m_pipe != INVALID_HANDLE_VALUE) {
            const PipeReq quit{kPipeOpQuit, 0};
            PipeWriteExact(m_pipe, &quit, sizeof(quit), 1500u);
            FlushFileBuffers(m_pipe);
            DisconnectNamedPipe(m_pipe);
            CloseHandle(m_pipe);
            m_pipe = INVALID_HANDLE_VALUE;
        }
        if (m_proc) {
            WaitForSingleObject(m_proc, 3000u);
            CloseHandle(m_proc);
            m_proc = nullptr;
        }
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, kIidUnknown) || IsEqualGUID(riid, kGuidPermIidAndPs)) {
            *ppv = static_cast<ISensorsCPLChangeDevicePermission*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return m_ref.fetch_add(1, std::memory_order_relaxed) + 1; }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG n = m_ref.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (n == 0) delete this;
        return n;
    }
    HRESULT STDMETHODCALLTYPE SetLocationOverridePermission(BOOL bEnable) override {
        try {
            const HRESULT hr = SendOp(kPipeOpSetOverride, bEnable);
            if (SUCCEEDED(hr)) SyncWin10LocationCurrentUser(bEnable != FALSE);
            return hr;
        } catch (...) {
            return E_FAIL;
        }
    }
    HRESULT STDMETHODCALLTYPE SetLocationProviderCSFlag(BOOL bEnable) override {
        try {
            return SendOp(kPipeOpSetProvider, bEnable);
        } catch (...) {
            return E_FAIL;
        }
    }
};

static std::wstring MakeRandomPipeToken() {
    GUID g1{}, g2{};
    CoCreateGuid(&g1);
    CoCreateGuid(&g2);
    wchar_t buf[80] = {};
    swprintf(buf, 80, L"wh-loc-%08lX%04X%04X%02X%02X%08lX%04X",
             g1.Data1, g1.Data2, g1.Data3, g1.Data4[0], g1.Data4[1], g2.Data1, g2.Data2);
    return buf;
}

static HRESULT LaunchElevatedPipeFallback(HWND ownerHwnd, void** ppv) {
    const std::wstring dir = StorageDir();
    if (dir.empty()) return E_FAIL;
    const std::wstring cplPath = dir + L"\\bin\\" + CplDllFileName(CurrentLang());
    if (GetFileAttributesW(cplPath.c_str()) == INVALID_FILE_ATTRIBUTES) return E_FAIL;

    const std::wstring token = MakeRandomPipeToken();
    const std::wstring pipeName = L"\\\\.\\pipe\\" + token;

    // Protected DACL: SYSTEM, Builtin Administrators, and Creator/Owner only.
    PSECURITY_DESCRIPTOR pSd = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;OW)", SDDL_REVISION_1, &pSd, nullptr)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = pSd;
    sa.bInheritHandle = FALSE;

    HANDLE hPipe = CreateNamedPipeW(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
        1u, 256u, 256u, 15000u, &sa);
    LocalFree(pSd);
    if (hPipe == INVALID_HANDLE_VALUE) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) {
        CloseHandle(hPipe);
        return E_OUTOFMEMORY;
    }

    BOOL connected = ConnectNamedPipe(hPipe, &ov);
    const DWORD connErr = connected ? ERROR_SUCCESS : GetLastError();
    if (!connected && connErr != ERROR_IO_PENDING && connErr != ERROR_PIPE_CONNECTED) {
        CloseHandle(ov.hEvent);
        CloseHandle(hPipe);
        return HRESULT_FROM_WIN32(connErr);
    }

    wchar_t sysDir[MAX_PATH] = {};
    if (GetSystemDirectoryW(sysDir, MAX_PATH) == 0) {
        CancelIoEx(hPipe, &ov);
        CloseHandle(ov.hEvent);
        CloseHandle(hPipe);
        return E_FAIL;
    }
    const std::wstring rundll = std::wstring(sysDir) + L"\\rundll32.exe";
    const std::wstring params = L"\"" + cplPath + L"\",DllRegisterServer --wh-loc-pipe=" + token;

    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    sei.hwnd = ownerHwnd;
    sei.lpVerb = L"runas";
    sei.lpFile = rundll.c_str();
    sei.lpParameters = params.c_str();
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExW(&sei)) {
        const DWORD seErr = GetLastError();
        CancelIoEx(hPipe, &ov);
        CloseHandle(ov.hEvent);
        CloseHandle(hPipe);
        return HRESULT_FROM_WIN32(seErr);
    }

    if (!sei.hProcess) {
        CancelIoEx(hPipe, &ov);
        CloseHandle(ov.hEvent);
        CloseHandle(hPipe);
        return E_FAIL;
    }

    bool clientReady = (connErr == ERROR_PIPE_CONNECTED);
    if (!clientReady) {
        HANDLE waitHs[2] = {ov.hEvent, sei.hProcess};
        const DWORD w = WaitForMultipleObjects(2u, waitHs, FALSE, 15000u);
        if (w == WAIT_OBJECT_0) {
            DWORD dummy = 0;
            clientReady = GetOverlappedResult(hPipe, &ov, &dummy, FALSE) != FALSE;
        } else {
            CancelIoEx(hPipe, &ov);
        }
    }
    CloseHandle(ov.hEvent);

    if (!clientReady) {
        CloseHandle(hPipe);
        CloseHandle(sei.hProcess);
        Wh_Log(L"[loc] Elevated helper exited before connecting to pipe");
        return E_FAIL;
    }

    ULONG clientPid = 0;
    const DWORD expectedPid = GetProcessId(sei.hProcess);
    if (!GetNamedPipeClientProcessId(hPipe, &clientPid) || clientPid != expectedPid) {
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
        CloseHandle(sei.hProcess);
        Wh_Log(L"[loc] Pipe client PID mismatch (%lu != %lu)", clientPid, expectedPid);
        return E_ACCESSDENIED;
    }

    *ppv = new (std::nothrow) CPipeLocationPermProxy(hPipe, sei.hProcess);
    if (!*ppv) {
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
        CloseHandle(sei.hProcess);
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

static HRESULT WINAPI CoGetObject_Shim(LPCWSTR pszName, BIND_OPTS* pBindOptions, REFIID riid, void** ppv) {
    const CoGetObject_t orig = g_coGetObject_Orig.load(std::memory_order_acquire);
    if (!orig) return E_UNEXPECTED;
    try {
        const bool isOurElev =
            pszName && ppv && IsEqualGUID(riid, kGuidPermIidAndPs) &&
            LowerStr(pszName).find(L"6ce51f75-0448-438e-b9ca-69c352a248a7") != std::wstring::npos;
        if (!isOurElev) {
            return orig(pszName, pBindOptions, riid, ppv);
        }

        void* realObj = nullptr;
        const HRESULT hr = orig(pszName, pBindOptions, riid, &realObj);
        if (SUCCEEDED(hr) && realObj) {
            auto* rawPerm = static_cast<ISensorsCPLChangeDevicePermission*>(realObj);
            *ppv = new (std::nothrow) CExplorerLocationPermProxy(rawPerm);
            rawPerm->Release();
            return *ppv ? S_OK : E_OUTOFMEMORY;
        }

        // Never double-prompt if the user explicitly declined UAC.
        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
            return hr;
        }

        Wh_Log(L"[loc] Native COM elevation returned 0x%08X; activating fallback", static_cast<unsigned>(hr));

        // If Explorer is already elevated, apply directly in-process via our verified DLL.
        if (IsCurrentProcessElevated()) {
            IClassFactory* cf = nullptr;
            if (SUCCEEDED(vreg::GetOurClassObject(kGuidPermLua, kIidClassFactory, reinterpret_cast<void**>(&cf))) && cf) {
                ISensorsCPLChangeDevicePermission* raw = nullptr;
                const HRESULT chr = cf->CreateInstance(nullptr, kGuidPermIidAndPs, reinterpret_cast<void**>(&raw));
                cf->Release();
                if (SUCCEEDED(chr) && raw) {
                    ISensorsCPLChangeDevicePermission* elev = WrapElevatedPerm(raw);
                    raw->Release();
                    if (elev) {
                        *ppv = new (std::nothrow) CExplorerLocationPermProxy(elev);
                        elev->Release();
                        return *ppv ? S_OK : E_OUTOFMEMORY;
                    }
                }
            }
        }

        HWND owner = nullptr;
        if (pBindOptions && pBindOptions->cbStruct >= sizeof(BIND_OPTS3)) {
            owner = reinterpret_cast<const BIND_OPTS3*>(pBindOptions)->hwnd;
        }
        return LaunchElevatedPipeFallback(owner, ppv);
    } catch (...) {
        return E_FAIL;
    }
}

// ============================================================================
// Optional Windows 7-era left-pane navigation links
//   - Tasks ("Control Panel Home" pane): View location activity (ID 225)
//   - See also (bottom pane):            Default Location (ID 224)
// Binary-verified CControlPanelNavLinks / CControlPanelNavLink ABI across
// Windows 7, Windows 8.1, and Windows 11 24H2 (shell32.dll 10.0.26100.9974).
// ============================================================================

static constexpr GUID kSidControlPanelPagesState = {
    0xA4803EE4, 0x5323, 0x4751, {0xB8, 0x51, 0x1B, 0xB2, 0xB7, 0x98, 0x8A, 0x39}
};
static constexpr GUID kSidControlPanelView = {
    0xA46E5C25, 0xC09C, 0x4CA8, {0x9A, 0x53, 0x49, 0xCF, 0x7F, 0x86, 0x55, 0x25}
};
static constexpr GUID kIidPropertyBag = {
    0x55272A00, 0x42CB, 0x11CE, {0x81, 0x35, 0x00, 0xAA, 0x00, 0x4B, 0xB8, 0x51}
};

using DPA_Create_t          = void*(WINAPI*)(int);
using DPA_InsertPtr_t       = int(WINAPI*)(void*, int, void*);
using DPA_DestroyCallback_t = void(WINAPI*)(void*, int(CALLBACK*)(void*, void*), void*);

struct ComCtlDpaApi {
    DPA_Create_t create = nullptr;
    DPA_InsertPtr_t insertPtr = nullptr;
    DPA_DestroyCallback_t destroyCb = nullptr;
};

static ComCtlDpaApi ResolveComCtlDpa() {
    ComCtlDpaApi api{};
    HMODULE hComCtl = GetModuleHandleW(L"comctl32.dll");
    if (!hComCtl) hComCtl = LoadLibraryW(L"comctl32.dll");
    if (!hComCtl) return api;
    auto getProc = [hComCtl](const char* name, WORD ord) -> void* {
        if (FARPROC p = GetProcAddress(hComCtl, name)) return reinterpret_cast<void*>(p);
        return reinterpret_cast<void*>(GetProcAddress(hComCtl, MAKEINTRESOURCEA(ord)));
    };
    api.create    = reinterpret_cast<DPA_Create_t>(getProc("DPA_Create", 328u));
    api.insertPtr = reinterpret_cast<DPA_InsertPtr_t>(getProc("DPA_InsertPtr", 334u));
    api.destroyCb = reinterpret_cast<DPA_DestroyCallback_t>(getProc("DPA_DestroyCallback", 386u));
    return api;
}

static LPWSTR AllocCoTaskStr(const wchar_t* s) {
    if (!s) return nullptr;
    const size_t bytes = (wcslen(s) + 1u) * sizeof(wchar_t);
    auto* out = static_cast<LPWSTR>(CoTaskMemAlloc(bytes));
    if (out) memcpy(out, s, bytes);
    return out;
}

struct CpNavLinkCommand {
    DWORD cmdType = 1u;              // +0x00: 1 = SHELL (ShellExecuteExW + SEE_MASK_DOENVSUBST)
    DWORD cmdId = 0u;                // +0x04
    LPWSTR lpFile = nullptr;         // +0x08
    LPWSTR lpParameters = nullptr;   // +0x10
    BYTE fCallback = 0u;             // +0x18
    BYTE fTopLevel = 0u;             // +0x19
    BYTE pad1[6] = {};               // +0x1A..+0x1F
    uint64_t sqm0 = 0u;              // +0x20
    uint64_t sqm1 = 0u;              // +0x28
};
static_assert(sizeof(CpNavLinkCommand) == 0x30, "CpNavLinkCommand ABI mismatch");
static_assert(offsetof(CpNavLinkCommand, lpFile) == 0x08, "CpNavLinkCommand::lpFile offset mismatch");
static_assert(offsetof(CpNavLinkCommand, lpParameters) == 0x10, "CpNavLinkCommand::lpParameters offset mismatch");

struct CpNavLink {
    DWORD listType = 0u;             // +0x00: 0 = Tasks (top), 1 = See also (bottom)
    DWORD pad0 = 0u;                 // +0x04
    LPWSTR pszName = nullptr;        // +0x08
    LPWSTR pszTooltip = nullptr;     // +0x10
    HICON hIcon = nullptr;           // +0x18
    CpNavLinkCommand cmd{};          // +0x20..+0x4F
    BYTE fDisabled = 0u;             // +0x50
    BYTE pad2[7] = {};               // +0x51..+0x57
};
static_assert(sizeof(CpNavLink) == 0x58, "CpNavLink ABI mismatch");
static_assert(offsetof(CpNavLink, pszName) == 0x08, "CpNavLink::pszName offset mismatch");
static_assert(offsetof(CpNavLink, hIcon) == 0x18, "CpNavLink::hIcon offset mismatch");
static_assert(offsetof(CpNavLink, cmd) == 0x20, "CpNavLink::cmd offset mismatch");
static_assert(offsetof(CpNavLink, fDisabled) == 0x50, "CpNavLink::fDisabled offset mismatch");

static int CALLBACK DestroyNavLinkCallback(void* p, void*) {
    auto* link = static_cast<CpNavLink*>(p);
    if (link) {
        CoTaskMemFree(link->pszName);
        CoTaskMemFree(link->pszTooltip);
        if (link->hIcon) DestroyIcon(link->hIcon);
        CoTaskMemFree(link->cmd.lpFile);
        CoTaskMemFree(link->cmd.lpParameters);
        delete link;
    }
    return 1;
}

class CWin7NavLinks final : public IUnknown {
    void* m_hdpa = nullptr;                      // +0x08 (read by shell32 PopulateNavLinks)
    std::atomic<LONG> m_cRef{1};                 // +0x10
    std::atomic<LONG> m_wasRead{0};              // +0x14
    DPA_DestroyCallback_t m_destroyCb = nullptr; // +0x18

    ~CWin7NavLinks() {
        if (m_hdpa && m_destroyCb) {
            m_destroyCb(m_hdpa, &DestroyNavLinkCallback, nullptr);
            m_hdpa = nullptr;
        }
    }

public:
    CWin7NavLinks(void* hdpa, DPA_DestroyCallback_t destroyCb)
        : m_hdpa(hdpa), m_destroyCb(destroyCb) {}

    bool WasRead() const { return m_wasRead.load(std::memory_order_acquire) != 0; }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, kIidUnknown)) {
            m_wasRead.store(1, std::memory_order_release);
            *ppv = static_cast<IUnknown*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(m_cRef.fetch_add(1, std::memory_order_relaxed) + 1);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        const LONG n = m_cRef.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (n == 0) delete this;
        return static_cast<ULONG>(n);
    }
};

static std::wstring LoadCplString(uint16_t id, const wchar_t* fallback) {
    HMODULE hCpl = g_hSensorsCpl.load(std::memory_order_acquire);
    if (hCpl) {
        wchar_t buf[256] = {};
        const int n = LoadStringW(hCpl, id, buf, static_cast<int>(sizeof(buf) / sizeof(buf[0])));
        if (n > 0) return std::wstring(buf, static_cast<size_t>(n));
    }
    return fallback ? std::wstring(fallback) : std::wstring();
}

static std::wstring ResolveLocationActivityParams() {
    HKEY hChan = nullptr;
    if (RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WINEVT\\Channels\\"
            L"Microsoft-Windows-LocationServiceProvider/Operational",
            0, KEY_READ | KEY_WOW64_64KEY, &hChan) == ERROR_SUCCESS && hChan) {
        RegCloseKey(hChan);
        return L"/c:\"Microsoft-Windows-LocationServiceProvider/Operational\"";
    }
    wchar_t xmlPath[MAX_PATH] = {};
    const DWORD n = ExpandEnvironmentStringsW(
        L"%SystemRoot%\\System32\\LocationNotificationsView.xml", xmlPath, MAX_PATH);
    if (n > 0 && n < MAX_PATH && GetFileAttributesW(xmlPath) != INVALID_FILE_ATTRIBUTES) {
        return std::wstring(L"/v:\"") + xmlPath + L"\"";
    }
    return {};
}

static void PublishWin7NavLinks(IUnknown* punkSite, IUnknown_QueryService_t origQs) {
    if (!punkSite || !origQs) return;
    const ComCtlDpaApi dpa = ResolveComCtlDpa();
    if (!dpa.create || !dpa.insertPtr || !dpa.destroyCb) return;

    IPropertyBag* pViewBag = nullptr;
    if (FAILED(origQs(punkSite, kSidControlPanelView, kIidPropertyBag,
                      reinterpret_cast<void**>(&pViewBag))) || !pViewBag) {
        return;
    }

    void* hdpa = dpa.create(4);
    if (!hdpa) {
        pViewBag->Release();
        return;
    }

    auto addShellLink = [&](DWORD listType, uint16_t strId, const wchar_t* fallbackLabel,
                            const wchar_t* file, const wchar_t* params) -> bool {
        const std::wstring label = LoadCplString(strId, fallbackLabel);
        if (label.empty() || !file || !file[0]) return false;
        auto* link = new (std::nothrow) CpNavLink{};
        if (!link) return false;
        link->listType = listType;
        link->pszName = AllocCoTaskStr(label.c_str());
        link->cmd.cmdType = 1u;  // SHELL
        link->cmd.lpFile = AllocCoTaskStr(file);
        link->cmd.lpParameters = (params && params[0]) ? AllocCoTaskStr(params) : nullptr;
        if (!link->pszName || !link->cmd.lpFile || (params && params[0] && !link->cmd.lpParameters)) {
            DestroyNavLinkCallback(link, nullptr);
            return false;
        }
        if (dpa.insertPtr(hdpa, 0x7fffffff, link) < 0) {
            DestroyNavLinkCallback(link, nullptr);
            return false;
        }
        return true;
    };

    const std::wstring evParams = ResolveLocationActivityParams();
    const bool ok1 = addShellLink(
        0u, 225u, L"View location activity",
        L"%SystemRoot%\\System32\\eventvwr.exe",
        evParams.empty() ? nullptr : evParams.c_str());
    const bool ok2 = addShellLink(
        1u, 224u, L"Default Location",
        L"ms-settings:privacy-location",
        nullptr);

    if (!ok1 && !ok2) {
        dpa.destroyCb(hdpa, &DestroyNavLinkCallback, nullptr);
        pViewBag->Release();
        return;
    }

    auto* pLinks = new (std::nothrow) CWin7NavLinks(hdpa, dpa.destroyCb);
    if (!pLinks) {
        dpa.destroyCb(hdpa, &DestroyNavLinkCallback, nullptr);
        pViewBag->Release();
        return;
    }

    // Write VT_UNKNOWN so shell32 CControlPanelNavModuleInner::Notify synchronously
    // deep-copies our links, then immediately write VT_EMPTY so pViewBag releases its
    // reference before LayoutInitialized returns (zero dangling objects on mod unload).
    VARIANT vLinks{};
    vLinks.vt = VT_UNKNOWN;
    vLinks.punkVal = static_cast<IUnknown*>(pLinks);
    const HRESULT hrWrite = pViewBag->Write(L"ControlPanelNavLinks", &vLinks);

    if (SUCCEEDED(hrWrite)) {
        VARIANT vEmpty{};
        vEmpty.vt = VT_EMPTY;
        pViewBag->Write(L"ControlPanelNavLinks", &vEmpty);
    }

    Wh_Log(L"[loc] Published Win7 nav links (hr=0x%08X, consumed=%d)",
           static_cast<unsigned>(hrWrite), pLinks->WasRead() ? 1 : 0);

    pLinks->Release();
    pViewBag->Release();
}

static HRESULT WINAPI IUnknown_QueryService_Shim(IUnknown* punk, REFGUID guidService, REFIID riid, void** ppvOut) {
    const IUnknown_QueryService_t orig = g_queryService_Orig.load(std::memory_order_acquire);
    if (!orig) return E_UNEXPECTED;
    const HRESULT hr = orig(punk, guidService, riid, ppvOut);
    try {
        if (SUCCEEDED(hr) && punk &&
            g_showWin7NavLinks.load(std::memory_order_acquire) &&
            !g_shuttingDown.load(std::memory_order_acquire) &&
            IsEqualGUID(guidService, kSidControlPanelPagesState) &&
            IsEqualGUID(riid, kIidPropertyBag)) {
            PublishWin7NavLinks(punk, orig);
        }
    } catch (...) {
    }
    return hr;
}

static bool WriteIatSlotLocked(void** slot, void* newFn, void*& outOrig) {
    for (const PatchedIatSlot& existing : g_patchedSlots) {
        if (existing.slot == slot) {
            outOrig = existing.origFn;
            return true;
        }
    }
    DWORD oldProt = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProt)) return false;
    void* prev = InterlockedExchangePointer(slot, newFn);
    DWORD dummy = 0;
    VirtualProtect(slot, sizeof(void*), oldProt, &dummy);
    g_patchedSlots.push_back({slot, prev});
    outOrig = prev;
    return true;
}

static void PatchSensorsCplIat(HMODULE hCpl, bool hookExplorer) {
    if (!hCpl) return;
    auto* base = reinterpret_cast<uint8_t*>(hCpl);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return;

    const IMAGE_DATA_DIRECTORY& impDir =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!impDir.VirtualAddress || !impDir.Size) return;

    std::lock_guard<std::mutex> lk(g_iatMutex);
    if (g_shuttingDown.load(std::memory_order_acquire)) return;
    int regPatched = 0;
    int olePatched = 0;
    int shlwapiPatched = 0;

    for (const auto* desc = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + impDir.VirtualAddress);
         desc->Name != 0; ++desc) {
        const char* dllName = reinterpret_cast<const char*>(base + desc->Name);
        const bool isKernel32 = (_stricmp(dllName, "KERNEL32.dll") == 0);
        const bool isOle32 = (_stricmp(dllName, "ole32.dll") == 0);
        const bool isShlwapi = (_stricmp(dllName, "SHLWAPI.dll") == 0);
        if (!isKernel32 && (!hookExplorer || (!isOle32 && !isShlwapi))) continue;

        const DWORD intRva = desc->OriginalFirstThunk ? desc->OriginalFirstThunk : desc->FirstThunk;
        if (!intRva || !desc->FirstThunk) continue;
        const auto* origThunk = reinterpret_cast<const IMAGE_THUNK_DATA64*>(base + intRva);
        auto* iatThunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(base + desc->FirstThunk);

        for (; origThunk->u1.AddressOfData != 0; ++origThunk, ++iatThunk) {
            void** slot = reinterpret_cast<void**>(&iatThunk->u1.Function);
            if (IMAGE_SNAP_BY_ORDINAL64(origThunk->u1.Ordinal)) {
                if (isShlwapi && hookExplorer && IMAGE_ORDINAL64(origThunk->u1.Ordinal) == 176u) {
                    void* orig = nullptr;
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&IUnknown_QueryService_Shim), orig)) {
                        g_queryService_Orig.store(reinterpret_cast<IUnknown_QueryService_t>(orig),
                                                  std::memory_order_release);
                        ++shlwapiPatched;
                    }
                }
                continue;
            }
            const auto* ibn =
                reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(base + origThunk->u1.AddressOfData);
            const char* fn = reinterpret_cast<const char*>(ibn->Name);

            if (isKernel32) {
                void* orig = nullptr;
                if (strcmp(fn, "RegOpenKeyExW") == 0) {
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&RegOpenKeyExW_Shim), orig)) {
                        g_regOpen_Orig.store(reinterpret_cast<RegOpenKeyExW_t>(orig), std::memory_order_release);
                        ++regPatched;
                    }
                } else if (strcmp(fn, "RegCreateKeyExW") == 0) {
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&RegCreateKeyExW_Shim), orig)) {
                        g_regCreate_Orig.store(reinterpret_cast<RegCreateKeyExW_t>(orig), std::memory_order_release);
                        ++regPatched;
                    }
                } else if (strcmp(fn, "RegQueryInfoKeyW") == 0) {
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&RegQueryInfoKeyW_Shim), orig)) {
                        g_regQueryInfo_Orig.store(reinterpret_cast<RegQueryInfoKeyW_t>(orig), std::memory_order_release);
                        ++regPatched;
                    }
                } else if (strcmp(fn, "RegEnumKeyExW") == 0) {
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&RegEnumKeyExW_Shim), orig)) {
                        g_regEnum_Orig.store(reinterpret_cast<RegEnumKeyExW_t>(orig), std::memory_order_release);
                        ++regPatched;
                    }
                } else if (strcmp(fn, "RegQueryValueExW") == 0) {
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&RegQueryValueExW_Shim), orig)) {
                        g_regQuery_Orig.store(reinterpret_cast<RegQueryValueExW_t>(orig), std::memory_order_release);
                        ++regPatched;
                    }
                } else if (strcmp(fn, "RegSetKeySecurity") == 0) {
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&RegSetKeySecurity_Shim), orig)) {
                        g_regSetSec_Orig.store(reinterpret_cast<RegSetKeySecurity_t>(orig), std::memory_order_release);
                        ++regPatched;
                    }
                } else if (strcmp(fn, "RegCloseKey") == 0) {
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&RegCloseKey_Shim), orig)) {
                        g_regClose_Orig.store(reinterpret_cast<RegCloseKey_t>(orig), std::memory_order_release);
                        ++regPatched;
                    }
                }
            } else if (isOle32 && hookExplorer && strcmp(fn, "CoGetObject") == 0) {
                void* orig = nullptr;
                if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&CoGetObject_Shim), orig)) {
                    g_coGetObject_Orig.store(reinterpret_cast<CoGetObject_t>(orig), std::memory_order_release);
                    ++olePatched;
                }
            }
        }
    }

    Wh_Log(L"[loc] Patched private s8cpl IAT (reg=%d, ole=%d, shlwapi=%d)", regPatched, olePatched, shlwapiPatched);
}

static void RestorePatchedIats() {
    {
        std::lock_guard<std::mutex> lk(g_iatMutex);
        for (const PatchedIatSlot& p : g_patchedSlots) {
            DWORD oldProt = 0;
            if (VirtualProtect(p.slot, sizeof(void*), PAGE_READWRITE, &oldProt)) {
                InterlockedExchangePointer(p.slot, p.origFn);
                DWORD dummy = 0;
                VirtualProtect(p.slot, sizeof(void*), oldProt, &dummy);
            }
        }
        g_patchedSlots.clear();
    }
    std::lock_guard<std::mutex> lk(g_regTrackMutex);
    g_regTrack.clear();
}

static void RunElevatedPipeClient(const std::wstring& token) {
    const std::wstring pipeName = L"\\\\.\\pipe\\" + token;
    HANDLE hPipe = CreateFileW(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                               OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
    if (hPipe == INVALID_HANDLE_VALUE) {
        Wh_Log(L"[loc-helper] CreateFileW(%s) failed (%lu)", pipeName.c_str(), GetLastError());
        return;
    }
    DWORD mode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(hPipe, &mode, nullptr, nullptr);

    const HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ISensorsCPLChangeDevicePermission* perm = nullptr;
    IClassFactory* cf = nullptr;
    if (SUCCEEDED(vreg::GetOurClassObject(kGuidPermLua, kIidClassFactory, reinterpret_cast<void**>(&cf))) && cf) {
        ISensorsCPLChangeDevicePermission* raw = nullptr;
        if (SUCCEEDED(cf->CreateInstance(nullptr, kGuidPermIidAndPs, reinterpret_cast<void**>(&raw))) && raw) {
            perm = WrapElevatedPerm(raw);
            raw->Release();
        }
        cf->Release();
    }

    for (int i = 0; i < 8; ++i) {
        PipeReq req{};
        if (!PipeReadExact(hPipe, &req, sizeof(req), 15000u)) break;
        if (req.op == kPipeOpQuit) break;
        PipeResp resp{E_FAIL};
        if (perm) {
            if (req.op == kPipeOpSetOverride) {
                resp.hr = perm->SetLocationOverridePermission(req.val != 0 ? TRUE : FALSE);
            } else if (req.op == kPipeOpSetProvider) {
                resp.hr = perm->SetLocationProviderCSFlag(req.val != 0 ? TRUE : FALSE);
            }
        }
        if (!PipeWriteExact(hPipe, &resp, sizeof(resp), 15000u)) break;
    }

    if (perm) perm->Release();
    if (SUCCEEDED(hrCo)) CoUninitialize();
    CloseHandle(hPipe);
}

}  // namespace loc_shim

// ============================================================================
// 7. Payload Setup & Multi-Process Helpers
// ============================================================================

static void PublishRegistration(const std::wstring& binDir, bool nameOnly = false) {
    vreg::SetTable(vreg::BuildTable(binDir, CurrentLang(), g_showTasks.load(std::memory_order_acquire), nameOnly));
}

static void UnpublishRegistration() {
    vreg::SetTable(nullptr);
    vreg::ForgetAllHandles();
    vreg::ReleaseScratchRoot();
}

static bool LoadVerifiedLocationModules(const std::wstring& binDir, const Bytes& expectedDui,
                                        const std::wstring& cplPath, const Bytes& expectedCpl) {
    const std::wstring duiPath = binDir + L"\\s8dui.dll";
    HMODULE hDui = g_hDui.load(std::memory_order_acquire);
    if (!hDui) {
        hDui = LoadVerifiedModule(duiPath, expectedDui);
        if (!hDui) return false;
        HMODULE pinned = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                           reinterpret_cast<LPCWSTR>(hDui), &pinned);
        g_hDui.store(hDui, std::memory_order_release);
    }
    HMODULE hCpl = LoadVerifiedModule(cplPath, expectedCpl);
    if (!hCpl) return false;
    HMODULE pinnedCpl = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                       reinterpret_cast<LPCWSTR>(hCpl), &pinnedCpl);
    g_hSensorsCpl.store(hCpl, std::memory_order_release);
    if (g_role == ProcessRole::Explorer || g_role == ProcessRole::DllHost ||
        g_role == ProcessRole::ElevatedHelper) {
        loc_shim::PatchSensorsCplIat(hCpl, g_role == ProcessRole::Explorer);
    }
    return true;
}

static bool BuildAndWriteBinaries(const std::wstring& binDir, const Bytes& rawSensors, const Bytes& rawDui,
                                  const Bytes& rawDuser, Lang lang, Bytes& outPatchedSensors,
                                  std::wstring& outCplPath) {
    outCplPath = binDir + L"\\" + CplDllFileName(lang);
    std::string err;
    if (!gen::BuildPatchedSensorsCpl(rawSensors, rawDuser, lang, outPatchedSensors, err)) {
        Wh_Log(L"BuildPatchedSensorsCpl failed: %S", err.c_str());
        return false;
    }
    if (!WriteFileAtomic(binDir + L"\\s8dui.dll", rawDui)) {
        Wh_Log(L"Failed to write s8dui.dll");
        return false;
    }
    if (!WriteFileAtomic(outCplPath, outPatchedSensors)) {
        Wh_Log(L"Failed to write %s", outCplPath.c_str());
        return false;
    }
    WriteFileAtomic(binDir + L"\\" + TasksXmlFileName(lang), gen::BuildTasksXmlUtf16(lang));
    return true;
}

static void RunExplorerSetup() {
    const std::wstring dir = StorageDir();
    if (dir.empty()) return;
    const std::wstring dlDir = dir + L"\\dl";
    const std::wstring binDir = dir + L"\\bin";

    HANDLE mtx = CreateMutexW(nullptr, FALSE, L"Windhawk.Win81LocationSettingsRestorer.Setup");
    if (mtx) {
        HANDLE hs[] = {mtx, g_stopEvent};
        const DWORD wait = WaitForMultipleObjects(g_stopEvent ? 2u : 1u, hs, FALSE, 120000u);
        if (wait == WAIT_OBJECT_0 + 1) { CloseHandle(mtx); return; }
    }
    auto releaseMtx = [&]() { if (mtx) { ReleaseMutex(mtx); CloseHandle(mtx); } };

    EnsureDir(dlDir);
    EnsureDir(binDir);
    for (const LangTable& lt : kLangTables) {
        DeleteFileW((binDir + L"\\" + LegacyCplDllFileName(lt.lang)).c_str());
    }

    Bytes rawSensors, rawDui, rawDuser;
    if (g_shuttingDown.load() || !GetVerifiedFile(dlDir, kFiles[0], rawSensors) ||
        g_shuttingDown.load() || !GetVerifiedFile(dlDir, kFiles[1], rawDui) ||
        g_shuttingDown.load() || !GetVerifiedFile(dlDir, kFiles[2], rawDuser)) {
        Wh_Log(L"Setup aborted: unable to obtain verified Microsoft binaries from msdl.microsoft.com");
        releaseMtx();
        return;
    }

    const Lang lang = CurrentLang();
    Bytes patchedSensors;
    std::wstring cplPath;
    if (!BuildAndWriteBinaries(binDir, rawSensors, rawDui, rawDuser, lang, patchedSensors, cplPath)) {
        releaseMtx();
        return;
    }
    releaseMtx();

    if (g_shuttingDown.load()) return;
    if (!LoadVerifiedLocationModules(binDir, rawDui, cplPath, patchedSensors)) {
        Wh_Log(L"Failed to load verified private Location Settings modules");
        return;
    }

    PublishRegistration(binDir, false);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    Wh_Log(L"Windows 8.1 Location Settings applet active (%s)", GetLangTable(lang).code);
}

static bool LoadStagedPayload(const std::wstring& binDir, const std::wstring& dlDir) {
    Bytes rawSensors, rawDui, rawDuser;
    if (!ReadPinnedFile(dlDir + L"\\" + kFiles[0].name, kFiles[0], rawSensors) ||
        !ReadPinnedFile(dlDir + L"\\" + kFiles[1].name, kFiles[1], rawDui) ||
        !ReadPinnedFile(dlDir + L"\\" + kFiles[2].name, kFiles[2], rawDuser)) {
        Wh_Log(L"[elev] pinned downloads not ready");
        return false;
    }
    const Lang lang = CurrentLang();
    const std::wstring cplPath = binDir + L"\\" + CplDllFileName(lang);
    Bytes expectedSensors;
    std::string err;
    if (!gen::BuildPatchedSensorsCpl(rawSensors, rawDuser, lang, expectedSensors, err)) {
        Wh_Log(L"[elev] deterministic rebuild failed: %S", err.c_str());
        return false;
    }
    if (!LoadVerifiedLocationModules(binDir, rawDui, cplPath, expectedSensors)) {
        Wh_Log(L"[elev] verified module load failed");
        return false;
    }
    return true;
}

static std::wstring ExtractPipeTokenFromCmdLine(const std::wstring& cmdLine) {
    const std::wstring lo = LowerStr(cmdLine);
    const std::wstring flag = L"--wh-loc-pipe=";
    const size_t pos = lo.find(flag);
    if (pos == std::wstring::npos) return {};
    size_t start = pos + flag.size();
    size_t end = start;
    while (end < cmdLine.size() && cmdLine[end] != L' ' && cmdLine[end] != L'\t' && cmdLine[end] != L'"') {
        ++end;
    }
    return cmdLine.substr(start, end - start);
}

static bool IsProcessAmd64() {
#if defined(_M_AMD64) || defined(__x86_64__)
    return true;
#else
    return false;
#endif
}

#endif  // LOC_GENERATOR_ONLY

}  // namespace

// ============================================================================
// 8. Windhawk Entry Points (external linkage)
// ============================================================================

#ifndef LOC_GENERATOR_ONLY

BOOL Wh_ModInit() {
    try {
        if (!IsProcessAmd64()) return FALSE;
        LoadSettings();

        wchar_t exe[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exe, MAX_PATH);
        const wchar_t* exeName = wcsrchr(exe, L'\\');
        exeName = exeName ? exeName + 1 : exe;
        const std::wstring rawCmd = GetCommandLineW() ? GetCommandLineW() : L"";
        const std::wstring cmdLine = LowerStr(rawCmd);

        std::wstring pipeToken;
        if (_wcsicmp(exeName, L"explorer.exe") == 0) {
            g_role = ProcessRole::Explorer;
        } else if (_wcsicmp(exeName, L"control.exe") == 0) {
            g_role = ProcessRole::Control;
        } else if (_wcsicmp(exeName, L"dllhost.exe") == 0 &&
                   cmdLine.find(L"6ce51f75-0448-438e-b9ca-69c352a248a7") != std::wstring::npos) {
            g_role = ProcessRole::DllHost;
        } else if (_wcsicmp(exeName, L"svchost.exe") == 0 &&
                   (cmdLine.find(L"rpcss") != std::wstring::npos ||
                    cmdLine.find(L"dcomlaunch") != std::wstring::npos ||
                    cmdLine.find(L"appinfo") != std::wstring::npos)) {
            g_role = ProcessRole::ComService;
        } else if (_wcsicmp(exeName, L"rundll32.exe") == 0) {
            pipeToken = ExtractPipeTokenFromCmdLine(rawCmd);
            if (pipeToken.empty()) return FALSE;
            g_role = ProcessRole::ElevatedHelper;
        } else {
            return FALSE;
        }

        const std::wstring dir = StorageDir();
        if (dir.empty()) return FALSE;
        const std::wstring dlDir = dir + L"\\dl";
        const std::wstring binDir = dir + L"\\bin";

        if (g_role == ProcessRole::ElevatedHelper) {
            if (LoadStagedPayload(binDir, dlDir)) {
                loc_shim::RunElevatedPipeClient(pipeToken);
                loc_shim::RestorePatchedIats();
            }
            ExitProcess(0);
        }

        if (g_role == ProcessRole::ComService) {
            vreg::CleanupStaleHives();
            if (!vreg::InstallHooks(false)) return FALSE;
            PublishRegistration(binDir, false);
            Wh_Log(L"[com-service] read-only Location Settings COM/elevation view published");
            return TRUE;
        }

        if (g_role == ProcessRole::Control) {
            const std::wstring cplPath = binDir + L"\\" + CplDllFileName(CurrentLang());
            if (GetFileAttributesW(cplPath.c_str()) == INVALID_FILE_ATTRIBUTES) return FALSE;
            vreg::CleanupStaleHives();
            if (!vreg::InstallHooks(false)) return FALSE;
            PublishRegistration(binDir, true);
            return TRUE;
        }

        if (g_role == ProcessRole::DllHost) {
            if (!LoadStagedPayload(binDir, dlDir)) return FALSE;
            vreg::CleanupStaleHives();
            if (!vreg::InstallHooks(true)) return FALSE;
            PublishRegistration(binDir, false);
            Wh_Log(L"[dllhost] verified elevated Location Settings host ready");
            return TRUE;
        }

        // Explorer: install hooks immediately, download/build/load asynchronously.
        vreg::CleanupStaleHives();
        g_shuttingDown.store(false);
        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!vreg::InstallHooks(true)) return FALSE;
        g_setupThread.emplace([] {
            try { RunExplorerSetup(); } catch (...) { Wh_Log(L"Explorer setup threw an exception"); }
        });
        return TRUE;
    } catch (...) {
        Wh_Log(L"Wh_ModInit threw an exception");
        return FALSE;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    try {
        const Lang oldLang = CurrentLang();
        const bool oldTasks = g_showTasks.load(std::memory_order_acquire);
        LoadSettings();
        const Lang newLang = CurrentLang();
        const bool newTasks = g_showTasks.load(std::memory_order_acquire);
        const bool langChanged = (oldLang != newLang);
        const bool tasksChanged = (oldTasks != newTasks);

        if (bReload) *bReload = langChanged ? TRUE : FALSE;

        const std::wstring dir = StorageDir();
        if (dir.empty()) return TRUE;
        const std::wstring dlDir = dir + L"\\dl";
        const std::wstring binDir = dir + L"\\bin";

        if (g_role == ProcessRole::Explorer && (langChanged || tasksChanged)) {
            Bytes rawSensors, rawDui, rawDuser;
            if (ReadPinnedFile(dlDir + L"\\" + kFiles[0].name, kFiles[0], rawSensors) &&
                ReadPinnedFile(dlDir + L"\\" + kFiles[1].name, kFiles[1], rawDui) &&
                ReadPinnedFile(dlDir + L"\\" + kFiles[2].name, kFiles[2], rawDuser)) {
                Bytes patchedSensors;
                std::wstring cplPath;
                if (BuildAndWriteBinaries(binDir, rawSensors, rawDui, rawDuser, newLang, patchedSensors, cplPath) && langChanged) {
                    LoadVerifiedLocationModules(binDir, rawDui, cplPath, patchedSensors);
                }
            }
        }

        if (langChanged || tasksChanged) {
            PublishRegistration(binDir, g_role == ProcessRole::Control);
            if (g_role == ProcessRole::Explorer) {
                SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
            }
        }
        return TRUE;
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged threw an exception");
        return FALSE;
    }
}

void Wh_ModBeforeUninit() {
    try {
        if (g_role == ProcessRole::DllHost) {
            loc_shim::RestorePatchedIats();
            return;
        }
        if (g_role != ProcessRole::Explorer) return;
        g_shuttingDown.store(true);
        loc_shim::RestorePatchedIats();
        if (g_stopEvent) SetEvent(g_stopEvent);
        CancelInFlightDownload();
        if (g_setupThread && g_setupThread->joinable()) g_setupThread->join();
        g_setupThread.reset();
        loc_shim::RestorePatchedIats();
    } catch (...) {
        Wh_Log(L"Wh_ModBeforeUninit threw an exception");
    }
}

void Wh_ModUninit() {
    try {
        bool keepFiles = true;
        if (g_role == ProcessRole::DllHost) {
            loc_shim::RestorePatchedIats();
        } else if (g_role == ProcessRole::Explorer) {
            loc_shim::RestorePatchedIats();
            keepFiles = Wh_GetIntSetting(L"keepDownloadedFiles") != 0;
            if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
        }
        UnpublishRegistration();

        if (g_role == ProcessRole::Explorer) {
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
            if (!keepFiles) {
                const std::wstring dir = StorageDir();
                if (!dir.empty()) {
                    const std::wstring dlDir = dir + L"\\dl";
                    const std::wstring binDir = dir + L"\\bin";
                    DeleteFileW((binDir + L"\\s8dui.dll").c_str());
                    for (const LangTable& lt : kLangTables) {
                        DeleteFileW((binDir + L"\\" + CplDllFileName(lt.lang)).c_str());
                        DeleteFileW((binDir + L"\\" + LegacyCplDllFileName(lt.lang)).c_str());
                        DeleteFileW((binDir + L"\\" + TasksXmlFileName(lt.lang)).c_str());
                    }
                    for (const FileSpec& f : kFiles) {
                        DeleteFileW((dlDir + L"\\" + f.name).c_str());
                    }
                    RemoveDirectoryW(binDir.c_str());
                    RemoveDirectoryW(dlDir.c_str());
                }
            }
        }
        g_role = ProcessRole::Other;
    } catch (...) {
        Wh_Log(L"Wh_ModUninit threw an exception");
    }
}

#else
// Standalone host self-test harness (compiled only when -DLOC_GENERATOR_ONLY is defined)
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: loc_gen_test <sensorscpl.dll> <duser.dll> <out_dir>\n";
        return 2;
    }
    auto readAll = [](const char* p) -> Bytes {
        std::ifstream f(p, std::ios::binary);
        return Bytes((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    };
    const Bytes rawSensors = readAll(argv[1]);
    const Bytes rawDuser = readAll(argv[2]);
    std::cout << "Input SHA-256 sensorscpl=" << sha256::Hex(rawSensors)
              << " duser=" << sha256::Hex(rawDuser) << "\n";
    for (const LangTable& lt : kLangTables) {
        const std::string code(lt.code, lt.code + wcslen(lt.code));
        Bytes outDll;
        std::string err;
        if (!gen::BuildPatchedSensorsCpl(rawSensors, rawDuser, lt.lang, outDll, err)) {
            std::cerr << "FAIL [" << code << "]: " << err << "\n";
            return 1;
        }
        const std::string outPath = std::string(argv[3]) + "/s8cpl2-" + code + ".dll";
        std::ofstream of(outPath, std::ios::binary);
        of.write(reinterpret_cast<const char*>(outDll.data()), static_cast<std::streamsize>(outDll.size()));
        const Bytes tasks = gen::BuildTasksXmlUtf16(lt.lang);
        const std::string tPath = std::string(argv[3]) + "/LocationSettings-" + code + ".tasks.xml";
        std::ofstream tf(tPath, std::ios::binary);
        tf.write(reinterpret_cast<const char*>(tasks.data()), static_cast<std::streamsize>(tasks.size()));
        std::cout << "OK [" << code << "] dll=" << outDll.size() << " B sha256=" << sha256::Hex(outDll)
                  << " tasks=" << tasks.size() << " B\n";
    }
    return 0;
}
#endif
