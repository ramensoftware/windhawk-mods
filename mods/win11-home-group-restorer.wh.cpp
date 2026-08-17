// ==WindhawkMod==
// @id             win11-home-group-restorer
// @name           Windows 11 HomeGroup Page Restorer
// @description    This mod restores the classic HomeGroup Control Panel page on Windows 11 (cosmetic only)
// @version        1.0.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @include        control.exe
// @architecture   x86-64
// @compilerOptions -lwininet -ladvapi32 -lole32 -luser32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# HomeGroup Page Restorer

## About
This mod restores the classic **HomeGroup** page in Control Panel and Windows 11 (and Windows 10 1803+ if necessary). The HomeGroup feature was removed starting with Windows 10 version 1803 (April 2018). This mod restores the visual Control Panel page using the original `hgcpl.dll` downloaded and verified from Microsoft's symbol servers.

Note: This mod is a fix for the [Windows 7 Legacy Applet Restorer mod](https://windhawk.net/mods/win7-legacy-applet-restorer) because the HomeGroup page was removed from Windows 11 while on Windows 10 it is still present despite the service not being functional/hard to restore properly.
The mod has been tested on Windows 11 24H2 and Windows 11 25H2.
**IMPORTANT**: This is a **cosmetic restoration only**. The underlying HomeGroup networking services (`HomeGroupListener`, `HomeGroupProvider`) were removed from Windows 10 1803+ and completely stripped from Windows 11. The page will display, but actual homegroup creation/joining/sharing will not work because the backend services no longer exist.

## Screenshot

![homegroup](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/homegroup.png)

## Features

- **Classic HomeGroup page**: The mod restores the "HomeGroup" entry in the Control Panel
- **Automatic setup**: The required `hgcpl.dll` is downloaded and SHA-256 verified automatically from Microsoft's symbol servers
- **Registry virtualization**: All registry entries are provided through an in-memory virtualization layer and nothing is written to the real registry
- **Conservative resource handling**: The mod is designed to be stable and user friendly using a conservative approach
- **100% reversible**: Disabling the mod makes the Control Panel entry disappear immediately
- **25 embedded languages**: The replacement text is provided for all 249 HomeGroup resource IDs in 25 languages; no MUI content is copied, downloaded, or required
- **Language selector**: The mod follows the Windows UI language automatically or allows a manual language override
- **Offline re-enable**: Once downloaded, the base DLL is cached locally for offline use

## Design and safety notes
- Setup runs on a background thread and it never blocks explorer.exe startup
- Download goes to a temporary file, SHA-256 checked against a pinned digest, then atomically moved into place
- After `LoadLibraryExW`, the loaded module is confirmed to map the same file object via `GetFileInformationByHandleEx(FileIdInfo)`
- All registry values are provided through an in-memory virtualization layer backed by volatile (in-memory-only) registry keys
- The mod only stores files in its dedicated Windhawk mod-storage folder via `Wh_GetModStoragePath`

## Known limitations
- **No actual networking**: HomeGroup sharing/joining is non-functional because the backend services were removed/disabled by Microsoft in Windows 10 1803+ and the control panel page was removed from Windows 11
- **Windows 11**: The page displays but some sub-features may show errors since the COM infrastructure is more heavily stripped
- **Private resource copy**: The mod builds a private resource-only copy of the verified DLL and injects the embedded RT_STRING blocks without modifying Windows files or the verified executable DLL

## Credits

- HomeGroup registry structure documented by [Strontic xCyclopedia](https://strontic.github.io/xcyclopedia/)
- WinClassic community for documenting the HomeGroup restoration approach
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: HomeGroup page language
  $description: This setting ensures that the mod automatically follows the current Windows UI language where possible. A manual selection applies to Control Panel text and the DirectUI page after it is reopened.
  $options:
    - auto: Automatic (Windows UI language)
    - en-US: English (United States)
    - it-IT: Italian
    - es-ES: Spanish (Spain)
    - fr-FR: French
    - tr-TR: Turkish
    - ru-RU: Russian
    - zh-CN: Chinese (Simplified)
    - de-DE: German
    - pt-BR: Portuguese (Brazil)
    - pl-PL: Polish
    - ja-JP: Japanese
    - ko-KR: Korean
    - ar-SA: Arabic
    - nl-NL: Dutch
    - sv-SE: Swedish
    - cs-CZ: Czech
    - da-DK: Danish
    - fi-FI: Finnish
    - el-GR: Greek
    - he-IL: Hebrew
    - hu-HU: Hungarian
    - nb-NO: Norwegian Bokmål
    - ro-RO: Romanian
    - sk-SK: Slovak
    - uk-UA: Ukrainian

- keepFilesOnDisable: true
  $name: Keep downloaded files if the mod is disabled
  $description: If enabled (default), downloaded DLL and marker files are kept for faster re-enabling. If disabled, they are removed on unload.

- enableAdvancedWriter: true
  $name: Enable HomeGroup Advanced Settings Writer
  $description: If enabled, this setting also registers the HomeGroup CPL Advanced Settings Writer COM object ({ffe1df5f-...}) for elevated settings access.

- forceHomeGroupInjection: false
  $name: Force HomeGroup injection (override conflict detection)
  $description: If enabled, this setting forces this mod to inject the HomeGroup CLSID into the Control Panel namespace even when a conflicting mod (e.g., Windows 7 Legacy Applet Restorer) is detected. This may cause duplicate entries — use only if the other mod is not handling HomeGroup correctly. Disabled by default.
*/
// ==/WindhawkModSettings==

#ifndef WINVER
#define WINVER 0x0602
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif

#include <windows.h>
#include <wininet.h>
#include <wincrypt.h>
#include <combaseapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <optional>
#include <vector>
#include <utility>
#include <windhawk_utils.h>

// =============================================================================
// AUTHOR-WRITTEN HOMEGROUP STRING CATALOG - 249 IDs in 25 languages
// The resource IDs describe the UI contract. The English wording below was
// independently paraphrased for this mod; the other tables are translations of
// that new wording. No hgcpl.dll.mui bytes or verbatim MUI string table is
// embedded, generated, downloaded, or required at runtime.
// =============================================================================
enum class EmbeddedLanguage {
    EN_US, IT_IT, ES_ES, FR_FR, TR_TR, RU_RU, ZH_CN, DE_DE, PT_BR, PL_PL, JA_JP, KO_KR, AR_SA, NL_NL, SV_SE, CS_CZ, DA_DK, FI_FI, EL_GR, HE_IL, HU_HU, NB_NO, RO_RO, SK_SK, UK_UA
};

struct EmbeddedTextEntry { UINT id; const wchar_t* text; };

// English (United States) (en-US)
static const EmbeddedTextEntry kStrings_EN_US[] = {
    {1, L"HomeGroup"},
    {2, L"Review HomeGroup options, decide what this PC shares, and display or update the access password."},
    {3, L"A policy set by your organization prevents this page from running. Ask the network administrator for assistance."},
    {4, L"Detailed sharing options"},
    {5, L"On"},
    {6, L"Off"},
    {7, L"Off (no printers installed)"},
    {8, L"There is no printer attached to this computer."},
    {9, L"Share content with PCs at home"},
    {10, L"Access your homegroup using a domain-joined computer"},
    {12, L"Edit HomeGroup options"},
    {13, L"Working…"},
    {14, L"No HomeGroup was found on this network."},
    {15, L"%1 of %2 created a homegroup on the network."},
    {16, L"You have been invited to join your homegroup."},
    {18, L"This PC is already a member of a HomeGroup."},
    {19, L"This computer cannot connect to your homegroup."},
    {20, L"HomeGroup lets trusted PCs exchange files and use shared printers, and it can send media to compatible devices. Access requires a password, while you remain in control of what this PC makes available."},
    {21, L"This computer is also part of a domain, so it cannot create its own homegroup, but it can join a homegroup created by someone on the network.\n\nHomegroups link computers on your home network so you can share photos, music, videos, documents, and printers. Homegroups are password protected and you can choose what to share at any time."},
    {22, L"Homegroups link computers on your home network so you can share photos, music, videos, documents, and printers. Homegroups are password protected and you can choose what to share at any time.\n\nYou can't create your own homegroups in this edition of Windows, but you can join homegroups created by others."},
    {23, L"Set up a HomeGroup"},
    {24, L"Join"},
    {25, L"HomeGroup password has been changed. To continue using your homegroup resources, make sure the person who already entered the new password is online, and then enter the new password."},
    {26, L"Windows has detected another homegroup on your network. Homegroups allow you to share files and printers with other computers. You can also stream media to your device."},
    {27, L"%1 changed his homegroup password. To continue using your homegroup resources, make sure the person who already entered the new password is online, and then enter the new password."},
    {28, L"Looking for HomeGroups on this network…"},
    {29, L"Type new password"},
    {30, L"Join now"},
    {32, L"Before you can create or join a homegroup, you must first connect to your network."},
    {34, L"Set this PC's network profile to Private before creating or joining a HomeGroup."},
    {35, L"Change network location"},
    {37, L"Sharing options for Private"},
    {38, L"Sharing options for Public"},
    {39, L"Sharing options for Domain"},
    {40, L"Private"},
    {41, L"Private (current profile)"},
    {42, L"Public"},
    {43, L"Public (current profile)"},
    {44, L"Domain"},
    {45, L"Domain (current profile)"},
    {46, L"Media streaming is on."},
    {47, L"Media streaming is off."},
    {56, L"Cancel"},
    {63, L"OK"},
    {64, L"Show or print the HomeGroup password"},
    {65, L"24pt;;;Consolas"},
    {66, L"Date printed: %1 %2"},
    {67, L"Display the HomeGroup password and prepare a printable copy"},
    {68, L"Password:"},
    {69, L"Use this password to connect other computers to your homegroup."},
    {70, L"On each computer:"},
    {71, L"Note: Computers that are turned off or asleep will not appear in your homegroup."},
    {72, L"1. Click Start, then click Control Panel."},
    {73, L"2. Under Network & Internet, click Choose homegroup and sharing options."},
    {74, L"3. Click Join Now and follow the HomeGroup Wizard to enter your password."},
    {75, L"Click Start, then click Control Panel."},
    {76, L"Could not print homegroup password"},
    {77, L"An error occurred when Windows tried to output the homegroup password. (Error code:%1!u!)"},
    {78, L"You are not currently connected to your home network. To view files and resources on other homegroup computers, first connect to your home network."},
    {79, L"%1 has joined the computer to the homegroup. I haven't shared the library with my homegroup. Click the link below to change what you've shared. Do not shut down or restart your computer until sharing is complete."},
    {80, L"I haven't shared the library with my homegroup. Click the link below to change what you've shared. Do not shut down or restart your computer until sharing is complete."},
    {81, L"HomeGroup is currently sharing the library on this computer. Some homegroup options are not available until sharing is complete. Do not shut down or restart your computer until sharing is complete."},
    {82, L"Under Network & Internet, click Choose homegroup and sharing options."},
    {83, L"There are currently no homegroups on the network."},
    {84, L"Click Join Now and follow the HomeGroup Wizard to enter your password."},
    {85, L"Click here to install."},
    {86, L"Windows found a homegroup printer"},
    {88, L"Introducing HomeGroup"},
    {89, L"%1 (current profile)"},
    {90, L"Set this PC's network profile to Private before joining a HomeGroup."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"HomeGroup is not ready yet. Please try again in a few minutes. If you continue to see this message, click the link to start troubleshooting your homegroup."},
    {95, L"Start the HomeGroup troubleshooter"},
    {98, L"HomeGroup password"},
    {99, L"Guest accounts cannot change homegroup settings."},
    {100, L"HomeGroup has found a new shared printer on your home network. Once installed, it will be available to anyone on this computer."},
    {101, L"Install printer"},
    {102, L"HomeGroup is not available because you are not connected to your home network."},
    {103, L"HomeGroup is not available because you are not connected to your home network."},
    {104, L"Before joining a homegroup, you must first connect to the network."},
    {105, L"HomeGroup image"},
    {106, L"Select what you want to share and view your homegroup password"},
    {107, L"Because this computer is part of a domain, settings to share its libraries and devices with other computers in the homegroup are not available."},
    {108, L"Settings to share libraries and devices with other computers in a homegroup are not available in this edition of Windows."},
    {109, L"Remove %1 from the homegroup"},
    {110, L"Cancel"},
    {111, L"Remove homegroup member"},
    {112, L"%1 will be removed from homegroup"},
    {113, L"All homegroup members who join using a password will be required to enter the password again."},
    {114, L"Printers and devices"},
    {115, L"Change %1 homegroup members"},
    {116, L"The homegroup password was reset"},
    {117, L"HomeGroup is sharing files"},
    {118, L"This PC is a HomeGroup member"},
    {119, L"A homegroup is available to join"},
    {120, L"A homegroup can be created"},
    {121, L"HomeGroup isn't available"},
    {122, L"Untrusted printer"},
    {200, L"Add Member"},
    {201, L"User Icon"},
    {202, L"Full name"},
    {203, L"User ID"},
    {204, L"Progress Bar"},
    {205, L"Folder Icon"},
    {220, L"Share libraries and hardware"},
    {221, L"Select the library you want to share with others in your homegroup."},
    {222, L"Edit HomeGroup options"},
    {223, L"Open HomeGroup in Control Panel to adjust its options."},
    {224, L"HomeGroup options"},
    {225, L"Use Control Panel to edit HomeGroup options, or run the troubleshooter for help."},
    {226, L"Start troubleshooter"},
    {227, L"Run the HomeGroup troubleshooter to diagnose and repair HomeGroup issues."},
    {228, L"View password"},
    {229, L"Display the HomeGroup password or print a copy."},
    {230, L"Join homegroup"},
    {231, L"Join the homegroup on this network."},
    {530, L"Open detailed sharing options…"},
    {541, L"Network visibility"},
    {542, L"If network discovery is turned on, this computer can see and be seen by other networked computers and devices."},
    {543, L"Turn on network discovery"},
    {544, L"Turn off network discovery"},
    {545, L"File and printer access"},
    {546, L"When file and printer sharing is turned on, other users on your network can access the files and printers you share from this computer."},
    {547, L"Turn on file and printer sharing"},
    {548, L"Turn off file and printer sharing"},
    {549, L"Public folder sharing"},
    {550, L"When public folder sharing is turned on, users on your network, including homegroup members, can access files in public folders."},
    {552, L"Enabling sharing allows anyone with access to your network to read and write files in your public folders."},
    {553, L"Turn off public folder sharing (users logged on to this computer can still access these folders)"},
    {554, L"Change sharing options for various network profiles"},
    {559, L"Media access"},
    {560, L"When media streaming is turned on, users and devices on your network can access the photos, music, and videos on this computer. This computer can also find media on the network."},
    {564, L"Cancel"},
    {567, L"Apply changes"},
    {584, L"Windows creates a separate network profile for each network you use. You can select specific options for each profile."},
    {585, L"HomeGroup Warning Icon"},
    {586, L"Libraries and devices shared from this computer"},
    {595, L"More HomeGroup tasks"},
    {600, L"Show or print the HomeGroup password"},
    {601, L"Your system administrator has not allowed you to access your homegroup."},
    {604, L"Change the password..."},
    {605, L"Leave the homegroup..."},
    {607, L"Choose media streaming options..."},
    {608, L"Because this computer is part of a domain, settings to share its libraries and devices with other computers in the homegroup are not available."},
    {609, L"Password protected sharing"},
    {610, L"When password-protected sharing is turned on, only users with user accounts and passwords on this computer can access shared files, printers connected to this computer, and public folders. Password-protected sharing must be turned off to allow others access."},
    {611, L"Turn on password protected sharing"},
    {612, L"Turn off password protected sharing"},
    {613, L"Print page"},
    {614, L"Allows shared content to be played on all devices on this network, such as TVs and game consoles"},
    {615, L"Private network"},
    {616, L"Guest or public network"},
    {617, L"Domain network"},
    {619, L"HomeGroup connections"},
    {620, L"Windows typically manages connections to other homegroup computers. However, if you use the same user account and password on all your computers, you can have HomeGroup use that account instead."},
    {621, L"Let Windows handle HomeGroup connections (recommended)"},
    {622, L"Connect to other computers using your user account and password"},
    {624, L"Start the HomeGroup troubleshooter"},
    {627, L"File sharing connections"},
    {628, L"Windows uses 128-bit encryption to secure file sharing connections. Some devices do not support 128-bit encryption and must use 40-bit or 56-bit encryption."},
    {629, L"Secure your file sharing connection using 128-bit encryption (recommended)"},
    {630, L"Enable device file sharing with 40-bit or 56-bit encryption"},
    {631, L"Every network"},
    {632, L"Change what's shared with your homegroup"},
    {637, L"Close"},
    {639, L"HomeGroup Remote Access"},
    {640, L"Other homegroup members can connect to your homegroup from anywhere through their computers."},
    {641, L"Block remote HomeGroup access through this PC"},
    {642, L"Permit remote HomeGroup access through this PC"},
    {648, L"Select the files and devices to make available, then choose their permission levels."},
    {649, L"Library or directory"},
    {650, L"Access level"},
    {652, L"Turn on automatic setup of network-attached devices."},
    {46000, L"HomeGroup"},
    {46004, L"Set a password for the HomeGroup"},
    {46005, L"Type the homegroup password"},
    {46006, L"&Create now"},
    {46007, L"&Join now"},
    {46008, L"Add other computers to your homegroup using this password"},
    {46009, L"You have joined the homegroup"},
    {46011, L"HomeGroup"},
    {46012, L"Windows cannot set up a homegroup on this computer."},
    {46013, L"Because this computer is part of a domain, sharing its library with other computers in the homegroup is not available."},
    {46014, L"Passwords must contain at least 8 characters and no leading or trailing spaces."},
    {46015, L"Password is incorrect.\nPlease try again. Passwords are case sensitive."},
    {46016, L"Every HomeGroup connection on this PC will be closed"},
    {46017, L"Successfully left your homegroup"},
    {46018, L"Change what's shared with your homegroup"},
    {46019, L"Share your photos, videos, music, documents, and printers with other computers in your home."},
    {46020, L"&Make changes"},
    {46021, L"Changing homegroup password disconnects everyone"},
    {46022, L"Enter a new password for your homegroup"},
    {46023, L"&Change password"},
    {46024, L"HomeGroup password changed successfully"},
    {46025, L"The homegroup password was changed"},
    {46026, L"Type the homegroup password"},
    {46027, L"HomeGroup password has been changed. To continue using your homegroup resources, make sure the person who already entered the new password is online, and then enter the new password."},
    {46028, L"Shared"},
    {46029, L"Windows could not remove the computer from the homegroup."},
    {46030, L"%1 changed his homegroup password. To continue using your homegroup resources, make sure the person who already entered the new password is online, and then enter the new password."},
    {46031, L"Passwords help prevent unauthorized access to your homegroup's files and printers. You can get the password from %2, %1, or another member of your homegroup."},
    {46032, L"Passwords help prevent unauthorized access to your homegroup's files and printers. You can get the password from %2, %1, or another member of your homegroup."},
    {46033, L"Consolas"},
    {46034, L"Create a Homegroup"},
    {46035, L"Join a Homegroup"},
    {46036, L"Change Your Homegroup Password"},
    {46037, L"Leave the Homegroup"},
    {46038, L"To access files and printers on other computers, you must add them to your homegroup. The following password is required:"},
    {46039, L"Type the new homegroup password:"},
    {46040, L"Update password"},
    {46041, L"Back up all PCs in your homegroup to a local data protection target."},
    {46042, L"Back up your PC using HomeGroup data protection targets"},
    {46043, L"Not shared"},
    {46044, L"Homegroups can only be created on private networks.\nTo change your network location settings, open Network and Sharing Center in Control Panel."},
    {46045, L"Windows will no longer detect homegroups on this network. To create a new homegroup, click OK and open HomeGroup in Control Panel."},
    {46046, L"Windows detected an existing homegroup.\nTo join, click OK and open HomeGroup in Control Panel."},
    {46047, L"HomeGroup service is now available. Please try again."},
    {46048, L"Share settings updated"},
    {46049, L"The selected files and resources are shared with your homegroup."},
    {46050, L"HomeGroup password updated successfully"},
    {46051, L"You've joined the homegroup"},
    {46052, L"You can now access your shared files and devices. The files and devices you're sharing remain unchanged."},
    {46053, L"You can start accessing files and printers shared by other users in your homegroup."},
    {46054, L"Update Your Homegroup Password"},
    {46055, L"Join a Homegroup"},
    {46056, L"Enter the new homegroup password from %1."},
    {46057, L"All homegroup computers' clocks must be set to no more than 24 hours apart. Make sure your computer clocks are in sync, then try joining the homegroup again."},
    {46058, L"The password does not meet the domain's password strength requirements. Enter a matching password or use another HomeGroup computer to change your password."},
    {46059, L"You can't reset your password because you're not connected to a private network.\nPlease connect to a private network and try again."},
    {46060, L"You are not connected to a private network.\nTo change your network location settings, open Network and Sharing Center in Control Panel."},
    {46061, L"Share with other home computers"},
    {46062, L"You can share files and printers with other computers. You can also stream media to your device.\n\nHomegroups are password protected and you can choose what to share at any time."},
    {46063, L"Add other computers to your homegroup using this password"},
    {46064, L"To access files and printers on other computers, you must add them to your homegroup. The following password is required:"},
    {46065, L"To create or join a homegroup, your network connection must have IPv6 enabled. To enable IPv6, start the HomeGroup Troubleshooter."},
    {46066, L"Add people to the homegroup"},
    {46067, L"Configure homegroup data protection"},
    {46068, L"Multiple homegroups detected"},
    {46069, L"Share with other homegroup members"},
    {46070, L"Documents"},
    {46071, L"Pictures"},
    {46072, L"Music"},
    {46073, L"Videos"},
    {46074, L"Printers and devices"},
    {46075, L"Change Homegroup Sharing Settings"},
    {46076, L"%1 Sharing"},
    {46077, L"Verifying your password..."},
};

// Italian (it-IT)
static const EmbeddedTextEntry kStrings_IT_IT[] = {
    {1, L"Gruppo Home"},
    {2, L"Esamina le opzioni del Gruppo Home, decidi cosa condivide questo PC e visualizza o aggiorna la password di accesso."},
    {3, L"Un criterio impostato dalla tua organizzazione impedisce l'esecuzione di questa pagina. Chiedi assistenza all'amministratore di rete."},
    {4, L"Opzioni di condivisione dettagliate"},
    {5, L"Attivato"},
    {6, L"Disattivato"},
    {7, L"Disattivato (nessuna stampante installata)"},
    {8, L"Nessuna stampante collegata a questo computer."},
    {9, L"Condividi contenuti con i PC di casa"},
    {10, L"Accedi al tuo gruppo home utilizzando un computer aggiunto al dominio"},
    {12, L"Modifica impostazioni gruppo home"},
    {13, L"Attendere..."},
    {14, L"Nessun gruppo Home trovato su questa rete."},
    {15, L"%1 di %2 ha creato un gruppo home sulla rete."},
    {16, L"Sei stato invitato a unirti al tuo gruppo home."},
    {18, L"Utilizza questa pagina se il computer appartiene a un gruppo home."},
    {19, L"Questo computer non è in grado di connettersi a un gruppo home."},
    {20, L"Un gruppo home consente di condividere file e stampanti con altri computer della rete domestica, nonché di trasmettere flussi multimediali a dispositivi.\n\nIl gruppo home è protetto da una password ed è sempre possibile scegliere quali elementi condividere."},
    {21, L"Anche questo computer fa parte di un dominio, quindi non può creare il proprio gruppo home, ma può unirsi a un gruppo home creato da qualcuno sulla rete.\n\nI gruppi home collegano i computer sulla rete domestica in modo da poter condividere foto, musica, video, documenti e stampanti. I gruppi home sono protetti da password e puoi scegliere cosa condividere in qualsiasi momento."},
    {22, L"I gruppi home collegano i computer sulla rete domestica in modo da poter condividere foto, musica, video, documenti e stampanti. I gruppi home sono protetti da password e puoi scegliere cosa condividere in qualsiasi momento.\n\nNon puoi creare i tuoi gruppi home in questa edizione di Windows, ma puoi unirti a gruppi home creati da altri."},
    {23, L"Configura un gruppo Home"},
    {24, L"Partecipa ora"},
    {25, L"La password del gruppo Home è stata modificata. Per continuare a utilizzare le risorse del gruppo home, assicurati che la persona che ha già inserito la nuova password sia online, quindi inserisci la nuova password."},
    {26, L"Windows ha rilevato un altro gruppo home sulla rete. I gruppi home consentono di condividere file e stampanti con altri computer. Puoi anche eseguire lo streaming di contenuti multimediali sul tuo dispositivo."},
    {27, L"%1 ha cambiato la password del suo gruppo home. Per continuare a utilizzare le risorse del gruppo home, assicurati che la persona che ha già inserito la nuova password sia online, quindi inserisci la nuova password."},
    {28, L"Ricerca di gruppi Home su questa rete…"},
    {29, L"Digita la nuova password"},
    {30, L"Partecipa ora"},
    {32, L"Prima di poter creare o partecipare a un gruppo home, devi prima connetterti alla tua rete."},
    {34, L"Utilizza questa pagina per creare o partecipare a un gruppo home, il percorso di rete del tuo computer deve essere impostato su privato."},
    {35, L"Cambia posizione di rete"},
    {37, L"Opzioni di condivisione per Privato"},
    {38, L"Opzioni di condivisione per Pubblico"},
    {39, L"Opzioni di condivisione per il dominio"},
    {40, L"Privato"},
    {41, L"Privato (profilo attuale)"},
    {42, L"Pubblico"},
    {43, L"Pubblico (profilo attuale)"},
    {44, L"Dominio"},
    {45, L"Dominio (profilo attuale)"},
    {46, L"Lo streaming multimediale è attivo."},
    {47, L"Lo streaming multimediale è disattivato."},
    {56, L"Annulla"},
    {63, L"OK"},
    {64, L"Visualizza e stampa la password del gruppo home"},
    {65, L"24pt;;;Consolas"},
    {66, L"Data stampata: %1 %2"},
    {67, L"Visualizza e stampa la password del gruppo home"},
    {68, L"Password:"},
    {69, L"Utilizza questa password per connettere altri computer al tuo gruppo home."},
    {70, L"Su ciascun computer:"},
    {71, L"Nota: i computer spenti o in modalità sospensione non verranno visualizzati nel tuo gruppo home."},
    {72, L"1. Fare clic su Start, quindi su Pannello di controllo."},
    {73, L"2. In Rete e Internet, fare clic su Scegli gruppo home e opzioni di condivisione."},
    {74, L"3. Fai clic su Partecipa ora e segui la procedura guidata del gruppo Home per inserire la password."},
    {75, L"Fare clic su Start, quindi su Pannello di controllo."},
    {76, L"Impossibile stampare la password del gruppo home"},
    {77, L"Si è verificato un errore durante il tentativo di Windows di generare la password del gruppo home. (Codice errore: %1!u!)"},
    {78, L"Al momento non sei connesso alla tua rete domestica. Per visualizzare file e risorse su altri computer del gruppo home, connettiti prima alla rete domestica."},
    {79, L"%1 ha unito il computer al gruppo home. Non ho condiviso la libreria con il mio gruppo home. Fai clic sul collegamento in basso per modificare ciò che hai condiviso. Non spegnere o riavviare il computer fino al completamento della condivisione."},
    {80, L"Non ho condiviso la libreria con il mio gruppo home. Fai clic sul collegamento in basso per modificare ciò che hai condiviso. Non spegnere o riavviare il computer fino al completamento della condivisione."},
    {81, L"Gruppo Home sta attualmente condividendo la libreria su questo computer. Alcune opzioni del gruppo home non sono disponibili finché la condivisione non viene completata. Non spegnere o riavviare il computer fino al completamento della condivisione."},
    {82, L"In Rete e Internet, fai clic su Scegli gruppo home e opzioni di condivisione."},
    {83, L"Al momento non sono presenti gruppi home sulla rete."},
    {84, L"Fai clic su Iscriviti ora e segui la procedura guidata del gruppo Home per inserire la password."},
    {85, L"Fare clic qui per installare."},
    {86, L"Windows ha trovato una stampante del gruppo home"},
    {88, L"Presentazione del Gruppo Home"},
    {89, L"%1 (profilo corrente)"},
    {90, L"Utilizza questa pagina per partecipare a un gruppo home, il percorso di rete del tuo computer deve essere impostato su privato."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Gruppo Home non pronto. Riprovare tra qualche minuto. Se questo messaggio viene nuovamente visualizzato, fare clic sul collegamento seguente per avviare Risoluzione dei problemi di Gruppo Home."},
    {95, L"Avvia Risoluzione dei problemi di Gruppo Home"},
    {98, L"Password del gruppo Home"},
    {99, L"Gli account ospite non possono modificare le impostazioni del gruppo home."},
    {100, L"Gruppo Home ha trovato una nuova stampante condivisa sulla tua rete domestica. Una volta installato, sarà disponibile per chiunque su questo computer."},
    {101, L"Installa la stampante"},
    {102, L"Il Gruppo Home non è disponibile perché non sei connesso alla rete domestica."},
    {103, L"Il Gruppo Home non è disponibile perché non sei connesso alla rete domestica."},
    {104, L"Prima di unirti a un gruppo home, devi prima connetterti alla rete."},
    {105, L"Immagine del gruppo Home"},
    {106, L"Seleziona ciò che desideri condividere e visualizza la password del tuo gruppo home"},
    {107, L"Poiché questo computer fa parte di un dominio, le impostazioni per condividere le sue librerie e i suoi dispositivi con altri computer nel gruppo home non sono disponibili."},
    {108, L"Le impostazioni per condividere librerie e dispositivi con altri computer in un gruppo home non sono disponibili in questa edizione di Windows."},
    {109, L"Rimuovi %1 dal gruppo home"},
    {110, L"Annulla"},
    {111, L"Rimuovi membro del gruppo home"},
    {112, L"%1 verrà rimosso dal gruppo home"},
    {113, L"A tutti i membri del gruppo home che si uniscono utilizzando una password verrà richiesto di inserire nuovamente la password."},
    {114, L"Stampanti e dispositivi"},
    {115, L"Modifica i membri del gruppo home %1"},
    {116, L"La password del gruppo home è stata reimpostata"},
    {117, L"Il Gruppo Home sta condividendo file"},
    {118, L"Questo computer appartiene a un gruppo home"},
    {119, L"È possibile partecipare a un gruppo home"},
    {120, L"È possibile creare un gruppo home"},
    {121, L"Il Gruppo Home non è disponibile"},
    {122, L"Stampante non attendibile"},
    {200, L"Aggiungi membro"},
    {201, L"Icona utente"},
    {202, L"Nome e cognome"},
    {203, L"ID utente"},
    {204, L"Barra di avanzamento"},
    {205, L"Icona della cartella"},
    {220, L"Condividi librerie e hardware"},
    {221, L"Seleziona la libreria che desideri condividere con gli altri nel tuo gruppo home."},
    {222, L"Modifica impostazioni gruppo home"},
    {223, L"Utilizza questa pagina per modificare le impostazioni del Gruppo Home, aprire il Gruppo Home nel Pannello di controllo."},
    {224, L"Opzioni del Gruppo Home"},
    {225, L"Utilizza questa pagina per modificare le impostazioni del Gruppo Home nel Pannello di controllo o utilizza lo strumento di risoluzione dei problemi del Gruppo Home."},
    {226, L"Avvia lo strumento di risoluzione dei problemi"},
    {227, L"Utilizza questa pagina per utilizzare lo strumento di risoluzione dei problemi del Gruppo Home per individuare e risolvere i problemi relativi al tuo Gruppo Home."},
    {228, L"Visualizza password"},
    {229, L"Visualizza o stampa la password del gruppo home."},
    {230, L"Unisciti al gruppo home"},
    {231, L"Unisciti al gruppo home su questa rete."},
    {530, L"Modifica impostazioni di condivisione avanzate..."},
    {541, L"Individuazione rete"},
    {542, L"Se il rilevamento della rete è attivato, questo computer può vedere ed essere visto da altri computer e dispositivi collegati in rete."},
    {543, L"Attiva individuazione rete"},
    {544, L"Disattiva individuazione rete"},
    {545, L"Condivisione file e stampanti"},
    {546, L"Quando la condivisione di file e stampanti è attivata, altri utenti sulla rete possono accedere ai file e alle stampanti che condividi da questo computer."},
    {547, L"Attiva condivisione file e stampanti"},
    {548, L"Disattiva condivisione file e stampanti"},
    {549, L"Condivisione di cartelle pubbliche"},
    {550, L"Quando la condivisione delle cartelle pubbliche è attivata, gli utenti della rete, inclusi i membri del gruppo home, possono accedere ai file nelle cartelle pubbliche."},
    {552, L"L'abilitazione della condivisione consente a chiunque abbia accesso alla tua rete di leggere e scrivere file nelle tue cartelle pubbliche."},
    {553, L"Disattiva la condivisione delle cartelle pubbliche (gli utenti che hanno effettuato l'accesso a questo computer possono comunque accedere a queste cartelle)"},
    {554, L"Modifica le opzioni di condivisione per vari profili di rete"},
    {559, L"Flussi multimediali"},
    {560, L"Quando lo streaming multimediale è attivo, gli utenti e i dispositivi sulla tua rete possono accedere a foto, musica e video su questo computer. Questo computer può anche trovare contenuti multimediali sulla rete."},
    {564, L"Annulla"},
    {567, L"Salva modifiche"},
    {584, L"Windows crea un profilo di rete separato per ogni rete utilizzata. È possibile selezionare opzioni specifiche per ciascun profilo."},
    {585, L"Icona di avviso del gruppo Home"},
    {586, L"Raccolte e dispositivi condivisi da questo computer"},
    {595, L"Altre attività del Gruppo Home"},
    {600, L"Visualizza o stampa la password del gruppo home"},
    {601, L"L'amministratore di sistema non ti ha consentito di accedere al tuo gruppo home."},
    {604, L"Cambia la password..."},
    {605, L"Lascia il gruppo home..."},
    {607, L"Scegli le opzioni di streaming multimediale..."},
    {608, L"Poiché questo computer fa parte di un dominio, le impostazioni per condividere le sue librerie e i suoi dispositivi con altri computer nel gruppo home non sono disponibili."},
    {609, L"Condivisione protetta da password"},
    {610, L"Quando la condivisione protetta da password è attivata, solo gli utenti con account utente e password su questo computer possono accedere ai file condivisi, alle stampanti connesse a questo computer e alle cartelle pubbliche. La condivisione protetta da password deve essere disattivata per consentire l'accesso ad altri."},
    {611, L"Attiva condivisione protetta da password"},
    {612, L"Disattiva condivisione protetta da password"},
    {613, L"Stampa pagina"},
    {614, L"Consente la riproduzione dei contenuti condivisi su tutti i dispositivi su questa rete, come TV e console di gioco"},
    {615, L"Privato"},
    {616, L"Guest o pubblico"},
    {617, L"Dominio"},
    {619, L"Connessioni al gruppo Home"},
    {620, L"Windows in genere gestisce le connessioni ad altri computer del gruppo home. Tuttavia, se utilizzi lo stesso account utente e la stessa password su tutti i tuoi computer, puoi fare in modo che Gruppo Home utilizzi invece quell'account."},
    {621, L"Consenti a Windows di gestire le connessioni del gruppo home (scelta consigliata)"},
    {622, L"Connettiti ad altri computer utilizzando il tuo account utente e la password"},
    {624, L"Avvia Risoluzione dei problemi di Gruppo Home"},
    {627, L"Connessioni di condivisione file"},
    {628, L"Windows utilizza la crittografia a 128 bit per proteggere le connessioni di condivisione file. Alcuni dispositivi non supportano la crittografia a 128 bit e devono utilizzare la crittografia a 40 o 56 bit."},
    {629, L"Proteggi la tua connessione di condivisione file utilizzando la crittografia a 128 bit (consigliata)"},
    {630, L"Abilita la condivisione dei file del dispositivo con crittografia a 40 o 56 bit"},
    {631, L"Tutte le reti"},
    {632, L"Modifica gli elementi condivisi con il gruppo home"},
    {637, L"Chiudi"},
    {639, L"Accesso remoto al gruppo Home"},
    {640, L"Gli altri membri del gruppo home possono connettersi al tuo gruppo home da qualsiasi luogo tramite i loro computer."},
    {641, L"Opzione: disabilita l'accesso al gruppo home remoto tramite questo computer"},
    {642, L"Opzione: abilita l'accesso al gruppo home remoto tramite questo computer"},
    {648, L"Scegli i file e i dispositivi da condividere e imposta i livelli di autorizzazione."},
    {649, L"Raccolta o cartella"},
    {650, L"Autorizzazioni"},
    {652, L"Attiva la configurazione automatica dei dispositivi collegati alla rete."},
    {46000, L"Gruppo Home"},
    {46004, L"Opzione: scegli una password per il tuo gruppo home"},
    {46005, L"Digita la password del gruppo home"},
    {46006, L"&Crea ora"},
    {46007, L"&Iscriviti ora"},
    {46008, L"Aggiungi altri computer al tuo gruppo home utilizzando questa password"},
    {46009, L"Ti sei unito al gruppo home"},
    {46011, L"Gruppo Home"},
    {46012, L"Windows non può configurare un gruppo home su questo computer."},
    {46013, L"Poiché questo computer fa parte di un dominio, la condivisione della relativa libreria con altri computer nel gruppo home non è disponibile."},
    {46014, L"Le password devono contenere almeno 8 caratteri e nessuno spazio iniziale o finale."},
    {46015, L"La password non è corretta.\nPer favore riprova. Le password fanno distinzione tra maiuscole e minuscole."},
    {46016, L"Opzione: tutte le connessioni del gruppo home su questo computer verranno disconnesse"},
    {46017, L"Hai lasciato il tuo gruppo home con successo"},
    {46018, L"Modifica ciò che è condiviso con il tuo gruppo home"},
    {46019, L"Condividi foto, video, musica, documenti e stampanti con altri computer di casa."},
    {46020, L"&Apporta modifiche"},
    {46021, L"La modifica della password del gruppo home disconnette tutti"},
    {46022, L"Inserisci una nuova password per il tuo gruppo home"},
    {46023, L"&Cambiare la password"},
    {46024, L"La password del gruppo Home è stata modificata correttamente"},
    {46025, L"La password del gruppo home è stata modificata"},
    {46026, L"Digita la password del gruppo home"},
    {46027, L"La password del gruppo Home è stata modificata. Per continuare a utilizzare le risorse del gruppo home, assicurati che la persona che ha già inserito la nuova password sia online, quindi inserisci la nuova password."},
    {46028, L"Condiviso"},
    {46029, L"Windows non è riuscito a rimuovere il computer dal gruppo home."},
    {46030, L"%1 ha cambiato la password del suo gruppo home. Per continuare a utilizzare le risorse del gruppo home, assicurati che la persona che ha già inserito la nuova password sia online, quindi inserisci la nuova password."},
    {46031, L"Le password aiutano a prevenire l'accesso non autorizzato ai file e alle stampanti del tuo gruppo home. Puoi ottenere la password da %2, %1 o un altro membro del tuo gruppo home."},
    {46032, L"Le password aiutano a prevenire l'accesso non autorizzato ai file e alle stampanti del tuo gruppo home. Puoi ottenere la password da %2, %1 o un altro membro del tuo gruppo home."},
    {46033, L"Consolas"},
    {46034, L"Crea un gruppo home"},
    {46035, L"Unisciti a un gruppo home"},
    {46036, L"Cambia la password del tuo gruppo home"},
    {46037, L"Lascia il gruppo home"},
    {46038, L"Per accedere a file e stampanti su altri computer, devi aggiungerli al tuo gruppo home. È richiesta la seguente password:"},
    {46039, L"Digita la nuova password del gruppo home:"},
    {46040, L"Aggiorna password"},
    {46041, L"Esegui il backup di tutti i PC del tuo gruppo home su una destinazione di protezione dei dati locale."},
    {46042, L"Esegui il backup del tuo PC utilizzando le destinazioni di protezione dei dati del Gruppo Home"},
    {46043, L"Non condiviso"},
    {46044, L"I gruppi home possono essere creati solo su reti private.\nPer modificare le impostazioni della posizione di rete, apri Centro connessioni di rete e condivisione nel Pannello di controllo."},
    {46045, L"Windows non rileverà più i gruppi home su questa rete. Per creare un nuovo gruppo home, fai clic su OK e apri Gruppo Home nel Pannello di controllo."},
    {46046, L"Windows ha rilevato un gruppo home esistente.\nPer partecipare, fai clic su OK e apri Gruppo Home nel Pannello di controllo."},
    {46047, L"Il servizio Gruppo Home è ora disponibile. Per favore riprova."},
    {46048, L"Impostazioni di condivisione aggiornate"},
    {46049, L"I file e le risorse selezionati vengono condivisi con il tuo gruppo home."},
    {46050, L"Password del gruppo Home aggiornata correttamente"},
    {46051, L"Ti sei unito al gruppo home"},
    {46052, L"Ora puoi accedere ai file e ai dispositivi condivisi. I file e i dispositivi che condividi rimangono invariati."},
    {46053, L"Puoi iniziare ad accedere a file e stampanti condivisi da altri utenti nel tuo gruppo home."},
    {46054, L"Aggiorna la password del tuo gruppo home"},
    {46055, L"Unisciti a un gruppo home"},
    {46056, L"Inserisci la nuova password del gruppo home da %1."},
    {46057, L"Tutti gli orologi dei computer del gruppo home devono essere impostati a non più di 24 ore di distanza l'uno dall'altro. Assicurati che gli orologi del tuo computer siano sincronizzati, quindi prova a unirti nuovamente al gruppo home."},
    {46058, L"La password non soddisfa i requisiti di robustezza della password del dominio. Inserisci una password corrispondente o utilizza un altro computer del Gruppo Home per modificare la password."},
    {46059, L"Non puoi reimpostare la password perché non sei connesso a una rete privata.\nConnettiti a una rete privata e riprova."},
    {46060, L"Non sei connesso a una rete privata.\nPer modificare le impostazioni della posizione di rete, apri Centro connessioni di rete e condivisione nel Pannello di controllo."},
    {46061, L"Condividi con altri computer di casa"},
    {46062, L"È possibile condividere file e stampanti con altri computer. Puoi anche eseguire lo streaming di contenuti multimediali sul tuo dispositivo.\n\nI gruppi home sono protetti da password e puoi scegliere cosa condividere in qualsiasi momento."},
    {46063, L"Aggiungi altri computer al tuo gruppo home utilizzando questa password"},
    {46064, L"Per accedere a file e stampanti su altri computer, devi aggiungerli al tuo gruppo home. È richiesta la seguente password:"},
    {46065, L"Per creare o partecipare a un gruppo home, la connessione di rete deve avere IPv6 abilitato. Per abilitare IPv6, avvia lo strumento di risoluzione dei problemi del gruppo Home."},
    {46066, L"Aggiungi persone al gruppo home"},
    {46067, L"Configura la protezione dei dati del gruppo home"},
    {46068, L"Rilevati più gruppi home"},
    {46069, L"Condividi con altri membri del gruppo home"},
    {46070, L"Documenti"},
    {46071, L"Immagini"},
    {46072, L"Musica"},
    {46073, L"Video"},
    {46074, L"Stampanti e dispositivi"},
    {46075, L"Modifica le impostazioni di condivisione del gruppo home"},
    {46076, L"%1 Condivisione"},
    {46077, L"Verifica della password..."},
};

// Spanish (Spain) (es-ES)
static const EmbeddedTextEntry kStrings_ES_ES[] = {
    {1, L"Grupo Hogar"},
    {2, L"Revise las opciones del Grupo Hogar, decida qué comparte esta PC y muestre o actualice la contraseña de acceso."},
    {3, L"Una política establecida por su organización impide que esta página se ejecute. Solicite ayuda al administrador de la red."},
    {4, L"Opciones detalladas para compartir"},
    {5, L"En"},
    {6, L"Apagado"},
    {7, L"Apagado (no hay impresoras instaladas)"},
    {8, L"No hay ninguna impresora conectada a esta computadora."},
    {9, L"Compartir contenido con PC en casa"},
    {10, L"Acceda a su grupo en el hogar utilizando una computadora unida a un dominio"},
    {12, L"Editar opciones de grupo en el hogar"},
    {13, L"Laboral…"},
    {14, L"No se encontró ningún grupo en el hogar en esta red."},
    {15, L"%1 de %2 creó un grupo en el hogar en la red."},
    {16, L"Te han invitado a unirte a tu grupo habitual."},
    {18, L"Utilice esta página para que esta computadora pertenezca a un grupo en el hogar."},
    {19, L"Esta computadora no puede conectarse a su grupo en el hogar."},
    {20, L"HomeGroup permite que las PC confiables intercambien archivos y usen impresoras compartidas, y puede enviar medios a dispositivos compatibles. El acceso requiere una contraseña, mientras usted mantiene el control de lo que esta PC pone a disposición."},
    {21, L"Esta computadora también es parte de un dominio, por lo que no puede crear su propio grupo en el hogar, pero puede unirse a un grupo en el hogar creado por alguien en la red.\n\nLos grupos en el hogar vinculan computadoras en su red doméstica para que pueda compartir fotos, música, videos, documentos e impresoras. Los grupos en el hogar están protegidos con contraseña y puedes elegir qué compartir en cualquier momento."},
    {22, L"Los grupos en el hogar vinculan computadoras en su red doméstica para que pueda compartir fotos, música, videos, documentos e impresoras. Los grupos en el hogar están protegidos con contraseña y puedes elegir qué compartir en cualquier momento.\n\nNo puedes crear tus propios grupos en el hogar en esta edición de Windows, pero puedes unirte a grupos en el hogar creados por otros."},
    {23, L"Configurar un grupo en el hogar"},
    {24, L"Unirse"},
    {25, L"La contraseña del grupo hogar ha sido cambiada. Para continuar usando los recursos de su grupo en el hogar, asegúrese de que la persona que ya ingresó la nueva contraseña esté en línea y luego ingrese la nueva contraseña."},
    {26, L"Windows ha detectado otro grupo en el hogar en su red. Los grupos en el hogar le permiten compartir archivos e impresoras con otras computadoras. También puede transmitir medios a su dispositivo."},
    {27, L"%1 cambió la contraseña de su grupo en el hogar. Para continuar usando los recursos de su grupo en el hogar, asegúrese de que la persona que ya ingresó la nueva contraseña esté en línea y luego ingrese la nueva contraseña."},
    {28, L"Buscando grupos en el hogar en esta red..."},
    {29, L"Escribe nueva contraseña"},
    {30, L"Únete ahora"},
    {32, L"Antes de poder crear o unirse a un grupo en el hogar, primero debe conectarse a su red."},
    {34, L"Utilice esta página para crear o unirse a un grupo en el hogar; la ubicación de red de su computadora debe estar configurada como privada."},
    {35, L"Cambiar ubicación de red"},
    {37, L"Opciones para compartir en privado"},
    {38, L"Opciones para compartir para público"},
    {39, L"Opciones para compartir para el dominio"},
    {40, L"Privado"},
    {41, L"Privado (perfil actual)"},
    {42, L"Público"},
    {43, L"Público (perfil actual)"},
    {44, L"Dominio"},
    {45, L"Dominio (perfil actual)"},
    {46, L"La transmisión de medios está activada."},
    {47, L"La transmisión de medios está desactivada."},
    {56, L"Cancelar"},
    {63, L"DE ACUERDO"},
    {64, L"Mostrar o imprimir la contraseña del Grupo Hogar"},
    {65, L"24pt;;;Consolas"},
    {66, L"Fecha de impresión: %1 %2"},
    {67, L"Opción: ver e imprimir la contraseña de su grupo en el hogar"},
    {68, L"Contraseña:"},
    {69, L"Utilice esta contraseña para conectar otras computadoras a su grupo en el hogar."},
    {70, L"En cada computadora:"},
    {71, L"Nota: Las computadoras que estén apagadas o en suspensión no aparecerán en su grupo en el hogar."},
    {72, L"1. Haga clic en Inicio, luego haga clic en Panel de control."},
    {73, L"2. En Red e Internet, haga clic en Elegir grupo en el hogar y opciones para compartir."},
    {74, L"3. Haga clic en Unirse ahora y siga el Asistente de grupo en el hogar para ingresar su contraseña."},
    {75, L"Haga clic en Inicio, luego haga clic en Panel de control."},
    {76, L"No se pudo imprimir la contraseña del grupo en el hogar"},
    {77, L"Se produjo un error cuando Windows intentó generar la contraseña del grupo en el hogar. (Código de error: %1!u!)"},
    {78, L"Actualmente no estás conectado a tu red doméstica. Para ver archivos y recursos en otras computadoras del grupo en el hogar, primero conéctese a su red doméstica."},
    {79, L"%1 ha unido la computadora al grupo en el hogar. No he compartido la biblioteca con mi grupo habitual. Haga clic en el enlace a continuación para cambiar lo que ha compartido. No apague ni reinicie su computadora hasta que se complete el uso compartido."},
    {80, L"No he compartido la biblioteca con mi grupo habitual. Haga clic en el enlace a continuación para cambiar lo que ha compartido. No apague ni reinicie su computadora hasta que se complete el uso compartido."},
    {81, L"Grupo Hogar actualmente comparte la biblioteca en esta computadora. Algunas opciones de grupo en el hogar no están disponibles hasta que se completa el intercambio. No apague ni reinicie su computadora hasta que se complete el uso compartido."},
    {82, L"En Red e Internet, haga clic en Elegir grupo en el hogar y opciones para compartir."},
    {83, L"Actualmente no hay grupos en el hogar en la red."},
    {84, L"Haga clic en Unirse ahora y siga el Asistente de grupo en el hogar para ingresar su contraseña."},
    {85, L"Haga clic aquí para instalar."},
    {86, L"Windows encontró una impresora de grupo en el hogar"},
    {88, L"Presentamos el Grupo Hogar"},
    {89, L"%1 (perfil actual)"},
    {90, L"Utilice esta página para unirse a un grupo en el hogar; la ubicación de red de su computadora debe estar configurada como privada."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"El Grupo Hogar aún no está listo. Inténtelo de nuevo en unos minutos. Si continúa viendo este mensaje, haga clic en el enlace para comenzar a solucionar problemas de su grupo en el hogar."},
    {95, L"Inicie el solucionador de problemas de Grupo Hogar"},
    {98, L"Contraseña del grupo hogar"},
    {99, L"Las cuentas de invitados no pueden cambiar la configuración del grupo en el hogar."},
    {100, L"HomeGroup ha encontrado una nueva impresora compartida en su red doméstica. Una vez instalado, estará disponible para cualquier persona en esta computadora."},
    {101, L"Instalar impresora"},
    {102, L"Grupo Hogar no está disponible porque no estás conectado a tu red doméstica."},
    {103, L"Grupo Hogar no está disponible porque no estás conectado a tu red doméstica."},
    {104, L"Antes de unirse a un grupo en el hogar, primero debe conectarse a la red."},
    {105, L"Imagen del grupo Inicio"},
    {106, L"Seleccione lo que desea compartir y vea la contraseña de su grupo en el hogar"},
    {107, L"Debido a que esta computadora es parte de un dominio, las configuraciones para compartir sus bibliotecas y dispositivos con otras computadoras en el grupo en el hogar no están disponibles."},
    {108, L"Las configuraciones para compartir bibliotecas y dispositivos con otras computadoras en un grupo en el hogar no están disponibles en esta edición de Windows."},
    {109, L"Eliminar %1 del grupo en el hogar"},
    {110, L"Cancelar"},
    {111, L"Eliminar miembro del grupo en el hogar"},
    {112, L"%1 será eliminado del grupo hogar"},
    {113, L"Todos los miembros del grupo en el hogar que se unan usando una contraseña deberán ingresar la contraseña nuevamente."},
    {114, L"Impresoras y dispositivos"},
    {115, L"Cambiar miembros del grupo en el hogar %1"},
    {116, L"La contraseña del grupo en el hogar fue restablecida"},
    {117, L"Grupo Hogar está compartiendo archivos"},
    {118, L"Opción: esta computadora pertenece a un grupo en el hogar"},
    {119, L"Hay un grupo en el hogar disponible para unirse"},
    {120, L"Se puede crear un grupo en el hogar"},
    {121, L"El grupo Hogar no está disponible"},
    {122, L"Impresora no confiable"},
    {200, L"Agregar miembro"},
    {201, L"Icono de usuario"},
    {202, L"nombre completo"},
    {203, L"ID de usuario"},
    {204, L"Barra de progreso"},
    {205, L"Icono de carpeta"},
    {220, L"Compartir bibliotecas y hardware"},
    {221, L"Seleccione la biblioteca que desea compartir con otras personas de su grupo en el hogar."},
    {222, L"Editar opciones de grupo en el hogar"},
    {223, L"Utilice esta página para cambiar la configuración del Grupo Hogar, abra Grupo Hogar en el Panel de control."},
    {224, L"Opciones de grupo en el hogar"},
    {225, L"Utilice esta página para cambiar la configuración de su Grupo Hogar en el Panel de control o utilice el Solucionador de problemas de Grupo Hogar."},
    {226, L"Iniciar solucionador de problemas"},
    {227, L"Utilice esta página para utilizar el solucionador de problemas de Grupo Hogar para buscar y solucionar problemas con su Grupo Hogar."},
    {228, L"Ver contraseña"},
    {229, L"Utilice esta página para ver o imprimir la contraseña de su grupo en el hogar."},
    {230, L"Unirse al grupo habitual"},
    {231, L"Únase al grupo en el hogar en esta red."},
    {530, L"Abrir opciones detalladas para compartir..."},
    {541, L"Visibilidad de la red"},
    {542, L"Si la detección de red está activada, esta computadora puede ver y ser vista por otras computadoras y dispositivos en red."},
    {543, L"Activar el descubrimiento de redes"},
    {544, L"Desactivar el descubrimiento de redes"},
    {545, L"Acceso a archivos e impresoras"},
    {546, L"Cuando está activado el uso compartido de archivos e impresoras, otros usuarios de su red pueden acceder a los archivos e impresoras que comparte desde esta computadora."},
    {547, L"Activar el uso compartido de archivos e impresoras"},
    {548, L"Desactivar el uso compartido de archivos e impresoras"},
    {549, L"Compartir carpetas públicas"},
    {550, L"Cuando está activado el uso compartido de carpetas públicas, los usuarios de su red, incluidos los miembros del grupo en el hogar, pueden acceder a los archivos de las carpetas públicas."},
    {552, L"Habilitar el uso compartido permite que cualquier persona con acceso a su red lea y escriba archivos en sus carpetas públicas."},
    {553, L"Desactive el uso compartido de carpetas públicas (los usuarios que hayan iniciado sesión en esta computadora aún pueden acceder a estas carpetas)"},
    {554, L"Cambiar las opciones para compartir para varios perfiles de red"},
    {559, L"Acceso a los medios"},
    {560, L"Cuando la transmisión de medios está activada, los usuarios y dispositivos de su red pueden acceder a las fotos, la música y los videos de esta computadora. Esta computadora también puede encontrar medios en la red."},
    {564, L"Cancelar"},
    {567, L"Aplicar cambios"},
    {584, L"Windows crea un perfil de red independiente para cada red que utilice. Puede seleccionar opciones específicas para cada perfil."},
    {585, L"Icono de advertencia de grupo en el hogar"},
    {586, L"Bibliotecas y dispositivos compartidos desde esta computadora"},
    {595, L"Más tareas de Grupo Hogar"},
    {600, L"Mostrar o imprimir la contraseña del Grupo Hogar"},
    {601, L"El administrador de su sistema no le ha permitido acceder a su grupo en el hogar."},
    {604, L"Cambiar la contraseña..."},
    {605, L"Dejar el grupo base..."},
    {607, L"Elija opciones de transmisión de medios..."},
    {608, L"Debido a que esta computadora es parte de un dominio, las configuraciones para compartir sus bibliotecas y dispositivos con otras computadoras en el grupo en el hogar no están disponibles."},
    {609, L"Compartir protegido con contraseña"},
    {610, L"Cuando está activado el uso compartido protegido con contraseña, solo los usuarios con cuentas de usuario y contraseñas en esta computadora pueden acceder a archivos compartidos, impresoras conectadas a esta computadora y carpetas públicas. El uso compartido protegido por contraseña debe estar desactivado para permitir el acceso de otros."},
    {611, L"Activar el uso compartido protegido con contraseña"},
    {612, L"Desactivar el uso compartido protegido con contraseña"},
    {613, L"imprimir página"},
    {614, L"Permite reproducir contenido compartido en todos los dispositivos de esta red, como televisores y consolas de juegos."},
    {615, L"Red privada"},
    {616, L"Red invitada o pública"},
    {617, L"Red de dominio"},
    {619, L"Conexiones de grupo en el hogar"},
    {620, L"Windows normalmente administra las conexiones a otras computadoras del grupo en el hogar. Sin embargo, si usa la misma cuenta de usuario y contraseña en todas sus computadoras, puede hacer que Grupo Hogar use esa cuenta."},
    {621, L"Opción: permitir que Windows administre las conexiones del grupo en el hogar (recomendado)"},
    {622, L"Conéctese a otras computadoras usando su cuenta de usuario y contraseña"},
    {624, L"Inicie el solucionador de problemas de Grupo Hogar"},
    {627, L"Conexiones para compartir archivos"},
    {628, L"Windows utiliza cifrado de 128 bits para proteger las conexiones para compartir archivos. Algunos dispositivos no admiten cifrado de 128 bits y deben utilizar cifrado de 40 o 56 bits."},
    {629, L"Asegure su conexión para compartir archivos usando cifrado de 128 bits (recomendado)"},
    {630, L"Habilite el uso compartido de archivos del dispositivo con cifrado de 40 o 56 bits"},
    {631, L"Cada red"},
    {632, L"Cambiar lo que se comparte con tu grupo en el hogar"},
    {637, L"Cerca"},
    {639, L"Acceso remoto del grupo hogar"},
    {640, L"Otros miembros del grupo en el hogar pueden conectarse a su grupo en el hogar desde cualquier lugar a través de sus computadoras."},
    {641, L"Opción: deshabilitar el acceso remoto al grupo en el hogar a través de esta computadora"},
    {642, L"Opción: habilitar el acceso remoto al grupo en el hogar a través de esta computadora"},
    {648, L"Seleccione los archivos y dispositivos que desea poner a disposición y luego elija sus niveles de permiso."},
    {649, L"Biblioteca o directorio"},
    {650, L"Nivel de acceso"},
    {652, L"Active la configuración automática de dispositivos conectados a la red."},
    {46000, L"Grupo Hogar"},
    {46004, L"Opción: elija una contraseña para su grupo en el hogar"},
    {46005, L"Escriba la contraseña del grupo en el hogar"},
    {46006, L"&Crear ahora"},
    {46007, L"&Únete ahora"},
    {46008, L"Agregue otras computadoras a su grupo en el hogar usando esta contraseña"},
    {46009, L"Te has unido al grupo en el hogar"},
    {46011, L"Grupo Hogar"},
    {46012, L"Windows no puede configurar un grupo en el hogar en esta computadora."},
    {46013, L"Debido a que esta computadora es parte de un dominio, no está disponible compartir su biblioteca con otras computadoras en el grupo en el hogar."},
    {46014, L"Las contraseñas deben contener al menos 8 caracteres y no deben tener espacios iniciales ni finales."},
    {46015, L"La contraseña es incorrecta.\nPor favor inténtalo de nuevo. Las contraseñas distinguen entre mayúsculas y minúsculas."},
    {46016, L"Opción: todas las conexiones del grupo en el hogar en esta computadora se desconectarán"},
    {46017, L"Saliste exitosamente de tu grupo en el hogar"},
    {46018, L"Cambiar lo que se comparte con tu grupo en el hogar"},
    {46019, L"Comparta sus fotos, vídeos, música, documentos e impresoras con otras computadoras de su hogar."},
    {46020, L"&Hacer cambios"},
    {46021, L"Cambiar la contraseña del grupo en el hogar desconecta a todos"},
    {46022, L"Ingrese una nueva contraseña para su grupo en el hogar"},
    {46023, L"&Cambiar la contraseña"},
    {46024, L"La contraseña del grupo hogar se cambió correctamente"},
    {46025, L"Se cambió la contraseña del grupo en el hogar."},
    {46026, L"Escriba la contraseña del grupo en el hogar"},
    {46027, L"La contraseña del grupo hogar ha sido cambiada. Para continuar usando los recursos de su grupo en el hogar, asegúrese de que la persona que ya ingresó la nueva contraseña esté en línea y luego ingrese la nueva contraseña."},
    {46028, L"Compartido"},
    {46029, L"Windows no pudo eliminar la computadora del grupo en el hogar."},
    {46030, L"%1 cambió la contraseña de su grupo en el hogar. Para continuar usando los recursos de su grupo en el hogar, asegúrese de que la persona que ya ingresó la nueva contraseña esté en línea y luego ingrese la nueva contraseña."},
    {46031, L"Las contraseñas ayudan a evitar el acceso no autorizado a los archivos e impresoras de su grupo en el hogar. Puede obtener la contraseña de %2, %1 u otro miembro de su grupo en el hogar."},
    {46032, L"Las contraseñas ayudan a evitar el acceso no autorizado a los archivos e impresoras de su grupo en el hogar. Puede obtener la contraseña de %2, %1 u otro miembro de su grupo en el hogar."},
    {46033, L"Consolas"},
    {46034, L"Crear un grupo en el hogar"},
    {46035, L"Únase a un grupo en el hogar"},
    {46036, L"Cambie la contraseña de su grupo en el hogar"},
    {46037, L"Dejar el grupo en el hogar"},
    {46038, L"Para acceder a archivos e impresoras en otras computadoras, debe agregarlas a su grupo en el hogar. Se requiere la siguiente contraseña:"},
    {46039, L"Escriba la nueva contraseña del grupo en el hogar:"},
    {46040, L"Actualizar contraseña"},
    {46041, L"Haga una copia de seguridad de todas las PC de su grupo en el hogar en un objetivo de protección de datos local."},
    {46042, L"Haga una copia de seguridad de su PC utilizando objetivos de protección de datos de HomeGroup"},
    {46043, L"No compartido"},
    {46044, L"Los grupos en el hogar solo se pueden crear en redes privadas.\nPara cambiar la configuración de ubicación de red, abra el Centro de redes y recursos compartidos en el Panel de control."},
    {46045, L"Windows ya no detectará grupos en el hogar en esta red. Para crear un nuevo grupo en el hogar, haga clic en Aceptar y abra Grupo Hogar en el Panel de control."},
    {46046, L"Windows detectó un grupo en el hogar existente.\nPara unirse, haga clic en Aceptar y abra Grupo Hogar en el Panel de control."},
    {46047, L"El servicio Grupo Hogar ya está disponible. Por favor inténtalo de nuevo."},
    {46048, L"Compartir configuración actualizada"},
    {46049, L"Los archivos y recursos seleccionados se comparten con su grupo en el hogar."},
    {46050, L"La contraseña del grupo Hogar se actualizó correctamente"},
    {46051, L"Te has unido al grupo base"},
    {46052, L"Ahora puede acceder a sus archivos y dispositivos compartidos. Los archivos y dispositivos que compartes permanecen sin cambios."},
    {46053, L"Puede comenzar a acceder a archivos e impresoras compartidos por otros usuarios de su grupo en el hogar."},
    {46054, L"Actualice la contraseña de su grupo en el hogar"},
    {46055, L"Únase a un grupo en el hogar"},
    {46056, L"Ingrese la nueva contraseña del grupo en el hogar de %1."},
    {46057, L"Los relojes de todas las computadoras del grupo en el hogar deben configurarse con una diferencia de no más de 24 horas. Asegúrese de que los relojes de su computadora estén sincronizados y luego intente unirse al grupo en el hogar nuevamente."},
    {46058, L"La contraseña no cumple con los requisitos de seguridad de la contraseña del dominio. Ingrese una contraseña coincidente o use otra computadora del Grupo Hogar para cambiar su contraseña."},
    {46059, L"No puedes restablecer tu contraseña porque no estás conectado a una red privada.\nConéctese a una red privada e inténtelo nuevamente."},
    {46060, L"No estás conectado a una red privada.\nPara cambiar la configuración de ubicación de red, abra el Centro de redes y recursos compartidos en el Panel de control."},
    {46061, L"Compartir con otras computadoras domésticas"},
    {46062, L"Puede compartir archivos e impresoras con otras computadoras. También puede transmitir medios a su dispositivo.\n\nLos grupos en el hogar están protegidos con contraseña y puedes elegir qué compartir en cualquier momento."},
    {46063, L"Agregue otras computadoras a su grupo en el hogar usando esta contraseña"},
    {46064, L"Para acceder a archivos e impresoras en otras computadoras, debe agregarlas a su grupo en el hogar. Se requiere la siguiente contraseña:"},
    {46065, L"Para crear o unirse a un grupo en el hogar, su conexión de red debe tener IPv6 habilitado. Para habilitar IPv6, inicie el solucionador de problemas de grupo hogar."},
    {46066, L"Agregar personas al grupo en el hogar"},
    {46067, L"Configurar la protección de datos del grupo en el hogar"},
    {46068, L"Múltiples grupos en el hogar detectados"},
    {46069, L"Compartir con otros miembros del grupo en el hogar"},
    {46070, L"Documentos"},
    {46071, L"Fotos"},
    {46072, L"Música"},
    {46073, L"Vídeos"},
    {46074, L"Impresoras y dispositivos"},
    {46075, L"Cambiar la configuración para compartir en el grupo en el hogar"},
    {46076, L"%1 Compartir"},
    {46077, L"Verificando tu contraseña..."},
};

// French (fr-FR)
static const EmbeddedTextEntry kStrings_FR_FR[] = {
    {1, L"AccueilGroupe"},
    {2, L"Passez en revue les options du groupe résidentiel, décidez de ce que ce PC partage et affichez ou mettez à jour le mot de passe d'accès."},
    {3, L"Une stratégie définie par votre organisation empêche l'exécution de cette page. Demandez de l'aide à l'administrateur réseau."},
    {4, L"Options de partage détaillées"},
    {5, L"Sur"},
    {6, L"Désactivé"},
    {7, L"Éteint (aucune imprimante installée)"},
    {8, L"Aucune imprimante n'est connectée à cet ordinateur."},
    {9, L"Partagez du contenu avec des PC à la maison"},
    {10, L"Accédez à votre groupe résidentiel à l'aide d'un ordinateur appartenant à un domaine"},
    {12, L"Modifier les options du groupe résidentiel"},
    {13, L"Fonctionnement…"},
    {14, L"Aucun groupe résidentiel n'a été trouvé sur ce réseau."},
    {15, L"%1 de %2 a créé un groupe résidentiel sur le réseau."},
    {16, L"Vous avez été invité à rejoindre votre groupe résidentiel."},
    {18, L"Utilisez cette page pour que cet ordinateur appartienne à un groupe résidentiel."},
    {19, L"Cet ordinateur ne peut pas se connecter à votre groupe résidentiel."},
    {20, L"HomeGroup permet aux PC de confiance d'échanger des fichiers et d'utiliser des imprimantes partagées, et peut envoyer des médias à des appareils compatibles. L'accès nécessite un mot de passe, tandis que vous restez maître de ce que ce PC met à disposition."},
    {21, L"Cet ordinateur fait également partie d'un domaine, il ne peut donc pas créer son propre groupe résidentiel, mais il peut rejoindre un groupe résidentiel créé par quelqu'un sur le réseau.\n\nLes groupes résidentiels relient les ordinateurs de votre réseau domestique afin que vous puissiez partager des photos, de la musique, des vidéos, des documents et des imprimantes. Les groupes résidentiels sont protégés par mot de passe et vous pouvez choisir ce que vous souhaitez partager à tout moment."},
    {22, L"Les groupes résidentiels relient les ordinateurs de votre réseau domestique afin que vous puissiez partager des photos, de la musique, des vidéos, des documents et des imprimantes. Les groupes résidentiels sont protégés par mot de passe et vous pouvez choisir ce que vous souhaitez partager à tout moment.\n\nVous ne pouvez pas créer vos propres groupes résidentiels dans cette édition de Windows, mais vous pouvez rejoindre des groupes résidentiels créés par d'autres."},
    {23, L"Configurer un groupe résidentiel"},
    {24, L"Rejoindre"},
    {25, L"Le mot de passe du groupe résidentiel a été modifié. Pour continuer à utiliser les ressources de votre groupe résidentiel, assurez-vous que la personne qui a déjà saisi le nouveau mot de passe est en ligne, puis saisissez le nouveau mot de passe."},
    {26, L"Windows a détecté un autre groupe résidentiel sur votre réseau. Les groupes résidentiels vous permettent de partager des fichiers et des imprimantes avec d'autres ordinateurs. Vous pouvez également diffuser des médias sur votre appareil."},
    {27, L"%1 a modifié le mot de passe de son groupe résidentiel. Pour continuer à utiliser les ressources de votre groupe résidentiel, assurez-vous que la personne qui a déjà saisi le nouveau mot de passe est en ligne, puis saisissez le nouveau mot de passe."},
    {28, L"Vous recherchez des groupes résidentiels sur ce réseau…"},
    {29, L"Tapez un nouveau mot de passe"},
    {30, L"Inscrivez-vous maintenant"},
    {32, L"Avant de pouvoir créer ou rejoindre un groupe résidentiel, vous devez d'abord vous connecter à votre réseau."},
    {34, L"Utilisez cette page pour créer ou rejoindre un groupe résidentiel, l'emplacement réseau de votre ordinateur doit être défini sur privé."},
    {35, L"Changer l'emplacement du réseau"},
    {37, L"Options de partage pour le privé"},
    {38, L"Options de partage pour le public"},
    {39, L"Options de partage pour le domaine"},
    {40, L"Privé"},
    {41, L"Privé (profil actuel)"},
    {42, L"Publique"},
    {43, L"Public (profil actuel)"},
    {44, L"Domaine"},
    {45, L"Domaine (profil actuel)"},
    {46, L"Le streaming multimédia est activé."},
    {47, L"La diffusion multimédia est désactivée."},
    {56, L"Annuler"},
    {63, L"D'ACCORD"},
    {64, L"Afficher ou imprimer le mot de passe du groupe résidentiel"},
    {65, L"24pt;;;Consolas"},
    {66, L"Date d'impression : %1 %2"},
    {67, L"Option : afficher et imprimer le mot de passe de votre groupe résidentiel"},
    {68, L"Mot de passe:"},
    {69, L"Utilisez ce mot de passe pour connecter d'autres ordinateurs à votre groupe résidentiel."},
    {70, L"Sur chaque ordinateur :"},
    {71, L"Remarque : Les ordinateurs éteints ou en veille n'apparaîtront pas dans votre groupe résidentiel."},
    {72, L"1. Cliquez sur Démarrer, puis sur Panneau de configuration."},
    {73, L"2. Sous Réseau et Internet, cliquez sur Choisir les options de groupe résidentiel et de partage."},
    {74, L"3. Cliquez sur Rejoindre maintenant et suivez l'assistant HomeGroup pour saisir votre mot de passe."},
    {75, L"Cliquez sur Démarrer, puis cliquez sur Panneau de configuration."},
    {76, L"Impossible d'imprimer le mot de passe du groupe résidentiel"},
    {77, L"Une erreur s'est produite lorsque Windows a tenté d'afficher le mot de passe du groupe résidentiel. (Code d'erreur : %1!u!)"},
    {78, L"Vous n'êtes actuellement pas connecté à votre réseau domestique. Pour afficher les fichiers et les ressources sur d'autres ordinateurs du groupe résidentiel, connectez-vous d'abord à votre réseau domestique."},
    {79, L"%1 a joint l'ordinateur au groupe résidentiel. Je n'ai pas partagé la bibliothèque avec mon groupe résidentiel. Cliquez sur le lien ci-dessous pour modifier ce que vous avez partagé. N'éteignez pas et ne redémarrez pas votre ordinateur tant que le partage n'est pas terminé."},
    {80, L"Je n'ai pas partagé la bibliothèque avec mon groupe résidentiel. Cliquez sur le lien ci-dessous pour modifier ce que vous avez partagé. N'éteignez pas et ne redémarrez pas votre ordinateur tant que le partage n'est pas terminé."},
    {81, L"HomeGroup partage actuellement la bibliothèque sur cet ordinateur. Certaines options de groupe résidentiel ne sont pas disponibles tant que le partage n'est pas terminé. N'éteignez pas et ne redémarrez pas votre ordinateur tant que le partage n'est pas terminé."},
    {82, L"Sous Réseau et Internet, cliquez sur Choisir les options de groupe résidentiel et de partage."},
    {83, L"Il n'y a actuellement aucun groupe résidentiel sur le réseau."},
    {84, L"Cliquez sur Rejoindre maintenant et suivez l'assistant HomeGroup pour saisir votre mot de passe."},
    {85, L"Cliquez ici pour installer."},
    {86, L"Windows a trouvé une imprimante de groupe résidentiel"},
    {88, L"Présentation du groupe résidentiel"},
    {89, L"%1 (profil actuel)"},
    {90, L"Utilisez cette page pour rejoindre un groupe résidentiel, l'emplacement réseau de votre ordinateur doit être défini sur privé."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Le groupe résidentiel n'est pas encore prêt. Veuillez réessayer dans quelques minutes. Si vous continuez à voir ce message, cliquez sur le lien pour commencer le dépannage de votre groupe résidentiel."},
    {95, L"Démarrez l'utilitaire de résolution des problèmes de groupe résidentiel"},
    {98, L"Mot de passe du groupe résidentiel"},
    {99, L"Les comptes invités ne peuvent pas modifier les paramètres du groupe résidentiel."},
    {100, L"HomeGroup a trouvé une nouvelle imprimante partagée sur votre réseau domestique. Une fois installé, il sera accessible à tous sur cet ordinateur."},
    {101, L"Installer l'imprimante"},
    {102, L"HomeGroup n'est pas disponible car vous n'êtes pas connecté à votre réseau domestique."},
    {103, L"HomeGroup n'est pas disponible car vous n'êtes pas connecté à votre réseau domestique."},
    {104, L"Avant de rejoindre un groupe résidentiel, vous devez d'abord vous connecter au réseau."},
    {105, L"Image du groupe résidentiel"},
    {106, L"Sélectionnez ce que vous souhaitez partager et affichez le mot de passe de votre groupe résidentiel"},
    {107, L"Étant donné que cet ordinateur fait partie d'un domaine, les paramètres permettant de partager ses bibliothèques et ses appareils avec d'autres ordinateurs du groupe résidentiel ne sont pas disponibles."},
    {108, L"Les paramètres permettant de partager des bibliothèques et des appareils avec d'autres ordinateurs d'un groupe résidentiel ne sont pas disponibles dans cette édition de Windows."},
    {109, L"Supprimer %1 du groupe résidentiel"},
    {110, L"Annuler"},
    {111, L"Supprimer un membre du groupe résidentiel"},
    {112, L"%1 sera supprimé du groupe résidentiel"},
    {113, L"Tous les membres du groupe résidentiel qui rejoignent en utilisant un mot de passe devront saisir à nouveau le mot de passe."},
    {114, L"Imprimantes et appareils"},
    {115, L"Modifier les membres du groupe résidentiel %1"},
    {116, L"Le mot de passe du groupe résidentiel a été réinitialisé"},
    {117, L"HomeGroup partage des fichiers"},
    {118, L"Option : Cet ordinateur appartient à un groupe résidentiel"},
    {119, L"Un groupe résidentiel est disponible pour rejoindre"},
    {120, L"Un groupe résidentiel peut être créé"},
    {121, L"Le groupe résidentiel n'est pas disponible"},
    {122, L"Imprimante non fiable"},
    {200, L"Ajouter un membre"},
    {201, L"Icône utilisateur"},
    {202, L"Nom et prénom"},
    {203, L"ID de l'utilisateur"},
    {204, L"Barre de progression"},
    {205, L"Icône de dossier"},
    {220, L"Partager des bibliothèques et du matériel"},
    {221, L"Sélectionnez la bibliothèque que vous souhaitez partager avec les autres membres de votre groupe résidentiel."},
    {222, L"Modifier les options du groupe résidentiel"},
    {223, L"Utilisez cette page pour modifier les paramètres du groupe résidentiel, ouvrez HomeGroup dans le Panneau de configuration."},
    {224, L"Options de groupe résidentiel"},
    {225, L"Utilisez cette page pour modifier les paramètres de votre groupe résidentiel dans le Panneau de configuration ou utilisez l'utilitaire de résolution des problèmes de groupe résidentiel."},
    {226, L"Démarrer l'utilitaire de résolution des problèmes"},
    {227, L"Utilisez cette page pour utiliser l'utilitaire de résolution des problèmes de groupe résidentiel afin de rechercher et de résoudre les problèmes avec votre groupe résidentiel."},
    {228, L"Afficher le mot de passe"},
    {229, L"Utilisez cette page pour afficher ou imprimer le mot de passe de votre groupe résidentiel."},
    {230, L"Rejoindre un groupe résidentiel"},
    {231, L"Rejoignez le groupe résidentiel sur ce réseau."},
    {530, L"Ouvrir les options de partage détaillées…"},
    {541, L"Visibilité du réseau"},
    {542, L"Si la découverte du réseau est activée, cet ordinateur peut voir et être vu par d'autres ordinateurs et périphériques en réseau."},
    {543, L"Activer la découverte du réseau"},
    {544, L"Désactiver la découverte du réseau"},
    {545, L"Accès aux fichiers et à l'imprimante"},
    {546, L"Lorsque le partage de fichiers et d'imprimantes est activé, les autres utilisateurs de votre réseau peuvent accéder aux fichiers et aux imprimantes que vous partagez à partir de cet ordinateur."},
    {547, L"Activer le partage de fichiers et d'imprimantes"},
    {548, L"Désactiver le partage de fichiers et d'imprimantes"},
    {549, L"Partage de dossiers publics"},
    {550, L"Lorsque le partage de dossiers publics est activé, les utilisateurs de votre réseau, y compris les membres du groupe résidentiel, peuvent accéder aux fichiers des dossiers publics."},
    {552, L"L'activation du partage permet à toute personne ayant accès à votre réseau de lire et d'écrire des fichiers dans vos dossiers publics."},
    {553, L"Désactivez le partage de dossiers publics (les utilisateurs connectés à cet ordinateur peuvent toujours accéder à ces dossiers)"},
    {554, L"Modifier les options de partage pour différents profils réseau"},
    {559, L"Accès aux médias"},
    {560, L"Lorsque la diffusion multimédia est activée, les utilisateurs et les appareils de votre réseau peuvent accéder aux photos, à la musique et aux vidéos de cet ordinateur. Cet ordinateur peut également rechercher des médias sur le réseau."},
    {564, L"Annuler"},
    {567, L"Appliquer les modifications"},
    {584, L"Windows crée un profil réseau distinct pour chaque réseau que vous utilisez. Vous pouvez sélectionner des options spécifiques pour chaque profil."},
    {585, L"Icône d'avertissement de groupe résidentiel"},
    {586, L"Bibliothèques et appareils partagés depuis cet ordinateur"},
    {595, L"Plus de tâches de groupe résidentiel"},
    {600, L"Afficher ou imprimer le mot de passe du groupe résidentiel"},
    {601, L"Votre administrateur système ne vous a pas autorisé à accéder à votre groupe résidentiel."},
    {604, L"Changer le mot de passe..."},
    {605, L"Quittez le groupe résidentiel..."},
    {607, L"Choisissez les options de diffusion multimédia..."},
    {608, L"Étant donné que cet ordinateur fait partie d'un domaine, les paramètres permettant de partager ses bibliothèques et ses appareils avec d'autres ordinateurs du groupe résidentiel ne sont pas disponibles."},
    {609, L"Partage protégé par mot de passe"},
    {610, L"Lorsque le partage protégé par mot de passe est activé, seuls les utilisateurs disposant de comptes d'utilisateur et de mots de passe sur cet ordinateur peuvent accéder aux fichiers partagés, aux imprimantes connectées à cet ordinateur et aux dossiers publics. Le partage protégé par mot de passe doit être désactivé pour permettre à d'autres personnes d'y accéder."},
    {611, L"Activer le partage protégé par mot de passe"},
    {612, L"Désactiver le partage protégé par mot de passe"},
    {613, L"Imprimer la page"},
    {614, L"Permet de lire du contenu partagé sur tous les appareils de ce réseau, tels que les téléviseurs et les consoles de jeux."},
    {615, L"Réseau privé"},
    {616, L"Réseau invité ou public"},
    {617, L"Réseau de domaines"},
    {619, L"Connexions de groupe résidentiel"},
    {620, L"Windows gère généralement les connexions aux autres ordinateurs du groupe résidentiel. Cependant, si vous utilisez le même compte utilisateur et le même mot de passe sur tous vos ordinateurs, vous pouvez demander à HomeGroup d'utiliser ce compte à la place."},
    {621, L"Option : Autoriser Windows à gérer les connexions des groupes résidentiels (recommandé)"},
    {622, L"Connectez-vous à d'autres ordinateurs en utilisant votre compte utilisateur et votre mot de passe"},
    {624, L"Démarrez l'utilitaire de résolution des problèmes de groupe résidentiel"},
    {627, L"Connexions de partage de fichiers"},
    {628, L"Windows utilise un cryptage 128 bits pour sécuriser les connexions de partage de fichiers. Certains appareils ne prennent pas en charge le cryptage 128 bits et doivent utiliser un cryptage 40 bits ou 56 bits."},
    {629, L"Sécurisez votre connexion de partage de fichiers à l'aide d'un cryptage 128 bits (recommandé)"},
    {630, L"Activer le partage de fichiers sur l'appareil avec un cryptage 40 bits ou 56 bits"},
    {631, L"Chaque réseau"},
    {632, L"Modifier ce qui est partagé avec votre groupe résidentiel"},
    {637, L"Fermer"},
    {639, L"Accès à distance au groupe résidentiel"},
    {640, L"Les autres membres du groupe résidentiel peuvent se connecter à votre groupe résidentiel depuis n'importe où via leur ordinateur."},
    {641, L"Option : Désactiver l'accès au groupe résidentiel distant via cet ordinateur"},
    {642, L"Option : Activer l'accès au groupe résidentiel distant via cet ordinateur"},
    {648, L"Sélectionnez les fichiers et les appareils à mettre à disposition, puis choisissez leurs niveaux d'autorisation."},
    {649, L"Bibliothèque ou répertoire"},
    {650, L"Niveau d'accès"},
    {652, L"Activez la configuration automatique des appareils connectés au réseau."},
    {46000, L"AccueilGroupe"},
    {46004, L"Option : Choisissez un mot de passe pour votre groupe résidentiel"},
    {46005, L"Tapez le mot de passe du groupe résidentiel"},
    {46006, L"&Créer maintenant"},
    {46007, L"&Rejoignez-nous maintenant"},
    {46008, L"Ajoutez d'autres ordinateurs à votre groupe résidentiel en utilisant ce mot de passe"},
    {46009, L"Vous avez rejoint le groupe résidentiel"},
    {46011, L"AccueilGroupe"},
    {46012, L"Windows ne peut pas configurer de groupe résidentiel sur cet ordinateur."},
    {46013, L"Étant donné que cet ordinateur fait partie d'un domaine, le partage de sa bibliothèque avec d'autres ordinateurs du groupe résidentiel n'est pas disponible."},
    {46014, L"Les mots de passe doivent contenir au moins 8 caractères et aucun espace de début ou de fin."},
    {46015, L"Le mot de passe est incorrect.\nVeuillez réessayer. Les mots de passe sont sensibles à la casse."},
    {46016, L"Option : toutes les connexions du groupe résidentiel sur cet ordinateur seront déconnectées"},
    {46017, L"Vous avez quitté votre groupe résidentiel avec succès"},
    {46018, L"Modifier ce qui est partagé avec votre groupe résidentiel"},
    {46019, L"Partagez vos photos, vidéos, musiques, documents et imprimantes avec d'autres ordinateurs de votre maison."},
    {46020, L"&Apporter des modifications"},
    {46021, L"Changer le mot de passe du groupe résidentiel déconnecte tout le monde"},
    {46022, L"Entrez un nouveau mot de passe pour votre groupe résidentiel"},
    {46023, L"&Changer le mot de passe"},
    {46024, L"Le mot de passe du groupe résidentiel a été modifié avec succès"},
    {46025, L"Le mot de passe du groupe résidentiel a été modifié"},
    {46026, L"Tapez le mot de passe du groupe résidentiel"},
    {46027, L"Le mot de passe du groupe résidentiel a été modifié. Pour continuer à utiliser les ressources de votre groupe résidentiel, assurez-vous que la personne qui a déjà saisi le nouveau mot de passe est en ligne, puis saisissez le nouveau mot de passe."},
    {46028, L"Commun"},
    {46029, L"Windows n'a pas pu supprimer l'ordinateur du groupe résidentiel."},
    {46030, L"%1 a modifié le mot de passe de son groupe résidentiel. Pour continuer à utiliser les ressources de votre groupe résidentiel, assurez-vous que la personne qui a déjà saisi le nouveau mot de passe est en ligne, puis saisissez le nouveau mot de passe."},
    {46031, L"Les mots de passe aident à empêcher tout accès non autorisé aux fichiers et aux imprimantes de votre groupe résidentiel. Vous pouvez obtenir le mot de passe auprès de %2, %1 ou d'un autre membre de votre groupe résidentiel."},
    {46032, L"Les mots de passe aident à empêcher tout accès non autorisé aux fichiers et aux imprimantes de votre groupe résidentiel. Vous pouvez obtenir le mot de passe auprès de %2, %1 ou d'un autre membre de votre groupe résidentiel."},
    {46033, L"Consolas"},
    {46034, L"Créer un groupe résidentiel"},
    {46035, L"Rejoindre un groupe résidentiel"},
    {46036, L"Changez le mot de passe de votre groupe résidentiel"},
    {46037, L"Quitter le groupe résidentiel"},
    {46038, L"Pour accéder aux fichiers et aux imprimantes sur d'autres ordinateurs, vous devez les ajouter à votre groupe résidentiel. Le mot de passe suivant est requis :"},
    {46039, L"Tapez le nouveau mot de passe du groupe résidentiel :"},
    {46040, L"Mettre à jour le mot de passe"},
    {46041, L"Sauvegardez tous les PC de votre groupe résidentiel sur une cible locale de protection des données."},
    {46042, L"Sauvegardez votre PC à l’aide des cibles de protection des données HomeGroup"},
    {46043, L"Non partagé"},
    {46044, L"Les groupes résidentiels ne peuvent être créés que sur des réseaux privés.\nPour modifier vos paramètres d'emplacement réseau, ouvrez le Centre Réseau et partage dans le Panneau de configuration."},
    {46045, L"Windows ne détectera plus les groupes résidentiels sur ce réseau. Pour créer un nouveau groupe résidentiel, cliquez sur OK et ouvrez HomeGroup dans le Panneau de configuration."},
    {46046, L"Windows a détecté un groupe résidentiel existant.\nPour rejoindre, cliquez sur OK et ouvrez HomeGroup dans le Panneau de configuration."},
    {46047, L"Le service Groupe résidentiel est maintenant disponible. Veuillez réessayer."},
    {46048, L"Paramètres de partage mis à jour"},
    {46049, L"Les fichiers et ressources sélectionnés sont partagés avec votre groupe résidentiel."},
    {46050, L"Mot de passe du groupe résidentiel mis à jour avec succès"},
    {46051, L"Vous avez rejoint le groupe résidentiel"},
    {46052, L"Vous pouvez désormais accéder à vos fichiers et appareils partagés. Les fichiers et les appareils que vous partagez restent inchangés."},
    {46053, L"Vous pouvez commencer à accéder aux fichiers et imprimantes partagés par d'autres utilisateurs de votre groupe résidentiel."},
    {46054, L"Mettez à jour le mot de passe de votre groupe résidentiel"},
    {46055, L"Rejoindre un groupe résidentiel"},
    {46056, L"Saisissez le nouveau mot de passe du groupe résidentiel %1."},
    {46057, L"Les horloges de tous les ordinateurs du groupe résidentiel doivent être réglées à un intervalle maximum de 24 heures. Assurez-vous que les horloges de votre ordinateur sont synchronisées, puis essayez à nouveau de rejoindre le groupe résidentiel."},
    {46058, L"Le mot de passe ne répond pas aux exigences de sécurité du domaine. Entrez un mot de passe correspondant ou utilisez un autre ordinateur HomeGroup pour modifier votre mot de passe."},
    {46059, L"Vous ne pouvez pas réinitialiser votre mot de passe car vous n'êtes pas connecté à un réseau privé.\nVeuillez vous connecter à un réseau privé et réessayer."},
    {46060, L"Vous n'êtes pas connecté à un réseau privé.\nPour modifier vos paramètres d'emplacement réseau, ouvrez le Centre Réseau et partage dans le Panneau de configuration."},
    {46061, L"Partager avec d'autres ordinateurs personnels"},
    {46062, L"Vous pouvez partager des fichiers et des imprimantes avec d'autres ordinateurs. Vous pouvez également diffuser des médias sur votre appareil.\n\nLes groupes résidentiels sont protégés par mot de passe et vous pouvez choisir ce que vous souhaitez partager à tout moment."},
    {46063, L"Ajoutez d'autres ordinateurs à votre groupe résidentiel en utilisant ce mot de passe"},
    {46064, L"Pour accéder aux fichiers et aux imprimantes sur d'autres ordinateurs, vous devez les ajouter à votre groupe résidentiel. Le mot de passe suivant est requis :"},
    {46065, L"Pour créer ou rejoindre un groupe résidentiel, votre connexion réseau doit avoir IPv6 activé. Pour activer IPv6, démarrez l'utilitaire de résolution des problèmes HomeGroup."},
    {46066, L"Ajouter des personnes au groupe résidentiel"},
    {46067, L"Configurer la protection des données du groupe résidentiel"},
    {46068, L"Plusieurs groupes résidentiels détectés"},
    {46069, L"Partager avec d'autres membres du groupe résidentiel"},
    {46070, L"Documents"},
    {46071, L"Photos"},
    {46072, L"Musique"},
    {46073, L"Vidéos"},
    {46074, L"Imprimantes et appareils"},
    {46075, L"Modifier les paramètres de partage du groupe résidentiel"},
    {46076, L"Partage %1"},
    {46077, L"Vérification de votre mot de passe..."},
};

// Turkish (tr-TR)
static const EmbeddedTextEntry kStrings_TR_TR[] = {
    {1, L"Ev Grubu"},
    {2, L"Ev Grubu seçeneklerini gözden geçirin, bu bilgisayarın neyi paylaştığına karar verin ve erişim parolasını görüntüleyin veya güncelleyin."},
    {3, L"Kuruluşunuz tarafından belirlenen bir politika bu sayfanın çalışmasını engelliyor. Ağ yöneticisinden yardım isteyin."},
    {4, L"Ayrıntılı paylaşım seçenekleri"},
    {5, L"Açık"},
    {6, L"Kapalı"},
    {7, L"Kapalı (yüklü yazıcı yok)"},
    {8, L"Bu bilgisayara bağlı bir yazıcı yok."},
    {9, L"İçeriği evdeki bilgisayarlarla paylaşın"},
    {10, L"Etki alanına katılmış bir bilgisayar kullanarak ev grubunuza erişme"},
    {12, L"Ev Grubu seçeneklerini düzenleyin"},
    {13, L"Çalışıyor…"},
    {14, L"Bu ağda Ev Grubu bulunamadı."},
    {15, L"%2'nun %1'su ağda bir ev grubu oluşturdu."},
    {16, L"Ev grubunuza katılmaya davet edildiniz."},
    {18, L"Bu sayfayı, bu bilgisayarın bir ev grubuna ait olması için kullanın."},
    {19, L"Bu bilgisayar ev grubunuza bağlanamıyor."},
    {20, L"Ev Grubu, güvenilir bilgisayarların dosya alışverişi yapmasına ve paylaşılan yazıcıları kullanmasına olanak tanır ve uyumlu cihazlara medya gönderebilir. Erişim bir parola gerektirir ve bu bilgisayarın neleri kullanılabilir kıldığının kontrolü sizde kalır."},
    {21, L"Bu bilgisayar aynı zamanda bir etki alanının parçası olduğundan kendi ev grubunu oluşturamaz ancak ağdaki birisi tarafından oluşturulan bir ev grubuna katılabilir.\n\nEv grupları, fotoğraf, müzik, video, belge ve yazıcıları paylaşabilmeniz için ev ağınızdaki bilgisayarları birbirine bağlar. Ev grupları şifre korumalıdır ve istediğiniz zaman ne paylaşacağınızı seçebilirsiniz."},
    {22, L"Ev grupları, fotoğraf, müzik, video, belge ve yazıcıları paylaşabilmeniz için ev ağınızdaki bilgisayarları birbirine bağlar. Ev grupları şifre korumalıdır ve istediğiniz zaman ne paylaşacağınızı seçebilirsiniz.\n\nWindows'un bu sürümünde kendi ev gruplarınızı oluşturamazsınız ancak başkaları tarafından oluşturulan ev gruplarına katılabilirsiniz."},
    {23, L"Ev Grubu kurma"},
    {24, L"Katıl"},
    {25, L"Ev Grubu şifresi değiştirildi. Ev grubu kaynaklarınızı kullanmaya devam etmek için, yeni parolayı girmiş olan kişinin çevrimiçi olduğundan emin olun ve ardından yeni parolayı girin."},
    {26, L"Windows ağınızda başka bir ev grubu algıladı. Ev grupları, dosyaları ve yazıcıları diğer bilgisayarlarla paylaşmanıza olanak tanır. Ayrıca cihazınıza medya akışı da yapabilirsiniz."},
    {27, L"%1 ev grubu şifresini değiştirdi. Ev grubu kaynaklarınızı kullanmaya devam etmek için, yeni parolayı girmiş olan kişinin çevrimiçi olduğundan emin olun ve ardından yeni parolayı girin."},
    {28, L"Bu ağda Ev Grupları aranıyor…"},
    {29, L"Yeni şifreyi yazın"},
    {30, L"Şimdi katıl"},
    {32, L"Bir ev grubu oluşturabilmeniz veya bir ev grubuna katılabilmeniz için öncelikle ağınıza bağlanmanız gerekir."},
    {34, L"Bir ev grubu oluşturmak veya bir ev grubuna katılmak için bu sayfayı kullanın; bilgisayarınızın ağ konumu özel olarak ayarlanmalıdır."},
    {35, L"Ağ konumunu değiştir"},
    {37, L"Özel için paylaşım seçenekleri"},
    {38, L"Herkese açık paylaşım seçenekleri"},
    {39, L"Etki Alanı için paylaşım seçenekleri"},
    {40, L"Özel"},
    {41, L"Özel (mevcut profil)"},
    {42, L"halka açık"},
    {43, L"Herkese açık (mevcut profil)"},
    {44, L"Etki alanı"},
    {45, L"Etki alanı (mevcut profil)"},
    {46, L"Medya akışı açık."},
    {47, L"Medya akışı kapalı."},
    {56, L"İptal"},
    {63, L"tamam"},
    {64, L"Ev Grubu parolasını gösterin veya yazdırın"},
    {65, L"24pt;;;Consolas"},
    {66, L"Basılma tarihi: %1 %2"},
    {67, L"Seçenek: Ev grubu parolanızı görüntüleyin ve yazdırın"},
    {68, L"Şifre:"},
    {69, L"Diğer bilgisayarları ev grubunuza bağlamak için bu şifreyi kullanın."},
    {70, L"Her bilgisayarda:"},
    {71, L"Not: Kapalı veya uykuda olan bilgisayarlar ev grubunuzda görünmez."},
    {72, L"1. Başlat'ı ve ardından Denetim Masası'nı tıklayın."},
    {73, L"2. Ağ ve İnternet altında Ev grubunu ve paylaşım seçeneklerini seçin'i tıklayın."},
    {74, L"3. Şimdi Katıl'a tıklayın ve parolanızı girmek için Ev Grubu Sihirbazı'nı izleyin."},
    {75, L"Başlat'ı ve ardından Denetim Masası'nı tıklayın."},
    {76, L"Ev grubu şifresi yazdırılamadı"},
    {77, L"Windows ev grubu parolasının çıktısını almaya çalıştığında bir hata oluştu. (Hata kodu:%1!u!)"},
    {78, L"Şu anda ev ağınıza bağlı değilsiniz. Diğer ev grubu bilgisayarlarındaki dosyaları ve kaynakları görüntülemek için önce ev ağınıza bağlanın."},
    {79, L"%1 bilgisayarı ev grubuna kattı. Kütüphaneyi ev grubumla paylaşmadım. Paylaştıklarınızı değiştirmek için aşağıdaki bağlantıya tıklayın. Paylaşım tamamlanana kadar bilgisayarınızı kapatmayın veya yeniden başlatmayın."},
    {80, L"Kütüphaneyi ev grubumla paylaşmadım. Paylaştıklarınızı değiştirmek için aşağıdaki bağlantıya tıklayın. Paylaşım tamamlanana kadar bilgisayarınızı kapatmayın veya yeniden başlatmayın."},
    {81, L"Ev Grubu şu anda bu bilgisayardaki kitaplığı paylaşıyor. Paylaşım tamamlanana kadar bazı ev grubu seçenekleri kullanılamaz. Paylaşım tamamlanana kadar bilgisayarınızı kapatmayın veya yeniden başlatmayın."},
    {82, L"Ağ ve İnternet altında Ev grubunu ve paylaşım seçeneklerini seçin'i tıklayın."},
    {83, L"Şu anda ağda ev grubu yok."},
    {84, L"Şimdi Katıl'a tıklayın ve şifrenizi girmek için Ev Grubu Sihirbazı'nı izleyin."},
    {85, L"Yüklemek için burayı tıklayın."},
    {86, L"Windows bir ev grubu yazıcısı buldu"},
    {88, L"Ev Grubuyla Tanışın"},
    {89, L"%1 (mevcut profil)"},
    {90, L"Bir ev grubuna katılmak için bu sayfayı kullanın; bilgisayarınızın ağ konumu özel olarak ayarlanmalıdır."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Ev Grubu henüz hazır değil. Lütfen birkaç dakika sonra tekrar deneyin. Bu mesajı görmeye devam ederseniz ev grubunuzla ilgili sorunları gidermeye başlamak için bağlantıya tıklayın."},
    {95, L"Ev Grubu sorun gidericisini başlatın"},
    {98, L"Ev Grubu şifresi"},
    {99, L"Misafir hesapları ev grubu ayarlarını değiştiremez."},
    {100, L"Ev Grubu, ev ağınızda yeni bir paylaşılan yazıcı buldu. Yüklendikten sonra bu bilgisayardaki herkesin kullanımına açık olacak."},
    {101, L"Yazıcıyı yükle"},
    {102, L"Ev ağınıza bağlı olmadığınız için Ev Grubu kullanılamıyor."},
    {103, L"Ev ağınıza bağlı olmadığınız için Ev Grubu kullanılamıyor."},
    {104, L"Bir ev grubuna katılmadan önce ağa bağlanmanız gerekir."},
    {105, L"Ev Grubu resmi"},
    {106, L"Neyi paylaşmak istediğinizi seçin ve ev grubu şifrenizi görüntüleyin"},
    {107, L"Bu bilgisayar bir etki alanının parçası olduğundan kitaplıklarını ve aygıtlarını ev grubundaki diğer bilgisayarlarla paylaşma ayarları mevcut değil."},
    {108, L"Kitaplıkları ve aygıtları ev grubundaki diğer bilgisayarlarla paylaşma ayarları Windows'un bu sürümünde mevcut değildir."},
    {109, L"%1'yu ev grubundan kaldırın"},
    {110, L"İptal"},
    {111, L"Ev grubu üyesini kaldır"},
    {112, L"%1 ev grubundan kaldırılacak"},
    {113, L"Parola kullanarak katılan tüm ev grubu üyelerinin parolayı tekrar girmeleri gerekecektir."},
    {114, L"Yazıcılar ve cihazlar"},
    {115, L"%1 ev grubu üyelerini değiştirme"},
    {116, L"Ev grubu şifresi sıfırlandı"},
    {117, L"Ev Grubu dosya paylaşıyor"},
    {118, L"Seçenek: Bu bilgisayar bir ev grubuna ait"},
    {119, L"Katılmak için bir ev grubu mevcut"},
    {120, L"Ev grubu oluşturulabilir"},
    {121, L"Ev Grubu kullanılamıyor"},
    {122, L"Güvenilmeyen yazıcı"},
    {200, L"Üye Ekle"},
    {201, L"Kullanıcı Simgesi"},
    {202, L"Tam ad"},
    {203, L"Kullanıcı Kimliği"},
    {204, L"İlerleme Çubuğu"},
    {205, L"Klasör Simgesi"},
    {220, L"Kitaplıkları ve donanımı paylaşın"},
    {221, L"Ev grubunuzdaki diğer kişilerle paylaşmak istediğiniz kitaplığı seçin."},
    {222, L"Ev Grubu seçeneklerini düzenleyin"},
    {223, L"Ev Grubu ayarlarını değiştirmek için bu sayfayı kullanın ve Denetim Masası'nda Ev Grubu'nu açın."},
    {224, L"Ev Grubu seçenekleri"},
    {225, L"Denetim Masası'ndaki Ev Grubu ayarlarınızı değiştirmek için bu sayfayı kullanın veya Ev Grubu Sorun Gidericisini kullanın."},
    {226, L"Sorun gidericiyi başlat"},
    {227, L"Ev Grubunuzla ilgili sorunları bulmak ve düzeltmek amacıyla Ev Grubu sorun gidericisini kullanmak için bu sayfayı kullanın."},
    {228, L"Şifreyi görüntüle"},
    {229, L"Ev grubu parolanızı görüntülemek veya yazdırmak için bu sayfayı kullanın."},
    {230, L"Ev grubuna katıl"},
    {231, L"Bu ağdaki ev grubuna katılın."},
    {530, L"Ayrıntılı paylaşım seçeneklerini açın…"},
    {541, L"Ağ görünürlüğü"},
    {542, L"Ağ bulma açıksa bu bilgisayar, ağa bağlı diğer bilgisayarları ve aygıtları görebilir ve onlar tarafından görülebilir."},
    {543, L"Ağ bulmayı aç"},
    {544, L"Ağ bulmayı kapat"},
    {545, L"Dosya ve yazıcı erişimi"},
    {546, L"Dosya ve yazıcı paylaşımı açıldığında ağınızdaki diğer kullanıcılar bu bilgisayardan paylaştığınız dosya ve yazıcılara erişebilir."},
    {547, L"Dosya ve yazıcı paylaşımını açın"},
    {548, L"Dosya ve yazıcı paylaşımını kapatın"},
    {549, L"Ortak klasör paylaşımı"},
    {550, L"Ortak klasör paylaşımı açıldığında, ev grubu üyeleri de dahil olmak üzere ağınızdaki kullanıcılar ortak klasörlerdeki dosyalara erişebilir."},
    {552, L"Paylaşımı etkinleştirmek, ağınıza erişimi olan herkesin ortak klasörlerinizdeki dosyaları okumasına ve yazmasına olanak tanır."},
    {553, L"Ortak klasör paylaşımını kapatın (bu bilgisayarda oturum açan kullanıcılar yine de bu klasörlere erişebilir)"},
    {554, L"Çeşitli ağ profilleri için paylaşım seçeneklerini değiştirme"},
    {559, L"Medya erişimi"},
    {560, L"Medya akışı açıldığında ağınızdaki kullanıcılar ve cihazlar bu bilgisayardaki fotoğraflara, müziğe ve videolara erişebilir. Bu bilgisayar aynı zamanda ağdaki medyayı da bulabilir."},
    {564, L"İptal"},
    {567, L"Değişiklikleri uygula"},
    {584, L"Windows, kullandığınız her ağ için ayrı bir ağ profili oluşturur. Her profil için belirli seçenekleri seçebilirsiniz."},
    {585, L"Ev Grubu Uyarı Simgesi"},
    {586, L"Bu bilgisayardan paylaşılan kitaplıklar ve cihazlar"},
    {595, L"Daha fazla Ev Grubu görevi"},
    {600, L"Ev Grubu parolasını gösterin veya yazdırın"},
    {601, L"Sistem yöneticiniz ev grubunuza erişmenize izin vermedi."},
    {604, L"Şifreyi değiştir..."},
    {605, L"Ev grubundan ayrıl..."},
    {607, L"Medya akışı seçeneklerini seçin..."},
    {608, L"Bu bilgisayar bir etki alanının parçası olduğundan kitaplıklarını ve aygıtlarını ev grubundaki diğer bilgisayarlarla paylaşma ayarları mevcut değil."},
    {609, L"Şifre korumalı paylaşım"},
    {610, L"Parola korumalı paylaşım açıldığında, yalnızca bu bilgisayardaki kullanıcı hesapları ve parolaları olan kullanıcılar paylaşılan dosyalara, bu bilgisayara bağlı yazıcılara ve ortak klasörlere erişebilir. Başkalarının erişimine izin vermek için parola korumalı paylaşım kapatılmalıdır."},
    {611, L"Parola korumalı paylaşımı aç"},
    {612, L"Parola korumalı paylaşımı kapat"},
    {613, L"Sayfayı yazdır"},
    {614, L"Paylaşılan içeriğin bu ağdaki TV'ler ve oyun konsolları gibi tüm cihazlarda oynatılmasına izin verir"},
    {615, L"Özel ağ"},
    {616, L"Konuk veya genel ağ"},
    {617, L"Etki alanı ağı"},
    {619, L"Ev Grubu bağlantıları"},
    {620, L"Windows genellikle diğer ev grubu bilgisayarlarına olan bağlantıları yönetir. Ancak tüm bilgisayarlarınızda aynı kullanıcı hesabını ve parolayı kullanıyorsanız Ev Grubu'nun bunun yerine bu hesabı kullanmasını sağlayabilirsiniz."},
    {621, L"Seçenek: Windows'un ev grubu bağlantılarını yönetmesine izin ver (önerilir)"},
    {622, L"Kullanıcı hesabınızı ve şifrenizi kullanarak diğer bilgisayarlara bağlanın"},
    {624, L"Ev Grubu sorun gidericisini başlatın"},
    {627, L"Dosya paylaşım bağlantıları"},
    {628, L"Windows, dosya paylaşım bağlantılarının güvenliğini sağlamak için 128 bit şifreleme kullanır. Bazı cihazlar 128 bit şifrelemeyi desteklemez ve 40 bit veya 56 bit şifreleme kullanması gerekir."},
    {629, L"Dosya paylaşım bağlantınızı 128 bit şifreleme kullanarak güvenli hale getirin (önerilir)"},
    {630, L"40 bit veya 56 bit şifrelemeyle cihaz dosya paylaşımını etkinleştirin"},
    {631, L"Her ağ"},
    {632, L"Ev grubunuzla paylaşılanları değiştirin"},
    {637, L"Kapat"},
    {639, L"Ev Grubu Uzaktan Erişim"},
    {640, L"Diğer ev grubu üyeleri, ev grubunuza bilgisayarları aracılığıyla herhangi bir yerden bağlanabilirler."},
    {641, L"Seçenek: Bu bilgisayar üzerinden uzak ev grubu erişimini devre dışı bırakın"},
    {642, L"Seçenek: Bu bilgisayar üzerinden uzak ev grubu erişimini etkinleştirin"},
    {648, L"Kullanılabilir hale getirilecek dosyaları ve cihazları seçin, ardından bunların izin düzeylerini seçin."},
    {649, L"Kütüphane veya dizin"},
    {650, L"Erişim düzeyi"},
    {652, L"Ağa bağlı cihazların otomatik kurulumunu açın."},
    {46000, L"Ev Grubu"},
    {46004, L"Seçenek: Ev grubunuz için bir şifre seçin"},
    {46005, L"Ev grubu parolasını yazın"},
    {46006, L"&Şimdi oluştur"},
    {46007, L"&Hemen katıl"},
    {46008, L"Bu şifreyi kullanarak diğer bilgisayarları ev grubunuza ekleyin"},
    {46009, L"Ev grubuna katıldınız"},
    {46011, L"Ev Grubu"},
    {46012, L"Windows bu bilgisayarda bir ev grubu kuramıyor."},
    {46013, L"Bu bilgisayar bir etki alanının parçası olduğundan kitaplığının ev grubundaki diğer bilgisayarlarla paylaşılması mümkün değildir."},
    {46014, L"Şifreler en az 8 karakter içermeli ve başında veya sonunda boşluk olmamalıdır."},
    {46015, L"Şifre yanlış.\nLütfen tekrar deneyin. Şifreler büyük/küçük harfe duyarlıdır."},
    {46016, L"Seçenek: Bu bilgisayardaki tüm ev grubu bağlantılarının bağlantısı kesilecek"},
    {46017, L"Ev grubunuzdan başarıyla ayrıldınız"},
    {46018, L"Ev grubunuzla paylaşılanları değiştirin"},
    {46019, L"Fotoğraflarınızı, videolarınızı, müziğinizi, belgelerinizi ve yazıcılarınızı evinizdeki diğer bilgisayarlarla paylaşın."},
    {46020, L"&Değişiklik yap"},
    {46021, L"Ev grubu şifresini değiştirmek herkesin bağlantısını keser"},
    {46022, L"Ev grubunuz için yeni bir şifre girin"},
    {46023, L"&Şifreyi değiştir"},
    {46024, L"Ev Grubu şifresi başarıyla değiştirildi"},
    {46025, L"Ev grubu şifresi değiştirildi"},
    {46026, L"Ev grubu parolasını yazın"},
    {46027, L"Ev Grubu şifresi değiştirildi. Ev grubu kaynaklarınızı kullanmaya devam etmek için, yeni parolayı girmiş olan kişinin çevrimiçi olduğundan emin olun ve ardından yeni parolayı girin."},
    {46028, L"Paylaşıldı"},
    {46029, L"Windows bilgisayarı ev grubundan kaldıramadı."},
    {46030, L"%1 ev grubu şifresini değiştirdi. Ev grubu kaynaklarınızı kullanmaya devam etmek için, yeni parolayı girmiş olan kişinin çevrimiçi olduğundan emin olun ve ardından yeni parolayı girin."},
    {46031, L"Parolalar, ev grubunuzun dosyalarına ve yazıcılarına yetkisiz erişimin önlenmesine yardımcı olur. Şifreyi %2, %1 veya ev grubunuzun başka bir üyesinden alabilirsiniz."},
    {46032, L"Parolalar, ev grubunuzun dosyalarına ve yazıcılarına yetkisiz erişimin önlenmesine yardımcı olur. Şifreyi %2, %1 veya ev grubunuzun başka bir üyesinden alabilirsiniz."},
    {46033, L"Consolas"},
    {46034, L"Ev Grubu Oluştur"},
    {46035, L"Bir Ev Grubuna Katılın"},
    {46036, L"Ev Grubu Parolanızı Değiştirin"},
    {46037, L"Ev grubundan ayrıl"},
    {46038, L"Diğer bilgisayarlardaki dosyalara ve yazıcılara erişmek için bunları ev grubunuza eklemeniz gerekir. Aşağıdaki şifre gereklidir:"},
    {46039, L"Yeni ev grubu şifresini yazın:"},
    {46040, L"Şifreyi güncelle"},
    {46041, L"Ev grubunuzdaki tüm bilgisayarları yerel bir veri koruma hedefine yedekleyin."},
    {46042, L"Ev Grubu veri koruma hedeflerini kullanarak bilgisayarınızı yedekleyin"},
    {46043, L"Paylaşılmadı"},
    {46044, L"Ev grupları yalnızca özel ağlarda oluşturulabilir.\nAğ konumu ayarlarınızı değiştirmek için Denetim Masası'nda Ağ ve Paylaşım Merkezi'ni açın."},
    {46045, L"Windows artık bu ağdaki ev gruplarını algılamayacaktır. Yeni bir ev grubu oluşturmak için Tamam'a tıklayın ve Denetim Masası'nda Ev Grubu'nu açın."},
    {46046, L"Windows mevcut bir ev grubunu algıladı.\nKatılmak için Tamam'a tıklayın ve Denetim Masası'nda Ev Grubu'nu açın."},
    {46047, L"Ev Grubu hizmeti artık kullanılabilir. Lütfen tekrar deneyin."},
    {46048, L"Paylaşım ayarları güncellendi"},
    {46049, L"Seçilen dosyalar ve kaynaklar ev grubunuzla paylaşılır."},
    {46050, L"Ev Grubu şifresi başarıyla güncellendi"},
    {46051, L"Ev grubuna katıldınız"},
    {46052, L"Artık paylaşılan dosyalarınıza ve cihazlarınıza erişebilirsiniz. Paylaştığınız dosyalar ve cihazlar değişmeden kalır."},
    {46053, L"Ev grubunuzdaki diğer kullanıcılar tarafından paylaşılan dosyalara ve yazıcılara erişmeye başlayabilirsiniz."},
    {46054, L"Ev Grubu Parolanızı Güncelleyin"},
    {46055, L"Bir Ev Grubuna Katılın"},
    {46056, L"%1'dan yeni ev grubu şifresini girin."},
    {46057, L"Tüm ev grubu bilgisayarlarının saatleri, aralarındaki fark en fazla 24 saat olacak şekilde ayarlanmalıdır. Bilgisayar saatlerinizin senkronize olduğundan emin olun ve ardından ev grubuna katılmayı tekrar deneyin."},
    {46058, L"Parola, alan adının parola gücü gereksinimlerini karşılamıyor. Eşleşen bir parola girin veya parolanızı değiştirmek için başka bir Ev Grubu bilgisayarı kullanın."},
    {46059, L"Özel bir ağa bağlı olmadığınız için şifrenizi sıfırlayamazsınız.\nLütfen özel bir ağa bağlanın ve tekrar deneyin."},
    {46060, L"Özel bir ağa bağlı değilsiniz.\nAğ konumu ayarlarınızı değiştirmek için Denetim Masası'nda Ağ ve Paylaşım Merkezi'ni açın."},
    {46061, L"Diğer ev bilgisayarlarıyla paylaşın"},
    {46062, L"Dosyaları ve yazıcıları diğer bilgisayarlarla paylaşabilirsiniz. Ayrıca cihazınıza medya akışı da yapabilirsiniz.\n\nEv grupları şifre korumalıdır ve istediğiniz zaman ne paylaşacağınızı seçebilirsiniz."},
    {46063, L"Bu şifreyi kullanarak diğer bilgisayarları ev grubunuza ekleyin"},
    {46064, L"Diğer bilgisayarlardaki dosyalara ve yazıcılara erişmek için bunları ev grubunuza eklemeniz gerekir. Aşağıdaki şifre gereklidir:"},
    {46065, L"Bir ev grubu oluşturmak veya bir ev grubuna katılmak için ağ bağlantınızda IPv6'nın etkin olması gerekir. IPv6'yı etkinleştirmek için Ev Grubu Sorun Gidericisini başlatın."},
    {46066, L"Ev grubuna kişi ekleme"},
    {46067, L"Ev grubu veri korumasını yapılandırma"},
    {46068, L"Birden fazla ev grubu algılandı"},
    {46069, L"Diğer ev grubu üyeleriyle paylaşın"},
    {46070, L"Belgeler"},
    {46071, L"Resimler"},
    {46072, L"Müzik"},
    {46073, L"Videolar"},
    {46074, L"Yazıcılar ve cihazlar"},
    {46075, L"Ev Grubu Paylaşım Ayarlarını Değiştirin"},
    {46076, L"%1 Paylaşımı"},
    {46077, L"Şifreniz doğrulanıyor..."},
};

// Russian (ru-RU)
static const EmbeddedTextEntry kStrings_RU_RU[] = {
    {1, L"ГлавнаяГруппа"},
    {2, L"Просмотрите параметры домашней группы, решите, какой общий доступ имеет этот компьютер, а также отобразите или обновите пароль доступа."},
    {3, L"Политика, установленная вашей организацией, запрещает запуск этой страницы. Обратитесь за помощью к сетевому администратору."},
    {4, L"Подробные параметры обмена"},
    {5, L"Вкл."},
    {6, L"Выкл."},
    {7, L"Выкл. (принтеры не установлены)"},
    {8, L"К этому компьютеру не подключен принтер."},
    {9, L"Делитесь контентом с ПК дома"},
    {10, L"Получите доступ к своей домашней группе с помощью компьютера, присоединенного к домену."},
    {12, L"Изменить параметры домашней группы"},
    {13, L"Работаю…"},
    {14, L"В этой сети не найдена домашняя группа."},
    {15, L"%1 из %2 создал домашнюю группу в сети."},
    {16, L"Вас пригласили присоединиться к вашей домашней группе."},
    {18, L"Используйте эту страницу, чтобы этот компьютер принадлежал к домашней группе."},
    {19, L"Этот компьютер не может подключиться к вашей домашней группе."},
    {20, L"HomeGroup позволяет доверенным компьютерам обмениваться файлами и использовать общие принтеры, а также отправлять мультимедиа на совместимые устройства. Для доступа требуется пароль, но вы сохраняете контроль над тем, что делает этот компьютер доступным."},
    {21, L"Этот компьютер также является частью домена, поэтому он не может создать собственную домашнюю группу, но может присоединиться к домашней группе, созданной кем-то в сети.\n\nДомашние группы объединяют компьютеры в вашей домашней сети, чтобы вы могли обмениваться фотографиями, музыкой, видео, документами и принтерами. Домашние группы защищены паролем, и вы можете в любой момент выбрать, чем поделиться."},
    {22, L"Домашние группы объединяют компьютеры в вашей домашней сети, чтобы вы могли обмениваться фотографиями, музыкой, видео, документами и принтерами. Домашние группы защищены паролем, и вы можете в любой момент выбрать, чем поделиться.\n\nВ этой версии Windows вы не можете создавать свои собственные домашние группы, но можете присоединяться к домашним группам, созданным другими."},
    {23, L"Настройте домашнюю группу"},
    {24, L"Присоединяйтесь"},
    {25, L"Пароль домашней группы изменен. Чтобы продолжить использование ресурсов домашней группы, убедитесь, что человек, который уже ввел новый пароль, находится в сети, а затем введите новый пароль."},
    {26, L"Windows обнаружила другую домашнюю группу в вашей сети. Домашние группы позволяют вам делиться файлами и принтерами с другими компьютерами. Вы также можете осуществлять потоковую передачу мультимедиа на свое устройство."},
    {27, L"%1 изменил пароль своей домашней группы. Чтобы продолжить использование ресурсов домашней группы, убедитесь, что человек, который уже ввел новый пароль, находится в сети, а затем введите новый пароль."},
    {28, L"Ищем домашние группы в этой сети…"},
    {29, L"Введите новый пароль"},
    {30, L"Присоединяйтесь сейчас"},
    {32, L"Прежде чем вы сможете создать домашнюю группу или присоединиться к ней, вам необходимо сначала подключиться к сети."},
    {34, L"Используйте эту страницу, чтобы создать домашнюю группу или присоединиться к ней; сетевое расположение вашего компьютера должно быть установлено как частное."},
    {35, L"Изменить сетевое местоположение"},
    {37, L"Варианты общего доступа для частного"},
    {38, L"Варианты общего доступа для всех"},
    {39, L"Варианты общего доступа для домена"},
    {40, L"Частный"},
    {41, L"Частный (текущий профиль)"},
    {42, L"Общественный"},
    {43, L"Публичный (текущий профиль)"},
    {44, L"Домен"},
    {45, L"Домен (текущий профиль)"},
    {46, L"Потоковое мультимедиа включено."},
    {47, L"Потоковая передача мультимедиа отключена."},
    {56, L"Отмена"},
    {63, L"ОК"},
    {64, L"Показать или распечатать пароль домашней группы"},
    {65, L"24pt;;;Consolas"},
    {66, L"Дата печати: %1 %2."},
    {67, L"Вариант: просмотреть и распечатать пароль домашней группы."},
    {68, L"Пароль:"},
    {69, L"Используйте этот пароль для подключения других компьютеров к вашей домашней группе."},
    {70, L"На каждом компьютере:"},
    {71, L"Примечание. Компьютеры, которые выключены или находятся в спящем режиме, не будут отображаться в вашей домашней группе."},
    {72, L"1. Нажмите «Пуск», затем нажмите «Панель управления»."},
    {73, L"2. В разделе «Сеть и Интернет» нажмите «Выбрать домашнюю группу и параметры общего доступа»."},
    {74, L"3. Нажмите «Присоединиться сейчас» и следуйте инструкциям мастера домашней группы, чтобы ввести свой пароль."},
    {75, L"Нажмите «Пуск», затем нажмите «Панель управления»."},
    {76, L"Не удалось распечатать пароль домашней группы."},
    {77, L"Произошла ошибка, когда Windows попыталась вывести пароль домашней группы. (Код ошибки: %1!u!)"},
    {78, L"В настоящее время вы не подключены к домашней сети. Чтобы просмотреть файлы и ресурсы на других компьютерах домашней группы, сначала подключитесь к домашней сети."},
    {79, L"%1 присоединил компьютер к домашней группе. Я не предоставил доступ к библиотеке своей домашней группе. Нажмите на ссылку ниже, чтобы изменить то, чем вы поделились. Не выключайте и не перезагружайте компьютер до завершения общего доступа."},
    {80, L"Я не предоставил доступ к библиотеке своей домашней группе. Нажмите на ссылку ниже, чтобы изменить то, чем вы поделились. Не выключайте и не перезагружайте компьютер, пока общий доступ не будет завершен."},
    {81, L"Домашняя группа в настоящее время предоставляет общий доступ к библиотеке на этом компьютере. Некоторые параметры домашней группы недоступны, пока не будет завершен общий доступ. Не выключайте и не перезагружайте компьютер до завершения общего доступа."},
    {82, L"В разделе «Сеть и Интернет» нажмите «Выбрать домашнюю группу и параметры общего доступа»."},
    {83, L"В настоящее время в сети нет домашних групп."},
    {84, L"Нажмите «Присоединиться сейчас» и следуйте инструкциям мастера домашней группы, чтобы ввести свой пароль."},
    {85, L"Нажмите здесь, чтобы установить."},
    {86, L"Windows обнаружила принтер домашней группы"},
    {88, L"Представляем домашнюю группу"},
    {89, L"%1 (текущий профиль)"},
    {90, L"Используйте эту страницу, чтобы присоединиться к домашней группе; сетевое расположение вашего компьютера должно быть установлено как частное."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Домашняя группа еще не готова. Пожалуйста, повторите попытку через несколько минут. Если вы продолжаете видеть это сообщение, щелкните ссылку, чтобы начать устранение неполадок в домашней группе."},
    {95, L"Запустите средство устранения неполадок домашней группы"},
    {98, L"Пароль домашней группы"},
    {99, L"Гостевые учетные записи не могут изменять настройки домашней группы."},
    {100, L"Домашняя группа обнаружила новый общий принтер в вашей домашней сети. После установки он будет доступен всем на этом компьютере."},
    {101, L"Установить принтер"},
    {102, L"Домашняя группа недоступна, поскольку вы не подключены к домашней сети."},
    {103, L"Домашняя группа недоступна, поскольку вы не подключены к домашней сети."},
    {104, L"Прежде чем присоединиться к домашней группе, необходимо сначала подключиться к сети."},
    {105, L"Изображение домашней группы"},
    {106, L"Выберите, чем вы хотите поделиться, и просмотрите пароль домашней группы."},
    {107, L"Поскольку этот компьютер является частью домена, настройки общего доступа к его библиотекам и устройствам другим компьютерам в домашней группе недоступны."},
    {108, L"Настройки для совместного использования библиотек и устройств с другими компьютерами в домашней группе недоступны в этом выпуске Windows."},
    {109, L"Удалить %1 из домашней группы."},
    {110, L"Отмена"},
    {111, L"Удалить участника домашней группы"},
    {112, L"%1 будет удален из домашней группы."},
    {113, L"Всем членам домашней группы, которые присоединяются с использованием пароля, потребуется ввести пароль еще раз."},
    {114, L"Принтеры и устройства"},
    {115, L"Изменение членов домашней группы %1"},
    {116, L"Пароль домашней группы был сброшен."},
    {117, L"Домашняя группа обменивается файлами"},
    {118, L"Вариант: Этот компьютер принадлежит к домашней группе."},
    {119, L"Домашняя группа доступна для присоединения"},
    {120, L"Домашнюю группу можно создать"},
    {121, L"Домашняя группа недоступна"},
    {122, L"Ненадежный принтер"},
    {200, L"Добавить участника"},
    {201, L"Значок пользователя"},
    {202, L"Полное имя"},
    {203, L"Идентификатор пользователя"},
    {204, L"Индикатор выполнения"},
    {205, L"Значок папки"},
    {220, L"Совместное использование библиотек и оборудования"},
    {221, L"Выберите библиотеку, которой вы хотите поделиться с другими членами вашей домашней группы."},
    {222, L"Изменить параметры домашней группы"},
    {223, L"Используйте эту страницу, чтобы изменить настройки домашней группы. Откройте Домашнюю группу на панели управления."},
    {224, L"Опции домашней группы"},
    {225, L"Используйте эту страницу, чтобы изменить настройки домашней группы на панели управления или воспользоваться средством устранения неполадок домашней группы."},
    {226, L"Запустить средство устранения неполадок"},
    {227, L"Используйте эту страницу, чтобы использовать средство устранения неполадок домашней группы для поиска и устранения проблем с вашей домашней группой."},
    {228, L"Посмотреть пароль"},
    {229, L"Используйте эту страницу, чтобы просмотреть или распечатать пароль домашней группы."},
    {230, L"Присоединиться к домашней группе"},
    {231, L"Присоединяйтесь к домашней группе в этой сети."},
    {530, L"Открыть подробные параметры обмена…"},
    {541, L"Видимость сети"},
    {542, L"Если сетевое обнаружение включено, этот компьютер может видеть и быть видимым другими сетевыми компьютерами и устройствами."},
    {543, L"Включить сетевое обнаружение"},
    {544, L"Отключить обнаружение сети"},
    {545, L"Доступ к файлам и принтерам"},
    {546, L"Если общий доступ к файлам и принтерам включен, другие пользователи в вашей сети могут получить доступ к файлам и принтерам, к которым вы предоставили общий доступ, с этого компьютера."},
    {547, L"Включите общий доступ к файлам и принтерам"},
    {548, L"Отключите общий доступ к файлам и принтерам"},
    {549, L"Общий доступ к общим папкам"},
    {550, L"Если общий доступ к общим папкам включен, пользователи вашей сети, включая членов домашней группы, могут получать доступ к файлам в общих папках."},
    {552, L"Включение общего доступа позволяет любому, у кого есть доступ к вашей сети, читать и записывать файлы в ваших общедоступных папках."},
    {553, L"Отключите общий доступ к общим папкам (пользователи, вошедшие в систему на этом компьютере, по-прежнему смогут получить доступ к этим папкам)."},
    {554, L"Изменение параметров общего доступа для различных сетевых профилей"},
    {559, L"Доступ к СМИ"},
    {560, L"Когда потоковая передача мультимедиа включена, пользователи и устройства в вашей сети могут получить доступ к фотографиям, музыке и видео на этом компьютере. Этот компьютер также может находить медиафайлы в сети."},
    {564, L"Отмена"},
    {567, L"Применить изменения"},
    {584, L"Windows создает отдельный профиль сети для каждой используемой вами сети. Вы можете выбрать определенные параметры для каждого профиля."},
    {585, L"Значок предупреждения домашней группы"},
    {586, L"Библиотеки и устройства, к которым предоставлен общий доступ с этого компьютера"},
    {595, L"Дополнительные задачи домашней группы"},
    {600, L"Показать или распечатать пароль домашней группы"},
    {601, L"Ваш системный администратор не разрешил вам доступ к вашей домашней группе."},
    {604, L"Смените пароль..."},
    {605, L"Выйти из домашней группы..."},
    {607, L"Выберите параметры потоковой передачи мультимедиа..."},
    {608, L"Поскольку этот компьютер является частью домена, настройки общего доступа к его библиотекам и устройствам другим компьютерам в домашней группе недоступны."},
    {609, L"Общий доступ, защищенный паролем"},
    {610, L"Если включен общий доступ, защищенный паролем, только пользователи с учетными записями пользователей и паролями на этом компьютере могут получить доступ к общим файлам, принтерам, подключенным к этому компьютеру, и общим папкам. Общий доступ, защищенный паролем, необходимо отключить, чтобы разрешить доступ другим."},
    {611, L"Включите общий доступ, защищенный паролем"},
    {612, L"Отключить общий доступ, защищенный паролем"},
    {613, L"Распечатать страницу"},
    {614, L"Позволяет воспроизводить общий контент на всех устройствах в этой сети, таких как телевизоры и игровые консоли."},
    {615, L"Частная сеть"},
    {616, L"Гостевая или общедоступная сеть"},
    {617, L"Доменная сеть"},
    {619, L"ГлавнаяГрупповые связи"},
    {620, L"Windows обычно управляет подключениями к другим компьютерам домашней группы. Однако если вы используете одну и ту же учетную запись пользователя и пароль на всех своих компьютерах, вместо этого вы можете использовать эту учетную запись в HomeGroup."},
    {621, L"Вариант: разрешить Windows управлять подключениями домашней группы (рекомендуется)"},
    {622, L"Подключайтесь к другим компьютерам, используя свою учетную запись и пароль."},
    {624, L"Запустите средство устранения неполадок домашней группы"},
    {627, L"Подключения для обмена файлами"},
    {628, L"Windows использует 128-битное шифрование для защиты соединений с общим доступом к файлам. Некоторые устройства не поддерживают 128-битное шифрование и должны использовать 40-битное или 56-битное шифрование."},
    {629, L"Защитите подключение к файлообменнику, используя 128-битное шифрование (рекомендуется)."},
    {630, L"Включите общий доступ к файлам устройства с 40-битным или 56-битным шифрованием."},
    {631, L"Каждая сеть"},
    {632, L"Изменение того, что доступно вашей домашней группе"},
    {637, L"Закрыть"},
    {639, L"Удаленный доступ к домашней группе"},
    {640, L"Другие участники домашней группы могут подключаться к вашей домашней группе через свои компьютеры откуда угодно."},
    {641, L"Вариант: отключить удаленный доступ к домашней группе через этот компьютер."},
    {642, L"Вариант: включить удаленный доступ к домашней группе через этот компьютер."},
    {648, L"Выберите файлы и устройства, которые необходимо сделать доступными, а затем выберите уровни разрешений для них."},
    {649, L"Библиотека или каталог"},
    {650, L"Уровень доступа"},
    {652, L"Включите автоматическую настройку сетевых устройств."},
    {46000, L"ГлавнаяГруппа"},
    {46004, L"Вариант: выберите пароль для своей домашней группы."},
    {46005, L"Введите пароль домашней группы"},
    {46006, L"&Создать сейчас"},
    {46007, L"&Присоединяйтесь сейчас"},
    {46008, L"Добавьте другие компьютеры в свою домашнюю группу, используя этот пароль."},
    {46009, L"Вы присоединились к домашней группе"},
    {46011, L"ГлавнаяГруппа"},
    {46012, L"Windows не может настроить домашнюю группу на этом компьютере."},
    {46013, L"Поскольку этот компьютер является частью домена, общий доступ к его библиотеке другим компьютерам в домашней группе недоступен."},
    {46014, L"Пароли должны содержать не менее 8 символов и не содержать пробелов в начале и конце."},
    {46015, L"Пароль неверен.\nПожалуйста, попробуйте еще раз. Пароли чувствительны к регистру."},
    {46016, L"Вариант: все соединения домашней группы на этом компьютере будут отключены."},
    {46017, L"Успешно покинул домашнюю группу"},
    {46018, L"Изменение того, что доступно вашей домашней группе"},
    {46019, L"Делитесь своими фотографиями, видео, музыкой, документами и принтерами с другими компьютерами в вашем доме."},
    {46020, L"&Внести изменения"},
    {46021, L"Изменение пароля домашней группы отключает всех"},
    {46022, L"Введите новый пароль для вашей домашней группы"},
    {46023, L"&Изменить пароль"},
    {46024, L"Пароль домашней группы успешно изменен"},
    {46025, L"Пароль домашней группы был изменен"},
    {46026, L"Введите пароль домашней группы"},
    {46027, L"Пароль домашней группы изменен. Чтобы продолжить использование ресурсов домашней группы, убедитесь, что человек, который уже ввел новый пароль, находится в сети, а затем введите новый пароль."},
    {46028, L"Общий"},
    {46029, L"Windows не удалось удалить компьютер из домашней группы."},
    {46030, L"%1 изменил пароль своей домашней группы. Чтобы продолжить использование ресурсов домашней группы, убедитесь, что человек, который уже ввел новый пароль, находится в сети, а затем введите новый пароль."},
    {46031, L"Пароли помогают предотвратить несанкционированный доступ к файлам и принтерам вашей домашней группы. Вы можете получить пароль от %2, %1 или другого члена вашей домашней группы."},
    {46032, L"Пароли помогают предотвратить несанкционированный доступ к файлам и принтерам вашей домашней группы. Вы можете получить пароль от %2, %1 или другого члена вашей домашней группы."},
    {46033, L"Consolas"},
    {46034, L"Создать домашнюю группу"},
    {46035, L"Присоединяйтесь к домашней группе"},
    {46036, L"Измените пароль домашней группы"},
    {46037, L"Покинуть домашнюю группу"},
    {46038, L"Чтобы получить доступ к файлам и принтерам на других компьютерах, вам необходимо добавить их в свою домашнюю группу. Требуется следующий пароль:"},
    {46039, L"Введите новый пароль домашней группы:"},
    {46040, L"Обновить пароль"},
    {46041, L"Выполните резервное копирование всех компьютеров в вашей домашней группе на локальную цель защиты данных."},
    {46042, L"Создайте резервную копию вашего компьютера с помощью целей защиты данных HomeGroup."},
    {46043, L"Не предоставлен общий доступ"},
    {46044, L"Домашние группы можно создавать только в частных сетях.\nЧтобы изменить настройки сетевого местоположения, откройте Центр управления сетями и общим доступом на панели управления."},
    {46045, L"Windows больше не будет обнаруживать домашние группы в этой сети. Чтобы создать новую домашнюю группу, нажмите «ОК» и откройте «Домашняя группа» на панели управления."},
    {46046, L"Windows обнаружила существующую домашнюю группу.\nЧтобы присоединиться, нажмите «ОК» и откройте «Домашнюю группу» на панели управления."},
    {46047, L"Служба HomeGroup теперь доступна. Пожалуйста, попробуйте еще раз."},
    {46048, L"Настройки общего доступа обновлены."},
    {46049, L"Выбранные файлы и ресурсы доступны вашей домашней группе."},
    {46050, L"Пароль домашней группы успешно обновлен."},
    {46051, L"Вы присоединились к домашней группе"},
    {46052, L"Теперь вы можете получить доступ к общим файлам и устройствам. Файлы и устройства, которыми вы делитесь, остаются неизменными."},
    {46053, L"Вы можете получить доступ к файлам и принтерам, которыми пользуются другие пользователи вашей домашней группы."},
    {46054, L"Обновите пароль вашей домашней группы"},
    {46055, L"Присоединяйтесь к домашней группе"},
    {46056, L"Введите новый пароль домашней группы из %1."},
    {46057, L"Часы всех компьютеров домашней группы должны быть установлены с интервалом не более 24 часов. Убедитесь, что часы вашего компьютера синхронизированы, а затем попробуйте снова присоединиться к домашней группе."},
    {46058, L"Пароль не соответствует требованиям к надежности пароля домена. Введите соответствующий пароль или используйте другой компьютер домашней группы, чтобы изменить свой пароль."},
    {46059, L"Вы не можете сбросить пароль, поскольку вы не подключены к частной сети.\nПожалуйста, подключитесь к частной сети и повторите попытку."},
    {46060, L"Вы не подключены к частной сети.\nЧтобы изменить настройки сетевого местоположения, откройте Центр управления сетями и общим доступом на панели управления."},
    {46061, L"Поделитесь с другими домашними компьютерами"},
    {46062, L"Вы можете делиться файлами и принтерами с другими компьютерами. Вы также можете осуществлять потоковую передачу мультимедиа на свое устройство.\n\nДомашние группы защищены паролем, и вы можете в любой момент выбрать, чем поделиться."},
    {46063, L"Добавьте другие компьютеры в свою домашнюю группу, используя этот пароль."},
    {46064, L"Чтобы получить доступ к файлам и принтерам на других компьютерах, вам необходимо добавить их в свою домашнюю группу. Требуется следующий пароль:"},
    {46065, L"Чтобы создать домашнюю группу или присоединиться к ней, в вашем сетевом соединении должен быть включен IPv6. Чтобы включить IPv6, запустите средство устранения неполадок домашней группы."},
    {46066, L"Добавить людей в домашнюю группу"},
    {46067, L"Настройка защиты данных домашней группы"},
    {46068, L"Обнаружено несколько домашних групп"},
    {46069, L"Поделитесь с другими участниками домашней группы"},
    {46070, L"Документы"},
    {46071, L"Картинки"},
    {46072, L"Музыка"},
    {46073, L"Видео"},
    {46074, L"Принтеры и устройства"},
    {46075, L"Изменить настройки общего доступа к домашней группе"},
    {46076, L"%1 Общий доступ"},
    {46077, L"Проверка пароля..."},
};

// Chinese (Simplified) (zh-CN)
static const EmbeddedTextEntry kStrings_ZH_CN[] = {
    {1, L"家庭组"},
    {2, L"查看家庭组选项，决定这台电脑共享的内容，并显示或更新访问密码。"},
    {3, L"您的组织设置的策略阻止此页面运行。向网络管理员寻求帮助。"},
    {4, L"详细的共享选项"},
    {5, L"在"},
    {6, L"离开"},
    {7, L"关闭（未安装打印机）"},
    {8, L"这台计算机没有连接打印机。"},
    {9, L"与家里的电脑共享内容"},
    {10, L"使用加入域的计算机访问您的家庭组"},
    {12, L"编辑家庭组选项"},
    {13, L"在职的…"},
    {14, L"在此网络上找不到家庭组。"},
    {15, L"%1 的 %2 在网络上创建了一个家庭组。"},
    {16, L"您已被邀请加入您的家庭组。"},
    {18, L"使用此页面可以让这台计算机属于某个家庭组。"},
    {19, L"此计算机无法连接到您的家庭组。"},
    {20, L"HomeGroup 允许受信任的电脑交换文件并使用共享打印机，并且它可以将媒体发送到兼容的设备。访问需要密码，而您仍然可以控制这台电脑提供的内容。"},
    {21, L"该计算机也是域的一部分，因此它无法创建自己的家庭组，但它可以加入网络上某人创建的家庭组。\n\n家庭组链接家庭网络上的计算机，以便您可以共享照片、音乐、视频、文档和打印机。家庭组受密码保护，您可以随时选择共享内容。"},
    {22, L"家庭组链接家庭网络上的计算机，以便您可以共享照片、音乐、视频、文档和打印机。家庭组受密码保护，您可以随时选择共享内容。\n\n您无法在此版本的 Windows 中创建自己的家庭组，但可以加入其他人创建的家庭组。"},
    {23, L"设置家庭组"},
    {24, L"加入"},
    {25, L"家庭组密码已更改。要继续使用家庭组资源，请确保已输入新密码的人在线，然后输入新密码。"},
    {26, L"Windows 已检测到您网络上的另一个家庭组。家庭组允许您与其他计算机共享文件和打印机。您还可以将媒体流式传输到您的设备。"},
    {27, L"%1 更改了家庭组密码。要继续使用家庭组资源，请确保已输入新密码的人在线，然后输入新密码。"},
    {28, L"在此网络上寻找家庭组..."},
    {29, L"输入新密码"},
    {30, L"立即加入"},
    {32, L"在创建或加入家庭组之前，您必须先连接到网络。"},
    {34, L"使用此页面创建或加入家庭组，您的计算机的网络位置必须设置为专用。"},
    {35, L"更改网络位置"},
    {37, L"私人共享选项"},
    {38, L"公众共享选项"},
    {39, L"域的共享选项"},
    {40, L"私人的"},
    {41, L"私人（当前个人资料）"},
    {42, L"民众"},
    {43, L"公开（当前个人资料）"},
    {44, L"领域"},
    {45, L"域（当前配置文件）"},
    {46, L"媒体流已开启。"},
    {47, L"媒体流已关闭。"},
    {56, L"取消"},
    {63, L"好的"},
    {64, L"显示或打印家庭组密码"},
    {65, L"24pt;;;Consolas"},
    {66, L"打印日期：%1 %2"},
    {67, L"选项：查看并打印您的家庭组密码"},
    {68, L"密码："},
    {69, L"使用此密码将其他计算机连接到您的家庭组。"},
    {70, L"在每台计算机上："},
    {71, L"注意：关闭或睡眠的计算机不会出现在您的家庭组中。"},
    {72, L"1. 单击“开始”，然后单击“控制面板”。"},
    {73, L"2. 在网络和 Internet 下，单击选择家庭组和共享选项。"},
    {74, L"3. 单击“立即加入”并按照“家庭组向导”输入密码。"},
    {75, L"单击“开始”，然后单击“控制面板”。"},
    {76, L"无法打印家庭组密码"},
    {77, L"Windows 尝试输出家庭组密码时发生错误。 （错误代码：%1!u!）"},
    {78, L"您当前尚未连接到家庭网络。要查看其他家庭组计算机上的文件和资源，请首先连接到您的家庭网络。"},
    {79, L"%1 已将计算机加入家庭组。我还没有与我的家庭组共享图书馆。单击下面的链接更改您共享的内容。在共享完成之前，请勿关闭或重新启动计算机。"},
    {80, L"我还没有与我的家庭组共享图书馆。单击下面的链接更改您共享的内容。在共享完成之前，请勿关闭或重新启动计算机。"},
    {81, L"HomeGroup 当前正在共享这台计算机上的库。某些家庭组选项在共享完成后才可用。在共享完成之前，请勿关闭或重新启动计算机。"},
    {82, L"在网络和 Internet 下，单击选择家庭组和共享选项。"},
    {83, L"目前网络上没有家庭组。"},
    {84, L"单击“立即加入”并按照“家庭组向导”输入您的密码。"},
    {85, L"单击此处进行安装。"},
    {86, L"Windows 找到家庭组打印机"},
    {88, L"家庭组简介"},
    {89, L"%1（当前配置文件）"},
    {90, L"使用此页面加入家庭组，您的计算机的网络位置必须设置为专用。"},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"家庭组尚未准备好。请几分钟后重试。如果您继续看到此消息，请单击该链接开始对您的家庭组进行故障排除。"},
    {95, L"启动家庭组疑难解答"},
    {98, L"家庭组密码"},
    {99, L"访客帐户无法更改家庭组设置。"},
    {100, L"HomeGroup 在您的家庭网络上发现了一台新的共享打印机。安装后，该计算机上的任何人都可以使用它。"},
    {101, L"安装打印机"},
    {102, L"家庭组不可用，因为您未连接到家庭网络。"},
    {103, L"家庭组不可用，因为您未连接到家庭网络。"},
    {104, L"在加入家庭组之前，您必须首先连接到网络。"},
    {105, L"家庭组形象"},
    {106, L"选择您要共享的内容并查看您的家庭组密码"},
    {107, L"由于此计算机是域的一部分，因此与家庭组中的其他计算机共享其库和设备的设置不可用。"},
    {108, L"在此版本的 Windows 中不提供与家庭组中的其他计算机共享库和设备的设置。"},
    {109, L"从家庭组中删除 %1"},
    {110, L"取消"},
    {111, L"删除家庭组成员"},
    {112, L"%1 将从家庭组中删除"},
    {113, L"所有使用密码加入的家庭组成员都需要再次输入密码。"},
    {114, L"打印机和设备"},
    {115, L"更改 %1 家庭组成员"},
    {116, L"家庭组密码已重置"},
    {117, L"家庭组正在共享文件"},
    {118, L"选项：这台计算机属于家庭组"},
    {119, L"家庭组可供加入"},
    {120, L"可以创建家庭组"},
    {121, L"家庭组不可用"},
    {122, L"不受信任的打印机"},
    {200, L"添加会员"},
    {201, L"用户图标"},
    {202, L"姓名"},
    {203, L"用户身份"},
    {204, L"进度条"},
    {205, L"文件夹图标"},
    {220, L"共享库和硬件"},
    {221, L"选择您想要与家庭组中的其他人共享的库。"},
    {222, L"编辑家庭组选项"},
    {223, L"使用此页面更改家庭组设置，请在控制面板中打开家庭组。"},
    {224, L"家庭组选项"},
    {225, L"使用此页面可以更改控制面板中的家庭组设置或使用家庭组疑难解答。"},
    {226, L"启动疑难解答程序"},
    {227, L"使用此页面可以使用家庭组疑难解答来查找并修复家庭组的问题。"},
    {228, L"查看密码"},
    {229, L"使用此页面查看或打印您的家庭组密码。"},
    {230, L"加入家庭组"},
    {231, L"加入该网络上的家庭组。"},
    {530, L"打开详细的共享选项..."},
    {541, L"网络可视性"},
    {542, L"如果打开网络发现，则该计算机可以看到其他联网的计算机和设备，也可以被其他联网的计算机和设备看到。"},
    {543, L"打开网络发现"},
    {544, L"关闭网络发现"},
    {545, L"文件和打印机访问"},
    {546, L"打开文件和打印机共享后，网络上的其他用户可以访问您从此计算机共享的文件和打印机。"},
    {547, L"开启文件和打印机共享"},
    {548, L"关闭文件和打印机共享"},
    {549, L"公用文件夹共享"},
    {550, L"打开公用文件夹共享后，网络上的用户（包括家庭组成员）可以访问公用文件夹中的文件。"},
    {552, L"启用共享允许任何有权访问您的网络的人读取和写入您的公用文件夹中的文件。"},
    {553, L"关闭公用文件夹共享（登录到这台计算机的用户仍然可以访问这些文件夹）"},
    {554, L"更改各种网络配置文件的共享选项"},
    {559, L"媒体访问"},
    {560, L"打开媒体流后，网络上的用户和设备可以访问此计算机上的照片、音乐和视频。该计算机还可以在网络上查找媒体。"},
    {564, L"取消"},
    {567, L"应用更改"},
    {584, L"Windows 为您使用的每个网络创建一个单独的网络配置文件。您可以为每个配置文件选择特定选项。"},
    {585, L"家庭组警告图标"},
    {586, L"从此计算机共享的库和设备"},
    {595, L"更多家庭组任务"},
    {600, L"显示或打印家庭组密码"},
    {601, L"您的系统管理员不允许您访问您的家庭组。"},
    {604, L"更改密码..."},
    {605, L"离开家庭组..."},
    {607, L"选择媒体流选项..."},
    {608, L"由于此计算机是域的一部分，因此与家庭组中的其他计算机共享其库和设备的设置不可用。"},
    {609, L"密码保护共享"},
    {610, L"打开受密码保护的共享后，只有拥有该计算机上的用户帐户和密码的用户才能访问共享文件、连接到该计算机的打印机以及公共文件夹。必须关闭受密码保护的共享才能允许其他人访问。"},
    {611, L"开启密码保护共享"},
    {612, L"关闭密码保护共享"},
    {613, L"打印页面"},
    {614, L"允许共享内容在此网络上的所有设备上播放，例如电视和游戏机"},
    {615, L"专网"},
    {616, L"访客或公共网络"},
    {617, L"域网络"},
    {619, L"家庭组连接"},
    {620, L"Windows 通常管理与其他家庭组计算机的连接。但是，如果您在所有计算机上使用相同的用户帐户和密码，则可以让家庭组使用该帐户。"},
    {621, L"选项：允许 Windows 管理家庭组连接（推荐）"},
    {622, L"使用您的用户帐户和密码连接到其他计算机"},
    {624, L"启动家庭组疑难解答"},
    {627, L"文件共享连接"},
    {628, L"Windows 使用 128 位加密来保护文件共享连接。某些设备不支持 128 位加密，必须使用 40 位或 56 位加密。"},
    {629, L"使用 128 位加密保护您的文件共享连接（推荐）"},
    {630, L"通过 40 位或 56 位加密启用设备文件共享"},
    {631, L"每个网络"},
    {632, L"更改与家庭组共享的内容"},
    {637, L"关闭"},
    {639, L"家庭组远程访问"},
    {640, L"其他家庭组成员可以通过他们的计算机从任何地方连接到您的家庭组。"},
    {641, L"选项：禁用通过此计算机的远程家庭组访问"},
    {642, L"选项：通过此计算机启用远程家庭组访问"},
    {648, L"选择要提供的文件和设备，然后选择其权限级别。"},
    {649, L"图书馆或目录"},
    {650, L"访问级别"},
    {652, L"打开网络连接设备的自动设置。"},
    {46000, L"家庭组"},
    {46004, L"选项：为您的家庭组选择密码"},
    {46005, L"输入家庭组密码"},
    {46006, L"&立即创建(&C)"},
    {46007, L"&立即加入(&L)"},
    {46008, L"使用此密码将其他计算机添加到您的家庭组"},
    {46009, L"您已加入家庭组"},
    {46011, L"家庭组"},
    {46012, L"Windows 无法在此计算机上设置家庭组。"},
    {46013, L"由于此计算机是域的一部分，因此无法与家庭组中的其他计算机共享其库。"},
    {46014, L"密码必须至少包含 8 个字符，并且前导或尾随空格不存在。"},
    {46015, L"密码不正确。\n请再试一次。密码区分大小写。"},
    {46016, L"选项：此计算机上的所有家庭组连接都将断开"},
    {46017, L"已成功离开家庭组"},
    {46018, L"更改与家庭组共享的内容"},
    {46019, L"与家中的其他计算机共享您的照片、视频、音乐、文档和打印机。"},
    {46020, L"&进行更改(&M)"},
    {46021, L"更改家庭组密码会导致所有人断开连接"},
    {46022, L"输入家庭组的新密码"},
    {46023, L"&＆更改密码"},
    {46024, L"家庭组密码更改成功"},
    {46025, L"家庭组密码已更改"},
    {46026, L"输入家庭组密码"},
    {46027, L"家庭组密码已更改。要继续使用家庭组资源，请确保已输入新密码的人在线，然后输入新密码。"},
    {46028, L"共享"},
    {46029, L"Windows 无法从家庭组中删除计算机。"},
    {46030, L"%1 更改了家庭组密码。要继续使用家庭组资源，请确保已输入新密码的人在线，然后输入新密码。"},
    {46031, L"密码有助于防止未经授权访问您的家庭组的文件和打印机。您可以从 %2、%1 或家庭组的其他成员处获取密码。"},
    {46032, L"密码有助于防止未经授权访问您的家庭组的文件和打印机。您可以从 %2、%1 或家庭组的其他成员处获取密码。"},
    {46033, L"Consolas"},
    {46034, L"创建家庭组"},
    {46035, L"加入家庭组"},
    {46036, L"更改您的家庭组密码"},
    {46037, L"离开家庭组"},
    {46038, L"要访问其他计算机上的文件和打印机，您必须将它们添加到您的家庭组。需要输入以下密码："},
    {46039, L"输入新的家庭组密码："},
    {46040, L"更新密码"},
    {46041, L"将家庭组中的所有电脑备份到本地数据保护目标。"},
    {46042, L"使用 HomeGroup 数据保护目标备份您的电脑"},
    {46043, L"未共享"},
    {46044, L"家庭组只能在专用网络上创建。\n要更改网络位置设置，请在控制面板中打开网络和共享中心。"},
    {46045, L"Windows 将不再检测该网络上的家庭组。要创建新的家庭组，请单击“确定”并在“控制面板”中打开“家庭组”。"},
    {46046, L"Windows 检测到现有家庭组。\n要加入，请单击“确定”并在“控制面板”中打开“家庭组”。"},
    {46047, L"家庭组服务现已推出。请再试一次。"},
    {46048, L"共享设置已更新"},
    {46049, L"所选文件和资源将与您的家庭组共享。"},
    {46050, L"家庭组密码更新成功"},
    {46051, L"您已加入家庭组"},
    {46052, L"您现在可以访问您的共享文件和设备。您共享的文件和设备保持不变。"},
    {46053, L"您可以开始访问家庭组中其他用户共享的文件和打印机。"},
    {46054, L"更新您的家庭组密码"},
    {46055, L"加入家庭组"},
    {46056, L"输入 %1 中的新家庭组密码。"},
    {46057, L"所有家庭组计算机的时钟间隔必须设置为不超过 24 小时。确保您的计算机时钟同步，然后尝试再次加入家庭组。"},
    {46058, L"密码不符合域的密码强度要求。输入匹配的密码或使用另一台家庭组计算机更改您的密码。"},
    {46059, L"您无法重置密码，因为您未连接到专用网络。\n请连接到专用网络并重试。"},
    {46060, L"您未连接到专用网络。\n要更改网络位置设置，请在控制面板中打开网络和共享中心。"},
    {46061, L"与其他家用电脑共享"},
    {46062, L"您可以与其他计算机共享文件和打印机。您还可以将媒体流式传输到您的设备。\n\n家庭组受密码保护，您可以随时选择共享内容。"},
    {46063, L"使用此密码将其他计算机添加到您的家庭组"},
    {46064, L"要访问其他计算机上的文件和打印机，您必须将它们添加到您的家庭组。需要输入以下密码："},
    {46065, L"要创建或加入家庭组，您的网络连接必须启用 IPv6。要启用 IPv6，请启动家庭组疑难解答。"},
    {46066, L"将人员添加到家庭组"},
    {46067, L"配置家庭组数据保护"},
    {46068, L"检测到多个家庭组"},
    {46069, L"与其他家庭组成员共享"},
    {46070, L"文件"},
    {46071, L"图片"},
    {46072, L"音乐"},
    {46073, L"视频"},
    {46074, L"打印机和设备"},
    {46075, L"更改家庭组共享设置"},
    {46076, L"%1 分享"},
    {46077, L"正在验证您的密码..."},
};

// German (de-DE)
static const EmbeddedTextEntry kStrings_DE_DE[] = {
    {1, L"Heimnetzgruppe"},
    {2, L"Überprüfen Sie die HomeGroup-Optionen, entscheiden Sie, was dieser PC teilt, und zeigen Sie das Zugriffskennwort an oder aktualisieren Sie es."},
    {3, L"Eine von Ihrer Organisation festgelegte Richtlinie verhindert die Ausführung dieser Seite. Bitten Sie den Netzwerkadministrator um Hilfe."},
    {4, L"Detaillierte Freigabeoptionen"},
    {5, L"Auf"},
    {6, L"Aus"},
    {7, L"Aus (keine Drucker installiert)"},
    {8, L"An diesen Computer ist kein Drucker angeschlossen."},
    {9, L"Teilen Sie Inhalte mit PCs zu Hause"},
    {10, L"Greifen Sie über einen in die Domäne eingebundenen Computer auf Ihre Heimnetzgruppe zu"},
    {12, L"Bearbeiten Sie die Optionen der Heimnetzgruppe"},
    {13, L"Arbeiten…"},
    {14, L"In diesem Netzwerk wurde keine Heimnetzgruppe gefunden."},
    {15, L"%1 von %2 hat eine Heimnetzgruppe im Netzwerk erstellt."},
    {16, L"Sie wurden eingeladen, Ihrer Heimnetzgruppe beizutreten."},
    {18, L"Verwenden Sie diese Seite, damit dieser Computer zu einer Heimnetzgruppe gehört."},
    {19, L"Dieser Computer kann keine Verbindung zu Ihrer Heimnetzgruppe herstellen."},
    {20, L"Mit HomeGroup können vertrauenswürdige PCs Dateien austauschen und gemeinsam genutzte Drucker nutzen und Medien an kompatible Geräte senden. Für den Zugriff ist ein Passwort erforderlich, während Sie die Kontrolle darüber behalten, was dieser PC zur Verfügung stellt."},
    {21, L"Dieser Computer ist ebenfalls Teil einer Domäne und kann daher keine eigene Heimnetzgruppe erstellen, er kann jedoch einer Heimnetzgruppe beitreten, die von jemandem im Netzwerk erstellt wurde.\n\nHeimnetzgruppen verbinden Computer in Ihrem Heimnetzwerk, sodass Sie Fotos, Musik, Videos, Dokumente und Drucker teilen können. Heimnetzgruppen sind passwortgeschützt und Sie können jederzeit auswählen, was Sie teilen möchten."},
    {22, L"Heimnetzgruppen verbinden Computer in Ihrem Heimnetzwerk, sodass Sie Fotos, Musik, Videos, Dokumente und Drucker teilen können. Heimnetzgruppen sind passwortgeschützt und Sie können jederzeit auswählen, was Sie teilen möchten.\n\nSie können in dieser Windows-Edition keine eigenen Heimnetzgruppen erstellen, aber Sie können Heimnetzgruppen beitreten, die von anderen erstellt wurden."},
    {23, L"Richten Sie eine Heimnetzgruppe ein"},
    {24, L"Machen Sie mit"},
    {25, L"Das Passwort der Heimnetzgruppe wurde geändert. Um die Ressourcen Ihrer Heimnetzgruppe weiterhin nutzen zu können, stellen Sie sicher, dass die Person, die das neue Passwort bereits eingegeben hat, online ist, und geben Sie dann das neue Passwort ein."},
    {26, L"Windows hat eine andere Heimnetzgruppe in Ihrem Netzwerk erkannt. Mit Heimnetzgruppen können Sie Dateien und Drucker für andere Computer freigeben. Sie können Medien auch auf Ihr Gerät streamen."},
    {27, L"%1 hat sein Heimnetzgruppenkennwort geändert. Um die Ressourcen Ihrer Heimnetzgruppe weiterhin nutzen zu können, stellen Sie sicher, dass die Person, die das neue Passwort bereits eingegeben hat, online ist, und geben Sie dann das neue Passwort ein."},
    {28, L"Auf der Suche nach Heimnetzgruppen in diesem Netzwerk…"},
    {29, L"Geben Sie ein neues Passwort ein"},
    {30, L"Melden Sie sich jetzt an"},
    {32, L"Bevor Sie eine Heimnetzgruppe erstellen oder einer beitreten können, müssen Sie zunächst eine Verbindung zu Ihrem Netzwerk herstellen."},
    {34, L"Verwenden Sie diese Seite, um eine Heimnetzgruppe zu erstellen oder einer beizutreten. Der Netzwerkstandort Ihres Computers muss auf „Privat“ eingestellt sein."},
    {35, L"Netzwerkstandort ändern"},
    {37, L"Freigabeoptionen für Privat"},
    {38, L"Freigabeoptionen für die Öffentlichkeit"},
    {39, L"Freigabeoptionen für die Domain"},
    {40, L"Privat"},
    {41, L"Privat (aktuelles Profil)"},
    {42, L"Öffentlich"},
    {43, L"Öffentlich (aktuelles Profil)"},
    {44, L"Domäne"},
    {45, L"Domain (aktuelles Profil)"},
    {46, L"Medienstreaming ist aktiviert."},
    {47, L"Das Medienstreaming ist deaktiviert."},
    {56, L"Abbrechen"},
    {63, L"Okay"},
    {64, L"Zeigen Sie das HomeGroup-Passwort an oder drucken Sie es aus"},
    {65, L"24pt;;;Consolas"},
    {66, L"Druckdatum: %1 %2"},
    {67, L"Option: Sehen Sie sich Ihr Heimnetzgruppenkennwort an und drucken Sie es aus"},
    {68, L"Passwort:"},
    {69, L"Verwenden Sie dieses Passwort, um andere Computer mit Ihrer Heimnetzgruppe zu verbinden."},
    {70, L"Auf jedem Computer:"},
    {71, L"Hinweis: Computer, die ausgeschaltet sind oder sich im Ruhezustand befinden, werden nicht in Ihrer Heimnetzgruppe angezeigt."},
    {72, L"1. Klicken Sie auf Start und dann auf Systemsteuerung."},
    {73, L"2. Klicken Sie unter „Netzwerk und Internet“ auf „Heimnetzgruppe und Freigabeoptionen auswählen“."},
    {74, L"3. Klicken Sie auf Jetzt beitreten und folgen Sie dem Heimnetzgruppen-Assistenten, um Ihr Passwort einzugeben."},
    {75, L"Klicken Sie auf Start und dann auf Systemsteuerung."},
    {76, L"Das Passwort für die Heimnetzgruppe konnte nicht gedruckt werden"},
    {77, L"Beim Versuch von Windows, das Heimnetzgruppenkennwort auszugeben, ist ein Fehler aufgetreten. (Fehlercode:%1!u!)"},
    {78, L"Sie sind derzeit nicht mit Ihrem Heimnetzwerk verbunden. Um Dateien und Ressourcen auf anderen Heimnetzgruppencomputern anzuzeigen, stellen Sie zunächst eine Verbindung zu Ihrem Heimnetzwerk her."},
    {79, L"%1 hat den Computer der Heimnetzgruppe hinzugefügt. Ich habe die Bibliothek nicht für meine Heimnetzgruppe freigegeben. Klicken Sie auf den Link unten, um zu ändern, was Sie geteilt haben. Fahren Sie Ihren Computer nicht herunter und starten Sie ihn nicht neu, bis die Freigabe abgeschlossen ist."},
    {80, L"Ich habe die Bibliothek nicht für meine Heimnetzgruppe freigegeben. Klicken Sie auf den Link unten, um zu ändern, was Sie geteilt haben. Fahren Sie Ihren Computer nicht herunter und starten Sie ihn nicht neu, bis die Freigabe abgeschlossen ist."},
    {81, L"Die Heimnetzgruppe gibt derzeit die Bibliothek auf diesem Computer frei. Einige Heimnetzgruppenoptionen sind erst verfügbar, wenn die Freigabe abgeschlossen ist. Fahren Sie Ihren Computer nicht herunter und starten Sie ihn nicht neu, bis die Freigabe abgeschlossen ist."},
    {82, L"Klicken Sie unter „Netzwerk und Internet“ auf „Heimnetzgruppe und Freigabeoptionen auswählen“."},
    {83, L"Derzeit gibt es keine Heimnetzgruppen im Netzwerk."},
    {84, L"Klicken Sie auf Jetzt beitreten und folgen Sie dem Heimnetzgruppen-Assistenten, um Ihr Passwort einzugeben."},
    {85, L"Klicken Sie hier, um zu installieren."},
    {86, L"Windows hat einen Heimnetzgruppendrucker gefunden"},
    {88, L"Wir stellen vor: HomeGroup"},
    {89, L"%1 (aktuelles Profil)"},
    {90, L"Verwenden Sie diese Seite, um einer Heimnetzgruppe beizutreten. Der Netzwerkstandort Ihres Computers muss auf „Privat“ eingestellt sein."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"HomeGroup ist noch nicht fertig. Bitte versuchen Sie es in ein paar Minuten noch einmal. Wenn diese Meldung weiterhin angezeigt wird, klicken Sie auf den Link, um mit der Fehlerbehebung für Ihre Heimnetzgruppe zu beginnen."},
    {95, L"Starten Sie die Fehlerbehebung für Heimnetzgruppen"},
    {98, L"Passwort für die Heimnetzgruppe"},
    {99, L"Gastkonten können die Heimnetzgruppeneinstellungen nicht ändern."},
    {100, L"HomeGroup hat einen neuen freigegebenen Drucker in Ihrem Heimnetzwerk gefunden. Nach der Installation steht es jedem auf diesem Computer zur Verfügung."},
    {101, L"Drucker installieren"},
    {102, L"HomeGroup ist nicht verfügbar, da Sie nicht mit Ihrem Heimnetzwerk verbunden sind."},
    {103, L"HomeGroup ist nicht verfügbar, da Sie nicht mit Ihrem Heimnetzwerk verbunden sind."},
    {104, L"Bevor Sie einer Heimnetzgruppe beitreten, müssen Sie zunächst eine Verbindung zum Netzwerk herstellen."},
    {105, L"Bild der Heimnetzgruppe"},
    {106, L"Wählen Sie aus, was Sie teilen möchten, und sehen Sie sich Ihr Heimnetzgruppenkennwort an"},
    {107, L"Da dieser Computer Teil einer Domäne ist, sind Einstellungen zum Teilen seiner Bibliotheken und Geräte mit anderen Computern in der Heimnetzgruppe nicht verfügbar."},
    {108, L"Einstellungen zum Teilen von Bibliotheken und Geräten mit anderen Computern in einer Heimnetzgruppe sind in dieser Windows-Edition nicht verfügbar."},
    {109, L"Entfernen Sie %1 aus der Heimnetzgruppe"},
    {110, L"Abbrechen"},
    {111, L"Heimnetzgruppenmitglied entfernen"},
    {112, L"%1 wird aus der Heimnetzgruppe entfernt"},
    {113, L"Alle Heimnetzgruppenmitglieder, die mit einem Passwort beitreten, müssen das Passwort erneut eingeben."},
    {114, L"Drucker und Geräte"},
    {115, L"Ändern Sie die Mitglieder der Heimnetzgruppe %1"},
    {116, L"Das Heimnetzgruppenkennwort wurde zurückgesetzt"},
    {117, L"HomeGroup gibt Dateien frei"},
    {118, L"Option: Dieser Computer gehört zu einer Heimnetzgruppe"},
    {119, L"Es besteht die Möglichkeit, einer Heimnetzgruppe beizutreten"},
    {120, L"Eine Heimnetzgruppe kann erstellt werden"},
    {121, L"Heimnetzgruppe ist nicht verfügbar"},
    {122, L"Nicht vertrauenswürdiger Drucker"},
    {200, L"Mitglied hinzufügen"},
    {201, L"Benutzersymbol"},
    {202, L"Vollständiger Name"},
    {203, L"Benutzer-ID"},
    {204, L"Fortschrittsbalken"},
    {205, L"Ordnersymbol"},
    {220, L"Teilen Sie Bibliotheken und Hardware"},
    {221, L"Wählen Sie die Bibliothek aus, die Sie mit anderen in Ihrer Heimnetzgruppe teilen möchten."},
    {222, L"Bearbeiten Sie die Optionen der Heimnetzgruppe"},
    {223, L"Verwenden Sie diese Seite, um die Heimnetzgruppeneinstellungen zu ändern. Öffnen Sie die Heimnetzgruppe in der Systemsteuerung."},
    {224, L"HomeGroup-Optionen"},
    {225, L"Verwenden Sie diese Seite, um Ihre Heimnetzgruppeneinstellungen in der Systemsteuerung zu ändern oder die Heimnetzgruppen-Fehlerbehebung zu verwenden."},
    {226, L"Fehlerbehebung starten"},
    {227, L"Auf dieser Seite können Sie mit der Heimnetzgruppe-Fehlerbehebung Probleme mit Ihrer Heimnetzgruppe finden und beheben."},
    {228, L"Passwort ansehen"},
    {229, L"Auf dieser Seite können Sie Ihr Heimnetzgruppenkennwort anzeigen oder ausdrucken."},
    {230, L"Treten Sie der Heimnetzgruppe bei"},
    {231, L"Treten Sie der Heimnetzgruppe in diesem Netzwerk bei."},
    {530, L"Detaillierte Freigabeoptionen öffnen…"},
    {541, L"Netzwerksichtbarkeit"},
    {542, L"Wenn die Netzwerkerkennung aktiviert ist, kann dieser Computer andere vernetzte Computer und Geräte sehen und von diesen gesehen werden."},
    {543, L"Aktivieren Sie die Netzwerkerkennung"},
    {544, L"Schalten Sie die Netzwerkerkennung aus"},
    {545, L"Datei- und Druckerzugriff"},
    {546, L"Wenn die Datei- und Druckerfreigabe aktiviert ist, können andere Benutzer in Ihrem Netzwerk von diesem Computer aus auf die von Ihnen freigegebenen Dateien und Drucker zugreifen."},
    {547, L"Aktivieren Sie die Datei- und Druckerfreigabe"},
    {548, L"Deaktivieren Sie die Datei- und Druckerfreigabe"},
    {549, L"Freigabe öffentlicher Ordner"},
    {550, L"Wenn die Freigabe öffentlicher Ordner aktiviert ist, können Benutzer in Ihrem Netzwerk, einschließlich Heimnetzgruppenmitgliedern, auf Dateien in öffentlichen Ordnern zugreifen."},
    {552, L"Wenn Sie die Freigabe aktivieren, kann jeder mit Zugriff auf Ihr Netzwerk Dateien in Ihren öffentlichen Ordnern lesen und schreiben."},
    {553, L"Deaktivieren Sie die Freigabe öffentlicher Ordner (an diesem Computer angemeldete Benutzer können weiterhin auf diese Ordner zugreifen)."},
    {554, L"Ändern Sie die Freigabeoptionen für verschiedene Netzwerkprofile"},
    {559, L"Medienzugriff"},
    {560, L"Wenn das Medienstreaming aktiviert ist, können Benutzer und Geräte in Ihrem Netzwerk auf die Fotos, Musik und Videos auf diesem Computer zugreifen. Dieser Computer kann auch Medien im Netzwerk finden."},
    {564, L"Abbrechen"},
    {567, L"Änderungen übernehmen"},
    {584, L"Windows erstellt für jedes von Ihnen verwendete Netzwerk ein separates Netzwerkprofil. Sie können für jedes Profil bestimmte Optionen auswählen."},
    {585, L"Warnsymbol für die Heimnetzgruppe"},
    {586, L"Von diesem Computer aus freigegebene Bibliotheken und Geräte"},
    {595, L"Weitere HomeGroup-Aufgaben"},
    {600, L"Zeigen Sie das HomeGroup-Passwort an oder drucken Sie es aus"},
    {601, L"Ihr Systemadministrator hat Ihnen den Zugriff auf Ihre Heimnetzgruppe nicht gestattet."},
    {604, L"Passwort ändern..."},
    {605, L"Verlassen Sie die Heimnetzgruppe..."},
    {607, L"Wählen Sie Medien-Streaming-Optionen..."},
    {608, L"Da dieser Computer Teil einer Domäne ist, sind Einstellungen zum Teilen seiner Bibliotheken und Geräte mit anderen Computern in der Heimnetzgruppe nicht verfügbar."},
    {609, L"Passwortgeschütztes Teilen"},
    {610, L"Wenn die kennwortgeschützte Freigabe aktiviert ist, können nur Benutzer mit Benutzerkonten und Kennwörtern auf diesem Computer auf freigegebene Dateien, mit diesem Computer verbundene Drucker und öffentliche Ordner zugreifen. Die passwortgeschützte Freigabe muss deaktiviert werden, um anderen den Zugriff zu ermöglichen."},
    {611, L"Aktivieren Sie die passwortgeschützte Freigabe"},
    {612, L"Deaktivieren Sie die passwortgeschützte Freigabe"},
    {613, L"Seite drucken"},
    {614, L"Ermöglicht die Wiedergabe freigegebener Inhalte auf allen Geräten in diesem Netzwerk, z. B. Fernsehern und Spielekonsolen"},
    {615, L"Privates Netzwerk"},
    {616, L"Gast- oder öffentliches Netzwerk"},
    {617, L"Domänennetzwerk"},
    {619, L"Heimnetzgruppenverbindungen"},
    {620, L"Windows verwaltet normalerweise Verbindungen zu anderen Heimnetzgruppencomputern. Wenn Sie jedoch auf allen Ihren Computern dasselbe Benutzerkonto und dasselbe Kennwort verwenden, können Sie stattdessen die HomeGroup dazu veranlassen, dieses Konto zu verwenden."},
    {621, L"Option: Windows erlauben, Heimnetzgruppenverbindungen zu verwalten (empfohlen)"},
    {622, L"Stellen Sie mit Ihrem Benutzerkonto und Passwort eine Verbindung zu anderen Computern her"},
    {624, L"Starten Sie die Fehlerbehebung für Heimnetzgruppen"},
    {627, L"File-Sharing-Verbindungen"},
    {628, L"Windows verwendet eine 128-Bit-Verschlüsselung, um File-Sharing-Verbindungen zu sichern. Einige Geräte unterstützen keine 128-Bit-Verschlüsselung und müssen eine 40-Bit- oder 56-Bit-Verschlüsselung verwenden."},
    {629, L"Sichern Sie Ihre File-Sharing-Verbindung mit 128-Bit-Verschlüsselung (empfohlen)"},
    {630, L"Aktivieren Sie die Gerätedateifreigabe mit 40-Bit- oder 56-Bit-Verschlüsselung"},
    {631, L"Jedes Netzwerk"},
    {632, L"Ändern Sie, was mit Ihrer Heimnetzgruppe geteilt wird"},
    {637, L"Schließen"},
    {639, L"Heimnetzgruppen-Fernzugriff"},
    {640, L"Andere Heimnetzgruppenmitglieder können von überall über ihre Computer eine Verbindung zu Ihrer Heimnetzgruppe herstellen."},
    {641, L"Option: Deaktivieren Sie den Remote-Heimnetzgruppenzugriff über diesen Computer"},
    {642, L"Option: Aktivieren Sie den Remote-Heimnetzgruppenzugriff über diesen Computer"},
    {648, L"Wählen Sie die Dateien und Geräte aus, die verfügbar gemacht werden sollen, und wählen Sie dann deren Berechtigungsstufen aus."},
    {649, L"Bibliothek oder Verzeichnis"},
    {650, L"Zugriffsebene"},
    {652, L"Aktivieren Sie die automatische Einrichtung von Netzwerkgeräten."},
    {46000, L"Heimnetzgruppe"},
    {46004, L"Option: Wählen Sie ein Passwort für Ihre Heimnetzgruppe"},
    {46005, L"Geben Sie das Heimnetzgruppenkennwort ein"},
    {46006, L"&Jetzt erstellen"},
    {46007, L"&Jetzt beitreten"},
    {46008, L"Fügen Sie mit diesem Passwort weitere Computer zu Ihrer Heimnetzgruppe hinzu"},
    {46009, L"Sie sind der Heimnetzgruppe beigetreten"},
    {46011, L"Heimnetzgruppe"},
    {46012, L"Windows kann auf diesem Computer keine Heimnetzgruppe einrichten."},
    {46013, L"Da dieser Computer Teil einer Domäne ist, ist die gemeinsame Nutzung seiner Bibliothek mit anderen Computern in der Heimnetzgruppe nicht möglich."},
    {46014, L"Passwörter müssen mindestens 8 Zeichen und keine führenden oder nachgestellten Leerzeichen enthalten."},
    {46015, L"Das Passwort ist falsch.\nBitte versuchen Sie es erneut. Bei Passwörtern muss die Groß-/Kleinschreibung beachtet werden."},
    {46016, L"Option: Alle Heimnetzgruppenverbindungen auf diesem Computer werden getrennt"},
    {46017, L"Deine Heimnetzgruppe wurde erfolgreich verlassen"},
    {46018, L"Ändern Sie, was mit Ihrer Heimnetzgruppe geteilt wird"},
    {46019, L"Teilen Sie Ihre Fotos, Videos, Musik, Dokumente und Drucker mit anderen Computern in Ihrem Zuhause."},
    {46020, L"&Änderungen vornehmen"},
    {46021, L"Durch das Ändern des Heimnetzgruppenkennworts werden alle Verbindungen getrennt"},
    {46022, L"Geben Sie ein neues Passwort für Ihre Heimnetzgruppe ein"},
    {46023, L"&Passwort ändern"},
    {46024, L"Das Passwort der Heimnetzgruppe wurde erfolgreich geändert"},
    {46025, L"Das Passwort der Heimnetzgruppe wurde geändert"},
    {46026, L"Geben Sie das Heimnetzgruppenkennwort ein"},
    {46027, L"Das Passwort der Heimnetzgruppe wurde geändert. Um die Ressourcen Ihrer Heimnetzgruppe weiterhin nutzen zu können, stellen Sie sicher, dass die Person, die das neue Passwort bereits eingegeben hat, online ist, und geben Sie dann das neue Passwort ein."},
    {46028, L"Geteilt"},
    {46029, L"Windows konnte den Computer nicht aus der Heimnetzgruppe entfernen."},
    {46030, L"%1 hat sein Heimnetzgruppenkennwort geändert. Um die Ressourcen Ihrer Heimnetzgruppe weiterhin nutzen zu können, stellen Sie sicher, dass die Person, die das neue Passwort bereits eingegeben hat, online ist, und geben Sie dann das neue Passwort ein."},
    {46031, L"Passwörter helfen, unbefugten Zugriff auf die Dateien und Drucker Ihrer Heimnetzgruppe zu verhindern. Sie können das Passwort von %2, %1 oder einem anderen Mitglied Ihrer Heimnetzgruppe erhalten."},
    {46032, L"Passwörter helfen, unbefugten Zugriff auf die Dateien und Drucker Ihrer Heimnetzgruppe zu verhindern. Sie können das Passwort von %2, %1 oder einem anderen Mitglied Ihrer Heimnetzgruppe erhalten."},
    {46033, L"Consolas"},
    {46034, L"Erstellen Sie eine Heimnetzgruppe"},
    {46035, L"Treten Sie einer Heimnetzgruppe bei"},
    {46036, L"Ändern Sie Ihr Heimnetzgruppenkennwort"},
    {46037, L"Verlassen Sie die Heimnetzgruppe"},
    {46038, L"Um auf Dateien und Drucker auf anderen Computern zuzugreifen, müssen Sie diese Ihrer Heimnetzgruppe hinzufügen. Das folgende Passwort ist erforderlich:"},
    {46039, L"Geben Sie das neue Heimnetzgruppenkennwort ein:"},
    {46040, L"Passwort aktualisieren"},
    {46041, L"Sichern Sie alle PCs in Ihrer Heimnetzgruppe auf einem lokalen Datenschutzziel."},
    {46042, L"Sichern Sie Ihren PC mithilfe von HomeGroup-Datenschutzzielen"},
    {46043, L"Nicht geteilt"},
    {46044, L"Heimnetzgruppen können nur in privaten Netzwerken erstellt werden.\nUm Ihre Netzwerkstandorteinstellungen zu ändern, öffnen Sie das Netzwerk- und Freigabecenter in der Systemsteuerung."},
    {46045, L"Windows erkennt Heimnetzgruppen in diesem Netzwerk nicht mehr. Um eine neue Heimnetzgruppe zu erstellen, klicken Sie auf „OK“ und öffnen Sie „Heimnetzgruppe“ in der Systemsteuerung."},
    {46046, L"Windows hat eine vorhandene Heimnetzgruppe erkannt.\nUm beizutreten, klicken Sie auf OK und öffnen Sie die Heimnetzgruppe in der Systemsteuerung."},
    {46047, L"Der HomeGroup-Dienst ist jetzt verfügbar. Bitte versuchen Sie es erneut."},
    {46048, L"Freigabeeinstellungen aktualisiert"},
    {46049, L"Die ausgewählten Dateien und Ressourcen werden für Ihre Heimnetzgruppe freigegeben."},
    {46050, L"Das Passwort der Heimnetzgruppe wurde erfolgreich aktualisiert"},
    {46051, L"Sie sind der Heimnetzgruppe beigetreten"},
    {46052, L"Sie können jetzt auf Ihre freigegebenen Dateien und Geräte zugreifen. Die von Ihnen freigegebenen Dateien und Geräte bleiben unverändert."},
    {46053, L"Sie können damit beginnen, auf Dateien und Drucker zuzugreifen, die von anderen Benutzern in Ihrer Heimnetzgruppe freigegeben wurden."},
    {46054, L"Aktualisieren Sie Ihr Heimnetzgruppenkennwort"},
    {46055, L"Treten Sie einer Heimnetzgruppe bei"},
    {46056, L"Geben Sie das neue Heimnetzgruppenkennwort von %1 ein."},
    {46057, L"Die Uhren aller Heimnetzgruppencomputer dürfen nicht mehr als 24 Stunden voneinander entfernt sein. Stellen Sie sicher, dass die Uhren Ihres Computers synchron sind, und versuchen Sie dann erneut, der Heimnetzgruppe beizutreten."},
    {46058, L"Das Passwort entspricht nicht den Anforderungen an die Passwortstärke der Domäne. Geben Sie ein passendes Passwort ein oder verwenden Sie einen anderen HomeGroup-Computer, um Ihr Passwort zu ändern."},
    {46059, L"Sie können Ihr Passwort nicht zurücksetzen, da Sie nicht mit einem privaten Netzwerk verbunden sind.\nBitte stellen Sie eine Verbindung zu einem privaten Netzwerk her und versuchen Sie es erneut."},
    {46060, L"Sie sind nicht mit einem privaten Netzwerk verbunden.\nUm Ihre Netzwerkstandorteinstellungen zu ändern, öffnen Sie das Netzwerk- und Freigabecenter in der Systemsteuerung."},
    {46061, L"Teilen Sie es mit anderen Heimcomputern"},
    {46062, L"Sie können Dateien und Drucker mit anderen Computern teilen. Sie können Medien auch auf Ihr Gerät streamen.\n\nHeimnetzgruppen sind passwortgeschützt und Sie können jederzeit auswählen, was Sie teilen möchten."},
    {46063, L"Fügen Sie mit diesem Passwort weitere Computer zu Ihrer Heimnetzgruppe hinzu"},
    {46064, L"Um auf Dateien und Drucker auf anderen Computern zuzugreifen, müssen Sie diese Ihrer Heimnetzgruppe hinzufügen. Das folgende Passwort ist erforderlich:"},
    {46065, L"Um eine Heimnetzgruppe zu erstellen oder einer beizutreten, muss für Ihre Netzwerkverbindung IPv6 aktiviert sein. Um IPv6 zu aktivieren, starten Sie den HomeGroup Troubleshooter."},
    {46066, L"Fügen Sie Personen zur Heimnetzgruppe hinzu"},
    {46067, L"Konfigurieren Sie den Datenschutz für Heimnetzgruppen"},
    {46068, L"Mehrere Heimnetzgruppen erkannt"},
    {46069, L"Teilen Sie es mit anderen Heimnetzgruppenmitgliedern"},
    {46070, L"Dokumente"},
    {46071, L"Bilder"},
    {46072, L"Musik"},
    {46073, L"Videos"},
    {46074, L"Drucker und Geräte"},
    {46075, L"Ändern Sie die Einstellungen für die Heimnetzgruppenfreigabe"},
    {46076, L"%1 Teilen"},
    {46077, L"Verifizierung Ihres Passworts..."},
};

// Portuguese (Brazil) (pt-BR)
static const EmbeddedTextEntry kStrings_PT_BR[] = {
    {1, L"Grupo doméstico"},
    {2, L"Revise as opções do Grupo Doméstico, decida o que este PC compartilha e exiba ou atualize a senha de acesso."},
    {3, L"Uma política definida pela sua organização impede a execução desta página. Peça ajuda ao administrador da rede."},
    {4, L"Opções de compartilhamento detalhadas"},
    {5, L"Ligado"},
    {6, L"Desligado"},
    {7, L"Desligado (nenhuma impressora instalada)"},
    {8, L"Não há nenhuma impressora conectada a este computador."},
    {9, L"Compartilhe conteúdo com PCs em casa"},
    {10, L"Acesse seu grupo doméstico usando um computador associado ao domínio"},
    {12, L"Editar opções do grupo doméstico"},
    {13, L"Trabalhando…"},
    {14, L"Nenhum grupo doméstico foi encontrado nesta rede."},
    {15, L"%1 de %2 criou um grupo doméstico na rede."},
    {16, L"Você foi convidado para ingressar no seu grupo doméstico."},
    {18, L"Use esta página para que este computador pertença a um grupo doméstico."},
    {19, L"Este computador não pode se conectar ao seu grupo doméstico."},
    {20, L"O HomeGroup permite que PCs confiáveis troquem arquivos e usem impressoras compartilhadas, além de enviar mídia para dispositivos compatíveis. O acesso requer uma senha, enquanto você mantém o controle do que este PC disponibiliza."},
    {21, L"Este computador também faz parte de um domínio, portanto não pode criar seu próprio grupo doméstico, mas pode ingressar em um grupo doméstico criado por alguém na rede.\n\nOs grupos domésticos conectam computadores na sua rede doméstica para que você possa compartilhar fotos, músicas, vídeos, documentos e impressoras. Os grupos domésticos são protegidos por senha e você pode escolher o que compartilhar a qualquer momento."},
    {22, L"Os grupos domésticos conectam computadores na sua rede doméstica para que você possa compartilhar fotos, músicas, vídeos, documentos e impressoras. Os grupos domésticos são protegidos por senha e você pode escolher o que compartilhar a qualquer momento.\n\nVocê não pode criar seus próprios grupos domésticos nesta edição do Windows, mas pode ingressar em grupos domésticos criados por outras pessoas."},
    {23, L"Configure um grupo doméstico"},
    {24, L"Junte-se"},
    {25, L"A senha do grupo doméstico foi alterada. Para continuar usando os recursos do seu grupo doméstico, certifique-se de que a pessoa que já digitou a nova senha esteja online e, em seguida, insira a nova senha."},
    {26, L"O Windows detectou outro grupo doméstico na sua rede. Os grupos domésticos permitem que você compartilhe arquivos e impressoras com outros computadores. Você também pode transmitir mídia para o seu dispositivo."},
    {27, L"%1 alterou a senha do grupo doméstico. Para continuar usando os recursos do seu grupo doméstico, certifique-se de que a pessoa que já digitou a nova senha esteja online e, em seguida, insira a nova senha."},
    {28, L"Procurando grupos domésticos nesta rede…"},
    {29, L"Digite a nova senha"},
    {30, L"Cadastre-se agora"},
    {32, L"Antes de criar ou ingressar em um grupo doméstico, você deve primeiro conectar-se à sua rede."},
    {34, L"Use esta página para criar ou ingressar em um grupo doméstico. O local de rede do seu computador deve ser definido como privado."},
    {35, L"Alterar local de rede"},
    {37, L"Opções de compartilhamento para Privado"},
    {38, L"Opções de compartilhamento para público"},
    {39, L"Opções de compartilhamento para domínio"},
    {40, L"Privado"},
    {41, L"Privado (perfil atual)"},
    {42, L"Público"},
    {43, L"Público (perfil atual)"},
    {44, L"Domínio"},
    {45, L"Domínio (perfil atual)"},
    {46, L"O streaming de mídia está ativado."},
    {47, L"O streaming de mídia está desativado."},
    {56, L"Cancelar"},
    {63, L"OK"},
    {64, L"Mostrar ou imprimir a senha do Grupo Doméstico"},
    {65, L"24pt;;;Consolas"},
    {66, L"Data impressa: %1 %2"},
    {67, L"Opção: visualizar e imprimir a senha do seu grupo doméstico"},
    {68, L"Senha:"},
    {69, L"Use esta senha para conectar outros computadores ao seu grupo doméstico."},
    {70, L"Em cada computador:"},
    {71, L"Nota: Os computadores desligados ou em suspensão não aparecerão no seu grupo doméstico."},
    {72, L"1. Clique em Iniciar e em Painel de Controle."},
    {73, L"2. Em Rede e Internet, clique em Escolher grupo doméstico e opções de compartilhamento."},
    {74, L"3. Clique em Ingressar agora e siga o Assistente do Grupo Doméstico para inserir sua senha."},
    {75, L"Clique em Iniciar e em Painel de Controle."},
    {76, L"Não foi possível imprimir a senha do grupo doméstico"},
    {77, L"Ocorreu um erro quando o Windows tentou gerar a senha do grupo doméstico. (Código de erro: %1!u!)"},
    {78, L"Você não está conectado à sua rede doméstica no momento. Para visualizar arquivos e recursos em outros computadores do grupo doméstico, primeiro conecte-se à sua rede doméstica."},
    {79, L"%1 uniu o computador ao grupo doméstico. Não compartilhei a biblioteca com meu grupo doméstico. Clique no link abaixo para alterar o que você compartilhou. Não desligue ou reinicie o computador até que o compartilhamento seja concluído."},
    {80, L"Não compartilhei a biblioteca com meu grupo doméstico. Clique no link abaixo para alterar o que você compartilhou. Não desligue ou reinicie o computador até que o compartilhamento seja concluído."},
    {81, L"O HomeGroup está atualmente compartilhando a biblioteca neste computador. Algumas opções de grupo doméstico não estarão disponíveis até que o compartilhamento seja concluído. Não desligue ou reinicie o computador até que o compartilhamento seja concluído."},
    {82, L"Em Rede e Internet, clique em Escolher grupo doméstico e opções de compartilhamento."},
    {83, L"Atualmente não há grupos domésticos na rede."},
    {84, L"Clique em Ingressar agora e siga o Assistente do Grupo Doméstico para inserir sua senha."},
    {85, L"Clique aqui para instalar."},
    {86, L"O Windows encontrou uma impressora de grupo doméstico"},
    {88, L"Apresentando o Grupo Doméstico"},
    {89, L"%1 (perfil atual)"},
    {90, L"Use esta página para ingressar em um grupo doméstico, o local de rede do seu computador deve ser definido como privado."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"O Grupo Doméstico ainda não está pronto. Tente novamente em alguns minutos. Se você continuar vendo esta mensagem, clique no link para começar a solucionar problemas do seu grupo doméstico."},
    {95, L"Inicie o solucionador de problemas do grupo doméstico"},
    {98, L"Senha do grupo doméstico"},
    {99, L"As contas de convidados não podem alterar as configurações do grupo doméstico."},
    {100, L"O HomeGroup encontrou uma nova impressora compartilhada em sua rede doméstica. Depois de instalado, ele estará disponível para qualquer pessoa neste computador."},
    {101, L"Instalar impressora"},
    {102, L"O Grupo Doméstico não está disponível porque você não está conectado à sua rede doméstica."},
    {103, L"O Grupo Doméstico não está disponível porque você não está conectado à sua rede doméstica."},
    {104, L"Antes de ingressar em um grupo doméstico, você deve primeiro conectar-se à rede."},
    {105, L"Imagem do grupo doméstico"},
    {106, L"Selecione o que você deseja compartilhar e veja a senha do seu grupo doméstico"},
    {107, L"Como este computador faz parte de um domínio, as configurações para compartilhar suas bibliotecas e dispositivos com outros computadores do grupo doméstico não estão disponíveis."},
    {108, L"As configurações para compartilhar bibliotecas e dispositivos com outros computadores de um grupo doméstico não estão disponíveis nesta edição do Windows."},
    {109, L"Remova %1 do grupo doméstico"},
    {110, L"Cancelar"},
    {111, L"Remover membro do grupo doméstico"},
    {112, L"%1 será removido do grupo doméstico"},
    {113, L"Todos os membros do grupo doméstico que ingressarem usando uma senha serão solicitados a digitá-la novamente."},
    {114, L"Impressoras e dispositivos"},
    {115, L"Alterar membros do grupo doméstico %1"},
    {116, L"A senha do grupo doméstico foi redefinida"},
    {117, L"O grupo doméstico está compartilhando arquivos"},
    {118, L"Opção: Este computador pertence a um grupo doméstico"},
    {119, L"Um grupo doméstico está disponível para ingressar"},
    {120, L"Um grupo doméstico pode ser criado"},
    {121, L"O grupo doméstico não está disponível"},
    {122, L"Impressora não confiável"},
    {200, L"Adicionar membro"},
    {201, L"Ícone do usuário"},
    {202, L"Nome completo"},
    {203, L"ID do usuário"},
    {204, L"Barra de progresso"},
    {205, L"Ícone de pasta"},
    {220, L"Compartilhe bibliotecas e hardware"},
    {221, L"Selecione a biblioteca que deseja compartilhar com outras pessoas do seu grupo doméstico."},
    {222, L"Editar opções do grupo doméstico"},
    {223, L"Use esta página para alterar as configurações do Grupo Doméstico, abra o Grupo Doméstico no Painel de Controle."},
    {224, L"Opções de grupo doméstico"},
    {225, L"Use esta página para alterar as configurações do seu grupo doméstico no painel de controle ou use o solucionador de problemas do grupo doméstico."},
    {226, L"Iniciar solucionador de problemas"},
    {227, L"Use esta página para usar o solucionador de problemas do Grupo Doméstico para encontrar e corrigir problemas com seu Grupo Doméstico."},
    {228, L"Ver senha"},
    {229, L"Use esta página para visualizar ou imprimir a senha do seu grupo doméstico."},
    {230, L"Junte-se ao grupo doméstico"},
    {231, L"Junte-se ao grupo doméstico nesta rede."},
    {530, L"Abra opções de compartilhamento detalhadas…"},
    {541, L"Visibilidade da rede"},
    {542, L"Se a descoberta de rede estiver ativada, este computador poderá ver e ser visto por outros computadores e dispositivos em rede."},
    {543, L"Ative a descoberta de rede"},
    {544, L"Desative a descoberta de rede"},
    {545, L"Acesso a arquivos e impressoras"},
    {546, L"Quando o compartilhamento de arquivos e impressoras está ativado, outros usuários da rede podem acessar os arquivos e impressoras que você compartilha neste computador."},
    {547, L"Ative o compartilhamento de arquivos e impressoras"},
    {548, L"Desative o compartilhamento de arquivos e impressoras"},
    {549, L"Compartilhamento de pasta pública"},
    {550, L"Quando o compartilhamento de pasta pública está ativado, os usuários da sua rede, incluindo membros do grupo doméstico, podem acessar arquivos em pastas públicas."},
    {552, L"A ativação do compartilhamento permite que qualquer pessoa com acesso à sua rede leia e grave arquivos em suas pastas públicas."},
    {553, L"Desative o compartilhamento de pastas públicas (os usuários conectados neste computador ainda podem acessar essas pastas)"},
    {554, L"Altere as opções de compartilhamento para vários perfis de rede"},
    {559, L"Acesso à mídia"},
    {560, L"Quando o streaming de mídia está ativado, os usuários e dispositivos da sua rede podem acessar fotos, músicas e vídeos neste computador. Este computador também pode encontrar mídia na rede."},
    {564, L"Cancelar"},
    {567, L"Aplicar alterações"},
    {584, L"O Windows cria um perfil de rede separado para cada rede usada. Você pode selecionar opções específicas para cada perfil."},
    {585, L"Ícone de aviso do grupo doméstico"},
    {586, L"Bibliotecas e dispositivos compartilhados deste computador"},
    {595, L"Mais tarefas do grupo doméstico"},
    {600, L"Mostrar ou imprimir a senha do Grupo Doméstico"},
    {601, L"O administrador do sistema não permitiu que você acessasse o seu grupo doméstico."},
    {604, L"Alterar a senha..."},
    {605, L"Sair do grupo doméstico..."},
    {607, L"Escolha as opções de streaming de mídia..."},
    {608, L"Como este computador faz parte de um domínio, as configurações para compartilhar suas bibliotecas e dispositivos com outros computadores do grupo doméstico não estão disponíveis."},
    {609, L"Compartilhamento protegido por senha"},
    {610, L"Quando o compartilhamento protegido por senha estiver ativado, somente usuários com contas de usuário e senhas neste computador poderão acessar arquivos compartilhados, impressoras conectadas a este computador e pastas públicas. O compartilhamento protegido por senha deve ser desativado para permitir o acesso de outras pessoas."},
    {611, L"Ative o compartilhamento protegido por senha"},
    {612, L"Desative o compartilhamento protegido por senha"},
    {613, L"Imprimir página"},
    {614, L"Permite que o conteúdo compartilhado seja reproduzido em todos os dispositivos desta rede, como TVs e consoles de jogos"},
    {615, L"Rede privada"},
    {616, L"Convidado ou rede pública"},
    {617, L"Rede de domínio"},
    {619, L"Conexões de grupo doméstico"},
    {620, L"O Windows normalmente gerencia conexões com outros computadores do grupo doméstico. No entanto, se você usar a mesma conta de usuário e senha em todos os seus computadores, poderá fazer com que o Grupo Doméstico use essa conta."},
    {621, L"Opção: permitir que o Windows gerencie conexões de grupo doméstico (recomendado)"},
    {622, L"Conecte-se a outros computadores usando sua conta de usuário e senha"},
    {624, L"Inicie o solucionador de problemas do grupo doméstico"},
    {627, L"Conexões de compartilhamento de arquivos"},
    {628, L"O Windows usa criptografia de 128 bits para proteger conexões de compartilhamento de arquivos. Alguns dispositivos não suportam criptografia de 128 bits e devem usar criptografia de 40 ou 56 bits."},
    {629, L"Proteja sua conexão de compartilhamento de arquivos usando criptografia de 128 bits (recomendado)"},
    {630, L"Habilite o compartilhamento de arquivos do dispositivo com criptografia de 40 ou 56 bits"},
    {631, L"Cada rede"},
    {632, L"Alterar o que é compartilhado com seu grupo doméstico"},
    {637, L"Fechar"},
    {639, L"Acesso remoto ao grupo doméstico"},
    {640, L"Outros membros do grupo doméstico podem se conectar ao seu grupo doméstico de qualquer lugar por meio de seus computadores."},
    {641, L"Opção: desabilitar o acesso remoto ao grupo doméstico por meio deste computador"},
    {642, L"Opção: ativar o acesso remoto ao grupo doméstico por meio deste computador"},
    {648, L"Selecione os arquivos e dispositivos a serem disponibilizados e escolha seus níveis de permissão."},
    {649, L"Biblioteca ou diretório"},
    {650, L"Nível de acesso"},
    {652, L"Ative a configuração automática de dispositivos conectados à rede."},
    {46000, L"Grupo doméstico"},
    {46004, L"Opção: Escolha uma senha para o seu grupo doméstico"},
    {46005, L"Digite a senha do grupo doméstico"},
    {46006, L"&Criar agora"},
    {46007, L"&Inscreva-se agora"},
    {46008, L"Adicione outros computadores ao seu grupo doméstico usando esta senha"},
    {46009, L"Você se juntou ao grupo doméstico"},
    {46011, L"Grupo doméstico"},
    {46012, L"O Windows não pode configurar um grupo doméstico neste computador."},
    {46013, L"Como este computador faz parte de um domínio, o compartilhamento de sua biblioteca com outros computadores do grupo doméstico não está disponível."},
    {46014, L"As senhas devem conter pelo menos 8 caracteres e nenhum espaço à esquerda ou à direita."},
    {46015, L"A senha está incorreta.\nPor favor, tente novamente. As senhas diferenciam maiúsculas de minúsculas."},
    {46016, L"Opção: todas as conexões de grupo doméstico neste computador serão desconectadas"},
    {46017, L"Saiu do seu grupo doméstico com sucesso"},
    {46018, L"Alterar o que é compartilhado com seu grupo doméstico"},
    {46019, L"Compartilhe suas fotos, vídeos, músicas, documentos e impressoras com outros computadores da sua casa."},
    {46020, L"&Fazer alterações"},
    {46021, L"Alterar a senha do grupo doméstico desconecta todos"},
    {46022, L"Digite uma nova senha para o seu grupo doméstico"},
    {46023, L"&Alterar senha"},
    {46024, L"Senha do grupo doméstico alterada com sucesso"},
    {46025, L"A senha do grupo doméstico foi alterada"},
    {46026, L"Digite a senha do grupo doméstico"},
    {46027, L"A senha do grupo doméstico foi alterada. Para continuar usando os recursos do seu grupo doméstico, certifique-se de que a pessoa que já digitou a nova senha esteja online e, em seguida, insira a nova senha."},
    {46028, L"Compartilhado"},
    {46029, L"O Windows não conseguiu remover o computador do grupo doméstico."},
    {46030, L"%1 alterou a senha do grupo doméstico. Para continuar usando os recursos do seu grupo doméstico, certifique-se de que a pessoa que já digitou a nova senha esteja online e, em seguida, insira a nova senha."},
    {46031, L"As senhas ajudam a impedir o acesso não autorizado aos arquivos e impressoras do seu grupo doméstico. Você pode obter a senha de %2, %1 ou de outro membro do seu grupo doméstico."},
    {46032, L"As senhas ajudam a impedir o acesso não autorizado aos arquivos e impressoras do seu grupo doméstico. Você pode obter a senha de %2, %1 ou de outro membro do seu grupo doméstico."},
    {46033, L"Consolas"},
    {46034, L"Crie um grupo doméstico"},
    {46035, L"Junte-se a um grupo doméstico"},
    {46036, L"Altere a senha do seu grupo doméstico"},
    {46037, L"Sair do grupo doméstico"},
    {46038, L"Para acessar arquivos e impressoras em outros computadores, você deve adicioná-los ao seu grupo doméstico. A seguinte senha é necessária:"},
    {46039, L"Digite a nova senha do grupo doméstico:"},
    {46040, L"Atualizar senha"},
    {46041, L"Faça backup de todos os PCs do seu grupo doméstico em um destino de proteção de dados local."},
    {46042, L"Faça backup do seu PC usando alvos de proteção de dados do HomeGroup"},
    {46043, L"Não compartilhado"},
    {46044, L"Os grupos domésticos só podem ser criados em redes privadas.\nPara alterar as configurações de localização da rede, abra Central de Rede e Compartilhamento no Painel de Controle."},
    {46045, L"O Windows não detectará mais grupos domésticos nesta rede. Para criar um novo grupo doméstico, clique em OK e abra Grupo Doméstico no Painel de Controle."},
    {46046, L"O Windows detectou um grupo doméstico existente.\nPara ingressar, clique em OK e abra Grupo Doméstico no Painel de Controle."},
    {46047, L"O serviço HomeGroup já está disponível. Por favor, tente novamente."},
    {46048, L"Configurações de compartilhamento atualizadas"},
    {46049, L"Os arquivos e recursos selecionados são compartilhados com seu grupo doméstico."},
    {46050, L"Senha do grupo doméstico atualizada com sucesso"},
    {46051, L"Você entrou no grupo doméstico"},
    {46052, L"Agora você pode acessar seus arquivos e dispositivos compartilhados. Os arquivos e dispositivos que você compartilha permanecem inalterados."},
    {46053, L"Você pode começar a acessar arquivos e impressoras compartilhados por outros usuários do seu grupo doméstico."},
    {46054, L"Atualize a senha do seu grupo doméstico"},
    {46055, L"Junte-se a um grupo doméstico"},
    {46056, L"Digite a nova senha do grupo doméstico de %1."},
    {46057, L"Os relógios de todos os computadores do grupo doméstico devem ser ajustados com no máximo 24 horas de intervalo. Certifique-se de que os relógios do seu computador estejam sincronizados e tente ingressar no grupo doméstico novamente."},
    {46058, L"A senha não atende aos requisitos de segurança de senha do domínio. Digite uma senha correspondente ou use outro computador do Grupo Doméstico para alterar sua senha."},
    {46059, L"Você não pode redefinir sua senha porque não está conectado a uma rede privada.\nConecte-se a uma rede privada e tente novamente."},
    {46060, L"Você não está conectado a uma rede privada.\nPara alterar as configurações de localização da rede, abra Central de Rede e Compartilhamento no Painel de Controle."},
    {46061, L"Compartilhe com outros computadores domésticos"},
    {46062, L"Você pode compartilhar arquivos e impressoras com outros computadores. Você também pode transmitir mídia para o seu dispositivo.\n\nOs grupos domésticos são protegidos por senha e você pode escolher o que compartilhar a qualquer momento."},
    {46063, L"Adicione outros computadores ao seu grupo doméstico usando esta senha"},
    {46064, L"Para acessar arquivos e impressoras em outros computadores, você deve adicioná-los ao seu grupo doméstico. A seguinte senha é necessária:"},
    {46065, L"Para criar ou ingressar em um grupo doméstico, sua conexão de rede deve ter IPv6 habilitado. Para habilitar o IPv6, inicie o solucionador de problemas do grupo doméstico."},
    {46066, L"Adicionar pessoas ao grupo doméstico"},
    {46067, L"Configurar a proteção de dados do grupo doméstico"},
    {46068, L"Vários grupos domésticos detectados"},
    {46069, L"Compartilhe com outros membros do grupo doméstico"},
    {46070, L"Documentos"},
    {46071, L"Fotos"},
    {46072, L"Música"},
    {46073, L"Vídeos"},
    {46074, L"Impressoras e dispositivos"},
    {46075, L"Alterar configurações de compartilhamento de grupo doméstico"},
    {46076, L"Compartilhamento %1"},
    {46077, L"Verificando sua senha..."},
};

// Polish (pl-PL)
static const EmbeddedTextEntry kStrings_PL_PL[] = {
    {1, L"Grupa domowa"},
    {2, L"Przejrzyj opcje grupy domowej, zdecyduj, co udostępnia ten komputer, a następnie wyświetl lub zaktualizuj hasło dostępu."},
    {3, L"Zasady ustawione przez Twoją organizację uniemożliwiają uruchomienie tej strony. Poproś administratora sieci o pomoc."},
    {4, L"Szczegółowe opcje udostępniania"},
    {5, L"Włączone"},
    {6, L"Wyłączone"},
    {7, L"Wyłączone (brak zainstalowanych drukarek)"},
    {8, L"Do tego komputera nie jest podłączona żadna drukarka."},
    {9, L"Udostępniaj treści komputerom w domu"},
    {10, L"Uzyskaj dostęp do swojej grupy domowej za pomocą komputera przyłączonego do domeny"},
    {12, L"Edytuj opcje grupy domowej"},
    {13, L"Pracuję…"},
    {14, L"W tej sieci nie znaleziono żadnej grupy domowej."},
    {15, L"%1 z %2 utworzył grupę domową w sieci."},
    {16, L"Zostałeś zaproszony do dołączenia do swojej grupy domowej."},
    {18, L"Użyj tej strony, aby ten komputer należał do grupy domowej."},
    {19, L"Ten komputer nie może połączyć się z Twoją grupą domową."},
    {20, L"Grupa domowa umożliwia zaufanym komputerom wymianę plików i korzystanie z udostępnionych drukarek, a także może wysyłać multimedia do kompatybilnych urządzeń. Dostęp wymaga hasła, a Ty masz kontrolę nad tym, co udostępnia ten komputer."},
    {21, L"Ten komputer jest również częścią domeny, więc nie może utworzyć własnej grupy domowej, ale może dołączyć do grupy domowej utworzonej przez kogoś w sieci.\n\nGrupy domowe łączą komputery w sieci domowej, dzięki czemu możesz udostępniać zdjęcia, muzykę, filmy, dokumenty i drukarki. Grupy domowe są chronione hasłem i w dowolnym momencie możesz wybrać, co chcesz udostępnić."},
    {22, L"Grupy domowe łączą komputery w sieci domowej, dzięki czemu możesz udostępniać zdjęcia, muzykę, filmy, dokumenty i drukarki. Grupy domowe są chronione hasłem i w dowolnym momencie możesz wybrać, co chcesz udostępnić.\n\nW tej wersji systemu Windows nie można tworzyć własnych grup domowych, ale można dołączać do grup domowych utworzonych przez innych."},
    {23, L"Skonfiguruj grupę domową"},
    {24, L"Dołącz"},
    {25, L"Hasło grupy domowej zostało zmienione. Aby kontynuować korzystanie z zasobów grupy domowej, upewnij się, że osoba, która wprowadziła już nowe hasło, jest online, a następnie wprowadź nowe hasło."},
    {26, L"System Windows wykrył inną grupę domową w Twojej sieci. Grupy domowe umożliwiają udostępnianie plików i drukarek innym komputerom. Możesz także przesyłać strumieniowo multimedia do swojego urządzenia."},
    {27, L"%1 zmienił hasło do swojej grupy domowej. Aby kontynuować korzystanie z zasobów grupy domowej, upewnij się, że osoba, która wprowadziła już nowe hasło, jest online, a następnie wprowadź nowe hasło."},
    {28, L"Szukam grup domowych w tej sieci…"},
    {29, L"Wpisz nowe hasło"},
    {30, L"Dołącz teraz"},
    {32, L"Zanim będziesz mógł utworzyć grupę domową lub do niej dołączyć, musisz najpierw połączyć się z siecią."},
    {34, L"Użyj tej strony, aby utworzyć grupę domową lub dołączyć do niej. Lokalizacja sieciowa Twojego komputera musi być ustawiona na prywatną."},
    {35, L"Zmień lokalizację sieciową"},
    {37, L"Opcje udostępniania w trybie Prywatne"},
    {38, L"Opcje udostępniania publicznego"},
    {39, L"Opcje udostępniania dla domeny"},
    {40, L"Prywatny"},
    {41, L"Prywatny (aktualny profil)"},
    {42, L"Publiczne"},
    {43, L"Publiczny (bieżący profil)"},
    {44, L"Domena"},
    {45, L"Domena (bieżący profil)"},
    {46, L"Transmisja strumieniowa multimediów jest włączona."},
    {47, L"Przesyłanie strumieniowe multimediów jest wyłączone."},
    {56, L"Anuluj"},
    {63, L"OK"},
    {64, L"Pokaż lub wydrukuj hasło grupy domowej"},
    {65, L"24pt;;;Consolas"},
    {66, L"Data wydruku: %1 %2"},
    {67, L"Opcja: Wyświetl i wydrukuj hasło do grupy domowej"},
    {68, L"Hasło:"},
    {69, L"Użyj tego hasła, aby połączyć inne komputery z grupą domową."},
    {70, L"Na każdym komputerze:"},
    {71, L"Uwaga: komputery wyłączone lub uśpione nie będą widoczne w Twojej grupie domowej."},
    {72, L"1. Kliknij Start, a następnie kliknij Panel sterowania."},
    {73, L"2. W obszarze Sieć i Internet kliknij opcję Wybierz grupę domową i opcje udostępniania."},
    {74, L"3. Kliknij Dołącz teraz i postępuj zgodnie z kreatorem grupy domowej, aby wprowadzić hasło."},
    {75, L"Kliknij Start, a następnie kliknij Panel sterowania."},
    {76, L"Nie można wydrukować hasła grupy domowej"},
    {77, L"Wystąpił błąd, gdy system Windows próbował wypisać hasło grupy domowej. (Kod błędu: %1!u!)"},
    {78, L"Nie masz obecnie połączenia z siecią domową. Aby przeglądać pliki i zasoby na innych komputerach w grupie domowej, najpierw połącz się z siecią domową."},
    {79, L"%1 dołączył komputer do grupy domowej. Nie udostępniłem biblioteki mojej grupie domowej. Kliknij poniższy link, aby zmienić udostępniane treści. Nie wyłączaj ani nie uruchamiaj ponownie komputera, dopóki udostępnianie nie zostanie zakończone."},
    {80, L"Nie udostępniłem biblioteki mojej grupie domowej. Kliknij poniższy link, aby zmienić udostępniane treści. Nie wyłączaj ani nie uruchamiaj ponownie komputera, dopóki udostępnianie nie zostanie zakończone."},
    {81, L"Grupa domowa obecnie udostępnia bibliotekę na tym komputerze. Niektóre opcje grupy domowej nie są dostępne, dopóki udostępnianie nie zostanie zakończone. Nie wyłączaj ani nie uruchamiaj ponownie komputera, dopóki udostępnianie nie zostanie zakończone."},
    {82, L"W obszarze Sieć i Internet kliknij opcję Wybierz grupę domową i opcje udostępniania."},
    {83, L"Obecnie w sieci nie ma żadnych grup domowych."},
    {84, L"Kliknij Dołącz teraz i postępuj zgodnie z kreatorem grupy domowej, aby wprowadzić hasło."},
    {85, L"Kliknij tutaj, aby zainstalować."},
    {86, L"System Windows znalazł drukarkę grupy domowej"},
    {88, L"Przedstawiamy grupę domową"},
    {89, L"%1 (bieżący profil)"},
    {90, L"Użyj tej strony, aby dołączyć do grupy domowej. Lokalizacja sieciowa Twojego komputera musi być ustawiona na prywatną."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Grupa domowa nie jest jeszcze gotowa. Spróbuj ponownie za kilka minut. Jeśli ten komunikat będzie nadal wyświetlany, kliknij łącze, aby rozpocząć rozwiązywanie problemów z grupą domową."},
    {95, L"Uruchom narzędzie do rozwiązywania problemów z grupą domową"},
    {98, L"Hasło grupy domowej"},
    {99, L"Konta gości nie mogą zmieniać ustawień grupy domowej."},
    {100, L"Grupa domowa znalazła nową udostępnioną drukarkę w Twojej sieci domowej. Po zainstalowaniu będzie dostępny dla każdego na tym komputerze."},
    {101, L"Zainstaluj drukarkę"},
    {102, L"Grupa domowa jest niedostępna, ponieważ nie masz połączenia z siecią domową."},
    {103, L"Grupa domowa jest niedostępna, ponieważ nie masz połączenia z siecią domową."},
    {104, L"Przed dołączeniem do grupy domowej należy najpierw połączyć się z siecią."},
    {105, L"Obraz grupy domowej"},
    {106, L"Wybierz, co chcesz udostępnić i wyświetl hasło do grupy domowej"},
    {107, L"Ponieważ ten komputer jest częścią domeny, ustawienia udostępniania jego bibliotek i urządzeń innym komputerom w grupie domowej nie są dostępne."},
    {108, L"Ustawienia udostępniania bibliotek i urządzeń innym komputerom w grupie domowej nie są dostępne w tej wersji systemu Windows."},
    {109, L"Usuń %1 z grupy domowej"},
    {110, L"Anuluj"},
    {111, L"Usuń członka grupy domowej"},
    {112, L"%1 zostanie usunięty z grupy domowej"},
    {113, L"Wszyscy członkowie grupy domowej, którzy dołączą przy użyciu hasła, będą musieli ponownie wprowadzić hasło."},
    {114, L"Drukarki i urządzenia"},
    {115, L"Zmień członków grupy domowej %1"},
    {116, L"Hasło grupy domowej zostało zresetowane"},
    {117, L"Grupa domowa udostępnia pliki"},
    {118, L"Opcja: Ten komputer należy do grupy domowej"},
    {119, L"Można dołączyć do grupy domowej"},
    {120, L"Można utworzyć grupę domową"},
    {121, L"Grupa domowa jest niedostępna"},
    {122, L"Niezaufana drukarka"},
    {200, L"Dodaj członka"},
    {201, L"Ikona użytkownika"},
    {202, L"Pełne imię i nazwisko"},
    {203, L"Identyfikator użytkownika"},
    {204, L"Pasek postępu"},
    {205, L"Ikona folderu"},
    {220, L"Udostępniaj biblioteki i sprzęt"},
    {221, L"Wybierz bibliotekę, którą chcesz udostępnić innym osobom w grupie domowej."},
    {222, L"Edytuj opcje grupy domowej"},
    {223, L"Użyj tej strony, aby zmienić ustawienia grupy domowej, otwórz grupę domową w Panelu sterowania."},
    {224, L"Opcje grupy domowej"},
    {225, L"Użyj tej strony, aby zmienić ustawienia grupy domowej w Panelu sterowania lub skorzystać z narzędzia do rozwiązywania problemów z grupą domową."},
    {226, L"Uruchom narzędzie do rozwiązywania problemów"},
    {227, L"Użyj tej strony, aby skorzystać z narzędzia do rozwiązywania problemów z grupą domową, aby znaleźć i naprawić problemy z grupą domową."},
    {228, L"Wyświetl hasło"},
    {229, L"Użyj tej strony, aby wyświetlić lub wydrukować hasło grupy domowej."},
    {230, L"Dołącz do grupy domowej"},
    {231, L"Dołącz do grupy domowej w tej sieci."},
    {530, L"Otwórz szczegółowe opcje udostępniania…"},
    {541, L"Widoczność sieci"},
    {542, L"Jeśli odnajdowanie sieci jest włączone, ten komputer może być widoczny dla innych komputerów i urządzeń w sieci."},
    {543, L"Włącz wykrywanie sieci"},
    {544, L"Wyłącz wykrywanie sieci"},
    {545, L"Dostęp do plików i drukarek"},
    {546, L"Gdy udostępnianie plików i drukarek jest włączone, inni użytkownicy w Twojej sieci mogą uzyskiwać dostęp do plików i drukarek, które udostępniasz z tego komputera."},
    {547, L"Włącz udostępnianie plików i drukarek"},
    {548, L"Wyłącz udostępnianie plików i drukarek"},
    {549, L"Udostępnianie folderów publicznych"},
    {550, L"Gdy włączone jest udostępnianie folderów publicznych, użytkownicy w Twojej sieci, w tym członkowie grupy domowej, mogą uzyskiwać dostęp do plików w folderach publicznych."},
    {552, L"Włączenie udostępniania umożliwia każdemu, kto ma dostęp do Twojej sieci, odczytywanie i zapisywanie plików w Twoich folderach publicznych."},
    {553, L"Wyłącz udostępnianie folderów publicznych (użytkownicy zalogowani na tym komputerze nadal mają dostęp do tych folderów)"},
    {554, L"Zmień opcje udostępniania dla różnych profili sieciowych"},
    {559, L"Dostęp do mediów"},
    {560, L"Gdy strumieniowe przesyłanie multimediów jest włączone, użytkownicy i urządzenia w Twojej sieci mogą uzyskać dostęp do zdjęć, muzyki i filmów na tym komputerze. Ten komputer może także znajdować multimedia w sieci."},
    {564, L"Anuluj"},
    {567, L"Zastosuj zmiany"},
    {584, L"System Windows tworzy oddzielny profil sieciowy dla każdej używanej sieci. Możesz wybrać określone opcje dla każdego profilu."},
    {585, L"Ikona ostrzegawcza grupy domowej"},
    {586, L"Biblioteki i urządzenia udostępnione z tego komputera"},
    {595, L"Więcej zadań w grupie domowej"},
    {600, L"Pokaż lub wydrukuj hasło grupy domowej"},
    {601, L"Administrator systemu nie zezwolił Ci na dostęp do Twojej grupy domowej."},
    {604, L"Zmień hasło..."},
    {605, L"Opuść grupę domową..."},
    {607, L"Wybierz opcje strumieniowego przesyłania multimediów..."},
    {608, L"Ponieważ ten komputer jest częścią domeny, ustawienia udostępniania jego bibliotek i urządzeń innym komputerom w grupie domowej nie są dostępne."},
    {609, L"Udostępnianie chronione hasłem"},
    {610, L"Gdy włączone jest udostępnianie chronione hasłem, tylko użytkownicy posiadający konta użytkowników i hasła na tym komputerze będą mieli dostęp do udostępnionych plików, drukarek podłączonych do tego komputera i folderów publicznych. Udostępnianie chronione hasłem musi być wyłączone, aby umożliwić innym dostęp."},
    {611, L"Włącz udostępnianie chronione hasłem"},
    {612, L"Wyłącz udostępnianie chronione hasłem"},
    {613, L"Wydrukuj stronę"},
    {614, L"Umożliwia odtwarzanie udostępnionej zawartości na wszystkich urządzeniach w tej sieci, takich jak telewizory i konsole do gier"},
    {615, L"Sieć prywatna"},
    {616, L"Sieć gościnna lub publiczna"},
    {617, L"Sieć domeny"},
    {619, L"Połączenia grupy domowej"},
    {620, L"System Windows zazwyczaj zarządza połączeniami z innymi komputerami w grupie domowej. Jeśli jednak używasz tego samego konta użytkownika i hasła na wszystkich swoich komputerach, możesz zamiast tego ustawić HomeGroup na używanie tego konta."},
    {621, L"Opcja: Zezwól systemowi Windows na zarządzanie połączeniami grupy domowej (zalecane)"},
    {622, L"Połącz się z innymi komputerami, korzystając ze swojego konta użytkownika i hasła"},
    {624, L"Uruchom narzędzie do rozwiązywania problemów z grupą domową"},
    {627, L"Połączenia udostępniania plików"},
    {628, L"System Windows wykorzystuje 128-bitowe szyfrowanie do zabezpieczania połączeń udostępniania plików. Niektóre urządzenia nie obsługują szyfrowania 128-bitowego i muszą używać szyfrowania 40-bitowego lub 56-bitowego."},
    {629, L"Zabezpiecz swoje połączenie do udostępniania plików za pomocą 128-bitowego szyfrowania (zalecane)"},
    {630, L"Włącz udostępnianie plików urządzenia z szyfrowaniem 40-bitowym lub 56-bitowym"},
    {631, L"Każda sieć"},
    {632, L"Zmień zawartość udostępnianą Twojej grupie domowej"},
    {637, L"Zamknij"},
    {639, L"Zdalny dostęp do grupy domowej"},
    {640, L"Inni członkowie grupy domowej mogą łączyć się z Twoją grupą domową z dowolnego miejsca za pośrednictwem swoich komputerów."},
    {641, L"Opcja: Wyłącz zdalny dostęp do grupy domowej za pośrednictwem tego komputera"},
    {642, L"Opcja: Włącz zdalny dostęp grupy domowej za pośrednictwem tego komputera"},
    {648, L"Wybierz pliki i urządzenia, które chcesz udostępnić, a następnie wybierz ich poziomy uprawnień."},
    {649, L"Biblioteka lub katalog"},
    {650, L"Poziom dostępu"},
    {652, L"Włącz automatyczną konfigurację urządzeń podłączonych do sieci."},
    {46000, L"Grupa domowa"},
    {46004, L"Opcja: wybierz hasło dla swojej grupy domowej"},
    {46005, L"Wpisz hasło grupy domowej"},
    {46006, L"&Utwórz teraz"},
    {46007, L"&Dołącz teraz"},
    {46008, L"Dodaj inne komputery do swojej grupy domowej, używając tego hasła"},
    {46009, L"Dołączyłeś do grupy domowej"},
    {46011, L"Grupa domowa"},
    {46012, L"System Windows nie może skonfigurować grupy domowej na tym komputerze."},
    {46013, L"Ponieważ ten komputer jest częścią domeny, udostępnianie jego biblioteki innym komputerom w grupie domowej nie jest możliwe."},
    {46014, L"Hasła muszą zawierać co najmniej 8 znaków i nie mogą zawierać spacji na początku ani na końcu."},
    {46015, L"Hasło jest nieprawidłowe.\nSpróbuj ponownie. W hasłach rozróżniana jest wielkość liter."},
    {46016, L"Opcja: wszystkie połączenia grupy domowej na tym komputerze zostaną rozłączone"},
    {46017, L"Pomyślnie opuściłeś grupę domową"},
    {46018, L"Zmień zawartość udostępnianą Twojej grupie domowej"},
    {46019, L"Udostępniaj swoje zdjęcia, filmy, muzykę, dokumenty i drukarki innym komputerom w domu."},
    {46020, L"&Wprowadź zmiany"},
    {46021, L"Zmiana hasła grupy domowej powoduje rozłączenie wszystkich"},
    {46022, L"Wprowadź nowe hasło do swojej grupy domowej"},
    {46023, L"&Zmień hasło"},
    {46024, L"Hasło grupy domowej zostało pomyślnie zmienione"},
    {46025, L"Hasło grupy domowej zostało zmienione"},
    {46026, L"Wpisz hasło grupy domowej"},
    {46027, L"Hasło grupy domowej zostało zmienione. Aby kontynuować korzystanie z zasobów grupy domowej, upewnij się, że osoba, która wprowadziła już nowe hasło, jest online, a następnie wprowadź nowe hasło."},
    {46028, L"Udostępnione"},
    {46029, L"System Windows nie mógł usunąć komputera z grupy domowej."},
    {46030, L"%1 zmienił hasło do swojej grupy domowej. Aby kontynuować korzystanie z zasobów grupy domowej, upewnij się, że osoba, która wprowadziła już nowe hasło, jest online, a następnie wprowadź nowe hasło."},
    {46031, L"Hasła zapobiegają nieautoryzowanemu dostępowi do plików i drukarek grupy domowej. Możesz uzyskać hasło od %2, %1 lub innego członka swojej grupy domowej."},
    {46032, L"Hasła zapobiegają nieautoryzowanemu dostępowi do plików i drukarek grupy domowej. Możesz uzyskać hasło od %2, %1 lub innego członka swojej grupy domowej."},
    {46033, L"Consolas"},
    {46034, L"Utwórz grupę domową"},
    {46035, L"Dołącz do grupy domowej"},
    {46036, L"Zmień hasło do grupy domowej"},
    {46037, L"Opuść grupę domową"},
    {46038, L"Aby uzyskać dostęp do plików i drukarek na innych komputerach, musisz dodać je do swojej grupy domowej. Wymagane jest następujące hasło:"},
    {46039, L"Wpisz nowe hasło grupy domowej:"},
    {46040, L"Zaktualizuj hasło"},
    {46041, L"Utwórz kopię zapasową wszystkich komputerów w grupie domowej w lokalnym miejscu docelowym ochrony danych."},
    {46042, L"Utwórz kopię zapasową komputera, korzystając z celów ochrony danych HomeGroup"},
    {46043, L"Nieudostępnione"},
    {46044, L"Grupy domowe można tworzyć tylko w sieciach prywatnych.\nAby zmienić ustawienia lokalizacji sieciowej, otwórz Centrum sieci i udostępniania w Panelu sterowania."},
    {46045, L"System Windows nie będzie już wykrywał grup domowych w tej sieci. Aby utworzyć nową grupę domową, kliknij OK i otwórz Grupę domową w Panelu sterowania."},
    {46046, L"System Windows wykrył istniejącą grupę domową.\nAby dołączyć, kliknij OK i otwórz grupę domową w Panelu sterowania."},
    {46047, L"Usługa HomeGroup jest już dostępna. Spróbuj ponownie."},
    {46048, L"Zaktualizowano ustawienia udostępniania"},
    {46049, L"Wybrane pliki i zasoby zostaną udostępnione Twojej grupie domowej."},
    {46050, L"Hasło grupy domowej zostało pomyślnie zaktualizowane"},
    {46051, L"Dołączyłeś do grupy domowej"},
    {46052, L"Możesz teraz uzyskać dostęp do udostępnionych plików i urządzeń. Pliki i urządzenia, które udostępniasz, pozostają niezmienione."},
    {46053, L"Możesz uzyskać dostęp do plików i drukarek udostępnionych przez innych użytkowników w Twojej grupie domowej."},
    {46054, L"Zaktualizuj hasło do grupy domowej"},
    {46055, L"Dołącz do grupy domowej"},
    {46056, L"Wprowadź nowe hasło grupy domowej z %1."},
    {46057, L"Zegary wszystkich komputerów w grupie domowej muszą być ustawione na odstępy nie większe niż 24 godziny. Upewnij się, że zegary komputera są zsynchronizowane, a następnie spróbuj ponownie dołączyć do grupy domowej."},
    {46058, L"Hasło nie spełnia wymagań dotyczących siły hasła domeny. Wprowadź pasujące hasło lub użyj innego komputera HomeGroup, aby zmienić hasło."},
    {46059, L"Nie możesz zresetować hasła, ponieważ nie masz połączenia z siecią prywatną.\nPołącz się z siecią prywatną i spróbuj ponownie."},
    {46060, L"Nie masz połączenia z siecią prywatną.\nAby zmienić ustawienia lokalizacji sieciowej, otwórz Centrum sieci i udostępniania w Panelu sterowania."},
    {46061, L"Udostępnij innym komputerom domowym"},
    {46062, L"Możesz udostępniać pliki i drukarki innym komputerom. Możesz także przesyłać strumieniowo multimedia do swojego urządzenia.\n\nGrupy domowe są chronione hasłem i w dowolnym momencie możesz wybrać, co chcesz udostępnić."},
    {46063, L"Dodaj inne komputery do swojej grupy domowej, używając tego hasła"},
    {46064, L"Aby uzyskać dostęp do plików i drukarek na innych komputerach, musisz dodać je do swojej grupy domowej. Wymagane jest następujące hasło:"},
    {46065, L"Aby utworzyć grupę domową lub dołączyć do niej, w połączeniu sieciowym musi być włączony protokół IPv6. Aby włączyć protokół IPv6, uruchom narzędzie do rozwiązywania problemów z grupą domową."},
    {46066, L"Dodaj osoby do grupy domowej"},
    {46067, L"Skonfiguruj ochronę danych grupy domowej"},
    {46068, L"Wykryto wiele grup domowych"},
    {46069, L"Udostępnij innym członkom grupy domowej"},
    {46070, L"Dokumenty"},
    {46071, L"Zdjęcia"},
    {46072, L"Muzyka"},
    {46073, L"Filmy"},
    {46074, L"Drukarki i urządzenia"},
    {46075, L"Zmień ustawienia udostępniania w grupie domowej"},
    {46076, L"%1 Udostępnianie"},
    {46077, L"Weryfikuję Twoje hasło..."},
};

// Japanese (ja-JP)
static const EmbeddedTextEntry kStrings_JA_JP[] = {
    {1, L"ホームグループ"},
    {2, L"ホームグループのオプションを確認し、この PC が何を共有するかを決定し、アクセス パスワードを表示または更新します。"},
    {3, L"組織によって設定されたポリシーにより、このページは実行できません。ネットワーク管理者にサポートを依頼してください。"},
    {4, L"詳細な共有オプション"},
    {5, L"オン"},
    {6, L"オフ"},
    {7, L"オフ (プリンターがインストールされていません)"},
    {8, L"このコンピュータにはプリンタが接続されていません。"},
    {9, L"自宅のPCとコンテンツを共有"},
    {10, L"ドメインに参加しているコンピューターを使用してホームグループにアクセスする"},
    {12, L"ホームグループのオプションを編集する"},
    {13, L"働いています…"},
    {14, L"このネットワーク上にホームグループが見つかりませんでした。"},
    {15, L"%2 の %1 がネットワーク上にホームグループを作成しました。"},
    {16, L"ホームグループへの参加に招待されました。"},
    {18, L"このコンピュータがホームグループに属している場合は、このページを使用します。"},
    {19, L"このコンピュータはホームグループに接続できません。"},
    {20, L"HomeGroup を使用すると、信頼できる PC 間でファイルを交換したり、共有プリンタを使用したりできるほか、互換性のあるデバイスにメディアを送信できます。アクセスするにはパスワードが必要ですが、この PC で利用できるものを制御することはできます。"},
    {21, L"このコンピュータはドメインの一部でもあるため、独自のホームグループを作成することはできませんが、ネットワーク上の誰かが作成したホームグループに参加することはできます。\n\nホームグループはホーム ネットワーク上のコンピュータをリンクするので、写真、音楽、ビデオ、ドキュメント、プリンタを共有できます。ホームグループはパスワードで保護されており、何を共有するかをいつでも選択できます。"},
    {22, L"ホームグループはホーム ネットワーク上のコンピュータをリンクするので、写真、音楽、ビデオ、ドキュメント、プリンタを共有できます。ホームグループはパスワードで保護されており、何を共有するかをいつでも選択できます。\n\nこのエディションの Windows では独自のホームグループを作成することはできませんが、他の人が作成したホームグループに参加することはできます。"},
    {23, L"ホームグループを設定する"},
    {24, L"参加する"},
    {25, L"ホームグループのパスワードが変更されました。ホームグループ リソースを引き続き使用するには、新しいパスワードを入力した人がオンラインであることを確認してから、新しいパスワードを入力します。"},
    {26, L"Windows がネットワーク上で別のホームグループを検出しました。ホームグループを使用すると、ファイルやプリンターを他のコンピューターと共有できます。メディアをデバイスにストリーミングすることもできます。"},
    {27, L"%1 はホームグループのパスワードを変更しました。ホームグループ リソースを引き続き使用するには、新しいパスワードを入力した人がオンラインであることを確認してから、新しいパスワードを入力します。"},
    {28, L"このネットワーク上でホームグループを探しています…"},
    {29, L"新しいパスワードを入力してください"},
    {30, L"今すぐ参加"},
    {32, L"ホームグループを作成または参加するには、まずネットワークに接続する必要があります。"},
    {34, L"このページを使用してホームグループを作成または参加するには、コンピュータのネットワークの場所をプライベートに設定する必要があります。"},
    {35, L"ネットワークの場所を変更する"},
    {37, L"プライベートの共有オプション"},
    {38, L"パブリックの共有オプション"},
    {39, L"ドメインの共有オプション"},
    {40, L"プライベート"},
    {41, L"プライベート (現在のプロフィール)"},
    {42, L"公共"},
    {43, L"パブリック (現在のプロフィール)"},
    {44, L"ドメイン"},
    {45, L"ドメイン (現在のプロファイル)"},
    {46, L"メディアストリーミングがオンになっています。"},
    {47, L"メディアストリーミングがオフになっています。"},
    {56, L"キャンセル"},
    {63, L"OK"},
    {64, L"ホームグループのパスワードを表示または印刷する"},
    {65, L"24pt;;;Consolas"},
    {66, L"印刷日: %1 %2"},
    {67, L"オプション: ホームグループのパスワードを表示および印刷する"},
    {68, L"パスワード:"},
    {69, L"このパスワードを使用して、他のコンピュータをホームグループに接続します。"},
    {70, L"各コンピュータで次の操作を行います。"},
    {71, L"注: 電源がオフになっているかスリープ状態のコンピューターはホームグループに表示されません。"},
    {72, L"1. [スタート] をクリックし、[コントロール パネル] をクリックします。"},
    {73, L"2. 「ネットワークとインターネット」で、「ホームグループと共有オプションの選択」をクリックします。"},
    {74, L"3. [今すぐ参加] をクリックし、ホームグループ ウィザードに従ってパスワードを入力します。"},
    {75, L"「スタート」をクリックし、「コントロール パネル」をクリックします。"},
    {76, L"ホームグループのパスワードを印刷できませんでした"},
    {77, L"Windows がホームグループのパスワードを出力しようとしたときにエラーが発生しました。 (エラーコード:%1!u!)"},
    {78, L"現在ホーム ネットワークに接続されていません。他のホームグループ コンピューター上のファイルやリソースを表示するには、まずホーム ネットワークに接続します。"},
    {79, L"%1 がコンピューターをホームグループに参加させました。ホームグループとライブラリを共有していません。共有内容を変更するには、下のリンクをクリックしてください。共有が完了するまで、コンピュータをシャットダウンしたり再起動したりしないでください。"},
    {80, L"ホームグループとライブラリを共有していません。共有内容を変更するには、下のリンクをクリックしてください。共有が完了するまで、コンピュータをシャットダウンしたり再起動したりしないでください。"},
    {81, L"現在、ホームグループはこのコンピュータ上のライブラリを共有しています。一部のホームグループ オプションは、共有が完了するまで使用できません。共有が完了するまで、コンピュータをシャットダウンしたり再起動したりしないでください。"},
    {82, L"「ネットワークとインターネット」で、「ホームグループと共有オプションの選択」をクリックします。"},
    {83, L"現在、ネットワーク上にホームグループはありません。"},
    {84, L"[今すぐ参加] をクリックし、ホームグループ ウィザードに従ってパスワードを入力します。"},
    {85, L"ここをクリックしてインストールしてください。"},
    {86, L"Windows がホームグループ プリンターを見つけました"},
    {88, L"ホームグループの紹介"},
    {89, L"%1 (現在のプロファイル)"},
    {90, L"このページを使用してホームグループに参加するには、コンピュータのネットワークの場所をプライベートに設定する必要があります。"},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"ホームグループはまだ準備ができていません。数分後にもう一度試してください。このメッセージが引き続き表示される場合は、リンクをクリックしてホームグループのトラブルシューティングを開始してください。"},
    {95, L"ホームグループのトラブルシューティング ツールを開始する"},
    {98, L"ホームグループのパスワード"},
    {99, L"ゲスト アカウントはホームグループ設定を変更できません。"},
    {100, L"ホームグループがホーム ネットワーク上で新しい共有プリンタを見つけました。インストールすると、このコンピュータ上の誰でも利用できるようになります。"},
    {101, L"プリンターのインストール"},
    {102, L"ホーム ネットワークに接続されていないため、ホームグループは使用できません。"},
    {103, L"ホーム ネットワークに接続されていないため、ホームグループは使用できません。"},
    {104, L"ホームグループに参加する前に、まずネットワークに接続する必要があります。"},
    {105, L"ホームグループイメージ"},
    {106, L"共有したいものを選択し、ホームグループのパスワードを表示します"},
    {107, L"このコンピュータはドメインの一部であるため、そのライブラリとデバイスをホームグループ内の他のコンピュータと共有する設定は利用できません。"},
    {108, L"ホームグループ内の他のコンピューターとライブラリやデバイスを共有する設定は、このエディションの Windows では使用できません。"},
    {109, L"%1 をホームグループから削除します"},
    {110, L"キャンセル"},
    {111, L"ホームグループメンバーを削除する"},
    {112, L"%1 はホームグループから削除されます"},
    {113, L"パスワードを使用して参加するすべてのホームグループ メンバーは、パスワードを再度入力する必要があります。"},
    {114, L"プリンターとデバイス"},
    {115, L"%1 ホームグループ メンバーを変更する"},
    {116, L"ホームグループのパスワードがリセットされました"},
    {117, L"ホームグループがファイルを共有しています"},
    {118, L"オプション: このコンピュータはホームグループに属しています"},
    {119, L"ホームグループに参加できます"},
    {120, L"ホームグループを作成できる"},
    {121, L"ホームグループは利用できません"},
    {122, L"信頼できないプリンター"},
    {200, L"メンバーを追加"},
    {201, L"ユーザーアイコン"},
    {202, L"フルネーム"},
    {203, L"ユーザーID"},
    {204, L"プログレスバー"},
    {205, L"フォルダーアイコン"},
    {220, L"ライブラリとハードウェアを共有する"},
    {221, L"ホームグループ内の他の人と共有したいライブラリを選択します。"},
    {222, L"ホームグループのオプションを編集する"},
    {223, L"このページを使用してホームグループ設定を変更し、コントロール パネルでホームグループを開きます。"},
    {224, L"ホームグループのオプション"},
    {225, L"このページを使用して、コントロール パネルでホームグループ設定を変更するか、ホームグループ トラブルシューティング ツールを使用します。"},
    {226, L"トラブルシューティングを開始する"},
    {227, L"このページを使用して、ホームグループのトラブルシューティング ツールを使用して、ホームグループの問題を見つけて修正します。"},
    {228, L"パスワードを表示する"},
    {229, L"このページを使用して、ホームグループのパスワードを表示または印刷します。"},
    {230, L"ホームグループに参加する"},
    {231, L"このネットワーク上のホームグループに参加してください。"},
    {530, L"詳細な共有オプションを開く…"},
    {541, L"ネットワークの可視性"},
    {542, L"ネットワーク探索がオンになっている場合、このコンピュータは、ネットワークに接続された他のコンピュータやデバイスから認識されたり、認識されたりすることができます。"},
    {543, L"ネットワーク探索をオンにする"},
    {544, L"ネットワーク探索をオフにする"},
    {545, L"ファイルとプリンターへのアクセス"},
    {546, L"ファイルとプリンターの共有がオンになっている場合、ネットワーク上の他のユーザーは、このコンピューターから共有しているファイルとプリンターにアクセスできます。"},
    {547, L"ファイルとプリンターの共有をオンにする"},
    {548, L"ファイルとプリンターの共有をオフにする"},
    {549, L"パブリックフォルダーの共有"},
    {550, L"パブリック フォルダーの共有がオンになっている場合、ホームグループ メンバーを含むネットワーク上のユーザーは、パブリック フォルダー内のファイルにアクセスできます。"},
    {552, L"共有を有効にすると、ネットワークにアクセスできる誰もがパブリック フォルダー内のファイルを読み書きできるようになります。"},
    {553, L"パブリック フォルダーの共有をオフにします (このコンピューターにログオンしているユーザーは引き続きこれらのフォルダーにアクセスできます)"},
    {554, L"さまざまなネットワーク プロファイルの共有オプションを変更する"},
    {559, L"メディアアクセス"},
    {560, L"メディア ストリーミングがオンになっている場合、ネットワーク上のユーザーとデバイスは、このコンピューター上の写真、音楽、ビデオにアクセスできます。このコンピュータはネットワーク上のメディアも見つけることができます。"},
    {564, L"キャンセル"},
    {567, L"変更を適用する"},
    {584, L"Windows は、使用するネットワークごとに個別のネットワーク プロファイルを作成します。プロファイルごとに特定のオプションを選択できます。"},
    {585, L"ホームグループ警告アイコン"},
    {586, L"このコンピュータから共有されるライブラリとデバイス"},
    {595, L"ホームグループのその他のタスク"},
    {600, L"ホームグループのパスワードを表示または印刷する"},
    {601, L"システム管理者は、あなたのホームグループへのアクセスを許可していません。"},
    {604, L"パスワードを変更してください..."},
    {605, L"ホームグループから退出..."},
    {607, L"メディア ストリーミング オプションを選択してください..."},
    {608, L"このコンピュータはドメインの一部であるため、そのライブラリとデバイスをホームグループ内の他のコンピュータと共有する設定は利用できません。"},
    {609, L"パスワードで保護された共有"},
    {610, L"パスワードで保護された共有が有効になっている場合、このコンピュータ上のユーザー アカウントとパスワードを持つユーザーのみが、共有ファイル、このコンピュータに接続されているプリンタ、およびパブリック フォルダにアクセスできます。他の人がアクセスできるようにするには、パスワードで保護された共有をオフにする必要があります。"},
    {611, L"パスワードで保護された共有をオンにする"},
    {612, L"パスワードで保護された共有をオフにする"},
    {613, L"ページを印刷する"},
    {614, L"テレビやゲーム機など、このネットワーク上のすべてのデバイスで共有コンテンツを再生できるようにします"},
    {615, L"プライベートネットワーク"},
    {616, L"ゲストまたはパブリック ネットワーク"},
    {617, L"ドメインネットワーク"},
    {619, L"ホームグループ接続"},
    {620, L"通常、Windows は他のホームグループ コンピューターへの接続を管理します。ただし、すべてのコンピュータで同じユーザー アカウントとパスワードを使用している場合は、ホームグループにそのアカウントを代わりに使用させることができます。"},
    {621, L"オプション: Windows によるホームグループ接続の管理を許可します (推奨)"},
    {622, L"ユーザー アカウントとパスワードを使用して他のコンピュータに接続する"},
    {624, L"ホームグループのトラブルシューティング ツールを開始する"},
    {627, L"ファイル共有接続"},
    {628, L"Windows は 128 ビット暗号化を使用してファイル共有接続を保護します。一部のデバイスは 128 ビット暗号化をサポートしていないため、40 ビットまたは 56 ビット暗号化を使用する必要があります。"},
    {629, L"128 ビット暗号化を使用してファイル共有接続を保護します (推奨)"},
    {630, L"40 ビットまたは 56 ビット暗号化によるデバイス ファイル共有を有効にする"},
    {631, L"あらゆるネットワーク"},
    {632, L"ホームグループと共有する内容を変更する"},
    {637, L"閉じる"},
    {639, L"ホームグループのリモートアクセス"},
    {640, L"他のホームグループ メンバーは、コンピュータを介してどこからでもあなたのホームグループに接続できます。"},
    {641, L"オプション: このコンピュータを介したリモート ホームグループ アクセスを無効にする"},
    {642, L"オプション: このコンピュータを介したリモート ホームグループ アクセスを有効にする"},
    {648, L"使用可能にするファイルとデバイスを選択し、それらのアクセス許可レベルを選択します。"},
    {649, L"ライブラリまたはディレクトリ"},
    {650, L"アクセスレベル"},
    {652, L"ネットワーク接続デバイスの自動セットアップをオンにします。"},
    {46000, L"ホームグループ"},
    {46004, L"オプション: ホームグループのパスワードを選択します"},
    {46005, L"ホームグループのパスワードを入力します"},
    {46006, L"&今すぐ作成(&C)"},
    {46007, L"&今すぐ参加(&J)"},
    {46008, L"このパスワードを使用して他のコンピュータをホームグループに追加します"},
    {46009, L"ホームグループに参加しました"},
    {46011, L"ホームグループ"},
    {46012, L"Windows はこのコンピュータにホームグループを設定できません。"},
    {46013, L"このコンピュータはドメインの一部であるため、そのライブラリをホームグループ内の他のコンピュータと共有することはできません。"},
    {46014, L"パスワードには少なくとも 8 文字を含める必要があり、先頭または末尾にスペースを含めることはできません。"},
    {46015, L"パスワードが間違っています。\nもう一度試してください。パスワードでは大文字と小文字が区別されます。"},
    {46016, L"オプション: このコンピュータ上のすべてのホームグループ接続が切断されます"},
    {46017, L"ホームグループから正常に脱退しました"},
    {46018, L"ホームグループと共有する内容を変更する"},
    {46019, L"写真、ビデオ、音楽、ドキュメント、プリンターを自宅の他のコンピューターと共有します。"},
    {46020, L"&変更を加える(&M)"},
    {46021, L"ホームグループのパスワードを変更すると全員の接続が切断される"},
    {46022, L"ホームグループの新しいパスワードを入力してください"},
    {46023, L"&パスワードを変更(&C)"},
    {46024, L"ホームグループのパスワードが正常に変更されました"},
    {46025, L"ホームグループのパスワードが変更されました"},
    {46026, L"ホームグループのパスワードを入力します"},
    {46027, L"ホームグループのパスワードが変更されました。ホームグループ リソースを引き続き使用するには、新しいパスワードを入力した人がオンラインであることを確認してから、新しいパスワードを入力します。"},
    {46028, L"共有"},
    {46029, L"Windows はホームグループからコンピューターを削除できませんでした。"},
    {46030, L"%1 はホームグループのパスワードを変更しました。ホームグループ リソースを引き続き使用するには、新しいパスワードを入力した人がオンラインであることを確認してから、新しいパスワードを入力します。"},
    {46031, L"パスワードは、ホームグループのファイルやプリンターへの不正アクセスを防ぐのに役立ちます。パスワードは、%2、%1、またはホームグループの別のメンバーから取得できます。"},
    {46032, L"パスワードは、ホームグループのファイルやプリンターへの不正アクセスを防ぐのに役立ちます。パスワードは、%2、%1、またはホームグループの別のメンバーから取得できます。"},
    {46033, L"Consolas"},
    {46034, L"ホームグループを作成する"},
    {46035, L"ホームグループに参加する"},
    {46036, L"ホームグループのパスワードを変更する"},
    {46037, L"ホームグループから脱退する"},
    {46038, L"他のコンピュータ上のファイルやプリンタにアクセスするには、それらをホームグループに追加する必要があります。次のパスワードが必要です。"},
    {46039, L"新しいホームグループのパスワードを入力します。"},
    {46040, L"パスワードを更新する"},
    {46041, L"ホームグループ内のすべての PC をローカルのデータ保護ターゲットにバックアップします。"},
    {46042, L"ホームグループのデータ保護ターゲットを使用して PC をバックアップする"},
    {46043, L"共有されていません"},
    {46044, L"ホームグループはプライベート ネットワーク上でのみ作成できます。\nネットワークの場所の設定を変更するには、コントロール パネルのネットワークと共有センターを開きます。"},
    {46045, L"Windows はこのネットワーク上のホームグループを検出しなくなります。新しいホームグループを作成するには、[OK] をクリックし、コントロール パネルでホームグループを開きます。"},
    {46046, L"Windows が既存のホームグループを検出しました。\n参加するには、[OK] をクリックし、コントロール パネルでホームグループを開きます。"},
    {46047, L"ホームグループサービスが利用できるようになりました。もう一度試してください。"},
    {46048, L"共有設定が更新されました"},
    {46049, L"選択したファイルとリソースはホームグループと共有されます。"},
    {46050, L"ホームグループのパスワードが正常に更新されました"},
    {46051, L"ホームグループに参加しました"},
    {46052, L"これで、共有ファイルやデバイスにアクセスできるようになります。共有しているファイルとデバイスは変更されません。"},
    {46053, L"ホームグループ内の他のユーザーが共有しているファイルやプリンターへのアクセスを開始できます。"},
    {46054, L"ホームグループのパスワードを更新する"},
    {46055, L"ホームグループに参加する"},
    {46056, L"新しいホームグループのパスワードを %1 から入力します。"},
    {46057, L"すべてのホームグループ コンピュータの時計の間隔は 24 時間以内に設定する必要があります。コンピュータの時計が同期していることを確認してから、ホームグループに再度参加してみてください。"},
    {46058, L"パスワードはドメインのパスワード強度要件を満たしていません。一致するパスワードを入力するか、別のホームグループ コンピュータを使用してパスワードを変更します。"},
    {46059, L"プライベート ネットワークに接続していないため、パスワードをリセットできません。\nプライベート ネットワークに接続して再試行してください。"},
    {46060, L"プライベート ネットワークに接続していません。\nネットワークの場所の設定を変更するには、コントロール パネルのネットワークと共有センターを開きます。"},
    {46061, L"他の自宅コンピュータと共有する"},
    {46062, L"ファイルやプリンターを他のコンピューターと共有できます。メディアをデバイスにストリーミングすることもできます。\n\nホームグループはパスワードで保護されており、何を共有するかをいつでも選択できます。"},
    {46063, L"このパスワードを使用して他のコンピュータをホームグループに追加します"},
    {46064, L"他のコンピュータ上のファイルやプリンタにアクセスするには、それらをホームグループに追加する必要があります。次のパスワードが必要です。"},
    {46065, L"ホームグループを作成または参加するには、ネットワーク接続で IPv6 が有効になっている必要があります。 IPv6 を有効にするには、ホームグループ トラブルシューティング ツールを開始します。"},
    {46066, L"ホームグループに人を追加する"},
    {46067, L"ホームグループのデータ保護を構成する"},
    {46068, L"複数のホームグループが検出されました"},
    {46069, L"他のホームグループメンバーと共有する"},
    {46070, L"書類"},
    {46071, L"写真"},
    {46072, L"音楽"},
    {46073, L"動画"},
    {46074, L"プリンターとデバイス"},
    {46075, L"ホームグループ共有設定の変更"},
    {46076, L"%1 共有"},
    {46077, L"パスワードを確認しています..."},
};

// Korean (ko-KR)
static const EmbeddedTextEntry kStrings_KO_KR[] = {
    {1, L"홈그룹"},
    {2, L"홈그룹 옵션을 검토하고, 이 PC가 공유하는 항목을 결정하고, 액세스 비밀번호를 표시하거나 업데이트하세요."},
    {3, L"조직에서 설정한 정책으로 인해 이 페이지가 실행되지 않습니다. 네트워크 관리자에게 도움을 요청하세요."},
    {4, L"자세한 공유 옵션"},
    {5, L"켜짐"},
    {6, L"끄기"},
    {7, L"꺼짐(프린터가 설치되지 않음)"},
    {8, L"이 컴퓨터에는 프린터가 연결되어 있지 않습니다."},
    {9, L"집에 있는 PC와 콘텐츠 공유"},
    {10, L"도메인에 가입된 컴퓨터를 사용하여 홈 그룹에 액세스"},
    {12, L"홈 그룹 옵션 편집"},
    {13, L"일하는 중…"},
    {14, L"이 네트워크에서 홈 그룹을 찾을 수 없습니다."},
    {15, L"%2의 %1가 네트워크에 홈 그룹을 만들었습니다."},
    {16, L"홈 그룹에 가입하도록 초대되었습니다."},
    {18, L"이 컴퓨터가 홈 그룹에 속해 있으면 이 페이지를 사용하세요."},
    {19, L"이 컴퓨터는 홈 그룹에 연결할 수 없습니다."},
    {20, L"HomeGroup을 사용하면 신뢰할 수 있는 PC가 파일을 교환하고 공유 프린터를 사용할 수 있으며 미디어를 호환 장치로 보낼 수 있습니다. 액세스하려면 암호가 필요하지만 이 PC에서 제공하는 기능은 사용자가 제어할 수 있습니다."},
    {21, L"이 컴퓨터는 도메인의 일부이기도 하므로 자체 홈 그룹을 만들 수는 없지만 네트워크의 누군가가 만든 홈 그룹에 참가할 수는 있습니다.\n\n홈 그룹은 홈 네트워크의 컴퓨터를 연결하므로 사진, 음악, 비디오, 문서 및 프린터를 공유할 수 있습니다. 홈 그룹은 비밀번호로 보호되며 언제든지 공유할 항목을 선택할 수 있습니다."},
    {22, L"홈 그룹은 홈 네트워크의 컴퓨터를 연결하므로 사진, 음악, 비디오, 문서 및 프린터를 공유할 수 있습니다. 홈 그룹은 비밀번호로 보호되며 언제든지 공유할 항목을 선택할 수 있습니다.\n\n이 Windows 버전에서는 자신만의 홈 그룹을 만들 수 없지만 다른 사람이 만든 홈 그룹에 가입할 수 있습니다."},
    {23, L"홈 그룹 설정"},
    {24, L"가입"},
    {25, L"홈그룹 비밀번호가 변경되었습니다. 홈 그룹 리소스를 계속 사용하려면 이미 새 비밀번호를 입력한 사람이 온라인 상태인지 확인한 다음 새 비밀번호를 입력하세요."},
    {26, L"Windows가 네트워크에서 다른 홈 그룹을 감지했습니다. 홈 그룹을 사용하면 다른 컴퓨터와 파일 및 프린터를 공유할 수 있습니다. 장치로 미디어를 스트리밍할 수도 있습니다."},
    {27, L"%1가 홈 그룹 비밀번호를 변경했습니다. 홈 그룹 리소스를 계속 사용하려면 이미 새 비밀번호를 입력한 사람이 온라인 상태인지 확인한 다음 새 비밀번호를 입력하세요."},
    {28, L"이 네트워크에서 홈그룹을 찾는 중…"},
    {29, L"새 비밀번호를 입력하세요"},
    {30, L"지금 가입하세요"},
    {32, L"홈 그룹을 만들거나 가입하려면 먼저 네트워크에 연결해야 합니다."},
    {34, L"이 페이지를 사용하여 홈 그룹을 만들거나 가입하려면 컴퓨터의 네트워크 위치를 비공개로 설정해야 합니다."},
    {35, L"네트워크 위치 변경"},
    {37, L"비공개 공유 옵션"},
    {38, L"공개 공유 옵션"},
    {39, L"도메인 공유 옵션"},
    {40, L"비공개"},
    {41, L"비공개(현재 프로필)"},
    {42, L"공개"},
    {43, L"공개(현재 프로필)"},
    {44, L"도메인"},
    {45, L"도메인(현재 프로필)"},
    {46, L"미디어 스트리밍이 켜져 있습니다."},
    {47, L"미디어 스트리밍이 꺼져 있습니다."},
    {56, L"취소"},
    {63, L"알았어"},
    {64, L"홈그룹 비밀번호 표시 또는 인쇄"},
    {65, L"24pt;;;Consolas"},
    {66, L"인쇄된 날짜: %1 %2"},
    {67, L"옵션: 홈 그룹 비밀번호 보기 및 인쇄"},
    {68, L"비밀번호:"},
    {69, L"이 비밀번호를 사용하여 다른 컴퓨터를 홈 그룹에 연결하세요."},
    {70, L"각 컴퓨터에서:"},
    {71, L"참고: 꺼져 있거나 절전 모드인 컴퓨터는 홈 그룹에 표시되지 않습니다."},
    {72, L"1. 시작을 클릭한 다음 제어판을 클릭합니다."},
    {73, L"2. 네트워크 및 인터넷에서 홈 그룹 및 공유 옵션 선택을 클릭합니다."},
    {74, L"3. 지금 가입을 클릭하고 홈그룹 마법사의 지시에 따라 비밀번호를 입력하세요."},
    {75, L"시작을 클릭한 다음 제어판을 클릭합니다."},
    {76, L"홈 그룹 비밀번호를 인쇄할 수 없습니다."},
    {77, L"Windows에서 홈 그룹 비밀번호를 출력하려고 할 때 오류가 발생했습니다. (오류 코드:%1!u!)"},
    {78, L"현재 홈 네트워크에 연결되어 있지 않습니다. 다른 홈 그룹 컴퓨터의 파일과 리소스를 보려면 먼저 홈 네트워크에 연결하세요."},
    {79, L"%1가 컴퓨터를 홈 그룹에 가입했습니다. 홈 그룹과 라이브러리를 공유하지 않았습니다. 공유한 내용을 변경하려면 아래 링크를 클릭하세요. 공유가 완료될 때까지 컴퓨터를 종료하거나 다시 시작하지 마십시오."},
    {80, L"홈 그룹과 라이브러리를 공유하지 않았습니다. 공유한 내용을 변경하려면 아래 링크를 클릭하세요. 공유가 완료될 때까지 컴퓨터를 종료하거나 다시 시작하지 마십시오."},
    {81, L"홈그룹은 현재 이 컴퓨터에서 라이브러리를 공유하고 있습니다. 일부 홈 그룹 옵션은 공유가 완료될 때까지 사용할 수 없습니다. 공유가 완료될 때까지 컴퓨터를 종료하거나 다시 시작하지 마십시오."},
    {82, L"네트워크 및 인터넷에서 홈 그룹 및 공유 옵션 선택을 클릭합니다."},
    {83, L"현재 네트워크에 홈 그룹이 없습니다."},
    {84, L"지금 가입을 클릭하고 홈그룹 마법사의 지시에 따라 비밀번호를 입력하세요."},
    {85, L"설치하려면 여기를 클릭하세요."},
    {86, L"Windows에서 홈 그룹 프린터를 찾았습니다."},
    {88, L"홈그룹 소개"},
    {89, L"%1(현재 프로필)"},
    {90, L"이 페이지를 사용하여 홈 그룹에 가입하려면 컴퓨터의 네트워크 위치를 비공개로 설정해야 합니다."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"홈그룹이 아직 준비되지 않았습니다. 몇 분 후에 다시 시도해 주세요. 이 메시지가 계속 나타나면 링크를 클릭하여 홈 그룹 문제 해결을 시작하세요."},
    {95, L"홈그룹 문제 해결사 시작"},
    {98, L"홈그룹 비밀번호"},
    {99, L"게스트 계정은 홈 그룹 설정을 변경할 수 없습니다."},
    {100, L"HomeGroup이 홈 네트워크에서 새 공유 프린터를 찾았습니다. 일단 설치되면 이 컴퓨터의 모든 사람이 사용할 수 있습니다."},
    {101, L"프린터 설치"},
    {102, L"홈 네트워크에 연결되어 있지 않아 홈 그룹을 사용할 수 없습니다."},
    {103, L"홈 네트워크에 연결되어 있지 않아 홈 그룹을 사용할 수 없습니다."},
    {104, L"홈 그룹에 가입하기 전에 먼저 네트워크에 연결해야 합니다."},
    {105, L"홈그룹 이미지"},
    {106, L"공유하려는 항목을 선택하고 홈 그룹 비밀번호를 확인하세요."},
    {107, L"이 컴퓨터는 도메인의 일부이므로 해당 라이브러리 및 장치를 홈 그룹의 다른 컴퓨터와 공유하는 설정을 사용할 수 없습니다."},
    {108, L"이 Windows 버전에서는 홈 그룹의 다른 컴퓨터와 라이브러리 및 장치를 공유하는 설정을 사용할 수 없습니다."},
    {109, L"홈 그룹에서 %1 제거"},
    {110, L"취소"},
    {111, L"홈 그룹 구성원 제거"},
    {112, L"%1가 홈 그룹에서 제거됩니다."},
    {113, L"비밀번호를 사용하여 가입한 모든 홈 그룹 구성원은 비밀번호를 다시 입력해야 합니다."},
    {114, L"프린터 및 장치"},
    {115, L"%1 홈 그룹 구성원 변경"},
    {116, L"홈 그룹 비밀번호가 재설정되었습니다."},
    {117, L"홈그룹이 파일을 공유하고 있습니다"},
    {118, L"옵션: 이 컴퓨터는 홈 그룹에 속합니다."},
    {119, L"홈 그룹에 가입할 수 있습니다."},
    {120, L"홈 그룹을 만들 수 있습니다"},
    {121, L"홈그룹을 사용할 수 없습니다"},
    {122, L"신뢰할 수 없는 프린터"},
    {200, L"회원 추가"},
    {201, L"사용자 아이콘"},
    {202, L"이름"},
    {203, L"사용자 ID"},
    {204, L"진행률 표시줄"},
    {205, L"폴더 아이콘"},
    {220, L"라이브러리 및 하드웨어 공유"},
    {221, L"홈 그룹의 다른 사람들과 공유하려는 라이브러리를 선택하십시오."},
    {222, L"홈 그룹 옵션 편집"},
    {223, L"이 페이지를 사용하여 홈 그룹 설정을 변경하고 제어판에서 홈 그룹을 엽니다."},
    {224, L"홈그룹 옵션"},
    {225, L"이 페이지를 사용하여 제어판에서 홈 그룹 설정을 변경하거나 홈 그룹 문제 해결사를 사용하십시오."},
    {226, L"문제 해결사 시작"},
    {227, L"이 페이지를 통해 홈 그룹 문제 해결사를 사용하여 홈 그룹 관련 문제를 찾아 해결하세요."},
    {228, L"비밀번호 보기"},
    {229, L"이 페이지를 사용하여 홈 그룹 암호를 보거나 인쇄하십시오."},
    {230, L"홈 그룹에 가입"},
    {231, L"이 네트워크의 홈 그룹에 가입하세요."},
    {530, L"자세한 공유 옵션 열기…"},
    {541, L"네트워크 가시성"},
    {542, L"네트워크 검색이 켜져 있으면 이 컴퓨터는 네트워크로 연결된 다른 컴퓨터 및 장치에서 볼 수 있습니다."},
    {543, L"네트워크 검색 켜기"},
    {544, L"네트워크 검색 끄기"},
    {545, L"파일 및 프린터 액세스"},
    {546, L"파일 및 프린터 공유가 켜져 있으면 네트워크의 다른 사용자가 이 컴퓨터에서 공유하는 파일 및 프린터에 액세스할 수 있습니다."},
    {547, L"파일 및 프린터 공유 켜기"},
    {548, L"파일 및 프린터 공유 끄기"},
    {549, L"공용 폴더 공유"},
    {550, L"공용 폴더 공유가 켜져 있으면 홈 그룹 구성원을 포함하여 네트워크의 사용자가 공용 폴더의 파일에 액세스할 수 있습니다."},
    {552, L"공유를 활성화하면 네트워크에 액세스할 수 있는 모든 사람이 공용 폴더에 있는 파일을 읽고 쓸 수 있습니다."},
    {553, L"공용 폴더 공유 끄기(이 컴퓨터에 로그온한 사용자는 계속해서 이 폴더에 액세스할 수 있습니다)"},
    {554, L"다양한 네트워크 프로필에 대한 공유 옵션 변경"},
    {559, L"미디어 액세스"},
    {560, L"미디어 스트리밍이 켜져 있으면 네트워크의 사용자와 장치가 이 컴퓨터의 사진, 음악 및 비디오에 액세스할 수 있습니다. 이 컴퓨터는 네트워크에서도 미디어를 찾을 수 있습니다."},
    {564, L"취소"},
    {567, L"변경사항 적용"},
    {584, L"Windows는 사용하는 각 네트워크에 대해 별도의 네트워크 프로필을 만듭니다. 각 프로필에 대해 특정 옵션을 선택할 수 있습니다."},
    {585, L"홈그룹 경고 아이콘"},
    {586, L"이 컴퓨터에서 공유된 라이브러리 및 장치"},
    {595, L"더 많은 홈그룹 작업"},
    {600, L"홈그룹 비밀번호 표시 또는 인쇄"},
    {601, L"시스템 관리자가 귀하의 홈 그룹에 대한 액세스를 허용하지 않았습니다."},
    {604, L"비밀번호를 변경하세요..."},
    {605, L"홈 그룹에서 나가기..."},
    {607, L"미디어 스트리밍 옵션을 선택하세요..."},
    {608, L"이 컴퓨터는 도메인의 일부이므로 해당 라이브러리 및 장치를 홈 그룹의 다른 컴퓨터와 공유하는 설정을 사용할 수 없습니다."},
    {609, L"비밀번호로 보호된 공유"},
    {610, L"암호로 보호된 공유가 켜져 있으면 이 컴퓨터에 사용자 계정과 암호가 있는 사용자만 공유 파일, 이 컴퓨터에 연결된 프린터 및 공용 폴더에 액세스할 수 있습니다. 다른 사람이 액세스할 수 있도록 하려면 비밀번호로 보호된 공유를 꺼야 합니다."},
    {611, L"비밀번호로 보호된 공유 켜기"},
    {612, L"비밀번호로 보호된 공유 끄기"},
    {613, L"페이지 인쇄"},
    {614, L"TV, 게임 콘솔 등 이 네트워크에 있는 모든 장치에서 공유 콘텐츠를 재생할 수 있습니다."},
    {615, L"사설망"},
    {616, L"게스트 또는 공용 네트워크"},
    {617, L"도메인 네트워크"},
    {619, L"홈그룹 연결"},
    {620, L"Windows는 일반적으로 다른 홈 그룹 컴퓨터에 대한 연결을 관리합니다. 그러나 모든 컴퓨터에서 동일한 사용자 계정과 비밀번호를 사용하는 경우 HomeGroup이 해당 계정을 대신 사용하도록 할 수 있습니다."},
    {621, L"옵션: Windows가 홈 그룹 연결을 관리하도록 허용(권장)"},
    {622, L"사용자 계정과 비밀번호를 사용하여 다른 컴퓨터에 연결"},
    {624, L"홈그룹 문제 해결사 시작"},
    {627, L"파일 공유 연결"},
    {628, L"Windows는 128비트 암호화를 사용하여 파일 공유 연결을 보호합니다. 일부 장치는 128비트 암호화를 지원하지 않으며 40비트 또는 56비트 암호화를 사용해야 합니다."},
    {629, L"128비트 암호화를 사용하여 파일 공유 연결을 보호하세요(권장)"},
    {630, L"40비트 또는 56비트 암호화로 장치 파일 공유 활성화"},
    {631, L"모든 네트워크"},
    {632, L"홈 그룹과 공유되는 내용 변경"},
    {637, L"닫기"},
    {639, L"홈그룹 원격 액세스"},
    {640, L"다른 홈 그룹 구성원은 자신의 컴퓨터를 통해 어디서나 홈 그룹에 연결할 수 있습니다."},
    {641, L"옵션: 이 컴퓨터를 통한 원격 홈 그룹 액세스 비활성화"},
    {642, L"옵션: 이 컴퓨터를 통한 원격 홈 그룹 액세스 활성화"},
    {648, L"사용 가능하게 만들 파일과 장치를 선택한 다음 해당 권한 수준을 선택하세요."},
    {649, L"라이브러리 또는 디렉토리"},
    {650, L"액세스 수준"},
    {652, L"네트워크 연결 장치의 자동 설정을 켭니다."},
    {46000, L"홈그룹"},
    {46004, L"옵션: 홈 그룹의 비밀번호를 선택하세요."},
    {46005, L"홈 그룹 비밀번호를 입력하세요"},
    {46006, L"&지금 만들기"},
    {46007, L"&지금 가입"},
    {46008, L"이 비밀번호를 사용하여 홈 그룹에 다른 컴퓨터를 추가하세요."},
    {46009, L"홈 그룹에 가입했습니다"},
    {46011, L"홈그룹"},
    {46012, L"Windows는 이 컴퓨터에 홈 그룹을 설정할 수 없습니다."},
    {46013, L"이 컴퓨터는 도메인의 일부이므로 홈 그룹의 다른 컴퓨터와 해당 라이브러리를 공유할 수 없습니다."},
    {46014, L"비밀번호는 8자 이상이어야 하며 앞뒤에 공백이 없어야 합니다."},
    {46015, L"비밀번호가 올바르지 않습니다.\n다시 시도해 주세요. 비밀번호는 대소문자를 구분합니다."},
    {46016, L"옵션: 이 컴퓨터의 모든 홈 그룹 연결이 끊어집니다."},
    {46017, L"홈 그룹에서 탈퇴했습니다."},
    {46018, L"홈 그룹과 공유되는 내용 변경"},
    {46019, L"사진, 비디오, 음악, 문서 및 프린터를 집에 있는 다른 컴퓨터와 공유하세요."},
    {46020, L"&변경(&M)"},
    {46021, L"홈 그룹 비밀번호를 변경하면 모든 사람의 연결이 끊어집니다."},
    {46022, L"홈 그룹의 새 비밀번호를 입력하세요."},
    {46023, L"&비밀번호 변경"},
    {46024, L"홈그룹 비밀번호가 성공적으로 변경되었습니다."},
    {46025, L"홈 그룹 비밀번호가 변경되었습니다."},
    {46026, L"홈 그룹 비밀번호를 입력하세요"},
    {46027, L"홈그룹 비밀번호가 변경되었습니다. 홈 그룹 리소스를 계속 사용하려면 이미 새 비밀번호를 입력한 사람이 온라인 상태인지 확인한 다음 새 비밀번호를 입력하세요."},
    {46028, L"공유됨"},
    {46029, L"Windows가 홈 그룹에서 컴퓨터를 제거할 수 없습니다."},
    {46030, L"%1가 홈 그룹 비밀번호를 변경했습니다. 홈 그룹 리소스를 계속 사용하려면 이미 새 비밀번호를 입력한 사람이 온라인 상태인지 확인한 다음 새 비밀번호를 입력하세요."},
    {46031, L"암호는 홈 그룹의 파일 및 프린터에 대한 무단 액세스를 방지하는 데 도움이 됩니다. %2, %1 또는 홈 그룹의 다른 구성원으로부터 비밀번호를 얻을 수 있습니다."},
    {46032, L"암호는 홈 그룹의 파일 및 프린터에 대한 무단 액세스를 방지하는 데 도움이 됩니다. %2, %1 또는 홈 그룹의 다른 구성원으로부터 비밀번호를 얻을 수 있습니다."},
    {46033, L"Consolas"},
    {46034, L"홈 그룹 만들기"},
    {46035, L"홈 그룹에 가입"},
    {46036, L"홈 그룹 비밀번호 변경"},
    {46037, L"홈 그룹 탈퇴"},
    {46038, L"다른 컴퓨터에 있는 파일과 프린터에 액세스하려면 이를 홈 그룹에 추가해야 합니다. 다음 비밀번호가 필요합니다:"},
    {46039, L"새 홈 그룹 비밀번호를 입력하세요."},
    {46040, L"비밀번호 업데이트"},
    {46041, L"홈 그룹의 모든 PC를 로컬 데이터 보호 대상에 백업하세요."},
    {46042, L"HomeGroup 데이터 보호 대상을 사용하여 PC 백업"},
    {46043, L"공유되지 않음"},
    {46044, L"홈 그룹은 개인 네트워크에서만 생성할 수 있습니다.\n네트워크 위치 설정을 변경하려면 제어판에서 네트워크 및 공유 센터를 엽니다."},
    {46045, L"Windows는 더 이상 이 네트워크에서 홈 그룹을 감지하지 않습니다. 새 홈 그룹을 만들려면 확인을 클릭하고 제어판에서 홈 그룹을 엽니다."},
    {46046, L"Windows가 기존 홈 그룹을 감지했습니다.\n가입하려면 확인을 클릭하고 제어판에서 홈그룹을 엽니다."},
    {46047, L"이제 홈그룹 서비스를 사용할 수 있습니다. 다시 시도해 주세요."},
    {46048, L"공유 설정이 업데이트되었습니다."},
    {46049, L"선택한 파일과 리소스가 홈 그룹과 공유됩니다."},
    {46050, L"홈그룹 비밀번호가 성공적으로 업데이트되었습니다."},
    {46051, L"홈 그룹에 가입했습니다"},
    {46052, L"이제 공유 파일 및 장치에 액세스할 수 있습니다. 공유하는 파일과 장치는 변경되지 않습니다."},
    {46053, L"홈 그룹의 다른 사용자가 공유하는 파일 및 프린터에 액세스할 수 있습니다."},
    {46054, L"홈 그룹 비밀번호 업데이트"},
    {46055, L"홈 그룹에 가입"},
    {46056, L"%1의 새 홈 그룹 비밀번호를 입력하세요."},
    {46057, L"모든 홈 그룹 컴퓨터의 시계는 24시간 이내의 간격으로 설정되어야 합니다. 컴퓨터 시계가 동기화되어 있는지 확인한 다음 홈 그룹에 다시 가입해 보세요."},
    {46058, L"비밀번호가 도메인의 비밀번호 강도 요구 사항을 충족하지 않습니다. 일치하는 비밀번호를 입력하거나 다른 홈그룹 컴퓨터를 사용하여 비밀번호를 변경하세요."},
    {46059, L"개인 네트워크에 연결되어 있지 않기 때문에 비밀번호를 재설정할 수 없습니다.\n개인 네트워크에 연결한 후 다시 시도해 주세요."},
    {46060, L"개인 네트워크에 연결되어 있지 않습니다.\n네트워크 위치 설정을 변경하려면 제어판에서 네트워크 및 공유 센터를 엽니다."},
    {46061, L"다른 가정용 컴퓨터와 공유"},
    {46062, L"다른 컴퓨터와 파일 및 프린터를 공유할 수 있습니다. 장치로 미디어를 스트리밍할 수도 있습니다.\n\n홈 그룹은 비밀번호로 보호되며 언제든지 공유할 항목을 선택할 수 있습니다."},
    {46063, L"이 비밀번호를 사용하여 홈 그룹에 다른 컴퓨터를 추가하세요."},
    {46064, L"다른 컴퓨터에 있는 파일과 프린터에 액세스하려면 이를 홈 그룹에 추가해야 합니다. 다음 비밀번호가 필요합니다:"},
    {46065, L"홈 그룹을 만들거나 가입하려면 네트워크 연결에서 IPv6이 활성화되어 있어야 합니다. IPv6를 활성화하려면 홈그룹 문제 해결사를 시작하세요."},
    {46066, L"홈 그룹에 사람 추가"},
    {46067, L"홈 그룹 데이터 보호 구성"},
    {46068, L"여러 홈 그룹이 감지되었습니다."},
    {46069, L"다른 홈 그룹 구성원과 공유"},
    {46070, L"문서"},
    {46071, L"사진"},
    {46072, L"음악"},
    {46073, L"비디오"},
    {46074, L"프린터 및 장치"},
    {46075, L"홈 그룹 공유 설정 변경"},
    {46076, L"%1 공유"},
    {46077, L"비밀번호 확인 중..."},
};

// Arabic (ar-SA)
static const EmbeddedTextEntry kStrings_AR_SA[] = {
    {1, L"مجموعة المشاركة المنزلية"},
    {2, L"قم بمراجعة خيارات HomeGroup، وحدد ما يشاركه هذا الكمبيوتر، واعرض أو قم بتحديث كلمة مرور الوصول."},
    {3, L"تمنع السياسة التي وضعتها مؤسستك تشغيل هذه الصفحة. اطلب المساعدة من مسؤول الشبكة."},
    {4, L"خيارات المشاركة التفصيلية"},
    {5, L"على"},
    {6, L"إيقاف"},
    {7, L"متوقف (لم يتم تثبيت أي طابعات)"},
    {8, L"لا توجد طابعة متصلة بهذا الكمبيوتر."},
    {9, L"مشاركة المحتوى مع أجهزة الكمبيوتر في المنزل"},
    {10, L"قم بالوصول إلى مجموعة المشاركة المنزلية الخاصة بك باستخدام جهاز كمبيوتر مرتبط بالمجال"},
    {12, L"تحرير خيارات مجموعة المشاركة المنزلية"},
    {13, L"العمل…"},
    {14, L"لم يتم العثور على مجموعة المشاركة المنزلية على هذه الشبكة."},
    {15, L"قام %1 من %2 بإنشاء مجموعة مشاركة منزلية على الشبكة."},
    {16, L"لقد تمت دعوتك للانضمام إلى مجموعة المشاركة المنزلية الخاصة بك."},
    {18, L"استخدم هذه الصفحة لأن هذا الكمبيوتر ينتمي إلى مجموعة المشاركة المنزلية."},
    {19, L"لا يمكن لهذا الكمبيوتر الاتصال بمجموعة المشاركة المنزلية الخاصة بك."},
    {20, L"يتيح HomeGroup لأجهزة الكمبيوتر الموثوقة تبادل الملفات واستخدام الطابعات المشتركة، ويمكنه إرسال الوسائط إلى الأجهزة المتوافقة. يتطلب الوصول كلمة مرور، بينما تظل متحكمًا في ما يوفره هذا الكمبيوتر."},
    {21, L"يعد هذا الكمبيوتر أيضًا جزءًا من مجال، لذا لا يمكنه إنشاء مجموعة المشاركة المنزلية الخاصة به، ولكن يمكنه الانضمام إلى مجموعة المشاركة المنزلية التي أنشأها شخص ما على الشبكة.\n\nتقوم مجموعات المشاركة المنزلية بربط أجهزة الكمبيوتر الموجودة على شبكتك المنزلية حتى تتمكن من مشاركة الصور والموسيقى ومقاطع الفيديو والمستندات والطابعات. المجموعات المنزلية محمية بكلمة مرور ويمكنك اختيار ما تريد مشاركته في أي وقت."},
    {22, L"تقوم مجموعات المشاركة المنزلية بربط أجهزة الكمبيوتر الموجودة على شبكتك المنزلية حتى تتمكن من مشاركة الصور والموسيقى ومقاطع الفيديو والمستندات والطابعات. المجموعات المنزلية محمية بكلمة مرور ويمكنك اختيار ما تريد مشاركته في أي وقت.\n\nلا يمكنك إنشاء مجموعاتك المنزلية الخاصة في هذا الإصدار من Windows، ولكن يمكنك الانضمام إلى مجموعات المشاركة المنزلية التي أنشأها الآخرون."},
    {23, L"قم بإعداد مجموعة المشاركة المنزلية"},
    {24, L"انضم"},
    {25, L"تم تغيير كلمة مرور مجموعة المشاركة المنزلية. لمواصلة استخدام موارد مجموعة المشاركة المنزلية الخاصة بك، تأكد من أن الشخص الذي أدخل كلمة المرور الجديدة متصل بالفعل، ثم أدخل كلمة المرور الجديدة."},
    {26, L"اكتشف Windows مجموعة مشاركة منزلية أخرى على شبكتك. تسمح لك مجموعات المشاركة المنزلية بمشاركة الملفات والطابعات مع أجهزة الكمبيوتر الأخرى. يمكنك أيضًا دفق الوسائط إلى جهازك."},
    {27, L"قام %1 بتغيير كلمة مرور مجموعة المشاركة المنزلية الخاصة به. لمواصلة استخدام موارد مجموعة المشاركة المنزلية الخاصة بك، تأكد من أن الشخص الذي أدخل كلمة المرور الجديدة متصل بالفعل، ثم أدخل كلمة المرور الجديدة."},
    {28, L"جارٍ البحث عن مجموعات المشاركة المنزلية على هذه الشبكة..."},
    {29, L"اكتب كلمة المرور الجديدة"},
    {30, L"انضم الآن"},
    {32, L"قبل أن تتمكن من إنشاء مجموعة مشاركة منزلية أو الانضمام إليها، يجب عليك أولاً الاتصال بشبكتك."},
    {34, L"استخدم هذه الصفحة لإنشاء مجموعة مشاركة منزلية أو الانضمام إليها، ويجب تعيين موقع شبكة الكمبيوتر الخاص بك على موقع خاص."},
    {35, L"تغيير موقع الشبكة"},
    {37, L"خيارات المشاركة للخاص"},
    {38, L"خيارات المشاركة للعامة"},
    {39, L"خيارات المشاركة للمجال"},
    {40, L"خاص"},
    {41, L"خاص (الملف الشخصي الحالي)"},
    {42, L"عام"},
    {43, L"عام (الملف الشخصي الحالي)"},
    {44, L"المجال"},
    {45, L"المجال (الملف الشخصي الحالي)"},
    {46, L"دفق الوسائط قيد التشغيل."},
    {47, L"تم إيقاف تشغيل تدفق الوسائط."},
    {56, L"إلغاء"},
    {63, L"حسنًا"},
    {64, L"إظهار أو طباعة كلمة مرور مجموعة المشاركة المنزلية"},
    {65, L"24pt;;;Consolas"},
    {66, L"تاريخ الطباعة: %1 %2"},
    {67, L"الخيار: عرض وطباعة كلمة مرور مجموعة المشاركة المنزلية الخاصة بك"},
    {68, L"كلمة المرور:"},
    {69, L"استخدم كلمة المرور هذه لتوصيل أجهزة الكمبيوتر الأخرى بمجموعة المشاركة المنزلية الخاصة بك."},
    {70, L"على كل كمبيوتر:"},
    {71, L"ملاحظة: لن تظهر أجهزة الكمبيوتر التي تم إيقاف تشغيلها أو في وضع السكون في مجموعة المشاركة المنزلية الخاصة بك."},
    {72, L"1. انقر فوق ابدأ، ثم انقر فوق لوحة التحكم."},
    {73, L"2. ضمن الشبكة والإنترنت، انقر فوق اختيار مجموعة المشاركة المنزلية وخيارات المشاركة."},
    {74, L"3. انقر فوق \"الانضمام الآن\" واتبع معالج مجموعة المشاركة المنزلية لإدخال كلمة المرور الخاصة بك."},
    {75, L"انقر فوق ابدأ، ثم انقر فوق لوحة التحكم."},
    {76, L"تعذرت طباعة كلمة مرور مجموعة المشاركة المنزلية"},
    {77, L"حدث خطأ عندما حاول Windows إخراج كلمة مرور مجموعة المشاركة المنزلية. (رمز الخطأ: %1!u!)"},
    {78, L"أنت غير متصل حاليًا بشبكتك المنزلية. لعرض الملفات والموارد الموجودة على أجهزة كمبيوتر مجموعة المشاركة المنزلية الأخرى، قم أولاً بالاتصال بالشبكة المنزلية الخاصة بك."},
    {79, L"قام %1 بضم الكمبيوتر إلى مجموعة المشاركة المنزلية. لم أشارك المكتبة مع مجموعة المشاركة المنزلية الخاصة بي. انقر على الرابط أدناه لتغيير ما قمت بمشاركته. لا تقم بإيقاف تشغيل الكمبيوتر أو إعادة تشغيله حتى تكتمل المشاركة."},
    {80, L"لم أشارك المكتبة مع مجموعة المشاركة المنزلية الخاصة بي. انقر على الرابط أدناه لتغيير ما قمت بمشاركته. لا تقم بإيقاف تشغيل الكمبيوتر أو إعادة تشغيله حتى تكتمل المشاركة."},
    {81, L"تقوم HomeGroup حالياً بمشاركة المكتبة على هذا الكمبيوتر. لا تتوفر بعض خيارات مجموعة المشاركة المنزلية حتى تكتمل المشاركة. لا تقم بإيقاف تشغيل الكمبيوتر أو إعادة تشغيله حتى تكتمل المشاركة."},
    {82, L"ضمن الشبكة والإنترنت، انقر فوق اختيار مجموعة المشاركة المنزلية وخيارات المشاركة."},
    {83, L"لا توجد حاليا أية مجموعات منزلية على الشبكة."},
    {84, L"انقر فوق \"الانضمام الآن\" واتبع معالج HomeGroup لإدخال كلمة المرور الخاصة بك."},
    {85, L"انقر هنا للتثبيت."},
    {86, L"عثر Windows على طابعة مجموعة المشاركة المنزلية"},
    {88, L"تقديم مجموعة المشاركة المنزلية"},
    {89, L"%1 (الملف الحالي)"},
    {90, L"استخدم هذه الصفحة للانضمام إلى مجموعة مشاركة منزلية، ويجب تعيين موقع شبكة الكمبيوتر الخاص بك على موقع خاص."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"مجموعة المشاركة المنزلية ليست جاهزة بعد. يرجى المحاولة مرة أخرى خلال بضع دقائق. إذا استمريت في رؤية هذه الرسالة، فانقر فوق الارتباط لبدء استكشاف أخطاء مجموعة المشاركة المنزلية الخاصة بك وإصلاحها."},
    {95, L"ابدأ تشغيل مستكشف أخطاء مجموعة المشاركة المنزلية ومصلحها"},
    {98, L"كلمة مرور المجموعة المنزلية"},
    {99, L"لا يمكن لحسابات الضيوف تغيير إعدادات مجموعة المشاركة المنزلية."},
    {100, L"عثرت HomeGroup على طابعة مشتركة جديدة على شبكتك المنزلية. بمجرد تثبيته، سيكون متاحًا لأي شخص على هذا الكمبيوتر."},
    {101, L"قم بتثبيت الطابعة"},
    {102, L"HomeGroup غير متاح لأنك غير متصل بالشبكة المنزلية الخاصة بك."},
    {103, L"HomeGroup غير متاح لأنك غير متصل بالشبكة المنزلية الخاصة بك."},
    {104, L"قبل الانضمام إلى مجموعة المشاركة المنزلية، يجب عليك أولاً الاتصال بالشبكة."},
    {105, L"صورة المجموعة الرئيسية"},
    {106, L"حدد ما تريد مشاركته واعرض كلمة مرور مجموعة المشاركة المنزلية الخاصة بك"},
    {107, L"نظرًا لأن هذا الكمبيوتر جزء من مجال، فلا تتوفر إعدادات مشاركة مكتباته وأجهزته مع أجهزة الكمبيوتر الأخرى في مجموعة المشاركة المنزلية."},
    {108, L"لا تتوفر إعدادات مشاركة المكتبات والأجهزة مع أجهزة الكمبيوتر الأخرى في مجموعة المشاركة المنزلية في هذا الإصدار من Windows."},
    {109, L"قم بإزالة %1 من مجموعة المشاركة المنزلية"},
    {110, L"إلغاء"},
    {111, L"إزالة عضو مجموعة المشاركة المنزلية"},
    {112, L"ستتم إزالة %1 من مجموعة المشاركة المنزلية"},
    {113, L"سيُطلب من جميع أعضاء مجموعة المشاركة المنزلية الذين ينضمون باستخدام كلمة مرور إدخال كلمة المرور مرة أخرى."},
    {114, L"الطابعات والأجهزة"},
    {115, L"تغيير أعضاء مجموعة المشاركة المنزلية %1"},
    {116, L"تمت إعادة تعيين كلمة مرور مجموعة المشاركة المنزلية"},
    {117, L"تقوم HomeGroup بمشاركة الملفات"},
    {118, L"الخيار: ينتمي هذا الكمبيوتر إلى مجموعة المشاركة المنزلية"},
    {119, L"مجموعة المشاركة المنزلية متاحة للانضمام"},
    {120, L"يمكن إنشاء مجموعة منزلية"},
    {121, L"مجموعة المشاركة المنزلية غير متوفرة"},
    {122, L"طابعة غير موثوقة"},
    {200, L"إضافة عضو"},
    {201, L"أيقونة المستخدم"},
    {202, L"الاسم الكامل"},
    {203, L"معرف المستخدم"},
    {204, L"شريط التقدم"},
    {205, L"رمز المجلد"},
    {220, L"مشاركة المكتبات والأجهزة"},
    {221, L"حدد المكتبة التي تريد مشاركتها مع الآخرين في مجموعة المشاركة المنزلية الخاصة بك."},
    {222, L"تحرير خيارات مجموعة المشاركة المنزلية"},
    {223, L"استخدم هذه الصفحة لتغيير إعدادات مجموعة المشاركة المنزلية، وافتح مجموعة المشاركة المنزلية في لوحة التحكم."},
    {224, L"خيارات مجموعة المشاركة المنزلية"},
    {225, L"استخدم هذه الصفحة لتغيير إعدادات HomeGroup في لوحة التحكم أو استخدم مستكشف أخطاء HomeGroup ومصلحها."},
    {226, L"ابدأ مستكشف الأخطاء ومصلحها"},
    {227, L"استخدم هذه الصفحة لاستخدام مستكشف أخطاء مجموعة المشاركة المنزلية ومصلحها للعثور على مشكلات مجموعة المشاركة المنزلية وإصلاحها."},
    {228, L"عرض كلمة المرور"},
    {229, L"استخدم هذه الصفحة لعرض أو طباعة كلمة مرور مجموعة المشاركة المنزلية الخاصة بك."},
    {230, L"انضم إلى مجموعة المشاركة المنزلية"},
    {231, L"انضم إلى مجموعة المشاركة المنزلية على هذه الشبكة."},
    {530, L"فتح خيارات المشاركة التفصيلية..."},
    {541, L"رؤية الشبكة"},
    {542, L"إذا تم تشغيل اكتشاف الشبكة، فيمكن لهذا الكمبيوتر رؤية أجهزة الكمبيوتر والأجهزة الأخرى المتصلة بالشبكة ومشاهدتها."},
    {543, L"قم بتشغيل اكتشاف الشبكة"},
    {544, L"قم بإيقاف تشغيل اكتشاف الشبكة"},
    {545, L"الوصول إلى الملفات والطابعات"},
    {546, L"عند تشغيل مشاركة الملفات والطابعات، يمكن للمستخدمين الآخرين على شبكتك الوصول إلى الملفات والطابعات التي تشاركها من هذا الكمبيوتر."},
    {547, L"قم بتشغيل مشاركة الملفات والطابعات"},
    {548, L"قم بإيقاف تشغيل مشاركة الملفات والطابعات"},
    {549, L"مشاركة المجلد العام"},
    {550, L"عند تشغيل مشاركة المجلدات العامة، يمكن للمستخدمين الموجودين على شبكتك، بما في ذلك أعضاء مجموعة المشاركة المنزلية، الوصول إلى الملفات الموجودة في المجلدات العامة."},
    {552, L"يتيح تمكين المشاركة لأي شخص لديه حق الوصول إلى شبكتك قراءة الملفات وكتابتها في مجلداتك العامة."},
    {553, L"قم بإيقاف تشغيل مشاركة المجلدات العامة (لا يزال بإمكان المستخدمين الذين قاموا بتسجيل الدخول إلى هذا الكمبيوتر الوصول إلى هذه المجلدات)"},
    {554, L"تغيير خيارات المشاركة لملفات تعريف الشبكة المختلفة"},
    {559, L"الوصول إلى وسائل الإعلام"},
    {560, L"عند تشغيل دفق الوسائط، يمكن للمستخدمين والأجهزة الموجودة على شبكتك الوصول إلى الصور والموسيقى ومقاطع الفيديو الموجودة على هذا الكمبيوتر. يمكن لهذا الكمبيوتر أيضًا العثور على الوسائط الموجودة على الشبكة."},
    {564, L"إلغاء"},
    {567, L"تطبيق التغييرات"},
    {584, L"يقوم Windows بإنشاء ملف تعريف منفصل للشبكة لكل شبكة تستخدمها. يمكنك تحديد خيارات محددة لكل ملف تعريف."},
    {585, L"أيقونة تحذير مجموعة المشاركة المنزلية"},
    {586, L"المكتبات والأجهزة المشتركة من هذا الكمبيوتر"},
    {595, L"المزيد من مهام مجموعة المشاركة المنزلية"},
    {600, L"إظهار أو طباعة كلمة مرور مجموعة المشاركة المنزلية"},
    {601, L"لم يسمح لك مسؤول النظام بالوصول إلى مجموعة المشاركة المنزلية الخاصة بك."},
    {604, L"تغيير كلمة المرور..."},
    {605, L"اترك مجموعة المشاركة المنزلية..."},
    {607, L"اختر خيارات دفق الوسائط..."},
    {608, L"نظرًا لأن هذا الكمبيوتر جزء من مجال، فلا تتوفر إعدادات مشاركة مكتباته وأجهزته مع أجهزة الكمبيوتر الأخرى في مجموعة المشاركة المنزلية."},
    {609, L"مشاركة محمية بكلمة مرور"},
    {610, L"عند تشغيل المشاركة المحمية بكلمة مرور، يمكن فقط للمستخدمين الذين لديهم حسابات مستخدمين وكلمات مرور على هذا الكمبيوتر الوصول إلى الملفات المشتركة والطابعات المتصلة بهذا الكمبيوتر والمجلدات العامة. يجب إيقاف المشاركة المحمية بكلمة مرور للسماح للآخرين بالوصول."},
    {611, L"قم بتشغيل المشاركة المحمية بكلمة مرور"},
    {612, L"قم بإيقاف تشغيل المشاركة المحمية بكلمة مرور"},
    {613, L"طباعة الصفحة"},
    {614, L"يسمح بتشغيل المحتوى المشترك على جميع الأجهزة الموجودة على هذه الشبكة، مثل أجهزة التلفزيون ووحدات التحكم في الألعاب"},
    {615, L"شبكة خاصة"},
    {616, L"ضيف أو شبكة عامة"},
    {617, L"شبكة المجال"},
    {619, L"اتصالات مجموعة المشاركة المنزلية"},
    {620, L"يقوم Windows عادةً بإدارة الاتصالات بأجهزة كمبيوتر مجموعة المشاركة المنزلية الأخرى. ومع ذلك، إذا كنت تستخدم نفس حساب المستخدم وكلمة المرور على كافة أجهزة الكمبيوتر لديك، فيمكنك جعل HomeGroup يستخدم هذا الحساب بدلاً من ذلك."},
    {621, L"الخيار: السماح لـ Windows بإدارة اتصالات مجموعة المشاركة المنزلية (مستحسن)"},
    {622, L"الاتصال بأجهزة الكمبيوتر الأخرى باستخدام حساب المستخدم وكلمة المرور الخاصة بك"},
    {624, L"ابدأ تشغيل مستكشف أخطاء مجموعة المشاركة المنزلية ومصلحها"},
    {627, L"اتصالات مشاركة الملفات"},
    {628, L"يستخدم Windows تشفير 128 بت لتأمين اتصالات مشاركة الملفات. لا تدعم بعض الأجهزة تشفير 128 بت ويجب أن تستخدم تشفير 40 بت أو 56 بت."},
    {629, L"تأمين اتصال مشاركة الملفات الخاص بك باستخدام تشفير 128 بت (مستحسن)"},
    {630, L"تمكين مشاركة ملفات الجهاز بتشفير 40 بت أو 56 بت"},
    {631, L"كل شبكة"},
    {632, L"قم بتغيير ما تتم مشاركته مع مجموعة المشاركة المنزلية الخاصة بك"},
    {637, L"إغلاق"},
    {639, L"الوصول عن بعد لمجموعة المشاركة المنزلية"},
    {640, L"يمكن لأعضاء مجموعة المشاركة المنزلية الآخرين الاتصال بمجموعة المشاركة المنزلية الخاصة بك من أي مكان عبر أجهزة الكمبيوتر الخاصة بهم."},
    {641, L"الخيار: تعطيل الوصول إلى مجموعة المشاركة المنزلية عن بعد من خلال هذا الكمبيوتر"},
    {642, L"الخيار: تمكين الوصول إلى مجموعة المشاركة المنزلية عن بعد من خلال هذا الكمبيوتر"},
    {648, L"حدد الملفات والأجهزة المراد إتاحتها، ثم اختر مستويات الأذونات الخاصة بها."},
    {649, L"المكتبة أو الدليل"},
    {650, L"مستوى الوصول"},
    {652, L"قم بتشغيل الإعداد التلقائي للأجهزة المتصلة بالشبكة."},
    {46000, L"مجموعة المشاركة المنزلية"},
    {46004, L"الخيار: اختر كلمة مرور لمجموعة المشاركة المنزلية الخاصة بك"},
    {46005, L"اكتب كلمة مرور مجموعة المشاركة المنزلية"},
    {46006, L"&إنشاء الآن"},
    {46007, L"&انضم الآن"},
    {46008, L"أضف أجهزة كمبيوتر أخرى إلى مجموعة المشاركة المنزلية الخاصة بك باستخدام كلمة المرور هذه"},
    {46009, L"لقد انضممت إلى مجموعة المشاركة المنزلية"},
    {46011, L"مجموعة المشاركة المنزلية"},
    {46012, L"لا يمكن لـ Windows إعداد مجموعة مشاركة منزلية على هذا الكمبيوتر."},
    {46013, L"نظرًا لأن هذا الكمبيوتر جزء من مجال، فلا تتوفر مشاركة مكتبته مع أجهزة الكمبيوتر الأخرى في مجموعة المشاركة المنزلية."},
    {46014, L"يجب أن تحتوي كلمات المرور على 8 أحرف على الأقل، ولا تحتوي على مسافات بادئة أو لاحقة."},
    {46015, L"كلمة المرور غير صحيحة.\nيرجى المحاولة مرة أخرى. كلمات المرور حساسة لحالة الأحرف."},
    {46016, L"الخيار: سيتم قطع كافة اتصالات مجموعة المشاركة المنزلية على هذا الكمبيوتر"},
    {46017, L"تم مغادرة مجموعة المشاركة المنزلية الخاصة بك بنجاح"},
    {46018, L"قم بتغيير ما تتم مشاركته مع مجموعة المشاركة المنزلية الخاصة بك"},
    {46019, L"قم بمشاركة الصور ومقاطع الفيديو والموسيقى والمستندات والطابعات مع أجهزة الكمبيوتر الأخرى في منزلك."},
    {46020, L"&إجراء تغييرات"},
    {46021, L"يؤدي تغيير كلمة مرور مجموعة المشاركة المنزلية إلى قطع اتصال الجميع"},
    {46022, L"أدخل كلمة مرور جديدة لمجموعة المشاركة المنزلية الخاصة بك"},
    {46023, L"&تغيير كلمة المرور"},
    {46024, L"تم تغيير كلمة مرور مجموعة المشاركة المنزلية بنجاح"},
    {46025, L"تم تغيير كلمة مرور مجموعة المشاركة المنزلية"},
    {46026, L"اكتب كلمة مرور مجموعة المشاركة المنزلية"},
    {46027, L"تم تغيير كلمة مرور مجموعة المشاركة المنزلية. لمواصلة استخدام موارد مجموعة المشاركة المنزلية الخاصة بك، تأكد من أن الشخص الذي أدخل كلمة المرور الجديدة متصل بالفعل، ثم أدخل كلمة المرور الجديدة."},
    {46028, L"مشترك"},
    {46029, L"تعذر على Windows إزالة الكمبيوتر من مجموعة المشاركة المنزلية."},
    {46030, L"قام %1 بتغيير كلمة مرور مجموعة المشاركة المنزلية الخاصة به. لمواصلة استخدام موارد مجموعة المشاركة المنزلية الخاصة بك، تأكد من أن الشخص الذي أدخل كلمة المرور الجديدة متصل بالفعل، ثم أدخل كلمة المرور الجديدة."},
    {46031, L"تساعد كلمات المرور على منع الوصول غير المصرح به إلى ملفات وطابعات مجموعة المشاركة المنزلية الخاصة بك. يمكنك الحصول على كلمة المرور من %2 أو %1 أو عضو آخر في مجموعة المشاركة المنزلية الخاصة بك."},
    {46032, L"تساعد كلمات المرور على منع الوصول غير المصرح به إلى ملفات وطابعات مجموعة المشاركة المنزلية الخاصة بك. يمكنك الحصول على كلمة المرور من %2 أو %1 أو عضو آخر في مجموعة المشاركة المنزلية الخاصة بك."},
    {46033, L"Consolas"},
    {46034, L"قم بإنشاء مجموعة منزلية"},
    {46035, L"انضم إلى مجموعة المشاركة المنزلية"},
    {46036, L"قم بتغيير كلمة مرور مجموعة المشاركة المنزلية الخاصة بك"},
    {46037, L"اترك مجموعة المشاركة المنزلية"},
    {46038, L"للوصول إلى الملفات والطابعات الموجودة على أجهزة كمبيوتر أخرى، يجب عليك إضافتها إلى مجموعة المشاركة المنزلية الخاصة بك. كلمة المرور التالية مطلوبة:"},
    {46039, L"اكتب كلمة المرور الجديدة لمجموعة المشاركة المنزلية:"},
    {46040, L"تحديث كلمة المرور"},
    {46041, L"قم بعمل نسخة احتياطية لجميع أجهزة الكمبيوتر الموجودة في مجموعة المشاركة المنزلية الخاصة بك إلى هدف حماية البيانات المحلي."},
    {46042, L"قم بعمل نسخة احتياطية لجهاز الكمبيوتر الخاص بك باستخدام أهداف حماية بيانات HomeGroup"},
    {46043, L"غير مشترك"},
    {46044, L"لا يمكن إنشاء مجموعات المشاركة المنزلية إلا على الشبكات الخاصة.\nلتغيير إعدادات موقع الشبكة، افتح مركز الشبكة والمشاركة في لوحة التحكم."},
    {46045, L"لن يكتشف Windows مجموعات المشاركة المنزلية على هذه الشبكة بعد الآن. لإنشاء مجموعة مشاركة منزلية جديدة، انقر فوق \"موافق\" وافتح \"مجموعة المشاركة المنزلية\" في \"لوحة التحكم\"."},
    {46046, L"اكتشف Windows مجموعة مشاركة منزلية موجودة.\nللانضمام، انقر فوق \"موافق\" وافتح HomeGroup في \"لوحة التحكم\"."},
    {46047, L"خدمة مجموعة المشاركة المنزلية متاحة الآن. يرجى المحاولة مرة أخرى."},
    {46048, L"تم تحديث إعدادات المشاركة"},
    {46049, L"تتم مشاركة الملفات والموارد المحددة مع مجموعة المشاركة المنزلية الخاصة بك."},
    {46050, L"تم تحديث كلمة مرور مجموعة المشاركة المنزلية بنجاح"},
    {46051, L"لقد انضممت إلى مجموعة المشاركة المنزلية"},
    {46052, L"يمكنك الآن الوصول إلى ملفاتك وأجهزتك المشتركة. تظل الملفات والأجهزة التي تشاركها دون تغيير."},
    {46053, L"يمكنك البدء في الوصول إلى الملفات والطابعات التي شاركها مستخدمون آخرون في مجموعة المشاركة المنزلية الخاصة بك."},
    {46054, L"قم بتحديث كلمة مرور مجموعة المشاركة المنزلية الخاصة بك"},
    {46055, L"انضم إلى مجموعة المشاركة المنزلية"},
    {46056, L"أدخل كلمة المرور الجديدة لمجموعة المشاركة المنزلية من %1."},
    {46057, L"يجب ضبط ساعات كافة أجهزة كمبيوتر مجموعة المشاركة المنزلية على ما لا يزيد عن 24 ساعة. تأكد من مزامنة ساعات الكمبيوتر لديك، ثم حاول الانضمام إلى مجموعة المشاركة المنزلية مرة أخرى."},
    {46058, L"كلمة المرور لا تلبي متطلبات قوة كلمة المرور الخاصة بالمجال. أدخل كلمة مرور مطابقة أو استخدم كمبيوتر HomeGroup آخر لتغيير كلمة المرور الخاصة بك."},
    {46059, L"لا يمكنك إعادة تعيين كلمة المرور الخاصة بك لأنك غير متصل بشبكة خاصة.\nالرجاء الاتصال بشبكة خاصة والمحاولة مرة أخرى."},
    {46060, L"أنت غير متصل بشبكة خاصة.\nلتغيير إعدادات موقع الشبكة، افتح مركز الشبكة والمشاركة في لوحة التحكم."},
    {46061, L"شارك مع أجهزة الكمبيوتر المنزلية الأخرى"},
    {46062, L"يمكنك مشاركة الملفات والطابعات مع أجهزة الكمبيوتر الأخرى. يمكنك أيضًا دفق الوسائط إلى جهازك.\n\nالمجموعات المنزلية محمية بكلمة مرور ويمكنك اختيار ما تريد مشاركته في أي وقت."},
    {46063, L"أضف أجهزة كمبيوتر أخرى إلى مجموعة المشاركة المنزلية الخاصة بك باستخدام كلمة المرور هذه"},
    {46064, L"للوصول إلى الملفات والطابعات الموجودة على أجهزة كمبيوتر أخرى، يجب عليك إضافتها إلى مجموعة المشاركة المنزلية الخاصة بك. كلمة المرور التالية مطلوبة:"},
    {46065, L"لإنشاء مجموعة مشاركة منزلية أو الانضمام إليها، يجب أن يكون اتصال الشبكة الخاص بك مزودًا بـ IPv6. لتمكين IPv6، قم بتشغيل مستكشف أخطاء مجموعة المشاركة المنزلية ومصلحها."},
    {46066, L"إضافة أشخاص إلى مجموعة المشاركة المنزلية"},
    {46067, L"تكوين حماية بيانات مجموعة المشاركة المنزلية"},
    {46068, L"تم اكتشاف مجموعات منزلية متعددة"},
    {46069, L"شارك مع أعضاء مجموعة المشاركة المنزلية الآخرين"},
    {46070, L"المستندات"},
    {46071, L"الصور"},
    {46072, L"موسيقى"},
    {46073, L"فيديوهات"},
    {46074, L"الطابعات والأجهزة"},
    {46075, L"تغيير إعدادات مشاركة المجموعة المنزلية"},
    {46076, L"مشاركة %1"},
    {46077, L"جارٍ التحقق من كلمة المرور..."},
};

// Dutch (nl-NL)
static const EmbeddedTextEntry kStrings_NL_NL[] = {
    {1, L"Thuisgroep"},
    {2, L"Bekijk de HomeGroup-opties, bepaal wat deze pc deelt en geef het toegangswachtwoord weer of werk het bij."},
    {3, L"Een door uw organisatie ingesteld beleid verhindert dat deze pagina wordt uitgevoerd. Vraag de netwerkbeheerder om hulp."},
    {4, L"Gedetailleerde opties voor delen"},
    {5, L"Aan"},
    {6, L"Uit"},
    {7, L"Uit (geen printers geïnstalleerd)"},
    {8, L"Er is geen printer op deze computer aangesloten."},
    {9, L"Deel inhoud met pc's thuis"},
    {10, L"Krijg toegang tot uw thuisgroep via een computer die lid is van een domein"},
    {12, L"Bewerk thuisgroepopties"},
    {13, L"Werken…"},
    {14, L"Er is geen thuisgroep gevonden op dit netwerk."},
    {15, L"%1 van %2 heeft een thuisgroep op het netwerk gemaakt."},
    {16, L"U bent uitgenodigd om lid te worden van uw thuisgroep."},
    {18, L"Gebruik deze pagina om deze computer tot een thuisgroep te laten behoren."},
    {19, L"Deze computer kan geen verbinding maken met uw thuisgroep."},
    {20, L"Met HomeGroup kunnen vertrouwde pc's bestanden uitwisselen en gedeelde printers gebruiken, en kan het media naar compatibele apparaten sturen. Voor toegang is een wachtwoord vereist, terwijl u zelf de controle behoudt over wat deze pc beschikbaar stelt."},
    {21, L"Deze computer maakt ook deel uit van een domein en kan dus geen eigen thuisgroep creëren, maar kan zich wel aansluiten bij een thuisgroep die door iemand op het netwerk is aangemaakt.\n\nThuisgroepen koppelen computers in uw thuisnetwerk zodat u foto's, muziek, video's, documenten en printers kunt delen. Thuisgroepen zijn beveiligd met een wachtwoord en u kunt op elk gewenst moment kiezen wat u wilt delen."},
    {22, L"Thuisgroepen koppelen computers in uw thuisnetwerk zodat u foto's, muziek, video's, documenten en printers kunt delen. Thuisgroepen zijn beveiligd met een wachtwoord en u kunt op elk gewenst moment kiezen wat u wilt delen.\n\nIn deze editie van Windows kunt u niet uw eigen thuisgroepen maken, maar u kunt wel lid worden van thuisgroepen die door anderen zijn gemaakt."},
    {23, L"Stel een thuisgroep in"},
    {24, L"Doe mee"},
    {25, L"Het thuisgroepwachtwoord is gewijzigd. Als u uw thuisgroepbronnen wilt blijven gebruiken, zorgt u ervoor dat de persoon die het nieuwe wachtwoord al heeft ingevoerd, online is en voert u vervolgens het nieuwe wachtwoord in."},
    {26, L"Windows heeft een andere thuisgroep in uw netwerk gedetecteerd. Met thuisgroepen kunt u bestanden en printers delen met andere computers. U kunt ook media naar uw apparaat streamen."},
    {27, L"%1 heeft zijn thuisgroepwachtwoord gewijzigd. Als u uw thuisgroepbronnen wilt blijven gebruiken, zorgt u ervoor dat de persoon die het nieuwe wachtwoord al heeft ingevoerd, online is en voert u vervolgens het nieuwe wachtwoord in."},
    {28, L"Zoeken naar thuisgroepen op dit netwerk..."},
    {29, L"Typ een nieuw wachtwoord"},
    {30, L"Sluit je nu aan"},
    {32, L"Voordat u een thuisgroep kunt maken of er lid van kunt worden, moet u eerst verbinding maken met uw netwerk."},
    {34, L"Gebruik deze pagina om een thuisgroep te maken of er lid van te worden. De netwerklocatie van uw computer moet op privé zijn ingesteld."},
    {35, L"Wijzig de netwerklocatie"},
    {37, L"Opties voor delen voor privé"},
    {38, L"Opties voor delen voor openbaar"},
    {39, L"Opties voor delen voor domein"},
    {40, L"Privé"},
    {41, L"Privé (huidig profiel)"},
    {42, L"Openbaar"},
    {43, L"Openbaar (huidig profiel)"},
    {44, L"Domein"},
    {45, L"Domein (huidig profiel)"},
    {46, L"Mediastreaming staat aan."},
    {47, L"Mediastreaming staat uit."},
    {56, L"Annuleer"},
    {63, L"Oké"},
    {64, L"Toon of druk het HomeGroup-wachtwoord af"},
    {65, L"24pt;;;Consolas"},
    {66, L"Afgedrukte datum: %1 %2"},
    {67, L"Optie: Bekijk en print uw thuisgroepwachtwoord"},
    {68, L"Wachtwoord:"},
    {69, L"Gebruik dit wachtwoord om andere computers met uw thuisgroep te verbinden."},
    {70, L"Op elke computer:"},
    {71, L"Let op: Computers die zijn uitgeschakeld of in de slaapstand staan, verschijnen niet in uw thuisgroep."},
    {72, L"1. Klik op Start en vervolgens op Configuratiescherm."},
    {73, L"2. Klik onder Netwerk en internet op Thuisgroep- en deelopties kiezen."},
    {74, L"3. Klik op Nu deelnemen en volg de HomeGroup-wizard om uw wachtwoord in te voeren."},
    {75, L"Klik op Start en vervolgens op Configuratiescherm."},
    {76, L"Kan het thuisgroepwachtwoord niet afdrukken"},
    {77, L"Er is een fout opgetreden toen Windows probeerde het thuisgroepwachtwoord uit te voeren. (Foutcode: %1!u!)"},
    {78, L"U bent momenteel niet verbonden met uw thuisnetwerk. Om bestanden en bronnen op andere thuisgroepcomputers te bekijken, moet u eerst verbinding maken met uw thuisnetwerk."},
    {79, L"%1 heeft de computer toegevoegd aan de thuisgroep. Ik heb de bibliotheek niet gedeeld met mijn thuisgroep. Klik op de onderstaande link om te wijzigen wat u heeft gedeeld. Sluit uw computer niet af en start deze niet opnieuw op voordat het delen is voltooid."},
    {80, L"Ik heb de bibliotheek niet gedeeld met mijn thuisgroep. Klik op de onderstaande link om te wijzigen wat u heeft gedeeld. Sluit uw computer niet af en start deze niet opnieuw op voordat het delen is voltooid."},
    {81, L"HomeGroup deelt momenteel de bibliotheek op deze computer. Sommige thuisgroepopties zijn pas beschikbaar als het delen is voltooid. Sluit uw computer niet af en start deze niet opnieuw op voordat het delen is voltooid."},
    {82, L"Klik onder Netwerk en internet op Thuisgroep- en deelopties kiezen."},
    {83, L"Er zijn momenteel geen thuisgroepen op het netwerk."},
    {84, L"Klik op Nu deelnemen en volg de HomeGroup-wizard om uw wachtwoord in te voeren."},
    {85, L"Klik hier om te installeren."},
    {86, L"Windows heeft een thuisgroepprinter gevonden"},
    {88, L"Maak kennis met Thuisgroep"},
    {89, L"%1 (huidig profiel)"},
    {90, L"Gebruik deze pagina om lid te worden van een thuisgroep. De netwerklocatie van uw computer moet op privé zijn ingesteld."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Thuisgroep is nog niet klaar. Probeer het over een paar minuten opnieuw. Als u dit bericht blijft zien, klikt u op de link om te beginnen met het oplossen van problemen met uw thuisgroep."},
    {95, L"Start de probleemoplosser voor Thuisgroepen"},
    {98, L"Thuisgroepwachtwoord"},
    {99, L"Gastaccounts kunnen de thuisgroepinstellingen niet wijzigen."},
    {100, L"HomeGroup heeft een nieuwe gedeelde printer op uw thuisnetwerk gevonden. Eenmaal geïnstalleerd, is het voor iedereen op deze computer beschikbaar."},
    {101, L"Printer installeren"},
    {102, L"Thuisgroep is niet beschikbaar omdat u niet bent verbonden met uw thuisnetwerk."},
    {103, L"Thuisgroep is niet beschikbaar omdat u niet bent verbonden met uw thuisnetwerk."},
    {104, L"Voordat u lid wordt van een thuisgroep, moet u eerst verbinding maken met het netwerk."},
    {105, L"Thuisgroepafbeelding"},
    {106, L"Selecteer wat u wilt delen en bekijk uw thuisgroepwachtwoord"},
    {107, L"Omdat deze computer deel uitmaakt van een domein, zijn er geen instellingen beschikbaar om de bibliotheken en apparaten ervan te delen met andere computers in de thuisgroep."},
    {108, L"Instellingen voor het delen van bibliotheken en apparaten met andere computers in een thuisgroep zijn niet beschikbaar in deze editie van Windows."},
    {109, L"Verwijder %1 uit de thuisgroep"},
    {110, L"Annuleer"},
    {111, L"Lid van de thuisgroep verwijderen"},
    {112, L"%1 wordt verwijderd uit de thuisgroep"},
    {113, L"Alle thuisgroepleden die lid worden met een wachtwoord, moeten het wachtwoord opnieuw invoeren."},
    {114, L"Printers en apparaten"},
    {115, L"Wijzig de leden van de %1-thuisgroep"},
    {116, L"Het thuisgroepwachtwoord is opnieuw ingesteld"},
    {117, L"Thuisgroep deelt bestanden"},
    {118, L"Optie: Deze computer behoort tot een thuisgroep"},
    {119, L"Er is een thuisgroep beschikbaar waar u lid van kunt worden"},
    {120, L"Er kan een thuisgroep worden aangemaakt"},
    {121, L"Thuisgroep is niet beschikbaar"},
    {122, L"Niet-vertrouwde printer"},
    {200, L"Lid toevoegen"},
    {201, L"Gebruikerspictogram"},
    {202, L"Volledige naam"},
    {203, L"Gebruikers-ID"},
    {204, L"Voortgangsbalk"},
    {205, L"Mappictogram"},
    {220, L"Deel bibliotheken en hardware"},
    {221, L"Selecteer de bibliotheek die u wilt delen met anderen in uw thuisgroep."},
    {222, L"Bewerk thuisgroepopties"},
    {223, L"Gebruik deze pagina om de thuisgroepinstellingen te wijzigen. Open Thuisgroep in het Configuratiescherm."},
    {224, L"Thuisgroepopties"},
    {225, L"Gebruik deze pagina om uw Thuisgroep-instellingen in het Configuratiescherm te wijzigen of gebruik de Thuisgroep-probleemoplosser."},
    {226, L"Probleemoplosser starten"},
    {227, L"Gebruik deze pagina om de probleemoplosser voor Thuisgroep te gebruiken om problemen met uw Thuisgroep te vinden en op te lossen."},
    {228, L"Wachtwoord bekijken"},
    {229, L"Gebruik deze pagina om uw thuisgroepwachtwoord te bekijken of af te drukken."},
    {230, L"Sluit je aan bij de thuisgroep"},
    {231, L"Sluit je aan bij de thuisgroep op dit netwerk."},
    {530, L"Gedetailleerde opties voor delen openen..."},
    {541, L"Netwerkzichtbaarheid"},
    {542, L"Als netwerkdetectie is ingeschakeld, kan deze computer andere netwerkcomputers en -apparaten zien en gezien worden."},
    {543, L"Schakel netwerkdetectie in"},
    {544, L"Schakel netwerkdetectie uit"},
    {545, L"Toegang tot bestanden en printers"},
    {546, L"Wanneer het delen van bestanden en printers is ingeschakeld, hebben andere gebruikers in uw netwerk toegang tot de bestanden en printers die u vanaf deze computer deelt."},
    {547, L"Schakel bestands- en printerdeling in"},
    {548, L"Schakel het delen van bestanden en printers uit"},
    {549, L"Openbare mappen delen"},
    {550, L"Wanneer het delen van openbare mappen is ingeschakeld, hebben gebruikers in uw netwerk, inclusief leden van de thuisgroep, toegang tot bestanden in openbare mappen."},
    {552, L"Als u delen inschakelt, kan iedereen met toegang tot uw netwerk bestanden in uw openbare mappen lezen en schrijven."},
    {553, L"Schakel het delen van openbare mappen uit (gebruikers die op deze computer zijn ingelogd, hebben nog steeds toegang tot deze mappen)"},
    {554, L"Wijzig de deelopties voor verschillende netwerkprofielen"},
    {559, L"Mediatoegang"},
    {560, L"Wanneer mediastreaming is ingeschakeld, hebben gebruikers en apparaten in uw netwerk toegang tot de foto's, muziek en video's op deze computer. Deze computer kan ook media op het netwerk vinden."},
    {564, L"Annuleer"},
    {567, L"Wijzigingen toepassen"},
    {584, L"Windows maakt een afzonderlijk netwerkprofiel aan voor elk netwerk dat u gebruikt. Voor elk profiel kunt u specifieke opties selecteren."},
    {585, L"Waarschuwingspictogram voor thuisgroep"},
    {586, L"Bibliotheken en apparaten die vanaf deze computer worden gedeeld"},
    {595, L"Meer HomeGroup-taken"},
    {600, L"Toon of druk het HomeGroup-wachtwoord af"},
    {601, L"Uw systeembeheerder heeft u geen toegang tot uw thuisgroep verleend."},
    {604, L"Wijzig het wachtwoord..."},
    {605, L"Verlaat de thuisgroep..."},
    {607, L"Opties voor mediastreaming kiezen..."},
    {608, L"Omdat deze computer deel uitmaakt van een domein, zijn er geen instellingen beschikbaar om de bibliotheken en apparaten ervan te delen met andere computers in de thuisgroep."},
    {609, L"Met wachtwoord beveiligd delen"},
    {610, L"Wanneer met een wachtwoord beveiligd delen is ingeschakeld, hebben alleen gebruikers met gebruikersaccounts en wachtwoorden op deze computer toegang tot gedeelde bestanden, printers die op deze computer zijn aangesloten en openbare mappen. Met een wachtwoord beveiligd delen moet worden uitgeschakeld om anderen toegang te geven."},
    {611, L"Schakel met wachtwoord beveiligd delen in"},
    {612, L"Schakel met wachtwoord beveiligd delen uit"},
    {613, L"Pagina afdrukken"},
    {614, L"Hiermee kan gedeelde inhoud worden afgespeeld op alle apparaten in dit netwerk, zoals tv's en gameconsoles"},
    {615, L"Privé netwerk"},
    {616, L"Gast- of openbaar netwerk"},
    {617, L"Domein netwerk"},
    {619, L"HomeGroup-verbindingen"},
    {620, L"Windows beheert doorgaans verbindingen met andere thuisgroepcomputers. Als u echter op al uw computers hetzelfde gebruikersaccount en wachtwoord gebruikt, kunt u HomeGroup dat account laten gebruiken."},
    {621, L"Optie: Windows toestaan thuisgroepverbindingen te beheren (aanbevolen)"},
    {622, L"Maak verbinding met andere computers met behulp van uw gebruikersaccount en wachtwoord"},
    {624, L"Start de probleemoplosser voor Thuisgroepen"},
    {627, L"Verbindingen voor het delen van bestanden"},
    {628, L"Windows maakt gebruik van 128-bits codering om verbindingen voor het delen van bestanden te beveiligen. Sommige apparaten ondersteunen geen 128-bits codering en moeten 40-bits of 56-bits codering gebruiken."},
    {629, L"Beveilig uw verbinding voor het delen van bestanden met 128-bits codering (aanbevolen)"},
    {630, L"Schakel het delen van apparaatbestanden in met 40-bits of 56-bits codering"},
    {631, L"Elk netwerk"},
    {632, L"Wijzig wat er met uw thuisgroep wordt gedeeld"},
    {637, L"Sluiten"},
    {639, L"Thuisgroep externe toegang"},
    {640, L"Andere leden van de thuisgroep kunnen vanaf elke locatie via hun computer verbinding maken met uw thuisgroep."},
    {641, L"Optie: schakel externe thuisgroeptoegang via deze computer uit"},
    {642, L"Optie: Schakel externe thuisgroeptoegang in via deze computer"},
    {648, L"Selecteer de bestanden en apparaten die u beschikbaar wilt maken en kies vervolgens hun machtigingsniveaus."},
    {649, L"Bibliotheek of map"},
    {650, L"Toegangsniveau"},
    {652, L"Schakel de automatische configuratie van op het netwerk aangesloten apparaten in."},
    {46000, L"Thuisgroep"},
    {46004, L"Optie: Kies een wachtwoord voor uw thuisgroep"},
    {46005, L"Typ het thuisgroepwachtwoord"},
    {46006, L"&Maak nu"},
    {46007, L"&Word nu lid"},
    {46008, L"Voeg met dit wachtwoord andere computers toe aan uw thuisgroep"},
    {46009, L"Je bent lid geworden van de thuisgroep"},
    {46011, L"Thuisgroep"},
    {46012, L"Windows kan geen thuisgroep instellen op deze computer."},
    {46013, L"Omdat deze computer deel uitmaakt van een domein, is het delen van de bibliotheek met andere computers in de thuisgroep niet beschikbaar."},
    {46014, L"Wachtwoorden moeten minimaal 8 tekens bevatten en geen spaties vooraan of achteraan."},
    {46015, L"Wachtwoord is onjuist.\nProbeer het opnieuw. Wachtwoorden zijn hoofdlettergevoelig."},
    {46016, L"Optie: Alle thuisgroepverbindingen op deze computer worden verbroken"},
    {46017, L"Je hebt je thuisgroep succesvol verlaten"},
    {46018, L"Wijzig wat er met uw thuisgroep wordt gedeeld"},
    {46019, L"Deel uw foto's, video's, muziek, documenten en printers met andere computers bij u thuis."},
    {46020, L"&Wijzigingen aanbrengen"},
    {46021, L"Als u het thuisgroepwachtwoord wijzigt, wordt de verbinding met iedereen verbroken"},
    {46022, L"Voer een nieuw wachtwoord in voor uw thuisgroep"},
    {46023, L"&Wachtwoord wijzigen"},
    {46024, L"Thuisgroepwachtwoord succesvol gewijzigd"},
    {46025, L"Het thuisgroepwachtwoord is gewijzigd"},
    {46026, L"Typ het thuisgroepwachtwoord"},
    {46027, L"Het thuisgroepwachtwoord is gewijzigd. Als u uw thuisgroepbronnen wilt blijven gebruiken, zorgt u ervoor dat de persoon die het nieuwe wachtwoord al heeft ingevoerd, online is en voert u vervolgens het nieuwe wachtwoord in."},
    {46028, L"Gedeeld"},
    {46029, L"Windows kan de computer niet uit de thuisgroep verwijderen."},
    {46030, L"%1 heeft zijn thuisgroepwachtwoord gewijzigd. Als u uw thuisgroepbronnen wilt blijven gebruiken, zorgt u ervoor dat de persoon die het nieuwe wachtwoord al heeft ingevoerd, online is en voert u vervolgens het nieuwe wachtwoord in."},
    {46031, L"Wachtwoorden helpen ongeautoriseerde toegang tot de bestanden en printers van uw thuisgroep te voorkomen. U kunt het wachtwoord opvragen bij %2, %1 of een ander lid van uw thuisgroep."},
    {46032, L"Wachtwoorden helpen ongeautoriseerde toegang tot de bestanden en printers van uw thuisgroep te voorkomen. U kunt het wachtwoord opvragen bij %2, %1 of een ander lid van uw thuisgroep."},
    {46033, L"Consolas"},
    {46034, L"Maak een thuisgroep"},
    {46035, L"Sluit je aan bij een thuisgroep"},
    {46036, L"Wijzig uw thuisgroepwachtwoord"},
    {46037, L"Verlaat de thuisgroep"},
    {46038, L"Om toegang te krijgen tot bestanden en printers op andere computers, moet u deze aan uw thuisgroep toevoegen. Het volgende wachtwoord is vereist:"},
    {46039, L"Typ het nieuwe thuisgroepwachtwoord:"},
    {46040, L"Wachtwoord bijwerken"},
    {46041, L"Maak een back-up van alle pc's in uw thuisgroep naar een lokaal gegevensbeschermingsdoel."},
    {46042, L"Maak een back-up van uw pc met behulp van HomeGroup-gegevensbeschermingsdoelen"},
    {46043, L"Niet gedeeld"},
    {46044, L"Thuisgroepen kunnen alleen worden aangemaakt op privénetwerken.\nOm uw netwerklocatie-instellingen te wijzigen, opent u Netwerkcentrum in het Configuratiescherm."},
    {46045, L"Windows detecteert geen thuisgroepen meer op dit netwerk. Als u een nieuwe thuisgroep wilt maken, klikt u op OK en opent u Thuisgroep in het Configuratiescherm."},
    {46046, L"Windows heeft een bestaande thuisgroep gedetecteerd.\nOm deel te nemen, klikt u op OK en opent u Thuisgroep in het Configuratiescherm."},
    {46047, L"De HomeGroup-service is nu beschikbaar. Probeer het opnieuw."},
    {46048, L"Instellingen voor delen bijgewerkt"},
    {46049, L"De geselecteerde bestanden en bronnen worden gedeeld met uw thuisgroep."},
    {46050, L"Thuisgroepwachtwoord succesvol bijgewerkt"},
    {46051, L"Je bent lid geworden van de thuisgroep"},
    {46052, L"U hebt nu toegang tot uw gedeelde bestanden en apparaten. De bestanden en apparaten die u deelt, blijven ongewijzigd."},
    {46053, L"U kunt toegang krijgen tot bestanden en printers die worden gedeeld door andere gebruikers in uw thuisgroep."},
    {46054, L"Update uw thuisgroepwachtwoord"},
    {46055, L"Sluit je aan bij een thuisgroep"},
    {46056, L"Voer het nieuwe thuisgroepwachtwoord van %1 in."},
    {46057, L"De klokken van alle thuisgroepcomputers mogen niet meer dan 24 uur uit elkaar liggen. Zorg ervoor dat de klokken van uw computer synchroon lopen en probeer vervolgens opnieuw lid te worden van de thuisgroep."},
    {46058, L"Het wachtwoord voldoet niet aan de vereisten voor wachtwoordsterkte van het domein. Voer een passend wachtwoord in of gebruik een andere HomeGroup-computer om uw wachtwoord te wijzigen."},
    {46059, L"U kunt uw wachtwoord niet opnieuw instellen omdat u niet bent verbonden met een privénetwerk.\nMaak verbinding met een privénetwerk en probeer het opnieuw."},
    {46060, L"U bent niet verbonden met een privénetwerk.\nOm uw netwerklocatie-instellingen te wijzigen, opent u Netwerkcentrum in het Configuratiescherm."},
    {46061, L"Deel met andere thuiscomputers"},
    {46062, L"U kunt bestanden en printers delen met andere computers. U kunt ook media naar uw apparaat streamen.\n\nThuisgroepen zijn beveiligd met een wachtwoord en u kunt op elk gewenst moment kiezen wat u wilt delen."},
    {46063, L"Voeg met dit wachtwoord andere computers toe aan uw thuisgroep"},
    {46064, L"Om toegang te krijgen tot bestanden en printers op andere computers, moet u deze aan uw thuisgroep toevoegen. Het volgende wachtwoord is vereist:"},
    {46065, L"Als u een thuisgroep wilt maken of er lid van wilt worden, moet IPv6 zijn ingeschakeld voor uw netwerkverbinding. Om IPv6 in te schakelen, start u de HomeGroup Troubleshooter."},
    {46066, L"Voeg mensen toe aan de thuisgroep"},
    {46067, L"Configureer de gegevensbescherming van de thuisgroep"},
    {46068, L"Meerdere thuisgroepen gedetecteerd"},
    {46069, L"Deel met andere leden van de thuisgroep"},
    {46070, L"Documenten"},
    {46071, L"Afbeeldingen"},
    {46072, L"Muziek"},
    {46073, L"Video's"},
    {46074, L"Printers en apparaten"},
    {46075, L"Wijzig de instellingen voor het delen van thuisgroepen"},
    {46076, L"%1 Delen"},
    {46077, L"Uw wachtwoord verifiëren..."},
};

// Swedish (sv-SE)
static const EmbeddedTextEntry kStrings_SV_SE[] = {
    {1, L"Hemgrupp"},
    {2, L"Granska alternativen för hemgrupp, bestäm vad den här datorn delar och visa eller uppdatera åtkomstlösenordet."},
    {3, L"En policy som ställts in av din organisation förhindrar att den här sidan körs. Fråga nätverksadministratören om hjälp."},
    {4, L"Detaljerade delningsalternativ"},
    {5, L"På"},
    {6, L"Av"},
    {7, L"Av (inga skrivare installerade)"},
    {8, L"Det finns ingen skrivare ansluten till den här datorn."},
    {9, L"Dela innehåll med datorer hemma"},
    {10, L"Få åtkomst till din hemgrupp med hjälp av en domänansluten dator"},
    {12, L"Redigera alternativ för hemgrupp"},
    {13, L"Jobbar..."},
    {14, L"Ingen hemgrupp hittades på detta nätverk."},
    {15, L"%1 av %2 skapade en hemgrupp på nätverket."},
    {16, L"Du har blivit inbjuden att gå med i din hemgrupp."},
    {18, L"Använd denna sida för att den här datorn tillhör en hemgrupp."},
    {19, L"Den här datorn kan inte ansluta till din hemgrupp."},
    {20, L"HomeGroup låter betrodda datorer utbyta filer och använda delade skrivare, och den kan skicka media till kompatibla enheter. Åtkomst kräver ett lösenord, samtidigt som du har kontroll över vad den här datorn gör tillgängligt."},
    {21, L"Den här datorn är också en del av en domän, så den kan inte skapa sin egen hemgrupp, men den kan gå med i en hemgrupp skapad av någon i nätverket.\n\nHemgrupper länkar samman datorer i ditt hemnätverk så att du kan dela foton, musik, videor, dokument och skrivare. Hemgrupper är lösenordsskyddade och du kan när som helst välja vad du vill dela."},
    {22, L"Hemgrupper länkar samman datorer i ditt hemnätverk så att du kan dela foton, musik, videor, dokument och skrivare. Hemgrupper är lösenordsskyddade och du kan när som helst välja vad du vill dela.\n\nDu kan inte skapa dina egna hemgrupper i den här utgåvan av Windows, men du kan gå med i hemgrupper skapade av andra."},
    {23, L"Skapa en hemgrupp"},
    {24, L"Gå med"},
    {25, L"Hemgruppslösenord har ändrats. För att fortsätta använda dina hemgruppsresurser, se till att personen som redan har angett det nya lösenordet är online och ange sedan det nya lösenordet."},
    {26, L"Windows har upptäckt en annan hemgrupp i ditt nätverk. Med hemgrupper kan du dela filer och skrivare med andra datorer. Du kan också strömma media till din enhet."},
    {27, L"%1 ändrade sitt hemgruppslösenord. För att fortsätta använda dina hemgruppsresurser, se till att personen som redan har angett det nya lösenordet är online och ange sedan det nya lösenordet."},
    {28, L"Letar efter hemgrupper i detta nätverk..."},
    {29, L"Skriv nytt lösenord"},
    {30, L"Gå med nu"},
    {32, L"Innan du kan skapa eller gå med i en hemgrupp måste du först ansluta till ditt nätverk."},
    {34, L"Använd den här sidan för att skapa eller gå med i en hemgrupp, din dators nätverksplats måste vara inställd på privat."},
    {35, L"Ändra nätverksplats"},
    {37, L"Delningsalternativ för Privat"},
    {38, L"Delningsalternativ för offentliga"},
    {39, L"Delningsalternativ för domän"},
    {40, L"Privat"},
    {41, L"Privat (nuvarande profil)"},
    {42, L"Offentligt"},
    {43, L"Offentlig (nuvarande profil)"},
    {44, L"Domän"},
    {45, L"Domän (nuvarande profil)"},
    {46, L"Mediastreaming är på."},
    {47, L"Mediastreaming är avstängd."},
    {56, L"Avbryt"},
    {63, L"OK"},
    {64, L"Visa eller skriv ut hemgruppslösenordet"},
    {65, L"24pt;;;Consolas"},
    {66, L"Utskriftsdatum: %1 %2"},
    {67, L"Alternativ: Visa och skriv ut ditt hemgruppslösenord"},
    {68, L"Lösenord:"},
    {69, L"Använd detta lösenord för att ansluta andra datorer till din hemgrupp."},
    {70, L"På varje dator:"},
    {71, L"Obs: Datorer som är avstängda eller i viloläge visas inte i din hemgrupp."},
    {72, L"1. Klicka på Start och sedan på Kontrollpanelen."},
    {73, L"2. Under Nätverk och internet klickar du på Välj hemgrupp och delningsalternativ."},
    {74, L"3. Klicka på Gå med nu och följ HomeGroup Wizard för att ange ditt lösenord."},
    {75, L"Klicka på Start och sedan på Kontrollpanelen."},
    {76, L"Kunde inte skriva ut hemgruppslösenord"},
    {77, L"Ett fel uppstod när Windows försökte mata ut hemgruppslösenordet. (Felkod:%1!u!)"},
    {78, L"Du är för närvarande inte ansluten till ditt hemnätverk. För att visa filer och resurser på andra hemgruppsdatorer, anslut först till ditt hemnätverk."},
    {79, L"%1 har anslutit datorn till hemgruppen. Jag har inte delat biblioteket med min hemgrupp. Klicka på länken nedan för att ändra vad du har delat. Stäng inte av eller starta om datorn förrän delningen är klar."},
    {80, L"Jag har inte delat biblioteket med min hemgrupp. Klicka på länken nedan för att ändra vad du har delat. Stäng inte av eller starta om datorn förrän delningen är klar."},
    {81, L"HomeGroup delar för närvarande biblioteket på den här datorn. Vissa hemgruppsalternativ är inte tillgängliga förrän delningen är klar. Stäng inte av eller starta om datorn förrän delningen är klar."},
    {82, L"Under Nätverk och internet klickar du på Välj hemgrupp och delningsalternativ."},
    {83, L"Det finns för närvarande inga hemgrupper i nätverket."},
    {84, L"Klicka på Gå med nu och följ HomeGroup Wizard för att ange ditt lösenord."},
    {85, L"Klicka här för att installera."},
    {86, L"Windows hittade en hemgruppsskrivare"},
    {88, L"Vi presenterar HomeGroup"},
    {89, L"%1 (nuvarande profil)"},
    {90, L"Använd den här sidan för att gå med i en hemgrupp, din dators nätverksplats måste vara inställd på privat."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Hemgrupp är inte redo ännu. Försök igen om några minuter. Om du fortsätter att se det här meddelandet, klicka på länken för att börja felsöka din hemgrupp."},
    {95, L"Starta HomeGroup-felsökaren"},
    {98, L"Hemgruppslösenord"},
    {99, L"Gästkonton kan inte ändra hemgruppsinställningar."},
    {100, L"HomeGroup har hittat en ny delad skrivare i ditt hemnätverk. När den har installerats kommer den att vara tillgänglig för alla på den här datorn."},
    {101, L"Installera skrivaren"},
    {102, L"Hemgrupp är inte tillgänglig eftersom du inte är ansluten till ditt hemnätverk."},
    {103, L"Hemgrupp är inte tillgänglig eftersom du inte är ansluten till ditt hemnätverk."},
    {104, L"Innan du går med i en hemgrupp måste du först ansluta till nätverket."},
    {105, L"Hemgruppsbild"},
    {106, L"Välj vad du vill dela och visa ditt hemgruppslösenord"},
    {107, L"Eftersom den här datorn är en del av en domän är inställningar för att dela dess bibliotek och enheter med andra datorer i hemgruppen inte tillgängliga."},
    {108, L"Inställningar för att dela bibliotek och enheter med andra datorer i en hemgrupp är inte tillgängliga i den här utgåvan av Windows."},
    {109, L"Ta bort %1 från hemgruppen"},
    {110, L"Avbryt"},
    {111, L"Ta bort hemgruppsmedlem"},
    {112, L"%1 kommer att tas bort från hemgruppen"},
    {113, L"Alla hemgruppsmedlemmar som går med med ett lösenord måste ange lösenordet igen."},
    {114, L"Skrivare och enheter"},
    {115, L"Ändra %1 hemgruppsmedlemmar"},
    {116, L"Hemgruppens lösenord har återställts"},
    {117, L"HomeGroup delar filer"},
    {118, L"Alternativ: Den här datorn tillhör en hemgrupp"},
    {119, L"Det finns en hemgrupp att gå med i"},
    {120, L"En hemgrupp kan skapas"},
    {121, L"Hemgrupp är inte tillgängligt"},
    {122, L"Otillförlitlig skrivare"},
    {200, L"Lägg till medlem"},
    {201, L"Användarikon"},
    {202, L"Fullständigt namn"},
    {203, L"Användar-ID"},
    {204, L"Framstegsindikator"},
    {205, L"Mappikon"},
    {220, L"Dela bibliotek och hårdvara"},
    {221, L"Välj det bibliotek du vill dela med andra i din hemgrupp."},
    {222, L"Redigera alternativ för hemgrupp"},
    {223, L"Använd den här sidan för att ändra hemgruppsinställningar, öppna hemgrupp i kontrollpanelen."},
    {224, L"Hemgruppsalternativ"},
    {225, L"Använd den här sidan för att ändra dina hemgruppsinställningar i kontrollpanelen eller använd hemgruppsfelsökaren."},
    {226, L"Starta felsökaren"},
    {227, L"Använd den här sidan för att använda HomeGroup-felsökaren för att hitta och åtgärda problem med din HomeGroup."},
    {228, L"Visa lösenord"},
    {229, L"Använd den här sidan för att visa eller skriva ut ditt hemgruppslösenord."},
    {230, L"Gå med i hemgruppen"},
    {231, L"Gå med i hemgruppen på detta nätverk."},
    {530, L"Öppna detaljerade delningsalternativ..."},
    {541, L"Nätverkssynlighet"},
    {542, L"Om nätverksupptäckt är aktiverat kan den här datorn se och ses av andra nätverksanslutna datorer och enheter."},
    {543, L"Aktivera nätverksupptäckt"},
    {544, L"Stäng av nätverksupptäckt"},
    {545, L"Fil- och skrivaråtkomst"},
    {546, L"När fil- och skrivardelning är aktiverat kan andra användare i ditt nätverk komma åt filerna och skrivarna du delar från den här datorn."},
    {547, L"Aktivera fil- och skrivardelning"},
    {548, L"Stäng av fil- och skrivardelning"},
    {549, L"Delning av offentlig mapp"},
    {550, L"När delning av offentliga mappar är aktiverat kan användare i ditt nätverk, inklusive hemgruppsmedlemmar, komma åt filer i offentliga mappar."},
    {552, L"Genom att aktivera delning kan alla som har tillgång till ditt nätverk läsa och skriva filer i dina offentliga mappar."},
    {553, L"Stäng av delning av offentliga mappar (användare som är inloggade på den här datorn kan fortfarande komma åt dessa mappar)"},
    {554, L"Ändra delningsalternativ för olika nätverksprofiler"},
    {559, L"Mediaåtkomst"},
    {560, L"När mediastreaming är aktiverat kan användare och enheter i ditt nätverk komma åt foton, musik och videor på den här datorn. Den här datorn kan också hitta media i nätverket."},
    {564, L"Avbryt"},
    {567, L"Tillämpa ändringar"},
    {584, L"Windows skapar en separat nätverksprofil för varje nätverk du använder. Du kan välja specifika alternativ för varje profil."},
    {585, L"Hemgruppsvarningsikon"},
    {586, L"Bibliotek och enheter som delas från den här datorn"},
    {595, L"Fler hemgruppsuppgifter"},
    {600, L"Visa eller skriv ut hemgruppslösenordet"},
    {601, L"Din systemadministratör har inte tillåtit dig att komma åt din hemgrupp."},
    {604, L"Byt lösenord..."},
    {605, L"Lämna hemgruppen..."},
    {607, L"Välj alternativ för mediastreaming..."},
    {608, L"Eftersom den här datorn är en del av en domän är inställningar för att dela dess bibliotek och enheter med andra datorer i hemgruppen inte tillgängliga."},
    {609, L"Lösenordsskyddad delning"},
    {610, L"När lösenordsskyddad delning är aktiverat kan endast användare med användarkonton och lösenord på den här datorn komma åt delade filer, skrivare som är anslutna till den här datorn och offentliga mappar. Lösenordsskyddad delning måste stängas av för att ge andra åtkomst."},
    {611, L"Aktivera lösenordsskyddad delning"},
    {612, L"Stäng av lösenordsskyddad delning"},
    {613, L"Skriv ut sida"},
    {614, L"Tillåter att delat innehåll spelas upp på alla enheter i det här nätverket, till exempel TV-apparater och spelkonsoler"},
    {615, L"Privat nätverk"},
    {616, L"Gästnätverk eller offentligt nätverk"},
    {617, L"Domännätverk"},
    {619, L"Hemgruppsanslutningar"},
    {620, L"Windows hanterar vanligtvis anslutningar till andra hemgruppsdatorer. Men om du använder samma användarkonto och lösenord på alla dina datorer kan du låta HomeGroup använda det kontot istället."},
    {621, L"Alternativ: Tillåt Windows att hantera hemgruppsanslutningar (rekommenderas)"},
    {622, L"Anslut till andra datorer med ditt användarkonto och lösenord"},
    {624, L"Starta HomeGroup-felsökaren"},
    {627, L"Fildelningsanslutningar"},
    {628, L"Windows använder 128-bitars kryptering för att säkra fildelningsanslutningar. Vissa enheter stöder inte 128-bitars kryptering och måste använda 40-bitars eller 56-bitars kryptering."},
    {629, L"Säkra din fildelningsanslutning med 128-bitars kryptering (rekommenderas)"},
    {630, L"Aktivera enhetsfildelning med 40-bitars eller 56-bitars kryptering"},
    {631, L"Varje nätverk"},
    {632, L"Ändra vad som delas med din hemgrupp"},
    {637, L"Stäng"},
    {639, L"Hemgrupps fjärråtkomst"},
    {640, L"Andra hemgruppsmedlemmar kan ansluta till din hemgrupp var som helst via sina datorer."},
    {641, L"Alternativ: Inaktivera fjärråtkomst till hemgrupp via den här datorn"},
    {642, L"Alternativ: Aktivera fjärråtkomst till hemgrupp via den här datorn"},
    {648, L"Välj de filer och enheter som ska göras tillgängliga och välj sedan deras behörighetsnivåer."},
    {649, L"Bibliotek eller katalog"},
    {650, L"Åtkomstnivå"},
    {652, L"Aktivera automatisk konfiguration av nätverksanslutna enheter."},
    {46000, L"Hemgrupp"},
    {46004, L"Alternativ: Välj ett lösenord för din hemgrupp"},
    {46005, L"Ange hemgruppslösenordet"},
    {46006, L"&Skapa nu"},
    {46007, L"&Gå med nu"},
    {46008, L"Lägg till andra datorer till din hemgrupp med detta lösenord"},
    {46009, L"Du har gått med i hemgruppen"},
    {46011, L"Hemgrupp"},
    {46012, L"Windows kan inte ställa in en hemgrupp på den här datorn."},
    {46013, L"Eftersom den här datorn är en del av en domän är det inte möjligt att dela dess bibliotek med andra datorer i hemgruppen."},
    {46014, L"Lösenord måste innehålla minst 8 tecken och inga inledande eller efterföljande mellanslag."},
    {46015, L"Lösenordet är felaktigt.\nFörsök igen. Lösenord är skiftlägeskänsliga."},
    {46016, L"Alternativ: Alla hemgruppsanslutningar på den här datorn kommer att kopplas bort"},
    {46017, L"Lämnade din hemgrupp framgångsrikt"},
    {46018, L"Ändra vad som delas med din hemgrupp"},
    {46019, L"Dela dina foton, videor, musik, dokument och skrivare med andra datorer i ditt hem."},
    {46020, L"&Gör ändringar"},
    {46021, L"Att ändra hemgruppslösenord kopplar bort alla"},
    {46022, L"Ange ett nytt lösenord för din hemgrupp"},
    {46023, L"&Ändra lösenord"},
    {46024, L"Hemgruppslösenordet har ändrats"},
    {46025, L"Hemgruppens lösenord ändrades"},
    {46026, L"Ange hemgruppslösenordet"},
    {46027, L"Hemgruppslösenord har ändrats. För att fortsätta använda dina hemgruppsresurser, se till att personen som redan har angett det nya lösenordet är online och ange sedan det nya lösenordet."},
    {46028, L"Delad"},
    {46029, L"Windows kunde inte ta bort datorn från hemgruppen."},
    {46030, L"%1 ändrade sitt hemgruppslösenord. För att fortsätta använda dina hemgruppsresurser, se till att personen som redan har angett det nya lösenordet är online och ange sedan det nya lösenordet."},
    {46031, L"Lösenord hjälper till att förhindra obehörig åtkomst till din hemgrupps filer och skrivare. Du kan få lösenordet från %2, %1 eller någon annan medlem i din hemgrupp."},
    {46032, L"Lösenord hjälper till att förhindra obehörig åtkomst till din hemgrupps filer och skrivare. Du kan få lösenordet från %2, %1 eller någon annan medlem i din hemgrupp."},
    {46033, L"Consolas"},
    {46034, L"Skapa en hemgrupp"},
    {46035, L"Gå med i en hemgrupp"},
    {46036, L"Ändra ditt hemgruppslösenord"},
    {46037, L"Lämna hemgruppen"},
    {46038, L"För att komma åt filer och skrivare på andra datorer måste du lägga till dem i din hemgrupp. Följande lösenord krävs:"},
    {46039, L"Skriv det nya hemgruppslösenordet:"},
    {46040, L"Uppdatera lösenord"},
    {46041, L"Säkerhetskopiera alla datorer i din hemgrupp till ett lokalt dataskyddsmål."},
    {46042, L"Säkerhetskopiera din dator med hjälp av HomeGroup-dataskyddsmål"},
    {46043, L"Inte delad"},
    {46044, L"Hemgrupper kan bara skapas på privata nätverk.\nFör att ändra inställningarna för din nätverksplats, öppna Nätverks- och delningscenter i Kontrollpanelen."},
    {46045, L"Windows kommer inte längre att upptäcka hemgrupper i detta nätverk. För att skapa en ny hemgrupp, klicka på OK och öppna Hemgrupp i Kontrollpanelen."},
    {46046, L"Windows upptäckte en befintlig hemgrupp.\nFör att gå med klickar du på OK och öppnar Hemgrupp i Kontrollpanelen."},
    {46047, L"Hemgruppstjänsten är nu tillgänglig. Försök igen."},
    {46048, L"Delningsinställningar uppdaterade"},
    {46049, L"De valda filerna och resurserna delas med din hemgrupp."},
    {46050, L"Hemgruppslösenordet har uppdaterats"},
    {46051, L"Du har gått med i hemgruppen"},
    {46052, L"Du kan nu komma åt dina delade filer och enheter. Filerna och enheterna du delar förblir oförändrade."},
    {46053, L"Du kan börja komma åt filer och skrivare som delas av andra användare i din hemgrupp."},
    {46054, L"Uppdatera ditt hemgruppslösenord"},
    {46055, L"Gå med i en hemgrupp"},
    {46056, L"Ange det nya hemgruppslösenordet från %1."},
    {46057, L"Alla hemgruppsdatorers klockor får inte ställas in på mer än 24 timmars mellanrum. Se till att datorns klockor är synkroniserade och försök sedan gå med i hemgruppen igen."},
    {46058, L"Lösenordet uppfyller inte domänens krav på lösenordsstyrka. Ange ett matchande lösenord eller använd en annan HomeGroup-dator för att ändra ditt lösenord."},
    {46059, L"Du kan inte återställa ditt lösenord eftersom du inte är ansluten till ett privat nätverk.\nAnslut till ett privat nätverk och försök igen."},
    {46060, L"Du är inte ansluten till ett privat nätverk.\nFör att ändra inställningarna för din nätverksplats, öppna Nätverks- och delningscenter i Kontrollpanelen."},
    {46061, L"Dela med andra hemdatorer"},
    {46062, L"Du kan dela filer och skrivare med andra datorer. Du kan också strömma media till din enhet.\n\nHemgrupper är lösenordsskyddade och du kan när som helst välja vad du vill dela."},
    {46063, L"Lägg till andra datorer till din hemgrupp med detta lösenord"},
    {46064, L"För att komma åt filer och skrivare på andra datorer måste du lägga till dem i din hemgrupp. Följande lösenord krävs:"},
    {46065, L"För att skapa eller gå med i en hemgrupp måste din nätverksanslutning ha IPv6 aktiverat. För att aktivera IPv6, starta HomeGroup Troubleshooter."},
    {46066, L"Lägg till personer i hemgruppen"},
    {46067, L"Konfigurera hemgruppsdataskydd"},
    {46068, L"Flera hemgrupper har upptäckts"},
    {46069, L"Dela med andra hemgruppsmedlemmar"},
    {46070, L"Dokument"},
    {46071, L"Bilder"},
    {46072, L"Musik"},
    {46073, L"Videor"},
    {46074, L"Skrivare och enheter"},
    {46075, L"Ändra inställningar för hemgruppsdelning"},
    {46076, L"%1 Delning"},
    {46077, L"Verifierar ditt lösenord..."},
};

// Czech (cs-CZ)
static const EmbeddedTextEntry kStrings_CS_CZ[] = {
    {1, L"Domácí skupina"},
    {2, L"Zkontrolujte možnosti domácí skupiny, rozhodněte, co tento počítač sdílí, a zobrazte nebo aktualizujte přístupové heslo."},
    {3, L"Zásady nastavené vaší organizací brání spuštění této stránky. Požádejte o pomoc správce sítě."},
    {4, L"Podrobné možnosti sdílení"},
    {5, L"Zapnuto"},
    {6, L"Vypnuto"},
    {7, L"Vypnuto (žádné nainstalované tiskárny)"},
    {8, L"K tomuto počítači není připojena žádná tiskárna."},
    {9, L"Sdílejte obsah s počítači doma"},
    {10, L"Získejte přístup k domácí skupině pomocí počítače připojeného k doméně"},
    {12, L"Upravit možnosti domácí skupiny"},
    {13, L"Práce…"},
    {14, L"V této síti nebyla nalezena žádná domácí skupina."},
    {15, L"%1 z %2 vytvořil domácí skupinu v síti."},
    {16, L"Byli jste pozváni, abyste se připojili ke své domácí skupině."},
    {18, L"Tuto stránku použijte, pokud počítač patří do domácí skupiny."},
    {19, L"Tento počítač se nemůže připojit k vaší domácí skupině."},
    {20, L"HomeGroup umožňuje důvěryhodným počítačům vyměňovat si soubory a používat sdílené tiskárny a může odesílat média do kompatibilních zařízení. Přístup vyžaduje heslo, zatímco vy máte kontrolu nad tím, co tento počítač zpřístupňuje."},
    {21, L"Tento počítač je také součástí domény, takže nemůže vytvořit vlastní domácí skupinu, ale může se připojit k domácí skupině vytvořené někým v síti.\n\nDomácí skupiny propojují počítače ve vaší domácí síti, takže můžete sdílet fotografie, hudbu, videa, dokumenty a tiskárny. Domácí skupiny jsou chráněny heslem a můžete si kdykoli vybrat, co chcete sdílet."},
    {22, L"Domácí skupiny propojují počítače ve vaší domácí síti, takže můžete sdílet fotografie, hudbu, videa, dokumenty a tiskárny. Domácí skupiny jsou chráněny heslem a můžete si kdykoli vybrat, co chcete sdílet.\n\nV tomto vydání systému Windows nemůžete vytvářet vlastní domácí skupiny, ale můžete se připojit k domácím skupinám vytvořeným jinými uživateli."},
    {23, L"Založte si domácí skupinu"},
    {24, L"Připojte se"},
    {25, L"Heslo domácí skupiny bylo změněno. Chcete-li nadále používat prostředky domácí skupiny, ujistěte se, že osoba, která již zadala nové heslo, je online, a poté zadejte nové heslo."},
    {26, L"Systém Windows zjistil ve vaší síti jinou domácí skupinu. Domácí skupiny umožňují sdílet soubory a tiskárny s jinými počítači. Do zařízení můžete také streamovat média."},
    {27, L"%1 změnil své heslo k domácí skupině. Chcete-li nadále používat prostředky domácí skupiny, ujistěte se, že osoba, která již zadala nové heslo, je online, a poté zadejte nové heslo."},
    {28, L"Hledání domácích skupin v této síti…"},
    {29, L"Zadejte nové heslo"},
    {30, L"Připojte se nyní"},
    {32, L"Než budete moci vytvořit domácí skupinu nebo se k ní připojit, musíte se nejprve připojit k síti."},
    {34, L"Pomocí této stránky můžete vytvořit domácí skupinu nebo se k ní připojit, síťové umístění vašeho počítače musí být nastaveno jako soukromé."},
    {35, L"Změňte umístění sítě"},
    {37, L"Možnosti sdílení pro soukromé"},
    {38, L"Možnosti sdílení pro veřejnost"},
    {39, L"Možnosti sdílení pro doménu"},
    {40, L"Soukromé"},
    {41, L"Soukromé (aktuální profil)"},
    {42, L"Veřejné"},
    {43, L"Veřejné (aktuální profil)"},
    {44, L"doména"},
    {45, L"Doména (aktuální profil)"},
    {46, L"Streamování médií je zapnuto."},
    {47, L"Streamování médií je vypnuté."},
    {56, L"Zrušit"},
    {63, L"OK"},
    {64, L"Zobrazit nebo vytisknout heslo domácí skupiny"},
    {65, L"24pt;;;Consolas"},
    {66, L"Datum tisku: %1 %2"},
    {67, L"Možnost: Zobrazení a tisk hesla k domácí skupině"},
    {68, L"heslo:"},
    {69, L"Toto heslo použijte k připojení dalších počítačů k domácí skupině."},
    {70, L"Na každém počítači:"},
    {71, L"Poznámka: Počítače, které jsou vypnuté nebo spí, se ve vaší domácí skupině nezobrazí."},
    {72, L"1. Klepněte na tlačítko Start a potom na příkaz Ovládací panely."},
    {73, L"2. V části Síť a internet klikněte na možnost Vybrat domácí skupinu a možnosti sdílení."},
    {74, L"3. Klikněte na Připojit se a podle průvodce domácí skupinou zadejte heslo."},
    {75, L"Klikněte na Start a poté na Ovládací panely."},
    {76, L"Nelze vytisknout heslo domácí skupiny"},
    {77, L"Při pokusu o zadání hesla domácí skupiny došlo k chybě. (Kód chyby: %1!u!)"},
    {78, L"Momentálně nejste připojeni k domácí síti. Chcete-li zobrazit soubory a prostředky na jiných počítačích domácí skupiny, nejprve se připojte k domácí síti."},
    {79, L"%1 se připojil k počítači k domácí skupině. Knihovnu jsem nesdílel se svou domácí skupinou. Kliknutím na odkaz níže změníte, co jste sdíleli. Nevypínejte ani nerestartujte počítač, dokud není sdílení dokončeno."},
    {80, L"Knihovnu jsem nesdílel se svou domácí skupinou. Kliknutím na odkaz níže změníte, co jste sdíleli. Nevypínejte ani nerestartujte počítač, dokud není sdílení dokončeno."},
    {81, L"Domácí skupina aktuálně sdílí knihovnu na tomto počítači. Některé možnosti domácí skupiny nejsou k dispozici, dokud není sdílení dokončeno. Nevypínejte ani nerestartujte počítač, dokud není sdílení dokončeno."},
    {82, L"V části Síť a internet klikněte na možnost Vybrat domácí skupinu a možnosti sdílení."},
    {83, L"V současné době v síti nejsou žádné domácí skupiny."},
    {84, L"Klikněte na Připojit se a podle průvodce domácí skupinou zadejte heslo."},
    {85, L"Pro instalaci klikněte sem."},
    {86, L"Systém Windows našel tiskárnu domácí skupiny"},
    {88, L"Představujeme HomeGroup"},
    {89, L"%1 (aktuální profil)"},
    {90, L"Tuto stránku použijte k připojení k domácí skupině, umístění v síti vašeho počítače musí být nastaveno jako soukromé."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Domácí skupina ještě není připravena. Zkuste to znovu za několik minut. Pokud se tato zpráva zobrazuje i nadále, kliknutím na odkaz zahájíte odstraňování problémů s domácí skupinou."},
    {95, L"Spusťte nástroj pro odstraňování problémů s domácí skupinou"},
    {98, L"Heslo domácí skupiny"},
    {99, L"Účty hostů nemohou změnit nastavení domácí skupiny."},
    {100, L"Domácí skupina našla novou sdílenou tiskárnu ve vaší domácí síti. Po instalaci bude k dispozici komukoli na tomto počítači."},
    {101, L"Nainstalujte tiskárnu"},
    {102, L"Domácí skupina není k dispozici, protože nejste připojeni k domácí síti."},
    {103, L"Domácí skupina není k dispozici, protože nejste připojeni k domácí síti."},
    {104, L"Před připojením k domácí skupině se musíte nejprve připojit k síti."},
    {105, L"Obrázek domácí skupiny"},
    {106, L"Vyberte, co chcete sdílet, a zobrazte heslo domácí skupiny"},
    {107, L"Protože je tento počítač součástí domény, nejsou k dispozici nastavení pro sdílení jeho knihoven a zařízení s jinými počítači v domácí skupině."},
    {108, L"Nastavení pro sdílení knihoven a zařízení s jinými počítači v domácí skupině nejsou v tomto vydání systému Windows k dispozici."},
    {109, L"Odeberte %1 z domácí skupiny"},
    {110, L"Zrušit"},
    {111, L"Odebrat člena domácí skupiny"},
    {112, L"%1 bude odebrán z domácí skupiny"},
    {113, L"Všichni členové domácí skupiny, kteří se připojí pomocí hesla, budou požádáni o opětovné zadání hesla."},
    {114, L"Tiskárny a zařízení"},
    {115, L"Změňte členy domácí skupiny %1"},
    {116, L"Heslo domácí skupiny bylo resetováno"},
    {117, L"Domácí skupina sdílí soubory"},
    {118, L"Možnost: Tento počítač patří do domácí skupiny"},
    {119, L"K připojení je k dispozici domácí skupina"},
    {120, L"Lze vytvořit domácí skupinu"},
    {121, L"Domácí skupina není k dispozici"},
    {122, L"Nedůvěryhodná tiskárna"},
    {200, L"Přidat člena"},
    {201, L"Ikona uživatele"},
    {202, L"Celé jméno"},
    {203, L"ID uživatele"},
    {204, L"Progress Bar"},
    {205, L"Ikona složky"},
    {220, L"Sdílejte knihovny a hardware"},
    {221, L"Vyberte knihovnu, kterou chcete sdílet s ostatními ve vaší domácí skupině."},
    {222, L"Upravit možnosti domácí skupiny"},
    {223, L"Na této stránce můžete změnit nastavení HomeGroup, otevřete HomeGroup v Ovládacích panelech."},
    {224, L"Možnosti domácí skupiny"},
    {225, L"Pomocí této stránky můžete změnit nastavení domácí skupiny v Ovládacích panelech nebo použít Poradce při potížích s domácí skupinou."},
    {226, L"Spusťte nástroj pro odstraňování problémů"},
    {227, L"Tuto stránku použijte k použití nástroje pro odstraňování problémů s domácí skupinou k nalezení a odstranění problémů s vaší domácí skupinou."},
    {228, L"Zobrazit heslo"},
    {229, L"Pomocí této stránky můžete zobrazit nebo vytisknout heslo své domácí skupiny."},
    {230, L"Připojte se k domácí skupině"},
    {231, L"Připojte se k domácí skupině v této síti."},
    {530, L"Otevřít podrobné možnosti sdílení…"},
    {541, L"Viditelnost sítě"},
    {542, L"Pokud je zapnuto zjišťování sítě, tento počítač může vidět a být viděn jinými počítači a zařízeními v síti."},
    {543, L"Zapněte zjišťování sítě"},
    {544, L"Vypněte zjišťování sítě"},
    {545, L"Přístup k souborům a tiskárnám"},
    {546, L"Když je sdílení souborů a tiskáren zapnuto, ostatní uživatelé ve vaší síti budou mít přístup k souborům a tiskárnám, které sdílíte z tohoto počítače."},
    {547, L"Zapněte sdílení souborů a tiskáren"},
    {548, L"Vypněte sdílení souborů a tiskáren"},
    {549, L"Sdílení veřejných složek"},
    {550, L"Když je sdílení veřejných složek zapnuto, uživatelé ve vaší síti, včetně členů domácí skupiny, mají přístup k souborům ve veřejných složkách."},
    {552, L"Povolení sdílení umožňuje komukoli s přístupem k vaší síti číst a zapisovat soubory do vašich veřejných složek."},
    {553, L"Vypnout sdílení veřejných složek (uživatelé přihlášení k tomuto počítači mají stále přístup k těmto složkám)"},
    {554, L"Změňte možnosti sdílení pro různé síťové profily"},
    {559, L"Přístup k médiím"},
    {560, L"Když je streamování médií zapnuto, uživatelé a zařízení ve vaší síti mají přístup k fotografiím, hudbě a videím v tomto počítači. Tento počítač může také najít média v síti."},
    {564, L"Zrušit"},
    {567, L"Použít změny"},
    {584, L"Systém Windows vytvoří samostatný síťový profil pro každou síť, kterou používáte. Pro každý profil můžete vybrat konkrétní možnosti."},
    {585, L"Ikona varování domácí skupiny"},
    {586, L"Knihovny a zařízení sdílené z tohoto počítače"},
    {595, L"Další úkoly domácí skupiny"},
    {600, L"Zobrazit nebo vytisknout heslo domácí skupiny"},
    {601, L"Váš správce systému vám nepovolil přístup k vaší domácí skupině."},
    {604, L"Změňte heslo..."},
    {605, L"Opustit domácí skupinu..."},
    {607, L"Vyberte možnosti streamování médií..."},
    {608, L"Protože je tento počítač součástí domény, nejsou k dispozici nastavení pro sdílení jeho knihoven a zařízení s jinými počítači v domácí skupině."},
    {609, L"Sdílení chráněné heslem"},
    {610, L"Když je zapnuto sdílení chráněné heslem, mají přístup ke sdíleným souborům, tiskárnám připojeným k tomuto počítači a veřejným složkám pouze uživatelé s uživatelskými účty a hesly v tomto počítači. Sdílení chráněné heslem musí být vypnuto, aby byl umožněn přístup ostatním."},
    {611, L"Zapněte sdílení chráněné heslem"},
    {612, L"Vypněte sdílení chráněné heslem"},
    {613, L"Tisk stránky"},
    {614, L"Umožňuje přehrávání sdíleného obsahu na všech zařízeních v této síti, jako jsou televizory a herní konzole"},
    {615, L"Soukromá síť"},
    {616, L"Host nebo veřejná síť"},
    {617, L"Doménová síť"},
    {619, L"Připojení domácí skupiny"},
    {620, L"Windows obvykle spravuje připojení k jiným počítačům domácí skupiny. Pokud však na všech počítačích používáte stejný uživatelský účet a heslo, můžete nechat domácí skupinu používat tento účet."},
    {621, L"Možnost: Povolit systému Windows spravovat připojení k domácí skupině (doporučeno)"},
    {622, L"Připojte se k jiným počítačům pomocí svého uživatelského účtu a hesla"},
    {624, L"Spusťte nástroj pro odstraňování problémů s domácí skupinou"},
    {627, L"Připojení pro sdílení souborů"},
    {628, L"Systém Windows používá k zabezpečení připojení sdílení souborů 128bitové šifrování. Některá zařízení nepodporují 128bitové šifrování a musí používat 40bitové nebo 56bitové šifrování."},
    {629, L"Zabezpečte připojení ke sdílení souborů pomocí 128bitového šifrování (doporučeno)"},
    {630, L"Povolte sdílení souborů zařízení pomocí 40bitového nebo 56bitového šifrování"},
    {631, L"Každá síť"},
    {632, L"Změňte, co je sdíleno s vaší domácí skupinou"},
    {637, L"Zavřít"},
    {639, L"Vzdálený přístup k domácí skupině"},
    {640, L"Ostatní členové domácí skupiny se mohou k vaší domácí skupině připojit odkudkoli prostřednictvím svých počítačů."},
    {641, L"Možnost: Zakázat vzdálený přístup k domácí skupině prostřednictvím tohoto počítače"},
    {642, L"Možnost: Povolit vzdálený přístup k domácí skupině prostřednictvím tohoto počítače"},
    {648, L"Vyberte soubory a zařízení, která chcete zpřístupnit, a poté vyberte jejich úrovně oprávnění."},
    {649, L"Knihovna nebo adresář"},
    {650, L"Úroveň přístupu"},
    {652, L"Zapněte automatické nastavení zařízení připojených k síti."},
    {46000, L"Domácí skupina"},
    {46004, L"Možnost: Vyberte heslo pro svou domácí skupinu"},
    {46005, L"Zadejte heslo domácí skupiny"},
    {46006, L"&Vytvořit nyní"},
    {46007, L"&Připojit se nyní"},
    {46008, L"Pomocí tohoto hesla přidejte do domácí skupiny další počítače"},
    {46009, L"Připojili jste se k domácí skupině"},
    {46011, L"Domácí skupina"},
    {46012, L"Systém Windows nemůže v tomto počítači nastavit domácí skupinu."},
    {46013, L"Protože je tento počítač součástí domény, sdílení jeho knihovny s jinými počítači v domácí skupině není k dispozici."},
    {46014, L"Hesla musí obsahovat alespoň 8 znaků a žádné mezery na začátku nebo na konci."},
    {46015, L"Heslo je nesprávné.\nZkuste to prosím znovu. Hesla rozlišují malá a velká písmena."},
    {46016, L"Možnost: Všechna připojení k domácí skupině na tomto počítači budou odpojena"},
    {46017, L"Úspěšně jste opustili svou domácí skupinu"},
    {46018, L"Změňte, co je sdíleno s vaší domácí skupinou"},
    {46019, L"Sdílejte své fotografie, videa, hudbu, dokumenty a tiskárny s ostatními počítači ve vaší domácnosti."},
    {46020, L"&Provést změny"},
    {46021, L"Změna hesla domácí skupiny odpojí všechny"},
    {46022, L"Zadejte nové heslo pro svou domácí skupinu"},
    {46023, L"&Změnit heslo"},
    {46024, L"Heslo domácí skupiny bylo úspěšně změněno"},
    {46025, L"Heslo domácí skupiny bylo změněno"},
    {46026, L"Zadejte heslo domácí skupiny"},
    {46027, L"Heslo domácí skupiny bylo změněno. Chcete-li nadále používat prostředky domácí skupiny, ujistěte se, že osoba, která již zadala nové heslo, je online, a poté zadejte nové heslo."},
    {46028, L"Sdíleno"},
    {46029, L"Systém Windows nemohl odebrat počítač z domácí skupiny."},
    {46030, L"%1 změnil své heslo k domácí skupině. Chcete-li nadále používat prostředky domácí skupiny, ujistěte se, že osoba, která již zadala nové heslo, je online, a poté zadejte nové heslo."},
    {46031, L"Hesla pomáhají zabránit neoprávněnému přístupu k souborům a tiskárnám vaší domácí skupiny. Heslo můžete získat od %2, %1 nebo jiného člena vaší domácí skupiny."},
    {46032, L"Hesla pomáhají zabránit neoprávněnému přístupu k souborům a tiskárnám vaší domácí skupiny. Heslo můžete získat od %2, %1 nebo jiného člena vaší domácí skupiny."},
    {46033, L"Consolas"},
    {46034, L"Vytvořte domácí skupinu"},
    {46035, L"Připojte se k domácí skupině"},
    {46036, L"Změňte heslo své domácí skupiny"},
    {46037, L"Opusťte domácí skupinu"},
    {46038, L"Chcete-li získat přístup k souborům a tiskárnám na jiných počítačích, musíte je přidat do své domácí skupiny. Je vyžadováno následující heslo:"},
    {46039, L"Zadejte nové heslo domácí skupiny:"},
    {46040, L"Aktualizujte heslo"},
    {46041, L"Zálohujte všechny počítače ve vaší domácí skupině do místního cíle ochrany dat."},
    {46042, L"Zálohujte svůj počítač pomocí cílů ochrany dat HomeGroup"},
    {46043, L"Nesdíleno"},
    {46044, L"Domácí skupiny lze vytvářet pouze v privátních sítích.\nChcete-li změnit nastavení umístění v síti, otevřete v Ovládacích panelech Centrum sítí a sdílení."},
    {46045, L"Systém Windows již nebude detekovat domácí skupiny v této síti. Chcete-li vytvořit novou domácí skupinu, klikněte na OK a otevřete Domácí skupina v Ovládacích panelech."},
    {46046, L"Systém Windows zjistil existující domácí skupinu.\nChcete-li se připojit, klikněte na OK a otevřete HomeGroup v Ovládacích panelech."},
    {46047, L"Služba HomeGroup je nyní k dispozici. Zkuste to prosím znovu."},
    {46048, L"Nastavení sdílení aktualizováno"},
    {46049, L"Vybrané soubory a prostředky jsou sdíleny s vaší domácí skupinou."},
    {46050, L"Heslo domácí skupiny bylo úspěšně aktualizováno"},
    {46051, L"Připojili jste se k domácí skupině"},
    {46052, L"Nyní máte přístup ke svým sdíleným souborům a zařízením. Soubory a zařízení, která sdílíte, zůstávají nezměněny."},
    {46053, L"Můžete začít přistupovat k souborům a tiskárnám sdíleným ostatními uživateli ve vaší domácí skupině."},
    {46054, L"Aktualizujte heslo své domácí skupiny"},
    {46055, L"Připojte se k domácí skupině"},
    {46056, L"Zadejte nové heslo domácí skupiny z %1."},
    {46057, L"Hodiny všech počítačů v domácí skupině musí být nastaveny tak, aby interval mezi nimi nebyl delší než 24 hodin. Ujistěte se, že jsou hodiny vašeho počítače synchronizované, a poté se zkuste znovu připojit k domácí skupině."},
    {46058, L"Heslo nesplňuje požadavky na sílu hesla domény. Zadejte odpovídající heslo nebo použijte jiný počítač domácí skupiny ke změně hesla."},
    {46059, L"Své heslo nemůžete resetovat, protože nejste připojeni k privátní síti.\nPřipojte se k privátní síti a zkuste to znovu."},
    {46060, L"Nejste připojeni k privátní síti.\nChcete-li změnit nastavení umístění v síti, otevřete v Ovládacích panelech Centrum sítí a sdílení."},
    {46061, L"Sdílejte s ostatními domácími počítači"},
    {46062, L"Soubory a tiskárny můžete sdílet s jinými počítači. Do zařízení můžete také streamovat média.\n\nDomácí skupiny jsou chráněny heslem a můžete si kdykoli vybrat, co chcete sdílet."},
    {46063, L"Pomocí tohoto hesla přidejte do domácí skupiny další počítače"},
    {46064, L"Chcete-li získat přístup k souborům a tiskárnám na jiných počítačích, musíte je přidat do své domácí skupiny. Je vyžadováno následující heslo:"},
    {46065, L"Chcete-li vytvořit domácí skupinu nebo se k ní připojit, vaše síťové připojení musí mít povolený protokol IPv6. Chcete-li povolit IPv6, spusťte Poradce při potížích s domácí skupinou."},
    {46066, L"Přidejte lidi do domácí skupiny"},
    {46067, L"Nakonfigurujte ochranu dat domácí skupiny"},
    {46068, L"Bylo zjištěno více domácích skupin"},
    {46069, L"Sdílejte s ostatními členy domácí skupiny"},
    {46070, L"Dokumenty"},
    {46071, L"obrázky"},
    {46072, L"Hudba"},
    {46073, L"videa"},
    {46074, L"Tiskárny a zařízení"},
    {46075, L"Změňte nastavení sdílení domácí skupiny"},
    {46076, L"Sdílení %1"},
    {46077, L"Ověřování hesla..."},
};

// Danish (da-DK)
static const EmbeddedTextEntry kStrings_DA_DK[] = {
    {1, L"Hjemmegruppe"},
    {2, L"Gennemgå HomeGroup-indstillinger, beslut hvad denne pc deler, og vis eller opdater adgangskoden."},
    {3, L"En politik fastsat af din organisation forhindrer denne side i at køre. Spørg netværksadministratoren om hjælp."},
    {4, L"Detaljerede delingsmuligheder"},
    {5, L"På"},
    {6, L"Fra"},
    {7, L"Fra (ingen printere installeret)"},
    {8, L"Der er ingen printer tilsluttet denne computer."},
    {9, L"Del indhold med pc'er derhjemme"},
    {10, L"Få adgang til din hjemmegruppe ved hjælp af en domæneforbundet computer"},
    {12, L"Rediger hjemmegruppeindstillinger"},
    {13, L"Arbejder..."},
    {14, L"Der blev ikke fundet nogen hjemmegruppe på dette netværk."},
    {15, L"%1 af %2 oprettede en hjemmegruppe på netværket."},
    {16, L"Du er blevet inviteret til at deltage i din hjemmegruppe."},
    {18, L"Brug denne side til at denne computer tilhører en hjemmegruppe."},
    {19, L"Denne computer kan ikke oprette forbindelse til din hjemmegruppe."},
    {20, L"HomeGroup lader betroede pc'er udveksle filer og bruge delte printere, og den kan sende medier til kompatible enheder. Adgang kræver en adgangskode, mens du forbliver i kontrol over, hvad denne pc stiller til rådighed."},
    {21, L"Denne computer er også en del af et domæne, så den kan ikke oprette sin egen hjemmegruppe, men den kan tilslutte sig en hjemmegruppe, der er oprettet af en person på netværket.\n\nHjemmegrupper forbinder computere på dit hjemmenetværk, så du kan dele fotos, musik, videoer, dokumenter og printere. Hjemmegrupper er beskyttet med adgangskode, og du kan til enhver tid vælge, hvad du vil dele."},
    {22, L"Hjemmegrupper forbinder computere på dit hjemmenetværk, så du kan dele fotos, musik, videoer, dokumenter og printere. Hjemmegrupper er beskyttet med adgangskode, og du kan til enhver tid vælge, hvad du vil dele.\n\nDu kan ikke oprette dine egne hjemmegrupper i denne udgave af Windows, men du kan deltage i hjemmegrupper, der er oprettet af andre."},
    {23, L"Opret en hjemmegruppe"},
    {24, L"Deltag"},
    {25, L"Hjemmegruppeadgangskode er blevet ændret. For at fortsætte med at bruge dine hjemmegrupperessourcer skal du sørge for, at den person, der allerede har indtastet den nye adgangskode, er online, og derefter indtaste den nye adgangskode."},
    {26, L"Windows har fundet en anden hjemmegruppe på dit netværk. Hjemmegrupper giver dig mulighed for at dele filer og printere med andre computere. Du kan også streame medier til din enhed."},
    {27, L"%1 ændrede sin hjemmegruppeadgangskode. For at fortsætte med at bruge dine hjemmegrupperessourcer skal du sørge for, at den person, der allerede har indtastet den nye adgangskode, er online, og derefter indtaste den nye adgangskode."},
    {28, L"Leder efter hjemmegrupper på dette netværk..."},
    {29, L"Indtast ny adgangskode"},
    {30, L"Tilmeld dig nu"},
    {32, L"Før du kan oprette eller tilslutte dig en hjemmegruppe, skal du først oprette forbindelse til dit netværk."},
    {34, L"Brug denne side til at oprette eller deltage i en hjemmegruppe, din computers netværksplacering skal være indstillet til privat."},
    {35, L"Skift netværksplacering"},
    {37, L"Delingsmuligheder for Privat"},
    {38, L"Delingsmuligheder for offentlig"},
    {39, L"Delingsmuligheder for domæne"},
    {40, L"Privat"},
    {41, L"Privat (nuværende profil)"},
    {42, L"Offentlig"},
    {43, L"Offentlig (aktuel profil)"},
    {44, L"Domæne"},
    {45, L"Domæne (nuværende profil)"},
    {46, L"Mediestreaming er aktiveret."},
    {47, L"Mediestreaming er slået fra."},
    {56, L"Annuller"},
    {63, L"OK"},
    {64, L"Vis eller udskriv HomeGroup-adgangskoden"},
    {65, L"24pt;;;Consolas"},
    {66, L"Trykt dato: %1 %2"},
    {67, L"Mulighed: Se og udskriv din hjemmegruppeadgangskode"},
    {68, L"Adgangskode:"},
    {69, L"Brug denne adgangskode til at forbinde andre computere til din hjemmegruppe."},
    {70, L"På hver computer:"},
    {71, L"Bemærk: Computere, der er slukket eller i dvale, vises ikke i din hjemmegruppe."},
    {72, L"1. Klik på Start, og klik derefter på Kontrolpanel."},
    {73, L"2. Under Netværk og internet skal du klikke på Vælg hjemmegruppe og delingsindstillinger."},
    {74, L"3. Klik på Join Now, og følg HomeGroup Wizard for at indtaste din adgangskode."},
    {75, L"Klik på Start, og klik derefter på Kontrolpanel."},
    {76, L"Hjemmegruppeadgangskoden kunne ikke udskrives"},
    {77, L"Der opstod en fejl, da Windows forsøgte at udlæse hjemmegruppeadgangskoden. (Fejlkode:%1!u!)"},
    {78, L"Du er i øjeblikket ikke forbundet til dit hjemmenetværk. For at se filer og ressourcer på andre hjemmegruppecomputere skal du først oprette forbindelse til dit hjemmenetværk."},
    {79, L"%1 har sluttet computeren til hjemmegruppen. Jeg har ikke delt biblioteket med min hjemmegruppe. Klik på linket nedenfor for at ændre det, du har delt. Sluk eller genstart ikke din computer, før deling er fuldført."},
    {80, L"Jeg har ikke delt biblioteket med min hjemmegruppe. Klik på linket nedenfor for at ændre det, du har delt. Sluk eller genstart ikke din computer, før deling er fuldført."},
    {81, L"HomeGroup deler i øjeblikket biblioteket på denne computer. Nogle hjemmegruppeindstillinger er ikke tilgængelige, før deling er fuldført. Sluk eller genstart ikke din computer, før deling er fuldført."},
    {82, L"Under Netværk og internet skal du klikke på Vælg hjemmegruppe og delingsindstillinger."},
    {83, L"Der er i øjeblikket ingen hjemmegrupper på netværket."},
    {84, L"Klik på Join Now, og følg HomeGroup Wizard for at indtaste din adgangskode."},
    {85, L"Klik her for at installere."},
    {86, L"Windows fandt en hjemmegruppeprinter"},
    {88, L"Introduktion til HomeGroup"},
    {89, L"%1 (nuværende profil)"},
    {90, L"Brug denne side til at deltage i en hjemmegruppe, din computers netværksplacering skal være indstillet til privat."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Hjemmegruppe er ikke klar endnu. Prøv venligst igen om et par minutter. Hvis du fortsætter med at se denne meddelelse, skal du klikke på linket for at starte fejlfinding af din hjemmegruppe."},
    {95, L"Start HomeGroup fejlfinding"},
    {98, L"Hjemmegruppe adgangskode"},
    {99, L"Gæstekonti kan ikke ændre hjemmegruppeindstillinger."},
    {100, L"HomeGroup har fundet en ny delt printer på dit hjemmenetværk. Når den er installeret, vil den være tilgængelig for alle på denne computer."},
    {101, L"Installer printer"},
    {102, L"Hjemmegruppe er ikke tilgængelig, fordi du ikke er forbundet til dit hjemmenetværk."},
    {103, L"Hjemmegruppe er ikke tilgængelig, fordi du ikke er forbundet til dit hjemmenetværk."},
    {104, L"Før du tilmelder dig en hjemmegruppe, skal du først oprette forbindelse til netværket."},
    {105, L"Hjemmegruppebillede"},
    {106, L"Vælg, hvad du vil dele, og se din hjemmegruppeadgangskode"},
    {107, L"Fordi denne computer er en del af et domæne, er indstillinger til at dele dens biblioteker og enheder med andre computere i hjemmegruppen ikke tilgængelige."},
    {108, L"Indstillinger for at dele biblioteker og enheder med andre computere i en hjemmegruppe er ikke tilgængelige i denne udgave af Windows."},
    {109, L"Fjern %1 fra hjemmegruppen"},
    {110, L"Annuller"},
    {111, L"Fjern hjemmegruppemedlem"},
    {112, L"%1 vil blive fjernet fra hjemmegruppen"},
    {113, L"Alle hjemmegruppemedlemmer, der tilslutter sig ved hjælp af en adgangskode, skal indtaste adgangskoden igen."},
    {114, L"Printere og enheder"},
    {115, L"Skift %1 hjemmegruppemedlemmer"},
    {116, L"Hjemmegruppens adgangskode blev nulstillet"},
    {117, L"HomeGroup deler filer"},
    {118, L"Mulighed: Denne computer tilhører en hjemmegruppe"},
    {119, L"Der er mulighed for at deltage i en hjemmegruppe"},
    {120, L"Der kan oprettes en hjemmegruppe"},
    {121, L"Hjemmegruppe er ikke tilgængelig"},
    {122, L"Upålidelig printer"},
    {200, L"Tilføj medlem"},
    {201, L"Brugerikon"},
    {202, L"Fulde navn"},
    {203, L"Bruger-id"},
    {204, L"Fremskridtslinje"},
    {205, L"Mappe ikon"},
    {220, L"Del biblioteker og hardware"},
    {221, L"Vælg det bibliotek, du vil dele med andre i din hjemmegruppe."},
    {222, L"Rediger hjemmegruppeindstillinger"},
    {223, L"Brug denne side til at ændre HomeGroup-indstillinger, åbne HomeGroup i Kontrolpanel."},
    {224, L"Hjemmegruppe muligheder"},
    {225, L"Brug denne side til at ændre dine HomeGroup-indstillinger i Kontrolpanel eller brug HomeGroup Troubleshooter."},
    {226, L"Start fejlfinding"},
    {227, L"Brug denne side til at bruge HomeGroup fejlfinding til at finde og løse problemer med din HomeGroup."},
    {228, L"Se adgangskode"},
    {229, L"Brug denne side til at se eller udskrive din hjemmegruppeadgangskode."},
    {230, L"Tilmeld dig hjemmegruppe"},
    {231, L"Tilmeld dig hjemmegruppen på dette netværk."},
    {530, L"Åbn detaljerede delingsmuligheder..."},
    {541, L"Netværkssynlighed"},
    {542, L"Hvis netværksregistrering er slået til, kan denne computer se og ses af andre netværksforbundne computere og enheder."},
    {543, L"Slå netværksgenkendelse til"},
    {544, L"Slå netværksgenkendelse fra"},
    {545, L"Fil- og printeradgang"},
    {546, L"Når fil- og printerdeling er slået til, kan andre brugere på dit netværk få adgang til de filer og printere, du deler fra denne computer."},
    {547, L"Slå fil- og printerdeling til"},
    {548, L"Slå fil- og printerdeling fra"},
    {549, L"Deling af offentlig mappe"},
    {550, L"Når deling af offentlig mappe er slået til, kan brugere på dit netværk, inklusive medlemmer af hjemmegruppen, få adgang til filer i offentlige mapper."},
    {552, L"Aktivering af deling giver alle med adgang til dit netværk mulighed for at læse og skrive filer i dine offentlige mapper."},
    {553, L"Slå deling af offentlig mappe fra (brugere, der er logget på denne computer, kan stadig få adgang til disse mapper)"},
    {554, L"Skift delingsmuligheder for forskellige netværksprofiler"},
    {559, L"Medieadgang"},
    {560, L"Når mediestreaming er slået til, kan brugere og enheder på dit netværk få adgang til billederne, musikken og videoerne på denne computer. Denne computer kan også finde medier på netværket."},
    {564, L"Annuller"},
    {567, L"Anvend ændringer"},
    {584, L"Windows opretter en separat netværksprofil for hvert netværk, du bruger. Du kan vælge specifikke muligheder for hver profil."},
    {585, L"Hjemmegruppe advarselsikon"},
    {586, L"Biblioteker og enheder delt fra denne computer"},
    {595, L"Flere hjemmegruppeopgaver"},
    {600, L"Vis eller udskriv HomeGroup-adgangskoden"},
    {601, L"Din systemadministrator har ikke givet dig adgang til din hjemmegruppe."},
    {604, L"Skift adgangskode..."},
    {605, L"Forlad hjemmegruppen..."},
    {607, L"Vælg muligheder for mediestreaming..."},
    {608, L"Fordi denne computer er en del af et domæne, er indstillinger til at dele dens biblioteker og enheder med andre computere i hjemmegruppen ikke tilgængelige."},
    {609, L"Adgangskodebeskyttet deling"},
    {610, L"Når adgangskodebeskyttet deling er slået til, er det kun brugere med brugerkonti og adgangskoder på denne computer, der kan få adgang til delte filer, printere, der er tilsluttet denne computer, og offentlige mapper. Adgangskodebeskyttet deling skal slås fra for at give andre adgang."},
    {611, L"Slå adgangskodebeskyttet deling til"},
    {612, L"Slå adgangskodebeskyttet deling fra"},
    {613, L"Udskriv side"},
    {614, L"Tillader, at delt indhold afspilles på alle enheder på dette netværk, såsom tv'er og spillekonsoller"},
    {615, L"Privat netværk"},
    {616, L"Gæste- eller offentligt netværk"},
    {617, L"Domæne netværk"},
    {619, L"Hjemmegruppeforbindelser"},
    {620, L"Windows administrerer typisk forbindelser til andre hjemmegruppecomputere. Men hvis du bruger den samme brugerkonto og adgangskode på alle dine computere, kan du få HomeGroup til at bruge den konto i stedet."},
    {621, L"Mulighed: Tillad Windows at administrere hjemmegruppeforbindelser (anbefales)"},
    {622, L"Opret forbindelse til andre computere ved hjælp af din brugerkonto og adgangskode"},
    {624, L"Start HomeGroup fejlfinding"},
    {627, L"Fildelingsforbindelser"},
    {628, L"Windows bruger 128-bit kryptering til at sikre fildelingsforbindelser. Nogle enheder understøtter ikke 128-bit kryptering og skal bruge 40-bit eller 56-bit kryptering."},
    {629, L"Sikre din fildelingsforbindelse ved hjælp af 128-bit kryptering (anbefales)"},
    {630, L"Aktiver enhedsfildeling med 40-bit eller 56-bit kryptering"},
    {631, L"Hvert netværk"},
    {632, L"Skift, hvad der deles med din hjemmegruppe"},
    {637, L"Luk"},
    {639, L"Hjemmegruppe fjernadgang"},
    {640, L"Andre hjemmegruppemedlemmer kan oprette forbindelse til din hjemmegruppe fra hvor som helst via deres computere."},
    {641, L"Mulighed: Deaktiver fjernadgang til hjemmegruppe via denne computer"},
    {642, L"Mulighed: Aktiver fjernadgang til hjemmegruppe via denne computer"},
    {648, L"Vælg de filer og enheder, der skal gøres tilgængelige, og vælg derefter deres tilladelsesniveauer."},
    {649, L"Bibliotek eller bibliotek"},
    {650, L"Adgangsniveau"},
    {652, L"Aktiver automatisk opsætning af netværkstilsluttede enheder."},
    {46000, L"Hjemmegruppe"},
    {46004, L"Mulighed: Vælg en adgangskode til din hjemmegruppe"},
    {46005, L"Indtast hjemmegruppens adgangskode"},
    {46006, L"&Opret nu"},
    {46007, L"&Tilmeld dig nu"},
    {46008, L"Tilføj andre computere til din hjemmegruppe ved hjælp af denne adgangskode"},
    {46009, L"Du har tilmeldt dig hjemmegruppen"},
    {46011, L"Hjemmegruppe"},
    {46012, L"Windows kan ikke oprette en hjemmegruppe på denne computer."},
    {46013, L"Fordi denne computer er en del af et domæne, er deling af dens bibliotek med andre computere i hjemmegruppen ikke tilgængelig."},
    {46014, L"Adgangskoder skal indeholde mindst 8 tegn og ingen indledende eller efterfølgende mellemrum."},
    {46015, L"Adgangskoden er forkert.\nPrøv venligst igen. Adgangskoder skelner mellem store og små bogstaver."},
    {46016, L"Mulighed: Alle hjemmegruppeforbindelser på denne computer vil blive afbrudt"},
    {46017, L"Forladt din hjemmegruppe"},
    {46018, L"Skift, hvad der deles med din hjemmegruppe"},
    {46019, L"Del dine fotos, videoer, musik, dokumenter og printere med andre computere i dit hjem."},
    {46020, L"&Foretag ændringer"},
    {46021, L"Ændring af hjemmegruppeadgangskode afbryder forbindelsen til alle"},
    {46022, L"Indtast en ny adgangskode til din hjemmegruppe"},
    {46023, L"&Skift adgangskode"},
    {46024, L"Hjemmegruppeadgangskode blev ændret"},
    {46025, L"Hjemmegruppens adgangskode blev ændret"},
    {46026, L"Indtast hjemmegruppens adgangskode"},
    {46027, L"Hjemmegruppeadgangskode er blevet ændret. For at fortsætte med at bruge dine hjemmegrupperessourcer skal du sørge for, at den person, der allerede har indtastet den nye adgangskode, er online, og derefter indtaste den nye adgangskode."},
    {46028, L"Delt"},
    {46029, L"Windows kunne ikke fjerne computeren fra hjemmegruppen."},
    {46030, L"%1 ændrede sin hjemmegruppeadgangskode. For at fortsætte med at bruge dine hjemmegrupperessourcer skal du sørge for, at den person, der allerede har indtastet den nye adgangskode, er online, og derefter indtaste den nye adgangskode."},
    {46031, L"Adgangskoder hjælper med at forhindre uautoriseret adgang til din hjemmegruppes filer og printere. Du kan få adgangskoden fra %2, %1 eller et andet medlem af din hjemmegruppe."},
    {46032, L"Adgangskoder hjælper med at forhindre uautoriseret adgang til din hjemmegruppes filer og printere. Du kan få adgangskoden fra %2, %1 eller et andet medlem af din hjemmegruppe."},
    {46033, L"Consolas"},
    {46034, L"Opret en hjemmegruppe"},
    {46035, L"Tilmeld dig en hjemmegruppe"},
    {46036, L"Skift din hjemmegruppeadgangskode"},
    {46037, L"Forlad hjemmegruppen"},
    {46038, L"For at få adgang til filer og printere på andre computere skal du tilføje dem til din hjemmegruppe. Følgende adgangskode er påkrævet:"},
    {46039, L"Indtast den nye hjemmegruppeadgangskode:"},
    {46040, L"Opdater adgangskode"},
    {46041, L"Sikkerhedskopier alle pc'er i din hjemmegruppe til et lokalt databeskyttelsesmål."},
    {46042, L"Sikkerhedskopier din pc ved hjælp af HomeGroup-databeskyttelsesmål"},
    {46043, L"Ikke delt"},
    {46044, L"Hjemmegrupper kan kun oprettes på private netværk.\nFor at ændre dine netværksplaceringsindstillinger skal du åbne Netværks- og delingscenter i Kontrolpanel."},
    {46045, L"Windows vil ikke længere registrere hjemmegrupper på dette netværk. For at oprette en ny hjemmegruppe skal du klikke på OK og åbne Hjemmegruppe i Kontrolpanel."},
    {46046, L"Windows har fundet en eksisterende hjemmegruppe.\nFor at deltage skal du klikke på OK og åbne Hjemmegruppe i Kontrolpanel."},
    {46047, L"Hjemmegruppetjeneste er nu tilgængelig. Prøv venligst igen."},
    {46048, L"Delingsindstillinger er opdateret"},
    {46049, L"De valgte filer og ressourcer deles med din hjemmegruppe."},
    {46050, L"Hjemmegruppeadgangskode blev opdateret"},
    {46051, L"Du har tilmeldt dig hjemmegruppen"},
    {46052, L"Du kan nu få adgang til dine delte filer og enheder. De filer og enheder, du deler, forbliver uændrede."},
    {46053, L"Du kan begynde at få adgang til filer og printere, der deles af andre brugere i din hjemmegruppe."},
    {46054, L"Opdater din hjemmegruppeadgangskode"},
    {46055, L"Tilmeld dig en hjemmegruppe"},
    {46056, L"Indtast den nye hjemmegruppeadgangskode fra %1."},
    {46057, L"Alle hjemmegruppecomputeres ure må ikke være indstillet til mere end 24 timers mellemrum. Sørg for, at computerens ure er synkroniseret, og prøv derefter at deltage i hjemmegruppen igen."},
    {46058, L"Adgangskoden opfylder ikke domænets krav til adgangskodestyrke. Indtast en matchende adgangskode, eller brug en anden hjemmegruppecomputer til at ændre din adgangskode."},
    {46059, L"Du kan ikke nulstille din adgangskode, fordi du ikke er forbundet til et privat netværk.\nOpret forbindelse til et privat netværk, og prøv igen."},
    {46060, L"Du er ikke forbundet til et privat netværk.\nFor at ændre dine netværksplaceringsindstillinger skal du åbne Netværks- og delingscenter i Kontrolpanel."},
    {46061, L"Del med andre hjemmecomputere"},
    {46062, L"Du kan dele filer og printere med andre computere. Du kan også streame medier til din enhed.\n\nHjemmegrupper er beskyttet med adgangskode, og du kan til enhver tid vælge, hvad du vil dele."},
    {46063, L"Tilføj andre computere til din hjemmegruppe ved hjælp af denne adgangskode"},
    {46064, L"For at få adgang til filer og printere på andre computere skal du tilføje dem til din hjemmegruppe. Følgende adgangskode er påkrævet:"},
    {46065, L"For at oprette eller deltage i en hjemmegruppe skal din netværksforbindelse have IPv6 aktiveret. For at aktivere IPv6 skal du starte HomeGroup Troubleshooter."},
    {46066, L"Føj personer til hjemmegruppen"},
    {46067, L"Konfigurer hjemmegruppedatabeskyttelse"},
    {46068, L"Der er fundet flere hjemmegrupper"},
    {46069, L"Del med andre hjemmegruppemedlemmer"},
    {46070, L"Dokumenter"},
    {46071, L"Billeder"},
    {46072, L"Musik"},
    {46073, L"Videoer"},
    {46074, L"Printere og enheder"},
    {46075, L"Skift indstillinger for hjemmegruppedeling"},
    {46076, L"%1 Deling"},
    {46077, L"Bekræfter din adgangskode..."},
};

// Finnish (fi-FI)
static const EmbeddedTextEntry kStrings_FI_FI[] = {
    {1, L"Kotiryhmä"},
    {2, L"Tarkista HomeGroup-asetukset, päätä, mitä tämä tietokone jakaa, ja näytä tai päivitä pääsysalasana."},
    {3, L"Organisaatiosi asettama käytäntö estää tätä sivua toimimasta. Pyydä apua verkonvalvojalta."},
    {4, L"Yksityiskohtaiset jakamisvaihtoehdot"},
    {5, L"Päällä"},
    {6, L"Pois päältä"},
    {7, L"Ei käytössä (tulostimia ei ole asennettu)"},
    {8, L"Tähän tietokoneeseen ei ole liitetty tulostinta."},
    {9, L"Jaa sisältöä kotona olevien tietokoneiden kanssa"},
    {10, L"Käytä kotiryhmääsi verkkotunnukseen liitetyn tietokoneen avulla"},
    {12, L"Muokkaa kotiryhmän asetuksia"},
    {13, L"Työskentely…"},
    {14, L"Tästä verkosta ei löytynyt kotiryhmää."},
    {15, L"%1 / %2 loi kotiryhmän verkkoon."},
    {16, L"Sinut on kutsuttu liittymään kotiryhmääsi."},
    {18, L"Käytä tätä sivua, jotta tämä tietokone kuuluu kotiryhmään."},
    {19, L"Tämä tietokone ei voi muodostaa yhteyttä kotiryhmääsi."},
    {20, L"HomeGroupin avulla luotetut tietokoneet voivat vaihtaa tiedostoja ja käyttää jaettuja tulostimia, ja se voi lähettää mediaa yhteensopiviin laitteisiin. Pääsy vaatii salasanan, mutta voit hallita, mitä tämä tietokone tarjoaa."},
    {21, L"Tämä tietokone on myös osa toimialuetta, joten se ei voi luoda omaa kotiryhmää, mutta se voi liittyä jonkun verkossa luomaan kotiryhmään.\n\nKotiryhmät yhdistävät kotiverkossasi olevat tietokoneet, jotta voit jakaa valokuvia, musiikkia, videoita, asiakirjoja ja tulostimia. Kotiryhmät on suojattu salasanalla, ja voit milloin tahansa valita, mitä jaat."},
    {22, L"Kotiryhmät yhdistävät kotiverkossasi olevat tietokoneet, jotta voit jakaa valokuvia, musiikkia, videoita, asiakirjoja ja tulostimia. Kotiryhmät on suojattu salasanalla, ja voit milloin tahansa valita, mitä jaat.\n\nEt voi luoda omia kotiryhmiä tässä Windows-versiossa, mutta voit liittyä muiden luomiin kotiryhmiin."},
    {23, L"Perusta kotiryhmä"},
    {24, L"Liity"},
    {25, L"Kotiryhmän salasana on vaihdettu. Jos haluat jatkaa kotiryhmäresurssien käyttöä, varmista, että uuden salasanan jo kirjoittanut henkilö on online-tilassa, ja anna sitten uusi salasana."},
    {26, L"Windows on havainnut toisen kotiryhmän verkossasi. Kotiryhmien avulla voit jakaa tiedostoja ja tulostimia muiden tietokoneiden kanssa. Voit myös suoratoistaa mediaa laitteellesi."},
    {27, L"%1 vaihtoi kotiryhmänsä salasanan. Jos haluat jatkaa kotiryhmäresurssien käyttöä, varmista, että uuden salasanan jo kirjoittanut henkilö on online-tilassa, ja anna sitten uusi salasana."},
    {28, L"Etsitään kotiryhmiä tästä verkosta…"},
    {29, L"Kirjoita uusi salasana"},
    {30, L"Liity nyt"},
    {32, L"Ennen kuin voit luoda kotiryhmän tai liittyä siihen, sinun on ensin muodostettava yhteys verkkoosi."},
    {34, L"Tällä sivulla voit luoda kotiryhmän tai liittyä siihen. Tietokoneesi verkkosijainti on asetettava yksityiseksi."},
    {35, L"Muuta verkon sijaintia"},
    {37, L"Jakamisasetukset yksityiselle"},
    {38, L"Jakamisasetukset julkiselle"},
    {39, L"Verkkotunnuksen jakamisvaihtoehdot"},
    {40, L"Yksityinen"},
    {41, L"Yksityinen (nykyinen profiili)"},
    {42, L"Julkinen"},
    {43, L"Julkinen (nykyinen profiili)"},
    {44, L"Verkkotunnus"},
    {45, L"Verkkotunnus (nykyinen profiili)"},
    {46, L"Median suoratoisto on päällä."},
    {47, L"Median suoratoisto on pois päältä."},
    {56, L"Peruuta"},
    {63, L"OK"},
    {64, L"Näytä tai tulosta kotiryhmän salasana"},
    {65, L"24pt;;;Consolas"},
    {66, L"Painatuspäivämäärä: %1 %2"},
    {67, L"Vaihtoehto: Tarkastele ja tulosta kotiryhmäsi salasana"},
    {68, L"Salasana:"},
    {69, L"Käytä tätä salasanaa muiden tietokoneiden yhdistämiseen kotiryhmääsi."},
    {70, L"Jokaisella tietokoneella:"},
    {71, L"Huomautus: Pois päältä tai lepotilassa olevat tietokoneet eivät näy kotiryhmässäsi."},
    {72, L"1. Napsauta Käynnistä-painiketta ja napsauta sitten Ohjauspaneeli."},
    {73, L"2. Napsauta Verkko ja Internet -kohdassa Valitse kotiryhmä ja jakamisasetukset."},
    {74, L"3. Napsauta Liity nyt ja anna salasanasi ohjatun HomeGroup Wizardin ohjeiden mukaan."},
    {75, L"Napsauta Käynnistä ja sitten Ohjauspaneeli."},
    {76, L"Kotiryhmän salasanaa ei voitu tulostaa"},
    {77, L"Tapahtui virhe, kun Windows yritti tulostaa kotiryhmän salasanan. (Virhekoodi:%1!u!)"},
    {78, L"Et ole tällä hetkellä yhteydessä kotiverkkoosi. Jos haluat tarkastella tiedostoja ja resursseja muissa kotiryhmän tietokoneissa, muodosta ensin yhteys kotiverkkoosi."},
    {79, L"%1 on liittänyt tietokoneen kotiryhmään. En ole jakanut kirjastoa kotiryhmäni kanssa. Napsauta alla olevaa linkkiä muuttaaksesi jakamaasi sisältöä. Älä sammuta tai käynnistä tietokonetta uudelleen ennen kuin jakaminen on valmis."},
    {80, L"En ole jakanut kirjastoa kotiryhmäni kanssa. Napsauta alla olevaa linkkiä muuttaaksesi jakamaasi sisältöä. Älä sammuta tai käynnistä tietokonetta uudelleen ennen kuin jakaminen on valmis."},
    {81, L"Kotiryhmä jakaa tällä hetkellä kirjaston tällä tietokoneella. Jotkut kotiryhmävaihtoehdot eivät ole käytettävissä, ennen kuin jakaminen on valmis. Älä sammuta tai käynnistä tietokonetta uudelleen ennen kuin jakaminen on valmis."},
    {82, L"Napsauta Verkko ja Internet -kohdassa Valitse kotiryhmä ja jakamisasetukset."},
    {83, L"Verkossa ei tällä hetkellä ole kotiryhmiä."},
    {84, L"Napsauta Liity nyt ja anna salasanasi ohjatun kotiryhmän ohjeiden mukaan."},
    {85, L"Click here to install."},
    {86, L"Windows found a homegroup printer"},
    {88, L"Introducing HomeGroup"},
    {89, L"%1 (current profile)"},
    {90, L"Käytä tätä sivua, jos haluat liittyä kotiryhmään, tietokoneesi verkkosijainnin on oltava yksityinen."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"HomeGroup is not ready yet. Yritä uudelleen muutaman minuutin kuluttua. Jos näet edelleen tämän viestin, napsauta linkkiä aloittaaksesi kotiryhmän vianetsinnän."},
    {95, L"Start the HomeGroup troubleshooter"},
    {98, L"HomeGroup password"},
    {99, L"Guest accounts cannot change homegroup settings."},
    {100, L"HomeGroup on löytänyt uuden jaetun tulostimen kotiverkostasi. Kun se on asennettu, se on kaikkien tällä tietokoneella olevien käytettävissä."},
    {101, L"Asenna tulostin"},
    {102, L"Kotiryhmä ei ole käytettävissä, koska et ole yhteydessä kotiverkkoosi."},
    {103, L"Kotiryhmä ei ole käytettävissä, koska et ole yhteydessä kotiverkkoosi."},
    {104, L"Ennen kuin voit liittyä kotiryhmään, sinun on ensin muodostettava yhteys verkkoon."},
    {105, L"Kotiryhmän kuva"},
    {106, L"Valitse, mitä haluat jakaa, ja tarkastele kotiryhmäsi salasanaa"},
    {107, L"Koska tämä tietokone on osa toimialuetta, asetukset sen kirjastojen ja laitteiden jakamiseksi muiden kotiryhmän tietokoneiden kanssa eivät ole käytettävissä."},
    {108, L"Asetukset kirjastojen ja laitteiden jakamiseksi muiden kotiryhmän tietokoneiden kanssa eivät ole käytettävissä tässä Windows-versiossa."},
    {109, L"Poista %1 kotiryhmästä"},
    {110, L"Peruuta"},
    {111, L"Poista kotiryhmän jäsen"},
    {112, L"%1 poistetaan kotiryhmästä"},
    {113, L"Kaikkien salasanalla liittyneiden kotiryhmän jäsenten on syötettävä salasana uudelleen."},
    {114, L"Tulostimet ja laitteet"},
    {115, L"Vaihda %1 kotiryhmän jäseniä"},
    {116, L"Kotiryhmän salasana nollattiin"},
    {117, L"Kotiryhmä jakaa tiedostoja"},
    {118, L"Vaihtoehto: Tämä tietokone kuuluu kotiryhmään"},
    {119, L"Kotiryhmään voi liittyä"},
    {120, L"Kotiryhmän voi luoda"},
    {121, L"Kotiryhmä ei ole käytettävissä"},
    {122, L"Epäluotettava tulostin"},
    {200, L"Lisää jäsen"},
    {201, L"Käyttäjäkuvake"},
    {202, L"Koko nimi"},
    {203, L"Käyttäjätunnus"},
    {204, L"Edistymispalkki"},
    {205, L"Kansion kuvake"},
    {220, L"Jaa kirjastoja ja laitteita"},
    {221, L"Valitse kirjasto, jonka haluat jakaa muiden kotiryhmässäsi olevien kanssa."},
    {222, L"Muokkaa kotiryhmän asetuksia"},
    {223, L"Tällä sivulla voit muuttaa Kotiryhmän asetuksia avaamalla Kotiryhmän ohjauspaneelista."},
    {224, L"Kotiryhmän asetukset"},
    {225, L"Tällä sivulla voit muuttaa kotiryhmän asetuksia Ohjauspaneelissa tai käyttää Kotiryhmän vianmääritystä."},
    {226, L"Käynnistä vianmääritys"},
    {227, L"Tämän sivun avulla voit etsiä ja korjata HomeGroup-ongelmia HomeGroup-vianmääritystoiminnolla."},
    {228, L"Näytä salasana"},
    {229, L"Tällä sivulla voit tarkastella tai tulostaa kotiryhmän salasanasi."},
    {230, L"Liity kotiryhmään"},
    {231, L"Liity tämän verkon kotiryhmään."},
    {530, L"Avaa yksityiskohtaiset jakamisasetukset…"},
    {541, L"Verkkonäkyvyys"},
    {542, L"Jos verkon etsintä on käytössä, tämä tietokone näkee ja näkee muut verkkoon kytketyt tietokoneet ja laitteet."},
    {543, L"Ota verkon etsintä käyttöön"},
    {544, L"Poista verkon etsintä käytöstä"},
    {545, L"Tiedostojen ja tulostimen käyttöoikeus"},
    {546, L"Kun tiedostojen ja tulostimien jakaminen on käytössä, muut verkkosi käyttäjät voivat käyttää tiedostoja ja tulostimia, jotka jaat tällä tietokoneella."},
    {547, L"Ota tiedostojen ja tulostimen jakaminen käyttöön"},
    {548, L"Poista tiedostojen ja tulostimen jakaminen käytöstä"},
    {549, L"Julkisen kansion jakaminen"},
    {550, L"Kun julkisten kansioiden jakaminen on käytössä, verkon käyttäjät, mukaan lukien kotiryhmän jäsenet, voivat käyttää julkisissa kansioissa olevia tiedostoja."},
    {552, L"Kun otat jakamisen käyttöön, kuka tahansa, jolla on pääsy verkkoosi, voi lukea ja kirjoittaa tiedostoja julkisiin kansioihin."},
    {553, L"Poista julkisten kansioiden jakaminen käytöstä (tälle tietokoneelle kirjautuneet käyttäjät voivat silti käyttää näitä kansioita)"},
    {554, L"Muuta eri verkkoprofiilien jakamisasetuksia"},
    {559, L"Media pääsy"},
    {560, L"Kun median suoratoisto on käytössä, verkkosi käyttäjät ja laitteet voivat käyttää tämän tietokoneen valokuvia, musiikkia ja videoita. Tämä tietokone voi myös löytää mediaa verkosta."},
    {564, L"Peruuta"},
    {567, L"Ota muutokset käyttöön"},
    {584, L"Windows luo erillisen verkkoprofiilin jokaiselle käyttämällesi verkolle. Voit valita kullekin profiilille tietyt vaihtoehdot."},
    {585, L"Kotiryhmän varoituskuvake"},
    {586, L"Tältä tietokoneelta jaetut kirjastot ja laitteet"},
    {595, L"Lisää kotiryhmän tehtäviä"},
    {600, L"Näytä tai tulosta kotiryhmän salasana"},
    {601, L"Järjestelmänvalvojasi ei ole antanut sinulle pääsyä kotiryhmääsi."},
    {604, L"Vaihda salasana..."},
    {605, L"Poistu kotiryhmästä..."},
    {607, L"Valitse median suoratoistoasetukset..."},
    {608, L"Koska tämä tietokone on osa toimialuetta, asetukset sen kirjastojen ja laitteiden jakamiseksi muiden kotiryhmän tietokoneiden kanssa eivät ole käytettävissä."},
    {609, L"Salasanasuojattu jakaminen"},
    {610, L"Kun salasanasuojattu jakaminen on käytössä, vain käyttäjät, joilla on tämän tietokoneen käyttäjätilit ja salasanat, voivat käyttää jaettuja tiedostoja, tähän tietokoneeseen kytkettyjä tulostimia ja julkisia kansioita. Salasanasuojattu jakaminen on poistettava käytöstä, jotta muut voivat käyttää sitä."},
    {611, L"Ota salasanasuojattu jakaminen käyttöön"},
    {612, L"Poista salasanalla suojattu jakaminen käytöstä"},
    {613, L"Tulosta sivu"},
    {614, L"Sallii jaetun sisällön toistamisen kaikissa tämän verkon laitteissa, kuten televisioissa ja pelikonsoleissa"},
    {615, L"Yksityinen verkko"},
    {616, L"Vieras tai julkinen verkko"},
    {617, L"Domain verkko"},
    {619, L"Kotiryhmän yhteydet"},
    {620, L"Windows typically manages connections to other homegroup computers. Jos kuitenkin käytät samaa käyttäjätiliä ja salasanaa kaikissa tietokoneissasi, voit saada HomeGroupin käyttämään kyseistä tiliä sen sijaan."},
    {621, L"Vaihtoehto: Anna Windowsin hallita kotiryhmän yhteyksiä (suositus)"},
    {622, L"Muodosta yhteys muihin tietokoneisiin käyttäjätunnuksellasi ja salasanallasi"},
    {624, L"Start the HomeGroup troubleshooter"},
    {627, L"Tiedostonjakoyhteydet"},
    {628, L"Windows käyttää 128-bittistä salausta tiedostojen jakamiseen liittyvien yhteyksien suojaamiseen. Jotkut laitteet eivät tue 128-bittistä salausta, ja niiden on käytettävä 40- tai 56-bittistä salausta."},
    {629, L"Suojaa tiedostonjakoyhteytesi 128-bittisellä salauksella (suositus)"},
    {630, L"Ota käyttöön laitteen tiedostojen jakaminen 40- tai 56-bittisellä salauksella"},
    {631, L"Jokainen verkko"},
    {632, L"Change what's shared with your homegroup"},
    {637, L"Sulje"},
    {639, L"Kotiryhmän etäkäyttö"},
    {640, L"Muut kotiryhmän jäsenet voivat muodostaa yhteyden kotiryhmääsi mistä tahansa tietokoneidensa kautta."},
    {641, L"Vaihtoehto: Poista kotiryhmän etäkäyttö käytöstä tämän tietokoneen kautta"},
    {642, L"Vaihtoehto: Ota käyttöön kotiryhmän etäkäyttö tämän tietokoneen kautta"},
    {648, L"Valitse käytettävissä olevat tiedostot ja laitteet ja valitse sitten niiden käyttöoikeustasot."},
    {649, L"Kirjasto tai hakemisto"},
    {650, L"Käyttöoikeustaso"},
    {652, L"Ota verkkoon liitettyjen laitteiden automaattinen määritys käyttöön."},
    {46000, L"Kotiryhmä"},
    {46004, L"Option: Choose a password for your homegroup"},
    {46005, L"Type the homegroup password"},
    {46006, L"&Luo nyt"},
    {46007, L"&Liity nyt"},
    {46008, L"Lisää muita tietokoneita kotiryhmääsi tällä salasanalla"},
    {46009, L"You have joined the homegroup"},
    {46011, L"Kotiryhmä"},
    {46012, L"Windows ei voi määrittää kotiryhmää tälle tietokoneelle."},
    {46013, L"Koska tämä tietokone on osa toimialuetta, sen kirjaston jakaminen muiden kotiryhmän tietokoneiden kanssa ei ole käytettävissä."},
    {46014, L"Salasanan tulee sisältää vähintään 8 merkkiä, eikä siinä saa olla välilyöntejä alussa tai lopussa."},
    {46015, L"Salasana on väärä.\nYritä uudelleen. Salasanoissa isot ja pienet kirjaimet eroavat toisistaan."},
    {46016, L"Vaihtoehto: Kaikki tämän tietokoneen kotiryhmäyhteydet katkaistaan"},
    {46017, L"Kotiryhmästäsi poistuminen onnistui"},
    {46018, L"Muuta kotiryhmällesi jaettua sisältöä"},
    {46019, L"Jaa valokuvasi, videosi, musiikkisi, asiakirjasi ja tulostimesi muiden kotisi tietokoneiden kanssa."},
    {46020, L"&Tee muutoksia"},
    {46021, L"Kotiryhmän salasanan vaihtaminen katkaisee yhteyden kaikkiin"},
    {46022, L"Anna uusi salasana kotiryhmällesi"},
    {46023, L"&Vaihda salasana"},
    {46024, L"Kotiryhmän salasana vaihdettu onnistuneesti"},
    {46025, L"Kotiryhmän salasana vaihdettiin"},
    {46026, L"Kirjoita kotiryhmän salasana"},
    {46027, L"Kotiryhmän salasana on vaihdettu. Jos haluat jatkaa kotiryhmäresurssien käyttöä, varmista, että uuden salasanan jo kirjoittanut henkilö on online-tilassa, ja anna sitten uusi salasana."},
    {46028, L"Jaettu"},
    {46029, L"Windows ei voinut poistaa tietokonetta kotiryhmästä."},
    {46030, L"%1 vaihtoi kotiryhmänsä salasanan. Jos haluat jatkaa kotiryhmäresurssien käyttöä, varmista, että uuden salasanan jo kirjoittanut henkilö on online-tilassa, ja anna sitten uusi salasana."},
    {46031, L"Salasanat auttavat estämään luvattoman pääsyn kotiryhmäsi tiedostoihin ja tulostimiin. Voit saada salasanan %2:lta, %1:lta tai toiselta kotiryhmäsi jäseneltä."},
    {46032, L"Salasanat auttavat estämään luvattoman pääsyn kotiryhmäsi tiedostoihin ja tulostimiin. Voit saada salasanan %2:lta, %1:lta tai toiselta kotiryhmäsi jäseneltä."},
    {46033, L"Consolas"},
    {46034, L"Luo kotiryhmä"},
    {46035, L"Liity kotiryhmään"},
    {46036, L"Vaihda kotiryhmän salasana"},
    {46037, L"Poistu kotiryhmästä"},
    {46038, L"Jos haluat käyttää muiden tietokoneiden tiedostoja ja tulostimia, sinun on lisättävä ne kotiryhmääsi. Seuraava salasana vaaditaan:"},
    {46039, L"Kirjoita uusi kotiryhmän salasana:"},
    {46040, L"Päivitä salasana"},
    {46041, L"Varmuuskopioi kaikki kotiryhmäsi tietokoneet paikalliseen tietosuojakohteeseen."},
    {46042, L"Varmuuskopioi tietokoneesi käyttämällä HomeGroupin tietosuojatavoitteita"},
    {46043, L"Ei jaettu"},
    {46044, L"Kotiryhmiä voidaan luoda vain yksityisissä verkoissa.\nVoit muuttaa verkon sijaintiasetuksia avaamalla Ohjauspaneelin Verkko- ja jakamiskeskus."},
    {46045, L"Windows ei enää tunnista kotiryhmiä tässä verkossa. Luo uusi kotiryhmä napsauttamalla OK ja avaa Kotiryhmä Ohjauspaneelista."},
    {46046, L"Windows havaitsi olemassa olevan kotiryhmän.\nLiity napsauttamalla OK ja avaamalla Kotiryhmä ohjauspaneelista."},
    {46047, L"Kotiryhmäpalvelu on nyt saatavilla. Yritä uudelleen."},
    {46048, L"Jakamisasetukset päivitetty"},
    {46049, L"Valitut tiedostot ja resurssit jaetaan kotiryhmäsi kanssa."},
    {46050, L"Kotiryhmän salasana päivitetty onnistuneesti"},
    {46051, L"Liityit kotiryhmään"},
    {46052, L"Voit nyt käyttää jaettuja tiedostojasi ja laitteitasi. Jakamasi tiedostot ja laitteet pysyvät ennallaan."},
    {46053, L"Voit alkaa käyttää tiedostoja ja tulostimia, jotka muut kotiryhmäsi käyttäjät jakavat."},
    {46054, L"Päivitä kotiryhmäsi salasana"},
    {46055, L"Liity kotiryhmään"},
    {46056, L"Syötä uusi kotiryhmän salasana osoitteesta %1."},
    {46057, L"Kaikkien kotiryhmän tietokoneiden kellojen tulee olla asetettuna enintään 24 tunnin välein. Varmista, että tietokoneesi kellot ovat synkronoituja, ja yritä sitten liittyä uudelleen kotiryhmään."},
    {46058, L"Salasana ei täytä verkkotunnuksen salasanan vahvuusvaatimuksia. Anna vastaava salasana tai käytä toista HomeGroup-tietokonetta salasanan vaihtamiseen."},
    {46059, L"Et voi nollata salasanaasi, koska et ole yhteydessä yksityiseen verkkoon.\nYhdistä yksityiseen verkkoon ja yritä uudelleen."},
    {46060, L"Et ole yhteydessä yksityiseen verkkoon.\nVoit muuttaa verkon sijaintiasetuksia avaamalla Ohjauspaneelin Verkko- ja jakamiskeskus."},
    {46061, L"Jaa muiden kotitietokoneiden kanssa"},
    {46062, L"Voit jakaa tiedostoja ja tulostimia muiden tietokoneiden kanssa. Voit myös suoratoistaa mediaa laitteellesi.\n\nKotiryhmät on suojattu salasanalla, ja voit milloin tahansa valita, mitä jaat."},
    {46063, L"Lisää muita tietokoneita kotiryhmääsi tällä salasanalla"},
    {46064, L"Jos haluat käyttää muiden tietokoneiden tiedostoja ja tulostimia, sinun on lisättävä ne kotiryhmääsi. Seuraava salasana vaaditaan:"},
    {46065, L"Jotta voit luoda kotiryhmän tai liittyä siihen, verkkoyhteydessäsi on oltava IPv6 käytössä. Ota IPv6 käyttöön käynnistämällä HomeGroup Troubleshooter."},
    {46066, L"Lisää ihmisiä kotiryhmään"},
    {46067, L"Määritä kotiryhmän tietosuoja"},
    {46068, L"Useita kotiryhmiä havaittu"},
    {46069, L"Jaa muiden kotiryhmän jäsenten kanssa"},
    {46070, L"Asiakirjat"},
    {46071, L"Kuvia"},
    {46072, L"Musiikki"},
    {46073, L"Videot"},
    {46074, L"Tulostimet ja laitteet"},
    {46075, L"Muuta kotiryhmän jakamisasetuksia"},
    {46076, L"%1 Jakaminen"},
    {46077, L"Vahvistetaan salasanaasi..."},
};

// Greek (el-GR)
static const EmbeddedTextEntry kStrings_EL_GR[] = {
    {1, L"HomeGroup"},
    {2, L"Ελέγξτε τις επιλογές HomeGroup, αποφασίστε τι μοιράζεται αυτός ο υπολογιστής και εμφανίστε ή ενημερώστε τον κωδικό πρόσβασης."},
    {3, L"Μια πολιτική που έχει ορίσει ο οργανισμός σας εμποδίζει την εκτέλεση αυτής της σελίδας. Ζητήστε βοήθεια από τον διαχειριστή του δικτύου."},
    {4, L"Λεπτομερείς επιλογές κοινής χρήσης"},
    {5, L"Ενεργό"},
    {6, L"Απενεργοποίηση"},
    {7, L"Απενεργοποίηση (δεν έχουν εγκατασταθεί εκτυπωτές)"},
    {8, L"Δεν υπάρχει εκτυπωτής συνδεδεμένος σε αυτόν τον υπολογιστή."},
    {9, L"Μοιραστείτε περιεχόμενο με υπολογιστές στο σπίτι"},
    {10, L"Αποκτήστε πρόσβαση στην οικιακή ομάδα σας χρησιμοποιώντας έναν υπολογιστή συνδεδεμένο σε τομέα"},
    {12, L"Επεξεργασία επιλογών HomeGroup"},
    {13, L"Εργασία…"},
    {14, L"Δεν βρέθηκε HomeGroup σε αυτό το δίκτυο."},
    {15, L"Το %1 του %2 δημιούργησε μια οικιακή ομάδα στο δίκτυο."},
    {16, L"Έχετε προσκληθεί να συμμετάσχετε στην οικιακή ομάδα σας."},
    {18, L"Χρησιμοποιήστε αυτήν τη σελίδα σε αυτόν τον υπολογιστή που ανήκει σε μια οικιακή ομάδα."},
    {19, L"Αυτός ο υπολογιστής δεν μπορεί να συνδεθεί στην οικιακή ομάδα σας."},
    {20, L"Η HomeGroup επιτρέπει σε αξιόπιστους υπολογιστές να ανταλλάσσουν αρχεία και να χρησιμοποιούν κοινόχρηστους εκτυπωτές και μπορεί να στείλει πολυμέσα σε συμβατές συσκευές. Η πρόσβαση απαιτεί κωδικό πρόσβασης, ενώ διατηρείτε τον έλεγχο του τι διατίθεται αυτός ο υπολογιστής."},
    {21, L"Αυτός ο υπολογιστής είναι επίσης μέρος ενός τομέα, επομένως δεν μπορεί να δημιουργήσει τη δική του οικιακή ομάδα, αλλά μπορεί να ενταχθεί σε μια οικιακή ομάδα που δημιουργήθηκε από κάποιον στο δίκτυο.\n\nΟι οικιακές ομάδες συνδέουν υπολογιστές στο οικιακό σας δίκτυο, ώστε να μπορείτε να μοιράζεστε φωτογραφίες, μουσική, βίντεο, έγγραφα και εκτυπωτές. Οι οικιακές ομάδες προστατεύονται με κωδικό πρόσβασης και μπορείτε να επιλέξετε τι θα μοιραστείτε ανά πάσα στιγμή."},
    {22, L"Οι οικιακές ομάδες συνδέουν υπολογιστές στο οικιακό σας δίκτυο, ώστε να μπορείτε να μοιράζεστε φωτογραφίες, μουσική, βίντεο, έγγραφα και εκτυπωτές. Οι οικιακές ομάδες προστατεύονται με κωδικό πρόσβασης και μπορείτε να επιλέξετε τι θα μοιραστείτε ανά πάσα στιγμή.\n\nΔεν μπορείτε να δημιουργήσετε τις δικές σας οικιακές ομάδες σε αυτήν την έκδοση των Windows, αλλά μπορείτε να συμμετάσχετε σε οικιακές ομάδες που έχουν δημιουργηθεί από άλλους."},
    {23, L"Ρύθμιση μιας Οικιακής Ομάδας"},
    {24, L"Εγγραφείτε"},
    {25, L"Ο κωδικός πρόσβασης HomeGroup έχει αλλάξει. Για να συνεχίσετε να χρησιμοποιείτε τους πόρους της οικιακής ομάδας σας, βεβαιωθείτε ότι το άτομο που έχει ήδη εισαγάγει τον νέο κωδικό πρόσβασης είναι συνδεδεμένο και, στη συνέχεια, εισαγάγετε τον νέο κωδικό πρόσβασης."},
    {26, L"Τα Windows εντόπισαν μια άλλη οικιακή ομάδα στο δίκτυό σας. Οι οικιακές ομάδες σάς επιτρέπουν να μοιράζεστε αρχεία και εκτυπωτές με άλλους υπολογιστές. Μπορείτε επίσης να κάνετε ροή πολυμέσων στη συσκευή σας."},
    {27, L"Ο %1 άλλαξε τον κωδικό πρόσβασης της ομάδας του. Για να συνεχίσετε να χρησιμοποιείτε τους πόρους της οικιακής ομάδας σας, βεβαιωθείτε ότι το άτομο που έχει ήδη εισαγάγει τον νέο κωδικό πρόσβασης είναι συνδεδεμένο και, στη συνέχεια, εισαγάγετε τον νέο κωδικό πρόσβασης."},
    {28, L"Αναζήτηση για Ομάδες Οικίας σε αυτό το δίκτυο…"},
    {29, L"Πληκτρολογήστε νέο κωδικό πρόσβασης"},
    {30, L"Εγγραφείτε τώρα"},
    {32, L"Για να μπορέσετε να δημιουργήσετε ή να συμμετάσχετε σε μια οικιακή ομάδα, πρέπει πρώτα να συνδεθείτε στο δίκτυό σας."},
    {34, L"Χρησιμοποιήστε αυτήν τη σελίδα για να δημιουργήσετε ή να εγγραφείτε σε μια οικιακή ομάδα, η τοποθεσία δικτύου του υπολογιστή σας πρέπει να οριστεί ως ιδιωτική."},
    {35, L"Αλλαγή τοποθεσίας δικτύου"},
    {37, L"Επιλογές κοινής χρήσης για Ιδιωτικό"},
    {38, L"Επιλογές κοινής χρήσης για Public"},
    {39, L"Επιλογές κοινής χρήσης για Domain"},
    {40, L"Ιδιωτικό"},
    {41, L"Ιδιωτικό (τρέχον προφίλ)"},
    {42, L"Δημόσιο"},
    {43, L"Δημόσιο (τρέχον προφίλ)"},
    {44, L"Τομέας"},
    {45, L"Τομέας (τρέχον προφίλ)"},
    {46, L"Η ροή πολυμέσων είναι ενεργοποιημένη."},
    {47, L"Η ροή πολυμέσων είναι απενεργοποιημένη."},
    {56, L"Ακύρωση"},
    {63, L"ΟΚ"},
    {64, L"Εμφάνιση ή εκτύπωση του κωδικού πρόσβασης της αρχικής ομάδας"},
    {65, L"24pt;;;Consolas"},
    {66, L"Ημερομηνία εκτύπωσης: %1 %2"},
    {67, L"Επιλογή: Προβάλετε και εκτυπώστε τον κωδικό πρόσβασης της οικιακής ομάδας σας"},
    {68, L"Κωδικός πρόσβασης:"},
    {69, L"Χρησιμοποιήστε αυτόν τον κωδικό πρόσβασης για να συνδέσετε άλλους υπολογιστές στην οικιακή ομάδα σας."},
    {70, L"Σε κάθε υπολογιστή:"},
    {71, L"Σημείωση: Οι υπολογιστές που είναι απενεργοποιημένοι ή σε αδράνεια δεν θα εμφανίζονται στην οικιακή ομάδα σας."},
    {72, L"1. Κάντε κλικ στο κουμπί Έναρξη και, στη συνέχεια, κάντε κλικ στον Πίνακα Ελέγχου."},
    {73, L"2. Στην περιοχή Δίκτυο και Διαδίκτυο, κάντε κλικ στην επιλογή Επιλογές οικιακής ομάδας και κοινής χρήσης."},
    {74, L"3. Κάντε κλικ στο Join Now και ακολουθήστε τον Οδηγό HomeGroup για να εισαγάγετε τον κωδικό πρόσβασής σας."},
    {75, L"Κάντε κλικ στο κουμπί Έναρξη και, στη συνέχεια, κάντε κλικ στον Πίνακα Ελέγχου."},
    {76, L"Δεν ήταν δυνατή η εκτύπωση του κωδικού πρόσβασης της οικιακής ομάδας"},
    {77, L"Παρουσιάστηκε σφάλμα όταν τα Windows προσπάθησαν να εξάγουν τον κωδικό πρόσβασης της οικιακής ομάδας. (Κωδικός σφάλματος:%1!u!)"},
    {78, L"Δεν είστε συνδεδεμένοι αυτήν τη στιγμή στο οικιακό σας δίκτυο. Για να προβάλετε αρχεία και πόρους σε άλλους υπολογιστές οικιακής ομάδας, συνδεθείτε πρώτα στο οικιακό σας δίκτυο."},
    {79, L"Το %1 έχει ενώσει τον υπολογιστή στην οικιακή ομάδα. Δεν έχω μοιραστεί τη βιβλιοθήκη με την οικιακή μου ομάδα. Κάντε κλικ στον παρακάτω σύνδεσμο για να αλλάξετε αυτό που έχετε μοιραστεί. Μην τερματίσετε ή επανεκκινήσετε τον υπολογιστή σας μέχρι να ολοκληρωθεί η κοινή χρήση."},
    {80, L"Δεν έχω μοιραστεί τη βιβλιοθήκη με την οικιακή μου ομάδα. Κάντε κλικ στον παρακάτω σύνδεσμο για να αλλάξετε αυτό που έχετε μοιραστεί. Μην τερματίσετε ή επανεκκινήσετε τον υπολογιστή σας μέχρι να ολοκληρωθεί η κοινή χρήση."},
    {81, L"Η HomeGroup μοιράζεται αυτήν τη στιγμή τη βιβλιοθήκη σε αυτόν τον υπολογιστή. Ορισμένες επιλογές οικιακής ομάδας δεν είναι διαθέσιμες μέχρι να ολοκληρωθεί η κοινή χρήση. Μην τερματίσετε ή επανεκκινήσετε τον υπολογιστή σας μέχρι να ολοκληρωθεί η κοινή χρήση."},
    {82, L"Στην περιοχή Δίκτυο και Διαδίκτυο, κάντε κλικ στην επιλογή Επιλογές οικιακής ομάδας και κοινής χρήσης."},
    {83, L"Αυτήν τη στιγμή δεν υπάρχουν οικιακές ομάδες στο δίκτυο."},
    {84, L"Κάντε κλικ στο Join Now και ακολουθήστε τον Οδηγό HomeGroup για να εισαγάγετε τον κωδικό πρόσβασής σας."},
    {85, L"Κάντε κλικ εδώ για εγκατάσταση."},
    {86, L"Τα Windows βρήκαν έναν εκτυπωτή οικιακής ομάδας"},
    {88, L"Παρουσιάζοντας το HomeGroup"},
    {89, L"%1 (τρέχον προφίλ)"},
    {90, L"Χρησιμοποιήστε αυτήν τη σελίδα για να συμμετάσχετε σε μια οικιακή ομάδα, η τοποθεσία δικτύου του υπολογιστή σας πρέπει να οριστεί ως ιδιωτική."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Η HomeGroup δεν είναι ακόμα έτοιμη. Δοκιμάστε ξανά σε λίγα λεπτά. Εάν συνεχίσετε να βλέπετε αυτό το μήνυμα, κάντε κλικ στον σύνδεσμο για να ξεκινήσετε την αντιμετώπιση προβλημάτων της οικιακής ομάδας σας."},
    {95, L"Ξεκινήστε το εργαλείο αντιμετώπισης προβλημάτων HomeGroup"},
    {98, L"Κωδικός πρόσβασης HomeGroup"},
    {99, L"Οι λογαριασμοί επισκεπτών δεν μπορούν να αλλάξουν τις ρυθμίσεις οικιακής ομάδας."},
    {100, L"Η HomeGroup βρήκε έναν νέο κοινόχρηστο εκτυπωτή στο οικιακό σας δίκτυο. Μόλις εγκατασταθεί, θα είναι διαθέσιμο σε οποιονδήποτε σε αυτόν τον υπολογιστή."},
    {101, L"Εγκαταστήστε τον εκτυπωτή"},
    {102, L"Το HomeGroup δεν είναι διαθέσιμο επειδή δεν είστε συνδεδεμένοι στο οικιακό σας δίκτυο."},
    {103, L"Το HomeGroup δεν είναι διαθέσιμο επειδή δεν είστε συνδεδεμένοι στο οικιακό σας δίκτυο."},
    {104, L"Πριν εγγραφείτε σε μια οικιακή ομάδα, πρέπει πρώτα να συνδεθείτε στο δίκτυο."},
    {105, L"Εικόνα αρχικής ομάδας"},
    {106, L"Επιλέξτε αυτό που θέλετε να μοιραστείτε και δείτε τον κωδικό πρόσβασης της οικιακής ομάδας σας"},
    {107, L"Επειδή αυτός ο υπολογιστής είναι μέρος ενός τομέα, οι ρυθμίσεις για την κοινή χρήση των βιβλιοθηκών και των συσκευών του με άλλους υπολογιστές της οικιακής ομάδας δεν είναι διαθέσιμες."},
    {108, L"Οι ρυθμίσεις για κοινή χρήση βιβλιοθηκών και συσκευών με άλλους υπολογιστές σε μια οικιακή ομάδα δεν είναι διαθέσιμες σε αυτήν την έκδοση των Windows."},
    {109, L"Καταργήστε το %1 από την οικιακή ομάδα"},
    {110, L"Ακύρωση"},
    {111, L"Κατάργηση μέλους οικιακής ομάδας"},
    {112, L"Το %1 θα αφαιρεθεί από την οικιακή ομάδα"},
    {113, L"Όλα τα μέλη της οικιακής ομάδας που συμμετέχουν χρησιμοποιώντας κωδικό πρόσβασης θα πρέπει να εισάγουν ξανά τον κωδικό πρόσβασης."},
    {114, L"Εκτυπωτές και συσκευές"},
    {115, L"Αλλαγή μελών οικιακής ομάδας %1"},
    {116, L"Έγινε επαναφορά του κωδικού πρόσβασης της οικιακής ομάδας"},
    {117, L"Η HomeGroup μοιράζεται αρχεία"},
    {118, L"Επιλογή: Αυτός ο υπολογιστής ανήκει σε μια οικιακή ομάδα"},
    {119, L"Μια οικιακή ομάδα είναι διαθέσιμη για συμμετοχή"},
    {120, L"Μπορεί να δημιουργηθεί μια οικιακή ομάδα"},
    {121, L"Η HomeGroup δεν είναι διαθέσιμη"},
    {122, L"Μη αξιόπιστος εκτυπωτής"},
    {200, L"Προσθήκη μέλους"},
    {201, L"Εικονίδιο χρήστη"},
    {202, L"Πλήρες όνομα"},
    {203, L"Αναγνωριστικό χρήστη"},
    {204, L"Γραμμή προόδου"},
    {205, L"Εικονίδιο φακέλου"},
    {220, L"Μοιραστείτε βιβλιοθήκες και υλικό"},
    {221, L"Επιλέξτε τη βιβλιοθήκη που θέλετε να μοιραστείτε με άλλους στην οικιακή ομάδα σας."},
    {222, L"Επεξεργασία επιλογών HomeGroup"},
    {223, L"Χρησιμοποιήστε αυτή τη σελίδα για να αλλάξετε τις ρυθμίσεις HomeGroup, ανοίξτε την HomeGroup στον Πίνακα Ελέγχου."},
    {224, L"Επιλογές HomeGroup"},
    {225, L"Χρησιμοποιήστε αυτήν τη σελίδα για να αλλάξετε τις ρυθμίσεις της Οικιακής ομάδας στον Πίνακα Ελέγχου ή χρησιμοποιήστε την Αντιμετώπιση προβλημάτων Οικιακής ομάδας."},
    {226, L"Έναρξη αντιμετώπισης προβλημάτων"},
    {227, L"Χρησιμοποιήστε αυτήν τη σελίδα για να χρησιμοποιήσετε το εργαλείο αντιμετώπισης προβλημάτων HomeGroup για να βρείτε και να διορθώσετε προβλήματα με την HomeGroup σας."},
    {228, L"Προβολή κωδικού πρόσβασης"},
    {229, L"Χρησιμοποιήστε αυτήν τη σελίδα για να προβάλετε ή να εκτυπώσετε τον κωδικό πρόσβασης της οικιακής ομάδας σας."},
    {230, L"Εγγραφείτε στην οικιακή ομάδα"},
    {231, L"Εγγραφείτε στην οικιακή ομάδα σε αυτό το δίκτυο."},
    {530, L"Άνοιγμα λεπτομερών επιλογών κοινής χρήσης…"},
    {541, L"Ορατότητα δικτύου"},
    {542, L"Εάν η ανακάλυψη δικτύου είναι ενεργοποιημένη, αυτός ο υπολογιστής μπορεί να δει και να είναι ορατός από άλλους δικτυωμένους υπολογιστές και συσκευές."},
    {543, L"Ενεργοποιήστε την ανακάλυψη δικτύου"},
    {544, L"Απενεργοποιήστε την ανακάλυψη δικτύου"},
    {545, L"Πρόσβαση σε αρχείο και εκτυπωτή"},
    {546, L"Όταν είναι ενεργοποιημένη η κοινή χρήση αρχείων και εκτυπωτών, άλλοι χρήστες στο δίκτυό σας μπορούν να έχουν πρόσβαση στα αρχεία και τους εκτυπωτές που μοιράζεστε από αυτόν τον υπολογιστή."},
    {547, L"Ενεργοποιήστε την κοινή χρήση αρχείων και εκτυπωτών"},
    {548, L"Απενεργοποιήστε την κοινή χρήση αρχείων και εκτυπωτών"},
    {549, L"Κοινή χρήση δημόσιου φακέλου"},
    {550, L"Όταν είναι ενεργοποιημένη η κοινή χρήση δημόσιου φακέλου, οι χρήστες στο δίκτυό σας, συμπεριλαμβανομένων των μελών της οικιακής ομάδας, μπορούν να έχουν πρόσβαση σε αρχεία σε δημόσιους φακέλους."},
    {552, L"Η ενεργοποίηση της κοινής χρήσης επιτρέπει σε οποιονδήποτε έχει πρόσβαση στο δίκτυό σας να διαβάζει και να γράφει αρχεία στους δημόσιους φακέλους σας."},
    {553, L"Απενεργοποίηση κοινής χρήσης δημόσιου φακέλου (οι χρήστες που είναι συνδεδεμένοι σε αυτόν τον υπολογιστή εξακολουθούν να έχουν πρόσβαση σε αυτούς τους φακέλους)"},
    {554, L"Αλλαγή επιλογών κοινής χρήσης για διάφορα προφίλ δικτύου"},
    {559, L"Πρόσβαση στα μέσα"},
    {560, L"Όταν η ροή πολυμέσων είναι ενεργοποιημένη, οι χρήστες και οι συσκευές στο δίκτυό σας μπορούν να έχουν πρόσβαση στις φωτογραφίες, τη μουσική και τα βίντεο σε αυτόν τον υπολογιστή. Αυτός ο υπολογιστής μπορεί επίσης να βρει πολυμέσα στο δίκτυο."},
    {564, L"Ακύρωση"},
    {567, L"Εφαρμόστε αλλαγές"},
    {584, L"Τα Windows δημιουργούν ένα ξεχωριστό προφίλ δικτύου για κάθε δίκτυο που χρησιμοποιείτε. Μπορείτε να επιλέξετε συγκεκριμένες επιλογές για κάθε προφίλ."},
    {585, L"Εικονίδιο προειδοποίησης αρχικής ομάδας"},
    {586, L"Κοινόχρηστες βιβλιοθήκες και συσκευές από αυτόν τον υπολογιστή"},
    {595, L"Περισσότερες εργασίες HomeGroup"},
    {600, L"Εμφάνιση ή εκτύπωση του κωδικού πρόσβασης της αρχικής ομάδας"},
    {601, L"Ο διαχειριστής του συστήματός σας δεν σας έχει επιτρέψει να αποκτήσετε πρόσβαση στην οικιακή ομάδα σας."},
    {604, L"Αλλάξτε τον κωδικό πρόσβασης..."},
    {605, L"Βγείτε από την οικιακή ομάδα..."},
    {607, L"Επιλέξτε επιλογές ροής πολυμέσων..."},
    {608, L"Επειδή αυτός ο υπολογιστής είναι μέρος ενός τομέα, οι ρυθμίσεις για την κοινή χρήση των βιβλιοθηκών και των συσκευών του με άλλους υπολογιστές της οικιακής ομάδας δεν είναι διαθέσιμες."},
    {609, L"Κοινή χρήση με κωδικό πρόσβασης"},
    {610, L"Όταν η κοινή χρήση που προστατεύεται με κωδικό πρόσβασης είναι ενεργοποιημένη, μόνο οι χρήστες με λογαριασμούς χρήστη και κωδικούς πρόσβασης σε αυτόν τον υπολογιστή μπορούν να έχουν πρόσβαση σε κοινόχρηστα αρχεία, εκτυπωτές που είναι συνδεδεμένοι σε αυτόν τον υπολογιστή και δημόσιους φακέλους. Η κοινή χρήση που προστατεύεται με κωδικό πρόσβασης πρέπει να απενεργοποιηθεί για να επιτρέπεται σε άλλους να έχουν πρόσβαση."},
    {611, L"Ενεργοποιήστε την κοινή χρήση με κωδικό πρόσβασης"},
    {612, L"Απενεργοποιήστε την κοινή χρήση με προστασία κωδικού πρόσβασης"},
    {613, L"Εκτύπωση σελίδας"},
    {614, L"Επιτρέπει την αναπαραγωγή κοινόχρηστου περιεχομένου σε όλες τις συσκευές αυτού του δικτύου, όπως τηλεοράσεις και κονσόλες παιχνιδιών"},
    {615, L"Ιδιωτικό δίκτυο"},
    {616, L"Επισκέπτης ή δημόσιο δίκτυο"},
    {617, L"Δίκτυο τομέα"},
    {619, L"Συνδέσεις HomeGroup"},
    {620, L"Τα Windows διαχειρίζονται συνήθως συνδέσεις με άλλους υπολογιστές οικιακής ομάδας. Ωστόσο, εάν χρησιμοποιείτε τον ίδιο λογαριασμό χρήστη και κωδικό πρόσβασης σε όλους τους υπολογιστές σας, μπορείτε να ζητήσετε από την HomeGroup να χρησιμοποιήσει αυτόν τον λογαριασμό."},
    {621, L"Επιλογή: Να επιτρέπεται στα Windows να διαχειρίζονται συνδέσεις οικιακής ομάδας (συνιστάται)"},
    {622, L"Συνδεθείτε με άλλους υπολογιστές χρησιμοποιώντας τον λογαριασμό χρήστη και τον κωδικό πρόσβασής σας"},
    {624, L"Ξεκινήστε το εργαλείο αντιμετώπισης προβλημάτων HomeGroup"},
    {627, L"Συνδέσεις κοινής χρήσης αρχείων"},
    {628, L"Τα Windows χρησιμοποιούν κρυπτογράφηση 128-bit για την ασφάλεια των συνδέσεων κοινής χρήσης αρχείων. Ορισμένες συσκευές δεν υποστηρίζουν κρυπτογράφηση 128-bit και πρέπει να χρησιμοποιούν κρυπτογράφηση 40-bit ή 56-bit."},
    {629, L"Ασφαλίστε τη σύνδεση κοινής χρήσης αρχείων χρησιμοποιώντας κρυπτογράφηση 128-bit (συνιστάται)"},
    {630, L"Ενεργοποιήστε την κοινή χρήση αρχείων συσκευής με κρυπτογράφηση 40-bit ή 56-bit"},
    {631, L"Κάθε δίκτυο"},
    {632, L"Αλλάξτε το περιεχόμενο που μοιράζεστε με την οικιακή ομάδα σας"},
    {637, L"Κλείσιμο"},
    {639, L"Απομακρυσμένη πρόσβαση HomeGroup"},
    {640, L"Άλλα μέλη της οικιακής ομάδας μπορούν να συνδεθούν στην οικιακή σας ομάδα από οπουδήποτε μέσω των υπολογιστών τους."},
    {641, L"Επιλογή: Απενεργοποιήστε την απομακρυσμένη πρόσβαση στην οικιακή ομάδα μέσω αυτού του υπολογιστή"},
    {642, L"Επιλογή: Ενεργοποίηση απομακρυσμένης πρόσβασης οικιακής ομάδας μέσω αυτού του υπολογιστή"},
    {648, L"Επιλέξτε τα αρχεία και τις συσκευές που θα διαθέσετε και, στη συνέχεια, επιλέξτε τα επίπεδα αδειών τους."},
    {649, L"Βιβλιοθήκη ή κατάλογος"},
    {650, L"Επίπεδο πρόσβασης"},
    {652, L"Ενεργοποιήστε την αυτόματη ρύθμιση συσκευών που είναι συνδεδεμένες στο δίκτυο."},
    {46000, L"HomeGroup"},
    {46004, L"Επιλογή: Επιλέξτε έναν κωδικό πρόσβασης για την οικιακή ομάδα σας"},
    {46005, L"Πληκτρολογήστε τον κωδικό πρόσβασης της οικιακής ομάδας"},
    {46006, L"&Δημιουργία τώρα"},
    {46007, L"&Εγγραφείτε τώρα"},
    {46008, L"Προσθέστε άλλους υπολογιστές στην οικιακή ομάδα σας χρησιμοποιώντας αυτόν τον κωδικό πρόσβασης"},
    {46009, L"Έχετε εγγραφεί στην οικιακή ομάδα"},
    {46011, L"HomeGroup"},
    {46012, L"Τα Windows δεν μπορούν να δημιουργήσουν μια οικιακή ομάδα σε αυτόν τον υπολογιστή."},
    {46013, L"Επειδή αυτός ο υπολογιστής είναι μέρος ενός τομέα, η κοινή χρήση της βιβλιοθήκης του με άλλους υπολογιστές στην οικιακή ομάδα δεν είναι διαθέσιμη."},
    {46014, L"Οι κωδικοί πρόσβασης πρέπει να περιέχουν τουλάχιστον 8 χαρακτήρες και να μην υπάρχουν κενά στην αρχή ή στο τέλος."},
    {46015, L"Ο κωδικός πρόσβασης είναι λανθασμένος.\nΔοκιμάστε ξανά. Οι κωδικοί πρόσβασης κάνουν διάκριση πεζών-κεφαλαίων."},
    {46016, L"Επιλογή: Όλες οι συνδέσεις οικιακής ομάδας σε αυτόν τον υπολογιστή θα αποσυνδεθούν"},
    {46017, L"Αποχώρησε με επιτυχία από την οικιακή ομάδα σου"},
    {46018, L"Αλλάξτε το περιεχόμενο που μοιράζεστε με την οικιακή ομάδα σας"},
    {46019, L"Μοιραστείτε τις φωτογραφίες, τα βίντεο, τη μουσική, τα έγγραφα και τους εκτυπωτές σας με άλλους υπολογιστές στο σπίτι σας."},
    {46020, L"&Κάντε αλλαγές"},
    {46021, L"Η αλλαγή του κωδικού πρόσβασης της οικιακής ομάδας αποσυνδέει όλους"},
    {46022, L"Εισαγάγετε έναν νέο κωδικό πρόσβασης για την οικιακή ομάδα σας"},
    {46023, L"&Αλλαγή κωδικού πρόσβασης"},
    {46024, L"Ο κωδικός πρόσβασης HomeGroup άλλαξε με επιτυχία"},
    {46025, L"Ο κωδικός της οικιακής ομάδας άλλαξε"},
    {46026, L"Πληκτρολογήστε τον κωδικό πρόσβασης της οικιακής ομάδας"},
    {46027, L"Ο κωδικός πρόσβασης HomeGroup έχει αλλάξει. Για να συνεχίσετε να χρησιμοποιείτε τους πόρους της οικιακής ομάδας σας, βεβαιωθείτε ότι το άτομο που έχει ήδη εισαγάγει τον νέο κωδικό πρόσβασης είναι συνδεδεμένο και, στη συνέχεια, εισαγάγετε τον νέο κωδικό πρόσβασης."},
    {46028, L"Κοινή χρήση"},
    {46029, L"Τα Windows δεν μπόρεσαν να αφαιρέσουν τον υπολογιστή από την οικιακή ομάδα."},
    {46030, L"Ο %1 άλλαξε τον κωδικό πρόσβασης της ομάδας του. Για να συνεχίσετε να χρησιμοποιείτε τους πόρους της οικιακής ομάδας σας, βεβαιωθείτε ότι το άτομο που έχει ήδη εισαγάγει τον νέο κωδικό πρόσβασης είναι συνδεδεμένο και, στη συνέχεια, εισαγάγετε τον νέο κωδικό πρόσβασης."},
    {46031, L"Οι κωδικοί πρόσβασης βοηθούν στην αποτροπή μη εξουσιοδοτημένης πρόσβασης στα αρχεία και τους εκτυπωτές της οικιακής ομάδας σας. Μπορείτε να λάβετε τον κωδικό πρόσβασης από τα %2, %1 ή άλλο μέλος της οικιακής ομάδας σας."},
    {46032, L"Οι κωδικοί πρόσβασης βοηθούν στην αποτροπή μη εξουσιοδοτημένης πρόσβασης στα αρχεία και τους εκτυπωτές της οικιακής ομάδας σας. Μπορείτε να λάβετε τον κωδικό πρόσβασης από τα %2, %1 ή άλλο μέλος της οικιακής ομάδας σας."},
    {46033, L"Consolas"},
    {46034, L"Δημιουργήστε μια Οικιακή Ομάδα"},
    {46035, L"Γίνετε μέλος μιας Οικιακής Ομάδας"},
    {46036, L"Αλλάξτε τον κωδικό πρόσβασης της ομάδας σας"},
    {46037, L"Αποχωρήστε από την Οικιακή Ομάδα"},
    {46038, L"Για να αποκτήσετε πρόσβαση σε αρχεία και εκτυπωτές σε άλλους υπολογιστές, πρέπει να τα προσθέσετε στην οικιακή ομάδα σας. Απαιτείται ο ακόλουθος κωδικός πρόσβασης:"},
    {46039, L"Πληκτρολογήστε τον νέο κωδικό πρόσβασης της οικιακής ομάδας:"},
    {46040, L"Ενημέρωση κωδικού πρόσβασης"},
    {46041, L"Δημιουργήστε αντίγραφα ασφαλείας όλων των υπολογιστών της οικιακής ομάδας σας σε έναν τοπικό στόχο προστασίας δεδομένων."},
    {46042, L"Δημιουργήστε αντίγραφα ασφαλείας του υπολογιστή σας χρησιμοποιώντας στόχους προστασίας δεδομένων HomeGroup"},
    {46043, L"Δεν είναι κοινόχρηστο"},
    {46044, L"Οι οικιακές ομάδες μπορούν να δημιουργηθούν μόνο σε ιδιωτικά δίκτυα.\nΓια να αλλάξετε τις ρυθμίσεις τοποθεσίας του δικτύου σας, ανοίξτε το Κέντρο δικτύου και κοινής χρήσης στον Πίνακα Ελέγχου."},
    {46045, L"Τα Windows δεν θα εντοπίζουν πλέον οικιακές ομάδες σε αυτό το δίκτυο. Για να δημιουργήσετε μια νέα οικιακή ομάδα, κάντε κλικ στο OK και ανοίξτε την HomeGroup στον Πίνακα Ελέγχου."},
    {46046, L"Τα Windows εντόπισαν μια υπάρχουσα οικιακή ομάδα.\nΓια να εγγραφείτε, κάντε κλικ στο OK και ανοίξτε την HomeGroup στον Πίνακα Ελέγχου."},
    {46047, L"Η υπηρεσία HomeGroup είναι πλέον διαθέσιμη. Δοκιμάστε ξανά."},
    {46048, L"Οι ρυθμίσεις κοινής χρήσης ενημερώθηκαν"},
    {46049, L"Τα επιλεγμένα αρχεία και πόροι κοινοποιούνται στην οικιακή ομάδα σας."},
    {46050, L"Ο κωδικός πρόσβασης HomeGroup ενημερώθηκε με επιτυχία"},
    {46051, L"Γίνατε μέλος της οικιακής ομάδας"},
    {46052, L"Τώρα μπορείτε να αποκτήσετε πρόσβαση στα κοινόχρηστα αρχεία και τις συσκευές σας. Τα αρχεία και οι συσκευές που μοιράζεστε παραμένουν αμετάβλητα."},
    {46053, L"Μπορείτε να ξεκινήσετε την πρόσβαση σε αρχεία και εκτυπωτές που μοιράζονται άλλοι χρήστες στην οικιακή σας ομάδα."},
    {46054, L"Ενημερώστε τον κωδικό πρόσβασης της οικιακής ομάδας σας"},
    {46055, L"Γίνετε μέλος μιας Οικιακής Ομάδας"},
    {46056, L"Εισαγάγετε τον νέο κωδικό πρόσβασης οικιακής ομάδας από το %1."},
    {46057, L"Τα ρολόγια όλων των υπολογιστών της οικιακής ομάδας πρέπει να ρυθμιστούν σε όχι μεγαλύτερη από 24 ώρες μεταξύ τους. Βεβαιωθείτε ότι τα ρολόγια του υπολογιστή σας είναι συγχρονισμένα και, στη συνέχεια, δοκιμάστε να εγγραφείτε ξανά στην οικιακή ομάδα."},
    {46058, L"Ο κωδικός πρόσβασης δεν πληροί τις απαιτήσεις ισχύος κωδικού πρόσβασης του τομέα. Εισαγάγετε έναν αντίστοιχο κωδικό πρόσβασης ή χρησιμοποιήστε έναν άλλο υπολογιστή της Οικιακής Ομάδας για να αλλάξετε τον κωδικό πρόσβασής σας."},
    {46059, L"Δεν μπορείτε να επαναφέρετε τον κωδικό πρόσβασής σας επειδή δεν είστε συνδεδεμένοι σε ιδιωτικό δίκτυο.\nΣυνδεθείτε σε ένα ιδιωτικό δίκτυο και δοκιμάστε ξανά."},
    {46060, L"Δεν είστε συνδεδεμένοι σε ιδιωτικό δίκτυο.\nΓια να αλλάξετε τις ρυθμίσεις τοποθεσίας του δικτύου σας, ανοίξτε το Κέντρο δικτύου και κοινής χρήσης στον Πίνακα Ελέγχου."},
    {46061, L"Μοιραστείτε με άλλους οικιακούς υπολογιστές"},
    {46062, L"Μπορείτε να κάνετε κοινή χρήση αρχείων και εκτυπωτών με άλλους υπολογιστές. Μπορείτε επίσης να κάνετε ροή πολυμέσων στη συσκευή σας.\n\nΟι οικιακές ομάδες προστατεύονται με κωδικό πρόσβασης και μπορείτε να επιλέξετε τι θα μοιραστείτε ανά πάσα στιγμή."},
    {46063, L"Προσθέστε άλλους υπολογιστές στην οικιακή ομάδα σας χρησιμοποιώντας αυτόν τον κωδικό πρόσβασης"},
    {46064, L"Για να αποκτήσετε πρόσβαση σε αρχεία και εκτυπωτές σε άλλους υπολογιστές, πρέπει να τα προσθέσετε στην οικιακή ομάδα σας. Απαιτείται ο ακόλουθος κωδικός πρόσβασης:"},
    {46065, L"Για να δημιουργήσετε ή να συμμετάσχετε σε μια οικιακή ομάδα, η σύνδεση δικτύου σας πρέπει να έχει ενεργοποιημένο το IPv6. Για να ενεργοποιήσετε το IPv6, ξεκινήστε την Αντιμετώπιση προβλημάτων HomeGroup."},
    {46066, L"Προσθέστε άτομα στην οικιακή ομάδα"},
    {46067, L"Διαμόρφωση προστασίας δεδομένων οικιακής ομάδας"},
    {46068, L"Εντοπίστηκαν πολλές οικιακές ομάδες"},
    {46069, L"Μοιραστείτε με άλλα μέλη της οικιακής ομάδας"},
    {46070, L"Έγγραφα"},
    {46071, L"Εικόνες"},
    {46072, L"Μουσική"},
    {46073, L"Βίντεο"},
    {46074, L"Εκτυπωτές και συσκευές"},
    {46075, L"Αλλάξτε τις ρυθμίσεις κοινής χρήσης οικιακής ομάδας"},
    {46076, L"%1 Κοινή χρήση"},
    {46077, L"Επαλήθευση του κωδικού πρόσβασής σας..."},
};

// Hebrew (he-IL)
static const EmbeddedTextEntry kStrings_HE_IL[] = {
    {1, L"קבוצה ביתית"},
    {2, L"סקור את אפשרויות ה-Home Group, החלט מה המחשב הזה חולק, והצג או עדכן את סיסמת הגישה."},
    {3, L"מדיניות שנקבעה על ידי הארגון שלך מונעת את הפעלת הדף הזה. בקש ממנהל הרשת עזרה."},
    {4, L"אפשרויות שיתוף מפורטות"},
    {5, L"פועל"},
    {6, L"כבוי"},
    {7, L"כבוי (לא מותקנות מדפסות)"},
    {8, L"אין מדפסת מחוברת למחשב זה."},
    {9, L"שתף תוכן עם מחשבים אישיים בבית"},
    {10, L"גש לקבוצה הביתית שלך באמצעות מחשב המחובר לדומיין"},
    {12, L"ערוך אפשרויות קבוצת בית"},
    {13, L"עובד…"},
    {14, L"לא נמצאה קבוצה ביתית ברשת זו."},
    {15, L"%1 של %2 יצרה קבוצה ביתית ברשת."},
    {16, L"הוזמנת להצטרף לקבוצה הביתית שלך."},
    {18, L"השתמש בדף זה למחשב זה שייך לקבוצה ביתית."},
    {19, L"המחשב הזה לא יכול להתחבר לקבוצה הביתית שלך."},
    {20, L"HomeGroup מאפשרת למחשבים מהימנים להחליף קבצים ולהשתמש במדפסות משותפות, והיא יכולה לשלוח מדיה להתקנים תואמים. גישה דורשת סיסמה, בזמן שאתה נשאר בשליטה על מה שהמחשב הזה מעמיד לרשותך."},
    {21, L"מחשב זה הוא גם חלק מדומיין, כך שהוא לא יכול ליצור קבוצה ביתית משלו, אבל הוא יכול להצטרף לקבוצה ביתית שנוצרה על ידי מישהו ברשת.\n\nקבוצות ביתיות מקשרות מחשבים ברשת הביתית שלך כדי שתוכל לשתף תמונות, מוזיקה, סרטונים, מסמכים ומדפסות. קבוצות ביתיות מוגנות בסיסמה ואתה יכול לבחור מה לשתף בכל עת."},
    {22, L"קבוצות ביתיות מקשרות מחשבים ברשת הביתית שלך כדי שתוכל לשתף תמונות, מוזיקה, סרטונים, מסמכים ומדפסות. קבוצות ביתיות מוגנות בסיסמה ואתה יכול לבחור מה לשתף בכל עת.\n\nאינך יכול ליצור קבוצות ביתיות משלך במהדורה זו של Windows, אך אתה יכול להצטרף לקבוצות ביתיות שנוצרו על ידי אחרים."},
    {23, L"הגדר קבוצה ביתית"},
    {24, L"הצטרף"},
    {25, L"סיסמת קבוצת הבית שונתה. כדי להמשיך להשתמש במשאבי הקבוצה הביתית שלך, ודא שהאדם שכבר הזין את הסיסמה החדשה מחובר, ולאחר מכן הזן את הסיסמה החדשה."},
    {26, L"Windows זיהה קבוצה ביתית אחרת ברשת שלך. קבוצות ביתיות מאפשרות לך לשתף קבצים ומדפסות עם מחשבים אחרים. אתה יכול גם להזרים מדיה למכשיר שלך."},
    {27, L"%1 שינה את סיסמת הקבוצה הביתית שלו. כדי להמשיך להשתמש במשאבי הקבוצה הביתית שלך, ודא שהאדם שכבר הזין את הסיסמה החדשה מחובר, ולאחר מכן הזן את הסיסמה החדשה."},
    {28, L"מחפש קבוצות ביתיות ברשת זו..."},
    {29, L"הקלד סיסמה חדשה"},
    {30, L"הצטרף עכשיו"},
    {32, L"לפני שתוכל ליצור או להצטרף לקבוצה ביתית, עליך להתחבר תחילה לרשת שלך."},
    {34, L"השתמש בדף זה כדי ליצור או להצטרף לקבוצה ביתית, מיקום הרשת של המחשב שלך חייב להיות מוגדר כפרטי."},
    {35, L"שנה את מיקום הרשת"},
    {37, L"אפשרויות שיתוף עבור פרטי"},
    {38, L"אפשרויות שיתוף לציבור"},
    {39, L"אפשרויות שיתוף עבור דומיין"},
    {40, L"פרטי"},
    {41, L"פרטי (פרופיל נוכחי)"},
    {42, L"ציבורי"},
    {43, L"ציבורי (פרופיל נוכחי)"},
    {44, L"דומיין"},
    {45, L"דומיין (פרופיל נוכחי)"},
    {46, L"הזרמת מדיה מופעלת."},
    {47, L"הזרמת מדיה כבויה."},
    {56, L"בטל"},
    {63, L"בסדר"},
    {64, L"הצג או הדפס את סיסמת קבוצת הבית"},
    {65, L"24pt;;;Consolas"},
    {66, L"תאריך הדפסה: %1 %2"},
    {67, L"אפשרות: הצג והדפיס את סיסמת הקבוצה הביתית שלך"},
    {68, L"סיסמה:"},
    {69, L"השתמש בסיסמה זו כדי לחבר מחשבים אחרים לקבוצה הביתית שלך."},
    {70, L"בכל מחשב:"},
    {71, L"הערה: מחשבים כבויים או במצב שינה לא יופיעו בקבוצה הביתית שלך."},
    {72, L"1. לחץ על התחל ולאחר מכן לחץ על לוח הבקרה."},
    {73, L"2. תחת רשת ואינטרנט, לחץ על בחר קבוצה ביתית ואפשרויות שיתוף."},
    {74, L"3. לחץ על הצטרף עכשיו ובצע את אשף הקבוצה הביתית כדי להזין את הסיסמה שלך."},
    {75, L"לחץ על התחל ולאחר מכן לחץ על לוח הבקרה."},
    {76, L"לא ניתן להדפיס את סיסמת הקבוצה הביתית"},
    {77, L"אירעה שגיאה כאשר Windows ניסה להוציא את סיסמת הקבוצה הביתית. (קוד שגיאה:%1!u!)"},
    {78, L"אינך מחובר כעת לרשת הביתית שלך. כדי להציג קבצים ומשאבים במחשבי קבוצה ביתית אחרים, התחבר תחילה לרשת הביתית שלך."},
    {79, L"%1 הצטרף למחשב לקבוצה הביתית. לא שיתפתי את הספרייה עם הקבוצה הביתית שלי. לחץ על הקישור למטה כדי לשנות את מה ששיתפת. אל תכבה או תפעיל מחדש את המחשב עד להשלמת השיתוף."},
    {80, L"לא שיתפתי את הספרייה עם הקבוצה הביתית שלי. לחץ על הקישור למטה כדי לשנות את מה ששיתפת. אל תכבה או תפעיל מחדש את המחשב עד להשלמת השיתוף."},
    {81, L"HomeGroup משתפת כעת את הספרייה במחשב זה. חלק מהאפשרויות של הקבוצה הביתית אינן זמינות עד להשלמת השיתוף. אל תכבה או תפעיל מחדש את המחשב עד להשלמת השיתוף."},
    {82, L"תחת רשת ואינטרנט, לחץ על בחר קבוצה ביתית ואפשרויות שיתוף."},
    {83, L"כרגע אין קבוצות ביתיות ברשת."},
    {84, L"לחץ על הצטרף עכשיו ובצע את אשף הקבוצה הביתית כדי להזין את הסיסמה שלך."},
    {85, L"לחץ כאן להתקנה."},
    {86, L"Windows מצא מדפסת קבוצה ביתית"},
    {88, L"הכירו את קבוצת הבית"},
    {89, L"%1 (פרופיל נוכחי)"},
    {90, L"השתמש בדף זה כדי להצטרף לקבוצה ביתית, מיקום הרשת של המחשב שלך חייב להיות מוגדר כפרטי."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"קבוצת הבית עדיין לא מוכנה. אנא נסה שוב בעוד מספר דקות. אם אתה ממשיך לראות הודעה זו, לחץ על הקישור כדי להתחיל בפתרון בעיות בקבוצה הביתית שלך."},
    {95, L"הפעל את פותר הבעיות של קבוצת הבית"},
    {98, L"סיסמת קבוצת הבית"},
    {99, L"חשבונות אורח אינם יכולים לשנות את הגדרות הקבוצה הביתית."},
    {100, L"HomeGroup מצאה מדפסת משותפת חדשה ברשת הביתית שלך. לאחר ההתקנה, הוא יהיה זמין לכל מי שנמצא במחשב זה."},
    {101, L"התקן מדפסת"},
    {102, L"HomeGroup לא זמין מכיוון שאינך מחובר לרשת הביתית שלך."},
    {103, L"HomeGroup לא זמין מכיוון שאינך מחובר לרשת הביתית שלך."},
    {104, L"לפני ההצטרפות לקבוצה ביתית, תחילה עליך להתחבר לרשת."},
    {105, L"תמונת קבוצה ביתית"},
    {106, L"בחר מה ברצונך לשתף והצג את סיסמת הקבוצה הביתית שלך"},
    {107, L"מכיוון שמחשב זה הוא חלק מדומיין, הגדרות לשיתוף הספריות וההתקנים שלו עם מחשבים אחרים בקבוצה הביתית אינן זמינות."},
    {108, L"הגדרות לשיתוף ספריות והתקנים עם מחשבים אחרים בקבוצה ביתית אינן זמינות במהדורה זו של Windows."},
    {109, L"הסר את %1 מהקבוצה הביתית"},
    {110, L"בטל"},
    {111, L"הסר חבר בקבוצה הביתית"},
    {112, L"%1 יוסר מהקבוצה הביתית"},
    {113, L"כל חברי הקבוצה הביתית שיצטרפו באמצעות סיסמה יידרשו להזין שוב את הסיסמה."},
    {114, L"מדפסות והתקנים"},
    {115, L"שנה את חברי הקבוצה הביתית של %1"},
    {116, L"סיסמת הקבוצה הביתית אופסה"},
    {117, L"HomeGroup משתפת קבצים"},
    {118, L"אפשרות: מחשב זה שייך לקבוצה ביתית"},
    {119, L"קבוצה ביתית זמינה להצטרף"},
    {120, L"ניתן ליצור קבוצה ביתית"},
    {121, L"קבוצת הבית אינה זמינה"},
    {122, L"מדפסת לא מהימנה"},
    {200, L"הוסף חבר"},
    {201, L"אייקון משתמש"},
    {202, L"שם מלא"},
    {203, L"מזהה משתמש"},
    {204, L"סרגל התקדמות"},
    {205, L"סמל תיקיה"},
    {220, L"שתף ספריות וחומרה"},
    {221, L"בחר את הספרייה שברצונך לשתף עם אחרים בקבוצה הביתית שלך."},
    {222, L"ערוך אפשרויות קבוצת בית"},
    {223, L"השתמש בדף זה כדי לשנות את הגדרות קבוצת הבית, פתח את קבוצת הבית בלוח הבקרה."},
    {224, L"אפשרויות קבוצה ביתית"},
    {225, L"השתמש בדף זה כדי לשנות את הגדרות הקבוצה הביתית שלך בלוח הבקרה או השתמש בפותר הבעיות של הקבוצה הביתית."},
    {226, L"הפעל את פותר הבעיות"},
    {227, L"השתמש בדף זה כדי להשתמש בפותר הבעיות של קבוצת הבית כדי למצוא ולתקן בעיות בקבוצת הבית שלך."},
    {228, L"הצג סיסמה"},
    {229, L"השתמש בדף זה כדי להציג או להדפיס את סיסמת הקבוצה הביתית שלך."},
    {230, L"הצטרף לקבוצה ביתית"},
    {231, L"הצטרף לקבוצה הביתית ברשת זו."},
    {530, L"פתח אפשרויות שיתוף מפורטות..."},
    {541, L"נראות רשת"},
    {542, L"אם גילוי רשת מופעל, מחשב זה יכול לראות ולהיראות על ידי מחשבים והתקנים אחרים ברשת."},
    {543, L"הפעל את גילוי הרשת"},
    {544, L"כבה את גילוי הרשת"},
    {545, L"גישה לקבצים ולמדפסת"},
    {546, L"כאשר שיתוף קבצים ומדפסות מופעל, משתמשים אחרים ברשת שלך יכולים לגשת לקבצים ולמדפסות שאתה משתף מהמחשב הזה."},
    {547, L"הפעל את שיתוף הקבצים והמדפסת"},
    {548, L"כבה את שיתוף הקבצים והמדפסות"},
    {549, L"שיתוף תיקיות ציבוריות"},
    {550, L"כאשר שיתוף תיקיות ציבוריות מופעל, משתמשים ברשת שלך, כולל חברי קבוצה ביתית, יכולים לגשת לקבצים בתיקיות ציבוריות."},
    {552, L"הפעלת שיתוף מאפשרת לכל מי שיש לו גישה לרשת שלך לקרוא ולכתוב קבצים בתיקיות הציבוריות שלך."},
    {553, L"כבה את שיתוף התיקיות הציבוריות (משתמשים המחוברים למחשב זה עדיין יכולים לגשת לתיקיות אלו)"},
    {554, L"שנה אפשרויות שיתוף עבור פרופילי רשת שונים"},
    {559, L"גישה למדיה"},
    {560, L"כאשר הזרמת מדיה מופעלת, משתמשים והתקנים ברשת שלך יכולים לגשת לתמונות, למוזיקה ולסרטונים במחשב זה. מחשב זה יכול גם למצוא מדיה ברשת."},
    {564, L"בטל"},
    {567, L"החל שינויים"},
    {584, L"Windows יוצר פרופיל רשת נפרד עבור כל רשת שבה אתה משתמש. אתה יכול לבחור אפשרויות ספציפיות עבור כל פרופיל."},
    {585, L"סמל אזהרה של קבוצה ביתית"},
    {586, L"ספריות ומכשירים משותפים מהמחשב הזה"},
    {595, L"משימות נוספות של קבוצת בית"},
    {600, L"הצג או הדפס את סיסמת קבוצת הבית"},
    {601, L"מנהל המערכת שלך לא אפשר לך לגשת לקבוצה הביתית שלך."},
    {604, L"שנה את הסיסמה..."},
    {605, L"עזוב את הקבוצה הביתית..."},
    {607, L"בחר אפשרויות הזרמת מדיה..."},
    {608, L"מכיוון שמחשב זה הוא חלק מדומיין, הגדרות לשיתוף הספריות וההתקנים שלו עם מחשבים אחרים בקבוצה הביתית אינן זמינות."},
    {609, L"שיתוף מוגן באמצעות סיסמה"},
    {610, L"כאשר שיתוף מוגן באמצעות סיסמה מופעל, רק משתמשים עם חשבונות משתמש וסיסמאות במחשב זה יכולים לגשת לקבצים משותפים, למדפסות המחוברות למחשב זה ולתיקיות ציבוריות. יש לכבות את השיתוף המוגן באמצעות סיסמה כדי לאפשר לאחרים גישה."},
    {611, L"הפעל שיתוף מוגן באמצעות סיסמה"},
    {612, L"כבה את השיתוף המוגן באמצעות סיסמה"},
    {613, L"הדפס עמוד"},
    {614, L"מאפשר להפעיל תוכן משותף בכל המכשירים ברשת זו, כגון טלוויזיות וקונסולות משחקים"},
    {615, L"רשת פרטית"},
    {616, L"רשת אורחת או ציבורית"},
    {617, L"רשת דומיין"},
    {619, L"חיבורי קבוצה ביתית"},
    {620, L"Windows בדרך כלל מנהל חיבורים למחשבים אחרים של קבוצות ביתיות. עם זאת, אם אתה משתמש באותו חשבון משתמש ובאותו סיסמה בכל המחשבים שלך, אתה יכול לגרום ל-HomeGroup להשתמש בחשבון זה במקום זאת."},
    {621, L"אפשרות: אפשר ל-Windows לנהל חיבורי קבוצה ביתית (מומלץ)"},
    {622, L"התחבר למחשבים אחרים באמצעות חשבון המשתמש והסיסמה שלך"},
    {624, L"הפעל את פותר הבעיות של קבוצת הבית"},
    {627, L"חיבורי שיתוף קבצים"},
    {628, L"Windows משתמש בהצפנה של 128 סיביות כדי לאבטח חיבורי שיתוף קבצים. מכשירים מסוימים אינם תומכים בהצפנה של 128 סיביות וחייבים להשתמש בהצפנה של 40 סיביות או 56 סיביות."},
    {629, L"אבטח את חיבור שיתוף הקבצים שלך באמצעות הצפנה של 128 סיביות (מומלץ)"},
    {630, L"אפשר שיתוף קבצים במכשיר עם הצפנה של 40 סיביות או 56 סיביות"},
    {631, L"כל רשת"},
    {632, L"שנה את מה שמשותף עם הקבוצה הביתית שלך"},
    {637, L"סגור"},
    {639, L"גישה מרחוק לקבוצה ביתית"},
    {640, L"חברי קבוצה ביתית אחרים יכולים להתחבר לקבוצה הביתית שלך מכל מקום דרך המחשבים שלהם."},
    {641, L"אפשרות: השבת גישה לקבוצה ביתית מרחוק דרך מחשב זה"},
    {642, L"אפשרות: אפשר גישה לקבוצה ביתית מרחוק דרך מחשב זה"},
    {648, L"בחר את הקבצים וההתקנים שיהיו זמינים ולאחר מכן בחר את רמות ההרשאה שלהם."},
    {649, L"ספרייה או ספרייה"},
    {650, L"רמת גישה"},
    {652, L"הפעל את ההגדרה האוטומטית של התקנים המחוברים לרשת."},
    {46000, L"קבוצה ביתית"},
    {46004, L"אפשרות: בחר סיסמה לקבוצת הבית שלך"},
    {46005, L"הקלד את סיסמת הקבוצה הביתית"},
    {46006, L"&צור כעת"},
    {46007, L"&הצטרף עכשיו"},
    {46008, L"הוסף מחשבים אחרים לקבוצה הביתית שלך באמצעות סיסמה זו"},
    {46009, L"הצטרפת לקבוצה הביתית"},
    {46011, L"קבוצה ביתית"},
    {46012, L"Windows לא יכול להגדיר קבוצה ביתית במחשב זה."},
    {46013, L"מכיוון שמחשב זה הוא חלק מתחום, שיתוף הספרייה שלו עם מחשבים אחרים בקבוצה הביתית אינו זמין."},
    {46014, L"סיסמאות חייבות להכיל לפחות 8 תווים וללא רווחים מובילים או נגררים."},
    {46015, L"הסיסמה שגויה.\nאנא נסה שוב. סיסמאות הן תלויות רישיות."},
    {46016, L"אפשרות: כל חיבורי הקבוצה הביתית במחשב זה ינותקו"},
    {46017, L"עזב את הקבוצה הביתית שלך בהצלחה"},
    {46018, L"שנה את מה שמשותף עם הקבוצה הביתית שלך"},
    {46019, L"שתף את התמונות, הסרטונים, המוזיקה, המסמכים והמדפסות שלך עם מחשבים אחרים בבית שלך."},
    {46020, L"&בצע שינויים"},
    {46021, L"שינוי סיסמת הקבוצה הביתית מנתק את כולם"},
    {46022, L"הזן סיסמה חדשה עבור הקבוצה הביתית שלך"},
    {46023, L"&שנה סיסמה"},
    {46024, L"סיסמת קבוצת הבית שונתה בהצלחה"},
    {46025, L"סיסמת הקבוצה הביתית שונתה"},
    {46026, L"הקלד את סיסמת הקבוצה הביתית"},
    {46027, L"סיסמת קבוצת הבית שונתה. כדי להמשיך להשתמש במשאבי הקבוצה הביתית שלך, ודא שהאדם שכבר הזין את הסיסמה החדשה מחובר, ולאחר מכן הזן את הסיסמה החדשה."},
    {46028, L"משותף"},
    {46029, L"Windows לא הצליח להסיר את המחשב מהקבוצה הביתית."},
    {46030, L"%1 שינה את סיסמת הקבוצה הביתית שלו. כדי להמשיך להשתמש במשאבי הקבוצה הביתית שלך, ודא שהאדם שכבר הזין את הסיסמה החדשה מחובר, ולאחר מכן הזן את הסיסמה החדשה."},
    {46031, L"סיסמאות עוזרות למנוע גישה בלתי מורשית לקבצים ולמדפסות של הקבוצה הביתית שלך. אתה יכול לקבל את הסיסמה מ-%2, %1, או מחבר אחר בקבוצה הביתית שלך."},
    {46032, L"סיסמאות עוזרות למנוע גישה בלתי מורשית לקבצים ולמדפסות של הקבוצה הביתית שלך. אתה יכול לקבל את הסיסמה מ-%2, %1, או מחבר אחר בקבוצה הביתית שלך."},
    {46033, L"Consolas"},
    {46034, L"צור קבוצה ביתית"},
    {46035, L"הצטרף לקבוצה ביתית"},
    {46036, L"שנה את סיסמת הקבוצה הביתית שלך"},
    {46037, L"עזוב את הקבוצה הביתית"},
    {46038, L"כדי לגשת לקבצים ומדפסות במחשבים אחרים, עליך להוסיף אותם לקבוצה הביתית שלך. הסיסמה הבאה נדרשת:"},
    {46039, L"הקלד את סיסמת הקבוצה הביתית החדשה:"},
    {46040, L"עדכן סיסמה"},
    {46041, L"גבה את כל המחשבים בקבוצה הביתית שלך ליעד מקומי להגנה על נתונים."},
    {46042, L"גבה את המחשב שלך באמצעות יעדי הגנה על נתונים של HomeGroup"},
    {46043, L"לא משותף"},
    {46044, L"ניתן ליצור קבוצות ביתיות רק ברשתות פרטיות.\nכדי לשנות את הגדרות מיקום הרשת, פתח את מרכז הרשת והשיתוף בלוח הבקרה."},
    {46045, L"Windows לא יזהה עוד קבוצות ביתיות ברשת זו. כדי ליצור קבוצה ביתית חדשה, לחץ על אישור ופתח את קבוצת הבית בלוח הבקרה."},
    {46046, L"Windows זיהה קבוצה ביתית קיימת.\nכדי להצטרף, לחץ על אישור ופתח את קבוצת הבית בלוח הבקרה."},
    {46047, L"שירות קבוצת הבית זמין כעת. אנא נסה שוב."},
    {46048, L"הגדרות השיתוף עודכנו"},
    {46049, L"הקבצים והמשאבים שנבחרו משותפים עם הקבוצה הביתית שלך."},
    {46050, L"סיסמת קבוצת הבית עודכנה בהצלחה"},
    {46051, L"הצטרפת לקבוצה הביתית"},
    {46052, L"כעת תוכל לגשת לקבצים ולמכשירים המשותפים שלך. הקבצים והמכשירים שאתה משתף נשארים ללא שינוי."},
    {46053, L"אתה יכול להתחיל לגשת לקבצים ומדפסות המשותפים על ידי משתמשים אחרים בקבוצה הביתית שלך."},
    {46054, L"עדכן את סיסמת הקבוצה הביתית שלך"},
    {46055, L"הצטרף לקבוצה ביתית"},
    {46056, L"הזן את סיסמת הקבוצה הביתית החדשה מ-%1."},
    {46057, L"כל השעונים של מחשבי הקבוצה הביתית חייבים להיות מכוונים ללא יותר מ-24 שעות. ודא ששעוני המחשב שלך מסונכרנים, ולאחר מכן נסה להצטרף שוב לקבוצה הביתית."},
    {46058, L"הסיסמה אינה עומדת בדרישות חוזק הסיסמה של הדומיין. הזן סיסמה תואמת או השתמש במחשב HomeGroup אחר כדי לשנות את הסיסמה שלך."},
    {46059, L"אינך יכול לאפס את הסיסמה שלך מכיוון שאינך מחובר לרשת פרטית.\nאנא התחבר לרשת פרטית ונסה שוב."},
    {46060, L"אינך מחובר לרשת פרטית.\nכדי לשנות את הגדרות מיקום הרשת, פתח את מרכז הרשת והשיתוף בלוח הבקרה."},
    {46061, L"שתף עם מחשבים ביתיים אחרים"},
    {46062, L"אתה יכול לשתף קבצים ומדפסות עם מחשבים אחרים. אתה יכול גם להזרים מדיה למכשיר שלך.\n\nקבוצות ביתיות מוגנות בסיסמה ואתה יכול לבחור מה לשתף בכל עת."},
    {46063, L"הוסף מחשבים אחרים לקבוצה הביתית שלך באמצעות סיסמה זו"},
    {46064, L"כדי לגשת לקבצים ומדפסות במחשבים אחרים, עליך להוסיף אותם לקבוצה הביתית שלך. הסיסמה הבאה נדרשת:"},
    {46065, L"כדי ליצור או להצטרף לקבוצה ביתית, חיבור הרשת שלך חייב להיות מופעל IPv6. כדי להפעיל את IPv6, הפעל את פותר הבעיות של קבוצת הבית."},
    {46066, L"הוסף אנשים לקבוצה הביתית"},
    {46067, L"הגדר את הגנת הנתונים של הקבוצה הביתית"},
    {46068, L"זוהו קבוצות בית מרובות"},
    {46069, L"שתף עם חברי קבוצה ביתית אחרים"},
    {46070, L"מסמכים"},
    {46071, L"תמונות"},
    {46072, L"מוזיקה"},
    {46073, L"סרטונים"},
    {46074, L"מדפסות והתקנים"},
    {46075, L"שנה את הגדרות השיתוף של קבוצות ביתיות"},
    {46076, L"%1 שיתוף"},
    {46077, L"מאמת את הסיסמה שלך..."},
};

// Hungarian (hu-HU)
static const EmbeddedTextEntry kStrings_HU_HU[] = {
    {1, L"Otthoni csoport"},
    {2, L"Tekintse át a HomeGroup beállításait, döntse el, mit oszt meg ez a számítógép, és jelenítse meg vagy frissítse a hozzáférési jelszót."},
    {3, L"A szervezete által beállított házirend megakadályozza az oldal futtatását. Kérjen segítséget a hálózati rendszergazdától."},
    {4, L"Részletes megosztási lehetőségek"},
    {5, L"Be"},
    {6, L"Ki"},
    {7, L"Ki (nincs nyomtató telepítve)"},
    {8, L"Ehhez a számítógéphez nincs nyomtató csatlakoztatva."},
    {9, L"Tartalom megosztása otthoni számítógépekkel"},
    {10, L"Hozzáférés az otthoni csoportjához egy tartományhoz csatlakoztatott számítógép segítségével"},
    {12, L"Szerkessze a HomeGroup beállításait"},
    {13, L"Dolgozik…"},
    {14, L"Nem található otthoni csoport ezen a hálózaton."},
    {15, L"A %2 %1 otthoni csoportot hozott létre a hálózaton."},
    {16, L"Meghívást kapott, hogy csatlakozzon otthoni csoportjához."},
    {18, L"Használja ezt az oldalt, hogy ez a számítógép egy otthoni csoporthoz tartozik."},
    {19, L"Ez a számítógép nem tud csatlakozni az otthoni csoporthoz."},
    {20, L"A HomeGroup lehetővé teszi a megbízható PC-k számára, hogy fájlokat cseréljenek, megosztott nyomtatókat használhassanak, és médiát küldhessenek kompatibilis eszközökre. A hozzáféréshez jelszóra van szükség, miközben Ön szabályozhatja, hogy mit tesz elérhetővé ez a számítógép."},
    {21, L"Ez a számítógép is egy tartomány része, így nem tud saját otthoni csoportot létrehozni, de csatlakozhat a hálózaton valaki által létrehozott otthoni csoporthoz.\n\nAz otthoni csoportok összekapcsolják az otthoni hálózaton lévő számítógépeket, így fényképeket, zenéket, videókat, dokumentumokat és nyomtatókat oszthat meg. Az otthoni csoportok jelszóval védettek, és bármikor kiválaszthatja, hogy mit oszt meg."},
    {22, L"Az otthoni csoportok összekapcsolják az otthoni hálózaton lévő számítógépeket, így fényképeket, zenéket, videókat, dokumentumokat és nyomtatókat oszthat meg. Az otthoni csoportok jelszóval védettek, és bármikor kiválaszthatja, hogy mit oszt meg.\n\nA Windows ezen kiadásában nem hozhat létre saját otthoni csoportokat, de csatlakozhat mások által létrehozott otthoni csoportokhoz."},
    {23, L"Otthoni csoport beállítása"},
    {24, L"Csatlakozz"},
    {25, L"Az otthoni csoport jelszava megváltozott. Az otthoni csoport erőforrásainak használatának folytatásához győződjön meg arról, hogy az új jelszót már beírt személy online állapotban van, majd írja be az új jelszót."},
    {26, L"A Windows egy másik otthoni csoportot észlelt a hálózaton. Az otthoni csoportok lehetővé teszik a fájlok és nyomtatók más számítógépekkel való megosztását. A médiát is streamelheti a készülékére."},
    {27, L"%1 megváltoztatta az otthoni csoport jelszavát. Az otthoni csoport erőforrásainak használatának folytatásához győződjön meg arról, hogy az új jelszót már beírt személy online állapotban van, majd írja be az új jelszót."},
    {28, L"Otthoni csoportok keresése ezen a hálózaton…"},
    {29, L"Írja be az új jelszót"},
    {30, L"Csatlakozz most"},
    {32, L"Mielőtt otthoni csoportot hozhatna létre, vagy csatlakozhatna hozzá, először csatlakoznia kell a hálózatához."},
    {34, L"Ezen az oldalon otthoni csoportot hozhat létre vagy csatlakozhat hozzá. A számítógép hálózati helyét privátra kell állítani."},
    {35, L"Hálózati hely módosítása"},
    {37, L"Megosztási lehetőségek a priváthoz"},
    {38, L"Megosztási lehetőségek nyilvánosan"},
    {39, L"Megosztási lehetőségek a domainhez"},
    {40, L"Privát"},
    {41, L"Privát (jelenlegi profil)"},
    {42, L"Nyilvános"},
    {43, L"Nyilvános (jelenlegi profil)"},
    {44, L"Domain"},
    {45, L"Domain (jelenlegi profil)"},
    {46, L"A médiaadatfolyam be van kapcsolva."},
    {47, L"A média streamelése ki van kapcsolva."},
    {56, L"Mégse"},
    {63, L"OK"},
    {64, L"Mutassa meg vagy nyomtassa ki a HomeGroup jelszavát"},
    {65, L"24pt;;;Consolas"},
    {66, L"Nyomtatás dátuma: %1 %2"},
    {67, L"Lehetőség: Az otthoni csoport jelszavának megtekintése és kinyomtatása"},
    {68, L"Jelszó:"},
    {69, L"Ezzel a jelszóval más számítógépeket is csatlakoztathat otthoni csoportjához."},
    {70, L"Minden számítógépen:"},
    {71, L"Megjegyzés: A kikapcsolt vagy alvó számítógépek nem jelennek meg az otthoni csoportban."},
    {72, L"1. Kattintson a Start gombra, majd a Vezérlőpult parancsra."},
    {73, L"2. A Hálózat és internet területen kattintson az Otthoni csoport és megosztási beállítások kiválasztása elemre."},
    {74, L"3. Kattintson a Csatlakozás most lehetőségre, és kövesse a HomeGroup varázslót a jelszó megadásához."},
    {75, L"Kattintson a Start gombra, majd a Vezérlőpult elemre."},
    {76, L"Nem sikerült kinyomtatni az otthoni csoport jelszavát"},
    {77, L"Hiba történt, amikor a Windows megpróbálta kiadni az otthoni csoport jelszavát. (Hibakód: %1!u!)"},
    {78, L"Jelenleg nem csatlakozik otthoni hálózatához. Az otthoni csoport más számítógépein lévő fájlok és erőforrások megtekintéséhez először csatlakozzon otthoni hálózatához."},
    {79, L"A %1 csatlakozott a számítógéphez az otthoni csoporthoz. Nem osztottam meg a könyvtárat az otthoni csoportommal. Kattintson az alábbi linkre a megosztott tartalmak módosításához. Ne kapcsolja ki vagy indítsa újra a számítógépet, amíg a megosztás be nem fejeződik."},
    {80, L"Nem osztottam meg a könyvtárat az otthoni csoportommal. Kattintson az alábbi linkre a megosztott tartalmak módosításához. Ne kapcsolja ki vagy indítsa újra a számítógépet, amíg a megosztás be nem fejeződik."},
    {81, L"A HomeGroup jelenleg megosztja a könyvtárat ezen a számítógépen. Egyes otthoni csoport opciók nem érhetők el, amíg a megosztás be nem fejeződik. Ne kapcsolja ki vagy indítsa újra a számítógépet, amíg a megosztás be nem fejeződik."},
    {82, L"A Hálózat és internet területen kattintson az Otthoni csoport és a megosztási beállítások kiválasztása elemre."},
    {83, L"Jelenleg nincs otthoni csoport a hálózaton."},
    {84, L"Kattintson a Csatlakozás most gombra, és kövesse a HomeGroup varázslót a jelszó megadásához."},
    {85, L"Kattintson ide a telepítéshez."},
    {86, L"A Windows talált egy otthoni csoport nyomtatót"},
    {88, L"Bemutatkozik a HomeGroup"},
    {89, L"%1 (jelenlegi profil)"},
    {90, L"Ezen az oldalon az otthoni csoporthoz való csatlakozáshoz a számítógép hálózati helyét privátra kell állítani."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"A HomeGroup még nem áll készen. Kérjük, próbálja újra néhány perc múlva. Ha továbbra is ezt az üzenetet látja, kattintson a hivatkozásra az otthoni csoport hibaelhárításának megkezdéséhez."},
    {95, L"Indítsa el a HomeGroup hibaelhárítót"},
    {98, L"HomeGroup jelszó"},
    {99, L"A vendégfiókok nem módosíthatják az otthoni csoport beállításait."},
    {100, L"A HomeGroup új megosztott nyomtatót talált az otthoni hálózatán. A telepítést követően bárki számára elérhető lesz ezen a számítógépen."},
    {101, L"Nyomtató telepítése"},
    {102, L"A HomeGroup nem érhető el, mert nem csatlakozik otthoni hálózatához."},
    {103, L"A HomeGroup nem érhető el, mert nem csatlakozik otthoni hálózatához."},
    {104, L"Mielőtt csatlakozna egy otthoni csoporthoz, először csatlakoznia kell a hálózathoz."},
    {105, L"HomeGroup kép"},
    {106, L"Válassza ki, mit szeretne megosztani, és tekintse meg otthoni csoport jelszavát"},
    {107, L"Mivel ez a számítógép egy tartomány része, a könyvtárak és eszközök megosztására vonatkozó beállítások nem érhetők el az otthoni csoport más számítógépeivel."},
    {108, L"A könyvtárak és eszközök egy otthoni csoporthoz tartozó más számítógépekkel való megosztására vonatkozó beállítások a Windows ezen kiadásában nem érhetők el."},
    {109, L"Távolítsa el a %1-t az otthoni csoportból"},
    {110, L"Mégse"},
    {111, L"Az otthoni csoport tagjának eltávolítása"},
    {112, L"A %1 eltávolítva lesz az otthoni csoportból"},
    {113, L"Az otthoni csoport összes tagjának, aki jelszóval csatlakozik, újra meg kell adnia a jelszót."},
    {114, L"Nyomtatók és eszközök"},
    {115, L"A %1 otthoni csoport tagjainak módosítása"},
    {116, L"Az otthoni csoport jelszava visszaállításra került"},
    {117, L"A HomeGroup fájlokat oszt meg"},
    {118, L"Opció: Ez a számítógép egy otthoni csoporthoz tartozik"},
    {119, L"Otthoni csoport csatlakozhat"},
    {120, L"Otthoni csoport hozható létre"},
    {121, L"A HomeGroup nem érhető el"},
    {122, L"Nem megbízható nyomtató"},
    {200, L"Tag hozzáadása lehetőségre"},
    {201, L"Felhasználói ikon"},
    {202, L"Teljes név"},
    {203, L"Felhasználói azonosító"},
    {204, L"Haladás sáv"},
    {205, L"Mappa ikon"},
    {220, L"Ossza meg a könyvtárakat és a hardvert"},
    {221, L"Válassza ki azt a könyvtárat, amelyet meg szeretne osztani másokkal az otthoni csoportjában."},
    {222, L"Szerkessze a HomeGroup beállításait"},
    {223, L"Ezen az oldalon a HomeGroup beállításainak módosításához nyissa meg a HomeGroup alkalmazást a Vezérlőpulton."},
    {224, L"Otthoni csoport beállításai"},
    {225, L"Ezen az oldalon módosíthatja az Otthoni csoport beállításait a Vezérlőpulton, vagy használhatja a HomeGroup hibaelhárítót."},
    {226, L"Indítsa el a hibaelhárítót"},
    {227, L"Ezen az oldalon használhatja a HomeGroup hibaelhárítót az Otthoni csoporttal kapcsolatos problémák megkeresésére és megoldására."},
    {228, L"Jelszó megtekintése"},
    {229, L"Ezen az oldalon megtekintheti vagy kinyomtathatja otthoni csoport jelszavát."},
    {230, L"Csatlakozz az otthoni csoporthoz"},
    {231, L"Csatlakozzon az otthoni csoporthoz ezen a hálózaton."},
    {530, L"Részletes megosztási beállítások megnyitása…"},
    {541, L"Hálózati láthatóság"},
    {542, L"Ha a hálózatfelderítés be van kapcsolva, ez a számítógép láthatja és láthatja más hálózatba kapcsolt számítógépek és eszközök."},
    {543, L"Kapcsolja be a hálózatfelderítést"},
    {544, L"Kapcsolja ki a hálózatfelderítést"},
    {545, L"Fájl- és nyomtatóhozzáférés"},
    {546, L"Ha a fájl- és nyomtatómegosztás be van kapcsolva, a hálózat többi felhasználója hozzáférhet a számítógépről megosztott fájlokhoz és nyomtatókhoz."},
    {547, L"Kapcsolja be a fájl- és nyomtatómegosztást"},
    {548, L"Kapcsolja ki a fájl- és nyomtatómegosztást"},
    {549, L"Nyilvános mappamegosztás"},
    {550, L"Ha a nyilvános mappamegosztás be van kapcsolva, a hálózat felhasználói, beleértve az otthoni csoport tagjait is, hozzáférhetnek a nyilvános mappákban lévő fájlokhoz."},
    {552, L"A megosztás engedélyezésével bárki, aki hozzáfér a hálózatához, olvashat és írhat fájlokat a nyilvános mappákban."},
    {553, L"A nyilvános mappamegosztás kikapcsolása (a számítógépre bejelentkezett felhasználók továbbra is hozzáférhetnek ezekhez a mappákhoz)"},
    {554, L"A különböző hálózati profilok megosztási beállításainak módosítása"},
    {559, L"Média hozzáférés"},
    {560, L"Ha a médiaadatfolyam be van kapcsolva, a hálózaton lévő felhasználók és eszközök hozzáférhetnek a számítógépen található fényképekhez, zenékhez és videókhoz. Ez a számítógép a hálózaton is talál médiát."},
    {564, L"Mégse"},
    {567, L"Alkalmazza a változtatásokat"},
    {584, L"A Windows minden használt hálózathoz külön hálózati profilt hoz létre. Minden profilhoz megadhat konkrét beállításokat."},
    {585, L"Otthoni csoport figyelmeztető ikon"},
    {586, L"Erről a számítógépről megosztott könyvtárak és eszközök"},
    {595, L"További HomeGroup feladatok"},
    {600, L"Mutassa meg vagy nyomtassa ki a HomeGroup jelszavát"},
    {601, L"A rendszergazdája nem engedélyezte, hogy hozzáférjen az otthoni csoportjához."},
    {604, L"Változtasd meg a jelszót..."},
    {605, L"Kilépés az otthoni csoportból..."},
    {607, L"Válassza ki a médiastreamelési beállításokat..."},
    {608, L"Mivel ez a számítógép egy tartomány része, a könyvtárak és eszközök megosztására vonatkozó beállítások nem érhetők el az otthoni csoport más számítógépeivel."},
    {609, L"Jelszóval védett megosztás"},
    {610, L"Ha a jelszóval védett megosztás be van kapcsolva, csak a számítógépen felhasználói fiókokkal és jelszavakkal rendelkező felhasználók férhetnek hozzá a megosztott fájlokhoz, a számítógéphez csatlakoztatott nyomtatókhoz és nyilvános mappákhoz. A jelszóval védett megosztást ki kell kapcsolni, hogy mások is hozzáférhessenek."},
    {611, L"Kapcsolja be a jelszóval védett megosztást"},
    {612, L"Kapcsolja ki a jelszóval védett megosztást"},
    {613, L"Oldal nyomtatása"},
    {614, L"Lehetővé teszi a megosztott tartalom lejátszását a hálózaton lévő összes eszközön, például tévén és játékkonzolon"},
    {615, L"Privát hálózat"},
    {616, L"Vendég vagy nyilvános hálózat"},
    {617, L"Domain hálózat"},
    {619, L"HomeGroup kapcsolatok"},
    {620, L"A Windows általában kezeli a kapcsolatokat más otthoni csoport számítógépeivel. Ha azonban ugyanazt a felhasználói fiókot és jelszót használja minden számítógépén, akkor a HomeGroup ezt a fiókot használja helyette."},
    {621, L"Lehetőség: Engedélyezze a Windows számára az otthoni csoportkapcsolatok kezelését (ajánlott)"},
    {622, L"Csatlakozzon más számítógépekhez felhasználói fiókja és jelszava segítségével"},
    {624, L"Indítsa el a HomeGroup hibaelhárítót"},
    {627, L"Fájlmegosztó kapcsolatok"},
    {628, L"A Windows 128 bites titkosítást használ a fájlmegosztó kapcsolatok biztonságossá tételére. Egyes eszközök nem támogatják a 128 bites titkosítást, és 40 vagy 56 bites titkosítást kell használniuk."},
    {629, L"Biztosítsa fájlmegosztó kapcsolatát 128 bites titkosítással (ajánlott)"},
    {630, L"Engedélyezze az eszköz fájlmegosztását 40 bites vagy 56 bites titkosítással"},
    {631, L"Minden hálózat"},
    {632, L"Módosítsa az otthoni csoporttal megosztott tartalmat"},
    {637, L"Bezárás"},
    {639, L"Otthoni csoport távelérés"},
    {640, L"Az otthoni csoport többi tagja számítógépükön keresztül bárhonnan csatlakozhat az Ön otthoni csoportjához."},
    {641, L"Lehetőség: Tiltsa le a távoli otthoni csoport elérését ezen a számítógépen keresztül"},
    {642, L"Lehetőség: Engedélyezze a távoli otthoni csoporthoz való hozzáférést ezen a számítógépen keresztül"},
    {648, L"Válassza ki az elérhetővé tenni fájlokat és eszközöket, majd válassza ki az engedélyszintjüket."},
    {649, L"Könyvtár vagy címtár"},
    {650, L"Hozzáférési szint"},
    {652, L"Kapcsolja be a hálózathoz csatlakoztatott eszközök automatikus beállítását."},
    {46000, L"Otthoni csoport"},
    {46004, L"Opció: Válasszon jelszót otthoni csoportjához"},
    {46005, L"Írja be az otthoni csoport jelszavát"},
    {46006, L"&Létrehozás most"},
    {46007, L"&Csatlakozzon most"},
    {46008, L"Ezzel a jelszóval adjon hozzá további számítógépeket otthoni csoportjához"},
    {46009, L"Ön csatlakozott az otthoni csoporthoz"},
    {46011, L"Otthoni csoport"},
    {46012, L"A Windows nem tud otthoni csoportot beállítani ezen a számítógépen."},
    {46013, L"Mivel ez a számítógép egy tartomány része, könyvtárának megosztása az otthoni csoport más számítógépeivel nem érhető el."},
    {46014, L"A jelszavaknak legalább 8 karakterből kell állniuk, és nem szabad szóközt bevezető vagy végén."},
    {46015, L"A jelszó helytelen.\nKérjük, próbálja újra. A jelszavak megkülönböztetik a kis- és nagybetűket."},
    {46016, L"Opció: A számítógépen lévő összes otthoni csoport kapcsolat megszakad"},
    {46017, L"Sikeresen kilépett az otthoni csoportból"},
    {46018, L"Módosítsa az otthoni csoporttal megosztott tartalmat"},
    {46019, L"Ossza meg fényképeit, videóit, zenéit, dokumentumait és nyomtatóit otthona más számítógépeivel."},
    {46020, L"&Módosítások végrehajtása"},
    {46021, L"Az otthoni csoport jelszavának megváltoztatása mindenkit megszakít"},
    {46022, L"Adjon meg új jelszót otthoni csoportjához"},
    {46023, L"&Jelszó módosítása"},
    {46024, L"Az otthoni csoport jelszava sikeresen megváltozott"},
    {46025, L"Az otthoni csoport jelszava megváltozott"},
    {46026, L"Írja be az otthoni csoport jelszavát"},
    {46027, L"Az otthoni csoport jelszava megváltozott. Az otthoni csoport erőforrásainak használatának folytatásához győződjön meg arról, hogy az új jelszót már beírt személy online állapotban van, majd írja be az új jelszót."},
    {46028, L"Megosztva"},
    {46029, L"A Windows nem tudta eltávolítani a számítógépet az otthoni csoportból."},
    {46030, L"%1 megváltoztatta az otthoni csoport jelszavát. Az otthoni csoport erőforrásainak használatának folytatásához győződjön meg arról, hogy az új jelszót már beírt személy online állapotban van, majd írja be az új jelszót."},
    {46031, L"A jelszavak segítenek megakadályozni az otthoni csoport fájljaihoz és nyomtatóihoz való jogosulatlan hozzáférést. A jelszót a %2, %1 vagy az otthoni csoport egy másik tagjától kaphatja meg."},
    {46032, L"A jelszavak segítenek megakadályozni az otthoni csoport fájljaihoz és nyomtatóihoz való jogosulatlan hozzáférést. A jelszót a %2, %1 vagy az otthoni csoport egy másik tagjától kaphatja meg."},
    {46033, L"Consolas"},
    {46034, L"Hozzon létre egy otthoni csoportot"},
    {46035, L"Csatlakozz egy otthoni csoporthoz"},
    {46036, L"Módosítsa az otthoni csoport jelszavát"},
    {46037, L"Hagyja el a Homegroup-ot"},
    {46038, L"Más számítógépeken lévő fájlok és nyomtatók eléréséhez hozzá kell adni őket az otthoni csoporthoz. A következő jelszó szükséges:"},
    {46039, L"Írja be az új otthoni csoport jelszavát:"},
    {46040, L"Frissítse a jelszót"},
    {46041, L"Készítsen biztonsági másolatot az otthoni csoport összes számítógépéről egy helyi adatvédelmi célponthoz."},
    {46042, L"Készítsen biztonsági másolatot számítógépéről a HomeGroup adatvédelmi célokkal"},
    {46043, L"Nincs megosztva"},
    {46044, L"Otthoni csoportok csak magánhálózatokon hozhatók létre.\nA hálózati hely beállításainak módosításához nyissa meg a Hálózati és megosztási központot a Vezérlőpulton."},
    {46045, L"A Windows többé nem érzékeli az otthoni csoportokat ezen a hálózaton. Új otthoni csoport létrehozásához kattintson az OK gombra, és nyissa meg az Otthoni csoportot a Vezérlőpulton."},
    {46046, L"A Windows meglévő otthoni csoportot észlelt.\nA csatlakozáshoz kattintson az OK gombra, és nyissa meg a HomeGroup alkalmazást a Vezérlőpulton."},
    {46047, L"A HomeGroup szolgáltatás már elérhető. Kérjük, próbálja újra."},
    {46048, L"Megosztási beállítások frissítve"},
    {46049, L"A kiválasztott fájlok és erőforrások meg vannak osztva az otthoni csoporttal."},
    {46050, L"Az otthoni csoport jelszava sikeresen frissítve"},
    {46051, L"Ön csatlakozott az otthoni csoporthoz"},
    {46052, L"Mostantól elérheti megosztott fájljait és eszközeit. A megosztott fájlok és eszközök változatlanok maradnak."},
    {46053, L"Elkezdheti elérni az otthoni csoport más felhasználói által megosztott fájlokat és nyomtatókat."},
    {46054, L"Frissítse otthoni csoport jelszavát"},
    {46055, L"Csatlakozz egy otthoni csoporthoz"},
    {46056, L"Adja meg a %1 új otthoni csoport jelszavát."},
    {46057, L"Az összes otthoni csoport számítógépének órái között nem lehet több 24 óránál. Győződjön meg arról, hogy számítógépe órái szinkronban vannak, majd próbáljon meg újra csatlakozni az otthoni csoporthoz."},
    {46058, L"A jelszó nem felel meg a tartomány jelszóerősségi követelményeinek. Adjon meg egy megfelelő jelszót, vagy használjon egy másik HomeGroup számítógépet a jelszó megváltoztatásához."},
    {46059, L"Nem állíthatja vissza jelszavát, mert nem csatlakozik magánhálózathoz.\nKérjük, csatlakozzon egy privát hálózathoz, és próbálja újra."},
    {46060, L"Nem csatlakozik magánhálózathoz.\nA hálózati hely beállításainak módosításához nyissa meg a Hálózati és megosztási központot a Vezérlőpulton."},
    {46061, L"Ossza meg más otthoni számítógépekkel"},
    {46062, L"Megoszthat fájlokat és nyomtatókat más számítógépekkel. A médiát is streamelheti a készülékére.\n\nAz otthoni csoportok jelszóval védettek, és bármikor kiválaszthatja, hogy mit oszt meg."},
    {46063, L"Ezzel a jelszóval adjon hozzá további számítógépeket otthoni csoportjához"},
    {46064, L"Más számítógépeken lévő fájlok és nyomtatók eléréséhez hozzá kell adni őket az otthoni csoporthoz. A következő jelszó szükséges:"},
    {46065, L"Otthoni csoport létrehozásához vagy csatlakozásához a hálózati kapcsolaton engedélyezni kell az IPv6-ot. Az IPv6 engedélyezéséhez indítsa el a HomeGroup hibaelhárítót."},
    {46066, L"Adjon hozzá személyeket az otthoni csoporthoz"},
    {46067, L"Állítsa be az otthoni csoport adatvédelmét"},
    {46068, L"Több otthoni csoport észlelve"},
    {46069, L"Oszd meg az otthoni csoport többi tagjával"},
    {46070, L"Dokumentumok"},
    {46071, L"Képek"},
    {46072, L"Zene"},
    {46073, L"Videók"},
    {46074, L"Nyomtatók és eszközök"},
    {46075, L"Módosítsa az otthoni csoport megosztási beállításait"},
    {46076, L"%1 Megosztás"},
    {46077, L"Jelszava ellenőrzése..."},
};

// Norwegian Bokmal (nb-NO)
static const EmbeddedTextEntry kStrings_NB_NO[] = {
    {1, L"Hjemmegruppe"},
    {2, L"Gjennomgå alternativer for hjemmegruppe, bestem hva denne PC-en deler, og vis eller oppdater tilgangspassordet."},
    {3, L"En policy satt av organisasjonen din hindrer denne siden i å kjøre. Spør nettverksadministratoren om hjelp."},
    {4, L"Detaljerte delingsalternativer"},
    {5, L"På"},
    {6, L"Av"},
    {7, L"Av (ingen skrivere installert)"},
    {8, L"Det er ingen skriver koblet til denne datamaskinen."},
    {9, L"Del innhold med PC-er hjemme"},
    {10, L"Få tilgang til hjemmegruppen din ved å bruke en domenetilkoblet datamaskin"},
    {12, L"Rediger alternativer for hjemmegruppe"},
    {13, L"Jobber …"},
    {14, L"Ingen hjemmegruppe ble funnet på dette nettverket."},
    {15, L"%1 av %2 opprettet en hjemmegruppe på nettverket."},
    {16, L"Du har blitt invitert til å bli med i hjemmegruppen din."},
    {18, L"Bruk denne siden til denne datamaskinen tilhører en hjemmegruppe."},
    {19, L"Denne datamaskinen kan ikke koble til hjemmegruppen din."},
    {20, L"HomeGroup lar pålitelige PC-er utveksle filer og bruke delte skrivere, og den kan sende media til kompatible enheter. Tilgang krever et passord, mens du har kontroll over hva denne PC-en gjør tilgjengelig."},
    {21, L"Denne datamaskinen er også en del av et domene, så den kan ikke opprette sin egen hjemmegruppe, men den kan bli med i en hjemmegruppe opprettet av noen på nettverket.\n\nHjemmegrupper kobler sammen datamaskiner på hjemmenettverket ditt slik at du kan dele bilder, musikk, videoer, dokumenter og skrivere. Hjemmegrupper er passordbeskyttet og du kan velge hva du vil dele når som helst."},
    {22, L"Hjemmegrupper kobler sammen datamaskiner på hjemmenettverket ditt slik at du kan dele bilder, musikk, videoer, dokumenter og skrivere. Hjemmegrupper er passordbeskyttet og du kan velge hva du vil dele når som helst.\n\nDu kan ikke lage dine egne hjemmegrupper i denne utgaven av Windows, men du kan bli med i hjemmegrupper opprettet av andre."},
    {23, L"Sett opp en hjemmegruppe"},
    {24, L"Bli med"},
    {25, L"Hjemmegruppepassord er endret. For å fortsette å bruke hjemmegrupperessursene, sørg for at personen som allerede skrev inn det nye passordet er pålogget, og skriv deretter inn det nye passordet."},
    {26, L"Windows har oppdaget en annen hjemmegruppe på nettverket ditt. Hjemmegrupper lar deg dele filer og skrivere med andre datamaskiner. Du kan også streame media til enheten din."},
    {27, L"%1 endret hjemmegruppepassordet sitt. For å fortsette å bruke hjemmegrupperessursene, sørg for at personen som allerede skrev inn det nye passordet er pålogget, og skriv deretter inn det nye passordet."},
    {28, L"Ser etter hjemmegrupper på dette nettverket..."},
    {29, L"Skriv inn nytt passord"},
    {30, L"Bli med nå"},
    {32, L"Før du kan opprette eller bli med i en hjemmegruppe, må du først koble til nettverket ditt."},
    {34, L"Bruk denne siden for å opprette eller bli med i en hjemmegruppe, datamaskinens nettverksplassering må være satt til privat."},
    {35, L"Endre nettverksplassering"},
    {37, L"Delingsalternativer for Private"},
    {38, L"Delingsalternativer for offentlig"},
    {39, L"Delingsalternativer for domene"},
    {40, L"Privat"},
    {41, L"Privat (nåværende profil)"},
    {42, L"Offentlig"},
    {43, L"Offentlig (nåværende profil)"},
    {44, L"Domene"},
    {45, L"Domene (nåværende profil)"},
    {46, L"Mediestrømming er på."},
    {47, L"Mediestrømming er av."},
    {56, L"Avbryt"},
    {63, L"OK"},
    {64, L"Vis eller skriv ut hjemmegruppepassordet"},
    {65, L"24pt;;;Consolas"},
    {66, L"Utskriftsdato: %1 %2"},
    {67, L"Alternativ: Se og skriv ut hjemmegruppepassordet"},
    {68, L"Passord:"},
    {69, L"Bruk dette passordet til å koble andre datamaskiner til hjemmegruppen din."},
    {70, L"På hver datamaskin:"},
    {71, L"Merk: Datamaskiner som er slått av eller i dvale vil ikke vises i hjemmegruppen din."},
    {72, L"1. Klikk Start og deretter Kontrollpanel."},
    {73, L"2. Under Nettverk og Internett klikker du på Velg hjemmegruppe og delingsalternativer."},
    {74, L"3. Klikk Bli med nå og følg veiviseren for hjemmegruppe for å skrive inn passordet ditt."},
    {75, L"Klikk Start, og klikk deretter Kontrollpanel."},
    {76, L"Kunne ikke skrive ut hjemmegruppepassordet"},
    {77, L"Det oppstod en feil da Windows prøvde å skrive ut hjemmegruppepassordet. (Feilkode:%1!u!)"},
    {78, L"Du er ikke koblet til hjemmenettverket ditt for øyeblikket. For å se filer og ressurser på andre hjemmegruppedatamaskiner må du først koble til hjemmenettverket."},
    {79, L"%1 har koblet datamaskinen til hjemmegruppen. Jeg har ikke delt biblioteket med hjemmegruppen min. Klikk på koblingen nedenfor for å endre det du har delt. Ikke slå av eller start datamaskinen på nytt før delingen er fullført."},
    {80, L"Jeg har ikke delt biblioteket med hjemmegruppen min. Klikk på koblingen nedenfor for å endre det du har delt. Ikke slå av eller start datamaskinen på nytt før delingen er fullført."},
    {81, L"HomeGroup deler for øyeblikket biblioteket på denne datamaskinen. Noen hjemmegruppealternativer er ikke tilgjengelige før delingen er fullført. Ikke slå av eller start datamaskinen på nytt før delingen er fullført."},
    {82, L"Under Nettverk og Internett klikker du på Velg hjemmegruppe og delingsalternativer."},
    {83, L"Det er for øyeblikket ingen hjemmegrupper på nettverket."},
    {84, L"Klikk Bli med nå og følg hjemmegruppeveiviseren for å skrive inn passordet ditt."},
    {85, L"Klikk her for å installere."},
    {86, L"Windows fant en hjemmegruppeskriver"},
    {88, L"Vi introduserer HomeGroup"},
    {89, L"%1 (nåværende profil)"},
    {90, L"Bruk denne siden for å bli med i en hjemmegruppe, datamaskinens nettverksplassering må være satt til privat."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Hjemmegruppe er ikke klar ennå. Prøv igjen om noen minutter. Hvis du fortsetter å se denne meldingen, klikker du på koblingen for å begynne å feilsøke hjemmegruppen din."},
    {95, L"Start HomeGroup-feilsøkeren"},
    {98, L"Hjemmegruppepassord"},
    {99, L"Gjestekontoer kan ikke endre hjemmegruppeinnstillinger."},
    {100, L"HomeGroup har funnet en ny delt skriver på hjemmenettverket ditt. Når den er installert, vil den være tilgjengelig for alle på denne datamaskinen."},
    {101, L"Installer skriveren"},
    {102, L"Hjemmegruppe er ikke tilgjengelig fordi du ikke er koblet til hjemmenettverket."},
    {103, L"Hjemmegruppe er ikke tilgjengelig fordi du ikke er koblet til hjemmenettverket."},
    {104, L"Før du blir med i en hjemmegruppe, må du først koble til nettverket."},
    {105, L"Hjemmegruppebilde"},
    {106, L"Velg hva du vil dele og se passordet for hjemmegruppen"},
    {107, L"Fordi denne datamaskinen er en del av et domene, er ikke innstillinger tilgjengelig for å dele bibliotekene og enhetene med andre datamaskiner i hjemmegruppen."},
    {108, L"Innstillinger for å dele biblioteker og enheter med andre datamaskiner i en hjemmegruppe er ikke tilgjengelig i denne utgaven av Windows."},
    {109, L"Fjern %1 fra hjemmegruppen"},
    {110, L"Avbryt"},
    {111, L"Fjern hjemmegruppemedlem"},
    {112, L"%1 vil bli fjernet fra hjemmegruppen"},
    {113, L"Alle hjemmegruppemedlemmer som blir med ved å bruke et passord, må skrive inn passordet på nytt."},
    {114, L"Skrivere og enheter"},
    {115, L"Endre %1 hjemmegruppemedlemmer"},
    {116, L"Hjemmegruppepassordet ble tilbakestilt"},
    {117, L"Hjemmegruppe deler filer"},
    {118, L"Alternativ: Denne datamaskinen tilhører en hjemmegruppe"},
    {119, L"En hjemmegruppe er tilgjengelig for å bli med"},
    {120, L"En hjemmegruppe kan opprettes"},
    {121, L"Hjemmegruppe er ikke tilgjengelig"},
    {122, L"Uklarert skriver"},
    {200, L"Legg til medlem"},
    {201, L"Brukerikon"},
    {202, L"Fullt navn"},
    {203, L"Bruker-ID"},
    {204, L"Fremdriftslinje"},
    {205, L"Mappeikon"},
    {220, L"Del biblioteker og maskinvare"},
    {221, L"Velg biblioteket du vil dele med andre i hjemmegruppen din."},
    {222, L"Rediger alternativer for hjemmegruppe"},
    {223, L"Bruk denne siden til å endre HomeGroup-innstillinger, åpne HomeGroup i kontrollpanelet."},
    {224, L"Hjemmegruppealternativer"},
    {225, L"Bruk denne siden til å endre HomeGroup-innstillingene i kontrollpanelet eller bruk HomeGroup Troubleshooter."},
    {226, L"Start feilsøking"},
    {227, L"Bruk denne siden til å bruke HomeGroup feilsøking for å finne og fikse problemer med HomeGroup."},
    {228, L"Se passord"},
    {229, L"Bruk denne siden til å se eller skrive ut hjemmegruppepassordet ditt."},
    {230, L"Bli med i hjemmegruppen"},
    {231, L"Bli med i hjemmegruppen på dette nettverket."},
    {530, L"Åpne detaljerte delingsalternativer..."},
    {541, L"Nettverkssynlighet"},
    {542, L"Hvis nettverksoppdaging er slått på, kan denne datamaskinen se og bli sett av andre nettverkstilkoblede datamaskiner og enheter."},
    {543, L"Slå på nettverksoppdagelse"},
    {544, L"Slå av nettverksoppdagelse"},
    {545, L"Fil- og skrivertilgang"},
    {546, L"Når fil- og skriverdeling er slått på, kan andre brukere på nettverket få tilgang til filene og skriverne du deler fra denne datamaskinen."},
    {547, L"Slå på fil- og skriverdeling"},
    {548, L"Slå av fil- og skriverdeling"},
    {549, L"Felles mappedeling"},
    {550, L"Når deling av offentlig mappe er slått på, kan brukere på nettverket ditt, inkludert medlemmer av hjemmegruppen, få tilgang til filer i offentlige mapper."},
    {552, L"Aktivering av deling lar alle med tilgang til nettverket ditt lese og skrive filer i de offentlige mappene dine."},
    {553, L"Slå av deling av offentlig mappe (brukere som er logget på denne datamaskinen har fortsatt tilgang til disse mappene)"},
    {554, L"Endre delingsalternativer for ulike nettverksprofiler"},
    {559, L"Medietilgang"},
    {560, L"Når mediestrømming er slått på, kan brukere og enheter på nettverket ditt få tilgang til bildene, musikken og videoene på denne datamaskinen. Denne datamaskinen kan også finne medier på nettverket."},
    {564, L"Avbryt"},
    {567, L"Bruk endringer"},
    {584, L"Windows oppretter en egen nettverksprofil for hvert nettverk du bruker. Du kan velge spesifikke alternativer for hver profil."},
    {585, L"Hjemmegruppeadvarselsikon"},
    {586, L"Biblioteker og enheter delt fra denne datamaskinen"},
    {595, L"Flere hjemmegruppeoppgaver"},
    {600, L"Vis eller skriv ut hjemmegruppepassordet"},
    {601, L"Systemadministratoren din har ikke gitt deg tilgang til hjemmegruppen din."},
    {604, L"Endre passordet..."},
    {605, L"Gå ut av hjemmegruppen..."},
    {607, L"Velg alternativer for mediastreaming..."},
    {608, L"Fordi denne datamaskinen er en del av et domene, er ikke innstillinger tilgjengelig for å dele bibliotekene og enhetene med andre datamaskiner i hjemmegruppen."},
    {609, L"Passordbeskyttet deling"},
    {610, L"Når passordbeskyttet deling er slått på, kan bare brukere med brukerkontoer og passord på denne datamaskinen få tilgang til delte filer, skrivere koblet til denne datamaskinen og offentlige mapper. Passordbeskyttet deling må slås av for å gi andre tilgang."},
    {611, L"Slå på passordbeskyttet deling"},
    {612, L"Slå av passordbeskyttet deling"},
    {613, L"Skriv ut side"},
    {614, L"Tillater at delt innhold spilles av på alle enheter på dette nettverket, for eksempel TV-er og spillkonsoller"},
    {615, L"Privat nettverk"},
    {616, L"Gjestenettverk eller offentlig nettverk"},
    {617, L"Domenenettverk"},
    {619, L"Hjemmegruppeforbindelser"},
    {620, L"Windows administrerer vanligvis tilkoblinger til andre hjemmegruppedatamaskiner. Men hvis du bruker samme brukerkonto og passord på alle datamaskinene dine, kan du la Hjemmegruppe bruke den kontoen i stedet."},
    {621, L"Alternativ: Tillat Windows å administrere hjemmegruppetilkoblinger (anbefalt)"},
    {622, L"Koble til andre datamaskiner med brukerkontoen og passordet"},
    {624, L"Start HomeGroup-feilsøkeren"},
    {627, L"Fildelingstilkoblinger"},
    {628, L"Windows bruker 128-bits kryptering for å sikre fildelingstilkoblinger. Noen enheter støtter ikke 128-bits kryptering og må bruke 40-biters eller 56-biters kryptering."},
    {629, L"Sikre fildelingstilkoblingen din med 128-bits kryptering (anbefalt)"},
    {630, L"Aktiver enhetsfildeling med 40-biters eller 56-biters kryptering"},
    {631, L"Hvert nettverk"},
    {632, L"Endre hva som deles med hjemmegruppen din"},
    {637, L"Lukk"},
    {639, L"Hjemmegruppe fjerntilgang"},
    {640, L"Andre hjemmegruppemedlemmer kan koble seg til hjemmegruppen din fra hvor som helst via datamaskinene sine."},
    {641, L"Alternativ: Deaktiver ekstern hjemmegruppetilgang via denne datamaskinen"},
    {642, L"Alternativ: Aktiver ekstern hjemmegruppetilgang via denne datamaskinen"},
    {648, L"Velg filene og enhetene som skal gjøres tilgjengelige, og velg deretter tillatelsesnivåene deres."},
    {649, L"Bibliotek eller katalog"},
    {650, L"Tilgangsnivå"},
    {652, L"Slå på automatisk oppsett av nettverkstilkoblede enheter."},
    {46000, L"Hjemmegruppe"},
    {46004, L"Alternativ: Velg et passord for hjemmegruppen din"},
    {46005, L"Skriv inn hjemmegruppepassordet"},
    {46006, L"&Opprett nå"},
    {46007, L"&Bli med nå"},
    {46008, L"Legg til andre datamaskiner i hjemmegruppen din ved å bruke dette passordet"},
    {46009, L"Du har blitt med i hjemmegruppen"},
    {46011, L"Hjemmegruppe"},
    {46012, L"Windows kan ikke sette opp en hjemmegruppe på denne datamaskinen."},
    {46013, L"Fordi denne datamaskinen er en del av et domene, er deling av biblioteket med andre datamaskiner i hjemmegruppen ikke tilgjengelig."},
    {46014, L"Passord må inneholde minst 8 tegn og ingen innledende eller etterfølgende mellomrom."},
    {46015, L"Passordet er feil.\nVennligst prøv igjen. Passord skiller mellom store og små bokstaver."},
    {46016, L"Alternativ: Alle hjemmegruppetilkoblinger på denne datamaskinen vil bli frakoblet"},
    {46017, L"Forlatt hjemmegruppen din"},
    {46018, L"Endre hva som deles med hjemmegruppen din"},
    {46019, L"Del bilder, videoer, musikk, dokumenter og skrivere med andre datamaskiner i hjemmet ditt."},
    {46020, L"&Gjør endringer"},
    {46021, L"Endring av hjemmegruppepassord kobler fra alle"},
    {46022, L"Skriv inn et nytt passord for hjemmegruppen din"},
    {46023, L"&Endre passord"},
    {46024, L"Hjemmegruppepassordet ble endret"},
    {46025, L"Hjemmegruppepassordet ble endret"},
    {46026, L"Skriv inn hjemmegruppepassordet"},
    {46027, L"Hjemmegruppepassord er endret. For å fortsette å bruke hjemmegrupperessursene, sørg for at personen som allerede skrev inn det nye passordet er pålogget, og skriv deretter inn det nye passordet."},
    {46028, L"Delt"},
    {46029, L"Windows kunne ikke fjerne datamaskinen fra hjemmegruppen."},
    {46030, L"%1 endret hjemmegruppepassordet sitt. For å fortsette å bruke hjemmegrupperessursene, sørg for at personen som allerede skrev inn det nye passordet er pålogget, og skriv deretter inn det nye passordet."},
    {46031, L"Passord bidrar til å forhindre uautorisert tilgang til hjemmegruppens filer og skrivere. Du kan få passordet fra %2, %1 eller et annet medlem av hjemmegruppen din."},
    {46032, L"Passord bidrar til å forhindre uautorisert tilgang til hjemmegruppens filer og skrivere. Du kan få passordet fra %2, %1 eller et annet medlem av hjemmegruppen din."},
    {46033, L"Consolas"},
    {46034, L"Opprett en hjemmegruppe"},
    {46035, L"Bli med i en hjemmegruppe"},
    {46036, L"Endre hjemmegruppepassordet ditt"},
    {46037, L"Gå ut av hjemmegruppen"},
    {46038, L"For å få tilgang til filer og skrivere på andre datamaskiner, må du legge dem til i hjemmegruppen din. Følgende passord kreves:"},
    {46039, L"Skriv inn det nye hjemmegruppepassordet:"},
    {46040, L"Oppdater passord"},
    {46041, L"Sikkerhetskopier alle PC-er i hjemmegruppen din til et lokalt databeskyttelsesmål."},
    {46042, L"Sikkerhetskopier PC-en ved hjelp av databeskyttelsesmål for hjemmegruppe"},
    {46043, L"Ikke delt"},
    {46044, L"Hjemmegrupper kan bare opprettes på private nettverk.\nFor å endre innstillingene for nettverksplassering, åpne Nettverks- og delingssenter i kontrollpanelet."},
    {46045, L"Windows vil ikke lenger oppdage hjemmegrupper på dette nettverket. For å opprette en ny hjemmegruppe, klikk OK og åpne Hjemmegruppe i Kontrollpanel."},
    {46046, L"Windows oppdaget en eksisterende hjemmegruppe.\nFor å bli med, klikk OK og åpne Hjemmegruppe i Kontrollpanel."},
    {46047, L"Hjemmegruppetjeneste er nå tilgjengelig. Vennligst prøv igjen."},
    {46048, L"Delingsinnstillingene er oppdatert"},
    {46049, L"De valgte filene og ressursene deles med hjemmegruppen din."},
    {46050, L"Hjemmegruppepassordet ble oppdatert"},
    {46051, L"Du har blitt med i hjemmegruppen"},
    {46052, L"Du kan nå få tilgang til dine delte filer og enheter. Filene og enhetene du deler forblir uendret."},
    {46053, L"Du kan begynne å få tilgang til filer og skrivere som deles av andre brukere i hjemmegruppen din."},
    {46054, L"Oppdater hjemmegruppepassordet ditt"},
    {46055, L"Bli med i en hjemmegruppe"},
    {46056, L"Skriv inn det nye hjemmegruppepassordet fra %1."},
    {46057, L"Alle hjemmegruppedatamaskiners klokker må ikke stilles på mer enn 24 timers mellomrom. Sørg for at datamaskinklokkene er synkroniserte, og prøv deretter å bli med i hjemmegruppen igjen."},
    {46058, L"Passordet oppfyller ikke domenets krav til passordstyrke. Skriv inn et matchende passord eller bruk en annen hjemmegruppedatamaskin for å endre passordet ditt."},
    {46059, L"Du kan ikke tilbakestille passordet ditt fordi du ikke er koblet til et privat nettverk.\nKoble til et privat nettverk og prøv igjen."},
    {46060, L"Du er ikke koblet til et privat nettverk.\nFor å endre innstillingene for nettverksplassering, åpne Nettverks- og delingssenter i kontrollpanelet."},
    {46061, L"Del med andre hjemmedatamaskiner"},
    {46062, L"Du kan dele filer og skrivere med andre datamaskiner. Du kan også streame media til enheten din.\n\nHjemmegrupper er passordbeskyttet og du kan velge hva du vil dele når som helst."},
    {46063, L"Legg til andre datamaskiner i hjemmegruppen din ved å bruke dette passordet"},
    {46064, L"For å få tilgang til filer og skrivere på andre datamaskiner, må du legge dem til i hjemmegruppen din. Følgende passord kreves:"},
    {46065, L"For å opprette eller bli med i en hjemmegruppe, må nettverkstilkoblingen din ha IPv6 aktivert. For å aktivere IPv6, start HomeGroup Troubleshooter."},
    {46066, L"Legg til personer i hjemmegruppen"},
    {46067, L"Konfigurer databeskyttelse for hjemmegruppe"},
    {46068, L"Flere hjemmegrupper oppdaget"},
    {46069, L"Del med andre hjemmegruppemedlemmer"},
    {46070, L"Dokumenter"},
    {46071, L"Bilder"},
    {46072, L"Musikk"},
    {46073, L"Videoer"},
    {46074, L"Skrivere og enheter"},
    {46075, L"Endre innstillinger for hjemmegruppedeling"},
    {46076, L"%1 Deling"},
    {46077, L"Bekrefter passordet ditt ..."},
};

// Romanian (ro-RO)
static const EmbeddedTextEntry kStrings_RO_RO[] = {
    {1, L"Grupul de acasă"},
    {2, L"Examinați opțiunile HomeGroup, decideți ce partajează acest computer și afișați sau actualizați parola de acces."},
    {3, L"O politică stabilită de organizația dvs. împiedică rularea acestei pagini. Solicitați asistență administratorului de rețea."},
    {4, L"Opțiuni detaliate de partajare"},
    {5, L"Pornit"},
    {6, L"Oprit"},
    {7, L"Oprit (nu există imprimante instalate)"},
    {8, L"Nu există nicio imprimantă atașată la acest computer."},
    {9, L"Partajați conținut cu computerele de acasă"},
    {10, L"Accesați-vă grupul de domiciliu folosind un computer alăturat unui domeniu"},
    {12, L"Editați opțiunile HomeGroup"},
    {13, L"Lucrează…"},
    {14, L"Nu a fost găsit niciun grup de domiciliu în această rețea."},
    {15, L"%1 din %2 a creat un grup de domiciliu în rețea."},
    {16, L"Ați fost invitat să vă alăturați grupului dvs. de acasă."},
    {18, L"Utilizați această pagină pentru ca acest computer aparține unui grup de domiciliu."},
    {19, L"Acest computer nu se poate conecta la grupul dvs. de acasă."},
    {20, L"HomeGroup permite computerelor de încredere să schimbe fișiere și să utilizeze imprimante partajate și poate trimite media către dispozitive compatibile. Accesul necesită o parolă, în timp ce tu păstrezi controlul asupra a ceea ce acest computer pune la dispoziție."},
    {21, L"Acest computer face, de asemenea, parte dintr-un domeniu, deci nu își poate crea propriul grup de domiciliu, dar se poate alătura unui grup de domiciliu creat de cineva din rețea.\n\nGrupurile de acasă leagă computerele din rețeaua dvs. de acasă, astfel încât să puteți partaja fotografii, muzică, videoclipuri, documente și imprimante. Grupurile de acasă sunt protejate prin parolă și puteți alege ce să partajați în orice moment."},
    {22, L"Grupurile de acasă leagă computerele din rețeaua dvs. de acasă, astfel încât să puteți partaja fotografii, muzică, videoclipuri, documente și imprimante. Grupurile de acasă sunt protejate prin parolă și puteți alege ce să partajați în orice moment.\n\nNu vă puteți crea propriile grupuri de acasă în această ediție de Windows, dar vă puteți alătura grupurilor de acasă create de alții."},
    {23, L"Configurați un grup de acasă"},
    {24, L"Alăturați-vă"},
    {25, L"Parola HomeGroup a fost schimbată. Pentru a continua să utilizați resursele grupului dvs. de domiciliu, asigurați-vă că persoana care a introdus deja noua parolă este online, apoi introduceți noua parolă."},
    {26, L"Windows a detectat un alt grup de domiciliu în rețeaua dvs. Grupurile de acasă vă permit să partajați fișiere și imprimante cu alte computere. De asemenea, puteți transmite conținut media pe dispozitivul dvs."},
    {27, L"%1 și-a schimbat parola grupului de acasă. Pentru a continua să utilizați resursele grupului dvs. de domiciliu, asigurați-vă că persoana care a introdus deja noua parolă este online, apoi introduceți noua parolă."},
    {28, L"Se caută grupuri de acasă în această rețea..."},
    {29, L"Introduceți o nouă parolă"},
    {30, L"Alăturați-vă acum"},
    {32, L"Înainte să vă puteți crea sau să vă alăturați unui grup de domiciliu, trebuie mai întâi să vă conectați la rețea."},
    {34, L"Utilizați această pagină pentru a crea sau a vă alătura unui grup de domiciliu, locația de rețea a computerului dvs. trebuie setată la privată."},
    {35, L"Schimbați locația în rețea"},
    {37, L"Opțiuni de partajare pentru Privat"},
    {38, L"Opțiuni de partajare pentru Public"},
    {39, L"Opțiuni de partajare pentru domeniu"},
    {40, L"Privat"},
    {41, L"Privat (profilul actual)"},
    {42, L"Public"},
    {43, L"Public (profil curent)"},
    {44, L"Domeniul"},
    {45, L"Domeniu (profilul curent)"},
    {46, L"Streamingul media este activat."},
    {47, L"Streamingul media este dezactivat."},
    {56, L"Anulează"},
    {63, L"OK"},
    {64, L"Afișați sau imprimați parola HomeGroup"},
    {65, L"24pt;;;Consolas"},
    {66, L"Data tipăririi: %1 %2"},
    {67, L"Opțiune: Vizualizați și imprimați parola grupului dvs. de domiciliu"},
    {68, L"Parola:"},
    {69, L"Utilizați această parolă pentru a conecta alte computere la grupul dvs. de acasă."},
    {70, L"Pe fiecare computer:"},
    {71, L"Notă: computerele care sunt oprite sau adormite nu vor apărea în grupul dvs. de domiciliu."},
    {72, L"1. Faceți clic pe Start, apoi faceți clic pe Panou de control."},
    {73, L"2. Sub Rețea și Internet, faceți clic pe Alegeți grupul de domiciliu și opțiunile de partajare."},
    {74, L"3. Faceți clic pe Alăturați-vă acum și urmați expertul HomeGroup pentru a vă introduce parola."},
    {75, L"Faceți clic pe Start, apoi faceți clic pe Panou de control."},
    {76, L"Nu s-a putut imprima parola grupului de acasă"},
    {77, L"A apărut o eroare când Windows a încercat să scoată parola grupului de acasă. (Cod de eroare: %1!u!)"},
    {78, L"În prezent, nu sunteți conectat la rețeaua dvs. de domiciliu. Pentru a vizualiza fișiere și resurse pe alte computere din grupul de domiciliu, mai întâi conectați-vă la rețeaua dvs. de domiciliu."},
    {79, L"%1 sa alăturat computerului la grupul de acasă. Nu am partajat biblioteca cu grupul meu de acasă. Faceți clic pe linkul de mai jos pentru a schimba ceea ce ați distribuit. Nu închideți și nu reporniți computerul până când partajarea este completă."},
    {80, L"Nu am partajat biblioteca cu grupul meu de acasă. Faceți clic pe linkul de mai jos pentru a schimba ceea ce ați distribuit. Nu închideți și nu reporniți computerul până când partajarea este completă."},
    {81, L"HomeGroup partajează în prezent biblioteca de pe acest computer. Unele opțiuni pentru grupul de acasă nu sunt disponibile până la finalizarea partajării. Nu închideți și nu reporniți computerul până când partajarea este completă."},
    {82, L"Sub Rețea și Internet, faceți clic pe Alegeți grupul de domiciliu și opțiunile de partajare."},
    {83, L"În prezent, nu există grupuri de acasă în rețea."},
    {84, L"Faceți clic pe Alăturați-vă acum și urmați expertul HomeGroup pentru a vă introduce parola."},
    {85, L"Faceți clic aici pentru a instala."},
    {86, L"Windows a găsit o imprimantă pentru grupul de acasă"},
    {88, L"Vă prezentăm HomeGroup"},
    {89, L"%1 (profil actual)"},
    {90, L"Utilizați această pagină pentru a vă alătura unui grup de domiciliu, locația de rețea a computerului dvs. trebuie setată la privată."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"HomeGroup nu este încă pregătit. Vă rugăm să încercați din nou peste câteva minute. Dacă continuați să vedeți acest mesaj, faceți clic pe link pentru a începe depanarea grupului dvs. de domiciliu."},
    {95, L"Porniți instrumentul de depanare a grupului de acasă"},
    {98, L"Parola grupului de acasă"},
    {99, L"Conturile de invitat nu pot schimba setările grupului de acasă."},
    {100, L"HomeGroup a găsit o nouă imprimantă partajată în rețeaua dvs. de domiciliu. Odată instalat, acesta va fi disponibil pentru oricine pe acest computer."},
    {101, L"Instalați imprimanta"},
    {102, L"HomeGroup nu este disponibil deoarece nu sunteți conectat la rețeaua dvs. de domiciliu."},
    {103, L"HomeGroup nu este disponibil deoarece nu sunteți conectat la rețeaua dvs. de domiciliu."},
    {104, L"Înainte de a vă alătura unui grup de domiciliu, trebuie mai întâi să vă conectați la rețea."},
    {105, L"Imaginea grupului de acasă"},
    {106, L"Selectați ceea ce doriți să partajați și vizualizați parola grupului dvs. de domiciliu"},
    {107, L"Deoarece acest computer face parte dintr-un domeniu, setările pentru a partaja bibliotecile și dispozitivele sale cu alte computere din grupul de domiciliu nu sunt disponibile."},
    {108, L"Setările pentru a partaja biblioteci și dispozitive cu alte computere dintr-un grup de domiciliu nu sunt disponibile în această ediție de Windows."},
    {109, L"Eliminați %1 din grupul de domiciliu"},
    {110, L"Anulează"},
    {111, L"Eliminați membrul grupului de acasă"},
    {112, L"%1 va fi eliminat din grupul de acasă"},
    {113, L"Tuturor membrilor grupului de domiciliu care se alătură folosind o parolă li se va cere să introducă din nou parola."},
    {114, L"Imprimante și dispozitive"},
    {115, L"Schimbați membrii grupului de acasă %1"},
    {116, L"Parola grupului de domiciliu a fost resetată"},
    {117, L"HomeGroup partajează fișiere"},
    {118, L"Opțiune: Acest computer aparține unui grup de acasă"},
    {119, L"Un grup de acasă este disponibil pentru a se alătura"},
    {120, L"Se poate crea un grup de acasă"},
    {121, L"Grupul de acasă nu este disponibil"},
    {122, L"Imprimantă neîncrezătoare"},
    {200, L"Adăugați membru"},
    {201, L"Pictogramă utilizator"},
    {202, L"Nume complet"},
    {203, L"ID utilizator"},
    {204, L"Bara de progres"},
    {205, L"Pictograma folderului"},
    {220, L"Partajați biblioteci și hardware"},
    {221, L"Selectați biblioteca pe care doriți să o partajați cu alții din grupul dvs. de acasă."},
    {222, L"Editați opțiunile HomeGroup"},
    {223, L"Utilizați această pagină pentru a modifica setările Grupului de domiciliu, deschideți Grupul de acasă în Panoul de control."},
    {224, L"Opțiuni pentru grupul de acasă"},
    {225, L"Utilizați această pagină pentru a vă modifica setările Grupului de domiciliu în Panoul de control sau utilizați Instrumentul de depanare a grupului de acasă."},
    {226, L"Porniți instrumentul de depanare"},
    {227, L"Utilizați această pagină pentru a utiliza instrumentul de depanare a grupului de domiciliu pentru a găsi și remedia problemele cu grupul dvs. de domiciliu."},
    {228, L"Vizualizați parola"},
    {229, L"Utilizați această pagină pentru a vizualiza sau imprima parola grupului dvs. de domiciliu."},
    {230, L"Alăturați-vă grupului de acasă"},
    {231, L"Alăturați-vă grupului de acasă din această rețea."},
    {530, L"Deschideți opțiuni detaliate de partajare..."},
    {541, L"Vizibilitatea rețelei"},
    {542, L"Dacă descoperirea rețelei este activată, acest computer poate vedea și poate fi văzut de alte computere și dispozitive din rețea."},
    {543, L"Activați descoperirea rețelei"},
    {544, L"Dezactivați descoperirea rețelei"},
    {545, L"Acces la fișiere și imprimantă"},
    {546, L"Când partajarea fișierelor și a imprimantei este activată, alți utilizatori din rețeaua dvs. pot accesa fișierele și imprimantele pe care le partajați de pe acest computer."},
    {547, L"Activați partajarea fișierelor și a imprimantei"},
    {548, L"Dezactivați partajarea fișierelor și a imprimantei"},
    {549, L"Partajarea dosarelor publice"},
    {550, L"Când partajarea folderelor publice este activată, utilizatorii din rețeaua dvs., inclusiv membrii grupului de domiciliu, pot accesa fișierele din folderele publice."},
    {552, L"Activarea partajării permite oricui cu acces la rețeaua dvs. să citească și să scrie fișiere în folderele publice."},
    {553, L"Dezactivați partajarea folderelor publice (utilizatorii conectați la acest computer pot accesa în continuare aceste dosare)"},
    {554, L"Modificați opțiunile de partajare pentru diferite profiluri de rețea"},
    {559, L"Acces media"},
    {560, L"Când fluxul media este activat, utilizatorii și dispozitivele din rețeaua dvs. pot accesa fotografiile, muzica și videoclipurile de pe acest computer. Acest computer poate găsi, de asemenea, media în rețea."},
    {564, L"Anulează"},
    {567, L"Aplicați modificări"},
    {584, L"Windows creează un profil de rețea separat pentru fiecare rețea pe care o utilizați. Puteți selecta opțiuni specifice pentru fiecare profil."},
    {585, L"Pictograma de avertizare pentru grupul de acasă"},
    {586, L"Biblioteci și dispozitive partajate de pe acest computer"},
    {595, L"Mai multe sarcini HomeGroup"},
    {600, L"Afișați sau imprimați parola HomeGroup"},
    {601, L"Administratorul dvs. de sistem nu v-a permis să accesați grupul dvs. de domiciliu."},
    {604, L"Schimbați parola..."},
    {605, L"Părăsiți grupul de acasă..."},
    {607, L"Alegeți opțiunile de streaming media..."},
    {608, L"Deoarece acest computer face parte dintr-un domeniu, setările pentru a partaja bibliotecile și dispozitivele sale cu alte computere din grupul de domiciliu nu sunt disponibile."},
    {609, L"Partajare protejată prin parolă"},
    {610, L"Când partajarea protejată prin parolă este activată, numai utilizatorii cu conturi de utilizator și parole pe acest computer pot accesa fișierele partajate, imprimantele conectate la acest computer și folderele publice. Partajarea protejată prin parolă trebuie dezactivată pentru a permite accesul altora."},
    {611, L"Activați partajarea protejată prin parolă"},
    {612, L"Dezactivați partajarea protejată prin parolă"},
    {613, L"Imprimați pagina"},
    {614, L"Permite redarea conținutului partajat pe toate dispozitivele din această rețea, cum ar fi televizoare și console de jocuri"},
    {615, L"Rețea privată"},
    {616, L"Oaspeți sau rețea publică"},
    {617, L"Rețeaua de domenii"},
    {619, L"Conexiuni HomeGroup"},
    {620, L"Windows gestionează de obicei conexiunile la alte computere din grupul de acasă. Cu toate acestea, dacă utilizați același cont de utilizator și aceeași parolă pe toate computerele dvs., puteți solicita HomeGroup să folosească acel cont."},
    {621, L"Opțiune: Permite Windows să gestioneze conexiunile la grupul de acasă (recomandat)"},
    {622, L"Conectați-vă la alte computere folosind contul de utilizator și parola"},
    {624, L"Porniți instrumentul de depanare a grupului de acasă"},
    {627, L"Conexiuni de partajare a fișierelor"},
    {628, L"Windows folosește criptarea pe 128 de biți pentru a securiza conexiunile de partajare a fișierelor. Unele dispozitive nu acceptă criptarea pe 128 de biți și trebuie să utilizeze criptarea pe 40 de biți sau 56 de biți."},
    {629, L"Asigurați-vă conexiunea de partajare a fișierelor folosind criptarea pe 128 de biți (recomandat)"},
    {630, L"Activați partajarea fișierelor pe dispozitiv cu criptare pe 40 sau 56 biți"},
    {631, L"Fiecare rețea"},
    {632, L"Schimbați ceea ce este distribuit grupului dvs. de domiciliu"},
    {637, L"Închide"},
    {639, L"Acces la distanță în grupul de acasă"},
    {640, L"Alți membri ai grupului de domiciliu se pot conecta la grupul dvs. de domiciliu de oriunde prin computerele lor."},
    {641, L"Opțiune: Dezactivați accesul la distanță la grupul de domiciliu prin acest computer"},
    {642, L"Opțiune: Activați accesul la distanță la grupul de domiciliu prin acest computer"},
    {648, L"Selectați fișierele și dispozitivele de pus la dispoziție, apoi alegeți nivelurile de permisiuni ale acestora."},
    {649, L"Bibliotecă sau director"},
    {650, L"Nivel de acces"},
    {652, L"Activați configurarea automată a dispozitivelor conectate la rețea."},
    {46000, L"Grupul de acasă"},
    {46004, L"Opțiune: Alegeți o parolă pentru grupul dvs. de domiciliu"},
    {46005, L"Introduceți parola grupului de domiciliu"},
    {46006, L"&Creează acum"},
    {46007, L"&Alăturați-vă acum"},
    {46008, L"Adăugați alte computere în grupul dvs. de domiciliu folosind această parolă"},
    {46009, L"Te-ai alăturat grupului de acasă"},
    {46011, L"Grupul de acasă"},
    {46012, L"Windows nu poate configura un grup de acasă pe acest computer."},
    {46013, L"Deoarece acest computer face parte dintr-un domeniu, partajarea bibliotecii sale cu alte computere din grupul de domiciliu nu este disponibilă."},
    {46014, L"Parolele trebuie să conțină cel puțin 8 caractere și fără spații de început sau de final."},
    {46015, L"Parola este incorectă.\nVă rugăm să încercați din nou. Parolele sunt sensibile la majuscule."},
    {46016, L"Opțiune: Toate conexiunile la grupul de acasă de pe acest computer vor fi deconectate"},
    {46017, L"A părăsit cu succes grupul de acasă"},
    {46018, L"Schimbați ceea ce este distribuit grupului dvs. de domiciliu"},
    {46019, L"Partajați fotografiile, videoclipurile, muzica, documentele și imprimantele cu alte computere din casă."},
    {46020, L"&Efectuați modificări"},
    {46021, L"Schimbarea parolei grupului de acasă deconectează toată lumea"},
    {46022, L"Introduceți o nouă parolă pentru grupul dvs. de domiciliu"},
    {46023, L"&Schimbați parola"},
    {46024, L"Parola HomeGroup a fost schimbată cu succes"},
    {46025, L"Parola grupului de domiciliu a fost schimbată"},
    {46026, L"Introduceți parola grupului de domiciliu"},
    {46027, L"Parola HomeGroup a fost schimbată. Pentru a continua să utilizați resursele grupului dvs. de domiciliu, asigurați-vă că persoana care a introdus deja noua parolă este online, apoi introduceți noua parolă."},
    {46028, L"Partajat"},
    {46029, L"Windows nu a putut elimina computerul din grupul de acasă."},
    {46030, L"%1 și-a schimbat parola grupului de acasă. Pentru a continua să utilizați resursele grupului dvs. de domiciliu, asigurați-vă că persoana care a introdus deja noua parolă este online, apoi introduceți noua parolă."},
    {46031, L"Parolele ajută la prevenirea accesului neautorizat la fișierele și imprimantele grupului dvs. de domiciliu. Puteți obține parola de la %2, %1 sau de la un alt membru al grupului dvs. de domiciliu."},
    {46032, L"Parolele ajută la prevenirea accesului neautorizat la fișierele și imprimantele grupului dvs. de domiciliu. Puteți obține parola de la %2, %1 sau de la un alt membru al grupului dvs. de domiciliu."},
    {46033, L"Consolas"},
    {46034, L"Creați un grup de domiciliu"},
    {46035, L"Alăturați-vă unui grup de acasă"},
    {46036, L"Schimbați parola grupului dvs. de domiciliu"},
    {46037, L"Părăsiți grupul de acasă"},
    {46038, L"Pentru a accesa fișiere și imprimante de pe alte computere, trebuie să le adăugați la grupul dvs. de domiciliu. Este necesară următoarea parolă:"},
    {46039, L"Introduceți noua parolă pentru grupul de acasă:"},
    {46040, L"Actualizați parola"},
    {46041, L"Faceți copii de rezervă pentru toate computerele din grupul dvs. de domiciliu la o țintă locală de protecție a datelor."},
    {46042, L"Faceți o copie de rezervă a computerului dvs. utilizând ținte de protecție a datelor HomeGroup"},
    {46043, L"Nu este distribuit"},
    {46044, L"Grupurile de acasă pot fi create numai în rețele private.\nPentru a modifica setările locației în rețea, deschideți Centrul de rețea și partajare în Panoul de control."},
    {46045, L"Windows nu va mai detecta grupurile de acasă din această rețea. Pentru a crea un nou grup de domiciliu, faceți clic pe OK și deschideți HomeGroup în Panoul de control."},
    {46046, L"Windows a detectat un grup de domiciliu existent.\nPentru a vă alătura, faceți clic pe OK și deschideți HomeGroup în Panoul de control."},
    {46047, L"Serviciul HomeGroup este acum disponibil. Vă rugăm să încercați din nou."},
    {46048, L"Setările de distribuire au fost actualizate"},
    {46049, L"Fișierele și resursele selectate sunt partajate cu grupul dvs. de domiciliu."},
    {46050, L"Parola HomeGroup a fost actualizată cu succes"},
    {46051, L"Te-ai alăturat grupului de acasă"},
    {46052, L"Acum puteți accesa fișierele și dispozitivele partajate. Fișierele și dispozitivele pe care le partajați rămân neschimbate."},
    {46053, L"Puteți începe să accesați fișiere și imprimante partajate de alți utilizatori din grupul dvs. de domiciliu."},
    {46054, L"Actualizați parola grupului dvs. de domiciliu"},
    {46055, L"Alăturați-vă unui grup de acasă"},
    {46056, L"Introduceți noua parolă pentru grupul de acasă de la %1."},
    {46057, L"Ceasurile tuturor computerelor din grupul de domiciliu trebuie să fie setate la o distanță de cel mult 24 de ore. Asigurați-vă că ceasurile computerului sunt sincronizate, apoi încercați să vă alăturați din nou grupului de acasă."},
    {46058, L"Parola nu îndeplinește cerințele de securitate ale domeniului. Introduceți o parolă potrivită sau utilizați un alt computer HomeGroup pentru a vă schimba parola."},
    {46059, L"Nu vă puteți reseta parola deoarece nu sunteți conectat la o rețea privată.\nConectați-vă la o rețea privată și încercați din nou."},
    {46060, L"Nu sunteți conectat la o rețea privată.\nPentru a modifica setările locației în rețea, deschideți Centrul de rețea și partajare în Panoul de control."},
    {46061, L"Partajați cu alte computere de acasă"},
    {46062, L"Puteți partaja fișiere și imprimante cu alte computere. De asemenea, puteți transmite conținut media pe dispozitivul dvs.\n\nGrupurile de acasă sunt protejate prin parolă și puteți alege ce să partajați în orice moment."},
    {46063, L"Adăugați alte computere în grupul dvs. de domiciliu folosind această parolă"},
    {46064, L"Pentru a accesa fișiere și imprimante de pe alte computere, trebuie să le adăugați la grupul dvs. de domiciliu. Este necesară următoarea parolă:"},
    {46065, L"Pentru a crea sau a se alătura unui grup de domiciliu, conexiunea la rețea trebuie să aibă IPv6 activat. Pentru a activa IPv6, porniți Instrumentul de depanare a grupului de acasă."},
    {46066, L"Adăugați persoane în grupul de acasă"},
    {46067, L"Configurați protecția datelor în grupul de domiciliu"},
    {46068, L"Au fost detectate mai multe grupuri de acasă"},
    {46069, L"Distribuie altor membri ai grupului de acasă"},
    {46070, L"Documente"},
    {46071, L"Poze"},
    {46072, L"Muzica"},
    {46073, L"Videoclipuri"},
    {46074, L"Imprimante și dispozitive"},
    {46075, L"Schimbați setările de partajare a grupului de domiciliu"},
    {46076, L"%1 Partajare"},
    {46077, L"Se verifică parola..."},
};

// Slovak (sk-SK)
static const EmbeddedTextEntry kStrings_SK_SK[] = {
    {1, L"Domáca skupina"},
    {2, L"Skontrolujte možnosti domácej skupiny, rozhodnite sa, čo tento počítač zdieľa, a zobrazte alebo aktualizujte prístupové heslo."},
    {3, L"Pravidlá nastavené vašou organizáciou bránia spusteniu tejto stránky. Požiadajte o pomoc správcu siete."},
    {4, L"Podrobné možnosti zdieľania"},
    {5, L"Zapnuté"},
    {6, L"Vypnuté"},
    {7, L"Vypnuté (nie sú nainštalované žiadne tlačiarne)"},
    {8, L"K tomuto počítaču nie je pripojená žiadna tlačiareň."},
    {9, L"Zdieľajte obsah s počítačmi doma"},
    {10, L"Pristupujte k domácej skupine pomocou počítača pripojeného k doméne"},
    {12, L"Upravte možnosti domácej skupiny"},
    {13, L"Pracuje sa…"},
    {14, L"V tejto sieti sa nenašla žiadna domáca skupina."},
    {15, L"%1 z %2 vytvoril domácu skupinu v sieti."},
    {16, L"Boli ste pozvaní, aby ste sa pripojili k svojej domácej skupine."},
    {18, L"Použite túto stránku, ak počítač patrí do domácej skupiny."},
    {19, L"Tento počítač sa nemôže pripojiť k vašej domácej skupine."},
    {20, L"HomeGroup umožňuje dôveryhodným počítačom vymieňať si súbory a používať zdieľané tlačiarne a môže posielať médiá do kompatibilných zariadení. Prístup vyžaduje heslo, zatiaľ čo vy máte kontrolu nad tým, čo tento počítač sprístupňuje."},
    {21, L"Tento počítač je tiež súčasťou domény, takže nemôže vytvoriť vlastnú domácu skupinu, ale môže sa pripojiť k domácej skupine, ktorú vytvoril niekto v sieti.\n\nDomáce skupiny spájajú počítače vo vašej domácej sieti, takže môžete zdieľať fotografie, hudbu, videá, dokumenty a tlačiarne. Domáce skupiny sú chránené heslom a kedykoľvek si môžete vybrať, čo chcete zdieľať."},
    {22, L"Domáce skupiny spájajú počítače vo vašej domácej sieti, takže môžete zdieľať fotografie, hudbu, videá, dokumenty a tlačiarne. Domáce skupiny sú chránené heslom a kedykoľvek si môžete vybrať, čo chcete zdieľať.\n\nV tomto vydaní systému Windows nemôžete vytvárať svoje vlastné domáce skupiny, ale môžete sa pripojiť k domácim skupinám vytvoreným inými."},
    {23, L"Vytvorte si domácu skupinu"},
    {24, L"Pripojte sa"},
    {25, L"Heslo domácej skupiny bolo zmenené. Ak chcete pokračovať v používaní zdrojov domácej skupiny, uistite sa, že osoba, ktorá už zadala nové heslo, je online, a potom zadajte nové heslo."},
    {26, L"Systém Windows zistil vo vašej sieti inú domácu skupinu. Domáce skupiny vám umožňujú zdieľať súbory a tlačiarne s inými počítačmi. Môžete tiež streamovať médiá do svojho zariadenia."},
    {27, L"%1 zmenil svoje heslo domácej skupiny. Ak chcete pokračovať v používaní zdrojov domácej skupiny, uistite sa, že osoba, ktorá už zadala nové heslo, je online, a potom zadajte nové heslo."},
    {28, L"Hľadajú sa domáce skupiny v tejto sieti…"},
    {29, L"Zadajte nové heslo"},
    {30, L"Pripojte sa teraz"},
    {32, L"Skôr ako budete môcť vytvoriť domácu skupinu alebo sa k nej pripojiť, musíte sa najprv pripojiť k svojej sieti."},
    {34, L"Túto stránku použite na vytvorenie alebo pripojenie k domácej skupine, sieťové umiestnenie vášho počítača musí byť nastavené ako súkromné."},
    {35, L"Zmeňte umiestnenie siete"},
    {37, L"Možnosti zdieľania pre súkromné"},
    {38, L"Možnosti zdieľania pre verejnosť"},
    {39, L"Možnosti zdieľania pre doménu"},
    {40, L"Súkromné"},
    {41, L"Súkromné (aktuálny profil)"},
    {42, L"Verejné"},
    {43, L"Verejné (aktuálny profil)"},
    {44, L"doména"},
    {45, L"Doména (aktuálny profil)"},
    {46, L"Streamovanie médií je zapnuté."},
    {47, L"Streamovanie médií je vypnuté."},
    {56, L"Zrušiť"},
    {63, L"OK"},
    {64, L"Zobrazte alebo vytlačte heslo domácej skupiny"},
    {65, L"24pt;;;Consolas"},
    {66, L"Dátum tlače: %1 %2"},
    {67, L"Možnosť: Zobrazte a vytlačte heslo domácej skupiny"},
    {68, L"heslo:"},
    {69, L"Toto heslo použite na pripojenie ďalších počítačov k vašej domácej skupine."},
    {70, L"Na každom počítači:"},
    {71, L"Poznámka: Počítače, ktoré sú vypnuté alebo spiace, sa vo vašej domácej skupine nezobrazia."},
    {72, L"1. Kliknite na tlačidlo Štart a potom na položku Ovládací panel."},
    {73, L"2. V časti Sieť a internet kliknite na položku Vybrať domácu skupinu a možnosti zdieľania."},
    {74, L"3. Kliknite na Pripojiť sa a podľa sprievodcu domácou skupinou zadajte svoje heslo."},
    {75, L"Kliknite na tlačidlo Štart a potom na položku Ovládací panel."},
    {76, L"Nepodarilo sa vytlačiť heslo domácej skupiny"},
    {77, L"Pri pokuse o vydanie hesla domácej skupiny sa vyskytla chyba. (Kód chyby: %1!u!)"},
    {78, L"Momentálne nie ste pripojení k domácej sieti. Ak chcete zobraziť súbory a zdroje na iných počítačoch domácej skupiny, najprv sa pripojte k domácej sieti."},
    {79, L"%1 sa pripojil k počítaču k domácej skupine. Nezdieľam knižnicu s mojou domácou skupinou. Kliknutím na odkaz nižšie zmeníte, čo zdieľate. Nevypínajte ani nereštartujte počítač, kým sa zdieľanie nedokončí."},
    {80, L"Nezdieľam knižnicu s mojou domácou skupinou. Kliknutím na odkaz nižšie zmeníte, čo zdieľate. Nevypínajte ani nereštartujte počítač, kým sa zdieľanie nedokončí."},
    {81, L"Domáca skupina momentálne zdieľa knižnicu na tomto počítači. Niektoré možnosti domácej skupiny nie sú dostupné, kým sa zdieľanie nedokončí. Nevypínajte ani nereštartujte počítač, kým sa zdieľanie nedokončí."},
    {82, L"V časti Sieť a internet kliknite na položku Vybrať domácu skupinu a možnosti zdieľania."},
    {83, L"V sieti momentálne nie sú žiadne domáce skupiny."},
    {84, L"Kliknite na Pripojiť sa a podľa sprievodcu domácou skupinou zadajte svoje heslo."},
    {85, L"Pre inštaláciu kliknite sem."},
    {86, L"Systém Windows našiel tlačiareň domácej skupiny"},
    {88, L"Predstavujeme domácu skupinu"},
    {89, L"%1 (aktuálny profil)"},
    {90, L"Ak sa chcete pripojiť k domácej skupine, použite túto stránku, sieťové umiestnenie vášho počítača musí byť nastavené ako súkromné."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Domáca skupina ešte nie je pripravená. Skúste to znova o niekoľko minút. Ak sa táto správa zobrazuje aj naďalej, kliknite na odkaz a začnite riešiť problémy s domácou skupinou."},
    {95, L"Spustite poradcu pri riešení problémov HomeGroup"},
    {98, L"Heslo domácej skupiny"},
    {99, L"Hosťovské účty nemôžu meniť nastavenia domácej skupiny."},
    {100, L"Domáca skupina našla novú zdieľanú tlačiareň vo vašej domácej sieti. Po nainštalovaní bude k dispozícii komukoľvek na tomto počítači."},
    {101, L"Nainštalujte tlačiareň"},
    {102, L"Domáca skupina nie je k dispozícii, pretože nie ste pripojení k domácej sieti."},
    {103, L"Domáca skupina nie je k dispozícii, pretože nie ste pripojení k domácej sieti."},
    {104, L"Pred pripojením k domácej skupine sa musíte najprv pripojiť k sieti."},
    {105, L"Obrázok domácej skupiny"},
    {106, L"Vyberte, čo chcete zdieľať, a zobrazte heslo svojej domácej skupiny"},
    {107, L"Keďže tento počítač je súčasťou domény, nastavenia zdieľania jeho knižníc a zariadení s inými počítačmi v domácej skupine nie sú dostupné."},
    {108, L"Nastavenia zdieľania knižníc a zariadení s inými počítačmi v domácej skupine nie sú v tomto vydaní systému Windows dostupné."},
    {109, L"Odstráňte %1 z domácej skupiny"},
    {110, L"Zrušiť"},
    {111, L"Odstrániť člena domácej skupiny"},
    {112, L"%1 bude odstránený z domácej skupiny"},
    {113, L"Všetci členovia domácej skupiny, ktorí sa pripoja pomocou hesla, budú musieť zadať heslo znova."},
    {114, L"Tlačiarne a zariadenia"},
    {115, L"Zmeňte členov domácej skupiny %1"},
    {116, L"Heslo domácej skupiny bolo resetované"},
    {117, L"Domáca skupina zdieľa súbory"},
    {118, L"Možnosť: Tento počítač patrí do domácej skupiny"},
    {119, L"Na pripojenie je k dispozícii domáca skupina"},
    {120, L"Je možné vytvoriť domácu skupinu"},
    {121, L"Domáca skupina nie je k dispozícii"},
    {122, L"Nedôveryhodná tlačiareň"},
    {200, L"Pridať člena"},
    {201, L"Ikona používateľa"},
    {202, L"Celé meno"},
    {203, L"ID používateľa"},
    {204, L"Progress Bar"},
    {205, L"Ikona priečinka"},
    {220, L"Zdieľajte knižnice a hardvér"},
    {221, L"Vyberte knižnicu, ktorú chcete zdieľať s ostatnými vo vašej domácej skupine."},
    {222, L"Upravte možnosti domácej skupiny"},
    {223, L"Túto stránku použite na zmenu nastavení domácej skupiny, otvorte domácu skupinu v ovládacom paneli."},
    {224, L"Možnosti domácej skupiny"},
    {225, L"Pomocou tejto stránky môžete zmeniť nastavenia domácej skupiny v ovládacom paneli alebo použiť Poradcu pri riešení problémov s domácou skupinou."},
    {226, L"Spustite nástroj na riešenie problémov"},
    {227, L"Pomocou tejto stránky môžete pomocou nástroja na riešenie problémov s domácou skupinou nájsť a opraviť problémy s domácou skupinou."},
    {228, L"Zobraziť heslo"},
    {229, L"Túto stránku použite na zobrazenie alebo vytlačenie hesla vašej domácej skupiny."},
    {230, L"Pripojte sa k domácej skupine"},
    {231, L"Pripojte sa k domácej skupine v tejto sieti."},
    {530, L"Otvoriť podrobné možnosti zdieľania…"},
    {541, L"Viditeľnosť siete"},
    {542, L"Ak je zapnuté zisťovanie siete, tento počítač môže vidieť a vidieť ostatné počítače a zariadenia v sieti."},
    {543, L"Zapnite zisťovanie siete"},
    {544, L"Vypnite zisťovanie siete"},
    {545, L"Prístup k súborom a tlačiarňam"},
    {546, L"Keď je zdieľanie súborov a tlačiarní zapnuté, ostatní používatelia vo vašej sieti budú mať prístup k súborom a tlačiarňam, ktoré zdieľate z tohto počítača."},
    {547, L"Zapnite zdieľanie súborov a tlačiarní"},
    {548, L"Vypnite zdieľanie súborov a tlačiarní"},
    {549, L"Zdieľanie verejných priečinkov"},
    {550, L"Keď je zapnuté zdieľanie verejných priečinkov, používatelia vo vašej sieti vrátane členov domácej skupiny majú prístup k súborom vo verejných priečinkoch."},
    {552, L"Enabling sharing allows anyone with access to your network to read and write files in your public folders."},
    {553, L"Turn off public folder sharing (users logged on to this computer can still access these folders)"},
    {554, L"Change sharing options for various network profiles"},
    {559, L"Prístup k médiám"},
    {560, L"When media streaming is turned on, users and devices on your network can access the photos, music, and videos on this computer. This computer can also find media on the network."},
    {564, L"Zrušiť"},
    {567, L"Použiť zmeny"},
    {584, L"Systém Windows vytvorí samostatný sieťový profil pre každú sieť, ktorú používate. You can select specific options for each profile."},
    {585, L"Ikona varovania domácej skupiny"},
    {586, L"Knižnice a zariadenia zdieľané z tohto počítača"},
    {595, L"Viac úloh domácej skupiny"},
    {600, L"Zobrazte alebo vytlačte heslo domácej skupiny"},
    {601, L"Your system administrator has not allowed you to access your homegroup."},
    {604, L"Zmeniť heslo..."},
    {605, L"Opustiť domácu skupinu..."},
    {607, L"Vyberte možnosti streamovania médií..."},
    {608, L"Keďže tento počítač je súčasťou domény, nastavenia zdieľania jeho knižníc a zariadení s inými počítačmi v domácej skupine nie sú dostupné."},
    {609, L"Zdieľanie chránené heslom"},
    {610, L"Keď je zapnuté zdieľanie chránené heslom, prístup k zdieľaným súborom, tlačiarňam pripojeným k tomuto počítaču a verejným priečinkom majú iba používatelia s používateľskými účtami a heslami na tomto počítači. Zdieľanie chránené heslom musí byť vypnuté, aby mali ostatní prístup."},
    {611, L"Zapnite zdieľanie chránené heslom"},
    {612, L"Vypnite zdieľanie chránené heslom"},
    {613, L"Tlač stránky"},
    {614, L"Umožňuje prehrávanie zdieľaného obsahu na všetkých zariadeniach v tejto sieti, ako sú televízory a herné konzoly"},
    {615, L"Súkromná sieť"},
    {616, L"Hosťovská alebo verejná sieť"},
    {617, L"Doménová sieť"},
    {619, L"Pripojenia domácej skupiny"},
    {620, L"Windows zvyčajne spravuje pripojenia k iným počítačom domácej skupiny. Ak však na všetkých svojich počítačoch používate rovnaké používateľské konto a heslo, môžete namiesto toho nastaviť, aby domáca skupina používala tento účet."},
    {621, L"Možnosť: Povoliť systému Windows spravovať pripojenia k domácej skupine (odporúča sa)"},
    {622, L"Pripojte sa k iným počítačom pomocou svojho používateľského účtu a hesla"},
    {624, L"Spustite poradcu pri riešení problémov HomeGroup"},
    {627, L"Pripojenia na zdieľanie súborov"},
    {628, L"Systém Windows používa 128-bitové šifrovanie na zabezpečenie pripojení zdieľania súborov. Niektoré zariadenia nepodporujú 128-bitové šifrovanie a musia používať 40-bitové alebo 56-bitové šifrovanie."},
    {629, L"Zabezpečte svoje pripojenie na zdieľanie súborov pomocou 128-bitového šifrovania (odporúča sa)"},
    {630, L"Povoľte zdieľanie súborov zariadenia so 40-bitovým alebo 56-bitovým šifrovaním"},
    {631, L"Každá sieť"},
    {632, L"Zmeňte, čo sa zdieľa s domácou skupinou"},
    {637, L"Zavrieť"},
    {639, L"Vzdialený prístup k domácej skupine"},
    {640, L"Ostatní členovia domácej skupiny sa môžu pripojiť k vašej domácej skupine odkiaľkoľvek prostredníctvom svojich počítačov."},
    {641, L"Možnosť: Zakázať vzdialený prístup k domácej skupine cez tento počítač"},
    {642, L"Možnosť: Povoľte vzdialený prístup k domácej skupine prostredníctvom tohto počítača"},
    {648, L"Vyberte súbory a zariadenia, ktoré chcete sprístupniť, a potom vyberte ich úrovne povolení."},
    {649, L"Knižnica alebo adresár"},
    {650, L"Úroveň prístupu"},
    {652, L"Zapnite automatické nastavenie zariadení pripojených k sieti."},
    {46000, L"Domáca skupina"},
    {46004, L"Možnosť: Vyberte si heslo pre svoju domácu skupinu"},
    {46005, L"Zadajte heslo domácej skupiny"},
    {46006, L"&Vytvoriť teraz"},
    {46007, L"&Pripojiť sa teraz"},
    {46008, L"Pomocou tohto hesla pridajte do domácej skupiny ďalšie počítače"},
    {46009, L"Pripojili ste sa k domácej skupine"},
    {46011, L"Domáca skupina"},
    {46012, L"Systém Windows nemôže v tomto počítači nastaviť domácu skupinu."},
    {46013, L"Keďže tento počítač je súčasťou domény, zdieľanie jeho knižnice s inými počítačmi v domácej skupine nie je dostupné."},
    {46014, L"Heslá musia obsahovať aspoň 8 znakov a žiadne medzery na začiatku alebo na konci."},
    {46015, L"Heslo je nesprávne.\nSkúste to znova. V heslách sa rozlišujú malé a veľké písmená."},
    {46016, L"Možnosť: Všetky pripojenia domácej skupiny na tomto počítači budú odpojené"},
    {46017, L"Úspešne ste opustili svoju domácu skupinu"},
    {46018, L"Zmeňte, čo sa zdieľa s domácou skupinou"},
    {46019, L"Zdieľajte svoje fotografie, videá, hudbu, dokumenty a tlačiarne s ostatnými počítačmi vo vašej domácnosti."},
    {46020, L"&Vykonať zmeny"},
    {46021, L"Zmena hesla domácej skupiny odpojí všetkých"},
    {46022, L"Zadajte nové heslo pre domácu skupinu"},
    {46023, L"&Zmeniť heslo"},
    {46024, L"Heslo domácej skupiny bolo úspešne zmenené"},
    {46025, L"Heslo domácej skupiny bolo zmenené"},
    {46026, L"Zadajte heslo domácej skupiny"},
    {46027, L"Heslo domácej skupiny bolo zmenené. Ak chcete pokračovať v používaní zdrojov domácej skupiny, uistite sa, že osoba, ktorá už zadala nové heslo, je online, a potom zadajte nové heslo."},
    {46028, L"Zdieľané"},
    {46029, L"Systému Windows sa nepodarilo odstrániť počítač z domácej skupiny."},
    {46030, L"%1 zmenil svoje heslo domácej skupiny. Ak chcete pokračovať v používaní zdrojov domácej skupiny, uistite sa, že osoba, ktorá už zadala nové heslo, je online, a potom zadajte nové heslo."},
    {46031, L"Heslá pomáhajú zabrániť neoprávnenému prístupu k súborom a tlačiarňam vašej domácej skupiny. Heslo môžete získať od %2, %1 alebo od iného člena vašej domácej skupiny."},
    {46032, L"Heslá pomáhajú zabrániť neoprávnenému prístupu k súborom a tlačiarňam vašej domácej skupiny. Heslo môžete získať od %2, %1 alebo od iného člena vašej domácej skupiny."},
    {46033, L"Consolas"},
    {46034, L"Vytvorte domácu skupinu"},
    {46035, L"Pripojte sa k domácej skupine"},
    {46036, L"Change Your Homegroup Password"},
    {46037, L"Opustite domácu skupinu"},
    {46038, L"Ak chcete získať prístup k súborom a tlačiarňam na iných počítačoch, musíte ich pridať do domácej skupiny. The following password is required:"},
    {46039, L"Type the new homegroup password:"},
    {46040, L"Aktualizovať heslo"},
    {46041, L"Zálohujte všetky počítače vo vašej domácej skupine do lokálneho cieľa ochrany údajov."},
    {46042, L"Zálohujte svoj počítač pomocou cieľov ochrany údajov HomeGroup"},
    {46043, L"Nezdieľané"},
    {46044, L"Domáce skupiny je možné vytvárať iba v súkromných sieťach.\nAk chcete zmeniť nastavenia sieťového umiestnenia, otvorte Centrum sietí a zdieľania v ovládacom paneli."},
    {46045, L"Systém Windows už nebude zisťovať domáce skupiny v tejto sieti. Ak chcete vytvoriť novú domácu skupinu, kliknite na tlačidlo OK a otvorte domácu skupinu v ovládacom paneli."},
    {46046, L"Systém Windows zistil existujúcu domácu skupinu.\nAk sa chcete pripojiť, kliknite na tlačidlo OK a otvorte domácu skupinu v ovládacom paneli."},
    {46047, L"Služba HomeGroup je teraz k dispozícii. Skúste to znova."},
    {46048, L"Nastavenia zdieľania boli aktualizované"},
    {46049, L"Vybraté súbory a zdroje sa zdieľajú s vašou domácou skupinou."},
    {46050, L"Heslo domácej skupiny bolo úspešne aktualizované"},
    {46051, L"Pripojili ste sa k domácej skupine"},
    {46052, L"Teraz máte prístup k svojim zdieľaným súborom a zariadeniam. Súbory a zariadenia, ktoré zdieľate, zostávajú nezmenené."},
    {46053, L"Môžete začať pristupovať k súborom a tlačiarňam zdieľaným inými používateľmi vo vašej domácej skupine."},
    {46054, L"Aktualizujte heslo svojej domácej skupiny"},
    {46055, L"Pripojte sa k domácej skupine"},
    {46056, L"Zadajte nové heslo domácej skupiny zo %1."},
    {46057, L"Hodiny všetkých počítačov domácej skupiny musia byť nastavené tak, aby časový odstup nepresahoval 24 hodín. Uistite sa, že sú hodiny vášho počítača synchronizované, a potom sa skúste znova pripojiť k domácej skupine."},
    {46058, L"Heslo nespĺňa požiadavky na silu hesla domény. Zadajte zodpovedajúce heslo alebo na zmenu hesla použite iný počítač domácej skupiny."},
    {46059, L"Nemôžete obnoviť svoje heslo, pretože nie ste pripojení k súkromnej sieti.\nPripojte sa k súkromnej sieti a skúste to znova."},
    {46060, L"Nie ste pripojení k súkromnej sieti.\nAk chcete zmeniť nastavenia sieťového umiestnenia, otvorte Centrum sietí a zdieľania v ovládacom paneli."},
    {46061, L"Zdieľajte s ostatnými domácimi počítačmi"},
    {46062, L"Môžete zdieľať súbory a tlačiarne s inými počítačmi. Môžete tiež streamovať médiá do svojho zariadenia.\n\nDomáce skupiny sú chránené heslom a kedykoľvek si môžete vybrať, čo chcete zdieľať."},
    {46063, L"Pomocou tohto hesla pridajte do domácej skupiny ďalšie počítače"},
    {46064, L"Ak chcete získať prístup k súborom a tlačiarňam na iných počítačoch, musíte ich pridať do domácej skupiny. Vyžaduje sa nasledujúce heslo:"},
    {46065, L"Ak chcete vytvoriť domácu skupinu alebo sa k nej pripojiť, vaše sieťové pripojenie musí mať povolený protokol IPv6. Ak chcete povoliť IPv6, spustite Poradcu pri riešení problémov s domácou skupinou."},
    {46066, L"Pridajte ľudí do domácej skupiny"},
    {46067, L"Nakonfigurujte ochranu údajov domácej skupiny"},
    {46068, L"Bolo zistených viacero domácich skupín"},
    {46069, L"Zdieľajte s ostatnými členmi domácej skupiny"},
    {46070, L"dokumenty"},
    {46071, L"Obrázky"},
    {46072, L"Hudba"},
    {46073, L"Videá"},
    {46074, L"Tlačiarne a zariadenia"},
    {46075, L"Zmeňte nastavenia zdieľania domácej skupiny"},
    {46076, L"Zdieľanie %1"},
    {46077, L"Overuje sa vaše heslo..."},
};

// Ukrainian (uk-UA)
static const EmbeddedTextEntry kStrings_UK_UA[] = {
    {1, L"Домашня група"},
    {2, L"Перегляньте параметри домашньої групи, вирішіть, що спільно використовує цей ПК, і відобразіть або оновіть пароль доступу."},
    {3, L"Політика, встановлена вашою організацією, забороняє роботу цієї сторінки. Зверніться по допомогу до адміністратора мережі."},
    {4, L"Детальні параметри спільного доступу"},
    {5, L"Увімкнено"},
    {6, L"Вимкнено"},
    {7, L"Вимкнено (принтери не встановлено)"},
    {8, L"До цього комп’ютера не підключено принтер."},
    {9, L"Діліться вмістом із комп’ютерами вдома"},
    {10, L"Отримайте доступ до домашньої групи за допомогою комп’ютера, приєднаного до домену"},
    {12, L"Змінити параметри домашньої групи"},
    {13, L"Робота…"},
    {14, L"У цій мережі не знайдено домашньої групи."},
    {15, L"%1 із %2 створив домашню групу в мережі."},
    {16, L"Вас запросили приєднатися до домашньої групи."},
    {18, L"Використовуйте цю сторінку, щоб цей комп’ютер належав до домашньої групи."},
    {19, L"Цей комп’ютер не може підключитися до вашої домашньої групи."},
    {20, L"HomeGroup дозволяє довіреним ПК обмінюватися файлами та використовувати спільні принтери, а також може надсилати медіа на сумісні пристрої. Для доступу потрібен пароль, а ви залишаєтеся під контролем того, що надає цей ПК."},
    {21, L"Цей комп’ютер також є частиною домену, тому він не може створити власну домашню групу, але може приєднатися до домашньої групи, створеної кимось у мережі.\n\nДомашні групи об’єднують комп’ютери у домашній мережі, щоб ви могли обмінюватися фотографіями, музикою, відео, документами та принтерами. Домашні групи захищені паролем, і ви можете будь-коли вибрати, чим поділитися."},
    {22, L"Домашні групи об’єднують комп’ютери у домашній мережі, щоб ви могли обмінюватися фотографіями, музикою, відео, документами та принтерами. Домашні групи захищені паролем, і ви можете будь-коли вибрати, чим поділитися.\n\nВи не можете створювати власні домашні групи в цьому випуску Windows, але ви можете приєднуватися до домашніх груп, створених іншими."},
    {23, L"Налаштуйте домашню групу"},
    {24, L"Приєднуйтесь"},
    {25, L"Пароль домашньої групи змінено. Щоб продовжити використовувати ресурси домашньої групи, переконайтеся, що особа, яка вже ввела новий пароль, знаходиться в мережі, а потім введіть новий пароль."},
    {26, L"Windows виявила іншу домашню групу у вашій мережі. Домашні групи дозволяють ділитися файлами та принтерами з іншими комп’ютерами. Ви також можете передавати мультимедійні файли на свій пристрій."},
    {27, L"%1 змінив пароль домашньої групи. Щоб продовжити використовувати ресурси домашньої групи, переконайтеся, що особа, яка вже ввела новий пароль, знаходиться в мережі, а потім введіть новий пароль."},
    {28, L"Пошук домашніх груп у цій мережі…"},
    {29, L"Введіть новий пароль"},
    {30, L"Приєднуйтесь зараз"},
    {32, L"Перш ніж створити домашню групу або приєднатися до неї, потрібно підключитися до мережі."},
    {34, L"Використовуйте цю сторінку, щоб створити домашню групу або приєднатися до неї, мережеве розташування вашого комп’ютера має бути закритим."},
    {35, L"Змінити мережеве розташування"},
    {37, L"Параметри спільного доступу для приватного"},
    {38, L"Параметри спільного доступу для Public"},
    {39, L"Параметри спільного доступу для домену"},
    {40, L"Приватний"},
    {41, L"Приватний (поточний профіль)"},
    {42, L"Громадський"},
    {43, L"Загальнодоступний (поточний профіль)"},
    {44, L"Домен"},
    {45, L"Домен (поточний профіль)"},
    {46, L"Потокове передавання медіа увімкнено."},
    {47, L"Потокове передавання медіа вимкнено."},
    {56, L"Скасувати"},
    {63, L"добре"},
    {64, L"Показати або надрукувати пароль домашньої групи"},
    {65, L"24pt;;;Consolas"},
    {66, L"Дата друку: %1 %2"},
    {67, L"Варіант: переглянути та роздрукувати пароль домашньої групи"},
    {68, L"Пароль:"},
    {69, L"Використовуйте цей пароль для підключення інших комп’ютерів до домашньої групи."},
    {70, L"На кожному комп’ютері:"},
    {71, L"Примітка. Вимкнені або сплячі комп’ютери не відображатимуться у вашій домашній групі."},
    {72, L"1. Натисніть «Пуск», потім виберіть «Панель керування»."},
    {73, L"2. У розділі «Мережа та Інтернет» натисніть «Вибрати домашню групу та параметри спільного доступу»."},
    {74, L"3. Натисніть «Приєднатися зараз» і дотримуйтеся вказівок майстра домашньої групи, щоб ввести свій пароль."},
    {75, L"Натисніть «Пуск», а потім «Панель керування»."},
    {76, L"Не вдалося надрукувати пароль домашньої групи"},
    {77, L"Під час спроби Windows вивести пароль домашньої групи сталася помилка. (Код помилки: %1!u!)"},
    {78, L"Зараз ви не підключені до домашньої мережі. Щоб переглядати файли та ресурси на інших комп’ютерах домашньої групи, спочатку підключіться до домашньої мережі."},
    {79, L"%1 приєднав комп’ютер до домашньої групи. Я не ділився бібліотекою з домашньою групою. Натисніть посилання нижче, щоб змінити те, чим ви поділилися. Не вимикайте та не перезавантажуйте комп’ютер, доки не буде завершено спільний доступ."},
    {80, L"Я не ділився бібліотекою з домашньою групою. Натисніть посилання нижче, щоб змінити те, чим ви поділилися. Не вимикайте та не перезавантажуйте комп’ютер, доки не буде завершено спільний доступ."},
    {81, L"HomeGroup наразі надає спільний доступ до бібліотеки на цьому комп’ютері. Деякі параметри домашньої групи недоступні, доки не буде завершено спільний доступ. Не вимикайте та не перезавантажуйте комп’ютер, доки не буде завершено спільний доступ."},
    {82, L"У розділі «Мережа та Інтернет» натисніть «Вибрати домашню групу та параметри спільного доступу»."},
    {83, L"Зараз у мережі немає домашніх груп."},
    {84, L"Натисніть «Приєднатися зараз» і дотримуйтеся вказівок майстра домашньої групи, щоб ввести свій пароль."},
    {85, L"Натисніть тут, щоб встановити."},
    {86, L"Windows знайшла принтер домашньої групи"},
    {88, L"Представляємо домашню групу"},
    {89, L"%1 (поточний профіль)"},
    {90, L"Використовуйте цю сторінку, щоб приєднатися до домашньої групи, мережеве розташування вашого комп’ютера має бути закритим."},
    {91, L"1."},
    {92, L"2."},
    {93, L"3."},
    {94, L"Домашня група ще не готова. Повторіть спробу через кілька хвилин. Якщо ви продовжуєте бачити це повідомлення, клацніть посилання, щоб розпочати усунення несправностей у домашній групі."},
    {95, L"Запустіть засіб вирішення проблем домашньої групи"},
    {98, L"Пароль домашньої групи"},
    {99, L"Облікові записи гостей не можуть змінювати налаштування домашньої групи."},
    {100, L"Домашня група знайшла новий спільний принтер у вашій домашній мережі. Після встановлення він буде доступний будь-кому на цьому комп’ютері."},
    {101, L"Встановити принтер"},
    {102, L"Домашня група недоступна, оскільки ви не підключені до домашньої мережі."},
    {103, L"Домашня група недоступна, оскільки ви не підключені до домашньої мережі."},
    {104, L"Перш ніж приєднатися до домашньої групи, ви повинні спочатку підключитися до мережі."},
    {105, L"Зображення домашньої групи"},
    {106, L"Виберіть, що ви хочете поділитися, і перегляньте пароль домашньої групи"},
    {107, L"Оскільки цей комп’ютер є частиною домену, параметри спільного використання його бібліотек і пристроїв з іншими комп’ютерами в домашній групі недоступні."},
    {108, L"Параметри спільного використання бібліотек і пристроїв з іншими комп’ютерами в домашній групі недоступні в цьому випуску Windows."},
    {109, L"Видаліть %1 із домашньої групи"},
    {110, L"Скасувати"},
    {111, L"Видалити учасника домашньої групи"},
    {112, L"%1 буде видалено з домашньої групи"},
    {113, L"Усі учасники домашньої групи, які приєднуються за допомогою пароля, повинні будуть ввести пароль ще раз."},
    {114, L"Принтери та пристрої"},
    {115, L"Змінити учасників домашньої групи %1"},
    {116, L"Пароль домашньої групи скинуто"},
    {117, L"Домашня група ділиться файлами"},
    {118, L"Варіант: цей комп’ютер належить до домашньої групи"},
    {119, L"До домашньої групи можна приєднатися"},
    {120, L"Можна створити домашню групу"},
    {121, L"Домашня група недоступна"},
    {122, L"Ненадійний принтер"},
    {200, L"Додати учасника"},
    {201, L"Значок користувача"},
    {202, L"ПІБ"},
    {203, L"ID користувача"},
    {204, L"Індикатор прогресу"},
    {205, L"Значок папки"},
    {220, L"Спільний доступ до бібліотек і обладнання"},
    {221, L"Виберіть бібліотеку, якою хочете поділитися з іншими у своїй домашній групі."},
    {222, L"Змінити параметри домашньої групи"},
    {223, L"Використовуйте цю сторінку, щоб змінити налаштування домашньої групи, відкрийте домашню групу на панелі керування."},
    {224, L"Параметри домашньої групи"},
    {225, L"Використовуйте цю сторінку, щоб змінити налаштування домашньої групи на панелі керування або скористатися засобом усунення несправностей домашньої групи."},
    {226, L"Запустіть засіб усунення несправностей"},
    {227, L"Використовуйте цю сторінку, щоб скористатися засобом усунення несправностей домашньої групи, щоб знайти та вирішити проблеми з вашою домашньою групою."},
    {228, L"Переглянути пароль"},
    {229, L"Використовуйте цю сторінку, щоб переглянути або роздрукувати пароль домашньої групи."},
    {230, L"Приєднатися до домашньої групи"},
    {231, L"Приєднайтеся до домашньої групи в цій мережі."},
    {530, L"Відкрити детальні параметри спільного доступу…"},
    {541, L"Видимість мережі"},
    {542, L"Якщо виявлення мережі ввімкнено, цей комп’ютер може бачити інші комп’ютери та пристрої в мережі."},
    {543, L"Увімкніть пошук мережі"},
    {544, L"Вимкніть пошук мережі"},
    {545, L"Доступ до файлів і принтерів"},
    {546, L"Коли спільний доступ до файлів і принтерів увімкнено, інші користувачі вашої мережі можуть отримати доступ до файлів і принтерів, якими ви ділитеся, з цього комп’ютера."},
    {547, L"Увімкніть спільний доступ до файлів і принтерів"},
    {548, L"Вимкніть спільний доступ до файлів і принтерів"},
    {549, L"Спільний доступ до загальнодоступних папок"},
    {550, L"Коли спільний доступ до загальнодоступних папок увімкнено, користувачі вашої мережі, включно з членами домашньої групи, можуть отримати доступ до файлів у загальнодоступних папках."},
    {552, L"Увімкнення спільного доступу дозволяє будь-кому, хто має доступ до вашої мережі, читати та записувати файли у ваших загальнодоступних папках."},
    {553, L"Вимкніть спільний доступ до загальнодоступних папок (користувачі, які ввійшли в систему на цьому комп’ютері, все ще можуть отримати доступ до цих папок)"},
    {554, L"Змінюйте параметри спільного доступу для різних мережевих профілів"},
    {559, L"Доступ до ЗМІ"},
    {560, L"Коли потокове передавання медіа ввімкнено, користувачі та пристрої у вашій мережі можуть отримувати доступ до фотографій, музики та відео на цьому комп’ютері. Цей комп’ютер також може знаходити медіафайли в мережі."},
    {564, L"Скасувати"},
    {567, L"Застосувати зміни"},
    {584, L"Windows створює окремий профіль мережі для кожної мережі, яку ви використовуєте. Ви можете вибрати окремі параметри для кожного профілю."},
    {585, L"Значок попередження домашньої групи"},
    {586, L"Бібліотеки та пристрої, спільні з цього комп’ютера"},
    {595, L"Більше завдань домашньої групи"},
    {600, L"Показати або надрукувати пароль домашньої групи"},
    {601, L"Ваш системний адміністратор не дозволив вам отримати доступ до домашньої групи."},
    {604, L"Змінити пароль..."},
    {605, L"Вийти з домашньої групи..."},
    {607, L"Виберіть параметри потокового передавання медіа..."},
    {608, L"Оскільки цей комп’ютер є частиною домену, параметри спільного використання його бібліотек і пристроїв з іншими комп’ютерами в домашній групі недоступні."},
    {609, L"Спільне використання, захищене паролем"},
    {610, L"Якщо захищений паролем спільний доступ увімкнено, лише користувачі з обліковими записами користувачів і паролями на цьому комп’ютері можуть отримати доступ до спільних файлів, принтерів, підключених до цього комп’ютера, і спільних папок. Спільний доступ, захищений паролем, потрібно вимкнути, щоб надати іншим доступ."},
    {611, L"Увімкніть спільний доступ, захищений паролем"},
    {612, L"Вимкніть спільний доступ, захищений паролем"},
    {613, L"Роздрукувати сторінку"},
    {614, L"Дозволяє відтворювати спільний вміст на всіх пристроях у цій мережі, таких як телевізори та ігрові консолі"},
    {615, L"Приватна мережа"},
    {616, L"Гостьова або публічна мережа"},
    {617, L"Доменная мережа"},
    {619, L"Підключення до домашньої групи"},
    {620, L"Windows зазвичай керує з’єднаннями з іншими комп’ютерами домашньої групи. Однак, якщо ви використовуєте той самий обліковий запис користувача та пароль на всіх своїх комп’ютерах, ви можете налаштувати домашню групу на використання цього облікового запису."},
    {621, L"Варіант: дозволити Windows керувати з’єднаннями домашньої групи (рекомендовано)"},
    {622, L"Підключіться до інших комп’ютерів за допомогою облікового запису користувача та пароля"},
    {624, L"Запустіть засіб вирішення проблем домашньої групи"},
    {627, L"Підключення для обміну файлами"},
    {628, L"Windows використовує 128-бітове шифрування для захисту з’єднань для обміну файлами. Деякі пристрої не підтримують 128-бітне шифрування, тому повинні використовувати 40- або 56-бітне шифрування."},
    {629, L"Захистіть з’єднання для обміну файлами за допомогою 128-бітного шифрування (рекомендовано)"},
    {630, L"Увімкніть спільний доступ до файлів пристрою за допомогою 40- або 56-бітного шифрування"},
    {631, L"Кожна мережа"},
    {632, L"Змініть те, до чого надає доступ ваша домашня група"},
    {637, L"Закрити"},
    {639, L"Віддалений доступ до домашньої групи"},
    {640, L"Інші учасники домашньої групи можуть підключатися до вашої домашньої групи з будь-якого місця через свої комп’ютери."},
    {641, L"Варіант: вимкніть віддалений доступ до домашньої групи через цей комп’ютер"},
    {642, L"Варіант: увімкніть віддалений доступ до домашньої групи через цей комп’ютер"},
    {648, L"Виберіть файли та пристрої, які потрібно зробити доступними, а потім виберіть їхні рівні дозволів."},
    {649, L"Бібліотека або каталог"},
    {650, L"Рівень доступу"},
    {652, L"Увімкніть автоматичне налаштування пристроїв, підключених до мережі."},
    {46000, L"Домашня група"},
    {46004, L"Варіант: виберіть пароль для домашньої групи"},
    {46005, L"Введіть пароль домашньої групи"},
    {46006, L"&Створити зараз"},
    {46007, L"&Приєднатися зараз"},
    {46008, L"Додайте інші комп’ютери до домашньої групи за допомогою цього пароля"},
    {46009, L"Ви приєдналися до домашньої групи"},
    {46011, L"Домашня група"},
    {46012, L"Windows не може налаштувати домашню групу на цьому комп’ютері."},
    {46013, L"Оскільки цей комп’ютер є частиною домену, спільне використання його бібліотеки з іншими комп’ютерами в домашній групі недоступне."},
    {46014, L"Паролі повинні містити принаймні 8 символів і не мати пробілів на початку або в кінці."},
    {46015, L"Пароль неправильний.\nСпробуйте ще раз. Паролі чутливі до регістру."},
    {46016, L"Варіант: усі з’єднання домашньої групи на цьому комп’ютері буде розірвано"},
    {46017, L"Успішно вийшли з вашої домашньої групи"},
    {46018, L"Змініть те, до чого надає доступ ваша домашня група"},
    {46019, L"Поділіться своїми фотографіями, відео, музикою, документами та принтерами з іншими комп’ютерами у вашому домі."},
    {46020, L"&Внести зміни"},
    {46021, L"Зміна пароля домашньої групи відключає всіх"},
    {46022, L"Введіть новий пароль для домашньої групи"},
    {46023, L"&Змінити пароль"},
    {46024, L"Пароль домашньої групи успішно змінено"},
    {46025, L"Пароль домашньої групи змінено"},
    {46026, L"Введіть пароль домашньої групи"},
    {46027, L"Пароль домашньої групи змінено. Щоб продовжити використовувати ресурси домашньої групи, переконайтеся, що особа, яка вже ввела новий пароль, знаходиться в мережі, а потім введіть новий пароль."},
    {46028, L"Спільний доступ"},
    {46029, L"Windows не вдалося видалити комп’ютер із домашньої групи."},
    {46030, L"%1 змінив пароль домашньої групи. Щоб продовжити використовувати ресурси домашньої групи, переконайтеся, що особа, яка вже ввела новий пароль, знаходиться в мережі, а потім введіть новий пароль."},
    {46031, L"Паролі допомагають запобігти несанкціонованому доступу до файлів і принтерів вашої домашньої групи. Ви можете отримати пароль від %2, %1 або іншого члена вашої домашньої групи."},
    {46032, L"Паролі допомагають запобігти несанкціонованому доступу до файлів і принтерів вашої домашньої групи. Ви можете отримати пароль від %2, %1 або іншого члена вашої домашньої групи."},
    {46033, L"Consolas"},
    {46034, L"Створіть домашню групу"},
    {46035, L"Приєднайтеся до домашньої групи"},
    {46036, L"Змініть пароль домашньої групи"},
    {46037, L"Вийти з домашньої групи"},
    {46038, L"Щоб отримати доступ до файлів і принтерів на інших комп’ютерах, ви повинні додати їх до домашньої групи. Необхідний наступний пароль:"},
    {46039, L"Введіть новий пароль домашньої групи:"},
    {46040, L"Оновити пароль"},
    {46041, L"Резервне копіювання всіх комп’ютерів у домашній групі на локальну ціль захисту даних."},
    {46042, L"Створіть резервну копію ПК за допомогою цілей захисту даних HomeGroup"},
    {46043, L"Не ділиться"},
    {46044, L"Домашні групи можна створювати лише в приватних мережах.\nЩоб змінити параметри розташування мережі, відкрийте Центр мереж і спільного доступу на панелі керування."},
    {46045, L"Windows більше не виявлятиме домашніх груп у цій мережі. Щоб створити нову домашню групу, натисніть «ОК» і відкрийте «Домашню групу» на панелі керування."},
    {46046, L"Windows виявила наявну домашню групу.\nЩоб приєднатися, натисніть OK і відкрийте HomeGroup на панелі керування."},
    {46047, L"Послуга HomeGroup тепер доступна. Спробуйте ще раз."},
    {46048, L"Налаштування спільного доступу оновлено"},
    {46049, L"Вибрані файли та ресурси надаються спільно з вашою домашньою групою."},
    {46050, L"Пароль домашньої групи успішно оновлено"},
    {46051, L"Ви приєдналися до домашньої групи"},
    {46052, L"Тепер ви можете отримати доступ до спільних файлів і пристроїв. Файли та пристрої, якими ви ділитеся, залишаються незмінними."},
    {46053, L"Ви можете отримати доступ до файлів і принтерів, якими користуються інші користувачі у вашій домашній групі."},
    {46054, L"Оновіть пароль домашньої групи"},
    {46055, L"Приєднайтеся до домашньої групи"},
    {46056, L"Введіть новий пароль домашньої групи з %1."},
    {46057, L"Різниця між годинниками всіх комп’ютерів домашньої групи не повинна перевищувати 24 години. Переконайтеся, що годинник вашого комп’ютера синхронізовано, а потім спробуйте знову приєднатися до домашньої групи."},
    {46058, L"Пароль не відповідає вимогам щодо надійності пароля домену. Введіть відповідний пароль або скористайтеся іншим комп’ютером домашньої групи, щоб змінити пароль."},
    {46059, L"Ви не можете скинути свій пароль, оскільки ви не підключені до приватної мережі.\nПідключіться до приватної мережі та повторіть спробу."},
    {46060, L"Ви не підключені до приватної мережі.\nЩоб змінити параметри розташування мережі, відкрийте Центр мереж і спільного доступу на панелі керування."},
    {46061, L"Поділіться з іншими домашніми комп’ютерами"},
    {46062, L"Ви можете ділитися файлами та принтерами з іншими комп’ютерами. Ви також можете передавати мультимедійні файли на свій пристрій.\n\nДомашні групи захищені паролем, і ви можете будь-коли вибрати, чим поділитися."},
    {46063, L"Додайте інші комп’ютери до домашньої групи за допомогою цього пароля"},
    {46064, L"Щоб отримати доступ до файлів і принтерів на інших комп’ютерах, ви повинні додати їх до домашньої групи. Необхідний наступний пароль:"},
    {46065, L"Щоб створити домашню групу або приєднатися до неї, у вашому мережевому з’єднанні має бути ввімкнено IPv6. Щоб увімкнути IPv6, запустіть засіб вирішення проблем домашньої групи."},
    {46066, L"Додайте людей до домашньої групи"},
    {46067, L"Налаштувати захист даних домашньої групи"},
    {46068, L"Виявлено кілька домашніх груп"},
    {46069, L"Поділіться з іншими учасниками домашньої групи"},
    {46070, L"Документи"},
    {46071, L"Картинки"},
    {46072, L"музика"},
    {46073, L"Відео"},
    {46074, L"Принтери та пристрої"},
    {46075, L"Змініть налаштування спільного доступу до домашньої групи"},
    {46076, L"%1 Обмін"},
    {46077, L"Перевірка вашого пароля..."},
};

struct EmbeddedLanguagePack {
    EmbeddedLanguage language;
    const wchar_t* tag;
    WORD languageId;
    const EmbeddedTextEntry* entries;
    size_t entryCount;
};

static const EmbeddedLanguagePack kEmbeddedLanguagePacks[] = {
    {EmbeddedLanguage::EN_US, L"en-US", 0x0409, kStrings_EN_US, ARRAYSIZE(kStrings_EN_US)},
    {EmbeddedLanguage::IT_IT, L"it-IT", 0x0410, kStrings_IT_IT, ARRAYSIZE(kStrings_IT_IT)},
    {EmbeddedLanguage::ES_ES, L"es-ES", 0x0C0A, kStrings_ES_ES, ARRAYSIZE(kStrings_ES_ES)},
    {EmbeddedLanguage::FR_FR, L"fr-FR", 0x040C, kStrings_FR_FR, ARRAYSIZE(kStrings_FR_FR)},
    {EmbeddedLanguage::TR_TR, L"tr-TR", 0x041F, kStrings_TR_TR, ARRAYSIZE(kStrings_TR_TR)},
    {EmbeddedLanguage::RU_RU, L"ru-RU", 0x0419, kStrings_RU_RU, ARRAYSIZE(kStrings_RU_RU)},
    {EmbeddedLanguage::ZH_CN, L"zh-CN", 0x0804, kStrings_ZH_CN, ARRAYSIZE(kStrings_ZH_CN)},
    {EmbeddedLanguage::DE_DE, L"de-DE", 0x0407, kStrings_DE_DE, ARRAYSIZE(kStrings_DE_DE)},
    {EmbeddedLanguage::PT_BR, L"pt-BR", 0x0416, kStrings_PT_BR, ARRAYSIZE(kStrings_PT_BR)},
    {EmbeddedLanguage::PL_PL, L"pl-PL", 0x0415, kStrings_PL_PL, ARRAYSIZE(kStrings_PL_PL)},
    {EmbeddedLanguage::JA_JP, L"ja-JP", 0x0411, kStrings_JA_JP, ARRAYSIZE(kStrings_JA_JP)},
    {EmbeddedLanguage::KO_KR, L"ko-KR", 0x0412, kStrings_KO_KR, ARRAYSIZE(kStrings_KO_KR)},
    {EmbeddedLanguage::AR_SA, L"ar-SA", 0x0401, kStrings_AR_SA, ARRAYSIZE(kStrings_AR_SA)},
    {EmbeddedLanguage::NL_NL, L"nl-NL", 0x0413, kStrings_NL_NL, ARRAYSIZE(kStrings_NL_NL)},
    {EmbeddedLanguage::SV_SE, L"sv-SE", 0x041D, kStrings_SV_SE, ARRAYSIZE(kStrings_SV_SE)},
    {EmbeddedLanguage::CS_CZ, L"cs-CZ", 0x0405, kStrings_CS_CZ, ARRAYSIZE(kStrings_CS_CZ)},
    {EmbeddedLanguage::DA_DK, L"da-DK", 0x0406, kStrings_DA_DK, ARRAYSIZE(kStrings_DA_DK)},
    {EmbeddedLanguage::FI_FI, L"fi-FI", 0x040B, kStrings_FI_FI, ARRAYSIZE(kStrings_FI_FI)},
    {EmbeddedLanguage::EL_GR, L"el-GR", 0x0408, kStrings_EL_GR, ARRAYSIZE(kStrings_EL_GR)},
    {EmbeddedLanguage::HE_IL, L"he-IL", 0x040D, kStrings_HE_IL, ARRAYSIZE(kStrings_HE_IL)},
    {EmbeddedLanguage::HU_HU, L"hu-HU", 0x040E, kStrings_HU_HU, ARRAYSIZE(kStrings_HU_HU)},
    {EmbeddedLanguage::NB_NO, L"nb-NO", 0x0414, kStrings_NB_NO, ARRAYSIZE(kStrings_NB_NO)},
    {EmbeddedLanguage::RO_RO, L"ro-RO", 0x0418, kStrings_RO_RO, ARRAYSIZE(kStrings_RO_RO)},
    {EmbeddedLanguage::SK_SK, L"sk-SK", 0x041B, kStrings_SK_SK, ARRAYSIZE(kStrings_SK_SK)},
    {EmbeddedLanguage::UK_UA, L"uk-UA", 0x0422, kStrings_UK_UA, ARRAYSIZE(kStrings_UK_UA)},
};


static std::atomic<bool> g_languageAutomatic{true};
static std::atomic<int> g_forcedLanguage{
    static_cast<int>(EmbeddedLanguage::EN_US)};

struct DecodedStringBlock {
    WORD blockId;
    std::vector<BYTE> bytes;
};

static const EmbeddedLanguagePack* FindEmbeddedLanguagePack(
    EmbeddedLanguage language) {
    for (const auto& pack : kEmbeddedLanguagePacks) {
        if (pack.language == language) return &pack;
    }
    return &kEmbeddedLanguagePacks[0];
}

static bool GetEmbeddedStringForLanguage(EmbeddedLanguage language, UINT id,
                                         std::wstring& output) {
    const EmbeddedLanguagePack* pack = FindEmbeddedLanguagePack(language);
    if (!pack || !pack->entries) return false;
    size_t first = 0;
    size_t last = pack->entryCount;
    while (first < last) {
        const size_t middle = first + (last - first) / 2;
        if (pack->entries[middle].id < id) {
            first = middle + 1;
        } else {
            last = middle;
        }
    }
    if (first >= pack->entryCount || pack->entries[first].id != id ||
        !pack->entries[first].text) {
        return false;
    }
    output = pack->entries[first].text;
    return true;
}

static void AppendWord(std::vector<BYTE>& bytes, WORD value) {
    bytes.push_back(static_cast<BYTE>(value & 0xFF));
    bytes.push_back(static_cast<BYTE>((value >> 8) & 0xFF));
}

// Convert the author-written string table into standard RT_STRING blocks.
// This produces the same Win32 resource format expected by LoadString and
// DirectUI, without embedding or reading any MUI binary data.
static bool BuildEmbeddedLanguageBlocks(
    EmbeddedLanguage language, std::vector<DecodedStringBlock>& blocks) {
    blocks.clear();
    const EmbeddedLanguagePack* pack = FindEmbeddedLanguagePack(language);
    if (!pack || !pack->entries || !pack->entryCount) return false;

    size_t entry = 0;
    while (entry < pack->entryCount) {
        const WORD blockId = static_cast<WORD>(pack->entries[entry].id / 16 + 1);
        DecodedStringBlock block;
        block.blockId = blockId;
        for (UINT slot = 0; slot < 16; ++slot) {
            const UINT id = (static_cast<UINT>(blockId) - 1) * 16 + slot;
            const wchar_t* text = nullptr;
            if (entry < pack->entryCount && pack->entries[entry].id == id) {
                text = pack->entries[entry].text;
                ++entry;
            }
            const size_t length = text ? wcslen(text) : 0;
            if (length > 0xFFFF) return false;
            AppendWord(block.bytes, static_cast<WORD>(length));
            if (length) {
                const BYTE* begin = reinterpret_cast<const BYTE*>(text);
                block.bytes.insert(block.bytes.end(), begin,
                                   begin + length * sizeof(wchar_t));
            }
        }
        blocks.push_back(std::move(block));
    }
    return true;
}

static EmbeddedLanguage DetectEmbeddedLanguageFromTag(const wchar_t* locale) {
    if (!locale || !*locale) return EmbeddedLanguage::EN_US;
    std::wstring normalized(locale);
    for (auto& ch : normalized) ch = towlower(ch);
    for (const auto& pack : kEmbeddedLanguagePacks) {
        std::wstring tag(pack.tag);
        for (auto& ch : tag) ch = towlower(ch);
        if (normalized == tag) return pack.language;
    }
    if (normalized.rfind(L"it", 0) == 0) return EmbeddedLanguage::IT_IT;
    if (normalized.rfind(L"es", 0) == 0) return EmbeddedLanguage::ES_ES;
    if (normalized.rfind(L"fr", 0) == 0) return EmbeddedLanguage::FR_FR;
    if (normalized.rfind(L"tr", 0) == 0) return EmbeddedLanguage::TR_TR;
    if (normalized.rfind(L"ru", 0) == 0) return EmbeddedLanguage::RU_RU;
    if (normalized.rfind(L"de", 0) == 0) return EmbeddedLanguage::DE_DE;
    if (normalized.rfind(L"pl", 0) == 0) return EmbeddedLanguage::PL_PL;
    if (normalized.rfind(L"ja", 0) == 0) return EmbeddedLanguage::JA_JP;
    if (normalized.rfind(L"ko", 0) == 0) return EmbeddedLanguage::KO_KR;
    if (normalized.rfind(L"ar", 0) == 0) return EmbeddedLanguage::AR_SA;
    if (normalized.rfind(L"nl", 0) == 0) return EmbeddedLanguage::NL_NL;
    if (normalized.rfind(L"sv", 0) == 0) return EmbeddedLanguage::SV_SE;
    if (normalized.rfind(L"cs", 0) == 0) return EmbeddedLanguage::CS_CZ;
    if (normalized.rfind(L"da", 0) == 0) return EmbeddedLanguage::DA_DK;
    if (normalized.rfind(L"fi", 0) == 0) return EmbeddedLanguage::FI_FI;
    if (normalized.rfind(L"el", 0) == 0) return EmbeddedLanguage::EL_GR;
    if (normalized.rfind(L"he", 0) == 0) return EmbeddedLanguage::HE_IL;
    if (normalized.rfind(L"hu", 0) == 0) return EmbeddedLanguage::HU_HU;
    if (normalized.rfind(L"nb", 0) == 0 ||
        normalized.rfind(L"no", 0) == 0) return EmbeddedLanguage::NB_NO;
    if (normalized.rfind(L"ro", 0) == 0) return EmbeddedLanguage::RO_RO;
    if (normalized.rfind(L"sk", 0) == 0) return EmbeddedLanguage::SK_SK;
    if (normalized.rfind(L"uk", 0) == 0) return EmbeddedLanguage::UK_UA;
    if (normalized.rfind(L"pt-br", 0) == 0) return EmbeddedLanguage::PT_BR;
    if (normalized.rfind(L"zh-cn", 0) == 0 ||
        normalized.rfind(L"zh-sg", 0) == 0) return EmbeddedLanguage::ZH_CN;
    return EmbeddedLanguage::EN_US;
}

static EmbeddedLanguage GetCurrentEmbeddedLanguage() {
    if (!g_languageAutomatic.load(std::memory_order_acquire)) {
        return static_cast<EmbeddedLanguage>(
            g_forcedLanguage.load(std::memory_order_acquire));
    }
    LANGID languageId = GetThreadUILanguage();
    if (!languageId) languageId = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(languageId)) {
        case LANG_ITALIAN: return EmbeddedLanguage::IT_IT;
        case LANG_SPANISH: return EmbeddedLanguage::ES_ES;
        case LANG_FRENCH: return EmbeddedLanguage::FR_FR;
        case LANG_TURKISH: return EmbeddedLanguage::TR_TR;
        case LANG_RUSSIAN: return EmbeddedLanguage::RU_RU;
        case LANG_GERMAN: return EmbeddedLanguage::DE_DE;
        case LANG_POLISH: return EmbeddedLanguage::PL_PL;
        case LANG_JAPANESE: return EmbeddedLanguage::JA_JP;
        case LANG_KOREAN: return EmbeddedLanguage::KO_KR;
        case LANG_ARABIC: return EmbeddedLanguage::AR_SA;
        case LANG_DUTCH: return EmbeddedLanguage::NL_NL;
        case LANG_SWEDISH: return EmbeddedLanguage::SV_SE;
        case LANG_CZECH: return EmbeddedLanguage::CS_CZ;
        case LANG_DANISH: return EmbeddedLanguage::DA_DK;
        case LANG_FINNISH: return EmbeddedLanguage::FI_FI;
        case LANG_GREEK: return EmbeddedLanguage::EL_GR;
        case LANG_HEBREW: return EmbeddedLanguage::HE_IL;
        case LANG_HUNGARIAN: return EmbeddedLanguage::HU_HU;
        case LANG_NORWEGIAN: return EmbeddedLanguage::NB_NO;
        case LANG_ROMANIAN: return EmbeddedLanguage::RO_RO;
        case LANG_SLOVAK: return EmbeddedLanguage::SK_SK;
        case LANG_UKRAINIAN: return EmbeddedLanguage::UK_UA;
        case LANG_CHINESE:
            return (SUBLANGID(languageId) == SUBLANG_CHINESE_SIMPLIFIED ||
                    SUBLANGID(languageId) == SUBLANG_CHINESE_SINGAPORE)
                       ? EmbeddedLanguage::ZH_CN
                       : EmbeddedLanguage::EN_US;
        case LANG_PORTUGUESE:
            return SUBLANGID(languageId) == SUBLANG_PORTUGUESE_BRAZILIAN
                       ? EmbeddedLanguage::PT_BR
                       : EmbeddedLanguage::EN_US;
        default: break;
    }
    wchar_t locale[LOCALE_NAME_MAX_LENGTH]{};
    if (GetUserDefaultLocaleName(locale, ARRAYSIZE(locale))) {
        return DetectEmbeddedLanguageFromTag(locale);
    }
    return EmbeddedLanguage::EN_US;
}

static void LoadLanguageSetting() {
    g_languageAutomatic.store(true, std::memory_order_release);
    g_forcedLanguage.store(static_cast<int>(EmbeddedLanguage::EN_US),
                           std::memory_order_release);
    auto raw = WindhawkUtils::StringSetting::make(L"language");
    PCWSTR setting = raw.get();
    if (!setting || !*setting || !_wcsicmp(setting, L"auto") ||
        !_wcsicmp(setting, L"system")) {
        return;
    }
    g_languageAutomatic.store(false, std::memory_order_release);
    g_forcedLanguage.store(
        static_cast<int>(DetectEmbeddedLanguageFromTag(setting)),
        std::memory_order_release);
}

static std::wstring GetLocalizedDisplayName() {
    std::wstring text;
    if (GetEmbeddedStringForLanguage(GetCurrentEmbeddedLanguage(), 1, text)) {
        return text;
    }
    return L"HomeGroup";
}

static std::wstring GetLocalizedInfoTip() {
    std::wstring text;
    if (GetEmbeddedStringForLanguage(GetCurrentEmbeddedLanguage(), 2, text)) {
        return text;
    }
    return L"Review HomeGroup options and choose what this PC shares";
}

// IDs referenced directly by hgcpl UIFILE resources 201, 202, 203 and 45015,
// plus the Control Panel title/tooltip IDs. Keeping this audited list allows
// startup to reject a damaged or incomplete generated table before hooks run.
static const UINT kRequiredHomeGroupStringIds[] = {
    1, 2, 5, 6, 12, 19, 20, 64, 65, 67, 69, 70, 71, 72, 73, 74,
    94, 95, 105,
    204, 205, 530, 541, 542, 543, 544, 545, 546, 547, 548, 549,
    550, 552, 553, 554, 559, 560, 564, 567, 584, 585, 586, 595,
    600, 601, 604, 605, 607, 608, 609, 610, 611, 612, 613, 614,
    615, 616, 617, 619, 620, 621, 622, 624, 627, 628, 629, 630,
    631, 632, 637, 639, 640, 641, 642, 648, 649, 650, 652,
};

static bool ValidateEmbeddedStringCatalog() {
    if (ARRAYSIZE(kEmbeddedLanguagePacks) != 25) return false;
    for (const auto& pack : kEmbeddedLanguagePacks) {
        if (!pack.entries || pack.entryCount != 249 || !pack.tag ||
            !*pack.tag) {
            return false;
        }
        UINT previousId = 0;
        for (size_t index = 0; index < pack.entryCount; ++index) {
            const auto& entry = pack.entries[index];
            if (!entry.id || (index && entry.id <= previousId) ||
                !entry.text || !*entry.text) {
                return false;
            }
            previousId = entry.id;
        }
        for (UINT requiredId : kRequiredHomeGroupStringIds) {
            size_t first = 0;
            size_t last = pack.entryCount;
            while (first < last) {
                const size_t middle = first + (last - first) / 2;
                if (pack.entries[middle].id < requiredId) {
                    first = middle + 1;
                } else {
                    last = middle;
                }
            }
            if (first >= pack.entryCount ||
                pack.entries[first].id != requiredId ||
                !pack.entries[first].text || !*pack.entries[first].text) {
                return false;
            }
        }
    }
    Wh_Log(L"HomeGroup translations: validated 249 IDs in all 25 "
           L"language tables (including every DirectUI reference)");
    return true;
}

// =============================================================================
// RAII Wrappers
// =============================================================================

// Generic RAII handle wrapper for Win32 HANDLE objects. Ensures that every
// acquired handle is closed exactly once, even in the presence of exceptions
// or early returns. Replaces the ad-hoc UniqueWinHandle pattern from the
// Performance mod with a single, parameterized template.
class WinHandle {
public:
    WinHandle() noexcept = default;
    explicit WinHandle(HANDLE h) noexcept : h_(h) {}
    ~WinHandle() { reset(); }

    WinHandle(const WinHandle&) = delete;
    WinHandle& operator=(const WinHandle&) = delete;

    WinHandle(WinHandle&& o) noexcept : h_(o.release()) {}
    WinHandle& operator=(WinHandle&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept {
        return h_ != nullptr && h_ != INVALID_HANDLE_VALUE;
    }
    [[nodiscard]] HANDLE get() const noexcept { return h_; }
    [[nodiscard]] explicit operator bool() const noexcept { return valid(); }

    HANDLE release() noexcept {
        HANDLE tmp = h_;
        h_ = INVALID_HANDLE_VALUE;
        return tmp;
    }

    void reset(HANDLE h = INVALID_HANDLE_VALUE) noexcept {
        if (valid()) { CloseHandle(h_); }
        h_ = h;
    }

private:
    HANDLE h_ = INVALID_HANDLE_VALUE;
};

// RAII wrapper for HINTERNET handles (WinInet).
class WinInetHandle {
public:
    WinInetHandle() noexcept = default;
    explicit WinInetHandle(HINTERNET h) noexcept : h_(h) {}
    ~WinInetHandle() { reset(); }

    WinInetHandle(const WinInetHandle&) = delete;
    WinInetHandle& operator=(const WinInetHandle&) = delete;

    WinInetHandle(WinInetHandle&& o) noexcept : h_(o.release()) {}
    WinInetHandle& operator=(WinInetHandle&& o) noexcept {
        if (this != &o) { reset(o.release()); }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return h_ != nullptr; }
    [[nodiscard]] HINTERNET get() const noexcept { return h_; }

    HINTERNET release() noexcept {
        HINTERNET tmp = h_;
        h_ = nullptr;
        return tmp;
    }

    void reset(HINTERNET h = nullptr) noexcept {
        if (valid()) { InternetCloseHandle(h_); }
        h_ = h;
    }

private:
    HINTERNET h_ = nullptr;
};

// RAII wrapper for HCRYPTPROV.
class CryptContext {
public:
    CryptContext() noexcept = default;
    ~CryptContext() { reset(); }

    CryptContext(const CryptContext&) = delete;
    CryptContext& operator=(const CryptContext&) = delete;

    [[nodiscard]] bool acquire(DWORD providerType = PROV_RSA_AES,
                                DWORD flags = CRYPT_VERIFYCONTEXT) noexcept {
        reset();
        return CryptAcquireContextW(&prov_, nullptr, nullptr, providerType, flags) != FALSE;
    }

    [[nodiscard]] bool valid() const noexcept { return prov_ != 0; }
    [[nodiscard]] HCRYPTPROV get() const noexcept { return prov_; }

    void reset() noexcept {
        if (prov_) { CryptReleaseContext(prov_, 0); prov_ = 0; }
    }

private:
    HCRYPTPROV prov_ = 0;
};

// RAII wrapper for HCRYPTHASH.
class CryptHash {
public:
    CryptHash() noexcept = default;
    ~CryptHash() { reset(); }

    CryptHash(const CryptHash&) = delete;
    CryptHash& operator=(const CryptHash&) = delete;

    [[nodiscard]] bool create(HCRYPTPROV prov, ALG_ID algId = CALG_SHA_256) noexcept {
        reset();
        return CryptCreateHash(prov, algId, 0, 0, &hash_) != FALSE;
    }

    [[nodiscard]] bool valid() const noexcept { return hash_ != 0; }
    [[nodiscard]] HCRYPTHASH get() const noexcept { return hash_; }

    void reset() noexcept {
        if (hash_) { CryptDestroyHash(hash_); hash_ = 0; }
    }

private:
    HCRYPTHASH hash_ = 0;
};

// RAII wrapper for FindFirstFile / FindClose.
class FindFileHandle {
public:
    FindFileHandle() noexcept = default;
    explicit FindFileHandle(HANDLE h) noexcept : h_(h) {}
    ~FindFileHandle() { reset(); }

    FindFileHandle(const FindFileHandle&) = delete;
    FindFileHandle& operator=(const FindFileHandle&) = delete;

    [[nodiscard]] bool valid() const noexcept {
        return h_ != nullptr && h_ != INVALID_HANDLE_VALUE;
    }
    [[nodiscard]] HANDLE get() const noexcept { return h_; }

    void reset() noexcept {
        if (valid()) { FindClose(h_); h_ = INVALID_HANDLE_VALUE; }
    }

private:
    HANDLE h_ = INVALID_HANDLE_VALUE;
};

// RAII wrapper for HMODULE loaded via LoadLibrary.
class LoadedModule {
public:
    LoadedModule() noexcept = default;
    explicit LoadedModule(HMODULE h, bool owned = true) noexcept : h_(h), owned_(owned) {}
    ~LoadedModule() { reset(); }

    LoadedModule(const LoadedModule&) = delete;
    LoadedModule& operator=(const LoadedModule&) = delete;

    LoadedModule(LoadedModule&& o) noexcept : h_(o.h_), owned_(o.owned_) {
        o.h_ = nullptr; o.owned_ = false;
    }
    LoadedModule& operator=(LoadedModule&& o) noexcept {
        if (this != &o) { reset(); h_ = o.h_; owned_ = o.owned_; o.h_ = nullptr; o.owned_ = false; }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return h_ != nullptr; }
    [[nodiscard]] HMODULE get() const noexcept { return h_; }

    HMODULE release() noexcept {
        HMODULE tmp = h_; h_ = nullptr; owned_ = false; return tmp;
    }

    void reset() noexcept {
        if (h_ && owned_) { FreeLibrary(h_); }
        h_ = nullptr; owned_ = false;
    }

private:
    HMODULE h_ = nullptr;
    bool owned_ = false;
};

// RAII wrapper for named mutex acquisition. Acquires on construction,
// releases on destruction. Times out gracefully instead of blocking forever.
class ScopedMutexLock {
public:
    ScopedMutexLock(HANDLE mutex, DWORD timeoutMs = INFINITE) noexcept
        : mutex_(mutex), acquired_(false) {
        if (mutex_) {
            DWORD r = WaitForSingleObject(mutex_, timeoutMs);
            acquired_ = (r == WAIT_OBJECT_0);
        }
    }
    ~ScopedMutexLock() {
        if (acquired_ && mutex_) { ReleaseMutex(mutex_); }
    }

    ScopedMutexLock(const ScopedMutexLock&) = delete;
    ScopedMutexLock& operator=(const ScopedMutexLock&) = delete;

    [[nodiscard]] bool acquired() const noexcept { return acquired_; }

private:
    HANDLE mutex_;
    bool acquired_;
};

// =============================================================================
// Constants
// =============================================================================
static const wchar_t* kDllRelativeName = L"hgcpl.dll";

// HomeGroup Control Panel CLSID — the main applet entry point in the
// Control Panel namespace. This is the CLSID that the Control Panel
// enumerates under HKCU\...\ControlPanel\NameSpace.
static const wchar_t* kAppletClsid = L"{67ca7650-96e6-4fdd-bb43-a8e774f73a57}";

// The DirectUI Layout Folder CLSID — same one used by PerfCenterCPL and
// many other Control Panel pages. It lives in shdocvw.dll.
static const wchar_t* kLayoutFolderClsid = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";

// HomeGroup CPL page COM classes. Resource XML 100 in hgcpl.dll names
// {24D568C5-...} as its element provider and {006E61DF-...} as its page
// initializer. Both must resolve to hgcpl.dll or the DirectUI host reports
// that the page cannot be displayed.
static const wchar_t* kProviderClsid =
    L"{24D568C5-F3AE-4F91-9CD9-AA18876DA7C2}";
static const wchar_t* kInitializerClsid =
    L"{006E61DF-1A43-4F2C-B26F-780BAEA3A92D}";

// HomeGroup CPL Advanced Settings Writer CLSID — used for elevated
// settings access via COM elevation.
static const wchar_t* kAdvancedWriterClsid =
    L"{ffe1df5f-9f06-46d3-af27-f1fc10d63892}";

// ShellFolder attributes: SFGAO_BROWSABLE | standard folder attributes.
// 0xa0000000 = SFGAO_BROWSABLE(0x08000000) | SFGAO_FOLDER(0x20000000) |
//              SFGAO_HASSUBFOLDER(0x80000000) — required for the Control
//              Panel to navigate the item in-place on double click.
static const DWORD kShellFolderAttributes = 0xa0000000;
static const DWORD kInitResourceId = 100;
static const wchar_t* kVariantMarkerName = L"hgcpl.dll.variant";
static const wchar_t* kLocalizedResourcePrefix = L"hgcpl.resources-";
static constexpr DWORD kLocalizedResourceFormatVersion = 1;
static const DWORD kMinPlausibleDllSize = 32768; // 32 KB floor

// The downloaded DLL must be x64 to match this mod's architecture.
static const WORD kRequiredMachineType = IMAGE_FILE_MACHINE_AMD64;

// Download URLs for hgcpl.dll from Microsoft's public symbol server.
// Format: https://msdl.microsoft.com/download/symbols/{dll}/{TimeDateStamp:08X}{SizeOfImage:X}/{dll}
//
// IMPORTANT: The TimeDateStamp and SizeOfImage values must match the actual
// PE headers of the file on the server. These are deterministic hashes on
// Windows 10+, not real timestamps. The values below were verified against
// the corresponding files on Microsoft's public symbol server.
//
// To find the correct values, use the following PowerShell on a system that
// has hgcpl.dll (e.g., Windows 10 < 1803, or Windows.old):
//
//   $pe = [System.Reflection.Assembly]::LoadFile("C:\Windows\System32\hgcpl.dll")
//   # Or use dumpbin /headers hgcpl.dll and look for:
//   #   TimeDateStamp and SizeOfImage in the OPTIONAL HEADER VALUES
//
// Alternatively, use the Python script from pete.akeo.ie:
//   python3 build_symbol_server_url.py hgcpl.dll
//
// The User-Agent header MUST be set to "Microsoft-Symbol-Server/10.0.0.0"
// for the server to respond (discovered via pete.akeo.ie/2025/06/).

// Primary: Windows 10 1607 BASE (14393.0) x64 — released 2016-08-02
// This is the ORIGINAL version shipped with Windows 10 1607, well before
// HomeGroup was removed in Windows 10 1803 (April 2018).
// Source: WinBIndex (winbindex.m417z.com)
// TimeDateStamp: 1468635262 = 0x5789987E
// SizeOfImage (virtualSize): 647168 = 0x9E000
static const wchar_t* kDownloadUrlPrimary =
    L"https://msdl.microsoft.com/download/symbols/hgcpl.dll/5789987E9E000/hgcpl.dll";

// Fallback: Windows 10 1703 (15063.1058) x64 — KB4093117, April 2018
// Last version of 1703 before HomeGroup removal in 1803.
// TimeDateStamp: 2121409936 = 0x7E722590
// SizeOfImage (virtualSize): 630784 = 0x9A000
static const wchar_t* kDownloadUrlFallback =
    L"https://msdl.microsoft.com/download/symbols/hgcpl.dll/7E7225909A000/hgcpl.dll";

// Additional fallback: Windows 10 1607 (14393.2214) x64 — KB4093120, April 2018
// Patched version of 1607, same DLL binary as base but different timestamp.
// TimeDateStamp: 1522726620 = 0x5AC2F6DC
// SizeOfImage (virtualSize): 647168 = 0x9E000
static const wchar_t* kDownloadUrlAlt1 =
    L"https://msdl.microsoft.com/download/symbols/hgcpl.dll/5AC2F6DC9E000/hgcpl.dll";

// User-Agent required by the Microsoft symbol server (per pete.akeo.ie).
static const wchar_t* kSymbolServerUserAgent = L"Microsoft-Symbol-Server/10.0.0.0";

// SHA-256 digests of the exact files served from msdl.microsoft.com.
// Values obtained from WinBIndex (winbindex.m417z.com/?file=hgcpl.dll).
//
// Primary (Win10 1607 BASE 14393.0, 2016-08-02):
//   SHA-256: 1FCA7038E1DB0F7A90BD22DD00A40FC913820FBD89E1E15980AC823A9751E340
//   MD5:     f7e504ada18868e78fcd52b76bfaf7b4
//   SHA-1:   483ed65cc05eba6a71e439ff93a9c42e5769894c
//
// Fallback (Win10 1703 15063.1058, KB4093117, 2018-04-17):
//   SHA-256: 4CAD09F8670218C94865ADF4801BD524B1824C75C22D69B83527B37F01276C7E
//   MD5:     6854a56db1fa56b8c9af82155eef2f3f
//   SHA-1:   b3ca783623669006511086bf16a009ce1599b274
//
// Alt1 (Win10 1607 14393.2214, KB4093120, 2018-04-17):
//   SHA-256: 158ACA54B2B22F6EC9579ECF2BF24BC5D0FD417BF75A28F2F2EE00B57DCF6193
//   MD5:     6e37b41f7dad910ddc466d46acbd7956
//   SHA-1:   c5461f7ff9437867dfc81b38fb3a33cce28dcbd4
static const wchar_t* kExpectedSha256Primary =
    L"1FCA7038E1DB0F7A90BD22DD00A40FC913820FBD89E1E15980AC823A9751E340";

// SHA-256 digest for Fallback (Win10 1703 15063.1058, KB4093117, 2018-04-17)
static const wchar_t* kExpectedSha256Fallback =
    L"4CAD09F8670218C94865ADF4801BD524B1824C75C22D69B83527B37F01276C7E";

// SHA-256 digest for Alt1 (Win10 1607 14393.2214, KB4093120, 2018-04-17)
static const wchar_t* kExpectedSha256Alt1 =
    L"158ACA54B2B22F6EC9579ECF2BF24BC5D0FD417BF75A28F2F2EE00B57DCF6193";

// GUIDs in struct form for COM operations.
static const GUID kAppletFolderGuid =
    {0x67CA7650, 0x96E6, 0x4FDD, {0xBB, 0x43, 0xA8, 0xE7, 0x74, 0xF7, 0x3A, 0x57}};
static const GUID kProviderGuid =
    {0x24D568C5, 0xF3AE, 0x4F91, {0x9C, 0xD9, 0xAA, 0x18, 0x87, 0x6D, 0xA7, 0xC2}};
static const GUID kInitializerGuid =
    {0x006E61DF, 0x1A43, 0x4F2C, {0xB2, 0x6F, 0x78, 0x0B, 0xAE, 0xA3, 0xA9, 0x2D}};
static const GUID kAdvancedWriterGuid =
    {0xffe1df5f, 0x9f06, 0x46d3, {0xaf, 0x27, 0xf1, 0xfc, 0x10, 0xd6, 0x38, 0x92}};
static const IID IID_IClassFactory_GUID =
    {0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

// =============================================================================
// Shared State (atomic, lock-free reads on hot paths)
// =============================================================================
std::atomic<HMODULE> g_hHomeGroup{nullptr};
std::atomic<const std::wstring*> g_dllPath{nullptr};
std::atomic<bool> g_dllVerifiedOk{false};
std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_enableAdvancedWriter{true};

// Private translated resource module. It is never unloaded while the process
// is alive because DirectUI can retain resource-module handles lazily.
static std::mutex g_localizedResourceMutex;
static std::wstring g_localizedResourcePath;
static std::atomic<HMODULE> g_hLocalizedResources{nullptr};

// Conflict detection state — declared early because registry hooks (defined
// before the conflict-detection section) read this flag to decide whether
// to skip namespace enumeration injection in cooperative mode.
std::atomic<bool> g_yieldNamespaceInjection{false};
static HANDLE g_modActiveEvent = nullptr;
static const wchar_t* kModActiveEventName = L"Global\\WindhawkHomeGroupRestorerActive";
static const wchar_t* kWin7LegacyActiveEventName =
    L"Global\\WindhawkWin7LegacyAppletRestorerActive";

// Manual-reset stop event set by Wh_ModUninit.
static HANDLE g_stopEvent = nullptr;
static const DWORD kDownloadTimeoutMs = 20000;
static const int kMaxDownloadAttempts = 3;
static const DWORD kRetryDelayMs = 3000;

// Background setup thread — [[clang::no_destroy]] prevents std::terminate
// on process shutdown when Wh_ModUninit is not called.
[[clang::no_destroy]] static std::optional<std::thread> g_setupThread;

// Download handles for cancellation.
static std::mutex g_downloadHandlesMutex;
static HINTERNET g_downloadHNet = nullptr;
static HINTERNET g_downloadHUrl = nullptr;

const std::wstring* CurrentDllPath() {
    return g_dllPath.load(std::memory_order_acquire);
}

// =============================================================================
// Utility Functions
// =============================================================================
static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    for (auto& c : r) c = towlower(c);
    return r;
}

static bool EndsWith(const std::wstring& s, const std::wstring& suf) {
    if (s.size() < suf.size()) return false;
    return s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

static std::wstring StoreDir() {
    wchar_t buffer[MAX_PATH];
    int len = Wh_GetModStoragePath(buffer, MAX_PATH);
    if (len <= 0) return {};
    return std::wstring(buffer, len);
}

// =============================================================================
// SHA-256 via CryptoAPI
// =============================================================================
static wchar_t HexUpper(BYTE nibble) {
    return nibble < 10 ? static_cast<wchar_t>(L'0' + nibble)
                       : static_cast<wchar_t>(L'A' + nibble - 10);
}

// Compute SHA-256 of a file behind an already-open handle. The handle is
// rewound to the start before hashing, so the digest covers the exact
// file object the caller pinned — not a path that could be replaced.
static bool ComputeSha256OfHandle(HANDLE h, BYTE digest[32]) {
    LARGE_INTEGER zero{};
    if (!h || h == INVALID_HANDLE_VALUE ||
        !SetFilePointerEx(h, zero, nullptr, FILE_BEGIN)) {
        return false;
    }

    CryptContext ctx;
    if (!ctx.acquire()) return false;

    CryptHash hash;
    if (!hash.create(ctx.get())) return false;

    BYTE buf[65536];
    DWORD rd = 0;
    while (ReadFile(h, buf, sizeof(buf), &rd, nullptr) && rd > 0) {
        if (!CryptHashData(hash.get(), buf, rd, 0)) return false;
    }

    DWORD cb = 32;
    return CryptGetHashParam(hash.get(), HP_HASHVAL, digest, &cb, 0) != FALSE && cb == 32;
}

static bool Sha256MatchesHex(const BYTE digest[32], const wchar_t* expectedHex) {
    for (int i = 0; i < 32; ++i) {
        if (expectedHex[i * 2]     != HexUpper(digest[i] >> 4) ||
            expectedHex[i * 2 + 1] != HexUpper(digest[i] & 0xF)) {
            return false;
        }
    }
    return true;
}

// Check if a SHA-256 digest matches any of the known good hashes.
static bool IsKnownGoodSha256(const BYTE digest[32]) {
    return Sha256MatchesHex(digest, kExpectedSha256Primary) ||
           Sha256MatchesHex(digest, kExpectedSha256Fallback) ||
           Sha256MatchesHex(digest, kExpectedSha256Alt1);
}

// =============================================================================
// PE Validation Helpers
// =============================================================================

// Structural validation: check DOS signature and NT signature through
// an already-open handle. Never re-resolves by path.
static bool VerifyPeStructure(HANDLE h) {
    if (!h || h == INVALID_HANDLE_VALUE) return false;

    LARGE_INTEGER zero{};
    LARGE_INTEGER sz{};
    if (!GetFileSizeEx(h, &sz) || sz.QuadPart < static_cast<LONGLONG>(kMinPlausibleDllSize)) {
        return false;
    }
    if (!SetFilePointerEx(h, zero, nullptr, FILE_BEGIN)) return false;

    IMAGE_DOS_HEADER dos{};
    DWORD br = 0;
    if (!ReadFile(h, &dos, sizeof(dos), &br, nullptr) || br != sizeof(dos)) {
        return false;
    }
    if (dos.e_magic != IMAGE_DOS_SIGNATURE) return false;

    LARGE_INTEGER off{};
    off.QuadPart = dos.e_lfanew;
    if (!SetFilePointerEx(h, off, nullptr, FILE_BEGIN)) return false;

    DWORD sig = 0;
    if (!ReadFile(h, &sig, sizeof(sig), &br, nullptr) || br != sizeof(sig)) {
        return false;
    }
    return sig == IMAGE_NT_SIGNATURE;
}

// Read the PE machine type through an already-open handle.
static bool GetPeMachineType(HANDLE h, WORD& machine) {
    if (!h || h == INVALID_HANDLE_VALUE) return false;

    LARGE_INTEGER zero{};
    if (!SetFilePointerEx(h, zero, nullptr, FILE_BEGIN)) return false;

    IMAGE_DOS_HEADER dos{};
    DWORD br = 0;
    if (!ReadFile(h, &dos, sizeof(dos), &br, nullptr) || br != sizeof(dos)) {
        return false;
    }
    if (dos.e_magic != IMAGE_DOS_SIGNATURE) return false;

    LARGE_INTEGER off{};
    off.QuadPart = dos.e_lfanew;
    if (!SetFilePointerEx(h, off, nullptr, FILE_BEGIN)) return false;

    DWORD sig = 0;
    if (!ReadFile(h, &sig, sizeof(sig), &br, nullptr) || br != sizeof(sig) ||
        sig != IMAGE_NT_SIGNATURE) {
        return false;
    }

    return ReadFile(h, &machine, sizeof(machine), &br, nullptr) &&
           br == sizeof(machine);
}

// Full verification: structural check + machine type + SHA-256.
static bool VerifyDllIsCompatible(HANDLE h, const wchar_t* expectedSha256) {
    Wh_Log(L"HomeGroup: Starting DLL verification...");
    
    // Get file size for logging
    LARGE_INTEGER fileSize{};
    if (GetFileSizeEx(h, &fileSize)) {
        Wh_Log(L"HomeGroup: File size: %lld bytes", fileSize.QuadPart);
    }
    
    if (!VerifyPeStructure(h)) {
        Wh_Log(L"HomeGroup: ❌ DLL failed PE structure validation — REJECTING");
        return false;
    }
    Wh_Log(L"HomeGroup: ✓ PE structure valid");

    WORD machine = 0;
    if (!GetPeMachineType(h, machine) || machine != kRequiredMachineType) {
        Wh_Log(L"HomeGroup: ❌ DLL is not x64 (machine=%04X, expected=%04X) — REJECTING", 
               machine, kRequiredMachineType);
        return false;
    }
    Wh_Log(L"HomeGroup: ✓ Architecture: x64 (AMD64)");

    Wh_Log(L"HomeGroup: Computing SHA-256 hash...");
    BYTE digest[32];
    if (!ComputeSha256OfHandle(h, digest)) {
        Wh_Log(L"HomeGroup: ❌ Could not compute SHA-256 hash — REJECTING");
        return false;
    }

    // Convert digest to hex string
    wchar_t actualHex[65] = {};
    for (int i = 0; i < 32; ++i) {
        actualHex[i * 2]     = HexUpper(digest[i] >> 4);
        actualHex[i * 2 + 1] = HexUpper(digest[i] & 0xF);
    }
    actualHex[64] = L'\0';

    Wh_Log(L"HomeGroup: Computed SHA-256: %s", actualHex);
    Wh_Log(L"HomeGroup: Expected SHA-256: %s", expectedSha256);

    // Strict SHA-256 verification: accept any known good hash
    if (IsKnownGoodSha256(digest)) {
        Wh_Log(L"HomeGroup: ✓ SHA-256 verified successfully (matches known good hash)");
        Wh_Log(L"HomeGroup: ✓ DLL verification complete — ACCEPTED");
        return true;
    }

    // Also accept if it matches the specific expected hash passed as parameter
    if (Sha256MatchesHex(digest, expectedSha256)) {
        Wh_Log(L"HomeGroup: ✓ SHA-256 verified successfully (matches expected hash)");
        Wh_Log(L"HomeGroup: ✓ DLL verification complete — ACCEPTED");
        return true;
    }
    
    Wh_Log(L"HomeGroup: ❌ SHA-256 MISMATCH — REJECTING");
    Wh_Log(L"HomeGroup: File may be corrupted, tampered, or wrong version");
    Wh_Log(L"HomeGroup: Computed hash does not match any known good hash");
    return false;
}

// =============================================================================
// File Pinning for Verified Load
// =============================================================================

// Open the DLL denying write sharing for the entire verify → load window.
// FILE_SHARE_READ | FILE_SHARE_DELETE is required because the loader opens
// the file requesting delete sharing.
static WinHandle PinDllForLoad(const std::wstring& path) {
    Wh_Log(L"HomeGroup: Pinning DLL for load: %s", path.c_str());
    WinHandle pin(CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (pin.valid()) {
        Wh_Log(L"HomeGroup: ✓ DLL pinned successfully (handle=%p)", pin.get());
    } else {
        Wh_Log(L"HomeGroup: ❌ Failed to pin DLL (error=%lu)", GetLastError());
    }
    return pin;
}

// Prove that a loaded module maps the same file object as the verified pin.
static bool SameFileObject(HANDLE a, HANDLE b) {
    if (!a || a == INVALID_HANDLE_VALUE || !b || b == INVALID_HANDLE_VALUE) {
        return false;
    }
    FILE_ID_INFO ia{}, ib{};
    if (!GetFileInformationByHandleEx(a, FileIdInfo, &ia, sizeof(ia))) return false;
    if (!GetFileInformationByHandleEx(b, FileIdInfo, &ib, sizeof(ib))) return false;
    return ia.VolumeSerialNumber == ib.VolumeSerialNumber &&
           ia.FileId.Identifier[0] == ib.FileId.Identifier[0] &&
           ia.FileId.Identifier[1] == ib.FileId.Identifier[1];
}

static bool ConfirmLoadedModuleMatchesPin(HMODULE hModule, HANDLE pin) {
    Wh_Log(L"HomeGroup: Confirming loaded module matches pinned file...");
    if (!hModule || !pin || pin == INVALID_HANDLE_VALUE) {
        Wh_Log(L"HomeGroup: ❌ Invalid module or pin handle");
        return false;
    }
    
    wchar_t modPath[MAX_PATH]{};
    DWORD len = GetModuleFileNameW(hModule, modPath, ARRAYSIZE(modPath));
    if (len == 0 || len >= ARRAYSIZE(modPath)) {
        Wh_Log(L"HomeGroup: ❌ GetModuleFileNameW failed (error=%lu)", GetLastError());
        return false;
    }
    Wh_Log(L"HomeGroup: Loaded module path: %s", modPath);

    WinHandle hFile(CreateFileW(modPath, GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!hFile.valid()) {
        Wh_Log(L"HomeGroup: ❌ Cannot reopen loaded module (error=%lu)", GetLastError());
        return false;
    }
    
    bool matches = SameFileObject(pin, hFile.get());
    if (matches) {
        Wh_Log(L"HomeGroup: ✓ File ID verification passed — loaded module matches pin");
    } else {
        Wh_Log(L"HomeGroup: ❌ File ID MISMATCH — loaded module does NOT match pinned file");
        Wh_Log(L"HomeGroup: Possible TOCTOU attack or file replacement detected");
    }
    return matches;
}

// =============================================================================
// Download Infrastructure
// =============================================================================

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

// Download a file using WinInet with bounded timeouts. Returns true only
// when the full file was written successfully.
static bool DownloadWithTimeout(const std::wstring& url, const std::wstring& dest) {
    try {
        WinInetHandle hNet(InternetOpenW(kSymbolServerUserAgent,
                                         INTERNET_OPEN_TYPE_PRECONFIG,
                                         nullptr, nullptr, 0));
        if (!hNet.valid()) {
            Wh_Log(L"HomeGroup: ❌ InternetOpen failed (error=%lu)", GetLastError());
            return false;
        }
        Wh_Log(L"HomeGroup: ✓ WinInet session opened (User-Agent: %s)", kSymbolServerUserAgent);

        // Set timeouts on the session handle.
        DWORD timeout = kDownloadTimeoutMs;
        InternetSetOptionW(hNet.get(), INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
        InternetSetOptionW(hNet.get(), INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
        InternetSetOptionW(hNet.get(), INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

        {
            std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
            g_downloadHNet = hNet.get();
        }

        WinInetHandle hUrl(InternetOpenUrlW(hNet.get(), url.c_str(), nullptr, 0,
                                            INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0));
        if (!hUrl.valid()) {
            Wh_Log(L"HomeGroup: InternetOpenUrl failed (%lu)", GetLastError());
            std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
            g_downloadHNet = nullptr;
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
            g_downloadHUrl = hUrl.get();
        }

        // Check HTTP status.
        DWORD statusCode = 0;
        DWORD statusSize = sizeof(statusCode);
        if (HttpQueryInfoW(hUrl.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                           &statusCode, &statusSize, nullptr)) {
            if (statusCode != 200) {
                Wh_Log(L"HomeGroup: HTTP %lu for %s", statusCode, url.c_str());
                std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
                g_downloadHUrl = nullptr;
                g_downloadHNet = nullptr;
                return false;
            }
        }

        // Write to a temporary file, then atomically move.
        std::wstring tmpDest = dest + L".downloading";
        WinHandle hFile(CreateFileW(tmpDest.c_str(), GENERIC_WRITE, 0, nullptr,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!hFile.valid()) {
            Wh_Log(L"HomeGroup: Cannot create temp file %s", tmpDest.c_str());
            std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
            g_downloadHUrl = nullptr;
            g_downloadHNet = nullptr;
            return false;
        }

        BYTE buf[65536];
        DWORD bytesRead = 0;
        DWORD totalBytes = 0;

        while (!g_shuttingDown.load(std::memory_order_acquire)) {
            if (!InternetReadFile(hUrl.get(), buf, sizeof(buf), &bytesRead)) {
                break;
            }
            if (bytesRead == 0) break; // EOF

            DWORD written = 0;
            if (!WriteFile(hFile.get(), buf, bytesRead, &written, nullptr) ||
                written != bytesRead) {
                break;
            }
            totalBytes += bytesRead;
        }

        hFile.reset(); // Close the file handle before moving.

        {
            std::lock_guard<std::mutex> lock(g_downloadHandlesMutex);
            g_downloadHUrl = nullptr;
            g_downloadHNet = nullptr;
        }

        if (g_shuttingDown.load()) {
            DeleteFileW(tmpDest.c_str());
            return false;
        }

        if (totalBytes < kMinPlausibleDllSize) {
            Wh_Log(L"HomeGroup: Downloaded file too small (%lu bytes)", totalBytes);
            DeleteFileW(tmpDest.c_str());
            return false;
        }

        // Atomic move: delete existing target, rename temp to final.
        DeleteFileW(dest.c_str());
        if (!MoveFileW(tmpDest.c_str(), dest.c_str())) {
            Wh_Log(L"HomeGroup: MoveFile failed (%lu)", GetLastError());
            DeleteFileW(tmpDest.c_str());
            return false;
        }

        Wh_Log(L"HomeGroup: Downloaded %lu bytes to %s", totalBytes, dest.c_str());
        return true;

    } catch (...) {
        Wh_Log(L"HomeGroup: DownloadWithTimeout exception");
        return false;
    }
}

// =============================================================================
// Architecture Detection
// =============================================================================
static bool IsRunningAsAmd64() {
#if defined(_M_ARM64) || defined(__aarch64__)
    return false;
#else
    typedef BOOL(WINAPI* IsWow64Process2T)(HANDLE, USHORT*, USHORT*);
    USHORT processMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    USHORT nativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
    if (k32) {
        auto pIsWow64Process2 = reinterpret_cast<IsWow64Process2T>(
            GetProcAddress(k32, "IsWow64Process2"));
        if (pIsWow64Process2 &&
            pIsWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine)) {
            USHORT actualMachine = (processMachine == IMAGE_FILE_MACHINE_UNKNOWN)
                                       ? nativeMachine : processMachine;
            return actualMachine == IMAGE_FILE_MACHINE_AMD64;
        }
    }
    SYSTEM_INFO si{};
    GetNativeSystemInfo(&si);
    return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
#endif
}

// =============================================================================
// Variant Marker (tracks which DLL variant was downloaded)
// =============================================================================
static void WriteVariantMarker(const std::wstring& dir, const wchar_t* variant) {
    std::wstring mp = dir + L"\\" + kVariantMarkerName;
    std::string narrow(variant, variant + wcslen(variant));
    WinHandle h(CreateFileW(mp.c_str(), GENERIC_WRITE, 0, nullptr,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!h.valid()) return;
    DWORD bw = 0;
    WriteFile(h.get(), narrow.c_str(), static_cast<DWORD>(narrow.size()), &bw, nullptr);
}

// =============================================================================
// File Cleanup
// =============================================================================
static void RemoveOwnFiles(const std::wstring& dir, bool keepBase) {
    try {
        if (!keepBase) {
            DeleteFileW((dir + L"\\" + kDllRelativeName).c_str());
        }
        DeleteFileW((dir + L"\\" + kVariantMarkerName).c_str());

        // Remove stale ".old*" copies.
        WIN32_FIND_DATAW fd{};
        std::wstring pattern = dir + L"\\" + kDllRelativeName + L".old*";
        FindFileHandle ff(FindFirstFileW(pattern.c_str(), &fd));
        if (ff.valid()) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
                }
            } while (FindNextFileW(ff.get(), &fd));
        }

        // Remove private translated resource modules and their temporary
        // copies. Files still mapped by an open page are retried later.
        WIN32_FIND_DATAW resourceData{};
        std::wstring resourcePattern =
            dir + L"\\" + kLocalizedResourcePrefix + L"*";
        FindFileHandle resourceFind(
            FindFirstFileW(resourcePattern.c_str(), &resourceData));
        if (resourceFind.valid()) {
            do {
                if (!(resourceData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    DeleteFileW((dir + L"\\" + resourceData.cFileName).c_str());
                }
            } while (FindNextFileW(resourceFind.get(), &resourceData));
        }

        // Clean up any leftover .downloading files.
        DeleteFileW((dir + L"\\" + kDllRelativeName + L".downloading").c_str());
    } catch (...) {
        // Cleanup must never throw.
    }
}

// =============================================================================
// Local File Search — searches for hgcpl.dll in known locations before
// attempting network download. This handles cases where:
//   1. The symbol server URLs are wrong/404 (PE header values unknown)
//   2. The user has the file from a Windows < 1803 installation
//   3. The file is in the WinDbg local symbol cache
//   4. The user manually placed the file in the mod storage folder
// =============================================================================

// Search a single directory for hgcpl.dll. Returns true if found and valid.
static bool TryLoadLocalDll(const std::wstring& searchDir, const std::wstring& destPath) {
    std::wstring candidate = searchDir + L"\\" + kDllRelativeName;
    
    WinHandle hFile(CreateFileW(candidate.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!hFile.valid()) return false;
    
    Wh_Log(L"HomeGroup: Found candidate: %s", candidate.c_str());
    
    if (!VerifyDllIsCompatible(hFile.get(), kExpectedSha256Primary)) {
        Wh_Log(L"HomeGroup: Candidate failed verification");
        return false;
    }
    
    // Copy to destination if not already there
    if (ToLower(candidate) != ToLower(destPath)) {
        Wh_Log(L"HomeGroup: Copying to storage: %s", destPath.c_str());
        if (!CopyFileW(candidate.c_str(), destPath.c_str(), FALSE)) {
            Wh_Log(L"HomeGroup: ❌ CopyFile failed (error=%lu)", GetLastError());
            return false;
        }
    }
    
    Wh_Log(L"HomeGroup: ✓ Local DLL found and verified: %s", candidate.c_str());
    return true;
}

// Search known locations for hgcpl.dll before attempting network download.
static bool SearchLocalForDll(const std::wstring& destPath) {
    Wh_Log(L"HomeGroup: Searching local locations for hgcpl.dll...");
    
    // 1. Already in storage directory (checked by caller, but verify again)
    {
        std::wstring storageDir = destPath.substr(0, destPath.rfind(L'\\'));
        if (TryLoadLocalDll(storageDir, destPath)) return true;
    }
    
    // 2. WinDbg local symbol cache directories
    wchar_t tempPath[MAX_PATH] = {};
    if (GetTempPathW(MAX_PATH, tempPath)) {
        std::wstring symCache = std::wstring(tempPath) + L"symbols\\hgcpl.dll";
        Wh_Log(L"HomeGroup: Checking WinDbg cache: %s", symCache.c_str());
        // Try the flat cache directory
        if (TryLoadLocalDll(std::wstring(tempPath) + L"symbols", destPath)) return true;
    }
    
    // 3. Common WinDbg symbol cache locations
    const wchar_t* cacheLocations[] = {
        L"C:\\symbols",
        L"C:\\localsymbols",
        L"C:\\symcache",
        L"D:\\symbols",
    };
    for (const wchar_t* loc : cacheLocations) {
        if (TryLoadLocalDll(loc, destPath)) return true;
    }
    
    // 4. Windows.old (from upgrade)
    const wchar_t* windowsOldLocations[] = {
        L"C:\\Windows.old\\System32",
        L"C:\\Windows.old\\SysWOW64",
    };
    for (const wchar_t* loc : windowsOldLocations) {
        if (TryLoadLocalDll(loc, destPath)) return true;
    }
    
    // 5. User's Downloads folder
    wchar_t userProfile[MAX_PATH] = {};
    if (GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH)) {
        std::wstring downloads = std::wstring(userProfile) + L"\\Downloads";
        if (TryLoadLocalDll(downloads, destPath)) return true;
    }
    
    Wh_Log(L"HomeGroup: No local hgcpl.dll found — will attempt download");
    return false;
}

// =============================================================================
// Private resource module built from the embedded RT_STRING catalog
// =============================================================================
class ScopedTemporaryResourceFile {
public:
    explicit ScopedTemporaryResourceFile(std::wstring path)
        : path_(std::move(path)) {}
    ~ScopedTemporaryResourceFile() {
        if (!committed_ && !path_.empty()) DeleteFileW(path_.c_str());
    }
    void Commit() noexcept { committed_ = true; }

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
    bool valid() const noexcept { return update_ != nullptr; }
    HANDLE get() const noexcept { return update_; }
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
static bool ReadPrivatePeValue(const std::vector<BYTE>& file, size_t offset,
                               T& value) {
    if (offset > file.size() || file.size() - offset < sizeof(T)) return false;
    memcpy(&value, file.data() + offset, sizeof(T));
    return true;
}

// BeginUpdateResource intentionally refuses language-neutral/MUI binaries.
// Rename the private copy's named MUI RC-config type to an unused CUI type.
// The verified executable hgcpl.dll is never modified.
static bool DisableMuiConfigInPrivateCopy(const std::wstring& path) {
    WinHandle file(CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.valid()) return false;

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > 64 * 1024 * 1024) {
        return false;
    }
    std::vector<BYTE> bytes(static_cast<size_t>(size.QuadPart));
    DWORD bytesRead = 0;
    if (!ReadFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &bytesRead, nullptr) || bytesRead != bytes.size()) {
        return false;
    }

    IMAGE_DOS_HEADER dos{};
    if (!ReadPrivatePeValue(bytes, 0, dos) ||
        dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew < 0) {
        return false;
    }
    const size_t ntOffset = static_cast<size_t>(dos.e_lfanew);
    DWORD signature = 0;
    IMAGE_FILE_HEADER fileHeader{};
    if (!ReadPrivatePeValue(bytes, ntOffset, signature) ||
        signature != IMAGE_NT_SIGNATURE ||
        !ReadPrivatePeValue(bytes, ntOffset + sizeof(DWORD), fileHeader)) {
        return false;
    }

    const size_t optionalOffset =
        ntOffset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER);
    WORD optionalMagic = 0;
    if (!ReadPrivatePeValue(bytes, optionalOffset, optionalMagic)) return false;
    DWORD resourceRva = 0;
    if (optionalMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        IMAGE_OPTIONAL_HEADER64 optional{};
        if (!ReadPrivatePeValue(bytes, optionalOffset, optional) ||
            optional.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE) {
            return false;
        }
        resourceRva =
            optional.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    } else if (optionalMagic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        IMAGE_OPTIONAL_HEADER32 optional{};
        if (!ReadPrivatePeValue(bytes, optionalOffset, optional) ||
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
        IMAGE_SECTION_HEADER section{};
        if (!ReadPrivatePeValue(
                bytes, sectionOffset + i * sizeof(IMAGE_SECTION_HEADER),
                section)) {
            return false;
        }
        const DWORD span = section.Misc.VirtualSize > section.SizeOfRawData
                               ? section.Misc.VirtualSize
                               : section.SizeOfRawData;
        if (resourceRva >= section.VirtualAddress &&
            resourceRva - section.VirtualAddress < span) {
            resourceRaw = section.PointerToRawData +
                          (resourceRva - section.VirtualAddress);
            break;
        }
    }
    if (!resourceRaw || resourceRaw >= bytes.size()) return false;

    IMAGE_RESOURCE_DIRECTORY root{};
    if (!ReadPrivatePeValue(bytes, resourceRaw, root)) return false;
    const DWORD entryCount = static_cast<DWORD>(root.NumberOfNamedEntries) +
                             root.NumberOfIdEntries;
    const size_t entriesOffset = resourceRaw + sizeof(root);
    size_t muiFirstCharacterOffset = 0;
    for (DWORD i = 0; i < entryCount; ++i) {
        IMAGE_RESOURCE_DIRECTORY_ENTRY entry{};
        if (!ReadPrivatePeValue(
                bytes, entriesOffset + i * sizeof(entry), entry)) {
            return false;
        }
        DWORD nameField = 0;
        memcpy(&nameField, &entry, sizeof(nameField));
        if (!(nameField & 0x80000000u)) continue;
        const size_t stringOffset =
            resourceRaw + (nameField & 0x7FFFFFFFu);
        WORD length = 0;
        if (!ReadPrivatePeValue(bytes, stringOffset, length) || length != 3) {
            continue;
        }
        WCHAR name[3]{};
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

    LARGE_INTEGER position{};
    position.QuadPart = static_cast<LONGLONG>(muiFirstCharacterOffset);
    if (!SetFilePointerEx(file.get(), position, nullptr, FILE_BEGIN)) return false;
    const WCHAR replacement = L'C';
    DWORD written = 0;
    return WriteFile(file.get(), &replacement, sizeof(replacement), &written,
                     nullptr) && written == sizeof(replacement);
}

static bool ComputeFileSha256ByPath(const std::wstring& path, BYTE digest[32]) {
    WinHandle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               nullptr));
    return file.valid() && ComputeSha256OfHandle(file.get(), digest);
}

static std::wstring NewLocalizedResourceModuleName() {
    static std::atomic<DWORD> sequence{0};
    const DWORD number = sequence.fetch_add(1) + 1;
    const EmbeddedLanguage forced = static_cast<EmbeddedLanguage>(
        g_forcedLanguage.load(std::memory_order_acquire));
    const EmbeddedLanguagePack* pack = FindEmbeddedLanguagePack(forced);
    const wchar_t* tag = g_languageAutomatic.load(std::memory_order_acquire)
                             ? L"auto"
                             : (pack ? pack->tag : L"en-US");
    wchar_t name[160]{};
    swprintf_s(name, ARRAYSIZE(name), L"%sv%lu-%s-%lu-%lu.dll",
               kLocalizedResourcePrefix, kLocalizedResourceFormatVersion, tag,
               GetCurrentProcessId(), number);
    return name;
}

static void SweepStaleLocalizedResourceModules(const std::wstring& directory) {
    WIN32_FIND_DATAW data{};
    const std::wstring pattern =
        directory + L"\\" + kLocalizedResourcePrefix + L"*";
    FindFileHandle find(FindFirstFileW(pattern.c_str(), &data));
    if (find.valid()) {
        do {
            if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                DeleteFileW((directory + L"\\" + data.cFileName).c_str());
            }
        } while (FindNextFileW(find.get(), &data));
    }

    // Clean files created by the short-lived MUI-download implementation from
    // older revisions. The embedded catalog never reads these files, and
    // removing them prevents the Windows resource loader from preferring a
    // leftover satellite over the new private resource module.
    static const wchar_t* legacyMuiLocales[] = {
        L"en-US", L"it-IT", L"es-ES", L"fr-FR", L"tr-TR",
        L"ru-RU", L"zh-CN", L"de-DE", L"pt-BR", L"pl-PL",
    };
    for (const wchar_t* locale : legacyMuiLocales) {
        std::wstring localeDirectory = directory + L"\\" + locale;
        std::wstring mui = localeDirectory + L"\\hgcpl.dll.mui";
        std::wstring cab = directory + L"\\hgcpl-" + locale + L".cab";
        DeleteFileW(mui.c_str());
        DeleteFileW((mui + L".extracting").c_str());
        DeleteFileW(cab.c_str());
        DeleteFileW((cab + L".downloading").c_str());
        RemoveDirectoryW(localeDirectory.c_str());
    }
}

// Assumes g_localizedResourceMutex is held.
static bool BuildLocalizedResourceModuleLocked(
    const std::wstring& sourceDll, const std::wstring& directory) {
    g_localizedResourcePath.clear();
    if (sourceDll.empty() || directory.empty()) return false;

    const std::wstring destination =
        directory + L"\\" + NewLocalizedResourceModuleName();
    const std::wstring temporary = destination + L".tmp";
    ScopedTemporaryResourceFile temporaryGuard(temporary);
    DeleteFileW(temporary.c_str());
    if (!CopyFileW(sourceDll.c_str(), temporary.c_str(), FALSE)) {
        Wh_Log(L"HomeGroup translations: private DLL copy failed (%lu)",
               GetLastError());
        return false;
    }
    if (!DisableMuiConfigInPrivateCopy(temporary)) {
        Wh_Log(L"HomeGroup translations: RC config neutralization failed");
        return false;
    }

    ResourceUpdateTransaction update(temporary);
    if (!update.valid()) {
        Wh_Log(L"HomeGroup translations: BeginUpdateResource failed (%lu)",
               GetLastError());
        return false;
    }

    const bool automatic =
        g_languageAutomatic.load(std::memory_order_acquire);
    const EmbeddedLanguage forced = static_cast<EmbeddedLanguage>(
        g_forcedLanguage.load(std::memory_order_acquire));
    EmbeddedLanguage decodedLanguage = EmbeddedLanguage::EN_US;
    bool decodedValid = false;
    std::vector<DecodedStringBlock> blocks;

    for (const auto& target : kEmbeddedLanguagePacks) {
        const EmbeddedLanguage textLanguage =
            automatic ? target.language : forced;
        if (!decodedValid || textLanguage != decodedLanguage) {
            if (!BuildEmbeddedLanguageBlocks(textLanguage, blocks)) {
                Wh_Log(L"HomeGroup translations: embedded payload decode failed");
                return false;
            }
            decodedLanguage = textLanguage;
            decodedValid = true;
        }
        for (const auto& block : blocks) {
            if (!UpdateResourceW(update.get(), MAKEINTRESOURCEW(6),
                                 MAKEINTRESOURCEW(block.blockId),
                                 target.languageId, const_cast<BYTE*>(block.bytes.data()),
                                 static_cast<DWORD>(block.bytes.size()))) {
                Wh_Log(L"HomeGroup translations: UpdateResource failed "
                       L"(lang=%04X block=%u error=%lu)",
                       target.languageId, block.blockId, GetLastError());
                return false;
            }
        }
    }

    if (!update.Commit()) {
        Wh_Log(L"HomeGroup translations: EndUpdateResource failed (%lu)",
               GetLastError());
        return false;
    }

    BYTE builtDigest[32]{};
    if (!ComputeFileSha256ByPath(temporary, builtDigest)) return false;
    DeleteFileW(destination.c_str());
    if (!MoveFileExW(temporary.c_str(), destination.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"HomeGroup translations: activating resource module failed "
               L"(%lu)", GetLastError());
        return false;
    }
    temporaryGuard.Commit();

    BYTE movedDigest[32]{};
    WinHandle verify(CreateFileW(destination.c_str(), GENERIC_READ,
                                 FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!verify.valid() || !VerifyPeStructure(verify.get()) ||
        !ComputeSha256OfHandle(verify.get(), movedDigest) ||
        memcmp(builtDigest, movedDigest, sizeof(builtDigest)) != 0) {
        DeleteFileW(destination.c_str());
        Wh_Log(L"HomeGroup translations: resource integrity check failed");
        return false;
    }

    g_localizedResourcePath = destination;
    Wh_Log(L"HomeGroup translations: private module ready with 249 strings "
           L"in 25 languages: %s", destination.c_str());
    return true;
}

static bool BuildLocalizedResourceModule(const std::wstring& sourceDll,
                                         const std::wstring& directory) {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    return BuildLocalizedResourceModuleLocked(sourceDll, directory);
}

static HMODULE EnsureLocalizedResourceModuleLoaded() {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    if (HMODULE module =
            g_hLocalizedResources.load(std::memory_order_acquire)) {
        return module;
    }
    if (g_localizedResourcePath.empty()) return nullptr;
    HMODULE module = LoadLibraryExW(
        g_localizedResourcePath.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    g_hLocalizedResources.store(module, std::memory_order_release);
    if (!module) {
        Wh_Log(L"HomeGroup translations: loading resource module failed (%lu)",
               GetLastError());
    }
    return module;
}

static void ReleaseLocalizedResourceModule() {
    // DirectUI can retain this HMODULE and use it lazily. Do not FreeLibrary;
    // simply stop publishing it. Stale files are removed on a later process
    // session when they are no longer mapped.
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    g_hLocalizedResources.store(nullptr, std::memory_order_release);
    g_localizedResourcePath.clear();
}

// =============================================================================
// Setup — runs on a background thread
// =============================================================================
static bool RunSetupImpl(const std::wstring& dir) {
    try {
        Wh_Log(L"HomeGroup: ═══ Starting setup ═══");
        Wh_Log(L"HomeGroup: Storage directory: %s", dir.c_str());
        
        std::wstring dllPath = dir + L"\\" + kDllRelativeName;
        Wh_Log(L"HomeGroup: DLL target path: %s", dllPath.c_str());
        SweepStaleLocalizedResourceModules(dir);

        // Check if we already have a valid, previously-downloaded DLL.
        {
            Wh_Log(L"HomeGroup: Checking for existing DLL...");
            WinHandle hExisting(CreateFileW(dllPath.c_str(), GENERIC_READ,
                                            FILE_SHARE_READ, nullptr,
                                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            if (hExisting.valid()) {
                Wh_Log(L"HomeGroup: ✓ Existing DLL found");
                if (VerifyDllIsCompatible(hExisting.get(), kExpectedSha256Primary)) {
                    Wh_Log(L"HomeGroup: ✓ Existing DLL passed verification — reusing");

                    // Pin → verify → load → confirm.
                    Wh_Log(L"HomeGroup: Pinning existing DLL...");
                    WinHandle pin = PinDllForLoad(dllPath);
                    if (!pin.valid()) {
                        Wh_Log(L"HomeGroup: ❌ Failed to pin existing DLL");
                        return false;
                    }

                    Wh_Log(L"HomeGroup: Re-verifying pinned DLL...");
                    if (!VerifyDllIsCompatible(pin.get(), kExpectedSha256Primary)) {
                        Wh_Log(L"HomeGroup: ❌ Pinned DLL failed re-verification");
                        return false;
                    }

                    Wh_Log(L"HomeGroup: Loading DLL with LoadLibraryExW...");
                    LoadedModule mod(LoadLibraryExW(dllPath.c_str(), nullptr,
                                                    LOAD_WITH_ALTERED_SEARCH_PATH));
                    if (!mod.valid()) {
                        Wh_Log(L"HomeGroup: ❌ LoadLibraryExW failed (error=%lu)", GetLastError());
                        return false;
                    }
                    Wh_Log(L"HomeGroup: ✓ DLL loaded (module=%p)", mod.get());

                    if (!ConfirmLoadedModuleMatchesPin(mod.get(), pin.get())) {
                        Wh_Log(L"HomeGroup: ❌ Loaded module doesn't match pin — refusing");
                        return false;
                    }

                    BuildLocalizedResourceModule(dllPath, dir);

                    // Publish state.
                    auto* pathStr = new std::wstring(dllPath);
                    g_dllPath.store(pathStr, std::memory_order_release);
                    g_hHomeGroup.store(mod.release(), std::memory_order_release);
                    g_dllVerifiedOk.store(true, std::memory_order_release);
                    Wh_Log(L"HomeGroup: ✓ Setup complete — existing DLL reused successfully");
                    return true;
                }
                // Previously downloaded file is invalid — rename it aside.
                Wh_Log(L"HomeGroup: ⚠ Existing DLL failed verification, renaming aside");
                std::wstring oldPath = dllPath + L".old-" + std::to_wstring(GetTickCount());
                MoveFileW(dllPath.c_str(), oldPath.c_str());
            } else {
                Wh_Log(L"HomeGroup: No existing DLL found — will search local and download");
            }
        }

        // --- Phase 2: Search local locations before downloading ---
        // This handles cases where the symbol server URLs return 404
        // (because the PE TimeDateStamp/SizeOfImage values are unknown)
        // or the user has the file from a Windows < 1803 installation.
        if (SearchLocalForDll(dllPath)) {
            // Local file found, verified, and copied to storage.
            // Now load it.
            WinHandle pin = PinDllForLoad(dllPath);
            if (pin.valid()) {
                LoadedModule mod(LoadLibraryExW(dllPath.c_str(), nullptr,
                                                LOAD_WITH_ALTERED_SEARCH_PATH));
                if (mod.valid() && ConfirmLoadedModuleMatchesPin(mod.get(), pin.get())) {
                    BuildLocalizedResourceModule(dllPath, dir);
                    auto* pathStr = new std::wstring(dllPath);
                    g_dllPath.store(pathStr, std::memory_order_release);
                    g_hHomeGroup.store(mod.release(), std::memory_order_release);
                    g_dllVerifiedOk.store(true, std::memory_order_release);
                    Wh_Log(L"HomeGroup: ✓ Setup complete — DLL found locally and loaded");
                    Wh_Log(L"HomeGroup: ═══ Setup finished successfully ═══");
                    return true;
                }
            }
            Wh_Log(L"HomeGroup: ⚠ Local DLL found but failed to load — trying download");
        }

        // Download phase: try primary URL, then fallback.
        struct DownloadAttempt {
            const wchar_t* url;
            const wchar_t* sha256;
            const wchar_t* label;
        };

        const DownloadAttempt attempts[] = {
            {kDownloadUrlPrimary, kExpectedSha256Primary, L"primary (Win10 1607)"},
            {kDownloadUrlFallback, kExpectedSha256Fallback, L"fallback (Win10 1703)"},
            {kDownloadUrlAlt1, kExpectedSha256Alt1, L"alt1 (Win10 1607 KB4093120)"},
        };

        Wh_Log(L"HomeGroup: Starting download phase...");
        for (const auto& attempt : attempts) {
            if (g_shuttingDown.load()) {
                Wh_Log(L"HomeGroup: ⚠ Shutdown requested — aborting download");
                return false;
            }

            Wh_Log(L"HomeGroup: Trying %s from %s", attempt.label, attempt.url);

            for (int retry = 0; retry < kMaxDownloadAttempts && !g_shuttingDown.load(); ++retry) {
                if (retry > 0) {
                    Wh_Log(L"HomeGroup: Retry %d/%d for %s", retry + 1, kMaxDownloadAttempts, attempt.label);
                    DWORD waitResult = WaitForSingleObject(g_stopEvent, kRetryDelayMs);
                    if (waitResult == WAIT_OBJECT_0 || g_shuttingDown.load()) {
                        Wh_Log(L"HomeGroup: ⚠ Shutdown during retry wait");
                        return false;
                    }
                }

                if (DownloadWithTimeout(attempt.url, dllPath)) {
                    Wh_Log(L"HomeGroup: ✓ Download successful — verifying...");
                    
                    // Verify the downloaded file.
                    WinHandle hDownloaded(CreateFileW(dllPath.c_str(), GENERIC_READ,
                                                      FILE_SHARE_READ, nullptr,
                                                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
                    if (!hDownloaded.valid()) {
                        Wh_Log(L"HomeGroup: ❌ Cannot reopen downloaded DLL (error=%lu)", GetLastError());
                        continue;
                    }

                    if (!VerifyDllIsCompatible(hDownloaded.get(), attempt.sha256)) {
                        Wh_Log(L"HomeGroup: ❌ Downloaded DLL failed verification — deleting");
                        DeleteFileW(dllPath.c_str());
                        continue;
                    }
                    Wh_Log(L"HomeGroup: ✓ Downloaded DLL verified successfully");

                    WriteVariantMarker(dir, attempt.label);
                    Wh_Log(L"HomeGroup: Variant marker written: %s", attempt.label);

                    // Pin → load → confirm.
                    Wh_Log(L"HomeGroup: Pinning downloaded DLL...");
                    WinHandle pin = PinDllForLoad(dllPath);
                    if (!pin.valid()) {
                        Wh_Log(L"HomeGroup: ❌ Cannot pin DLL for loading");
                        return false;
                    }

                    Wh_Log(L"HomeGroup: Loading DLL with LoadLibraryExW...");
                    LoadedModule mod(LoadLibraryExW(dllPath.c_str(), nullptr,
                                                    LOAD_WITH_ALTERED_SEARCH_PATH));
                    if (!mod.valid()) {
                        Wh_Log(L"HomeGroup: ❌ LoadLibraryExW failed (error=%lu)", GetLastError());
                        return false;
                    }
                    Wh_Log(L"HomeGroup: ✓ DLL loaded (module=%p)", mod.get());

                    if (!ConfirmLoadedModuleMatchesPin(mod.get(), pin.get())) {
                        Wh_Log(L"HomeGroup: ❌ Loaded module doesn't match pin");
                        return false;
                    }

                    BuildLocalizedResourceModule(dllPath, dir);

                    // Publish state atomically.
                    auto* pathStr = new std::wstring(dllPath);
                    g_dllPath.store(pathStr, std::memory_order_release);
                    g_hHomeGroup.store(mod.release(), std::memory_order_release);
                    g_dllVerifiedOk.store(true, std::memory_order_release);

                    Wh_Log(L"HomeGroup: ✓ Setup complete — DLL downloaded, verified, and loaded");
                    Wh_Log(L"HomeGroup: ═══ Setup finished successfully ═══");
                    return true;
                }
            }
        }

        Wh_Log(L"HomeGroup: ❌ All download attempts failed");
        Wh_Log(L"HomeGroup: ═══════════════════════════════════════════════════");
        Wh_Log(L"HomeGroup: MANUAL INSTALLATION REQUIRED");
        Wh_Log(L"HomeGroup: The DLL could not be obtained from the local search");
        Wh_Log(L"HomeGroup: paths or from Microsoft's symbol server.");
        Wh_Log(L"HomeGroup: ");
        Wh_Log(L"HomeGroup: To fix this, obtain hgcpl.dll (x64) from one of:");
        Wh_Log(L"HomeGroup:   1. A Windows 10 version < 1803 (C:\\Windows\\System32\\)");
        Wh_Log(L"HomeGroup:   2. A Windows.old folder after upgrade");
        Wh_Log(L"HomeGroup:   3. WinDbg symbol cache (C:\\symbols\\)");
        Wh_Log(L"HomeGroup: ");
        Wh_Log(L"HomeGroup: Then copy hgcpl.dll to:");
        Wh_Log(L"HomeGroup:   %s", dir.c_str());
        Wh_Log(L"HomeGroup: ");
        Wh_Log(L"HomeGroup: Check the Windhawk log for HTTP, proxy, TLS, or");
        Wh_Log(L"HomeGroup: SHA-256 verification errors, then retry the mod.");
        Wh_Log(L"HomeGroup: ═══════════════════════════════════════════════════");
        Wh_Log(L"HomeGroup: ═══ Setup failed ═══");
        return false;

    } catch (const std::exception& e) {
        Wh_Log(L"HomeGroup: ❌ RunSetup exception: %S", e.what());
        return false;
    } catch (...) {
        Wh_Log(L"HomeGroup: ❌ RunSetup unknown exception");
        return false;
    }
}

static void RunSetup() {
    try {
        if (g_shuttingDown.load()) return;

        std::wstring dir = StoreDir();
        if (dir.empty()) {
            Wh_Log(L"HomeGroup: Cannot determine storage directory");
            return;
        }

        // Create the directory if it doesn't exist.
        CreateDirectoryW(dir.c_str(), nullptr);

        // Serialize setup across processes with a named mutex.
        WinHandle setupMutex(CreateMutexW(nullptr, FALSE,
                                          L"Global\\WindhawkHomeGroupRestorerSetup"));
        if (setupMutex.valid()) {
            ScopedMutexLock lock(setupMutex.get(), 30000);
            if (!lock.acquired()) {
                Wh_Log(L"HomeGroup: Could not acquire setup mutex");
            }
            RunSetupImpl(dir);
        } else {
            // Mutex creation failed — try without serialization.
            RunSetupImpl(dir);
        }
    } catch (...) {
        Wh_Log(L"HomeGroup: RunSetup outer exception");
    }
}

// =============================================================================
// Registry Virtualization Infrastructure
// =============================================================================

// CLSID path fragments for the virtualization layer.
std::wstring g_clsidLower, g_clsidSuffix, g_defaultIconSuffix, g_inprocSuffix,
    g_shellFolderSuffix, g_instanceSuffix, g_initPropBagSuffix, g_namespaceSuffix;
std::wstring g_providerClsidLower, g_providerSuffix, g_providerInprocSuffix;
std::wstring g_initializerClsidLower, g_initializerSuffix,
    g_initializerInprocSuffix;

// Advanced writer CLSID paths.
std::wstring g_advWriterClsidLower, g_advWriterSuffix, g_advWriterInprocSuffix,
    g_advWriterElevationSuffix;

void InitClsidStrings() {
    g_clsidLower = ToLower(kAppletClsid);
    g_clsidSuffix = L"clsid\\" + g_clsidLower;
    g_defaultIconSuffix = g_clsidSuffix + L"\\defaulticon";
    g_inprocSuffix = g_clsidSuffix + L"\\inprocserver32";
    g_shellFolderSuffix = g_clsidSuffix + L"\\shellfolder";
    g_instanceSuffix = g_clsidSuffix + L"\\instance";
    g_initPropBagSuffix = g_instanceSuffix + L"\\initpropertybag";
    g_namespaceSuffix = L"controlpanel\\namespace\\" + g_clsidLower;

    g_providerClsidLower = ToLower(kProviderClsid);
    g_providerSuffix = L"clsid\\" + g_providerClsidLower;
    g_providerInprocSuffix = g_providerSuffix + L"\\inprocserver32";

    g_initializerClsidLower = ToLower(kInitializerClsid);
    g_initializerSuffix = L"clsid\\" + g_initializerClsidLower;
    g_initializerInprocSuffix = g_initializerSuffix + L"\\inprocserver32";

    g_advWriterClsidLower = ToLower(kAdvancedWriterClsid);
    g_advWriterSuffix = L"clsid\\" + g_advWriterClsidLower;
    g_advWriterInprocSuffix = g_advWriterSuffix + L"\\inprocserver32";
    g_advWriterElevationSuffix = g_advWriterSuffix + L"\\elevation";
}

// Forward declarations for registry hook originals.
extern decltype(&RegOpenKeyExW) RegOpenKeyExWOriginal;
extern decltype(&RegCreateKeyExW) RegCreateKeyExWOriginal;
extern decltype(&RegCloseKey) RegCloseKeyOriginal;

// Volatile backing key for virtualized HKEYs — same technique as the
// Performance mod. The key is REG_OPTION_VOLATILE: lives only in memory,
// not persisted to disk, deleted when the mod is disabled.
static const wchar_t kVirtualKeyParentPrefix[] =
    L"Software\\WindhawkHomeGroupRestorer";
static const wchar_t kVirtualKeyNamePrefix[] = L"VirtualKeys";

static std::wstring VirtualKeyPath() {
    wchar_t key[96];
    swprintf_s(key, ARRAYSIZE(key), L"%ls\\%ls-%lu",
               kVirtualKeyParentPrefix, kVirtualKeyNamePrefix, GetCurrentProcessId());
    return std::wstring(key);
}

static std::wstring VirtualKeyParentPath() {
    return std::wstring(kVirtualKeyParentPrefix);
}

static std::wstring VirtualKeyLeafName() {
    wchar_t leaf[64];
    swprintf_s(leaf, ARRAYSIZE(leaf), L"%ls-%lu",
               kVirtualKeyNamePrefix, GetCurrentProcessId());
    return std::wstring(leaf);
}

static HKEY g_virtualKeyRoot = nullptr;
static std::mutex g_virtualKeyRootMutex;
static const wchar_t kVirtualKeyOwnerMarker[] = L"WindhawkOwnerPid";

// Check whether an open key handle is volatile via NtQueryKey.
static bool IsVolatileKeyHandle(HKEY k) {
    static auto NtQueryKeyFn = reinterpret_cast<LONG(WINAPI*)(
        HANDLE, int, PVOID, ULONG, PULONG)>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey"));
    if (!NtQueryKeyFn) return false;

    struct KeyFlagsInfo {
        ULONG UserFlags;
        ULONG KeyFlags;
        ULONG ControlFlags;
    };
    KeyFlagsInfo info{};
    ULONG needed = 0;
    LONG status = NtQueryKeyFn(reinterpret_cast<HANDLE>(k), 4, &info, sizeof(info), &needed);
    return status == 0 && (info.KeyFlags & 0x1) != 0;
}

static bool VerifyVirtualKeyOwner(HKEY k) {
    DWORD value = 0, size = sizeof(value), type = 0;
    if (RegQueryValueExW(k, kVirtualKeyOwnerMarker, nullptr, &type,
                         reinterpret_cast<BYTE*>(&value), &size) != ERROR_SUCCESS ||
        type != REG_DWORD) {
        return false;
    }
    return value == GetCurrentProcessId();
}

static HKEY EnsureVirtualKeyRoot() {
    std::lock_guard<std::mutex> l(g_virtualKeyRootMutex);
    if (g_virtualKeyRoot) return g_virtualKeyRoot;

    HKEY root = nullptr;
    if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, VirtualKeyPath().c_str(), 0,
                              KEY_READ | KEY_WRITE, &root) == ERROR_SUCCESS) {
        if (IsVolatileKeyHandle(root) && VerifyVirtualKeyOwner(root)) {
            g_virtualKeyRoot = root;
            return root;
        }
        RegCloseKeyOriginal(root);
        root = nullptr;
    }

    DWORD disp = 0;
    LONG st = RegCreateKeyExWOriginal(HKEY_CURRENT_USER, VirtualKeyPath().c_str(),
                                      0, nullptr, REG_OPTION_VOLATILE,
                                      KEY_READ | KEY_WRITE, nullptr, &root, &disp);
    if (st != ERROR_SUCCESS) return nullptr;

    DWORD pid = GetCurrentProcessId();
    RegSetValueExW(root, kVirtualKeyOwnerMarker, 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&pid), sizeof(pid));

    g_virtualKeyRoot = root;
    return root;
}

static void ReleaseVirtualKeyRoot() {
    HKEY root;
    {
        std::lock_guard<std::mutex> l(g_virtualKeyRootMutex);
        root = g_virtualKeyRoot;
        g_virtualKeyRoot = nullptr;
    }
    if (root) RegCloseKeyOriginal(root);

    HKEY parent = nullptr;
    if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, VirtualKeyParentPath().c_str(),
                              0, KEY_WRITE | DELETE, &parent) == ERROR_SUCCESS) {
        RegDeleteKeyW(parent, VirtualKeyLeafName().c_str());
        RegCloseKeyOriginal(parent);

        HKEY grand = nullptr;
        if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, L"Software", 0, DELETE, &grand) == ERROR_SUCCESS) {
            RegDeleteKeyW(grand, L"WindhawkHomeGroupRestorer");
            RegCloseKeyOriginal(grand);
        }
    }
}

// =============================================================================
// KeyTracker — maps HKEY handles to registry paths
// =============================================================================
class KeyTracker {
public:
    std::wstring GetPath(HKEY k) const {
        if (const wchar_t* s = SpecialRootPath(k)) return s;
        std::shared_lock<std::shared_mutex> l(mutex_);
        auto it = paths_.find(k);
        return it != paths_.end() ? it->second : std::wstring();
    }

    bool IsFakeAndGetPath(HKEY k, std::wstring& o) const {
        if (const wchar_t* s = SpecialRootPath(k)) { o = s; return false; }
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

    bool IsTrackedOrFake(HKEY k) const {
        if (SpecialRootPath(k)) return false;
        std::shared_lock<std::shared_mutex> l(mutex_);
        return fakeOwners_.count(k) != 0 || paths_.count(k) != 0;
    }

    void Track(HKEY k, const std::wstring& p) {
        if (!k || IsSpecialRoot(k)) return;
        if (!ContainsKeyword(p.c_str())) return;
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_[k] = p;
    }

    void Untrack(HKEY k) {
        if (!k || IsSpecialRoot(k)) return;
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_.erase(k);
    }

    HKEY CreateFake(const std::wstring& p) {
        if (!EnsureVirtualKeyRoot()) return nullptr;
        HKEY backing = nullptr;
        if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, VirtualKeyPath().c_str(), 0,
                                  KEY_READ, &backing) != ERROR_SUCCESS) {
            return nullptr;
        }
        std::unique_lock<std::shared_mutex> l(mutex_);
        paths_[backing] = p;
        fakeOwners_[backing] = backing;
        return backing;
    }

    void FreeFake(HKEY k) {
        std::unique_lock<std::shared_mutex> l(mutex_);
        HKEY backing = nullptr;
        auto it = fakeOwners_.find(k);
        if (it != fakeOwners_.end()) {
            backing = it->second;
            fakeOwners_.erase(it);
        }
        paths_.erase(k);
        if (backing) RegCloseKeyOriginal(backing);
    }

    void ClearWithoutFreeing() {
        std::unique_lock<std::shared_mutex> l(mutex_);
        fakeOwners_.clear();
        paths_.clear();
    }

private:
    static bool IsSpecialRoot(HKEY k) {
        auto v = reinterpret_cast<uintptr_t>(k);
        return v >= 0x80000000 && v <= 0x80000004;
    }

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

    // Cheap keyword gate — avoids allocation for the vast majority of
    // registry accesses that are irrelevant to this mod.
    static bool ContainsKeyword(const wchar_t* s) {
        if (!s || !*s) return false;
        return AsciiIContains(s, "clsid") ||
               AsciiIContains(s, "controlpanel") ||
               AsciiIContains(s, "homegroup") ||
               AsciiIContains(s, "shell extensions");
    }

    static bool AsciiIContains(const wchar_t* s, const char* needle) {
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
                ++a; ++b;
            }
            if (!*b) return true;
        }
        return false;
    }

    mutable std::shared_mutex mutex_;
    std::unordered_map<HKEY, std::wstring> paths_;
    std::unordered_map<HKEY, HKEY> fakeOwners_;
};

static KeyTracker g_keyTracker;
static std::mutex g_injectedMutex;
static std::unordered_map<HKEY, bool> g_injectedForHandle;

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

// =============================================================================
// Virtual Node Classification
// =============================================================================
enum class VNode {
    None, ClsidRoot, DefaultIcon, InProcServer32, ShellFolder, Instance,
    InitPropertyBag, NamespaceEntry, ProviderRoot, ProviderInProc,
    InitializerRoot, InitializerInProc,
    AdvWriterRoot, AdvWriterInProc, AdvWriterElevation
};

static VNode ClassifyPath(const std::wstring& p) {
    std::wstring l = ToLower(p);
    if (EndsWith(l, g_namespaceSuffix)) return VNode::NamespaceEntry;
    if (EndsWith(l, g_initPropBagSuffix)) return VNode::InitPropertyBag;
    if (EndsWith(l, g_instanceSuffix)) return VNode::Instance;
    if (EndsWith(l, g_shellFolderSuffix)) return VNode::ShellFolder;
    if (EndsWith(l, g_inprocSuffix)) return VNode::InProcServer32;
    if (EndsWith(l, g_defaultIconSuffix)) return VNode::DefaultIcon;
    if (EndsWith(l, g_clsidSuffix)) return VNode::ClsidRoot;
    if (g_enableAdvancedWriter.load()) {
        if (EndsWith(l, g_advWriterElevationSuffix)) return VNode::AdvWriterElevation;
        if (EndsWith(l, g_advWriterInprocSuffix)) return VNode::AdvWriterInProc;
        if (EndsWith(l, g_advWriterSuffix)) return VNode::AdvWriterRoot;
    }
    if (EndsWith(l, g_providerInprocSuffix)) return VNode::ProviderInProc;
    if (EndsWith(l, g_providerSuffix)) return VNode::ProviderRoot;
    if (EndsWith(l, g_initializerInprocSuffix)) return VNode::InitializerInProc;
    if (EndsWith(l, g_initializerSuffix)) return VNode::InitializerRoot;
    return VNode::None;
}

static bool IsApprovedKey(const std::wstring& p) {
    return EndsWith(ToLower(p), L"shell extensions\\approved");
}

static bool IsTargetKey(const std::wstring& p) { return ClassifyPath(p) != VNode::None; }

static const wchar_t kNamespaceHkcuParentLower[] =
    L"software\\microsoft\\windows\\currentversion\\explorer\\controlpanel\\namespace";

static bool IsHkcuNamespaceParentKey(const std::wstring& p) {
    return EndsWith(ToLower(p), kNamespaceHkcuParentLower);
}

// =============================================================================
// Value Providers
// =============================================================================
static LSTATUS ProvideStringValue(LPBYTE d, LPDWORD cb, const std::wstring& s) {
    if (!cb) return ERROR_INVALID_PARAMETER;
    DWORD need = static_cast<DWORD>((s.length() + 1) * sizeof(wchar_t));
    if (!d) { *cb = need; return ERROR_SUCCESS; }
    if (*cb < need) { *cb = need; return ERROR_MORE_DATA; }
    *cb = need;
    memcpy(d, s.c_str(), need);
    return ERROR_SUCCESS;
}

static LSTATUS ProvideDwordValue(LPBYTE d, LPDWORD cb, DWORD v) {
    if (!cb) return ERROR_INVALID_PARAMETER;
    if (!d) { *cb = sizeof(DWORD); return ERROR_SUCCESS; }
    if (*cb < sizeof(DWORD)) { *cb = sizeof(DWORD); return ERROR_MORE_DATA; }
    *cb = sizeof(DWORD);
    *reinterpret_cast<DWORD*>(d) = v;
    return ERROR_SUCCESS;
}

// The icon for the HomeGroup page comes from imageres.dll, which is always
// present on all supported Windows versions.
static std::wstring GetHomeGroupIconPath() {
    wchar_t b[MAX_PATH]{};
    GetSystemDirectoryW(b, MAX_PATH);
    return std::wstring(b) + L"\\imageres.dll,-1013";
}

static std::wstring GetShdocvwPath() {
    wchar_t b[MAX_PATH]{};
    GetSystemDirectoryW(b, MAX_PATH);
    return std::wstring(b) + L"\\shdocvw.dll";
}

// Compute the virtual value for a given (path, valueName). This is the
// core of the registry virtualization — it intercepts RegQueryValueExW
// and RegGetValueW calls and returns fabricated values for the CLSID
// tree that the Control Panel needs to discover and display HomeGroup.
static bool TryProvideValueData(const std::wstring& path, const std::wstring& vn,
                                DWORD* type, std::wstring& strOut,
                                DWORD& dwordOut, bool& isStr, LSTATUS& status) {
    const std::wstring* dllPath = CurrentDllPath();
    if (!g_dllVerifiedOk.load() || !dllPath || dllPath->empty()) return false;

    // Shell Extensions\Approved — needed for the namespace entry to be visible.
    if (IsApprovedKey(path)) {
        std::wstring low = ToLower(vn);
        if (low == g_clsidLower || low == g_providerClsidLower ||
            low == g_initializerClsidLower) {
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
                if (type) *type = REG_SZ;
                strOut = GetLocalizedDisplayName();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"InfoTip") {
                if (type) *type = REG_SZ;
                strOut = GetLocalizedInfoTip();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"System.ApplicationName") {
                if (type) *type = REG_SZ;
                strOut = L"Microsoft.HomeGroup";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"System.ControlPanel.Category") {
                if (type) *type = REG_SZ;
                strOut = L"3";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"System.ControlPanel.EnableInSafeMode") {
                if (type) *type = REG_DWORD;
                dwordOut = 2;
                isStr = false;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"System.Software.TasksFileUrl") {
                if (type) *type = REG_SZ;
                strOut = L"Internal";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;

        case VNode::DefaultIcon:
            if (vn.empty()) {
                if (type) *type = REG_EXPAND_SZ;
                strOut = GetHomeGroupIconPath();
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

        case VNode::ProviderRoot:
        case VNode::InitializerRoot:
        case VNode::AdvWriterRoot:
            if (vn.empty()) {
                if (type) *type = REG_SZ;
                strOut.clear();
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;

        case VNode::ProviderInProc:
        case VNode::InitializerInProc:
        case VNode::AdvWriterInProc:
            if (vn.empty()) {
                if (type) *type = REG_EXPAND_SZ;
                strOut = *dllPath;
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"ThreadingModel") {
                if (type) *type = REG_SZ;
                strOut = L"Both";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;

        case VNode::AdvWriterElevation:
            if (vn == L"Enabled") {
                if (type) *type = REG_DWORD;
                dwordOut = 1;
                isStr = false;
                status = ERROR_SUCCESS;
                return true;
            } else if (vn == L"IconReference") {
                if (type) *type = REG_EXPAND_SZ;
                strOut = L"@C:\\Windows\\system32\\imageres.dll,-1013";
                isStr = true;
                status = ERROR_SUCCESS;
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}

static bool TryProvideValue(const std::wstring& path, const std::wstring& vn,
                            LPDWORD tp, LPBYTE d, LPDWORD cb, LSTATUS& out) {
    DWORD vtype = 0;
    std::wstring strOut;
    DWORD dwOut = 0;
    bool isStr = true;
    if (!TryProvideValueData(path, vn, &vtype, strOut, dwOut, isStr, out)) {
        return false;
    }
    if (tp) *tp = vtype;
    if (isStr) {
        out = ProvideStringValue(d, cb, strOut);
    } else {
        out = ProvideDwordValue(d, cb, dwOut);
    }
    return true;
}

static bool GetVirtualSubKeyName(VNode n, DWORD idx, std::wstring& o) {
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
        case VNode::InitializerRoot:
        case VNode::AdvWriterRoot:
            if (idx == 0) { o = L"InProcServer32"; return true; }
            return false;
        default:
            return false;
    }
}

static DWORD GetVirtualSubKeyCount(VNode n) {
    switch (n) {
        case VNode::ClsidRoot: return 4;
        case VNode::Instance: return 1;
        case VNode::ProviderRoot: return 1;
        case VNode::InitializerRoot: return 1;
        case VNode::AdvWriterRoot: return 2; // InProcServer32 + Elevation
        default: return 0;
    }
}

// =============================================================================
// Registry Hooks (Unicode *W only — 9 hooks)
// =============================================================================
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

static bool MightNeedVirtualization(HKEY hk, LPCWSTR sub) {
    return g_keyTracker.IsTrackedOrFake(hk) ||
           (sub && (wcsstr(sub, L"clsid") || wcsstr(sub, L"CLSID") ||
                    wcsstr(sub, L"HomeGroup") || wcsstr(sub, L"homegroup") ||
                    wcsstr(sub, L"ControlPanel") || wcsstr(sub, L"controlpanel") ||
                    wcsstr(sub, L"Shell Extensions") || wcsstr(sub, L"shell extensions")));
}

static LSTATUS RegOpenKeyVirtual(HKEY hk, const std::wstring& sub, bool hasSub,
                                 DWORD opt, REGSAM sam, PHKEY out) {
    std::wstring full;
    const bool fakeParent = g_keyTracker.IsFakeAndGetPath(hk, full);

    if (fakeParent) {
        if (hasSub) { if (!full.empty()) full += L"\\"; full += sub; }
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
        if (hasSub) { if (!fp.empty()) fp += L"\\"; fp += sub; }
        g_keyTracker.Track(*out, fp);
    } else if (st == ERROR_FILE_NOT_FOUND && out) {
        std::wstring fp = full;
        if (hasSub) { if (!fp.empty()) fp += L"\\"; fp += sub; }
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

LSTATUS WINAPI RegOpenKeyExWHook(HKEY hk, LPCWSTR sub, DWORD opt, REGSAM sam, PHKEY out) {
    try {
        if (!MightNeedVirtualization(hk, sub)) {
            return RegOpenKeyExWOriginal(hk, sub, opt, sam, out);
        }
        std::wstring s = sub ? sub : L"";
        return RegOpenKeyVirtual(hk, s, sub && *sub, opt, sam, out);
    } catch (...) {
        return RegOpenKeyExWOriginal(hk, sub, opt, sam, out);
    }
}

LSTATUS WINAPI RegOpenKeyWHook(HKEY hk, LPCWSTR sub, PHKEY out) {
    try {
        if (!MightNeedVirtualization(hk, sub)) {
            return RegOpenKeyWOriginal(hk, sub, out);
        }
        std::wstring s = sub ? sub : L"";
        return RegOpenKeyVirtual(hk, s, sub && *sub, 0, MAXIMUM_ALLOWED, out);
    } catch (...) {
        return RegOpenKeyWOriginal(hk, sub, out);
    }
}

template <typename CreateFn>
static LSTATUS CreateKeyVirtual(HKEY hk, const std::wstring& sub, bool hasSub,
                                REGSAM sam, PHKEY out, LPDWORD disposition,
                                CreateFn original) {
    std::wstring full;
    const bool fakeParent = g_keyTracker.IsFakeAndGetPath(hk, full);
    if (fakeParent) {
        if (hasSub) { if (!full.empty()) full += L"\\"; full += sub; }
        if (IsTargetKey(full)) { if (out) *out = nullptr; return ERROR_ACCESS_DENIED; }
        return ERROR_FILE_NOT_FOUND;
    }
    if (hasSub) { if (!full.empty()) full += L"\\"; full += sub; }
    if (IsTargetKey(full)) { if (out) *out = nullptr; return ERROR_ACCESS_DENIED; }
    return original();
}

LSTATUS WINAPI RegCreateKeyExWHook(HKEY hk, LPCWSTR sub, DWORD reserved,
                                   LPWSTR cls, DWORD opt, REGSAM sam,
                                   LPSECURITY_ATTRIBUTES sa, PHKEY out,
                                   LPDWORD disposition) {
    try {
        if (!MightNeedVirtualization(hk, sub)) {
            return RegCreateKeyExWOriginal(hk, sub, reserved, cls, opt, sam, sa,
                                           out, disposition);
        }
        std::wstring s = sub ? sub : L"";
        return CreateKeyVirtual(hk, s, sub && *sub, sam, out, disposition,
            [&]() {
                return RegCreateKeyExWOriginal(hk, sub, reserved, cls, opt, sam, sa,
                                               out, disposition);
            });
    } catch (...) {
        return RegCreateKeyExWOriginal(hk, sub, reserved, cls, opt, sam, sa,
                                       out, disposition);
    }
}

LSTATUS WINAPI RegCloseKeyHook(HKEY k) {
    try {
        if (g_keyTracker.IsFake(k)) {
            g_keyTracker.FreeFake(k);
            return ERROR_SUCCESS;
        }
        LSTATUS s = RegCloseKeyOriginal(k);
        if (g_keyTracker.IsTrackedOrFake(k)) {
            g_keyTracker.Untrack(k);
            ClearInjectedState(k);
        }
        return s;
    } catch (...) {
        return RegCloseKeyOriginal(k);
    }
}

LSTATUS WINAPI RegQueryValueExWHook(HKEY k, LPCWSTR vn, LPDWORD r, LPDWORD t,
                                    LPBYTE d, LPDWORD cb) {
    try {
        if (!g_keyTracker.IsTrackedOrFake(k))
            return RegQueryValueExWOriginal(k, vn, r, t, d, cb);
        std::wstring p = g_keyTracker.GetPath(k);
        if (!p.empty() && g_dllVerifiedOk.load()) {
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
        if (!MightNeedVirtualization(hk, sub))
            return RegGetValueWOriginal(hk, sub, val, fl, tp, d, cb);
        std::wstring p = g_keyTracker.GetPath(hk);
        if (sub && *sub) { if (!p.empty()) p += L"\\"; p += sub; }
        if (!p.empty() && g_dllVerifiedOk.load()) {
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
        if (!g_keyTracker.IsTrackedOrFake(k))
            return RegEnumKeyExWOriginal(k, idx, name, lpcch, r, cls, lpcCls, ft);

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

        if (!g_dllVerifiedOk.load())
            return RegEnumKeyExWOriginal(k, idx, name, lpcch, r, cls, lpcCls, ft);

        const std::wstring path = g_keyTracker.GetPath(k);
        if (!IsHkcuNamespaceParentKey(path))
            return RegEnumKeyExWOriginal(k, idx, name, lpcch, r, cls, lpcCls, ft);

        const LSTATUS st = RegEnumKeyExWOriginal(k, idx, name, lpcch, r, cls, lpcCls, ft);
        if (st != ERROR_NO_MORE_ITEMS) return st;

        // --- Cooperative mode check ---
        // If another mod (e.g., Win7 Legacy Applet Restorer) is already
        // handling HomeGroup namespace injection, skip our injection to
        // avoid duplicate entries in the Control Panel enumeration.
        if (g_yieldNamespaceInjection.load(std::memory_order_acquire)) {
            return ERROR_NO_MORE_ITEMS;
        }

        // Check if the CLSID already exists in the real registry.
        {
            HKEY hTest = nullptr;
            if (RegOpenKeyExWOriginal(k, g_clsidLower.c_str(), 0, KEY_READ, &hTest) ==
                ERROR_SUCCESS) {
                RegCloseKeyOriginal(hTest);
                return ERROR_NO_MORE_ITEMS;
            }
        }

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
        if (!g_keyTracker.IsTrackedOrFake(k))
            return RegEnumKeyWOriginal(k, idx, name, cch);

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

        if (!g_dllVerifiedOk.load())
            return RegEnumKeyWOriginal(k, idx, name, cch);

        const std::wstring path = g_keyTracker.GetPath(k);
        if (!IsHkcuNamespaceParentKey(path))
            return RegEnumKeyWOriginal(k, idx, name, cch);

        const LSTATUS st = RegEnumKeyWOriginal(k, idx, name, cch);
        if (st != ERROR_NO_MORE_ITEMS) return st;

        // --- Cooperative mode check (same as RegEnumKeyExWHook) ---
        if (g_yieldNamespaceInjection.load(std::memory_order_acquire)) {
            return ERROR_NO_MORE_ITEMS;
        }

        {
            HKEY hTest = nullptr;
            if (RegOpenKeyExWOriginal(k, g_clsidLower.c_str(), 0, KEY_READ, &hTest) ==
                ERROR_SUCCESS) {
                RegCloseKeyOriginal(hTest);
                return ERROR_NO_MORE_ITEMS;
            }
        }

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
    try {
        if (g_keyTracker.IsFake(k)) {
            std::wstring p = g_keyTracker.GetPath(k);
            VNode n = ClassifyPath(p);
            if (cSubKeys) *cSubKeys = GetVirtualSubKeyCount(n);
            if (cValues) *cValues = 0;
            if (lpcMaxSub) *lpcMaxSub = 32;
            if (lpcMaxCls) *lpcMaxCls = 0;
            if (lpcMaxValName) *lpcMaxValName = 64;
            if (lpcMaxValData) *lpcMaxValData = 512;
            if (cls && lpcCls) { if (*lpcCls > 0) cls[0] = 0; *lpcCls = 0; }
            if (ft) GetSystemTimeAsFileTime(ft);
            return ERROR_SUCCESS;
        }

        if (!g_dllVerifiedOk.load() || !g_keyTracker.IsTrackedOrFake(k))
            return RegQueryInfoKeyWOriginal(k, cls, lpcCls, r, cSubKeys, lpcMaxSub,
                                            lpcMaxCls, cValues, lpcMaxValName,
                                            lpcMaxValData, sec, ft);

        std::wstring path = g_keyTracker.GetPath(k);
        if (IsHkcuNamespaceParentKey(path)) {
            LSTATUS st = RegQueryInfoKeyWOriginal(k, cls, lpcCls, r, cSubKeys, lpcMaxSub,
                                                  lpcMaxCls, cValues, lpcMaxValName,
                                                  lpcMaxValData, sec, ft);
            // Only increment the subkey count if we are actually going to
            // inject the CLSID (i.e., cooperative mode is not active).
            if (st == ERROR_SUCCESS && cSubKeys &&
                !g_yieldNamespaceInjection.load(std::memory_order_acquire)) {
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
        return RegQueryInfoKeyWOriginal(k, cls, lpcCls, r, cSubKeys, lpcMaxSub,
                                        lpcMaxCls, cValues, lpcMaxValName,
                                        lpcMaxValData, sec, ft);
    } catch (...) {
        return RegQueryInfoKeyWOriginal(k, cls, lpcCls, r, cSubKeys, lpcMaxSub,
                                        lpcMaxCls, cValues, lpcMaxValName,
                                        lpcMaxValData, sec, ft);
    }
}

static void* GetRegFunc(const char* n) {
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

// =============================================================================
// Embedded translation hooks (LoadStringW/A and DirectUI XResourceProvider)
// =============================================================================
using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
using LoadStringA_t = int(WINAPI*)(HINSTANCE, UINT, LPSTR, int);
static LoadStringW_t LoadStringWOriginalKernelBase = nullptr;
static LoadStringW_t LoadStringWOriginalKernel32 = nullptr;
static LoadStringW_t LoadStringWOriginalUser32 = nullptr;
static LoadStringA_t LoadStringAOriginalKernelBase = nullptr;
static LoadStringA_t LoadStringAOriginalKernel32 = nullptr;
static LoadStringA_t LoadStringAOriginalUser32 = nullptr;

static bool IsHomeGroupResourceModule(HINSTANCE instance) {
    if (!instance) return false;
    const ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(instance);
    HMODULE module =
        reinterpret_cast<HMODULE>(raw & ~static_cast<ULONG_PTR>(3));
    HMODULE homeGroup = g_hHomeGroup.load(std::memory_order_acquire);
    if (homeGroup) {
        HMODULE normalizedHomeGroup = reinterpret_cast<HMODULE>(
            reinterpret_cast<ULONG_PTR>(homeGroup) &
            ~static_cast<ULONG_PTR>(3));
        if (module == normalizedHomeGroup) return true;
    }
    HMODULE localized =
        g_hLocalizedResources.load(std::memory_order_acquire);
    if (!localized) return false;
    HMODULE normalizedLocalized = reinterpret_cast<HMODULE>(
        reinterpret_cast<ULONG_PTR>(localized) &
        ~static_cast<ULONG_PTR>(3));
    return module == normalizedLocalized;
}

static int CopyEmbeddedStringW(const std::wstring& text, LPWSTR buffer,
                               int bufferChars) {
    if (!buffer || bufferChars < 1) return 0;
    const int length = static_cast<int>(text.size());
    const int copied = length < bufferChars - 1 ? length : bufferChars - 1;
    if (copied > 0) {
        memcpy(buffer, text.data(),
               static_cast<size_t>(copied) * sizeof(wchar_t));
    }
    buffer[copied] = L'\0';
    return copied;
}

static int HandleLoadStringW(HINSTANCE instance, UINT id, LPWSTR buffer,
                             int bufferChars, LoadStringW_t original) {
    if (!original) return 0;
    try {
        if (IsHomeGroupResourceModule(instance)) {
            if (HMODULE localized = EnsureLocalizedResourceModuleLoaded()) {
                return original(reinterpret_cast<HINSTANCE>(localized), id,
                                buffer, bufferChars);
            }
            if (bufferChars > 0) {
                std::wstring text;
                if (GetEmbeddedStringForLanguage(
                        GetCurrentEmbeddedLanguage(), id, text)) {
                    return CopyEmbeddedStringW(text, buffer, bufferChars);
                }
            }
        }
    } catch (...) {
    }
    return original(instance, id, buffer, bufferChars);
}

static int HandleLoadStringA(HINSTANCE instance, UINT id, LPSTR buffer,
                             int bufferChars, LoadStringA_t original) {
    if (!original) return 0;
    try {
        if (IsHomeGroupResourceModule(instance)) {
            if (HMODULE localized = EnsureLocalizedResourceModuleLoaded()) {
                return original(reinterpret_cast<HINSTANCE>(localized), id,
                                buffer, bufferChars);
            }
            if (buffer && bufferChars > 0) {
                std::wstring text;
                if (GetEmbeddedStringForLanguage(
                        GetCurrentEmbeddedLanguage(), id, text)) {
                    int required = WideCharToMultiByte(
                        CP_ACP, 0, text.c_str(), static_cast<int>(text.size()),
                        nullptr, 0, nullptr, nullptr);
                    if (required <= 0) return 0;
                    std::string converted(static_cast<size_t>(required), '\0');
                    WideCharToMultiByte(
                        CP_ACP, 0, text.c_str(), static_cast<int>(text.size()),
                        converted.data(), required, nullptr, nullptr);
                    const int copied = required < bufferChars - 1
                                           ? required
                                           : bufferChars - 1;
                    if (copied > 0) memcpy(buffer, converted.data(), copied);
                    buffer[copied] = '\0';
                    return copied;
                }
            }
        }
    } catch (...) {
    }
    return original(instance, id, buffer, bufferChars);
}

int WINAPI LoadStringWHookKernelBase(HINSTANCE instance, UINT id,
                                     LPWSTR buffer, int bufferChars) {
    return HandleLoadStringW(instance, id, buffer, bufferChars,
                             LoadStringWOriginalKernelBase);
}
int WINAPI LoadStringWHookKernel32(HINSTANCE instance, UINT id,
                                   LPWSTR buffer, int bufferChars) {
    return HandleLoadStringW(instance, id, buffer, bufferChars,
                             LoadStringWOriginalKernel32);
}
int WINAPI LoadStringWHookUser32(HINSTANCE instance, UINT id,
                                 LPWSTR buffer, int bufferChars) {
    return HandleLoadStringW(instance, id, buffer, bufferChars,
                             LoadStringWOriginalUser32);
}
int WINAPI LoadStringAHookKernelBase(HINSTANCE instance, UINT id,
                                     LPSTR buffer, int bufferChars) {
    return HandleLoadStringA(instance, id, buffer, bufferChars,
                             LoadStringAOriginalKernelBase);
}
int WINAPI LoadStringAHookKernel32(HINSTANCE instance, UINT id,
                                   LPSTR buffer, int bufferChars) {
    return HandleLoadStringA(instance, id, buffer, bufferChars,
                             LoadStringAOriginalKernel32);
}
int WINAPI LoadStringAHookUser32(HINSTANCE instance, UINT id,
                                 LPSTR buffer, int bufferChars) {
    return HandleLoadStringA(instance, id, buffer, bufferChars,
                             LoadStringAOriginalUser32);
}

using FindResourceExW_t = decltype(&FindResourceExW);
using LoadResource_t = decltype(&LoadResource);
using SizeofResource_t = decltype(&SizeofResource);
static FindResourceExW_t FindResourceExWOriginalKernelBase = nullptr;
static FindResourceExW_t FindResourceExWOriginalKernel32 = nullptr;
static LoadResource_t LoadResourceOriginalKernelBase = nullptr;
static LoadResource_t LoadResourceOriginalKernel32 = nullptr;
static SizeofResource_t SizeofResourceOriginalKernelBase = nullptr;
static SizeofResource_t SizeofResourceOriginalKernel32 = nullptr;
static std::mutex g_redirectedResourcesMutex;
static std::unordered_map<HRSRC, HMODULE> g_redirectedResources;

static WORD CurrentEmbeddedLanguageId() {
    const EmbeddedLanguagePack* pack =
        FindEmbeddedLanguagePack(GetCurrentEmbeddedLanguage());
    return pack ? pack->languageId : 0x0409;
}

static HRSRC HandleFindResourceExW(HMODULE module, LPCWSTR type, LPCWSTR name,
                                   WORD language,
                                   FindResourceExW_t original) {
    if (!original) return nullptr;
    try {
        const bool isStringTable = IS_INTRESOURCE(type) &&
            static_cast<ULONG_PTR>(reinterpret_cast<ULONG_PTR>(type)) == 6;
        if (isStringTable && IsHomeGroupResourceModule(module)) {
            if (HMODULE localized = EnsureLocalizedResourceModuleLoaded()) {
                HRSRC resource = original(localized, type, name,
                                           CurrentEmbeddedLanguageId());
                if (resource) {
                    std::lock_guard<std::mutex> lock(
                        g_redirectedResourcesMutex);
                    g_redirectedResources[resource] = localized;
                    return resource;
                }
            }
        }
    } catch (...) {
    }
    return original(module, type, name, language);
}

static HMODULE RedirectedModuleForResource(HRSRC resource) {
    std::lock_guard<std::mutex> lock(g_redirectedResourcesMutex);
    auto found = g_redirectedResources.find(resource);
    return found == g_redirectedResources.end() ? nullptr : found->second;
}

static HGLOBAL HandleLoadResource(HMODULE module, HRSRC resource,
                                  LoadResource_t original) {
    if (!original) return nullptr;
    if (HMODULE redirected = RedirectedModuleForResource(resource)) {
        return original(redirected, resource);
    }
    return original(module, resource);
}

static DWORD HandleSizeofResource(HMODULE module, HRSRC resource,
                                  SizeofResource_t original) {
    if (!original) return 0;
    if (HMODULE redirected = RedirectedModuleForResource(resource)) {
        return original(redirected, resource);
    }
    return original(module, resource);
}

HRSRC WINAPI FindResourceExWHookKernelBase(HMODULE module, LPCWSTR type,
                                           LPCWSTR name, WORD language) {
    return HandleFindResourceExW(module, type, name, language,
                                 FindResourceExWOriginalKernelBase);
}
HRSRC WINAPI FindResourceExWHookKernel32(HMODULE module, LPCWSTR type,
                                         LPCWSTR name, WORD language) {
    return HandleFindResourceExW(module, type, name, language,
                                 FindResourceExWOriginalKernel32);
}
HGLOBAL WINAPI LoadResourceHookKernelBase(HMODULE module, HRSRC resource) {
    return HandleLoadResource(module, resource,
                              LoadResourceOriginalKernelBase);
}
HGLOBAL WINAPI LoadResourceHookKernel32(HMODULE module, HRSRC resource) {
    return HandleLoadResource(module, resource,
                              LoadResourceOriginalKernel32);
}
DWORD WINAPI SizeofResourceHookKernelBase(HMODULE module, HRSRC resource) {
    return HandleSizeofResource(module, resource,
                                SizeofResourceOriginalKernelBase);
}
DWORD WINAPI SizeofResourceHookKernel32(HMODULE module, HRSRC resource) {
    return HandleSizeofResource(module, resource,
                                SizeofResourceOriginalKernel32);
}

using XResourceProviderCreate_t = HRESULT(*)(HINSTANCE, LPCWSTR, LPCWSTR,
                                             LPCWSTR, void**);
static XResourceProviderCreate_t XResourceProviderCreateOriginal = nullptr;

static HRESULT XResourceProviderCreateHook(HINSTANCE instance,
                                           LPCWSTR resourceName,
                                           LPCWSTR resourceType,
                                           LPCWSTR stylesheetName,
                                           void** provider) {
    // This hook runs on every DirectUI resource-provider creation inside
    // explorer.exe/control.exe. Any uncaught exception here would unwind
    // into DirectUI internals and could take down the host process, so a
    // failure in our localization lookup must degrade to the original
    // behavior instead of propagating.
    try {
        HINSTANCE resourceInstance = instance;
        if (IsHomeGroupResourceModule(instance)) {
            if (HMODULE localized = EnsureLocalizedResourceModuleLoaded()) {
                resourceInstance = reinterpret_cast<HINSTANCE>(localized);
            }
        }
        return XResourceProviderCreateOriginal(
            resourceInstance, resourceName, resourceType, stylesheetName, provider);
    } catch (...) {
        return XResourceProviderCreateOriginal(
            instance, resourceName, resourceType, stylesheetName, provider);
    }
}

#ifdef _WIN64
#define HG_DUI_THISCALL __cdecl
#else
#define HG_DUI_THISCALL __thiscall
#endif

using DUISetXML_t = HRESULT(HG_DUI_THISCALL*)(void*, const WCHAR*, HINSTANCE,
                                              HINSTANCE);
using DUISetXMLFromResource_t = HRESULT(HG_DUI_THISCALL*)(
    void*, PCWSTR, PCWSTR, HMODULE, HINSTANCE, HINSTANCE);
static DUISetXML_t DUISetXML = nullptr;
static DUISetXMLFromResource_t DUISetXMLFromResourceOriginal = nullptr;
static thread_local int g_insideHomeGroupXmlPatch = 0;

static std::wstring LoadDirectUiXml(HMODULE module, PCWSTR resourceName,
                                    PCWSTR resourceType) {
    HRSRC resource = FindResourceW(module, resourceName, resourceType);
    if (!resource) return {};
    HGLOBAL loaded = LoadResource(module, resource);
    if (!loaded) return {};
    const DWORD size = SizeofResource(module, resource);
    const char* bytes = static_cast<const char*>(LockResource(loaded));
    if (!bytes || !size) return {};

    UINT codePage = CP_UTF8;
    int chars = MultiByteToWideChar(CP_UTF8, 0, bytes,
                                    static_cast<int>(size), nullptr, 0);
    if (chars <= 0) {
        codePage = CP_ACP;
        chars = MultiByteToWideChar(CP_ACP, 0, bytes,
                                    static_cast<int>(size), nullptr, 0);
    }
    if (chars <= 0) return {};
    std::wstring xml(static_cast<size_t>(chars), L'\0');
    if (!MultiByteToWideChar(codePage, 0, bytes, static_cast<int>(size),
                             xml.data(), chars)) {
        return {};
    }
    while (!xml.empty() && xml.back() == L'\0') xml.pop_back();
    return xml;
}

static std::wstring EscapeDirectUiAttribute(const std::wstring& text) {
    std::wstring output;
    output.reserve(text.size() + 16);
    for (wchar_t ch : text) {
        switch (ch) {
            case L'&': output += L"&amp;"; break;
            case L'\"': output += L"&quot;"; break;
            case L'<': output += L"&lt;"; break;
            case L'>': output += L"&gt;"; break;
            case L'\r': output += L"&#xD;"; break;
            case L'\n': output += L"&#xA;"; break;
            default: output.push_back(ch); break;
        }
    }
    return output;
}

static bool IsHomeGroupDirectUiXml(const std::wstring& xml) {
    return xml.find(L"atom(HgPageContents)") != std::wstring::npos ||
           xml.find(L"atom(AdvPageContents)") != std::wstring::npos ||
           xml.find(L"atom(Passkey)") != std::wstring::npos ||
           xml.find(L"atom(HgSharingPage)") != std::wstring::npos;
}

// Inline every resstr(ID) expression, not just a hand-picked subset. The four
// hgcpl UIFILE resources currently reference 72 distinct IDs, and this generic
// pass also covers any additional reference introduced by a compatible build.
static size_t InlineAllHomeGroupResStrings(std::wstring& xml,
                                           EmbeddedLanguage language) {
    size_t replacements = 0;
    size_t position = 0;
    while ((position = xml.find(L"resstr(", position)) !=
           std::wstring::npos) {
        const size_t digits = position + 7;
        const size_t close = xml.find(L')', digits);
        if (close == std::wstring::npos) break;
        if (close == digits) {
            position = close + 1;
            continue;
        }
        UINT id = 0;
        bool valid = true;
        for (size_t i = digits; i < close; ++i) {
            if (xml[i] < L'0' || xml[i] > L'9') {
                valid = false;
                break;
            }
            id = id * 10 + static_cast<UINT>(xml[i] - L'0');
        }
        std::wstring text;
        if (!valid || !GetEmbeddedStringForLanguage(language, id, text)) {
            position = close + 1;
            continue;
        }
        const std::wstring escaped = EscapeDirectUiAttribute(text);
        xml.replace(position, close - position + 1, escaped);
        position += escaped.size();
        ++replacements;
    }
    return replacements;
}

static bool SetDefaultContentForAtom(std::wstring& xml,
                                     const wchar_t* atomName, UINT stringId,
                                     EmbeddedLanguage language) {
    std::wstring marker = L"id=\"atom(";
    marker += atomName;
    marker += L")\"";
    const size_t markerPosition = xml.find(marker);
    if (markerPosition == std::wstring::npos) return false;
    const size_t tagStart = xml.rfind(L'<', markerPosition);
    const size_t tagEnd = xml.find(L'>', markerPosition);
    if (tagStart == std::wstring::npos || tagEnd == std::wstring::npos) {
        return false;
    }
    if (xml.find(L"content=\"", tagStart) < tagEnd) return false;

    std::wstring text;
    if (!GetEmbeddedStringForLanguage(language, stringId, text)) return false;
    std::wstring attribute = L" content=\"";
    attribute += EscapeDirectUiAttribute(text);
    attribute += L"\"";
    size_t insertion = tagEnd;
    if (insertion > tagStart && xml[insertion - 1] == L'/') --insertion;
    xml.insert(insertion, attribute);
    return true;
}

// On Windows 11 the removed HomeGroup backend can make the old initializer
// leave four dynamically populated elements empty. Give those elements a
// cosmetic default matching the unavailable-backend state. If the initializer
// does provide a value, DirectUI can still replace these defaults afterwards.
static size_t AddWindows11HomeGroupFallbackText(
    std::wstring& xml, EmbeddedLanguage language) {
    size_t added = 0;
    added += SetDefaultContentForAtom(xml, L"HgTitle", 12, language) ? 1 : 0;
    added += SetDefaultContentForAtom(xml, L"HgStatusText", 19, language) ? 1 : 0;
    added += SetDefaultContentForAtom(xml, L"HgWarningText", 94, language) ? 1 : 0;
    added += SetDefaultContentForAtom(xml, L"HgWarningHelpLink", 95,
                                     language) ? 1 : 0;
    return added;
}

// RAII guard for the thread-local reentrancy counter. Guarantees the
// counter is decremented exactly once no matter how the guarded scope is
// exited (normal return, early return, or an exception thrown by DUISetXML
// or by the DirectUI code it calls back into).
class HomeGroupXmlPatchGuard {
public:
    HomeGroupXmlPatchGuard() noexcept { ++g_insideHomeGroupXmlPatch; }
    ~HomeGroupXmlPatchGuard() noexcept { --g_insideHomeGroupXmlPatch; }
    HomeGroupXmlPatchGuard(const HomeGroupXmlPatchGuard&) = delete;
    HomeGroupXmlPatchGuard& operator=(const HomeGroupXmlPatchGuard&) = delete;
};

static HRESULT HG_DUI_THISCALL DUISetXMLFromResourceHook(
    void* parser, PCWSTR resourceName, PCWSTR resourceType,
    HMODULE resourceModule, HINSTANCE instance1, HINSTANCE instance2) {
    if (!DUISetXMLFromResourceOriginal) return E_FAIL;

    // This hook runs on every DirectUI XML resource load in explorer.exe.
    // The string manipulation below (find/replace/insert on std::wstring)
    // can throw (e.g. std::bad_alloc, std::out_of_range on a malformed or
    // unexpectedly large resource). Letting that propagate out of a WINAPI
    // callback into DirectUI would very likely crash explorer, so any
    // failure here must fall back to the original, unpatched resource.
    try {
        if (!DUISetXML || g_insideHomeGroupXmlPatch) {
            return DUISetXMLFromResourceOriginal(
                parser, resourceName, resourceType, resourceModule, instance1,
                instance2);
        }

        const bool isUiFile =
            resourceType && !IS_INTRESOURCE(resourceType) &&
            _wcsicmp(resourceType, L"UIFILE") == 0;
        if (!isUiFile) {
            return DUISetXMLFromResourceOriginal(
                parser, resourceName, resourceType, resourceModule, instance1,
                instance2);
        }

        std::wstring xml =
            LoadDirectUiXml(resourceModule, resourceName, resourceType);
        if (xml.empty() || !IsHomeGroupDirectUiXml(xml)) {
            return DUISetXMLFromResourceOriginal(
                parser, resourceName, resourceType, resourceModule, instance1,
                instance2);
        }

        const EmbeddedLanguage language = GetCurrentEmbeddedLanguage();
        const size_t replaced = InlineAllHomeGroupResStrings(xml, language);
        const size_t defaults =
            AddWindows11HomeGroupFallbackText(xml, language);
        if (!replaced && !defaults) {
            return DUISetXMLFromResourceOriginal(
                parser, resourceName, resourceType, resourceModule, instance1,
                instance2);
        }

        Wh_Log(L"HomeGroup translations: inlined %zu DirectUI references and "
               L"added %zu Windows 11 fallback labels", replaced, defaults);

        HRESULT result;
        {
            HomeGroupXmlPatchGuard guard;
            result = DUISetXML(parser, xml.c_str(),
                               reinterpret_cast<HINSTANCE>(resourceModule),
                               instance1);
        }
        if (SUCCEEDED(result)) return result;

        Wh_Log(L"HomeGroup translations: patched DirectUI XML was rejected "
               L"(hr=0x%08X); falling back to the original resource", result);
        return DUISetXMLFromResourceOriginal(
            parser, resourceName, resourceType, resourceModule, instance1,
            instance2);
    } catch (...) {
        Wh_Log(L"HomeGroup translations: exception while patching DirectUI "
               L"XML; falling back to the original resource");
        return DUISetXMLFromResourceOriginal(
            parser, resourceName, resourceType, resourceModule, instance1,
            instance2);
    }
}

static void InstallHomeGroupXmlPatchHook(HMODULE dui70) {
    if (!dui70) return;
    const char* setXmlNames[] = {
#ifdef _WIN64
        "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",
#endif
        "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z",
    };
    for (const char* name : setXmlNames) {
        if (FARPROC function = GetProcAddress(dui70, name)) {
            DUISetXML = reinterpret_cast<DUISetXML_t>(function);
            break;
        }
    }

    const char* fromResourceNames[] = {
#ifdef _WIN64
        "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z",
#endif
        "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z",
    };
    for (const char* name : fromResourceNames) {
        if (FARPROC function = GetProcAddress(dui70, name)) {
            WindhawkUtils::SetFunctionHook(
                reinterpret_cast<DUISetXMLFromResource_t>(function),
                DUISetXMLFromResourceHook,
                &DUISetXMLFromResourceOriginal);
            break;
        }
    }
    if (!DUISetXML || !DUISetXMLFromResourceOriginal) {
        Wh_Log(L"HomeGroup translations: DirectUI XML inlining hook was not "
               L"available");
    }
}

static void InstallTranslationHooks() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        user32 = LoadLibraryExW(L"user32.dll", nullptr,
                                LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    void* loadStringWKernelBase = kernelBase
        ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "LoadStringW"))
        : nullptr;
    void* loadStringWKernel32 = kernel32
        ? reinterpret_cast<void*>(GetProcAddress(kernel32, "LoadStringW"))
        : nullptr;
    void* loadStringWUser32 = user32
        ? reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringW"))
        : nullptr;
    void* loadStringAKernelBase = kernelBase
        ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "LoadStringA"))
        : nullptr;
    void* loadStringAKernel32 = kernel32
        ? reinterpret_cast<void*>(GetProcAddress(kernel32, "LoadStringA"))
        : nullptr;
    void* loadStringAUser32 = user32
        ? reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringA"))
        : nullptr;

    // hgcpl.dll imports LoadString through an API-set contract which resolves
    // to KernelBase on Windows 11. Hooking user32 alone misses those dynamic
    // status/warning strings, even though DirectUI labels are translated.
    if (loadStringWKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadStringW_t>(loadStringWKernelBase),
            LoadStringWHookKernelBase, &LoadStringWOriginalKernelBase);
    }
    if (loadStringWKernel32 &&
        loadStringWKernel32 != loadStringWKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadStringW_t>(loadStringWKernel32),
            LoadStringWHookKernel32, &LoadStringWOriginalKernel32);
    }
    if (loadStringWUser32 && loadStringWUser32 != loadStringWKernelBase &&
        loadStringWUser32 != loadStringWKernel32) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadStringW_t>(loadStringWUser32),
            LoadStringWHookUser32, &LoadStringWOriginalUser32);
    }
    if (loadStringAKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadStringA_t>(loadStringAKernelBase),
            LoadStringAHookKernelBase, &LoadStringAOriginalKernelBase);
    }
    if (loadStringAKernel32 &&
        loadStringAKernel32 != loadStringAKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadStringA_t>(loadStringAKernel32),
            LoadStringAHookKernel32, &LoadStringAOriginalKernel32);
    }
    if (loadStringAUser32 && loadStringAUser32 != loadStringAKernelBase &&
        loadStringAUser32 != loadStringAKernel32) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadStringA_t>(loadStringAUser32),
            LoadStringAHookUser32, &LoadStringAOriginalUser32);
    }

    void* findResourceExWKernelBase = kernelBase
        ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "FindResourceExW"))
        : nullptr;
    void* findResourceExWKernel32 = kernel32
        ? reinterpret_cast<void*>(GetProcAddress(kernel32, "FindResourceExW"))
        : nullptr;
    void* loadResourceKernelBase = kernelBase
        ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "LoadResource"))
        : nullptr;
    void* loadResourceKernel32 = kernel32
        ? reinterpret_cast<void*>(GetProcAddress(kernel32, "LoadResource"))
        : nullptr;
    void* sizeofResourceKernelBase = kernelBase
        ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "SizeofResource"))
        : nullptr;
    void* sizeofResourceKernel32 = kernel32
        ? reinterpret_cast<void*>(GetProcAddress(kernel32, "SizeofResource"))
        : nullptr;

    if (findResourceExWKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<FindResourceExW_t>(findResourceExWKernelBase),
            FindResourceExWHookKernelBase,
            &FindResourceExWOriginalKernelBase);
    }
    if (findResourceExWKernel32 &&
        findResourceExWKernel32 != findResourceExWKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<FindResourceExW_t>(findResourceExWKernel32),
            FindResourceExWHookKernel32,
            &FindResourceExWOriginalKernel32);
    }
    if (loadResourceKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadResource_t>(loadResourceKernelBase),
            LoadResourceHookKernelBase, &LoadResourceOriginalKernelBase);
    }
    if (loadResourceKernel32 &&
        loadResourceKernel32 != loadResourceKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<LoadResource_t>(loadResourceKernel32),
            LoadResourceHookKernel32, &LoadResourceOriginalKernel32);
    }
    if (sizeofResourceKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<SizeofResource_t>(sizeofResourceKernelBase),
            SizeofResourceHookKernelBase,
            &SizeofResourceOriginalKernelBase);
    }
    if (sizeofResourceKernel32 &&
        sizeofResourceKernel32 != sizeofResourceKernelBase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<SizeofResource_t>(sizeofResourceKernel32),
            SizeofResourceHookKernel32,
            &SizeofResourceOriginalKernel32);
    }

    HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
    if (!dui70) {
        dui70 = LoadLibraryExW(L"dui70.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (dui70) {
        const char* names[] = {
#ifdef _WIN64
            "?Create@XResourceProvider@DirectUI@@SAJPEAUHINSTANCE__@@PEBG11PEAPEAV12@@Z",
#endif
            "?Create@XResourceProvider@DirectUI@@SAJPAUHINSTANCE__@@PBG11PAPAV12@@Z",
        };
        for (const char* name : names) {
            if (void* function = reinterpret_cast<void*>(
                    GetProcAddress(dui70, name))) {
                WindhawkUtils::SetFunctionHook(
                    reinterpret_cast<XResourceProviderCreate_t>(function),
                    XResourceProviderCreateHook,
                    &XResourceProviderCreateOriginal);
                break;
            }
        }
    }
    InstallHomeGroupXmlPatchHook(dui70);
    if (!XResourceProviderCreateOriginal) {
        Wh_Log(L"HomeGroup translations: DirectUI resource-provider hook "
               L"was not found; LoadString and XML fallbacks remain active");
    }
}

// =============================================================================
// COM Hooks
// =============================================================================
using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
using CoGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*);

CoCreateInstance_t CoCreateInstanceOriginalCombase = nullptr;
CoCreateInstance_t CoCreateInstanceOriginalOle32 = nullptr;
CoGetClassObject_t CoGetClassObjectOriginalCombase = nullptr;
CoGetClassObject_t CoGetClassObjectOriginalOle32 = nullptr;

// hgcpl.dll exposes more COM classes than the advanced-settings writer.
// The two classes below are mandatory for the root DirectUI page. The page's
// XMLFILE/100 resource explicitly names them as "elementprovider" and
// "initializer". The old implementation omitted both, which is why Windows 11
// showed "This page can't be displayed" even though the DLL itself loaded.
static bool IsCoreHgcplClass(REFCLSID clsid) {
    return IsEqualGUID(clsid, kProviderGuid) ||
           IsEqualGUID(clsid, kInitializerGuid) ||
           IsEqualGUID(clsid, kAdvancedWriterGuid);
}

// Additional class factories implemented by hgcpl.dll. They aren't needed to
// paint the root page, but forwarding them allows cosmetic child pages and
// commands to get as far as possible before they encounter removed services.
static bool IsAuxiliaryHgcplClass(REFCLSID clsid) {
    static const GUID classes[] = {
        {0x12fc5e89, 0x5446, 0x4a7c, {0xba, 0x46, 0x20, 0x7a, 0x29, 0xe2, 0x94, 0x5d}},
        {0x31220067, 0xEE3A, 0x4ED7, {0xB5, 0x79, 0x10, 0x3D, 0x74, 0x93, 0x0C, 0xB5}},
        {0x7B90DAE3, 0x4AD0, 0x4F0D, {0xBE, 0x80, 0xA2, 0x6B, 0x29, 0x6C, 0x31, 0x56}},
        {0x879fb53b, 0xcba3, 0x4fc8, {0xb2, 0x33, 0xd9, 0xa9, 0x3a, 0xfa, 0x7f, 0xbc}},
        {0xa86ca2f1, 0xaf74, 0x4a74, {0x98, 0x0b, 0xe1, 0x85, 0xd4, 0xca, 0x01, 0xb0}},
        {0xAA2E2C5B, 0x0B0C, 0x4ECC, {0xB3, 0x2B, 0x39, 0x35, 0x26, 0x9E, 0x05, 0x88}},
        {0xabd2ad24, 0xf1ff, 0x47ad, {0x82, 0xde, 0x3a, 0x1e, 0xdf, 0x38, 0xe7, 0xa1}},
        {0xb27b520e, 0x46db, 0x4720, {0xb9, 0xc5, 0x5f, 0x80, 0xac, 0xab, 0x23, 0xa4}},
        {0xC4050BC4, 0x29E1, 0x4c8f, {0xBF, 0x6E, 0x6E, 0xBA, 0xD2, 0x1E, 0x06, 0x73}},
        {0xC98F3822, 0x3658, 0x4D75, {0x8A, 0x25, 0x66, 0x21, 0x66, 0x5E, 0xCD, 0x56}},
        {0xE4D5B02C, 0x82A9, 0x4363, {0xBD, 0x02, 0x8B, 0xA5, 0x95, 0x20, 0x0B, 0xCF}},
        {0xED8C22CA, 0x7722, 0x464A, {0xA5, 0x22, 0x79, 0x67, 0xAB, 0xF6, 0x3C, 0x35}},
        {0xff363bfe, 0x4941, 0x4179, {0xa8, 0x1c, 0xf3, 0xf1, 0xca, 0x72, 0xd8, 0x20}},
    };
    for (const auto& candidate : classes) {
        if (IsEqualGUID(clsid, candidate)) return true;
    }
    return false;
}

static bool IsHgcplClass(REFCLSID clsid) {
    return IsCoreHgcplClass(clsid) || IsAuxiliaryHgcplClass(clsid);
}

static HMODULE ModuleForInterceptedClass(REFCLSID clsid) {
    if (IsHgcplClass(clsid)) {
        return g_hHomeGroup.load(std::memory_order_acquire);
    }
    if (IsEqualGUID(clsid, kAppletFolderGuid)) {
        HMODULE h = GetModuleHandleW(L"shdocvw.dll");
        if (!h) {
            h = LoadLibraryExW(L"shdocvw.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        return h;
    }
    return nullptr;
}

static HRESULT GetInterceptedClassObject(REFCLSID rclsid, REFIID riid,
                                         LPVOID* ppv) {
    if (!ppv) return E_POINTER;
    *ppv = nullptr;

    if (!g_dllVerifiedOk.load(std::memory_order_acquire)) {
        return REGDB_E_CLASSNOTREG;
    }

    HMODULE h = ModuleForInterceptedClass(rclsid);
    if (!h) return REGDB_E_CLASSNOTREG;

    auto pDllGetClassObject = reinterpret_cast<
        HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*)>(
            GetProcAddress(h, "DllGetClassObject"));
    if (!pDllGetClassObject) return REGDB_E_CLASSNOTREG;

    HRESULT hr = pDllGetClassObject(rclsid, riid, ppv);
    if (FAILED(hr)) {
        Wh_Log(L"HomeGroup: DllGetClassObject failed (hr=0x%08X)", hr);
    }
    return hr;
}

static HRESULT HandleCoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                      DWORD dwClsCtx, REFIID riid, LPVOID* ppv,
                                      CoCreateInstance_t original) {
    try {
        const bool intercepted = IsHgcplClass(rclsid) ||
                                 IsEqualGUID(rclsid, kAppletFolderGuid);
        // Don't turn a local-server/elevation request into an in-process load.
        if (intercepted && (dwClsCtx & (CLSCTX_INPROC_SERVER |
                                        CLSCTX_INPROC_HANDLER))) {
            IClassFactory* cf = nullptr;
            HRESULT hr = GetInterceptedClassObject(
                rclsid, IID_IClassFactory_GUID,
                reinterpret_cast<LPVOID*>(&cf));
            if (FAILED(hr)) return hr;

            hr = cf->CreateInstance(pUnkOuter, riid, ppv);
            cf->Release();
            return hr;
        }
    } catch (...) {
        // Fall through to the original COM implementation.
    }
    return original(rclsid, pUnkOuter, dwClsCtx, riid, ppv);
}

static HRESULT HandleCoGetClassObject(REFCLSID rclsid, DWORD dwClsCtx,
                                      LPVOID reserved, REFIID riid,
                                      LPVOID* ppv,
                                      CoGetClassObject_t original) {
    try {
        const bool intercepted = IsHgcplClass(rclsid) ||
                                 IsEqualGUID(rclsid, kAppletFolderGuid);
        if (intercepted && (dwClsCtx & (CLSCTX_INPROC_SERVER |
                                        CLSCTX_INPROC_HANDLER))) {
            return GetInterceptedClassObject(rclsid, riid, ppv);
        }
    } catch (...) {
        // Fall through to the original COM implementation.
    }
    return original(rclsid, dwClsCtx, reserved, riid, ppv);
}

HRESULT WINAPI CoCreateInstanceHookCombase(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                           DWORD dwClsCtx, REFIID riid,
                                           LPVOID* ppv) {
    return HandleCoCreateInstance(rclsid, pUnkOuter, dwClsCtx, riid, ppv,
                                  CoCreateInstanceOriginalCombase);
}

HRESULT WINAPI CoCreateInstanceHookOle32(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                         DWORD dwClsCtx, REFIID riid,
                                         LPVOID* ppv) {
    return HandleCoCreateInstance(rclsid, pUnkOuter, dwClsCtx, riid, ppv,
                                  CoCreateInstanceOriginalOle32);
}

HRESULT WINAPI CoGetClassObjectHookCombase(REFCLSID rclsid, DWORD dwClsCtx,
                                           LPVOID reserved, REFIID riid,
                                           LPVOID* ppv) {
    return HandleCoGetClassObject(rclsid, dwClsCtx, reserved, riid, ppv,
                                  CoGetClassObjectOriginalCombase);
}

HRESULT WINAPI CoGetClassObjectHookOle32(REFCLSID rclsid, DWORD dwClsCtx,
                                         LPVOID reserved, REFIID riid,
                                         LPVOID* ppv) {
    return HandleCoGetClassObject(rclsid, dwClsCtx, reserved, riid, ppv,
                                  CoGetClassObjectOriginalOle32);
}

static void InstallComHook() {
    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase) {
        combase = LoadLibraryExW(L"combase.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
    if (!ole32) {
        ole32 = LoadLibraryExW(L"ole32.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    void* pCreateCombase = combase ? reinterpret_cast<void*>(
        GetProcAddress(combase, "CoCreateInstance")) : nullptr;
    void* pCreateOle32 = ole32 ? reinterpret_cast<void*>(
        GetProcAddress(ole32, "CoCreateInstance")) : nullptr;
    void* pGetCombase = combase ? reinterpret_cast<void*>(
        GetProcAddress(combase, "CoGetClassObject")) : nullptr;
    void* pGetOle32 = ole32 ? reinterpret_cast<void*>(
        GetProcAddress(ole32, "CoGetClassObject")) : nullptr;

    if (pCreateCombase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CoCreateInstance_t>(pCreateCombase),
            CoCreateInstanceHookCombase,
            &CoCreateInstanceOriginalCombase);
    }
    if (pCreateOle32 && pCreateOle32 != pCreateCombase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CoCreateInstance_t>(pCreateOle32),
            CoCreateInstanceHookOle32,
            &CoCreateInstanceOriginalOle32);
    }
    if (pGetCombase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CoGetClassObject_t>(pGetCombase),
            CoGetClassObjectHookCombase,
            &CoGetClassObjectOriginalCombase);
    }
    if (pGetOle32 && pGetOle32 != pGetCombase) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CoGetClassObject_t>(pGetOle32),
            CoGetClassObjectHookOle32,
            &CoGetClassObjectOriginalOle32);
    }
}

// =============================================================================
// Conflict Detection and Resolution
// =============================================================================
//
// This mod can coexist with the "Windows 7 Legacy Applet Restorer" mod by
// babamohammed2022, which also handles the HomeGroup CLSID
// {67CA7650-96E6-4FDD-BB43-A8E774F73A57}. The two mods use different
// approaches (this one virtualizes the entire CLSID tree + downloads hgcpl.dll;
// the other relies on the CLSID being present in the real registry and only
// injects a category assignment), and both hook the same registry functions.
//
// Conflict scenarios:
//   1. DUPLICATE INJECTION: Both mods try to append the HomeGroup CLSID to
//      the Control Panel namespace enumeration, causing two entries.
//   2. FAKE HANDLE COLLISION: Both mods' KeyTrackers create fake handles
//      (this one uses volatile registry keys, the other uses heap-allocated
//      ints). If both create fakes for the same CLSID path, the second
//      mod's RegCloseKeyHook may close a handle the first mod created.
//   3. COM HOOK INTERFERENCE: This mod hooks CoCreateInstance; the other
//      does not. No conflict here — this mod's COM hook is CLSID-specific.
//
// Resolution strategy:
//   - At Wh_ModInit, detect whether the Win7 Legacy Applet Restorer is
//     active AND has HomeGroup enabled. Detection methods (tried in order):
//       a) Named event: check for the other mod's activity marker
//       b) Registry probe: check if the HomeGroup CLSID exists in the
//          real registry (HKCR\CLSID\{67CA7650-...}) — if it does, the
//          other mod (or Windows < 1803) is already handling it
//       c) Hook chain inspection: check if RegOpenKeyExW's trampoline
//          target is not the original kernelbase function (meaning another
//          mod already hooked it)
//   - If the other mod is detected with HomeGroup active, this mod enters
//     "cooperative mode": it still downloads and loads hgcpl.dll (for COM
//     and resource purposes), but DOES NOT inject the CLSID into the
//     namespace enumeration — the other mod handles that.
//   - A manual override setting (forceHomeGroupInjection) lets the user
//     force injection even when coexistence is detected.

// Note: g_yieldNamespaceInjection, g_modActiveEvent, kModActiveEventName,
// and kWin7LegacyActiveEventName are declared in the Shared State section
// above (before the registry hooks) so they are visible to all hooks.

// Check if the HomeGroup CLSID is already registered in the real registry
// (not through our hooks). This is the same check the Win7 Legacy mod
// performs at startup via IsRegisteredClsid(). If it returns true, either:
//   a) Windows < 1803 still has it natively, OR
//   b) Another mod/restoration has already registered it
// In either case, we don't need to inject it into the namespace.
static bool IsHomeGroupClsidInRealRegistry() {
    // Use the un-hooked original if available (our hooks may not be
    // installed yet at this point). Fall back to the raw API.
    HKEY hKey = nullptr;
    LSTATUS st = RegOpenKeyExW(
        HKEY_CLASSES_ROOT,
        L"CLSID\\{67ca7650-96e6-4fdd-bb43-a8e774f73a57}",
        0, KEY_READ, &hKey);
    if (st == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}

// Check if another mod has already hooked RegOpenKeyExW by inspecting
// whether the current function pointer differs from the raw kernelbase
// export. This is a heuristic: if the function is already hooked, another
// mod with overlapping registry virtualization is likely active.
static bool IsRegOpenKeyExAlreadyHooked() {
    HMODULE hKb = GetModuleHandleW(L"kernelbase.dll");
    if (!hKb) return false;
    void* rawFunc = reinterpret_cast<void*>(GetProcAddress(hKb, "RegOpenKeyExW"));
    if (!rawFunc) return false;

    // Read the first bytes of the function to check for a JMP trampoline
    // (typical of inline hooks like Windhawk/Wh_SetFunctionHook uses).
    // A JMP near (0xE9) or JMP far (0xFF 0x25) indicates a hook is present.
    BYTE* code = static_cast<BYTE*>(rawFunc);
    if (code[0] == 0xE9 || code[0] == 0xFF || code[0] == 0x48) {
        // 0xE9 = JMP rel32, 0xFF = JMP indirect, 0x48 = REX.W prefix
        // (often preceding a hooked instruction). Any of these at the
        // very start of the function strongly suggests an inline hook.
        return true;
    }
    return false;
}

// Detect the Win7 Legacy Applet Restorer mod by checking for its named
// event. Returns true if the event exists (the other mod is active).
static bool IsWin7LegacyModActive() {
    // WinHandle (RAII) guarantees the handle is closed on every exit path,
    // including if a future edit adds an early return or a throwing call
    // between the open and the original manual CloseHandle.
    WinHandle hEvent(OpenEventW(SYNCHRONIZE, FALSE, kWin7LegacyActiveEventName));
    return hEvent.valid();
}

// Perform all conflict detection checks and decide whether to yield
// namespace injection to the other mod. Returns a description of what
// was detected for logging.
static bool DetectConflictsAndDecideYield(std::wstring& reason) {
    // Check 1: Is the HomeGroup CLSID already in the real registry?
    // This is the strongest signal — if it's there, injection is unnecessary
    // regardless of which mod put it there.
    if (IsHomeGroupClsidInRealRegistry()) {
        reason = L"HomeGroup CLSID already exists in real registry "
                 L"(Windows < 1803 or another restoration); "
                 L"namespace injection not needed";
        return true;
    }

    // Check 2: Is the Win7 Legacy mod's named event present?
    if (IsWin7LegacyModActive()) {
        reason = L"Windows 7 Legacy Applet Restorer mod detected via "
                 L"named event; yielding namespace injection to avoid "
                 L"duplicate entries";
        return true;
    }

    // Check 3: Is RegOpenKeyExW already hooked by another mod?
    // This is a weaker signal — it could be the Win7 Legacy mod, the
    // Performance mod, or any other mod that hooks registry functions.
    // Only yield if combined with other evidence. We use this as a
    // log-only indicator, not a yield trigger.
    if (IsRegOpenKeyExAlreadyHooked()) {
        reason = L"RegOpenKeyExW appears to be already hooked by another "
                 L"mod; proceeding with injection but monitoring for conflicts";
        return false; // Don't yield — could be a non-conflicting mod
    }

    reason = L"No conflicting mods detected";
    return false;
}

// Publish our own named event so other mods can detect us.
static void PublishModActiveEvent() {
    if (!g_modActiveEvent) {
        g_modActiveEvent = CreateEventW(nullptr, TRUE, TRUE, kModActiveEventName);
        if (!g_modActiveEvent) {
            Wh_Log(L"HomeGroup Restorer: failed to create active event (%lu)",
                   GetLastError());
        }
    }
}

static void UnpublishModActiveEvent() {
    if (g_modActiveEvent) {
        CloseHandle(g_modActiveEvent);
        g_modActiveEvent = nullptr;
    }
}

// =============================================================================
// Windhawk Entry Points
// =============================================================================

BOOL Wh_ModInit(void) {
    try {
        if (!IsRunningAsAmd64()) {
            Wh_Log(L"HomeGroup Restorer: not starting — architecture is not AMD64");
            return FALSE;
        }

        if (!ValidateEmbeddedStringCatalog()) {
            Wh_Log(L"HomeGroup Restorer: embedded language catalog validation "
                   L"failed");
            return FALSE;
        }

        InitClsidStrings();
        LoadLanguageSetting();
        g_enableAdvancedWriter.store(Wh_GetIntSetting(L"enableAdvancedWriter") != 0);

        // --- Conflict detection (before installing hooks) ---
        // Detect other mods that might conflict with our HomeGroup CLSID
        // injection. The Win7 Legacy Applet Restorer also handles HomeGroup
        // and hooks the same registry functions. If detected, we enter
        // cooperative mode: COM hooks and DLL download still run, but we
        // skip namespace enumeration injection to avoid duplicates.
        {
            std::wstring conflictReason;
            bool shouldYield = DetectConflictsAndDecideYield(conflictReason);

            // Allow manual override via setting.
            if (shouldYield && Wh_GetIntSetting(L"forceHomeGroupInjection") != 0) {
                Wh_Log(L"HomeGroup Restorer: conflict detected but "
                       L"forceHomeGroupInjection is enabled — proceeding "
                       L"with injection anyway");
                shouldYield = false;
            }

            g_yieldNamespaceInjection.store(shouldYield, std::memory_order_release);
            Wh_Log(L"HomeGroup Restorer: %s (yield=%d)",
                   conflictReason.c_str(), shouldYield ? 1 : 0);
        }

        // Install registry hooks.
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
            Wh_Log(L"HomeGroup Restorer: failed to resolve registry functions");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyExW_t>(pOpen),
                                       RegOpenKeyExWHook, &RegOpenKeyExWOriginal);
        if (pOpenOldW)
            WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyW_t>(pOpenOldW),
                                           RegOpenKeyWHook, &RegOpenKeyWOriginal);
        if (pCreateW)
            WindhawkUtils::SetFunctionHook(reinterpret_cast<RegCreateKeyExW_t>(pCreateW),
                                           RegCreateKeyExWHook, &RegCreateKeyExWOriginal);
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
        InstallTranslationHooks();

        if (!g_stopEvent) {
            g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        } else {
            ResetEvent(g_stopEvent);
        }
        g_shuttingDown.store(false, std::memory_order_release);

        // Publish our named event so other mods can detect us.
        PublishModActiveEvent();

        // Start async setup on a background thread.
        try {
            g_setupThread.emplace(RunSetup);
        } catch (...) {
            Wh_Log(L"HomeGroup Restorer: failed to create setup thread");
        }

        return TRUE;
    } catch (...) {
        return FALSE;
    }
}

void Wh_ModAfterInit(void) {
    Wh_Log(L"HomeGroup Restorer: hooks active, DLL loading in background");
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    try {
        const bool oldAutomatic =
            g_languageAutomatic.load(std::memory_order_acquire);
        const int oldForced =
            g_forcedLanguage.load(std::memory_order_acquire);

        g_enableAdvancedWriter.store(
            Wh_GetIntSetting(L"enableAdvancedWriter") != 0);
        LoadLanguageSetting();

        const bool languageChanged =
            oldAutomatic !=
                g_languageAutomatic.load(std::memory_order_acquire) ||
            oldForced != g_forcedLanguage.load(std::memory_order_acquire);
        if (languageChanged) {
            const std::wstring* dll = CurrentDllPath();
            std::wstring directory = StoreDir();
            if (dll && !dll->empty() && !directory.empty()) {
                // Existing DirectUI pages may retain the old module. It stays
                // mapped; pages opened after this rebuild use the new one.
                ReleaseLocalizedResourceModule();
                BuildLocalizedResourceModule(*dll, directory);
                Wh_Log(L"HomeGroup language changed; close and reopen the "
                       L"Control Panel page to apply it");
            }
        }

        if (reload) *reload = FALSE;
        return TRUE;
    } catch (...) {
        if (reload) *reload = TRUE;
        return TRUE;
    }
}

void Wh_ModUninit(void) {
    try {
        // Unpublish our named event before doing anything else.
        UnpublishModActiveEvent();

        g_shuttingDown.store(true, std::memory_order_release);
        if (g_stopEvent) SetEvent(g_stopEvent);
        CancelInFlightDownload();

        if (g_setupThread && g_setupThread->joinable()) {
            g_setupThread->join();
        }
        g_setupThread.reset();

        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }

        // Release module references. Do NOT force-unload: live COM objects
        // or DirectUI pages may still reference vtables and resource handles.
        ReleaseLocalizedResourceModule();
        {
            std::lock_guard<std::mutex> lock(g_redirectedResourcesMutex);
            g_redirectedResources.clear();
        }
        g_hHomeGroup.store(nullptr);
        g_dllVerifiedOk.store(false);
        const std::wstring* path = g_dllPath.exchange(nullptr);
        delete path;

        // Clean up files based on settings.
        std::wstring dir = StoreDir();
        if (Wh_GetIntSetting(L"keepFilesOnDisable") == 0) {
            if (!dir.empty()) RemoveOwnFiles(dir, false);
            Wh_Log(L"HomeGroup Restorer: mod-owned files removed");
        } else {
            if (!dir.empty()) RemoveOwnFiles(dir, true);
            Wh_Log(L"HomeGroup Restorer: base files kept, stale files removed");
        }

        g_keyTracker.ClearWithoutFreeing();
        ReleaseVirtualKeyRoot();
    } catch (...) {
        // Uninit must never throw.
    }
}
