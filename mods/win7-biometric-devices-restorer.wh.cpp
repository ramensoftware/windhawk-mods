// ==WindhawkMod==
// @id              win7-biometric-devices-restorer
// @name            Windows 7 Biometric Devices Applet Restorer
// @description     This mod restores the Windows 7 Biometric Devices Control Panel applet on Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @include         dllhost.exe
// @include         svchost.exe
// @architecture    x86-64
// @compilerOptions -lwininet -ladvapi32 -lole32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Biometric Devices Applet Restorer

This mod restores the classic Windows 7 Biometric Devices Control Panel applet that was removed from modern versions of Windows.

**IMPORTANT:** `svchost.exe` must be included in the mod's target processes for this mod to start and work properly.

### What the mod does inside `svchost.exe`

The mod only activates in the service hosts that run the **RpcSs** (RPC / COM), **DcomLaunch** and **Appinfo** (UAC) services; it does nothing in any other `svchost.exe` instance. In those hosts it hooks the registry read/open functions and *serves* a few registrations to COM and UAC that do not exist in the real registry (nothing is written to the registry):

- a "Virtual Factory" class `{0142E4D1-FB7A-11DC-BA4A-000FFE7AB428}` with `Elevation\Enabled=1`, served by `shpafact.dll`;
- an AppID for that class with a `DllSurrogate`;
- the `IWinBioPolicy` interface and its type library.

This is needed **only** for the elevated "Change biometric settings" page: COM elevation starts an elevated `dllhost.exe`, which loads the mod's private `biocpl-<lang>.dll` from the mod's storage folder. The rest of the applet works without it, but the mod will not start unless `svchost.exe` is included.

## Screenshot

![Biometric Devices](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/biometricdevice.png)

## About

The mod downloads the original Windows 7 binaries from Microsoft's public symbol server (`msdl.microsoft.com`, over HTTPS, using the system proxy settings) on first use, verifies them with pinned SHA-256 hashes, applies the necessary compatibility patches, and registers the applet virtually so that no permanent system files or registry keys are modified. This mod has been tested on Windows 11 24H2.
It supports 6 languages: English, Italian, French, Spanish, Portuguese, and German.
The language can follow the Windows UI language or be set to a specific locale.

The applet appears under **Hardware and Sound** in Control Panel Category view, in Large / Small Icons view, in Control Panel Search, and can also be opened directly with `control /name Microsoft.BiometricDevices`.

Network access: the mod sends three fixed HTTPS GET requests, for `biocpl.dll`, `dui70.dll` and `duser.dll` under `https://msdl.microsoft.com/download/symbols/` (the server may redirect to Microsoft's blob storage). The requests carry no user data, only the user agent `Microsoft-Symbol-Server/10.0.0.0`. The downloaded files are checked against a pinned size and SHA-256 before use. The download is performed whenever the mod has no verified cached copy: on first use, and again after the mod is disabled if "Keep cached Microsoft binaries when disabled" is off. There is no setting that turns the download off.

Architecture: only 64-bit Windows on x64 is supported. The Windows 7 binaries are x64-only and cannot be loaded into the native ARM64 Explorer, so the mod is not available on ARM64.

Note: an internet connection is required on first use, and it might take a moment to load the applet while the mod downloads the required dependencies. If the download fails (for example, no network when Explorer starts), the mod retries by itself every few minutes (2 minutes, doubling up to 30 minutes). After the first-time download completes, refresh or reopen Control Panel.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $description: This setting selects the display language for the Biometric Devices applet using authentic Windows 7 translations. "Automatic" follows the Windows UI language.
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
  $description: This setting controls whether the classic blue task links are displayed under Biometric Devices in Category view and Control Panel Search.
- keepDownloadedFiles: true
  $name: Keep cached Microsoft binaries when disabled
  $description: This setting keeps the downloaded Microsoft binaries in mod storage when the mod is disabled so they do not need to be downloaded again when re-enabling the mod.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#ifndef BIO_GENERATOR_ONLY
#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <setupapi.h>
#include <unknwn.h>
#endif

namespace {

using Bytes = std::vector<uint8_t>;

// ============================================================================
// 1. Supported Languages & Authentic Windows 7 SP1 Microsoft String Tables
//    Extracted verbatim from official KB2483139 Win7 SP1 x64 Language Packs
//    (biocpl.dll.mui: 91 strings; duser.dll.mui: 4 strings remapped 1100->2000)
// ============================================================================

enum class Lang : int {
    En = 0,
    It = 1,
    Fr = 2,
    Es = 3,
    Pt = 4,
    De = 5,
};

struct StringEntry {
    uint16_t id;
    const wchar_t* text;
};

static const StringEntry kStringsEn[] = {
    {1u, L"Biometric Devices"},
    {2u, L"Use a fingerprint reader to log on to Windows or with programs that work with biometrics."},
    {101u, L"Biometric Devices"},
    {102u, L"Welcome to Biometric Devices"},
    {103u, L"Select by Touch"},
    {104u, L"Confirm Delete"},
    {105u, L"Message"},
    {106u, L"Change settings"},
    {120u, L"Biometrics"},
    {121u, L"Use your fingerprint with Windows"},
    {122u, L"Manage your fingerprint data"},
    {123u, L"Remove your fingerprint data"},
    {124u, L"Stop using your fingerprint to log on to Windows"},
    {125u, L"Change biometric settings"},
    {126u, L"Device Manager"},
    {127u, L"User Accounts"},
    {130u, L"Use biometric devices with Windows"},
    {131u, L"You can use a fingerprint reader to log on to Windows. Some programs also offer features that work with biometrics."},
    {132u, L"Windows did not find any biometric devices on this computer. You might need to attach your device or install device drivers."},
    {133u, L"You cannot use biometric devices with this user account. To use biometrics, log on to a different user account."},
    {134u, L"The Windows Biometric Service is not running on this computer. Contact your system administrator."},
    {135u, L"Biometrics is turned off on this computer. To use biometrics, click 'Change biometric settings' and then click 'Biometrics on.'"},
    {140u, L"A fingerprint reader, which is a biometric device, is intended to give you quick, convenient access to your computer.\n\nFingerprint readers vary in security capabilities and it might not be appropriate to use your fingerprint reader to protect data that you wish to keep private, such as personal financial information, or to access sensitive resources, such as corporate networks. You can contact your fingerprint reader provider or fingerprint software provider for additional information regarding the appropriate use of your device.\n\nYou can best protect private and sensitive information on your computer by using technologies that leverage hardware security devices, such as Trusted Platform Modules and smart cards, to enable the secure encryption of your data, and by limiting access with a strong password that you memorize, store in a secure place, or otherwise keep private in a manner that works for you, regardless of whether you use a fingerprint reader. You can also encrypt your data. Some editions of Windows include Encrypting File System to encrypt individual files and folders and BitLocker Drive Encryption to encrypt drives."},
    {141u, L"Enrolled"},
    {142u, L"Not enrolled"},
    {143u, L"Locked by a program"},
    {144u, L"Unavailable"},
    {145u, L"Enrollment in progress"},
    {146u, L"Why is my device locked?"},
    {147u, L"Why isn't my device available?"},
    {148u, L"Why isn't my device available?"},
    {150u, L"There are no fingerprint readers available at this time"},
    {151u, L"There are no fingerprint readers attached to this computer or all the fingerprint readers are being used by programs."},
    {152u, L"Biometrics Disabled"},
    {153u, L"Biometrics is turned off on this computer. To turn on biometrics, click Change biometric settings in the Biometrics Control Panel."},
    {154u, L"Biometrics Disabled"},
    {155u, L"Your system administrator has turned off biometrics on this computer. To turn on biometrics, contact your system administrator."},
    {156u, L"You have no fingerprint data or logon credentials to remove"},
    {170u, L"Are you sure you want to delete your fingerprint data?"},
    {171u, L"You will be prompted to enter your Windows password before your fingerprint data is deleted.\nAfter your fingerprint data is deleted, you will need to use your Windows password to log on to this computer.  If you don\u2019t remember your Windows password, you should change it now before you delete your fingerprint data."},
    {172u, L"Are you sure you want to delete fingerprint data for all users?"},
    {173u, L"After the fingerprint data is deleted, all users will need to use their Windows password to log on to this computer. Before deleting fingerprint data, make sure all users remember their Windows password. If a user does not remember their Windows password, you should have them change it before you delete their fingerprint data."},
    {174u, L"Are you sure you want to disable using your fingerprint to log into Windows?"},
    {175u, L"After disabling fingerprint based logon, all users will need to use their Windows password to log onto this computer. Before disabling this feature, make sure all users remember their Windows password. If a user does not remember their Windows password, you should have them change it before you disable this feature."},
    {176u, L"Are you sure you want to disable using your fingerprint to log into a domain?"},
    {177u, L"After disabling fingerprint based logon, all domain users will need to use their Windows password to log onto this computer. Before disabling this feature, make sure all users remember their Windows password. If a user does not remember their Windows password, you should have them change it before you disable this feature."},
    {178u, L"Confirm your current password"},
    {179u, L"Stop using your fingerprint to log on to Windows"},
    {180u, L"Some settings are managed by your system administrator. <a>Why can't I change some settings?</a>"},
    {181u, L"Some settings are managed by your system administrator. Why can't I change some settings?"},
    {182u, L"This program is blocked by group policy. For more information, contact your system administrator."},
    {190u, L"Off"},
    {191u, L"On"},
    {200u, L"More than one fingerprint reader was found. Touch the scanning surface of the fingerprint reader that you want to use."},
    {201u, L"An enrollment is already in progress. Finish the enrollment in progress and try again."},
    {202u, L"Could not detect which fingerprint reader was touched. Try again."},
    {210u, L"The selected fingerprint reader has no management application installed"},
    {211u, L"This computer does not have a program that can be used to manage your fingerprint data with this fingerprint reader. To use the fingerprint reader, you need to get a program from your computer manufacturer (if the fingerprint reader is part of your computer) or the fingerprint reader manufacturer."},
    {212u, L"A fingerprint management application is already running"},
    {213u, L"Close any open fingerprint management applications and then try again."},
    {531u, L"Biometric Devices"},
    {538u, L"Properties"},
    {540u, L"Welcome to Biometric Devices"},
    {542u, L"Con&tinue"},
    {543u, L"&Cancel"},
    {551u, L"Select the fingerprint reader that you want to use"},
    {552u, L"&Cancel"},
    {554u, L"&Delete"},
    {555u, L"&Cancel"},
    {557u, L"&Close"},
    {568u, L"&Save Changes"},
    {569u, L"&Cancel"},
    {577u, L"Change biometric settings"},
    {578u, L"These settings control biometrics and fingerprint logon for this computer. If you want to delete biometric data and fingerprint logon information for all users of this computer, turn off biometrics."},
    {581u, L"For information on creating a strong password, see"},
    {582u, L"Tips for creating a strong password"},
    {583u, L"."},
    {584u, L"Biometrics on"},
    {585u, L"Biometrics on"},
    {586u, L"Allow users to log on to Windows using their fingerprints"},
    {587u, L"Allow users to log on to a domain using their fingerprints"},
    {588u, L"Biometrics off"},
    {589u, L"Biometrics off"},
    {590u, L"Turning off any of these settings will delete saved fingerprint logon information.\nTurning off biometrics also deletes fingerprint records for all users."},
    {591u, L"Turning off any of these settings will delete saved fingerprint logon information. Turning off biometrics also deletes fingerprint records for all users."},
    {592u, L"Your fingerprint data could not be deleted. You might not have permission to delete the data,  you are currently using a remote connection, or you are currently enrolling fingerprint data."},
    {593u, L"Your fingerprint data could not be deleted. You might not have permission to delete the data,  you are currently using a remote connection, or you are currently enrolling fingerprint data."},
    {594u, L"There was a failure while saving your settings. You may not have permission to change the settings or you are currently using a remote connection."},
    {595u, L"There was a failure while saving your settings. You may not have permission to change the settings or you are currently using a remote connection."},
    {596u, L"<a>How does a user change their Windows password?</a>"},
    {597u, L"How does a user change their Windows password?"},
    {2000u, L"Used to change vertical viewing area"},
    {2001u, L"Used to change horizontal viewing area"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
};

static const StringEntry kStringsIt[] = {
    {1u, L"Dispositivi biometrici"},
    {2u, L"Utilizzare un lettore di impronta digitale per accedere a Windows o con programmi che utilizzano la biometria."},
    {101u, L"Dispositivi biometrici"},
    {102u, L"Dispositivi biometrici"},
    {103u, L"Seleziona tramite tocco"},
    {104u, L"Conferma eliminazione"},
    {105u, L"Messaggio"},
    {106u, L"Modifica impostazioni"},
    {120u, L"Biometria"},
    {121u, L"Utilizzare le impronte digitali con Windows"},
    {122u, L"Gestire i dati delle impronte digitali"},
    {123u, L"Rimuovere i dati delle impronte digitali"},
    {124u, L"Interrompere l'uso delle impronte digitali per accedere a Windows"},
    {125u, L"Cambia impostazioni biometriche"},
    {126u, L"Gestione dispositivi"},
    {127u, L"Account utente"},
    {130u, L"Utilizza dispositivi biometrici con Windows"},
    {131u, L"\u00C8 possibile utilizzare un lettore di impronta digitale per accedere a Windows. Alcuni programmi offrono inoltre funzionalit\u00E0 che utilizzano la biometria."},
    {132u, L"Impossibile trovare dispositivi biometrici nel computer in uso. Potrebbe essere necessario collegare il dispositivo o installare i driver per il dispositivo."},
    {133u, L"Impossibile utilizzare dispositivi biometrici con questo account utente. Per utilizzare la biometria, effettuare l'accesso con un account utente diverso."},
    {134u, L"Servizio di biometria Windows non \u00E8 in esecuzione nel computer in uso. Contattare l'amministratore del sistema."},
    {135u, L"La biometria \u00E8 disattivata nel computer in uso. Per utilizzare la biometria, fare clic su Cambia impostazioni biometriche, quindi su Biometria attivata."},
    {140u, L"Un lettore di impronta digitale, che \u00E8 un dispositivo biometrico, ha la funzione di offrire un accesso rapido e comodo al computer.\n\nI lettori di impronta digitale variano a seconda delle funzioni di sicurezza e quindi non tutti sono appropriati per proteggere dati che si desidera mantenere riservati, quali le informazioni finanziarie personali, o per accedere a risorse sensibili, quali le reti aziendali. \u00C8 possibile contattare il fornitore del lettore di impronta digitale o il fornitore del software di gestione delle impronte digitali per ulteriori informazioni sull'utilizzo appropriato del dispositivo.\n\n\u00C8 possibile ottenere una migliore protezione delle informazioni riservate e sensibili presenti nel computer utilizzando tecnologie che sfruttano dispositivi di sicurezza hardware, quali Trusted Platform Module e smart card, in grado di proteggere i dati tramite crittografia e limitando l'accesso con una password complessa che \u00E8 possibile memorizzare, archiviare in un luogo sicuro o comunque proteggere nel modo che si ritiene pi\u00F9 opportuno, indipendentemente dal fatto che si utilizzi un lettore di impronta digitale. \u00C8 inoltre possibile crittografare i dati. Alcune edizioni di Windows includono EFS (Encrypting File System) per crittografare singoli file e cartelle e Crittografia unit\u00E0 BitLocker per crittografare unit\u00E0."},
    {141u, L"Registrato"},
    {142u, L"Non registrato"},
    {143u, L"Bloccato da un programma"},
    {144u, L"Non disponibile"},
    {145u, L"Registrazione in corso"},
    {146u, L"Perch\u00E9 il dispositivo \u00E8 bloccato"},
    {147u, L"Perch\u00E9 il dispositivo non \u00E8 disponibile"},
    {148u, L"Perch\u00E9 il dispositivo non \u00E8 disponibile"},
    {150u, L"Nessun lettore di impronta digitale attualmente disponibile"},
    {151u, L"Nessun lettore di impronta digitale collegato al computer oppure tutti i lettori sono utilizzati dai programmi."},
    {152u, L"Biometria disattivata"},
    {153u, L"La biometria \u00E8 disattivata nel computer in uso. Per attivare la biometria, fare clic su Cambia impostazioni biometriche nel pannello di controllo Biometria."},
    {154u, L"Biometria disattivata"},
    {155u, L"L'amministratore del sistema ha disattivato la biometria nel computer in uso. Per attivare la biometria, contattare l'amministratore del sistema."},
    {156u, L"Nessun dato delle impronte digitali o credenziale di accesso da rimuovere"},
    {170u, L"Eliminare i dati delle impronte digitali?"},
    {171u, L"Prima che i dati delle impronte digitali vengano eliminati, verr\u00E0 chiesto di immettere la password di Windows.\nDopo l'eliminazione dei dati delle impronte digitali sar\u00E0 necessario utilizzare la password di Windows per accedere al computer. Se non si ricorda la password di Windows, \u00E8 necessario cambiarla prima di eliminare i dati delle impronte digitali."},
    {172u, L"Eliminare i dati delle impronte digitali di tutti gli utenti?"},
    {173u, L"Dopo l'eliminazione dei dati delle impronte digitali, tutti gli utenti dovranno utilizzare la password di Windows per accedere al computer. Prima di eliminare i dati delle impronte digitali, accertarsi che tutti gli utenti ricordino la propria password di Windows. Se un utente ha dimenticato la password, dovrebbe cambiarla prima che i dati delle impronte digitali vengano eliminati."},
    {174u, L"Disattivare l'uso delle impronte digitali per accedere a Windows?"},
    {175u, L"Dopo la disattivazione dell'accesso basato su impronta digitale, tutti gli utenti dovranno utilizzare la propria password di Windows per accedere a questo computer. Prima di disattivare questa funzionalit\u00E0, assicurarsi che tutti gli utenti ricordino la password di Windows. Se un utente non la ricorda, sar\u00E0 necessario attendere che la cambi prima di poter disattivare questa funzionalit\u00E0."},
    {176u, L"Disattivare l'utilizzo delle impronte digitali per l'accesso a un dominio?"},
    {177u, L"Dopo la disattivazione dell'accesso basato su impronta digitale, tutti gli utenti del dominio dovranno utilizzare la propria password di Windows per accedere a questo computer. Prima di disattivare questa funzionalit\u00E0, assicurarsi che tutti gli utenti ricordino la password di Windows. Se un utente non la ricorda, sar\u00E0 necessario attendere che la cambi prima di poter disattivare questa funzionalit\u00E0."},
    {178u, L"Confermare la password corrente"},
    {179u, L"Interrompere l'uso delle impronte digitali per accedere a Windows"},
    {180u, L"Alcune impostazioni sono gestite dall'amministratore del sistema. <a>Ragioni che impediscono la modifica di alcune impostazioni</a>"},
    {181u, L"Alcune impostazioni sono gestite dall'amministratore del sistema. Ragioni che impediscono la modifica di alcune impostazioni."},
    {182u, L"Questo programma \u00E8 bloccato da Criteri di gruppo. Per ulteriori informazioni, contattare l'amministratore del sistema."},
    {190u, L"Disattivata"},
    {191u, L"Attivata"},
    {200u, L"\u00C8 stato rilevato pi\u00F9 di un lettore di impronta digitale. Toccare la superficie del dispositivo di scansione sul lettore che si desidera utilizzare."},
    {201u, L"\u00C8 gi\u00E0 in corso una registrazione. Terminare la registrazione in corso e riprovare."},
    {202u, L"Impossibile determinare quale lettore di impronta digitale sia stato toccato. Riprovare."},
    {210u, L"Nessuna applicazione di gestione installata per il lettore di impronta digitale selezionato"},
    {211u, L"Il computer non dispone di un programma per gestire i dati delle impronte digitali con questo lettore di impronta digitale. Per utilizzare il lettore, procurarsi un programma dal produttore del computer (se il lettore \u00E8 un componente del computer) o dal produttore del lettore di impronta digitale."},
    {212u, L"Un'applicazione di gestione impronte digitali \u00E8 gi\u00E0 in esecuzione"},
    {213u, L"Chiudere tutte le applicazioni di gestione impronte digitali e riprovare."},
    {531u, L"Dispositivi biometrici"},
    {538u, L"Propriet\u00E0"},
    {540u, L"Dispositivi biometrici"},
    {542u, L"Con&tinua"},
    {543u, L"&Annulla"},
    {551u, L"Selezionare il lettore di impronta digitale che si desidera utilizzare"},
    {552u, L"&Annulla"},
    {554u, L"&Elimina"},
    {555u, L"&Annulla"},
    {557u, L"&Chiudi"},
    {568u, L"&Salva modifiche"},
    {569u, L"&Annulla"},
    {577u, L"Cambia impostazioni biometriche"},
    {578u, L"Queste impostazioni controllano la biometria e l'accesso tramite impronta digitale per il computer in uso. Se si desidera eliminare i dati biometrici e le informazioni per l'accesso tramite impronta digitale per tutti gli utenti del computer, disattivare la biometria."},
    {581u, L"Per informazioni sulla creazione di una password complessa, vedere"},
    {582u, L"Suggerimenti per creare una password complessa"},
    {583u, L"."},
    {584u, L"Biometria attivata"},
    {585u, L"Biometria attivata"},
    {586u, L"Consenti agli utenti di utilizzare le impronte digitali per accedere a Windows"},
    {587u, L"Consenti agli utenti di utilizzare le impronte digitali per accedere a un dominio"},
    {588u, L"Biometria disattivata"},
    {589u, L"Biometria disattivata"},
    {590u, L"La disattivazione di una qualsiasi di queste impostazioni implica l'eliminazione delle informazioni salvate per l'accesso tramite impronta digitale.\nLa disattivazione della biometria implica l'eliminazione dei record relativi alle impronte digitali di tutti gli utenti."},
    {591u, L"La disattivazione di una qualsiasi di queste impostazioni implica l'eliminazione delle informazioni salvate per l'accesso tramite impronta digitale. La disattivazione della biometria implica l'eliminazione dei record relativi alle impronte digitali di tutti gli utenti."},
    {592u, L"Impossibile eliminare i dati delle impronte digitali. \u00C8 possibile che non si disponga delle autorizzazioni necessarie per eliminare i dati, che venga attualmente utilizzata una connessione remota o che sia in corso la registrazione dei dati delle impronte digitali."},
    {593u, L"Impossibile eliminare i dati delle impronte digitali. \u00C8 possibile che non si disponga delle autorizzazioni necessarie per eliminare i dati, che venga attualmente utilizzata una connessione remota o che sia in corso la registrazione dei dati delle impronte digitali."},
    {594u, L"Errore durante il salvataggio delle impostazioni. \u00C8 possibile che non si disponga delle autorizzazioni necessarie per modificare le impostazioni o che venga attualmente utilizzata una connessione remota."},
    {595u, L"Errore durante il salvataggio delle impostazioni. \u00C8 possibile che non si disponga delle autorizzazioni necessarie per modificare le impostazioni o che venga attualmente utilizzata una connessione remota."},
    {596u, L"<a>Come cambiare la password di Windows</a>"},
    {597u, L"Come cambiare la password di Windows."},
    {2000u, L"Per modificare l'area di visualizzazione verticale"},
    {2001u, L"Per modificare l'area di visualizzazione orizzontale"},
    {2002u, L"Orizzontale"},
    {2003u, L"Verticale"},
};

static const StringEntry kStringsFr[] = {
    {1u, L"P\u00E9riph\u00E9riques biom\u00E9triques"},
    {2u, L"Utilisez un lecteur d\u2019empreintes digitales pour ouvrir une session Windows ou des programmes prenant en charge la biom\u00E9trie."},
    {101u, L"P\u00E9riph\u00E9riques biom\u00E9triques"},
    {102u, L"Bienvenue dans les p\u00E9riph\u00E9riques biom\u00E9triques"},
    {103u, L"S\u00E9lection tactile"},
    {104u, L"Confirmer la suppression"},
    {105u, L"Message"},
    {106u, L"Modifier les param\u00E8tres"},
    {120u, L"Biom\u00E9trie"},
    {121u, L"Utiliser votre empreinte digitale avec Windows"},
    {122u, L"G\u00E9rer vos donn\u00E9es d\u2019empreinte digitale"},
    {123u, L"Supprimer vos donn\u00E9es d\u2019empreinte digitale"},
    {124u, L"Cesser d\u2019utiliser votre empreinte digitale pour ouvrir une session Windows"},
    {125u, L"Modifier les param\u00E8tres de biom\u00E9trie"},
    {126u, L"Gestionnaire de p\u00E9riph\u00E9riques"},
    {127u, L"Comptes d\u2019utilisateurs"},
    {130u, L"Utiliser des p\u00E9riph\u00E9riques biom\u00E9triques avec Windows"},
    {131u, L"Vous pouvez utiliser un lecteur d\u2019empreintes digitales pour ouvrir une session Windows. Certains programmes int\u00E8grent aussi des fonctionnalit\u00E9s qui fonctionnent avec la biom\u00E9trie."},
    {132u, L"Windows n\u2019a trouv\u00E9 aucun p\u00E9riph\u00E9rique biom\u00E9trique sur cet ordinateur. Vous devez peut-\u00EAtre connecter votre p\u00E9riph\u00E9rique ou installer les pilotes du p\u00E9riph\u00E9rique."},
    {133u, L"Vous ne pouvez pas utiliser les p\u00E9riph\u00E9riques biom\u00E9triques avec ce compte d\u2019utilisateur. Pour utiliser la biom\u00E9trie, ouvrez une session sous un autre compte d\u2019utilisateur."},
    {134u, L"Le service de biom\u00E9trie Windows n\u2019est pas en cours d\u2019ex\u00E9cution sur cet ordinateur. Contactez votre administrateur syst\u00E8me."},
    {135u, L"La biom\u00E9trie est d\u00E9sactiv\u00E9e sur cet ordinateur. Pour l\u2019utiliser, cliquez sur \u00AB\u00A0Modifier les param\u00E8tres de biom\u00E9trie\u00A0\u00BB, puis sur \u00AB\u00A0Biom\u00E9trie activ\u00E9e\u00A0\u00BB."},
    {140u, L"Un lecteur d\u2019empreintes digitales, qui est un p\u00E9riph\u00E9rique biom\u00E9trique, permet d\u2019acc\u00E9der rapidement et facilement \u00E0 votre ordinateur.\n\nLes fonctionnalit\u00E9s de s\u00E9curit\u00E9 variant d\u2019un lecteur d\u2019empreintes digitales \u00E0 l\u2019autre, il peut \u00EAtre d\u00E9conseill\u00E9 d\u2019utiliser le v\u00F4tre pour prot\u00E9ger des donn\u00E9es confidentielles, telles que des informations financi\u00E8res personnelles, ou pour acc\u00E9der \u00E0 des ressources sensibles, telles que des r\u00E9seaux d\u2019entreprise. Vous pouvez contacter le fournisseur de votre lecteur ou l\u2019\u00E9diteur de votre logiciel de lecture d\u2019empreintes digitales pour plus d\u2019informations sur l\u2019utilisation appropri\u00E9e de votre p\u00E9riph\u00E9rique.\n\nPour prot\u00E9ger au mieux les informations sensibles ou priv\u00E9es de votre ordinateur, utilisez les technologies des p\u00E9riph\u00E9riques de s\u00E9curit\u00E9 mat\u00E9rielle, telles que les modules de plateforme s\u00E9curis\u00E9e (TPM) et les cartes \u00E0 puce, pour activer le chiffrement s\u00E9curis\u00E9 de vos donn\u00E9es\u00A0; par ailleurs, limitez l\u2019acc\u00E8s par un mot de passe fort que vous m\u00E9morisez, conservez en lieu s\u00FBr ou maintenez confidentiel de la fa\u00E7on qui vous convient, ind\u00E9pendamment de l\u2019utilisation ou non d\u2019un lecteur d\u2019empreintes digitales. Vous pouvez \u00E9galement chiffrer vos donn\u00E9es. Certaines \u00E9ditions de Windows proposent le syst\u00E8me de fichiers EFS pour chiffrer des fichiers et dossiers individuels et le chiffrement de lecteur BitLocker pour chiffrer les lecteurs."},
    {141u, L"Inscrit"},
    {142u, L"Non inscrit"},
    {143u, L"Verrouill\u00E9 par un programme"},
    {144u, L"Non disponible"},
    {145u, L"Inscription en cours"},
    {146u, L"Pourquoi mon p\u00E9riph\u00E9rique est-il verrouill\u00E9\u00A0?"},
    {147u, L"Pourquoi mon p\u00E9riph\u00E9rique n\u2019est-il pas disponible\u00A0?"},
    {148u, L"Pourquoi mon p\u00E9riph\u00E9rique n\u2019est-il pas disponible\u00A0?"},
    {150u, L"Aucun lecteur d\u2019empreintes digitales n\u2019est actuellement disponible."},
    {151u, L"Aucun lecteur d\u2019empreintes digitales n\u2019est connect\u00E9 \u00E0 cet ordinateur ou ils sont tous utilis\u00E9s par des programmes."},
    {152u, L"Biom\u00E9trie d\u00E9sactiv\u00E9e"},
    {153u, L"La biom\u00E9trie est d\u00E9sactiv\u00E9e sur cet ordinateur. Pour l\u2019activer, cliquez sur \u00AB\u00A0Modifier les param\u00E8tres de biom\u00E9trie\u00A0\u00BB dans le Panneau de configuration du service de biom\u00E9trie."},
    {154u, L"Biom\u00E9trie d\u00E9sactiv\u00E9e"},
    {155u, L"Votre administrateur syst\u00E8me a d\u00E9sactiv\u00E9 la biom\u00E9trie sur cet ordinateur. Pour l\u2019activer, contactez votre administrateur syst\u00E8me."},
    {156u, L"Vous n\u2019avez pas de donn\u00E9es d\u2019empreintes digitales ou d\u2019informations d\u2019identification d\u2019ouverture de session \u00E0 supprimer"},
    {170u, L"Voulez-vous vraiment supprimer vos donn\u00E9es d\u2019empreinte digitale\u00A0?"},
    {171u, L"Vous serez invit\u00E9 \u00E0 entrer votre mot de passe Windows avant de supprimer vos donn\u00E9es d\u2019empreinte digitale.\nUne fois ces donn\u00E9es supprim\u00E9es, vous devrez utiliser votre mot de passe Windows pour ouvrir une session sur cet ordinateur. Si vous avez oubli\u00E9 votre mot de passe Windows, modifiez-le maintenant avant de supprimer vos donn\u00E9es d\u2019empreinte digitale."},
    {172u, L"Voulez-vous vraiment supprimer les donn\u00E9es d\u2019empreinte digitale pour tous les utilisateurs\u00A0?"},
    {173u, L"Une fois les donn\u00E9es d\u2019empreinte digitale supprim\u00E9es, tous les utilisateurs devront utiliser leur mot de passe Windows pour ouvrir une session sur cet ordinateur. Avant de supprimer ces donn\u00E9es, assurez-vous que les utilisateurs n\u2019ont pas oubli\u00E9 leur mot de passe Windows. Si un utilisateur a oubli\u00E9 son mot de passe Windows, demandez-lui de le modifier avant que vous supprimiez les donn\u00E9es d\u2019empreinte digitale."},
    {174u, L"Voulez-vous vraiment d\u00E9sactiver l\u2019utilisation de vos empreintes digitales pour ouvrir une session dans Windows\u00A0?"},
    {175u, L"Apr\u00E8s la d\u00E9sactivation de l\u2019ouverture de session bas\u00E9e sur les empreintes digitales, tous les utilisateurs devront utiliser leur mot de passe Windows pour ouvrir une session sur cet ordinateur. Avant de d\u00E9sactiver cette fonction, v\u00E9rifiez que tous les utilisateurs se souviennent de leur mot de passe Windows. Si un utilisateur ne se souvient pas de son mot de passe Windows, vous devez lui demander de le changer avant de d\u00E9sactiver cette fonction."},
    {176u, L"Voulez-vous vraiment d\u00E9sactiver l\u2019utilisation de vos empreintes digitales pour ouvrir une session dans un domaine\u00A0?"},
    {177u, L"Apr\u00E8s la d\u00E9sactivation de l\u2019ouverture de session bas\u00E9e sur les empreintes digitales, tous les utilisateurs du domaine devront utiliser leur mot de passe Windows pour ouvrir une session sur cet ordinateur. Avant de d\u00E9sactiver cette fonction, v\u00E9rifiez que tous les utilisateurs se souviennent de leur mot de passe Windows. Si un utilisateur ne se souvient pas de son mot de passe Windows, vous devez lui demander de le changer avant de d\u00E9sactiver cette fonction."},
    {178u, L"Confirmez votre mot de passe actuel"},
    {179u, L"Arr\u00EAter l\u2019utilisation de vos empreintes digitales pour ouvrir une session dans Windows"},
    {180u, L"Certains param\u00E8tres sont g\u00E9r\u00E9s par votre administrateur syst\u00E8me. <a>Pourquoi m\u2019est-il impossible de changer certains param\u00E8tres\u00A0?</a>"},
    {181u, L"Certains param\u00E8tres sont g\u00E9r\u00E9s par votre administrateur syst\u00E8me. Pourquoi est-il impossible de changer certains param\u00E8tres\u00A0?"},
    {182u, L"Ce programme est bloqu\u00E9 par une strat\u00E9gie de groupe. Pour plus d\u2019informations, contactez votre administrateur syst\u00E8me."},
    {190u, L"Inactif"},
    {191u, L"Actif"},
    {200u, L"Plusieurs lecteurs d\u2019empreintes digitales ont \u00E9t\u00E9 d\u00E9tect\u00E9s. Touchez la surface d\u2019analyse du lecteur d\u2019empreintes digitales que vous souhaitez utiliser."},
    {201u, L"Une inscription est d\u00E9j\u00E0 en cours. Terminez l\u2019inscription en cours et recommencez."},
    {202u, L"Impossible de d\u00E9tecter quel lecteur d\u2019empreintes digitales a \u00E9t\u00E9 touch\u00E9. Recommencez."},
    {210u, L"Aucune application de gestion n\u2019est install\u00E9e pour le lecteur d\u2019empreintes digitales s\u00E9lectionn\u00E9."},
    {211u, L"Cet ordinateur n\u2019est dot\u00E9 d\u2019aucun programme pouvant \u00EAtre utilis\u00E9 pour g\u00E9rer vos donn\u00E9es d\u2019empreinte digitale avec ce lecteur d\u2019empreintes digitales. Pour utiliser ce lecteur d\u2019empreintes digitales, procurez-vous un programme aupr\u00E8s du fabricant de votre ordinateur (si le lecteur d\u2019empreintes digitales fait partie int\u00E9grante de votre ordinateur) ou du fabricant de lecteur d\u2019empreintes digitales."},
    {212u, L"Une application de gestion d\u2019empreintes digitales est d\u00E9j\u00E0 en cours d\u2019ex\u00E9cution"},
    {213u, L"Fermez les applications de gestion d\u2019empreintes digitales ouvertes, puis recommencez."},
    {531u, L"P\u00E9riph\u00E9riques biom\u00E9triques"},
    {538u, L"Propri\u00E9t\u00E9s"},
    {540u, L"Bienvenue dans les p\u00E9riph\u00E9riques biom\u00E9triques"},
    {542u, L"Con&tinuer"},
    {543u, L"&Annuler"},
    {551u, L"S\u00E9lectionner le lecteur d\u2019empreintes digitales \u00E0 utiliser"},
    {552u, L"&Annuler"},
    {554u, L"&Supprimer"},
    {555u, L"&Annuler"},
    {557u, L"&Fermer"},
    {568u, L"Enregi&strer les modifications"},
    {569u, L"&Annuler"},
    {577u, L"Modifier les param\u00E8tres de biom\u00E9trie"},
    {578u, L"Ces param\u00E8tres g\u00E8rent la biom\u00E9trie et l\u2019ouverture de session par empreinte digitale pour cet ordinateur. Si vous souhaitez supprimer les donn\u00E9es biom\u00E9triques et les informations d\u2019ouverture de session par empreinte digitale de tous les utilisateurs de cet ordinateur, d\u00E9sactivez la biom\u00E9trie."},
    {581u, L"Pour obtenir des informations sur la cr\u00E9ation d\u2019un mot de passe fort, voir"},
    {582u, L"Conseils pour cr\u00E9er un mot de passe fort"},
    {583u, L"."},
    {584u, L"Biom\u00E9trie activ\u00E9e"},
    {585u, L"Biom\u00E9trie activ\u00E9e"},
    {586u, L"Permettre aux utilisateurs d\u2019ouvrir une session Windows au moyen de leurs empreintes digitales"},
    {587u, L"Permettre aux utilisateurs d\u2019ouvrir une session sur un domaine au moyen de leurs empreintes digitales"},
    {588u, L"Biom\u00E9trie d\u00E9sactiv\u00E9e"},
    {589u, L"Biom\u00E9trie d\u00E9sactiv\u00E9e"},
    {590u, L"La d\u00E9sactivation de l\u2019un de ces param\u00E8tres supprime les informations enregistr\u00E9es d\u2019ouverture de session par empreintes digitales.\nLa d\u00E9sactivation de la biom\u00E9trie supprime \u00E9galement les enregistrements d\u2019empreintes digitales de tous les utilisateurs."},
    {591u, L"La d\u00E9sactivation de l\u2019un de ces param\u00E8tres supprime les informations enregistr\u00E9es d\u2019ouverture de session par empreintes digitales. La d\u00E9sactivation de la biom\u00E9trie supprime \u00E9galement les enregistrements d\u2019empreintes digitales de tous les utilisateurs."},
    {592u, L"Vos donn\u00E9es d\u2019empreintes digitales n\u2019ont pas pu \u00EAtre supprim\u00E9es. Vous n\u2019avez peut-\u00EAtre pas l\u2019autorisation de supprimer les donn\u00E9es, vous utilisez actuellement une connexion distante ou vous inscrivez actuellement des donn\u00E9es d\u2019empreintes digitales."},
    {593u, L"Vos donn\u00E9es d\u2019empreintes digitales n\u2019ont pas pu \u00EAtre supprim\u00E9es. Vous n\u2019avez peut-\u00EAtre pas l\u2019autorisation de supprimer les donn\u00E9es, vous utilisez actuellement une connexion distante ou vous inscrivez actuellement des donn\u00E9es d\u2019empreintes digitales."},
    {594u, L"Une erreur s\u2019est produite lors de l\u2019enregistrement de vos param\u00E8tres. Vous n\u2019avez peut-\u00EAtre pas l\u2019autorisation de modifier les param\u00E8tres ou vous utilisez actuellement une connexion \u00E0 distance."},
    {595u, L"Une erreur s\u2019est produite lors de l\u2019enregistrement de vos param\u00E8tres. Vous n\u2019avez peut-\u00EAtre pas l\u2019autorisation de modifier les param\u00E8tres ou vous utilisez actuellement une connexion \u00E0 distance."},
    {596u, L"<a>Comment un utilisateur change-t-il son mot de passe Windows\u00A0?</a>"},
    {597u, L"Comment un utilisateur change-t-il son mot de passe Windows\u00A0?"},
    {2000u, L"Utilis\u00E9 pour modifier la zone d\u2019affichage verticale"},
    {2001u, L"Utilis\u00E9 pour modifier la zone d\u2019affichage horizontale"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
};

static const StringEntry kStringsEs[] = {
    {1u, L"Dispositivos biom\u00E9tricos"},
    {2u, L"Use un lector de huellas digitales para iniciar sesi\u00F3n en Windows o en programas que funcionan con biometr\u00EDa."},
    {101u, L"Dispositivos biom\u00E9tricos"},
    {102u, L"Dispositivos biom\u00E9tricos"},
    {103u, L"Selecci\u00F3n t\u00E1ctil"},
    {104u, L"Confirmar eliminaci\u00F3n"},
    {105u, L"Mensaje"},
    {106u, L"Cambiar configuraci\u00F3n"},
    {120u, L"Biometr\u00EDa"},
    {121u, L"Usar la huella digital con Windows"},
    {122u, L"Administrar los datos de la huella digital"},
    {123u, L"Quitar los datos de la huella digital"},
    {124u, L"Dejar de usar la huella digital para iniciar sesi\u00F3n en Windows"},
    {125u, L"Cambiar la configuraci\u00F3n de biometr\u00EDa"},
    {126u, L"Administrador de dispositivos"},
    {127u, L"Cuentas de usuario"},
    {130u, L"Usar dispositivos biom\u00E9tricos con Windows"},
    {131u, L"Puede usar un lector de huellas digitales para iniciar sesi\u00F3n en Windows. Algunos programas tambi\u00E9n ofrecen caracter\u00EDsticas que funcionan con biometr\u00EDa."},
    {132u, L"Windows no encontr\u00F3 ning\u00FAn dispositivo biom\u00E9trico en este equipo. Es posible que deba conectar su dispositivo o instalar controladores del dispositivo."},
    {133u, L"No puede usar dispositivos biom\u00E9tricos con esta cuenta de usuario. Para usar biometr\u00EDa, inicie sesi\u00F3n con otra cuenta de usuario."},
    {134u, L"El Servicio biom\u00E9trico de Windows no se est\u00E1 ejecutando en este equipo. P\u00F3ngase en contacto con el administrador del sistema."},
    {135u, L"La biometr\u00EDa est\u00E1 desactivada en este equipo. Si desea usarla, haga clic en 'Cambiar la configuraci\u00F3n de biometr\u00EDa' y, a continuaci\u00F3n, en 'Biometr\u00EDa activada'."},
    {140u, L"La funci\u00F3n del lector de huellas digitales, que es un dispositivo biom\u00E9trico, consiste en proporcionar un acceso r\u00E1pido y c\u00F3modo al equipo.\n\nLos lectores de huellas digitales var\u00EDan en cuanto a la capacidad de seguridad y es posible que no sea apropiado usarlos para proteger datos que desee mantener en privado, como la informaci\u00F3n financiera personal, o para tener acceso a recursos confidenciales, como las redes corporativas. P\u00F3ngase en contacto con el proveedor del lector de huellas digitales o del software de huellas digitales para obtener informaci\u00F3n adicional sobre el uso apropiado del dispositivo.\n\nPuede proteger mejor la informaci\u00F3n privada y confidencial del equipo con tecnolog\u00EDas que sacan provecho de los dispositivos de seguridad de hardware, como los M\u00F3dulos de plataforma segura y las tarjetas inteligentes, para habilitar el cifrado seguro de los datos, y limitando el acceso con una contrase\u00F1a segura que deber\u00E1 memorizar, guardar en lugar seguro o mantener en privado del modo que m\u00E1s convenga en cada caso, independientemente de si se usa un lector de huellas digitales. Tambi\u00E9n puede cifrar los datos. Algunas ediciones de Windows incluyen el Sistema de cifrado de archivos para cifrar archivos y carpetas y Cifrado de unidad BitLocker para cifrar unidades."},
    {141u, L"Inscrito"},
    {142u, L"No inscrito"},
    {143u, L"Bloqueado por un programa"},
    {144u, L"No disponible"},
    {145u, L"Inscripci\u00F3n en curso"},
    {146u, L"\u00BFPor qu\u00E9 est\u00E1 bloqueado el dispositivo?"},
    {147u, L"\u00BFPor qu\u00E9 no est\u00E1 disponible el dispositivo?"},
    {148u, L"\u00BFPor qu\u00E9 no est\u00E1 disponible el dispositivo?"},
    {150u, L"No hay lectores de huellas digitales disponibles en este momento."},
    {151u, L"No hay lectores de huellas digitales conectados a este equipo, o bien todos los lectores de huellas digitales est\u00E1n us\u00E1ndose en programas."},
    {152u, L"Biometr\u00EDa deshabilitada"},
    {153u, L"La biometr\u00EDa est\u00E1 desactivada en este equipo. Para activarla, haga clic en Cambiar la configuraci\u00F3n de biometr\u00EDa en el Panel de control de biometr\u00EDa."},
    {154u, L"Biometr\u00EDa deshabilitada"},
    {155u, L"El administrador del sistema desactiv\u00F3 la biometr\u00EDa en el equipo. Para activar la biometr\u00EDa, p\u00F3ngase en contacto con el administrador del sistema."},
    {156u, L"No tiene datos de huella digital ni credenciales de inicio de sesi\u00F3n para quitar."},
    {170u, L"\u00BFEst\u00E1 seguro de que desea eliminar sus datos de huellas digitales?"},
    {171u, L"Se le pedir\u00E1 que escriba su contrase\u00F1a de Windows antes de que se eliminen los datos de huellas digitales.\nUna vez eliminados los datos de huellas digitales, deber\u00E1 usar la contrase\u00F1a de Windows para iniciar sesi\u00F3n en este equipo. Si no recuerda la contrase\u00F1a de Windows, deber\u00E1 cambiarla ahora antes de eliminar los datos de huellas digitales."},
    {172u, L"\u00BFEst\u00E1 seguro de que desea eliminar los datos de huellas digitales de todos los usuarios?"},
    {173u, L"Una vez eliminados los datos de huellas digitales, todos los usuarios deber\u00E1n usar sus contrase\u00F1as de Windows para iniciar sesi\u00F3n en este equipo. Antes de eliminar los datos de huellas digitales, aseg\u00FArese de que todos los usuarios recuerdan sus contrase\u00F1as de Windows. Si alg\u00FAn usuario no la recuerda, aseg\u00FArese de que la cambie antes de eliminar los datos de huellas digitales."},
    {174u, L"\u00BFEst\u00E1 seguro de que desea deshabilitar el uso de huellas digitales para iniciar sesi\u00F3n en Windows?"},
    {175u, L"Tras deshabilitar el inicio de sesi\u00F3n basado en huellas digitales, todos los usuarios deber\u00E1n usar su contrase\u00F1a de Windows para iniciar sesi\u00F3n en el equipo. Antes de deshabilitar esta caracter\u00EDstica, aseg\u00FArese de que todos los usuarios recuerdan su contrase\u00F1a de Windows. Si alg\u00FAn usuario no la recuerda, debe hacer que la cambie antes de deshabilitar esta caracter\u00EDstica."},
    {176u, L"\u00BFEst\u00E1 seguro de que desea deshabilitar el uso de huellas digitales para iniciar sesi\u00F3n en un dominio?"},
    {177u, L"Tras deshabilitar el inicio de sesi\u00F3n basado en huellas digitales, todos los usuarios del dominio deber\u00E1n usar su contrase\u00F1a de Windows para iniciar sesi\u00F3n en el equipo. Antes de deshabilitar esta caracter\u00EDstica, aseg\u00FArese de que todos los usuarios recuerdan su contrase\u00F1a de Windows. Si alg\u00FAn usuario no la recuerda, debe hacer que la cambie antes de deshabilitar esta caracter\u00EDstica."},
    {178u, L"Confirme su contrase\u00F1a actual"},
    {179u, L"Dejar de usar la huella digital para iniciar sesi\u00F3n en Windows"},
    {180u, L"El administrador del sistema administra algunas de las configuraciones. <a>\u00BFPor qu\u00E9 no puedo cambiar algunas configuraciones?</a>"},
    {181u, L"El administrador del sistema administra algunas de las configuraciones. \u00BFPor qu\u00E9 no se pueden cambiar algunas configuraciones?"},
    {182u, L"La directiva de grupo bloquea a este programa. Para obtener m\u00E1s informaci\u00F3n, p\u00F3ngase en contacto con el administrador de sistema."},
    {190u, L"Desactivada"},
    {191u, L"Activada"},
    {200u, L"Se encontraron varios lectores de huellas digitales. Toque la superficie de examen del lector de huellas digitales que desea usar."},
    {201u, L"Ya hay una inscripci\u00F3n en curso. Finalice esta inscripci\u00F3n e int\u00E9ntelo de nuevo."},
    {202u, L"No se pudo detectar qu\u00E9 lector de huellas digitales se toc\u00F3. Int\u00E9ntelo de nuevo."},
    {210u, L"El lector de huellas digitales seleccionado no tiene instalada una aplicaci\u00F3n de administraci\u00F3n."},
    {211u, L"Este equipo no tiene un programa que se pueda usar para administrar los datos de huellas digitales con este lector de huellas digitales. Para usar el lector de huellas digitales, necesita obtener un programa del fabricante del equipo (si el lector es un componente del equipo) o del fabricante del lector de huellas digitales."},
    {212u, L"Ya hay una aplicaci\u00F3n de administraci\u00F3n de huellas digitales en ejecuci\u00F3n"},
    {213u, L"Cierre las aplicaciones de administraci\u00F3n de huellas digitales que est\u00E9n abiertas e int\u00E9ntelo de nuevo."},
    {531u, L"Dispositivos biom\u00E9tricos"},
    {538u, L"Propiedades"},
    {540u, L"Dispositivos biom\u00E9tricos"},
    {542u, L"&Continuar"},
    {543u, L"&Cancelar"},
    {551u, L"Seleccione el lector de huellas digitales que desea usar"},
    {552u, L"&Cancelar"},
    {554u, L"&Eliminar"},
    {555u, L"&Cancelar"},
    {557u, L"&Cerrar"},
    {568u, L"&Guardar cambios"},
    {569u, L"&Cancelar"},
    {577u, L"Cambiar la configuraci\u00F3n de biometr\u00EDa"},
    {578u, L"Esta configuraci\u00F3n controla la biometr\u00EDa y el inicio de sesi\u00F3n con huella digital de este equipo. Si desea eliminar los datos biom\u00E9tricos y la informaci\u00F3n del inicio de sesi\u00F3n con huella digital de todos los usuarios de este equipo, desactive la biometr\u00EDa."},
    {581u, L"Para obtener informaci\u00F3n sobre c\u00F3mo crear contrase\u00F1as seguras, consulte"},
    {582u, L"Sugerencias para crear una contrase\u00F1a segura"},
    {583u, L"."},
    {584u, L"Biometr\u00EDa activada"},
    {585u, L"Biometr\u00EDa activada"},
    {586u, L"Permite a los usuarios iniciar sesi\u00F3n en Windows con sus huellas digitales"},
    {587u, L"Permite a los usuarios iniciar sesi\u00F3n en un dominio con sus huellas digitales"},
    {588u, L"Biometr\u00EDa desactivada"},
    {589u, L"Biometr\u00EDa desactivada"},
    {590u, L"Al desactivar cualquiera de estas opciones se eliminar\u00E1 la informaci\u00F3n de inicio de sesi\u00F3n con huella digital guardada.\nAl desactivar la biometr\u00EDa tambi\u00E9n se eliminan los registros de huella digital de todos los usuarios."},
    {591u, L"Al desactivar cualquiera de estas opciones se eliminar\u00E1 la informaci\u00F3n de inicio de sesi\u00F3n con huella digital guardada. Al desactivar la biometr\u00EDa tambi\u00E9n se eliminan los registros de huella digital de todos los usuarios."},
    {592u, L"No se pudieron eliminar los datos de su huella digital. Quiz\u00E1 no tiene permiso para eliminar los datos, est\u00E1 usando una conexi\u00F3n remota o est\u00E1 inscribiendo los datos de la huella digital."},
    {593u, L"No se pudieron eliminar los datos de su huella digital. Quiz\u00E1 no tiene permiso para eliminar los datos, est\u00E1 usando una conexi\u00F3n remota o est\u00E1 inscribiendo los datos de la huella digital."},
    {594u, L"Error al guardar la configuraci\u00F3n. Es posible que no tenga permiso para cambiar la configuraci\u00F3n o que est\u00E9 usando una conexi\u00F3n remota en estos momentos."},
    {595u, L"Error al guardar la configuraci\u00F3n. Es posible que no tenga permiso para cambiar la configuraci\u00F3n o que est\u00E9 usando una conexi\u00F3n remota en estos momentos."},
    {596u, L"<a>\u00BFC\u00F3mo cambia un usuario su contrase\u00F1a de Windows?</a>"},
    {597u, L"\u00BFC\u00F3mo cambia un usuario su contrase\u00F1a de Windows?"},
    {2000u, L"Se usa para cambiar el \u00E1rea de observaci\u00F3n vertical"},
    {2001u, L"Se usa para cambiar el \u00E1rea de observaci\u00F3n horizontal"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
};

static const StringEntry kStringsPt[] = {
    {1u, L"Dispositivos Biom\u00E9tricos"},
    {2u, L"Use um leitor de impress\u00E3o digital para efetuar logon no Windows ou com programas que trabalham com biometria."},
    {101u, L"Dispositivos Biom\u00E9tricos"},
    {102u, L"Bem-vindo aos Dispositivos Biom\u00E9tricos"},
    {103u, L"Selecionar por Toque"},
    {104u, L"Confirmar Exclus\u00E3o"},
    {105u, L"Mensagem"},
    {106u, L"Alterar configura\u00E7\u00F5es"},
    {120u, L"Biometria"},
    {121u, L"Usar sua impress\u00E3o digital no Windows"},
    {122u, L"Gerenciar dados de impress\u00E3o digital"},
    {123u, L"Remover dados de impress\u00E3o digital"},
    {124u, L"Parar de usar impress\u00E3o digital para fazer logon no Windows"},
    {125u, L"Alterar configura\u00E7\u00F5es de biometria"},
    {126u, L"Gerenciador de Dispositivos"},
    {127u, L"Contas de Usu\u00E1rio"},
    {130u, L"Usar dispositivos biom\u00E9tricos com o Windows"},
    {131u, L"Voc\u00EA pode usar um leitor de impress\u00E3o digital para fazer logon no Windows. Alguns programas tamb\u00E9m oferecem recursos que funcionam com biometria."},
    {132u, L"O Windows n\u00E3o localizou dispositivos biom\u00E9tricos neste computador. Talvez seja necess\u00E1rio voc\u00EA conectar seu dispositivo ou instalar drivers do dispositivo."},
    {133u, L"N\u00E3o \u00E9 poss\u00EDvel usar dispositivos biom\u00E9tricos com esta conta de usu\u00E1rio. Para usar a biometria, efetue logon em uma conta de usu\u00E1rio diferente."},
    {134u, L"O Servi\u00E7o de Biometria do Windows n\u00E3o est\u00E1 funcionando neste computador. Entre em contato com o administrador do sistema."},
    {135u, L"A biometria est\u00E1 desativada neste computador. Para usar a biometria, clique em 'Alterar configura\u00E7\u00F5es de biometria' e em 'Biometria ativada'."},
    {140u, L"Um leitor de impress\u00E3o digital, que \u00E9 um dispositivo biom\u00E9trico, permite o acesso r\u00E1pido e conveniente ao seu computador.\n\nOs leitores de impress\u00E3o digital variam nos recursos de seguran\u00E7a e talvez n\u00E3o seja aconselh\u00E1vel usar seu leitor para proteger os dados que voc\u00EA deseja manter privados, como informa\u00E7\u00F5es financeiras, ou para acessar recursos confidenciais, como as redes corporativas. Contate o fornecedor do leitor de impress\u00E3o digital ou o fornecedor do software de impress\u00E3o digital para obter mais informa\u00E7\u00F5es sobre o uso adequado de seu dispositivo.\n\n\u00C9 poss\u00EDvel proteger melhor as informa\u00E7\u00F5es confidenciais e privadas em seu computador usando tecnologias que aproveitam os dispositivos hardware de seguran\u00E7a, como dispositivos do TPM (Trusted Platform Module) e cart\u00F5es inteligentes, que permitem a criptografia segura de seus dados, pois limitam o acesso por uma senha forte que voc\u00EA pode memorizar, armazenar em um local seguro ou manter privada do modo que desejar, independentemente de voc\u00EA usar ou n\u00E3o um leitor de impress\u00E3o digital. Tamb\u00E9m \u00E9 poss\u00EDvel criptografar os dados. Algumas edi\u00E7\u00F5es do Windows incluem o Sistema de Arquivos com Criptografia para criptografar arquivos e pastas espec\u00EDficos e a Criptografia de Unidade de Disco BitLocker para criptografar unidades de disco."},
    {141u, L"Registrado"},
    {142u, L"N\u00E3o registrado"},
    {143u, L"Bloqueado por um programa"},
    {144u, L"N\u00E3o dispon\u00EDvel"},
    {145u, L"Registro em andamento"},
    {146u, L"Por que meu dispositivo est\u00E1 bloqueado?"},
    {147u, L"Por que meu dispositivo n\u00E3o est\u00E1 dispon\u00EDvel?"},
    {148u, L"Por que meu dispositivo n\u00E3o est\u00E1 dispon\u00EDvel?"},
    {150u, L"N\u00E3o h\u00E1 leitores de impress\u00E3o digital dispon\u00EDveis no momento"},
    {151u, L"N\u00E3o h\u00E1 nenhum leitor de impress\u00E3o digital conectado a este computador ou todos os leitores de impress\u00E3o digital est\u00E3o sendo usados por outros programas."},
    {152u, L"Biometria Desabilitada"},
    {153u, L"A biometria est\u00E1 desativada neste computador. Para ativ\u00E1-la, clique em Alterar configura\u00E7\u00F5es de biometria no Painel de Controle de Biometria."},
    {154u, L"Biometria Desabilitada"},
    {155u, L"O administrador do sistema desativou a biometria neste computador. Para ativar a biometria, contate o administrador do sistema."},
    {156u, L"N\u00E3o h\u00E1 dados de impress\u00E3o digital nem credenciais de logon a remover"},
    {170u, L"Tem certeza de que deseja excluir os dados de impress\u00E3o digital?"},
    {171u, L"Voc\u00EA ser\u00E1 solicitado a digitar sua senha do Windows antes de excluir os dados de impress\u00E3o digital.\nAp\u00F3s a exclus\u00E3o dos dados de impress\u00E3o digital, voc\u00EA precisar\u00E1 usar sua senha do Windows para fazer logon no computador. Se n\u00E3o se lembrar de sua senha do Windows, altere-a agora, antes de excluir os dados de impress\u00E3o digital."},
    {172u, L"Tem certeza de que deseja excluir os dados de impress\u00E3o digital de todos os usu\u00E1rios?"},
    {173u, L"Depois que os dados de impress\u00E3o digital forem exclu\u00EDdos, todos os usu\u00E1rios precisar\u00E3o usar suas senhas do Windows para fazer logon no computador. Antes de excluir os dados de impress\u00E3o digital, verifique se todos os usu\u00E1rios se lembram de suas senhas do Windows. Se algum usu\u00E1rio n\u00E3o se lembrar desse senha, voc\u00EA dever\u00E1 solicitar que ele altere-a antes da exclus\u00E3o dos dados de impress\u00E3o digital."},
    {174u, L"Tem certeza de que deseja desabilitar usando sua impress\u00E3o digital para conectar-se ao Windows?"},
    {175u, L"Depois que o logon baseado em impress\u00E3o digital for desabilitado, todos os usu\u00E1rios precisar\u00E3o usar suas senhas do Windows para fazer logon neste computador. Antes de desabilitar esse recurso, verifique se todos os usu\u00E1rios se lembram de suas senhas do Windows. Se algum usu\u00E1rio n\u00E3o se lembrar da senha, voc\u00EA dever\u00E1 solicitar que sua altera\u00E7\u00E3o antes da desabilita\u00E7\u00E3o desse recurso."},
    {176u, L"Tem certeza de que deseja desabilitar o uso das impress\u00F5es digitais para fazer login em um dom\u00EDnio?"},
    {177u, L"Ap\u00F3s desabilitar o logon baseado em impress\u00E3o digital, todos os usu\u00E1rios precisar\u00E3o usar suas senhas do Windows para fazer logon no computador. Antes de desabilitar esse recurso, verifique se todos os usu\u00E1rios se lembram de suas senhas do Windows. Se algum usu\u00E1rio n\u00E3o se lembrar dessa senha, voc\u00EA dever\u00E1 solicitar que ele altere-a antes de desabilitar esse recurso."},
    {178u, L"Confirme a senha atual"},
    {179u, L"Parar de usar impress\u00E3o digital para fazer logon no Windows"},
    {180u, L"Algumas configura\u00E7\u00F5es s\u00E3o gerenciadas pelo administrador do sistema. <a>Por que n\u00E3o consigo alterar algumas configura\u00E7\u00F5es?</a>"},
    {181u, L"Algumas configura\u00E7\u00F5es s\u00E3o gerenciadas pelo administrador do sistema. Por que n\u00E3o posso alterar algumas configura\u00E7\u00F5es?"},
    {182u, L"Este programa est\u00E1 bloqueado pela diretiva de grupo. Para obter mais informa\u00E7\u00F5es, contate o administrador do sistema."},
    {190u, L"Desativado(a)"},
    {191u, L"Ativado(a)"},
    {200u, L"Mais de um leitor de impress\u00E3o digital foi encontrado. Toque na superf\u00EDcie de digitaliza\u00E7\u00E3o do leitor de impress\u00E3o de digital que deseja usar."},
    {201u, L"Um registro j\u00E1 est\u00E1 em andamento. Encerre o registro em andamento e tente novamente."},
    {202u, L"N\u00E3o foi poss\u00EDvel detectar o leitor de impress\u00E3o digital que foi tocado. Tente novamente."},
    {210u, L"N\u00E3o h\u00E1 nenhum aplicativo de gerenciamento instalado no leitor de impress\u00E3o digital selecionado"},
    {211u, L"O computador n\u00E3o possui um programa que possa ser usado para gerenciar os dados de impress\u00E3o digital com este leitor de impress\u00E3o digital. Para usar esse leitor de impress\u00E3o digital, voc\u00EA precisa obter um programa com o fabricante do computador (caso o leitor fa\u00E7a parte do computador) ou com o fabricante do leitor."},
    {212u, L"Um aplicativo de gerenciamento de impress\u00E3o digital j\u00E1 est\u00E1 em execu\u00E7\u00E3o"},
    {213u, L"Feche todos os aplicativos de gerenciamento de impress\u00E3o digital e tente novamente."},
    {531u, L"Dispositivos Biom\u00E9tricos"},
    {538u, L"Propriedades"},
    {540u, L"Bem-vindo aos Dispositivos Biom\u00E9tricos"},
    {542u, L"Con&tinuar"},
    {543u, L"&Cancelar"},
    {551u, L"Selecionar o leitor de impress\u00E3o digital a ser usado"},
    {552u, L"&Cancelar"},
    {554u, L"E&xcluir"},
    {555u, L"&Cancelar"},
    {557u, L"&Fechar"},
    {568u, L"&Salvar Altera\u00E7\u00F5es"},
    {569u, L"Cancela&r"},
    {577u, L"Alterar configura\u00E7\u00F5es de biometria"},
    {578u, L"Estas configura\u00E7\u00F5es controlam a biometria e o logon por impress\u00E3o impress\u00E3o digital neste computador. Se desejar excluir os dados biom\u00E9tricos e as informa\u00E7\u00F5es de logon por impress\u00E3o digital para todos os usu\u00E1rios do computador, desative a biometria."},
    {581u, L"Para obter informa\u00E7\u00F5es sobre como criar uma senha forte, consulte"},
    {582u, L"Dicas para a cria\u00E7\u00E3o de uma senha forte"},
    {583u, L"."},
    {584u, L"Biom\u00E9trica ativada"},
    {585u, L"Biom\u00E9trica ativada"},
    {586u, L"Permitir que os usu\u00E1rios fa\u00E7am logon no Windows usando suas impress\u00F5es digitais"},
    {587u, L"Permitir que os usu\u00E1rios fa\u00E7am logon em um dom\u00EDnio usando suas impress\u00F5es digitais"},
    {588u, L"Biom\u00E9trica desativada"},
    {589u, L"Biom\u00E9trica desativada"},
    {590u, L"Se essas configura\u00E7\u00F5es forem desabilitadas, as informa\u00E7\u00F5es de logon de impress\u00E3o digital que foram salvas ser\u00E3o exclu\u00EDdas.\nA desabilita\u00E7\u00E3o da biom\u00E9trica tamb\u00E9m exclui os registros de impress\u00E3o digital de todos os usu\u00E1rios."},
    {591u, L"Se uma dessas configura\u00E7\u00F5es for desabilitada, as informa\u00E7\u00F5es de logon de impress\u00E3o digital salvas ser\u00E3o exclu\u00EDdas. A desabilita\u00E7\u00E3o da biom\u00E9trica tamb\u00E9m exclui os registros de impress\u00E3o digital de todos os usu\u00E1rios."},
    {592u, L"Seus dados de impress\u00E3o digital n\u00E3o foram exclu\u00EDdos. Talvez voc\u00EA n\u00E3o tenha permiss\u00E3o para excluir os dados, voc\u00EA esteja usando uma conex\u00E3o remota ou esteja registrando dados de impress\u00E3o digital."},
    {593u, L"Seus dados de impress\u00E3o digital n\u00E3o foram exclu\u00EDdos. Talvez voc\u00EA n\u00E3o tenha permiss\u00E3o para excluir os dados, voc\u00EA esteja usando uma conex\u00E3o remota ou esteja registrando dados de impress\u00E3o digital."},
    {594u, L"Falha ao salvar as configura\u00E7\u00F5es. Talvez voc\u00EA n\u00E3o tenha permiss\u00E3o para alterar as configura\u00E7\u00F5es ou esteja usando uma conex\u00E3o remota no momento."},
    {595u, L"Falha ao salvar as configura\u00E7\u00F5es. Talvez voc\u00EA n\u00E3o tenha permiss\u00E3o para alterar as configura\u00E7\u00F5es ou esteja usando uma conex\u00E3o remota no momento."},
    {596u, L"<a>Como um usu\u00E1rio pode alterar a senha do Windows?</a>"},
    {597u, L"Como um usu\u00E1rio pode alterar a senha do Windows?"},
    {2000u, L"Usado para alterar a \u00E1rea de exibi\u00E7\u00E3o vertical"},
    {2001u, L"Usado para alterar a \u00E1rea de exibi\u00E7\u00E3o horizontal"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertical"},
};

static const StringEntry kStringsDe[] = {
    {1u, L"Biometrische Ger\u00E4te"},
    {2u, L"Verwenden Sie f\u00FCr die Windows-Anmeldung einen Fingerabdruckleser oder Programme mit Biometriefunktionen."},
    {101u, L"Biometrische Ger\u00E4te"},
    {102u, L"Willkommen bei den biometrischen Ger\u00E4ten"},
    {103u, L"Durch Ber\u00FChrung ausw\u00E4hlen"},
    {104u, L"L\u00F6schen best\u00E4tigen"},
    {105u, L"Nachricht"},
    {106u, L"Einstellungen \u00E4ndern"},
    {120u, L"Biometrie"},
    {121u, L"Melden Sie sich mit Ihrem Fingerabdruck bei Windows an."},
    {122u, L"Fingerabdruckdaten verwalten"},
    {123u, L"Fingerabdruckdaten entfernen"},
    {124u, L"Fingerabdruck nicht mehr f\u00FCr die Windows-Anmeldung verwenden."},
    {125u, L"Biometrieeinstellungen \u00E4ndern"},
    {126u, L"Ger\u00E4te-Manager"},
    {127u, L"Benutzerkonten"},
    {130u, L"Biometrieger\u00E4te mit Windows verwenden"},
    {131u, L"Sie k\u00F6nnen einen Fingerabdruckleser f\u00FCr die Windows-Anmeldung verwenden. Einige Programme bieten auch Biometriefunktionen an."},
    {132u, L"Auf dem Computer wurden keine Biometrieger\u00E4te gefunden. M\u00F6glicherweise m\u00FCssen Sie das Ger\u00E4t erst anschlie\u00DFen oder Ger\u00E4tetreiber installieren."},
    {133u, L"Mit diesem Benutzerkonto k\u00F6nnen Sie keine biometrischen Ger\u00E4te verwenden. Wenn Sie die Biometrie verwenden m\u00F6chten, melden Sie sich mit einem anderen Benutzerkonto an."},
    {134u, L"Der Windows-Biometriedienst wird auf diesem Computer nicht ausgef\u00FChrt. Wenden Sie sich an den Systemadministrator."},
    {135u, L"Die Biometrie ist auf diesem Computer deaktiviert. Wenn Sie die Biometrie verwenden m\u00F6chten, klicken Sie auf \"Biometrieeinstellungen \u00E4ndern\", und klicken Sie dann auf \"Biometrie ein\"."},
    {140u, L"Mit dem Fingerabdruckleser, einem biometrischen Ger\u00E4t, k\u00F6nnen Sie schnell und bequem auf den Computer zugreifen.\n\nDie Sicherheitsfunktionen der Fingerabdruckleser variieren. Daher sollten Sie den Fingerabdruckleser m\u00F6glicherweise nicht verwenden, um vertrauliche Daten (z. B. pers\u00F6nliche Finanzdaten) zu sch\u00FCtzen oder um auf sensible Ressourcen (z. B. Unternehmensnetzwerke) zuzugreifen. Wenden Sie sich an den Anbieter des Fingerabdrucklesers oder der Fingerabdrucksoftware, um zus\u00E4tzliche Informationen zur richtigen Verwendung des Ger\u00E4ts zu erhalten.\n\nSie k\u00F6nnen private und sensible Daten auf dem Computer am besten mit Technologien sch\u00FCtzen, die auf Hardwaresicherheitsger\u00E4ten basieren, beispielsweise Trusted Platform Modules (TPMs) und Smartcards. Diese Technologien erm\u00F6glichen die sichere Verschl\u00FCsselung von Daten. Zudem sollten Sie den Zugriff mit einem sicheren Kennwort einschr\u00E4nken, das Sie sich entweder merken, an einem sicheren Ort aufbewahren oder auf andere Weise vor unberechtigtem Zugriff sch\u00FCtzen, ganz gleich, ob Sie einen Fingerabdruckleser verwenden oder nicht. Sie k\u00F6nnen Ihre Daten auch verschl\u00FCsseln. Einige Editionen von Windows beinhalten neben einem verschl\u00FCsselten Dateisystem (Encrypting File System, EFS) zum Verschl\u00FCsseln einzelner Dateien und Ordner auch eine BitLocker-Laufwerksverschl\u00FCsselung zum Verschl\u00FCsseln von Laufwerken."},
    {141u, L"Registriert"},
    {142u, L"Nicht registriert"},
    {143u, L"Durch Programm gesperrt"},
    {144u, L"Nicht verf\u00FCgbar"},
    {145u, L"Registrierung in Bearbeitung"},
    {146u, L"Warum ist das Ger\u00E4t gesperrt?"},
    {147u, L"Warum ist das Ger\u00E4t nicht verf\u00FCgbar?"},
    {148u, L"Warum ist das Ger\u00E4t nicht verf\u00FCgbar?"},
    {150u, L"Es sind zurzeit keine Fingerabdruckleser verf\u00FCgbar."},
    {151u, L"An diesem Computer sind keine Fingerabdruckleser angeschlossen, oder s\u00E4mtliche Fingerabdruckleser werden derzeit von Programmen verwendet."},
    {152u, L"Biometrie deaktiviert"},
    {153u, L"Die Biometrie ist auf diesem Computer deaktiviert. Wenn Sie die Biometrie aktivieren m\u00F6chten, klicken Sie in der Biometriesystemsteuerung auf \"Biometrieeinstellungen \u00E4ndern\"."},
    {154u, L"Biometrie deaktiviert"},
    {155u, L"Der Systemadministrator hat die Biometrie auf diesem Computer deaktiviert. Wenn Sie die Biometrie aktivieren m\u00F6chten, wenden Sie sich an den Systemadministrator."},
    {156u, L"Es sind keine Fingerabdruckdaten oder Anmeldeinformationen vorhanden, die entfernt werden k\u00F6nnen."},
    {170u, L"M\u00F6chten Sie die Fingerabdruckdaten wirklich l\u00F6schen?"},
    {171u, L"Sie werden zur Eingabe des Windows-Kennworts aufgefordert, bevor die Fingerabdruckdaten gel\u00F6scht werden.\nNachdem die Fingerabdruckdaten gel\u00F6scht wurden, m\u00FCssen Sie sich mit Ihrem Windows-Kennwort am Computer anmelden. Wenn Sie sich nicht an das Windows-Kennwort erinnern k\u00F6nnen, sollten Sie es jetzt \u00E4ndern, bevor Sie die Fingerabdruckdaten l\u00F6schen."},
    {172u, L"M\u00F6chten Sie die Fingerabdruckdaten wirklich f\u00FCr alle Benutzer l\u00F6schen?"},
    {173u, L"Nachdem die Fingerabdruckdaten gel\u00F6scht wurden, m\u00FCssen sich s\u00E4mtliche Benutzer mit dem Windows-Kennwort am Computer anmelden. Vor dem L\u00F6schen der Fingerabdruckdaten m\u00FCssen Sie sicherstellen, dass sich alle Benutzer an ihr Windows-Kennwort erinnern. Fordern Sie die Benutzer, die sich nicht an ihr Windows-Kennwort erinnern, zum \u00C4ndern des Windows-Kennworts auf, bevor Sie ihre Fingerabdruckdaten l\u00F6schen."},
    {174u, L"Sind Sie sicher, dass Sie die Verwendung Ihres Fingerabdrucks zur Windows-Anmeldung deaktivieren m\u00F6chten?"},
    {175u, L"Nach der Deaktivierung der Anmeldung mit Fingerabdruck m\u00FCssen alle Benutzer ihr Windows-Kennwort verwenden, um sich beim Computer anzumelden. Stellen Sie sicher, dass alle Benutzer \u00FCber ihr Windows-Kennwort verf\u00FCgen, bevor Sie das Feature deaktivieren. Wenn ein Benutzer das Windows-Kennwort vergessen hat, sollte es zun\u00E4chst ge\u00E4ndert werden, bevor Sie das Feature deaktivieren."},
    {176u, L"Sind Sie sicher, dass Sie die Verwendung Ihres Fingerabdrucks zur Dom\u00E4nenanmeldung deaktivieren m\u00F6chten?"},
    {177u, L"Nach der Deaktivierung der Anmeldung mit Fingerabdruck m\u00FCssen alle Dom\u00E4nenbenutzer ihr Windows-Kennwort verwenden, um sich beim Computer anzumelden. Stellen Sie sicher, dass alle Benutzer \u00FCber ihr Windows-Kennwort verf\u00FCgen, bevor Sie das Feature deaktivieren. Wenn ein Benutzer das Windows-Kennwort vergessen hat, sollte es zun\u00E4chst ge\u00E4ndert werden, bevor Sie das Feature deaktivieren."},
    {178u, L"Best\u00E4tigen Sie Ihr aktuelles Kennwort."},
    {179u, L"Fingerabdruck nicht mehr f\u00FCr die Windows-Anmeldung verwenden."},
    {180u, L"Einige Einstellungen werden vom Systemadministrator gesteuert. <a>Warum k\u00F6nnen Sie einige Einstellungen nicht \u00E4ndern?</a>"},
    {181u, L"Einige Einstellungen werden vom Systemadministrator verwaltet. Warum kann ich einige Einstellungen nicht \u00E4ndern?"},
    {182u, L"Dieses Programm wurde durch eine Gruppenrichtlinie geblockt. Weitere Informationen erhalten Sie vom Systemadministrator."},
    {190u, L"Aus"},
    {191u, L"Ein"},
    {200u, L"Es wurden mehrere Fingerabdruckleser gefunden. Ber\u00FChren Sie die Scanoberfl\u00E4che des Fingerabdrucklesers, den Sie verwenden m\u00F6chten."},
    {201u, L"Eine Registrierung ist bereits in Bearbeitung. Beenden Sie die Registrierung, und wiederholen Sie den Vorgang."},
    {202u, L"Es wurde nicht erkannt, welcher Fingerabdruckleser ber\u00FChrt wurde. Wiederholen Sie den Vorgang."},
    {210u, L"Auf dem ausgew\u00E4hlten Fingerabdruckleser ist keine Verwaltungsanwendung installiert."},
    {211u, L"Auf diesem Computer ist kein Programm installiert, das zum Verwalten der Fingerabdruckdaten f\u00FCr diesen Fingerabdruckleser verwendet werden kann. Damit der Fingerabdruckleser verwendet werden kann, m\u00FCssen Sie vom Hersteller des Computers (falls der Fingerabdruckleser im Lieferumfang des Computers enthalten war) oder des Fingerabdrucklesers ein entsprechendes Programm anfordern."},
    {212u, L"Es wird bereits eine Anwendung zur Verwaltung von Fingerabdr\u00FCcken ausgef\u00FChrt."},
    {213u, L"Schlie\u00DFen Sie alle Anwendungen zur Verwaltung von Fingerabdr\u00FCcken, und wiederholen Sie den Vorgang."},
    {531u, L"Biometrische Ger\u00E4te"},
    {538u, L"Eigenschaften"},
    {540u, L"Willkommen bei den biometrischen Ger\u00E4ten"},
    {542u, L"Wei&ter"},
    {543u, L"&Abbrechen"},
    {551u, L"W\u00E4hlen Sie den Fingerabdruckleser aus, den Sie verwenden m\u00F6chten."},
    {552u, L"&Abbrechen"},
    {554u, L"&L\u00F6schen"},
    {555u, L"&Abbrechen"},
    {557u, L"&Schlie\u00DFen"},
    {568u, L"\u00C4n&derungen speichern"},
    {569u, L"A&bbrechen"},
    {577u, L"Biometrieeinstellungen \u00E4ndern"},
    {578u, L"Mithilfe dieser Einstellungen werden die Biometrie und die Fingerabdruckanmeldung f\u00FCr den Computer gesteuert. Wenn Sie die biometrischen Daten und die Fingerabdruck-Anmeldeinformationen f\u00FCr alle Benutzer dieses Computers l\u00F6schen m\u00F6chten, deaktivieren Sie die Biometrie."},
    {581u, L"Weitere Informationen zum Erstellen eines sicheren Kennworts finden Sie unter"},
    {582u, L"Tipps zum Erstellen eines sicheren Kennworts"},
    {583u, L"."},
    {584u, L"Biometrie ein"},
    {585u, L"Biometrie ein"},
    {586u, L"Erm\u00F6glicht Benutzern das Anmelden bei Windows mithilfe des Fingerabdrucks."},
    {587u, L"Erm\u00F6glicht Benutzern das Anmelden bei einer Dom\u00E4ne mithilfe des Fingerabdrucks."},
    {588u, L"Biometrie aus"},
    {589u, L"Biometrie aus"},
    {590u, L"Wenn Sie eine dieser Einstellungen deaktivieren, werden die gespeicherten Fingerabdruck-Anmeldeinformationen gel\u00F6scht.\nDurch das Deaktivieren der Biometrie werden zudem die Fingerabdruckdatens\u00E4tze f\u00FCr alle Benutzer gel\u00F6scht."},
    {591u, L"Wenn Sie eine dieser Einstellungen deaktivieren, werden die gespeicherten Fingerabdruck-Anmeldeinformationen gel\u00F6scht. Durch das Deaktivieren der Biometrie werden zudem die Fingerabdruckdatens\u00E4tze f\u00FCr alle Benutzer gel\u00F6scht."},
    {592u, L"Ihre Fingerabdruckdaten konnten nicht gel\u00F6scht werden. Sie verf\u00FCgen m\u00F6glicherweise nicht \u00FCber die Berechtigung zum L\u00F6schen von Fingerabdruckdaten. Sie verwenden derzeit eine Remoteverbindung, oder Sie registrieren gerade Fingerabdruckdaten."},
    {593u, L"Ihre Fingerabdruckdaten konnten nicht gel\u00F6scht werden. Sie verf\u00FCgen m\u00F6glicherweise nicht \u00FCber die Berechtigung zum L\u00F6schen von Fingerabdruckdaten. Sie verwenden derzeit eine Remoteverbindung, oder Sie registrieren gerade Fingerabdruckdaten."},
    {594u, L"Beim Speichern der Einstellungen ist ein Fehler aufgetreten. Sie verf\u00FCgen m\u00F6glicherweise nicht \u00FCber die Berechtigung zum \u00C4ndern der Einstellungen, oder Sie verwenden derzeit eine Remoteverbindung."},
    {595u, L"Beim Speichern der Einstellungen ist ein Fehler aufgetreten. Sie verf\u00FCgen m\u00F6glicherweise nicht \u00FCber die Berechtigung zum \u00C4ndern der Einstellungen, oder Sie verwenden derzeit eine Remoteverbindung."},
    {596u, L"<a>Wie k\u00F6nnen Benutzer das Windows-Kennwort \u00E4ndern?</a>"},
    {597u, L"Wie k\u00F6nnen Benutzer das Windows-Kennwort \u00E4ndern?"},
    {2000u, L"Wird zum \u00C4ndern des vertikalen Anzeigebereichs verwendet"},
    {2001u, L"Wird zum \u00C4ndern des horizontalen Anzeigebereichs verwendet"},
    {2002u, L"Horizontal"},
    {2003u, L"Vertikal"},
};

struct LangTable {
    Lang lang;
    const wchar_t* code;
    const StringEntry* entries;
    size_t count;
};

static const LangTable kLangTables[] = {
    {Lang::En, L"en", kStringsEn, sizeof(kStringsEn) / sizeof(kStringsEn[0])},
    {Lang::It, L"it", kStringsIt, sizeof(kStringsIt) / sizeof(kStringsIt[0])},
    {Lang::Fr, L"fr", kStringsFr, sizeof(kStringsFr) / sizeof(kStringsFr[0])},
    {Lang::Es, L"es", kStringsEs, sizeof(kStringsEs) / sizeof(kStringsEs[0])},
    {Lang::Pt, L"pt", kStringsPt, sizeof(kStringsPt) / sizeof(kStringsPt[0])},
    {Lang::De, L"de", kStringsDe, sizeof(kStringsDe) / sizeof(kStringsDe[0])},
};

static const LangTable& GetLangTable(Lang lang) {
    const int idx = static_cast<int>(lang);
    if (idx >= 0 && idx < static_cast<int>(sizeof(kLangTables) / sizeof(kLangTables[0]))) {
        return kLangTables[idx];
    }
    return kLangTables[0];
}

static const wchar_t* LookupString(Lang lang, uint16_t id) {
    const LangTable& t = GetLangTable(lang);
    for (size_t i = 0; i < t.count; ++i) {
        if (t.entries[i].id == id) return t.entries[i].text;
    }
    return L"";
}

// ============================================================================
// 2. Self-Contained SHA-256 (zero extra crypto library dependency)
// ============================================================================

namespace sha256 {

static inline uint32_t Rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
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
    const uint64_t bitLen = static_cast<uint64_t>(len) * 8u;
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
// 3. PE & Resource Builder (portable C++ -- testable standalone via -DBIO_GENERATOR_ONLY)
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
        if (isStr != o.isStr) return isStr > o.isStr;  // named entries precede ID entries in PE .rsrc
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

// Build a complete, properly sorted PE .rsrc blob positioned at baseRva.
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
    out.reserve(payloadPos + 65536);

    auto writeDirHeader = [&](uint16_t nNamed, uint16_t nId) {
        Append32(out, 0);
        Append32(out, 0);
        Append16(out, 0);
        Append16(out, 0);
        Append16(out, nNamed);
        Append16(out, nId);
    };

    // Level 0: Type directory
    {
        auto [nn, ni] = countEntries(tree);
        writeDirHeader(nn, ni);
        for (const auto& tkv : tree) {
            const uint32_t idField = tkv.first.isStr ? (0x80000000u | strOff[tkv.first.name]) : tkv.first.id;
            Append32(out, idField);
            Append32(out, 0x80000000u | typeDirOff[tkv.first]);
        }
    }
    // Level 1 & Level 2 directories
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

    // Unicode resource directory strings
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

    // Data descriptors
    for (const LeafInfo& lf : leaves) {
        Append32(out, baseRva + lf.dataOff);
        Append32(out, static_cast<uint32_t>(lf.data->size()));
        Append32(out, 0);
        Append32(out, 0);
    }

    // Payloads
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

static bool PatchImportDui70ToB7dui(Bytes& peBytes, const PeLayout& pe, std::string& err) {
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
            static const char kRepl[10] = "b7dui.dll";
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

static void PatchXml100(Bytes& data, Lang lang, const std::wstring& selfDllPath) {
    std::string xml(reinterpret_cast<const char*>(data.data()), data.size());

    // Replace icon="biocpl.dll,-1" -> icon="<selfDllPath>,-1"
    const std::string iconFrom = "icon=\"biocpl.dll,-1\"";
    const std::string iconTo = "icon=\"" + XmlEscapeUtf8(WideToUtf8(selfDllPath)) + ",-1\"";
    size_t pos = 0;
    while ((pos = xml.find(iconFrom, pos)) != std::string::npos) {
        xml.replace(pos, iconFrom.size(), iconTo);
        pos += iconTo.size();
    }

    // Replace displayname="@biocpl.dll,-<id>" -> literal localized UTF-8 string
    const std::string dispPrefix = "displayname=\"@biocpl.dll,-";
    pos = 0;
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

static bool BuildPatchedBioCpl(const Bytes& rawBioCpl, const Bytes& rawDuser, Lang lang,
                               const std::wstring& selfDllPath, Bytes& outDll, std::string& err) {
    Bytes peBytes = rawBioCpl;
    PeLayout pe;
    if (!pe.Parse(peBytes)) {
        err = "failed to parse biocpl.dll PE";
        return false;
    }

    std::string commonSheet;
    if (!ExtractCommonSheet(rawDuser, commonSheet, err)) return false;

    if (!PatchImportDui70ToB7dui(peBytes, pe, err)) return false;

    std::vector<RsrcItem> origItems;
    if (!ExtractResources(peBytes, pe, origItems)) {
        err = "failed to extract biocpl.dll resources";
        return false;
    }

    std::vector<RsrcItem> newItems;
    newItems.reserve(origItems.size() + 32);
    int uiPatched = 0;
    int xmlPatched = 0;
    for (RsrcItem& it : origItems) {
        if (it.type.isStr && it.type.name == L"MUI") continue;
        if (!it.type.isStr && it.type.id == 6u) continue;
        if (it.type.isStr && it.type.name == L"XMLFILE" && !it.name.isStr && it.name.id == 100u) {
            PatchXml100(it.data, lang, selfDllPath);
            ++xmlPatched;
        } else if (it.type.isStr && it.type.name == L"UIFILE") {
            std::string ui(reinterpret_cast<const char*>(it.data.data()), it.data.size());
            const size_t closePos = ui.find("</stylesheets>");
            if (closePos != std::string::npos && ui.find("resid=\"common\"") == std::string::npos) {
                ui.insert(closePos, "\r\n" + commonSheet + "\r\n");
                it.data.assign(ui.begin(), ui.end());
                ++uiPatched;
            }
        }
        newItems.push_back(std::move(it));
    }
    if (xmlPatched != 1 || uiPatched < 8) {
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
        err = "insufficient PE header slack for .rsrc7 section";
        return false;
    }

    // Zero BoundImport (DD[11]), Security (DD[4]), and stale header slack (clears bound "DUI70.dll" at 0x3f1)
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

    static const char kSecName[8] = {'.', 'r', 's', 'r', 'c', '7', 0, 0};
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
    const std::wstring tSettings = XmlEscapeWide(LookupString(lang, 125u));
    const std::wstring tManage = XmlEscapeWide(LookupString(lang, 122u));
    const std::wstring xml =
        L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n"
        L"<applications xmlns=\"http://schemas.microsoft.com/windows/cpltasks/v1\" "
        L"xmlns:sh=\"http://schemas.microsoft.com/windows/tasks/v1\">\r\n"
        L"  <application id=\"{0142E4D0-FB7A-11DC-BA4A-000FFE7AB428}\">\r\n"
        L"    <sh:task id=\"{0142E4D4-FB7A-11DC-BA4A-000FFE7AB428}\">\r\n"
        L"      <sh:name>" + tSettings + L"</sh:name>\r\n"
        L"      <sh:keywords>biometric;biometrics;fingerprint;biometrici;impronta;biom\u00E9trie;empreinte;huella;impress\u00E3o;fingerabdruck;biometrie</sh:keywords>\r\n"
        L"      <sh:command>explorer.exe shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\2\\::{0142E4D0-FB7A-11DC-BA4A-000FFE7AB428}\\BiometricsSettings</sh:command>\r\n"
        L"    </sh:task>\r\n"
        L"    <sh:task id=\"{0142E4D5-FB7A-11DC-BA4A-000FFE7AB428}\">\r\n"
        L"      <sh:name>" + tManage + L"</sh:name>\r\n"
        L"      <sh:keywords>biometric;fingerprint;manage;impronte;empreintes;huellas;impress\u00F5es;fingerabdruck</sh:keywords>\r\n"
        L"      <sh:command>explorer.exe shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\2\\::{0142E4D0-FB7A-11DC-BA4A-000FFE7AB428}</sh:command>\r\n"
        L"    </sh:task>\r\n"
        L"    <category id=\"2\">\r\n"
        L"      <sh:task idref=\"{0142E4D4-FB7A-11DC-BA4A-000FFE7AB428}\"/>\r\n"
        L"      <sh:task idref=\"{0142E4D5-FB7A-11DC-BA4A-000FFE7AB428}\"/>\r\n"
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

#ifndef BIO_GENERATOR_ONLY

// ============================================================================
// 4. Pinned Official Microsoft Symbol Server Files (Windows 7 SP1 x64)
// ============================================================================

struct FileSpec {
    const wchar_t* name;
    const wchar_t* msdlPath;
    size_t size;
    const char* sha256;
};

static const FileSpec kFiles[] = {
    {
        L"biocpl.dll",
        L"biocpl.dll/4CE7C4FA7f000/biocpl.dll",
        504320u,
        "9e2d2b199ae8048fbfce693c1c02edc02d8f94028927fc8fb1832a2481d3731a",
    },
    {
        L"dui70.dll",
        L"dui70.dll/56F58A79f2000/dui70.dll",
        977408u,
        "7eaa6ddf25510c466d55ba84fe1ea05e2c41aad09c0a1ffa8a3d55718aad147b",
    },
    {
        L"duser.dll",
        L"duser.dll/4A5BDF2643000/duser.dll",
        260608u,
        "dd663029b2eb7b12fdb00fce403d8326141e540e3b9ce84cd5871473d3e2e2cf",
    },
};

// Official Windows 7 Biometric Devices COM & Shell GUIDs
static const wchar_t* kAppletClsid          = L"{0142E4D0-FB7A-11DC-BA4A-000FFE7AB428}";
static const wchar_t* kLayoutFolderClsid    = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";
static const wchar_t* kVirtualFactoryClsid  = L"{0142E4D1-FB7A-11DC-BA4A-000FFE7AB428}";
static const wchar_t* kElemProviderClsid    = L"{0142E4D2-FB7A-11DC-BA4A-000FFE7AB428}";
static const wchar_t* kInitializerClsid     = L"{0142E4D3-FB7A-11DC-BA4A-000FFE7AB428}";
static const wchar_t* kWinBioPolicyClsid    = L"{52CFFDFE-A115-4164-AFB1-EAD719C52C15}";
static const wchar_t* kTaskHandler1Clsid    = L"{8E35B548-F174-4C7D-81E2-8ED33126F6FD}";
static const wchar_t* kTaskHandler2Clsid    = L"{CCFB7955-B4DC-42CE-893D-884D72DD6B19}";
static const wchar_t* kTaskHandler3Clsid    = L"{CBC84B69-69EA-439B-B791-DF15F60333CF}";
static const wchar_t* kIWinBioPolicyIid     = L"{1037B3EB-2876-4226-A0E6-1B718CBBCCB2}";
static const wchar_t* kWinBioPolicyLibId    = L"{84E2B44A-0627-43AE-AB85-C0E3C0FFFD49}";
static const wchar_t* kPsoaInterfaceClsid   = L"{00020424-0000-0000-C000-000000000046}";

enum class ProcessRole { Other, Explorer, Control, DllHost, ComService };

static ProcessRole g_role = ProcessRole::Other;
static std::atomic<int> g_langSetting{0};
static std::atomic<bool> g_showTasks{true};
static std::atomic<bool> g_shuttingDown{false};
static HANDLE g_stopEvent = nullptr;
// no_destroy: Explorer exits without Wh_ModUninit, so a still-joinable std::thread
// would call std::terminate() from its destructor during process shutdown.
[[clang::no_destroy]] static std::optional<std::thread> g_setupThread;
static std::atomic<HMODULE> g_hDui{nullptr};
static std::atomic<HMODULE> g_hBioCpl{nullptr};

static std::mutex g_inetMutex;
static HINTERNET g_activeInet = nullptr;
static HINTERNET g_activeUrl = nullptr;

// ============================================================================
// 5. Utilities: Settings, Paths, Atomic File I/O, MSDL Download
// ============================================================================

static std::wstring LowerStr(std::wstring s) {
    for (wchar_t& c : s) c = towlower(c);
    return s;
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
}

static Lang CurrentLang() {
    return static_cast<Lang>(g_langSetting.load(std::memory_order_acquire));
}

static std::wstring CplDllFileName(Lang lang) {
    return std::wstring(L"biocpl-") + GetLangTable(lang).code + L".dll";
}

static std::wstring TasksXmlFileName(Lang lang) {
    return std::wstring(L"BiometricDevices-") + GetLangTable(lang).code + L".tasks.xml";
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

// Verifies the file through a handle that denies writers and deletes (FILE_SHARE_READ only),
// then loads it while that handle is still open, so the verified bytes are the loaded bytes
// (no TOCTOU). Dependencies are searched in System32 only; b7dui.dll is loaded first, so
// biocpl's import of it binds to the module that is already in memory.
static HMODULE LoadVerifiedModuleImpl(const std::wstring& path, const Bytes& expected) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Cannot open for verification: %s", path.c_str());
        return nullptr;
    }
    struct HandleCloser {
        HANDLE& h;
        ~HandleCloser() { if (h != INVALID_HANDLE_VALUE) CloseHandle(h); }
    } closer{h};
    LARGE_INTEGER sz{};
    bool ok = GetFileSizeEx(h, &sz) && sz.QuadPart > 0 &&
              static_cast<unsigned long long>(sz.QuadPart) == expected.size();
    if (ok) {
        Bytes disk(expected.size());
        size_t got = 0;
        while (ok && got < disk.size()) {
            DWORD rd = 0;
            const DWORD chunk = static_cast<DWORD>(std::min<size_t>(disk.size() - got, 1u << 20));
            if (!ReadFile(h, disk.data() + got, chunk, &rd, nullptr) || rd == 0) ok = false;
            else got += rd;
        }
        ok = ok && disk == expected;
    }
    if (!ok) {
        Wh_Log(L"Verification mismatch before LoadLibrary: %s", path.c_str());
        return nullptr;
    }
    // The handle stays open (closed by HandleCloser) until LoadLibraryExW has returned.
    return LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
}

static HMODULE LoadVerifiedModule(const std::wstring& path, const Bytes& expected) {
    try {
        return LoadVerifiedModuleImpl(path, expected);
    } catch (...) {
        Wh_Log(L"Exception while verifying/loading: %s", path.c_str());
        return nullptr;
    }
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

// Narrow pre-filter: true only if the subkey names one of this mod's GUIDs or is
// ...\ControlPanel\NameSpace. Everything else is passed straight to the real API.
static bool MayBeVirtual(LPCWSTR sub) {
    if (!sub || !*sub) return false;
    const wchar_t* const guids[] = {
        kAppletClsid, kVirtualFactoryClsid, kElemProviderClsid, kInitializerClsid, kWinBioPolicyClsid,
        kTaskHandler1Clsid, kTaskHandler2Clsid, kTaskHandler3Clsid, kIWinBioPolicyIid, kWinBioPolicyLibId,
    };
    for (const wchar_t* p = sub; *p; ++p) {
        if (*p != L'{') continue;
        for (const wchar_t* g : guids) {
            if (_wcsnicmp(p, g, 38) == 0) return true;
        }
    }
    static const wchar_t kNs[] = L"controlpanel\\namespace";
    size_t len = wcslen(sub);
    while (len > 0 && sub[len - 1] == L'\\') --len;
    const size_t nsLen = (sizeof(kNs) / sizeof(kNs[0])) - 1;
    if (len >= nsLen && _wcsnicmp(sub + len - nsLen, kNs, nsLen) == 0 &&
        (len == nsLen || sub[len - nsLen - 1] == L'\\')) {
        return true;
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
    if (lowerPath.find(L"\\registry\\user") != std::wstring::npos) return false;
    return EndsWithKey(lowerPath, L"controlpanel\\namespace");
}

// Ephemeral process-private hive handle pool via RegLoadAppKeyW (zero HKLM/HKCU writes)
struct TrackedHandle {
    std::wstring path;
    bool fake = false;
};

static std::shared_mutex g_handleMutex;
static std::unordered_map<HKEY, TrackedHandle> g_handles;
// Number of entries in g_handles; lets the hot path skip the lock while nothing is tracked.
static std::atomic<size_t> g_handleCount{0};
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

using RegOpenKeyExW_t    = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
using RegOpenKeyW_t      = LSTATUS(WINAPI*)(HKEY, LPCWSTR, PHKEY);
using RegCreateKeyExW_t  = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
using RegCloseKey_t      = LSTATUS(WINAPI*)(HKEY);
using RegQueryValueExW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegGetValueW_t     = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
using RegEnumKeyExW_t    = LSTATUS(WINAPI*)(HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPWSTR, LPDWORD, PFILETIME);
using RegEnumKeyW_t      = LSTATUS(WINAPI*)(HKEY, DWORD, LPWSTR, DWORD);
using RegEnumValueW_t    = LSTATUS(WINAPI*)(HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegQueryInfoKeyW_t = LSTATUS(WINAPI*)(HKEY, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);

static RegOpenKeyExW_t    RegOpenKeyExW_Orig;
static RegOpenKeyW_t      RegOpenKeyW_Orig;
static RegCreateKeyExW_t  RegCreateKeyExW_Orig;
static RegCloseKey_t      RegCloseKey_Orig;
static RegQueryValueExW_t RegQueryValueExW_Orig;
static RegGetValueW_t     RegGetValueW_Orig;
static RegEnumKeyExW_t    RegEnumKeyExW_Orig;
static RegEnumKeyW_t      RegEnumKeyW_Orig;
static RegEnumValueW_t    RegEnumValueW_Orig;
static RegQueryInfoKeyW_t RegQueryInfoKeyW_Orig;

static HKEY NewFakeHandle(const std::wstring& lowerPath) {
    std::unique_lock<std::shared_mutex> lk(g_handleMutex);
    HKEY root = EnsureScratchRootLocked();
    if (!root) return nullptr;
    HKEY dup = nullptr;
    if (RegOpenKeyExW_Orig(root, nullptr, 0, KEY_READ, &dup) != ERROR_SUCCESS || !dup) return nullptr;
    g_handles[dup] = {lowerPath, true};
    g_handleCount.store(g_handles.size(), std::memory_order_release);
    return dup;
}

// Real handles are tracked only for ...\ControlPanel\NameSpace parents, which the enum/query hooks
// must recognise; every other real key is resolved on demand with NtQueryKey.
static void TrackRealHandle(HKEY h, const std::wstring& lowerPath) {
    if (!h) return;
    std::unique_lock<std::shared_mutex> lk(g_handleMutex);
    g_handles[h] = {lowerPath, false};
    g_handleCount.store(g_handles.size(), std::memory_order_release);
}

static bool LookupHandle(HKEY h, std::wstring& path, bool& fake) {
    if (!h || g_handleCount.load(std::memory_order_acquire) == 0) return false;
    std::shared_lock<std::shared_mutex> lk(g_handleMutex);
    auto it = g_handles.find(h);
    if (it == g_handles.end()) return false;
    path = it->second.path;
    fake = it->second.fake;
    return true;
}

static void UntrackHandle(HKEY h) {
    if (!h || g_handleCount.load(std::memory_order_acquire) == 0) return;
    std::unique_lock<std::shared_mutex> lk(g_handleMutex);
    g_handles.erase(h);
    g_handleCount.store(g_handles.size(), std::memory_order_release);
}

static void ForgetAllHandles() {
    std::unique_lock<std::shared_mutex> lk(g_handleMutex);
    g_handles.clear();
    g_handleCount.store(0, std::memory_order_release);
}

// Lowercase NT path of a real key (\registry\machine\...), via NtQueryKey(KeyNameInformation).
static bool QueryRealKeyPath(HKEY h, std::wstring& out) {
    using NtQueryKey_t = LONG(NTAPI*)(HANDLE, int, PVOID, ULONG, PULONG);
    static const NtQueryKey_t fn = reinterpret_cast<NtQueryKey_t>(reinterpret_cast<void*>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey")));
    if (!fn || !h) return false;
    std::vector<unsigned char> buf(1024);
    for (int attempt = 0; attempt < 3; ++attempt) {
        ULONG need = 0;
        const LONG st = fn(h, 3 /*KeyNameInformation*/, buf.data(), static_cast<ULONG>(buf.size()), &need);
        if (st >= 0) {
            const ULONG bytes = *reinterpret_cast<ULONG*>(buf.data());
            if (bytes + sizeof(ULONG) > buf.size()) return false;
            out.assign(reinterpret_cast<const wchar_t*>(buf.data() + sizeof(ULONG)), bytes / sizeof(wchar_t));
            out = LowerStr(out);
            return true;
        }
        if (st == static_cast<LONG>(0x80000005) || st == static_cast<LONG>(0xC0000023)) {
            buf.resize(std::max<size_t>(need, buf.size() * 2));
            continue;
        }
        return false;
    }
    return false;
}

// Decides whether an open/create/get on (hKey, sub) can concern a virtual key and, if so,
// produces the parent's lowercase path. Returns false => caller must use the real API unchanged.
static bool ResolveBase(HKEY hKey, LPCWSTR sub, std::wstring& base, bool& baseFake) {
    baseFake = false;
    if (LookupHandle(hKey, base, baseFake)) return true;  // fake handle or namespace parent
    if (!MayBeVirtual(sub)) return false;
    const wchar_t* pre = PredefinedName(hKey);
    if (*pre) { base = pre; return true; }
    return QueryRealKeyPath(hKey, base);
}

static void ReleaseScratchRoot() {
    HKEY root = nullptr;
    std::wstring path;
    {
        std::unique_lock<std::shared_mutex> lk(g_handleMutex);
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
    // Win7 tooltip text (string id 2) as a literal, so it shows in all 6 languages without a resource lookup
    const std::wstring infoTip = LookupString(lang, 2u);
    const std::wstring applet = L"clsid\\" + LowerStr(kAppletClsid);

    std::vector<Val> appletVals = {
        S(L"", REG_SZ, dispName),
        S(L"LocalizedString", REG_EXPAND_SZ, L"@" + cplPath + L",-1"),
        S(L"InfoTip", REG_SZ, infoTip),
        S(L"System.ApplicationName", REG_SZ, L"Microsoft.BiometricDevices"),
        S(L"System.ControlPanel.Category", REG_SZ, L"2"),
        D(L"{305CA226-D286-468e-B848-2B2E8E697B74} 2", 2u),
        D(L"System.ControlPanel.EnableInSafeMode", 3u),
        S(L"System.Software.TasksFileUrl", REG_SZ, showTasks ? tasksPath : L"Internal"),
    };
    add(applet, std::move(appletVals),
        nameOnly ? std::vector<std::wstring>{}
                 : std::vector<std::wstring>{L"DefaultIcon", L"InProcServer32", L"Instance", L"ShellFolder"});

    add(L"controlpanel\\namespace\\" + LowerStr(kAppletClsid),
        {S(L"", REG_SZ, dispName)}, {});
    if (nameOnly) return t;

    // LayoutFolder shell applet registration
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

    // All 6 internal biocpl.dll COM classes
    struct BioClass { const wchar_t* clsid; const wchar_t* desc; };
    static const BioClass kBioClasses[] = {
        {kElemProviderClsid, L"Biometrics Control Panel Element Provider"},
        {kInitializerClsid,  L"Biometrics Control Panel Initializer"},
        {kWinBioPolicyClsid, L"Biometrics Policy Object"},
        {kTaskHandler1Clsid, L"Biometrics Task Handler 1"},
        {kTaskHandler2Clsid, L"Biometrics Task Handler 2"},
        {kTaskHandler3Clsid, L"Biometrics Task Handler 3"},
    };
    for (const BioClass& bc : kBioClasses) {
        const std::wstring root = L"clsid\\" + LowerStr(bc.clsid);
        add(root, {S(L"", REG_SZ, bc.desc)}, {L"InProcServer32"});
        add(root + L"\\inprocserver32",
            {S(L"", REG_SZ, cplPath),
             S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
    }

    // UAC Elevation Virtual Factory (shpafact.dll) for elevated Biometric Settings policy object
    const std::wstring factory = L"clsid\\" + LowerStr(kVirtualFactoryClsid);
    add(factory,
        {S(L"", REG_SZ, L"Virtual Factory for Biometrics"),
         S(L"AppId", REG_SZ, kVirtualFactoryClsid),
         S(L"LocalizedString", REG_EXPAND_SZ, L"@" + cplPath + L",-1")},
        {L"Elevation", L"InProcServer32", L"VirtualServerObjects"});
    add(factory + L"\\elevation",
        {D(L"Enabled", 1u),
         S(L"IconReference", REG_EXPAND_SZ, L"@" + cplPath + L",-1")}, {});
    add(factory + L"\\inprocserver32",
        {S(L"", REG_EXPAND_SZ, L"%SystemRoot%\\System32\\shpafact.dll"),
         S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
    add(factory + L"\\virtualserverobjects",
        {S(kWinBioPolicyClsid, REG_SZ, L"")}, {});

    add(L"appid\\" + LowerStr(kVirtualFactoryClsid),
        {S(L"", REG_SZ, L"Virtual Factory for Biometrics"),
         S(L"DllSurrogate", REG_SZ, L"")}, {});

    // Cross-process COM marshaling for IWinBioPolicy via PSOAInterface + TypeLib
    const std::wstring iface = L"interface\\" + LowerStr(kIWinBioPolicyIid);
    add(iface, {S(L"", REG_SZ, L"IWinBioPolicy")}, {L"ProxyStubClsid32", L"TypeLib"});
    add(iface + L"\\proxystubclsid32", {S(L"", REG_SZ, kPsoaInterfaceClsid)}, {});
    add(iface + L"\\typelib",
        {S(L"", REG_SZ, kWinBioPolicyLibId),
         S(L"Version", REG_SZ, L"1.0")}, {});

    const std::wstring tlib = L"typelib\\" + LowerStr(kWinBioPolicyLibId);
    add(tlib, {}, {L"1.0"});
    add(tlib + L"\\1.0", {S(L"", REG_SZ, L"winbiopolicy 1.0 Type Library")}, {L"0", L"FLAGS"});
    add(tlib + L"\\1.0\\0", {}, {L"win64"});
    add(tlib + L"\\1.0\\0\\win64", {S(L"", REG_SZ, cplPath)}, {});
    add(tlib + L"\\1.0\\flags", {S(L"", REG_SZ, L"0")}, {});

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

static LSTATUS HandleOpen(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    if (samDesired & KEY_WOW64_32KEY) {
        return RegOpenKeyExW_Orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }
    std::wstring basePath;
    bool baseFake = false;
    if (!ResolveBase(hKey, lpSubKey, basePath, baseFake)) {
        return RegOpenKeyExW_Orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
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
        if (IsNamespaceParent(full)) TrackRealHandle(realOut, full);
        if (phkResult) *phkResult = realOut;
    }
    return st;
}

static LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    try {
        return HandleOpen(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    } catch (...) {
        return RegOpenKeyExW_Orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }
}

static LSTATUS WINAPI RegOpenKeyW_Hook(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult) {
    // Only calls that concern a virtual key are rerouted; everything else (including the
    // NULL/empty subkey case that returns hKey itself) goes to the real RegOpenKeyW unchanged.
    try {
        if (lpSubKey && *lpSubKey && phkResult) {
            std::wstring base;
            bool baseFake = false;
            if (ResolveBase(hKey, lpSubKey, base, baseFake)) {
                // RegOpenKeyW opens with MAXIMUM_ALLOWED, so the real-handle path must too.
                return HandleOpen(hKey, lpSubKey, 0, MAXIMUM_ALLOWED, phkResult);
            }
        }
    } catch (...) {
    }
    return RegOpenKeyW_Orig(hKey, lpSubKey, phkResult);
}

static LSTATUS WINAPI RegCreateKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass,
                                           DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSec,
                                           PHKEY phkResult, LPDWORD lpdwDisposition) {
    try {
        if (samDesired & KEY_WOW64_32KEY) {
            return RegCreateKeyExW_Orig(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSec, phkResult, lpdwDisposition);
        }
        std::wstring basePath;
        bool baseFake = false;
        if (ResolveBase(hKey, lpSubKey, basePath, baseFake)) {
            const std::wstring full = JoinPath(basePath, lpSubKey);
            const auto table = GetTable();
            if (table && FindKey(*table, full)) {
                if (!phkResult) return ERROR_INVALID_PARAMETER;
                HKEY fakeH = NewFakeHandle(full);
                if (fakeH) {
                    *phkResult = fakeH;
                    if (lpdwDisposition) *lpdwDisposition = REG_OPENED_EXISTING_KEY;
                    return ERROR_SUCCESS;
                }
            }
            if (baseFake) return ERROR_ACCESS_DENIED;
            HKEY realOut = nullptr;
            const LSTATUS st = RegCreateKeyExW_Orig(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
                                                    lpSec, &realOut, lpdwDisposition);
            if (st == ERROR_SUCCESS && realOut) {
                if (IsNamespaceParent(full)) TrackRealHandle(realOut, full);
                if (phkResult) *phkResult = realOut;
            }
            return st;
        }
    } catch (...) {
    }
    return RegCreateKeyExW_Orig(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSec, phkResult, lpdwDisposition);
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
            const Val* v = k ? FindValue(*k, lpValueName) : nullptr;
            if (!v) return ERROR_FILE_NOT_FOUND;
            return WriteVal(*v, lpType, lpData, lpcbData, false);
        }
    } catch (...) {
    }
    return RegQueryValueExW_Orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegGetValueW_Hook(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags,
                                        LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    try {
        if (dwFlags & RRF_SUBKEY_WOW6432KEY) {
            return RegGetValueW_Orig(hKey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
        }
        std::wstring basePath;
        bool baseFake = false;
        if (ResolveBase(hKey, lpSubKey, basePath, baseFake)) {
            const std::wstring full = JoinPath(basePath, lpSubKey);
            const auto table = GetTable();
            const Key* k = table ? FindKey(*table, full) : nullptr;
            if (k) {
                const Val* v = FindValue(*k, lpValue);
                if (!v) return ERROR_FILE_NOT_FOUND;
                const bool noExpand = (dwFlags & RRF_NOEXPAND) != 0;
                const DWORD typeMask = dwFlags & 0x0000FFFFu;
                if (typeMask != 0 && typeMask != RRF_RT_ANY) {
                    const bool okType =
                        (v->type == REG_DWORD && (typeMask & RRF_RT_REG_DWORD)) ||
                        (v->type == REG_SZ && (typeMask & RRF_RT_REG_SZ)) ||
                        (v->type == REG_EXPAND_SZ && ((typeMask & RRF_RT_REG_EXPAND_SZ) ||
                                                      (!noExpand && (typeMask & RRF_RT_REG_SZ))));
                    if (!okType) return ERROR_UNSUPPORTED_TYPE;
                }
                return WriteVal(*v, pdwType, reinterpret_cast<LPBYTE>(pvData), pcbData, !noExpand);
            }
            if (baseFake) return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
    }
    return RegGetValueW_Orig(hKey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
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
            if (!lpData && !lpcbData && !lpType) return ERROR_SUCCESS;
            return WriteVal(v, lpType, lpData, lpcbData, false);
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

// ---- Direct COM Activation for Private Applet & Biometric Classes ----
struct ComClassEntry {
    GUID clsid;
    int target;  // 0 = shdocvw.dll, 1 = private biocpl-<lang>.dll
};

static const ComClassEntry kComClasses[] = {
    {{0x0142E4D0, 0xFB7A, 0x11DC, {0xBA, 0x4A, 0x00, 0x0F, 0xFE, 0x7A, 0xB4, 0x28}}, 0},
    {{0x0142E4D2, 0xFB7A, 0x11DC, {0xBA, 0x4A, 0x00, 0x0F, 0xFE, 0x7A, 0xB4, 0x28}}, 1},
    {{0x0142E4D3, 0xFB7A, 0x11DC, {0xBA, 0x4A, 0x00, 0x0F, 0xFE, 0x7A, 0xB4, 0x28}}, 1},
    {{0x52CFFDFE, 0xA115, 0x4164, {0xAF, 0xB1, 0xEA, 0xD7, 0x19, 0xC5, 0x2C, 0x15}}, 1},
    {{0x8E35B548, 0xF174, 0x4C7D, {0x81, 0xE2, 0x8E, 0xD3, 0x31, 0x26, 0xF6, 0xFD}}, 1},
    {{0xCCFB7955, 0xB4DC, 0x42CE, {0x89, 0x3D, 0x88, 0x4D, 0x72, 0xDD, 0x6B, 0x19}}, 1},
    {{0xCBC84B69, 0x69EA, 0x439B, {0xB7, 0x91, 0xDF, 0x15, 0xF6, 0x03, 0x33, 0xCF}}, 1},
};

static const GUID kIidClassFactory = {0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static HRESULT GetOurFactory(REFCLSID clsid, REFIID riid, void** ppv) {
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
        mod = g_hBioCpl.load(std::memory_order_acquire);
    }
    if (!mod) return REGDB_E_CLASSNOTREG;
    using DllGetClassObject_t = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, LPVOID*);
    auto fn = reinterpret_cast<DllGetClassObject_t>(reinterpret_cast<void*>(GetProcAddress(mod, "DllGetClassObject")));
    return fn ? fn(clsid, riid, ppv) : REGDB_E_CLASSNOTREG;
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstanceCombase_Orig;
static CoCreateInstance_t CoCreateInstanceOle32_Orig;

static HRESULT HandleCoCreateInstance(REFCLSID rclsid, LPUNKNOWN outer, DWORD ctx, REFIID riid, LPVOID* ppv,
                                      CoCreateInstance_t orig) {
    try {
        if (ppv && (ctx & CLSCTX_INPROC_SERVER) && GetTable()) {
            IClassFactory* factory = nullptr;
            HRESULT hr = GetOurFactory(rclsid, kIidClassFactory, reinterpret_cast<void**>(&factory));
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

static HRESULT WINAPI CoCreateInstanceCombase_Hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID r, LPVOID* p) {
    return HandleCoCreateInstance(c, o, ctx, r, p, CoCreateInstanceCombase_Orig);
}

static HRESULT WINAPI CoCreateInstanceOle32_Hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID r, LPVOID* p) {
    return HandleCoCreateInstance(c, o, ctx, r, p, CoCreateInstanceOle32_Orig);
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
        {"RegOpenKeyExW",    (void*)RegOpenKeyExW_Hook,    (void**)&RegOpenKeyExW_Orig,    true},
        {"RegOpenKeyW",      (void*)RegOpenKeyW_Hook,      (void**)&RegOpenKeyW_Orig,      false},
        {"RegCreateKeyExW",  (void*)RegCreateKeyExW_Hook,  (void**)&RegCreateKeyExW_Orig,  true},
        {"RegCloseKey",      (void*)RegCloseKey_Hook,      (void**)&RegCloseKey_Orig,      true},
        {"RegQueryValueExW", (void*)RegQueryValueExW_Hook, (void**)&RegQueryValueExW_Orig, true},
        {"RegGetValueW",     (void*)RegGetValueW_Hook,     (void**)&RegGetValueW_Orig,     true},
        {"RegEnumValueW",    (void*)RegEnumValueW_Hook,    (void**)&RegEnumValueW_Orig,    false},
        {"RegEnumKeyExW",    (void*)RegEnumKeyExW_Hook,    (void**)&RegEnumKeyExW_Orig,    true},
        {"RegEnumKeyW",      (void*)RegEnumKeyW_Hook,      (void**)&RegEnumKeyW_Orig,      true},
        {"RegQueryInfoKeyW", (void*)RegQueryInfoKeyW_Hook, (void**)&RegQueryInfoKeyW_Orig, true},
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
    void* pCombase = combase ? reinterpret_cast<void*>(GetProcAddress(combase, "CoCreateInstance")) : nullptr;
    void* pOle32 = ole32 ? reinterpret_cast<void*>(GetProcAddress(ole32, "CoCreateInstance")) : nullptr;
    if (pCombase) Wh_SetFunctionHook(pCombase, (void*)CoCreateInstanceCombase_Hook, (void**)&CoCreateInstanceCombase_Orig);
    if (pOle32 && pOle32 != pCombase) {
        Wh_SetFunctionHook(pOle32, (void*)CoCreateInstanceOle32_Hook, (void**)&CoCreateInstanceOle32_Orig);
    }
    return true;
}

}  // namespace vreg

// ============================================================================
// 6b. Private biocpl-<lang>.dll IAT Shims (Win10/11 WbioSrvc Anti-Hang & Nav)
//
// Windows 10/11's winbio.dll wraps every RPC call (WinBioEnumBiometricUnits,
// WinBioGetCredentialState, WinBioOpenSession) in a 66-iteration (~10.05s)
// retry loop when WbioSrvc is disabled or unreachable, and wbiosrvc.dll can
// block up to 30s on cold trigger-start. Because Win7 biocpl.dll makes all
// three calls sequentially (the last one on the UI thread in
// CHubPage::RefreshPage) before hiding the empty WaitingForServiceControl
// placeholder, opening Biometric Devices can otherwise show a blank white pane
// for 30+ seconds.
//
// We patch ONLY our private biocpl-<lang>.dll's IAT (leaving system winbio.dll
// untouched for all other Explorer/OS components) and cleanly restore every
// slot in Wh_ModBeforeUninit / Wh_ModUninit before unloading.
// ============================================================================

namespace bio_shim {

struct alignas(4) BioIdentityRaw {
    uint8_t bytes[0x4C];  // sizeof(WINBIO_IDENTITY) == 76 bytes on x64
};
static_assert(sizeof(BioIdentityRaw) == 0x4C, "WINBIO_IDENTITY ABI size mismatch");

using WinBioEnumBiometricUnits_t = HRESULT(WINAPI*)(ULONG, PVOID*, SIZE_T*);
using WinBioGetCredentialState_t = HRESULT(WINAPI*)(BioIdentityRaw, ULONG, ULONG*);
using WinBioOpenSession_t = HRESULT(WINAPI*)(ULONG, ULONG, ULONG, ULONG*, SIZE_T, const GUID*, ULONG*);
using WinBioFree_t = HRESULT(WINAPI*)(PVOID);
using WinBioCloseSession_t = HRESULT(WINAPI*)(ULONG);
using SHStrDupW_t = HRESULT(WINAPI*)(LPCWSTR, LPWSTR*);

using SetupDiGetClassDevsW_t = HDEVINFO(WINAPI*)(const GUID*, PCWSTR, HWND, DWORD);
using SetupDiEnumDeviceInterfaces_t = BOOL(WINAPI*)(HDEVINFO, PSP_DEVINFO_DATA, const GUID*, DWORD,
                                                    PSP_DEVICE_INTERFACE_DATA);
using SetupDiDestroyDeviceInfoList_t = BOOL(WINAPI*)(HDEVINFO);

constexpr HRESULT kHrServiceUnavailable = static_cast<HRESULT>(0x800106D9);  // EPT_S_NOT_REGISTERED
constexpr ULONG kWinBioCredentialNotSet = 1u;
constexpr ULONGLONG kFastFailCooldownMs = 15000ull;
constexpr DWORD kTimeoutStoppedNoHwMs = 450u;
constexpr DWORD kTimeoutStoppedWithHwMs = 2500u;
constexpr DWORD kTimeoutRunningWorkerMs = 1800u;
constexpr DWORD kTimeoutOpenSessionUiMs = 600u;
constexpr int kMaxInFlightProbes = 2;

// GUID_DEVINTERFACE_BIOMETRIC_READER {53D29EF7-377C-4D14-864B-EB3A85769359}
static constexpr GUID kGuidDevInterfaceBiometricReader = {
    0x53d29ef7u, 0x377cu, 0x4d14u, {0x86u, 0x4bu, 0xebu, 0x3au, 0x85u, 0x76u, 0x93u, 0x59u}};

enum class WbioSvcState : int {
    Running = 0,
    StartPending = 1,
    StoppedStartable = 2,
    DisabledOrMissing = 3,
    Unknown = 4,
};

struct PatchedIatSlot {
    void** slot;
    void* origFn;
};

static std::mutex g_iatMutex;
static std::vector<PatchedIatSlot> g_patchedSlots;

static std::atomic<WinBioEnumBiometricUnits_t> g_enumUnits_Orig{nullptr};
static std::atomic<WinBioGetCredentialState_t> g_getCredState_Orig{nullptr};
static std::atomic<WinBioOpenSession_t> g_openSession_Orig{nullptr};
static std::atomic<WinBioFree_t> g_free_Orig{nullptr};
static std::atomic<WinBioCloseSession_t> g_closeSession_Orig{nullptr};
static std::atomic<SHStrDupW_t> g_shStrDupW_Orig{nullptr};

static std::atomic<ULONGLONG> g_fastFailUntilTick{0};
static std::atomic<int> g_inFlightProbes{0};

static bool IsWinBioTransportFailure(HRESULT hr) {
    if (SUCCEEDED(hr)) return false;
    const WORD fac = static_cast<WORD>((static_cast<DWORD>(hr) >> 16) & 0x1FFFu);
    const WORD code = static_cast<WORD>(static_cast<DWORD>(hr) & 0xFFFFu);
    if (fac == FACILITY_RPC || fac == FACILITY_WIN32) {
        switch (code) {
            case 1058u:  // ERROR_SERVICE_DISABLED
            case 1060u:  // ERROR_SERVICE_DOES_NOT_EXIST
            case 1062u:  // ERROR_SERVICE_NOT_ACTIVE
            case 1460u:  // ERROR_TIMEOUT
            case 1708u:  // RPC_S_NO_ENDPOINT_FOUND
            case 1717u:  // RPC_S_UNKNOWN_IF
            case 1722u:  // RPC_S_SERVER_UNAVAILABLE
            case 1723u:  // RPC_S_SERVER_TOO_BUSY
            case 1726u:  // RPC_S_CALL_FAILED
            case 1753u:  // EPT_S_NOT_REGISTERED
                return true;
            default:
                break;
        }
    }
    return false;
}

static bool ReadWbioStartTypeFromRegistry(DWORD& outStartType) {
    DWORD val = 0;
    DWORD cb = sizeof(val);
    const LSTATUS st = RegGetValueW(
        HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\WbioSrvc", L"Start",
        RRF_RT_REG_DWORD, nullptr, &val, &cb);
    if (st == ERROR_SUCCESS && cb == sizeof(val)) {
        outStartType = val;
        return true;
    }
    return false;
}

static WbioSvcState QueryWbioServiceState() {
    SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm) {
        DWORD startType = 0;
        if (ReadWbioStartTypeFromRegistry(startType) && startType == SERVICE_DISABLED) {
            return WbioSvcState::DisabledOrMissing;
        }
        return WbioSvcState::Unknown;
    }

    bool canQueryConfig = true;
    SC_HANDLE svc = OpenServiceW(scm, L"WbioSrvc", SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG);
    if (!svc) {
        const DWORD err = GetLastError();
        if (err == ERROR_SERVICE_DOES_NOT_EXIST) {
            CloseServiceHandle(scm);
            return WbioSvcState::DisabledOrMissing;
        }
        canQueryConfig = false;
        svc = OpenServiceW(scm, L"WbioSrvc", SERVICE_QUERY_STATUS);
    }
    if (!svc) {
        CloseServiceHandle(scm);
        DWORD startType = 0;
        if (ReadWbioStartTypeFromRegistry(startType) && startType == SERVICE_DISABLED) {
            return WbioSvcState::DisabledOrMissing;
        }
        return WbioSvcState::Unknown;
    }

    SERVICE_STATUS_PROCESS ssp{};
    DWORD needed = 0;
    const BOOL gotStatus = QueryServiceStatusEx(
        svc, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE>(&ssp), sizeof(ssp), &needed);
    if (gotStatus) {
        if (ssp.dwCurrentState == SERVICE_RUNNING) {
            CloseServiceHandle(svc);
            CloseServiceHandle(scm);
            return WbioSvcState::Running;
        }
        if (ssp.dwCurrentState == SERVICE_START_PENDING) {
            CloseServiceHandle(svc);
            CloseServiceHandle(scm);
            return WbioSvcState::StartPending;
        }
    }

    DWORD startType = SERVICE_DEMAND_START;
    bool gotStartType = false;
    if (canQueryConfig) {
        alignas(QUERY_SERVICE_CONFIGW) uint8_t cfgBuf[4096];
        DWORD cfgNeed = 0;
        if (QueryServiceConfigW(svc, reinterpret_cast<LPQUERY_SERVICE_CONFIGW>(cfgBuf),
                                static_cast<DWORD>(sizeof(cfgBuf)), &cfgNeed)) {
            startType = reinterpret_cast<const QUERY_SERVICE_CONFIGW*>(cfgBuf)->dwStartType;
            gotStartType = true;
        }
    }
    CloseServiceHandle(svc);
    CloseServiceHandle(scm);

    if (!gotStartType) {
        gotStartType = ReadWbioStartTypeFromRegistry(startType);
    }
    if (gotStartType && startType == SERVICE_DISABLED) {
        return WbioSvcState::DisabledOrMissing;
    }
    return gotStatus ? WbioSvcState::StoppedStartable : WbioSvcState::Unknown;
}

template <typename Fn>
static Fn GetProcAs(HMODULE mod, const char* name) {
    return reinterpret_cast<Fn>(reinterpret_cast<void*>(GetProcAddress(mod, name)));
}

static bool HasPresentBiometricReaderDevice() {
    HMODULE hSetup = GetModuleHandleW(L"setupapi.dll");
    if (!hSetup) hSetup = LoadLibraryExW(L"setupapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hSetup) return false;

    const auto pGetClassDevs = GetProcAs<SetupDiGetClassDevsW_t>(hSetup, "SetupDiGetClassDevsW");
    const auto pEnumInterfaces =
        GetProcAs<SetupDiEnumDeviceInterfaces_t>(hSetup, "SetupDiEnumDeviceInterfaces");
    const auto pDestroyList =
        GetProcAs<SetupDiDestroyDeviceInfoList_t>(hSetup, "SetupDiDestroyDeviceInfoList");
    if (!pGetClassDevs || !pEnumInterfaces || !pDestroyList) return false;

    HDEVINFO hDev = pGetClassDevs(&kGuidDevInterfaceBiometricReader, nullptr, nullptr,
                                  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDev == INVALID_HANDLE_VALUE) return false;

    SP_DEVICE_INTERFACE_DATA did{};
    did.cbSize = sizeof(did);
    const BOOL found = pEnumInterfaces(hDev, nullptr, &kGuidDevInterfaceBiometricReader, 0, &did);
    pDestroyList(hDev);
    return found != FALSE;
}

static void TripFastFail() {
    g_fastFailUntilTick.store(GetTickCount64() + kFastFailCooldownMs, std::memory_order_release);
}

static void ClearFastFail() {
    g_fastFailUntilTick.store(0, std::memory_order_release);
}

static bool ShouldFastFailNow(bool isOpenSession, WbioSvcState& outSvc, bool& outHasHw, DWORD& outTimeoutMs) {
    outHasHw = false;
    outTimeoutMs = 0;
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        outSvc = WbioSvcState::Unknown;
        return true;
    }

    outSvc = QueryWbioServiceState();
    if (outSvc == WbioSvcState::DisabledOrMissing) {
        TripFastFail();
        return true;
    }

    const ULONGLONG now = GetTickCount64();
    if (now < g_fastFailUntilTick.load(std::memory_order_acquire)) {
        return true;
    }

    if (g_inFlightProbes.load(std::memory_order_acquire) >= kMaxInFlightProbes) {
        return true;
    }

    if (outSvc == WbioSvcState::Running) {
        outTimeoutMs = isOpenSession ? kTimeoutOpenSessionUiMs : kTimeoutRunningWorkerMs;
    } else {
        outHasHw = HasPresentBiometricReaderDevice();
        if (isOpenSession) {
            outTimeoutMs = outHasHw ? kTimeoutOpenSessionUiMs : 250u;
        } else {
            outTimeoutMs = outHasHw ? kTimeoutStoppedWithHwMs : kTimeoutStoppedNoHwMs;
        }
    }
    return false;
}

struct InFlightGuard {
    ~InFlightGuard() { g_inFlightProbes.fetch_sub(1, std::memory_order_acq_rel); }
};

// Worker threads are tracked and joined in JoinWorkers() (called from Wh_ModBeforeUninit), so the
// mod image can never be unmapped while a worker is still running its code.
static std::mutex g_workersMutex;
static std::vector<HANDLE> g_workers;

static void JoinWorkers() {
    std::vector<HANDLE> workers;
    {
        std::lock_guard<std::mutex> lk(g_workersMutex);
        workers.swap(g_workers);
    }
    for (HANDLE h : workers) {
        WaitForSingleObject(h, INFINITE);  // bounded by WinBio's own RPC timeouts
        CloseHandle(h);
    }
}

template <typename StateT>
static bool LaunchAndWait(const std::shared_ptr<StateT>& state,
                          DWORD(WINAPI* proc)(LPVOID),
                          DWORD timeoutMs) {
    state->hDone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!state->hDone) return false;

    auto* threadArg = new (std::nothrow) std::shared_ptr<StateT>(state);
    if (!threadArg) {
        CloseHandle(state->hDone);
        state->hDone = nullptr;
        return false;
    }

    {
        std::lock_guard<std::mutex> lk(g_workersMutex);
        if (g_shuttingDown.load(std::memory_order_acquire)) {
            delete threadArg;
            CloseHandle(state->hDone);
            state->hDone = nullptr;
            return false;
        }
        // Drop handles of workers that already finished.
        for (size_t i = 0; i < g_workers.size();) {
            if (WaitForSingleObject(g_workers[i], 0) == WAIT_OBJECT_0) {
                CloseHandle(g_workers[i]);
                g_workers.erase(g_workers.begin() + static_cast<std::ptrdiff_t>(i));
            } else {
                ++i;
            }
        }
        g_inFlightProbes.fetch_add(1, std::memory_order_acq_rel);
        HANDLE hThread = CreateThread(nullptr, 0, proc, threadArg, 0, nullptr);
        if (!hThread) {
            g_inFlightProbes.fetch_sub(1, std::memory_order_acq_rel);
            delete threadArg;
            CloseHandle(state->hDone);
            state->hDone = nullptr;
            return false;
        }
        try {
            g_workers.push_back(hThread);
        } catch (...) {
            // Could not record the handle; the worker still runs, but cannot be joined.
            CloseHandle(hThread);
        }
    }

    const DWORD wait = WaitForSingleObject(state->hDone, timeoutMs);
    if (wait == WAIT_OBJECT_0) return true;

    bool expected = false;
    if (state->claimed.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
        return false;  // Timed out; worker will see claimed==true and release any late result.
    }
    // Worker won the race right as WaitForSingleObject expired; wait briefly for SetEvent.
    WaitForSingleObject(state->hDone, 100u);
    return true;
}

struct EnumUnitsState {
    HANDLE hDone = nullptr;
    std::atomic<bool> claimed{false};
    WinBioEnumBiometricUnits_t fnEnum = nullptr;
    WinBioFree_t fnFree = nullptr;
    ULONG factor = 0;
    HRESULT hr = kHrServiceUnavailable;
    PVOID units = nullptr;
    SIZE_T count = 0;

    ~EnumUnitsState() {
        if (hDone) CloseHandle(hDone);
    }
};

static DWORD WINAPI EnumUnitsThreadProc(LPVOID param) {
    try {
        InFlightGuard guard;
        std::unique_ptr<std::shared_ptr<EnumUnitsState>> holder(
            static_cast<std::shared_ptr<EnumUnitsState>*>(param));
        std::shared_ptr<EnumUnitsState> st = std::move(*holder);

        PVOID units = nullptr;
        SIZE_T count = 0;
        const HRESULT hr = st->fnEnum ? st->fnEnum(st->factor, &units, &count) : kHrServiceUnavailable;
        st->hr = hr;
        st->units = units;
        st->count = count;

        bool expected = false;
        const bool callerGetsResult =
            st->claimed.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
        if (callerGetsResult) {
            SetEvent(st->hDone);
        } else {
            if (SUCCEEDED(hr)) {
                ClearFastFail();
                if (units && st->fnFree) st->fnFree(units);
            }
        }
    } catch (...) {
        Wh_Log(L"[bio] worker threw an exception");
    }
    return 0;
}

struct GetCredState {
    HANDLE hDone = nullptr;
    std::atomic<bool> claimed{false};
    WinBioGetCredentialState_t fnGetCred = nullptr;
    BioIdentityRaw identity{};
    ULONG type = 0;
    HRESULT hr = kHrServiceUnavailable;
    ULONG credState = kWinBioCredentialNotSet;

    ~GetCredState() {
        if (hDone) CloseHandle(hDone);
    }
};

static DWORD WINAPI GetCredThreadProc(LPVOID param) {
    try {
        InFlightGuard guard;
        std::unique_ptr<std::shared_ptr<GetCredState>> holder(
            static_cast<std::shared_ptr<GetCredState>*>(param));
        std::shared_ptr<GetCredState> st = std::move(*holder);

        ULONG cred = kWinBioCredentialNotSet;
        const HRESULT hr = st->fnGetCred ? st->fnGetCred(st->identity, st->type, &cred)
                                         : kHrServiceUnavailable;
        st->hr = hr;
        st->credState = cred;

        bool expected = false;
        const bool callerGetsResult =
            st->claimed.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
        if (callerGetsResult) {
            SetEvent(st->hDone);
        } else if (SUCCEEDED(hr)) {
            ClearFastFail();
        }
    } catch (...) {
        Wh_Log(L"[bio] worker threw an exception");
    }
    return 0;
}

struct OpenSessionState {
    HANDLE hDone = nullptr;
    std::atomic<bool> claimed{false};
    WinBioOpenSession_t fnOpen = nullptr;
    WinBioCloseSession_t fnClose = nullptr;
    ULONG factor = 0;
    ULONG poolType = 0;
    ULONG flags = 0;
    std::vector<ULONG> units;
    bool hasRealDbGuid = false;
    GUID dbGuid{};
    const GUID* dbSentinel = nullptr;
    HRESULT hr = kHrServiceUnavailable;
    ULONG session = 0;

    ~OpenSessionState() {
        if (hDone) CloseHandle(hDone);
    }
};

static DWORD WINAPI OpenSessionThreadProc(LPVOID param) {
    try {
        InFlightGuard guard;
        std::unique_ptr<std::shared_ptr<OpenSessionState>> holder(
            static_cast<std::shared_ptr<OpenSessionState>*>(param));
        std::shared_ptr<OpenSessionState> st = std::move(*holder);

        ULONG* pUnits = st->units.empty() ? nullptr : st->units.data();
        const SIZE_T nUnits = st->units.size();
        const GUID* pDb = st->hasRealDbGuid ? &st->dbGuid : st->dbSentinel;
        ULONG sess = 0;
        const HRESULT hr = st->fnOpen
                               ? st->fnOpen(st->factor, st->poolType, st->flags, pUnits, nUnits, pDb, &sess)
                               : kHrServiceUnavailable;
        st->hr = hr;
        st->session = sess;

        bool expected = false;
        const bool callerGetsResult =
            st->claimed.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
        if (callerGetsResult) {
            SetEvent(st->hDone);
        } else {
            if (SUCCEEDED(hr)) {
                ClearFastFail();
                if (sess && st->fnClose) st->fnClose(sess);
            }
        }
    } catch (...) {
        Wh_Log(L"[bio] worker threw an exception");
    }
    return 0;
}

static HRESULT WINAPI WinBioEnumBiometricUnits_Shim(ULONG factor, PVOID* unitSchemaArray, SIZE_T* unitCount) {
    if (unitSchemaArray) *unitSchemaArray = nullptr;
    if (unitCount) *unitCount = 0;

    try {
        const WinBioEnumBiometricUnits_t orig = g_enumUnits_Orig.load(std::memory_order_acquire);
        if (!orig) return kHrServiceUnavailable;

        const ULONGLONG t0 = GetTickCount64();
        WbioSvcState svc = WbioSvcState::Unknown;
        bool hasHw = false;
        DWORD timeoutMs = 0;
        if (ShouldFastFailNow(false, svc, hasHw, timeoutMs)) {
            Wh_Log(L"[bio] WinBioEnumBiometricUnits fast-fail (svc=%d, %llums)",
                   static_cast<int>(svc), static_cast<unsigned long long>(GetTickCount64() - t0));
            return kHrServiceUnavailable;
        }

        auto st = std::make_shared<EnumUnitsState>();
        st->fnEnum = orig;
        st->fnFree = g_free_Orig.load(std::memory_order_acquire);
        st->factor = factor;

        if (!LaunchAndWait(st, EnumUnitsThreadProc, timeoutMs)) {
            TripFastFail();
            Wh_Log(L"[bio] WinBioEnumBiometricUnits timed out after %ums (svc=%d, pnp=%d)",
                   timeoutMs, static_cast<int>(svc), hasHw ? 1 : 0);
            return kHrServiceUnavailable;
        }

        if (IsWinBioTransportFailure(st->hr)) {
            TripFastFail();
        } else if (SUCCEEDED(st->hr)) {
            ClearFastFail();
            if (unitSchemaArray) {
                *unitSchemaArray = st->units;
            } else if (st->units && st->fnFree) {
                st->fnFree(st->units);
            }
            if (unitCount) *unitCount = st->count;
        }
        Wh_Log(L"[bio] WinBioEnumBiometricUnits done: hr=0x%08X units=%zu svc=%d pnp=%d (%llums)",
               static_cast<unsigned>(st->hr), st->count, static_cast<int>(svc), hasHw ? 1 : 0,
               static_cast<unsigned long long>(GetTickCount64() - t0));
        return st->hr;
    } catch (...) {
        return kHrServiceUnavailable;
    }
}

static HRESULT WINAPI WinBioGetCredentialState_Shim(BioIdentityRaw identity, ULONG type,
                                                    ULONG* credentialState) {
    if (credentialState) *credentialState = kWinBioCredentialNotSet;

    try {
        const WinBioGetCredentialState_t orig = g_getCredState_Orig.load(std::memory_order_acquire);
        if (!orig) return kHrServiceUnavailable;

        WbioSvcState svc = WbioSvcState::Unknown;
        bool hasHw = false;
        DWORD timeoutMs = 0;
        if (ShouldFastFailNow(false, svc, hasHw, timeoutMs)) {
            return kHrServiceUnavailable;
        }

        auto st = std::make_shared<GetCredState>();
        st->fnGetCred = orig;
        st->identity = identity;
        st->type = type;

        if (!LaunchAndWait(st, GetCredThreadProc, timeoutMs)) {
            TripFastFail();
            return kHrServiceUnavailable;
        }

        if (IsWinBioTransportFailure(st->hr)) {
            TripFastFail();
        } else if (SUCCEEDED(st->hr)) {
            ClearFastFail();
            if (credentialState) *credentialState = st->credState;
        }
        return st->hr;
    } catch (...) {
        return kHrServiceUnavailable;
    }
}

static HRESULT WINAPI WinBioOpenSession_Shim(ULONG factor, ULONG poolType, ULONG flags,
                                             ULONG* unitArray, SIZE_T unitCount,
                                             const GUID* databaseId, ULONG* sessionHandle) {
    if (sessionHandle) *sessionHandle = 0;

    try {
        const WinBioOpenSession_t orig = g_openSession_Orig.load(std::memory_order_acquire);
        if (!orig) return kHrServiceUnavailable;

        WbioSvcState svc = WbioSvcState::Unknown;
        bool hasHw = false;
        DWORD timeoutMs = 0;
        if (ShouldFastFailNow(true, svc, hasHw, timeoutMs)) {
            return kHrServiceUnavailable;
        }

        auto st = std::make_shared<OpenSessionState>();
        st->fnOpen = orig;
        st->fnClose = g_closeSession_Orig.load(std::memory_order_acquire);
        st->factor = factor;
        st->poolType = poolType;
        st->flags = flags;
        if (unitArray && unitCount > 0 && unitCount <= 256u) {
            st->units.assign(unitArray, unitArray + unitCount);
        }
        if (reinterpret_cast<uintptr_t>(databaseId) > 0xFFFFu) {
            st->hasRealDbGuid = true;
            st->dbGuid = *databaseId;
        } else {
            st->hasRealDbGuid = false;
            st->dbSentinel = databaseId;
        }

        if (!LaunchAndWait(st, OpenSessionThreadProc, timeoutMs)) {
            TripFastFail();
            return kHrServiceUnavailable;
        }

        if (IsWinBioTransportFailure(st->hr)) {
            TripFastFail();
        } else if (SUCCEEDED(st->hr)) {
            ClearFastFail();
            if (sessionHandle) {
                *sessionHandle = st->session;
            } else if (st->session && st->fnClose) {
                st->fnClose(st->session);
            }
        }
        return st->hr;
    } catch (...) {
        return kHrServiceUnavailable;
    }
}

static HRESULT WINAPI SHStrDupW_Shim(LPCWSTR psz, LPWSTR* ppwsz) {
    const SHStrDupW_t orig = g_shStrDupW_Orig.load(std::memory_order_acquire);
    if (!orig) return E_FAIL;
    // Win7 biocpl.dll CHubPage::_InitNavPane passes "Microsoft.Biometrics" for the
    // left-pane "Change biometric settings" link, whereas the registered canonical
    // name in ControlPanel\NameSpace is "Microsoft.BiometricDevices".
    if (psz && _wcsicmp(psz, L"Microsoft.Biometrics") == 0) {
        return orig(L"Microsoft.BiometricDevices", ppwsz);
    }
    return orig(psz, ppwsz);
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

static void PatchBioCplIat(HMODULE hCpl) {
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
    int winbioPatched = 0;
    int shlwapiPatched = 0;

    for (const auto* desc = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + impDir.VirtualAddress);
         desc->Name != 0; ++desc) {
        const char* dllName = reinterpret_cast<const char*>(base + desc->Name);
        const bool isWinBio = (_stricmp(dllName, "winbio.dll") == 0);
        const bool isShlwapi = (_stricmp(dllName, "shlwapi.dll") == 0);
        if (!isWinBio && !isShlwapi) continue;

        const DWORD intRva = desc->OriginalFirstThunk ? desc->OriginalFirstThunk : desc->FirstThunk;
        if (!intRva || !desc->FirstThunk) continue;
        const auto* origThunk = reinterpret_cast<const IMAGE_THUNK_DATA64*>(base + intRva);
        auto* iatThunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(base + desc->FirstThunk);

        for (; origThunk->u1.AddressOfData != 0; ++origThunk, ++iatThunk) {
            if (IMAGE_SNAP_BY_ORDINAL64(origThunk->u1.Ordinal)) continue;
            const auto* ibn =
                reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(base + origThunk->u1.AddressOfData);
            const char* fn = reinterpret_cast<const char*>(ibn->Name);
            void** slot = reinterpret_cast<void**>(&iatThunk->u1.Function);

            if (isWinBio) {
                if (strcmp(fn, "WinBioFree") == 0) {
                    if (!g_free_Orig.load(std::memory_order_acquire)) {
                        g_free_Orig.store(reinterpret_cast<WinBioFree_t>(*slot), std::memory_order_release);
                    }
                } else if (strcmp(fn, "WinBioCloseSession") == 0) {
                    if (!g_closeSession_Orig.load(std::memory_order_acquire)) {
                        g_closeSession_Orig.store(reinterpret_cast<WinBioCloseSession_t>(*slot),
                                                  std::memory_order_release);
                    }
                } else if (strcmp(fn, "WinBioEnumBiometricUnits") == 0) {
                    void* orig = nullptr;
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&WinBioEnumBiometricUnits_Shim), orig)) {
                        g_enumUnits_Orig.store(reinterpret_cast<WinBioEnumBiometricUnits_t>(orig),
                                               std::memory_order_release);
                        ++winbioPatched;
                    }
                } else if (strcmp(fn, "WinBioGetCredentialState") == 0) {
                    void* orig = nullptr;
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&WinBioGetCredentialState_Shim), orig)) {
                        g_getCredState_Orig.store(reinterpret_cast<WinBioGetCredentialState_t>(orig),
                                                  std::memory_order_release);
                        ++winbioPatched;
                    }
                } else if (strcmp(fn, "WinBioOpenSession") == 0) {
                    void* orig = nullptr;
                    if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&WinBioOpenSession_Shim), orig)) {
                        g_openSession_Orig.store(reinterpret_cast<WinBioOpenSession_t>(orig),
                                                 std::memory_order_release);
                        ++winbioPatched;
                    }
                }
            } else if (isShlwapi && strcmp(fn, "SHStrDupW") == 0) {
                void* orig = nullptr;
                if (WriteIatSlotLocked(slot, reinterpret_cast<void*>(&SHStrDupW_Shim), orig)) {
                    g_shStrDupW_Orig.store(reinterpret_cast<SHStrDupW_t>(orig), std::memory_order_release);
                    ++shlwapiPatched;
                }
            }
        }
    }

    // Fallback in case WinBioFree/WinBioCloseSession weren't yet populated.
    if (HMODULE hWb = GetModuleHandleW(L"winbio.dll")) {
        if (!g_free_Orig.load(std::memory_order_acquire)) {
            g_free_Orig.store(GetProcAs<WinBioFree_t>(hWb, "WinBioFree"),
                              std::memory_order_release);
        }
        if (!g_closeSession_Orig.load(std::memory_order_acquire)) {
            g_closeSession_Orig.store(GetProcAs<WinBioCloseSession_t>(hWb, "WinBioCloseSession"),
                                      std::memory_order_release);
        }
    }

    Wh_Log(L"[bio] Patched private biocpl IAT (winbio=%d, shlwapi=%d)", winbioPatched, shlwapiPatched);
}

static void RestorePatchedIats() {
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

}  // namespace bio_shim

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

static bool LoadVerifiedBioModules(const std::wstring& binDir, const Bytes& expectedDui,
                                   const std::wstring& cplPath, const Bytes& expectedCpl) {
    const std::wstring duiPath = binDir + L"\\b7dui.dll";
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
    g_hBioCpl.store(hCpl, std::memory_order_release);
    if (g_role == ProcessRole::Explorer) {
        bio_shim::PatchBioCplIat(hCpl);
    }
    return true;
}

static bool BuildAndWriteBinaries(const std::wstring& binDir, const Bytes& rawBio, const Bytes& rawDui,
                                  const Bytes& rawDuser, Lang lang, Bytes& outPatchedBio,
                                  std::wstring& outCplPath) {
    outCplPath = binDir + L"\\" + CplDllFileName(lang);
    std::string err;
    if (!gen::BuildPatchedBioCpl(rawBio, rawDuser, lang, outCplPath, outPatchedBio, err)) {
        Wh_Log(L"BuildPatchedBioCpl failed: %S", err.c_str());
        return false;
    }
    if (!WriteFileAtomic(binDir + L"\\b7dui.dll", rawDui)) {
        Wh_Log(L"Failed to write b7dui.dll");
        return false;
    }
    if (!WriteFileAtomic(outCplPath, outPatchedBio)) {
        Wh_Log(L"Failed to write %s", outCplPath.c_str());
        return false;
    }
    WriteFileAtomic(binDir + L"\\" + TasksXmlFileName(lang), gen::BuildTasksXmlUtf16(lang));
    return true;
}

// Returns false only when the Windows 7 binaries could not be obtained (e.g. no network),
// i.e. when trying again later can help; every other outcome returns true.
static bool RunExplorerSetupOnce() {
    const std::wstring dir = StorageDir();
    if (dir.empty()) return true;
    const std::wstring dlDir = dir + L"\\dl";
    const std::wstring binDir = dir + L"\\bin";

    HANDLE mtx = CreateMutexW(nullptr, FALSE, L"Windhawk.Win7BiometricDevicesRestorer.Setup");
    if (mtx) {
        HANDLE hs[] = {mtx, g_stopEvent};
        const DWORD wait = WaitForMultipleObjects(g_stopEvent ? 2u : 1u, hs, FALSE, 120000u);
        if (wait == WAIT_OBJECT_0 + 1) { CloseHandle(mtx); return true; }
    }
    auto releaseMtx = [&]() { if (mtx) { ReleaseMutex(mtx); CloseHandle(mtx); } };

    EnsureDir(dlDir);
    EnsureDir(binDir);

    Bytes rawBio, rawDui, rawDuser;
    if (g_shuttingDown.load() || !GetVerifiedFile(dlDir, kFiles[0], rawBio) ||
        g_shuttingDown.load() || !GetVerifiedFile(dlDir, kFiles[1], rawDui) ||
        g_shuttingDown.load() || !GetVerifiedFile(dlDir, kFiles[2], rawDuser)) {
        Wh_Log(L"Setup aborted: unable to obtain verified Windows 7 binaries from msdl.microsoft.com");
        releaseMtx();
        return g_shuttingDown.load();
    }

    const Lang lang = CurrentLang();
    Bytes patchedBio;
    std::wstring cplPath;
    if (!BuildAndWriteBinaries(binDir, rawBio, rawDui, rawDuser, lang, patchedBio, cplPath)) {
        releaseMtx();
        return true;
    }
    releaseMtx();

    if (g_shuttingDown.load()) return true;
    if (!LoadVerifiedBioModules(binDir, rawDui, cplPath, patchedBio)) {
        Wh_Log(L"Failed to load verified private biometric modules");
        return true;
    }

    PublishRegistration(binDir, false);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    Wh_Log(L"Windows 7 Biometric Devices applet active (%s)", GetLangTable(lang).code);
    return true;
}

// Retries a failed download on a long, growing interval (2 min, doubling, capped at 30 min),
// so the applet appears by itself once the network is back. Stops as soon as the mod unloads.
static void RunExplorerSetup() {
    DWORD delayMs = 2u * 60u * 1000u;
    const DWORD maxDelayMs = 30u * 60u * 1000u;
    while (!g_shuttingDown.load()) {
        if (RunExplorerSetupOnce()) return;
        if (!g_stopEvent || WaitForSingleObject(g_stopEvent, delayMs) == WAIT_OBJECT_0) return;
        delayMs = std::min<DWORD>(delayMs * 2u, maxDelayMs);
    }
}

static bool LoadDllHostPayload(const std::wstring& binDir, const std::wstring& dlDir) {
    Bytes rawBio, rawDui, rawDuser;
    if (!ReadPinnedFile(dlDir + L"\\" + kFiles[0].name, kFiles[0], rawBio) ||
        !ReadPinnedFile(dlDir + L"\\" + kFiles[1].name, kFiles[1], rawDui) ||
        !ReadPinnedFile(dlDir + L"\\" + kFiles[2].name, kFiles[2], rawDuser)) {
        Wh_Log(L"[dllhost] pinned downloads not ready");
        return false;
    }
    const Lang lang = CurrentLang();
    const std::wstring cplPath = binDir + L"\\" + CplDllFileName(lang);
    Bytes expectedBio;
    std::string err;
    if (!gen::BuildPatchedBioCpl(rawBio, rawDuser, lang, cplPath, expectedBio, err)) {
        Wh_Log(L"[dllhost] deterministic rebuild failed: %S", err.c_str());
        return false;
    }
    if (!LoadVerifiedBioModules(binDir, rawDui, cplPath, expectedBio)) {
        Wh_Log(L"[dllhost] verified module load failed");
        return false;
    }
    Wh_Log(L"[dllhost] verified elevated WinBioPolicy host ready");
    return true;
}

#endif  // BIO_GENERATOR_ONLY

}  // namespace

// ============================================================================
// 8. Windhawk Entry Points (external linkage)
// ============================================================================

#ifndef BIO_GENERATOR_ONLY

static BOOL ModInitImpl() {
#if !defined(_M_AMD64) && !defined(__x86_64__)
    return FALSE;  // The Windows 7 binaries are x64-only.
#endif
    LoadSettings();

    wchar_t exe[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    const wchar_t* exeName = wcsrchr(exe, L'\\');
    exeName = exeName ? exeName + 1 : exe;
    const std::wstring cmdLine = LowerStr(GetCommandLineW() ? GetCommandLineW() : L"");

    if (_wcsicmp(exeName, L"explorer.exe") == 0) {
        g_role = ProcessRole::Explorer;
    } else if (_wcsicmp(exeName, L"control.exe") == 0) {
        g_role = ProcessRole::Control;
    } else if (_wcsicmp(exeName, L"dllhost.exe") == 0 &&
               cmdLine.find(L"0142e4d1-fb7a-11dc-ba4a-000ffe7ab428") != std::wstring::npos) {
        g_role = ProcessRole::DllHost;
    } else if (_wcsicmp(exeName, L"svchost.exe") == 0 &&
               (cmdLine.find(L"rpcss") != std::wstring::npos ||
                cmdLine.find(L"dcomlaunch") != std::wstring::npos ||
                cmdLine.find(L"appinfo") != std::wstring::npos)) {
        g_role = ProcessRole::ComService;
    } else {
        return FALSE;
    }

    const std::wstring dir = StorageDir();
    if (dir.empty()) return FALSE;
    const std::wstring dlDir = dir + L"\\dl";
    const std::wstring binDir = dir + L"\\bin";

    if (g_role == ProcessRole::ComService) {
        vreg::CleanupStaleHives();
        if (!vreg::InstallHooks(false)) return FALSE;
        PublishRegistration(binDir, false);
        Wh_Log(L"[com-service] read-only Biometric Devices COM/elevation view published");
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
        if (!LoadDllHostPayload(binDir, dlDir)) return FALSE;
        vreg::CleanupStaleHives();
        if (!vreg::InstallHooks(true)) return FALSE;
        PublishRegistration(binDir, false);
        return TRUE;
    }

    // Explorer: install hooks immediately, download/build/load asynchronously.
    vreg::CleanupStaleHives();
    g_shuttingDown.store(false);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!vreg::InstallHooks(true)) return FALSE;
    try {
        g_setupThread.emplace([] {
            try { RunExplorerSetup(); } catch (...) { Wh_Log(L"Explorer setup threw an exception"); }
        });
    } catch (...) {
        Wh_Log(L"Could not start setup thread");
        return FALSE;
    }
    return TRUE;
}

static BOOL ModSettingsChangedImpl(BOOL* bReload) {
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
        Bytes rawBio, rawDui, rawDuser;
        if (ReadPinnedFile(dlDir + L"\\" + kFiles[0].name, kFiles[0], rawBio) &&
            ReadPinnedFile(dlDir + L"\\" + kFiles[1].name, kFiles[1], rawDui) &&
            ReadPinnedFile(dlDir + L"\\" + kFiles[2].name, kFiles[2], rawDuser)) {
            Bytes patchedBio;
            std::wstring cplPath;
            if (BuildAndWriteBinaries(binDir, rawBio, rawDui, rawDuser, newLang, patchedBio, cplPath) && langChanged) {
                LoadVerifiedBioModules(binDir, rawDui, cplPath, patchedBio);
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
}

static void ModBeforeUninitImpl() {
    if (g_role != ProcessRole::Explorer) return;
    g_shuttingDown.store(true);
    bio_shim::RestorePatchedIats();
    if (g_stopEvent) SetEvent(g_stopEvent);
    CancelInFlightDownload();
    if (g_setupThread && g_setupThread->joinable()) g_setupThread->join();
    g_setupThread.reset();
    bio_shim::RestorePatchedIats();
    // Wait for any WinBio probe thread still running mod code before the image is unloaded.
    bio_shim::JoinWorkers();
}

static void ModUninitImpl() {
    bool keepFiles = true;
    if (g_role == ProcessRole::Explorer) {
        bio_shim::RestorePatchedIats();
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
                DeleteFileW((binDir + L"\\b7dui.dll").c_str());
                for (const LangTable& lt : kLangTables) {
                    DeleteFileW((binDir + L"\\" + CplDllFileName(lt.lang)).c_str());
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
}

BOOL Wh_ModInit() {
    try {
        return ModInitImpl();
    } catch (...) {
        Wh_Log(L"Wh_ModInit threw an exception");
        return FALSE;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    try {
        return ModSettingsChangedImpl(bReload);
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged threw an exception");
        return TRUE;
    }
}

void Wh_ModBeforeUninit() {
    try {
        ModBeforeUninitImpl();
    } catch (...) {
        Wh_Log(L"Wh_ModBeforeUninit threw an exception");
        // Still make sure nothing keeps running mod code past unload.
        try { bio_shim::JoinWorkers(); } catch (...) {}
    }
}

void Wh_ModUninit() {
    try {
        ModUninitImpl();
    } catch (...) {
        Wh_Log(L"Wh_ModUninit threw an exception");
        g_role = ProcessRole::Other;
    }
}

#else
// Standalone host self-test harness (compiled only when -DBIO_GENERATOR_ONLY is defined)
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: bio_gen_test <biocpl.dll> <duser.dll> <out_dir>\n";
        return 2;
    }
    auto readAll = [](const char* p) -> Bytes {
        std::ifstream f(p, std::ios::binary);
        return Bytes((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    };
    const Bytes rawBio = readAll(argv[1]);
    const Bytes rawDuser = readAll(argv[2]);
    std::cout << "Input SHA-256 biocpl=" << sha256::Hex(rawBio)
              << " duser=" << sha256::Hex(rawDuser) << "\n";
    for (const LangTable& lt : kLangTables) {
        const std::string code(lt.code, lt.code + wcslen(lt.code));
        const std::wstring fakePath =
            std::wstring(L"C:\\ProgramData\\Windhawk\\ModsWritable\\win7-biometric-devices-restorer\\bin\\biocpl-") +
            lt.code + L".dll";
        Bytes outDll;
        std::string err;
        if (!gen::BuildPatchedBioCpl(rawBio, rawDuser, lt.lang, fakePath, outDll, err)) {
            std::cerr << "FAIL [" << code << "]: " << err << "\n";
            return 1;
        }
        const std::string outPath = std::string(argv[3]) + "/biocpl-" + code + ".dll";
        std::ofstream of(outPath, std::ios::binary);
        of.write(reinterpret_cast<const char*>(outDll.data()), static_cast<std::streamsize>(outDll.size()));
        const Bytes tasks = gen::BuildTasksXmlUtf16(lt.lang);
        const std::string tPath = std::string(argv[3]) + "/BiometricDevices-" + code + ".tasks.xml";
        std::ofstream tf(tPath, std::ios::binary);
        tf.write(reinterpret_cast<const char*>(tasks.data()), static_cast<std::streamsize>(tasks.size()));
        std::cout << "OK [" << code << "] dll=" << outDll.size() << " B sha256=" << sha256::Hex(outDll)
                  << " tasks=" << tasks.size() << " B\n";
    }
    return 0;
}
#endif
